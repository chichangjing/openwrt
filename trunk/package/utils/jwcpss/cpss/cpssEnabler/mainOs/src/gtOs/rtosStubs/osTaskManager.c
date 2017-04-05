/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osTaskManager.c
*
* DESCRIPTION:
*       functions that implement the task manager for RTOS on simulation.
*       RTOS on simulation - is real time operating system that instead of
*           accessing the HW it access a remote simulation via sockets.
*
*       motivation:
*       when osTaskLock is called , it need to suspend the tasks that may call
*       cpss for read/write register.
*       but since there are still some tasks must keep on running at this stage
*       beside this task(current task) , like:
*           1. tNetTask - that manage TCP stack in vxWorks .
*                        --> this task will not be registered in the DB and therefore
*                           will not be locked/unlocked by the code.
*           next 3 tasks are simulation tasks (that are not CPSS/application tasks)
*           those tasks should get special treatment (VIP treatment...)
*           2. simTask0 - that manage the read reply - on sockets.
*           3. simTask1 - that manage the read reply - on sockets..
*           4. simTask2 - that simulate the interrupt context.
*
*       ASSUMPTION -- all tasks that may manipulate cpss will register the
*                     manager via osTaskCreate(...)
*
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*
*******************************************************************************/

#include <gtOs/gtGenTypes.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsTimer.h>
#include <gtOs/gtOsStr.h>

#include <asicSimulation/SCIB/scib.h>


#define TASK_MANAGER_DB_SIZE    50
#define OS_MAX_TASK_NAME_LENGTH 20

typedef struct{
    GT_U32          valid;
    GT_TASK         tid;
    GT_U32          vip;
    GT_U32          interrupt;
    GT_U32          suspended;
    GT_TASK         lockingTask;
    char            namePtr[OS_MAX_TASK_NAME_LENGTH+1];
}TASK_MANAGER_DB_STC;


static TASK_MANAGER_DB_STC taskManagerdb[TASK_MANAGER_DB_SIZE];

static GT_U32   suspendLevel = 0;
static GT_TASK  taskIdSuspending = 0;


static GT_U32   suspendLevelFromInterrupt = 0;
static GT_U32   currentTasksNum = 0;

#define PROTECT_TASK_DOING_READ_WRITE_START_MAC         SCIB_SEM_TAKE
#define PROTECT_TASK_DOING_READ_WRITE_END_MAC           SCIB_SEM_SIGNAL

#define LOCK_DB     PROTECT_TASK_DOING_READ_WRITE_START_MAC
#define UNLOCK_DB   PROTECT_TASK_DOING_READ_WRITE_END_MAC

/*******************************************************************************
* osTaskManagerInfoPtrGet
*
* DESCRIPTION:
*       get the task info in DB
*
* INPUTS:
*       tid    - Task ID
*
* OUTPUTS:
*       vipPtr  - is this task "VIP"
*       interruptPtr - is this task "interrupt"
*
* RETURNS:
*       pointer in DB about task -- or NULL if not found
*
* COMMENTS:
*      not locking DB to avoid dead lock
*
*******************************************************************************/
static TASK_MANAGER_DB_STC* osTaskManagerInfoPtrGet(
    IN GT_TASK      tid
)
{
    GT_U32  ii;

    for(ii = 0 ; ii < currentTasksNum ; ii++)
    {
        if(taskManagerdb[ii].valid && taskManagerdb[ii].tid == tid)
        {
            return &taskManagerdb[ii];
        }
    }
    return NULL;
}

/*******************************************************************************
* osTaskManagerFlagsSet
*
* DESCRIPTION:
*       set flags of the task
*
* INPUTS:
*       tid    - Task ID
*       vip  - this task is "VIP"
*       interrupt - this task is "interrupt"
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_FOUND - task not found
*       GT_NOT_INITIALIZED - DB not initialized
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskManagerFlagsSet(
    IN GT_TASK      tid,
    IN GT_U32       vip,
    IN GT_U32       interrupt
)
{
    TASK_MANAGER_DB_STC* dbPtr;

    LOCK_DB;

    dbPtr = osTaskManagerInfoPtrGet(tid);
    if(dbPtr == NULL)
    {
        UNLOCK_DB;
        return GT_NOT_FOUND;
    }

    dbPtr->vip = vip;
    dbPtr->interrupt = interrupt;

    UNLOCK_DB;
    return GT_OK;
}


/*******************************************************************************
* osTaskManagerRegister
*
* DESCRIPTION:
*       register a task into manager
*
* INPUTS:
*       tid    - Task ID
*       namePtr  - task name, string no longer then OS_MAX_TASK_NAME_LENGTH
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PTR - on null pointer
*       GT_BAD_PARAM - on name too long
*       GT_FULL - table full can't register new task--->
*                  so  need to set larger TASK_MANAGER_DB_SIZE
*       GT_ALREADY_EXIST - task already registered
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskManagerRegister(
    IN GT_TASK      tid,
    IN const char   *namePtr
)
{
    char simTasks[]="simTask";
    char serialTask[]="SERIAL";
    GT_U32  firstNonValid = 0xFFFFFFFF;
    GT_U32  ii;

    if(namePtr == NULL)
    {
        osPrintf("osTaskManagerRegister: null \n");
        return GT_BAD_PTR;
    }
    else if(osStrlen(namePtr) > OS_MAX_TASK_NAME_LENGTH)
    {
        osPrintf("osTaskManagerRegister: %s len > OS_MAX_TASK_NAME_LENGTH\n",namePtr);
        return GT_BAD_PARAM;
    }

    LOCK_DB;
    for(ii = 0 ; ii < TASK_MANAGER_DB_SIZE ; ii++)
    {
        if(taskManagerdb[ii].valid == 0)
        {
            if(firstNonValid == 0xFFFFFFFF)
            {
                firstNonValid = ii;
            }
        }
        else if(taskManagerdb[ii].tid == tid)
        {
            firstNonValid = ii;
            /* it seems that if a task is terminated (task exit the code) */
            /* the OS may give the task ID to a new task ... , so we
               need to update the name of the task */
            break;
        }
    }

    if(firstNonValid == 0xFFFFFFFF)
    {
        UNLOCK_DB;
        osPrintf("osTaskManagerRegister: DB full \n");
        return GT_FULL;
    }

    ii = firstNonValid;

    taskManagerdb[ii].valid = 1;
    taskManagerdb[ii].tid = tid;
    osStrCpy(taskManagerdb[ii].namePtr,namePtr);
    taskManagerdb[ii].vip = 0;
    taskManagerdb[ii].interrupt = 0;
    taskManagerdb[ii].suspended = 0;
    taskManagerdb[ii].lockingTask = 0;

    /* look for "VIP" tasks */
    if((0 == osMemCmp(simTasks,namePtr,osStrlen(simTasks))) ||
       (0 == osMemCmp(serialTask,namePtr,osStrlen(serialTask))))
    {
        taskManagerdb[ii].vip = 1;
    }

    if(ii >= currentTasksNum)
    {
        currentTasksNum = ii+1;
    }

    UNLOCK_DB;
    return GT_OK;
}

/*******************************************************************************
* osTaskManagerUnRegister
*
* DESCRIPTION:
*       un-register a task from manager
*
* INPUTS:
*       tid    - Task ID
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_FOUND - task id is not registered
*       GT_NOT_INITIALIZED - DB not initialized
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskManagerUnRegister(
    IN GT_TASK      tid
)
{
    TASK_MANAGER_DB_STC* dbPtr;


    LOCK_DB;

    dbPtr = osTaskManagerInfoPtrGet(tid);
    if(dbPtr == NULL)
    {
        UNLOCK_DB;
        return GT_NOT_FOUND;
    }

    dbPtr->valid = 0;

    UNLOCK_DB;

    return GT_OK;
}


/*******************************************************************************
* osTaskManagerSuspendAllExcludeVip
*
* DESCRIPTION:
*       suspend all registered tasks except for some "vip" tasks (and my own)
*
* INPUTS:
*
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_STATE - bad state due to trying to suspend task from deferent tid
*                      then original one.
*                      or task is trying to lock while interrupt locked all previously
*       GT_NOT_INITIALIZED - DB not initialized
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskManagerSuspendAllExcludeVip(
    void
)
{
    GT_U32  ii;
    GT_STATUS rc;
    GT_TASK tid;
    GT_U32  vip,interrupt;
    GT_U32  lockOnlySingleTask=0;
    GT_U32  errorLine=0;
    TASK_MANAGER_DB_STC* dbPtr;

    rc = osTaskGetSelf(&tid);
    if(rc != GT_OK)
    {
        return rc;
    }

    LOCK_DB;

    dbPtr = osTaskManagerInfoPtrGet(tid);
    if(dbPtr == NULL)
    {
        interrupt = 0;
        vip = 0;
    }
    else
    {
        vip = dbPtr->vip;
        interrupt = dbPtr->interrupt;
    }

    if(vip && (interrupt == 0))
    {
        /* the vip tasks (that are not interrupts) must not lock the other tasks
           because we assume that those vip , must always run regardless of
           other tasks.
        */
        rc = GT_BAD_STATE;
        errorLine = __LINE__;
        goto exit_cleanly_lbl;
    }

    if(interrupt == 0)
    {
        if(suspendLevel)
        {
            if(taskIdSuspending != tid)
            {
                /* this is not the task that locked everybody , how this happened ???? */
                rc = GT_BAD_STATE;
                errorLine = __LINE__;
                goto exit_cleanly_lbl;
            }

            suspendLevel++;

            rc = GT_OK;
            goto exit_cleanly_lbl;
        }
        else
        {
            /* this is the first level of locking from a task (not from "interrupt")*/

            /* check that we are not locked under "interrupt" */
            if(suspendLevelFromInterrupt)
            {
                rc = GT_BAD_STATE;
                errorLine = __LINE__;
                goto exit_cleanly_lbl;
            }
        }

        /* save the task Id that locked the other tasks */
        taskIdSuspending = tid;
    }
    else
    {
        if(suspendLevelFromInterrupt)
        {
            suspendLevelFromInterrupt++;

            rc = GT_OK;
            goto exit_cleanly_lbl;
        }

        if(suspendLevel)
        {
            /* at this stage , there was already a task that locked all other tasks,
               but now there is an "interrupt" that should also lock this locking task.
            */

            lockOnlySingleTask = 1;
        }

    }

    for(ii = 0 ; ii < currentTasksNum ; ii++)
    {
        if(taskManagerdb[ii].valid == 0)
        {
            /* skip non valid entries */
            continue;
        }
        else if(taskManagerdb[ii].vip == 1 || taskManagerdb[ii].interrupt == 1)
        {
            /* skip "VIP" entries , and interrupts */
            continue;
        }
        else if(taskManagerdb[ii].tid == tid)
        {
            /* skip own entry */
            continue;
        }
        else if(lockOnlySingleTask && taskManagerdb[ii].tid != taskIdSuspending)
        {
            /* this is under interrupt that should lock ONLY task that did
               previously "lock to other tasks" */
            continue;
        }

        /* suspend this task */
        rc += osTaskSuspend(taskManagerdb[ii].tid);

        taskManagerdb[ii].lockingTask = tid;
    }

    if(interrupt == 0)
    {
        suspendLevel = 1;
    }
    else
    {
        suspendLevelFromInterrupt = 1;
    }

exit_cleanly_lbl:

    if(rc != GT_OK)
    {
        TASK_MANAGER_DB_STC* dbPtr = osTaskManagerInfoPtrGet(tid);

        osPrintf("osTaskManagerResumeAllExcludeVip : RC error [%d] , caused by task [%s] [0x%8.8d] in line [%ld]\n",
                    rc,dbPtr ? dbPtr->namePtr : "not registered",
                    tid,errorLine);
    }

    UNLOCK_DB;
    return rc;
}


/*******************************************************************************
* osTaskManagerResumeAllExcludeVip
*
* DESCRIPTION:
*       Resume all tasks that were suspended by osTaskManagerSuspendAllExcludeVip
*
* INPUTS:
*
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_STATE - bad state due to :
*                      1. trying to suspend task from deferent tid then original one.
*                      2. no suspend was done previously
*       GT_NOT_INITIALIZED - DB not initialized
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskManagerResumeAllExcludeVip(
    void
)
{
    GT_U32  ii;
    GT_STATUS rc;
    GT_TASK tid;
    GT_U32  interrupt;
    GT_U32  unlockOnlySingleTask=0;
    GT_U32  errorLine=0;
    TASK_MANAGER_DB_STC* dbPtr;

    rc = osTaskGetSelf(&tid);
    if(rc != GT_OK)
    {
        return rc;
    }

    LOCK_DB;

    dbPtr = osTaskManagerInfoPtrGet(tid);
    if(dbPtr == NULL)
    {
        interrupt = 0;
    }
    else
    {
        interrupt = dbPtr->interrupt;
    }

    if(interrupt == 0)
    {
        if(suspendLevel == 0)
        {
            /* not allow to resume , the suspend level is still exist */
            rc = GT_BAD_STATE;
            errorLine = __LINE__;
            goto exit_cleanly_lbl;
        }

        if(taskIdSuspending != tid)
        {
            /* this is not the task that locked everybody , how this happened ???? */
            rc = GT_BAD_STATE;
            errorLine = __LINE__;
            goto exit_cleanly_lbl;
        }

        suspendLevel--;

        if(suspendLevel)
        {
            /* not allow to resume , the suspend level is still exist */
            rc = GT_OK;
            goto exit_cleanly_lbl;
        }
    }
    else
    {
        if(suspendLevelFromInterrupt == 0)
        {
            /* no suspend was done */
            rc = GT_BAD_STATE;
            errorLine = __LINE__;
            goto exit_cleanly_lbl;
        }

        suspendLevelFromInterrupt--;

        if(suspendLevelFromInterrupt)
        {
            /* not allow to resume , the suspend level is still exist */
            rc = GT_OK;
            goto exit_cleanly_lbl;
        }

        if(suspendLevel)
        {
            /* at this stage , there was already a task that locked all other tasks,
               but there was an "interrupt" that also locked this locking task.

               we need to unlock only this task.
            */

            unlockOnlySingleTask = 1;
        }
    }


    for(ii = 0 ; ii < currentTasksNum ; ii++)
    {
        if(taskManagerdb[ii].valid == 0)
        {
            /* skip non valid entries */
            continue;
        }
        else if(taskManagerdb[ii].lockingTask != tid)
        {
            /* skip tasks that not locked by me */
            continue;
        }

        /* resume this task */
        rc += osTaskResume(taskManagerdb[ii].tid);

        taskManagerdb[ii].lockingTask = 0;
    }

exit_cleanly_lbl:

    if(rc != GT_OK)
    {
        TASK_MANAGER_DB_STC* dbPtr = osTaskManagerInfoPtrGet(tid);

        osPrintf("osTaskManagerResumeAllExcludeVip : RC error [%d] , caused by task [%s] [0x%8.8d] in line [%ld]\n",
                    rc,dbPtr ? dbPtr->namePtr : "not registered",
                    tid,errorLine);
    }

    UNLOCK_DB;
    return rc;
}


/*******************************************************************************
* osTaskManagerSuspend
*
* DESCRIPTION:
*       Suspend task in DB .
*
* INPUTS:
*
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_NOT_FOUND - task not found
*       GT_NOT_INITIALIZED - DB not initialized
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskManagerSuspend(
    IN GT_TASK      tid
)
{
    TASK_MANAGER_DB_STC* dbPtr = osTaskManagerInfoPtrGet(tid);

    if(dbPtr == NULL)
    {
        return GT_NOT_FOUND;
    }

    /* not locking DB to avoid dead-lock , because the
        osTaskManagerSuspendAllExcludeVip(...) calling osTaskSuspend(...)
        that call osTaskManagerSuspend , and this is under DB lock */

    /* next checking is for debugging only , and should be removed ... */
    if((suspendLevelFromInterrupt && dbPtr->interrupt) || /* should not suspend interrupt while interrupt is working */
        (dbPtr->vip && dbPtr->interrupt == 0))/* should not suspend vip (that is not interrupt)*/
    {
        GT_TASK tmpTid;
        TASK_MANAGER_DB_STC* tmpDbPtr;
        osTaskGetSelf(&tmpTid);
        tmpDbPtr = osTaskManagerInfoPtrGet(tmpTid);

        while(1)
        {
            osPrintf(" osTaskManagerSuspend : [%s] locked by task [%s]\n",
                dbPtr->namePtr,
                tmpDbPtr ? tmpDbPtr->namePtr : "unregistered task");

            osTimerWkAfter(5000);
        }
    }

    dbPtr->suspended = 1;

    return GT_OK;
}

/*******************************************************************************
* osTaskManagerResume
*
* DESCRIPTION:
*       Resume task in DB .
*
* INPUTS:
*
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_NOT_FOUND - task not found
*       GT_NOT_INITIALIZED - DB not initialized
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskManagerResume(
    IN GT_TASK      tid
)
{
    TASK_MANAGER_DB_STC* dbPtr = osTaskManagerInfoPtrGet(tid);

    if(dbPtr == NULL)
    {
        return GT_NOT_FOUND;
    }

    /* not locking DB to avoid dead-lock , because the
        osTaskManagerSuspendAllExcludeVip(...) calling osTaskSuspend(...)
        that call osTaskManagerSuspend , and this is under DB lock */

    dbPtr->suspended = 0;

    return GT_OK;
}

/*******************************************************************************
* osTaskManagerNameGet
*
* DESCRIPTION:
*       get the name of a task (if registered in DB)
*
*
* INPUTS:
*
*
* OUTPUTS:
*
* RETURNS:
*       pointer to name of task
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U8   * osTaskManagerNameGet(
    IN GT_TASK  tid
)
{
    TASK_MANAGER_DB_STC* dbPtr = osTaskManagerInfoPtrGet(tid);
    static GT_U8    unknownName[]="unknown tid";

    if(dbPtr == NULL)
    {
        return unknownName;
    }

    return dbPtr->namePtr;
}


/*******************************************************************************
* osTaskManagerDump
*
* DESCRIPTION:
*
*
* INPUTS:
*
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_STATE - bad state due to :
*                      1. trying to suspend task from deferent tid then original one.
*                      2. no suspend was done previously
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskManagerDump(
    void
)
{
    TASK_MANAGER_DB_STC* dbPtr;
    GT_U32  ii;

    osPrintf("osTaskManagerDump- start \n");

    for(ii = 0 ; ii < currentTasksNum ; ii++)
    {
        if(taskManagerdb[ii].valid == 0)
        {
            /* skip non valid entries */
            continue;
        }

        osPrintf("task[%s] tid[0x%8.8x] %s %s %s \n",
                taskManagerdb[ii].namePtr,
                taskManagerdb[ii].tid,
                taskManagerdb[ii].vip ? "[VIP]" : "",
                taskManagerdb[ii].interrupt ? "[interrupt]" : "",
                taskManagerdb[ii].suspended ? "[Suspended]" : "");
    }

    osPrintf("suspendLevelFromInterrupt[%d],suspendLevel[%d]",
              suspendLevelFromInterrupt,suspendLevel);

    if(suspendLevel)
    {
        dbPtr = osTaskManagerInfoPtrGet(taskIdSuspending);
        osPrintf("suspending task is [%s] [%d]",dbPtr? dbPtr->namePtr : "unknown name",taskIdSuspending);
    }

    osPrintf("osTaskManagerDump- end \n");

    return GT_OK;
}
