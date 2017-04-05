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
#include <silicon/bobcat2/mvHwsBobcat2PortIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <mac/mvHwsMacIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmIf.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3If.h>

/*******************************************************************************
* mvHwsBobcat2PCSMarkModeSet
*
* DESCRIPTION:
*       Mark/Un-mark PCS unit
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       enable      - GT_TRUE  for mark the PCS,
*       	      GT_FALSE for un-mark the PCS
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   	- on success
*       GT_BAD_PARAM    - on wrong parameter
*******************************************************************************/
GT_STATUS mvHwsBobcat2PCSMarkModeSet
(
	GT_U8   devNum,
	GT_UOPT portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL enable
)
{
	MV_HWS_PORT_INIT_PARAMS *curPortParams;

	if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
		((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
	{
		return GT_BAD_PARAM;
	}

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

	switch (portMode)
	{
        case _10GBase_KR:
        case _40GBase_KR:
        case _10GBase_SR_LR:
        case _40GBase_SR_LR:
        case _12GBaseR:
        case _48GBaseR:
        case _12GBase_SR:
        case _48GBase_SR:
            if (HWS_DEV_IS_BOBCAT2_A0(devNum))
            {
                CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, curPortParams->portPcsNumber,
                                                FEC_DEC_DMA_WR_DATA, enable, 1));
            }
            break;
        default:
            osPrintf("mvHwsPCSMarkModeSet: portMode %d is not supported for Mark/Un-mark PCS unit\n", portMode);
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsBobcat2PortFixAlign90Ext
*
* DESCRIPTION:
*       Fix Align90 parameters
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       optAlgoMask - bit mask for optimization algorithms
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsBobcat2PortFixAlign90Ext
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
	case _40GBase_KR:
	case _100GBase_KR10:
	case _10GBase_SR_LR:
	case _20GBase_SR_LR:
	case _40GBase_SR_LR:
    case _12GBaseR:
    case _48GBaseR:
    case _12GBase_SR:
    case _48GBase_SR:
	case _12_5GBase_KR:  /* XLHGL_KR */
	case XLHGL_KR4:
	case INTLKN_4Lanes_3_125G:
	case INTLKN_8Lanes_3_125G:
	case INTLKN_4Lanes_6_25G:
	case INTLKN_8Lanes_6_25G:
		if (HWS_DEV_IS_BOBCAT2_A0(devNum))
		{
			CHECK_STATUS(mvHwsPortFixAlign90(devNum, portGroup, phyPortNum, portMode, 0));
		}
		CHECK_STATUS(mvHwsBobcat2PortFineTune(devNum, portGroup, phyPortNum, portMode, GT_TRUE));
		break;
	default:
		break;
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsBobcat2PortRxAutoTuneSetExt
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
GT_STATUS mvHwsBobcat2PortRxAutoTuneSetExt
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
	GT_U32 unitAddr, regAddr;
	GT_U32 unitIndex;
	GT_U32 numOfActLanes;

	/* avoid warnings */
	portTuningMode = portTuningMode;
	optAlgoMask = optAlgoMask;

	(void)results; /* avoid warnings */

	if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
		((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
	{
		return GT_BAD_PARAM;
	}

	/* for RX Training Only */
	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
	hwsOsMemSetFuncPtr(autoTunePass, 0, sizeof(autoTunePass));
	hwsOsMemSetFuncPtr(res, 0, sizeof(res));
	hwsOsMemSetFuncPtr(rxStatus, 0, sizeof(rxStatus));
	hwsOsMemSetFuncPtr(txStatus, 0, sizeof(txStatus));

	/* rebuild active lanes list according to current configuration (redundancy) */
	CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

	/* mark PCS to be under Rx training for Puma3B0 and BobcatA0 only */
	if (HWS_DEV_IS_BOBCAT2_A0(devNum))
	{
		switch (portMode)
		{
		case _40GBase_KR:
		case _40GBase_SR_LR:
            mvUnitExtInfoGet(devNum, MMPCS_UNIT, curPortParams->portPcsNumber, &unitAddr, &unitIndex,NULL);
			regAddr = FEC_DEC_DMA_WR_DATA + unitAddr + (curPortParams->portPcsNumber * unitIndex);
			CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 1, 1));
			break;
		default:
			break;
		}
	}

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
			mvHwsSerdesAutoTuneStatus(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)), (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &rxStatus[i], NULL);
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
		CHECK_STATUS(mvHwsSerdesRxAutoTuneStart(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)), (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), GT_FALSE));
	}

	CHECK_STATUS(mvHwsBobcat2PortFixAlign90Ext(devNum, portGroup, phyPortNum, portMode));

	/* unreset PCS Rx only */
	hwsOsTimerWkFuncPtr(1);

	/* unmark PCS to be under Rx training for Puma3B0 and BobcatA0 only */
	if (HWS_DEV_IS_BOBCAT2_A0(devNum))
	{
		switch (portMode)
		{
		case _40GBase_KR:
		case _40GBase_SR_LR:

            mvUnitExtInfoGet(devNum, MMPCS_UNIT, (GT_U8)curPortParams->portPcsNumber, &unitAddr, &unitIndex,NULL);
			regAddr = FEC_DEC_DMA_WR_DATA + unitAddr + (curPortParams->portPcsNumber * unitIndex);
			CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 1));

			/* call to WA */
			mvHwsMmPcs28nm40GBackWa(devNum, portGroup, curPortParams->portPcsNumber);
			break;
		default:
			break;
		}
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsBobcat2PortTxAutoTuneSetExt
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
GT_STATUS mvHwsBobcat2PortTxAutoTuneSetExt
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
		CHECK_STATUS(mvHwsBobcat2PortFixAlign90Ext(devNum, portGroup, phyPortNum, portMode));

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
* mvHwsBobcat2PortAutoTuneSetExt
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
GT_STATUS mvHwsBobcat2PortAutoTuneSetExt
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

    if (portTuningMode == RxTrainingAdative || portTuningMode == RxTrainingStopAdative)
    {
        return GT_NOT_SUPPORTED;
    }

    if (portTuningMode == RxTrainingOnly)
    {
        return mvHwsBobcat2PortRxAutoTuneSetExt(devNum, portGroup, phyPortNum, portMode, portMode, portTuningMode, results);
    }
    else
    {
        return mvHwsBobcat2PortTxAutoTuneSetExt(devNum, portGroup, phyPortNum, portMode, portTuningMode, results);
    }
}

/*******************************************************************************
* mvHwsBobcat2PortFineTune
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
GT_STATUS mvHwsBobcat2PortFineTune
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

		 if (HWS_DEV_IS_BOBCAT2_B0(devNum))
		{
			 /* Run fix Align90 process on SERDES */
			 CHECK_STATUS(mvHwsComH28nmRev3SerdesFixAlign90(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
											(curLanesList[i] & 0xFFFF)));
		 }
	}

	return GT_OK;
}

#if 0
/*******************************************************************************
* mvHwsBobcat2PortBetterAdaptationSet
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
GT_STATUS mvHwsBobcat2PortBetterAdaptationSet
(
    GT_U8   	devNum,
    GT_U32  	portGroup,
    GT_U32  	serdesNum,
    GT_BOOL 	*betterAlign90
)
{
	if (HWS_DEV_IS_BOBCAT2_B0(devNum))
	{
		CHECK_STATUS(mvHwsComH28nmRev3FindBetterAdaptation(devNum, portGroup, serdesNum, 300, 10, betterAlign90));
	}

	return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsBobcat2PortExtendedModeCfg
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
GT_STATUS mvHwsBobcat2PortExtendedModeCfg
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL extendedMode
)
{
	/* avoid warnings */
	devNum		 = devNum;
	portGroup    = portGroup;
	phyPortNum   = phyPortNum;
	portMode     = portMode;
	extendedMode = extendedMode;

	return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* mvHwsBobcat2PortExtendedModeCfgGet
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
GT_STATUS mvHwsBobcat2PortExtendedModeCfgGet
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL *extendedMode
)
{
	/* avoid warnings */
	devNum		 = devNum;
	portGroup    = portGroup;
	phyPortNum   = phyPortNum;
	portMode     = portMode;
	extendedMode = extendedMode;

	return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* mvHwsBobcat2PortPsyncBypassCfg
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
GT_STATUS mvHwsBobcat2PortPsyncBypassCfg
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

	if (portMode == SGMII2_5)
	{
		/* set P Sync Bypass */
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, macNum,
							GIG_PORT_MAC_CONTROL_REGISTER4, (1 << 6), (1 << 6)));
		return GT_OK;
	}

	/* unset P Sync Bypass */
	CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, curPortParams->portMacNumber,
						GIG_PORT_MAC_CONTROL_REGISTER4, 0, (1 << 6)));

	return GT_OK;
}

