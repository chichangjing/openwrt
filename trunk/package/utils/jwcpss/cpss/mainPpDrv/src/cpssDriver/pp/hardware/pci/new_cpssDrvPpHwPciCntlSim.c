/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpHwTEMPLATECntl.c
*
* DESCRIPTION:
*       Prestera driver Hardware read and write functions implementation.
*       TEMPLATE
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <asicSimulation/SCIB/scib.h>




typedef struct PRV_CPSS_BUS_DRIVER_PCISIM_STCT {
    CPSS_BUS_DRIVER_OBJ_STC common;
    GT_UINTPTR          baseAddr;
    GT_U8               devId;
    GT_BOOL             addrCompletEnable;
    GT_U32              addrCompletShadow[4];
    CPSS_OS_MUTEX       hwComplSem;
    GT_U8               compIdx;
    PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo;
    GT_U8               ramBurstInfoLen;
    GT_U32              lastWriteAddr[2];
} PRV_CPSS_BUS_DRIVER_PCISIM_STC;


/*******************************************************************************
* Intenal Macros and definitions
*******************************************************************************/

#define debugPrint(_x)\
    if(memoryAccessTraceOn == GT_TRUE)\
       cpssOsPrintf _x


/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Forward functions declarations.
*******************************************************************************/
static GT_STATUS hwPpPciSimReadInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);

static GT_STATUS hwPpPciSimWriteInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

static GT_STATUS hwCompletion
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
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
            address = drv->baseAddr + ADDR_COMP_REG;
            data = (drv->addrCompletShadow[1]<<8) |
                   (drv->addrCompletShadow[2]<<16) |
                   (drv->addrCompletShadow[3]<<24);

            data = CPSS_32BIT_LE(data);

            scibWriteMemory(drv->devId, address, 1, &data);
        }
        else
        {
            compIdx = i;
        }
    }

    address = ((drv->baseAddr) + ((compIdx << 24) | (regAddr & 0x00ffffff)));
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
*       devNum  - The PP dev Num to read from.
*       portGroupId  - The port group Id.
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
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_U32      *dataPtr
)
{
    GT_UINTPTR  address;  /*physical address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U32  j;      /* count iterator for the write loop*/
    GT_U32  nextRegionAddr; /* address of the next region after the one
                                currently used */
    GT_U32  loopLength = 0; /* when length exceeds region addr, Set to end of
                            region range */
    GT_U32  data;   /* data to be write to memory */
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    if (drv->addrCompletEnable == GT_FALSE)
    {
        address = regAddr;

        for(j = 0; j < length; j++)
        {
            data = CPSS_32BIT_LE(dataPtr[j]);


            scibWriteMemory(drv->devId, address, 1, &data);

            address += 4;
        }

        return GT_OK;
    }


    hwCompletion(drv, regAddr, &compIdx, &address);

    /* check whether completion region boundaries exceeded*/
    nextRegionAddr =  (GT_U32)(drv->addrCompletShadow[compIdx] + 1)<<24;
    if((GT_U32)(regAddr + length * 4 - 1) < nextRegionAddr)
    {
        for(j = 0; j < length; j++)
        {
            data = CPSS_32BIT_LE(dataPtr[j]);

            scibWriteMemory(drv->devId, address, 1, &data);

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
            data = CPSS_32BIT_LE(dataPtr[j]);

            scibWriteMemory(drv->devId, address, 1, &data);

            address += 4;
        }
        if (compIdx != 0)
            cpssOsMutexUnlock(drv->hwComplSem);

        /* Recursive call for rest of data in next region.  */
        hwWrite(drv,nextRegionAddr,length-loopLength,dataPtr+loopLength);
    }

    return GT_OK;
}


/*******************************************************************************
*  hwRead
*
* DESCRIPTION:
*      This function receives address and buffer pointer and reads the the PP
*      RAM into the buffer.
* INPUTS:
*       devNum  - The PP dev Num to read from.
*       portGroupId  - The port group Id.
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
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
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

    if (drv->addrCompletEnable == GT_FALSE)
    {
        address = regAddr;

        for(j = 0;j < length;j++)
        {
            scibReadMemory(drv->devId, address, 1, &temp);

            dataPtr[j] = CPSS_32BIT_LE(temp);
            address += 4;
        }

        return GT_OK;
    }

    hwCompletion(drv, regAddr, &compIdx, &address);

    /* check whether completion region boundaries are exceeded*/
    nextRegionAddr = (GT_U32)(drv->addrCompletShadow[compIdx]+1)<<24;
    if ((GT_U32)(regAddr+length*4-1) < nextRegionAddr)
    {
        for(j = 0;j < length;j++)
        {
            scibReadMemory(drv->devId, address, 1, &temp);
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
            scibReadMemory(drv->devId, address, 1, &temp);
            dataPtr[j] = CPSS_32BIT_LE(temp);
            address += 4;
        }
        if (compIdx != 0)
            cpssOsMutexUnlock(drv->hwComplSem);

        /* recursive call for rest of data in next region */
        hwRead(drv,nextRegionAddr,length-loopLength,
                dataPtr+loopLength);
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPciSimReadRegister
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
static GT_STATUS hwPpPciSimReadRegister
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    debugPrint(("Read on device %d, from register 0x%08x, ",drv->common.devNum, regAddr));
    hwRead (drv,regAddr, 1, data);
    debugPrint((" data 0x%08x.\n", *data));
    return GT_OK;
}


/*******************************************************************************
* hwPpPciSimWriteRegister
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
static GT_STATUS hwPpPciSimWriteRegister
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    debugPrint(("Write on device %d, to register 0x%08x, data 0x%08x.\n",
                drv->common.devNum, regAddr, value));

    hwWrite (drv,regAddr, 1, &value);
    return GT_OK;
}


/*******************************************************************************
* hwPpPciSimReadRam
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
static GT_STATUS hwPpPciSimReadRam
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32  *data
)
{
    hwRead (drv, addr, length, data);
    return GT_OK;
}



/*******************************************************************************
* hwPpPciSimWriteRam
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
static GT_STATUS hwPpPciSimWriteRam
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    hwWrite(drv, addr, length, data);
    return GT_OK;
}

/*******************************************************************************
* hwPpPciSimIsrRead
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
static GT_STATUS hwPpPciSimIsrRead
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  addrData;    /* physical address for PCI transaction */
    GT_U8   compIdx = 0;    /* address completion register field Isr index = 1*/
    GT_U32  temp;
    GT_UINTPTR addrCompletion;
    GT_U32  dataCompletion;

    if (drv->addrCompletEnable == GT_FALSE)
    {
        scibReadMemory(drv->devId, regAddr, 1, &temp);

        *dataPtr = CPSS_32BIT_LE(temp);

        return GT_OK;
    }

    /* msb_ = region value */
    addrRegion = (GT_U8)(regAddr >> 24);

    if (addrRegion != 0)
    {
        /* Set addrRegion in AddrComplition register = 1 */
        compIdx = 1;

        addrCompletion = (drv->baseAddr + ADDR_COMP_REG);

        /* Update Address Completion shadow.            */
        drv->addrCompletShadow[compIdx] = addrRegion;

        /* Update Hw Address Completion Region 1- using region 0. */
        dataCompletion = (
                (drv->addrCompletShadow[1] << 8) |
                (drv->addrCompletShadow[2] << 16) |
                (drv->addrCompletShadow[3] << 24));

        /* write the address completion 3 times.
        because the PP have a 2 entry write buffer
        so, the 3 writes will make sure we do get
        to the hardware register itself */

        dataCompletion = CPSS_32BIT_LE(dataCompletion);


        scibWriteMemory(drv->devId, addrCompletion, 1, &dataCompletion);

    }
    addrData = (drv->baseAddr + ((compIdx << 24) | (regAddr & 0x00ffffff)));
    /* read the data from ASIC */
    scibReadMemory(drv->devId, addrData, 1, &temp);
    *dataPtr = CPSS_32BIT_LE(temp);

    return GT_OK;
}

/*******************************************************************************
* hwPpPciSimIsrWrite
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
static GT_STATUS hwPpPciSimIsrWrite
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  address;    /*physical address for PCI transaction */
    GT_U8   compIdx = 0;    /* address completion register field Isr index = 1*/
    GT_U32  data;       /* data to be write to memory */
    GT_U32  tmpValue;
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    if (drv->addrCompletEnable == GT_FALSE)
    {
        tmpValue = CPSS_32BIT_LE(value);

        scibWriteMemory(drv->devId, regAddr, 1, &tmpValue);

        return GT_OK;
    }

    addrRegion = (GT_U8)(regAddr >> 24);

    if (addrRegion != 0)
    {
        GT_U32   addrCompletion;

        /* use Region #1 */
        compIdx = 1;

        addrCompletion = (drv->baseAddr + ADDR_COMP_REG);

        /* Update Address Completion shadow.            */
        drv->addrCompletShadow[compIdx] = addrRegion;

        /* Update Hw Address Completion Regin 1- using Region 0 */
        data = ((drv->addrCompletShadow[1] << 8) |
                (drv->addrCompletShadow[2] << 16) |
                (drv->addrCompletShadow[3] << 24));

        tmpValue = CPSS_32BIT_LE(data);


        scibWriteMemory(drv->devId, addrCompletion, 1, &tmpValue);


    }
    address = (drv->baseAddr + ((compIdx << 24) | (regAddr & 0x00ffffff)));

    tmpValue = CPSS_32BIT_LE(value);


    scibWriteMemory(drv->devId, address, 1, &tmpValue);

    return GT_OK;
}

/*******************************************************************************
* hwPpPciSimReadInternalPciReg
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
static GT_STATUS hwPpPciSimReadInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_U32 temp;

    scibPciRegRead(drv->devId, regAddr, 1, &temp);

    *data = CPSS_32BIT_LE(temp);
    return GT_OK;
}


/*******************************************************************************
* hwPpPciSimWriteInternalPciReg
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
static GT_STATUS hwPpPciSimWriteInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_U32 temp;
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    temp = CPSS_32BIT_LE(data);

    scibPciRegWrite(drv->devId, regAddr, 1, &temp);
    return GT_OK;
}


/*******************************************************************************
* hwPpPciSimResetAndInitCtrlReadRegister
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
static GT_STATUS hwPpPciSimResetAndInitCtrlReadRegister
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_U32 temp;

    scibMemoryClientRegRead(drv->devId, 
                            SCIB_MEM_ACCESS_DFX_E, 
                            regAddr, 1, &temp);

    *data = CPSS_32BIT_LE(temp);

    debugPrint(("Reset and Init Controller Read: addr 0x%X data 0x%X \n", 
                regAddr, *data));

    return GT_OK;
}


/*******************************************************************************
* hwPpPciSimResetAndInitCtrlWriteRegister
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
static GT_STATUS hwPpPciSimResetAndInitCtrlWriteRegister
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_U32 temp;
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    debugPrint(("Reset and Init Controller Write: addr 0x%X data 0x%X \n", regAddr, data));

    temp = CPSS_32BIT_LE(data);

    scibMemoryClientRegWrite(drv->devId, 
                             SCIB_MEM_ACCESS_DFX_E, 
                             regAddr, 1, &temp);
    return GT_OK;
}


/*******************************************************************************
* hwPpPciSimHwInit
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
static GT_STATUS hwPpPciSimHwInit
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv
)
{
    GT_UINTPTR  address;
    GT_U8       i;
    GT_U32      tmpData;

    if (drv->addrCompletEnable == GT_TRUE)
    {
        /* Reset Hw Address Completion          */
        address = drv->baseAddr + ADDR_COMP_REG;

        tmpData = 0;

        scibWriteMemory(drv->devId, address, 1, &tmpData);
    }

    /* Update Address Completion shadow     */
    for (i = 0; i < 4; i++)
    {
        drv->addrCompletShadow[i] = 0;
    }

    if(PRV_CPSS_DRV_ERRATA_GET_MAC(drv->common.devNum,PRV_CPSS_DRV_ERRATA_PCI_SLAVE_UNABLE_TO_ABORT_TRANSACTION_E) == GT_TRUE)
    {
        /* Initialize the PCI Timer and Retry register with 0xFF.   */
        /* Perform a read modify write on the register.             */
        hwPpPciSimReadInternalPciReg(drv,0x104,&tmpData);

        tmpData = (tmpData & 0xFFFF0000) | 0xFFFF;  /*set both timer values to maximum */

        hwPpPciSimWriteInternalPciReg(drv,0x104,tmpData);
    }
    return GT_OK;
}

/*******************************************************************************
* hwPpPciSimDestroyDrv
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
static GT_VOID hwPpPciSimDestroyDrv
(
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv
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
    PRV_CPSS_BUS_DRIVER_PCISIM_STC *drv;
    char                name[20];     /* name of mutex */


    /* create object */
    drv = (PRV_CPSS_BUS_DRIVER_PCISIM_STC*)cpssOsMalloc(sizeof(*drv));
    if (drv == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    *drvRet = (CPSS_BUS_DRIVER_OBJ_STC*)drv;

    cpssOsMemSet(drv, 0, sizeof(*drv));
    drv->common.devNum = devNum;
    drv->common.portGroupId = portGroupId;
    drv->common.drvHwPpDestroyDrv = (CPSS_DRV_DESTROY_DRV_FUNC) hwPpPciSimDestroyDrv;
    drv->common.drvHwPpHwInit = (CPSS_DRV_HWINIT_FUNC) hwPpPciSimHwInit;
    drv->common.drvHwPpReadReg = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciSimReadRegister;
    drv->common.drvHwPpWriteReg = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciSimWriteRegister;
    drv->common.drvHwPpReadRam = (CPSS_DRV_HW_READ_RAM_FUNC) hwPpPciSimReadRam;
    drv->common.drvHwPpWriteRam = (CPSS_DRV_HW_WRITE_RAM_FUNC) hwPpPciSimWriteRam;
#if 0
    /* optional */
    drv->common.drvHwPpReadVec = (CPSS_DRV_HW_READ_VEC_FUNC) hwPpPciSimReadVec;
    drv->common.drvHwPpWriteVec = (CPSS_DRV_HW_READ_VEC_FUNC) hwPpPciSimWriteVec;
#endif
    drv->common.drvHwPpIsrRead = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciSimIsrRead;
    drv->common.drvHwPpIsrWrite = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciSimIsrWrite;
    /* optional */
    drv->common.drvHwPpReadIntPciReg = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciSimReadInternalPciReg;
    /* optional */
    drv->common.drvHwPpWriteIntPciReg = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciSimWriteInternalPciReg;
    /* optional */
    drv->common.drvHwPpResetAndInitControllerReadReg = (CPSS_DRV_HW_READ_REG_FUNC)hwPpPciSimResetAndInitCtrlReadRegister;
    /* optional */
    drv->common.drvHwPpResetAndInitControllerWriteReg = (CPSS_DRV_HW_WRITE_REG_FUNC)hwPpPciSimResetAndInitCtrlWriteRegister;


    drv->baseAddr = hwInfo->baseAddr;

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].simInfo.devId =
                            (GT_U8)drv->devId;

    drv->compIdx            = 2;

    cpssOsSprintf (name,"hwComplSem-%d_%ld",devNum,portGroupId);

    cpssOsMutexCreate (name,&(drv->hwComplSem));


    return GT_OK;
}
