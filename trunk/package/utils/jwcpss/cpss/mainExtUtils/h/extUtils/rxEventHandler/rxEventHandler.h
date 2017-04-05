/*******************************************************************************
*                Copyright 2014, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* rxEventHandler.h
*
* DESCRIPTION:
*       This module defines the API for CPSS_PP_RX_BUFFER_QUEUE*_E and
*       CPSS_PP_RX_ERR_QUEUE*_E event handling.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __rxEventHandler_h__
#define __rxEventHandler_h__

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>

#define RX_EV_HANDLER_DEVNUM_ALL 0xff
#define RX_EV_HANDLER_QUEUE_ALL  0xff

/*
 * typedef: enum RX_EV_HANDLER_TYPE_ENT
 *
 * Description:
 *      Event type
 *
 * Enumerations:
 *      RX_EV_HANDLER_TYPE_RX_E     - CPSS_PP_RX_BUFFER_QUEUE*_E events
 *      RX_EV_HANDLER_TYPE_RX_ERR_E - CPSS_PP_RX_ERR_QUEUE*_E events
 *      RX_EV_HANDLER_TYPE_ANY_E    - any type of events
 *
 */
typedef enum
{
    RX_EV_HANDLER_TYPE_RX_E,
    RX_EV_HANDLER_TYPE_RX_ERR_E,
    RX_EV_HANDLER_TYPE_ANY_E
} RX_EV_HANDLER_EV_TYPE_ENT;

/*******************************************************************************
* RX_EV_PACKET_RECEIVE_CB_FUN
*
* DESCRIPTION:
*       Function called to handle incoming Rx packet in the CPU
*
* APPLICABLE DEVICES: ALL Devices
*
* INPUTS:
*       cookie       - a cookie passed to rxEventHandlerAddCallback()
*       evType       - event type
*       devNum       - Device number.
*       queueIdx     - The queue from which this packet was received.
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (pointer to)information parameters of received packets
*
* RETURNS:
*       GT_OK - no error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*RX_EV_PACKET_RECEIVE_CB_FUN)
(
  IN  GT_UINTPTR                cookie,
  IN  RX_EV_HANDLER_EV_TYPE_ENT evType,
  IN  GT_U8                     devNum,
  IN  GT_U8                     queueIdx,
  IN  GT_U32                    numOfBuff,
  IN  GT_U8                     *packetBuffs[],
  IN  GT_U32                    buffLen[],
  IN  GT_VOID                   *rxParamsPtr
);

/*******************************************************************************
* rxEventHandlerLibInit
*
* DESCRIPTION:
*       Initialize library
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS rxEventHandlerLibInit(GT_VOID);

/*******************************************************************************
* rxEventHandlerInitHandler
*
* DESCRIPTION:
*       Initialize event handler for Rx/RxErr events
*       rxEventHandlerDoCallbacks() must be called from the same process
*       (another thread of this process allowed)
*
* INPUTS:
*       devNum  - the device number or RX_EV_HANDLER_DEVNUM_ALL
*       queue   - the rx queue number or RX_EV_HANDLER_QUEUE_ALL
*       type    - event type
*
* OUTPUTS:
*       evHandlePtr  - handle
*
* RETURNS:
*       GT_OK on success
*       GT_ALREADY_EXIST success, already initialized
*       GT_BAD_PARAM
*       GT_FAIL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS rxEventHandlerInitHandler(
  IN  GT_U8                     devNum,
  IN  GT_U8                     queue,
  IN  RX_EV_HANDLER_EV_TYPE_ENT evType,
  OUT GT_UINTPTR                *evHandlePtr
);

/*******************************************************************************
* rxEventHandlerAddCallback
*
* DESCRIPTION:
*       Add callback function for Rx events
*
* INPUTS:
*       devNum  - the device number or RX_EV_HANDLER_DEVNUM_ALL
*       queue   - the rx queue number or RX_EV_HANDLER_QUEUE_ALL
*       type    - event type
*       cbFunc  - callback function
*       cookie  - cookie value to be passed to callback function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS rxEventHandlerAddCallback(
  IN  GT_U8                         devNum,
  IN  GT_U8                         queue,
  IN  RX_EV_HANDLER_EV_TYPE_ENT     evType,
  IN  RX_EV_PACKET_RECEIVE_CB_FUN   cbFunc,
  IN  GT_UINTPTR                    cookie
);

/*******************************************************************************
* rxEventHandlerRemoveCallback
*
* DESCRIPTION:
*       Add callback function for Rx events
*
* INPUTS:
*       devNum  - the device number or RX_EV_HANDLER_DEVNUM_ALL
*       queue   - the rx queue number or RX_EV_HANDLER_QUEUE_ALL
*       evType  - event type
*       cbFunc  - callback function
*       cookie  - cookie passed to rxEventHandlerAddCallback()
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS rxEventHandlerRemoveCallback(
  IN  GT_U8                         devNum,
  IN  GT_U8                         queue,
  IN  RX_EV_HANDLER_EV_TYPE_ENT     evType,
  IN  RX_EV_PACKET_RECEIVE_CB_FUN   cbFunc,
  IN  GT_UINTPTR                    cookie
);


/*******************************************************************************
* rxEventHandlerDoCallbacks
*
* DESCRIPTION:
*       Execute all callback routines for event handler
*
* INPUTS:
*       handle      - the handle of CPSS event handler
*       evType      - event type
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuff   - Num of used buffs in packetBuffs.
*       packetBuffs - The received packet buffers list.
*       buffLen     - List of buffer lengths for packetBuffs.
*       rxParamsPtr - (pointer to)information parameters of received packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS rxEventHandlerDoCallbacks
(
    IN GT_UINTPTR                   handle,
    IN RX_EV_HANDLER_EV_TYPE_ENT    evType,
    IN GT_U8                        devNum,
    IN GT_U8                        queue,
    IN GT_U32                       numOfBuff,
    IN GT_U8                        *packetBuffs[],
    IN GT_U32                       buffLen[],
    IN GT_VOID                      *rxParamsPtr
);

#endif /* __rxEventHandler_h__ */
