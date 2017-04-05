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
* mvComPhyHIf.c
*
* DESCRIPTION:
*         Com PHY H (serdes) interface
*
* FILE REVISION NUMBER:
*       $Revision: 62 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <serdes/mvHwsSerdesIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/comPhyH/mvComPhyHDb.h>
#include <serdes/comPhyH/mvComPhyHIf.h>


static GT_U32 scalingThr = 50;
static GT_32  fminus1_thr = 3;
static GT_U32 fminus1Counter = 15;
static GT_U32 runAlign90opt = 1;
static GT_U32 runDfeOpti = 1;
static GT_U32 align90StartTap = 85;
static GT_U32 bestFfeAdd = 3;

/* internal registers */
const GT_U32 COM_H_Pattern_Reg = 0x6c;
const GT_U32 COM_H_PT_PATTERN_SEL_Reg = 0x54;
const GT_U32 COM_H_PHY_TEST_OOB_Reg = 0x84;
const GT_U32 COM_H_SEL_BITS_Reg = 0x8C;
const GT_U32 COM_H_INVERT_Reg = 0x90;
const GT_U32 COM_H_PHY_ISOLATE_MODE_CTRL_Reg = 0x98;

const GT_U32 COM_H_ERROR_Cntr_MSB_Reg = 0x7C;
const GT_U32 COM_H_ERROR_Cntr_LSB_Reg = 0x80;
const GT_U32 COM_H_FRAME_Cntr_MSB1_Reg = 0x70;
const GT_U32 COM_H_FRAME_Cntr_MSB_Reg = 0x74;
const GT_U32 COM_H_FRAME_Cntr_LSB_Reg = 0x78;
const GT_U32 COM_H_GEN1_SET2_Reg = 0xF4;

const GT_U32 COM_H_TX_AMP_Reg = 0x34;
const GT_U32 COM_H_TX_EMPH_Reg = 0x178;
const GT_U32 COM_H_RX_FFE_Reg = 0x18;
const GT_U32 COM_H_RX_DFE_Reg = 0x8;
const GT_U32 COM_H_PU_TX_INTP_Reg = 0x4;
const GT_U32 COM_H_PPM_Reg = 0x14;
const GT_U32 COM_H_DFE_LOCK_Reg = 0x1C;
const GT_U32 COM_H_CFG_Reg = 0x20;
const GT_U32 COM_H_DFE_Reg = 0x2C;
const GT_U32 COM_H_CFG_Reg1 = 0x24;
const GT_U32 COM_H_CFG_Reg2 = 0x28;
const GT_U32 COM_H_DFE_Reg1 = 0x30;
const GT_U32 COM_H_CalibrReg = 0x15C;
const GT_U32 COM_H_RxClockAl90 = 0x168;
const GT_U32 COM_H_FFE_Reg = 0x1A4;
const GT_U32 COM_H_OS_PH_Reg = 0x1A0;
const GT_U32 COM_H_DFE_CFG_Reg = 0x19C;
const GT_U32 COM_H_Genration1_Set1 = 0x38;

static GT_STATUS dfeScalingCheck
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_U32  threshold
);

static GT_STATUS getFfeAvarege
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  serdesNum
);

static GT_STATUS align90opt
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  serdesNum
);


static GT_STATUS mvHwsComHCheckSerdesAccess
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  serdesNum
);

static char* mvHwsSerdesTypeGetFunc(void)
{
  return "COM_H";
}

static GT_STATUS mvHwsComHSerdesAutoTuneRxStatus
(
 GT_U8                   devNum,
 GT_U32                  portGroup,
 GT_U32                  serdesNum,
 MV_HWS_AUTO_TUNE_STATUS *rxStatus
);

static GT_STATUS mvHwsComHSerdesAutoTuneTxStatus
(
 GT_U8                   devNum,
 GT_U32                  portGroup,
 GT_U32                  serdesNum,
 MV_HWS_AUTO_TUNE_STATUS *txStatus
);
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* mvHwsComHIfInit
*
* DESCRIPTION:
*       Init Com_H serdes configuration sequences and IF functions.
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
GT_STATUS mvHwsComHIfInit(MV_HWS_SERDES_FUNC_PTRS *funcPtrArray)
{
  if (hwsInitDone == GT_FALSE)
  {
    /* init sequences DB */
    CHECK_STATUS(hwsComPhyHSeqInit());
  }

  funcPtrArray[COM_PHY_H].serdesPowerCntrlFunc = mvHwsComHSerdesPowerCtrl;
  funcPtrArray[COM_PHY_H].serdesManualRxCfgFunc = mvHwsComHSerdesManualRxConfig;
  funcPtrArray[COM_PHY_H].serdesManualTxCfgFunc = mvHwsComHSerdesManualTxConfig;
  funcPtrArray[COM_PHY_H].serdesLoopbackCfgFunc = mvHwsComHSerdesLoopback;
  funcPtrArray[COM_PHY_H].serdesLoopbackGetFunc = mvHwsComHSerdesLoopbackGet;
  funcPtrArray[COM_PHY_H].serdesPpmCfgFunc = mvHwsComHSerdesPpmConfig;
  funcPtrArray[COM_PHY_H].serdesTestGenFunc = mvHwsComHSerdesTestGen;
  funcPtrArray[COM_PHY_H].serdesTestGenGetFunc = mvHwsComHSerdesTestGenGet;
  funcPtrArray[COM_PHY_H].serdesTypeGetFunc    = mvHwsSerdesTypeGetFunc;
  funcPtrArray[COM_PHY_H].serdesTestGenStatusFunc = mvHwsComHSerdesTestGenStatus;
  funcPtrArray[COM_PHY_H].serdesEomGetFunc = mvHwsComHSerdesEOMGet;
  funcPtrArray[COM_PHY_H].serdesAutoTuneCfgFunc = mvHwsComHSerdesAutoTuneCfg;
  funcPtrArray[COM_PHY_H].serdesRxAutoTuneStartFunc = mvHwsComHSerdesRxAutoTuneStart;
  funcPtrArray[COM_PHY_H].serdesTxAutoTuneStartFunc = mvHwsComHSerdesTxAutoTuneStart;
  funcPtrArray[COM_PHY_H].serdesAutoTuneStartFunc = mvHwsComHSerdesAutoTuneStart;
  funcPtrArray[COM_PHY_H].serdesAutoTuneResultFunc = mvHwsComHSerdesAutoTuneResult;
  funcPtrArray[COM_PHY_H].serdesAutoTuneStatusFunc = mvHwsComHSerdesAutoTuneStatus;
  funcPtrArray[COM_PHY_H].serdesResetFunc = mvHwsComHSerdesReset;
  funcPtrArray[COM_PHY_H].serdesDfeCfgFunc = mvHwsComHSerdesDfeConfig;
  funcPtrArray[COM_PHY_H].serdesDfeStatusFunc = mvHwsComHSerdesDfeStatus;
  funcPtrArray[COM_PHY_H].serdesPolarityCfgFunc = mvHwsComHSerdesPolarityConfig;
  funcPtrArray[COM_PHY_H].serdesPolarityCfgGetFunc = mvHwsComHSerdesPolarityConfigGet;
  funcPtrArray[COM_PHY_H].serdesDfeOptiFunc = mvHwsComHDfeOpti;
  funcPtrArray[COM_PHY_H].serdesFfeCfgFunc = mvHwsComHSerdesFfeConfig;
  funcPtrArray[COM_PHY_H].serdesTxEnableFunc = mvHwsComHSerdesTxEnable;
  funcPtrArray[COM_PHY_H].serdesTxEnableGetFunc = mvHwsComHSerdesTxEnableGet;
  funcPtrArray[COM_PHY_H].serdesSeqGetFunc = hwsComPhyHSeqGet;
  funcPtrArray[COM_PHY_H].serdesSeqSetFunc = hwsComPhyHSeqSet;

  hwsInitDone = GT_TRUE;

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesTestGen
*
* DESCRIPTION:
*       Activates the Serdes test generator/checker.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       txPattern - pattern to transmit ("Other" means HW default - K28.5
*                                               [alternate running disparity])
*       mode      - test mode or normal
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesTestGen
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TX_PATTERN  txPattern,
    MV_HWS_SERDES_TEST_GEN_MODE mode
)
{
  MV_CFG_SEQ *seq;
  GT_U32 baseAddr;
  GT_U32 patternData;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));

  switch(txPattern)
  {
  case   _1T:
    patternData = 1;
    break;
  case _2T:
    patternData = 2;
    break;
  case _5T:
    patternData = 3;
    break;
  case _10T:
    patternData = 4;
    break;
  case PRBS7:
    patternData = 0x80;
    break;
  case  PRBS9:
    patternData = 0x81;
      break;
  case PRBS15:
    patternData = 0x82;
    break;
  case  PRBS23:
    patternData = 0x83;
    break;
  case  PRBS31:
    patternData = 0x84;
      break;
  case DFETraining:
    patternData = 0x12;
    break;
      case Other:
        patternData = 0;
        break;
  default:
    return GT_NOT_SUPPORTED;
  }

  /* set pattern type */
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_Pattern_Reg, patternData, 0xFFFF));

  /* run pattern sequence */
  seq = (mode == SERDES_NORMAL) ? &hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ] : &hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ];
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesTestGenGet
*
* DESCRIPTION:
*       Get configuration of the Serdes test generator/checker.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       txPatternPtr - pattern to transmit ("Other" means any mode not
*                       included explicitly in MV_HWS_SERDES_TX_PATTERN type)
*       modePtr      - test mode or normal
*
* RETURNS:
*       GT_OK  - on success
*       GT_NOT_SUPPORTED - unexpected pattern
*       GT_FAIL - HW error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesTestGenGet
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TX_PATTERN  *txPatternPtr,
    MV_HWS_SERDES_TEST_GEN_MODE *modePtr
)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

    /* get pattern type */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_Pattern_Reg, &data, 0xFFFF));

    switch(data)
    {
        case 1:
            *txPatternPtr = _1T;
            break;
        case 2:
            *txPatternPtr = _2T;
            break;
        case 3:
            *txPatternPtr = _5T;
            break;
        case 4:
            *txPatternPtr = _10T;
            break;
        case 0x80:
            *txPatternPtr = PRBS7;
            break;
        case  0x81:
            *txPatternPtr = PRBS9;
            break;
        case 0x82:
            *txPatternPtr = PRBS15;
            break;
        case  0x83:
            *txPatternPtr = PRBS23;
            break;
        case  0x84:
            *txPatternPtr = PRBS31;
            break;
        case 0x12:
            *txPatternPtr = DFETraining;
            break;
        default:
            *txPatternPtr = Other;
            break;
    }

    /* get PRBS test mode */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_PT_PATTERN_SEL_Reg, &data, 0x8000));
    *modePtr = (0 == data) ? SERDES_NORMAL : SERDES_TEST;

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesTestGenStatus
*
* DESCRIPTION:
*       Read the tested pattern receive error counters and status.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       txPattern - pattern to transmit
*
* OUTPUTS:
*       Counters and status.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesTestGenStatus
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    MV_HWS_SERDES_TX_PATTERN  txPattern,
    GT_BOOL                   counterAccumulateMode,
    MV_HWS_SERDES_TEST_GEN_STATUS *status
)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  txPattern = txPattern;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_PT_PATTERN_SEL_Reg, &data, 0));
  status->lockStatus = (data >> 12) & 1;

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_ERROR_Cntr_MSB_Reg, &data, 0));
  status->errorsCntr = (data << 16);
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_ERROR_Cntr_LSB_Reg, &data, 0));
  status->errorsCntr += data;
    /* read 48 bit counter */
  status->txFramesCntr.s[3] = 0;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_FRAME_Cntr_MSB1_Reg, &data, 0));
  status->txFramesCntr.s[2] = (GT_U16)data;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_FRAME_Cntr_MSB_Reg, &data, 0));
  status->txFramesCntr.s[1] = (GT_U16)data;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_FRAME_Cntr_LSB_Reg, &data, 0));
  status->txFramesCntr.s[0] = (GT_U16)data;

  /* reset counter */
  if (counterAccumulateMode == GT_FALSE)
  {
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_PHY_TEST_OOB_Reg, 0x2030, 0));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_PHY_TEST_OOB_Reg, 0x30, 0));
  }

  return 0;
}


/*******************************************************************************
* mvHwsComHSerdesReset
*
* DESCRIPTION:
*       Per SERDES Clear the serdes registers (back to defaults.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - serdes number
*       analogReset - Analog Reset (On/Off)
*       digitalReset - digital Reset (On/Off)
*       syncEReset - SyncE Reset (On/Off)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesReset
(
    GT_U8      devNum,
    GT_U32     portGroup,
    GT_U32     serdesNum,
    GT_BOOL    analogReset,
    GT_BOOL    digitalReset,
    GT_BOOL    syncEReset
)
{
    MV_CFG_SEQ *seq;

    seq = (analogReset == GT_TRUE) ? &hwsSerdesSeqDb[SERDES_SD_RESET_SEQ] : &hwsSerdesSeqDb[SERDES_SD_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    seq = (digitalReset == GT_TRUE) ? &hwsSerdesSeqDb[SERDES_RF_RESET_SEQ] : &hwsSerdesSeqDb[SERDES_RF_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    seq = (syncEReset == GT_TRUE) ? &hwsSerdesSeqDb[SERDES_SYNCE_RESET_SEQ] : &hwsSerdesSeqDb[SERDES_SYNCE_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    return 0;
}
/*******************************************************************************
* mvHwsComHSerdesPowerCtrl
*
* DESCRIPTION:
*       Init physical port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       powerUp   - true for power UP
*       baudRate  -
*       refClock  - ref clock value
*       refClockSource - ref clock source (primary line or secondary)
*       media     - RXAUI or XAUI
*       mode      - 10BIT mode (enable/disable)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesPowerCtrl
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 powerUp,
    MV_HWS_SERDES_SPEED     baudRate,
    MV_HWS_REF_CLOCK        refClock,
    MV_HWS_REF_CLOCK_SRC    refClockSource,
    MV_HWS_SERDES_MEDIA     media,
    MV_HWS_SERDES_10B_MODE  mode
)
{
  MV_CFG_SEQ *seq;
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 mask;
  GT_U32 polCntr;
  GT_U32 sq_thres_cal;
  GT_U32 sq_pkcm_cal;
  GT_U32 sq_cmpth_cal;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  refClock = refClock;

  /* config media */
  if (media == RXAUI_MEDIA)
  {
    data = (1 << serdesNum);
    mask = (1 << serdesNum);
  }
  else
  {
    data = 0;
    mask = (1 << serdesNum);
  }
  CHECK_STATUS(genRegisterSet(devNum, portGroup, MG_Media_Interface_Reg, data, mask));

  /* config 10BIT mode */
  if (mode == _10BIT_ON)
  {
    data = (1 << 14);
    mask = (1 << 14);
  }
  else
  {
    data = 0;
    mask = (1 << 14);
  }
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum;
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_0, data, mask));

  /* serdes power down */
  /*seq = &hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ];
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));*/

  if (powerUp)
  {
    CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_FALSE, GT_TRUE, GT_TRUE));

    data = (refClockSource == PRIMARY) ? 0 : 1;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x33c, data, (1 << 10)));

    switch(baudRate)
    {
    case _1_25G:
      seq = &hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ];
      break;
    case _3_125G:
      seq = &hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ];
      break;
    case _3_75G:
      seq = &hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ];
      break;
    case _4_25G:
      seq = &hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ];
      break;
    case _5G:
      seq = &hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ];
      break;
    case _6_25G:
      seq = &hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ];
      break;
    case _7_5G:
      seq = &hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ];
      break;
    case _10_3125G:
      seq = &hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ];
      break;
    case _12_5G:
        seq = &hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ];
        break;
    case SPEED_NA:
        return GT_FAIL;
    default:
      return GT_NOT_SUPPORTED;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    seq = &hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    /* serdes wait PLL */
    seq = &hwsSerdesSeqDb[SERDES_SERDES_WAIT_PLL_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    /* run CAL_START */
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_DFE_Reg, 0x80C0, 0));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_DFE_Reg, 0x00C0, 0));
    polCntr = 0;
    do
    {
        hwsOsExactDelayPtr(devNum, portGroup, 1);
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_RX_DFE_Reg, &data, 0));
        polCntr++;

    } while (!((data >> 14) & 1) && (polCntr < 10));

    if (polCntr == 10)
    {
        return GT_FAIL;
    }

    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    /* SEL_SMPLR_VCM[1:0]=0x05 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1CC, 0xA28, 0));

    /* write to 0x1E0 bits [15:14] */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1E0, (1 << 14), (1 << 14)));
    /* store sq_cmpth_cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+0x1E0, &data, 0));
    sq_cmpth_cal = (data >> 8) & 0x3F;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1E0, (2 << 14), (3 << 14)));
    /* store sq_pkcm_cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+0x1E0, &data, 0));
    sq_pkcm_cal = (data >> 8) & 0x3F;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1E0, (3 << 14), (3 << 14)));
    /* store sq_cmpth_cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+0x1E0, &data, 0));
    sq_thres_cal = (data >> 8) & 0x3F;

    /* write results to external SQ */
    data = (1 << 15) + (sq_cmpth_cal << 10) + (sq_pkcm_cal << 4) + (sq_thres_cal & 0xF);
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1BC, data, 0));

    seq = &hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_FALSE, GT_FALSE, GT_FALSE));

    baseAddr = unitAddr + unitIndex*serdesNum;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_STATUS_0, &data, 0));
    if ((data & 0x1c) != 0x1c)
    {
      return GT_FAIL;
    }
  }
  else
  {
    /*CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_TRUE, GT_FALSE, GT_FALSE));*/
    seq = &hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
    /*seq = &hwsSerdesSeqDb[SERDES_SERDES_RXINT_DOWN_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
    CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_TRUE, GT_TRUE, GT_TRUE));*/
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesManualTxConfig
*
* DESCRIPTION:
*       Per SERDES configure the TX parameters: amplitude, 3 TAP Tx FIR.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesManualTxConfig
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_U32  txAmp,
    GT_BOOL txAmpAdj,
    GT_U32  emph0,
    GT_U32  emph1,
    GT_U32  txAmpShft
)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  txAmpShft = txAmpShft;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  data = (txAmpAdj ? 1:0);
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_TX_AMP_Reg, (data << 6), (1 << 6)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_TX_AMP_Reg, (txAmp << 1), 0x3E));

  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_GEN1_SET2_Reg, emph0, 0xF));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_GEN1_SET2_Reg, (1 << 4), (1 << 4)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_TX_AMP_Reg, (emph1 << 7), 0x780));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_TX_AMP_Reg, (1 << 11), (1 << 11)));

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesManualRxConfig
*
* DESCRIPTION:
*       Per SERDES configure the RX parameters: squelch Threshold, FFE and DFE
*       operation
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical lane number
*       portTuningMode - lane tuning mode (short / long)
*       sqlch     - Squelch threshold
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesManualRxConfig
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    MV_HWS_PORT_MAN_TUNE_MODE portTuningMode,
    GT_U32  sqlch,
    GT_U32  ffeRes,
    GT_U32  ffeCap,
    GT_BOOL dfeEn,
    GT_U32  alig

)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_32  curVal, tmpAlig;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  sqlch = sqlch;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  if (portTuningMode == StaticLongReach)
  {
    /* only in Long Reach - set highest DFE resolution */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_DFE_LOCK_Reg, 0xD047, 0));
  }

  /* set bit 7 to 1 */
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, (1 << 7), (1 << 7)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, (ffeRes << 4), (7 << 4)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, ffeCap, 0xF));
  /* TBD:: CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, (sqlch << 8), 0x1F00));*/

  data = (dfeEn  ? 1:0);
  CHECK_STATUS(genRegisterSet(devNum, portGroup, unitAddr + serdesNum * unitIndex + SERDES_EXTERNAL_CONFIGURATION_2, (data << 4), (1 << 4)));

  /*  set bit 15 of 0x168 */
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));

  /* config align90 */
  /* read current align90 value */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, &data, 0));
  curVal = (data >> 8) & 0x7F;
  tmpAlig = alig;
  /* move to align90 value with step = 4 */
  if (((curVal - 4) > tmpAlig) || ((GT_U32)(curVal + 4) < alig))
  {
    while((curVal - 4) > tmpAlig)
    {
      curVal -= 4;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (curVal << 8), 0x7F00));
    }
    while((GT_U32)(curVal + 4) < alig)
    {
      curVal += 4;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (curVal << 8), 0x7F00));
    }
  }
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (alig << 8), 0x7F00));


  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesPolarityConfig
*
* DESCRIPTION:
*       Per serdes invert the Tx or Rx.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesPolarityConfig
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL invertTx,
    GT_BOOL invertRx
)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  data = (invertRx) ? 1 : 0;
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_INVERT_Reg, (data << 11), (1 << 11)));
  data = (invertTx) ? 1 : 0;
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_INVERT_Reg, (data << 10), (1 << 10)));

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesPolarityConfigGet
*
* DESCRIPTION:
*       Per SERDES invert the Tx or Rx.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       invertTx  - invert TX polarity (GT_TRUE - invert, GT_FALSE - don't)
*       invertRx  - invert RX polarity (GT_TRUE - invert, GT_FALSE - don't)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesPolarityConfigGet
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    GT_BOOL           *invertTx,
    GT_BOOL           *invertRx
)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum;
    /* check analog reset */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_1, &data, (1 << 3)));
    if(data == 0)
        return GT_NOT_INITIALIZED;

    /* check digital reset */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_1, &data, (1 << 6)));
    if(data == 0)
        return GT_NOT_INITIALIZED;

    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_INVERT_Reg, &data, (1 << 10)));
    *invertTx = (data) ? 1 : 0;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_INVERT_Reg, &data, (1 << 11)));
    *invertRx = (data) ? 1 : 0;

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesPpmConfig
*
* DESCRIPTION:
*       Per SERDES add ability to add/sub PPM from main baud rate.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesPpmConfig
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_PPM_VALUE  ppmVal
)
{
  GT_U32 baseAddr;
  GT_U32 puTxIntp;
  GT_U32 txInitFoffset;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  puTxIntp = 1;
  switch (ppmVal)
  {
  case Neg_3_TAPS:
    txInitFoffset = 0x1;
    break;
  case Neg_2_TAPS:
    txInitFoffset = 0x2;
    break;
  case Neg_1_TAPS:
    txInitFoffset = 0x3;
    break;
  case ZERO_TAPS:
    puTxIntp = 0;
    txInitFoffset = 0;
    break;
  case POS_1_TAPS:
    txInitFoffset = 1023;
    break;
  case POS_2_TAPS:
    txInitFoffset = 1022;
    break;
  case POS_3_TAPS:
    txInitFoffset = 1021;
    break;
  default:
    return GT_BAD_PARAM;
    break;
  }
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_PU_TX_INTP_Reg, (puTxIntp << 11), (1 << 11)));

  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_PPM_Reg, txInitFoffset, 0x3FF));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_PPM_Reg, (1 << 10), (1 << 10)));

  CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_FALSE, GT_FALSE, GT_FALSE));

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesDfeConfig
*
* DESCRIPTION:
*       Per SERDES configure the DFE parameters.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       dfeMode   - Enable/disable/freeze/Force
*       dfeCfg    - array of dfe configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesDfeConfig
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_DFE_MODE   dfeMode,
    GT_32            *dfeCfg
)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 dfeVal[6];
  GT_U32 i;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

  switch(dfeMode)
  {
  case DFE_DISABLE:
    baseAddr = unitAddr + unitIndex*serdesNum;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_2, 0, (1 << 4)));
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_DFE_CFG_Reg, 0, (1 << 12)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_CFG_Reg, 0, 0x3F));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_Genration1_Set1, 0, (1 << 10)));
    break;
  case DFE_ENABLE:
    baseAddr = unitAddr + unitIndex*serdesNum;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_2, (1 << 4), (1 << 4)));
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_DFE_CFG_Reg, 0, (1 << 12)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_CFG_Reg, 0, 0x3F));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_Genration1_Set1, (1 << 10), (1 << 10)));
    break;
  case DFE_FREEZE:
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_DFE_CFG_Reg, (1 << 12), (1 << 12)));
    break;
  case DFE_FORCE:
    for (i = 0; i < 6; i++)
    {
      dfeVal[i] = (GT_U32)dfeCfg[i];
    }
    if (dfeCfg[1] < 0)
    {
      dfeVal[1] = (1 << 6) + ABS(dfeCfg[1]);
    }
    if (dfeCfg[2] < 0)
    {
      dfeVal[2] = (1 << 5) + ABS(dfeCfg[2]);
    }
    if (dfeCfg[3] < 0)
    {
      dfeVal[3] = (1 << 4) + ABS(dfeCfg[3]);
    }
    if (dfeCfg[4] < 0)
    {
      dfeVal[4] = (1 << 4) + ABS(dfeCfg[4]);
    }
    if (dfeCfg[5] < 0)
    {
      dfeVal[5] = (1 << 4) + ABS(dfeCfg[5]);
    }
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    data = (dfeVal[0] << 10) + (dfeVal[1] << 4) + dfeVal[3];
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_CFG_Reg1, data, 0));
    data = (dfeVal[2] << 8) + (dfeVal[4] << 4) + dfeVal[5];
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_CFG_Reg2, data, 0));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_CFG_Reg, 0x3F, 0x3F));
    break;
  default:
    return GT_BAD_PARAM;
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesDfeStatus
*
* DESCRIPTION:
*       Per SERDES get the DFE status and parameters.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       dfeLock   - Locked/Not locked
*       dfeCfg    - array of dfe configuration parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesDfeStatus
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    GT_BOOL           *dfeLock,
    GT_32            *dfeCfg
)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 i;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  for (i = 0; (dfeCfg) && (i < 6); i++)
  {
    dfeCfg[i] = 0;
  }
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_DFE_Reg, &data, 0));
  dfeCfg[0] = (data >> 10) & 0x3F;
  /* bit 9 is the sign */
  if ((data >> 9) & 1)
  {
    dfeCfg[1] = ((data >> 4) & 0x1F) * (-1);
  }
  else
  {
    dfeCfg[1] = ((data >> 4) & 0x1F);
  }
  /* bit 3 is the sign */
  if ((data >> 3) & 1)
  {
    dfeCfg[3] = (data & 7) * (-1);
  }
  else
  {
    dfeCfg[3] = (data & 7);
  }

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_DFE_Reg1, &data, 0));
  /* bit 12 is the sign */
  if ((data >> 12) & 1)
  {
    dfeCfg[2] = ((data >> 8) & 0xF) * (-1);
  }
  else
  {
    dfeCfg[2] = ((data >> 8) & 0xF);
  }
  /* bit 7 is the sign */
  if ((data >> 7) & 1)
  {
    dfeCfg[4] = ((data >> 4) & 7) * (-1);
  }
  else
  {
    dfeCfg[4] = ((data >> 4) & 7);
  }
  /* bit 3 is the sign */
  if ((data >> 3) & 1)
  {
    dfeCfg[5] = (data & 7) * (-1);
  }
  else
  {
    dfeCfg[5] = (data & 7);
  }

  /* read DFE lock status */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_DFE_LOCK_Reg, &data, 0));
  if ((data >> 12) & 1)
  {
    *dfeLock = GT_TRUE;
  }
  else
  {
    *dfeLock = GT_FALSE;
  }
  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesLoopback
*
* DESCRIPTION:
*       Perform an internal loopback (SERDES loopback) for debug/BIST use.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - type of serdes (LP/COM_PHY...)
*       lbMode    - loopback mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesLoopback
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_SERDES_LB  lbMode
)
{
  MV_CFG_SEQ *seq;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  switch (lbMode)
  {
  case SERDES_LP_DISABLE:
    seq = &hwsSerdesSeqDb[SERDES_SD_LPBK_NORMAL_SEQ];
    break;
  case SERDES_LP_AN_TX_RX:
    seq = &hwsSerdesSeqDb[SERDES_SD_ANA_TX_2_RX_SEQ];
    break;

  case SERDES_LP_DIG_TX_RX:
    seq = &hwsSerdesSeqDb[SERDES_SD_DIG_TX_2_RX_SEQ];
    break;
  case SERDES_LP_DIG_RX_TX:
    seq = &hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ];
    break;
  default:
    return GT_NOT_SUPPORTED;
    break;
  }
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesLoopbackGet
*
* DESCRIPTION:
*       Get status of internal loopback (SERDES loopback).
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       serdesType - type of serdes (LP/COM_PHY...)
*
* OUTPUTS:
*       lbModePtr - current loopback mode
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesLoopbackGet
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_SERDES_LB  *lbModePtr
)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x8c, &data,
                                                                (0xF << 12)));

    switch(data)
    {
        case (1<<13):
            *lbModePtr = SERDES_LP_AN_TX_RX;
            break;

        case (1<<14):
            *lbModePtr = SERDES_LP_DIG_TX_RX;
            break;

        case 0x9000:
            CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x54,
                                                    &data, (0xF0 | 0x8000)));
            if((data & 0xD0) && (data & 0x8000))
            {
                *lbModePtr = SERDES_LP_DIG_RX_TX;
            }
            else
            {
                return GT_NOT_SUPPORTED;
            }
            break;

        case 0:
                *lbModePtr = SERDES_LP_DISABLE;
            break;

        default:
            return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesAutoTuneCfg
*
* DESCRIPTION:
*       Per SERDES configure parameters for TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
*       txTraining - Tx Training (true/false)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesAutoTuneCfg
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL rxTraining,
    GT_BOOL txTraining
)
{
  MV_CFG_SEQ *seq;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  mvHwsComHSerdesRxAutoTuneStart(devNum,portGroup,serdesNum,rxTraining);
  seq = (txTraining) ? &hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ] : &hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ];
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesRxAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
*       txTraining - Tx Training (true/false)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesRxAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL rxTraining
)
{
  MV_CFG_SEQ *seq;
  GT_U32 baseAddr;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  if (rxTraining == GT_TRUE)
  {
    /* DFE scaling */
    CHECK_STATUS(dfeScalingCheck(devNum, portGroup, serdesNum, scalingThr));
    /*  set bit 15 of 0x168 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));
    /* unset bit 7 of 0x18 - ffe_setting_force  */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, 0, (1 << 7)));
  }

  seq = (rxTraining) ? &hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ] : &hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ];
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
  if (rxTraining == GT_TRUE)
  {
    /* run enable trigger in loop to get average result value */
    CHECK_STATUS(getFfeAvarege(devNum, portGroup, serdesNum));
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesTxAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
*       txTraining - Tx Training (true/false)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesTxAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL txTraining
)
{
  MV_CFG_SEQ *seq;
  GT_U32 baseAddr;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

  if (txTraining == GT_TRUE)
  {
    /* DFE scaling */
    CHECK_STATUS(dfeScalingCheck(devNum, portGroup, serdesNum, scalingThr));

    /* start training */
    baseAddr = unitAddr + unitIndex*serdesNum;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_3, (1 << 8), (1 << 8)));
  }
  else
  {
    seq = &hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
  }

  return GT_OK;
}


/*******************************************************************************
* mvHwsComHSerdesAutoTuneStart
*
* DESCRIPTION:
*       Per SERDES control the TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       rxTraining - Rx Training (true/false)
*       txTraining - Tx Training (true/false)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL rxTraining,
    GT_BOOL txTraining
)
{

    CHECK_STATUS(mvHwsComHSerdesRxAutoTuneStart(devNum, portGroup, serdesNum, rxTraining));

    CHECK_STATUS(mvHwsComHSerdesAutoTuneCfg(devNum, portGroup, serdesNum, rxTraining, txTraining));
    CHECK_STATUS(mvHwsComHSerdesTxAutoTuneStart(devNum, portGroup, serdesNum, txTraining));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesAutoTuneStatus
*
* DESCRIPTION:
*       Per SERDES check the Rx & Tx training status
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       rxTraining - Rx Training (true/false)
*       txTraining - Tx Training (true/false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesAutoTuneStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
  MV_CFG_SEQ *seq;
  GT_U32 retryNum;

  /* Rx Status */
  retryNum= 10;
  *rxStatus = TUNE_NOT_COMPLITED;

  while(*rxStatus != TUNE_PASS && retryNum > 0)
  {
    CHECK_STATUS(mvHwsComHSerdesAutoTuneRxStatus(devNum, portGroup, serdesNum, rxStatus));

    retryNum--;

    if(*rxStatus == TUNE_FAIL && retryNum > 0)
    {
      /* start training again */
      seq = &hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ];
      CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
    }
  }

  /* Tx Status */
  retryNum= 10;
  *txStatus = TUNE_NOT_COMPLITED;

  while(*txStatus != TUNE_PASS && retryNum > 0)
  {
    CHECK_STATUS(mvHwsComHSerdesAutoTuneTxStatus(devNum, portGroup, serdesNum, txStatus));

    retryNum--;

    if(*txStatus == TUNE_FAIL && retryNum > 0)
    {
      /* start training again */
      seq = &hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ];
      CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
    }
    }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesAutoTuneRxStatus
*
* DESCRIPTION:
*       Per SERDES check the Rx training status
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       rxTraining - Rx Training (true/false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS mvHwsComHSerdesAutoTuneRxStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus
)
{
  MV_CFG_SEQ *seq;
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 status;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum;

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_3, &data, 0));
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_STATUS_1, &status, 0));

  /* check RX status */
  if ((data >> 7) & 1)
  {
    if (status & 1)
    {
      if ((status >> 1) & 1)
      {
        *rxStatus = TUNE_FAIL;
      }
      else
      {
        *rxStatus = TUNE_PASS;
      }
    }
    else
    {
      *rxStatus = TUNE_NOT_COMPLITED;
    }
    /* set RX done */
    if ((*rxStatus == TUNE_FAIL) || (*rxStatus == TUNE_PASS))
    {
      seq = &hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ];
      CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
    }
    if ((*rxStatus == TUNE_PASS) && (runAlign90opt))
    {
        CHECK_STATUS(align90opt(devNum,portGroup,serdesNum));
    }
  }
  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesAutoTuneTxStatus
*
* DESCRIPTION:
*       Per SERDES check the Tx training status
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       txTraining - Tx Training (true/false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS mvHwsComHSerdesAutoTuneTxStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
  MV_CFG_SEQ *seq;
  GT_U32 baseAddr;
  GT_U32 data, adaptTx, emph0, emph1, ampTx;
  GT_U32 status;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_3, &data, 0));
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_STATUS_1, &status, 0));

  /* check TX status */
  if ((data >> 8) & 1)
  {
    if ((status >> 2) & 1)
    {
      if ((status >> 3) & 1)
      {
        *txStatus = TUNE_FAIL;
      }
      else
      {
        *txStatus = TUNE_PASS;
        baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x1FC, 1, 0));
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x110, &adaptTx, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x1FC, 0, 0));
        ampTx = (adaptTx >> 11) & 0x1F;
        if (ampTx == 15)
        {
            ampTx = 31;
        }
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_TX_AMP_Reg, (ampTx << 1), (0x1F << 1)));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_TX_AMP_Reg, ((adaptTx >> 10) & 0x1) << 6, (0x1 << 6)));
        emph0 = (adaptTx >> 6) & 0xF;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_GEN1_SET2_Reg, emph0, 0xF));
        if ((emph0 + 9) < 0xf)
        {
            emph1 = emph0 + 9;
        }
        else
        {
            emph1 = 0xF;
        }

        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_TX_AMP_Reg, (emph1 << 7), (0xF << 7)));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_GEN1_SET2_Reg, 1 << 4, (1 << 4)));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_TX_AMP_Reg, 1 << 11, (0x1 << 11)));
      }
    }
    else
    {
      *txStatus = TUNE_NOT_COMPLITED;
    }

    /* set TX done */
    if ((*txStatus == TUNE_FAIL) || (*txStatus == TUNE_PASS))
    {
      seq = &hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ];
      CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
    }
    if ((*txStatus == TUNE_PASS) && (runAlign90opt))
    {
        CHECK_STATUS(align90opt(devNum,portGroup,serdesNum));
    }
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesAutoTuneResult
*
* DESCRIPTION:
*       Per SERDES return the adapted tuning results
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       results - the adapted tuning results.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesAutoTuneResult
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_RESULTS *results
)
{
  GT_U32 data;
  GT_U32 baseAddr;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  if (results == NULL)
  {
    return GT_BAD_PTR;
  }
  CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  genRegisterGet(devNum, portGroup, baseAddr + COM_H_FFE_Reg, &data, 0);
  results->ffeR = U32_GET_FIELD(data, 12, 3);
  results->ffeC = U32_GET_FIELD(data, 8, 4);

  genRegisterGet(devNum, portGroup, baseAddr + COM_H_OS_PH_Reg, &data, 0);
  results->sampler = U32_GET_FIELD(data, 9, 7);

  genRegisterGet(devNum, portGroup, baseAddr + COM_H_RX_FFE_Reg, &data, 0);
  results->sqleuch = U32_GET_FIELD(data, 8, 5);

  genRegisterGet(devNum, portGroup, baseAddr + COM_H_TX_AMP_Reg, &data, 0);
  results->txAmp = U32_GET_FIELD(data, 1, 5);

  genRegisterGet(devNum, portGroup, baseAddr + COM_H_GEN1_SET2_Reg, &data, 0);
  results->txEmph0 = U32_GET_FIELD(data, 0, 4);

  genRegisterGet(devNum, portGroup, baseAddr + COM_H_TX_AMP_Reg, &data, 0);
  results->txEmph1 = U32_GET_FIELD(data, 7, 4);

  genRegisterGet(devNum, portGroup, baseAddr + COM_H_DFE_Reg, &data, 0);
  results->dfeVals[0] = U32_GET_FIELD(data, 10, 6);
  results->dfeVals[1] = U32_GET_FIELD(data, 4, 5);
  /* bit 9 is the sign */
  if ((data >> 9) & 1)
  {
    results->dfeVals[1] = results->dfeVals[1] * (-1);
  }
  results->dfeVals[3] = U32_GET_FIELD(data, 0, 3);
  /* bit 3 is the sign */
  if ((data >> 3) & 1)
  {
    results->dfeVals[3] = results->dfeVals[3] * (-1);
  }

  genRegisterGet(devNum, portGroup, baseAddr + COM_H_DFE_Reg1, &data, 0);
  results->dfeVals[2] = U32_GET_FIELD(data, 8, 4);
  /* bit 12 is the sign */
  if ((data >> 12) & 1)
  {
    results->dfeVals[2] = results->dfeVals[2] * (-1);
  }
  results->dfeVals[4] = U32_GET_FIELD(data, 4, 3);
  /* bit 7 is the sign */
  if ((data >> 7) & 1)
  {
    results->dfeVals[4] = results->dfeVals[4] * (-1);
  }

  results->dfeVals[5] = U32_GET_FIELD(data, 0, 3);
  /* bit 3 is the sign */
  if ((data >> 3) & 1)
  {
    results->dfeVals[5] = results->dfeVals[5] * (-1);
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesFfeConfig
*
* DESCRIPTION:
*       Per SERDES configure FFE.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       ffeRes    - FFE R value
*       ffeCap    - FFE C value
*       align     - sampler value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesFfeConfig
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  serdesNum,
  GT_U32  ffeRes,
  GT_U32  ffeCap,
  GT_U32  alig
)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_32  curVal, tmpAlig;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

    /* set bit 7 to 1 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, (1 << 7), (1 << 7)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, (ffeRes << 4), (7 << 4)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, ffeCap, 0xF));

    /*  set bit 15 of 0x168 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));

    /* config align90 */
    /* read current align90 value */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, &data, 0));
    curVal = (data >> 8) & 0x7F;
    tmpAlig = alig;
    /* move to align90 value with step = 4 */
    if (((curVal - 4) > tmpAlig) || ((GT_U32)(curVal + 4) < alig))
    {
        while((curVal - 4) > tmpAlig)
        {
            curVal -= 4;
            CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));
            CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (curVal << 8), 0x7F00));
        }
        while((GT_U32)(curVal + 4) < alig)
        {
            curVal += 4;
            CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));
            CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (curVal << 8), 0x7F00));
        }
    }
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (1 << 15), (1 << 15)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (alig << 8), 0x7F00));


    return GT_OK;
}

/*******************************************************************************
* mvHwsComHIfClose
*
* DESCRIPTION:
*       Release all system resources allocated by Serdes IF functions.
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
void mvHwsComHIfClose(void)
{
  hwsComPhyHSeqFree();
  hwsInitDone = GT_FALSE;
}

/* local functions */
static GT_STATUS getFfeAvarege
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
)
{
#define  RX_PASS_NUM (1)
  GT_U32 i;
  GT_U32 curFfe[RX_PASS_NUM];
  GT_U32 avFfeR, avFfeC;
  GT_U32 baseAddr;
  GT_U32 data, status;
  GT_U32 passNum = 0;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum;
  for (i = 0; (i < 100) && (passNum < RX_PASS_NUM); i++)
  {
    /* disable RX training */
    genRegisterSet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_3, 0, (1 << 7));
    hwsOsTimerWkFuncPtr(1);
    /* enable RX training */
    genRegisterSet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_3, (1 << 7), (1 << 7));
    hwsOsTimerWkFuncPtr(1);
    /* check RX status */
    genRegisterGet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_3, &data, 0);
    genRegisterGet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_STATUS_1, &status, 0);
    if ((data >> 7) & 1)
    {
      if (status & 1)
      {
        if ((status >> 1) & 1)
        {
          continue;
        }
        else
        {
          /* RX TUNE_PASS */
          /* get results */
          genRegisterGet(devNum, portGroup, baseAddr + SERDES_PHY_REGS + COM_H_FFE_Reg, &curFfe[passNum], 0);
          passNum++;
        }
      }
      else
      {
        continue;
      }
    }
  }
  if ((i == 100) && (passNum != 10))
  {
    return GT_FAIL;
  }

  /* calculate average FFE */
  avFfeC = 0;
  avFfeR = 0;
  for (i = 0; i < RX_PASS_NUM; i++)
  {
    avFfeC += U32_GET_FIELD(curFfe[i], 8, 4);
    avFfeR += U32_GET_FIELD(curFfe[i], 12, 3);
  }
  /* round the average FFE_C up (maximum is 0xF) */
  if ((avFfeC % RX_PASS_NUM) < 5)
  {
    avFfeC = (avFfeC / RX_PASS_NUM);
  }
  else
  {
    avFfeC = (avFfeC / RX_PASS_NUM);
    if (avFfeC < 0xF)
    {
      avFfeC += 1;
    }
  }
  avFfeR = (avFfeR / RX_PASS_NUM);

  /* force results */
  genRegisterSet(devNum, portGroup, baseAddr + SERDES_PHY_REGS+COM_H_RX_FFE_Reg, (1 << 7), (1 << 7));
  genRegisterSet(devNum, portGroup, baseAddr + SERDES_PHY_REGS+COM_H_RX_FFE_Reg, (avFfeR << 4), (7 << 4));
  genRegisterSet(devNum, portGroup, baseAddr + SERDES_PHY_REGS+COM_H_RX_FFE_Reg, avFfeC, 0xF);
  return GT_OK;
}


static GT_32 dfeFMinus1Cal(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
  GT_32 f0a, f0b;
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1B8, (9 << 3), (0x1F << 3)));
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+0x1B4, &data, 0));
  f0a = (data >> 10) & 0x3F;

  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1B8, (0x12 << 3), (0x1F << 3)));
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+0x1B4, &data, 0));
  f0b = (data >> 8) & 0x3F;

  return (f0b - f0a);
}

static GT_STATUS align90fine(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum, GT_32 fminus1)
{
  GT_32  cur_align90;
  GT_U32 baseAddr, baseExtAddr;
  GT_U32 data;
  GT_U32 loopCounter;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
  baseExtAddr = unitAddr + unitIndex*serdesNum;
  genRegisterGet(devNum, portGroup, baseAddr+COM_H_OS_PH_Reg, &data, 0);
  cur_align90 = (data >> 9) & 0x7F;

  if (fminus1 > 0)
  {
    /* force align90 value - 1 */
    genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, ((cur_align90-1) << 8), (0x7F << 8));
  }
  else if (fminus1 < 0)
  {
    /* force align90 value + 1 */
    genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, ((cur_align90+1) << 8), (0x7F << 8));
  }
  hwsOsTimerWkFuncPtr(1);

  if (fminus1 != 0)
  {
    genRegisterSet(devNum, portGroup, baseExtAddr+SERDES_EXTERNAL_CONFIGURATION_2, 0x20, 0x20); /* DFE_PAT_DIS=1 */
    genRegisterSet(devNum, portGroup, baseExtAddr+SERDES_EXTERNAL_CONFIGURATION_3, 0x80, 0x80); /* RX_TRAIN_EN=1 */

    /* poll on Rx_train_complete + not Rx_train_fail */
    loopCounter = 0;
    do
    {
      genRegisterGet(devNum, portGroup, baseExtAddr+SERDES_EXTERNAL_STATUS_1, &data, 0);
      hwsOsTimerWkFuncPtr(1);
      loopCounter++;
    } while(((data & 3) != 1) && (loopCounter < 10));
    if ((loopCounter == 10) && ((data & 3) != 1))
    {
      return GT_FAIL;
    }

    hwsOsTimerWkFuncPtr(1);
    genRegisterSet(devNum, portGroup, baseExtAddr+SERDES_EXTERNAL_CONFIGURATION_2, 0, 0x20); /* DFE_PAT_DIS=0 */
    genRegisterSet(devNum, portGroup, baseExtAddr+SERDES_EXTERNAL_CONFIGURATION_3, 0, 0x80); /* RX_TRAIN_EN=0 */
  }

  return GT_OK;
}

static GT_STATUS align90opt(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
  GT_32  fminus1,cur_align90;
  GT_U32 baseAddr;
  GT_U32 counter = 0;
  GT_U32 data, tmp;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_OS_PH_Reg, &data, 0));
  cur_align90 = (data >> 9) & 0x7F;

  /*  go to align90StartTap with step = 4 */
  tmp = cur_align90 + 4;
  while (tmp < align90StartTap)
  {
    genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (tmp << 8), (0x7F << 8));
    tmp += 4;
  }
  tmp = cur_align90 - 4;
  while (tmp > align90StartTap)
  {
      genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (tmp << 8), (0x7F << 8));
      tmp -= 4;
  }
  genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (align90StartTap << 8), (0x7F << 8));

  /* DFE_RES = 0x1 */
  /*CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_DFE_LOCK_Reg, 0xD147, 0));*/

  /* Read FFE results and force them. */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_FFE_Reg, &data, 0));
  tmp = (1 << 7) + (((data >> 12) & 0x7) << 4) + ((data >> 8) & 0xF);
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, tmp, 0xFF));

  fminus1 = dfeFMinus1Cal(devNum, portGroup, serdesNum);
  while ((ABS(fminus1) > fminus1_thr ) && (counter < fminus1Counter))
  {
    CHECK_STATUS(align90fine(devNum, portGroup, serdesNum, fminus1));
    fminus1 = dfeFMinus1Cal(devNum, portGroup, serdesNum);
    counter++;
  }
  if (counter == fminus1Counter)
  {
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RxClockAl90, (cur_align90 << 8), (0x7F << 8)));
  }

  return GT_OK;
}


/*For each Serdes in the port make new global function, call it DFEOpti(CoreNum,SerdesNum)

Read current adapted FFE_R (offset 0x1A4[14:12]) call it TRN_FFE_R.
Scan From TRN_FFE_R - 2 (>=0) to FFE_R + 2 (<=7) and look for the following criteria with 1mS delay each time
Make global F3_factor=3, F4_Factor=2, F5_factor=1.
DFE_F3 = offset 0x2C[3:0] without sign bit [2:0].
DFE_F4 = offset 0x30[7:4] without sign bit [6:4].
DFE_F5 = offset 0x30[3:0] without sign bit [2:0].
Best FFE_R is the one that makes Min{F3_Factor*abs(DFE_F3)+ F4_Factor*abs(DFE_F4)+ F5_Factor*abs(DFE_F5)}
*/
GT_STATUS mvHwsComHDfeOpti(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum, GT_U32 *bestFfe)
{
  GT_U32 trnFfeR;
  GT_U32 baseAddr;
  GT_U32 startScan, endScan, i;
  GT_U32 data, dfeF3, dfeF4, dfeF5, dfeNotOk;
  GT_U32 factorArr[7];
  GT_U32 minVal;
  GT_U32 F3_factor = 3;
  GT_U32 F4_factor = 2;
  GT_U32 F5_factor = 1;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

    if (!runDfeOpti)
    {
        return GT_OK;
    }
  if (bestFfe == NULL)
  {
      return GT_BAD_PARAM;
  }
  hwsOsMemSetFuncPtr(factorArr, 0, sizeof(factorArr));

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_FFE_Reg, &data, 0));
  trnFfeR = (data >> 12) & 0x7;

  switch (trnFfeR)
  {
  case 0:
      startScan = 0;
      endScan = 4;
      break;
  case 1:
      startScan = 0;
      endScan = 3;
      break;
  case 2:
      startScan = 0;
      endScan = 4;
      break;
  case 3:
      startScan = 1;
      endScan = 5;
      break;
  case 4:
      startScan = 2;
      endScan = 6;
      break;
  case 5:
      startScan = 3;
      endScan = 7;
      break;
  case 6:
      startScan = 2;
      endScan = 7;
      break;
  case 7:
      startScan = 3;
      endScan = 7;
      break;
  default:
      return GT_BAD_VALUE;
  }
  for (i = startScan; i <= endScan ; i++)
  {
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, (1 << 7), (1 << 7)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_RX_FFE_Reg, (i << 4), (7 << 4)));
        dfeNotOk = 2;
        while (dfeNotOk)
        {
                        hwsOsExactDelayPtr(devNum, portGroup, 2);
            /* check PHY ready */
            CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_SEL_BITS_Reg, &data, 0));
            if (((data >> 6) & 1) == 0)
            {
                dfeNotOk--;
                CHECK_STATUS(genRegisterSet(devNum, portGroup, SERDES_EXTERNAL_CONFIGURATION_2 + unitAddr + unitIndex*serdesNum, 0, (1 << 4)));
                CHECK_STATUS(genRegisterSet(devNum, portGroup, SERDES_EXTERNAL_CONFIGURATION_2 + unitAddr + unitIndex*serdesNum, 1, (1 << 4)));
            }
            else
            {
                dfeNotOk = 0;
            }
        }
        if (((data >> 6) & 1) == 0)
        {
            return GT_FAIL;
        }
        /* DFE scaling */
        CHECK_STATUS(dfeScalingCheck(devNum, portGroup, serdesNum, scalingThr));
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_DFE_Reg, &dfeF3, 0));
    dfeF3 &= 0x7;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_DFE_Reg1, &data, 0));
    dfeF4 = (data >> 4) & 7;
    dfeF5 = data & 7;
    factorArr[i] = (dfeF3 * F3_factor) + (dfeF4 * F4_factor) + (dfeF5 * F5_factor);
  }

  /* search for min value in factorArr - FFER index */
  minVal = 0xFFFFFFFF;
  *bestFfe = 0;

  for (i = startScan; i <= endScan ; i++)
  {
    /*if ((factorArr[i] == minVal) && ((*bestFfe + 1) == i))
        {
      *bestFfe = i;
        }*/
        if (factorArr[i] < minVal)
        {
            minVal = factorArr[i];
            *bestFfe = i;
        }
  }
    if (*bestFfe < endScan)
    {
        if ((factorArr[*bestFfe] + bestFfeAdd) >= factorArr[*bestFfe + 1])
        {
            *bestFfe = *bestFfe + 1;
        }
    }

  return GT_OK;
}

static GT_STATUS mvHwsComHCheckSerdesAccess(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum;
  /* check analog reset */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_1, &data, 0));
  if(((data >> 3) & 1) == 0)
      return GT_NOT_INITIALIZED;

  return GT_OK;
}

static GT_STATUS dfeScalingCheck(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum, GT_U32 threshold)
{
    GT_U32 baseAddr;
    GT_U32 data, dfeRes, tmp;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

    for (dfeRes = 0; dfeRes < 4; dfeRes++)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_DFE_LOCK_Reg, (dfeRes << 8), (3 << 8)));
        /* wait 1 msec */
                hwsOsExactDelayPtr(devNum, portGroup, 1);
        /* check DFE_F0 */
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_DFE_Reg, &data, 0));
        tmp = ((data >> 10) & 0x3F);
        if (tmp > threshold)
        {
            continue;
        }
        else
        {
            return GT_OK;
        }
    }
    return GT_OK;
}

void mvHwsSetScalingThreshold(GT_U32 val)
{
    scalingThr = val;
}
void mvHwsSetdfeFMinus1Cal(GT_U32 thr, GT_U32 counter)
{
    fminus1_thr = thr;
    fminus1Counter = counter;
}
void mvHwsGlobalCfg(GT_U32 align, GT_U32 dfeOpti, GT_U32 tap, GT_U32 add)
{
    runAlign90opt = align;
    runDfeOpti = dfeOpti;
    align90StartTap = tap;
    bestFfeAdd = add;
}

/*******************************************************************************
* mvHwsComHSerdesTxEnable
*
* DESCRIPTION:
*       Enable/Disable Tx.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       enable    - whether to enable or disable Tx.
*
* OUTPUTS:
*       results - the calibration results.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesTxEnable
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    GT_BOOL     enable
    )
{
    GT_U32 baseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_1,
        (enable << 2), (1 << 2)));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesTxEnableGet
*
* DESCRIPTION:
*        Enable / Disable transmission of packets in SERDES layer of a port.
*        Use this API to disable Tx for loopback ports.
*
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       enablePtr       - GT_TRUE  - Enable transmission of packets in
*                                    SERDES layer of a port
*                       - GT_FALSE - Disable transmission of packets in
*                                    SERDES layer of a port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, serdesNum
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*
* COMMENTS:
*       Disabling transmission of packets in SERDES layer of a port causes
*       to link down of devices that are connected to the port.
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesTxEnableGet
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL *enablePtr
)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHCheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_1, &data, (1 << 2)));
    *enablePtr = (data) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}
