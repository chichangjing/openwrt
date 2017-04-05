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
* mvHwsPortCtrlPort.h
*
* DESCRIPTION:
*       Port Control Port Initialization
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlPort_H
#define __mvHwsPortCtrlPort_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Message processing actions */
#define PORT_MSG_PEEK (0)
#define PORT_MSG_PROC (1)
#define PORT_MSG_FREE (2)


/* Regular Port Management API */
/* =========================== */

/*******************************************************************************
* mvPortCtrlPortRoutine
*
* DESCRIPTION: Port mng process execution sequence
*******************************************************************************/
void mvPortCtrlPortRoutine(void* pvParameters);

/*******************************************************************************
* mvPortCtrlPortMsg
*
* DESCRIPTION: Port mng message processing state execution
*******************************************************************************/
void mvPortCtrlPortMsg(void);

/*******************************************************************************
* mvPortCtrlPortMsgExec
*
* DESCRIPTION: Port mng message processing state execution
*******************************************************************************/
GT_STATUS mvPortCtrlPortMsgExec(GT_U32 port,
                                MV_HWS_IPC_CTRL_MSG_STRUCT *msg);

/*******************************************************************************
* mvPortCtrlPortMsgReply
*
* DESCRIPTION: Port mng message reply
*******************************************************************************/
GT_STATUS mvPortCtrlPortMsgReply(GT_U32 rcode, GT_U8 queueId, GT_U8 msgType);

/*******************************************************************************
* mvPortCtrlPortDelay
*
* DESCRIPTION: Port mng message delay state execution
*******************************************************************************/
void mvPortCtrlPortDelay(void);

/* AP/SD Port Management API */
/* ========================= */

/*******************************************************************************
* mvPortCtrlApPortRoutine
*
* DESCRIPTION: AP Port mng process execution sequence
*******************************************************************************/
void mvPortCtrlApPortRoutine(void* pvParameters);

/*******************************************************************************
* mvPortCtrlApPortMsg
*
* DESCRIPTION: AP Port message state execution
*******************************************************************************/
void mvPortCtrlApPortMsg(void);

/*******************************************************************************
* mvPortCtrlApPortActive
*
* DESCRIPTION: AP Port active state execution
*******************************************************************************/
void mvPortCtrlApPortActive(void);

/*******************************************************************************
* mvPortCtrlApPortDelay
*
* DESCRIPTION: AP Port delay state execution
*******************************************************************************/
void mvPortCtrlApPortDelay(void);

#ifndef DISABLE_CLI
/*******************************************************************************
* mvPortCtrlApPortStatus
*
* DESCRIPTION: AP Port state & status show
*******************************************************************************/
void mvPortCtrlApPortStatus(void);
#endif /* DISABLE_CLI */

/* Time & Threshold  API */
/* ====================== */

/*******************************************************************************
* mvPortCtrlCurrentTs
*
* DESCRIPTION: Return current time stamp
*******************************************************************************/
GT_U32 mvPortCtrlCurrentTs(void);

/*******************************************************************************
* mvPortCtrlThresholdSet
*
* DESCRIPTION: Set threshold for port
*******************************************************************************/
void mvPortCtrlThresholdSet(GT_U32 delay, GT_U32 *thrershold);

/*******************************************************************************
* mvPortCtrlThresholdCheck
*
* DESCRIPTION: Check if port threshold exceeded
*******************************************************************************/
GT_U32 mvPortCtrlThresholdCheck(GT_U32 thrershold);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlPort_H */

