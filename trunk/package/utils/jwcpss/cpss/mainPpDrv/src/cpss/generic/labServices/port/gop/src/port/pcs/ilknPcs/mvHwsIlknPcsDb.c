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
* mvHwsIlknPcsDb.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/ilknPcs/mvHwsIlknPcsDb.h>

MV_CFG_SEQ hwsIlknPcsPscSeqDb[MV_ILKNPCS_LAST_SEQ];

#ifndef CO_CPU_RUN
static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS resetSeqParams[] = {
    {INTLKN_RF_UNIT, ILKN_0_EN,                     0x0,       0xC, 0, 0}
};

static MV_OP_PARAMS unresetSeqParams[] = {
    {INTLKN_RF_UNIT, ILKN_0_EN,                     0xC,       0xC, 0, 0}
};

static MV_OP_PARAMS modeMiscSeqParams[] = {
    {INTLKN_UNIT,    SD_FC_LED_converter_control_0, 0x4,       0x0, 0, 0},
    {INTLKN_UNIT,    SD_FC_LED_converter_control_0, 0xC,       0x0, 0, 0},
    {INTLKN_UNIT,    ILKN_RESETS,                   0x2,       0x2, 0, 0}
};

static MV_OP_PARAMS modeLane4SeqParams[] = {
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_0,              0x7FF07FF, 0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_1,              0xF,       0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,                0x3,       0x3, 0, 0}
};

static MV_OP_PARAMS modeLane8SeqParams[] = {
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_0,              0x7FF07FF, 0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_1,              0xFF,      0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,                0x3,       0x3, 0, 0}
};

static MV_OP_PARAMS modeLane12SeqParams[] = {
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_0,              0x7FF07FF, 0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_1,              0xFFF,     0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,                0x3,       0x3, 0, 0}
};

static MV_OP_PARAMS modeLane16SeqParams[] = {
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_0,              0x7FF07FF, 0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_1,              0xFFFF,    0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,                0x3,       0x3, 0, 0}
};

static MV_OP_PARAMS modeLane24SeqParams[] = {
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_0,              0x7FF07FF, 0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_PCS_CFG_1,              0xFFFFFF,  0x0, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,                0x3,       0x3, 0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {resetSeqParams,    MV_SEQ_SIZE(resetSeqParams)},        /* ILKNPCS_RESET_SEQ        */
    {unresetSeqParams,  MV_SEQ_SIZE(unresetSeqParams)},      /* ILKNPCS_UNRESET_SEQ      */
    {modeMiscSeqParams, MV_SEQ_SIZE(modeMiscSeqParams)},     /* ILKNPCS_MODE_MISC_SEQ    */
    {modeLane12SeqParams, MV_SEQ_SIZE(modeLane12SeqParams)}, /* ILKNPCS_MODE_12_LANE_SEQ */
    {modeLane16SeqParams, MV_SEQ_SIZE(modeLane16SeqParams)}, /* ILKNPCS_MODE_16_LANE_SEQ */
    {modeLane24SeqParams, MV_SEQ_SIZE(modeLane24SeqParams)}, /* ILKNPCS_MODE_24_LANE_SEQ */
    {modeLane4SeqParams, MV_SEQ_SIZE(modeLane4SeqParams)},   /* ILKNPCS_MODE_4_LANE_SEQ  */
    {modeLane8SeqParams, MV_SEQ_SIZE(modeLane8SeqParams)}    /* ILKNPCS_MODE_8_LANE_SEQ  */
};

GT_STATUS hwsIlknPcsSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* ILKN PCS Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsIlknPcsPscSeqDb[0], seqIdToParams, MV_ILKNPCS_LAST_SEQ));

    /* ILKN PCS Sequences operation & parameters update */
    mvInitSeqOperation(&hwsIlknPcsPscSeqDb[0], seqIdToParams, MV_ILKNPCS_LAST_SEQ);

    return GT_OK;
}

void hwsIlknPcsSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_ILKNPCS_LAST_SEQ); i++)
  {
      if (hwsIlknPcsPscSeqDb[i].cfgSeq != NULL)
      {
          hwsOsFreeFuncPtr(hwsIlknPcsPscSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsIlknPcsSeqGet(MV_HWS_ILKNPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_ILKNPCS_LAST_SEQ) ||
      (hwsIlknPcsPscSeqDb[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsIlknPcsPscSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsIlknPcsSeqSet(GT_BOOL firstLine, MV_HWS_ILKNPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_ILKNPCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsIlknPcsPscSeqDb[seqType], numOfOp, seqLine);
}

#else
GT_STATUS hwsIlknPcsSeqInit(void)
{
    hwsGenSeqInit(hwsIlknPcsPscSeqDb, MV_ILKNPCS_LAST_SEQ);
    return GT_OK;
}
#endif


