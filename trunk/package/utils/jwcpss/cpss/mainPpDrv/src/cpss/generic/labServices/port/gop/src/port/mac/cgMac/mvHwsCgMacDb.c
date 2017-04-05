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
* mvHwsCgMacDb.c
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
#include <mac/cgMac/mvHwsCgMacDb.h>

MV_CFG_SEQ hwsCgMacSeqDb[MV_MAC_CG_LAST_SEQ];

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS modeSeqParams[] = {
    {CG_UNIT, CG_RXDMA_CONVERTER_PACKET_SIZE_RESTRICTION, 0x40,       0x3FFF,   0, 0},
    {CG_UNIT, CG_CREDIT_LIMIT,                            0x1c07050,  0x0,      0, 0},
    {CG_UNIT, CG_INIT_CREDIT,                             0x1C,       0x0,      0, 0},
    {CG_UNIT, CG_TX_FIFO_SECTIONS,                        0x6,        0x0,      0, 0},
    {CG_UNIT, CG_COMMAND_CONFIG,                          0xd3,       0x0,      0, 0},
    {CG_UNIT, CG_COMMAND_CONFIG,                          0x200d0,    0x0,      0, 0},
    {CG_UNIT, CG_FRM_LENGTH,                              0x2820,     0x0,      0, 0}
};

static MV_OP_PARAMS resetSeqParams[] = {
    {CG_UNIT, CG_COMMAND_CONFIG,                          0x0,        0x3,      0, 0},
    {CG_UNIT, CG_RESETS,                                  0x0,        0x0,      0, 0}
};

static MV_OP_PARAMS unresetSeqParams[] = {
    {CG_UNIT, CG_RESETS,                                  0x6FFFFFF,  0x0,      0, 0},
    {CG_UNIT, CG_RESETS,                                  0x1FFFFFFF, 0x0,      0, 0},
    {CG_UNIT, CG_CONTROL_0,                               0x100000,   0x100000, 0, 0},
    {CG_UNIT, CG_COMMAND_CONFIG,                          0x3,        0x3,      0, 0}
};

static MV_OP_PARAMS lbNormalSeqParams[] = {
    {CG_UNIT, CG_COMMAND_CONFIG,                          0x0,        0x400,    0, 0}
};

static MV_OP_PARAMS lbTx2RxSeqParams[] = {
    {CG_UNIT, CG_COMMAND_CONFIG,                          0x400,      0x400,    0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
     {unresetSeqParams,  MV_SEQ_SIZE(unresetSeqParams)},	/* CGMAC_UNRESET_SEQ     */
     {resetSeqParams,    MV_SEQ_SIZE(resetSeqParams)},		/* CGMAC_RESET_SEQ       */
     {modeSeqParams,     MV_SEQ_SIZE(modeSeqParams)},		/* CGMAC_MODE_SEQ        */
     {lbNormalSeqParams, MV_SEQ_SIZE(lbNormalSeqParams)},	/* CGMAC_LPBK_NORMAL_SEQ */
     {lbTx2RxSeqParams,  MV_SEQ_SIZE(lbTx2RxSeqParams)}		/* CGMAC_LPBK_TX2RX_SEQ  */
};

GT_STATUS hwsCgMacSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* CG MAC Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsCgMacSeqDb[0], seqIdToParams, MV_MAC_CG_LAST_SEQ));

    /* CG MAC Sequences operation & parameters update */
    mvInitSeqOperation(&hwsCgMacSeqDb[0], seqIdToParams, MV_MAC_CG_LAST_SEQ);

    return GT_OK;
}

void hwsCgMacSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_MAC_CG_LAST_SEQ); i++)
  {
      if (hwsCgMacSeqDb[i].cfgSeq != NULL)
      {
        hwsOsFreeFuncPtr(hwsCgMacSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsCgMacSeqGet(MV_HWS_CG_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_MAC_CG_LAST_SEQ) ||
      (hwsCgMacSeqDb[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsCgMacSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsCgMacSeqSet(GT_BOOL firstLine, MV_HWS_CG_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_MAC_CG_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsCgMacSeqDb[seqType], numOfOp, seqLine);
}


