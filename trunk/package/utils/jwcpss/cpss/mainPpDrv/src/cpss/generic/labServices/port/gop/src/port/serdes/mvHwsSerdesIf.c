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
* mvHwsLion2If.c
*
* DESCRIPTION:
*       Lion2 specific HW Services init
*
* FILE REVISION NUMBER:
*       $Revision: 58 $
*
*******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>
#include <port/private/mvHwsPortPrvIf.h>
#include <port/serdes/mvHwsSerdesPrvIf.h>

#ifndef CO_CPU_RUN
#include <serdes/comPhyHRev2/mvComPhyHRev2If.h>
#include <serdes/comPhyH/mvComPhyHDb.h>
#include <serdes/comPhyHRev2/mvComPhyHRev2Db.h>
#endif
#ifdef HWS_DEBUG
extern GT_FILEP fdHws;
#endif
#ifndef MV_HWS_REDUCED_BUILD
#include <mvHwsIpcApis.h>
#endif

/************************* Globals *******************************************************/

static MV_HWS_SERDES_FUNC_PTRS hwsSerdesFuncsPtr[HWS_MAX_DEVICE_NUM][SERDES_LAST];

/*******************************************************************************
* mvHwsSerdesPowerCtrl
*
* DESCRIPTION:
*       Init physical port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       powerUp   - true for power UP
*       baudRate  -
*       refClock  - ref clock value
*       refClockSource - ref cloack source (primary line or secondary)
*       media     - RXAUI or XAUI
*       mode      - 10BIT mode (enable/disable)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesPowerCtrl
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    GT_BOOL                 powerUp,
    MV_HWS_SERDES_SPEED     baudRate,
    MV_HWS_REF_CLOCK        refClock,
    MV_HWS_REF_CLOCK_SRC    refClockSource,
    MV_HWS_SERDES_MEDIA     media,
    MV_HWS_SERDES_10B_MODE  mode
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }

    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesPowerCntrlFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Power Control: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

	CHECK_STATUS(hwsSerdesFuncsPtr[devNum][serdesType].serdesPowerCntrlFunc(devNum,portGroup,serdesNum,powerUp,baudRate,refClock,refClockSource,media,mode));

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsSerdesManualRxConfig
*
* DESCRIPTION:
*       Per SERDES configure the RX parameters: squelch Threshold, FFE and DFE
*       operation
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
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
GT_STATUS mvHwsSerdesManualRxConfig
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    MV_HWS_PORT_MAN_TUNE_MODE portTuningMode,
    GT_U32              sqlch,
    GT_U32              ffeRes,
    GT_U32              ffeCap,
    GT_BOOL             dfeEn,
    GT_U32              alig
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesManualRxCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
#ifndef MV_HWS_REDUCED_BUILD
	if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
		return (mvHwsSerdesManualRxConfigIpc(devNum,portGroup,serdesNum,serdesType,portTuningMode,sqlch,ffeRes,ffeCap,dfeEn,alig));
#endif

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Manual Rx Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif
    return hwsSerdesFuncsPtr[devNum][serdesType].serdesManualRxCfgFunc(devNum,portGroup,serdesNum,portTuningMode,sqlch,ffeRes,ffeCap,dfeEn,alig);
}

/*******************************************************************************
* mvHwsSerdesManualTxConfig
*
* DESCRIPTION:
*       Per SERDES configure the TX parameters: amplitude, 3 TAP Tx FIR.
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
GT_STATUS mvHwsSerdesManualTxConfig
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_U32              txAmp,
    GT_BOOL             txAmpAdj,
    GT_U32              emph0,
    GT_U32              emph1,
    GT_BOOL             txAmpShft
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesManualTxCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }
#ifndef MV_HWS_REDUCED_BUILD
	if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
		return (mvHwsSerdesManualTxConfigIpc(devNum,portGroup,serdesNum,serdesType,txAmp,txAmpAdj,emph0,emph1,txAmpShft));
#endif

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Manual Tx Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif
    return hwsSerdesFuncsPtr[devNum][serdesType].serdesManualTxCfgFunc(devNum,portGroup,serdesNum,txAmp, txAmpAdj,emph0,emph1, txAmpShft);
}

/*******************************************************************************
* mvHwsSerdesManualTxConfigGet
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
*           txAmp       - Tx amplitude
*           txAmpAdj    - TX adjust
*           txemph0     - TX emphasis 0
*           txemph1     - TX emphasis 1
*           txAmpShft   - TX amplitude shift
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesManualTxConfigGet
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          serdesNum,
    MV_HWS_SERDES_TYPE              serdesType,
    MV_HWS_MAN_TUNE_TX_CONFIG_DATA  *configParams
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesManualTxCfgGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Manual Tx Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif
    return hwsSerdesFuncsPtr[devNum][serdesType].serdesManualTxCfgGetFunc(devNum, portGroup, serdesNum, configParams);
}

/*******************************************************************************
* mvHwsSerdesLoopback
*
* DESCRIPTION:
*       Perform an internal loopback (SERDES loopback) for debug/BIST use.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - type of serdes (LP/COM_PHY...)
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
GT_STATUS mvHwsSerdesLoopback
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    MV_HWS_SERDES_LB    lbMode
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesLoopbackCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Loopback Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif
    return hwsSerdesFuncsPtr[devNum][serdesType].serdesLoopbackCfgFunc(devNum,portGroup,serdesNum,lbMode);
}


/*******************************************************************************
* mvHwsSerdesLoopbackGet
*
* DESCRIPTION:
*       Get status of internal loopback (SERDES loopback).
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - type of serdes (LP/COM_PHY...)
*
* OUTPUTS:
*       lbModePtr - current loopback mode
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesLoopbackGet
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    MV_HWS_SERDES_LB    *lbModePtr
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesLoopbackGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Loopback Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif
    return hwsSerdesFuncsPtr[devNum][serdesType].serdesLoopbackGetFunc(devNum,portGroup,serdesNum,lbModePtr);
}

/*******************************************************************************
* mvHwsSerdesTestGen
*
* DESCRIPTION:
*       Activates the Serdes test generator/checker.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       txPattern - pattern to transmit
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
GT_STATUS mvHwsSerdesTestGen
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TX_PATTERN  txPattern,
    MV_HWS_SERDES_TYPE        serdesType,
    MV_HWS_SERDES_TEST_GEN_MODE mode
)
{

    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesTestGenFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Test Generator: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesTestGenFunc(devNum,portGroup,serdesNum,txPattern,mode);
}

/*******************************************************************************
* mvHwsSerdesTestGenGet
*
* DESCRIPTION:
*       Get configuration of the Serdes test generator/checker.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - type of SerDes
*
* OUTPUTS:
*       txPattern - pattern to transmit
*       mode      - test mode or normal
*
* RETURNS:
*       GT_OK  - on success
*       GT_NOT_SUPPORTED - on wrong serdes type
*       GT_FAIL - HW error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesTestGenGet
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TYPE        serdesType,
    MV_HWS_SERDES_TX_PATTERN  *txPatternPtr,
    MV_HWS_SERDES_TEST_GEN_MODE *modePtr
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesTestGenGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Get Test Generator: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesTestGenGetFunc(devNum, portGroup, serdesNum,
                                            txPatternPtr, modePtr);
}

/*******************************************************************************
* mvHwsSerdesTestGenStatus
*
* DESCRIPTION:
*       Read the tested pattern receive error counters and status.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       txPattern - pattern to transmit
*
* OUTPUTS:
*       Counters and status.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesTestGenStatus
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TYPE        serdesType,
    MV_HWS_SERDES_TX_PATTERN  txPattern,
    GT_BOOL                   counterAccumulateMode,
    MV_HWS_SERDES_TEST_GEN_STATUS *status
)
{

    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesTestGenStatusFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Test Generator Status: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesTestGenStatusFunc(devNum, portGroup, serdesNum, txPattern, counterAccumulateMode, status);
}

/*******************************************************************************
* mvHwsSerdesEOMGet
*
* DESCRIPTION:
*       Returns the horizontal/vertical Rx eye margin.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       timeout   - wait time in msec
*
* OUTPUTS:
*       horizontal/vertical Rx eye margin and EOM status.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesEOMGet
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TYPE        serdesType,
    GT_U32                    timeout,
    MV_HWS_SERDES_EOM_RES     *results
)
{
    if ((NULL == results) || (serdesType >= SERDES_LAST))
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesEomGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes EOM: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesEomGetFunc(devNum, portGroup, serdesNum, timeout, results);
}

/*******************************************************************************
* mvHwsSerdesPpmConfig
*
* DESCRIPTION:
*       Per SERDES add ability to add/sub PPM from main baud rate.
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
GT_STATUS mvHwsSerdesPpmConfig
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    MV_HWS_PPM_VALUE    ppmVal
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesPpmCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run PPM Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesPpmCfgFunc(devNum,portGroup,serdesNum,ppmVal);
}

/*******************************************************************************
* mvHwsSerdesPpmConfigGet
*
* DESCRIPTION:
*       Per SERDES read PPM value.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       ppmVal - PPM current value.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesPpmConfigGet
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    MV_HWS_PPM_VALUE    *ppmVal
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesPpmGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesPpmGetFunc(devNum,portGroup,serdesNum,ppmVal);
}
#endif

/*******************************************************************************
* mvHwsSerdesAutoTuneCfg
*
* DESCRIPTION:
*       Per SERDES configure parameters for TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
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
GT_STATUS mvHwsSerdesAutoTuneCfg
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_BOOL rxTraining,
    GT_BOOL txTraining
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Auto Tune Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneCfgFunc(devNum,portGroup,serdesNum,rxTraining,txTraining);
}

/*******************************************************************************
* mvHwsSerdesRxAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
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
GT_STATUS mvHwsSerdesRxAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_BOOL rxTraining
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesRxAutoTuneStartFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run RX Auto Tune Start: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesRxAutoTuneStartFunc(devNum,portGroup,serdesNum,rxTraining);
}

/*******************************************************************************
* mvHwsSerdesTxAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
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
GT_STATUS mvHwsSerdesTxAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_BOOL txTraining
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesTxAutoTuneStartFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run TX Auto Tune Start: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesTxAutoTuneStartFunc(devNum,portGroup,serdesNum,txTraining);
}

/*******************************************************************************
* mvHwsSerdesAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
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
GT_STATUS mvHwsSerdesAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_BOOL rxTraining,
    GT_BOOL txTraining
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneStartFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Auto Tune Start: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneStartFunc(devNum,portGroup,serdesNum,rxTraining,txTraining);
}


/*******************************************************************************
* mvHwsSerdesAutoTuneResult
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
GT_STATUS mvHwsSerdesAutoTuneResult
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    MV_HWS_AUTO_TUNE_RESULTS *results
)
{

    if ((results == NULL) || (serdesType >= SERDES_LAST) )
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneResultFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Auto Tune Result: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneResultFunc(devNum,portGroup,serdesNum,results);
}

/*******************************************************************************
* mvHwsSerdesAutoTuneStatus
*
* DESCRIPTION:
*       Per SERDES check the Rx & Tx training status.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       rxStatus - Rx Training status.
*       txStatus - Tx Training status.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesAutoTuneStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneStatusFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }


#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Auto Tune Status: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneStatusFunc(devNum,portGroup,serdesNum,rxStatus,txStatus);
}

/*******************************************************************************
* mvHwsSerdesReset
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
GT_STATUS mvHwsSerdesReset
(
    GT_U8      devNum,
    GT_U32     portGroup,
    GT_U32     serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    GT_BOOL    analogReset,
    GT_BOOL    digitalReset,
    GT_BOOL    syncEReset
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesResetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Reset: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesResetFunc(devNum,portGroup,serdesNum,analogReset,digitalReset,syncEReset);
}

/*******************************************************************************
* mvHwsSerdesResetExt
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
*       coreReset - Core Reset (On/Off)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesResetExt
(
    GT_U8      devNum,
    GT_U32     portGroup,
    GT_U32     serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    GT_BOOL    analogReset,
    GT_BOOL    digitalReset,
    GT_BOOL    syncEReset,
    GT_BOOL    coreReset
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesResetExtFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Reset Extended: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesResetExtFunc(devNum,portGroup,serdesNum,analogReset,digitalReset,syncEReset, coreReset);
}

/*******************************************************************************
* mvHwsSerdesDfeConfig
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
*
*******************************************************************************/
GT_STATUS mvHwsSerdesDfeConfig
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    MV_HWS_DFE_MODE   dfeMode,
    GT_32            *dfeCfg
)
{
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run DFE Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeCfgFunc(devNum,portGroup,serdesNum,dfeMode,(GT_REG_DATA *)dfeCfg);
}


/*******************************************************************************
* mvHwsSerdesDfeStatus
*
* DESCRIPTION:
*       Per SERDES get the DFE status and parameters.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       dfeLock   - Locked/Not locked
*       dfeCfg    - array of dfe configuration parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesDfeStatus
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    GT_BOOL           *dfeLock,
    GT_32             *dfeCfg
)
{

    if ((dfeCfg == NULL) || (dfeLock == NULL) || (serdesType >= SERDES_LAST))
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeStatusFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run DFE Status: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeStatusFunc(devNum,portGroup,serdesNum,dfeLock,dfeCfg);
}

/*******************************************************************************
* mvHwsSerdesDfeStatusExt
*
* DESCRIPTION:
*       Per SERDES get the of DFE status and parameters (extended API).
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       dfeLock   - Locked/Not locked
*       dfeCfg    - array of dfe configuration parameters
*       f0Cfg     - array of f0 configuration parameters
*       savCfg    - array of sav configuration parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesDfeStatusExt
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    GT_BOOL           *dfeLock,
    GT_32             *dfeCfg,
    GT_32             *f0Cfg,
    GT_32             *savCfg
)
{

    if ((dfeCfg == NULL) || (dfeLock == NULL) || (serdesType >= SERDES_LAST))
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeStatusExtFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run DFE Status: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeStatusExtFunc(devNum,portGroup,serdesNum,dfeLock,
                                                                (GT_REG_DATA *)dfeCfg, (GT_REG_DATA *)f0Cfg, (GT_REG_DATA *)savCfg);
}

/*******************************************************************************
* mvHwsSerdesPolarityConfig
*
* DESCRIPTION:
*       Per SERDES invert the Tx or Rx.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       invertTx  - invert TX polarity (GT_TRUE - invert, GT_FALSE - don't)
*       invertRx  - invert RX polarity (GT_TRUE - invert, GT_FALSE - don't)
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesPolarityConfig
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    GT_BOOL           invertTx,
    GT_BOOL           invertRx
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesPolarityCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Polarity Config: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesPolarityCfgFunc(devNum,portGroup, serdesNum,
                                                    invertTx, invertRx);
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsSerdesPolarityConfigGet
*
* DESCRIPTION:
*       Per SERDES invert the Tx or Rx.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       invertTx  - invert TX polarity (GT_TRUE - invert, GT_FALSE - don't)
*       invertRx  - invert RX polarity (GT_TRUE - invert, GT_FALSE - don't)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesPolarityConfigGet
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    GT_BOOL           *invertTx,
    GT_BOOL           *invertRx
)
{
    if ((invertTx == NULL) || (invertRx == NULL) || (serdesType >= SERDES_LAST))
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesPolarityCfgGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Polarity Get: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesPolarityCfgGetFunc(devNum, portGroup, serdesNum,
                                            invertTx, invertRx);
}
#endif

/*******************************************************************************
* mvHwsSerdesDfeOpti
*
* DESCRIPTION:
*       Per SERDES find best FFE_R is the min value for scanned DFE range.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       ffeR   - best value
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesDfeOpti
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  serdesNum,
  MV_HWS_SERDES_TYPE serdesType,
  GT_U32  *ffeR
)
{
    if ((ffeR == NULL) || (serdesType >= SERDES_LAST))
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeOptiFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run DFE Optimal: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeOptiFunc(devNum, portGroup, serdesNum, (GT_UREG_DATA *)ffeR);
}

/*******************************************************************************
* mvHwsSerdesFfeConfig
*
* DESCRIPTION:
*       Per SERDES configure FFE.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       ffeRes    - FFE R value
*       ffeCap    - FFE C value
*       align     - sampler value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesFfeConfig
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    GT_U32  ffeRes,
    GT_U32  ffeCap,
    GT_U32  alig
)
{
    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesFfeCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run DFE Optimal: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesFfeCfgFunc(devNum, portGroup, serdesNum, ffeRes, ffeCap, alig);
}

/*******************************************************************************
* mvHwsSerdesCalibrationStatus
*
* DESCRIPTION:
*       Per SERDES return the calibration results.
*       Can be run after create port
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       results - the calibration results.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesCalibrationStatus
(
    GT_U8                       devNum,
    GT_U32                      portGroup,
    GT_U32                      serdesNum,
    MV_HWS_SERDES_TYPE          serdesType,
    MV_HWS_CALIBRATION_RESULTS  *results
)
{
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesCalibrationStatusFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Calibration Status: serdes %s.\n",
        hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesCalibrationStatusFunc(devNum, portGroup, serdesNum, results);
}

/*******************************************************************************
* mvHwsSerdesTxEnable
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
*       results - the calibration results.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesTxEnable
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_BOOL             enable
)
{
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesTxEnableFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Tx Disable: serdes %s.\n",
        hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesTxEnableFunc(devNum, portGroup, serdesNum, enable);
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsSerdesTxEnableGet
*
* DESCRIPTION:
*        Enable / Disable transmission of packets in SERDES layer of a port.
*        Use this API to disable Tx for loopback ports.
*
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
*
* OUTPUTS:
*       enablePtr       - GT_TRUE  - Enable transmission of packets in
*                                    SERDES layer of a port
*                       - GT_FALSE - Disable transmission of packets in
*                                    SERDES layer of a port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, serdesNum
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*
* COMMENTS:
*       Disabling transmission of packets in SERDES layer of a port causes
*       to link down of devices that are connected to the port.
*
*******************************************************************************/
GT_STATUS mvHwsSerdesTxEnableGet
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_SERDES_TYPE serdesType,
    GT_BOOL *enable
)
{
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }

    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesTxEnableGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesTxEnableGetFunc(devNum, portGroup, serdesNum, enable);
}

/*******************************************************************************
* hwsSerdesSeqGet
*
* DESCRIPTION:
*       Get SERDES sequence one line.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsSerdesSeqGet
(
    GT_U8   devNum,
    GT_U32  portGroup,
    MV_HWS_COM_PHY_H_SUB_SEQ seqType,
    MV_CFG_ELEMENT *seqLine,
    GT_U32 lineNum
)
{
    MV_HWS_SERDES_TYPE serdesType;
    devNum = devNum;
    portGroup = portGroup;

    serdesType = (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum));
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesSeqGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesSeqGetFunc(seqType, seqLine, lineNum);
}

/*******************************************************************************
* hwsSerdesSeqSet
*
* DESCRIPTION:
*       Set SERDES sequence one line.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsSerdesSeqSet
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_BOOL firstLine,
    MV_HWS_COM_PHY_H_SUB_SEQ seqType,
    MV_CFG_ELEMENT *seqLine,
    GT_U32 numOfOp
)
{
    MV_HWS_SERDES_TYPE serdesType;
    devNum = devNum;
    portGroup = portGroup;

    serdesType = (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum));
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesSeqSetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesSeqSetFunc(firstLine, seqType, seqLine, numOfOp);
}
#endif

/*******************************************************************************
* mvHwsSerdesTxIfSelect
*
* DESCRIPTION:
*       tx interface select.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
*       enable    - whether to enable or disable Tx.
*
* OUTPUTS:
*       results - the calibration results.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesTxIfSelect
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_U32              serdesTxIfNum
)
{
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesTxIfSelectFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesTxIfSelectFunc(devNum, portGroup, serdesNum, serdesTxIfNum);
}

/*******************************************************************************
* mvHwsSerdesTxIfSelectGet
*
* DESCRIPTION:
*       Return the SERDES Tx interface select number.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - physical serdes number
*       serdesType  - SERDES types
*
* OUTPUTS:
*       serdesTxIfNum - interface select number
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesTxIfSelectGet
(
	GT_U8               devNum,
	GT_U32              portGroup,
	GT_U32              serdesNum,
	MV_HWS_SERDES_TYPE  serdesType,
	GT_U32              *serdesTxIfNum
)
{
	GT_STATUS rc;

	if (serdesType >= SERDES_LAST)
	{
		return GT_BAD_PARAM;
	}
	if (hwsSerdesFuncsPtr[devNum][serdesType].serdesTxIfSelectGetFunc == NULL)
	{
		return GT_NOT_IMPLEMENTED;
	}

	rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesTxIfSelectGetFunc(devNum, portGroup, serdesNum, serdesTxIfNum);
	return rc;
}

/*******************************************************************************
* mvHwsSerdesSqlchCfg
*
* DESCRIPTION:
*       Configure squelch threshold value.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
*       sqlchVal  - squelch threshold value.
*
* OUTPUTS:
*       results - the calibration results.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesSqlchCfg
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_U32              sqlchVal
)
{
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesSqlchCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesSqlchCfgFunc(devNum, portGroup, serdesNum, sqlchVal);
}

/*******************************************************************************
* mvHwsSerdesAutoTuneStatusShort
*
* DESCRIPTION:
*       Per SERDES check Rx or Tx training status
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
*
* OUTPUTS:
*       status - Training status (true/false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesAutoTuneStatusShort
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneStatusShortFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneStatusShortFunc(devNum, portGroup, serdesNum, rxStatus, txStatus);
}

/*******************************************************************************
* mvHwsSerdesTxAutoTuneStop
*
* DESCRIPTION:
*       Per SERDES stop the TX training & Rx Training process
*       Can be run after create port and training start.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesTxAutoTuneStop
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_SERDES_TYPE  serdesType
)
{
    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneStopFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return hwsSerdesFuncsPtr[devNum][serdesType].serdesAutoTuneStopFunc(devNum, portGroup, serdesNum);
}

/*******************************************************************************
* mvHwsSerdesFixAlign90Start
*
* DESCRIPTION:
*       Start fix Align90 process on current SERDES.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
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
GT_STATUS mvHwsSerdesFixAlign90Start
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    MV_HWS_ALIGN90_PARAMS   *params
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesFixAlign90Start == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesFixAlign90Start(devNum, portGroup, serdesNum, params);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesFixAlign90Status
*
* DESCRIPTION:
*       Return fix Align90 process current status on current SERDES.
*       Can be run after create port and start Align90.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
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
GT_STATUS mvHwsSerdesFixAlign90Status
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    MV_HWS_AUTO_TUNE_STATUS *trainingStatus
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesFixAlign90Status == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesFixAlign90Status(devNum, portGroup, serdesNum, trainingStatus);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesFixAlign90Stop
*
* DESCRIPTION:
*       Stop fix Align90 process on current SERDES.
*       Can be run after create port and start Align90.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
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
GT_STATUS mvHwsSerdesFixAlign90Stop
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    MV_HWS_ALIGN90_PARAMS   *params,
    GT_BOOL                 fixAlignPass
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesFixAlign90Stop == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesFixAlign90Stop(devNum, portGroup, serdesNum, params, fixAlignPass);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesRev2DfeCheck
*
* DESCRIPTION:
*       Check DFE values range and start sampler calibration, if needed.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesRev2DfeCheck
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeCheckFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesDfeCheckFunc(devNum, portGroup, serdesNum);
    return rc;
}

/*******************************************************************************
* mvHwsComSerdesSpeedGet
*
* DESCRIPTION:
*       Return SERDES baud rate.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
*
* OUTPUTS:
*       baudRate - baud rate.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesSpeedGet
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    MV_HWS_SERDES_SPEED *rate
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesSpeedGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesSpeedGetFunc(devNum, portGroup, serdesNum, rate);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesManualRxConfigGet
*
* DESCRIPTION:
*       Return SERDES RX parameters configured manually .
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - serdes type
*
* OUTPUTS:
*       sqlchPtr - Squelch threshold.
*       ffeResPtr - FFE R
*       ffeCapPtr - FFE C
*       aligPtr   - align 90 value
*
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesManualRxConfigGet
(
    IN GT_U8               devNum,
    IN GT_U32              portGroup,
    IN GT_U32              serdesNum,
    IN MV_HWS_SERDES_TYPE  serdesType,
    OUT MV_HWS_PORT_MAN_TUNE_MODE *portTuningModePtr,
    OUT GT_U32             *sqlchPtr,
    OUT GT_U32             *ffeResPtr,
    OUT GT_U32             *ffeCapPtr,
    OUT GT_BOOL			   *dfeEnPtr,
    OUT GT_U32             *aligPtr
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesManualRxCfgGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesManualRxCfgGetFunc(devNum, portGroup, serdesNum, portTuningModePtr,
																sqlchPtr, ffeResPtr, ffeCapPtr, dfeEnPtr, aligPtr);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesArrayPowerCtrl
*
* DESCRIPTION:
*       Init physical port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesType - serdes type
*       numOfSer  - number of SERDESes to configure
*       serdesArr - collection of SERDESes to configure
*       powerUp   - true for power UP
*       baudRate  -
*       refClock  - ref clock value
*       refClockSource - ref clock source (primary line or secondary)
*       media     - RXAUI or XAUI
*       mode      - 10BIT mode (enable/disable)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesArrayPowerCtrl
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    IN MV_HWS_SERDES_TYPE   serdesType,
    GT_U32                  numOfSer,
    GT_U32                  *serdesArr,
    GT_BOOL                 powerUp,
    MV_HWS_SERDES_SPEED     baudRate,
    MV_HWS_REF_CLOCK        refClock,
    MV_HWS_REF_CLOCK_SRC    refClockSource,
    MV_HWS_SERDES_MEDIA     media,
    MV_HWS_SERDES_10B_MODE  mode
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
      return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesArrayPowerCntrlFunc == NULL)
    {
		return GT_NOT_IMPLEMENTED;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Serdes Power Control: serdes %s.\n",
      hwsSerdesFuncsPtr[devNum][serdesType].serdesTypeGetFunc());
    osFclose(fdHws);
#endif
    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesArrayPowerCntrlFunc(devNum,portGroup,numOfSer,(GT_UOPT *)serdesArr,powerUp,baudRate,refClock,refClockSource,media,mode);

    return rc;
}

/*******************************************************************************
* mvHwsSerdesAcTerminationCfg
*
* DESCRIPTION:
*       Configures AC termination on current serdes.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - serdes number
*       acTermEn   - enable or disable AC termination
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesAcTerminationCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    GT_BOOL                 acTermEn
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesAcTermCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesAcTermCfgFunc(devNum,portGroup,serdesNum,acTermEn);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesDigitalReset
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
GT_STATUS mvHwsSerdesDigitalReset
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    MV_HWS_RESET        digitalReset
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesDigitalReset == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesDigitalReset(devNum,portGroup,serdesNum,digitalReset);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesSqDetectCfg
*
* DESCRIPTION:
*       Serdes sq_detect mask configuration.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       sqDetect  - true to enable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesSqDetectCfg
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_TYPE  serdesType,
    GT_BOOL             sqDetect
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesSqDetectCfgFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesSqDetectCfgFunc(devNum,portGroup,serdesNum,sqDetect);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesSignalDetectGet
*
* DESCRIPTION:
*       Return Signal Detect state on PCS (true/false).
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - physical PCS number
*       serdesType - serdes PCS type
*       signalDet  - if true, signal was detected
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesSignalDetectGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    GT_BOOL                 *signalDet
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesSignalDetectGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesSignalDetectGetFunc(devNum,portGroup,serdesNum,signalDet);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesCdrLockStatusGet
*
* DESCRIPTION:
*       Return SERDES CDR lock status (true - locked /false - notlocked).
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - physical PCS number
*       serdesType - serdes PCS type
*       cdrLock    - if true, CDR was locked
*
* OUTPUTS:
*       cdrLock  - if true, CDR was locked
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesCdrLockStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_SERDES_TYPE      serdesType,
    GT_BOOL                 *cdrLock
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesCdrLockStatusGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesCdrLockStatusGetFunc(devNum,portGroup,serdesNum,cdrLock);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesScanSampler
*
* DESCRIPTION:
*       Scan SERDES sampler.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - physical PCS number
*       serdesType - serdes PCS type
*
* OUTPUTS:
*       bestValue  - sampler value
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesScanSampler
(
	GT_U8                  devNum,
	GT_U8                  portGroup,
	GT_U32                 serdesNum,
	MV_HWS_SERDES_TYPE     serdesType,
	GT_U32                 waitTime,
	GT_U32                 samplerSelect,
	GT_U32                 errorThreshold,
    MV_HWS_SAMPLER_RESULTS *samplerResults
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesScanSamplerFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesScanSamplerFunc(devNum, portGroup, serdesNum, waitTime, samplerSelect, errorThreshold, samplerResults);
    return rc;
}

#ifndef VIPS_BUILD
/*******************************************************************************
* mvHwsSerdesDigitalRfResetToggleSet
*
* DESCRIPTION:
*       Run digital RF Reset/Unreset on current SERDES.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       waitTime    - wait time between Reset/Unreset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on error
********************************************************************************/
GT_STATUS mvHwsSerdesDigitalRfResetToggleSet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_U8                   waitTime
)
{
    GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U8   i;

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
    {
        return GT_BAD_PARAM;
    }

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        CHECK_STATUS(mvHwsSerdesDigitalReset(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                             (curLanesList[i] & 0xFFFF), (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)), RESET));
    }

    hwsOsTimerWkFuncPtr(waitTime);

    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        CHECK_STATUS(mvHwsSerdesDigitalReset(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                             (curLanesList[i] & 0xFFFF), (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)), UNRESET));
    }

    return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsSerdesPartialPowerDown
*
* DESCRIPTION:
*       Enable/Disable power down of Tx and Rx on Serdes.
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
GT_STATUS mvHwsSerdesPartialPowerDown
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    MV_HWS_SERDES_TYPE     serdesType,
    GT_BOOL     powerDownRx,
    GT_BOOL     powerDownTx
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesPartialPowerDownFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesPartialPowerDownFunc(devNum, portGroup, serdesNum, powerDownRx, powerDownTx);
    return rc;
}

/*******************************************************************************
* mvHwsSerdesPartialPowerStatusGet
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
GT_STATUS mvHwsSerdesPartialPowerStatusGet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    MV_HWS_SERDES_TYPE     serdesType,
    GT_BOOL 	*powerRxStatus,
    GT_BOOL 	*powerTxStatus
)
{
    GT_STATUS rc;

    if (serdesType >= SERDES_LAST)
    {
        return GT_BAD_PARAM;
    }
    if (hwsSerdesFuncsPtr[devNum][serdesType].serdesPartialPowerStatusGetFunc == NULL)
    {
        return GT_NOT_IMPLEMENTED;
    }

    rc = hwsSerdesFuncsPtr[devNum][serdesType].serdesPartialPowerStatusGetFunc(devNum, portGroup, serdesNum, powerRxStatus, powerTxStatus);
    return rc;
}

/*******************************************************************************
* hwsSerdesGetFuncPtr
*
* DESCRIPTION:
*       Get function structure pointer.
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
GT_STATUS hwsSerdesGetFuncPtr
(
	GT_U8						devNum,
	MV_HWS_SERDES_FUNC_PTRS		**hwsFuncsPtr
)
{
	if (devNum >= HWS_MAX_DEVICE_NUM)
	{
		return GT_NOT_SUPPORTED;
	}
	
    *hwsFuncsPtr = &hwsSerdesFuncsPtr[devNum][0];

	return GT_OK;
}

