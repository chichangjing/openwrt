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
* mvHwsHglPcsDb.c
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/hglPcs/mvHwsHglPcsDb.h>

MV_CFG_SEQ hwsHglPscSeqDb[MV_HGLPCS_LAST_SEQ];

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS modeMiscSeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg1,           (1 << 4),  (1 << 4),  0, 0},
    {XPCS_UNIT, XPCS_Global_Fifo_Threshold_Configuration,  0xF,       0xF,      0, 0},
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,           (1 << 3),  (3 << 3),  0, 0}
};

static MV_OP_PARAMS modeLane4SeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,           (2 << 5),  (3 << 5),  0, 0},
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,           (7 << 13), (7 << 13), 0, 0}
};

static MV_OP_PARAMS modeLane6SeqParams[] = {
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,           (3 << 5),  (3 << 5),  0, 0},
    {XPCS_UNIT, XPCS_Global_Configuration_Reg0,           (5 << 13), (7 << 13), 0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {modeMiscSeqParams,  MV_SEQ_SIZE(modeMiscSeqParams)},  /* HGLPCS_MODE_MISC_SEQ   */
    {modeLane4SeqParams, MV_SEQ_SIZE(modeLane4SeqParams)}, /* HGLPCS_MODE_4_LANE_SEQ */
    {modeLane6SeqParams, MV_SEQ_SIZE(modeLane6SeqParams)}  /* HGLPCS_MODE_6_LANE_SEQ */
};

GT_STATUS hwsHglPscSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* HGL PCS Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsHglPscSeqDb[0], seqIdToParams, MV_HGLPCS_LAST_SEQ));

    /* HGL PCS Sequences operation & parameters update */
    mvInitSeqOperation(&hwsHglPscSeqDb[0], seqIdToParams, MV_HGLPCS_LAST_SEQ);

    return GT_OK;
}

void hwsHglPcsSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_HGLPCS_LAST_SEQ); i++)
  {
      if (hwsHglPscSeqDb[i].cfgSeq != NULL)
      {
          hwsOsFreeFuncPtr(hwsHglPscSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsHglPcsSeqGet(MV_HWS_HGLPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_HGLPCS_LAST_SEQ) ||
      (hwsHglPscSeqDb[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsHglPscSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsHglPcsSeqSet(GT_BOOL firstLine, MV_HWS_HGLPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_HGLPCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsHglPscSeqDb[seqType], numOfOp, seqLine);
}

