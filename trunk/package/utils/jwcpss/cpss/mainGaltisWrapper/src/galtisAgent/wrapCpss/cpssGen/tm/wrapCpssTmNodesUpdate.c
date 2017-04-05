/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssTmNodesUpdate.c
*
* DESCRIPTION:
*       Wrapper functions for TM configuration library control interface.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/tm/cpssTmCtl.h>
#include <cpss/generic/tm/cpssTmNodesUpdate.h>

/*******************************************************************************
* cpssTmQueueUpdate
*
* DESCRIPTION:
*       Update queue parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Queue index.
*       paramsPtr       - Queue parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'dropProfileInd' parameter will be updated in any case,
*       set it's value to be the same as in DB if you don't want to change it.
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32   index;
    CPSS_TM_QUEUE_PARAMS_STC     params;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    params.shapingProfilePtr = (GT_U32)inArgs[2];
    params.quantum = (GT_U32)inArgs[3];
    params.dropProfileInd = (GT_U32)inArgs[4];
    params.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[5];

    /* call cpss api function */
    result = cpssTmQueueUpdate(devNum, index, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssTmAnodeUpdate
*
* DESCRIPTION:
*       Update A-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*       3. 'dropProfileInd' parameter will be updated in any case,
*       set it's value to be the same as in DB if you don't want to change it.
*******************************************************************************/
static CMD_STATUS wrCpssTmAnodeUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32   index;
    CPSS_TM_A_NODE_PARAMS_STC     params;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    params.shapingProfilePtr = (GT_U32)inArgs[2];
    params.quantum = (GT_U32)inArgs[3];
    for(ii=0; ii<8; ii++)
        params.schdModeArr[ii] = (GT_U32)inArgs[4+ii];
    params.dropProfileInd = (GT_U32)inArgs[12];
    params.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[13];
    params.numOfChildren = (GT_U32)inArgs[14];

    /* call cpss api function */
    result = cpssTmAnodeUpdate(devNum, index, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssTmBnodeUpdate
*
* DESCRIPTION:
*       Update B-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*******************************************************************************/
static CMD_STATUS wrCpssTmBnodeUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32   index;
    CPSS_TM_B_NODE_PARAMS_STC     params;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    params.shapingProfilePtr = (GT_U32)inArgs[2];
    params.quantum = (GT_U32)inArgs[3];
    for(ii=0; ii<8; ii++)
        params.schdModeArr[ii] = (GT_U32)inArgs[4+ii];
    params.dropProfileInd = (GT_U32)inArgs[12];
    params.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[13];
    params.numOfChildren = (GT_U32)inArgs[14];

    /* call cpss api function */
    result = cpssTmBnodeUpdate(devNum, index, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssTmCnodeUpdate
*
* DESCRIPTION:
*       Update C-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*******************************************************************************/
static CMD_STATUS wrCpssTmCnodeUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32   index;
    CPSS_TM_C_NODE_PARAMS_STC     params;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    params.shapingProfilePtr = (GT_U32)inArgs[2];
    params.quantum = (GT_U32)inArgs[3];
    for(ii=0; ii<8; ii++)
        params.schdModeArr[ii] = (GT_U32)inArgs[4+ii];
    params.dropCosMap = (GT_U32)inArgs[12];
    for(ii=0; ii<8; ii++)
        params.dropProfileIndArr[ii] = (GT_U32)inArgs[13+ii];
    params.eligiblePrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[21];
    params.numOfChildren = (GT_U32)inArgs[22];

    /* call cpss api function */
    result = cpssTmCnodeUpdate(devNum, index, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssTmPortShapingUpdate
*
* DESCRIPTION:
*       Update Port Shaping parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       cirBw           - Port CIR bandwidth.
*       eirBw           - Port EIR bandwidth.
*       cbs             - Port CIR burst size.
*       ebs             - Port EIR burst size.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_BAD_SIZE              - on Port's min/max token too large.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortShapingUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8    devNum;
    GT_U32   index;
    GT_U32   cirBw;
    GT_U32   eirBw;
    GT_U32   cbs;
    GT_U32   ebs;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    cirBw = (GT_U32)inArgs[2];
    eirBw = (GT_U32)inArgs[3];
    cbs = (GT_U32)inArgs[4];
    ebs = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssTmPortShapingUpdate(devNum, index, cirBw, eirBw, cbs, ebs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmPortSchedulingUpdate
*
* DESCRIPTION:
*       Update Port Scheduling parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       eligPrioFuncId  - Eligible Priority Function pointer.
*       quantumArrPtr   - Port quantum 8 cell array.
*       schdModeArr     - Port RR/DWRR priority pointer for C-level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortSchedulingUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index;
    GT_U32    eligPrioFuncId;
    GT_U32    quantumArrPtr[8]; /* 8 cells array */
    CPSS_TM_SCHD_MODE_ENT   schdModeArr[8];/* 8 cells array */
    GT_U32    ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    for(ii=0; ii<8; ii++)
        quantumArrPtr[ii] = (GT_U16)inArgs[2+ii];
    for(ii=0; ii<8; ii++)
        schdModeArr[ii] = (GT_BOOL)inArgs[10+ii];

    eligPrioFuncId = (CPSS_TM_ELIG_FUNC_NODE_ENT)inArgs[18];


    /* call cpss api function */
    result = cpssTmPortSchedulingUpdate(devNum, index, eligPrioFuncId, quantumArrPtr, schdModeArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssTmPortDropUpdate
*
* DESCRIPTION:
*       Update Port Drop parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       wredProfileRef  - Port Drop Profile reference.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortDropUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index;
    GT_U32    wredProfileRef;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    wredProfileRef = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssTmPortDropUpdate(devNum, index, wredProfileRef);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmPortDropCosUpdate
*
* DESCRIPTION:
*       Update Port Drop per Cos parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       paramsPtr       - Port Drop per Cos parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortDropCosUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index;
    CPSS_TM_PORT_DROP_PER_COS_STC  params;
    GT_U32    ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    params.dropCosMap = (GT_U32)inArgs[2];
    for(ii=0; ii<8; ii++)
        params.dropProfileIndArr[ii] = (GT_32)inArgs[3+ii];

    /* call cpss api function */
    result = cpssTmPortDropCosUpdate(devNum, index, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssTmQueueUpdate",
        &wrCpssTmQueueUpdate,
        6, 0},
    {"cpssTmAnodeUpdate",
        &wrCpssTmAnodeUpdate,
        15, 0},
    {"cpssTmBnodeUpdate",
        &wrCpssTmBnodeUpdate,
        15, 0},
    {"cpssTmCnodeUpdate",
        &wrCpssTmCnodeUpdate,
        23, 0},
    {"cpssTmPortShapingUpdate",
        &wrCpssTmPortShapingUpdate,
        6, 0},
    {"cpssTmPortSchedulingUpdate",
        &wrCpssTmPortSchedulingUpdate,
        19, 0},
    {"cpssTmPortDropUpdate",
        &wrCpssTmPortDropUpdate,
        3, 0},
    {"cpssTmPortDropCosUpdate",
        &wrCpssTmPortDropCosUpdate,
        11, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmNodesUpdate
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
GT_STATUS cmdLibInitCpssTmNodesUpdate
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

