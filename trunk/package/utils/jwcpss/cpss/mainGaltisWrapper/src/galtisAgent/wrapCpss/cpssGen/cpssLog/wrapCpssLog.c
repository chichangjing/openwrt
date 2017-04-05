/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssLog.c
*
* DESCRIPTION:
*       Wrapper functions for CPSS LOG API
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
#include <cpss/generic/log/cpssLog.h>

/*******************************************************************************
* cpssLogEnableSet
*
* DESCRIPTION:
*       The function enables/disables Log feature.
*
* INPUTS:
*       enable - GT_TRUE for log enable and GT_False for log disable
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssLogEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    enable = (GT_BOOL)inArgs[0];

    cpssLogEnableSet(enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssLogLibEnableSet
*
* DESCRIPTION:
*       The function enables/disables specific type of log information for
*       specific or all libs.
*
* INPUTS:
*       lib - lib to enable/disable log for.
*       type - log type.
*       enable - GT_TRUE for log enable and GT_FALSE for log disable
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PARAM    - bad parameter lib or type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssLogLibEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_LOG_LIB_ENT    lib;
    CPSS_LOG_TYPE_ENT   type;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lib = (CPSS_LOG_LIB_ENT)inArgs[0];
    type = (CPSS_LOG_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    result = cpssLogLibEnableSet(lib,type,enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssLogApiFormatSet
*
* DESCRIPTION:
*       The function set format for API related log.
*
* INPUTS:
*       format - format of API related logs.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PARAM    - bad parameter format
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssLogApiFormatSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_LOG_API_FORMAT_ENT format;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    format = (CPSS_LOG_API_FORMAT_ENT)inArgs[0];

    result = cpssLogApiFormatSet(format);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssLogStackSizeSet
*
* DESCRIPTION:
*       Defines the size of the stack, how many function will be logged before
*       an error occurred.
*
* INPUTS:
*       size - the amount of functions.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK           - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssLogStackSizeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32    size;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    size = (GT_U32)inArgs[0];

    result = cpssLogStackSizeSet(size);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssLogLineNumberEnableSet
*
* DESCRIPTION:
*       The function enables/disables using of a source code line number by
*       some logging functions
*
* INPUTS:
*       enable - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssLogLineNumberEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    enable = (GT_BOOL)inArgs[0];

    cpssLogLineNumberEnableSet(enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssLogEnableSet",
        &wrCpssLogEnableSet,
        1, 0},
    {"cpssLogLibEnableSet",
        &wrCpssLogLibEnableSet,
        3, 0},
    {"cpssLogApiFormatSet",
        &wrCpssLogApiFormatSet,
        1, 0},
    {"cpssLogStackSizeSet",
        &wrCpssLogStackSizeSet,
        1, 0},
    {"cpssLogLineNumberEnableSet",
        &wrCpssLogLineNumberEnableSet,
        1, 0}
};


#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssLog
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
GT_STATUS cmdLibInitCpssLog
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

