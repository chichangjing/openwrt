/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrafficTable.h
*
* DESCRIPTION:
*       Manage the Rx Tx tables of the traffic sent from CPU and traffic
*       received at the CPU
*
* FILE REVISION NUMBER:
*       $Revision: 8.*
*******************************************************************************/
#ifndef __tgfTrafficTableh
#define __tgfTrafficTableh

#include <extUtils/rxEventHandler/rxEventHandler.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************\
 *                              Define section                                *
\******************************************************************************/

/* Rx buffer size */
#define TGF_RX_BUFFER_MAX_SIZE_CNS   0x3FFF

/* Tx buffer size */
#define TGF_TX_BUFFER_MAX_SIZE_CNS   0x3FFF

/* Tx buffer min size */
#define TGF_TX_BUFFER_MIN_SIZE_CNS   0x3C

/* default sleep time in single loop step to get captured packet */
#define TGF_DEFAULT_CAPTURE_SLEEP_TIME_CNS 10

/* number of time loops to get captured packet */
extern GT_U32   prvTgfCaptureTimeLoop;

/******************************************************************************\
 *                              Common types section                          *
\******************************************************************************/

/*******************************************************************************
* TGF_FREE_RX_INFO_FUNC
*
* DESCRIPTION:
*       Prototype of callback function to free the specific rx info format.
*
* INPUTS:
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*TGF_FREE_RX_INFO_FUNC)
(
    IN GT_VOID*  specificDeviceFormatPtr
);

/*******************************************************************************
* TGF_COPY_RX_INFO_FUNC
*
* DESCRIPTION:
*       Prototype of callback function to COPY the specific rx info format.
*
* INPUTS:
*       srcSpecificDeviceFormatPtr - (pointer to) the SRC specific device Rx
*                                    info format.
*
* OUTPUTS:
*       dstSpecificDeviceFormatPtr - (pointer to) the DST specific device Rx
*                                    info format.
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*TGF_COPY_RX_INFO_FUNC)
(
    IN  const GT_VOID*  srcSpecificDeviceFormatPtr,
    OUT       GT_VOID*  dstSpecificDeviceFormatPtr
);

/*
 * Typedef: enum TGF_RX_MODE_ENT
 *
 * Description:
 *      The table entry adding mode for the received packets
 *
 * Enumerations:
 *      TGF_RX_CYCLIC_E - for cyclic collection.
 *      TGF_RX_ONCE_E   - for collecting until buffer is full
 */
typedef enum
{
    TGF_RX_CYCLIC_E,
    TGF_RX_ONCE_E
} TGF_RX_MODE_ENT;

/*
 * Typedef: TGF_RX_ENTRY
 *
 * Description:
 *      Entry of the Rx packet table
 *
 * Fields:
 *      isEmpty - is entry empty flag
 *      devNum  - The device number in which the packet was received.
 *      queue   - The Rx queue in which the packet was received.
 *      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
 *      freeRxInfoFunc    - callback function to free the specific rx info format,
 *      copyRxInfoFunc    - callback function to copy the specific rx info format,
 *      packetBufferPtr   - pointer to the packet buffer (allocated by this C file)
 *      packetOriginalLen - length of original packet (packet may have been too
 *                          long , so we didn't keep all of it only 1536 bytes)
 *      bufferLen -length of the buffer allocated in packetBufferPtr
 */
typedef struct
{
    GT_BOOL               isEmpty;
    GT_U8                 devNum;
    GT_U8                 queue;
    GT_VOID*              specificDeviceFormatPtr;
    TGF_FREE_RX_INFO_FUNC freeRxInfoFunc;
    TGF_COPY_RX_INFO_FUNC copyRxInfoFunc;
    GT_U8*                packetBufferPtr;
    GT_U32                packetOriginalLen;
    GT_U32                bufferLen;
} TGF_RX_ENTRY;


/******************************************************************************\
 *                            Public API section                              *
\******************************************************************************/
/*******************************************************************************
* prvTgfTrafficTableRxStartCapture
*
* DESCRIPTION:
*       Start/Stop collecting the received packets without delay.
*
* INPUTS:
*       enable - GT_TRUE - start capture
*                GT_FALSE - stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficTableRxStartCapture
(
    IN GT_BOOL          enable
);

/*******************************************************************************
* tgfTrafficTableRxStartCapture
*
* DESCRIPTION:
*       Start/Stop collecting the received packets
*
* INPUTS:
*       enable - GT_TRUE - start capture
*                GT_FALSE - stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxStartCapture
(
    IN GT_BOOL          enable
);

/*******************************************************************************
* tgfTrafficTableRxModeSet
*
* DESCRIPTION:
*       Set received packets collection mode and parameters
*
* INPUTS:
*       rxMode        - collection mode
*       buferSize     - buffer size
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_OUT_OF_CPU_MEM - memory allocation Failure
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxModeSet
(
    IN TGF_RX_MODE_ENT  rxMode,
    IN GT_U32           buferSize
);

/*******************************************************************************
* tgfTrafficTableRxPcktReceiveCb
*
* DESCRIPTION:
*       Function called to handle incoming Rx packet in the CPU
*
* INPUTS:
*       devNum       - Device number.
*       queueIdx     - The queue from which this packet was received.
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (pointer to)information parameters of received packets
*
* RETURNS:
*       GT_OK      - on success
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxPcktReceiveCb
(
    IN GT_UINTPTR                cookie,
    IN RX_EV_HANDLER_EV_TYPE_ENT evType,
    IN GT_U8            devNum,
    IN GT_U8            queueIdx,
    IN GT_U32           numOfBuff,
    IN GT_U8            *packetBuffs[],
    IN GT_U32           buffLen[],
    IN GT_VOID          *rxParamsPtr
);

/*******************************************************************************
* tgfTrafficTableRxPcktTblClear
*
* DESCRIPTION:
*       Clear table
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxPcktTblClear
(
    GT_VOID
);

/*******************************************************************************
* tgfTrafficTableRxPcktGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       currentIndex    - the current entry's index
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - the length of the copied packet to gtBuf
*       packetLenPtr    - the Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr        - The Rx queue in which the packet was received.
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*
* RETURNS:
*       GT_OK - on success
*       GT_OUT_OF_RANGE - index is out of range
*       GT_NOT_FOUND -  entry in index is entry not valid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTableRxPcktGet
(
    INOUT GT_U32        currentIndex,
    OUT   GT_U8         *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT   GT_U32        *packetLenPtr,
    OUT   GT_U8         *devNumPtr,
    OUT   GT_U8         *queuePtr,
    OUT   GT_VOID       *specificDeviceFormatPtr
);

/*******************************************************************************
* tgfTrafficTableRxPcktIsCaptureReady
*
* DESCRIPTION:
*       Check if the engine is ready to capture rx packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the engine is ready to capture rx packets
*       GT_FALSE - the engine is NOT ready to capture rx packets
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL tgfTrafficTableRxPcktIsCaptureReady
(
    GT_VOID
);

/*******************************************************************************
* tgfTrafficTableNetRxPacketCbRegister
*
* DESCRIPTION:
*       Register a CB function to be called on every RX packet to CPU
*
* INPUTS:
*       rxPktReceiveCbFun - CB function to be called on every RX packet to CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_STATE - om illegal state
*
* COMMENTS:
*       Function is implemented in the AppDemo application
*
*******************************************************************************/
GT_STATUS tgfTrafficTableNetRxPacketCbRegister
(
    IN  RX_EV_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
);

/*******************************************************************************
* tgfTrafficTableRxDoCountOnlySet
*
* DESCRIPTION:
*       Function sets Counting Only for incoming Rx packet in the CPU
*
* INPUTS:
*       doCountOnly - GT_TRUE - only count packets
*                     GT_FALSE - make regular capture treatment.
*
* RETURNS:
*       previouse state of Do Count Only
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL tgfTrafficTableRxDoCountOnlySet
(
    IN GT_BOOL         doCountOnly
);

/*******************************************************************************
* tgfTrafficTableRxToTxSet
*
* DESCRIPTION:
*       Function sets Rx To Tx mode of Rx Packet callback
*
* INPUTS:
*       doRxToTx - GT_TRUE - each Rx packet goes to Tx from CPU
*                  GT_FALSE - make regular capture treatment.
*
* RETURNS:
*       previouse state of Rx To Tx mode
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL tgfTrafficTableRxToTxSet
(
    IN GT_BOOL         doRxToTx
);

/*******************************************************************************
* tgfTrafficTableRxCountGet
*
* DESCRIPTION:
*       Function returns counter of RX packets
*
* INPUTS:
*       none
*
* RETURNS:
*       counter
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 tgfTrafficTableRxCountGet
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfTrafficTableh */

