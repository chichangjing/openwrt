/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefDxChXcat.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- DxCh xCat devices
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah3.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsDxChXcat.h>

/* User defined register 1 - used instead of non-existing addresses */
#define STUB_ADDRESS_CNS            0xF4

/* set values that will be updated/filled during runtime (initialization stage)*/
#define FILLED_IN_RUNTIME_CNS       0

/* the first XG port */
#define XCAT_FIRST_XG_PORT_CNS      24

/* xcat A0 copied from ch3 and updated in runtime */
#define CH3_NUM_MASK_REG_CNS        68


static PRV_CPSS_DRV_INTERRUPT_SCAN_STC xcatA0IntrScanArr[CH3_NUM_MASK_REG_CNS] =
{
    /* copied from ch3 */
    {FILLED_IN_RUNTIME_CNS}
};

static const GT_U32 xcatA0NumTreeElements = sizeof(xcatA0IntrScanArr)/sizeof(xcatA0IntrScanArr[0]);

/* distance between interrupts of port 24 and 25 --> same distance between any 2 XG ports */
#define DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS   \
    (PRV_CPSS_XCAT_XG_PORT25_PORT_E - PRV_CPSS_XCAT_XG_PORT24_PORT_E)

/* distance between interrupts of lanes in XG port --> same distance between any 2 lanes (of XG port) */
#define DISTANCE_BETWEEN_XG_LANES_INTERRUPTS_CNS   \
    (PRV_CPSS_XCAT_XG_PORT24_LANE1_SUM_SUM_E - PRV_CPSS_XCAT_XG_PORT24_LANE0_SUM_SUM_E)


/* interrupt register for the GE MAC port */
#define GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port)      (0x0A800020  + (port)*0x400)

/* interrupt register for the XG MAC port */
#define XG_MAC_PORT_INTERRUPT_CAUSE_MAC(port)      (0x08800014  + (port)*0x400)

/* interrupt register for the XG port XPCS interrupts */
#define XG_PORT_XPCS_INTERRUPT_CAUSE_MAC(port)      (0x08800214  + (port)*0x400)

/* interrupt register for the XG port XPCS lane interrupts */
#define XG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(port,lane)      (0x08800260 + (port)*0x400 + (lane)*0x44)

#define XG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port)      (0x08800058  + (port)*0x400)

/* this register not exists --> we assume that it behave like 'summary bits set to 1'*/
/* so we change the value to dummy register that the engine can
    write to ---> used only in the init phase */
/* we not change xcatMaskRegDefaultSummaryArr[ii] because this
   is the value that will be kept in the 'shadow' of the cpss-ISR manager */
#define XG_PORT_SUMMARY_INTERRUPT_MASK_MAC(port)      STUB_ADDRESS_CNS

/* GE MAC port leaf */
#define GE_MAC_PORT_INTERRUPT_LEAF_MAC(port) \
            { (port) + 1 ,GT_FALSE,0, NULL,GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) , GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port) * 32) + PRV_CPSS_XCAT_LINK_STATUS_CHANGED_PORT0_E,  \
             ((port) * 32) + PRV_CPSS_XCAT_TX_FIFO_UNDERRUN_PORT0_E,     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


/* stacking port -- dummy GE MAC port leaf */
#define STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(port) \
            { (port) - 4 ,GT_FALSE,0, NULL,GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) , GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port) * 32) + PRV_CPSS_XCAT_LINK_STATUS_CHANGED_PORT0_E,  \
             ((port) * 32) + PRV_CPSS_XCAT_TX_FIFO_UNDERRUN_PORT0_E,     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}



/* XG MAC port leaf */
#define XG_MAC_PORT_INTERRUPT_LEAF_MAC(port) \
            { 1 ,GT_FALSE,0, NULL,XG_MAC_PORT_INTERRUPT_CAUSE_MAC(port) , XG_MAC_PORT_INTERRUPT_CAUSE_MAC(port) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port-XCAT_FIRST_XG_PORT_CNS) * DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS) + PRV_CPSS_XCAT_XG_PORT24_LINK_STATUS_CHANGED_E,  \
             ((port-XCAT_FIRST_XG_PORT_CNS) * DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS) + PRV_CPSS_XCAT_XG_PORT24_XG_COUNT_EXPIRED_E,     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* dummy function to read from in the ISR context ,
the function allow to select between XG / GE registers
of the 'stacking ports' (flex ports) */
static GT_STATUS xcatXgPortMultiplexerHwPpPortGroupIsrRead
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    GT_STATUS rc;
    /* set bit 0 -- 'Summary of the register' */
    /* set bit 1 -- 'GE summary' */
    /* set bit 2 -- 'XG summary' */

    rc = prvCpssDrvHwPpPortGroupIsrRead(devNum,portGroupId,regAddr,dataPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* emulate bits 24..27 to be copied also to bits 20..23 */
    (*dataPtr) |= ((*dataPtr) >> 4) & ((0xF) << 20);

    return GT_OK;
}


/* XG port XPCS lane leaf */
#define XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(port,lane) \
            { 7 + (lane) ,GT_FALSE,0, NULL,XG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(port,lane) , XG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(port,lane) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port-XCAT_FIRST_XG_PORT_CNS) * DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS) + ((lane) * DISTANCE_BETWEEN_XG_LANES_INTERRUPTS_CNS) + PRV_CPSS_XCAT_XG_PORT24_LANE0_SUM_PRBS_ERROR_E,  \
             ((port-XCAT_FIRST_XG_PORT_CNS) * DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS) + ((lane) * DISTANCE_BETWEEN_XG_LANES_INTERRUPTS_CNS) + PRV_CPSS_XCAT_XG_PORT24_LANE0_SUM_DETECTED_IIAII_E,     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* XG port XPCS leaf */
#define XG_PORT_XPCS_INTERRUPT_LEAF_MAC(port) \
            { 2 ,GT_FALSE,0, NULL,XG_PORT_XPCS_INTERRUPT_CAUSE_MAC(port) , XG_PORT_XPCS_INTERRUPT_CAUSE_MAC(port) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port-XCAT_FIRST_XG_PORT_CNS) * DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS) + PRV_CPSS_XCAT_XPCS_PORT24_LINK_STATUS_CHANGED_E,  \
             ((port-XCAT_FIRST_XG_PORT_CNS) * DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS) + PRV_CPSS_XCAT_XPCS_PORT24_LANE3_E,     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 4,NULL, NULL}

/* XG port summary leaf , for ports 24..27 */
#define XG_PORT_SUMMARY_LEAF_MAC(port)    \
            {(port), GT_FALSE,0, NULL, XG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) , XG_PORT_SUMMARY_INTERRUPT_MASK_MAC(port),                    \
              prvCpssDrvHwPpPortGroupIsrRead,                                                                    \
              prvCpssDrvHwPpPortGroupIsrWrite,                                                                   \
             ((port-XCAT_FIRST_XG_PORT_CNS) * DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS) + PRV_CPSS_XCAT_XG_PORT24_SUM_XG_E,          \
             ((port-XCAT_FIRST_XG_PORT_CNS) * DISTANCE_BETWEEN_XG_PORTS_INTERRUPTS_CNS) + PRV_CPSS_XCAT_XG_PORT24_SUM_XPCS_SUMMARY_E,\
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL}

/* the sub tree of interrupts that relate to the XG port , without the summary register */
#define XG_PORT_SUB_TREE_MAC(port)                          \
        XG_MAC_PORT_INTERRUPT_LEAF_MAC(port),  /* bit 1 - XG */    \
        XG_PORT_XPCS_INTERRUPT_LEAF_MAC(port), /* bit 2 , has 6 'childs' --> per lane interrupts */      \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(port,0), /* lane 0 */  \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(port,1), /* lane 1 */  \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(port,2), /* lane 2 */  \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(port,3)  /* lane 3 */

/* all the sub tree of interrupts that relate to the XG port 24..27 */
#define XG_PORT_TREE_MAC(port)      \
        XG_PORT_SUMMARY_LEAF_MAC(port), \
            XG_PORT_SUB_TREE_MAC(port)


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

static PRV_CPSS_DRV_INTERRUPT_SCAN_STC xcatIntrScanArr[] =
{
        /* Global */
    {0, GT_FALSE,0, NULL, 0x30, 0x34,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_XCAT_GLOBAL_PEX_SUM_E,
        PRV_CPSS_XCAT_GLOBAL_EQ_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0x0, 24, NULL, NULL},


             /* PEX */
        {1,GT_FALSE,0, NULL, 0x000F1900, 0x000F1910,
            prvCpssDrvHwPpPortGroupReadInternalPciReg,
            prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            PRV_CPSS_XCAT_PEX_RCV_A_E,
            PRV_CPSS_XCAT_PEX_RCV_D_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


             /* PEX ERROR */
        {2,GT_FALSE,0, NULL, 0x000F1900, 0x000F1910,
            prvCpssDrvHwPpPortGroupReadInternalPciReg,
            prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            PRV_CPSS_XCAT_PEX_DL_DOWN_TX_ACC_ERR_E,
            PRV_CPSS_XCAT_PEX_PEX_LINK_FAIL_E,
            FILLED_IN_RUNTIME_CNS, 0xFF1F1B, 0xFFFFFFFF, 0,NULL, NULL},


             /* CNC */
        {3,GT_FALSE,0, NULL, 0x08000100, 0x08000104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_CNC_WRAPAROUND_BLOCK0_E,
            PRV_CPSS_XCAT_CNC_DUMP_FINISHED_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* TTI */
        {4,GT_FALSE,0, NULL, 0x0C000004    , 0x0C000008    ,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_TTI_CPU_ADDRESS_OUT_OF_RANGE_E,
            PRV_CPSS_XCAT_TTI_ACCESS_DATA_ERROR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* TCC_Lower (IPCL)*/
        {5,GT_FALSE,0, NULL, 0x0E0001A4    , 0x0E0001A8    ,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_TCC_LOWER_SUM_TCAM_ERROR_DETECTED_E,
            PRV_CPSS_XCAT_TCC_LOWER_SUM_TCAM_BIST_FAILED_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* TCC_Upper (IPVX)*/
        {6,GT_FALSE,0, NULL, 0x0E800060    , 0x0E800064    ,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_TCC_UPPER_SUM_TCAM_ERROR_DETECTED_E,
            PRV_CPSS_XCAT_TCC_UPPER_SUM_TCAM_BIST_FAILED_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* BCN */
        {7,GT_FALSE,0, NULL, 0x0A400018  , 0x0A40001c    ,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_BCN_COUNTER_WRAP_AROUND_ERR_E,
            PRV_CPSS_XCAT_BCN_COUNTER_WRAP_AROUND_ERR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* EPLR */
        {8,GT_FALSE,0, NULL, 0x03800100   , 0x03800104    ,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_EPLR_SUM_DATA_ERROR_E,
            PRV_CPSS_XCAT_EPLR_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Miscellaneous */
        {9,GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_MISC_TWSI_TIME_OUT_E,
            PRV_CPSS_XCAT_MISC_PEX_ADDR_UNMAPPED_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Buffers Memory Main */
        {10,GT_FALSE,0, NULL, 0x07800090, 0x07800094,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_BM_MAIN_REG0_SUM_E,
            PRV_CPSS_XCAT_BM_MAIN_REG1_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2, NULL, NULL},

                 /* Buffers Memory Register 0  */
            {1,GT_FALSE,0, NULL, 0x07800024, 0x07800028,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_BM_TX_FIFO_OVER_RUN_PORT0_E,
                PRV_CPSS_XCAT_BUF_MEM_BURST_FIFO_OVER_RUN_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                 /* Buffers Memory Register 1  */
            {2,GT_FALSE,0, NULL, 0x07800098, 0x0780009C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_BUF_MEM_BANK0_ONE_ECC_ERROR_E,
                PRV_CPSS_XCAT_BUF_MEM_BANK1_TWO_OR_MORE_ECC_ERRORS_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


            /* Transmit Queue */
        {11, GT_FALSE,0, NULL, 0x01800100, 0x01800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_TXQ_SUM_FLASH_E,
            PRV_CPSS_XCAT_TXQ_SUM_FULL_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 5, NULL, NULL},

             /* (bit 1) Transmit Queue WatchDog -->
                revered bit --> removed from the enum and the tree */

            /* Transmit Queue Flash */
             {2, GT_FALSE,0, NULL, 0x01800110, 0x01800114,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_XCAT_TQ_TXQ2_FLUSH_PORT0_E,
                 PRV_CPSS_XCAT_TQ_TXQ2_FLUSH_PORT_CPU_63_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Transmit Queue General */
             {3, GT_FALSE,0, NULL, 0x01800118, 0x0180011C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_XCAT_TQ_SNIFF_DESC_DROP_E,
                 PRV_CPSS_XCAT_TXQ_GEN_BUF_FULL_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* GPP */
             {4, GT_FALSE,0, NULL, 0x018001CC, 0x018001C0,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_XCAT_GPP_INTERRUPT1_E,
                 PRV_CPSS_XCAT_GPP_INTERRUPT12_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

               /* Egress STC */
             {5, GT_FALSE,0, NULL, 0x01800130, 0x01800134,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_XCAT_EGRESS_STC_PORT0_E,
                 PRV_CPSS_XCAT_EGRESS_STC_PORT27_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Transmit Queue Full */
             {7, GT_FALSE,0, NULL, 0x0180018C, 0x018001EC,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_XCAT_TQ_DESC_FULL_PORT0_E,
                 PRV_CPSS_XCAT_TQ_XG_MC_FIFO_FULL_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


        /* L2 Bridge  */
        {12, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_EB_NA_FIFO_FULL_E,
            PRV_CPSS_XCAT_EB_SECURITY_BREACH_UPDATE_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


           /* Buffer Management Register 0*/
        {13, GT_FALSE,0, NULL, 0x03000040, 0x03000044,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_BM_EMPTY_CLEAR_E,
            PRV_CPSS_XCAT_BM_PORT_RX_FULL_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

           /* Buffer Management Register 1*/
        {14, GT_FALSE,0, NULL, 0x03000048, 0x0300004C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_BM_MAX_BUFF_REACHED_PORT0_E,
            PRV_CPSS_XCAT_BM_MAX_BUFF_REACHED_PORT_CPU_63_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


        /* FDBIntSum - FDB Interrupt Cause,
           maskRcvIntrEn - both AUProcCompletedInt(bit 9) and AgeViaTriggerEnded(bit 4)
           should never be masked to avoid missed events situation.  */
        {15, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_MAC_NUM_OF_HOP_EXP_E,
            PRV_CPSS_XCAT_MAC_FIFO_2_CPU_EXCEEDED_E,
            FILLED_IN_RUNTIME_CNS, 0x0, (0xFFFFFFFF & ~ (BIT_4 | BIT_9)), 0,NULL, NULL},

        /* Ports - Tri-speed ports MIB counters  */
        {16, GT_FALSE,0, NULL, 0x04004010, 0x04805110,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_3SPPMIB_GOP0MIB_SUM_E,
            PRV_CPSS_XCAT_3SPPMIB_PORTS_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 5, NULL, NULL},

            /* GOP0 MIB  */
            {1, GT_FALSE,0, NULL, 0x04004000, 0x04004004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_GOP_ADDRESS_OUT_OF_RANGE_0_E,
                PRV_CPSS_XCAT_GOP_COUNT_COPY_DONE_0_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* GOP1 MIB  */
            {2, GT_FALSE,0, NULL, 0x04804000, 0x04804004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_GOP_ADDRESS_OUT_OF_RANGE_1_E,
                PRV_CPSS_XCAT_GOP_COUNT_COPY_DONE_1_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* GOP2 MIB  */
            {3, GT_FALSE,0, NULL, 0x05004000, 0x05004004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_GOP_ADDRESS_OUT_OF_RANGE_2_E,
                PRV_CPSS_XCAT_GOP_COUNT_COPY_DONE_2_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GOP3 MIB  */
            {4, GT_FALSE,0, NULL, 0x05804000, 0x05804004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_GOP_ADDRESS_OUT_OF_RANGE_3_E,
                PRV_CPSS_XCAT_GOP_COUNT_COPY_DONE_3_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Tri-speed Ports Register */
            {5, GT_FALSE,0, NULL, 0x05004010, 0x04805114,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_3SPP_PORT0_SUM_E,
                PRV_CPSS_XCAT_3SPP_PORT23_SUM_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF,24,NULL, NULL},


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

                /* NOTE :
                    HW not generate the GE interrupts of ports 24..27 ,
                    on this register !!!! --> but into register 0x80 !!!
                    see the WA done for those in register 0x80 !!!
                */


        /* CPU Port SUM  */         /*0x0A80FC20*/                      /*0x0A80FC24*/
        {17, GT_FALSE,0, NULL, GE_MAC_PORT_INTERRUPT_CAUSE_MAC(63), GE_MAC_PORT_INTERRUPT_CAUSE_MAC(63) + 4,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_XCAT_LINK_STATUS_CHANGED_PORT_CPU_63_E,
             PRV_CPSS_XCAT_TX_FIFO_UNDERRUN_PORT_CPU_63_E,
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* XG Ports  */
        {18, GT_FALSE,0, NULL, 0x00000080, 0x00000084,
             xcatXgPortMultiplexerHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_XCAT_XG_PORT24_SUM_DUMMY_FOR_GE_E,
             PRV_CPSS_XCAT_XG_PORT27_SUM_E,
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 8,NULL, NULL},

   /*bit 20*/STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(24), /* dummy GE Port 24 */
   /*bit 21*/STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(25), /* dummy GE Port 25 */
   /*bit 22*/STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(26), /* dummy GE Port 26 */
   /*bit 23*/STACKING_GE_MAC_PORT_INTERRUPT_LEAF_MAC(27), /* dummy GE Port 27 */

   /*bit 24*/XG_PORT_TREE_MAC(24),    /* XG Port 24 */
   /*bit 25*/XG_PORT_TREE_MAC(25),    /* XG Port 25 */
   /*bit 26*/XG_PORT_TREE_MAC(26),    /* XG Port 26 */
   /*bit 27*/XG_PORT_TREE_MAC(27),    /* XG Port 27 */

            /* Tx SDMA  */
        {19, GT_FALSE,0, NULL, 0x00002810, 0x00002818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_TX_BUFFER_QUEUE0_E,
            PRV_CPSS_XCAT_TX_END_QUEUE7_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Rx SDMA  */
        {20, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_RX_BUFFER_QUEUE0_E,
            PRV_CPSS_XCAT_SDMA_RX_PACKET_CNT_OFF_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* PCL  */
        {21, GT_FALSE,0, NULL, 0x0B800004, 0x0B800008,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_POLICY_ENGINE_MG_ADDR_OUT_OF_RANGE_E,
            PRV_CPSS_XCAT_POLICY_ENGINE_PCL_CONFIG_TABLE_1_PARITY_ERROR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* IPLR0IntSum  */
        {22, GT_FALSE,0, NULL, 0x0C800100, 0x0C800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_IPLR0_SUM_DATA_ERROR_E,
            PRV_CPSS_XCAT_IPLR0_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* IPLR1IntSum  */
        {23, GT_FALSE,0, NULL, 0x0D000100, 0x0D000104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_IPLR1_SUM_DATA_ERROR_E,
            PRV_CPSS_XCAT_IPLR1_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Pre-Egress */
        {24, GT_FALSE,0, NULL, 0x0B000058, 0x0B00005C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_XCAT_PREEGR_INGR_STC_SUM_E,
            PRV_CPSS_XCAT_PREEGR_SCT_RATE_LIM_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

                /* Ingress STC  */
            {1, GT_FALSE,0, NULL, 0x0B000020, 0x0B000024,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_MAC_SFLOW_PORT0_E,
                PRV_CPSS_XCAT_MAC_SFLOW_PORT27_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* STC Rate Limiters */
            {2, GT_FALSE,0, NULL, 0x0B000060, 0x0B000064,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_XCAT_SCT_RATE_LIMITER_1_PKT_DROP_E,
                PRV_CPSS_XCAT_SCT_RATE_LIMITER_31_PKT_DROP_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
};

/* number of elements in the array of xcatIntrScanArr[] */
#define XCAT_NUM_ELEMENTS_IN_SCAN_TREE_CNS \
    (sizeof(xcatIntrScanArr)/sizeof(xcatIntrScanArr[0]))

/* number of mask registers -- according to enum of interrupts */
#define XCAT_NUM_MASK_REGISTERS_CNS  (PRV_CPSS_XCAT_LAST_INT_E / 32)

/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 xcatMaskRegDefaultSummaryArr[XCAT_NUM_MASK_REGISTERS_CNS] =
{
    /* filled from xcatIntrScanArr */
    FILLED_IN_RUNTIME_CNS
};

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 xcatMaskRegMapArr[XCAT_NUM_MASK_REGISTERS_CNS] =
{
    /* filled from xcatIntrScanArr */
    FILLED_IN_RUNTIME_CNS
};



/*******************************************************************************
* prvCpssDrvPpIntDefDxChXcatInit
*
* DESCRIPTION:
*       Interrupts initialization for the DxChXcat devices.
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
void prvCpssDrvPpIntDefDxChXcatInit(void)
{
    GT_STATUS   rc;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;


    /*call to init the Cheetah3 interrupts */
    prvCpssDrvPpIntDefCheetah3Init();

    if(xcatA0NumTreeElements !=
       prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH3_E].numOfScanElements)
    {
        /* error -- we need both arrays with same size otherwise the 'copy' of
           arrays can't be done */
        return;
    }

    /* xcat info */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_XCAT_REV0_E];
    *devFamilyPtr = prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH3_E];
    devFamilyPtr->interruptsScanArray = xcatA0IntrScanArr;

    /* copy ch3 interrupts tree into the xcat A0 tree */
    cpssOsMemCpy(xcatA0IntrScanArr,
        prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH3_E].interruptsScanArray,
        sizeof(xcatA0IntrScanArr));

    /* update the xCat A0 tree */
    /* PEX */
    xcatA0IntrScanArr[1].causeRegAddr = 0x000F1900; /*fix from 0x00071900 in ch3 */
    xcatA0IntrScanArr[1].maskRegAddr  = 0x000F1910; /*fix from 0x00071910 in ch3 */

    /* PEX ERROR */
    xcatA0IntrScanArr[2].causeRegAddr = 0x000F1900; /*fix from 0x00071900 in ch3 */
    xcatA0IntrScanArr[2].maskRegAddr  = 0x000F1910; /*fix from 0x00071910 in ch3 */


    /*
       1. fill the array of mask registers addresses
       2. fill the array of default values for the mask registers
       3. update the bits of nonSumBitMask in the scan tree
    */
    rc = prvCpssDrvExMxDxHwPpMaskRegInfoGet(
            CPSS_PP_FAMILY_DXCH_XCAT_E,
            XCAT_NUM_ELEMENTS_IN_SCAN_TREE_CNS,
            xcatIntrScanArr,
            XCAT_NUM_MASK_REGISTERS_CNS,
            xcatMaskRegDefaultSummaryArr,
            xcatMaskRegMapArr);
    if(rc != GT_OK)
    {
        /* error */
        return ;
    }

    /* set Xcat revision 1 and above specific configurations */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_XCAT_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_XCAT_LAST_INT_E;
    devFamilyPtr->maskRegistersAddressesArray = xcatMaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = xcatMaskRegDefaultSummaryArr;
    devFamilyPtr->interruptsScanArray = xcatIntrScanArr;
    devFamilyPtr->numOfScanElements = XCAT_NUM_ELEMENTS_IN_SCAN_TREE_CNS;
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
* prvCpssDrvPpIntDefXcatPrint
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
void  prvCpssDrvPpIntDefXcatPrint(
    void
)
{
    prvCpssDrvPpIntDefPrint(XCAT_NUM_ELEMENTS_IN_SCAN_TREE_CNS,xcatIntrScanArr,
                            XCAT_NUM_MASK_REGISTERS_CNS,xcatMaskRegMapArr,
                            xcatMaskRegDefaultSummaryArr);
}
#endif/*DUMP_DEFAULT_INFO*/

