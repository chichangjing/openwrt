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
* mvHwsPortCtrlGeneral.c
*
* DESCRIPTION:
*       Port Control General State Machine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>
#include <private/mvHwsPortMiscIf.h>
#include <private/mvPortModeElements.h>
#include <mvHwsPortCfgIf.h>

static MV_HWS_PORT_CTRL_COMMON *infoPtr;

/*******************************************************************************
* mvPortCtrlGenRoutine
*
* DESCRIPTION: General process execution sequence
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
void mvPortCtrlGenRoutine(void* pvParameters)
{
    mvPortCtrlSyncLock();
    mvPortCtrlSyncUnlock();

    infoPtr = &(mvPortCtrlGen.info);

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
        if (mvPortCtrlGen.funcTbl[infoPtr->event][infoPtr->state] != NULL)
        {
            mvPortCtrlGen.funcTbl[infoPtr->event][infoPtr->state]();
        }
        else
        {
            mvPcPrintf("Error, Port Ctrl, General Func table: state[%d] event[%d] is NULL\n",
                       infoPtr->state, infoPtr->event);
        }
    }
}

/*******************************************************************************
* mvPortCtrlGenMsgProcess
*
* DESCRIPTION: General message process state execution
*              - Read message from message queue
*                  Message received
*                    Add Log entry
*                    Execute received message processing
*              - Message not received
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
void mvPortCtrlGenMsgProcess(void)
{
    GT_STATUS rcode;
    GT_U32 targetPort;
    MV_HWS_IPC_REPLY_MSG_STRUCT                         reply;
    MV_HWS_IPC_CTRL_MSG_STRUCT                          recvMsg;
    MV_HWS_IPC_PORT_INFO_STRUCT                         *infoMsg;
    MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_DATA_STRUCT       *tuneMsg;
    MV_HWS_IPC_PORT_AUTO_TUNE_STOP_DATA_STRUCT          *tuneStopMsg;
    MV_HWS_IPC_PORT_SERDES_MANUAL_RX_CONFIG_DATA_STRUCT *manRxCfgMsg;
    MV_HWS_IPC_PORT_SERDES_MANUAL_TX_CONFIG_DATA_STRUCT *manTxCfgMsg;
    MV_HWS_IPC_PORT_POLARITY_SET_DATA_STRUCT            *polMsg;
    MV_HWS_IPC_PORT_FEC_CONFIG_DATA_STRUCT              *fecMsg;
    MV_HWS_IPC_PORT_TX_ENABLE_DATA_STRUCT               *txMsg;
    MV_HWS_IPC_PORT_LOOPBACK_SET_DATA_STRUCT            *lbSetMsg;
    MV_HWS_IPC_PORT_LOOPBACK_GET_DATA_STRUCT            *lbGetMsg;
    MV_HWS_IPC_PORT_PPM_SET_DATA_STRUCT                 *ppmMsg;
    MV_HWS_IPC_PORT_IF_GET_DATA_STRUCT                  *ifMsg;
    MV_HWS_IPC_PORT_PCS_ACTIVE_STATUS_GET_DATA_STRUCT   *pcsActiveMsg;
    MV_HWS_IPC_PORT_PARMAS_DATA_STRUCT                  *paramsMsg;
    MV_HWS_IPC_PORT_FLOW_CONTROL_SET_DATA_STRUCT        *portFcStateSetMsg;

    MV_HWS_PORT_INIT_PARAMS  portParams;
    GT_U32 activeLanesList[MV_HWS_MAX_LANES_NUM_PER_PORT];
    GT_U32 i;

    infoPtr->state = PORT_GEN_MSG_PROCESS_STATE;

    /* Process messages from General message queue */
    rcode = mvPortCtrlProcessMsgRecv(M6_GENERAL, MV_PROCESS_MSG_RX_NO_DELAY, &recvMsg);
    if (rcode != GT_OK)
    {
        infoPtr->event = PORT_GEN_DELAY_EVENT;
    }
    else
    {
        /* Check that the message is valid: msg type and queueId are in the defined range */
        if ((recvMsg.ctrlMsgType >= MV_HWS_IPC_LAST_CTRL_MSG_TYPE) ||
            (recvMsg.msgQueueId  >= MV_HWS_MAX_HWS2HOST_REPLY_QUEUE_NUM) &&
            (recvMsg.msgQueueId  != MV_PORT_CTRL_NO_MSG_REPLY))
        {
            reply.returnCode = GT_BAD_PARAM;
            /* Build and Send IPC reply */
            mvHwsIpcReplyMsg(recvMsg.msgQueueId, &reply);
            return;
        }

        reply.replyTo = recvMsg.ctrlMsgType;

        /* Decodes received IPC message */
        switch (recvMsg.ctrlMsgType)
        {
        case MV_HWS_IPC_PORT_PARAMS_MSG:
        {
            targetPort = recvMsg.msgData.portParams.phyPortNum;
            paramsMsg  = &recvMsg.msgData.portParams;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));

            /* since some of data type don't match between structures (such as active Serdes)
            ** the copy should be param-by-param (instead of memcpy) */
            portParams.portStandard = paramsMsg->portStandard;
            portParams.portMacType = paramsMsg->portMacType;
            portParams.portMacNumber = paramsMsg->portMacNumber;
            portParams.portPcsType = paramsMsg->portPcsType;
            portParams.portPcsNumber = paramsMsg->portMacNumber;
            portParams.portFecMode = paramsMsg->portFecMode;
            portParams.serdesSpeed = paramsMsg->serdesSpeed;
            portParams.firstLaneNum = paramsMsg->firstLaneNum;
            portParams.numOfActLanes = (GT_U32)paramsMsg->numOfActLanes;

            /* copy Serdes list */
            portParams.activeLanesList = activeLanesList;
            for(i = 0; i< paramsMsg->numOfActLanes; i++)
            {
                portParams.activeLanesList[i] = (GT_U32)paramsMsg->activeLanesList[i];
            }
            portParams.serdesMediaType = paramsMsg->serdesMediaType;
            portParams.serdes10BitStatus = paramsMsg->serdes10BitStatus;

            reply.returnCode = hwsPortModeParamsSet(recvMsg.devNum,
                                                    paramsMsg->portGroup,
                                                    paramsMsg->phyPortNum,
                                                    &portParams);
            break;
        }
        case MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_MSG:
        {
            targetPort =  recvMsg.msgData.portAutoTuneSetExt.phyPortNum;
            tuneMsg    = &recvMsg.msgData.portAutoTuneSetExt;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortAutoTuneSetExt(recvMsg.devNum,
                                                       tuneMsg->portGroup,
                                                       tuneMsg->phyPortNum,
                                                       tuneMsg->portMode,
                                                       tuneMsg->portTuningMode,
                                                       tuneMsg->optAlgoMask,
                                                       &reply.readData.portReplyGet.results);
            if (reply.returnCode != GT_OK)
            {
                mvPortCtrlLogAdd(PORT_GENERAL_LOG_RES(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType,
                                                      targetPort, LOG_RESULT_MASK, MV_GEN_TUNE_FAIL));
                mvPcPrintf("Error, Port Mng, Port-%d Rx Training Failed\n", targetPort);
            }
            else
            {
                mvPortCtrlLogAdd(PORT_GENERAL_LOG_RES(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType,
                                                      targetPort, LOG_RESULT_MASK, MV_GEN_TUNE_PASS));
            }
            break;
        }
        case MV_HWS_IPC_PORT_AUTO_TUNE_STOP_MSG:
        {
            targetPort  =  recvMsg.msgData.portAutoTuneStop.phyPortNum;
            tuneStopMsg = &recvMsg.msgData.portAutoTuneStop;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortAutoTuneStop(recvMsg.devNum,
                                                     tuneStopMsg->portGroup,
                                                     tuneStopMsg->phyPortNum,
                                                     tuneStopMsg->portMode,
                                                     tuneStopMsg->stopRx,
                                                     tuneStopMsg->stopTx);
            break;
        }
        case MV_HWS_IPC_PORT_AUTO_TUNE_STATE_CHK_MSG:
        {
            targetPort =  recvMsg.msgData.portAutoTuneStateChk.phyPortNum;
            infoMsg    = &recvMsg.msgData.portAutoTuneStateChk;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortAutoTuneStateCheck(recvMsg.devNum,
                                                           infoMsg->portGroup,
                                                           infoMsg->phyPortNum,
                                                           infoMsg->portMode,
                                                           &reply.readData.portAutoTuneStateChk.rxTune,
                                                           &reply.readData.portAutoTuneStateChk.txTune);
            break;
        }
        case MV_HWS_IPC_PORT_SERDES_MANUAL_RX_CONFIG_MSG:
        {
            manRxCfgMsg = &recvMsg.msgData.serdesManualRxConfig;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, 0x7F));
            reply.returnCode = mvHwsSerdesManualRxConfig(recvMsg.devNum,
                                                         manRxCfgMsg->portGroup,
                                                         manRxCfgMsg->serdesNum,
                                                         manRxCfgMsg->serdesType,
                                                         manRxCfgMsg->portTuningMode,
                                                         manRxCfgMsg->sqlch,
                                                         manRxCfgMsg->ffeRes,
                                                         manRxCfgMsg->ffeCap,
                                                         manRxCfgMsg->dfeEn,
                                                         manRxCfgMsg->alig);
            break;
        }
        case MV_HWS_IPC_PORT_SERDES_MANUAL_TX_CONFIG_MSG:
        {
            manTxCfgMsg = &recvMsg.msgData.serdesManualTxConfig;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, 0x7F));
            reply.returnCode = mvHwsSerdesManualTxConfig(recvMsg.devNum,
                                                         manTxCfgMsg->portGroup,
                                                         manTxCfgMsg->serdesNum,
                                                         manTxCfgMsg->serdesType,
                                                         manTxCfgMsg->txAmp,
                                                         manTxCfgMsg->txAmpAdj,
                                                         manTxCfgMsg->emph0,
                                                         manTxCfgMsg->emph1,
                                                         manTxCfgMsg->txAmpShft);
            break;
        }
        case MV_HWS_IPC_PORT_POLARITY_SET_MSG:
        {
            targetPort =  recvMsg.msgData.portPolaritySet.phyPortNum;
            polMsg     = &recvMsg.msgData.portPolaritySet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortPolaritySet(recvMsg.devNum,
                                                    polMsg->portGroup,
                                                    polMsg->phyPortNum,
                                                    polMsg->portMode,
                                                    polMsg->txInvMask,
                                                    polMsg->rxInvMask);
            break;
        }
        case MV_HWS_IPC_PORT_FEC_CONFIG_MSG:
        {
            targetPort =  recvMsg.msgData.portFecConfig.phyPortNum;
            fecMsg     = &recvMsg.msgData.portFecConfig;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortFecCofig(recvMsg.devNum,
                                                 fecMsg->portGroup,
                                                 fecMsg->phyPortNum,
                                                 fecMsg->portMode,
                                                 fecMsg->portFecEn);
            break;
        }
        case MV_HWS_IPC_PORT_FEC_CONFIG_GET_MSG:
        {
            targetPort =  recvMsg.msgData.portFecConfigGet.phyPortNum;
            infoMsg    = &recvMsg.msgData.portFecConfigGet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortFecCofigGet(recvMsg.devNum,
                                                    infoMsg->portGroup,
                                                    infoMsg->phyPortNum,
                                                    infoMsg->portMode,
                                                    &reply.readData.portStatusGet.status);
            break;
        }
        case MV_HWS_IPC_PORT_LINK_STATUS_GET_MSG:
        {
            targetPort =  recvMsg.msgData.portLinkStatus.phyPortNum;
            infoMsg    = &recvMsg.msgData.portLinkStatus;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortLinkStatusGet(recvMsg.devNum,
                                                      infoMsg->portGroup,
                                                      infoMsg->phyPortNum,
                                                      infoMsg->portMode,
                                                      &reply.readData.portStatusGet.status);
            if (reply.returnCode == GT_OK)
            {
                if (reply.readData.portStatusGet.status == GT_FALSE)
                {
                    mvPortCtrlLogAdd(PORT_GENERAL_LOG_RES(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType,
                                                          targetPort, LOG_RESULT_MASK, MV_GEN_LINK_DOWN));
                    mvPcPrintf("Port-%d Link Down\n", targetPort);
                }
                else
                {
                    mvPortCtrlLogAdd(PORT_GENERAL_LOG_RES(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType,
                                                          targetPort, LOG_RESULT_MASK, MV_GEN_LINK_UP));
                    mvPcPrintf("Port-%d Link Up\n", targetPort);
                }
            }
            else
            {
                mvPcPrintf("Error, Port Mng, Port-%d Link Get Failed\n", targetPort);
            }
            break;
        }
        case MV_HWS_IPC_PORT_TX_ENABLE_MSG:
        {
            targetPort =  recvMsg.msgData.portTxEnableData.phyPortNum;
            txMsg      = &recvMsg.msgData.portTxEnableData;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortTxEnable(recvMsg.devNum,
                                                 txMsg->portGroup,
                                                 txMsg->phyPortNum,
                                                 txMsg->portMode,
                                                 txMsg->enable);
            break;
        }
        case  MV_HWS_IPC_PORT_TX_ENABLE_GET_MSG:
        {
            targetPort =  recvMsg.msgData.portTxEnableGet.phyPortNum;
            infoMsg    = &recvMsg.msgData.portTxEnableGet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortTxEnableGet(recvMsg.devNum,
                                                    infoMsg->portGroup,
                                                    infoMsg->phyPortNum,
                                                    infoMsg->portMode,
                                                    &reply.readData.portSerdesTxEnableGet.status[0]);
            break;
        }
        case MV_HWS_IPC_PORT_LOOPBACK_SET_MSG:
        {
            targetPort =  recvMsg.msgData.portLoopbackSet.phyPortNum;
            lbSetMsg   = &recvMsg.msgData.portLoopbackSet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortLoopbackSet(recvMsg.devNum,
                                                    lbSetMsg->portGroup,
                                                    lbSetMsg->phyPortNum,
                                                    lbSetMsg->portMode,
                                                    lbSetMsg->lpPlace,
                                                    lbSetMsg->lbType);
            break;
        }
        case  MV_HWS_IPC_PORT_LOOPBACK_STATUS_GET_MSG:
        {
            targetPort =  recvMsg.msgData.portLoopbackGet.phyPortNum;
            lbGetMsg   = &recvMsg.msgData.portLoopbackGet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortLoopbackStatusGet(recvMsg.devNum,
                                                          lbGetMsg->portGroup,
                                                          lbGetMsg->phyPortNum,
                                                          lbGetMsg->portMode,
                                                          lbGetMsg->lpPlace,
                                                          &reply.readData.portLoopbackStatusGet.lbType);
            break;
        }
        case MV_HWS_IPC_PORT_PPM_SET_MSG:
        {
            targetPort =  recvMsg.msgData.portPPMSet.phyPortNum;
            ppmMsg     = &recvMsg.msgData.portPPMSet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortPPMSet(recvMsg.devNum,
                                               ppmMsg->portGroup,
                                               ppmMsg->phyPortNum,
                                               ppmMsg->portMode,
                                               ppmMsg->portPPM);
            break;
        }
        case  MV_HWS_IPC_PORT_PPM_GET_MSG:
        {
            targetPort =  recvMsg.msgData.portPPMGet.phyPortNum;
            infoMsg    = &recvMsg.msgData.portPPMGet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortPPMGet(recvMsg.devNum,
                                               infoMsg->portGroup,
                                               infoMsg->phyPortNum,
                                               infoMsg->portMode,
                                               &reply.readData.portPpmGet.portPpm);
            break;
        }
        case  MV_HWS_IPC_PORT_IF_GET_MSG:
        {
            targetPort =  recvMsg.msgData.portInterfaceGet.phyPortNum;
            ifMsg      = &recvMsg.msgData.portInterfaceGet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortInterfaceGet(recvMsg.devNum,
                                                     ifMsg->portGroup,
                                                     ifMsg->phyPortNum,
                                                     &reply.readData.portIfGet.portIf);
            break;
        }
        case MV_HWS_IPC_PORT_SIGNAL_DETECT_GET_MSG:
        {
            targetPort =  recvMsg.msgData.portSignalDetectGet.phyPortNum;
            infoMsg    = &recvMsg.msgData.portSignalDetectGet;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortSignalDetectGet(recvMsg.devNum,
                                                        infoMsg->portGroup,
                                                        infoMsg->phyPortNum,
                                                        infoMsg->portMode,
                                                        &reply.readData.portSerdesSignalDetectGet.status[0]);
            break;
        }
        case MV_HWS_IPC_PORT_CDR_LOCK_STATUS_GET_MSG:
        {
            targetPort =  recvMsg.msgData.portCdrLockStatus.phyPortNum;
            infoMsg    = &recvMsg.msgData.portCdrLockStatus;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortCdrLockStatusGet(recvMsg.devNum,
                                                         infoMsg->portGroup,
                                                         infoMsg->phyPortNum,
                                                         infoMsg->portMode,
                                                         &reply.readData.portSerdesCdrLockStatusGet.status[0]);
            break;
        }
        case MV_HWS_IPC_PORT_PCS_ACTIVE_STATUS_GET_CONFIG_MSG:
        {
            targetPort   =  recvMsg.msgData.portPcsActiveStatus.phyPortNum;
            pcsActiveMsg = &recvMsg.msgData.portPcsActiveStatus;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortPcsActiveStatusGet(recvMsg.devNum,
                                                           pcsActiveMsg->portGroup,
                                                           pcsActiveMsg->phyPortNum,
                                                           pcsActiveMsg->portMode,
                                                           &reply.readData.portReplyGet.results);
            break;
        }
        case MV_HWS_IPC_PORT_FC_STATE_SET_MSG:
        {
            portFcStateSetMsg   = &recvMsg.msgData.portFcStateSet;
            targetPort =  portFcStateSetMsg->phyPortNum;
            mvPortCtrlLogAdd(PORT_GENERAL_LOG(PORT_GEN_MSG_PROCESS_STATE, recvMsg.ctrlMsgType, targetPort));
            reply.returnCode = mvHwsPortFlowControlStateSet(recvMsg.devNum,
                                                            portFcStateSetMsg->portGroup,
                                                            portFcStateSetMsg->phyPortNum,
                                                            portFcStateSetMsg->portMode,
                                                            portFcStateSetMsg->fcState);
            break;
        }
        default:
            reply.returnCode = GT_NOT_SUPPORTED;
        }

        /* Build and Send IPC reply */
        mvHwsIpcReplyMsg(recvMsg.msgQueueId, &reply);
    }
}

/*******************************************************************************
* mvPortCtrlGenDelay
*
* DESCRIPTION: General message delay state execution
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
void mvPortCtrlGenDelay(void)
{
    infoPtr->state = PORT_GEN_DELAY_STATE;
    mvPortCtrlProcessDelay(infoPtr->delayDuration);
    infoPtr->event = PORT_GEN_MSG_EVENT;
}

