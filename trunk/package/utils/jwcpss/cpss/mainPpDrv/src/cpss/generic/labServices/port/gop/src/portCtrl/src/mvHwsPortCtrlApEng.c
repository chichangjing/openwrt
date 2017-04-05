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
* mvHwsPortCtrlAp.c
*
* DESCRIPTION:
*       Port Control AP Detection Engine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>
#include <mvHwsServiceCpuInt.h>

extern GT_STATUS mvHwsPortFixAlign90Ext
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode
);

/* Default values for AP Device and Port Group */
#ifdef BOBK_DEV_SUPPORT
#include "mv_hws_avago_if.h"
GT_U8  apDevNum    = 0;
/* AVAGO_SERDES_INIT_BYPASS_VCO
** ============================ 
** BOBK Avago SERDES Init includes VCO calibration,
** this calibration adds 20msec delay for each SERDES
** The portGroup parameter is used to pass bypass VCO
** calibration indication to SERDES Init API in case
** called during AP process
** Only valid for BOBK
*/
GT_U32 apPortGroup = AVAGO_SERDES_INIT_BYPASS_VCO; 
#else
GT_U8  apDevNum    = 0;
GT_U32 apPortGroup = 0;
#endif /* BOBK_DEV_SUPPORT */

/*******************************************************************************
* mvHwsApConvertPortMode
*
* DESCRIPTION: Convert AP state machine port mode to valid port mode numbering
*******************************************************************************/
MV_HWS_PORT_STANDARD mvHwsApConvertPortMode(MV_HWA_AP_PORT_MODE apPortMode)
{
    switch (apPortMode)
    {
    case Port_1000Base_KX: return _1000Base_X;
    case Port_10GBase_KX4: return _10GBase_KX4;
    case Port_10GBase_R:   return _10GBase_KR;
    case Port_40GBase_R:   return _40GBase_KR;
    default:               return NON_SUP_MODE;
    }
}

/*******************************************************************************
* mvApArbFsmGet
* mvApArbFsmSet
*
* DESCRIPTION: List of AP ARM SM API's
*******************************************************************************/

/** mvApArbFsmGet
*******************************************************************************/
MV_HWS_AP_SM_STATE mvApArbFsmGet(GT_U8 pcsNum)
{
    /* Read Arbiter FSM from ANEG Control Register 0
    ** (internal reg 0x80) bits [14:4] to AP port status register
    */
    GT_U32 data;
    MV_HWS_AP_SM_STATE arbState;

    CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_ANEG_CTRL_0, &data, 0));

    arbState = (data >> AP_INT_REG_ANEG_CTRL_0_ANEG_STATE_SHIFT) &
                        AP_INT_REG_ANEG_CTRL_0_ANEG_STATE_MASK;

    return arbState;
}

/** mvApArbFsmSet
*******************************************************************************/
GT_STATUS mvApArbFsmSet(GT_U8 pcsNum, MV_HWS_AP_SM_STATE arbState)
{
    GT_U32 data = (GT_U32)arbState;

    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_ANEG_CTRL_0,
                                    (data << AP_INT_REG_ANEG_CTRL_0_ANEG_STATE_SHIFT) + 1,
                                             AP_INT_REG_ANEG_CTRL_0_ANEG_MASK));

    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_ANEG_CTRL_0,
                                    (data << AP_INT_REG_ANEG_CTRL_0_ANEG_STATE_SHIFT),
                                             AP_INT_REG_ANEG_CTRL_0_ANEG_MASK));

    return GT_OK;
}

/*******************************************************************************
* mvApSetPcsMux
* mvApPcsMuxReset
*
* DESCRIPTION: Set [9:8] bits in PCS40G Common Control 0x088C0414
*              Modes:
*               0x0 = 10G; 10G; PCS working in 10G with lane 0
*               0x1 = 20G; 20G; PCS working in 20G with lane 0,1
*               0x2 = 40G; 40G; PCS working in 40G with lane 0,1,2,3
*               0x3 = AP
*
*              Set to 0 in case of Reset
*******************************************************************************/

/** mvApSetPcsMux
*******************************************************************************/
GT_STATUS mvApPcsMuxSet(GT_U8 pcsNum, GT_U8 pcsMode)
{
    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    PCS40G_COMMON_CTRL_REG,
                                    (pcsMode << PCS40G_COMMON_CTRL_REG_PCS_MODE_SHIFT),
                                                PCS40G_COMMON_CTRL_REG_PCS_MODE_MASK));

    return GT_OK;
}

/** mvApPcsMuxReset
*******************************************************************************/
GT_STATUS mvApPcsMuxReset(GT_U8 pcsNum)
{
    GT_U32 data;

    CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup,MMPCS_UNIT, pcsNum,
                                    PCS40G_COMMON_CTRL_REG, &data, (3 << 8)));

    if (data == 0x300)
    {
        mvApPcsMuxSet(pcsNum, 0);
    }

    return GT_OK;
}

/*******************************************************************************
* mvApHcdFecParamsSet
* mvApHcdFcParamsSet
* mvApHcdAdvertiseSet
* mvApFcResolutionSet
*
* DESCRIPTION: List of AP HCD API's
*******************************************************************************/

/** mvApHcdFecParamsSet
*******************************************************************************/
GT_STATUS mvApHcdFecParamsSet(GT_U8 pcsNum, GT_U8 fecAbility, GT_U8 fecEnable)
{
    GT_U32 data = (fecEnable << 1) + fecAbility;

    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_802_3_AP_ADV_REG_3,
                                    (data << AP_INT_REG_802_3_AP_ADV_REG_3_FEC_SHIFT),
                                             AP_INT_REG_802_3_AP_ADV_REG_3_FEC_MASK));

    return GT_OK;
}

/** mvApHcdFcParamsSet
*******************************************************************************/
GT_STATUS mvApHcdFcParamsSet(GT_U8 pcsNum, GT_U8 fcEnable, GT_U8 fcDir)
{
  GT_U32 data = (fcDir << 1) + fcEnable;

  CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                  AP_INT_REG_802_3_AP_ADV_REG_1,
                                  (data << AP_INT_REG_802_3_AP_ADV_REG_1_FC_PAUSE_SHIFT),
                                           AP_INT_REG_802_3_AP_ADV_REG_1_FC_PAUSE_MASK));

  return GT_OK;
}

/** mvApHcdAdvertiseSet
*******************************************************************************/
GT_STATUS mvApHcdAdvertiseSet(GT_U8 pcsNum, GT_U32 modeVector)
{
    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_REG_CFG_0,
                                    (modeVector << AP_REG_CFG_0_ADV_SHIFT),
                                                   AP_REG_CFG_0_ADV_MASK));

    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_REG_CFG_1, 0, AP_REG_CFG_1_ADV_MASK));

    return GT_OK;
}

/** mvApFcResolutionSet
*******************************************************************************/
GT_STATUS mvApFcResolutionSet(GT_U8 portIndex)
{
    GT_U8  localFcEn;
    GT_U8  localFcAsm;
    GT_U8  peerFcEn;
    GT_U8  peerFcAsm;
    GT_U32 data;

    MV_HWS_AP_SM_INFO *apSm = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    GT_U8 pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1, &data, 0));

    localFcEn  = (GT_U8)AP_CTRL_FC_PAUSE_GET(apSm->options);
    localFcAsm = (GT_U8)AP_CTRL_FC_ASM_GET(apSm->options);

    peerFcEn  = (GT_U8)(data >> AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1_FC_PAUSE_SHIFT);
    peerFcAsm = (GT_U8)(data >> AP_INT_REG_802_3_LP_BASE_ABILITY_REG_1_FC_DIR_SHIFT);

    AP_CTRL_REM_FC_PAUSE_SET(apSm->options, peerFcEn);
    AP_CTRL_REM_FC_ASM_SET(apSm->options, peerFcAsm);

    if ((localFcEn & peerFcEn) ||
        (localFcAsm & peerFcEn & peerFcAsm))
    {
        AP_ST_HCD_FC_TX_RES_SET(apSm->hcdStatus, 1); /* Set FC Tx */
    }

    if ((localFcEn & peerFcEn) ||
        (localFcEn & localFcAsm & peerFcAsm))
    {
        AP_ST_HCD_FC_RX_RES_SET(apSm->hcdStatus, 1); /* Set FC Rx */
    }

    return GT_OK;
}

/*******************************************************************************
* mvApReset
* mvApAnEnable
* mvApAnRestart
* mvApStop
* mvApResetStatus
* mvApResetTimer
* mvApPortDelete
*
* DESCRIPTION: List of AP HW mechanisem API's
*******************************************************************************/

/** mvApReset
*******************************************************************************/
GT_STATUS mvApReset(GT_U8 pcsNum, GT_U8 enable)
{
    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_802_3_AP_CTRL,
                                    (enable << AP_INT_REG_802_3_AP_CTRL_RST_SHIFT),
                                               AP_INT_REG_802_3_AP_CTRL_RST_MASK));

    return GT_OK;
}

/** mvApAnEnable
*******************************************************************************/
GT_STATUS mvApAnEnable(GT_U8 pcsNum, GT_U8 anEn)
{
    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_802_3_AP_CTRL,
                                    (anEn << AP_INT_REG_802_3_AP_CTRL_AN_ENA_SHIFT),
                                             AP_INT_REG_802_3_AP_CTRL_AN_ENA_MASK));


    return GT_OK;
}


/** mvApAnRestart
*******************************************************************************/
GT_STATUS mvApAnRestart(GT_U8 pcsNum, GT_U8 reset)
{
    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_802_3_AP_CTRL,
                                    (reset << AP_INT_REG_802_3_AP_CTRL_AN_RST_SHIFT),
                                              AP_INT_REG_802_3_AP_CTRL_AN_RST_MASK));

    return GT_OK;
}

/** mvApUnreset
*******************************************************************************/
GT_STATUS mvApUnreset(GT_U8 pcsNum, GT_BOOL unreset)
{
    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    PCS_CLOCK_RESET_REG,
                                    (unreset << PCS_CLOCK_RESET_REG_AP_RESET_SHIFT),
                                                PCS_CLOCK_RESET_REG_AP_RESET_MASK));

    return GT_OK;
}

/** mvApStop
*******************************************************************************/
void mvApStop(GT_U8 pcsNum)
{
    mvApAnEnable(pcsNum, 0);
    mvApArbFsmSet(pcsNum, ST_AN_ENABLE);
    mvApHcdAdvertiseSet(pcsNum, 0);
}

/** mvApResetStatus
*******************************************************************************/
void mvApResetStatus(GT_U8 portIndex)
{
    MV_HWS_AP_SM_INFO *apSm = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);

    apSm->hcdStatus   = 0;
    apSm->status      = 0;
}

/** mvApResetTimer
*******************************************************************************/
void mvApResetTimer(GT_U8 portIndex)
{
    MV_HWS_AP_SM_TIMER *apTimer = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].timer);

    apTimer->txDisThreshold   = 0;
    apTimer->abilityThreshold = 0;
    apTimer->abilityCount     = 0;
    apTimer->linkThreshold    = 0;
    apTimer->linkCount        = 0;
}

/** mvApResetStats
*******************************************************************************/
void mvApResetStats(GT_U8 portIndex)
{
    MV_HWS_AP_SM_STATS *apStats = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);

    apStats->txDisCnt          = 0;
    apStats->abilityCnt        = 0;
    apStats->abilitySuccessCnt = 0;
    apStats->linkFailCnt       = 0;
    apStats->linkSuccessCnt    = 0;
}

/** mvApResetStatsTimestamp
*******************************************************************************/
void mvApResetStatsTimestamp(GT_U8 portIndex)
{
    MV_HWS_AP_SM_STATS *apStats = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);

    apStats->timestampTime     = 0;
    apStats->hcdResoultionTime = 0;
    apStats->linkUpTime        = 0;
}

/** mvApPortDelete
*******************************************************************************/
GT_STATUS mvApPortDelete(GT_U8 portIndex)
{
    MV_HWS_PORT_STANDARD portMode;
    MV_HWA_AP_PORT_MODE  apPortMode;
    MV_HWS_AP_SM_INFO    *apSm = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);

    apPortMode = AP_ST_HCD_TYPE_GET(apSm->hcdStatus);

    portMode = mvHwsApConvertPortMode(apPortMode);
    if(portMode == NON_SUP_MODE)
    {
        return GT_NOT_SUPPORTED;
    }

    /* Send port delete message */
    mvPortCtrlApPortMsgSend(portIndex,
                            (GT_U8)MV_HWS_IPC_PORT_RESET_MSG,
                            (GT_U8)apSm->queueId,
                            (GT_U8)apSm->portNum,
                            (GT_U8)portMode,
                            (GT_U8)PORT_POWER_DOWN,
                            0, 0);

    return GT_OK;
}

/*******************************************************************************
*                            AP Port Init State                                *
* ============================================================================ *
*******************************************************************************/

/*******************************************************************************
* mvHwsInitialConfiguration
*
* DESCRIPTION: Set DMA in 10G/40G mode (instead of 1G mode)
*******************************************************************************/
GT_STATUS mvHwsInitialConfiguration(GT_U8 portIndex)
{
    MV_HWS_AP_SM_INFO  *apSm    = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_STATS *apStats = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);

    /* set DMA in 10G/40G mode (instead of 1G mode) */
    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, XLGMAC_UNIT,
                                    AP_CTRL_MAC_GET(apSm->ifNum), 0x84, 0, (1 << 12)));

    /* reset timestamps */
    mvApResetStatsTimestamp(portIndex);
    apStats->timestampTime = mvPortCtrlCurrentTs();

    return GT_OK;
}

/** mvApSerdesPowerUp
*******************************************************************************/
GT_STATUS mvApSerdesPowerUp(GT_U8 serdesNum, GT_BOOL powerUp)
{
    GT_STATUS status = GT_OK;
    GT_U8 devNum = 0;

    status = mvHwsSerdesPowerCtrl(apDevNum,                    /* devNum         */
                                  apPortGroup,                 /* portGroup      */
                                  serdesNum,                   /* serdesNum      */
                                  HWS_DEV_SERDES_TYPE(devNum), /* serdesType     */
                                  powerUp,                     /* powerUp        */
                                  _3_125G,                     /* baudRate       */
                                  _156dot25Mhz,                /* refClock       */
                                  PRIMARY,                     /* refClockSource */
                                  XAUI_MEDIA,                  /* media          */
                                  _10BIT_OFF);                 /* mode           */

    return status;
}

/*******************************************************************************
* mvApPortInit
*
* DESCRIPTION: AP Port init execution sequence
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_STATUS mvApPortInit(GT_U8 portIndex)
{
    MV_HWS_AP_SM_INFO  *apSm    = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_STATS *apStats = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);

    GT_U8 pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);
    GT_U8 laneNum = AP_CTRL_LANE_GET(apSm->ifNum);

    mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

    mvApResetStatus(portIndex);
    mvApResetTimer(portIndex);

    /* Lane power up in 3.125 */
    if (mvApSerdesPowerUp(laneNum, GT_TRUE) != GT_OK)
    {
        /* if Serdes power-up failed, power it down, exit and next iteration
        ** the Serdes will be powered-up again
        */
        apSm->status = AP_PORT_SERDES_INIT_FAILURE;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

        mvApSerdesPowerUp(laneNum, GT_FALSE);
        return GT_OK;
    }

    /* PCS MUX set to AP */
    mvApPcsMuxSet(pcsNum, 3);
    mvApUnreset(pcsNum, GT_TRUE);
    /* AP Enable */
    mvApAnEnable(pcsNum, 1);

    /* AP ARB state machine - AN_ENABLE */
    mvApArbFsmSet(pcsNum, ST_AN_ENABLE);
    AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_AN_ENABLE);

    /* AP state & status update */
    apSm->status = AP_PORT_INIT_SUCCESS;
    mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

    apSm->state = AP_PORT_SM_TX_DISABLE_STATE;

    /* AP statistics update */
    apStats->abilityInvalidCnt = 0;

    /* AP register dump */
    mvPortCtrlDbgCtrlRegsDump(portIndex, pcsNum, AP_PORT_SM_INIT_STATE);
    mvPortCtrlDbgIntRegsDump(portIndex, pcsNum, AP_PORT_SM_INIT_STATE);
    mvPortCtrlDbgAllIntRegsDump(portIndex, pcsNum, AP_PORT_SM_INIT_STATE);

    return GT_OK;
}

/*******************************************************************************
*                            AP Port Tx Disable State                          *
* ============================================================================ *
*******************************************************************************/

/*******************************************************************************
* mvApParallelDetectModeSet
*
* DESCRIPTION: Check if KX4(XAUI)/1G are advertised
*              if not, close parallel detect for this port.
*              This is done by enabling overwrite bit
*              (bits [6] or [8] and closing parallel detect (bits [7] or [9])
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApParallelDetectModeSet(GT_U8 portIndex)
{
    GT_U32 data = 0;

    MV_HWS_AP_SM_INFO *apSm = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    GT_U8 pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    /* Enable Overwrite and disable parallel detect for XAUI */
    if(AP_CTRL_10GBase_KX4_GET(apSm->capability) == 0)
    {
        data |= (1 << 6);
    }

    /* Enable Overwrite and disable parallel detect for 1G */
    if(AP_CTRL_1000Base_KX_GET(apSm->capability) == 0)
    {
        data |= (1 << 8);
    }

    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_ANEG_CTRL_1, data,
                                    AP_INT_REG_ANEG_CTRL_1_PD_CAP_MASK));

    return GT_OK;
}

/** mvApPreCfgLoopback
*******************************************************************************/
GT_STATUS mvApPreCfgLoopback(GT_U8 pcsNum, GT_U8 lbEn)
{
    CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_INT_REG_ANEG_CTRL_1,
                                    (lbEn << AP_INT_REG_ANEG_CTRL_1_LB_NONCE_MATCH_SHIFT),
                                             AP_INT_REG_ANEG_CTRL_1_LB_NONCE_MATCH_MASK));

    return GT_OK;
}

/*******************************************************************************
* mvApPortTxDisable
*
* DESCRIPTION: AP Port Tx Disable execution sequence
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_STATUS mvApPortTxDisable(GT_U8 portIndex)
{
    GT_U8 laneNum;
    GT_U8 pcsNum;
    MV_HWS_AP_SM_INFO       *apSm     = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_TIMER      *apTimer  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].timer);
    MV_HWS_AP_SM_STATS      *apStats  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);
    MV_HWS_AP_DETECT_ITEROP *apIntrop = &(mvHwsPortCtrlApPortDetect.introp);
    GT_U8 devNum = 0;

    /* Extract PCS and Lane number */
    pcsNum  = AP_CTRL_PCS_GET(apSm->ifNum);
    laneNum = AP_CTRL_LANE_GET(apSm->ifNum);

    if (apSm->status != AP_PORT_TX_DISABLE_IN_PROGRESS)
    {
        /* SERDES Tx Disable */
        mvHwsSerdesTxEnable(apDevNum, apDevNum, laneNum, HWS_DEV_SERDES_TYPE(devNum), GT_FALSE);

        /* AP ARB state machine - TX_DISABLE */
        mvApArbFsmSet(pcsNum, ST_TX_DISABLE);
        AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_TX_DISABLE);

        /* Reload advertisement - Configure HW with HCD parameters from AP control */
        mvApHcdAdvertiseSet(pcsNum, AP_CTRL_ADV_ALL_GET(apSm->capability));

        mvApParallelDetectModeSet(portIndex);

        /* Config FC and FEC */
        mvApHcdFcParamsSet(pcsNum, (GT_U8)AP_CTRL_FC_PAUSE_GET(apSm->options),
                                   (GT_U8)AP_CTRL_FC_ASM_GET(apSm->options));

        mvApHcdFecParamsSet(pcsNum, (GT_U8)AP_CTRL_FEC_ABIL_GET(apSm->options),
                                    (GT_U8)AP_CTRL_FEC_REQ_GET(apSm->options));

        /* Config loopback if needed */
        if (AP_CTRL_LB_EN_GET(apSm->options))
        {
            mvApPreCfgLoopback(pcsNum, 1);
        }

        /* AP state & status update */
        apSm->status = AP_PORT_TX_DISABLE_IN_PROGRESS;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

        /* AP statistics update */
        apStats->txDisCnt++;

        /* Tx Disable timer start */
        mvPortCtrlThresholdSet(apIntrop->txDisDuration, &(apTimer->txDisThreshold));
    }
    else if (apSm->status == AP_PORT_TX_DISABLE_IN_PROGRESS)
    {
        /* Check Tx Disable threshold */
        if (mvPortCtrlThresholdCheck(apTimer->txDisThreshold))
        {
            /* clear Tx Disable */
            mvHwsSerdesTxEnable(apDevNum, apDevNum, laneNum, HWS_DEV_SERDES_TYPE(devNum), GT_TRUE);

            /* AP state & status update */
            apSm->status = AP_PORT_TX_DISABLE_SUCCESS;
            mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

            mvApArbFsmSet(pcsNum, ST_ABILITY_DET);
            AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_ABILITY_DET);
            mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

            apSm->state = AP_PORT_SM_RESOLUTION_STATE;

            /* Ability timer start */
            mvPortCtrlThresholdSet(apIntrop->abilityDuration, &(apTimer->abilityThreshold));

            /* AP register dump */
            mvPortCtrlDbgCtrlRegsDump(portIndex, pcsNum, AP_PORT_SM_TX_DISABLE_STATE);
            mvPortCtrlDbgIntRegsDump(portIndex, pcsNum, AP_PORT_SM_TX_DISABLE_STATE);
            mvPortCtrlDbgAllIntRegsDump(portIndex, pcsNum, AP_PORT_SM_TX_DISABLE_STATE);
        }
    }

    return GT_OK;
}

/*******************************************************************************
*                            AP Port Resolution State                          *
* ============================================================================ *
*******************************************************************************/

/*******************************************************************************
* mvApPortResolutionSuccess
*
* DESCRIPTION: AP Port Resolution - AP execution sequence
*              Read the power-up bits in AP status reg 0
*              As certain that exactly one of them is active
*
* INPUTS:
*       GT_U8 portIndex
*       GT_U8 mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void mvApPortResolutionSuccess(GT_U8 portIndex, GT_U8 mode)
{
    GT_U8 laneNum;
    MV_HWS_PORT_STANDARD portMode;
    MV_HWA_AP_PORT_MODE apPortMode;
    MV_HWS_AP_SM_INFO       *apSm     = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_TIMER      *apTimer  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].timer);
    MV_HWS_AP_DETECT_ITEROP *apIntrop = &(mvHwsPortCtrlApPortDetect.introp);

    GT_U8 pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    /* PCS MUX set according to HCD resolution */
    apPortMode = AP_ST_HCD_TYPE_GET(apSm->hcdStatus);
    switch (apPortMode)
    {
    case Port_10GBase_KX4:
    case Port_1000Base_KX:
        mvApPcsMuxSet(pcsNum, 0);
        break;

    case Port_10GBase_R:
        mvApPcsMuxSet(pcsNum, 0);
        break;

    case Port_40GBase_R:
        mvApPcsMuxSet(pcsNum, 2);
        break;

    default:
        break;
    }

    /* SERDES power down */
    laneNum = AP_CTRL_LANE_GET(apSm->ifNum);
    mvApSerdesPowerUp(laneNum, GT_FALSE);

    /* AP Port mode parameters update */
    if(mvHwsApSetPortParameters(apSm->portNum, apPortMode) != GT_OK)
    {
        mvPcPrintf("Error, AP , Port-%d Params set Failed\n", apSm->portNum);
        return;
    }

    portMode = mvHwsApConvertPortMode(apPortMode);

    /* Send port init message */
    mvPortCtrlApPortMsgSend(portIndex,
                            (GT_U8)MV_HWS_IPC_PORT_INIT_MSG,
                            (GT_U8)apSm->queueId,
                            (GT_U8)apSm->portNum,
                            (GT_U8)portMode,
                            (GT_U8)GT_FALSE,
                            (GT_U8)MHz_156,
                            (GT_U8)PRIMARY_LINE_SRC);

    if (mode == PORT_CTRL_AP_LINK_CHECK_MODE)
    {
        /* Link check timer start - AP */
        mvPortCtrlThresholdSet(apIntrop->apLinkDuration, &(apTimer->linkThreshold));
    }
    else
    {
        /* Link check timer start - Parallel Detect */
        mvPortCtrlThresholdSet(apIntrop->pdLinkDuration, &(apTimer->linkThreshold));
    }

    /* AP state & status update */
    apSm->status = AP_PORT_LINK_CHECK_START;
    mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

    apSm->state = AP_PORT_SM_ACTIVE_STATE;
}

/*******************************************************************************
* mvApPortResolutionParallelDetect
*
* DESCRIPTION: AP Port Resolution - Parallel Detect execution sequence
*              Read the power-up bits in AP status reg 0 (MPCS regfile).
*              As certain that exactly one of them is active.
*              resolution is done in ST_AN_GOOD_CK state so we need to
*              update state machine
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortResolutionParallelDetect(GT_U8 portIndex)
{
    GT_U8 pcsNum;
    GT_U32 plData;

    MV_HWS_AP_SM_INFO *apSm = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);

    pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    /* AP ARB state machine - ST_AN_GOOD_CK */
    mvApArbFsmSet(pcsNum, ST_AN_GOOD_CK);
    AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_AN_GOOD_CK);

    /* AP state & status update */
    apSm->status = AP_PORT_PD_RESOLUTION_IN_PROGRESS;
    mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

    CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_REG_ST_0, &plData, 0));

    /* Activate the matching symbol-lock bit in AP configuration reg 0/1 */
    switch (plData & AP_REG_ST_0_AP_PWT_UP_MASK)
    {
    case AP_REG_ST_0_AP_PWT_UP_1G_KX_MASK:
        AP_ST_HCD_TYPE_SET(apSm->capability,Port_1000Base_KX);
        break;

    case AP_REG_ST_0_AP_PWT_UP_10G_KX4_MASK: /* 10G XAUI */
        AP_ST_HCD_TYPE_SET(apSm->capability, Port_10GBase_KX4);
        break;

    default:
        apSm->status = AP_PORT_PD_RESOLUTION_FAILURE;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

        apSm->state = AP_PORT_SM_INIT_STATE;
        return GT_OK;
    }

    AP_ST_HCD_FOUND_SET(apSm->hcdStatus, 1);
    mvPortCtrlLogAdd(AP_PORT_DET_LOG_EXT(apSm->hcdStatus, apSm->portNum));

    /* AP ARB state machine - ST_AN_GOOD_CK */
    mvApArbFsmSet(pcsNum, ST_AN_GOOD_CK);
    AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_AN_GOOD_CK);

    /* AP state & status update */
    apSm->status = AP_PORT_RESOLUTION_SUCCESS;
    mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

    /* AP Resolution Success API */
    mvApPortResolutionSuccess(portIndex, PORT_CTRL_PD_LINK_CHECK_MODE);

    return GT_OK;
}

/*******************************************************************************
* mvApPortResolutionAP
*
* DESCRIPTION: AP Port Resolution - AP execution sequence
*              Read the power-up bits in AP status reg 0
*              As certain that exactly one of them is active
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortResolutionAP(GT_U8 portIndex)
{
    GT_U32 plData;
    GT_U32 data;
    GT_U32 hcdType = 0;
    GT_U8  pcsNum;

    MV_HWS_AP_SM_INFO       *apSm     = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_STATS      *apStats  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);
    MV_HWS_AP_DETECT_ITEROP *apIntrop = &(mvHwsPortCtrlApPortDetect.introp);

    pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    /* AP state & status update */
    apSm->status = AP_PORT_AP_RESOLUTION_IN_PROGRESS;
    AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_AN_GOOD_CK);
    mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

    CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                    AP_REG_ST_0, &plData, 0));

    /* HCD resolved bit[13] validate */
    if ((plData >> AP_REG_ST_0_HCD_RESOLVED_SHIFT) &
                   AP_REG_ST_0_HCD_RESOLVED_MASK)
    {
        /* Activate the matching symbol-lock bit in AP configuration */
        switch (plData & AP_REG_ST_0_AP_PWT_UP_MASK)
        {
        case AP_REG_ST_0_AP_PWT_UP_10G_KR_MASK:
            CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                            AP_REG_CFG_1, &data, 0));
            CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                            AP_REG_CFG_1, data | AP_REG_CFG_1_ADV_10G_KR_MASK, 0));
            hcdType = Port_10GBase_R;
            break;

        case AP_REG_ST_0_AP_PWT_UP_40G_KR4_MASK:
            CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                            AP_REG_CFG_0, &data, 0));
            CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                            AP_REG_CFG_0, data | AP_REG_CFG_0_ADV_40G_KR4_MASK, 0));
            hcdType = Port_40GBase_R;
            break;

        case AP_REG_ST_0_AP_PWT_UP_1G_KX_MASK:
            CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                            AP_REG_CFG_0, &data, 0));
            CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                            AP_REG_CFG_0, data | AP_REG_CFG_0_ADV_1G_KX_MASK, 0));
            hcdType = Port_1000Base_KX;
            break;

        case AP_REG_ST_0_AP_PWT_UP_10G_KX4_MASK: /* 10G XAUI */
            CHECK_STATUS(genUnitRegisterGet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                            AP_REG_CFG_1, &data, 0));
            CHECK_STATUS(genUnitRegisterSet(apDevNum, apPortGroup, MMPCS_UNIT, pcsNum,
                                            AP_REG_CFG_1, data | AP_REG_CFG_1_ADV_10G_KX4_MASK, 0));
            hcdType = Port_10GBase_KX4;
            break;

        default:
            break;
        }

        /* AP HCD update */
        AP_ST_HCD_FOUND_SET(apSm->hcdStatus, 1);
        AP_ST_HCD_TYPE_SET(apSm->hcdStatus, hcdType);
        AP_ST_HCD_FEC_RES_SET(apSm->hcdStatus, (plData >> 5) & 0x1);

        /* HCD Interrupt update */
        CHECK_STATUS(mvApFcResolutionSet(portIndex));
        mvHwsServCpuIntrSet(portIndex, MV_HWS_PORT_HCD_EVENT | (hcdType << MV_HWS_PORT_HCD_SHIFT));
        AP_ST_HCD_INT_TRIG_SET(apSm->hcdStatus, 1);
        mvPortCtrlLogAdd(AP_PORT_DET_LOG_EXT(apSm->hcdStatus, apSm->portNum));

        /* AP state & status update */
        apSm->status = AP_PORT_RESOLUTION_SUCCESS;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

        /* AP Resolution Success API */
        mvApPortResolutionSuccess(portIndex, PORT_CTRL_AP_LINK_CHECK_MODE);

        /* AP Timing measurment */
        apStats->hcdResoultionTime = mvPortCtrlCurrentTs() - apStats->timestampTime;
        apStats->timestampTime = mvPortCtrlCurrentTs();

        /* AP register dump */
        mvPortCtrlDbgCtrlRegsDump(portIndex, pcsNum, AP_PORT_SM_RESOLUTION_STATE);
        mvPortCtrlDbgIntRegsDump(portIndex, pcsNum, AP_PORT_SM_RESOLUTION_STATE);
        mvPortCtrlDbgAllIntRegsDump(portIndex, pcsNum, AP_PORT_SM_RESOLUTION_STATE);

        /* AP statistics update */
        apStats->abilitySuccessCnt++;
    }
    else /* NO Resolution */
    {
        /* AP Reset statistics and timers */
        mvApResetStatus(portIndex);
        mvApResetTimer(portIndex);

        /* AP state & status update */
        apSm->status = AP_PORT_AP_NO_RESOLUTION;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

        apSm->state = AP_PORT_SM_TX_DISABLE_STATE;

        /* AP validate Ability failure case */
        apStats->abilityInvalidCnt++;
        if (apStats->abilityInvalidCnt > apIntrop->abilityFailMaxInterval)
        {
            apSm->status = AP_PORT_AP_RESOLUTION_MAX_LOOP_FAILURE;
            mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

            apSm->state = AP_PORT_SM_INIT_STATE;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvApPortResolution
*
* DESCRIPTION: AP Port Resolution execution sequence
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortResolution(GT_U8 portIndex)
{
    MV_HWS_AP_SM_STATE fsmState;
    GT_U8 pcsNum;

    MV_HWS_AP_SM_INFO       *apSm     = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_TIMER      *apTimer  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].timer);
    MV_HWS_AP_SM_STATS      *apStats  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);
    MV_HWS_AP_DETECT_ITEROP *apIntrop = &(mvHwsPortCtrlApPortDetect.introp);

    pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    /* Check Ability detect threshold */
    if (!(mvPortCtrlThresholdCheck(apTimer->abilityThreshold)))
    {
        return GT_OK;
    }

    /* Validate Ability detect max interval, in case crossed update state to INIT */
    apTimer->abilityCount++;
    if(apTimer->abilityCount >= apIntrop->abilityMaxInterval)
    {
        /* AP ARB state machine - AN_ENABLE */
        mvApArbFsmSet(pcsNum, ST_AN_ENABLE);
        AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_AN_ENABLE);

        /* AP state & status update */
        apSm->status = AP_PORT_AP_RESOLUTION_TIMER_FAILURE;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

        apSm->state = AP_PORT_SM_INIT_STATE;
        return GT_OK;
    }

    /* Ability timer restart */
    mvPortCtrlThresholdSet(apIntrop->abilityDuration, &(apTimer->abilityThreshold));

    /* AP statistics update */
    apStats->abilityCnt++;

    /* Extract AP ARB state */
    fsmState = mvApArbFsmGet(pcsNum);

    if (apSm->status != AP_PORT_RESOLUTION_IN_PROGRESS)
    {
        /* AP state & status update */
        AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, fsmState);
        apSm->status = AP_PORT_RESOLUTION_IN_PROGRESS;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));
    }

    switch (fsmState)
    {
    case ST_LINK_STAT_CK:
        CHECK_STATUS(mvApPortResolutionParallelDetect(portIndex));
        break;

    case ST_AN_GOOD_CK:
        CHECK_STATUS(mvApPortResolutionAP(portIndex));
        break;

    default:
        break;
    }

    return GT_OK;
}

/*******************************************************************************
*                            AP Port Link Check State                          *
* ============================================================================ *
*******************************************************************************/

/*******************************************************************************
* mvApPortLinkFailure
*
* DESCRIPTION: AP Port Link failure execution sequence
*
* INPUTS:
*       GT_U8  portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_STATUS mvApPortLinkFailure(GT_U8 portIndex)
{
    MV_HWS_AP_SM_INFO  *apSm    = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_STATS *apStats = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);

    GT_U8 pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    /* This point means 1 of 2:
    ** 1) during port power up, Max link check was exceeded so the port should
    **    be shut down and the AP process will start over.
    ** 2) link was up but now it's down so AP process should start over (after
    **    the port will be shut down).
    */

    /* Link Interrupt status reset */
    mvHwsServCpuIntrStatusReset(portIndex);

    /* Delete the port */
    apSm->status = AP_PORT_DELETE_IN_PROGRESS;
    mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

    if (mvApPortDelete(portIndex) != GT_OK)
    {
        return GT_OK;
    }

    /* AP statistics update */
    apStats->linkFailCnt++;

    /* clear symbol-lock (using Advertise set) */
    mvApHcdAdvertiseSet(pcsNum, AP_CTRL_ADV_ALL_GET(apSm->capability));

    /* AP Reset status  */
    mvApResetStatus(portIndex);

    /* AP state & status update */
    apSm->state = AP_PORT_SM_DELETE_STATE;

    return GT_OK;
}

/*******************************************************************************
* mvApPortLinkValidate
*
* DESCRIPTION: AP Port Link validate execution sequence
*
* INPUTS:
*       GT_U8  portIndex
*       GT_U16 linkCheckInterval
*       GT_U16 linkCheckDuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_STATUS mvApPortLinkValidate(GT_U8 portIndex, GT_U16 linkCheckInterval, GT_U16 linkCheckDuration)
{
    MV_HWS_PORT_CTRL_AP_INIT *portInit = &(mvHwsPortCtrlApPortMng.apInitStatus);
    MV_HWS_AP_SM_INFO        *apSm     = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_TIMER       *apTimer  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].timer);

    /* AP state & status update */
    apSm->status = AP_PORT_LINK_CHECK_VALIDATE;
    mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

    apTimer->linkCount++;

    /*
    ** In case port was port init Failure
    ** There is no need to wait and check link status and port delete is executed
    */
    if ((apTimer->linkCount >= linkCheckInterval) ||
        (*portInit & MV_PORT_CTRL_NUM(portIndex)))
    {
        /* Link check failure */
        mvApPortLinkFailure(portIndex);
    }
    else
    {
        /* Link check timer restart */
        mvPortCtrlThresholdSet(linkCheckDuration, &(apTimer->linkThreshold));
    }

    return GT_OK;
}

/*******************************************************************************
* mvApPortLinkUp
*
* DESCRIPTION: AP Port Link Up execution sequence
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_STATUS mvApPortLinkUp(GT_U8 portIndex)
{
    MV_HWS_PORT_STANDARD portMode;
    MV_HWA_AP_PORT_MODE  apPortMode;
    GT_BOOL              linkUp;
    GT_U16               linkCheckDuration;
    GT_U16               linkCheckInterval;
    GT_U8                pcsNum;

    MV_HWS_PORT_CTRL_PORT_SM *portSm   = &(mvHwsPortCtrlApPortMng.apPortSm[portIndex]);
    MV_HWS_AP_SM_INFO        *apSm     = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_TIMER       *apTimer  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].timer);
    MV_HWS_AP_SM_STATS       *apStats  = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);
    MV_HWS_AP_DETECT_ITEROP  *apIntrop = &(mvHwsPortCtrlApPortDetect.introp);

    pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    /* Check Link check threshold */
    if (!(mvPortCtrlThresholdCheck(apTimer->linkThreshold)))
    {
        return GT_OK;
    }

    if (apSm->status == AP_PORT_LINK_CHECK_START)
    {
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));
    }

    apPortMode = AP_ST_HCD_TYPE_GET(apSm->hcdStatus);
    portMode   = mvHwsApConvertPortMode(apPortMode);

    /* Check link on MAC according to HCD resolution */
    if ((apPortMode == Port_1000Base_KX) || (apPortMode == Port_10GBase_KX4))
    {
        linkCheckDuration = apIntrop->pdLinkDuration;
        linkCheckInterval = apIntrop->pdLinkMaxInterval;
    }
    else
    {
        linkCheckDuration = apIntrop->apLinkDuration;
        linkCheckInterval = apIntrop->apLinkMaxInterval;
    }

    if (portSm->state == PORT_SM_ACTIVE_STATE)
    {
        /* Extarct Link status */
        mvHwsPortLinkStatusGet(apDevNum, apDevNum, apSm->portNum, portMode, &linkUp);

        /* Port is Active & Link_UP detected */
        if (linkUp)
        {
            /* Link up process - only at first occurance */
            if (apSm->status != AP_PORT_LINK_CHECK_SUCCESS)
            {
                /* AP state & status update */
                apSm->status = AP_PORT_LINK_CHECK_SUCCESS;
                AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_AN_GOOD);
                mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

                /* AP HCD update */
                AP_ST_HCD_LINK_SET(apSm->hcdStatus, 1);

                /* Link Interrupt update */
                mvHwsServCpuIntrSet(portIndex, MV_HWS_PORT_LINK_EVENT);
                AP_ST_LINK_INT_TRIG_SET(apSm->hcdStatus, 1);
                mvPortCtrlLogAdd(AP_PORT_DET_LOG_EXT(apSm->hcdStatus, apSm->portNum));

                /* AP Timing measurment */
                apStats->linkUpTime = mvPortCtrlCurrentTs() - apStats->timestampTime;
                apStats->timestampTime = mvPortCtrlCurrentTs();

                /* AP statistics update */
                apStats->linkSuccessCnt++;

                /* AP register dump */
                mvPortCtrlDbgCtrlRegsDump(portIndex, pcsNum, AP_PORT_SM_ACTIVE_STATE);
            }

            /* Link check timer restart */
            mvPortCtrlThresholdSet(linkCheckDuration, &(apTimer->linkThreshold));
            apTimer->linkCount = 0;
        }
        /* Port is Active NO Link_UP detected */
        else
        {
            /* Port is Active and No link detected,
            ** The previous status was link success, therefore it means link down
            ** is detected first time
            ** In this case execute Link failure
            */
            if (apSm->status == AP_PORT_LINK_CHECK_SUCCESS)
            {
                /* AP state & status update */
                apSm->status = AP_PORT_LINK_CHECK_FAILURE;
                mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

                /* Link failure */
                mvApPortLinkFailure(portIndex);
                return GT_OK;
            }

            /* Port is Active and No link detected,
            ** The previous status was NOT link success
            ** It means that the port is in the process of bring up
            ** this case execute Link validate
            */
            mvApPortLinkValidate(portIndex, linkCheckInterval, linkCheckDuration);
        }
    }
    else /* portSm->state != PORT_SM_ACTIVE_STATE */
    {
        /* Port is Not Active
        ** It means that the port is in the process of bring up
        ** this case execute Link validate
        */
        mvApPortLinkValidate(portIndex, linkCheckInterval, linkCheckDuration);
    }

    return GT_OK;
}

/*******************************************************************************
* mvApPortDeleteValidate
*
* DESCRIPTION: AP Port Link Up execution sequence
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_STATUS mvApPortDeleteValidate(GT_U8 portIndex)
{
    MV_HWS_PORT_CTRL_PORT_SM *portSm  = &(mvHwsPortCtrlApPortMng.apPortSm[portIndex]);
    MV_HWS_AP_SM_INFO        *apSm    = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);
    MV_HWS_AP_SM_STATS       *apStats = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].stats);

    if (portSm->state == PORT_SM_IDLE_STATE)
    {
        /* AP state & status update */
        apSm->status = AP_PORT_DELETE_SUCCESS;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

        /* AP Timing reset */
        mvApResetStatsTimestamp(portIndex);

        /* AP Timing measurment */
        apStats->timestampTime = mvPortCtrlCurrentTs();

        /* AP state & status update */
        apSm->state = AP_PORT_SM_INIT_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
*                            AP Port Delete State                              *
* ============================================================================ *
*******************************************************************************/

/*******************************************************************************
* mvApPortDeleteMsg
*
* DESCRIPTION: AP Port Delete Msg execution sequence
*
* INPUTS:
*       GT_U8 portIndex
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_STATUS mvApPortDeleteMsg(GT_U8 portIndex)
{
    MV_HWS_AP_SM_INFO *apSm = &(mvHwsPortCtrlApPortDetect.apPortSm[portIndex].info);

    GT_U8 pcsNum = AP_CTRL_PCS_GET(apSm->ifNum);

    /* Validate AP port state & status */
    if (apSm->state != AP_PORT_SM_IDLE_STATE)
    {
        if ((apSm->state == AP_PORT_SM_ACTIVE_STATE) &&
            (apSm->status != AP_PORT_DELETE_IN_PROGRESS))
        {
            /* AP Port Delete */
            CHECK_STATUS(mvApPortDelete(portIndex));
        }

        /* PCS MUX to non AP mode */
        mvApPcsMuxReset(pcsNum);

        /* AP Reset All */
        mvApResetStatus(portIndex);
        mvApResetTimer(portIndex);
        mvApResetStats(portIndex);
        mvApResetStatsTimestamp(portIndex);

        /* AP ARB state machine - AN_ENABLE */
        AP_ST_ARB_FSM_SET(apSm->ARMSmStatus, ST_AN_ENABLE);

        /* AP state & status update */
        apSm->status = AP_PORT_DELETE_SUCCESS;
        mvPortCtrlLogAdd(AP_PORT_SM_LOG(apSm->state, apSm->status, apSm->portNum, apSm->ARMSmStatus));

        apSm->state  = AP_PORT_SM_IDLE_STATE;
    }

    return GT_OK;
}

