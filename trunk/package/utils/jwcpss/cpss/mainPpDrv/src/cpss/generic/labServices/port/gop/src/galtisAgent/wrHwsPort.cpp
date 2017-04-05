
/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the \"File\") is owned and distributed by Marvell
* International Ltd. and/or its affiliates (\"Marvell\") under the following
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
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"
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
* wrLn2LMS.c
*
* DESCRIPTION:
*       Implementation of the Galtis Wrapper functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*
*******************************************************************************/

/* General H Files */
#include <common/os/gtOs.h>
#include <common/system/system.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <common/macros/gtCommonFuncs.h>
#include <galtisAgent/wrapUtil/galtisValues.h>
#include <galtisAgent/wrapUtil/cmdUtils.h>
#include <port/mvHwsPortInitIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <port/mvHwsPortCfgIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <mac/mvHwsMacIf.h>
#include <private/mvPortModeElements.h>

extern "C" {
GT_STATUS cmdLibInitHwsPortInit
(
    GT_VOID
);
#ifndef WIN32
GT_STATUS hwMmpcsInterruptMask
(
    GT_U8   devNum,
    GT_U32  pcsNum,
    GT_BOOL enInt
);
#endif
GT_U32 runRxOnPort = 0;

/* this global parameter disables 40G/10G WA when enabled */
GT_U32 apEnableMask = 0;
}

/* ports Tx polarity info per core 64 serdes */
static GT_U32 txPortPolarity[2][2];
/* ports Rx polarity info per core 64 serdes */
static GT_U32 rxPortPolarity[2][2];

/*******************************************************************************
* wrHwsPortCreateSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortCreateSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      phyPortNum = inFields[0];
    GT_BOOL     lbPort;

    MV_HWS_PORT_STANDARD    portMode;
    MV_HWS_REF_CLOCK_SUP_VAL refClock;
    MV_HWS_REF_CLOCK_SOURCE  refClockSource;
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 txInv, rxInv, data, serdesNum;
    GT_U32 macNum, accessAddr, pcsNum;
    static GT_BOOL portPolarityInitDone = GT_FALSE;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    if ((!portPolarityInitDone) && ((HWS_DEV_SILICON_TYPE(devNum) == Puma3A0) || (HWS_DEV_SILICON_TYPE(devNum) == Puma3B0)))
    {
        /* read user define registers */
        genRegisterGet(0,0,0xF0,&txPortPolarity[0][0],0);
        genRegisterGet(0,0,0xF4,&txPortPolarity[0][1],0);
        genRegisterGet(0,0,0xF8,&rxPortPolarity[0][0],0);
        genRegisterGet(0,0,0xFC,&rxPortPolarity[0][1],0);
        genRegisterGet(1,1,0xF0,&txPortPolarity[1][0],0);
        genRegisterGet(1,1,0xF8,&rxPortPolarity[1][0],0);
        txPortPolarity[1][1] = 0;
        rxPortPolarity[1][1] = 0;
        portPolarityInitDone = GT_TRUE;
    }
    portMode = (MV_HWS_PORT_STANDARD)inFields[1];
    lbPort = (GT_BOOL)inFields[2];

    switch(inFields[3])
    {
    case 0:
      refClock = MHz_25;
      break;
    case 1:
      refClock = MHz_125;
      break;
    case 2:
      refClock = MHz_156;
      break;
    default:
      refClock = MHz_125;
      break;
    }

    switch(inFields[4])
    {
    case 0:
      refClockSource = PRIMARY_LINE_SRC;
      break;
    case 1:
      refClockSource = SECONDARY_LINE_SRC;
      break;
    default:
      refClockSource = PRIMARY_LINE_SRC;
      break;
    }
    /* run WA 10G/1G */
    if (HWS_DEV_SILICON_TYPE(devNum) == Lion2B0 && apEnableMask == 0)
    {
        switch (portMode)
        {
        case _10GBase_KR:
        case _10GBase_SR_LR:
            /* force link down on port */
            curPortParams = hwsPortsParamsArrayGet(devNum, devNum, phyPortNum);
            macNum = curPortParams[portMode].portMacNumber;
            accessAddr  = 0x088c0000 + macNum * 0x1000;

            genRegisterSet(devNum, devNum, accessAddr, (1 << 2), (1 << 2));
            break;
        case _40GBase_KR:
        case _40GBase_SR_LR:
            curPortParams = hwsPortsParamsArrayGet(devNum, devNum, phyPortNum);
            pcsNum = curPortParams[portMode].portPcsNumber;
#ifndef WIN32
            /* disable SYNC_Change (B12) interrupt */
            hwMmpcsInterruptMask(devNum,pcsNum, GT_FALSE);
#endif
            break;
        default:
            break;
        }
    }
    rc = mvHwsPortInit(devNum, devNum, phyPortNum, portMode, lbPort, refClock, refClockSource);

    /* enable interrupt to run 10G/40G WA*/
    if (HWS_DEV_SILICON_TYPE(devNum) == Lion2B0 && apEnableMask == 0)
    {
        curPortParams = hwsPortsParamsArrayGet(devNum, devNum, phyPortNum);
        switch (portMode)
        {
        case _10GBase_KR:
        case _10GBase_SR_LR:
            serdesNum = curPortParams[portMode].firstLaneNum;
            pcsNum = curPortParams[portMode].portPcsNumber;

            /* store mmpcs create port flag for WAs */
            accessAddr = 0xDC + 0x088c0400 + pcsNum * 0x1000;
            genRegisterSet(devNum, devNum, accessAddr, 0x8000, 0x8000);

            /* read SQ detect indication */
            accessAddr  = 0x09800000 + serdesNum * 0x400;
            genRegisterGet(devNum, devNum, accessAddr, &data, 0);
            if (((data >> 1) & 1) && (runRxOnPort))
            {
                /* run RX training only */
                osPrintf("\nRun RX training on port %d.", phyPortNum);
                mvHwsPortAutoTuneSet(devNum, devNum, phyPortNum, portMode, RxTrainingOnly, 0);
            }
            break;
        case _40GBase_KR:
        case _40GBase_SR_LR:
            pcsNum = curPortParams[portMode].portPcsNumber;

            /* store mmpcs create port flag for WAs */
            accessAddr = 0xDC + 0x088c0400 + pcsNum * 0x1000;
            genRegisterSet(devNum, devNum, accessAddr, 0x8000, 0x8000);

#ifndef WIN32
            hwMmpcsInterruptMask(devNum,pcsNum, GT_TRUE);
#endif
            break;
        default:
            break;
        }
    }

    if ((rc == GT_OK) && ((HWS_DEV_SILICON_TYPE(devNum) == Puma3A0) || (HWS_DEV_SILICON_TYPE(devNum) == Puma3B0)))
    {
        curPortParams = hwsPortsParamsArrayGet(devNum, devNum, phyPortNum);

        if (curPortParams[portMode].firstLaneNum < 32)
        {
            txInv = (txPortPolarity[devNum][0] >> curPortParams[portMode].firstLaneNum);
            rxInv = (rxPortPolarity[devNum][0] >> curPortParams[portMode].firstLaneNum);
        }
        else
        {
            txInv = (txPortPolarity[devNum][1] >> (curPortParams[portMode].firstLaneNum - 32));
            rxInv = (rxPortPolarity[devNum][1] >> (curPortParams[portMode].firstLaneNum - 32));
        }
        if ((portMode == INTLKN_24Lanes_6_25G) || (portMode == INTLKN_24Lanes_3_125G))
        {
            txInv += ((txPortPolarity[1][0] >> 24) << 16);
            rxInv += ((rxPortPolarity[1][0] >> 24) << 16);
        }
        if ((phyPortNum >= 32) &&
            ((portMode == _100GBase_KR10) || (portMode == _100GBase_SR10) || (portMode == CHGL)))
        {
            txInv &= 0xFF;
            rxInv &= 0xFF;
            txInv += (((txPortPolarity[devNum][0] >> 26) & 3) << 8);
            rxInv += (((rxPortPolarity[devNum][0] >> 26) & 3) << 8);
        }
        rc = mvHwsPortPolaritySet(devNum, devNum, phyPortNum, portMode, txInv, rxInv);

    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortDeleteSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortDeleteSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      phyPortNum = inFields[0];

    MV_HWS_PORT_STANDARD    portMode;
    MV_HWS_PORT_ACTION      action;
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 pcsNum, macNum, accessAddr;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
        portMode = (MV_HWS_PORT_STANDARD)inFields[1];
    switch(inFields[2])
    {
    case 0:
      action = PORT_POWER_DOWN;
      break;
    case 1:
      action = PORT_RESET;
      break;
    default:
      action = PORT_POWER_DOWN;
      break;
    }

    /* disable interrupt to run 10G/40G WA*/
    if (HWS_DEV_SILICON_TYPE(devNum) == Lion2B0)
    {
        switch (portMode)
        {
        case _10GBase_KR:
        case _10GBase_SR_LR:
        case _40GBase_KR:
        case _40GBase_SR_LR:
            hwsOsTimerWkFuncPtr(5);
            curPortParams = hwsPortsParamsArrayGet(devNum, devNum, phyPortNum);
            pcsNum = curPortParams[portMode].portPcsNumber;
#ifndef WIN32
            hwMmpcsInterruptMask(devNum,pcsNum, GT_FALSE);
#endif
            hwsOsTimerWkFuncPtr(5);
            break;
        default:
            break;
        }
    }

    rc = mvHwsPortReset(devNum, devNum, phyPortNum, portMode, action);

    if (HWS_DEV_SILICON_TYPE(devNum) == Lion2B0)
    {
        switch (portMode)
        {
        case _10GBase_KR:
        case _10GBase_SR_LR:
            /* disable force link down */
            curPortParams = hwsPortsParamsArrayGet(devNum, devNum, phyPortNum);
            macNum = curPortParams[portMode].portMacNumber;
            accessAddr  = 0x088C0000 + macNum * 0x1000;

            genRegisterSet(devNum, devNum, accessAddr, 0, (1 << 2));
            pcsNum = curPortParams[portMode].portPcsNumber;

            /* store mmpcs create port flag for WAs */
            accessAddr = 0xDC + 0x088c0400 + pcsNum * 0x1000;
            genRegisterSet(devNum, devNum, accessAddr, 0, 0x8000);

        break;
        case _40GBase_KR:
        case _40GBase_SR_LR:
            curPortParams = hwsPortsParamsArrayGet(devNum, devNum, phyPortNum);
            pcsNum = curPortParams[portMode].portPcsNumber;

            /* store mmpcs create port flag for WAs */
            accessAddr = 0xDC + 0x088c0400 + pcsNum * 0x1000;
            genRegisterSet(devNum, devNum, accessAddr, 0, 0x8000);
            break;
        default:
            break;
        }
    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortAutoTuneSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortAutoTuneSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      phyPortNum = inFields[0];

    MV_HWS_PORT_STANDARD    portMode;
    MV_HWS_PORT_AUTO_TUNE_MODE  portTuningMode;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
        portMode = (MV_HWS_PORT_STANDARD)inFields[1];

    switch(inFields[2])
    {
    case 1:
      portTuningMode = RxTrainingOnly;
      break;
    case 2:
      portTuningMode = TRxTuneCfg;
      break;
    case 3:
      portTuningMode = TRxTuneStart;
      break;
    case 4:
      portTuningMode = TRxTuneStatus;
      break;
    default:
      portTuningMode = RxTrainingOnly;
      break;
    }

    rc = mvHwsPortAutoTuneSet(devNum, devNum, phyPortNum, portMode, portTuningMode, 0);
    if (rc != GT_OK)
    {
        osPrintf("\nmvHwsPortAutoTuneSet FAILED on device %d, port %d, portTuningMode %d.",
                 devNum, phyPortNum, portTuningMode);
    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortLinkStatusGetFirstEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortLinkStatusGetFirstEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      phyPortNum = inArgs[1];
    GT_BOOL     link;

    MV_HWS_PORT_STANDARD    portMode;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
        portMode = (MV_HWS_PORT_STANDARD)inArgs[2];
    rc = mvHwsPortLinkStatusGet(devNum, devNum, phyPortNum, portMode, &link);

    inFields[1] = (link) ? 1:0;

    /* pack and output table fields */
    fieldOutput("%d%d",phyPortNum,inFields[1]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortLinkStatusGetNextEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortLinkStatusGetNextEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortLoopbackCfgSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortLoopbackCfgSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      phyPortNum = inFields[0];

    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    MV_HWS_PORT_STANDARD    portMode;
    MV_HWS_UNIT             lpPlace;
    MV_HWS_PORT_LB_TYPE     lbType;
    GT_U32 txInv, rxInv;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
        portMode = (MV_HWS_PORT_STANDARD)inFields[1];

    switch(inFields[2])
    {
    case 0:
      lbType = DISABLE_LB;
      break;
    case 1:
      lbType = RX_2_TX_LB;
      break;
    case 2:
      lbType = TX_2_RX_LB;
      break;
    default:
      lbType = DISABLE_LB;
      break;
    }

    switch(inFields[3])
    {
    case 0:
      lpPlace = HWS_MAC;
      break;
    case 1:
      lpPlace = HWS_PCS;
      break;
    case 2:
      lpPlace = HWS_PMA;
      break;
    default:
      lpPlace = HWS_MAC;
      break;
    }

    curPortParams = hwsPortsParamsArrayGet(HWS_DEV_SILICON_TYPE(devNum), phyPortNum);
    if (lpPlace == HWS_MAC)
    {
        if (curPortParams[portMode].portPcsType == XPCS)
        {
            CHECK_CMD_STATUS(mvHwsPcsReset(devNum,devNum,curPortParams[portMode].portPcsNumber,curPortParams[portMode].portPcsType,RESET));
        }
        CHECK_CMD_STATUS(mvHwsMacReset(devNum,devNum,curPortParams[portMode].portMacNumber,curPortParams[portMode].portMacType,RESET));
    }

    rc = mvHwsPortLoopbackSet(devNum, devNum, phyPortNum, portMode, lpPlace, lbType);

    if (lpPlace == HWS_MAC)
    {
        if (curPortParams[portMode].portPcsType == XPCS)
        {
            CHECK_CMD_STATUS(mvHwsPcsReset(devNum,devNum,curPortParams[portMode].portPcsNumber,curPortParams[portMode].portPcsType,UNRESET));
        }
        CHECK_CMD_STATUS(mvHwsMacReset(devNum,devNum,curPortParams[portMode].portMacNumber,curPortParams[portMode].portMacType,UNRESET));
    }

    if (lpPlace == HWS_PMA)
    {
        if (lbType == TX_2_RX_LB)
        {
            /* disable lane swap */
            rc = mvHwsPortPolaritySet(devNum, devNum, phyPortNum, portMode, GT_FALSE, GT_FALSE);
        }
        if (lbType == DISABLE_LB)
        {
            if ((rc == GT_OK) && ((HWS_DEV_SILICON_TYPE(devNum) == Puma3A0) || (HWS_DEV_SILICON_TYPE(devNum) == Puma3B0)))
            {
                if (curPortParams[portMode].firstLaneNum < 32)
                {
                    txInv = (txPortPolarity[devNum][0] >> curPortParams[portMode].firstLaneNum);
                    rxInv = (rxPortPolarity[devNum][0] >> curPortParams[portMode].firstLaneNum);
                }
                else
                {
                    txInv = (txPortPolarity[devNum][1] >> (curPortParams[portMode].firstLaneNum - 32));
                    rxInv = (rxPortPolarity[devNum][1] >> (curPortParams[portMode].firstLaneNum - 32));
                }
                if ((portMode == INTLKN_24Lanes_6_25G) || (portMode == INTLKN_24Lanes_3_125G))
                {
                    txInv += ((txPortPolarity[1][0] >> 24) << 16);
                    rxInv += ((rxPortPolarity[1][0] >> 24) << 16);
                }
                rc = mvHwsPortPolaritySet(devNum, devNum, phyPortNum, portMode, txInv, rxInv);
            }
        }
    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortManualTuneCfgSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortManualTuneCfgSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      phyPortNum = inFields[0];

    MV_HWS_PORT_STANDARD    portMode;
    MV_HWS_PORT_MAN_TUNE_MODE portTuningMode;
    MV_HWS_AUTO_TUNE_RESULTS tunParams;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
        portMode = (MV_HWS_PORT_STANDARD)inFields[1];

    switch(inFields[2])
    {
    case 0:
      portTuningMode =   StaticLongReach;
      break;
    case 1:
      portTuningMode = StaticShortReach;
      break;
    default:
      portTuningMode = StaticShortReach;
      break;
    }

    if (portTuningMode == StaticLongReach)
    {
     /* RX tuning params */
      tunParams.sqleuch = 1;
      tunParams.ffeR = 2;
      tunParams.ffeC = 15;
      tunParams.align90 = 0x50;
      /* TX tuning params */
      tunParams.txAmp = 0x1f;
      tunParams.txEmph0 = 0xB;
      tunParams.txEmph1 = 2;
    }
    else
    {
      /* RX tuning params */
      tunParams.sqleuch = 3;
      tunParams.ffeR = 5;
      tunParams.ffeC = 13;
      tunParams.align90 = 0x56;
      /* TX tuning params */
      tunParams.txAmp = 0x19;
      tunParams.txEmph0 = 0x5;
      tunParams.txEmph1 = 1;
    }

    rc = mvHwsPortManTuneSet(devNum, devNum, phyPortNum, portMode, portTuningMode, &tunParams);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortRemapSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortRemapSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       phyPortNum, devNum;

    MV_HWS_PORT_STANDARD    portMode;
    GT_U32    sdVec[32];
    GT_U32    sdSize;
    GT_U32    tmp;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    devNum = inArgs[0];
    phyPortNum = inArgs[1];

    portMode = (MV_HWS_PORT_STANDARD)inFields[0];
    tmp = (GT_U32)inFields[3];
    sdSize = 0;
    for (int i = 0; i < 32; i++)
    {
      if ((tmp >> i) & 1)
      {
        sdVec[sdSize] = i;
        sdSize++;
      }
    }

    hwsPortsParamsCfg(devNum, devNum, phyPortNum, portMode,(GT_U32)inFields[1], (GT_U32)inFields[2], sdSize, sdVec);

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

static GT_U32    wrPortRemapPortModeCur;

/*******************************************************************************
* wrHwsPortRemapGetFirstEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortRemapGetFirstEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8     phyPortNum, devNum;
    GT_U32    macNum;
    GT_U32    pcsNum;
    GT_U32    *sdVec;
    GT_U32    sdSize;
    GT_U32    tmp, i;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    devNum = inArgs[0];
    phyPortNum = inArgs[1];

    wrPortRemapPortModeCur = 0;//_100Base_FX;
    sdSize = 0;
    hwsPortsParamsCfgGet(devNum, portGroup, phyPortNum, (MV_HWS_PORT_STANDARD)wrPortRemapPortModeCur, &macNum, &pcsNum, &sdSize, &sdVec);

    inFields[1] = macNum;
    inFields[2] = pcsNum;
    tmp = 0;
    for (i = 0; i < sdSize; i++)
    {
      tmp += (1 << sdVec[i]);
    }
    inFields[3] = tmp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d",wrPortRemapPortModeCur,inFields[1], inFields[2], inFields[3]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortRemapGetNexttEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortRemapGetNextEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       phyPortNum, devNum;
    GT_U32      macNum;
    GT_U32      pcsNum;
    GT_U32      *sdVec;
    GT_U32      sdSize;
    GT_U32      tmp, i;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    devNum = inArgs[0];
    phyPortNum = inArgs[1];

    wrPortRemapPortModeCur++;
    sdSize = 0;
    if (wrPortRemapPortModeCur >= NON_SUP_MODE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    hwsPortsParamsCfgGet(devNum, portGroup, phyPortNum, (MV_HWS_PORT_STANDARD)wrPortRemapPortModeCur, &macNum, &pcsNum, &sdSize, &sdVec);

    inFields[1] = macNum;
    inFields[2] = pcsNum;
    tmp = 0;
    for (i = 0; i < sdSize; i++)
    {
      tmp += (1 << sdVec[i]);
    }
    inFields[3] = tmp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d",wrPortRemapPortModeCur,inFields[1], inFields[2], inFields[3]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortPolaritySet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortPolaritySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /* Fields List */
    GT_U32  PortNum  = (GT_U32)inFields[0];
    GT_U32  PortMode = (GT_U32)inFields[1];
    GT_U32  TxInv    = (GT_U32)inFields[2];
    GT_U32  RxInv    = (GT_U32)inFields[3];

    res = mvHwsPortPolaritySet(DevNum, DevNum, PortNum, (MV_HWS_PORT_STANDARD)PortMode, TxInv, RxInv);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortTxDisableSet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortTxDisableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /* Fields List */
    GT_U32  PortNum  = (GT_U32)inFields[0];
    GT_U32  PortMode = (GT_U32)inFields[1];
    GT_U32  Action   = (GT_U32)inFields[2];

    res = mvHwsPortTxEnable(DevNum, DevNum, PortNum, (MV_HWS_PORT_STANDARD)PortMode, (GT_BOOL)Action);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortPPMSet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortPPMSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /* Fields List */
    GT_U32  PortNum  = (GT_U32)inFields[0];
    GT_U32  PortMode = (GT_U32)inFields[1];
    GT_U32  PPM      = (GT_U32)inFields[2];

    res = mvHwsPortPPMSet(DevNum, DevNum, PortNum, (MV_HWS_PORT_STANDARD)PortMode, (MV_HWS_PPM_VALUE)PPM);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}


/*******************************************************************************
* wrHwsPortFecConfigSet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortFecConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /* Fields List */
    GT_U32  PortNum  = (GT_U32)inFields[0];
    GT_U32  PortMode = (GT_U32)inFields[1];
    GT_BOOL fecEn    = (GT_BOOL)inFields[2];

    res = mvHwsPortFecCofig(DevNum, DevNum, PortNum, (MV_HWS_PORT_STANDARD)PortMode, fecEn);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsClearChannelCfgSet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsClearChannelCfgSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  devNum = (GT_U32)inArgs[0];
    GT_U32  portNum  = (GT_U32)inArgs[1];
    GT_U32  portMode = (GT_U32)inArgs[2];

    /* Fields List */
    GT_U32  txIpg = (GT_U32)inFields[0];
    GT_U32  txPreamble = (GT_U32)inFields[1];
    GT_U32  rxPreamble = (GT_U32)inFields[2];
    GT_U32  txCrc = (GT_U32)inFields[3];
    GT_U32  rxCrc = (GT_U32)inFields[4];

    res = mvHwsPortClearChannelCfg(devNum, devNum, portNum, (MV_HWS_PORT_STANDARD)portMode,
                                   txIpg, txPreamble, rxPreamble, (MV_HWS_PORT_CRC_MODE)txCrc, (MV_HWS_PORT_CRC_MODE)rxCrc);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPortAcTerminationSet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPortAcTerminationSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  devNum = (GT_U32)inArgs[0];
    /* Fields List */
    GT_U32  portNum  = (GT_U32)inFields[0];
    GT_U32  portMode = (GT_U32)inFields[1];
    GT_U32  action = (GT_U32)inFields[2];

    res = mvHwsPortAcTerminationCfg(devNum, devNum, portNum, (MV_HWS_PORT_STANDARD)portMode,
                                   (GT_BOOL)action);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
*
* dbCommands - Galtis database galtis commands
*
*******************************************************************************/
static CMD_COMMAND dbCommands[] = {
{"HwsPortCreateSet", &wrHwsPortCreateSetEntry, 1,  5},
{"HwsPortDeleteSet", &wrHwsPortDeleteSetEntry, 1,  3},
{"HwsPortAutoTuneSet", &wrHwsPortAutoTuneSetEntry, 1,  3},
{"HwsPortLinkStatusGetFirst", &wrHwsPortLinkStatusGetFirstEntry, 3,  0},
{"HwsPortLinkStatusGetNext", &wrHwsPortLinkStatusGetNextEntry, 3,  0},
{"HwsPortLoopbackCfgSet", &wrHwsPortLoopbackCfgSetEntry, 1,  4},
{"HwsPortManualTuneCfgSet", &wrHwsPortManualTuneCfgSetEntry, 1,  3},
{"HwsPortRemapGetFirst", &wrHwsPortRemapGetFirstEntry, 2,  0},
{"HwsPortRemapGetNext", &wrHwsPortRemapGetNextEntry, 2,  0},
{"HwsPortRemapSet", &wrHwsPortRemapSetEntry, 2,  4},
{"HwsPortPolaritySet", &wrHwsPortPolaritySet, 1, 4},
{"HwsPortTxDisableSet", &wrHwsPortTxDisableSet, 1, 3},
{"HwsPortPPMSet", &wrHwsPortPPMSet, 1, 3},
{"HwsPortFecConfigSet", &wrHwsPortFecConfigSet, 1, 3},
/*{"HwsClearChannelCfgGetFirst",&wrHwsClearChannelCfgGetFirst, 3, 0},
{"HwsClearChannelCfgGetNext",&wrHwsClearChannelCfgGetNext, 3, 0},*/
{"HwsClearChannelCfgSet",&wrHwsClearChannelCfgSet, 3, 5},
{"HwsPortAcTerminationSet", &wrHwsPortAcTerminationSet, 1, 3},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitLn2LMSExt
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitHwsPortInit()
{
    return cmdInitLibrary(dbCommands, numCommands);
}

