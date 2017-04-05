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
* mvHwsMMPcsIf_v3.c
*
* DESCRIPTION:
*       MMPCS V3.0 interface API for puma3B0
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <port/private/mvPortModeElements.h>
#include <pcs/mmPcs/mvHwsMMPcs28nmIf.h>
#include <pcs/mmPcs/mvHwsMMPcs28nmDb.h>

#ifdef RUN_PCS_DEBUG
#include <gtOs/gtOsIo.h>
#define DEBUG_PCS(s)              osPrintf s
#else
#define DEBUG_PCS(s)
#endif

#ifndef CO_CPU_RUN
static char* mvHwsPcsTypeGetFunc(void)
{
    return "MMPCS_28nm";
}
#endif

static GT_BOOL hwsInitDone = GT_FALSE;


static GT_STATUS mvHwsMMPcs28nmPcsNumGet
(
	GT_U8		devNum,
	GT_U32		firstPcsNum,
	GT_U8		pcsIndex,	
	GT_UOPT		*activePcsNum
);

/*******************************************************************************
* mvHwsMMPcs28nmIfInit
*
* DESCRIPTION:
*       Init MMPCS configuration sequences and IF functions.
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
GT_STATUS mvHwsMMPcs28nmIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsMMPscSeqInit());
        CHECK_STATUS(hwsMMPsc28nmSeqInit());
    }

#ifndef CO_CPU_RUN
    funcPtrArray[MMPCS].pcsTestGenFunc = mvHwsMMPcs28nmTestGenCfg;
    funcPtrArray[MMPCS].pcsTestGenStatusFunc = mvHwsMMPcs28nmTestGenStatus;
    funcPtrArray[MMPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    funcPtrArray[MMPCS].pcsFecCfgFunc = mvHwsMMPcs28nmFecConfig;
    funcPtrArray[MMPCS].pcsLbCfgFunc = mvHwsMMPcs28nmLoopBack;
    funcPtrArray[MMPCS].pcsLbCfgGetFunc = mvHwsMMPcs28nmLoopBackGet;
    funcPtrArray[MMPCS].pcsFecCfgGetFunc = mvHwsMMPcs28nmFecConfigGet;
    funcPtrArray[MMPCS].pcsActiveStatusGetFunc = mvHwsMMPcs28nmActiveStatusGet;
    funcPtrArray[MMPCS].pcsCheckGearBoxFunc = mvHwsMMPcs28nmCheckGearBox;
#endif

    funcPtrArray[MMPCS].pcsRxResetFunc = mvHwsMMPcs28nmRxReset;
    funcPtrArray[MMPCS].pcsExtPllCfgFunc = mvMmPcs28nmExtPllCfg;
    funcPtrArray[MMPCS].pcsResetFunc = mvHwsMMPcs28nmReset;
    funcPtrArray[MMPCS].pcsModeCfgFunc = mvHwsMMPcs28nmMode;
    funcPtrArray[MMPCS].pcsSignalDetectMaskEn = mvHwsMMPcs28nmSignalDetectMaskSet;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmRxReset
*
* DESCRIPTION:
*       Set the selected RX PCS type and number to reset or exit from reset.
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
GT_STATUS mvHwsMMPcs28nmRxReset
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_RESET            action
)
{
    GT_UREG_DATA data;
    GT_U8   i;
    GT_U8   countVal;
	GT_UOPT activePcsNum;

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS40G_COMMON_CONTROL, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    if (action == RESET)
    {
        /* reset PCS Rx only */
        /* on first lane reset bits 5,4,3 */
        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS_RESET_REG, 0, 0x38));
        for (i = 1; i < countVal; i++)
        {
			/* Return the current active PCS number */
			CHECK_STATUS(mvHwsMMPcs28nmPcsNumGet(devNum, pcsNum, i, &activePcsNum));

			/* on other lanes reset bits 4,3 only */
            CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, activePcsNum, PCS_RESET_REG, 0, 0x18));
        }
    }
    else
    {
        /* un reset PCS Rx only */
        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS_RESET_REG, (1 << 5), (1 << 5)));
        for (i = 0; i < countVal; i++)
        {
			/* Return the current active PCS number */
			CHECK_STATUS(mvHwsMMPcs28nmPcsNumGet(devNum, pcsNum, i, &activePcsNum));

			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, activePcsNum, PCS_RESET_REG, (1 << 4), (1 << 4)));
        }
        for (i = 0; i < countVal; i++)
        {
			/* Return the current active PCS number */
			CHECK_STATUS(mvHwsMMPcs28nmPcsNumGet(devNum, pcsNum, i, &activePcsNum));

			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, activePcsNum, PCS_RESET_REG, (1 << 3), (1 << 3)));
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmReset
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
GT_STATUS mvHwsMMPcs28nmReset
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_RESET            action
)
{
    GT_UREG_DATA data;
    GT_UREG_DATA countVal;
    MV_CFG_SEQ   *seq;

    /* read PCS mode */
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS40G_COMMON_CONTROL, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

   if (FULL_RESET == action)
   {
        switch(countVal)
        {
            case 1:
                seq = &hwsMmPsc28nmSeqDb[MMPCS_RESET_1_LANE_SEQ];
                break;
            case 2:
				if((HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0) || (pcsNum != 29))
				{
					seq = &hwsMmPsc28nmSeqDb[MMPCS_POWER_DOWN_2_LANE_SEQ];
				}
				else
				{
					seq = &hwsMmPsc28nmSeqDb[MMPCS_FABRIC_POWER_DOWN_2_LANE_SEQ];
				}
                break;
            case 4:
                seq = &hwsMmPsc28nmSeqDb[MMPCS_POWER_DOWN_4_LANE_SEQ];
                break;
            default:
                return GT_BAD_PARAM;
        }
   }
   else if (action == RESET)
   {
        switch(countVal)
        {
            case 1:
                seq = &hwsMmPsc28nmSeqDb[MMPCS_RESET_1_LANE_SEQ];
                break;
            case 2:
				if((HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0) || (pcsNum != 29))
				{
					seq = &hwsMmPsc28nmSeqDb[MMPCS_RESET_2_LANE_SEQ];
				}
				else
				{
					seq = &hwsMmPsc28nmSeqDb[MMPCS_FABRIC_RESET_2_LANE_SEQ];
				}
                break;
            case 4:
                seq = &hwsMmPsc28nmSeqDb[MMPCS_RESET_4_LANE_SEQ];
                break;
            default:
                return GT_BAD_PARAM;
        }
    }
    else
    {
        switch(countVal)
        {
            case 1:
                if (HWS_DEV_GOP_REV(devNum) < GOP_28NM_REV3)
                {
                    seq = &hwsMmPsc28nmSeqDb[MMPCS_UNRESET_1_LANE_SEQ];
                }
                else
                {
                    seq = &hwsMmPsc28nmSeqDb[MMPCS_GOP_REV3_UNRESET_1_LANE_SEQ];
                }
                break;
            case 2:
                if((HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0) || (pcsNum != 29))
                {
                    if (HWS_DEV_GOP_REV(devNum) < GOP_28NM_REV3)
                    {
                        seq = &hwsMmPsc28nmSeqDb[MMPCS_UNRESET_2_LANE_SEQ];
                    }
                    else
                    {
                        seq = &hwsMmPsc28nmSeqDb[MMPCS_GOP_REV3_UNRESET_2_LANE_SEQ];
                    }
                }
                else
                {
                    seq = &hwsMmPsc28nmSeqDb[MMPCS_FABRIC_UNRESET_2_LANE_SEQ];
                }
                break;
            case 4:
                if (HWS_DEV_GOP_REV(devNum) < GOP_28NM_REV3)
                {
                    seq = &hwsMmPsc28nmSeqDb[MMPCS_UNRESET_4_LANE_SEQ];
                }
                else
                {
                    seq = &hwsMmPsc28nmSeqDb[MMPCS_GOP_REV3_UNRESET_4_LANE_SEQ];
                }
                break;
            default:
                return GT_BAD_PARAM;
        }
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}


/*******************************************************************************
* mvHwsMMPcs28nmMode
*
* DESCRIPTION:
*       Set the internal mux's to the required PCS in the PI.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       fecCtrl   - FEC control
*       numOfLanes - number of lanes aggregated in PCS
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMMPcs28nmMode
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_UOPT                 numOfLanes
)
{
    MV_CFG_SEQ *seq;
    GT_UREG_DATA mask;
    GT_UOPT activePcsNum;
	GT_UOPT i;

    /* clear register used to prevent WA function execution */
	if (HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0)
	{
		/* EXTERNAL_CONTROL - Select recovered clock 0 per pair of cores.
		   In Alleycat3 device this setting is not relevant due to Mux in DMA unit between regular and extended Stacking ports */
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, pcsNum, EXTERNAL_CONTROL, 0, 3));

		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, pcsNum, FEC_DEC_DMA_WR_DATA, 0, 0));
	}

    /* Configure FEC disable/enable on PCS */
	CHECK_STATUS(mvHwsMMPcs28nmFecConfig(devNum, portGroup, pcsNum, (fecCtrl == FEC_ON) ? GT_TRUE : GT_FALSE));

	mask = (1 << 10) + (1 << 7); /* always set bits 7 and 10 to 0; */
	for (i = 0; i < numOfLanes; i++)
	{
		/* Return the current active PCS number */
		CHECK_STATUS(mvHwsMMPcs28nmPcsNumGet(devNum, pcsNum, (GT_U8)i, &activePcsNum));

		/* always set bit 7 to 0; */
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, activePcsNum, PCS40G_COMMON_CONTROL, 0, mask));
	}

    /* reduce the probability of the link loss to about once in ~1000 times */
    for (i = 0; i < numOfLanes; i++)
    {
        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, pcsNum, CHANNEL0_RX_GB_FIFO_CONFIGURATION + i*4, 0x18, 0));
    }

    switch (numOfLanes)
    {
        case 1:
            seq = &hwsMmPsc28nmSeqDb[MMPCS_MODE_1_LANE_SEQ];
            break;
        case 2:
			if((HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0) || (pcsNum != 29))
			{
				seq = &hwsMmPsc28nmSeqDb[MMPCS_MODE_2_LANE_SEQ];
			}
			else
			{
				seq = &hwsMmPsc28nmSeqDb[MMPCS_FABRIC_MODE_2_LANE_SEQ];
			}
            
            break;
        case 4:
            seq = &hwsMmPsc28nmSeqDb[MMPCS_MODE_4_LANE_SEQ];
            break;
        default:
            return GT_NOT_SUPPORTED;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

GT_STATUS mvMmPcs28nmExtPllCfg
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum
)
{

    CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS40G_COMMON_CONTROL, (1 << 14), (1 << 14)));

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmSignalDetectMaskSet
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
GT_STATUS mvHwsMMPcs28nmSignalDetectMaskSet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    GT_BOOL                 maskEn
)
{
    GT_UREG_DATA data;
    GT_U8        countVal, i;
	GT_UOPT      activePcsNum;

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS40G_COMMON_CONTROL, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    data = (maskEn == GT_TRUE) ? (1 << 14) : 0;
    for (i = 0; i < countVal; i++)
    {
		/* Return the current active PCS number */
		CHECK_STATUS(mvHwsMMPcs28nmPcsNumGet(devNum, pcsNum, i, &activePcsNum));

        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, activePcsNum, CHANNEL_CONFIGURATION, data, (1 << 14)));
    }

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsMMPcs28nmCheckGearBox
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
GT_STATUS mvHwsMMPcs28nmCheckGearBox
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    GT_UOPT                 numOfLanes,
    GT_BOOL                 *laneLock
)
{
    GT_UREG_DATA data;
    GT_UREG_DATA mask;

    /* Check Gear Box Status */
    /* gear box status can show lock in not used bits, so read only required bits */
    mask = ((1 << numOfLanes) - 1)<<1;
    genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, GEARBOX_STATUS, &data, mask);

    *laneLock = (data == mask) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmLoopback
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
GT_STATUS mvHwsMMPcs28nmLoopBack
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
      seq = &hwsMmPscSeqDb[MMPCS_LPBK_NORMAL_SEQ];
      break;
    case RX_2_TX_LB:
      seq = &hwsMmPscSeqDb[MMPCS_LPBK_RX2TX_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsMmPscSeqDb[MMPCS_LPBK_TX2RX_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmLoopBackGet
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
GT_STATUS mvHwsMMPcs28nmLoopBackGet
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  pcsNum,
	MV_HWS_PORT_LB_TYPE     *lbType
)
{
	GT_UREG_DATA	data;

	CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS40G_COMMON_CONTROL, &data, (0x3 << 3)));

	switch(data>>3)
	{
	case 0:
		*lbType = DISABLE_LB;
		break;
	case 1:
		*lbType = TX_2_RX_LB;
		break;
	case 2:
		*lbType = RX_2_TX_LB;
		break;
	default:
		return GT_NOT_SUPPORTED;
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmTestGenCfg
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
GT_STATUS mvHwsMMPcs28nmTestGenCfg
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern
)
{
    MV_CFG_SEQ *seq;

    CHECK_STATUS(mvHwsMMPcs28nmReset(devNum,portGroup,pcsNum,RESET));

    switch (pattern)
    {
    case TEST_GEN_KRPAT:
      seq = &hwsMmPscSeqDb[MMPCS_GEN_KRPAT_SEQ];
      break;
    case TEST_GEN_Normal:
      seq = &hwsMmPscSeqDb[MMPCS_GEN_NORMAL_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    CHECK_STATUS(mvHwsMMPcs28nmReset(devNum,portGroup,pcsNum,UNRESET));

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmTestGenStatus
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
GT_STATUS mvHwsMMPcs28nmTestGenStatus
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN  pattern,
    MV_HWS_TEST_GEN_STATUS        *status
)
{
  GT_UREG_DATA data;

  pattern = pattern;

  if (status == 0)
  {
    return GT_BAD_PARAM;
  }

  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, FEC_DEC_STATUS_REG, &data, 0));
  status->checkerLock = (data >> 3) & 1; /* bit 3 */

  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, TEST_PATTERN_ERROR_COUNT, &data, 0));
  status->totalErrors = data;

  status->goodFrames = 0;

  return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmFecConfig
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
GT_STATUS mvHwsMMPcs28nmFecConfig
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    GT_BOOL                 fecEn
)
{
    GT_UREG_DATA data;
    GT_U8        countVal, i;
	GT_UOPT		 activePcsNum;

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS40G_COMMON_CONTROL, &data, 0));
    countVal = (data >> 8) & 0x3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    data = (fecEn == GT_TRUE) ? (1 << 10) : 0;
    for (i = 0; i < countVal; i++)
    {
		/* Return the current active PCS number */
		CHECK_STATUS(mvHwsMMPcs28nmPcsNumGet(devNum, pcsNum, i, &activePcsNum));

		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, MMPCS_UNIT, activePcsNum, PCS40G_COMMON_CONTROL, data, (1 << 10)));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmFecConfigGet
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
GT_STATUS mvHwsMMPcs28nmFecConfigGet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    GT_BOOL                 *fecEn
)
{
    GT_UREG_DATA data;

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS40G_COMMON_CONTROL, &data, 0));

    *fecEn = ((data >> 10) & 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmActiveStatusGet
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
GT_STATUS mvHwsMMPcs28nmActiveStatusGet
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
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS_RESET_REG, &data, 0));
    /* check unreset bit status */
    /*if (!((data >> 6) & 1))*/
	if (!(data & 0x7))
    {
        /* unit is under RESET */
        return GT_OK;
    }

    /* get number of active lanes */
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, MMPCS_UNIT, pcsNum, PCS40G_COMMON_CONTROL, &data, 0));
    switch ((data >> 8) & 3)
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
    default:
        *numOfLanes = 0;
        break;
    }

    return GT_OK;
}
#endif

/*******************************************************************************
* hwsMMPcs28nmIfClose
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
void hwsMMPcs28nmIfClose(void)
{
#ifndef CO_CPU_RUN
  hwsMMPcsSeqFree();
  hwsMMPcs28nmSeqFree();
#endif
  hwsInitDone = GT_FALSE;
}

GT_STATUS mvHwsMmPcs28nm40GConnectWa
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum
)
{
    devNum = devNum;
    portGroup = portGroup;
    pcsNum = pcsNum;

    return GT_OK;
}

GT_STATUS mvHwsMmPcs28nm40GBackWa
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum
)
{
    GT_U32 regAddr, data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 countVal;
    GT_U32 gearVal;

    mvUnitExtInfoGet(devNum, MMPCS_UNIT, pcsNum, &unitAddr, &unitIndex , &pcsNum);

    regAddr = FEC_DEC_DMA_WR_DATA + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    if (data != 0x8000)
    {
        /* unit not initalize or under TRx training, delete */
        DEBUG_PCS(("\nRun mvHwsMmPcs40GBackWa PORT NOT CREATED on device %d, pcs %d", devNum, pcsNum));
        return GT_OK;
    }
    hwsOsTimerWkFuncPtr(2);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    if (countVal != 4)
    {
        /* not 40G */
        DEBUG_PCS(("\nRun mvHwsMmPcs40GBackWa PORT NOT CREATED (4) on device %d, pcs %d", devNum, pcsNum));
        return GT_OK;
    }

    DEBUG_PCS(("\nRun mvHwsMmPcs40GBackWa on device %d, pcs %d", devNum, pcsNum));
    /* Check Gear Box Status */
    regAddr = unitAddr + pcsNum*unitIndex + GEARBOX_STATUS;
    genRegisterGet(devNum, portGroup, regAddr, &data, 0);
    gearVal = (data >> 1) & 0xF;
    if (gearVal != 0xF)
    {
        DEBUG_PCS(("\nCheck Gear Box Status on device %d, pcs %d (data 0x%x)", devNum, pcsNum, data));
        if (gearVal == 0)
        {
            /* no connector in port */
            return GT_OK;
        }
        else
        {
          return GT_FAIL;
        }
    }
    else
    {
        regAddr = PCS40G_COMMON_STATUS + unitAddr + pcsNum*unitIndex;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        if (!(data & 1))
        {
            DEBUG_PCS(("\nRun RESET on device %d, pcs %d", devNum, pcsNum));
            /* one SD sq_detect = 1 perform pcs reset sequence */
            mvHwsMMPcs28nmReset(devNum,portGroup,pcsNum,RESET);
            hwsOsTimerWkFuncPtr(1);
            mvHwsMMPcs28nmReset(devNum,portGroup,pcsNum,UNRESET);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcs28nmPcsNumGet
*
* DESCRIPTION:
*       Return the current active PCS number.
*
* INPUTS:
*       devNum    - system device number
*       pcsNum    - physical PCS number
*       laneNum   - lane number in PCS
*
* OUTPUTS:
*       activePcsNum - Current active PCS number
*
* RETURNS:
*       None.
*
*******************************************************************************/
static GT_STATUS mvHwsMMPcs28nmPcsNumGet
(
	GT_U8		devNum,
	GT_U32		firstPcsNum,
	GT_U8		pcsIndex,	
	GT_UOPT		*activePcsNum
)
{
	if((HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0) || (firstPcsNum != 29))
	{
		*activePcsNum = firstPcsNum + pcsIndex;
	}	
	else
	{
		/* for Alleycat3 port #29, the first PCS number is 29 and second PCS number is 50 */
		*activePcsNum = (pcsIndex == 0) ? 29 : 50;
	}

	return GT_OK;
}

