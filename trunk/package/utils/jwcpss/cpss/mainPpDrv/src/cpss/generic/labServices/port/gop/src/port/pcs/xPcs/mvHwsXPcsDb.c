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
* mvHwsXPcsDb.c
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/xPcs/mvHwsXPcsDb.h>

MV_CFG_SEQ hwsXpcsPscSeqDb[MV_XPCS_LAST_SEQ];

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS resetSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,  0x0,          0x1,         0, 0}
};

static MV_OP_PARAMS unresetSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,  0x1,          0x1,         0, 0}
};

static MV_OP_PARAMS modeMiscSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,  0x0,         (3 << 3),     0, 0},
    {XPCS_UNIT, XPCS_Lane0_configuration_Reg,    0x0,         (1 << 6),     0, 0},
    {XPCS_UNIT, XPCS_Lane1_configuration_Reg,    0x0,         (1 << 6),     0, 0},
    {XPCS_UNIT, XPCS_Lane2_configuration_Reg,    0x0,         (1 << 6),     0, 0},
    {XPCS_UNIT, XPCS_Lane3_configuration_Reg,    0x0,         (1 << 6),     0, 0},
    {XPCS_UNIT, XPCS_Lane4_configuration_Reg,    0x0,         (1 << 6),     0, 0},
    {XPCS_UNIT, XPCS_Lane5_configuration_Reg,    0x0,         (1 << 6),     0, 0}
};

static MV_OP_PARAMS modeLane1SeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,  0x0,         (3 << 5),     0, 0}
};

static MV_OP_PARAMS modeLane2SeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0, (1 << 5),     (3 << 5),     0, 0}
};

static MV_OP_PARAMS modeLane4SeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0, (2 << 5),     (3 << 5),     0, 0}
};

static MV_OP_PARAMS lbNormalSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg1,  0x0,         (3 << 1),     0, 0}
};

static MV_OP_PARAMS lbRx2TxSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg1, (2 << 1),     (3 << 1),     0, 0}
};

static MV_OP_PARAMS lbTx2RxSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg1, (1 << 1),     (3 << 1),     0, 0}
};

static MV_OP_PARAMS genPrbs7SeqParams[] = {
    {XPCS_UNIT, XPCS_Lane0_configuration_Reg,   (0x17 << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane1_configuration_Reg,   (0x17 << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane2_configuration_Reg,   (0x17 << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane3_configuration_Reg,   (0x17 << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane4_configuration_Reg,   (0x17 << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane5_configuration_Reg,   (0x17 << 11), (0x1F << 11), 0, 0}
};

static MV_OP_PARAMS genPrbs23SeqParams[] = {
    {XPCS_UNIT, XPCS_Lane0_configuration_Reg,   (0x1B << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane1_configuration_Reg,   (0x1B << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane2_configuration_Reg,   (0x1B << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane3_configuration_Reg,   (0x1B << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane4_configuration_Reg,   (0x1B << 11), (0x1F << 11), 0, 0},
    {XPCS_UNIT, XPCS_Lane5_configuration_Reg,   (0x1B << 11), (0x1F << 11), 0, 0}
};

static MV_OP_PARAMS xpcsGenCjpatSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg1, (0x1 << 10),  (0x1 << 10),  0, 0},
    {XPCS_UNIT, XPCS_Lane0_configuration_Reg,   (0x1 << 7),   (0x1 << 7),   0, 0},
    {XPCS_UNIT, XPCS_Lane1_configuration_Reg,   (0x1 << 7),   (0x1 << 7),   0, 0},
    {XPCS_UNIT, XPCS_Lane2_configuration_Reg,   (0x1 << 7),   (0x1 << 7),   0, 0},
    {XPCS_UNIT, XPCS_Lane3_configuration_Reg,   (0x1 << 7),   (0x1 << 7),   0, 0},
    {XPCS_UNIT, XPCS_Lane4_configuration_Reg,   (0x1 << 7),   (0x1 << 7),   0, 0},
    {XPCS_UNIT, XPCS_Lane5_configuration_Reg,   (0x1 << 7),   (0x1 << 7),   0, 0}
};

static MV_OP_PARAMS xpcsGenCrpatSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg1, (0x1 << 11),  (0x1 << 11),  0, 0},
    {XPCS_UNIT, XPCS_Lane0_configuration_Reg,   (0x1 << 8),   (0x1 << 8),   0, 0},
    {XPCS_UNIT, XPCS_Lane1_configuration_Reg,   (0x1 << 8),   (0x1 << 8),   0, 0},
    {XPCS_UNIT, XPCS_Lane2_configuration_Reg,   (0x1 << 8),   (0x1 << 8),   0, 0},
    {XPCS_UNIT, XPCS_Lane3_configuration_Reg,   (0x1 << 8),   (0x1 << 8),   0, 0},
    {XPCS_UNIT, XPCS_Lane4_configuration_Reg,   (0x1 << 8),   (0x1 << 8),   0, 0},
    {XPCS_UNIT, XPCS_Lane5_configuration_Reg,   (0x1 << 8),   (0x1 << 8),   0, 0}
};

static MV_OP_PARAMS genNormalSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg1, (0 << 10),    (3 << 10),    0, 0},
    {XPCS_UNIT, XPCS_Lane0_configuration_Reg,    0x0,          0xF980,      0, 0},
    {XPCS_UNIT, XPCS_Lane1_configuration_Reg,    0x0,          0xF980,      0, 0},
    {XPCS_UNIT, XPCS_Lane2_configuration_Reg,    0x0,          0xF980,      0, 0},
    {XPCS_UNIT, XPCS_Lane3_configuration_Reg,    0x0,          0xF980,      0, 0},
    {XPCS_UNIT, XPCS_Lane4_configuration_Reg,    0x0,          0xF980,      0, 0},
    {XPCS_UNIT, XPCS_Lane5_configuration_Reg,    0x0,          0xF980,      0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {resetSeqParams,        MV_SEQ_SIZE(resetSeqParams)},        /* XPCS_RESET_SEQ       */
    {unresetSeqParams,      MV_SEQ_SIZE(unresetSeqParams)},      /* XPCS_UNRESET_SEQ     */
    {modeMiscSeqParams,     MV_SEQ_SIZE(modeMiscSeqParams)},     /* XPCS_MODE_MISC_SEQ   */
    {modeLane1SeqParams,    MV_SEQ_SIZE(modeLane1SeqParams)},    /* XPCS_MODE_1_LANE_SEQ */
    {modeLane2SeqParams,    MV_SEQ_SIZE(modeLane2SeqParams)},    /* XPCS_MODE_2_LANE_SEQ */
    {modeLane4SeqParams,    MV_SEQ_SIZE(modeLane4SeqParams)},    /* XPCS_MODE_4_LANE_SEQ */
    {lbNormalSeqParams,     MV_SEQ_SIZE(lbNormalSeqParams)},     /* XPCS_LPBK_NORMAL_SEQ */
    {lbRx2TxSeqParams,      MV_SEQ_SIZE(lbRx2TxSeqParams)},      /* XPCS_LPBK_RX2TX_SEQ  */
    {lbTx2RxSeqParams,      MV_SEQ_SIZE(lbTx2RxSeqParams)},      /* XPCS_LPBK_TX2RX_SEQ  */
    {genPrbs7SeqParams,     MV_SEQ_SIZE(genPrbs7SeqParams)},     /* XPCS_GEN_PRBS7_SEQ   */
    {genPrbs23SeqParams,    MV_SEQ_SIZE(genPrbs23SeqParams)},    /* XPCS_GEN_PRBS23_SEQ  */
    {xpcsGenCjpatSeqParams, MV_SEQ_SIZE(xpcsGenCjpatSeqParams)}, /* XPCS_GEN_CJPAT_SEQ   */
    {xpcsGenCrpatSeqParams, MV_SEQ_SIZE(xpcsGenCrpatSeqParams)}, /* XPCS_GEN_CRPAT_SEQ   */
    {genNormalSeqParams,    MV_SEQ_SIZE(genNormalSeqParams)}     /* XPCS_GEN_NORMAL_SEQ  */
};

#ifndef CO_CPU_RUN
GT_STATUS hwsXPscSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* X PCS Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsXpcsPscSeqDb[0], seqIdToParams, MV_XPCS_LAST_SEQ));

    /* X PCS Sequences operation & parameters update */
    mvInitSeqOperation(&hwsXpcsPscSeqDb[0], seqIdToParams, MV_XPCS_LAST_SEQ);

    return GT_OK;
}

void hwsXPcsSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_XPCS_LAST_SEQ); i++)
  {
      if (hwsXpcsPscSeqDb[i].cfgSeq != NULL)
      {
        hwsOsFreeFuncPtr(hwsXpcsPscSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsXPcsSeqGet(MV_HWS_XPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_XPCS_LAST_SEQ) ||
      (hwsXpcsPscSeqDb[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsXpcsPscSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsXPcsSeqSet(GT_BOOL firstLine, MV_HWS_XPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_XPCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsXpcsPscSeqDb[seqType], numOfOp, seqLine);
}
#else
GT_STATUS hwsXPscSeqInit(void)
{
    hwsGenSeqInit(hwsXpcsPscSeqDb, MV_XPCS_LAST_SEQ);
    return GT_OK;
}
#endif

