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
*       CGPCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/cgPcs/mvHwsCgPcsIf.h>
#include <pcs/cgPcs/mvHwsCgPcsDb.h>


static char* mvHwsPcsTypeGetFunc(void)
{
  return "CGPCS";
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
GT_STATUS mvHwsCgPcsIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsCgPcsSeqInit());
    }

    funcPtrArray[CGPCS].pcsResetFunc = NULL; /* all functionality moved to CG MAC */
    funcPtrArray[CGPCS].pcsModeCfgFunc = mvHwsCgPcsMode;
    funcPtrArray[CGPCS].pcsLbCfgFunc = mvHwsCgPcsLoopBack;
    funcPtrArray[CGPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    funcPtrArray[CGPCS].pcsSignalDetectMaskEn = mvHwsCgPcsSignalDetectMaskSet;
    funcPtrArray[CGPCS].pcsFecCfgFunc = mvHwsCgPcsFecConfig;
    funcPtrArray[CGPCS].pcsFecCfgGetFunc = mvHwsCgPcsFecConfigGet;
    funcPtrArray[CGPCS].pcsCheckGearBoxFunc = mvHwsCgPcsCheckGearBox;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

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
)
{
    MV_CFG_SEQ *seq;
    GT_U32 data, mask;
    GT_U32 accessAddr;
    GT_U32 baseAddr;
    GT_U32 unitIndex;

    /* EXTERNAL_CONTROL + 0x1000 * 16;  access XLG MAC 16 */
    mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);
    if (pcsNum == 0)
    {
        /* network port */
        accessAddr = baseAddr + EXTERNAL_CONTROL;
    }
    else
    {
        /* fabric ports */
        accessAddr = baseAddr + unitIndex * 16 + EXTERNAL_CONTROL;
    }
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, 1, 3));

    seq = &hwsCgPcsPscSeqDb[CGPCS_MODE_MISC_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    /* FEC config */
    data = 0;
    mask = (7 << 23);
    if (fecCtrl == FEC_ON)
    {
        data = (7 << 23);
    }
    /* write data to 0x1800000 [25:23]*/
    mvUnitInfoGet(devNum, CG_UNIT, &baseAddr, &unitIndex);
    accessAddr  = baseAddr + pcsNum * unitIndex + CG_CONTROL_0;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, data, mask));

    switch(numOfLanes)
    {
    case 10:
      seq = &hwsCgPcsPscSeqDb[CGPCS_MODE_10_LANE_SEQ];
      break;
    case 12:
      seq = &hwsCgPcsPscSeqDb[CGPCS_MODE_12_LANE_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
  
}


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
)
{
    MV_CFG_SEQ *seq;

    switch(lbType)
    {
    case DISABLE_LB:
      seq = &hwsCgPcsPscSeqDb[CGPCS_LPBK_NORMAL_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsCgPcsPscSeqDb[CGPCS_LPBK_TX2RX_SEQ];
      break; 
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
    
    return GT_OK;  
}

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
)
{
    GT_U32 regAddr, data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, CG_UNIT, &unitAddr, &unitIndex);

    regAddr = CG_CONTROL_0 + unitAddr + pcsNum*unitIndex;
    data = (maskEn == GT_TRUE) ? (1 << 21) : 0;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, (1 << 21)));

    return GT_OK;
}

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
)
{
    GT_U32 regAddr, data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, CG_UNIT, &unitAddr, &unitIndex);

    regAddr = CG_CONTROL_0 + unitAddr + pcsNum*unitIndex;
    data = (fecEn == GT_TRUE) ? (3 << 23) : 0;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, (3 << 23)));

    return GT_OK;
}

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
)
{
    GT_U32 regAddr, data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, CG_UNIT, &unitAddr, &unitIndex);

    regAddr = CG_CONTROL_0 + unitAddr + pcsNum*unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));

    *fecEn = ((data >> 23) & 3) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


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
)
{
    GT_U32 regAddr;
    GT_U32 data;
    GT_U32 mask;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, CG_UNIT, &unitAddr, &unitIndex);

    /* Check Gear Box Status */
    regAddr = unitAddr + pcsNum*unitIndex + CG_MULTI_LANE_ALIGN_STATUS1;
    genRegisterGet(devNum, portGroup, regAddr, &data, 0);
    /* gear box status can show lock in not used bits, so read only
       required bits */
    if (numOfLanes < 9)
    {
        mask = ((1 << numOfLanes) - 1)<<1;
        *laneLock = (data == mask) ? GT_TRUE : GT_FALSE;
    }
    else
    {
        if (data != 0xFF)
        {
            *laneLock = GT_FALSE;
        }
        else
        {
            regAddr = unitAddr + pcsNum*unitIndex + CG_MULTI_LANE_ALIGN_STATUS2;
            genRegisterGet(devNum, portGroup, regAddr, &data, 0);
            mask = ((1 << (numOfLanes-8)) - 1)<<1;
            *laneLock = (data == mask) ? GT_TRUE : GT_FALSE;
        }
    }

    return GT_OK;
}

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
void hwsCgPcsIfClose(void)
{
  hwsCgPcsSeqFree();
  hwsInitDone = GT_FALSE;
}
