/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmNetIf.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <galtisAgent/wrapUtil/cmdCpssNetReceive.h>

/*******************************************************************************/
/**************************Rx PACKET HANDLING **********************************/
/*******************************************************************************/

extern CMD_STATUS wrCpssExMxPmNetIfRxPacketTableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL getNext
);

extern CMD_STATUS wrCpssExMxNetIfRxPacketTableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL getNext
);

/*******************************************************************************
* wrCpssExMxPmNetIfRxPacketTableGet (table command)
*
* DESCRIPTION:
*     Get first/next entry from the DXCH rxNetIf table. (always "TO_CPU" dsa tag !)
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*     getNext            - GT_TRUE - "get next"
*                          GT_FALSE - "get first"
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfRxPacketTableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL getNext
)
{
    GT_STATUS rc = GT_OK;
#if defined EXMXPM_FAMILY
    rc = wrCpssExMxPmNetIfRxPacketTableGet(inArgs, inFields,
                                           numFields, outArgs, getNext);
#endif

#if defined EX_FAMILY
    rc = wrCpssExMxNetIfRxPacketTableGet(inArgs, inFields,
                                         numFields, outArgs, getNext);
#endif
    return rc;
}

/*******************************************************************************
* wrCmdCpssGenRxStartCapture (general command)
*
* DESCRIPTION:
*     Start collecting the received packets
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfRxStartCapture
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssRxStartCapture();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCmdCpssGenRxStopCapture (general command)
*
* DESCRIPTION:
*     Stop collecting the received packets
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfRxStopCapture
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssRxStopCapture();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCmdExMxPmRxSetMode (general command)
*
* DESCRIPTION:
*     set received packets collection mode and parameters
*
* INPUTS:
*     [0] GALTIS_RX_MODE_ENT lRxMode -
*          collection mode
*
*     [1] GT_U32 lBuferSize -
*          buffer size
*
*     [2] GT_U32 lNumOfEnteries -
*          number of entries
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfRxSetMode
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GALTIS_RX_MODE_ENT lRxMode;
    GT_U32 lBuferSize;
    GT_U32 lNumOfEnteries;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lRxMode = (GALTIS_RX_MODE_ENT)inArgs[0];
    lBuferSize = (GT_U32)inArgs[1];
    lNumOfEnteries = (GT_U32)inArgs[2];

    /* call tapi api function */
    result = cmdCpssRxSetMode(lRxMode, lBuferSize, lNumOfEnteries);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCmdExMxPmRxGetMode (general command)
*
* DESCRIPTION:
*     Get received packets collection mode and parameters
*
* INPUTS:
*     none
*
* OUTPUTS:
*     [0] GALTIS_RX_MODE_ENT lMode -
*          GT_RX_CYCLIC or GT_RX_ONCE
*
*     [1] GT_U32 lBuffSize -
*          packet buffer size
*
*     [2] GT_U32 lNumEntries -
*          number of entries
*
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfRxGetMode
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GALTIS_RX_MODE_ENT lMode;
    GT_U32 lBuffSize;
    GT_U32 lNumEntries;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssRxGetMode(&lMode, &lBuffSize, &lNumEntries);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", lMode, lBuffSize,
        lNumEntries);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmNetIfRxPacketTableGetFirst (table command)
*
* DESCRIPTION:
*     Get first entry from the DXCH rxNetIf table. (always "TO_CPU" dsa tag !)
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfRxPacketTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssGenNetIfRxPacketTableGet(inArgs,inFields,numFields,outArgs,
                    GT_FALSE);/* get first */
}

/*******************************************************************************
* wrCpssExMxPmNetIfRxPacketTableGetNext (table command)
*
* DESCRIPTION:
*     Get next entry from the DXCH rxNetIf table. (always "TO_CPU" dsa tag !)
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenNetIfRxPacketTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssGenNetIfRxPacketTableGet(inArgs,inFields,numFields,outArgs,
                    GT_TRUE);/* get next */
}

/*******************************************************************************
* wrCpssExMxPmNetIfRxPacketTableClear (table command)
*
* DESCRIPTION:
*     clear all entries from the DXCH rxNetIf table.
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssGenNetIfRxPacketTableClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdCpssRxPktClearTbl();
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/**** database initialization **************************************/


static CMD_COMMAND dbCommands[] =
{
        /* Rx packets table -- start */
        {"cpssGenNetIfRxStartCapture",
            &wrCpssGenNetIfRxStartCapture,
            0, 0},

        {"cpssGenNetIfRxStopCapture",
            &wrCpssGenNetIfRxStopCapture,
            0, 0},

        {"cpssGenNetIfRxSetMode",
            &wrCpssGenNetIfRxSetMode,
            3, 0},

        {"cpssGenNetIfRxGetMode",
            &wrCpssGenNetIfRxGetMode,
            0, 0},

        {"cpssGenNetIfRxPacketTableGetFirst",/* get first */
            &wrCpssGenNetIfRxPacketTableGetFirst,
            0, 0},

        {"cpssGenNetIfRxPacketTableGetNext",
            &wrCpssGenNetIfRxPacketTableGetNext,/* get next */
            0, 0},

        {"cpssGenNetIfRxPacketTableClear",
            &wrCpssGenNetIfRxPacketTableClear,/* clear */
            0, 0}
        /* Rx packets table -- End */

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

extern GT_STATUS cmdLibInitCpssGenTxNetIf
(
    GT_VOID
);

/*******************************************************************************
* cmdLibResetCpssGenNetIf
*
* DESCRIPTION:
*     Library database reset function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID cmdLibResetCpssGenNetIf
(
    GT_VOID
)
{
    cmdCpssRxStopCapture();
    cmdCpssRxPktClearTbl();
}

/*******************************************************************************
* cmdLibInitCpssExMxPmNetIf
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
GT_STATUS cmdLibInitCpssGenNetIf
(
    GT_VOID
)
{
    GT_STATUS rc;

    rc = cmdLibInitCpssGenTxNetIf();
    if(rc != GT_OK)
        return rc;

    rc = wrCpssRegisterResetCb(cmdLibResetCpssGenNetIf);
    if(rc != GT_OK)
    {
        return rc;
    }

    return cmdInitLibrary(dbCommands, numCommands);
}

