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
* mvHwsCgPcsIf.h
*
* DESCRIPTION:
*       CG PCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#ifndef __mvHwsCgPcsIf_H
#define __mvHwsCgPcsIf_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <pcs/mvHwsPcsIf.h>

/*******************************************************************************
* mvHwsCgPcsIfInit
*
* DESCRIPTION:
*       Init PCS configuration sequences and IF functions.
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
GT_STATUS mvHwsCgPcsIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray);

/*******************************************************************************
* mvHwsCgPcsReset
*
* DESCRIPTION:
*       Set the selected PCS type and number to reset or exit from reset.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       action    - reset / unreset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            action
);

/*******************************************************************************
* mvHwsCgPcsMode
*
* DESCRIPTION:
*       Set the internal mux's to the required PCS in the PI.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       fecCtrl   - FEC control
*       numOfLanes - number of lanes agregated in PCS
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsMode
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_U32                  numOfLanes
);

/*******************************************************************************
* mvHwsCgPcsLoopback
*
* DESCRIPTION:
*       Set PCS loop back.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*       lbType    - loop back type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsLoopBack
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_LB_TYPE     lbType
);

/*******************************************************************************
* mvHwsCgPcsSignalDetectMaskSet
*
* DESCRIPTION:
*       Set all related PCS with Signal Detect Mask value (1/0).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       maskEn    - if true, enable signal detect mask bit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsSignalDetectMaskSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 maskEn
);

/*******************************************************************************
* mvHwsCgPcsTestGenCfg
*
* DESCRIPTION:
*       Set PCS internal test generator mechanisms.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pattern   - pattern to generate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsTestGenCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern
);

/*******************************************************************************
* mvHwsCgPcsTestGenStatus
*
* DESCRIPTION:
*       Get PCS internal test generator mechanisms error counters and status.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pattern   - pattern to generate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsTestGenStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status    
);

/*******************************************************************************
* mvHwsCgPcsFecConfig
*
* DESCRIPTION:
*       Configure FEC disable/enable on PCS.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       fecEn     - if true, enable FEC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsFecConfig
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 fecEn
);

/*******************************************************************************
* mvHwsCgPcsFecConfigGet
*
* DESCRIPTION:
*       Return FEC disable/enable on PCS.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*
* OUTPUTS:
*       fecEn     - if true, enable FEC
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsFecConfigGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 *fecEn
);

/*******************************************************************************
* mvHwsCgPcsCheckGearBox
*
* DESCRIPTION:
*       Check Gear Box Status on related lanes.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       numOfLanes - number of lanes agregated in PCS
*
* OUTPUTS:
*       laneLock  - true or false.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsCheckGearBox
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_U32                  numOfLanes,
    GT_BOOL                 *laneLock
);

/*******************************************************************************
* hwsCgPcsIfClose
*
* DESCRIPTION:
*       Release all system resources allocated by PCS IF functions.
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
void hwsCgPcsIfClose(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsCgPcsIf_H */

