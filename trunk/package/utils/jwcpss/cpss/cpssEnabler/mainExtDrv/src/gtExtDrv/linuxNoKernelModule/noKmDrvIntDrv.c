/*******************************************************************************
*                Copyright 2015, MARVELL SEMICONDUCTOR, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL ISRAEL LTD. (MSIL).                                          *
********************************************************************************
* noKmDrvIntDrv.c
*
* DESCRIPTION:
*       Interrupt driver for linux based boards.
*       TODO Implement using polling
*
* DEPENDENCIES:
*       -   procfs, /dev/mem
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#define _BSD_SOURCE
#include <gtExtDrv/drivers/gtPciDrv.h>
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsMem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

unsigned long long extDrvInterruptCount = 0;

#ifdef NOKM_DRV_EMULATE_INTERRUPTS
GT_EXT_DRV_PCI_MAP_STC* prvExtDrvNokmGetMappingByIntNum(GT_U32 intNum);
struct intTaskParams_STC {
    GT_U32              intNum;
    GT_BOOL             enabled;
    GT_VOIDINTFUNCPTR   routine;
    GT_U32              param;
    GT_UINTPTR          regsBase;
};
#define MAX_INTERRUPT 8
static struct intTaskParams_STC intArr[MAX_INTERRUPT];
static GT_U32 intArrSize = 0;


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
    GT_U32 i = (GT_U32)((GT_UINTPTR)paramsPtr);
    GT_UINTPTR          regsBase;

    regsBase = intArr[i].regsBase;

    while (1)
    {
        GT_U32 intSumValue;
        GT_VOIDINTFUNCPTR   routine;
        GT_U32              param;

        routine = intArr[i].routine;
        param = intArr[i].param;
        if (intArr[i].enabled == GT_TRUE)
        {
            /* check interrupt summary register */
            intSumValue = *((volatile GT_U32 *)(regsBase + 0x30));
            if (intSumValue != 0)
            {
                osTaskLock();
                routine(param);
                osTaskUnLock();
            }
            usleep(20000); /* 20ms */
        } else {
            /* intArr[i].enabled != GT_TRUE */
            usleep(50000); /* 500ms */
        }
        /*TODO intLOCK/intUNLOCK */
    }
    return 0;
}
#endif /* NOKM_DRV_EMULATE_INTERRUPTS */

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
#ifdef NOKM_DRV_EMULATE_INTERRUPTS
    GT_U32 i;
    for (i = 0; i < intArrSize; i++)
    {
        if (intArr[i].intNum == intVecNum)
        {
            intArr[i].enabled = GT_TRUE;
            return GT_OK;
        }
    }
    return GT_FAIL;
#else
    return GT_OK;
    return GT_NOT_IMPLEMENTED;
#endif
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
#ifdef NOKM_DRV_EMULATE_INTERRUPTS
    GT_U32 i;
    for (i = 0; i < intArrSize; i++)
    {
        if (intArr[i].intNum == intVecNum)
        {
            intArr[i].enabled = GT_FALSE;
            return GT_OK;
        }
    }
    return GT_FAIL;
#else
    return GT_OK;
    return GT_NOT_IMPLEMENTED;
#endif
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
GT_STATUS extDrvIntConnect
(
    IN  GT_U32           intVec,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
)
{
#ifdef NOKM_DRV_EMULATE_INTERRUPTS
    GT_EXT_DRV_PCI_MAP_STC *map;
    GT_CHAR taskName[40];
    GT_TASK tid;
    GT_U32  i;

    if (intVec == 0)
    {
        fprintf(stderr, "extDrvIntConnect(): intVec==0 => BAD_PARAM\n");
        return GT_BAD_PARAM;
    }
    /* search if already bound */
    for (i = 0; i < intArrSize; i++)
    {
        if (intArr[i].intNum == intVec)
        {
            GT_BOOL eSaved = intArr[i].enabled;
            intArr[i].enabled = GT_FALSE;
            intArr[i].param = parameter;
            intArr[i].routine = (GT_VOIDINTFUNCPTR)routine;
            intArr[i].enabled = eSaved;
            return GT_OK;
        }
    }

    if (i == MAX_INTERRUPT)
    {
        fprintf(stderr, "extDrvIntConnect: too many interrupts already connected (%d)\n", intArrSize);
        return GT_FAIL;
    }

    map = prvExtDrvNokmGetMappingByIntNum(intVec);
    if (map == NULL)
        return GT_FAIL;

    intArr[intArrSize].intNum = intVec;
    intArr[intArrSize].enabled = GT_TRUE;
    intArr[intArrSize].routine = (GT_VOIDINTFUNCPTR)routine;
    intArr[intArrSize].param = parameter;
    intArr[intArrSize].regsBase = map->regs.base;
    intArrSize++;

    sprintf(taskName,"intTask%02x:%02x:%d",
            (intVec>>8) & 0xff, (intVec>>3) & 0x1f, intVec & 0x7);

    osTaskCreate(taskName, 0/*prio*/, 0x2000/*stk size*/,
            intTask, (GT_VOID_PTR)((GT_UINTPTR)(intArrSize-1)), &tid);

    if (0 == tid)
    {
        fprintf(stderr, "extDrvIntConnect: osTaskCreate(%s): %s\n", taskName, strerror(errno));
        return GT_FAIL;
    }

    return  GT_OK;
#else
    return GT_OK;
    return  GT_NOT_IMPLEMENTED;
#endif
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


