/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsIpcApi.c
*
* DESCRIPTION:
*           This file contains APIs for HWS IPC
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <mvHwsPortInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <mvHwsIpcDefs.h>
#include <mvHwsIpcApis.h>

/**************************** Globals ****************************************************/
#ifndef MV_HWS_REDUCED_BUILD

/* IPC queue IDs pool */
GT_U32 hwsIpcQueueIdPool[HWS_MAX_DEVICE_NUM][MV_HWS_MAX_HOST2HWS_REQ_MSG_NUM] = {{0},{0}};

/* the structure to gather HWS IPC statistics on Host*/
MV_HWS_IPC_STATISTICS_STRUCT mvHwsIpcStatistics[HWS_MAX_DEVICE_NUM];

/**************************** Pre-Declaration ********************************************/
GT_STATUS mvHwsSetIpcInfo(GT_U8 devNum, GT_U32 msgType, MV_HWS_IPC_CTRL_MSG_STRUCT *msgDataPtr,
                          GT_U32 msgLength);
GT_STATUS mvHwsIpcCtrlMsgTx(MV_HWS_IPC_CTRL_MSG_STRUCT *txCtrlMsg);
GT_STATUS mvHwsIpcReplyMsgRx(GT_U8 devNum, MV_HWS_IPC_CTRL_MSG_DEF_TYPE msgId, GT_U32 queueId,
                             MV_HWS_IPC_REPLY_MSG_STRUCT *rxReplyData);

/*******************************************************************************
* mvHwsIpcDbInit
*
* DESCRIPTION:
*       HW Services Ipc database initialization
*
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_VOID mvHwsIpcDbInit
(
    GT_U8 devNum
)
{
    hwsOsMemSetFuncPtr(hwsIpcQueueIdPool[devNum],0 , MV_HWS_MAX_HOST2HWS_REQ_MSG_NUM);
    hwsOsMemSetFuncPtr(mvHwsIpcStatistics[devNum].mvHwsHostRxMsgCount, 0, MV_HWS_IPC_LAST_CTRL_MSG_TYPE);
    hwsOsMemSetFuncPtr(mvHwsIpcStatistics[devNum].mvHwsHostTxMsgCount, 0, MV_HWS_IPC_LAST_CTRL_MSG_TYPE);
    hwsOsMemSetFuncPtr(mvHwsIpcStatistics[devNum].mvHwsPortIpcFailureCount, 0 , MV_HWS_IPC_MAX_PORT_NUM);
}

/*******************************************************************************
* mvHwsGetQueueId
*
* DESCRIPTION:
*       finds free queue for Host 2 HWS connection
*
*
* INPUTS:
*       devNum      - device number
*       queueId     - pointer to queue ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_U32 mvHwsGetQueueId
(
    GT_U8 devNum,
    GT_U32 * queueId
)
{
    GT_U32   i;

    for (i = MV_HWS_IPC_TX_0_CH_ID; i < MV_HWS_MAX_HOST2HWS_REQ_MSG_NUM; i++)
    {
        if (hwsIpcQueueIdPool[devNum][i] == MV_HWS_IPC_FREE_QUEUE)
        {
            *queueId = i;
            hwsIpcQueueIdPool[devNum][i] = MV_HWS_IPC_QUEUE_BUSY;
            return GT_OK;
        }
    }

    if (i == MV_HWS_MAX_HOST2HWS_REQ_MSG_NUM)
    {
        mvHwsIpcStatistics[devNum].mvHwsIpcGenFailureCount++;
        osPrintf("No free Host2Hws TX message\n");
        return GT_NO_RESOURCE;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsSetIpcInfo
*
* DESCRIPTION:
*       returns queue Id to the pool
*
*
* INPUTS:
*       devNum     - device number
*       queueId    - queue Id
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsReturnQueueId
(
    GT_U8 devNum,
    GT_U32 queueId
)
{
    if (queueId >= MV_HWS_MAX_HOST2HWS_REQ_MSG_NUM)
    {
        mvHwsIpcStatistics[devNum].mvHwsIpcGenFailureCount++;
        osPrintf("mvHwsReturnQueueId queue ID %d doesn't exist \n", queueId);
        return GT_BAD_PARAM;
    }

    hwsIpcQueueIdPool[devNum][queueId] = MV_HWS_IPC_FREE_QUEUE;

    return GT_OK;

}

/*******************************************************************************
* mvHwsIpcCtrlMsgTx
*
* DESCRIPTION:
*       Send IPC message from Host to HW Services
*
*
* INPUTS:
*       txCtrlMsg - pointer to the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsIpcCtrlMsgTx
(
    MV_HWS_IPC_CTRL_MSG_STRUCT *txCtrlMsg
)
{
    /* Check that the message is valid: msg type and queueId are in the defined range */
    if ((txCtrlMsg->ctrlMsgType >= MV_HWS_IPC_LAST_CTRL_MSG_TYPE ) ||
         (txCtrlMsg->msgQueueId >= MV_HWS_MAX_HWS2HOST_REPLY_QUEUE_NUM))
    {
        mvHwsIpcStatistics[txCtrlMsg->devNum].mvHwsPortIpcFailureCount[txCtrlMsg->msgData.portGeneral.phyPortNum]++;
        osPrintf ("mvHwsIpcCtrlMsgTx wrong parameter msg type %d queue ID %d",txCtrlMsg->ctrlMsgType,txCtrlMsg->msgQueueId);
        return GT_BAD_PARAM;
    }

    /* Update counter */
    mvHwsIpcStatistics[txCtrlMsg->devNum].mvHwsHostTxMsgCount[txCtrlMsg->ctrlMsgType]++;

    /* Send msg to Service CPU*/
    if (txCtrlMsg->ctrlMsgType != MV_HWS_IPC_PORT_AP_SYS_CFG_VALID_MSG)
    {
        CHECK_STATUS(mvHwsServiceCpuMsgSend(txCtrlMsg->devNum, MV_HWS_IPC_LOW_PRI_QUEUE, (char*)txCtrlMsg));
    }
    else /* MV_HWS_IPC_PORT_SYS_CFG_VALID_MSG */
    {
        CHECK_STATUS(mvHwsServiceCpuMsgSend(txCtrlMsg->devNum, MV_HWS_IPC_HIGH_PRI_QUEUE, (char*)txCtrlMsg));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsSetIpcInfo
*
* DESCRIPTION:
*       writes IPC data to message structure
*
*
* INPUTS:
*       devNum     - system device number
*       msgDataPtr - pointer to message data
*       msgLength  - message length
*       msgType    - message type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSetIpcInfo
(
    GT_U8                        devNum,
    MV_HWS_IPC_CTRL_MSG_DEF_TYPE msgType,
    MV_HWS_IPC_CTRL_MSG_STRUCT   *msgDataPtr,
    GT_U32                       msgLength
)
{
    GT_U32 queueId;

    msgDataPtr->devNum      = devNum;
    msgDataPtr->msgLength   = (GT_U8)msgLength;
    msgDataPtr->ctrlMsgType = (GT_U8)msgType;
    CHECK_STATUS(mvHwsGetQueueId(devNum, &queueId));
    msgDataPtr->msgQueueId  = (GT_U8)queueId;

    return GT_OK;
}

/*******************************************************************************
* mvHwsIpcReplyMsgRx
*
* DESCRIPTION:
*       Gets reply from HW Services to the Host
*
* INPUTS:
*       devNum      - system device number
*       queueId     - queue ID
*       msgId       - message ID
*       rxReplyData - pointer to message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsIpcReplyMsgRx
(
    GT_U8                        devNum,
    MV_HWS_IPC_CTRL_MSG_DEF_TYPE msgId,
    GT_U32                       queueId,
    MV_HWS_IPC_REPLY_MSG_STRUCT  *rxReplyData
)
{
    GT_STATUS rcode;
    GT_U32 replyTimeout = 100;

    /* Wait for message from Service CPU */
    do
    {
        rcode = mvHwsServiceCpuMsgRecv(devNum, queueId, (char*)rxReplyData);
        if (rcode == GT_OK)
        {
            break;
        }
        else
        {
            hwsOsExactDelayPtr(devNum, 0, 1);
            replyTimeout--;
        }
    } while (replyTimeout > 0);

    /* Free the queue */
    CHECK_STATUS(mvHwsReturnQueueId(devNum, queueId));

    if (rcode != GT_OK)
    {
        osPrintf("Failed to read from channel %d\n", queueId);
        return (GT_FAIL);
    }

    if (rxReplyData->replyTo != (GT_U32)msgId)
    {
        mvHwsIpcStatistics[devNum].mvHwsIpcGenFailureCount++;
        osPrintf ("mvHwsIpceplyMsgRx wrong msg ID %d Expetced %d queue ID %d\n",rxReplyData->replyTo, msgId,queueId);
        return GT_BAD_PARAM;
    }

    /* Updte statistics*/
    mvHwsIpcStatistics[devNum].mvHwsHostRxMsgCount[msgId]++;

    return GT_OK;
}


/*******************************************************************************
* mvHwsIpcSendRequestAndGetReply
*
* DESCRIPTION:
*       Gets reply from HW Services to the Host
*
* INPUTS:
*       requestMsg  - pointer to request message
*       replyData   - pointer to reply message
*       msgId       - message ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on errorGT_U8 devNum,
                           MV_HWS_IPC_CTRL_MSG_STRUCT *msgDataPtr,
                           GT_U32 msgLength,

*
*******************************************************************************/
GT_STATUS mvHwsIpcSendRequestAndGetReply
(
    GT_U8                                   devNum,
    MV_HWS_IPC_CTRL_MSG_STRUCT              *requestMsg,
    MV_HWS_IPC_REPLY_MSG_STRUCT             *replyData,
    GT_U32                                  msgLength,
    MV_HWS_IPC_CTRL_MSG_DEF_TYPE            msgId
)
{
    /* Set IPC info */
    CHECK_STATUS(mvHwsSetIpcInfo (devNum,msgId,requestMsg, msgLength));

    /* Send IPC message */
    CHECK_STATUS(mvHwsIpcCtrlMsgTx (requestMsg));

    /* Wait for reply */
    CHECK_STATUS(mvHwsIpcReplyMsgRx (requestMsg->devNum,msgId,requestMsg->msgQueueId,replyData));

    return GT_OK;
}

#endif /* MV_HWS_REDUCED_BUILD */

