/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osWin32Task.c
*
* DESCRIPTION:
*       Win32 Operating System Simulation. Task facility implementation.
*
* DEPENDENCIES:
*       Win32, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/

#include <windows.h>
#include <process.h>
#include <time.h>
#include <winbase.h>
#include <assert.h>

#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsTimer.h>

#include <asicSimulation/SCIB/scib.h>

#define PROTECT_TASK_DOING_READ_WRITE_START_MAC         SCIB_SEM_TAKE

#define PROTECT_TASK_DOING_READ_WRITE_END_MAC       SCIB_SEM_SIGNAL

/* SHOST functions that manage registered tasks */
extern GT_VOID t_reg_task_table(DWORD win_tid, HANDLE hnd, const GT_CHAR *task_name);
extern GT_VOID t_unreg_task_table(DWORD win_tid);

/************ Defines  ********************************************************/

static GT_SEM OS_TaskSemId;
static GT_STATUS osTaskInit (GT_VOID);

#define OS_MAX_TASK_NAME_LENGTH 20

#define OS_MAX_TASKS    24

typedef struct
{
    GT_BOOL     valid;
    GT_CHAR     threadName[OS_MAX_TASK_NAME_LENGTH];
    HANDLE      threadHandle;
    DWORD       threadId;
    unsigned (__TASKCONV *start_addr)(GT_VOID*);
    GT_VOID*    arglist;
}OS_TASK_T;

static OS_TASK_T     OS_TaskArray[OS_MAX_TASKS];
#define OS_TaskArrayClearEntry(i) \
    OS_TaskArray[i].valid = GT_FALSE; \
    OS_TaskArray[i].threadId = 0; \
    OS_TaskArray[i].threadName[0] = 0; \
    OS_TaskArray[i].threadHandle = 0; \
    OS_TaskArray[i].start_addr = NULL; \
    OS_TaskArray[i].arglist = NULL;

/************ Public Functions ************************************************/

/*******************************************************************************
* osTaskWrapper
*
* DESCRIPTION:
*       task wrapper
*
* INPUTS:
*       param   - the index in OS_TaskArray
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
static DWORD WINAPI osTaskWrapper(LPVOID param)
{
    if ((DWORD)param >= OS_MAX_TASKS)
    {
        /* wrong parameter */
        return 0;
    }
    if (OS_TaskArray[(DWORD)param].start_addr == NULL)
    {
        /* ??? already destroyed task */
        return 0;
    }
    OS_TaskArray[(DWORD)param].start_addr(OS_TaskArray[(DWORD)param].arglist);

    /* free OS_TaskArray entry */
    osSemWait(OS_TaskSemId, OS_WAIT_FOREVER);
    if ( OS_TaskArray[(DWORD)param].valid == GT_TRUE &&
            OS_TaskArray[(DWORD)param].threadId == GetCurrentThreadId() )
    {
        OS_TaskArrayClearEntry((DWORD)param);
    }
    osSemSignal(OS_TaskSemId);
    t_unreg_task_table(GetCurrentThreadId());

    return 0;
}

/*******************************************************************************
* osTaskCreate
*
* DESCRIPTION:
*       Create OS Task and start it.
*
* INPUTS:
*       name    - task name, string no longer then OS_MAX_TASK_NAME_LENGTH
*       prio    - task priority 1 - 64 => HIGH
*       stack   - task Stack Size
*       start_addr - task Function to execute
*       arglist    - pointer to list of parameters for task function
*
* OUTPUTS:
*       tid   - Task ID
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskCreate
(
    IN  const GT_CHAR *name,
    IN  GT_U32  prio,
    IN  GT_U32  stack,
    IN  unsigned (__TASKCONV *start_addr)(GT_VOID*),
    IN  GT_VOID *arglist,
    OUT GT_TASK *tid
)
{
    DWORD                       i, threadID;
    HANDLE                      hThread;
    static GT_BOOL facInit  =   GT_FALSE;

    if (facInit == GT_FALSE)
    {
        osTaskInit();
        facInit = GT_TRUE;
    }

    if(strlen(name)>OS_MAX_TASK_NAME_LENGTH)
        return(GT_FAIL);

    osSemWait(OS_TaskSemId, OS_WAIT_FOREVER);

    /* Look for free place in OS_TaskArray */
    for(i = 1; i < OS_MAX_TASKS; i++)
    {
        if( OS_TaskArray[i].valid == GT_FALSE)
            break;
    }
    if(i == OS_MAX_TASKS)
    {
        /* No more place in the OS_TaskArray */
        osSemSignal(OS_TaskSemId);
        return GT_FAIL;
    }
    /* Save parameters in the taskArray */
    OS_TaskArray[i].threadHandle = 0;
    OS_TaskArray[i].threadId = 0;
    strcpy(OS_TaskArray[i].threadName, name);
    OS_TaskArray[i].start_addr = start_addr;
    OS_TaskArray[i].arglist = arglist;

    /* Create thread and save Handle */
    hThread = CreateThread(NULL, stack, osTaskWrapper, (LPVOID)i, CREATE_SUSPENDED,&threadID);

    /* A handle to the new thread indicates success. NULL indicates failure. */
    if(hThread == NULL)
    {
        /* Can't create Thread */
        osSemSignal(OS_TaskSemId);
        return GT_FAIL;
    }

    /* register the task with the SHOST */
    /* we need to register the task , because the SHOST in the context of
       'interrupts' will do 'tasks lock' but not via the APIs of mainOs but only
       on the tasks that are registered internally with the SHOST
       meaning we must register out tasks there */
    t_reg_task_table( threadID ,hThread, name );


    /* Save parameters in the taskArray */
    OS_TaskArray[i].threadHandle = hThread;
    OS_TaskArray[i].threadId = threadID;
    /* state valid only after saving hThread,threadID other wise osTaskUnLock()
    may return fail during initialization of the system */
    OS_TaskArray[i].valid = GT_TRUE;

    if(tid != NULL)
    { /* task ID is a thread ID */
        *tid = i;
    }

    osSemSignal(OS_TaskSemId);

    osTaskResume(i);

    osSetTaskPrior(i, prio, NULL);

    return GT_OK;
}


/*******************************************************************************
* osTaskDelete
*
* DESCRIPTION:
*       Deletes existing task.
*
* INPUTS:
*       tid   - Task ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, delete calling task (itself)
*
*******************************************************************************/
GT_STATUS osTaskDelete
(
    IN GT_TASK tid
)
{
    BOOL        ret;
    DWORD       threadId;
    int         i;
    GT_TASK     it=0;

    osSemWait(OS_TaskSemId, OS_WAIT_FOREVER);

    /* check validity of task number */
    if(tid != 0 && OS_TaskArray[tid].valid == GT_FALSE)
    {
        osSemSignal(OS_TaskSemId);
        return GT_FALSE;
    }

    /* get current thread Id */
    threadId = GetCurrentThreadId();
    /* If terminating this thread */
    if (tid == 0 || OS_TaskArray[tid].threadId == threadId)
    {/* Terminate current thread with code 0 */
        /* Look for task ID in OS_TaskArray */
        for(i = 1, ret = FALSE; i < OS_MAX_TASKS; i++)
        {
            if( OS_TaskArray[i].valid == GT_TRUE &&
                OS_TaskArray[i].threadId == threadId)
            {
                it = i; /* current task Id */
                ret = TRUE; /* tid is found */
                break;
            }
        }

        if(ret == TRUE)
        {/* update entry */
            OS_TaskArrayClearEntry(it);  /* invalidate entry */
        }

        osSemSignal(OS_TaskSemId);
        t_unreg_task_table(threadId);
        ExitThread(0);
    }


    /* terminate not current thread */
    t_unreg_task_table(OS_TaskArray[tid].threadId);
    ret = TerminateThread(OS_TaskArray[tid].threadHandle, 0);
    if(ret)
    { /* thread was successfully terminated */
        OS_TaskArrayClearEntry(tid);  /* invalidate entry */
    }

    osSemSignal(OS_TaskSemId);

    if(!ret)
    {
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* osTaskSuspend
*
* DESCRIPTION:
*       Suspends existing task/thread.
*
* INPUTS:
*       tid   - Task ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, suspend calling task (itself)
*
*******************************************************************************/
GT_STATUS osTaskSuspend
(
    IN GT_TASK tid
)
{
    DWORD        retVal;

    if(tid == 0)
    {/* suspend current thread */
        retVal = SuspendThread(GetCurrentThread());
    }
    else
    {
        /* check validity of task number */
        if(OS_TaskArray[tid].valid == GT_FALSE)
            return GT_FALSE;
        /* suspend thread */
        retVal = SuspendThread(OS_TaskArray[tid].threadHandle);
    }

    if(retVal == -1)
        return GT_FAIL;

    return GT_OK;
}


/*******************************************************************************
* osTaskResume
*
* DESCRIPTION:
*       Resumes existing task/thread.
*
* INPUTS:
*       tid   - Task ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osTaskResume
(
    IN GT_TASK tid
)
{
    DWORD        retVal;

    if(tid == 0)
    {/* resume current thread */
        retVal = ResumeThread(GetCurrentThread());
    }
    else
    {
        /* check validity of task number */
        if(OS_TaskArray[tid].valid == GT_FALSE)
            return GT_FALSE;
        /* suspend thread */
        retVal = ResumeThread(OS_TaskArray[tid].threadHandle);
    }

    if(retVal == -1)
        return GT_FAIL;

    return GT_OK;
}


/*******************************************************************************
* osTaskGetSelf
*
* DESCRIPTION:
*       returns the current task (thread) id
*
* INPUTS:
*       none
*
* OUTPUTS:
*       tid -  the current task (thread) id
*
* RETURNS:
*       GT_OK - on success.
*       GT_FAIL - if parameter is invalid
*       GT_NOT_FOUND - current task (thread) id not found in OS task array
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskGetSelf
(
    OUT GT_U32 *tid
)
{
    DWORD        threadId;
    int                i;

        /* check validity of function arguments */
    if (tid == NULL)
        return GT_FAIL;

    threadId = GetCurrentThreadId();
    /* look for thread in table */
    for(i = 1; i < OS_MAX_TASKS; i++)
    {
        if(threadId == OS_TaskArray[i].threadId)
        {/* if thread ID found */
            *tid = i;
            return GT_OK;
        }
    }

    *tid = threadId;/* return the 'WIN32' handle */

    return GT_NOT_FOUND;
}


/*******************************************************************************
* osSetTaskPrior
*
* DESCRIPTION:
*       Changes priority of task/thread.
*
* INPUTS:
*       tid     - Task ID
*       newprio - new priority of task
*
* OUTPUTS:
*       oldprio - old priority of task
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, change priotity of calling task (itself)
*
*******************************************************************************/
GT_STATUS osSetTaskPrior
(
    IN  GT_TASK tid,
    IN  GT_U32  newprio,
    OUT GT_U32  *oldprio
)
{
    HANDLE        hThread;
    int           win32Pri;

    if(tid == 0)
    {/* get handle to the current thread */
        hThread = GetCurrentThread();
    }
    else
    {
        /* check validity of task number */
        if(OS_TaskArray[tid].valid == GT_FALSE)
            return GT_FAIL;

        /* get handle to the task */
        hThread = OS_TaskArray[tid].threadHandle;
    }

    /* get old and set new priority */
    if(oldprio != NULL)
        *oldprio = (unsigned long)GetThreadPriority(hThread);

    switch (newprio)
    {
        case 10: win32Pri = THREAD_PRIORITY_ABOVE_NORMAL; break;
        default: win32Pri = THREAD_PRIORITY_NORMAL; break;
    }

    if( SetThreadPriority(hThread, win32Pri) == FALSE)
        return  GT_FAIL;

    return GT_OK;
}

/*******************************************************************************
* osGetTaskPrior
*
* DESCRIPTION:
*       Gets priority of task/thread.
*
* INPUTS:
*       tid     - Task ID
*
* OUTPUTS:
*       prio - priority of task
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, gets priotity of calling task (itself)
*
*******************************************************************************/
GT_STATUS osGetTaskPrior
(
    IN  GT_TASK tid,
    OUT GT_U32  *prio
)
{
    HANDLE        hThread;

    if(prio == NULL)
        return GT_FAIL;


    if(tid == 0)
    {/* get handle to the current thread */
        hThread = GetCurrentThread();
    }
    else
    {
        /* check validity of task number */
        if(OS_TaskArray[tid].valid == GT_FALSE)
            return GT_FAIL;

        /* get handle to the task ID*/
        hThread = OS_TaskArray[tid].threadHandle;
    }

    *prio = (unsigned long)GetThreadPriority(hThread);

    return GT_OK;
}

/*******************************************************************************
* osTaskLock
*
* DESCRIPTION:
*       Disable task rescheduling of current task.
*
* INPUTS:
*       None
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
GT_STATUS osTaskLock(GT_VOID)
{
    GT_U8 tdLoop;
    GT_U32 tid;

    tid = GetCurrentThreadId();

    PROTECT_TASK_DOING_READ_WRITE_START_MAC;

    for (tdLoop = 0 ; tdLoop < OS_MAX_TASKS ; tdLoop++)
    {
        if ( (OS_TaskArray[tdLoop].valid) &&
             (OS_TaskArray[tdLoop].threadId != tid) )
        {
            if (osTaskSuspend (tdLoop) != GT_OK)
            {
                PROTECT_TASK_DOING_READ_WRITE_END_MAC;
                return GT_FAIL;
            }
        }
    }

    PROTECT_TASK_DOING_READ_WRITE_END_MAC;

    return GT_OK;
}


/*******************************************************************************
* osTaskUnLock
*
* DESCRIPTION:
*       Enable task rescheduling.
*
* INPUTS:
*       None
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
GT_STATUS osTaskUnLock (GT_VOID)
{
    GT_U8 tdLoop;
    GT_U32 tid;

    tid = GetCurrentThreadId();

    for (tdLoop = 0 ; tdLoop < OS_MAX_TASKS ; tdLoop++)
    {
        if ( (OS_TaskArray[tdLoop].valid) &&
             (OS_TaskArray[tdLoop].threadId != tid) )
        {
          if (osTaskResume (tdLoop) != GT_OK)
            return GT_FAIL;
        }
    }

    return GT_OK;
}


static GT_STATUS osTaskInit (GT_VOID)
{
    long    i, rc;

    for (i=0; i<OS_MAX_TASKS; i++)
    {
        OS_TaskArrayClearEntry(i);
    }
    rc = osSemBinCreate(NULL, 1, &OS_TaskSemId);

    return rc;
}
