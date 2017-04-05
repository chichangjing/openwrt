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
* genDrvDsrIntDrv.c
*
* DESCRIPTION:
*       Includes interrupt managment routines.
*
* DEPENDENCIES:
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/

#include <gtExtDrv/drivers/gtIntDrv.h>
#include <gtExtDrv/drivers/pssBspApis.h>
#include <prestera/common/gtPresteraDefs.h>


typedef GT_VOID (*USER_DSR_PTR)(IN GT_U8 intVecMapIdx);

typedef struct
{
    GT_U32          intVecNum;
    GT_U8           intVecMapIdx;
    USER_DSR_PTR    routine;
} ISR_DSR_PARAMS; 

typedef struct intVec
{
    ISR_DSR_PARAMS  *isrDsrPrms;
    struct intVec   *nextPtr;
} INT_VEC;


static GT_TASK          dsrTid = 0;
static ISR_DSR_PARAMS   *dsrPrmsPtr[MAX_PP_DEVICES];
static GT_SEM           intLockSem = NULL;
static GT_SEM           dsrSem = NULL;

#define FIFO_SIZE       ((MAX_PP_DEVICES * 2) + 1)
static INT_VEC          intVecFifo[FIFO_SIZE];
static INT_VEC          *fifoHeadPtr = NULL;
static INT_VEC          *fifoTailPtr = NULL;


/*******************************************************************************
* extDrvIntIsr
*
* DESCRIPTION:
*       This is the main interrupt service routine bound to the interrupt vector.
*
* INPUTS:
*       parameter - the ISR/DSR parameter structure pointer including the
*                   interrupt vector number and the DSR signalling semaphore.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Invoked in ISR context !!
*
*******************************************************************************/
static GT_VOID extDrvIntIsr
(
    IN  GT_U32      parameter
)
{
    ISR_DSR_PARAMS  *isrParamsPtr = (ISR_DSR_PARAMS*)parameter;
    
    /* disable the interrupt vector */
    osIntDisable(isrParamsPtr->intVecNum);
    
    fifoTailPtr->isrDsrPrms = isrParamsPtr;
    fifoTailPtr = fifoTailPtr->nextPtr;
    
    /* signal the DSR */
    osSemSignal(dsrSem);
}
    

/*******************************************************************************
* extDrvDsr
*
* DESCRIPTION:
*       This is the main Deferred Service Routine, from which the other
*       interrupt SRs are called according to the devices types.
*
* INPUTS:
*       param   - the ISR/DSR parameter structure pointer including the
*                 interrupt vector number and the DSR signalling semaphore.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Assumption: The bounded routine in the intConnect uses 
*                   osSetIntLockUnlock for preemption protection between DSR 
*                   and other tasks.
*
*******************************************************************************/
static unsigned __TASKCONV extDrvDsr
(
    IN GT_VOID * notUsed
)
{
    ISR_DSR_PARAMS  *dsrParamsPtr;
    
    while (1)
    {
        /* wait for the Isr signal */
        osSemWait(dsrSem, OS_WAIT_FOREVER);

        while (fifoHeadPtr != fifoTailPtr)
        {
            dsrParamsPtr = fifoHeadPtr->isrDsrPrms;
            fifoHeadPtr = fifoHeadPtr->nextPtr;

            /* invoke the bounded routine */
            (*(dsrParamsPtr->routine))(dsrParamsPtr->intVecMapIdx);
            
            /* enable the interrupt vector */
            osIntEnable(dsrParamsPtr->intVecNum);
        }
    }
}


/*******************************************************************************
* extDrvIntConnect
*
* DESCRIPTION:
*       Connect a specified C routine to a specified interrupt vector.
*
* INPUTS:
*       intVecNum - interrupt vector number to attach to
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
    IN  GT_U32           intVecNum,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
)
{
    ISR_DSR_PARAMS  *isrDsrParamsPtr;
    GT_U32          i;
    static GT_BOOL  firstTime = GT_TRUE;
    
    if (GT_TRUE == firstTime)
    {
        /* create semaphore for interrupt lock unlock routines */
        if(osSemBinCreate("intLock", OS_SEMB_FULL, &intLockSem) != GT_OK)
        {
            return GT_FAIL;
        }
        
        /* clean the dsr task id table */
        osMemSet(dsrPrmsPtr, 0, sizeof(dsrPrmsPtr));
        
        /* create the interrupt vector FIFO linked chain */
        for (i = 0; i < (FIFO_SIZE-1); i++)
        {
            intVecFifo[i].isrDsrPrms  = NULL;
            intVecFifo[i].nextPtr     = &intVecFifo[i+1];
        }
        intVecFifo[i].nextPtr =  &intVecFifo[0];
        
        /* set the head and tail pointers of the FIFO */
        fifoHeadPtr = &intVecFifo[0];
        fifoTailPtr = &intVecFifo[0];
            
        /* create semaphore for signalling done from Isr to Dsr */
        if(osSemBinCreate("PssDsr", OS_SEMB_EMPTY, &dsrSem) != GT_OK)
        {
            return GT_FAIL;
        }
        
        /* create the Dsr task */
        if (osTaskCreate("PssDsr", DSR_PRIO_CNS, DSR_STACK_SIZE_CNS, extDrvDsr, 
                         (GT_VOID*)NULL, &dsrTid) != GT_OK)
        {
            return GT_FAIL;
        }
        firstTime = GT_FALSE;
    }
    
    if (dsrPrmsPtr[intVecNum] != NULL)
    {
        /* vector allready initialized */
        return GT_FAIL;
    }
    
    /* allocate memory for Isr and Dsr */
    isrDsrParamsPtr = osStaticMalloc(sizeof(ISR_DSR_PARAMS));
    if (NULL == isrDsrParamsPtr)
    {
        return GT_FAIL;
    }
    /* set all Isr/Dsr data parameters */
    isrDsrParamsPtr->intVecNum      = intVecNum;
    isrDsrParamsPtr->routine        = (USER_DSR_PTR)routine;
    isrDsrParamsPtr->intVecMapIdx   = (GT_U8)parameter;
    
    dsrPrmsPtr[intVecNum] = isrDsrParamsPtr;
    
    return osInterruptConnect(intVecNum, extDrvIntIsr, (GT_U32)isrDsrParamsPtr);
}


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
    if (osIntEnable(intVecNum) != GT_OK)
    {
        return GT_FAIL;
    }
    
    return GT_OK;
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
    if (osIntDisable(intVecNum) != GT_OK)
    {
        return GT_FAIL;
    }
    
    return GT_OK;
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
    if (NULL == key)
        return 1;

    switch (mode)
    {
        case INTR_MODE_UNLOCK:
            if (intLockSem)
            {
                osSemSignal(intLockSem);
            }
            break;

        case INTR_MODE_LOCK:
            if (intLockSem)
            {
                osSemWait(intLockSem, OS_WAIT_FOREVER);
            }
            break;

        default:
            return 1;
            break;
    }

    return 0;
}

