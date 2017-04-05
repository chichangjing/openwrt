/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChVirtualTcam.c
*
* DESCRIPTION:
*       Wrapper functions for Cpss Dx Virtual Tcam
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 34 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>


/*******************************************************************************
* cpssDxChVirtualTcamManagerCreate
*
* DESCRIPTION:
*       Create Virtual TCAM manager.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_ALREADY_EXIST         - if already exist TCAM Manager with given Id
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamManagerCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId = (GT_U32)inArgs[0];

    /* call cpss api function */
    result = cpssDxChVirtualTcamManagerCreate(vTcamMngId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamManagerDelete
*
* DESCRIPTION:
*       Delete Virtual TCAM manager.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                   - on success
*       GT_BAD_STATE            - on devices still attached to TCAM manager
*       GT_NOT_FOUND            - on TCAM manager not found
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamManagerDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId = (GT_U32)inArgs[0];

    /* call cpss api function */
    result = cpssDxChVirtualTcamManagerDelete(vTcamMngId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamManagerDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing Virtual TCAM manager.
*   TCAM of one of old devices (if exist) copied to TCAMs the newly added devices.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       devListArr     - the array of device ids to add to the Virtual TCAM Manager.
*                          (APPLICABLE RANGES: 0..255)
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the Virtual TCAM Manager id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*       GT_ALREADY_EXIST         - if device already exist in DB
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
#define VIRT_TCAM_DEV_NUM_CNS 8
static CMD_STATUS wrCpssDxChVirtualTcamManagerDevAdd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId, numOfDevs;
    GT_U8           devList[VIRT_TCAM_DEV_NUM_CNS];
    GT_U32          ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId = (GT_U32)inArgs[0];
    numOfDevs  = (GT_U32)inArgs[1];
    for(ii = 0; ii < VIRT_TCAM_DEV_NUM_CNS; ii++)
    {
        devList[ii] = (GT_U8)inArgs[ii+2];
    }

    /* call cpss api function */
    result = cpssDxChVirtualTcamManagerDevListAdd(vTcamMngId, devList, numOfDevs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamManagerDevListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing Virtual TCAM manager.
*   TCAMs of removed devices not updated.
*   Removing last device caused cleaning up all Tcam Manager configurations.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       devListArr     - the array of device ids to remove from the
*                        Virtual TCAM Manager.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the Virtual TCAM Manager id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*       GT_NOT_INITIALIZED       - if there are no devices in DB
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamManagerDevRemove
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId, numOfDevs;
    GT_U8           devList[VIRT_TCAM_DEV_NUM_CNS];
    GT_U32          ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId = (GT_U32)inArgs[0];
    numOfDevs  = (GT_U32)inArgs[1];
    for(ii = 0; ii < VIRT_TCAM_DEV_NUM_CNS; ii++)
    {
        devList[ii] = (GT_U8)inArgs[ii+2];
    }

    /* call cpss api function */
    result = cpssDxChVirtualTcamManagerDevListRemove(vTcamMngId, devList, numOfDevs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamDefrag
*
* DESCRIPTION:
*       Defragment TCAM manager memory.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamDefrag
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId = (GT_U32)inArgs[0];

    /* call cpss api function */
    result = cpssDxChVirtualTcamDefrag(vTcamMngId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamResize
*
* DESCRIPTION:
*       Resize Virtual TCAM. Guaranteed amount of rules updated.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - Virtual TCAM Manager Id
*                        (APPLICABLE RANGES: 0..31)
*       vTcamId        - unique Id of  Virtual TCAM
*       rulePlace      - place to insert/remove space after it.
*                        for logical-index driven vTCAMs - logical-index,
*                        for priority driven vTCAMs - priority value.
*                        ----------------------------------------
*                        For logical-index driven vTCAMs:
*                        a) toInsert == GT_TRUE
*                             Function adds free space with size <sizeInRules> rules
*                             after logical-index == rulePlace.
*                             Logical indexes of existing rules higher than inserted space
*                             (i.e. rules with old-logical-index >= rulePlace)
*                             are increased by <sizeInRules>.
*                        b) toInsert == GT_FALSE
*                             Function invalidates existing rules with logical indexes
*                             between <rulePlace> and <rulePlace + sizeInRules - 1>.
*                             This space became free. Function decreases logical indexes of
*                             existing rules higher than freed space
*                             (i.e. rules with old-logical-index >= rulePlace) by <sizeInRules>.
*                        ----------------------------------------
*                        For priority driven vTCAMs:
*                        a) toInsert == GT_TRUE
*                             Function adds free space with size <sizeInRules> rules
*                             after all rules with priority == rulePlace.
*                        b) toInsert == GT_FALSE
*                             Function removes and frees space of <sizeInRules> rules
*                             after all rules with priority == rulePlace.
*                             No rules are removed, but rules can be moved in TCAM
*                             to gather free spaces to one chunk .
*                             Rules with given and lower priorities can be compressed
*                             using free spaces between them.
*                        ----------------------------------------
*                        CPSS_DXCH_VIRTUAL_TCAM_APPEND_CNS - used
*                        for logical-index driven vTCAMs only
*                        to append free space (i.e toInsert == GT_TRUE)
*                        CPSS_DXCH_VIRTUAL_TCAM_BEFORE_START_CNS - used
*                        for logical-index driven vTCAMs only
*                        to insert free space before the 0-th rule (i.e toInsert == GT_TRUE)
*       toInsert       - GT_TRUE - insert rule-places, GT_FALSE - remove rule-places
*                        logical index of rules up to inserted space increased,
*                        logical index of rules up to removed space - decreased,
*                        rules inside removed space - removed.
*       sizeInRules    - amount of rule-space to insert/remove
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NO_RESOURCE           - not enough free place in TCAM
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*       Example1 (logical-index driven vTCAM):
*           Old vTcam size = 200, rulePlace = 50, toInsert = GT_TRUE, sizeInRules = 100
*           New vTam size is 300.
*           Rules 0-49 logical index unchanged.
*           Rules 50-199 logical index increased by 100 to be 150-299.
*           Place for rules 50-149 added, contains no valid rules.
*       Example2 (logical-index driven vTCAM)
*           Old vTcam size = 300, rulePlace = 50, toInsert = GT_FALSE, sizeInRules = 100
*           New vTam size is 200.
*           Rules 0-49 logical index unchanged.
*           Rules 150-299 logical index decreased by 100 to be 50-199.
*           Place of rules 50-149 removed with all rules that contained.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamResize
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    GT_U32          rulePlace;
    GT_BOOL         toInsert;
    GT_U32          sizeInRules;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];
    rulePlace   = (GT_U32)inArgs[2];
    toInsert    = (GT_BOOL)inArgs[3];
    sizeInRules = (GT_U32)inArgs[4]; 

    /* call cpss api function */
    result = cpssDxChVirtualTcamResize(vTcamMngId,vTcamId,rulePlace,toInsert,sizeInRules);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChVirtualTcamRuleValidStatusSet
*
* DESCRIPTION:
*       Set Rule Valid Status in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*       valid            - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamRuleValidStatusSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId;
    GT_BOOL         valid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];
    ruleId   = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)inArgs[2];
    valid    = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChVirtualTcamRuleValidStatusSet(vTcamMngId,vTcamId,ruleId,valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleValidStatusGet
*
* DESCRIPTION:
*       Get Rule Valid Status in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       validPtr         - (pointer to)GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamRuleValidStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId;
    GT_BOOL         valid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];
    ruleId   = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)inArgs[2];

    /* call cpss api function */
    result = cpssDxChVirtualTcamRuleValidStatusGet(vTcamMngId,vTcamId,ruleId,&valid);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", valid);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleDelete
*
* DESCRIPTION:
*       Delete Rule From internal DB and Invalidate it in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamRuleDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];
    ruleId   = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)inArgs[2];

    /* call cpss api function */
    result = cpssDxChVirtualTcamRuleDelete(vTcamMngId,vTcamId,ruleId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleMove
*
* DESCRIPTION:
*       Move Rule in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       srcRuleId        - source rule id: for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*       dstRuleId        - destination rule id: for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamRuleMove
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     srcRuleId, dstRuleId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];
    srcRuleId   = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)inArgs[2];
    dstRuleId   = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)inArgs[3];

    /* call cpss api function */
    result = cpssDxChVirtualTcamRuleMove(vTcamMngId,vTcamId,srcRuleId,dstRuleId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChVirtualTcamRulePriorityUpdate
*
* DESCRIPTION:
*       Update rule priority (for priority driven vTCAM).
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - rule id: that used to refer existing rule
*       priority         - the new priority of the rule
*       position         - position in range of rule of the
*                          specified priority (see enum)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*       when the vtcam is FULL the rule MAY be absent from the HW for some time
*       during the function operation.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamRulePriorityUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId;
    GT_U32          priority;
    CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];
    ruleId      = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)inArgs[2];
    priority    = (GT_U32)inArgs[3];
    position    = (CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChVirtualTcamRulePriorityUpdate(vTcamMngId,vTcamId,ruleId,priority,position);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamDbRuleIdToHwIndexConvert
*
* DESCRIPTION:
*       debug function - convert rule Id to logical index and to hardware index.
*       purpose is to allow application to know where the CPSS inserted the rule
*       in to the hardware.
*       this is 'DB' (DataBase) operation that not access the hardware.
*       NOTE: the function will return failure if the ruleId was not inserted.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - ruleId to find.
*
* OUTPUTS:
*       logicalIndexPtr  - (pointer to) the logical index.  (can be NULL)
*       hwIndexPtr       - (pointer to) the hardware index. (can be NULL)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - if both logicalIndexPtr and hwIndexPtr are NULL
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamDbRuleIdToHwIndexConvert
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId;
    GT_U32                             logicalIndex;
    GT_U32                             hwIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];
    ruleId   = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)inArgs[2];

    /* call cpss api function */
    result = cpssDxChVirtualTcamDbRuleIdToHwIndexConvert(vTcamMngId,vTcamId,ruleId,&logicalIndex, &hwIndex);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", logicalIndex,hwIndex);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChVirtualTcamUsageGet
*
* DESCRIPTION:
*       Get Virtual TCAM usage.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       vTcamUsagePtr   - (pointer to) Virtual TCAM Usage structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamUsageGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC   vTcamUsage;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChVirtualTcamUsageGet(vTcamMngId,vTcamId,&vTcamUsage);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", vTcamUsage.rulesUsed,vTcamUsage.rulesFree);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamNextRuleIdGet
*
* DESCRIPTION:
*        Function gets next existing rule ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*                           CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS used to start iteration of rules
*
* OUTPUTS:
*       nextRuleIdPtr     -      (pointer to)id of next rule
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_EMPTY                 - on 'START_RULE_ID' indication and no rules exists.
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NO_MORE               - all rule already passed by iteration
*
* COMMENTS:
*      The next rule is once written and not removed rule.
*      It can be valid or invalid.
*      It is the next by lookup order.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVirtualTcamNextRuleIdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ID        ruleId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ID        nextRuleId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId  = (GT_U32)inArgs[0];
    vTcamId     = (GT_U32)inArgs[1];
    ruleId      = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ID)inArgs[2];

    /* call cpss api function */
    result = cpssDxChVirtualTcamNextRuleIdGet(vTcamMngId,vTcamId,ruleId, &nextRuleId);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", nextRuleId);
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChVirtualTcamsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    GT_U32          ii;
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC   vTcamInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*support options that the number of fields will grow ... give default to all fields */
    cpssOsMemSet(&vTcamInfo,0,sizeof(vTcamInfo));

    /* map input arguments to locals */
    vTcamMngId          = (GT_U32)inArgs[0];

    ii = 0;
    vTcamId                         = (GT_U32)inFields[ii++];
    vTcamInfo.clientGroup           = (GT_U32)inFields[ii++];
    vTcamInfo.hitNumber             = (GT_U32)inFields[ii++];
    vTcamInfo.ruleSize              = (CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT)inFields[ii++];
    vTcamInfo.autoResize            = (GT_BOOL)inFields[ii++];
    vTcamInfo.guaranteedNumOfRules  = (GT_U32)inFields[ii++];
    vTcamInfo.ruleAdditionMethod    = (CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT)inFields[ii];

    /* call cpss api function */
    rc = cpssDxChVirtualTcamCreate(vTcamMngId,vTcamId,&vTcamInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChVirtualTcamsDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId          = (GT_U32)inArgs[0];

    vTcamId             = (GT_U32)inFields[0];

    /* call cpss api function */
    rc = cpssDxChVirtualTcamRemove(vTcamMngId,vTcamId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChVirtualTcamsClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    vTcamMngId          = (GT_U32)inArgs[0];

    for(vTcamId = 0; vTcamId <= CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS; vTcamId++)
    {
        /* call cpss api function */
        rc = cpssDxChVirtualTcamRemove(vTcamMngId,vTcamId);
        if(rc == GT_NOT_FOUND)
        {
            rc = GT_OK;
        }
        else if(rc != GT_OK)
        {
            break;
        }
    }
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/* index to use for 'refresh' on cpssDxChVirtualTcamGet */
static GT_U32 virtualTcamGlobalIndex = 0;

/* wrapper for cpssDxChVirtualTcamGet (allow refresh of multi entries)*/
static CMD_STATUS wrCpssDxChVirtualTcamGet
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc = GT_OK;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    GT_U32  ii;/*index in inFields */
    GT_U32  actualNumFields;/* number of fields */
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC vTcamInfo;

    cpssOsMemSet(&vTcamInfo, 0, sizeof(vTcamInfo));

    /* map input arguments to locals */
    vTcamMngId          = (GT_U32)inArgs[0];

    for(vTcamId = virtualTcamGlobalIndex; vTcamId <= CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS; vTcamId++)
    {
        rc = cpssDxChVirtualTcamInfoGet(vTcamMngId,vTcamId,&vTcamInfo);
        if(rc == GT_NOT_FOUND)
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if(vTcamId > CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    virtualTcamGlobalIndex = vTcamId + 1;

    ii = 0;
    inFields[ii++] = vTcamId                          ;
    inFields[ii++] = vTcamInfo.clientGroup            ;
    inFields[ii++] = vTcamInfo.hitNumber              ;
    inFields[ii++] = vTcamInfo.ruleSize               ;
    inFields[ii++] = vTcamInfo.autoResize             ;
    inFields[ii++] = vTcamInfo.guaranteedNumOfRules   ;
    inFields[ii++] = vTcamInfo.ruleAdditionMethod     ;

    actualNumFields = ii;
    /* pack and output table fields */
    for (ii = 0; ii < actualNumFields; ii++)
    {
        fieldOutput("%d", inFields[ii]);
        fieldOutputSetAppendMode();
    }

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/* wrapper for cpssDxChVirtualTcamInfoGet (allow refresh of multi entries)*/
static CMD_STATUS wrCpssDxChVirtualTcamsGetFirst
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    virtualTcamGlobalIndex = 0;

    return wrCpssDxChVirtualTcamGet(inArgs,inFields,numFields,outArgs);
}
/* wrapper for cpssDxChVirtualTcamInfoGet (allow refresh of multi entries)*/
static CMD_STATUS wrCpssDxChVirtualTcamsGetNext
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChVirtualTcamGet(inArgs,inFields,numFields,outArgs);
}

GT_U32          wrCpssDxChVirtualTcam_useVTcamApi = GT_FALSE;
GT_U32          wrCpssDxChVirtualTcam_vTcamMngId = 0;
GT_U32          wrCpssDxChVirtualTcam_vTcamId = 0;
GT_U32          wrCpssDxChVirtualTcam_vTcamRulePrio = 0;

void wrCpssDxChVirtualTcamRuleOperation
(
    IN  GT_BOOL         usePclTtiWrapers,
    IN  GT_U32          vTcamMngId,
    IN  GT_U32          vTcamId,
    IN  GT_U32          rulePrio
)
{
    wrCpssDxChVirtualTcam_useVTcamApi   = (usePclTtiWrapers) ? GT_FALSE : GT_TRUE;
    wrCpssDxChVirtualTcam_vTcamMngId    = vTcamMngId;
    wrCpssDxChVirtualTcam_vTcamId       = vTcamId;
    wrCpssDxChVirtualTcam_vTcamRulePrio = rulePrio;

    return;
}

static CMD_STATUS wrCpssDxChVirtualTcamRuleOperationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BOOL         usePclTtiWrapers;
    GT_U32          vTcamMngId;
    GT_U32          vTcamId;
    GT_U32          rulePrio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)        
        return CMD_AGENT_ERROR;    

    /* map input arguments to locals */
    usePclTtiWrapers    = (GT_BOOL)inArgs[0];
    vTcamMngId          = (GT_U32)inArgs[1];
    vTcamId             = (GT_U32)inArgs[2];
    rulePrio            = (GT_U32)inArgs[3];

    wrCpssDxChVirtualTcamRuleOperation(usePclTtiWrapers,vTcamMngId,vTcamId,rulePrio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChVirtualTcamManagerCreate",
        &wrCpssDxChVirtualTcamManagerCreate,
        1, 0},
    {"cpssDxChVirtualTcamManagerDelete",
        &wrCpssDxChVirtualTcamManagerDelete,
        1, 0},
    {"cpssDxChVirtualTcamManagerDevAdd",
        &wrCpssDxChVirtualTcamManagerDevAdd,
        10, 0},
    {"cpssDxChVirtualTcamManagerDevRemove",
        &wrCpssDxChVirtualTcamManagerDevRemove,
        10, 0},
    {"cpssDxChVirtualTcamDefrag",
        &wrCpssDxChVirtualTcamDefrag,
        1, 0},
    {"cpssDxChVirtualTcamResize",
        &wrCpssDxChVirtualTcamResize,
        5, 0},
    {"cpssDxChVirtualTcamRuleValidStatusSet",
        &wrCpssDxChVirtualTcamRuleValidStatusSet,
        4, 0},
    {"cpssDxChVirtualTcamRuleValidStatusGet",
        &wrCpssDxChVirtualTcamRuleValidStatusGet,
        3, 0},
    {"cpssDxChVirtualTcamRuleDelete",
        &wrCpssDxChVirtualTcamRuleDelete,
        3, 0},
    {"cpssDxChVirtualTcamRuleMove",
        &wrCpssDxChVirtualTcamRuleMove,
        4, 0},
    {"cpssDxChVirtualTcamRulePriorityUpdate",
        &wrCpssDxChVirtualTcamRulePriorityUpdate,
        5, 0},
    {"cpssDxChVirtualTcamDbRuleIdToHwIndexConvert",
        &wrCpssDxChVirtualTcamDbRuleIdToHwIndexConvert,
        3, 0},
    {"cpssDxChVirtualTcamUsageGet",
        &wrCpssDxChVirtualTcamUsageGet,
        2, 0},
    {"cpssDxChVirtualTcamNextRuleIdGet",
        &wrCpssDxChVirtualTcamNextRuleIdGet,
        3, 0},

    /* start table: cpssDxChVirtualTcams */

    {"cpssDxChVirtualTcamsSet",
        &wrCpssDxChVirtualTcamsSet,
        1, 7},

    {"cpssDxChVirtualTcamsGetFirst",
        &wrCpssDxChVirtualTcamsGetFirst,
        1, 0},

    {"cpssDxChVirtualTcamsGetNext",
        &wrCpssDxChVirtualTcamsGetNext,
        1, 0},

    {"cpssDxChVirtualTcamsDelete",
        &wrCpssDxChVirtualTcamsDelete,
        1, 7},

    {"cpssDxChVirtualTcamsClear",
        &wrCpssDxChVirtualTcamsClear,
        1, 0},

    /* end table: cpssDxChVirtualTcams */

    {"cpssDxChVirtualTcamRuleOperationSet",
        &wrCpssDxChVirtualTcamRuleOperationSet,
        4, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChVirtualTcam
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
GT_STATUS cmdLibInitCpssDxChVirtualTcam
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

