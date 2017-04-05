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
* mvHwsIlknPcsIf.h
*
* DESCRIPTION:
*       InterLaken unit PCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/ilknPcs/mvHwsIlknPcsIf.h>
#include <pcs/ilknPcs/mvHwsIlknPcsDb.h>


#ifndef CO_CPU_RUN
static char* mvHwsPcsTypeGetFunc(void)
{
  return "ILKN_PCS";
}
#endif
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
GT_STATUS mvHwsIlknPcsIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsIlknPcsSeqInit());
    }

    funcPtrArray[INTLKN_PCS].pcsResetFunc = mvHwsIlknPcsReset;
    funcPtrArray[INTLKN_PCS].pcsModeCfgFunc = mvHwsIlknPcsMode;
#ifndef CO_CPU_RUN
    funcPtrArray[INTLKN_PCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
#endif
    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsIlknPcsReset
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
GT_STATUS mvHwsIlknPcsReset
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_RESET            action
)
{
    MV_CFG_SEQ *seq;

    seq = (action == RESET) ? &hwsIlknPcsPscSeqDb[ILKNPCS_RESET_SEQ]: &hwsIlknPcsPscSeqDb[ILKNPCS_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;

}

/*******************************************************************************
* mvHwsIlknPcsMode
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
GT_STATUS mvHwsIlknPcsMode
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_UOPT                 numOfLanes
)
{
	GT_UREG_DATA regData; 
	GT_UOPT xlgNum;
    MV_CFG_SEQ *seq;

    fecCtrl = fecCtrl;

    
	if(numOfLanes == 4 || numOfLanes == 8)
	{
		/* for BC2, the mux value should be as follows 
		   TBD - should this function be separated to a diffeerent component */
		regData = 1;
		xlgNum = 64;
	}
	else
	{
		regData = 2;
		xlgNum = 16;
	}

	/* Set MUX to ILKN */
	CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, xlgNum, EXTERNAL_CONTROL, regData, 3));

    seq = &hwsIlknPcsPscSeqDb[ILKNPCS_MODE_MISC_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
    switch(numOfLanes)
    {
	case 4:
      seq = &hwsIlknPcsPscSeqDb[ILKNPCS_MODE_4_LANE_SEQ];
      break;
	case 8:
      seq = &hwsIlknPcsPscSeqDb[ILKNPCS_MODE_8_LANE_SEQ];
      break;
    case 12:
      seq = &hwsIlknPcsPscSeqDb[ILKNPCS_MODE_12_LANE_SEQ];
      break;
    case 16:
      seq = &hwsIlknPcsPscSeqDb[ILKNPCS_MODE_16_LANE_SEQ];
      break;
    case 24:
      seq = &hwsIlknPcsPscSeqDb[ILKNPCS_MODE_24_LANE_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;

}

#ifndef CO_CPU_RUN
/*******************************************************************************
* hwsIlknPcsIfClose
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
void hwsIlknPcsIfClose(void)
{
  hwsIlknPcsSeqFree();
  hwsInitDone = GT_FALSE;
}
#endif
