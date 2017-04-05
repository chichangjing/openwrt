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
* mvHwsPortCtrlDb.h
*
* DESCRIPTION:
*       Port Control Database
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlDb_H
#define __mvHwsPortCtrlDb_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Port Control function pointer definition   */
typedef void (*PORT_CTRL_FUNCPTR)(void);

/* Port Control common table definition */
typedef struct
{
    GT_U8 state;
    GT_U8 event;
    GT_U8 delayDuration;

}MV_HWS_PORT_CTRL_COMMON;

/* Port Control Supervisor module (M1) table definition */
typedef struct
{
    MV_HWS_PORT_CTRL_COMMON info;
    GT_U8                   highMsgThreshold;
    GT_U8                   lowMsgThreshold;
    PORT_CTRL_FUNCPTR       funcTbl[SPV_MAX_EVENT][SPV_MAX_STATE];
}MV_HWS_PORT_CTRL_SUPERVISOR;

/* Port management module (M2) table definition */
typedef struct
{
    MV_HWS_PORT_CTRL_COMMON info;
    PORT_CTRL_FUNCPTR       funcTbl[PORT_MNG_MAX_EVENT][PORT_MNG_MAX_STATE];

}MV_HWS_PORT_CTRL_PORT_MNG;

/* Port Control Port State machine module (M3) table definition */
typedef struct
{
    GT_U16 type;
    GT_U8  state;
    GT_U8  status;

}MV_HWS_PORT_CTRL_PORT_SM;

/* Port Control AP Port management module (M4) - Pending message table definition */
typedef struct
{
    GT_U8 ctrlMsgType;
    GT_U8 msgQueueId;
    GT_U8 phyPortNum;
    GT_U8 portMode;
    GT_U8 action;
    GT_U8 refClock;
    GT_U8 refClockSource;

}MV_HWS_IPC_CTRL_MSG_AP_STRUCT;

typedef struct
{
    GT_U8                         pendTasksCount;
    GT_U8                         pendTasksAddIndex;
    GT_U8                         pendTasksProcIndex;
    MV_HWS_IPC_CTRL_MSG_AP_STRUCT pendTasks[MV_PORT_CTRL_PORT_PEND_MSG_NUM];

}MV_HWS_PORT_CTRL_PEND_MSG_TBL;

/* Port Control AP Port management module (M4) - timer table definition */
typedef struct
{
    GT_U8  sysCfState;
    GT_U8  sysCfStatus;
    GT_U32 sysCfgThreshold;
    GT_U8  trainingCount;
    GT_U8  trainingMaxInterval;
    GT_U32 trainingThreshold;

}MV_HWS_PORT_CTRL_AP_PORT_TIMER;

typedef GT_U32 MV_HWS_PORT_CTRL_AP_INIT;

/* Port Control AP Port management module (M4) table */
typedef struct
{
    MV_HWS_PORT_CTRL_COMMON        info;
    PORT_CTRL_FUNCPTR              funcTbl[AP_PORT_MNG_MAX_EVENT][AP_PORT_MNG_MAX_STATE];
    MV_HWS_PORT_CTRL_PORT_SM       apPortSm[MV_PORT_CTRL_MAX_AP_PORT_NUM];
    MV_HWS_PORT_CTRL_AP_PORT_TIMER apPortTimer[MV_PORT_CTRL_MAX_AP_PORT_NUM];
    MV_HWS_PORT_CTRL_PEND_MSG_TBL  tasks[MV_PORT_CTRL_MAX_AP_PORT_NUM];
    MV_HWS_PORT_CTRL_AP_INIT       apInitStatus;

}MV_HWS_PORT_CTRL_AP_PORT_MNG;

/* Port Control AP Detection State machine module (M5) - Info section definition */

typedef struct
{
    /*
    ** AP Configuration Info
    ** =====================
    */

    /*
    ** Interface Number
    ** [00:07] Lane Number
    ** [08:15] PCS Number
    ** [16:23] MAC Number
    ** [24:31] Reserved
    */
    GT_U32 ifNum;

    /*
    ** Capability
    ** [00:00] Advertisement 40GBase KR4
    ** [01:01] Advertisement 10GBase KR
    ** [02:02] Advertisement 10GBase KX4
    ** [03:03] Advertisement 1000Base KX
    ** [04:04] Advertisement 20GBase KR2
    ** [05:05] Advertisement 10GBase KX2
    ** [06:15] Reserved
    */
    GT_U16 capability;

    /*
    ** Options
    ** [00:00] Flow Control Pause Enable
    ** [01:01] Flow Control Asm Direction
    ** [02:03] Reserved
    ** [04:04] FEC Suppress En
    ** [05:05] FEC Request
    ** [06:07] Reserved
    ** [09:09] loopback Enable
    ** [10:15] Reserved
    */
    GT_U16 options;

    /*
    ** AP Status Info
    ** ==============
    */
    GT_U8 state;

    /*
    ** AP Status
    ** [00:00] Signal Detect
    ** [01:01] CDR Lock
    ** [02:02] PCS Lock
    ** [03:07] Reserved
    */
    GT_U8 status;

    /*
    ** ARBSmStatus
    ** [00:00] ST_AN_ENABLE
    ** [01:01] ST_TX_DISABLE
    ** [02:02] ST_LINK_STAT_CK
    ** [03:03] ST_PARALLEL_FLT
    ** [04:04] ST_ABILITY_DET
    ** [05:05] ST_ACK_DETECT
    ** [06:06] ST_COMPLETE_ACK
    ** [07:07] ST_NP_WAIT
    ** [08:08] ST_AN_GOOD_CK
    ** [09:09] ST_AN_GOOD
    ** [10:10] ST_SERDES_WAIT
    */
    GT_U16 ARMSmStatus;

    /*
    ** HCD Status
    ** [03:03] Found
    ** [04:04] ARBSmError
    ** [05:05] FEC Result
    ** [06:06] Flow Control Rx Result
    ** [07:07] Flow Control Tx Result
    ** [08:15] Local HCD Type ------------> [00:00] 40GBase KR4
    ** [16:23] Reserved                     [01:01] 10GBase KR
    ** [24:30] port                         [02:02] 10GBase KX4
    ** [31:31] Link                         [03:03] 1000Base KX
    **                                      [04:04] 20GBase KR2
    **                                      [05:05] 10GBase KX2
    */
    GT_U32 hcdStatus;

    GT_U8 portNum;
    GT_U8 queueId;

}MV_HWS_AP_SM_INFO;

/* Port Control AP Detection State machine module (M5) - timer table definition */
typedef struct
{
    GT_U32 txDisThreshold;
    GT_U32 abilityThreshold;
    GT_U16 abilityCount;
    GT_U32 linkThreshold;
    GT_U16 linkCount;

}MV_HWS_AP_SM_TIMER;

/* Port Control AP Detection State machine module (M5) - Debug section definition */
typedef struct
{
    GT_U16 abilityInvalidCnt; /* Number of Ability detect invalid intervals - ability detected but no resolution */

    GT_U16 txDisCnt;          /* Number of Tx Disable intervals executed */
    GT_U16 abilityCnt;        /* Number of Ability detect intervals executed */
    GT_U16 abilitySuccessCnt; /* Number of Ability detect successfull intervals executed */
    GT_U16 linkFailCnt;       /* Number of Link check fail intervals executed */
    GT_U16 linkSuccessCnt;    /* Number of Link check successfull intervals executed */

    GT_U32 timestampTime;     /* temp parameter holds time stamp for HCD and link up time calc */
    GT_U32 hcdResoultionTime; /* Time duration for HCD resolution */
    GT_U32 linkUpTime;        /* Time duration for Link up */

}MV_HWS_AP_SM_STATS;

/* Port Control AP Detection State machine module (M5) table definition */
typedef struct
{
    MV_HWS_AP_SM_INFO  info;
    MV_HWS_AP_SM_TIMER timer;
    MV_HWS_AP_SM_STATS stats;

}MV_HWS_AP_DETECT_SM;

/* Port Control AP Detection State machine module (M5) - Interop section definition */
typedef struct
{
    GT_U16 attrBitMask;
    GT_U16 txDisDuration;          /* Tx Disable duration - default 60 msec */
    GT_U16 abilityDuration;        /* Ability detect duration - default 6 msec */
    GT_U16 abilityMaxInterval;     /* Ability detect max interval - default 21 = 126msec */
    GT_U16 abilityFailMaxInterval; /* Ability detect fail interval - default 10, ARB SM report GOOD CHECK but no HCD */
    GT_U16 apLinkDuration;         /* AP Link check duration - default 20 msec */
    GT_U16 apLinkMaxInterval;      /* AP Link check max interval - default 25 */
    GT_U16 pdLinkDuration;         /* PD Link check duration - default 25 msec */
    GT_U16 pdLinkMaxInterval;      /* PD Link check max interval - default 10 */

}MV_HWS_AP_DETECT_ITEROP;

/* Port Control AP Detection State machine module (M5) table definition */

typedef struct
{
    MV_HWS_PORT_CTRL_COMMON info;
    PORT_CTRL_FUNCPTR funcTbl[AP_PORT_DETECT_MAX_EVENT][AP_PORT_DETECT_MAX_STATE];
    MV_HWS_AP_DETECT_SM apPortSm[MV_PORT_CTRL_MAX_AP_PORT_NUM];
    MV_HWS_AP_DETECT_ITEROP introp;

}MV_HWS_PORT_CTRL_AP_DETECT;

/* Port Control General module (M6) table definition */
typedef struct
{
    MV_HWS_PORT_CTRL_COMMON info;
    PORT_CTRL_FUNCPTR funcTbl[PORT_GEN_MAX_EVENT][PORT_GEN_MAX_STATE];

}MV_HWS_PORT_CTRL_GENERAL;

/* Port Control Supervisor module (M1) table */
extern MV_HWS_PORT_CTRL_SUPERVISOR mvPortCtrlSpv;
/* Port Control Port management module (M2) table */
extern MV_HWS_PORT_CTRL_PORT_MNG mvPortCtrlPortMng;
/* Port Control Port State machine module (M3) table */
extern MV_HWS_PORT_CTRL_PORT_SM mvPortCtrlPortSm[MV_PORT_CTRL_MAX_PORT_NUM];
/* Port Control AP Port management module (M4) table */
extern MV_HWS_PORT_CTRL_AP_PORT_MNG mvHwsPortCtrlApPortMng;
/* Port Control AP Port State machine module (M5) table */
extern MV_HWS_PORT_CTRL_AP_DETECT mvHwsPortCtrlApPortDetect;
/* Port Control General module (M6) table */
extern MV_HWS_PORT_CTRL_GENERAL mvPortCtrlGen;

/*******************************************************************************
* mvPortCtrlDbInit
*
* DESCRIPTION: Initialize Port control database
*******************************************************************************/
GT_STATUS mvPortCtrlDbInit(void);

/*******************************************************************************
* mvPortCtrlDevInit
*
* DESCRIPTION: Initialize Port control device
*******************************************************************************/
GT_STATUS mvPortCtrlDevInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlDb_H */


