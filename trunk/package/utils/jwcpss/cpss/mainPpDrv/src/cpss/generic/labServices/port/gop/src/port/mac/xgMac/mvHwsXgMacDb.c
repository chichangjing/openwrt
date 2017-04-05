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
*       $Revision: 14 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <mac/xgMac/mvHwsXgMacDb.h>

MV_CFG_SEQ hwsXgMacSeqDb[MV_MAC_XG_LAST_SEQ];

#ifndef CO_CPU_RUN
static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS modeSeqParams[] = {
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER3, (1 << 13), (7 << 13), 0, 0},
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER4, (1 << 8),  (1 << 8),  0, 0},
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER0,  0x0,      (1 << 11), 0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {modeSeqParams, MV_SEQ_SIZE(modeSeqParams)} /* XGMAC_MODE_SEQ */
};

GT_STATUS hwsXgMacSeqInit(GT_U8 devNum)
{
    MV_OP_PARAMS *updateParams;
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* XG MAC Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsXgMacSeqDb[0], seqIdToParams, MV_MAC_XG_LAST_SEQ));

    /* XG MAC Sequences operation & parameters update */
    mvInitSeqOperation(&hwsXgMacSeqDb[0], seqIdToParams, MV_MAC_XG_LAST_SEQ);

    /* XG MAC Sequences update */
    if (HWS_DEV_GOP_REV(devNum) >= GOP_28NM_REV2)
    {
        updateParams = (MV_OP_PARAMS*)hwsXgMacSeqDb[XGMAC_MODE_SEQ].cfgSeq[1].params;
        updateParams->mask |= (1 << 12);
    }

    return GT_OK;
}

void hwsXgMacSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_MAC_XG_LAST_SEQ); i++)
  {
      if (hwsXgMacSeqDb[i].cfgSeq != NULL)
      {
          hwsOsFreeFuncPtr(hwsXgMacSeqDb[i].cfgSeq);
      }
  }

}

GT_STATUS hwsXgMacSeqGet(MV_HWS_XG_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if ((seqType >= MV_MAC_XG_LAST_SEQ) ||
      (hwsXgMacSeqDb[seqType].cfgSeq == NULL))
  {
      return GT_NO_SUCH;
  }

  return mvGetSeqOperation(&hwsXgMacSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsXgMacSeqSet(GT_BOOL firstLine, MV_HWS_XG_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_MAC_XG_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  return mvSetSeqOperation(firstLine, &hwsXgMacSeqDb[seqType], numOfOp, seqLine);
}
#else
GT_STATUS hwsXgMacSeqInit(void)
{

    hwsGenSeqInit(hwsXgMacSeqDb, MV_MAC_XG_LAST_SEQ);
    return GT_OK;
}
#endif

