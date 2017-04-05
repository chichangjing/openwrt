/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpHwPciCntlSim.c
*
* DESCRIPTION:
*       Prestera driver Hardware Simulation for PCI read and write functions
*       implementation.
*
*
* FILE REVISION NUMBER:
*       $Revision: 19 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <asicSimulation/SCIB/scib.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

extern GT_BOOL  prvCpssDrvTraceHwWrite[PRV_CPSS_MAX_PP_DEVICES_CNS];
extern GT_BOOL  prvCpssDrvTraceHwRead[PRV_CPSS_MAX_PP_DEVICES_CNS];

#define debugPrint(_x)\
    if(memoryAccessTraceOn == GT_TRUE)\
       cpssOsPrintf _x

/* Macro to convert device ID to simulation device ID */
#define SIM_DEV_ID(dev_num,_portGroupId)     \
    (prvCpssDrvPpConfig[(dev_num)]->hwCtrl[_portGroupId].simInfo.devId)

#define SIM_ADDR_COMPL_EN_GET(dev_num,_portGroupId)  \
    (prvCpssDrvPpConfig[(dev_num)]->hwCtrl[_portGroupId].simInfo.addrCompletEnable)

/*******************************************************************************
* Global Hw configuration params.
*******************************************************************************/

/*******************************************************************************
* Intenal Macros and definitions
*******************************************************************************/

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

static GT_STATUS hwPpPciReadInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);

static GT_STATUS hwPpPciWriteInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
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
    IN GT_BOOL  isDiag
)
{
    GT_U8   i;
    char    name[20];
    GT_U32  address;     /* Physical address for PCI transaction */
    GT_U32  tmpData;

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].simInfo.devId =
                            (GT_U8)scibGetDeviceId(baseAddr);

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].simInfo.addrCompletEnable =
                            scibAddressCompletionStatusGet(SIM_DEV_ID(devNum,portGroupId));

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr           = baseAddr;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx            = 2;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase    = internalPciBase;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[0]   = 0;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[1]   = 0;

    if (SIM_ADDR_COMPL_EN_GET(devNum,portGroupId) == GT_TRUE)
    {
        /* Reset Hw Address Completion          */
        address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr + ADDR_COMP_REG;

        tmpData = 0;


        scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &tmpData);
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

    if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_SLAVE_UNABLE_TO_ABORT_TRANSACTION_E) == GT_TRUE)
    {
        /* Initialize the PCI Timer and Retry register with 0xFF.   */
        /* Perform a read modify write on the register.             */
        hwPpPciReadInternalPciReg(devNum,portGroupId,0x104,&tmpData);

        tmpData = (tmpData & 0xFFFF0000) | 0xFFFF;  /*set both timer values to maximum */

        hwPpPciWriteInternalPciReg(devNum,portGroupId,0x104,tmpData);
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
    GT_U32  address;  /*physical address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U8   i;      /* count iterator for the complition index compare loop*/
    GT_U32  j;      /* count iterator for the write loop*/
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

    if (SIM_ADDR_COMPL_EN_GET(devNum,portGroupId) == GT_FALSE)
    {
        address = regAddr;

        for(j = 0; j < length; j++)
        {
            data = CPSS_32BIT_LE(dataPtr[j]);


            scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

            address += 4;
        }

        /* Check if trace hw write is enabled */
        if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
        {
            return cpssTraceHwAccessWrite(devNum,
                                          portGroupId,
                                          GT_FALSE, /* ISR context */
                                          CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                          regAddr,
                                          length, /* length to trace the data in words */
                                          dataPtr);
        }

        return GT_OK;
    }

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
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx =
                (GT_U8)(((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx - 1) % 2)+ 2);

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


            scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);
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
            for(j = 0; j < length; j++)
            {
                data = CPSS_32BIT_LE(dataPtr[j]);


                scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

                address += 4;
            }

            cpssOsMutexUnlock(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
        }
        else
        {
            loopLength = (nextRegionAddr - regAddr) / 4;
            for(j = 0; j < loopLength; j++)
            {
                data = CPSS_32BIT_LE(dataPtr[j]);


                scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

                address += 4;
            }
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
            for(j = 0; j < length; j++)
            {
                data = CPSS_32BIT_LE(dataPtr[j]);


                scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

                address += 4;
            }
        }
        else
        {
            loopLength = (0x1000000 - regAddr) / 4;
            for(j = 0; j < loopLength; j++)
            {
                data = CPSS_32BIT_LE(dataPtr[j]);


                scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

                address += 4;
            }
            /* recursive call for rest of data in next region */
            hwWrite(devNum,portGroupId,0x1000000,length-loopLength,dataPtr+loopLength);
        }
    }

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
*       length  - Number of writes in 4 byte each.
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
    GT_U32  address;  /*physical address for PCI access */
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

    if (SIM_ADDR_COMPL_EN_GET(devNum,portGroupId) == GT_FALSE)
    {
        hwWrite(devNum,portGroupId, regAddr, length, dataPtr);

        return GT_OK;
    }

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
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx =
                (GT_U8)(((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx - 1) % 2)+ 2);

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


            scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

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
                data = CPSS_32BIT_LE(dataPtr[j]);


                scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

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
                data = CPSS_32BIT_LE(dataPtr[j]);


                scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

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
            for(j = length-1; j >= 0; j--)
            {
                data = CPSS_32BIT_LE(dataPtr[j]);


                scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

                address -= 4;
            }
        }
        else
        {
            loopLength = (0x1000000 - regAddr) / 4;
            /* recursive call for rest of data in next region */
            hwWrite(devNum,portGroupId,0x1000000,length-loopLength,dataPtr+loopLength);
            for(j = loopLength-1; j >= 0; j--)
            {
                data = CPSS_32BIT_LE(dataPtr[j]);


                scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);

                address -= 4;
            }
        }
    }

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
    GT_U8   addrRegion; /* 8 bit MSB value of PP internal address*/
    GT_U32  address;  /*physical access address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U32  temp;
    GT_U32  i;      /* count iterator for the complition index compare loop*/
    GT_U32  j;      /* count iterator for the write loop*/
    GT_U32  nextRegionAddr; /* address of the next region after the one
                                currently used */
    GT_U32  loopLength = 0; /* when length exceeds region addr, Set to end of
                            region range */
    GT_U32  data;   /* data to be write to memory */
    GT_U32  traceLength;    /* length to trace the data */

    if (SIM_ADDR_COMPL_EN_GET(devNum,portGroupId) == GT_FALSE)
    {
        address = regAddr;

        for(j = 0;j < length;j++)
        {
            scibReadMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &temp);

            dataPtr[j] = CPSS_32BIT_LE(temp);
            address += 4;
        }

        if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
        {
            return cpssTraceHwAccessRead(devNum,
                                         portGroupId,
                                         GT_FALSE, /* ISR context */
                                         CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                         regAddr,
                                         length, /* length to trace the data in words */
                                         dataPtr);
        }

        return GT_OK;
    }

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
        {   /* Set addrRegion in AddrComplition register */

            /*round robin on Region index : 2,3*/
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx =
                (GT_U8)(((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx -1) % 2) + 2); /*round
                                                                      robin
                                                                    - 1,2,3*/
            compIdx = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx;

                      /*update Address Complition shadow*/
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] =
                addrRegion;

            /* update Hw Address Completion - using completion region 0 */
            address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                    ADDR_COMP_REG;
            data = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1]<<8) |
                   (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2]<<16) |
                   (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3]<<24);

            data = CPSS_32BIT_LE(data);


            scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);
        }
        else
        {
            compIdx = (GT_U8)i;
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
                scibReadMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &temp);

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
                scibReadMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &temp);

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
                scibReadMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &temp);

                dataPtr[i] = CPSS_32BIT_LE(temp);
                address += 4;
            }
        }
        else
        {
            loopLength =  (0x1000000 - regAddr)/4;
            for(j = 0;j < loopLength ;j++)
            {
                scibReadMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &temp);

                dataPtr[j] = CPSS_32BIT_LE(temp);
                address += 4;
            }

            /* recursive call for rest of data in next region */
            hwRead(devNum,portGroupId,0x1000000,length-loopLength,dataPtr + loopLength);
        }
    }


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
    debugPrint(("Read on device %d, from register 0x%08x, ",devNum, regAddr));
    hwRead (devNum, portGroupId,regAddr, 1, data);
    debugPrint((" data 0x%08x.\n", *data));

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
    debugPrint(("Write on device %d, to register 0x%08x, data 0x%08x.\n",
                devNum, regAddr, value));

    hwWrite (devNum, portGroupId,regAddr, 1, &value);

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
    GT_U32 data, mask;

    debugPrint(("GetField on device %d, from register 0x%08x, offset %02d, length %02d, ",
                devNum, regAddr, fieldOffset, fieldLength));

    hwRead( devNum, portGroupId,regAddr, 1,&data);
    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    *fieldData = (GT_U32)((data & mask) >> fieldOffset);

    debugPrint(("data 0x%08x.\n", *fieldData));

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
    GT_U32 data, mask;
    debugPrint(("SetField on device %d, to register 0x%08x, offset %02d, length %02d, data 0x%08x.\n",
                devNum, regAddr, fieldOffset, fieldLength, fieldData));

    hwRead( devNum, portGroupId,regAddr, 1,&data);
    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    data &= ~mask;   /* turn the field off */
    /* insert the new value of field in its place */
    data |= ((fieldData << fieldOffset) & mask);

    hwWrite( devNum,portGroupId, regAddr, 1, &data);

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
    GT_U32 data;

    debugPrint(("hwPpReadRegBitMask on device %d, from register 0x%08x, mask 0x%08x,",
                devNum, regAddr, mask));

    hwRead ( devNum, portGroupId,regAddr, 1,&data);
    *dataPtr = (data) & mask;

    debugPrint((" data 0x%08x.", *dataPtr));
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
    GT_U32 data;

    debugPrint(("hwPpWriteRegBitMask on device %d, from register 0x%08x, mask 0x%08x, data 0x%08x.\n",
                devNum, regAddr, mask, value));

    /* in case mask = 0xffffffff call hwWrite only */
    if(mask == 0xffffffff)
    {
        hwWrite (devNum, portGroupId,regAddr, 1, &value);
        return GT_OK;
    }
    /* else read modify write */
    hwRead ( devNum,portGroupId, regAddr, 1, &data);
    data &= (~mask);            /* Turn the field off.                        */
    data |= (value & mask);     /* Insert the new value of field in its place.*/

    hwWrite (devNum,portGroupId, regAddr, 1, &data);
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
        debugPrint(("hwPpReadRam on device %d, from address 0x%08x, data 0x%08x.\n",
                    devNum, addr + 4 * i, data[i]));
    }

    ramBurstInfo = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo;
    for(i = 0; i < prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfoLen; i++)
    {
        if(ramBurstInfo[i].ramBase == (ramBurstInfo[i].ramBaseMask & addr))
            return hwPpReadBurst(devNum,portGroupId,addr,length,&(ramBurstInfo[i]),data);
    }

    hwRead (devNum,portGroupId, addr, length, data);
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
            debugPrint(("hwPpWriteRam on device %d, to address 0x%08x, data 0x%08x.\n",
                        devNum, addr + i * 4, data[i]));
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
*       Read from PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id.
*       addrArr - Address array to read from.
*       arrLen  - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr - An array containing the read data.
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
        debugPrint(("hwPpPciReadVec on device %d, from address 0x%08x, data 0x%08x.\n",
                    devNum, addrArr[i], dataArr[i]));
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpPciWriteVec
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
*       addrArr - Address offset to write to.
*       dataArr - An array containing the data to be written.
*       arrLen  - The size of addrArr/dataArr.
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
        debugPrint(("hwPpPciWriteVec on device %d, to address 0x%08x, data 0x%08x.\n",
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
    GT_U32              i;
    if(memoryAccessTraceOn == GT_TRUE)
    {
        for( i = 0; i < length; i++)
        {
            debugPrint(("hwPpWriteRam on device %d, to address 0x%08x, data 0x%08x.\n",
                        devNum, addr + i * 4, data[i]));
        }
    }
    status = hwWriteInReverse(devNum,portGroupId, addr, length, data);
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
    GT_U32  addrData;    /* physical address for PCI transaction */
    GT_U8   compIdx;    /* address completion register field Isr index = 1*/
    GT_U32  temp, addrCompletion;
    GT_U32  dataCompletion;

    if (SIM_ADDR_COMPL_EN_GET(devNum,portGroupId) == GT_FALSE)
    {
        scibReadMemory(SIM_DEV_ID(devNum,portGroupId), regAddr, 1, &temp);

        *dataPtr = CPSS_32BIT_LE(temp);

        /* Check if trace hw read is enabled */
        if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
        {
            /* trace HW read access */
            return cpssTraceHwAccessRead(devNum,
                                         portGroupId,
                                         GT_TRUE,    /* ISR context */
                                         CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,   /* Address space */
                                         regAddr,
                                         1,         /* length to trace the data in words */
                                         dataPtr);
        }

        return GT_OK;
    }

    /* Set addrRegion in AddrComplition register = 1 */
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

        /* Update Address Completion shadow.            */
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] = addrRegion;

        /* Update Hw Address Completion Region 1- using region 0. */
        dataCompletion =
                ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1] << 8) |
                (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2] << 16) |
                (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3] << 24));

        /* write the address completion 3 times.
        because the PP have a 2 entry write buffer
        so, the 3 writes will make sure we do get
        to the hardware register itself */

        dataCompletion = CPSS_32BIT_LE(dataCompletion);


        scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), addrCompletion, 1, &dataCompletion);

        addrData = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr) + ((compIdx << 24) |
                   (regAddr & 0x00ffffff)));
    }
    /* read the data from ASIC */
    scibReadMemory(SIM_DEV_ID(devNum,portGroupId), addrData, 1, &temp);
    *dataPtr = CPSS_32BIT_LE(temp);


    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        /* trace HW read access */
        return cpssTraceHwAccessRead(devNum,
                                     portGroupId,
                                     GT_TRUE,    /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,   /* Address space */
                                     regAddr,
                                     1,         /* length to trace the data in words */
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
    GT_U32  address;    /*physical address for PCI transaction */
    GT_U8   compIdx;    /* address completion register field Isr index = 1*/
    GT_U32  data;       /* data to be write to memory */
    GT_U32  tmpValue;
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    if (SIM_ADDR_COMPL_EN_GET(devNum,portGroupId) == GT_FALSE)
    {
        tmpValue = CPSS_32BIT_LE(value);


        scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), regAddr, 1, &tmpValue);

        /* Check if trace hw write is enabled */
        if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
        {
            /* trace HW write access */
            return cpssTraceHwAccessWrite(devNum,
                                          portGroupId,
                                          GT_TRUE,  /* ISR context */
                                          CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,  /* Address space */
                                          regAddr,
                                          1,        /* length to trace the data in words */
                                          &value);
        }

        return GT_OK;
    }

    compIdx = 1;
    addrRegion = (GT_U8)(regAddr >> 24);

    if (addrRegion == 0)
    {
        address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr + regAddr;
    }
    else  /* use Region #1 */
    {
        GT_U32   addrCompletion;

        addrCompletion = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr +
                            ADDR_COMP_REG);

        /* Update Address Completion shadow.            */
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[compIdx] = addrRegion;

        /* Update Hw Address Completion Regin 1- using Region 0 */
        data = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[1] << 8) |
                (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[2] << 16) |
                (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[3] << 24));

        tmpValue = CPSS_32BIT_LE(data);


        scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), addrCompletion, 1, &tmpValue);

        address = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr) + ((compIdx << 24) |
                   (regAddr & 0x00ffffff)));

    }

    tmpValue = CPSS_32BIT_LE(value);


    scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &tmpValue);

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* trace HW write access */
        return cpssTraceHwAccessWrite(devNum,
                                      portGroupId,
                                      GT_TRUE,   /* ISR context */
                                      CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,  /* Address space */
                                      regAddr,
                                      1,        /* length to trace the data in words */
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
    GT_U32 temp;

    scibPciRegRead(SIM_DEV_ID(devNum,portGroupId), regAddr, 1, &temp);

    *data = CPSS_32BIT_LE(temp);

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
    {
        /* trace HW read access */
        return cpssTraceHwAccessRead(devNum,
                                     portGroupId,
                                     GT_FALSE,  /* ISR context */
                                     CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E,   /* Address space */
                                     regAddr,
                                     1,     /* length to trace the data in words */
                                     data);
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpPciWriteInternalPciReg
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
static GT_STATUS hwPpPciWriteInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
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

    scibPciRegWrite(SIM_DEV_ID(devNum,portGroupId), regAddr, 1, &temp);

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE)
    {
        /* trace HW write access */
        return cpssTraceHwAccessWrite(devNum,
                                      portGroupId,
                                      GT_FALSE,  /* ISR context */
                                      CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E,   /* Address space */
                                      regAddr,
                                      1,     /* length to trace the data in words */
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
    GT_U32 temp;

    scibMemoryClientRegRead(SIM_DEV_ID(devNum,0), 
                            SCIB_MEM_ACCESS_DFX_E, 
                            regAddr, 1, &temp);

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
    GT_U32 temp;
    if ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E ) )
    {
        /* we are in HSU or HA process -- hardware should not be affected */
        return GT_OK;
    }

    debugPrint(("Reset and Init Controller Write: addr 0x%X data 0x%X \n", regAddr, data));

    temp = CPSS_32BIT_LE(data);

    scibMemoryClientRegWrite(SIM_DEV_ID(devNum,0), 
                             SCIB_MEM_ACCESS_DFX_E, 
                             regAddr, 1, &temp);

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
    GT_U32 data = 0, mask;

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
    GT_U32 data = 0, mask;

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
    /* fix warning */
    enable = enable;

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
    OUT GT_U32  *pciAddr,
    OUT GT_BOOL *signalSem
)
{
    GT_U8   addrRegion; /* 8 bit MST value of PP internal address*/
    GT_U32  address;  /*physical access address for PCI access */
    GT_U8   compIdx; /* address completion register field index 0-3*/
    GT_U8   i;      /* count iterator for the completion index compare loop*/
    GT_U32  data;

    *signalSem = GT_FALSE;

    if (SIM_ADDR_COMPL_EN_GET(devNum,portGroupId) == GT_FALSE)
    {
        *pciAddr = regAddr;
        return GT_OK;
    }

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
            /* Set addrRegion in AddrComplition register.   */
            /* round robin on Region index : 2,3            */
            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx =
                (GT_U8)(((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx - 1) % 2) + 2); /* round
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


            scibWriteMemory(SIM_DEV_ID(devNum,portGroupId), address, 1, &data);
        }
        else
        {
            compIdx = i;
        }

        *signalSem = GT_TRUE;
        *pciAddr = ((prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr) | (compIdx << 24) |
                    (regAddr & 0x00ffffff));
    }
    else
    {
        address = (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr |
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
    GT_U32      pciAddr;        /* The address to be used for pci access,   */
                                /* after setting the address completion     */
                                /* register.                                */
    GT_BOOL     signalSem;      /* Should the semaphore be signaled after   */
                                /* the burst operation.                     */

    GT_U32      tempBurstBuff[PRV_CPSS_DRV_MAX_BURST_BLOCK_SIZE_CNS];/* A temporary buffer   */
    GT_U32      *tempData;      /* used to hold the data to be written in   */
                                /* case a read-modify write operation is    */
                                /* needed.                                  */
    GT_U32      words2Allign;   /* Number of additional words need to be    */
                                /* read to reach the needed allignment for  */
                                /* this Ram.                                */
    GT_U32      allignDestAddr; /* The alligned destination address.        */
    GT_U32      allignSize;     /* The actual size to be read as a result   */
                                /* of the allignment.                       */
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
    /* Take any possible needed allignments into account.               */
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

    /* Set the destination address to an alligned address   */
    words2Allign    = (destAddr / sizeof(GT_U32)) % ramBurstInfo->burstSize;
    allignDestAddr  = destAddr - (words2Allign * sizeof(GT_U32));
    allignSize      = length + words2Allign;

    /* Check if the end of the block is properly alligned.  */
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

    /* Update allignDstAddr with the address compeltion part.   */
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

            scibReadMemory(SIM_DEV_ID(devNum,portGroupId), tempDestAddr, sizeOfBurst, tempSrcAddr);
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


        scibWriteMemory(devNum, tempDestAddr, sizeOfBurst, tempSrcAddr);
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
                                        GT_FALSE,     /* ISR context */
                                        CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,     /* Address space */
                                        allignDestAddr,
                                        allignSize,   /* length to trace the data in words */
                                        data);
        if(retVal != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
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
    GT_U32      pciAddr;        /* The address to be used for pci access,   */
                                /* after setting the address completion     */
                                /* register.                                */
    GT_BOOL     signalSem;      /* Should the semaphore be signaled after   */
                                /* the burst operation.                     */
    GT_U32      tempBurstBuff[PRV_CPSS_DRV_MAX_BURST_BLOCK_SIZE_CNS];/* A temporary buffer   */
    GT_U32      *tempData;      /* used to hold the data to be written in   */
                                /* case a read-modify write operation is    */
                                /* needed.                                  */
    GT_U32      words2Allign;   /* Number of additional words need to be    */
                                /* read to reach the needed allignment for  */
                                /* this Ram.                                */
    GT_U32      allignSrcAddr;  /* The alligned source address.             */
    GT_U32      allignSize;     /* The actual size to be read as a result   */
                                /* of the allignment.                       */
    GT_U32      *tempDstAddr;   /* Used when multiple burst are needed to   */
    GT_U32      tempSrcAddr;    /* perform the read operation, i.e addrRes- */
                                /* -olution != 1.                           */
    GT_U32      numOfBursts;    /* Number of bursts to be performed, used   */
                                /* when addrResolution != 1.                */
    GT_U32      sizeOfBurst;    /* Size of each burst.                      */
    GT_U32      i;
    GT_STATUS   retVal;

    /* Set the destination address to an alligned address   */
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

    /* Update allignSrcAddr with the address compeltion part.   */
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

        scibReadMemory(SIM_DEV_ID(devNum,portGroupId), tempSrcAddr, sizeOfBurst, tempDstAddr);
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
                                       allignSize,   /* length to trace the data in words */
                                       data);
        if(retVal != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
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
*       devNum          - The PP device number to read from.
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
    GT_U32  pciAddr;
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
    GT_U32  pciAddr;
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

/*******************************************************************************
* prvCpssDrvDevNumToSimDevIdConvert
*
* DESCRIPTION:
*       convert CPSS devNum,coreId to simulation deviceId as used by the SCIB
*       layer of the simulation
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id.
*                     relevant only to 'multi-port-groups' devices.
*                     (NOT Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - on bad devNum,portGroupId
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS   prvCpssDrvDevNumToSimDevIdConvert(
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
    OUT GT_U32      *simDeviceIdPtr
)
{
    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);
    CPSS_NULL_PTR_CHECK_MAC(simDeviceIdPtr);

    if(portGroupId >= CPSS_MAX_PORT_GROUPS_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    (*simDeviceIdPtr) = SIM_DEV_ID(devNum,portGroupId);

    return GT_OK;

}
