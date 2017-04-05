/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChHwInit.c
*
* DESCRIPTION:
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>

/*******************************************************************************
* cpssDxChHwPpSoftResetTrigger
*
* DESCRIPTION:
*       This routine issue soft reset for a specific pp.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       device - device number to reset.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       The soft reset is the
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwPpSoftResetTrigger
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChHwPpSoftResetTrigger(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChHwPpSoftResetSkipParamSet
*
* DESCRIPTION:
*       This routine configure if to skip parameters related to soft reset.
*
* INPUTS:
*       devNum      - device number to reset.
*       skipType    - The skip parameter to set
*                     see comments in CPSS_HW_PP_RESET_SKIP_TYPE_ENT.
*       skipEnable  - GT_FALSE: Do Not Skip
*                     GT_TRUE:  Skip
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, or skipType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwPpSoftResetSkipParamSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_HW_PP_RESET_SKIP_TYPE_ENT skipType;
    GT_BOOL skipEnable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    skipType = (CPSS_HW_PP_RESET_SKIP_TYPE_ENT)inArgs[1];
    skipEnable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChHwPpSoftResetSkipParamSet(devNum,skipType, skipEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHwPpSoftResetSkipParamGet
*
* DESCRIPTION:
*       This routine return configuration of skip parameters related to soft reset.
*
* INPUTS:
*       devNum      - device number to reset.
*       skipType    - The skip parameter to set
*                     see comments in CPSS_HW_PP_RESET_SKIP_TYPE_ENT.
*
* OUTPUTS:
*       skipEnablePtr   - GT_FALSE: Do Not Skip
*                         GT_TRUE:  Skip
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, or skipType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwPpSoftResetSkipParamGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_HW_PP_RESET_SKIP_TYPE_ENT  skipType;
    GT_BOOL                         skipEnable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    skipType = (CPSS_HW_PP_RESET_SKIP_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChHwPpSoftResetSkipParamGet(devNum,skipType, &skipEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", skipEnable);


    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHwPpInitStageGet
*
* DESCRIPTION:
*       Indicates the initialization stage of the device.
*
* INPUTS:
*       devNum       - device number to reset.
*
* OUTPUTS:
*       initStagePtr - the stage of the device
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, or skipType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwPpInitStageGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_HW_PP_INIT_STAGE_ENT       initStage;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChHwPpInitStageGet(devNum,&initStage);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", initStage);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHwInterruptCoalescingSet
*
* DESCRIPTION:
*       Configures the interrupt coalescing parameters and enable\disable the
*       functionality.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: interrupt coalesing is enabled
*                 GT_FALSE: interrupt coalesing is disabled
*       period  - Minimal IDLE period between two consecutive interuupts.
*                 The units of this input parameter is in nSec, the
*                 resolution is in 320 nSec (the HW resolusion for the
*                 interrupt coalescing period is 0.32uSec). In case interrupt
*                 coalesing is enabled (enable == GT_TRUE) should be in the
*                 range 320 - 5242560. Otherwise (enable == GT_FALSE),
*                 ignored.
*       linkChangeOverride  - GT_TRUE: A link change in one of the ports
*                             results interrupt regardless of the (coalescing)
*                             period.
*                             GT_FALSE: A link change in one of the ports
*                             does not results interrupt immediately but
*                             according to the (coalescing) period.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwInterruptCoalescingSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_BOOL     enable;
    GT_U32      period;
    GT_BOOL     linkChangeOverride;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    period = (GT_U32)inArgs[2];
    linkChangeOverride = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChHwInterruptCoalescingSet(devNum, enable, period, linkChangeOverride);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChHwInterruptCoalescingGet
*
* DESCRIPTION:
*       Gets the interrupt coalescing configuration parameters.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE: interrupt coalesing is enabled
*                                 GT_FALSE: interrupt coalesing is disabled
*       periodPtr  - (pointer to) Minimal IDLE period between two consecutive
*                    interuupts. The units are in nSec with resolution of
*                    320nSec (due to HW resolution) and is relevant only in
*                    case interrupt coalesing is enabled (*enablePtr == GT_TRUE).
*                    Range 320 - 5242560.
*       linkChangeOverridePtr  - (pointer to)
*                                GT_TRUE: A link change in one of the ports
*                                results interrupt regardless of the (coalescing)
*                                period.
*                                GT_FALSE: A link change in one of the ports
*                                does not results interrupt immediately but
*                                according to the (coalescing) period.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - parameter out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The HW resolusion for the interrupt coalescing period is 0.32uSec.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwInterruptCoalescingGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_BOOL     enable;
    GT_U32      period;
    GT_BOOL     linkChangeOverride;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChHwInterruptCoalescingGet(devNum, &enable, &period,
                                                           &linkChangeOverride);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", enable, period,
                                                linkChangeOverride);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHwAuDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Address Update
*       descriptor, for a given device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwAuDescSizeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_PP_DEVICE_TYPE devType;
    GT_U32 descSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devType = (CPSS_PP_DEVICE_TYPE)inArgs[0];

    /* call cpss api function */
    result = cpssDxChHwAuDescSizeGet(devType, &descSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHwCoreClockGet
*
* DESCRIPTION:
*       This function returns the core clock value from cpss DB and from HW.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum         - The Pp's device number.
*
* OUTPUTS:
*       coreClkDbPtr    - Pp's core clock from cpss DB (MHz)
*       coreClkHwPtr    - Pp's core clock read from HW (MHz) or zero
*                         if PLL configuration as sampled at reset could
*                         not be mapped to core clock value.
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwCoreClockGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 coreClkDb;
    GT_U32 coreClkHw;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChHwCoreClockGet(devNum, &coreClkDb, &coreClkHw);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", coreClkDb, coreClkHw);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHwRxBufAlignmentGet
*
* DESCRIPTION:
*       This function returns the required alignment in bytes of a RX buffer,
*       for a given device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       byteAlignmentPtr - The buffer alignment (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwRxBufAlignmentGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_PP_DEVICE_TYPE devType;
    GT_U32 byteAlignment;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devType = (CPSS_PP_DEVICE_TYPE)inArgs[0];

    /* call cpss api function */
    result = cpssDxChHwRxBufAlignmentGet(devType, &byteAlignment);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHwRxDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Rx descriptor,
*       for a given device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwRxDescSizeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_PP_DEVICE_TYPE devType;
    GT_U32 descSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devType = (CPSS_PP_DEVICE_TYPE)inArgs[0];

    /* call cpss api function */
    result = cpssDxChHwRxDescSizeGet(devType, &descSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHwTxDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Tx descriptor,
*       for a given device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChHwTxDescSizeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_PP_DEVICE_TYPE devType;
    GT_U32 descSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devType = (CPSS_PP_DEVICE_TYPE)inArgs[0];

    /* call cpss api function */
    result = cpssDxChHwTxDescSizeGet(devType, &descSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgHwDevNumSet
*
* DESCRIPTION:
*       write HW device number
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       hwDevNum - HW device number (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgHwDevNumSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8           devNum;
    GT_HW_DEV_NUM   hwDevNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    hwDevNum = (GT_HW_DEV_NUM)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCfgHwDevNumSet(devNum, hwDevNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgHwDevNumGet
*
* DESCRIPTION:
*       Read HW device number
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       hwDevNumPtr - Pointer to HW device number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgHwDevNumGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_HW_DEV_NUM   hwDevNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCfgHwDevNumGet(devNum, &hwDevNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hwDevNum);

    return CMD_OK;

}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChHwPpSoftResetTrigger",
         &wrCpssDxChHwPpSoftResetTrigger,
         1, 0},
    {"cpssDxChHwPpSoftResetSkipParamSet",
         &wrCpssDxChHwPpSoftResetSkipParamSet,
         3, 0},
    {"cpssDxChHwPpSoftResetSkipParamGet",
         &wrCpssDxChHwPpSoftResetSkipParamGet,
         2, 0},
    {"cpssDxChHwPpInitStageGet",
         &wrCpssDxChHwPpInitStageGet,
         1, 0},
    {"cpssDxChHwInterruptCoalescingSet",
         &wrCpssDxChHwInterruptCoalescingSet,
         4, 0},
    {"cpssDxChHwInterruptCoalescingGet",
         &wrCpssDxChHwInterruptCoalescingGet,
         1, 0},
    {"cpssDxChHwAuDescSizeGet",
         &wrCpssDxChHwAuDescSizeGet,
         1, 0},
    {"cpssDxChHwCoreClockGet",
         &wrCpssDxChHwCoreClockGet,
         1, 0},
    {"cpssDxChHwRxBufAlignmentGet",
         &wrCpssDxChHwRxBufAlignmentGet,
         1, 0},
    {"cpssDxChHwRxDescSizeGet",
         &wrCpssDxChHwRxDescSizeGet,
         1, 0},
    {"cpssDxChHwTxDescSizeGet",
         &wrCpssDxChHwTxDescSizeGet,
         1, 0},
    {"cpssDxChCfgHwDevNumSet",
        &wrCpssDxChCfgHwDevNumSet,
        2, 0},
    {"cpssDxChCfgHwDevNumGet",
        &wrCpssDxChCfgHwDevNumGet,
        1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChHwInit
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
GT_STATUS cmdLibInitCpssDxChHwInit
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

