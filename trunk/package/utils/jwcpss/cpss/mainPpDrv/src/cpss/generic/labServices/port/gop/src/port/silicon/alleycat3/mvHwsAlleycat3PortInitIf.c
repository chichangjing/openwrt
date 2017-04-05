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
* mvHwsAlleycat3PortInitIf.c
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
#include <silicon/alleycat3/mvHwsAlleycat3PortIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <mac/mvHwsMacIf.h>

/*******************************************************************************
* mvHwsAlleycat3PortInit
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
GT_STATUS mvHwsAlleycat3PortInit
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
	MV_HWS_PORT_INIT_PARAMS *curPortParams;
	GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
	GT_BOOL serdesInit = GT_TRUE;
	GT_BOOL extMode;

	/* validation */
	CHECK_STATUS(mvHwsPortValidate(devNum, portGroup, phyPortNum, portMode));

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

	CHECK_STATUS(mvHwsPortLoopbackValidate((GT_U32)curPortParams->portPcsType, lbPort, &serdesInit));

	/* rebuild active lanes list according to current configuration (redundancy) */
	CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

	if (serdesInit == GT_TRUE)
	{
		if (curPortParams->portMacNumber == 31)
		{
			CHECK_STATUS(mvHwsSerdesTxIfSelect(devNum, portGroup, 10,
							   (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)), 2));
		}
		else
		{
			CHECK_STATUS(mvHwsPortExtendedModeCfgGet(devNum, portGroup, phyPortNum, portMode, &extMode));
			if (extMode == GT_TRUE)
			{
				CHECK_STATUS(mvHwsExtendedPortSerdesTxIfSelectSet(devNum, portGroup, phyPortNum, portMode));
			}
		}

		/* power up the serdes */
		CHECK_STATUS(mvHwsPortSerdesPowerUp(devNum, portGroup, phyPortNum, portMode, refClock, refClockSource, &(curLanesList[0])));

		/* set P Sync Bypass */
		CHECK_STATUS(mvHwsAlleycat3PortPsyncBypassCfg(devNum, portGroup, phyPortNum, portMode));

		/* Configures the DP/Core Clock Selector on port */
		CHECK_STATUS(mvHwsClockSelectorConfig(devNum, portGroup, phyPortNum, portMode));

		/* change default squelch threshold value only for KR (CR) modes*/
		CHECK_STATUS(mvHwsPortModeSquelchCfg(devNum, portGroup, phyPortNum, portMode, &(curLanesList[0]), 17, 2, 8));
	}

	CHECK_STATUS(mvHwsPortModeCfg(devNum, portGroup, phyPortNum, portMode));

	/* in lbPort mode, only for MMPCS configure PCS with External PLL */
	if ((lbPort == GT_TRUE) && (curPortParams->portPcsType == MMPCS))
	{
		CHECK_STATUS(mvHwsPcsExtPllCfg(devNum, portGroup,
						curPortParams->portPcsNumber,
					    curPortParams->portPcsType));
	}

	CHECK_STATUS(mvHwsPortStartCfg(devNum, portGroup, phyPortNum, portMode));

	if (serdesInit == GT_TRUE)
	{
		/* run SERDES digital reset / unreset */
		switch (portMode)
		{
		case HGL:
			CHECK_STATUS(mvHwsSerdesDigitalRfResetToggleSet(devNum, portGroup, phyPortNum, portMode, 100));
			break;
		default:
				break;
		}
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsAlleycat3PortReset
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
GT_STATUS mvHwsAlleycat3PortReset
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode,
	MV_HWS_PORT_ACTION	action
)
{
	MV_HWS_PORT_INIT_PARAMS *curPortParams;
	GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
	GT_U32 i;

	/* validation */
	CHECK_STATUS(mvHwsPortValidate(devNum, portGroup, phyPortNum, portMode));

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

	/* mark port as under delete */
	switch (portMode)
	{
	case RXAUI:
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XPCS_UNIT, curPortParams->portPcsNumber,
							XPCS_Internal_Metal_Fix, 0x8001, 0));
		break;
	default:
		break;
	}

	CHECK_STATUS(mvHwsPortStopCfg(devNum, portGroup, phyPortNum, portMode, action, &(curLanesList[0]), FULL_RESET, RESET));

	/* PORT_RESET on each related serdes */
	for (i = 0; (PORT_RESET == action) && (i < curPortParams->numOfActLanes); i++)
	{
		CHECK_STATUS(mvHwsSerdesReset(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
								(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum),
								GT_TRUE,GT_TRUE,GT_TRUE));
	}

	/* mark port as deleted */
	switch (portMode)
	{
	case RXAUI:
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XPCS_UNIT, curPortParams->portPcsNumber,
							XPCS_Internal_Metal_Fix, 0, 0));
		break;
	default:
		break;
	}

	return GT_OK;
}

