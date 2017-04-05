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
* mvHwsPortCfgIf.h
*
* DESCRIPTION:
*           This file contains API for port configuartion and tuning parameters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
******************************************************************************/

#ifndef __mvHwsPortCfgIf_H
#define __mvHwsPortCfgIf_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mvHwsPortInitIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <serdes/mvHwsSerdesIf.h>

#define IS_SR_MODE(portMode) (portMode == _10GBase_SR_LR || \
				portMode == _20GBase_SR_LR || \
				portMode == _40GBase_SR_LR || \
				portMode == _100GBase_SR10 || \
				portMode == INTLKN_12Lanes_10_3125G || \
				portMode == INTLKN_16Lanes_10_3125G || \
				portMode == INTLKN_12Lanes_12_5G || \
				portMode == INTLKN_16Lanes_12_5G || \
				portMode == TCAM || \
				portMode == _12GBase_SR || \
				portMode == _48GBase_SR)

#define IS_KR_MODE(portMode) (portMode == _10GBase_KR || \
				portMode == _20GBase_KR || \
				portMode == _40GBase_KR || \
				portMode == _100GBase_KR10 || \
				portMode == _12_5GBase_KR || \
				portMode == XLHGL_KR4 || \
				portMode == CHGL11_LR12 || \
				portMode == _12GBaseR || \
				portMode == _48GBaseR)

/*
 * Typedef: enum MV_HWS_PORT_STANDARD
 *
 * Description: Defines the different port CRC modes.
 *              The XG port is capable of working at four CRC modes,
 *              the standard four-bytes mode and the proprietary
 *              one-byte/two-bytes/three-bytes CRC mode.
 *
 */
typedef enum
{
    HWS_1Byte_CRC,
    HWS_2Bytes_CRC,
    HWS_3Bytes_CRC,
    HWS_4Bytes_CRC

}MV_HWS_PORT_CRC_MODE;


/*******************************************************************************
* hwsPortExtendedModeCfg
*
* DESCRIPTION:
*       Enable / disable extended mode on port specified.
*       Extended ports supported only in Lion2 and Alleycat3 devices.
*       For Lion2:	1G, 10GBase-R, 20GBase-R2, RXAUI - can be normal or extended
*					XAUI, DXAUI, 40GBase-R - only extended
*		For Alleycat3:	ports 25 and 27 can be 10GBase_KR, 10GBase_SR_LR - normal or extended modes
*						port 27 can be 20GBase_KR, 20GBase_SR_LR - only in extended mode
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
GT_STATUS hwsPortExtendedModeCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 extendedMode
);

/*******************************************************************************
* mvHwsPortExtendedModeCfgGet
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
GT_STATUS mvHwsPortExtendedModeCfgGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 *extendedMode
);

/*******************************************************************************
* mvHwsPortFixAlign90Ext
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
GT_STATUS mvHwsPortFixAlign90Ext
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode
);

/*******************************************************************************
* mvHwsExtendedPortSerdesTxIfSelectSet
*
* DESCRIPTION:
*       Configures the Serdes Tx interface selector for Extended Ports
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
GT_STATUS mvHwsExtendedPortSerdesTxIfSelectSet
(
	GT_U8					devNum,
	GT_U32					portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD	portMode
);

/*******************************************************************************
* hwsPortTxAutoTuneStartSet
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
GT_STATUS hwsPortTxAutoTuneStartSet
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode,
	GT_U32  optAlgoMask
);

/*******************************************************************************
* hwsPortTxAutoTuneActivateSet
*
* DESCRIPTION:
*       Unreset Mac and PCS.
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
GT_STATUS hwsPortTxAutoTuneActivateSet
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode
);

/*******************************************************************************
* mvHwsPortAutoTuneStop
*
* DESCRIPTION:
*       Stop Tx and Rx training.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
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
GT_STATUS mvHwsPortAutoTuneStop
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 stopRx,
    GT_BOOL                 stopTx
);

/*******************************************************************************
* mvHwsPortAutoTuneStateCheck
*
* DESCRIPTION:
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
GT_STATUS mvHwsPortAutoTuneStateCheck
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_AUTO_TUNE_STATUS *rxTune,
    MV_HWS_AUTO_TUNE_STATUS *txTune
);

/*******************************************************************************
* mvHwsPortFecCofig
*
* DESCRIPTION:
*       Configure FEC disable/enable on port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portFecEn  - GT_TRUE for FEC enable, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFecCofig
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 portFecEn
);

/*******************************************************************************
* mvHwsPortFecCofigGet
*
* DESCRIPTION:
*       Return FEC status disable/enable on port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       portFecEn  - GT_TRUE for FEC enable, GT_FALSE otherwise
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFecCofigGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 *portFecEn
);

/*******************************************************************************
* mvHwsPortInterfaceGet
*
* DESCRIPTION:
*       Gets Interface mode and speed of a specified port.
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroup - core number
*       phyPortNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       portModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS mvHwsPortInterfaceGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    *portModePtr
);

/*******************************************************************************
* mvHwsPortClearChannelCfg
*
* DESCRIPTION:
*       Configures MAC advanced feature  accordingly.
*       Can be run before create port or after delete port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       txIpg      - TX_IPG
*       txPreamble - TX Preamble
*       rxPreamble - RX Preamble
*       txCrc      - TX CRC
*       rxCrc      - RX CRC
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortClearChannelCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_U32                  txIpg,
    GT_U32                  txPreamble,
    GT_U32                  rxPreamble,
    MV_HWS_PORT_CRC_MODE    txCrc,
    MV_HWS_PORT_CRC_MODE    rxCrc
);

/*******************************************************************************
* mvHwsPortAcTerminationCfg
*
* DESCRIPTION:
*       Configures AC termination in all port serdes lanes according to mode.
*       Can be run after create port only.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portAcTermEn - enable or disable AC termination
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAcTerminationCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 portAcTermEn
);

/*******************************************************************************
* mvHwsPortCheckGearBox
*
* DESCRIPTION:
*       Check Gear Box Status on related lanes.
*       Can be run after create port only.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       laneLock  - true or false.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortCheckGearBox
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 *laneLock
);

/*******************************************************************************
* hwsForceGearBoxExt
*
* DESCRIPTION:
*       utility function for device specific identification
*
*******************************************************************************/
GT_STATUS hwsForceGearBoxExt
(
	void
);

/*******************************************************************************
* mvHwsPortFixAlign90Start
*
* DESCRIPTION:
*       Start fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
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
GT_STATUS mvHwsPortFixAlign90Start
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_ALIGN90_PARAMS   *serdesParams
);

/*******************************************************************************
* mvHwsPortFixAlign90Status
*
* DESCRIPTION:
*       Run fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       statusPtr - do all lanes of port passed align90 successfully
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFixAlign90Status
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_AUTO_TUNE_STATUS *statusPtr
);

/*******************************************************************************
* mvHwsPortFixAlign90Stop
*
* DESCRIPTION:
*       Stop fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
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
GT_STATUS mvHwsPortFixAlign90Stop
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_ALIGN90_PARAMS   *serdesParams
);

/*******************************************************************************
* hwsPortFixAlign90Ext
*
* DESCRIPTION:
*       Run fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
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
GT_STATUS hwsPortFixAlign90Ext
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode,
	GT_U32	optAlgoMask
);

#if 0
/*******************************************************************************
* mvHwsPortBetterAdaptationSet
*
* DESCRIPTION:
*       Run the better Align90 adaptation algorithm on specific port number.
*       The function returns indication if the Better adaptation algorithm
*       succeeded to run on port or not.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       betterAlign90 - indicates if the Better adaptation algorithm run on
*       		specific port or not
*
* RETURNS:  GT_OK   - on success
*           GT_FAIL - on error
*******************************************************************************/
GT_STATUS mvHwsPortBetterAdaptationSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 *betterAlign90
);
#endif

/*******************************************************************************
* mvHwsPortPartialPowerDown
*
* DESCRIPTION:
*   	Enable/Disable the power down Tx and Rx of on Port.
*   	The configuration performs Enable/Disable of Tx and Rx on specific Serdes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       powerDownRx - Enable/Disable the Port Rx power down
*       powerDownTx - Enable/Disable the Port Tx power down
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortPartialPowerDown
(
    GT_U8   	devNum,
    GT_U32   	portGroup,
    GT_U32   	phyPortNum,
    MV_HWS_PORT_STANDARD	portMode,
    GT_BOOL 	powerDownRx,
    GT_BOOL 	powerDownTx
);

/*******************************************************************************
* mvHwsPortPartialPowerStatusGet
*
* DESCRIPTION:
*   	Get the status of power Tx and Rx on port
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       powerRxStatus - GT_TRUE - Serdes power Rx is down
*                       GT_FALSE - Serdes power Rx is up
*       powerTxStatus - GT_TRUE - Serdes power Tx is down
*                       GT_FALSE - Serdes power Tx is up
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortPartialPowerStatusGet
(
    GT_U8   	devNum,
    GT_U32   	portGroup,
    GT_U32   	phyPortNum,
    MV_HWS_PORT_STANDARD	portMode,
    GT_BOOL 	*powerRxStatus,
    GT_BOOL 	*powerTxStatus
);

/*******************************************************************************
* mvHwsPortFixAlign90
*
* DESCRIPTION:
*       Run fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
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
GT_STATUS mvHwsPortFixAlign90
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode,
	GT_U32 dummyForCompilation
);

/*******************************************************************************
* mvHwsPortUnitReset
*
* DESCRIPTION:
*       Reset/Unreset the MAC/PCS unit number of port.
*       For PMA (Serdes) unit, performs Power-Down for all port lanes
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       unitType    - Type of unit: MAC, PCS or PMA(Serdes)
*       action      - RESET:   Reset the MAC/PCS.  Power-down for PMA(Serdes)
*                     UNRESET: Unreset the MAC/PCS.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortUnitReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_UNIT             unitType,
    MV_HWS_RESET            action
);

/*******************************************************************************
* mvHwsPortPcsActiveStatusGet
*
* DESCRIPTION:
*       Return number of PCS active lanes or 0 if PCS unit is under RESET.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPcsActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_U32                  *numOfLanes
);

/*******************************************************************************
* mvHwsPortPsyncBypassCfg
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
GT_STATUS mvHwsPortPsyncBypassCfg
(
    GT_U8	                devNum,
    GT_U32	                portGroup,
    GT_U32	                phyPortNum,
    MV_HWS_PORT_STANDARD	portMode
);

/*******************************************************************************
* mvHwsPortEnhanceTuneMaxDelaySet
*
* DESCRIPTION:
*       Perform Enhance Tuning for finding the best peak of the eye
*       on specific port.
*       This API executes the iCAL (Rx-Training) with Max Delay value.
*       Max Delay value set the maximum iterations for running pCAL in
*       mvHwsAvagoSerdesAutoTuneStatus which included the delay.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - mode type of port
*       min_LF     - Minimum LF value that can be set on Serdes (0...15)
*       max_LF     - Maximum LF value that can be set on Serdes (0...15)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortEnhanceTuneSet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_U8       min_LF,
    GT_U8       max_LF
);

/*******************************************************************************
* mvHwsPortVoltageGet
*
* DESCRIPTION:
*       Get the voltage (in mV) of BobK device
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*
* OUTPUTS:
*       voltage - device voltage value (in mV)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortVoltageGet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      *voltage
);

/*******************************************************************************
* mvHwsPortTemperatureGet
*
* DESCRIPTION:
*       Get the Temperature (in C) of BobK device
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*
* OUTPUTS:
*       voltage - device voltage value (in C)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTemperatureGet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_32       *temperature
);

/*******************************************************************************
* mvHwsPortFlowControlStateSet
*
* DESCRIPTION:
*       Activates the port loopback modes.
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       fcState    - flow control state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFlowControlStateSet
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          phyPortNum,
    MV_HWS_PORT_STANDARD            portMode,
    MV_HWS_PORT_FLOW_CONTROL_ENT    fcState
);

#ifdef __cplusplus
}
#endif

#endif /* __mvHwsPortMiscIf_H */


