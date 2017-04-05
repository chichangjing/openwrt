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
* mvComPhyH28nmRev3If.c
*
* DESCRIPTION:
*         Com PHY H (serdes) interface
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <serdes/mvHwsSerdesIf.h>
#include <serdes/mvHwsSerdes28nmPrvIf.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmDb.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmEomIf.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3Db.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3If.h>

#ifndef CO_CPU_RUN
static char* mvHwsComH28nmRev3SerdesTypeGetFunc(void)
{
  return "COM_H_28NM_REV3";
}
#endif

/************************* Globals *******************************************************/

static GT_BOOL hwsInitDone = GT_FALSE;
static GT_BOOL loopbackMode = GT_FALSE;

/************************* pre-declaration ***********************************************/

extern GT_STATUS mvHwsComH28nmCheckSerdesAccess(GT_U8 devNum, GT_UOPT  portGroup, GT_UOPT  serdesNum);

static GT_STATUS mvHwsComH28nmRev3SerdesTxAutoTuneStop
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
);

static GT_STATUS mvHwsComH28nmRev3SerdesDfeScale
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         serdesNum,
    GT_UREG_DATA    *ffeR
);

static GT_STATUS mvHwsComH28nmRev3SerdesDfeSatGet
(
    GT_U8   devNum,
    GT_U32  *dfeSat
);

GT_STATUS mvHwsComH28nmRev3SerdesAutoTuneStatus
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    MV_HWS_AUTO_TUNE_STATUS     *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS     *txStatus
);

GT_STATUS mvHwsComH28nmRev3SerdesFixAlign90Start
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    MV_HWS_ALIGN90_PARAMS   *params
);

GT_STATUS mvHwsComH28nmRev3SerdesFixAlign90Status
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *trainingStatus
);

GT_STATUS mvHwsComH28nmRev3SerdesFixAlign90Stop
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 serdesNum,
    MV_HWS_ALIGN90_PARAMS *params,
    GT_BOOL fixAlignPass
);

GT_STATUS mvHwsComH28nmSerdesScanSampler
(
    GT_U8     devNum,
    GT_U8     portGroup,
    GT_U32    serdesNum,
    GT_U32    waitTime,
    GT_U32    samplerSelect,
    GT_U32    errorThreshold,
    MV_HWS_SAMPLER_RESULTS  *samplerResults
);

/*******************************************************************************
* mvHwsComH28nmRev3IfInit
*
* DESCRIPTION:
*       Init Com_H serdes configuration sequences and IF functions.
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
GT_STATUS mvHwsComH28nmRev3IfInit(MV_HWS_SERDES_FUNC_PTRS *funcPtrArray)
{

    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsComPhyH28nmRev3SeqInit());
    }

    /* 28nm implementation */
    funcPtrArray[COM_PHY_28NM].serdesArrayPowerCntrlFunc    = NULL;
    funcPtrArray[COM_PHY_28NM].serdesPowerCntrlFunc         = mvHwsComH28nmSerdesPowerCtrl;
    funcPtrArray[COM_PHY_28NM].serdesManualTxCfgFunc        = mvHwsComH28nmSerdesManualTxConfig;
    funcPtrArray[COM_PHY_28NM].serdesAutoTuneCfgFunc        = mvHwsComH28nmSerdesAutoTuneCfg;
    funcPtrArray[COM_PHY_28NM].serdesRxAutoTuneStartFunc    = mvHwsComH28nmSerdesRxAutoTuneStart;
    funcPtrArray[COM_PHY_28NM].serdesTxAutoTuneStartFunc    = mvHwsComH28nmSerdesTxAutoTuneStart;
    funcPtrArray[COM_PHY_28NM].serdesAutoTuneStartFunc      = mvHwsComH28nmSerdesAutoTuneStart;
    funcPtrArray[COM_PHY_28NM].serdesAutoTuneResultFunc     = mvHwsComH28nmSerdesAutoTuneResult;
    funcPtrArray[COM_PHY_28NM].serdesResetFunc              = mvHwsComH28nmSerdesReset;
    funcPtrArray[COM_PHY_28NM].serdesDigitalReset           = mvHwsComH28nmSerdesDigitalReset;
    funcPtrArray[COM_PHY_28NM].serdesCoreReset              = mvHwsComH28nmSerdesCoreReset;
    funcPtrArray[COM_PHY_28NM].serdesSeqGetFunc             = hwsComPhy28nmSeqGet;
    funcPtrArray[COM_PHY_28NM].serdesSeqSetFunc             = hwsComPhy28nmSeqSet;
    funcPtrArray[COM_PHY_28NM].serdesDfeCfgFunc             = mvHwsComH28nmSerdesDfeConfig;
    funcPtrArray[COM_PHY_28NM].serdesLoopbackCfgFunc        = mvHwsComH28nmSerdesLoopback;
    funcPtrArray[COM_PHY_28NM].serdesLoopbackGetFunc        = mvHwsComH28nmSerdesLoopbackGet;
    funcPtrArray[COM_PHY_28NM].serdesPpmCfgFunc             = mvHwsComH28nmSerdesPpmConfig;
    funcPtrArray[COM_PHY_28NM].serdesPpmGetFunc             = mvHwsComH28nmSerdesPpmConfigGet;
    funcPtrArray[COM_PHY_28NM].serdesTestGenFunc            = mvHwsComH28nmSerdesTestGen;
    funcPtrArray[COM_PHY_28NM].serdesTestGenGetFunc         = mvHwsComH28nmSerdesTestGenGet;
    funcPtrArray[COM_PHY_28NM].serdesTestGenStatusFunc      = mvHwsComH28nmSerdesTestGenStatus;
    funcPtrArray[COM_PHY_28NM].serdesEomGetFunc             = mvHwsComH28nmSerdesEOMGet;
    funcPtrArray[COM_PHY_28NM].serdesDfeStatusFunc          = NULL;
    funcPtrArray[COM_PHY_28NM].serdesDfeStatusExtFunc       = mvHwsComH28nmSerdesDfeStatus;
    funcPtrArray[COM_PHY_28NM].serdesPolarityCfgFunc        = mvHwsComH28nmSerdesPolarityConfig;
    funcPtrArray[COM_PHY_28NM].serdesPolarityCfgGetFunc     = mvHwsComH28nmSerdesPolarityConfigGet;
    funcPtrArray[COM_PHY_28NM].serdesFfeCfgFunc             = NULL;
    funcPtrArray[COM_PHY_28NM].serdesCalibrationStatusFunc  = mvHwsComH28nmSerdesCalibrationStatus;
    funcPtrArray[COM_PHY_28NM].serdesTxEnableFunc           = mvHwsComH28nmSerdesTxEnable;
    funcPtrArray[COM_PHY_28NM].serdesTxEnableGetFunc        = mvHwsComH28nmSerdesTxEnableGet;
    funcPtrArray[COM_PHY_28NM].serdesTxIfSelectFunc         = mvHwsComH28nmSerdesTxIfSelect;
    funcPtrArray[COM_PHY_28NM].serdesTxIfSelectGetFunc      = mvHwsComH28nmSerdesTxIfSelectGet;
    funcPtrArray[COM_PHY_28NM].serdesSqlchCfgFunc           = NULL;
    funcPtrArray[COM_PHY_28NM].serdesAcTermCfgFunc          = mvHwsComH28nmSerdesAcTerminationCfg;
    funcPtrArray[COM_PHY_28NM].serdesAutoTuneStopFunc       = mvHwsComH28nmRev3SerdesTxAutoTuneStop;
    funcPtrArray[COM_PHY_28NM].serdesDfeCheckFunc           = mvHwsComH28nmDfeCheck;
    funcPtrArray[COM_PHY_28NM].serdesManualRxCfgGetFunc     = mvHwsComH28nmSerdesManualRxConfigGet;
    funcPtrArray[COM_PHY_28NM].serdesCdrLockStatusGetFunc   = NULL;
    funcPtrArray[COM_PHY_28NM].serdesScanSamplerFunc        = mvHwsComH28nmSerdesScanSampler;
    funcPtrArray[COM_PHY_28NM].serdesAutoTuneStatusShortFunc= mvHwsComH28nmSerdesAutoTuneStatusShort;

    /* 28nmRev3 implementation */
    funcPtrArray[COM_PHY_28NM].serdesTypeGetFunc            = mvHwsComH28nmRev3SerdesTypeGetFunc;
    funcPtrArray[COM_PHY_28NM].serdesAutoTuneStatusFunc     = mvHwsComH28nmRev3SerdesAutoTuneStatus;
    funcPtrArray[COM_PHY_28NM].serdesDfeOptiFunc            = mvHwsComH28nmRev3SerdesDfeScale;
    funcPtrArray[COM_PHY_28NM].serdesManualRxCfgFunc        = mvHwsComH28nmRev3SerdesManualRxConfig;
    funcPtrArray[COM_PHY_28NM].serdesFixAlign90Start        = mvHwsComH28nmRev3SerdesFixAlign90Start;
    funcPtrArray[COM_PHY_28NM].serdesFixAlign90Stop         = mvHwsComH28nmRev3SerdesFixAlign90Stop;
    funcPtrArray[COM_PHY_28NM].serdesFixAlign90Status       = mvHwsComH28nmRev3SerdesFixAlign90Status;
    funcPtrArray[COM_PHY_28NM].serdesPartialPowerDownFunc   = mvHwsComH28nmRev3SerdesPartialPowerDown;
    funcPtrArray[COM_PHY_28NM].serdesPartialPowerStatusGetFunc = mvHwsComH28nmRev3SerdesPartialPowerStatusGet;
    funcPtrArray[COM_PHY_28NM].serdesSpeedGetFunc           = mvHwsComH28nmRev3SerdesSpeedGet;
    funcPtrArray[COM_PHY_28NM].serdesSignalDetectGetFunc    = mvHwsComH28nmRev3SerdesSignalDetectGet;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesTxAutoTuneStop
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesTxAutoTuneStop
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
)
{
	/* only disable TRx tuning (no training results copy) */
    CHECK_STATUS(mvHwsComH28nmSerdesTxAutoTuneStart(devNum, portGroup, serdesNum, GT_FALSE));

	return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesManualRxConfig
*
* DESCRIPTION:
*       Per SERDES configure the RX parameters: squelch Threshold, FFE and DFE
*       operation
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical lane number
*       portTuningMode - lane tuning mode (short / long)
*       sqlch     - Squelch threshold
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesManualRxConfig
(
    GT_U8    devNum,
    GT_UOPT  portGroup,
    GT_UOPT  serdesNum,
    MV_HWS_PORT_MAN_TUNE_MODE portTuningMode,
    GT_UREG_DATA  sqlch,
    GT_UREG_DATA  ffeRes,
    GT_UREG_DATA  ffeCap,
    GT_BOOL		  dfeEn,
    GT_UREG_DATA  alig
)
{
	GT_UREG_DATA data;
	GT_32        curVal, tmpAlig;

	sqlch = sqlch;

	CHECK_STATUS(mvHwsComH28nmCheckSerdesAccess(devNum, portGroup, serdesNum));

	if (portTuningMode == StaticLongReach)
	{
		/* only in Long Reach - set 3mv DFE resolution */
		CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV3_G1_SET_4_Reg, 0x0, 0x300));
		CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV2_DFE_LOCK_Reg, 0x8000, 0x8000));
	}
	else
	{
		/* only in Long Reach - set 4mv DFE resolution */
		CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV3_G1_SET_4_Reg, 0x100, 0x300));
		CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV2_DFE_LOCK_Reg, 0x8000, 0x8000));
	}

	/* set bit 7 to 1 */
	CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV3_G1_SET_3_Reg, ( 1 << 7 ), ( 1 << 7 )));

	CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV3_G1_SET_3_Reg, ( ffeRes << 4 ), ( 7 << 4 )));

	CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV3_G1_SET_3_Reg, ffeCap, 0xf));

	CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Squelch_FFE_Reg, (sqlch << 8), (0xf << 8)));

	data = dfeEn ? 1 : 0;
	CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_2, (data << 4), (1 << 4)));

	/*  set bit 7 of COM_H_REV2_OS_PH_OFFSET_REG os_ph_force*/
	CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV2_OS_PH_OFFSET_Reg, (1 << 7), (1 << 7)));

	/* config align90 */
	/* read current align90 value */
	CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV2_OS_PH_Reg, &data , 0));

    curVal = (data >> 9) & 0x7F;
    tmpAlig = alig;
    /* move to align90 value with step = 4 */
    if (((curVal - 4) > tmpAlig) || ((GT_UREG_DATA)(curVal + 4) < alig))
    {
        while ((curVal - 4) > tmpAlig)
        {
            curVal -= 4;
            /* Set fource Align90 value */
            CHECK_STATUS(mvHwsComH28nmSerdesForceAlign90Set(devNum, portGroup, serdesNum, curVal));
        }

        while ((GT_UREG_DATA)(curVal + 4) < alig)
        {
            curVal += 4;
            /* Set force Align90 value */
            CHECK_STATUS(mvHwsComH28nmSerdesForceAlign90Set(devNum, portGroup, serdesNum, curVal));
        }
    }

    /* Set force Align90 value */
    CHECK_STATUS(mvHwsComH28nmSerdesForceAlign90Set(devNum, portGroup, serdesNum, alig));

	return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesSpeedGet
*
* DESCRIPTION:
*       Return SERDES baud rate.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       baudRate - baud rate.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesSpeedGet
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_SPEED *rate
)
{
    GT_U32 data, fbDivVal;

    if (rate == NULL)
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsComH28nmCheckSerdesAccess(devNum, portGroup, serdesNum));

    /* read FBDiv (for speeds in which SERDES_EXTERNAL_CONFIGURATION_0 value is the same */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_FeedbackDivider_Reg, &fbDivVal, 0));

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &data , 0));
    switch (data & 0x7F8)
    {
    case 0x330:
        *rate = _1_25G;
        break;
    case 0x440:
        *rate = _3_125G;
        break;
    case 0x4C8:
        *rate = _3_75G;
        break;
    case 0x550:
        *rate = _4_25G;
        break;
    case 0x198:
        *rate = (fbDivVal == 0xA244) ? _5_625G : _5G;
        break;
    case 0x5D8:
        *rate = _6_25G;
        break;
    case 0x6E8:
        *rate = _7_5G;
        break;
    case 0x770:
        *rate = _10_3125G;
        break;
    case 0x7F8:
        switch(fbDivVal)
        {
        case 0xA234:
            *rate = _10_9375G;
            break;
        case 0xA244:
            *rate = _11_25G;
            break;
        case 0xA277:
            *rate = _12_1875G;
            break;
        default:
            /* TBD - what about _11_5625G & _12_5G */
            *rate = SPEED_NA;
        }
    default:
        *rate = SPEED_NA;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesDfeSatGet
*
* DESCRIPTION:
*       Get the DfeSat value according to the current temperature of DFX server.
*
* INPUTS:   devNum - system device number
*
* OUTPUTS:  dfeSat value
*
* RETURNS:
*       GT_OK - on success
********************************************************************************/
static GT_STATUS mvHwsComH28nmRev3SerdesDfeSatGet
(
    GT_U8   devNum,
    GT_U32  *dfeSat
)
{
    GT_U32  data;
    GT_BOOL negative;
    GT_U32  temperature;

    /* get the temperature value from Server register=0xF8078[9:0] */
    hwsServerRegGetFuncPtr(devNum, MV_SERVER_Temp_Reg, &data);

    data = data & 0x3FF;
    if (data < 596)
    {
        negative = GT_TRUE;
        /* Convert float to int due to missing float capability in Free RTOS
        ** Multiply by 1000 each floating element
        ** Originaly defined "(float)((596 - data) / 2.154)" */
        temperature = (((596 - data) * 1000) / 2154);
    }
    else
    {
        negative = GT_FALSE;
        /* Convert float to int due to missing float capability in Free RTOS
        ** Multiply by 1000 each floating element
        ** Originaly defined "(float)((data - 596) / 2.154)" */
        temperature = (((data - 596) * 1000) / 2154);
    }

    *dfeSat = (negative) ? (GT_U32)(53 + temperature/9) : (GT_U32)(53 - temperature/9);

    return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesPartialPowerDown
*
* DESCRIPTION:
*       Enable/Disable Serdes power down on Tx and Rx.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - number of SERDESes to configure
*       powerDownRx - Enable/Disable the power down on Serdes Rx
*       powerDownTx - Enable/Disable the power down on Serdes Tx
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesPartialPowerDown
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     powerDownRx,
    GT_BOOL     powerDownTx
)
{
    GT_U32  setMode, setRxMode, setTxMode;

    setRxMode = (!powerDownRx) << 11;
    setTxMode = (!powerDownTx) << 12;
    setMode = setRxMode | setTxMode;

    /* Serdes power down set Rx bit[11] and Tx bit[12] */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, setMode, (3 << 11)));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesPartialPowerStatusGet
*
* DESCRIPTION:
*       Get the status of power Tx and Rx on Serdes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - number of SERDESes to configure
*       powerRxStatus - GT_TRUE - Serdes power Rx is down
*                       GT_FALSE - Serdes power Rx is up
*       powerTxStatus - GT_TRUE - Serdes power Tx is down
*                       GT_FALSE - Serdes power Tx is up
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesPartialPowerStatusGet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     *powerRxStatus,
    GT_BOOL     *powerTxStatus
)
{
    GT_U32 serdesPowerStatus;

    /* Serdes power down get Rx bit[11] and Tx bit[12] */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &serdesPowerStatus, (3 << 11)));

    *powerRxStatus = (GT_BOOL)(!(serdesPowerStatus >> 11));
    *powerTxStatus = (GT_BOOL)(!(serdesPowerStatus >> 12));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesFixAlign90
*
* DESCRIPTION:
*       Start fix Align90 process on current SERDES.
*       Can be run after create port.
*       For Rev 3.0: SW based search of align90 according to DFE_F1, DFE_F2
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* RETURNS:
*       0  - on success
*       1  - on error
********************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesFixAlign90
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum
)
{
    GT_U32  adaptedAlign90, bestAlign90=0, align90SearchWindow=5;
    GT_U32  startR, endR, adaptedOffset, adaptedR, adaptedC, bestR=0;
    GT_U32  dfeRes, dfeF0, dfeF1, dfeF2, dfeSatVal, dfeSum=63;
    GT_U32  F0d=0, maxF0d=0, F0dThr=0, sumF0dThr=4, maxSumF0d=0, sumF0d;
    GT_U32  i, j, data;
    GT_U32  ffeR, ffeC;
    GT_BOOL algoBaseDFE = GT_FALSE;

    if (loopbackMode == GT_TRUE)
    {
        /* Serdes PRBS testing is done without connecting to opposite serdes, but in loopback */
        CHECK_STATUS(mvHwsComH28nmSerdesTestGen(devNum, portGroup, serdesNum, PRBS31, SERDES_TEST));

        /* Delay in 1ms */
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &data, 0xFFFF));

    /* if the baudRate is below 10.3125G the dfe_pat_dis must be disabled */
    if (IS_SERDES_SPEED_BELOW_10G(data))
    {
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_2, 1 << 5, 1 << 5));
    }

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Data_Phase_Offset_Reg, &adaptedAlign90, 0xFE00));
    /* Get the Adapted Offset Phase value */
    adaptedAlign90 = adaptedAlign90 >> 9;

    /* define bestAlign90 */
    bestAlign90 = adaptedAlign90;

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Saved_DFE_Values_Reg0, &data, 0));

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_FFE_Reg, &adaptedR, 0x7000));
    /* Get the adaptedR value */
    adaptedR = adaptedR >> 12;

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_FFE_Reg, &adaptedC, 0xF00));
    /* Get the adaptedC value */
    adaptedC = adaptedC >> 8;

    adaptedR = (adaptedR < 2) ? 2 : adaptedR;

    startR = (adaptedR > 2) ? adaptedR-1 : 2;
    endR   = (adaptedR < 7) ? adaptedR+1 : 7;

    /* Get the DfeSat value according to the current temperature of Server */
    CHECK_STATUS(mvHwsComH28nmRev3SerdesDfeSatGet(devNum, &dfeSatVal));

    /* Verify if DFE is not saturated */
    for(dfeRes=0; dfeRes < 4; dfeRes++)
    {
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_G1_SET_4_Reg, dfeRes<<8, 0x300));

        /* Delay in 1msec */
        hwsOsExactDelayPtr(devNum, portGroup, 1);

        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Adapt_DFE_Reg0, &dfeF0, 0xFC00));
        /* Get the DFE F0 value */
        dfeF0 = dfeF0 >> 10;

        /* If the value is lower that Saturation limit, then stop */
        if((dfeF0 < dfeSatVal) || (dfeRes == 3))
        {
            break;
        }
    }

    /* at this point DFE is supposed to be not saturated */
    for (j=startR; j <= endR; j++)
    {
        sumF0d=0;

        /* Force FFE_R values, leave FFE_C as adapted */
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_G1_SET_3_Reg, 0x80+(j<<4) + adaptedC, 0xFF));

        /* Loop and search for DFE_F1+DFE_F2 minimum */
        for (i=adaptedAlign90-align90SearchWindow; i < adaptedAlign90+align90SearchWindow; i++)
        {
            /* set os_ph[5:0] */
            CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_OFFSET_Phase_Reg, 0x80+i, 0xFF));

            /* force os_ph */
            CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_OFFSET_Phase_Reg, 0x100, 0x100));

            /* Delay in 1ms */
            hwsOsExactDelayPtr(devNum, portGroup, 1);

            /* update os_ph */
            CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_OFFSET_Phase_Reg, 0, 0x100));

            if (algoBaseDFE == GT_TRUE)
            {
                /* Get the DFE_F1 without sign bit */
                CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Adapt_DFE_Reg0, &dfeF1, 0x1F0));
                dfeF1 = dfeF1 >> 4;

                /* Get the DFE_F2 without sign bit */
                CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Adapt_DFE_Reg0, &dfeF2, 0xF00));
                dfeF2 = dfeF2 >> 8;

                if((dfeF2 + dfeF1) < dfeSum)
                {
                    dfeSum = dfeF2+dfeF1;
                    bestR = j;
                    bestAlign90 = i;
                }
            }
            else    /* for algoBaseDFE = GT_FALSE */
            {
                sumF0d = sumF0d + F0d;
                CHECK_STATUS(mvHwsComH28nmRev3SerdesF0dGet(devNum, portGroup, serdesNum, &F0d));

                if(F0d > (maxF0d+F0dThr))
                {
                    maxF0d = F0d;
                    bestAlign90 = i;
                }
            }
        }

        if (algoBaseDFE == GT_FALSE)
        {
            /* check if sumF0d for each j is above maximum */
            if (sumF0d > (maxSumF0d+sumF0dThr))
            {
                maxSumF0d = sumF0d;
                bestR = j;
            }
        }
    }

    /* Configure the best value */

    /* Set os_ph[5:0] */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_OFFSET_Phase_Reg, 0x80+bestAlign90, 0xFF));

    /* update os_ph */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_OFFSET_Phase_Reg, 0x100, 0x100));

    /* Delay in 1msec */
    hwsOsExactDelayPtr(devNum, portGroup, 1);

    /* update os_ph */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_OFFSET_Phase_Reg, 0, 0x100));

    /* Force Best FFE_R values, leave FFE_C as adapted*/
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_G1_SET_3_Reg, 0x80+(bestR<<4)+adaptedC, 0xFF));

    if (loopbackMode == GT_TRUE)
    {
        /* Serdes PRBS testing is done without connecting to opposite serdes, but in loopback */
        CHECK_STATUS(mvHwsComH28nmSerdesTestGen(devNum, portGroup, serdesNum, PRBS31, SERDES_NORMAL));
    }

    /* set the optimal value of Adapted Offset Phase value into SD_METAL_FIX register */
    if ((HWS_DEV_IS_BOBCAT2_B0(devNum)) || (HWS_DEV_SILICON_TYPE(devNum) >= Alleycat3A0))
    {
        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Data_Phase_Offset_Reg, &data, 0xFE00));
        /* Get the Adapted Offset Phase value */
        adaptedOffset = data >> 9;

        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_FFE_Reg, &data, 0xFFFF));

        ffeR = (data >> 12) & 0x7;
        ffeC = (data >> 8) & 0xF;
        data = 0x100 | adaptedOffset | (ffeR<<9) | (ffeC<<12);
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SD_METAL_FIX, data, 0xFFFF));

        /* get the F0d value of DFE register */
        CHECK_STATUS(mvHwsComH28nmRev3SerdesF0dGet(devNum, portGroup, serdesNum, &data));

        /* set the F0d value of DFE register */
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_RESERVED_REGISTER_2, data, 0x7F));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesFixAlign90Start
*
* DESCRIPTION:
*       Start fix Align90 process on current SERDES.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       params    - pointer to structure to store SERDES configuration parameters that must be restore
*                   during process stop operation
*
* OUTPUTS:
*       params    - SERDES configuration parameters that must be restore
*                   during process stop operation
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesFixAlign90Start
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_ALIGN90_PARAMS   *params
)
{
    /* not needed in Rev 3.0 */

    /* avoid warnings */
	devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    (void)params;

    return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesFixAlign90Status
*
* DESCRIPTION:
*       Return fix Align90 process current status on current SERDES.
*       Can be run after create port and start Align90.
*		Note: this functionality is not needed in Rev 3.0 !!!
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       trainingPass - true, if process finished and passed;
*                    - false, otherwise (doesn't finished or passed)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesFixAlign90Status
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *trainingStatus
)
{
	/* not needed in Rev 3.0 */

    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    (void)trainingStatus;

	return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesFixAlign90Stop
*
* DESCRIPTION:
*       Stop fix Align90 process on current SERDES.
*       Can be run after create port and start Align90.
*		Note: this functionality is not needed in Rev 3.0 !!!
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       params    - SERDES parameters that must be restored (return by mvHwsComHRev2SerdesFixAlign90Start)
*       fixAlignPass - true, if fix Align90 process passed; false otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesFixAlign90Stop
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 serdesNum,
    MV_HWS_ALIGN90_PARAMS *params,
    GT_BOOL fixAlignPass
)
{
    /* not needed in Rev 3.0 */

    /* avoid warnings */
	devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    fixAlignPass = fixAlignPass;
    (void)params;

	return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesAutoTuneStatus
*
* DESCRIPTION:
*       Per SERDES check the Rx & Tx training status
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       rxTraining - Rx Training (true/false)
*       txTraining - Tx Training (true/false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesAutoTuneStatus
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    MV_HWS_AUTO_TUNE_STATUS     *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS     *txStatus
)
{
    CHECK_STATUS(mvHwsComH28nmSerdesAutoTuneStatus(devNum, portGroup, serdesNum, rxStatus, txStatus));

    return GT_OK;
}

/******************************************************************************
* mvHwsComH28nmRev3SerdesF0dGet
*
* DESCRIPTION:  Returns the F0d value of DFE register
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:  F0d value
*
* RETURNS:  GT_OK   - on success
*           GT_FAIL - on error
******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesF0dGet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_U32      *F0d
)
{
    GT_UREG_DATA data;

    /* update only F0 */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_DFE_Update_Reg, 0x40, 0xFC0));

    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_FFE_REG1, 0x2500, 0xFFFF));

    /* Delay in 1ms */
    hwsOsExactDelayPtr(devNum, portGroup, 1);

    /* Get the F0d value */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Adapt_DFE_Reg0, &data, 0xFC00));

    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_FFE_REG1, 0x100, 0xFFFF));

    /* update all coefficients */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_DFE_Update_Reg, 0xFC0, 0xFC0));

    /* Return the F0d value */
    *F0d = data >> 10;

    return GT_OK;
}

static GT_STATUS mvHwsComH28nmRev3SerdesDfeSet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_U32      align90Cal,
    GT_U32      midPoint,
    GT_U32      ffeR,
    GT_U32      *arryR
)
{
    GT_U32 startAddr;

    /* force shifted align90 calibration value */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_RxClockAl90_Reg, 0x8000+align90Cal, 0xFF00));

    /* mid point decision according to interconnect */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Cal_Reg_23, midPoint, 0xFFFF));

    /* Change the default FFE_R */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_RX_Train_Reg13, ffeR, 0xFFFF));

    for(startAddr=0; startAddr < 8; startAddr++)
    {
        /* Change the FFE table to include combination */
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Rx_Train_Reg4 + (4 * startAddr),
                                            arryR[startAddr], 0xFFFF));
    }

    return GT_OK;
}

/******************************************************************************
* mvHwsComH28nmRev3SerdesDfeScale
*
* DESCRIPTION: DFE Resolution SW based search with align90 mid point decision
*              according to interconnect
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - physical serdes number
*
* OUTPUTS:  None
*
* RETURNS:  GT_OK   - on success
*           GT_FAIL - on error
******************************************************************************/
static GT_STATUS mvHwsComH28nmRev3SerdesDfeScale
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         serdesNum,
    GT_UREG_DATA    *ffeR
)
{
    GT_U32  data, dfeF0, dfeRes, dfeSatVal;
    GT_U32  align90Offset0=84, align90Offset1=87, align90Offset23=92;
    GT_U32  midPoint0, midPoint1, midPoint23;
    GT_U32  ffeR2, ffeR3, ffeR4;

    GT_U32 arryLR[8] = { 0x2F2E, 0x3F3E, 0x4F4E, 0x2D2C, 0x3D3C, 0x4D4C, 0x2B2A, 0x3B3A };
    GT_U32 arryMR[8] = { 0x3F3E, 0x4F4E, 0x5F5E, 0x3D3C, 0x4D4C, 0x5D5C, 0x4B4A, 0x5B5A };
    GT_U32 arrySR[8] = { 0x4F4E, 0x5F5E, 0x6F6E, 0x4D4C, 0x5D5C, 0x6D6C, 0x5B5A, 0x6B6A };

    (void)ffeR;

    /* Get the serdes speed */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &data, 0xFFFF));

    /* if Serdes speed is 12.5G midpoint need to be shifted */
    midPoint0  = (data == 0x1FFA) ? 0x423d : 0x423f;
    midPoint1  = (data == 0x1FFA) ? 0x633d : 0x633f;
    midPoint23 = (data == 0x1FFA) ? 0x213d : 0x213f;

    /* if Serdes speed is 12.5G default R need to be updated */
    ffeR2  = (data == 0x1FFA) ? 0x012F : 0x012F;
    ffeR3  = (data == 0x1FFA) ? 0x014F : 0x013F;
    ffeR4  = (data == 0x1FFA) ? 0x015F : 0x014F;

    /* if the baudRate is below 10.3125G the dfe_pat_dis must be disabled */
    if (IS_SERDES_SPEED_BELOW_10G(data))
    {
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_2, 1 << 5, 1 << 5));
    }

    /* Get the DfeSat value according to the current temperature of DFX server */
    CHECK_STATUS(mvHwsComH28nmRev3SerdesDfeSatGet(devNum, &dfeSatVal));

    /* un-force align90 calibration in order to read real calibration value */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_RxClockAl90_Reg, 0, 0x8000));

    if(loopbackMode == GT_TRUE)
    {
        /* Serdes PRBS testing is done without connecting to opposite serdes, but in loopback */
        CHECK_STATUS(mvHwsComH28nmSerdesTestGen(devNum, portGroup, serdesNum, PRBS31, SERDES_TEST));

        /* Delay in 1ms */
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }

    /* Run for all resolutions */
    for(dfeRes=0; dfeRes < 4; dfeRes++)
    {
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_G1_SET_4_Reg, dfeRes<<8, 0x300));

        /* Delay in 2ms */
        hwsOsExactDelayPtr(devNum, portGroup, 2);

        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Adapt_DFE_Reg0, &data, 0xFC00));
        /* Get the DFE F0 value */
        dfeF0 = data >> 10;

        /* If the value is lower that Saturation limit, then stop */
        if((dfeF0 < dfeSatVal) || (dfeRes == 3))
        {
            break;
        }
    }

    switch (dfeRes)
    {
    case 0:
        /* force shifted align90 calibration value */
        align90Offset0 = align90Offset0 << 8;
        CHECK_STATUS(mvHwsComH28nmRev3SerdesDfeSet(devNum, portGroup, serdesNum, align90Offset0, midPoint0, ffeR2, arryLR));
        break;
    case 1:
        /* force shifted align90 calibration value */
        align90Offset1 = align90Offset1 << 8;
        CHECK_STATUS(mvHwsComH28nmRev3SerdesDfeSet(devNum, portGroup, serdesNum, align90Offset1, midPoint1, ffeR3, arryMR));
        break;
    case 2:
    case 3:
        /* force shifted align90 calibration value */
        align90Offset23 = align90Offset23 << 8;
        CHECK_STATUS(mvHwsComH28nmRev3SerdesDfeSet(devNum, portGroup, serdesNum, align90Offset23, midPoint23, ffeR4, arrySR));
        break;
    default:
        osPrintf("mvHwsComH28nmRev3SerdesDfeScale: Invalid dfeRes value %d !!\n", dfeRes);
        return GT_BAD_VALUE;
    }

    if(loopbackMode == GT_TRUE)
    {
        /* Serdes PRBS testing is done without connecting to opposite serdes, but in loopback */
        CHECK_STATUS(mvHwsComH28nmSerdesTestGen(devNum, portGroup, serdesNum, PRBS31, SERDES_NORMAL));
    }

   return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmRev3SerdesSignalDetectGet
*
* DESCRIPTION:
*       Return Signal Detect state on PCS (true/false).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       signalDet - if true, signal was detected
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesSignalDetectGet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    GT_BOOL     *signalDet
)
{
    GT_U32 data;
    GT_U32 i;

    for(i=0; i < 10; i++)
    {
        /* Read PLL lock status bit 0x18[1] */
        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_STATUS_0, &data, (1 << 1)));
        if (data == 1)
        {
            *signalDet = GT_FALSE; /* PLL is not in lock */
            return GT_OK;
        }
        /* delay in 1ms */
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }

    /* PLL is in lock */
    *signalDet = GT_TRUE;

    return GT_OK;
}

/******************************************************************************
* mvHwsComH28nmRev3FindBetterAdaptation
*
* DESCRIPTION: Find a better Align90 adaptation value when temperature on
*              device is changed from cold->hot or from hot->cold.
*              Loops on certain FFE_R and Align90 values and calls to
*              mvHwsComH28nmRev3SerdesF0dGet to evaluate the improvment.
*              This procedure is relevant only when:
*               - Serdes speeds is 10.3125G and above
*               - signal detect is 0
*               - CDR is locked
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - physical serdes number
*       threshDb    - threshold in db
*       avgFactor   - number of iterations for getting the average value of F0d
*
* OUTPUTS:  *donePtr - indicate if Better Adaptation algorithm run on
*                      Serdes or not
*
* RETURNS:  GT_OK   - on success
*           GT_FAIL - on error
*           GT_BAD_STATE - if Serdes is in Power Down
*           GT_NOT_SUPPORTED - if Serdes speed is wrong
*           GT_NOT_READY - if SQ_detect is not 0 and CDR is not locked
******************************************************************************/
GT_STATUS mvHwsComH28nmRev3FindBetterAdaptation
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_U32      threshDb,
    GT_U32      avgFactor,
    GT_BOOL     *donePtr
)
{
    GT_U32  bestAdaptedOffset, adaptedOffset;
    GT_U32  bestffeR, lowerffeR, upperffeR, lowerOsPh, upperOsPh;
    GT_U32  i, j, sdMetalFixVal;
    GT_U32  ffeR, ffeC, maxF0d=0, F0d=0, sumF0d=0;
    GT_U32  data, cdrLock, sqDetect;
    GT_BOOL smaller = GT_FALSE;
    float   voltDiv, tmpThreshDb;

    /* 20log table */
    static double dbValue[] =
    {/*  dbValue       voltDiv */
     /*------------------------*/
        -1.99266,   /*  0.795  */
        -1.9382,    /*  0.8    */
        -1.88408,   /*  0.805  */
        -1.83029,   /*  0.81   */
        -1.77684,   /*  0.815  */
        -1.72372,   /*  0.82   */
        -1.67092,   /*  0.825  */
        -1.61843,   /*  0.83   */
        -1.56627,   /*  0.835  */
        -1.51441,   /*  0.84   */
        -1.46286,   /*  0.845  */
        -1.41162,   /*  0.85   */
        -1.36067,   /*  0.855  */
        -1.31003,   /*  0.86   */
        -1.25967,   /*  0.865  */
        -1.20961,   /*  0.87   */
        -1.15983,   /*  0.875  */
        -1.11034,   /*  0.88   */
        -1.06113,   /*  0.885  */
        -1.01219,   /*  0.89   */
        -0.96353,   /*  0.895  */
        -0.91514,   /*  0.9    */
        -0.86702,   /*  0.905  */
        -0.81917,   /*  0.91   */
        -0.77157,   /*  0.915  */
        -0.72424,   /*  0.92   */
        -0.67716,   /*  0.925  */
        -0.63034,   /*  0.93   */
        -0.58376,   /*  0.935  */
        -0.53744,   /*  0.94   */
        -0.49136,   /*  0.945  */
        -0.44552,   /*  0.95   */
        -0.39993,   /*  0.955  */
        -0.35457,   /*  0.96   */
        -0.30945,   /*  0.965  */
        -0.26456,   /*  0.97   */
        -0.21990,   /*  0.975  */
        -0.17547,   /*  0.98   */
        -0.13127,   /*  0.985  */
        -0.08729,   /*  0.99   */
        -0.04353,   /*  0.995  */
        0,          /*  1      */
        0.04332,    /*  1.005  */
        0.08642,    /*  1.01   */
        0.12932,    /*  1.015  */
        0.17200,    /*  1.02   */
        0.21447,    /*  1.025  */
        0.25674,    /*  1.03   */
        0.29880,    /*  1.035  */
        0.34066,    /*  1.04   */
        0.38232,    /*  1.045  */
        0.42378,    /*  1.05   */
        0.46504,    /*  1.055  */
        0.50611,    /*  1.06   */
        0.54699,    /*  1.065  */
        0.58767,    /*  1.07   */
        0.62816,    /*  1.075  */
        0.66847,    /*  1.08   */
        0.70859,    /*  1.085  */
        0.74852,    /*  1.09   */
        0.78828,    /*  1.095  */
        0.82785,    /*  1.1    */
        0.86724,    /*  1.105  */
        0.90645,    /*  1.11   */
        0.94549,    /*  1.115  */
        0.98436,    /*  1.12   */
        1.02305,    /*  1.125  */
        1.06156,    /*  1.13   */
        1.09991,    /*  1.135  */
        1.13809,    /*  1.14   */
        1.1761,     /*  1.145  */
        1.21395,    /*  1.15   */
        1.25163,    /*  1.155  */
        1.28915,    /*  1.16   */
        1.32651,    /*  1.165  */
        1.36371,    /*  1.17   */
        1.40075,    /*  1.175  */
        1.43764,    /*  1.18   */
        1.47436,    /*  1.185  */
        1.51093,    /*  1.19   */
        1.54735,    /*  1.195  */
        1.58362 };  /*  1.2    */

    if (avgFactor == 0)
    {
        osPrintf("mvHwsComH28nmRev3FindBetterAdaptation: avgFactor value should be bigger than 0\n");
        return GT_BAD_PARAM;
    }

    *donePtr = GT_FALSE;

    /* recall saved adapted values in per-serdes macro user defined register */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SD_METAL_FIX, &sdMetalFixVal, 0xFFFF));

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_RESERVED_REGISTER_2, &data, 0xFFFF));

    /* exit from function if Serdes is in Power Down: SD_METAL_FIX reg = 0x100, F0d value of DFE register is not saved */
    if ((sdMetalFixVal == 0x100) || (data == 0))
    {
        osPrintf("mvHwsComH28nmRev3FindBetterAdaptation: Serdes is in Power down, no need to run the better Align90 adaptation\n");
        return GT_BAD_STATE;
    }

    /* Read GEN_Rx/Tx */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &data, 0xFFFF));

    /* exit from function if Serdes speed is not 10.3125G (0x1F72), 11.25G (0x1FFA) and above */
    if (IS_SERDES_SPEED_BELOW_10G(data))
    {
        osPrintf("mvHwsComH28nmRev3FindBetterAdaptation: no need to run the better Align90 adaptation on current Serdes speed\n");
        return GT_NOT_SUPPORTED;
    }

    tmpThreshDb = (float)threshDb/1000;

    /* Get signal detect */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_Squelch_FFE_Reg, &sqDetect, 1<<14));
    sqDetect >>= 14;

    /* cdr_lock_det_en=1 */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_SEL_BITS_Reg, 0x100, 0x100));

    /* Get cdr_lock status */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_28NM_SEL_BITS_Reg, &cdrLock, 1<<7));
    cdrLock = cdrLock  >> 7;

    /* run the procedure only if SQ_detect=1 and CDR is not lock check */
    if ((cdrLock==0) || (sqDetect==1))
    {
        osPrintf("mvHwsComH28nmRev3FindBetterAdaptation: remove from function (cdrLock==0) and (sqDetect==1)\n");
        return GT_NOT_READY;
    }

    adaptedOffset = sdMetalFixVal & 0x7F;
    ffeR = (sdMetalFixVal >> 9) & 0x7;
    ffeC = (sdMetalFixVal >> 12) & 0xF;

    /* get average F0d value */
    for (i = 0; i < avgFactor; i++)
    {
        CHECK_STATUS(mvHwsComH28nmRev3SerdesF0dGet(devNum, portGroup, serdesNum, &F0d));
        sumF0d = sumF0d + F0d;
    }
    F0d = sumF0d / avgFactor;

    /* recall the initial value of adaptedOffset saved immediately after training */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_RESERVED_REGISTER_2, &data, 0x7F));

    /* Convert the (F0d / adaptedOffset) value from Volt to db.
       This conversion is equal to: dbValue = 20*log10f(F0d / data) */
    voltDiv = (float)0.795;

    for (i=0; i<81; i++)
    {
        if (voltDiv > (((float)F0d / (float)data) + 0.0001))
        {
            break;
        }
        voltDiv = (float)(voltDiv + 0.005);  /* This is an approximation (+/-0.005) of the F0d/data value */
    }
    /* for i==0 */
    i = (i==0) ? 1 : i;

    if (dbValue[i-1] < -tmpThreshDb-0.0001)
    {   /* eye become smaller and above threshDb usually from cold->hot */
        smaller = GT_TRUE;
    }
    else if (dbValue[i-1] > tmpThreshDb+0.0001)
    {   /* eye become bigger and above threshDb usually from hot->cold */
        smaller = GT_FALSE;
    }
    else
    {   /* if the dbValue is between threshDb value (usually +/- 0.5db),
           the value is good and there is no need to find a better adaptation */
        return GT_OK;
    }

    /* in this stage the Better Adaptation algorithm run on Serdes */
    *donePtr = GT_TRUE;

    switch (ffeR)
    {
    case 2:
        lowerffeR = (smaller) ? 1 : 2;
        upperffeR = (smaller) ? 2 : 3;
        break;
    case 3:
    case 4:
        lowerffeR = (smaller) ? 2 : 3;
        upperffeR = (smaller) ? 3 : 4;
        break;
    case 5:
        lowerffeR = (smaller) ? 4 : 5;
        upperffeR = (smaller) ? 5 : 6;
        break;
    case 6:
        lowerffeR = (smaller) ? 5 : 6;
        upperffeR = (smaller) ? 6 : 6;
        break;
    default:
        lowerffeR=ffeR;
        upperffeR=ffeR;
        break;
    }

    bestAdaptedOffset = adaptedOffset;
    bestffeR = ffeR;

    lowerOsPh = adaptedOffset-1;
    upperOsPh = adaptedOffset+3;

    for (i=lowerffeR; i <= upperffeR; i++)
    {
        for (j=lowerOsPh; j < upperOsPh; j++)
        {
            /* Set fource Align90 value */
            CHECK_STATUS(mvHwsComH28nmSerdesForceAlign90Set(devNum, portGroup, serdesNum, j));

            /* force FFE values */
            CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV3_G1_SET_3_Reg, ((1<<7)+(i<<4)+ffeC), 0xFF));

            /* Delay in 1ms */
            hwsOsExactDelayPtr(devNum, portGroup, 1);

            /* get updated Eye opening and check if it is improved */
            CHECK_STATUS(mvHwsComH28nmRev3SerdesF0dGet(devNum, portGroup, serdesNum, &F0d));

            if (F0d >= maxF0d)
            {
                maxF0d = F0d;
                bestAdaptedOffset = j;
                bestffeR = i;
            }
            else
            {
                i = lowerffeR + 1;
                j = upperOsPh;
                break;
            }
        }
    }

    /* force FFE values */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, INTERNAL_REG, serdesNum, COM_H_REV3_G1_SET_3_Reg, ((1<<7)+(bestffeR<<4)+ffeC), 0xFF));

    /* Set force Align90 value */
    CHECK_STATUS(mvHwsComH28nmSerdesForceAlign90Set(devNum, portGroup, serdesNum, bestAdaptedOffset));

    /* Get the F0d value of DFE register */
    CHECK_STATUS(mvHwsComH28nmRev3SerdesF0dGet(devNum, portGroup, serdesNum, &F0d));

    /* Save the F0d value of DFE register */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_RESERVED_REGISTER_2, F0d, 0x7F));

    /* Save the bestAdaptedOffset value in SD_METAL_FIX register */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SD_METAL_FIX, bestAdaptedOffset, 0x7F));

	return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsComH28nmIfClose
*
* DESCRIPTION:
*       Release all system resources allocated by Serdes IF functions.
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
void mvHwsComH28nmRev3IfClose(void)
{
    hwsInitDone = GT_FALSE;
}

#endif

