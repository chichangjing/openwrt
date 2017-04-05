/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChDiagPortSlicesDirectCtrl.c
*
* DESCRIPTION:
*       Wrapper functions for CPSS DXCH Port AP API
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/cpssDxChPortSlicesDirectCtrlDbg.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>    
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h> 


/*******************************************************************************
* cpssDxChSlicesNumSet
*
* DESCRIPTION:
*      The function sets slice number used in Pizza Arbiter for specific unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - port group id
*       unitIdx         - unit idex
*       sliceNum2Config - slices number to be set in pizza arbiter
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*       GT_ABORTED      - error in internal data structures
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChSlicesNumSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    portGroupNum;
    GT_U32    unitIdInt;
    CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT unitId;
    GT_U32    sliceNum2Config;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8) inArgs[0];
    portGroupNum    = (GT_U32)inArgs[1];
    unitIdInt       = (GT_U32)inArgs[2];
    sliceNum2Config = (GT_U32)inArgs[3];

    if (unitIdInt >= CPSS_DXCH_DIAG_PA_UNIT_MAX_E)
    {
        return GT_BAD_PARAM;
    }

    unitId = (CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT)unitIdInt;
    /* call cpss api function */
    result = cpssDxChSlicesNumSet(devNum, portGroupNum, unitId,sliceNum2Config);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChSlicesNumGet
*
* DESCRIPTION:
*      The function gets slice number used in Pizza Arbiter for specific unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       unitIdx     - unit idex
*       sliceNumConfigPtr   - slices number to be set in pizza arbiter
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*       GT_ABORTED      - error in internal data structures
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChSlicesNumGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    portGroupNum;
    GT_U32    unitIdInt;
    CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT unitId;
    GT_U32    sliceNum2Config;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8) inArgs[0];
    portGroupNum    = (GT_U32)inArgs[1];
    unitIdInt       = (GT_U32)inArgs[2];

    if (unitIdInt >= CPSS_DXCH_DIAG_PA_UNIT_MAX_E)
    {
        return GT_BAD_PARAM;
    }

    unitId = (CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT)unitIdInt;
    /* call cpss api function */
    result = cpssDxChSlicesNumGet(devNum, portGroupNum, unitId,&sliceNum2Config);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", sliceNum2Config);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChSliceStateSet
*
* DESCRIPTION:
*      The function sets slice state for specific unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       unitIdx     - unit idex
*       sliceIdx2Config   - slices number to be set in pizza arbiter
*       portIdx           - local port id
*       isEnable          - is enable or disable
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*       GT_ABORTED      - error in internal data structures
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChSliceStateSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    portGroupNum;
    GT_U32    unitIdInt;
    CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT unitId;
    GT_U32    sliceNum2Config;
    GT_U32    portIdx;
    GT_U32    isEnableInt;
    GT_BOOL   isEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8) inArgs[0];
    portGroupNum    = (GT_U32)inArgs[1];
    unitIdInt       = (GT_U32)inArgs[2];
    sliceNum2Config = (GT_U32)inArgs[3];
    portIdx         = (GT_U32)inArgs[4];
    isEnableInt     = (GT_U32)inArgs[5];

    if (unitIdInt >= CPSS_DXCH_DIAG_PA_UNIT_MAX_E)
    {
        return GT_BAD_PARAM;
    }
    unitId = (CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT)unitIdInt;
    isEnable = (GT_BOOL)  (isEnableInt != 0);
    /* call cpss api function */
    result = cpssDxChSliceStateSet(devNum, portGroupNum, unitId,sliceNum2Config,portIdx,isEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChSliceStateGet
*
* DESCRIPTION:
*      The function gets slice state for specific unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       unitIdx     - unit idex
*       sliceIdx2Config   - slices number to be set in pizza arbiter
*
* OUTPUTS:
*       portIdxPtr       - local port id
*       isEnablePtr      - is enable or disable
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*       GT_ABORTED      - error in internal data structures
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChSliceStateGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    portGroupNum;
    GT_U32    unitIdInt;
    CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT unitId;
    GT_U32    sliceNum2Config;
    GT_U32    portIdx;
    GT_BOOL   isEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8) inArgs[0];
    portGroupNum    = (GT_U32)inArgs[1];
    unitIdInt       = (GT_U32)inArgs[2];
    sliceNum2Config = (GT_U32)inArgs[3];

    if (unitIdInt >= CPSS_DXCH_DIAG_PA_UNIT_MAX_E)
    {
        return GT_BAD_PARAM;
    }
    unitId = (CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT)unitIdInt;
    portIdx  = 255; 
    isEnable = GT_FALSE;
    /* call cpss api function */
    result = cpssDxChSliceStateGet(devNum, portGroupNum, unitId,sliceNum2Config,/*OUT*/&portIdx,&isEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",isEnable,portIdx);
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
     /* commandName;                   funcReference;         funcArgs;  funcFields */
     {"cpssDxChSlicesNumSet",  &wrCpssDxChSlicesNumSet,  4,             0}
    ,{"cpssDxChSlicesNumGet",  &wrCpssDxChSlicesNumGet,  3,             0}
    ,{"cpssDxChSliceStateSet", &wrCpssDxChSliceStateSet, 6,             0} 
    ,{"cpssDxChSliceStateGet", &wrCpssDxChSliceStateGet, 4,             0} 
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPortSlicesDirectCtrl
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
GT_STATUS cmdLibInitCpssDxChPortSlicesDirectCtrl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

