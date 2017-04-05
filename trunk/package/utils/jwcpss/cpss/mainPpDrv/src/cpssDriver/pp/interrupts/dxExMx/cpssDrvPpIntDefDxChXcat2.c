/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefDxChXcat2.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- DxCh xCat2 devices
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsDxChXcat2.h>

/* set values that will be updated/filled during runtime (initialization stage)*/
#define FILLED_IN_RUNTIME_CNS       0

/* interrupt register for the GE MAC port */
#define GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) (0x0A800020  + ((port) * 0x400))

/* GE MAC port leaf */
#define GE_MAC_PORT_INTERRUPT_LEAF_MAC(port)                             \
            { ((port) + 1) ,GT_FALSE, 0, NULL,                           \
            GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port),                       \
            (GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) + 4),                 \
             prvCpssDrvHwPpPortGroupIsrRead,                             \
             prvCpssDrvHwPpPortGroupIsrWrite,                            \
             (((port) * 32) + PRV_CPSS_XCAT2_PORTS_NETW_0_LINK_STATUS_CHANGED_E),  \
             (((port) * 32) + PRV_CPSS_XCAT2_PORTS_NETW_0_MIB_COUNTERS_WRAPAROUND_E),     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* stacking port -- GE MAC port leaf */
#define STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(port) \
            { (port) ,GT_FALSE,0, NULL,                               \
             GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port),                   \
             (GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) + 4),             \
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((((port) - 24) * 32) + PRV_CPSS_XCAT2_PORTS_STACK_24_LINK_STATUS_CHANGED_E),     \
             ((((port) - 24) * 32) + PRV_CPSS_XCAT2_PORTS_STACK_24_MIB_COUNTERS_WRAPAROUND_E), \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}



/*
 * Typedef: struct INTERRUPT_SCAN
 *
 * Description: Basic struct for accessing interrupt register in hierarchy
 *              tree.
 *
 * Fields:
 *      bitNum          - Sum bit num in upper hierarchy, representing this
 *                        register.
 *      isGpp           - Is hierarchy connected to another device.
 *      gppId           - The gpp Id represented by this node (Valid only if
 *                        isGpp is GT_TRUE).
 *      gppFuncPtr      - Pointer to isrFunc to be called if isGPP == GT_TRUE.
 *      devNum          - Device number for isrFunc Call.
 *      causeRegAddr    - Address of the interrupt cause register to scan.
 *      maskRegAddr     - Address of the interrupt mask register to update after
 *                        scanning.
 *      pRegReadFunc    - A pointer to a function to be used for reading the
 *                        interrupt cause register.
 *      pRegWriteFunc   - A pointer to a function to be used for writing the
 *                        interrupt mask register.
 *      startIdx        - The start interrupt index representing the interrupts
 *                        to be scanned.
 *      endIdx          - The end interrupt index representing the interrupts to
 *                        be scanned.
 *      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 *      subIntrListLen  - List length sub interrupts list.
 *      subIntrScan     - pointer to sum bit interrupt register structure.
 *      nextIntrScan    - pointer to next interrupt struct in hierarchy.
 */

static PRV_CPSS_DRV_INTERRUPT_SCAN_STC xcat2IntrScanArr[] =
{
    /* Global Interrupt Cause Register [0x00000030] */
    {0, GT_FALSE,0, NULL, 0x30, 0x34,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_GLOBAL_CNC_SUM_E,
        PRV_CPSS_XCAT2_GLOBAL_HA_HEP_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0x0, 23, NULL, NULL},


    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_CNC_SUM_E */
    /* CNC Interrupt Cause Register [0x08000100] */
    {3,GT_FALSE,0, NULL, 0x08000100, 0x08000104,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_CNC_MAIN_FUNC_SUM_E,
        PRV_CPSS_XCAT2_CNC_MAIN_DATA_ERR_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_CNC_MAIN_FUNC_SUM_E */
    /* CNC Functional Interrupt Cause Register [0x08000108] */
    {1,GT_FALSE,0, NULL, 0x08000108, 0x0800010C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_CNC_BLOCK0_WRAPAROUND_E,
        PRV_CPSS_XCAT2_CNC_DUMP_FINISHED_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_CNC_MAIN_DATA_ERR_SUM_E */
    /* CNC Data Error Interrupt Cause [0x08000110] */
    {2,GT_FALSE,0, NULL, 0x08000110, 0x08000114,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_CNC_DATA_ERR_INTRRUPT0_E,
        PRV_CPSS_XCAT2_CNC_DATA_ERR_INTRRUPT7_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_TTI_SUM_E */
    /* TTI Unit Interrupt Summary Cause [0x0C0001C0] */
    {4,GT_FALSE,0, NULL, 0x0C0001C0, 0x0C0001C4,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TTI_MAIN_TTI_ENGINE_SUM_E,
        PRV_CPSS_XCAT2_TTI_MAIN_ITS_ENGINE_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_TTI_MAIN_TTI_ENGINE_SUM_E */
    /* TTI_Int_Sum address[0x0C000004] */
    {1,GT_FALSE,0, NULL, 0x0C000004, 0x0C000008,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TTI_CPU_ADDRESS_OUT_OF_RANGE_E,
        PRV_CPSS_XCAT2_TTI_ACCESS_DATA_ERROR_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_TTI_MAIN_ITS_ENGINE_SUM_E */
    /* GTS Interrupt Cause [0x0C002028] */
    {3,GT_FALSE,0, NULL, 0x0C002028, 0x0C00202C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TTI_ITS_CPU_ADDR_OUT_OF_RANGE_E,
        PRV_CPSS_XCAT2_TTI_ITS_GLOBAL_FIFO_FULL_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_TCC_LOWER_SUM_E */
    /* TCC Unit Interrupt Summary Cause [0x0C0001BC] */
    {5,GT_FALSE,0, NULL, 0x0E0001BC, 0x0E0001C0,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TCC_LOWER_UNIT_SUM_E,
        PRV_CPSS_XCAT2_TCC_LOWER_UNIT_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_TCC_LOWER_UNIT_SUM_E */
    /* TCC Interrupt Cause [0x0E0001A4] */
    {1,GT_FALSE,0, NULL, 0x0E0001A4, 0x0E0001A8,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TCC_LOWER_UNIT_TCAM_ERROR_DETECTED_E,
        PRV_CPSS_XCAT2_TCC_LOWER_UNIT_TCAM_BIST_FAILED_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_TCC_UPPER_SUM_E */
    /* TCC_Upper_TCC Unit Interrupt Summary Cause [0x0E800078]*/
    {6,GT_FALSE,0, NULL, 0x0E800078, 0x0E80007C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TCC_UPPER_UNIT_SUM_E,
        PRV_CPSS_XCAT2_TCC_UPPER_UNIT_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_TCC_UPPER_UNIT_SUM_E */
    /* TCC_Upper_TCC Interrupt Cause [0x0E800060]*/
    {1,GT_FALSE,0, NULL, 0x0E800060, 0x0E800064,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TCC_UPPER_UNIT_TCAM_ERROR_DETECTED_E,
        PRV_CPSS_XCAT2_TCC_UPPER_UNIT_TCAM_BIST_FAILED_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_BCN_SUM_E */
    /* BCNIntSum address [0x0A400018] */
    {7,GT_FALSE,0, NULL, 0x0A400018, 0x0A40001C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_BCN_COUNTER_WRAP_AROUND_ERR_E,
        PRV_CPSS_XCAT2_BCN_COUNTER_WRAP_AROUND_ERR_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_EPLR_SUM_E */
    /* EPLR Policer Interrupt Cause [0x03800100] */
    {8,GT_FALSE,0, NULL, 0x03800100, 0x03800104,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_EPLR_DATA_ERROR_E,
        PRV_CPSS_XCAT2_EPLR_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_MISC_SUM_E */
    /* Miscellaneous Interrupt Cause Register [0x00000038] */
    {9,GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_MISC_TWSI_TIME_OUT_E,
        PRV_CPSS_XCAT2_MISC_PEX_ADDR_UNMAPPED_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_BUF_MEM_SUM_E */
    /* Buffer Memory Main Interrupt Cause Register [0x07800090] */
    {10,GT_FALSE,0, NULL, 0x07800090, 0x07800094,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_BUF_MEM_REG0_SUM_E,
        PRV_CPSS_XCAT2_BUF_MEM_REG1_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_BUF_MEM_REG0_SUM_E */
    /* Buffer Memory Interrupt Cause Register0 [0x07800024] */
    {1,GT_FALSE,0, NULL, 0x07800024, 0x07800028,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_BUF_MEM_REG0_TX_FIFO_OVER_RUN_PORT0_E,
        PRV_CPSS_XCAT2_BUF_MEM_REG0_BURST_FIFO_OVER_RUN_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_BUF_MEM_REG1_SUM_E */
    /* Buffer Memory Interrupt Cause Register1 */
    {2,GT_FALSE,0, NULL, 0x07800098, 0x0780009C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_BUF_MEM_REG1_BANK0_ONE_ECC_ERROR_E,
        PRV_CPSS_XCAT2_BUF_MEM_REG1_BANK1_TWO_OR_MORE_ECC_ERRORS_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_TXQ_SUM_E */
    /* Transmit Queue Interrupt Summary Cause Register [0x01800100] */
    {11, GT_FALSE,0, NULL, 0x01800100, 0x01800104,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TXQ_MAIN_FLASH_E,
        PRV_CPSS_XCAT2_TXQ_MAIN_FULL_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 5, NULL, NULL},

         /* (bit 1) Transmit Queue WatchDog -->
            revered bit --> removed from the enum and the tree */

    /* Subtree *.*.PRV_CPSS_XCAT2_TXQ_MAIN_FLASH_E */
    /* Transmit Queue Flush Done Interrupt Cause [0x01800110] */
     {2, GT_FALSE,0, NULL, 0x01800110, 0x01800114,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TXQ_FLUSH_PORT0_E,
        PRV_CPSS_XCAT2_TXQ_FLUSH_PORT_CPU_63_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_TXQ_MAIN_GENERAL_E */
    /* Transmit Queue General Interrupt Cause [0x01800118] */
    {3, GT_FALSE,0, NULL, 0x01800118, 0x0180011C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TXQ_GEN_EGR_MIRROR_DESC_DROP_E,
        PRV_CPSS_XCAT2_TXQ_GEN_BUF_FULL_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_TXQ_MAIN_GPP_E */
    /* GPP Interrupt Cause Register */
    {4, GT_FALSE,0, NULL, 0x018001CC, 0x018001C0,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TXQ_GPP_INTERRUPT0_E,
        PRV_CPSS_XCAT2_TXQ_GPP_INTERRUPT11_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_TXQ_MAIN_EGRESS_STC_E */
    /* Egress STC Interrupt Cause [0x01800130] */
    {5, GT_FALSE,0, NULL, 0x01800130, 0x01800134,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TXQ_EGRESS_STC_PORT0_E,
        PRV_CPSS_XCAT2_TXQ_EGRESS_STC_PORT27_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_TXQ_MAIN_FULL_E */
    /* Transmit Queue Desc Full Interrupt Summary Cause [0x0180018C] */
    {7, GT_FALSE,0, NULL, 0x0180018C, 0x018001EC,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TXQ_FULL_DESC_PORT0_E,
        PRV_CPSS_XCAT2_TXQ_FULL_DESC_XG_MULTICAST_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_BRIDGE_SUM_E */
    /* Bridge Engine Interrupt Cause Register [0x02040130] */
    {12, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_BRIDGE_NA_FIFO_FULL_E,
        PRV_CPSS_XCAT2_BRIDGE_SECURITY_BREACH_UPDATE_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_BM0_SUM_E */
    /* Buffer Management Interrupt Cause Register0 [0x03000040] */
    {13, GT_FALSE,0, NULL, 0x03000040, 0x03000044,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_BM0_EMPTY_CLEAR_E,
        PRV_CPSS_XCAT2_BM0_PORT_RX_FULL_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_BM1_SUM_E */
    /* Buffer Management Interrupt Cause Register 1 [0x03000048]*/
    {14, GT_FALSE,0, NULL, 0x03000048, 0x0300004C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_BM1_MAX_BUFF_REACHED_PORT0_E,
        PRV_CPSS_XCAT2_BM1_MAX_BUFF_REACHED_PORT_CPU_63_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_FDB_SUM_E */
    /* FDB MAC Table Interrupt Cause Register [0x06000018] */
       /*maskRcvIntrEn - both AUProcCompletedInt(bit 9) and AgeViaTriggerEnded(bit 4)
       should never be masked to avoid missed events situation.  */
    {15, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_FDB_MAC_NUM_OF_HOP_EXP_E,
        PRV_CPSS_XCAT2_FDB_MAC_FIFO_2_CPU_EXCEEDED_E,
        FILLED_IN_RUNTIME_CNS, 0x0,(0xFFFFFFFF&(~(BIT_9 | BIT_4))),0,NULL,NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_PORTS_MIB_SUM_E */
    /* Tri-speed Ports and MIB Counters Summary Register [0x04004010] */
    {16, GT_FALSE,0, NULL, 0x04004010, 0x04805110,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP0MIB_SUM_E,
        PRV_CPSS_XCAT2_PORTS_MIB_PORT_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 5, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_PORTS_MIB_GOP0MIB_SUM_E */
    /* Ports GOP0 MIBs Interrupt Cause [0x04004000] */
    {1, GT_FALSE,0, NULL, 0x04004000, 0x04004004,
        prvCpssDxChPortGroupGopIsrRegRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP0_ADDRESS_OUT_OF_RANGE_E,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP0_COUNT_COPY_DONE_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_PORTS_MIB_GOP1MIB_SUM_E */
    /* Ports GOP1 MIBs Interrupt Cause [0x04804000] */
    {2, GT_FALSE,0, NULL, 0x04804000, 0x04804004,
        prvCpssDxChPortGroupGopIsrRegRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP1_ADDRESS_OUT_OF_RANGE_E,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP1_COUNT_COPY_DONE_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_PORTS_MIB_GOP2MIB_SUM_E */
    /* Ports GOP2 MIBs Interrupt Cause [0x05004000] */
    {3, GT_FALSE,0, NULL, 0x05004000, 0x05004004,
        prvCpssDxChPortGroupGopIsrRegRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP2_ADDRESS_OUT_OF_RANGE_E,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP2_COUNT_COPY_DONE_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_PORTS_MIB_GOP3MIB_SUM_E */
    /* Ports GOP3 and Stack MIBs Interrupt Cause [0x05004000] */
    {4, GT_FALSE,0, NULL, 0x05804000, 0x05804004,
        prvCpssDxChPortGroupGopIsrRegRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP3_ADDRESS_OUT_OF_RANGE_E,
        PRV_CPSS_XCAT2_PORTS_MIB_GOP3_COUNT_COPY_DONE_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_PORTS_MIB_PORT_SUM_E */
    /* Ports Interrupt Summary Register [0x05004010] */
    {5, GT_FALSE,0, NULL, 0x05004010, 0x04805114,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PORTS_NETW_0_SUM_E,
        PRV_CPSS_XCAT2_PORTS_NETW_23_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF,24,NULL, NULL},

    /* Subtree *.*.*.PRV_CPSS_XCAT2_PORTS_NETW_0_SUM_E - ..._24_SUM_E */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(0),  /* Port  0 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(1),  /* Port  1 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(2),  /* Port  2 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(3),  /* Port  3 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(4),  /* Port  4 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(5),  /* Port  5 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(6),  /* Port  6 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(7),  /* Port  7 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(8),  /* Port  8 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(9),  /* Port  9 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(10), /* Port 10 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(11), /* Port 11 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(12), /* Port 12 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(13), /* Port 13 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(14), /* Port 14 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(15), /* Port 15 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(16), /* Port 16 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(17), /* Port 17 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(18), /* Port 18 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(19), /* Port 19 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(20), /* Port 20 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(21), /* Port 21 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(22), /* Port 22 */
    GE_MAC_PORT_INTERRUPT_LEAF_MAC(23), /* Port 23 */

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_CPU_PORT_SUM_E */
    /* CPU Port Interrupt Cause Register [0x0A80FC20] */
    {17, GT_FALSE,0, NULL,
       GE_MAC_PORT_INTERRUPT_CAUSE_MAC(63),
       (GE_MAC_PORT_INTERRUPT_CAUSE_MAC(63) + 4),
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_CPU_PORT_LINK_STATUS_CHANGED_E,
        PRV_CPSS_XCAT2_CPU_PORT_TX_FIFO_UNDERRUN_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_STACK_PORTS_SUM_E */
    /* Stack Ports  Interrupt Cause Register [0x00000080] */
    {18, GT_FALSE,0, NULL, 0x00000080, 0x00000084,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_STACK_PORT24_SUM_E,
        PRV_CPSS_XCAT2_STACK_PORT27_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 4,NULL, NULL},

    /* Subtree *.*.*.PRV_CPSS_XCAT2_STACK_PORT24_SUM_E - ..._PORT27_SUM_E*/
    /* Port 24 Interrupt Cause Register [0x0A806020] */
    STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(24), /* GE Port 24 */
    STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(25), /* GE Port 25 */
    STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(26), /* GE Port 26 */
    STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(27), /* GE Port 27 */

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_TX_SDMA_SUM_E */
    /* Transmit SDMA Interrupt Cause Register TxSDMAInt [0x00002810] */
    {19, GT_FALSE,0, NULL, 0x00002810, 0x00002818,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_TX_SDMA_BUFFER_QUEUE0_E,
        PRV_CPSS_XCAT2_TX_SDMA_END_QUEUE7_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_RX_SDMA_SUM_E */
    /* Receive SDMA Interrupt Cause Register RxSDMAInt [0x0000280C] */
    {20, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_RX_SDMA_BUFFER_QUEUE0_E,
        PRV_CPSS_XCAT2_RX_SDMA_PACKET_CNT_OVERFLOW_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_IPCL_SUM_E */
    /* PCL Unit Interrupt Summary Cause [0x0B800060] */
    {21, GT_FALSE,0, NULL, 0x0B800060, 0x0B800064,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_IPCL_SUM_E,
        PRV_CPSS_XCAT2_IPCL_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1, NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_IPCL_SUM_E */
    /* Policy Engine Interrupt Cause [0x0B800004] */
    {1, GT_FALSE,0, NULL, 0x0B800004, 0x0B800008,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_IPCL_MG_ADDR_OUT_OF_RANGE_E,
        PRV_CPSS_XCAT2_IPCL_INLIF_TABLE_ECC_ERROR_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_IPLR0_SUM_E */
    /* IPLR0IntSum address [0x0C800100]*/
    {22, GT_FALSE,0, NULL, 0x0C800100, 0x0C800104,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_IPLR0_DATA_ERROR_E,
        PRV_CPSS_XCAT2_IPLR0_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_IPLR1_SUM_E */
    /* IPLR1IntSum address [0x0D000100] */
    {23, GT_FALSE,0, NULL, 0x0D000100, 0x0D000104,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_IPLR1_DATA_ERROR_E,
        PRV_CPSS_XCAT2_IPLR1_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_PREEGR_SUM_E */
    /* Pre-Egress Interrupt Summary Register [0x0B000058] */
    {24, GT_FALSE,0, NULL, 0x0B000058, 0x0B00005C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PREEGR_INGR_STC_SUM_E,
        PRV_CPSS_XCAT2_PREEGR_SCT_RATE_LIM_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_PREEGR_INGR_STC_SUM_E */
    /* Ingress STC (Samiling To CPU) Interrupt Cause [0x0B000020] */
    {1, GT_FALSE,0, NULL, 0x0B000020, 0x0B000024,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PREEGR_INGR_STC_PORT0_E,
        PRV_CPSS_XCAT2_PREEGR_INGR_STC_PORT27_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_PREEGR_SCT_RATE_LIM_SUM_E */
    /* CPU Code Rate Limiters Interrupt Cause [0x0B000060] */
    {2, GT_FALSE,0, NULL, 0x0B000060, 0x0B000064,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_1_PKT_DROP_E,
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_31_PKT_DROP_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.PRV_CPSS_XCAT2_GLOBAL_HA_HEP_SUM_E */
    /* Hep Interrupt Summary Cause [0x0F0003A8] */
    {25, GT_FALSE,0, NULL, 0x0F0003A8, 0x0F0003AC,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_HA_HEP_HA_SUM_E,
        PRV_CPSS_XCAT2_HA_HEP_ETS_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_HA_HEP_HA_SUM_E */
    /* HA Interrupt Cause [0x0F0003A0] */
    {1, GT_FALSE,0, NULL, 0x0F0003A0, 0x0F0003A4,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_HA_EPCL_DATA_FIFO_FULL_E,
        PRV_CPSS_XCAT2_HA_STG3_OVERSIZE_TAG_RCV_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    /* Subtree *.*.PRV_CPSS_XCAT2_HA_ETS_HEP_HA_SUM_E */
    /* GTS Interrupt Cause [0x0F000828] */
    {3, GT_FALSE,0, NULL, 0x0F000828, 0x0F00082C,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT2_HA_ETS_CPU_ADDR_OUT_OF_RANGE_E,
        PRV_CPSS_XCAT2_HA_ETS_GLOBAL_FIFO_FULL_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
};

/* number of elements in the array of xcatIntrScanArr[] */
#define XCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS \
    (sizeof(xcat2IntrScanArr)/sizeof(xcat2IntrScanArr[0]))

/* number of mask registers -- according to enum of interrupts */
#define XCAT2_NUM_MASK_REGISTERS_CNS  (PRV_CPSS_XCAT2_LAST_INT_E / 32)

/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 xcat2MaskRegDefaultSummaryArr[XCAT2_NUM_MASK_REGISTERS_CNS] =
{
    /* filled from xcatIntrScanArr */
    FILLED_IN_RUNTIME_CNS
};

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 xcat2MaskRegMapArr[XCAT2_NUM_MASK_REGISTERS_CNS] =
{
    /* filled from xcatIntrScanArr */
    FILLED_IN_RUNTIME_CNS
};



/*******************************************************************************
* prvCpssDrvPpIntDefDxChXcat2Init
*
* DESCRIPTION:
*       Interrupts initialization for the DxChXcat2 devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefDxChXcat2Init(void)
{
    GT_STATUS   rc;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;


    /*
       1. fill the array of mask registers addresses
       2. fill the array of default values for the mask registers
       3. update the bits of nonSumBitMask in the scan tree
    */
    rc = prvCpssDrvExMxDxHwPpMaskRegInfoGet(
            CPSS_PP_FAMILY_DXCH_XCAT2_E,
            XCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS,
            xcat2IntrScanArr,
            XCAT2_NUM_MASK_REGISTERS_CNS,
            xcat2MaskRegDefaultSummaryArr,
            xcat2MaskRegMapArr);
    if (rc != GT_OK)
    {
        /* error */
        return ;
    }

    /* set Xcat revision 1 and above specific configurations */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_XCAT2_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_XCAT2_LAST_INT_E;
    devFamilyPtr->maskRegistersAddressesArray = xcat2MaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = xcat2MaskRegDefaultSummaryArr;
    devFamilyPtr->interruptsScanArray = xcat2IntrScanArr;
    devFamilyPtr->numOfScanElements = XCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS;
    devFamilyPtr->fdbTrigEndedId = 0;/* don't care !!!! */
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=NULL;

}

#ifdef DUMP_DEFAULT_INFO
/*******************************************************************************
* prvCpssDrvPpIntDefXcat2Print
*
* DESCRIPTION:
*      print the interrupts arrays info, of xcat devices
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       void
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void  prvCpssDrvPpIntDefXcat2Print(
    void
)
{
    prvCpssDrvPpIntDefPrint(XCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS,xcat2IntrScanArr,
                            XCAT2_NUM_MASK_REGISTERS_CNS,xcat2MaskRegMapArr,
                            xcat2MaskRegDefaultSummaryArr);
}
#endif/*DUMP_DEFAULT_INFO*/

