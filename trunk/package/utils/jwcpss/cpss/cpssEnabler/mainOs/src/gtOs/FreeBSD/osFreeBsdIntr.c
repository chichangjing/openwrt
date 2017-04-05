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
* osFreeBsdIntr.c
*
* DESCRIPTION:
*       FreeBsd User Mode Operating System wrapper. Interrupt facility.
*
* DEPENDENCIES:
*       FreeBsd, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
* COMMENTS: 
*
*******************************************************************************/


#include <gtOs/gtOsIntr.h>
#include <gtOs/gtOsSem.h>

#include <asicSimulation/SCIB/scib.h>
#include <common/SHOST/GEN/INTR/EXP/INTR.H>

/***** Public Functions ************************************************/

/******Local Variables ************************/
static GT_U32 osInterGo = 0;

GT_MUTEX IntLockmutex;

/*******************************************************************************
* osIntEnable
*
* DESCRIPTION:
*       Enable corresponding interrupt bits
*
* INPUTS:
*       intVecNum - new interrupt bits
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
    IN GT_U32   intVecNum
)
{
  SHOSTG_interrupt_enable_one(intVecNum);

  return GT_OK ;
}

/*******************************************************************************
* osIntDisable
*
* DESCRIPTION:
*       Disable corresponding interrupt bits.
*
* INPUTS:
*       intVecNum - new interrupt bits
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
    IN GT_U32   intVecNum
)
{
  SHOSTG_interrupt_disable_one(intVecNum);

  return GT_OK ;
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
extern void SHOSTG_interrupt_do_enable(UINT_32 new_val);
GT_32 osSetIntLockUnlock
(
    IN      INTERRUPT_MODE mode,
    INOUT   GT_32          *key
)
{
    static INTERRUPT_MODE currMode = INTR_MODE_UNLOCK;
    static int intLockCnt = 0;
    INTERRUPT_MODE temp;
  
    /* lock locking counter */
    osMutexLock(IntLockmutex, 0);

    if (mode == INTR_MODE_LOCK)
        intLockCnt++;
    else if ((mode == INTR_MODE_UNLOCK) && (intLockCnt > 0))
        intLockCnt--;

    temp = currMode;
    currMode = mode;

    if ((mode == INTR_MODE_UNLOCK) && (intLockCnt == 0))
    {
        osMutexUnlock(IntLockmutex);
        if (osInterGo == 0)
        {
            SHOSTG_go_intr_go ();
            osInterGo = 1;
        }

        SHOSTG_interrupt_do_enable(*key);

    }
    else
    {

        osMutexUnlock(IntLockmutex);
    }

    return temp;
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
    GT_U32  shostIntVector = vector + 1;
    /* call SHOST to bind ISR with simulated interrupts
       engine */
    SHOSTG_bind_interrupt_with_param ( 15, shostIntVector,
                                       routine, NULL, parameter);

    return GT_OK;
}



