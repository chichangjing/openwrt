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
* osVxTask.c
*
* DESCRIPTION:
*       VxWorks Operating System wrapper. Task facility implementation.
*
* DEPENDENCIES:
*       VxWorks, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <vxWorks.h>
#include <taskLib.h>
#include <string.h>

#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsGen.h>


/************ Defines  ********************************************************/

#define VXW_DEF_STACK_SIZE  0x2000

#ifdef RTOS_ON_SIM

#include <asicSimulation/SCIB/scib.h>

#define PROTECT_TASK_DOING_READ_WRITE_START_MAC         SCIB_SEM_TAKE
#define PROTECT_TASK_DOING_READ_WRITE_END_MAC           SCIB_SEM_SIGNAL

GT_STATUS osTaskManagerRegister(
    IN GT_TASK      tid,
    IN const GT_CHAR  *namePtr
);
GT_STATUS osTaskManagerUnRegister(
    IN GT_TASK      tid
);
GT_STATUS osTaskManagerSuspendAllExcludeVip(
    GT_VOID
);
GT_STATUS osTaskManagerResumeAllExcludeVip(
    GT_VOID
);
GT_STATUS osTaskManagerSuspend(
    IN GT_TASK      tid
);
GT_STATUS osTaskManagerResume(
    IN GT_TASK      tid
);
#endif /*RTOS_ON_SIM*/


/************ Public Functions ************************************************/

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
    IN  GT_VOID  *arglist,
    OUT GT_TASK  *tid
)
{
    GT_32 new_size;
    GT_CHAR name_copy[65];

    IS_WRAPPER_OPEN_STATUS;
    if (stack == 0)
      stack = VXW_DEF_STACK_SIZE;

    new_size = stack & ~0x01UL;

    strncpy(name_copy, name, 64);
    name_copy[64] = 0;

    *tid = taskSpawn (name_copy,
                      prio, 0,
                      new_size,
                      (FUNCPTR) start_addr,
                      (int) arglist, 0, 0, 0, 0, 0, 0, 0, 0, 0);

#ifdef RTOS_ON_SIM
    if ( (osTaskManagerRegister(*tid,name)) != GT_OK )
       return GT_FAIL;
#endif /*RTOS_ON_SIM*/

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
    STATUS rc;

    IS_WRAPPER_OPEN_STATUS;
    if (tid == 0)
        tid = taskIdSelf();

    rc = taskDelete(tid);

    if (rc != OK)
        return GT_FAIL;

#ifdef RTOS_ON_SIM
    if ( (osTaskManagerUnRegister(tid)) != GT_OK )
       return GT_FAIL;
#endif /*RTOS_ON_SIM*/

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
    STATUS rc;

    IS_WRAPPER_OPEN_STATUS;
    if (tid == 0)
      tid = taskIdSelf();

    rc = taskSuspend(tid);

    if (rc != OK)
      return GT_FAIL;

#ifdef RTOS_ON_SIM
    if ((osTaskManagerSuspend(tid)) != GT_OK)
      return GT_FAIL;
#endif /*RTOS_ON_SIM*/

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
    STATUS rc;

    IS_WRAPPER_OPEN_STATUS;
    rc = taskResume(tid);
    if (rc != OK)
      return GT_FAIL;

#ifdef RTOS_ON_SIM
    if ((osTaskManagerResume(tid)) != GT_OK)
      return GT_FAIL;
#endif /*RTOS_ON_SIM*/

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
    IS_WRAPPER_OPEN_STATUS;
    /* check validity of function arguments */
    if (tid == NULL)
        return GT_FAIL;
    *tid = taskIdSelf();
    return GT_OK;
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
    STATUS status = OK;

    IS_WRAPPER_OPEN_STATUS;
    if (oldprio != NULL)
    {
        status = taskPriorityGet((int)tid,(int*)oldprio);
    }

    status = taskPrioritySet((int)tid, (int)newprio);

    if (status != OK)
        return GT_FAIL;

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
    STATUS status;

    IS_WRAPPER_OPEN_STATUS;
    if (prio == NULL)
        return GT_FAIL;

    status = taskPriorityGet((int)tid,(int*)prio);

    if (status != OK)
        return GT_FAIL;

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
    STATUS rc;

    IS_WRAPPER_OPEN_STATUS;

#ifndef RTOS_ON_SIM
    rc = taskLock();
    if ( rc != OK )
       return GT_FAIL;
#else /*RTOS_ON_SIM*/
    PROTECT_TASK_DOING_READ_WRITE_START_MAC;
    rc = osTaskManagerSuspendAllExcludeVip();
    /* the unlock of the 'protection' can be done only during 'tasks unlock'
      see osTaskUnLock */
    /*PROTECT_TASK_DOING_READ_WRITE_END_MAC;*/
    if ( rc != GT_OK )
       return GT_FAIL;
#endif /*RTOS_ON_SIM*/

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
    STATUS rc;

    IS_WRAPPER_OPEN_STATUS;
#ifndef RTOS_ON_SIM
    rc = taskUnlock();
    if ( rc != OK )
       return GT_FAIL;
#else /*RTOS_ON_SIM*/
    rc = osTaskManagerResumeAllExcludeVip();
    PROTECT_TASK_DOING_READ_WRITE_END_MAC;
    if ( rc != GT_OK )
       return GT_FAIL;
#endif /*RTOS_ON_SIM*/

    return GT_OK;
}


