/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osWin32Intr.c
*
* DESCRIPTION:
*       Win32 Operating System Simulation. Interrupt facility.
*
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/


#include <gtOs/gtOsIntr.h>
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsSem.h>

/* ROS definitions and SHOST API */
#include <os/simTypes.h>
#include <common/SHOST/HOST_D/EXP/HOST_D.H>

#include <asicSimulation/SInit/sinit.h>
#include <asicSimulation/SDistributed/sdistributed.h>

/* define macro that WA the numbering issue in  the SHOST */
#define WA_SHOST_NUMBERING(_vector) ((_vector) + 1)

#include <asicSimulation/SCIB/scib.h>

/*#define PROTECT_TASK_DOING_READ_WRITE_START_MAC         SCIB_SEM_TAKE done in the SHOST */

/*#define PROTECT_TASK_DOING_READ_WRITE_END_MAC           SCIB_SEM_SIGNAL  done in the SHOST*/

/****** Types ************************/
typedef struct {
    GT_VOIDFUNCPTR isrCb;
    GT_U32         param;
}OS_ISR_INFO_STC;

/******Local Variables ************************/
static GT_U32 osInterGo = 0;

static OS_ISR_INFO_STC    isrInfo[MAX_UINT_8];

/***** Public Functions ************************************************/
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
    SHOSTG_interrupt_enable_one(WA_SHOST_NUMBERING(intVecNum));

    return GT_OK;
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
    SHOSTG_interrupt_disable_one(WA_SHOST_NUMBERING(intVecNum));

    return GT_OK;
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
    static INTERRUPT_MODE currMode = INTR_MODE_UNLOCK;
    INTERRUPT_MODE temp;

    temp = currMode;
    currMode = mode;

    if (mode == INTR_MODE_UNLOCK)
    {
        if (osInterGo == 0)
        {
            SHOSTG_go_intr_go ();
            osInterGo = 1;
        }
        SHOSTG_interrupt_enable(*key);
    }
    else
    {
/*        PROTECT_TASK_DOING_READ_WRITE_START_MAC; done in the SHOST */

        temp = SHOSTG_interrupt_disable();

/*        PROTECT_TASK_DOING_READ_WRITE_END_MAC; done in the SHOST */

        *key = temp;
    }

    return temp;
}

/*******************************************************************************
* osIsr
*
* DESCRIPTION:
*       lock all other tasks when ISR is invoked
*
* INPUTS:
*       vector    - interrupt vector number to attach to
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID osIsr(GT_U32  vector)
{
   /* osTaskLock(); Done in SHOST */

    if(sasicgSimulationRole == SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_VIA_BROKER_E)
    {
        if(sdistAppViaBrokerInterruptMaskMode ==
           INTERRUPT_MODE_BROKER_AUTOMATICALLY_MASK_INTERRUPT_LINE)
        {
            /* the broker put itself into 'masked' mode --> so we need to release it */
            simDistributedInterruptUnmask(vector);/* note no need to use macro WA_SHOST_NUMBERING on this vector ! */

            /* we took the 'Easy way' , and set 'unmask' before reading the PP
               cause register .

               because if we do it after calling the isrCb , we will need much
               more complex mechanism , to avoid lose of interrupts !
            */
        }
    }

    ((GT_VOIDINTFUNCPTR)(isrInfo[vector].isrCb))(isrInfo[vector].param);

  /*  osTaskUnLock(); Done in SHOST */
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
    if (vector >= MAX_UINT_8)
    {
        return GT_FAIL;
    }

    /* update DB */
    isrInfo[vector].isrCb = routine ;
    isrInfo[vector].param = parameter;

    /* call SHOST to bind ISR with simulated interrupts engine */
    SHOSTG_bind_interrupt_with_param ( 15, WA_SHOST_NUMBERING(vector),
                                       osIsr, NULL, vector);
    return GT_OK;
}

