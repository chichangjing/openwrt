/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenIntDefs.h
*
* DESCRIPTION:
*       This file includes general structures definitions for interrupts
*       handling, Packet Reception, and Address Update Messages
*
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*
*******************************************************************************/
#ifndef __prvCpssGenIntDefsh
#define __prvCpssGenIntDefsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* support running from the PSS */
#ifndef __gtCoreIntDefsh

/* Defines  */
#define NUM_OF_RX_QUEUES    (8)
#define NUM_OF_TX_QUEUES    (8)

#endif /* #ifndef __gtCoreIntDefsh */

#include <cpssCommon/cpssBuffManagerPool.h>
#include <cpss/generic/cpssTypes.h>

/* support running from the PSS */
#ifndef __gtCoreIntDefsh

/************************   RX Related Definitions  ***************************/

/* macro that define the alignment needed for rx descriptor that given to the PP
relate to packets that the PP send to the CPU*/
#define RX_DESC_ALIGN       (16)  /* In bytes */
/* macro that define the alignment needed for rx buffers that given to the PP ,
relate to packets that the PP send to the CPU*/
/*WA for GM-PUMA2*/
#ifdef EXMXPM_FAMILY
    #define RX_BUFF_ALIGN       (128)   /* In bytes */
#else
    #define RX_BUFF_ALIGN       (8)     /* In bytes */
#endif
/* macro that define the multiple size of rx buffers that given to the PP ,
relate to packets that the PP send to the CPU*/
#define RX_BUFF_SIZE_MULT   (8)   /* In bytes */
/* macro that define the rx descriptor size that given to the PP ,
relate to packets that the PP send to the CPU*/
#define RX_DESC_SIZE        (16)  /* In Bytes */

/* macro that define that CPU own the Rx descriptor --
the value in field "ownership bit" in the Rx descriptor in the PP
relate to packets that the PP send to the CPU */
#define RX_DESC_CPU_OWN     (0)
/* macro that define that DMA (PP) own the Rx descriptor --
the value in field "ownership bit" in the Rx descriptor in the PP
relate to packets that the PP send to the CPU */
#define RX_DESC_DMA_OWN     (1)

/* Max number of descriptors per Rx packet. */
#define RX_MAX_PACKET_DESC  (1000)

/* Resets the Rx descriptor's word1 & word2. */
#define RX_DESC_RESET_MAC(devNum,rxDesc) ((rxDesc)->word1 = CPSS_32BIT_LE(0xA0000000))

/* Returns / Sets the Own bit field of the rx descriptor.           */
#define RX_DESC_GET_OWN_BIT(rxDesc) (((rxDesc)->word1) >> 31)

/* Returns the First bit field of the rx descriptor.                */
#define RX_DESC_GET_FIRST_BIT(rxDesc) ((((rxDesc)->word1) >> 27) & 0x1)

/* Returns the Last bit field of the rx descriptor.                 */
#define RX_DESC_GET_LAST_BIT(rxDesc) ((((rxDesc)->word1) >> 26) & 0x1)

/* Returns the Resource error bit field of the rx descriptor.       */
#define RX_DESC_GET_REC_ERR_BIT(rxDesc) ((((rxDesc)->word1) >> 28) & 0x1)

/* Return the buffer size field from the second word of an Rx desc. */
/* Make sure to set the lower 3 bits to 0.                          */
#define RX_DESC_GET_BUFF_SIZE_FIELD_MAC(rxDesc)             \
            (((((rxDesc)->word2) >> 3) & 0x7FF) << 3)
#define RX_DESC_SET_BUFF_SIZE_FIELD_MAC(devNum,rxDesc,val)         \
            (rxDesc)->word2 = CPSS_32BIT_LE((rxDesc)->word2);  \
            U32_SET_FIELD_MAC((rxDesc)->word2,0,14,(val));      \
            (rxDesc)->word2 = CPSS_32BIT_LE((rxDesc)->word2)

/* Return the byte count field from the second word of an Rx desc.  */
/* Make sure to set the lower 3 bits to 0.                          */
#define RX_DESC_GET_BYTE_COUNT_FIELD(rxDesc)        \
            ((((rxDesc)->word2) >> 16) & 0x3FFF)

/* Return the Src_Port field from the first word of an Rx desc.     */
#define RX_DESC_GET_SRC_PORT(rxDesc) ((GT_U8)((((rxDesc)->word1) >> 10) & 0x3F))

/* Return the Cpu Code field from the first word of an Rx desc.     */
#define RX_DESC_GET_CPU_CODE(rxDesc) ((GT_U8)((((rxDesc)->word1) >> 2) & 0xFF))

/* Return the src device field from the first word of an Rx desc.     */
#define RX_DESC_GET_SRC_DEV(rxDesc) ((GT_U8)((((rxDesc)->word1) >> 16) & 0x7F))

/* Return Input_Encap field from the first word of an Rx desc.     */
#define RX_DESC_GET_INP_ENCAPS(rxDesc) ((GT_U8)((((rxDesc)->word1) >> 23) & 0x7))

#endif /* #ifndef __gtCoreIntDefsh */

/*
 * Typedef: struct PRV_CPSS_RX_DESC_STC
 *
 * Description: Includes the PP Rx descriptor fields, to be used for handling
 *              received packets.
 *
 * Fields:
 *      word1           - First word of the Rx Descriptor.
 *      word2           - Second word of the Rx Descriptor.
 *      buffPointer     - The physical data-buffer address.
 *      nextDescPointer - The physical address of the next Rx descriptor.
 *
 */
typedef struct
{
    volatile GT_U32         word1;
    volatile GT_U32         word2;

    volatile GT_U32         buffPointer;
    volatile GT_U32         nextDescPointer;

}PRV_CPSS_RX_DESC_STC;



/*
 * typedef: struct PRV_CPSS_SW_RX_DESC_STC
 *
 * Description: Sw management Tx descriptor.
 *
 * Fields:
 *      rxDesc          - Points to the Rx descriptor representing this Sw Rx
 *                        desc.
 *      swNextDesc      - A pointer to the next descriptor in the linked-list.
 *      shadowRxDesc    - A shadow struct to hold the real descriptor data
 *                        after byte swapping to save non-cacheable memory
 *                        access.
 *
 */
typedef struct PRV_CPSS_SW_RX_DESC_STCT
{
    PRV_CPSS_RX_DESC_STC      *rxDesc;
    struct PRV_CPSS_SW_RX_DESC_STCT    *swNextDesc;

    PRV_CPSS_RX_DESC_STC      shadowRxDesc;
}PRV_CPSS_SW_RX_DESC_STC;


/*
 * Typedef: struct PRV_CPSS_RX_DESC_LIST_STC
 *
 * Description: The control block of a single Rx descriptors list.
 *
 * Fields:
 *
 *      swRxDescBlock   - Points to the beginning of SwRx descriptors block.
 *                        use to avoid Mem reallocation at hotSynch/Shutdown
 *                        operation.
 *      next2Return     - Points to the descriptor to which the next returned
 *                        buffer should be attached.
 *      next2Receive    - Points to the descriptor from which the next packet
 *                        will be fetched when an Rx interrupt is received.
 *                        (This actually points to the first desc. of the packet
 *                        in case of a multi desc. packet).
 *      freeDescNum     - Number of free descriptors in list.
 *      maxDescNum      - Maximal number descriptors in the list
 *      headerOffset    - Number of reserved bytes before each buffer, to be
 *                        kept for application and internal use.
 *      forbidQEn       - When set to GT_TRUE enabling the Rx SDMA on buffer
 *                        release is forbidden.
 */
typedef struct
{
    void                *swRxDescBlock;
    PRV_CPSS_SW_RX_DESC_STC   *next2Return;
    PRV_CPSS_SW_RX_DESC_STC   *next2Receive;

    GT_U32              freeDescNum;
    GT_U32              maxDescNum;
    GT_U32              headerOffset;
    GT_BOOL             forbidQEn;
}PRV_CPSS_RX_DESC_LIST_STC;


/************************   AU Related Definitions  ***************************/

/* support running from the PSS */
#ifndef __gtCoreIntDefsh

#define AU_BLOCK_ALIGN      (16)  /* In Bytes */
#define AU_DESC_SIZE        (16)  /* In Bytes */

/* ExMxPm devices uses 32 byte alignment between AU/FU messages.
  Also baseaddresses for AUQ and FUQ must be 32 byte alignment. */
#define AU_DESC_ALIGN_EXT_5       (32)  /* In Bytes */
#define AU_DESC_SIZE_EXT_5        (32)  /* In Bytes */

/* AU descriptor related macros.    */

/* Checks if the given desc. is not valid.       */
#define AU_DESC_IS_NOT_VALID(auMsgPtr)                                     \
            (((auMsgPtr)->word0 == 0) && ((auMsgPtr)->word1 == 0) &&   \
             ((auMsgPtr)->word2 == 0) && ((auMsgPtr)->word3 == 0))


/* Invalidates the given AU desc.               */
#define AU_DESC_RESET_MAC(auMsgPtr)        \
            (auMsgPtr)->word0 = 0;         \
            (auMsgPtr)->word1 = 0;         \
            (auMsgPtr)->word2 = 0;         \
            (auMsgPtr)->word3 = 0

/* swap the 4 words of au message */
#define AU_DESC_SWAP_MAC(origAuMsgPtr,swappedAuMsg)        \
            swappedAuMsg.word0 = CPSS_32BIT_LE((origAuMsgPtr)->word0); \
            swappedAuMsg.word1 = CPSS_32BIT_LE((origAuMsgPtr)->word1); \
            swappedAuMsg.word2 = CPSS_32BIT_LE((origAuMsgPtr)->word2); \
            swappedAuMsg.word3 = CPSS_32BIT_LE((origAuMsgPtr)->word3)



/* Checks if the given desc. is not valid.
  Used for Puma devices. The Puma device has AU message with bit#0 always == 0.
  In the init of AU queue and after reading of AU the CPSS writes 0xFFFF FFFF
  to the first word of AU. The PP changes bit#0 of the first to 0 when transfers
  AU to the queue. So the first word of valid AU is NOT equal to 0xFFFF FFFF */
#define AU_DESC_IS_NOT_VALID_EXT_5(auMsgPtr)  ((auMsgPtr)->word0 == 0xFFFFFFFF)

/* Checks if the given desc. is valid. */
#define AU_DESC_IS_VALID_EXT_5(auMsgPtr)  ((auMsgPtr)->word0 != 0xFFFFFFFF)

/* Invalidates the given AU desc. Extended for 5 words. */
#define AU_DESC_RESET_EXT_5(auMsgPtr)   (auMsgPtr)->word0 = 0xFFFFFFFF;


#endif /* #ifndef __gtCoreIntDefsh */

/*
 * Typedef: struct PRV_CPSS_AU_DESC_STC
 *
 * Description: Includes fields definitions of the Address Update messages sent
 *              to the CPU.
 *
 * Fields:
 *
 *      word0 - word#0 of AU descriptor
 *      word1 - word#1 of AU descriptor
 *      word2 - word#2 of AU descriptor
 *      word3 - word#3 of AU descriptor
 *
 */
typedef struct
{
    GT_U32  word0;
    GT_U32  word1;
    GT_U32  word2;
    GT_U32  word3;
}PRV_CPSS_AU_DESC_STC;

/*
 * Typedef: struct PRV_CPSS_AU_DESC_EXT_5_STC
 *
 * Description: Includes fields definitions of the Address Update messages sent
 *              to the CPU. Extended for 5 words.
 *
 * Fields:
 *
 *      word0 - word#0 of AU descriptor
 *      word1 - word#1 of AU descriptor
 *      word2 - word#2 of AU descriptor
 *      word3 - word#3 of AU descriptor
 *      word4 - word#4 of AU descriptor
 *      unusedWord5 - word#5 of AU/FU message with no data
 *      unusedWord6 - word#6 of AU/FU message with no data
 *      unusedWord7 - word#7 of AU/FU message with no data
 */
typedef struct
{
    GT_U32  word0;
    GT_U32  word1;
    GT_U32  word2;
    GT_U32  word3;
    GT_U32  word4;
    GT_U32  unusedWord5;
    GT_U32  unusedWord6;
    GT_U32  unusedWord7;
}PRV_CPSS_AU_DESC_EXT_5_STC;

/*
 * Typedef: struct PRV_CPSS_AU_DESC_EXT_8_STC
 *
 * Description: Includes fields definitions of the Address Update messages sent
 *              to the CPU. Extended for 8 words. Relevant to Lion2 devices.
 *
 * Fields:
 *  elem0  - AU/FU message data elem0 (word0-word3)
 *  elem1  - AU/FU message data elem1 (word4-word7)
 */
typedef struct
{
    PRV_CPSS_AU_DESC_STC elem0;
    PRV_CPSS_AU_DESC_STC elem1;
}PRV_CPSS_AU_DESC_EXT_8_STC;

/*
 * typedef: struct PRV_CPSS_AU_DESC_CTRL_STC
 *
 * Description: Address Update descriptors block control struct.
 *
 * Fields:
 *      blockAddr         - Address of the descs. block (Virtual Address)
 *      blockSize         - Size (in descs.) of the descs. block.
 *      currDescIdx       - Index of the next desc. to handle.
 *      auCtrlSem         - Semaphore for mutual exclusion.
 *      unreadCncCounters - relevant for DxCh3 and above devices.
 *                          The CNC block uploaded
 *                          to the same memory as FDB uploaded. The same 4 words
 *                          record that contains FDB FU record can contain two
 *                          CNC counters. The CNC block contains 2048 CNC counters
 *                          and uploaded to 1024 records.
 *                          This field is set to 2048 by an API triggering the CNC
 *                          block upload and decremented by API retrieves the CNC
 *                          counters from this Queue.
 *
 */
typedef struct
{
    GT_UINTPTR  blockAddr;
    GT_U32      blockSize;
    GT_U32      currDescIdx;
    GT_U32      unreadCncCounters;
}PRV_CPSS_AU_DESC_CTRL_STC;


/* support running from the PSS */
#ifndef __gtCoreIntDefsh

/* macro that define the alignment needed for Tx descriptor that given to the PP
relate to packets that the CPU send to the PP*/
#define TX_DESC_ALIGN       (16)  /* In bytes */
/* macro that define the Tx "short" buffer size that given to the PP ,
this buffer needed in cases that the PP need to insert data to the packet that
the application put it the Tx buffers
relate to packets that the CPU send to the PP*/
#define TX_SHORT_BUFF_SIZE  (8)   /* Bytes    */

/* Define the Tx "short" buffer size 16 bytes,
   for CPSS to insert up to 4 words DSA tag into packets sent from CPU.
   Used for eArch devices (Bobcat2, Caelum, Bobcat3) */
#define TX_SHORT_BUFF_SIZE_16_CNS  (16)   /* Bytes    */

/* macro that define the Tx Header size that given to the PP ,
this header is given beside the Tx Descriptor and the Buffers
relate to packets that the CPU send to the PP*/
#define TX_HEADER_SIZE      (16)  /* Bytes    */
/* macro that define the Tx descriptor size that given to the PP ,
relate to packets that the CPU send to the PP*/
#define TX_DESC_SIZE        (16)  /* In Bytes */

/************************************/
/* Tx descriptor related macros.    */
/************************************/

/* macro that define that CPU own the Tx descriptor --
the value in field "ownership bit" in the Tx descriptor in the PP
relate to packets that the CPU send to the PP */
#define TX_DESC_CPU_OWN     (0)
/* macro that define that DMA (PP) own the Tx descriptor --
the value in field "ownership bit" in the Tx descriptor in the PP
relate to packets that the CPU send to the PP */
#define TX_DESC_DMA_OWN     (1)


/* Resets the Tx descriptor's word1 & word2.                        */
#define TX_DESC_RESET_MAC(txDesc)                                           \
            (txDesc)->word1 = 0x0;                                      \
            (txDesc)->word2 = 0x0

/* Copy a tx descriptor from one struct to another      */
#define TX_DESC_COPY_MAC(dstDesc,srcDesc)                                   \
            (dstDesc)->buffPointer      = (srcDesc)->buffPointer;       \
            (dstDesc)->word2            = (srcDesc)->word2;             \
            (dstDesc)->word1            = (srcDesc)->word1;

/* Get / Set the Own bit field of the tx descriptor.    */
#define TX_DESC_GET_OWN_BIT_MAC(devNum,txDesc)                         \
            (CPSS_32BIT_LE((txDesc)->word1) >> 31)
#define TX_DESC_SET_OWN_BIT_MAC(txDesc,val)                     \
            (U32_SET_FIELD_MAC((txDesc)->word1,31,1,val))

/* Get / Set the First bit field of the tx descriptor.  */
#define TX_DESC_GET_FIRST_BIT_MAC(devNum,txDesc)                       \
            ((CPSS_32BIT_LE((txDesc)->word1) >> 21) &0x1)
#define TX_DESC_SET_FIRST_BIT_MAC(txDesc,val)                   \
            (U32_SET_FIELD_MAC((txDesc)->word1,21,1,val))


/* Get / Set the Last bit field of the tx descriptor.   */
#define TX_DESC_GET_LAST_BIT_MAC(devNum,txDesc)                        \
            ((CPSS_32BIT_LE((txDesc)->word1)>> 20) & 0x1)
#define TX_DESC_SET_LAST_BIT_MAC(txDesc,val)                    \
            (U32_SET_FIELD_MAC((txDesc)->word1,20,1,val))


/* Get / Set the Reject indication bit field of the tx descriptor.  */
#define TX_DESC_GET_REJECT_BIT_MAC(txDesc)                  \
            (((txDesc)->word1 >> 0) & 0x1)
#define TX_DESC_SET_REJECT_BIT_MAC(txDesc,val)              \
            (U32_SET_FIELD_MAC((txDesc)->word1,0,1,val))

/* Get / Set the Byte Count field in the tx descriptor.       */
#define TX_DESC_GET_BYTE_CNT_MAC(txDesc)            \
            (U32_GET_FIELD_MAC((CPSS_32BIT_LE((txDesc)->word2)),16,14))

#define TX_DESC_SET_BYTE_CNT_MAC(txDesc,val)            \
            (U32_SET_FIELD_MAC((txDesc)->word2,16,14,val))

/* Set the Vid field in the tx descriptor.              */
#define TX_DESC_SET_VID_MAC(txDesc,val)            \
            (U32_SET_FIELD_MAC((txDesc)->word1,0,12,val))

/* Get / Set the Int bit field of the tx descriptor.    */
#define TX_DESC_GET_INT_BIT(txDesc)     (((txDesc)->word1 >> 23) & 0x1)
#define TX_DESC_SET_INT_BIT_MAC(txDesc,val) \
            (U32_SET_FIELD_MAC((txDesc)->word1,23,1,val))

#endif /* #ifndef __gtCoreIntDefsh */

/*
 * Typedef: struct PRV_CPSS_TX_DESC_STC
 *
 * Description: Includes the PP Tx descriptor fields, to be used for handling
 *              packet transmits.
 *
 * Fields:
 *      word1           - First word of the Tx Descriptor.
 *      word2           - Second word of the Tx Descriptor.
 *      buffPointer     - The physical data-buffer address.
 *      nextDescPointer - The physical address of the next Tx descriptor.
 *
 */
typedef struct
{
    volatile GT_U32         word1;
    volatile GT_U32         word2;

    volatile GT_U32         buffPointer;
    volatile GT_U32         nextDescPointer;
}PRV_CPSS_TX_DESC_STC;





/*
 * typedef: struct PRV_CPSS_SW_TX_DESC_STC
 *
 * Description: Sw management Tx descriptor.
 *
 * Fields:
 *      txDesc          - Points to the Tx descriptor representing this Sw Tx
 *                        desc.
 *      swNextDesc      - A pointer to the next descriptor in the linked-list.
 *      txHeader        - A pointer to the tx header.
 *      shortBuffer     - A pointer to a 8 bytes buffer to be used when
 *                        transmitting buffers of size <= 8.
 *      userData        - A data to be stored in gtBuf on packet transmit
 *                        request, and returned to user on TxBufferQueue.
 *      evReqHndl       - The user event handle got from gtEventBind when
 *                        working in Event Request Driven mode.
 *
 */
typedef struct PRV_CPSS_SW_TX_DESC_STCT
{
    PRV_CPSS_TX_DESC_STC      *txDesc;
    struct PRV_CPSS_SW_TX_DESC_STCT    *swNextDesc;
    GT_U8               *txHeader;
    GT_U8               *shortBuffer;
    GT_PTR              userData;
    GT_UINTPTR          evReqHndl;
}PRV_CPSS_SW_TX_DESC_STC;



/*
 * Typedef: struct PRV_CPSS_TX_DESC_LIST_STC
 *
 * Description: The control block of a single Tx descriptors list.
 *
 * Fields:
 *      swTxDescBlock   - Points to the beginning of SwTx descriptors block.
 *                        use to avoid Mem reallocation at hotSynch/Shutdown
 *                        operations.
 *      poolId          - The Tx End FIFO pool used when working in Event-Request
 *                        -Driven mode.
 *      next2Free       - Points to the descriptor from which the next
 *                        transmitted buffer should be freed, When receiving
 *                        a TxBufferQueue interrupt.
 *      next2Feed       - Points to the descriptor to which the next transmitted
 *                        packet should be attached.
 *                        (This actually points to the first desc. of the packet
 *                        in case of a multi desc. packet).
 *      freeDescNum     - Number of free descriptors in list.
 *      maxDescNum      - maximum number of descriptors in list.
 *      txListSem       - Semaphore for mutual exclusion on the access to the Tx
 *                        descriptors list.
 *      freeCpssBuff    - Whether to free the tx core buffer,  this is true
 *                        whenever a packet is transferred to the CPSS with the
 *                        bufCopy on (parameter of coreGetTxPacket()).
 *      actualBuffSize  - the real buffer size due to descriptor alignment
 *                        limitations. 
 *                        Relevant only to queues working as Packet Generator.
 *      firstDescPtr    - (pointer to) the first descriptor place in the chain.
 *                        Relevant only to queues working as Packet Generator.
 *      freeDescPtr     - (pointer to) the next to use descriptor in the chain.
 *                        When the chain is empty, the first and free point to
 *                        the same place.
 *                        Relevant only to queues working as Packet Generator.
 *      scratchPadPtr   - (pointer to) the scratch pad descriptor (used during 
 *                        descriptors and buffers manipulations for update and
 *                        removal).
 *                        Relevant only to queues working as Packet Generator.
 *      revPacketIdDb   - (pointer to) per queue DB to allocate packet ID during
 *                        removal operation.
 *                        Relevant only to queues working as Packet Generator.
 *      userQueueEnabled - Indication for the last queue enable or disable 
 *                         operation requested by the application.
 */
typedef struct
{
    void                        *swTxDescBlock;
    CPSS_BM_POOL_ID             poolId;

    PRV_CPSS_SW_TX_DESC_STC     *next2Free;
    PRV_CPSS_SW_TX_DESC_STC     *next2Feed;

    GT_U32                      freeDescNum;
    GT_U32                      maxDescNum;

    GT_BOOL                     freeCpssBuff;

    GT_U32                      actualBuffSize;
    PRV_CPSS_TX_DESC_STC        *firstDescPtr;
    PRV_CPSS_TX_DESC_STC        *freeDescPtr;
    PRV_CPSS_TX_DESC_STC        *scratchPadPtr;

    GT_U32                      *revPacketIdDb;
    GT_BOOL                     userQueueEnabled;

}PRV_CPSS_TX_DESC_LIST_STC;



/*
 * typedef: enum PRV_CPSS_AUQ_STATE_ENT
 *
 * Description:
 *      enumeration for the states of AU queues.
 *      Relevant for WA:
 *          PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E
 *
 *
 * Enumerations:
 *      PRV_CPSS_AUQ_STATE_ALL_FULL_E -
 *                              all AU queues are full
 *      PRV_CPSS_AUQ_STATE_ONE_EMPTY_E -
 *                              one of two AU queues is empty
 *      PRV_CPSS_AUQ_STATE_ALL_EMPTY_E -
 *                              all AU queues are empty
 */
typedef enum
{
    PRV_CPSS_AUQ_STATE_ALL_FULL_E,
    PRV_CPSS_AUQ_STATE_ONE_EMPTY_E,
    PRV_CPSS_AUQ_STATE_ALL_EMPTY_E
}PRV_CPSS_AUQ_STATE_ENT;

/****************   General Interrupts Control Definitions  *******************/


/*
 * Typedef: struct PRV_CPSS_INTERRUPT_CTRL_STC
 *
 * Description: Includes all needed definitions for interrupts handling
 *              in port group level. (Rx, Tx, Address Updates,...).
 *
 * Fields:
 *      rxDescList  - A list of Rx_Descriptor_List control structs, one for each
 *                    Rx queue.
 *      txDescList  - A list of Tx_Descriptor_List control structs, one for each
 *                    Tx queue.
 *      auDescCtrl[]  - Array of : Control block of the AU desc.
 *                      index in the array is the 'portGroupId':
 *                      for 'non multi-port-groups' device use index 0.
 *                      for 'multi-port-groups' device use portGroupId as index.
 *      fuDescCtrl[]  - Array of : Control block of the FU desc.
 *                      index in the array is the 'portGroupId':
 *                      for 'non multi-port-groups' device use index 0.
 *                      for 'multi-port-groups' device use portGroupId as index.
 *      secondaryAuDescCtrl - Array of : Control block of the secondary AU desc.
 *                      index in the array is the 'portGroupId':
 *                      for 'non multi-port-groups' device use index 0.
 *                      for 'multi-port-groups' device use portGroupId as index.
 *
 *      auqDeadLockWa[] - stuct to hold parameters relate to the WA for
 *              PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E
 *                      index in the array is the 'portGroupId':
 *                      for 'non multi-port-groups' device use index 0.
 *                      for 'multi-port-groups' device use portGroupId as index.
 *          needToCheckSecondaryAuQueue - indication that we need/no need to get
 *                  AU messages from the secondary AUQ.
 *          auqRewind - indication that the primary AUQ was rewind since last
 *                      time that it was queried
 *                      GT_TRUE - AUQ was rewind since last call
 *                      GT_FALSE - AUQ wasn't rewind since last call
 *          activated - was this port group activated for the last WA.
 *          needWaWaitForReTriggering -
 *                      (this flag ignored for non-multi port group device ,
 *                       -- ExMxPm code not need it and DxCh ignor for non multi-port groups device)
 *                      indication that this port group started the WA in function
 *                      cpssDxChBrgFdbTriggerAuQueueWa and already cleared the
 *                      'trigger action' bit in HW , so the functions that re-trigger
 *                      the action (cpssDxChBrgFdbMacTriggerToggle , cpssDxChBrgFdbTrigActionStart)
 *                      will know if current port group need to re-trigger the action or not.
 *                      (this flag ignored for non-multi port group device ,
 *                       -- ExMxPm code not need it and DxCh ignor for non multi-port groups device)
 *          notReady - this port group not finished the trigger action but it
 *                     is not ready for for WA.
 *                     when the port group triggered action but not finished
 *                     when the AUQ is not full , the WA not need to start.
 *                     so this is indication to skip the port group when checking
 *                     if new trigger can start after starting the WA (due to
 *                     other port groups)
 *                     this flag is set/cleared in function cpssDxChBrgFdbTriggerAuQueueWa
 *                     this flag is used by cpssDxChBrgFdbTrigActionStart
 *                     this flag is MAYBE cleared by cpssDxChBrgFdbTrigActionStart
 *                     (ExMxPm code -- this flag ignored)
 *          activeSecondaryAuqIndex
 *                   - 0 - secondary AUQ is currently active queue; 1 - secondary AUQ1 is currently active queue;
 *          secondaryState
 *                   - enumeration for the states of secondary AU queues.
 *          primaryState
 *                   - enumeration for the states of primary AU queues.
 *
 *    activeAuqIndex - 0 - primary AUQ is currently active queue; 1 - primary AUQ1 is currently active queue.
 *    au1DescCtrl[]  - Array of : Control block of the AU1 desc.
 *                     index in the array is the 'portGroupId':
 *                     for 'non multi-port-groups' device use index 0.
 *                     for 'multi-port-groups' device use portGroupId as index.
 *    secondaryAu1DescCtrl - Array of : Control block of the secondary AU1 desc.
 *                     index in the array is the 'portGroupId':
 *                     for 'non multi-port-groups' device use index 0.
 *                     for 'multi-port-groups' device use portGroupId as index.
 *    txGeneratorPacketIdDb - structure to hold packet ID DB for packet generator:
 *          cookie - (pointer to) database correlating packet ID in
 *                   Packet Generator mode to allocation place in queue.
 *          freeLinkedList - (pointer to) database linked list of free packets ID.
 *          firstFree - the first (in a linked list) free packet ID. 
 */
typedef struct
{
    PRV_CPSS_RX_DESC_LIST_STC    rxDescList[NUM_OF_RX_QUEUES];
    PRV_CPSS_TX_DESC_LIST_STC    txDescList[NUM_OF_TX_QUEUES];
    PRV_CPSS_AU_DESC_CTRL_STC    auDescCtrl[CPSS_MAX_PORT_GROUPS_CNS];
    PRV_CPSS_AU_DESC_CTRL_STC    fuDescCtrl[CPSS_MAX_PORT_GROUPS_CNS];
    PRV_CPSS_AU_DESC_CTRL_STC    secondaryAuDescCtrl[CPSS_MAX_PORT_GROUPS_CNS];
    struct{
        GT_BOOL   needToCheckSecondaryAuQueue;
        GT_BOOL   auqRewind;

        GT_BOOL   activated;
        GT_BOOL   needWaWaitForReTriggering;
        GT_BOOL   notReady;
        GT_U32    activeSecondaryAuqIndex;
        PRV_CPSS_AUQ_STATE_ENT secondaryState;
        PRV_CPSS_AUQ_STATE_ENT primaryState;
    }auqDeadLockWa[CPSS_MAX_PORT_GROUPS_CNS];
    GT_U32  activeAuqIndex[CPSS_MAX_PORT_GROUPS_CNS];
    PRV_CPSS_AU_DESC_CTRL_STC    au1DescCtrl[CPSS_MAX_PORT_GROUPS_CNS];
    PRV_CPSS_AU_DESC_CTRL_STC    secondaryAu1DescCtrl[CPSS_MAX_PORT_GROUPS_CNS];
    struct{
        GT_U32      *cookie;
        GT_U32      *freeLinkedList;
        GT_U32      firstFree;
        GT_U32      generatorsTotalDesc;
    }txGeneratorPacketIdDb;

}PRV_CPSS_INTERRUPT_CTRL_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenIntDefsh */


