/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssGenPhySmi.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
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
#include <cpss/generic/phy/cpssGenPhySmi.h>
#include <extPhyM/phyMacMSec.h> 

/*******************************************************************************
* cpssPhyErrataWaExecute
*
* DESCRIPTION:
*   This function execute workaround for PHY errata from a selected list.
*
* INPUTS:
*       devNum      - Device Number.
*       portNum     - Port Number.
*       errataWa    - the selected workaround to execute
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on not initialized function pointers for
*                                  read and write phy registers operations.
*       GT_BAD_PARAM             - on wrong device, port or requested WA
*       GT_NOT_APPLICABLE_DEVICE - on access to PHY the WA is not intended to
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssPhyErrataWaExecute
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    CPSS_PHY_ERRATA_WA_ENT errataWa;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    errataWa = (CPSS_PHY_ERRATA_WA_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssPhyErrataWaExecute(devNum, portNum, errataWa);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

#if !(defined (EXMXPM_FAMILY))

/*******************************************************************************
* wrPhyMacEEEModeSet
*
* DESCRIPTION:
*   This function sets EEE mode for 1540 PHY
*
* INPUTS:
*       devNum      - Device Number.
*       portNum     - Port Number.
*       mode        - EEE mode
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrPhyMacEEEModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8     devNum;
    GT_PHYSICAL_PORT_NUM     portNum;
    MACPHY_EEE_LPI_MODE_ENT mode;   
    GT_STATUS result = GT_FAIL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    mode  = (MACPHY_EEE_LPI_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    result = phyMacEEEModeSet(devNum,portNum,mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrPhyMacEEEModeGet
*
* DESCRIPTION:
*   This function sets EEE mode for 1540 PHY
*
* INPUTS:
*       devNum      - Device Number.
*       portNum     - Port Number.
*       modePtr     - EEE mode
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrPhyMacEEEModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8     devNum;
    GT_PHYSICAL_PORT_NUM     portNum;
    MACPHY_EEE_LPI_MODE_ENT mode;   
    GT_STATUS result = GT_FAIL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    result = phyMacEEEModeGet(devNum,portNum,&mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

#endif

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssPhyErrataWaExecute",
         &wrCpssPhyErrataWaExecute,
         3, 0}
#if !(defined (EXMXPM_FAMILY))
        ,

        {"phyMacEEESetMode",
         &wrPhyMacEEEModeSet,
         3, 0},

        {"phyMacEEEGetMode",
         &wrPhyMacEEEModeGet,
         2, 0}
#endif
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssGenPhySmi
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
GT_STATUS cmdLibInitCpssGenPhySmi
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

