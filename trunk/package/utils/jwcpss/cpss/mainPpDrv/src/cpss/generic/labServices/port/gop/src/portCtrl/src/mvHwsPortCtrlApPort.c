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
* mvHwsPortCtrlApPort.c
*
* DESCRIPTION:
*       Port Control AP Port State Machine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>
#include <mvHwsPortCfgIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <private/mvPortModeElements.h>

static MV_HWS_PORT_CTRL_COMMON *infoPtr;

GT_STATUS mvPortCtrlApPortPendMsg(GT_U32 port, MV_HWS_IPC_CTRL_MSG_AP_STRUCT **msg,
                                  GT_U32 *msgType, GT_U32 action);
GT_STATUS mvPortCtrlApPortPendAdd(GT_U32 port, MV_HWS_IPC_CTRL_MSG_AP_STRUCT *msg);

void mvPortCtrlPortActiveExecTraining(GT_U32 targetPort,
                                      MV_HWS_PORT_CTRL_PORT_SM *portCtrlSmPtr);

/*******************************************************************************
* mvPortCtrlApPortRoutine
*
* DESCRIPTION: AP Port mng process execution sequence
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
void mvPortCtrlApPortRoutine(void* pvParameters)
{
    mvPortCtrlSyncLock();
    mvPortCtrlSyncUnlock();

    infoPtr = &(mvHwsPortCtrlApPortMng.info);

    for( ;; )
    {
        /*
        ** AP Port Mng State Machine Transitions Table
        ** +================+=============+=============+=============+
        ** + Event \ State  +  Active O1  +   Msg O2    +  Delay O3   +
        ** +================+=============+=============+=============+
        ** + Active         +     O1      +   Invalid   +    ==> O1   +
        ** +================+=============+=============+=============+
        ** + Msg            +   ==> O2    +     O2      +   Invalid   +
        ** +================+=============+=============+=============+
        ** + Delay          +   Invalid   +   ==> O3    +   Invalid   +
        ** +================+=============+=============+=============+
        **
        **
        ** Port SM State Machine Transitions
        ** +============================+=============+=============+=============+=============+=============+
        ** + Event \ State              + Idle O0     + Serdes      + Serdes      + MAC / PCS   + Active O4   +
        ** +                            +             + Config O1   + Training O2 + Config O3   +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Start Execute              +   ==> O1    +             +             +             +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Serdes Config in Progress  +             +     O1      +             +             +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Serdes Config Success      +             +    ==> O2   +             +             +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Serdes Config Failure      +             +    ==> O0   +             +             +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Serdes Train in Progress   +             +             +     O2      +             +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Serdes Train Success       +             +             +    ==> O3   +             +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Serdes Train Failure       +             +             +    ==> O0   +             +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Serdes Train end Wait Ack  +             +             +    ==> O3   +             +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + MAC/PCS Config in Progress +             +             +             +     O3      +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + MAC/PCS Config Success     +             +             +             +    ==> O4   +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + MAC/PCS Config Failure     +             +             +             +    ==> O0   +             +
        ** +============================+=============+=============+=============+=============+=============+
        ** + Delete Success             +     O0      +    ==> O0   +    ==> O0   +    ==> O0   +    ==> O0   +
        ** +============================+=============+=============+=============+=============+=============+
        */
        if (mvHwsPortCtrlApPortMng.funcTbl[infoPtr->event][infoPtr->state] != NULL)
        {
            mvHwsPortCtrlApPortMng.funcTbl[infoPtr->event][infoPtr->state]();
        }
        else
        {
            mvPcPrintf("Error, Ap Port Ctrl, Port Func table: state[%d] event[%d] is NULL\n",
                       infoPtr->state, infoPtr->event);
        }
    }
}

/*******************************************************************************
* mvPortCtrlApPortResetExec
*
* DESCRIPTION: Port mng message processing - reset execution
*              - Execute port reset and return to Idle state
*
* INPUTS:
*       GT_U32 port
*       MV_HWS_PORT_CTRL_PORT_SM *info
*       MV_HWS_IPC_CTRL_MSG_AP_STRUCT *msg
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlApPortResetExec(GT_U32 port,
                                    MV_HWS_PORT_CTRL_PORT_SM *info,
                                    MV_HWS_IPC_CTRL_MSG_AP_STRUCT *msg)
{
    info->status = PORT_SM_DELETE_IN_PROGRESS;
    mvPortCtrlLogAdd(PORT_SM_LOG(info->state, info->status, port));

    /* Execute HWS Port Reset */
    if (mvHwsPortReset(0, /* devNum */
                       0, /* portGroup */
                       (GT_U32)msg->phyPortNum,
                       (MV_HWS_PORT_STANDARD)msg->portMode,
                       PORT_POWER_DOWN) != GT_OK)
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
* mvPortCtrlApPortActiveExec
* mvPortCtrlApPortActiveExecTrainingInProgress
* mvPortCtrlApPortActiveExecTrainingEndWaitAck
*
* DESCRIPTION: AP Port active state execution
*
*           State Description
*           - Check if Port state is Training
*               In case not in pending, exit
*
*           - Training State
*
*               Check if there are active messages in the queue
*                  Note: messages are not released until execution ends
*                  the port state defines how the message will be executed
*
*               - Training Status = In progress
*                   Check training threshold
*                     Threshold execeeded, Set result to FALSE
*                     Threshold Not execeeded
*                       Check training result
*                         Training result return fail, Set result to FALSE
*                         Training result return OK,
*                           Training In progress, Continue
*                           Training ended successfully, Set training result to TRUE
*                           Training ended with failure, Set training result to FALSE
*                     Training stop in case  Training success / failure / threshold exceeded
*                       Training stop return fail, Set result to FALSE
*
*                       Training failure / Training threshold exceeded
*                         Execute port reset sequence
*                         Training timer reset
*                         Release message
*                         Notify AP Detection Process - failure
*
*                       Training success
*                         Check for system Ack ("PIZZA" configuration)
*                           Ack received
*                             System config timer reset
*                             Release message
*                             Notify AP Detection Process - success
*                           Ack Not received
*                             System config timer start
*
*               - Training Status = Wait for system Ack
*                   Check for system Ack ("PIZZA" configuration)
*                     Ack received
*                       System config timer reset
*                       Release message
*                       Notify AP Detection Process - success
*                     Ack Not received
*                       Check system config threshold
*                         System config threshold exceeded
*                           Execute port reset sequence
*                           Training timer reset
*                           Release message
*                           Notify AP Detection Process - failure
*                         System config threshold Not exceeded
*                           Continue
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

/*******************************************************************************
* mvPortCtrlApPortActiveExecTrainingInProgress
*
*******************************************************************************/
void mvPortCtrlApPortActiveExecTrainingInProgress(GT_U32 portIndex,
                                                  MV_HWS_IPC_CTRL_MSG_AP_STRUCT *recvMsg)
{
    GT_U32                         msgType;
    MV_HWS_PORT_CTRL_PORT_SM       *portSm    = &(mvHwsPortCtrlApPortMng.apPortSm[portIndex]);
    MV_HWS_PORT_CTRL_AP_PORT_TIMER *portTimer = &(mvHwsPortCtrlApPortMng.apPortTimer[portIndex]);
    MV_HWS_PORT_CTRL_AP_INIT       *portInit  = &(mvHwsPortCtrlApPortMng.apInitStatus);
    GT_U32                         tuneResult = TUNE_NOT_COMPLITED;
    MV_HWS_AUTO_TUNE_STATUS_RES    tuneRes;

    /* Check training threshold */
    if (!(mvPortCtrlThresholdCheck(portTimer->trainingThreshold)))
    {
        return;
    }

    portTimer->trainingCount++;

    /* Execute HWS Training check */
    if (mvHwsPortAutoTuneSet(0, /* devNum */
                             0, /* portGroup */
                             (GT_U32)recvMsg->phyPortNum,
                             (MV_HWS_PORT_STANDARD)recvMsg->portMode,
                             TRxTuneStatusNonBlocking,
                             &tuneRes) == GT_OK)
    /* Execute HWS Training check */
    {
        if (tuneRes.txTune == TUNE_PASS)
        {
            /* Training success */
            tuneResult = TUNE_PASS;
        }
        else if (tuneRes.txTune == TUNE_FAIL)
        {
            /* Training failure */
            tuneResult = TUNE_FAIL;
        }
    }

    if ((tuneResult == TUNE_NOT_COMPLITED) &&
        (portTimer->trainingCount < portTimer->trainingMaxInterval))
    {
        /* Training timer restart */
        mvPortCtrlThresholdSet(PORT_CTRL_TRAINING_DURATION, &(portTimer->trainingThreshold));
        return;
    }

    /* Clear training timer */
    portTimer->trainingCount = 0;

    /* Training timer expire and training not complited */
    if (tuneResult == TUNE_NOT_COMPLITED)
    {
        /* Training failure */
        tuneResult = TUNE_FAIL;
    }

    /* Training success / failure / threshold exceeded */
    /* =============================================== */

    /* Execute HWS Training check */
    if (mvHwsPortAutoTuneSet(0, /* devNum */
                             0, /* portGroup */
                             (GT_U32)recvMsg->phyPortNum,
                             (MV_HWS_PORT_STANDARD)recvMsg->portMode,
                             TRxTuneStop,
                             NULL) != GT_OK)
    {
        /* Training failure */
        tuneResult = TUNE_FAIL;
    }

    /* Training failure / Training threshold exceeded */
    if (tuneResult == TUNE_FAIL)
    {
        portSm->status = PORT_SM_SERDES_TRAINING_FAILURE;
        mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
        mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);

        /* Training timer reset */
        mvPortCtrlThresholdSet(PORT_CTRL_TIMER_DEFAULT, &(portTimer->trainingThreshold));

        /* Mark port init failure */
        *portInit |= MV_PORT_CTRL_NUM(portIndex);

        /* Release message */
        mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
    }
    /* Training success */
    else if (tuneResult == TUNE_PASS)
    {
        portSm->status = PORT_SM_SERDES_TRAINING_END_WAIT_FOR_SYS_ACK;
        mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));

        /* Check System Config */
        if ((portTimer->sysCfState == PORT_SM_SERDES_SYSTEM_DISABLE) ||
           ((portTimer->sysCfState  == PORT_SM_SERDES_SYSTEM_ENABLE) &&
            (portTimer->sysCfStatus == PORT_SM_SERDES_SYSTEM_VALID)))
        {
            portSm->state  = PORT_SM_ACTIVE_STATE;
            portSm->status = PORT_SM_SERDES_TRAINING_SUCCESS;
            mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));

            /* Training timer reset */
            mvPortCtrlThresholdSet(PORT_CTRL_TIMER_DEFAULT, &(portTimer->trainingThreshold));

            /* Release message */
            mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
        }
        else
        {
            /* System config timer start */
            mvPortCtrlThresholdSet(PORT_CTRL_SYSTEM_CONFIG_DURATION, &(portTimer->sysCfgThreshold));
        }
    }
}

/*******************************************************************************
* mvPortCtrlApPortActiveExecTrainingEndWaitAck
*
*******************************************************************************/
void mvPortCtrlApPortActiveExecTrainingEndWaitAck(GT_U32 portIndex,
                                                  MV_HWS_IPC_CTRL_MSG_AP_STRUCT *recvMsg)
{
    GT_U32                         msgType;
    MV_HWS_PORT_CTRL_PORT_SM       *portSm    = &(mvHwsPortCtrlApPortMng.apPortSm[portIndex]);
    MV_HWS_PORT_CTRL_AP_PORT_TIMER *portTimer = &(mvHwsPortCtrlApPortMng.apPortTimer[portIndex]);
    MV_HWS_PORT_CTRL_AP_INIT       *portInit  = &(mvHwsPortCtrlApPortMng.apInitStatus);

    /* Check System Config */
    if ((portTimer->sysCfState == PORT_SM_SERDES_SYSTEM_DISABLE) ||
       ((portTimer->sysCfState  == PORT_SM_SERDES_SYSTEM_ENABLE) &&
        (portTimer->sysCfStatus == PORT_SM_SERDES_SYSTEM_VALID)))
    {
        portSm->state  = PORT_SM_ACTIVE_STATE;
        portSm->status = PORT_SM_SERDES_TRAINING_SUCCESS;

        /* System config timer reset */
        mvPortCtrlThresholdSet(PORT_CTRL_TIMER_DEFAULT, &(portTimer->sysCfgThreshold));

        /* Release message */
        mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
    }
    else
    {
        /* Check system config threshold */
        if (mvPortCtrlThresholdCheck((GT_U32)portTimer->sysCfgThreshold))
        {
            portSm->status = PORT_SM_SERDES_TRAINING_FAILURE;
            mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
            mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);

            /* System config timer reset */
            mvPortCtrlThresholdSet(PORT_CTRL_TIMER_DEFAULT, &(portTimer->sysCfgThreshold));

            /* Mark port init failure */
            *portInit |= MV_PORT_CTRL_NUM(portIndex);

            /* Release message */
            mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
        }
    }
}

/*******************************************************************************
* mvPortCtrlApPortActiveExec
*
*******************************************************************************/
void mvPortCtrlApPortActiveExec(GT_U32 portIndex)
{
    GT_U32                        msgType;
    MV_HWS_IPC_CTRL_MSG_AP_STRUCT *recvMsg = NULL;
    MV_HWS_PORT_CTRL_PORT_SM      *portSm  = &(mvHwsPortCtrlApPortMng.apPortSm[portIndex]);

    if (portSm->state != PORT_SM_SERDES_TRAINING_STATE)
    {
        return;
    }

    /* Check if there are active messages for execution */
    if (mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_PROC) != GT_OK)
    {
        /* No messages to process */
        return;
    }

    switch (portSm->status)
    {
    case PORT_SM_SERDES_TRAINING_IN_PROGRESS:
        mvPortCtrlApPortActiveExecTrainingInProgress(portIndex, recvMsg);
        break;

    case PORT_SM_SERDES_TRAINING_END_WAIT_FOR_SYS_ACK:
        mvPortCtrlApPortActiveExecTrainingEndWaitAck(portIndex, recvMsg);
        break;
    }
}

/*******************************************************************************
* mvPortCtrlApPortActive
*
* DESCRIPTION: AP Port Active state execution
*
*           State Description
*           - loop over all AP Port currently executed
*           - Trigger state change
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
void mvPortCtrlApPortActive(void)
{
    GT_U32 port;

    infoPtr->state = AP_PORT_MNG_ACTIVE_PORT_STATE;

    for (port = 0; port < MV_PORT_CTRL_MAX_AP_PORT_NUM; port++)
    {
        mvPortCtrlApPortActiveExec(port);
    }

    infoPtr->event = AP_PORT_MNG_MSG_PROCESS_EVENT;
}

/*******************************************************************************
* mvPortCtrlApPortPendExec
*
* DESCRIPTION: AP Port message state execution
*
*           State Description
*           - Check if there are new pending messages for execution
*               In case no message is pending, exit
*           - Message is pending, and Message type is RESET
*               Execute port reset sequence
*           - Message is pending, and Message type is INIT
*               Port state == Idle
*               Extract pending message for execution
*               Execute port Init
*                 Execute SERDES Configuration
*                 Execute MAC/PCS Configuration
*                 Execute training
*                   Training configuration
*                   Training start
*                   Training timeout timer
*
*              Note: current execution use current port init API which execute
*              SERDES + MAC/PCS Configuration. once available it will be seperated
*              to two API's and MAC/PCS Configuration will be executed after training ends
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
void mvPortCtrlApPortPendExec(GT_U32 portIndex)
{
    GT_U32                         msgType;
    MV_HWS_IPC_CTRL_MSG_AP_STRUCT  *recvMsg   = NULL;
    MV_HWS_PORT_CTRL_PORT_SM       *portSm    = &(mvHwsPortCtrlApPortMng.apPortSm[portIndex]);
    MV_HWS_PORT_CTRL_AP_PORT_TIMER *portTimer = &(mvHwsPortCtrlApPortMng.apPortTimer[portIndex]);
    MV_HWS_PORT_CTRL_AP_INIT       *portInit  = &(mvHwsPortCtrlApPortMng.apInitStatus);
    MV_HWS_AP_SM_INFO              *apSm      = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_PORT_FLOW_CONTROL_ENT    fcState;
    GT_U32                          fcRxPauseEn, fcTxPauseEn;
    MV_HWS_PORT_INIT_PARAMS        *curPortParams;

    /* Check if there are new pending messages for execution */
    if (mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_PEEK) != GT_OK)
    {
        /* No messages to process */
        return;
    }

    /* Handle reset message in any port execution state */
    if ((msgType == MV_HWS_IPC_PORT_RESET_MSG) ||
        (msgType == MV_HWS_IPC_PORT_RESET_EXT_MSG))
    {
        if (mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_PROC) == GT_OK)
        {
            if ((portSm->state != PORT_SM_IDLE_STATE) &&
                (portSm->status != PORT_SM_DELETE_SUCCESS))
            {
                mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);
            }

            /* Clear port init failure */
            *portInit &= ~(MV_PORT_CTRL_NUM(portIndex));

            /* Release message */
            mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
            return;
        }
    }
    /* Handle init nessage in IDLE state, any other state Ignore */
    else if (msgType == MV_HWS_IPC_PORT_INIT_MSG)
    {
        if (portSm->state == PORT_SM_IDLE_STATE)
        {
            if (mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_PROC) == GT_OK)
            {
                /* Clear port init failure */
                *portInit &= ~(MV_PORT_CTRL_NUM(portIndex));

                portSm->status = PORT_SM_START_EXECUTE;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                portSm->state  = PORT_SM_SERDES_CONFIG_STATE;
                portSm->status = PORT_SM_SERDES_CONFIG_IN_PROGRESS;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                /**PLACE Holder: HWS config SERDES*/
                if (0)
                {
                    portSm->status = PORT_SM_MAC_PCS_CONFIG_FAILURE;
                    mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                    mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);

                    /* Mark port init failure */
                    *portInit |= MV_PORT_CTRL_NUM(portIndex);

                    /* Release message */
                    mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
                    return;
                }
                portSm->status = PORT_SM_SERDES_CONFIG_SUCCESS;
                portSm->state  = PORT_SM_MAC_PCS_CONFIG_STATE;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                portSm->status = PORT_SM_MAC_PCS_CONFIG_IN_PROGRESS;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                /**PLACE Holder: HWS config MAC / PCS*/

                /* Execute HWS Port Init */
                if (mvHwsPortInit(0, /* devNum */
                                  0, /* portGroup */
                                  (GT_U32)recvMsg->phyPortNum,
                                  (MV_HWS_PORT_STANDARD)recvMsg->portMode,
                                  (GT_BOOL)recvMsg->action,
                                  (MV_HWS_REF_CLOCK_SUP_VAL)recvMsg->refClock,
                                  (MV_HWS_REF_CLOCK_SOURCE)recvMsg->refClockSource) != GT_OK)
                {
                    mvPcPrintf("Error, Port Mng, Port-%d Init Failed\n", recvMsg->phyPortNum);
                    portSm->status = PORT_SM_MAC_PCS_CONFIG_FAILURE;
                    mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                    mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);

                    /* Mark port init failure */
                    *portInit |= MV_PORT_CTRL_NUM(portIndex);

                    /* Release message */
                    mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
                    return;
                }
                portSm->status = PORT_SM_MAC_PCS_CONFIG_SUCCESS;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));

                portSm->status = PORT_SM_FEC_CONFIG_IN_PROGRESS;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                curPortParams = hwsPortModeParamsGet(0, 0,
                                                    (GT_U32)recvMsg->phyPortNum, 
                                                    (MV_HWS_PORT_STANDARD)recvMsg->portMode);
                if(mvHwsPcsFecConfig(0, /* devNum */
                                     0, /* portGroup */
                                     curPortParams->portPcsNumber,
                                     curPortParams->portPcsType,
                                     ((1 == AP_ST_HCD_FEC_RES_GET(apSm->hcdStatus)) ? GT_TRUE : GT_FALSE)) != GT_OK)
                {
                    mvPcPrintf("Error, Port Mng, Port-%d FEC config Failed\n", recvMsg->phyPortNum);
                    portSm->status = PORT_SM_FEC_CONFIG_FAILURE;
                    mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                    mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);

                    /* Mark port init failure */
                    *portInit |= MV_PORT_CTRL_NUM(portIndex);

                    /* Release message */
                    mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
                    return;
                }
                portSm->status = PORT_SM_FEC_CONFIG_SUCCESS;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));

                portSm->status = PORT_SM_FC_STATE_SET_IN_PROGRESS;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                fcRxPauseEn = AP_ST_HCD_FC_RX_RES_GET(apSm->hcdStatus);
                fcTxPauseEn = AP_ST_HCD_FC_TX_RES_GET(apSm->hcdStatus);
                if (fcRxPauseEn && fcTxPauseEn)
                {
                    fcState = MV_HWS_PORT_FLOW_CONTROL_RX_TX_E;
                }
                else if(fcRxPauseEn)
                {
                    fcState = MV_HWS_PORT_FLOW_CONTROL_RX_ONLY_E;
                }
                else if(fcTxPauseEn)
                {
                    fcState = MV_HWS_PORT_FLOW_CONTROL_TX_ONLY_E;
                }
                else
                {
                    fcState = MV_HWS_PORT_FLOW_CONTROL_DISABLE_E;
                }
                if (mvHwsPortFlowControlStateSet(0, /* devNum */
                                                 0, /* portGroup */
                                                 (GT_U32)recvMsg->phyPortNum,
                                                 (MV_HWS_PORT_STANDARD)recvMsg->portMode,
                                                 fcState) != GT_OK)
                {
                    mvPcPrintf("Error, Port Mng, Port-%d FC state set Failed\n", recvMsg->phyPortNum);
                    portSm->status = PORT_SM_FC_STATE_SET_FAILURE;
                    mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                    mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);

                    /* Mark port init failure */
                    *portInit |= MV_PORT_CTRL_NUM(portIndex);

                    /* Release message */
                    mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
                    return;
                }
                portSm->status = PORT_SM_FC_STATE_SET_SUCCESS;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));

                portSm->state  = PORT_SM_SERDES_TRAINING_STATE;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                portSm->status = PORT_SM_SERDES_TRAINING_IN_PROGRESS;
                mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));

                /* Execute HWS Training config */
                if (HWS_DEV_SILICON_TYPE(0) != BobK)
                {
                    if (mvHwsPortAutoTuneSet(0, /* devNum */
                                             0, /* portGroup */
                                             (GT_U32)recvMsg->phyPortNum,
                                             (MV_HWS_PORT_STANDARD)recvMsg->portMode,
                                             TRxTuneCfg,
                                             0) != GT_OK)
                    {
                        mvPcPrintf("Error, Port Mng, Port-%d training config Failed\n", recvMsg->phyPortNum);
                        portSm->status = PORT_SM_SERDES_TRAINING_CONFIG_FAILURE;
                        mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                        mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);

                        /* Mark port init failure */
                        *portInit |= MV_PORT_CTRL_NUM(portIndex);

                        /* Release message */
                        mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
                        return;
                    }
                }

                /* Execute HWS Training start */
                if (mvHwsPortAutoTuneSet(0, /* devNum */
                                         0, /* portGroup */
                                         (GT_U32)recvMsg->phyPortNum,
                                         (MV_HWS_PORT_STANDARD)recvMsg->portMode,
                                         TRxTuneStart,
                                         0) != GT_OK)
                {
                    mvPcPrintf("Error, Port Mng, Port-%d training start Failed\n", recvMsg->phyPortNum);
                    portSm->status = PORT_SM_SERDES_TRAINING_FAILURE;
                    mvPortCtrlLogAdd(AP_PORT_MNG_LOG(portSm->state, portSm->status, recvMsg->phyPortNum));
                    mvPortCtrlApPortResetExec((GT_U32)recvMsg->phyPortNum, portSm, recvMsg);

                    /* Mark port init failure */
                    *portInit |= MV_PORT_CTRL_NUM(portIndex);

                    /* Release message */
                    mvPortCtrlApPortPendMsg(portIndex, &recvMsg, &msgType, PORT_MSG_FREE);
                    return;
                }

                /* Training timer start */
                mvPortCtrlThresholdSet(PORT_CTRL_TRAINING_DURATION, &(portTimer->trainingThreshold));
            }
        }
    }
}

/*******************************************************************************
* mvPortCtrlApPortMsg
*
* DESCRIPTION: AP Port message state execution
*
*           State Description
*           - loop over all AP Port pending queues, and trigger queue execution
*           - Trigger state change
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
void mvPortCtrlApPortMsg(void)
{
    GT_U32 port;

    infoPtr->state = AP_PORT_MNG_MSG_PROCESS_STATE;

    for (port = 0; port < MV_PORT_CTRL_MAX_AP_PORT_NUM; port++)
    {
        mvPortCtrlApPortPendExec(port);
    }

    infoPtr->event = AP_PORT_MNG_DELAY_EVENT;
}

/*******************************************************************************
* mvPortCtrlApPortDelay
*
* DESCRIPTION: AP Port delay state execution
*
*           State Description
*           - Execute delay
*           - Trigger state change
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
void mvPortCtrlApPortDelay(void)
{
    infoPtr->state = AP_PORT_MNG_PORT_DELAY_STATE;
    mvPortCtrlProcessDelay(mvHwsPortCtrlApPortMng.info.delayDuration);
    infoPtr->event = AP_PORT_MNG_ACTIVE_PORT_EVENT;
}

/*******************************************************************************
* mvPortCtrlApPortPendAdd
*
* DESCRIPTION: The functions add an IPC message to the port pending tasks
*              message queue
*              It validate if there is free space in the queue, and in case
*              valid it adds the message to the the queue
*
* INPUTS:
*       GT_U32                        targetPort
*       MV_HWS_IPC_CTRL_MSG_AP_STRUCT *msg
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlApPortPendAdd(GT_U32 port, MV_HWS_IPC_CTRL_MSG_AP_STRUCT *msg)
{
    GT_U8 nextPendTaskAddIndex;
    MV_HWS_PORT_CTRL_PEND_MSG_TBL *portCtrlSmPendMsgPtr;

    portCtrlSmPendMsgPtr = &(mvHwsPortCtrlApPortMng.tasks[port]);

    if (((portCtrlSmPendMsgPtr->pendTasksAddIndex + 1) != portCtrlSmPendMsgPtr->pendTasksProcIndex) &&
         (portCtrlSmPendMsgPtr->pendTasksCount < MV_PORT_CTRL_PORT_PEND_MSG_NUM))
    {
        nextPendTaskAddIndex = portCtrlSmPendMsgPtr->pendTasksAddIndex + 1;
        /* Roll over */
        if (nextPendTaskAddIndex == MV_PORT_CTRL_PORT_PEND_MSG_NUM)
        {
            nextPendTaskAddIndex = 0;
        }

        mvPortCtrlSyncLock();
        osMemCpy(&(portCtrlSmPendMsgPtr->pendTasks[nextPendTaskAddIndex]), msg, sizeof(MV_HWS_IPC_CTRL_MSG_AP_STRUCT));
        portCtrlSmPendMsgPtr->pendTasksCount++;
        portCtrlSmPendMsgPtr->pendTasksAddIndex = nextPendTaskAddIndex;
        mvPortCtrlSyncUnlock();
    }
    else
    {
        mvPcPrintf("Error, AP Port Mng, Port Index-%d Add message failed!\n", port);
        return GT_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
**
**  mvPortCtrlApPortPendMsg
**  ____________________________________________________________________________
**
* DESCRIPTION: The functions return IPC message waiting in the port pending tasks
*              message queue for execution
*              The message is not release to the queue until it is executed
*
* INPUTS:
*       GT_U32                        port
*       MV_HWS_IPC_CTRL_MSG_AP_STRUCT *msg
*       GT_U32                        *msgType
*       GT_U32                        action - PEEK, PROC, FREE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
**
*******************************************************************************/
GT_STATUS mvPortCtrlApPortPendMsg(GT_U32 port, MV_HWS_IPC_CTRL_MSG_AP_STRUCT **msg,
                                  GT_U32 *msgType, GT_U32 action)
{
    GT_U8 nextPendTasksIndex;
    MV_HWS_PORT_CTRL_PEND_MSG_TBL *portCtrlSmPendMsgPtr;

    portCtrlSmPendMsgPtr = &(mvHwsPortCtrlApPortMng.tasks[port]);

    if ((portCtrlSmPendMsgPtr->pendTasksProcIndex != portCtrlSmPendMsgPtr->pendTasksAddIndex) ||
        ((portCtrlSmPendMsgPtr->pendTasksProcIndex == portCtrlSmPendMsgPtr->pendTasksAddIndex) &&
         (portCtrlSmPendMsgPtr->pendTasksCount == MV_PORT_CTRL_PORT_PEND_MSG_NUM)))
    {
        nextPendTasksIndex = portCtrlSmPendMsgPtr->pendTasksProcIndex + 1;
        /* Roll over */
        if (nextPendTasksIndex == MV_PORT_CTRL_PORT_PEND_MSG_NUM)
        {
            nextPendTasksIndex = 0;
        }

        /* Return message type */
        if (action == PORT_MSG_PEEK)
        {
            *msgType = (GT_U32)portCtrlSmPendMsgPtr->pendTasks[nextPendTasksIndex].ctrlMsgType;
        }
        /* Return message pointer */
        else if (action == PORT_MSG_PROC)
        {
            *msg = &(portCtrlSmPendMsgPtr->pendTasks[nextPendTasksIndex]);
        }
        /* Release message */
        else if (action == PORT_MSG_FREE)
        {
            mvPortCtrlSyncLock();
            portCtrlSmPendMsgPtr->pendTasksProcIndex = nextPendTasksIndex;
            portCtrlSmPendMsgPtr->pendTasksCount--;
            mvPortCtrlSyncUnlock();
        }

        return GT_OK;
    }

    return GT_ERROR;
}

/*******************************************************************************
* mvPortCtrlApPortMsgSend
*
* DESCRIPTION: The functions build and send IPC message to AP Port init queue
*
* INPUTS:
*       GT_U8 portIndex
*       GT_U msgType
*       GT_U8 queueId
*       GT_U8 portNum
*       GT_U8 portMode
*       GT_U8 lbPort
*       GT_U8 refClk
*       GT_U8 refClkSrc
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlApPortMsgSend(GT_U8 portIndex, GT_U8 msgType, GT_U8 queueId,
                                  GT_U8 portNum, GT_U8 portMode, GT_U8 action,
                                  GT_U8 refClk, GT_U8 refClkSrc)
{
    MV_HWS_IPC_CTRL_MSG_AP_STRUCT apIpcMsg;

    apIpcMsg.ctrlMsgType    = msgType;
    apIpcMsg.msgQueueId     = queueId;
    apIpcMsg.phyPortNum     = portNum;
    apIpcMsg.portMode       = portMode;
    apIpcMsg.action         = action;
    apIpcMsg.refClock       = refClk;
    apIpcMsg.refClockSource = refClkSrc;

    mvPortCtrlApPortPendAdd(portIndex, &apIpcMsg);

    return GT_OK;
}

