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
* mvHwsPortCtrlLog.h
*
* DESCRIPTION:
*       Port Control Log
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlLog_H
#define __mvHwsPortCtrlLog_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* mvPortCtrlLogInit
*
* DESCRIPTION: Initialize and clear real time log
*******************************************************************************/
GT_STATUS mvPortCtrlLogInit(void);

/*******************************************************************************
* mvPortCtrlLogInfoGet
*
* DESCRIPTION: Return real time log info
*******************************************************************************/
void mvPortCtrlLogInfoGet(GT_U32 *fwBaseAddr, GT_U32 *logBaseAddr, GT_U32 *logPointer,
                          GT_U32 *logCount, GT_U32 *logReset);

/*******************************************************************************
* mvPortCtrlLogAdd
*
* DESCRIPTION: Add Log entry to the Log and override oldest Log entry in case
*              the Log is full
*******************************************************************************/
void mvPortCtrlLogAdd(GT_U32 entry);

/*******************************************************************************
* mvPortCtrlLogDump
*
* DESCRIPTION: Print Log content according to Log count
*              The order of print is from the oldest to the newest message
*******************************************************************************/
void mvPortCtrlLogDump(GT_U32 port);

/*******************************************************************************
* mvPortCtrlLogLock
* mvPortCtrlLogUnlock
*
* DESCRIPTION: These two functions are used to protect real time log
*******************************************************************************/
GT_STATUS mvPortCtrlLogLock(void);
GT_STATUS mvPortCtrlLogUnlock(void);

/*******************************************************************************
* mvPortCtrlIpcLock
* mvPortCtrlIpcUnlock
*
* DESCRIPTION: These two functions are used to protect IPC
*******************************************************************************/
GT_STATUS mvPortCtrlIpcLock(void);
GT_STATUS mvPortCtrlIpcUnlock(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlLog_H */
