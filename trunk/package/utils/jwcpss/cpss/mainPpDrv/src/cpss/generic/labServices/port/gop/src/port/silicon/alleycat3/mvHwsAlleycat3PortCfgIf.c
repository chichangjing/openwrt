/*******************************************************************************
*       	 Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsPortCfgIf.c
*
* DESCRIPTION:
*           This file contains API for port configuartion and tuning parameters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 48 $
******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <mvHwsPortInitIf.h>
#include <private/mvHwsPortPrvIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <private/mvPortModeElements.h>
#include <silicon/alleycat3/mvHwsAlleycat3PortIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <mac/mvHwsMacIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmIf.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3If.h>

/*******************************************************************************
* mvHwsAlleycat3PortFixAlign90Ext
*
* DESCRIPTION:
*       Fix Align90 parameters
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortFixAlign90Ext
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode
)
{
	switch (portMode)
	{
	case _10GBase_KR:
	case _20GBase_KR:
	case _10GBase_SR_LR:
	case _20GBase_SR_LR:
	case _12GBaseR:
	case _12_5GBase_KR:  /* XLHGL_KR */
		CHECK_STATUS(mvHwsAlleycat3PortFineTune(devNum, portGroup, phyPortNum, portMode, GT_TRUE));
		break;
	default:
		break;
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsAlleycat3PortRxAutoTuneSetExt
*
* DESCRIPTION:
*       Sets the port Rx parameters according to different working
*       modes/topologies.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       optAlgoMask - bit mask for optimization algorithms
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortRxAutoTuneSetExt
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD        portMode,
	MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode,
	GT_U32  optAlgoMask,
	void *  results
)
{
	MV_HWS_AUTO_TUNE_RESULTS res[HWS_MAX_SERDES_NUM];
	MV_HWS_PORT_INIT_PARAMS *curPortParams;
	MV_HWS_AUTO_TUNE_STATUS rxStatus[HWS_MAX_SERDES_NUM];
	MV_HWS_AUTO_TUNE_STATUS txStatus[HWS_MAX_SERDES_NUM];
	GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
	GT_BOOL autoTunePass[HWS_MAX_SERDES_NUM];
	GT_BOOL laneFail = GT_FALSE;
	GT_U32  sqleuch, ffeR, ffeC, align90;
	GT_U32  txAmp, txEmph0, txEmph1;
	GT_U32  i;
	GT_U32 numOfActLanes;

	/* avoid warnings */
	portTuningMode = portTuningMode;
	optAlgoMask = optAlgoMask;

	(void)results; /* avoid warnings */

	if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) |
		((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
	{
		return GT_BAD_PARAM;
	}

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
	hwsOsMemSetFuncPtr(autoTunePass, 0, sizeof(autoTunePass));
	hwsOsMemSetFuncPtr(res, 0, sizeof(res));
	hwsOsMemSetFuncPtr(rxStatus, 0, sizeof(rxStatus));
	hwsOsMemSetFuncPtr(txStatus, 0, sizeof(txStatus));

	/* rebuild active lanes list according to current configuration (redundancy) */
	CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

	numOfActLanes = curPortParams->numOfActLanes;

	/* on each related serdes */
	for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < numOfActLanes); i++)
	{
		if (autoTunePass[i] == GT_FALSE)
		{
			CHECK_STATUS(mvHwsSerdesRxAutoTuneStart(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
										(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), GT_TRUE));
		}
	}

	/* check status and start auto tune again if not passed */
	for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < numOfActLanes); i++)
	{
		if (autoTunePass[i] == GT_FALSE)
		{
			mvHwsSerdesAutoTuneStatus(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
								(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &rxStatus[i], NULL);
			if (rxStatus[i] == TUNE_PASS)
			{
				autoTunePass[i] = GT_TRUE;
			}
			else
			{
				laneFail = GT_TRUE;
			}
		}
	}

	hwsOsTimerWkFuncPtr(1);

	if (laneFail == GT_TRUE)
	{
		return GT_FAIL;
	}

	 /* get results */
	 sqleuch = ffeR = ffeC = align90 = 0;
	 txAmp = txEmph0 = txEmph1 = 0;
	 for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < numOfActLanes); i++)
	{
		CHECK_STATUS(mvHwsSerdesAutoTuneResult(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
								(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &res[i]));
		sqleuch += res[i].sqleuch;
		ffeR += res[i].ffeR;
		ffeC += res[i].ffeC;
		txAmp += res[i].txAmp;
		txEmph0 += res[i].txEmph0;
		txEmph1 += res[i].txEmph1;
	 }

	 /* disable training on each related serdes */
	 for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < numOfActLanes); i++)
	{
		 CHECK_STATUS(mvHwsSerdesRxAutoTuneStart(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
								(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), GT_FALSE));
	 }

	 CHECK_STATUS(mvHwsAlleycat3PortFixAlign90Ext(devNum, portGroup, phyPortNum, portMode));

	/* unreset PCS Rx only */
	hwsOsTimerWkFuncPtr(1);

	return GT_OK;
}

/*******************************************************************************
* mvHwsAlleycat3PortTxAutoTuneSetExt
*
* DESCRIPTION:
*       Sets the port Tx only parameters according to different working
*       modes/topologies.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port TX related tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortTxAutoTuneSetExt
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode,
	void    *results
)
{
	MV_HWS_AUTO_TUNE_STATUS_RES *tuneRes;

	if (portTuningMode <= TRxTuneStatus)
	{
		CHECK_STATUS(hwsPortTxAutoTuneStartSet(devNum, portGroup, phyPortNum, portMode, portTuningMode, 0/*optAlgoMask*/));
	}

	if (portTuningMode == TRxTuneStatus)
	{
		CHECK_STATUS(mvHwsAlleycat3PortFixAlign90Ext(devNum, portGroup, phyPortNum, portMode));

		/* during Tx config both MAC and PCS are in Reset. Status part only Un-Reset the PCS
		   so MAC needs to be Un-Reset as well */
		CHECK_STATUS(hwsPortTxAutoTuneActivateSet(devNum, portGroup, phyPortNum, portMode, portTuningMode));
	}
	else if (portTuningMode == TRxTuneStatusNonBlocking)
	{
		if (results == NULL)
		{
			osPrintf("Error: results cannot be NULL when using TRxTuneStatusNonBlocking param\n");
			return GT_BAD_PARAM;
		}
		tuneRes = (MV_HWS_AUTO_TUNE_STATUS_RES*)results;

		CHECK_STATUS(mvHwsPortAutoTuneStateCheck(devNum, portGroup, phyPortNum, portMode,
												 &tuneRes->rxTune, &tuneRes->txTune));
	}
	else if (portTuningMode == TRxTuneStop)
	{
		/* Fix align90 wll be executed during AP stop )and after that the MAC will be un-reset */
		CHECK_STATUS(mvHwsPortAutoTuneStop(devNum, portGroup, phyPortNum, portMode, GT_FALSE /*Rx*/, GT_TRUE /*Tx*/));
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsAlleycat3PortAutoTuneSetExt
*
* DESCRIPTION:
*       Sets the port Tx and Rx parameters according to different working
*       modes/topologies.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       optAlgoMask - bit mask for optimization algorithms
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortAutoTuneSetExt
(
    GT_U8    devNum,
    GT_U32   portGroup,
    GT_U32   phyPortNum,
    MV_HWS_PORT_STANDARD            portMode,
    MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode,
    GT_U32   optAlgoMask,
    void     *results
)
{
    /* avoid warnings */
    optAlgoMask = optAlgoMask;

    if (portTuningMode == RxTrainingOnly)
    {
        return mvHwsAlleycat3PortRxAutoTuneSetExt(devNum, portGroup, phyPortNum, portMode, portMode, portTuningMode, results);
    }
    else
    {
        return mvHwsAlleycat3PortTxAutoTuneSetExt(devNum, portGroup, phyPortNum, portMode, portTuningMode, results);
    }
}

/*******************************************************************************
* mvHwsAlleycat3PortFineTune
*
* DESCRIPTION:
*       - Set the PPM compensation for Rx/TRX training in 10G and above.
*         Relevant to GOP_28NM_REV1 and above.
*       - Set FixAlign90 for Serdes 28nmRev3 process on current SERDES
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       configPpm  - enable/disable the PPM configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortFineTune
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL configPpm
)
{
	MV_HWS_PORT_INIT_PARAMS *curPortParams;
	GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
	GT_U32 i;

	if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
		((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
	{
		return GT_BAD_PARAM;
	}

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

	/* rebuild active lanes list according to current configuration (redundancy) */
	CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

	/* on each related serdes */
	for (i = 0; i < curPortParams->numOfActLanes; i++)
	{
		if (curPortParams->serdesSpeed >= _10_3125G)
		{
			/* Set the PPM compensation on Serdes after Rx/TRX training in 10G and above */
			CHECK_STATUS(mvHwsComH28nmSerdesAdaptPpm(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
										(curLanesList[i] & 0xFFFF), configPpm));
		}

		 /* Run fix Align90 process on SERDES */
		 CHECK_STATUS(mvHwsComH28nmRev3SerdesFixAlign90(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
										(curLanesList[i] & 0xFFFF)));
	}

	return GT_OK;
}

#if 0
/*******************************************************************************
* mvHwsAlleycat3PortBetterAdaptationSet
*
* DESCRIPTION:
*       Run the better Align90 adaptation algorithm on specific port number.
*       The function returns indication if the Better adaptation algorithm
*       succeeded to run on port or not.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - serdes number
*       portMode   - port standard metric
*
* OUTPUTS:
*       betterAlign90 - indicates if the Better adaptation algorithm run on
*       		specific port or not
*
* RETURNS:  GT_OK   - on success
*           GT_FAIL - on error
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortBetterAdaptationSet
(
    GT_U8   	devNum,
    GT_U32  	portGroup,
    GT_U32  	serdesNum,
    GT_BOOL 	*betterAlign90
)
{
    CHECK_STATUS(mvHwsComH28nmRev3FindBetterAdaptation(devNum, portGroup, serdesNum, 300, 10, betterAlign90));

    return GT_OK;
}
#endif

/*******************************************************************************
* hwsPortAlleycat3ExtendedModeCfg
*
* DESCRIPTION:
*       Enable / disable extended mode on port specified.
*       Extended ports supported only in Lion2 and Alleycat3 devices.
*       For Lion2:      1G, 10GBase-R, 20GBase-R2, RXAUI - can be normal or extended
*       				XAUI, DXAUI, 40GBase-R - only extended
*       	For Alleycat3:  ports 25 and 27 can be 10GBase_KR, 10GBase_SR_LR - normal or extended modes
*       					port 27 can be 20GBase_KR, 20GBase_SR_LR - only in extended mode
*
* INPUTS:
*       devNum       - system device number
*       portGroup    - port group (core) number
*       phyPortNum   - physical port number
*       portMode     - port standard metric
*       extendedMode - enable / disable
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortExtendedModeCfg
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL extendedMode
)
{
	GT_U32 offset;

	portGroup = portGroup;

	if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
		((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
	{
		return GT_BAD_PARAM;
	}

	/* for Alleycat3A0: extended ports 25 or 27
	In Alleycat3 there is a Mux for DMA unit between regular and extended Stacking ports 25,27-29 in MSMS4 unit
	The selector (port RX interface) for changing the configuration exists
	in XLGMAC unit with MAC number 24 in External_control register:
	- External_Control_0 (bit 0) - select between regular port 25 (value=0, internal MAC) and extended port 28 (value=1, external client 1)
	- External_Control_1 (bit 1) - select between regular port 27 (value=0, internal MAC) and extended port 29 (value=1, external client 3) */
	if (phyPortNum != 25 && phyPortNum != 27)
	{
		return GT_OK;
	}

	offset = (phyPortNum == 25) ? 0 : 1;
	CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, 24, EXTERNAL_CONTROL, (extendedMode << offset), (1 << offset)));

	return GT_OK;
}

/*******************************************************************************
* mvHwsAlleycat3PortExtendedModeCfgGet
*
* DESCRIPTION:
*       Returns the extended mode status on port specified.
*
* INPUTS:
*       devNum       - system device number
*       portGroup    - port group (core) number
*       phyPortNum   - physical port number
*       portMode     - port standard metric
*
* OUTPUTS:
*       extendedMode - enable / disable
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortExtendedModeCfgGet
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL *extendedMode
)
{
	GT_UREG_DATA data;
	GT_U32 offset;

	if ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum))
	{
		return GT_BAD_PARAM;
	}

	if (phyPortNum != 25 && phyPortNum != 27)
	{
		*extendedMode = GT_FALSE;
		return GT_OK;
	}

	CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XLGMAC_UNIT, 24, EXTERNAL_CONTROL, &data, 0));

	/* In Alleycat3:
	- for port#25 - if bit 0 in External_Control reg is 0x1, the port is in Extended mode
	- for port#27 - if bit 1 in External_Control reg is 0x1, the port is in Extended mode */
	offset = (phyPortNum == 25) ? 0 : 1;
	*extendedMode = ((data >> offset) & 0x1) ? GT_TRUE : GT_FALSE;

	return GT_OK;
}

/*******************************************************************************
* mvHwsAlleycat3PortPsyncBypassCfg
*
* DESCRIPTION:
*       Configures the bypass of synchronization module.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAlleycat3PortPsyncBypassCfg
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode
)
{
	MV_HWS_PORT_INIT_PARAMS   *curPortParams;
	GT_UOPT macNum;

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
	if (curPortParams == NULL)
	{
		return GT_NOT_SUPPORTED;
	}

	macNum = curPortParams->portMacNumber;

	/* for Network ports */
	if (phyPortNum <= 23)
	{
		if (portMode == SGMII2_5)
		{
			/* set P Sync Bypass */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, macNum,
								GIG_PORT_MAC_CONTROL_REGISTER4, (1 << 6), (1 << 6)));
			return GT_OK;
		}
		else
		{
			/* unset P Sync Bypass */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, macNum,
								GIG_PORT_MAC_CONTROL_REGISTER4, 0, (1 << 6)));
			return GT_OK;
		}
	}

	/* for Stacking ports */
	if ((phyPortNum >= 24) && (phyPortNum <= 27))
	{
		/* set P Sync Bypass */
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, macNum,
							GIG_PORT_MAC_CONTROL_REGISTER4, (1 << 6), (1 << 6)));
		return GT_OK;
	}

	/* for CPU port */
	if (phyPortNum == 31)
	{
		/* unset P Sync Bypass */
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, macNum,
							GIG_PORT_MAC_CONTROL_REGISTER4, 0, (1 << 6)));
		return GT_OK;
	}

	/* unset P Sync Bypass */
	CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, curPortParams->portMacNumber,
						GIG_PORT_MAC_CONTROL_REGISTER4, 0, (1 << 6)));

	return GT_OK;
}


