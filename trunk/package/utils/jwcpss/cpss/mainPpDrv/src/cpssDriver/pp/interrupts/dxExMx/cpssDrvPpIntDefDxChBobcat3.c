/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefDxChBobcat3.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- DxCh Bobcat3 devices
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
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

/* convert bit in the enum of the huge interrupts of the device into BIT_0..BIT_31 in a register */
#define DEVICE_INT_TO_BIT_MAC(deviceInterruptId)    \
            (1 << ((deviceInterruptId) & 0x1f))

#define FDB_REG_MASK_CNS        \
    (0xFFFFFFFF & ~                                                             \
        (DEVICE_INT_TO_BIT_MAC(PRV_CPSS_BOBCAT2_FDB_AGE_VIA_TRIGGER_ENDED_E) |     \
         DEVICE_INT_TO_BIT_MAC(PRV_CPSS_BOBCAT2_FDB_AU_PROC_COMPLETED_E)))

#define   fdb_SUB_TREE_MAC(bit)                                                 \
    /* FDBIntSum - FDB Interrupt Cause,                                      */ \
    /*   maskRcvIntrEn - both AUProcCompletedInt(bit of PRV_CPSS_BOBK_FDB_AU_PROC_COMPLETED_E) and AgeViaTriggerEnded(bit of PRV_CPSS_BOBK_FDB_AGE_VIA_TRIGGER_ENDED_E)*/\
    /*   should never be masked to avoid missed events situation.  */           \
    {bit, GT_FALSE,PRV_CPSS_DXCH_UNIT_FDB_E, NULL, 0x430001b0, 0x430001b4,      \
        prvCpssDrvHwPpPortGroupIsrRead,                                         \
        prvCpssDrvHwPpPortGroupIsrWrite,                                        \
        PRV_CPSS_BOBCAT2_FDB_NUM_OF_HOP_EX_P_E,                                 \
        PRV_CPSS_BOBCAT2_FDB_ADDRESS_OUT_OF_RANGE_E,                            \
        FILLED_IN_RUNTIME_CNS, 0x0, FDB_REG_MASK_CNS,                           \
             0,NULL, NULL}

#define   FuncUnitsIntsSum_SUB_TREE_MAC(bit)                                   \
/* FuncUnitsIntsSum Interrupt Cause */                                         \
{bit, GT_FALSE, PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x000003f8, 0x000003FC,         \
    prvCpssDrvHwPpPortGroupIsrRead,                                            \
    prvCpssDrvHwPpPortGroupIsrWrite,                                           \
    PRV_CPSS_BOBCAT2_FUNCTIONAL_S_SUMMARY_FUNCTIONAL_INTERRUPT_SUMMARY_E,      \
    PRV_CPSS_BOBCAT2_FUNCTIONAL_S_SUMMARY_FUNC_UNITS_SUM_29_E,                 \
    FILLED_IN_RUNTIME_CNS, 0, 0x0, 1 , NULL, NULL},                            \
                                                                               \
    fdb_SUB_TREE_MAC(4)

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

static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC bobcat3IntrScanArr[] =
{
    /* Global Interrupt Cause */
    {0, GT_FALSE, PRV_CPSS_DXCH_UNIT_MG_E, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_BOBCAT2_GLOBAL_S_SUMMARY_FUNC_UNITSS_SUM_E,
        PRV_CPSS_BOBCAT2_GLOBAL_S_SUMMARY_PORTS_2_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 5 , NULL, NULL},

        FuncUnitsIntsSum_SUB_TREE_MAC(3),

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


};

/* number of elements in the array of bobcat3IntrScanArr[] */
#define BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS \
    (sizeof(bobcat3IntrScanArr)/sizeof(bobcat3IntrScanArr[0]))

#define BOBCAT2_NUM_MASK_REGISTERS_CNS  (PRV_CPSS_BOBCAT2_LAST_INT_E / 32)

/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 bobcat3MaskRegDefaultSummaryArr[BOBCAT2_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 bobcat3MaskRegMapArr[BOBCAT2_NUM_MASK_REGISTERS_CNS] =
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
* setBobcat3DedicatedEventsConvertInfo
*
* DESCRIPTION:
*      set info needed by chIntCauseToUniEvConvertDedicatedTables(...) for the
*      Bobcat3 devices.
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
static void setBobcat3DedicatedEventsConvertInfo(IN GT_U8   devNum)
{
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataArr   = bobcat2UniEvMapTableWithExtData;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataSize  = bobcat2UniEvMapTableWithExtDataSize;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableArr              = bobcat2UniEvMapTable;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableSize             = bobcat2UniEvMapTableSize;
    prvCpssDrvPpConfig[devNum]->intCtrl.numReservedPorts              = 72;

    return;
}



/*******************************************************************************
* prvCpssDrvPpIntDefDxChBobcat3Init
*
* DESCRIPTION:
*       Interrupts initialization for the Bobcat3 devices.
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
GT_STATUS prvCpssDrvPpIntDefDxChBobcat3Init
(
    IN GT_U8   devNum ,
    IN GT_U32   ppRevision
)
{
    GT_STATUS   rc;         /* return code */
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC * devInterruptInfoPtr;
                            /* pointer to device interrupt info */

    ppRevision = ppRevision;

    /*set info needed by chIntCauseToUniEvConvertDedicatedTables(...) */
    setBobcat3DedicatedEventsConvertInfo(devNum);


    /*
       1. fill the array of mask registers addresses
       2. fill the array of default values for the mask registers
       3. update the bits of nonSumBitMask in the scan tree
    */
    rc = prvCpssDrvExMxDxHwPpMaskRegInfoGet(
            CPSS_PP_FAMILY_DXCH_BOBCAT2_E,
            BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS,
            bobcat3IntrScanArr,
            BOBCAT2_NUM_MASK_REGISTERS_CNS,
            bobcat3MaskRegDefaultSummaryArr,
            bobcat3MaskRegMapArr);
    if(rc != GT_OK)
    {
        /* error */
        return rc;
    }

    /* bobcat3 info */
    devInterruptInfoPtr =
        &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_BOBCAT3_E];

    devInterruptInfoPtr->numOfInterrupts = PRV_CPSS_BOBCAT2_LAST_INT_E;
    devInterruptInfoPtr->maskRegistersAddressesArray = bobcat3MaskRegMapArr;
    devInterruptInfoPtr->maskRegistersDefaultValuesArray = bobcat3MaskRegDefaultSummaryArr;
    devInterruptInfoPtr->interruptsScanArray = bobcat3IntrScanArr;
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


