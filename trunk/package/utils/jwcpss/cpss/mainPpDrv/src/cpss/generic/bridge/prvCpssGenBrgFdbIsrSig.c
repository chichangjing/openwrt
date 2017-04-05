/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenBrgFdbIsrSig.c
*
* DESCRIPTION:
*       Includes ISR and FDB signalling mechanism create/signal/wait/delete API
*
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#include <cpss/generic/bridge/private/prvCpssGenBrgFdbIsrSig.h>
#include <cpss/extServices/os/gtOs/cpssOsSem.h>
#include <cpss/generic/events/cpssGenEventRequests.h>

static CPSS_SYS_HA_MODE_ENT fdbHaMode = CPSS_SYS_HA_MODE_ACTIVE_E;

/* semaphore for access to mac address update registers */
static GT_UINTPTR   queryHndl = 0;

/* semaphore for trigger relaited mac table actions  */
static GT_UINTPTR   trigHndl = 0;

/* semaphore tokens */
static GT_U8    querySemToken = 0;
static GT_U8    trigSemToken  = 0;

/* did we already created the ISR for FBD query signaling */
GT_BOOL fdbIsrSigQueryCreateCalled = GT_FALSE;

/* did we already created the ISR for FBD "trig action done" signaling */
GT_BOOL fdbIsrSigTrigActionDoneCreateCalled = GT_FALSE;

/*******************************************************************************
* prvCpssFdbIsrSigCreate
*
* DESCRIPTION:
*       This routine creates a signalling mechanism for FDB ISR.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       sigType  - The event signal type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigCreate
(
    IN CPSS_FDB_ISR_SIG_ENT       sigType
)
{
    CPSS_UNI_EV_CAUSE_ENT   event;
    GT_UINTPTR              *hndlPtr;

    switch (sigType)
    {
        case CPSS_QUERY_E:
            event = CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E;
            querySemToken = 1;
            hndlPtr = &queryHndl;
            break;
        case CPSS_MAC_TRIG_ACTION_E:
            event = CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E;
            trigSemToken = 0;
            hndlPtr = &trigHndl;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return cpssEventBind(&event, 1, hndlPtr);
}

/*******************************************************************************
* prvCpssFdbIsrSigDelete
*
* DESCRIPTION:
*       This routine deletes a signalling mechanism for FDB ISR.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       sigType  - The event signal type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigDelete
(
    IN CPSS_FDB_ISR_SIG_ENT       sigType
)
{
    /* the function should unbind event when "unbind" event feature
     will be added*/
    (void)sigType;
    return GT_OK;
}

/*******************************************************************************
* prvCpssFdbIsrSigSignal
*
* DESCRIPTION:
*       This routine signals a waiting task.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       sigType  - The event signal type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigSignal
(
    IN CPSS_FDB_ISR_SIG_ENT       sigType
)
{
    switch (sigType)
    {
        case CPSS_QUERY_E:
            querySemToken = 1;
            break;
        case CPSS_MAC_TRIG_ACTION_E:
            trigSemToken = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssFdbIsrSigWait
*
* DESCRIPTION:
*       This routine waits for a signal for the event to occur.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       sigType  - The event signal type.
*       timeout  - Time-out in milliseconds or 0 to wait forever.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigWait
(
    IN CPSS_FDB_ISR_SIG_ENT sigType,
    IN GT_U32               timeout
)
{
    GT_UINTPTR              hndl;
    GT_U32                  evBitmapArr[CPSS_UNI_EV_BITMAP_SIZE_CNS];
    GT_STATUS               rc;
    GT_U32                  *timeoutPtr = NULL;
    GT_U32                  intExtData; /* Device number */
    GT_U8                   intDev;     /* Dummy */
    CPSS_UNI_EV_CAUSE_ENT   event;

    if(fdbHaMode == CPSS_SYS_HA_MODE_STANDBY_E)
    {
        /* in standby there are no interrupts from HW so we can't wait
           for one ... */
        return GT_OK;
    }

    switch (sigType)
    {
        case CPSS_QUERY_E:
            if (1 == querySemToken)
            {
                querySemToken = 0;
                return GT_OK;
            }
            event = CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E;
            hndl = queryHndl;
            break;
        case CPSS_MAC_TRIG_ACTION_E:
            if (1 == trigSemToken)
            {
                trigSemToken = 0;
                return GT_OK;
            }
            event = CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E;
            hndl = trigHndl;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (timeout != CPSS_OS_SEM_WAIT_FOREVER_CNS)
        timeoutPtr = &timeout;

    rc = cpssEventSelect(hndl, timeoutPtr, evBitmapArr, CPSS_UNI_EV_BITMAP_SIZE_CNS);
    if (GT_OK != rc)
        return rc;

    return cpssEventRecv(hndl, event, &intExtData , &intDev);

}

/*******************************************************************************
* prvCpssFdbIsrSigHaModeSet
*
* DESCRIPTION:
*       function to set CPU High Availability mode of operation.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - the device number.
*       mode - active or standby
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on failure.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigHaModeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_SYS_HA_MODE_ENT    mode
)
{
    devNum = devNum;/* not needed parameter in current implementation ... */

    fdbHaMode = mode;

    return GT_OK;
}

/*******************************************************************************
* prvCpssFdbIsrSigUnmaskDevice
*
* DESCRIPTION:
*       This routine unmasks the unified events of "mac update done" /
*       "mac trigger action done" on specific device.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sigType  - The event signal type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFdbIsrSigUnmaskDevice
(
    IN GT_U8                      devNum,
    IN CPSS_FDB_ISR_SIG_ENT       sigType
)
{
    CPSS_UNI_EV_CAUSE_ENT   event;

    switch (sigType)
    {
        case CPSS_QUERY_E:
            event = CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E;
            break;
        case CPSS_MAC_TRIG_ACTION_E:
            event = CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return cpssEventDeviceMaskSet(devNum,event,CPSS_EVENT_UNMASK_E);
}

