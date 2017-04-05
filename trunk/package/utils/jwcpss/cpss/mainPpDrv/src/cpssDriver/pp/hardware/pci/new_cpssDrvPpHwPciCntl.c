/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpHwPciCntl.c
*
* DESCRIPTION:
*       Prestera driver Hardware read and write functions implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>




typedef struct PRV_CPSS_BUS_DRIVER_PCI_STCT {
    CPSS_BUS_DRIVER_OBJ_STC common;
    GT_UINTPTR          baseAddr;
    GT_UINTPTR          internalPciBase;
    GT_UINTPTR          resetAndInitControllerBase;
    GT_U32              addrCompletShadow[4];
    CPSS_OS_MUTEX       hwComplSem;
    GT_U8               compIdx;
    PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo;
    GT_U8               ramBurstInfoLen;
} PRV_CPSS_BUS_DRIVER_PCI_STC;









/* Object that holds callback function to HW access */
extern CPSS_DRV_HW_ACCESS_OBJ_STC prvCpssHwAccessObj;

#define debugPrint(_x)\
    if(memoryAccessTraceOn == GT_TRUE)\
       cpssOsPrintf _x

#ifdef HW_DEBUG_TRACE
#define debugTrace(_x)      cpssOsPrintSync _x
#else
#define debugTrace(_x)
#endif  /* HW_DEBUG_TRACE */



/*******************************************************************************
* Intenal Macros and definitions
*******************************************************************************/
/* future changes possible */
#define BASEADDR(_drv) (_drv->baseAddr)

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Forward functions declarations.
*******************************************************************************/
static GT_STATUS hwWrite
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_U32      *dataPtr
);

static GT_STATUS hwCompletion
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN  GT_U32      regAddr,
    OUT GT_U8       *compIdxPtr,
    OUT GT_UINTPTR  *addressPtr
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  address;  /*physical access address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U8   i;      /* count iterator for the completion index compare loop*/
    GT_U32  data;   /* data to be write to memory */

    /* check if addrRegion is 0 */
    if ((regAddr & 0xFF000000) == 0)
    {
        compIdx = 0;
    }
    else
    {
        cpssOsMutexLock(drv->hwComplSem);

        addrRegion = (GT_U8)(regAddr >> 24);
        /* compare addr region to existing Address regions*/
        for (i = 3;(i > 1) &&
             (addrRegion != drv->addrCompletShadow[i]);
             i--);
        if(i == 1)
        {   /* Set addrRegion in AddrCompletion register */

            /*round robin on Region index : 2,3*/
            drv->compIdx++;
            if (drv->compIdx >= 4)
                drv->compIdx = 2;
            compIdx = drv->compIdx;

                      /*update Address Completion shadow*/
            drv->addrCompletShadow[compIdx] = addrRegion;

            /* update Hw Address Completion - using completion region 0 */
            address = BASEADDR(drv) + ADDR_COMP_REG;
            data = (drv->addrCompletShadow[1]<<8) |
                   (drv->addrCompletShadow[2]<<16) |
                   (drv->addrCompletShadow[3]<<24);

            data = CPSS_32BIT_LE(data);

            *(volatile GT_U32 *)(address) = data;
            /*write the address completion 3 times.
            because the PP have a 2 entry write buffer
            so, the 3 writes will make sure we do get
            to the hardware register itself */
            GT_SYNC;
            *(volatile GT_U32 *)(address) = data;
            GT_SYNC;
            *(volatile GT_U32 *)(address) = data;
            GT_SYNC;
        }
        else
        {
            compIdx = i;
        }
    }

    address = (BASEADDR(drv) + ((compIdx << 24) | (regAddr & 0x00ffffff)));
    *compIdxPtr = compIdx;
    *addressPtr = address;
    return GT_OK;
}

/*******************************************************************************
*  hwWrite
*
* DESCRIPTION:
*      This function receives address and data pointer and write the data to
*      the PP.
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address .
*       length  - Nubmber of writes in 4 byte each.
*       dataPtr - Pointer to data array.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       in case that the region (addr[31:24]) is 0 we preform the write sequence
*       without any mutual exclusion, this is mandatory due to the fact that
*       the ISR also uses this function.
*
*******************************************************************************/
static GT_STATUS hwWrite
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_U32      *dataPtr
)
{
    GT_UINTPTR  address;  /*physical address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U32  j = 0;      /* count iterator for the write loop*/
    GT_U32  nextRegionAddr; /* address of the next region after the one
                                currently used */
    GT_U32  loopLength = 0; /* when length exceeds region addr, Set to end of
                            region range */

    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    debugTrace(("Write: addr 0x%X len 0x%X data 0x%X \n", regAddr, length, *dataPtr));

    hwCompletion(drv, regAddr, &compIdx, &address);

    /* check whether completion region boundaries exceeded*/
    nextRegionAddr =  (GT_U32)(drv->addrCompletShadow[compIdx] + 1)<<24;
    if((GT_U32)(regAddr + length * 4 - 1) < nextRegionAddr)
    {

        for(j = 0; j < length; j++)
        {
            *((volatile GT_U32*)address) = CPSS_32BIT_LE(dataPtr[j]);

            /* need to put GT_SYNC to avoid from having the CPU doing
            * write combine. Some CPUs like advanced new PPC can do write combining
            * of two writes to continuous addresses. So, to avoid that we use the eioio
            * in PPC architecture;*/
            GT_SYNC;

            address += 4;
        }
        if (compIdx != 0)
            cpssOsMutexUnlock(drv->hwComplSem);
    }
    else
    {
        loopLength = (nextRegionAddr - regAddr) / 4;
        for(j = 0; j < loopLength; j++)
        {
            *((volatile GT_U32*)address) = CPSS_32BIT_LE(dataPtr[j]);
            /* need to put GT_SYNC to avoid from having the CPU doing
            * write combine. Some CPUs like advanced new PPC can do write combining
            * of two writes to continuous addresses. So, to avoid that we use the eioio
            * in PPC architecture; */
            GT_SYNC;

            address += 4;
        }
        if (compIdx != 0)
            cpssOsMutexUnlock(drv->hwComplSem);

        /* Recursive call for rest of data in next region.  */
        hwWrite(drv,nextRegionAddr,length-loopLength,
                dataPtr+loopLength);
    }   /* End of if complId !=0    */

    return GT_OK;
}




/*******************************************************************************
*  hwRead
*
* DESCRIPTION:
*      This function receives address and buffer pointer and reads the the PP
*      RAM into the buffer.
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address .
*       length  - Nubmber of writes in 4 byte each.
*       dataPtr - Pointer to data array.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       in case that the region (addr[31:24]) is 0 we preform the read sequence
*       without any mutual exclusion, this is mandatory due to the fact that
*       the ISR also uses this function.
*
*******************************************************************************/
static GT_STATUS hwRead
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    OUT GT_U32      *dataPtr
)
{
    GT_UINTPTR  address;  /*physical access address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U32  temp;
    GT_U32  j;      /* count iterator for the write loop*/
    GT_U32  nextRegionAddr; /* address of the next region after the one
                                currently used */
    GT_U32  loopLength = 0; /* when length exceeds region addr, Set to end of
                            region range */

    hwCompletion(drv, regAddr, &compIdx, &address);

    /* check whether completion region boundaries are exceeded*/
    nextRegionAddr = (GT_U32)(drv->addrCompletShadow[compIdx]+1)<<24;
    if ((GT_U32)(regAddr+length*4-1) < nextRegionAddr)
    {
        for(j = 0;j < length;j++)
        {
            temp = *( (volatile GT_U32 *) address);
            GT_SYNC; /*to avoid read combine */
            dataPtr[j] = CPSS_32BIT_LE(temp);
            address += 4;
        }
        if (compIdx != 0)
            cpssOsMutexUnlock(drv->hwComplSem);
    }
    else
    {
        loopLength =  (nextRegionAddr - regAddr)/4;
        for(j = 0;j < loopLength ;j++)
        {
            temp = *( (volatile GT_U32 *) address);
            GT_SYNC;/* to avoid read combine by some advanced CPUs */
            dataPtr[j] = CPSS_32BIT_LE(temp);
            address += 4;
        }
        if (compIdx != 0)
            cpssOsMutexUnlock(drv->hwComplSem);

        /* recursive call for rest of data in next region */
        hwRead(drv,nextRegionAddr,length-loopLength,
                dataPtr+loopLength);
    }

    debugTrace(("Read: addr 0x%X len 0x%X data 0x%X \n", regAddr, length, *dataPtr));


    return GT_OK;
}


/*******************************************************************************
* hwPpPciReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciReadRegister
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(drv->common.devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            GT_U32 dummy;

            hwRead (drv, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (drv, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    debugPrint(("Read from dev %d, reg 0x%x,",drv->common.devNum, regAddr));
    hwRead (drv,regAddr,1, data);
    debugPrint((" data 0x%x.\n", *data));
    return GT_OK;
}


/*******************************************************************************
* hwPpPciWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       drv             - Driver object ptr
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciWriteRegister
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(drv->common.devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            GT_U32 dummy;

            hwRead (drv, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (drv, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    debugPrint(("Write to device %d, reg 0x%x, data 0x%x.\n",drv->common.devNum,regAddr,
                value));

    hwWrite (drv,regAddr, 1,&value);

    return GT_OK;
}


/*******************************************************************************
* hwPpPciReadRam
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       drv     - Driver object ptr
*       addr    - Address offset to read from.
*       length  - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       data    - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciReadRam
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32  *data
)
{
    hwRead (drv, addr, length, data);
    return GT_OK;
}



/*******************************************************************************
* hwPpPciWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       drv     - Driver object ptr
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciWriteRam
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    hwWrite(drv, addr, length, data);
    return GT_OK;
}

/*******************************************************************************
* hwPpPciIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to read from.
*                 Note: regAddr should be < 0x1000000
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciIsrRead
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  addrData;    /* physical address for PCI transaction */
    GT_U8   compIdx;    /* address completion register field Isr index = 1*/
    GT_U32  temp;
    GT_UINTPTR addrCompletion;
    GT_U32  dataComletion;

    /* Set addrRegion in AddrCompletion register = 1 */
    compIdx = 1;

    /* msb_ = region value */
    addrRegion = (GT_U8)(regAddr >> 24);

    if (addrRegion == 0)
    {
        addrData = BASEADDR(drv) + regAddr;
    }
    else
    {
        addrCompletion = (BASEADDR(drv) + ADDR_COMP_REG);

        /* Update Address Completion shadow.            */
        drv->addrCompletShadow[compIdx] = addrRegion;

        /* Update Hw Address Completion Region 1- using region 0. */
        dataComletion =
                ((drv->addrCompletShadow[1] << 8) |
                (drv->addrCompletShadow[2] << 16) |
                (drv->addrCompletShadow[3] << 24));

        /* write the address completion 3 times.
        because the PP have a 2 entry write buffer
        so, the 3 writes will make sure we do get
        to the hardware register itself */

        dataComletion = CPSS_32BIT_LE(dataComletion);

        *(volatile GT_U32 *)(addrCompletion) = dataComletion;
        *(volatile GT_U32 *)(addrCompletion) = dataComletion;
        *(volatile GT_U32 *)(addrCompletion) = dataComletion;
        GT_SYNC;

        addrData = (BASEADDR(drv) + ((compIdx << 24) |
                   (regAddr & 0x00ffffff)));
    }
    /* read the data from ASIC */
    temp = *( (volatile GT_U32 *) addrData);
    *dataPtr = CPSS_32BIT_LE(temp);

    debugTrace(("ISR Read: addr 0x%X data 0x%X \n", regAddr,  *dataPtr));

    return GT_OK;
}
/*******************************************************************************
* hwPpPciIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciIsrWrite
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  address;    /*physical address for PCI transaction */
    GT_U8   compIdx;    /* address completion register field Isr index = 1*/
    GT_U32  data;       /* data to be write to memory */
    GT_U32  tmpValue;
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    debugTrace(("ISR Write: addr 0x%X data 0x%X \n", regAddr, value));

    compIdx = 1;
    addrRegion = (GT_U8)(regAddr >> 24);

    if (addrRegion == 0)
    {
        address = BASEADDR(drv) + regAddr;
    }
    else  /* use Region #1 */
    {
        GT_UINTPTR   addrCompletion;

        addrCompletion = (BASEADDR(drv) + ADDR_COMP_REG);

        /* Update Address Completion shadow.            */
        drv->addrCompletShadow[compIdx] = addrRegion;

        /* Update Hw Address Completion Regin 1- using Region 0 */
        data = ((drv->addrCompletShadow[1] << 8) |
                (drv->addrCompletShadow[2] << 16) |
                (drv->addrCompletShadow[3] << 24));

        *(volatile GT_U32 *)(addrCompletion) = CPSS_32BIT_LE(data);
        GT_SYNC;/* to prevent data write before addreCompleteion write is done*/


        address = (BASEADDR(drv) + ((compIdx << 24) |
                   (regAddr & 0x00ffffff)));

    }

    tmpValue = CPSS_32BIT_LE(value);
    /* write data 3 times */
    *( (volatile GT_U32*) address) = tmpValue;
    *( (volatile GT_U32*) address) = tmpValue;
    *( (volatile GT_U32*) address) = tmpValue;
    GT_SYNC;/* to prevent optimization return before write complete */

    return GT_OK;
}

/*******************************************************************************
* hwPpPciReadInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciReadInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_UINTPTR address;     /* physical access address for PCI transaction. */
    GT_U32  temp;

    address = (drv->internalPciBase + regAddr);
    temp = *((volatile GT_U32 *)address);

    GT_SYNC; /*to avoid read combining */

    *data = CPSS_32BIT_LE(temp);

    debugTrace(("PCI Read: addr 0x%X data 0x%X \n", regAddr, *data));

    return GT_OK;
}


/*******************************************************************************
* hwPpPciWriteInternalPciReg
*
* DESCRIPTION:
*       This function writes to an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to write to.
*       data    - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciWriteInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_UINTPTR address;     /* physical address for PCI transaction.    */
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }


    debugTrace(("PCI Write: addr 0x%X data 0x%X \n", regAddr, data));

    address = (drv->internalPciBase + regAddr);
    *((volatile GT_U32*)address) = CPSS_32BIT_LE(data);

    return GT_OK;
}


/*******************************************************************************
* hwPpPciResetAndInitCtrlReadRegister
*
* DESCRIPTION:
*       This function reads a register from the Reset and Init Controller.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciResetAndInitCtrlReadRegister
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_UINTPTR address;     /* physical access address for transaction. */
    GT_U32  temp;

    if (drv->resetAndInitControllerBase == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    address = (drv->resetAndInitControllerBase + regAddr);
    temp = *((volatile GT_U32 *)address);

    GT_SYNC; /*to avoid read combining */

    *data = CPSS_32BIT_LE(temp);

    debugTrace(("Reset and Init Controller Read: addr 0x%X data 0x%X \n", 
                regAddr, *data));

    return GT_OK;
}


/*******************************************************************************
* hwPpPciResetAndInitCtrlWriteRegister
*
* DESCRIPTION:
*       This function writes a register to the Reset and Init Controller.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to wrire to.
*       data    - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciResetAndInitCtrlWriteRegister
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_UINTPTR address;     /* physical address for transaction.    */
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    if (drv->resetAndInitControllerBase == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    debugTrace(("Reset and Init Controller Write: addr 0x%X data 0x%X \n", regAddr, data));

    address = (drv->resetAndInitControllerBase + regAddr);
    *((volatile GT_U32*)address) = CPSS_32BIT_LE(data);

    return GT_OK;
}



/*******************************************************************************
* hwPpPciHwInit
*
* DESCRIPTION:
*       This function initializes a given PP.
*
* INPUTS:
*       drv     - Driver object ptr
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciHwInit
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv
)
{
    GT_BOOL     skipHwWrite; /* skip write to HW */
    GT_UINTPTR  address;
    GT_U8       i;

    /* allow write to HW */
    skipHwWrite = GT_FALSE;

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterWriteFunc != NULL)
    {
       GT_STATUS rc;
       rc = prvCpssHwAccessObj.hwAccessRegisterWriteFunc( 
                                                  drv->common.devNum, drv->common.portGroupId, 
                                                  ADDR_COMP_REG, 0,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           /* callback disables write to HW */
           skipHwWrite = GT_TRUE;
           rc = GT_OK;
       }        

       if(rc!= GT_OK)
        return rc;
    }

    if (skipHwWrite == GT_FALSE)
    {    
        /* Reset Hw Address Completion          */
        address = BASEADDR(drv) + ADDR_COMP_REG;
    
        *(volatile GT_U32 *)(address) = 0;
        *(volatile GT_U32 *)(address) = 0;
        *(volatile GT_U32 *)(address) = 0;
        GT_SYNC;

        /* Check if HW access required. */
        if(prvCpssHwAccessObj.hwAccessRegisterWriteFunc != NULL)
        {
           prvCpssHwAccessObj.hwAccessRegisterWriteFunc( 
                                                      drv->common.devNum, drv->common.portGroupId, 
                                                      ADDR_COMP_REG, 0,
                                                      CPSS_DRV_HW_ACCESS_STAGE_POST_E);
        }
    }

    /* Update Address Completion shadow     */
    for (i = 0; i < 4; i++)
    {
       drv->addrCompletShadow[i] = 0;
    }
    return GT_OK;
}

/*******************************************************************************
* hwPpPciDestroyDrv
*
* DESCRIPTION:
*       Destroy driver object
*
* INPUTS:
*       drv             - Driver object ptr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID hwPpPciDestroyDrv
(
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv
)
{
    if(drv->ramBurstInfo != NULL && drv->ramBurstInfoLen != 0)
        cpssOsFree(drv->ramBurstInfo);
    cpssOsMutexDelete (drv->hwComplSem);
    cpssOsFree(drv);
}

/*******************************************************************************
* cpssDriverPpHwBusPciInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum      - The PP's device number to init the structure for.
*       portGroupId - The port group id.
*       hwInfo      - Hardware info (PCI address)
*
* OUTPUTS:
*       drvPtr   - pointer
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDriverPpHwBusPciInit
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  PRV_CPSS_DRV_HW_INFO_STC    *hwInfo,
    OUT CPSS_BUS_DRIVER_OBJ_STC     **drvRet
)
{
    PRV_CPSS_BUS_DRIVER_PCI_STC *drv;
    char    name[20];        /* name of mutex */


    /* create object */
    drv = (PRV_CPSS_BUS_DRIVER_PCI_STC*)cpssOsMalloc(sizeof(*drv));
    if (drv == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    cpssOsMemSet(drv, 0, sizeof(*drv));
    drv->common.devNum = devNum;
    drv->common.portGroupId = portGroupId;
    drv->common.drvHwPpDestroyDrv = (CPSS_DRV_DESTROY_DRV_FUNC) hwPpPciDestroyDrv;
    drv->common.drvHwPpHwInit = (CPSS_DRV_HWINIT_FUNC) hwPpPciHwInit;
    drv->common.drvHwPpReadReg = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciReadRegister;
    drv->common.drvHwPpWriteReg = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciWriteRegister;
    drv->common.drvHwPpReadRam = (CPSS_DRV_HW_READ_RAM_FUNC) hwPpPciReadRam;
    drv->common.drvHwPpWriteRam = (CPSS_DRV_HW_WRITE_RAM_FUNC) hwPpPciWriteRam;
    drv->common.drvHwPpIsrRead = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciIsrRead;
    drv->common.drvHwPpIsrWrite = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciIsrWrite;
    drv->common.drvHwPpReadIntPciReg = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciReadInternalPciReg;
    drv->common.drvHwPpWriteIntPciReg = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciWriteInternalPciReg;
    drv->common.drvHwPpResetAndInitControllerReadReg = (CPSS_DRV_HW_READ_REG_FUNC)hwPpPciResetAndInitCtrlReadRegister;
    drv->common.drvHwPpResetAndInitControllerWriteReg = (CPSS_DRV_HW_WRITE_REG_FUNC)hwPpPciResetAndInitCtrlWriteRegister;


    drv->baseAddr = hwInfo->baseAddr;
    drv->internalPciBase = hwInfo->internalPciBase;
    drv->resetAndInitControllerBase = hwInfo->resetAndInitControllerBase;

    drv->compIdx            = 2;

    cpssOsSprintf (name,"hwComplSem-%d_%ld",devNum,portGroupId);

    cpssOsMutexCreate (name,&(drv->hwComplSem));

    *drvRet = (CPSS_BUS_DRIVER_OBJ_STC*)drv;

    return GT_OK;
}
