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
* mvAvagoSerdesIf.c.c
*
* DESCRIPTION:
*         Avago interface
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/avago/mvAvagoIf.h>


#ifdef _VISUALC
#pragma warning(disable : 4100)
#endif

#ifndef ASIC_SIMULATION
/* Avago include */
#include <aapl/marvell/mv_hws_avago_if.h>
#include <aapl.h>
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
#ifdef FW_DOWNLOAD_FROM_SERVER
#include <aapl/marvell/sd28firmware/avago_fw_load.h>
#endif
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
#endif /* ASIC_SIMULATION */

#ifndef CPSS_BLOB
#define TUNE_EXEC_TIME
#endif
#if defined(TUNE_EXEC_TIME) && (!defined WIN32)
#include <sys/time.h>
#endif

/************************* definition *****************************************************/
#define AVAGO_AAPL_LGPL
/*#define MARVELL_AVAGO_DEBUG_FLAG*/

#ifdef MARVELL_AVAGO_DEBUG_FLAG
#define AVAGO_DBG(s) osPrintf s
#else
#define AVAGO_DBG(s)
#endif /* MARVELL_AVAGO_DEBUG_FLAG */

#define MV_AVAGO_TRAINING_TIMEOUT       6000
#define MV_AVAGO_MIN_EYE_SIZE           10
#define MV_AVAGO_EYE_TAP                4

#define ICAL_TRAINING_MODE              0
#define PCAL_TRAINING_MODE              1

#define SKIP_SERDES                     0xFFFF

#define MV_AVAGO_TX_EQUALIZE_PASS       (0x54)

/************************* Globals *******************************************************/
#ifndef ASIC_SIMULATION
GT_U8 avagoBaudRate2DividerValue[LAST_MV_HWS_SERDES_SPEED]= {NA_VALUE,8,20,NA_VALUE,24,NA_VALUE,32,40,48,66,72,NA_VALUE,80,70,78,NA_VALUE};
GT_U8 avagoBaudRate2DwellTimeValue[LAST_MV_HWS_SERDES_SPEED]= {NA_VALUE,8,8,NA_VALUE,8,NA_VALUE,8,9,10,11,11,NA_VALUE,12,12,12,NA_VALUE};

extern Aapl_t* aaplSerdesDb[HWS_MAX_DEVICE_NUM];
extern GT_STATUS mvCfgSeqRunPolOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           index,
    MV_CFG_ELEMENT  *element
);
#endif /* ASIC_SIMULATION */

/************************* Declaration ***************************************************/
#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsAvagoSerdesTypeGetFunc
*
* DESCRIPTION:
*       Return "AVAGO" string
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Return "AVAGO" string
*
*******************************************************************************/
static char* mvHwsAvagoSerdesTypeGetFunc(void)
{
  return "AVAGO";
}
#endif /* CO_CPU_RUN */

/*******************************************************************************
* mvHwsAvagoIfInit
*
* DESCRIPTION:
*       Init Avago Serdes IF functions.
*
* INPUTS:
*       devNum       - system device number
*       funcPtrArray - array for function registration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoIfInit
(
    GT_U8  devNum,
    MV_HWS_SERDES_FUNC_PTRS *funcPtrArray
)
{
#ifdef ASIC_SIMULATION
    /* avoid warning */
    devNum = devNum;
#else
    GT_STATUS res;

    res = mvHwsAvagoSerdesInit(devNum);
    if(res == GT_ALREADY_EXIST)
    {
        /* Avago Serdes for this device was already initialized */
        return GT_OK;
    }
    else if(res != GT_OK)
    {
        return res;
    }
#endif
    /* Avago implementation */
    funcPtrArray[AVAGO].serdesArrayPowerCntrlFunc    = NULL;
    funcPtrArray[AVAGO].serdesPowerCntrlFunc         = mvHwsAvagoSerdesPowerCtrl;
    funcPtrArray[AVAGO].serdesManualTxCfgFunc        = mvHwsAvagoSerdesManualTxConfig;
    funcPtrArray[AVAGO].serdesManualRxCfgFunc        = NULL;
    funcPtrArray[AVAGO].serdesAutoTuneCfgFunc        = NULL;
    funcPtrArray[AVAGO].serdesRxAutoTuneStartFunc    = mvHwsAvagoSerdesRxAutoTuneStart;
    funcPtrArray[AVAGO].serdesTxAutoTuneStartFunc    = mvHwsAvagoSerdesTxAutoTuneStart;
    funcPtrArray[AVAGO].serdesAutoTuneStartFunc      = mvHwsAvagoSerdesAutoTuneStart;
    funcPtrArray[AVAGO].serdesAutoTuneResultFunc     = mvHwsAvagoSerdesAutoTuneResult;
    funcPtrArray[AVAGO].serdesAutoTuneStatusFunc     = mvHwsAvagoSerdesAutoTuneStatus;
    funcPtrArray[AVAGO].serdesAutoTuneStatusShortFunc = mvHwsAvagoSerdesAutoTuneStatusShort;
    funcPtrArray[AVAGO].serdesResetFunc              = mvHwsAvagoSerdesReset;
    funcPtrArray[AVAGO].serdesDigitalReset           = mvHwsAvagoSerdesDigitalReset;
    funcPtrArray[AVAGO].serdesCoreReset              = NULL;
#ifndef CO_CPU_RUN
    funcPtrArray[AVAGO].serdesSeqGetFunc             = NULL;
    funcPtrArray[AVAGO].serdesSeqSetFunc             = NULL;
    funcPtrArray[AVAGO].serdesDfeCfgFunc             = mvHwsAvagoSerdesDfeConfig;
    funcPtrArray[AVAGO].serdesLoopbackCfgFunc        = mvHwsAvagoSerdesLoopback;
    funcPtrArray[AVAGO].serdesLoopbackGetFunc        = mvHwsAvagoSerdesLoopbackGet;
    funcPtrArray[AVAGO].serdesPpmCfgFunc             = NULL;
    funcPtrArray[AVAGO].serdesPpmGetFunc             = NULL;
    funcPtrArray[AVAGO].serdesTestGenFunc            = mvHwsAvagoSerdesTestGen;
    funcPtrArray[AVAGO].serdesTestGenGetFunc         = mvHwsAvagoSerdesTestGenGet;
    funcPtrArray[AVAGO].serdesTestGenStatusFunc      = mvHwsAvagoSerdesTestGenStatus;
    funcPtrArray[AVAGO].serdesEomGetFunc             = NULL;
    funcPtrArray[AVAGO].serdesDfeStatusFunc          = NULL;
    funcPtrArray[AVAGO].serdesDfeStatusExtFunc       = NULL;
    funcPtrArray[AVAGO].serdesPolarityCfgFunc        = mvHwsAvagoSerdesPolarityConfig;
    funcPtrArray[AVAGO].serdesPolarityCfgGetFunc     = mvHwsAvagoSerdesPolarityConfigGet;
    funcPtrArray[AVAGO].serdesDfeOptiFunc            = NULL;
    funcPtrArray[AVAGO].serdesFfeCfgFunc             = NULL;
    funcPtrArray[AVAGO].serdesCalibrationStatusFunc  = NULL;
    funcPtrArray[AVAGO].serdesTxEnableFunc           = mvHwsAvagoSerdesTxEnable;
    funcPtrArray[AVAGO].serdesTxEnableGetFunc        = mvHwsAvagoSerdesTxEnableGet;
    funcPtrArray[AVAGO].serdesTxIfSelectFunc         = mvHwsAvagoSerdesTxIfSelect;
    funcPtrArray[AVAGO].serdesTxIfSelectGetFunc      = mvHwsAvagoSerdesTxIfSelectGet;
    funcPtrArray[AVAGO].serdesSqlchCfgFunc           = NULL;
    funcPtrArray[AVAGO].serdesAcTermCfgFunc          = NULL;
    funcPtrArray[AVAGO].serdesAutoTuneStopFunc       = mvHwsAvagoSerdesTxAutoTuneStop;
    funcPtrArray[AVAGO].serdesDfeCheckFunc           = NULL;
    funcPtrArray[AVAGO].serdesSpeedGetFunc           = NULL;
    funcPtrArray[AVAGO].serdesManualRxCfgGetFunc     = NULL;
    funcPtrArray[AVAGO].serdesManualTxCfgGetFunc     = mvHwsAvagoSerdesManualTxConfigGet;
    funcPtrArray[AVAGO].serdesSignalDetectGetFunc    = NULL;
    funcPtrArray[AVAGO].serdesCdrLockStatusGetFunc   = NULL;
    funcPtrArray[AVAGO].serdesScanSamplerFunc        = NULL;
    funcPtrArray[AVAGO].serdesFixAlign90Start        = NULL;
    funcPtrArray[AVAGO].serdesFixAlign90Stop         = NULL;
    funcPtrArray[AVAGO].serdesFixAlign90Status       = NULL;
    funcPtrArray[AVAGO].serdesTypeGetFunc            = mvHwsAvagoSerdesTypeGetFunc;
#endif /* CO_CPU_RUN */

    return GT_OK;
}

#ifdef FW_DOWNLOAD_FROM_SERVER
/*******************************************************************************
* mvHwsAvagoSetFwDownloadFileNames
*
* DESCRIPTION:
*       Set Avago FW Download files names
*                osPrintf("Chip %d (%s %s), ring %d, SBus devices: %d\n", chip, aapl->chip_name[chip], aapl->chip_rev[chip], ring, aapl->max_sbus_addr[chip][ring]);

* INPUTS:
*
* char *sertdesFwName
* char *sbusMasterFwName;
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS mvHwsAvagoSetFwDownloadFileNames
(
    char *serdesFwName,
    char *sbusMasterFwName
)
{
    serdesFileName = serdesFwName;
    sbusMasterFileName = sbusMasterFwName;
}
#endif /* FW_DOWNLOAD_FROM_SERVER */

/*******************************************************************************
* mvHwsAvagoSerdesPollingSet
*
* DESCRIPTION:
*       Set the Polling on Avago Serdes register address
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - number of SERDESes to configure
*       regAddr    - Serdes register address (offset) to access
*       mask       - register read mask
*       numOfLoops - polling on spico ready
*       delay      - delay in ms after register get value
*       expectedValue - value for stopping the polling on register
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesPollingSet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_U32      regAddr,
    GT_U32      mask,
    GT_U32      numOfLoops,
    GT_U32      delay,
    GT_U32      expectedValue
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    regAddr = regAddr;
    mask = mask;
    numOfLoops = numOfLoops;
    delay = delay;
    expectedValue = expectedValue;
#else
    MV_CFG_ELEMENT          element;
    MV_POLLING_OP_PARAMS    pollParams;

    pollParams.indexOffset = SERDES_UNIT;
    pollParams.regOffset = regAddr;
    pollParams.mask = mask;
    pollParams.numOfLoops = numOfLoops;
    pollParams.waitTime = delay;
    pollParams.data = expectedValue;

    element.params = &pollParams;

    mvCfgSeqRunPolOp(devNum, portGroup, serdesNum, &element);
#endif /* ASIC_SIMULATION */
    return GT_OK;
}

#ifndef ASIC_SIMULATION
/*******************************************************************************
* mvHwsAvagoSerdesRxAutoTuneStatusShort
*
* DESCRIPTION:
*       Per SERDES check the Rx training status
*       This function is necessary for 802.3ap functionality
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       rxStatus - status of Rx-Training
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS mvHwsAvagoSerdesRxAutoTuneStatusShort
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus
)
{
    GT_U32 data;

    *rxStatus = TUNE_NOT_COMPLITED;

    /* get DFE status */
    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x126, ((0 << 12) | (0xB << 8)), &data));
    data &= 0xBF; /* check only bits #0-5 and #7 */
    *rxStatus = (data == 0x80) ? TUNE_PASS : TUNE_NOT_COMPLITED;

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTxAutoTuneStatusShort
*
* DESCRIPTION:
*       Per SERDES check the Tx training status
*       This function is necessary for 802.3ap functionality
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       rxStatus - status of Tx-Training
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS mvHwsAvagoSerdesTxAutoTuneStatusShort
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
    GT_U32 data;
    GT_U32 sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    *txStatus = TUNE_NOT_COMPLITED;

    /* check PMD training status */
    data = avago_serdes_pmd_status(aaplSerdesDb[devNum], sbus_addr);
    if (data == 0)
        *txStatus = TUNE_FAIL;
    else if (data == 0x1)
        *txStatus = TUNE_PASS;
    else if (data == 0x2)
        *txStatus = TUNE_NOT_COMPLITED;

    return GT_OK;
}
#endif /* ASIC_SIMULATION */

/*******************************************************************************
* mvHwsAvagoSerdesAutoTuneStatusShort
*
* DESCRIPTION:
*       Check the Serdes Rx or Tx training status
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       rxStatus - Rx-Training status
*       txStatus - Tx-Training status
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesAutoTuneStatusShort
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    if(rxStatus)
    {
        *rxStatus = TUNE_PASS;
    }
    if(txStatus)
    {
        *txStatus = TUNE_PASS;
    }
#else
    if ((NULL == rxStatus) && (NULL == txStatus))
    {
        return GT_BAD_PARAM;
    }

    if (rxStatus != NULL)
    {
        CHECK_STATUS(mvHwsAvagoSerdesRxAutoTuneStatusShort(devNum, portGroup, serdesNum, rxStatus));
    }

    if (txStatus != NULL)
    {
        CHECK_STATUS(mvHwsAvagoSerdesTxAutoTuneStatusShort(devNum, portGroup, serdesNum, txStatus));
    }
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesPowerCtrl
*
* DESCRIPTION:
*       Power up SERDES list.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - number of SERDESes to configure
*       powerUp   - True for PowerUP, False for PowerDown
*       baudRate  - Serdes speed
*       refClock  - ref clock value
*       refClockSource - ref clock source (primary line or secondary)
*       media     - RXAUI or XAUI
*       mode      - Serdes bus modes: 10Bits/20Bits/40Bits
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesPowerCtrl
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 serdesNum,
    GT_BOOL                 powerUp,
    MV_HWS_SERDES_SPEED     baudRate,
    MV_HWS_REF_CLOCK        refClock,
    MV_HWS_REF_CLOCK_SRC    refClockSource,
    MV_HWS_SERDES_MEDIA     media,
    MV_HWS_SERDES_10B_MODE  mode
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    powerUp = powerUp;
    baudRate = baudRate;
    refClock = refClock;
    refClockSource = refClockSource;
    media = media;
    mode = mode;
#else
    GT_U32 divider, dwell;

   /* define deivider value */
    if (avagoBaudRate2DividerValue[baudRate] != NA_VALUE){
        divider = avagoBaudRate2DividerValue[baudRate];
    }
    else
        divider = NA_VALUE;

    /* define dwell value */
    if (avagoBaudRate2DwellTimeValue[baudRate] != NA_VALUE){
        dwell = avagoBaudRate2DwellTimeValue[baudRate];
    }
    else
        dwell = 11;

    CHECK_STATUS(mvHwsAvagoSerdesPowerCtrlImpl(devNum, portGroup, serdesNum, powerUp, divider, refClock, refClockSource, media, mode));

    /* select DFE tuning dwell time */
    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x18, 0x7, NULL));

    /* big register write LSB */
    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x19, 1<<dwell, NULL));

    /* big register write MSB */
    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x19, 0, NULL));

    return GT_OK;
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesManualTxConfig
*
* DESCRIPTION:
*       Per SERDES configure the TX parameters: amplitude, 3 TAP Tx FIR.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       txAmp     - Tx amplitude
*       txAmpAdj  - not used in Avago serdes
*       emph0     - Post-cursor setting
*       emph1     - Pre-cursor setting
*       slewRate  - Slew rate setting [0..3], 0 is fastest
*                   (the SlewRate parameter is not supported in Avago Serdes)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesManualTxConfig
(
    GT_U8    devNum,
    GT_UOPT  portGroup,
    GT_UOPT  serdesNum,
    GT_UREG_DATA    txAmp,
    GT_BOOL         txAmpAdj,
    GT_UREG_DATA    emph0,
    GT_UREG_DATA    emph1,
    GT_UREG_DATA    slewRate
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    txAmp = txAmp;
    txAmpAdj = txAmpAdj;
    emph0 = emph0;
    emph1 = emph1;
    slewRate = slewRate;
#else

    Avago_serdes_tx_eq_t tx_eq;
    GT_U32 sbus_addr;
    GT_U32 rc1, rc2, rc3, rc4;

    /* txAmpAdj is not used in Avago serdes */
    txAmpAdj = GT_FALSE;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    /* Set the TX equalization values */
    tx_eq.atten = (short)txAmp;     /* Attenuator setting */
    tx_eq.post  = (short)emph0;     /* Post-cursor setting */
    tx_eq.pre   = (short)emph1;     /* Pre-cursor setting */
    tx_eq.slew  = (short)slewRate;  /* Slew rate setting */

    if((tx_eq.atten == tx_eq.pre) && (tx_eq.atten == tx_eq.post))
    {
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x15, (3 << 14) | (tx_eq.atten & 0xFF), &rc3));
    }
    else
    {
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x15, (3 << 14) | 0, &rc1));
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x15, (2 << 14) | (tx_eq.post & 0xFF), &rc2));
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x15, (1 << 14) | (tx_eq.atten & 0xFF), &rc3));
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x15, (0 << 14) | (tx_eq.pre & 0xFF), &rc4));

        /* If SpicoInterrupt 0x15 succeeds to run - the return code is the Interrupt number=0x15
           The TX equalization configuration succeeds only if all the four SpicoInterrupt settings are passed: (0x15)x4=0x54 */
        if ((rc1+rc2+rc3+rc4) != MV_AVAGO_TX_EQUALIZE_PASS)
        {
            osPrintf("SerDes failed to apply since new EQ settings exceed the limit %d %d %d %d\n", rc1, rc2, rc3, rc4);
            return GT_FAIL;
        }
    }

#if 0
    /* the SlewRate parameter is not supported in Avago Serdes */
    /* these settings are needed due to problem with SlewRate configuration in avago_serdes_set_tx_eq */
    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x18, 0x4240, NULL));
    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x19, (0x184 + slewRate), NULL));
#endif

#endif /* ASIC_SIMULATION */
    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesManualTxConfigGet
*
* DESCRIPTION:
*       Per SERDES get the configure TX parameters: amplitude, 3 TAP Tx FIR.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       configParams - Manual Tx params structures:
*           txAmp       Tx amplitude
*           txAmpAdj    not used in Avago serdes
*           emph0       TX emphasis 0
*           emph1       TX emphasis 1
*           txAmpShft   not used in Avago serdes
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesManualTxConfigGet
(
    GT_U8                           devNum,
    GT_UOPT                         portGroup,
    GT_UOPT                         serdesNum,
    MV_HWS_MAN_TUNE_TX_CONFIG_DATA  *configParams
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    hwsOsMemSetFuncPtr(configParams,0,sizeof(*configParams));
#else

    MV_HWS_AUTO_TUNE_RESULTS    results;

    /* txAmpAdj and txAmpShft are not used in Avago serdes */
    configParams->txAmpAdj  = GT_FALSE;
    configParams->txAmpShft = GT_FALSE;

    CHECK_STATUS(mvHwsAvagoSerdesAutoTuneResult(devNum, portGroup, serdesNum, &results));

    /* Get the TX configuration values */
    configParams->txAmp   = results.txAmp;
    configParams->txEmph0 = results.txEmph0;
    configParams->txEmph1 = results.txEmph1;
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesReset
*
* DESCRIPTION:
*       Per SERDES Clear the serdes registers (back to defaults.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - serdes number
*       analogReset - Analog Reset (On/Off)
*       digitalReset - digital Reset (On/Off)
*       syncEReset - SyncE Reset (On/Off)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesReset
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     analogReset,
    GT_BOOL     digitalReset,
    GT_BOOL     syncEReset
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    analogReset = analogReset;
    digitalReset = digitalReset;
    syncEReset = syncEReset;
#else

  return(mvHwsAvagoSerdesResetImpl(devNum, portGroup, serdesNum, analogReset, digitalReset, syncEReset));
#endif /* ASIC_SIMULATION */

  return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesDigitalReset
*
* DESCRIPTION:
*       Run digital reset / unreset on current SERDES.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - serdes number
*       digitalReset - digital Reset (On/Off)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesDigitalReset
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         serdesNum,
    MV_HWS_RESET    digitalReset
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    digitalReset = digitalReset;
#else

    GT_U32  data;

    /* SERDES SD RESET/UNRESET init */
    data = (digitalReset == UNRESET) ? 0 : 1;
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_1, (data << 2), (1 << 2)));
#endif /* ASIC_SIMULATION */

    return GT_OK;
}
/*******************************************************************************
* mvHwsAvagoSerdesPolarityConfig
*
* DESCRIPTION:
*       Per Serdes invert the Tx or Rx.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical Serdes number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesPolarityConfig
(
    GT_U8   devNum,
    GT_UOPT portGroup,
    GT_UOPT serdesNum,
    GT_BOOL invertTx,
    GT_BOOL invertRx
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    invertTx = invertTx;
    invertRx = invertRx;
#else

    return (mvHwsAvagoSerdesPolarityConfigImpl(devNum, portGroup, serdesNum, invertTx, invertRx));
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesPolarityConfigGet
*
* DESCRIPTION:
*       Returns the Tx and Rx SERDES invert state.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       invertTx - invert TX polarity (GT_TRUE - invert, GT_FALSE - don't)
*       invertRx - invert RX polarity (GT_TRUE - invert, GT_FALSE - don't)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesPolarityConfigGet
(
    GT_U8             devNum,
    GT_UOPT           portGroup,
    GT_UOPT           serdesNum,
    GT_BOOL           *invertTx,
    GT_BOOL           *invertRx
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    invertTx = invertTx;
    invertRx = invertRx;
#else

    unsigned int avagoInvertTx;
    unsigned int avagoInvertRx;
    int res;

    res = mvHwsAvagoSerdesPolarityConfigGetImpl(devNum, portGroup, serdesNum,
                                                &avagoInvertTx, &avagoInvertRx);
    if(res != GT_OK)
    {
        osPrintf("mvHwsAvagoSerdesPolarityConfigGetImpl failed (%d)\n", res);
        return GT_FAIL;
    }

    *invertTx = avagoInvertTx;
    *invertRx = avagoInvertRx;
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesDfeConfig
*
* DESCRIPTION:
*       Per SERDES configure the DFE parameters.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       dfeMode   - Enable/disable/freeze/Force
*       dfeCfg    - array of dfe configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesDfeConfig
(
    GT_U8               devNum,
    GT_UOPT             portGroup,
    GT_UOPT             serdesNum,
    MV_HWS_DFE_MODE     dfeMode,
    GT_REG_DATA         *dfeCfg
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    dfeMode = dfeMode;
    dfeCfg = dfeCfg;
#else

    Avago_serdes_dfe_tune_t  dfe_state;
    GT_U32 sbus_addr;

    /* dfeCfg is not used in Avago serdes */
    dfeCfg = NULL;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    /* Initialize the dfe_state struct with default values */
    avago_serdes_tune_init(aaplSerdesDb[devNum], &dfe_state);
    CHECK_AVAGO_RET_CODE();

    switch (dfeMode)
    {
        case DFE_STOP_ADAPTIVE:
            dfe_state.tune_mode = AVAGO_DFE_STOP_ADAPTIVE;
            break;
        case DFE_START_ADAPTIVE:
            dfe_state.tune_mode = AVAGO_DFE_START_ADAPTIVE;
            break;
        case DFE_ICAL:
            dfe_state.tune_mode = AVAGO_DFE_ICAL;
            break;
        case DFE_PCAL:
            dfe_state.tune_mode = AVAGO_DFE_PCAL;
            break;
        default:
            return GT_BAD_PARAM;
    }

    mvHwsAvagoAccessLock(devNum);
    /* Run/Halt DFE tuning on a serdes based on dfe_tune_mode */
    avago_serdes_tune(aaplSerdesDb[devNum], sbus_addr, &dfe_state);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTestGen
*
* DESCRIPTION:
*       Activates the Serdes test generator/checker.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical Serdes number
*       txPattern - pattern to transmit ("Other" means HW default - K28.5
*                                               [alternate running disparity])
*       mode      - test mode or normal
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesTestGen
(
    GT_U8                       devNum,
    GT_UOPT                     portGroup,
    GT_UOPT                     serdesNum,
    MV_HWS_SERDES_TX_PATTERN    txPattern,
    MV_HWS_SERDES_TEST_GEN_MODE mode
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    txPattern = txPattern;
    mode = mode;
#else

    GT_U32  sbus_addr;
    Avago_serdes_rx_cmp_data_t  rxPatternData = AVAGO_SERDES_RX_CMP_DATA_OFF;
    Avago_serdes_tx_data_sel_t  txPatternData = AVAGO_SERDES_TX_DATA_SEL_CORE;
    long tx_user[4] = {0};

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    if (mode == SERDES_TEST)
    {
        switch (txPattern)
        {
            case _1T:
                tx_user[0] = 0xAAAAA;
                tx_user[1] = 0xAAAAA;
                tx_user[2] = 0xAAAAA;
                tx_user[3] = 0xAAAAA;
                break;
            case _2T:
                tx_user[0] = 0xCCCCC;
                tx_user[1] = 0xCCCCC;
                tx_user[2] = 0xCCCCC;
                tx_user[3] = 0xCCCCC;
                break;
            case _5T:
                tx_user[0] = 0x7C1F;
                tx_user[1] = 0x7C1F;
                tx_user[2] = 0x7C1F;
                tx_user[3] = 0x7C1F;
                break;
            case _10T:
                tx_user[0] = 0x3FF;
                tx_user[1] = 0x3FF;
                tx_user[2] = 0x3FF;
                tx_user[3] = 0x3FF;
                break;
            case PRBS7:
                txPatternData = AVAGO_SERDES_TX_DATA_SEL_PRBS7;
                rxPatternData = AVAGO_SERDES_RX_CMP_DATA_PRBS7;
                break;
            case PRBS9:
                txPatternData = AVAGO_SERDES_TX_DATA_SEL_PRBS9;
                rxPatternData = AVAGO_SERDES_RX_CMP_DATA_PRBS9;
                break;
            case PRBS11:
                txPatternData = AVAGO_SERDES_TX_DATA_SEL_PRBS11;
                rxPatternData = AVAGO_SERDES_RX_CMP_DATA_PRBS11;
                break;
            case PRBS15:
                txPatternData = AVAGO_SERDES_TX_DATA_SEL_PRBS15;
                rxPatternData = AVAGO_SERDES_RX_CMP_DATA_PRBS15;
                break;
            case PRBS23:
                txPatternData = AVAGO_SERDES_TX_DATA_SEL_PRBS23;
                rxPatternData = AVAGO_SERDES_RX_CMP_DATA_PRBS23;
                break;
            case PRBS31:
                txPatternData = AVAGO_SERDES_TX_DATA_SEL_PRBS31;
                rxPatternData = AVAGO_SERDES_RX_CMP_DATA_PRBS31;
                break;
            default:
                return GT_NOT_SUPPORTED;
        }

        /* USER modes txPattern: 1T, 2T, 5T, 10T */
        if (txPattern <= _10T)
        {
            mvHwsAvagoAccessLock(devNum);
            /* Sets the USER TX data source */
            avago_serdes_set_tx_data_sel(aaplSerdesDb[devNum], sbus_addr, AVAGO_SERDES_TX_DATA_SEL_USER);
            mvHwsAvagoAccessUnlock(devNum);
            CHECK_AVAGO_RET_CODE();

            mvHwsAvagoAccessLock(devNum);
            /* Sets the USER pattern to compare incoming data against, Auto-seed to received 40 bit repeating pattern */
            avago_serdes_set_rx_cmp_data(aaplSerdesDb[devNum], sbus_addr, AVAGO_SERDES_RX_CMP_DATA_SELF_SEED);
            mvHwsAvagoAccessUnlock(devNum);
            CHECK_AVAGO_RET_CODE();

            mvHwsAvagoAccessLock(devNum);
            /* Loads the 80-bit value into the TX user data register */
            avago_serdes_set_tx_user_data(aaplSerdesDb[devNum], sbus_addr, tx_user);
            mvHwsAvagoAccessUnlock(devNum);
            CHECK_AVAGO_RET_CODE();

            mvHwsAvagoAccessLock(devNum);
            /* reset counter */
            avago_serdes_error_reset(aaplSerdesDb[devNum], sbus_addr);
            mvHwsAvagoAccessUnlock(devNum);
            CHECK_AVAGO_RET_CODE();
        }
        else /* for PRBS modes */
        {
            mvHwsAvagoAccessLock(devNum);
            /* Sets the PRBS TX data source */
            avago_serdes_set_tx_data_sel(aaplSerdesDb[devNum], sbus_addr, txPatternData);
            mvHwsAvagoAccessUnlock(devNum);
            CHECK_AVAGO_RET_CODE();

            mvHwsAvagoAccessLock(devNum);
            /* Sets the PRBS pattern to compare incoming data against */
            avago_serdes_set_rx_cmp_data(aaplSerdesDb[devNum], sbus_addr, rxPatternData);
            mvHwsAvagoAccessUnlock(devNum);
            CHECK_AVAGO_RET_CODE();

            /* Sets the data comparisons (0x203): Turn on data comparison with Compare Sum and PRBS-generated data */
            CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x3, 0x203, NULL));

            mvHwsAvagoAccessLock(devNum);
            /* reset counter */
            avago_serdes_error_reset(aaplSerdesDb[devNum], sbus_addr);
            mvHwsAvagoAccessUnlock(devNum);
            CHECK_AVAGO_RET_CODE();
        }
    }
    else /* for SERDES_NORMAL mode */
    {
        mvHwsAvagoAccessLock(devNum);
        /* Sets the TX data source */
        avago_serdes_set_tx_data_sel(aaplSerdesDb[devNum], sbus_addr, AVAGO_SERDES_TX_DATA_SEL_CORE);
        mvHwsAvagoAccessUnlock(devNum);
        CHECK_AVAGO_RET_CODE();

        mvHwsAvagoAccessLock(devNum);
        /* Sets the PRBS pattern to compare incoming data against */
        avago_serdes_set_rx_cmp_data(aaplSerdesDb[devNum], sbus_addr, AVAGO_SERDES_RX_CMP_DATA_OFF);
        mvHwsAvagoAccessUnlock(devNum);
        CHECK_AVAGO_RET_CODE();
    }
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTestGenGet
*
* DESCRIPTION:
*       Get configuration of the Serdes test generator/checker.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       txPatternPtr - pattern to transmit ("Other" means any mode not
*                       included explicitly in MV_HWS_SERDES_TX_PATTERN type)
*       modePtr      - test mode or normal
*
* RETURNS:
*       GT_OK  - on success
*       GT_NOT_SUPPORTED - unexpected pattern
*       GT_FAIL - HW error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesTestGenGet
(
    GT_U8                       devNum,
    GT_UOPT                     portGroup,
    GT_UOPT                     serdesNum,
    MV_HWS_SERDES_TX_PATTERN    *txPatternPtr,
    MV_HWS_SERDES_TEST_GEN_MODE *modePtr
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    txPatternPtr = txPatternPtr;
    modePtr = modePtr;
#else

    GT_U32 sbus_addr;
    Avago_serdes_tx_data_sel_t data_sel;
    long tx_user[2] = {0};

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    mvHwsAvagoAccessLock(devNum);
    data_sel = avago_serdes_get_tx_data_sel(aaplSerdesDb[devNum], sbus_addr);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();

    switch(data_sel)
    {
        case AVAGO_SERDES_TX_DATA_SEL_PRBS7:
            *txPatternPtr = PRBS7;
            break;
        case AVAGO_SERDES_TX_DATA_SEL_PRBS9:
            *txPatternPtr = PRBS9;
            break;
        case AVAGO_SERDES_TX_DATA_SEL_PRBS11:
            *txPatternPtr = PRBS11;
            break;
        case AVAGO_SERDES_TX_DATA_SEL_PRBS15:
            *txPatternPtr = PRBS15;
            break;
        case AVAGO_SERDES_TX_DATA_SEL_PRBS23:
            *txPatternPtr = PRBS23;
            break;
        case AVAGO_SERDES_TX_DATA_SEL_PRBS31:
            *txPatternPtr = PRBS31;
            break;
        case AVAGO_SERDES_TX_DATA_SEL_USER: /* User pattern generator */
            mvHwsAvagoAccessLock(devNum);
            avago_serdes_get_tx_user_data(aaplSerdesDb[devNum], sbus_addr, tx_user);
            mvHwsAvagoAccessUnlock(devNum);
            tx_user[0] = tx_user[0] & 0x3FF;
            tx_user[1] = (tx_user[1] >> 4) & 0x3FFF;

            if (tx_user[0] == 0x2AA)
            {
                *txPatternPtr = _1T;
            }
            else if (tx_user[0] == 0x333)
            {
                *txPatternPtr = _2T;
            }
            else if (tx_user[0] == 0x1F)
            {
                *txPatternPtr = _5T;
            }
            else if ((tx_user[0] == 0x3FF) && (tx_user[1] == 0x3FF0))
            {
                *txPatternPtr = _10T;
            }
            break;
        default:
            *txPatternPtr = Other;
            break;
    }

    *modePtr = (data_sel == AVAGO_SERDES_TX_DATA_SEL_CORE) ? SERDES_NORMAL : SERDES_TEST;
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTestGenStatus
*
* DESCRIPTION:
*       Read the tested pattern receive error counters.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       txPattern - pattern to transmit
*       counterAccumulateMode - Enable/Disable reset the accumulation of error counters
*
* OUTPUTS:
*       error counter.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesTestGenStatus
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          serdesNum,
    MV_HWS_SERDES_TX_PATTERN        txPattern,
    GT_BOOL                         counterAccumulateMode,
    MV_HWS_SERDES_TEST_GEN_STATUS   *status
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    txPattern = txPattern;
    counterAccumulateMode = counterAccumulateMode;
    hwsOsMemSetFuncPtr(status,0,sizeof(*status));
#else

    GT_U32 sbus_addr;
    GT_U32 data;

    /* avoid warning */
    txPattern = txPattern;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, AVAGO_EXTERNAL_STATUS, &data, (1 <<2)));

    mvHwsAvagoAccessLock(devNum);
    /* Retrieves the value of the error counter */
    status->errorsCntr = avago_serdes_get_errors(aaplSerdesDb[devNum], sbus_addr, AVAGO_LSB, 1);
    mvHwsAvagoAccessUnlock(devNum);
    status->lockStatus = (data) ? 0 : 1; /* if bit[2] is set then there is not signal (and vice versa) */
    status->txFramesCntr.s[0] = 0;
    status->txFramesCntr.s[1] = 0;
    status->txFramesCntr.s[2] = 0;
    status->txFramesCntr.s[3] = 0;

    CHECK_AVAGO_RET_CODE();

    /* reset counter */
    if (counterAccumulateMode == GT_FALSE)
    {
        mvHwsAvagoAccessLock(devNum);
        avago_serdes_error_reset(aaplSerdesDb[devNum], sbus_addr);
        mvHwsAvagoAccessUnlock(devNum);
        CHECK_AVAGO_RET_CODE();
    }
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesLoopback
*
* DESCRIPTION:
*       Perform an Internal/External SERDES loopback mode for Debug/BIST use.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical Serdes number
*       lbMode    - loopback mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesLoopback
(
    GT_U8             devNum,
    GT_UOPT           portGroup,
    GT_UOPT           serdesNum,
    MV_HWS_SERDES_LB  lbMode
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    if(lbMode != SERDES_LP_DISABLE)
    {
        return GT_NOT_IMPLEMENTED;
    }
#else

    GT_U32 sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    switch(lbMode)
    {
        case SERDES_LP_DISABLE:
            /* set loopback_clock=ref_clock */
            CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x30, 0, NULL));
            CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x8, 0x100, NULL));
            mvHwsAvagoAccessLock(devNum);
            avago_serdes_set_tx_data_sel(aaplSerdesDb[devNum], sbus_addr, AVAGO_SERDES_TX_DATA_SEL_CORE);
            mvHwsAvagoAccessUnlock(devNum);
            break;
        case SERDES_LP_AN_TX_RX:
            /* set loopback_clock=ref_clock */
            CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x30, 0, NULL));
            /* Internal Loopback mode */
            CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x8, 0x101, NULL));
            break;
        case SERDES_LP_DIG_RX_TX:
            /* set loopback and loopback_clock=CDR_clock */
            CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x30, 0x10, NULL));
            mvHwsAvagoAccessLock(devNum);
            avago_serdes_set_tx_data_sel(aaplSerdesDb[devNum], sbus_addr, AVAGO_SERDES_TX_DATA_SEL_LOOPBACK);
            mvHwsAvagoAccessUnlock(devNum);
            break;
        default:
            return GT_NOT_SUPPORTED;
    }

    CHECK_AVAGO_RET_CODE();
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesLoopbackGet
*
* DESCRIPTION:
*       Gets the status of Internal/External SERDES loopback mode.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical Serdes number
*
* OUTPUTS:
*     lbModePtr - current loopback mode
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesLoopbackGet
(
    GT_U8             devNum,
    GT_UOPT           portGroup,
    GT_UOPT           serdesNum,
    MV_HWS_SERDES_LB  *lbModePtr
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    *lbModePtr = SERDES_LP_DISABLE;
#else

    GT_U32  sbus_addr;
    GT_BOOL internalLB;
    Avago_serdes_tx_data_sel_t  externalLB;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    mvHwsAvagoAccessLock(devNum);
    /* Get TRUE if internal loopback is enabled, FALSE if external signal */
    internalLB = avago_serdes_get_rx_input_loopback(aaplSerdesDb[devNum], sbus_addr);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();

    if (internalLB == GT_TRUE)
    {
        /* Internal Loopback mode */
        *lbModePtr = SERDES_LP_AN_TX_RX;
        return GT_OK;
    }

    mvHwsAvagoAccessLock(devNum);
    /* Get the selected TX data source */
    externalLB = avago_serdes_get_tx_data_sel(aaplSerdesDb[devNum], sbus_addr);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();

    /* Check the External Loopback or Disable mode */
    *lbModePtr = (externalLB == AVAGO_SERDES_TX_DATA_SEL_LOOPBACK) ? SERDES_LP_DIG_TX_RX : SERDES_LP_DISABLE;
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTxEnable
*
* DESCRIPTION:
*       Enable/Disable Tx.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       enable    - whether to enable or disable Tx.
*
* OUTPUTS:
*    None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesTxEnable
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     enable
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    enable = enable;
#else

    GT_U32  sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    mvHwsAvagoAccessLock(devNum);
    /* Set the TX output Enabled/Disabled */
    avago_serdes_set_tx_output_enable(aaplSerdesDb[devNum], sbus_addr, enable);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTxEnableGet
*
* DESCRIPTION:
*        Gets the status of SERDES Tx mode - Enable/Disable transmission of packets.
*        Use this API to disable Tx for loopback ports.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       enablePtr:
*           - GT_TRUE - Enable transmission of packets from SERDES
*           - GT_FALSE - Disable transmission of packets from SERDES
*
* RETURNS:
*       0  - on success
*       1  - on error
*
* COMMENTS:
*       Disabling transmission of packets from SERDES causes to link down
*       of devices that are connected to the port.
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesTxEnableGet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     *enablePtr
)
{
#ifdef ASIC_SIMULATION    
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    enablePtr = enablePtr;
#else

    GT_U32  sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    mvHwsAvagoAccessLock(devNum);
    /* Get the SERDES TX output mode: TRUE in enabled, FALSE otherwise */
    *enablePtr = avago_serdes_get_tx_output_enable(aaplSerdesDb[devNum], sbus_addr);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTxIfSelect
*
* DESCRIPTION:
*       Tx interface select.
*
* INPUTS:
*       devNum        - system device number
*       portGroup     - port group (core) number
*       serdesNum     - physical serdes number
*       serdesTxIfNum - number of serdes Tx interface
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesTxIfSelect
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         serdesNum,
    GT_UREG_DATA    serdesTxIfNum
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    serdesTxIfNum = serdesTxIfNum;
#else

    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, serdesTxIfNum, (7 << 3)));
#endif /* ASIC_SIMULATION */

    return GT_OK; 
}

/*******************************************************************************
* mvHwsAvagoSerdesTxIfSelectGet
*
* DESCRIPTION:
*       Return the SERDES Tx interface select number.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       serdesTxIfNum - number of serdes Tx interface
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesTxIfSelectGet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    GT_U32      *serdesTxIfNum
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    *serdesTxIfNum = 0;
#else

    GT_UREG_DATA data;

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &data, (7 << 3)));

    if (data > 7)
    {
        return GT_BAD_PARAM;
    }

    *serdesTxIfNum = data;
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesAutoTuneStartExt
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - physical serdes number
*       rxTraining - Rx Training modes
*       txTraining - Tx Training modes
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesAutoTuneStartExt
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    MV_HWS_RX_TRAINING_MODES    rxTraining,
    MV_HWS_TX_TRAINING_MODES    txTraining
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    rxTraining = rxTraining;
    txTraining = txTraining;
#else

    GT_U32  sbus_addr;
    Avago_serdes_dfe_tune_t   dfe_state;
    Avago_serdes_pmd_config_t pmd_mode;

    if ((txTraining != IGNORE_TX_TRAINING) && (rxTraining != IGNORE_RX_TRAINING))
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    /* Initialize the dfe_state struct with default values */
    avago_serdes_tune_init(aaplSerdesDb[devNum], &dfe_state);
    CHECK_AVAGO_RET_CODE();

    /* Initialize the pmd_mode */
    avago_serdes_pmd_init(aaplSerdesDb[devNum], &pmd_mode);

    switch(rxTraining)
    {
        case IGNORE_RX_TRAINING:
            break;
        case START_CONTINUE_ADAPTIVE_TUNING:
            /* start continues adaptive tuning */
            dfe_state.tune_mode = AVAGO_DFE_START_ADAPTIVE;
            break;
        case STOP_CONTINUE_ADAPTIVE_TUNING:
            /* stop continues adaptive tuning */
            dfe_state.tune_mode = AVAGO_DFE_STOP_ADAPTIVE;
            break;
        case ONE_SHOT_DFE_TUNING:
            dfe_state.tune_mode = AVAGO_DFE_ICAL;
            break;
        default:
            return GT_NOT_SUPPORTED;
    }

    if ((rxTraining != IGNORE_RX_TRAINING))
    {
        mvHwsAvagoAccessLock(devNum);
        /* Launches and halts DFE tuning procedures */
        avago_serdes_tune(aaplSerdesDb[devNum], sbus_addr, &dfe_state);
        mvHwsAvagoAccessUnlock(devNum);
        CHECK_AVAGO_RET_CODE();
    }

    if (txTraining != IGNORE_TX_TRAINING)
    {
        pmd_mode.sbus_addr = sbus_addr;
        pmd_mode.disable_timeout = TRUE;
        pmd_mode.train_mode = (txTraining == START_TRAINING) ? AVAGO_PMD_TRAIN : AVAGO_PMD_BYPASS;

        /* change IF_Dwell_Shift to 0x1 */
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x26, 0x5C01, NULL));

        mvHwsAvagoAccessLock(devNum);
        /* Launches and halts PMD link training procedure */
        avago_serdes_pmd_train(aaplSerdesDb[devNum], &pmd_mode);
        mvHwsAvagoAccessUnlock(devNum);
        CHECK_AVAGO_RET_CODE();
    }
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting.
*       This function calls to the mvHwsAvagoSerdesAutoTuneStartExt, which includes
*       all the functional options.
*       Can be run after create port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - physical serdes number
*       rxTraining - Rx Training (true (AVAGO_DFE_ICAL) /false)
*       txTraining - Tx Training (true (AVAGO_PMD_TRAIN) /false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesAutoTuneStart
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     rxTraining,
    GT_BOOL     txTraining
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    rxTraining = rxTraining;
    txTraining = txTraining;
#else

    MV_HWS_RX_TRAINING_MODES    rxMode;
    MV_HWS_TX_TRAINING_MODES    txMode;

    if (txTraining && rxTraining)
    {
        return GT_BAD_PARAM;
    }

    /* for TRUE: set Single time iCal */
    rxMode = (rxTraining == GT_TRUE) ? ONE_SHOT_DFE_TUNING : IGNORE_RX_TRAINING;

    txMode = (txTraining == GT_TRUE) ? START_TRAINING : IGNORE_TX_TRAINING;

    CHECK_STATUS(mvHwsAvagoSerdesAutoTuneStartExt(devNum, portGroup, serdesNum, rxMode, txMode));
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesRxAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the Rx Training starting.
*       Can be run after create port.
*
* INPUTS:
*       devNum - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesRxAutoTuneStart
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     rxTraining
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    rxTraining = rxTraining;
#else

    MV_HWS_RX_TRAINING_MODES    rxMode;

    /* for TRUE: set Single time iCal */
    rxMode = (rxTraining == GT_TRUE) ? ONE_SHOT_DFE_TUNING : IGNORE_RX_TRAINING;

    CHECK_STATUS(mvHwsAvagoSerdesAutoTuneStartExt(devNum, portGroup, serdesNum, rxMode, IGNORE_TX_TRAINING));
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTxAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the TX training starting.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       txTraining - Tx Training (true/false)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesTxAutoTuneStart
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     txTraining
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    txTraining = txTraining;
#else

    MV_HWS_TX_TRAINING_MODES    txMode;

    /* for TRUE: set the AVAGO_PMD_TRAIN mode, for FALSE: set the AVAGO_PMD_BYPASS mode */
    txMode = (txTraining == GT_TRUE) ? START_TRAINING : STOP_TRAINING;

    CHECK_STATUS(mvHwsAvagoSerdesAutoTuneStartExt(devNum, portGroup, serdesNum, IGNORE_RX_TRAINING, txMode));
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesAutoTuneStatus
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
GT_STATUS mvHwsAvagoSerdesAutoTuneStatus
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    rxStatus = rxStatus;
    txStatus = txStatus;
#else
    GT_U32 i;

    if ((NULL == rxStatus) && (NULL == txStatus))
    {
        return GT_BAD_PARAM;
    }

    if (rxStatus != NULL)
    {
        *rxStatus = TUNE_NOT_COMPLITED;

        for (i = 0; i < MV_AVAGO_TRAINING_TIMEOUT; i++)
        {
            /* Delay in 1ms */
            hwsOsExactDelayPtr(devNum, portGroup, 1);

            CHECK_STATUS(mvHwsAvagoSerdesRxAutoTuneStatusShort(devNum, portGroup, serdesNum, rxStatus));
            if (*rxStatus == TUNE_PASS)
                break;
        }

        /* in case training failed or took too long/short */
        if ((*rxStatus != TUNE_PASS) || (i == MV_AVAGO_TRAINING_TIMEOUT))
        {
            *rxStatus = TUNE_FAIL; /* else *rxStatus = TUNE_PASS */
        }
    }

    if (txStatus != NULL)
    {
        *txStatus = TUNE_NOT_COMPLITED;

        for (i = 0; i < MV_AVAGO_TRAINING_TIMEOUT; i++)
        {
            /* Delay in 1ms */
            hwsOsExactDelayPtr(devNum, portGroup, 1);

            CHECK_STATUS(mvHwsAvagoSerdesTxAutoTuneStatusShort(devNum, portGroup, serdesNum, txStatus));
            if (*txStatus == TUNE_PASS)
                break;
        }

        /* in case training failed or took too long/short */
        if ((*txStatus != TUNE_PASS) || (i == MV_AVAGO_TRAINING_TIMEOUT))
        {
            *txStatus = TUNE_FAIL; /* else *txStatus = TUNE_PASS */
        }
    }
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesTxAutoTuneStop
*
* DESCRIPTION:
*       Per SERDES stop the TX training
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
GT_STATUS mvHwsAvagoSerdesTxAutoTuneStop
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
#else

    GT_U32  sbus_addr;
    Avago_serdes_pmd_config_t pmd_mode;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    /* Initialize the pmd_mode */
    avago_serdes_pmd_init(aaplSerdesDb[devNum], &pmd_mode);

    pmd_mode.sbus_addr = sbus_addr;
    pmd_mode.disable_timeout = TRUE;
    pmd_mode.train_mode = AVAGO_PMD_RESTART;

    mvHwsAvagoAccessLock(devNum);
    /* stop the PMD link training procedure */
    avago_serdes_pmd_train(aaplSerdesDb[devNum], &pmd_mode);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesManualCtleConfig
*
* DESCRIPTION:
*       Set the Serdes Manual CTLE config for DFE
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       dcGain        - DC-Gain value        (rang: 0-255)
*       lowFrequency  - CTLE Low-Frequency   (rang: 0-15)
*       highFrequency - CTLE High-Frequency  (rang: 0-15)
*       bandWidth     - CTLE Band-width      (rang: 0-15)
*       loopBandwidth - CTLE Loop Band-width (rang: 0-15)
*       squelch       - Signal OK threshold  (rang: 0-310)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesManualCtleConfig
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_UOPT     dcGain,
    GT_UOPT     lowFrequency,
    GT_UOPT     highFrequency,
    GT_UOPT     bandWidth,
    GT_UOPT     loopBandwidth,
    GT_UOPT     squelch
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    dcGain = dcGain;
    lowFrequency = lowFrequency;
    highFrequency = highFrequency;
    bandWidth = bandWidth;
    loopBandwidth = loopBandwidth;
    squelch = squelch;
#else

    GT_U32  sbus_addr;
    GT_U32  ctleParam=0;
    GT_U32  row;
    GT_U32  column = 2;
    GT_U32  data;

    if (squelch > 308)
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    for (row = 0; row <= 4; row++)
    {
        switch (row)
        {
            case 0:
                ctleParam = highFrequency;
                break;
            case 1:
                ctleParam = lowFrequency;
                break;
            case 2:
                ctleParam = dcGain;
                break;
            case 3:
                ctleParam = bandWidth;
                break;
            case 4:
                ctleParam = loopBandwidth;
                break;
        }

        data = ((column << 12) | (row << 8)) | ctleParam;

        /* Update the param values of DFE */
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x26, data, NULL));
    }

    /* formula to convert the milli-volt to fix value */
    squelch = (squelch < 68) ? 68 : squelch;
    squelch = (squelch - 68) / 16;

    /* Set the signal OK threshold on Serdes */
    CHECK_STATUS(mvHwsAvagoSerdesSignalOkCfg(devNum, portGroup, serdesNum, squelch));
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesManualCtleConfigGet
*
* DESCRIPTION:
*       Get the Serdes CTLE (RX) configurations
*
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical lane number
*
* OUTPUTS:
*       configParams - Ctle params structures:
*       dcGain         DC-Gain value        (rang: 0-255)
*       lowFrequency   CTLE Low-Frequency   (rang: 0-15)
*       highFrequency  CTLE High-Frequency  (rang: 0-15)
*       bandWidth      CTLE Band-width      (rang: 0-15)
*       loopBandwidth  CTLE Loop Band-width (rang: 0-15)
*       squelch        Signal OK threshold  (rang: 0-310)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesManualCtleConfigGet
(
    GT_U8                               devNum,
    GT_UOPT                             portGroup,
    GT_UOPT                             serdesNum,
    MV_HWS_MAN_TUNE_CTLE_CONFIG_DATA    *configParams
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    hwsOsMemSetFuncPtr(configParams,0,sizeof(*configParams));
#else

    MV_HWS_AUTO_TUNE_RESULTS    results;
    GT_UOPT                     signalThreshold;

    CHECK_STATUS(mvHwsAvagoSerdesAutoTuneResult(devNum, portGroup, serdesNum, &results));

    configParams->highFrequency = results.HF;
    configParams->lowFrequency = results.LF;
    configParams->dcGain = results.DC;
    configParams->bandWidth = results.BW;
    configParams->loopBandwidth = results.LB;

    /* Get the signal OK threshold on Serdes */
    CHECK_STATUS(mvHwsAvagoSerdesSignalOkThresholdGet(devNum, portGroup, serdesNum, &signalThreshold));
    configParams->squelch = signalThreshold;

#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesSignalOkCfg
*
* DESCRIPTION:
*       Set the signal OK threshold on Serdes
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       signalThreshold - Signal OK threshold (0-15)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesSignalOkCfg
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_UOPT     signalThreshold
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    signalThreshold = signalThreshold;
#else

    GT_U32  sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    mvHwsAvagoAccessLock(devNum);
    avago_serdes_initialize_signal_ok(aaplSerdesDb[devNum], sbus_addr, signalThreshold);
    mvHwsAvagoAccessUnlock(devNum);

    CHECK_AVAGO_RET_CODE();
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesSignalOkThresholdGet
*
* DESCRIPTION:
*       Get the signal OK threshold on Serdes
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       signalThreshold - Signal OK threshold (0-15)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesSignalOkThresholdGet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_UOPT     *signalThreshold
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    *signalThreshold = 0;
#else

    GT_U32  sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    mvHwsAvagoAccessLock(devNum);
    *signalThreshold = avago_serdes_get_signal_ok_threshold(aaplSerdesDb[devNum], sbus_addr);
    mvHwsAvagoAccessUnlock(devNum);

    CHECK_AVAGO_RET_CODE();
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesErrorInject
*
* DESCRIPTION:
*       Injects errors into the RX or TX data
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       numOfBits - Number of error bits to inject (max=65535)
*       serdesDirection - Rx or Tx
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesErrorInject
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_UOPT     numOfBits,
    SERDES_DIRECTION    serdesDirection
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    numOfBits = numOfBits;
    serdesDirection = serdesDirection;
#else

    GT_U32  sbus_addr;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    mvHwsAvagoAccessLock(devNum);
    (serdesDirection == RX_DIRECTION) ? avago_serdes_rx_inject_error(aaplSerdesDb[devNum], sbus_addr, numOfBits)
                                      : avago_serdes_tx_inject_error(aaplSerdesDb[devNum], sbus_addr, numOfBits);
    mvHwsAvagoAccessUnlock(devNum);

    CHECK_AVAGO_RET_CODE();
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesShiftSamplePoint
*
* DESCRIPTION:
*       Shift Serdes sampling point earlier in time
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       delay     - set the delay (0-0xF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesShiftSamplePoint
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_U32      delay
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    delay = delay;
#else

    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x18, 0x402e, NULL));
    delay = (delay<<4) + delay;
    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x19, delay, NULL));
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesArrayAutoTuneSet
*
* DESCRIPTION:
*       Set iCAL(CTLE and DFE) or pCAL(DFE) Auto Tuning on multiple Serdeses
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesArr - collection of SERDESes to configure
*       numOfSerdes  - number of SERDESes to configure
*       trainingMode - for value 0: set iCAL mode,
*                      for value 1: set pCAL mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesArrayAutoTuneSet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_U32      *serdesArr,
    GT_UOPT     numOfSerdes,
    GT_BOOL     trainingMode
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum       = devNum;
    portGroup    = portGroup;
    serdesArr    = serdesArr;
    numOfSerdes  = numOfSerdes;
    trainingMode = trainingMode;
#else

    GT_U8   interruptData;
    GT_U32  i, k, j;
    GT_U32  serdesArr_copy[MAX_AVAGO_SERDES_NUMBER] = {0};
    GT_U32  passCounter = 0;
    GT_U32  numOfActiveSerdes = 0;

    MV_HWS_AUTO_TUNE_STATUS  rxStatus;

    /* copy the original Serdes array for saving the data */
    hwsOsMemCopyFuncPtr(serdesArr_copy, serdesArr, sizeof(GT_U32) * numOfSerdes);

    /* for iCAL data=0x1, for pCAL data=0x2 */
    interruptData = (trainingMode) ? 0x2 : 0x1;

    for (j=0; j < numOfSerdes; j++)
    {
        if (serdesArr_copy[j] == SKIP_SERDES) /* skip Serdes[j] if No Signal-Detect */
            continue;

        numOfActiveSerdes++;

        /* run iCAL(CTLE and DFE) or pCAL(DFE) */
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArr_copy[j], 0xa, interruptData, NULL));
    }

    if (numOfActiveSerdes == 0)
    {
        osPrintf("No need to run Training on Serdes\n");
        return GT_OK;
    }

    for (k = 0; k < MV_AVAGO_TRAINING_TIMEOUT; k++)
    {
        for (i=0; i < numOfSerdes; i++)
        {
            if (serdesArr_copy[i] == SKIP_SERDES) /* if Serdes pass then no need to check status on it */
                continue;

            /* check that training completed */
            CHECK_STATUS(mvHwsAvagoSerdesRxAutoTuneStatusShort(devNum, portGroup, serdesArr_copy[i], &rxStatus));
            if (rxStatus == TUNE_PASS)
            {
                passCounter++;

                /* Stop traning if Timeout reached or if traning finished too fast */
                if ((k >= MV_AVAGO_TRAINING_TIMEOUT-1) || (k < 10))
                {
                    /* stop the training */
                    CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArr_copy[i], 0xa, 0x0, NULL));
                    osPrintf("Error: Training failed. Stopped training k=%d\n", k);
                    return GT_TIMEOUT;
                }

                serdesArr_copy[i] = SKIP_SERDES;
            }
        }

        /* when all Serdeses finish, then stop the Timeout loop */
        if (passCounter == numOfActiveSerdes)
            break;
        else
            hwsOsExactDelayPtr(devNum, portGroup, 1); /* Delay of 1ms */
    }
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesEnhanceTune
*
* DESCRIPTION:
*       Set the ICAL with shifted sampling point to find best sampling point
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesArr - collection of SERDESes to configure
*       numOfSerdes - number of SERDESes to configure
*       min_LF      - Minimum LF value that can be set on Serdes (0...15)
*       max_LF      - Maximum LF value that can be set on Serdes (0...15)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesEnhanceTune
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_U32      *serdesArr,
    GT_UOPT     numOfSerdes,
    GT_U8       min_LF,
    GT_U8       max_LF
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesArr = serdesArr;
    numOfSerdes = numOfSerdes;
    min_LF = min_LF;
    max_LF = max_LF;
#else

    GT_U32  sbus_addr;
    GT_U32  dly, eye;
    GT_U32  sdDetect;
    GT_U32  i, j, k, LF, GAIN;
    GT_U32  static_delay[] = {10, 11, 13, 15}; /* these are currently best belay values */
    GT_U32  best_dly[MAX_AVAGO_SERDES_NUMBER] = {0};
    GT_U32  best_eye[MAX_AVAGO_SERDES_NUMBER] = {0};
    GT_U32  best_LF[MAX_AVAGO_SERDES_NUMBER]  = {0};
    GT_U32  LF1_Arr[MAX_AVAGO_SERDES_NUMBER]  = {0};
    GT_U32  LF2_Arr[MAX_AVAGO_SERDES_NUMBER]  = {0};
    GT_U32  DFEgain_Arr[MAX_AVAGO_SERDES_NUMBER]    = {0};
    GT_U32  serdesArrValid[MAX_AVAGO_SERDES_NUMBER] = {0};

#ifdef TUNE_EXEC_TIME
    struct timeval tvalBefore, tvalAfter;
    gettimeofday (&tvalBefore, NULL);
#endif

    for (i=0; i < MAX_AVAGO_SERDES_NUMBER; i++)
        best_eye[i] = 4;

    if (numOfSerdes > MAX_AVAGO_SERDES_NUMBER)
    {
        osPrintf("numOfSerdes %d is greater than MAX_AVAGO_SERDES_NUMBER %d\n", numOfSerdes, MAX_AVAGO_SERDES_NUMBER);
        return GT_BAD_PARAM;
    }

    /* check CDR lock and Signal Detect on all Serdeses */
    for (i=0; i < numOfSerdes; i++)
    {
        CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, MV_HWS_SERDES_NUM(serdesArr[i]), &sbus_addr));

        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, MV_HWS_SERDES_NUM(serdesArr[i]),
                                            AVAGO_EXTERNAL_STATUS, &sdDetect, (1 << 2)));
        if (sdDetect != 0)
        {
            serdesArr[i] = SKIP_SERDES; /* change the Serdes number to mark this array's element as not relevant */
            continue;
        }

        /* set SamplePoint to default value (delay=0) */
        CHECK_STATUS(mvHwsAvagoSerdesShiftSamplePoint(devNum, portGroup, serdesArr[i], static_delay[0]));
    }

    /* copy the original Serdes array for saving the data */
    hwsOsMemCopyFuncPtr(serdesArrValid, serdesArr, sizeof(GT_U32) * numOfSerdes);

    /* run iCAL(CTLE and DFE) */
    CHECK_STATUS(mvHwsAvagoSerdesArrayAutoTuneSet(devNum, portGroup, serdesArr, numOfSerdes, ICAL_TRAINING_MODE));

    /* save the LF value of all Serdeses */
    for (k=0; k < numOfSerdes; k++)
    {
        if (serdesArr[k] == SKIP_SERDES) /* No Signal-Detect on this Serdes */
            continue;

        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArr[k], 0x126, ((2 << 12) | (1 << 8)), &LF));
        best_LF[k]=LF;
    }

    /* find the peak of the eye accoding to delay */
    for (i=0; i < (sizeof(static_delay)/sizeof(GT_U32)); i++)
    {
        dly = static_delay[i];

        for (k=0; k < numOfSerdes; k++)
        {
            if (serdesArr[k] == SKIP_SERDES) /* No Signal-Detect on this Serdes */
                continue;

            CHECK_STATUS(mvHwsAvagoSerdesShiftSamplePoint(devNum, portGroup, serdesArr[k], dly));
        }

        /* trigger pCAL(DFE) on all relevant Serdeses */
        CHECK_STATUS(mvHwsAvagoSerdesArrayAutoTuneSet(devNum, portGroup, serdesArr, numOfSerdes, PCAL_TRAINING_MODE));

        for (j=0; j < numOfSerdes; j++)
        {
            if (serdesArr[j] == SKIP_SERDES) /* No Signal-Detect on this Serdes */
                continue;

            CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesArr[j], &sbus_addr));

            mvHwsAvagoAccessLock(devNum);
            /* get a simple eye metric in range [0..1000] */
            eye = avago_serdes_eye_get_simple_metric(aaplSerdesDb[devNum], sbus_addr);
            mvHwsAvagoAccessUnlock(devNum);
            CHECK_AVAGO_RET_CODE();

            CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArr[j], 0x126, ((2 << 12) | (1 << 8)), &LF));

            if (eye >= (best_eye[j] - MV_AVAGO_EYE_TAP))
            {
                if (eye > best_eye[j])
                {
                    best_eye[j] = eye;
                }
                best_dly[j] = dly;

                if (LF >= best_LF[j])
                    best_LF[j] = LF;
            }
            osPrintf("current_eye[%d]=%d dly[%d]=%d\n", j, eye, j, dly);

            if (best_eye[j] < MV_AVAGO_MIN_EYE_SIZE)
            {
                osPrintf("Error on Serdes %d: Eye is too small (size is %d). Training Failed\n", serdesArr[j], best_eye[j]);
                return GT_FAIL;
            }
        }
    }

    for (k=0; k < numOfSerdes; k++)
    {
        if (serdesArr[k] == SKIP_SERDES) /* skip Serdes if the dly is the same as the best_dly */
            continue;

        CHECK_STATUS(mvHwsAvagoSerdesShiftSamplePoint(devNum, portGroup, serdesArr[k], best_dly[k]));
    }

    /* run iCAL(CTLE and DFE) with best delay */
    CHECK_STATUS(mvHwsAvagoSerdesArrayAutoTuneSet(devNum, portGroup, serdesArr, numOfSerdes, ICAL_TRAINING_MODE));

    for (k=0; k < numOfSerdes; k++)
    {
        /* read the LF value */
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArrValid[k], 0x126, ((2 << 12) | (1 << 8)), &LF1_Arr[k]));
    }

    /* run iCAL(CTLE and DFE) with best delay */
    CHECK_STATUS(mvHwsAvagoSerdesArrayAutoTuneSet(devNum, portGroup, serdesArr, numOfSerdes, ICAL_TRAINING_MODE));

    for (k=0; k < numOfSerdes; k++)
    {
        /* read again the LF value after run the iCAL */
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArrValid[k], 0x126, ((2 << 12) | (1 << 8)), &LF2_Arr[k]));

        /* read the DFE gain */
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArrValid[k], 0x126, ((3 << 12) | (0 << 8)), &DFEgain_Arr[k]));

        LF = (LF1_Arr[k] + LF2_Arr[k])/2; /* take the avarge from two tunning values */
        LF = (LF < min_LF) ? min_LF : LF;
        LF = (LF > max_LF) ? max_LF : LF;

        /* write the selected LF value back to Serdes */
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArrValid[k], 0x26, ((2 << 12) | (1 << 8) | LF), NULL));

        GAIN = (DFEgain_Arr[k] > 1) ? (DFEgain_Arr[k]-1) : DFEgain_Arr[k];
        /* write the selected GAIN value back to Serdes */
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesArrValid[k], 0x26, ((3 << 12) | (0 << 8) | GAIN), NULL));

        /* write the best delay and store it in SERDES_EXTERNAL_CONFIGURATION_2 (offset 0x8)*/
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesArrValid[k], SERDES_EXTERNAL_CONFIGURATION_2,
                                            (LF << 4 | best_dly[k]), 0xFF));

        osPrintf("Serdes %d: setting best_dly=%d best_eye=%d LF=%d GAIN=%d\n", serdesArrValid[k], best_dly[k], best_eye[k], LF, GAIN);
    }

#ifdef TUNE_EXEC_TIME
    gettimeofday (&tvalAfter, NULL);
    printf("mvHwsAvagoSerdesEnhanceTune Time in microseconds: %ld microseconds\n", ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L
                                                        +tvalAfter.tv_usec) - tvalBefore.tv_usec);
#endif /* TUNE_EXEC_TIME */

#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/********************************************************************************
   runMultipleEnhanceTune is a debug function for Testing only for running
   the mvHwsAvagoSerdesEnhanceTune from Terminal
********************************************************************************/
void runMultipleEnhanceTune
(
    GT_U32  serdesNumbers0_31,
    GT_U32  serdesNumbers32_63,
    GT_U8   min_LF,
    GT_U8   max_LF
)
{
    GT_U32  serdesArr[64]= {0};
    GT_U32  i, numOfSerdes;

    numOfSerdes=0; /* num of Serdes to run the function */
    for (i=0; i<32; i++)
    {
        if ((((serdesNumbers0_31 >> i) & 1) == 1))
        {
            serdesArr[numOfSerdes] = i;
            numOfSerdes++;
            osPrintf("Serdes %d\n", i);
        }
    }

    for (i=32; i<64; i++)
    {
        if ((((serdesNumbers32_63 >> (i-32)) & 1) == 1))
        {
            serdesArr[numOfSerdes] = i;
            numOfSerdes++;
            osPrintf("Serdes %d\n", i);
        }
    }

    mvHwsAvagoSerdesEnhanceTune(0, 0, serdesArr, numOfSerdes, min_LF, max_LF);
}

/*******************************************************************************
* mvHwsAvagoSerdesVoltageGet
*
* DESCRIPTION:
*       Get the voltage (in mV) from Avago Serdes
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       voltage - Serdes voltage value (in mV)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesVoltageGet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_UOPT     *voltage
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    *voltage = 0;
#else

    GT_U32      sbus_addr;
    GT_U32      avagoVoltage;
    GT_STATUS   res;

    if ( ((serdesNum != 20) && ((serdesNum < 24) || (serdesNum > 27)))
         && ((serdesNum < 33) || (serdesNum > 35)) )
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    /* get the Serdes Voltage value in mV */
    res = mvHwsAvagoSerdesSbmVoltageGet(devNum, portGroup, serdesNum, 9, &avagoVoltage);
    if(res != GT_OK)
    {
        osPrintf("mvHwsAvagoSerdesSbmVoltageGet failed (%d)\n", res);
        return GT_FAIL;
    }

    *voltage = avagoVoltage;
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

/*******************************************************************************
* mvHwsAvagoSerdesAutoTuneResult
*
* DESCRIPTION:
*       Per SERDES return the adapted tuning results
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       results - the adapted tuning results.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAvagoSerdesAutoTuneResult
(
    GT_U8                       devNum,
    GT_UOPT                     portGroup,
    GT_UOPT                     serdesNum,
    MV_HWS_AUTO_TUNE_RESULTS    *results
)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;
    serdesNum = serdesNum;
    hwsOsMemSetFuncPtr(results,0,sizeof(*results));

#else

    GT_U32  sbus_addr;
    GT_U32  data, column, i;
    GT_U32  ctle[5];
    GT_U32  dfe[13];
    Avago_serdes_tx_eq_t tx_eq;

    CHECK_STATUS(mvHwsAvagoConvertSerdesToSbusAddr(devNum, serdesNum, &sbus_addr));

    mvHwsAvagoAccessLock(devNum);
    /* Gets the TX equalization values */
    avago_serdes_get_tx_eq(aaplSerdesDb[devNum], sbus_addr, &tx_eq);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();

    results->txAmp   = tx_eq.atten;
    results->txEmph0 = tx_eq.post;
    results->txEmph1 = tx_eq.pre;

    for (i=0; i <= 12; i++)
    {
        if (i==1)
        {
            column=1;
            data = (column << 12) | (8 << 8);
        }
        else
        {
            column=3;
            data = (column << 12) | (i << 8);
        }
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x126, data, &dfe[i]));
        (dfe[i] > 0x8000) ? (dfe[i] = dfe[i] - 0x10000) : dfe[i];
        results->DFE[i] = dfe[i];
        AVAGO_DBG(("   DFE[%d] = %d \n", i, results->DFE[i]));
    }

    column=2;
    for (i = 0; i <= 4; i++)
    {
        CHECK_STATUS(mvHwsAvagoSerdesSpicoInterrupt(devNum, portGroup, serdesNum, 0x126, ((column << 12) | (i << 8)), &ctle[i]));
    }

    results->HF = ctle[0];
    results->LF = ctle[1];
    results->DC = ctle[2];
    results->BW = ctle[3];
    results->LB = ctle[4];
    mvHwsAvagoAccessLock(devNum);
    results->EO = avago_serdes_eye_get_simple_metric(aaplSerdesDb[devNum], sbus_addr);
    mvHwsAvagoAccessUnlock(devNum);
    CHECK_AVAGO_RET_CODE();

    AVAGO_DBG(("   HF = %d \n", results->HF));
    AVAGO_DBG(("   LF = %d \n", results->LF));
    AVAGO_DBG(("   DC = %d \n", results->DC));
    AVAGO_DBG(("   BW = %d \n", results->BW));
    AVAGO_DBG(("   LB = %d \n", results->LB));
    AVAGO_DBG(("   EO = %d \n", results->EO));
#endif /* ASIC_SIMULATION */

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsAvagoIfClose
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
void mvHwsAvagoIfClose(GT_U8 devNum)
{
#ifdef ASIC_SIMULATION
    /* avoid warnings */
    devNum = devNum;
#else

    osPrintf("Close AAPL for devNum %d\n", devNum);

    if(aaplSerdesDb[devNum] == NULL)
    {
        osPrintf("AAPL is already closed for devNum %d\n", devNum);
        return;
    }

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
    /* close AAPL */
    aapl_destruct(aaplSerdesDb[devNum]);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

    aaplSerdesDb[devNum] = NULL;
#endif /* ASIC_SIMULATION */
}

#endif

