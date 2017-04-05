/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssLog.c
*
* DESCRIPTION:
*       CPSS log implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/cpssPresteraDefs.h>

/* buffer size for strings */
#define PRV_LOG_STRING_BUFFER_SIZE_CNS      2048
#define PRV_LOG_STRING_NAME_BUFFER_SIZE_CNS 160

#define PRV_CPSS_LOG_NOT_VALID_CNS   0xFFFFFFFF
/************************** Internal definitions  *****************************/
/*
 * Typedef: enum PRV_CPSS_LOG_PHASE_ENT
 *
 * Description:
 *      This enum describes API call log phase.
 * Enumerations:
 *      PRV_CPSS_LOG_PHASE_ENTER_E -
 *          before API call log phase
 *      PRV_CPSS_LOG_PHASE_EXIT_E -
 *          after API call log phase
 *      PRV_CPSS_LOG_PHASE_ERROR_E -
 *          on function error
 *      PRV_CPSS_LOG_PHASE_INFO_E -
 *          information during normal API processing
*/
typedef enum {
    PRV_CPSS_LOG_PHASE_ENTER_E,
    PRV_CPSS_LOG_PHASE_EXIT_E,
    PRV_CPSS_LOG_PHASE_ERROR_E,
    PRV_CPSS_LOG_PHASE_INFO_E
}PRV_CPSS_LOG_PHASE_ENT;

/*
 * Typedef: struct PRV_CPSS_LOG_API_STACK_PARAM_STC
 *
 * Description:
 *      This structure describes stored information for OUT or INOUT parameters.
 * Fields:
 *      paramValue - value of parameter. Only pointers need to be stored
 *      paramsPtr  - pointer of parameter's information for parsing.
 *
*/
typedef struct {
    GT_VOID_PTR                     paramValue;
    PRV_CPSS_LOG_FUNC_PARAM_STC    *paramsPtr;
}PRV_CPSS_LOG_API_STACK_PARAM_STC;

/* maximal number of parameters to store in stack DB */
#define PRV_CPSS_LOG_API_PARAM_MAX_NUM_CNS 20

/*
 * Typedef: struct PRV_CPSS_LOG_API_STACK_STC
 *
 * Description:
 *      This structure describes memory used in API exit logging logic.
 * Fields:
 *      functionId  - function identifier (of the function we just exit from).
 *      paramKey    - value of function's key for union and arrays parsing
 *      numOfParams - number of stored parameters
 *      paramsArr   - array of information about OUT or INOUT parameters
 *
*/
typedef struct {
    GT_U32                              functionId;
    PRV_CPSS_LOG_PARAM_KEY_INFO_STC     paramKey;
    GT_U32                              numOfParams;
    PRV_CPSS_LOG_API_STACK_PARAM_STC    paramsArr[PRV_CPSS_LOG_API_PARAM_MAX_NUM_CNS];
}PRV_CPSS_LOG_API_STACK_STC;

/*
 * Typedef: struct PRV_CPSS_LOG_API_HISTORY_ENTRY_STC
 *
 * Description:
 *      This structure describes API log history database entry.
 *
 * Fields:
 *      entryHistoryFileName -
 *          history log file name
 *      entryHistoryFile -
 *          history log file descriptor
 *      nextEntryPtr -
 *          pointer to next history entry
 *
 *
 */
typedef struct cpss_log_api_history_entry{
    char entryHistoryFileName[80];
    CPSS_OS_FILE entryHistoryFile;
    struct cpss_log_api_history_entry * nextEntryPtr;
}PRV_CPSS_LOG_API_HISTORY_ENTRY_STC;

/*******************************************************************************
* prvCpssLogTabs
*
* DESCRIPTION:
*       Log tab for recursive structures
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
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
void prvCpssLogTabs
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType
);

/************************** Variables definitions *****************************/
GT_BOOL prvCpssLogEnabled = GT_FALSE;
CPSS_LOG_API_FORMAT_ENT prvCpssLogFormat = CPSS_LOG_API_FORMAT_ALL_PARAMS_E;
GT_U32 prvCpssLogStackSize=0;
static GT_BOOL prvCpssLogLibsLogsTypesTable[CPSS_LOG_LIB_ALL_E][CPSS_LOG_TYPE_ALL_E];
GT_U32 prvCpssLogTabIndex = 0;
/* use a line number in some log messages */
static GT_BOOL prvCpssLogLineNumberEnabled = GT_TRUE;
/* format of pointer values in the log */
CPSS_LOG_POINTER_FORMAT_ENT prvCpssLogPointerFormat = CPSS_LOG_POINTER_FORMAT_SIMPLE_E;
/* Context of entrance support */
static GT_U32 prvCpssLogApiContextIndex = 0;
static CPSS_LOG_LIB_ENT prvCpssLogApiContextLib = CPSS_LOG_LIB_ALL_E;
static GT_U32  prvCpssLogEntryFuncId = PRV_CPSS_LOG_NOT_VALID_CNS;

/* Common use buffers */
static char buffer[PRV_LOG_STRING_BUFFER_SIZE_CNS];
static char buffer1[PRV_LOG_STRING_NAME_BUFFER_SIZE_CNS];
static char buffer2[PRV_LOG_STRING_NAME_BUFFER_SIZE_CNS];

/* Time formats */
#define CPSS_LOG_TIME_FORMAT_DATE_TIME_CNS              "%c"
#define CPSS_LOG_TIME_FORMAT_SHORT_DATE_TIME_CNS        "%m/%d/%y  %I:%M:%S %p"
#define CPSS_LOG_TIME_FORMAT_SHORT_DATE_ISO_TIME_CNS    "%m/%d/%y  %H:%M:%S"

/* Log numeric format */
const GT_CHAR *prvCpssLogFormatNumFormat            = "%s = %d";

/* String constants */
const GT_CHAR *prvCpssLogErrorMsgDeviceNotExist     = "PRV_CPSS_DEV_CHECK_MAC: Device %d not exist.\n";
const GT_CHAR *prvCpssLogErrorMsgFdbIndexOutOfRange = "PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC: Device %d, FDB index %d is out of range\n";
const GT_CHAR *prvCpssLogErrorMsgPortGroupNotValid  = "PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC: Multi-port group device %d, port group %d not valid\n";
const GT_CHAR *prvCpssLogErrorMsgPortGroupNotActive = "PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC: Multi-port group device %d, port group %d not active\n";
const GT_CHAR *prvCpssLogErrorMsgIteratorNotValid   = "__MAX_NUM_ITERATIONS_CHECK_CNS: Device %d, current iterator %d not valid %d\n";
const GT_CHAR *prvCpssLogErrorMsgPortGroupBitmapNotValid
                                                    = "PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC: Multi-port group device %d, port group bitmap %d not valid \n";
const GT_CHAR *prvCpssLogErrorMsgDeviceNotInitialized
                                                    = "PRV_CPSS_DRV_CHECK_PP_DEV_MAC: Device %d not initialized.\n";

/* Current time format  */
static CPSS_LOG_TIME_FORMAT_ENT timeFormat = CPSS_LOG_TIME_FORMAT_NO_DATE_TIME_E;

/* History log database */
static PRV_CPSS_LOG_API_HISTORY_ENTRY_STC firstHistoryDbEntry;
/* Pointer to current history log database entry */
static PRV_CPSS_LOG_API_HISTORY_ENTRY_STC * currentHistoryEntryPtr = 0;
/* Current number of entries in history log database */
static GT_U32 currentHistoryEntryNum = 0;

/* Maximal API stack size */
#define PRV_CPSS_LOG_API_STACK_SIZE_CNS     256
/* API call stack database */
PRV_CPSS_LOG_API_STACK_STC prvCpssLogApiStack[PRV_CPSS_LOG_API_STACK_SIZE_CNS];
GT_U32 prvCpssLogStackCurrentIndex = 0;

/* Log API parameters and save history in database */
#define PRV_CPSS_LOG_AND_HISTORY_MAC(log_phase, log_lib, log_type, ...) \
    prvCpssLogAndHistory(log_phase, log_lib, log_type, __VA_ARGS__)

/* Log history name */
#ifdef  WIN32
static char prvCpssLogHistoryName[80] = "c:\\temp\\cpss_log_entry";
#else
static char prvCpssLogHistoryName[80] = "/tmp/cpss_log_entry";
#endif

/* GT_TRUE - entry level function is not logged,
   GT_FALSE - output all types of log */
static GT_BOOL prvCpssLogSilent = GT_FALSE;

/******************************************************************************/
#define PRV_CPSS_LOG_TYPE_GET(function_id) \
    (prvCpssLogEntryFuncId == function_id) ? CPSS_LOG_TYPE_ENTRY_LEVEL_FUNCTION_E : CPSS_LOG_TYPE_NON_ENTRY_LEVEL_FUNCTION_E;

#define PRV_CPSS_LOG_PARAM_LIB_DB(lib_name) \
    prvCpssLogParamLibDbGet_##lib_name

/* Get API log entries database from specific lib */
#define DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(lib_name) \
extern void PRV_CPSS_LOG_PARAM_LIB_DB(lib_name) \
( \
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr, \
    OUT GT_U32 * logFuncDbSizePtr \
)

DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(NOT_DEFINED);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_APP_DRIVER_CALL);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_BRIDGE);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_PCL);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_TRUNK);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_TTI);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_TUNNEL);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_PROTECTION);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_PTP);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_SYSTEM_RECOVERY);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_TCAM);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_VERSION);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_VNT);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_PHY);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_POLICER);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_HW_INIT);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_CONFIG);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_COS);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_CSCD);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_CUT_THROUGH);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_CNC);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_DIAG);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_PORT);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_TM);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_TM_GLUE);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_SMI);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_INIT);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_NETWORK_IF);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_NST);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_OAM);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_IP);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_LPM);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_IP_LPM);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_MIRROR);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_RESOURCE_MANAGER);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_IPFIX);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_FABRIC);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_MULTI_PORT_GROUP);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_DRAGONITE);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_L2_MLL);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_LOGICAL_TARGET);
DECLARE_EXTERNAL_LOG_PARAM_LIB_DB_GET(CPSS_LOG_LIB_VIRTUAL_TCAM);


/* Array of lib specific database access functions */
PRV_CPSS_LOG_PARAM_LIB_DB_GET_FUNC prvCpssLogParamLibDbArray[CPSS_LOG_LIB_ALL_E];

/* Array of CPSS API return code strings */
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_PRV_CPSS_LOG_FUNC_RET_CODE_ENT[] =
{
    /* L2 Bridge */
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_ERROR                ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_OK                   ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_FAIL                 ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_BAD_VALUE            ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_OUT_OF_RANGE         ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_BAD_PARAM            ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_BAD_PTR              ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_BAD_SIZE             ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_BAD_STATE            ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_SET_ERROR            ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_GET_ERROR            ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_CREATE_ERROR         ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NOT_FOUND            ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NO_MORE              ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NO_SUCH              ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_TIMEOUT              ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NO_CHANGE            ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NOT_SUPPORTED        ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NOT_IMPLEMENTED      ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NOT_INITIALIZED      ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NO_RESOURCE          ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_FULL                 ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_EMPTY                ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_INIT_ERROR           ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NOT_READY            ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_ALREADY_EXIST        ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_OUT_OF_CPU_MEM       ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_ABORTED              ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_NOT_APPLICABLE_DEVICE),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_UNFIXABLE_ECC_ERROR  ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_UNFIXABLE_BIST_ERROR ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_CHECKSUM_ERROR       ),
    PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(GT_DSA_PARSING_ERROR    )
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(PRV_CPSS_LOG_FUNC_RET_CODE_ENT);

/*
 * Typedef: struct PRV_CPSS_LOG_STAT_STC
 *
 * Description:
 *      This structure describes statistic information of CPSS LOG internals.
 *
 * Fields:
 *      maxNumOfApiParams - maximal number of parameters in API functions provided for log.
 *      maxStackDbSize    - maximal size in entries of API stack DB.
 *      errorStackDbFull  - number of stack DB full errors
 *      errorStackParamDbFull - number of stack parameters DB full errors
 *      errorStackIndex0 - number of stack index 0 errors
 *      errorWrongFuncId - number of wrong function ID errors
 *      errorParamInfoNullPointer - number of NULL pointer parameters info errors
 *
 */
typedef struct{
    GT_U32 maxNumOfApiParams;
    GT_U32 maxStackDbSize;
    GT_U32 errorStackDbFull;
    GT_U32 errorStackParamDbFull;
    GT_U32 errorStackIndex0;
    GT_U32 errorWrongFuncId;
    GT_U32 errorParamInfoNullPointer;
}PRV_CPSS_LOG_STAT_STC;

/* log statistic */
static PRV_CPSS_LOG_STAT_STC prvLogStat = {0,0,0,0,0,0,0};

/*******************************************************************************
* prvCpssLogStatDump
*
* DESCRIPTION:
*       Dump statistic information of the log.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssLogStatDump(GT_VOID)
{
    cpssOsPrintf("\n LOG Statistic \n Maximal number of parameters: %d\n Maximal stack size: %d\n",prvLogStat.maxNumOfApiParams, prvLogStat.maxStackDbSize);
    cpssOsPrintf("\n LOG Errors \n Stack DB Full: %d\n Stack params DB Full: %d\n",prvLogStat.errorStackDbFull, prvLogStat.errorStackParamDbFull);
    cpssOsPrintf("Stack DB Index is 0: %d\n Wrong function ID: %d\n Param pointer is NULL: %d\n",prvLogStat.errorStackIndex0, prvLogStat.errorWrongFuncId, prvLogStat.errorParamInfoNullPointer);
    return;
}
/*******************************************************************************
* prvCpssLogApiHistoryDbClose
*
* DESCRIPTION:
*       Close history log database.
*
* INPUTS:
*       entryPtrPtr             - (pointer to) pointer to current database entry.
*       numEntriesPtr           - (pointer to) number of database entries to log.
*       clear                   -  flag, clear the database or not.
*
* OUTPUTS:
*       entryPtrPtr             - (pointer to) pointer to current database entry after clean up.
*       numEntriesPtr           - (pointer to) number of database entries after clean up.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void prvCpssLogApiHistoryDbClose
(
    INOUT PRV_CPSS_LOG_API_HISTORY_ENTRY_STC ** entryPtrPtr,
    INOUT GT_U32 * numEntriesPtr,
    IN    GT_BOOL  clear
)
{
    GT_U32 numEntries;
    PRV_CPSS_LOG_API_HISTORY_ENTRY_STC * currentHistoryEntryPtr;

    CPSS_API_LOCK_MAC(0, 0);
    numEntries = *numEntriesPtr;
    currentHistoryEntryPtr = *entryPtrPtr;

    /* Database is empty */
    if (numEntries == 0)
    {
        CPSS_API_UNLOCK_MAC(0, 0);
        return;
    }

    /* Iterate all database entries */
    if (numEntries > 1)
    {
        *numEntriesPtr = numEntries - 1;
        prvCpssLogApiHistoryDbClose(&currentHistoryEntryPtr->nextEntryPtr, numEntriesPtr, clear);
    }

    if (currentHistoryEntryPtr->entryHistoryFile != CPSS_OS_FILE_INVALID)
    {
        /* Close file */
        cpssOsFclose(currentHistoryEntryPtr->entryHistoryFile);
        if (clear == GT_TRUE) {
            currentHistoryEntryPtr->entryHistoryFile =
                    cpssOsFopen((const char *)currentHistoryEntryPtr->entryHistoryFileName, "w");
            cpssOsFclose(currentHistoryEntryPtr->entryHistoryFile);
        }
        if (currentHistoryEntryPtr != &firstHistoryDbEntry)
        {
            /* Free entry */
            cpssOsFree(currentHistoryEntryPtr);
        }
    }
    /* Reset current entry pointer and number of entries */
    *numEntriesPtr = 0;
    *entryPtrPtr = 0;
    CPSS_API_UNLOCK_MAC(0, 0);
}

/*******************************************************************************
* prvCpssLogApiHistoryDbLogOutput
*
* DESCRIPTION:
*       Output history log database.
*
* INPUTS:
*       currentHistoryEntryPtr                - Start database entry to start from.
*       numEntries              - Number of database entries to print.
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
static void prvCpssLogApiHistoryDbLogOutput
(
    IN PRV_CPSS_LOG_API_HISTORY_ENTRY_STC * currentHistoryEntryPtr,
    IN GT_U32 numEntries
)
{
    GT_U32 entryIndex;

    /* Iterate all database entries */
    for (entryIndex = 0; entryIndex < numEntries; entryIndex++)
    {
        if (currentHistoryEntryPtr->entryHistoryFile != CPSS_OS_FILE_INVALID)
        {
            cpssOsLog(prvCpssLogApiContextLib, CPSS_LOG_TYPE_ENTRY_LEVEL_FUNCTION_E, "API history log entry %d.\n", (numEntries - entryIndex - 1));

            /* Close file */
            cpssOsFclose(currentHistoryEntryPtr->entryHistoryFile);

            /* Open file in read mode */
            currentHistoryEntryPtr->entryHistoryFile = cpssOsFopen((const char *)currentHistoryEntryPtr->entryHistoryFileName, "r");
            if (currentHistoryEntryPtr->entryHistoryFile == CPSS_OS_FILE_INVALID)
            {
                cpssOsLog(prvCpssLogApiContextLib, CPSS_LOG_TYPE_ERROR_E, " ERROR: File opening failed!\n");
                return;
            }

            /* Print history file */
            while (cpssOsFgets(buffer, 1024, currentHistoryEntryPtr->entryHistoryFile) != 0)
            {
                cpssOsLog(prvCpssLogApiContextLib, CPSS_LOG_TYPE_ERROR_E, buffer);
            }
        }

        /* Get next entry */
        currentHistoryEntryPtr = currentHistoryEntryPtr->nextEntryPtr;
    }
}

/*******************************************************************************
* prvCpssLogApiHistory
*
* DESCRIPTION:
*       Manage history log database.
*
* INPUTS:
*       logPhase        - API call log phase - enter or exit.
*       strBufferPtr    - (pointre to) log string buffer
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
static GT_VOID prvCpssLogApiHistory
(
    IN PRV_CPSS_LOG_PHASE_ENT   logPhase,
    IN const char * strBufferPtr
)
{
    static PRV_CPSS_LOG_PHASE_ENT lastState;

    if (prvCpssLogStackSize == 0)
    {
        /* History log disabled */
        return;
    }

    switch (logPhase)
    {
        case PRV_CPSS_LOG_PHASE_ENTER_E:
            if (prvCpssLogApiContextIndex > 1)
            {
                /* Print history in context of previous API */
                break;
            }
            /* New API parsing started */
            if (currentHistoryEntryNum < prvCpssLogStackSize)
            {
                if (currentHistoryEntryPtr == 0)
                {
                    /* Init current history entry */
                    currentHistoryEntryPtr = &firstHistoryDbEntry;
                    /* Set list tail to list head */
                    currentHistoryEntryPtr->nextEntryPtr = currentHistoryEntryPtr;
                }
                else
                {
                    /* Allocate new list entry */
                    currentHistoryEntryPtr->nextEntryPtr = cpssOsMalloc(sizeof(PRV_CPSS_LOG_API_HISTORY_ENTRY_STC));
                    /* Set current list entry */
                    currentHistoryEntryPtr = currentHistoryEntryPtr->nextEntryPtr;
                    /* Set list tail to list head */
                    currentHistoryEntryPtr->nextEntryPtr = &firstHistoryDbEntry;
                }

                cpssOsSprintf(currentHistoryEntryPtr->entryHistoryFileName, "%s_%d", prvCpssLogHistoryName, currentHistoryEntryNum);

                /* Open new file stream */
                currentHistoryEntryPtr->entryHistoryFile =
                    cpssOsFopen((const char *)currentHistoryEntryPtr->entryHistoryFileName, "w");
                if (currentHistoryEntryPtr->entryHistoryFile == CPSS_OS_FILE_INVALID)
                {
                    cpssOsLog(prvCpssLogApiContextLib, CPSS_LOG_TYPE_ERROR_E, " ERROR: File opening failed!\n");
                    break;
                }
                /* Increment entry counter */
                currentHistoryEntryNum++;
            }
            else
            {
                if (currentHistoryEntryPtr->entryHistoryFile != CPSS_OS_FILE_INVALID)
                {
                    /* Sets the position to the beginning of the file */
                    cpssOsRewind(currentHistoryEntryPtr->entryHistoryFile);
                    /* Set current list entry to next in database */
                    currentHistoryEntryPtr = currentHistoryEntryPtr->nextEntryPtr;
                }
            }
            break;
        case PRV_CPSS_LOG_PHASE_EXIT_E:
            if (currentHistoryEntryPtr)
            {
                if (lastState == PRV_CPSS_LOG_PHASE_ERROR_E)
                {
                    if (prvCpssLogApiContextIndex == 1)
                    {
                        /* Start logging from list head */
                        prvCpssLogApiHistoryDbLogOutput(currentHistoryEntryPtr->nextEntryPtr, currentHistoryEntryNum);
                        /* Clean up database structure */
                        prvCpssLogApiHistoryDbClose(&currentHistoryEntryPtr, &currentHistoryEntryNum, GT_FALSE);

                        lastState = PRV_CPSS_LOG_PHASE_EXIT_E;
                    }
                }
            }
            break;
        case PRV_CPSS_LOG_PHASE_ERROR_E:
            lastState = PRV_CPSS_LOG_PHASE_ERROR_E;
            break;
        default:
            return;
    }

    if (lastState != PRV_CPSS_LOG_PHASE_ERROR_E)
    {
        lastState = logPhase;
    }

    if (currentHistoryEntryPtr)
    {
        cpssOsFprintf(currentHistoryEntryPtr->entryHistoryFile, strBufferPtr);
    }
}

/*******************************************************************************
* prvCpssLogAndHistory
*
* DESCRIPTION:
*       Log output and log history
*
* INPUTS:
*       logPhase        - API call log phase - enter or exit.
*       lib             - the function will print the log of the functions in "lib".
*       type            - the function will print the logs from type "type".
*       format          - usual printf format string.
*       ...             - additional parameters.
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
static GT_VOID prvCpssLogAndHistory
(
    IN PRV_CPSS_LOG_PHASE_ENT   logPhase,
    IN CPSS_LOG_LIB_ENT         lib,
    IN CPSS_LOG_TYPE_ENT        type,
    IN const char*              format,
    ...
)
{
    va_list args;
    GT_BOOL enableLib = GT_FALSE;

    cpssLogLibEnableGet(lib,type,&enableLib);

    cpssOsMemSet(buffer, 0, sizeof(buffer));

    va_start(args, format);
    cpssOsVsprintf(buffer, format, args);
    va_end(args);

    switch (type)
    {
        case CPSS_LOG_TYPE_INFO_E:
        case CPSS_LOG_TYPE_ERROR_E:
            if (enableLib == GT_TRUE) {
                cpssOsLog(lib, type, buffer);
            }
            break;
        case CPSS_LOG_TYPE_ENTRY_LEVEL_FUNCTION_E:
        case CPSS_LOG_TYPE_NON_ENTRY_LEVEL_FUNCTION_E:
            if (prvCpssLogSilent == GT_FALSE)
            {
                cpssOsLog(lib, type, buffer);
            }
            break;
        default:
            cpssOsLog(lib, type, buffer);
            break;
    }

    if (type != CPSS_LOG_TYPE_INFO_E)
    {
        prvCpssLogApiHistory(logPhase, buffer);
    }

    return;
}

/*******************************************************************************
* prvCpssLogAndHistoryParam
*
* DESCRIPTION:
*       Log output and log history of function's parameter
*
* INPUTS:
*       lib             - the function will print the log of the functions in "lib".
*       type            - the function will print the logs from type "type".
*       format          - usual printf format string.
*       ...             - additional parameters.
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
GT_VOID prvCpssLogAndHistoryParam
(
    IN CPSS_LOG_LIB_ENT         lib,
    IN CPSS_LOG_TYPE_ENT        type,
    IN const char*              format,
    ...
)
{
    va_list args;

    cpssOsMemSet(buffer, 0, sizeof(buffer));

    va_start(args, format);
    cpssOsVsprintf(buffer, format, args);
    va_end(args);

    switch (type)
    {
        case CPSS_LOG_TYPE_INFO_E:
        case CPSS_LOG_TYPE_ERROR_E:
        default:
            cpssOsLog(lib, type, buffer);
            break;
        case CPSS_LOG_TYPE_ENTRY_LEVEL_FUNCTION_E:
        case CPSS_LOG_TYPE_NON_ENTRY_LEVEL_FUNCTION_E:
            if (prvCpssLogSilent == GT_FALSE)
            {
                cpssOsLog(lib, type, buffer);
            }
            break;
    }

    if (type != CPSS_LOG_TYPE_INFO_E)
    {
        prvCpssLogApiHistory(PRV_CPSS_LOG_PHASE_INFO_E, buffer);
    }

    return;
}

/*******************************************************************************
* prvCpssLogCpssLogEnterParams
*
* DESCRIPTION:
*       Log API enter parameters function.
*
* INPUTS:
*       functionId      - function ID
*       logType         - log type
*       funcEntryPtr    - pointer to API log entry.
*       args            - pointer to current argument in argument list.
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
static void prvCpssLogCpssLogEnterParams
(
    IN GT_U32                         functionId,
    IN CPSS_LOG_TYPE_ENT              logType,
    IN PRV_CPSS_LOG_FUNC_ENTRY_STC  * funcEntryPtr,
    IN va_list                      * args
)
{
    GT_U32                            paramIdx;
    PRV_CPSS_LOG_FUNC_PARAM_STC     * paramPtr;
    PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC paramData;
    GT_BOOL                           skipLog;
    CPSS_LOG_LIB_ENT                  contextLib = prvCpssLogApiContextLib;
    GT_U32                            numOfStoredParams;
    GT_BOOL                           saveParam;
    va_list                           preLogicArgc;

    if (prvCpssLogSilent == GT_FALSE && prvCpssLogFormat == CPSS_LOG_API_FORMAT_NO_PARAMS_E)
    {
        /* Silent mode is off and API parameters not eligible to log */
        return;
    }

    cpssOsMemSet(&paramData, 0, sizeof(PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC));

    if (funcEntryPtr->prvCpssLogPreLogFunc)
    {
        /* copy argc because prvCpssLogPreLogFunc changes argc state */
        #ifdef va_copy
        va_copy(preLogicArgc, *args);
        #else
        cpssOsMemCpy(&preLogicArgc, args, sizeof(*args));
        #endif

        /* Call pre-log API function */
        funcEntryPtr->prvCpssLogPreLogFunc(preLogicArgc, &paramData);

        va_end(preLogicArgc);
    }

    if (prvLogStat.maxNumOfApiParams < funcEntryPtr->numOfParams)
    {
        prvLogStat.maxNumOfApiParams = funcEntryPtr->numOfParams;
    }

    if (prvCpssLogStackCurrentIndex >= PRV_CPSS_LOG_API_STACK_SIZE_CNS)
    {
        prvLogStat.errorStackDbFull++;
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E, "ERROR: stack DB is full. size %d\n", prvCpssLogStackCurrentIndex);
        return;
    }

    /* store function related information */
    prvCpssLogApiStack[prvCpssLogStackCurrentIndex].functionId = functionId;
    prvCpssLogApiStack[prvCpssLogStackCurrentIndex].paramKey = paramData.paramKey;
    numOfStoredParams = 0;

    /* Parsing of parameters */
    for (paramIdx = 0; paramIdx < funcEntryPtr->numOfParams; paramIdx++)
    {
        /* Reset parameter LOG format to default */
        paramData.formatPtr = 0;

        /* get pointer to parameter's information */
        paramPtr = funcEntryPtr->paramsPtrPtr[paramIdx];

        skipLog = GT_FALSE;
        saveParam = GT_TRUE;
        switch (paramPtr->direction)
        {
            case PRV_CPSS_LOG_PARAM_IN_E:
                saveParam = GT_FALSE;
                break;
            case PRV_CPSS_LOG_PARAM_OUT_E:
                skipLog = GT_TRUE;
                break;
            default: /* INOUT */
                break;
        }

        /* Log CPSS API parameters */
        if (paramPtr->logFunc)
        {
            paramPtr->logFunc(contextLib, logType, paramPtr->paramNamePtr, args, skipLog, &paramData);
            if (saveParam)
            {
                if (numOfStoredParams >= PRV_CPSS_LOG_API_PARAM_MAX_NUM_CNS)
                {
                    prvLogStat.errorStackParamDbFull++;
                    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E, "ERROR: Param could not be stored %s\n", paramPtr->paramNamePtr);
                }
                else
                {
                    /* save INOUT or OUT parameter's information and value for further processing in Exit function */
                    prvCpssLogApiStack[prvCpssLogStackCurrentIndex].paramsArr[numOfStoredParams].paramsPtr = paramPtr;
                    prvCpssLogApiStack[prvCpssLogStackCurrentIndex].paramsArr[numOfStoredParams].paramValue = paramData.paramValue;
                    numOfStoredParams++;
                }
            }
        }
        else
        {
            prvLogStat.errorParamInfoNullPointer++;
            PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E, "ERROR: Param could not be logged %s\n", paramPtr->paramNamePtr);
        }
    }

    prvCpssLogApiStack[prvCpssLogStackCurrentIndex].numOfParams = numOfStoredParams;
    prvCpssLogStackCurrentIndex++;

    if (prvLogStat.maxStackDbSize  < prvCpssLogStackCurrentIndex)
    {
        prvLogStat.maxStackDbSize = prvCpssLogStackCurrentIndex;
    }
}

/*******************************************************************************
* prvCpssLogCpssLogExitParams
*
* DESCRIPTION:
*       Log API parameters function.
*
* INPUTS:
*       functionId      - function ID
*       logType         - log type
*       funcEntryPtr    - pointer to API log entry.
*       args            - pointer to current argument in argument list.
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
static void prvCpssLogCpssLogExitParams
(
    IN GT_U32                         functionId,
    IN CPSS_LOG_TYPE_ENT              logType,
    IN PRV_CPSS_LOG_FUNC_ENTRY_STC  * funcEntryPtr
)
{
    GT_U32                            paramIdx;
    PRV_CPSS_LOG_FUNC_PARAM_STC     * paramPtr;
    PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC paramData;
    GT_BOOL                           skipLog;
    CPSS_LOG_LIB_ENT                  contextLib = prvCpssLogApiContextLib;
    GT_U32                            numOfStoredParams;

    (void)funcEntryPtr;
    if (prvCpssLogSilent == GT_FALSE && prvCpssLogFormat == CPSS_LOG_API_FORMAT_NO_PARAMS_E)
    {
        /* Silent mode is off and API parameters not eligible to log */
        return;
    }

    if (prvCpssLogStackCurrentIndex)
    {
        prvCpssLogStackCurrentIndex--;
    }
    else
    {
        prvLogStat.errorStackIndex0++;
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E, "ERROR: stack index 0, functionId %d\n", functionId);
    }

    /* check log's DB consistency */
    if(prvCpssLogApiStack[prvCpssLogStackCurrentIndex].functionId != functionId)
    {
        prvLogStat.errorWrongFuncId++;
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E, "ERROR: wrong functionId %d expected %d\n", functionId, prvCpssLogApiStack[prvCpssLogStackCurrentIndex].functionId);
        return;
    }

    cpssOsMemSet(&paramData, 0, sizeof(PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC));

    /* use function related info from DB */
    paramData.paramKey = prvCpssLogApiStack[prvCpssLogStackCurrentIndex].paramKey;
    numOfStoredParams = prvCpssLogApiStack[prvCpssLogStackCurrentIndex].numOfParams;

    skipLog = GT_FALSE;

    /* Parsing of parameters */
    for (paramIdx = 0; paramIdx < numOfStoredParams; paramIdx++)
    {
        /* Reset parameter LOG format to default */
        paramData.formatPtr = 0;

        paramPtr =  prvCpssLogApiStack[prvCpssLogStackCurrentIndex].paramsArr[paramIdx].paramsPtr;
        paramData.paramValue = prvCpssLogApiStack[prvCpssLogStackCurrentIndex].paramsArr[paramIdx].paramValue;

        /* Log CPSS API parameters */
        if (paramPtr->logFunc)
        {
            paramPtr->logFunc(contextLib, logType, paramPtr->paramNamePtr, NULL, skipLog, &paramData);
        }
        else
        {
            prvLogStat.errorParamInfoNullPointer++;
            PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E, "ERROR: Param could not be logged %s\n", paramPtr->paramNamePtr);
        }
    }
}
/*******************************************************************************
* prvCpssLogApiFunctionLogTrigger
*
* DESCRIPTION:
*       Check triggered API function log types
*
* INPUTS:
*       logType - type of the log
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_TRUE - API log triggered
*       GT_FALSE - API log not triggered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL prvCpssLogApiFunctionLogTrigger
(
    IN CPSS_LOG_TYPE_ENT logType
)
{
    GT_BOOL enableLib = GT_FALSE;
    GT_STATUS rc;

    /* Get entrance lib status */
    rc = cpssLogLibEnableGet(prvCpssLogApiContextLib, logType, &enableLib);
    if (rc != GT_OK)
    {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, prvCpssLogApiContextLib, CPSS_LOG_TYPE_ERROR_E, "prvCpssLogApiFunctionLogTrigger: rc = %d\n", rc);
        return GT_FALSE;
    }

    if (prvCpssLogStackSize)
    {
        /* Log history enabled if API context lib log is disabled and vise versa */
        prvCpssLogSilent = (enableLib) ? GT_FALSE : GT_TRUE;
        /* Activate API history log */
        enableLib = GT_TRUE;
    }
    else
    {
        /* Log history disabled */
        prvCpssLogSilent = GT_FALSE;
    }

    /* API log  trigger status */
    return enableLib;
}

/*******************************************************************************
* prvCpssLogApiLibContextSet
*
* DESCRIPTION:
*       Set API LIB context logic
*
* INPUTS:
*       functionId - function identifier
*       logPhase  - API call log phase - enter or exit
*
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void prvCpssLogApiLibContextSet
(
    IN GT_U32   functionId,
    IN PRV_CPSS_LOG_PHASE_ENT  logPhase
)
{
    GT_U32 funcLibId = (functionId >> 16) & 0xFFFF;

    switch (logPhase)
    {
        case PRV_CPSS_LOG_PHASE_ENTER_E:
            if (prvCpssLogApiContextIndex == 0)
            {
                /* Set API call lib context */
                prvCpssLogApiContextLib = funcLibId;
                /* Set API entry level type */
                prvCpssLogEntryFuncId = functionId;
            }
            /* Increment API context index */
            prvCpssLogApiContextIndex++;
            break;
        case PRV_CPSS_LOG_PHASE_EXIT_E:
            /* Decrement API context index */
            prvCpssLogApiContextIndex--;
            if (prvCpssLogApiContextIndex == 0)
            {
                /* Reset API call lib context */
                prvCpssLogApiContextLib = CPSS_LOG_LIB_ALL_E;
                /* Reset API log type */
                prvCpssLogEntryFuncId = PRV_CPSS_LOG_NOT_VALID_CNS;
            }
            break;
        default:
            PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, prvCpssLogApiContextLib, CPSS_LOG_TYPE_ERROR_E, "prvCpssLogApiLibContextSet: wrong log phase.\n");
    }
}

/*******************************************************************************
* prvCpssLogParamLibDbGet_NOT_DEFINED
*
* DESCRIPTION:
*       Functions DB entry for not defined yet lib
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamLibDbGet_NOT_DEFINED
(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = 0;
    *logFuncDbSizePtr = 0;
}

/*******************************************************************************
* prvCpssLogInit
*
* DESCRIPTION:
*       Init log related info
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogInit
(
    GT_VOID
)
{
    GT_U32 funcId;

    for (funcId = 0; funcId < CPSS_LOG_LIB_ALL_E; funcId++)
    {
        switch (funcId)
        {
            case CPSS_LOG_LIB_APP_DRIVER_CALL_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_APP_DRIVER_CALL);
                break;
            case CPSS_LOG_LIB_BRIDGE_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_BRIDGE);
                break;
            case CPSS_LOG_LIB_CNC_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_CNC);
                break;
            case CPSS_LOG_LIB_CONFIG_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_CONFIG);
                break;
            case CPSS_LOG_LIB_COS_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_COS);
                break;
            case CPSS_LOG_LIB_HW_INIT_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_HW_INIT);
                break;
            case CPSS_LOG_LIB_CSCD_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_CSCD);
                break;
            case CPSS_LOG_LIB_CUT_THROUGH_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_CUT_THROUGH);
                break;
            case CPSS_LOG_LIB_DIAG_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_DIAG);
                break;
            case CPSS_LOG_LIB_IPFIX_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_IPFIX);
                break;
            case CPSS_LOG_LIB_PCL_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_PCL);
                break;
            case CPSS_LOG_LIB_FABRIC_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_FABRIC);
                break;
            case CPSS_LOG_LIB_PHY_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_PHY);
                break;
            case CPSS_LOG_LIB_MIRROR_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_MIRROR);
                break;
            case CPSS_LOG_LIB_POLICER_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_POLICER);
                break;
            case CPSS_LOG_LIB_MULTI_PORT_GROUP_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_MULTI_PORT_GROUP);
                break;
            case CPSS_LOG_LIB_NETWORK_IF_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_NETWORK_IF);
                break;
            case CPSS_LOG_LIB_PORT_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_PORT);
                break;
            case CPSS_LOG_LIB_PROTECTION_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_PROTECTION);
                break;
            case CPSS_LOG_LIB_NST_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_NST);
                break;
            case CPSS_LOG_LIB_SYSTEM_RECOVERY_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_SYSTEM_RECOVERY);
                break;
            case CPSS_LOG_LIB_OAM_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_OAM);
                break;
            case CPSS_LOG_LIB_PTP_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_PTP);
                break;
            case CPSS_LOG_LIB_TRUNK_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_TRUNK);
                break;
            case CPSS_LOG_LIB_TTI_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_TTI);
                break;
            case CPSS_LOG_LIB_TUNNEL_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_TUNNEL);
                break;
            case CPSS_LOG_LIB_VNT_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_VNT);
                break;
            case CPSS_LOG_LIB_RESOURCE_MANAGER_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_RESOURCE_MANAGER);
                break;
            case CPSS_LOG_LIB_VERSION_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_VERSION);
                break;
            case CPSS_LOG_LIB_TM_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_TM);
                break;
            case CPSS_LOG_LIB_TM_GLUE_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_TM_GLUE);
                break;
            case CPSS_LOG_LIB_TCAM_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_TCAM);
                break;
            case CPSS_LOG_LIB_SMI_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_SMI);
                break;
            case CPSS_LOG_LIB_INIT_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_INIT);
                break;
#ifdef DRAGONITE_TYPE_A1
            case CPSS_LOG_LIB_DRAGONITE_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_DRAGONITE);
                break;
#endif
            case CPSS_LOG_LIB_IP_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_IP);
                break;
            case CPSS_LOG_LIB_LPM_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_LPM);
                break;
            case CPSS_LOG_LIB_IP_LPM_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_IP_LPM);
                break;
            case CPSS_LOG_LIB_L2_MLL_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_L2_MLL);
                break;
            case CPSS_LOG_LIB_LOGICAL_TARGET_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_LOGICAL_TARGET);
                break;
            case CPSS_LOG_LIB_VIRTUAL_TCAM_E:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(CPSS_LOG_LIB_VIRTUAL_TCAM);
                break;
            default:
                prvCpssLogParamLibDbArray[funcId] = PRV_CPSS_LOG_PARAM_LIB_DB(NOT_DEFINED);
                break;
        }
    }
}

/*******************************************************************************
* prvCpssLogApiFunctionEntryGet
*
* DESCRIPTION:
*       Get function entry from specific lib database by function ID
*
* INPUTS:
*       functionId - function identifier
*       logPhase   - API call log phase - enter or exit
*
* OUTPUTS:
*       functionEntryPtrPtr - (pointer to) pointer to function entry or NULL if not found
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void prvCpssLogApiFunctionEntryGet
(
    IN GT_U32 functionId,
    IN PRV_CPSS_LOG_PHASE_ENT  logPhase GT_UNUSED,
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** functionEntryPtrPtr
)
{
    GT_U32 funcLibId = (functionId >> 16) & 0xFFFF;
    GT_U32 funcDbIdx = functionId & 0xFFFF;

    PRV_CPSS_LOG_FUNC_ENTRY_STC * functionDbPtr = 0;
    GT_U32 functionEntryDbSize;

    /* Null entry pointer by default */
    *functionEntryPtrPtr = 0;

    if (funcLibId >= CPSS_LOG_LIB_ALL_E)
    {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, prvCpssLogApiContextLib, CPSS_LOG_TYPE_ERROR_E, " ERROR: %s lib is out of range %d \n", funcLibId);
        return;
    }
    /* Get API log entries database from specific lib */
    prvCpssLogParamLibDbArray[funcLibId](&functionDbPtr, &functionEntryDbSize);
    if (funcDbIdx >= functionEntryDbSize)
    {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, prvCpssLogApiContextLib, CPSS_LOG_TYPE_ERROR_E, " ERROR: functionId[%d] is out of range \n", funcDbIdx);
        return;
    }

    /* Get specific API function log entry from database */
    *functionEntryPtrPtr = &functionDbPtr[funcDbIdx];
}

/*******************************************************************************
* prvCpssLogApiEnter
*
* DESCRIPTION:
*       Internal API for API/Internal/Driver function call log - enter function.
*
* INPUTS:
*       functionId - function identifier (of the function we just entered into).
*       ... - the parameters values of the function we just entered into.
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
void prvCpssLogApiEnter
(
    IN GT_U32 functionId,
    ...
)
{
    PRV_CPSS_LOG_FUNC_ENTRY_STC  *funcEntryPtr;
    va_list args;
    CPSS_LOG_TYPE_ENT logType;

    /* Get function DB Pointer */
    prvCpssLogApiFunctionEntryGet(functionId, PRV_CPSS_LOG_PHASE_ENTER_E, &funcEntryPtr);
    if (funcEntryPtr == 0)
    {
        return;
    }
    /* Set API context */
    prvCpssLogApiLibContextSet(functionId, PRV_CPSS_LOG_PHASE_ENTER_E);
    /* Get log type after API/lib context set */
    logType = PRV_CPSS_LOG_TYPE_GET(functionId);

    /* Check API log trigger conditions */
    if (prvCpssLogApiFunctionLogTrigger(logType) == GT_FALSE)
    {
        return;
    }

    /* Log header with name of function */
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ENTER_E, prvCpssLogApiContextLib, logType, "Enter: %s\n",funcEntryPtr->apiName);

    va_start(args, functionId);

    prvCpssLogCpssLogEnterParams(functionId, logType, funcEntryPtr, &args);

    va_end(args);
}

/*******************************************************************************
* prvCpssLogApiExit
*
* DESCRIPTION:
*       Internal API for API/Internal function call log - exit function.
*
* INPUTS:
*       functionId - function identifier (of the function we just exit from).
*       rc - the return code of the function.
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
void prvCpssLogApiExit
(
    IN GT_U32 functionId,
    IN GT_STATUS rc
)
{
    PRV_CPSS_LOG_FUNC_ENTRY_STC  *funcEntryPtr;
    CPSS_LOG_TYPE_ENT logType;
    CPSS_LOG_LIB_ENT  contextLib = prvCpssLogApiContextLib;

    /* Function DB Pointer */
    prvCpssLogApiFunctionEntryGet(functionId, PRV_CPSS_LOG_PHASE_EXIT_E, &funcEntryPtr);
    if (funcEntryPtr == 0)
    {
        return;
    }

    /* Get log type after API/lib context set */
    logType = PRV_CPSS_LOG_TYPE_GET(functionId);

    /* Check API log trigger conditions */
    if (prvCpssLogApiFunctionLogTrigger(logType) == GT_TRUE)
    {
        /* log header with name of function */
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_EXIT_E, contextLib, logType, "Exit: %s\n",funcEntryPtr->apiName);

        prvCpssLogCpssLogExitParams(functionId, logType, funcEntryPtr);

        /* log API return code */
        PRV_CPSS_LOG_VAR_ENUM_MAP_MAC(rc, PRV_CPSS_LOG_FUNC_RET_CODE_ENT);
    }

    /* Reset API context */
    prvCpssLogApiLibContextSet(functionId, PRV_CPSS_LOG_PHASE_EXIT_E);
}

/*******************************************************************************
* prvCpssLogErrorCommon
*
* DESCRIPTION:
*       Log API for API/Internal function error message
*
* INPUTS:
*       functionName    - name of function that generates error.
*       fileName        - name of file that generates error.
*       line            - line number in file, may be excluded from log by global configuration.
*       rc              - the return code of the function.
*       errorMsgPtr     - cpssOsLog formatted message.
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
GT_VOID prvCpssLogErrorCommon
(
    IN const char *functionName,
    IN const char *fileName,
    IN GT_U32 line,
    IN GT_STATUS rc,
    const char * errorMsgPtr
)
{
    CPSS_LOG_LIB_ENT  contextLib = prvCpssLogApiContextLib;
    GT_U8  * enumStringPtr;
    GT_U8  * timeStampPtr = (GT_U8 *)buffer1;
    GT_U32 timeStampSize = sizeof(buffer1);

    cpssOsMemSet(buffer1, 0, sizeof(buffer1));

    switch (timeFormat)
    {
        case CPSS_LOG_TIME_FORMAT_DATE_TIME_E:
            /* Get current time */
            cpssOsStrfTime((char *)timeStampPtr, timeStampSize, CPSS_LOG_TIME_FORMAT_DATE_TIME_CNS, cpssOsTime());
            break;
        case CPSS_LOG_TIME_FORMAT_SHORT_DATE_TIME_E:
            /* Get current time */
            cpssOsStrfTime((char *)timeStampPtr, timeStampSize, CPSS_LOG_TIME_FORMAT_SHORT_DATE_TIME_CNS, cpssOsTime());
            break;
        case CPSS_LOG_TIME_FORMAT_SHORT_DATE_ISO_TIME_E:
            /* Get current time */
            cpssOsStrfTime((char *)timeStampPtr, timeStampSize, CPSS_LOG_TIME_FORMAT_SHORT_DATE_ISO_TIME_CNS, cpssOsTime());
            break;
        case CPSS_LOG_TIME_FORMAT_NO_DATE_TIME_E:
            timeStampPtr = 0;
            break;
        default:
            PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E, "Bad time format %d\n", timeFormat);
            return;
    }

    if (timeStampPtr)
    {
        /* log error timestamp */
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_INFO_E, "%s ", timeStampPtr);
    }

    enumStringPtr = (GT_U8 *)buffer1;
    /* Get API return code string */
    PRV_CPSS_LOG_ENUM_STRING_GET_MAC(rc, PRV_CPSS_LOG_FUNC_RET_CODE_ENT, enumStringPtr);

    /* log error message */
    if (GT_TRUE == prvCpssLogLineNumberEnabled)
    {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E,
                                     "ERROR %s in function: %s, file %s, line[%d]. %s\n",
                                     enumStringPtr, functionName, fileName, line, errorMsgPtr);
    }
    else
    {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_ERROR_E, contextLib, CPSS_LOG_TYPE_ERROR_E,
                                     "ERROR %s in function: %s, file %s. %s\n",
                                     enumStringPtr, functionName, fileName, errorMsgPtr);
    }

}

/*******************************************************************************
* prvCpssLogError
*
* DESCRIPTION:
*       Internal API for API/Internal function error log
*
* INPUTS:
*       functionName    - name of function that generates error.
*       fileName        - name of file that generates error.
*       line            - line number in file, may be excluded from log by global configuration.
*       rc              - the return code of the function.
*       formatPtr       - cpssOsLog format starting.
*       ...               optional parameters according to formatPtr
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
GT_VOID prvCpssLogError
(
    IN const char *functionName,
    IN const char *fileName,
    IN GT_U32 line,
    IN GT_STATUS rc,
    IN const char * formatPtr,
    ...
)
{
    va_list args;

    /* Need separate buffer for info message to prevent override data by recursive call using global buffer */
    static char buffer[PRV_LOG_STRING_BUFFER_SIZE_CNS];

    va_start(args, formatPtr);
    cpssOsVsprintf(buffer, formatPtr, args);
    va_end(args);

    /* Call common error log function */
    prvCpssLogErrorCommon(functionName, fileName, line, rc, buffer);
}

/*******************************************************************************
* prvCpssLogInformation
*
* DESCRIPTION:
*       Internal API for API/Internal function information log
*
* INPUTS:
*       functionName    - name of function that generates error.
*       fileName        - name of file that generates error.
*       line            - line number in file, may be excluded from log by global configuration.
*       formatPtr       - cpssOsLog format starting.
*       ...               optional parameters according to formatPtr
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
GT_VOID prvCpssLogInformation
(
    IN const char *functionName,
    IN const char *fileName,
    IN GT_U32 line,
    IN const char * formatPtr,
    ...
)
{
    CPSS_LOG_LIB_ENT  contextLib = prvCpssLogApiContextLib;
    va_list args;
    /* Need separate buffer for info message to prevent override data by recursive call using global buffer */
    static char buffer[PRV_LOG_STRING_BUFFER_SIZE_CNS];

    va_start(args, formatPtr);
    cpssOsVsprintf(buffer, formatPtr, args);
    va_end(args);

    /* log information message */
    if (GT_TRUE == prvCpssLogLineNumberEnabled)
    {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, CPSS_LOG_TYPE_INFO_E,
                                     "Information: in function %s, file %s, line[%d]. %s\n",
                                     functionName, fileName, line, buffer);
    }
    else
    {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, CPSS_LOG_TYPE_INFO_E,
                                     "Information: in function %s, file %s. %s\n",
                                     functionName, fileName, buffer);
    }

}

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
GT_VOID cpssLogEnableSet
(
     IN GT_BOOL enable
)
{
    prvCpssLogEnabled = enable;
}

/*******************************************************************************
* cpssLogEnableGet
*
* DESCRIPTION:
*       The function gets the status of the Log feature (enabled/disabled).
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_TRUE         - if the log is enabled
*       GT_FALSE        - if the log is disabled
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL cpssLogEnableGet(GT_VOID)
{
    return prvCpssLogEnabled;
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
GT_STATUS cpssLogLibEnableSet
(
    IN  CPSS_LOG_LIB_ENT    lib,
    IN  CPSS_LOG_TYPE_ENT   type,
    IN  GT_BOOL             enable
)
{
    GT_U32 i; /* iterator */
    GT_U32 k; /* iterator */

    if (lib > CPSS_LOG_LIB_ALL_E)
        return GT_BAD_PARAM;

    if (type > CPSS_LOG_TYPE_ALL_E)
        return GT_BAD_PARAM;

    if ((type != CPSS_LOG_TYPE_ALL_E) && (lib != CPSS_LOG_LIB_ALL_E))
    {
        /* specific lib and specific type */
        prvCpssLogLibsLogsTypesTable[lib][type] = enable;
    }
    else if (lib != CPSS_LOG_LIB_ALL_E)
    {
        /* specific lib but all types */
        for (i = 0; i < CPSS_LOG_TYPE_ALL_E; i++)
        {
            prvCpssLogLibsLogsTypesTable[lib][i] = enable;
        }
    }
    else if (type != CPSS_LOG_TYPE_ALL_E)
    {
        /* specific type but all libs */
        for (i = 0; i < CPSS_LOG_LIB_ALL_E; i++)
        {
            prvCpssLogLibsLogsTypesTable[i][type] = enable;
        }
    }
    else
    {   /* all libs and all types */
        for (i = 0; i < CPSS_LOG_LIB_ALL_E; i++)
        {
            for (k = 0; k < CPSS_LOG_TYPE_ALL_E; k++)
            {
                prvCpssLogLibsLogsTypesTable[i][k] = enable;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssLogLibEnableGet
*
* DESCRIPTION:
*       The function gets the status of specific type of log information for
*       specific lib (enabled/disabled).
*
* INPUTS:
*       lib - lib to get the log status for.
*       type - log type.
*
* OUTPUTS:
*       enablePtr - GT_TRUE - log is enabled for the specific lib and log type.
*                   GT_FALSE - log is enabled for the specific lib and log type.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PTR      - bad pointer enablePtr
*       GT_BAD_PARAM    - bad parameter lib or type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogLibEnableGet
(
    IN   CPSS_LOG_LIB_ENT   lib,
    IN   CPSS_LOG_TYPE_ENT  type,
    OUT  GT_BOOL            * enablePtr
)
{
    if (lib >= CPSS_LOG_LIB_ALL_E)
        return GT_BAD_PARAM;

    if (type >= CPSS_LOG_TYPE_ALL_E)
        return GT_BAD_PARAM;

    if (enablePtr!=NULL)
    {
        *enablePtr = prvCpssLogLibsLogsTypesTable[lib][type];
        return GT_OK;
    }
    return GT_BAD_PTR;
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
GT_STATUS cpssLogApiFormatSet
(
    IN CPSS_LOG_API_FORMAT_ENT format
)
{
    prvCpssLogFormat=format;
    return GT_OK;
}

/*******************************************************************************
* cpssLogApiFormatGet
*
* DESCRIPTION:
*       The function gets the format of API related log.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       CPSS_LOG_API_FORMAT_NO_PARAMS_E          - not documenting any paramter
*       CPSS_LOG_API_FORMAT_ALL_PARAMS_E         - documenting all the parameters
*                                                  values
*       CPSS_LOG_API_FORMAT_NON_ZERO_PARAMS_E    - documenting only the non zero
*                                                  parameters values
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CPSS_LOG_API_FORMAT_ENT cpssLogApiFormatGet(GT_VOID)
{
    return prvCpssLogFormat;
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
GT_STATUS cpssLogStackSizeSet
(
    GT_U32 size
)
{
    CPSS_API_LOCK_MAC(0, 0);
    if (currentHistoryEntryPtr != NULL || currentHistoryEntryNum != 0) {
        prvCpssLogApiHistoryDbClose(&currentHistoryEntryPtr, &currentHistoryEntryNum, GT_TRUE);
    }
    prvCpssLogStackSize = size;
    CPSS_API_UNLOCK_MAC(0, 0);
    return GT_OK;
}

/*******************************************************************************
* cpssLogStackSizeGet
*
* DESCRIPTION:
*       The function gets the size of the log stack
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       the log stack size
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 cpssLogStackSizeGet(GT_VOID)
{
    return prvCpssLogStackSize;
}

/* Compare two enum value */
static int enumCompare
(
    const void *a,
    const void *b
)
{
    PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC *a1=(PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC*)a;
    PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC *b1=(PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC*)b;
    if (a1->enumValue==b1->enumValue)
    {
        return 0;
    }
    if (a1->enumValue<b1->enumValue)
    {
        return -1;
    }
    if (a1->enumValue>b1->enumValue)
    {
        return 1;
    }
    return 0;
}

/*******************************************************************************
* cpssLogTimeFormatSet
*
* DESCRIPTION:
*       The function sets log time format.
*
* INPUTS:
*       format - log time format.
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
GT_STATUS cpssLogTimeFormatSet
(
    IN CPSS_LOG_TIME_FORMAT_ENT format
)
{
    switch (format)
    {
        case CPSS_LOG_TIME_FORMAT_DATE_TIME_E:
        case CPSS_LOG_TIME_FORMAT_SHORT_DATE_TIME_E:
        case CPSS_LOG_TIME_FORMAT_SHORT_DATE_ISO_TIME_E:
        case CPSS_LOG_TIME_FORMAT_NO_DATE_TIME_E:
            timeFormat = format;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssLogEnumMap
*
* DESCRIPTION:
*       The function to log mapped enum parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - parameter/structure field name.
*       keyValue - lookup enum value
*       enumArray - array of emumerator strings.
*       enumArrayEntries - number of enum array entries.
*       enumArrayEntrySize - array entry size
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
void prvCpssLogEnumMap
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U32                   keyValue,
    IN void                   * enumArray,
    IN GT_U32                   enumArrayEntries,
    IN GT_U32                   enumArrayEntrySize
)
{
    GT_U8 * enumStringPtr = (GT_U8 *)buffer1;

    /* Get enum string value by its key */
    prvCpssEnumStringValueGet(fieldName, keyValue, enumArray, enumArrayEntries, enumArrayEntrySize, enumStringPtr);
    /* Indent tabs  */
    prvCpssLogTabs(contextLib, logType);
    /* Log out enum string value */
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %s\n", fieldName, enumStringPtr);
}

/*******************************************************************************
* prvCpssEnumStringValueGet
*
* DESCRIPTION:
*       The function maps enum parameter to string value
*
* INPUTS:
*       fieldName - parameter/structure field name.
*       keyValue - lookup enum value
*       enumArray - array of emumerator strings.
*       enumArrayEntries - number of enum array entries.
*       enumArrayEntrySize - array entry size
*
* OUTPUTS:
*       keyStringPtr - (pointer to) key value mapped string
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssEnumStringValueGet
(
    IN GT_CHAR_PTR              fieldName GT_UNUSED,
    IN GT_U32                   keyValue,
    IN void                   * enumArray,
    IN GT_U32                   enumArrayEntries,
    IN GT_U32                   enumArrayEntrySize,
    OUT GT_U8                 * keyStringPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC entry;
    PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC * currentHistoryEntryPtr = &entry;
    currentHistoryEntryPtr->enumValue = keyValue;
    rc = cpssOsBsearch((void *)currentHistoryEntryPtr, (void *)enumArray, enumArrayEntries, enumArrayEntrySize, enumCompare, (void*)&currentHistoryEntryPtr);
    if (rc == GT_OK) {
        cpssOsSprintf((char *)keyStringPtr, "%s", currentHistoryEntryPtr->namePtr);
    } else {
        cpssOsSprintf((char *)keyStringPtr, "0x%08x", keyValue);
    }
}

/*******************************************************************************
* prvCpssLogStcNumber
*
* DESCRIPTION:
*       The function to log numeric parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
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
void prvCpssLogStcNumber
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U32                   numValue
)
{
    PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(numValue);
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %u\n", fieldName, numValue);
}

/*******************************************************************************
* prvCpssLogStcUintptr
*
* DESCRIPTION:
*       The function to log GT_UINTPTR parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
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
void prvCpssLogStcUintptr
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_UINTPTR               numValue
)
{
    GT_CHAR *formatStr;
    PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(numValue);
    prvCpssLogTabs(contextLib, logType);
    formatStr = (prvCpssLogPointerFormat == CPSS_LOG_POINTER_FORMAT_PREFIX_E) ?
        "%s = (addr)%p\n" :  "%s = %p\n";
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType,
                                 formatStr, fieldName, numValue);
}

/*******************************************************************************
* prvCpssLogStcByte
*
* DESCRIPTION:
*       The function to log byte parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
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
void prvCpssLogStcByte
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U8                    byteValue
)
{
    PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(byteValue);
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = 0x%x\n", fieldName, byteValue);
}

/*******************************************************************************
* prvCpssLogParamFuncStc_GT_FLOAT32_PTR
*
* DESCRIPTION:
*       The function to log GT_FLOAT32 parameter by pointer
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       fieldPtr - (pointer to) field value
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
void prvCpssLogParamFuncStc_GT_FLOAT32_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    GT_CHAR_PTR                 namePtr,
    IN GT_VOID                * fieldPtr
)
{
    PRV_CPSS_LOG_SET_FIELD_VAL_PTR_MAC(GT_FLOAT32 *, valPtr);
    prvCpssLogStcFloat(contextLib, logType, namePtr, *valPtr);
}

/*******************************************************************************
* prvCpssLogStcFloat
*
* DESCRIPTION:
*       The function to log float parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
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
void prvCpssLogStcFloat
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN float                    numValue
)
{
    PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(numValue);
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %f\n", fieldName, numValue);
}

/*******************************************************************************
* prvCpssLogStcPointer
*
* DESCRIPTION
*       The function to log pointer parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       ptrValue - (pointer to) field value.
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
void prvCpssLogStcPointer
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN void *                   ptrValue
)
{
    GT_CHAR *formatStr;
    PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(ptrValue);
    prvCpssLogTabs(contextLib, logType);
    formatStr = (prvCpssLogPointerFormat == CPSS_LOG_POINTER_FORMAT_PREFIX_E) ?
        "%s = (addr)%p\n" :  "%s = %p\n";
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType,
                                 formatStr, fieldName, ptrValue);
}

/*******************************************************************************
* prvCpssLogParamFuncStc_GT_U8_PTR_PTR
*
* DESCRIPTION
*       The function to log pointer to GT_U8_PTR parameter.
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       fieldPtr - (pointer to) field value.
*       inOutParamInfoPtr - (pointer to) auxiliary parameters.
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
void prvCpssLogParamFuncStc_GT_U8_PTR_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_FIELD_VAL_PTR_MAC(GT_U8**, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    prvCpssLogStcPointer(contextLib, logType, namePtr, *valPtr);
}
/*******************************************************************************
* prvCpssLogTabs
*
* DESCRIPTION:
*       Log tab for recursive structures
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
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
void prvCpssLogTabs
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType
)
{
    GT_U32 i;
    for (i = 0; i < prvCpssLogTabIndex; i++){
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "\t");
    }
}

/*******************************************************************************
* prvCpssLogStcLogStart
*
* DESCRIPTION:
*       The function starts log structure
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       stcName - structure field name.
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
void prvCpssLogStcLogStart
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 stcName
)
{
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s {\n", stcName);
    prvCpssLogTabIndex++;
}

/*******************************************************************************
* prvCpssLogStcLogEnd
*
* DESCRIPTION:
*       The function ends log structure
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
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
void prvCpssLogStcLogEnd
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType
)
{
    if (prvCpssLogTabIndex)
    {
        prvCpssLogTabIndex--;
    }
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "}\n");
}

/*******************************************************************************
* prvCpssLogPacket
*
* DESCRIPTION:
*       The function logs packet as array of bytes
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       packetSize -
*           size of packet (in bytes)
*       packetPtr -
*           a pointer to the first byte of the packet
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
void prvCpssLogPacket
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_U32                      packetSize,
    IN GT_U8                       *packetPtr
)
{
    GT_U32  byteCount = 0; /* index */
    static char buffer[PRV_LOG_STRING_BUFFER_SIZE_CNS]; /* buffer for the 16 byte printing */

    /* printing the packet data, 16 byte in each line*/
    for (byteCount = 0; byteCount < packetSize; byteCount++)
    {
        /* adress of the new 16 byte series */
        if((byteCount & 0x0F) == 0)
        {
            cpssOsSprintf(buffer,"0x%4.4x :", byteCount);
        }

        cpssOsSprintf(buffer,"%s %2.2x",buffer,((GT_U8*)packetPtr)[byteCount]);

        /* end of 16 bytes series */
        if((byteCount & 0x0F) == 0x0F)
        {
            cpssOsSprintf(buffer,"%s\n",buffer);
            prvCpssLogTabs(contextLib, logType);
            PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, buffer);
            cpssOsSprintf(buffer,"");
        }
    }

    /* the packet size is less than 16 bytes so we print the only line*/
    if (packetSize != 0 && packetSize < 16) 
    {
        cpssOsSprintf(buffer,"%s\n",buffer);
        prvCpssLogTabs(contextLib, logType);
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, buffer);
        cpssOsSprintf(buffer,"");
    }
}

/*******************************************************************************
* prvCpssLogEnum
*
* DESCRIPTION:
*       The function to log simple enum parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - parameter/structure field name.
*       enumArray - array of emumerator strings.
*       enumArrayIndex - log string access index in enum array.
*       enumArrayEntries - number of enum array entries.
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
void prvCpssLogEnum
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN char                   * enumArray[],
    IN GT_U32                   enumArrayIndex,
    IN GT_U32                   enumArrayEntries
)
{
    prvCpssLogTabs(contextLib, logType);
    if (enumArrayIndex < enumArrayEntries) {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %s\n", fieldName, enumArray[enumArrayIndex]);
    } else {
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = 0x%08x\n", fieldName, enumArrayIndex);
    }
}

/*******************************************************************************
* prvCpssLogStcBool
*
* DESCRIPTION:
*       The function logs GT_BOOL structure field
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       bValue - field value.
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
void prvCpssLogStcBool
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_BOOL                  bValue
)
{
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %s\n", fieldName, (bValue == GT_FALSE) ? "GT_FALSE" : "GT_TRUE");
}

/*******************************************************************************
* prvCpssLogStcMac
*
* DESCRIPTION:
*       Function to log mac address parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       macPtr - pointer to ethernet address.
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
void prvCpssLogStcMac
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U8                  * macPtr
)
{
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %02x:%02x:%02x:%02x:%02x:%02x\n", fieldName,
                macPtr[0],macPtr[1],macPtr[2],macPtr[3],macPtr[4],macPtr[5]);
}

/*******************************************************************************
* prvCpssLogStcIpV4
*
* DESCRIPTION:
*       Function to log IPV4 address parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       ipAddrPtr - pointer to IPV4 address.
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
void prvCpssLogStcIpV4
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U8                  * ipAddrPtr
)
{
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %d.%d.%d.%d\n", fieldName,
                ipAddrPtr[0],ipAddrPtr[1],ipAddrPtr[2], ipAddrPtr[3]);
}

/*******************************************************************************
* prvCpssLogStcIpV6
*
* DESCRIPTION:
*       Function to log IPV6 address parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       ipV6Ptr - pointer to IPV6 address.
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
void prvCpssLogStcIpV6
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U8                  * ipV6Ptr
)
{
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n", fieldName,
                ipV6Ptr[0],ipV6Ptr[1],ipV6Ptr[2], ipV6Ptr[3],ipV6Ptr[4],ipV6Ptr[5],ipV6Ptr[6], ipV6Ptr[7],
                ipV6Ptr[8],ipV6Ptr[9],ipV6Ptr[10], ipV6Ptr[11],ipV6Ptr[12],ipV6Ptr[13],ipV6Ptr[14], ipV6Ptr[15]);
}

/*******************************************************************************
* prvCpssLogParamDataCheck
*
* DESCRIPTION:
*       Parameters validation.
*
* INPUTS:
*       skipLog - skip log flag.
*       inOutParamInfoPtr - pointer to parameter log data.
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
GT_BOOL prvCpssLogParamDataCheck
(
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC  * inOutParamInfoPtr
)
{
    if (inOutParamInfoPtr == 0)
    {
        return GT_FALSE;
    }
    return (skipLog) ? GT_FALSE : GT_TRUE;
}

/*******************************************************************************
* cpssLogHistoryDump
*
* DESCRIPTION:
*       Dump log history database.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - if history log disabled
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogHistoryDump
(
    GT_VOID
)
{
    if (prvCpssLogStackSize == 0)
    {
        cpssOsLog(prvCpssLogApiContextLib, CPSS_LOG_TYPE_INFO_E, "cpssLogHistoryDump: API history log disabled!\n");
        return GT_FAIL;
    }

    if (currentHistoryEntryPtr == NULL)
    {
        cpssOsLog(prvCpssLogApiContextLib, CPSS_LOG_TYPE_INFO_E, "cpssLogHistoryDump: API history log - NULL pointer!\n");
        return GT_FAIL;
    }

    CPSS_API_LOCK_MAC(0,0);

    /* Start logging from list head */
    prvCpssLogApiHistoryDbLogOutput(currentHistoryEntryPtr->nextEntryPtr, currentHistoryEntryNum);

    CPSS_API_UNLOCK_MAC(0,0);

    return GT_OK;
}

/*******************************************************************************
* prvCpssLogParamFuncStcTypeArray
*
* DESCRIPTION:
*       Log array in structure
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       firstElementPtr -
*           pointer to first array element
*       elementNum -
*           number of array elements
*       elementSize -
*           size of array element in bytes
*       logFunc -
*           pointer to specific type log function
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
GT_VOID prvCpssLogParamFuncStcTypeArray
(
    IN CPSS_LOG_LIB_ENT        contextLib,
    IN CPSS_LOG_TYPE_ENT       logType,
    IN GT_CHAR_PTR             namePtr,
    IN GT_VOID               * firstElementPtr,
    IN GT_U32                  elementNum,
    IN GT_U32                  elementSize,
    IN PRV_CPSS_LOG_STC_FUNC   logFunc,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    GT_U32 i;
    GT_U8 * currPtr;

    if (firstElementPtr == NULL)
    {
        prvCpssLogTabs(contextLib, logType);
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }

    currPtr = (GT_U8 *)firstElementPtr;
    /* Log all array elements */
    for (i = 0; i < elementNum; i++)
    {
        cpssOsSprintf(buffer2,"%s[%d]", namePtr, i);
        logFunc(contextLib, logType, buffer2, (GT_VOID *)currPtr, inOutParamInfoPtr);
        /* Increment pointer address */
        currPtr += elementSize;
    }
}

/*******************************************************************************
* prvCpssLogParamFuncStcTypeArray2
*
* DESCRIPTION:
*       Log two-dimentional array
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       firstElementPtr -
*           pointer to first array element
*       rowNum -
*           number of array rows
*       rowNum -
*           number of array columns
*       elementSize -
*           size of array element in bytes
*       logFunc -
*           pointer to specific type log function
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
GT_VOID prvCpssLogParamFuncStcTypeArray2
(
    IN CPSS_LOG_LIB_ENT        contextLib,
    IN CPSS_LOG_TYPE_ENT       logType,
    IN GT_CHAR_PTR             namePtr,
    IN GT_VOID               * firstElementPtr,
    IN GT_U32                  rowNum,
    IN GT_U32                  colNum,
    IN GT_U32                  elementSize,
    IN PRV_CPSS_LOG_STC_FUNC   logFunc,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    GT_U32 i, j;
    GT_U8 * currPtr;

    (void)colNum;
    if (firstElementPtr == NULL)
    {
        prvCpssLogTabs(contextLib, logType);
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }

    currPtr = (GT_U8 *)firstElementPtr;
    /* Log all array elements */
    for (i = 0; i < rowNum; i++)
    {
        for (j = 0; j < colNum; j++)
        {

            cpssOsSprintf(buffer2, "%s[%d][%d]", namePtr, i, j);
            logFunc(contextLib, logType, buffer2, (GT_VOID *)currPtr, inOutParamInfoPtr);
            /* Increment pointer address */
            currPtr += elementSize;
        }
    }
}

/*******************************************************************************
* getArraySize
*
* DESCRIPTION:
*       extract array size from param info
*
* INPUTS:
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*           inOutParamInfoPtr->paramKey.paramKeyArr[0] - pointer to output array size
*           inOutParamInfoPtr->paramKey.paramKeyArr[1] - the array max size
*           inOutParamInfoPtr->paramKey.paramKeyArr[2] - size of an input array
*           inOutParamInfoPtr->paramKey.paramKeyArr[3] - value of 1 is a sign that the output might be an array of strucutures
*
* OUTPUTS:
*       arrSizePtr -
*           pointer to the array size
*
* RETURNS :
*       GT_OK - if the extraction successed
*       GT_BAD_PARAM - otherwise  
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getArraySize
(   
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr,
    OUT GT_U32                 * arrSizePtr
)
{
    GT_U32 size, maxSize; /*index, number of structures, max number of structures */
    GT_U32  *sizePtr; /* pointer for size of an output array */

    sizePtr = (GT_U32 *)inOutParamInfoPtr->paramKey.paramKeyArr[0]; /* extracting the pointer to the output array size */
    maxSize = (GT_U32)inOutParamInfoPtr->paramKey.paramKeyArr[1]; /* extracting the max size valid for the output array */
    if (sizePtr != 0) /* the array is output paramenter */
    {
        size = *sizePtr; /* getting the size of the output array */
        if ((*sizePtr > maxSize) || (size == 0)) /* the size is not valid, the array is not valid, no need to log it */
        {
            arrSizePtr = NULL;
            return GT_BAD_PARAM;
        }
    }
    else
    {
        /* if it is output array the sizePtr is null, there is no structure to output - no need to log it  */
        if (inOutParamInfoPtr->paramKey.paramKeyArr[3] == 1)
        {
            arrSizePtr = NULL;
            return GT_BAD_PARAM;
        }
        size = (GT_U32)inOutParamInfoPtr->paramKey.paramKeyArr[2]; /* extracting number of structures (input parameter) */
    }
    *arrSizePtr = size;
    return GT_OK;
}

/*******************************************************************************
* prvCpssLogArrayOfParamsHandle
*
* DESCRIPTION:
*       Log array of parameters (input and output arrays)
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       firstElementPtr -
*           pointer to first array element
*       elementSize -
*           size of array element in bytes
*       logFunc -
*           pointer to specific type log function
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*           inOutParamInfoPtr->paramKey.paramKeyArr[0] - pointer to output array size
*           inOutParamInfoPtr->paramKey.paramKeyArr[1] - the array max size
*           inOutParamInfoPtr->paramKey.paramKeyArr[2] - size of an input array
*           inOutParamInfoPtr->paramKey.paramKeyArr[3] - value of 1 is a sign that the output might be an array of strucutures 
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
GT_VOID prvCpssLogArrayOfParamsHandle
(
    IN CPSS_LOG_LIB_ENT        contextLib,
    IN CPSS_LOG_TYPE_ENT       logType,
    IN GT_CHAR_PTR             namePtr,
    IN GT_VOID               * firstElementPtr,
    IN GT_U32                  elementSize,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr,
    IN PRV_CPSS_LOG_STC_FUNC   logFunc
)
{
    GT_U32 size; /* pointer for size of an output array */
    GT_STATUS rc; /* result status */
    
    rc = getArraySize(inOutParamInfoPtr, &size); /* extracting the array size using helper function */
    if (rc != GT_OK) 
    {
        prvCpssLogStcLogStart(contextLib, logType, namePtr);
        prvCpssLogStcLogEnd(contextLib, logType);
        return;
    }

    if (size > 0) /* array */
    {
        prvCpssLogStcLogStart(contextLib, logType, namePtr);
        prvCpssLogParamFuncStcTypeArray(contextLib, logType, namePtr, firstElementPtr, size, elementSize, logFunc, inOutParamInfoPtr); /* Log all array elements */
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    else /* one structure */
        logFunc(contextLib,logType,namePtr,firstElementPtr,inOutParamInfoPtr);
}
/*******************************************************************************
* GT_VOID prvCpssLogArrayOfEnumsHandle
*
* DESCRIPTION:
*       Log array of parameters (input and output arrays)
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name
*       firstElementPtr -
*           pointer to first enum in the array
*       enumSize -
*           size of the enum
*       enumArray -
*           key value array of the enum values
*       enumArrayEntries -
*           the size of the enum array
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*           inOutParamInfoPtr->paramKey.paramKeyArr[0] - pointer to output array size
*           inOutParamInfoPtr->paramKey.paramKeyArr[1] - the array max size
*           inOutParamInfoPtr->paramKey.paramKeyArr[2] - size of an input array
*           inOutParamInfoPtr->paramKey.paramKeyArr[3] - value of 1 is a sign that the output might be an array of strucutures 
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
GT_VOID prvCpssLogArrayOfEnumsHandle
(
    IN CPSS_LOG_LIB_ENT        contextLib,
    IN CPSS_LOG_TYPE_ENT       logType,
    IN GT_CHAR_PTR             namePtr,
    IN GT_VOID               * firstElementPtr,
    IN GT_U32                  enumSize,
    IN char                  * enumArray[],
    IN GT_U32                  enumArrayEntries,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    GT_U32  i, size; /* index, the array size */
    GT_U8   *currPtr; /* temporary pointer for iterating on the array */
    GT_U32  *enumPtr; /* pointer for the current enum */
    GT_STATUS rc; /* result status */
    
    rc = getArraySize(inOutParamInfoPtr, &size); /* extracting the array size using helper function */
    if (rc != GT_OK) 
    {
        prvCpssLogStcLogStart(contextLib, logType, namePtr);
        prvCpssLogStcLogEnd(contextLib, logType);
        return;
    }

    enumPtr = (GT_U32*)firstElementPtr;
    if (size > 0) /* array */
    {
        currPtr = (GT_U8 *)firstElementPtr;
        prvCpssLogStcLogStart(contextLib, logType, namePtr);
        /* Log all array elements */
        for (i = 0; i < size; i++)
        {
            enumPtr = (GT_U32 *)currPtr;
            cpssOsSprintf(buffer2,"%s[%d]", namePtr, i);
            prvCpssLogEnum(contextLib, logType, buffer2, enumArray, *enumPtr, enumArrayEntries);
            /* Increment pointer address */
            currPtr += enumSize; 
        }
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    else /* one structure */
    {
        enumPtr = (GT_U32 *)firstElementPtr;
        prvCpssLogEnum(contextLib, logType, namePtr, enumArray, *enumPtr, enumArrayEntries);
    }
}
/*******************************************************************************
* cpssLogHistoryFileNameSet
*
* DESCRIPTION:
*       Set log history file name.
*
* INPUTS:
*       fileNamePtr - pointer to history file name string
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - on null pointer
*       GT_BAD_PARAM    - string exceeds maximmal size
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogHistoryFileNameSet
(
    IN const char * fileNamePtr
)
{
    if (fileNamePtr == NULL)
    {
        return GT_BAD_PTR;
    }

    if (cpssOsStrlen(fileNamePtr) > 80)
    {
        return GT_BAD_PARAM;
    }

    CPSS_API_LOCK_MAC(0, 0);
    cpssOsSprintf(prvCpssLogHistoryName, "%s", fileNamePtr);

    /* Clean up database structure */
    prvCpssLogApiHistoryDbClose(&currentHistoryEntryPtr, &currentHistoryEntryNum, GT_FALSE);
    CPSS_API_UNLOCK_MAC(0, 0);

    return GT_OK;
}

/*******************************************************************************
* prvCpssLogStateCheck
*
* DESCRIPTION:
*       Debug function to check log readiness before new API call
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - log is ready to run
*       GT_BAD_STATE    - log is state machine in bad state to run log
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssLogStateCheck
(
    GT_VOID
)
{
    CPSS_API_LOCK_MAC(0,0);
    /* Check context of entrance */
    if (prvCpssLogApiContextIndex != 0 ||
        prvCpssLogApiContextLib != CPSS_LOG_LIB_ALL_E ||
        prvCpssLogEntryFuncId != PRV_CPSS_LOG_NOT_VALID_CNS)
    {
        return GT_BAD_STATE;
    }
    CPSS_API_UNLOCK_MAC(0,0);

    return GT_OK;
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
GT_VOID cpssLogLineNumberEnableSet
(
     IN GT_BOOL enable
)
{
    prvCpssLogLineNumberEnabled = enable;
}

/*******************************************************************************
* cpssLogLineNumberEnableGet
*
* DESCRIPTION:
*       The function gets the status of the using of a sorce code line number
*       by some logging functions
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_TRUE         - if enabled
*       GT_FALSE        - if disabled
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL cpssLogLineNumberEnableGet
(
    GT_VOID
)
{
    return prvCpssLogLineNumberEnabled;
}

/*******************************************************************************
* cpssLogPointerFormatSet
*
* DESCRIPTION:
*       Set a format of pointer values
*
* INPUTS:
*       value - a format type
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
GT_VOID cpssLogPointerFormatSet
(
     IN CPSS_LOG_POINTER_FORMAT_ENT value
)
{
    prvCpssLogPointerFormat = value;
}

/*******************************************************************************
* cpssLogPointerFormatGet
*
* DESCRIPTION:
*       get a format used for a pointer-values logging.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS :
*       a format used for a pointer-values logging.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CPSS_LOG_POINTER_FORMAT_ENT cpssLogPointerFormatGet
(
    GT_VOID
)
{
    return prvCpssLogPointerFormat;
}

/*******************************************************************************
* prvCpssLogParamFunc_GT_U8_PTR_ARRAY
*
* DESCRIPTION:
*       logs an array of GT_U8 pointers
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_U8_PTR_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U8**, paramVal);
    if (paramVal == NULL) 
    {
        prvCpssLogStcLogStart(contextLib, logType, namePtr);
        prvCpssLogStcLogEnd(contextLib, logType);
        return;
    }
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_U8 *),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_U8_PTR_PTR);
}

/*******************************************************************************
* prvCpssLogParamFunc_GT_U8_ARRAY
*
* DESCRIPTION:
*       logs an array of GT_U8 elements
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_U8_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U8*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_U8),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_U8_PTR);
}

/*******************************************************************************
* prvCpssLogParamFunc_GT_U32_ARRAY
*
* DESCRIPTION:
*       logs an array of GT_U32 elements
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_U32_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U32*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_U32),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_U32_PTR);
}

/*******************************************************************************
* prvCpssLogParamFunc_PACKET_PTR
*
* DESCRIPTION:
*       logs an array of GT_U8 pointers as a packet (stream of bytes)
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_PACKET_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    GT_U32 size; /* pointer for size of an output array */
    GT_STATUS rc; /* result status */
    
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U8*, paramVal);

    rc = getArraySize(inOutParamInfoPtr, &size); /* extracting the packet size using helper function */
    /* if the packet size is not valid or the packet size is 0 or the pointer to the packet is NULL, there is nothing to log */
    if (rc != GT_OK || size == 0 || paramVal == NULL)
    {
        prvCpssLogStcLogStart(contextLib, logType, namePtr);
        prvCpssLogStcLogEnd(contextLib, logType);
        return;
    }

    /* log the packet using the hekper function prvCpssLogPacket */
    prvCpssLogStcLogStart(contextLib, logType, namePtr);
    prvCpssLogPacket(contextLib,logType,size,paramVal);
    prvCpssLogStcLogEnd(contextLib, logType);
}

/*******************************************************************************
* prvCpssLogParamFunc_PACKET_PTR_ARRAY
*
* DESCRIPTION:
*       logs an array of packets pointers
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_PACKET_PTR_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    GT_U32 i, size; /*index, number of packets */
    GT_U8  *currBufferPtr; /* temporary pointer to the buffer we working on */
    GT_U32 *currLenPtr; /* temporary pointer to the size of the packet we working on */
    GT_STATUS rc; /* result status */

    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U8**, paramVal);
    if (paramVal == NULL) 
    {
        prvCpssLogTabs(contextLib, logType);
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }

    rc = getArraySize(inOutParamInfoPtr, &size); /* extracting the packet size using helper function */
    /* if the size is not valid or the packet size is 0, there is nothing to log */
    if (rc != GT_OK || size == 0)
    {
        prvCpssLogStcLogStart(contextLib, logType, namePtr);
        prvCpssLogStcLogEnd(contextLib, logType);
        return;
    }

    currLenPtr = (GT_U32 *)inOutParamInfoPtr->paramKey.paramKeyArr[4]; /* extracting the pointer to the output array of packets size */
    if (currLenPtr == NULL)
    {
        /* length array is not valid. log as array of pointers. */
        prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_U8 *),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_U8_PTR_PTR);
        return;
    }

    /* iterate over both arrays (packets and sizes) for loging the packets */
    prvCpssLogStcLogStart(contextLib, logType, namePtr);
    /* Log all array elements */
    for (i = 0; i < size; i++)
    {
        currBufferPtr  = paramVal[i];

        cpssOsSprintf(buffer2,"%s[%d]", namePtr, i);
        if (currBufferPtr == NULL)
        {
            prvCpssLogTabs(contextLib, logType);
            PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = NULL\n", buffer2);
        }
        else 
        {
            prvCpssLogStcLogStart(contextLib, logType, buffer2);
            prvCpssLogPacket(contextLib,logType,*currLenPtr,currBufferPtr);
            prvCpssLogStcLogEnd(contextLib, logType);
        }

        /* Increment pointer address */
        currLenPtr++;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}

/*******************************************************************************
* prvCpssLogParamFunc_GT_BOOL_ARRAY
*
* DESCRIPTION:
*       logs an array of GT_BOOL elements
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_BOOL_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_BOOL*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,paramVal,sizeof(GT_BOOL),inOutParamInfoPtr,prvCpssLogParamFuncStc_GT_BOOL_PTR);
}

/*******************************************************************************
* prvCpssLogParamFunc_DSA_PTR
*
* DESCRIPTION:
*       logs an array of GT_U8 as DSA (array of words)
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_DSA_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    GT_U32  word; /* for saving a word in DSA tag */

    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_U8*, paramVal);
    if (paramVal == NULL) 
    {
        prvCpssLogTabs(contextLib, logType);
        PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }

    /* first word is always exists */
    word = ((paramVal[0]) << 24) |
           ((paramVal[1]) << 16) |
           ((paramVal[2]) << 8)  |
           ((paramVal[3]) << 0) ;
    prvCpssLogStcLogStart(contextLib, logType, namePtr);
    prvCpssLogTabs(contextLib, logType);   
    cpssOsLog(contextLib,logType,"%s[0] = %x\n", namePtr,word); /*log first word*/

    /* check the extended bit */
    if(0 == U32_GET_FIELD_MAC(word,12,1))
    {
        /* word 0 is the last word in the DSA tag */
        prvCpssLogStcLogEnd(contextLib, logType);
        return;
    }
    /* copy the data of the second word */
    word = ((paramVal[4]) << 24) |
           ((paramVal[5]) << 16) |
           ((paramVal[6]) <<  8) |
           ((paramVal[7]) <<  0) ;
    prvCpssLogTabs(contextLib, logType);
    cpssOsLog(contextLib,logType,"%s[1] = %x\n", namePtr,word); /*log second word*/

    /* check the extended bit */
    if(0 == U32_GET_FIELD_MAC(word,31,1))
    {
        /* word 1 is the last word in the DSA tag */
        prvCpssLogStcLogEnd(contextLib, logType);
        return;
    }
    /* copy the data of the third word */
    word = ((paramVal[8]) << 24) |
           ((paramVal[9]) << 16) |
           ((paramVal[10]) <<  8) |
           ((paramVal[11]) <<  0) ;
    prvCpssLogTabs(contextLib, logType);
    cpssOsLog(contextLib,logType,"%s[2] = %x\n", namePtr,word); /*log third word*/

    /* check the extended bit */
    if(0 == U32_GET_FIELD_MAC(word,31,1))
    {
        /* 3 words DSA tag is not supported */
        prvCpssLogStcLogEnd(contextLib, logType);
        return;
    }
    /* copy the data of the forth word */
    word = ((paramVal[12]) << 24) |
           ((paramVal[13]) << 16) |
           ((paramVal[14]) <<  8) |
           ((paramVal[15]) <<  0) ;
    prvCpssLogTabs(contextLib, logType);
    cpssOsLog(contextLib,logType,"%s[3] = %x\n", namePtr,word); /*log fourth word*/
    prvCpssLogStcLogEnd(contextLib, logType);
}

/*******************************************************************************
* prvCpssLogParamFunc_GT_CHAR_ARRAY
*
* DESCRIPTION:
*       logs an array of GT_CHAR as a string
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_CHAR_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_CHAR*, paramVal);
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = %s\n", namePtr, paramVal);
}

/*******************************************************************************
* prvCpssLogStcHexNumber
*
* DESCRIPTION:
*       The function to log hex parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
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
void prvCpssLogStcHexNumber
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U32                   numValue
)
{
    PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(numValue);
    prvCpssLogTabs(contextLib, logType);
    PRV_CPSS_LOG_AND_HISTORY_MAC(PRV_CPSS_LOG_PHASE_INFO_E, contextLib, logType, "%s = 0x%08x\n", fieldName, numValue);
}

/*******************************************************************************
* prvCpssLogParamFuncStc_GT_U32_HEX_PTR
*
* DESCRIPTION:
*       logs a variable from GT_U32 type in hexa format
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       fieldPtr -
*           pointer to the field
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFuncStc_GT_U32_HEX_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_FIELD_VAL_PTR_MAC(GT_U32*, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    prvCpssLogStcHexNumber(contextLib, logType, namePtr, *valPtr);
}
