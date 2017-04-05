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
* gt88245IntDrv.c
*
* DESCRIPTION:
*       Interrupt driver for linux uClinux based boards.
*
* DEPENDENCIES:
*       -   System controller.
*       -   LSP.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsIntr.h>
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsMem.h>


#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <string.h>

#include "kerneldrv/include/prestera_glob.h"


#define IN
#define OUT

/******************************************************************************
* struct intTaskParams_STC
* 
* DESCRIPTION:
*       This structure is used to pass parameters to intTask()
*******************************************************************************/
struct intTaskParams_STC {
    GT_U32 devNum;
    GT_U32 cookie;
};

static GT_U32 tid;
static GT_VOIDINTFUNCPTR _userRoutine;

/*************** Globals ******************************************************/

/* file descriptor returned by openning the PP *nix device driver */
extern GT_32 gtPpFd;


/*******************************************************************************
* extDrvIntEnable
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
GT_STATUS extDrvIntEnable
(
    IN GT_U32   intVecNum
)
{
    if (0 == tid)
    {
        fprintf(stderr, "intEnable - tid(0): %s\n", strerror(errno));
        return GT_FAIL;
    }

    if (osTaskResume(tid))
    {
        fprintf(stderr, "osTaskResume: %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Enable the IRQ  */
    /*if (ioctl (gtPpFd, PRESTERA_IOC_INTENABLE, intVecNum))
    {
        fprintf(stderr, "Interrupt enabled failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }*/

    return GT_OK ;
}


/*******************************************************************************
* extDrvIntDisable
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
GT_STATUS extDrvIntDisable
(
    IN GT_U32   intVecNum
)
{
    /* Disable the irq */
    /*if (ioctl (gtPpFd, PRESTERA_IOC_INTDISABLE, intVecNum))
    {
        fprintf(stderr, "Interrupt disabled failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }*/

    if (0 == tid)
    {
        fprintf(stderr, "intDisable - tid(0): %s\n", strerror(errno));
        return GT_FAIL;
    }

    if (osTaskSuspend(tid))
    {
        fprintf(stderr, "osTaskSuspend: %s\n", strerror(errno));
        return GT_FAIL;
    }

    return GT_OK ;
}


/*******************************************************************************
* intTask
*
* DESCRIPTION:
*       Interrupt handler task.
*
* INPUTS:
*       param1  - device number
*       param2  - ISR cookie
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
unsigned __TASKCONV intTask(GT_VOID *paramsPtr)
{
    GT_U32 devNum;
    GT_U32 cookie;
    
    cookie = ((struct intTaskParams_STC*)paramsPtr)->cookie;
     
    devNum = ((struct intTaskParams_STC*)paramsPtr)->devNum;

    osFree(paramsPtr);
    
    /* No need to wait because not interrupts in the driver until intEnable */
    while (1)
    {
        /* Wait for interrupt */
        if (ioctl (gtPpFd, PRESTERA_IOC_WAIT, cookie))
        {
            fprintf(stderr, "Interrupt wait failed: errno(%s)\n",
                            strerror(errno));
            return (int)GT_FAIL;
        }

        osTaskLock();

        _userRoutine(devNum);

        osTaskUnLock();
    }
}


/*******************************************************************************
* extDrvIntConnect
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
GT_U32 extDrvIntConnect
(
    IN  GT_U32           intVec,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
)
{
    static GT_U32 intialized[2] = { 0, 0 };
    GT_U8 taskName[32];
    GT_VecotrCookie_STC vector_cookie;
    struct intTaskParams_STC* paramsPtr;
        
    if (intVec > 63)
    {
        fprintf(stderr, "unknown interrupt vector: %ul\n", (int)intVec);
        return GT_FAIL;
    }
    if (intialized[intVec >> 5] & (1 << (intVec & 0x1F)))
    {
        fprintf(stderr, "intialized: %s\n", strerror(errno));
        return GT_FAIL;
    }
    intialized[intVec >> 5] |= (1 << (intVec & 0x1F));

    vector_cookie.vector = intVec;

    if (ioctl (gtPpFd, PRESTERA_IOC_INTCONNECT, &vector_cookie))
    {
        fprintf(stderr, "Interrupt connect failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    _userRoutine = (GT_VOIDINTFUNCPTR)routine;

    sprintf(taskName, "intTask%d", intVec);

    paramsPtr = (struct intTaskParams_STC*)osMalloc(sizeof(*paramsPtr));
    paramsPtr->cookie = vector_cookie.cookie;
    paramsPtr->devNum = parameter;
    
    osTaskCreate(taskName, 0/*prio*/, 0x2000/*stk size*/,
            intTask, paramsPtr, &tid);

    if (0 == tid)
    {
        osFree(paramsPtr);
        fprintf(stderr, "extDrvIntConnect: osTaskCreate(%s): %s\n", taskName, strerror(errno));
        return GT_FAIL;
    }

    return  GT_OK;
}


/*******************************************************************************
* extDrvSetIntLockUnlock
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
GT_32 extDrvSetIntLockUnlock
(
    IN      INTERRUPT_MODE mode,
    INOUT   GT_32          *key
)
{
    return GT_OK;
}


