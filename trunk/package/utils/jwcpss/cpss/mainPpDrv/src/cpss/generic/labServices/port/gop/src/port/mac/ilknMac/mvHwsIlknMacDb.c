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
* mvHwsIlknMacDb.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <mac/ilknMac/mvHwsIlknMacDb.h>

MV_CFG_SEQ hwsIlknMacSeqDb[MV_MAC_INTKLN_LAST_SEQ];

#ifndef CO_CPU_RUN
static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS mode4LanesSeqParams[] = {
    {INTLKN_UNIT,    RXDMA_converter_control_0, 0x41,       0x57,   0, 0},
    {INTLKN_UNIT,    TXDMA_converter_control_0, 0x15009,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_0,          0x0,        0x7700, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_0,        0x100F0F0,  0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_1,        0x1FF00,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_2,          0x15000100, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_3,          0x303,      0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_4,          0x60303,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_6,          0x11030000, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,            0x2,        0x2,    0, 0}
};

static MV_OP_PARAMS mode8LanesSeqParams[] = {
    {INTLKN_UNIT,    RXDMA_converter_control_0, 0x42,       0x57,   0, 0},
    {INTLKN_UNIT,    TXDMA_converter_control_0, 0x1500A,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_0,          0x1100,     0x7700, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_0,        0x100F0F0,  0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_1,        0x1FF00,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_2,          0x15000100, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_3,          0x707,      0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_4,          0x60303,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_6,          0x11030000, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,            0x2,        0x2,    0, 0}
};

static MV_OP_PARAMS mode12LanesSeqParams[] = {
    {INTLKN_UNIT,    RXDMA_converter_control_0, 0x3,        0x7,    0, 0},
    {INTLKN_UNIT,    TXDMA_converter_control_0, 0x3,        0x7,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_0,          0x2200,     0x7700, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_0,        0x100F0F0,  0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_1,        0x1FF00,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_2,          0x15000100, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_3,          0xB0B,      0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_4,          0x20301,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_6,          0x11030000, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,            0x2,        0x2,    0, 0}
};

static MV_OP_PARAMS mode16LanesSeqParams[] = {
    {INTLKN_UNIT,    RXDMA_converter_control_0, 0x4,        0x7,    0, 0},
    {INTLKN_UNIT,    TXDMA_converter_control_0, 0x4,        0x7,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_0,          0x3300,     0x7700, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_0,        0x100F0F0,  0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_1,        0x1FF00,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_2,          0x15000100, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_3,          0xF0F,      0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_4,          0x20301,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_6,          0x11030000, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,            0x2,        0x2,    0, 0}
};

static MV_OP_PARAMS mode24LanesSeqParams[] = {
    {INTLKN_UNIT,    RXDMA_converter_control_0, 0x6,        0x7,    0, 0},
    {INTLKN_UNIT,    TXDMA_converter_control_0, 0x6,        0x7,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_0,          0x5500,     0x7700, 0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_0,        0x100F0F0,  0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_CH_FC_CFG_1,        0x1FF00,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_2,          0x15000100, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_3,          0x1717,     0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_4,          0x20301,    0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_MAC_CFG_6,          0x11030000, 0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_STAT_EN,            0x2,        0x2,    0, 0}
};

static MV_OP_PARAMS resetSeqParams[] = {
    {INTLKN_UNIT,    ILKN_RESETS,               0x0,        0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_EN,                 0x0,        0x3,    0, 0}
};

static MV_OP_PARAMS unresetSeqParams[] = {
    {INTLKN_UNIT,    ILKN_RESETS,               0x2F,       0x0,    0, 0},
    {INTLKN_UNIT,    ILKN_RESETS,               0x3F,       0x0,    0, 0},
    {INTLKN_RF_UNIT, ILKN_0_EN,                 0x3,        0x3,    0, 0},
    {INTLKN_UNIT,    GENX_converter_control_0,  0x1,        0x1,    0, 0},
    {INTLKN_UNIT,    GENX_converter_control_0,  0x0,        0x1,    0, 0}
};

static MV_OP_PARAMS lbNormalSeqParams[] = {
    {INTLKN_UNIT,    RXDMA_converter_control_0, 0x0,        0x20,   0, 0}
};

static MV_OP_PARAMS lbTx2RxSeqParams[] = {
    {INTLKN_UNIT,    RXDMA_converter_control_0, 0x20,       0x20,   0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {unresetSeqParams,     MV_SEQ_SIZE(unresetSeqParams)},      /* INTLKN_MAC_UNRESET_SEQ */
    {resetSeqParams,       MV_SEQ_SIZE(resetSeqParams)},        /* INTLKN_MAC_RESET_SEQ   */
    {mode12LanesSeqParams, MV_SEQ_SIZE(mode12LanesSeqParams)},  /* INTLKN_MAC_MODE_12_LAN */
    {mode16LanesSeqParams, MV_SEQ_SIZE(mode16LanesSeqParams)},  /* INTLKN_MAC_MODE_16_LAN */
    {mode24LanesSeqParams, MV_SEQ_SIZE(mode24LanesSeqParams)},  /* INTLKN_MAC_MODE_24_LAN */
    {lbNormalSeqParams,    MV_SEQ_SIZE(lbNormalSeqParams)},     /* INTLKN_MAC_LPBK_NORMAL */
    {lbTx2RxSeqParams,     MV_SEQ_SIZE(lbTx2RxSeqParams)},      /* INTLKN_MAC_LPBK_TX2RX_ */
    {mode8LanesSeqParams,  MV_SEQ_SIZE(mode8LanesSeqParams)},   /* INTLKN_MAC_MODE_4_LANE */
    {mode4LanesSeqParams,  MV_SEQ_SIZE(mode4LanesSeqParams)},   /* INTLKN_MAC_MODE_8_LANE */
};

GT_STATUS hwsIlknMacSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* INTLKN MAC Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsIlknMacSeqDb[0], seqIdToParams, MV_MAC_INTKLN_LAST_SEQ));

    /* INTLKN MAC Sequences operation & parameters update */
    mvInitSeqOperation(&hwsIlknMacSeqDb[0], seqIdToParams, MV_MAC_INTKLN_LAST_SEQ);

    return GT_OK;
}
#else
GT_STATUS hwsIlknMacSeqInit(void)
{
    hwsGenSeqInit(hwsIlknMacSeqDb, MV_MAC_INTKLN_LAST_SEQ);
    return GT_OK;
}
#endif

#ifndef CO_CPU_RUN
void hwsIlknMacSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_MAC_INTKLN_LAST_SEQ); i++)
  {
      if (hwsIlknMacSeqDb[i].cfgSeq != NULL)
      {
          hwsOsFreeFuncPtr(hwsIlknMacSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsIlknMacSeqGet(MV_HWS_INTKLN_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_MAC_INTKLN_LAST_SEQ) ||
      (hwsIlknMacSeqDb[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsIlknMacSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsIlknMacSeqSet(GT_BOOL firstLine, MV_HWS_INTKLN_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_MAC_INTKLN_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsIlknMacSeqDb[seqType], numOfOp, seqLine);
}
#endif
