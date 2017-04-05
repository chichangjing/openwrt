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
* mvHwsBobcat2PortInitIf.c
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
#include <silicon/bobk/mvHwsBobKPortIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <mac/mvHwsMacIf.h>

/*******************************************************************************
* mvHwsBobKPortInit
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
GT_STATUS mvHwsBobKPortInit
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

	/* validation */
	CHECK_STATUS(mvHwsPortValidate(devNum, portGroup, phyPortNum, portMode));

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

	CHECK_STATUS(mvHwsPortLoopbackValidate((GT_U32)curPortParams->portPcsType, lbPort, &serdesInit));

	/* rebuild active lanes list according to current configuration (redundancy) */
	CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

	if (serdesInit == GT_TRUE)
	{
		/* power up the serdes */
		CHECK_STATUS(mvHwsPortSerdesPowerUp(devNum, portGroup, phyPortNum, portMode, refClock, refClockSource, &(curLanesList[0])));

		/* set P Sync Bypass */
		CHECK_STATUS(mvHwsPortPsyncBypassCfg(devNum, portGroup, phyPortNum, portMode));

		/* Configures the DP/Core Clock Selector on port */
		CHECK_STATUS(mvHwsClockSelectorConfig(devNum, portGroup, phyPortNum, portMode));

		/* TBD - change default squelch threshold value only for KR (CR) modes*/
		/* CHECK_STATUS(mvHwsPortModeSquelchCfg(devNum, portGroup, phyPortNum, portMode, &(curLanesList[0]), 17, 2, 8)); */
	}

    /* GEMAC gets wrong signal detect polarity */
    CHECK_STATUS(mvHwsBobKPortSignalDetectMask(devNum, portGroup, phyPortNum, portMode, GT_TRUE));

    /* Configure MAC/PCS */
	CHECK_STATUS(mvHwsPortModeCfg(devNum, portGroup, phyPortNum, portMode));

	/* in lbPort mode, only for MMPCS configure PCS with External PLL */
	if ((lbPort == GT_TRUE) && (curPortParams->portPcsType == MMPCS))
	{
		CHECK_STATUS(mvHwsPcsExtPllCfg(devNum, portGroup,
						curPortParams->portPcsNumber,
					    curPortParams->portPcsType));
	}

    /* Un-Reset the port */
	CHECK_STATUS(mvHwsPortStartCfg(devNum, portGroup, phyPortNum, portMode));

	if (serdesInit == GT_TRUE)
	{
		/* run SERDES digital reset / unreset */
		switch (portMode)
		{
		case SGMII2_5:
			CHECK_STATUS(mvHwsSerdesDigitalRfResetToggleSet(devNum, portGroup, phyPortNum, portMode, 10));
			break;
		default:
			break;
		}

#if 0 /* TBD - currently not needed for Avago */
		/* enable AC termination, if needed */
		switch (portMode)
		{
            case _1000Base_X:
            case _10GBase_SR_LR:
            case _20GBase_SR_LR:
            case _40GBase_SR_LR:
            case _12GBaseR:
            case _48GBaseR:
                for (i = 0; i < curPortParams->numOfActLanes; i++)
                {
                    CHECK_STATUS(mvHwsSerdesAcTerminationCfg(devNum, portGroup, MV_HWS_SERDES_NUM(curLanesList[i]), 
                                                             (HWS_DEV_SERDES_TYPE(devNum)), GT_TRUE));
                }
                break;
            default:
                break;
		}
#endif

#if 0 /* TBD - currently not needed for Avago */
        switch (portMode)
        {
            case SGMII:
            case QSGMII:
                /* TBD: need to find the correct Amp value for manual Tx tune */
                break;
            default:
                break;
        }
#endif
    }

	return GT_OK;
}

/*******************************************************************************
* mvHwsBobKPortReset
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
GT_STATUS mvHwsBobKPortReset
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
        case SGMII2_5:
            /* clear P Sync Bypass */
            CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, curPortParams->portMacNumber,
                                            GIG_PORT_MAC_CONTROL_REGISTER4, 0, (1 << 6)));
            break;
        default:
            break;
	}

	CHECK_STATUS(mvHwsPortStopCfg(devNum, portGroup, phyPortNum, portMode, action, &(curLanesList[0]), FULL_RESET, FULL_RESET));

    /* GEMAC gets wrong signal detect polarity (un-do the JIRA) */
    CHECK_STATUS(mvHwsBobKPortSignalDetectMask(devNum, portGroup, phyPortNum, portMode, GT_FALSE));

	/* PORT_RESET on each related serdes */
	for (i = 0; (PORT_RESET == action) && (i < curPortParams->numOfActLanes); i++) {
		CHECK_STATUS(mvHwsSerdesReset(devNum, portGroup, MV_HWS_SERDES_NUM(curLanesList[i]),
								      HWS_DEV_SERDES_TYPE(devNum), GT_TRUE, GT_TRUE, GT_TRUE));
	}

	return GT_OK;
}

