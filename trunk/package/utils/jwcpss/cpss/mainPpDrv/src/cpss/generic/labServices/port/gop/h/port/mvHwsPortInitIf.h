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
* mvHwsPortInitIf.h
*
* DESCRIPTION:
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
******************************************************************************/

#ifndef __mvHwServicesPortIf_H
#define __mvHwServicesPortIf_H

#ifdef __cplusplus
extern "C" {
#endif

#include <common/os/hwsEnv.h>

/*
 * Typedef: enum MV_HWS_PORT_STANDARD
 *
 * Description: Defines the different port standard metrics.
 *
 */
typedef enum
{
    _100Base_FX,
    SGMII,
    _1000Base_X,
    SGMII2_5,
    QSGMII,
    _10GBase_KX4,
    _10GBase_KX2,   /* 10GBase-DHX */
    _10GBase_KR,
    _20GBase_KR,
    _40GBase_KR,
    _100GBase_KR10,
    HGL,
    RHGL,
    CHGL,   /* CHGL_LR10 */
    RXAUI,
    _20GBase_KX4,   /* HS-XAUI */
    _10GBase_SR_LR,
    _20GBase_SR_LR,
    _40GBase_SR_LR,
    _12_5GBase_KR,   /* XLHGL_KR, XLHGS */
    XLHGL_KR4,    /* 48G */
    HGL16G,
    HGS,
    HGS4,
    _100GBase_SR10,
    CHGL_LR12,
    TCAM,
    INTLKN_12Lanes_6_25G,
    INTLKN_16Lanes_6_25G,
    INTLKN_24Lanes_6_25G,
    INTLKN_12Lanes_10_3125G,
    INTLKN_16Lanes_10_3125G,
    INTLKN_12Lanes_12_5G,
    INTLKN_16Lanes_12_5G,
    INTLKN_16Lanes_3_125G,
    INTLKN_24Lanes_3_125G,
    CHGL11_LR12,

    INTLKN_4Lanes_3_125G,
    INTLKN_8Lanes_3_125G,
    INTLKN_4Lanes_6_25G,
    INTLKN_8Lanes_6_25G,

    _2_5GBase_QX,
    _5GBase_DQX,
    _5GBase_HX,
    _12GBaseR,
    _5GBaseR,
    _48GBaseR,
    _12GBase_SR,
    _48GBase_SR,

    NON_SUP_MODE,
    LAST_PORT_MODE = NON_SUP_MODE

}MV_HWS_PORT_STANDARD;


/*
 * Typedef: enum MV_HWS_REF_CLOCK_SOURCE
 *
 * Description: Defines the supported reference clock source.
 *
 */
typedef enum
{
  PRIMARY_LINE_SRC,
  SECONDARY_LINE_SRC

}MV_HWS_REF_CLOCK_SOURCE;

/*
 * Typedef: enum MV_HWS_REF_CLOCK_SUP_VAL
 *
 * Description: Defines the supported reference clock.
 *
 */
typedef enum
{
  MHz_156,
  MHz_78,
  MHz_25,
  MHz_125

}MV_HWS_REF_CLOCK_SUP_VAL; /* need to be synchronized with MV_HWS_REF_CLOCK enum */

/*
 * Typedef: enum MV_HWS_PORT_ACTION
 *
 * Description: Defines different actions during port delete.
 *
 */
typedef enum
{
  PORT_POWER_DOWN,
  PORT_RESET

}MV_HWS_PORT_ACTION;

/*
 * Typedef: enum MV_HWS_PORT_INIT_FLAVOR
 *
 * Description: Defines different suites of port init process:
 *      PORT_AP_INIT - 	run AP on current port       
 *      PORT_REGULAR_INIT - regular port create flow (no SERDES training)  
 *      PORT_RX_TRAIN_INIT - create port and run RX training on all SERDESes 
 *      PORT_TRX_TRAIN_INIT - create port and run TRX training on all SERDESes
 *
 */
typedef enum
{
  PORT_AP_INIT,
  PORT_REGULAR_INIT,
  PORT_RX_TRAIN_INIT,
  PORT_TRX_TRAIN_INIT

}MV_HWS_PORT_INIT_FLAVOR;

/*
 * Typedef: enum MV_HWS_RESET
 *
 * Description: Defines reset types to set valid sequence
 *
 */
typedef enum
{
    RESET,
    UNRESET,
    FULL_RESET

}MV_HWS_RESET;

typedef struct
{
    MV_HWS_PORT_INIT_FLAVOR portFlavor;

}MV_HWS_PORT_INIT_FLAVOR_CFG;
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
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 lbPort,
    MV_HWS_REF_CLOCK_SUP_VAL refClock,
    MV_HWS_REF_CLOCK_SOURCE  refClockSource
);

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
*   	refClockSrc - Reference clock source line
*   	portFlavor  - describe port create suite (with/without additional actions)
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
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 lbPort,
    MV_HWS_REF_CLOCK_SUP_VAL refClock,
    MV_HWS_REF_CLOCK_SOURCE  refClockSource,
    MV_HWS_PORT_INIT_FLAVOR_CFG  portFlavorParams
);

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
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PORT_ACTION      action
);

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
);

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
*       portPcsType - port pcs type
*       lbPort      - if true, init port without serdes activity
*       serdesInit  - if true, init port serdes
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
);

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
);

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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif

#endif /* mvHwServicesPortIf_H */


