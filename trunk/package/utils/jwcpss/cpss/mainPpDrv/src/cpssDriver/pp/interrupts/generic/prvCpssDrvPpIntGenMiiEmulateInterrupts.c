/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvPpIntGenMiiEmulateInterrupts.c
*
* DESCRIPTION:
*       NOTE : this is for "Emulations of events" that the CPSS is unaware of ...
*
*       File intend to cover the "Emulations of events" that the ASIC is not
*       doing by itself.
*       For example:
*       in ASIC that has "CPU Ethernet port" there are no "Rx interrupts" from
*       ASIC , but this file get the event from the BSP(external driver).
*       another example is the "TX buffer sent" in this ASIC that has
*       "CPU Ethernet port"
*
*       This file includes callback functions bound to the external driver , to
*       be called on Rx packet and on Tx buffer sent
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif 

/*******************************************************************************
* internal definitions
*******************************************************************************/

/* #define DRV_INTERRUPTS_DBG */

#ifdef DRV_INTERRUPTS_DBG
#define DBG_INFO(x)     cpssOsPrintf x
#else
#define DBG_INFO(x)
#endif

/* statistical counters for Ethernet port debugging */
static PRV_CPSS_GEN_NETIF_MII_STAT_STC ethPortDbg[PRV_CPSS_GEN_NETIF_MAX_RX_QUEUES_NUM_CNS] = {{0}};

static GT_STATUS unifiedEventTreat
(
    GT_U8   devNum,
    GT_U32 uEvent
)
{
    GT_U32  i;
    PRV_CPSS_DRV_EV_REQ_NODE_STC    *evNodesPool;   /* Event node pool for the device     */
    GT_BOOL             intMasked = GT_FALSE;      /* Int received while masked          */

    evNodesPool = prvCpssDrvPpConfig[devNum]->intCtrl.
                    portGroupInfo[CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS].intNodesPool;

    for(i = 0; (i < prvCpssDrvPpConfig[devNum]->intCtrl.numOfIntBits) 
                                && (evNodesPool[i].uniEvCause != uEvent); i++);

    /* If a callBack routine is registered for the event    */
    /* invoke it.                                           */
    if (evNodesPool[i].intCbFuncPtr != NULL)
    {
        evNodesPool[i].intCbFuncPtr(devNum, i);
    }

    /* This is a regular event */
    if (prvCpssDrvEvReqQUserHndlGet(evNodesPool[i].uniEvCause) != NULL)
    {
        /* insert the event into queue */
/* TBD - need to implement mii rx interrupt mask treatment
        intMaskBit = 1 << (i & 0x1f);
        intMaskBits |= intMaskBit;

        if (*maskShdwValPtr & intMaskBit)
        {
            intMasked = GT_FALSE;
            newInt = 1;
        }
        else
        {
            intMasked = GT_TRUE;
        }
*/
        DBG_INFO(("unifiedEventTreat:call prvCpssDrvEvReqQInsert\n"));
        return prvCpssDrvEvReqQInsert(evNodesPool, i, intMasked);
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* interruptEthPortRxSR
*
* DESCRIPTION:
*       This function is invoked when a packet has been received from the
*       Ethernet port driver.
*
* INPUT:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segment length.
*       numOfSegments   - The number of segment in segment list.
*       rxQueue         - the received queue number
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS interruptMiiRxSR
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           rxQueue
)
{
    GT_STATUS                       rc;
    PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *qCtrl_PTR;        /* queue control struct     */
    GT_32                           intKey = 0;         /* context of interrupts    */
    PRV_CPSS_GEN_NETIF_RX_Q_MSG_STC qMsg;               /* the message sent to Q    */
    GT_U32                          seg;                /* iterator                 */
    PRV_CPSS_GEN_NETIF_SEG_STC      *currSeg_PTR;       /* segment ptr              */
    PRV_CPSS_GEN_NETIF_SEG_STC      *newSeg_PTR;        /* segment ptr              */
    CPSS_OS_MSGQ_ID                 queueId;
    GT_U32                          packetLength = 0;   /* packet length            */

    GT_U32 devNum = prvCpssGenNetIfMiiDevNumGet();
    if (devNum == PRV_CPSS_GEN_NETIF_MII_DEV_NOT_SET)
    {
        return GT_OK;
    }
    qCtrl_PTR = prvCpssGenNetIfMiiRxQCtrlGet();

    if (NULL == qCtrl_PTR)
    {
        return GT_OK;
    }

    /* validate the number of segments received */
    if (0 == numOfSegments || PRV_CPSS_GEN_NETIF_MAX_GT_BUF_SEG_NUM_CNS < numOfSegments)
    {
        qCtrl_PTR->packetFreeFunc_PTR(segmentList, numOfSegments, rxQueue);
        return GT_OK;
    }

    /* validate the Rx queue number */
    if (rxQueue >= NUM_OF_RX_QUEUES)
    {
        qCtrl_PTR->packetFreeFunc_PTR(segmentList, numOfSegments, rxQueue);
        return GT_OK;
    }

    ethPortDbg[rxQueue].pcktIn++;

    /* validate minimal packet size */
    if (segmentLen[0] < PRV_CPSS_GEN_NETIF_MII_BUFF_MIN_SIZE_CNS)
    {
        qCtrl_PTR->packetFreeFunc_PTR(segmentList, numOfSegments, rxQueue);
        ethPortDbg[rxQueue].pcktLen++;
        return GT_OK;
    }

    /* get the first segment node from pool */
    newSeg_PTR = prvCpssGenNetIfMiiPoolGetBuf(PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E);

    if (NULL == newSeg_PTR)
    {
        qCtrl_PTR->packetFreeFunc_PTR(segmentList, numOfSegments, rxQueue);
        ethPortDbg[rxQueue].noQbuffer++;

        return GT_OK;
    }

    /* save the first segment, it is transmitted     */
    /* through the queue to the Rx handler routine  */
    qMsg.seg = (GT_UINTPTR)newSeg_PTR;

    packetLength = segmentLen[0];
    newSeg_PTR->len     = segmentLen[0];
    newSeg_PTR->seg_PTR = segmentList[0];

    currSeg_PTR = newSeg_PTR;

    /* get buffers from pool for all remaining segments */
    for (seg = 1; seg < numOfSegments; seg++)
    {
        /* get a segment node from pool */
        newSeg_PTR = prvCpssGenNetIfMiiPoolGetBuf(PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E);
        /* buffer allocation failed on on certain segment
           already allocated buffers has to be released */
        if (NULL == newSeg_PTR)
        {
            /* find start of the list */
            newSeg_PTR = (PRV_CPSS_GEN_NETIF_SEG_STC *)qMsg.seg;
            /* run on the list till failed segment */
            while (newSeg_PTR != NULL)
            {
                currSeg_PTR = newSeg_PTR;
                newSeg_PTR = newSeg_PTR->nextSeg_PTR;
                prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E, currSeg_PTR);
            }

            qCtrl_PTR->packetFreeFunc_PTR(segmentList, numOfSegments, rxQueue);
            /* update the number of free buffers according to loop iterator */
            ethPortDbg[rxQueue].noQbuffer++ ;

            return GT_OK;
        }
        packetLength += segmentLen[seg];
        newSeg_PTR->len     = segmentLen[seg];
        newSeg_PTR->seg_PTR = segmentList[seg];

        currSeg_PTR->nextSeg_PTR = newSeg_PTR;

        currSeg_PTR = newSeg_PTR;
    }

    /* terminate the linked list of segments */
    currSeg_PTR->nextSeg_PTR = NULL;

    /* set the Queue Rx queue number */
    qMsg.rxQueue = rxQueue;
    queueId = qCtrl_PTR->intQueId[(GT_U8)rxQueue];

    /* send the packet through Queue */
    if (cpssOsMsgQSend(queueId,(GT_PTR)&qMsg,
                       PRV_CPSS_NETIF_MII_RX_Q_MSG_LEN_CNS,
                       CPSS_OS_MSGQ_WAIT_FOREVER) != GT_OK)
    {
        /* queue send failed, clean up all allocated buffers */
        ethPortDbg[rxQueue].msgSend++;

        /* Free the buffer */
        newSeg_PTR = (PRV_CPSS_GEN_NETIF_SEG_STC*)qMsg.seg;

        while (newSeg_PTR != NULL)
        {
            currSeg_PTR = newSeg_PTR;
            newSeg_PTR = newSeg_PTR->nextSeg_PTR;
            prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E, currSeg_PTR);
        }

        qCtrl_PTR->packetFreeFunc_PTR(segmentList, numOfSegments, rxQueue);

        return GT_OK;
    }

    PRV_CPSS_LOCK_BY_MUTEX;
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

    rc = unifiedEventTreat((GT_U8)devNum, CPSS_PP_RX_BUFFER_QUEUE0_E + rxQueue);
        
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    PRV_CPSS_UNLOCK_BY_MUTEX;

    if (rc != GT_OK)
    {
        qCtrl_PTR->packetFreeFunc_PTR(segmentList, numOfSegments, rxQueue);
        return GT_OK;
    }
    qCtrl_PTR->rxInOctetsCounter[rxQueue] += packetLength;
    qCtrl_PTR->rxInPktsCounter[rxQueue]++;
    
    ethPortDbg[rxQueue].pcktTreated++;

    return GT_OK;
}

/*******************************************************************************
* rawSocketRx
*
* DESCRIPTION:
*       This function is invoked when a packet has been received from the
*       raw packts handler task
*
* INPUT:
*       packet          - A pointer to the entire packet
*       packetLen       - The length of the packet.
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS rawSocketRx
(
    IN GT_U8_PTR        packet,
    IN GT_U32           packetLen,
    IN GT_U32           rxQueue
)
{
    GT_STATUS                       rc;
    PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *qCtrl_PTR;        /* queue control struct     */
    GT_32                           intKey = 0;         /* context of interrupts    */
    PRV_CPSS_GEN_NETIF_RX_Q_MSG_STC qMsg;               /* the message sent to Q    */
    PRV_CPSS_GEN_NETIF_SEG_STC      *currSeg_PTR;       /* segment ptr              */
    PRV_CPSS_GEN_NETIF_SEG_STC      *newSeg_PTR;        /* segment ptr              */
    CPSS_OS_MSGQ_ID                 queueId;
    GT_U32                          packetLength = 0;   /* packet length            */

    GT_U32 devNum = prvCpssGenNetIfMiiDevNumGet();
    if (devNum == PRV_CPSS_GEN_NETIF_MII_DEV_NOT_SET)
    {
        return GT_OK;
    }

    qCtrl_PTR = prvCpssGenNetIfMiiRxQCtrlGet();

    if (NULL == qCtrl_PTR)
    {
        return GT_OK;
    }

    /* get the first segment node from pool */
    newSeg_PTR = prvCpssGenNetIfMiiPoolGetBuf(PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E);

    if (NULL == newSeg_PTR)
        return GT_OK;

    qMsg.seg = (GT_UINTPTR)newSeg_PTR;

    packetLength = packetLen;
    newSeg_PTR->len     = packetLen;
    newSeg_PTR->seg_PTR = packet;

    currSeg_PTR = newSeg_PTR;

    
    /* terminate the linked list of segments */
    currSeg_PTR->nextSeg_PTR = NULL;

    /* set the Queue Rx queue number */
    qMsg.rxQueue = rxQueue;
    queueId = qCtrl_PTR->intQueId[(GT_U8)rxQueue];

    /* send the packet through Queue */
    if (cpssOsMsgQSend(queueId,(GT_PTR)&qMsg,
                       PRV_CPSS_NETIF_MII_RX_Q_MSG_LEN_CNS,
                       CPSS_OS_MSGQ_WAIT_FOREVER) != GT_OK)
    {
        /* queue send failed, clean up all allocated buffers */
        ethPortDbg[rxQueue].msgSend++;

        /* Free the buffer */
        newSeg_PTR = (PRV_CPSS_GEN_NETIF_SEG_STC*)qMsg.seg;

        while (newSeg_PTR != NULL)
        {
            currSeg_PTR = newSeg_PTR;
            newSeg_PTR = newSeg_PTR->nextSeg_PTR;
            prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E, currSeg_PTR);
        }
        return GT_OK;
    }

    PRV_CPSS_LOCK_BY_MUTEX;
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

    rc = unifiedEventTreat((GT_U8)devNum, CPSS_PP_RX_BUFFER_QUEUE0_E + rxQueue);
        
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    PRV_CPSS_UNLOCK_BY_MUTEX;

    if (rc != GT_OK)
    {
        return GT_OK;
    }
    qCtrl_PTR->rxInOctetsCounter[rxQueue] += packetLength;
    qCtrl_PTR->rxInPktsCounter[rxQueue]++;    

    return GT_OK;
}

/*******************************************************************************
* interruptEthPortTxEndSR
*
* DESCRIPTION:
*       This function is invoked when a packet transmit has ended from the
*       Ethernet port driver.
*
* INPUT:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS interruptMiiTxEndSR
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments
)
{
    GT_32                      intKey;      /* context of interrupts       */
    PRV_CPSS_NETIF_MII_TX_CTRL_STC *txCtrl_PTR;  /* Tx control struct           */
    GT_U8                       txDevice;    /* the source device number    */
    GT_U8                      *hdr_PTR;     /* header buffer ptr           */
    GT_U8                       txQueue;/* the queue the packet was send on */
    PRV_CPSS_TX_BUF_QUEUE_FIFO_STC          *fifoPtr;     /* TxEnd FIFO pointer         */
    PRV_CPSS_SW_TX_FREE_DATA_STC    *txEndFifoPtr;/* TxEnd FIFO element ptr     */
    GT_UINTPTR                  userData;     /* user data from packet      */
#if __WORDSIZE == 64
    typedef struct {
        GT_UINTPTR  cookie;
        GT_UINTPTR  evReqHndl;
    } COOKIE_HANDLER_STC;
    COOKIE_HANDLER_STC *cookie_handler;
#else /* !(__WORDSIZE == 64) */
    GT_U32                  evHndl;       /* event handle from packet   */
#endif
    PRV_CPSS_DRV_EVENT_HNDL_STC     *evHndlPtr;    /* ptr to event handle        */

    numOfSegments = numOfSegments;

    txCtrl_PTR = prvCpssGenNetIfMiiTxCtrlGet();

    if (NULL == txCtrl_PTR)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* get the device number from the header segment (segment 0) */
    hdr_PTR = segmentList[0] - PRV_CPSS_GEN_NETIF_MII_HDR_MAC_OFFSET_CNS;
    txDevice  = hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_DEVICE_OFFSET_CNS]; /* devNum is GT_U8 */
    txQueue = hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_QUEUE_OFFSET_CNS]; /* queue is GT_U8 */
#if __WORDSIZE == 64
    CPSS_TBD_BOOKMARK
    /* Redesign: pointer to structure is stored instead of cookie + evReqHndl */
    cpssOsMemCpy(&cookie_handler,
        hdr_PTR+PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE_OFFSET_CNS, sizeof(cookie_handler));
    if (cookie_handler)
    {
        userData = cookie_handler->cookie;
        evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)cookie_handler->evReqHndl;
        cpssOsFree(cookie_handler);
    }
    else
    {
        userData = 0;
        evHndlPtr = NULL;
    }
#endif

    /* validate the device number */
    if (txDevice >= PRV_CPSS_MAX_PP_DEVICES_CNS ||
        PRV_CPSS_IS_DEV_EXISTS_MAC(txDevice) == GT_FALSE)
    {
        prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E, (GT_VOID *)hdr_PTR);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* if in sync mode return header buffer */
    if (txCtrl_PTR->txMode == PRV_CPSS_GEN_NETIF_MII_TX_MODE_SYNCH_E)
    {
        prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E, (GT_VOID *)hdr_PTR);
        return GT_OK;
    }

#if __WORDSIZE == 64
    /* userData & evHndlPtr already copied from header */
#else /* !(__WORDSIZE == 64) */
    /* extract user data (cookie) from packet header */
    userData  = (GT_U32)hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE_OFFSET_CNS]   << 24;
    userData |= (GT_U32)hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE_OFFSET_CNS+1] << 16;
    userData |= (GT_U32)hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE_OFFSET_CNS+2] <<  8;
    userData |= (GT_U32)hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE_OFFSET_CNS+3];

    /* extract the user handle from tx packet */
    evHndl  = (GT_U32)hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_HNDL_OFFSET_CNS]   << 24;
    evHndl |= (GT_U32)hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_HNDL_OFFSET_CNS+1] << 16;
    evHndl |= (GT_U32)hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_HNDL_OFFSET_CNS+2] <<  8;
    evHndl |= (GT_U32)hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_HNDL_OFFSET_CNS+3];
    evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)evHndl;
#endif

    /* set the FIFO element data */
    txEndFifoPtr = (PRV_CPSS_SW_TX_FREE_DATA_STC*)hdr_PTR;
    txEndFifoPtr->devNum   = txDevice;
    txEndFifoPtr->queueNum = txQueue;
    txEndFifoPtr->userData = (GT_PTR)userData;
    txEndFifoPtr->nextPtr  = NULL;

    /* insert the new Tx end node into FIFO */

    /* lock to prevent preemption on FIFO */
    intKey = 0;
    PRV_CPSS_LOCK_BY_MUTEX;
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

    fifoPtr = (PRV_CPSS_TX_BUF_QUEUE_FIFO_STC*)evHndlPtr->extDataPtr;

    if (NULL == fifoPtr->tailPtr)
    {
        fifoPtr->tailPtr = (PRV_CPSS_SW_TX_FREE_DATA_STC*)hdr_PTR;
        fifoPtr->headPtr = (PRV_CPSS_SW_TX_FREE_DATA_STC*)hdr_PTR;
    }
    else
    {
        fifoPtr->tailPtr->nextPtr = (PRV_CPSS_SW_TX_FREE_DATA_STC*)hdr_PTR;
        fifoPtr->tailPtr          = (PRV_CPSS_SW_TX_FREE_DATA_STC*)hdr_PTR;
    }

    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    PRV_CPSS_UNLOCK_BY_MUTEX;

    prvCpssDrvEvReqNotify(evHndlPtr);

    return GT_OK;
}

/*******************************************************************************
* miiDbgPortStat
*
* DESCRIPTION:
*       debug function , that print the counters of the packets arrived to the
*       "CPU Ethernet port manager"
*
* INPUT:
*       none
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS miiDbgPortStat(void)
{
    GT_U16 i;

    cpssOsPrintf("Ethernet port statistics\n");
    cpssOsPrintf("Q in      treated tagCmnd cpuCode queSend noBuff  Length  IntType DevNum\n");

    for (i = 0; i < NUM_OF_RX_QUEUES; i++)
    {
        cpssOsPrintf("%-1d %-7X %-7X %-7X %-7X %-7X %-7X %-7X %-7X %-7X\n",
                        i, ethPortDbg[i].pcktIn, ethPortDbg[i].pcktTreated,
                        ethPortDbg[i].command, ethPortDbg[i].cpuCode,
                        ethPortDbg[i].msgSend, ethPortDbg[i].noQbuffer,
                        ethPortDbg[i].pcktLen, ethPortDbg[i].intType,
                        ethPortDbg[i].device);
    }

    return GT_OK;
}


