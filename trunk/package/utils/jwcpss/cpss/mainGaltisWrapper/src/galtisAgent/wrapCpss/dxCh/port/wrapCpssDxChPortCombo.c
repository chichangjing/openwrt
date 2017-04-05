/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChPortCombo.c
*
* DESCRIPTION:
*       Wrapper functions for Combo port cpss.dxCh functions
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
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCombo.h>

/*******************************************************************************
* cpssDxChPortSerdesComboModeSet
*
* DESCRIPTION:
*       Set Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number, values 20..23.
*       mode     - serdes mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum, mode
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortSerdesComboModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    CPSS_DXCH_PORT_SERDES_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mode = (CPSS_DXCH_PORT_SERDES_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesComboModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortSerdesComboModeGet
*
* DESCRIPTION:
*       Get Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number, values 20..23.
*
* OUTPUTS:
*       modePtr   -  pointer to Serdes mode.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortSerdesComboModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    CPSS_DXCH_PORT_SERDES_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesComboModeGet(devNum, portNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortComboModeEnableSet
*
* DESCRIPTION:
*       Register/unregister port as combo in CPSS SW DB and set some specific
*       parameters. This configuration will be used by cpssDxChPortModeSpeedSet,
*       when port's physical interface will be actually configured and for
*       other system needs.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*       enable  -   GT_TRUE - port is combo;
*                   GT_FALSE - port is regular.
*       paramsPtr - (ptr to) parameters describing specific combo port options,
*                   doesn't matter if enable == GT_FALSE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - paramsPtr == NULL
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortComboModeEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_U8                   portNum;
    GT_BOOL                 enable;
    CPSS_DXCH_PORT_COMBO_PARAMS_STC params;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];
    params.macArray[0].macNum           = (GT_U32)inArgs[3];
    params.macArray[0].macPortGroupNum  = (GT_U32)inArgs[4];
    params.macArray[1].macNum           = (GT_U32)inArgs[5];
    params.macArray[1].macPortGroupNum  = (GT_U32)inArgs[6];
    params.preferredMacIdx              = (GT_U32)inArgs[7];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortComboModeEnableSet(devNum, portNum, enable, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortComboModeEnableGet
*
* DESCRIPTION:
*       Get if port registered as combo and its parameters.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*
* OUTPUTS:
*       enablePtr       -   GT_TRUE - port is combo;
*                           GT_FALSE - port is regular.
*       paramsPtr       - parameters describing specific combo port options
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - enablePtr or paramsPtr == NULL
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortComboModeEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    GT_BOOL     enable;
    CPSS_DXCH_PORT_COMBO_PARAMS_STC params;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortComboModeEnableGet(devNum, portNum, &enable, &params);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "enable=%d,macNum[0]=%d,macPortGroupNum[0]=%d,macNum[1]=%d,macPortGroupNum[1]=%d,preferredMacIdx=%d", 
                 enable, params.macArray[0].macNum, params.macArray[0].macPortGroupNum,
                 params.macArray[1].macNum, params.macArray[1].macPortGroupNum,
                 params.preferredMacIdx);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortComboPortActiveMacSet
*
* DESCRIPTION:
*       Activate one of MAC's serving combo port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*       macPtr  - (ptr to) parameters describing one of MAC of given combo port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - macPtr == NULL
*       GT_FAIL         - on error
*
* COMMENTS:
*   
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortComboPortActiveMacSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_U8                   portNum;
    CPSS_DXCH_PORT_MAC_PARAMS_STC  mac;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_U8)inArgs[1];
    mac.macNum  = (GT_U32)inArgs[2];
    mac.macPortGroupNum = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortComboPortActiveMacSet(devNum, portNum, &mac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortComboPortActiveMacGet
*
* DESCRIPTION:
*       Get current active MAC of combo port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*
* OUTPUTS:
*       macPtr  - (ptr to) parameters describing active MAC
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - macPtr == NULL
*       GT_FAIL         - on error
*
* COMMENTS:
*   
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortComboPortActiveMacGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    CPSS_DXCH_PORT_MAC_PARAMS_STC   mac;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortComboPortActiveMacGet(devNum, portNum, &mac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "macNum=%d,macPortGroupNum=%d", 
                 mac.macNum, mac.macPortGroupNum);

    return CMD_OK;
}





