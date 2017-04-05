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
*       $Revision: 8 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <pcs/mmPcs/mvHwsMMPcsDbRev3.h>


MV_CFG_SEQ hwsMmPscSeqDbRev3[MV_MM_PCS_LAST_SEQ];

#ifndef CO_CPU_RUN

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS resetLane1SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x840,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS unresetLane1SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x40,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x841,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x845,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x847,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS fabricResetLane2SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x800,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x800,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS fabricUnresetLane2SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x40,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x40,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x841,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x841,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x845,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x845,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x847,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x847,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS fabricResetLane4SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x800,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x800,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x600,          0x800,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x800,          0x800,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS fabricUnresetLane4SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x600,          0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x800,          0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x600,          0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x800,          0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x600,          0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x800,          0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x600,          0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x800,          0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x600,          0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x800,          0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x827,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x827,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x600,          0x827,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x800,          0x827,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS resetLane2SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x800,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x800,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS unresetLane2SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x40,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x40,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x841,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x400,          0x841,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x845,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x845,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x847,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x847,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS resetLane4SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x800,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x800,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,         0x800,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,         0x800,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS unresetLane4SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                  0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,         0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,         0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,         0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,         0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,         0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,         0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,         0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,         0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,         0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,         0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                  0x827,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,         0x827,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,         0x827,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,         0x827,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS modeLane1SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,          0x3820, 0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,          0xA0,   0xFFFF, 0, 0}
};

static MV_OP_PARAMS modeLane2SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,          0x3920, 0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,          0xA0,   0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,          0x1A0,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS modeLane4SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,          0x3A20, 0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,          0xA0,   0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x1000, 0x1A0,  0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x2000, 0x2A0,  0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x3000, 0x3A0,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS fabricModeLane1SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,          0x3820, 0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,          0xA0,   0xFFFF, 0, 0}
};

static MV_OP_PARAMS fabricModeLane2SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,          0x3920, 0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,          0xA0,   0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x400,  0x1A0,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS fabricModeLane4SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,          0x3A20, 0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,          0xA0,   0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x400,  0x1A0,  0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x600,  0x2A0,  0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x800,  0x3A0,  0xFFFF, 0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_RESET_SEQ                 */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_UNRESET_SEQ               */
    {modeLane1SeqParams,          MV_SEQ_SIZE(modeLane1SeqParams)},          /* MMPCS_MODE_1_LANE_SEQ           */
    {modeLane2SeqParams,          MV_SEQ_SIZE(modeLane2SeqParams)},          /* MMPCS_MODE_2_LANE_SEQ           */
    {modeLane4SeqParams,          MV_SEQ_SIZE(modeLane4SeqParams)},          /* MMPCS_MODE_4_LANE_SEQ           */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_LPBK_NORMAL_SEQ           */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_LPBK_RX2TX_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_LPBK_TX2RX_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_GEN_NORMAL_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_GEN_KRPAT_SEQ             */
    {fabricModeLane1SeqParams,    MV_SEQ_SIZE(fabricModeLane1SeqParams)},    /* MMPCS_FABRIC_MODE_1_LANE_SEQ    */
    {fabricModeLane2SeqParams,    MV_SEQ_SIZE(fabricModeLane2SeqParams)},    /* MMPCS_FABRIC_MODE_2_LANE_SEQ    */
    {fabricModeLane4SeqParams,    MV_SEQ_SIZE(fabricModeLane4SeqParams)},    /* MMPCS_FABRIC_MODE_4_LANE_SEQ    */
    {resetLane1SeqParams,         MV_SEQ_SIZE(resetLane1SeqParams)},         /* MMPCS_RESET_1_LANE_SEQ          */
    {resetLane2SeqParams,         MV_SEQ_SIZE(resetLane2SeqParams)},         /* MMPCS_RESET_2_LANE_SEQ          */
    {resetLane4SeqParams,         MV_SEQ_SIZE(resetLane4SeqParams)},         /* MMPCS_RESET_4_LANE_SEQ          */
    {unresetLane1SeqParams,       MV_SEQ_SIZE(unresetLane1SeqParams)},       /* MMPCS_UNRESET_1_LANE_SEQ        */
    {unresetLane2SeqParams,       MV_SEQ_SIZE(unresetLane2SeqParams)},       /* MMPCS_UNRESET_2_LANE_SEQ        */
    {unresetLane4SeqParams,       MV_SEQ_SIZE(unresetLane4SeqParams)},       /* MMPCS_UNRESET_4_LANE_SEQ        */
    {fabricResetLane2SeqParams,   MV_SEQ_SIZE(fabricResetLane2SeqParams)},   /* MMPCS_FABRIC_RESET_2_LANE_SEQ   */
    {fabricResetLane4SeqParams,   MV_SEQ_SIZE(fabricResetLane4SeqParams)},   /* MMPCS_FABRIC_RESET_4_LANE_SEQ   */
    {fabricUnresetLane2SeqParams, MV_SEQ_SIZE(fabricUnresetLane2SeqParams)}, /* MMPCS_FABRIC_UNRESET_2_LANE_SEQ */
    {fabricUnresetLane4SeqParams, MV_SEQ_SIZE(fabricUnresetLane4SeqParams)}, /* MMPCS_FABRIC_UNRESET_4_LANE_SEQ */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_GOP_REV3_UNRESET_1_LANE_SEQ */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_GOP_REV3_UNRESET_2_LANE_SEQ */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_GOP_REV3_UNRESET_4_LANE_SEQ */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_POWER_DOWN_2_LANE_SEQ        */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* MMPCS_POWER_DOWN_4_LANE_SEQ        */
    {NULL,                        MV_SEQ_SIZE(0)}                            /* MMPCS_FABRIC_POWER_DOWN_2_LANE_SEQ */
};

GT_STATUS hwsMMPscRev3SeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* MM PCS Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsMmPscSeqDbRev3[0], seqIdToParams, MV_MM_PCS_LAST_SEQ));

    /* MM PCS Sequences operation & parameters update */
    mvInitSeqOperation(&hwsMmPscSeqDbRev3[0], seqIdToParams, MV_MM_PCS_LAST_SEQ);

    return GT_OK;
}

void hwsMMPcsRev3SeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_MM_PCS_LAST_SEQ); i++)
  {
      if (hwsMmPscSeqDbRev3[i].cfgSeq != NULL)
      {
          hwsOsFreeFuncPtr(hwsMmPscSeqDbRev3[i].cfgSeq);
      }
  }

}

GT_STATUS hwsMMPcsRev3SeqGet(MV_HWS_MM_PCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if (seqType >= MV_MM_PCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  if (hwsMmPscSeqDbRev3[seqType].cfgSeq == NULL)
  {
      /* check prev version */
      return hwsMMPcsSeqGet(seqType, seqLine, lineNum);
  }

  return mvGetSeqOperation(&hwsMmPscSeqDbRev3[seqType], seqLine, lineNum);
}

GT_STATUS hwsMMPcsRev3SeqSet(GT_BOOL firstLine, MV_HWS_MM_PCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_MM_PCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  if (hwsMmPscSeqDbRev3[seqType].cfgSeq == NULL)
  {
      /* check prev version */
      return hwsMMPcsSeqSet(firstLine, seqType, seqLine, numOfOp);
  }

  return mvSetSeqOperation(firstLine, &hwsMmPscSeqDbRev3[seqType], numOfOp, seqLine);
}

#else
GT_STATUS hwsMMPscRev3SeqInit(void)
{
    hwsGenSeqInit(hwsMmPscSeqDbRev3, MV_MM_PCS_LAST_SEQ);
    return GT_OK;
}
#endif

