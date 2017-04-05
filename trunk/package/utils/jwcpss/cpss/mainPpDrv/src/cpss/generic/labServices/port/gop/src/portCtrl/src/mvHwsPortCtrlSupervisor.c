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
* mvHwsPortCtrlSupervisor.c
*
* DESCRIPTION:
*       Port Control Supervisor State Machine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>

static MV_HWS_PORT_CTRL_COMMON *infoPtr;
static GT_U32 spvHighMsgCount = 0;
static GT_U32 spvLowMsgCount  = 0;

#ifdef BOBK_DEV_SUPPORT
extern void mvHwsAvagoAaplAddrGet(unsigned char devNum, unsigned int *devAddr,
                                  unsigned int *devSize);
#endif /* BOBK_DEV_SUPPORT */

/*******************************************************************************
* mvPortCtrlSpvRoutine
*
* DESCRIPTION: Supervisor process execution sequence
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
void mvPortCtrlSpvRoutine(void* pvParameters)
{
    mvPortCtrlSyncLock();
    mvPortCtrlSyncUnlock();

    infoPtr = &(mvPortCtrlSpv.info);

    for( ;; )
    {
        /*
        ** State Machine Transitions Table
        ** +================+=============+=============+=============+
        ** + Event \ State  + High Msg O1 + Low Msg O2  + Delay O3    +
        ** +================+=============+=============+=============+
        ** + High Msg       +     O1      +     O2      +    ==> O1   +
        ** +================+=============+=============+=============+
        ** + Low Msg        +   ==> O2    +   Invalid   +   Invalid   +
        ** +================+=============+=============+=============+
        ** + Delay          +   Invalid   +   ==> O3    +   Invalid   +
        ** +================+=============+=============+=============+
        */
        if (mvPortCtrlSpv.funcTbl[infoPtr->event][infoPtr->state] != NULL)
        {
            mvPortCtrlSpv.funcTbl[infoPtr->event][infoPtr->state]();
        }
        else
        {
            mvPcPrintf("Error, Port Ctrl, Supervisor Func table: state[%d] event[%d] is NULL\n",
                       infoPtr->state, infoPtr->event);
        }
    }
}

/*******************************************************************************
* mvPortCtrlSpvHighMsgExecute
*
* DESCRIPTION: Supervisor High priority message process state processing
*              - Handle "Pizza Arbbiter" Configuration
*                  Read message from high priority queue
*                  Message received
*                    Update database
*                    Add Log entry
*
* INPUTS:
*       MV_HWS_IPC_CTRL_MSG_STRUCT *recvMsg
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void mvPortCtrlSpvHighMsgExecute(MV_HWS_IPC_CTRL_MSG_STRUCT *recvMsg)
{
    MV_HWS_IPC_PORT_INFO_STRUCT *msgParams;

    /* Extract target port from IPC message and update system config valid status */
    /* System configuration use the format of Port Init message */
    msgParams = (MV_HWS_IPC_PORT_INFO_STRUCT*)&(recvMsg->msgData);

    if (recvMsg->ctrlMsgType == MV_HWS_IPC_PORT_AP_SYS_CFG_VALID_MSG)
    {
        mvHwsPortCtrlApPortMng.apPortTimer[msgParams->phyPortNum].sysCfStatus = PORT_SM_SERDES_SYSTEM_VALID;
        mvPortCtrlLogAdd(SPV_LOG(SPV_HIGH_MSG_PROCESS_STATE, recvMsg->ctrlMsgType,
                                 msgParams->phyPortNum, REG_PORT));
        /* TODO - Add handling of system state DISABLE / ENABLE */

        spvHighMsgCount++;
        if (spvHighMsgCount >= mvPortCtrlSpv.highMsgThreshold)
        {
            spvHighMsgCount = 0;
            infoPtr->event = SPV_LOW_MSG_EVENT;
        }
    }
}

/*******************************************************************************
* mvPortCtrlSpvHighMsg
*
* DESCRIPTION: Supervisor High priority message process state execution
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
void mvPortCtrlSpvHighMsg(void)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT recvMsg;

    infoPtr->state = SPV_HIGH_MSG_PROCESS_STATE;

    /* Process messages from High priority IPC message queue */
    if (mvHwsIpcRequestGet(MV_HWS_IPC_HIGH_PRI_QUEUE,
                           sizeof(MV_HWS_IPC_CTRL_MSG_STRUCT),
                           (char*)&recvMsg) != GT_OK)
    {
        spvHighMsgCount = 0;
        infoPtr->event = SPV_LOW_MSG_EVENT;
    }
    else
    {
         mvPortCtrlSpvHighMsgExecute(&recvMsg);
    }
}

/*******************************************************************************
* mvPortCtrlSpvLowMsgExecute
*
* DESCRIPTION: Supervisor Low priority message process state processing
*              - Validate system process can receive messgae
*                  In case Valid
*                    Handle Port Init / Reset IPC Message
*                      Add log entry
*                      Send message to Port process
*                    Handle AP Port IPC Message
*                      Add log entry
*                      Send message to AP Detection process
*                    Handle General IPC Message
*                      Add log entry
*                      Send message to General process
*                    Message not received
*                      Trigger state change
*              - Not Valid
*                  Trigger state change
*
* INPUTS:
*       MV_HWS_IPC_CTRL_MSG_STRUCT *recvMsg
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void mvPortCtrlSpvLowMsgExecute(MV_HWS_IPC_CTRL_MSG_STRUCT *recvMsg)
{
    MV_HWS_IPC_PORT_INIT_DATA_STRUCT *msgParams;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyMsg;
    GT_U32 avagoAaplAddr;
    GT_U32 avagoAaplSize;
    GT_U32 avagoGuiState;


    msgParams = (MV_HWS_IPC_PORT_INIT_DATA_STRUCT*)&(recvMsg->msgData);

    switch (recvMsg->ctrlMsgType)
    {
    case MV_HWS_IPC_PORT_AVAGO_SERDES_INIT_MSG:
#ifdef BOBK_DEV_SUPPORT
        mvHwsAvagoAaplAddrGet(0, &avagoAaplAddr, &avagoAaplSize);
#endif /* BOBK_DEV_SUPPORT */
        replyMsg.replyTo    = recvMsg->ctrlMsgType;
        replyMsg.returnCode = GT_OK;
        replyMsg.readData.portReplyGet.results = avagoAaplAddr;
        mvHwsIpcReplyMsg(recvMsg->msgQueueId, &replyMsg);
        break;

    case MV_HWS_IPC_PORT_AVAGO_GUI_SET_MSG:
        avagoGuiState = recvMsg->msgData.avagoGuiSet.state;
        avagoGuiState &= MV_PORT_CTRL_AVAGO_GUI_MASK;
        mvHwsApDetectStateSet(avagoGuiState);
        replyMsg.replyTo    = recvMsg->ctrlMsgType;
        replyMsg.returnCode = GT_OK;
        mvHwsIpcReplyMsg(recvMsg->msgQueueId, &replyMsg);
        break;

    /* Send messages to Port Mng process message queue */
    case MV_HWS_IPC_PORT_INIT_MSG:
    case MV_HWS_IPC_PORT_RESET_MSG:
    case MV_HWS_IPC_PORT_RESET_EXT_MSG:
        mvPortCtrlPortSm[msgParams->phyPortNum].type = REG_PORT;
        mvPortCtrlLogAdd(SPV_LOG(SPV_LOW_MSG_PROCESS_STATE, recvMsg->ctrlMsgType,
                                 msgParams->phyPortNum, REG_PORT));
        mvPortCtrlProcessMsgSend(M2_PORT_MNG, recvMsg);
        break;

    /* Send messages to AP Detection process message queue */
    case MV_HWS_IPC_PORT_AP_ENABLE_MSG:
    case MV_HWS_IPC_PORT_AP_DISABLE_MSG:
    case MV_HWS_IPC_PORT_AP_CFG_GET_MSG:
    case MV_HWS_IPC_PORT_AP_STATUS_MSG:
    case MV_HWS_IPC_PORT_AP_STATS_MSG:
    case MV_HWS_IPC_PORT_AP_STATS_RESET_MSG:
    case MV_HWS_IPC_PORT_AP_INTROP_GET_MSG:
    case MV_HWS_IPC_PORT_AP_INTROP_SET_MSG:
    case MV_HWS_IPC_PORT_AP_DEBUG_GET_MSG:
        mvPortCtrlPortSm[msgParams->phyPortNum].type = AP_PORT;
        mvPortCtrlLogAdd(SPV_LOG(SPV_LOW_MSG_PROCESS_STATE, recvMsg->ctrlMsgType,
                                 msgParams->phyPortNum, AP_PORT));
        mvPortCtrlProcessMsgSend(M5_AP_PORT_DET, recvMsg);
        break;

    /* Send messages to General process message queue */
    default:
        mvPortCtrlLogAdd(SPV_LOG(SPV_LOW_MSG_PROCESS_STATE, recvMsg->ctrlMsgType,
                                 msgParams->phyPortNum, REG_PORT));
        mvPortCtrlProcessMsgSend(M6_GENERAL, recvMsg);
        break;
    }
    spvLowMsgCount++;
}

/*******************************************************************************
* mvPortCtrlSpvLowMsg
*
* DESCRIPTION: Supervisor Low priority message process state execution
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
void mvPortCtrlSpvLowMsg(void)
{
    GT_STATUS                  rcode;
    GT_U32                     portMsgNum;
    GT_U32                     apMsgNum;
    GT_U32                     genMsgNum;
    MV_HWS_IPC_CTRL_MSG_STRUCT recvMsg;
    
    infoPtr->state = SPV_LOW_MSG_PROCESS_STATE;

    /* Validate system process can receive messgaes
    ** Threshold is set to max of 6 message at each processing interval
    ** under the validation that the target queue has enough space
    */
    rcode  = mvPortCtrlProcessPendMsgNum(M2_PORT_MNG, &portMsgNum);
    rcode |= mvPortCtrlProcessPendMsgNum(M5_AP_PORT_DET, &apMsgNum);
    rcode |= mvPortCtrlProcessPendMsgNum(M6_GENERAL, &genMsgNum);
    if (rcode != GT_OK)
    {
        mvPcPrintf("Error, Supervisor, Failed to get number of pending messages\n");
        return;
    }

    if ((portMsgNum     < MV_PORT_CTRL_SPV_MSG_EXEC_THRESHOLD) &&
        (apMsgNum       < MV_PORT_CTRL_SPV_MSG_EXEC_THRESHOLD) &&
        (genMsgNum      < MV_PORT_CTRL_SPV_MSG_EXEC_THRESHOLD) &&
        (spvLowMsgCount < mvPortCtrlSpv.lowMsgThreshold))
    {
        /* Process messages from Low priority IPC message queue */
        if (mvHwsIpcRequestGet(MV_HWS_IPC_LOW_PRI_QUEUE,
                               sizeof(MV_HWS_IPC_CTRL_MSG_STRUCT),
                               (char*)&recvMsg) != GT_OK)
        {
            spvLowMsgCount = 0;
            infoPtr->event = SPV_DELAY_EVENT;
        }
        else
        {
           mvPortCtrlSpvLowMsgExecute(&recvMsg);
        }
    }
    else
    {
        spvLowMsgCount = 0;
        infoPtr->event = SPV_DELAY_EVENT;
    }
}

/*******************************************************************************
* mvPortCtrlSpvDelay
*
* DESCRIPTION: Superviosr message delay state execution
*              - Execute delay
*              - Trigger state change
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
void mvPortCtrlSpvDelay(void)
{
    infoPtr->state = SPV_DELAY_STATE;
    mvPortCtrlProcessDelay(infoPtr->delayDuration);
    infoPtr->event = SPV_HIGH_MSG_EVENT;
}

