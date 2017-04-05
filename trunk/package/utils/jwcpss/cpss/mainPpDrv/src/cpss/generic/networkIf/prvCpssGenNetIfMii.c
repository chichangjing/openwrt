/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenNetIfMii.c
*
* DESCRIPTION:
*       Implement CPSS generic MII/CPU Ethernet port network interface
*       management APIs
*
* FILE REVISION NUMBER:
*       $Revision: 21 $
*******************************************************************************/

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvPpIntGenMiiEmulateInterrupts.h>

/****************************************************************************
 * private data                                                             *
 ****************************************************************************/
#ifdef SHARED_MEMORY
extern int multiProcessAppDemo;
#endif

extern GT_STATUS extDrvEthPortRxInitWithoutInterruptConnection
(
    IN GT_U32           rxBufPoolSize,
    IN GT_U8_PTR        rxBufInfo_PTR,
    IN GT_U32           rxQNum,
    IN GT_U32           rxQbufPercentageArr[],
    IN GT_U32           rxBufSize,
    INOUT GT_U32        *numOfRxBufsPtr,
    IN GT_U32           headerOffset
);

/*******************************************************************************
* prvCpssGenNetIfMiiTxCtrlGet
*
* DESCRIPTION:
*       Return the ethernet port transmit control data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_NETIF_MII_TX_CTRL_STC *prvCpssGenNetIfMiiTxCtrlGet(GT_VOID)
{
    if (sysGenGlobalInfo.prvCpssGenNetIfMiiCtrl.txValid)
        return &sysGenGlobalInfo.prvCpssGenNetIfMiiTxCtrl;
    else
        return NULL;
}

/*******************************************************************************
* prvCpssGenNetIfMiiRxQCtrlGet
*
* DESCRIPTION:
*       Return the ethernet port recieve queue control data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *prvCpssGenNetIfMiiRxQCtrlGet(GT_VOID)
{
    if (sysGenGlobalInfo.prvCpssGenNetIfMiiCtrl.rxValid)
        return &sysGenGlobalInfo.prvCpssGenNetIfMiiRxQCtrl;
    else
        return NULL;
}

/*******************************************************************************
* prvCpssGenNetIfMiiDevNumGet
*
* DESCRIPTION:
*       Return the ethernet port device number
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 prvCpssGenNetIfMiiDevNumGet(GT_VOID)
{
    return sysGenGlobalInfo.prvMiiDevNum;
}

/*******************************************************************************
* prvCpssGenNetIfMiiTxModeSet
*
* DESCRIPTION:
*       Set the ethernet port tx mode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       txMode - tx mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise
*       GT_NOT_SUPPORTED
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiTxModeSet
(
    IN PRV_CPSS_GEN_NETIF_MII_TX_MODE_ENT txMode
)
{
    GT_STATUS rc;

    if (sysGenGlobalInfo.prvCpssGenNetIfMiiTxCtrl.txMode == txMode)
        return GT_OK;

    rc = cpssExtDrvEthPortTxModeSet(txMode);
    if (rc != GT_OK)
    {
        return rc;
    }
    sysGenGlobalInfo.prvCpssGenNetIfMiiTxCtrl.txMode = txMode;
    return GT_OK;
}


/*******************************************************************************
* prvCpssGenNetIfMiiRxPacketGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr- Num of buffs in packetBuffsArrPtr.
*
* OUTPUTS:
*       numOfBuffPtr       - Num of used buffs in packetBuffsArrPtr.
*       packetBuffsArrPtr  - (pointer to)The received packet buffers list.
*       buffLenArr         - List of buffer lengths for packetBuffsArrPtr.
*
* RETURNS:
*       GT_NO_MORE               - no more packets on the device/queue
*       GT_OK                    - packet got with no error.
*       GT_FAIL                  - failed to get the packet
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong devNum,portNum,queueIdx.
*       GT_NOT_INITIALIZED       - the library was not initialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRxPacketGet
(
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffsArrPtr[],
    OUT GT_U32                              buffLenArr[]
)
{
    GT_STATUS               rc;             /* Return Code                  */
    GT_U32                  i;              /* Iterator                     */
    PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *qCtrlPtr;      /* rx Q ctrl struct ptr         */
    PRV_CPSS_GEN_NETIF_RX_Q_MSG_STC  qMsg;           /* message from Queue           */
    PRV_CPSS_GEN_NETIF_SEG_STC   *currSegPtr;    /* segment ptr                  */
    PRV_CPSS_GEN_NETIF_SEG_STC   *tmpSegPtr;     /* temp segment ptr             */
    GT_U32 numMessages;
    GT_U32 messageSize = PRV_CPSS_NETIF_MII_RX_Q_MSG_LEN_CNS;

    qCtrlPtr = prvCpssGenNetIfMiiRxQCtrlGet();

    if (NULL == qCtrlPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(((GT_STATUS)GT_ERROR), LOG_ERROR_NO_MSG);
    }
    if (queueIdx >= PRV_CPSS_GEN_NETIF_MAX_RX_QUEUES_NUM_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get waiting message from Queue */
    rc = cpssOsMsgQNumMsgs(qCtrlPtr->intQueId[queueIdx],&numMessages);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (numMessages == 0)
    {
        return /* it's not error for log */ GT_NO_MORE;
    }

    /* get a message from queue */
    rc = cpssOsMsgQRecv(qCtrlPtr->intQueId[queueIdx],(GT_PTR)&qMsg,
                       &messageSize,CPSS_OS_MSGQ_WAIT_FOREVER);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (messageSize != PRV_CPSS_NETIF_MII_RX_Q_MSG_LEN_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* get data from queue message and validate the first segment pointer */
    if((currSegPtr = (PRV_CPSS_GEN_NETIF_SEG_STC*)(qMsg.seg)) == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* copy all buffer pointers to buffer list */
    for(i = 0; currSegPtr != NULL; i++)
    {
        packetBuffsArrPtr[i]  = currSegPtr->seg_PTR;
        buffLenArr[i]   = currSegPtr->len;
        tmpSegPtr       = currSegPtr;
        currSegPtr      = currSegPtr->nextSeg_PTR;
        if((rc = prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E, tmpSegPtr)) != GT_OK)
            return rc;
    }

    /* update the actual number of buffers in packet */
    *numOfBuffPtr = i;

    return GT_OK;
}

/*******************************************************************************
* ethPrtInitRx
*
* DESCRIPTION:
*       This function initializes the Core Rx module, by allocating the list of
*       Rx buffers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       rxQbufPercentageArr - the buffer percentage dispersal for all queues
*       rxBufSize           - Size of a single Rx buffer.
*       headerOffset        - The application required header offset to be
*                             kept before
*       rxBufBlockPtr       - (Pointer to) a block of memory to be used for
*                              allocating Rx packet data buffers.
*       rxBufBlockSize      - The raw size in byte of rxDataBufBlock.
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
static GT_STATUS ethPrtInitRx
(
    IN GT_U32    rxQbufPercentageArr[CPSS_MAX_RX_QUEUE_CNS],
    IN GT_U32    rxBufSize,
    IN GT_U32    headerOffset,
    IN GT_U8    *rxBufBlockPtr,
    IN GT_U32    rxBufBlockSize
)
{
    GT_U16              rxQueue;                /* Rx queue number          */
    char                semName[30];            /* semaphore name           */
    GT_U32              numOfRxBufs;            /* number of Rx buffers     */
    GT_STATUS           status;                 /* routine status           */
    GT_U32              segPoolSize;            /*  */
    GT_U8             * segPoolBlock_PTR;       /*  */
    CPSS_OS_MSGQ_ID     msgqId;

    /* validate the Rx buffer size */
    if (0 == rxBufSize)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* save space before user header for tagged packets indication */
    rxBufSize    += PRV_CPSS_NETIF_MII_TAGGED_PCKT_FLAG_LEN_CNS;
    headerOffset += PRV_CPSS_NETIF_MII_TAGGED_PCKT_FLAG_LEN_CNS;

    /* update the number of buffers in the pool */
    numOfRxBufs = rxBufBlockSize / rxBufSize;

    /* validate that we did not run out of buffers */
    if (0 == numOfRxBufs)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }


#ifdef SHARED_MEMORY
    if(multiProcessAppDemo == 0)
    {
        /* init the external driver Rx */
        status = cpssExtDrvEthPortRxInit(rxBufBlockSize, rxBufBlockPtr, CPSS_MAX_RX_QUEUE_CNS,
         rxQbufPercentageArr, rxBufSize, &numOfRxBufs,
        headerOffset);
        /* temporary solution, similar to previous version */
        if ((GT_OK != status) && (GT_NOT_SUPPORTED != status))
        {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* init the external driver Rx */
        status = extDrvEthPortRxInitWithoutInterruptConnection(rxBufBlockSize, rxBufBlockPtr,
                 CPSS_MAX_RX_QUEUE_CNS, rxQbufPercentageArr, rxBufSize, &numOfRxBufs,
                 headerOffset);
        /* temporary solution, similar to previous version */
        if ((GT_OK != status) && (GT_NOT_SUPPORTED != status))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

    }
#else
    /* init the external driver Rx */
    status = cpssExtDrvEthPortRxInit(rxBufBlockSize, rxBufBlockPtr, CPSS_MAX_RX_QUEUE_CNS,
                                 rxQbufPercentageArr, rxBufSize, &numOfRxBufs,
                                 headerOffset);
    /* temporary solution, similar to previous version */
    if ((GT_OK != status) && (GT_NOT_SUPPORTED != status))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
#endif

    /* create segment pool that is used by the Rx ISR and Handler */
    segPoolSize         = numOfRxBufs * sizeof(PRV_CPSS_GEN_NETIF_SEG_STC);
    segPoolBlock_PTR    = cpssOsMalloc(segPoolSize);

    if (NULL == segPoolBlock_PTR)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    status = prvCpssGenNetIfMiiCreatePool(PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E, sizeof(PRV_CPSS_GEN_NETIF_SEG_STC),
                                     &numOfRxBufs, segPoolBlock_PTR, segPoolSize,
                                     NULL);
    if (GT_OK != status)
    {
        return status;
    }

    /* create the queues for the Rx packets (one per int queue) */
    for (rxQueue = 0; rxQueue < PRV_CPSS_GEN_NETIF_MAX_RX_QUEUES_NUM_CNS; rxQueue++)
    {
        cpssOsSprintf(semName,"rxQ-%d", rxQueue);

        status = cpssOsMsgQCreate(semName,numOfRxBufs,PRV_CPSS_NETIF_MII_RX_Q_MSG_LEN_CNS,&msgqId);
        if (status != GT_OK)
        {
            return status;
        }
        sysGenGlobalInfo.prvCpssGenNetIfMiiRxQCtrl.intQueId[rxQueue] = msgqId;
    }

    /* set the header size */
    sysGenGlobalInfo.prvCpssGenNetIfMiiRxQCtrl.headerOffset = headerOffset;
    sysGenGlobalInfo.prvCpssGenNetIfMiiRxQCtrl.auQueId      = NULL;
    sysGenGlobalInfo.prvCpssGenNetIfMiiRxQCtrl.numOfRxBufs  = numOfRxBufs;

    /* bind External Driver routine */
    sysGenGlobalInfo.prvCpssGenNetIfMiiRxQCtrl.packetFreeFunc_PTR = cpssExtDrvEthRxPacketFree;

    sysGenGlobalInfo.prvCpssGenNetIfMiiCtrl.rxValid = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* ethPrtInitTx
*
* DESCRIPTION:
*       This function initializes the Core Tx module, by creating the
*       tx Header pool and queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       numOfTxDesc      - number of TX descriptors
*       txHdrBufBlockPtr - ptr to the Tx header buffer block.
*       txHdrBufBlockSize- Tx header buffer block size.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ethPrtInitTx
(
    IN GT_U8     devNum,
    IN GT_U32       numOfTxDesc,
    IN GT_U8        *txHdrBufBlockPtr,
    IN GT_U32       txHdrBufBlockSize
)
{
    GT_STATUS       status;
    GT_U32          numOfTxHdrBufs;

    /* tx buffers are taken from user allocated Tx decs Block. User MUST */
    /* allocate this area and set the tx desc block size accordingly.    */
    if (0 == txHdrBufBlockSize || NULL == txHdrBufBlockPtr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    numOfTxHdrBufs = txHdrBufBlockSize / CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS;

    /* create pool with caller`s allocated memory */
    status = prvCpssGenNetIfMiiCreatePool(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E,
                                         CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS,
                                         &numOfTxHdrBufs,
                                         (GT_U8*)txHdrBufBlockPtr,
                                         txHdrBufBlockSize,
                                         NULL);
    if (GT_OK != status)
    {
        return status;
    }

    /* init the external driver Tx */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        status = cpssExtDrvEthPortTxInit(numOfTxDesc);
        if (GT_OK != status)
        {
            return status;
        }
    }

    /* init the external driver Tx mode to asynch */
    sysGenGlobalInfo.prvCpssGenNetIfMiiTxCtrl.txMode = PRV_CPSS_GEN_NETIF_MII_TX_MODE_ASYNCH_E;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        status = cpssExtDrvEthPortTxModeSet(CPSS_EXTDRV_ETH_TX_MODE_ASYNC_E);

        /* temporary solution, similar to previous version */
        if ((GT_OK != status) && (GT_NOT_SUPPORTED != status))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    sysGenGlobalInfo.prvCpssGenNetIfMiiCtrl.txValid = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenNetIfMiiInitNetIfDev
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx buffers and Tx header
*       buffers (For a single device).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device number.
*       numOfTxDesc - number of requested TX descriptors.
*       txInternalBufBlockPtr  - (Pointer to) a block of host memory to be used
*                                for internal TX buffers.
*       txInternalBufBlockSize - The raw size in bytes of txInternalBufBlock memory.
*       bufferPercentageArr    - A table (entry per queue) that describes the buffer
*                         dispersal among all Rx queues. (values 0..100)
*       rxBufSize       - The size of the Rx data buffer.
*       headerOffset    - packet header offset size
*       rxBufBlockPtr   - (Pointer to) a block of memory to be used for
*                        allocating Rx packet data buffers.
*       rxBufBlockSize  - The raw size in byte of rxDataBufBlock.
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
GT_STATUS prvCpssGenNetIfMiiInitNetIfDev
(
    IN GT_U8     devNum,
    IN GT_U32    numOfTxDesc,
    IN GT_U8    *txInternalBufBlockPtr,
    IN GT_U32    txInternalBufBlockSize,
    IN GT_U32    bufferPercentageArr[CPSS_MAX_RX_QUEUE_CNS],
    IN GT_U32    rxBufSize,
    IN GT_U32    headerOffset,
    IN GT_U8    *rxBufBlockPtr,
    IN GT_U32    rxBufBlockSize
)
{
    GT_STATUS           retVal;

    /* currently we support only one CPU MII port per system */
    if (sysGenGlobalInfo.prvMiiDevNum != PRV_CPSS_GEN_NETIF_MII_DEV_NOT_SET)
    {
        if (sysGenGlobalInfo.prvMiiDevNum == devNum)
        {
            return GT_OK;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    /* Init RX */
    retVal = ethPrtInitRx(bufferPercentageArr,rxBufSize,headerOffset,rxBufBlockPtr,rxBufBlockSize);

    if (GT_OK != retVal)
    {
        return retVal;
    }

    /* Init TX */
    retVal = ethPrtInitTx(devNum, numOfTxDesc,txInternalBufBlockPtr,txInternalBufBlockSize);

    if (GT_OK != retVal)
    {
        return retVal;
    }

    /* bind the RX callback routine */
    retVal = cpssExtDrvEthInputHookAdd(interruptMiiRxSR);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    retVal = cpssExtDrvEthRawSocketRxHookAdd(rawSocketRx);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    /* bind the TX complete callback routine */
    retVal = cpssExtDrvEthTxCompleteHookAdd(interruptMiiTxEndSR);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    /* enable the ethernet port */
    retVal = cpssExtDrvEthPortEnable();
    if (GT_OK != retVal)
    {
        return retVal;
    }

    sysGenGlobalInfo.prvMiiDevNum = devNum;

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenNetIfMiiRemove
*
* DESCRIPTION:
*       This function is called upon Hot removal of a device, inorder to release
*       all Network Interface related structures.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
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
GT_STATUS prvCpssGenNetIfMiiRemove
(
    IN  GT_U8   devNum
)
{
    /* to avoid warning */
    devNum = devNum;

    /* disable the ethernet port */
    return cpssExtDrvEthPortDisable();
}

/*******************************************************************************
* prvCpssGenNetIfMiiRxBufFree
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number through which these buffers where
*                     received.
*       rxQueue     - The Rx queue number through which these buffers where
*                     received.
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
)
{
    PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *qCtrl_PTR;
    GT_U32                  firstBuffOffset;/* offset to the actual start of
                                                   the FIRST buffer */

    /* to avoid warning */
    devNum = devNum;

    /* get the Rx control block */
    qCtrl_PTR = prvCpssGenNetIfMiiRxQCtrlGet();

    if (NULL == qCtrl_PTR)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if(rxBuffList == NULL || buffListLen == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* get the offset before the start of the first buffer pointer to the
       actual start of the first buffer */
    firstBuffOffset = PRV_CPSS_NETIF_MII_RX_ORIG_PCKT_TAG_BYTES_GET(rxBuffList[0], qCtrl_PTR->headerOffset);

    /* update the buffer pointer to the actual start of the buffer */
    rxBuffList[0] -= (firstBuffOffset);

    qCtrl_PTR->packetFreeFunc_PTR(rxBuffList, buffListLen, rxQueue);

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenNetIfMiiTxBufferQueueGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*       hndl        - Handle got from gtEventBind.
*
* OUTPUTS:
*       devPtr      - The device the packet was transmitted from.
*       cookiePtr   - The user cookie handed on the transmit request.
*       queuePtr    - The queue from which this packet was transmitted
*       statusPtr   - GT_OK if packet transmission was successful, GT_FAIL on
*                     packet reject.
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - the CPSS not handling the ISR for the device
*                                  so function must not be called
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiTxBufferQueueGet
(
    IN  GT_UINTPTR          hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
)
{
    PRV_CPSS_DRV_EVENT_HNDL_STC *evHndlPtr;  /* The event handle pointer        */
    PRV_CPSS_TX_BUF_QUEUE_FIFO_STC          *fifoPtr;    /* The Handle Tx-End FIFO          */
    GT_U32                  intKey;      /* The interrupt key               */
    PRV_CPSS_SW_TX_FREE_DATA_STC    *fifoElemPtr;/* FIFO element                    */

    evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl;

    /* validate handle */
    if (NULL == evHndlPtr || NULL == evHndlPtr->extDataPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* set alias FIFO pointer */
    fifoPtr = (PRV_CPSS_TX_BUF_QUEUE_FIFO_STC*)evHndlPtr->extDataPtr;

    /* validate FIFO is not empty */
    if (NULL == fifoPtr->headPtr)
    {
        return /* it's not error for log */ GT_NO_MORE;
    }

    /* lock section from ISR preemption */
    intKey = 0;
    PRV_CPSS_LOCK_BY_MUTEX;
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, (GT_32*)&intKey);

    /* set the packet header from FIFO first element */
    fifoElemPtr = fifoPtr->headPtr;

    /* remove the first element from FIFO */
    fifoPtr->headPtr = fifoElemPtr->nextPtr;

    if (NULL == fifoPtr->headPtr)
    {
        /* last element in FIFO, update last pointer */
        fifoPtr->tailPtr = NULL;
    }

    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, (GT_32*)&intKey);
    PRV_CPSS_UNLOCK_BY_MUTEX;

    /* update other output parameters */
    *devPtr     = fifoElemPtr->devNum;
    *queuePtr   = fifoElemPtr->queueNum;
    *cookiePtr  = fifoElemPtr->userData;
    *statusPtr  = GT_OK;

    /* return the packet header buffer to pool */
    if (prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E,
                                 (GT_U8*)fifoElemPtr) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenNetIfMiiRxQueueCountersGet
*
* DESCRIPTION:
*       This routine returns the caller rx packets and octets counters
*       for given queue.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*       rxQueue     - rx queue.
*
* OUTPUTS:
*       rxInPktsPtr      - Packets received on this queue.
*       rxInOctetsPtr    - Octets received on this queue.
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRxQueueCountersGet
(
    IN  GT_U32               rxQueue,
    OUT GT_U32               *rxInPktsPtr,
    OUT GT_U32               *rxInOctetsPtr
)
{
    PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *qCtrl_PTR;        /* queue control struct     */
    qCtrl_PTR = prvCpssGenNetIfMiiRxQCtrlGet();

    if (NULL == qCtrl_PTR)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    *rxInOctetsPtr = qCtrl_PTR->rxInOctetsCounter[rxQueue];
    *rxInPktsPtr = qCtrl_PTR->rxInPktsCounter[rxQueue];

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenNetIfMiiRxQueueCountersClear
*
* DESCRIPTION:
*       This routine claers rx packets and octets counters for given queue.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*       rxQueue     - rx queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRxQueueCountersClear
(
    IN  GT_U32               rxQueue
)
{
    PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *qCtrl_PTR;        /* queue control struct     */
    qCtrl_PTR = prvCpssGenNetIfMiiRxQCtrlGet();

    if (NULL == qCtrl_PTR)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    qCtrl_PTR->rxInOctetsCounter[rxQueue] = 0;
    qCtrl_PTR->rxInPktsCounter[rxQueue] = 0;

    return GT_OK;
}
