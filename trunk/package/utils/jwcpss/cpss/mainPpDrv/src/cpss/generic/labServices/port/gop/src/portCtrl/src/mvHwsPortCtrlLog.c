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
*******************************************************************************
* mvHwsPortCtrlLog.c
*
* DESCRIPTION:
*       Port Control Log
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>

/* Port Control Log */
MV_HWS_PORT_CTRL_LOG_ENTRY portLog[MV_PORT_CTRL_LOG_SIZE];

/* Port Control Log pointer */
GT_U32 logPointer;

/* Port Control Log count */
GT_U32 logCount;

/* Port Control Host Log reset */
GT_U32 logReset;

/* Port Control Log reset */
GT_U32 logLocalReset;

#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE

/* AP Registers description */
char *regType[PORT_CTRL_MAX_AP_REGS] =
{
    [PORT_CTRL_AP_REG_CFG_0]                                   "Cfg-0",
    [PORT_CTRL_AP_REG_CFG_1]                                   "Cfg-1",
    [PORT_CTRL_AP_REG_ST_0]                                    "St-0",
    [PORT_CTRL_AP_INT_REG_802_3_AP_CTRL]                       "Ctrl",
    [PORT_CTRL_AP_INT_REG_802_3_AP_ST]                         "St",
    [PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_1]                  "AdvReg1",
    [PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_2]                  "AdvReg2",
    [PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_3]                  "AdvReg3",
    [PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1]         "LpReg1",
    [PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_2]         "LpReg2",
    [PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_3]         "LpReg3",
    [PORT_CTRL_AP_INT_REG_ANEG_CTRL_0]                         "Aneg0",
    [PORT_CTRL_AP_INT_REG_ANEG_CTRL_1]                         "Aneg1",
    [PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_REG]                "NxtPg",
    [PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_0_15]          "NxtPg-0",
    [PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_16_31]         "NxtPg-16",
    [PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_REG]        "LpNxtPg",
    [PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_0_15]  "LpNxtPg-0",
    [PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_16_31] "LpNxtPg-16",
    [PORT_CTRL_AP_INT_REG_802_3_BP_ETH_ST_REG]                 "BpSt",
    [PORT_CTRL_AP_INT_REG_802_3_LP_ADV_REG]                    "LpAdv"
};

/* Message Type description */
char *msgType[MV_HWS_IPC_LAST_CTRL_MSG_TYPE] =
{
   [MV_HWS_IPC_PORT_INIT_MSG]                         "Init",
   [MV_HWS_IPC_PORT_SERDES_RESET_MSG]                 "Serdes Reset",
   [MV_HWS_IPC_PORT_RESET_MSG]                        "Reset",
   [MV_HWS_IPC_PORT_RESET_EXT_MSG]                    "Reset Ext",
   [MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_MSG]            "Auto Tune Set",
   [MV_HWS_IPC_PORT_AUTO_TUNE_STOP_MSG]               "Auto Tune Stop",
   [MV_HWS_IPC_PORT_AUTO_TUNE_STATE_CHK_MSG]          "Auto Tune Check",
   [MV_HWS_IPC_PORT_POLARITY_SET_MSG]                 "Polarity",
   [MV_HWS_IPC_PORT_FEC_CONFIG_MSG]                   "Fec Config",
   [MV_HWS_IPC_PORT_FEC_CONFIG_GET_MSG]               "Fec Get",
   [MV_HWS_IPC_PORT_LINK_STATUS_GET_MSG]              "Link Status",
   [MV_HWS_IPC_PORT_TX_ENABLE_MSG]                    "Tx Enable",
   [MV_HWS_IPC_PORT_TX_ENABLE_GET_MSG]                "Tx Enable Get",
   [MV_HWS_IPC_PORT_SIGNAL_DETECT_GET_MSG]            "Signal Detect",
   [MV_HWS_IPC_PORT_LOOPBACK_SET_MSG]                 "Loopback",
   [MV_HWS_IPC_PORT_LOOPBACK_STATUS_GET_MSG]          "Loopback Status",
   [MV_HWS_IPC_PORT_PPM_SET_MSG]                      "PPM Set",
   [MV_HWS_IPC_PORT_PPM_GET_MSG]                      "PPM Get",
   [MV_HWS_IPC_PORT_IF_GET_MSG]                       "If Get",
   [MV_HWS_IPC_PORT_SERDES_MANUAL_RX_CONFIG_MSG]      "Serdes Man Rx",
   [MV_HWS_IPC_PORT_SERDES_MANUAL_TX_CONFIG_MSG]      "Serdes Man Tx",
   [MV_HWS_IPC_PORT_PCS_ACTIVE_STATUS_GET_CONFIG_MSG] "PCS Act Status",
   [MV_HWS_IPC_PORT_AP_ENABLE_MSG]                    "AP Enable",
   [MV_HWS_IPC_PORT_AP_DISABLE_MSG]                   "AP Disable",
   [MV_HWS_IPC_PORT_AP_SYS_CFG_VALID_MSG]             "AP Sys Cfg",
   [MV_HWS_IPC_PORT_AP_CFG_GET_MSG]                   "AP Cfg Get",
   [MV_HWS_IPC_PORT_AP_STATUS_MSG]                    "AP Status",
   [MV_HWS_IPC_PORT_AP_STATS_MSG]                     "AP Stats",
   [MV_HWS_IPC_PORT_AP_STATS_RESET_MSG]               "AP Reset",
   [MV_HWS_IPC_PORT_AP_INTROP_GET_MSG]                "AP Introp Get",
   [MV_HWS_IPC_PORT_AP_INTROP_SET_MSG]                "AP Introp Set",
   [MV_HWS_IPC_PORT_AP_DEBUG_GET_MSG]                 "AP Debug",
   [MV_HWS_IPC_PORT_PARAMS_MSG]                       "Params Set",
   [MV_HWS_IPC_PORT_AVAGO_SERDES_INIT_MSG]            "Avago Serdes Init",
   [MV_HWS_IPC_PORT_AVAGO_GUI_SET_MSG]                "Avago GUI Set",
   [MV_HWS_IPC_PORT_FC_STATE_SET_MSG]                 "FC State Set"
};

/* Port Type description */
char *portType[] =
{
    "Reg",
    "AP"
};

/* Supervisour state description */
char *superState[] =
{
    "High Pri Msg",
    "Low Pri Msg",
    "Delay"
};

/* Port Management state description */
char *portMngState[] =
{
    "Msg Process",
    "Delay"
};

/* Port State machine state description */
char *portMngSmState[] =
{
    "Idle",
    "Serdes Config",
    "Serdes Training",
    "Mac/Pcs Config",
    "Active",
    "Delete"
};

/* Port State machine status description */
char *portMngSmStatus[] =
{
    " ",
    "Start Execute",
    "Serdes Config In Progress",
    "Serdes Config Success",
    "Serdes Config Failure",
    "Serdes Training In Progress",
    "Serdes Training Success",
    "Serdes Training Failure",
    "Serdes Training Config Failure",
    "Serdes Training End Wait for Ack",
    "Mac/Pcs Config In Progress",
    "Mac/Pcs Config Success",
    "Mac/Pcs Config Failure",
    "Delete In Progress",
    "Delete Success",
    "Delete Failure",
    "FEC Config In Progress",
    "FEC Config Success",
    "FEC Config Failure",
    "FC State Set In Progress",
    "FC State Set Success",
    "FC State Set Failure"
};

/* AP HCD Found description */
char* apPortMngHcdFound[] =
{
    "Not Found",
    "Found"
};

/* AP HCD Tyoe description */
char* apPortMngHcdType[] =
{
    "1000Base KX",
    "10GBase KX4",
    "10GBase KR",
    "40GBase KR4"
};

/* AP General filed description */
char* apPortMngGen[] =
{
    "Off",
    "On"
};

#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

#ifndef DISABLE_CLI
/* AP Port State machine description */
char *apPortMngSmState[] =
{
    "Idle",
    "Init",
    "Tx Disable",
    "Resolution",
    "Active",
    "Delete"
};

/* AP Port State machine status description */
char *apPortMngSmStatus[] =
{
    "Start Execute",
    "Serdes Init Failure",
    "Init Success",
    "Tx Disable In Progress",
    "Tx Disable Failure",
    "Tx Disable Success",
    "Resolution In Progress",
    "Parallel Detect Resolution In Progress",
    "Parallel Detect Resolution Failure",
    "AP Resolution In Progress",
    "AP No Resolution",
    "AP Resolution Timer Failure",
    "AP Resolution Max Interval Failure",
    "AP Resolution Success",
    "Link Check Start",
    "Link Check Validate",
    "Link Check Failure",
    "Link Check Success",
    "Delete In Progress",
    "Delete Success",
    "Delete Failure"
};
#endif /* DISABLE_CLI */

/*******************************************************************************
* mvPortCtrlLogInit
*
* DESCRIPTION: Initialize and clear real time log
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
GT_STATUS mvPortCtrlLogInit(void)
{
    osMemSet(portLog, 0, sizeof(MV_HWS_PORT_CTRL_LOG_ENTRY) * MV_PORT_CTRL_LOG_SIZE);
    logPointer    = 0;
    logCount      = 0;
    logReset      = 0;
    logLocalReset = 0;

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlLogInfoGet
*
* DESCRIPTION: Return real time log info
*
* INPUTS:
*       GT_U32 *fwBaseAddr,
*       GT_U32 *logPointerAddr
*       GT_U32 *logCountAddr
*       GT_U32 *logResetAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlLogInfoGet(GT_U32 *fwBaseAddr,
                          GT_U32 *logBaseAddr,
                          GT_U32 *logPointerAddr,
                          GT_U32 *logCountAddr,
                          GT_U32 *logResetAddr)
{
    *fwBaseAddr     = LOAD_ADDR;
    *logBaseAddr    = (GT_U32)portLog;
    *logPointerAddr = (GT_U32)&logPointer;
    *logCountAddr   = (GT_U32)&logCount;
    *logResetAddr   = (GT_U32)&logReset;
}

/*******************************************************************************
* mvPortCtrlLogAdd
*
* DESCRIPTION: Add Log entry to the Log and override oldest Log entry in case
*              the Log is full
*
* INPUTS:
*       GT_U32 entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlLogAdd
(
    GT_U32 entry
)
{
    GT_U32 timestamp = mvPortCtrlCurrentTs();

    if (mvPortCtrlLogLock() == GT_OK)
    {
        if (logLocalReset < logReset)
        {
            logCount      = 0;
            logLocalReset = logReset;
        }

        portLog[logPointer].info      = entry;
        portLog[logPointer].timestamp = timestamp;

        if (logPointer + 1 >= MV_PORT_CTRL_LOG_SIZE)
        {
            logPointer = 0;
        }
        else
        {
            logPointer++;
        }

        if (logCount + 1 >= MV_PORT_CTRL_LOG_SIZE)
        {
            logCount = MV_PORT_CTRL_LOG_SIZE - 1;
        }
        else
        {
            logCount++;
        }
        mvPortCtrlLogUnlock();
    }
}

#ifndef DISABLE_CLI
/*******************************************************************************
* arbSmStateDesc
*
* DESCRIPTION: Print AP ARB State machine status description
*
* INPUTS:
*       GT_U32 mask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void arbSmStateDesc(GT_U32 mask)
{
    if (mask != 0)
    {
        if      (mask & ST_AN_ENABLE)    mvPcPrintf("ARB Status AN_ENABLE\n");
        else if (mask & ST_TX_DISABLE)   mvPcPrintf("ARB Status TX_DISABLE\n");
        else if (mask & ST_LINK_STAT_CK) mvPcPrintf("ARB Status LINK_STATE_CHECK\n");
        else if (mask & ST_PARALLEL_FLT) mvPcPrintf("ARB Status PARALLEL_FLT\n");
        else if (mask & ST_ABILITY_DET)  mvPcPrintf("ARB Status ABILITY_DET\n");
        else if (mask & ST_ACK_DETECT)   mvPcPrintf("ARB Status ST_ACK_DETECT\n");
        else if (mask & ST_COMPLETE_ACK) mvPcPrintf("ARB Status ST_COMPLETE_ACK\n");
        else if (mask & ST_NP_WAIT)      mvPcPrintf("ARB Status ST_NP_WAIT\n");
        else if (mask & ST_AN_GOOD_CK)   mvPcPrintf("ARB Status AN_GOOD_CK\n");
        else if (mask & ST_AN_GOOD)      mvPcPrintf("ARB Status AN_GOOD\n");
        else if (mask & ST_SERDES_WAIT)  mvPcPrintf("ARB Status SERDES WAIT\n");
    }
    else
    {
        mvPcPrintf("\n");
    }
}
#endif /* DISABLE_CLI */

#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
/*******************************************************************************
* mvPortCtrlLogDump
*
* DESCRIPTION: Print Log content according to Log count
*              The order of print is from the oldest to the newest message
*
* INPUTS:
*       GT_U32 port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlLogDump(GT_U32 port)
{
    GT_U32 logIndex;
    GT_U32 msgId;
    GT_U32 stateId;
    GT_U32 portId;
    GT_U32 regId;
    GT_U32 regVal;
    GT_U32 hcdId;
    GT_U32 arbId;
    GT_U32 statusId;
    GT_U32 logActiveCount;
    GT_U32 logActivePointer;
    GT_U32 logInfo;
    GT_U32 timestamp;

    logActiveCount   = logCount;
    logActivePointer = logPointer;

    if (logActiveCount >= MV_PORT_CTRL_LOG_SIZE)
    {
        logActiveCount = MV_PORT_CTRL_LOG_SIZE - 1;
    }

    if (logActivePointer >= MV_PORT_CTRL_LOG_SIZE)
    {
        logActivePointer = 0;
    }

    if (mvPortCtrlLogLock() == GT_OK)
    {
        logCount = 0;
        mvPortCtrlLogUnlock();
    }

    if (logActiveCount > logActivePointer)
    {
        logActivePointer = MV_PORT_CTRL_LOG_SIZE - (logActiveCount - logActivePointer);
    }
    else
    {
        logActivePointer = logActivePointer - logActiveCount;
    }

    mvPcPrintf("Port Control Realtime Log\n"
               "=========================\n"
               "Num of log entries  %03d\n"
               "Current entry index %03d\n",
               logActiveCount, logActivePointer);

    /* Dump log */
    for (logIndex = 0; logIndex < logActiveCount; logIndex++)
    {
        if (mvPortCtrlLogLock() != GT_OK)
            return;

        logInfo   = portLog[logActivePointer].info;
        timestamp = portLog[logActivePointer].timestamp;

        mvPortCtrlLogUnlock();

        if ((logIndex % 10) == 0)
        {
            mvPortCtrlProcessDelay(2);
        }

        /* Update next entry log pointer */
        logActivePointer++;
        if (logActivePointer >= MV_PORT_CTRL_LOG_SIZE)
        {
            logActivePointer = 0;
        }

        switch (LOG_MOD_GET(logInfo))
        {
        case M0_DEBUG:
            portId  = REG_LOG_PORT_GET(logInfo);
            stateId = REG_LOG_STATE_GET(logInfo);
            regId   = REG_LOG_IDX_GET(logInfo);
            regVal  = REG_LOG_VAL_GET(logInfo);

            /* Validate inputs params */
            if (((port != LOG_ALL_PORT_DUMP) && (port != portId)) ||
                 (stateId >= AP_PORT_SM_MAX_STATE) ||
                 (regId >= PORT_CTRL_MAX_AP_REGS))
            {
                break;
            }

            /* Process message
            ** ===============
            ** Print registers in sections:
            **   Cfg-0, Cfg-1, St-0
            **   Ctrl, St, AdvReg1, AdvReg2, AdvReg3
            **   LpReg1, LpReg2, LpReg3, Aneg0, Aneg1
            **   NxtPg, NxtPg-0, NxtPg-16
            **   LpNxtPg, LpNxtPg-0, LpNxtPg-16, BpSt, LpAdv
            */
            if (regId == PORT_CTRL_AP_REG_CFG_0)
            {
                mvPcPrintf("%08d: AP REG  Port %d, State O%d: ", timestamp, MV_HWS_PORT_CTRL_AP_PHYSICAL_PORT_GET(portId), stateId);
            }
            else if ((regId == PORT_CTRL_AP_INT_REG_802_3_AP_CTRL) ||
                     (regId == PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1) ||
                     (regId == PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_REG) ||
                     (regId == PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_REG))
            {
                mvPcPrintf("%08d: AP REG  Port %d, State O%d, AP Int: ", timestamp, MV_HWS_PORT_CTRL_AP_PHYSICAL_PORT_GET(portId), stateId);
            }

            mvPcPrintf("%s 0x%04x ", regType[regId], regVal);

            if ((regId == PORT_CTRL_AP_REG_ST_0) ||
                (regId == PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_3) ||
                (regId == PORT_CTRL_AP_INT_REG_ANEG_CTRL_1) ||
                (regId == PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_16_31) ||
                (regId == PORT_CTRL_AP_INT_REG_802_3_LP_ADV_REG))
            {
                mvPcPrintf("\n");
            }
            break;

        case M1_SUPERVISOR:
            portId  = SPV_LOG_PORT_GET(logInfo);
            msgId   = SPV_LOG_MSG_GET(logInfo);
            stateId = SPV_LOG_STATE_GET(logInfo);

            /* Validate inputs params */
            if (((port != LOG_ALL_PORT_DUMP) && (port != portId)) ||
                 (msgId >= MV_HWS_IPC_LAST_CTRL_MSG_TYPE) ||
                 (stateId >= SPV_MAX_STATE))
            {
                break;
            }

            /* Process message */
            mvPcPrintf("%08d: Super   Port %02d, %s, %s Msg Sent to execute, O%d(%s)\n",
                       timestamp,
                       portId,
                       portType[SPV_LOG_TYPE_GET(logInfo)],
                       msgType[msgId],
                       stateId + 1,
                       superState[stateId]);

            break;

        case M2_PORT_MNG:
            portId  = PORT_MNG_LOG_PORT_GET(logInfo);
            msgId   = PORT_MNG_MSG_GET(logInfo);
            stateId = PORT_MNG_LOG_STATE_GET(logInfo);

            /* Validate inputs params */
            if (((port != LOG_ALL_PORT_DUMP) && (port != portId)) ||
                 (msgId >= MV_HWS_IPC_LAST_CTRL_MSG_TYPE) ||
                 (stateId >= PORT_MNG_MAX_STATE))
            {
                break;
            }

            /* Process message */
            mvPcPrintf("%08d: PortMng Port %02d, %s Msg start execute, O%d(%s)\n",
                       timestamp,
                       portId,
                       msgType[msgId],
                       stateId,
                       portMngState[stateId]);
            break;

        case M3_PORT_SM:
            portId   = PORT_SM_LOG_PORT_GET(logInfo);
            statusId = PORT_SM_LOG_STATUS_GET(logInfo);
            stateId  = PORT_SM_LOG_STATE_GET(logInfo);

            /* Validate inputs params */
            if (((port != LOG_ALL_PORT_DUMP) && (port != portId)) ||
                 (statusId >= PORT_SM_MAX_STATUS) ||
                 (stateId >= PORT_SM_MAX_STATE))
            {
                break;
            }

            /* Process message */
            mvPcPrintf("%08d: Port SM Port %02d, %s, O%d(%s)\n",
                       timestamp,
                       portId,
                       portMngSmStatus[statusId],
                       stateId,
                       portMngSmState[stateId]);
            break;

        case M4_AP_PORT_MNG:
            portId   = AP_PORT_MNG_LOG_PORT_GET(logInfo);
            statusId = AP_PORT_MNG_LOG_STATUS_GET(logInfo);
            stateId  = AP_PORT_MNG_LOG_STATE_GET(logInfo);

            /* Validate inputs params */
            if (((port != LOG_ALL_PORT_DUMP) && (port != portId)) ||
                 (statusId >= PORT_SM_MAX_STATUS) ||
                 (stateId >= PORT_SM_MAX_STATE))
            {
                break;
            }

            /* Process message */
            mvPcPrintf("%08d: Port SM Port %02d, %s, O%d(%s)\n",
                       timestamp,
                       portId,
                       portMngSmStatus[statusId],
                       stateId,
                       portMngSmState[stateId]);
            break;

        case M5_AP_PORT_DET:
            portId   = AP_PORT_DET_LOG_PORT_GET(logInfo);
            statusId = AP_PORT_DET_LOG_STATUS_GET(logInfo);
            stateId  = AP_PORT_DET_LOG_STATE_GET(logInfo);
            arbId    = AP_PORT_DET_LOG_HW_SM_GET(logInfo);

            /* Validate inputs params */
            if (((port != LOG_ALL_PORT_DUMP) && (port != portId)) ||
                 (statusId >= AP_PORT_MAX_STATUS) ||
                 (stateId >= AP_PORT_SM_MAX_STATE))
            {
                break;
            }

            /* Process message */
            mvPcPrintf("%08d: AP SM   Port %02d, %s, O%d(%s) ",
                       timestamp,
                       portId,
                       apPortMngSmStatus[statusId],
                       stateId,
                       apPortMngSmState[stateId]);

            arbSmStateDesc(arbId);
            break;

        case M5_AP_PORT_DET_EXT:
            portId = AP_PORT_DET_EXT_LOG_PORT_GET(logInfo);
            hcdId  = AP_PORT_DET_EXT_LOG_HCD_GET(logInfo);

            /* Validate inputs params */
            if (((port != LOG_ALL_PORT_DUMP) && (port != portId)) ||
                 (hcdId >= 4))
            {
                break;
            }

            /* Process message */
            mvPcPrintf("%08d: AP SM   Port %02d, HCD[%s %s Link %s, FEC Result %s] FC[Tx %s, RX %s] Int[HCD %s, Link %s] \n",
                       timestamp,
                       portId,
                       apPortMngHcdFound[AP_PORT_DET_EXT_LOG_FOUND_GET(logInfo)],
                       apPortMngHcdType[AP_PORT_DET_EXT_LOG_HCD_GET(logInfo)],
                       apPortMngGen[AP_PORT_DET_EXT_LOG_LINK_GET(logInfo)],
                       apPortMngGen[AP_PORT_DET_EXT_LOG_FEC_RES_GET(logInfo)],
                       apPortMngGen[AP_PORT_DET_EXT_LOG_TX_FC_GET(logInfo)],
                       apPortMngGen[AP_PORT_DET_EXT_LOG_RX_FC_GET(logInfo)],
                       apPortMngGen[AP_PORT_DET_EXT_LOG_HCD_INT_GET(logInfo)],
                       apPortMngGen[AP_PORT_DET_EXT_LOG_LINK_INT_GET(logInfo)]);
            break;

        case M6_GENERAL:
            portId = PORT_GENERAL_LOG_PORT_GET(logInfo);
            msgId  = PORT_GENERAL_LOG_MSG_GET(logInfo);

            /* Validate inputs params */
            if (((port != LOG_ALL_PORT_DUMP) && (port != portId)) ||
                 (msgId >= MV_HWS_IPC_LAST_CTRL_MSG_TYPE))
            {
                break;
            }

            /* Process message */
            mvPcPrintf("%08d: General Port %02d, Executing %s Msg",
                       timestamp,
                       portId,
                       msgType[msgId]);

            switch (msgId)
            {
            case MV_HWS_IPC_PORT_LINK_STATUS_GET_MSG:
                if (PORT_GENERAL_LOG_RES_GET(logInfo))
                {
                    mvPcPrintf(" %s", (PORT_GENERAL_LOG_DETAIL_GET(logInfo) == MV_GEN_LINK_DOWN) ? "Fail" :"OK");
                }
                break;
            case MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_MSG:
                if (PORT_GENERAL_LOG_RES_GET(logInfo))
                {
                    mvPcPrintf(" %s", (PORT_GENERAL_LOG_DETAIL_GET(logInfo) == MV_GEN_TUNE_FAIL) ? "Fail" :"Pass");
                }
                break;
            }
            mvPcPrintf("\n");
            break;
        }
    }
}
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

#ifndef DISABLE_CLI
/*******************************************************************************
* mvPortCtrlApPortStatus
*
* DESCRIPTION: AP Port state & status show
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlApPortStatus(void)
{
    GT_U8 portIndex;

    MV_HWS_AP_SM_INFO *apSm;

    for (portIndex = 0; portIndex < MV_PORT_CTRL_MAX_AP_PORT_NUM; portIndex++)
    {
        apSm = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);

        if (apSm->state != AP_PORT_SM_IDLE_STATE)
        {
            mvPcPrintf("AP Port %02d, %s, O%d(%s) ",
                       apSm->portNum, apPortMngSmStatus[apSm->status],
                       apSm->state, apPortMngSmState[apSm->state]);

            arbSmStateDesc(apSm->ARMSmStatus);
        }
    }
}
#endif /* DISABLE_CLI */
