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
*       $Revision: 13 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <mac/hglMac/mvHwsHglMacDb.h>

MV_CFG_SEQ hwsHglMacSeqDb[MV_MAC_HGL_LAST_SEQ];

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS modeSeqParams[] = {
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER3, (7 << 13), (7 << 13), 0, 0},
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER4, (1 << 8),  (1 << 8),  0, 0},
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER0,  0x0,      (1 << 11), 0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {modeSeqParams, MV_SEQ_SIZE(modeSeqParams)} /* HGLMAC_MODE_SEQ */
};

GT_STATUS hwsHglMacSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* HGL MAC Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsHglMacSeqDb[0], seqIdToParams, MV_MAC_HGL_LAST_SEQ));

    /* HGL MAC Sequences operation & parameters update */
    mvInitSeqOperation(&hwsHglMacSeqDb[0], seqIdToParams, MV_MAC_HGL_LAST_SEQ);

    return GT_OK;
}

void hwsHglMacSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_MAC_HGL_LAST_SEQ); i++)
  {
      if (hwsHglMacSeqDb[i].cfgSeq != NULL)
      {
        hwsOsFreeFuncPtr(hwsHglMacSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsHglMacSeqGet(MV_HWS_HGL_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
    if ((seqType >= MV_MAC_HGL_LAST_SEQ) ||
        (hwsHglMacSeqDb[seqType].cfgSeq == NULL))
    {
        return GT_NO_SUCH;
    }

    return mvGetSeqOperation(&hwsHglMacSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsHglMacSeqSet(GT_BOOL firstLine, MV_HWS_HGL_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
    if (seqType < MV_MAC_HGL_LAST_SEQ)
    {
        return mvSetSeqOperation(firstLine, &hwsHglMacSeqDb[seqType], numOfOp, seqLine);
    }

    return GT_NO_SUCH;
}

