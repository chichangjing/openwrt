
/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the \"File\") is owned and distributed by Marvell
* International Ltd. and/or its affiliates (\"Marvell\") under the following
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
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"
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
* wrHwsPcs.cpp
*
* DESCRIPTION:
*       Implementation of the Galtis Wrapper functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

/* General H Files */
#include <common/os/gtOs.h>
#include <common/system/system.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <common/macros/gtCommonFuncs.h>
#include <galtisAgent/wrapUtil/galtisValues.h>
#include <galtisAgent/wrapUtil/cmdUtils.h>
#include <pcs/mvHwsPcsIf.h>
#include <pcs/gPcs/mvHwsGPcsIf.h>
#include <pcs/hglPcs/mvHwsHglPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcsIf.h>
#include <pcs/xPcs/mvHwsXPcsIf.h>
#include <pcs/gPcs/mvHwsGPcsDb.h>
#include <pcs/hglPcs/mvHwsHglPcsDb.h>
#include <pcs/cgPcs/mvHwsCgPcsDb.h>
#include <pcs/ilknPcs/mvHwsIlknPcsDb.h>
#include <pcs/mmPcs/mvHwsMMPcsDb.h>
#include <pcs/mmPcs/mvHwsMMPcsDbRev3.h>
#include <pcs/mmPcs/mvHwsMMPcs28nmDb.h>
#include <pcs/xPcs/mvHwsXPcsDb.h>
#include <common/siliconIf/mvSiliconIf.h>

extern "C" {
GT_STATUS cmdLibInitHwsPcsIf
(
    GT_VOID
);
}

/*******************************************************************************
* wrHwsPcsModeCfgSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPcsModeCfgSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      pcsNum = inFields[0];
    MV_HWS_PORT_PCS_TYPE    pcsType;
    MV_HWS_PORT_FEC_MODE    fecCtrl;
    GT_U32                  numOfLanes;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    switch(inFields[1])
    {
    case 1:
      pcsType = GPCS;
      break;
    case 2:
      pcsType = XPCS;
      break;
    case 3:
      pcsType = MMPCS;
      break;
    case 4:
      pcsType = HGLPCS;
      break;
    default:
          galtisOutput(outArgs, GT_NOT_SUPPORTED, "");
          return CMD_OK;
    }
    switch(inFields[2])
    {
    case 0:
      numOfLanes = 1;
      break;
    case 1:
      numOfLanes = 2;
      break;
    case 2:
      numOfLanes = 4;
      break;
    case 3:
      numOfLanes = 6;
      break;
    }
    if (inFields[3] == 1)
    {
      fecCtrl = FEC_ON;
    }
    else
    {
      fecCtrl = FEC_OFF;
    }

    rc = mvHwsPcsModeCfg(devNum, devNum, pcsNum, pcsType, fecCtrl,numOfLanes);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPcsResetCfgSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPcsResetCfgSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      pcsNum = inFields[0];
    MV_HWS_PORT_PCS_TYPE    pcsType;
    MV_HWS_RESET  action;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }

    switch(inFields[1])
    {
    case 1:
      pcsType = GPCS;
      break;
    case 2:
      pcsType = XPCS;
      break;
    case 3:
      pcsType = MMPCS;
      break;
    case 4:
      pcsType = HGLPCS;
      break;
    default:
          galtisOutput(outArgs, GT_NOT_SUPPORTED, "");
          return CMD_OK;
    }

    if (inFields[2] == 0)
    {
      action = UNRESET;
    }
    else
    {
      action = RESET;
    }
    rc = mvHwsPcsReset(devNum, devNum, pcsNum, pcsType, action);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPcsTestGeneratorCfgSetEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPcsTestGeneratorCfgSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      pcsNum = inFields[0];
    MV_HWS_PORT_TEST_GEN_PATTERN pattern;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }

    switch(inFields[2])
    {
    case 0:
      pattern = TEST_GEN_PRBS7;
      break;
    case 1:
      pattern = TEST_GEN_PRBS23;
      break;
    case 2:
      pattern = TEST_GEN_CJPAT;
      break;
    case 3:
      pattern = TEST_GEN_CRPAT;
      break;
    case 4:
      pattern = TEST_GEN_KRPAT;
      break;
    case 5:
      pattern = TEST_GEN_Normal;
      break;
    default:
          galtisOutput(outArgs, GT_NOT_SUPPORTED, "");
          return CMD_OK;
    }

    switch(inFields[1])
    {
    case 1:
      rc = mvHwsGPcsTestGenCfg(devNum,devNum,pcsNum,pattern);
      break;
    case 2:
      rc = mvHwsXPcsTestGenCfg(devNum,devNum,pcsNum,pattern);
      break;
    case 3:
      rc = mvHwsMMPcsTestGenCfg(devNum,devNum,pcsNum,pattern);
      break;
    case 4:
      rc = mvHwsXPcsTestGenCfg(devNum,devNum,pcsNum,pattern);
      break;
    default:
          galtisOutput(outArgs, GT_NOT_SUPPORTED, "");
          return CMD_OK;
    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}


/*******************************************************************************
* wrHwsPcsTestGeneratorErrorsGetFirstEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPcsTestGeneratorErrorsGetFirstEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      pcsNum = inArgs[2];
    MV_HWS_PORT_TEST_GEN_PATTERN pattern;
    MV_HWS_TEST_GEN_STATUS status;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    switch(inArgs[3])
    {
    case 0:
      pattern = TEST_GEN_PRBS7;
      break;
    case 1:
      pattern = TEST_GEN_PRBS23;
      break;
    case 2:
      pattern = TEST_GEN_CJPAT;
      break;
    case 3:
      pattern = TEST_GEN_CRPAT;
      break;
    case 4:
      pattern = TEST_GEN_KRPAT;
      break;
    case 5:
      pattern = TEST_GEN_Normal;
      break;
    default:
          galtisOutput(outArgs, GT_NOT_SUPPORTED, "");
          return CMD_OK;
    }

    switch(inArgs[1])
    {
    case 1:
      rc = mvHwsGPcsTestGenStatus(devNum, devNum, pcsNum, pattern, &status);
      break;
    case 2:
      rc = mvHwsXPcsTestGenStatus(devNum, devNum, pcsNum, pattern, &status);
      break;
    case 3:
      rc = mvHwsMMPcsTestGenStatus(devNum, devNum, pcsNum, pattern, &status);
      break;
    case 4:
      rc = mvHwsHglPcsTestGenStatus(devNum, devNum, pcsNum, pattern, &status);
      break;
    default:
          galtisOutput(outArgs, GT_NOT_SUPPORTED, "");
          return CMD_OK;
    }
    inFields[1] = (GT_32)status.checkerLock;
    inFields[2] = (GT_32)status.goodFrames;
    inFields[3] = (GT_32)status.totalErrors;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", (GT_32)pcsNum, inFields[1],inFields[2],inFields[3]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPcsTestGeneratorErrorsGetNextEntry
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPcsTestGeneratorErrorsGetNextEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

static GT_U32 seqCurLine;
/*******************************************************************************
* wrHwspcsSeqCfgGetFirst
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPcsSeqCfgGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    MV_POLLING_OP_PARAMS *polParam;
    MV_WRITE_OP_PARAMS   *wrParam;
    MV_DELAY_OP_PARAMS   *delOp;
    MV_CFG_ELEMENT cfgEl;
    GT_U8           devNum; 
    GT_U32          baseAddr, unitIndex;

    seqCurLine = 0;
    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    switch(inArgs[1])
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        /* CG PCS */
      rc = hwsCgPcsSeqGet((MV_HWS_CGPCS_SUB_SEQ)(inArgs[1]), &cfgEl, 0);
      break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      /* GE PCS */
      rc = hwsGPcsSeqGet((MV_HWS_GPCS_SUB_SEQ)(inArgs[1]-7), &cfgEl, 0);
      break;
    case 13:
    case 14:
    case 15:
      /* HGL PCS */
      rc = hwsHglPcsSeqGet((MV_HWS_HGLPCS_SUB_SEQ)(inArgs[1]-13), &cfgEl, 0);
      break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
      /* ILKN PCS */
      rc = hwsIlknPcsSeqGet((MV_HWS_ILKNPCS_SUB_SEQ)(inArgs[1]-16), &cfgEl, 0);
      break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
        /* MMPCS */
        rc = hwsMMPcsSeqGet((MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]-22), &cfgEl, 0);
        break;
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
      /* XG PCS */
      rc = hwsXPcsSeqGet((MV_HWS_XPCS_SUB_SEQ)(inArgs[1]-32), &cfgEl, 0);
      break;
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
		if(HWS_DEV_SILICON_TYPE(devNum) == Puma3B0)
		{
			/* MMPCS Rev3 sequences */
			rc = hwsMMPcsRev3SeqGet((MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]- 46 + 13), &cfgEl, 0);
		}
		else
		{
			/* MMPCS 28nm sequences */
			rc = hwsMMPcs28nmSeqGet((MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]- 46 + 13), &cfgEl, 0);
		}
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    devNum = inArgs[0];
    inFields[0] = 0;
    inFields[1] = cfgEl.op;
    switch(inFields[1])
    {
    case WRITE_OP:
      wrParam = (MV_WRITE_OP_PARAMS *)cfgEl.params;
      mvUnitInfoGet(devNum, (MV_HWS_UNITS_ID)wrParam->indexOffset, &baseAddr, &unitIndex);
      inFields[2] = baseAddr;
      inFields[3] = wrParam->regOffset;
      inFields[4] = wrParam->data;
      inFields[5] = wrParam->mask;
      inFields[6] = 0;
      inFields[7] = 0;
      break;
    case DELAY_OP:
      delOp = (MV_DELAY_OP_PARAMS *)cfgEl.params;
      inFields[2] = 0;
      inFields[3] = 0;
      inFields[4] = 0;
      inFields[5] = 0;
      inFields[6] = delOp->delay;
      inFields[7] = 0;
      break;
    case POLLING_OP:
      polParam = (MV_POLLING_OP_PARAMS *)cfgEl.params;
      mvUnitInfoGet(devNum, (MV_HWS_UNITS_ID)polParam->indexOffset, &baseAddr, &unitIndex);
      inFields[2] = baseAddr;
      inFields[3] = polParam->regOffset;
      inFields[4] = polParam->data;
      inFields[5] = polParam->mask;
      inFields[6] = polParam->waitTime;
      inFields[7] = polParam->numOfLoops;
      break;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d",inFields[0],inFields[1],inFields[2],inFields[3],inFields[4],inFields[5],inFields[6],inFields[7]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsPcsSeqCfgGetNext
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPcsSeqCfgGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    MV_POLLING_OP_PARAMS *polParam;
    MV_WRITE_OP_PARAMS   *wrParam;
    MV_DELAY_OP_PARAMS   *delOp;
    MV_CFG_ELEMENT cfgEl;
    GT_U8           devNum; 
    GT_U32          baseAddr, unitIndex;

    inFields[0] = ++seqCurLine;
    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    switch(inArgs[1])
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        /* CG PCS */
      rc = hwsCgPcsSeqGet((MV_HWS_CGPCS_SUB_SEQ)(inArgs[1]), &cfgEl, seqCurLine);
      break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      /* GE PCS */
      rc = hwsGPcsSeqGet((MV_HWS_GPCS_SUB_SEQ)(inArgs[1]-7), &cfgEl, seqCurLine);
      break;
    case 13:
    case 14:
    case 15:
      /* HGL PCS */
      rc = hwsHglPcsSeqGet((MV_HWS_HGLPCS_SUB_SEQ)(inArgs[1]-13), &cfgEl, seqCurLine);
      break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
      /* ILKN PCS */
      rc = hwsIlknPcsSeqGet((MV_HWS_ILKNPCS_SUB_SEQ)(inArgs[1]-16), &cfgEl, seqCurLine);
      break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
        /* MMPCS */
        rc = hwsMMPcsSeqGet((MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]-22), &cfgEl, seqCurLine);
        break;
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
      /* XG PCS */
      rc = hwsXPcsSeqGet((MV_HWS_XPCS_SUB_SEQ)(inArgs[1]-32), &cfgEl, seqCurLine);
      break;
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
		if(HWS_DEV_SILICON_TYPE(devNum) == Puma3B0)
		{
			/* MMPCS Rev3 sequences */
			rc = hwsMMPcsRev3SeqGet((MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]- 46 + 13), &cfgEl, seqCurLine);
		}
		else
		{
			/* MMPCS 28nm sequences */
			rc = hwsMMPcs28nmSeqGet((MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]- 46 + 13), &cfgEl, seqCurLine);
		}
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    if (rc == GT_NO_MORE)
    {
      galtisOutput(outArgs, GT_OK, "%d", -1);
      return CMD_OK;
    }

    devNum = inArgs[0];
    inFields[1] = cfgEl.op;
    switch(inFields[1])
    {
    case WRITE_OP:
      wrParam = (MV_WRITE_OP_PARAMS *)cfgEl.params;
      mvUnitInfoGet(devNum, (MV_HWS_UNITS_ID)wrParam->indexOffset, &baseAddr, &unitIndex);
      inFields[2] = baseAddr;
      inFields[3] = wrParam->regOffset;
      inFields[4] = wrParam->data;
      inFields[5] = wrParam->mask;
      inFields[6] = 0;
      inFields[7] = 0;
      break;
    case DELAY_OP:
      delOp = (MV_DELAY_OP_PARAMS *)cfgEl.params;
      inFields[2] = 0;
      inFields[3] = 0;
      inFields[4] = 0;
      inFields[5] = 0;
      inFields[6] = delOp->delay;
      inFields[7] = 0;
      break;
    case POLLING_OP:
      polParam = (MV_POLLING_OP_PARAMS *)cfgEl.params;
      mvUnitInfoGet(devNum, (MV_HWS_UNITS_ID)polParam->indexOffset, &baseAddr, &unitIndex);
      inFields[2] = baseAddr;
      inFields[3] = polParam->regOffset;
      inFields[4] = polParam->data;
      inFields[5] = polParam->mask;
      inFields[6] = polParam->waitTime;
      inFields[7] = polParam->numOfLoops;
      break;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d",inFields[0],inFields[1],inFields[2],inFields[3],inFields[4],inFields[5],inFields[6],inFields[7]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;

}

/*******************************************************************************
* wrHwsPcsSeqCfgSet
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsPcsSeqCfgSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    MV_POLLING_OP_PARAMS polParam;
    MV_WRITE_OP_PARAMS   wrParam;
    MV_DELAY_OP_PARAMS   delOp;
    MV_CFG_ELEMENT       cfgEl;
    GT_U8           devNum; 
    MV_HWS_UNITS_ID unitId;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }

    devNum = inArgs[0];
    switch(inFields[1])
    {
    case WRITE_OP:
      mvUnitInfoGetByAddr(devNum, (GT_U32)inFields[2], &unitId);
      wrParam.indexOffset = unitId;
      wrParam.regOffset = inFields[3];
      wrParam.data = inFields[4];
      wrParam.mask = inFields[5];
      cfgEl.params = &wrParam;
      cfgEl.op = WRITE_OP;
      break;
    case DELAY_OP:
      delOp.delay = inFields[6];
      cfgEl.params = &delOp;
      cfgEl.op = DELAY_OP;
      break;
    case POLLING_OP:
      mvUnitInfoGetByAddr(devNum, (GT_U32)inFields[2], &unitId);
      polParam.indexOffset = unitId;
      polParam.regOffset = inFields[3];
      polParam.data = inFields[4];
      polParam.mask = inFields[5];
      polParam.waitTime = inFields[6];
      polParam.numOfLoops = inFields[7];
      cfgEl.params = &polParam;
      cfgEl.op = POLLING_OP;
      break;
    }

    switch(inArgs[1])
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        /* CG PCS */
      rc = hwsCgPcsSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
          (MV_HWS_CGPCS_SUB_SEQ)(inArgs[1]), &cfgEl, inFields[0]);
      break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      /* GE PCS */
      rc = hwsGPcsSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_GPCS_SUB_SEQ)(inArgs[1]-7), &cfgEl, inFields[0]);
      break;
    case 13:
    case 14:
    case 15:
      /* HGL PCS */
      rc = hwsHglPcsSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_HGLPCS_SUB_SEQ)(inArgs[1]-13), &cfgEl, inFields[0]);
      break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
      /* ILKN PCS */
      rc = hwsIlknPcsSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_ILKNPCS_SUB_SEQ)(inArgs[1]-16), &cfgEl, inFields[0]);
      break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
        /* MMPCS */
      rc = hwsMMPcsSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]-22), &cfgEl, inFields[0]);
        break;
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
      /* XG PCS */
      rc = hwsXPcsSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_XPCS_SUB_SEQ)(inArgs[1]-32), &cfgEl, inFields[0]);
      break;
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
		if(HWS_DEV_SILICON_TYPE(devNum) == Puma3B0)
		{
			/* MMPCS Rev3 sequences */
			rc = hwsMMPcsRev3SeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
									(MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]- 46 + 13), &cfgEl, inFields[0]);
		}
		else
		{
			/* MMPCS 28nm sequences */
			rc = hwsMMPcs28nmSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
									(MV_HWS_MM_PCS_SUB_SEQ)(inArgs[1]- 46 + 13), &cfgEl, inFields[0]);
		}
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;

}

/*******************************************************************************
*
* dbCommands - Galtis database galtis commands
*
*******************************************************************************/
static CMD_COMMAND dbCommands[] = {
{"HwsPcsModeCfgSet", &wrHwsPcsModeCfgSetEntry, 1,  4},
{"HwsPcsResetCfgSet", &wrHwsPcsResetCfgSetEntry, 1,  3},
{"HwsPcsTestGeneratorCfgSet", &wrHwsPcsTestGeneratorCfgSetEntry, 1,  3},
{"HwsPcsTestGeneratorErrorsGetFirst", &wrHwsPcsTestGeneratorErrorsGetFirstEntry, 4,  0},
{"HwsPcsTestGeneratorErrorsGetNext", &wrHwsPcsTestGeneratorErrorsGetNextEntry, 4,  0},
{"HwsPcsSeqCfgGetFirst", &wrHwsPcsSeqCfgGetFirst, 2,  0},
{"HwsPcsSeqCfgGetNext", &wrHwsPcsSeqCfgGetNext, 2,  0},
{"HwsPcsSeqCfgSet", &wrHwsPcsSeqCfgSet, 2,  8},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitHwsPcsIf
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitHwsPcsIf()
{
    return cmdInitLibrary(dbCommands, numCommands);
}

