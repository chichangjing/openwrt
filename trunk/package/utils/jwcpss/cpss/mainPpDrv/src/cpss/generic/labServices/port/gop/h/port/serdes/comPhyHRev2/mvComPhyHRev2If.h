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
* mvComPhyHIf.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
*
*******************************************************************************/

#ifndef __mvComPhyHRev2If_H
#define __mvComPhyHRev2If_H

/* General H Files */
#include <serdes/mvHwsSerdesIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>


/*
 * Typedef: enum MV_HWS_SERDES_TRAINING_OPT_ALGO
 *
 * Description: Defines different Serdes training algorithms.
 *
 * Comment: Used as a bit mask for determine which algorithms to run
 *
 */
typedef enum
{
	DFE_OPT_ALGO     = 0x1,
	FFE_OPT_ALGO     = 0x2,
	Align90_OPT_ALGO = 0x4

}MV_HWS_SERDES_TRAINING_OPT_ALGO; 

/*******************************************************************************
* mvHwsComHRev2IfInit
*
* DESCRIPTION:
*       Init Com_H serdes configuration sequences and IF functions.
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
GT_STATUS mvHwsComHRev2IfInit(MV_HWS_SERDES_FUNC_PTRS *funcPtrArray);

/*******************************************************************************
* mvHwsComHRev2SerdesTrainingOptDelayInit
*
* DESCRIPTION:
*       Set the dynamic and static Delays for DFE and align90
*
* INPUTS:
*       dynamicDelayInterval - Interval delay for DFE
*       dynamicDelayDuration - Duration delay for DFE
*       staticDelayDuration - Duration delay for align90
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesTrainingOptDelayInit
(
    GT_U32  dynamicDelayInterval,
    GT_U32  dynamicDelayDuration,
    GT_U32  staticDelayDuration
);

/*******************************************************************************
* mvHwsComHRev2SerdesTestGen
*
* DESCRIPTION:
*       Activates the Serdes test generator/checker.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
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
GT_STATUS mvHwsComHRev2SerdesTestGen
(
 GT_U8                     devNum,
 GT_U32                    portGroup,
 GT_U32                    serdesNum,
 MV_HWS_SERDES_TX_PATTERN  txPattern,
 MV_HWS_SERDES_TEST_GEN_MODE mode
);

/*******************************************************************************
* mvHwsComHRev2SerdesTestGenGet
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
GT_STATUS mvHwsComHRev2SerdesTestGenGet
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TX_PATTERN  *txPatternPtr,
    MV_HWS_SERDES_TEST_GEN_MODE *modePtr
);

/*******************************************************************************
* mvHwsComHRev2SerdesPowerCtrl
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
GT_STATUS mvHwsComHRev2SerdesPowerCtrl
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 powerUp,
    MV_HWS_SERDES_SPEED     baudRate,
    MV_HWS_REF_CLOCK        refClock,
    MV_HWS_REF_CLOCK_SRC    refClockSource,
    MV_HWS_SERDES_MEDIA     media,
    MV_HWS_SERDES_10B_MODE  mode
);

/*******************************************************************************
* mvHwsComHRev2SerdesDfeStatusExt
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
*       f0Cfg     - array of f0 configuration parameters
*       savCfg    - array of sav configuration parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesDfeStatusExt
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    GT_BOOL           *dfeLock,
    GT_32             *dfeCfg,
    GT_32             *f0Cfg,
    GT_32             *savCfg
);

/*******************************************************************************
* mvHwsComHRev2SerdesTestGenGet
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
GT_STATUS mvHwsComHRev2SerdesTestGenGet
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TX_PATTERN  *txPatternPtr,
    MV_HWS_SERDES_TEST_GEN_MODE *modePtr
);

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneCfg
*
* DESCRIPTION:
*       Per SERDES configure parameters for TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesAutoTuneCfg
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL rxTraining,
    GT_BOOL txTraining
);

/*******************************************************************************
* mvHwsComHRev2SerdesManualRxConfig
*
* DESCRIPTION:
*       Per SERDES configure the RX parameters: squelch Threshold, FFE and DFE
*       operation
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical lane number
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
GT_STATUS mvHwsComHRev2SerdesManualRxConfig
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_PORT_MAN_TUNE_MODE portTuningMode,
    GT_U32  sqlch,
    GT_U32  ffeRes,
    GT_U32  ffeCap,
    GT_BOOL dfeEn,
    GT_U32  alig
);

/*******************************************************************************
* mvHwsComHRev2SerdesManualTxConfig
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
GT_STATUS mvHwsComHRev2SerdesManualTxConfig
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_U32  txAmp,
    GT_BOOL txAmpAdj,
    GT_U32  emph0,
    GT_U32  emph1,
    GT_U32  txAmpShft
);

/*******************************************************************************
* mvHwsComHRev2SerdesPpmConfig
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
GT_STATUS mvHwsComHRev2SerdesPpmConfig
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_PPM_VALUE  ppmVal
);

/*******************************************************************************
* mvHwsComHRev2SerdesPpmConfigGet
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
GT_STATUS mvHwsComHRev2SerdesPpmConfigGet
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_PPM_VALUE  *ppmVal
);

/*******************************************************************************
* mvHwsComHRev2SerdesFfeTableCfg
*
* DESCRIPTION:
*       Per SERDES config the FFE table (used by auto tune)
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
GT_STATUS mvHwsComHRev2SerdesFfeTableCfg
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
	MV_HWS_PORT_MAN_TUNE_MODE tuneMode
);

/*******************************************************************************
* mvHwsComHRev2SerdesRxAutoTuneStart
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
GT_STATUS mvHwsComHRev2SerdesRxAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL rxTraining
);

/*******************************************************************************
* mvHwsComHRev2SerdesTxAutoTuneStart
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
GT_STATUS mvHwsComHRev2SerdesTxAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL txTraining
);

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneStart
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
GT_STATUS mvHwsComHRev2SerdesAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL rxTraining,
    GT_BOOL txTraining
);

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneStatus
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
GT_STATUS mvHwsComHRev2SerdesAutoTuneStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
);

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneResult
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
GT_STATUS mvHwsComHRev2SerdesAutoTuneResult
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_RESULTS *results
);

/*******************************************************************************
* mvHwsComHRev2SerdesCalibrationStatus
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
GT_STATUS mvHwsComHRev2SerdesCalibrationStatus
(
    GT_U8                       devNum,
    GT_U32                      portGroup,
    GT_U32                      serdesNum,
    MV_HWS_CALIBRATION_RESULTS  *results
);

/*******************************************************************************
* mvHwsComHRev2SerdesTxEnable
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
GT_STATUS mvHwsComHRev2SerdesTxEnable
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    GT_BOOL     enable
);

/*******************************************************************************
* mvHwsComHRev3SerdesTxEnableGet
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
GT_STATUS mvHwsComHRev2SerdesTxEnableGet
(
 GT_U8   devNum,
 GT_U32  portGroup,
 GT_U32  serdesNum,
 GT_BOOL *enablePtr
 );

/*******************************************************************************
* mvHwsComHRev2SerdesTestGenStatus
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
GT_STATUS mvHwsComHRev2SerdesTestGenStatus
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TX_PATTERN  txPattern,
    GT_BOOL                   counterAccumulateMode,
    MV_HWS_SERDES_TEST_GEN_STATUS *status
);

/*******************************************************************************
* mvHwsComHRev2SerdesTxIfSelect
*
* DESCRIPTION:
*       tx interface select.
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
GT_STATUS mvHwsComHRev2SerdesTxIfSelect
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    GT_U32              serdesTxIfNum
);

/*******************************************************************************
* mvHwsComHRev2SqlchCfg
*
* DESCRIPTION:
*       Configure squelch threshold value.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
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
GT_STATUS mvHwsComHRev2SqlchCfg
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    GT_U32              sqlchVal
);

/*******************************************************************************
* mvHwsComHRev2DfeCheck
*
* DESCRIPTION:
*       Check DFE values range and start sampler calibration, if needed.
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
GT_STATUS mvHwsComHRev2DfeCheck
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
);

/*******************************************************************************
* mvHwsComHRev2SerdesSpeedGet
*
* DESCRIPTION:
*       Return SERDES baud rate.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       baudRate - baud rate.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesSpeedGet
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 serdesNum,
    MV_HWS_SERDES_SPEED *rate
);

/*******************************************************************************
* mvHwsComHRev2SerdesManualRxConfigGet
*
* DESCRIPTION:
*       Return SERDES RX parameters configured manually .
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
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
GT_STATUS mvHwsComHRev2SerdesManualRxConfigGet
(
    IN GT_U8					  devNum,
    IN GT_U32					  portGroup,
    IN GT_U32					  serdesNum,
    OUT MV_HWS_PORT_MAN_TUNE_MODE *portTuningModePtr,
	OUT GT_U32					  *sqlchPtr,
    OUT GT_U32					  *ffeResPtr,
    OUT GT_U32					  *ffeCapPtr,
    OUT GT_BOOL					  *dfeEnPtr,
	OUT GT_U32					  *aligPtr
);

/*******************************************************************************
* mvHwsComHRev2IfClose
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
void mvHwsComHRev2IfClose(void);

/*******************************************************************************
* mvHwsComHRev2SerdesArrayPowerCtrl
*
* DESCRIPTION:
*       Init physical port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
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
GT_STATUS mvHwsComHRev2SerdesArrayPowerCtrl
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  numOfSer,
    GT_U32                  *serdesArr,
    GT_BOOL                 powerUp,
    MV_HWS_SERDES_SPEED     baudRate,
    MV_HWS_REF_CLOCK        refClock,
    MV_HWS_REF_CLOCK_SRC    refClockSource,
    MV_HWS_SERDES_MEDIA     media,
    MV_HWS_SERDES_10B_MODE  mode
);

/*******************************************************************************
* mvHwsComHRev2SerdesAcTerminationCfg
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
GT_STATUS mvHwsComHRev2SerdesAcTerminationCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 acTermEn
);

/*******************************************************************************
* mvHwsComHRev2DigitalSerdesReset
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
GT_STATUS mvHwsComHRev2DigitalSerdesReset
(
    GT_U8      devNum,
    GT_U32     portGroup,
    GT_U32     serdesNum,
    MV_HWS_RESET    digitalReset
);


/*******************************************************************************
* mvHwsComHRev2SerdesFixAlign90Start
*
* DESCRIPTION:
*       Start fix Align90 process on current SERDES.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
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
GT_STATUS mvHwsComHRev2SerdesFixAlign90Start
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_ALIGN90_PARAMS   *params
);

/*******************************************************************************
* mvHwsComHRev2SerdesFixAlign90Status
*
* DESCRIPTION:
*       Return fix Align90 process current status on current SERDES.
*       Can be run after create port and start Align90.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
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
GT_STATUS mvHwsComHRev2SerdesFixAlign90Status
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *trainingStatus
);

/*******************************************************************************
* mvHwsComHRev2SerdesFixAlign90Stop
*
* DESCRIPTION:
*       Stop fix Align90 process on current SERDES.
*       Can be run after create port and start Align90.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
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
GT_STATUS mvHwsComHRev2SerdesFixAlign90Stop
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 serdesNum,
    MV_HWS_ALIGN90_PARAMS *params,
    GT_BOOL fixAlignPass
);

/*******************************************************************************
* mvHwsComHRev2SerdesTrainingOptimization
*
* DESCRIPTION:
*       Per SERDES Run the Serdes training optimization algorithms.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesList  - array of physical Serdes number
*		numOfActLanes - number of Serdes.  
*       tuningMode - Short/Long reach
*       algoMask   - algorithm but mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesTrainingOptimization 
(
	GT_U8						devNum, 
	GT_U32						portGroup, 
	GT_U32						*serdesList,
	GT_U32                      numOfActLanes,
	MV_HWS_PORT_MAN_TUNE_MODE	tuningMode,
	GT_U32						algoMask
);

/*******************************************************************************
* mvHwsComHRev2SerdesOptSetFfeThreshold
*
* DESCRIPTION:
*       Set FFR threshold.
*
* INPUTS:
*       ffeFirstTh - FFE first threshold
*       ffeFinalTh - FFE final threshold
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesOptSetFfeThreshold 
(
	GT_32     ffeFirstTh,
	GT_32     ffeFinalTh
);

/*******************************************************************************
* mvHwsComHRev2SerdesOptSetF0dStopThreshold
*
* DESCRIPTION:
*       Set F0D stop threshold.
*
* INPUTS:
*       f0dStopTh - F0D stop threshold
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesOptSetF0dStopThreshold 
(
	GT_U32     f0dStopTh
);

/*******************************************************************************
* mvHwsComHRev2SerdesConfigRegulator
*
* DESCRIPTION:
*       Per SERDES Run the Serdes Regulator Configuration.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesList  - array of physical Serdes number
*		numOfActLanes - number of Serdes.  
*		setDefault - set to default value 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesConfigRegulator 
(
	GT_U8						devNum, 
	GT_U32						portGroup, 
	GT_U32						*serdesList,
	GT_U32                      numOfActLanes,
	GT_BOOL                     setDefault
);

/*******************************************************************************
* mvHwsComHRev2SerdesSignalDetectGet
*
* DESCRIPTION:
*       Return Signal Detect state on PCS (true/false).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       signalDet - if true, signal was detected
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesSignalDetectGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 *signalDet
);

/*******************************************************************************
* mvHwsComHRev2SerdesCdrLockStatusGet
*
* DESCRIPTION:
*       Return SERDES CDR lock status (true - locked /false - notlocked).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       cdrLock  - if true, CDR was locked
*
* OUTPUTS:
*       cdrLock  - if true, CDR was locked
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesCdrLockStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 *cdrLock
);

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneStatusShort
*
* DESCRIPTION:
*       Per SERDES check Rx or Tx training status
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       isTx      - Rx or Tx
*
* OUTPUTS:
*       status - Training status (true/false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesAutoTuneStatusShort
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
);

/*******************************************************************************
* mvHwsComHRev2SerdesTxAutoTuneStop
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
GT_STATUS mvHwsComHRev2SerdesTxAutoTuneStop
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
);

/*******************************************************************************
* mvHwsComHRev2SerdesOptAlgoParams
*
* DESCRIPTION:
*       Change default thresholds of:
*		ffe optimizer for first iteration, final iteration and f0d stop threshold
*
* INPUTS:
*       ffeFirstTh - threshold value for changing ffe optimizer for first iteration
*       ffeFinalTh - threshold value for changing ffe optimizer for final iteration
*       f0dStopTh  - value for changing default of f0d stop threshold
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesOptAlgoParams
(
    GT_32     ffeFirstTh,
    GT_32     ffeFinalTh,
    GT_U32    f0dStopTh
);

/*******************************************************************************
* mvHwsComHRev2SerdesSetTuningParam
*
* DESCRIPTION:
*       Setting tuning parameters according to the receive value:
*       Enables/Disabled eye check, Setting PRESET command (INIT=1 or PRESET=2),
*       Gen1 Amplitude values, initialize Amplitude values preset Amplitude values.
*       Parameter will not set if received value not in the range:
*       presetCmdVal    - enum INIT/PRESET
*       eyeCheckEnVal   - GT_TRUE/GT_FALSE
*       gen1TrainAmp    - 0-0x1F
*       gen1TrainEmph0  - 0-0xF
*       gen1TrainEmph1  - 0-0xF
*       gen1AmpAdj      - GT_TRUE/GT_FALSE
*       initialAmp      - 0-0x3F
*       initialEmph0    - 0-0xF
*       initialEmph1    - 0-0xF
*       presetAmp       - 0-0x3F
*       presetEmph0     - 0-0xF
*       presetEmph1     - 0-0xF
*
* INPUTS:
*       params - pointer to structure to store tuning parameters for changing default values
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesSetTuningParam
(
    MV_HWS_SERDES_TUNING_PARAMS  *params
);

#endif /* __mvComPhyHRev2If_H */

