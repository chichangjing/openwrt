/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdEngine.c
*
* DESCRIPTION:
*       commander database engine code, responsible for parsing single
*       command and execution of appropriate api function declared in
*       cmdBase.h header file.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/os/cmdExecute.h>
#include <cmdShell/shell/cmdParser.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/shell/cmdEngine.h>
#include <gtUtil/gtTimeMeasure.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <cmdShell/common/cpssApiCallLock.h>

static GT_CHAR cmdDupBuffer[CMD_MAX_BUFFER];
static GT_U32 cmdDupBufferOffset = 0;

/* commands input argument buffer */
static GT_UINTPTR inputBuffer[CMD_MAX_ARGS];

/* commands fields input argument buffer */
static GT_UINTPTR fieldBuffer[CMD_MAX_FIELDS];

/* commands output argument buffer */
static GT_CHAR outputBuffer[CMD_MAX_BUFFER];

/* Number of input fields */
static GT_U32        numFields = 0;

/* function (command) to invoke */
static cmdWrapperFuncPtr   wrapperRef;

/* Mutual exclusion semaphore to protect
 *    inputBuffer, fieldBuffer, outputBuffer, wrapperRef, etc
 * Used by cmdEventRun(), cmdWrapperRun()
 */
static GT_SEM  gRunMtx;

/* The command task wait on this semaphore */
static GT_SEM  gSemCmd;

/* The processing task wait on this semaphore */
static GT_SEM  gSemProc;

/* Processing task ID */
static GT_U32  gProcTaskId;

/* Function invokation return code */
static GT_STATUS gRc;

/* Is it a shell command*/
static GT_BOOL isShellCmd;

/* Shell input buffer pointer */
static GT_CHAR *gShellInBufferPtr;

/* If the timeout has expired */
static GT_U32 timeOutExpired = 0;

/* Wait maximum 30 seconds */
#define CMD_PROC_TIMOUT 30000

/* Wait maximum 3 minutes */
static GT_U32 cmdSysTimeout = (CMD_PROC_TIMOUT * 6);

/* command time measure enable*/
static GT_BOOL prvCmdTimeMeasureEnable = GT_FALSE;

#define CMD_EXE_TASK_PRIO (CMD_THREAD_PRIO - 1)

/* The shell message */
#define SHELL_CMD_MSG "shell command executed\r\n"
#define SHELL_CMD_MSG_LENGTH sizeof(SHELL_CMD_MSG)
#if 0
#define DEBUG_ENGINE_STREAM
#endif

/*******************************************************************************
* wrCmdTimeMeasureEnableSet
*
* DESCRIPTION:
*       Set Command Time Measuring enable.
* INPUTS:
*       Galtis Wrapper input
* OUTPUTS:
*       Galtis Wrapper output
* RETURNS:
*       Galtis Wrapper return codes
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCmdTimeMeasureEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    prvCmdTimeMeasureEnable = (GT_BOOL)inArgs[0];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrGtTimeExactDumpNodes
*
* DESCRIPTION:
*       Dump Exact Time Nodes.
* INPUTS:
*       Galtis Wrapper input
* OUTPUTS:
*       Galtis Wrapper output
* RETURNS:
*       Galtis Wrapper return codes
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrGtTimeExactDumpNodes
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gtTimeExactDumpNodes();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrGtTimeExactResetNodes
*
* DESCRIPTION:
*       Set Command Time Measuring enable.
* INPUTS:
*       Galtis Wrapper input
* OUTPUTS:
*       Galtis Wrapper output
* RETURNS:
*       Galtis Wrapper return codes
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrGtTimeExactResetNodes
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32      numFields,
    OUT GT_8       outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gtTimeExactResetNodes();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_COMMAND dbCommands[] =
{
    {"cmdTimeMeasureEnableSet",
     &wrCmdTimeMeasureEnableSet,
     1, 0},
    {"gtTimeExactDumpNodes",
     &wrGtTimeExactDumpNodes,
     0, 0},
    {"gtTimeExactResetNodes",
     &wrGtTimeExactResetNodes,
     0, 0}
};

#define PRV_NUM_OF_COMMANDS_CNS (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdProcTask
*
* DESCRIPTION:
*       Command's processor.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on fialure
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS __TASKCONV cmdProcTask(GT_VOID *params)
{

    GT_STATUS rc; /* Return code */

/* for private versions (for ex. automatic cpssInitSystem before shell) */
#ifdef PRE_SHELL_FUNCTION
    {
        /* declaration */
        GT_VOID PRE_SHELL_FUNCTION(GT_VOID);

        PRE_SHELL_FUNCTION();
    }
#endif /*PRE_SHELL_FUNCTION*/

    rc = cmdInitLibrary(dbCommands, PRV_NUM_OF_COMMANDS_CNS);
    if (rc != GT_OK)
    {
        return rc;
    }

    while (GT_TRUE)
    {
        /* Wait for incomming command to invoke */
        rc = cmdOsSigSemWait(gSemProc, CPSS_OS_SEM_WAIT_FOREVER_CNS);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (isShellCmd == GT_TRUE)
        {
            if (prvCmdTimeMeasureEnable != GT_FALSE)
            {
                GT_DEFINE_TIME_EXACT_NODE("SHELL_NOT_GALTIS_COMMAND_START");
            }
            CPSS_API_CALL_LOCK();
            gRc = osShellExecute(gShellInBufferPtr);
            CPSS_API_CALL_UNLOCK();
            if (prvCmdTimeMeasureEnable != GT_FALSE)
            {
                GT_DEFINE_TIME_EXACT_NODE("SHELL_NOT_GALTIS_COMMAND_END");
            }
        }
        else
        {
    #ifdef DEBUG_ENGINE_STREAM
            cmdOsPrintf("\nCalled function\n");
    #endif
            if (prvCmdTimeMeasureEnable != GT_FALSE)
            {
                GT_DEFINE_TIME_EXACT_NODE("SHELL_GALTIS_COMMAND_START");
            }
            CPSS_API_CALL_LOCK();
            gRc = wrapperRef(inputBuffer, fieldBuffer, numFields, (GT_8*)outputBuffer);
            CPSS_API_CALL_UNLOCK();
            if (prvCmdTimeMeasureEnable != GT_FALSE)
            {
                GT_DEFINE_TIME_EXACT_NODE("SHELL_GALTIS_COMMAND_END");
            }
        }

        /* Signal command that invokation comleted */
        rc = cmdOsSigSemSignal(gSemCmd);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
}

/*******************************************************************************
* cmdEngineInit
*
* DESCRIPTION:
*       initializes engine. Spawn new task for command execution.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on fialure
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdEngineInit(GT_VOID)
{

    GT_STATUS rc; /* Return code*/
    GT_U32    stackSize; /* task stack size */

    /* Allocate EventRun mutex. Use semaphore because it locks resources
     * for long period
     */
    rc = cmdOsSigSemBinCreate("cmdShellMtx", CPSS_OS_SEMB_FULL_E, &gRunMtx);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Allocate command semaphore */
    rc = cmdOsSigSemBinCreate("cmdShellSem", CPSS_OS_SEMB_EMPTY_E, &gSemCmd);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Allocate command Processor semaphore */
    rc = cmdOsSigSemBinCreate("cmdProcSem", CPSS_OS_SEMB_EMPTY_E, &gSemProc);
    if (rc != GT_OK)
    {
        return rc;
    }

#ifdef GM_USED
    /* GM need huge stack size to take care long recursive calls */
    stackSize = 5000000;
#else
    stackSize = 0x20000; /* 128K */
#endif

    /* Create command execute task*/
    if (cmdOsTaskCreate(
            "cmdExec",
            CMD_EXE_TASK_PRIO,     /* thread priority      */
            stackSize,             /* thread stack size    */
            cmdProcTask,
            NULL,
            &gProcTaskId) != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cmdClearDupBuff
*
* DESCRIPTION:
*       initializes (clears) internal cyclic buffer, used as static input and
*       field container.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS cmdClearDupBuff(GT_VOID)
{
    cmdDupBufferOffset = 0;
    return GT_OK;
}

/*******************************************************************************
* cmdDuplicateString
*
* DESCRIPTION:
*       duplicates null terminated string and stores it in static buffer.
*
* INPUTS:
*       src     - pointer to source NULL terminatyed string to be duplicated
*
* OUTPUTS:
*       dest    - pointer to "safe" duplicated string
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure (out of memory - increase CMD_MAX_BUFFER size)
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS cmdDuplicateString
(
    GT_CHAR *src,
    GT_CHAR **dest
)
{
    GT_U32 length = cmdOsStrlen(src) + 1;
    if (length + cmdDupBufferOffset > CMD_MAX_BUFFER)
    {
        *dest = NULL;
        return GT_FAIL;
    }
    cmdOsStrCpy(cmdDupBuffer + cmdDupBufferOffset, src);
    *dest = cmdDupBuffer + cmdDupBufferOffset;
    cmdDupBufferOffset += length;
    return GT_OK;
}

/*******************************************************************************
* cmdReadFields
*
* DESCRIPTION:
*       insert into fields[] the table fields of the calling command - if the
*       table has 2 fields  fields[0] = table field 1, fields[1] = table field 2
*
* INPUTS:
*       fieldBuffer - fields ascii buffer read from command line
*
* OUTPUTS:
*       fields      - the recived table fields
*       numFields   - number of fields read
*
* RETURNS:
*       GT_OK   - read successful
*       GT_FAIL - insufficient or too many fields in buffer
*
* COMMENTS:
*       none
*
*******************************************************************************/
static GT_STATUS cmdReadFields
(
    IN  GT_CHAR   *fieldBuffer,
    OUT GT_UINTPTR  fields[],
    OUT GT_U32 *numFields
)
{
    GT_U32 fieldEnd = 0, field = 0, token;
    GT_CHAR *string;

    if (cmdParseInit(fieldBuffer) != GT_OK)
        return GT_FAIL;     /* internal error */

    while (!fieldEnd) {
        if (cmdParseRead() != GT_OK)
            return GT_FAIL;     /* not enough arguments */
        if (cmdParseGet(&token) != GT_OK)
            return GT_FAIL;
        switch (token) {
            case tokDECIMAL:
            case tokHEXADECIMAL:
                if (cmdParseGetNumeric(fields + (field++)) != GT_OK)
                    return GT_FAIL;
                break;
            case tokSTRING:
                if (cmdParseGetString(&string) != GT_OK)
                    return GT_FAIL;
                if (cmdDuplicateString(string,
                    (GT_CHAR**) (fields + (field++))) != GT_OK)
                        return GT_FAIL;
                break;
            case ((GT_U32)tokEOF):
                fieldEnd = 1;
                break;
            default:
                return GT_FAIL;
        }
    }
    *numFields = field;
    return (field > 0) ? GT_OK : GT_FAIL;
}

/*******************************************************************************
* cmdEventFields
*
* DESCRIPTION:
*       parses input buffer, and tells whether command has fields as input
*
* INPUTS:
*       inBuffer    - null terminated string holding command buffer
*
* RETURNS:
*       GT_TRUE     - fields need to be read
*       GT_FALSE    - command has no fields as input
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_BOOL cmdEventFields
(
    GT_CHAR *inBuffer
)
{
    CMD_COMMAND  *command;
    GT_CHAR      *commandName;
    GT_U32        token;

    if (cmdParseInit(inBuffer) != GT_OK)
        return GT_FALSE;    /* internal error */

    if (cmdParseRead() != GT_OK)
        return GT_FALSE;    /* internal error */

    if (cmdParseGet(&token) != GT_OK)
        return GT_FALSE;    /* internal error */

    if (token == tokEOF)
        return GT_FALSE;

    if (cmdParseGetConstant(&commandName) != GT_OK)
        return GT_FALSE;    /* invalid syntax */

    if (cmdGetCommand(commandName, &command) != GT_OK)
        return GT_FALSE;

    return (command->funcFields > 0) ? GT_TRUE : GT_FALSE;
}


/*******************************************************************************
* cmdEventRunTimeoutSet
*
* DESCRIPTION:
*       Set timeout for running a single command by cmdEventRun function.
*
* INPUTS:
*       cmdTimeout - command run timeout
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdEventRunTimeoutSet
(
    IN GT_U32 cmdTimeout
)
{
    cmdSysTimeout = cmdTimeout;

    return GT_OK;
}


/*******************************************************************************
* cmdEventRun
*
* DESCRIPTION:
*       command interpreter; parses and executes single command stored
*       in null terminated string.
*
* INPUTS:
*       inBuffer    - null terminated string holding command buffer
*       inFieldBuffer - null terminated string holding field values
*
* OUTPUTS:
*       outBuffer   - pointer to null terminated string holding output
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS cmdEventRun
(
    GT_CHAR *inBuffer,
    GT_CHAR *inFieldBuffer,
    GT_CHAR **outBuffer
)
{

    CMD_COMMAND  *command;
    GT_CHAR      *commandName, *string;
    GT_U32        token, args, i;
    GT_STATUS     status;


    if (cmdParseInit(inBuffer) != GT_OK)
    {
        return CMD_AGENT_ERROR;     /* internal error */
    }

    if (cmdParseRead() != GT_OK)
    {
        return CMD_AGENT_ERROR;     /* internal error */
    }

    if (cmdParseGet(&token) != GT_OK)
    {
        return CMD_AGENT_ERROR;     /* internal error */
    }
    if (token == tokEOF)
    {
        *outBuffer = "";
        return CMD_OK;
    }

    cmdOsSigSemWait(gRunMtx, CPSS_OS_SEM_WAIT_FOREVER_CNS);

    cmdClearDupBuff();
    status = cmdParseGetConstant(&commandName);
    if (status != GT_OK)
    {
        cmdOsSigSemSignal(gRunMtx);
        return CMD_SYNTAX_ERROR;    /* invalid syntax */
    }
    /* Non command shell*/
    isShellCmd = GT_FALSE;
    if ((inBuffer[0] == '@') && (inBuffer[1] == '@'))
    {
        isShellCmd = GT_TRUE;
        gShellInBufferPtr = inBuffer + 2;

    }
    if ((isShellCmd == GT_FALSE) &&
        ((status = cmdGetCommand(commandName, &command)) == GT_OK))
    {
        /* Resete time out*/
        timeOutExpired = 0;

        args = command->funcArgs;
        /* argument parsing pipeline */
        for (i = 0; i < args; i++)
        {
            if (i != 0)
            {
                if (cmdParseRead() != GT_OK)
                {
                    cmdOsSigSemSignal(gRunMtx);
                    return CMD_AGENT_ERROR;     /* internal error */
                }
                if (cmdParseGet(&token) != GT_OK)
                {
                    cmdOsSigSemSignal(gRunMtx);
                    return CMD_AGENT_ERROR;     /* internal error */
                }
                if (token != tokCOMMA) {
                    if (token == tokEOF)
                    {
                        cmdOsSigSemSignal(gRunMtx);
                        return CMD_ARG_UNDERFLOW;
                    }
                    cmdOsSigSemSignal(gRunMtx);
                    return CMD_SYNTAX_ERROR;    /* invalid syntax */
                }
            }

            if (cmdParseRead() != GT_OK)
            {
                cmdOsSigSemSignal(gRunMtx);
                return CMD_SYNTAX_ERROR;        /* invalid syntax */
            }
            if (cmdParseGet(&token) != GT_OK)
            {
                cmdOsSigSemSignal(gRunMtx);
                return CMD_ARG_UNDERFLOW;
            }
            switch (token)
            {
                /* numeric argument */
                case tokDECIMAL:
                case tokHEXADECIMAL:
                    if(cmdParseGetNumeric((GT_UINTPTR *)(inputBuffer + i)) != GT_OK)
                    {
                        cmdOsSigSemSignal(gRunMtx);
                        return CMD_AGENT_ERROR;   /* invalid / not enough arguments */
                    }
                    break;
                /* character buffer: pass as a pointer to string */
                case tokSTRING:
                    if (cmdParseGetString(&string) != GT_OK)
                    {
                        cmdOsSigSemSignal(gRunMtx);
                        return CMD_AGENT_ERROR;
                    }
                    if (cmdDuplicateString(string,
                        (GT_CHAR**) (inputBuffer + i)) != GT_OK)
                    {
                        cmdOsSigSemSignal(gRunMtx);
                        return CMD_AGENT_ERROR;
                    }
                    break;
                case ((GT_U32)tokEOF):
                    cmdOsSigSemSignal(gRunMtx);
                    return CMD_ARG_UNDERFLOW;
                default:
                    cmdOsSigSemSignal(gRunMtx);
                    return CMD_SYNTAX_ERROR;
            }
        }

        /* read and parse fields, if available */
        if (command->funcFields > 0)
        {
            if(cmdReadFields(inFieldBuffer,fieldBuffer,&numFields)
               != GT_OK)
            {
                cmdOsSigSemSignal(gRunMtx);
                return CMD_SYNTAX_ERROR;
            }
        }

        /* parsing successfull: execute api function */
        /* Take the semaphore - update operation to */
        wrapperRef = command->funcReference;

        /* Signal proc taske that a new command is ready */
        status = cmdOsSigSemSignal(gSemProc);
        if (status != GT_OK)
        {
            cmdOsSigSemSignal(gRunMtx);
            return CMD_AGENT_ERROR;
        }

        while (timeOutExpired < cmdSysTimeout)
        {
            status = cmdOsSigSemWait(gSemCmd, CMD_PROC_TIMOUT);
            /* Time out expired */
            if (status == GT_TIMEOUT)
            {
                timeOutExpired += CMD_PROC_TIMOUT;
            }
            else
            {
                /* Reset timeout */
                timeOutExpired = 0;
                *outBuffer = outputBuffer;
                cmdOsSigSemSignal(gRunMtx);
                return status;
            }
        }

    }
    else
    {
        /* This is a shell command and not Galtis */
        /* shell command may be already recognized by "@@" prefix */
        if (isShellCmd == GT_FALSE)
        {
            isShellCmd = GT_TRUE;
            gShellInBufferPtr = inBuffer;
        }

        /* Signal proc task that a new command is ready */
        status = cmdOsSigSemSignal(gSemProc);
        if (status != GT_OK)
        {
            cmdOsSigSemSignal(gRunMtx);
            return CMD_AGENT_ERROR;
        }

        while (timeOutExpired < cmdSysTimeout)
        {
            status = cmdOsSigSemWait(gSemCmd, CMD_PROC_TIMOUT);
            /* Time out expired - wait 3 minutes*/
            if (status == GT_TIMEOUT)
            {
                timeOutExpired += CMD_PROC_TIMOUT;
            }
            else
            {
                if (gRc == GT_OK)
                {
                    /* Reset time out*/
                    timeOutExpired = 0;
                    /* Copy the shell massage to the output buffer*/
                    cmdOsMemCpy(outputBuffer, SHELL_CMD_MSG, SHELL_CMD_MSG_LENGTH);
                    *outBuffer = outputBuffer;
                    cmdOsSigSemSignal(gRunMtx);
                    return CMD_OK;
                }

                cmdOsSigSemSignal(gRunMtx);
                return CMD_SYNTAX_ERROR; /* command not found */
            }
        }

    }

    cmdOsPrintf("\nCommand timeout expired.\n");
    /* Clear the timeout counter*/
    timeOutExpired = 0;

    cmdOsSigSemSignal(gRunMtx);
    return CMD_OK;
}

/*******************************************************************************
* cmdParseLine
*
* DESCRIPTION:
*       Parses single command stored in null terminated string.
*
* INPUTS:
*       inBuffer      - null terminated string holding command buffer
*       inFieldBuffer - null terminated string holding field values
*
* OUTPUTS:
*       isShellCmd - GT_TRUE, if it is shell command.
*
* RETURNS:
*       CMD_OK   - if there are no problems,
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS cmdParseLine
(
    IN  GT_CHAR      *inBuffer,
    IN  GT_CHAR      *inFieldBuffer,
    OUT GT_BOOL      *isShellCmd
)
{
    GT_CHAR      *commandName, *string;
    CMD_COMMAND  *command;
    GT_U32       numFields;          /* number of fields   */
    GT_U32       token, args, i;
    GT_STATUS    status;

    if (cmdParseInit(inBuffer) != GT_OK)
        return CMD_AGENT_ERROR;     /* internal error */

    if (cmdParseRead() != GT_OK)
        return CMD_AGENT_ERROR;     /* internal error */

    if (cmdParseGet(&token) != GT_OK)
        return CMD_AGENT_ERROR;     /* internal error */

    if (token == tokEOF)
    {
        return CMD_OK;
    }

    cmdClearDupBuff();
    status = cmdParseGetConstant(&commandName);
    if (status != GT_OK)
    {
        return CMD_SYNTAX_ERROR;
    }

        /* Non command shell*/
    *isShellCmd = GT_FALSE;

    /* get command name */
    if ((status = cmdGetCommand(commandName, &command)) == GT_OK)
    {
        args = command->funcArgs;
        /* argument parsing pipeline */
        for (i = 0; i < args; i++)
        {
            if (i != 0)
            {
                if (cmdParseRead() != GT_OK)
                    return CMD_AGENT_ERROR;     /* internal error */

                if (cmdParseGet(&token) != GT_OK)
                    return CMD_AGENT_ERROR;     /* internal error */

                if (token != tokCOMMA) {
                    if (token == tokEOF)
                    {
                        return CMD_ARG_UNDERFLOW;
                    }

                    return CMD_SYNTAX_ERROR;    /* invalid syntax */
                }
            }

            if (cmdParseRead() != GT_OK)
            {
                return CMD_SYNTAX_ERROR;        /* invalid syntax */
            }


            if (cmdParseGet(&token) != GT_OK)
            {
                return CMD_ARG_UNDERFLOW;
            }

            switch (token) {
                /* numeric argument */
                case tokDECIMAL:
                case tokHEXADECIMAL:
                    if(cmdParseGetNumeric(inputBuffer + i) != GT_OK)
                        /* invalid / not enough arguments */
                        return CMD_AGENT_ERROR;
                    break;
                /* character buffer: pass as a pointer to string */
                case tokSTRING:
                    if (cmdParseGetString(&string) != GT_OK)
                        return CMD_AGENT_ERROR;
                    if (cmdDuplicateString(string,
                        (GT_CHAR**) (inputBuffer + i)) != GT_OK)
                            return CMD_AGENT_ERROR;
                    break;
                case ((GT_U32)tokEOF):
                    return CMD_ARG_UNDERFLOW;
                default:
                    return CMD_SYNTAX_ERROR;
            }
        }

        /* read and parse fields, if available */
        if (command->funcFields > 0)
        {
            if(cmdReadFields(inFieldBuffer,fieldBuffer,&numFields)
               != GT_OK)
            {
                return CMD_SYNTAX_ERROR;
            }
        }
    }
    else
    {
        /* This is a shell command and not Galtis*/
        *isShellCmd = GT_TRUE;
        gShellInBufferPtr = inBuffer;
    }

    return CMD_OK;
}


#if defined(CMD_LUA_CLI) && defined(CMD_LUA_GALTIS)

#include <lua.h>
#include "lua/cmdLua.h"


int cmdLuaGaltisWrapper = 0;
static lua_State *gL = NULL;
static GT_BOOL fieldOutputAppendMode = GT_FALSE;
static int nresults;
static int nfields;
static int fieldsIndex;

/*******************************************************************************
* cmdLuaGaltisWrapperPush
*
* DESCRIPTION:
*       Push to stack one galtis output value
*
* INPUTS:
*       format          - the incoming arguments format (%d%c%x%s...). The %f
*                         can be used for adding a pre-prepared fields output
*                         string (by using fieldOutput function)
*       ap              - list of arguments to put in the resultString. For %f
*                         format no argument is needed
*
* OUTPUTS:
*       a value pushed to gL stack
*
* RETURNS:
*       pointer to next item in format string
*
* COMMENTS:
*
*******************************************************************************/
const GT_CHAR* cmdLuaGaltisWrapperPush(const GT_CHAR* format, va_list *argP)
{
    const GT_CHAR *p = format;
    GT_32 arsize, idx;

    while (*p && *p != '%')
        p++;

    if (!*p)
        return p;

    switch (*++p) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '*':
            if (*p == '*')
            {
                p++;
                arsize = va_arg(*argP, GT_32);
            } else {
                arsize = 0;
                while (*p >= '0' && *p <= '9')
                    arsize = (arsize * 10) + (*p++ - '0');
            }
            switch (*p) {
                case 'b':
                case 'B':
                    {
                        GT_U8* dest;
                        GT_U8* array = va_arg(*argP, GT_U8*);

                        dest = (GT_U8*)lua_newuserdata(gL, arsize);
                        for (idx = 0; idx < arsize; idx++) {
                            dest[idx] = array[idx];
                        }
                        cmdLuaSetCustomType(gL, -1, CMD_LUA_TYPE_BYTEARRAY);
                    }
                    break;
                case 'w':
                case 'W':
                    {
                        GT_U8* dest;
                        GT_U16* array = va_arg(*argP, GT_U16*);

                        dest = (GT_U8*)lua_newuserdata(gL, arsize*2);
                        for (idx = 0; idx < arsize; idx++) {
                            dest[idx*2]   = (GT_U8)((array[idx]>>8) & 0xff);
                            dest[idx*2+1] = (GT_U8)( array[idx]     & 0xff);
                        }
                        cmdLuaSetCustomType(gL, -1, CMD_LUA_TYPE_U16ARRAY);
                    }
                    break;
                case 'd':
                case 'D':
                    {
                        GT_U8* dest;
                        GT_U32* array = va_arg(*argP, GT_U32*);

                        dest = (GT_U8*)lua_newuserdata(gL, arsize*4);
                        for (idx = 0; idx < arsize; idx++) {
                            dest[idx*4]   = (GT_U8)((array[idx]>>24) & 0xff);
                            dest[idx*4+1] = (GT_U8)((array[idx]>>16) & 0xff);
                            dest[idx*4+2] = (GT_U8)((array[idx]>>8)  & 0xff);
                            dest[idx*4+3] = (GT_U8)( array[idx]      & 0xff);
                        }
                        cmdLuaSetCustomType(gL, -1, CMD_LUA_TYPE_U32ARRAY);
                    }
                    break;
            }
            break;
        case 'x':
        case 'X':
            /* get the data and point to next parameter in the variable list */
            lua_pushnumber(gL, va_arg(*argP, GT_U32));
            break;
        case 'd':
        case 'D':
            /* get the data and point to next parameter in the variable list */
            lua_pushnumber(gL, va_arg(*argP, GT_32));
            break;

        case 'l':
        case 'L':
            {
                GT_U8* dest;
                GT_U64 *big;
                big = va_arg(*argP, GT_U64*);

                dest = (GT_U8*)lua_newuserdata(gL, 8);
                dest[0] = (GT_U8)((big->l[0]>>24) & 0xff);
                dest[1] = (GT_U8)((big->l[0]>>16) & 0xff);
                dest[2] = (GT_U8)((big->l[0]>>8)  & 0xff);
                dest[3] = (GT_U8)( big->l[0]      & 0xff);
                dest[4] = (GT_U8)((big->l[1]>>24) & 0xff);
                dest[5] = (GT_U8)((big->l[1]>>16) & 0xff);
                dest[6] = (GT_U8)((big->l[1]>>8)  & 0xff);
                dest[7] = (GT_U8)( big->l[1]      & 0xff);
                cmdLuaSetCustomType(gL, -1, CMD_LUA_TYPE_U64);
            }
            break;

        case 's':
        case 'S':
            /* get the data and point to next parameter in the variable list */
            lua_pushstring(gL, va_arg(*argP, GT_8*));
            break;
        default:
            break;
    }
    p++;
    return p;
}


/*******************************************************************************
* cmdLuaGaltisWrapperOutput
*
* DESCRIPTION:
*       captures galtis output
*
* INPUTS:
*       status          - status that will be added to resultString
*       format          - the incoming arguments format (%d%c%x%s...). The %f
*                         can be used for adding a pre-prepared fields output
*                         string (by using fieldOutput function)
*       ...             - list of arguments to put in the resultString. For %f
*                         format no argument is needed
*
* OUTPUTS:
*       pushed to gL stack
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID cmdLuaGaltisWrapperOutput(GT_STATUS status, const GT_CHAR *format, va_list ap)
{
    const GT_CHAR *p = format;
    if (!gL)
        return;
#if 0
    lua_getglobal(gL, "print");
    lua_pushfstring(gL, "cmdLuaGaltisWrapperOutput() status=%d format=%s",status,format);
    lua_call(gL, 1, 0);
#endif
    lua_pushnumber(gL, status);
    nresults = 1;

    if (!*p)
        return;
    if (*p != '%')
    {
        lua_pushstring(gL, format);
        nresults++;
        return;
    }
    while (*p)
    {
        if (*p != '%')
            break;
        if (p[1] == 'f' || p[1] == 'F')
        {
            p += 2;
            lua_pushvalue(gL, fieldsIndex);
            nresults++;
        } else {
            p = cmdLuaGaltisWrapperPush(p, &ap);
            nresults++;
        }
    }
}

/*******************************************************************************
* cmdLuaGaltisWrapperFieldOutput
*
* DESCRIPTION:
*       captures galtis output
*       builds table field on the gL stack at index fieldsIndex
*
* INPUTS:
*       format  - the incoming arguments format (%d%c%x%s...)
*       ...     - list of arguments to put in the fields output string
*
* OUTPUTS:
*       data inserted into table located at gL stack index fieldsIndex
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID cmdLuaGaltisWrapperFieldOutput(const GT_CHAR *format, va_list ap)
{
    const GT_CHAR *p = format;

    if (!gL)
        return;
#if 0
    lua_getglobal(gL, "print");
    lua_pushfstring(gL, "cmdLuaGaltisWrapperFieldOutput() format=%s",format);
    lua_call(gL, 1, 0);
#endif

    if (fieldOutputAppendMode == GT_FALSE)
    {
        lua_newtable(gL);
        lua_replace(gL, fieldsIndex);
        nfields = 0;
    }
    fieldOutputAppendMode = GT_FALSE;

    while (*p)
    {
        p = cmdLuaGaltisWrapperPush(p, &ap);
        lua_rawseti(gL, fieldsIndex, ++nfields);
    }
}

/*******************************************************************************
* fieldOutputSetAppendMode
*
* DESCRIPTION:
*       causes fieldOutput to continue the output string
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID cmdLuaGaltisWrapperFieldSetAppendMode(GT_VOID)
{
    if (!gL)
        return;
    lua_getglobal(gL, "print");
    lua_pushliteral(gL, "cmdLuaGaltisWrapperFieldSetAppendMode()");
    fieldOutputAppendMode = GT_TRUE;
}

static int cmdLuaValueToGT32(lua_State *L, int i, GT_UINTPTR *ret)
{
    switch (lua_type(L, i))
    {
        case LUA_TNIL:
            *ret = 0;
            return 0;
        case LUA_TBOOLEAN:
            *ret = lua_toboolean(L, i) ? 1 : 0;
            return 0;
        case LUA_TNUMBER:
            *ret = (GT_32)lua_tointeger(L, i);
            return 0;
        case LUA_TSTRING:
            *ret = (GT_32)lua_tostring(L, i);
            return 0;
        case LUA_TUSERDATA:
            /* encode in hex and pass as string */
            /* not supported yet */
            /* return 0 */
            break;
        /* special case for GT_U64?? */
    }
    return 1;
}
/*******************************************************************************
* cmdLuaRunGaltisWrapper
*
* DESCRIPTION:
*       Wrapper to execute existing Galtis Wrappers
*
* INPUTS:
*       L       - Lua state
*         1st   - Galtis wrapper name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       wrapper output
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaRunGaltisWrapper(lua_State *L)
{
    CMD_COMMAND     *command;
    GT_STATUS       status;
    int             numArgs;
    int             n, i;
    int             err;

    n = lua_gettop(L);
    if (n < 1 || !lua_isstring(L, 1))
    {
        lua_pushnumber(L, GT_BAD_PARAM);
        return 1;
    }

    status = cmdGetCommand(lua_tostring(L, 1), &command);
    if (status != GT_OK)
    {
        lua_pushnumber(L, GT_NOT_SUPPORTED);
        return 1;
    }

    /* wrapper exists */
    cmdOsSigSemWait(gRunMtx, CPSS_OS_SEM_WAIT_FOREVER_CNS);

    numArgs = 0;
    numFields = 0;
    for (err = 0,i = 2; i <= n; i++)
    {
        if (lua_type(L,i) != LUA_TTABLE)
        {
            if (numArgs < CMD_MAX_ARGS)
            {
                err = cmdLuaValueToGT32(L, i, inputBuffer+numArgs);
                if (!err)
                    numArgs++;
            } else {
                err = 1;
            }
        }
        else
        {
            /* LUA_TTABLE */
            int k;
            for (k = 1; ; k++)
            {
                lua_rawgeti(L, i, k); /* get value */
                if (lua_isnil(L, -1))
                {
                    lua_pop(L, 1);
                    break;
                }
                if (numFields < CMD_MAX_FIELDS)
                {
                    err = cmdLuaValueToGT32(L, -1, fieldBuffer+numFields);
                    if (!err)
                        numFields++;
                } else {
                    err = 1;
                }
                lua_pop(L, 1);
            }
        }
        if (err)
            break;
    }

    if (err || numArgs < command->funcArgs || numFields < command->funcFields)
    {
        cmdOsSigSemSignal(gRunMtx);
        lua_pushnumber(L, GT_BAD_PARAM);
        return 1;
    }

    nresults = 0;
    nfields = 0;

    lua_newtable(L);
    fieldsIndex = lua_gettop(L);

    /* Enable galtisOutput hook */
    gL = L;
    cmdLuaGaltisWrapper = 1;

    isShellCmd = GT_FALSE;
    wrapperRef = command->funcReference;

    /* Resete time out*/
    timeOutExpired = 0;

    /* Signal proc taske that a new command is ready */
    status = cmdOsSigSemSignal(gSemProc);
    if (status != GT_OK)
    {
        cmdLuaGaltisWrapper = 0;
        cmdOsSigSemSignal(gRunMtx);
        lua_pushnumber(L, CMD_AGENT_ERROR);
        return 1;
    }

    while (timeOutExpired < cmdSysTimeout)
    {
        status = cmdOsSigSemWait(gSemCmd, CMD_PROC_TIMOUT);
        /* Time out expired */
        if (status == GT_TIMEOUT)
        {
            timeOutExpired += CMD_PROC_TIMOUT;
        }
        else
        {
            /* Reset timeout */
            timeOutExpired = 0;
            break;
        }
    }
    /* disable galtisOutput hook */
    cmdLuaGaltisWrapper = 0;
    gL = NULL;


    if (timeOutExpired >= cmdSysTimeout)
    {
        /* Time out expired */
        lua_pushnumber(L, GT_TIMEOUT);
        n = 1;
    } else {
        n = nresults;
    }

    cmdOsSigSemSignal(gRunMtx);

    return nresults;
}

#endif /* defined(CMD_LUA_CLI) && defined(CMD_LUA_GALTIS) */

#if defined(CMD_LUA_CLI) && !defined(CMD_LUA_GALTIS)
    /* stubs to make cmdWrapUtils happy */
    int cmdLuaGaltisWrapper = 0;
    GT_VOID cmdLuaGaltisWrapperOutput(GT_STATUS status, const GT_CHAR *format, va_list ap) {}
    GT_VOID cmdLuaGaltisWrapperFieldOutput(const GT_CHAR *format, va_list ap) {}
    GT_VOID cmdLuaGaltisWrapperFieldSetAppendMode(GT_VOID) {}
#endif /* defined(CMD_LUA_CLI) && !defined(CMD_LUA_GALTIS) */
