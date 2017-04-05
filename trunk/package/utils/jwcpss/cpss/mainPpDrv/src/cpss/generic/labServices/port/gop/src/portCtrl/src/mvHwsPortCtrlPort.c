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
* mvHwsPortCtrlPort.c
*
* DESCRIPTION:
*       Port Control Port State Machine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>

static MV_HWS_PORT_CTRL_COMMON *infoPtr;

/*******************************************************************************
* mvPortCtrlPortRoutine
*
* DESCRIPTION: Port mng process execution sequence
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
void mvPortCtrlPortRoutine(void* pvParameters)
{
    mvPortCtrlSyncLock();
    mvPortCtrlSyncUnlock();

    infoPtr = &(mvPortCtrlPortMng.info);

    for( ;; )
    {
        /*
        ** State Machine Transitions Table
        ** +================+=============+=============+
        ** + Event \ State  +   Msg O1    +   Delay O2  +
        ** +================+=============+=============+
        ** + Msg            +     O1      +    ==> O1   +
        ** +================+=============+=============+
        ** + Delay          +   ==> O2    +   Invalid   +
        ** +================+=============+=============+
        */
        if (mvPortCtrlPortMng.funcTbl[infoPtr->event][infoPtr->state] != NULL)
        {
            mvPortCtrlPortMng.funcTbl[infoPtr->event][infoPtr->state]();
        }
        else
        {
            mvPcPrintf("Error, Port Ctrl, Port Func table: state[%d] event[%d] is NULL\n",
                       infoPtr->state, infoPtr->event);
        }
    }
}

/*******************************************************************************
* mvPortCtrlPortResetExec
*
* DESCRIPTION: Port mng message processing - reset execution
*              - Execute port reset and return to Idle state
*
* INPUTS:
*       GT_U32 port
*       MV_HWS_PORT_CTRL_PORT_SM *info
*       MV_HWS_IPC_CTRL_MSG_STRUCT *msg
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlPortResetExec(GT_U32 port,
                                  MV_HWS_PORT_CTRL_PORT_SM *info,
                                  MV_HWS_IPC_CTRL_MSG_STRUCT *msg)
{
    MV_HWS_IPC_PORT_RESET_DATA_STRUCT *msgParams;

    msgParams = (MV_HWS_IPC_PORT_RESET_DATA_STRUCT*)&(msg->msgData);

    info->status = PORT_SM_DELETE_IN_PROGRESS;
    mvPortCtrlLogAdd(PORT_SM_LOG(info->state, info->status, port));

    /* Execute HWS Port Reset */
    if (mvHwsPortReset(msg->devNum,
                       msgParams->portGroup,
                       msgParams->phyPortNum,
                       msgParams->portMode,
                       msgParams->action) != GT_OK)
    {
        info->status = PORT_SM_DELETE_FAILURE;
        mvPortCtrlLogAdd(PORT_SM_LOG(info->state, info->status, port));

        return GT_FAIL;
    }

    info->state = PORT_SM_IDLE_STATE;
    info->status = PORT_SM_DELETE_SUCCESS;
    mvPortCtrlLogAdd(PORT_SM_LOG(info->state, info->status, port));

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlPortMsg
*
* DESCRIPTION: Port mng message processing state execution
*              - Read message from port message queue queue
*                  Message received
*                    Add Log entry
*                    Execute message to completion
*                    Send reply message
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
void mvPortCtrlPortMsg(void)
{
    GT_STATUS                        rcode;
    MV_HWS_IPC_CTRL_MSG_STRUCT       recvMsg;
    MV_HWS_IPC_PORT_INIT_DATA_STRUCT *msgParams;

    infoPtr->state = PORT_MNG_MSG_PROCESS_STATE;

    do
    {
        rcode = mvPortCtrlProcessMsgRecv(M2_PORT_MNG, MV_PROCESS_MSG_RX_NO_DELAY, &recvMsg);
        if (rcode == GT_OK)
        {
            msgParams = (MV_HWS_IPC_PORT_INIT_DATA_STRUCT*)&(recvMsg.msgData);

            mvPortCtrlLogAdd(PORT_MNG_LOG(PORT_MNG_MSG_PROCESS_STATE, msgParams->phyPortNum, recvMsg.ctrlMsgType));

            rcode = mvPortCtrlPortMsgExec(msgParams->phyPortNum, &recvMsg);

            mvPortCtrlPortMsgReply(rcode, recvMsg.msgQueueId, recvMsg.ctrlMsgType);
        }
    } while (rcode == GT_OK);

    infoPtr->event = PORT_MNG_DELAY_EVENT;
}

/*******************************************************************************
* mvPortCtrlPortMsgExec
*
* DESCRIPTION: Port mng message processing state execution
*              - Idle State
*                  Init message - Execute port init from Idle state to Active state
*                  Reset message - Execute port reset and return to Idle state
*              - Active State
*                  Init message - Return Error
*                  Reset message - Execuet port reset and return to Idle state
* INPUTS:
*       GT_U32 port
*       MV_HWS_IPC_CTRL_MSG_STRUCT *msg
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlPortMsgExec(GT_U32 port,
                                MV_HWS_IPC_CTRL_MSG_STRUCT *msg)
{
    MV_HWS_IPC_PORT_INIT_DATA_STRUCT *msgParams;
    MV_HWS_PORT_CTRL_PORT_SM         *portCtrlSmPtr = &(mvPortCtrlPortSm[port]);
    GT_U8                            msgType = msg->ctrlMsgType;

    switch (portCtrlSmPtr->state)
    {
        case PORT_SM_IDLE_STATE:
            if (msgType == MV_HWS_IPC_PORT_INIT_MSG)
            {
                portCtrlSmPtr->status = PORT_SM_START_EXECUTE;
                mvPortCtrlLogAdd(PORT_SM_LOG(portCtrlSmPtr->state, portCtrlSmPtr->status, port));
                portCtrlSmPtr->state  = PORT_SM_SERDES_CONFIG_STATE;
                portCtrlSmPtr->status = PORT_SM_SERDES_CONFIG_IN_PROGRESS;
                mvPortCtrlLogAdd(PORT_SM_LOG(portCtrlSmPtr->state, portCtrlSmPtr->status, port));
                /**PLACE Holder: HWS config SERDES*/
                if (0)
                {
                    portCtrlSmPtr->status = PORT_SM_MAC_PCS_CONFIG_FAILURE;
                    mvPortCtrlLogAdd(PORT_SM_LOG(portCtrlSmPtr->state, portCtrlSmPtr->status, port));
                    /* Move to Idle state to enable SM execution for future messages */
                    portCtrlSmPtr->state  = PORT_SM_IDLE_STATE;
                    return GT_FAIL;
                }
                portCtrlSmPtr->status = PORT_SM_SERDES_CONFIG_SUCCESS;
                portCtrlSmPtr->state  = PORT_SM_MAC_PCS_CONFIG_STATE;
                mvPortCtrlLogAdd(PORT_SM_LOG(portCtrlSmPtr->state, portCtrlSmPtr->status, port));
                portCtrlSmPtr->status = PORT_SM_MAC_PCS_CONFIG_IN_PROGRESS;
                mvPortCtrlLogAdd(PORT_SM_LOG(portCtrlSmPtr->state, portCtrlSmPtr->status, port));
                /**PLACE Holder: HWS config MAC / PCS*/
                msgParams = (MV_HWS_IPC_PORT_INIT_DATA_STRUCT*)&(msg->msgData);

                /* Execute HWS Port Init */
                if (mvHwsPortInit(msg->devNum,
                                  msgParams->portGroup,
                                  msgParams->phyPortNum,
                                  msgParams->portMode,
                                  msgParams->lbPort,
                                  msgParams->refClock,
                                  msgParams->refClockSource) != GT_OK)
                {
                    mvPcPrintf("Error, Port Mng, Port-%d Init Failed\n", port);

                    portCtrlSmPtr->status = PORT_SM_SERDES_CONFIG_FAILURE;
                    mvPortCtrlLogAdd(PORT_SM_LOG(portCtrlSmPtr->state, portCtrlSmPtr->status, port));
                    /* Move to Idle state to enable SM execution for future messages */
                    portCtrlSmPtr->state  = PORT_SM_IDLE_STATE;
                    return GT_FAIL;
                }
                portCtrlSmPtr->status = PORT_SM_MAC_PCS_CONFIG_SUCCESS;
                portCtrlSmPtr->state  = PORT_SM_ACTIVE_STATE;
                mvPortCtrlLogAdd(PORT_SM_LOG(portCtrlSmPtr->state, portCtrlSmPtr->status, port));
            }
            else if ((msgType == MV_HWS_IPC_PORT_RESET_MSG) ||
                     (msgType == MV_HWS_IPC_PORT_RESET_EXT_MSG))
            {
                if (mvPortCtrlPortResetExec(port, portCtrlSmPtr, msg) != GT_OK)
                {
                    return GT_FAIL;
                }
            }
            break;

        case PORT_SM_ACTIVE_STATE:
            if ((msgType == MV_HWS_IPC_PORT_RESET_MSG) ||
                (msgType == MV_HWS_IPC_PORT_RESET_EXT_MSG))
            {
                if (mvPortCtrlPortResetExec(port, portCtrlSmPtr, msg) != GT_OK)
                {
                    return GT_FAIL;
                }
            }
            else if (msgType == MV_HWS_IPC_PORT_INIT_MSG)
            {
                mvPcPrintf("Error, Port Mng, Port-%d Init in Active state\n", port);
            }
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlPortMsgReply
*
* DESCRIPTION: Port mng message reply
*
* INPUTS:
*       GT_U32 rcode
*       GT_U8 queueId
*       GT_U8 msgType
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlPortMsgReply(GT_U32 rcode, GT_U8 queueId, GT_U8 msgType)
{
    MV_HWS_IPC_REPLY_MSG_STRUCT reply;

    reply.replyTo    = msgType;
    reply.returnCode = rcode;

    /* Build and Send IPC reply */
    CHECK_STATUS(mvHwsIpcReplyMsg(queueId, &reply));

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlPortDelay
*
* DESCRIPTION: Port mng message delay state execution
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
void mvPortCtrlPortDelay(void)
{
    infoPtr->state = PORT_MNG_DELAY_STATE;
    mvPortCtrlProcessDelay(infoPtr->delayDuration);
    infoPtr->event = PORT_MNG_MSG_PROCESS_EVENT;
}

