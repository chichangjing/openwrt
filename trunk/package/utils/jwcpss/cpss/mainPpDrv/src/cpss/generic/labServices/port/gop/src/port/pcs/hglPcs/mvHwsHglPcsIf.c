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
* mvHwsHglPcsIf.c
*
* DESCRIPTION:
*       HGL PCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/hglPcs/mvHwsHglPcsIf.h>
#include <pcs/hglPcs/mvHwsHglPcsDb.h>
#include <pcs/xPcs/mvHwsXPcsDb.h>


static char* mvHwsPcsTypeGetFunc(void)
{
  return "HGLPCS";
}
static GT_BOOL hwsInitDone = GT_FALSE;
/*******************************************************************************
* mvHwsHglPcsIfInit
*
* DESCRIPTION:
*       Init HGL PCS configuration sequences and IF functions.
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
GT_STATUS mvHwsHglPcsIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsHglPscSeqInit());
    }

    funcPtrArray[HGLPCS].pcsResetFunc = mvHwsHglPcsReset;
    funcPtrArray[HGLPCS].pcsModeCfgFunc = mvHwsHglPcsMode;
    funcPtrArray[HGLPCS].pcsLbCfgFunc = mvHwsHglPcsLoopBack;
    funcPtrArray[HGLPCS].pcsTestGenStatusFunc = mvHwsHglPcsTestGenStatus;
    funcPtrArray[HGLPCS].pcsActiveStatusGetFunc = mvHwsHglPcsActiveStatusGet;

    funcPtrArray[HGLPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsHglPcsReset
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
GT_STATUS mvHwsHglPcsReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            resetMode
)
{
    MV_CFG_SEQ *seq;

    seq = (resetMode == RESET) ? &hwsXpcsPscSeqDb[XPCS_RESET_SEQ]: &hwsXpcsPscSeqDb[XPCS_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
    
    return GT_OK;
}

/*******************************************************************************
* mvHwsHglPcsMode
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
GT_STATUS mvHwsHglPcsMode
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_U32                  numOfLanes
)
{
    MV_CFG_SEQ *seq;

    fecCtrl = fecCtrl;

    seq = &hwsHglPscSeqDb[HGLPCS_MODE_MISC_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    switch(numOfLanes)
    {
    case 4:
      seq = &hwsHglPscSeqDb[HGLPCS_MODE_4_LANE_SEQ];
      break;
    case 6:
      seq = &hwsHglPscSeqDb[HGLPCS_MODE_6_LANE_SEQ];
      break; 
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
    
    return GT_OK;  
}

/*******************************************************************************
* mvHwsHglPcsLoopBack
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
GT_STATUS mvHwsHglPcsLoopBack
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
      seq = &hwsXpcsPscSeqDb[XPCS_LPBK_NORMAL_SEQ];
      break;
    case RX_2_TX_LB:
      seq = &hwsXpcsPscSeqDb[XPCS_LPBK_RX2TX_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsXpcsPscSeqDb[XPCS_LPBK_TX2RX_SEQ];
      break; 
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
    
    return GT_OK;  
}

/*******************************************************************************
* mvHwsHglPcsTestGenStatus
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
GT_STATUS mvHwsHglPcsTestGenStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status    
)
{
  GT_U32 data, tmp;
  GT_U32 accessAddr;
  GT_U32 baseAddr;
  GT_U32 unitIndex;

  if (status == 0)
  {
    return GT_BAD_PARAM;
  }
      
  mvUnitInfoGet(devNum, XPCS_UNIT, &baseAddr, &unitIndex);
  accessAddr = baseAddr + pcsNum * unitIndex;
  if ((pattern == TEST_GEN_CJPAT) || (pattern == TEST_GEN_CRPAT))
  {
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Tx_Packets_Cntr_LSB, &data, 0));
    status->goodFrames = data; 
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Tx_Packets_Cntr_MSB, &data, 0));
    status->goodFrames += (data << 16); 

    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_LSB_Lane0, &data, 0));
    status->totalErrors = data; 
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_MSB_Lane0, &data, 0));
    status->totalErrors += (data << 16); 

    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_LSB_Lane1, &data, 0));
    tmp = data; 
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_MSB_Lane1, &data, 0));
    tmp += (data << 16); 
    status->totalErrors += tmp;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_LSB_Lane2, &data, 0));
    tmp = data; 
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_MSB_Lane2, &data, 0));
    tmp += (data << 16); 
    status->totalErrors += tmp;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_LSB_Lane3, &data, 0));
    tmp = data; 
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_MSB_Lane3, &data, 0));
    tmp += (data << 16); 
    status->totalErrors += tmp;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_LSB_Lane4, &data, 0));
    tmp = data; 
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_MSB_Lane4, &data, 0));
    tmp += (data << 16); 
    status->totalErrors += tmp;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_LSB_Lane5, &data, 0));
    tmp = data; 
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_Rx_Bad_Packets_Cntr_MSB_Lane5, &data, 0));
    tmp += (data << 16); 
    status->totalErrors += tmp;

    status->checkerLock = 0;
  }
  else if ((pattern == TEST_GEN_PRBS7) || (pattern == TEST_GEN_PRBS23))
  {
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane0, &data, 0));
    status->totalErrors = data; 
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane1, &data, 0));
    status->totalErrors += data;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane2, &data, 0));
    
    status->totalErrors += data;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane3, &data, 0));
    status->totalErrors += data;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane4, &data, 0));
    status->totalErrors += data;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane5, &data, 0));
    status->totalErrors += data;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane0, &data, 0));
    status->checkerLock = data & 1;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane1, &data, 0));
    status->checkerLock += (data & 1) << 1;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane2, &data, 0));
    status->checkerLock += (data & 1) << 2;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane3, &data, 0));
    status->checkerLock += (data & 1) << 3;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane4, &data, 0));
    status->checkerLock += (data & 1) << 4;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr+XPCS_PRBS_Error_Cntr_Lane5, &data, 0));
    status->checkerLock += (data & 1) << 5;
  }
  else
  {
    return GT_NOT_SUPPORTED;
  }
  return GT_OK;
}

/*******************************************************************************
* mvHwsHglPcsActiveStatusGet
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
GT_STATUS mvHwsHglPcsActiveStatusGet
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

    mvUnitInfoGet(devNum, XPCS_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + pcsNum * unitIndex;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + XPCS_Global_Configuration_Reg0, &data, 0));

    *numOfLanes = 0;
    /* check unreset bit status */
    if (!(data & 1))
    {
        /* unit is under RESET */
        return GT_OK;
    }
    /* get number of active lanes */
    switch ((data >> 5) & 3)
    {
    case 0:
        *numOfLanes = 1;
        break;
    case 1:
        *numOfLanes = 2;
        break;
    case 2:
        *numOfLanes = 4;
        break;
    case 3:
        *numOfLanes = 6;
        break;
    default:
        *numOfLanes = 0;
        break;
    }

    return GT_OK;
}

/*******************************************************************************
* hwsHglPcsIfClose
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
void hwsHglPcsIfClose(void)
{
  hwsHglPcsSeqFree();
  hwsInitDone = GT_FALSE;
}
