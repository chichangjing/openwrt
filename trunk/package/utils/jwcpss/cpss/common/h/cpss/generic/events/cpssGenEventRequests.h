/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenEventRequests.h
*
* DESCRIPTION:
*       Includes unified event routine. The routines allow a user application
*       to bind, select and receive events from PP, XBAR and FA devices.
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#ifndef __cpssGenEventRequestsh
#define __cpssGenEventRequestsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>

/*
 * Typedef: enum CPSS_EVENT_MASK_SET_ENT
 *
 * Description: Defines the different interrupt node states, that an interrupt
 *              node may have.
 *
 * Fields:
 *      CPSS_EVENT_MASK_E   - mask unified event , so application will not get
 *                            an event
 *      CPSS_EVENT_UNMASK_E - UNmask unified event , so application can receive
 *                            an event
 *
 */
typedef enum
{
    CPSS_EVENT_MASK_E          = 0,
    CPSS_EVENT_UNMASK_E        = 1
}CPSS_EVENT_MASK_SET_ENT;

/*******************************************************************************
* cpssEventBind
*
* DESCRIPTION:
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to wait for the event
*       (cpssEventSelect), receive the event(cpssEventRecv) or transmit a packet
*       using the Network Interface.
*
*       NOTE : the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet or
*              cpssEventDeviceMaskWithEvExtDataSet
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*
* OUTPUTS:
*       hndlPtr     - (pointer to) The user handle for the bounded events.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_FULL - when trying to set the "tx buffer queue unify event"
*                 (CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same
*                 handler
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventBind
(
    IN  CPSS_UNI_EV_CAUSE_ENT uniEventArr[],
    IN  GT_U32               arrLength,
    OUT GT_UINTPTR           *hndlPtr
);

/*******************************************************************************
* cpssEventDestroy
*
* DESCRIPTION:
*       This routine destroy the handle , and this handle is not valid any more.
*       The handle could have been create either by cpssEventIsrBind or cpssEventBind
*       API implementation sequence:
*       1. Signal any locked semaphores (so application's task can continue)
*       2. Release dynamic allocations relate to this event.
*
*       NOTE : the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet or
*              cpssEventDeviceMaskWithEvExtDataSet
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl                - The user handle for the bounded events.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_BAD_PARAM - bad hndl parameter , the hndl parameter is not legal
*                     (was not returned by cpssEventBind(...)/cpssEventIsrBind(...))
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventDestroy
(
    IN  GT_UINTPTR          hndl
);

/*******************************************************************************
* cpssEventSelect
*
* DESCRIPTION:
*       This function waiting for one of the events ,relate to the handler,
*       to happen , and gets a list of events (in array of bitmaps format) that
*       occurred .
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl                - The user handle for the bounded events.
*       timeoutPtr          - (pointer to) Wait timeout in milliseconds
*                             NULL pointer means wait forever.
*       evBitmapArrLength   - The bitmap array length (in words).
*
* OUTPUTS:
*       evBitmapArr         - The bitmap array of the received events.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_TIMEOUT - when the "time out" requested by the caller expired and no
*                    event occurred during this period
*       GT_BAD_PARAM - bad hndl parameter , the hndl parameter is not legal
*                     (was not returned by cpssEventBind(...))
*       GT_BAD_PTR  - evBitmapArr parameter is NULL pointer
*                     (and evBitmapArrLength != 0)
*       GT_EMPTY - there are no events to retrieve (relevant when timeoutPtr != NULL).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventSelect
(
    IN  GT_UINTPTR          hndl,
    IN  GT_U32              *timeoutPtr,
    OUT GT_U32              evBitmapArr[],
    IN  GT_U32              evBitmapArrLength
);

/*******************************************************************************
* cpssEventRecv
*
* DESCRIPTION:
*       This function gets general information about the selected unified event.
*       The function retrieve information about the device number that relate to
*       the event , and extra info about port number / priority queue that
*       relate to the event.
*       The function retrieve the info about the first occurrence of this event
*       in the queue.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl        - The user handle for the bounded events.
*       evCause     - The specified unify event that info about it's first
*                     waiting occurrence required.
* OUTPUTS:
*       evExtDataPtr- (pointer to)The additional data (port num / priority
*                     queue number) the event was received upon.
*       evDevPtr    - (pointer to)The device the event was received upon
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_NO_MORE   - There is no more info to retrieve about the specified
*                      event .
*       GT_BAD_PARAM - bad hndl parameter ,
*                      or hndl bound to CPSS_PP_TX_BUFFER_QUEUE_E --> not allowed
*                      use dedicated "get tx ended info" function instead
*       GT_BAD_PTR  - one of the parameters is NULL pointer
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventRecv
(
    IN  GT_UINTPTR          hndl,
    IN  CPSS_UNI_EV_CAUSE_ENT evCause,
    OUT GT_U32              *evExtDataPtr,
    OUT GT_U8               *evDevPtr
);


/*******************************************************************************
* cpssEventDeviceMaskSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
GT_STATUS cpssEventDeviceMaskSet
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN CPSS_EVENT_MASK_SET_ENT  operation
);

/*******************************************************************************
* cpssEventDeviceGenerate
*
* DESCRIPTION:
*       This debug routine configures device to generate unified event for
*       specific element in the event associated with extra data.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3.
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                   queue number / other ) the event was received upon.
*                   Use CPSS_PARAM_NOT_USED_CNS to generate events for all supported evExtData.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device Or the 'extra data' has value that not relate
*                      to the uniEvent
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*       GT_NOT_SUPPORTED - on not supported unify event
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssEventDeviceGenerate
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData
);

/*******************************************************************************
* cpssEventDeviceMaskWithEvExtDataSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device , for
*       specific element in the event associated with extra data.
*       Since Each unified event may be associated with multiple HW interrupts,
*       each HW interrupt has different 'extra data' in the context of the
*       relevant unified event
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                     queue number / other ) the event was received upon.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device Or the 'extra data' has value that not relate
*                      to the uniEvent
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
GT_STATUS cpssEventDeviceMaskWithEvExtDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData,
    IN CPSS_EVENT_MASK_SET_ENT  operation
);

/*******************************************************************************
* CPSS_EVENT_ISR_CB_FUNC
*
* DESCRIPTION:
*       This is a prototype of the functions that need to be bound to handler
*       that aggregate one or more events . this CB was bound by calling
*       function cpssEventIsrBind(...)
*
*       This application's function would be called at Interrupt context !!!
*
*       This allowing the application to use any synchronism mechanism which
*       suits its needs.
*
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl        - The user handle for the bounded events.
*                     as returned as output by function cpssEventIsrBind(...)
*       cookie      - The user's cookie(application's data) for the bounded events.
*                     as given as input to function cpssEventIsrBind(...)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_VOID (*CPSS_EVENT_ISR_CB_FUNC)
(
    IN GT_UINTPTR   hndl,
    IN GT_VOID*     cookie
);


/*******************************************************************************
* cpssEventIsrBind
*
* DESCRIPTION:
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to check if there are
*       waiting events (cpssEventWaitingEventsGet), clear the already served
*       events (cpssEventTreatedEventsClear) , receive the event(cpssEventRecv)
*
*       This is alternative way of treating events as opposed of the other
*       Select mechanism used with function cpssEventBind(...) (where
*       Application had to wait on binary semaphore, which was released by the
*       relevant interrupt).
*
*       In the new scheme, the application provides an ISR function which would
*       be called whenever the relevant interrupt(s) have been occurred.
*       Thus, allowing the application to use any synchronism mechanism which
*       suits its needs.
*       The application ISR function would be called at Interrupt context !!!
*
*       NOTE : 1. the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet or
*              cpssEventDeviceMaskWithEvExtDataSet
*              2. for this handler the function cpssEventSelect(..) is not
*                 applicable.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*       isrCbPtr  - (pointer for) application ISR function to be called when
*                     events are received
*       cookie  - storing application data (cookie)
*
* OUTPUTS:
*       hndlPtr     - (pointer to) The user handle for the bounded events.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_FULL - when trying to set the "tx buffer queue unify event"
*                 (CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same
*                 handler
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventIsrBind
(
    IN  CPSS_UNI_EV_CAUSE_ENT   uniEventArr[],
    IN  GT_U32                  arrLength,
    IN  CPSS_EVENT_ISR_CB_FUNC  isrCbPtr,
    IN  GT_VOID*                cookie,
    OUT GT_UINTPTR              *hndlPtr
);

/*******************************************************************************
* cpssEventWaitingEventsGet
*
* DESCRIPTION:
*       This function returns a bitmap reflecting the unified events waiting
*       in queue for the user application.
*       the function get the events from the waiting queue.
*       This function should be called after a call to clear already served
*       events (cpssEventTreatedEventsClear).
*       The application receives a bitmap of events that are waiting.
*
* INPUTS:
*       hndl            - The user handle.
*       evBitmapLength  - The size of bitmap array in words.
*
* OUTPUTS:
*       evBitmapArr     - The updated bitmap array.(can be NULL if
*                         evBitmapLength ==0 )
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_BAD_PARAM  - the hndl parameter is not legal (was not returned by
*                       cpssEventIsrBind(...))
*       GT_NO_MORE -  no events are waiting
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventWaitingEventsGet
(
    IN GT_UINTPTR       hndl,
    OUT GT_U32          evBitmapArr[],
    IN GT_U32           evBitmapLength
);


/*******************************************************************************
* cpssEventTreatedEventsClear
*
* DESCRIPTION:
*       remove the events from the waiting queue , that already served
*       (Received) and has no new waiting for All Handler event.
*       The events are unified under a Handler received from the CPSS.
*
*       NOTE : the function protect itself by internal "interrupts lock" in
*              the start of function , and "interrupts unlock" in the end of
*              function.
*
* INPUTS:
*       hndl    - The user handle.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PARAM  - the hndl parameter is not legal
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEventTreatedEventsClear
(
    IN GT_UINTPTR       hndl
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenEventRequestsh */


