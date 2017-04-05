
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssTmNodesTree.c
*
* DESCRIPTION:
*       Wrapper functions for TM Tree APIs. 
*
* FILE REVISION NUMBER:
*       $Revision: 1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/tm/cpssTmNodesTree.h>

/*******************************************************************************
* cpssTmTreeChangeStatus
*
* DESCRIPTION:
*       Change the tree DeQ status. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       status          - Tree status.
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
static CMD_STATUS wrCpssTmTreeChangeStatus
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    status = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssTmTreeChangeStatus(devNum, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmTreeStatusGet
*
* DESCRIPTION:
*       Retrieve tree DeQ status. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*
* OUTPUTS:
*       status          -(pointer to)  Tree status.
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
static CMD_STATUS wrCpssTmTreeStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* call cpss api function */
    result = cpssTmTreeStatusGet(devNum, &status);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

	/* pack output arguments to galtis string */
	galtisOutput(outArgs, result, "%d",status);

    return CMD_OK;
}




/*******************************************************************************
* cpssTmTreeDwrrPrioSet
*
* DESCRIPTION:
*       Change the tree DWRR priority. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       priosPtr        - Priority array pointer.
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
*       Set priosPtr[i] = GT_FALSE/GT_TRUE, if DWRR for prio i is disabled/enabled.
*******************************************************************************/
static CMD_STATUS wrCpssTmTreeDwrrPrioSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   priosArr[8];
    GT_U32    i;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    for(i=0; i<8; i++) {
        priosArr[i] = (GT_BOOL)inArgs[i+1];
    }

    /* call cpss api function */
    result = cpssTmTreeDwrrPrioSet(devNum, priosArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssTmTreeDwrrPrioGet
*
* DESCRIPTION:
*       Retrieve the tree DWRR per priority status. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*
* OUTPUTS:
*       priosPtr        - Status per priority array pointer.
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
*       Set priosPtr[i] = GT_FALSE/GT_TRUE, if DWRR for prio i is disabled/enabled.
*******************************************************************************/
static CMD_STATUS wrCpssTmTreeDwrrPrioGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   priosArr[8];
    GT_U32    i;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    for(i=0; i<8; i++) {
        priosArr[i] = (GT_BOOL)inArgs[i+1];
    }

    /* call cpss api function */
    result = cpssTmTreeDwrrPrioGet(devNum, priosArr);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d",
                    priosArr[0],priosArr[1],priosArr[2],priosArr[3],priosArr[4],priosArr[5],priosArr[6],priosArr[7]);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssTmTreeChangeStatus",
        &wrCpssTmTreeChangeStatus,
        2, 0},
    {"cpssTmTreeStatusGet",
        &wrCpssTmTreeStatusGet,
        1, 0},

    {"cpssTmTreeDwrrPrioSet",
        &wrCpssTmTreeDwrrPrioSet,
        9, 0},
    {"cpssTmTreeDwrrPrioGet",
        &wrCpssTmTreeDwrrPrioGet,
        1, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmNodesTree
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
GT_STATUS cmdLibInitCpssTmNodesTree
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
