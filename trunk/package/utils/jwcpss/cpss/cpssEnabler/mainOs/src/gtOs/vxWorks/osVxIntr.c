/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osVxIntr.c
*
* DESCRIPTION:
*       VxWorks Operating System wrapper. Interrupt facility.
*
* DEPENDENCIES:
*       VxWorks, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#include <intLib.h>
#include <logLib.h>
#include <taskLib.h>
#include <vxWorks.h>

#include <gtOs/gtOsIntr.h>
#include <gtOs/gtOsGen.h>

#ifdef RTOS_ON_SIM
    #include <asicSimulation/SCIB/scib.h>
    #include <common/SHOST/GEN/INTR/EXP/INTR.H>

    #define PROTECT_TASK_DOING_READ_WRITE_START_MAC         SCIB_SEM_TAKE

    #define PROTECT_TASK_DOING_READ_WRITE_END_MAC           SCIB_SEM_SIGNAL
#endif /*RTOS_ON_SIM*/
/************ Extern functions ************************************************/
extern GT_STATUS bspIntEnable(IN GT_U32 intMask);
extern GT_STATUS bspIntDisable(IN GT_U32 intMask);
extern GT_STATUS bspIntConnect(IN GT_U32 irq,IN GT_VOIDFUNCPTR rtn, IN GT_U32 param);
GT_VOID prvDebugCpuUtilizationFromInterrupt(GT_BOOL endOfInt);

static GT_VOIDFUNCPTR intRoutine = NULL;

/*******************************************************************************
* intRoutineWrapper
*
* DESCRIPTION:
*       ISR wrapper
*
* INPUTS:
*       parameter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID intRoutineWrapper
(
    IN GT_U32 parameter
)
{
    prvDebugCpuUtilizationFromInterrupt(GT_FALSE);
    (*intRoutine)(parameter);
    prvDebugCpuUtilizationFromInterrupt(GT_TRUE);
}

/************ Public Functions ************************************************/
/*******************************************************************************
* osIntEnable
*
* DESCRIPTION:
*       Enable corresponding interrupt bits
*
* INPUTS:
*       intMask - new interrupt bits
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osIntEnable
(
    IN GT_U32   intMask
)
{
    IS_WRAPPER_OPEN_STATUS;

#ifndef RTOS_ON_SIM
    return bspIntEnable(intMask);
#else /*RTOS_ON_SIM*/
  SHOSTG_interrupt_enable_one(intMask + 1);
  return GT_OK ;
#endif /*RTOS_ON_SIM*/
}

/*******************************************************************************
* osIntDisable
*
* DESCRIPTION:
*       Disable corresponding interrupt bits.
*
* INPUTS:
*       intMask - new interrupt bits
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osIntDisable
(
    IN GT_U32   intMask
)
{
    IS_WRAPPER_OPEN_STATUS;

#ifndef RTOS_ON_SIM
    return bspIntDisable(intMask);
#else /*RTOS_ON_SIM*/
    SHOSTG_interrupt_disable_one(intMask + 1);
    return GT_OK ;
#endif /*RTOS_ON_SIM*/
}

/*******************************************************************************
* osSetIntLockUnlock
*
* DESCRIPTION:
*       Lock/unlock interrupts
*
* INPUTS:
*       mode   - interrupt state lock/unlock
*       key    - if mode is INTR_MODE_UNLOCK, lock key returned by
*                preceding interrupt disable call
*
* OUTPUTS:
*       key    - if mode is INTR_MODE_LOCK lock key for the interrupt
*                level
*
* RETURNS:
*       Lock key for the interrupt level
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_32 osSetIntLockUnlock
(
    IN      INTERRUPT_MODE mode,
    INOUT   GT_32          *key
)
{
    static int current_mask;

    IS_WRAPPER_OPEN_PTR;

    if (key == NULL)
        return 0;

    switch (mode)
    {
        case INTR_MODE_UNLOCK :
#ifndef RTOS_ON_SIM
            intUnlock(*key);
#else /*RTOS_ON_SIM*/
            SHOSTG_interrupt_enable(*key);
        PROTECT_TASK_DOING_READ_WRITE_END_MAC;
#endif /*RTOS_ON_SIM*/

        break;

        case INTR_MODE_LOCK :
#ifndef RTOS_ON_SIM
            current_mask = intLock();
#else /*RTOS_ON_SIM*/
        PROTECT_TASK_DOING_READ_WRITE_START_MAC;
            current_mask = SHOSTG_interrupt_disable();
        /* the unlock of the 'protection' can be done only during 'tasks unlock'
          see INTR_MODE_UNLOCK */
        /*PROTECT_TASK_DOING_READ_WRITE_END_MAC;*/
#endif /*RTOS_ON_SIM*/

            *key = current_mask;

        break;

        default:
        break;
    }

    return (current_mask);
}

/*******************************************************************************
* osInterruptConnect
*
* DESCRIPTION:
*       Connect a specified C routine to a specified interrupt vector.
*
* INPUTS:
*       vector    - interrupt vector number to attach to
*       routine   - routine to be called
*       parameter - parameter to be passed to routine
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
GT_STATUS osInterruptConnect
(
    IN  GT_U32           vector,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
)
{
#ifndef RTOS_ON_SIM
    IS_WRAPPER_OPEN_STATUS;

    if (intRoutine == NULL)
    {
        intRoutine = routine;
        return bspIntConnect(vector, (GT_VOIDFUNCPTR)intRoutineWrapper, parameter);
    }
    else if(intRoutine == routine)
    {
        return bspIntConnect(vector, (GT_VOIDFUNCPTR)intRoutineWrapper, parameter);
    }
    else
    {
        return bspIntConnect(vector, routine, parameter);
    }

#else /*RTOS_ON_SIM*/
    GT_U32  shostIntVector = vector + 1;
    IS_WRAPPER_OPEN_STATUS;
    /* call SHOST to bind ISR with simulated interrupts
       engine */
    SHOSTG_bind_interrupt_with_param ( 15, shostIntVector,
                                       routine, NULL, parameter);

     return GT_OK;
#endif /*RTOS_ON_SIM*/
}


