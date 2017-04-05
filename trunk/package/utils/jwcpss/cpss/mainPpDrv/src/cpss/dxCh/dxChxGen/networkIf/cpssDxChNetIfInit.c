/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIfInit.c
*
* DESCRIPTION:
*       Include CPSS DXCH SDMA/"Ethernet port" network interface initialization
*       functions.
*
* FILE REVISION NUMBER:
*       $Revision: 62 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>


#define ALIGN_4B_MASK_CNS   0x3
#define ALIGN_4B_CNS        4

/*******************************************************************************
* sdmaTxRegConfig
*
* DESCRIPTION:
*       Set the needed values for SDMA registers to enable Tx activity.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The Pp device number.
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
static GT_STATUS sdmaTxRegConfig
(
    IN GT_U8 devNum
)
{
    GT_U8       i;  /* SDMA queue iterator */
    GT_STATUS   rc; /* return code */
    GT_U32      portGroupId;/* port group Id for multi-port-groups device support */

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    /* Since working in SP the configure transmit queue WRR value to 0 */
    for(i = 0; i < NUM_OF_TX_QUEUES; i++)
    {
       rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId, PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQWrrPrioConfig[i], 0);
       if(rc != GT_OK)
           return rc;

        /********* Tx SDMA Token-Bucket Queue<n> Counter ************/
        rc = prvCpssHwPpWriteRegister(devNum, PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txSdmaTokenBucketQueueCnt[i], 0);
        if(rc != GT_OK)
            return rc;

        /********** Tx SDMA Token Bucket Queue<n> Configuration ***********/
        rc = prvCpssHwPpWriteRegister(devNum, PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txSdmaTokenBucketQueueConfig[i] , 0xfffffcff);
        if(rc != GT_OK)
            return rc;
    }

    /*********************/
    rc = prvCpssHwPpWriteRegister(devNum, PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txSdmaWrrTokenParameters, 0xffffffc1);
    if(rc != GT_OK)
        return rc;

    /*********** Set all queues to Fix priority **********/
    rc = prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQFixedPrioConfig, 0xFF);

    return rc;
}

/*******************************************************************************
* sdmaTxGeneratorInit
*
* DESCRIPTION:
*       This function initializes the Tx SDMA queue that are configured to act
*       as Packet Generator.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - The device number to init the Tx unit for.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS sdmaTxGeneratorInit
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddr;
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;   /* Points to the relevant Tx desc. list */

    PRV_CPSS_TX_DESC_STC *firstTxDesc;/* Points to the first Tx desc in list. */
    GT_U8 txQueue;              /* Index of the Tx Queue.               */
    GT_U32 sizeOfDescAndBuff;   /* The amount of memory (in bytes) that a     */
                                /* single descriptor and adjacent buffer will */
                                /* occupy, including the alignment.           */

    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;

    CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC *multiNetIfTxSdmaQueuePtr;
                            /* per queue descriptors & buffers configurations*/
    GT_U8 *memBlockPtr;     /* pointer to descriptors & buffers memory block */
    GT_U32 memBlockSize;    /* descriptors & buffers memory block size */
    CPSS_TX_BUFF_MALLOC_FUNC mallocFunc = NULL;/* Function for allocating the */
                                             /* descriptors & buffers memory. */
    GT_UINTPTR  physicalMemoryPointer;   /* pointer to phyisical memory. */
    GT_U32      generatorsTotalDesc; /* total number of descriptors\buffers */
                                     /* in all Packet Generators. */
    GT_U32 ii; /* loop iterator */

    moduleCfgPtr = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum);

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    txDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList;
    regsAddr    = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    generatorsTotalDesc = 0;

    /* Tx SDMA queues configuration for Packet Generators */
    for(txQueue = 0; txQueue < NUM_OF_TX_QUEUES; txQueue++)
    {
        multiNetIfTxSdmaQueuePtr = &moduleCfgPtr->
                        multiNetIfCfg.txSdmaQueuesConfig[portGroupId][txQueue];

        /* Verify queue is assigned for Packet Generator */
        if(CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E !=
                                            multiNetIfTxSdmaQueuePtr->queueMode)
        {
            continue;
        }

        /* Packet Generation Enable - Tx SDMA will not edit the ownership bit */
        /* in the descriptor. */
        rc = prvCpssHwPpPortGroupSetRegField(
            devNum,
            portGroupId,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    sdmaRegs.txSdmaPacketGeneratorConfigQueue[txQueue],
            0, 1, 1);

        if( GT_OK != rc )
        {
           return rc;
        }

        /* Since the configuration is in consecutive <descriptor,buffer>      */
        /* pairs descriptor alignment limitations are imposed on the combined */
        /* descriptor & buffer length, thus actual buffer size might be       */
        /* increased. */

        sizeOfDescAndBuff = sizeof(PRV_CPSS_TX_DESC_STC) +
                                            multiNetIfTxSdmaQueuePtr->buffSize;

        if((sizeOfDescAndBuff % TX_DESC_ALIGN) != 0)
        {
            sizeOfDescAndBuff += (TX_DESC_ALIGN -
                                   (sizeOfDescAndBuff % TX_DESC_ALIGN));
        }

        txDescList[txQueue].actualBuffSize = sizeOfDescAndBuff
                                                - sizeof(PRV_CPSS_TX_DESC_STC);

        if( CPSS_TX_BUFF_STATIC_ALLOC_E ==
                            multiNetIfTxSdmaQueuePtr->buffAndDescAllocMethod )
        {
            memBlockPtr = (GT_U8*)multiNetIfTxSdmaQueuePtr->
                                        memData.staticAlloc.buffAndDescMemPtr;
            memBlockSize = multiNetIfTxSdmaQueuePtr->
                                        memData.staticAlloc.buffAndDescMemSize;

            /* Set memory block to descriptor alignment */
            if(((GT_UINTPTR)memBlockPtr % TX_DESC_ALIGN) != 0)
            {
                memBlockPtr  += (TX_DESC_ALIGN -
                                     ((GT_UINTPTR)memBlockPtr % TX_DESC_ALIGN));
                memBlockSize -= (TX_DESC_ALIGN -
                                     ((GT_UINTPTR)memBlockPtr % TX_DESC_ALIGN));
            }

            /* Check if static memory supplied is enough for the requested */
            /* amount of descriptors and buffers.                          */
            /* Additional descriptor is required for the scratch pad. */
            if( (memBlockSize/sizeOfDescAndBuff) <
                                  (multiNetIfTxSdmaQueuePtr->numOfTxDesc + 1) )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
        else /* CPSS_TX_BUFF_DYNAMIC_ALLOC_E */
        {
            mallocFunc = multiNetIfTxSdmaQueuePtr->
                                    memData.dynamicAlloc.buffAndDescMallocFunc;

            /* Additional descriptor is required for the scratch pad. */
            memBlockSize = sizeOfDescAndBuff *
                                    (multiNetIfTxSdmaQueuePtr->numOfTxDesc + 1);
            memBlockPtr = mallocFunc(memBlockSize, TX_DESC_ALIGN);
            if( NULL == memBlockPtr )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }
        }

        txDescList[txQueue].freeDescNum = multiNetIfTxSdmaQueuePtr->numOfTxDesc;
        txDescList[txQueue].maxDescNum  = multiNetIfTxSdmaQueuePtr->numOfTxDesc;

        firstTxDesc = (PRV_CPSS_TX_DESC_STC *)memBlockPtr;
        txDescList[txQueue].firstDescPtr = firstTxDesc;
        txDescList[txQueue].freeDescPtr = firstTxDesc;

        /* Set Tx SDMA Current Descriptor Pointer to first descriptor in chain */
        rc = cpssOsVirt2Phy((GT_UINTPTR)firstTxDesc,/*OUT*/&physicalMemoryPointer);
        if (rc != GT_OK)
        {
            return rc;
        }

        #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
            if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        #endif

        rc = prvCpssHwPpPortGroupWriteRegister(
            devNum,
            portGroupId,
            regsAddr->sdmaRegs.txDmaCdp[txQueue],
            (GT_U32)physicalMemoryPointer);

        if(rc != GT_OK)
            return rc;

        /* Fill descriptors with buffer pointers*/
        for( ii = 0 ; ii <= txDescList[txQueue].maxDescNum ; ii++ )
        {
            firstTxDesc = (PRV_CPSS_TX_DESC_STC *)memBlockPtr;

            rc = cpssOsVirt2Phy(
                        (GT_UINTPTR)(memBlockPtr+sizeof(PRV_CPSS_TX_DESC_STC)),
                        /*OUT*/&physicalMemoryPointer);
            if (rc != GT_OK)
            {
                return rc;
            }

            #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                if (0 != (physicalMemoryPointer & 0xffffffff00000000L))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            #endif

            /* descriptor pointing to the adjacent buffer */
            firstTxDesc->buffPointer = CPSS_32BIT_LE((GT_U32)physicalMemoryPointer);

            /* next descriptor place in the chain */
            memBlockPtr += sizeOfDescAndBuff;
        }

        txDescList[txQueue].scratchPadPtr = firstTxDesc;

        PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.rateMode[portGroupId][txQueue] =
                                  CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E;

        txDescList[txQueue].revPacketIdDb =
           (GT_U32*)cpssOsMalloc(sizeof(GT_U32)*txDescList[txQueue].maxDescNum);
        if( NULL == txDescList[txQueue].revPacketIdDb)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        /* reset memory */
        cpssOsMemSet(txDescList[txQueue].revPacketIdDb,0,(sizeof(GT_U32)*txDescList[txQueue].maxDescNum));

        txDescList[txQueue].userQueueEnabled = GT_FALSE;

        generatorsTotalDesc += txDescList[txQueue].maxDescNum;
    }

    PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.generatorsTotalDesc = generatorsTotalDesc;
    /* Allocate place for packet ID DB of Packet Generators */
    if( 0 != generatorsTotalDesc )
    {
        /* allocate packet ID cookies memory */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie =
                (GT_U32*)cpssOsMalloc(sizeof(GT_U32)*generatorsTotalDesc);
        if( NULL == PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        /* fill packet ID cookies memory */
        cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.cookie,
                     0xFF,
                     sizeof(GT_U32)*generatorsTotalDesc);

        /* allocate free packet ID linked list */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.freeLinkedList =
                (GT_U32*)cpssOsMalloc(sizeof(GT_U32)*generatorsTotalDesc);
        if( NULL == PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.freeLinkedList)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        /* initialize the packet ID linked list - each ID pointing to the next */
        for( ii = 0; ii < generatorsTotalDesc ; ii++ )
        {
            PRV_CPSS_PP_MAC(devNum)->
                intCtrl.txGeneratorPacketIdDb.freeLinkedList[ii] = ii + 1;
        }

        PRV_CPSS_PP_MAC(devNum)->intCtrl.txGeneratorPacketIdDb.firstFree = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* sdmaTxInit
*
* DESCRIPTION:
*       This function initializes the Core Tx module, by allocating the cyclic
*       Tx descriptors list, and the tx Headers buffers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - The device number to init the Tx unit for.
*       descBlockPtr   - A block to allocate the descriptors from.
*       descBlockSize  - Size in bytes of descBlockPtr.
*
* OUTPUTS:
*       numOfDescsPtr  - Number of allocated Tx descriptors.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS sdmaTxInit
(
    IN GT_U8    devNum,
    IN GT_U8    *descBlockPtr,
    IN GT_U32   descBlockSize,
    OUT GT_U32  *numOfDescsPtr
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddr;
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;   /* Points to the relevant Tx desc. list */

    PRV_CPSS_TX_DESC_STC *firstTxDesc;/* Points to the first Tx desc in list. */
    GT_U8 txQueue;              /* Index of the Tx Queue.               */
    GT_U32 numOfTxDesc;         /* Number of Tx desc. that may be       */
                                /* allocated from the given block.      */
    GT_U32 sizeOfDesc;          /* The amount of memory (in bytes) that*/
                                /* a single desc. will occupy, including*/
                                /* the alignment.                       */
    GT_U32 descPerQueue;        /* Number of descriptors per Tx Queue.  */
    GT_U32 i;
    GT_U8 *shortBuffsBlock;     /* The Tx short buffers block taken     */
                                /* from the descBlockPtr.               */
    PRV_CPSS_SW_TX_DESC_STC *swTxDesc = NULL;/* Points to the Sw Tx desc to   */
                                /* init.                                */
    PRV_CPSS_SW_TX_DESC_STC *firstSwTxDesc;/* Points to the first Sw Tx desc  */
                                /* in list.                             */
    GT_UINTPTR  phyNext2Feed;       /* The physical address of the next2Feed*/
                                /* field.                               */
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */
    GT_UINTPTR nextDescPointer;
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;

    moduleCfgPtr = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum);

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    txDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList;
    regsAddr    = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* Set the descBlockPtr to point to an aligned start address. */
    if(((GT_UINTPTR)descBlockPtr % TX_DESC_ALIGN) != 0)
    {
        descBlockSize -= TX_DESC_ALIGN - (((GT_UINTPTR)descBlockPtr) % TX_DESC_ALIGN);

        descBlockPtr =
            (GT_U8*)((GT_UINTPTR)descBlockPtr +
                     (TX_DESC_ALIGN - (((GT_UINTPTR)descBlockPtr) % TX_DESC_ALIGN)));
    }

    /* Tx Descriptor list initialization.   */
    sizeOfDesc  = sizeof(PRV_CPSS_TX_DESC_STC);

    if((sizeOfDesc % TX_DESC_ALIGN) != 0)
    {
        sizeOfDesc += (TX_DESC_ALIGN-(sizeof(PRV_CPSS_TX_DESC_STC) % TX_DESC_ALIGN));

    }

    numOfTxDesc = descBlockSize / sizeOfDesc;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum)==GT_TRUE)
    {
        /* each tx descriptor is 16 bytes (sizeof PRV_CPSS_TX_DESC_STC) */
        /* each 'short buffer' is 16 bytes ---> TX_SHORT_BUFF_SIZE_16_CNS */
        /* for each tx descriptor we need 'short buffer' */
        /* so ration is 1:1 --> meaning split -->
            1/2 for tx descriptors
            1/2 for 'short buffers' */

        numOfTxDesc = numOfTxDesc / 2;
    }
    else
    {
        /* each tx descriptor is 16 bytes (sizeof PRV_CPSS_TX_DESC_STC) */
        /* each 'short buffer' is 8 bytes --> TX_SHORT_BUFF_SIZE */
        /* for each tx descriptor we need 'short buffer' */
        /* so ration is 2:1 --> meaning split -->
            2/3 for tx descriptors
            1/3 for 'short buffers' */

        numOfTxDesc = (numOfTxDesc * 2) / 3;
    }

    /* Short buffers are right after descriptors. Short buffers must be
       64 Bit (8 Byte) aligned. Descriptors are already aligned to 16 bytes.
       So shortBuffsBlock is aligned also. */
    shortBuffsBlock   = (GT_U8*)(((GT_UINTPTR)descBlockPtr) + (numOfTxDesc * sizeOfDesc));

    descPerQueue = numOfTxDesc / NUM_OF_TX_QUEUES;
    /* Number of descriptors must be divided by 2.  */
    descPerQueue -= (descPerQueue & 0x1);

    *numOfDescsPtr = descPerQueue * NUM_OF_TX_QUEUES;

    for(txQueue = 0; txQueue < NUM_OF_TX_QUEUES; txQueue++)
    {
        /* Queue is assigned for Packet Generator */
        if(CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E ==
            moduleCfgPtr->
               multiNetIfCfg.txSdmaQueuesConfig[portGroupId][txQueue].queueMode)
        {
            continue;
        }

        txDescList[txQueue].freeDescNum = descPerQueue;
        txDescList[txQueue].maxDescNum = descPerQueue;

        firstTxDesc = (PRV_CPSS_TX_DESC_STC*)descBlockPtr;

        /* hotSynch/shutdown treatment - allocate Mem only once */
        if (txDescList[txQueue].swTxDescBlock == NULL)
        {
            firstSwTxDesc = (PRV_CPSS_SW_TX_DESC_STC *)
                         cpssOsMalloc(sizeof(PRV_CPSS_SW_TX_DESC_STC )*descPerQueue);
            if(firstSwTxDesc == NULL)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }
            cpssOsMemSet(firstSwTxDesc, 0, sizeof(PRV_CPSS_SW_TX_DESC_STC )*descPerQueue);

            /* save pointer to allocated Mem block on per queue structure */
            txDescList[txQueue].swTxDescBlock = firstSwTxDesc;
        }
        else
        {
            firstSwTxDesc = txDescList[txQueue].swTxDescBlock;
        }

        if(firstSwTxDesc == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        /* create Tx End FIFO in request driven mode */
        if (txDescList[txQueue].poolId == 0)
        {
            if (cpssBmPoolCreate(sizeof(PRV_CPSS_SW_TX_FREE_DATA_STC),
                                 CPSS_BM_POOL_4_BYTE_ALIGNMENT_E,
                                 descPerQueue,
                                 &txDescList[txQueue].poolId) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
            }

        }
        else
        {
            if (cpssBmPoolReCreate(txDescList[txQueue].poolId) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
            }
        }

        for(i = 0; i < descPerQueue; i++)
        {

            swTxDesc = firstSwTxDesc + i;


            swTxDesc->txDesc = (PRV_CPSS_TX_DESC_STC*)descBlockPtr;
            descBlockPtr   = (GT_U8*)((GT_UINTPTR)descBlockPtr + sizeOfDesc);

            TX_DESC_RESET_MAC(swTxDesc->txDesc);

            if((descPerQueue - 1) != i)
            {
                /* Next descriptor should not be configured for the last one.*/
                swTxDesc->swNextDesc  = firstSwTxDesc + i + 1;
                rc = cpssOsVirt2Phy((GT_UINTPTR)descBlockPtr, /*OUT*/&nextDescPointer);
                if (rc != GT_OK)
                {
                    return rc;
                }

                #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                    if (0 != (nextDescPointer & 0xffffffff00000000L))
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                    }
                #endif

                swTxDesc->txDesc->nextDescPointer = CPSS_32BIT_LE((GT_U32)nextDescPointer);
            }

            swTxDesc->shortBuffer = shortBuffsBlock;

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum)==GT_TRUE)
            {
                shortBuffsBlock =
                (GT_U8*)((GT_UINTPTR)shortBuffsBlock + TX_SHORT_BUFF_SIZE_16_CNS);
            }
            else
            {
                shortBuffsBlock =
                (GT_U8*)((GT_UINTPTR)shortBuffsBlock + TX_SHORT_BUFF_SIZE);
            }

        }

        /* Close the cyclic desc. list. */
        swTxDesc->swNextDesc = firstSwTxDesc;
        rc = cpssOsVirt2Phy((GT_UINTPTR)firstTxDesc, /*OUT*/&nextDescPointer);
        if (rc != GT_OK)
        {
            return rc;
        }

        #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
            if (0 != (nextDescPointer & 0xffffffff00000000L))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        #endif

        swTxDesc->txDesc->nextDescPointer = CPSS_32BIT_LE((GT_U32)nextDescPointer);

        txDescList[txQueue].next2Feed   = firstSwTxDesc;
        txDescList[txQueue].next2Free   = firstSwTxDesc;
    }


    if((GT_UINTPTR)shortBuffsBlock >
       ((GT_UINTPTR)moduleCfgPtr->netIfCfg.txDescBlock +
                    moduleCfgPtr->netIfCfg.txDescBlockSize))
    {
        /*wrong calculation of the short buffers */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    for(i = 0; i < NUM_OF_TX_QUEUES; i++)
    {
        /* Queue is assigned for Packet Generator */
        if(CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E ==
            moduleCfgPtr->
               multiNetIfCfg.txSdmaQueuesConfig[portGroupId][i].queueMode)
        {
            continue;
        }

        rc = cpssOsVirt2Phy((GT_UINTPTR)(txDescList[i].next2Feed->txDesc),/*OUT*/&phyNext2Feed);
        if (rc != GT_OK)
        {
            return rc;
        }

        #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
            if (0 != (phyNext2Feed & 0xffffffff00000000L))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        #endif

        rc = prvCpssHwPpPortGroupWriteRegister(
            devNum,
            portGroupId,
            regsAddr->sdmaRegs.txDmaCdp[i],(GT_U32)phyNext2Feed);

        if(rc != GT_OK)
            return rc;
    }

    return sdmaTxRegConfig(devNum);
}

/*******************************************************************************
* sdmaRxInit
*
* DESCRIPTION:
*       This function initializes the Core Rx module, by allocating the cyclic
*       Rx descriptors list, and the rx buffers. -- SDMA
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number to init the Rx unit for.
*       descBlockPtr  - A block to allocate the descriptors from.
*       descBlockSize - Size in bytes of descBlock.
*       rxBufInfoPtr  - A block to allocate the Rx buffers from.
*
* OUTPUTS:
*       numOfDescsPtr  - Number of Rx descriptors allocated.
*       numOfBufsPtr   - Number of Rx buffers allocated.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS sdmaRxInit
(
    IN GT_U8                    devNum,
    IN GT_U8                    *descBlockPtr,
    IN GT_U32                   descBlockSize,
    IN CPSS_RX_BUF_INFO_STC     *rxBufInfoPtr,
    OUT GT_U32                  *numOfDescsPtr,
    OUT GT_U32                  *numOfBufsPtr
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddr;
    PRV_CPSS_RX_DESC_LIST_STC *rxDescList;   /* Points to the relevant Rx desc. list */
    PRV_CPSS_RX_DESC_STC *rxDesc = NULL; /* Points to the Rx desc to init.    */
    PRV_CPSS_RX_DESC_STC *firstRxDesc;/* Points to the first Rx desc in list. */
    PRV_CPSS_RX_DESC_STC *theFirstRxDesc;/* Points to the first Rx desc. */
    GT_U8 rxQueue;              /* Index of the Rx Queue.               */
    GT_U32 numOfRxDesc;         /* Number of Rx desc. that may be       */
                                /* allocated from the given block.      */
    GT_U32 sizeOfDesc;          /* The amount of memory (in bytes) that*/
                                /* a single desc. will occupy, including*/
                                /* the alignment.                       */
    GT_U32 actualBuffSize;      /* Size of a single buffer, after taking*/
                                /* the required alignment into account.*/
    PRV_CPSS_SW_RX_DESC_STC *swRxDesc = NULL;/* Points to the Sw Rx desc to   */
                                /* init.                                */
    PRV_CPSS_SW_RX_DESC_STC *firstSwRxDesc;/* Points to the first Sw Rx desc  */
                                /* in list.                             */
    GT_U32 headerOffsetSize;    /* Size in bytes of the header offset   */
                                /* after alignment to RX_BUFF_ALIGN.    */
    GT_UINTPTR virtBuffAddr;    /* The virtual address of the Rx buffer */
                                /* To be enqueued into the current Rx   */
                                /* Descriptor.                          */
    GT_UINTPTR phyAddr;         /* Physical Rx descriptor's address.    */
    GT_U32 i;                   /* Loop counter                         */
    GT_U32 rxSdmaRegVal;        /* Rx SDMA Queues Register value        */

    /* The following vars. will hold the data from rxBufInfoPtr, and only  */
    /* some of them will be used, depending on the allocation method.   */
    CPSS_RX_BUFF_ALLOC_METHOD_ENT allocMethod;
    GT_U32 buffBlockSize = 0;   /* The static buffer block size (bytes) */
    GT_U8 *buffBlock = NULL;    /* A pointer to the static buffers block*/
    CPSS_RX_BUFF_MALLOC_FUNC mallocFunc = NULL;/* Function for allocating the buffers. */
    GT_U32         buffPercentageSum;/* buffer percentage summary       */
    GT_BOOL        needNetIfInit; /* network interface initialization or reconstruction.*/
    GT_U32         tmpValue;/* tempo value */
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */
    GT_U32         headerOffset = rxBufInfoPtr->headerOffset;
    GT_U32         buffSize = rxBufInfoPtr->rxBufSize;
    GT_U32         regOffset; /* offset in register */
    GT_U32         regData;   /* register data */

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    buffPercentageSum = 0;

    *numOfDescsPtr = 0;
    *numOfBufsPtr  = 0;

    /* save space before user header for internal packets indication */
    buffSize     += PRV_CPSS_DXCH_NET_INTERNAL_RX_PACKET_INFO_SIZE_CNS;
    headerOffset += PRV_CPSS_DXCH_NET_INTERNAL_RX_PACKET_INFO_SIZE_CNS;

    rxDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList;
    regsAddr    = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /*  we need to set TO all port groups that the representative for 'TO_CPU packets'
            is the first active port group.
        */
        tmpValue = portGroupId << 1;

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                /* Not used any more */
            }
            else
            {
                /* bit 0 - <CpuPortMode> set to 0 'global mode' */
                /* bits 1..4 -  <CpuTargetCore> set to the 'SDMA_PORT_GROUP_ID'  */
                rc = prvCpssHwPpSetRegField(devNum,
                        PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.cpuPortDist,
                        0,5,
                        tmpValue);
            }
        }
        else
        {
            /* bit 0 - <CpuPortMode> set to 0 'global mode' */
            /* bits 1..2 -  <CpuTargetCore> set to the 'SDMA_PORT_GROUP_ID'  */
            rc = prvCpssHwPpSetRegField(devNum,
                    regsAddr->txqVer1.egr.global.cpuPortDistribution,0,3,
                    tmpValue);
        }

        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* get that state of the device , check if the device need network interface */
    /* initialization or reconstruction.                                         */
    rc = prvCpssPpConfigDevDbNetIfInitGet(devNum, &needNetIfInit);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* we need to get the values that were last used , when the HW didn't do reset */
    if(needNetIfInit == GT_FALSE)
    {
        /* get the info to synch with current HW state
           NOTE: the 'save' of this info was done in
           prvCpssPpConfigDevDbPrvInfoSet(...) */
        rc = prvCpssPpConfigDevDbPrvNetInfInfoGet(devNum, rxDescList);
        if (rc != GT_OK)
        {
            return rc;
        }

        for(rxQueue = 0; rxQueue < NUM_OF_RX_QUEUES; rxQueue++)
        {
            /* update the total number of desc */
            *numOfDescsPtr += rxDescList[rxQueue].freeDescNum;

            /* update the total number of buffers */
            *numOfBufsPtr += rxDescList[rxQueue].freeDescNum;

            /* Enable Rx SDMA Queue */
            rc = cpssDxChNetIfSdmaRxQueueEnable(devNum, rxQueue, GT_TRUE);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;;
    }

    /* Due to Cheetah 3 Errata - " FEr#2009: DSDMA resource errors may cause
       PCI Express packets reception malfunction"
       -- see PRV_CPSS_DXCH3_SDMA_WA_E */
    if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH3_SDMA_WA_E) == GT_TRUE)
    {
        /* A bit per TC defines the behavior in case of Rx resource error.
           Set all bits to 1=drop; In a case of resource error, drop the
           current packet */

        rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regsAddr->sdmaRegs.sdmaCfgReg,8,8,0xff);
        if(rc != GT_OK)
            return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* no such bit as there is no special MAC for CPU */
    }
    else
    /* Set SelPortSDMA to SDMA */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,
                                                regsAddr->globalRegs.globalControl,20,1,1);
        if(rc != GT_OK)
            return rc;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            /* Set CPUPortActive to not active */
            regOffset = 0;
            regData = 0;
        }
        else /* CPSS_PP_FAMILY_DXCH_XCAT3_E */
        {
            /* Set CPU to work in SDMA mode */
            regOffset = 1;
            regData = 1;
        }

        rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,
                                                    regsAddr->globalRegs.cpuPortCtrlReg,
                                                    regOffset,1,regData);
        if(rc != GT_OK)
            return rc;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_XCAT3_RM_RX_SDMA_WRONG_QUEUE_WA_E))
    {
        /* Set <sel_port_sdma> in General Configuration register to be 0 */
        rc = prvCpssHwPpSetRegField(devNum, regsAddr->globalRegs.generalConfigurations, 0, 1, 0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    allocMethod = rxBufInfoPtr->allocMethod;
    if(allocMethod == CPSS_RX_BUFF_DYNAMIC_ALLOC_E)
    {
        mallocFunc = rxBufInfoPtr->buffData.dynamicAlloc.mallocFunc;
    }
    else
    {
        buffBlock = (GT_U8*)rxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr;
        buffBlockSize = rxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize;
    }

    /* Set the descBlockPtr to point to an aligned start address. */
    if(((GT_UINTPTR)descBlockPtr % RX_DESC_ALIGN) != 0)
    {
        descBlockSize = descBlockSize - (RX_DESC_ALIGN - ((GT_UINTPTR)descBlockPtr % RX_DESC_ALIGN));
        descBlockPtr = descBlockPtr + (RX_DESC_ALIGN - ((GT_UINTPTR)descBlockPtr % RX_DESC_ALIGN));
    }

    /* Rx Descriptor list initialization.   */
    sizeOfDesc = sizeof(PRV_CPSS_RX_DESC_STC);

    if((sizeOfDesc % RX_DESC_ALIGN) != 0)
    {
        sizeOfDesc += (RX_DESC_ALIGN -(sizeof(PRV_CPSS_RX_DESC_STC) % RX_DESC_ALIGN));
    }

    /* The buffer size must be a multiple of RX_BUFF_SIZE_MULT  */
    actualBuffSize = buffSize - (buffSize % RX_BUFF_SIZE_MULT);
    if(actualBuffSize == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if(allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
    {
        /* Number of Rx descriptors is calculated according to the  */
        /* size of the given Rx Buffers block.                      */
        /* Take the "dead" block in head of the buffers block as a  */
        /* result of the allignment.                                */
        numOfRxDesc = buffBlockSize / actualBuffSize;

        /* Set numOfRxDesc according to the number of descriptors that  */
        /* may be allocated from descBlockPtr and the number of buffers */
        /* that may be allocated from buffBlock.                        */
        if((descBlockSize / sizeOfDesc) < numOfRxDesc)
            numOfRxDesc = descBlockSize / sizeOfDesc;
    }
    else /* dynamic and no buffer allocation */
    {
        /* Number of Rx descriptors is calculated according to the  */
        /* (fixed) size of the given Rx Descriptors block.                  */
        numOfRxDesc = descBlockSize / sizeOfDesc;
    }

    theFirstRxDesc = (PRV_CPSS_RX_DESC_STC*)descBlockPtr;

    for(rxQueue = 0; rxQueue < NUM_OF_RX_QUEUES; rxQueue++)
    {
        buffPercentageSum += rxBufInfoPtr->bufferPercentage[rxQueue];

        /* validate the total percentage */
        if (buffPercentageSum > 100)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        /* set the desc count according to the percentage */
        rxDescList[rxQueue].freeDescNum =
            (rxBufInfoPtr->bufferPercentage[rxQueue] * numOfRxDesc) / 100;
        rxDescList[rxQueue].maxDescNum = rxDescList[rxQueue].freeDescNum;
        /* update the total number of desc */
        *numOfDescsPtr += rxDescList[rxQueue].freeDescNum;

        rxDescList[rxQueue].forbidQEn = GT_FALSE;

        firstRxDesc = (PRV_CPSS_RX_DESC_STC*)descBlockPtr;

        /* hotSynch/shutdown treatment - Allocate Mem only once */
        if (rxDescList[rxQueue].swRxDescBlock == NULL)
        {
            firstSwRxDesc = (PRV_CPSS_SW_RX_DESC_STC*)
                         cpssOsMalloc(sizeof(PRV_CPSS_SW_RX_DESC_STC)*
                                        rxDescList[rxQueue].freeDescNum);
            /* save pointer to allocated Mem block on per queue structure */
            rxDescList[rxQueue].swRxDescBlock = firstSwRxDesc;
        }
        else
        {
            firstSwRxDesc = rxDescList[rxQueue].swRxDescBlock;
        }

        if(firstSwRxDesc == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        /* loop on all descriptors in the queue*/
        for(i = 0; i < rxDescList[rxQueue].freeDescNum; i++)
        {
            swRxDesc = firstSwRxDesc + i;

            rxDesc      = (PRV_CPSS_RX_DESC_STC*)descBlockPtr;
            descBlockPtr   = (GT_U8*)((GT_UINTPTR)descBlockPtr + sizeOfDesc);
            swRxDesc->rxDesc = rxDesc;
            RX_DESC_RESET_MAC(devNum,swRxDesc->rxDesc);

            if((rxDescList[rxQueue].freeDescNum - 1) != i)
            {
                /* Next descriptor should not be configured for the last one.*/
                swRxDesc->swNextDesc  = (firstSwRxDesc + i + 1);
                rc = cpssOsVirt2Phy((GT_UINTPTR)descBlockPtr, &phyAddr);
                if (rc != GT_OK)
                {
                    return rc;
                }

                #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                    if (0 != (phyAddr & 0xffffffff00000000L))
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                    }
                #endif

                swRxDesc->rxDesc->nextDescPointer = CPSS_32BIT_LE((GT_U32)phyAddr);
            }
        }

        /* Close the cyclic desc. list. */
        swRxDesc->swNextDesc = firstSwRxDesc;
        rc = cpssOsVirt2Phy((GT_UINTPTR)firstRxDesc, &phyAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
            if (0 != (phyAddr & 0xffffffff00000000L))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        #endif

        swRxDesc->rxDesc->nextDescPointer = CPSS_32BIT_LE((GT_U32)phyAddr);

        rxDescList[rxQueue].next2Receive    = firstSwRxDesc;
        rxDescList[rxQueue].next2Return     = firstSwRxDesc;
    }

    /* save Rx descriptor initial configuration parameters for later use after SW restart */
    rc  = prvCpssPpConfigDevDbPrvNetInfInfoSet(devNum, theFirstRxDesc, rxDescList);
    if(rc != GT_OK)
        return rc;

    /* Rx Buffers initialization.           */
    if (allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E ||
        allocMethod == CPSS_RX_BUFF_DYNAMIC_ALLOC_E)
    {
       if(allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
        {
            /* Set the buffers block to point to a properly alligned block. */
            if(((GT_UINTPTR)buffBlock % RX_BUFF_ALIGN) != 0)
            {
                buffBlockSize = (buffBlockSize -
                                 (RX_BUFF_ALIGN -
                                  ((GT_UINTPTR)buffBlock % RX_BUFF_ALIGN)));

                buffBlock =
                    (GT_U8*)((GT_UINTPTR)buffBlock +
                             (RX_BUFF_ALIGN - ((GT_UINTPTR)buffBlock % RX_BUFF_ALIGN)));
            }

            /* Check if the given buffers block, is large enough to be cut  */
            /* into the needed number of buffers.                           */
            if((buffBlockSize / (*numOfDescsPtr)) < RX_BUFF_SIZE_MULT)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        headerOffsetSize = headerOffset;
        if((headerOffsetSize % RX_BUFF_ALIGN) != 0)
        {
            headerOffsetSize += (RX_BUFF_ALIGN -(headerOffsetSize % RX_BUFF_ALIGN));
        }

        for(rxQueue = 0; rxQueue < NUM_OF_RX_QUEUES; rxQueue++)
        {
            swRxDesc = rxDescList[rxQueue].next2Receive;
            rxDescList[rxQueue].headerOffset = headerOffset;

            /* update the total number of buffers */
            *numOfBufsPtr += rxDescList[rxQueue].freeDescNum;

            for(i = 0; i < rxDescList[rxQueue].freeDescNum; i++)
            {
                RX_DESC_SET_BUFF_SIZE_FIELD_MAC(devNum,swRxDesc->rxDesc,
                                            (actualBuffSize - headerOffsetSize));

                /* Set the Rx desc. buff pointer field. */
                if(allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
                {
                    virtBuffAddr = ((GT_UINTPTR)buffBlock) + headerOffsetSize;
                    rc = cpssOsVirt2Phy(virtBuffAddr, &phyAddr);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                        if (0 != (phyAddr & 0xffffffff00000000L))
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                        }
                    #endif

                    swRxDesc->rxDesc->buffPointer = CPSS_32BIT_LE((GT_U32)phyAddr);

                    buffBlock = (GT_U8*)(((GT_UINTPTR)buffBlock) + actualBuffSize);

                    /* Set the buffers block to point to a properly alligned block*/
                    if(((GT_UINTPTR)buffBlock % RX_BUFF_ALIGN) != 0)
                    {
                        buffBlock =
                            (GT_U8*)((GT_UINTPTR)buffBlock +
                                     (RX_BUFF_ALIGN -
                                      ((GT_UINTPTR)buffBlock % RX_BUFF_ALIGN)));
                    }
                }
                else
                {
                    virtBuffAddr = (GT_UINTPTR)mallocFunc(actualBuffSize,RX_BUFF_ALIGN);
                    if(virtBuffAddr == 0)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

                    rc = cpssOsVirt2Phy(virtBuffAddr + headerOffsetSize, &phyAddr);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    #if __WORDSIZE == 64    /* phyAddr must fit in 32 bit */
                        if (0 != (phyAddr & 0xffffffff00000000L))
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                        }
                    #endif

                    swRxDesc->rxDesc->buffPointer = CPSS_32BIT_LE((GT_U32)phyAddr);
                }

                swRxDesc = swRxDesc->swNextDesc;
            }
        }
    }
    else /* no buffer allocation*/
    {
        for(rxQueue = 0; rxQueue < NUM_OF_RX_QUEUES; rxQueue++)
        {
            rxDescList[rxQueue].freeDescNum = 0;
        }
    }


    /* read rxSDMA Queue Register */
    rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,&rxSdmaRegVal);
    if(rc != GT_OK)
        return rc;

    /* fix of CQ 106607 : disable SDMA if it is enabled (during cpss restart)
       because if traffic is send to the CPU queues, then it will change the
       'link list' that we try to set to the PP during this function, that may
       cause asynchronous settings between the SW descriptors and the HW descriptors

       --> so we disable the queues at this stage and will enable right after the
       settings of the first descriptor into each queue.
    */
    if(rxSdmaRegVal & 0xFF)/* one of the queues enabled */
    {
        for(i = 0; i < NUM_OF_RX_QUEUES; i++)
        {
            /* for each queue set Enable bit to 0, and Disable bit to 1 */
            rxSdmaRegVal |= 1 << (i + NUM_OF_RX_QUEUES);
            rxSdmaRegVal &= (~(1 << i));

            /* write Rx SDMA Queues Reg - Disable Rx SDMA Queues */
            rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,rxSdmaRegVal);
            if(rc != GT_OK)
                return rc;
        }

        /* wait for DMA operations to end on all queues */
        /* wait for bits 0xff to clear */
        rc = prvCpssPortGroupBusyWaitByMask(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,0xff,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read rxSDMA Queue Register */
        rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,&rxSdmaRegVal);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    for(i = 0; i < NUM_OF_RX_QUEUES; i++)
    {
        /* set the first descriptor (start of link list) to the specific queue */
        rc = cpssOsVirt2Phy((GT_UINTPTR)(rxDescList[i].next2Receive->rxDesc),  /*OUT*/&phyAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        #if __WORDSIZE == 64
            /* phyAddr must fit in 32 bit */
            if (0 != (phyAddr & 0xffffffff00000000L))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        #endif

        rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxDmaCdp[i],(GT_U32)phyAddr);
        if(rc != GT_OK)
            return rc;

        /* Set the Receive Interrupt Frame Boundaries   */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regsAddr->sdmaRegs.sdmaCfgReg,0,1,1);
        if(rc != GT_OK)
            return rc;

        /* for each queue set Enable bit to 1, and Disable bit to 0.*/
        rxSdmaRegVal |= 1 << i;
        rxSdmaRegVal &= (~(1 << (i + NUM_OF_RX_QUEUES)));
    }
    /* Do not enable the queues in CPSS_RX_BUFF_NO_ALLOC_E mode. */
    /* Application responsibility to enable the queue after all buffers attached. */
    if (allocMethod != CPSS_RX_BUFF_NO_ALLOC_E)
    {
        /* if rxsdma init is going under recovery process as PP is disabled to write CPU memory, enable Rx SDMA queues just  */
        /* after PP would be able to access CPU memory(in catch up stage)- otherwise syncronization may be lost between      */
        /* PP and descriptors                                                                                                */
        if (systemRecoveryInfo.systemRecoveryMode.haCpuMemoryAccessBlocked == GT_FALSE )
        {
            /* write Rx SDMA Queues Reg - Enable Rx SDMA Queues */
            rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,rxSdmaRegVal);
            if(rc != GT_OK)
                return rc;
        }
    }

#ifndef ASIC_SIMULATION
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE ||
       CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        for ( i = 0 ; i < 8 ; i++ )
        {
            /* write Rx SDMA Resource Error Count and Mode - Retry, re-enable queue */
            rc = prvCpssHwPpPortGroupSetRegField(
                        devNum,
                        portGroupId,
                        regsAddr->sdmaRegs.rxSdmaResourceErrorCountAndMode[i],
                        9, 1, 1);
            if(rc != GT_OK)
                return rc;
        }
    }
#endif /* ASIC_SIMULATION */

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChNetIfInit
*
* DESCRIPTION:
*       Initialize the network interface SDMA structures, Rx descriptors & buffers
*       and Tx descriptors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device to initialize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   1. The application must call only one of the following APIs per device:
*   cpssDxChNetIfMiiInit - for MII/RGMII Ethernet port networkIf initialization.
*   cpssDxChNetIfInit - for SDMA networkIf initialization.
*   In case more than one of the mentioned above API is called
*   GT_FAIL will be return.
*   2. In case CPSS_RX_BUFF_NO_ALLOC_E is set, the application must enable
*   RX queues after attaching the buffers. See: cpssDxChNetIfSdmaRxQueueEnable.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfInit
(
    IN  GT_U8       devNum
)
{
    GT_STATUS          rc;
    GT_U32             numRxDataBufs;   /* These vars. are used to be   */
    GT_U32             numRxDescriptors;/* sent to Tx /Rx initialization*/
    GT_U32             numTxDescriptors;/* functions. and hold the      */
                                        /* number of the different descs*/
                                        /* and buffers allocated.       */

    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;
    GT_U8       tcQueue;
    GT_BOOL     tcQueueEnable;
    CPSS_SYSTEM_RECOVERY_INFO_STC   tempSystemRecoveryInfo;
    /* save current recovery state */
    tempSystemRecoveryInfo = systemRecoveryInfo;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    moduleCfgPtr = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum);

    /* initialize the DB , to be like the HW reset value */
    rc = cpssDxChNetIfPrePendTwoBytesHeaderSet(devNum,GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /* SDMA mode */
        /* Check for valid configuration parameters */
        if((moduleCfgPtr->netIfCfg.rxDescBlock == NULL) ||
           (moduleCfgPtr->netIfCfg.rxDescBlockSize == 0) ||
           ((moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E) &&
           ((moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr == NULL) ||
           (moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize == 0))) ||
           ((moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_DYNAMIC_ALLOC_E) &&
           (moduleCfgPtr->netIfCfg.rxBufInfo.buffData.dynamicAlloc.mallocFunc == NULL)) ||
           (moduleCfgPtr->netIfCfg.txDescBlock == NULL) ||
           (moduleCfgPtr->netIfCfg.txDescBlockSize== 0))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
        if ((tempSystemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
            (tempSystemRecoveryInfo.systemRecoveryMode.haCpuMemoryAccessBlocked == GT_TRUE) )
        {
            if (tempSystemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)
            {
                /* HA event took place. Shut down DMA before init */
                rc =  cpssDxChNetIfRemove(devNum);
                if (rc != GT_OK)
                {
                    systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                    return rc;
                }
            }
        }
        if( (tempSystemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E) ||
            ((tempSystemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E)&&
             (tempSystemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)) )
        {
            /* This point is reachable only in case of System Recovery rx disable mode or*/
            /* any other init action  not connected with System Recovery process. In both*/
            /* cases hw write is enable*/

            /* init RX */
            rc = sdmaRxInit(devNum,(GT_U8*)moduleCfgPtr->netIfCfg.rxDescBlock,
                            moduleCfgPtr->netIfCfg.rxDescBlockSize,
                            &(moduleCfgPtr->netIfCfg.rxBufInfo),
                            &numRxDescriptors,&numRxDataBufs);
            if (GT_OK != rc)
            {
                systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                return rc;
            }
        }

        if( (tempSystemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E) ||
            ((tempSystemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E)&&
             (tempSystemRecoveryInfo.systemRecoveryMode.continuousTx == GT_FALSE)) )
        {
            /* This point is reachable only in case of System Recovery rx disable mode or*/
            /* any other init action  not connected with System Recovery process. In both*/
            /* cases hw write is enable*/

            /* init TX */
            rc = sdmaTxInit(devNum,(GT_U8*)moduleCfgPtr->netIfCfg.txDescBlock,
                            moduleCfgPtr->netIfCfg.txDescBlockSize,
                            &numTxDescriptors);
            if (GT_OK != rc)
            {
                systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                return rc;
            }

            /* init TX Generator */
            rc = sdmaTxGeneratorInit(devNum);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
        if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                        PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E))
        {
            if (!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            {
                /* Don't apply this WA on CPU port in SDMA mode, */
                /* restore queueing of packets on all TC queues */
                for(tcQueue = 0; tcQueue < CPSS_TC_RANGE_CNS; tcQueue++)
                {
                    tcQueueEnable = CPSS_PORTS_BMP_IS_PORT_SET_MAC((&(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                        info_PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E.portTxQueuesBmpPtr[tcQueue])),CPSS_CPU_PORT_NUM_CNS);
                    tcQueueEnable = BIT2BOOL_MAC(tcQueueEnable);

                    rc = prvCpssDxChPortTxQueueingEnableSet(devNum,CPSS_CPU_PORT_NUM_CNS,tcQueue,tcQueueEnable);
                    if (GT_OK != rc)
                    {
                        systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                        return rc;
                    }
                }
            }
        }
        systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
        /* bind the DSMA Tx completed callback to the driver */
        rc = prvCpssDrvInterruptPpTxEndedCbBind(devNum,
                                    prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle);
        if(rc != GT_OK)
        {
            return rc;
        }

    }
    else
    {
        /* SDMA emulation over RGMII/MII */
        CPSS_DXCH_NETIF_MII_INIT_STC neifMiiInit;
        GT_U32 miiDevNum;
        GT_U32 numOfRxBufs;
        GT_U8 *tmpRxBufBlockPtr;
        GT_U32 i;

        miiDevNum = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E.devNum;
        if (miiDevNum == devNum)
        {
            /* Check for valid configuration parameters */
            if(((moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E) &&
                ((moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr == NULL) ||
                 (moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize == 0))) ||
               ((moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_DYNAMIC_ALLOC_E) &&
                (moduleCfgPtr->netIfCfg.rxBufInfo.buffData.dynamicAlloc.mallocFunc == NULL)) ||
               (moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_NO_ALLOC_E) ||
               (moduleCfgPtr->netIfCfg.txDescBlock == NULL) ||
               (moduleCfgPtr->netIfCfg.txDescBlockSize== 0))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            /* this block in all recovery modes run the same init sequence: hw should be enable for write operation*/
            systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;

            neifMiiInit.txInternalBufBlockPtr = (GT_U8 *)moduleCfgPtr->netIfCfg.txDescBlock;
            neifMiiInit.txInternalBufBlockSize = moduleCfgPtr->netIfCfg.txDescBlockSize;
            /* Set the number of TX descriptors to (number of tx header buffers) * 2 */
            neifMiiInit.numOfTxDesc =
                (neifMiiInit.txInternalBufBlockSize / CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS)*2;
            neifMiiInit.rxBufSize = moduleCfgPtr->netIfCfg.rxBufInfo.rxBufSize;
            neifMiiInit.headerOffset = moduleCfgPtr->netIfCfg.rxBufInfo.headerOffset;


            for (i = 0;i < CPSS_MAX_RX_QUEUE_CNS;i++)
            {
                neifMiiInit.bufferPercentage[i] =
                    moduleCfgPtr->netIfCfg.rxBufInfo.bufferPercentage[i];
            }

            if (moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
            {
                neifMiiInit.rxBufBlockPtr = (GT_U8 *)moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr;
                neifMiiInit.rxBufBlockSize = moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize;
            }
            else
            {
                numOfRxBufs = moduleCfgPtr->netIfCfg.rxBufInfo.buffData.dynamicAlloc.numOfRxBuffers;

                /* set the requested buffer block size */
                neifMiiInit.rxBufBlockSize = numOfRxBufs * neifMiiInit.rxBufSize;

                /* allocate memory for buffer pool */
                tmpRxBufBlockPtr = moduleCfgPtr->netIfCfg.rxBufInfo.buffData.dynamicAlloc.mallocFunc(
                                    neifMiiInit.rxBufBlockSize, ALIGN_4B_CNS);

                if (NULL == tmpRxBufBlockPtr)
                {
                    systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                neifMiiInit.rxBufBlockPtr = tmpRxBufBlockPtr;
            }
            rc = prvCpssDxChNetIfMiiInitNetIfDev(devNum,&neifMiiInit);
            if (rc != GT_OK)
            {
                systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
        }
    }
    PRV_CPSS_PP_MAC(devNum)->cpuPortMode = CPSS_NET_CPU_PORT_MODE_SDMA_E;
    PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.allocMethod = moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod;
    PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone = GT_TRUE;

    /* indicate that network interface init is done and parameters for SW restart are saved */
    rc = prvCpssPpConfigDevDbNetIfInitSet(devNum, GT_FALSE);
    return rc;
}

/*******************************************************************************
* cpssDxChNetIfInit
*
* DESCRIPTION:
*       Initialize the network interface SDMA structures, Rx descriptors & buffers
*       and Tx descriptors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device to initialize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   1. The application must call only one of the following APIs per device:
*   cpssDxChNetIfMiiInit - for MII/RGMII Ethernet port networkIf initialization.
*   cpssDxChNetIfInit - for SDMA networkIf initialization.
*   In case more than one of the mentioned above API is called
*   GT_FAIL will be return.
*   2. In case CPSS_RX_BUFF_NO_ALLOC_E is set, the application must enable
*   RX queues after attaching the buffers. See: cpssDxChNetIfSdmaRxQueueEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfInit
(
    IN  GT_U8       devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChNetIfInit(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChNetIfRemove
*
* DESCRIPTION:
*       This function is called upon Hot removal of a device, inorder to release
*       all Network Interface related structures.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device that was removed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfRemove
(
    IN  GT_U8   devNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    rxSdmaRegVal; /* Rx SDMA Queues Register value*/
    GT_U32    portGroupId;  /* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    if(PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        /* Delete the Tx descriptors linked-list    */

        /* read Rx SDMA Queues Register. for each queue set disable */
        /* bit to 1, and enable bit to 0.                           */
        rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   sdmaRegs.rxQCmdReg,&rxSdmaRegVal);

        if(rc != GT_OK)
        {
            return rc;
        }

        rxSdmaRegVal |= 0x0000FF00;/* set 8 bits */
        rxSdmaRegVal &= 0xFFFFFF00;/* clear 8 bits */

        /* write Rx SDMA Queues Reg - Disable Rx SDMA Queues */
        rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    sdmaRegs.rxQCmdReg,rxSdmaRegVal);
        if(rc != GT_OK)
        {
            return rc;
        }
        }
    }
    else
    {
        return prvCpssGenNetIfMiiRemove(devNum);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfRemove
*
* DESCRIPTION:
*       This function is called upon Hot removal of a device, inorder to release
*       all Network Interface related structures.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device that was removed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfRemove
(
    IN  GT_U8   devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfRemove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChNetIfRemove(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


