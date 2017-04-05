/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* wrapCpssDxChDiagDataIntegrity.c
*
* DESCRIPTION:
*       Wrapper functions for Diag Data Integrity CPSS DxCh functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>
#include <galtisAgent/wrapCpss/dxCh/diag/wrapCpssDxChDiagDataIntegrity.h>

#define MAX_NUM_DEVICES_CNS 128

#define CMD_DXCH_DIAG_DATA_INTEGRITY_BANK_NUM_CNS 2
#define CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_NUM_CNS 2
#define CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_DB_SIZE_CNS (CMD_DXCH_DIAG_DATA_INTEGRITY_BANK_NUM_CNS*CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_NUM_CNS*8*2/*Single and Multiple*/)

#define CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_DB_SIZE_CNS (2/*IP/PCL*/ * 2/*X/Y*/ * 8/*potgGroup*/ * 16384/*arrayAddress*/ * 4/*rules in line*/)

typedef struct
{
    GT_U16 eventCounterArr[BIT_17];
    GT_U16 memTypeArr[BIT_17];
    GT_U8  origPortGroupId[BIT_17];
} CMD_DXCH_DIAG_DATA_INTEGRITY_COUNTER_DB_STC;


typedef struct
{
    GT_U16 eventCounterArr[CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_DB_SIZE_CNS];
    GT_U8  origPortGroupId[CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_DB_SIZE_CNS];
} CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_COUNTER_DB_STC;


typedef struct
{
    GT_U32 eventCounterArr[CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_DB_SIZE_CNS];
} CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_COUNTER_DB_STC;


/*
 * Typedef: enum CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ENT
 *
 * Description:
 *      This enum defines error cause type display filter
 * Enumerations:
 *      CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ALL_E - get all events.
 *      CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_SINGLE_ECC_E - get single ECC events.
 *      CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_MULTIPLE_ECC_E - get multiple ECC events.
 *      CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_PARITY_E - get parity events. 
 * 
 *  Comments:
 */
typedef enum{
    CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ALL_E,
    CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_SINGLE_ECC_E,
    CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_MULTIPLE_ECC_E,
    CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_PARITY_E
}CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ENT;

static CMD_DXCH_DIAG_DATA_INTEGRITY_COUNTER_DB_STC *cmdDataIntegrityEventsDbPtr = NULL;
static CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_COUNTER_DB_STC *cmdDataIntegrityMppmEventsDbPtr = NULL;
static CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_COUNTER_DB_STC *cmdDataIntegrityTcamEventsDbPtr = NULL;
static GT_BOOL isDataIntegrityInitDone = GT_FALSE;

/* debug flag to open trace of events */
GT_U32 wrapCpssDxChTraceEvents = 0;

GT_U32 currentDbKey = 0;
GT_U32 currentMppmDbKey = 0;
GT_U32 currentTcamDbKey = 0;
GT_U32 endDbKey = 0x20000; /* DB size */

/*******************************************************************************
* wrapCpssDxChDataIntegrityTraceEnable
*
*   DESCRIPTION:
*       Routine to enable trace 
*
*   INPUTS:
*       enable - enable/disable trace output
*
*   OUTPUTS:
*       None
*
*   RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void wrapCpssDxChDataIntegrityTraceEnable
(
    IN GT_U32 enable
)
{
    wrapCpssDxChTraceEvents = enable;
    return;
}

/*******************************************************************************
* wrCpssDxChDataIntegrityEventCounterIncrement
*
*   DESCRIPTION:
*       Function for increment counter per data integrity event
*
*   INPUTS:
*       devNum      - device number
*       eventPtr    - (pointer to) data integrity event structure
*
*   OUTPUTS:
*       None
*
*   RETURNS:
*       CMD_OK            - on success.
*       GT_BAD_PARAM      - on wrong values of input parameters.
*       GT_OUT_OF_CPU_MEM - on out of CPU memory
*
* COMMENTS:
*       Called from cpssEnabler.
*       First call is init DB, eventPtr = NULL
*******************************************************************************/
static GT_STATUS wrCpssDxChDiagDataIntegrityEventCounterIncrement
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC     *eventPtr
)
{
    GT_U32 key = 0; /* key index to DB */

    if(devNum >= MAX_NUM_DEVICES_CNS)
    {
        /* check device out of range */
        return GT_BAD_PARAM;
    }

    if(isDataIntegrityInitDone == GT_FALSE)
    {
        isDataIntegrityInitDone = GT_TRUE;
        /* first call - DB initialization only */
        cmdDataIntegrityEventsDbPtr = (CMD_DXCH_DIAG_DATA_INTEGRITY_COUNTER_DB_STC*)cmdOsMalloc(sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_COUNTER_DB_STC));
        cmdDataIntegrityMppmEventsDbPtr = (CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_COUNTER_DB_STC*)cmdOsMalloc(sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_COUNTER_DB_STC));
        cmdDataIntegrityTcamEventsDbPtr = (CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_COUNTER_DB_STC*)cmdOsMalloc(sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_COUNTER_DB_STC));

        if((cmdDataIntegrityEventsDbPtr == NULL) || (cmdDataIntegrityMppmEventsDbPtr == NULL) || (cmdDataIntegrityTcamEventsDbPtr == NULL))
        {
            return GT_OUT_OF_CPU_MEM;
        }
        else
        {
            cmdOsMemSet(cmdDataIntegrityEventsDbPtr, 0, sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_COUNTER_DB_STC));
            cmdOsMemSet(cmdDataIntegrityMppmEventsDbPtr, 0, sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_COUNTER_DB_STC));
            cmdOsMemSet(cmdDataIntegrityTcamEventsDbPtr, 0, sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_COUNTER_DB_STC));
            return GT_OK;
        }
    }
    else
    {
        CPSS_NULL_PTR_CHECK_MAC(eventPtr);

        if((eventPtr->memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E) ||
           (eventPtr->memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E))
        {
            if((eventPtr->location.mppmMemLocation.mppmId > 1) ||
               (eventPtr->location.mppmMemLocation.bankId > 1))
            {
                return GT_BAD_PARAM;
            }

            key = eventPtr->location.mppmMemLocation.mppmId;
            key |= (eventPtr->location.mppmMemLocation.bankId << 1);
            if(eventPtr->eventsType == CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E)
            {
                key |= (0 << 2);
            }
            else if(eventPtr->eventsType == CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E)
            {
                key |= (1 << 2);
            }
            else
            {
                return GT_BAD_PARAM;
            }
            key |= (eventPtr->causePortGroupId << 3);

            cmdDataIntegrityMppmEventsDbPtr->origPortGroupId[key] = (GT_U8)eventPtr->causePortGroupId;
            cmdDataIntegrityMppmEventsDbPtr->eventCounterArr[key] += 1;

            return GT_OK;
        }
        else if((eventPtr->memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E) ||
                (eventPtr->memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E))
        {
            /*
                Key Structure:
                    bits[0:10] - TCAM line index for Router TCAM, PCL rule index for policy TCAM
                    bits[11] - array type X/Y
                    bits[12:14] - portGroup
                    bits[15] - Policy or Router
            */
            key = eventPtr->location.tcamMemLocation.ruleIndex;

            if(eventPtr->location.tcamMemLocation.arrayType == CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E)
            {
                key |= (0 << 11);
            }
            else if(eventPtr->location.tcamMemLocation.arrayType == CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E)
            {
                key |= (1 << 11);
            }
            else
            {
                return GT_BAD_PARAM;
            }

            key |= (eventPtr->causePortGroupId << 12);

            if(eventPtr->memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E)
            {
                key |= (0 << 15);
            }
            else
            {
                key |= (1 << 15);
            }

            cmdDataIntegrityTcamEventsDbPtr->eventCounterArr[key] += 1;

            return GT_OK;
        }
        else 
        {
            /* calculate index for DB access */
            key = eventPtr->location.memLocation.dfxMemoryId;
            key |= (eventPtr->location.memLocation.dfxClientId << 7);
            key |= (eventPtr->location.memLocation.dfxPipeId << 12);
        }

        switch(eventPtr->eventsType)
        {
            case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E:
                U32_SET_FIELD_MAC(key, 15, 2, 1);
                break;
            case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E:
                U32_SET_FIELD_MAC(key, 15, 2, 2);
                break;
            case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E:
                U32_SET_FIELD_MAC(key, 15, 2, 3);
                break;
            default:
                return GT_BAD_PARAM;
        }

        if (wrapCpssDxChTraceEvents)
        {
            cpssOsPrintf("eventPtr->memLocation.dfxPipeId = 0x%x\r\n", U32_GET_FIELD_MAC(key, 12 , 3));
            cpssOsPrintf("eventPtr->memLocation.dfxClientId = 0x%x\r\n", U32_GET_FIELD_MAC(key, 7 , 5));
            cpssOsPrintf("eventPtr->memLocation.dfxMemoryId = %d\r\n", U32_GET_FIELD_MAC(key, 0 , 7));
            cpssOsPrintf("eventPtr->eventsType = 0x%x\r\n", eventPtr->eventsType);
            cpssOsPrintf("eventPtr->memType = 0x%x\r\n", eventPtr->memType);
            cpssOsPrintf("key = 0x%x\r\n", key);
        }

        cmdDataIntegrityEventsDbPtr->eventCounterArr[key] += 1;
        cmdDataIntegrityEventsDbPtr->memTypeArr[key] = eventPtr->memType;
        cmdDataIntegrityEventsDbPtr->origPortGroupId[key] = (GT_U8)eventPtr->causePortGroupId;
    }

    return GT_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityCountersCbGet
*
*   DESCRIPTION:
*       Routine to bind a CB function that receives block of
*       data integrity event counters for given device.
*
*   INPUTS:
*       dataIntegrityEventCounterBlockGetCB - callback function
*
*   OUTPUTS:
*       None
*
*   RETURNS:
*       CMD_OK            - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS wrCpssDxChDiagDataIntegrityCountersCbGet
(
    DXCH_DATA_INTEGRITY_EVENT_COUNTER_INCREMENT_FUNC **dataIntegrityEventCounterBlockGetCB
)
{
    *dataIntegrityEventCounterBlockGetCB = wrCpssDxChDiagDataIntegrityEventCounterIncrement;
    return GT_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityEventTableGetFirst (table command)
*
* DESCRIPTION:
*     get first data integrity event counter entry.
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityEventTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;
    CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ENT  filterType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT       errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
    GT_BOOL filterEnable = GT_TRUE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    filterType = (CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ENT)inArgs[1];

    if(filterType > CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_PARITY_E)
        return CMD_AGENT_ERROR;


    if(devNum >= MAX_NUM_DEVICES_CNS || cmdDataIntegrityEventsDbPtr == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    currentDbKey = 0;

    while(currentDbKey < endDbKey)
    {
        switch((currentDbKey >> 15) & 0x3)
        {
            case 1:
                errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                if(filterType == CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_PARITY_E)
                {
                    filterEnable = GT_FALSE;
                }
                break;
            case 2:
                errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                if(filterType == CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_SINGLE_ECC_E)
                {
                    filterEnable = GT_FALSE;
                }
                break;
            case 3:
                errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                if(filterType == CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_MULTIPLE_ECC_E)
                {
                    filterEnable = GT_FALSE;
                }
                break;
            default:
                filterEnable = GT_TRUE;
                break;
        }
        if(filterType == CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ALL_E)
        {
            filterEnable = GT_FALSE;
        }

        if((cmdDataIntegrityEventsDbPtr->eventCounterArr[currentDbKey] == 0) || (filterEnable == GT_TRUE))
        {
            currentDbKey++;
            continue;
        }
        else
        {
            break;
        }

    }

    if(currentDbKey < endDbKey)
    {
        inFields[0] = errorType;
        inFields[1] = cmdDataIntegrityEventsDbPtr->memTypeArr[currentDbKey];
        inFields[2] = cmdDataIntegrityEventsDbPtr->origPortGroupId[currentDbKey];
        inFields[3] = U32_GET_FIELD_MAC(currentDbKey, 12, 3);
        inFields[4] = U32_GET_FIELD_MAC(currentDbKey, 7, 5);
        inFields[5] = U32_GET_FIELD_MAC(currentDbKey, 0, 7);
        inFields[6] = cmdDataIntegrityEventsDbPtr->eventCounterArr[currentDbKey];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityEventTableGetNext (table command)
*
* DESCRIPTION:
*     get next data integrity event counter entry.
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityEventTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ENT  filterType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT       errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
    GT_BOOL filterEnable = GT_TRUE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    filterType = (CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ENT)inArgs[1];

    currentDbKey++;

    while(currentDbKey < endDbKey)
    {
        switch((currentDbKey >> 15) & 0x3)
        {
            case 1:
                errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                if(filterType == CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_PARITY_E)
                {
                    filterEnable = GT_FALSE;
                }
                break;
            case 2:
                errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                if(filterType == CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_SINGLE_ECC_E)
                {
                    filterEnable = GT_FALSE;
                }
                break;
            case 3:
                errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                if(filterType == CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_MULTIPLE_ECC_E)
                {
                    filterEnable = GT_FALSE;
                }
                break;
            default:
                filterEnable = GT_TRUE;
                break;
        }
        if(filterType == CMD_DXCH_DIAG_DATA_INTEGRITY_EVENT_TYPE_FILTER_ALL_E)
        {
            filterEnable = GT_FALSE;
        }

        if((cmdDataIntegrityEventsDbPtr->eventCounterArr[currentDbKey] == 0) || (filterEnable == GT_TRUE))
        {
            currentDbKey++;
            continue;
        }
        else
        {
            break;
        }

    }

    if(currentDbKey < endDbKey)
    {
        inFields[0] = errorType;
        inFields[1] = cmdDataIntegrityEventsDbPtr->memTypeArr[currentDbKey];
        inFields[2] = cmdDataIntegrityEventsDbPtr->origPortGroupId[currentDbKey];
        inFields[3] = U32_GET_FIELD_MAC(currentDbKey, 12, 3);
        inFields[4] = U32_GET_FIELD_MAC(currentDbKey, 7, 5);
        inFields[5] = U32_GET_FIELD_MAC(currentDbKey, 0, 7);
        inFields[6] = cmdDataIntegrityEventsDbPtr->eventCounterArr[currentDbKey];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityEventTableClear (table command)
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityEventTableClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(devNum >= MAX_NUM_DEVICES_CNS || cmdDataIntegrityEventsDbPtr == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(cmdDataIntegrityEventsDbPtr, 0, sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_COUNTER_DB_STC));

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityMppmEventTableGetFirst (table command)
*
* DESCRIPTION:
*     get first data integrity event counter entry.
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityMppmEventTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT       errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(devNum >= MAX_NUM_DEVICES_CNS || cmdDataIntegrityMppmEventsDbPtr == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    currentMppmDbKey = 0;

    while(currentMppmDbKey < CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_DB_SIZE_CNS)
    {
        if(cmdDataIntegrityMppmEventsDbPtr->eventCounterArr[currentMppmDbKey] == 0)
        {
            currentMppmDbKey++;
            continue;
        }
        else
        {
            break;
        }
    }

    if(currentMppmDbKey < CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_DB_SIZE_CNS)
    {
        if(U32_GET_FIELD_MAC(currentMppmDbKey, 2, 1) == 1)
        {
            errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
        }
        else
        {
            errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
        }

        inFields[0] = errorType;
        inFields[1] = cmdDataIntegrityMppmEventsDbPtr->origPortGroupId[currentMppmDbKey];
        inFields[2] = U32_GET_FIELD_MAC(currentMppmDbKey, 0, 1);
        inFields[3] = U32_GET_FIELD_MAC(currentMppmDbKey, 1, 1);
        inFields[4] = cmdDataIntegrityMppmEventsDbPtr->eventCounterArr[currentMppmDbKey];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityEventTableGetNext (table command)
*
* DESCRIPTION:
*     get next data integrity event counter entry.
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityMppmEventTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT       errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    currentMppmDbKey++;

    while(currentMppmDbKey < CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_DB_SIZE_CNS)
    {
        if(cmdDataIntegrityMppmEventsDbPtr->eventCounterArr[currentMppmDbKey] == 0)
        {
            currentMppmDbKey++;
            continue;
        }
        else
        {
            break;
        }
    }

    if(currentMppmDbKey < CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_DB_SIZE_CNS)
    {
        if(U32_GET_FIELD_MAC(currentMppmDbKey, 2, 1) == 1)
        {
            errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
        }
        else
        {
            errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
        }

        inFields[0] = errorType;
        inFields[1] = cmdDataIntegrityMppmEventsDbPtr->origPortGroupId[currentMppmDbKey];
        inFields[2] = U32_GET_FIELD_MAC(currentMppmDbKey, 0, 1);
        inFields[3] = U32_GET_FIELD_MAC(currentMppmDbKey, 1, 1);
        inFields[4] = cmdDataIntegrityMppmEventsDbPtr->eventCounterArr[currentMppmDbKey];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityMppmEventTableClear (table command)
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityMppmEventTableClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(devNum >= MAX_NUM_DEVICES_CNS || cmdDataIntegrityMppmEventsDbPtr == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(cmdDataIntegrityMppmEventsDbPtr, 0, sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_MPPM_COUNTER_DB_STC));

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityTcamEventTableGetFirst (table command)
*
* DESCRIPTION:
*     get first data integrity event counter entry.
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityTcamEventTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType;
    CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT    arrayType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(devNum >= MAX_NUM_DEVICES_CNS || cmdDataIntegrityTcamEventsDbPtr == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    currentTcamDbKey = 0;

    while(currentTcamDbKey < CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_DB_SIZE_CNS)
    {
        if(cmdDataIntegrityTcamEventsDbPtr->eventCounterArr[currentTcamDbKey] == 0)
        {
            currentTcamDbKey++;
            continue;
        }
        else
        {
            break;
        }
    }

    if(currentTcamDbKey < CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_DB_SIZE_CNS)
    {
        if(U32_GET_FIELD_MAC(currentTcamDbKey, 15, 1) == 0)
        {
            memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E;
        }
        else
        {
            memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E;
        }

        if(U32_GET_FIELD_MAC(currentTcamDbKey, 11, 1) == 0)
        {
            arrayType = CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E;
        }
        else
        {
            arrayType = CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E;
        }


        inFields[0] = memType;
        inFields[1] = U32_GET_FIELD_MAC(currentTcamDbKey, 12, 3);
        inFields[2] = arrayType;
        inFields[3] = U32_GET_FIELD_MAC(currentTcamDbKey, 0, 11);
        inFields[4] = cmdDataIntegrityTcamEventsDbPtr->eventCounterArr[currentTcamDbKey];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDataIntegrityTcamEventTableGetNext (table command)
*
* DESCRIPTION:
*     get next data integrity event counter entry.
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityTcamEventTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType;
    CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT    arrayType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    currentTcamDbKey++;

    while(currentTcamDbKey < CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_DB_SIZE_CNS)
    {
        if(cmdDataIntegrityTcamEventsDbPtr->eventCounterArr[currentTcamDbKey] == 0)
        {
            currentTcamDbKey++;
            continue;
        }
        else
        {
            break;
        }
    }

    if(currentTcamDbKey < CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_DB_SIZE_CNS)
    {
        if(U32_GET_FIELD_MAC(currentTcamDbKey, 15, 1) == 0)
        {
            memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E;
        }
        else
        {
            memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E;
        }

        if(U32_GET_FIELD_MAC(currentTcamDbKey, 11, 1) == 0)
        {
            arrayType = CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E;
        }
        else
        {
            arrayType = CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E;
        }


        inFields[0] = memType;
        inFields[1] = U32_GET_FIELD_MAC(currentTcamDbKey, 12, 3);
        inFields[2] = arrayType;
        inFields[3] = U32_GET_FIELD_MAC(currentTcamDbKey, 0, 11);
        inFields[4] = cmdDataIntegrityTcamEventsDbPtr->eventCounterArr[currentTcamDbKey];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChDiagDataIntegrityTcamEventTableClear (table command)
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
static CMD_STATUS wrCpssDxChDiagDataIntegrityTcamEventTableClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(devNum >= MAX_NUM_DEVICES_CNS || cmdDataIntegrityTcamEventsDbPtr == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(cmdDataIntegrityTcamEventsDbPtr, 0, sizeof(CMD_DXCH_DIAG_DATA_INTEGRITY_TCAM_COUNTER_DB_STC));

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagDataIntegrityEventMaskSet
*
* DESCRIPTION:
*       Function sets mask/unmask for ECC/Parity event.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*       errorType   - type of error interrupt
*                     relevant only for ECC protected memories 
*       maskEnable  - mask/unmask interrupt
*                       GT_TRUE - mask interrupt
*                       GT_FALSE - unmask interrupt
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityEventMaskSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType;
    CPSS_EVENT_MASK_SET_ENT                         operation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];
    errorType = (CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT)inArgs[2];
    operation = (CPSS_EVENT_MASK_SET_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityEventMaskSet(devNum, memType, errorType, operation);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}



/*******************************************************************************
* cpssDxChDiagDataIntegrityEventMaskGet
*
* DESCRIPTION:
*       Function gets mask/unmask for ECC/Parity interrupt.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*                     
*
* OUTPUTS:
*       errorTypePtr   - (pointer to) type of error interrupt
*       maskEnablePtr  - (pointer to) mask/unmask interrupt
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityEventMaskGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType;
    CPSS_EVENT_MASK_SET_ENT                         operation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityEventMaskGet(devNum, memType, &errorType, &operation);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", errorType, operation);

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorInfoGet
*
* DESCRIPTION:
*       Function gets ECC/Parity error counter.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum         - PP device number
*       memLocationPtr - (pointer to) memory location indexes
*
* OUTPUTS:
*       errorCounterPtr    - (pointer to) error counter
*       failedRowPtr       - (pointer to) failed raw
*       failedSyndromePtr  - (pointer to) failed syndrome
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityErrorInfoGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType;
    CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT    location;
    GT_U32                                          portGroupId;
    GT_U32                                          dfxPipeId;
    GT_U32                                          dfxClientId;
    GT_U32                                          dfxMemoryId;
    GT_U32                                          mppmId;
    GT_U32                                          bankId;
    GT_U32                                          errorCounter;
    GT_U32                                          failedRaw;
    GT_U32                                          failedColumn;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];
    dfxPipeId = (GT_U32)inArgs[2];
    dfxClientId = (GT_U32)inArgs[3];
    dfxMemoryId = (GT_U32)inArgs[4];
    portGroupId = (GT_U32)inArgs[5];
    mppmId = (GT_U32)inArgs[6];
    bankId = (GT_U32)inArgs[7];

    if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E) ||
      (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E))
    {
        location.mppmMemLocation.portGroupId = portGroupId;
        location.mppmMemLocation.mppmId = mppmId;
        location.mppmMemLocation.bankId = bankId;
    }
    else
    {
        portGroupId = 1; /*DFX port group*/
        location.memLocation.dfxClientId = dfxClientId;
        location.memLocation.dfxMemoryId = dfxMemoryId;
        location.memLocation.dfxPipeId = dfxPipeId;
    }

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityErrorInfoGet(devNum, memType, &location, &errorCounter, &failedRaw, &failedColumn);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", errorCounter, failedRaw, failedColumn);

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorInjectionConfigSet
*
* DESCRIPTION:
*       Function enables/disable injection of error during next write operation.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum       - PP device number
*       memType      - type of memory(table)
*       injectMode   - error injection mode
*                      relevant only for ECC protected memories
*       injectEnable - enable/disable error injection
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType, injectMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Memory with parity can not be configured with multiple error injection
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityErrorInjectionConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT  injectMode;
    GT_BOOL                                         injectEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];
    injectMode = (CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT)inArgs[2];
    injectEnable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityErrorInjectionConfigSet(devNum, memType, injectMode, injectEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorInjectionConfigGet
*
* DESCRIPTION:
*       Function gets status of error injection.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum       - PP device number
*       memType      - type of memory(table)
*
* OUTPUTS:
*       injectModePtr   - (pointer to) error injection mode
*       injectEnablePtr - (pointer to) enable/disable error injection
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityErrorInjectionConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                           result;
    GT_U8                                               devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT          memType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT      injectMode;
    GT_BOOL                                             injectEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityErrorInjectionConfigGet(devNum, memType, &injectMode, &injectEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", injectMode, injectEnable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorCountEnableSet
*
* DESCRIPTION:
*       Function enables/disable error counter.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*       errorType   - error type
*       countEnable - enable/disable error counter
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityErrorCountEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType;
    GT_BOOL                                         countEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];
    errorType = (CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT)inArgs[2];
    countEnable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityErrorCountEnableSet(devNum, memType, errorType, countEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorCountEnableGet
*
* DESCRIPTION:
*       Function gets status of error counter.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*
* OUTPUTS:
*       errorTypePtr   - (pointer to) error type
*       countEnablePtr - (pointer to) status of error counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityErrorCountEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType;
    GT_BOOL                                         countEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityErrorCountEnableGet(devNum, memType, &errorType, &countEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", errorType, countEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityProtectionTypeGet
*
* DESCRIPTION:
*       Function gets memory protection type.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityProtectionTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                           result;
    GT_U8                                               devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT          memType;
    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  protectionType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityProtectionTypeGet(devNum, memType, &protectionType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", protectionType);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet
*
* DESCRIPTION:
*       Function enables/disables TCAM parity daemon.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*                     only ROUTER_TCAM and PCL_TCAM supported
*       enable      - GT_TRUE - enable daemon
*                     GT_FALSE - disable daemon
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityTcamParityDaemonEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT  memType;
    GT_BOOL                                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet(devNum, memType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet
*
* DESCRIPTION:
*       Function gets status of TCAM parity daemon.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*                     only ROUTER_TCAM and PCL_TCAM supported
*
* OUTPUTS:
*       enablePtr   - (pointer to) daemon status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDataIntegrityTcamParityDaemonEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT  memType;
    GT_BOOL                                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet(devNum, memType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChDiagDataIntegrityTcamEventCntTblGetFirst",
        &wrCpssDxChDiagDataIntegrityTcamEventTableGetFirst,
        1,0},

    {"cpssDxChDiagDataIntegrityTcamEventCntTblGetNext",
        &wrCpssDxChDiagDataIntegrityTcamEventTableGetNext,
        1,0},

    {"cpssDxChDiagDataIntegrityTcamEventCntTblClear",
        &wrCpssDxChDiagDataIntegrityTcamEventTableClear,
        1,0},

    {"cpssDxChDiagDataIntegrityMppmEventCntTblGetFirst",
        &wrCpssDxChDiagDataIntegrityMppmEventTableGetFirst,
        1,0},

    {"cpssDxChDiagDataIntegrityMppmEventCntTblGetNext",
        &wrCpssDxChDiagDataIntegrityMppmEventTableGetNext,
        1,0},

    {"cpssDxChDiagDataIntegrityMppmEventCntTblClear",
        &wrCpssDxChDiagDataIntegrityMppmEventTableClear,
        1,0},

    {"cpssDxChDiagDataIntegrityEventCntTblGetFirst",
        &wrCpssDxChDiagDataIntegrityEventTableGetFirst,
        2,0},

    {"cpssDxChDiagDataIntegrityEventCntTblGetNext",
        &wrCpssDxChDiagDataIntegrityEventTableGetNext,
        2,0},

    {"cpssDxChDiagDataIntegrityEventCntTblClear",
        &wrCpssDxChDiagDataIntegrityEventTableClear,
        1,0},

    {"cpssDxChDiagDataIntegrityEventMaskSet",
        &wrCpssDxChDiagDataIntegrityEventMaskSet,
        4, 0},

    {"cpssDxChDiagDataIntegrityEventMaskGet",
        &wrCpssDxChDiagDataIntegrityEventMaskGet,
        2, 0},

    {"cpssDxChDiagDataIntegrityErrorInfoGet",
        &wrCpssDxChDiagDataIntegrityErrorInfoGet,
        8, 0},

    {"cpssDxChDiagDataIntegrityErrorInjectionConfigSet",
        &wrCpssDxChDiagDataIntegrityErrorInjectionConfigSet,
        4, 0},

    {"cpssDxChDiagDataIntegrityErrorInjectionConfigGet",
        &wrCpssDxChDiagDataIntegrityErrorInjectionConfigGet,
        2, 0},

    {"cpssDxChDiagDataIntegrityErrorCountEnableSet",
        &wrCpssDxChDiagDataIntegrityErrorCountEnableSet,
        4, 0},

    {"cpssDxChDiagDataIntegrityErrorCountEnableGet",
        &wrCpssDxChDiagDataIntegrityErrorCountEnableGet,
        2, 0},

    {"cpssDxChDiagDataIntegrityProtectionTypeGet",
        &wrCpssDxChDiagDataIntegrityProtectionTypeGet,
        2, 0},

    {"cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet",
        &wrCpssDxChDiagDataIntegrityTcamParityDaemonEnableSet,
        3, 0},

    {"cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet",
        &wrCpssDxChDiagDataIntegrityTcamParityDaemonEnableGet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChDiagDataIntegrity
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
GT_STATUS cmdLibInitCpssDxChDiagDataIntegrity
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

