/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssTmNodesCtl.c
*
* DESCRIPTION:
*       Wrapper functions for TM Shaping APIs.
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
#include <cpss/generic/tm/cpssTmNodesCtl.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>

/*******************************************************************************
* cpssTmNodesCtlQueueInstall
*
* DESCRIPTION:
*       Install queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Makes a queue capable of getting traffic.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlQueueInstall
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      queueInd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueInd = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmNodesCtlQueueInstall(devNum, queueInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmNodesCtlQueueUninstall
*
* DESCRIPTION:
*       Uninstall queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       - Makes a queue incapable of getting traffic. Queue must be not in use!
*       - User should flush Q before this call.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlQueueUninstall
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      queueInd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueInd = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmNodesCtlQueueUninstall(devNum, queueInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssTmNodesCtlQueueInstallStatusGet
*
* DESCRIPTION:
*       REtrieves install status if the queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       status            - (Pointer to) queue install status (APPLICABLE RANGES: 0..1).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       - Makes a queue incapable of getting traffic. Queue must be not in use!
*       - User should flush Q before this call.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlQueueInstallStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      queueInd;
    GT_U32       status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueInd = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmNodesCtlQueueInstallStatusGet(devNum, queueInd, &status);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", status);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmNodesCtlNodeDelete
*
* DESCRIPTION:
*       Delete node from scheduling hierarchy.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       level             - Scheduling level: Queue/A/B/C-node/Port.
*       index             - Queue/node index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_STATE             - on not free node's subtree.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlNodeDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_TM_LEVEL_ENT    level;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    level = (CPSS_TM_LEVEL_ENT)inArgs[1];
    index = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssTmNodesCtlNodeDelete(devNum,level, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssTmNodesCtlTransPortDelete
*
* DESCRIPTION:
*       Delete port and all its subtree from scheduling hierarchy.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlTransPortDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmNodesCtlTransPortDelete(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmQueueFlush
*
* DESCRIPTION:
*       Flush queue by a given index. All packets dequeued from this queue are dropped.
*       Wait till flush queue has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush queue blocking API.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlQueueFlush
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmQueueFlush(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmQueueFlushTriggerActionStart
*
* DESCRIPTION:
*       Activate Flush queue by a given index.
*       All packets dequeued from this queue are dropped.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush queue non blocking API.
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueFlushTriggerActionStart
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmQueueFlushTriggerActionStart(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmQueueFlushTriggerActionStatusGet
*
* DESCRIPTION:
*       Get whether Flush queue has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       actFinishedPtr  - GT_FALSE - flush is active (packets are dropped on dequeue).
*                       - GT_TRUE  - flush off /completed.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueFlushTriggerActionStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;
    GT_BOOL     actFinished;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmQueueFlushTriggerActionStatusGet(devNum, index, &actFinished);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", actFinished);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmPortFlush
*
* DESCRIPTION:
*       Flush port by a given index. All packets dequeued from this port (and all its queues)
*       are dropped. Wait till flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush port blocking API.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlPortFlush
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmPortFlush(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmPortFlushTriggerActionStart
*
* DESCRIPTION:
*       Flush port by a given index. All packets dequeued from this port (and all its queues)
*       are dropped.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush port non blocking API.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortFlushTriggerActionStart
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmPortFlushTriggerActionStart(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmPortFlushTriggerActionStatusGet
*
* DESCRIPTION:
*       Get whether Flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       actFinishedPtr  - GT_FALSE - flush is active (packets are dropped on dequeue).
*                       - GT_TRUE  - flush off /completed.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortFlushTriggerActionStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;
    GT_BOOL     actFinished;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmPortFlushTriggerActionStatusGet(devNum, index, &actFinished);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", actFinished);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmNodesCtlReadNextChange
*
* DESCRIPTION:
*       Read next tree index/range change after reshuffling.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       changePtr         - Change structure pointer.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_NO_RESOURCE           - on empty list.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlReadNextChange
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_TM_TREE_CHANGE_STC      change;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssTmNodesCtlReadNextChange(devNum, &change);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", change.index, change.oldIndex, change.newIndex);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmNodesCtlCleanList
*
* DESCRIPTION:
*       Empty list of reshuffling changes.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmNodesCtlCleanList
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssTmNodesCtlCleanList(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
      {"cpssTmNodesCtlQueueInstall",
        &wrCpssTmNodesCtlQueueInstall,
        2, 0},
      {"cpssTmNodesCtlQueueUninstall",
        &wrCpssTmNodesCtlQueueUninstall,
        2, 0},
      {"cpssTmNodesCtlQueueInstallStatusGet",
        &wrCpssTmNodesCtlQueueInstallStatusGet,
        2, 0},
      {"cpssTmNodesCtlNodeDelete",
        &wrCpssTmNodesCtlNodeDelete,
        3, 0},
      {"cpssTmNodesCtlTransPortDelete",
        &wrCpssTmNodesCtlTransPortDelete,
        2, 0},
      {"cpssTmQueueFlush",
        &wrCpssTmNodesCtlQueueFlush,
        2, 0},
      {"cpssTmQueueFlushTriggerActionStart",
        &wrCpssTmQueueFlushTriggerActionStart,
        2, 0},
      {"cpssTmQueueFlushTriggerActionStatusGet",
        &wrCpssTmQueueFlushTriggerActionStatusGet,
        2, 0},
      {"cpssTmPortFlush",
        &wrCpssTmNodesCtlPortFlush,
        2, 0},
      {"cpssTmPortFlushTriggerActionStart",
        &wrCpssTmPortFlushTriggerActionStart,
        2, 0},
      {"cpssTmPortFlushTriggerActionStatusGet",
        &wrCpssTmPortFlushTriggerActionStatusGet,
        2, 0},
      {"cpssTmNodesCtlReadNextChange",
        &wrCpssTmNodesCtlReadNextChange,
        1, 0},
      {"cpssTmNodesCtlCleanList",
        &wrCpssTmNodesCtlCleanList,
        1, 0}
 };

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmNodesCtl
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
GT_STATUS cmdLibInitCpssTmNodesCtl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
