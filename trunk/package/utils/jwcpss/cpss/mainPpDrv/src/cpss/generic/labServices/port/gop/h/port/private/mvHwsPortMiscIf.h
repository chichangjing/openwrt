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
* mvHwsPortMiscIf.h
*
* DESCRIPTION:
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
******************************************************************************/

#ifndef __mvHwsPortMiscIf_H
#define __mvHwsPortMiscIf_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mvHwsPortInitIf.h>

/*
 * Typedef: enum MV_HWS_PORT_AUTO_TUNE_MODE
 *
 * Description: Defines different port auto tuning modes.
 *
 */
typedef enum
{
    RxTrainingOnly = 1,
    TRxTuneCfg,
    TRxTuneStart,
    TRxTuneStatus,

	/* for AP */
	TRxTuneStatusNonBlocking,
	TRxTuneStop,

    /* for Avago */
    RxTrainingAdative,      /* run DFE_START_ADAPTIVE mode */
    RxTrainingStopAdative   /* run DFE_STOP_ADAPTIVE  mode */

}MV_HWS_PORT_AUTO_TUNE_MODE;

typedef enum
{
  TUNE_PASS,
  TUNE_FAIL,
  TUNE_NOT_COMPLITED
}MV_HWS_AUTO_TUNE_STATUS;


typedef enum
{
  Neg_3_TAPS,
  Neg_2_TAPS,
  Neg_1_TAPS,
  ZERO_TAPS,
  POS_1_TAPS,
  POS_2_TAPS,
  POS_3_TAPS

}MV_HWS_PPM_VALUE;

/*
 * Typedef: enum MV_HWS_PORT_MAN_TUNE_MODE
 *
 * Description: Defines different port manual tuning modes.
 *
 */
typedef enum
{
  StaticLongReach,  /*(Rx & Tx)*/
  StaticShortReach   /*(Rx & Tx)*/

}MV_HWS_PORT_MAN_TUNE_MODE;

typedef struct
{
	MV_HWS_AUTO_TUNE_STATUS rxTune;
	MV_HWS_AUTO_TUNE_STATUS txTune;

}MV_HWS_AUTO_TUNE_STATUS_RES;

typedef struct
{
  GT_U32 ffeR;
  GT_U32 ffeC;
  GT_U32 sampler;
  GT_U32 sqleuch;
  GT_U32 txAmp;       /* atten in Avago */
  GT_U32 txEmph0;     /* post in Avago */
  GT_U32 txEmph1;     /* pre in Avago */
  GT_U32 align90;
  GT_32  dfeVals[6];

  /* Lion2B additions */
  GT_U32 txAmpAdj;
  GT_U32 txAmpShft;
  GT_U32 txEmph0En;
  GT_U32 txEmph1En;

  /* Avago */
  GT_U32 DC;
  GT_U32 LF;
  GT_U32 HF;
  GT_U32 BW;
  GT_U32 LB;
  GT_U32 EO;
  GT_32  DFE[13];

}MV_HWS_AUTO_TUNE_RESULTS;

typedef struct
{
    GT_U32      sqlch;
    GT_U32      ffeRes;
    GT_U32      ffeCap;
    GT_BOOL     dfeEn;
    GT_U32      alig;

}MV_HWS_MAN_TUNE_RX_CONFIG_DATA;

typedef struct
{
    GT_U32              txAmp;
    GT_BOOL             txAmpAdj;
    GT_U32              txEmph0;
    GT_U32              txEmph1;
    GT_BOOL             txAmpShft;

}MV_HWS_MAN_TUNE_TX_CONFIG_DATA;

typedef struct
{
    GT_U32     dcGain;
    GT_U32     lowFrequency;
    GT_U32     highFrequency;
    GT_U32     bandWidth;
    GT_U32     loopBandwidth;
    GT_U32     squelch;

}MV_HWS_MAN_TUNE_CTLE_CONFIG_DATA;

typedef struct
{
	GT_BOOL calDone;
	GT_U32	txImpCal;
	GT_U32	rxImpCal;
	GT_U32	ProcessCal;
	GT_U32	speedPll;
	GT_U32	sellvTxClk;
	GT_U32	sellvTxData;
	GT_U32	sellvTxIntp;
	GT_U32	sellvTxDrv;
	GT_U32	sellvTxDig;
	GT_U32	sellvRxSample;
	GT_BOOL ffeDone;
	GT_32	ffeCal[8];
	
}MV_HWS_CALIBRATION_RESULTS;

/*
 * Typedef: enum MV_HWS_PORT_TEST_GEN_PATTERN
 *
 * Description: Defines different port (PCS) test generator patterns.
 *
 */
typedef enum
{
  TEST_GEN_PRBS7,
  TEST_GEN_PRBS23,
  TEST_GEN_CJPAT,
  TEST_GEN_CRPAT,
  TEST_GEN_KRPAT,
  TEST_GEN_Normal

}MV_HWS_PORT_TEST_GEN_PATTERN;

/*
 * Typedef: enum MV_HWS_PORT_TEST_GEN_ACTION
 *
 * Description: Defines different port test generator actions.
 *
 */
typedef enum
{
  NORMAL_MODE,
  TEST_MODE
}MV_HWS_PORT_TEST_GEN_ACTION;


/*
 * Typedef: enum MV_HWS_PORT_LB_TYPE
 *
 * Description: Defines different port loop back types.
 *
    DISABLE_LB - disable loop back
    RX_2_TX_LB - configure port to send back all received packets
    TX_2_RX_LB - configure port to receive back all sent packets 

 */
typedef enum
{
    DISABLE_LB,
    RX_2_TX_LB,
    TX_2_RX_LB,         /* on SERDES level - analog loopback */
    TX_2_RX_DIGITAL_LB  /* on SERDES level - digital loopback */

}MV_HWS_PORT_LB_TYPE;

/*
 * Typedef: enum MV_HWS_UNIT
 *
 * Description: Defines different port loop back levels.
 *
 */
typedef enum
{
  HWS_MAC,
  HWS_PCS,
  HWS_PMA /*serdes */

}MV_HWS_UNIT;

/*
 * Typedef: struct MV_HWS_TEST_GEN_STATUS
 *
 * Description: Defines port test generator results.
 *
 */
typedef struct  
{
  GT_U32  totalErrors;
  GT_U32  goodFrames;
  GT_U32  checkerLock;

}MV_HWS_TEST_GEN_STATUS;

/*
 * Typedef: enum MV_HWS_PORT_PRESET_CMD
 *
 * Description: Defines PRESET commands.
 *
 */
typedef enum
{
  INIT = 1,
  PRESET = 2,
  PRESET_NA = 0xFFFF

}MV_HWS_PORT_PRESET_CMD;

/*
 * Typedef: struct MV_HWS_SERDES_TUNNIG_PARAMS
 *
 * Description: Defines parameters of port tuning.
 *
 */
typedef struct
{
    MV_HWS_PORT_PRESET_CMD  presetCmdVal;
    GT_BOOL                 eyeCheckEnVal;
    GT_U32                  gen1TrainAmp;
    GT_U32                  gen1TrainEmph0;
    GT_U32                  gen1TrainEmph1;
    GT_BOOL                 gen1AmpAdj;
    GT_U32                  initialAmp;
    GT_U32                  initialEmph0;
    GT_U32                  initialEmph1;
    GT_U32                  presetAmp;
    GT_U32                  presetEmph0;
    GT_U32                  presetEmph1;

}MV_HWS_SERDES_TUNING_PARAMS;

/*
 * typedef: enum MV_HWS_PORT_FLOW_CONTROL_ENT
 *
 * Description: Enumeration of Port FC Direction enablers.
 *
 * Enumerations:
 *  MV_HWS_PORT_FLOW_CONTROL_DISABLE_E  - Both disabled
 *  MV_HWS_PORT_FLOW_CONTROL_RX_TX_E    - Both enabled
 *  MV_HWS_PORT_FLOW_CONTROL_RX_ONLY_E  - Only Rx FC enabled
 *  MV_HWS_PORT_FLOW_CONTROL_TX_ONLY_E  - Only Tx FC enabled
 *
 * Comments:
 *         None.
 */
typedef enum
{
    MV_HWS_PORT_FLOW_CONTROL_DISABLE_E = GT_FALSE,
    MV_HWS_PORT_FLOW_CONTROL_RX_TX_E = GT_TRUE,
    MV_HWS_PORT_FLOW_CONTROL_RX_ONLY_E,
    MV_HWS_PORT_FLOW_CONTROL_TX_ONLY_E
} MV_HWS_PORT_FLOW_CONTROL_ENT;

/*******************************************************************************
* mvHwsPortAutoTuneSet
*
* DESCRIPTION:
*       Sets the port Tx and Rx parameters according to different working 
*       modes/topologies.
*       Can be run any time after create port.
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
GT_STATUS mvHwsPortAutoTuneSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PORT_AUTO_TUNE_MODE  portTuningMode,
    void *                      results
);

/*******************************************************************************
* mvHwsPortAutoTuneSetExt
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
GT_STATUS mvHwsPortAutoTuneSetExt
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PORT_AUTO_TUNE_MODE  portTuningMode,
	GT_U32					optAlgoMask,
    void *                  results
);

/*******************************************************************************
* mvHwsPortAvagoAutoTuneSetExt
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
GT_STATUS mvHwsPortAvagoAutoTuneSetExt
(
    GT_U8                       devNum,
	GT_U32                      portGroup,
	GT_U32                      phyPortNum,
	MV_HWS_PORT_STANDARD        portMode,
	MV_HWS_PORT_AUTO_TUNE_MODE  portTuningMode,
	GT_U32                      optAlgoMask,
	void                        *results
);

/*******************************************************************************
* mvHwsPortAutoTuneOptimization
*
* DESCRIPTION:
*       Run optimization for the training results.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       optAlgoMask- bit mask of optimization algorithms
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneOptimization
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
	GT_U32					optAlgoMask
);

/*******************************************************************************
* mvHwsPortAutoTuneDelayInit
*
* DESCRIPTION:
*       Sets the delay values which are used in Serdes training optimization 
*		algorithm
*
* INPUTS:
*       dynamicDelayInterval - determines the number of training iteration in   
*							   which the delay will be executed (DFE algorithm)
*       dynamicDelayDuration - delay duration in mSec (DFE algorithm)
*       staticDelayDuration  - delay duration in mSec (Align90 algorithm)
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneDelayInit
(
	GT_U32     dynamicDelayInterval, /* DFE */
	GT_U32     dynamicDelayDuration, /* DFE */
	GT_U32     staticDelayDuration   /* Align90 */
);

/*******************************************************************************
* mvHwsPortManTuneSet
*
* DESCRIPTION:
*       Sets the port Tx and Rx parameters according to different working 
*       modes/topologies.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortManTuneSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PORT_MAN_TUNE_MODE portTuningMode,
    MV_HWS_AUTO_TUNE_RESULTS *tunParams
);

/*******************************************************************************
* mvHwsPortManualRxConfig
*
* DESCRIPTION:
*   	configures SERDES Rx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       configParams- pointer to array of the config params structures
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortManualRxConfig
(
    GT_U8   	                    devNum,
    GT_U32   	                    portGroup,
    GT_U32   	                    phyPortNum,
    MV_HWS_PORT_STANDARD	        portMode,
    MV_HWS_PORT_MAN_TUNE_MODE       portTuningMode,
    MV_HWS_MAN_TUNE_RX_CONFIG_DATA  *confifParams
);

/*******************************************************************************
* mvHwsPortManualTxConfig
*
* DESCRIPTION:
*   	configures SERDES tx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       configParams- pointer to array of the config params structures
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortManualTxConfig
(
    GT_U8   	                    devNum,
    GT_U32   	                    portGroup,
    GT_U32   	                    phyPortNum,
    MV_HWS_PORT_STANDARD	        portMode,
    MV_HWS_PORT_MAN_TUNE_MODE       portTuningMode,
    MV_HWS_MAN_TUNE_TX_CONFIG_DATA  *configParams
);

/*******************************************************************************
* mvHwsPortManualTxConfigGet
*
* DESCRIPTION:
*       Get SERDES tx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*
* OUTPUTS:
*       configParams- pointer to array of the config params structures:
*           txAmp     Tx amplitude
*           txAmpAdj  TX adjust
*           txemph0   TX emphasis 0
*           txemph1   TX emphasis 1
*           txAmpShft TX amplitude shift
*
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortManualTxConfigGet
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          phyPortNum,
    MV_HWS_PORT_STANDARD            portMode,
    MV_HWS_PORT_MAN_TUNE_MODE       portTuningMode,
    MV_HWS_MAN_TUNE_TX_CONFIG_DATA  *configParams
);

/*******************************************************************************
* mvHwsPortManualCtleConfig
*
* DESCRIPTION:
*       configures SERDES Ctle parameters for specific one SERDES lane or
*       for all SERDES lanes on port.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       laneNum     - Serdes number on port or
*                     0xFF for all Serdes numbers on port
*       configParams- pointer to array of the config params structures
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortManualCtleConfig
(
    GT_U8                               devNum,
    GT_U32                              portGroup,
    GT_U32                              phyPortNum,
    MV_HWS_PORT_STANDARD                portMode,
    GT_U8                               laneNum,
    MV_HWS_MAN_TUNE_CTLE_CONFIG_DATA    *configParams
);

/*******************************************************************************
* mvHwsPortManualCtleConfigGet
*
* DESCRIPTION:
*       Get SERDES Ctle parameters of each SERDES lane.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       laneNum     - Serdes number on port or
*                     0xFF for all Serdes numbers on port
*
* OUTPUTS:
*       configParams - array of Ctle params structures, Ctle parameters of each SERDES lane.
*       Ctle params structures:
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
*******************************************************************************/
GT_STATUS mvHwsPortManualCtleConfigGet
(
    GT_U8                               devNum,
    GT_U32                              portGroup,
    GT_U32                              phyPortNum,
    MV_HWS_PORT_STANDARD                portMode,
    GT_U8                               laneNum,
    MV_HWS_MAN_TUNE_CTLE_CONFIG_DATA    configParams[]
);

/*******************************************************************************
* mvHwsPortTestGenerator
*
* DESCRIPTION:
*       Activate the port related PCS Tx generator and Rx checker control.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portPattern - port tuning mode
*       actionMode - 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTestGenerator
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PORT_TEST_GEN_PATTERN  portPattern,
    MV_HWS_PORT_TEST_GEN_ACTION   actionMode    
);

/*******************************************************************************
* mvHwsPortTestGeneratorStatus
*
* DESCRIPTION:
*       Get test errors - every get clears the errors.
*       Can be run any time after delete port or after power up
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Test system results: total errors, good frames,	checker Lock Status
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTestGeneratorStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PORT_TEST_GEN_PATTERN  portPattern,
    MV_HWS_TEST_GEN_STATUS        *status    
);

/*******************************************************************************
* mvHwsPortPPMSet
*
* DESCRIPTION:
*       Increase/decrease  Tx clock on port (added/sub ppm).
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portPPM    - limited to +/- 3 taps
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPPMSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PPM_VALUE        portPPM
);

/*******************************************************************************
* mvHwsPortPPMGet
*
* DESCRIPTION:
*       Check the entire line configuration, return ppm value in case of match in all
*       or error in case of different configuration.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       portPPM    - current PPM
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPPMGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PPM_VALUE        *portPPM
);

/*******************************************************************************
* mvHwsPortLoopbackSet
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
*       lpPlace    - unit for loopback configuration
*       lpType     - loopback type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLoopbackSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_UNIT             lpPlace,
    MV_HWS_PORT_LB_TYPE     lbType    
);

/*******************************************************************************
* mvHwsPortLoopbackStatusGet
*
* DESCRIPTION:
*       Retrive MAC loopback status.
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       lpPlace    - unit for loopback configuration
*
* OUTPUTS:
*       lbType    - supported loopback type
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLoopbackStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_UNIT             lpPlace,
    MV_HWS_PORT_LB_TYPE     *lbType
);

/*******************************************************************************
* mvHwsPortLinkStatusGet
*
* DESCRIPTION:
*       Returns the port link status.
*       Can be run any time.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       Port link UP status (true/false).
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLinkStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 *linkStatus    
);

/*******************************************************************************
* mvHwsPortPolaritySet
*
* DESCRIPTION:
*       Defines the port polarity of the Serdes lanes (Tx/Rx).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       txInvMask  - bitmap of 32 bit, each bit represent Serdes
*       rxInvMask  - bitmap of 32 bit, each bit represent Serdes
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPolaritySet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_32                   txInvMask,
	GT_32                   rxInvMask
);

/*******************************************************************************
* mvHwsPortTxDisable
*
* DESCRIPTION:
*       Turn of the port Tx according to selection.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       enable     - enable/disable port Tx
*
* OUTPUTS:
*       Tuning results for recommended settings.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTxEnable
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL					enable
);


/*******************************************************************************
* mvHwsPortTxEnableGet
*
* DESCRIPTION:
*       Retrieve the status of all port serdeses.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       TxStatus per serdes.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTxEnableGet
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL serdesTxStatus[]
);

/*******************************************************************************
* mvHwsPortSignalDetectGet
*
* DESCRIPTION:
*       Retrieve the signal detected status of all port serdeses.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       signalDetect per serdes.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortSignalDetectGet
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL                 signalDet[]
);

/*******************************************************************************
* mvHwsPortCdrLockStatusGet
*
* DESCRIPTION:
*       Retrieve the CDR lock status of all port serdeses.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       cdrLockStatus per serdes.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortCdrLockStatusGet
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL                 cdrLockStatus[]
);

/*******************************************************************************
* mvHwsPortLaneNumberGet
*
* DESCRIPTION:
*       Returns the active port lanes (after applying redundancy).
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       laneNumber - size of allocated memory
*
* OUTPUTS:
*       List of lanes and lanes number.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLaneNumberGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_U32                 *laneNumber,
    GT_U32                 **listOfLanes
);

/*******************************************************************************
* mvHwsVersionGet
*
* DESCRIPTION:
*       Returns version of HWS APIs.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*     Pointer to version string
*
*******************************************************************************/
const GT_CHAR* mvHwsVersionGet(void);


#ifdef __cplusplus
}
#endif

#endif /* __mvHwsPortMiscIf_H */


