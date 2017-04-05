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
* mvHwsMMPcsDb.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 21 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <pcs/mmPcs/mvHwsMMPcsDb.h>

#define MMPCS_IND_OFFSET 0x1000

MV_CFG_SEQ hwsMmPscSeqDb[MV_MM_PCS_LAST_SEQ];

#ifndef CO_CPU_RUN

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS resetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                                 0x0,        0x7F,               0, 0}
};

static MV_OP_PARAMS unresetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                                 0x1,        0x1,                0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                 0x2,        0x2,                0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                 0x4,        0x4,                0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                 0x20,       0x20,               0, 0}
};

static MV_OP_PARAMS mmpcsLane1ModeSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                         0x0,       (3 << 8) + (1 << 7), 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                        (1 << 14),  (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (3 << 8),            0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                (4 << 7),   (0xF << 7),          0, 0},
    {MMPCS_UNIT, TX_IDLE_REMOVAL_THRESHOLDS,                   (0xA << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, TX_IDLE_INSERTION_THRESHOLDS,                 (0x6 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_REMOVAL_THRESHOLDS,                   (0xB << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_INSERTION_THRESHOLDS,                 (0x8 << 8),  0xFF00,             0, 0}
};

static MV_OP_PARAMS mmpcsLane2ModeSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                        (1 << 8),   (3 << 8) + (1 << 7), 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + MMPCS_IND_OFFSET,      0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + MMPCS_IND_OFFSET,     (1 << 8),   (3 << 8),            0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                (2 << 7),   (0xF << 7),          0, 0},
    {MMPCS_UNIT, TX_IDLE_REMOVAL_THRESHOLDS,                   (0x9 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, TX_IDLE_INSERTION_THRESHOLDS,                 (0x5 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_REMOVAL_THRESHOLDS,                   (0xB << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_INSERTION_THRESHOLDS,                 (0x8 << 8),  0xFF00,             0, 0}
};

static MV_OP_PARAMS mmpcsLane4ModeSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                        (2 << 8),   (3 << 8) + (1 << 7), 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION +     MMPCS_IND_OFFSET,  0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 2 * MMPCS_IND_OFFSET,  0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 3 * MMPCS_IND_OFFSET,  0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION                       ,  0x0,       (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION +     MMPCS_IND_OFFSET, (1 << 8),   (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 2 * MMPCS_IND_OFFSET, (2 << 8),   (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 3 * MMPCS_IND_OFFSET, (3 << 8),   (3 << 8),            0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                (1 << 7),   (0xF << 7),          0, 0},
    {MMPCS_UNIT, TX_IDLE_REMOVAL_THRESHOLDS,                   (0x8 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, TX_IDLE_INSERTION_THRESHOLDS,                 (0x4 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_REMOVAL_THRESHOLDS,                   (0xC << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_INSERTION_THRESHOLDS,                 (0x8 << 8),  0xFF00,             0, 0}
};

static MV_OP_PARAMS lbNormalSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                         0x0,       (3 << 3) + (1 << 7), 0, 0}
};

static MV_OP_PARAMS lbTx2RxSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                        (1 << 3),   (3 << 3) + (1 << 7), 0, 0}
};

static MV_OP_PARAMS lbRx2TxSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                        (2 << 3),   (3 << 3) + (1 << 7), 0, 0}
};

static MV_OP_PARAMS genNormalSeqParams[] = {
    {MMPCS_UNIT, TEST_PATTERN_CONFIGURATION,                    0x0,       (1 << 5),            0, 0},
    {MMPCS_UNIT, TEST_PATTERN_CONFIGURATION,                    0x0,        0x3,                0, 0}
};

static MV_OP_PARAMS genKrpatSeqParams[] = {
    {MMPCS_UNIT, TEST_PATTERN_CONFIGURATION,                   (1 << 5),   (1 << 5),            0, 0},
    {MMPCS_UNIT, TEST_PATTERN_CONFIGURATION,                   (2 << 2),   (3 << 2),            0, 0},
    {MMPCS_UNIT, TEST_PATTERN_CONFIGURATION,                    0x3,        0x3,                0, 0}
};

static MV_OP_PARAMS fabricModeLane1SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                         0x0,       (3 << 8) + (1 << 7), 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                        (1 << 14),  (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x400,      0x400,              0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                (4 << 7),   (0xF << 7),          0, 0},
    {MMPCS_UNIT, TX_IDLE_REMOVAL_THRESHOLDS,                   (0xA << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, TX_IDLE_INSERTION_THRESHOLDS,                 (0x6 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_REMOVAL_THRESHOLDS,                   (0xB << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_INSERTION_THRESHOLDS,                 (0x8 << 8),  0xFF00,             0, 0}
};

static MV_OP_PARAMS fabricModeLane2SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                        (1 << 8),   (3 << 8) + (1 << 7), 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x400,                 0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x400,                (1 << 8),   (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                        (1 << 10),  (1 << 10),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x400,                (1 << 10),  (1 << 10),           0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                (2 << 7),   (0xF << 7),          0, 0},
    {MMPCS_UNIT, TX_IDLE_REMOVAL_THRESHOLDS,                   (0x9 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, TX_IDLE_INSERTION_THRESHOLDS,                 (0x5 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_REMOVAL_THRESHOLDS,                   (0xB << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_INSERTION_THRESHOLDS,                 (0x8 << 8),  0xFF00,             0, 0}
};

static MV_OP_PARAMS fabricModeLane4SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                        (2 << 8),   (3 << 8) + (1 << 7), 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x400,                 0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x600,                 0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x800,                 0x0,       (1 << 14),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                         0x0,       (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x400,                (1 << 8),   (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x600,                (2 << 8),   (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x800,                (3 << 8),   (3 << 8),            0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                        (1 << 10),  (1 << 10),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x400,                (1 << 10),  (1 << 10),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x600,                (1 << 10),  (1 << 10),           0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x800,                (1 << 10),  (1 << 10),           0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                                (1 << 7),   (0xF << 7),          0, 0},
    {MMPCS_UNIT, TX_IDLE_REMOVAL_THRESHOLDS,                   (0x8 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, TX_IDLE_INSERTION_THRESHOLDS,                 (0x4 << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_REMOVAL_THRESHOLDS,                   (0xC << 8),  0xFF00,             0, 0},
    {MMPCS_UNIT, RX_IDLE_INSERTION_THRESHOLDS,                 (0x8 << 8),  0xFF00,             0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {resetSeqParams,           MV_SEQ_SIZE(resetSeqParams)},           /* MMPCS_RESET_SEQ                 */
    {unresetSeqParams,         MV_SEQ_SIZE(unresetSeqParams)},         /* MMPCS_UNRESET_SEQ               */
    {mmpcsLane1ModeSeqParams,  MV_SEQ_SIZE(mmpcsLane1ModeSeqParams)},  /* MMPCS_MODE_1_LANE_SEQ           */
    {mmpcsLane2ModeSeqParams,  MV_SEQ_SIZE(mmpcsLane2ModeSeqParams)},  /* MMPCS_MODE_2_LANE_SEQ           */
    {mmpcsLane4ModeSeqParams,  MV_SEQ_SIZE(mmpcsLane4ModeSeqParams)},  /* MMPCS_MODE_4_LANE_SEQ           */
    {lbNormalSeqParams,        MV_SEQ_SIZE(lbNormalSeqParams)},        /* MMPCS_LPBK_NORMAL_SEQ           */
    {lbRx2TxSeqParams,         MV_SEQ_SIZE(lbRx2TxSeqParams)},         /* MMPCS_LPBK_RX2TX_SEQ            */
    {lbTx2RxSeqParams,         MV_SEQ_SIZE(lbTx2RxSeqParams)},         /* MMPCS_LPBK_TX2RX_SEQ            */
    {genNormalSeqParams,       MV_SEQ_SIZE(genNormalSeqParams)},       /* MMPCS_GEN_NORMAL_SEQ            */
    {genKrpatSeqParams,        MV_SEQ_SIZE(genKrpatSeqParams)},        /* MMPCS_GEN_KRPAT_SEQ             */
    {fabricModeLane1SeqParams, MV_SEQ_SIZE(fabricModeLane1SeqParams)}, /* MMPCS_FABRIC_MODE_1_LANE_SEQ    */
    {fabricModeLane2SeqParams, MV_SEQ_SIZE(fabricModeLane2SeqParams)}, /* MMPCS_FABRIC_MODE_2_LANE_SEQ    */
    {fabricModeLane4SeqParams, MV_SEQ_SIZE(fabricModeLane4SeqParams)}, /* MMPCS_FABRIC_MODE_4_LANE_SEQ    */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_RESET_1_LANE_SEQ          */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_RESET_2_LANE_SEQ          */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_RESET_4_LANE_SEQ          */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_UNRESET_1_LANE_SEQ        */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_UNRESET_2_LANE_SEQ        */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_UNRESET_4_LANE_SEQ        */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_FABRIC_RESET_2_LANE_SEQ   */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_FABRIC_RESET_4_LANE_SEQ   */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_FABRIC_UNRESET_2_LANE_SEQ */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_FABRIC_UNRESET_4_LANE_SEQ */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_GOP_REV3_UNRESET_1_LANE_SEQ */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_GOP_REV3_UNRESET_2_LANE_SEQ */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_GOP_REV3_UNRESET_4_LANE_SEQ */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_POWER_DOWN_2_LANE_SEQ        */
    {NULL,                     MV_SEQ_SIZE(0)},                        /* MMPCS_POWER_DOWN_4_LANE_SEQ        */
    {NULL,                     MV_SEQ_SIZE(0)}                         /* MMPCS_FABRIC_POWER_DOWN_2_LANE_SEQ */
};

GT_STATUS hwsMMPscSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* MM PCS Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsMmPscSeqDb[0], seqIdToParams, MV_MM_PCS_LAST_SEQ));

    /* MM PCS Sequences operation & parameters update */
    mvInitSeqOperation(&hwsMmPscSeqDb[0], seqIdToParams, MV_MM_PCS_LAST_SEQ);

    return GT_OK;
}

void hwsMMPcsSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_MM_PCS_LAST_SEQ); i++)
  {
      if (hwsMmPscSeqDb[i].cfgSeq != NULL)
      {
          hwsOsFreeFuncPtr(hwsMmPscSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsMMPcsSeqGet(MV_HWS_MM_PCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if (seqType >= MV_MM_PCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsMmPscSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsMMPcsSeqSet(GT_BOOL firstLine, MV_HWS_MM_PCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_MM_PCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsMmPscSeqDb[seqType], numOfOp, seqLine);
}

#else
GT_STATUS hwsMMPscSeqInit(void)
{
    hwsGenSeqInit(hwsMmPscSeqDb, MV_MM_PCS_LAST_SEQ);
    return GT_OK;
}
#endif


