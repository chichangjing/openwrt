/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* transmit_task.c
*
* DESCRIPTION:
*       TX task
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <gtOs/gtOsTimer.h>
#include <gtOs/gtOsMsgQ.h>
#include <gtOs/gtOsTask.h>
#include <mainLuaWrapper/wraplNetIf.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
    #include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
    #include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif /* CHX_FAMILY */

/************* Defines ********************************************************/
#define TRANSMIT_TASK_QUEUE_LEN 10
/* the timeout to send transmit status */
#define TRANSMIT_TASK_REPLY_TIMEOUT 50

/************* Data ******************************************************/
static GT_TASK      transmitTaskId;
static GT_MSGQ_ID   transmitTaskQueueId;
static int          transmitTaskCreated = 0;

/* use this etherType to recognize 'Tagged packes' that send
  from the traffic generator to the PP */
#ifdef CHX_FAMILY
static GT_U16 etherTypeForVlanTag = 0x8100;
#endif

/*******************************************************************************
* send_packet_DxCh
*
* DESCRIPTION:
*       Transmit the traffic to the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       bufferPtr        - (pointer to) the buffer to send
*       bufferLength     - buffer length (include CRC bytes)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The packet will ingress to the PP after 'loopback' and will act as
*       it was received from a traffic generator connected to the port
*
*******************************************************************************/
static GT_STATUS send_packet_DxCh
(
    IN CPSS_INTERFACE_INFO_STC          *portInterfacePtr,
    IN GT_U8                            *bufferPtr,
    IN GT_U32                            bufferLength
)
{
#ifdef CHX_FAMILY
    GT_U8   devNum    = 0;
    GT_U32  numOfBufs = 0;
    GT_U32  vpt       = 0;
    GT_U32  cfiBit    = 0;
    GT_U32  vid       = 0;
    GT_BOOL packetIsTagged = GT_FALSE;

    GT_STATUS   rc;
    CPSS_DXCH_NET_TX_PARAMS_STC       dxChPcktParams;
    CPSS_DXCH_NET_DSA_FROM_CPU_STC    *fromCpuPtr    = NULL;

    GT_U8   *buffList[1];
    GT_U32  buffLenList[1];

    cpssOsMemSet(&dxChPcktParams, 0, sizeof(dxChPcktParams));


    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    buffList[0]    = bufferPtr;
    buffLenList[0] = bufferLength;

    numOfBufs = 1;
    devNum    = (GT_U8)portInterfacePtr->hwDevNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if ((bufferPtr[12] == (GT_U8)(etherTypeForVlanTag >> 8)) &&
        (bufferPtr[13] == (GT_U8)(etherTypeForVlanTag & 0xFF)))
    {
        packetIsTagged = GT_TRUE;
        vpt    = (GT_U8) (bufferPtr[14] >> 5);
        cfiBit = (GT_U8) ((bufferPtr[14] >> 4) & 1);
        vid    = ((bufferPtr[14] & 0xF) << 8) | bufferPtr[15];
    }
    else
    {
        packetIsTagged = GT_FALSE;
        vpt    = 0;
        cfiBit = 0;
        vid    = 4094;
    }

    dxChPcktParams.cookie = NULL;
    dxChPcktParams.packetIsTagged = packetIsTagged;

    dxChPcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    dxChPcktParams.sdmaInfo.txQueue   = 7;
    dxChPcktParams.sdmaInfo.evReqHndl = 0;
    dxChPcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

    dxChPcktParams.dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
    dxChPcktParams.dsaParam.commonParams.vpt            = (GT_U8) vpt;
    dxChPcktParams.dsaParam.commonParams.cfiBit         = (GT_U8) cfiBit;
    dxChPcktParams.dsaParam.commonParams.vid            = (GT_U16) vid;
    dxChPcktParams.dsaParam.commonParams.dropOnSource   = GT_FALSE;
    dxChPcktParams.dsaParam.commonParams.packetIsLooped = GT_FALSE;

    dxChPcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;

    fromCpuPtr = &dxChPcktParams.dsaParam.dsaInfo.fromCpu;

    fromCpuPtr->dstInterface        = *portInterfacePtr;
    if( CPSS_INTERFACE_PORT_E == portInterfacePtr->type )
    {
        CPSS_API_CALL_LOCK();
        rc = cpssDxChCfgHwDevNumGet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                           (GT_HW_DEV_NUM   *)&(fromCpuPtr->dstInterface.devPort.hwDevNum));
        CPSS_API_CALL_UNLOCK();
        if(GT_OK != rc)
            return rc;
    }
    fromCpuPtr->tc                  = 7;
    fromCpuPtr->dp                  = CPSS_DP_GREEN_E;
    fromCpuPtr->egrFilterEn         = GT_FALSE;
    fromCpuPtr->cascadeControl      = GT_TRUE;
    fromCpuPtr->egrFilterRegistered = GT_FALSE;
    fromCpuPtr->srcId               = 0;
    CPSS_API_CALL_LOCK();
    rc = cpssDxChCfgHwDevNumGet(devNum, &(fromCpuPtr->srcHwDev));
    CPSS_API_CALL_UNLOCK();
    if(GT_OK != rc)
        return rc;
    fromCpuPtr->extDestInfo.devPort.dstIsTagged = dxChPcktParams.packetIsTagged;
    fromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU = GT_FALSE;

    CPSS_API_CALL_LOCK();
    rc = cpssDxChNetIfSdmaSyncTxPacketSend(devNum, &dxChPcktParams, buffList, buffLenList, numOfBufs);
    CPSS_API_CALL_UNLOCK();
    return rc;
#else /* !defined(CHX_FAMILY) */
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* transmitTask
*
* DESCRIPTION:
*       this task get data to be transmitted from queue,
*       transmit it, then send TX status
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID __TASKCONV transmitTask(void)
{
    TX_PARAMS_STC *txParams;
    GT_U32  msgSize;
    GT_STATUS rc;

    txParams = (TX_PARAMS_STC *)cpssOsCacheDmaMalloc(sizeof(*txParams));
    if (NULL == txParams)
    {
        /* failed to allocate DMA memory, destroy task */
        osMsgQDelete(transmitTaskQueueId);
        transmitTaskCreated = 0;
        return;
    }

    for(;;)
    {
        /* receive packet desc */
        msgSize = sizeof(*txParams);
        rc = osMsgQRecv(transmitTaskQueueId, txParams, &msgSize, OS_MSGQ_WAIT_FOREVER);
        if (rc != GT_OK)
            break;
        rc = GT_NOT_SUPPORTED;
        if(CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(txParams->srcInterface.hwDevNum)->devFamily))
        {
            rc = send_packet_DxCh(&(txParams->srcInterface),
                        txParams->packet.data,
                        txParams->packet.len);
        }
        /* send reply if required */
        if (txParams->replyQ)
        {
            osMsgQSend(txParams->replyQ, &rc, sizeof(rc), TRANSMIT_TASK_REPLY_TIMEOUT);
        }
    }
    /* smth is wrong, destroy task */
    osMsgQDelete(transmitTaskQueueId);
    cpssOsCacheDmaFree(txParams);
    transmitTaskCreated = 0;
}


/*******************************************************************************
* transmitTaskQueueIdGet
*
* DESCRIPTION:
*       get queue id for packet transmit task
*       start transmit task if not started yet
*
* INPUTS:
*       None
*
* OUTPUTS:
*       msgqId      message queue to transmit TX_PARAMS_STC to
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS transmitTaskQueueIdGet(
    OUT GT_MSGQ_ID  *msgqId
)
{
    GT_STATUS rc;
    CPSS_NULL_PTR_CHECK_MAC(msgqId);
    
    if (transmitTaskCreated)
    {
        *msgqId = transmitTaskQueueId;
        return GT_OK;
    }
    rc = osMsgQCreate("txTaskQ", TRANSMIT_TASK_QUEUE_LEN,
            sizeof(TX_PARAMS_STC), &transmitTaskQueueId);
    if (rc != GT_OK)
    {
        return rc;
    }
    *msgqId = transmitTaskQueueId;
    osTaskCreate(
        "txTask", /* name */
        5, /* prio */
        32768, /* stack */
        (unsigned (__TASKCONV *)(void*))transmitTask, /* entry point */
        NULL, /* arg */
        &transmitTaskId);
    transmitTaskCreated = 1;
    return GT_OK;
}

