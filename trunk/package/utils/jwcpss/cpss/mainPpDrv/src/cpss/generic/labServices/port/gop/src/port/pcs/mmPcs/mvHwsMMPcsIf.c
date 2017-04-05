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
* mvHwsMMPcsIf.c
*
* DESCRIPTION:
*       MMPCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 35 $
*
*******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/mmPcs/mvHwsMMPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcsDb.h>

static char* mvHwsPcsTypeGetFunc(void)
{
  return "MMPCS";
}

GT_STATUS mvHwsMMPcsResetWa
(
 GT_U8                   devNum,
 GT_U32                  portGroup,
 GT_U32                  pcsNum
);

static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* mvHwsMMPcsIfInit
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
GT_STATUS mvHwsMMPcsIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsMMPscSeqInit());
    }

    funcPtrArray[MMPCS].pcsResetFunc = mvHwsMMPcsReset;
    funcPtrArray[MMPCS].pcsModeCfgFunc = mvHwsMMPcsMode;
    funcPtrArray[MMPCS].pcsLbCfgFunc = mvHwsMMPcsLoopBack;
    funcPtrArray[MMPCS].pcsTestGenFunc = mvHwsMMPcsTestGenCfg;
    funcPtrArray[MMPCS].pcsTestGenStatusFunc = mvHwsMMPcsTestGenStatus;
    funcPtrArray[MMPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    funcPtrArray[MMPCS].pcsRxResetFunc = mvHwsMMPcsRxReset;
    funcPtrArray[MMPCS].pcsSignalDetectMaskEn = mvHwsMMPcsSignalDetectMaskSet;
    funcPtrArray[MMPCS].pcsFecCfgFunc = mvHwsMMPcsFecConfig;
    funcPtrArray[MMPCS].pcsFecCfgGetFunc = mvHwsMMPcsFecConfigGet;
    funcPtrArray[MMPCS].pcsActiveStatusGetFunc = mvHwsMMPcsActiveStatusGet;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}


static GT_STATUS mvHwsMMPcsUnreset
(
  GT_U8  devNum,
  GT_U32 portGroup,
  GT_U32 pcsNum,
  GT_U32 countVal
)
{
  GT_U32 regAddr;
  GT_U32 i;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

  for (i = 0; i < countVal; i++)
  {
    regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 1, 1));
  }
  for (i = 0; i < countVal; i++)
  {
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x2, 0x2));
  }
  for (i = 0; i < countVal; i++)
  {
    regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    /* only MMPCS 0 are relevant; other lanes keep reset set 0 */
    if (i == 0)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x4, 0x4));
    }
    else
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x0, 0x4));
    }
  }
  for (i = 0; i < countVal; i++)
  {
    regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    /* only MMPCS 0 are relevant; other lanes keep reset set 0 */
    if (i == 0)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x20, 0x20));
    }
    else
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x0, 0x20));
    }
  }
  for (i = 0; i < countVal; i++)
  {
    regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    /* only MMPCS 0 are relevant; other lanes keep reset set 0 */
    if (i == 0)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x40, 0x40));
    }
    else
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x0, 0x40));
    }
  }
  for (i = 0; i < countVal; i++)
  {
    regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x10, 0x10));
  }
  for (i = 0; i < countVal; i++)
  {
    regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 8, 8));
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsReset
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
GT_STATUS mvHwsMMPcsReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            action
)
{
    MV_CFG_SEQ *seq;
    GT_U32 regAddr;
    GT_U32 data, i;
    GT_U32 countVal;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    if (action == RESET)
    {
      seq = &hwsMmPscSeqDb[MMPCS_RESET_SEQ];
      for (i = 0; i < countVal; i++)
      {
        CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum + i, seq->cfgSeq, seq->cfgSeqSize));
      }
    }
    else
    {
      CHECK_STATUS(mvHwsMMPcsUnreset(devNum,portGroup,pcsNum,countVal));

      mvHwsMMPcsResetWa(devNum, portGroup, pcsNum);
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsMode
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
GT_STATUS mvHwsMMPcsMode
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_U32                  numOfLanes
)
{
    MV_CFG_SEQ *seq;
    GT_U32 regAddr;
    GT_U32 data;
    GT_U32 mask;
    GT_U32 i;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    /* config FEC and lanes number */
    for (i = 0; i < numOfLanes; i++)
    {
      regAddr = PCS40G_COMMON_CONTROL + unitAddr + (pcsNum+i) * unitIndex;
      data = (fecCtrl == FEC_ON) ? (1 << 10) : 0;
      mask = (1 << 10) + (1 << 7); /* always set bit 7 to 0;  */
      CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, mask));
    }

    switch(numOfLanes)
    {
    case 1:
      seq = &hwsMmPscSeqDb[MMPCS_MODE_1_LANE_SEQ];
      break;
    case 2:
      seq = &hwsMmPscSeqDb[MMPCS_MODE_2_LANE_SEQ];
      break;
    case 4:
      seq = &hwsMmPscSeqDb[MMPCS_MODE_4_LANE_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsLoopback
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
GT_STATUS mvHwsMMPcsLoopBack
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
* mvHwsMMPcsTestGenCfg
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
GT_STATUS mvHwsMMPcsTestGenCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern
)
{
    MV_CFG_SEQ *seq;

    CHECK_STATUS(mvHwsMMPcsReset(devNum,portGroup,pcsNum,RESET));

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

    CHECK_STATUS(mvHwsMMPcsReset(devNum,portGroup,pcsNum,UNRESET));

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsTestGenStatus
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
GT_STATUS mvHwsMMPcsTestGenStatus
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
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  pattern = pattern;

  if (status == 0)
  {
    return GT_BAD_PARAM;
  }

  mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);
  accessAddr = FEC_DEC_STATUS_REG + unitAddr + pcsNum * unitIndex;;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
  status->checkerLock = (data >> 3) & 1; /* bit 3 */

  accessAddr = TEST_PATTERN_ERROR_COUNT + unitAddr + pcsNum * unitIndex;;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
  status->totalErrors = data;

  status->goodFrames = 0;

  return GT_OK;
}

/*******************************************************************************
* hwsMMPcsIfClose
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
void hwsMMPcsIfClose(void)
{
  hwsMMPcsSeqFree();
  hwsInitDone = GT_FALSE;
}

static GT_STATUS pcsRxReset
(
  GT_U8  devNum,
  GT_U32 portGroup,
  GT_U32 pcsNum,
  GT_U32 countVal
)
{
  GT_U32 regAddr, data, i;
  GT_U32 tryNum;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

  regAddr = unitAddr + pcsNum*unitIndex + 0x30;
  genRegisterGet(devNum, portGroup, regAddr, &data, 0);
  tryNum = 0;
  /*osPrintf("\nBefore Run PCS WA on pcs #%d (data is 0x%x).",  pcsNum, data);*/
  while (((data & 1) == 0) && (tryNum < 10))
  {
    /* reset PCS */
    for (i = 0; i < countVal; i++)
    {
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 0x38));
    }

    hwsOsTimerWkFuncPtr(1);
    /* unreset PCS */
    for (i = 0; i < countVal; i++)
    {
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 5), (1 << 5)));
    }
    for (i = 0; i < countVal; i++)
    {
        regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 4), (1 << 4)));
    }
    for (i = 0; i < countVal; i++)
    {
        regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 3), (1 << 3)));
    }

    hwsOsTimerWkFuncPtr(1);
    /* Check AlignLock */
    regAddr = unitAddr + pcsNum*unitIndex + 0x30;
    genRegisterGet(devNum, portGroup, regAddr, &data, 0);
    tryNum++;
  }
    /*osPrintf("\nAfter Run PCS WA on pcs #%d (data is 0x%x, loop number is %d).",
        pcsNum, data, tryNum);*/

  if ((data & 1) == 1)
  {
    /* align locked */
    return GT_OK;
  }
    return GT_FAIL;
}


GT_STATUS mvHwsMMPcsResetWa
(
 GT_U8                   devNum,
 GT_U32                  portGroup,
 GT_U32                  pcsNum
)
{
    GT_U32 regAddr, data, gearVal;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    hwsOsTimerWkFuncPtr(10);
    /* Check Gear Box Status */
    regAddr = unitAddr + pcsNum*unitIndex + 0xD0;
    genRegisterGet(devNum, portGroup, regAddr, &data, 0);
    gearVal = (data >> 1) & 0xF;
    /*osPrintf("\nPCS reset WA on port %d. Gear Box is 0x%x.", pcsNum, gearVal);*/
    if (gearVal != 0xF)
    {
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
        GT_U32 countVal;

        regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum*unitIndex;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        countVal = (data >> 8) & 3;
        countVal = (countVal == 0) ? 1 : countVal * 2;
        /* Lane is Locked -> Check the Align Lock Status  */
        CHECK_STATUS(pcsRxReset(devNum, portGroup, pcsNum, countVal));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsRxReset
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
GT_STATUS mvHwsMMPcsRxReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            action
)
{
    devNum = devNum;
    portGroup = portGroup;
    pcsNum = pcsNum;
    action = action;

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsSignalDetectMaskSet
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
GT_STATUS mvHwsMMPcsSignalDetectMaskSet
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
    GT_U32 countVal, i;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum*unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    for (i = 0; i < countVal; i++)
    {
        regAddr = CHANNEL_CONFIGURATION + unitAddr + (pcsNum + i)*unitIndex;
        data = (maskEn == GT_TRUE) ? (1 << 15) : 0;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, (1 << 15)));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsFecConfig
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
GT_STATUS mvHwsMMPcsFecConfig
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
    GT_U32 countVal, i;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum*unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    data = (fecEn == GT_TRUE) ? (1 << 10) : 0;
    for (i = 0; i < countVal; i++)
    {
        regAddr = PCS40G_COMMON_CONTROL + unitAddr + (pcsNum + i)*unitIndex;
    	CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, (1 << 10)));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsFecConfigGet
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
GT_STATUS mvHwsMMPcsFecConfigGet
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

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));

    *fecEn = ((data >> 10) & 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsActiveStatusGet
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
GT_STATUS mvHwsMMPcsActiveStatusGet
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

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + pcsNum * unitIndex;

    *numOfLanes = 0;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + PCS_RESET_REG, &data, 0));
    /* check unreset bit status */
    if (!((data >> 6) & 1))
    {
        /* unit is under RESET */
        return GT_OK;
    }
    /* get number of active lanes */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + PCS40G_COMMON_CONTROL, &data, 0));
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

