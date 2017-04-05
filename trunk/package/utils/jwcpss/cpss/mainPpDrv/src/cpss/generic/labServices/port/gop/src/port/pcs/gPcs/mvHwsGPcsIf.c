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
* mvHwsGPcsIf.c
*
* DESCRIPTION:
*       GPCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/gPcs/mvHwsGPcsIf.h>
#include <pcs/gPcs/mvHwsGPcsDb.h>
#include <mac/geMac/mvHwsGeMacDb.h>


static char* mvHwsPcsTypeGetFunc(void)
{
  return "GPCS";
}
static GT_BOOL hwsInitDone = GT_FALSE;

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
GT_STATUS mvHwsGPcsIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsGPscSeqInit());
    }

    funcPtrArray[GPCS].pcsResetFunc = mvHwsGPcsReset;
    funcPtrArray[GPCS].pcsModeCfgFunc = mvHwsGPcsMode;
    funcPtrArray[GPCS].pcsLbCfgFunc = mvHwsGPcsLoopBack;
    funcPtrArray[GPCS].pcsTestGenFunc = mvHwsGPcsTestGenCfg;
    funcPtrArray[GPCS].pcsTestGenStatusFunc = mvHwsGPcsTestGenStatus;
    funcPtrArray[GPCS].pcsActiveStatusGetFunc = mvHwsGPcsActiveStatusGet;

    funcPtrArray[GPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsGPcsMode
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
GT_STATUS mvHwsGPcsMode
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_U32                  numOfLanes
)
{
    devNum      = devNum;
    portGroup   = portGroup;
    pcsNum      = pcsNum;
    fecCtrl     = fecCtrl;   
    numOfLanes  = numOfLanes;

  return GT_OK;
}

/*******************************************************************************
* mvHwsGPcsReset
*
* DESCRIPTION:
*       Set the selected PCS number to reset or exit from reset.
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
GT_STATUS mvHwsGPcsReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            resetMode
)
{
    MV_CFG_SEQ *seq;

    seq = (resetMode == RESET) ? &hwsGPscSeqDb[GPCS_RESET_SEQ] : &hwsGPscSeqDb[GPCS_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
    
    return GT_OK;
}

/*******************************************************************************
* mvHwsGPcsLoopback
*
* DESCRIPTION:
*       Set PCS loop back.
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
GT_STATUS mvHwsGPcsLoopBack
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_LB_TYPE     lbType
)
{
    MV_CFG_SEQ *seq;

    switch(lbType)
    {
    case DISABLE_LB:
      seq = &hwsGPscSeqDb[GPCS_LPBK_NORMAL_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsGPscSeqDb[GPCS_LPBK_TX2RX_SEQ];
      break; 
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
    
    return GT_OK;
}
  
/*******************************************************************************
* mvHwsGPcsTestGenCfg
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
GT_STATUS mvHwsGPcsTestGenCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern
)
{
    MV_CFG_SEQ *seq;

    CHECK_STATUS(mvHwsGPcsReset(devNum,portGroup,pcsNum,RESET));

    switch (pattern)
    {
    case TEST_GEN_PRBS7:
      seq = &hwsGPscSeqDb[GPCS_GEN_PRBS7_SEQ];
      break;
    case TEST_GEN_Normal:
      seq = &hwsGPscSeqDb[GPCS_GEN_NORMAL_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
  
    CHECK_STATUS(mvHwsGPcsReset(devNum,portGroup,pcsNum,UNRESET));

    return GT_OK;
}

/*******************************************************************************
* mvHwsGPcsTestGenStatus
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
GT_STATUS mvHwsGPcsTestGenStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status    
)
{
  GT_U32 data;
  GT_U32 accessAddr;
  GT_U32 baseAddr;
  GT_U32 unitIndex;

  pattern = pattern;

  if (status == 0)
  {
    return GT_BAD_PARAM;
  }

  mvUnitInfoGet(devNum, GEMAC_UNIT, &baseAddr, &unitIndex);
  accessAddr  = baseAddr + pcsNum * unitIndex;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+GIG_Port_PRBS_Status_Reg, &data, 0));
  status->checkerLock = data & 1; /* bit 0 */
      
  CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+GIG_Port_PRBS_Error_Cntr_Reg, &data, 0));
  status->totalErrors = data; 

  status->goodFrames = 0;

  return GT_OK;
}

/*******************************************************************************
* mvHwsGPcsActiveStatusGet
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
GT_STATUS mvHwsGPcsActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_U32                  *numOfLanes
)
{
    GT_U32 data;
    GT_U32 baseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    if (numOfLanes == NULL)
    {
      return GT_BAD_PARAM;
    }

    mvUnitInfoGet(devNum, GEMAC_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + pcsNum * unitIndex;

    *numOfLanes = 0;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + GIG_PORT_MAC_CONTROL_REGISTER2, &data, 0));
    /* check unreset bit status */
    if (!((data >> 3) & 1))
    {
        /* unit is under RESET */
        return GT_OK;
    }
    /* one active lane always */
    *numOfLanes = 1;

    return GT_OK;
}

/*******************************************************************************
* hwsGPcsIfClose
*
* DESCRIPTION:
*       Release all system resources allocated by GE PCS IF functions.
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
void hwsGPcsIfClose(void)
{
  hwsGPcsSeqFree();
  hwsInitDone = GT_FALSE;
}
