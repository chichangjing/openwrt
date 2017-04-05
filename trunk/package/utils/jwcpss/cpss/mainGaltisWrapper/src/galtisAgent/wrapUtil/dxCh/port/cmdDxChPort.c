/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdDxChPort.c
*
* DESCRIPTION:
*     galtis agent interface for port interface/speed configuration
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>


/*******************************************************************************
* gtAppDemoLionPortModeSpeedSet
*
* DESCRIPTION:
*       Example of configuration sequence of different ports interface modes 
*       and speeds
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       ifMode   - Interface mode.
*       speed    - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       At this moment supported:
*           CPSS_PORT_INTERFACE_MODE_RXAUI_E        10G
*           CPSS_PORT_INTERFACE_MODE_XGMII_E (XAUI) 10G, 20G
*           CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E  10G
*           CPSS_PORT_INTERFACE_MODE_SGMII_E        10M, 100M, 1G, 2.5G
*           CPSS_PORT_INTERFACE_MODE_1000BASE_X_E   1G
*           CPSS_PORT_INTERFACE_MODE_XLG_E          40G
*
*******************************************************************************/
static CMD_STATUS wrGtAppDemoLionPortModeSpeedSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8     devNum;
    GT_U8     portNum;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    CPSS_PORT_SPEED_ENT  speed;
    GT_STATUS result = GT_FAIL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ifMode  = (CPSS_PORT_INTERFACE_MODE_ENT)inArgs[2];
    speed   = (CPSS_PORT_SPEED_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    result = cmdAppLionPortModeSpeedSet(devNum,portNum,ifMode,speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* gtAppDemoXcat2StackPortsModeSpeedSet
*
* DESCRIPTION:
*       Example of configuration sequence for stack ports of xcat2
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       ifMode   - Interface mode.
*       speed    - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*       GT_BAD_PARAM - device not exists
*       GT_NOT_SUPPORTED - wrong ifMode,speed
*       GT_NOT_APPLICABLE_DEVICE - wrong devFamily
*
* COMMENTS:
*       At this moment supported:
*           CPSS_PORT_INTERFACE_MODE_SGMII_E        10M, 100M, 1G, 2.5G
*           CPSS_PORT_INTERFACE_MODE_1000BASE_X_E   1G
*           CPSS_PORT_INTERFACE_MODE_100BASE_FX_E   100M
*
*******************************************************************************/
static CMD_STATUS wrGtAppXcat2StackPortModeSpeedSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8     devNum;
    GT_U8     portNum;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    CPSS_PORT_SPEED_ENT  speed;
    GT_STATUS result = GT_FAIL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ifMode  = (CPSS_PORT_INTERFACE_MODE_ENT)inArgs[2];
    speed   = (CPSS_PORT_SPEED_ENT)inArgs[3];

    result = cmdAppXcat2StackPortsModeSpeedSet(devNum,portNum,ifMode,speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* gtAppDemoXcat2SfpPortEnable
*
* DESCRIPTION:
*       This function configures MUX switching PHYs to SFPs to enable testing 
*       of 2.5G mode on network ports
*
* INPUTS:
*       devNum  - device number
*       portNum - port number (just network ports)
*       enable  -   GT_TRUE  - use SFP port
*                   GT_FALSE - use PHY port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*       GT_BAD_PARAM - wrong device or port
*       GT_NOT_APPLICABLE_DEVICE - wrong devFamily
*
* COMMENTS:
*       Enable SFP must be done only after port configured to one of appropriate
*       modes i.e. 1000BaseX, SGMII 2.5G or SGMII 1G (while in case of SGMII 1G
*       on SFP port to get link you must enable ForceLinkPass and inband autoneg. 
*       bypass, because SGMII 1G supposed to work over PHY).
*       And if you want to disable SFP and go back to default (PHY) you must
*       first configure QSGMII or other mode supported by regular network ports
*       on given device and only then call this function. 
*
*******************************************************************************/
static CMD_STATUS wrGtAppDemoXcat2SfpPortEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8     devNum;
    GT_U8     portNum;
    GT_BOOL   enable;
    GT_STATUS result = GT_FAIL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];

    result = cmdAppXcat2SfpPortEnable(devNum,portNum,enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

        {"gtAppDemoLionPortModeSpeedSet",
         &wrGtAppDemoLionPortModeSpeedSet,
         4, 0},

        {"gtAppDemoXcat2StackPortsModeSpeedSet",
         &wrGtAppXcat2StackPortModeSpeedSet,
         4, 0},

        {"gtAppDemoXcat2SfpPortEnable",
         &wrGtAppDemoXcat2SfpPortEnable,
         3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitAppDemoLionPort
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
GT_STATUS cmdLibInitAppDemoLionPort
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


