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
* mvHwsPcsIf.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/

#ifndef __mvHwsPcsIf_H
#define __mvHwsPcsIf_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* General H Files */
#include <private/mvHwsPortTypes.h>
#include <private/mvHwsPortMiscIf.h>

typedef GT_STATUS (*MV_HWS_PCS_CHECK_GEAR_BOX)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_U32                  numOfLanes,
    GT_BOOL                 *laneLock
);

typedef GT_STATUS (*MV_HWS_PCS_RESET_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            action
);

typedef GT_STATUS (*MV_HWS_PCS_MODE_CFG_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_U32                  numOfLanes
);

typedef GT_STATUS (*MV_HWS_PCS_LB_CFG_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_LB_TYPE     lbType
);

typedef GT_STATUS (*MV_HWS_PCS_LB_CFG_GET_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
	MV_HWS_PORT_LB_TYPE     *lbType
);

typedef GT_STATUS (*MV_HWS_PCS_TEST_GEN_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern
);

typedef GT_STATUS (*MV_HWS_PCS_TEST_GEN_STATUS_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status
);

typedef GT_STATUS (*MV_HWS_PCS_EXT_PLL_CFG_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum
);

typedef GT_STATUS (*MV_HWS_PCS_SIGNAL_DETECT_MASK_SET_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 maskEn
);

typedef GT_STATUS (*MV_HWS_PCS_FEX_CFG_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 fecEn
);

typedef GT_STATUS (*MV_HWS_PCS_FEX_CFG_GET_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 *fecEn
);

typedef GT_STATUS (*MV_HWS_PCS_ACTIVE_STATUS_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_U32                  *numOfLanes
);

typedef char* (*MV_HWS_PCS_TYPE_GET_FUNC_PTR)(void);

typedef struct
{
  MV_HWS_PCS_RESET_FUNC_PTR     pcsResetFunc;
  MV_HWS_PCS_MODE_CFG_FUNC_PTR	pcsModeCfgFunc;
  MV_HWS_PCS_LB_CFG_FUNC_PTR    pcsLbCfgFunc;
  MV_HWS_PCS_LB_CFG_GET_FUNC_PTR	pcsLbCfgGetFunc;
  MV_HWS_PCS_TEST_GEN_FUNC_PTR		pcsTestGenFunc;
  MV_HWS_PCS_TEST_GEN_STATUS_FUNC_PTR	pcsTestGenStatusFunc;
  MV_HWS_PCS_TYPE_GET_FUNC_PTR		pcsTypeGetFunc;
  MV_HWS_PCS_RESET_FUNC_PTR			pcsRxResetFunc;
  MV_HWS_PCS_EXT_PLL_CFG_FUNC_PTR	pcsExtPllCfgFunc;
  MV_HWS_PCS_SIGNAL_DETECT_MASK_SET_FUNC_PTR	pcsSignalDetectMaskEn;
  MV_HWS_PCS_FEX_CFG_FUNC_PTR		pcsFecCfgFunc;
  MV_HWS_PCS_FEX_CFG_GET_FUNC_PTR   pcsFecCfgGetFunc;
  MV_HWS_PCS_ACTIVE_STATUS_FUNC_PTR	pcsActiveStatusGetFunc;
  MV_HWS_PCS_CHECK_GEAR_BOX         pcsCheckGearBoxFunc;

}MV_HWS_PCS_FUNC_PTRS;

/*******************************************************************************
* hwsPcsIfInit
*
* DESCRIPTION:
*       Init all supported PCS types.
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
void hwsPcsIfInit(GT_U8 deviceNum);

/*******************************************************************************
* mvHwsPcsReset
*
* DESCRIPTION:
*       Set the selected PCS type and number to reset or exit from reset.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_RESET            action
);

/*******************************************************************************
* mvHwsPcsModeCfg
*
* DESCRIPTION:
*       Set the internal mux's to the required PCS in the PI.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsModeCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_U32                  numOfLanes
);

/*******************************************************************************
* mvHwsPcsLoopbackCfg
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
GT_STATUS mvHwsPcsLoopbackCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_LB_TYPE     lbType
);

/*******************************************************************************
* mvHwsPcsLoopbackCfgGet
*
* DESCRIPTION:
*       Return the PCS loop back mode state.
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
GT_STATUS mvHwsPcsLoopbackCfgGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_LB_TYPE     *lbType
);

/*******************************************************************************
* mvHwsPcsTestGenCfg
*
* DESCRIPTION:
*       Set PCS internal test generator mechanisms.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsTestGenCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern
);

/*******************************************************************************
* mvHwsPcsTestGenStatus
*
* DESCRIPTION:
*       Get PCS internal test generator mechanisms error counters and status.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsTestGenStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status
);

/*******************************************************************************
* mvHwsPcsRxReset
*
* DESCRIPTION:
*       Set the selected RX PCS type and number to reset or exit from reset.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsRxReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_RESET            action
);

/*******************************************************************************
* mvHwsPcsExtPllCfg
*
* DESCRIPTION:
*       Set the selected PCS type and number to external PLL mode.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsExtPllCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType
);

/*******************************************************************************
* mvHwsPcsSignalDetectMaskSet
*
* DESCRIPTION:
*       Set all related PCS with Signal Detect Mask value (1/0).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsSignalDetectMaskSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_BOOL                 maskEn
);

/*******************************************************************************
* mvHwsPcsFecConfig
*
* DESCRIPTION:
*       Configure FEC disable/enable on PCS.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsFecConfig
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_BOOL                 fecEn
);

/*******************************************************************************
* mvHwsPcsFecConfigGet
*
* DESCRIPTION:
*       Return FEC disable/enable status on PCS.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*
* OUTPUTS:
*       fecEn     - if true, enable FEC
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsFecConfigGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_BOOL                 *fecEn
);

/*******************************************************************************
* mvHwsPcsActiveStatusGet
*
* DESCRIPTION:
*       Return number of PCS active lanes or 0, if current PCS isn't active.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_U32                  *numOfLanes
);

/*******************************************************************************
* mvHwsPcsCheckGearBox
*
* DESCRIPTION:
*       Check Gear Box Status on related lanes (currently used only on MMPCS).
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
GT_STATUS mvHwsPcsCheckGearBox
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_U32                  numOfLanes,
    GT_BOOL                 *laneLock
);

/*******************************************************************************
* hwsPcsIfClose
*
* DESCRIPTION:
*       Free all resources allocated by supported PCS types.
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
void hwsPcsIfClose(void);

/*******************************************************************************
* hwsPcsGetFuncPtr
*
* DESCRIPTION:
*       Get function structure pointer.
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
GT_STATUS hwsPcsGetFuncPtr
(
	GT_U8                 devNum,
	MV_HWS_PCS_FUNC_PTRS  **hwsFuncsPtr
);

#ifdef __cplusplus
}
#endif

#endif /* __mvHwsPcsIf_H */

