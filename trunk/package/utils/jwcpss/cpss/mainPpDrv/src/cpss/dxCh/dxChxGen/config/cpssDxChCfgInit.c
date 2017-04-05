/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCfgInit.c
*
* DESCRIPTION:
*       Core initialization of PPs and shadow data structures, and declarations
*       of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 109 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChCfg.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpm.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/catchUp/private/prvCpssDxChCatchUp.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcam.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChConfigLog.h>

/* Object that holds calbback function to table HW access */
extern  CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC prvDxChTableCpssHwAccessObj;
/* get number of entries of table that HW supports :

_tableType - one of PRV_CPSS_DXCH_TABLE_ENT
*/
#define TABLE_NUM_ENTRIES_GET_MAC(_devNum, _tableType) \
    ((_tableType < PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize) ?  \
        /* table in valid range of the device */                         \
        PRV_CPSS_DXCH_PP_MAC(_devNum)->accessTableInfoPtr[_tableType].maxNumOfEntries : \
    0)/* unknown table for the device*/

/*******************************************************************************
 * Internal usage variables
 ******************************************************************************/

/* number of entries in the cheetah3 mac2me table */
#define PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS 8
#define PRV_CPSS_LION3_MAC2ME_TABLE_MAX_ENTRIES_CNS 128

/* table not valid --> 0 entries */
#define TABLE_NOT_VALID_CNS     0

/* define the number of different FDB messages queues that we may use:
    1. primary AUQ
    2. additional primary AUQ
    3. primary FUQ
    4. secondary AUQ for primary first AUQ
    5. secondary AUQ for primary second AUQ
*/
#define MESSAGE_QUEUE_NUM_CNS   5

#define QUEUE_NOT_USED_CNS  0xFF

#define DEVICE_NOT_EXISTS_CNS 0xFF


/*******************************************************************************
* internalCfgAddrUpInit
*
* DESCRIPTION:
*       This function initializes the Core Address Update mechanism, by
*       allocating the AU descs. block.
*       supports 'per port group'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to init the Au unit for.
*       portGroupId  - the port group Id
*       auDescBlockPtr  - (pointer to)A block to allocate the Au descriptors from.
*       auDescBlockSize - Size in bytes of Au descBlock.
*       fuDescBlockPtr  - (pointer to)A block to allocate the Fu descriptors from.
*                         Valid if useFuQueue is GT_TRUE.
*       fuDescBlockSize - Size in bytes of Fu descBlock.
*                         Valid if useFuQueue is GT_TRUE.
*       useFuQueue      - GT_TRUE  - Fu queue is used - for DxCh2 devices only.
*                       - GT_FALSE - Fu queue is unused.
*       au1DescBlockPtr - (pointer to)A block to allocate the additional Au descriptors from.
*       au1DescBlockSize
*                       - Size in bytes of Au1 descBlock.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internalCfgAddrUpInit
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U8    *auDescBlockPtr,
    IN GT_U32   auDescBlockSize,
    IN GT_U8    *fuDescBlockPtr,
    IN GT_U32   fuDescBlockSize,
    IN GT_BOOL  useFuQueue,
    IN GT_U8    *au1DescBlockPtr,
    IN GT_U32   au1DescBlockSize
)
{
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrl[MESSAGE_QUEUE_NUM_CNS] = {NULL};    /* Points to the desc. list. */
    PRV_CPSS_AU_DESC_CTRL_STC   *tmpDescCtrl[MESSAGE_QUEUE_NUM_CNS] = {NULL}; /* Points to the desc. list. */
    GT_UINTPTR                  phyAddr;/* The physical address of the AU block*/
    GT_STATUS                   retVal;
    GT_U32                      numOfIter = 0;/* the number of iterations in initialization */
    GT_U32                      ii;          /* iterator */
    PRV_CPSS_AU_DESC_STC        *addrUpMsgPtr = NULL;   /* update message address */
    GT_U32                      *auMemPtr = NULL;/* pointer to the SDMA memory , to start of current message */
    GT_U8                       *descBlock[MESSAGE_QUEUE_NUM_CNS] = {NULL};  /* A block to allocate the Au/Fu descriptors from.*/
    GT_U32                      descBlockSize[MESSAGE_QUEUE_NUM_CNS] = {0}; /* Size in bytes of Fu/Au descBlock. */
    GT_BOOL                     initHwPointerAfterShutdown = GT_FALSE;
    GT_BOOL                     didHwReset;/* are we after HW reset or only after 'cpss SW re-init' (without HW reset since last call of this function)*/
    GT_U32                      alignmentNumBytes;/* number of bytes for alignment movement*/
    GT_U32                      auqIndex = 0;/*index for the AU in the arrays of : descCtrl,descBlock,descBlockSize*/
    GT_U32                      auq1Index = QUEUE_NOT_USED_CNS;/*index for the additional AU in the arrays of : descCtrl,descBlock,descBlockSize*/
    GT_U32                      fuqIndex = 0;/*index for the FU in the arrays of : descCtrl,descBlock,descBlockSize*/
    GT_U32  addrUpdateQueueCtrl[MESSAGE_QUEUE_NUM_CNS] = {0};
    GT_U32  addrUpdateQueueBaseAddr[MESSAGE_QUEUE_NUM_CNS]= {0};
    GT_U32  secondaryAuqIndex = QUEUE_NOT_USED_CNS;/* index in the descCtrl[] that the secondary uses ,
                                    initialized with unreachable index value */
    GT_U32  secondaryAuq1Index = QUEUE_NOT_USED_CNS;/* index in the descCtrl[] that the secondary for additional AUQ uses,
                                    initialized with unreachable index value */
    GT_U8   *secondaryAuqDescBlockPtr = NULL;/*pointer to the secondary AUQ block */
    GT_U32  secondaryAuqDescBlockSize;/*size in bytes of the secondary AUQ block */
    GT_U32  regAddr;                  /* register address */
    GT_U32 auMessageNumBytes; /* number of bytes in AU/FU message */
    CPSS_SYSTEM_RECOVERY_INFO_STC  tempSystemRecoveryInfo;
    GT_BOOL auqRecoverySkipInit = GT_FALSE;
    GT_BOOL fuqRecoverySkipInit = GT_FALSE;
    GT_BOOL doSdma;/* do SDMA or onChipFifo */
    GT_BOOL initAuq = GT_TRUE;/* indication to init the AUQ on the device */

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        if(portGroupId != PRV_CPSS_FIRST_ACTIVE_PORT_GROUP_ID_MAC(devNum))
        {
            /* the AUQ should not be set on this port group */
            initAuq = GT_FALSE;
        }
    }


    tempSystemRecoveryInfo = systemRecoveryInfo; /* save recovery state*/
    if (tempSystemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E)
    {
        /* recovery process is in progress */
        if(tempSystemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_TRUE)
        {
          /* auq  continues to receive packets, no init required  */
            auqRecoverySkipInit = GT_TRUE;
        }
        if(tempSystemRecoveryInfo.systemRecoveryMode.continuousFuMessages == GT_TRUE)
        {
            /* fuq  continues to receive packets, no init required  */
            fuqRecoverySkipInit = GT_TRUE;
        }
        if ((auqRecoverySkipInit == GT_FALSE) ||(fuqRecoverySkipInit == GT_FALSE))
        {
            /* in this case we need perform hw write for full initialization */
            systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
        }
    }

    auMessageNumBytes = 4 * PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.auMessageNumOfWords;

    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                   info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                       enabled == GT_FALSE))
    {
        doSdma = GT_TRUE;
    }
    else
    {
        doSdma = GT_FALSE;
    }

    if(doSdma == GT_TRUE)
    {
        if (initAuq == GT_TRUE && auqRecoverySkipInit == GT_FALSE)
        {
            numOfIter = 0;
            if(auDescBlockPtr == NULL)
            {
                /* no Hw changes needed */
                /* nothing more to do */
                return GT_OK;
            }

            auqIndex = numOfIter;

            /* AUQ registers */
            addrUpdateQueueCtrl[numOfIter] =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
            addrUpdateQueueBaseAddr[numOfIter] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;

            descCtrl[auqIndex] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]);
            descBlock[auqIndex] = auDescBlockPtr;
            descBlockSize[auqIndex] = auDescBlockSize;

            numOfIter++;

            /* Initialize primary AUQ1 descriptor block */
            if(au1DescBlockPtr)
            {
                auq1Index = numOfIter;

                /* AUQ registers */
                addrUpdateQueueCtrl[numOfIter] =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;

                addrUpdateQueueBaseAddr[numOfIter] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;

                descCtrl[auq1Index] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);
                descBlock[auq1Index] = au1DescBlockPtr;
                descBlockSize[auq1Index] = au1DescBlockSize;
                numOfIter++;
            }
        }
        if (fuqRecoverySkipInit == GT_FALSE)
        {
            if(useFuQueue == GT_TRUE)
            {
                /* FUQ registers */
                addrUpdateQueueCtrl[numOfIter] =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
                addrUpdateQueueBaseAddr[numOfIter] =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;

                fuqIndex = numOfIter;
                descCtrl[fuqIndex] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
                descBlock[fuqIndex] = fuDescBlockPtr;
                descBlockSize[fuqIndex] = fuDescBlockSize;

                numOfIter++;
            }
            else
            {
                fuqIndex = QUEUE_NOT_USED_CNS;/*not relevant*/
            }
        }

        if (initAuq == GT_TRUE && auqRecoverySkipInit == GT_FALSE)
        {
            /* check if need to set the secondary AUQ for WA */
            if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                 PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E) == GT_TRUE)
            {
                descCtrl[numOfIter] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);

                secondaryAuqDescBlockSize =
                    CPSS_DXCH_PP_FDB_AUQ_DEADLOCK_EXTRA_MESSAGES_NUM_WA_CNS *
                    auMessageNumBytes;

                if(auDescBlockSize <= secondaryAuqDescBlockSize)
                {
                    /* caller not giving enough resources */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
                }

                /* we need to take the needed size from the primary AUQ */
                secondaryAuqDescBlockPtr = auDescBlockPtr +
                    (auDescBlockSize - secondaryAuqDescBlockSize);

                /* update the primary AUQ size */
                auDescBlockSize -= secondaryAuqDescBlockSize;
                descBlockSize[auqIndex] = auDescBlockSize;

                /* NOTE : the auDescBlock is already aligned so we not need to align
                   the secondaryAuqDescBlockPtr
                */

                descBlock[numOfIter] = secondaryAuqDescBlockPtr;
                descBlockSize[numOfIter] = secondaryAuqDescBlockSize;

                /* use the same registers as the primary AUQ */
                addrUpdateQueueCtrl[numOfIter] = addrUpdateQueueCtrl[auqIndex];
                addrUpdateQueueBaseAddr[numOfIter] = addrUpdateQueueBaseAddr[auqIndex];

                secondaryAuqIndex = numOfIter;

                numOfIter++;

                /* Initialize secondary AUQ1 descriptor block */
                if(au1DescBlockPtr)
                {
                    if(au1DescBlockSize <= secondaryAuqDescBlockSize)
                    {
                        /* caller not giving enough resources */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
                    }

                    descCtrl[numOfIter] =
                        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId]);

                    /* we need to take the needed size from the additional primary AUQ */
                    secondaryAuqDescBlockPtr = au1DescBlockPtr +
                        (au1DescBlockSize - secondaryAuqDescBlockSize);

                    /* update the additional primary AUQ size */
                    au1DescBlockSize -= secondaryAuqDescBlockSize;
                    descBlockSize[auq1Index] = au1DescBlockSize;

                    /* NOTE : the au1DescBlock is already aligned so we don't need to align
                       the secondaryAuqDescBlockPtr
                    */
                    descBlock[numOfIter] = secondaryAuqDescBlockPtr;
                    descBlockSize[numOfIter] = secondaryAuqDescBlockSize;

                    /* use the same registers as the primary AUQ1 */
                    addrUpdateQueueCtrl[numOfIter] = addrUpdateQueueCtrl[auq1Index];
                    addrUpdateQueueBaseAddr[numOfIter] = addrUpdateQueueBaseAddr[auq1Index];

                    secondaryAuq1Index = numOfIter;

                    numOfIter++;
                }
            }
        }
        if(numOfIter == 0)
        {
            /* nothing more to do */
            goto exit_cleanly_lbl;
        }
        /* get that state of the device , is the device after HW reset and
           not did the AU/FU queues settings */
        retVal = prvCpssPpConfigDevDbHwResetGet(devNum,&didHwReset);
        if(retVal != GT_OK)
        {
            systemRecoveryInfo = tempSystemRecoveryInfo;
            return retVal;
        }
        /* if system recovery is in progress the following block is skipped */
        if (tempSystemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E)
        {
            /* we need to get the values that were last used , when the HW didn't do reset ,
               and when the DB of descCtrl[0]->blockAddr == 0 (not initialized yet)  */
            if(descCtrl[0]->blockAddr == 0 && didHwReset == GT_FALSE)

            {
                tmpDescCtrl[0] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]);
                tmpDescCtrl[1] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
                tmpDescCtrl[2] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);
                tmpDescCtrl[3] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);
                tmpDescCtrl[4] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId]);

                /* get the info to synch with current HW state
                   NOTE: the 'save' of this info was done in
                   prvCpssPpConfigDevDbPrvInfoSet(...) */
                retVal = prvCpssPpConfigDevDbPrvInfoGet(devNum,portGroupId,
                                                        tmpDescCtrl[0],
                                                        tmpDescCtrl[1],
                                                        tmpDescCtrl[2],
                                                        tmpDescCtrl[3],
                                                        tmpDescCtrl[4]);
                if (retVal != GT_OK)
                {
                     systemRecoveryInfo = tempSystemRecoveryInfo;
                     return retVal;
                }


                if(auqIndex < MESSAGE_QUEUE_NUM_CNS)
                {
                    descCtrl[auqIndex] = tmpDescCtrl[0];
                }

                if(fuqIndex < MESSAGE_QUEUE_NUM_CNS)
                {
                    descCtrl[fuqIndex] = tmpDescCtrl[1];
                }

                if(secondaryAuqIndex < MESSAGE_QUEUE_NUM_CNS)
                {
                    descCtrl[secondaryAuqIndex] = tmpDescCtrl[2];
                }

                if(auq1Index < MESSAGE_QUEUE_NUM_CNS)
                {
                    descCtrl[auq1Index] = tmpDescCtrl[3];
                }

                if(secondaryAuq1Index < MESSAGE_QUEUE_NUM_CNS)
                {
                    descCtrl[secondaryAuq1Index] = tmpDescCtrl[4];
                }
             }
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            if(fuqRecoverySkipInit == GT_FALSE)
            {
                if (tempSystemRecoveryInfo.systemRecoveryMode.haCpuMemoryAccessBlocked == GT_FALSE)
                {
                    /* enable/disable  FU queue */
                    retVal = prvCpssHwPpPortGroupSetRegField(devNum,
                            portGroupId,
                            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl,
                            31,1,
                            (useFuQueue == GT_TRUE) ? 1 : 0);
                    if (retVal != GT_OK)
                    {
                         systemRecoveryInfo = tempSystemRecoveryInfo;
                         return retVal;
                    }
                }
            }
        }

        for(ii = 0; ii < numOfIter; ii++)
        {
            /* if device is not after HW reset (meaning we already set the device
               with AU/FU queue size and base address , and we cant set those
               values again to HW , we only need to clean the AU/FU queues from
               leftovers messages) */
            if( didHwReset == GT_FALSE)
            {
                /* the pointer to start of 'next message to handle'  */
                auMemPtr = (GT_U32 *)(descCtrl[ii]->blockAddr + (auMessageNumBytes * descCtrl[ii]->currDescIdx));

                 /* point descPtr to the current AU descriptor in the queue */
                addrUpMsgPtr = (PRV_CPSS_AU_DESC_STC*)auMemPtr;

                /* handle AUQ or FUQ (when no CNC used) */
                if((ii == auqIndex)|| (ii == auq1Index) || /*AUQ*/
                    (ii == fuqIndex /*FUQ*/&& (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum)) &&
                     0 == descCtrl[ii]->unreadCncCounters)||
                    (ii == secondaryAuqIndex) || (ii == secondaryAuq1Index)/*secondary AUQ*/)
                {
                    /* Clear the AU queue until the HW pointer */
                    while (!AU_DESC_IS_NOT_VALID(addrUpMsgPtr))
                    {
                        descCtrl[ii]->currDescIdx =
                                    ((descCtrl[ii]->currDescIdx + 1) %
                                                    descCtrl[ii]->blockSize);
                        AU_DESC_RESET_MAC(addrUpMsgPtr);

                        auMemPtr = (GT_U32 *)(descCtrl[ii]->blockAddr + (auMessageNumBytes * descCtrl[ii]->currDescIdx));

                        /* prevent infinite loop */
                        if(descCtrl[ii]->currDescIdx == 0)
                        {
                            /* HW pointer at the end of AU/FU queue =>
                              Need to rewind it */
                            initHwPointerAfterShutdown = GT_TRUE;
                            break;
                        }
                    }
                }
                else if(ii == fuqIndex)/* relevant to CNC counters only */
                {
                    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) &&
                       descCtrl[ii]->unreadCncCounters)
                    {
                        /*since we may have CNC messages on queue , clear it also */

                        if(descCtrl[ii]->unreadCncCounters & 1)
                        {
                            /* make the number even , because 2 counters in an entry */
                            descCtrl[ii]->unreadCncCounters ++;
                        }

                        while(descCtrl[ii]->unreadCncCounters)
                        {
                            descCtrl[ii]->unreadCncCounters -=2;/* 2 CNC counters in 'Single entry' */

                            descCtrl[ii]->currDescIdx =
                                        ((descCtrl[ii]->currDescIdx + 1) %
                                                        descCtrl[ii]->blockSize);
                            AU_DESC_RESET_MAC(addrUpMsgPtr);

                            auMemPtr = (GT_U32 *)(descCtrl[ii]->blockAddr + (auMessageNumBytes * descCtrl[ii]->currDescIdx));

                            /* prevent infinite loop */
                            if(descCtrl[ii]->currDescIdx == 0)
                            {
                                /* HW pointer at the end of AU/FU queue =>
                                  Need to rewind it */
                                initHwPointerAfterShutdown = GT_TRUE;
                                break;
                            }
                        }

                        descCtrl[ii]->unreadCncCounters = 0;
                    }
                }

                if (initHwPointerAfterShutdown == GT_FALSE)
                {
                    /* go to the next iteration */
                    continue;
                }
           }

            /* Set the descBlock to point to an aligned start address. */
            if((((GT_UINTPTR)descBlock[ii]) % auMessageNumBytes) != 0)
            {
                alignmentNumBytes =  (auMessageNumBytes -
                                 (((GT_UINTPTR)descBlock[ii]) % auMessageNumBytes));
                descBlockSize[ii] -= alignmentNumBytes;/* update size according to new buffer alignment*/
                descBlock[ii]     += alignmentNumBytes;/* update buffer according to alignment */
            }

            descCtrl[ii]->currDescIdx = 0;
            descCtrl[ii]->blockAddr = (GT_UINTPTR)descBlock[ii];
            descCtrl[ii]->blockSize = descBlockSize[ii] / auMessageNumBytes;/*num of entries in the block*/
            descCtrl[ii]->unreadCncCounters = 0;

            cpssOsMemSet(descBlock[ii],0,sizeof(GT_U8) * descBlockSize[ii]);

            if((ii == secondaryAuqIndex) || (ii == secondaryAuq1Index))
            {
                /* no more to do for this queue as we not do any HW writings for
                    this queue */
                continue;
            }
            retVal = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,
                                     addrUpdateQueueCtrl[ii],0,31,
                                     descCtrl[ii]->blockSize);/*num of entries in the block*/
            if (retVal != GT_OK)
            {
                systemRecoveryInfo = tempSystemRecoveryInfo;
                return retVal;
            }
            retVal = cpssOsVirt2Phy(descCtrl[ii]->blockAddr,/*OUT*/&phyAddr);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                if (0 != (phyAddr & 0xffffffff00000000L))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            #endif

            retVal = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,
                                       addrUpdateQueueBaseAddr[ii],(GT_U32)phyAddr);
            if (retVal != GT_OK)
            {
                systemRecoveryInfo = tempSystemRecoveryInfo;
                return retVal;
            }
            if ( (tempSystemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E ) ||
                 (tempSystemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_HA_E) )
            {
                 /* if HSU or HA process is in progress */

                if ((auqRecoverySkipInit == GT_FALSE) ||(fuqRecoverySkipInit == GT_FALSE))
                {
                    /* if non-continuous mode is chosen */
                    /* clear all possible FIFO messages in AUQ */
                    if(ii == auqIndex)
                    {
                        cpssOsTimerWkAfter(1);
                        /* the pointer to start of 'next message to handle'  */
                        addrUpMsgPtr = (PRV_CPSS_AU_DESC_STC *)(descCtrl[ii]->blockAddr + (auMessageNumBytes * descCtrl[ii]->currDescIdx));

                        /* Clear the AU queue until the HW pointer */
                        while (!AU_DESC_IS_NOT_VALID(addrUpMsgPtr))
                        {
                            descCtrl[ii]->currDescIdx =
                                        ((descCtrl[ii]->currDescIdx + 1) %
                                                        descCtrl[ii]->blockSize);
                            AU_DESC_RESET_MAC(addrUpMsgPtr);

                            addrUpMsgPtr = (PRV_CPSS_AU_DESC_STC*)(descCtrl[ii]->blockAddr + (auMessageNumBytes * descCtrl[ii]->currDescIdx));
                        }
                    }
                }
            }
        }
    }

    else
    {

        /* use FIFO registers not PCI.
           change auQBaseAddr register address to use FIFO.
           do not change auQControl register address. */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr =
                PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAddrUpdateMsgs.AUMsgToCPU.AUMsgToCPU;
        }
        else if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr= 0x0B000034;
        }
        else
        {
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr= 0x06000034;
        }

        if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
        {
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBGlobalConfig.FDBGlobalConfig;
            }
            else
            {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
        }
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
        }

       /* For DxCh1 and DxCh2 disable PCI interface
        in the Global Control register.
        For DxCh3 and above devices disable "AUMsgsToCPUIF"
        in FDB Global Configuration register. */
        retVal = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,
                 regAddr,
                 20,1,0);
        if (retVal != GT_OK)
        {
            systemRecoveryInfo = tempSystemRecoveryInfo;
            return retVal;
        }
    }

exit_cleanly_lbl:
    if(portGroupId == PRV_CPSS_LAST_ACTIVE_PORT_GROUP_ID_MAC(devNum))
    {
        /* for 'multi port groups' device -- state that PP ready only after all port groups finished */

        /* state to the special DB that the device finished PP logical init */
        prvCpssPpConfigDevDbHwResetSet(devNum,GT_FALSE);
    }
    systemRecoveryInfo = tempSystemRecoveryInfo;
    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChCfgDevRemove
*
* DESCRIPTION:
*
*       Remove the device from the CPSS.
*       This function we release the DB of CPSS that associated with the PP ,
*       and will "know" that the device is not longer valid.
*       This function will not do any HW actions , and therefore can be called
*       before or after the HW was actually removed.
*
*       After this function is called the devNum is no longer valid for any API
*       until this devNum is used in the initialization sequence for a "new" PP.
*
*       NOTE: 1. Application may want to disable the Traffic to the CPU , and
*             messages (AUQ) , prior to this operation (if the device still exists).
*             2. Application may want to a reset the HW PP , and there for need
*             to call the "hw reset" function (before calling this function)
*             and after calling this function to do the initialization
*             sequence all over again for the device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to remove.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgDevRemove
(
    IN GT_U8   devNum
)
{
    GT_STATUS   rc;
    CPSS_PP_FAMILY_TYPE_ENT         devFamily;
    GT_U32  ii;
    CPSS_PP_DEVICE_TYPE             devType;

    /* check that the device still exists */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /*store values needed for later use */
    devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;
    devType = PRV_CPSS_PP_MAC(devNum)->devType;

    rc = prvCpssDxChHwRegAddrDbRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* free allocations are 'DXCH' specific allocations (not generic) */
    /* need to free :
       1. pools
       2. dynamic allocations
       3. semaphores/mutexes
    */
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.enhancedInfoPtr);
    for(ii = 0;ii < PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->numOfUplinkPorts ;ii++)
    {
        FREE_PTR_MAC(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].internalConnectionsArr);
    }
    for(ii = 0;ii < PRV_CPSS_MAX_MAC_PORTS_NUM_CNS ;ii++)
    {
        FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMibShadowArr[ii]);
    }
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
            extraTrunksInfoPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E.
            portTxQueuesBmpPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.
            pcsLoopbackBmpPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.
            portEnableBmpPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_LION2_DISMATCH_PORT_LINK_WA_E.
            portForceLinkDownBmpPtr);

    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.keyModePerFidBmpPtr);

    /* next part -- release the 'generic' DB */
    rc = prvCpssPpConfigDevDbRelease(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*************************************************************/
    /* NOTE :  PRV_CPSS_PP_MAC(devNum) == NULL at this point !!! */
    /*************************************************************/

    /* check that the family of the cheetah is empty , so we can release the
       global needed DB , relate to the cheetah family */
    if(prvCpssFamilyInfoArray[devFamily] == NULL)
    {
        /* release the LPM DB if no more DXCH devices */
        rc = prvCpssDxChIpLpmDbRelease();
        if(rc != GT_OK)
        {
            return rc;
        }
        if(CPSS_PP_FAMILY_DXCH_BOBCAT2_E > devFamily)
        {
            prvCpssDxChLpmTcamRowsClear();
        }
    }

    /* reset the HWS */
    switch(devType)
    {
        case CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC:
             hwsHooperIfClose(devNum);
            break;
        case CPSS_BOBCAT2_ALL_DEVICES_CASES_MAC:
            hwsBobcat2IfClose(devNum);
            break;
        case CPSS_BOBK_ALL_DEVICES_CASES_MAC:
            mvHwsDeviceClose(devNum);/* call generic function of HWS */
            break;
#if 0 /*we wait until HWS fix CRASH inside the function of hwsBobKIfClose() */
        case CPSS_BOBCAT3_ALL_DEVICES_CASES_MAC:
            CPSS_TBD_BOOKMARK_BOBCAT3
            mvHwsDeviceClose(devNum);/* call generic function of HWS */
            break;
#endif /*0*/
        case PRV_CPSS_DXCH_XCAT3_DEVICES_CASES_MAC:
            hwsAlleycat3IfClose(devNum);
            break;
        case CPSS_LION2_PORT_GROUPS_01234567_DEVICES_CASES_MAC:
        case CPSS_LION3_PORT_GROUPS_01234567_DEVICES_CASES_MAC:
            hwsLion2IfClose(devNum);
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgDevRemove
*
* DESCRIPTION:
*
*       Remove the device from the CPSS.
*       This function we release the DB of CPSS that associated with the PP ,
*       and will "know" that the device is not longer valid.
*       This function will not do any HW actions , and therefore can be called
*       before or after the HW was actually removed.
*
*       After this function is called the devNum is no longer valid for any API
*       until this devNum is used in the initialization sequence for a "new" PP.
*
*       NOTE: 1. Application may want to disable the Traffic to the CPU , and
*             messages (AUQ) , prior to this operation (if the device still exists).
*             2. Application may want to a reset the HW PP , and there for need
*             to call the "hw reset" function (before calling this function)
*             and after calling this function to do the initialization
*             sequence all over again for the device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to remove.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevRemove
(
    IN GT_U8   devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgDevRemove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChCfgDevRemove(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgDevEnable
*
* DESCRIPTION:
*       This routine sets the device state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to set.
*       enable - GT_TRUE device enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgDevEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;       /* return code */
    GT_U32  bit;        /* The bit to be written to the register    */
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    bit = BOOL2BIT_MAC(enable);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E ||
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
        {
            CPSS_TBD_BOOKMARK_EARCH/* current GM doesn't support DFX unit so far */
            return GT_OK;
        }

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                   dfxUnits.server.resetControl;
        }
        else /* CPSS_PP_FAMILY_DXCH_XCAT3_E || CPSS_PP_FAMILY_DXCH_BOBCAT2_E */
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                                  DFXServerUnitsBC2SpecificRegs.deviceResetCtrl;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        rc = prvCpssDrvHwPpResetAndInitControllerSetRegField(
                                                    devNum, regAddr, 0, 1, bit);
    }
    else
    {
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, bit);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChCfgDevEnable
*
* DESCRIPTION:
*       This routine sets the device state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to set.
*       enable - GT_TRUE device enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgDevEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChCfgDevEnable(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgDevEnableGet
*
* DESCRIPTION:
*       This routine gets the device state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum - device number to set.
*
* OUTPUTS:
*       enablePtr - GT_TRUE device enable, GT_FALSE disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgDevEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    bit;
    GT_U32    regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E ||
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
        {
            CPSS_TBD_BOOKMARK_EARCH/* current GM doesn't support DFX unit so far */
            *enablePtr = GT_TRUE;
            return GT_OK;
        }

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                   dfxUnits.server.resetControl;
        }
        else /* CPSS_PP_FAMILY_DXCH_XCAT3_E || CPSS_PP_FAMILY_DXCH_BOBCAT2_E */
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                                  DFXServerUnitsBC2SpecificRegs.deviceResetCtrl;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        rc = prvCpssDrvHwPpResetAndInitControllerGetRegField(
                                                   devNum, regAddr, 0, 1, &bit);
    }
    else
    {
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &bit);
    }

    if(GT_OK != rc)
    {
        return rc;
    }

    *enablePtr =  BIT2BOOL_MAC(bit);

    return rc;
}

/*******************************************************************************
* cpssDxChCfgDevEnableGet
*
* DESCRIPTION:
*       This routine gets the device state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum - device number to set.
*
* OUTPUTS:
*       enablePtr - GT_TRUE device enable, GT_FALSE disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgDevEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChCfgDevEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgNextDevGet
*
* DESCRIPTION:
*
*       Return the number of the next existing device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to start from. For the first one devNum should be 0xFF.
*
* OUTPUTS:
*       nextDevNumPtr - number of next device after devNum.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_MORE               - devNum is the last device. nextDevNumPtr will be set to 0xFF.
*       GT_BAD_PARAM             - devNum > max device number
*       GT_BAD_PTR               - nextDevNumPtr pointer is NULL.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgNextDevGet
(
    IN  GT_U8 devNum,
    OUT GT_U8 *nextDevNumPtr
)
{
    GT_U8 num;

    /* device should be in the allowed range but not necessary exist */
    if ((devNum > PRV_CPSS_MAX_PP_DEVICES_CNS) && (devNum != DEVICE_NOT_EXISTS_CNS))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (nextDevNumPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (devNum == 0xFF)
        num = 0;
    else
        num = (GT_U8)(devNum+1);

    for (; num<PRV_CPSS_MAX_PP_DEVICES_CNS; num++)
    {
        if (prvCpssPpConfig[num] != NULL)
        {
            *nextDevNumPtr = num;
            return GT_OK;
        }
    }

    /* no more devices found */
    *nextDevNumPtr = DEVICE_NOT_EXISTS_CNS;
    return /* it's not error for log */ GT_NO_MORE;
}

/*******************************************************************************
* cpssDxChCfgNextDevGet
*
* DESCRIPTION:
*
*       Return the number of the next existing device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to start from. For the first one devNum should be 0xFF.
*
* OUTPUTS:
*       nextDevNumPtr - number of next device after devNum.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_MORE               - devNum is the last device. nextDevNumPtr will be set to 0xFF.
*       GT_BAD_PARAM             - devNum > max device number
*       GT_BAD_PTR               - nextDevNumPtr pointer is NULL.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgNextDevGet
(
    IN  GT_U8 devNum,
    OUT GT_U8 *nextDevNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgNextDevGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, nextDevNumPtr));

    rc = internal_cpssDxChCfgNextDevGet(devNum, nextDevNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, nextDevNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChCfgAddrUpInit
*
* DESCRIPTION:
*       This function initializes the Core Address Update mechanism, by
*       allocating the AU descs. block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to init the Au unit for.
*       auDescBlockPtr  - (pointer to)A block to allocate the Au descriptors from.
*       auDescBlockSize - Size in bytes of Au descBlock.
*       fuDescBlockPtr  - (pointer to)A block to allocate the Fu descriptors from.
*                         Valid if useFuQueue is GT_TRUE.
*       fuDescBlockSize - Size in bytes of Fu descBlock.
*                         Valid if useFuQueue is GT_TRUE.
*       useFuQueue      - GT_TRUE  - Fu queue is used - for DxCh2 devices only.
*                       - GT_FALSE - Fu queue is unused.
*       useDoubleAuq    - Support configuration of two AUQ memory regions.
*                         GT_TRUE - CPSS manages two AU Queues with the same size:
*                         auqCfg->auDescBlockSize / 2.
*                         GT_FALSE - CPSS manages single AU Queue with size:
*                         auqCfg->auDescBlockSize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChCfgAddrUpInit
(
    IN GT_U8    devNum,
    IN GT_U8    *auDescBlockPtr,
    IN GT_U32   auDescBlockSize,
    IN GT_U8    *fuDescBlockPtr,
    IN GT_U32   fuDescBlockSize,
    IN GT_BOOL  useFuQueue,
    IN GT_BOOL  useDoubleAuq
)
{
    GT_STATUS   rc;                             /* return code */
    GT_U32      portGroupId;                    /* port group ID */
    GT_U32      numActivePortGroups = 0;        /* number of active port groups */
    GT_U32      perPortGroupAuSize;             /* AUQ size per port group */
    GT_U32      perPortGroupFuSize;             /* FUQ size per port group */
    GT_U32      index=0;
    GT_U8       *perPortGroupAuDescBlockPtr;    /* pointer to AUQ descriptors block per port group */
    GT_U8       *perPortGroupFuDescBlockPtr;    /* pointer to FUQ descriptors block per port group */
    GT_U8       *perPortGroupAu1DescBlockPtr;   /* pointer to AUQ1 descriptors block per port group */
    GT_U8       *au1DescBlockPtr = 0;           /* pointer to AUQ1 descriptors first block */
    GT_U32      auqSize = auDescBlockSize;      /* Temporary AUQ size */
    CPSS_SYSTEM_RECOVERY_INFO_STC  tempSystemRecoveryInfo;

    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                   info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                       enabled == GT_FALSE))
    {
        if(auDescBlockPtr == NULL)
        {
            /* no Hw changes needed */
            /* nothing more to do */
            return GT_OK;
        }
    }

    tempSystemRecoveryInfo = systemRecoveryInfo; /* save recovery state*/
    if ( (tempSystemRecoveryInfo.systemRecoveryMode.haCpuMemoryAccessBlocked == GT_TRUE)&&
         (tempSystemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_HA_E) )
    {
        systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
        if ( (useFuQueue == GT_TRUE) && (tempSystemRecoveryInfo.systemRecoveryMode.continuousFuMessages == GT_FALSE) )
        {
            /* This init is under system recovery process after HA event. PP starts to send  */
            /* messages to newly reprogramed FUQ only if it has indication that "old" one is full. */
            /* After HA event there is no knowledge about FUQ status. So WA on previously defined queue is performed*/
            rc = prvCpssDxChHaFuqNonContinuousMsgModeHandle(devNum);
            if (rc != GT_OK)
            {
                systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                return rc;
            }
        }

        if(tempSystemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)
        {
            /* This init is under system recovery process after HA event. PP starts to send  */
            /* messages to newly reprogramed AUQ only if it has indication that "old" one is full. */
            /* After HA event there is no knowledge about AUQ status. So WA on previously defined queue is performed*/
            rc = prvCpssDxChHaAuqNonContinuousMsgModeHandle(devNum);
            if (rc != GT_OK)
            {
                systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                return rc;
            }
        }

        systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
    }
    else
    {
        if ( (tempSystemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E)||
             (tempSystemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_HA_E) )
        {
            if (tempSystemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)
            {
                systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
                rc = prvCpssDxChDisableAuqAndSaveAuqCurrentStatus(devNum);
                if (rc != GT_OK)
                {
                    systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                    return rc;
                }
                systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
            }
        }
    }

    /* Memory allocated for two AUQ */
    if(useDoubleAuq == GT_TRUE)
    {
        auqSize /= 2;
        au1DescBlockPtr = &auDescBlockPtr[auqSize];
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        numActivePortGroups++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(GT_TRUE == PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
        {
            if(portGroupId == PRV_CPSS_FIRST_ACTIVE_PORT_GROUP_ID_MAC(devNum))
            {
                /* single instance AUQ */
                perPortGroupAuSize = auqSize; /*relevant only to the first port group */
            }
            else
            {
                /* single instance AUQ */
                perPortGroupAuSize = 0; /*relevant only to the first port group */
            }
            /* all port groups get the same FUQ size */
            perPortGroupFuSize = fuDescBlockSize / numActivePortGroups;
        }
        else
        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            /* all port groups get the same AUQ size */
            perPortGroupAuSize = auqSize / numActivePortGroups;
            /* all port groups get the same FUQ size */
            perPortGroupFuSize = fuDescBlockSize / numActivePortGroups;
        }
        else
        {
            perPortGroupAuSize = auqSize;
            perPortGroupFuSize = fuDescBlockSize;
        }

        perPortGroupAuDescBlockPtr = auDescBlockPtr ?
            &auDescBlockPtr[perPortGroupAuSize * index] : NULL;

        perPortGroupFuDescBlockPtr = useFuQueue ?
            &fuDescBlockPtr[perPortGroupFuSize * index] : NULL;

        perPortGroupAu1DescBlockPtr = au1DescBlockPtr ?
            &au1DescBlockPtr[perPortGroupAuSize * index] : NULL;

        rc = internalCfgAddrUpInit(devNum,
                    portGroupId,
                    perPortGroupAuDescBlockPtr,
                    perPortGroupAuSize,
                    perPortGroupFuDescBlockPtr,
                    perPortGroupFuSize,
                    useFuQueue,
                    perPortGroupAu1DescBlockPtr,
                    perPortGroupAuSize);
        index++;

        /* Set primary AUQ index */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.activeAuqIndex[portGroupId] = 0;
        /* Set primary AUQ init state - 'FULL';
        When WA triggered for the first time - all primary AUQs are full */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState =
            PRV_CPSS_AUQ_STATE_ALL_FULL_E;
        /* Set secondary AUQ index */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activeSecondaryAuqIndex = 0;
        /* Set secondary AUQ state - 'EMPTY' */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState =
            PRV_CPSS_AUQ_STATE_ALL_EMPTY_E;

        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if ( (tempSystemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E)||
         ( (tempSystemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_HA_E) &&
           tempSystemRecoveryInfo.systemRecoveryMode.haCpuMemoryAccessBlocked == GT_FALSE) )
    {
        if (tempSystemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)
        {
            systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
            rc = prvCpssDxChRestoreAuqCurrentStatus(devNum);
            if (rc != GT_OK)
            {
                systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                return rc;
            }
            systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChCfgPpLogicalInit
*
* DESCRIPTION:
*       This function Performs PP RAMs divisions to memory pools, according to
*       the supported modules in system.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       ppConfigPtr - (pointer to)includes PP specific logical initialization
*                     params.
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_PP_MEM -  If the given configuration can't fit into the given
*                           PP's memory capabilities, or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  This function should perform all PP initializations, which
*           includes:
*               -   Init the Driver's part (by calling driverPpInit()).
*               -   Ram structs init, and Ram size check.
*               -   Initialization of the interrupt mechanism data structure.
*       2.  When done, the function should leave the PP in disable state, and
*           forbid any packets forwarding, and any interrupts generation.
*       3.  The execution flow for preparing for the initialization of core
*           modules is described below:
*
*
*             +-----------------+
*             | Init the 3 RAMs |   This initialization includes all structs
*             | conf. structs   |   fields but the base address field.
*             |                 |
*             +-----------------+
*                     |
*   (Ram is big       |                 +-------------------------+
*    enough to hold   +--(No)-----------| return GT_OUT_OF_PP_MEM |
*    all tables?)     |                 +-------------------------+
*                     |
*                   (Yes)
*                     |
*                     V
*             +-------------------+
*             | Set the Rams base |
*             | addr. according   |
*             | to the location   |
*             | fields.           |
*             +-------------------+
*                     |
*                     |
*                     V
*             +----------------------------------+
*             | Init the                         |
*             | PRV_CPSS_DXCH_MODULE_CONFIG_STC  |
*             | struct, according                |
*             | to the info in RAM               |
*             | conf. struct.                    |
*             +----------------------------------+
*                     |
*                     |
*                     |
*                     V
*             +---------------------------------+
*             | set the prepared                |
*             | module Cfg struct               |
*             | in the appropriate              |
*             | PRV_CPSS_DXCH_PP_CONFIG_STC     |
*             | struct.                         |
*             +---------------------------------+
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgPpLogicalInit
(
    IN      GT_U8                           devNum,
    IN      CPSS_DXCH_PP_CONFIG_INIT_STC    *ppConfigPtr
)
{
    GT_STATUS         rc = GT_OK;
    GT_U32            value;
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;/* pointer to the module
                                                configure of the PP's database*/
    GT_U32              indexOfLastMemory;/* index of start of last memory in the LPM */
    GT_U32              indexForPbr;/* index of start of PBR in the LPM */
    GT_U32              numOfPbrBlocks;/* number of blocks PBR needs */
    GT_U32              blockIndexForPbr;/* block index of start of PBR in the LPM */
    GT_U32              numOfIndexesInFirstBlockNotUsedForPbr;/* amount need to be skipped from first PBR block */
    GT_U32              lpmRamNumOfLines;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ppConfigPtr);

    /* Configure the module configruation struct.   */
    moduleCfgPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg);

    /* Init the AU and FU (if fuqUseSeparate is GT_TRUE) mechanism according
     to the data. Set in the module configuration structure. */
    /* Init the AU mechanism according to the data  */
    /* set in the module configuration structure.   */
    rc = prvCpssDxChCfgAddrUpInit(devNum,moduleCfgPtr->auCfg.auDescBlock,
                                  moduleCfgPtr->auCfg.auDescBlockSize,
                                  moduleCfgPtr->fuCfg.fuDescBlock,
                                  moduleCfgPtr->fuCfg.fuDescBlockSize,
                                  moduleCfgPtr->fuqUseSeparate,
                                  moduleCfgPtr->useDoubleAuq);
    if (rc != GT_OK)
        return rc;

    if( (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) )
    {
        switch(ppConfigPtr->routingMode)
        {
            case CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E:
                value = 0;
                break;
            case CPSS_DXCH_TCAM_ROUTER_BASED_E:
                value = 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssHwPpSetRegField(devNum,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerAdditionalCtrlReg,
                                       0,1,value);
        if (rc != GT_OK)
            return rc;
    }

    moduleCfgPtr->ip.routingMode = ppConfigPtr->routingMode;
    moduleCfgPtr->ip.maxNumOfPbrEntries = ppConfigPtr->maxNumOfPbrEntries;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        lpmRamNumOfLines = PRV_CPSS_DXCH_LPM_RAM_GET_NUM_OF_LINES_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.lpmRam);
        if (moduleCfgPtr->ip.maxNumOfPbrEntries >= lpmRamNumOfLines)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfLinesInBlock !=
            PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.totalNumOfLinesInBlockIncludingGap)
        {
            /* this is indication that memory is not consecutive */

            if(moduleCfgPtr->ip.maxNumOfPbrEntries >
                PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfLinesInBlock)
            {
                /* need to calculate how many blocks the pbr needs */
                numOfPbrBlocks = (moduleCfgPtr->ip.maxNumOfPbrEntries +
                                  (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfLinesInBlock-1))/
                                  (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfLinesInBlock);

                /* find the index of the block were the pbr entries will start */
                blockIndexForPbr = (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfBlocks - numOfPbrBlocks)*
                                    (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.totalNumOfLinesInBlockIncludingGap);

                /* upadte the index according to the amount of bpr residing in the first block */
                numOfIndexesInFirstBlockNotUsedForPbr = (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfLinesInBlock*numOfPbrBlocks)-
                                                        moduleCfgPtr->ip.maxNumOfPbrEntries;
                indexForPbr = blockIndexForPbr + numOfIndexesInFirstBlockNotUsedForPbr;

            }
            else
            {
                indexOfLastMemory =
                    PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.totalNumOfLinesInBlockIncludingGap *
                    (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfBlocks - 1);

                indexForPbr = indexOfLastMemory +
                    (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfLinesInBlock -
                        moduleCfgPtr->ip.maxNumOfPbrEntries);
            }
        }
        else
        {
            indexForPbr = lpmRamNumOfLines - moduleCfgPtr->ip.maxNumOfPbrEntries;
        }

        PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.indexForPbr = indexForPbr;


        rc = prvCpssHwPpSetRegField(devNum,
                                    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->LPM.directAccessMode,
                                    0, 19,
                                    indexForPbr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgPpLogicalInit
*
* DESCRIPTION:
*       This function Performs PP RAMs divisions to memory pools, according to
*       the supported modules in system.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       ppConfigPtr - (pointer to)includes PP specific logical initialization
*                     params.
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_PP_MEM -  If the given configuration can't fit into the given
*                           PP's memory capabilities, or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  This function should perform all PP initializations, which
*           includes:
*               -   Init the Driver's part (by calling driverPpInit()).
*               -   Ram structs init, and Ram size check.
*               -   Initialization of the interrupt mechanism data structure.
*       2.  When done, the function should leave the PP in disable state, and
*           forbid any packets forwarding, and any interrupts generation.
*       3.  The execution flow for preparing for the initialization of core
*           modules is described below:
*
*
*             +-----------------+
*             | Init the 3 RAMs |   This initialization includes all structs
*             | conf. structs   |   fields but the base address field.
*             |                 |
*             +-----------------+
*                     |
*   (Ram is big       |                 +-------------------------+
*    enough to hold   +--(No)-----------| return GT_OUT_OF_PP_MEM |
*    all tables?)     |                 +-------------------------+
*                     |
*                   (Yes)
*                     |
*                     V
*             +-------------------+
*             | Set the Rams base |
*             | addr. according   |
*             | to the location   |
*             | fields.           |
*             +-------------------+
*                     |
*                     |
*                     V
*             +----------------------------------+
*             | Init the                         |
*             | PRV_CPSS_DXCH_MODULE_CONFIG_STC  |
*             | struct, according                |
*             | to the info in RAM               |
*             | conf. struct.                    |
*             +----------------------------------+
*                     |
*                     |
*                     |
*                     V
*             +---------------------------------+
*             | set the prepared                |
*             | module Cfg struct               |
*             | in the appropriate              |
*             | PRV_CPSS_DXCH_PP_CONFIG_STC     |
*             | struct.                         |
*             +---------------------------------+
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgPpLogicalInit

(
    IN      GT_U8                           devNum,
    IN      CPSS_DXCH_PP_CONFIG_INIT_STC    *ppConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgPpLogicalInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ppConfigPtr));

    rc = internal_cpssDxChCfgPpLogicalInit(devNum, ppConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ppConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
*
* DESCRIPTION:
*       Sets device ID modification for Routed packets.
*       Enables/Disables FORWARD DSA tag modification of the <source device>
*       and <source port> fields of packets routed by the local device.
*       The <source device> is set to the local device ID and the <source port>
*       is set to 61 (the virtual router port).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - Device number.
*       portNum         - Port number (or CPU port)
*       modifyEnable    - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 modifyEnable
)
{
    GT_U32  regAddr;     /* register address */
    GT_U32  regValue;    /* register value */
    GT_U32  fieldOffset; /* The start bit number in the register */
    GT_U32  portGroupId; /*the port group Id - support multi port groups device */
    GT_U32   localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    regValue = (modifyEnable  == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
                                               portNum,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                               LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_ROUTED_SRC_DEVICE_ID_PORT_MODE_ENABLE_E, /* field name */
                                               PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                               regValue);
    }
    else
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        fieldOffset = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                     PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                     localPort;

        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                routerDevIdModifyEnReg[OFFSET_TO_WORD_MAC(fieldOffset)];

        return prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,
                                         regAddr, OFFSET_TO_BIT_MAC(fieldOffset), 1, regValue);
    }
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
*
* DESCRIPTION:
*       Sets device ID modification for Routed packets.
*       Enables/Disables FORWARD DSA tag modification of the <source device>
*       and <source port> fields of packets routed by the local device.
*       The <source device> is set to the local device ID and the <source port>
*       is set to 61 (the virtual router port).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - Device number.
*       portNum         - Port number (or CPU port)
*       modifyEnable    - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 modifyEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modifyEnable));

    rc = internal_cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(devNum, portNum, modifyEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modifyEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet
*
* DESCRIPTION:
*       Gets device ID modification for Routed packets.
*       See Set Api comments.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - Device number.
*       portNum          - Port number (or CPU port)
*
* OUTPUTS:
*       modifyEnablePtr  - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *modifyEnablePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;     /* register address */
    GT_U32    regValue;    /* register value */
    GT_U32    fieldOffset; /* The start bit number in the register */
    GT_U32    portGroupId; /*the port group Id - support multi port groups device */
    GT_U32     localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(modifyEnablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_ROUTED_SRC_DEVICE_ID_PORT_MODE_ENABLE_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &regValue);
        if(GT_OK != rc)
        {
            return rc;
        }
    }
    else
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        fieldOffset = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                     PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                     localPort;

        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                routerDevIdModifyEnReg[OFFSET_TO_WORD_MAC(fieldOffset)];

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,
                                         regAddr, OFFSET_TO_BIT_MAC(fieldOffset), 1, &regValue);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    *modifyEnablePtr = (regValue == 0) ? GT_FALSE : GT_TRUE;

    return rc;
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet
*
* DESCRIPTION:
*       Gets device ID modification for Routed packets.
*       See Set Api comments.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - Device number.
*       portNum          - Port number (or CPU port)
*
* OUTPUTS:
*       modifyEnablePtr  - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *modifyEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modifyEnablePtr));

    rc = internal_cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet(devNum, portNum, modifyEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modifyEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgDsaTagSrcDevPortModifySet
*
* DESCRIPTION:
*       Enables/Disables Device ID Modification upon packet sending to another
*       stack unit.
*
*       When Connecting DxCh Devices to SOHO in a Value Blade model to enable
*       DxCh grade Capabilities for FE Ports, in a staking system, we must be
*       able to overcome the 32 devices limitation.
*       To do that, SOHO Device Numbers are not unique and packets received
*       from the SOHO by the DxCh and are relayed to other stack units
*       have their Device ID changed to the DxCh Device ID.
*       On Upstream (packet from SOHO to DxCh):
*           The SOHO Sends a packet to the DxCh and the packet is sent back
*           to the SOHO. In this case we don't want to change the Device ID in
*           the DSA Tag so that the SOHO is able to filter the Src Port and is
*           able to send back the packet to its source when doing cross chip
*           flow control.
*       On Downsteam (packet from SOHO to SOHO):
*           The SOHO receives a packet from another SOHO in this case we
*           want to change the Device ID so that the packet is not filtered.
*
*       On top of the above, for forwarding packets between the DxCh devices
*       and for Auto Learning, the Port Number must also be changed.
*       In addition Changing the Device ID is needed only for FORWARD DSA Tag.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - Device number.
*       modifedDsaSrcDev    - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgDsaTagSrcDevPortModifySet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     modifedDsaSrcDev
)
{
    GT_U32  regAddr;     /* register address */
    GT_U32  regValue;    /* register value */
    GT_U32  startBit;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    regValue = (modifedDsaSrcDev == GT_TRUE) ? 1 : 0;

    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    eggrDSATagTypeConf[0];
        startBit = 28;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    hdrAltGlobalConfig;
        startBit = 16;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, startBit, 1, regValue);
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortModifySet
*
* DESCRIPTION:
*       Enables/Disables Device ID Modification upon packet sending to another
*       stack unit.
*
*       When Connecting DxCh Devices to SOHO in a Value Blade model to enable
*       DxCh grade Capabilities for FE Ports, in a staking system, we must be
*       able to overcome the 32 devices limitation.
*       To do that, SOHO Device Numbers are not unique and packets received
*       from the SOHO by the DxCh and are relayed to other stack units
*       have their Device ID changed to the DxCh Device ID.
*       On Upstream (packet from SOHO to DxCh):
*           The SOHO Sends a packet to the DxCh and the packet is sent back
*           to the SOHO. In this case we don't want to change the Device ID in
*           the DSA Tag so that the SOHO is able to filter the Src Port and is
*           able to send back the packet to its source when doing cross chip
*           flow control.
*       On Downsteam (packet from SOHO to SOHO):
*           The SOHO receives a packet from another SOHO in this case we
*           want to change the Device ID so that the packet is not filtered.
*
*       On top of the above, for forwarding packets between the DxCh devices
*       and for Auto Learning, the Port Number must also be changed.
*       In addition Changing the Device ID is needed only for FORWARD DSA Tag.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - Device number.
*       modifedDsaSrcDev    - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortModifySet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     modifedDsaSrcDev
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgDsaTagSrcDevPortModifySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modifedDsaSrcDev));

    rc = internal_cpssDxChCfgDsaTagSrcDevPortModifySet(devNum, modifedDsaSrcDev);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modifedDsaSrcDev));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgDsaTagSrcDevPortModifyGet
*
* DESCRIPTION:
*       Get status of Device ID Modification upon packet sending to another
*       stack unit. See Set Api comments.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum               - Device number.
*
* OUTPUTS:
*       modifedDsaSrcDevPtr  - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgDsaTagSrcDevPortModifyGet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     *modifedDsaSrcDevPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;     /* register address */
    GT_U32    regValue;    /* register value */
    GT_U32    startBit;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(modifedDsaSrcDevPtr);

    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    eggrDSATagTypeConf[0];
        startBit = 28;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    hdrAltGlobalConfig;
        startBit = 16;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, startBit, 1, &regValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    *modifedDsaSrcDevPtr = (regValue == 0) ? GT_FALSE : GT_TRUE;

    return rc;
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortModifyGet
*
* DESCRIPTION:
*       Get status of Device ID Modification upon packet sending to another
*       stack unit. See Set Api comments.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum               - Device number.
*
* OUTPUTS:
*       modifedDsaSrcDevPtr  - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortModifyGet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     *modifedDsaSrcDevPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgDsaTagSrcDevPortModifyGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modifedDsaSrcDevPtr));

    rc = internal_cpssDxChCfgDsaTagSrcDevPortModifyGet(devNum, modifedDsaSrcDevPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modifedDsaSrcDevPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgTableNumEntriesGet
*
*
* DESCRIPTION:
*       the function return the number of entries of each individual table in
*       the HW
*
*       when several type of entries like ARP and tunnelStart resize in the same
*       table (memory) , the function returns the number of entries for the least
*       denominator type of entry --> in this case number of ARP entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number
*       table        - type of the specific table
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgTableNumEntriesGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_CFG_TABLES_ENT    table,
    OUT GT_U32                      *numEntriesPtr
)
{
    GT_U32  numEntries;/* tmp num entries value */
    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* validate the pointer */
    CPSS_NULL_PTR_CHECK_MAC(numEntriesPtr);

    switch(table)
    {
        case CPSS_DXCH_CFG_TABLE_VLAN_E:
            numEntries =
                1 + PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_VLAN_MAC(devNum);
            break;
        case CPSS_DXCH_CFG_TABLE_FDB_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb;
            break;
        case CPSS_DXCH_CFG_TABLE_PCL_ACTION_E:
        case CPSS_DXCH_CFG_TABLE_PCL_TCAM_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                /* num of 10B rules */
                numEntries =  PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws;
            }
            else {
                if (PRV_CPSS_PP_MAC(devNum)->devFamily  <= CPSS_PP_FAMILY_CHEETAH2_E)
                {
                    numEntries =
                        PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws
                        * 2;
                }
                else /* ch3 and above */
                {
                    numEntries =
                        PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws
                        * 4;
                }
            }

            /* the number of 'standard' rules (same number of actions) */

            break;
        case CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop;
            break;
        case CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E:
        case CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.router;
            break;
        case CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_QOS_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.ecmpQos;
            break;
        case CPSS_DXCH_CFG_TABLE_TTI_TCAM_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelTerm;
            break;
        case CPSS_DXCH_CFG_TABLE_MLL_PAIR_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.mllPairs;
            break;
        case CPSS_DXCH_CFG_TABLE_POLICER_METERS_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum;
            break;
        case CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily  <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                numEntries = TABLE_NOT_VALID_CNS;/* no such table*/
            }
            else /* ch3 and above */
            {
                numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum;
            }
            break;
        case CPSS_DXCH_CFG_TABLE_VIDX_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.vidxNum;
            break;
        case CPSS_DXCH_CFG_TABLE_ARP_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerArp;
            break;
        case CPSS_DXCH_CFG_TABLE_TUNNEL_START_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart;
            break;
        case CPSS_DXCH_CFG_TABLE_STG_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.stgNum;
            break;
        case CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E:
            numEntries = (GT_U32)(PRV_CPSS_DXCH_QOS_PROFILE_MAX_MAC(devNum));
            break;
        case CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily  <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                numEntries = TABLE_NOT_VALID_CNS;/* no such table*/
            }
            else if (PRV_CPSS_PP_MAC(devNum)->devFamily  >= CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
            {
                numEntries = PRV_CPSS_LION3_MAC2ME_TABLE_MAX_ENTRIES_CNS;
            }
            else /* ch3 and above */
            {
                numEntries = PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS;
            }
            break;

        case CPSS_DXCH_CFG_TABLE_CNC_E:
            /* the number of CNC counters in the device */
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlocks *
                         PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlockNumEntries;
            break;

        case CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E:
            /* the number of counters in CNC block */
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlockNumEntries;
            break;

        case CPSS_DXCH_CFG_TABLE_TRUNK_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.trunksNum;
            break;

        case CPSS_DXCH_CFG_TABLE_LPM_RAM_E:
            numEntries = PRV_CPSS_DXCH_LPM_RAM_GET_NUM_OF_LINES_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.lpmRam);
            break;
        case CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_E:
            numEntries = TABLE_NUM_ENTRIES_GET_MAC(devNum,PRV_CPSS_DXCH_LION3_TABLE_ECMP_E);
            break;

        case CPSS_DXCH_CFG_TABLE_L2_MLL_LTT_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            numEntries = 1 + PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_L2_LTT_MAC(devNum);
            break;

        case CPSS_DXCH_CFG_TABLE_EPORT_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            numEntries = PRV_CPSS_DXCH_MAX_PORT_NUMBER_MAC(devNum);
            break;

        case CPSS_DXCH_CFG_TABLE_DEFAULT_EPORT_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            numEntries = PRV_CPSS_DXCH_MAX_DEFAULT_EPORT_NUMBER_MAC(devNum);
            break;

        case CPSS_DXCH_CFG_TABLE_PHYSICAL_PORT_E:
            numEntries = PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* return the caller with needed info */
    *numEntriesPtr = numEntries;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgTableNumEntriesGet
*
*
* DESCRIPTION:
*       the function return the number of entries of each individual table in
*       the HW
*
*       when several type of entries like ARP and tunnelStart resize in the same
*       table (memory) , the function returns the number of entries for the least
*       denominator type of entry --> in this case number of ARP entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number
*       table        - type of the specific table
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgTableNumEntriesGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_CFG_TABLES_ENT    table,
    OUT GT_U32                      *numEntriesPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgTableNumEntriesGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, table, numEntriesPtr));

    rc = internal_cpssDxChCfgTableNumEntriesGet(devNum, table, numEntriesPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, table, numEntriesPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgReNumberDevNum
*
* DESCRIPTION:
*       function allow the caller to modify the DB of the cpss ,
*       so all info that was 'attached' to 'oldDevNum' will be moved to 'newDevNum'.
*
*       NOTE:
*       1. it is the responsibility of application to update the 'devNum' of
*       HW entries from 'oldDevNum' to 'newDevNum' using appropriate API's
*       such as 'Trunk members','Fdb entries','NH entries','PCE entries'...
*       2. it's application responsibility to update the HW device number !
*          see API cpssDxChCfgHwDevNumSet
*       3. no HW operations involved in this API
*
*       NOTE:
*       this function MUST be called under 'Interrupts are locked' and under
*       'Tasks lock'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       oldDevNum  - old device number
*       newDevNum  - new device number (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - the device oldDevNum not exist
*       GT_OUT_OF_RANGE - the device newDevNum value > 0x1f (0..31)
*       GT_ALREADY_EXIST - the new device number is already used
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgReNumberDevNum
(
    IN GT_U8   oldDevNum,
    IN GT_U8   newDevNum
)
{
    GT_STATUS   rc;
    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(oldDevNum);

    /*validate the new device number*/
    if(newDevNum >= BIT_5)
    {
        /*device number limited to 5 bits */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(oldDevNum == newDevNum)
    {
        /* nothing more to do */
        return GT_OK;
    }

    /* swap the special DB - for re-init support */
    rc = prvCpssPpConfigDevDbRenumber(oldDevNum,newDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* swap the cpss DB */
    if(prvCpssPpConfig[newDevNum] != NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    prvCpssPpConfig[newDevNum] = prvCpssPpConfig[oldDevNum];
    prvCpssPpConfig[oldDevNum] = NULL;

    /* let the cpssDriver also 'SWAP' pointers */
    rc = prvCpssDrvHwPpRenumber(oldDevNum,newDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(newDevNum))
    {
        /* UN-bind the old device from the DSMA Tx completed callback to the driver */
        rc = prvCpssDrvInterruptPpTxEndedCbBind(oldDevNum,NULL);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* bind the DSMA Tx completed callback to the driver - to the new device */
        rc = prvCpssDrvInterruptPpTxEndedCbBind(newDevNum,
                                    prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* update network interface DB*/
    sysGenGlobalInfo.prvMiiDevNum = newDevNum;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgReNumberDevNum
*
* DESCRIPTION:
*       function allow the caller to modify the DB of the cpss ,
*       so all info that was 'attached' to 'oldDevNum' will be moved to 'newDevNum'.
*
*       NOTE:
*       1. it is the responsibility of application to update the 'devNum' of
*       HW entries from 'oldDevNum' to 'newDevNum' using appropriate API's
*       such as 'Trunk members','Fdb entries','NH entries','PCE entries'...
*       2. it's application responsibility to update the HW device number !
*          see API cpssDxChCfgHwDevNumSet
*       3. no HW operations involved in this API
*
*       NOTE:
*       this function MUST be called under 'Interrupts are locked' and under
*       'Tasks lock'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       oldDevNum  - old device number
*       newDevNum  - new device number (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - the device oldDevNum not exist
*       GT_OUT_OF_RANGE - the device newDevNum value > 0x1f (0..31)
*       GT_ALREADY_EXIST - the new device number is already used
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgReNumberDevNum
(
    IN GT_U8   oldDevNum,
    IN GT_U8   newDevNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgReNumberDevNum);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, oldDevNum, newDevNum));

    rc = internal_cpssDxChCfgReNumberDevNum(oldDevNum, newDevNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, oldDevNum, newDevNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgDevInfoGet
*
* DESCRIPTION:
*       the function returns device static information
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number
*
* OUTPUTS:
*       devInfoPtr   - (pointer to) device information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgDevInfoGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_CFG_DEV_INFO_STC   *devInfoPtr
)
{
    GT_STATUS   rc;

    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* validate the pointer */
    CPSS_NULL_PTR_CHECK_MAC(devInfoPtr);

    /* fill in generic part of device info */
    rc = prvCpssPpConfigDevInfoGet(devNum,&(devInfoPtr->genDevInfo));
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        devInfoPtr->genDevInfo.hwDevNumMode = CPSS_GEN_CFG_HW_DEV_NUM_MODE_DUAL_E;
    }

    devInfoPtr->genDevInfo.numOfLedInfPerPortGroup =
        prvCpssLedStreamNumOfInterfacesInPortGroupGet(devNum);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgDevInfoGet
*
* DESCRIPTION:
*       the function returns device static information
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number
*
* OUTPUTS:
*       devInfoPtr   - (pointer to) device information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevInfoGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_CFG_DEV_INFO_STC   *devInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgDevInfoGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, devInfoPtr));

    rc = internal_cpssDxChCfgDevInfoGet(devNum, devInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, devInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChCfgBindPortPhymacObject
*
* DESCRIPTION:
*         The function binds port phymac pointer
*         to 1540M PHYMAC object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    portNum        - port number
*    macDrvObjPtr   - port phymac object pointer
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgBindPortPhymacObject
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_MACDRV_OBJ_STC * const macDrvObjPtr
)
{
    GT_STATUS   rc;
    GT_U32 portMacNum; /* MAC number */

    /* validate devNum and portNum */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    /* validate the MACDRV object pointer */
    CPSS_NULL_PTR_CHECK_MAC(macDrvObjPtr);

    /*NOTE: generic function will work with MAC port number */
    rc = prvCpssPpConfigPortToPhymacObjectBind(devNum,portMacNum,macDrvObjPtr);
    if (rc != GT_OK) {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgBindPortPhymacObject
*
* DESCRIPTION:
*         The function binds port phymac pointer
*         to 1540M PHYMAC object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    portNum        - port number
*    macDrvObjPtr   - port phymac object pointer
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgBindPortPhymacObject(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_MACDRV_OBJ_STC * const macDrvObjPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgBindPortPhymacObject);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, macDrvObjPtr));

    rc = internal_cpssDxChCfgBindPortPhymacObject(devNum, portNum, macDrvObjPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, macDrvObjPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgPortDefaultSourceEportNumberSet
*
* DESCRIPTION:
*       Set Port default Source ePort number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       ePort         - ePort number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Default value is physical port number. In Multi-core
*          architecture it is the global physical port number (i.e. Local Core
*          Port number bitwise ORed with LocalCoreID<<4)
*       2. Can be overriden by TTI Action/RBridge
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgPortDefaultSourceEportNumberSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_PORT_NUM                         ePort
)
{
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,ePort);

    /* write to TTI-Physical-Port-Attribute table */
    return prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E,
                                             portNum,/*global port*/
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                             LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_DEFAULT_SOURCE_EPORT_NUMBER_E, /* field name */
                                             PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                             ePort);
}

/*******************************************************************************
* cpssDxChCfgPortDefaultSourceEportNumberSet
*
* DESCRIPTION:
*       Set Port default Source ePort number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       ePort         - ePort number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Default value is physical port number. In Multi-core
*          architecture it is the global physical port number (i.e. Local Core
*          Port number bitwise ORed with LocalCoreID<<4)
*       2. Can be overriden by TTI Action/RBridge
*
*******************************************************************************/
GT_STATUS cpssDxChCfgPortDefaultSourceEportNumberSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_PORT_NUM                         ePort
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgPortDefaultSourceEportNumberSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ePort));

    rc = internal_cpssDxChCfgPortDefaultSourceEportNumberSet(devNum, portNum, ePort);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ePort));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgPortDefaultSourceEportNumberGet
*
* DESCRIPTION:
*       Get Port default Source ePort number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       ePortPtr      - pointer to ePort number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Default value is physical port number. In Multi-core
*          architecture it is the global physical port number (i.e. Local Core
*          Port number bitwise ORed with LocalCoreID<<4)
*       2. Can be overriden by TTI Action/RBridge
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgPortDefaultSourceEportNumberGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT GT_PORT_NUM                         *ePortPtr
)
{
    GT_STATUS   rc;     /* function return code */
    GT_U32  value;      /* value to write */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(ePortPtr);

    /* read from TTI-Physical-Port-Attribute table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                     PRV_CPSS_DXCH_LION3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E,
                                     portNum,
                                     PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                     LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_DEFAULT_SOURCE_EPORT_NUMBER_E, /* field name */
                                     PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                     &value);

    *ePortPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChCfgPortDefaultSourceEportNumberGet
*
* DESCRIPTION:
*       Get Port default Source ePort number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       ePortPtr      - pointer to ePort number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Default value is physical port number. In Multi-core
*          architecture it is the global physical port number (i.e. Local Core
*          Port number bitwise ORed with LocalCoreID<<4)
*       2. Can be overriden by TTI Action/RBridge
*
*******************************************************************************/
GT_STATUS cpssDxChCfgPortDefaultSourceEportNumberGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT GT_PORT_NUM                         *ePortPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgPortDefaultSourceEportNumberGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ePortPtr));

    rc = internal_cpssDxChCfgPortDefaultSourceEportNumberGet(devNum, portNum, ePortPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ePortPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgEgressHashSelectionModeSet
*
* DESCRIPTION:
*       Set how egress pipe get 6 bits hash from the ingress 12 bits hash. (EQ-->TXQ)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       selectionMode       - hash selection mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgEgressHashSelectionModeSet
(
    IN GT_U8                                                        devNum,
    IN CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT    selectionMode
)
{
    GT_U32    regAddr;      /* value to write */
    GT_U32    value;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(selectionMode)
    {
        case CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_LSB_E:
            value = (5  << 5) | /* last bit 5 */
                    (0 << 0);   /* start bit 0 */
            break;
        case CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_MSB_E:
            value = (11 << 5) | /* last bit 11 */
                    (6 << 0);   /* start bit 6 */
            break;
        case CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_12_BITS_E:
            value = (11 << 5) | /* last bit 11 */
                    (0 << 0);   /* start bit 0 */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).preEgrEngineGlobalConfig.cscdTrunkHashBitSelectionConfig;

    return prvCpssHwPpSetRegField(devNum,regAddr ,0 ,10 ,value);
}

/*******************************************************************************
* cpssDxChCfgEgressHashSelectionModeSet
*
* DESCRIPTION:
*       Set how egress pipe get 6 bits hash from the ingress 12 bits hash. (EQ-->TXQ)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       selectionMode       - hash selection mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCfgEgressHashSelectionModeSet
(
    IN GT_U8                                                        devNum,
    IN CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT    selectionMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgEgressHashSelectionModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, selectionMode));

    rc = internal_cpssDxChCfgEgressHashSelectionModeSet(devNum, selectionMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, selectionMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgEgressHashSelectionModeGet
*
* DESCRIPTION:
*       Return how egress pipe get 6 bits hash from the ingress 12 bits hash. (EQ-->TXQ)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       selectionModePtr        - (pointer to)hash selection mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgEgressHashSelectionModeGet
(
    IN  GT_U8                                                       devNum,
    OUT CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT   *selectionModePtr
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      hwData;
    GT_U32      regAddr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(selectionModePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).preEgrEngineGlobalConfig.cscdTrunkHashBitSelectionConfig;

    rc = prvCpssHwPpGetRegField(devNum,regAddr ,0 ,10 ,&hwData);

    if(rc != GT_OK)
        return rc;

    switch(hwData)
    {
        case (5 << 5) | /* last bit 5 */
             (0 << 0):
            *selectionModePtr = CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_LSB_E;
            break;
        case (11 << 5) | /* last bit 11 */
              (6 << 0):   /* start bit 6 */
            *selectionModePtr = CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_MSB_E;
            break;
        case (11 << 5) | /* last bit 11 */
             ( 0 << 0):   /* start bit 0 */
            *selectionModePtr = CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_12_BITS_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChCfgEgressHashSelectionModeGet
*
* DESCRIPTION:
*       Return how egress pipe get 6 bits hash from the ingress 12 bits hash. (EQ-->TXQ)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       selectionModePtr        - (pointer to)hash selection mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgEgressHashSelectionModeGet
(
    IN  GT_U8                                                       devNum,
    OUT CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT   *selectionModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgEgressHashSelectionModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, selectionModePtr));

    rc = internal_cpssDxChCfgEgressHashSelectionModeGet(devNum, selectionModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, selectionModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChCfgIngressDropCntrModeSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device number.
*       mode      - Ingress Drop Counter Mode.
*       portNum   - port number monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
*                   counter mode.
*       vlan      - VLAN ID monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E
*                   counter mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or mode.
*       GT_OUT_OF_RANGE          - on wrong portNum or vlan.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgIngressDropCntrModeSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT mode,
    IN  GT_PORT_NUM                                 portNum,
    IN  GT_U16                                      vlan
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    value;       /* register field value */
    GT_U32    fieldLength; /* the number of bits to be written to register */
    GT_U32    portGroupId,portPortGroupId;/*the port group Id - support multi-port-groups device */
    GT_PORT_NUM     localPort;   /* local port - support multi-port-groups device */
    GT_STATUS rc;          /* return code */

    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            ingrDropCntr.ingrDropCntrConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.ingressDropCntrConfReg;
    }

    switch(mode)
    {
        case CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ALL_E:
            fieldLength = 2;
            value = 0;
            break;

        case     CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E:
            if (vlan > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                fieldLength = 15;
            }
            else
            {
                fieldLength = 14;
            }
            value = 1 | (GT_U32)vlan << 2;
            break;

        case     CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E:
            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                PRV_CPSS_HW_DEV_NUM_MAC(devNum),portNum);

            localPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                PRV_CPSS_HW_DEV_NUM_MAC(devNum),
                portNum);

            if(localPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
                {
                    fieldLength = 17;
                }
                else
                {
                    fieldLength = 15;
                }
            }
            else
            {
                fieldLength = 14;
            }

            value = 2 | localPort << 2;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    if((mode == CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E) &&
       (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE))
    {
        /* hold 'port filter' for not eArch devices.
           The eArch devices uses ePort for filter.
           Therefore same ePort value is in all port groups */

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        /* loop on all port groups :
            on the port group that 'own' the port , set the needed configuration
            on other port groups put 'NULL port'
        */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

        {
            if((CPSS_PORT_GROUP_UNAWARE_MODE_CNS == portPortGroupId) || (portPortGroupId == portGroupId))
            {
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
            }
            else
            {
                localPort = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
            }

            value = 2 | localPort << 2;

            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr ,0 ,fieldLength ,value);

            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

        rc = GT_OK;
    }
    else
    {
        portPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        rc = prvCpssHwPpSetRegField(devNum, regAddr ,0 ,fieldLength ,value);
    }

    if(rc == GT_OK)
    {
        /* save the info for 'get' configuration and 'read' counters */
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                cfgIngressDropCntrMode.portGroupId = portPortGroupId;
    }

    return rc;

}

/*******************************************************************************
* cpssDxChCfgIngressDropCntrModeSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device number.
*       mode      - Ingress Drop Counter Mode.
*       portNum   - port number monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
*                   counter mode.
*       vlan      - VLAN ID monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E
*                   counter mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or mode.
*       GT_OUT_OF_RANGE          - on wrong portNum or vlan.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrModeSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT mode,
    IN  GT_PORT_NUM                                 portNum,
    IN  GT_U16                                      vlan
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgIngressDropCntrModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode, portNum, vlan));

    rc = internal_cpssDxChCfgIngressDropCntrModeSet(devNum, mode, portNum, vlan);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode, portNum, vlan));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgIngressDropCntrModeGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device number.
*
* OUTPUTS:
*       modePtr      - pointer to Ingress Drop Counter Mode.
*       portNumPtr   - pointer to port number monitored by Ingress Drop Counter.
*                      This parameter is applied upon
*                      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
*                      counter mode.
*       vlanPtr      - pointer to VLAN ID monitored by Ingress Drop Counter.
*                      This parameter is applied upon
*                      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E
*                      counter mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - illegal state of configuration
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgIngressDropCntrModeGet
(
    IN   GT_U8                                       devNum,
    OUT  CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT *modePtr,
    OUT  GT_PORT_NUM                                 *portNumPtr,
    OUT  GT_U16                                      *vlanPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      value;       /* register field value */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS   rc;          /* return code */
    GT_U32      fieldOffset; /* The number of bits to be written to register */


    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            ingrDropCntr.ingrDropCntrConfig;
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            fieldOffset = 17;
        }
        else
        {
            fieldOffset = 15;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.ingressDropCntrConfReg;
        fieldOffset = 14;
    }

    portGroupId = PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
            cfgIngressDropCntrMode.portGroupId;

    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr ,0 ,fieldOffset ,&value);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(value & 0x3)
    {
        case 0:
            *modePtr = CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ALL_E;
            break;
        case 1:
            *modePtr = CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E;
            if (vlanPtr != NULL)
            {
                if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
                {
                    *vlanPtr = (GT_U16)((value >> 2) & 0x1FFF);
                }
                else
                {
                    *vlanPtr = (GT_U16)((value >> 2) & 0xFFF);
                }
            }

            break;
        case 2:
            *modePtr = CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E;
            if (portNumPtr != NULL)
            {
                if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
                {
                    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
                    {
                        *portNumPtr = (value >> 2) & 0x7FFF;
                    }
                    else
                    {
                        *portNumPtr = (value >> 2) & 0x1FFF;
                    }
                }
                else
                {
                    *portNumPtr = (value >> 2) & 0xFFF;

                    if(portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
                    {
                        /* support multi-port-groups device , convert local port to global port */
                        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,(*portNumPtr));
                    }
                }
            }

            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    return GT_OK;

}

/*******************************************************************************
* cpssDxChCfgIngressDropCntrModeGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device number.
*
* OUTPUTS:
*       modePtr      - pointer to Ingress Drop Counter Mode.
*       portNumPtr   - pointer to port number monitored by Ingress Drop Counter.
*                      This parameter is applied upon
*                      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
*                      counter mode.
*       vlanPtr      - pointer to VLAN ID monitored by Ingress Drop Counter.
*                      This parameter is applied upon
*                      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E
*                      counter mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - illegal state of configuration
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrModeGet
(
    IN   GT_U8                                       devNum,
    OUT  CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT *modePtr,
    OUT  GT_PORT_NUM                                 *portNumPtr,
    OUT  GT_U16                                      *vlanPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgIngressDropCntrModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr, portNumPtr, vlanPtr));

    rc = internal_cpssDxChCfgIngressDropCntrModeGet(devNum, modePtr, portNumPtr, vlanPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr, portNumPtr, vlanPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgIngressDropCntrSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - Device number.
*       counter     - Ingress Drop Counter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgIngressDropCntrSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      counter
)
{
    GT_U32      regAddr;     /* register address */

    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* next devices hold the counter as 'read only,clear' so can't 'set' to it*/
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
        eqBlkCfgRegs.ingressDropCntrReg;

    return prvCpssPortGroupsBmpCounterSet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              regAddr, 0, 32, counter);
}

/*******************************************************************************
* cpssDxChCfgIngressDropCntrSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - Device number.
*       counter     - Ingress Drop Counter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      counter
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgIngressDropCntrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counter));

    rc = internal_cpssDxChCfgIngressDropCntrSet(devNum, counter);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counter));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChCfgIngressDropCntrGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       counterPtr  - (pointer to) Ingress Drop Counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgIngressDropCntrGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *counterPtr
)
{
    GT_U32      regAddr;     /* register address */

    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* validate the pointer */
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            ingrDropCntr.ingrDropCntr;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.ingressDropCntrReg;
    }

    return prvCpssPortGroupsBmpCounterSummary(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                       regAddr, 0, 32,
                                                       counterPtr, NULL);
}

/*******************************************************************************
* cpssDxChCfgIngressDropCntrGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       counterPtr  - (pointer to) Ingress Drop Counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgIngressDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counterPtr));

    rc = internal_cpssDxChCfgIngressDropCntrGet(devNum, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChCfgGlobalEportSet
*
* DESCRIPTION:
*       Set Global ePorts ranges configuration.
*       Global ePorts are ePorts that are global in the entire system, and are
*       not unique per device as regular ePorts are.
*
*       Global ePorts are used for Representing:
*       1. a Trunk.(map target ePort to trunkId by the E2PHY)
*       2. a ePort ECMP Group.(map 'primary' target ePort to 'secondary' target ePort)
*       3. an ePort Replication Group (eVIDX).(map target ePort to eVIDX group)
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
*       globalPtr   - (pointer to) Global EPorts for the Bridge,L2Mll to use.
*                           In the Bridge Used for:
*                           a) Whether the bridge engine looks at the device number
*                              when performing MAC SA lookup or local switching check.
*                           b) That the local device ID (own device) is learned in the
*                              FDB entry with the global ePort
*
*                           In the L2MLL Used for:
*                           Enabling the L2MLL replication engine to ignore the
*                           device ID when comparing source and target interfaces
*                           for source filtering.
*                           Typically configured to include global ePorts
*                           representing a trunk or an ePort ECMP group
*
*                       NOTE: For defining the EPorts that used for 'Multi-Target ePort to eVIDX Mapping'
*                           use API cpssDxChL2MllMultiTargetPortSet(...)
*                           (see cpssDxChL2Mll.h file for more related functions)
*
*       l2EcmpPtr   - (pointer to) L2 ECMP Primary ePorts.
*                     Used for: The 'Primary ePorts' that need to be converted to
*                      one of their 'Secondary ePorts'.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgGlobalEportSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *globalPtr,
    IN  CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *l2EcmpPtr
)
{
    GT_STATUS rc;       /* return code */
    GT_U32    regAddr;  /* register address */
    GT_U32    ecmpPattern, ecmpMask, globalPattern, globalMask;/* registers values */
    GT_U32    ePortMask;/* mask of ePort */
    GT_U32    noMatchPattern,noMatchMask;/*these values guaranties NO Match.*/
    GT_U32    fieldSize;    /* register field size */
    /*these values guaranties NO Match.
    because in formula ((ePort & Mask) == Pattern) ? global : not_global
    then if bit set in pattern and not set in mask cause that no ePort can be
    treated as 'global' */
    noMatchPattern = 1;
    noMatchMask = 0;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(l2EcmpPtr);
    CPSS_NULL_PTR_CHECK_MAC(globalPtr);

    ePortMask = PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum);

    /* check ECMP pattern & mask values */
    if( GT_TRUE == l2EcmpPtr->enable )
    {
        if( (l2EcmpPtr->mask > ePortMask) ||
            ((l2EcmpPtr->pattern & l2EcmpPtr->mask) != l2EcmpPtr->pattern) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        ecmpPattern = l2EcmpPtr->pattern;
        ecmpMask    = l2EcmpPtr->mask;
    }
    else
    {
        ecmpPattern = noMatchPattern;
        ecmpMask    = noMatchMask;
    }

    /* check global pattern & mask values */
    if( GT_TRUE == globalPtr->enable )
    {
        if( (globalPtr->mask > ePortMask) ||
            ((globalPtr->pattern & globalPtr->mask) != globalPtr->pattern) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        globalPattern = globalPtr->pattern;
        globalMask    = globalPtr->mask;
    }
    else
    {
        globalPattern = noMatchPattern;
        globalMask    = noMatchMask;
    }
    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        fieldSize = 15;
    }
    else
    {
        fieldSize = 13;
    }

    /* write ECMP global ePort range value */
    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).L2ECMP.ePortECMPEPortValue;
    rc = prvCpssHwPpSetRegField(devNum, regAddr ,0 ,fieldSize ,ecmpPattern);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* write ECMP global ePort range mask */
    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).L2ECMP.ePortECMPEPortMask;
    rc = prvCpssHwPpSetRegField(devNum, regAddr ,0 ,fieldSize ,ecmpMask);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* write bridge global ePort range value */
    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
        globalEportConifguration.globalEPortValue;
    rc = prvCpssHwPpSetRegField(devNum, regAddr ,0 ,fieldSize ,globalPattern);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* write bridge global ePort range mask */
    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                globalEportConifguration.globalEPortMask;
    rc = prvCpssHwPpSetRegField(devNum, regAddr ,0 ,fieldSize ,globalMask);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* write L2 MLL global ePort range value */
    regAddr = PRV_DXCH_REG1_UNIT_MLL_MAC(devNum).MLLGlobalCtrl.globalEPortRangeConfig;
    rc = prvCpssHwPpSetRegField(devNum, regAddr ,0 ,20 ,globalPattern);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* write L2 MLL global ePort range mask */
    regAddr = PRV_DXCH_REG1_UNIT_MLL_MAC(devNum).MLLGlobalCtrl.globalEPortRangeMask;
    rc = prvCpssHwPpSetRegField(devNum, regAddr ,0 ,20 ,globalMask);
    if( GT_OK != rc )
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgGlobalEportSet
*
* DESCRIPTION:
*       Set Global ePorts ranges configuration.
*       Global ePorts are ePorts that are global in the entire system, and are
*       not unique per device as regular ePorts are.
*
*       Global ePorts are used for Representing:
*       1. a Trunk.(map target ePort to trunkId by the E2PHY)
*       2. a ePort ECMP Group.(map 'primary' target ePort to 'secondary' target ePort)
*       3. an ePort Replication Group (eVIDX).(map target ePort to eVIDX group)
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
*       globalPtr   - (pointer to) Global EPorts for the Bridge,L2Mll to use.
*                           In the Bridge Used for:
*                           a) Whether the bridge engine looks at the device number
*                              when performing MAC SA lookup or local switching check.
*                           b) That the local device ID (own device) is learned in the
*                              FDB entry with the global ePort
*
*                           In the L2MLL Used for:
*                           Enabling the L2MLL replication engine to ignore the
*                           device ID when comparing source and target interfaces
*                           for source filtering.
*                           Typically configured to include global ePorts
*                           representing a trunk or an ePort ECMP group
*
*                       NOTE: For defining the EPorts that used for 'Multi-Target ePort to eVIDX Mapping'
*                           use API cpssDxChL2MllMultiTargetPortSet(...)
*                           (see cpssDxChL2Mll.h file for more related functions)
*
*       l2EcmpPtr   - (pointer to) L2 ECMP Primary ePorts.
*                     Used for: The 'Primary ePorts' that need to be converted to
*                      one of their 'Secondary ePorts'.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgGlobalEportSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *globalPtr,
    IN  CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *l2EcmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgGlobalEportSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, globalPtr, l2EcmpPtr));

    rc = internal_cpssDxChCfgGlobalEportSet(devNum, globalPtr, l2EcmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, globalPtr, l2EcmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChCfgGlobalEportGet
*
* DESCRIPTION:
*       Get Global ePorts ranges configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       globalPtr   - (pointer to) Global EPorts for the Bridge,L2MLL to use.
*       l2EcmpPtr   - (pointer to) L2 ECMP Primary ePorts.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgGlobalEportGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *globalPtr,
    OUT CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *l2EcmpPtr
)
{
    GT_STATUS rc;       /* return code */
    GT_U32    regAddr;  /* register address */
    GT_U32    pattern, mask; /* Global ePort registers values */
    GT_U32    l2MllPattern, l2MllMask; /* Global ePort registers values */
    GT_U32    fieldSize;    /* register field size */
    GT_BOOL   l2MllEnable;  /* L2MLL logic for value & mask */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(l2EcmpPtr);
    CPSS_NULL_PTR_CHECK_MAC(globalPtr);

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        fieldSize = 15;
    }
    else
    {
        fieldSize = 13;
    }

    /* read ECMP global ePort range value */
    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).L2ECMP.ePortECMPEPortValue;
    rc = prvCpssHwPpGetRegField(devNum, regAddr ,0 ,fieldSize ,&pattern);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* read ECMP global ePort range mask */
    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).L2ECMP.ePortECMPEPortMask;
    rc = prvCpssHwPpGetRegField(devNum, regAddr ,0 ,fieldSize ,&mask);
    if( GT_OK != rc )
    {
        return rc;
    }

    if ((pattern & mask) != pattern)
    {
        l2EcmpPtr->enable = GT_FALSE;
        l2EcmpPtr->pattern = 0x0;
        l2EcmpPtr->mask = 0x0;
    }
    else
    {
        l2EcmpPtr->enable = GT_TRUE;
        l2EcmpPtr->pattern = pattern;
        l2EcmpPtr->mask = mask;
    }

    /* read bridge global ePort range value */
    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                globalEportConifguration.globalEPortValue;
    rc = prvCpssHwPpGetRegField(devNum, regAddr ,0 ,fieldSize ,&pattern);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* read bridge global ePort range mask */
    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                globalEportConifguration.globalEPortMask;
    rc = prvCpssHwPpGetRegField(devNum, regAddr ,0 ,fieldSize ,&mask);
    if( GT_OK != rc )
    {
        return rc;
    }

    if ((pattern & mask) != pattern)
    {
        globalPtr->enable = GT_FALSE;
        globalPtr->pattern = 0x0;
        globalPtr->mask = 0x0;
    }
    else
    {
        globalPtr->enable = GT_TRUE;
        globalPtr->pattern = pattern;
        globalPtr->mask = mask;
    }

    /* read L2 MLL global ePort range value */
    regAddr = PRV_DXCH_REG1_UNIT_MLL_MAC(devNum).MLLGlobalCtrl.globalEPortRangeConfig;
    rc = prvCpssHwPpGetRegField(devNum, regAddr ,0 ,20 ,&l2MllPattern);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* read L2 MLL global ePort range mask */
    regAddr = PRV_DXCH_REG1_UNIT_MLL_MAC(devNum).MLLGlobalCtrl.globalEPortRangeMask;
    rc = prvCpssHwPpGetRegField(devNum, regAddr ,0 ,20 ,&l2MllMask);
    if( GT_OK != rc )
    {
        return rc;
    }

    if ((l2MllPattern & l2MllMask) != l2MllPattern)
    {
        l2MllEnable = GT_FALSE;
    }
    else
    {
        l2MllEnable = GT_TRUE;
    }


    /* compare register values from bridge and from L2Mll */
    if( (l2MllPattern != pattern) ||
        (l2MllMask != mask) )
    {
        /* compare if both MLL and L2I are disabled - good result */
        if ( (globalPtr->enable == GT_FALSE) && (l2MllEnable == GT_FALSE) )
        {
            /* return the L2 global pattern */
            return GT_OK;
        }
        else
        {
            /* bridge and L2Mll are not synchronized */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgGlobalEportGet
*
* DESCRIPTION:
*       Get Global ePorts ranges configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       globalPtr   - (pointer to) Global EPorts for the Bridge,L2MLL to use.
*       l2EcmpPtr   - (pointer to) L2 ECMP Primary ePorts.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgGlobalEportGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *globalPtr,
    OUT CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *l2EcmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgGlobalEportGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, globalPtr, l2EcmpPtr));

    rc = internal_cpssDxChCfgGlobalEportGet(devNum, globalPtr, l2EcmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, globalPtr, l2EcmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChCfgHwAccessObjectBind
*
* DESCRIPTION:
*         The function binds/unbinds a callback routines for HW access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    cfgAccessObjPtr - HW access object pointer.
*    bind            -  GT_TRUE - bind callback routines.
*                      GT_FALSE - unbind callback routines.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgHwAccessObjectBind
(
    IN CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC *cfgAccessObjPtr,
    IN GT_BOOL                     bind
)
{
    if(bind)
        cpssOsMemCpy( &prvDxChTableCpssHwAccessObj, cfgAccessObjPtr, sizeof(CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC));
    else
        cpssOsMemSet( &prvDxChTableCpssHwAccessObj, 0, sizeof(CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgHwAccessObjectBind
*
* DESCRIPTION:
*         The function binds/unbinds a callback routines for HW access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    cfgAccessObjPtr - HW access object pointer.
*    bind            -  GT_TRUE - bind callback routines.
*                      GT_FALSE - unbind callback routines.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgHwAccessObjectBind
(
    IN CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC *cfgAccessObjPtr,
    IN GT_BOOL                     bind
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgHwAccessObjectBind);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, cfgAccessObjPtr, bind));

    rc = internal_cpssDxChCfgHwAccessObjectBind(cfgAccessObjPtr, bind);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, cfgAccessObjPtr, bind));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCfgHitlessWriteMethodEnableSet
*
* DESCRIPTION:
*       Enable/disable hitless write methood.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  hitless write methood is enable. In this case write operation
*                                 would be performed only if writing data is not equal with
*                                 affected memory contents.
*                       GT_FALSE: hitless write methood is disable. In this case write operation
*                                 is done at any rate.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCfgHitlessWriteMethodEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_BOOL                             enable
)
{
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.hitlessWriteMethodEnable = enable;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgHitlessWriteMethodEnableSet
*
* DESCRIPTION:
*       Enable/disable hitless write methood.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  hitless write methood is enable. In this case write operation
*                                 would be performed only if writing data is not equal with
*                                 affected memory contents.
*                       GT_FALSE: hitless write methood is disable. In this case write operation
*                                 is done at any rate.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgHitlessWriteMethodEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCfgHitlessWriteMethodEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChCfgHitlessWriteMethodEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
