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


/*  Address range of PUMA's LPM SRAM - used in hwWrite */
#define PUMA_LPM_SRAM_START     0x50000000
#define PUMA_LPM_SRAM_END       0x90000000


/*******************************************************************************
* Intenal Macros and definitions
*******************************************************************************/
#define DUMMY_REG_ADDR  0xC
/*
 *  Description:
 *          Read access to Pci area, in order to complete the write action.
 *
 *  IN  GT_UINTPTR  baseAddr
 *  OUT GT_U32  data
 *
 *  NOTE:
 *          The read must be performed from region 0, in order to prevent
 *          un-needed address completion register update.
 *
 */
#define READ_AFTER_WRITE(devNum,portGroupId,baseAddr,data)                                \
            if((drvGlobalInfo.performReadAfterWrite == GT_TRUE) &&                          \
               (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].doReadAfterWrite == GT_TRUE))\
            {                                                                 \
                /* assumed GT_SYNC done before */                             \
                (data) = *((volatile GT_U32*)((baseAddr) + ADDR_COMP_REG));   \
            }

/*
 *  Description:
 *          This function drains the CPU write buffer, like in PPC 750
 *          and then before a read-after-write, to guarantee the transaction
 *          buffer of the PCI bridge (like discovery or MPC107) flushes
 *          its write
 *
 *  IN  GT_U8  devNum
 *
 */
#define DRAIN_WRITE_BUFFERS(devNum,portGroupId) {                           \
    GT_UINTPTR dummyBaseAddr, dummyData;                            \
    dummyBaseAddr = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr;\
    GT_SYNC;                                                    \
    READ_AFTER_WRITE(devNum,portGroupId,dummyBaseAddr,dummyData);           \
    (void)dummyData; /* fix GCC warning - warning: variable 'dummyData' set but not used [-Wunused-but-set-variable] */\
    }

#define DUMMY_WRITE(devNum,portGroupId)                                           \
    if(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].doReadAfterWrite == GT_TRUE)\
    {                                                                 \
       *((volatile GT_U32*)                                           \
            (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr | DUMMY_REG_ADDR)) \
             = 0;                                                     \
        /* GT_SYNC; assumed to be done after */                       \
    }

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Forward functions declarations.
*******************************************************************************/
static GT_STATUS hwPpWriteBurst
(
    IN  GT_U8               devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32              destAddr,
    IN  GT_U32              length,
    IN  GT_U32              *data,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo
);
static GT_STATUS hwPpReadBurst
(
    IN  GT_U8               devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32              srcAddr,
    IN  GT_U32              length,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo,
    OUT GT_U32              *data
);
static GT_STATUS hwWrite
(
    IN  GT_U8       devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_U32      *dataPtr
);

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
static GT_STATUS driverHwPciCntlInit
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
    GT_UINTPTR  address;     /* Physical address for PCI transaction */
    GT_BOOL     skipHwWrite; /* skip write to HW */
    GT_STATUS   rc;          /* return status */

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr           = baseAddr;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx            = 2;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase    = internalPciBase;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[0]   = 0;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[1]   = 0;

    /* allow write to HW */
    skipHwWrite = GT_FALSE;

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterWriteFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterWriteFunc( 
                                                  devNum, portGroupId, 
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
        address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr + ADDR_COMP_REG;
    
        *(volatile GT_U32 *)(address) = 0;
        *(volatile GT_U32 *)(address) = 0;
        *(volatile GT_U32 *)(address) = 0;
        GT_SYNC;

        /* Check if HW access required. */
        if(prvCpssHwAccessObj.hwAccessRegisterWriteFunc != NULL)
        {
           prvCpssHwAccessObj.hwAccessRegisterWriteFunc( 
                                                      devNum, portGroupId, 
                                                      ADDR_COMP_REG, 0,
                                                      CPSS_DRV_HW_ACCESS_STAGE_POST_E);
        }
    }

    /* Update Address Completion shadow     */
    for (i = 0; i < 4; i++)
    {
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[i] = 0;
    }

    if ((isDiag == GT_FALSE) && (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem == (CPSS_OS_MUTEX)0))
    {
        cpssOsSprintf (name,"hwComplSem-%d_%ld",devNum,portGroupId);

        cpssOsMutexCreate (name,&(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem));
    }

    return GT_OK;
}


/*******************************************************************************
* handleReadAfterWrite
*
* DESCRIPTION:
*       This function handles the read after write HW limitation.
*
* INPUTS:
*       devNum  - The Pp's device number.
*       portGroupId  - The port group Id.
*       regAddr - The register / Ram address to read / write to.
*       length  - Number of words toe read / write.
*       isRead  - GT_TRUE, this is a read operation, GT_FALSE, this is a write
*                 operation.
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
static GT_STATUS handleReadAfterWrite
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  length,
    IN  GT_BOOL isRead
)
{
    GT_U32  tmpData;
    GT_U32  tmpAddr;
    GT_U32  i;

    if(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].doReadAfterWrite == GT_FALSE)
    {
        return GT_OK;
    }

    if(isRead == GT_FALSE)
    {
        /* This is a write operation    */
        if(length > 1)
        {
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[1] =
                regAddr + (length - 2) * 4;
        }
        else
        {
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[1] =
                prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[0];
        }

        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[0] =
            regAddr + (length - 1) * 4;
    }
    else
    {
        /* This is a read operation     */
        for(i = 0; i < 2; i++)
        {
            tmpAddr = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[i];
            if((regAddr <= tmpAddr) &&
               ((regAddr + (length - 1) * 4) >= tmpAddr))
            {
                tmpData = 0;
                tmpAddr = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].dummyReadAfterWriteRegAddr;
                hwWrite(devNum,portGroupId,tmpAddr,1,&tmpData);
                hwWrite(devNum,portGroupId,tmpAddr,1,&tmpData);
                break;
            }
        }
    }
    return GT_OK;
}

#ifdef USE_PUMA_LPM_DWORD
/*******************************************************************************
*  hwWriteDouble
*
* DESCRIPTION:
*      This function receives address and data pointer and write the data to the PP
*      Write width is 64 bit if it's possible
*
* INPUTS:
*       devNum  - The PP dev Num to read from.
*       portGroupId  - The port group Id.
*       address - The register's PHYSICAL address .
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
static GT_STATUS hwWriteDouble
(
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
    IN  GT_UINTPTR  address,
    IN  GT_U32      length,
    IN  GT_U32      *dataPtr
)
{
    GT_U32  lastWord;
    GT_U32  index;

    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }
    /* save pointer to the last word  in case that 'length' will be changed */
    lastWord = dataPtr[length-1];

    /* write 32-bit prefix, if exists  - address should be 64-bit aligned */
    if (address & 0x7)
    {
        *(volatile GT_U32*)(address) = CPSS_32BIT_LE(dataPtr[0]);
        address +=4;
        length -=1;
    }

    /* 64 bit writes - data divided to two 32-bit words to avoid compiler issues. Swap should be done since we are writing to PCI/PEX */
    for(index = 0;index < (length & 0xFFFFFFFE);index+=2,address+=8)
    {
        cpssExtDrvPciDoubleWrite(address, CPSS_32BIT_LE(dataPtr[index]), CPSS_32BIT_LE(dataPtr[index+1]));
    }

    /* write 32-bit postfix, if exists  - check if length is odd */
    if (length & 0x1)
    {
        *((volatile GT_U32*)(address)) = CPSS_32BIT_LE(lastWord);
    }

    return GT_OK;
}
#endif /* USE_PUMA_LPM_DWORD */

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
    IN  GT_U8       devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_U32      *dataPtr
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  address;  /*physical address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U8   i;      /* count iterator for the completion index compare loop*/
    GT_U32  j = 0;      /* count iterator for the write loop*/
    GT_U32  nextRegionAddr; /* address of the next region after the one
                                currently used */
    GT_U32  loopLength = 0; /* when length exceeds region addr, Set to end of
                            region range */
    GT_U32  data;   /* data to be write to memory */
    GT_U32  traceLength;    /* length to trace the data */
#ifdef USE_PUMA_LPM_DWORD
    GT_BOOL pumaLpmSram = GT_FALSE; /* for Puma - if destination is LPM SRAM */
#endif
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    debugTrace(("Write: addr 0x%X len 0x%X data 0x%X \n", regAddr, length, *dataPtr));

    /* Handle read after write situation.   */
    handleReadAfterWrite(devNum,portGroupId,regAddr,length,GT_FALSE);

#ifdef USE_PUMA_LPM_DWORD
    /* Check if destination is within LPM SRAM space  of PUMA */
    if (CPSS_IS_EXMXPM_FAMILY_MAC(prvCpssDrvPpConfig[devNum]->devFamily))
    {
        if((regAddr >= PUMA_LPM_SRAM_START) && (regAddr < PUMA_LPM_SRAM_END))
        {
            pumaLpmSram = GT_TRUE;
        }
    }
#endif

    /* check if addrRegion is 0 */
    if((regAddr & 0xFF000000) != 0)
    {
        cpssOsMutexLock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);

        addrRegion = (GT_U8)(regAddr >> 24);
        /* compare addr region to existing Address regions*/
        for (i = 3;(i > 1) &&
             (addrRegion != prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[i]);
             i--);
        if(i == 1)
        {
            /* Set addrRegion in AddrCompletion register    */
            /*round robin on Region index : 2,3             */
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx = (GT_U8)
                (((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx - 1) % 2)+ 2);

            compIdx = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx;

            /* Update Address Completion shadow.            */
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] =
                addrRegion;

            /* Update Hw Address Completion - using completion region 0.*/
            address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                        ADDR_COMP_REG;
            data = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1]<<8) |
                    (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2]<<16) |
                    (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3]<<24);

            data = CPSS_32BIT_LE(data);

            *(volatile GT_U32 *)(address) = (volatile GT_U32)data;
            /* write the address completion 3 times.
            because the PP have a 2 entry write buffer
            so, the 3 writes will make sure we do get
            to the hardware register itself */
            GT_SYNC;
            *(volatile GT_U32 *)(address) = (volatile GT_U32)data;
            GT_SYNC;
            *(volatile GT_U32 *)(address) = (volatile GT_U32)data;
            GT_SYNC;
        }
        else
        {
            compIdx = i;
        }

        address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr + ((compIdx << 24) |
                   (regAddr & 0x00ffffff)));
        /* check whether completion region boundaries exceeded*/
        nextRegionAddr =  (GT_U32)(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].
                                   addrCompletShadow[compIdx] + 1)<<24;
        if((GT_U32)(regAddr + length * 4 - 1) < nextRegionAddr)
        {
#ifdef USE_PUMA_LPM_DWORD
            if ((length > 1) && (GT_TRUE == pumaLpmSram))
            {
                hwWriteDouble(devNum, portGroupId,address, length, dataPtr);
            }
            else
            {
#endif /* USE_PUMA_LPM_DWORD */

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
#ifdef USE_PUMA_LPM_DWORD
            } /* else */
#endif /* USE_PUMA_LPM_DWORD */
            cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
        }
        else
        {
            loopLength = (nextRegionAddr - regAddr) / 4;
#ifdef USE_PUMA_LPM_DWORD
            if ((loopLength>1) && (GT_TRUE == pumaLpmSram))
            {
                hwWriteDouble(devNum, portGroupId ,address, loopLength, dataPtr);
            }
            else
            {
#endif /* USE_PUMA_LPM_DWORD */
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
#ifdef USE_PUMA_LPM_DWORD
            } /* else */
#endif /* USE_PUMA_LPM_DWORD */
            cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);

            /* Recursive call for rest of data in next region.  */
            hwWrite(devNum,portGroupId,nextRegionAddr,length-loopLength,
                    dataPtr+loopLength);
        }
    }   /* End of if complId !=0    */
    else
    {
        /* Addr Region = 0          */
        address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                   (regAddr & 0x00ffffff));
        if((GT_U32)(regAddr+length*4-1) < 0x1000000)
        {
#ifdef USE_PUMA_LPM_DWORD
            if ((length > 1) && (GT_TRUE == pumaLpmSram))
            {
                hwWriteDouble(devNum, portGroupId,address, length, dataPtr);
            }
            else
            {
#endif /* USE_PUMA_LPM_DWORD */
            for(i = 0;i < length;i++)
            {
                *((volatile GT_U32*) address) = CPSS_32BIT_LE(dataPtr[i]);
                /* need to put GT_SYNC to avoid from having the CPU doing
                * write combine. Some CPUs like advanced new PPC can do write combining
                * of two writes to continuous addresses. So, to avoid that we use the eioio
                * in PPC architecture;*/
                GT_SYNC;

                address += 4;
            }
#ifdef USE_PUMA_LPM_DWORD
            } /* else */
#endif /* USE_PUMA_LPM_DWORD */
        }
        else
        {
            loopLength = (0x1000000 - regAddr) / 4;
#ifdef USE_PUMA_LPM_DWORD
            if ((loopLength > 1) && (GT_TRUE == pumaLpmSram))
            {
                hwWriteDouble(devNum, portGroupId,address, loopLength, dataPtr);
            }
            else
            {
#endif /* USE_PUMA_LPM_DWORD */
            for(j = 0; j < loopLength; j++)
            {
                *((volatile GT_U32*) address) = CPSS_32BIT_LE(dataPtr[j]);
                /* need to put GT_SYNC to avoid from having the CPU doing
                * write combine. Some CPUs like advanced new PPC can do write combining
                * of two writes to continuous addresses. So, to avoid that we use the eioio
                * in PPC architecture;*/
                GT_SYNC;

                address += 4;
            }
#ifdef USE_PUMA_LPM_DWORD
            } /* else */
#endif /* USE_PUMA_LPM_DWORD */
            /* recursive call for rest of data in next region */
            hwWrite(devNum,portGroupId,0x1000000,length-loopLength,dataPtr+loopLength);
        }
    }

    DUMMY_WRITE(devNum,portGroupId);

    /*now after we finished all the writes, we need to drain the CPU write buffer
     * as well as the PCI bridge write buffer */

    DRAIN_WRITE_BUFFERS(devNum,portGroupId);


    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* trace HW write access */
        if (loopLength > 0)
        {
            traceLength = loopLength;
        }
        else
        {
            traceLength = length;
        }

        return cpssTraceHwAccessWrite(devNum,
                                      portGroupId,
                                      GT_FALSE, /* ISR context */
                                      CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                      regAddr,
                                      traceLength, /* length to trace the data in words */
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
*
*******************************************************************************/
static GT_STATUS hwWriteInReverse
(
    IN  GT_U8       devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    IN  GT_U32      *dataPtr
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  address;  /*physical address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_32   i;      /* count iterator for the completion index compare loop*/
    GT_32   j;      /* count iterator for the write loop*/
    GT_U32  nextRegionAddr; /* address of the next region after the one
                                currently used */
    GT_U32  loopLength = 0; /* when length exceeds region addr, Set to end of
                            region range */
    GT_U32  data;   /* data to be write to memory */
    GT_U32  traceLength;    /* length to trace the data */

    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    /* Handle read after write situation.   */
    handleReadAfterWrite(devNum,portGroupId,regAddr,length,GT_FALSE);

    /* check if addrRegion is 0 */
    if((regAddr & 0xFF000000) != 0)
    {
        cpssOsMutexLock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);

        addrRegion = (GT_U8)(regAddr >> 24);
        /* compare addr region to existing Address regions*/
        for (i = 3;(i > 1) &&
             (addrRegion != prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[i]);
             i--);
        if(i == 1)
        {
            /* Set addrRegion in AddrCompletion register    */
            /*round robin on Region index : 2,3             */
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx = (GT_U8)
                (((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx - 1) % 2)+ 2);

            compIdx = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx;

            /* Update Address Completion shadow.            */
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] =
                addrRegion;

            /* Update Hw Address Completion - using completion region 0.*/
            address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                        ADDR_COMP_REG;
            data = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1]<<8) |
                    (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2]<<16) |
                    (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3]<<24);

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
            compIdx = (GT_U8)i;
        }

        address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr + ((compIdx << 24) |
                   (regAddr & 0x00ffffff))) + (length - 1) * 4;
        /* check whether completion region boundaries exceeded*/
        nextRegionAddr =  (GT_U32)(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].
                                   addrCompletShadow[compIdx] + 1)<<24;
        if((GT_U32)(regAddr + length * 4 - 1) < nextRegionAddr)
        {
            for(j = length-1; j >= 0; j--)
            {
                *((volatile GT_U32*)address) = CPSS_32BIT_LE(dataPtr[j]);
                GT_SYNC;

                address -= 4;
            }
            cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
        }
        else
        {
            loopLength = (nextRegionAddr - regAddr) / 4;
            cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
            /* Recursive call for rest of data in next region.  */
            hwWrite(devNum,portGroupId,nextRegionAddr,length-loopLength,
                    dataPtr+loopLength);
            cpssOsMutexLock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
            for(j = loopLength-1; j >= 0; j--)
            {
                *((volatile GT_U32*)address) = CPSS_32BIT_LE(dataPtr[j]);
                GT_SYNC;

                address -= 4;
            }
            cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);

        }
    }   /* End of if complId !=0    */
    else
    {
        /* Addr Region = 0          */
        address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                   (regAddr & 0x00ffffff)) + (length - 1)*4;
        if((GT_U32)(regAddr+length*4-1) < 0x1000000)
        {
            for(i = length-1;i >= 0;i--)
            {
                *((volatile GT_U32*) address) = CPSS_32BIT_LE(dataPtr[i]);
                address -= 4;
                GT_SYNC;
            }
        }
        else
        {
            loopLength = (0x1000000 - regAddr) / 4;
            /* recursive call for rest of data in next region */
            hwWrite(devNum,portGroupId,0x1000000,length-loopLength,dataPtr+loopLength);
            for(j = loopLength-1; j >= 0; j--)
            {
                *((volatile GT_U32*) address) = CPSS_32BIT_LE(dataPtr[j]);
                address -= 4;
                GT_SYNC;
            }
        }
    }

    DUMMY_WRITE(devNum,portGroupId);

    DRAIN_WRITE_BUFFERS(devNum,portGroupId);

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* trace HW write access */

        if (loopLength > 0)
        {
            traceLength = loopLength;
        }
        else
        {
            traceLength = length;
        }

        return cpssTraceHwAccessWrite(devNum,
                                      portGroupId,
                                      GT_FALSE, /* ISR context */
                                      CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                      regAddr,
                                      traceLength, /* length to trace the data in words */
                                      dataPtr);
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
    IN  GT_U8       devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32      regAddr,
    IN  GT_U32      length,
    OUT GT_U32      *dataPtr
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  address;  /*physical access address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U32  temp;
    GT_U8   i;      /* count iterator for the completion index compare loop*/
    GT_U32  j;      /* count iterator for the write loop*/
    GT_U32  nextRegionAddr; /* address of the next region after the one
                                currently used */
    GT_U32  loopLength = 0; /* when length exceeds region addr, Set to end of
                            region range */
    GT_U32  data;   /* data to be write to memory */
    GT_U32  traceLength;    /* length to trace the data */

    /* Handle read after write situation.   */
    handleReadAfterWrite(devNum,portGroupId,regAddr,length,GT_TRUE);

    /* check if addrRegion is 0 */
    if((regAddr & 0xFF000000)!=0)
    {
        cpssOsMutexLock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);

        addrRegion = (GT_U8)(regAddr >> 24);
        /* compare addr region to existing Address regions*/
        for (i = 3;(i > 1) &&
             (addrRegion != prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[i]);
             i--);
        if(i == 1)
        {   /* Set addrRegion in AddrCompletion register */

            /*round robin on Region index : 2,3*/
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx = (GT_U8)
                (((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx -1) % 2) + 2); /*round
                                                                      robin
                                                                    - 1,2,3*/
            compIdx = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx;

                      /*update Address Completion shadow*/
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] =
                addrRegion;

            /* update Hw Address Completion - using completion region 0 */
            address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                    ADDR_COMP_REG;
            data = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1]<<8) |
                   (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2]<<16) |
                   (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3]<<24);

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
            DUMMY_WRITE(devNum,portGroupId);
        }
        else
        {
            compIdx = i;
        }

        address = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr) + ((compIdx << 24) |
                   (regAddr & 0x00ffffff)));

        /* check whether completion region boundaries are exceeded*/
        nextRegionAddr = (GT_U32)(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].
                                  addrCompletShadow[compIdx]+1)<<24;
        if ((GT_U32)(regAddr+length*4-1) < nextRegionAddr)
        {
            for(j = 0;j < length;j++)
            {
                temp = *( (volatile GT_U32 *) address);
                GT_SYNC; /*to avoid read combine */
                dataPtr[j] = CPSS_32BIT_LE(temp);
                address += 4;
            }
            cpssOsMutexUnlock ( prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
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
            cpssOsMutexUnlock ( prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);

            /* recursive call for rest of data in next region */
            hwRead(devNum,portGroupId,nextRegionAddr,length-loopLength,
                    dataPtr+loopLength);
        }

    }   /*end of if complId !=0*/
    else
    {
        address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                   (regAddr & 0x00ffffff));
           /* check whether completion region boundaries exceeded*/
        if ((GT_U32)(regAddr+length*4-1) < 0x1000000)
        {
            for(i = 0;i < length;i++)
            {
                temp = *( (volatile GT_U32 *) address);
                GT_SYNC;
                dataPtr[i] = CPSS_32BIT_LE(temp);
                address += 4;
            }
        }
        else
        {
            loopLength =  (0x1000000 - regAddr)/4;
            for(j = 0;j < loopLength ;j++)
            {
                temp = *( (volatile GT_U32 *) address);
                GT_SYNC;
                dataPtr[j] = CPSS_32BIT_LE(temp);
                address += 4;
            }

            /* recursive call for rest of data in next region */
            hwRead(devNum,portGroupId,0x1000000,length-loopLength,dataPtr + loopLength);
        }
    }

    debugTrace(("Read: addr 0x%X len 0x%X data 0x%X \n", regAddr, length, *dataPtr));


    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        /* trace HW read access */
        if (loopLength > 0)
        {
            traceLength = loopLength;
        }
        else
        {
            traceLength = length;
        }

        return cpssTraceHwAccessRead(devNum,
                                     portGroupId,
                                     GT_FALSE, /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                     regAddr,
                                     traceLength, /* length to trace the data in words */
                                     dataPtr);
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpPciReadRegister
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
static GT_STATUS hwPpPciReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            GT_U32 dummy;

            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    debugPrint(("Read from dev %d, reg 0x%x,",devNum, regAddr));
    hwRead (devNum, portGroupId,regAddr,1, data);
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
static GT_STATUS hwPpPciWriteRegister
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            GT_U32 dummy;

            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    debugPrint(("Write to device %d, reg 0x%x, data 0x%x.\n",devNum,regAddr,
                value));

    hwWrite (devNum, portGroupId,regAddr, 1,&value);

    return GT_OK;
}


/*******************************************************************************
* hwPpPciGetRegField
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
static GT_STATUS hwPpPciGetRegField
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

    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            GT_U32 dummy;

            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    debugPrint(("GetField from dev %d, reg 0x%x, offset %d, length %d,",
                devNum, regAddr, fieldOffset, fieldLength));

    hwRead( devNum, portGroupId,regAddr, 1,&data);
    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    *fieldData = (GT_U32)((data & mask) >> fieldOffset);

    debugPrint(("data 0x%x.\n", *fieldData));

    return GT_OK;
}

/*******************************************************************************
* hwPpPciSetRegField
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
static GT_STATUS hwPpPciSetRegField
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData
)
{
    GT_U32 data = 0, mask;
    GT_U32 dummy;

    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    debugPrint(("SetField to dev %d, reg 0x%x, offset %d, length %d, \
        data 0x%x.\n",devNum, regAddr, fieldOffset, fieldLength, fieldData));

    hwRead( devNum, portGroupId,regAddr, 1,&data);
    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    data &= ~mask;   /* turn the field off */
    /* insert the new value of field in its place */
    data |= ((fieldData << fieldOffset) & mask);

    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    hwWrite( devNum, portGroupId,regAddr, 1, &data);

    return GT_OK;
}




/*******************************************************************************
* hwPpPciReadRegBitMask
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
static GT_STATUS hwPpPciReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    GT_U32 data = 0;

    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            GT_U32 dummy;

            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    debugPrint(("\nhwPpReadRegBitMask on device %d, from register 0x%x, \
                mask 0x%x,", devNum, regAddr, mask));

    hwRead ( devNum, portGroupId,regAddr, 1,&data);
    *dataPtr = (data) & mask;

    debugPrint((" data is 0x%x.\n", *dataPtr));
    return GT_OK;


}




/*******************************************************************************
* hwPpPciWriteRegBitMask
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
static GT_STATUS hwPpPciWriteRegBitMask
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
)

{
    GT_U32 data = 0;
    GT_U32 dummy;

    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    debugPrint(("\nhwPpWriteRegBitMask on device %d, from register 0x%x, \
                mask 0x%x, data is 0x%x.\n",
                devNum, regAddr, mask, value));

    /* in case mask = 0xffffffff call hwWrite only */
    if(mask == 0xffffffff)
    {
        hwWrite (devNum, portGroupId,regAddr, 1, &value);
        return GT_OK;
    }
    /* else read modify write */
    hwRead ( devNum, portGroupId,regAddr, 1, &data);
    data &= (~mask);            /* Turn the field off.                        */
    data |= (value & mask);     /* Insert the new value of field in its place.*/

    if ((regAddr & 0xFFFF0000) == 0x09800000)
    {
        if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DRV_ERRATA_SERDES_INTERNAL_REGS_ACCESS_WA_E) == GT_TRUE)
        {
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
            hwRead (devNum, portGroupId, PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 1, &dummy);
        }
    }

    hwWrite (devNum, portGroupId,regAddr, 1, &data);

    return GT_OK;
}


/*******************************************************************************
* hwPpPciReadRam
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
static GT_STATUS hwPpPciReadRam
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
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

    hwRead (devNum, portGroupId,addr, length, data);
    return GT_OK;
}



/*******************************************************************************
* hwPpPciWriteRam
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
static GT_STATUS hwPpPciWriteRam
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
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

    hwWrite(devNum, portGroupId,addr, length, data);
    return GT_OK;
}


/*******************************************************************************
* hwPpPciReadVec
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
static GT_STATUS hwPpPciReadVec
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
        rc = hwRead (devNum, portGroupId,addrArr[i], 1, &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
        debugPrint(("\nhwPpPciReadVec on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addrArr[i], dataArr[i]));
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpPciWriteVec
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
static GT_STATUS hwPpPciWriteVec
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
        rc = hwWrite(devNum, portGroupId,addrArr[i], 1, &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
        debugPrint(("\nhwPpPciWriteVec on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addrArr[i], dataArr[i]));
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpPciWriteRamInReverse
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
static GT_STATUS hwPpPciWriteRamInReverse
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
    status = hwWriteInReverse(devNum, portGroupId,addr, length, data);
    return status;
}

/*******************************************************************************
* hwPpPciIsrRead
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
static GT_STATUS hwPpPciIsrRead
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
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
        addrData = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr + regAddr;
    }
    else
    {
        
        addrCompletion = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                        ADDR_COMP_REG);

#if (defined _linux)
        cpssOsMutexLock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
#endif

        /* Update Address Completion shadow.            */
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] = addrRegion;

        /* Update Hw Address Completion Region 1- using region 0. */
        dataComletion =
                ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1] << 8) |
                (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2] << 16) |
                (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3] << 24));

        /* write the address completion 3 times.
        because the PP have a 2 entry write buffer
        so, the 3 writes will make sure we do get
        to the hardware register itself */

        dataComletion = CPSS_32BIT_LE(dataComletion);

        *(volatile GT_U32 *)(addrCompletion) = dataComletion;
        *(volatile GT_U32 *)(addrCompletion) = dataComletion;
        *(volatile GT_U32 *)(addrCompletion) = dataComletion;
        GT_SYNC;

#if (defined _linux)
    cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
#endif

        addrData = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr) + ((compIdx << 24) |
                   (regAddr & 0x00ffffff)));

    }
    /* read the data from ASIC */
    temp = *( (volatile GT_U32 *) addrData);
    *dataPtr = CPSS_32BIT_LE(temp);

    debugTrace(("ISR Read: addr 0x%X data 0x%X \n", regAddr,  *dataPtr));

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        /* Check if trace hw read is enabled */
        return cpssTraceHwAccessRead(devNum,
                                     portGroupId,
                                     GT_TRUE,  /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                     regAddr,
                                     1,    /* length to trace the data in words */
                                     dataPtr);
    }

    return GT_OK;
}
/*******************************************************************************
* hwPpPciIsrWrite
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
static GT_STATUS hwPpPciIsrWrite
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
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
        address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr + regAddr;
    }
    else  /* use Region #1 */
    {

        GT_UINTPTR   addrCompletion;

        addrCompletion = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                            ADDR_COMP_REG);

#if (defined _linux)
        cpssOsMutexLock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
#endif

        /* Update Address Completion shadow.            */
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] = addrRegion;

        /* Update Hw Address Completion Regin 1- using Region 0 */
        data = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1] << 8) |
                (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2] << 16) |
                (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3] << 24));

        *(volatile GT_U32 *)(addrCompletion) = CPSS_32BIT_LE(data);
        GT_SYNC;/* to prevent data write before addreCompleteion write is done*/

#if (defined _linux)
        cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
#endif

        address = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr) + ((compIdx << 24) |
                   (regAddr & 0x00ffffff)));

    }

    tmpValue = CPSS_32BIT_LE(value);
    /* write data 3 times */
    *( (volatile GT_U32*) address) = tmpValue;
    *( (volatile GT_U32*) address) = tmpValue;
    *( (volatile GT_U32*) address) = tmpValue;
    GT_SYNC;/* to prevent optimization return before write complete */


    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* Check if trace hw write is enabled */
        return cpssTraceHwAccessWrite(devNum,
                                      portGroupId,
                                      GT_TRUE,   /* ISR context */
                                      CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,  /* Address space */
                                      regAddr,
                                      1,    /* length to trace the data in words */
                                      &value);
    }

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
static GT_STATUS hwPpPciReadInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_UINTPTR address;     /* physical access address for PCI transaction. */
    GT_U32  temp;

    address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase + regAddr);
    temp = *((volatile GT_U32 *)address);

    GT_SYNC; /*to avoid read combining */

    *data = CPSS_32BIT_LE(temp);

    debugTrace(("PCI Read: addr 0x%X data 0x%X \n", regAddr, *data));

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        /* Check if trace hw read is enabled */
        return cpssTraceHwAccessRead(devNum,
                                     portGroupId,
                                     GT_FALSE,  /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E,   /* Address space */
                                     regAddr,
                                     1,    /* length to trace the data in words */
                                     data);
    }

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
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id.
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
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
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

    address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase + regAddr);
    *((volatile GT_U32*)address) = CPSS_32BIT_LE(data);

    DRAIN_WRITE_BUFFERS(devNum,portGroupId);

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* Check if trace hw write is enabled */
        return cpssTraceHwAccessWrite(devNum,
                                      portGroupId,
                                      GT_FALSE, /* ISR context */
                                      CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E,  /* Address space */
                                      regAddr,
                                      1,    /* length to trace the data in words */
                                      &data);
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpPciResetAndInitCtrlReadRegister
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
static GT_STATUS hwPpPciResetAndInitCtrlReadRegister
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_UINTPTR address;     /* physical access address for transaction. */
    GT_U32  temp;

    address = (prvCpssDrvPpConfig[devNum]->resetAndInitControllerBase + regAddr);
    temp = *((volatile GT_U32 *)address);

    GT_SYNC; /*to avoid read combining */

    *data = CPSS_32BIT_LE(temp);

    debugTrace(("Reset and Init Controller Read: addr 0x%X data 0x%X \n", 
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
* hwPpPciResetAndInitCtrlWriteRegister
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
static GT_STATUS hwPpPciResetAndInitCtrlWriteRegister
(
    IN  GT_U8   devNum,
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

    debugTrace(("Reset and Init Controller Write: addr 0x%X data 0x%X \n", regAddr, data));

    address = (prvCpssDrvPpConfig[devNum]->resetAndInitControllerBase + regAddr);
    *((volatile GT_U32*)address) = CPSS_32BIT_LE(data);

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
* hwPpPciResetAndInitCtrlGetRegField
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
static GT_STATUS hwPpPciResetAndInitCtrlGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)
{
    GT_STATUS rc;
    GT_U32 data, mask;

    rc = hwPpPciResetAndInitCtrlReadRegister(devNum, regAddr, &data);
    if( GT_OK != rc )
    {
        return rc;
    }

    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    *fieldData = (GT_U32)((data & mask) >> fieldOffset);

    return GT_OK;
}


/*******************************************************************************
* hwPpPciResetAndInitCtrlSetRegField
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
static GT_STATUS hwPpPciResetAndInitCtrlSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
)
{
    GT_STATUS rc;
    GT_U32 data, mask;

    rc = hwPpPciResetAndInitCtrlReadRegister(devNum, regAddr, &data);
    if( GT_OK != rc )
    {
        return rc;
    }

    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    data &= ~mask;   /* turn the field off */
    /* insert the new value of field in its place */
    data |= ((fieldData << fieldOffset) & mask);

    return hwPpPciResetAndInitCtrlWriteRegister(devNum, regAddr, data);
}


/*******************************************************************************
* cpssDrvHwReadAfterWriteEnable
*
* DESCRIPTION:
*       This function enables / disables the read after write operations.
*
* INPUTS:
*       enable  - GT_TRUE to enable, GT_FALSE disable.
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
GT_INLINE GT_U32 cpssDrvHwReadAfterWriteEnable
(
    IN  GT_BOOL enable
)
{
    drvGlobalInfo.performReadAfterWrite = enable;

    return GT_OK;
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
*       regAddr - The address to read / write from.
*
* OUTPUTS:
*       compByteIdx - The address completion index that was choosen.
*       pciAddr     - The address to be accessed on the Pci (for read / write).
*       signalSem   - Should the semaphore be signaled after the read / write is
*                     performed.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS setAddrCompletion
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U8   *compByteIdx,
    OUT GT_UINTPTR  *pciAddr,
    OUT GT_BOOL *signalSem
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_UINTPTR  address;  /*physical access address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U8   i;      /* count iterator for the completion index compare loop*/
    GT_U32  data;

    *signalSem = GT_FALSE;

     /* check if addrRegion is 0 */
    if((regAddr & 0xFF000000) != 0)
    {
        cpssOsMutexLock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);

        addrRegion = (GT_U8)(regAddr >> 24);
        /* Compare addr region to existing Address regions. */
        for(i = 3;
            ((i > 1) &&
             (addrRegion != prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[i]));
             i--);
        /* The address was not recently used.   */
        if(i == 1)
        {
            /* Set addrRegion in AddrCompletion register.   */
            /* round robin on Region index : 2,3            */
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx = (GT_U8)
                (((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx - 1) % 2) + 2); /* round
                                                                         robin
                                                                       - 1,2,3*/
            compIdx = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx;

            /* Update Address Completion shadow.    */
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] =
                addrRegion;

            /* Update Hw Address Completion - using completion region 0 */
            address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                    ADDR_COMP_REG;
            data = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1]<<8) |
                    (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2]<<16) |
                    (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3]<<24));

            data = CPSS_32BIT_LE(data);

            *(volatile GT_U32 *)(address) = data;
            GT_SYNC; /*to avoid write combine */

            *(volatile GT_U32 *)(address) = data;
            GT_SYNC;

            *(volatile GT_U32 *)(address) = data;
            GT_SYNC;
        }
        else
        {
            compIdx = i;
        }

        *signalSem = GT_TRUE;
        *pciAddr = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr) + ((compIdx << 24) |
                    (regAddr & 0x00FFFFFF)));
    }
    else
    {
        address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                   (regAddr & 0x00ffffff));
        compIdx = 0;
        *pciAddr = address;
    }

    *compByteIdx = compIdx;
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
    IN  GT_U8               devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32              destAddr,
    IN  GT_U32              length,
    IN  GT_U32              *data,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo
)
{
    GT_U8       addrCompIdx;    /* Index of the address completion byte to  */
                                /* be used for this pci access.             */
    GT_UINTPTR      pciAddr;        /* The address to be used for pci access,   */
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
    retVal = setAddrCompletion(devNum,portGroupId,allignDestAddr,&addrCompIdx,&pciAddr,
                               &signalSem);
    if(retVal != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* Update allignDstAddr with the address completion part.   */
    allignDestAddr = (allignDestAddr & 0xFFFFFF) | (pciAddr & 0xFF000000);

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

        for(i = 0; i < numOfBursts; i++)
        {
            tempSrcAddr =
                (GT_U32*)(((GT_UINTPTR)tempData) + (i * sizeOfBurst *
                                                sizeof(GT_U32)));
            tempDestAddr= (allignDestAddr +
                           (i * sizeOfBurst * ramBurstInfo->addrResolution *
                            sizeof(GT_U32)));

            retVal = cpssExtDrvDmaRead(tempDestAddr,sizeOfBurst,
                                   ramBurstInfo->burstSize,tempSrcAddr);
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

        retVal = cpssExtDrvDmaWrite(tempDestAddr,tempSrcAddr,sizeOfBurst,
                                ramBurstInfo->burstSize);
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

    DUMMY_WRITE(devNum,portGroupId);

    /*now after we finished all the writes,
    we need to drain the CPU write buffer
    as well as the PCI bridge write buffer */
    DRAIN_WRITE_BUFFERS(devNum,portGroupId);

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* trace HW write access */
        retVal = cpssTraceHwAccessWrite(devNum,
                                        portGroupId,
                                        GT_FALSE, /* ISR context */
                                        CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
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
* hwPpReadBurst
*
* DESCRIPTION:
*      This function performs a burst read from a given device's Ram address.
*
* INPUTS:
*       devNum          - The PP device number to read from.
*       portGroupId  - The port group Id.
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
    IN  GT_U8               devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32              srcAddr,
    IN  GT_U32              length,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo,
    OUT GT_U32              *data
)
{
    GT_U8       addrCompIdx;    /* Index of the address completion byte to  */
                                /* be used for this pci access.             */
    GT_UINTPTR      pciAddr;        /* The address to be used for pci access,   */
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
    retVal = setAddrCompletion(devNum,portGroupId,allignSrcAddr,&addrCompIdx,&pciAddr,
                               &signalSem);
    if(retVal != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* Update allignSrcAddr with the address completion part.   */
    allignSrcAddr = (allignSrcAddr & 0xFFFFFF) | (pciAddr & 0xFF000000);

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
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
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
* prvCpssDrvHwPciDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes PCI device driver object
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
GT_STATUS prvCpssDrvHwPciDriverObjectInit
(
     void
)
{
#ifdef INCL_EXDXMX_DRIVER
     /* driver object initialization */
    prvCpssDrvMngInfPciPtr->drvHwCntlInit          = driverHwPciCntlInit;
    prvCpssDrvMngInfPciPtr->drvHwPpReadReg         = hwPpPciReadRegister;
    prvCpssDrvMngInfPciPtr->drvHwPpWriteReg        = hwPpPciWriteRegister;
    prvCpssDrvMngInfPciPtr->drvHwPpGetRegField     = hwPpPciGetRegField;
    prvCpssDrvMngInfPciPtr->drvHwPpSetRegField     = hwPpPciSetRegField;
    prvCpssDrvMngInfPciPtr->drvHwPpReadRegBitMask  = hwPpPciReadRegBitMask;
    prvCpssDrvMngInfPciPtr->drvHwPpWriteRegBitMask = hwPpPciWriteRegBitMask;
    prvCpssDrvMngInfPciPtr->drvHwPpReadRam         = hwPpPciReadRam;
    prvCpssDrvMngInfPciPtr->drvHwPpWriteRam        = hwPpPciWriteRam;
    prvCpssDrvMngInfPciPtr->drvHwPpReadVec         = hwPpPciReadVec;
    prvCpssDrvMngInfPciPtr->drvHwPpWriteVec        = hwPpPciWriteVec;
    prvCpssDrvMngInfPciPtr->drvHwPpWriteRamRev     = hwPpPciWriteRamInReverse;
    prvCpssDrvMngInfPciPtr->drvHwPpIsrRead         = hwPpPciIsrRead;
    prvCpssDrvMngInfPciPtr->drvHwPpIsrWrite        = hwPpPciIsrWrite;
    prvCpssDrvMngInfPciPtr->drvHwPpReadIntPciReg   = hwPpPciReadInternalPciReg;
    prvCpssDrvMngInfPciPtr->drvHwPpWriteIntPciReg  = hwPpPciWriteInternalPciReg;
    prvCpssDrvMngInfPciPtr->drvHwPpResetAndInitControllerReadReg = 
                                           hwPpPciResetAndInitCtrlReadRegister;
    prvCpssDrvMngInfPciPtr->drvHwPpResetAndInitControllerWriteReg = 
                                           hwPpPciResetAndInitCtrlWriteRegister;
    prvCpssDrvMngInfPciPtr->drvHwPpResetAndInitControllerGetRegField = 
                                           hwPpPciResetAndInitCtrlGetRegField;
    prvCpssDrvMngInfPciPtr->drvHwPpResetAndInitControllerSetRegField = 
                                           hwPpPciResetAndInitCtrlSetRegField;
#endif /*INCL_EXDXMX_DRIVER*/
    return GT_OK;
}



/********************************************************************************
* !!!!!!!!!!!!!!!!!!!!! FOR DEBUG PURPOSES ONLY !!!!!!!!!!!!!!!!!!!!!!!!!!
********************************************************************************/
/*#define DEBUG_BURST*/
#ifdef DEBUG_BURST

GT_STATUS cpssReverseTest2(GT_U32 startAddr, GT_U32 size)
{
    GT_U32  i;
    GT_U32  *data = cpssOsMalloc(size*4);
    if (!data)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    for(i = 0; i < size; i++)
    {
        data[i] = i*4;
    }
    prvCpssDrvHwPpWriteRam(0,startAddr, size,data);
    prvCpssDrvHwPpWriteRamInReverse(1, startAddr, size,data);
    cpssOsFree(data);
    return GT_OK;
}


GT_STATUS cpssReverseTest(GT_U32 startAddr, GT_U32 size)
{
    GT_U32  i;
    GT_U32  data[2] = {0x0, 0x1};

    for(i = 0; i < size; i += 8)
    {
        data[0] = i;
        data[1] = i+4;
        prvCpssDrvHwPpWriteRam(0,startAddr + i, 2,data);
        prvCpssDrvHwPpWriteRamInReverse(1, startAddr + i, 2,data);
    }
    return GT_OK;
}


GT_STATUS cpssPerformanceTest(GT_U32 startAddr,GT_U32 size,GT_U32 isRead)
{
    GT_U32  i;
    GT_U32  data[2];

    for(i = 0; i < size; i+=8)
    {
        if(isRead == 0)
        {
            data[0] = i;
            data[1] = i+1;
            prvCpssDrvHwPpWriteRam(0,startAddr + i,2,data);
        }
        else
        {
            prvCpssDrvHwPpReadRam(0,startAddr + i,2,data);
        }
    }
    return GT_OK;
}

GT_STATUS cpssNewDmaTest(GT_U32 address,GT_U32 length,GT_U32 val,GT_U32 isRead)
{
    GT_U32  *data;
    GT_U32  i;

    data = cpssOsMalloc(length * sizeof(GT_U32));
    if(data == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    if(isRead == 1)
    {
        cpssOsMemSet(data,0,sizeof(GT_U32) * length);
    }
    else
    {
        for(i = 0; i < length; i++)
        {
            data[i] = val + i;
        }
    }

    cpssOsPrintf("data - 0x%x.\n",data);

    if(isRead == 1)
    {
        if(prvCpssDrvHwPpReadRam(0,address,length,data) != GT_OK)
            cpssOsPrintf("Failed.\n");
        for(i = 0; i < length; i++)
        {
            cpssOsPrintf("data[%d] = 0x%x.\n",i,data[i]);
        }
    }
    else
    {
        if(prvCpssDrvHwPpWriteRam(0,address,length,data) != GT_OK)
            cpssOsPrintf("Failed.\n");
    }

    /*osFree(data);*/
    return GT_OK;

}

GT_STATUS cpssTestWriteBurst
(
    IN GT_U32   address,
    IN GT_U32   length,
    IN GT_U32   val
)
{
    GT_U32  *data;
    GT_U32  i;
    GT_U8   idx;
    GT_UINTPTR  pciAddr;
    GT_BOOL signalSem;
    GT_U32  portGroupId = 0;

    setAddrCompletion(0,portGroupId,address,&idx,&pciAddr,&signalSem);

    data = cpssOsMalloc(length * sizeof(GT_U32));
    if(data == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /*data = (GT_U32*)((((GT_U32)data) + 32) & 0xFFFFFFF0);*/

    for(i = 0; i < length; i++)
    {
        data[i] = val + i;
        data[i] = CPSS_32BIT_LE(data[i]);
    }

    cpssOsPrintf("data - 0x%x.\n",data);
    if(cpssExtDrvDmaWrite(pciAddr,data,length,2) != GT_OK)
        cpssOsPrintf("Failed.\n");

    /*osFree(data);*/
    return GT_OK;
}



GT_STATUS testReadBurst
(
    IN GT_U32   address,
    IN GT_U32   length
)
{
    GT_U32  *data;
    GT_U32  i;
    GT_U8   idx;
    GT_UINTPTR  pciAddr;
    GT_BOOL signalSem;
    GT_U32  portGroupId = 0;

    setAddrCompletion(0,portGroupId,address,&idx,&pciAddr,&signalSem);

    data = cpssOsMalloc(length * sizeof(GT_U32));
    if(data == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /*data = (GT_U32*)((((GT_U32)data) + 32) & 0xFFFFFFF0);*/

    cpssOsMemSet(data,0,sizeof(length * sizeof(GT_U32)));

    cpssOsPrintf("data - 0x%x.\n",data);
    if(cpssExtDrvDmaRead(pciAddr,length,2,data) != GT_OK)
        cpssOsPrintf("Failed.\n");

    for(i = 0; i < length; i++)
    {
        data[i] = CPSS_32BIT_LE(data[i]);
        cpssOsPrintf("data[%d] = 0x%x.\n",i,data[i]);
    }

    /*osFree(data);*/
    return GT_OK;
}

#endif /* DEBUG_BURST */





