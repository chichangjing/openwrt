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




typedef struct PRV_CPSS_BUS_DRIVER_PEXMBUS_STCT {
    CPSS_BUS_DRIVER_OBJ_STC common;
    GT_UINTPTR          baseAddr;
#ifdef ASIC_SIMULATION
    GT_U8               simDevId;
    GT_BOOL             simAddrCompletEnable;
#endif /* ASIC_SIMULATION */
    GT_U32              addrCompletShadow[8];
    GT_U32              isrAddrCompletionRegionsBmp;
    GT_U32              isrAddrRegionUsed;
    GT_U32              isrAddrRegionMin;
    GT_U32              isrAddrRegionMax;
    GT_U32              appAddrCompletionRegionsBmp;
    GT_U32              appAddrRegionUsed;
    GT_U32              appAddrRegionMin;
    GT_U32              appAddrRegionMax;
    CPSS_OS_MUTEX       hwComplSem;
    GT_UINTPTR          internalPciBase;
    GT_UINTPTR          resetAndInitControllerBase;
    PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo;
    GT_U8               ramBurstInfoLen;


} PRV_CPSS_BUS_DRIVER_PEXMBUS_STC;


/*******************************************************************************
* Intenal Macros and definitions
*******************************************************************************/
#ifdef ASIC_SIMULATION
#include <asicSimulation/SCIB/scib.h>
/* Macro to convert device ID to simulation device ID */
#define SIM_DEV_ID(_drv)     (_drv->simDevId)
#define SIM_ADDR_COMPL_EN_GET(_drv)  (_drv->simAddrCompletEnable)

/* the WM simulation does not support address completion */
/* the GM simulation supports address completion */
#define ADDRESS_COMPLETION_ENABLE_MAC(_drv) \
    SIM_ADDR_COMPL_EN_GET(_drv)
#define ADDRESS_COMPLETION_DISABLE_SUPPORTED_CNS 1
#else /*ASIC_SIMULATION*/
/* the HW supports address completion */
#define ADDRESS_COMPLETION_DISABLE_SUPPORTED_CNS 0
#endif /*ASIC_SIMULATION*/

/* bits of address passes as is throw PCI window */
#define NOT_ADDRESS_COMPLETION_BITS_NUM_CNS 19
/* bits of address extracted from address completion registers */
#define ADDRESS_COMPLETION_BITS_MASK_CNS    (0xFFFFFFFF << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS)


#define debugPrint(_x)\
    if(memoryAccessTraceOn == GT_TRUE)\
       cpssOsPrintf _x

#ifdef HW_DEBUG_TRACE
#define debugTrace(_x)      cpssOsPrintSync _x
#else
#define debugTrace(_x)
#endif  /* HW_DEBUG_TRACE */

/* to exit all functions according to system state */
#define SYSTEM_STATE_CAUSED_SKIP_MAC                                                               \
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&          \
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) ) \
    {                                                                                              \
        /* we are in HSU or HA process -- hardware should not be affected */                       \
        return GT_OK;                                                                              \
    }

#if (ADDRESS_COMPLETION_DISABLE_SUPPORTED_CNS == 0)
#define CPU_MEMORY_BASE_ADDR_MAC(_drv) (_drv->baseAddr)
#else
#define CPU_MEMORY_BASE_ADDR_MAC(_drv)                \
    ((ADDRESS_COMPLETION_ENABLE_MAC(_drv) == GT_FALSE) \
    ? 0 : drv->baseAddr)
#endif




/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Forward functions declarations.
*******************************************************************************/

/*******************************************************************************
*   cpuReadMemoryWord
*
* DESCRIPTION:
*       Read memory from device.
*
* INPUTS:
*       drv         - Driver object ptr
*       memAddr     - address of first word to read.
*
* OUTPUTS:
*       dataPtr - pointer to copy read data.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
static void cpuReadMemoryWord
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32        memAddr,
    OUT GT_U32        *dataPtr
)
{
    GT_U32     temp;    /* temp word */
    GT_UINTPTR address; /* address adjusted to window */

    address = CPU_MEMORY_BASE_ADDR_MAC(drv) + memAddr;
#ifdef ASIC_SIMULATION
    scibReadMemory(SIM_DEV_ID(drv), address, 1/*length*/, &temp);
#else /*ASIC_SIMULATION*/
    temp = *((volatile GT_U32 *)address);
    GT_SYNC; /* to avoid read combining */
#endif /*ASIC_SIMULATION*/
    *dataPtr = CPSS_32BIT_LE(temp);
}

/*******************************************************************************
*   cpuWriteMemoryWord
*
* DESCRIPTION:
*       Write memory from device.
*
* INPUTS:
*       drv         - Driver object ptr
*       memAddr     - address of first word to read.
*       data        - write data.
*
* OUTPUTS:
*       dataPtr - pointer to copy read data.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
static void cpuWriteMemoryWord
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32        memAddr,
    OUT GT_U32        data
)
{
    GT_U32     leData;  /*little endian data*/
    GT_UINTPTR address; /* address adjusted to window */

    address = CPU_MEMORY_BASE_ADDR_MAC(drv) + memAddr;
    leData = CPSS_32BIT_LE(data);
#ifdef ASIC_SIMULATION
    scibWriteMemory(SIM_DEV_ID(drv), address, 1/*length*/, &leData);
#else /*ASIC_SIMULATION*/
    *((volatile GT_U32*)address) = leData;
    /* need to put GT_SYNC to avoid from having the CPU doing
    * write combine. Some CPUs like advanced new PPC can do write combining
    * of two writes to continuous addresses. So, to avoid that we use the eioio
    * in PPC architecture */
    GT_SYNC; /* to avoid from write combining */
#endif /*ASIC_SIMULATION*/
}

/*******************************************************************************
*   cpuReadMemoryArray
*
* DESCRIPTION:
*       Read memory from device.
*
* INPUTS:
*       drv         - Driver object ptr
*       memAddr     - address of first word to read.
*       length - number of words to read.
*
* OUTPUTS:
*       dataPtr - pointer to copy read data.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
static void cpuReadMemoryArray
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32        memAddr,
    IN  GT_U32        length,
    OUT GT_U32        *dataPtr
)
{
    GT_U32     temp;    /* temp word       */
    GT_U32     i;       /* loop index      */
    GT_UINTPTR address; /* address adjusted to window */
    GT_UINTPTR base;    /* address adjusted to window */

    base = CPU_MEMORY_BASE_ADDR_MAC(drv) + memAddr;
    for (i = 0; (i < length); i++)
    {
        address = base + (4 * i);
#ifdef ASIC_SIMULATION
        scibReadMemory(SIM_DEV_ID(drv), address, 1/*length*/, &temp);
#else /*ASIC_SIMULATION*/
        temp = *( (volatile GT_U32 *) address);
        GT_SYNC; /* to avoid read combining */
#endif /*ASIC_SIMULATION*/
        dataPtr[i] = CPSS_32BIT_LE(temp);
    }
}

/*******************************************************************************
*   cpuWriteMemoryArray
*
* DESCRIPTION:
*       Write memory from device.
*
* INPUTS:
*       drv         - Driver object ptr
*       memAddr     - address of first word to read.
*       length - number of words to read.
*       dataPtr - pointer to copy read data.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
static void cpuWriteMemoryArray
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32        memAddr,
    IN  GT_U32        length,
    OUT GT_U32        *dataPtr
)
{
    GT_U32     leData;  /* little endian data */
    GT_U32     i;       /* loop index         */
    GT_UINTPTR address; /* address adjusted to window */
    GT_UINTPTR base;    /* address adjusted to window */

    base = CPU_MEMORY_BASE_ADDR_MAC(drv) + memAddr;
    for (i = 0; (i < length); i++)
    {
        address = base + (4 * i);
        leData = CPSS_32BIT_LE(dataPtr[i]);
#ifdef ASIC_SIMULATION
        scibWriteMemory(SIM_DEV_ID(drv), address, 1/*length*/, &leData);
#else /*ASIC_SIMULATION*/
        *((volatile GT_U32*)address) = leData;
        /* need to put GT_SYNC to avoid from having the CPU doing
        * write combine. Some CPUs like advanced new PPC can do write combining
        * of two writes to continuous addresses. So, to avoid that we use the eioio
        * in PPC architecture */
        GT_SYNC; /* to avoid from write combining */
#endif /*ASIC_SIMULATION*/
    }
}

/*******************************************************************************
* setAddrCompletion
*
* DESCRIPTION:
*       This function sets the address completion registers value (in shadow and
*       Hw), for a read / write operation.
*
* INPUTS:
*       drv         - Driver object ptr
*       regAddr - The address to read / write from .
*       appAccess - GT_TRUE - regular CPSS API, GT_FALSE - ISR engine
*
* OUTPUTS:
*       compIdxPtr  - The address completion index that was choosen.
*       pciAddr     - The address to be accessed on the Pci (for read / write).
*                     (without PCI window base)
*       signalSem   - Should the semaphore be signaled after the read / write is
*                     performed.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       Source address bits are [31:19] - completion bits, [18:0] passed bits.
*       Comlpetion bits must be written to one of region address completion registers
*       Access address is Window_base + bits [21:19] - region number, [18:0] passed bits.
*
*******************************************************************************/
static GT_STATUS setAddrCompletion
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32      regAddr,
    IN  GT_BOOL     appAccess,
    OUT GT_U8       *compIdxPtr,
    OUT GT_U32      *pciAddr,
    OUT GT_BOOL     *signalSem
)
{
    GT_U32  addrRegion;  /* 13 bit MSB value of PP internal address */
    GT_U32  address;     /* address of completion register in region 0 */
    GT_U32  compIdx;     /* address completion register field index 0-3*/
    GT_U32  i;           /* count iterator for the completion index compare loop*/
    GT_U32  regBmp;      /* the bitmap of regions  */
    GT_U32  regMin;      /* the minimal of regions */
    GT_U32  regMax;      /* the maximal of regions */
    GT_U32  *regUsedPtr; /* pointer to last used region   */

    if ((regAddr & ADDRESS_COMPLETION_BITS_MASK_CNS) == 0)
    {
        /* address comletion bits are 0, region0 used */
        *compIdxPtr = 0;
        *pciAddr    = regAddr;
        *signalSem  = GT_FALSE;
        return GT_OK;
    }

    if (appAccess == GT_FALSE)
    {
        /*ISR*/
        *signalSem  = GT_FALSE;
        regBmp      = drv->isrAddrCompletionRegionsBmp;
        regUsedPtr  = &(drv->isrAddrRegionUsed);
        regMin      = drv->isrAddrRegionMin;
        regMax      = drv->isrAddrRegionMax;
    }
    else
    {
        /*CPSS API*/
        *signalSem  = GT_TRUE;
        regBmp      = drv->appAddrCompletionRegionsBmp;
        regUsedPtr  = &(drv->appAddrRegionUsed);
        regMin      = drv->appAddrRegionMin;
        regMax      = drv->appAddrRegionMax;
    }

    if (*signalSem == GT_TRUE)
    {
        cpssOsMutexLock(drv->hwComplSem);
    }

    addrRegion = (regAddr >> NOT_ADDRESS_COMPLETION_BITS_NUM_CNS);

    /* Compare addr region to existing Address regions. */
    compIdx = 0xFFFF; /* wrong value as stamp */
    for (i = regMin; (i <= regMax); i++)
    {
        /* bypass not assigned regions */
        if ((regBmp & (1 << i)) == 0)
            continue;

        if (addrRegion == drv->addrCompletShadow[i])
        {
            /* ready region found */
            compIdx = i;
            break;
        }
    }

    if (compIdx == 0xFFFF)
    {
        /* needed to configure region */
        /* try from next bit */
        compIdx = *regUsedPtr + 1;
        if (compIdx > regMax)
        {
            compIdx = regMin;
        }
        else
        {
            /* look for next bit in bitmap */
            for (i = compIdx; (i <= regMax); i++)
            {
                if ((regBmp & (1 << i)) != 0)
                {
                    compIdx = i;
                    break;
                }
            }
        }
        /* save new used region */
        *regUsedPtr = compIdx;

        /* configure completion region in HW */
        /* adders base added at the lower level cpuWrite/cpuRead */
        address = PEX_MBUS_ADDR_COMP_REG_MAC(compIdx);
        cpuWriteMemoryWord(drv, address, addrRegion);
        /* save configuration */
        drv->addrCompletShadow[compIdx] = addrRegion;
    }

    /* adders base added at the lower level cpuWrite/cpuRead */
    *pciAddr =
        ((compIdx << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS)
        | (regAddr & (~ ADDRESS_COMPLETION_BITS_MASK_CNS)));
    *compIdxPtr = compIdx;

    return GT_OK;
}

/*******************************************************************************
*  hwRead
*
* DESCRIPTION:
*      This function receives address and buffer pointer and reads the the PP
*      RAM into the buffer.
* INPUTS:
*       drv          - Driver object ptr
*       regAddr      - The register's address .
*       length       - Nubmber of writes in 4 byte each.
*       appAccess    - GT_TRUE - regular CPSS API, GT_FALSE - ISR engine
*       dataPtr      - Pointer to data array.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwRead
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_BOOL     appAccess,
    OUT GT_U32      *dataPtr
)
{
    GT_STATUS rc;        /* return code                             */
    GT_U8     compIdx;   /* address completion register field index */
    GT_U32    address;   /* physical access address for PCI access  */
    GT_BOOL   signalSem; /* is needed to unlock semaphore           */
    GT_U32    lenInReg;  /* length of the part in the same region   */

#if (ADDRESS_COMPLETION_DISABLE_SUPPORTED_CNS != 0)
    if (ADDRESS_COMPLETION_ENABLE_MAC(drv) == GT_FALSE)
    {
        cpuReadMemoryArray(drv, regAddr, length, dataPtr);

        return GT_OK;
    }
#endif /*ADDRESS_COMPLETION_DISABLE_SUPPORTED_CNS*/

    /* support the region bound crossing address space by splitting to recursive calls */
    if (((regAddr + (4 * (length - 1))) & ADDRESS_COMPLETION_BITS_MASK_CNS)
        != (regAddr & ADDRESS_COMPLETION_BITS_MASK_CNS))
    {
        while (1)
        {
            /* distance to the end of region in words */
            lenInReg =
                (((regAddr & ADDRESS_COMPLETION_BITS_MASK_CNS)
                 + (1 << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS)) - regAddr) / 4;

            if (length <= lenInReg)
            {
                lenInReg = length;
            }
            /* read part in current region */
            rc = hwRead(drv, regAddr, lenInReg, appAccess, dataPtr);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* increment addresses and decrement length */
            regAddr += (4 * lenInReg);
            dataPtr += lenInReg;
            length  -= lenInReg;

            if (length == 0)
                break;
        }

        return GT_OK;
    }

    rc = setAddrCompletion(drv, regAddr, appAccess,
        &compIdx, &address, &signalSem);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpuReadMemoryArray(drv, address, length, dataPtr);

    if (signalSem != GT_FALSE)
    {
        cpssOsMutexUnlock(drv->hwComplSem);
    }

    return GT_OK;
}

/*******************************************************************************
*  hwWrite
*
* DESCRIPTION:
*      This function receives address and data pointer and write the data to
*      the PP.
* INPUTS:
*       drv          - Driver object ptr
*       regAddr      - The register's address .
*       length       - Nubmber of writes in 4 byte each.
*       appAccess    - GT_TRUE - regular CPSS API, GT_FALSE - ISR engine
*       dataPtr      - Pointer to data array.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwWrite
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_BOOL     appAccess,
    IN  GT_U32      *dataPtr
)
{
    GT_STATUS rc;        /* return code                             */
    GT_U8     compIdx;   /* address completion register field index */
    GT_U32    address;   /* physical access address for PCI access  */
    GT_BOOL   signalSem; /* is needed to unlock semaphore           */
    GT_U32    lenInReg;  /* length of the part in the same region   */

    SYSTEM_STATE_CAUSED_SKIP_MAC;

    debugTrace(("Write: addr 0x%X len 0x%X data 0x%X \n", regAddr, length, *dataPtr));

#if (ADDRESS_COMPLETION_DISABLE_SUPPORTED_CNS != 0)
    if (ADDRESS_COMPLETION_ENABLE_MAC(drv) == GT_FALSE)
    {
        cpuWriteMemoryArray(drv, regAddr, length, dataPtr);

        return GT_OK;
    }
#endif /*ADDRESS_COMPLETION_DISABLE_SUPPORTED_CNS*/

    /* support the region bound crossing address space by splitting to recursive calls */
    if (((regAddr + (4 * (length - 1))) & ADDRESS_COMPLETION_BITS_MASK_CNS)
        != (regAddr & ADDRESS_COMPLETION_BITS_MASK_CNS))
    {
        while (1)
        {
            /* distance to the end of region in words */
            lenInReg =
                (((regAddr & ADDRESS_COMPLETION_BITS_MASK_CNS)
                 + (1 << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS)) - regAddr) / 4;

            if (length <= lenInReg)
            {
                lenInReg = length;
            }
            /* write part in current region */
            rc = hwWrite(drv, regAddr, lenInReg, appAccess, dataPtr);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* increment addresses and decrement length */
            regAddr += (4 * lenInReg);
            dataPtr += lenInReg;
            length  -= lenInReg;

            if (length == 0)
                break;
        }

        return GT_OK;
    }

    rc = setAddrCompletion(drv, regAddr, appAccess,
        &compIdx, &address, &signalSem);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpuWriteMemoryArray(drv, address, length, dataPtr);

    if (signalSem != GT_FALSE)
    {
        cpssOsMutexUnlock(drv->hwComplSem);
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusReadRegister
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
static GT_STATUS hwPpPexMbusReadRegister
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    debugPrint(("Read from dev %d, reg 0x%x,",drv->common.devNum, regAddr));
    hwRead (drv, regAddr, 1, GT_TRUE /*appAccess*/, data);
    debugPrint((" data 0x%x.\n", *data));
    return GT_OK;
}


/*******************************************************************************
* hwPpPexMbusWriteRegister
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
static GT_STATUS hwPpPexMbusWriteRegister
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    debugPrint(
        ("Write to device %d, reg 0x%x, data 0x%x.\n",drv->common.devNum,regAddr,
        value));

    hwWrite(drv, regAddr, 1, GT_TRUE /*appAccess*/, &value);
    return GT_OK;
}


/*******************************************************************************
* hwPpPexMbusReadRam
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
static GT_STATUS hwPpPexMbusReadRam
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32  *data
)
{
    hwRead(drv, addr, length, GT_TRUE /*appAccess*/, data);
    return GT_OK;
}



/*******************************************************************************
* hwPpPexMbusWriteRam
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
static GT_STATUS hwPpPexMbusWriteRam
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    hwWrite(drv, addr, length, GT_TRUE /*appAccess*/, data);
    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusIsrRead
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
static GT_STATUS hwPpPexMbusIsrRead
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    debugPrint(("Read from dev %d, reg 0x%x,",drv->common.devNum, regAddr));
    hwRead(drv, regAddr, 1, GT_FALSE /*appAccess*/, dataPtr);
    debugPrint((" data 0x%x.\n", *dataPtr));
    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusIsrWrite
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
static GT_STATUS hwPpPexMbusIsrWrite
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    debugPrint(
        ("Write to device %d, reg 0x%x, data 0x%x.\n",drv->common.devNum,regAddr,
        value));

    hwWrite(drv, regAddr, 1, GT_FALSE /*appAccess*/, &value);

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusReadInternalPciReg
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
static GT_STATUS hwPpPexMbusReadInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
#ifndef ASIC_SIMULATION
    GT_UINTPTR address;     /* physical access address for PCI transaction. */
#endif
    GT_U32  temp;

#ifdef ASIC_SIMULATION
    scibPciRegRead(SIM_DEV_ID(drv), regAddr, 1, &temp);
#else /*ASIC_SIMULATION*/
    address = (drv->internalPciBase + regAddr);
    temp = *((volatile GT_U32 *)address);
    GT_SYNC; /*to avoid read combining */
#endif /*ASIC_SIMULATION*/


    *data = CPSS_32BIT_LE(temp);

    debugTrace(("PCI Read: addr 0x%X data 0x%X \n", regAddr, *data));

    return GT_OK;
}


/*******************************************************************************
* hwPpPexMbusWriteInternalPciReg
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
static GT_STATUS hwPpPexMbusWriteInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
#ifndef ASIC_SIMULATION
    GT_UINTPTR address;     /* physical address for PCI transaction.    */
#endif
    GT_U32     leData;      /* LE data */
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }


    debugTrace(("PCI Write: addr 0x%X data 0x%X \n", regAddr, data));

    leData = CPSS_32BIT_LE(data);
#ifdef ASIC_SIMULATION
    scibPciRegWrite(SIM_DEV_ID(drv), regAddr, 1, &leData);
#else /*ASIC_SIMULATION*/
    address = (drv->internalPciBase + regAddr);

    *((volatile GT_U32*)address) = leData;
    /* need to put GT_SYNC to avoid from having the CPU doing
    * write combine. Some CPUs like advanced new PPC can do write combining
    * of two writes to continuous addresses. So, to avoid that we use the eioio
    * in PPC architecture */
    GT_SYNC; /* to avoid from write combining */
#endif /*ASIC_SIMULATION*/

    return GT_OK;
}


/*******************************************************************************
* hwPpPexMbusResetAndInitCtrlReadRegister
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
static GT_STATUS hwPpPexMbusResetAndInitCtrlReadRegister
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_U32     temp;        /* data in HW format                        */
    GT_UINTPTR address;     /* physical access address for transaction. */

#ifdef ASIC_SIMULATION
    address = regAddr;
    scibMemoryClientRegRead(
        SIM_DEV_ID(drv), SCIB_MEM_ACCESS_DFX_E, address, 1, &temp);
#else /*ASIC_SIMULATION*/
    if (drv->resetAndInitControllerBase == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    address = (drv->resetAndInitControllerBase + regAddr);
    temp = *((volatile GT_U32 *)address);
    GT_SYNC; /*to avoid read combining */
#endif /*ASIC_SIMULATION*/

    *data = CPSS_32BIT_LE(temp);

    debugPrint(("Reset and Init Controller Read: addr 0x%X data 0x%X \n",
                regAddr, *data));

    return GT_OK;
}


/*******************************************************************************
* hwPpPexMbusResetAndInitCtrlWriteRegister
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
static GT_STATUS hwPpPexMbusResetAndInitCtrlWriteRegister
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_U32     temp;        /* data in HW format                        */
    GT_UINTPTR address;     /* physical access address for transaction. */

    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    debugPrint(("Reset and Init Controller Write: addr 0x%X data 0x%X \n", regAddr, data));

    temp = CPSS_32BIT_LE(data);

#ifdef ASIC_SIMULATION
    address = regAddr;
    scibMemoryClientRegWrite(
        SIM_DEV_ID(drv), SCIB_MEM_ACCESS_DFX_E, address, 1, &temp);
#else /*ASIC_SIMULATION*/
    if (drv->resetAndInitControllerBase == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    address = (drv->resetAndInitControllerBase + regAddr);
    *((volatile GT_U32*)address) = temp;
    GT_SYNC; /*to avoid read combining */
#endif /*ASIC_SIMULATION*/

    return GT_OK;
}



/*******************************************************************************
* hwPpPexMbusHwInit
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
static GT_STATUS hwPpPexMbusHwInit
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv
)
{
    GT_U8   i;               /* iterator      */
    GT_U32  data;

    /* Reset Hw Address Completion registers not needed         */
    /* The 0-th region must be zero and never updated           */
    /* Other regions were be updated when will be needed after  */
    /* unsucssess searching needed value in shadow              */

    /* Update Address Completion shadow     */
    for (i = 0; i < 8; i++)
    {
        drv->addrCompletShadow[i] = 0;
    }

    /* set 8-region mode: regAddr = 0x140, set bit16 to 0 */
    cpuReadMemoryWord(drv, 0x140, &data);
    data &= (~ (1 << 16));
    cpuWriteMemoryWord(drv, 0x140, data);

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusDestroyDrv
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
static GT_VOID hwPpPexMbusDestroyDrv
(
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv
)
{
    if(drv->ramBurstInfo != NULL && drv->ramBurstInfoLen != 0)
        cpssOsFree(drv->ramBurstInfo);
    cpssOsMutexDelete (drv->hwComplSem);
    cpssOsFree(drv);
}

/*******************************************************************************
* cpssDriverPpHwBusPexMbusInit
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
GT_STATUS cpssDriverPpHwBusPexMbusInit
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  PRV_CPSS_DRV_HW_INFO_STC    *hwInfo,
    OUT CPSS_BUS_DRIVER_OBJ_STC     **drvRet
)
{
    PRV_CPSS_BUS_DRIVER_PEXMBUS_STC *drv;
    char    name[20];        /* name of mutex */
    GT_U32  regBmpIsr;       /* the bitmap of regions for ISR  */
    GT_U32  regBmpApp;       /* the bitmap of regions for CPSS */
    GT_U32  regBmp;          /* the bitmap of regions (work) */
    GT_U8   i;               /* iterator      */

    /* Check completion masks validity */
    regBmpIsr = hwInfo->isrAddrCompletionRegionsBmp;
    regBmpApp = hwInfo->appAddrCompletionRegionsBmp;
    if (((regBmpIsr & 0xFE) == 0) || ((regBmpIsr & (~ 0xFE)) != 0))
    {
        /* regions 1-7 only allowed to use */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (((regBmpApp & 0xFE) == 0) || ((regBmpApp & (~ 0xFE)) != 0))
    {
        /* regions 1-7 only allowed to use */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if ((regBmpIsr & regBmpApp) != 0)
    {
        /* ISR and CPSS must use different regions */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* create object */
    drv = (PRV_CPSS_BUS_DRIVER_PEXMBUS_STC*)cpssOsMalloc(sizeof(*drv));
    if (drv == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    *drvRet = (CPSS_BUS_DRIVER_OBJ_STC*)drv;

    cpssOsMemSet(drv, 0, sizeof(*drv));
    drv->common.devNum = devNum;
    drv->common.portGroupId = portGroupId;
    drv->common.drvHwPpDestroyDrv = (CPSS_DRV_DESTROY_DRV_FUNC) hwPpPexMbusDestroyDrv;
    drv->common.drvHwPpHwInit = (CPSS_DRV_HWINIT_FUNC) hwPpPexMbusHwInit;
    drv->common.drvHwPpReadReg = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPexMbusReadRegister;
    drv->common.drvHwPpWriteReg = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPexMbusWriteRegister;
    drv->common.drvHwPpReadRam = (CPSS_DRV_HW_READ_RAM_FUNC) hwPpPexMbusReadRam;
    drv->common.drvHwPpWriteRam = (CPSS_DRV_HW_WRITE_RAM_FUNC) hwPpPexMbusWriteRam;
#if 0
    /* optional */
    drv->common.drvHwPpReadVec = (CPSS_DRV_HW_READ_VEC_FUNC) hwPpPexMbusReadVec;
    drv->common.drvHwPpWriteVec = (CPSS_DRV_HW_READ_VEC_FUNC) hwPpPexMbusWriteVec;
#endif
    drv->common.drvHwPpIsrRead = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPexMbusIsrRead;
    drv->common.drvHwPpIsrWrite = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPexMbusIsrWrite;
    /* optional */
    drv->common.drvHwPpReadIntPciReg = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPexMbusReadInternalPciReg;
    /* optional */
    drv->common.drvHwPpWriteIntPciReg = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPexMbusWriteInternalPciReg;
    /* optional */
    drv->common.drvHwPpResetAndInitControllerReadReg = (CPSS_DRV_HW_READ_REG_FUNC)hwPpPexMbusResetAndInitCtrlReadRegister;
    /* optional */
    drv->common.drvHwPpResetAndInitControllerWriteReg = (CPSS_DRV_HW_WRITE_REG_FUNC)hwPpPexMbusResetAndInitCtrlWriteRegister;

    drv->baseAddr = hwInfo->baseAddr;
    drv->internalPciBase = hwInfo->internalPciBase;
    drv->resetAndInitControllerBase = hwInfo->resetAndInitControllerBase;

#ifdef ASIC_SIMULATION
    drv->simDevId = (GT_U8)scibGetDeviceId(drv->baseAddr);
    drv->simAddrCompletEnable =
        scibAddressCompletionStatusGet(SIM_DEV_ID(drv));

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].simInfo.devId =
                            (GT_U8)drv->simDevId;
#endif


    cpssOsSprintf (name,"hwComplSem-%d_%ld",devNum,portGroupId);
    cpssOsMutexCreate (name,&(drv->hwComplSem));

    /* ISR bitmap bounds */
    regBmp = regBmpIsr;
    /* find first 1-bit index */
    for (i = 0; ((regBmp & 1) == 0); regBmp = (regBmp >> 1), i++){};
    drv->isrAddrRegionMin = i;
    /* find last 1-bit index */
    for (/**/; (regBmp != 0); regBmp = (regBmp >> 1), i++){};
    drv->isrAddrRegionMax = i - 1;

    /* APP bitmap bounds */
    regBmp = regBmpApp;
    /* find first 1-bit index */
    for (i = 0; ((regBmp & 1) == 0); regBmp = (regBmp >> 1), i++){};
    drv->appAddrRegionMin = i;
    /* find last 1-bit index */
    for (/**/; (regBmp != 0); regBmp = (regBmp >> 1), i++){};
    drv->appAddrRegionMax = i - 1;

    /* init last used regions as MAX regions */
    drv->isrAddrRegionUsed = drv->isrAddrRegionMax;
    drv->appAddrRegionUsed = drv->appAddrRegionMax;


    return GT_OK;
}
