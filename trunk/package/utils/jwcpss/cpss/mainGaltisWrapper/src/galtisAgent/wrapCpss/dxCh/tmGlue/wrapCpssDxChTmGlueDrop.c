/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* wrapCpssDxChTmGlueDrop.c
*
* DESCRIPTION:
*       Wrapper functions for TM Drop = cpssDxChTmGlueDrop.c functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 19 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueDrop.h>


/*******************************************************************************
* cpssDxChTmGlueDropQueueProfileIdSet
*
* DESCRIPTION:
*   The function maps every Queue Id to its Profile Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       queueId            - TM queue id 
*                            (APPLICABLE RANGES: 0..16383).
*       profileId          - per queue profile Id
*                            (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueDropQueueProfileIdSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    queueId;
    GT_U32    profileId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    queueId    = (GT_U32)inArgs[1];
    profileId  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTmGlueDropQueueProfileIdSet(devNum, queueId, profileId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChTmGlueDropQueueProfileIdGet
*
* DESCRIPTION:
*   The function gets the Profile Id of a given QueueId.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       queueId            - TM queue id 
*                            (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       profileIdPtr       - (pointer to) profile Id per Queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueDropQueueProfileIdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    queueId;
    GT_U32    profileId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    queueId  = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueDropQueueProfileIdGet(devNum, queueId, &profileId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileId);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueDropProfileDropMaskSet
* 
* DESCRIPTION:
*   The function configures the Drop Masking.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       profileId          - configuration per profile Id
*                            (APPLICABLE RANGES: 0..7).
*       tc                 - TM traffic class 
*                            (APPLICABLE RANGES: 0..15).
*       dropMaskCfgPtr     - (pointer to) drop mask configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueDropProfileDropMaskSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    tc;
    GT_U32    profileId;
    CPSS_DXCH_TM_GLUE_DROP_MASK_STC dropMaskCfg; 

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    profileId  = (GT_U32)inArgs[1];
    tc         = (GT_U32)inArgs[2];

    dropMaskCfg.qTailDropUnmask = (GT_U32)inArgs[3];
    dropMaskCfg.qWredDropUnmask = (GT_U32)inArgs[4];
    dropMaskCfg.aTailDropUnmask = (GT_U32)inArgs[5];
    dropMaskCfg.aWredDropUnmask = (GT_U32)inArgs[6];
    dropMaskCfg.bTailDropUnmask = (GT_U32)inArgs[7];
    dropMaskCfg.bWredDropUnmask = (GT_U32)inArgs[8];
    dropMaskCfg.cTailDropUnmask = (GT_U32)inArgs[9];
    dropMaskCfg.cWredDropUnmask = (GT_U32)inArgs[10];
    dropMaskCfg.portTailDropUnmask      = (GT_U32)inArgs[11];
    dropMaskCfg.portWredDropUnmask      = (GT_U32)inArgs[12];
    dropMaskCfg.outOfResourceDropUnmask = (GT_U32)inArgs[13];
    dropMaskCfg.redPacketsDropMode      = (GT_U32)inArgs[14];

    /* call cpss api function */
    result = cpssDxChTmGlueDropProfileDropMaskSet(devNum, profileId, tc, &dropMaskCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueDropProfileDropMaskGet
*
* DESCRIPTION:
*   The function gets the Drop Mask configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       profileId          - configuration per profile Id
*                            (APPLICABLE RANGES: 0..7).
*       tc                 - TM traffic class 
*                            (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       dropMaskCfgPtr     - (pointer to) structure for drop mask configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueDropProfileDropMaskGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    tc;
    GT_U32    profileId;
    CPSS_DXCH_TM_GLUE_DROP_MASK_STC dropMaskCfg; 

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    profileId  = (GT_U32)inArgs[1];
    tc         = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTmGlueDropProfileDropMaskGet(devNum, profileId, tc, &dropMaskCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d", 
                 dropMaskCfg.qTailDropUnmask, dropMaskCfg.qWredDropUnmask, 
                 dropMaskCfg.aTailDropUnmask, dropMaskCfg.aWredDropUnmask,
                 dropMaskCfg.bTailDropUnmask, dropMaskCfg.bWredDropUnmask,
                 dropMaskCfg.cTailDropUnmask, dropMaskCfg.cWredDropUnmask,
                 dropMaskCfg.portTailDropUnmask, dropMaskCfg.portWredDropUnmask,
                 dropMaskCfg.outOfResourceDropUnmask, dropMaskCfg.redPacketsDropMode);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTmGlueDropTcToCosSet
* 
* DESCRIPTION:
*   The function configures TM TC to COS mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       tmTc               - TM traffic class
*                            (APPLICABLE RANGES: 0..15).
*       cos                - mapped COS value for TM-TC
*                            (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueDropTcToCosSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    tmTc;
    GT_U32    cos;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    tmTc    = (GT_U32)inArgs[1];
    cos     = (GT_U32)inArgs[2];


    /* call cpss api function */
    result = cpssDxChTmGlueDropTcToCosSet(devNum, tmTc, cos);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueDropTcToCosGet
*
* DESCRIPTION:
*   The function retrieves the TM TC to COS mapping COS value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       tmTc               - TM traffic class
*                            (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       cosPtr             - (pointer to) mapped COS value for TM-TC
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueDropTcToCosGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    tmTc;
    GT_U32    cos;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    tmTc    = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueDropTcToCosGet(devNum, tmTc, &cos);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cos);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChTmGlueDropQueueProfileIdSet",
        &wrCpssDxChTmGlueDropQueueProfileIdSet,
        3, 0},

    {"cpssDxChTmGlueDropQueueProfileIdGet",
        &wrCpssDxChTmGlueDropQueueProfileIdGet,
        2, 0},

    {"cpssDxChTmGlueDropProfileDropMaskSet",
        &wrCpssDxChTmGlueDropProfileDropMaskSet,
        15, 0},

    {"cpssDxChTmGlueDropProfileDropMaskGet",
        &wrCpssDxChTmGlueDropProfileDropMaskGet,
        3, 0},

    {"cpssDxChTmGlueDropTcToCosSet",
        &wrCpssDxChTmGlueDropTcToCosSet,
        3, 0},

    {"cpssDxChTmGlueDropTcToCosGet",
        &wrCpssDxChTmGlueDropTcToCosGet,
        2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChTmGlueDrop
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
GT_STATUS cmdLibInitCpssDxChTmGlueDrop
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


