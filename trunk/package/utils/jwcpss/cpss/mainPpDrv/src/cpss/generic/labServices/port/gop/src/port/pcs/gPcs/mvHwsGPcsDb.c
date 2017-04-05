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
* mvHwsGPcsDb.c
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <pcs/gPcs/mvHwsGPcsDb.h>
#include <mac/geMac/mvHwsGeMacDb.h>

MV_CFG_SEQ hwsGPscSeqDb[MV_GPCS_LAST_SEQ];

#ifndef CO_CPU_RUN

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS resetSeqParams[] = {
    {GEMAC_UNIT, GIG_PORT_MAC_CONTROL_REGISTER2,  0x0,      (1 << 3),  0, 0}
};

static MV_OP_PARAMS unresetSeqParams[] = {
    {GEMAC_UNIT, GIG_PORT_MAC_CONTROL_REGISTER2, (1 << 3),  (1 << 3),  0, 0}
};

static MV_OP_PARAMS lbNormalSeqParams[] = {
    {GEMAC_UNIT, GIG_PORT_MAC_CONTROL_REGISTER1,  0x0,      (1 << 6),  0, 0}
};

static MV_OP_PARAMS lbTx2RxSeqParams[] = {
    {GEMAC_UNIT, GIG_PORT_MAC_CONTROL_REGISTER1, (1 << 6),  (1 << 6),  0, 0}
};

static MV_OP_PARAMS genPrbs7SeqParams[] = {
    {GEMAC_UNIT, GIG_PORT_MAC_CONTROL_REGISTER2, (3 << 10), (3 << 10), 0, 0}
};

static MV_OP_PARAMS genNormalSeqParams[] = {
    {GEMAC_UNIT, GIG_PORT_MAC_CONTROL_REGISTER2,  0x0,      (3 << 10), 0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {resetSeqParams,     MV_SEQ_SIZE(resetSeqParams)},    /* GPCS_RESET_SEQ       */
    {unresetSeqParams,   MV_SEQ_SIZE(unresetSeqParams)},  /* GPCS_UNRESET_SEQ     */
    {lbNormalSeqParams,  MV_SEQ_SIZE(lbNormalSeqParams)}, /* GPCS_LPBK_NORMAL_SEQ */
    {lbTx2RxSeqParams,   MV_SEQ_SIZE(lbTx2RxSeqParams)},  /* GPCS_LPBK_TX2RX_SEQ  */
    {genPrbs7SeqParams,  MV_SEQ_SIZE(genPrbs7SeqParams)}, /* GPCS_GEN_PRBS7_SEQ   */
    {genNormalSeqParams, MV_SEQ_SIZE(genNormalSeqParams)} /* GPCS_GEN_NORMAL_SEQ  */
};

GT_STATUS hwsGPscSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* G PCS Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsGPscSeqDb[0], seqIdToParams, MV_GPCS_LAST_SEQ));

    /* G PCS Sequences operation & parameters update */
    mvInitSeqOperation(&hwsGPscSeqDb[0], seqIdToParams, MV_GPCS_LAST_SEQ);

    return GT_OK;
}

void hwsGPcsSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_GPCS_LAST_SEQ); i++)
  {
      if (hwsGPscSeqDb[i].cfgSeq != NULL)
      {
        hwsOsFreeFuncPtr(hwsGPscSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsGPcsSeqGet(MV_HWS_GPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_GPCS_LAST_SEQ) ||
      (hwsGPscSeqDb[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsGPscSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsGPcsSeqSet(GT_BOOL firstLine, MV_HWS_GPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_GPCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsGPscSeqDb[seqType], numOfOp, seqLine);
}

#else
GT_STATUS hwsGPscSeqInit(void)
{
    hwsGenSeqInit(hwsGPscSeqDb, MV_GPCS_LAST_SEQ);
    return GT_OK;
}
#endif

