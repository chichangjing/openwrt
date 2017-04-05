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
* mvHwsXlgMacDb.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <mac/xlgMac/mvHwsXlgMacDbRev3.h>

MV_CFG_SEQ hwsXlgMacSeqDbRev3[MV_MAC_XLG_LAST_SEQ];

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS mode1LaneSeqParams[] = {
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER3, 0x2000, 0xE000, 0, 0},
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER4, 0x210,  0xF10,  0, 0}
};

static MV_OP_PARAMS mode2LaneSeqParams[] = {
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER3, 0x4000, 0xE000, 0, 0},
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER4, 0x410,  0xF10,  0, 0}
};

static MV_OP_PARAMS mode4LaneSeqParams[] = {
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER3, 0x6000, 0xE000, 0, 0},
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER4, 0x410,  0xF10,  0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
     {NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_UNRESET_SEQ     */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_RESET_SEQ       */
    ,{mode1LaneSeqParams, MV_SEQ_SIZE(mode1LaneSeqParams)} /* XLGMAC_MODE_1_Lane_SEQ */
    ,{mode2LaneSeqParams, MV_SEQ_SIZE(mode2LaneSeqParams)} /* XLGMAC_MODE_2_Lane_SEQ */
    ,{mode4LaneSeqParams, MV_SEQ_SIZE(mode4LaneSeqParams)} /* XLGMAC_MODE_4_Lane_SEQ */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_LPBK_NORMAL_SEQ */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_LPBK_RX2TX_SEQ  */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_LPBK_TX2RX_SEQ  */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_FC_DISABLE_SEQ  */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_FC_BOTH_SEQ     */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_FC_RX_ONLY_SEQ  */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_FC_TX_ONLY_SEQ  */
    ,{NULL,               MV_SEQ_SIZE(0)}                  /* XLGMAC_POWER_DOWN_SEQ  */
};

GT_STATUS hwsXlgMacRev3SeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* XLG MAC Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsXlgMacSeqDbRev3[0], seqIdToParams, MV_MAC_XLG_LAST_SEQ));

    /* XLG MAC Sequences operation & parameters update */
    mvInitSeqOperation(&hwsXlgMacSeqDbRev3[0], seqIdToParams, MV_MAC_XLG_LAST_SEQ);

    return GT_OK;
}

void hwsXlgMacRev3SeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_MAC_XLG_LAST_SEQ); i++)
  {
      if (hwsXlgMacSeqDbRev3[i].cfgSeq != NULL)
      {
          hwsOsFreeFuncPtr(hwsXlgMacSeqDbRev3[i].cfgSeq);
      }
  }
}

GT_STATUS hwsXlgMacRev3SeqGet(MV_HWS_XLG_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_MAC_XLG_LAST_SEQ) ||
      (hwsXlgMacSeqDbRev3[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsXlgMacSeqDbRev3[seqType], seqLine, lineNum);
}

GT_STATUS hwsXlgMacRev3SeqSet(GT_BOOL firstLine, MV_HWS_XLG_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_MAC_XLG_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsXlgMacSeqDbRev3[seqType], numOfOp, seqLine);
}

