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
* cmdNetReceive.h
*
* DESCRIPTION:
*       This file implements Packet Capture functionally user exit functions
*       for Galtis.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
******************************************************************************/

#ifndef __cmdNetReceive_h__
#define __cmdNetReceive_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GALTIS_RX_BUFFER_MAX_SIZE_CNS   0x600   /*1536*/


/*******************************************************************************
* FREE_RX_INFO_FUNC
*
* DESCRIPTION:
*       prototype of callback function to free the specific rx info format.
*
* INPUTS:
*      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
typedef void (*FREE_RX_INFO_FUNC)
(
    IN void*  specificDeviceFormatPtr
);

/*******************************************************************************
* COPY_RX_INFO_FUNC
*
* DESCRIPTION:
*       prototype of callback function to COPY the specific rx info format.
*
* INPUTS:
*      srcSpecificDeviceFormatPtr - (pointer to) the SOURCE specific device Rx
*                                   info format.
*                                   format of DXCH / EXMX /DXSAL ...
*
* OUTPUTS:
*      dstSpecificDeviceFormatPtr - (pointer to) the DESTINATION specific device Rx
*                                   info format.
*                                   format of DXCH / EXMX /DXSAL ...
*
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
typedef void (*COPY_RX_INFO_FUNC)
(
    IN  const void*  srcSpecificDeviceFormatPtr,
    OUT void*        dstSpecificDeviceFormatPtr
);

/*
 * Typedef: enum GALTIS_RX_MODE_ENT
 *
 * Description:
 *      The table entry adding mode for the received packets
 *
 * Enumerations:
 *      GALTIS_RX_CYCLIC_E - for cyclic collection.
 *      GALTIS_RX_ONCE_E   - for collecting until buffer is full
 */
typedef enum
{
    GALTIS_RX_CYCLIC_E,
    GALTIS_RX_ONCE_E
} GALTIS_RX_MODE_ENT;

/*******************************************************************************
* cmdCpssRxStartCapture
*
* DESCRIPTION:
*       Start collecting the received packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssRxStartCapture
*
*******************************************************************************/
GT_STATUS cmdCpssRxStartCapture(void);

/*******************************************************************************
* cmdCpssRxStopCapture
*
* DESCRIPTION:
*       Stop collecting the received packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssRxStopCapture
*
*
*******************************************************************************/
GT_STATUS cmdCpssRxStopCapture(void);

/*******************************************************************************
* cmdCpssRxSetMode
*
* DESCRIPTION:
*       set received packets collection mode and parameters
*
* INPUTS:
*       rxMode        - collection mode
*       buferSize     - buffer size
*       numOfEnteries - number of entries
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success
*       GT_OUT_OF_CPU_MEM   - memory allocation Failure
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssRxSetMode
*
*
*******************************************************************************/
GT_STATUS cmdCpssRxSetMode
(
    IN GALTIS_RX_MODE_ENT   rxMode,
    IN GT_U32       buferSize,
    IN GT_U32       numOfEnteries
);


/*******************************************************************************
* cmdCpssRxGetMode
*
* DESCRIPTION:
*       Get received packets collection mode and parameters
*
* INPUTS:
*       None
*
* OUTPUTS:
*       modePtr        - GALTIS_RX_CYCLIC_E or GALTIS_RX_ONCE_E
*       buffSizePtr    - packet buffer size
*       numEntriesPtr  - number of entries
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
* GalTis:
*       Command - cmdCpssRxGetMode
*
*
*******************************************************************************/
GT_STATUS cmdCpssRxGetMode
(
    OUT GALTIS_RX_MODE_ENT  *modePtr,
    OUT GT_U32      *buffSizePtr,
    OUT GT_U32      *numEntriesPtr
);

/*******************************************************************************
* cmdCpssRxPktReceive
*
* DESCRIPTION:
*       Receive packet callback function . the caller wants to register its
*       packet info with the manager array.
*
*       This function give services to all the rx packets info formats.
*
* INPUTS:
*      devNum  - The device number in which the packet was received.
*      queue   - The Rx queue in which the packet was received.
*      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*                             NOTE : this pointer is allocated by the specific
*                             device "C" file.
*      freeRxInfoFunc - callback function to free the specific rx info format,
*      copyRxInfoFunc - callback function to copy the specific rx info format,
*      numOfBuff    - Num of used buffs in packetBuffs
*      packetBuffs  - The received packet buffers list
*      buffLen      - List of buffer lengths for packetBuffs
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FULL - when buffer is full
*       GT_BAD_STATE - the galtis-cmd not ready/set to "save" the packet with info
*       GT_BAD_PARAM - on bad parameter
*       GT_OUT_OF_CPU_MEM - on CPU memory allocation failure
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
GT_STATUS cmdCpssRxPktReceive
(
    IN GT_U8           devNum,
    IN GT_U8           queue,
    IN void*           specificDeviceFormatPtr,
    IN FREE_RX_INFO_FUNC freeRxInfoFunc,
    IN COPY_RX_INFO_FUNC copyRxInfoFunc,
    IN GT_U32          numOfBuff,
    IN GT_U8           *packetBuffs[],
    IN GT_U32          buffLen[]
);

/*******************************************************************************
* cmdCpssRxPktClearTbl
*
* DESCRIPTION:
*       clear cmdCpssRxPktClearTbl table
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
* GalTis:
*       Table - rxNetIf
*
*******************************************************************************/
GT_STATUS cmdCpssRxPktClearTbl(void);

/*******************************************************************************
* cmdCpssRxPkGetFirst
*
* DESCRIPTION:
*       Get Next entry from rxNetworkIf table
*
* INPUTS:
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       currentIndexPtr - get current entry's index
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - the length of the copied packet to gtBuf
*       packetLenPtr    - the Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr       - The Rx queue in which the packet was received.
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*
* RETURNS:
*       GT_OK - on success
*       GT_NO_MORE - when no more entries
*
* COMMENTS:
*
* GalTis:
*       Table - rxNetworkIf
*
*******************************************************************************/
GT_STATUS cmdCpssRxPkGetFirst
(
    OUT GT_U32          *currentIndexPtr,
    OUT GT_U8           *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT GT_U32          *packetLenPtr,
    OUT GT_U8           *devNumPtr,
    OUT GT_U8           *queuePtr,
    OUT void*           specificDeviceFormatPtr

);

/*******************************************************************************
* cmdCpssRxPkGetNext
*
* DESCRIPTION:
*       Get Next entry from rxNetworkIf table
*
* INPUTS:
*       packetBufLenPtr - the length of the user space for the packet
*       currentIndexPtr - the PREVIOUS entry's index
*
* OUTPUTS:
*       currentIndexPtr - get current entry's index
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - the length of the copied packet to gtBuf
*       packetLenPtr    - the Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr       - The Rx queue in which the packet was received.
*       specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
*                             format of DXCH / EXMX /DXSAL ...
*
* RETURNS:
*       GT_OK - on success
*       GT_NO_MORE - when no more entries
*
* COMMENTS:
*
* GalTis:
*       Table - rxNetworkIf
*
*******************************************************************************/
GT_STATUS cmdCpssRxPkGetNext
(
    INOUT GT_U32        *currentIndexPtr,
    OUT GT_U8           *packetBufPtr,
    INOUT GT_U32        *packetBufLenPtr,
    OUT GT_U32          *packetLenPtr,
    OUT GT_U8           *devNumPtr,
    OUT GT_U8           *queuePtr,
    OUT void*           specificDeviceFormatPtr

);


/*******************************************************************************
* cmdCpssRxPktIsCaptureReady
*
* DESCRIPTION:
*       check if the galtis-cmd is ready to capture rx packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the galtis-cmd is ready to capure rx packets
*       GT_FALSE - the galtis-cmd is NOT ready to capure rx packets
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL cmdCpssRxPktIsCaptureReady(void);

/*
 * Typedef: enum GT_RX_ENTRY_TYPE_ENT
 *
 * Description:
 *      The table entry type of packet
 *
 * Enumerations:
 *      GT_RX_ENTRY_TYPE_GENERIC_E - generic packet
 *      GT_RX_ENTRY_TYPE_CAPTURED_BY_ENH_UT_E - packet trapped to the CPU by the ENH-UT engine
 *                                  and is considered internal packet
*/
typedef enum {
    GT_RX_ENTRY_TYPE_GENERIC_E,
    GT_RX_ENTRY_TYPE_CAPTURED_BY_ENH_UT_E,

    GT_RX_ENTRY_TYPE___LAST___E/* must be last*/
}GT_RX_ENTRY_TYPE_ENT;

/***** Capture packet table entry  *******************************/

/*
 * Typedef: CMD_RX_ENTRY
 *
 * Description:
 *      Entry of the Rx packet table
 *
 * Fields:
 *      isEmpty - is entry empty flag
 *      devNum  - The device number in which the packet was received.
 *      queue   - The Rx queue in which the packet was received.
 *      specificDeviceFormatPtr - (pointer to) the specific device Rx info format.
 *                             format of DXCH / EXMX /DXSAL ...
 *                             NOTE : this pointer is allocated by the specific
 *                             device "C" file.
 *      freeRxInfoFunc - callback function to free the specific rx info format,
 *      copyRxInfoFunc - callback function to copy the specific rx info format,
 *      packetBufferPtr - pointer to the packet buffer (allocated by this C file)
 *      packetOriginalLen - length of original packet (packet may have been too
 *                          long , so we didn't keep all of it only 1536 bytes)
 *      bufferLen -length of the buffer allocated in packetBufferPtr
 *      entryType - indication about the type of the entry
 */
typedef struct {
    GT_BOOL         isEmpty;
    GT_U8           devNum;
    GT_U8           queue;
    void*           specificDeviceFormatPtr;
    FREE_RX_INFO_FUNC freeRxInfoFunc;
    COPY_RX_INFO_FUNC copyRxInfoFunc;
    GT_U8*          packetBufferPtr;
    GT_U32          packetOriginalLen;
    GT_U32          bufferLen;
    GT_RX_ENTRY_TYPE_ENT entryType;
} CMD_RX_ENTRY;




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdNetReceive_h__ */



