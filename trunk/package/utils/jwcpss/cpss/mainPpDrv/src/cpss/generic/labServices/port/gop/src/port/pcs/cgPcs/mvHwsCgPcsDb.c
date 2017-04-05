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
* mvHwsCgPcsDb.c
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
#include <pcs/cgPcs/mvHwsCgPcsDb.h>

MV_CFG_SEQ hwsCgPcsPscSeqDb[MV_CGPCS_LAST_SEQ];

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS modeMiscSeqParams[] = {
    {CG_UNIT, CG_CONTROL_0, 0x1B,      0x3FFFFFF, 0, 0},
    {CG_UNIT, CG_RESETS,    0x0,       0x4000000, 0, 0},
    {CG_UNIT, CG_RESETS,    0x4000000, 0x4000000, 0, 0}
};

static MV_OP_PARAMS modeLane10SeqParams[] = {
    {CG_UNIT, CG_CONTROL_0, 0x400000,  0x400000,  0, 0},
    {CG_UNIT, CG_CONTROL_0, 0x3,       0x3,       0, 0}
};

static MV_OP_PARAMS modeLane12SeqParams[] = {
    {CG_UNIT, CG_CONTROL_0, 0x0,       0x400000,  0, 0},
    {CG_UNIT, CG_CONTROL_0, 0x3,       0x3,       0, 0}
};

static MV_OP_PARAMS lbNormalSeqParams[] = {
    {CG_UNIT, CG_CONTROL_1, 0x0,       0x4000,    0, 0}
};

static MV_OP_PARAMS lbTx2RxSeqParams[] = {
    {CG_UNIT, CG_CONTROL_1, 0x4000,    0x4000,    0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {NULL,                MV_SEQ_SIZE(0)},                   /* CGPCS_RESET_SEQ        */
    {NULL,                MV_SEQ_SIZE(0)},                   /* CGPCS_UNRESET_SEQ      */
    {modeMiscSeqParams,   MV_SEQ_SIZE(modeMiscSeqParams)},   /* CGPCS_MODE_MISC_SEQ    */
    {modeLane10SeqParams, MV_SEQ_SIZE(modeLane10SeqParams)}, /* CGPCS_MODE_10_LANE_SEQ */
    {modeLane12SeqParams, MV_SEQ_SIZE(modeLane12SeqParams)}, /* CGPCS_MODE_12_LANE_SEQ */
    {lbNormalSeqParams,   MV_SEQ_SIZE(lbNormalSeqParams)},   /* CGPCS_LPBK_NORMAL_SEQ  */
    {lbTx2RxSeqParams,    MV_SEQ_SIZE(lbTx2RxSeqParams)}     /* CGPCS_LPBK_TX2RX_SEQ   */
};

GT_STATUS hwsCgPcsSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* CG PCS Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsCgPcsPscSeqDb[0], seqIdToParams, MV_CGPCS_LAST_SEQ));

    /* CG PCS Sequences operation & parameters update */
    mvInitSeqOperation(&hwsCgPcsPscSeqDb[0], seqIdToParams, MV_CGPCS_LAST_SEQ);

    return GT_OK;
}

void hwsCgPcsSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_CGPCS_LAST_SEQ); i++)
  {
      if (hwsCgPcsPscSeqDb[i].cfgSeq != NULL)
      {
        hwsOsFreeFuncPtr(hwsCgPcsPscSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsCgPcsSeqGet(MV_HWS_CGPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_CGPCS_LAST_SEQ) ||
      (hwsCgPcsPscSeqDb[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsCgPcsPscSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsCgPcsSeqSet(GT_BOOL firstLine, MV_HWS_CGPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_CGPCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsCgPcsPscSeqDb[seqType], numOfOp, seqLine);
}

