/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenNetworkIfMii.h
*
* DESCRIPTION:    generic network interface MII types definitions
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*******************************************************************************/

#ifndef __prvCpssGenNetworkIfMiih
#define __prvCpssGenNetworkIfMiih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>

/* Number of interrupt queues */
#define PRV_CPSS_GEN_NETIF_MAX_RX_QUEUES_NUM_CNS 8

/* Maximum number of segments in a single gtBuf */
#define PRV_CPSS_GEN_NETIF_MAX_GT_BUF_SEG_NUM_CNS  100

#define PRV_CPSS_GEN_NETIF_MII_DEV_NOT_SET 0xFFFFFFFF


/*
 * Typedef: enum PRV_CPSS_GEN_NETIF_MII_TX_MODE_ENT
 *
 * Description: Define the MII transmission mode.
 *
 * Enumerations:
 *  PRV_CPSS_GEN_NETIF_MII_TX_MODE_ASYNCH_E - don't wait for TX done - free packet
 *                                            when interrupt received 
 *  PRV_CPSS_GEN_NETIF_MII_TX_MODE_SYNCH_E - wait to TX done and free packet immediately
 *
 */
typedef enum
{
    PRV_CPSS_GEN_NETIF_MII_TX_MODE_ASYNCH_E = 0,
    PRV_CPSS_GEN_NETIF_MII_TX_MODE_SYNCH_E  = 1
} PRV_CPSS_GEN_NETIF_MII_TX_MODE_ENT;

/*
 * typedef: struct PRV_CPSS_NETIF_MII_TX_CTRL_STC
 *
 * Description:  Defines the MII TX data
 *
 * Fields:
 *      txMode            - TX mode
 *
 * Comment:
 */
typedef struct
{
    PRV_CPSS_GEN_NETIF_MII_TX_MODE_ENT txMode;
} PRV_CPSS_NETIF_MII_TX_CTRL_STC;

/*******************************************************************************
* PRV_CPSS_NETIF_MII_PACKET_FREE_FUNC
*
* DESCRIPTION:
*       The prototype of the routine to be called to free the received Rx buffer.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - Receive queue number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_NETIF_MII_PACKET_FREE_FUNC)
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
);

/*
 * typedef: struct PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC
 *
 * Description:  Defines the ethernet port RX Queue data
 *
 * Fields:
 *      packetFreeFunc_PTR      - free segments for RXed packet routine.
 *      headerOffset            - the header size added to each packet recieved.
 *      intQueId                - the interrupt queue id.
 *      auQueId                 - the mirrored Address Update queue id used in
 *                                polling mode and controlled learning through
 *                                DSA tag.
 *      numOfRxBufs             - the number of buffers used for Rx.
 *      rxInPktsCounter         - Packets received on this queue since last read.
 *      rxInOctetsCounter       - Octets received on this queue since last read.
 *
 * Comment:
 */
typedef struct
{
    PRV_CPSS_NETIF_MII_PACKET_FREE_FUNC  packetFreeFunc_PTR;
    GT_U32               headerOffset;
    CPSS_OS_MSGQ_ID      intQueId[PRV_CPSS_GEN_NETIF_MAX_RX_QUEUES_NUM_CNS];
    void *               auQueId;
    GT_U32               numOfRxBufs;
    GT_U32               rxInPktsCounter[PRV_CPSS_GEN_NETIF_MAX_RX_QUEUES_NUM_CNS];
    GT_U32               rxInOctetsCounter[PRV_CPSS_GEN_NETIF_MAX_RX_QUEUES_NUM_CNS];
} PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC;

/*
 * typedef: struct PRV_CPSS_GEN_NETIF_ETH_PORT_CTRL_VALID_STC
 *
 * Description:  flag indicating that the control struct is valid
 *
 * Fields:
 *      rxValid   - Rx control struct has been initialized and valid
 *      txValid   - Tx control struct has been initialized and valid.
 *
 * Comment:
 */
typedef struct
{
    GT_BOOL         rxValid;
    GT_BOOL         txValid;
} PRV_CPSS_GEN_NETIF_ETH_PORT_CTRL_VALID_STC;


/*
 * typedef: struct PRV_CPSS_GEN_NETIF_RX_Q_MSG_STC
 *
 * Description: message sent from rx SR to interrupt handler.
 *
 * Fields:
 *      rxQueue         - the Rx queue number
 *      seg             - first segment
 *
 * Comments:
 */
typedef struct
{
    GT_U32      rxQueue;
    GT_UINTPTR  seg;
} PRV_CPSS_GEN_NETIF_RX_Q_MSG_STC;

/*
 * typedef: struct PRV_CPSS_GEN_NETIF_SEG_STCT
 *
 * Description: Segment list is used by the Rx ISR and the interrupt
 *              handler routine. The first segment pointer is passed
 *              by Queue from ISR to interrupt handler.
 *
 * Values:
 *      len     - the segment length
 *      seg     - pointer to a segment
 *      nextSeg - next segment pointer
 *
 * Comments:
 */
typedef struct PRV_CPSS_GEN_NETIF_SEG_STCT
{
    GT_U32               len;
    GT_U8                *seg_PTR;
    struct PRV_CPSS_GEN_NETIF_SEG_STCT *nextSeg_PTR;
} PRV_CPSS_GEN_NETIF_SEG_STC;

/*
 * Typedef: enum PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT
 *
 * Description:
 *  Define the buffer pool type.
 *
 * Enumerations:
 *  PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E - RX segments pool
 *  PRV_CPSS_GEN_NETIF_MII_TX_POOL_E - TX header buffers pool
 *  PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E - max pools
 *  
 */
typedef enum
{
    PRV_CPSS_GEN_NETIF_MII_RX_SEG_POOL_E   = 0,
    PRV_CPSS_GEN_NETIF_MII_TX_POOL_E       = 1,
    PRV_CPSS_GEN_NETIF_MII_MAX_POOLS_E     = 2
} PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT;

/*
 * typedef: struct PRV_CPSS_GEN_NETIF_MII_STAT_STC
 *
 * Description:
 *
 * Fields:
 *      pcktIn      - number of packets received counter
 *      pcktTreated - number of packets treated counter
 *      pcktLen     - number of packets with invalid length counter
 *      command     - number of packets with invalid Marvell-Tag command counter
 *      cpuCode     - number of packets with invalid Marvell-Tag CPU code counter
 *      noQbuffer   - lack of buffer resource counter
 *      msgSend     - message queue send failed counter
 *      intType     - invalid interrupt type counter
 *      device      - invalide device number (from Marvell-Tag) counter
 *
 * Comment:
 */
typedef struct
{
    GT_U32          pcktIn;
    GT_U32          pcktTreated;
    GT_U32          pcktLen;
    GT_U32          command;
    GT_U32          cpuCode;
    GT_U32          noQbuffer;
    GT_U32          msgSend;
    GT_U32          intType;
    GT_U32          device;
} PRV_CPSS_GEN_NETIF_MII_STAT_STC;

/* Rx/Tx queue message size */
#define PRV_CPSS_NETIF_MII_RX_Q_MSG_LEN_CNS         (sizeof(PRV_CPSS_GEN_NETIF_RX_Q_MSG_STC))


/* Header buffer size and offsets
 * Here are the pool buffer and original packet buffer offsets that are used
 * to build the first segment (buffer taken from pool) and the offset of the
 * original buffer to be used as the second segment.
 *
 *          first buffer          second buffer           second buffer
 *          taken from            original buffer         original buffer
 *          pool header           with VLAN tag           no VLAN tag
 *        _______________         _______________         _______________
 *       |  txDevice     |       |  MAC dest[0]  |       |  MAC dest[0]  |
 *       |  txQueue      |       |  MAC dest[1]  |       |  MAC dest[1]  |
 *       |   reserved    |       |  MAC dest[2]  |       |  MAC dest[2]  |
 *       |   reserved    |       |  MAC dest[3]  |       |  MAC dest[3]  |
 *       |   cookie[0]   |       |  MAC dest[4]  |       |  MAC dest[4]  |
 *       |   cookie[1]   |       |  MAC dest[5]  |       |  MAC dest[5]  |
 *       |   cookie[2]   |       |  MAC src[0]   |       |  MAC src[0]   |
 *       |   cookie[3]   |       |  MAC src[1]   |       |  MAC src[1]   |
 *       |   handle[0]   |       |  MAC src[2]   |       |  MAC src[2]   |
 *       |   handle[1]   |       |  MAC src[3]   |       |  MAC src[3]   |
 *       |   handle[2]   |       |  MAC src[4]   |       |  MAC src[4]   |
 *       |   handle[3]   |       |  MAC src[5]   |       |  MAC src[5]   |
 *       |  MAC dest[0]  |<-ptr  |     0x81      |       | EtherType/Len |<-ptr
 *       |  MAC dest[1]  |       |     0x00      |       | ngth field    |
 *       |  MAC dest[2]  |       |   802.1Q Tag  |       |    PAYLOAD    |
 *       |  MAC dest[3]  |       |   802.1Q Tag  |       |       "       |
 *       |  MAC dest[4]  |       | EtherType/Len |<-ptr  |       "       |
 *       |  MAC dest[5]  |       | ngth field    |       |       "       |
 *       |  MAC src[0]   |       |    PAYLOAD    |       -----------------
 *       |  MAC src[1]   |       |       "       |
 *       |  MAC src[2]   |       -----------------
 *       |  MAC src[3]   |
 *       |  MAC src[4]   |
 *       |  MAC src[5]   |
 *       |  MRVL tag[0]  |
 *       |  MRVL tag[1]  |
 *       |  MRVL tag[2]  |
 *       |  MRVL tag[3]  |
 *       -----------------
 */
#define PRV_CPSS_GEN_NETIF_MII_HDR_DEVICE_OFFSET_CNS       0 /* GT_U8 */
#define PRV_CPSS_GEN_NETIF_MII_HDR_QUEUE_OFFSET_CNS        1 /* GT_U8 */
#define PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE_OFFSET_CNS       4
#define PRV_CPSS_GEN_NETIF_MII_HDR_HNDL_OFFSET_CNS         8
#define PRV_CPSS_GEN_NETIF_MII_HDR_MAC_SIZE_CNS     12
#define PRV_CPSS_GEN_NETIF_MII_HDR_MAC_OFFSET_CNS   12
#define PRV_CPSS_GEN_NETIF_MII_HDR_MRVLTAG_OFFSET_CNS 24

/* additional user packet header, this is used to indicate if the packet  */
/* was received tagged or not.                                            */
#define PRV_CPSS_NETIF_MII_TAGGED_PCKT_FLAG_LEN_CNS    4

#define PRV_CPSS_GEN_NETIF_MII_BUFF_MIN_SIZE_CNS    60

/* offset in original packet for un/tagged frames */
#define PRV_CPSS_GEN_NETIF_BUFF_PCKT_UNTAGGED_OFFSET_CNS 12
#define PRV_CPSS_GEN_NETIF_BUFF_PCKT_TAGGED_OFFSET_CNS   16

#define PRV_CPSS_GEN_NETIF_MRVL_TAG_PCKT_OFFSET_CNS 12
#define PRV_CPSS_GEN_NETIF_VLAN_ETHR_TYPE_MSB_CNS   0x81
#define PRV_CPSS_GEN_NETIF_VLAN_ETHR_TYPE_LSB_CNS   0x00


/* the Rx "save" 4 bytes for internal use */
#define PRV_CPSS_NETIF_MII_RX_OFFSET_TO_INTERNAL_INFO_CNS(headerOffset) \
    (headerOffset + PRV_CPSS_NETIF_MII_TAGGED_PCKT_FLAG_LEN_CNS)

/* INOUT GT_U8* buffPtr
   buffPtr -- the pointer to the buffer as it is going to be sent to the
              application
   hdrOffset --
   bytesMoved - number of byte moved from the start of the original buffers that
                came from the external driver
   --- comment : use those macros only when this is FIRST buffer . ---
*/
#define PRV_CPSS_NETIF_MII_RX_ORIG_PCKT_TAG_BYTES_GET(bufPtr, headerOffset)   \
                (*((bufPtr)-PRV_CPSS_NETIF_MII_RX_OFFSET_TO_INTERNAL_INFO_CNS(headerOffset)))

#define PRV_CPSS_NETIF_MII_RX_ORIG_PCKT_TAG_BYTES_SET(bufPtr,bytesMoved,headerOffset)  \
    {                                                                                     \
        (*((bufPtr)-PRV_CPSS_NETIF_MII_RX_OFFSET_TO_INTERNAL_INFO_CNS(headerOffset))) = (GT_U8)(bytesMoved);\
    }

/*
 * Typedef: PRV_CPSS_NETIF_MII_RX_BUFF_MALLOC_FUNC
 *
 * Description: Prototype of the function pointer to be provided by the
 *              user to perform Rx buffer allocation.
 *              The CPSS invokes this user function when allocating
 *              Rx buffers. This is relevant only if  the DYNAMIC_ALLOC
 *              allocation method is used.
 *
 * Fields:
 *      size   - the size of the Rx buffer to allocate.  This value reflects
 *               the Rx buffer size that the user defines in the
 *               TAPI gtSysConfig.h structure GT_SYS_CONFIG
 *      align - The required byte-alignment for the Rx buffer
 *
 */
typedef GT_U8* (*PRV_CPSS_NETIF_MII_RX_BUFF_MALLOC_FUNC)
(
    IN GT_U32 size,
    IN GT_U32 align
);

/*******************************************************************************
* prvCpssGenNetIfMiiInitNetIfDev
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx buffers and Tx header
*       buffers (For a single device).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device number.
*       numOfTxDesc - number of requested TX descriptors.
*       txInternalBufBlockPtr  - (Pointer to) a block of host memory to be used
*                                for internal TX buffers.
*       txInternalBufBlockSize - The raw size in bytes of txInternalBufBlock memory.
*       bufferPercentageArr    - A table (entry per queue) that describes the buffer
*                         dispersal among all Rx queues. (values 0..100)
*       rxBufSize       - The size of the Rx data buffer.
*       headerOffset    - packet header offset size
*       rxBufBlockPtr   - (Pointer to) a block of memory to be used for
*                        allocating Rx packet data buffers.
*       rxBufBlockSize  - The raw size in byte of rxDataBufBlock.
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
GT_STATUS prvCpssGenNetIfMiiInitNetIfDev
(
    IN GT_U8     devNum,
    IN GT_U32    numOfTxDesc,
    IN GT_U8    *txInternalBufBlockPtr,
    IN GT_U32    txInternalBufBlockSize,
    IN GT_U32    bufferPercentageArr[CPSS_MAX_RX_QUEUE_CNS],
    IN GT_U32    rxBufSize,
    IN GT_U32    headerOffset,
    IN GT_U8    *rxBufBlockPtr,
    IN GT_U32    rxBufBlockSize
);

/*******************************************************************************
* prvCpssGenNetIfMiiRemove
*
* DESCRIPTION:
*       This function is called upon Hot removal of a device, inorder to release
*       all Network Interface related structures.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*       devNum  - The device that was removed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRemove
(
    IN  GT_U8   devNum
);

/*******************************************************************************
* prvCpssGenNetIfMiiRxBufFree
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number through which these buffers where
*                     received.
*       rxQueue     - The Rx queue number through which these buffers where
*                     received.
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
);

/*******************************************************************************
* prvCpssGenNetIfMiiTxBufferQueueGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*       hndl        - Handle got from gtEventBind.
*
* OUTPUTS:
*       devPtr      - The device the packet was transmitted from.
*       cookiePtr   - The user cookie handed on the transmit request.
*       queuePtr    - The queue from which this packet was transmitted
*       statusPtr   - GT_OK if packet transmission was successful, GT_FAIL on
*                     packet reject.
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - the CPSS not handling the ISR for the device
*                                  so function must not be called
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiTxBufferQueueGet
(
    IN  GT_UINTPTR           hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
);

/*******************************************************************************
* prvCpssGenNetIfMiiRxPacketGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr- Num of buffs in packetBuffsArrPtr.
*
* OUTPUTS:
*       numOfBuffPtr       - Num of used buffs in packetBuffsArrPtr.
*       packetBuffsArrPtr  - (pointer to)The received packet buffers list.
*       buffLenArr         - List of buffer lengths for packetBuffsArrPtr.
*
* RETURNS:
*       GT_NO_MORE               - no more packets on the device/queue
*       GT_OK                    - packet got with no error.
*       GT_FAIL                  - failed to get the packet
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong devNum,portNum,queueIdx.
*       GT_NOT_INITIALIZED       - the library was not initialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRxPacketGet
(
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffsArrPtr[],
    OUT GT_U32                              buffLenArr[]
);

/*******************************************************************************
* prvCpssGenNetIfMiiTxCtrlGet
*
* DESCRIPTION:
*       Return the ethernet port transmit control data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
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
PRV_CPSS_NETIF_MII_TX_CTRL_STC *prvCpssGenNetIfMiiTxCtrlGet(GT_VOID);

/*******************************************************************************
* prvCpssGenNetIfMiiRxQCtrlGet
*
* DESCRIPTION:
*       Return the ethernet port recieve queue control data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
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
PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *prvCpssGenNetIfMiiRxQCtrlGet(GT_VOID);

/*******************************************************************************
* prvCpssGenNetIfMiiDevNumGet
*
* DESCRIPTION:
*       Return the ethernet port device number
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
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
GT_U32 prvCpssGenNetIfMiiDevNumGet(GT_VOID);


/*******************************************************************************
* prvCpssGenNetIfMiiTxModeSet
*
* DESCRIPTION:
*       Set the ethernet port tx mode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       txMode - tx mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise
*       GT_NOT_SUPPORTED
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiTxModeSet
(
    IN PRV_CPSS_GEN_NETIF_MII_TX_MODE_ENT txMode
);


/*******************************************************************************
* prvCpssGenNetIfMiiRxQueueCountersGet
*
* DESCRIPTION:
*       This routine returns the caller rx packets and octets counters
*       for given queue.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*       rxQueue     - rx queue.
*
* OUTPUTS:
*       rxInPktsPtr      - Packets received on this queue.
*       rxInOctetsPtr    - Octets received on this queue.
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRxQueueCountersGet
(
    IN  GT_U32               rxQueue,
    OUT GT_U32               *rxInPktsPtr,
    OUT GT_U32               *rxInOctetsPtr
);

/*******************************************************************************
* prvCpssGenNetIfMiiRxQueueCountersClear
*
* DESCRIPTION:
*       This routine claers rx packets and octets counters for given queue.
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*       rxQueue     - rx queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiRxQueueCountersClear
(
    IN  GT_U32               rxQueue
);

/* ----------- Buffers pool management routines --------------- */

/*******************************************************************************
* prvCpssGenNetIfMiiCreatePool
*
* DESCRIPTION:
*       Create a pool of buffers with pre-allocated memory or allocated memory.
*       Align the pool and buffers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       poolIdx         - the pool index
*       reqBufferSize   - Buffer's requested size
*       numOfBuffersPtr - (pointer to)number of requested buffers in the pool
*       poolPtr         - (pointer to)pool memory allocted by user
*       poolSize        - size of the pointed pool
*       mallocFunc      - Function for allocating the buffers
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to)number of buffers actually initialized in the pool
*
* RETURNS:
*       GT_OK           - For successful operation.
*       GT_BAD_VALUE    - incorrect pool index.
*       GT_NO_RESOURCE  - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       If dynamic allocation, the pool size is calculated from needed buffers
*       and buffer size. If the number of buffer differs from request number,
*       the routine does not fail but returns the actual buffers used.
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiCreatePool
(
    IN      PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT    poolIdx,
    IN      GT_U32                                  reqBufferSize,
    INOUT   GT_U32                                  *numOfBuffersPtr,
    IN      GT_U8                                   *poolPtr,
    IN      GT_U32                                  poolSize,
    IN      PRV_CPSS_NETIF_MII_RX_BUFF_MALLOC_FUNC  mallocFunc
);

/*******************************************************************************
* prvCpssGenNetIfMiiPoolGetBuf
*
* DESCRIPTION:
*       Return pointer to a buffer from the requested pool
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       poolIdx - The pool index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID* - Pointer to the new allocated buffer. NULL is returned in case
*                  no buffer is not available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID *prvCpssGenNetIfMiiPoolGetBuf
(
    IN  PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT poolIdx
);

/*******************************************************************************
* prvCpssGenNetIfMiiPoolFreeBuf
*
* DESCRIPTION:
*       Free a buffer back to its pool.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       poolIdx - The pool index.
*       bufPtr  - Pointer to buffer to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - For successful operation.
*       GT_BAD_PTR          - The returned buffer does not belong to this pool
*       GT_NOT_INITIALIZED  - In case of un-initialized package.
*       GT_BAD_VALUE        - incorrect pool index.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssGenNetIfMiiPoolFreeBuf
(
    IN  PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT   poolIdx,
    IN  GT_VOID                                *bufPtr
);
/*******************************************************************************
* prvCpssGenNetIfMiiPoolGetBufSize
*
* DESCRIPTION:
*       Get the buffer size.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       poolIdx - The pool index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_U32  - the buffer size.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 prvCpssGenNetIfMiiPoolGetBufSize
(
    IN  PRV_CPSS_GEN_NETIF_MII_POOL_TYPE_ENT   poolIdx
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __cpssGenNetIfTypesh */


