/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpHwPexMbusCntl.c
*
* DESCRIPTION:
*       Prestera driver Hardware read and write functions implementation.
*       Relevant for PEX or MBUS with 8 address comlection regions configured
*       in 8 separate registers.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

/* NOT operator for GT_BOOL values */
#define PRV_BOOL_NOT_MAC(_val) (((_val) == GT_FALSE) ? GT_TRUE : GT_FALSE)

#ifdef ASIC_SIMULATION
#include <asicSimulation/SCIB/scib.h>
/* Macro to convert device ID to simulation device ID */
#define SIM_DEV_ID(dev_num,_portGroupId)     \
    (prvCpssDrvPpConfig[(dev_num)]->hwCtrl[_portGroupId].simInfo.devId)

#define SIM_ADDR_COMPL_EN_GET(dev_num,_portGroupId)  \
    (prvCpssDrvPpConfig[(dev_num)]->hwCtrl[_portGroupId].simInfo.addrCompletEnable)

/* the WM simulation does not support address completion */
/* the GM simulation supports address completion */
#define ADDRESS_COMLETION_ENABLE_MAC(dev_num,_portGroupId)\
    SIM_ADDR_COMPL_EN_GET(dev_num,_portGroupId)
#define ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS 1
#else /*ASIC_SIMULATION*/
/* the HW supports address completion */
#define ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS 0
#endif /*ASIC_SIMULATION*/

/* bits of address passes as is throw PCI window */
#define NOT_ADDRESS_COMPLETION_BITS_NUM_CNS 19
/* bits of address extracted from address completion registers */
#define ADDRESS_COMPLETION_BITS_MASK_CNS    (0xFFFFFFFF << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS)

extern GT_BOOL  prvCpssDrvTraceHwWrite[PRV_CPSS_MAX_PP_DEVICES_CNS];
extern GT_BOOL  prvCpssDrvTraceHwRead[PRV_CPSS_MAX_PP_DEVICES_CNS];

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

/* to exit all functions according to system state */
#define SYSTEM_STATE_CAUSED_SKIP_MAC                                                               \
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&          \
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) ) \
    {                                                                                              \
        /* we are in HSU or HA process -- hardware should not be affected */                       \
        return GT_OK;                                                                              \
    }

#if (ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS == 0)
#define CPU_MEMORY_BASE_ADDR_MAC(deviceId, portGroupId) \
    prvCpssDrvPpConfig[deviceId]->hwCtrl[portGroupId].baseAddr
#else
#define CPU_MEMORY_BASE_ADDR_MAC(deviceId, portGroupId)                \
    ((ADDRESS_COMLETION_ENABLE_MAC(deviceId, portGroupId) == GT_FALSE) \
    ? 0 : prvCpssDrvPpConfig[deviceId]->hwCtrl[portGroupId].baseAddr)
#endif


#ifdef USE_PUMA_LPM_DWORD
#ifndef ASIC_SIMULATION
#define HW_SUPPORT_PUMA_LPM_DWORD
/*  Address range of PUMA's LPM SRAM - used in hwWrite */
#define PUMA_LPM_SRAM_START     0x50000000
#define PUMA_LPM_SRAM_END       0x90000000
#endif /*no ASIC_SIMULATION*/
#endif /*USE_PUMA_LPM_DWORD*/


/*******************************************************************************
*   cpuReadMemoryWord
*
* DESCRIPTION:
*       Read memory from device.
*
* INPUTS:
*       deviceId    - ID of device, which is equal to PSS Core API device ID.
*       portGroupId - port Group Id
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
    IN  GT_U32        deviceId,
    IN  GT_U32        portGroupId,
    IN  GT_U32        memAddr,
    OUT GT_U32        *dataPtr
)
{
    GT_U32     temp;    /* temp word */
    GT_UINTPTR address; /* address adjusted to window */

    address = CPU_MEMORY_BASE_ADDR_MAC(deviceId, portGroupId) + memAddr;
#ifdef ASIC_SIMULATION
    scibReadMemory(SIM_DEV_ID(deviceId, portGroupId), address, 1/*length*/, &temp);
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
*       deviceId    - ID of device, which is equal to PSS Core API device ID.
*       portGroupId - port Group Id
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
    IN  GT_U32        deviceId,
    IN  GT_U32        portGroupId,
    IN  GT_U32        memAddr,
    OUT GT_U32        data
)
{
    GT_U32     leData;  /*little endian data*/
    GT_UINTPTR address; /* address adjusted to window */

    address = CPU_MEMORY_BASE_ADDR_MAC(deviceId, portGroupId) + memAddr;
    leData = CPSS_32BIT_LE(data);
#ifdef ASIC_SIMULATION
    scibWriteMemory(SIM_DEV_ID(deviceId, portGroupId), address, 1/*length*/, &leData);
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
*       deviceId    - ID of device, which is equal to PSS Core API device ID.
*       portGroupId - port Group Id
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
    IN  GT_U32        deviceId,
    IN  GT_U32        portGroupId,
    IN  GT_U32        memAddr,
    IN  GT_U32        length,
    OUT GT_U32        *dataPtr
)
{
    GT_U32     temp;    /* temp word       */
    GT_U32     i;       /* loop index      */
    GT_UINTPTR address; /* address adjusted to window */
    GT_UINTPTR base;    /* address adjusted to window */

    base = CPU_MEMORY_BASE_ADDR_MAC(deviceId, portGroupId) + memAddr;
    for (i = 0; (i < length); i++)
    {
        address = base + (4 * i);
#ifdef ASIC_SIMULATION
        scibReadMemory(SIM_DEV_ID(deviceId, portGroupId), address, 1/*length*/, &temp);
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
*       deviceId    - ID of device, which is equal to PSS Core API device ID.
*       portGroupId - port Group Id
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
    IN  GT_U32        deviceId,
    IN  GT_U32        portGroupId,
    IN  GT_U32        memAddr,
    IN  GT_U32        length,
    OUT GT_U32        *dataPtr
)
{
    GT_U32     leData;  /* little endian data */
    GT_U32     i;       /* loop index         */
    GT_UINTPTR address; /* address adjusted to window */
    GT_UINTPTR base;    /* address adjusted to window */

    base = CPU_MEMORY_BASE_ADDR_MAC(deviceId, portGroupId) + memAddr;
    for (i = 0; (i < length); i++)
    {
        address = base + (4 * i);
        leData = CPSS_32BIT_LE(dataPtr[i]);
#ifdef ASIC_SIMULATION
        scibWriteMemory(SIM_DEV_ID(deviceId, portGroupId), address, 1/*length*/, &leData);
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
*   cpuWriteMemoryArrayInReverse
*
* DESCRIPTION:
*       Write memory from device in reverce order.
*
* INPUTS:
*       deviceId    - ID of device, which is equal to PSS Core API device ID.
*       portGroupId - port Group Id
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
static void cpuWriteMemoryArrayInReverse
(
    IN  GT_U32        deviceId,
    IN  GT_U32        portGroupId,
    IN  GT_U32        memAddr,
    IN  GT_U32        length,
    OUT GT_U32        *dataPtr
)
{
    GT_U32     leData;  /* little endian data */
    GT_U32     i,j;      /* loop index         */
    GT_UINTPTR address; /* address adjusted to window */
    GT_UINTPTR base;    /* address adjusted to window */

    base = CPU_MEMORY_BASE_ADDR_MAC(deviceId, portGroupId) + memAddr;
    for (j = 0; (j < length); j++)
    {
        i = length - 1 - j;
        address = base + (4 * i);
        leData = CPSS_32BIT_LE(dataPtr[i]);
#ifdef ASIC_SIMULATION
        scibWriteMemory(SIM_DEV_ID(deviceId, portGroupId), address, 1/*length*/, &leData);
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
*       devNum  - The device number to set the address completion value for.
*       portGroupId  - The port group Id.
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
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
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
    PRV_CPSS_DRV_STRUCT_PP_PORT_GROUP_HW_STC *hwCtrlPtr; /* pointer to dev+portGroup Control */

    hwCtrlPtr = &(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId]);

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
        regBmp      = hwCtrlPtr->isrAddrCompletionRegionsBmp;
        regUsedPtr  = &(hwCtrlPtr->isrAddrRegionUsed);
        regMin      = hwCtrlPtr->isrAddrRegionMin;
        regMax      = hwCtrlPtr->isrAddrRegionMax;
    }
    else
    {
        /*CPSS API*/
        *signalSem  = GT_TRUE;
        regBmp      = hwCtrlPtr->appAddrCompletionRegionsBmp;
        regUsedPtr  = &(hwCtrlPtr->appAddrRegionUsed);
        regMin      = hwCtrlPtr->appAddrRegionMin;
        regMax      = hwCtrlPtr->appAddrRegionMax;
    }

    if (*signalSem == GT_TRUE)
    {
        cpssOsMutexLock(hwCtrlPtr->hwComplSem);
    }

    addrRegion = (regAddr >> NOT_ADDRESS_COMPLETION_BITS_NUM_CNS);

    /* Compare addr region to existing Address regions. */
    compIdx = 0xFFFF; /* wrong value as stamp */
    for (i = regMin; (i <= regMax); i++)
    {
        /* bypass not assigned regions */
        if ((regBmp & (1 << i)) == 0)
            continue;

        if (addrRegion == hwCtrlPtr->addrCompletShadow[i])
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
        cpuWriteMemoryWord(devNum, portGroupId, address, addrRegion);
        /* save configuration */
        hwCtrlPtr->addrCompletShadow[compIdx] = addrRegion;
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
*       devNum       - The PP dev Num to read from.
*       portGroupId  - The port group Id.
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
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
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

#if (ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS != 0)
    if (ADDRESS_COMLETION_ENABLE_MAC(devNum, portGroupId) == GT_FALSE)
    {
        cpuReadMemoryArray(
            devNum, portGroupId, regAddr, length, dataPtr);

        if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
        {
            return cpssTraceHwAccessRead(devNum,
                                         portGroupId,
                                         PRV_BOOL_NOT_MAC(appAccess), /* ISR context */
                                         CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* PCI/PEX space */
                                         regAddr,
                                         length, /* length to trace the data in words */
                                         dataPtr);
        }

        return GT_OK;
    }
#endif /*ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS*/

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
            rc = hwRead(
                devNum, portGroupId, regAddr, lenInReg, appAccess, dataPtr);
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

    rc = setAddrCompletion(
        devNum, portGroupId, regAddr, appAccess,
        &compIdx, &address, &signalSem);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpuReadMemoryArray(
        devNum, portGroupId, address, length, dataPtr);

    if (signalSem != GT_FALSE)
    {
        cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
    }

    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        return cpssTraceHwAccessRead(devNum,
                                     portGroupId,
                                     PRV_BOOL_NOT_MAC(appAccess), /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* PCI/PEX space */
                                     regAddr,
                                     length, /* length to trace the data in words */
                                     dataPtr);
    }

    return GT_OK;
}

#ifdef HW_SUPPORT_PUMA_LPM_DWORD
/*******************************************************************************
*  hwWriteCopletedDouble
*
* DESCRIPTION:
*      This function receives address and data pointer and write the data to the PP
*      Write width is 64 bit if it's possible
*
* INPUTS:
*       devNum  - The PP dev Num to read from.
*       portGroupId  - The port group Id.
*       completedAddress - The register's address with address completion bits.
*       length  - Nubmber of writes in 4 byte each.
*       dataPtr - Pointer to data array.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwWriteCopletedDouble
(
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
    IN  GT_U32      completedAddress,
    IN  GT_U32      length,
    IN  GT_U32      *dataPtr
)
{
    GT_U32      lastWord;
    GT_U32      index;
    GT_U32      address;
    GT_UINTPTR  pciAddress;

    address = completedAddress;

    /* save pointer to the last word  in case that 'length' will be changed */
    lastWord = dataPtr[length-1];

    /* write 32-bit prefix, if exists  - address should be 64-bit aligned */
    if (address & 0x7)
    {
        cpuWriteMemoryWord(devNum, portGroupId, address, dataPtr[0]);
        address +=4;
        length -=1;
    }

    /* 64 bit writes - data divided to two 32-bit words to avoid compiler issues. Swap should be done since we are writing to PCI/PEX */
    for(index = 0;index < (length & 0xFFFFFFFE);index+=2,address+=8)
    {
        pciAddress = address + prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr;
        cpssExtDrvPciDoubleWrite(address, CPSS_32BIT_LE(dataPtr[index]), CPSS_32BIT_LE(dataPtr[index+1]));
    }

    /* write 32-bit postfix, if exists  - check if length is odd */
    if (length & 0x1)
    {
        cpuWriteMemoryWord(devNum, portGroupId, address, lastWord);
    }

    return GT_OK;
}
#endif /* HW_SUPPORT_PUMA_LPM_DWORD */

/*******************************************************************************
*  hwWrite
*
* DESCRIPTION:
*      This function receives address and data pointer and write the data to
*      the PP.
* INPUTS:
*       devNum       - The PP dev Num to read from.
*       portGroupId  - The port group Id.
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
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
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
#ifdef HW_SUPPORT_PUMA_LPM_DWORD
    GT_BOOL pumaLpmSram = GT_FALSE; /* for Puma - if destination is LPM SRAM */
#endif

    SYSTEM_STATE_CAUSED_SKIP_MAC;

    debugTrace(("Write: addr 0x%X len 0x%X data 0x%X \n", regAddr, length, *dataPtr));

#if (ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS != 0)
    if (ADDRESS_COMLETION_ENABLE_MAC(devNum, portGroupId) == GT_FALSE)
    {
        cpuWriteMemoryArray(
            devNum, portGroupId, regAddr, length, dataPtr);

        if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
        {
            return cpssTraceHwAccessWrite(devNum,
                                         portGroupId,
                                         PRV_BOOL_NOT_MAC(appAccess), /* ISR context */
                                         CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* PCI/PEX space */
                                         regAddr,
                                         length, /* length to trace the data in words */
                                         dataPtr);
        }

        return GT_OK;
    }
#endif /*ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS*/

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
            rc = hwWrite(
                devNum, portGroupId, regAddr, lenInReg, appAccess, dataPtr);
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

    rc = setAddrCompletion(
        devNum, portGroupId, regAddr, appAccess,
        &compIdx, &address, &signalSem);
    if (rc != GT_OK)
    {
        return rc;
    }

#ifdef HW_SUPPORT_PUMA_LPM_DWORD
    /* Check if destination is within LPM SRAM space  of PUMA */
    if (CPSS_IS_EXMXPM_FAMILY_MAC(prvCpssDrvPpConfig[devNum]->devFamily))
    {
        if((regAddr >= PUMA_LPM_SRAM_START) && (regAddr < PUMA_LPM_SRAM_END))
        {
            pumaLpmSram = GT_TRUE;
        }
    }

    if (pumaLpmSram == GT_FALSE)
    {
#endif /*HW_SUPPORT_PUMA_LPM_DWORD*/
        cpuWriteMemoryArray(
            devNum, portGroupId, address, length, dataPtr);
#ifdef HW_SUPPORT_PUMA_LPM_DWORD
    }
    else
    {
        hwWriteCopletedDouble(
            devNum, portGroupId, address, length, dataPtr);
    }
#endif /*HW_SUPPORT_PUMA_LPM_DWORD*/

    if (signalSem != GT_FALSE)
    {
        cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
    }

    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        return cpssTraceHwAccessWrite(devNum,
                                     portGroupId,
                                     PRV_BOOL_NOT_MAC(appAccess), /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* PCI/PEX space */
                                     regAddr,
                                     length, /* length to trace the data in words */
                                     dataPtr);
    }

    return GT_OK;
}

/*******************************************************************************
*  hwWriteInReverse
*
* DESCRIPTION:
*      This function receives address and data pointer and perform reverse
*      write of the data to the PP.
* INPUTS:
*       devNum       - The PP dev Num to read from.
*       portGroupId  - The port group Id.
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
static GT_STATUS hwWriteInReverse
(
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_BOOL     appAccess,
    IN  GT_U32      *dataPtr
)
{
    GT_STATUS rc;            /* return code                             */
    GT_U8     compIdx;       /* address completion register field index */
    GT_U32    address;       /* physical access address for PCI access  */
    GT_BOOL   signalSem;     /* is needed to unlock semaphore           */
    GT_U32    lenInReg;      /* length of the part in the same region   */
    GT_U32    regAddrLast;   /* last register address in region         */
    GT_U32    regAddrFirst;  /* first register address in region        */
    GT_U32    *dataPtrFirst; /* first data word to write in region      */

    SYSTEM_STATE_CAUSED_SKIP_MAC;

    debugTrace(("Write: addr 0x%X len 0x%X data 0x%X \n", regAddr, length, *dataPtr));

#if (ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS != 0)
    if (ADDRESS_COMLETION_ENABLE_MAC(devNum, portGroupId) == GT_FALSE)
    {
        cpuWriteMemoryArrayInReverse(
            devNum, portGroupId, regAddr, length, dataPtr);

        if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
        {
            return cpssTraceHwAccessWrite(devNum,
                                         portGroupId,
                                         PRV_BOOL_NOT_MAC(appAccess), /* ISR context */
                                         CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* PCI/PEX space */
                                         regAddr,
                                         length, /* length to trace the data in words */
                                         dataPtr);
        }

        return GT_OK;
    }
#endif /*ADDRESS_COMLETION_DISABLE_SUPPORTED_CNS*/

    /* support the region bound crossing address space by splitting to recursive calls */
    if (((regAddr + (4 * (length - 1))) & ADDRESS_COMPLETION_BITS_MASK_CNS)
        != (regAddr & ADDRESS_COMPLETION_BITS_MASK_CNS))
    {
        while (1)
        {
            regAddrLast  = regAddr + (4 * (length - 1));
            regAddrFirst = (regAddrLast & ADDRESS_COMPLETION_BITS_MASK_CNS);
            if (regAddr > regAddrFirst)
            {
                regAddrFirst = regAddr;
            }

            lenInReg = ((regAddrLast - regAddrFirst) / 4) + 1;

            dataPtrFirst = dataPtr + (length - lenInReg);

            /* write part in current region */
            rc = hwWriteInReverse(
                devNum, portGroupId, regAddrFirst, lenInReg, appAccess, dataPtrFirst);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* decrement length */
            length  -= lenInReg;

            if (length == 0)
                break;
        }

        return GT_OK;
    }

    rc = setAddrCompletion(
        devNum, portGroupId, regAddr, appAccess,
        &compIdx, &address, &signalSem);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpuWriteMemoryArrayInReverse(
        devNum, portGroupId, address, length, dataPtr);

    if (signalSem != GT_FALSE)
    {
        cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
    }

    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        return cpssTraceHwAccessWrite(devNum,
                                     portGroupId,
                                     PRV_BOOL_NOT_MAC(appAccess), /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* PCI/PEX space */
                                     regAddr,
                                     length, /* length to trace the data in words */
                                     dataPtr);
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpReadBurst
*
* DESCRIPTION:
*      This function performs a burst read from a given device's Ram address.
*
* INPUTS:
*       devNum          - The PP device number to read from.
*       portGroupId     - The port group Id.
*       srcAddr         - The source address to read from.
*       length          - Number of 4 byte words to read.
*       ramBurstInfo    - Holds data regarding the burst to be performed on the
*                         Ram.
*
* OUTPUTS:
*       data            - Pointer to data array holding the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpReadBurst
(
    IN  GT_U8                             devNum,
    IN GT_U32                             portGroupId,
    IN  GT_U32                            srcAddr,
    IN  GT_U32                            length,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo,
    OUT GT_U32                            *data
)
{
    GT_U8       addrCompIdx;    /* Index of the address completion byte to  */
                                /* be used for this pci access.             */
    GT_U32      complAddr;      /* The address to be used for pci access,   */
                                /* after setting the address completion     */
                                /* register.                                */
    GT_BOOL     signalSem;      /* Should the semaphore be signaled after   */
                                /* the burst operation.                     */
    GT_U32      tempBurstBuff[PRV_CPSS_DRV_MAX_BURST_BLOCK_SIZE_CNS];/* A temporary buffer   */
    GT_U32      *tempData;      /* used to hold the data to be written in   */
                                /* case a read-modify write operation is    */
                                /* needed.                                  */
    GT_U32      words2Allign;   /* Number of additional words need to be    */
                                /* read to reach the needed alignment for  */
                                /* this Ram.                                */
    GT_U32      allignSrcAddr;  /* The aligned source address.             */
    GT_U32      allignSize;     /* The actual size to be read as a result   */
                                /* of the alignment.                       */
    GT_U32      *tempDstAddr;   /* Used when multiple burst are needed to   */
    GT_U32      tempSrcAddr;    /* perform the read operation, i.e addrRes- */
                                /* -olution != 1.                           */
    GT_U32      numOfBursts;    /* Number of bursts to be performed, used   */
                                /* when addrResolution != 1.                */
    GT_U32      sizeOfBurst;    /* Size of each burst.                      */
    GT_U32      i;
    GT_STATUS   retVal;
#ifdef ASIC_SIMULATION
    GT_UINTPTR  base;

    base = CPU_MEMORY_BASE_ADDR_MAC(devNum, portGroupId);
#endif

    /* Set the destination address to an aligned address   */
    words2Allign    = (srcAddr / sizeof(GT_U32)) % ramBurstInfo->burstSize;
    allignSrcAddr   = srcAddr - (words2Allign * sizeof(GT_U32));
    allignSize      = length + words2Allign;

    if((allignSize % ramBurstInfo->burstSize) != 0)
    {
        allignSize += (ramBurstInfo->burstSize -
                       (allignSize % ramBurstInfo->burstSize));
    }

    /* Set the source address according to the          */
    /* required address resolution.                     */
    allignSrcAddr   = (((allignSrcAddr & (~ramBurstInfo->ramBaseMask)) *
                        ramBurstInfo->addrResolution) +
                       (allignSrcAddr & ramBurstInfo->ramBaseMask));

    /* Set the address completion register. */
    retVal = setAddrCompletion(
        devNum,portGroupId,allignSrcAddr, GT_TRUE /*appAccess*/,
        &addrCompIdx, &complAddr, &signalSem);
    if(retVal != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    if(ramBurstInfo->addrResolution != 1)
    {
        sizeOfBurst = ramBurstInfo->burstSize;
    }
    else
    {
        sizeOfBurst = allignSize;
    }
    numOfBursts = allignSize / sizeOfBurst;

    if(allignSize != length)
    {
        tempData = tempBurstBuff;
    }
    else
    {
        tempData        = data;
    }

#ifdef ASIC_SIMULATION
    for(i = 0; i < numOfBursts; i++)
    {
        tempDstAddr = (GT_U32*)(((GT_UINTPTR)tempData) + (i * sizeOfBurst *
                                                      sizeof(GT_U32)));
        tempSrcAddr = (allignSrcAddr +
                       (i * sizeOfBurst * ramBurstInfo->addrResolution *
                        sizeof(GT_U32)));

        scibReadMemory(
            SIM_DEV_ID(devNum,portGroupId),
            (base + tempSrcAddr), sizeOfBurst, tempDstAddr);
    }
#else /*ASIC_SIMULATION*/
    /* Update allignSrcAddr with the address completion part.   */
    allignSrcAddr = complAddr +
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr;

    for(i = 0; i < numOfBursts; i++)
    {
        tempDstAddr = (GT_U32*)(((GT_UINTPTR)tempData) + (i * sizeOfBurst *
                                                      sizeof(GT_U32)));
        tempSrcAddr = (allignSrcAddr +
                       (i * sizeOfBurst * ramBurstInfo->addrResolution *
                        sizeof(GT_U32)));

        retVal = cpssExtDrvDmaRead(tempSrcAddr,sizeOfBurst,ramBurstInfo->burstSize,
                               tempDstAddr);
        if(retVal != GT_OK)
        {
            if(signalSem == GT_TRUE)
            {
                cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
            }
            return retVal;
        }
    }
#endif /*ASIC_SIMULATION*/

    /* Copy the appropriate part of 'tempData' into  'data',    */
    /* according to words2Allign Value.                         */
    for(i = 0; i < length; i++)
    {
        data[i] = CPSS_32BIT_LE(tempData[i + words2Allign]);
    }
    if(memoryAccessTraceOn == GT_TRUE)
    {
        for(i = 0; i < allignSize; i++)
        {
            debugPrint(("data[%d] - 0x%x.\n",i,data[i]));
        }
    }

    /* Free the address completion semaphore (if needed).       */
    if(signalSem == GT_TRUE)
    {
        cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
    }

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        /* trace HW read access */
        retVal = cpssTraceHwAccessRead(devNum,
                                       portGroupId,
                                       GT_FALSE, /* ISR context */
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* PCI/PEX space */
                                       allignSrcAddr,
                                       allignSize, /* length to trace the data in words */
                                       tempData);
        if (retVal != GT_OK)
        {
            return retVal;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpWriteBurst
*
* DESCRIPTION:
*      This function performs a burst write to a given device's Ram address.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       portGroupId  - The port group Id.
*       destAddr        - The destination address to write to.
*       length          - Number of 4 byte words to write.
*       data            - Pointer to data array to be written.
*       ramBurstInfo    - Holds data regarding the burst to be performed on the
*                         Ram.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       If the given destination address is not a multiple of burstSize, then
*       a read - modify write is performed on the Ram.
*
*******************************************************************************/
static GT_STATUS hwPpWriteBurst
(
    IN  GT_U8                             devNum,
    IN GT_U32                             portGroupId,
    IN  GT_U32                            destAddr,
    IN  GT_U32                            length,
    IN  GT_U32                            *data,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo
)
{
    GT_U8       addrCompIdx;    /* Index of the address completion byte to  */
                                /* be used for this pci access.             */
    GT_U32      complAddr;      /* The address to be used for pci access,   */
                                /* after setting the address completion     */
                                /* register.                                */
    GT_BOOL     signalSem;      /* Should the semaphore be signaled after   */
                                /* the burst operation.                     */

    GT_U32      tempBurstBuff[PRV_CPSS_DRV_MAX_BURST_BLOCK_SIZE_CNS];/* A temporary buffer   */
    GT_U32      *tempData;      /* used to hold the data to be written in   */
                                /* case a read-modify write operation is    */
                                /* needed.                                  */
    GT_U32      words2Allign;   /* Number of additional words need to be    */
                                /* read to reach the needed alignment for  */
                                /* this Ram.                                */
    GT_U32      allignDestAddr; /* The aligned destination address.        */
    GT_U32      allignSize;     /* The actual size to be read as a result   */
                                /* of the alignment.                       */
    GT_U32      *tempSrcAddr;   /* Used when multiple burst are needed to   */
    GT_U32      tempDestAddr;   /* perform the write operation, i.e addrRes-*/
                                /* -olution != 1.                           */
    GT_U32      numOfBursts;    /* Number of bursts to be performed, used   */
                                /* when addrResolution != 1.                */
    GT_U32      sizeOfBurst;    /* Size of each burst.                      */
    GT_U32      i, t;
    GT_STATUS   retVal = GT_OK;
    GT_UINTPTR  base;

    base = CPU_MEMORY_BASE_ADDR_MAC(devNum, portGroupId);

    /* If the buffer to be written is larger than PRV_CPSS_DRV_MAX_BURST_BLOCK_SIZE_CNS, */
    /* split this write to multiple writes.                             */
    /* Take any possible needed alignments into account.               */
    if(length > (PRV_CPSS_DRV_MAX_BURST_BLOCK_SIZE_CNS - PRV_CPSS_DRV_MAX_BURST_EXTRA_ALLIGN_CNS))
    {
        sizeOfBurst = PRV_CPSS_DRV_MAX_BURST_BLOCK_SIZE_CNS - PRV_CPSS_DRV_MAX_BURST_EXTRA_ALLIGN_CNS;
        t = 0;
        for(i = 0; i < length / sizeOfBurst; i++)
        {
            retVal = hwPpWriteBurst(devNum,portGroupId,destAddr + (i * sizeOfBurst * 4),sizeOfBurst,
                           data + (i * sizeOfBurst),ramBurstInfo);
            if(retVal != GT_OK)
                return retVal;
            t++;
        }

        /* write remainder */
        if(length%sizeOfBurst != 0)
        {
            retVal = hwPpWriteBurst(devNum,portGroupId,
                                    destAddr + (t * sizeOfBurst * 4),
                                    length%sizeOfBurst,
                                    data + (t * sizeOfBurst),ramBurstInfo);
        }
        return retVal;
    }

    /* Set the destination address to an aligned address   */
    words2Allign    = (destAddr / sizeof(GT_U32)) % ramBurstInfo->burstSize;
    allignDestAddr  = destAddr - (words2Allign * sizeof(GT_U32));
    allignSize      = length + words2Allign;

    /* Check if the end of the block is properly aligned.  */
    if((allignSize % ramBurstInfo->burstSize) != 0)
    {
        allignSize += (ramBurstInfo->burstSize -
                       (allignSize % ramBurstInfo->burstSize));
    }

    /* Set the destination address according to the     */
    /* required address resolution.                     */
    allignDestAddr = (((allignDestAddr & (~ramBurstInfo->ramBaseMask)) *
                       ramBurstInfo->addrResolution) +
                      (allignDestAddr & ramBurstInfo->ramBaseMask));

    /* Set the address completion register. */
    retVal = setAddrCompletion(
        devNum,portGroupId,allignDestAddr,GT_TRUE /*appAccess*/,
        &addrCompIdx, &complAddr, &signalSem);
    if(retVal != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if(ramBurstInfo->addrResolution != 1)
    {
        sizeOfBurst = ramBurstInfo->burstSize;
    }
    else
    {
        sizeOfBurst = allignSize;
    }
    numOfBursts = allignSize / sizeOfBurst;

    /* Update allignDstAddr with the address completion part.   */
    allignDestAddr = complAddr + base;

    if(allignSize != length)
    {
        tempData = tempBurstBuff;

        for(i = 0; i < numOfBursts; i++)
        {
            tempSrcAddr =
                (GT_U32*)(((GT_UINTPTR)tempData) + (i * sizeOfBurst *
                                                sizeof(GT_U32)));
            tempDestAddr= (allignDestAddr +
                           (i * sizeOfBurst * ramBurstInfo->addrResolution *
                            sizeof(GT_U32)));

#ifdef ASIC_SIMULATION
            scibReadMemory(
                SIM_DEV_ID(devNum,portGroupId),
                tempDestAddr, sizeOfBurst, tempSrcAddr);
            retVal = GT_OK;
#else /*ASIC_SIMULATION*/
            retVal = cpssExtDrvDmaRead(tempDestAddr,sizeOfBurst,
                                   ramBurstInfo->burstSize,tempSrcAddr);
#endif /*ASIC_SIMULATION*/
            if(retVal != GT_OK)
            {
                if(signalSem == GT_TRUE)
                {
                    cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
                }
                return retVal;
            }
        }
    }
    else
    {
        tempData        = data;
    }

    /* Copy 'data' into 'tempData', according to words2Allign Value */
    for(i = 0; i < length; i++)
    {
        tempData[i + words2Allign] = CPSS_32BIT_LE(data[i]);
    }
    if(memoryAccessTraceOn == GT_TRUE)
    {
        for(i = 0; i < allignSize; i++)
        {
            debugPrint(("tempData[%d] - 0x%x.\n",i,tempData[i]));
        }
    }
    /* Now write the whole data back into the Ram.              */
    for(i = 0; i < numOfBursts; i++)
    {
        tempSrcAddr =
            (GT_U32*)(((GT_UINTPTR)tempData) + (i * ramBurstInfo->burstSize *
                                            sizeof(GT_U32)));
        tempDestAddr= (allignDestAddr +
                       (i * ramBurstInfo->burstSize *
                        ramBurstInfo->addrResolution * sizeof(GT_U32)));

#ifdef ASIC_SIMULATION
            scibWriteMemory(
                SIM_DEV_ID(devNum,portGroupId),
                tempDestAddr, sizeOfBurst, tempSrcAddr);
            retVal = GT_OK;
#else /*ASIC_SIMULATION*/
            retVal = cpssExtDrvDmaWrite(tempDestAddr,tempSrcAddr,sizeOfBurst,
                                    ramBurstInfo->burstSize);
#endif /*ASIC_SIMULATION*/
        if(retVal != GT_OK)
        {
            if(signalSem == GT_TRUE)
            {
                cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
            }
            return retVal;
        }
    }

    /* Restore the value of 'data', if it was byte swapped. */
    if(tempData == data)
    {
        for(i = 0; i < length; i++)
        {
            data[i] = CPSS_32BIT_LE(data[i]);
        }
    }

    /* Free the address completion semaphore (if needed).       */
    if(signalSem == GT_TRUE)
    {
        cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
    }

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* trace HW write access */
        retVal = cpssTraceHwAccessWrite(devNum,
                                        portGroupId,
                                        GT_FALSE, /* ISR context */
                                        CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* PCI/PEX space */
                                        allignDestAddr,
                                        allignSize, /* length to trace the data in words */
                                        data);
        if (retVal != GT_OK)
        {
            return retVal;
        }
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
*       devNum  - The PP to read from.
*       portGroupId  - The port group Id.
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
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    debugPrint(("Read from dev %d, reg 0x%x,",devNum, regAddr));
    hwRead (devNum, portGroupId, regAddr, 1, GT_TRUE /*appAccess*/, data);
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
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id.
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
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
)
{
    debugPrint(
        ("Write to device %d, reg 0x%x, data 0x%x.\n",devNum,regAddr,
        value));

    hwWrite(devNum, portGroupId, regAddr, 1, GT_TRUE /*appAccess*/, &value);

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusGetRegField
*
* DESCRIPTION:
*       Read a selected register field.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPexMbusGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)
{
    GT_U32 data = 0, mask;

    debugPrint(("GetField from dev %d, reg 0x%x, offset %d, length %d,",
                devNum, regAddr, fieldOffset, fieldLength));

    hwRead(devNum, portGroupId, regAddr, 1, GT_TRUE /*appAccess*/, &data);
    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    *fieldData = (GT_U32)((data & mask) >> fieldOffset);

    debugPrint(("data 0x%x.\n", *fieldData));

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusSetRegField
*
* DESCRIPTION:
*       Write value to selected register field.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
static GT_STATUS hwPpPexMbusSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
)
{
    GT_U32 data = 0, mask;

    debugPrint(("SetField to dev %d, reg 0x%x, offset %d, length %d, \
        data 0x%x.\n",devNum, regAddr, fieldOffset, fieldLength, fieldData));

    hwRead(devNum, portGroupId, regAddr, 1, GT_TRUE /*appAccess*/, &data);
    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    data &= ~mask;   /* turn the field off */
    /* insert the new value of field in its place */
    data |= ((fieldData << fieldOffset) & mask);

    hwWrite(devNum, portGroupId, regAddr, 1, GT_TRUE /*appAccess*/, &data);

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to read from.
*       portGroupId  - The port group Id.
*       regAddr - Register address to read from.
*       mask    - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
*******************************************************************************/
static GT_STATUS hwPpPexMbusReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    GT_U32 data = 0;

    debugPrint(("\nhwPpReadRegBitMask on device %d, from register 0x%x, \
                mask 0x%x,", devNum, regAddr, mask));

    hwRead(devNum, portGroupId, regAddr, 1, GT_TRUE /*appAccess*/, &data);
    *dataPtr = (data) & mask;

    debugPrint((" data is 0x%x.\n", *dataPtr));
    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to write to.
*       portGroupId  - The port group Id.
*       regAddr - Register address to write to.
*       mask    - Mask for selecting the written bits.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
static GT_STATUS hwPpPexMbusWriteRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    IN GT_U32   value
)

{
    GT_U32 data = 0;

    debugPrint(("\nhwPpWriteRegBitMask on device %d, from register 0x%x, \
                mask 0x%x, data is 0x%x.\n",
                devNum, regAddr, mask, value));

    /* in case mask = 0xffffffff call hwWrite only */
    if(mask == 0xffffffff)
    {
        hwWrite(devNum, portGroupId, regAddr, 1, GT_TRUE /*appAccess*/, &value);
        return GT_OK;
    }
    /* else read modify write */
    hwRead(devNum, portGroupId,regAddr, 1, GT_TRUE /*appAccess*/, &data);
    data &= (~mask);            /* Turn the field off.                        */
    data |= (value & mask);     /* Insert the new value of field in its place.*/

    hwWrite(devNum, portGroupId, regAddr, 1, GT_TRUE /*appAccess*/, &data);

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusReadRam
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id.
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
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
)
{
    PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo;  /* Holds the devices ram burst  */
                                        /* parameters.                  */
    GT_U32               i;

    for( i = 0; i < length; i++)
    {
        debugPrint(("\nhwPpReadRam on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addr + 4 * i, data[i]));
    }

    ramBurstInfo = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo;
    for(i = 0; i < prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfoLen; i++)
    {
        if(ramBurstInfo[i].ramBase == (ramBurstInfo[i].ramBaseMask & addr))
            return hwPpReadBurst(devNum,portGroupId,addr,length,&(ramBurstInfo[i]),data);
    }

    hwRead(devNum, portGroupId,addr, length, GT_TRUE /*appAccess*/, data);

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
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
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
)
{
    GT_STATUS           retVal;
    PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo;  /* Holds the devices ram burst  */
                                        /* parameters.                  */
    GT_U32              i;
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    if(memoryAccessTraceOn == GT_TRUE)
    {
        for( i = 0; i < length; i++)
        {
          debugPrint(("\nhwPpWriteRam on device %d, from address 0x%x, \
                       data is 0x%x.\n",devNum, addr + i * 4, data[i]));
        }
    }
    ramBurstInfo = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo;
    for(i = 0; i < prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfoLen; i++)
    {
        if(ramBurstInfo[i].ramBase == (ramBurstInfo[i].ramBaseMask & addr))
        {
            retVal =  hwPpWriteBurst(devNum,portGroupId,addr,length,data,
                                     &(ramBurstInfo[i]));
            return retVal;
        }
    }

    hwWrite(devNum, portGroupId,addr, length, GT_TRUE /*appAccess*/, data);
    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusReadVec
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id.
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
static GT_STATUS hwPpPexMbusReadVec
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen
)
{
    GT_U32      i;
    GT_STATUS   rc;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwRead(
            devNum, portGroupId, addrArr[i], 1, GT_TRUE /*appAccess*/, &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
        debugPrint(("\nhwPpPexMbusReadVec on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addrArr[i], dataArr[i]));
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpPexMbusWriteVec
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
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
static GT_STATUS hwPpPexMbusWriteVec
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
)
{
    GT_U32      i;
    GT_STATUS   rc;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwWrite(
            devNum, portGroupId,addrArr[i], 1, GT_TRUE /*appAccess*/, &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
        debugPrint(("\nhwPpPexMbusWriteVec on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addrArr[i], dataArr[i]));
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusWriteRamInReverse
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
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
static GT_STATUS hwPpPexMbusWriteRamInReverse
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    GT_STATUS           status;
    GT_U32              i = 0;
    if(memoryAccessTraceOn == GT_TRUE)
    {
        for( i = 0; i < length; i++)
        {
           debugPrint(("\nhwPpWriteRam on device %d, from address 0x%x, \
                        data is 0x%x.\n",devNum, addr + i * 4, data[i]));
        }
    }
    status = hwWriteInReverse(
        devNum, portGroupId,addr, length, GT_TRUE /*appAccess*/, data);
    return status;
}

/*******************************************************************************
* hwPpPexMbusIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - The port group Id.
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
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   *dataPtr
)
{
    debugPrint(("Read from dev %d, reg 0x%x,",devNum, regAddr));
    hwRead(devNum, portGroupId, regAddr, 1, GT_FALSE /*appAccess*/, dataPtr);
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
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id.
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
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
)
{
    debugPrint(
        ("Write to device %d, reg 0x%x, data 0x%x.\n",devNum,regAddr,
        value));

    hwWrite(devNum, portGroupId, regAddr, 1, GT_FALSE /*appAccess*/, &value);

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
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id.
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
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{

    GT_U32  temp;

#ifndef ASIC_SIMULATION
    GT_UINTPTR address;     /* physical access address for PCI transaction. */
    address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase + regAddr);
#endif

#ifdef ASIC_SIMULATION
    scibPciRegRead(SIM_DEV_ID(devNum,portGroupId), regAddr, 1, &temp);
#else /*ASIC_SIMULATION*/
    temp = *((volatile GT_U32 *)address);
    GT_SYNC; /*to avoid read combining */
#endif /*ASIC_SIMULATION*/


    *data = CPSS_32BIT_LE(temp);

    debugTrace(("PCI Read: addr 0x%X data 0x%X \n", regAddr, *data));

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        /* Check if trace hw read is enabled */
        return cpssTraceHwAccessRead(devNum,
                                     portGroupId,
                                     GT_FALSE,  /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E,   /* PCI/PEX space */
                                     regAddr,
                                     1,    /* length to trace the data in words */
                                     data);
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpPexMbusWriteInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to read from.
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
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_U32     leData;      /* LE data */

#ifndef ASIC_SIMULATION
    GT_UINTPTR address;     /* physical address for PCI transaction.    */
    address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase + regAddr);
#endif
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }


    debugTrace(("PCI Write: addr 0x%X data 0x%X \n", regAddr, data));

    leData = CPSS_32BIT_LE(data);
#ifdef ASIC_SIMULATION
    scibPciRegWrite(SIM_DEV_ID(devNum,portGroupId), regAddr, 1, &leData);
#else /*ASIC_SIMULATION*/
    *((volatile GT_U32*)address) = leData;
    /* need to put GT_SYNC to avoid from having the CPU doing
    * write combine. Some CPUs like advanced new PPC can do write combining
    * of two writes to continuous addresses. So, to avoid that we use the eioio
    * in PPC architecture */
    GT_SYNC; /* to avoid from write combining */
#endif /*ASIC_SIMULATION*/

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* Check if trace hw write is enabled */
        return cpssTraceHwAccessWrite(devNum,
                                      portGroupId,
                                      GT_FALSE, /* ISR context */
                                      CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E,  /* PCI/PEX space */
                                      regAddr,
                                      1,    /* length to trace the data in words */
                                      &data);
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusResetAndInitCtrlReadRegister
*
* DESCRIPTION:
*       This function reads a register from the Reset and Init Controller.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
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
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_U32     temp;        /* data in HW format                        */
    GT_UINTPTR address;     /* physical access address for transaction. */

#ifdef ASIC_SIMULATION
    address = regAddr;
    scibMemoryClientRegRead(
        SIM_DEV_ID(devNum,0), SCIB_MEM_ACCESS_DFX_E, address, 1, &temp);
#else /*ASIC_SIMULATION*/
    address = (prvCpssDrvPpConfig[devNum]->resetAndInitControllerBase + regAddr);
    temp = *((volatile GT_U32 *)address);
    GT_SYNC; /*to avoid read combining */
#endif /*ASIC_SIMULATION*/

    *data = CPSS_32BIT_LE(temp);

    debugPrint(("Reset and Init Controller Read: addr 0x%X data 0x%X \n",
                regAddr, *data));

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        return cpssTraceHwAccessRead(devNum,
                                     0,
                                     GT_FALSE,  /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E,   /* Address space */
                                     regAddr,
                                     1,    /* length to trace the data in words */
                                     data);
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusResetAndInitCtrlWriteRegister
*
* DESCRIPTION:
*       This function writes a register to the Reset and Init Controller.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
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
    IN  GT_U8   devNum,
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
        SIM_DEV_ID(devNum,0), SCIB_MEM_ACCESS_DFX_E, address, 1, &temp);
#else /*ASIC_SIMULATION*/
    address = (prvCpssDrvPpConfig[devNum]->resetAndInitControllerBase + regAddr);
    *((volatile GT_U32*)address) = temp;
    GT_SYNC; /*to avoid read combining */
#endif /*ASIC_SIMULATION*/

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        return cpssTraceHwAccessWrite(devNum,
                                      0,
                                      GT_FALSE, /* ISR context */
                                      CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E,  /* Address space */
                                      regAddr,
                                      1,    /* length to trace the data in words */
                                      &data);
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusResetAndInitCtrlGetRegField
*
* DESCRIPTION:
*       This function reads a selected register field from the Reset and Init
*       Controller.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       regAddr - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - the read field data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPexMbusResetAndInitCtrlGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)
{
    GT_STATUS rc;
    GT_U32 data = 0, mask;

    rc = hwPpPexMbusResetAndInitCtrlReadRegister(devNum, regAddr, &data);
    if( GT_OK != rc )
    {
        return rc;
    }

    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    *fieldData = (GT_U32)((data & mask) >> fieldOffset);

    return GT_OK;
}

/*******************************************************************************
* hwPpPexMbusResetAndInitCtrlSetRegField
*
* DESCRIPTION:
*       This function writes a selected register field to the Reset and Init
*       Controller.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
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
static GT_STATUS hwPpPexMbusResetAndInitCtrlSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
)
{
    GT_STATUS rc;
    GT_U32 data = 0, mask;

    rc = hwPpPexMbusResetAndInitCtrlReadRegister(devNum, regAddr, &data);
    if( GT_OK != rc )
    {
        return rc;
    }

    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    data &= ~mask;   /* turn the field off */
    /* insert the new value of field in its place */
    data |= ((fieldData << fieldOffset) & mask);

    return hwPpPexMbusResetAndInitCtrlWriteRegister(devNum, regAddr, data);
}

/*******************************************************************************
* driverHwPciCntlInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum          - The PP's device number to init the structure for.
*       portGroupId  - The port group Id.
*       baseAddr        - The PP base address on the host interface bus.
*       internalPciBase - Base address to which the internal pci registers
*                         are mapped to.
*       isDiag          - Is this initialization is for diagnostics purposes
*                         (GT_TRUE), or is it a final initialization of the Hw
*                         Cntl unit (GT_FALSE)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       1.  In case isDiag == GT_TRUE, no semaphores are initialized.
*
* Galtis:
*       None.
*
*******************************************************************************/
static GT_STATUS driverHwPexMbusCntlInit
(
    IN GT_U8        devNum,
    IN GT_U32       portGroupId,
    IN GT_UINTPTR   baseAddr,
    IN GT_UINTPTR   internalPciBase,
    IN GT_BOOL      isDiag
)
{
    GT_U8   i;               /* iterator      */
    char    name[20];        /* name of mutex */
    GT_U32  data;            /* regiser data  */
    GT_U32  regBmpIsr;       /* the bitmap of regions for ISR  */
    GT_U32  regBmpApp;       /* the bitmap of regions for CPSS */
    GT_U32  regBmp;          /* the bitmap of regions (work) */

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr           = baseAddr;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx            = 0;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase    = internalPciBase;

    /* Check completion masks validity */
    regBmpIsr = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].isrAddrCompletionRegionsBmp;
    regBmpApp = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].appAddrCompletionRegionsBmp;
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

    /* ISR bitmap bounds */
    regBmp = regBmpIsr;
    /* find first 1-bit index */
    for (i = 0; ((regBmp & 1) == 0); regBmp = (regBmp >> 1), i++){};
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].isrAddrRegionMin = i;
    /* find last 1-bit index */
    for (/**/; (regBmp != 0); regBmp = (regBmp >> 1), i++){};
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].isrAddrRegionMax = i - 1;

    /* APP bitmap bounds */
    regBmp = regBmpApp;
    /* find first 1-bit index */
    for (i = 0; ((regBmp & 1) == 0); regBmp = (regBmp >> 1), i++){};
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].appAddrRegionMin = i;
    /* find last 1-bit index */
    for (/**/; (regBmp != 0); regBmp = (regBmp >> 1), i++){};
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].appAddrRegionMax = i - 1;

    /* init last used regions as MAX regions */
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].isrAddrRegionUsed =
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].isrAddrRegionMax;

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].appAddrRegionUsed =
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].appAddrRegionMax;

    /* Reset Hw Address Completion registers not needed         */
    /* The 0-th region must be zero and never updated           */
    /* Other regions were be updated when will be needed after  */
    /* unsucssess searching needed value in shadow              */

    /* Update Address Completion shadow     */
    for (i = 0; i < 8; i++)
    {
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[i] = 0;
    }

    if ((isDiag == GT_FALSE) && (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem == (CPSS_OS_MUTEX)0))
    {
        cpssOsSprintf (name,"hwComplSem-%d_%ld",devNum,portGroupId);

        cpssOsMutexCreate (name,&(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem));
    }

#ifdef ASIC_SIMULATION
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].simInfo.devId =
                            (GT_U8)scibGetDeviceId(baseAddr);
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].simInfo.addrCompletEnable =
        scibAddressCompletionStatusGet(SIM_DEV_ID(devNum,portGroupId));
#endif

    /* set 8-region mode: regAddr = 0x140, set bit16 to 0 */
    cpuReadMemoryWord(devNum, portGroupId, 0x140, &data);
    data &= (~ (1 << 16));
    cpuWriteMemoryWord(devNum, portGroupId, 0x140, data);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvHwPexMbusDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes PEX device with 8 address complete regions driver object
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPexMbusDriverObjectInit
(
     void
)
{
#ifdef INCL_EXDXMX_DRIVER
    /* driver object initialization */
    prvCpssDrvMngInfPexMbusPtr->drvHwPpReadReg         = hwPpPexMbusReadRegister;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpWriteReg        = hwPpPexMbusWriteRegister;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpGetRegField     = hwPpPexMbusGetRegField;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpSetRegField     = hwPpPexMbusSetRegField;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpReadRegBitMask  = hwPpPexMbusReadRegBitMask;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpWriteRegBitMask = hwPpPexMbusWriteRegBitMask;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpReadRam         = hwPpPexMbusReadRam;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpWriteRam        = hwPpPexMbusWriteRam;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpReadVec         = hwPpPexMbusReadVec;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpWriteVec        = hwPpPexMbusWriteVec;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpWriteRamRev     = hwPpPexMbusWriteRamInReverse;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpIsrRead         = hwPpPexMbusIsrRead;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpIsrWrite        = hwPpPexMbusIsrWrite;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpReadIntPciReg   = hwPpPexMbusReadInternalPciReg;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpWriteIntPciReg  = hwPpPexMbusWriteInternalPciReg;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpResetAndInitControllerReadReg     =
        hwPpPexMbusResetAndInitCtrlReadRegister;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpResetAndInitControllerWriteReg    =
        hwPpPexMbusResetAndInitCtrlWriteRegister;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpResetAndInitControllerGetRegField =
        hwPpPexMbusResetAndInitCtrlGetRegField;
    prvCpssDrvMngInfPexMbusPtr->drvHwPpResetAndInitControllerSetRegField =
        hwPpPexMbusResetAndInitCtrlSetRegField;
    prvCpssDrvMngInfPexMbusPtr->drvHwCntlInit          = driverHwPexMbusCntlInit;
#endif /*INCL_EXDXMX_DRIVER*/
    return GT_OK;
}





