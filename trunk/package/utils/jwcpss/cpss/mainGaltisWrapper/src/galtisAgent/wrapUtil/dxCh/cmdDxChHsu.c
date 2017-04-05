/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdDxChHsu.c
*
* DESCRIPTION:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/hsu/cpssDxChHsu.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/hsu/private/prvCpssDxChHsu.h>

/*******************************************************************************
* cpssDxChHsuOldImagePreUpdatePreparation
*
* DESCRIPTION:
*       This function perform following actions:
*       1. Disable interrupts
*       2. export of all hsu datatypes
*       3. warm restart
*
* APPLICABLE DEVICES: All DxCh Devices.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChHsuOldImagePreUpdatePreparation
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_BOOL                         isMultipleIteration;
    GT_U32                          origIterationSize;
    CPSS_SYSTEM_RECOVERY_MODE_STC   hsuMode = {0};

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    isMultipleIteration = (GT_BOOL)inArgs[0];
    origIterationSize = (GT_U32)inArgs[1];
    switch ((GT_U32)inArgs[2])
    {
    case 0:
        hsuMode.continuousAuMessages = GT_TRUE;
        hsuMode.continuousFuMessages = GT_TRUE;
        hsuMode.continuousRx = GT_TRUE;
        hsuMode.continuousTx = GT_TRUE;
        break;
    case 1:
        hsuMode.continuousAuMessages = GT_FALSE;
        hsuMode.continuousFuMessages = GT_FALSE;
        hsuMode.continuousRx = GT_FALSE;
        hsuMode.continuousTx = GT_FALSE;
        break;
    default:
        return GT_BAD_PARAM;
    }
    result =  cmdAppDxChHsuOldImagePreUpdatePreparation(isMultipleIteration,
                                                        origIterationSize,
                                                        hsuMode);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChHsuNewImageUpdateAndSynch
*
* DESCRIPTION:
*       This function perform following actions:
*       1. import of all hsu datatypes
*       2. setting HSU state in "HSU is complete"
*       3. enable interrupts
*
* APPLICABLE DEVICES: All DxCh Devices.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChHsuNewImageUpdateAndSynch
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_BOOL isMultipleIteration;
    GT_U32 origIterationSize;
    GT_U32 hsuDataSize;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    isMultipleIteration = (GT_BOOL)inArgs[0];
    origIterationSize = (GT_U32)inArgs[1];
    hsuDataSize = (GT_U32)inArgs[2];
    result = cmdAppDxChHsuNewImageUpdateAndSynch(isMultipleIteration,
                                                   origIterationSize,
                                                   hsuDataSize);
     /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}





/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssDxChHsuOldImagePreUpdatePreparation",
         &wrCpssDxChHsuOldImagePreUpdatePreparation,
         3, 0},
        {"cpssDxChHsuNewImageUpdateAndSynch",
         &wrCpssDxChHsuNewImageUpdateAndSynch,
         4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitAppDemoDxChHsu
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
GT_STATUS cmdLibInitAppDemoDxChHsu
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

