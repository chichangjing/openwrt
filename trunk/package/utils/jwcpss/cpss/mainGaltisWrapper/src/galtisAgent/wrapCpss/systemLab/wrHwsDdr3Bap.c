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
* wrHwsDdr3Bap.cpp
*
* DESCRIPTION:
*       Implementation of the Galtis Wrapper functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4.*
*******************************************************************************/

/* Common for all HWS Galtis wrappers */
#include <galtisAgent/wrapCpss/systemLab/wrHwsCommon.h>

/* General H Files */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <mvHwsDdr3Bap.h>

GT_STATUS    ddr3BapInitAllController
( 
    GT_U32          devNum
);

static GT_U32 accessSize;
static GT_U32 accessBaseAddress;

/*******************************************************************************
* wrHwsDdr3BapInitController
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrHwsDdr3BapInitController
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];


    res = ddr3BapInitAllController(DevNum);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3BapAlgoRun
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrHwsDdr3BapAlgoRun
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U8  DevNum   = (GT_U8)inArgs[0];
    GT_U32  AlgoType = (GT_U32)inArgs[1];

	MV_HWS_BAP_ALGO_PARAMS bapAlgoParams;

	bapAlgoParams.algoFlowMask = 0;
	bapAlgoParams.algoType = (MV_HWS_ALGO_TYPE)AlgoType;

    res = mvHwsDdr3BapRunAlg(DevNum, &bapAlgoParams);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3BapRegAccessSet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrHwsDdr3BapRegAccessSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U8   DevNum     = (GT_U8)inArgs[0];
    GT_U32  BapID      = (GT_U32)inArgs[1];
    GT_U32  RegAddress = (GT_U32)inArgs[2];
    GT_U32  MaskHigh   = (GT_U32)inArgs[3];
    GT_U32  MaskLow    = (GT_U32)inArgs[4];

    /* Fields List */
    GT_U32  DataHigh = (GT_U32)inFields[0];
    GT_U32  DataLow  = (GT_U32)inFields[1];

    if (MaskLow == 0)
    {
        MaskLow = 0xffffffff;
    }
    
    if (MaskHigh == 0)
    {
        MaskHigh = 0xffffffff;
    }
    
    res = mvHwsDdr3BapRegWrite(DevNum, BapID, RegAddress, DataLow, DataHigh, MaskLow, MaskHigh);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3BapRegAccessGetFirst
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrHwsDdr3BapRegAccessGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U8   DevNum     = (GT_U8)inArgs[0];
    GT_U32  BapID      = (GT_U32)inArgs[1];
    GT_U32  RegAddress = (GT_U32)inArgs[2];
    GT_U32  MaskHigh   = (GT_U32)inArgs[3];
    GT_U32  MaskLow    = (GT_U32)inArgs[4];

    GT_U32  DataLow, DataHigh;

    if (MaskLow == 0)
    {
        MaskLow = 0xffffffff;
    }

    if (MaskHigh == 0)
    {
        MaskHigh = 0xffffffff;
    }

    res = mvHwsDdr3BapRegRead(DevNum, BapID, RegAddress, &DataLow, &DataHigh, MaskLow, MaskHigh);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    /* Fields List */
    inFields[0] = DataHigh;
    inFields[1] = DataLow;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3BapRegAccessGetNext
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrHwsDdr3BapRegAccessGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3BapExternalMemAccessSet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrHwsDdr3BapExternalMemAccessSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U8   DevNum      = (GT_U8)inArgs[0];
    GT_U32  BapID       = (GT_U32)inArgs[1];
    GT_U32  BaseAddress = (GT_U32)inArgs[2];
    GT_U32  Size        = (GT_U32)inArgs[3];

    /* Fields List */
    GT_U32 wordArr[4];

    /* Warnings */
    Size = Size;

    if (BaseAddress % 16 != 0)
    {
        cpssOsPrintf("Base address must be aligned to 16 bytes\n");
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    
    wordArr[0]       = (GT_U32)inFields[1];
    wordArr[1]       = (GT_U32)inFields[2];
    wordArr[2]       = (GT_U32)inFields[3];
    wordArr[3]       = (GT_U32)inFields[4];

    res = mvHwsDdr3BapMemWrite(DevNum, BapID, BaseAddress, wordArr);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3BapExternalMemAccessGetFirst
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrHwsDdr3BapExternalMemAccessGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U8   DevNum      = (GT_U8)inArgs[0];
    GT_U32  BapID       = (GT_U32)inArgs[1];
    GT_U32  BaseAddress = (GT_U32)inArgs[2];
    GT_U32  Size        = (GT_U32)inArgs[3];

    GT_U32 wordArr[4];

    if (BaseAddress % 16 != 0)
    {
        cpssOsPrintf("Base address must be aligned to 16 bytes\n");
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    
    accessSize = Size;
    accessBaseAddress = BaseAddress;
    cpssOsMemSet(wordArr, 0, sizeof(wordArr));
    
    
    res = mvHwsDdr3BapMemRead(DevNum, BapID, BaseAddress, wordArr);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    accessSize = (accessSize < 16) ? 0 : accessSize - 16;
    cpssOsPrintf("accessSize is %d\n", accessSize);
    accessBaseAddress += 16;

    /* Fields List */
    inFields[0] = (GT_32)BaseAddress;
    inFields[1] = wordArr[0];
    inFields[2] = wordArr[1];
    inFields[3] = wordArr[2];
    inFields[4] = wordArr[3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3BapExternalMemAccessGetNext
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrHwsDdr3BapExternalMemAccessGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U8   DevNum      = (GT_U8)inArgs[0];
    GT_U32  BapID       = (GT_U32)inArgs[1];

    GT_U32 wordArr[4];
    
    if (accessSize == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    
    cpssOsMemSet(wordArr, 0, sizeof(wordArr));
    
    res = mvHwsDdr3BapMemRead(DevNum, BapID, accessBaseAddress, wordArr);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }
    
    /* Fields List */
    inFields[0] = (GT_32)accessBaseAddress;
    inFields[1] = wordArr[0];
    inFields[2] = wordArr[1];
    inFields[3] = wordArr[2];
    inFields[4] = wordArr[3];

    accessSize = (accessSize < 16) ? 0 : accessSize - 16;
    cpssOsPrintf("accessSize is %d\n", accessSize);
    accessBaseAddress += 16;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
*
* dbCommands - Galtis database galtis commands
*
*******************************************************************************/
static CMD_COMMAND dbCommands[] = {
{"HwsDdr3BapInitController", &wrHwsDdr3BapInitController, 1, 0},
{"HwsDdr3BapAlgoRun", &wrHwsDdr3BapAlgoRun, 2, 0},
{"HwsDdr3BapRegAccessSet", &wrHwsDdr3BapRegAccessSet, 5, 2},
{"HwsDdr3BapRegAccessGetFirst", &wrHwsDdr3BapRegAccessGetFirst, 5, 0},
{"HwsDdr3BapRegAccessGetNext", &wrHwsDdr3BapRegAccessGetNext, 5, 0},
{"HwsDdr3BapExternalMemAccessSet", &wrHwsDdr3BapExternalMemAccessSet, 4, 5},
{"HwsDdr3BapExternalMemAccessGetFirst", &wrHwsDdr3BapExternalMemAccessGetFirst, 4, 0},
{"HwsDdr3BapExternalMemAccessGetNext", &wrHwsDdr3BapExternalMemAccessGetNext, 4, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitDdr3Bap
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
GT_STATUS cmdLibInitDdr3Bap()
{
    return cmdInitLibrary(dbCommands, numCommands);
}
