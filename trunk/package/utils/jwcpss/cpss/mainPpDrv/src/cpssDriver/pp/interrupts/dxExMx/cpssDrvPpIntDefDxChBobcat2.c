/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefDxChBobcat2.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- DxCh Bobcat2 devices
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsBobcat2.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxUtilLion.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwRegAddr.h>

extern void  prvCpssDrvPpIntDefBobcat2Print(
    void
);


#define EXTRA_INFO_SUB_TREE_MASK_CNS     0xFFFF0000 /* bits 16..31     */
#define EXTRA_INFO_SUB_TREE_BASE_MAC(index)     ((index+1)*0x00010000) /* start at bit 16 */
/* macro to allow field of 'gppId' to indicate that the entry is not relevant to 'A0' */
#define NON_A0_SUB_TREE_CNS                         EXTRA_INFO_SUB_TREE_BASE_MAC(0)

/* macro to allow field of 'gppId' to indicate that the sub tree is 'TXQ_PFC' to
   replace with sub tree of B0

   replace the sub tree of txqPfc_SUB_TREE_MAC with bc2_B0_txqPfc_SUB_TREE_MAC

   NOTE: the tree hold the A0 info because it take more entries then the one by
        the B0.
        the replace will fill extra entries with 'skip info'  (entry.maskRegAddr = CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)


*/
#define A0_TXQ_PFC_TO_REPLACE_WITH_B0_SUB_TREE_CNS    EXTRA_INFO_SUB_TREE_BASE_MAC(1)

/* macro to allow field of 'gppId' to indicate that the sub tree is 'TXQ_LL' to
   replace with sub tree of A0

   replace the sub tree of bc2_B0_txqLl_SUB_TREE_MAC with txqLl_NODE_MAC

   NOTE: the tree hold the B0 info because it take more entries then the one by
        the A0.
        the replace will fill extra entries with 'skip info'  (entry.maskRegAddr = CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)


*/
#define B0_TXQ_LL_TO_REPLACE_WITH_A0_SUB_TREE_CNS    EXTRA_INFO_SUB_TREE_BASE_MAC(2)

/* non valid entry - to be used for 'skip' indication */
static CPSS_INTERRUPT_SCAN_STC nonValidEntry = CPSS_EVENT_SKIP_INTERRUPT_ENTRY_CNS;

/* convert bit in the enum of the huge interrupts of the device into BIT_0..BIT_31 in a register */
#define DEVICE_INT_TO_BIT_MAC(deviceInterruptId)    \
            (1 << ((deviceInterruptId) & 0x1f))

/* FDBIntSum - FDB Interrupt Cause,
   maskRcvIntrEn - both AUProcCompletedInt(bit of PRV_CPSS_BOBCAT2_FDB_AU_PROC_COMPLETED_E)
                   and  AgeViaTriggerEnded(bit of PRV_CPSS_BOBCAT2_FDB_AGE_VIA_TRIGGER_ENDED_E)
   should never be masked to avoid missed events situation.  */
#define FDB_REG_MASK_CNS        \
    (0xFFFFFFFF & ~                                                             \
        (DEVICE_INT_TO_BIT_MAC(PRV_CPSS_BOBCAT2_FDB_AGE_VIA_TRIGGER_ENDED_E) |  \
         DEVICE_INT_TO_BIT_MAC(PRV_CPSS_BOBCAT2_FDB_AU_PROC_COMPLETED_E)))

/* node for ingress STC interrupts leaf */
#define INGRESS_STC_NODE_FOR_16_PORTS_MAC(index) \
            {(index+1), GT_FALSE,PRV_CPSS_DXCH_UNIT_EQ_E, NULL, 0x0d000620 + (index * 0x10) , 0x0d000624 + (index * 0x10), \
                 prvCpssDrvHwPpPortGroupIsrRead,                                        \
                 prvCpssDrvHwPpPortGroupIsrWrite,                                       \
                PRV_CPSS_BOBCAT2_INGRESS_STC_PORT_0_INGRESS_SAMPLE_LOADED_E   + (index * 32),    \
                PRV_CPSS_BOBCAT2_INGRESS_STC_PORT_15_INGRESS_SAMPLE_LOADED_E  + (index * 32),    \
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* node for egress STC interrupts leaf */
#define EGRESS_STC_NODE_FOR_18_PORTS_MAC(index) \
            {(index+6), GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_DQ_E, NULL, 0x40000630 + (index * 4) , 0x40000640 + (index * 4), \
                 prvCpssDrvHwPpPortGroupIsrRead,                                        \
                 prvCpssDrvHwPpPortGroupIsrWrite,                                       \
                PRV_CPSS_BOBCAT2_TXQ_DQ_EGR_STC_PORT_0_E   + (index * 32),    \
                PRV_CPSS_BOBCAT2_TXQ_DQ_EGR_STC_PORT_17_E  + (index * 32),    \
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


/* node for ingress SCT rate limiters interrupts leaf */
#define SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(index) \
            {(index+1), GT_FALSE,PRV_CPSS_DXCH_UNIT_EQ_E, NULL, 0x0D010020  + (index * 0x10) , 0x0D010024  + (index * 0x10), \
                 prvCpssDrvHwPpPortGroupIsrRead,                                         \
                 prvCpssDrvHwPpPortGroupIsrWrite,                                       \
                PRV_CPSS_BOBCAT2_CPU_CODE_RATE_LIMITERS_SCT_RATE_LIMITER_0_PKT_DROPED_E   + (index * 32),    \
                PRV_CPSS_BOBCAT2_CPU_CODE_RATE_LIMITERS_SCT_RATE_LIMITER_15_PKT_DROPED_E  + (index * 32),    \
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


/* TXQ queue: low_crossing for port - cause */
#define BOBCAT2_TXQ_QUEUE_LOW_CROSSING_PORT_INTERRUPT_CAUSE_MAC(port) \
        (0x3C090080 + 0x4*(port))

/* TXQ queue: low_crossing for port - mask */
#define BOBCAT2_TXQ_QUEUE_LOW_CROSSING_PORT_INTERRUPT_MASK_MAC(port) \
        (0x3C090280 + 0x4*(port))

/* TXQ queue: low_crossing for port */
#define BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC(bitIndexInCaller,port)   \
            {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, \
              BOBCAT2_TXQ_QUEUE_LOW_CROSSING_PORT_INTERRUPT_CAUSE_MAC(port) , \
              BOBCAT2_TXQ_QUEUE_LOW_CROSSING_PORT_INTERRUPT_MASK_MAC(port)  , \
              prvCpssDrvHwPpPortGroupIsrRead,                    \
              prvCpssDrvHwPpPortGroupIsrWrite,                   \
              PRV_CPSS_BOBCAT2_LOW_CROSSING_PORT_##port##_LOW_THRESHOLD_CROSSED_FOR_PORT_QUEUE0_E, \
              PRV_CPSS_BOBCAT2_LOW_CROSSING_PORT_##port##_LOW_THRESHOLD_CROSSED_FOR_PORT_QUEUE7_E,\
              FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* TXQ queue: high_crossing for port - cause */
#define BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_PORT_INTERRUPT_CAUSE_MAC(port) \
        (0x3C090480 + 0x4*(port))

/* TXQ queue: high_crossing for port - mask */
#define BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_PORT_INTERRUPT_MASK_MAC(port) \
        (0x3C090680 + 0x4*(port))

/* TXQ queue: high_crossing for port */
#define BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC(bitIndexInCaller,port)   \
            {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, \
              BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_PORT_INTERRUPT_CAUSE_MAC(port) , \
              BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_PORT_INTERRUPT_MASK_MAC(port)  , \
              prvCpssDrvHwPpPortGroupIsrRead,                    \
              prvCpssDrvHwPpPortGroupIsrWrite,                   \
              PRV_CPSS_BOBCAT2_HIGH_CROSSING_PORT_##port##_HIGH_THRESHOLD_CROSSED_FOR_PORT_QUEUE0_E, \
              PRV_CPSS_BOBCAT2_HIGH_CROSSING_PORT_##port##_HIGH_THRESHOLD_CROSSED_FOR_PORT_QUEUE7_E,\
              FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}



/* TXQ-PFC : pfcPortGroup_OverFlowIntSum  */
#define BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(portGroup)   \
                {portGroup+3, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E001330 + 4*(portGroup), 0x3E001350 + 4*(portGroup),      \
                    prvCpssDrvHwPpPortGroupIsrRead,                                                                                     \
                    prvCpssDrvHwPpPortGroupIsrWrite,                                                                                    \
                    PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_##portGroup##_OVER_FLOW_SUM_PFC_PORT_GROUP##portGroup##_GLOBAL_PFC_TC0_OVERFLOW_E,  \
                    PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_##portGroup##_OVER_FLOW_SUM_PFC_PORT_GROUP##portGroup##_GLOBAL_PFC_TC7_OVERFLOW_E,  \
                    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* TXQ-PFC : pfcPortGroup_UnderFlowIntSum  */
#define BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(portGroup)   \
                {portGroup+12, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E001370 + 4*(portGroup), 0x3E001390 + 4*(portGroup),        \
                    prvCpssDrvHwPpPortGroupIsrRead,                                                                                       \
                    prvCpssDrvHwPpPortGroupIsrWrite,                                                                                      \
                    PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_##portGroup##_UNDER_FLOW_SUM_PFC_PORT_GROUP##portGroup##_GLOBAL_PFC_TC0_UNDERFLOW_E,  \
                    PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_##portGroup##_UNDER_FLOW_SUM_PFC_PORT_GROUP##portGroup##_GLOBAL_PFC_TC7_UNDERFLOW_E,  \
                    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


/* RXDMA : rxDmaScdma %n IntSum  */
#define BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC(bitIndexInCaller,port)   \
             /*rxDmaSum0IntSum*/                                          \
            {bitIndexInCaller, GT_FALSE, PRV_CPSS_DXCH_UNIT_RXDMA_E, NULL, 0x17000908 + 4*(port) , 0x17000D0C + 4*(port),\
                prvCpssDrvHwPpPortGroupIsrRead,                           \
                prvCpssDrvHwPpPortGroupIsrWrite,                          \
                PRV_CPSS_BOBCAT2_RX_DMA_SCDMA_PORT_##port##_SUM_EOP_DESC_FIFO_OVERRUN_E,    \
                PRV_CPSS_BOBCAT2_B0_RX_DMA_SCDMA_PORT_##port##_SUM_OS_DROP_E, \
                FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}


/*
0x10180800 + 0x1000*t: where t (0-55) represents Network PTP
0x10180800 + 0x200000 + 0x1000*t: where t (0-15) represents Fabric PTP from 56 to 71
*/
#define BOBCAT2_PTP_PORT_BASE_MAC(port)                 \
    (((port)< 56) ?                                     \
        (0x10180800 + 0x1000*(port)) :                  \
        (0x10180800 + 0x200000 + 0x1000*((port)-56)))

/* ptp - cause */
#define BOBCAT2_PTP_PORT_INTERRUPT_CAUSE_MAC(port) \
        (BOBCAT2_PTP_PORT_BASE_MAC(port) + (0x00))

/* ptp - mask */
#define BOBCAT2_PTP_PORT_INTERRUPT_MASK_MAC(port) \
        (BOBCAT2_PTP_PORT_BASE_MAC(port) + (0x04))

/* PTP interrupts for giga/XLG port */
#define BOBCAT2_PTP_FOR_PORT_INT_ELEMENT_MAC(bitIndexInCaller,port)   \
            {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL, \
              BOBCAT2_PTP_PORT_INTERRUPT_CAUSE_MAC(port) , \
              BOBCAT2_PTP_PORT_INTERRUPT_MASK_MAC(port)  , \
              prvCpssDrvHwPpPortGroupIsrRead,                    \
              prvCpssDrvHwPpPortGroupIsrWrite,                   \
              PRV_CPSS_BOBCAT2_PTP_PORT_##port##_PTP_RX_FIFO_FULL0_E, \
              PRV_CPSS_BOBCAT2_PTP_PORT_##port##_TIMESTAMP_QUEUE1_FULL_E,\
              FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/*
0x10000000 + 0x200000 + 0x1000*(g-56): where g (56-71) represents Fabric Ports from 56 to 71
0x10000000 + 0x1000*g: where g (0-55) represents Network Ports from 0 to 55
*/
#define BOBCAT2_GIGA_PORT_BASE_MAC(port)                \
    (((port)< 56) ?                                     \
        (0x10000000 + 0x1000*(port)) :                  \
        (0x10000000 + 0x200000 + 0x1000*((port)-56)))

/* gig summary - cause */
#define BOBCAT2_GIGA_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) \
        (BOBCAT2_GIGA_PORT_BASE_MAC(port) + (0xA0))

/* gig summary - mask */
#define BOBCAT2_GIGA_PORT_SUMMARY_INTERRUPT_MASK_MAC(port) \
        (BOBCAT2_GIGA_PORT_BASE_MAC(port) + (0xA4))

/* gig - cause */
#define BOBCAT2_GIGA_PORT_INTERRUPT_CAUSE_MAC(port) \
        (BOBCAT2_GIGA_PORT_BASE_MAC(port) + (0x20))

/* gig - mask */
#define BOBCAT2_GIGA_PORT_INTERRUPT_MASK_MAC(port) \
        (BOBCAT2_GIGA_PORT_BASE_MAC(port) + (0x24))

/* summary of Giga mac that called from MG */
#define BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(bitIndexInCaller,port)   \
                {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL, \
                BOBCAT2_GIGA_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) , \
                BOBCAT2_GIGA_PORT_SUMMARY_INTERRUPT_MASK_MAC(port)  , \
                prvCpssDrvHwPpPortGroupIsrRead,                    \
                prvCpssDrvHwPpPortGroupIsrWrite,                   \
                PRV_CPSS_BOBCAT2_GIGA_PORT_##port##_SUM_INTERNAL_SUM_E, \
                PRV_CPSS_BOBCAT2_GIGA_PORT_##port##_SUM_EXTERNAL_SUM_E,\
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL}, \
                    /* interrupts of the giga mac */   \
                    { 1 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL,  \
                        BOBCAT2_GIGA_PORT_INTERRUPT_CAUSE_MAC(port) ,\
                        BOBCAT2_GIGA_PORT_INTERRUPT_MASK_MAC(port),\
                     prvCpssDrvHwPpPortGroupIsrRead,                          \
                     prvCpssDrvHwPpPortGroupIsrWrite,                         \
                     PRV_CPSS_BOBCAT2_GIGA_PORT_##port##_LINK_STATUS_CHANGED_E,  \
                     PRV_CPSS_BOBCAT2_GIGA_PORT_##port##_MIB_COUNTER_WRAPAROUND_E,  \
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},    \
                        \
                     /*bit 2*/ \
                     BOBCAT2_PTP_FOR_PORT_INT_ELEMENT_MAC(2,port)

/* summary of Giga mac that called from XLG mac */
#define BOBCAT2_GIGA_PORT_SUMMARY_FROM_XLG_INT_SUB_TREE_MAC(port)   \
                {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL, \
                BOBCAT2_GIGA_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) , \
                BOBCAT2_GIGA_PORT_SUMMARY_INTERRUPT_MASK_MAC(port)  , \
                prvCpssDrvHwPpPortGroupIsrRead,                    \
                prvCpssDrvHwPpPortGroupIsrWrite,                   \
                PRV_CPSS_BOBCAT2_GIGA_PORT_##port##_SUM_INTERNAL_SUM_E, \
                PRV_CPSS_BOBCAT2_GIGA_PORT_##port##_SUM_INTERNAL_SUM_E, /*see PTP issue below */ \
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1/*see PTP issue below */,NULL, NULL}, \
                    /* interrupts of the giga mac */   \
                    { 1 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL,  \
                        BOBCAT2_GIGA_PORT_INTERRUPT_CAUSE_MAC(port) ,\
                        BOBCAT2_GIGA_PORT_INTERRUPT_MASK_MAC(port),\
                     prvCpssDrvHwPpPortGroupIsrRead,                          \
                     prvCpssDrvHwPpPortGroupIsrWrite,                         \
                     PRV_CPSS_BOBCAT2_GIGA_PORT_##port##_LINK_STATUS_CHANGED_E,  \
                     PRV_CPSS_BOBCAT2_GIGA_PORT_##port##_MIB_COUNTER_WRAPAROUND_E,  \
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}
                    /* NOTE: PTP issue: bit2 for the PTP not needed here because used from the XLG summary bit 7 */

/*
0x10180000 + 0x1000*k: where k (48-55) represents Network MPCS
0x10180000 + 0x200000 + 0x1000*k: where k (0-15) represents Fabric MPCS from 56 to 71
*/
#define BOBCAT2_MPCS_PORT_BASE_MAC(port)                 \
    (((port)< 56) ?                                     \
        (0x10180000  + 0x1000*(port)) :                  \
        (0x10180000  + 0x200000 + 0x1000*((port)-56)))

/* MPCS - cause */
#define BOBCAT2_MPCS_PORT_INTERRUPT_CAUSE_MAC(port) \
        (BOBCAT2_MPCS_PORT_BASE_MAC(port) + (0x08))

/* MPCS - mask */
#define BOBCAT2_MPCS_PORT_INTERRUPT_MASK_MAC(port) \
        (BOBCAT2_MPCS_PORT_BASE_MAC(port) + (0x0C))


/*
0x100C0000 + 0x200000 + 0x1000*p: where p (0-15) represents Fabric XLG Mac
0x100C0000 + 0x1000*p: where p (48-55) represents Network XLG Mac
*/
#define BOBCAT2_XLG_PORT_BASE_MAC(port)                 \
    (((port)< 56) ?                                     \
        (0x100C0000 + 0x1000*(port)) :                  \
        (0x100C0000 + 0x200000 + 0x1000*((port)-56)))

/* XLG summary - cause */
#define BOBCAT2_XLG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) \
        (BOBCAT2_XLG_PORT_BASE_MAC(port) + (0x58))

/* XLG summary - mask */
#define BOBCAT2_XLG_PORT_SUMMARY_INTERRUPT_MASK_MAC(port) \
        (BOBCAT2_XLG_PORT_BASE_MAC(port) + (0x5c))

/* XLG - cause */
#define BOBCAT2_XLG_PORT_INTERRUPT_CAUSE_MAC(port) \
        (BOBCAT2_XLG_PORT_BASE_MAC(port) + (0x14))

/* XLG - mask */
#define BOBCAT2_XLG_PORT_INTERRUPT_MASK_MAC(port) \
        (BOBCAT2_XLG_PORT_BASE_MAC(port) + (0x18))

/*
0x10180400 + 0x1000*x: where x (48-54 in steps of 2) represents Network XPCS
0x10180400 + 0x200000 + 0x1000*x: where x (0-14 in steps of 2) represents Fabric XPCS from 56 to 71
*/
#define BOBCAT2_XPCS_PORT_BASE_MAC(port)                \
    (((port)< 56) ?                                     \
        (0x10180400 + 0x1000*(port)) :                  \
        (0x10180400 + 0x200000 + 0x1000*((port)-56)))
/* XPCS - cause */
#define BOBCAT2_XPCS_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) \
        (BOBCAT2_XPCS_PORT_BASE_MAC(port) + (0x14))
/* XPCS - mask */
#define BOBCAT2_XPCS_PORT_SUMMARY_INTERRUPT_MASK_MAC(port) \
        (BOBCAT2_XPCS_PORT_BASE_MAC(port) + (0x18))

/* base address of per port per lane registers */
#define BOBCAT2_XPCS_PORT_PER_LANE_BASE_MAC(port,lane)                \
        (BOBCAT2_XPCS_PORT_BASE_MAC(port) + 0x50 + 0x44*(lane))

/* XPCS - per port , per lane - cause */
#define BOBCAT2_XPCS_PORT_LANE_SUMMARY_INTERRUPT_CAUSE_MAC(port,lane) \
        (BOBCAT2_XPCS_PORT_PER_LANE_BASE_MAC(port,lane) + (0x10))

/* XPCS - per port , per lane - mask */
#define BOBCAT2_XPCS_PORT_LANE_SUMMARY_INTERRUPT_MASK_MAC(port,lane) \
        (BOBCAT2_XPCS_PORT_PER_LANE_BASE_MAC(port,lane) + (0x14))

#define BOBCAT2_XPCS_FOR_PORT_INT_SUB_TREE_MAC(bitIndexInCaller,port) \
                {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL, \
                BOBCAT2_XPCS_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) , \
                BOBCAT2_XPCS_PORT_SUMMARY_INTERRUPT_MASK_MAC(port)  , \
                prvCpssDrvHwPpPortGroupIsrRead,                    \
                prvCpssDrvHwPpPortGroupIsrWrite,                   \
                PRV_CPSS_BOBCAT2_XPCS_PORT_##port##_LINK_STATUS_CHANGED_E,\
                PRV_CPSS_BOBCAT2_XPCS_PORT_##port##_SUM_LANE3_E,\
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 4,NULL, NULL},\
                    BOBCAT2_XPCS_FOR_PORT_LANE_INT_ELEMENT_MAC(7 ,port,0),\
                    BOBCAT2_XPCS_FOR_PORT_LANE_INT_ELEMENT_MAC(8 ,port,1),\
                    BOBCAT2_XPCS_FOR_PORT_LANE_INT_ELEMENT_MAC(9 ,port,2),\
                    BOBCAT2_XPCS_FOR_PORT_LANE_INT_ELEMENT_MAC(10,port,3)

/* per port per lane interrupt */
#define BOBCAT2_XPCS_FOR_PORT_LANE_INT_ELEMENT_MAC(bitIndexInCaller,port,lane) \
                {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL, \
                BOBCAT2_XPCS_PORT_LANE_SUMMARY_INTERRUPT_CAUSE_MAC(port,lane) , \
                BOBCAT2_XPCS_PORT_LANE_SUMMARY_INTERRUPT_MASK_MAC(port,lane) , \
                prvCpssDrvHwPpPortGroupIsrRead,                    \
                prvCpssDrvHwPpPortGroupIsrWrite,                   \
                PRV_CPSS_BOBCAT2_XPCS_PORT_##port##_LANE_##lane##_PRBS_ERROR_E,\
                PRV_CPSS_BOBCAT2_XPCS_PORT_##port##_LANE_##lane##_DETECTED_IIAII_E,\
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


/* support XPCS interrupt summary (bit 4) : only even ports have it */
#define XPCS_BIT4_EXISTS_MAC(port)   (1-((port) & 1))

/* even port hold XPCS interrupts too */
#define BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC(bitIndexInCaller,port)   \
                BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC(bitIndexInCaller,port), \
                    /*bit 4*/ \
                    BOBCAT2_XPCS_FOR_PORT_INT_SUB_TREE_MAC(4,port)

#define BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC(bitIndexInCaller,port)   \
                /* XLG - External Units Interrupts cause */                    \
                {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL, \
                BOBCAT2_XLG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) , \
                BOBCAT2_XLG_PORT_SUMMARY_INTERRUPT_MASK_MAC(port)  , \
                prvCpssDrvHwPpPortGroupIsrRead,                    \
                prvCpssDrvHwPpPortGroupIsrWrite,                   \
                PRV_CPSS_BOBCAT2_XLG_PORT_##port##_SUM_XLG_PORT_INTERRUPT_SUMMARY_E,\
                PRV_CPSS_BOBCAT2_XLG_PORT_##port##_SUM_PTP_UNIT_INTERRUPT_SUMMARY_E,\
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 4+(XPCS_BIT4_EXISTS_MAC(port)),NULL, NULL},\
                    /* interrupts of the XLG mac */   \
                    { 1 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL,  \
                    BOBCAT2_XLG_PORT_INTERRUPT_CAUSE_MAC(port) ,\
                    BOBCAT2_XLG_PORT_INTERRUPT_MASK_MAC(port),\
                    prvCpssDrvHwPpPortGroupIsrRead,                          \
                    prvCpssDrvHwPpPortGroupIsrWrite,                         \
                    PRV_CPSS_BOBCAT2_XLG_PORT_##port##_LINK_STATUS_CHANGED_E,  \
                    PRV_CPSS_BOBCAT2_XLG_PORT_##port##_PFC_SYNC_FIFO_OVERRUN_E,  \
                    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
                    /*bit2*/  \
                    BOBCAT2_GIGA_PORT_SUMMARY_FROM_XLG_INT_SUB_TREE_MAC(port),   \
                    /* bit 4 - XPCS - will be added for even ports only */ \
                    /* interrupts of the MPCS mac (also called GB)*/   \
                    { 5 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL,  \
                    BOBCAT2_MPCS_PORT_INTERRUPT_CAUSE_MAC(port) ,\
                    BOBCAT2_MPCS_PORT_INTERRUPT_MASK_MAC(port),\
                    prvCpssDrvHwPpPortGroupIsrRead,                          \
                    prvCpssDrvHwPpPortGroupIsrWrite,                         \
                    PRV_CPSS_BOBCAT2_MPCS_PORT_##port##_SUM__ACCESS_ERROR_E,  \
                    PRV_CPSS_BOBCAT2_MPCS_PORT_##port##_SUM_GB_LOCK_SYNC_CHANGE_E,  \
                    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
                    /*bit 7*/ \
                    BOBCAT2_PTP_FOR_PORT_INT_ELEMENT_MAC(7,port)



#define BOBCAT2_GIGA_PORTS_0_30_SUMMARY_INT_SUB_TREE_MAC    \
                                                      /*bit,port*/  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  1 ,  0 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  2 ,  1 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  3 ,  2 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  4 ,  3 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  5 ,  4 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  6 ,  5 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  7 ,  6 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  8 ,  7 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  9 ,  8 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 10 ,  9 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 11 , 10 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 12 , 11 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 13 , 12 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 14 , 13 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 15 , 14 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 16 , 15 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 17 , 16 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 18 , 17 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 19 , 18 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 20 , 19 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 21 , 20 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 22 , 21 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 23 , 22 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 24 , 23 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 25 , 24 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 26 , 25 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 27 , 26 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 28 , 27 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 29 , 28 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 30 , 29 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 31 , 30 )

#define BOBCAT2_GIGA_PORTS_31_47_SUMMARY_INT_SUB_TREE_MAC    \
                                                      /*bit,port*/   \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  1 ,  31 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  2 ,  32 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  3 ,  33 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  4 ,  34 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  5 ,  35 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  6 ,  36 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  7 ,  37 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  8 ,  38 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC(  9 ,  39 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 10 ,  40 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 11 ,  41 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 12 ,  42 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 13 ,  43 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 14 ,  44 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 15 ,  45 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 16 ,  46 ),  \
            BOBCAT2_GIGA_PORT_SUMMARY_FROM_MG_INT_SUB_TREE_MAC( 17 ,  47 )


#define BOBCAT2_XLG_PORTS_48_55_SUMMARY_INT_SUB_TREE_MAC    \
                                                      /*bit,port*/  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC ( 18 , 48 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  ( 19 , 49 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC ( 20 , 50 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  ( 21 , 51 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC ( 22 , 52 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  ( 23 , 53 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC ( 24 , 54 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  ( 25 , 55 )

#define BOBCAT2_XLG_PORTS_56_71_SUMMARY_INT_SUB_TREE_MAC    \
                                                      /*bit,port*/  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC (  1 , 56 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  (  2 , 57 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC (  3 , 58 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  (  4 , 59 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC (  5 , 60 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  (  6 , 61 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC (  7 , 62 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  (  8 , 63 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC (  9 , 64 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  ( 10 , 65 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC ( 11 , 66 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  ( 12 , 67 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC ( 13 , 68 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  ( 14 , 69 ),  \
            BOBCAT2_XLG_EVEN_PORT_SUMMARY_INT_SUB_TREE_MAC ( 15 , 70 ),  \
            BOBCAT2_XLG_ODD_PORT_SUMMARY_INT_SUB_TREE_MAC  ( 16 , 71 )

/*
0x10180A00 + 0x1000*(z+o*16): where z (0-1) represents two TAIs, where o (0-2) represents group of ports served by those TAIs
0x10180A00 + 0x1000*(z+o*4+48): where z (0-1) represents two TAIs, where o (0-1) represents group of ports served by those TAIs
0x10180A00 + 0x200000 + 0x1000*(z+o*4): where z (0-1) represents two TAIs, where o (0-3) represents group of ports served by those TAIs
*/
#define BOBCAT2_TAI_PORT_BASE_MAC(port,taiId)           \
    ((((port)< 56) ?                                    \
        (0x10180A00 + 0x1000*(port)) :                  \
        (0x10180A00 + 0x200000 + 0x1000*((port)-56)))   \
    + 0x1000 * (taiId))

/* TAI - cause */
#define BOBCAT2_TAI_PORT_INTERRUPT_CAUSE_MAC(port,taiId) \
        (BOBCAT2_TAI_PORT_BASE_MAC(port,taiId) + (0x00))

/* TAI - mask */
#define BOBCAT2_TAI_PORT_INTERRUPT_MASK_MAC(port,taiId) \
        (BOBCAT2_TAI_PORT_BASE_MAC(port,taiId) + (0x04))


                                           /*bitIndex, port , taiId(0/1) */
#define BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(bitIndex , port ,taiId)       \
    /* TAI  */                                                             \
    {bitIndex, GT_FALSE,PRV_CPSS_DXCH_UNIT_GOP_E, NULL,                    \
        BOBCAT2_TAI_PORT_INTERRUPT_CAUSE_MAC(port,taiId) ,                     \
        BOBCAT2_TAI_PORT_INTERRUPT_MASK_MAC(port,taiId),                       \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_TAI_PORT_##port##_TAI_##taiId##_GENERATION_INT_E,     \
        PRV_CPSS_BOBCAT2_TAI_PORT_##port##_TAI_##taiId##_FREQUENCY_DRIFT_E,    \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   TTI_BIT_CNS   1
#define   tti_SUB_TREE_MAC(bit)                                   \
    /* tti  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TTI_E, NULL, 0x01000004, 0x01000008,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_TTI_SUM_CPU_ADDRESS_OUT_OF_RANGE_E,                   \
        PRV_CPSS_BOBCAT2_TTI_SUM_TTI1_ACCESS_DATA_ERROR_E,                     \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   bc2_B0_tti_SUB_TREE_MAC(bit)                                   \
    /* tti  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TTI_E, NULL, 0x01000004, 0x01000008,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_B0_TTI_SUM_CPU_ADDRESS_OUT_OF_RANGE_E,                \
        PRV_CPSS_BOBCAT2_B0_TTI_SUM_FIRST_CRITICAL_ECC_ONE_ERROR_CORRECTED_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* when used in 'causeRegAddr' indicate that entry in table of 'interrupts tree'
   is 'place holder' */
#define PLACE_HOLDER_ENTRY_CAUSE_REG_ADDR_CNS   0xFFFFFFFF
/* when used in 'oldCauseRegAddr' indicate that table of 'override' is ended */
#define LAST_ENTRY_CAUSE_REG_ADDR_CNS           0xFFFFFFFE

#define LAST_ENTRY_INT_CAUSE_CNS    0xFFFFFFFF
#define LAST_ENTRY_DRV_INTERRUPT_SCAN_OVERRIDE_INFO_CNS     LAST_ENTRY_INT_CAUSE_CNS,{0}
/* structure to allow 'override' of element in the interrupts tree by other element.
   this is useful for new silicon revision that relay on previous revision with
   'minor' changes in the interrupts tree
*/
typedef struct{
    GT_U32                      oldIntCause;/*compared to startIdx*/
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC newInfo;
}PRV_CPSS_DRV_INTERRUPT_SCAN_OVERRIDE_INFO_STC;


#define   pcl_SUB_TREE_MAC(bit)                                   \
    /* pcl  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_PCL_E, NULL, 0x02000004, 0x02000008,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_IPCL_SUM_MG_ADDR_OUT_OF_RANGE_E,                      \
        PRV_CPSS_BOBCAT2_IPCL_SUM_LOOKUP2_FIFO_FULL_E,                         \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   l2i_SUB_TREE_MAC(bit)                                   \
    /* L2 Bridge  */                                                           \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_L2I_E, NULL, 0x03002100, 0x03002104,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_BRIDGE_ADDRESS_OUT_OF_RANGE_E,                        \
        PRV_CPSS_BOBCAT2_BRIDGE_UPDATE_SECURITY_BREACH_E,                      \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   fdb_SUB_TREE_MAC(bit)                                   \
    /* FDBIntSum - FDB Interrupt Cause,                                      */\
    /*   maskRcvIntrEn - both AUProcCompletedInt(bit of PRV_CPSS_BOBCAT2_FDB_AU_PROC_COMPLETED_E) and AgeViaTriggerEnded(bit of PRV_CPSS_BOBCAT2_FDB_AGE_VIA_TRIGGER_ENDED_E)*/\
    /*   should never be masked to avoid missed events situation.  */          \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_FDB_E, NULL, 0x040001b0, 0x040001b4,       \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_FDB_NUM_OF_HOP_EX_P_E,                                \
        PRV_CPSS_BOBCAT2_FDB_ADDRESS_OUT_OF_RANGE_E,                           \
        FILLED_IN_RUNTIME_CNS, 0x0, FDB_REG_MASK_CNS,                          \
             0,NULL, NULL}

#define   tcam_SUB_TREE_MAC(bit)                                               \
    /* tcam  */                                                                \
    {5, GT_FALSE,PRV_CPSS_DXCH_UNIT_TCAM_E, NULL, 0x05501004, 0x05501000,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_TCAM_SUM_TCAM_LOGIC_SUMMARY_E,                        \
        PRV_CPSS_BOBCAT2_TCAM_SUM_TCAM_LOGIC_SUMMARY_E,                        \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1,NULL, NULL},                 \
        /* tcam Logic*/                                                        \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_TCAM_E, NULL, 0x0550100c, 0x05501008,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                    \
            prvCpssDrvHwPpPortGroupIsrWrite,                                   \
            PRV_CPSS_BOBCAT2_TCAM_LOGIC_SUM_CPU_ADDRESS_OUT_OF_RANGE_E,        \
            PRV_CPSS_BOBCAT2_TCAM_LOGIC_SUM_TCAM_ARRAY_PARITY_ERROR_E,         \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   router_SUB_TREE_MAC(bit)                                               \
    /* router  */                                                              \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_IPVX_E, NULL, 0x06000970, 0x06000974,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_ROUTER_I_PV_X_BAD_ADDR_E,                             \
        PRV_CPSS_BOBCAT2_ROUTER_STG13_SIPSA_RETURNS_LFIFO_FULL_E,              \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define ETA_SUB_TREE_MAC(bit,instance)                                         \
    /* same address space as LPM */                                            \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_LPM_E, NULL,                             \
        0x58E00104 + (0x00100000 * (instance)),                                \
        0x58E00100 + (0x00100000 * (instance)),                                \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_LPM_ETA_##instance##_ETA_WRONG_ADDRESS_E,             \
        PRV_CPSS_BOBCAT2_LPM_ETA_##instance##_ETA_WRONG_ADDRESS_E,             \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


#define   lpm_SUB_TREE_MAC(bit)                                               \
    /* LPM  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_LPM_E, NULL, 0x58D00100, 0x58D00110,       \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_LPM_LPM_GENERAL_SUM_E,                                \
        PRV_CPSS_BOBCAT2_LPM_ETA_1_SUM_E,                                      \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 3,NULL, NULL},                 \
        /* LPM general*/                                                       \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_LPM_E, NULL, 0x58D00120, 0x58D00130,   \
            prvCpssDrvHwPpPortGroupIsrRead,                                    \
            prvCpssDrvHwPpPortGroupIsrWrite,                                   \
            PRV_CPSS_BOBCAT2_LPM_GENERAL_LPM_WRONG_ADDR_E,                     \
            PRV_CPSS_BOBCAT2_LPM_GENERAL_AGING_CACHE_FULL_E,                   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},             \
        /* bit 2 , instance 0 */                                               \
        ETA_SUB_TREE_MAC(2,0),                                                 \
        /* bit 3 , instance 1 */                                               \
        ETA_SUB_TREE_MAC(3,1)

#define   iplr0_SUB_TREE_MAC(bit)                                               \
    /* iplr0  */                                                               \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_IPLR_E, NULL, 0x0B000200, 0x0B000204,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_IPLR0_DATA_ERROR_E,                                   \
        PRV_CPSS_BOBCAT2_IPLR0_IPFIX_ALARMED_ENTRIES_FIFO_WRAPAROUND_E,        \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


#define   eqIngressStc_SUB_TREE_MAC(bit)                                               \
    /* Ingress STC  */                                                     \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EQ_E, NULL, 0x0d000600, 0x0d000604,    \
         prvCpssDrvHwPpPortGroupIsrRead,                                   \
         prvCpssDrvHwPpPortGroupIsrWrite,                                  \
        PRV_CPSS_BOBCAT2_ING_STC_SUM_REG0_INGRESS_SAMPLE_LOADED_E,         \
        PRV_CPSS_BOBCAT2_ING_STC_SUM_REG15_INGRESS_SAMPLE_LOADED_E,        \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 16,NULL, NULL},            \
                                                                           \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(0),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(1),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(2),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(3),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(4),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(5),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(6),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(7),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(8),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(9),                              \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(10),                             \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(11),                             \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(12),                             \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(13),                             \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(14),                             \
        INGRESS_STC_NODE_FOR_16_PORTS_MAC(15)

#define   eqSctRateLimiters_SUB_TREE_MAC(bit)                                               \
        /* SCT Rate Limiters */                                                \
        {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EQ_E, NULL, 0x0D010000, 0x0D010004,    \
             prvCpssDrvHwPpPortGroupIsrRead,                                   \
             prvCpssDrvHwPpPortGroupIsrWrite,                                  \
            PRV_CPSS_BOBCAT2_SCT_RATE_LIMITERS_REG0_CPU_CODE_RATE_LIMITER_E,   \
            PRV_CPSS_BOBCAT2_SCT_RATE_LIMITERS_REG15_CPU_CODE_RATE_LIMITER_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 16,NULL, NULL},            \
                                                                               \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(0),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(1),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(2),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(3),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(4),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(5),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(6),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(7),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(8),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(9),                        \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(10),                       \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(11),                       \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(12),                       \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(13),                       \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(14),                       \
            SCT_RATE_LIMITERS_NODE_FOR_16_PORTS_MAC(15)


#define   eq_SUB_TREE_MAC(bit)                                               \
    /* Pre-Egress Interrupt Summary Cause */                                   \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EQ_E, NULL, 0x0D000058, 0x0D00005C,        \
         prvCpssDrvHwPpPortGroupIsrRead,                                       \
         prvCpssDrvHwPpPortGroupIsrWrite,                                      \
        PRV_CPSS_BOBCAT2_EQ_INGRESS_STC_E,                                     \
        PRV_CPSS_BOBCAT2_EQ_CRITICAL_ECC_ERROR_E,                              \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 3,NULL, NULL},                 \
                                                                               \
        eqIngressStc_SUB_TREE_MAC(1),                                          \
        eqSctRateLimiters_SUB_TREE_MAC(2),                                     \
                                                                               \
        /* Critical ECC Error Int  */                                          \
        {3, GT_FALSE,PRV_CPSS_DXCH_UNIT_EQ_E, NULL, 0x0D0000A0 , 0x0D0000A4 ,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                    \
            prvCpssDrvHwPpPortGroupIsrWrite,                                   \
            PRV_CPSS_BOBCAT2_CRITICAL_ECC_ERROR_CLEAR_DESC_CRITICAL_ECC_ONE_ERROR_E,     \
            PRV_CPSS_BOBCAT2_CRITICAL_ECC_ERROR_INCREMENT_DESC_CRITICAL_ECC_TWO_ERROR_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   eplr_SUB_TREE_MAC(bit)                                               \
    /* eplr  */                                                                \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EPLR_E, NULL, 0x0E000200, 0x0E000204,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_EPLR_SUM_DATA_ERROR_E,                                \
        PRV_CPSS_BOBCAT2_EPLR_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAPAROUND_E,     \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   ha_SUB_TREE_MAC(bit)                                               \
   /* HA  */                                                                  \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_HA_E, NULL, 0x0F000300, 0x0F000304,       \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_HA_SUM_REGFILE_ADDRESS_ERROR_E,                       \
        PRV_CPSS_BOBCAT2_B0_HA_SUM_OVERSIZE_HEADER_SIZE_E,                     \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   epcl_SUB_TREE_MAC(bit)                                               \
    /* EPCL  */                                                                \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EPCL_E, NULL, 0x14000010, 0x14000014,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_EPCL_SUM_REGFILE_ADDRESS_ERROR_E,                     \
        PRV_CPSS_BOBCAT2_EPCL_SUM_REGFILE_ADDRESS_ERROR_E,                     \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


#define   ermrk_SUB_TREE_MAC(bit)                                               \
    /* ERMRK  */                                                               \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_ERMRK_E, NULL, 0x15000004, 0x15000008,    \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_ERMRK_SUM_REGFILE_ADDRESS_ERROR_E,                    \
        PRV_CPSS_BOBCAT2_ERMRK_SUM_EGRESS_TIMESTAMP_QUEUE1_FULL_E,             \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   eoam_SUB_TREE_MAC(bit)                                               \
    /* egress OAM  */                                                          \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EOAM_E, NULL, 0x1E0000F0, 0x1E0000F4,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_EGR_OAM_SUM_KEEP_ALIVE_AGING_E,                       \
        PRV_CPSS_BOBCAT2_EGR_OAM_SUM_TX_PERIOD_E,                              \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   tmFcu_SUB_TREE_MAC(bit)                                               \
    /* TM - fcu  */                                                            \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_FCU_E, NULL, 0x08000010, 0x08000014,   \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_TM_FCU_SUM_ETHERNET_RX_FIFO_OVERRUN_E,                \
        PRV_CPSS_BOBCAT2_TM_FCU_SUM_FC_QUEUE_COUNTER_UNDERRUN_E,               \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   ioam_SUB_TREE_MAC(bit)                                               \
    /* ingress OAM  */                                                         \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_IOAM_E, NULL, 0x1C0000F0, 0x1C0000F4,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_INGR_OAM_SUM_KEEP_ALIVE_AGING_E,                      \
        PRV_CPSS_BOBCAT2_INGR_OAM_SUM_TX_PERIOD_E,                             \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   mll_SUB_TREE_MAC(bit)                                               \
    /* MLL  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_MLL_E, NULL, 0x1D000030, 0x1D000034,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_MLL_SUM_INVALID_IP_MLL_ACCESS_E,                      \
        PRV_CPSS_BOBCAT2_MLL_SUM__FILE_ECC_2_ERROR_E,                          \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   cnc_SUB_TREE_MAC(bit,instance)                                               \
    /* CNC-0  */                                                               \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_CNC_##instance##_E, NULL,                           \
        0x2E000100 + ((instance)*0x01000000),                                  \
        0x2E000104 + ((instance)*0x01000000),                                  \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_CNC##instance##_SUM_WRAPAROUND_FUNC_SUM_E,                       \
        PRV_CPSS_BOBCAT2_CNC##instance##_SUM_MISC_FUNC_SUM_E,                             \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 3,NULL, NULL},                 \
                                                                               \
        /* WraparoundFuncInterruptSum  */                                      \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_CNC_##instance##_E, NULL,  \
            0x2E000190 + ((instance)*0x01000000),                                  \
            0x2E0001A4 + ((instance)*0x01000000),                                  \
            prvCpssDrvHwPpPortGroupIsrRead,                                    \
            prvCpssDrvHwPpPortGroupIsrWrite,                                   \
            PRV_CPSS_BOBCAT2_CNC##instance##_WRAPAROUND_SUM_BLOCK0_WRAPAROUND_E,          \
            PRV_CPSS_BOBCAT2_CNC##instance##_WRAPAROUND_SUM_BLOCK15_WRAPAROUND_E,         \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},             \
        /* RateLimitFuncInterruptSum  */                                       \
        {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_CNC_##instance##_E, NULL,  \
            0x2E0001B8 + ((instance)*0x01000000),                                  \
            0x2E0001CC + ((instance)*0x01000000),                                  \
            prvCpssDrvHwPpPortGroupIsrRead,                                    \
            prvCpssDrvHwPpPortGroupIsrWrite,                                   \
            PRV_CPSS_BOBCAT2_CNC##instance##_RATE_LIMIT_SUM_BLOCK0_RATE_LIMIT_FIFO_DROP_E,  \
            PRV_CPSS_BOBCAT2_CNC##instance##_RATE_LIMIT_SUM_BLOCK15_RATE_LIMIT_FIFO_DROP_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},             \
        /* MiscFuncInterruptSum  */                                            \
        {3, GT_FALSE,PRV_CPSS_DXCH_UNIT_CNC_##instance##_E, NULL, \
            0x2E0001E0 + ((instance)*0x01000000),                                  \
            0x2E0001E4 + ((instance)*0x01000000),                                  \
            prvCpssDrvHwPpPortGroupIsrRead,                                    \
            prvCpssDrvHwPpPortGroupIsrWrite,                                   \
            PRV_CPSS_BOBCAT2_CNC##instance##_MISC_SUM_DUMP_FINISHED_E,                    \
            PRV_CPSS_BOBCAT2_CNC##instance##_MISC_SUM_CNC_UPDATE_LOST_E,                  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


#define   iplr1_SUB_TREE_MAC(bit)                                               \
    /* iplr1  */                                                               \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_IPLR_1_E, NULL, 0x20000200, 0x20000204,  \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_IPLR1_SUM_DATA_ERROR_E,                               \
        PRV_CPSS_BOBCAT2_IPLR1_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAPAROUND_E,    \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   eft_SUB_TREE_MAC(bit)                                               \
    /* eft  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EGF_EFT_E, NULL, 0x350010A0, 0x350010B0,  \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_EFT_SUM_EGRESS_WRONG_ADDR_E,                          \
        PRV_CPSS_BOBCAT2_EFT_SUM_INC_BUS_IS_TOO_SMALL_INT_E,                   \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   sht_SUB_TREE_MAC(bit)                                               \
    /* sht  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EGF_SHT_E, NULL, 0x4E020010, 0x4E020020,  \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_SHT_SUM_SHT_WRONG_ADDR_E,                             \
        PRV_CPSS_BOBCAT2_B0_SHT_SUM_SPANNING_TREE_STATE_TABLE_WRONG_ADDRESS_E, \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   qag_SUB_TREE_MAC(bit)                                               \
    /* QAG  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_EGF_QAG_E, NULL, 0x3BF00010, 0x3BF00020,  \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_QAG_SUM_QAG_WRONG_ADDR_E,                             \
        PRV_CPSS_BOBCAT2_B0_QAG_SUM_EPORT_ATTRIBUTES_TABLE_WRONG_ADDRESS_E,    \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


#define   txqQueue_SUB_TREE_MAC(bit)                                               \
    /* TXQ-queue  */                                                           \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090000, 0x3C090004,\
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_TXQ_SUM_GENERAL1_SUM_E,                               \
        PRV_CPSS_BOBCAT2_TXQ_SUM_GENERAL2_SUM_E,                               \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 13,NULL, NULL},                \
                                                                               \
        /* TXQ-queue : txqGenIntSum  */                                        \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090008, 0x3C09000c,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXQ_GEN_SUM_GLOBAL_DESC_FULL_E,                        \
            PRV_CPSS_BOBCAT2_TXQ_GEN_SUM_TD_CLR_ECC_TWO_ERROR_DETECTED_E,           \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                  \
        /* TXQ-queue : txqPortDesc0IntSum  */                                       \
        {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090040, 0x3C090044,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXQ_PORT_DESC_SUM_DESC_FULL_PORT_0_E,                  \
            PRV_CPSS_BOBCAT2_TXQ_PORT_DESC_SUM_DESC_FULL_PORT_30_E,                 \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                  \
        /* TXQ-queue : txqPortDesc1IntSum  */                                       \
        {3, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090048, 0x3C09004C,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXQ_PORT_DESC_SUM_DESC_FULL_PORT_31_E,                 \
            PRV_CPSS_BOBCAT2_TXQ_PORT_DESC_SUM_DESC_FULL_PORT_61_E,                 \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                  \
        /* TXQ-queue : txqPortDesc2IntSum  */                                       \
        {4, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090050, 0x3C090054,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXQ_PORT_DESC_SUM_DESC_FULL_PORT_62_E,                 \
            PRV_CPSS_BOBCAT2_TXQ_PORT_DESC_SUM_DESC_FULL_PORT_71_E,                 \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                  \
        /* TXQ-queue : txqPortBuff0IntSum  */                                       \
        {5, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090060, 0x3C090064,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXQ_PORT_BUFF0_SUM_BUFF_FULL_PORT0_E,                  \
            PRV_CPSS_BOBCAT2_TXQ_PORT_BUFF0_SUM_BUFF_FULL_PORT30_E,                 \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                  \
        /* TXQ-queue : txqPortBuff1IntSum  */                                       \
        {6, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090068, 0x3C09006C,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXQ_PORT_BUFF1_SUM_BUFF_FULL_PORT31_E,                 \
            PRV_CPSS_BOBCAT2_TXQ_PORT_BUFF1_SUM_BUFF_FULL_PORT61_E,                 \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                  \
        /* TXQ-queue : txqPortBuff2IntSum  */                                       \
        {7, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090070, 0x3C090074,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXQ_PORT_BUFF2_SUM_BUFF_FULL_PORT62_E,                 \
            PRV_CPSS_BOBCAT2_TXQ_PORT_BUFF2_SUM_BUFF_FULL_PORT71_E,                 \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                  \
        /* TXQ-queue : Low_Crossing_Int_Sum_Ports_30_to_0*/                         \
        {8, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090880, 0x3C090884,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_LOW_CROSSING_SUM_PORTS_30_TO_0_LOW_THRESHOLD_CROSSED_PORT0_SUM_E,  \
            PRV_CPSS_BOBCAT2_LOW_CROSSING_SUM_PORTS_30_TO_0_LOW_THRESHOLD_CROSSED_PORT30_SUM_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 31,NULL, NULL},                             \
                                                                  /* bit */ /*port*/            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 1 ,   0  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 2 ,   1  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 3 ,   2  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 4 ,   3  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 5 ,   4  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 6 ,   5  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 7 ,   6  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 8 ,   7  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 9 ,   8  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 10,   9  ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 11,   10 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 12,   11 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 13,   12 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 14,   13 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 15,   14 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 16,   15 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 17,   16 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 18,   17 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 19,   18 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 20,   19 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 21,   20 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 22,   21 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 23,   22 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 24,   23 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 25,   24 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 26,   25 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 27,   26 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 28,   27 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 29,   28 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 30,   29 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 31,   30 ),            \
        /* TXQ-queue : Low_Crossing_Int_Sum_Ports_61_to_31*/                                    \
        {9, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090888, 0x3C09088C,              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                    \
            PRV_CPSS_BOBCAT2_LOW_CROSSING_SUM_PORTS_61_TO_31_LOW_THRESHOLD_CROSSED_PORT31_SUM_E,\
            PRV_CPSS_BOBCAT2_LOW_CROSSING_SUM_PORTS_61_TO_31_LOW_THRESHOLD_CROSSED_PORT61_SUM_E,\
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 31,NULL, NULL},                             \
                                                                  /* bit */ /*port*/            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 1 ,   31 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 2 ,   32 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 3 ,   33 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 4 ,   34 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 5 ,   35 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 6 ,   36 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 7 ,   37 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 8 ,   38 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 9 ,   39 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 10,   40 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 11,   41 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 12,   42 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 13,   43 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 14,   44 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 15,   45 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 16,   46 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 17,   47 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 18,   48 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 19,   49 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 20,   50 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 21,   51 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 22,   52 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 23,   53 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 24,   54 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 25,   55 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 26,   56 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 27,   57 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 28,   58 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 29,   59 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 30,   60 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 31,   61 ),            \
                                                                                                \
        /* TXQ-queue : Low_Crossing_Int_Sum_Ports_71_to_62*/                                    \
        {10, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090890, 0x3C090894,             \
            prvCpssDrvHwPpPortGroupIsrRead,                                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                    \
            PRV_CPSS_BOBCAT2_LOW_CROSSING_SUM_PORTS_71_TO_62_LOW_THRESHOLD_CROSSED_PORT62_SUM_E,\
            PRV_CPSS_BOBCAT2_LOW_CROSSING_SUM_PORTS_71_TO_62_LOW_THRESHOLD_CROSSED_PORT71_SUM_E,\
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 10,NULL, NULL},                             \
                                                                  /* bit */ /*port*/            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 1 ,   62 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 2 ,   63 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 3 ,   64 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 4 ,   65 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 5 ,   66 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 6 ,   67 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 7 ,   68 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 8 ,   69 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 9 ,   70 ),            \
                BOBCAT2_TXQ_QUEUE_LOW_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 10,   71 ),            \
                                                                                                \
        /* TXQ-queue : High_Crossing_Int_Sum_Ports_30_to_0*/                                    \
        {11, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C090898, 0x3C09089c,             \
            prvCpssDrvHwPpPortGroupIsrRead,                                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                    \
            PRV_CPSS_BOBCAT2_HIGH_CROSSING_SUM_PORTS_30_TO_0_HIGH_THRESHOLD_CROSSED_PORT0_SUM_E,  \
            PRV_CPSS_BOBCAT2_HIGH_CROSSING_SUM_PORTS_30_TO_0_HIGH_THRESHOLD_CROSSED_PORT30_SUM_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 31,NULL, NULL},                               \
                                                                  /* bit */ /*port*/              \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 1 ,   0  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 2 ,   1  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 3 ,   2  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 4 ,   3  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 5 ,   4  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 6 ,   5  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 7 ,   6  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 8 ,   7  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 9 ,   8  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 10,   9  ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 11,   10 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 12,   11 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 13,   12 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 14,   13 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 15,   14 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 16,   15 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 17,   16 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 18,   17 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 19,   18 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 20,   19 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 21,   20 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 22,   21 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 23,   22 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 24,   23 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 25,   24 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 26,   25 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 27,   26 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 28,   27 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 29,   28 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 30,   29 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 31,   30 ),             \
        /* TXQ-queue : High_Crossing_Int_Sum_Ports_61_to_31*/                                     \
        {12, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C0908a0, 0x3C0908a4,               \
            prvCpssDrvHwPpPortGroupIsrRead,                                                       \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                      \
            PRV_CPSS_BOBCAT2_HIGH_CROSSING_SUM_PORTS_61_TO_31_HIGH_THRESHOLD_CROSSED_PORT31_SUM_E,\
            PRV_CPSS_BOBCAT2_HIGH_CROSSING_SUM_PORTS_61_TO_31_HIGH_THRESHOLD_CROSSED_PORT61_SUM_E,\
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 31,NULL, NULL},                               \
                                                                  /* bit */ /*port*/              \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 1 ,   31 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 2 ,   32 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 3 ,   33 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 4 ,   34 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 5 ,   35 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 6 ,   36 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 7 ,   37 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 8 ,   38 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 9 ,   39 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 10,   40 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 11,   41 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 12,   42 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 13,   43 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 14,   44 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 15,   45 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 16,   46 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 17,   47 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 18,   48 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 19,   49 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 20,   50 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 21,   51 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 22,   52 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 23,   53 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 24,   54 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 25,   55 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 26,   56 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 27,   57 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 28,   58 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 29,   59 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 30,   60 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 31,   61 ),             \
        /* TXQ-queue : High_Crossing_Int_Sum_Ports_71_to_62*/                                     \
        {13, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E, NULL, 0x3C0908a8, 0x3C0908ac,               \
            prvCpssDrvHwPpPortGroupIsrRead,                                                       \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                      \
            PRV_CPSS_BOBCAT2_HIGH_CROSSING_SUM_PORTS_71_TO_62_HIGH_THRESHOLD_CROSSED_PORT62_SUM_E,\
            PRV_CPSS_BOBCAT2_HIGH_CROSSING_SUM_PORTS_71_TO_62_HIGH_THRESHOLD_CROSSED_PORT71_SUM_E,\
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 10,NULL, NULL},                               \
                                                                  /* bit */ /*port*/              \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 1 ,   62 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 2 ,   63 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 3 ,   64 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 4 ,   65 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 5 ,   66 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 6 ,   67 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 7 ,   68 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 8 ,   69 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 9 ,   70 ),             \
                BOBCAT2_TXQ_QUEUE_HIGH_CROSSING_FOR_PORT_INT_ELEMENT_MAC( 10,   71 )

/* new node in B0 that not exists in A0 */
#define   bc2_B0_txqLL_single_ECC_Error_SUB_TREE_MAC(bit)                             \
    /* LL_ECC_Error -- not in A0 */                                            \
    {bit, GT_FALSE,(NON_A0_SUB_TREE_CNS | PRV_CPSS_DXCH_UNIT_TXQ_LL_E), NULL, 0x3d0a0510, 0x3d0a0514, \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_B0_LL_SINGLE_ECC_ERROR_ECC_SINGLE_ERROR_BANK_0_E,     \
        PRV_CPSS_BOBCAT2_B0_LL_SINGLE_ECC_ERROR_ECC_SINGLE_ERROR_BANK_15_E,    \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* new node in B0 that not exists in A0 */
#define   bc2_B0_txqLL_double_ECC_Error_SUB_TREE_MAC(bit)                             \
    /* LL_ECC_Error -- not in A0 */                                            \
    {bit, GT_FALSE,(NON_A0_SUB_TREE_CNS | PRV_CPSS_DXCH_UNIT_TXQ_LL_E), NULL, 0x3d0a0518, 0x3d0a051c, \
        prvCpssDrvHwPpPortGroupIsrRead,                                        \
        prvCpssDrvHwPpPortGroupIsrWrite,                                       \
        PRV_CPSS_BOBCAT2_LL_DOUBLE_ECC_ERROR_ECC_DOUBLE_ERROR_BANK_0_E,     \
        PRV_CPSS_BOBCAT2_LL_DOUBLE_ECC_ERROR_ECC_DOUBLE_ERROR_BANK_15_E,    \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   txqLl_NODE_MAC(bit)                                               \
    /* TXQ-LL  */                                                                  \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_LL_E, NULL, 0x3D0A0508, 0x3D0A050c,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                            \
        prvCpssDrvHwPpPortGroupIsrWrite,                                           \
        PRV_CPSS_BOBCAT2_LL_SUM_ID_FIFO_OVERRUN_E,                                 \
        PRV_CPSS_BOBCAT2_LL_SUM_LL_WRONG_ADDR_E,                                   \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define TXQLL_BIT_CNS   26

#define   bc2_B0_txqLl_SUB_TREE_MAC(bit)                                           \
    /* TXQ-LL  */                                                                  \
    {bit, GT_FALSE,(B0_TXQ_LL_TO_REPLACE_WITH_A0_SUB_TREE_CNS | PRV_CPSS_DXCH_UNIT_TXQ_LL_E), NULL, 0x3D0A0520, 0x3D0A0524,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                            \
        prvCpssDrvHwPpPortGroupIsrWrite,                                           \
        PRV_CPSS_BOBCAT2_B0_LL_SUMMARY_SUM_LL_SUM_E,                               \
        PRV_CPSS_BOBCAT2_B0_LL_SUMMARY_SUM_ECC_DOUBLE_SUMMARY_E,                   \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 3,NULL, NULL},                     \
        txqLl_NODE_MAC(1),                                                         \
        bc2_B0_txqLL_single_ECC_Error_SUB_TREE_MAC(2),                             \
        bc2_B0_txqLL_double_ECC_Error_SUB_TREE_MAC(3)


static PRV_CPSS_DRV_INTERRUPT_SCAN_STC txqLl_NODE_arr[] = {
    txqLl_NODE_MAC(TXQLL_BIT_CNS)
};
static GT_U32   txqLl_NODE_numEntries = sizeof(txqLl_NODE_arr)/sizeof(txqLl_NODE_arr[0]);

static PRV_CPSS_DRV_INTERRUPT_SCAN_STC bc2_B0_txqLl_SUB_TREE_arr[] = {
    bc2_B0_txqLl_SUB_TREE_MAC(TXQLL_BIT_CNS)
};
static GT_U32   bc2_B0_txqLl_SUB_TREE_numEntries = sizeof(bc2_B0_txqLl_SUB_TREE_arr)/sizeof(bc2_B0_txqLl_SUB_TREE_arr[0]);

#define TXQPFC_BIT_CNS  27

/* this is A0 format of the sub tree , that in compilation time is in the 'tree'.
   so if B0 initialize it need replace it with B0 sub tree  */
#define   txqPfc_SUB_TREE_MAC(bit)                                               \
    /* TXQ-PFC  */                                                                 \
    {bit, GT_FALSE,(A0_TXQ_PFC_TO_REPLACE_WITH_B0_SUB_TREE_CNS | PRV_CPSS_DXCH_UNIT_TXQ_PFC_E), NULL, 0x3E001300, 0x3E001304,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                            \
        prvCpssDrvHwPpPortGroupIsrWrite,                                           \
        PRV_CPSS_BOBCAT2_PFC_SUM_PFC_COUNTERS_PARITY_SUM_E,                        \
        PRV_CPSS_BOBCAT2_PFC_SUM_PFC_MSGS_SETS_OVERRUNS_EGR7_41_SUM_E,             \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 23,NULL, NULL},                    \
                                                                                   \
        /* TXQ-PFC : PfcCountersParityIntSum  */                                   \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E001310, 0x3E001314,   \
            prvCpssDrvHwPpPortGroupIsrRead,                                        \
            prvCpssDrvHwPpPortGroupIsrWrite,                                       \
            PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP0_COUNTERS_PARITY_ERR_E, \
            PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_WRONG_ADDR_E,                      \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                 \
        /* TXQ-PFC : pfcCntrOverFlowIntSum  */                                     \
        {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E001320, 0x3E001324,   \
            prvCpssDrvHwPpPortGroupIsrRead,                                        \
            prvCpssDrvHwPpPortGroupIsrWrite,                                       \
            PRV_CPSS_BOBCAT2_PFC_CNTR_OVER_FLOW_SUM_PFC_PORT_GROUP0_OVERFLOW_E,    \
            PRV_CPSS_BOBCAT2_PFC_CNTR_OVER_FLOW_SUM_PFC_PORT_GROUP7_UNDERFLOW_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                 \
                                                                                   \
        /*3*/    BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(0),    \
        /*4*/    BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(1),    \
        /*5*/    BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(2),    \
        /*6*/    BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(3),    \
        /*7*/    BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(4),    \
        /*8*/    BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(5),    \
        /*9*/    BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(6),    \
        /*10*/   BOBCAT2_TXQ_QUEUE_PFC_OVER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(7),    \
                                                                                   \
        /*12*/   BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(0),   \
        /*13*/   BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(1),   \
        /*14*/   BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(2),   \
        /*15*/   BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(3),   \
        /*16*/   BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(4),   \
        /*17*/   BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(5),   \
        /*18*/   BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(6),   \
        /*19*/   BOBCAT2_TXQ_QUEUE_PFC_UNDER_FLOW_PORT_GROUP_INT_ELEMENT_MAC(7),   \
                                                                                   \
        /* TXQ-PFC : PfcFifosOverrunsIntSum */                                     \
        {20, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E0013B0, 0x3E0013B4,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                        \
            prvCpssDrvHwPpPortGroupIsrWrite,                                       \
            PRV_CPSS_BOBCAT2_PFC_FIFO_SUM_PFC_PORT_GROUP0_EQ_FIFO_OVERRUN_E,       \
            PRV_CPSS_BOBCAT2_PFC_FIFO_SUM_PFC_PORT_GROUP7_FC_IND_FIFO_OVERRUN_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                 \
        /* TXQ-PFC : PfcMsgsSetsOverrunsEgr3_0Cause0 IntSum */                     \
        {21, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E0013B8, 0x3E0013BC,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                        \
            prvCpssDrvHwPpPortGroupIsrWrite,                                       \
            PRV_CPSS_BOBCAT2_PFC_OVERRUN_0_PFC_PORT_GROUP0_QCN_MSG_OVERRUN_E,      \
            PRV_CPSS_BOBCAT2_PFC_OVERRUN_0_PFC_PORT_GROUP3_EGR3_MSG_OVERRUN_E,     \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                 \
        /* TXQ-PFC : PfcMsgsSetsOverrunsEgr3_0Cause1 IntSum */                     \
        {22, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E0013C0, 0x3E0013C4,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                        \
            prvCpssDrvHwPpPortGroupIsrWrite,                                       \
            PRV_CPSS_BOBCAT2_PFC_OVERRUN_1_PFC_PORT_GROUP4_QCN_MSG_OVERRUN_E,      \
            PRV_CPSS_BOBCAT2_PFC_OVERRUN_1_PFC_PORT_GROUP7_EGR3_MSG_OVERRUN_E,     \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                 \
        /* TXQ-PFC : PfcMsgsSetsOverrunsEgr7_4Cause0 IntSum */                     \
        {23, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E0013D0, 0x3E0013D4,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                        \
            prvCpssDrvHwPpPortGroupIsrWrite,                                       \
            PRV_CPSS_BOBCAT2_PFC_OVERRUN_2_PFC_PORT_GROUP0_EGR4_MSG_OVERRUN_E,     \
            PRV_CPSS_BOBCAT2_PFC_OVERRUN_2_PFC_PORT_GROUP3_EGR7_MSG_OVERRUN_E,     \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                 \
        /* TXQ-PFC : PfcMsgsSetsOverrunsEgr7_4Cause1 IntSum */                     \
        {24, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E0013D8, 0x3E0013DC,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                        \
            prvCpssDrvHwPpPortGroupIsrWrite,                                       \
            PRV_CPSS_BOBCAT2_PFC_OVERRUN_3_PFC_PORT_GROUP4_EGR4_MSG_OVERRUN_E,     \
            PRV_CPSS_BOBCAT2_PFC_OVERRUN_3_PFC_PORT_GROUP7_EGR7_MSG_OVERRUN_E,     \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

static PRV_CPSS_DRV_INTERRUPT_SCAN_STC txqPfc_SUB_TREE_arr[] = {
    txqPfc_SUB_TREE_MAC(TXQPFC_BIT_CNS)
};
static GT_U32   txqPfc_SUB_TREE_numEntries = sizeof(txqPfc_SUB_TREE_arr)/sizeof(txqPfc_SUB_TREE_arr[0]);

#define   bc2_B0_txqPfc_SUB_TREE_MAC(bit)                                          \
    /* TXQ-PFC  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E001300, 0x3E001304,     \
        prvCpssDrvHwPpPortGroupIsrRead,                                            \
        prvCpssDrvHwPpPortGroupIsrWrite,                                           \
        PRV_CPSS_BOBCAT2_B0_PFC_SUM_PFC_PARITY_SUM_E,                              \
        PRV_CPSS_BOBCAT2_B0_PFC_SUM_PFC_MSGS_SETS_OVERRUNS_EGR3_0_SUM_E,           \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 6,NULL, NULL},                     \
        /*pfcParityIntSum*/                                                        \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3e001310, 0x3e001314,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                            \
            prvCpssDrvHwPpPortGroupIsrWrite,                                           \
            PRV_CPSS_BOBCAT2_B0_PFC_PARITY_SUM_PFC_PORT_GROUP0_COUNTERS_PARITY_ERR_E,  \
            PRV_CPSS_BOBCAT2_B0_PFC_PARITY_SUM_PFC_WRONG_ADDR_E,                       \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                     \
        /*PfcCntrOverUnderFlowIntSum*/                                                 \
        {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3e001320, 0x3e001324,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                            \
            prvCpssDrvHwPpPortGroupIsrWrite,                                           \
            PRV_CPSS_BOBCAT2_B0_PFC_CNTR_OVER_UNDER_FLOW_SUM_PFC_PORT_GROUP0_OVERFLOW_E,  \
            PRV_CPSS_BOBCAT2_B0_PFC_CNTR_OVER_UNDER_FLOW_SUM_PFC_PORT_GROUP0_UNDERFLOW_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                     \
        /*PfcPipe_0_GlobalOverUnderflowIntSum*/                                      \
        {3, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3e001370, 0x3e001390,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                            \
            prvCpssDrvHwPpPortGroupIsrWrite,                                           \
            PRV_CPSS_BOBCAT2_B0_PFC_PIPE_0_GLOBAL_OVER_UNDERFLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC0_UNDERFLOW_E,  \
            PRV_CPSS_BOBCAT2_B0_PFC_PIPE_0_GLOBAL_OVER_UNDERFLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC7_OVERFLOW_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                     \
        /*PfcCcOverUnderFlowIntSum*/                                                 \
        {4, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3e001328, 0x3e00132c,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                            \
            prvCpssDrvHwPpPortGroupIsrWrite,                                           \
            PRV_CPSS_BOBCAT2_B0_PFC_CC_OVER_UNDER_FLOW_SUM_PFC_PORT_GROUP0_CC_OVERFLOW_E,  \
            PRV_CPSS_BOBCAT2_B0_PFC_CC_OVER_UNDER_FLOW_SUM_PFC_PORT_GROUP0_CC_UNDERFLOW_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                     \
        /*PfcIndFifoOverrunsIntSum*/                                                 \
        {5, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3e0013b0, 0x3e0013b4,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                            \
            prvCpssDrvHwPpPortGroupIsrWrite,                                           \
            PRV_CPSS_BOBCAT2_B0_PFC_IND_FIFO_OVERRUNS_SUM_PFC_PORT_GROUP0_FC_IND_FIFO_OVERRUN_E,  \
            PRV_CPSS_BOBCAT2_B0_PFC_IND_FIFO_OVERRUNS_SUM_PFC_PORT_GROUP0_FC_IND_FIFO_OVERRUN_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                     \
        /*PfcIndFifoOverrunsIntSum*/                                                 \
        {6, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3e0013b8, 0x3e0013bC,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                            \
            prvCpssDrvHwPpPortGroupIsrWrite,                                           \
            PRV_CPSS_BOBCAT2_B0_PFC_MSGS_SETS_OVERRUNS_EGR3_0_SUM_PFC_PORT_GROUP0_QCN0_MSG_OVERRUN_E,  \
            PRV_CPSS_BOBCAT2_B0_PFC_MSGS_SETS_OVERRUNS_EGR3_0_SUM_PFC_PORT_GROUP0_EGR0_MSG_OVERRUN_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

static PRV_CPSS_DRV_INTERRUPT_SCAN_STC bc2_B0_txqPfc_SUB_TREE_arr[] = {
    bc2_B0_txqPfc_SUB_TREE_MAC(TXQPFC_BIT_CNS)
};
static GT_U32   bc2_B0_txqPfc_SUB_TREE_numEntries = sizeof(bc2_B0_txqPfc_SUB_TREE_arr)/sizeof(bc2_B0_txqPfc_SUB_TREE_arr[0]);

#define   txqQcn_SUB_TREE_MAC(bit)                                               \
    /* TXQ-QCN  */                                                                 \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_QCN_E, NULL, 0x3F000100, 0x3F000110,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                            \
        prvCpssDrvHwPpPortGroupIsrWrite,                                           \
        PRV_CPSS_BOBCAT2_QCN_SUM_CN_BUFFER_FIFO_OVERRUN_E,                         \
        PRV_CPSS_BOBCAT2_QCN_SUM_DESC_ECC_DOUBLE_ERROR_DETECTED_INT_E,             \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   txqDq_SUB_TREE_MAC(bit)                                               \
    /*  TxQ DQ Interrupt Summary Cause  */                                         \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_DQ_E, NULL, 0x40000600, 0x40000604,       \
         prvCpssDrvHwPpPortGroupIsrRead,                                           \
         prvCpssDrvHwPpPortGroupIsrWrite,                                          \
        PRV_CPSS_BOBCAT2_TXQ_DQ_SUM_FLUSH_DONE0_SUM_E,                             \
        PRV_CPSS_BOBCAT2_TXQ_DQ_SUM_GENERAL_SUM_E,                                 \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 4,NULL, NULL},                     \
                                                                                   \
        EGRESS_STC_NODE_FOR_18_PORTS_MAC(0), /*bit 6 */                            \
        EGRESS_STC_NODE_FOR_18_PORTS_MAC(1), /*bit 7 */                            \
        EGRESS_STC_NODE_FOR_18_PORTS_MAC(2), /*bit 8 */                            \
        EGRESS_STC_NODE_FOR_18_PORTS_MAC(3)  /*bit 9 */

#define   FuncUnitsIntsSum_SUB_TREE_MAC(bit)                                   \
/* FuncUnitsIntsSum Interrupt Cause */                                         \
{bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x000003f8, 0x000003FC,         \
    prvCpssDrvHwPpPortGroupIsrRead,                                            \
    prvCpssDrvHwPpPortGroupIsrWrite,                                           \
    PRV_CPSS_BOBCAT2_FUNCTIONAL_S_SUMMARY_FUNCTIONAL_INTERRUPT_SUMMARY_E,      \
    PRV_CPSS_BOBCAT2_FUNCTIONAL_S_SUMMARY_FUNC_UNITS_SUM_29_E,                 \
    FILLED_IN_RUNTIME_CNS, 0, 0x0, 28 , NULL, NULL},                           \
                                                                               \
    tti_SUB_TREE_MAC(TTI_BIT_CNS/*1*/),                                        \
    pcl_SUB_TREE_MAC(2),                                                       \
    l2i_SUB_TREE_MAC(3),                                                       \
    fdb_SUB_TREE_MAC(4),                                                       \
    tcam_SUB_TREE_MAC(5),                                                      \
    router_SUB_TREE_MAC(6),                                                    \
    lpm_SUB_TREE_MAC(7),                                                       \
    iplr0_SUB_TREE_MAC(8),                                                     \
    eq_SUB_TREE_MAC(9),                                                        \
    eplr_SUB_TREE_MAC(10),                                                     \
    ha_SUB_TREE_MAC(11),                                                       \
    epcl_SUB_TREE_MAC(12),                                                     \
    ermrk_SUB_TREE_MAC(13),                                                    \
    eoam_SUB_TREE_MAC(14),                                                     \
    tmFcu_SUB_TREE_MAC(15),                                                    \
    ioam_SUB_TREE_MAC(16),                                                     \
    mll_SUB_TREE_MAC(17),                                                      \
    /* bit 18 empty */                                                         \
    cnc_SUB_TREE_MAC(19,0),/*bit 19 cnc0*/                                     \
    cnc_SUB_TREE_MAC(20,1),/*bit 20 cnc1*/                                     \
    iplr1_SUB_TREE_MAC(21),                                                    \
    eft_SUB_TREE_MAC(22),                                                      \
    sht_SUB_TREE_MAC(23),                                                      \
    qag_SUB_TREE_MAC(24),                                                      \
    txqQueue_SUB_TREE_MAC(25),                                                 \
    bc2_B0_txqLl_SUB_TREE_MAC(TXQLL_BIT_CNS/*26*/),/*for A0 will be replaced in runtime with txqLl_NODE_MAC(26) */        \
    txqPfc_SUB_TREE_MAC(TXQPFC_BIT_CNS/*27*/),/*for B0 will be replaced in runtime with bc2_B0_txqPfc_SUB_TREE_MAC(27) */ \
    txqQcn_SUB_TREE_MAC(28),                                                   \
    txqDq_SUB_TREE_MAC(29)

#define   bm_gen1_BIT_CNS   1
#define   bm_gen2_BIT_CNS   2
/* BM General Cause Reg1 */
#define   bm_gen1_SUB_TREE_MAC(bit)                                   \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_BM_E, NULL, 0x16000300, 0x16000304,        \
        prvCpssDrvHwPpPortGroupIsrRead,                                         \
        prvCpssDrvHwPpPortGroupIsrWrite,                                        \
        PRV_CPSS_BOBCAT2_BM_GEN1_SUM_IN_PROG_CT_CLEAR_FIFO_OVERRUN_CORE_0_E,    \
        PRV_CPSS_BOBCAT2_BM_GEN1_SUM_GLOBAL_RX_FULL_E,                          \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}

/* BM General Cause Reg2 */
#define   bm_gen2_SUB_TREE_MAC(bit)                                   \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_BM_E, NULL, 0x16000308, 0x1600030C,        \
        prvCpssDrvHwPpPortGroupIsrRead,                                         \
        prvCpssDrvHwPpPortGroupIsrWrite,                                        \
        PRV_CPSS_BOBCAT2_BM_GEN2_SUM_CORE_0_RX_FULL_E,                          \
        PRV_CPSS_BOBCAT2_BM_GEN2_SUM_CORE_5_RX_DMA_ALLOCATION_DELAYED_E,        \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}


/* BM General Cause Reg1 */
#define   bc2_B0_bm_gen1_SUB_TREE_MAC(bit)                                   \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_BM_E, NULL, 0x16000300, 0x16000304,        \
        prvCpssDrvHwPpPortGroupIsrRead,                                         \
        prvCpssDrvHwPpPortGroupIsrWrite,                                        \
        PRV_CPSS_BOBCAT2_B0_BM_GEN1_SUM_IN_PROG_CT_CLEAR_FIFO_OVERRUN_CORE_0_E,    \
        PRV_CPSS_BOBCAT2_B0_BM_GEN1_SUM_GLOBAL_RX_FULL_E,                          \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}

/* BM General Cause Reg2 */
#define   bc2_B0_bm_gen2_SUB_TREE_MAC(bit)                                   \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_BM_E, NULL, 0x16000308, 0x1600030C,        \
        prvCpssDrvHwPpPortGroupIsrRead,                                         \
        prvCpssDrvHwPpPortGroupIsrWrite,                                        \
        PRV_CPSS_BOBCAT2_B0_BM_GEN2_SUM_CORE_0_RX_FULL_E,                          \
        PRV_CPSS_BOBCAT2_B0_BM_GEN2_SUM_GLOBAL_BUFFER_COUNTER_UNDERFLOW_E,        \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}

#define   bm_SUB_TREE_MAC(bit)                                          \
    /* bmSumIntSum */                                                               \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_BM_E, NULL, 0x16000310, 0x16000314,            \
        prvCpssDrvHwPpPortGroupIsrRead,                                             \
        prvCpssDrvHwPpPortGroupIsrWrite,                                            \
        PRV_CPSS_BOBCAT2_BM_SUM_SUM_BM_GENERAL_REG1_SUMMARY_E,                      \
        PRV_CPSS_BOBCAT2_BM_SUM_SUM_BM_GENERAL_REG2_SUMMARY_E,                      \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 2 , NULL, NULL},                             \
        /* BM General Cause Reg1 */                                                 \
        bm_gen1_SUB_TREE_MAC(bm_gen1_BIT_CNS),                                      \
        /* BM General Cause Reg2 */                                                 \
        bm_gen2_SUB_TREE_MAC(bm_gen2_BIT_CNS)

#define   rxDmaSum0_BIT_CNS   1
#define   rxDmaSum0_NODE_MAC(bit)                                           \
        {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_RXDMA_E, NULL, 0x17000900, 0x17000904, \
            prvCpssDrvHwPpPortGroupIsrRead,                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                    \
            PRV_CPSS_BOBCAT2_RX_DMA0_SUM_IBUF_HDR_FIFO_OVERRUN_E,               \
            PRV_CPSS_BOBCAT2_RX_DMA0_SUM_GLOBAL_ALLOC_COUNTER_REACHED_LIMIT_E,  \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}

#define   bc2_B0_rxDmaSum0_NODE_MAC(bit)                                           \
        {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_RXDMA_E, NULL, 0x17000E00, 0x17000E04, \
            prvCpssDrvHwPpPortGroupIsrRead,                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                    \
            PRV_CPSS_BOBCAT2_B0_RX_DMA0_SUM_IBUF_HDR_FIFO_OVERRUN_E,            \
            PRV_CPSS_BOBCAT2_B0_RX_DMA0_SUM_COMMON_BUFFER_EMPTY_OF_GROUP0_E,    \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}

#define   rxDma_SUB_TREE_MAC(bit)                                          \
    /* rxDmaIntSum */                                                               \
    {2, GT_FALSE, PRV_CPSS_DXCH_UNIT_RXDMA_E, NULL, 0x17001048, 0x1700104c,         \
        prvCpssDrvHwPpPortGroupIsrRead,                                             \
        prvCpssDrvHwPpPortGroupIsrWrite,                                            \
        PRV_CPSS_BOBCAT2_RX_DMA_SUM_RX_DMA_INTERRUPT_SUMMARY0_E,                    \
        PRV_CPSS_BOBCAT2_RX_DMA_SUM_RX_DMAINTERRUPT_SUMMARY3_E,                     \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 4 , NULL, NULL},                             \
         /*rxDmaSum0IntSum*/                                                        \
        {1, GT_FALSE, PRV_CPSS_DXCH_UNIT_RXDMA_E, NULL, 0x17001028, 0x1700102c,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_RX_DMA_SUM_GENERAL_RX_DMA_S_SUMMARY_E,                 \
            PRV_CPSS_BOBCAT2_RX_DMA_SUM_SC_DMA29_S_SUM_E,                           \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 31 , NULL, NULL},                        \
             /*bit 1 : rxDmaSum0IntSum*/                                            \
             rxDmaSum0_NODE_MAC(rxDmaSum0_BIT_CNS),                                 \
                                                 /* bit */ /*port*/                 \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 2  ,  0 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 3  ,  1 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 4  ,  2 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 5  ,  3 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 6  ,  4 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 7  ,  5 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 8  ,  6 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 9  ,  7 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 10 ,  8 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 11 ,  9 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 12 ,  10),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 13 ,  11),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 14 ,  12),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 15 ,  13),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 16 ,  14),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 17 ,  15),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 18 ,  16),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 19 ,  17),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 20 ,  18),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 21 ,  19),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 22 ,  20),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 23 ,  21),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 24 ,  22),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 25 ,  23),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 26 ,  24),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 27 ,  25),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 28 ,  26),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 29 ,  27),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 30 ,  28),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 31 ,  29),                    \
                                                                                    \
         /*rxDmaSum1IntSum*/                                                        \
        {2, GT_FALSE, PRV_CPSS_DXCH_UNIT_RXDMA_E, NULL, 0x17001030, 0x17001034,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_RX_DMA_SUM1_SUM_SC_DMA30_S_SUM_E,                      \
            PRV_CPSS_BOBCAT2_RX_DMA_SUM1_SUM_SC_DMA60_S_SUM_E,                      \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 31 , NULL, NULL},                        \
                                                 /* bit */ /*port*/                 \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 1  , 30 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 2  , 31 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 3  , 32 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 4  , 33 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 5  , 34 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 6  , 35 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 7  , 36 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 8  , 37 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 9  , 38 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 10 , 39 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 11 , 40 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 12 , 41 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 13 , 42 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 14 , 43 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 15 , 44 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 16 , 45 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 17 , 46 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 18 , 47 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 19 , 48 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 20 , 49 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 21 , 50 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 22 , 51 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 23 , 52 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 24 , 53 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 25 , 54 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 26 , 55 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 27 , 56 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 28 , 57 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 29 , 58 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 30 , 59 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 31 , 60 ),                    \
                                                                                    \
         /*rxDmaSum2IntSum*/                                                        \
        {3, GT_FALSE, PRV_CPSS_DXCH_UNIT_RXDMA_E, NULL, 0x17001038, 0x1700103C,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_RX_DMA_SUM2_SUM_SC_DMA61_S_SUM_E,                      \
            PRV_CPSS_BOBCAT2_RX_DMA_SUM2_SUM_SC_DMA64_S_SUM_E,                      \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 4 , NULL, NULL},                         \
                                                 /* bit */ /*port*/                 \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 1  , 61 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 2  , 62 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 3  , 63 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 4  , 64 ),                    \
                                                                                    \
         /*rxDmaSum3IntSum*/                                                        \
        {4, GT_FALSE, PRV_CPSS_DXCH_UNIT_RXDMA_E, NULL, 0x17001040, 0x17001044,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_RX_DMA_SUM3_SUM_SC_DMA65_S_SUM_E,                      \
            PRV_CPSS_BOBCAT2_RX_DMA_SUM3_SUM_SC_DMA72_S_SUM_E,                      \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 8 , NULL, NULL},                         \
                                                 /* bit */ /*port*/                 \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 1  , 65 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 2  , 66 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 3  , 67 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 4  , 68 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 5  , 69 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 6  , 70 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 7  , 71 ),                    \
            BOBCAT2_RXDMA_SCDMA_PORT_INT_ELEMENT_MAC( 8  , 72 )


#define   txDma_SUB_TREE_MAC(bit)                                          \
    /* txdmaSumIntSum */                                                            \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002040, 0x26002044,         \
        prvCpssDrvHwPpPortGroupIsrRead,                                             \
        prvCpssDrvHwPpPortGroupIsrWrite,                                            \
        PRV_CPSS_BOBCAT2_TXDMA_SUM_SUM_SC_DMA_ID_FIFO_OVERRUN_REG1_SUMMARY_E,       \
        PRV_CPSS_BOBCAT2_TXDMA_SUM_SUM_RX_DMA_UPDATES_FIF_OS_OVERRUN_REG1_SUMMARY_E,\
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 8 , NULL, NULL},                             \
        /* txdmaFifoOverrun1IntSum */                                               \
        {1, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002000, 0x26002004,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN1_SUM_SC_DMA_0_ID_FIFO_OVERRUN_E,    \
            PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN1_SUM_SC_DMA_30_ID_FIFO_OVERRUN_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                         \
        /* txdmaFifoOverrun2IntSum */                                               \
        {2, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002008, 0x2600200C,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN2_SUM_SC_DMA_31_ID_FIFO_OVERRUN_E,   \
            PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN2_SUM_SC_DMA_61_ID_FIFO_OVERRUN_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                         \
        /* txdmaFifoOverrun3IntSum */                                               \
        {3, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002010, 0x26002014,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN3_SUM_SC_DMA_62_ID_FIFO_OVERRUN_E,   \
            PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN3_SUM_SC_DMA_73_ID_FIFO_OVERRUN_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                         \
        /* txdmaWaitReply1IntSum */                                                 \
        {4, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002020, 0x26002024,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                        \
            PRV_CPSS_BOBCAT2_TXDMA_WAIT_REPLY1_SUM_SC_DMA_0_WAITING_FOR_NEXT_REPLY_E,    \
            PRV_CPSS_BOBCAT2_TXDMA_WAIT_REPLY1_SUM_SC_DMA_30_WAITING_FOR_NEXT_REPLY_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                              \
        /* txdmaWaitReply2IntSum */                                                      \
        {5, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002028, 0x2600202C,          \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TXDMA_WAIT_REPLY2_SUM_SC_DMA_31_WAITING_FOR_NEXT_REPLY_E,   \
            PRV_CPSS_BOBCAT2_TXDMA_WAIT_REPLY2_SUM_SC_DMA_61_WAITING_FOR_NEXT_REPLY_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                              \
        /* txdmaWaitReply3IntSum */                                                      \
        {6, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002030, 0x26002034,          \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TXDMA_WAIT_REPLY3_SUM_SC_DMA_62_WAITING_FOR_NEXT_REPLY_E,   \
            PRV_CPSS_BOBCAT2_TXDMA_WAIT_REPLY3_SUM_SC_DMA_73_WAITING_FOR_NEXT_REPLY_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                              \
        /* txdmaGenenalIntSum */                                                         \
        {7, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002038, 0x2600203c,          \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TXDMA_GENENAL_SUM_REQUEST_EXECUTOR_NOT_READY_E,             \
            PRV_CPSS_BOBCAT2_B0_TXDMA_GENENAL_SUM_BANK_REQUESTS_ON_AIR_REACHED_MAXIMAL_THRESHOLD_E, \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                              \
        /* txdmaRxDmaUpdate1IntSum */                                                    \
        {8, GT_FALSE, PRV_CPSS_DXCH_UNIT_TXDMA_E, NULL, 0x26002018, 0x2600201C,          \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TXDMA_RX_DMA_UPDATE1_SUM_RX_DMA_0_UPDATES_FIF_OS_OVERRUN_E, \
            PRV_CPSS_BOBCAT2_TXDMA_RX_DMA_UPDATE1_SUM_RX_DMA_0_UPDATES_FIF_OS_OVERRUN_E, \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}


#define   txFifo_SUB_TREE_MAC(bit)                                          \
    /* txFifoIntSum */                                                                   \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_TX_FIFO_E, NULL, 0x27000440, 0x27000444,            \
        prvCpssDrvHwPpPortGroupIsrRead,                                                  \
        prvCpssDrvHwPpPortGroupIsrWrite,                                                 \
        PRV_CPSS_BOBCAT2_TX_FIFO_SUM_TX_FIFO_GENERAL_REG1_SUMMARY_E,                     \
        PRV_CPSS_BOBCAT2_TX_FIFO_SUM_CT_BYTE_COUNT_ARRIVED_LATE_REG_3_SUMMARY_E,         \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 7 , NULL, NULL},                                  \
                                                                                         \
        /* txFifoGeberal1IntSum */                                                       \
        {1, GT_FALSE, PRV_CPSS_DXCH_UNIT_TX_FIFO_E, NULL, 0x27000400, 0x27000404,        \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TX_FIFO_GEBERAL1_SUM_LATENCY_FIFO_NOT_READY_E,              \
            PRV_CPSS_BOBCAT2_TX_FIFO_GEBERAL1_SUM_CT_BC_IDDB_I_DS_RUN_OUT_E,             \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                              \
        /* txFifoNotReady1IntSum */                                                      \
        {2, GT_FALSE, PRV_CPSS_DXCH_UNIT_TX_FIFO_E, NULL, 0x27000408, 0x2700040C,        \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY1_SUM_SHIFTER_0_SOP_EOP_FIFO_NOT_READY_E,  \
            PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY1_SUM_SHIFTER_30_SOP_EOP_FIFO_NOT_READY_E, \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                              \
        /* txFifoNotReady2IntSum */                                                      \
        {3, GT_FALSE, PRV_CPSS_DXCH_UNIT_TX_FIFO_E, NULL, 0x27000410, 0x27000414,        \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY2_SUM_SHIFTER_31_SOP_EOP_FIFO_NOT_READY_E, \
            PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY2_SUM_SHIFTER_61_SOP_EOP_FIFO_NOT_READY_E, \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                              \
        /* txFifoNotReady3IntSum */                                                      \
        {4, GT_FALSE, PRV_CPSS_DXCH_UNIT_TX_FIFO_E, NULL, 0x27000418, 0x2700041C,        \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY3_SUM_SHIFTER_62_SOP_EOP_FIFO_NOT_READY_E, \
            PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY3_SUM_SHIFTER_73_SOP_EOP_FIFO_NOT_READY_E, \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                              \
        /* txFifoCTLateArrived1IntSum */                                                 \
        {5, GT_FALSE, PRV_CPSS_DXCH_UNIT_TX_FIFO_E, NULL, 0x27000420, 0x27000424,        \
            prvCpssDrvHwPpPortGroupIsrRead,                                              \
            prvCpssDrvHwPpPortGroupIsrWrite,                                             \
            PRV_CPSS_BOBCAT2_TX_FIFO_CT_LATE_ARRIVED1_SUM_SC_DMA_0_CT_BYTE_COUNT_ARRIVED_LATE_E,    \
            PRV_CPSS_BOBCAT2_TX_FIFO_CT_LATE_ARRIVED1_SUM_SC_DMA_30_CT_BYTE_COUNT_ARRIVED_LATE_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                                         \
        /* txFifoCTLateArrived2IntSum */                                                            \
        {6, GT_FALSE, PRV_CPSS_DXCH_UNIT_TX_FIFO_E, NULL, 0x27000428, 0x2700042C,                   \
            prvCpssDrvHwPpPortGroupIsrRead,                                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                        \
            PRV_CPSS_BOBCAT2_TX_FIFO_CT_LATE_ARRIVED2_SUM_SC_DMA_31_CT_BYTE_COUNT_ARRIVED_LATE_E,   \
            PRV_CPSS_BOBCAT2_TX_FIFO_CT_LATE_ARRIVED2_SUM_SC_DMA_61_CT_BYTE_COUNT_ARRIVED_LATE_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                                         \
        /* txFifoCTLateArrived3IntSum */                                                            \
        {7, GT_FALSE, PRV_CPSS_DXCH_UNIT_TX_FIFO_E, NULL, 0x27000430, 0x27000434,                   \
            prvCpssDrvHwPpPortGroupIsrRead,                                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                        \
            PRV_CPSS_BOBCAT2_TX_FIFO_CT_LATE_ARRIVED3_SUM_SC_DMA_62_CT_BYTE_COUNT_ARRIVED_LATE_E,   \
            PRV_CPSS_BOBCAT2_TX_FIFO_CT_LATE_ARRIVED3_SUM_SC_DMA_73_CT_BYTE_COUNT_ARRIVED_LATE_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}

#define   bma_SUB_TREE_MAC(bit)                                          \
    /* bmaIntSum */                                                             \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_BMA_E, NULL, 0x2D019000, 0x2D019004,       \
        prvCpssDrvHwPpPortGroupIsrRead,                                         \
        prvCpssDrvHwPpPortGroupIsrWrite,                                        \
        PRV_CPSS_BOBCAT2_BMA_SUM_MC_CNT_RMW_CLEAR_FIFO_FULL_E,                  \
        PRV_CPSS_BOBCAT2_BMA_SUM_FINAL_CLEAR_UDB_FIFO3_FULL_E,                  \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}

#define   mppm_SUB_TREE_MAC(bit)                                          \
    /* mppmIntSum */                                                            \
    {bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_MPPM_E, NULL, 0x470021B0, 0x470021B4,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                         \
        prvCpssDrvHwPpPortGroupIsrWrite,                                        \
        PRV_CPSS_BOBCAT2_MPPM_SUM_RX_CLIENT_0_S_SUM_E,                          \
        PRV_CPSS_BOBCAT2_MPPM_SUM_MPPM_RF_ERR_E,                                \
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 5 , NULL, NULL},                         \
                                                                                \
        /* mppmRxClient 0 IntSum */                                             \
        {1, GT_FALSE, PRV_CPSS_DXCH_UNIT_MPPM_E, NULL, 0x47002008, 0x4700202C,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                    \
            PRV_CPSS_BOBCAT2_MPPM_RX_CLIENT_0_SUM_RX_IDDB_WR_ID_FIFO_UNDERRUN_E,\
            PRV_CPSS_BOBCAT2_MPPM_RX_CLIENT_0_SUM_CREDITS_COUNTER_UNDERRUN_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                     \
        /* mppmTxClient 0 IntSum */                                             \
        {7, GT_FALSE, PRV_CPSS_DXCH_UNIT_MPPM_E, NULL, 0x47002080, 0x470020B0,  \
            prvCpssDrvHwPpPortGroupIsrRead,                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                    \
            PRV_CPSS_BOBCAT2_MPPM_TX_CLIENT_0_SUM_TX_RD_BURST_FIFO_OVERRUN_E,   \
            PRV_CPSS_BOBCAT2_MPPM_TX_CLIENT_0_SUM_TX_RD_BURST_FIFO_OVERRUN_E,   \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                     \
        /* mppmBank 0 IntSum */                                                 \
        {15, GT_FALSE, PRV_CPSS_DXCH_UNIT_MPPM_E, NULL, 0x47002120, 0x47002170, \
            prvCpssDrvHwPpPortGroupIsrRead,                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                    \
            PRV_CPSS_BOBCAT2_MPPM_BANK_0_SUM_BANK_LATENCY_FIFO_OVERRUN_E,       \
            PRV_CPSS_BOBCAT2_MPPM_BANK_0_SUM_BANK_LATENCY_FIFO_OVERRUN_E,       \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                     \
        /* mppmBank 1 IntSum */                                                 \
        {16, GT_FALSE, PRV_CPSS_DXCH_UNIT_MPPM_E, NULL, 0x47002124, 0x47002174, \
            prvCpssDrvHwPpPortGroupIsrRead,                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                    \
            PRV_CPSS_BOBCAT2_MPPM_BANK_1_SUM_BANK_LATENCY_FIFO_OVERRUN_E,       \
            PRV_CPSS_BOBCAT2_MPPM_BANK_1_SUM_BANK_LATENCY_FIFO_OVERRUN_E,       \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL},                     \
        /* mppmEccIntSum */                                                     \
        {27, GT_FALSE, PRV_CPSS_DXCH_UNIT_MPPM_E, NULL, 0x47002000, 0x47002004, \
            prvCpssDrvHwPpPortGroupIsrRead,                                     \
            prvCpssDrvHwPpPortGroupIsrWrite,                                    \
            PRV_CPSS_BOBCAT2_MPPM_ECC_SUM_MPPM_ECC_DOUBLE_ERROR_E,              \
            PRV_CPSS_BOBCAT2_MPPM_ECC_SUM_MPPM_ECC_SINGLE_ERROR_E,              \
            FILLED_IN_RUNTIME_CNS, 0, 0x0, 0 , NULL, NULL}



#define   DataPathIntSum_SUB_TREE_MAC(bit)                                      \
/* DataPathIntSum */                                                            \
{bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x000000A4, 0x000000A8,          \
    prvCpssDrvHwPpPortGroupIsrRead,                                             \
    prvCpssDrvHwPpPortGroupIsrWrite,                                            \
    PRV_CPSS_BOBCAT2_DATA_PATH_S_SUMMARY_DATA_PATH_SUM1_E,                      \
    PRV_CPSS_BOBCAT2_DATA_PATH_S_SUMMARY_DATA_PATH_SUM9_E,                      \
    FILLED_IN_RUNTIME_CNS, 0, 0x0, 5/*without bma*/ , NULL, NULL},              \
                                                                                \
    bm_SUB_TREE_MAC(1),                                                         \
    rxDma_SUB_TREE_MAC(2),                                                      \
    txDma_SUB_TREE_MAC(6),                                                      \
    txFifo_SUB_TREE_MAC(7),                                                     \
    /*bma_SUB_TREE_MAC(8), -- internal */                                       \
    mppm_SUB_TREE_MAC(9)


#define   IlknTxfifo_SUB_TREE_MAC(bit)                                             \
    /* Ilkn Txfifo Summary Cause Reg*/                                                    \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1B0000A0, 0x1B0000A4,          \
        prvCpssDrvHwPpPortGroupIsrRead,                                                   \
        prvCpssDrvHwPpPortGroupIsrWrite,                                                  \
        PRV_CPSS_BOBCAT2_ILKN_TXFIFO_SUM_ILKN_TX_FIFO_GENERAL_SUMMARY_E,                  \
        PRV_CPSS_BOBCAT2_ILKN_TXFIFO_SUM_ILKN_TX_FIFO_UNDERRUN_SUMMARY_E,                 \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 10,NULL, NULL},                           \
        /* IlknTxfifoGenIntSum */                                                         \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000020, 0x1b000030,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_GEN_SUM_MEM_ANS_FIFO_OVERFLOW_E,                 \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_GEN_SUM_BAD_ADDR_ACCESS_E,                       \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoStreamData1IntSum */                                                 \
        {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000040, 0x1b000044,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DATA1_SUM_STREAM_ID_0_DATA_MEM_FULL_E,    \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DATA1_SUM_STREAM_ID_30_DATA_MEM_FULL_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoStreamData2IntSum */                                                 \
        {3, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000050, 0x1b000054,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DATA2_SUM_STREAM_ID_31_DATA_MEM_FULL_E,   \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DATA2_SUM_STREAM_ID_61_DATA_MEM_FULL_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoStreamData3IntSum */                                                 \
        {4, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000060, 0x1b000064,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DATA3_SUM_STREAM_ID_62_DATA_MEM_FULL_E,   \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DATA3_SUM_STREAM_ID_63_DATA_MEM_FULL_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoStreamDesc2IntSum */                                                 \
        {5, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000058, 0x1b00005C,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DESC2_SUM_STREAM_ID_31_DESC_MEM_FULL_E,   \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DESC2_SUM_STREAM_ID_61_DESC_MEM_FULL_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoStreamDesc3IntSum */                                                 \
        {6, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000068, 0x1b00006C,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DESC3_SUM_STREAM_ID_62_DESC_MEM_FULL_E,   \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DESC3_SUM_STREAM_ID_63_DESC_MEM_FULL_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoStreamDesc1IntSum */                                                 \
        {7, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000048, 0x1b00004C,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DESC1_SUM_STREAM_ID_0_DESC_MEM_FULL_E,    \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_STREAM_DESC1_SUM_STREAM_ID_30_DESC_MEM_FULL_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoUnderrun1IntSum */                                                   \
        {8, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000070, 0x1b000074,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_UNDERRUN1_SUM_STREAM_ID_0_UNDERRUN_E,            \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_UNDERRUN1_SUM_STREAM_ID_30_UNDERRUN_E,           \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoUnderrun2IntSum */                                                   \
        {9, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000078, 0x1b00007C,      \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_UNDERRUN2_SUM_STREAM_ID_31_UNDERRUN_E,           \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_UNDERRUN2_SUM_STREAM_ID_61_UNDERRUN_E,           \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* IlknTxfifoUnderrun3IntSum */                                                   \
        {10, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E, NULL, 0x1b000080, 0x1b000084,     \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_UNDERRUN3_SUM_STREAM_ID_62_UNDERRUN_E,           \
            PRV_CPSS_BOBCAT2_ILKN_TXFIFO_UNDERRUN3_SUM_STREAM_ID_63_UNDERRUN_E,           \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   ethTxfifo_SUB_TREE_MAC(bit)                                             \
    /* ETH TX FIFO Interrupt Cause */                                                     \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E, NULL, 0x1A000440, 0x1A000444,           \
        prvCpssDrvHwPpPortGroupIsrRead,                                                   \
        prvCpssDrvHwPpPortGroupIsrWrite,                                                  \
        PRV_CPSS_BOBCAT2_ETH_TXFIFO_SUM_TX_FIFO_GENERAL_REG1_SUMMARY_E,                   \
        PRV_CPSS_BOBCAT2_ETH_TXFIFO_SUM_CT_BYTE_COUNT_ARRIVED_LATE_REG_3_SUMMARY_E,       \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 7,NULL, NULL},                            \
        /* ethTxfifoGenIntSum */                                                          \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E, NULL, 0x1A000400, 0x1A000404,       \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_GEN_SUM_LATENCY_FIFO_NOT_READY_E,                 \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_GEN_SUM_CT_BC_IDDB_I_DS_RUN_OUT_E,                \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                        \
        /* ethTxfifoNotReady1IntSum */                                                    \
        {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E, NULL, 0x1A000408, 0x1A00040C,       \
            prvCpssDrvHwPpPortGroupIsrRead,                                               \
            prvCpssDrvHwPpPortGroupIsrWrite,                                              \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_NOT_READY1_SUM_SHIFTER_0_SOP_EOP_FIFO_NOT_READY_E,  \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_NOT_READY1_SUM_SHIFTER_30_SOP_EOP_FIFO_NOT_READY_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                          \
        /* ethTxfifoNotReady2IntSum */                                                      \
        {3, GT_FALSE,PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E, NULL, 0x1A000410, 0x1A000414,         \
            prvCpssDrvHwPpPortGroupIsrRead,                                                 \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_NOT_READY2_SUM_SHIFTER_31_SOP_EOP_FIFO_NOT_READY_E, \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_NOT_READY2_SUM_SHIFTER_61_SOP_EOP_FIFO_NOT_READY_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                          \
        /* ethTxfifoNotReady3IntSum */                                                      \
        {4, GT_FALSE,PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E, NULL, 0x1A000418, 0x1A00041C,         \
            prvCpssDrvHwPpPortGroupIsrRead,                                                 \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_NOT_READY3_SUM_SHIFTER_62_SOP_EOP_FIFO_NOT_READY_E, \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_NOT_READY3_SUM_SHIFTER_73_SOP_EOP_FIFO_NOT_READY_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                          \
        /* ethTxfifoCTLateArrived1IntSum */                                                 \
        {5, GT_FALSE,PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E, NULL, 0x1A000420, 0x1A000424,         \
            prvCpssDrvHwPpPortGroupIsrRead,                                                 \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_CT_LATE_ARRIVED1_SUM_SC_DMA_0_CT_BYTE_COUNT_ARRIVED_LATE_E,   \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_CT_LATE_ARRIVED1_SUM_SC_DMA_30_CT_BYTE_COUNT_ARRIVED_LATE_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                    \
        /* ethTxfifoCTLateArrived2IntSum */                                                           \
        {6, GT_FALSE,PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E, NULL, 0x1A000428, 0x1A00042C,                   \
            prvCpssDrvHwPpPortGroupIsrRead,                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                          \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_CT_LATE_ARRIVED2_SUM_SC_DMA_31_CT_BYTE_COUNT_ARRIVED_LATE_E,  \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_CT_LATE_ARRIVED2_SUM_SC_DMA_61_CT_BYTE_COUNT_ARRIVED_LATE_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                    \
        /* ethTxfifoCTLateArrived3IntSum */                                                           \
        {7, GT_FALSE,PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E, NULL, 0x1A000430, 0x1A000434,                   \
            prvCpssDrvHwPpPortGroupIsrRead,                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                          \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_CT_LATE_ARRIVED3_SUM_SC_DMA_62_CT_BYTE_COUNT_ARRIVED_LATE_E,  \
            PRV_CPSS_BOBCAT2_ETH_TXFIFO_CT_LATE_ARRIVED3_SUM_SC_DMA_73_CT_BYTE_COUNT_ARRIVED_LATE_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   tmEgressGlue_SUB_TREE_MAC(bit)                                             \
    /* tm Egress Glue */                                                                              \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x1900002C, 0x19000030,                   \
        prvCpssDrvHwPpPortGroupIsrRead,                                                               \
        prvCpssDrvHwPpPortGroupIsrWrite,                                                              \
        PRV_CPSS_BOBCAT2_TM_EGRESS_GLUE_SUM_TM_EGRESS_GLUE_SUM_E,                                     \
        PRV_CPSS_BOBCAT2_TM_EGRESS_GLUE_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND_6_SUMMARY_E, \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 24,NULL, NULL},                                       \
        /* tmEgressGenIntSum */                                                                       \
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000024, 0x19000028,               \
            prvCpssDrvHwPpPortGroupIsrRead,                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_GEN_SUM_AGING_PACKET_DROP_COUNTER_WRAPAROUND_E,                \
            PRV_CPSS_BOBCAT2_TM_EGRESS_GEN_SUM_REGFILE_ADDRESS_ERROR_E,                               \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                    \
        /* tmEgressStatAgeTimeCntr0IntSum */                                                          \
        {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000100, 0x19000110,               \
            prvCpssDrvHwPpPortGroupIsrRead,                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_TIME_CNTR0_SUM_STATISTICAL_AGING_TIME_COUNTER_WRAPAROUND0_E,   \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_TIME_CNTR0_SUM_STATISTICAL_AGING_TIME_COUNTER_WRAPAROUND30_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                               \
        /* tmEgressStatAgeTimeCntr1IntSum */                                                                     \
        {3, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000104, 0x19000114,                          \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                      \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                     \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_TIME_CNTR1_SUM_STATISTICAL_AGING_TIME_COUNTER_WRAPAROUND31_E,  \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_TIME_CNTR1_SUM_STATISTICAL_AGING_TIME_COUNTER_WRAPAROUND61_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                               \
        /* tmEgressStatAgeTimeCntr2IntSum */                                                                     \
        {4, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000108, 0x19000118,                          \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                      \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                     \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_TIME_CNTR2_SUM_STATISTICAL_AGING_TIME_COUNTER_WRAPAROUND62_E,  \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_TIME_CNTR2_SUM_STATISTICAL_AGING_TIME_COUNTER_WRAPAROUND92_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                               \
        /* tmEgressStatAgeTimeCntr3IntSum */                                                                     \
        {5, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x1900010c, 0x1900011c,                          \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                      \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                     \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_TIME_CNTR3_SUM_STATISTICAL_AGING_TIME_COUNTER_WRAPAROUND93_E,  \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_TIME_CNTR3_SUM_STATISTICAL_AGING_TIME_COUNTER_WRAPAROUND98_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                               \
        /* tmEgressStatAgePacketCntr0IntSum */                                                                   \
        {6, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000038, 0x19000048,                          \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                      \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                     \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_PACKET_CNTR0_SUM_STATISTICAL_AGING_PACKET_COUNTER_WRAPAROUND0_E,  \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_PACKET_CNTR0_SUM_STATISTICAL_AGING_PACKET_COUNTER_WRAPAROUND30_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                  \
        /* tmEgressStatAgePacketCntr1IntSum */                                                                      \
        {7, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x1900003c, 0x1900004c,                             \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                         \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                        \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_PACKET_CNTR1_SUM_STATUSISTICAL_AGING_PACKET_COUNTER_WRAPAROUND31_E, \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_PACKET_CNTR1_SUM_STATUSISTICAL_AGING_PACKET_COUNTER_WRAPAROUND61_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                    \
        /* tmEgressStatAgePacketCntr2IntSum */                                                                        \
        {8, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000040, 0x19000050,                               \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_PACKET_CNTR2_SUM_STATISTICAL_AGING_PACKET_COUNTER_WRAPAROUND62_E,   \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_PACKET_CNTR2_SUM_STATISTICAL_AGING_PACKET_COUNTER_WRAPAROUND92_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                    \
        /* tmEgressStatAgePacketCntr3IntSum */                                                                        \
        {9, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000044, 0x19000054,                               \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_PACKET_CNTR3_SUM_STATISTICAL_AGING_PACKET_COUNTER_WRAPAROUND93_E,   \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_PACKET_CNTR3_SUM_STATISTICAL_AGING_PACKET_COUNTER_WRAPAROUND99_E,   \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                    \
        /*tmEgressStatAgeOctCntr0IntSum*/                                                                             \
        {10, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000120, 0x19000130,                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_OCT_CNTR0_SUM_STATISTICAL_AGING_OCTET_COUNTER_WRAPAROUND0_E,        \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_OCT_CNTR0_SUM_STATISTICAL_AGING_OCTET_COUNTER_WRAPAROUND30_E,       \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                    \
        /*tmEgressStatAgeOctCntr1IntSum*/                                                                             \
        {11, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000124, 0x19000134,                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_OCT_CNTR1_SUM_STATISTICAL_AGING_OCTET_COUNTER_WRAPAROUND31_E,       \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_OCT_CNTR1_SUM_STATISTICAL_AGING_OCTET_COUNTER_WRAPAROUND61_E,       \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                    \
        /*tmEgressStatAgeOctCntr2IntSum*/                                                                             \
        {12, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000128, 0x19000138,                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_OCT_CNTR2_SUM_STATISTICAL_AGING_OCTET_COUNTER_WRAPAROUND62_E,       \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_OCT_CNTR2_SUM_STATISTICAL_AGING_OCTET_COUNTER_WRAPAROUND92_E,       \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                    \
        /*tmEgressStatAgeOctCntr3IntSum*/                                                                             \
        {13, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x1900012c, 0x1900013c,                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_OCT_CNTR3_SUM_STATISTICAL_AGING_OCTET_COUNTER_WRAPAROUND93_E,       \
            PRV_CPSS_BOBCAT2_TM_EGRESS_STATUS_AGE_OCT_CNTR3_SUM_STATISTICAL_AGING_OCTET_COUNTER_WRAPAROUND98_E,       \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                    \
        /*tmEgressAgingAgedOutPacketOctCntr0IntSum*/                                                                  \
        {14, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000140, 0x19000150,                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_AGING_AGED_OUT_PACKET_OCT_CNTR0_SUM_STATISTICAL_AGING_AGED_OUT_PACKET_COUNTER_WRAPAROUND0_E,   \
            PRV_CPSS_BOBCAT2_TM_EGRESS_AGING_AGED_OUT_PACKET_OCT_CNTR0_SUM_STATISTICAL_AGING_AGED_OUT_PACKET_COUNTER_WRAPAROUND30_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressAgingAgedOutPacketOctCntr1IntSum*/                                                                                  \
        {15, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000144, 0x19000154,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_AGING_AGED_OUT_PACKET_OCT_CNTR1_SUM_STATISTICAL_AGING_AGED_OUT_PACKET_COUNTER_WRAPAROUND31_E,  \
            PRV_CPSS_BOBCAT2_TM_EGRESS_AGING_AGED_OUT_PACKET_OCT_CNTR1_SUM_STATISTICAL_AGING_AGED_OUT_PACKET_COUNTER_WRAPAROUND61_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressAgingAgedOutPacketOctCntr2IntSum*/                                                                                  \
        {16, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19000148, 0x19000158,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_AGING_AGED_OUT_PACKET_OCT_CNTR2_SUM_STATISTICAL_AGING_AGED_OUT_PACKET_COUNTER_WRAPAROUND62_E,  \
            PRV_CPSS_BOBCAT2_TM_EGRESS_AGING_AGED_OUT_PACKET_OCT_CNTR2_SUM_STATISTICAL_AGING_AGED_OUT_PACKET_COUNTER_WRAPAROUND92_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressAgingAgedOutPacketOctCntr3IntSum*/                                                                                  \
        {17, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x1900014c, 0x1900015c,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_AGING_AGED_OUT_PACKET_OCT_CNTR3_SUM_STATISTICAL_AGING_AGED_OUT_PACKET_COUNTER_WRAPAROUND93_E,  \
            PRV_CPSS_BOBCAT2_TM_EGRESS_AGING_AGED_OUT_PACKET_OCT_CNTR3_SUM_STATISTICAL_AGING_AGED_OUT_PACKET_COUNTER_WRAPAROUND99_E,  \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressPerPortAgingAgedOutPacketOctCntr0IntSum*/                                                                           \
        {18, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19002020, 0x19002040,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR0_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND0_E,  \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR0_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND30_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressPerPortAgingAgedOutPacketOctCntr1IntSum*/                                                                           \
        {19, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19002024, 0x19002044,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR1_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND31_E, \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR1_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND61_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressPerPortAgingAgedOutPacketOctCntr2IntSum*/                                                                           \
        {20, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19002028, 0x19002048,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR2_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND62_E, \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR2_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND92_E, \
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressPerPortAgingAgedOutPacketOctCntr3IntSum*/                                                                           \
        {21, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x1900202c, 0x1900204c,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR3_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND93_E, \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR3_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND123_E,\
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressPerPortAgingAgedOutPacketOctCntr4IntSum*/                                                                           \
        {22, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19002030, 0x19002050,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR4_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND124_E,\
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR4_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND154_E,\
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressPerPortAgingAgedOutPacketOctCntr4IntSum*/                                                                           \
        {23, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19002034, 0x19002054,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR5_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND155_E,\
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR5_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND185_E,\
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                                                                    \
        /*tmEgressPerPortAgingAgedOutPacketOctCntr5IntSum*/                                                                           \
        {24, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E, NULL, 0x19002038, 0x19002058,                                              \
            prvCpssDrvHwPpPortGroupIsrRead,                                                                                           \
            prvCpssDrvHwPpPortGroupIsrWrite,                                                                                          \
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR6_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND186_E,\
            PRV_CPSS_BOBCAT2_TM_EGRESS_PER_PORT_AGING_AGED_OUT_PACKET_OCT_CNTR6_SUM_PER_PORT_AGED_OUT_PACKET_COUNTERS_WRAPAROUND191_E,\
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define   FuncUnits1IntsSum_SUB_TREE_MAC(bit)                                             \
/* FuncUnits1IntsSum  */                                                                  \
{bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x000003F4, 0x000003F0,                     \
    prvCpssDrvHwPpPortGroupIsrRead,                                                       \
    prvCpssDrvHwPpPortGroupIsrWrite,                                                      \
    PRV_CPSS_BOBCAT2_FUNCTIONAL_1_S_SUMMARY_FUNC_UNITS_1_SUM_1_E,                         \
    PRV_CPSS_BOBCAT2_FUNCTIONAL_1_S_SUMMARY_FUNC_UNITS_1_SUM_7_E,                         \
    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 6 /* currently ignore TM unit as sub tree */,NULL, NULL},\
                                                                                          \
    IlknTxfifo_SUB_TREE_MAC(1),                                                           \
                                                                                          \
    /* TMDROP Interrupt Cause */                                                          \
    {2, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_DROP_E, NULL, 0x09000030, 0x09000034,              \
        prvCpssDrvHwPpPortGroupIsrRead,                                                   \
        prvCpssDrvHwPpPortGroupIsrWrite,                                                  \
        PRV_CPSS_BOBCAT2_TM_DROP_SUM_REGFILE_ADDRESS_ERROR_E,                             \
        PRV_CPSS_BOBCAT2_TM_DROP_SUM_REGFILE_ADDRESS_ERROR_E,                             \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                            \
                                                                                          \
    /* TMQMAP Interrupt Cause */                                                          \
    {3, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_QMAP_E, NULL, 0x0A000020, 0x0A000024,              \
        prvCpssDrvHwPpPortGroupIsrRead,                                                   \
        prvCpssDrvHwPpPortGroupIsrWrite,                                                  \
        PRV_CPSS_BOBCAT2_TM_QMAP_SUM_REGFILE_ADDRESS_ERROR_E,                             \
        PRV_CPSS_BOBCAT2_TM_QMAP_SUM_REGFILE_ADDRESS_ERROR_E,                             \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                            \
                                                                                          \
    ethTxfifo_SUB_TREE_MAC(4),                                                            \
                                                                                          \
    /* tm Ingress Glue */                                                                 \
    {5, GT_FALSE,PRV_CPSS_DXCH_UNIT_TM_INGRESS_GLUE_E, NULL, 0x18000008, 0x1800000C,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                                   \
        prvCpssDrvHwPpPortGroupIsrWrite,                                                  \
        PRV_CPSS_BOBCAT2_TM_INGRESS_GLUE_SUM_SHIFTER_SOP_EOP_FIFO_NOT_READY_E,            \
        PRV_CPSS_BOBCAT2_TM_INGRESS_GLUE_SUM_REGFILE_ADDRESS_ERROR_E,                     \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},                            \
                                                                                          \
    tmEgressGlue_SUB_TREE_MAC(6)
    /* bit 7 -- currently ignore TM unit as sub tree !!!! */


/* NOTE: Erratum in A0 --> not connected */
/*GOP/<PR IP> PR IP Units/PR Interrupts/PR Summary Cause Reg*/
#define BOBCAT2_B0_ILKN_PR_SUMMARY_INT_SUB_TREE_MAC(bitIndexInCaller)   \
    /* ILKN PR summary Interrupts */                    \
    {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL, \
    0x1F001B00 , 0x1F001B04  , \
    prvCpssDrvHwPpPortGroupIsrRead,                    \
    prvCpssDrvHwPpPortGroupIsrWrite,                   \
    PRV_CPSS_BOBCAT2_ILKN_PR_SUMMARY_REG_FILL_OVERRUN_0_SUM_E,\
    PRV_CPSS_BOBCAT2_ILKN_PR_SUMMARY_REG_PR_GENERAL_REG_SUM_E,\
    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 19,NULL, NULL},\
        /* ILKN_PR Fill Overrun Interrupt Cause 0 */   \
        { 1 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A40 , 0x1F001A44,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_OVERRUN_0_CHANNEL_0_FILL_OVERRUN_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_OVERRUN_0_CHANNEL_30_FILL_OVERRUN_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR Fill Overrun Interrupt Cause 1 */   \
        { 2 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A48 , 0x1F001A4C,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_OVERRUN_1_CHANNEL31_FILL_OVERRUN_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_OVERRUN_1_CHANNEL61_FILL_OVERRUN_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR Fill Overrun Interrupt Cause 2 */   \
        { 3 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A50 , 0x1F001A54,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_OVERRUN_2_CHANNEL62_FILL_OVERRUN_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_OVERRUN_2_CHANNEL63_FILL_OVERRUN_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR Fill Underrun Interrupt Cause 0 */   \
        { 4 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A60 , 0x1F001A64,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_UNDERRUN_0_CHANNEL_0_FILL_UNDERRUN_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_UNDERRUN_0_CHANNEL_30_FILL_UNDERRUN_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR Fill Underrun Interrupt Cause 1 */   \
        { 5 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A68 , 0x1F001A6C,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_UNDERRUN_1_CHANNEL31_FILL_UNDERRUN_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_UNDERRUN_1_CHANNEL61_FILL_UNDERRUN_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR Fill Underrun Interrupt Cause 2 */   \
        { 6 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A70 , 0x1F001A74,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_UNDERRUN_2_CHANNEL62_FILL_UNDERRUN_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_FILL_UNDERRUN_2_CHANNEL63_FILL_UNDERRUN_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR No SOP Interrupt Cause 0 */   \
        { 7 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A20 , 0x1F001A24,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_SOP_0_CHANNEL_0_NO_SOP_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_SOP_0_CHANNEL_30_NO_SOP_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR No SOP Interrupt Cause 1 */   \
        { 8 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A28 , 0x1F001A2C,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_SOP_1_CHANNEL31_NO_SOP_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_SOP_1_CHANNEL61_NO_SOP_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR No SOP Interrupt Cause 2 */   \
        { 9 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A30 , 0x1F001A34,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_SOP_2_CHANNEL62_NO_SOP_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_SOP_2_CHANNEL63_NO_SOP_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR No EOP Interrupt Cause 0 */   \
        { 10 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A90 , 0x1F001A94,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_EOP_0_CHANNEL_0_NO_EOP_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_EOP_0_CHANNEL_30_NO_EOP_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR No EOP Interrupt Cause 1 */   \
        { 11 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A98 , 0x1F001A9C,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_EOP_1_CHANNEL31_NO_EOP_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_EOP_1_CHANNEL61_NO_EOP_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR No EOP Interrupt Cause 2 */   \
        { 12 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001AA0 , 0x1F001AA4,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_EOP_2_CHANNEL62_NO_EOP_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_NO_EOP_2_CHANNEL63_NO_EOP_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR Rx Full Interrupt Cause 0 */   \
        { 13 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A00 , 0x1F001A04,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_RX_FULL_0_CHANNEL_0_RX_FULL_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_RX_FULL_0_CHANNEL_30_RX_FULL_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR Rx Full Interrupt Cause 1 */   \
        { 14 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A08 , 0x1F001A0C,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_RX_FULL_1_CHANNEL31_RX_FULL_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_RX_FULL_1_CHANNEL61_RX_FULL_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR Rx Full Interrupt Cause 2 */   \
        { 15 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A10 , 0x1F001A14,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_RX_FULL_2_CHANNEL62_RX_FULL_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_RX_FULL_2_CHANNEL63_RX_FULL_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR MIB Wraparound Interrupt Cause 0 */   \
        { 16 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001F00 , 0x1F001F04,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_MIB_WRAPAROUND_0_CHANNEL_0_MIB_WRAPAROUND_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_MIB_WRAPAROUND_0_CHANNEL_30_MIB_WRAPAROUND_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR MIB Wraparound Interrupt Cause 1 */   \
        { 17 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001F08 , 0x1F001F0C,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_MIB_WRAPAROUND_1_CHANNEL31_MIB_WRAPAROUND_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_MIB_WRAPAROUND_1_CHANNEL61_MIB_WRAPAROUND_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR MIB Wraparound Interrupt Cause 2 */   \
        { 18 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001F10 , 0x1F001F14,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_MIB_WRAPAROUND_2_CHANNEL62_MIB_WRAPAROUND_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_MIB_WRAPAROUND_2_CHANNEL63_MIB_WRAPAROUND_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},  \
        /* ILKN_PR MIB Wraparound Interrupt Cause 2 */   \
        { 19 ,GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL,  \
        0x1F001A80 , 0x1F001A84,\
        prvCpssDrvHwPpPortGroupIsrRead,                          \
        prvCpssDrvHwPpPortGroupIsrWrite,                         \
        PRV_CPSS_BOBCAT2_ILKN_PR_GENERAL_REG_RF_ERROR_E,  \
        PRV_CPSS_BOBCAT2_ILKN_PR_GENERAL_REG_RF_ERROR_E,  \
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* PTP interrupts for ILKN port */
#define BOBCAT2_PTP_FOR_ILKN_PORT_INT_ELEMENT_MAC(bitIndexInCaller)   \
            {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL, \
              0x1F002000 , 0x1F002004  , \
              prvCpssDrvHwPpPortGroupIsrRead,                    \
              prvCpssDrvHwPpPortGroupIsrWrite,                   \
              PRV_CPSS_BOBCAT2_PTP_PORT_ILKN_PTP_RX_FIFO_FULL0_E, \
              PRV_CPSS_BOBCAT2_PTP_PORT_ILKN_TIMESTAMP_QUEUE1_FULL_E,\
              FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* NOTE: Erratum in A0 --> not connected */
/* NOTE: in B0 --> Still not know how connected */
/*GOP/<ILKN WRP IP> Interlaken Wrapper IP Units/Converters registers/Interlaken Interrupt Cause*/
#define BOBCAT2_B0_ILKN_WRAP_INT_ELEMENT_MAC(bitIndexInCaller)   \
    /* ILKN PR summary Interrupts */                    \
    {bitIndexInCaller, GT_FALSE,PRV_CPSS_DXCH_UNIT_ILKN_E, NULL, \
    0x1F000870 , 0x1F000874  , \
    prvCpssDrvHwPpPortGroupIsrRead,                    \
    prvCpssDrvHwPpPortGroupIsrWrite,                   \
    PRV_CPSS_BOBCAT2_B0_ILKN_WRAPPER_ILKN_INTERRUPT_E,\
    PRV_CPSS_BOBCAT2_B0_ILKN_WRAPPER_TOO_SHORT_TX_PACKET_E ,\
    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0/* not point to others */,NULL, NULL}

CPSS_TBD_BOOKMARK/* from where pointed to this reg <ILKN WRP IP> */
/* NOTE: new register in B0 --> Still not know how connected */
/*GOP/<ILKN WRP IP> Interlaken Wrapper IP Units/Converters registers/Interlaken Interrupt Cause*/
#define BOBCAT2_B0_ILKN_WRAP_SUMMARY_INT_SUB_TREE_MAC(bitIndexInCaller,myFatherRegisterUniqueId)   \
    /* ILKN PR summary Interrupts */                    \
    {bitIndexInCaller, GT_FALSE, (NON_A0_SUB_TREE_CNS | PRV_CPSS_DXCH_UNIT_ILKN_E), NULL, \
    0x1F00089C  , 0x1F0008A0  , \
    prvCpssDrvHwPpPortGroupIsrRead,                    \
    prvCpssDrvHwPpPortGroupIsrWrite,                   \
    PRV_CPSS_BOBCAT2_B0_ILKN_WRAPPER_SUM_INTERLAKEN_CONVERTERS_INTERRUPT_SUMMARY_E,\
    PRV_CPSS_BOBCAT2_B0_ILKN_WRAPPER_SUM_PTP_INTERRUPT_SUMMARY_E ,\
    FILLED_IN_RUNTIME_CNS, 0x0, myFatherRegisterUniqueId /*<maskRcvIntrEn>0xFFFFFFFF*/, \
        2/*without TAI0,1 that connected to MG*/,NULL, NULL}, \
        /*bit 1*/ \
        BOBCAT2_B0_ILKN_WRAP_INT_ELEMENT_MAC(1),        \
        /*bit 2*/ \
        BOBCAT2_B0_ILKN_PR_SUMMARY_INT_SUB_TREE_MAC(2)
CPSS_TBD_BOOKMARK
#if 0 /* access to PTP Unit of ILNK stuck CPU */
, \
        /*bit 5*/ \
        BOBCAT2_PTP_FOR_ILKN_PORT_INT_ELEMENT_MAC(5)
#endif
#if 0
static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC bobcat2IntrScanArr[] =
{
/* FuncUnitsIntsSum Interrupt Cause */
{3  , GT_FALSE, PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x000003f8, 0x000003FC,
    prvCpssDrvHwPpPortGroupIsrRead,
    prvCpssDrvHwPpPortGroupIsrWrite,
    PRV_CPSS_BOBCAT2_FUNCTIONAL_S_SUMMARY_FUNCTIONAL_INTERRUPT_SUMMARY_E,
    PRV_CPSS_BOBCAT2_FUNCTIONAL_S_SUMMARY_FUNC_UNITS_SUM_29_E,
    FILLED_IN_RUNTIME_CNS, 0, 0x0, 2 , NULL, NULL},

    pcl_SUB_TREE_MAC(26),
/*    bc2_B0_txqLl_SUB_TREE_MAC(TXQLL_BIT_CNS),*//*for A0 will be replaced in runtime with txqLl_NODE_MAC(26) */

    {0  , GT_FALSE, 0, NULL, 0, CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS,
        0,        0,        0,        0,
        0, 0, 0x0, 0 , 0, 0},

    /* TXQ-PFC  */
    {27, GT_FALSE,(A0_TXQ_PFC_TO_REPLACE_WITH_B0_SUB_TREE_CNS | PRV_CPSS_DXCH_UNIT_TXQ_PFC_E), NULL, 0x3E001300, 0x3E001304,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_BOBCAT2_PFC_SUM_PFC_COUNTERS_PARITY_SUM_E,
        PRV_CPSS_BOBCAT2_PFC_SUM_PFC_MSGS_SETS_OVERRUNS_EGR7_41_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1,NULL, NULL},

        /* TXQ-PFC : PfcCountersParityIntSum  */
        {1, GT_FALSE,PRV_CPSS_DXCH_UNIT_TXQ_PFC_E, NULL, 0x3E001310, 0x3E001314,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP0_COUNTERS_PARITY_ERR_E,
            PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_WRONG_ADDR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

    {0  , GT_FALSE, 0, NULL, 0, CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS,
        0,        0,        0,        0,
        0, 0, 0x0, 0 , 0, 0}

};
#else
/* Don't use "const" because we Fill some values at initialization stage */
/* Bobcat2 interrupt tree */
static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC bobcat2IntrScanArr[] =
{
    /* Global Interrupt Cause */
    {0, GT_FALSE, PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_BOBCAT2_GLOBAL_S_SUMMARY_FUNC_UNITSS_SUM_E,
        PRV_CPSS_BOBCAT2_GLOBAL_S_SUMMARY_PORTS_2_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 9 , NULL, NULL},

        /* NOTE: the PEX and the DFX are not connected to MG but directly to the MSYS (CPU periphery) */

        FuncUnitsIntsSum_SUB_TREE_MAC(3),

        DataPathIntSum_SUB_TREE_MAC(4),

        /* ports0SumIntSum */
        {5,GT_FALSE,PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x00000080, 0x00000084,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_BOBCAT2_PORTS0_SUM_SUM_PORT_SUM_1_E,
            PRV_CPSS_BOBCAT2_PORTS0_SUM_SUM_PORT_SUM_31_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 31,NULL, NULL},

            /* giga ports 0..30 in bits 1..31 */
            BOBCAT2_GIGA_PORTS_0_30_SUMMARY_INT_SUB_TREE_MAC,

        /* Miscellaneous */
        {7,GT_FALSE,PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x00000038, 0x0000003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_BOBCAT2_MISC_TWSI_TIME_OUT_E,
            PRV_CPSS_BOBCAT2_B0_MISC_SERDES_OUT_OF_RANGE_VIOLATION_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1,NULL, NULL},
            /* HostCpuDoorbellIntSum */
            {24,GT_FALSE,PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x00000518, 0x0000051c,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_BOBCAT2_HOST_CPU_DOORBELL_SUM_DATA_TO_HOST_CPU_0_E,
                PRV_CPSS_BOBCAT2_HOST_CPU_DOORBELL_SUM_DATA_TO_HOST_CPU_30_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
        /* Tx SDMA  */
        {8, GT_FALSE,PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x00002810, 0x00002818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_0_E,
            PRV_CPSS_BOBCAT2_B0_TX_SDMA_TX_REJECT_0_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
        /* Rx SDMA  */
        {9, GT_FALSE,PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_0_E,
            PRV_CPSS_BOBCAT2_RX_SDMA_PACKET_CNT_OF_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        FuncUnits1IntsSum_SUB_TREE_MAC(11),

        /* ports1SumIntSum */
        {16,GT_FALSE,PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x00000150, 0x00000154,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_BOBCAT2_PORTS1_SUM_SUM_PORT_1_SUM_1_E,
            PRV_CPSS_BOBCAT2_PORTS1_SUM_SUM_PORT_1_SUM_30_E,/*30 in B0 and 29 in A0*/
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF,
                /* field subIntrListLen 30 in B0 and 29 in A0 --> set in runtime to 29 */
                30,
                NULL, NULL},


            /* giga ports 31..47 in bits 1..17 */
            BOBCAT2_GIGA_PORTS_31_47_SUMMARY_INT_SUB_TREE_MAC,

            /* XLG ports 48..55 in bits 18..25 */
            BOBCAT2_XLG_PORTS_48_55_SUMMARY_INT_SUB_TREE_MAC,

            /* TAI in bits 26..29 */
                                        /*bitIndex, port, taiId(0/1)*/
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(26 , 64 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(27 , 68 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(28 , 64 ,1),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(29 , 68 ,1),

            /* !!! B0 only !!! (in A0 Erratum not connected) ILKN_WRAP */
            BOBCAT2_B0_ILKN_WRAP_SUMMARY_INT_SUB_TREE_MAC(30,
                /* must be value to unique the 'register' that my father uses */
                PRV_CPSS_BOBCAT2_PORTS1_SUM_SUM_PORT_1_SUM_30_E/* id to find 'my father' */),

        /* ports2SumIntSum */
        {17,GT_FALSE,PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x00000158, 0x0000015c,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_BOBCAT2_PORTS2_SUM_SUM_PORT_2_SUM_1_E,
            PRV_CPSS_BOBCAT2_PORTS2_SUM_SUM_PORT_2_SUM_30_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 30,NULL, NULL},

            /* XLG ports 56..71 in bits 1..16 */
            BOBCAT2_XLG_PORTS_56_71_SUMMARY_INT_SUB_TREE_MAC,

            /* TAI in bits 17..30 */
                                        /*bitIndex, port, taiId(0/1)*/
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(17 , 48 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(18 , 52 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(19 , 48 ,1),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(20 , 52 ,1),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(21 ,  0 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(22 , 16 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(23 , 32 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(24 ,  0 ,1),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(25 , 16 ,1),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(26 , 32 ,1),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(27 , 56 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(28 , 60 ,0),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(29 , 56 ,1),
            BOBCAT2_TAI_SUMMARY_INT_SUB_TREE_MAC(30 , 60 ,1)
};
#endif

/* number of elements in the array of bobcat2IntrScanArr[] */
#define BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS \
    (sizeof(bobcat2IntrScanArr)/sizeof(bobcat2IntrScanArr[0]))

#define BOBCAT2_NUM_MASK_REGISTERS_CNS  (PRV_CPSS_BOBCAT2_LAST_INT_E / 32)

#define BC2_B0_RXDMA_OFFSET_FROM_A0_CNS         0x500

/* array for BC2_B0 to allow 'override' of element in the interrupts tree by BC2_A0.*/
static PRV_CPSS_DRV_INTERRUPT_SCAN_OVERRIDE_INFO_STC    bc2B0OverrideA0Arr[] =
{
    /* TTI sub tree for BC2_B0 --> different then for A0 */
    {
        PRV_CPSS_BOBCAT2_TTI_SUM_CPU_ADDRESS_OUT_OF_RANGE_E,
        /* override in runtime --> according to 'old oldIntCause' */
        bc2_B0_tti_SUB_TREE_MAC(TTI_BIT_CNS)
    },

    /* rxDmaSum0IntSum sub tree for BC2_B0 --> different then for A0 */
    {
        PRV_CPSS_BOBCAT2_RX_DMA0_SUM_IBUF_HDR_FIFO_OVERRUN_E,
        /* override in runtime --> according to 'old oldIntCause' */
        bc2_B0_rxDmaSum0_NODE_MAC(rxDmaSum0_BIT_CNS)
    },

    /* BM General Cause Reg1 sub tree for BC2_B0 --> different then for A0 */
    {
        PRV_CPSS_BOBCAT2_BM_GEN1_SUM_IN_PROG_CT_CLEAR_FIFO_OVERRUN_CORE_0_E,
        /* override in runtime --> according to 'old oldIntCause' */
        bc2_B0_bm_gen1_SUB_TREE_MAC(bm_gen1_BIT_CNS)
    },

    /* BM General Cause Reg1 sub tree for BC2_B0 --> different then for A0 */
    {
        PRV_CPSS_BOBCAT2_BM_GEN2_SUM_CORE_0_RX_FULL_E,
        /* override in runtime --> according to 'old oldIntCause' */
        bc2_B0_bm_gen2_SUB_TREE_MAC(bm_gen2_BIT_CNS)
    },



    /* must be last */
    { LAST_ENTRY_DRV_INTERRUPT_SCAN_OVERRIDE_INFO_CNS}
};


/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 bobcat2MaskRegDefaultSummaryArr[BOBCAT2_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 bobcat2MaskRegMapArr[BOBCAT2_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};


/* get the first value of event in register (clear its 5 LSBits)*/
#define GET_FIRST_EVENT_IN_REG_MAC(_event)  ((_event) & (~0x1f))

/* the full 32 events for event that belongs to a register ,
   will have index equal to the 32 events of the register */
#define SET_INDEX_EQUAL_EVENT__FULL_REG_MAC(_event) \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 0 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 1 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 2 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 3 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 4 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 5 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 6 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 7 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 8 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 9 ),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 10),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 11),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 12),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 13),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 14),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 15),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 16),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 17),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 18),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 19),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 20),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 21),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 22),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 23),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 24),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 25),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 26),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 27),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 28),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 29),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 30),   \
    SET_INDEX_EQUAL_EVENT_MAC(GET_FIRST_EVENT_IN_REG_MAC(_event) + 31)



/* state for ports 0..71 that event hold extra index equal to event */
#define SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(_prefix,_postFix)   \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      0),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      1),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      2),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      3),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      4),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      5),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      6),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      7),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      8),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,      9),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     10),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     11),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     12),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     13),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     14),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     15),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     16),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     17),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     18),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     19),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     20),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     21),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     22),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     23),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     24),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     25),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     26),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     27),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     28),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     29),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     30),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     31),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     32),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     33),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     34),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     35),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     36),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     37),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     38),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     39),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     40),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     41),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     42),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     43),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     44),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     45),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     46),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     47),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     48),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     49),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     50),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     51),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     52),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     53),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     54),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     55),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     56),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     57),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     58),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     59),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     60),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     61),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     62),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     63),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     64),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     65),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     66),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     67),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     68),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     69),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     70),    \
    SET_INDEX_EQUAL_EVENT_PER_PORT_MAC (_prefix,_postFix,     71)


/* state for ports 0..71 that event hold extra port number */
#define SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(_prefix,_postFix)   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     0),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     1),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     2),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     3),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     4),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     5),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     6),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     7),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     8),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     9),    \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     10),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     11),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     12),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     13),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     14),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     15),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     16),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     17),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     18),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     19),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     20),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     21),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     22),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     23),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     24),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     25),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     26),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     27),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     28),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     29),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     30),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     31),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     32),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     33),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     34),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     35),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     36),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     37),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     38),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     39),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     40),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     41),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     42),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     43),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     44),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     45),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     46),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     47),   \
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(_prefix,_postFix)

/* state for even ports 48,50,52..70 that event hold extra port number */
#define SET_EVENT_PER_PORTS_48_70_EVEN__ON_ALL_PORTS_MAC(_prefix,_postFix)   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     48),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     50),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     52),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     54),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     56),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     58),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     60),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     62),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     64),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     66),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     68),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     70)

/* state for odd ports 49,51,53..71 that event hold extra port number */
#define SET_EVENT_PER_PORTS_49_71_ODD__ON_ALL_PORTS_MAC(_prefix,_postFix)   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     49),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     51),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     53),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     55),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     57),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     59),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     61),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     63),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     65),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     67),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     69),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     71)

#define SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(_prefix,_postFix)   \
        SET_EVENT_PER_PORTS_48_70_EVEN__ON_ALL_PORTS_MAC(_prefix,_postFix),  \
        SET_EVENT_PER_PORTS_49_71_ODD__ON_ALL_PORTS_MAC(_prefix,_postFix)

/* state for ports 72..255 that event hold extra port number */
#define SET_EVENT_PER_PORTS_72_TO_255___ON_ALL_PORTS_MAC(_prefix,_postFix)   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     72      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     73      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     74      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     75      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     76      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     77      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     78      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     79      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     80      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     81      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     82      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     83      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     84      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     85      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     86      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     87      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     88      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     89      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     90      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     91      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     92      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     93      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     94      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     95      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     96      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     97      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     98      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     99      ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     100     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     101     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     102     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     103     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     104     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     105     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     106     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     107     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     108     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     109     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     110     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     111     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     112     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     113     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     114     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     115     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     116     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     117     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     118     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     119     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     120     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     121     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     122     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     123     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     124     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     125     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     126     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     127     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     128     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     129     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     130     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     131     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     132     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     133     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     134     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     135     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     136     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     137     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     138     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     139     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     140     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     141     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     142     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     143     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     144     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     145     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     146     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     147     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     148     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     149     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     150     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     151     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     152     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     153     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     154     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     155     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     156     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     157     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     158     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     159     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     160     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     161     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     162     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     163     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     164     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     165     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     166     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     167     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     168     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     169     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     170     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     171     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     172     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     173     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     174     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     175     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     176     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     177     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     178     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     179     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     180     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     181     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     182     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     183     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     184     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     185     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     186     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     187     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     188     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     189     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     190     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     191     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     192     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     193     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     194     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     195     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     196     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     197     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     198     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     199     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     200     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     201     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     202     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     203     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     204     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     205     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     206     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     207     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     208     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     209     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     210     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     211     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     212     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     213     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     214     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     215     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     216     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     217     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     218     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     219     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     220     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     221     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     222     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     223     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     224     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     225     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     226     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     227     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     228     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     229     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     230     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     231     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     232     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     233     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     234     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     235     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     236     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     237     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     238     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     239     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     240     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     241     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     242     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     243     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     244     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     245     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     246     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     247     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     248     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     249     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     250     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     251     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     252     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     253     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     254     ),   \
    SET_EVENT_PER_PORT_MAC(_prefix,_postFix,     255     )

/* state for ALL 0..255 ports that event hold extra port number */
#define SET_EVENT_PER_256_PORTS___ON_ALL_PORTS_MAC(_prefix,_postFix)   \
    /* the first 72 ports */                                       \
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(_prefix,_postFix),   \
    /* the ports 72..255 */                                        \
    SET_EVENT_PER_PORTS_72_TO_255___ON_ALL_PORTS_MAC(_prefix,_postFix)

/* state for even ports 48,50,52..70 with lanes 0..3 that event hold extra port number , lane number */
#define SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(_prefix,_postFix) \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     48),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     50),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     52),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     54),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     56),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     58),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     60),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     62),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     64),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     66),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     68),   \
    SET_EVENT_PER_PORT_FOR_4_LANES_MAC(_prefix,_postFix,     70)

#define SET_EVENT_PER_INDEX____0_255___MAC(_prefix,_postFix)   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     0       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     1       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     2       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     3       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     4       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     5       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     6       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     7       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     8       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     9       ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     10      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     11      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     12      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     13      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     14      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     15      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     16      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     17      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     18      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     19      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     20      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     21      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     22      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     23      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     24      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     25      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     26      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     27      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     28      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     29      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     30      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     31      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     32      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     33      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     34      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     35      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     36      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     37      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     38      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     39      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     40      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     41      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     42      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     43      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     44      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     45      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     46      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     47      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     48      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     49      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     50      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     51      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     52      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     53      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     54      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     55      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     56      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     57      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     58      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     59      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     60      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     61      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     62      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     63      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     64      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     65      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     66      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     67      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     68      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     69      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     70      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     71      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     72      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     73      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     74      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     75      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     76      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     77      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     78      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     79      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     80      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     81      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     82      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     83      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     84      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     85      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     86      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     87      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     88      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     89      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     90      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     91      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     92      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     93      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     94      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     95      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     96      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     97      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     98      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     99      ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     100     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     101     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     102     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     103     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     104     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     105     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     106     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     107     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     108     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     109     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     110     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     111     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     112     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     113     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     114     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     115     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     116     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     117     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     118     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     119     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     120     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     121     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     122     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     123     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     124     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     125     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     126     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     127     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     128     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     129     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     130     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     131     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     132     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     133     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     134     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     135     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     136     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     137     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     138     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     139     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     140     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     141     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     142     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     143     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     144     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     145     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     146     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     147     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     148     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     149     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     150     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     151     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     152     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     153     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     154     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     155     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     156     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     157     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     158     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     159     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     160     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     161     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     162     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     163     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     164     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     165     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     166     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     167     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     168     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     169     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     170     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     171     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     172     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     173     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     174     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     175     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     176     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     177     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     178     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     179     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     180     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     181     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     182     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     183     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     184     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     185     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     186     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     187     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     188     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     189     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     190     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     191     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     192     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     193     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     194     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     195     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     196     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     197     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     198     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     199     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     200     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     201     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     202     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     203     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     204     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     205     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     206     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     207     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     208     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     209     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     210     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     211     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     212     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     213     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     214     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     215     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     216     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     217     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     218     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     219     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     220     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     221     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     222     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     223     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     224     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     225     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     226     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     227     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     228     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     229     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     230     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     231     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     232     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     233     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     234     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     235     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     236     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     237     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     238     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     239     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     240     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     241     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     242     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     243     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     244     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     245     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     246     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     247     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     248     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     249     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     250     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     251     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     252     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     253     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     254     ),   \
    SET_EVENT_PER_INDEX_MAC(_prefix,_postFix,     255     )


#define SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix,_portNum , param)   \
    _prefix##_##PORT##_##_portNum##_##_postFix,     param

/* state for 72 ports that event hold extra param as : ((portNum) << port_offset) |  ext_param */
#define SET_EVENT_PER_72_PORTS_WITH_PORT_OFFSET_AND_PARAM___ON_ALL_PORTS_MAC(_prefix,_postFix,ext_param,port_offset) \
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 0 , ((0 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 1 , ((1 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 2 , ((2 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 3 , ((3 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 4 , ((4 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 5 , ((5 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 6 , ((6 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 7 , ((7 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 8 , ((8 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 9 , ((9 << port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 10, ((10<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 11, ((11<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 12, ((12<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 13, ((13<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 14, ((14<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 15, ((15<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 16, ((16<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 17, ((17<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 18, ((18<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 19, ((19<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 20, ((20<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 21, ((21<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 22, ((22<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 23, ((23<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 24, ((24<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 25, ((25<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 26, ((26<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 27, ((27<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 28, ((28<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 29, ((29<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 30, ((30<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 31, ((31<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 32, ((32<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 33, ((33<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 34, ((34<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 35, ((35<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 36, ((36<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 37, ((37<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 38, ((38<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 39, ((39<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 40, ((40<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 41, ((41<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 42, ((42<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 43, ((43<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 44, ((44<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 45, ((45<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 46, ((46<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 47, ((47<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 48, ((48<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 49, ((49<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 50, ((50<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 51, ((51<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 52, ((52<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 53, ((53<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 54, ((54<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 55, ((55<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 56, ((56<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 57, ((57<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 58, ((58<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 59, ((59<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 60, ((60<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 61, ((61<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 62, ((62<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 63, ((63<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 64, ((64<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 65, ((65<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 66, ((66<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 67, ((67<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 68, ((68<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 69, ((69<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 70, ((70<< port_offset) | (ext_param))) ,\
    SET_EVENT_PER_PORT__WITH_PARAM_MAC(_prefix,_postFix, 71, ((71<< port_offset) | (ext_param)))

/* Interrupt cause to unified event map for Lion2
 This Table is for unified event with extended data

  Array structure:
  1. The first element of the array and the elements after MARK_END_OF_UNI_EV_CNS are unified event types.
  2. Elements after unified event type are pairs of interrupt cause and event extended data,
     until MARK_END_OF_UNI_EV_CNS.
*/
static GT_U16 bobcat2UniEvMapTableWithExtData[] = {
 /* Per Queue events */
 CPSS_PP_TX_BUFFER_QUEUE_E,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_0_E,  0,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_1_E,  1,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_2_E,  2,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_3_E,  3,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_4_E,  4,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_5_E,  5,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_6_E,  6,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_BUFFER_QUEUE_7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TX_ERR_QUEUE_E,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_ERROR_QUEUE_0_E,  0,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_ERROR_QUEUE_1_E,  1,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_ERROR_QUEUE_2_E,  2,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_ERROR_QUEUE_3_E,  3,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_ERROR_QUEUE_4_E,  4,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_ERROR_QUEUE_5_E,  5,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_ERROR_QUEUE_6_E,  6,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_ERROR_QUEUE_7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TX_END_E,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_END_QUEUE_0_E,  0,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_END_QUEUE_1_E,  1,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_END_QUEUE_2_E,  2,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_END_QUEUE_3_E,  3,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_END_QUEUE_4_E,  4,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_END_QUEUE_5_E,  5,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_END_QUEUE_6_E,  6,
    PRV_CPSS_BOBCAT2_TX_SDMA_TX_END_QUEUE_7_E,  7,
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_RX_BUFFER_QUEUE0_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_0_E,     0,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE1_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_1_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE2_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_2_E,     2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE3_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_3_E,     3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE4_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_4_E,     4,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE5_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_5_E,     5,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE6_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_6_E,     6,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE7_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_BUFFER_QUEUE_7_E,     7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE0_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_ERROR_QUEUE_0_E,      0,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE1_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_ERROR_QUEUE_1_E,      1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE2_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_ERROR_QUEUE_2_E,      2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE3_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_ERROR_QUEUE_3_E,      3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE4_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_ERROR_QUEUE_4_E,      4,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE5_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_ERROR_QUEUE_5_E,      5,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE6_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_ERROR_QUEUE_6_E,      6,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE7_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RX_ERROR_QUEUE_7_E,      7,
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_MAC_SFLOW_E,
    SET_EVENT_PER_256_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_INGRESS_STC,INGRESS_SAMPLE_LOADED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_EGRESS_SFLOW_E,
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_TXQ_DQ_EGR_STC,E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_AN_COMPLETED_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,AN_COMPLETED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_EEE_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS_WITH_PORT_OFFSET_AND_PARAM___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,PCS_RX_PATH_RECEIVED_LPI_E,0 ,8/*port<<8*/),
    SET_EVENT_PER_72_PORTS_WITH_PORT_OFFSET_AND_PARAM___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,PCS_TX_PATH_RECEIVED_LPI_E,1 ,8/*port<<8*/),
    SET_EVENT_PER_72_PORTS_WITH_PORT_OFFSET_AND_PARAM___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,MAC_RX_PATH_RECEIVED_LPI_E,2 ,8/*port<<8*/),
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_PORT_LINK_STATUS_CHANGED_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,LINK_STATUS_CHANGED_E),
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,LINK_STATUS_CHANGED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_RX_FIFO_OVERRUN_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,RX_FIFO_OVERRUN_E),
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,RX_FIFO_OVERRUN_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_TX_UNDERRUN_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,TX_UNDERRUN_E),
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,TX_UNDERRUN_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_FC_STATUS_CHANGED_E,
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,FC_STATUS_CHANGED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_ILLEGAL_SEQUENCE_E,
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,UNKNOWN_SEQUENCE_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_FAULT_TYPE_CHANGE_E,
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,FAULT_CHANGED_E),
 MARK_END_OF_UNI_EV_CNS,

  CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E,
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,ADDRESS_OUT_OF_RANGE_E),
 MARK_END_OF_UNI_EV_CNS,

  CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E,
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,NO_BUFF_PACKET_DROP_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_COUNT_COPY_DONE_E,
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,XG_COUNT_COPY_DONE_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_COUNT_EXPIRED_E,
    /* XLG ports 48..71 */
    SET_EVENT_PER_PORTS_48_71___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XLG,XG_COUNT_EXPIRED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_SYNC_STATUS_CHANGED_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,SYNC_STATUS_CHANGED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PRBS_ERROR_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,PRBS_ERROR_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_TX_FIFO_OVERRUN_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,TX_FIFO_OVERRUN_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_TX_FIFO_UNDERRUN_E,
    /* GIGA ports 0..71 */
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_GIGA,TX_FIFO_UNDERRUN_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_TXQ2_FLUSH_PORT_E,
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_TXQ_DQ_FLUSH_DONE_SUM,E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_PORT_DESC_FULL_E,
    SET_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_TXQ_PORT_DESC_SUM_DESC_FULL,E),
 MARK_END_OF_UNI_EV_CNS,

 /* Per Port XPCS events */
 /* XPCS ports even ports between 48..70 */
 CPSS_PP_PORT_PCS_LINK_STATUS_CHANGED_E,
    SET_EVENT_PER_PORTS_48_70_EVEN__ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,LINK_STATUS_CHANGED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_DESKEW_TIMEOUT_E,
    SET_EVENT_PER_PORTS_48_70_EVEN__ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,DESKEW_TIMEOUT_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_DETECTED_COLUMN_IIAII_E,
    SET_EVENT_PER_PORTS_48_70_EVEN__ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,DETECTED_COLUMN_IIAII_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_DESKEW_ERROR_E,
    SET_EVENT_PER_PORTS_48_70_EVEN__ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,DESKEW_ERROR_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E,
    SET_EVENT_PER_PORTS_48_70_EVEN__ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,PPM_FIFO_UNDERRUN_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E,
    SET_EVENT_PER_PORTS_48_70_EVEN__ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,PPM_FIFO_OVERRUN_E),
 MARK_END_OF_UNI_EV_CNS,

 /* Per Port Per lane MMPCS events */
 /* MMPCS ports even ports between 48..70 , 4 lanes per port*/
 CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
    SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,SIGNAL_DETECT_CHANGED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_PRBS_ERROR_E,
    SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,PRBS_ERROR_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_DISPARITY_ERROR_E,
    SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,DISPARITY_ERROR_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_SYMBOL_ERROR_E,
    SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,SYMBOL_ERROR_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_CJR_PAT_ERROR_E,
    SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,CJR_PAT_ERROR_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,
    SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,SIGNAL_DETECT_CHANGED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E,
    SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,SYNC_STATUS_CHANGED_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_DETECTED_IIAII_E,
    SET_EVENT_PER_PORTS_48_70_EVEN_FOR_4_LANES___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_XPCS,DETECTED_IIAII_E),
 MARK_END_OF_UNI_EV_CNS,

  /* Per Index events */
 CPSS_PP_GPP_E,
    PRV_CPSS_BOBCAT2_MISC_GPP_0_E,  0,
    PRV_CPSS_BOBCAT2_MISC_GPP_1_E,  1,
    PRV_CPSS_BOBCAT2_MISC_GPP_2_E,  2,
    PRV_CPSS_BOBCAT2_MISC_GPP_3_E,  3,
    PRV_CPSS_BOBCAT2_MISC_GPP_4_E,  4,
    PRV_CPSS_BOBCAT2_MISC_GPP_5_E,  5,
    PRV_CPSS_BOBCAT2_MISC_GPP_6_E,  6,
    PRV_CPSS_BOBCAT2_MISC_GPP_7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_CNC_WRAPAROUND_BLOCK_E,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK0_WRAPAROUND_E,    0,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK1_WRAPAROUND_E,    1,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK2_WRAPAROUND_E,    2,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK3_WRAPAROUND_E,    3,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK4_WRAPAROUND_E,    4,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK5_WRAPAROUND_E,    5,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK6_WRAPAROUND_E,    6,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK7_WRAPAROUND_E,    7,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK8_WRAPAROUND_E,    8,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK9_WRAPAROUND_E,    9,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK10_WRAPAROUND_E,  10,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK11_WRAPAROUND_E,  11,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK12_WRAPAROUND_E,  12,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK13_WRAPAROUND_E,  13,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK14_WRAPAROUND_E,  14,
    PRV_CPSS_BOBCAT2_CNC0_WRAPAROUND_SUM_BLOCK15_WRAPAROUND_E,  15,

    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK0_WRAPAROUND_E,   16,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK1_WRAPAROUND_E,   17,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK2_WRAPAROUND_E,   18,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK3_WRAPAROUND_E,   19,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK4_WRAPAROUND_E,   20,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK5_WRAPAROUND_E,   21,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK6_WRAPAROUND_E,   22,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK7_WRAPAROUND_E,   23,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK8_WRAPAROUND_E,   24,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK9_WRAPAROUND_E,   25,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK10_WRAPAROUND_E,  26,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK11_WRAPAROUND_E,  27,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK12_WRAPAROUND_E,  28,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK13_WRAPAROUND_E,  29,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK14_WRAPAROUND_E,  30,
    PRV_CPSS_BOBCAT2_CNC1_WRAPAROUND_SUM_BLOCK15_WRAPAROUND_E,  31,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_BM_MISC_E,
    PRV_CPSS_BOBCAT2_BM_GEN1_SUM_GLOBAL_RX_FULL_E,           4,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_CNTR_OVERFLOW_E,
    PRV_CPSS_BOBCAT2_RX_SDMA_RESOURCE_ERROR_CNT_OF_E,  0,
    PRV_CPSS_BOBCAT2_RX_SDMA_BYTE_CNT_OF_E,            1,
    PRV_CPSS_BOBCAT2_RX_SDMA_PACKET_CNT_OF_E,          2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_MISC_E,
    PRV_CPSS_BOBCAT2_TXQ_GEN_SUM_QUEUE_WRONG_ADDR_E,    0,
    PRV_CPSS_BOBCAT2_TXQ_GEN_SUM_GLOBAL_DESC_FULL_E,    1,
    PRV_CPSS_BOBCAT2_TXQ_GEN_SUM_GLOBAL_BUFF_FULL_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_SCT_RATE_LIMITER_E,
    SET_EVENT_PER_INDEX____0_255___MAC(PRV_CPSS_BOBCAT2_CPU_CODE_RATE_LIMITERS_SCT_RATE_LIMITER,PKT_DROPED_E),
 MARK_END_OF_UNI_EV_CNS,

  CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E,
    PRV_CPSS_BOBCAT2_IPLR0_ADDRESS_OUT_OF_MEMORY_E,    0,
    PRV_CPSS_BOBCAT2_EPLR_SUM_ADDRESS_OUT_OF_MEMORY_E,     1,
    PRV_CPSS_BOBCAT2_IPLR1_SUM_ADDRESS_OUT_OF_MEMORY_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_DATA_ERR_E,
    /* PRV_CPSS_BOBCAT2_IPLR0_DATA_ERROR_E,    0, --> bound in CPSS_PP_DATA_INTEGRITY_ERROR_E (like in Lion2) */
    PRV_CPSS_BOBCAT2_EPLR_SUM_DATA_ERROR_E,     1,
    /* PRV_CPSS_BOBCAT2_IPLR1_SUM_DATA_ERROR_E,    2, --> bound in CPSS_PP_DATA_INTEGRITY_ERROR_E (like in Lion2)*/
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E,
    PRV_CPSS_BOBCAT2_IPLR0_IPFIX_WRAPAROUND_E,    0,
    PRV_CPSS_BOBCAT2_EPLR_SUM_IPFIX_WRAPAROUND_E,     1,
    PRV_CPSS_BOBCAT2_IPLR1_SUM_IPFIX_WRAPAROUND_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_ALARM_E,
    PRV_CPSS_BOBCAT2_IPLR0_IPFIX_ALARM_E,    0,
    PRV_CPSS_BOBCAT2_EPLR_SUM_IPFIX_ALARM_E,     1,
    PRV_CPSS_BOBCAT2_IPLR1_SUM_IPFIX_ALARM_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
    PRV_CPSS_BOBCAT2_IPLR0_IPFIX_ALARMED_ENTRIES_FIFO_WRAPAROUND_E,    0,
    PRV_CPSS_BOBCAT2_EPLR_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAPAROUND_E,     1,
    PRV_CPSS_BOBCAT2_IPLR1_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAPAROUND_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PCL_LOOKUP_DATA_ERROR_E,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL0_ACTION0_TCAM_ACCESS_DATA_ERROR_E  , 0,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL0_ACTION1_TCAM_ACCESS_DATA_ERROR_E  , 0,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL0_ACTION2_TCAM_ACCESS_DATA_ERROR_E  , 0,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL0_ACTION3_TCAM_ACCESS_DATA_ERROR_E  , 0,

    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL1_ACTION0_TCAM_ACCESS_DATA_ERROR_E  , 1,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL1_ACTION1_TCAM_ACCESS_DATA_ERROR_E  , 1,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL1_ACTION2_TCAM_ACCESS_DATA_ERROR_E  , 1,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL1_ACTION3_TCAM_ACCESS_DATA_ERROR_E  , 1,

    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL2_ACTION0_TCAM_ACCESS_DATA_ERROR_E  , 2,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL2_ACTION1_TCAM_ACCESS_DATA_ERROR_E  , 2,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL2_ACTION2_TCAM_ACCESS_DATA_ERROR_E  , 2,
    PRV_CPSS_BOBCAT2_IPCL_SUM_PCL2_ACTION3_TCAM_ACCESS_DATA_ERROR_E  , 2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PCL_LOOKUP_FIFO_FULL_E,
    PRV_CPSS_BOBCAT2_IPCL_SUM_LOOKUP0_FIFO_FULL_E,       0,
    PRV_CPSS_BOBCAT2_IPCL_SUM_LOOKUP1_FIFO_FULL_E,     1,
    PRV_CPSS_BOBCAT2_IPCL_SUM_LOOKUP2_FIFO_FULL_E,       2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_EB_NA_FIFO_FULL_E,
    PRV_CPSS_BOBCAT2_FDB_NA_FIFO_FULL0_E    , 0 ,
    PRV_CPSS_BOBCAT2_FDB_NA_FIFO_FULL1_E    , 1 ,
    PRV_CPSS_BOBCAT2_FDB_NA_FIFO_FULL2_E    , 2 ,
    PRV_CPSS_BOBCAT2_FDB_NA_FIFO_FULL3_E    , 3 ,
    PRV_CPSS_BOBCAT2_FDB_NA_FIFO_FULL4_E    , 4 ,
    PRV_CPSS_BOBCAT2_FDB_NA_FIFO_FULL5_E    , 5 ,
    PRV_CPSS_BOBCAT2_FDB_NA_FIFO_FULL6_E    , 6 ,
    PRV_CPSS_BOBCAT2_FDB_NA_FIFO_FULL7_E    , 7 ,
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_TTI_ACCESS_DATA_ERROR_E,
     PRV_CPSS_BOBCAT2_TTI_SUM_TTI0_ACCESS_DATA_ERROR_E, 0,
     PRV_CPSS_BOBCAT2_TTI_SUM_TTI1_ACCESS_DATA_ERROR_E, 1,
 MARK_END_OF_UNI_EV_CNS,

 /* next HW interrupt value will have same index (evExtData) value in CPSS_PP_CRITICAL_HW_ERROR_E */
 CPSS_PP_CRITICAL_HW_ERROR_E,
/* The following event is in valid traffic case. The event is when allocation counter get to total buffers limit.
   This is legal situation for stress test scenarios.
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_RX_DMA0_SUM_GLOBAL_ALLOC_COUNTER_OVERFLOW_E), */
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_RX_DMA0_SUM_CREDIT_COUNTER_OVERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_RX_DMA0_SUM_IBUF_TRANS_FIFO_OVERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_RX_DMA0_SUM_IBUF_DESC_FIFO_OVERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_RX_DMA0_SUM_IBUF2_SCDMA_HDR_RAM_RD_MASK_OVERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_RX_DMA0_SUM_IBUF_HDR_FIFO_OVERRUN_E),

    SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_RX_DMA_SCDMA, SUM_FIRST_PACKET_CHUNK_WITHOUT_SOP_E),
    SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_RX_DMA_SCDMA, SUM_PACKET_WITH_NO_EOP_E),
/*  The following event is in valid traffic case. Should be verified in HW.
    SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_RX_DMA_SCDMA, SUM_PORT_BUF_CNT_UNDERFLOW_E), */
    SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_RX_DMA_SCDMA, SUM_PORT_BUF_CNT_OVERFLOW_E),
    SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_RX_DMA_SCDMA, SUM_SOP_DESC_FIFO_OVERRUN_E),
    SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_RX_DMA_SCDMA, SUM_EOP_DESC_FIFO_OVERRUN_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_RX_CLIENT_0_SUM_CREDITS_COUNTER_UNDERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_RX_CLIENT_0_SUM_RX_LB_TOKEN_FIFO_OVERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_RX_CLIENT_0_SUM_RX_WR_DATA_TO_BANK_WITHOUT_VALID_BANK_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_RX_CLIENT_0_SUM_RX_IDDB_LATENCY_FIFO_OVERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_RX_CLIENT_0_SUM_RX_PHYSICAL_BANK_SYNC_FIFO_OVERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_RX_CLIENT_0_SUM_RX_IDDB_WR_ID_FIFO_UNDERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_TX_CLIENT_0_SUM_TX_RD_BURST_FIFO_OVERRUN_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_BANK_0_SUM_BANK_LATENCY_FIFO_OVERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_BANK_1_SUM_BANK_LATENCY_FIFO_OVERRUN_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP0_COUNTERS_PARITY_ERR_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_CNTR_OVER_FLOW_SUM_PFC_PORT_GROUP0_UNDERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_CNTR_OVER_FLOW_SUM_PFC_PORT_GROUP0_OVERFLOW_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_0_UNDER_FLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC0_UNDERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_0_UNDER_FLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC1_UNDERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_0_UNDER_FLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC2_UNDERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_0_UNDER_FLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC3_UNDERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_0_UNDER_FLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC4_UNDERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_0_UNDER_FLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC5_UNDERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_0_UNDER_FLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC6_UNDERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PORT_GROUP_0_UNDER_FLOW_SUM_PFC_PORT_GROUP0_GLOBAL_PFC_TC7_UNDERFLOW_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BM_GEN1_SUM_CORE_0_INVALID_BUFFER_CLEAR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BM_GEN1_SUM_CORE_0_NEXT_TABLE_UPDATE_WAS_LOST_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BMA_SUM_MC_CNT_COUNTER_FLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BMA_SUM_SHIFTER_LATENCY_FIFO_OVERFLOW_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BMA_SUM_MC_CNT_PARITY_ERROR_E),

    /*full register*/    SET_INDEX_EQUAL_EVENT__FULL_REG_MAC(PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN1_SUM_SC_DMA_ID_FIFO_OVERRUN_REG1_SUMMARY_E),
    /*full register*/    SET_INDEX_EQUAL_EVENT__FULL_REG_MAC(PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN2_SUM_SC_DMA_ID_FIFO_OVERRUN_REG2_SUMMARY_E),
    /*full register*/    SET_INDEX_EQUAL_EVENT__FULL_REG_MAC(PRV_CPSS_BOBCAT2_TXDMA_FIFO_OVERRUN3_SUM_SC_DMA_ID_FIFO_OVERRUN_REG3_SUMMARY_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXDMA_RX_DMA_UPDATE1_SUM_RX_DMA_0_UPDATES_FIF_OS_OVERRUN_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXDMA_GENENAL_SUM_TX_Q_TO_TX_DMA_HA_DESC_OVERRUN_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXDMA_GENENAL_SUM_TX_Q_TO_TX_DMA_DESC_OVERRUN_E),

    /*full register*/    SET_INDEX_EQUAL_EVENT__FULL_REG_MAC(PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY1_SUM_SHIFTER_SOP_EOP_FIFO_OVERRUNT_REG1_SUMMARY_E),
    /*full register*/    SET_INDEX_EQUAL_EVENT__FULL_REG_MAC(PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY2_SUM_SHIFTER_SOP_EOP_FIFO_OVERRUNT_REG2_SUMMARY_E),
    /*full register*/    SET_INDEX_EQUAL_EVENT__FULL_REG_MAC(PRV_CPSS_BOBCAT2_TX_FIFO_NOT_READY3_SUM_SHIFTER_SOP_EOP_FIFO_OVERRUNT_REG3_SUMMARY_E),

/*  The following events are in valid traffic case. Should be verified in HW.
    SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_PTP, FIFO_READ_ERROR_E),
    SET_INDEX_EQUAL_EVENT_PER_72_PORTS___ON_ALL_PORTS_MAC(PRV_CPSS_BOBCAT2_PTP, FIFO_WRITE_ERROR_E), */

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_ILKN_WRAPPER_SERIAL_ERROR_E),

 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_DATA_INTEGRITY_ERROR_E,
    /* -- _ECC_ --*/

/*full register*/    SET_INDEX_EQUAL_EVENT__FULL_REG_MAC(PRV_CPSS_BOBCAT2_CRITICAL_ECC_ERROR_BM_CRITICAL_ECC_SUM_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TTI_SUM_CRITICAL_ECC_ONE_ERROR_CORRECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TTI_SUM_CRITICAL_ECC_TWO_ERROR_DETECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_HA_SUM_ECC_SINGLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_HA_SUM_ECC_DOUBLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MLL_SUM__FILE_ECC_1_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MLL_SUM__FILE_ECC_2_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_EFT_SUM_DESC_ECC_SINGLE_ERROR_DETECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_EFT_SUM_DESC_ECC_DOUBLE_ERROR_DETECTED_INT_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXQ_GEN_SUM_TD_CLR_ECC_ONE_ERROR_CORRECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXQ_GEN_SUM_TD_CLR_ECC_TWO_ERROR_DETECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP0_PFC_IND_FIFO_ECC_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP1_PFC_IND_FIFO_ECC_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP2_PFC_IND_FIFO_ECC_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP3_PFC_IND_FIFO_ECC_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP4_PFC_IND_FIFO_ECC_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP5_PFC_IND_FIFO_ECC_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP6_PFC_IND_FIFO_ECC_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP7_PFC_IND_FIFO_ECC_ERR_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_QCN_SUM_DESC_ECC_SINGLE_ERROR_DETECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_QCN_SUM_DESC_ECC_DOUBLE_ERROR_DETECTED_INT_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXQ_DQ_GEN_SUM_DQ_INC_ECC_ONE_ERROR_CORRECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXQ_DQ_GEN_SUM_DQ_INC_ECC_TWO_ERROR_DETECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXQ_DQ_GEN_SUM_DQ_CLR_ECC_ONE_ERROR_CORRECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXQ_DQ_GEN_SUM_DQ_CLR_ECC_TWO_ERROR_DETECTED_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXDMA_GENENAL_SUM_ECC_SINGLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXDMA_GENENAL_SUM_ECC_DOUBLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TX_FIFO_GEBERAL1_SUM_ECC_0_SINGLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TX_FIFO_GEBERAL1_SUM_ECC_1_SINGLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TX_FIFO_GEBERAL1_SUM_ECC_0_DOUBLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TX_FIFO_GEBERAL1_SUM_ECC_1_DOUBLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_SUM_ECC_ERROR_SUM_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_ECC_SUM_MPPM_ECC_DOUBLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_MPPM_ECC_SUM_MPPM_ECC_SINGLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_ETH_TXFIFO_GEN_SUM_ECC_SINGLE_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_ETH_TXFIFO_GEN_SUM_ECC_DOUBLE_ERROR_E),

    /* -- _PARITY_ -- */
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TCAM_LOGIC_SUM_TCAM_ARRAY_PARITY_ERROR_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP0_COUNTERS_PARITY_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP1_COUNTERS_PARITY_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP2_COUNTERS_PARITY_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP3_COUNTERS_PARITY_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP4_COUNTERS_PARITY_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP5_COUNTERS_PARITY_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP6_COUNTERS_PARITY_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_PFC_PARITY_SUM_PFC_PORT_GROUP7_COUNTERS_PARITY_ERR_E),

    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_QCN_SUM_CN_BUFFER_FIFO_PARITY_ERR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_TXQ_DQ_MEM_ERR_SUM_TOKEN_BUCKET_PRIO_PARITY_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BM_GEN1_SUM_CORE_0_VALID_TABLE_PARITY_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BM_GEN1_SUM_CORE_1_VALID_TABLE_PARITY_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BM_GEN1_SUM_CORE_2_VALID_TABLE_PARITY_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BM_GEN1_SUM_CORE_3_VALID_TABLE_PARITY_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BM_GEN1_SUM_CORE_4_VALID_TABLE_PARITY_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BM_GEN1_SUM_CORE_5_VALID_TABLE_PARITY_ERROR_E),
    /* BMA is internal unit -- SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_BMA_SUM_MC_CNT_PARITY_ERROR_E),*/

    /*-- DATA_ERROR --*/

    /* no other _DATA_ERROR_ beside those that already bound in:
        CPSS_PP_PCL_LOOKUP_DATA_ERROR_E,
        CPSS_PP_TTI_ACCESS_DATA_ERROR_E,
        CPSS_PP_POLICER_DATA_ERR_E
    */
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_IPLR0_DATA_ERROR_E),
    SET_INDEX_EQUAL_EVENT_MAC(PRV_CPSS_BOBCAT2_IPLR1_SUM_DATA_ERROR_E),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_OAM_EXCEPTION_KEEPALIVE_AGING_E,
    PRV_CPSS_BOBCAT2_INGR_OAM_SUM_KEEP_ALIVE_AGING_E,       0,
    PRV_CPSS_BOBCAT2_EGR_OAM_SUM_KEEP_ALIVE_AGING_E,        1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_OAM_EXCEPTION_EXCESS_KEEPALIVE_E,
    PRV_CPSS_BOBCAT2_INGR_OAM_SUM_EXCESS_KEEPALIVE_E,       0,
    PRV_CPSS_BOBCAT2_EGR_OAM_SUM_EXCESS_KEEPALIVE_E,        1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_OAM_EXCEPTION_INVALID_KEEPALIVE_E,
    PRV_CPSS_BOBCAT2_INGR_OAM_SUM_INVALID_KEEPALIVE_HASH_E, 0,
    PRV_CPSS_BOBCAT2_EGR_OAM_SUM_INVALID_KEEPALIVE_HASH_E,  1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_OAM_EXCEPTION_RDI_STATUS_E,
    PRV_CPSS_BOBCAT2_INGR_OAM_SUM_RDI_STATUS_E,             0,
    PRV_CPSS_BOBCAT2_EGR_OAM_SUM_RDI_STATUS_E,              1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_OAM_EXCEPTION_TX_PERIOD_E,
    PRV_CPSS_BOBCAT2_INGR_OAM_SUM_TX_PERIOD_E,              0,
    PRV_CPSS_BOBCAT2_EGR_OAM_SUM_TX_PERIOD_E,               1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_OAM_EXCEPTION_MEG_LEVEL_E,
    PRV_CPSS_BOBCAT2_INGR_OAM_SUM_MEG_LEVEL_EXCEPTION_E,    0,
    PRV_CPSS_BOBCAT2_EGR_OAM_SUM_MEG_LEVEL_EXCEPTION_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_OAM_EXCEPTION_SOURCE_INTERFACE_E,
    PRV_CPSS_BOBCAT2_INGR_OAM_SUM_SOURCE_ERFACE_EXCEPTION_E,   0,
    PRV_CPSS_BOBCAT2_EGR_OAM_SUM_SOURCE_ERFACE_EXCEPTION_E,    1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_OAM_ILLEGAL_ENTRY_INDEX_E,
    PRV_CPSS_BOBCAT2_INGR_OAM_SUM_ILLEGAL_OAM_ENTRY_INDEX_E,    0,
    PRV_CPSS_BOBCAT2_EGR_OAM_SUM_ILLEGAL_OAM_ENTRY_INDEX_E,     1,
 MARK_END_OF_UNI_EV_CNS

};
/* Interrupt cause to unified event map for Bobcat2 with extended data size */
static GT_U32 bobcat2UniEvMapTableWithExtDataSize = (sizeof(bobcat2UniEvMapTableWithExtData)/(sizeof(GT_U16)));


/* Interrupt cause to unified event map for Bobcat2
 This Table is for unified event without extended data
*/
static GT_U16 bobcat2UniEvMapTable[][2] =
{
 {CPSS_PP_MISC_TWSI_TIME_OUT_E,          PRV_CPSS_BOBCAT2_MISC_TWSI_TIME_OUT_E},
 {CPSS_PP_MISC_TWSI_STATUS_E,            PRV_CPSS_BOBCAT2_MISC_TWSI_STATUS_E},
 {CPSS_PP_MISC_ILLEGAL_ADDR_E,           PRV_CPSS_BOBCAT2_MISC_ILLEGAL_ADDR_E},
/*
 {CPSS_PP_MISC_CPU_PORT_RX_OVERRUN_E,    PRV_CPSS_BOBCAT2_CPU_PORT_SUM_RX_FIFO_OVERRUN_E},
 {CPSS_PP_BM_AGED_PACKET_E,              PRV_CPSS_BOBCAT2_BM_0_AGED_PACKET_E},
 {CPSS_PP_BM_MAX_BUFF_REACHED_E,         PRV_CPSS_BOBCAT2_BM_0_MAX_BUFF_REACHED_E},
 {CPSS_PP_BM_WRONG_SRC_PORT_E,           PRV_CPSS_BOBCAT2_BM_0_WRONG_SRC_PORT_E},
*/
 {CPSS_PP_TQ_SNIFF_DESC_DROP_E,          PRV_CPSS_BOBCAT2_TXQ_DQ_GEN_SUM_EGRESS_MIRORR_DESC_DROP_E},

 {CPSS_PP_EB_SECURITY_BREACH_UPDATE_E,   PRV_CPSS_BOBCAT2_BRIDGE_UPDATE_SECURITY_BREACH_E},
 {CPSS_PP_MAC_NUM_OF_HOP_EXP_E,          PRV_CPSS_BOBCAT2_FDB_NUM_OF_HOP_EX_P_E},
 {CPSS_PP_MAC_NA_LEARNED_E,              PRV_CPSS_BOBCAT2_FDB_NA_LEARNT_E},
 {CPSS_PP_MAC_NA_NOT_LEARNED_E,          PRV_CPSS_BOBCAT2_FDB_NA_NOT_LEARNT_E},
 {CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E,   PRV_CPSS_BOBCAT2_FDB_AGE_VIA_TRIGGER_ENDED_E},
 {CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E,    PRV_CPSS_BOBCAT2_FDB_AU_PROC_COMPLETED_E},
 {CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E,    PRV_CPSS_BOBCAT2_FDB_AU_MSG_TOCPU_READY_E},
 {CPSS_PP_MAC_NA_SELF_LEARNED_E,         PRV_CPSS_BOBCAT2_FDB_NA_SELF_LEARNED_E},
 {CPSS_PP_MAC_NA_FROM_CPU_LEARNED_E,     PRV_CPSS_BOBCAT2_FDB_N_AFROM_CPU_LEARNED_E},
 {CPSS_PP_MAC_NA_FROM_CPU_DROPPED_E,     PRV_CPSS_BOBCAT2_FDB_N_AFROM_CPU_DROPPED_E},
 {CPSS_PP_MAC_AGED_OUT_E,                PRV_CPSS_BOBCAT2_FDB_AGED_OUT_E},
 {CPSS_PP_MAC_FIFO_2_CPU_EXCEEDED_E,     PRV_CPSS_BOBCAT2_FDB_AU_FIFO_TO_CPU_IS_FULL_E},
 {CPSS_PP_EB_AUQ_PENDING_E,              PRV_CPSS_BOBCAT2_MISC_AUQ_PENDING_E},
 {CPSS_PP_EB_AUQ_FULL_E,                 PRV_CPSS_BOBCAT2_MISC_AU_QUEUE_FULL_E},
 {CPSS_PP_EB_AUQ_OVER_E,                 PRV_CPSS_BOBCAT2_MISC_AUQ_OVERRUN_E},
 {CPSS_PP_EB_AUQ_ALMOST_FULL_E,          PRV_CPSS_BOBCAT2_MISC_AUQ_ALMOST_FULL_E},
 {CPSS_PP_EB_FUQ_PENDING_E,              PRV_CPSS_BOBCAT2_MISC_FUQ_PENDING_E},
 {CPSS_PP_EB_FUQ_FULL_E,                 PRV_CPSS_BOBCAT2_MISC_FU_QUEUE_FULL_E},
 {CPSS_PP_MISC_GENXS_READ_DMA_DONE_E,    PRV_CPSS_BOBCAT2_MISC_GENXS_READ_DMA_DONE_E},
 {CPSS_PP_MISC_PEX_ADDR_UNMAPPED_E,      PRV_CPSS_BOBCAT2_MISC_PEX_ADDR_UNMAPPED_E},
 {CPSS_PP_EB_MG_ADDR_OUT_OF_RANGE_E,     PRV_CPSS_BOBCAT2_BRIDGE_ADDRESS_OUT_OF_RANGE_E},
/*
 {CPSS_PP_CNC_DUMP_FINISHED_E,           PRV_CPSS_BOBCAT2_CNC_DUMP_FINISHED_E},
*/
 {CPSS_PP_TTI_CPU_ADDRESS_OUT_OF_RANGE_E,PRV_CPSS_BOBCAT2_TTI_SUM_CPU_ADDRESS_OUT_OF_RANGE_E},

 {CPSS_PP_PCL_MG_ADDR_OUT_OF_RANGE_E,    PRV_CPSS_BOBCAT2_IPCL_SUM_MG_ADDR_OUT_OF_RANGE_E},
 {CPSS_PP_PORT_802_3_AP_E,               PRV_CPSS_BOBCAT2_MISC_Z80_INTERRUPT_E},
 {CPSS_PP_MAC_BANK_LEARN_COUNTERS_OVERFLOW_E, PRV_CPSS_BOBCAT2_FDB_BLC_OVERFLOW_E}/* new in Bobcat2; Caelum; Bobcat3 */
};

/* Interrupt cause to unified event map for Bobcat2 without extended data size */
static GT_U32 bobcat2UniEvMapTableSize = (sizeof(bobcat2UniEvMapTable)/(sizeof(GT_U16)*2));


/*******************************************************************************
* setBobcat2DedicatedEventsConvertInfo
*
* DESCRIPTION:
*      set info needed by chIntCauseToUniEvConvertDedicatedTables(...) for the
*      Bobcat2 devices.
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
static void setBobcat2DedicatedEventsConvertInfo(IN GT_U8   devNum)
{
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataArr   = bobcat2UniEvMapTableWithExtData;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataSize  = bobcat2UniEvMapTableWithExtDataSize;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableArr              = bobcat2UniEvMapTable;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableSize             = bobcat2UniEvMapTableSize;
    prvCpssDrvPpConfig[devNum]->intCtrl.numReservedPorts              = 72;

    return;
}

/*******************************************************************************
* skipSubTreeOfValidSubTree
*
* DESCRIPTION:
*       set indication on sub tree that it should be ignored (skipped).
*
* INPUTS:
*       fullTreePtr - pointer to the 'Full tree' of interrupts
*       numElementsInFullTree - number of elements in the 'Full tree'
*       subTreeIndex - index in the 'Full tree' that represent the 'Sub tree'
*                   which need invalidation.
* OUTPUTS:
*       lastIndexSubTreePtr - (pointer to) Index in the 'Full tree' which belong
*                           to the last Index in the 'Sub Tree'.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_STATE - algorithm / DB error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS skipSubTreeOfValidSubTree
(
    IN PRV_CPSS_DRV_INTERRUPT_SCAN_STC    *fullTreePtr ,
    IN GT_U32    numElementsInFullTree,
    IN GT_U32    subTreeIndex,
    OUT GT_U32   *lastIndexSubTreePtr
)
{
    GT_STATUS   rc;
    GT_U32  ii, iiNew;
    CPSS_INTERRUPT_SCAN_STC *currentElementPtr;
    GT_U32  count;

    if(subTreeIndex >= numElementsInFullTree)
    {
        /* algorithm / DB error */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    currentElementPtr = &fullTreePtr[subTreeIndex];
    count = currentElementPtr->subIntrListLen;

    /* make the start of the sub tree not valid */
    *currentElementPtr = nonValidEntry;

    for(ii = (subTreeIndex + 1);/* skip start of sub tree */
       (ii < numElementsInFullTree) && count ;
       ii++)
    {
        currentElementPtr = &fullTreePtr[ii];

        if(currentElementPtr->subIntrListLen) /* sub tree */
        {
            /* let recursive on the sub tree to invalidate it */
            rc = skipSubTreeOfValidSubTree(fullTreePtr,numElementsInFullTree,ii,&iiNew);
            if(rc != GT_OK)
            {
                return rc;
            }

            ii = iiNew;/* last index that is in the sub tree ...
                          with the '++' of the loop it will become 'first index out the tree' */
        }
        else /* leaf . not sub tree */
        {
            /* make the entry to be not valid */
            *currentElementPtr = nonValidEntry;
        }

        count--;/* indicate that another child (leaf/sub tree) was done */
    }

    *lastIndexSubTreePtr = ii-1;/* we return value of 'last Index in the Sub Tree'*/

    return GT_OK;
}

/*******************************************************************************
* replaceSubTree
*
* DESCRIPTION:
*       replace in 'main tree' 'old sub tree' with 'new sub tree'
*       NOTE: the 'old sub tree' must be larger the the 'new sub tree'
*
* INPUTS:
*       fullTreePtr - pointer to the 'Full tree' of interrupts
*       numElementsInFullTree - number of elements in the 'Full tree'
*       numElementsInOldSubTree - number of elements in the 'old sub tree'
*       newSubTreePtr - pointer to the 'new sub tree' of interrupts
*       numElementsInNewSubTree - number of elements in the 'new sub tree'
*       subTreeIndex - index in the 'Full tree' that represent the 'Old sub tree'
*                   which need replacement.
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - the 'old sub tree' smaller then the 'new sub tree'
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS replaceSubTree
(
    IN PRV_CPSS_DRV_INTERRUPT_SCAN_STC    *fullTreePtr ,
    IN GT_U32    numElementsInFullTree,
    IN GT_U32    numElementsInOldSubTree,
    IN PRV_CPSS_DRV_INTERRUPT_SCAN_STC    *newSubTreePtr ,
    IN GT_U32    numElementsInNewSubTree,
    IN GT_U32    subTreeIndex
)
{
    GT_U32  ii,jj;
    CPSS_INTERRUPT_SCAN_STC *currentElementPtr;
    GT_U32  count;

    if((numElementsInOldSubTree < numElementsInNewSubTree) ||
        ((subTreeIndex + numElementsInOldSubTree) > numElementsInFullTree))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    count = numElementsInNewSubTree;

    currentElementPtr = &fullTreePtr[subTreeIndex];

    for(ii = subTreeIndex,jj= 0 ;
       (ii < numElementsInFullTree) && count ;
       ii++,jj++,currentElementPtr++)
    {
        /*copy element from the new sub tree */
        *currentElementPtr = newSubTreePtr[jj];

        count--;
    }

    count = numElementsInOldSubTree - numElementsInNewSubTree;

    /* continue with setting the 'delta' between old and new entries with 'skip' entries */
    for(/*continue ii , currentElementPtr*/ ;
       (ii < numElementsInFullTree) && count ;
       ii++,currentElementPtr++)
    {
        /*copy element from the new sub tree */
        *currentElementPtr = nonValidEntry;

        count--;
    }

    return GT_OK;
}

/*******************************************************************************
* jumpSubTree
*
* DESCRIPTION:
*       jump over sub tree.
*
* INPUTS:
*       fullTreePtr - pointer to the 'Full tree' of interrupts
*       numElementsInFullTree - number of elements in the 'Full tree'
*       subTreeIndex - index in the 'Full tree' that represent the 'Sub tree'
*                   which need to jump over.
* OUTPUTS:
*       lastIndexSubTreePtr - (pointer to) Index in the 'Full tree' which belong
*                           to the last Index in the 'Sub Tree'.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_STATE - algorithm / DB error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS jumpSubTree
(
    IN PRV_CPSS_DRV_INTERRUPT_SCAN_STC    *fullTreePtr ,
    IN GT_U32    numElementsInFullTree,
    IN GT_U32    subTreeIndex,
    OUT GT_U32   *lastIndexSubTreePtr
)
{
    GT_STATUS   rc;
    GT_U32  ii, iiNew;
    CPSS_INTERRUPT_SCAN_STC *currentElementPtr;
    GT_U32  count;

    if(subTreeIndex >= numElementsInFullTree)
    {
        /* algorithm / DB error */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    currentElementPtr = &fullTreePtr[subTreeIndex];
    if(currentElementPtr->maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
    {
        /* jump over 'skipped' sub tree */
        ii = subTreeIndex;
        do{
            currentElementPtr++;
            ii++;

            if(ii == numElementsInFullTree)
            {
                /* we got to end of tree */
                break;
            }
        }
        while(currentElementPtr->maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS);


        *lastIndexSubTreePtr = ii-1;/* we return value of 'last Index in the Sub Tree'*/

        return GT_OK;
    }

    count = currentElementPtr->subIntrListLen;

    for(ii = (subTreeIndex + 1);/* skip start of sub tree */
       (ii < numElementsInFullTree) && count ;
       ii++)
    {
        currentElementPtr = &fullTreePtr[ii];

        if(currentElementPtr->maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
        {
            /* skip this entry */
            continue;
        }
        else
        if(currentElementPtr->subIntrListLen) /* sub tree */
        {
            /* let recursive on the sub tree to jump it */
            rc = jumpSubTree(fullTreePtr,numElementsInFullTree,ii,&iiNew);
            if(rc != GT_OK)
            {
                return rc;
            }

            ii = iiNew;/* last index that is in the sub tree ...
                          with the '++' of the loop it will become 'first index out the tree' */
        }
        else /* leaf . not sub tree */
        {
            /* nothing to do */
        }

        count--;/* indicate that another child (leaf/sub tree) was done */
    }

    *lastIndexSubTreePtr = ii-1;/* we return value of 'last Index in the Sub Tree'*/

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvPpIntRemoveSubTreeList
*
* DESCRIPTION:
*       'remove' of array of sub trees from the interrupts tree. (state nodes as 'skipped')
*       this is useful for new silicon revision that relay on previous revision with
*       'minor' changes in the interrupts tree
*
* INPUTS:
*       fullTreePtr - pointer to the 'Full tree' of interrupts
*       numElementsInFullTree - number of elements in the 'Full tree'
*       removeSubTreeByEventArrPtr - array of event represents sub trees to remove from the 'full tree'.
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_STATE - algorithm / DB error
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntRemoveSubTreeList
(
    IN PRV_CPSS_DRV_INTERRUPT_SCAN_STC    *fullTreePtr ,
    IN GT_U32    numElementsInFullTree,
    IN GT_U32    *removeSubTreeByEventArrPtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32  ii,jj,kk,orig_jj;/* index iterator */
    GT_U32  currentEvent,bitIndex;

    /* loop on removed elements and find it in the 'full tree' */
    for(ii = 0 ; removeSubTreeByEventArrPtr[ii] != LAST_ENTRY_INT_CAUSE_CNS ; ii++)
    {
        currentEvent = removeSubTreeByEventArrPtr[ii];

        /* loop in the full tree look for the event */
        for(jj = 0 ; jj < numElementsInFullTree ; jj++)
        {
            if((currentEvent >> 5) != (fullTreePtr[jj].startIdx >> 5))
            {
                /* the event is not in this register */
                continue;
            }

            /* found the register that hold this event ... need to remove sub
               tree according to proper bit index */
            bitIndex = currentEvent & 0x1f;

            orig_jj = jj;

            jj++;/* go to first element of the register */

            /* we need to find the 'need to remove' sub tree in the next sub trees of current sub tree */
            for(kk = 0; kk < fullTreePtr[orig_jj].subIntrListLen ;kk++)
            {
                if(fullTreePtr[jj].maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
                {
                    /* jump over the 'already skipped' sub tree without changing it */
                    jumpSubTree(fullTreePtr,numElementsInFullTree,
                        jj/* current index in the table*/,&jj/*index in fullTreePtr[] of last entry in the 'sub tree'*/);

                    /* jj is last index that is in the sub tree ...
                      need to add 1 so it become 'first index out the tree' */
                    jj+=1;

                    /* we not consider this as 'child' !!!
                       so do not count this iterator. */
                    kk--;
                }
                else
                if(fullTreePtr[jj].bitNum != bitIndex)
                {
                    /* jump over the current sub tree without changing it */
                    jumpSubTree(fullTreePtr,numElementsInFullTree,
                        jj/* current index in the table*/,&jj/*index in fullTreePtr[] of last entry in the 'sub tree'*/);

                    /* jj is last index that is in the sub tree ...
                      need to add 1 so it become 'first index out the tree' */
                    jj+=1;
                }
                else
                {
                    /* found the sub tree to remove */
                    /* state that all leaf under this sub tree to be 'ignored'*/
                    /* as it is NOT valid for A0 devices */
                    rc = skipSubTreeOfValidSubTree(fullTreePtr , numElementsInFullTree/* max elements */ ,
                        jj/* current index in the table*/ , &jj/*index in fullTreePtr[] of last entry in the 'sub tree'*/);
                    if(rc != GT_OK)
                    {
                        /* error */
                        return rc;
                    }

                    /* update that one less 'sub tree' in this register */
                    if(fullTreePtr[orig_jj].subIntrListLen)
                    {
                        fullTreePtr[orig_jj].subIntrListLen--;
                    }
                    else
                    {
                        /* can not decrement from 0 ! */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                    }

                    /* we are done removing this sub tree that relate to the specified event */
                    break;
                }
            }

            /* we are done removing this sub tree that relate to the specified event */
            break;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvPpIntDefDxChBobcat2Init
*
* DESCRIPTION:
*       Interrupts initialization for the Bobcat2 devices.
*
* INPUTS:
*       devNum - the device number
*       ppRevision - the revision of the device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_OUT_OF_CPU_MEM - fail to allocate cpu memory (osMalloc)
*       GT_BAD_PARAM - the scan tree information has error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntDefDxChBobcat2Init
(
    IN GT_U8   devNum ,
    IN GT_U32   ppRevision
)
{
    GT_STATUS   rc;         /* return code */
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC * devInterruptInfoPtr;
                            /* pointer to device interrupt info */
    GT_U32 isBc2A0;/* indication that this is 'A0' or other */
    GT_U32  ii,jj,orig_jj;/* index iterator */
    static GT_U32   initDone = 0;
    GT_U32  orig_maskRcvIntrEn;

    /*set info needed by chIntCauseToUniEvConvertDedicatedTables(...) */
    setBobcat2DedicatedEventsConvertInfo(devNum);

    if(initDone == 0)
    {
        initDone  = 1;
        /* NOTE:
           1. currently the 'tree' hold info of A0 , with place holders that may
            be filled by B0
           2. we not support mix of A0 and B0 under one CPS management (one CPSS)
        */

        if(prvCpssDrvPpConfig[devNum]->devSubFamily == CPSS_PP_SUB_FAMILY_NONE_E)
        {
            isBc2A0 = (ppRevision == 0) ? 1 : 0;
        }
        else
        {
            isBc2A0 = 0;
        }

        if(0 == isBc2A0)
        {
            /* remove indications of NON_A0_SUB_TREE_CNS */
            for(jj = 0 ; jj < BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS ; jj++)
            {
                orig_jj = jj;

                if((PRV_CPSS_DXCH_UNIT_ENT)bobcat2IntrScanArr[jj].gppId ==
                                                    PRV_CPSS_DXCH_UNIT_RXDMA_E)
                {
                    /* the tree hold addresses of A0 need to add to all 0x500 */
                    bobcat2IntrScanArr[jj].causeRegAddr += BC2_B0_RXDMA_OFFSET_FROM_A0_CNS;
                    bobcat2IntrScanArr[jj].maskRegAddr  += BC2_B0_RXDMA_OFFSET_FROM_A0_CNS;
                }
                else
                if((bobcat2IntrScanArr[jj].gppId & EXTRA_INFO_SUB_TREE_MASK_CNS) ==
                        A0_TXQ_PFC_TO_REPLACE_WITH_B0_SUB_TREE_CNS)
                {
                    /* we need to replace the sub tree of
                        txqPfc_SUB_TREE_MAC with
                        bc2_B0_txqPfc_SUB_TREE_MAC */
                    rc = replaceSubTree(bobcat2IntrScanArr, BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS/* max elements */ ,
                        /* old info */
                        txqPfc_SUB_TREE_numEntries,
                        /* new info */
                        bc2_B0_txqPfc_SUB_TREE_arr , bc2_B0_txqPfc_SUB_TREE_numEntries,
                        jj/* current index in the table*/ );
                    if(rc != GT_OK)
                    {
                        /* error */
                        return rc;
                    }

                    /* last index that is in the sub tree ...
                      with the '++' of the loop it will become 'first index out the tree' */
                    jj += (txqPfc_SUB_TREE_numEntries/*old size*/ - 1);
                }

                /* remove indications from the gppId field */
                bobcat2IntrScanArr[orig_jj].gppId &= ~EXTRA_INFO_SUB_TREE_MASK_CNS;
            }

            /* override the DB of A0 with the info of B0 */
            for(ii = 0 ; bc2B0OverrideA0Arr[ii].oldIntCause != LAST_ENTRY_INT_CAUSE_CNS ; ii++)
            {
                /* look for the entry to be replaced in bobcat2IntrScanArr[] */
                for(jj = 0 ; jj < BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS ; jj++)
                {
                    if(bc2B0OverrideA0Arr[ii].oldIntCause == bobcat2IntrScanArr[jj].startIdx)
                    {
                        /* match */
                        break;
                    }
                }

                if(jj >= BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS)
                {
                    /* the entry was not found in the DB ?! DB error ?! */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                /* override the old entry with the new entry */
                bobcat2IntrScanArr[jj] = bc2B0OverrideA0Arr[ii].newInfo;
            }
        }
        else /* A0 */
        {
            /* we need to 'skip' ALL 'sub trees' that stated 'NON_A0_SUB_TREE_CNS' */
            /* as those are 'not A0' sub trees */

            for(jj = 0 ; jj < BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS ; jj++)
            {
                orig_jj = jj;

                if((bobcat2IntrScanArr[jj].gppId & EXTRA_INFO_SUB_TREE_MASK_CNS) ==
                        NON_A0_SUB_TREE_CNS)
                {
                    orig_maskRcvIntrEn = bobcat2IntrScanArr[jj].maskRcvIntrEn;
                    /* state that all leaf under this sub tree to be 'ignored'*/
                    /* as it is NOT valid for A0 devices */
                    rc = skipSubTreeOfValidSubTree(bobcat2IntrScanArr , BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS/* max elements */ ,
                        jj/* current index in the table*/ , &jj/*index in bobcat2IntrScanArr[] of last entry in the 'sub tree'*/);
                    if(rc != GT_OK)
                    {
                        /* error */
                        return rc;
                    }
                    /* last index that is in the sub tree ...
                      with the '++' of the loop it will become 'first index out the tree' */

                    /* we need to update the number of subIntrListLen at the 'father' */
                    for(ii = 0; ii < orig_jj ; ii++)
                    {
                        /* look for the ID that unique the register that my father uses */
                        if((orig_maskRcvIntrEn >> 5) ==
                            (bobcat2IntrScanArr[ii].startIdx >> 5))
                        {
                            /* match found */
                            break;
                        }
                    }

                    if(ii == orig_jj)
                    {
                        /* DB was not set properly */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                    }

                    /* update the father about the number of children in subIntrListLen ! */
                    bobcat2IntrScanArr[ii].subIntrListLen --;
                }
                else
                if((bobcat2IntrScanArr[jj].gppId & EXTRA_INFO_SUB_TREE_MASK_CNS) ==
                        B0_TXQ_LL_TO_REPLACE_WITH_A0_SUB_TREE_CNS)
                {

                    /* we need to replace the sub tree of
                        bc2_B0_txqLl_SUB_TREE_MAC
                        with txqLl_NODE_MAC */
                    rc = replaceSubTree(bobcat2IntrScanArr, BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS/* max elements */ ,
                        /* old info */
                        bc2_B0_txqLl_SUB_TREE_numEntries,
                        /* new info */
                        txqLl_NODE_arr , txqLl_NODE_numEntries,
                        jj/* current index in the table*/ );
                    if(rc != GT_OK)
                    {
                        /* error */
                        return rc;
                    }

                    /* last index that is in the sub tree ...
                      with the '++' of the loop it will become 'first index out the tree' */
                    jj += (bc2_B0_txqLl_SUB_TREE_numEntries/*old size*/ - 1);
                }

                /* remove indications from the gppId field */
                bobcat2IntrScanArr[orig_jj].gppId &= ~EXTRA_INFO_SUB_TREE_MASK_CNS;
            }
        }
    }

    /*
       1. fill the array of mask registers addresses
       2. fill the array of default values for the mask registers
       3. update the bits of nonSumBitMask in the scan tree
    */
    rc = prvCpssDrvExMxDxHwPpMaskRegInfoGet(
            CPSS_PP_FAMILY_DXCH_BOBCAT2_E,
            BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS,
            bobcat2IntrScanArr,
            BOBCAT2_NUM_MASK_REGISTERS_CNS,
            bobcat2MaskRegDefaultSummaryArr,
            bobcat2MaskRegMapArr);
    if(rc != GT_OK)
    {
        /* error */
        return rc;
    }

    /* bobcat2 info */
    devInterruptInfoPtr =
        &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_BOBCAT2_E];

    devInterruptInfoPtr->numOfInterrupts = PRV_CPSS_BOBCAT2_LAST_INT_E;
    devInterruptInfoPtr->maskRegistersAddressesArray = bobcat2MaskRegMapArr;
    devInterruptInfoPtr->maskRegistersDefaultValuesArray = bobcat2MaskRegDefaultSummaryArr;
    devInterruptInfoPtr->interruptsScanArray = bobcat2IntrScanArr;
    devInterruptInfoPtr->numOfScanElements = BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS;
    devInterruptInfoPtr->fdbTrigEndedId = 0;/* don't care */
    devInterruptInfoPtr->fdbTrigEndedCbPtr = NULL;
    devInterruptInfoPtr->hasFakeInterrupts = GT_FALSE;
    devInterruptInfoPtr->firstFakeInterruptId = 0;/* don't care */
    devInterruptInfoPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
    devInterruptInfoPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
    devInterruptInfoPtr->notAccessibleBeforeStartInitPtr = NULL;

    return GT_OK;
}

#ifdef DUMP_DEFAULT_INFO

/*******************************************************************************
* prvCpssDrvPpIntDefBobcat2Print
*
* DESCRIPTION:
*      print the interrupts arrays info, of Bobcat2 devices
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
void  prvCpssDrvPpIntDefBobcat2Print(
    void
)
{
    cpssOsPrintf("Bobcat2 - start : \n");
    /* port group 0 */
    prvCpssDrvPpIntDefPrint(BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS,bobcat2IntrScanArr,
                            BOBCAT2_NUM_MASK_REGISTERS_CNS,bobcat2MaskRegDefaultSummaryArr,bobcat2MaskRegMapArr);
    cpssOsPrintf("Bobcat2 - End : \n");

}
#endif/*DUMP_DEFAULT_INFO*/


/*******************************************************************************
* prvCpssDrvPpIntDefBobcat2Print_regInfoByInterruptIndex
*
* DESCRIPTION:
*      print for Bobcat2 devices the register info according to value in
*       PRV_CPSS_BOBCAT2_INT_CAUSE_ENT (assuming that this register is part of
*       the interrupts tree)
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
void  prvCpssDrvPpIntDefBobcat2Print_regInfoByInterruptIndex(
    IN PRV_CPSS_BOBCAT2_INT_CAUSE_ENT   interruptId
)
{
    cpssOsPrintf("Bobcat2 - start regInfoByInterruptIndex : \n");

    if(interruptId >= PRV_CPSS_BOBCAT2_LAST_INT_E)
    {
        cpssOsPrintf("interruptId[%d] >= maximum(%d) \n" ,
            interruptId , PRV_CPSS_BOBCAT2_LAST_INT_E);
    }
    else
    {
        prvCpssDrvPpIntDefPrint_regInfoByInterruptIndex(BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS,bobcat2IntrScanArr,interruptId);
    }
    cpssOsPrintf("Bobcat2 - End regInfoByInterruptIndex : \n");

}


