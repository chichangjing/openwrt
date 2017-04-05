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
* mvComPhyH28nmIf.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvComPhyHRev28nmRev3If_H
#define __mvComPhyHRev28nmRev3If_H

/* General H Files */
#include <serdes/mvHwsSerdesIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmIf.h>

#ifdef __cplusplus
extern "C" {
#endif 



/*******************************************************************************
* mvHwsComH28nmRev3IfInit
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
GT_STATUS mvHwsComH28nmRev3IfInit(MV_HWS_SERDES_FUNC_PTRS *funcPtrArray);

#ifndef CO_CPU_RUN

/*******************************************************************************
* mvHwsComH28nmRev3SerdesManualRxConfig
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
GT_STATUS mvHwsComH28nmRev3SerdesManualRxConfig
(
    GT_U8   devNum,
    GT_UOPT  portGroup,
    GT_UOPT  serdesNum,
    MV_HWS_PORT_MAN_TUNE_MODE portTuningMode,
    GT_UREG_DATA  sqlch,
    GT_UREG_DATA  ffeRes,
    GT_UREG_DATA  ffeCap,
    GT_BOOL dfeEn,
    GT_UREG_DATA  alig
);

/******************************************************************************
* mvHwsComH28nmRev3SerdesF0dGet
*
* DESCRIPTION:  Returns the F0d value of DFE register
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:  F0d value
*
* RETURNS:  GT_OK   - on success
*           GT_FAIL - on error
******************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesF0dGet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_U32      *F0d
);

/*******************************************************************************
* mvHwsComH28nmRev3SerdesSignalDetectGet
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
GT_STATUS mvHwsComH28nmRev3SerdesSignalDetectGet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    GT_BOOL     *signalDet
);

/*******************************************************************************
* mvHwsComH28nmRev3SerdesFixAlign90
*
* DESCRIPTION:
*       Start fix Align90 process on current SERDES.
*       Can be run after create port.
*       For Rev 3.0: SW based search of align90 according to DFE_F1, DFE_F2
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* RETURNS:
*       0  - on success
*       1  - on error
********************************************************************************/
GT_STATUS mvHwsComH28nmRev3SerdesFixAlign90
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum
);

/******************************************************************************
* mvHwsComH28nmRev3FindBetterAdaptation
*
* DESCRIPTION: Find a better Align90 adaptation value when temperature on
*              device is changed from cold->hot or from hot->cold.
*              Loops on certain FFE_R and Align90 values and calls to
*              mvHwsComH28nmRev3SerdesF0dGet to evaluate the improvment.
*              This procedure is relevant only when:
*               - Serdes speeds is 10.3125G and above
*               - signal detect is 0
*               - CDR is locked
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - physical serdes number
*       threshDb    - threshold in db
*       avgFactor   - number of iterations for getting the average value of F0d
*
* OUTPUTS:  *donePtr - indicate if Better Adaptation algorithm run on
*                      Serdes or not
*
* RETURNS:  GT_OK   - on success
*           GT_FAIL - on error
*           GT_BAD_STATE - if Serdes is in Power Down
*           GT_NOT_SUPPORTED - if Serdes speed is wrong
*           GT_NOT_READY - if SQ_detect is not 0 and CDR is not locked
******************************************************************************/
GT_STATUS mvHwsComH28nmRev3FindBetterAdaptation
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_U32      threshDb,
    GT_U32      avgFactor,
    GT_BOOL     *donePtr
);

/*******************************************************************************
* mvHwsComH28nmRev3SerdesPartialPowerDown
*
* DESCRIPTION:
*       Enable/Disable Serdes power down on Tx and Rx.
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
GT_STATUS mvHwsComH28nmRev3SerdesPartialPowerDown
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     powerDownRx,
    GT_BOOL     powerDownTx
);

/*******************************************************************************
* mvHwsComH28nmRev3SerdesPartialPowerStatusGet
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
GT_STATUS mvHwsComH28nmRev3SerdesPartialPowerStatusGet
(
    GT_U8       devNum,
    GT_UOPT     portGroup,
    GT_UOPT     serdesNum,
    GT_BOOL     *powerRxStatus,
    GT_BOOL     *powerTxStatus
);

/*******************************************************************************
* mvHwsComH28nmRev3SerdesSpeedGet
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
GT_STATUS mvHwsComH28nmRev3SerdesSpeedGet
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    MV_HWS_SERDES_SPEED *rate
);

/*******************************************************************************
* mvHwsComH28nmIfClose
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
void mvHwsComH28nmRev3IfClose(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __mvComPhyHRev28nmRev3If_H */

