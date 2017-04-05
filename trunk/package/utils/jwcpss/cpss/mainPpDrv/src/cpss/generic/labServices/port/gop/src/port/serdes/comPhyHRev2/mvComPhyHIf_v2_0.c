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
*       $Revision: 108 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <serdes/mvHwsSerdesIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/comPhyHRev2/mvComPhyHRev2Db.h>
#include <serdes/comPhyHRev2/mvComPhyHRev2If.h>
#include <serdes/comPhyH/mvComPhyHIf.h>
#include <serdes/comPhyH/mvComPhyEomIf.h>


/**************************** Globals ****************************************************/

static GT_U32 timer_dfe2 = 6;
static GT_U32 timer_dfe1 = 6;
static GT_U32 timer_f0d2 = 6;
static GT_U32 timer_f0d1 = 1;
static GT_U32 timer_dfe3 = 6;
static GT_U32 timer_edge1 = 6;
static GT_U32 timer_edge2 = 6;
static GT_U32 timer_f0b1 = 4;
static GT_U32 timer_f0b2 = 6;

static GT_U32 internalPuDelay = 1;
static GT_U32 rxTrDly = 20;
static GT_U32 highThr = 110;
static GT_U32 lowThr  = 80;
static GT_U32 calcDelay = 1;

static GT_32 dfeThreshold = 3;
static GT_U32 dfeCheckRun = 0;

GT_U32 stopPrbs = 1;
GT_U32 os_delta_max_2 = 0x14;

/* General Optimization values */
GT_32 optPrintLevel = 0;		  /* Optimization print level 0=No, 1=Partial, 2=Full		*/

/* DFE Opt */
GT_U32 eyeCenterPopulation = 1000;
GT_U32 eyeMeasurmentPopulation = 10000;

/* FFE Opt */
GT_U32 minDfeRes = 1;
GT_32  ffeFirstTH = 0;			  /* Threshold for ffe optimizer for first iteration		*/
GT_32  ffeFinalTH = 0;			  /* Threshold for ffe optimizer for second iteration and on*/
GT_U32 ffecSteps  = 8;			  /* ffe-C step on FFE optimization							*/
GT_BOOL forceFfeVal = GT_FALSE;	  /* "0"-use default init&limit ffeRC,"1"-use settings below*/
GT_U32 initFfeR = 0;			  /* Init ffeR value to be used if forceFfeVal="1"			*/
GT_U32 initFfeC = 0xF;			  /* Init ffeC value to be used if forceFfeVal="1"			*/
GT_U32 limitFfeR = 3;			  /* The limit of ffeR value to be used if forceFfeVal="1"	*/
GT_U32 limitFfeC = 0;			  /* The limit of ffeC value to be used if forceFfeVal="1"	*/

/* Align90 Opt */
GT_U32 triggerAStart = 1;	      /* Align90 opt filter level for Right data				*/
GT_U32 triggerBStart = 1;	      /* Align90 opt filter level for Left data					*/
GT_32  shiftAlign90  = 0;	      /* Adding constant shift to the align90 optimization		*/
GT_U32 f0dStopTH = 9;			  /* F0d Stop Thereshold									*/

/* Delay parameters */
GT_U32 dfeDelayInterval = 10;
GT_U32 dfeDelayDuration = 0;
GT_U32 align90DelayDuration = 0;

static GT_BOOL hwsInitDone = GT_FALSE;

GT_U32 trainExtMask = 0x3;

/* these variables allow to reduce training trials */
GT_U32 tuneRetryNum = 1;
GT_U32 tuneTimeoutCount = 20;

/* enables/disabled tune config WA */
GT_BOOL enableTuneCfgWa = GT_TRUE;

void mvHwsComHRev2SetTuneMode(GT_BOOL singleStepTune)
{
	tuneRetryNum = (singleStepTune) ? 1: 20;
	tuneTimeoutCount = (singleStepTune) ? 20 : 100;

	trainExtMask = (singleStepTune) ? 0 : 0x3; /* disable/enable additional tuning options */
}

/**************************** Definition *************************************************/

#define MV_HWS_ALIGN90_VEC_SIZE		128

#define MV_DEBUG_SERDES_OPT

#ifdef  MV_DEBUG_SERDES_OPT
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
#include <cpss/extServices/private/prvCpssBindFunc.h>
#define DEBUG_SERDES_OPT(level,s) if (optPrintLevel >= level) {cpssOsPrintf s;}
#else
#include <gtOs/gtOsIo.h>
#define DEBUG_SERDES_OPT(level,s) if (optPrintLevel >= level) {osPrintf s;}
#endif
#else /* MV_DEBUG_SERDES_OPT */
#define DEBUG_SERDES_OPT(level,s)
#endif


#define MV_HWS_MAX_LANES_PER_PORT	4


/**************************** Pre-Declaration ********************************************/

GT_STATUS mvHwsComHRev2SerdesDfeOpt
(
	GT_U8  devNum,
	GT_U32 portGroup,
	GT_U32 serdesNum
);

GT_STATUS mvHwsComHRev2SerdesFfeOpt
(	
	GT_U8						devNum,
	GT_U32						portGroup,
	GT_U32						*serdesList, 
	GT_U32						numOfActLanes,
	MV_HWS_PORT_MAN_TUNE_MODE	portTuningMode
);

GT_STATUS mvHwsComHRev2SerdesAlign90Opt
(	
	GT_U8 devNum, 
	GT_U32 portGroup,  
	GT_U32 *serdesList, 
	GT_U32 numOfActLanes
);

GT_U32 mvHwsComHRev2SerdesFixDfeResolution
(
	GT_U8   devNum, 
	GT_U32  portGroup, 
	GT_U32  serdesNum
);

GT_STATUS mvHwsComHRev2SerdesPrintAll
(	GT_U8		devNum,
	GT_U32		portGroup,
	GT_U32		serdesNum,
	GT_32		optPrintLevel
);

static GT_STATUS mvHwsComHRev2CheckSerdesAccess
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  serdesNum
);

static GT_STATUS mvHwsComHRev2SerdesAutoTuneStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 isTx,
    MV_HWS_AUTO_TUNE_STATUS *status
);

static GT_STATUS mvHwsRev2SqlchImp
(
    GT_U8 devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
);

static char* mvHwsSerdesRev2TypeGetFunc(void)
{
  return "COM_H_REV2";
}

/*******************************************************************************
* mvHwsComHRev2IfInit
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
GT_STATUS mvHwsComHRev2IfInit(MV_HWS_SERDES_FUNC_PTRS *funcPtrArray)
{

  /* use PHY_H_V1.0 implementation */
#ifndef CO_CPU_RUN
  funcPtrArray[COM_PHY_H_REV2].serdesLoopbackGetFunc = mvHwsComHSerdesLoopbackGet;
  funcPtrArray[COM_PHY_H_REV2].serdesEomGetFunc = mvHwsComHSerdesEOMGet;
  funcPtrArray[COM_PHY_H_REV2].serdesPolarityCfgGetFunc = mvHwsComHSerdesPolarityConfigGet;
#endif
  funcPtrArray[COM_PHY_H_REV2].serdesLoopbackCfgFunc = mvHwsComHSerdesLoopback;
  funcPtrArray[COM_PHY_H_REV2].serdesResetFunc = mvHwsComHSerdesReset;
  funcPtrArray[COM_PHY_H_REV2].serdesDfeCfgFunc = mvHwsComHSerdesDfeConfig;
  funcPtrArray[COM_PHY_H_REV2].serdesDfeStatusFunc = mvHwsComHSerdesDfeStatus;
  funcPtrArray[COM_PHY_H_REV2].serdesPolarityCfgFunc = mvHwsComHSerdesPolarityConfig;
  funcPtrArray[COM_PHY_H_REV2].serdesFfeCfgFunc = mvHwsComHSerdesFfeConfig;

  if (hwsInitDone == GT_FALSE)
  {
      /* init PHY_H_Rev1 sequences */
      CHECK_STATUS(hwsComPhyHSeqInit());

      /* init PHY_H_Rev2 sequences */
      CHECK_STATUS(hwsComPhyHRev2SeqInit());
  }

  /* new implementation */
  funcPtrArray[COM_PHY_H_REV2].serdesAutoTuneStatusShortFunc = mvHwsComHRev2SerdesAutoTuneStatusShort;
  funcPtrArray[COM_PHY_H_REV2].serdesTestGenFunc        = mvHwsComHRev2SerdesTestGen;
  funcPtrArray[COM_PHY_H_REV2].serdesPowerCntrlFunc     = mvHwsComHRev2SerdesPowerCtrl;
  funcPtrArray[COM_PHY_H_REV2].serdesManualTxCfgFunc    = mvHwsComHRev2SerdesManualTxConfig;
  funcPtrArray[COM_PHY_H_REV2].serdesManualRxCfgFunc    = mvHwsComHRev2SerdesManualRxConfig;
  funcPtrArray[COM_PHY_H_REV2].serdesPpmCfgFunc         = mvHwsComHRev2SerdesPpmConfig;
  funcPtrArray[COM_PHY_H_REV2].serdesTypeGetFunc        = mvHwsSerdesRev2TypeGetFunc;
  funcPtrArray[COM_PHY_H_REV2].serdesAutoTuneCfgFunc    = mvHwsComHRev2SerdesAutoTuneCfg;
  funcPtrArray[COM_PHY_H_REV2].serdesRxAutoTuneStartFunc= mvHwsComHRev2SerdesRxAutoTuneStart;
  funcPtrArray[COM_PHY_H_REV2].serdesTxAutoTuneStartFunc= mvHwsComHRev2SerdesTxAutoTuneStart;
  funcPtrArray[COM_PHY_H_REV2].serdesAutoTuneStartFunc  = mvHwsComHRev2SerdesAutoTuneStart;
  funcPtrArray[COM_PHY_H_REV2].serdesAutoTuneResultFunc = mvHwsComHRev2SerdesAutoTuneResult;
  funcPtrArray[COM_PHY_H_REV2].serdesAutoTuneStatusFunc = mvHwsComHRev2SerdesAutoTuneStatus;
  funcPtrArray[COM_PHY_H_REV2].serdesDfeStatusExtFunc   = mvHwsComHRev2SerdesDfeStatusExt;
  funcPtrArray[COM_PHY_H_REV2].serdesDfeOptiFunc        = NULL; /* N/A in this serdes */
  funcPtrArray[COM_PHY_H_REV2].serdesCalibrationStatusFunc = mvHwsComHRev2SerdesCalibrationStatus;
  funcPtrArray[COM_PHY_H_REV2].serdesTxEnableFunc       = mvHwsComHRev2SerdesTxEnable;
  funcPtrArray[COM_PHY_H_REV2].serdesTestGenStatusFunc  = mvHwsComHRev2SerdesTestGenStatus;
  funcPtrArray[COM_PHY_H_REV2].serdesTxIfSelectFunc     = mvHwsComHRev2SerdesTxIfSelect;
  funcPtrArray[COM_PHY_H_REV2].serdesSqlchCfgFunc       = mvHwsComHRev2SqlchCfg;

#ifndef CO_CPU_RUN
  funcPtrArray[COM_PHY_H_REV2].serdesTestGenGetFunc     = mvHwsComHRev2SerdesTestGenGet;
  funcPtrArray[COM_PHY_H_REV2].serdesTxEnableGetFunc    = mvHwsComHRev2SerdesTxEnableGet;
  funcPtrArray[COM_PHY_H_REV2].serdesPpmGetFunc         = mvHwsComHRev2SerdesPpmConfigGet;
  funcPtrArray[COM_PHY_H_REV2].serdesSeqGetFunc         = hwsComPhyHRev2SeqGet;
  funcPtrArray[COM_PHY_H_REV2].serdesSeqSetFunc         = hwsComPhyHRev2SeqSet;
#endif

  funcPtrArray[COM_PHY_H_REV2].serdesAcTermCfgFunc      = mvHwsComHRev2SerdesAcTerminationCfg;
  funcPtrArray[COM_PHY_H_REV2].serdesArrayPowerCntrlFunc= mvHwsComHRev2SerdesArrayPowerCtrl;
  funcPtrArray[COM_PHY_H_REV2].serdesDigitalReset       = mvHwsComHRev2DigitalSerdesReset;
  funcPtrArray[COM_PHY_H_REV2].serdesFixAlign90Start    = mvHwsComHRev2SerdesFixAlign90Start;
  funcPtrArray[COM_PHY_H_REV2].serdesFixAlign90Status   = mvHwsComHRev2SerdesFixAlign90Status;
  funcPtrArray[COM_PHY_H_REV2].serdesFixAlign90Stop     = mvHwsComHRev2SerdesFixAlign90Stop;
  funcPtrArray[COM_PHY_H_REV2].serdesAutoTuneStopFunc   = mvHwsComHRev2SerdesTxAutoTuneStop;
  funcPtrArray[COM_PHY_H_REV2].serdesDfeCheckFunc           = mvHwsComHRev2DfeCheck;
  funcPtrArray[COM_PHY_H_REV2].serdesSpeedGetFunc       = mvHwsComHRev2SerdesSpeedGet;
  funcPtrArray[COM_PHY_H_REV2].serdesManualRxCfgGetFunc = mvHwsComHRev2SerdesManualRxConfigGet;
  funcPtrArray[COM_PHY_H_REV2].serdesSignalDetectGetFunc = mvHwsComHRev2SerdesSignalDetectGet;
  funcPtrArray[COM_PHY_H_REV2].serdesCdrLockStatusGetFunc = mvHwsComHRev2SerdesCdrLockStatusGet;

  hwsInitDone = GT_TRUE;

  return GT_OK;
}

static GT_STATUS hwsComHRev2SerdesFastCornerFix
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum
)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

    /* Read speedPLL : read page 0 offset 0x208[3:0]. */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_RX_DFE_Reg, &data, 0));

    if ((data & 0xF) <= 11)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x38,  0xfd2, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x154, 7, 0));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesTestGen
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
GT_STATUS mvHwsComHRev2SerdesTestGen
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

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

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

  patternData |= 0x20; /* PRBS load continuous mode */

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  /* set pattern type */
  baseAddr = unitAddr + SERDES_PHY_REGS + serdesNum * unitIndex;
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_Pattern_Reg, patternData, 0xFF));

  /* run pattern sequence */
  seq = (mode == SERDES_NORMAL) ? &hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ] : &hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ];
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

  return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsComHRev2SerdesTestGenGet
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
GT_STATUS mvHwsComHRev2SerdesTestGenGet
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

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

    /* get pattern type */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+
                                COM_H_REV2_Pattern_Reg, &data, 0xFF));

    data &= 0xDF; /* clear PRBS load continuous mode bit */

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
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+
                                COM_H_REV2_PT_PATTERN_SEL_Reg, &data, 0x8000));
    *modePtr = (0 == data) ? SERDES_NORMAL : SERDES_TEST;

    return GT_OK;
}
#endif

static GT_STATUS hwsFixCalOs(GT_U8 devNum, GT_U32 portGroup, GT_U32 serdesNum)
{
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 i;
    GT_U32 err;
    GT_U32 cal;
    GT_BOOL pass;

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + SERDES_PHY_REGS + serdesNum * unitIndex;

    pass = GT_FALSE;
    for (i = 0; i < 20; i++)
    {
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x1e4, &data, 0));
        err = (data >> 7) & 1;
        cal = data & 0x7F;
        if (err || (cal > highThr) || (cal < lowThr))
        {
            genRegisterSet(devNum, portGroup,baseAddr+0x15c,1<<10,1<<10);  /* assert align90 calibration */
            hwsOsExactDelayPtr(devNum, portGroup, 1);
            genRegisterSet(devNum, portGroup,baseAddr+0x15c,0, 1<<10);     /* de-assert align90 calibration*/
            hwsOsExactDelayPtr(devNum, portGroup, calcDelay);
        }
        else
        {
            pass = GT_TRUE;
            break;
        }
    }
    if ((i == 20) && (pass == GT_FALSE))
    {
        genRegisterSet(devNum, portGroup,baseAddr+0x15c, 1<<4, 1<<4);  /* assert align90 reset */
        hwsOsExactDelayPtr(devNum, portGroup, 1);
        genRegisterSet(devNum, portGroup,baseAddr+0x15c, 0, 1<<4);     /* de-assert align90 reset */

        return GT_OK;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesPowerCtrl
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
GT_STATUS mvHwsComHRev2SerdesPowerCtrl
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
  GT_U32 data, i;
  GT_U32 mask;
  GT_U32 unitAddr;
  GT_U32 unitIndex;
  MV_HWS_CALIBRATION_RESULTS  results;

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
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

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
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_EXTERNAL_CONFIGURATION_0;
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr, data, mask));

  /* serdes power down */

  /*seq = &hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ];
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));*/

  if (powerUp)
  {
    /* Serdes Analog Un Reset*/
    CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_FALSE, GT_TRUE, GT_TRUE));

    /* Reference clock source */
    baseAddr = unitAddr + unitIndex*serdesNum + 0x33c;
    data = (refClockSource == PRIMARY) ? 0 : 1;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr, (data << 10), (1 << 10)));

    /* Serdes Speed */
    switch(baudRate)
    {
    case _1_25G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_1_25G_SEQ];
      break;
    case _3_125G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_3_125G_SEQ];
      break;
    case _3_33G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_3_3G_SEQ];
      break;
    case _3_75G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_3_75G_SEQ];
      break;
    case _4_25G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_4_25G_SEQ];
      break;
    case _5G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_5G_SEQ];
      break;
    case _6_25G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_6_25G_SEQ];
      break;
    case _7_5G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_7_5G_SEQ];
      break;
    case _10_3125G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_10_3125G_SEQ];
      break;
    case _11_5625G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_11_5625G_SEQ];
      break;
    case _12_5G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_12_5G_SEQ];
      break;
    case _12_1875G:
      seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_12_1875G_SEQ];
      break;
    case SPEED_NA:
    default:
      return GT_FAIL;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    /* Soft Reset (writ pulse) */  /* Lion2B addition */
    /*
    data = mask = (1 << 10);
    baseAddr = SERDES_PHY_REGS + serdesNum * SERDES_IND_OFFSET + COM_H_REV2_SoftReset_Reg;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr, data, mask));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr, 0, mask));
    */
    /* wait 1 msec */
    hwsOsExactDelayPtr(devNum, portGroup, internalPuDelay);

    /* Serdes Power up Ctrl */
    seq = &hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    /* Serdes wait PLL (first time) */
    seq = &hwsSerdesRev2SeqDb[SERDES_SERDES_WAIT_PLL_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    /* VDD calibration start (pulse) */
    baseAddr = unitAddr + SERDES_PHY_REGS + serdesNum * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_CalibrReg, (1 << 2), (1 << 2)));
    /* wait 1 msec */
    hwsOsExactDelayPtr(devNum, portGroup, internalPuDelay);

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_CalibrReg, 0, (1 << 2)));

    if (baudRate == _3_33G)
    {
        /* change FBDiv to give 3.33G */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x14C, 0x2404, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x150, 0x8043, 0));
        /* wait 1 msec */
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }

    if (baudRate == _11_5625G)
    {
        /* change FBDiv to 0x25 give 11.5625G */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x14C, 0xA254, 0));
        /* wait 1 msec */
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }

    /* check serdes state */
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_EXTERNAL_STATUS_0;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr, &data, 0));
    if (baudRate == _1_25G)
    {
        /* for 1.25G no rx init check */
        if ((data & 0xc) != 0xc)
        {
          return GT_FAIL;
        }
    }
    else
    {
        if ((data & 0x1c) != 0x1c)
        {
          return GT_FAIL;
        }
    }

    /* ext calibration calc & force */
    mvHwsComHRev2SerdesCalibrationStatus(devNum,portGroup,serdesNum,&results);
    for (i = 0; i < 8; i++)
    {
        if (results.ffeCal[i] < 2)
        {
            if (i == 7)
            {
                results.ffeCal[i] = results.ffeCal[i-1];
            }
            else
            {
                results.ffeCal[i] = results.ffeCal[i+1];
            }
        }
    }
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x1FC, 1, 0));
    /* Write back FFE_CAL[0:3] */
    data = (results.ffeCal[0] << 12) + (results.ffeCal[1] << 8) +
        (results.ffeCal[2] << 4) + results.ffeCal[3];
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0xF4, data, 0));
    /* Write back FFE_CAL[4:7] */
    data = (results.ffeCal[4] << 12) + (results.ffeCal[5] << 8) +
        (results.ffeCal[6] << 4) + results.ffeCal[7];
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0xF8, data, 0));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0xF0, (1 << 14), (1 << 14)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x1FC, 0, 0));

    /* Serdes wait PLL (second time) */
    seq = &hwsSerdesRev2SeqDb[SERDES_SERDES_WAIT_PLL_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    /* Serdes Rx Init up */
    if (baudRate != _1_25G)
    {
        seq = &hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ];
        CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
    }

    /* Fix potential align90 calibration errors */
    hwsFixCalOs(devNum, portGroup, serdesNum);

    if (baudRate == _12_1875G)
    {
        /* change FBDiv to 0x27 give 12.1875G */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x14C, 0xA277, 0));
        /* wait 10 msec */
        hwsOsExactDelayPtr(devNum, portGroup, 10);
    }

    /* Serdes Digital Un Reset */
    CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_FALSE, GT_FALSE, GT_FALSE));
    hwsComHRev2SerdesFastCornerFix(devNum,portGroup,serdesNum);
  }
  else
  {
    seq = &hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesManualTxConfig
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
GT_STATUS mvHwsComHRev2SerdesManualTxConfig
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

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  /* read CID reg for serdes revision */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_CID_Reg, &data, 0));
  if ((data & 0xFF) == 0x25)
  {
      /* Puma3B0 addition */
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_GEN1_SET1_Reg, (txAmpShft << 15), (1 << 15)));
  }
  else
  {
      /* Lion2B addition */
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_TX_AMP_Reg, txAmpShft, (1 << 0)));
  }

  data = (txAmpAdj ? 1:0);
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_TX_AMP_Reg, (data << 6), (1 << 6)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_TX_AMP_Reg, (txAmp << 1), 0x3E));

  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_GEN1_SET2_Reg, emph0, 0xF));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_GEN1_SET2_Reg, (1 << 4), (1 << 4)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_TX_AMP_Reg, (emph1 << 7), 0x780));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_TX_AMP_Reg, (1 << 11), (1 << 11)));

  /* Lion2B addition */
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 1, (1 << 0)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG1_Reg, (1 << 2), (1 << 2)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0, (1 << 0)));

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesManualRxConfig
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
GT_STATUS mvHwsComHRev2SerdesManualRxConfig
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
  GT_U32 sqlchImprove;

  sqlch = sqlch;

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  if (portTuningMode == StaticLongReach)
  {
    /* Long Reach - set highest DFE resolution */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_DFE_LOCK_Reg, 0xD047, 0));
  }
  else
  {
    /* Short Reach - set almost highest DFE resolution */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_DFE_LOCK_Reg, 0xD147, 0));
  }

  /* read bit 15 from 0x1BC, if == 1, set to 0 */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_Ext_Squelch_Calibr, &data, 0));
  sqlchImprove = (data >> 15) & 1;
  if (sqlchImprove)
  {
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_Ext_Squelch_Calibr, 0, (1 << 15)));
  }

  /* set bit 7 to 1 */
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_RX_FFE_Reg, (1 << 7), (1 << 7)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_RX_FFE_Reg, (ffeRes << 4), (7 << 4)));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_RX_FFE_Reg, ffeCap, 0xF));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_RX_FFE_Reg, (sqlch << 8), 0x1F00));

  /* if bit 15 from 0x1BC was == 1, call to WA */
  if (sqlchImprove)
  {
      hwsOsTimerWkFuncPtr(1);
      mvHwsRev2SqlchImp(devNum,portGroup,serdesNum);
  }

  data = (dfeEn  ? 1:0);
  CHECK_STATUS(genRegisterSet(devNum, portGroup, unitAddr + serdesNum * unitIndex + SERDES_EXTERNAL_CONFIGURATION_2, (data << 4), (1 << 4)));

  /*  set bit 7 of COM_H_REV2_OS_PH_OFFSET_Reg os_ph_force*/
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, (1 << 7), (1 << 7)));

  /* config align90 */
  /* read current align90 value */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_Reg, &data, 0));
  curVal = (data >> 9) & 0x7F;
  tmpAlig = alig;
  /* move to align90 value with step = 4 */
  if (((curVal - 4) > tmpAlig) || ((GT_U32)(curVal + 4) < alig))
  {
    while((curVal - 4) > tmpAlig)
    {
      curVal -= 4;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, curVal, 0x007F));
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, 1<<8, 1<<8));
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, 0, 1<<8));

    }
    while((GT_U32)(curVal + 4) < alig)
    {
      curVal += 4;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, curVal, 0x007F));
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, 1<<8, 1<<8));
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, 0, 1<<8));

    }
  }
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, alig, 0x007F));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, 1<<8, 1<<8));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, 0, 1<<8));

  if (dfeCheckRun)
  {
      /* dfe check */
      mvHwsComHRev2DfeCheck(devNum,portGroup,serdesNum);
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesPpmConfig
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
GT_STATUS mvHwsComHRev2SerdesPpmConfig
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

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
  puTxIntp = 1;
  switch (ppmVal)
  {
  case Neg_3_TAPS:
    txInitFoffset = 0x3;
    break;
  case Neg_2_TAPS:
    txInitFoffset = 0x2;
    break;
  case Neg_1_TAPS:
    txInitFoffset = 0x1;
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
  }

  CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_FALSE, GT_TRUE, GT_FALSE));

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_PU_TX_INTP_Reg, (puTxIntp << 11), (1 << 11)));

  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_PPM_Reg, txInitFoffset, 0x3FF));
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_PPM_Reg, (1 << 10), (1 << 10)));

  CHECK_STATUS(mvHwsComHSerdesReset(devNum,portGroup,serdesNum, GT_FALSE, GT_FALSE, GT_FALSE));

  return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsComHRev2SerdesPpmConfigGet
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
GT_STATUS mvHwsComHRev2SerdesPpmConfigGet
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    MV_HWS_PPM_VALUE  *ppmVal
)
{
  GT_U32 baseAddr;
  GT_U32 unitAddr;
  GT_U32 unitIndex;
  GT_U32 data;

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_PPM_Reg, &data, 0x3FF));

  switch (data)
  {
  case 0x3:
      *ppmVal = Neg_3_TAPS;
      break;
  case 0x2:
      *ppmVal = Neg_2_TAPS;
      break;
  case 0x1:
      *ppmVal = Neg_1_TAPS;
      break;
  case 0:
      *ppmVal = ZERO_TAPS;
      break;
  case 1023:
      *ppmVal = POS_1_TAPS;
      break;
  case 1022:
      *ppmVal = POS_2_TAPS;
      break;
  case 1021:
      *ppmVal = POS_3_TAPS;
      break;
  default:
    return GT_BAD_PARAM;
  }

  return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsComHRev2SerdesDfeStatusExt
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
*       f0Cfg     - array of f0 configuration parameters
*       savCfg    - array of sav configuration parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesDfeStatusExt
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            serdesNum,
    GT_BOOL           *dfeLock,
    GT_32             *dfeCfg,
    GT_32             *f0Cfg,
    GT_32             *savCfg
)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 i;
  GT_BOOL signBit;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
  for (i = 0; (f0Cfg) && (i < 3); i++)
  {
    f0Cfg[i] = 0;
  }

  for (i = 0; (savCfg) && (i < 5); i++)
  {
    savCfg[i] = 0;
  }

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  CHECK_STATUS(mvHwsComHSerdesDfeStatus(devNum, portGroup, serdesNum, dfeLock, dfeCfg));

  /* lion2B addition */

  /* go to page 1 */
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0x1, 0x1));

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG3_Reg, &data, 0));
  f0Cfg[0] = U32_GET_FIELD(data, 10, 6); /* F0a*/
  f0Cfg[1] = U32_GET_FIELD(data, 4, 6);  /* F0b*/

  signBit = U32_GET_FIELD(data, 3, 1);
  savCfg[2] = U32_GET_FIELD(data, 0, 3); /* Sav_f3 */
  if (signBit == GT_TRUE)
  {
    savCfg[2] |= 0x80000000;
  }

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG4_Reg, &data, 0));
  f0Cfg[2] = U32_GET_FIELD(data, 10, 6); /* F0d*/

  signBit = U32_GET_FIELD(data, 9, 1);
  savCfg[0] = U32_GET_FIELD(data, 4, 5); /* Sav_f1 */
  if (signBit == GT_TRUE)
  {
      savCfg[0] |= 0x80000000;
  }

  signBit = U32_GET_FIELD(data, 3, 1);
  savCfg[3] = U32_GET_FIELD(data, 0, 3); /* Sav_f4 */
  if (signBit == GT_TRUE)
  {
      savCfg[3] |= 0x80000000;
  }

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG5_Reg, &data, 0));

  /* Sav_f2 */
  signBit = U32_GET_FIELD(data, 8, 1);
  savCfg[1] = U32_GET_FIELD(data, 4, 4);
  if (signBit == GT_TRUE)
  {
      savCfg[1] |= 0x80000000;
  }

  /* Sav_f5 */
  signBit = U32_GET_FIELD(data, 3, 1);
  savCfg[4] = U32_GET_FIELD(data, 0, 3);
  if (signBit == GT_TRUE)
  {
      savCfg[4] |= 0x80000000;
  }

  /* return to page 0 */
  CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0x0, 0x1));

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesPostTuneCfgWa
*
* DESCRIPTION:
*       Per SERDES configure adding Initial internal training for forcing TX values to INIT level
*       done by loopback -> training -> sending INIT command
*
* INPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesPostTuneCfgWa
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  serdesNum
)
{
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 baseAddr;
	GT_U32 serdesRegAddr;

	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
	baseAddr = unitAddr + unitIndex * serdesNum;
	serdesRegAddr = baseAddr + SERDES_PHY_REGS;

	/* WA is needed for forcing TX values to INIT level in the Initial internal training */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x94, 0x4000, 0x4000));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x1FC, 0x1, 0x1));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x11C, 0x800, 0x800));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x9C, 0x3FF, 0xFFF));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x94, 0x197, 0xFFF));

	/* extrernal macro for remote access*/
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x00C,0xA800 ,0xFC00));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x020, 0x0, 0x100));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x020, 0x0, 0x0));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x020, 0x1, 0x1));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x020, 0x0, 0x1));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x00C, 0x0, 0xFC00));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x020, 0x0, 0x1));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x020, 0x1, 0x1));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x020, 0x0, 0x1));

	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x1FC, 0x0, 0x1));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x94, 0x0, 0x4000));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x1FC, 0x1, 0x1));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x11C, 0x0, 0x800));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesRegAddr + 0x1FC, 0x0 ,0x1));

	return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneCfg
*
* DESCRIPTION:
*       Per SERDES configure parameters for TX training & Rx Training starting
*       Can be run after create port.
*
* INPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesAutoTuneCfg
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL rxTraining,
    GT_BOOL txTraining
)
{
  MV_CFG_SEQ *seq;
  GT_U32 data;
  GT_U32 unitAddr;
  GT_U32 unitIndex;
  GT_U32 baseAddr;

  if (txTraining && rxTraining)
  {
      return GT_BAD_PARAM;
  }

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

  mvHwsComHRev2SerdesRxAutoTuneStart(devNum,portGroup,serdesNum,rxTraining);

  if((enableTuneCfgWa == GT_TRUE) && (txTraining == GT_TRUE))
  {
	/* Tx training is enabled so need to perform tune config WA */
	CHECK_STATUS(mvHwsComHRev2SerdesPostTuneCfgWa(devNum, portGroup, serdesNum));
  }

  seq = (txTraining) ? &hwsSerdesRev2SeqDb[SERDES_TX_TRAINING_ENABLE_SEQ] : &hwsSerdesRev2SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ];
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

  if (txTraining == GT_FALSE)
  {
      /* disable TX training, sync TX enbale/disable cfg */
      mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
      baseAddr = unitAddr + unitIndex * serdesNum;

      CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_1, &data, (1 << 2)));
      data = (data >> 2) & 1;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + SERDES_PHY_REGS+COM_H_REV2_Isolate_Reg, (data << 8), (1 << 8)));
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesFfeTableCfg
*
* DESCRIPTION:
*       Per SERDES config the FFE table
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
GT_STATUS mvHwsComHRev2SerdesFfeTableCfg
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
	MV_HWS_PORT_MAN_TUNE_MODE tuneMode
)
{
	MV_CFG_SEQ *seq;

	/* configure FFE table */
	seq = (tuneMode == StaticLongReach) ? &hwsSerdesRev2SeqDb[SERDES_FFE_TABLE_LR_SEQ] :
										  &hwsSerdesRev2SeqDb[SERDES_FFE_TABLE_SR_SEQ];

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

	return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesRxAutoTuneStart
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
GT_STATUS mvHwsComHRev2SerdesRxAutoTuneStart
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_BOOL rxTraining
)
{
  MV_CFG_SEQ *seq;
  GT_U32 data;
  GT_U32 baseAddr;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum;

#if 0
  /* start PRBS if not started yet */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + SERDES_PHY_REGS + COM_H_REV2_PT_PATTERN_SEL_Reg, &data, 0));
  if (!(data & 0x8000)) /* bit 15 - PHY test started */
  {
      /* start PRBS */
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SERDES_PHY_REGS + COM_H_REV2_Pattern_Reg, 0xC4, 0));
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SERDES_PHY_REGS + COM_H_REV2_PT_PATTERN_SEL_Reg, 0x80E0, 0));
      /* set flag - PRBS startrd by HWS in MetalFix Reg bit 15 */
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SD_METAL_FIX, 0x8000, 0x8000));
  }
#endif

  seq = (rxTraining) ? &hwsSerdesRev2SeqDb[SERDES_RX_TRAINING_ENABLE_SEQ] : &hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ];
  CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

  if (rxTraining == GT_FALSE)
  {
#if 0
      /* stop PRBS if started by HWS */
      CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+SD_METAL_FIX, &data, 0));
      if (data & 0x8000) /* bit 15 - PHY test started */
      {
          CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SERDES_PHY_REGS + COM_H_REV2_Pattern_Reg, 0, 0));
          CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SERDES_PHY_REGS + COM_H_REV2_PT_PATTERN_SEL_Reg, 0, 0));
          CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+SD_METAL_FIX, 0, 0x8000));
      }
#endif
      /* disable RX training, sync TX enbale/disable cfg */
      CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_1, &data, (1 << 2)));
      data = (data >> 2) & 1;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + SERDES_PHY_REGS+COM_H_REV2_Isolate_Reg, (data << 8), (1 << 8)));

  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesTxAutoTuneStart
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
GT_STATUS mvHwsComHRev2SerdesTxAutoTuneStart
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

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

  if (txTraining == GT_TRUE)
  {
    /* DFE scaling */
    /*CHECK_STATUS(dfeScalingCheck(devNum, portGroup, serdesNum, scalingThr)); Doron */

    /* start training */
    baseAddr = unitAddr + unitIndex*serdesNum;
    CHECK_STATUS(genRegisterSet(devNum, portGroup,baseAddr+SERDES_EXTERNAL_CONFIGURATION_3, (1 << 8), (1 << 8)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup,baseAddr+SERDES_PHY_REGS+0x1FC, 1, 1));
    CHECK_STATUS(genRegisterSet(devNum, portGroup,baseAddr+SERDES_PHY_REGS+0x2C, 2, 2));
    CHECK_STATUS(genRegisterSet(devNum, portGroup,baseAddr+SERDES_PHY_REGS+0x1FC, 0, 1));
  }
  else
  {
    seq = &hwsSerdesRev2SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneStart
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
GT_STATUS mvHwsComHRev2SerdesAutoTuneStart
(
 GT_U8   devNum,
 GT_U32  portGroup,
 GT_U32  serdesNum,
 GT_BOOL rxTraining,
 GT_BOOL txTraining
)
{

    if (txTraining && rxTraining)
    {
        return GT_BAD_PARAM;
    }

    /*CHECK_STATUS(mvHwsComHRev2SerdesRxAutoTuneStart(devNum, portGroup, serdesNum, rxTraining)); already done in AutoTuneCfg */

    CHECK_STATUS(mvHwsComHRev2SerdesAutoTuneCfg(devNum, portGroup, serdesNum, rxTraining, txTraining));
    CHECK_STATUS(mvHwsComHRev2SerdesTxAutoTuneStart(devNum, portGroup, serdesNum, txTraining));

  return GT_OK;
}

GT_STATUS mvHwsComHRev2SerdesExtTrainingRun
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
    GT_U32                  serdesNum,
	MV_HWS_AUTO_TUNE_STATUS *rxStatus
)
{
	GT_U32 retryNum;

	retryNum = tuneRetryNum;
	*rxStatus = TUNE_FAIL;

	/* start training again */
	CHECK_STATUS(mvHwsComHRev2SerdesRxAutoTuneStart(devNum, portGroup, serdesNum, GT_TRUE));

	while((*rxStatus == TUNE_FAIL) && (retryNum > 0))
	{
		/*CHECK_STATUS(UpdateTrainingCounter(devNum, portGroup, serdesNum));*/ /* Haim */

		CHECK_STATUS(mvHwsComHRev2SerdesAutoTuneStatusGet(devNum, portGroup, serdesNum, GT_FALSE, rxStatus));

		retryNum--;

		if((*rxStatus == TUNE_FAIL) && (retryNum > 0))
		{
			DEBUG_SERDES_OPT(2, ("*** (Re-Run) Restart training for serdes %d (iteration %d)\n", serdesNum, 20-retryNum));

			/* delay 500 ms */
			hwsOsExactDelayPtr(devNum, portGroup, 500);

			/* start training again */
			CHECK_STATUS(mvHwsComHRev2SerdesRxAutoTuneStart(devNum, portGroup, serdesNum, GT_TRUE));
		}
    }

	return GT_OK;
}

GT_STATUS mvHwsComHRev2SerdesConfigRegulator 
(
	GT_U8						devNum, 
	GT_U32						portGroup, 
	GT_U32						*serdesList,
	GT_U32                      numOfActLanes,
	GT_BOOL                     setDefault
)
{
	GT_U32 i;

	GT_U32 intBaseAddr[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 unitAddr;
	GT_U32 unitIndex;


	if (numOfActLanes > MV_HWS_MAX_LANES_PER_PORT) 
	{
		DEBUG_SERDES_OPT(0, ("Wrong Lanes number %d",numOfActLanes));
		return GT_BAD_PARAM;
	}

	
	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

	
	for (i = 0; i < numOfActLanes; i++)
	{
		if (serdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
		{
			continue;
		}

		intBaseAddr[i] = unitAddr + unitIndex * serdesList[i] + SERDES_PHY_REGS;

		if (setDefault == GT_TRUE)
		{
			 /* set to default value */
			CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x134, 0x3050, 0));
		}
		else
		{

			/* Configure regulator to high value */
			CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x134, 0x3070, 0));
		}
	

	}
	return GT_OK;
}


/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneStatus
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
GT_STATUS mvHwsComHRev2SerdesAutoTuneStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
    GT_U32 retryNum;
	/*
    MV_HWS_AUTO_TUNE_STATUS localRx = TUNE_FAIL;
    MV_HWS_AUTO_TUNE_STATUS localTx = TUNE_FAIL;
	*/

    /* Rx Status */
    if (rxStatus != NULL)
    {
        retryNum = tuneRetryNum;
        *rxStatus = TUNE_FAIL;

		DEBUG_SERDES_OPT(1, ("serdesNum %d Training\n", serdesNum));

        while((*rxStatus == TUNE_FAIL) && (retryNum > 0))
        {
            CHECK_STATUS(mvHwsComHRev2SerdesAutoTuneStatusGet(devNum, portGroup, serdesNum, GT_FALSE, rxStatus));

            retryNum--;

			if (retryNum == 10)
			{
				DEBUG_SERDES_OPT(2,("Configure regulator to high value\n"));
				CHECK_STATUS(mvHwsComHRev2SerdesConfigRegulator(devNum, portGroup, &serdesNum, 1, GT_FALSE));
			}

            if((*rxStatus == TUNE_FAIL) && (retryNum > 0))
            {
				DEBUG_SERDES_OPT(2, ("*** Restart training for serdes %d (iteration %d)\n", serdesNum, 20-retryNum));

                /* start training again */
                CHECK_STATUS(mvHwsComHRev2SerdesRxAutoTuneStart(devNum, portGroup, serdesNum, GT_TRUE));
            }
        }

		if((*rxStatus == TUNE_FAIL) && (trainExtMask & 0x1))
		{
			DEBUG_SERDES_OPT(1, ("serdesNum %d ExtTrainingRun\n", serdesNum));

			CHECK_STATUS(mvHwsComHRev2SerdesExtTrainingRun(devNum, portGroup, serdesNum, rxStatus));
		}

		if (retryNum <= 10)
		{
			DEBUG_SERDES_OPT(2,("Configure regulator to low value\n"));
			CHECK_STATUS(mvHwsComHRev2SerdesConfigRegulator(devNum, portGroup, &serdesNum, 1, GT_TRUE));
		}

        /*localRx = *rxStatus;*/
    }

    /* Tx Status */
    if (txStatus != NULL)
    {
        retryNum = (tuneRetryNum == 1) ? 1 : 10;
        *txStatus = TUNE_FAIL;

        while((*txStatus == TUNE_FAIL) && (retryNum > 0))
        {
            CHECK_STATUS(mvHwsComHRev2SerdesAutoTuneStatusGet(devNum, portGroup, serdesNum, GT_TRUE, txStatus));

            retryNum--;

            if ((*txStatus == TUNE_FAIL) && (retryNum > 0))
            {
                /* need to change back bug width to 16 bit before training */
                CHECK_STATUS(mvHwsComHRev2SerdesAutoTuneCfg(devNum, portGroup, serdesNum, GT_FALSE, GT_TRUE));
                /* start training again */
                CHECK_STATUS(mvHwsComHRev2SerdesTxAutoTuneStart(devNum, portGroup, serdesNum, GT_TRUE));
            }
        }

        /*localTx = *txStatus;*/
    }
#if 0
    if ((localTx == TUNE_PASS) || (localRx == TUNE_PASS))
    {
        CHECK_STATUS(hwsFixAlign90(devNum, portGroup, serdesNum));
        if (dfeCheckRun)
        {
            /* dfe check */
            mvHwsComHRev2DfeCheck(devNum,portGroup,serdesNum);
        }
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneStatusGet
*
* DESCRIPTION:
*       Per SERDES check Rx or Tx training status
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       isTx      - Rx or Tx
*
* OUTPUTS:
*       status - Training status (true/false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
#define POLLING_DELAY_TIME_CNS  25 /* msec */
static GT_STATUS mvHwsComHRev2SerdesAutoTuneStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 isTx,
    MV_HWS_AUTO_TUNE_STATUS *status
)
{
    GT_U32 timeout;
    MV_HWS_AUTO_TUNE_STATUS *rxStatus;
    MV_HWS_AUTO_TUNE_STATUS *txStatus;

    rxStatus = isTx ? NULL : status;
    txStatus = isTx ? status : NULL;
    timeout = tuneTimeoutCount; /* timeout = 100 * POLLING_DELAY_TIME_CNS msec */

    do
    {
        hwsOsExactDelayPtr(devNum, portGroup, POLLING_DELAY_TIME_CNS);
        CHECK_STATUS(mvHwsComHRev2SerdesAutoTuneStatusShort(devNum, portGroup,
                                                            serdesNum,
                                                            rxStatus,
                                                            txStatus));
    }while((--timeout > 0) && (TUNE_NOT_COMPLITED == *status));

    /* disable Rx or Tx training */
    if (isTx)
    {
        CHECK_STATUS(mvHwsComHRev2SerdesTxAutoTuneStart(devNum, portGroup,
                                                        serdesNum, GT_FALSE));
    }
    else
    {
        CHECK_STATUS(mvHwsComHRev2SerdesRxAutoTuneStart(devNum, portGroup,
                                                        serdesNum, GT_FALSE));
    }

    if (timeout == 0)
    {
        *status = TUNE_FAIL;
        return GT_TIMEOUT;
    }

    if (isTx && (*status != TUNE_FAIL))
    {
        GT_U32 baseAddr;
        GT_U32 unitAddr;
        GT_U32 unitIndex;
        GT_U32 tx2Data, tx3Data;

        mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
        /* this point means that Tx training pass */
        baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

        /* move to page 1 */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0x1, 0x1));

        /* get Manual Tx config parameters */
        genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_TX_TRAIN_Reg2, &tx2Data, 0);
        genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_TX_TRAIN_Reg3, &tx3Data, 0);

        /* move back to page 0 */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0x0, 0x1));

        CHECK_STATUS(mvHwsComHRev2SerdesManualTxConfig(devNum, portGroup, serdesNum,
                                                      U32_GET_FIELD(tx2Data, 11, 5),   /* Tx Amp      */
                                                       U32_GET_FIELD(tx2Data, 10, 1),   /* Tx Amp Adj  */
                                                       U32_GET_FIELD(tx2Data, 6,  4),   /* Tx Emph0    */
                                                       U32_GET_FIELD(tx2Data, 2,  4),   /* Tx Emph1    */
                                                       U32_GET_FIELD(tx3Data, 7,  1))); /* Tx Amp Shft */
        /* update Tx Train Emph 0/1 enable */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_GEN1_SET2_Reg,
                                    (U32_GET_FIELD(tx2Data, 0, 1) << 4), (1 << 4)));

        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_TX_AMP_Reg,
                                    (U32_GET_FIELD(tx2Data, 1, 1) << 11), (1 << 11)));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneResult
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
GT_STATUS mvHwsComHRev2SerdesAutoTuneResult
(
    GT_U8                    devNum,
    GT_U32                   portGroup,
    GT_U32                   serdesNum,
    MV_HWS_AUTO_TUNE_RESULTS *results
)
{
  GT_U32 data;
  GT_U32 baseAddr;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
  if (results == NULL)
  {
    return GT_BAD_PTR;
  }
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

  genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_FFE_Reg), &data, 0);
  results->ffeR = U32_GET_FIELD(data, 12, 3);
  results->ffeC = U32_GET_FIELD(data, 8, 4);

  genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_OS_PH_Reg), &data, 0);
  results->sampler = U32_GET_FIELD(data, 9, 7);

  genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_RX_FFE_Reg), &data, 0);
  results->sqleuch = U32_GET_FIELD(data, 8, 5);

  /* TX tuning results */
  CHECK_STATUS(genRegisterSet(devNum, portGroup, (baseAddr + COM_H_REV2_REG2_Reg), (1), (1 << 0)));
  genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_REG1_Reg), &data, 0);
  CHECK_STATUS(genRegisterSet(devNum, portGroup, (baseAddr + COM_H_REV2_REG2_Reg), (0), (1 << 0)));

  /* Check if Auto tuning */
  if ((data & 0x4) == 0)
  {
      CHECK_STATUS(genRegisterSet(devNum, portGroup, (baseAddr + COM_H_REV2_REG2_Reg), (1), (1 << 0)));
      genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_TX_TRAIN_Reg2), &data, 0);
      results->txAmp = U32_GET_FIELD(data, 11, 5);
      results->txAmpAdj = U32_GET_FIELD(data, 10, 1);
      results->txEmph0 = U32_GET_FIELD(data, 6, 4);
      results->txEmph1 = U32_GET_FIELD(data, 2, 4);
      results->txEmph1En = U32_GET_FIELD(data, 1, 1);
      results->txEmph0En = U32_GET_FIELD(data, 0, 1);
      genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_TX_TRAIN_Reg3), &data, 0);
      results->txAmpShft = U32_GET_FIELD(data, 7, 1);
      CHECK_STATUS(genRegisterSet(devNum, portGroup, (baseAddr + COM_H_REV2_REG2_Reg), (0), (1 << 0)));
  }
  else /* Manual tuning */
  {
      genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_TX_AMP_Reg), &data, 0);
      results->txAmp = U32_GET_FIELD(data, 1, 5);

      /* lion2B addition*/
      results->txAmpAdj = U32_GET_FIELD(data, 6, 1);

      results->txEmph1 = U32_GET_FIELD(data, 7, 4);

      /* lion2B addition*/
      results->txEmph1En = U32_GET_FIELD(data, 11, 1);

      genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_GEN1_SET2_Reg), &data, 0);
      results->txEmph0 = U32_GET_FIELD(data, 0, 4);

      /* lion2B addition*/
      results->txEmph0En = U32_GET_FIELD(data, 4, 1);

      /* read CID reg for serdes revision */
      CHECK_STATUS(genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_CID_Reg), &data, 0));
      if ((data & 0xFF) == 0x25)
      {
          /* Puma3B0 */
          genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_GEN1_SET1_Reg), &data, 0);
          results->txAmpShft = U32_GET_FIELD(data, 15, 1);
      }
      else
      {
          /* Lion2B */
          genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_TX_AMP_Reg), &data, 0);
          results->txAmpShft = U32_GET_FIELD(data, 0, 1);
      }
  }

  genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_DFE_Reg), &data, 0);
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

  genRegisterGet(devNum, portGroup,(baseAddr + COM_H_REV2_DFE_Reg1), &data, 0);
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
* mvHwsComHRev2SerdesCalibrationStatus
*
* DESCRIPTION:
*       Per SERDES return the calibration results.
*       Can be run after create port
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       results - the calibration results.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesCalibrationStatus
(
    GT_U8                       devNum,
    GT_U32                      portGroup,
    GT_U32                      serdesNum,
    MV_HWS_CALIBRATION_RESULTS  *results
)
{
    GT_U32 baseAddr;
    GT_U32 data, mask;
    GT_U32 vddCal[6];
    GT_U32 i;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

    /* Cal Done & Speed PLL */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_RX_DFE_Reg, &data, 0));
    results->calDone = (data >> 14) & 0x1;
    results->speedPll = data  & 0xF;

    /* Tx Imp Cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG6_Reg, &data, 0));
    results->txImpCal = (data >> 4) & 0xF;

    /* Rx Imp Cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG7_Reg, &data, 0));
    results->rxImpCal = (data >> 8) & 0xF;

    /* Process Cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG9_Reg, &data, 0));
    results->ProcessCal = (data >> 8) & 0xF;

    /* Sell V data */
    mask = (0x7 << 5);
    for (i=1; i<=6; i++)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG8_Reg, i << 5, mask));
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG8_Reg, &data, 0));
        vddCal[i-1] = data & 0x1F;
    }

    results->sellvTxClk     = vddCal[0];
    results->sellvTxData    = vddCal[1];
    results->sellvTxIntp    = vddCal[2];
    results->sellvTxDrv     = vddCal[3];
    results->sellvTxDig     = vddCal[4];
    results->sellvRxSample  = vddCal[5];

    /* go to page 1 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0x1, 0x1));

    /* FFE Done */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG10_Reg, &data, 0));
    results->ffeDone = (data >> 10) & 0x1;

    mask = (0x7 << 11);
    for (i = 0; i < 8; i++)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG10_Reg, i << 11, mask));
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG10_Reg, &data, 0));
        results->ffeCal[i] = (data >> 6) & 0xF;
    }

    /* go to page 0 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0, 0x1));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2TxDisable
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
GT_STATUS mvHwsComHRev2SerdesTxEnable
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      serdesNum,
    GT_BOOL     enable
)
{
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 baseAddr;

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + SERDES_EXTERNAL_CONFIGURATION_1, ((!enable) << 2), (1 << 2)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + SERDES_PHY_REGS+COM_H_REV2_Isolate_Reg, ((!enable) << 8), (1 << 8)));

    return GT_OK;
}

#ifndef CO_CPU_RUN
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
GT_STATUS mvHwsComHRev2SerdesTxEnableGet
(
 GT_U8   devNum,
 GT_U32  portGroup,
 GT_U32  serdesNum,
 GT_BOOL *enablePtr
 )
{
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 baseAddr;
    GT_U32 data;

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + COM_H_REV2_Isolate_Reg, &data, 0));
    *enablePtr = ((data >> 8) & 1) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsComHRev2SerdesTestGenStatus
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
GT_STATUS mvHwsComHRev2SerdesTestGenStatus
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

  CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_PT_PATTERN_SEL_Reg, &data, 0));
  status->lockStatus = (data >> 12) & 1;

  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_ERROR_Cntr_MSB_Reg, &data, 0));
  status->errorsCntr = (data << 16);
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_ERROR_Cntr_LSB_Reg, &data, 0));
  status->errorsCntr += data;
    /* read 48 bit counter */
  status->txFramesCntr.s[3] = 0;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_FRAME_Cntr_MSB1_Reg, &data, 0));
  status->txFramesCntr.s[2] = (GT_U16)data;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_FRAME_Cntr_MSB_Reg, &data, 0));
  status->txFramesCntr.s[1] = (GT_U16)data;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_FRAME_Cntr_LSB_Reg, &data, 0));
  status->txFramesCntr.s[0] = (GT_U16)data;

  /* reset counter */
  if (counterAccumulateMode == GT_FALSE)
  {
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_SEL_BITS_Reg, (1 << 11), (1 << 11)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_SEL_BITS_Reg, 0, (1 << 11)));
  }

  return 0;
}


/*******************************************************************************
* mvHwsComHRev2IfClose
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
void mvHwsComHRev2IfClose(void)
{
    hwsComPhyHSeqFree();
    hwsComPhyHRev2SeqFree();
    hwsInitDone = GT_FALSE;
}

static GT_STATUS mvHwsComHRev2CheckSerdesAccess(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum;
    /* check analog reset */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_1, &data, 0));
    if(((data >> 3) & 1) == 0)
        return GT_NOT_INITIALIZED;

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesTxIfSelect
*
* DESCRIPTION:
*       tx interface select.
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
GT_STATUS mvHwsComHRev2SerdesTxIfSelect
(
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
    GT_U32              serdesTxIfNum
)
{
    GT_U32 baseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_EXTERNAL_CONFIGURATION_3;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr, serdesTxIfNum, 7));

    return GT_OK;
}


GT_STATUS mvHwsComHRev2SqlchCfg
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_U32  sqlch
)
{
    GT_U32 baseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_RX_FFE_Reg, (sqlch << 8), 0x1F00));

    /* WA to improve signal detection */
    mvHwsRev2SqlchImp(devNum, portGroup, serdesNum);
    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2DfeCheck
*
* DESCRIPTION:
*       Check DFE values range and start sampler calibration, if needed.
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
GT_STATUS mvHwsComHRev2DfeCheck
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
)
{
  GT_U32 baseAddr, accessAddr, data;
  GT_32  dfeF0, dfeF1;
  GT_U32 unitAddr;
  GT_U32 unitIndex;
  GT_U32 tryNumber;

  if (!dfeCheckRun)
  {
      return GT_OK;
  }
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

  accessAddr = baseAddr + COM_H_REV2_DFE_Reg;
  tryNumber= 0;
  do
  {
      genRegisterGet(devNum, portGroup, accessAddr, &data, 0);
      dfeF0 = U32_GET_FIELD(data, 10, 6);
      dfeF1 = U32_GET_FIELD(data, 4, 5);
      /* bit 9 is the sign */
      if ((data >> 9) & 1)
      {
        dfeF1 = dfeF1 * (-1);
      }

      if ((dfeF0 - dfeF1) > dfeThreshold)
      {
          return GT_OK;
      }
      else
      {
          /* sampler calibration start */
          genRegisterSet(devNum, portGroup, baseAddr + COM_H_REV2_CalibrReg, (1 << 9), (1 << 9));
          hwsOsExactDelayPtr(devNum, portGroup, 1);
          genRegisterSet(devNum, portGroup, baseAddr + COM_H_REV2_CalibrReg, 0, (1 << 9));
          hwsOsExactDelayPtr(devNum, portGroup, 2);
      }

  }while (((dfeF0 - dfeF1) <= dfeThreshold) && (tryNumber++ < 10));

  if ((tryNumber == 10) && ((dfeF0 - dfeF1) <= dfeThreshold))
  {
      return GT_FAIL;
  }

  return GT_OK;
}

void mvHwsRev2GlobalCfg(GT_U32 puDelay, GT_U32 rxTranDelay, GT_U32 highThrVal, GT_U32 lowThrVal, GT_U32 calcDelayVal, GT_32 dfeThr, GT_U32 dfeCRun)
{
    internalPuDelay = puDelay;
    rxTrDly = rxTranDelay;
    highThr = highThrVal;
    lowThr  = lowThrVal;
    calcDelay = calcDelayVal;
    dfeThreshold = dfeThr;
    dfeCheckRun = dfeCRun;
}

/* WA to improve signal detection */
static GT_STATUS mvHwsRev2SqlchImp(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
    GT_U32 baseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 data;
    GT_U32 sq_cmpth_cal, sq_pkcm_cal, sq_thres_cal;

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

    /* write to 0x1E0 bits [15:14] */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG8_Reg, (1 << 14), (3 << 14)));
    /* store sq_cmpth_cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG8_Reg, &data, 0));
    sq_cmpth_cal = (data >> 8) & 0x3F;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG8_Reg, (2 << 14), (3 << 14)));
    /* store sq_pkcm_cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG8_Reg, &data, 0));
    sq_pkcm_cal = (data >> 8) & 0x3F;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG8_Reg, (3 << 14), (3 << 14)));
    /* store sq_cmpth_cal */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_REG8_Reg, &data, 0));
    sq_thres_cal = (data >> 8) & 0x3F;

    /* write results to external SQ */
    if (sq_cmpth_cal < 2)
        sq_cmpth_cal = 5;
    else
        sq_cmpth_cal= sq_cmpth_cal/2;

    data = (1 << 15) + (sq_cmpth_cal << 10) + (sq_pkcm_cal << 4) + (sq_thres_cal & 0xF);
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_Ext_Squelch_Calibr, data, 0));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesAutoTuneStatusShort
*
* DESCRIPTION:
*       Per SERDES check Rx or Tx training status
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       isTx      - Rx or Tx
*
* OUTPUTS:
*       status - Training status (true/false)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesAutoTuneStatusShort
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *rxStatus,
    MV_HWS_AUTO_TUNE_STATUS *txStatus
)
{
    GT_U32 serdesBaseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 data, data2;

    if ((NULL == rxStatus) && (NULL == txStatus))
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum, portGroup, serdesNum));
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    serdesBaseAddr = unitAddr + unitIndex * serdesNum;

    if (rxStatus != NULL)
    {
		*rxStatus = TUNE_NOT_COMPLITED;
        /* check that given type of training is enabled */
        CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr +
                                                SERDES_EXTERNAL_CONFIGURATION_3,
                                                                    &data, 0));
        data = (data >> 7) & 0x1; /* Rx - bit[7] */
        if (data != 0)
        {/* if application indeed initiated this type of training */
	        /* read status */
	        CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr +
	                                                        SERDES_EXTERNAL_STATUS_1,
	                                                                    &data, 0));
	        data &= 0x3; /* bit[1:0] for Rx */

	        if (data == 0x1)
	        {
	            *rxStatus = TUNE_PASS;
	        }
	        else if (data == 0x3)
	        {
	            *rxStatus = TUNE_FAIL;
	        }
		}
    }

    if (txStatus != NULL)
    {
		*txStatus = TUNE_NOT_COMPLITED;
        /* check that given type of training is enabled */
        CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr +
                                                SERDES_EXTERNAL_CONFIGURATION_3,
                                                                    &data, 0));
        data = (data >> 8) & 0x1; /* Tx - bit[8] */
        if (data == 0)
        {/* application didn't initiate this type of training */
            return GT_OK;
        }

        /* read status */
        CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr +
                                                        SERDES_EXTERNAL_STATUS_1,
                                                                    &data, 0));
        /* bit[3:2] for Tx */
        data = (data >> 2) & 0x3;
        if ((data != 0x1) && (data != 0x3))
        {
            return GT_OK;
        }

		/* move to page 1 */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr +
                                                        SERDES_PHY_REGS + 0x1FC,
                                                                    0x1, 0x1));

        /* read Remote PHY TX train complete interrupt.
            High means remote PHY has completed TX training. */
        CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr +
                                                        SERDES_PHY_REGS + 0xA8,
                                                            &data2, (1 << 9)));

		/* move back to page 0 */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr +
                                                        SERDES_PHY_REGS + 0x1FC,
                                                                    0x0, 0x1));

        if (0 == data2)
        {
			/* remote PHY didn't finish his training so Tx status is NOT_COMPLETED*/
            return GT_OK;
        }

        if (data == 0x1)
        {
            *txStatus = TUNE_PASS;
        }
        else if (data == 0x3)
        {
            *txStatus = TUNE_FAIL;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesTxAutoTuneStop
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
GT_STATUS mvHwsComHRev2SerdesTxAutoTuneStop
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum
)
{
	/* only disable TRx tuning (no training results copy) */
    CHECK_STATUS(mvHwsComHRev2SerdesTxAutoTuneStart(devNum, portGroup, serdesNum, GT_FALSE));

#if 0
    {
        GT_U32 baseAddr;
        GT_U32 unitAddr;
        GT_U32 unitIndex;
        GT_U32 tx2Data, tx3Data;

        /* get unit base address and unit index for current device */
        mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

        /* this point means that Tx training pass */
        baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

        /* move to page 1 */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0x1, 0x1));

        /* get Manual Tx config parameters */
        genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_TX_TRAIN_Reg2, &tx2Data, 0);
        genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_TX_TRAIN_Reg3, &tx3Data, 0);

        /* move back to page 0 */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_REG2_Reg, 0x0, 0x1));

        CHECK_STATUS(mvHwsComHRev2SerdesManualTxConfig(devNum, portGroup, serdesNum,
                                                       U32_GET_FIELD(tx2Data, 11, 5),   /* Tx Amp      */
                                                       U32_GET_FIELD(tx2Data, 10, 1),   /* Tx Amp Adj  */
                                                       U32_GET_FIELD(tx2Data, 6,  4),   /* Tx Emph0    */
                                                       U32_GET_FIELD(tx2Data, 2,  4),   /* Tx Emph1    */
                                                       U32_GET_FIELD(tx3Data, 7,  1))); /* Tx Amp Shft */
        /* update Tx Train Emph 0/1 enable */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_GEN1_SET2_Reg,
                                    (U32_GET_FIELD(tx2Data, 0, 1) << 4), (1 << 4)));

        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_TX_AMP_Reg,
                                    (U32_GET_FIELD(tx2Data, 1, 1) << 11), (1 << 11)));
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesFixAlign90Start
*
* DESCRIPTION:
*       Start fix Align90 process on current SERDES.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       params    - pointer to structure to store SERDES configuration parameters that must be restore
*                   during process stop operation
*
* OUTPUTS:
*       params    - SERDES configuration parameters that must be restore
*                   during process stop operation
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesFixAlign90Start
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_ALIGN90_PARAMS   *params
)
{
    GT_U32 align1;
    /*GT_U32 adapted_ffe_R, adapted_ffe_C;*/
    GT_U32 tmp, data;
    GT_U32 baseAddr, extBaseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    if (params == NULL)
    {
        return GT_BAD_PARAM;
    }

    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;
    extBaseAddr = unitAddr + unitIndex*serdesNum;

    /* store start value of Align90 for feature calculation */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x1A0, &align1, 0));
    params->startAlign90 = align1 >> 9;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x1A4, &data, 0));
    /*
    adapted_ffe_R = (data >> 12) & 7;
    adapted_ffe_C = (data >> 8) & 0xF;
    */

    params->adaptedFfeR = (data >> 12) & 7;
    params->adaptedFfeC = (data >> 8) & 0xF;
#if 0
    /* start PRBS if not started yet */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + COM_H_REV2_PT_PATTERN_SEL_Reg, &data, 0));
    if (!(data & 0x8000)) /* bit 15 - PHY test started */
    {
        /* start PRBS */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_REV2_Pattern_Reg, 0xC4, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_REV2_PT_PATTERN_SEL_Reg, 0x80E0, 0));
        /* set flag - PRBS started by HWS in MetalFix Reg bit 15 */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, extBaseAddr+SD_METAL_FIX, 0x8000, 0x8000));
    }
#endif
    /* force C & R */
    /*
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x18, ((1<<7) + (adapted_ffe_R<<4) + adapted_ffe_C), 0xff));
    */

    /* move to p1 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1FC, 1, 1));

    /* store os_delta_max_2 */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x120, &data, 0));
    params->osDeltaMax = data & 0x1f;
    /* os_delta_max_2 = 20 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x120, os_delta_max_2, 0x1f));

    /* store RX training control register 1 */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x38, &tmp, 0));
    params->rxTrainingCfg = tmp;

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x38, 0, 0));

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x130, 0, (1 << 7)));

    /* clear bit 11 of register 0x284 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + 0x84, 0x8078, 0xC87C));

    /* move to p0 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1FC, 0, 1));

    CHECK_STATUS(genRegisterGet(devNum, portGroup, extBaseAddr + 0, &data, 0));
    data = (data >> 7) & 0xF;

    /* Check if baud rate is below 7.5G */
    if (data < 0xC)
    {
        /* disable DFE pattern protect in these frequencies */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, extBaseAddr + 0x8, (1 << 5), (1 << 5)));
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1AC, (timer_dfe2 << 3), 7 << 3));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1AC, (timer_dfe1 << 6), 7 << 6));

    /* move to p1 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1FC, 1, 1));

    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x140, timer_f0d2, 7));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x140, (timer_f0b2 << 3), (7 << 3)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x140, (timer_edge2 << 6), (7 << 6)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x7C,  timer_f0d1, 7));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x7C,  (timer_f0b1 << 3), (7 << 3)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x7C, (timer_edge1 << 6), (7 << 6)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x7C, (timer_dfe3 << 9), (7 << 9)));

    /* move to p0 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1FC, 0, 1));

    /* start Rx training */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, extBaseAddr+0xC, 1<<7, 1<<7));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesFixAlign90Status
*
* DESCRIPTION:
*       Return fix Align90 process current status on current SERDES.
*       Can be run after create port and start Align90.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       trainingPass - true, if process finished and passed;
*                    - false, otherwise (doesn't finished or passed)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesFixAlign90Status
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    MV_HWS_AUTO_TUNE_STATUS *trainingStatus
)
{
    GT_U32 extBaseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 data;

    if (trainingStatus == NULL)
    {
        return GT_BAD_PARAM;
    }
    *trainingStatus = TUNE_NOT_COMPLITED;

    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    extBaseAddr = unitAddr + unitIndex*serdesNum;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, extBaseAddr + 0x1c, &data, 0));
    if (data & 1)
    {
        *trainingStatus = ((data >> 1) & 1) ? TUNE_FAIL : TUNE_PASS;
    }

#ifdef  MV_DEBUG_SERDES_OPT
        mvHwsComHRev2SerdesPrintAll(devNum, portGroup, serdesNum, 1);
#endif
    return GT_OK;
}

#if 0
/* config align90 */
static GT_STATUS hwsSetAlign90
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_U32                  alig
)
{
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 accBaseAddr;
    GT_U32 curVal, tmpAlign, data;
    GT_U32 align90Jump = 4;

    /* read current align90 value */
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

    accBaseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_Reg, &data, 0));
    curVal = (data >> 9) & 0x7F;
    tmpAlign = alig;

    /* move to align90 value with step = align90Jump (set force bit) */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, 0x80, 0x80));

    if (((curVal - align90Jump) > tmpAlign) || ((curVal + align90Jump) < alig))
    {
        while((curVal - align90Jump) >= tmpAlign)
        {
          curVal -= align90Jump;

          CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, curVal, 0x7F));
          CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, (1<<8), (1<<8)));
          CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, 0, (1<<8)));

          hwsOsExactDelayPtr(devNum, portGroup, 1);
        }
        while((curVal + align90Jump) <= alig)
        {
          curVal += align90Jump;

          CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, curVal, 0x7F));
          CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, (1<<8), (1<<8)));
          CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, 0, (1<<8)));

          hwsOsExactDelayPtr(devNum, portGroup, 1);
        }
    }

    CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, curVal, 0x7F));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, (1<<8), (1<<8)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, 0, (1<<8)));

    return GT_OK;
}

static void hwsReadDFE_F0F1
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_U32                  *F0,
    GT_32                   *F1
)
{
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 accBaseAddr;
    GT_U32 data;

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

    accBaseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
    genRegisterGet(devNum, portGroup, accBaseAddr + COM_H_REV2_DFE_Reg, &data, 0);
    *F0 = (data >> 10) & 0x3F;
    *F1 = (data >> 4) & 0x3F;

    if (*F1 > 31)
        *F1 = -(*F1>>1);

    return;
}
#endif

/*******************************************************************************
* mvHwsComHRev2SerdesFixAlign90Stop
*
* DESCRIPTION:
*       Stop fix Align90 process on current SERDES.
*       Can be run after create port and start Align90.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       params    - SERDES parameters that must be restored (return by mvHwsComHRev2SerdesFixAlign90Start)
*       fixAlignPass - true, if fix Align90 process passed; false otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesFixAlign90Stop
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 serdesNum,
    MV_HWS_ALIGN90_PARAMS *params,
    GT_BOOL fixAlignPass
)
{
    GT_U32 baseAddr, extBaseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 align, data;
    GT_U32 adapted_ffe_R = 0, adapted_ffe_C;
    GT_U32 adapted_ffe_R_2, adapted_ffe_C_2;

    if (params == NULL)
    {
        return GT_BAD_PARAM;
    }

    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;
    extBaseAddr = unitAddr + unitIndex * serdesNum;

    /* clear Rx training */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, extBaseAddr+0xC, 0, 1<<7));
#if 0
    /* stop PRBS if started by HWS */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, extBaseAddr+SD_METAL_FIX, &data, 0));
    /*if (data & 0x8000)*/ /* bit 15 - PHY test started */
    if ((data & 0x8000) && stopPrbs == 1)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_Pattern_Reg, 0, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_PT_PATTERN_SEL_Reg, 0, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, extBaseAddr+SD_METAL_FIX, 0, 0x8000));
    }
#endif
    /* put back read register */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1FC, 1, 1));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x38, params->rxTrainingCfg, 0));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x120, params->osDeltaMax, 0x1f));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x1FC, 0, 1));

    /* Improved FixALign90 results */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x1A4, &data, 0));
    adapted_ffe_R_2 = (data >> 12) & 7;
    adapted_ffe_C_2 = (data >> 8) & 0xF;

    if (params->adaptedFfeC > adapted_ffe_C_2)
    {
        adapted_ffe_C= params->adaptedFfeC;
    }
    else
    {
        adapted_ffe_C= adapted_ffe_C_2;
    }

    if ((params->adaptedFfeR == 0) || (adapted_ffe_R_2 == 0))
    {
		/* long reach */
        adapted_ffe_R = 0;
    }
	else if ((params->adaptedFfeR == 1) && (adapted_ffe_R_2 == 1))
    {
		/* long reach */
        adapted_ffe_R = 1;
    }
	else if ((params->adaptedFfeR == 2) && (adapted_ffe_R_2 == 2)) 
    {
		/* short reach */
        adapted_ffe_R = 2;
    }
	else if ((params->adaptedFfeR == 3) || (adapted_ffe_R_2 == 3)) 
    {
		/* short reach */
        adapted_ffe_R = 3;
    }

    /* force C & R */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x18, ((1<<7) + (adapted_ffe_R<<4) + adapted_ffe_C), 0xff));

    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x1A0, &align, 0));
    if (!fixAlignPass)
    {
        align = params->startAlign90;
    }
    else
    {
        align = (params->startAlign90 + (align >> 9))/2;
    }
    /* force os_ph_offset */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x188, align, 0x7F));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x188, 0x80, 0x80));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x188, (1 << 8), (1 << 8)));
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+0x188, 0, (1 << 8)));
#if 0
    if ((data & 0x8000) && stopPrbs == 2) /* bit 15 - PHY test started */
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_Pattern_Reg, 0, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr+COM_H_REV2_PT_PATTERN_SEL_Reg, 0, 0));
        CHECK_STATUS(genRegisterSet(devNum, portGroup, extBaseAddr+SD_METAL_FIX, 0, 0x8000));
    }	
#endif
    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesSpeedGet
*
* DESCRIPTION:
*       Return SERDES baud rate.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       baudRate - baud rate.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesSpeedGet
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 serdesNum,
    MV_HWS_SERDES_SPEED *rate
)
{
    GT_U32 extBaseAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 regData;

    if (rate == NULL)
    {
        return GT_BAD_PARAM;
    }

    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    extBaseAddr = unitAddr + unitIndex*serdesNum;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, extBaseAddr + SERDES_EXTERNAL_CONFIGURATION_0, &regData, 0));
    switch (regData & 0x7F8)
    {
    case 0x330:
        *rate = _1_25G;
        break;
    case 0x440:
        *rate = _3_125G;
        break;
    case 0x4C8:
        *rate = _3_75G;
        break;
    case 0x550:
        *rate = _4_25G;
        break;
    case 0x198:
        *rate = _5G;
        break;
    case 0x5D8:
        *rate = _6_25G;
        break;
    case 0x6E8:
        *rate = _7_5G;
        break;
    case 0x770:
        *rate = _10_3125G;
        break;
    /*case 0x7F8:
        *rate = _11_5625G;
        break;*/
    case 0x7F8:
        *rate = _12_5G;
        break;
    default:
        *rate = SPEED_NA;
    }
    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesManualRxConfigGet
*
* DESCRIPTION:
*       Return SERDES RX parameters configured manually .
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       sqlchPtr - Squelch threshold.
*       ffeResPtr - FFE R
*       ffeCapPtr - FFE C
*       aligPtr   - align 90 value
*
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesManualRxConfigGet
(
    IN GT_U8               devNum,
    IN GT_U32              portGroup,
    IN GT_U32              serdesNum,
    OUT MV_HWS_PORT_MAN_TUNE_MODE *portTuningModePtr,
    OUT GT_U32             *sqlchPtr,
    OUT GT_U32             *ffeResPtr,
    OUT GT_U32             *ffeCapPtr,
    OUT GT_BOOL			   *dfeEnPtr,
    OUT GT_U32             *aligPtr
)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;


    if ((portTuningModePtr == NULL) || (ffeResPtr == NULL) || (ffeCapPtr == NULL) || 
		(sqlchPtr == NULL) || (dfeEnPtr == NULL) || (aligPtr == NULL))
    {
        return GT_BAD_PARAM;
    }
    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

	/* read port tune mode */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_DFE_LOCK_Reg, &data, (1 << 8)));
	*portTuningModePtr = (data) ? StaticShortReach : StaticLongReach;

    /* set bit 7 to 1 */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_RX_FFE_Reg, &data, 0));
    *ffeResPtr = (data >> 4) & 7;
    *ffeCapPtr = data & 0xF;
    *sqlchPtr = (data >> 8) & 0x1F;

	CHECK_STATUS(genRegisterGet(devNum, portGroup, unitAddr + serdesNum * unitIndex + SERDES_EXTERNAL_CONFIGURATION_2, &data, 0));
	*dfeEnPtr = (data >> 4) & 0x1;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_REV2_OS_PH_OFFSET_Reg, &data, 0));
    *aligPtr = data & 0x7F;

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesArrayPowerCtrl
*
* DESCRIPTION:
*       Init physical port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       numOfSer  - number of SERDESes to configure
*       serdesArr - collection of SERDESes to configure
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
GT_STATUS mvHwsComHRev2SerdesArrayPowerCtrl
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  numOfSer,
    GT_U32                  *serdesArr,
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
  GT_U32 data, i,j;
  GT_U32 serdesNum, curPortGroup;
  GT_U32 mask;
  GT_U32 unitAddr;
  GT_U32 unitIndex;
  MV_HWS_CALIBRATION_RESULTS  results;

  refClock = refClock;

  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

  for (i = 0; i < numOfSer; i++)
  {
      serdesNum = (serdesArr[i] & 0xFFFF);
      curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

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

      CHECK_STATUS(genRegisterSet(devNum, curPortGroup, MG_Media_Interface_Reg, data, mask));

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
      baseAddr = unitAddr + unitIndex*serdesNum + SERDES_EXTERNAL_CONFIGURATION_0;
      CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr, data, mask));
  }

  /* serdes power up / down */
  if (powerUp)
  {
    for (i = 0; i < numOfSer; i++)
    {
        serdesNum = (serdesArr[i] & 0xFFFF);
        curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

        /* Serdes Analog Un Reset*/
        CHECK_STATUS(mvHwsComHSerdesReset(devNum,curPortGroup,serdesNum, GT_FALSE, GT_TRUE, GT_TRUE));

        /* Reference clock source */
        baseAddr = unitAddr + unitIndex*serdesNum + 0x33c;
        data = (refClockSource == PRIMARY) ? 0 : 1;
        CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr, (data << 10), (1 << 10)));

        /* Serdes Speed */
        switch(baudRate)
        {
        case _1_25G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_1_25G_SEQ];
          break;
        case _3_125G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_3_125G_SEQ];
          break;
        case _3_33G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_3_3G_SEQ];
          break;
        case _3_75G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_3_75G_SEQ];
          break;
        case _4_25G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_4_25G_SEQ];
          break;
        case _5G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_5G_SEQ];
          break;
        case _6_25G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_6_25G_SEQ];
          break;
        case _7_5G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_7_5G_SEQ];
          break;
        case _10_3125G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_10_3125G_SEQ];
          break;
        case _11_5625G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_11_5625G_SEQ];
          break;
        case _12_5G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_12_5G_SEQ];
          break;
        case _12_1875G:
          seq = &hwsSerdesRev2SeqDb[SERDES_SPEED_12_1875G_SEQ];
          break;
        case SPEED_NA:
        default:
          return GT_FAIL;
        }
        CHECK_STATUS(mvCfgSeqExec(devNum, curPortGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
    }

    /* Serdes Power up Ctrl */
    seq = &hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ];
    CHECK_STATUS(mvCfgSerdesSeqParallelExec(devNum, portGroup, numOfSer, serdesArr, seq->cfgSeq, seq->cfgSeqSize));

    /* Serdes wait PLL (first time) */
    seq = &hwsSerdesRev2SeqDb[SERDES_SERDES_WAIT_PLL_SEQ];
    CHECK_STATUS(mvCfgSerdesSeqParallelExec(devNum, portGroup, numOfSer, serdesArr, seq->cfgSeq, seq->cfgSeqSize));

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

    for (i = 0; i < numOfSer; i++)
    {
        serdesNum = (serdesArr[i] & 0xFFFF);
        curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

        /* VDD calibration start (pulse) */
        baseAddr = unitAddr + SERDES_PHY_REGS + serdesNum * unitIndex;
        CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr+COM_H_REV2_CalibrReg, (1 << 2), (1 << 2)));
    }

    /* wait 1 msec */
    hwsOsExactDelayPtr(devNum, portGroup, internalPuDelay);

    for (i = 0; i < numOfSer; i++)
    {
        serdesNum = (serdesArr[i] & 0xFFFF);
        curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

        baseAddr = unitAddr + SERDES_PHY_REGS + serdesNum * unitIndex;
        CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr+COM_H_REV2_CalibrReg, 0, (1 << 2)));
    }

    if (baudRate == _3_33G)
    {
        for (i = 0; i < numOfSer; i++)
        {
            serdesNum = (serdesArr[i] & 0xFFFF);
            curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

            baseAddr = unitAddr + SERDES_PHY_REGS + serdesNum * unitIndex;
            /* change FBDiv to give 3.33G */
            CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr+0x14C, 0x2404, 0));
            CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr+0x150, 0x8043, 0));
        }
        /* wait 1 msec */
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }
    if (baudRate == _11_5625G)
    {
        for (i = 0; i < numOfSer; i++)
        {
            serdesNum = (serdesArr[i] & 0xFFFF);
            curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

            baseAddr = unitAddr + SERDES_PHY_REGS + serdesNum * unitIndex;
            /* change FBDiv to 0x25 give 11.5625G */
            CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr+0x14C, 0xA254, 0));
        }
        /* wait 1 msec */
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }

    /* check serdes state */
    for (i = 0; i < numOfSer; i++)
    {
        serdesNum = (serdesArr[i] & 0xFFFF);
        curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

        baseAddr = unitAddr + unitIndex*serdesNum + SERDES_EXTERNAL_STATUS_0;
        CHECK_STATUS(genRegisterGet(devNum, curPortGroup, baseAddr, &data, 0));
        if (baudRate == _1_25G)
        {
            /* for 1.25G no rx init check */
            if ((data & 0xc) != 0xc)
            {
              return GT_FAIL;
            }
        }
        else
        {
            if ((data & 0x1c) != 0x1c)
            {
              return GT_FAIL;
            }
        }

        /* ext calibration calc & force */
        mvHwsComHRev2SerdesCalibrationStatus(devNum,curPortGroup,serdesNum,&results);
        for (j = 0; j < 8; j++)
        {
            if (results.ffeCal[j] < 2)
            {
                if (j == 7)
                {
                    results.ffeCal[j] = results.ffeCal[j-1];
                }
                else
                {
                    results.ffeCal[j] = results.ffeCal[j+1];
                }
            }
        }

        baseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
        CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr + 0x1FC, 1, 0));
        /* Write back FFE_CAL[0:3] */
        data = (results.ffeCal[0] << 12) + (results.ffeCal[1] << 8) +
            (results.ffeCal[2] << 4) + results.ffeCal[3];
        CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr + 0xF4, data, 0));
        /* Write back FFE_CAL[4:7] */
        data = (results.ffeCal[4] << 12) + (results.ffeCal[5] << 8) +
            (results.ffeCal[6] << 4) + results.ffeCal[7];
        CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr + 0xF8, data, 0));
        CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr + 0xF0, (1 << 14), (1 << 14)));
        CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr + 0x1FC, 0, 0));
    }

    /* Serdes wait PLL (second time) */
    seq = &hwsSerdesRev2SeqDb[SERDES_SERDES_WAIT_PLL_SEQ];
    CHECK_STATUS(mvCfgSerdesSeqParallelExec(devNum, portGroup, numOfSer, serdesArr, seq->cfgSeq, seq->cfgSeqSize));

    /* Serdes Rx Init up */
    if (baudRate != _1_25G)
    {
        seq = &hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ];
        CHECK_STATUS(mvCfgSerdesSeqParallelExec(devNum, portGroup, numOfSer, serdesArr, seq->cfgSeq, seq->cfgSeqSize));
    }

    /* Fix potential align90 calibration errors */
    for (i = 0; i < numOfSer; i++)
    {
        serdesNum = (serdesArr[i] & 0xFFFF);
        curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);
        hwsFixCalOs(devNum, curPortGroup, serdesNum);
    }

    if (baudRate == _12_1875G)
    {
        for (i = 0; i < numOfSer; i++)
        {
            serdesNum = (serdesArr[i] & 0xFFFF);
            curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

            baseAddr = unitAddr + SERDES_PHY_REGS + serdesNum * unitIndex;
            /* change FBDiv to 0x27 give 12.1875G */
            CHECK_STATUS(genRegisterSet(devNum, curPortGroup, baseAddr+0x14C, 0xA277, 0));
        }
        /* wait 10 msec */
        hwsOsExactDelayPtr(devNum, portGroup, 10);
    }

    /* Serdes Digital Un Reset */
    for (i = 0; i < numOfSer; i++)
    {
        serdesNum = (serdesArr[i] & 0xFFFF);
        curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

        CHECK_STATUS(mvHwsComHSerdesReset(devNum,curPortGroup,serdesNum, GT_FALSE, GT_FALSE, GT_FALSE));
        hwsComHRev2SerdesFastCornerFix(devNum, curPortGroup, serdesNum);
    }
  }
  else
  {
      for (i = 0; i < numOfSer; i++)
      {
        serdesNum = (serdesArr[i] & 0xFFFF);
        curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

        seq = &hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ];
        CHECK_STATUS(mvCfgSeqExec(devNum, curPortGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));
      }
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesAcTerminationCfg
*
* DESCRIPTION:
*       Configures AC termination on current serdes.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       serdesNum  - serdes number
*       acTermEn   - enable or disable AC termination
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesAcTerminationCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 acTermEn
)
{
    GT_U32 accessAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    CHECK_STATUS(mvHwsComHRev2CheckSerdesAccess(devNum,portGroup,serdesNum));

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    accessAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS + COM_H_REV2_RX_FFE_Reg;

    data = (acTermEn == GT_TRUE) ? (1 << 15) : 0;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, data, (1 << 15)));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2DigitalSerdesReset
*
* DESCRIPTION:
*       Run digital reset / unreset on current SERDES.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - serdes number
*       digitalReset - digital Reset (On/Off)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2DigitalSerdesReset
(
    GT_U8           devNum,
    GT_U32          portGroup,
    GT_U32          serdesNum,
    MV_HWS_RESET    digitalReset
)
{
    MV_CFG_SEQ *seq;

    seq = (digitalReset == RESET) ? &hwsSerdesSeqDb[SERDES_RF_RESET_SEQ] : &hwsSerdesSeqDb[SERDES_RF_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, serdesNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesSignalDetectGet
*
* DESCRIPTION:
*       Return Signal Detect state on PCS (true/false).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       signalDet - if true, signal was detected
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesSignalDetectGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 *signalDet
)
{
    GT_U32 regAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 i;

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    regAddr = unitAddr + unitIndex*serdesNum + SERDES_EXTERNAL_STATUS_0;

    for(i = 0; i < 10; i++)
    {
        /* Read PLL lock status bit 0x18[1]. */
        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        if ((data >> 1) & 1)
        {
            *signalDet = GT_FALSE; /* PLL is not in lock */
            return GT_OK;
        }
        hwsOsExactDelayPtr(devNum, portGroup, 1);
    }

    /* PLL is in lock */
    *signalDet = GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesCdrLockStatusGet
*
* DESCRIPTION:
*       Return SERDES CDR lock status (true - locked /false - notlocked).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       cdrLock  - if true, CDR was locked
*
* OUTPUTS:
*       cdrLock  - if true, CDR was locked
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesCdrLockStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_BOOL                 *cdrLock
)
{
    GT_U32 regAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    regAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS + COM_H_REV2_SEL_BITS_Reg;

    /* CDR lock dectection is enabled - set bit 8 */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x100, 0x100));

    /* read CDR lock status */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    if ((data >> 7) & 1)
    {
        *cdrLock = GT_TRUE; /* CDR is locked */
        return GT_OK;
    }

    /* CDR isnot locked */
    *cdrLock = GT_FALSE;
    return GT_OK;
}


/************************************************************************/

/************************************************************************/
/*             Serdes Training Optimization Algorithms                  */
/************************************************************************/

/*******************************************************************************
* mvHwsComHRev2SerdesTrainingOptimization
*
* DESCRIPTION:
*       Per SERDES Run the Serdes training optimization algorithms.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesList  - bit map of physical Serdes number
*		numOfActLanes - number of Serdes.  
*       tuningMode - Short/Long reach
*       algoMask  - algorithm but mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesTrainingOptimization 
(
	GT_U8						devNum, 
	GT_U32						portGroup, 
	GT_U32						*serdesList, 
	GT_U32                      numOfActLanes,
	MV_HWS_PORT_MAN_TUNE_MODE	tuningMode,
	GT_U32						algoMask
)
{
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 i;
	
	if (algoMask == 0)
	{
		DEBUG_SERDES_OPT(1, ("algoMask is 0. nothing to do...\n"));

		return GT_OK;
	}

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

	for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < numOfActLanes); i++)
	{
		CHECK_STATUS(mvHwsComHRev2SerdesPrintAll(devNum, portGroup, (serdesList[i] & 0xFFFF), 1));
	}

	DEBUG_SERDES_OPT(1, ("<Train Result"));

	if (algoMask & DFE_OPT_ALGO)
	{
		DEBUG_SERDES_OPT(1, ("\n ---------------------------------   DFE Optimization  ---------------------------------------\n"));
		for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < numOfActLanes); i++)
		{

			CHECK_STATUS(mvHwsComHRev2SerdesDfeOpt(devNum, portGroup, (serdesList[i] & 0xFFFF)));

		}
	}

	if ((algoMask & FFE_OPT_ALGO) || (algoMask & Align90_OPT_ALGO))
	{
		for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < numOfActLanes); i++)
		{
			CHECK_STATUS(mvHwsComHRev2SerdesFixDfeResolution(devNum, portGroup, (serdesList[i] & 0xFFFF)));
		}
	}

	if (algoMask & FFE_OPT_ALGO)
	{
		DEBUG_SERDES_OPT(1, ("\n ---------------------------------   FFE Optimization  ---------------------------------------\n"));

		CHECK_STATUS(mvHwsComHRev2SerdesFfeOpt(devNum, portGroup, serdesList, numOfActLanes, tuningMode));
	}

	if (algoMask & Align90_OPT_ALGO)
	{
		DEBUG_SERDES_OPT(1, ("\n ---------------------------------   Align90 Optimization  -----------------------------------\n"));
		CHECK_STATUS(mvHwsComHRev2SerdesAlign90Opt(devNum, portGroup, serdesList, numOfActLanes));
	}

	return GT_OK;
}

GT_U32 mvHwsComHRev2SerdesFixDfeResolution
(
	GT_U8   devNum, 
	GT_U32  portGroup, 
	GT_U32  serdesNum
)
{
	GT_U32 baseAddr;
	GT_U32 data;
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	
	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
	baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;
	
	/* check analog reset */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + COM_H_Adapted_DFE_Coefficient0_Reg, &data, 0));
	data = (data >> 10) & 0x3F;

	if(data > 57)
	{
		/* Decision Feedback Equalization (DFE) Resolution */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_DFE_Resolution_Control_Reg, (2 << 8), (3 << 8)));
		DEBUG_SERDES_OPT(1,("\nDFE_F0=%1d - Updating DFE resolution to 2", data));
	}

	else if(data > 44)
	{
		/* Decision Feedback Equalization (DFE) Resolution */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_DFE_Resolution_Control_Reg, (1 << 8), (3 << 8)));
		DEBUG_SERDES_OPT(1,("\nDFE_F0=%1d - Updating DFE resolution to 1", data));
	}

	return GT_OK;
}


GT_STATUS mvHwsComHRev2SerdesOptSetFfeThreshold
(
	GT_32     ffeFirstTh,
	GT_32     ffeFinalTh
)
{
	ffeFirstTH = ffeFirstTh;
	ffeFinalTH = ffeFinalTh;
	
	return GT_OK;
}

GT_STATUS mvHwsComHRev2SerdesOptSetF0dStopThreshold
(
	GT_U32     f0dStopTh
)
{
	f0dStopTH = f0dStopTh;
	
	return GT_OK;
}

GT_STATUS mvHwsComHRev2SerdesTrainingOptDelayInit
(
	GT_U32     dynamicDelayInterval,
	GT_U32     dynamicDelayDuration,
	GT_U32     staticDelayDuration
)
{
	dfeDelayInterval = dynamicDelayInterval;
	dfeDelayDuration = dynamicDelayDuration;
	align90DelayDuration = staticDelayDuration;

	return GT_OK;
}

GT_STATUS mvHwsComHRev2SerdesSetAlign90 
(
	GT_U8  devNum, 
	GT_U32 portGroup, 
	GT_U32 serdesBaseAddr, 
	GT_U32 newValue
)
{
	GT_STATUS rc;
	
	/* Set Phase Offset Force (can be kept as set)*/
	rc  = genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, (1<<7), (1<<7));
	/* Shift the align90 to nPhase */
	rc |= genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, newValue, 0x7F); /* 0x188 was COM_H_REV2_OS_PH_OFFSET_Reg */
	/* Activate the write */
	rc |= genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, (1<<8), (1<<8));
	rc |= genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, 0, (1<<8));

	return rc;
}

GT_STATUS mvHwsComHRev2SerdesShiftAlign90
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  serdesNum,
	GT_BOOL isLeftShift,
	GT_U32  basicAlign,
	GT_U32  curAlign
)
{
	GT_U32 intBaseAddr;
	GT_U32 unitAddr;
	GT_U32 unitIndex;

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
	intBaseAddr = unitAddr + (unitIndex * serdesNum) + SERDES_PHY_REGS;

	/* Shift align90 out of the eye and back */
	if(isLeftShift)
	{
		/* Shift the align90 to low (out of the eye) */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, basicAlign - 64, 0x7F));
	}
	else
	{
		/* Shift the align90 to high (max value out of the eye) */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, 127, 0x7F));
	}

	/*  Activate the write  */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, (1<<8), (1<<8)));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, 0, (1<<8)));
				
	/* set back the align90 to the current align90 value */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, curAlign, 0x7F));

	/*  Activate the write  */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, (1<<8), (1<<8)));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + COM_H_REV2_OS_PH_OFFSET_Reg, 0, (1<<8)));

	return GT_OK;
}

GT_STATUS mvHwsComHRev2SerdesMoveAlign90Out
(
	GT_U8	devNum,
	GT_U32  portGroup,
	GT_U32	serdesNum,
	GT_32  iterationNum
)
{
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 serdesBaseAddr;
	GT_U32 basicAlign = 0;
	GT_U32 align = 0;

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
	serdesBaseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;

	if(iterationNum >= 0)
	{
		/* Read RxClkbasicAlign90 value */
		CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr + 0x168, &basicAlign, 0));
		basicAlign = ((basicAlign)>> 8) & 0x7f;
		/* Read reg offset 0x18 for OS_PHASE_offset(final value used as Align90) */
		CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr + 0x188, &align, 0));							
		align = align & 0x7f;

		DEBUG_SERDES_OPT(2, (" basicAlign=%2d_align=%2d", basicAlign, align));
	}

	if(iterationNum%2 == 0)
	{
		/* Shift the align90 to low (out of the eye) [setalign90] */
		CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90(devNum, portGroup, serdesBaseAddr, basicAlign-64));	
		DEBUG_SERDES_OPT(2, ("_ShiftToLeft--%3d", basicAlign-64));
	}
	else
	{
		/* Shift the align90 to high (max value out of the eye) */
		CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90(devNum, portGroup, serdesBaseAddr, 127));
		DEBUG_SERDES_OPT(2, ("_ShiftToRight-127"));
	}
	/* Shift back the align90 */
	CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90(devNum, portGroup, serdesBaseAddr, align));								

	DEBUG_SERDES_OPT(2, ("_ShiftBackTo-%2d", align));

	return GT_OK;
}

GT_STATUS mvHwsComHRev2SerdesEomEyeWidthGet
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  serdesNum,
	MV_EOM_EYE_DATA         *eomEyeData,
	GT_BOOL                 *widthValid
)
{
	GT_U32 leftEdge = 0;
    GT_U32 rightEdge = 0;
	GT_U32 targetBER, voltage;
	GT_U32 berCount = 10;
	GT_U32 validEye = 1;
	GT_U32 loopNum = 0;

	voltage = 0;
	targetBER = 0;

	*widthValid = GT_TRUE;

	CHECK_STATUS(mvHwsEomInit(devNum, portGroup, serdesNum));


	/* find left and right edges */

	CHECK_STATUS(mvHwsEomFindEyeCenter(devNum, portGroup, serdesNum, voltage, eyeCenterPopulation, targetBER, &leftEdge, &rightEdge));


	if (leftEdge == 0)
	{
		DEBUG_SERDES_OPT(1, ("\n<Step1_LeftEdge=0-NoEye"));
		*widthValid = GT_FALSE;
	}
	else
	{
		loopNum = 0;

		do
		{
			CHECK_STATUS(mvHwsEomEyeMeasure(devNum, portGroup, serdesNum, voltage, eyeMeasurmentPopulation, berCount, leftEdge, rightEdge, eomEyeData));
			
			DEBUG_SERDES_OPT(2, (" LEdge=%4d ", leftEdge));
			DEBUG_SERDES_OPT(2, ("REdge=%4d ", rightEdge));
			DEBUG_SERDES_OPT(2, ("EyeW=%2d ", eomEyeData->eyeWidth));
			DEBUG_SERDES_OPT(2, ("LEyeW=%2d ", eomEyeData->leftEyeWidth));
			DEBUG_SERDES_OPT(2, ("REyeW=%2d ",  eomEyeData->rightEyeWidth));

			validEye = 1;
			if (eomEyeData->eyeWidth == 0)
			{
				/* osPrintf("\nNo eye was found in EOMEyeMeasure?!?\n"); */
				validEye = 0;
				loopNum++;
			}

		} while ((validEye == 0) && (loopNum < 5));

		if(validEye == 0)
		{
			DEBUG_SERDES_OPT(1, ("\n<Step2_Eye=0_EomEyeMeasFailed!!!"));
			*widthValid = GT_FALSE;
		}
	}

	CHECK_STATUS(mvHwsEomClose(devNum, portGroup, serdesNum));

	return GT_OK;
}

/* Perform DFE train */
GT_STATUS mvHwsComHRev2SerdesDfeTrain												
(
	GT_U8 devNum, 
	GT_U32 portGroup, 
	GT_U32 *serdesList, 
	GT_U32 numOfActLanes,
	GT_BOOL defResTrainEn
)
{
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 intBaseAddr[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 extBaseAddr[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 currSerdesList[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 retryNum;
	GT_U32 i;
	GT_U32 currNumOfActLanes = 0;
	MV_HWS_AUTO_TUNE_STATUS rxStatus; 
	MV_HWS_AUTO_TUNE_STATUS txStatus;
	GT_BOOL rxStatusFail = GT_FALSE;
	
	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

	for (i = 0; i < numOfActLanes; i++)
	{
		currSerdesList[i] = serdesList[i];
		extBaseAddr[i] = unitAddr + unitIndex*serdesList[i];
		intBaseAddr[i] = extBaseAddr[i] + SERDES_PHY_REGS;

		if (serdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
		{
			continue;
		}

		currNumOfActLanes++;

		/*Move to Page 1 */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x1FC, 0x1, 1));

		/* Set to call DFE only */			
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x130, 1<<7, 1<<7));

		/* Disable dfe_res_train_en */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x16C, defResTrainEn<<9, 1<<9));

		/* Disable reg_cdrphase_opt_en */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x40, 0, 1<<3));

		/*Move to Page 0 */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x1FC, 0, 1));

		/* Start ext. Rx train */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, extBaseAddr[i] + 0xc, 1<<7, 1<<7));
	}

	/* check for completion */
	retryNum = 40;
	rxStatus = TUNE_NOT_COMPLITED;
	
	while((currNumOfActLanes != 0) && (retryNum > 0))
	{
		for (i = 0; i < numOfActLanes; i++)
		{
			if (currSerdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
			{
				continue;
			}

			CHECK_STATUS(mvHwsComHRev2SerdesAutoTuneStatusShort(devNum, portGroup, serdesList[i], &rxStatus, &txStatus));
			if(rxStatus == TUNE_FAIL)
			{
				rxStatusFail = GT_TRUE;
			}
			if (rxStatus != TUNE_NOT_COMPLITED)
			{
				currSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
				currNumOfActLanes--;
			}
		}

		retryNum--;	

		if (currNumOfActLanes != 0)
		{
			/* wait to all lanes to complete */
			hwsOsExactDelayPtr(devNum, portGroup, 1);
		}
	}


	for (i = 0; i < numOfActLanes; i++)
	{
		if (serdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
		{
			continue;
		}

		/* Stop ext. Rx train */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, extBaseAddr[i] + 0xc, 0<<7, 1<<7));
	
		/*Move to Page 1 */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x1FC, 0x1, 1));
	
		/* Enable dfe_res_train_en (restore value) */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x16C, 1<<9, 1<<9));
	
		/* Enable reg_cdrphase_opt_en (restore value) */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x40, 1<<3, 1<<3));
	
		/*Move to Page 0 */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x1FC, 0, 1));

	}

	if(currNumOfActLanes != 0)
	{
		return GT_TIMEOUT;
	}
	else if(rxStatusFail == GT_TRUE)
	{
		return GT_FAIL;
	}
	
	DEBUG_SERDES_OPT(2, (" <-DFEtrainIn-%1dmS", (40-retryNum)));
	
	return GT_OK;
}

/* Perform DFE train, check for enough wait time and read F0d value */
GT_STATUS mvHwsComHRev2SerdesDfeTrainCheckF0dRead												
(
	GT_U8   devNum, 
	GT_U32  portGroup, 
	GT_U32  *serdesList, 
	GT_U32  numOfActLanes,
	GT_BOOL defResTrainEn,
	GT_U32  *f0d
)
{
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 intBaseAddr[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 extBaseAddr[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 ecp[MV_HWS_MAX_LANES_PER_PORT] = {0};
	GT_U32 dfeFailCount[MV_HWS_MAX_LANES_PER_PORT] = {0};
	GT_U32 ecpFailCount[MV_HWS_MAX_LANES_PER_PORT] = {0};
	GT_U32 currSerdesList[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 currNumOfActLanes = 0;
	GT_U32 i;
	GT_BOOL f0dError[MV_HWS_MAX_LANES_PER_PORT], ecpError[MV_HWS_MAX_LANES_PER_PORT];
	GT_STATUS res;
	
	if (defResTrainEn)
	{
		/* Define setting so the DFE train will includes DFE resolution - Enable dfe_res_train_en */
		DEBUG_SERDES_OPT(1, ("\nDefine setting so the DFE train includes DFE resolution"));
	}

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

	for (i = 0; i < numOfActLanes; i++)
	{
		currSerdesList[i] = serdesList[i];
		if (serdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
		{
			continue;
		}
		
		currNumOfActLanes++;

		extBaseAddr[i] = unitAddr + unitIndex*serdesList[i];
		intBaseAddr[i] = extBaseAddr[i] + SERDES_PHY_REGS;
	}


	do {
		/* run DFE training */
		res = mvHwsComHRev2SerdesDfeTrain(devNum, portGroup, currSerdesList,numOfActLanes, defResTrainEn);
		if (res == GT_FAIL)
		{
			/* training failed */
			DEBUG_SERDES_OPT(1, ("<DFEtrain=GT_FAIL"));
		}
		else if (res == GT_TIMEOUT)
		{
			/* training didn't completed */
			DEBUG_SERDES_OPT(1, ("<DFEtrain=GT_TIMEOUT"));
		}
		else if(res == GT_NOT_INITIALIZED)
		{
			/* Serdes is not initialized */
			DEBUG_SERDES_OPT(0, ("Serdes is not initialized\n"));
			return res;
		}
		

		for (i = 0; i < numOfActLanes; i++)
		{
			if (currSerdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
			{
				continue;
			}
			ecpError[i] = f0dError[i] = GT_FALSE;

			/*Move to Page 1 */
			CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x1FC, 0x1, 1));
		
			/* Read F0d */
			CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr[i] + 0x128, &f0d[i], 0));
			f0d[i] = (f0d[i] >> 10) & 0x3F;

			CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr[i] + 0x12c, &ecp[i], 0));							
			ecp[i] = (ecp[i] >> 9) & 0x1;

			/*Move to Page 0 */
			CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x1FC, 0x0, 1));
		
			/* Approving ECP is valid */
			if(ecp[i] == 0)
			{
				DEBUG_SERDES_OPT(1, ("Lane %d <-ECP=0-RetryDfeTrain",currSerdesList[i]));
				ecpFailCount[i]++;
				ecpError[i] = GT_TRUE;

				if (ecpFailCount[i] >= 2)
				{
					/* Failure: exit with some F0D value */
					DEBUG_SERDES_OPT(0, ("\nLane %d <-ECP 2 times !!!\n",currSerdesList[i]));
				
					/*f0d [i]= 0; */
					currSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
					currNumOfActLanes--;

					continue;
				}
			}
			else
			{
				/* clear failure counter */
				ecpFailCount[i] = 0;
			}

			/* Approving DFE train completion (F0d=63 is per low train period) */
			if(f0d[i] == 63)
			{
				DEBUG_SERDES_OPT(1, ("Lane %d  <-F0d=63-RetryDfeTrain",currSerdesList[i]));
				dfeFailCount[i]++;
				f0dError[i] = GT_TRUE;
			}

			if(dfeFailCount[i] >= 15)
			{
				DEBUG_SERDES_OPT(1, ("Lane %d  <-Failed_DFEtrain after %2d runs\n\n",currSerdesList[i], (dfeFailCount[i]+1)));
			}

			if ((dfeFailCount[i] > 15) || ((f0dError[i] == GT_FALSE) && (ecpError[i] == GT_FALSE)))
			{
				currSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
				currNumOfActLanes--;
			}
		}
	} while(currNumOfActLanes > 0);
	
	for (i = 0; i < numOfActLanes; i++)
	{
		if (serdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
		{
			continue;
		}

		if((f0dError[i] == GT_TRUE)/* || (ecpError[i] == GT_TRUE)*/)
		{
			/* Failure: exit with some F0D value */
			DEBUG_SERDES_OPT(0, ("Error: Lane %d  F0D bad value (F0d %d ECP %d)\n",serdesList[i], f0d[i], ecp[i]));
			DEBUG_SERDES_OPT(0, ("exit Lane %d  with F0D=0\n",serdesList[i]));
			f0d[i] = 0;
		}
	}

	return GT_OK;
}

/************************************************************************/
/*          DFE Optimization Algorithm                                  */

GT_STATUS mvHwsComHRev2SerdesDfeOpt
(
	GT_U8  devNum,
	GT_U32 portGroup,
	GT_U32 serdesNum
)
{
	/*
	GT_U32 popCount = 10000;
	GT_U32 berCount = 10;
	*/
	
	MV_EOM_EYE_DATA  eomEyeData;
	GT_U32 eyeWidthMax=0;
	GT_U32 eyeWidthMin=64;
	GT_U32 eyeWidthMinCount=0;
	GT_U32 eyeWidthMin_2=64;
	GT_U32 eyeWidthMinCount_2=0;
	GT_U32 eomFailCount =0;
	GT_U32 eomSuccessCount = 0;
	GT_BOOL eyeValid = GT_FALSE;
	int i;

	eomFailCount =0;
	eomSuccessCount = 0;

    hwsOsMemSetFuncPtr(&eomEyeData, 0, sizeof(MV_EOM_EYE_DATA));

	DEBUG_SERDES_OPT(1, ("DFEOpt"));

	for(i=1; i<=20; i++)
	{
		DEBUG_SERDES_OPT(2, ("\nStep#1 Iteration#%2d:", i));

		/* delay in order to allow the context switch */
		if(((i % dfeDelayInterval) == 0) && (dfeDelayDuration != 0))
		{
			hwsOsExactDelayPtr(devNum, portGroup, dfeDelayDuration);
		}

		CHECK_STATUS(mvHwsComHRev2SerdesMoveAlign90Out(devNum, portGroup, serdesNum, i));
		CHECK_STATUS(mvHwsComHRev2SerdesEomEyeWidthGet(devNum, portGroup, serdesNum, &eomEyeData, &eyeValid));
		if (eyeValid == GT_FALSE)
		{
			DEBUG_SERDES_OPT(2, ("\nFailureInStep1, EyeWidthInterval(%d)\n", i));
			continue;
		}

		/* update max value */
		eyeWidthMax = (eomEyeData.eyeWidth > eyeWidthMax) ? eomEyeData.eyeWidth : eyeWidthMax;
		
		if((eomEyeData.eyeWidth > eyeWidthMax/3 ) && (eomEyeData.eyeWidth <= (eyeWidthMin+1)))
		{
			if(eomEyeData.eyeWidth < eyeWidthMin)
			{
				if(i>1)
				{
					eyeWidthMin_2 = eyeWidthMin;
					eyeWidthMinCount_2=eyeWidthMinCount;
				}
				eyeWidthMin = eomEyeData.eyeWidth;
				eyeWidthMinCount=1;
			}
			else
			{
				eyeWidthMinCount++;
			}
		}
		else if((eomEyeData.eyeWidth > eyeWidthMax/3 ) && (eomEyeData.eyeWidth <= (eyeWidthMin_2+1)))
		{
			if(eomEyeData.eyeWidth < eyeWidthMin_2)
			{
				eyeWidthMin_2 = eomEyeData.eyeWidth;
				eyeWidthMinCount_2=1;
			}
			else
			{
				eyeWidthMinCount_2++;
			}
		}	
				
		if(eomEyeData.eyeWidth <= eyeWidthMax/3 )
		{
			eomFailCount++; 
			DEBUG_SERDES_OPT(2, ("\nFailureInStep1->EyeWidth=%2d is lower than %2d/3 (EyeWidthMAX/3)\n", eomEyeData.eyeWidth, eyeWidthMax));
		}
		else
		{
			eomSuccessCount++;
		}

		DEBUG_SERDES_OPT(2, ("EyeWMAX=%2d EyeWMIN=%2d/%1dtimes EyeWMIN_2=%2d/%1dtimes ", 
						eyeWidthMax, eyeWidthMin, eyeWidthMinCount, eyeWidthMin_2, eyeWidthMinCount_2));
	}
	
	DEBUG_SERDES_OPT(2, ("\n############## Completed Step#1 - EyeWidthMAX%2d, EyeWidthMIN=%2d / %1d times, EyeWidthMIN_2=%2d / %1d times  ##############", 
					eyeWidthMax,eyeWidthMin, eyeWidthMinCount, eyeWidthMin_2, eyeWidthMinCount_2));

	if(eyeWidthMin<(eyeWidthMin_2-1) && eyeWidthMinCount<2)
	{
		eyeWidthMin=eyeWidthMin_2;
	}
	
	for(i=1; i<=40; i++)
	{
		DEBUG_SERDES_OPT(2, ("\nStep#2 Iteration#%2d", i));

		/* delay in order to allow the context switch */
		if(((i % dfeDelayInterval) == 0) && (dfeDelayDuration != 0))
		{
			hwsOsExactDelayPtr(devNum, portGroup, dfeDelayDuration);
		}
		
		CHECK_STATUS(mvHwsComHRev2SerdesMoveAlign90Out(devNum, portGroup, serdesNum, i));
		CHECK_STATUS(mvHwsComHRev2SerdesEomEyeWidthGet(devNum, portGroup, serdesNum, &eomEyeData, &eyeValid));
		if (eyeValid == GT_FALSE)
		{
			DEBUG_SERDES_OPT(2, ("\nFailureInStep2, EyeWidthInterval(%d)\n", i));
			continue;
		}

		DEBUG_SERDES_OPT(2, ("->CurrentEyeWidth=%2d", eomEyeData.eyeWidth));

		if(eomEyeData.eyeWidth <= eyeWidthMax/3 )
		{
			eomFailCount++; 
			DEBUG_SERDES_OPT(2, ("\nFailureInStep2->EyeWidth=%2d is lower than %2d/3 (EyeWidthMAX/3)\n", eomEyeData.eyeWidth, eyeWidthMax));
						
			continue;
		}
		
		eomSuccessCount++;
		
		if(  (eomEyeData.eyeWidth <= eyeWidthMin+1) ||
		   ( (eomEyeData.eyeWidth <= eyeWidthMin+3) && (i > 10)) ||
		   (((eomEyeData.eyeWidth <= ((eyeWidthMin + eyeWidthMax)/2)) && (i > 30))))
		{
			DEBUG_SERDES_OPT(1, ("=>EyeWMAX=%2d EyeWMIN=%2d FinalEyeW=%2d/%2dsteps [%2dgoodVs. %2dbadRuns] ",eyeWidthMax, eyeWidthMin, eomEyeData.eyeWidth, (20+i), eomSuccessCount, eomFailCount));
			
			return GT_OK;
		}
	}

	if (eyeValid == GT_TRUE)
	{
		DEBUG_SERDES_OPT(1, ("\n=>EyeWMAX=%2d EyeWMIN=%2d FinalEyeW=%2d / %2d steps [%2d goodVs. %2d badRuns] ",
							eyeWidthMax, eyeWidthMin, eomEyeData.eyeWidth, (20+i), eomSuccessCount, eomFailCount));
	}

	DEBUG_SERDES_OPT(1, ("\n\nError: Failed to find smallest eye in 40 runs\n\n"));
	
	return GT_FAIL;
}

/************************************************************************/
/*          FFE Optimization Algorithm                                  */

GT_STATUS mvHwsComHRev2SerdesFfeOpt
(	
	GT_U8						devNum,
	GT_U32						portGroup,
	GT_U32						*serdesList, 
	GT_U32						numOfActLanes,
	MV_HWS_PORT_MAN_TUNE_MODE	portTuningMode
)
{
	GT_U32 intBaseAddr[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 unitAddr;
	GT_U32 unitIndex;

	GT_U32 data;
	GT_U32 f1Sign;
	GT_U32 f2Sign;
	GT_32  f1;
	GT_32  f2;
	GT_U32 ffer;
	GT_U32 ffec;
	GT_U32 minFfeC;
	GT_U32 maxFfeR;
	GT_U32 f0d[MV_HWS_MAX_LANES_PER_PORT];
	GT_32 overBoostThresh;
	GT_U32 i, loopIndex;
	GT_U32 currActiveLanes;
	GT_U32 tmpSerdesList[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 align90Val;

	DEBUG_SERDES_OPT(1, ("_FFEOpt (%s)", (portTuningMode == StaticLongReach) ? "KR" : "SR_LR"));

	if (numOfActLanes > MV_HWS_MAX_LANES_PER_PORT)
	{
		DEBUG_SERDES_OPT(0, ("Wrong Lanes number %d",numOfActLanes));
		return GT_FAIL;
	}

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

	for (i = 0; i < numOfActLanes; i++)
    {
		intBaseAddr[i] = unitAddr + unitIndex*serdesList[i] + SERDES_PHY_REGS;

		/* Read initial value of Align90 */
		CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr[i] + 0x1A0, &align90Val, 0));
		align90Val = (align90Val >> 9) & 0x7f;

		DEBUG_SERDES_OPT(1, ("Lane %d: Align90=%d\n", serdesList[i], align90Val));

		/*  Set Align90 to new nPhase value */
		CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90 (devNum, portGroup, intBaseAddr[i], align90Val));        
	}

	/* configure initial FFE values */
	ffer = (portTuningMode == StaticLongReach) ? 0x0 : 0x2;
	ffec = 0xF;

	if (forceFfeVal == GT_TRUE)
	{
		ffer = initFfeR;
		ffec = initFfeC;
	}

	for (i = 0; i < numOfActLanes; i++)
	{
		intBaseAddr[i] = unitAddr + unitIndex*serdesList[i] + SERDES_PHY_REGS;

		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x18, (1 << 7), (1 << 7))); /* Enable force FFE value*/
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x18, (ffer << 4), (0x7 << 4))); /* FFE_R */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x18, ffec,  0xf));				  /* FFE_C */
		
		tmpSerdesList[i] = serdesList[i];
	}

	currActiveLanes = numOfActLanes;

	/* configure limit FFE value */
	minFfeC = (portTuningMode == StaticLongReach) ? 0 : 0x7;
	maxFfeR = 3;

	if (forceFfeVal == GT_TRUE)
	{
		maxFfeR = limitFfeR;
		minFfeC = limitFfeC;
	}

	DEBUG_SERDES_OPT(1, (" -> Set FFE_R to %d & FFE_C to %d  ", ffer, ffec));
	

	for(loopIndex=0; (currActiveLanes != 0) && (loopIndex < 25) ; loopIndex++)
	{
		if(loopIndex == 0)
		{
			overBoostThresh = ffeFirstTH;
		}
		else
		{
			overBoostThresh = ffeFinalTH;
		}

		for (i = 0; i < numOfActLanes; i++)
		{
			if (tmpSerdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
			{
				continue;
			}

			/* Read F1 & F2 */
			/*Move to Page 1 */
			CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x1FC, 0x1, 1));

			/* Read reg offset 0x128 for f0d/f1/f4 */
			CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr[i] + 0x128, &data, 0));	
			f1 = (data >> 4) & 0x1F;
			f1Sign = (data >> 9) & 0x1;
			f1 = (f1Sign) ? (-1) * f1 : f1;
		
			/* Read reg offset 0x12c for Eye_Check_Pass(ECP)/f2/f5 */
			CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr[i] + 0x12c, &data, 0));
			f2 = (data >> 4) & 0xF;
			f2Sign = (data >> 8) & 0x1;
			f2 = (f2Sign) ? (-1) * f2 : f2; 

			/*Move to Page 0 */
			CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x1FC, 0x0, 1));

			/* Read reg offset 0x18 for FFE-R & FFE-C */
			CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr[i] + 0x18, &data, 0));
			ffer = (data >> 4) & 0x7;
			ffec = data & 0xf;
		
			if((((f1 + f2) < overBoostThresh) || (f1 < 0)))
			{
				DEBUG_SERDES_OPT(1, ("\nLane %d f1=%2d f2=%3d ffeR/ffeC updated from-%2d/%2d ", 
					serdesList[i], f1, f2, ffer, ffec));
				
				if(ffec > minFfeC)
				{
					ffec = (ffec > (ffecSteps + minFfeC)) ? ffec - ffecSteps : minFfeC;
				}
				else if(ffer < maxFfeR)
				{
					ffer++;
				}
				else
				{
					tmpSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
					currActiveLanes--;
					DEBUG_SERDES_OPT(1, ("to %2d/%2d - FFE-R Reached the limit no update!!!",ffer , ffec));
				}
				
				if(tmpSerdesList[i] != MV_HWS_SERDES_NOT_ACTIVE)
				{
					CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x18, (ffer << 4), (0x7 << 4)));				
					CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr[i] + 0x18, ffec, 0xf));				
					
					DEBUG_SERDES_OPT(1, ("to %2d/%2d", ffer , ffec));
				}

			}
			else
			{
				tmpSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
				currActiveLanes--;
				DEBUG_SERDES_OPT(2, ("\n"));
				DEBUG_SERDES_OPT(1, ("Lane %d f1=%2d, f2=%2d, ffeR/ffeC=%2d/%2d >>>> FFE is optimized   ",
					serdesList[i], f1, f2, ffer, ffec));
			}
		
		}

		if (currActiveLanes != 0)
		{
			CHECK_STATUS(mvHwsComHRev2SerdesDfeTrainCheckF0dRead(devNum, portGroup, tmpSerdesList, 
																 numOfActLanes, GT_FALSE, f0d));
		}
	}

	return GT_OK;
}

/************************************************************************/
/*          Align90 Optimization Algorithm                              */

GT_STATUS mvHwsComHRev2SerdesAlign90Opt
(
	GT_U8  devNum,
 	GT_U32 portGroup,
	GT_U32 *serdesList,
	GT_U32 numOfActLanes
)
{
	GT_U32 intBaseAddr[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 initPhase[MV_HWS_MAX_LANES_PER_PORT] = {0};		/* MSI1	Rx training align90 value						*/
	GT_U32 nPhase[MV_HWS_MAX_LANES_PER_PORT] = {0};		/* MSI1	current align90 value							*/
	GT_U32 f0dVec[MV_HWS_MAX_LANES_PER_PORT][MV_HWS_ALIGN90_VEC_SIZE];	/* MSI1 F0d [nPhase] result vector	*/
	/*GT_U32 f0dMax[MV_HWS_MAX_LANES_PER_PORT] = {0};*/		/* MSI1	max f0d result									*/
	GT_U32 meanF0dPhase = 0;							/* MSI1	center of the valid f0dPassVec window		    */
	GT_U32 optAlign90   = 0;							/* MSI1	m1 optimal align 90 value						*/
	GT_U32 nPhaseMin[MV_HWS_MAX_LANES_PER_PORT]	= {0};	/* minimum nPhase value during the algorithm run		*/
	GT_U32 nPhaseMax[MV_HWS_MAX_LANES_PER_PORT]	= {0};	/* maximum nPhase value during the algorithm run		*/
	GT_U32 f0dStopThresh[MV_HWS_MAX_LANES_PER_PORT] = {0};	/* The lowest F0D value to algorithm allowed to reach 	*/
	GT_U32 f0d4StepThresh[MV_HWS_MAX_LANES_PER_PORT] = {0};/* Threshold for enabling 4 steps run 					*/
	GT_U32 phaseRange = 60;								/* phase range of the algorithm search window			*/
	GT_U32 f0d[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 calPhase[MV_HWS_MAX_LANES_PER_PORT];
	GT_U32 triggerA[MV_HWS_MAX_LANES_PER_PORT];			/* Free using trigger for capture Left data				*/
	GT_U32 triggerB[MV_HWS_MAX_LANES_PER_PORT];			/* Free using trigger for capture Right data			*/
	GT_U32 i, currActiveLanes;
	GT_U32 tmpSerdesList[MV_HWS_MAX_LANES_PER_PORT];


	DEBUG_SERDES_OPT(1, ("_Align90opt  "));
	DEBUG_SERDES_OPT(1, ("... "));

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

	currActiveLanes = numOfActLanes;

	if (numOfActLanes > MV_HWS_MAX_LANES_PER_PORT)
	{
		DEBUG_SERDES_OPT(0, ("Wrong Lanes number %d",numOfActLanes));
		return GT_FAIL;
	}

	for (i = 0; i < numOfActLanes; i++)
	{
		intBaseAddr[i] = unitAddr + unitIndex*serdesList[i] + SERDES_PHY_REGS;
		triggerA[i] = triggerAStart;
		triggerB[i] = triggerBStart;

		/* Read initial value of Align90 */
		CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr[i] + 0x1A0, &initPhase[i], 0));
		initPhase[i] = (initPhase[i] >> 9) & 0x7f;

		/* Getting the calPhase value */
		CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr[i] + 0x1e4, &calPhase[i], 0));
		calPhase[i] = calPhase[i] & 0x7F;
		DEBUG_SERDES_OPT(2, ("\nCal_Phase=%2d _ ", calPhase[i]));

		/*f0dMax[i] = 0;*/

		/* Capture Left data with smooth phase change */
		DEBUG_SERDES_OPT(2, ("\nCapture Left data with smooth phase change:"));

		nPhase[i] = initPhase[i];
		tmpSerdesList[i] = serdesList[i];
		if (nPhase[i] < (calPhase[i] - phaseRange))
		{
			tmpSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
			currActiveLanes--;
		}
	}
	
	while (currActiveLanes != 0)
	{

		for (i = 0; i < numOfActLanes; i++)
		{
			if (tmpSerdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
			{
				continue;
			}

			/*  Set Align90 to new nPhase value */
			CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90 (devNum, portGroup, intBaseAddr[i], nPhase[i]));	
		}

		/* Perform DFE train & read F0d value */
		CHECK_STATUS(mvHwsComHRev2SerdesDfeTrainCheckF0dRead(devNum, portGroup, tmpSerdesList, numOfActLanes, 
															 GT_FALSE, f0d));

		
		for (i = 0; i < numOfActLanes; i++)
		{
			if (tmpSerdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
			{
				continue;
			}
		
			/* in first iteration we use the F0d value to determine the thresholds */
			if (nPhase[i] == initPhase[i])
			{
				f0dStopThresh[i] = ((f0d[i]/2) > f0dStopTH) ? (f0d[i]/2) : f0dStopTH;
				f0d4StepThresh[i] = (((f0d[i] + f0dStopThresh[i]) / 2) > 13) ? ((f0d[i] + f0dStopThresh[i]) / 2) : 13;
				DEBUG_SERDES_OPT(2, ("Lane %d F0d=%2d _ f0dStopThresh=%2d f0d4StepThresh =%2d", tmpSerdesList[i], 
					f0d[i], f0dStopThresh[i], f0d4StepThresh[i]));
			}

			/* after first execution of DFE training, F0D value should not be 0 */
			if((nPhase[i] == initPhase[i]) && (f0d[i] == 0))
			{
				DEBUG_SERDES_OPT(0, ("Lane %d F0D is 0 for first Align90 Init val (%d)",i ,nPhase[i]));
				return GT_FAIL;
			}

			f0dVec[i][nPhase[i]] = f0d[i];
		
			CHECK_STATUS(mvHwsComHRev2SerdesPrintAll(devNum, portGroup,serdesList[i], 2));
			DEBUG_SERDES_OPT(2, ("\tLane %d m1_f0d_vec[%2d] = %2d",serdesList[i], nPhase[i], f0dVec[i][nPhase[i]]));

			if(f0d[i] < f0dStopThresh[i])	
			{

				triggerA[i]--;
				nPhase[i]+=2;

				if(triggerA[i] == 0)
				{
					DEBUG_SERDES_OPT(2, ("f0d < f0dStopThresh -> stop search"));
					tmpSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
					currActiveLanes--;
					continue;
				}
			}

			nPhase[i] -= 2;

			if (f0d[i] > f0d4StepThresh[i])
			{
				/* 4 step mode */
				/*  Set Align90 to new nPhase value */
				CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90 (devNum, portGroup, intBaseAddr[i], nPhase[i]));
				nPhase[i] -= 2;
			}

			if (nPhase[i] < (calPhase[i] - phaseRange))
			{
				tmpSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
				currActiveLanes--;
			} 
		}

	}

	for (i = 0; i < numOfActLanes; i++)
	{
		/* Define MinPhase limit */
		nPhaseMin[i] = nPhase[i];

		/* move phase back */
		DEBUG_SERDES_OPT(2, ("\nSmooth phase back"));
		
		for (nPhase[i]=nPhase[i]+2; nPhase[i]<=initPhase[i]; nPhase[i]+=2)
		{
			/*  Set Align90 to new nPhase value */
			CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90 (devNum, portGroup, intBaseAddr[i], nPhase[i]));	
			DEBUG_SERDES_OPT(2, ("Lane %d _%2d", serdesList[i], nPhase[i]));
		}
	}

	/* delay in order to allow the context switch */
	if(align90DelayDuration != 0)
	{
		hwsOsExactDelayPtr(devNum, portGroup, align90DelayDuration);
	}

	/* Capture Right data with smooth phase change */
	DEBUG_SERDES_OPT(2, ("\nCapture Right data with smooth phase change:"));
	
	currActiveLanes = numOfActLanes;
	
	for (i = 0; i < numOfActLanes; i++)
	{
		tmpSerdesList[i] = serdesList[i];
		nPhase[i]=initPhase[i]+2;
		if ((nPhase[i] > (calPhase[i] + phaseRange)) || (nPhase[i] > 127))
		{
			tmpSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
			currActiveLanes--;
		} 
	}

	while (currActiveLanes > 0)
	{
		for (i = 0; i < numOfActLanes; i++)
		{
			if (tmpSerdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
			{
				continue;
			}

			/*  Set Align90 to new nPhase value */
			CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90 (devNum, portGroup, intBaseAddr[i], nPhase[i]));			
		}

		/* Perform DFE train & read F0d value */
		CHECK_STATUS(mvHwsComHRev2SerdesDfeTrainCheckF0dRead(devNum, portGroup, tmpSerdesList, numOfActLanes,
															 GT_FALSE, f0d));

		for (i = 0; i < numOfActLanes; i++)
		{
			if (tmpSerdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
			{
				continue;
			}


			f0dVec[i][nPhase[i]] = f0d[i];
		
			CHECK_STATUS(mvHwsComHRev2SerdesPrintAll(devNum, portGroup, serdesList[i], 2));
			DEBUG_SERDES_OPT(2, ("\tLane %d m1_f0d_vec[%2d] = %2d", serdesList[i], nPhase[i], f0dVec[i][nPhase[i]]));

			if(f0d[i] < f0dStopThresh[i])								
			{
				triggerB[i]--;
				nPhase[i]-=2;
			}

			if(triggerB[i] ==0)
			{
				DEBUG_SERDES_OPT(2, (" Lane %d f0d < f0dStopThresh -> stop search",serdesList[i]));
				tmpSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
				currActiveLanes--;
				continue;
			}

			nPhase[i]+=2;
			if (f0d[i] > f0d4StepThresh[i])
			{
				/* 4 step mode */
				/*  Set Align90 to new nPhase value */
				CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90 (devNum, portGroup, intBaseAddr[i], nPhase[i]));
				nPhase[i] += 2;
			}

			if ((nPhase[i] > (calPhase[i] + phaseRange)) || (nPhase[i] > 127))
			{
				tmpSerdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
				currActiveLanes--;
			} 

		}
	}

	for (i = 0; i < numOfActLanes; i++)
	{
		/* Define MaxPhase limit */
		nPhaseMax[i] =	nPhase[i];

		meanF0dPhase = (nPhaseMax[i] + nPhaseMin[i])/2;
		optAlign90 = meanF0dPhase + shiftAlign90;
	
		DEBUG_SERDES_OPT(2, ("\nnPhaseMin=%2d, nPhaseMax=%2d, shiftAlign90=%2d,optAlign90=%2d\n", 
						nPhaseMin[i], nPhaseMax[i], shiftAlign90, optAlign90));

		/* move phase back optAlign90 ±1tap*/
		DEBUG_SERDES_OPT(2, ("\nSmooth phase back..."));
	
		for (nPhase[i]=nPhase[i]-2; nPhase[i]>=optAlign90+1; nPhase[i]-=2)
		{
			/*  Set Align90 to new nPhase value */
			CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90 (devNum, portGroup, intBaseAddr[i], nPhase[i]));				
			DEBUG_SERDES_OPT(2, ("_%2d", nPhase[i]));
		}

		if ((optAlign90 < 30) || (optAlign90 > 120))
		{	
			DEBUG_SERDES_OPT(0, ("Error: Align90 is not valid (%d)\n", optAlign90));
			return GT_FAIL;
		}

		/*  Set Align90 to algorithm optimal value */
		CHECK_STATUS(mvHwsComHRev2SerdesSetAlign90 (devNum, portGroup, intBaseAddr[i], optAlign90));

		DEBUG_SERDES_OPT(2, ("\ncal_phase=%2d, nm1_mean_f0d_phase=%2d _ optAlign90=%2d   -->  End of Align90 Opt", 
					 calPhase[i], meanF0dPhase, optAlign90));
	}

	/* Perform DFE train */
	CHECK_STATUS(mvHwsComHRev2SerdesDfeTrainCheckF0dRead(devNum, portGroup, serdesList, numOfActLanes, GT_TRUE, f0d));

	for (i = 0; i < numOfActLanes; i++)
	{
		CHECK_STATUS(mvHwsComHRev2SerdesPrintAll(devNum, portGroup, serdesList[i], 1));
	}

	DEBUG_SERDES_OPT(1, (" <-Align90opt\n"));

	DEBUG_SERDES_OPT(1, (" Done\n"));
	return GT_OK;
}

/************************************************************************/

GT_STATUS mvHwsComHRev2SerdesPrintAll
(
	GT_U8		devNum,
	GT_U32		portGroup,
	GT_U32		serdesNum,
	GT_32		printLevel
)
{
	GT_U32 intBaseAddr;
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 data;
	GT_U32 f1Sign;
	GT_U32 f2Sign;
	GT_U32 f3Sign;
	GT_U32 f4Sign;
	GT_U32 f5Sign;
	GT_32 f1;
	GT_32 f2;
	GT_32 f3;
	GT_32 f4;
	GT_32 f5;
	
	GT_U32 f0a, f0b, f0d;
	GT_U32 ecp, ffer, ffec;
	GT_U32 align;

	if (optPrintLevel < printLevel)
	{
		return GT_OK;
	}
	
	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
	intBaseAddr = unitAddr + unitIndex*serdesNum + SERDES_PHY_REGS;
	
	/*Move to Page 1 */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + 0x1FC, 0x1, 1));				
	/* Read reg offset 0x124 for f0a/f0b/f3 */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr + 0x124, &data, 0));	/* &data */						
	f0a = (data >> 10) & 0x3f;
	f0b = (data >> 4) & 0x3f;
	/*params->f3 = (data >> 0) & 0xf;*/
	f3 = (data >> 0) & 0x7;
	f3Sign = (data >> 3) & 0x1;
	f3 = (f3Sign) ? (-1) * f3 : f3;
	/* Read reg offset 0x128 for f0d/f1/f4 */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr + 0x128, &data, 0));							
	f0d = (data >> 10) & 0x3f;
	f1 = (data >> 4) & 0x1F;
	f1Sign = (data >> 9) & 0x1;
	f1 = (f1Sign) ? (-1) * f1 : f1; 
	f4 = (data >> 0) & 0x7;
	f4Sign = (data >> 3) & 0x1;
	f4 = (f4Sign) ? (-1) * f4 : f4;
	/* Read reg offset 0x12c for Eye_Check_Pass(ECP)/f2/f5 */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr + 0x12c, &data, 0));							
	ecp = (data >> 9) & 0x1;
	f2 = (data >> 4) & 0xF;
	f2Sign = (data >> 8) & 0x1;
	f2 = (f2Sign) ? (-1) * f2 : f2; 
	f5 = (data >> 0) & 0x7;
	f5Sign = (data >> 3) & 0x1;
	f5 = (f5Sign) ? (-1) * f5 : f5;
	/*Move to Page 0 */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, intBaseAddr + 0x1FC, 0x0, 1));								
	
	/* Read reg offset 0x18 for FFE-R & FFE-C */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr + 0x1A4, &data, 0));	
	ffer = (data >> 12) & 0x7;
	ffec = (data >> 8) & 0xf;
	/* Read reg offset 0x18 for OS_PHASE_offset(final value used as Align90) */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, intBaseAddr + 0x1A0, &data, 0));							
	align = (data >> 9) & 0x7f;
	
	DEBUG_SERDES_OPT(printLevel, ("\nC,L|FFEr,c|Aln90|F1-5|F0a,b,d|ECP: %1d %2d | %1d %2d | %3d | %2d %2d %2d %2d %2d | %2d %2d %2d| %1d", 
					 portGroup, serdesNum, ffer, ffec, align, f1, f2, f3, f4, f5, f0a, f0b, f0d, ecp));
	
	return GT_OK;	
}

/*******************************************************************************
* mvHwsComHRev2SerdesOptAlgoParams
*
* DESCRIPTION:
*       Change default thresholds of:
*		ffe optimizer for first iteration, final iteration and f0d stop threshold
*
* INPUTS:
*       ffeFirstTh - threshold value for changing ffe optimizer for first iteration
*       ffeFinalTh - threshold value for changing ffe optimizer for final iteration
*       f0dStopTh  - value for changing default of f0d stop threshold
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesOptAlgoParams
(
    GT_32     ffeFirstTh,
    GT_32     ffeFinalTh,
    GT_U32    f0dStopTh
)
{
    ffeFirstTH = ffeFirstTh;
    ffeFinalTH = ffeFinalTh;
    f0dStopTH = f0dStopTh;

    return GT_OK;
}

/*******************************************************************************
* mvHwsComHRev2SerdesSetTuningParam
*
* DESCRIPTION:
*       Setting tuning parameters according to the receive value:
*       Enables/Disabled eye check, Setting PRESET command (INIT=1 or PRESET=2),
*       Gen1 Amplitude values, initialize Amplitude values preset Amplitude values.
*       Parameter will not set if received value not in the range:
*       presetCmdVal    - enum INIT/PRESET
*       eyeCheckEnVal   - GT_TRUE/GT_FALSE
*       gen1TrainAmp    - 0-0x1F
*       gen1TrainEmph0  - 0-0xF
*       gen1TrainEmph1  - 0-0xF
*       gen1AmpAdj      - GT_TRUE/GT_FALSE
*       initialAmp      - 0-0x3F
*       initialEmph0    - 0-0xF
*       initialEmph1    - 0-0xF
*       presetAmp       - 0-0x3F
*       presetEmph0     - 0-0xF
*       presetEmph1     - 0-0xF
*
* INPUTS:
*       params - pointer to structure to store tuning parameters for changing default values
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHRev2SerdesSetTuningParam
(
    MV_HWS_SERDES_TUNING_PARAMS  *params
)
{
    GT_U32 data;
    GT_STATUS rc = GT_OK;

    if ((params == NULL) ||
        (params->gen1TrainAmp > 0x1F) || (params->gen1TrainEmph0 > 0xF) || (params->gen1TrainEmph1 >0xF) ||
        (params->initialAmp > 0x3F) || (params->initialEmph0 > 0xF) || (params->initialEmph1 > 0xF) ||
        (params->presetAmp > 0x3F) || (params->presetEmph0 > 0xF) || (params->presetEmph1 > 0xF))
    {
        return GT_BAD_PARAM;
    }

    if (params->presetCmdVal != PRESET_NA)
    {
        /* in COM_H_REV2_REG1_Reg bits 0:1 config presetCmdVal */
        rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_REG1_Reg, params->presetCmdVal, (0x3));
        if(rc != GT_OK)
        {
            osPrintf("\nFail to configure presetCmdVal error code 0x%x\n", rc);
            return rc;
        }
    }

    /* in case Preset command == PRESET -> disable eye check */
    if (params->presetCmdVal == PRESET)
    {
        params->eyeCheckEnVal = GT_FALSE;
    }

    /* in COM_H_REV2_FRAME_Cntr_MSB1_Val bit 15 Config eyeCheckEnVal in case eye check disable [bit 15] = 1 */
    data = (params->eyeCheckEnVal == GT_FALSE) ? 1 : 0 ;
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_FRAME_Cntr_MSB1_Reg, (data << 15), (1 << 15));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure eyeCheckEnVal error code 0x%x\n", rc);
        return rc;
    }

    /* in COM_H_REV2_TX_AMP_Reg bits 1:5 config gen1TrainAmp */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AMP_Reg, (params->gen1TrainAmp << 1), (0x1F << 1));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure gen1TrainAmp error code 0x%x\n", rc);
        return rc;
    }

    /* in COM_H_REV2_TX_AMP_Reg bits 7:10 config gen1TrainEmph1 */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AMP_Reg, (params->gen1TrainEmph1 << 7), (0xF << 7));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure gen1TrainEmph1 error code 0x%x\n", rc);
        return rc;
    }

    /* in COM_H_REV2_GEN1_SET2_Reg bit 6 config gen1AmpAdj */
    data = (params->gen1AmpAdj == GT_TRUE ) ? 1 : 0;
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AMP_Reg, (data << 6), (1 << 6));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure gen1AmpAdj error code 0x%x\n", rc);
        return rc;
    }

    /* in COM_H_REV2_GEN1_SET2_Reg bits 0:3 config gen1TrainEmph0 */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_GEN1_SET2_Reg, params->gen1TrainEmph0 , (0xF));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure gen1TrainEmph0 error code 0x%x\n", rc);
        return rc;
    }

    /* in COM_H_REV2_TX_AmpDefault_Reg bits 8:13 config Initial_AMP */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AmpDefault_Reg, (params->initialAmp << 8), (0x3F << 8));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure initialAmp error code 0x%x\n", rc);
        return rc;
    }

    /* in COM_H_REV2_TX_AmpDefault_Reg bits 0:3 sconfig Initial_Emph0 */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AmpDefault_Reg, params->initialEmph0, (0xF));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure initialEmph0 error code 0x%x\n", rc);
        return rc;
    }

    /* in COM_H_REV2_TX_AmpDefault_Reg bits 4:7 config Initial_Emph1 */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AmpDefault_Reg, (params->initialEmph1 << 4), (0xF << 4));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure initialEmph1 error code 0x%x\n", rc);
        return rc;
    }

    /* in CCOM_H_REV2_TX_AmpDefault0 bits 8:13 config presetAmp */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AmpDefault0, (params->presetAmp << 8), (0x3F << 8));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure presetAmp error code 0x%x\n", rc);
        return rc;
    }

    /* in CCOM_H_REV2_TX_AmpDefault0 bits 0:3 config presetEmph0 */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AmpDefault0, params->presetEmph0, (0xF));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure presetEmph0 error code 0x%x\n", rc);
        return rc;
    }

   /* in CCOM_H_REV2_TX_AmpDefault0 bits 4:7 config presetEmph1 */
    rc = hwsSetSeqParams(SERDES_TX_TRAINING_ENABLE_SEQ, SERDES_PHY_REGS + COM_H_REV2_TX_AmpDefault0, (params->presetEmph1 << 4), (0xF << 4));
    if(rc != GT_OK)
    {
        osPrintf("\nFail to configure presetEmph1 error code 0x%x\n", rc);
        return rc;
    }

    return GT_OK;
}




