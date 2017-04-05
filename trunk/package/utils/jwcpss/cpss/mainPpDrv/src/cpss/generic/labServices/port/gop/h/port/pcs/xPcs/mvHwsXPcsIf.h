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
* mvHwsXPcsIf.h
*
* DESCRIPTION:
*       GPCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

#ifndef __mvHwsXPcsIf_H
#define __mvHwsXPcsIf_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <common/siliconIf/mvSiliconIf.h>
#include <pcs/mvHwsPcsIf.h>

/*******************************************************************************
* mvHwsGPcsIfInit
*
* DESCRIPTION:
*       Init GPCS configuration sequences and IF functions.
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
GT_STATUS mvHwsXPcsIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray);

/*******************************************************************************
* mvHwsXPcsReset
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
GT_STATUS mvHwsXPcsReset
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_RESET            action
);

/*******************************************************************************
* mvHwsXPcsMode
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
GT_STATUS mvHwsXPcsMode
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_UOPT                 numOfLanes
);

/*******************************************************************************
* mvHwsXPcsLoopback
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
GT_STATUS mvHwsXPcsLoopBack
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_LB_TYPE     lbType
);

/*******************************************************************************
* mvHwsXPcsLoopBackGet
*
* DESCRIPTION:
*       Return the PCS loop back mode state.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
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
GT_STATUS mvHwsXPcsLoopBackGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_LB_TYPE     *lbType
);

/*******************************************************************************
* mvHwsXPcsTestGenCfg
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
GT_STATUS mvHwsXPcsTestGenCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern
);

/*******************************************************************************
* mvHwsXPcsTestGenStatus
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
GT_STATUS mvHwsXPcsTestGenStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status    
);

/*******************************************************************************
* mvHwsXPcsActiveStatusGet
*
* DESCRIPTION:
*       Return number of PCS active lanes or 0, if current PCS isn't active.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXPcsActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_U32                  *numOfLanes
);

/*******************************************************************************
* mvHwsXPcsSignalDetectBypassEn
*
* DESCRIPTION:
*       Set all related PCS with Signal Detect Bypass value (1/0).
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
GT_STATUS mvHwsXPcsSignalDetectBypassEn
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 maskEn
);

/*******************************************************************************
* hwsXPcsIfClose
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
void hwsXPcsIfClose(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsXPcsIf_H */

