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
* mvHwsPortInitIf.c
*
* DESCRIPTION: Port extrenal interface
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 55 $
******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mvHwsPortInitIf.h>
#include <private/mvHwsPortPrvIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <mac/mvHwsMacIf.h>
#ifndef MV_HWS_REDUCED_BUILD
#include <mvHwsIpcApis.h>
#endif
GT_BOOL  multiLaneConfig = GT_TRUE;

/*****************************************************************************************/

GT_BOOL multiLaneConfigSupport(GT_BOOL enable)
{
	GT_BOOL oldState = multiLaneConfig;
	multiLaneConfig = enable;
	return oldState;
}

/*******************************************************************************
* mvHwsPortInit
*
* DESCRIPTION:
*       Init physical port. Configures the port mode and all it's elements
*       accordingly.
*       Does not verify that the selected mode/port number is valid at the
*       core level.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       lbPort     - if true, init port without serdes activity
*       refClock   - Reference clock frequency
*       refClockSrc - Reference clock source line
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortInit
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL lbPort,
	MV_HWS_REF_CLOCK_SUP_VAL	refClock,
	MV_HWS_REF_CLOCK_SOURCE refClockSource
)
{
	MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

#ifndef MV_HWS_REDUCED_BUILD
	if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
		return (mvHwsPortInitIpc(devNum,portGroup, phyPortNum,portMode, lbPort, refClock, refClockSource));
#endif
	hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

	if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].devPortinitFunc == NULL)
	{
		return GT_BAD_PTR;
	}

	return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].devPortinitFunc(devNum, portGroup, phyPortNum, portMode, lbPort, refClock, refClockSource);
}

/*******************************************************************************
* mvHwsPortReset
*
* DESCRIPTION:
*       Clears the port mode and release all its resources according to selected.
*       Does not verify that the selected mode/port number is valid at the core
*       level and actual terminated mode.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       action    - Power down or reset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortReset
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode,
	MV_HWS_PORT_ACTION	action
)
{
	MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

#ifndef MV_HWS_REDUCED_BUILD
	if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
		return (mvHwsPortResetIpc(devNum,portGroup,phyPortNum,portMode,action));
#endif

	hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

	if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].devPortResetFunc == NULL)
	{
		return GT_BAD_PTR;
	}

	return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].devPortResetFunc(devNum, portGroup, phyPortNum, portMode, action);
}

/*******************************************************************************
* mvHwsPortFlavorInit
*
* DESCRIPTION:
*       Init physical port. Configures the port mode and all it's elements
*       accordingly to specified flavor.
*       Does not verify that the selected mode/port number is valid at the
*       core level.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       lbPort     - if true, init port without serdes activity
*       refClock   - Reference clock frequency
*       refClockSrc - Reference clock source line
*       portFlavor  - describe port create suite (with/without additional actions)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFlavorInit
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode,
	GT_BOOL	lbPort,
	MV_HWS_REF_CLOCK_SUP_VAL	refClock,
	MV_HWS_REF_CLOCK_SOURCE	refClockSource,
	MV_HWS_PORT_INIT_FLAVOR_CFG	portFlavorParams
)
{
    /* avoid warning */
    portFlavorParams = portFlavorParams;

    return mvHwsPortInit(devNum,portGroup,phyPortNum,portMode,lbPort,refClock,refClockSource);
}

/*******************************************************************************
* mvHwsPortValidate
*
* DESCRIPTION:
*       Validate port API's input parameters
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortValidate
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode
)
{
	MV_HWS_PORT_INIT_PARAMS *curPortParams;

	/* validation */
	if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
		(portMode >= (MV_HWS_PORT_STANDARD)HWS_DEV_PORT_MODES(devNum)))
	{
		return GT_BAD_PARAM;
	}

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
	if ((curPortParams == NULL) || (curPortParams->numOfActLanes == 0))
	{
		return GT_NOT_SUPPORTED;
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsPortLoopbackValidate
*
* DESCRIPTION:
*       Validate loopback port input parameters.
*       In MMPCS mode: the ref_clk comes from ExtPLL, thus the Serdes can be in power-down.
*       In all other PCS modes: there is no ExtPLL, thus the ref_clk comes is taken
*       from Serdes, so the Serdes should be in power-up.
*
* INPUTS:
*       portPcsType   - port pcs type
*       lbPort     - if true, init port without serdes activity
*       serdesInit     - if true, init port serdes
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLoopbackValidate
(
	GT_U32	portPcsType,
	GT_BOOL	lbPort,
	GT_BOOL	*serdesInit
)
{
    if (lbPort == GT_TRUE)
    {
        /* in MMPCS PCS mode for Loopback mode, the ref_clk comes from ExtPLL,
           so the Serdes can be in power-down */
        if ((MV_HWS_PORT_PCS_TYPE)portPcsType == MMPCS)
        {
            *serdesInit = GT_FALSE;
        }
        else /* for all other PCS modes in Loopback mode, there is no ExtPLL,
                so the Serdes should be in power-up */
        {
            *serdesInit = GT_TRUE;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsPortSerdesPowerUp
*
* DESCRIPTION:
*       power up the serdes lanes
*      assumes .parameters validation in the calling function
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       refClock   - Reference clock frequency
*       refClockSrc - Reference clock source line
*       curLanesList     - active Serdes lanes list according to configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortSerdesPowerUp
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	MV_HWS_REF_CLOCK_SUP_VAL	refClock,
	MV_HWS_REF_CLOCK_SOURCE refClockSource,
	GT_U32 *curLanesList
)
{
	GT_U32 i;
	MV_HWS_PORT_INIT_PARAMS *curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    if (curPortParams == NULL)
    {
        return GT_NOT_SUPPORTED;
    }

	/* power up the serdes */
	if (multiLaneConfig == GT_TRUE)
	{
		CHECK_STATUS(mvHwsSerdesArrayPowerCtrl(devNum, portGroup,
								(MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)),
								curPortParams->numOfActLanes,
								curLanesList, GT_TRUE, curPortParams->serdesSpeed,
								refClock, refClockSource, curPortParams->serdesMediaType,
								curPortParams->serdes10BitStatus));
	}
	else
	{ /* on each related serdes */
		for (i = 0; i < curPortParams->numOfActLanes; i++)
		{
			CHECK_STATUS(mvHwsSerdesPowerCtrl(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
								(curLanesList[i] & 0xFFFF), (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)),
								GT_TRUE, curPortParams->serdesSpeed,refClock, refClockSource,
								curPortParams->serdesMediaType, curPortParams->serdes10BitStatus));
		}
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsPortModeSquelchCfg
*
* DESCRIPTION:
*       reconfigure default squelch threshold value only for KR (CR) modes
*      assumes .parameters validation in the calling function
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       curLanesList     - active Serdes lanes list according to configuration
*       txAmp   - Reference clock frequency
*       emph0 - Reference clock source line
*       emph1 - Reference clock source line
* *
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortModeSquelchCfg
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_U32 *curLanesList,
	GT_U32 txAmp,
	GT_U32 emph0,
	GT_U32 emph1
)
{
	GT_U32 i;
	GT_U32 squelchVal;
	MV_HWS_PORT_INIT_PARAMS *curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    if (curPortParams == NULL)
    {
        return GT_NOT_SUPPORTED;
    }

	switch (portMode)
	{
	case _10GBase_KR:
	case _20GBase_KR:
	case _40GBase_KR:
	case _100GBase_KR10:
	case _12_5GBase_KR:
	case XLHGL_KR4:
	case RXAUI:
	case _12GBaseR:
	case _5GBaseR:
    case _48GBaseR:
    case _12GBase_SR:
    case _48GBase_SR:
		/* on each related serdes */
		for (i = 0; i < curPortParams->numOfActLanes; i++)
		{
			mvHwsSerdesSqlchCfg(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
						(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), 2);
			mvHwsSerdesManualTxConfig(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
						(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), 26, GT_TRUE, 3, 3, GT_TRUE);
		}
		break;
	case _10GBase_SR_LR:
	case _20GBase_SR_LR:
	case _40GBase_SR_LR:
		squelchVal = (HWS_DEV_SERDES_TYPE(devNum) <= COM_PHY_H_REV2) ? 6 : 2;

		/* on each related serdes */
		for (i = 0; i < curPortParams->numOfActLanes; i++)
		{
			mvHwsSerdesSqlchCfg(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
						(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), squelchVal);
		}
	case _100GBase_SR10:
		/* on each related serdes transmitter setting needs to be tuned to compliance XLPPI eye mask on a compliance board such as 32QSFP */
		for (i = 0; i < curPortParams->numOfActLanes; i++)
		{
			mvHwsSerdesManualTxConfig(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)), (curLanesList[i]& 0xFFFF),
						  HWS_DEV_SERDES_TYPE(devNum), txAmp, GT_TRUE, emph0, emph1, GT_FALSE);
		}
		break;
	default:
		break;
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsPortModeCfg
*
* DESCRIPTION:
*      configures MAC and PCS components
*      assumes .parameters validation in the calling function
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortModeCfg
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode
)
{
	MV_HWS_PORT_INIT_PARAMS *curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    if (curPortParams == NULL)
    {
        return GT_NOT_SUPPORTED;
    }

	/* set PCS mode */
	if (curPortParams->serdesMediaType == RXAUI_MEDIA)
	{
		CHECK_STATUS(mvHwsPcsModeCfg(devNum, portGroup, curPortParams->portPcsNumber,
					 curPortParams->portPcsType, curPortParams->portFecMode,
					 curPortParams->numOfActLanes * 2));
	}
	else
	{
		CHECK_STATUS(mvHwsPcsModeCfg(devNum, portGroup, curPortParams->portPcsNumber,
					curPortParams->portPcsType, curPortParams->portFecMode,
					curPortParams->numOfActLanes));
	}

	/* set MAC mode */
	CHECK_STATUS(mvHwsMacModeCfg(devNum, portGroup, curPortParams->portMacNumber,
				curPortParams->portMacType, curPortParams->numOfActLanes));

	return GT_OK;
}

/*******************************************************************************
* mvHwsPortStartCfg
*
* DESCRIPTION:
*      Unreset  MAC and PCS components
*      assumes .parameters validation in the calling function
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortStartCfg
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode
)
{
	MV_HWS_PORT_INIT_PARAMS *curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    if (curPortParams == NULL)
    {
        return GT_NOT_SUPPORTED;
    }

	/* pcs unreset */
	CHECK_STATUS(mvHwsPcsReset(devNum, portGroup,
						curPortParams->portPcsNumber,
						curPortParams->portPcsType, UNRESET));

	/* run RF (digital reset / unreset */
	if (curPortParams->portPcsType == XPCS)
	{
		CHECK_STATUS(mvHwsSerdesDigitalRfResetToggleSet(devNum, portGroup, phyPortNum, portMode, 1));
	}

	/* mac unreset */
	CHECK_STATUS(mvHwsMacReset(devNum, portGroup,
						curPortParams->portMacNumber,
						curPortParams->portMacType, UNRESET));

	return GT_OK;
}

/*******************************************************************************
* mvHwsPortStopCfg
*
* DESCRIPTION:
*      Reset  MAC and PCS components
*      Port power down on each related serdes
*      assumes .parameters validation in the calling function
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       action - port power down or reset
*       curLanesList     - active Serdes lanes list according to configuration
*       reset pcs        - reset pcs option
*       reset mac        - reset mac option
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortStopCfg
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    MV_HWS_PORT_ACTION action,
    GT_U32 *curLanesList,
    MV_HWS_RESET reset_pcs,
    MV_HWS_RESET reset_mac
)
{
	GT_U32 i;
	MV_HWS_PORT_INIT_PARAMS *curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    if (curPortParams == NULL)
    {
        return GT_NOT_SUPPORTED;
    }

    if(MMPCS == curPortParams->portPcsType)
    {
        CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
						    curPortParams->portPcsType, reset_pcs));
    }
    else
    {
        CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
						    curPortParams->portPcsType, RESET));
    }
    if(XLGMAC == curPortParams->portMacType)
    {
        CHECK_STATUS(mvHwsMacReset(devNum, portGroup, curPortParams->portMacNumber,
		  				    curPortParams->portMacType, reset_mac));
    }
    else
    {
        CHECK_STATUS(mvHwsMacReset(devNum, portGroup, curPortParams->portMacNumber,
		  				    curPortParams->portMacType, RESET));
    }

	if ((curPortParams->portMacType == GEMAC_SG) ||
	    (curPortParams->portMacType == GEMAC_X))
	{
		CHECK_STATUS(mvHwsMacModeSelectDefaultCfg(devNum, portGroup,
									curPortParams->portMacNumber,GEMAC_X));
	}

	/* rebuild active lanes list according to current configuration (redundancy) */
	CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

	/* PORT_POWER_DOWN on each related serdes */
	for (i = 0; (PORT_POWER_DOWN == action) && (i < curPortParams->numOfActLanes); i++)
	{
		CHECK_STATUS(mvHwsSerdesPowerCtrl(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)), (curLanesList[i] & 0xFFFF),
						  HWS_DEV_SERDES_TYPE(devNum), GT_FALSE, curPortParams->serdesSpeed, _156dot25Mhz,
						  PRIMARY_LINE_SRC, curPortParams->serdesMediaType, curPortParams->serdes10BitStatus));
	}

	return GT_OK;
}

