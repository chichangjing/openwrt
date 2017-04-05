/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssOsTask.h
*
* DESCRIPTION:
*       Operating System wrapper. Task facility.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#ifndef __cpssOsTaskh
#define __cpssOsTaskh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/************ Defines  ********************************************************/
#define __TASKCONV

/************* Typedefs *******************************************************/

typedef GT_U32  CPSS_TASK;

/*******************************************************************************
* CPSS_OS_TASK_CREATE_FUNC
*
* DESCRIPTION:
*       Create OS Task/Thread and start it.
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
typedef GT_STATUS (*CPSS_OS_TASK_CREATE_FUNC)
(
    IN  const GT_CHAR *name,
    IN  GT_U32     prio,
    IN  GT_U32     stack,
    IN  unsigned   (__TASKCONV *start_addr)(GT_VOID*),
    IN  GT_VOID    *arglist,
    OUT CPSS_TASK  *tid
);

/*******************************************************************************
* CPSS_OS_TASK_DELETE_FUNC
*
* DESCRIPTION:
*       Deletes existing task/thread.
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
typedef GT_STATUS (*CPSS_OS_TASK_DELETE_FUNC)
(
    IN CPSS_TASK tid
);

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
typedef GT_STATUS (*CPSS_OS_TASK_GET_SELF_FUNC)
(
    OUT GT_U32 *tid
);

/*******************************************************************************
* CPSS_OS_TASK_LOCK_FUNC
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
typedef GT_STATUS (*CPSS_OS_TASK_LOCK_FUNC) (GT_VOID);

/*******************************************************************************
* CPSS_OS_TASK_UNLOCK_FUNC
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
typedef GT_STATUS (*CPSS_OS_TASK_UNLOCK_FUNC) (GT_VOID);

/* CPSS_OS_TASK_BIND_STC -
    structure that hold the "os Task" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_OS_TASK_CREATE_FUNC               osTaskCreateFunc;
    CPSS_OS_TASK_DELETE_FUNC               osTaskDeleteFunc;
    CPSS_OS_TASK_GET_SELF_FUNC             osTaskGetSelfFunc;
    CPSS_OS_TASK_LOCK_FUNC                 osTaskLockFunc;
    CPSS_OS_TASK_UNLOCK_FUNC               osTaskUnLockFunc;
}CPSS_OS_TASK_BIND_STC;

#ifdef __cplusplus
}
#endif

#endif  /* __cpssOsTaskh */
/* Do Not Add Anything Below This Line */

