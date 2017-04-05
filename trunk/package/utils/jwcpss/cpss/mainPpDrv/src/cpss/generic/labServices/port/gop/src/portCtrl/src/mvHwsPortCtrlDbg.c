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
*       Port Control Debug
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>

/* Default values for Register dump filering */
GT_U32 apRegDumpActive   = PORT_CTRL_DBG_REG_DUMP_DISABLE;
GT_U32 apRegDumpEnhanced = PORT_CTRL_DBG_REG_INT_NONE;

/* Default values for AP Device and Port Group */
static GT_U8  apDevNum    = 0;
static GT_U32 apPortGroup = 0;

extern void mvApResetStats(GT_U8 portIndex);

GT_U32 apDumpParam = LOG_ALL_PORT_DUMP;

/*******************************************************************************
* mvPortCtrlDebugParamSet
*
* DESCRIPTION: Debug parameter selection function
*
* INPUTS:
*       param - All ports 0xFFFF
*             - port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void mvPortCtrlDebugParamSet(GT_U32 param)
{
    apDumpParam = param;
#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
    mvPortCtrlDbgUnlock();
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */
}

#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
/*******************************************************************************
* mvPortCtrlDebugRoutine
*
* DESCRIPTION: Debug process execution sequence
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
void mvPortCtrlDebugRoutine(void* pvParameters)
{
    for( ;; )
    {
        if (mvPortCtrlDbgLock() == GT_OK)
        {
            mvPortCtrlLogDump(apDumpParam);
        }
    }
}
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

/*******************************************************************************
* mvPortCtrlDbgCfgRegsDump
*
* DESCRIPTION: Config AP register dump functionality
*
* INPUTS:
*       GT_U32 active - 0 - Disable
*                       1 - Enable
*       GT_U32 mode   - 0 - Control, Status
*                       0 - Control, Status, Internal registers
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgCfgRegsDump(GT_U32 active, GT_U32 mode)
{
    apRegDumpActive   = active;
    apRegDumpEnhanced = mode;
}

/*******************************************************************************
* mvPortCtrlDbgCtrlRegsDump
*
* DESCRIPTION: Print AP control and status registers
*
* INPUTS:
*       GT_U8 port
*       GT_U8 pcsNum
*       GT_U8 state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgCtrlRegsDump(GT_U8 portIndex, GT_U8 pcsNum, GT_U8 state)
{
    GT_U32 cfgReg0;
    GT_U32 cfgReg1;
    GT_U32 stReg0;

    if (apRegDumpActive != PORT_CTRL_DBG_REG_DUMP_DISABLE)
    {
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_REG_CFG_0, &cfgReg0, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_REG_CFG_1, &cfgReg1, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_REG_ST_0,  &stReg0,  0);

        mvPortCtrlLogAdd(REG_LOG(state, portIndex, PORT_CTRL_AP_REG_CFG_0, cfgReg0));
        mvPortCtrlLogAdd(REG_LOG(state, portIndex, PORT_CTRL_AP_REG_CFG_1, cfgReg1));
        mvPortCtrlLogAdd(REG_LOG(state, portIndex, PORT_CTRL_AP_REG_ST_0, stReg0));
    }
}

/*******************************************************************************
* mvPortCtrlDbgIntRegsDump
*
* DESCRIPTION: Print AP internal registers
*
* INPUTS:
*       GT_U8 port
*       GT_U8 pcsNum
*       GT_U8 state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgIntRegsDump(GT_U8 port, GT_U8 pcsNum, GT_U8 state)
{
    GT_U32 apCtrl;
    GT_U32 apSt;
    GT_U32 apAdvReg1;
    GT_U32 apAdvReg2;
    GT_U32 apAdvReg3;
    GT_U32 apLpReg1;
    GT_U32 apLpReg2;
    GT_U32 apLpReg3;
    GT_U32 apAnegCtrl0;
    GT_U32 apAnegCtrl1;

    if ((apRegDumpActive   != PORT_CTRL_DBG_REG_DUMP_DISABLE) &&
        (apRegDumpEnhanced  & PORT_CTRL_DBG_REG_INT_REDUCE))
    {
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_AP_CTRL, &apCtrl, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_AP_ST, &apSt, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_AP_ADV_REG_1, &apAdvReg1, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_AP_ADV_REG_2, &apAdvReg2, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_AP_ADV_REG_3, &apAdvReg3, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1, &apLpReg1, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_LP_BASE_ABILITY_REG_2, &apLpReg2, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_LP_BASE_ABILITY_REG_3, &apLpReg3, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_ANEG_CTRL_0, &apAnegCtrl0, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_ANEG_CTRL_1, &apAnegCtrl1, 0);

        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_AP_CTRL, apCtrl));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_AP_ST, apSt));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_1, apAdvReg1));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_2, apAdvReg2));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_AP_ADV_REG_3, apAdvReg3));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1, apLpReg1));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_2, apLpReg2));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_LP_BASE_ABILITY_REG_3, apLpReg3));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_ANEG_CTRL_0, apAnegCtrl0));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_ANEG_CTRL_1, apAnegCtrl1));
    }
}

/*******************************************************************************
* mvPortCtrlDbgAllIntRegsDump
*
* DESCRIPTION: Print All AP internal registers
*
* INPUTS:
*       GT_U8 port
*       GT_U8 pcsNum
*       GT_U8 state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgAllIntRegsDump(GT_U8 port, GT_U8 pcsNum, GT_U8 state)
{
    GT_U32 apNextPageTx;
    GT_U32 apNextPageCode_0_15;
    GT_U32 apNextPageCode_16_31;
    GT_U32 apLpNextPageAbilityTx;
    GT_U32 apLpNextPageAbilityCode_0_15;
    GT_U32 apLpNextPageAbilityCode_16_31;
    GT_U32 apBpEthSt;
    GT_U32 apLpAdv;

    if ((apRegDumpActive   != PORT_CTRL_DBG_REG_DUMP_DISABLE) &&
        (apRegDumpEnhanced  & PORT_CTRL_DBG_REG_INT_FULL))
    {
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_NEXT_PG_TX_REG, &apNextPageTx, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_NEXT_PG_TX_CODE_0_15, &apNextPageCode_0_15, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_NEXT_PG_TX_CODE_16_31, &apNextPageCode_16_31, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_REG, &apLpNextPageAbilityTx, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_0_15, &apLpNextPageAbilityCode_0_15, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_16_31, &apLpNextPageAbilityCode_16_31, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_BP_ETH_ST_REG, &apBpEthSt, 0);
        genUnitRegisterGet(0, 0, MMPCS_UNIT, pcsNum, AP_INT_REG_802_3_LP_ADV_REG, &apLpAdv, 0);

        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_REG, apNextPageTx));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_0_15, apNextPageCode_0_15));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_NEXT_PG_TX_CODE_16_31, apNextPageCode_16_31));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_REG, apLpNextPageAbilityTx));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_0_15, apLpNextPageAbilityCode_0_15));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_LP_NEXT_PG_ABILITY_CODE_16_31, apLpNextPageAbilityCode_16_31));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_BP_ETH_ST_REG, apBpEthSt));
        mvPortCtrlLogAdd(REG_LOG(state, port, PORT_CTRL_AP_INT_REG_802_3_LP_ADV_REG, apLpAdv));
    }
}

/*******************************************************************************
* mvPortCtrlDbgLinkRegsDump
*
* DESCRIPTION: Print SD, CDR, and PCS Link status registers
*
* INPUTS:
*       GT_U8 port
*       GT_U8 laneNum
*       GT_U8 state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgLinkRegsDump(GT_U8 port, GT_U8 laneNum, GT_U8 state)
{
    GT_U32 data;
    GT_U32 sd  = 0;
    GT_U32 cdr = 0;

    if (apRegDumpActive != PORT_CTRL_DBG_REG_DUMP_DISABLE)
    {
        /* CDR lock dectection enable */
        genUnitRegisterSet(apDevNum, apPortGroup, SERDES_PHY_UNIT, laneNum, 0x8C, 0x100, 0x100);
        genUnitRegisterGet(apDevNum, apPortGroup, SERDES_PHY_UNIT, laneNum, 0x8C, &data, 0);
        if (data & 0x80)
        {
            cdr = 1;
        }

        genUnitRegisterGet(apDevNum, apPortGroup, SERDES_UNIT, laneNum, 0x18, &data, 0);
        if (!(data & 0x2))
        {
            sd = 1;
        }

        mvPcPrintf("Port %d State O%d: Signal Detect %s, CDR Lock %s\n",
                   port, state,
                   (sd == 1) ? ("On") : ("Off"),
                   (cdr == 1) ? ("On") : ("Off"));
    }
}

#ifndef DISABLE_CLI
/*******************************************************************************
* mvPortCtrlDbgStatsDump
*
* DESCRIPTION: Print AP protocol statistics counter and timers
*
* INPUTS:
*       GT_U8 port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgStatsDump(GT_U8 port, GT_U8 peer)
{
    GT_U8 portIndex;
    GT_U8 peerPortIndex;

    MV_HWS_AP_SM_INFO  *apSm;
    MV_HWS_AP_SM_STATS *apStats;
    MV_HWS_AP_SM_INFO  *peerApSm;
    MV_HWS_AP_SM_STATS *peerApStats;

    if (port != 0)
    {
        portIndex = MV_HWS_PORT_CTRL_AP_LOGICAL_PORT_GET(port);
        apSm      = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
        apStats   = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);
    }

    if (peer != 0)
    {
        peerPortIndex = MV_HWS_PORT_CTRL_AP_LOGICAL_PORT_GET(peer);
        peerApSm      = &(mvHwsPortCtrlApPortDetect.apPortSm[peerPortIndex].info);
        peerApStats   = &(mvHwsPortCtrlApPortDetect.apPortSm[peerPortIndex].stats);
    }

    if (peer != 0)
    {
        mvPcPrintf("Port              %8d  %8d\n"
                   "Tx Disable cnt    %08d  %08d\n"
                   "Ability check cnt %08d  %08d\n"
                   "Ability succ cnt  %08d  %08d\n"
                   "Link Fail cnt     %08d  %08d\n"
                   "Link succ cnt     %08d  %08d\n",
                   apSm->portNum,              peerApSm->portNum,
                   apStats->txDisCnt,          peerApStats->txDisCnt,
                   apStats->abilityCnt,        peerApStats->abilityCnt,
                   apStats->abilitySuccessCnt, peerApStats->abilitySuccessCnt,
                   apStats->linkFailCnt,       peerApStats->linkFailCnt,
                   apStats->linkSuccessCnt,    peerApStats->linkSuccessCnt);

        mvPcPrintf("HCD Time          %08d  %08d\n"
                   "link Up Time      %08d  %08d\n"
                   "Total Exec Time   %08d  %08d\n",
                   apStats->hcdResoultionTime,                       peerApStats->hcdResoultionTime,
                   apStats->linkUpTime,                              peerApStats->linkUpTime,
                   apStats->hcdResoultionTime + apStats->linkUpTime, peerApStats->hcdResoultionTime + peerApStats->linkUpTime);
    }
    else if (port != 0)
    {
        mvPcPrintf("Port              %8d\n"
                   "Tx Disable cnt    %08d\n"
                   "Ability check cnt %08d\n"
                   "Ability succ cnt  %08d\n"
                   "Link Fail cnt     %08d\n"
                   "Link succ cnt     %08d\n",
                   apSm->portNum,
                   apStats->txDisCnt,
                   apStats->abilityCnt,
                   apStats->abilitySuccessCnt,
                   apStats->linkFailCnt,
                   apStats->linkSuccessCnt);

        mvPcPrintf("HCD Time          %08d\n"
                   "link Up Time      %08d\n"
                   "Total Exec Time   %08d\n",
                   apStats->hcdResoultionTime,
                   apStats->linkUpTime,
                   apStats->hcdResoultionTime + apStats->linkUpTime);
    }
}
#endif /* DISABLE_CLI */

/*******************************************************************************
* mvPortCtrlDbgReg
*
* DESCRIPTION: Configure AP register
*
* INPUTS:
*       GT_U8 reg
*       GT_U8 value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgReg(GT_U8 reg, GT_U8 value)
{
    mvPcPrintf("AP Debug: Reg 0x%x, Value 0x%x\n", reg, value);
}

/*******************************************************************************
* mvPortCtrlDbgStatsReset
*
* DESCRIPTION: Reset AP protocol statistics counter
*
* INPUTS:
*       GT_U8 port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgStatsReset(GT_U8 port)
{
    GT_U8 portIndex = MV_HWS_PORT_CTRL_AP_LOGICAL_PORT_GET(port);

    mvApResetStats(portIndex);
}

#ifndef DISABLE_CLI
/*******************************************************************************
* mvPortCtrlDbgIntropDump
*
* DESCRIPTION: Print AP protocol interop parameters
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
void mvPortCtrlDbgIntropDump(void)
{
    MV_HWS_AP_DETECT_ITEROP *apIntrop = &(mvHwsPortCtrlApPortDetect.introp);

    mvPcPrintf("Tx Disable duration        %08d msec\n"
               "Ability duration           %08d msec\n"
               "Ability max interval       %08d intervals\n"
               "Ability fail max interval  %08d intervals\n"
               "AP Link Duration           %08d msec\n"
               "AP Link Max interval check %08d intervals\n"
               "PD Link Duration           %08d msec\n"
               "PD Link Max interval check %08d intervals\n",
               apIntrop->txDisDuration,
               apIntrop->abilityDuration,
               apIntrop->abilityMaxInterval,
               apIntrop->abilityFailMaxInterval,
               apIntrop->apLinkDuration,
               apIntrop->apLinkMaxInterval,
               apIntrop->pdLinkDuration,
               apIntrop->pdLinkMaxInterval);
}
#endif /* DISABLE_CLI */

/*******************************************************************************
* mvPortCtrlDbgIntropCfgParam
*
* DESCRIPTION: Config AP protocol interop parameters
*
* INPUTS:
*       GT_U8 param
*       GT_U8 value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgIntropCfgParam(GT_U8 param, GT_U8 value)
{
    MV_HWS_AP_DETECT_ITEROP *apIntrop = &(mvHwsPortCtrlApPortDetect.introp);
    GT_U8 paramIndex = 1 << param;

    if (paramIndex == AP_PORT_INTROP_TX_DIS)
    {
        apIntrop->txDisDuration = value;
    }
    else if (paramIndex ==  AP_PORT_INTROP_ABILITY_DUR)
    {
        apIntrop->abilityDuration = value;
    }
    else if (paramIndex == AP_PORT_INTROP_ABILITY_MAX_INT)
    {
        apIntrop->abilityMaxInterval = value;
    }
    else if (paramIndex == AP_PORT_INTROP_ABILITY_MAX_FAIL_INT)
    {
        apIntrop->abilityFailMaxInterval = value;
    }
    else if (paramIndex == AP_PORT_INTROP_AP_LINK_DUR)
    {
        apIntrop->apLinkDuration = value;
    }
    else if (paramIndex == AP_PORT_INTROP_AP_LINK_MAX_INT)
    {
        apIntrop->apLinkMaxInterval = value;
    }
    else if (paramIndex == PD_PORT_INTROP_AP_LINK_DUR)
    {
        apIntrop->pdLinkDuration = value;
    }
    else if (paramIndex == PD_PORT_INTROP_AP_LINK_MAX_INT)
    {
        apIntrop->pdLinkMaxInterval = value;
    }
}

/*******************************************************************************
* mvPortCtrlDbgIntropCfg
*
* DESCRIPTION: Config AP protocol interop parameters
*
* INPUTS:
*       GT_U8 param
*       GT_U8 value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlDbgIntropCfg(MV_HWS_AP_DETECT_ITEROP *apIntropParams)
{
    MV_HWS_AP_DETECT_ITEROP *apIntrop = &(mvHwsPortCtrlApPortDetect.introp);

    if (apIntropParams->attrBitMask & AP_PORT_INTROP_TX_DIS)
    {
        apIntrop->txDisDuration = apIntropParams->txDisDuration;
    }
    if (apIntropParams->attrBitMask & AP_PORT_INTROP_ABILITY_DUR)
    {
        apIntrop->abilityDuration = apIntropParams->abilityDuration;
    }
    if (apIntropParams->attrBitMask & AP_PORT_INTROP_ABILITY_MAX_INT)
    {
        apIntrop->abilityMaxInterval = apIntropParams->abilityMaxInterval;
    }
    if (apIntropParams->attrBitMask & AP_PORT_INTROP_ABILITY_MAX_FAIL_INT)
    {
        apIntrop->abilityFailMaxInterval = apIntropParams->abilityFailMaxInterval;
    }
    if (apIntropParams->attrBitMask & AP_PORT_INTROP_AP_LINK_DUR)
    {
        apIntrop->apLinkDuration = apIntropParams->apLinkDuration;
    }
    if (apIntropParams->attrBitMask & AP_PORT_INTROP_AP_LINK_MAX_INT)
    {
        apIntrop->apLinkMaxInterval = apIntropParams->apLinkMaxInterval;
    }
    if (apIntropParams->attrBitMask & PD_PORT_INTROP_AP_LINK_DUR)
    {
        apIntrop->pdLinkDuration = apIntropParams->pdLinkDuration;
    }
    if (apIntropParams->attrBitMask & PD_PORT_INTROP_AP_LINK_MAX_INT)
    {
        apIntrop->pdLinkMaxInterval = apIntropParams->pdLinkMaxInterval;
    }
#ifndef DISABLE_CLI
    /* Print AP protocol interop parameters */
    mvPortCtrlDbgIntropDump();
#endif /* DISABLE_CLI */
}

