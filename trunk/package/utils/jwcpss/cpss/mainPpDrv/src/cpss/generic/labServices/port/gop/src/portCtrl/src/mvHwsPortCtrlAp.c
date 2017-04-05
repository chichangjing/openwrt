/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsPortCtrlAp.c
*
* DESCRIPTION:
*       Port Control AP Detection State Machine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>

GT_U32 mvHwsApDetectExecutionEnabled = 0;

static MV_HWS_PORT_CTRL_COMMON *infoPtr;

/*******************************************************************************
* mvHwsApDetectStateSet
*
* DESCRIPTION: AP Detection process enable / disable set
*
* INPUTS:
*       state - enable / disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvHwsApDetectStateSet(GT_U32 state)
{
    mvHwsApDetectExecutionEnabled = state;
}

/*******************************************************************************
* mvHwsApDetectRoutine
*
* DESCRIPTION:
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvHwsApDetectRoutine(void* pvParameters)
{
    mvPortCtrlSyncLock();
    mvPortCtrlSyncUnlock();

    infoPtr = &(mvHwsPortCtrlApPortDetect.info);

    for( ;; )
    {
        /*
        ** State Machine Transitions Table
        ** +================+=============+=============+=============+
        ** + Event \ State  +  Active O1  +   Msg O2    +  Delay O3   +
        ** +================+=============+=============+=============+
        ** + Active         +     O1      +   Invalid   +    ==> O1   +
        ** +================+=============+=============+=============+
        ** + Msg            +   ==> O2    +     O2      +   Invalid   +
        ** +================+=============+=============+=============+
        ** + Delay          +   Invalid   +   ==> O3    +   Invalid   +
        ** +================+=============+=============+=============+
        */
        if (mvHwsPortCtrlApPortDetect.funcTbl[infoPtr->event][infoPtr->state] != NULL)
        {
            mvHwsPortCtrlApPortDetect.funcTbl[infoPtr->event][infoPtr->state]();
        }
        else
        {
            mvPcPrintf("Error, Port Detect, Func table: state[%d] event[%d] is NULL\n",
                       infoPtr->state, infoPtr->event);
        }
    }
}

/*******************************************************************************
* mvPortCtrlApPortDetectionActiveExec
*
* DESCRIPTION: AP Detect Active state execution
*              - Exract port state
*              - Execute state functionality
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
*******************************************************************************/
void mvPortCtrlApPortDetectionActiveExec(GT_U8 portIndex)
{
    MV_HWS_AP_SM_INFO *apSm = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);

    switch (apSm->state)
    {
    case AP_PORT_SM_IDLE_STATE:
        return;

    case AP_PORT_SM_INIT_STATE:
        mvApPortInit(portIndex);
        break;

    case AP_PORT_SM_TX_DISABLE_STATE:
        mvApPortTxDisable(portIndex);
        break;

    case AP_PORT_SM_RESOLUTION_STATE:
        mvApPortResolution(portIndex);
        break;

    case AP_PORT_SM_ACTIVE_STATE:
        mvApPortLinkUp(portIndex);
        break;

    case AP_PORT_SM_DELETE_STATE:
        mvApPortDeleteValidate(portIndex);
        break;

    default:
        mvPcPrintf("Error, AP Detect, PortIndex-%d Invalid state %d!!!\n", portIndex, apSm->state);
        break;
    }
}

/*******************************************************************************
* mvPortCtrlApDetectActive
*
* DESCRIPTION: AP Detect Active state execution
*              - Scan all ports and call port execution function
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
*******************************************************************************/
void mvPortCtrlApDetectActive(void)
{
    GT_U8 portIndex;

    infoPtr->state = AP_PORT_DETECT_ACTIVE_PORT_STATE;

    for (portIndex = 0; portIndex < MV_PORT_CTRL_MAX_AP_PORT_NUM; portIndex++)
    {
        mvPortCtrlApPortDetectionActiveExec(portIndex);
    }

    infoPtr->event = AP_PORT_DETECT_MSG_PROCESS_EVENT;
}

/*******************************************************************************
* mvPortCtrlApPortMsgReply
*
* DESCRIPTION: AP Port mng info message reply
*
* INPUTS:
*       MV_HWS_IPC_REPLY_MSG_STRUCT replyMsg
*       GT_U8 queueId
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlApPortMsgReply(MV_HWS_IPC_REPLY_MSG_STRUCT *replyMsg, GT_U8 queueId)
{
    /* Build and Send IPC reply */
    CHECK_STATUS(mvHwsIpcReplyMsg(queueId, replyMsg));

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlApPortDetectionMsgExec
*
* DESCRIPTION: AP Detect message state execution
*              - Extract Message type, Message params, and Port index
*              - Handle AP Enable message
*                  AP State == IDLE
*                    Extract and update message parameters
*                    Execute port initial configuration
*                    Trigger state change
*                  AP State != IDLE
*                    Send reply error message to calling applicaion
*              - Handle AP Disable message
*                  AP State != IDLE
*                    Execute port delete
*                  AP State == IDLE
*                    Print error message
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
*******************************************************************************/
void mvPortCtrlApPortDetectionMsgExec(MV_HWS_IPC_CTRL_MSG_STRUCT *msg)
{
    GT_U8 msgType;
    GT_U8 portIndex;
    GT_U32 fwBaseAddr;
    GT_U32 logBaseAddr;
    GT_U32 logPointer;
    GT_U32 logCount;
    GT_U32 logReset;
    MV_HWS_IPC_PORT_AP_DATA_STRUCT *msgParams;
    MV_HWS_AP_SM_INFO *apSm;
    MV_HWS_AP_SM_STATS *apStats;
    MV_HWS_AP_DETECT_ITEROP *apIntrop;

    MV_HWS_IPC_REPLY_MSG_STRUCT replyMsg;

    msgType = msg->ctrlMsgType;
    msgParams = (MV_HWS_IPC_PORT_AP_DATA_STRUCT*)&(msg->msgData);

    replyMsg.replyTo    = msgType;
    replyMsg.returnCode = GT_OK;

    /* Validate input port number */
    if ((msgParams->phyPortNum < MV_PORT_CTRL_AP_PORT_NUM_BASE) &&
        (msgType != MV_HWS_IPC_PORT_AP_INTROP_GET_MSG) &&
        (msgType != MV_HWS_IPC_PORT_AP_INTROP_SET_MSG) &&
        (msgType != MV_HWS_IPC_PORT_AP_DEBUG_GET_MSG))
    {
        replyMsg.returnCode = GT_BAD_PARAM;
        mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        return;
    }

    portIndex = MV_HWS_PORT_CTRL_AP_LOGICAL_PORT_GET(msgParams->phyPortNum);
    apSm     = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    apStats  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);

    switch (msgType)
    {
    case MV_HWS_IPC_PORT_AP_ENABLE_MSG:
        if (apSm->state == AP_PORT_SM_IDLE_STATE)
        {
            apSm->queueId = msg->msgQueueId;
            apSm->portNum = msgParams->phyPortNum;
            AP_CTRL_LANE_SET(apSm->ifNum, msgParams->laneNum);
            AP_CTRL_PCS_SET(apSm->ifNum, msgParams->pcsNum);
            AP_CTRL_MAC_SET(apSm->ifNum, msgParams->macNum);
            AP_CTRL_ADV_ALL_SET(apSm->capability, msgParams->advMode);
            AP_CTRL_LB_EN_SET(apSm->options, AP_CTRL_LB_EN_GET(msgParams->options));
            AP_CTRL_FC_PAUSE_SET(apSm->options, AP_CTRL_FC_PAUSE_GET(msgParams->options));
            AP_CTRL_FC_ASM_SET(apSm->options, AP_CTRL_FC_ASM_GET(msgParams->options));
            AP_CTRL_FEC_ABIL_SET(apSm->options, AP_CTRL_FEC_ABIL_GET(msgParams->options));
            AP_CTRL_FEC_REQ_SET(apSm->options, AP_CTRL_FEC_REQ_GET(msgParams->options));

            /* Initial configuration */
            mvHwsInitialConfiguration(portIndex);

            apSm->status = AP_PORT_START_EXECUTE;
            apSm->state = AP_PORT_SM_INIT_STATE;
            mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        }
        else
        {
            mvPcPrintf("Error, AP Detect, Port-%d is under execution, AP Enable is not valid!!!\n", msgParams->phyPortNum);
            replyMsg.returnCode = GT_CREATE_ERROR;
            mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        }
        break;

    case MV_HWS_IPC_PORT_AP_DISABLE_MSG:
        if (apSm->state != AP_PORT_SM_IDLE_STATE)
        {
            mvApPortDeleteMsg(portIndex);
            mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        }
        else
        {
            mvPcPrintf("Error, AP Detect, Port-%d is NOT under execution, AP Disable is not valid!!!\n", msgParams->phyPortNum);
            replyMsg.returnCode = GT_FAIL;
            mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        }
        break;

    case MV_HWS_IPC_PORT_AP_CFG_GET_MSG:
        replyMsg.readData.portApCfgGet.ifNum      = apSm->ifNum;
        replyMsg.readData.portApCfgGet.capability = apSm->capability;
        replyMsg.readData.portApCfgGet.options    = apSm->options;
        mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        break;

    case MV_HWS_IPC_PORT_AP_STATUS_MSG:
        replyMsg.readData.portApStatusGet.state       = apSm->state;
        replyMsg.readData.portApStatusGet.status      = apSm->status;
        replyMsg.readData.portApStatusGet.laneNum     = AP_CTRL_LANE_GET(apSm->ifNum);
        replyMsg.readData.portApStatusGet.ARMSmStatus = apSm->ARMSmStatus;
        replyMsg.readData.portApStatusGet.hcdStatus   = apSm->hcdStatus;
        mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        break;

    case MV_HWS_IPC_PORT_AP_STATS_MSG:
        replyMsg.readData.portApStatsGet.txDisCnt          = apStats->txDisCnt;
        replyMsg.readData.portApStatsGet.abilityCnt        = apStats->abilityCnt;
        replyMsg.readData.portApStatsGet.abilitySuccessCnt = apStats->abilitySuccessCnt;
        replyMsg.readData.portApStatsGet.linkFailCnt       = apStats->linkFailCnt;
        replyMsg.readData.portApStatsGet.linkSuccessCnt    = apStats->linkSuccessCnt;
        replyMsg.readData.portApStatsGet.hcdResoultionTime = apStats->hcdResoultionTime;
        replyMsg.readData.portApStatsGet.linkUpTime        = apStats->linkUpTime;
        mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        break;

    case MV_HWS_IPC_PORT_AP_STATS_RESET_MSG:
        mvPortCtrlDbgStatsReset(msgParams->phyPortNum);
        mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        break;

    case MV_HWS_IPC_PORT_AP_INTROP_GET_MSG:
        apIntrop = &(mvHwsPortCtrlApPortDetect.introp);
        replyMsg.readData.portApIntropGet.txDisDuration          = apIntrop->txDisDuration;
        replyMsg.readData.portApIntropGet.abilityDuration        = apIntrop->abilityDuration;
        replyMsg.readData.portApIntropGet.abilityMaxInterval     = apIntrop->abilityMaxInterval;
        replyMsg.readData.portApIntropGet.abilityFailMaxInterval = apIntrop->abilityFailMaxInterval;
        replyMsg.readData.portApIntropGet.apLinkDuration         = apIntrop->apLinkDuration;
        replyMsg.readData.portApIntropGet.apLinkMaxInterval      = apIntrop->apLinkMaxInterval;
        replyMsg.readData.portApIntropGet.pdLinkDuration         = apIntrop->pdLinkDuration;
        replyMsg.readData.portApIntropGet.pdLinkMaxInterval      = apIntrop->pdLinkMaxInterval;
        mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        break;

    case MV_HWS_IPC_PORT_AP_INTROP_SET_MSG:
        apIntrop = (MV_HWS_AP_DETECT_ITEROP*)&(msg->msgData);
        mvPortCtrlDbgIntropCfg(apIntrop);
        mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        break;

    case MV_HWS_IPC_PORT_AP_DEBUG_GET_MSG:
        mvPortCtrlLogInfoGet(&fwBaseAddr, &logBaseAddr, &logPointer, &logCount, &logReset);
        replyMsg.readData.portApDebugGet.fwBaseAddr     = fwBaseAddr;
        replyMsg.readData.portApDebugGet.logBaseAddr    = logBaseAddr;
        replyMsg.readData.portApDebugGet.logPointerAddr = logPointer;
        replyMsg.readData.portApDebugGet.logCountAddr   = logCount;
        replyMsg.readData.portApDebugGet.logResetAddr   = logReset;
        mvPortCtrlApPortMsgReply(&replyMsg, msg->msgQueueId);
        break;
    }
}

/*******************************************************************************
* mvPortCtrlApDetectMsg
*
* DESCRIPTION: AP Detect message state execution
*              - Read message from AP detection message queue
*                  Message received
*                    Execute message to completion
*                  Message not received
*                    Trigger state change
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
*******************************************************************************/
void mvPortCtrlApDetectMsg(void)
{
    GT_STATUS rcode;

    MV_HWS_IPC_CTRL_MSG_STRUCT recvMsg;

    infoPtr->state = AP_PORT_DETECT_MSG_PROCESS_STATE;

    do
    {
        rcode = mvPortCtrlProcessMsgRecv(M5_AP_PORT_DET, MV_PROCESS_MSG_RX_DELAY, &recvMsg);
        if (rcode == GT_OK)
        {
            mvPortCtrlApPortDetectionMsgExec(&recvMsg);
        }
    } while (rcode == GT_OK);

    infoPtr->event = AP_PORT_DETECT_DELAY_EVENT;
}

/*******************************************************************************
* mvPortCtrlApDetectDelay
*
* DESCRIPTION: AP Delay state execution
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
*******************************************************************************/
void mvPortCtrlApDetectDelay(void)
{
    infoPtr->state = AP_PORT_DETECT_DELAY_STATE;

    do
    {
        /* When Avago GUI is enabled, AP periodic execution is placed on hold
        ** AP process will not execute any activity
        */
        mvPortCtrlProcessDelay(mvHwsPortCtrlApPortDetect.info.delayDuration);
    } while (mvHwsApDetectExecutionEnabled == MV_PORT_CTRL_AVAGO_GUI_ENABLED);

    infoPtr->event = AP_PORT_DETECT_ACTIVE_PORT_EVENT;
}


