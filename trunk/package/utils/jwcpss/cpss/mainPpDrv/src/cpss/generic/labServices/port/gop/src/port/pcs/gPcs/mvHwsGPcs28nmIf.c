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
* mvHwsGPcs28nmIf.c
*
* DESCRIPTION:
*       GPCS interface API (for bobcat2)
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/gPcs/mvHwsGPcs28nmIf.h>
#include <pcs/gPcs/mvHwsGPcsDb.h>
#include <mac/geMac/mvHwsGeMacDb.h>
#include <private/mvPortModeElements.h>
#include <private/mvHwsPortPrvIf.h>

#ifndef CO_CPU_RUN
#include <pcs/gPcs/mvHwsGPcsIf.h>
#endif

#ifndef CO_CPU_RUN
static char* mvHwsPcsTypeGetFunc(void)
{
  return "GPCS_28nm";
}
#endif

static GT_BOOL hwsInitDone = GT_FALSE;
/*******************************************************************************
* mvHwsGPcs28nmIfInit
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
GT_STATUS mvHwsGPcs28nmIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsGPscSeqInit());
    }

    funcPtrArray[GPCS].pcsResetFunc = mvHwsGPcs28nmReset;
    funcPtrArray[GPCS].pcsModeCfgFunc = mvHwsGPcs28nmModeCfg;
    funcPtrArray[GPCS_NET].pcsResetFunc = mvHwsGPcs28nmReset;
    funcPtrArray[GPCS_NET].pcsModeCfgFunc = mvHwsGPcsNet28nmModeCfg;
    funcPtrArray[QSGMII_PCS].pcsResetFunc = mvHwsGPcs28nmReset;
    funcPtrArray[QSGMII_PCS].pcsModeCfgFunc = mvHwsGPcsNet28nmModeCfg;

#ifndef CO_CPU_RUN
    funcPtrArray[GPCS].pcsLbCfgFunc = mvHwsGPcs28nmLoopBack;
    funcPtrArray[GPCS].pcsLbCfgGetFunc = mvHwsGPcs28nmLoopBackGet;
    funcPtrArray[GPCS].pcsSignalDetectMaskEn = mvHwsGPcs28nmSignalDetectMaskSet;
    funcPtrArray[GPCS].pcsTestGenStatusFunc = mvHwsGPcs28nmTestGenStatus;
    funcPtrArray[GPCS].pcsTestGenFunc = mvHwsGPcs28nmTestGenCfg;
    funcPtrArray[GPCS].pcsActiveStatusGetFunc = mvHwsGPcs28nmActiveStatusGet;
    funcPtrArray[GPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    funcPtrArray[GPCS_NET].pcsLbCfgFunc = mvHwsGPcs28nmLoopBack;
    funcPtrArray[GPCS_NET].pcsLbCfgGetFunc = mvHwsGPcs28nmLoopBackGet;
    funcPtrArray[GPCS_NET].pcsSignalDetectMaskEn = mvHwsGPcs28nmSignalDetectMaskSet;
    funcPtrArray[GPCS_NET].pcsTestGenStatusFunc = mvHwsGPcs28nmTestGenStatus;
    funcPtrArray[GPCS_NET].pcsTestGenFunc = mvHwsGPcs28nmTestGenCfg;
    funcPtrArray[GPCS_NET].pcsActiveStatusGetFunc = mvHwsGPcs28nmActiveStatusGet;
    funcPtrArray[GPCS_NET].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    funcPtrArray[QSGMII_PCS].pcsLbCfgFunc = mvHwsGPcs28nmLoopBack;
    funcPtrArray[QSGMII_PCS].pcsLbCfgGetFunc = mvHwsGPcs28nmLoopBackGet;
    funcPtrArray[QSGMII_PCS].pcsSignalDetectMaskEn = mvHwsQsgmiiPcs28nmSignalDetectMaskSet;
    funcPtrArray[QSGMII_PCS].pcsTestGenStatusFunc = mvHwsGPcs28nmTestGenStatus;
    funcPtrArray[QSGMII_PCS].pcsTestGenFunc = mvHwsGPcs28nmTestGenCfg;
    funcPtrArray[QSGMII_PCS].pcsActiveStatusGetFunc = mvHwsGPcs28nmActiveStatusGet;
    funcPtrArray[QSGMII_PCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
#endif

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsGPcsNet28nmModeCfg
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
GT_STATUS mvHwsGPcsNet28nmModeCfg
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_UOPT                 numOfLanes
)
{
    devNum      = devNum;
    portGroup   = portGroup;
    pcsNum      = pcsNum;
    fecCtrl     = fecCtrl;
    numOfLanes  = numOfLanes;

    /* EXTERNAL_CONTROL - Select recovered clock 0 per pair of cores */
    /*CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, pcsNum, EXTERNAL_CONTROL, 0, 3));*/

    return GT_OK;
}

/*******************************************************************************
* mvHwsGPcs28nmModeCfg
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
GT_STATUS mvHwsGPcs28nmModeCfg
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_UOPT                 numOfLanes
)
{
	devNum      = devNum;
	portGroup   = portGroup;
	pcsNum      = pcsNum;
    fecCtrl     = fecCtrl;
    numOfLanes  = numOfLanes;

    /* EXTERNAL_CONTROL - Select recovered clock 0 per pair of cores */
    /*CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, pcsNum, EXTERNAL_CONTROL, 0, 3));*/

    return GT_OK;
}

/*******************************************************************************
* mvHwsGPcs28nmReset
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
GT_STATUS mvHwsGPcs28nmReset
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_RESET            resetMode
)
{
    MV_CFG_SEQ *seq;

    seq = (resetMode == RESET) ? &hwsGPscSeqDb[GPCS_RESET_SEQ] : &hwsGPscSeqDb[GPCS_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsGPcs28nmLoopback
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
GT_STATUS mvHwsGPcs28nmLoopBack
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
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
* mvHwsGPcs28nmLoopBackGet
*
* DESCRIPTION:
*       Get the PCS loop back mode state.
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
GT_STATUS mvHwsGPcs28nmLoopBackGet
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  pcsNum,
	MV_HWS_PORT_LB_TYPE     *lbType
)
{
	GT_UREG_DATA	data;

	CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, pcsNum, GIG_PORT_MAC_CONTROL_REGISTER1, &data, (0x1 << 6)));

	switch(data>>6)
	{
	case 0:
		*lbType = DISABLE_LB;
		break;
	case 1:
		*lbType = TX_2_RX_LB;
		break;
	default:
		return GT_NOT_SUPPORTED;
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsGPcs28nmSerdesSignalDetectMaskSet
*
* DESCRIPTION:
*       Set all related PCS with Signal Detect Mask value (1/0).
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - port number
*       PortMode   - port mode
*       maskEn     - if true, enable signal detect mask bit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsGPcs28nmSerdesSignalDetectMaskSet
(
    GT_U8                   devNum,
	GT_UOPT                 portGroup,
    GT_UOPT                 phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 maskEn
)
{
	MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 curLanesList[1];

	/* signal detect is done in the Serdes */

	/* we assume that PCS num is the correlated to port num*/
	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);


	if(curPortParams->numOfActLanes != 1)
	{
		return GT_BAD_SIZE;
	}

	CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));


	/* set bit 0 */
	CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, curLanesList[0] & 0xFFFF,
                                        SERDES_EXTERNAL_CONFIGURATION_2, (maskEn << 8), (1 << 8)));

    return GT_OK;
}

/*******************************************************************************
* mvHwsGPcs28nmSignalDetectMaskSet
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
GT_STATUS mvHwsGPcs28nmSignalDetectMaskSet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    GT_BOOL                 maskEn
)
{
	/* avoid warnings */
	devNum = devNum;
	portGroup = portGroup;
	pcsNum = pcsNum;
	maskEn = maskEn;

	/* we assume that PCS num is the correlated to port num*/
	return GT_NOT_SUPPORTED;
	/*return mvHwsGPcs28nmSerdesSignalDetectMaskSet(devNum, portGroup, pcsNum, GPCS, maskEn);*/
}

/*******************************************************************************
* mvHwsQsgmiiPcs28nmSignalDetectMaskSet
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
GT_STATUS mvHwsQsgmiiPcs28nmSignalDetectMaskSet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    GT_BOOL                 maskEn
)
{
	/* we assume that PCS num is the correlated to port num*/
	return mvHwsGPcs28nmSerdesSignalDetectMaskSet(devNum, portGroup, pcsNum, QSGMII, maskEn);
}

/*******************************************************************************
* mvHwsGPcs28nmTestGenCfg
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
GT_STATUS mvHwsGPcs28nmTestGenCfg
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
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

    CHECK_STATUS(mvHwsGPcs28nmReset(devNum,portGroup,pcsNum,UNRESET));

    return GT_OK;
}

/*******************************************************************************
* mvHwsGPcs28nmTestGenStatus
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
GT_STATUS mvHwsGPcs28nmTestGenStatus
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status
)
{
  GT_UREG_DATA data;

  pattern = pattern;

  if (status == 0)
  {
    return GT_BAD_PARAM;
  }

  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, pcsNum, GIG_Port_PRBS_Status_Reg, &data, 0));
  status->checkerLock = data & 1; /* bit 0 */

  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, pcsNum, GIG_Port_PRBS_Error_Cntr_Reg, &data, 0));
  status->totalErrors = data;

  status->goodFrames = 0;

  return GT_OK;
}

/*******************************************************************************
* mvHwsGPcs28nmActiveStatusGet
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
GT_STATUS mvHwsGPcs28nmActiveStatusGet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    GT_UOPT                 *numOfLanes
)
{
    GT_UREG_DATA data;

    if (numOfLanes == NULL)
    {
      return GT_BAD_PARAM;
    }

    *numOfLanes = 0;
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, pcsNum, GIG_PORT_MAC_CONTROL_REGISTER2, &data, 0));
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
#endif

/*******************************************************************************
* hwsGPcs28nmIfClose
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
void hwsGPcs28nmIfClose(void)
{
#ifndef CO_CPU_RUN
  hwsGPcsSeqFree();
#endif
  hwsInitDone = GT_FALSE;
}
