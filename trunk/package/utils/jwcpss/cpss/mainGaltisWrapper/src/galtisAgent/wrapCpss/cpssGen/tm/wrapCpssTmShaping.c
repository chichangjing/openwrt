/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssTmShaping.c
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
#include <cpss/generic/tm/cpssTmShaping.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>




/*******************************************************************************
* cpssTmShapingProfileCreate
*
* DESCRIPTION:
*       Create a Shaping Profile.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Level to configure the profile for.
*       profileStrPtr   - Shaping profile configuration struct pointer.
*
* OUTPUTS:
*       profileIndPtr   - (pointer to) The created Shaping profile index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_FULL                  - on full allocation of Shaping profile table.
*       GT_NOT_INITIALIZED       - on not configured periodic update rate for the given level
*       GT_BAD_SIZE              - on too large min/max token.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmShapingProfileCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    CPSS_TM_LEVEL_ENT                       level;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC      profileStr;
    GT_U32                                  profileInd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    level = (GT_U32)inArgs[1];

    profileStr.cirBw = (GT_U32)inArgs[2];
    profileStr.cbs = (GT_U32)inArgs[3];
    profileStr.eirBw = (GT_U32)inArgs[4];
    profileStr.ebs = (GT_U32)inArgs[5];  

    /* call cpss api function */
    result = cpssTmShapingProfileCreate(devNum, level, &profileStr, &profileInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileInd);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmShapingProfileDelete
*
* DESCRIPTION:
*       Delete a Shaping Profile.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       profileInd      - An index of the Shaping profile.
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
static CMD_STATUS wrCpssTmShapingProfileDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    profileInd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileInd = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssTmShapingProfileDelete(devNum, profileInd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmShapingProfileRead
*
* DESCRIPTION:
*       Read Shaping profile.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Level to read the profile for.
*       profileInd      - An index of the Shaping profile.
*
* OUTPUTS:
*       profileStrPtr   - (pointer to) Shaping profile configuration structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The cirBw and eirBw may deviate from the originally configured
*       by cpssTmShapingProfileCreate or cpssTmShapingProfileUpdate by the 
*       bw accuracy parameter provisioned in cpssTmSchedPeriodicSchemeConfig API.
*******************************************************************************/
static CMD_STATUS wrCpssTmShapingProfileRead
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    CPSS_TM_LEVEL_ENT                   level;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileStr;
    GT_U32                              profileInd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    level = (GT_U32)inArgs[1];
    profileInd = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssTmShapingProfileRead(devNum, level, profileInd, &profileStr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", profileStr.cirBw, profileStr.cbs, profileStr.eirBw, profileStr.ebs);

    return CMD_OK;
}


/*******************************************************************************
* cpssTmShapingProfileUpdate
*
* DESCRIPTION:
*       Update a Shaping Profile.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Level to update the profile for.
*       profileInd      - An index of the Shaping profile.
*       profileStrPtr   - Shaping profile configuration struct pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_INITIALIZED       - on not configured periodic update rate for the given level
*       GT_BAD_SIZE              - on too large min/max token.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmShapingProfileUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    CPSS_TM_LEVEL_ENT                       level;
    GT_U32                                  profileInd;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC      profileStr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    level = (GT_U32)inArgs[1];
    profileInd = (GT_U32)inArgs[2];

    profileStr.cirBw = (GT_U32)inArgs[3];
    profileStr.cbs = (GT_U32)inArgs[4];
    profileStr.eirBw = (GT_U32)inArgs[5];
    profileStr.ebs = (GT_U32)inArgs[6];  

    /* call cpss api function */
    result = cpssTmShapingProfileUpdate(devNum, level, profileInd, &profileStr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssTmShapingProfileCreate",
        &wrCpssTmShapingProfileCreate,
        6, 0},

    {"cpssTmShapingProfileDelete",
        &wrCpssTmShapingProfileDelete,
        2, 0},

    {"cpssTmShapingProfileRead",
        &wrCpssTmShapingProfileRead,
        3, 0},

    {"cpssTmShapingProfileUpdate",
        &wrCpssTmShapingProfileUpdate,
        7, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmShaping
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
GT_STATUS cmdLibInitCpssTmShaping
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
