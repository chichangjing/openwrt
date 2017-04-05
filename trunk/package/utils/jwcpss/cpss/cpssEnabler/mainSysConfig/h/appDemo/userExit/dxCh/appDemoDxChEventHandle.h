/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* appDemoDxChEventHandle.h
*
* DESCRIPTION:
*       this library contains the implementation of the event handling functions
*       for the DXCH device  , and the redirection to the GalTis Agent for
*       extra actions .
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/userExit/userEventHandler.h>
#include <extUtils/rxEventHandler/rxEventHandler.h>

/*******************************************************************************
* appDemoDxChNetRxPacketCbRegister
*
* DESCRIPTION:
*       register a CB function to be called on every RX packet to CPU
*
*       NOTE: function is implemented in the 'AppDemo' (application)
*
* INPUTS:
*       rxPktReceiveCbFun - CB function to be called on every RX packet to CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoDxChNetRxPacketCbRegister
(
    IN  RX_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
);


/*******************************************************************************
* appDemoDxChNetRxPktHandle
*
* DESCRIPTION:
*       application routine to receive frames . -- DXCH function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
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
GT_STATUS appDemoDxChNetRxPktHandle
(
    IN GT_UINTPTR                           evHandler,
    IN RX_EV_HANDLER_EV_TYPE_ENT            evType,
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx
);

/*******************************************************************************
* appDemoDxChNetRxPktGet
*
* DESCRIPTION:
*       application routine to get the packets from the queue and 
*       put it in msgQ. -- DXCH function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       msgQId      - Message queue Id.
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
GT_STATUS appDemoDxChNetRxPktGet
(
    IN GT_U8            devNum,
    IN GT_U8            queueIdx,
    IN CPSS_OS_MSGQ_ID  msgQId
);

/*******************************************************************************
* appDemoDxChNetRxPktTreat
*
* DESCRIPTION:
*       application routine to treat the packets . -- DXCH function
*
* INPUTS:
*       rxParamsPtr - (pointer to) rx paacket params
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
GT_STATUS appDemoDxChNetRxPktTreat
(
    IN APP_DEMO_RX_PACKET_PARAMS  *rxParamsPtr
);

/*******************************************************************************
* appDemoDxChNetPtpOverUdpWaCb
*
* DESCRIPTION:
*        ?
*
* APPLICABLE DEVICES: ALL Devices
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_OK                    - no error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxChNetPtpOverUdpWaCb
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    IN GT_U32                               numOfBuff,
    IN GT_U8                               *packetBuffs[],
    IN GT_U32                               buffLen[],
    IN void                                 *rxParamsPtr
);
