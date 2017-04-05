/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* wrapCpssDxChTmGlueFlowControl.c
*
* DESCRIPTION:
*       Wrapper functions for TM Flow Control API.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueFlowControl.h>

/*******************************************************************************
* cpssDxChTmGlueFlowControlEgressCounterSet
*
* DESCRIPTION:
*       Set value of the TM Egress Flow Control counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       tmPortInd       - TM port index.
*                         (APPLICABLE RANGES: 0..191).
*       value           - value of the TM egress flow control counter.
*                         (APPLICABLE RANGES: 0..1023).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number, TM port index.
*       GT_OUT_OF_RANGE          - on wrong counter value.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueFlowControlEgressCounterSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tmPortInd;
    GT_U32 value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    tmPortInd   = (GT_U32)inArgs[1];
    value       = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTmGlueFlowControlEgressCounterSet(devNum, tmPortInd, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueFlowControlEgressCounterGet
*
* DESCRIPTION:
*       Get value of the TM Egress Flow Control counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       tmPortInd       - TM port index.
*                         (APPLICABLE RANGES: 0..191).
* OUTPUTS:
*       valuePtr        - (poiter to) value of the TM egress flow control counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or TM port index.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueFlowControlEgressCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tmPortInd;
    GT_U32 value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    tmPortInd = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueFlowControlEgressCounterGet(devNum, tmPortInd, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable Flow Control for TM.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       egressEnable - GT_TRUE / GT_FALSE - enable / disable egress Flow Control for TM.
*       ingressEnable- GT_TRUE / GT_FALSE - enable / disable ingress Flow Control for TM,
*                      relevant for Caelum only.  
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       egressEnable must be enabled whenever the TM is used
*       to response back-pressure from physical ports to TM Ports,
*       used along with cpssTmSchedPortExternalBpSet.
* 
*       Caelum global ingressEnable is used along with
*       Caelum port cpssDxChTmGluePfcResponseModeSet.
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueFlowControlEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_BOOL     egressEenable;
    GT_BOOL     ingressEenable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    egressEenable = (GT_BOOL)inArgs[1];
    ingressEenable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTmGlueFlowControlEnableSet(devNum, egressEenable, ingressEenable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueFlowControlEnableGet
*
* DESCRIPTION:
*       Get state of Flow Control for TM.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       egressEnablePtr  - (pointer to) egress flow control state.
*       ingressEnablePtr - (pointer to) ingress flow control state.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueFlowControlEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL egressEnable;
    GT_BOOL ingressEnable = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTmGlueFlowControlEnableGet(devNum, &egressEnable, &ingressEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", egressEnable, ingressEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueFlowControlEgressThresholdsSet
*
* DESCRIPTION:
*       Set XON/XOFF TM Egress Flow Control thresholds values.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       tmPortInd       - TM port index.
*                         (APPLICABLE RANGES: 0..191).
*       xOffThreshold    - TM FC Queue XOFF threshold.
*                         (APPLICABLE RANGES: 0..1023).
*       xOnThreshold     - TM FC Queue XON threshold.
*                         (APPLICABLE RANGES: 0..1023).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number, TM port index.
*       GT_HW_ERROR              - on hardware error.
*       GT_OUT_OF_RANGE          - on wrong XOFF/XON threshold.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueFlowControlEgressThresholdsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tmPortInd;
    GT_U32 xOffThreshold;
    GT_U32 xOnThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    tmPortInd       = (GT_U32)inArgs[1];
    xOffThreshold   = (GT_U32)inArgs[2];
    xOnThreshold    = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChTmGlueFlowControlEgressThresholdsSet(devNum, tmPortInd,
                                                          xOffThreshold,
                                                          xOnThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueFlowControlEgressThresholdsGet
*
* DESCRIPTION:
*       Get XON/XOFF TM Egress Flow Control thresholds values.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       tmPortInd       - TM port index.
*                         (APPLICABLE RANGES: 0..191).
* OUTPUTS:
*       xOffThresholdPtr - (pointer to) TM FC Queue XOFF threshold.
*       xOnThresholdPtr  - (pointer to) TM FC Queue XON threshold.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or TM port index.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueFlowControlEgressThresholdsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tmPortInd;
    GT_U32 xOffThreshold;
    GT_U32 xOnThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    tmPortInd       = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueFlowControlEgressThresholdsGet(devNum, tmPortInd,
                                                          &xOffThreshold,
                                                          &xOnThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", xOffThreshold, xOnThreshold);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueFlowControlPortSpeedSet
*
* DESCRIPTION:
*       Set PFC calibration value by port speed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number.
*       speed       - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*   	This API should be called on changing of TM Enabled	physical port speed
*   	mapped to the PFC port, as a result of:
*   		1) port speed has changed. (cpssDxChPortModeSpeedSet)
*   		2) PFC port mapping has changed (cpssDxChTmGluePfcPortMappingSet).
* 
*       This table is the calibration value to be multiplied to the value in the PFC header
*       to be alligned to the number of cycles according to the port speed.
*        
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueFlowControlPortSpeedSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    CPSS_PORT_SPEED_ENT     speed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    speed = (CPSS_PORT_SPEED_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTmGlueFlowControlPortSpeedSet(devNum, portNum, speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/
static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChTmGlueFlowControlEgressCounterSet",
        &wrCpssDxChTmGlueFlowControlEgressCounterSet,
        3, 0},
    {"cpssDxChTmGlueFlowControlEgressCounterGet",
        &wrCpssDxChTmGlueFlowControlEgressCounterGet,
       2, 0},
    {"cpssDxChTmGlueFlowControlEnableSet",
        &wrCpssDxChTmGlueFlowControlEnableSet,
        3, 0},
    {"cpssDxChTmGlueFlowControlEnableGet",
        &wrCpssDxChTmGlueFlowControlEnableGet,
        1, 0},
    {"cpssDxChTmGlueFlowControlEgressThresholdsSet",
        &wrCpssDxChTmGlueFlowControlEgressThresholdsSet,
        4, 0},
    {"cpssDxChTmGlueFlowControlEgressThresholdsGet",
        &wrCpssDxChTmGlueFlowControlEgressThresholdsGet,
        2, 0},

    {"cpssDxChTmGlueFlowControlPortSpeedSet",
        &wrCpssDxChTmGlueFlowControlPortSpeedSet,
        3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChTmGluePfc
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
GT_STATUS cmdLibInitCpssDxChTmGlueFlowControl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
