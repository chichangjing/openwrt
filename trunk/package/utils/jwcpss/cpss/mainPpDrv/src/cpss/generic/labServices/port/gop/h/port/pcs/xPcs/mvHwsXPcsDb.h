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
* mvHwsXPcsDb.h
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

#ifndef __mvHwsXPcsDbIf_H
#define __mvHwsXPcsDbIf_H

#ifdef __cplusplus
extern "C" {
#endif

/* General H Files */
#include <gtOs/gtGenTypes.h>
#include <common/configElementDb/mvCfgElementDb.h>


typedef enum
{ 
  XPCS_RESET_SEQ,
  XPCS_UNRESET_SEQ,

  XPCS_MODE_MISC_SEQ,
  XPCS_MODE_1_LANE_SEQ,
  XPCS_MODE_2_LANE_SEQ,
  XPCS_MODE_4_LANE_SEQ,

  XPCS_LPBK_NORMAL_SEQ,
  XPCS_LPBK_RX2TX_SEQ,
  XPCS_LPBK_TX2RX_SEQ,

  XPCS_GEN_PRBS7_SEQ,
  XPCS_GEN_PRBS23_SEQ,
  XPCS_GEN_CJPAT_SEQ,
  XPCS_GEN_CRPAT_SEQ,
  XPCS_GEN_NORMAL_SEQ,

  MV_XPCS_LAST_SEQ
}MV_HWS_XPCS_SUB_SEQ;

extern MV_CFG_SEQ hwsXpcsPscSeqDb[MV_XPCS_LAST_SEQ];

GT_STATUS hwsXPscSeqInit(void);
void hwsXPcsSeqFree(void);
GT_STATUS hwsXPcsSeqGet(MV_HWS_XPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum);
GT_STATUS hwsXPcsSeqSet(GT_BOOL firstLine, MV_HWS_XPCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum);

#ifdef __cplusplus
}
#endif

#endif /* __mvHwsXPcsDbIf_H */

