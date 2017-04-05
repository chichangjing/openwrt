/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssGenEvents.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <galtisAgent/wrapCpss/Gen/Events/wrapCpssGenEvents.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>

#define MAX_NUM_DEVICES 128

/* debug flag to open trace of events */
GT_U32 wrapCpssTraceEvents = 0;

/* Event Counter DB for PP, FA and XBAR */
/* The size of counter DB per device is sum of PP, FA and XBAR unified  */
/* events, for convenience. */
static GT_U32   uniEvIndex, endUniIndex;
static GT_U8    devNum;

#define END_OF_TABLE_MAC    0x7fffffff
static struct{
    GT_32 oldIndex;/* integer on purpose */
    GT_32 newIndex;/* integer on purpose */
} convertGaltisIndexToUniEvIndexArr[] =
{
    {189 , CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E},/* CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E  */
    {228 , CPSS_PP_PCL_TCC_ECC_ERR_E},/*CPSS_PP_PCL_TCC_ECC_ERR_E */
    {235 , CPSS_PP_CNC_WRAPAROUND_BLOCK_E},/*CPSS_PP_CNC_WRAPAROUND_BLOCK_E */
    {333 , CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E},/*CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E*/

    {END_OF_TABLE_MAC,END_OF_TABLE_MAC}/* must be last */
};

/* base values for XBAR, FA and DRAGONITE events in Galtis GUI */
#define PRV_GALTIS_EVENTS_XBAR_BASE_CNS         2000
#define PRV_GALTIS_EVENTS_FA_BASE_CNS           3000
#define PRV_GALTIS_EVENTS_DRAGONITE_BASE_CNS    4000

/*******************************************************************************
* appDemoGenEventCounterGet
*
* DESCRIPTION:
*     get the number of times that specific event happened.
*
* INPUTS:
*     devNum      - device number
*     uniEvent    - unified event
*     clearOnRead - do we 'clear' the counter after 'read' it
*               GT_TRUE - set counter to 0 after get it's value
*               GT_FALSE - don't update the counter (only read it)
* OUTPUTS:
*     counterPtr  - (pointer to)the counter (the number of times that specific event happened)
*
* RETURNS:
*     GT_OK              - on success.
*     GT_BAD_PARAM       - on bad devNum or uniEvent.
*     GT_BAD_PTR         - on NULL pointer.
*     GT_NOT_INITIALIZED - the counters DB not initialized for the device.
*
* COMMENTS:
*     none
*
*******************************************************************************/
extern GT_STATUS appDemoGenEventCounterGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN  GT_BOOL                  clearOnRead,
    OUT GT_U32                  *counterPtr
);

/*******************************************************************************
* convertCpssUniEvIndexToGaltisIndex
*
* DESCRIPTION:
*    when the events added to galtis the event of:
*    CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E had value of 189
*
*    but due to changes on the events that relate to 'per port PCS (XPCS)' and 'per port - per lane'
*    the 13 existing events reduce to 12 events  , so the event of
*    CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E has now value of 188 !!!
*
*    we need a function that will manage those 'WA' to support the Galtis.
*
* INPUTS:
*       cpssUniEvIndex - the cpss Unified index to convert
*       tableVersion - version of table
*
* OUTPUTS:
*       none
*
* RETURNS:
*       the modified Galtis index to use
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 convertCpssUniEvIndexToGaltisIndex
(
    IN GT_U32   cpssUniEvIndex,
    IN GT_U32   tableVersion
)
{
    GT_U32  ii;
    GT_32   offset = 0;/* integer on purpose */
    GT_32   origIndex = (GT_32)cpssUniEvIndex;/* integer on purpose */
    GT_32   finalIndex;/* integer on purpose */

    if(tableVersion)
    {
        /* All PP events in Galtis are same as in CPSS.
           All XBAR, FA and Dragonite events in Galtis are different from CPSS
           and starting from own base values. */
        finalIndex = origIndex;
        if((cpssUniEvIndex >= CPSS_XBAR_UNI_EV_MIN_E) && (cpssUniEvIndex <= CPSS_XBAR_UNI_EV_MAX_E))
        {
            finalIndex = cpssUniEvIndex - CPSS_XBAR_UNI_EV_MIN_E + PRV_GALTIS_EVENTS_XBAR_BASE_CNS;
        }
        else if((cpssUniEvIndex >= CPSS_FA_UNI_EV_MIN_E) && (cpssUniEvIndex <= CPSS_FA_UNI_EV_MAX_E))
        {
            finalIndex = cpssUniEvIndex - CPSS_FA_UNI_EV_MIN_E + PRV_GALTIS_EVENTS_FA_BASE_CNS;
        }
        else if((cpssUniEvIndex >= CPSS_DRAGONITE_UNI_EV_MIN_E) && (cpssUniEvIndex <= CPSS_DRAGONITE_UNI_EV_MAX_E))
        {
            finalIndex = cpssUniEvIndex - CPSS_DRAGONITE_UNI_EV_MIN_E + PRV_GALTIS_EVENTS_DRAGONITE_BASE_CNS;
        }
    }
    else
    {
        ii = 0;
        while(convertGaltisIndexToUniEvIndexArr[ii].oldIndex != END_OF_TABLE_MAC)
        {
            if(origIndex >= convertGaltisIndexToUniEvIndexArr[ii].newIndex)
            {
                offset += (convertGaltisIndexToUniEvIndexArr[ii].oldIndex -
                           convertGaltisIndexToUniEvIndexArr[ii].newIndex);
            }

            ii++;
        }
                                 /* offset may be negative */
        finalIndex = origIndex + offset;

        if(finalIndex < 0)
        {
            /* error */
            finalIndex = 0;
        }

        if(offset != 0)
        {
            cmdOsPrintf("cpssUniEvIndex = 0x%x , %d\n",cpssUniEvIndex,cpssUniEvIndex);
        }
    }

    return (GT_U32)finalIndex;
}

/*******************************************************************************
* prv_wrCpssGenEventTableGetFirst (table command)
*
* DESCRIPTION:
*     get first event counter entry.
*
* INPUTS:
*      tableVersion - version of table
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
static CMD_STATUS prv_wrCpssGenEventTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_U32  tableVersion
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       counter = 0;

    GT_U32      devType;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    devType = (GT_U32)inArgs[1];

    if(devType > 2)
        return CMD_AGENT_ERROR;


    if(devNum >= MAX_NUM_DEVICES ||
       GT_NOT_INITIALIZED == appDemoGenEventCounterGet(devNum, 0/*not relevant*/, GT_FALSE, NULL))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(devType == 0) /* In case of PP,  the first and last index*/
    {
        uniEvIndex  = CPSS_PP_UNI_EV_MIN_E;
        endUniIndex = CPSS_PP_UNI_EV_MAX_E + 1; /* we should increment the
                            endUniIndex in order not to loose the last event */
    }
    else if(devType == 1) /* In case of XBAR,  the first and last index*/
    {
        uniEvIndex  = CPSS_XBAR_UNI_EV_MIN_E;
        endUniIndex = CPSS_XBAR_UNI_EV_MAX_E + 1; /* we should increment the
                            endUniIndex in order not to loose the last event */
    }
    else /* In case of FA,  the first and last index*/
    {
        uniEvIndex  = CPSS_FA_UNI_EV_MIN_E;
        endUniIndex = CPSS_FA_UNI_EV_MAX_E + 1; /* we should increment the
                            endUniIndex in order not to loose the last event */
    }

    while(uniEvIndex < endUniIndex)
    {
        rc = appDemoGenEventCounterGet(devNum, uniEvIndex, GT_FALSE, &counter);
        if(GT_OK != rc || 0 != counter)
        {
            break;
        }

        uniEvIndex++;
    }

    if(uniEvIndex < endUniIndex && GT_OK == rc)
    {
        /* update the index sent to galtis with the WA of modifications in the cpssEvents enum
           Vs. the static Galtis enum */
        inFields[0] = convertCpssUniEvIndexToGaltisIndex(uniEvIndex, tableVersion);
        inFields[1] = counter;

        /* pack and output table fields */
        fieldOutput("%d%d", inFields[0], inFields[1]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssGenEventTableGetFirst (table command)
*
* DESCRIPTION:
*     get first event counter entry.
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
static CMD_STATUS wrCpssGenEventTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return prv_wrCpssGenEventTableGetFirst(inArgs, inFields, numFields, outArgs, 0 /* version */);
}


/*******************************************************************************
* wrCpssGenEventTableGetFirst_1 (table command)
*
* DESCRIPTION:
*     get first event counter entry.
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
static CMD_STATUS wrCpssGenEventTableGetFirst_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return prv_wrCpssGenEventTableGetFirst(inArgs, inFields, numFields, outArgs, 1 /* version */);
}

/*******************************************************************************
* prv_wrCpssGenEventTableGetNext (table command)
*
* DESCRIPTION:
*     get next event counter entry.
*
* INPUTS:
*      tableVersion - version of table
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
static CMD_STATUS prv_wrCpssGenEventTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_U32  tableVersion
)
{
    GT_STATUS    rc = GT_OK;
    GT_U32       counter = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    uniEvIndex++;


    while(uniEvIndex < endUniIndex)
    {
        rc = appDemoGenEventCounterGet(devNum, uniEvIndex, GT_FALSE, &counter);
        if(GT_OK != rc || 0 != counter)
        {
            break;
        }

        uniEvIndex++;
    }

    if(uniEvIndex < endUniIndex && GT_OK == rc)
    {
        /* update the index sent to galtis with the WA of modifications in the cpssEvents enum
           Vs. the static Galtis enum */
        inFields[0] = convertCpssUniEvIndexToGaltisIndex(uniEvIndex, tableVersion);
        inFields[1] = counter;

        /* pack and output table fields */
        fieldOutput("%d%d", inFields[0], inFields[1]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssGenEventTableGetNext (table command)
*
* DESCRIPTION:
*     get next event counter entry.
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
static CMD_STATUS wrCpssGenEventTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{    
    return prv_wrCpssGenEventTableGetNext(inArgs, inFields, numFields, outArgs, 0 /* version */);
}

/*******************************************************************************
* wrCpssGenEventTableGetNext_1 (table command)
*
* DESCRIPTION:
*     get next event counter entry.
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
static CMD_STATUS wrCpssGenEventTableGetNext_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{    
    return prv_wrCpssGenEventTableGetNext(inArgs, inFields, numFields, outArgs, 1 /* version */);
}

/*******************************************************************************
* wrCpssExMxPmNetIfRxPacketTableClear (table command)
*
* DESCRIPTION:
*     clear all entries in event counter entry block.
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
static CMD_STATUS wrCpssGenEventTableClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    rc = GT_OK;

    GT_U8       devNum;
    GT_U32      devType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    devType = (GT_U32)inArgs[1];

    if(devType > 2)
        return CMD_AGENT_ERROR;


    if(devNum >= MAX_NUM_DEVICES ||
       GT_NOT_INITIALIZED == appDemoGenEventCounterGet(devNum, 0/*not relevant*/, GT_FALSE, NULL))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(devType == 0) /* In case of PP,  the first and last index*/
    {
        uniEvIndex  = CPSS_PP_UNI_EV_MIN_E;
        endUniIndex = CPSS_PP_UNI_EV_MAX_E;
    }
    else if(devType == 1) /* In case of XBAR,  the first and last index*/
    {
        uniEvIndex  = CPSS_XBAR_UNI_EV_MIN_E;
        endUniIndex = CPSS_XBAR_UNI_EV_MAX_E;
    }
    else /* In case of FA,  the first and last index*/
    {
        uniEvIndex  = CPSS_FA_UNI_EV_MIN_E;
        endUniIndex = CPSS_FA_UNI_EV_MAX_E;
    }

    endUniIndex++;/* to have ability to 'clear' the last even too */

    while(uniEvIndex < endUniIndex)
    {
        rc = appDemoGenEventCounterGet(devNum, uniEvIndex, GT_TRUE, NULL);
        if(GT_OK != rc )
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        uniEvIndex++;
    }


    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssEventDeviceMaskSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device - in HW.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad value on one of the parameters
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
static CMD_STATUS wrCpssEventDeviceMaskSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                    devNum;
    CPSS_UNI_EV_CAUSE_ENT    uniEvent;
    CPSS_EVENT_MASK_SET_ENT  operation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    uniEvent    = (CPSS_UNI_EV_CAUSE_ENT)inArgs[1];
    operation   = (CPSS_EVENT_MASK_SET_ENT)inArgs[2];

    rc = cpssEventDeviceMaskSet(devNum, uniEvent, operation);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssEventDeviceMaskWithEvExtDataSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device , for
*       specific element in the event associated with extra data.
*       Since Each unified event may be associated with multiple HW interrupts,
*       each HW interrupt has different 'extra data' in the context of the
*       relevant unified event
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                     queue number / other ) the event was received upon.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device Or the 'extra data' has value that not relate
*                      to the uniEvent
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
static CMD_STATUS wrCpssEventDeviceMaskWithEvExtDataSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                    devNum;
    CPSS_UNI_EV_CAUSE_ENT    uniEvent;
    GT_U32                   evExtData;
    CPSS_EVENT_MASK_SET_ENT  operation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    uniEvent    = (CPSS_UNI_EV_CAUSE_ENT)inArgs[1];
    evExtData   = (GT_U32)inArgs[2];
    operation   = (CPSS_EVENT_MASK_SET_ENT)inArgs[3];

    rc = cpssEventDeviceMaskWithEvExtDataSet(devNum, uniEvent, evExtData, operation);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/**** database initialization **************************************/


static CMD_COMMAND dbCommands[] =
{
    {"cpssGenEventTableGetFirst",
        &wrCpssGenEventTableGetFirst,
        2, 0},

    {"cpssGenEventTableGetNext",
        &wrCpssGenEventTableGetNext,
        2, 0},

    {"cpssGenEventTableClear",
        &wrCpssGenEventTableClear,
        2, 0},

    /* Version 1 */
    {"cpssGenEventTable_1GetFirst",
        &wrCpssGenEventTableGetFirst_1,
        2, 0},

    {"cpssGenEventTable_1GetNext",
        &wrCpssGenEventTableGetNext_1,
        2, 0},

    {"cpssGenEventTable_1Clear",
        &wrCpssGenEventTableClear,/* clear same as version 0 */
        2, 0},

    {"cpssEventDeviceMaskSet",
        &wrCpssEventDeviceMaskSet,
        3, 0},

    {"cpssEventDeviceMaskWithEvExtDataSet",
        &wrCpssEventDeviceMaskWithEvExtDataSet,
        4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssGenEvents
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
GT_STATUS cmdLibInitCpssGenEvents
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

