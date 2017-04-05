/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsPortApCtrlDiagIf.c
*
* DESCRIPTION: AP Port Control Diag interface
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 42 $
******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <mvHwsPortApInitIf.h>
#include <mvHwsPortCtrlApInitIf.h>
#include <mvHwsPortCtrlApDefs.h>
#include <mvHwsPortCtrlApLogDefs.h>
#include <mvHwsIpcDefs.h>
#include <private/mvHwsPortApInitIfPrv.h>
#include <mvHwsIpcApis.h>

/**************************** Pre-Declaration ********************************************/
void mvApPortCtrlDebugInfoShowEntry(GT_U32 port, GT_U32 timestamp, GT_U32 info);

/**************************** Definition **************************************/
#define MV_PORT_CTRL_LOG_SIZE (512)

#define AP_PORT_SM_MAX_STATE  (6)
#define SPV_MAX_STATE         (3)
#define PORT_MNG_MAX_STATE    (2)
#define PORT_SM_MAX_STATUS    (22)
#define PORT_SM_MAX_STATE     (5)
#define AP_PORT_MAX_STATUS    (20)
#define MV_GEN_LINK_DOWN      (0x0)
#define MV_GEN_LINK_UP        (0x1)
#define MV_GEN_TUNE_FAIL      (0x0)
#define MV_GEN_TUNE_PASS      (0x1)

/* Bobcat2 Only */
#define MV_PORT_CTRL_AP_PORT_NUM_BASE (48)
#define MV_HWS_PORT_CTRL_AP_PHYSICAL_PORT_GET(phyPort) (phyPort + MV_PORT_CTRL_AP_PORT_NUM_BASE)

/**************************** Globals *****************************************/
/* Port Log Control Parameters */
GT_U32 mvHwsFwLogOffsetAddr;
GT_U32 mvHwsFwLogCountOffsetAddr;
GT_U32 mvHwsFwLogPointerOffsetAddr;
GT_U32 mvHwsFwLogResetOffsetAddr;

/* Port Log */
MV_HWS_PORT_CTRL_LOG_ENTRY portLog[MV_PORT_CTRL_LOG_SIZE];

/* AP Registers description */
char *regType[PORT_CTRL_MAX_AP_REGS] =
{
    "",   /* the values in MV_PORT_CTRL_AP_REGS enum started from 1 */
    /* [PORT_CTRL_AP_REG_CFG_0] */                                   "Cfg-0",
    /* [PORT_CTRL_AP_REG_CFG_1] */                                   "Cfg-1",
    /* [PORT_CTRL_AP_REG_ST_0] */                                    "St-0",
    /* [PORT_CTRL_AP_INT_REG_802_3_AP_CTRL] */                       "Ctrl",
    /* [PORT_CTRL_AP_INT_REG_802_3_AP_ST] */                         "St",
    /* [PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_1] */                  "AdvReg1",
    /* [PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_2] */                  "AdvReg2",
    /* [PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_3] */                  "AdvReg3",
    /* [PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1] */         "LpReg1",
    /* [PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_2] */         "LpReg2",
    /* [PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_3] */         "LpReg3",
    /* [PORT_CTRL_AP_INT_REG_ANEG_CTRL_0] */                         "Aneg0",
    /* [PORT_CTRL_AP_INT_REG_ANEG_CTRL_1] */                         "Aneg1",
    /* [PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_REG] */                "NxtPg",
    /* [PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_0_15] */          "NxtPg-0",
    /* [PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_16_31] */         "NxtPg-16",
    /* [PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_REG] */        "LpNxtPg",
    /* [PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_0_15] */  "LpNxtPg-0",
    /* [PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_16_31] */ "LpNxtPg-16",
    /* [PORT_CTRL_AP_INT_REG_802_3_BP_ETH_ST_REG] */                 "BpSt",
    /* [PORT_CTRL_AP_INT_REG_802_3_LP_ADV_REG] */                    "LpAdv"
};

/* Message Type description */
char *msgType[MV_HWS_IPC_LAST_CTRL_MSG_TYPE] =
{
   /* [MV_HWS_IPC_PORT_INIT_MSG] */                         "Init",
   /* [MV_HWS_IPC_PORT_SERDES_RESET_MSG] */                 "Serdes Reset",
   /* [MV_HWS_IPC_PORT_RESET_MSG] */                        "Reset",
   /* [MV_HWS_IPC_PORT_RESET_EXT_MSG] */                    "Reset Ext",
   /* [MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_MSG] */            "Auto Tune Set",
   /* [MV_HWS_IPC_PORT_AUTO_TUNE_STOP_MSG] */               "Auto Tune Stop",
   /* [MV_HWS_IPC_PORT_AUTO_TUNE_STATE_CHK_MSG] */          "Auto Tune Check",
   /* [MV_HWS_IPC_PORT_POLARITY_SET_MSG] */                 "Polarity",
   /* [MV_HWS_IPC_PORT_FEC_CONFIG_MSG] */                   "Fec Config",
   /* [MV_HWS_IPC_PORT_FEC_CONFIG_GET_MSG] */               "Fec Get",
   /* [MV_HWS_IPC_PORT_LINK_STATUS_GET_MSG] */              "Link Status",
   /* [MV_HWS_IPC_PORT_TX_ENABLE_MSG] */                    "Tx Enable",
   /* [MV_HWS_IPC_PORT_TX_ENABLE_GET_MSG] */                "Tx Enable Get",
   /* [MV_HWS_IPC_PORT_SIGNAL_DETECT_GET_MSG] */            "Signal Detect",
   /* [MV_HWS_IPC_PORT_CDR_LOCK_STATUS_GET_MSG] */          "CDR Lock Status",
   /* [MV_HWS_IPC_PORT_LOOPBACK_SET_MSG] */                 "Loopback",
   /* [MV_HWS_IPC_PORT_LOOPBACK_STATUS_GET_MSG] */          "Loopback Status",
   /* [MV_HWS_IPC_PORT_PPM_SET_MSG] */                      "PPM Set",
   /* [MV_HWS_IPC_PORT_PPM_GET_MSG] */                      "PPM Get",
   /* [MV_HWS_IPC_PORT_IF_GET_MSG] */                       "If Get",
   /* [MV_HWS_IPC_PORT_SERDES_MANUAL_RX_CONFIG_MSG] */      "Serdes Man Rx",
   /* [MV_HWS_IPC_PORT_SERDES_MANUAL_TX_CONFIG_MSG] */      "Serdes Man Tx",
   /* [MV_HWS_IPC_PORT_PCS_ACTIVE_STATUS_GET_CONFIG_MSG] */ "PCS Act Status",
   /* [MV_HWS_IPC_PORT_FC_STATE_SET_MSG] */                 "FC State Set",
   /* [MV_HWS_IPC_PORT_AP_ENABLE_MSG] */                    "AP Enable",
   /* [MV_HWS_IPC_PORT_AP_DISABLE_MSG] */                   "AP Disable",
   /* [MV_HWS_IPC_PORT_AP_SYS_CFG_VALID_MSG] */             "AP Sys Cfg",
   /* [MV_HWS_IPC_PORT_AP_CFG_GET_MSG] */                   "AP Cfg Get",
   /* [MV_HWS_IPC_PORT_AP_STATUS_MSG] */                    "AP Status",
   /* [MV_HWS_IPC_PORT_AP_STATS_MSG] */                     "AP Stats",
   /* [MV_HWS_IPC_PORT_AP_STATS_RESET_MSG] */               "AP Reset",
   /* [MV_HWS_IPC_PORT_AP_INTROP_GET_MSG] */                "AP Introp Get",
   /* [MV_HWS_IPC_PORT_AP_INTROP_SET_MSG] */                "AP Introp Set",
   /* [MV_HWS_IPC_PORT_AP_DEBUG_GET_MSG] */                 "AP Debug",
   /* [MV_HWS_IPC_PORT_PARAMS_MSG] */                       "Params Set",
   /* [MV_HWS_IPC_PORT_AVAGO_SERDES_INIT_MSG] */            "Avago Serdes Init",
   /* [MV_HWS_IPC_PORT_AVAGO_GUI_SET_MSG] */                "Avago GUI Set"

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

/*******************************************************************************
* mvHwsApPortCtrlDebugParamsSet
*
* DESCRIPTION:
*       Set AP debug information
*
* INPUTS:
*       apFwLogBaseAddr    - AP Firmware Real-time log base address
*       apFwLogCountAddr   - AP Firmware Real-time log count address
*       apFwLogPointerAddr - AP Firmware Real-time log pointer address
*       apFwLogResetAddr   - AP Firmware Real-time log reset address
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvHwsApPortCtrlDebugParamsSet
(
    GT_U32 apFwLogBaseAddr,
    GT_U32 apFwLogCountAddr,
    GT_U32 apFwLogPointerAddr,
    GT_U32 apFwLogResetAddr
)
{
    mvHwsFwLogOffsetAddr        = apFwLogBaseAddr;
    mvHwsFwLogCountOffsetAddr   = apFwLogCountAddr;
    mvHwsFwLogPointerOffsetAddr = apFwLogPointerAddr;
    mvHwsFwLogResetOffsetAddr   = apFwLogResetAddr;
}

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
        if      (mask & ST_AN_ENABLE)    osPrintf("ARB Status AN_ENABLE\n");
        else if (mask & ST_TX_DISABLE)   osPrintf("ARB Status TX_DISABLE\n");
        else if (mask & ST_LINK_STAT_CK) osPrintf("ARB Status LINK_STATE_CHECK\n");
        else if (mask & ST_PARALLEL_FLT) osPrintf("ARB Status PARALLEL_FLT\n");
        else if (mask & ST_ABILITY_DET)  osPrintf("ARB Status ABILITY_DET\n");
        else if (mask & ST_ACK_DETECT)   osPrintf("ARB Status ST_ACK_DETECT\n");
        else if (mask & ST_COMPLETE_ACK) osPrintf("ARB Status ST_COMPLETE_ACK\n");
        else if (mask & ST_NP_WAIT)      osPrintf("ARB Status ST_NP_WAIT\n");
        else if (mask & ST_AN_GOOD_CK)   osPrintf("ARB Status AN_GOOD_CK\n");
        else if (mask & ST_AN_GOOD)      osPrintf("ARB Status AN_GOOD\n");
        else if (mask & ST_SERDES_WAIT)  osPrintf("ARB Status SERDES WAIT\n");
    }
    else
    {
        osPrintf("\n");
    }
}

/*******************************************************************************
* mvApPortCtrlStatusShow
*
* DESCRIPTION:
*       Print AP port status information stored in system.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortCtrlStatusShow
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  apPortNum
)
{
    MV_HWS_AP_PORT_STATUS apStatus;

    CHECK_STATUS(mvHwsApPortCtrlStatusGet(devNum, portGroup, apPortNum, &apStatus));

    osPrintf("======== AP Port %d, Lane %d info ========\n", apPortNum, apStatus.apLaneNum);
    osPrintf("AP Port State           - %s\n", apPortMngSmState[apStatus.smState]);
    osPrintf("AP Port Status          - %s\n", apPortMngSmStatus[apStatus.smStatus]);
    osPrintf("AP Port ARB State       - ");
    switch (apStatus.arbStatus)
    {
    case ST_AN_ENABLE:    osPrintf("ST_AN_ENABLE\n");    break;
    case ST_TX_DISABLE:   osPrintf("ST_TX_DISABLE\n");   break;
    case ST_LINK_STAT_CK: osPrintf("ST_LINK_STAT_CK\n"); break;
    case ST_PARALLEL_FLT: osPrintf("ST_PARALLEL_FLT\n"); break;
    case ST_ABILITY_DET:  osPrintf("ST_ABILITY_DET\n");  break;
    case ST_ACK_DETECT:   osPrintf("ST_ACK_DETECT\n");   break;
    case ST_COMPLETE_ACK: osPrintf("ST_COMPLETE_ACK\n"); break;
    case ST_NP_WAIT:      osPrintf("ST_NP_WAIT\n");      break;
    case ST_AN_GOOD_CK:   osPrintf("ST_AN_GOOD_CK\n");   break;
    case ST_AN_GOOD:      osPrintf("ST_AN_GOOD\n");      break;
    case ST_SERDES_WAIT:  osPrintf("ST_SERDES_WAIT\n");  break;
    default:              osPrintf("Unknown\n");         break;
    }

    osPrintf("AP Port HCD             - %s\n", (apStatus.hcdResult.hcdFound) ? ("Found") : ("Not Found."));
    osPrintf("AP Port HCD Type        - ");
    switch (apStatus.postApPortMode)
    {
    case _40GBase_KR:  osPrintf("40GBase KR4\n"); break;
    case _10GBase_KR:  osPrintf("10GBase KR\n");  break;
    case _10GBase_KX4: osPrintf("10GBase KX4\n"); break;
    case _1000Base_X:  osPrintf("1000Base KX\n"); break;
    default:           osPrintf("Unknown\n");     break;
    }

    osPrintf("AP Port HCD FEC Result  - %s\n", (apStatus.hcdResult.hcdFecEn)       ? ("FEC enabled") : ("FEC disabled"));
    osPrintf("AP Port HCD FC Rx Pause - %s\n", (apStatus.hcdResult.hcdFcRxPauseEn) ? ("FC Rx enabled") : ("FC Rx disabled"));
    osPrintf("AP Port HCD FC Tx Pause - %s\n", (apStatus.hcdResult.hcdFcTxPauseEn) ? ("FC Tx enabled") : ("FC Tx disabled"));
    osPrintf("AP Port HCD Link Status - %s\n", (apStatus.hcdResult.hcdLinkStatus)  ? ("Link Up") : ("Link Down"));

    return GT_OK;
}

/*******************************************************************************
* mvApPortCtrlStatsShow
*
* DESCRIPTION:
*       Print AP port statistics information stored in system
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortCtrlStatsShow
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  apPortNum
)
{
    MV_HWS_AP_PORT_STATS apStats;

    CHECK_STATUS(mvHwsApPortCtrlStatsGet(devNum, portGroup, apPortNum, &apStats));

    osPrintf("======== AP Port %d stats ========\n", apPortNum);
    osPrintf("Tx Disable cnt        %08d\n", apStats.txDisCnt);
    osPrintf("Ability check cnt     %08d\n", apStats.abilityCnt);
    osPrintf("Resolution Detect cnt %08d\n", apStats.abilitySuccessCnt);
    osPrintf("Link Fail cnt         %08d\n", apStats.linkFailCnt);
    osPrintf("Link Success cnt      %08d\n", apStats.linkSuccessCnt);
    osPrintf("HCD Time              %08d\n", apStats.hcdResoultionTime);
    osPrintf("link Up Time          %08d\n", apStats.linkUpTime);
    osPrintf("Total Exec Time       %08d\n", apStats.hcdResoultionTime + apStats.linkUpTime);

    return GT_OK;
}

/*******************************************************************************
* mvApPortCtrlIntropShow
*
* DESCRIPTION:
*       Print AP port introp information stored in system
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortCtrlIntropShow
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  apPortNum
)
{
    MV_HWS_AP_PORT_INTROP apIntrop;

    CHECK_STATUS(mvHwsApPortCtrlIntropGet(devNum, portGroup, apPortNum, &apIntrop));

    osPrintf("======== AP Introp Parameters  ========\n");
    osPrintf("Tx Disable duration        %08d msec\n",      apIntrop.txDisDuration);
    osPrintf("Ability duration           %08d msec\n",      apIntrop.abilityDuration);
    osPrintf("Ability max interval       %08d intervals\n", apIntrop.abilityMaxInterval);
    osPrintf("Ability fail max interval  %08d intervals\n", apIntrop.abilityFailMaxInterval);
    osPrintf("AP Link Duration           %08d msec\n",      apIntrop.apLinkDuration);
    osPrintf("AP Link Max interval check %08d intervals\n", apIntrop.apLinkMaxInterval);
    osPrintf("PD Link Duration           %08d msec\n",      apIntrop.pdLinkDuration);
    osPrintf("PD Link Max interval check %08d intervals\n", apIntrop.pdLinkMaxInterval);

    return GT_OK;
}

/*******************************************************************************
* mvApPortCtrlDebugLogShow
*
* DESCRIPTION:
*       Print AP port real-time log information stored in system
*
* INPUTS:
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortCtrlDebugInfoShow
(
    GT_U32  apPortNum
)
{
    GT_U32 logIndex;
    GT_U32 logCount=0;
    GT_U32 logPointer=0;
    GT_U32 logReset=0;
    GT_U32 logActiveCount;
    GT_U32 logActivePointer;
    GT_U32 logInfo;
    GT_U32 timestamp;
    GT_U8  devNum = 0; /* TODO: hack */

    /* Read log control parameters */
    mvHwsServiceCpuRead(devNum, mvHwsFwLogCountOffsetAddr,   (unsigned int*)&logCount,   4);
    mvHwsServiceCpuRead(devNum, mvHwsFwLogPointerOffsetAddr, (unsigned int*)&logPointer, 4);
    mvHwsServiceCpuRead(devNum, mvHwsFwLogResetOffsetAddr,   (unsigned int*)&logReset,   4);
    /* Read log content */
    mvHwsServiceCpuRead(devNum, mvHwsFwLogOffsetAddr, (unsigned int*)portLog, sizeof(portLog));
    /* Mark log count reset (by Service CPU) */
    logReset++;
    mvHwsServiceCpuWrite(devNum, mvHwsFwLogResetOffsetAddr, (unsigned int*)&logReset, 4);

    /* Process Log */
    /* =========== */
    logActiveCount   = logCount;
    logActivePointer = logPointer;

    /* Adjust log control parameters */
    if (logActivePointer >= MV_PORT_CTRL_LOG_SIZE)
    {
        logActivePointer = 0;
    }

    if (logActiveCount >= MV_PORT_CTRL_LOG_SIZE)
    {
        logActiveCount = MV_PORT_CTRL_LOG_SIZE;
    }

    if (logActiveCount > logActivePointer)
    {
        logActivePointer = MV_PORT_CTRL_LOG_SIZE - (logActiveCount - logActivePointer);
    }
    else
    {
        logActivePointer = logActivePointer - logActiveCount;
    }

    /* Dump log */
    osPrintf("Port Control Realtime Log\n"
             "=========================\n"
             "Num of log entries  %03d\n"
             "Current entry index %03d\n",
             logActiveCount, logActivePointer);

    for (logIndex = 0; logIndex < logActiveCount; logIndex++)
    {
        logInfo   = portLog[logActivePointer].info;
        timestamp = portLog[logActivePointer].timestamp;

        mvApPortCtrlDebugInfoShowEntry(apPortNum, timestamp, logInfo);

        logActivePointer++;
        if (logActivePointer >= MV_PORT_CTRL_LOG_SIZE)
        {
            logActivePointer = 0;
        }
    }

    return GT_OK;
}

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
void mvApPortCtrlDebugInfoShowEntry(GT_U32 port, GT_U32 timestamp, GT_U32 logInfo)
{
    GT_U32 msgId;
    GT_U32 stateId;
    GT_U32 portId;
    GT_U32 regId;
    GT_U32 regVal;
    GT_U32 hcdId;
    GT_U32 arbId;
    GT_U32 statusId;

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
            osPrintf("%08d: AP REG  Port %d, State O%d: ", timestamp, MV_HWS_PORT_CTRL_AP_PHYSICAL_PORT_GET(portId), stateId);
        }
        else if ((regId == PORT_CTRL_AP_INT_REG_802_3_AP_CTRL) ||
                 (regId == PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1) ||
                 (regId == PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_REG) ||
                 (regId == PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_REG))
        {
            osPrintf("%08d: AP REG  Port %d, State O%d, AP Int: ", timestamp, MV_HWS_PORT_CTRL_AP_PHYSICAL_PORT_GET(portId), stateId);
        }

        osPrintf("%s 0x%04x ", regType[regId], regVal);

        if ((regId == PORT_CTRL_AP_REG_ST_0) ||
            (regId == PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_3) ||
            (regId == PORT_CTRL_AP_INT_REG_ANEG_CTRL_1) ||
            (regId == PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_16_31) ||
            (regId == PORT_CTRL_AP_INT_REG_802_3_LP_ADV_REG))
        {
            osPrintf("\n");
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
        osPrintf("%08d: Super   Port %02d, %s, %s Msg Sent to execute, O%d(%s)\n",
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
        osPrintf("%08d: PortMng Port %02d, %s Msg start execute, O%d(%s)\n",
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
        osPrintf("%08d: Port SM Port %02d, %s, O%d(%s)\n",
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
        osPrintf("%08d: Port SM Port %02d, %s, O%d(%s)\n",
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
        osPrintf("%08d: AP SM   Port %02d, %s, O%d(%s) ",
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
        osPrintf("%08d: AP SM   Port %02d, HCD[%s %s Link %s, FEC Result %s] FC[Tx %s, RX %s] Int[HCD %s, Link %s] \n",
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
        osPrintf("%08d: General Port %02d, Executing %s Msg",
                   timestamp,
                   portId,
                   msgType[msgId]);

        switch (msgId)
        {
        case MV_HWS_IPC_PORT_LINK_STATUS_GET_MSG:
            if (PORT_GENERAL_LOG_RES_GET(logInfo))
            {
                osPrintf(" %s", (PORT_GENERAL_LOG_DETAIL_GET(logInfo) == MV_GEN_LINK_DOWN) ? "Fail" :"OK");
            }
            break;
        case MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_MSG:
            if (PORT_GENERAL_LOG_RES_GET(logInfo))
            {
                osPrintf(" %s", (PORT_GENERAL_LOG_DETAIL_GET(logInfo) == MV_GEN_TUNE_FAIL) ? "Fail" :"Pass");
            }
            break;
        }
        osPrintf("\n");
        break;
    }
}

