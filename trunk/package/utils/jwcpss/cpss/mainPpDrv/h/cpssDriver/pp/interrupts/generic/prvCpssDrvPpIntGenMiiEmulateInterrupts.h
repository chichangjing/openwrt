/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvPpIntGenMiiEmulateInterrupts.h
*
* DESCRIPTION:
*       Includes general definitions for the mii interrupts emulation handling
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __prvCpssDrvPpIntGenMiiEmulateInterruptsh
#define __prvCpssDrvPpIntGenMiiEmulateInterruptsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>

/*******************************************************************************
* interruptEthPortRxSR
*
* DESCRIPTION:
*       This function is invoked when a packet has been received from the
*       Ethernet port driver.
*
* INPUT:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segment length.
*       numOfSegments   - The number of segment in segment list.
*       rxQueue         - the received queue number
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS interruptMiiRxSR
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           rxQueue
);

/*******************************************************************************
* rawPacketRx
*
* DESCRIPTION:
*       This function is invoked when a packet has been received from the
*       raw packts handler task
*
* INPUT:
*       packet          - A pointer to the entire packet
*       packetLen       - The length of the packet.
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS rawSocketRx
(
    IN GT_U8_PTR        packet,
    IN GT_U32           packetLen,
    IN GT_U32           rxQueue
);

/*******************************************************************************
* interruptEthPortTxEndSR
*
* DESCRIPTION:
*       This function is invoked when a packet transmit has ended from the
*       Ethernet port driver.
*
* INPUT:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS interruptMiiTxEndSR
(
    IN GT_U8              * segmentList[],
    IN GT_U32               numOfSegment
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvPpIntGenMiiEmulateInterruptsh */


