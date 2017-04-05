/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenDragonite.h
*
* DESCRIPTION:
*       Private definitions for CPSS DRAGONITE implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*******************************************************************************/
#ifndef __prvCpssGenDragoniteh
#define __prvCpssGenDragoniteh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/dragonite/cpssGenDragonite.h>

#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>

#define PRV_CPSS_DRAGONITE_MAX_DATA_SIZE_CNS 2304 /* maximum size of data in bytes */
#define PRV_CPSS_DRAGONITE_MAX_IRQ_MEM_SIZE_CNS 12 /* maximum size of irq memory in bytes */

/* communication structures length in words */
#define PRV_CPSS_DRAGONITE_CONFIG_STRUCT_LENGTH_CNS 107
#define PRV_CPSS_DRAGONITE_SYSTEM_STRUCT_LENGTH_CNS 43
#define PRV_CPSS_DRAGONITE_CHIP_STRUCT_LENGTH_CNS 71
#define PRV_CPSS_DRAGONITE_PORT_CMD_STRUCT_LENGTH_CNS 288
#define PRV_CPSS_DRAGONITE_PORT_CNTR_STRUCT_LENGTH_CNS 576
#define PRV_CPSS_DRAGONITE_PORT_STATUS_STRUCT_LENGTH_CNS 288
#define PRV_CPSS_DRAGONITE_PORT_MEASURE_STRUCT_LENGTH_CNS 384
#define PRV_CPSS_DRAGONITE_PORT_LAYER2_STRUCT_LENGTH_CNS 192
#define PRV_CPSS_DRAGONITE_DEBUG_STRUCT_LENGTH_CNS 1
#define PRV_CPSS_DRAGONITE_OTHER_STRUCT_LENGTH_CNS 1

/*
 * Error codes DRAGONITE controller returns by communication protocol:
 * bit 0 – Check Sum Error.
 * bit 1 – Length Error.
 * bit 2 – Structure version mismatch.
 * bit 3 – Unknown Type. When this bit is set, there will be no data structure
 *          returned, only the header.
 * bit 4 – Out of range value in the structure data.
 *          The DRAGONITE manager will fill the data in the structure with zeros as long as the
 *          received data was in range. In case of data out of range, the received out of
 *          range data will be returned.
 * bit 5 – Config is not allowed.
*/
#define PRV_CPSS_DRAGONITE_ERROR_OK_CNS               0
#define PRV_CPSS_DRAGONITE_ERROR_CHECKSUM_CNS         BIT_0
#define PRV_CPSS_DRAGONITE_ERROR_BAD_SIZE_CNS         BIT_1
#define PRV_CPSS_DRAGONITE_ERROR_VERSION_MISMATCH_CNS BIT_2
#define PRV_CPSS_DRAGONITE_ERROR_UNKNOWN_TYPE_CNS     BIT_3
#define PRV_CPSS_DRAGONITE_ERROR_OUT_OF_RANGE_CNS     BIT_4
#define PRV_CPSS_DRAGONITE_ERROR_BAD_STATE_CNS        BIT_5

/*
 * typedef: struct PRV_CPSS_DRAGONITE_COMM_MSG_STRUCT_VALID_STC
 *
 * Description: structure holds internal info per dragonite structure type
 *
 * Fields:
 *   structureLength - length of current structure type
 *   structureVersion - currently supported structure version
 *   pt2DecodeFunc - pointer to function translating structure HW format to SW
 *   pt2EncodeFunc - pointer to function translating structure SW format to HW
 *                   (for writable structures only)
 *
 * Note:
 *   None
 */
typedef struct
{
    GT_U32  structureLength;
    GT_U32  structureVersion;
    GT_VOID (*pt2DecodeFunc)(OUT CPSS_GEN_DRAGONITE_DATA_STC *);
    GT_STATUS (*pt2EncodeFunc)(IN CPSS_GEN_DRAGONITE_DATA_STC *);

} PRV_CPSS_DRAGONITE_COMM_MSG_STRUCT_VALID_STC;

/* interrupts management definitions */

/*
 * typedef: struct PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC
 *
 * Description: irq shared memory structure
 *
 * Fields:
 *   irqCause - dragonite interrupt cause register
 *   irqMask  - dragonite interrupt mask register
 *   irqFlag  - dragonite interrupt flag bit
 *
 * Note:
 *   None
 */
typedef struct
{
    GT_U32 irqCause; /* IMO_flag bit 31 */
    GT_U32 irqMask;  /* irqPolarity bit 31 */
    GT_U32 irqFlag;  /* irqFlag bit 0, the rest not in use for now */
} PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC;

 
/*
 * Typedef: enum PRV_CPSS_GEN_DRAGONITE_IRQ_TYPE_ENT
 *
 * Description:
 *      This enum defines possible interrupts types - used to define bitmap of 
 *                             occurred interrupts and interrupts mask
 * Fields:
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_ON_E    - Port On
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_OFF_E   - Port Off
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_DETECT_FAIL_E -  Detection Failed
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_FAULT_E -   OVL or SC or thermal
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_UDL_E - Port UDL
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_FAULT_ON_STARTUP_E - OVL or SC during startup or DvDt fail
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_PM_E -  port turned off due to power management
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_POWER_DENIED_E - port power denied at startup
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_SPARE0_E - Port_Spare0
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_SPARE1_E -  Port_Spare1
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_OVER_TEMP_E -    OverTemp
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_TEMP_ALARM_E -   TempAlarm
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_DEVICE_FAULT_E - When any DRAGONITE device stops communicating
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_DEVICE_SPARE0_E -    Device_Spare0
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_DEVICE_SPARE1_E -    Device_Spare1
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_OVER_CONSUM_E -  When power consumption is above power limit (Static or
 *                                                                                           Dynamic)
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_LOW_AF_E - VmainLowAF
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_LOW_AT_E - VmainLowAT
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_HIGH_E -   VmainHigh
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_SPARE0_E -    System_Spare0
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_SPARE1_E -    System_Spare1
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_READ_EVENT_E -   IRQ_Read_Event
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED0_E    - reserved
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED1_E    - reserved
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED2_E    - reserved
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_WRITE_EVENT_E -  IRQ_Write_Event
 *      PRV_CPSS_GEN_DRAGONITE_IRQ_MAX_E - for generic implementation purposes
 *
 */
typedef enum
{
    PRV_CPSS_GEN_DRAGONITE_IRQ_MIN_E = 0,
    PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_ON_E = ( PRV_CPSS_GEN_DRAGONITE_IRQ_MIN_E ),
    PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_OFF_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_DETECT_FAIL_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_FAULT_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_UDL_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_FAULT_ON_STARTUP_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_PM_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_POWER_DENIED_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_SPARE0_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_SPARE1_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_OVER_TEMP_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_TEMP_ALARM_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_DEVICE_FAULT_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_DEVICE_SPARE0_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_DEVICE_SPARE1_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_OVER_CONSUM_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_LOW_AF_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_LOW_AT_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_HIGH_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_SPARE0_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_SPARE1_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_READ_EVENT_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED0_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED1_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED2_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_WRITE_EVENT_E,
    PRV_CPSS_GEN_DRAGONITE_IRQ_MAX_E = PRV_CPSS_GEN_DRAGONITE_IRQ_WRITE_EVENT_E
} PRV_CPSS_GEN_DRAGONITE_IRQ_TYPE_ENT;

/*
 * typedef: struct PRV_CPSS_DRAGONITE_INTERRUPT_CTRL_STC
 *
 * Description: dragonite interrupt control structure
 *
 * Fields:
 *   intNodesPool - data on received interrupts
 *   isrFuncPtr   - Isr function pointer to be called on interrupt reception.
 *   intScanRoot  - data for accessing interrupt register
 *   intMaskShadow - Interrupt mask registers shadow array
 *   intVecNum     - dragonite interrupt vector number
 *   intMask       - dragonite interrupt mask
 *   isrConnectionId - Id to be used for future access to the connected isr.
 *
 * Note:
 *   None
 */
typedef struct
{
    PRV_CPSS_DRV_EV_REQ_NODE_STC      *intNodesPool;
    CPSS_EVENT_ISR_FUNC               isrFuncPtr;
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC   *intScanRoot;

    GT_U32              *intMaskShadow;
    GT_U32              intVecNum;
    GT_UINTPTR          intMask;
    GT_U32              isrConnectionId;

}PRV_CPSS_DRAGONITE_INTERRUPT_CTRL_STC;

#define PRV_CPSS_DRAGONITE_COMM_MSG_MEM_OFFSET_CNS 0x100 /* in words of 32 bits */
#define PRV_CPSS_DRAGONITE_HEADER_SIZE_FOR_CHECKSUM_CNS  (5<<2) /* five words of header without checksum */
#define PRV_CPSS_DRAGONITE_DATA_OFFSET_CNS  6 /* comm message data offset in words */

extern GT_U32 *commMsgPtr; /* pointer to start of communication message shared memory */
extern GT_U32 *irqMemPtr;  /* pointer to start of interrupt shared memory */

/* CPU is working in big-endian mode now */
extern GT_BOOL bigEndian;

/* macro read word from shared memory swapped if in big-endian mode */
#define PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(data) (bigEndian ? BYTESWAP_MAC(data) : data)
/* macro set word in shared memory swapped if in big-endian mode */
#define PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(reg, data) reg = (bigEndian ? BYTESWAP_MAC(data) : data)
/* macro read checksum from comm. msg header */
#define PRV_CPSS_DRAGONITE_HDR_CHECKSUM_GET_MAC      PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[0])
/* macro set checksum in comm. msg header */
#define PRV_CPSS_DRAGONITE_HDR_CHECKSUM_SET_MAC(val) PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(commMsgPtr[0], val)
/* macro read memory owner bit from comm. msg header */
#define PRV_CPSS_DRAGONITE_MO_FLAG_GET_MAC         U32_GET_FIELD_MAC(PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[1]), 31, 1)
/* return memory ownership to dragonite - kirkwood can execute just this action on this bit */
#define PRV_CPSS_DRAGONITE_MO_FLAG_DRAGONITE_SET_MAC                \
{                                                                   \
    GT_U32 comMsgHdr;                                               \
    comMsgHdr = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[1]);  \
    U32_SET_FIELD_MAC(comMsgHdr, 31, 1, 1);                         \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(commMsgPtr[1], comMsgHdr);   \
}
/* macro set read/write flag in comm. msg header to write operation */
#define PRV_CPSS_DRAGONITE_RW_FLAG_WRITE_SET_MAC                    \
{                                                                   \
    GT_U32 comMsgHdr;                                               \
    comMsgHdr = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[1]);  \
    U32_SET_FIELD_MAC(comMsgHdr, 30, 1, 1);                         \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(commMsgPtr[1], comMsgHdr);   \
}
/* macro set read/write flag in comm. msg header to read operation */
#define PRV_CPSS_DRAGONITE_RW_FLAG_READ_SET_MAC                     \
{                                                                   \
    GT_U32 comMsgHdr;                                               \
    comMsgHdr = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[1]);  \
    U32_SET_FIELD_MAC(comMsgHdr, 30, 1, 0);                         \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(commMsgPtr[1], comMsgHdr);   \
}
/* macro read message type field from comm. msg header */
#define PRV_CPSS_DRAGONITE_MSG_TYPE_GET_MAC U32_GET_FIELD_MAC(PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[1]), 0, 30)
/* macro set message type field in comm. msg header */
#define PRV_CPSS_DRAGONITE_MSG_TYPE_SET_MAC(val)                    \
{                                                                   \
    GT_U32 comMsgHdr;                                               \
    comMsgHdr = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[1]);  \
    U32_SET_FIELD_MAC(comMsgHdr, 0, 30, val);                       \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(commMsgPtr[1], comMsgHdr);   \
}
/* macro read structure length field from comm. msg header */
#define PRV_CPSS_DRAGONITE_LENGTH_GET_MAC PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[2])
/* macro set structure length field in comm. msg header */
#define PRV_CPSS_DRAGONITE_LENGTH_SET_MAC(val) PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(commMsgPtr[2], val)
/* macro read structure version field from comm. msg header */
#define PRV_CPSS_DRAGONITE_VERSION_GET_MAC PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[3])
/* macro set structure version field in comm. msg header */
#define PRV_CPSS_DRAGONITE_VERSION_SET_MAC(val) PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(commMsgPtr[3], val)
/* macro read error vector field from comm. msg header */
#define PRV_CPSS_DRAGONITE_ERROR_VECTOR_GET_MAC PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(commMsgPtr[4])
/* kirkwood doesn't sends to dragonite error messages */
#define PRV_CPSS_DRAGONITE_ERROR_VECTOR_CLEAR_MAC  commMsgPtr[4] = 0
/* macro pointer to start of communication message data part */
#define PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC (&commMsgPtr[PRV_CPSS_DRAGONITE_DATA_OFFSET_CNS])
/* macro pointer to start of communication message without checksum field */
#define PRV_CPSS_DRAGONITE_MSG_TYPE_LONG_PTR_MAC (&commMsgPtr[1])

/* macro read/write write finished interrupt flag in dragonite interrupt memory */
#define PRV_CPSS_DRAGONITE_IRQ_WRITE_FLAG_GET_MAC(irqMemPtr)   \
            U32_GET_FIELD_MAC(PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause), 25, 1)
#define PRV_CPSS_DRAGONITE_IRQ_WRITE_FLAG_SET_MAC(irqMemPtr, val)              \
{                                                                       \
    GT_U32 irqCauseTmp;                                                 \
    irqCauseTmp = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause); \
    U32_SET_FIELD_MAC(irqCauseTmp, 25, 1, val);                         \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause, irqCauseTmp);  \
}

/* macro read/write read data ready interrupt flag in dragonite interrupt memory */
#define PRV_CPSS_DRAGONITE_IRQ_READ_FLAG_GET_MAC(irqMemPtr)    \
            U32_GET_FIELD_MAC(PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause), 21, 1)
#define PRV_CPSS_DRAGONITE_IRQ_READ_FLAG_SET_MAC(irqMemPtr, val)               \
{                                                                       \
    GT_U32 irqCauseTmp;                                                 \
    irqCauseTmp = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause); \
    U32_SET_FIELD_MAC(irqCauseTmp, 21, 1, val);                         \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause, irqCauseTmp);  \
}

/* macro to read/write interrupt memory owner flag */
#define PRV_CPSS_DRAGONITE_IRQ_IMO_FLAG_GET_MAC(irqMemPtr) \
            U32_GET_FIELD_MAC(PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause), 31, 1)
#define PRV_CPSS_DRAGONITE_IRQ_IMO_FLAG_SET_MAC(irqMemPtr, val)                \
{                                                                       \
    GT_U32 irqCauseTmp;                                                 \
    irqCauseTmp = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause); \
    U32_SET_FIELD_MAC(irqCauseTmp, 31, 1, val);                         \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause, irqCauseTmp);  \
}

/* macro to read/write dragonite interrupt mask register */
#define PRV_CPSS_DRAGONITE_IRQ_MASK_GET_MAC(irqMemPtr) \
            U32_GET_FIELD_MAC(PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqMask), 0, 31)
#define PRV_CPSS_DRAGONITE_IRQ_MASK_SET_MAC(irqMemPtr, val)                    \
{                                                                       \
    GT_U32 irqMaskTmp;                                                  \
    irqMaskTmp = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqMask);   \
    U32_SET_FIELD_MAC(irqMaskTmp, 0, 30, val);                          \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqMask, irqMaskTmp);    \
}

/* macro to read/write dragonite interrupt mask register */
#define PRV_CPSS_DRAGONITE_IRQ_CAUSE_BITS_GET_MAC(irqMemPtr)   \
            U32_GET_FIELD_MAC(PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause), 0, 31)
#define PRV_CPSS_DRAGONITE_IRQ_CAUSE_BITS_SET_MAC(irqMemPtr, val)                \
{                                                                       \
    GT_U32 irqCauseTmp;                                                 \
    irqCauseTmp = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause); \
    U32_SET_FIELD_MAC(irqCauseTmp, 0, 30, val);                         \
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause, irqCauseTmp);  \
}

/* macro to read interrupt flag bit from dragonite interrupt memory
 * Note: irqFlag is RO for host */
#define PRV_CPSS_DRAGONITE_IRQ_INTERRUPT_FLAG_GET_MAC(irqMemPtr)   \
            U32_GET_FIELD_MAC(PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqFlag), 0, 1)

/*******************************************************************************
* dragoniteInterruptsInit
*
* DESCRIPTION:
*       Initialize the DRAGONITE interrupts mechanism
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*    intVectNum - DRAGONITE interrupt vector number
*    intMask    - DRAGONITE interrupt mask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dragoniteInterruptsInit
(
    IN GT_U32 intVectNum,
    IN GT_U32 intMask
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenDragoniteh */

