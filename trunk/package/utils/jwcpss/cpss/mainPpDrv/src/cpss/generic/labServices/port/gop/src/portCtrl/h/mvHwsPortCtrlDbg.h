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
* mvHwsPortCtrlDbg.h
*
* DESCRIPTION:
*       Port Control Port Debug
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlDbg_H
#define __mvHwsPortCtrlDbg_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* mvPortCtrlDebugParamSet
*
* DESCRIPTION: Debug parameter selection function
*******************************************************************************/
void mvPortCtrlDebugParamSet(GT_U32 param);

#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
/*******************************************************************************
* mvPortCtrlDebugRoutine
*
* DESCRIPTION: Debug process execution sequence
*******************************************************************************/
void mvPortCtrlDebugRoutine(void* pvParameters);

/*******************************************************************************
* mvPortCtrlDbgLock
* mvPortCtrlDbgUnlock
*
* DESCRIPTION: These two functions are used to sync print
*******************************************************************************/
GT_STATUS mvPortCtrlDbgLock(void);
GT_STATUS mvPortCtrlDbgUnlock(void);
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

/*******************************************************************************
* mvPortCtrlDbgCfgRegsDump
*
* DESCRIPTION: Config AP register dump functionality
*******************************************************************************/
void mvPortCtrlDbgCfgRegsDump(GT_U32 active, GT_U32 mode);

/*******************************************************************************
* mvPortCtrlDbgIntRegsDump
*
* DESCRIPTION: Print AP internal registers
*******************************************************************************/
void mvPortCtrlDbgIntRegsDump(GT_U8 port, GT_U8 pcsNum, GT_U8 state);

/*******************************************************************************
* mvPortCtrlDbgAllIntRegsDump
*
* DESCRIPTION: Print AP All internal registers
*******************************************************************************/
void mvPortCtrlDbgAllIntRegsDump(GT_U8 port, GT_U8 pcsNum, GT_U8 state);

/*******************************************************************************
* mvPortCtrlDbgLinkRegsDump
*
* DESCRIPTION: Print SD, CDR, and PCS Link status registers
*******************************************************************************/
void mvPortCtrlDbgLinkRegsDump(GT_U8 port, GT_U8 laneNum, GT_U8 state);

/*******************************************************************************
* mvPortCtrlDbgCtrlRegsDump
*
* DESCRIPTION: Print AP control, and status registers
*******************************************************************************/
void mvPortCtrlDbgCtrlRegsDump(GT_U8 port, GT_U8 pcsNum, GT_U8 state);

#ifndef DISABLE_CLI
/*******************************************************************************
* mvPortCtrlDbgStatsDump
*
* DESCRIPTION: Print AP protocol statistics counter and timers
*******************************************************************************/
void mvPortCtrlDbgStatsDump(GT_U8 port, GT_U8 peer);
#endif /* DISABLE_CLI */

/*******************************************************************************
* mvPortCtrlDbgReg
*
* DESCRIPTION: Configure AP register
*******************************************************************************/
void mvPortCtrlDbgReg(GT_U8 reg, GT_U8 value);

/*******************************************************************************
* mvPortCtrlDbgStatsReset
*
* DESCRIPTION: Print AP protocol statistics counter
*******************************************************************************/
void mvPortCtrlDbgStatsReset(GT_U8 port);

#ifndef DISABLE_CLI
/*******************************************************************************
* mvPortCtrlDbgIntropDump
*
* DESCRIPTION: Print AP protocol interop parameters
*******************************************************************************/
void mvPortCtrlDbgIntropDump(void);
#endif /* DISABLE_CLI */

/*******************************************************************************
* mvPortCtrlDbgIntropCfgParam
*
* DESCRIPTION: Config AP protocol interop parameters
*******************************************************************************/
void mvPortCtrlDbgIntropCfgParam(GT_U8 param, GT_U8 value);

/*******************************************************************************
* mvPortCtrlDbgIntropCfg
*
* DESCRIPTION: Config AP protocol interop parameters
*******************************************************************************/
void mvPortCtrlDbgIntropCfg(MV_HWS_AP_DETECT_ITEROP *apIntrop);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlDbg_H */

