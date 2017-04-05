
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
* wrHwsMac.cpp
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
#include <mac/mvHwsMacIf.h>
#include <mac/geMac/mvHwsGeMacDb.h>
#include <mac/hglMac/mvHwsHglMacDb.h>
#include <mac/xgMac/mvHwsXgMacDb.h>
#include <mac/xlgMac/mvHwsXlgMacDb.h>
#include <mac/cgMac/mvHwsCgMacDb.h>
#include <mac/ilknMac/mvHwsIlknMacDb.h>
#include <private/mvHwsPortPrvIf.h>

extern "C" {
GT_STATUS cmdLibInitHwsMacIf
(
    GT_VOID
);
}

/*******************************************************************************
* wrHwsMacModeCfgSetEntry
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
CMD_STATUS wrHwsMacModeCfgSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      macNum = inFields[0];
    MV_HWS_PORT_MAC_TYPE    macType;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }
    switch(inFields[1])
    {
    case 0:
      macType = GEMAC_X;
      break;
    case 1:
      macType = GEMAC_SG;
      break;
    case 2:
      macType = XLGMAC;
      break;
    case 3:
      macType = XGMAC;
      break;
    case 4:
      macType = HGLMAC;
      break;
    case 5:
      macType = CGMAC;
      break;
    case 6:
      macType = INTLKN_MAC;
      break;
    case 7:
      macType = TCAM_MAC;
      break;
    default:
        galtisOutput(outArgs, CMD_SYNTAX_ERROR, "");
        return CMD_OK;
    }

    rc = mvHwsMacModeCfg(devNum, devNum, macNum, macType, (GT_U32)inFields[2]);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsMacResetCfgSetEntry
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
CMD_STATUS wrHwsMacResetCfgSetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;         /* Return code  */
    GT_U8       devNum = inArgs[0];
    GT_U32      macNum = inFields[0];
    MV_HWS_PORT_MAC_TYPE    macType;
    MV_HWS_RESET  action;

    /*check for valid arguments */
    if(!inArgs || !outArgs || !inFields)
    {
        return CMD_AGENT_ERROR;
    }

    switch(inFields[1])
    {
    case 0:
      macType = GEMAC_X;
      break;
    case 1:
      macType = GEMAC_SG;
      break;
    case 2:
      macType = XLGMAC;
      break;
    case 3:
      macType = XGMAC;
      break;
    case 4:
      macType = HGLMAC;
      break;
    case 5:
      macType = CGMAC;
      break;
    case 6:
      macType = INTLKN_MAC;
      break;
    case 7:
      macType = TCAM_MAC;
      break;
    default:
        galtisOutput(outArgs, CMD_SYNTAX_ERROR, "");
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
    rc = mvHwsMacReset(devNum, devNum, macNum, macType, action);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

static GT_U32 seqCurLine;
/*******************************************************************************
* wrHwsMacSeqCfgGetFirst
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
CMD_STATUS wrHwsMacSeqCfgGetFirst
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
    devNum = inArgs[0];
    switch(inArgs[1])
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      /* GE MAC */
      rc = hwsGeMacSeqGet((MV_HWS_GE_MAC_SUB_SEQ)inArgs[1], &cfgEl, 0);
      break;
    case 7:
      /* HGL MAC */
      rc = hwsHglMacSeqGet((MV_HWS_HGL_MAC_SUB_SEQ)(inArgs[1]-7), &cfgEl, 0);
      break;
    case 8:
      /* XG MAC */
      rc = hwsXgMacSeqGet((MV_HWS_XG_MAC_SUB_SEQ)(inArgs[1]-8), &cfgEl, 0);
      break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
      /* XLG MAC */
      rc = hwsXlgMacSeqGet((MV_HWS_XLG_MAC_SUB_SEQ)(inArgs[1]-9), &cfgEl, 0);
      break;
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
      /* CG MAC */
      rc = hwsCgMacSeqGet((MV_HWS_CG_MAC_SUB_SEQ)(inArgs[1]-17), &cfgEl, 0);
      break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
      /* INLK MAC */
      rc = hwsIlknMacSeqGet((MV_HWS_INTKLN_MAC_SUB_SEQ)(inArgs[1]-22), &cfgEl, 0);
      break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

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
* wrHwsMacSeqCfgGetNext
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
CMD_STATUS wrHwsMacSeqCfgGetNext
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
      /* GE MAC */
      rc = hwsGeMacSeqGet((MV_HWS_GE_MAC_SUB_SEQ)inArgs[1], &cfgEl, seqCurLine);
      break;
    case 7:
      /* HGL MAC */
      rc = hwsHglMacSeqGet((MV_HWS_HGL_MAC_SUB_SEQ)(inArgs[1]-7), &cfgEl, seqCurLine);
      break;
    case 8:
      /* XG MAC */
      rc = hwsXgMacSeqGet((MV_HWS_XG_MAC_SUB_SEQ)(inArgs[1]-8), &cfgEl, seqCurLine);
      break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
      /* XLG MAC */
      rc = hwsXlgMacSeqGet((MV_HWS_XLG_MAC_SUB_SEQ)(inArgs[1]-9), &cfgEl, seqCurLine);
      break;
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
      /* CG MAC */
      rc = hwsCgMacSeqGet((MV_HWS_CG_MAC_SUB_SEQ)(inArgs[1]-17), &cfgEl, seqCurLine);
      break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
      /* INLK MAC */
      rc = hwsIlknMacSeqGet((MV_HWS_INTKLN_MAC_SUB_SEQ)(inArgs[1]-22), &cfgEl, seqCurLine);
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
* wrHwsMacSeqCfgSet
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
CMD_STATUS wrHwsMacSeqCfgSet
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
    GT_U8                devNum;
    MV_HWS_UNITS_ID      unitId;

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
      /* GE MAC */
      rc = hwsGeMacSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_GE_MAC_SUB_SEQ)inArgs[1], &cfgEl, inFields[0]);
      break;
    case 7:
      /* HGL MAC */
      rc = hwsHglMacSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_HGL_MAC_SUB_SEQ)(inArgs[1]-7), &cfgEl, inFields[0]);
      break;
    case 8:
      /* XG MAC */
      rc = hwsXgMacSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_XG_MAC_SUB_SEQ)(inArgs[1]-8), &cfgEl, inFields[0]);
      break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
      /* XLG MAC */
      rc = hwsXlgMacSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_XLG_MAC_SUB_SEQ)(inArgs[1]-9), &cfgEl, inFields[0]);
      break;
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
      /* CG MAC */
      rc = hwsCgMacSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_CG_MAC_SUB_SEQ)(inArgs[1]-17), &cfgEl, inFields[0]);
      break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
      /* INLK MAC */
      rc = hwsIlknMacSeqSet(((inFields[0] == 0) ? GT_TRUE : GT_FALSE),
        (MV_HWS_INTKLN_MAC_SUB_SEQ)(inArgs[1]-22), &cfgEl, inFields[0]);
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
{"HwsMacModeCfgSet", &wrHwsMacModeCfgSetEntry, 1,  3},
{"HwsMacResetCfgSet", &wrHwsMacResetCfgSetEntry, 1,  3},
{"HwsMacSeqCfgGetFirst", &wrHwsMacSeqCfgGetFirst, 2,  0},
{"HwsMacSeqCfgGetNext", &wrHwsMacSeqCfgGetNext, 2,  0},
{"HwsMacSeqCfgSet", &wrHwsMacSeqCfgSet, 2,  8},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitHwsMacIf
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
GT_STATUS cmdLibInitHwsMacIf()
{
    return cmdInitLibrary(dbCommands, numCommands);
}

