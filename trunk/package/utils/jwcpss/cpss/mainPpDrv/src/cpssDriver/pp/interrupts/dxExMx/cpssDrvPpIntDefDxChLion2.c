/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefDxChLion2.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- DxCh Lion2 devices
*
* FILE REVISION NUMBER:
*       $Revision: 37 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLion2.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxUtilLion.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwRegAddr.h>



#define LION2_NUM_PORT_GROUPS_CNS  8

#define LION2_TXQ_QUEUE_ADDR(offset) ((offset) + PRV_CPSS_DXCH_LION2_TXQ_QUEUE_BASE_ADDRESS_CNS)
#define LION2_TXQ_EGR0_ADDR(offset) ((offset) + PRV_CPSS_DXCH_LION2_TXQ_EGR0_BASE_ADDRESS_CNS)
#define LION2_TXQ_EGR1_ADDR(offset) ((offset) + PRV_CPSS_DXCH_LION2_TXQ_EGR1_BASE_ADDRESS_CNS)
#define LION2_TXQ_DIST_ADDR(offset) ((offset) + PRV_CPSS_DXCH_LION2_TXQ_DIST_BASE_ADDRESS_CNS)

/* Don't use "const" because we Fill some values at initialization stage */
/* a scan tree of single port group */
static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC lion2IntrScanArrSinglePortGroup[] =
{
    /* Global Interrupt Cause */
    {0, GT_FALSE, 0, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_LION2_GLOBAL_PEX_SUM_E,
        PRV_CPSS_LION2_GLOBAL_DFX_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0, 0x80000000, 29 , NULL, NULL},


        /* PEX */
        {1,GT_FALSE,0, NULL, PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS, PRV_CPSS_DRV_SCAN_PEX_MASK_REG_PLACE_HOLDER_CNS,
             prvCpssDrvHwPpPortGroupReadInternalPciReg,
             prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            PRV_CPSS_LION2_PEX_RCV_A_E,
            PRV_CPSS_LION2_PEX_RCV_D_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


        /* PEX ERROR */
        {2,GT_FALSE,0, NULL, PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS, PRV_CPSS_DRV_SCAN_PEX_MASK_REG_PLACE_HOLDER_CNS,
             prvCpssDrvHwPpPortGroupReadInternalPciReg,
             prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            PRV_CPSS_LION2_PEX_DL_DWN_TX_ACC_ERR_E,
            PRV_CPSS_LION2_PEX_PEX_LINK_FAIL_E,
            FILLED_IN_RUNTIME_CNS, 0xFF1F1B, 0xFFFFFFFF, 0,NULL, NULL},


        /* CNC */
        {3,GT_FALSE,0, NULL, 0x08000108, 0x0800010c,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_CNC_BLOCK0_WRAPAROUND_E,
            PRV_CPSS_LION2_CNC_CNC_UPDATE_LOST_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* IP */
        {4,GT_FALSE,0, NULL, 0x02800970, 0x02800974,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_IP_ROUTER_NHE_DIP_DATA_ERROR_E,
            PRV_CPSS_LION2_IP_ROUTER_URPF_VLAN_DATA_ERROR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

         /* TXQ_SHT_GEN */
        {5,GT_FALSE,0, NULL, 0x118F0120, 0x118F0130,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TXQ_SHT_GEN_EGR_VLAN_TBL_PARITY_ERROR_E,
            PRV_CPSS_LION2_TXQ_SHT_GEN_WRONG_ADDR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* txq_dq_int_sum */
        {6,GT_FALSE,0, NULL, 0x11000100, 0x11000104,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_SUM_E,
            PRV_CPSS_LION2_TXQ_DQ_GEN_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 4,NULL, NULL},

            /* TXQ_DQ Flush Done */
            {1,GT_FALSE,0, NULL, 0x11000108, 0x1100010c,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT0_E,
                PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT15_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TXQ_DQ Memory Error */
            {2,GT_FALSE,0, NULL, 0x11000110, 0x11000114,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_TOKEN_BUCKET_PRIO_PARITY_ERROR_E,
                PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_STATE_VARIABLE_PARITY_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TXQ_DQ Egress STC */
            {3,GT_FALSE,0, NULL, 0x11000118, 0x1100011c,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT0_E,
                PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT15_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TXQ_DQ General Interrupt */
            {4,GT_FALSE,0, NULL, 0x11000120, 0x11000124,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_TXQ_DQ_GEN_EGRESS_MIRROR_DESC_DROP_E,
                PRV_CPSS_LION2_TXQ_DQ_GEN_WRONG_ADDR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* tti_int_sum */
        {7,GT_FALSE,0, NULL, 0x010001C0, 0x010001C4,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TTI_ENG_SUM_E,
            PRV_CPSS_LION2_TTI_ITS_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

            /* TTI Engine Interrupt */
            {1,GT_FALSE,0, NULL, 0x01000004, 0x01000008,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_TTI_ENG_WRONG_ADDR_E,
                PRV_CPSS_LION2_TTI_ENG_ACCESS_DATA_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TTI GTS Interrupt */
            {3,GT_FALSE,0, NULL, 0x01002028, 0x0100202C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_TTI_GTS_WRONG_ADDR_E,
                PRV_CPSS_LION2_TTI_GTS_GLOBAL_FIFO_FULL_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* tcc_lower_int_sum (PCL) */
        {8,GT_FALSE,0, NULL, 0x0D0001BC, 0x0D0001C0,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TCC_LOWER_GEN_SUM_E,
            PRV_CPSS_LION2_TCC_LOWER_DFX_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1,NULL, NULL},

            /* tcc_lower gen (PCL) */
            {1,GT_FALSE,0, NULL, 0x0D0001A4, 0x0D0001A8,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E,
                PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_BIST_FAILED_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* tcc_upper_int_sum (IPvX) */
        {9,GT_FALSE,0, NULL, 0x0D800078, 0x0D80007C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TCC_UPPER_GEN_SUM_E,
            PRV_CPSS_LION2_TCC_UPPER_DFX_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1,NULL, NULL},

            /* tcc_upper gen (IPvX) */
            {1,GT_FALSE,0, NULL, 0x0D800060, 0x0D800064,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E,
                PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_BIST_FAILED_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* ha_int_sum  (header alteration) */
        {10,GT_FALSE,0, NULL, 0x0E8003A8, 0x0E8003AC,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_HA_GEN_SUM_E,
            PRV_CPSS_LION2_HA_ETS_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

            /* HA Summary (header alteration) */
            {1,GT_FALSE,0, NULL, 0x0E8003A0, 0x0E8003A4,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_HA_GEN_TS_ARP_TABLE_PARITY_ERROR_E,
                PRV_CPSS_LION2_HA_GEN_TS_ARP_TABLE_PARITY_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GTS Summary (header alteration) */
            {3,GT_FALSE,0, NULL, 0x0E800828, 0x0E80082C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_HA_GTS_WRONG_ADDR_E,
                PRV_CPSS_LION2_HA_GTS_GLOBAL_FIFO_OVERRUN_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Miscellaneous */
        {11,GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_MISC_TWSI_TIME_OUT_E,
            PRV_CPSS_LION2_MISC_Z80_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Txq LL Interrupt Cause */
        {12,GT_FALSE,0, NULL, 0x108A0008, 0x108A000C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TXQ_LL_GEN_ID_FIFO_OVERRUN_E,
            PRV_CPSS_LION2_TXQ_LL_GEN_LL_WRONG_ADDR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* TxqQueueIntSum   */
        {13, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90000), LION2_TXQ_QUEUE_ADDR(0x90004),
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TXQ_QUEUE_GEN_SUM_E,
            PRV_CPSS_LION2_TXQ_QUEUE_QCN_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 9, NULL, NULL},

             /* TxqQueueIntSum  - gen */
             {1, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90008), LION2_TXQ_QUEUE_ADDR(0x9000C),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_GEN_GLOBAL_DESC_FULL_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_3_OVERRUN_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port Descriptor reg 0 */
             {2, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90040), LION2_TXQ_QUEUE_ADDR(0x90044),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT0_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT30_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port Descriptor reg 1 */
             {3, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90048), LION2_TXQ_QUEUE_ADDR(0x9004c),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT31_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT61_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port Descriptor reg 2 */
             {4, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90050), LION2_TXQ_QUEUE_ADDR(0x90054),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT62_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT63_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


             /* TxqQueueIntSum  - Port buff full reg 0 */
             {5, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90060), LION2_TXQ_QUEUE_ADDR(0x90064),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_BUFF_FULL_PORT0_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_BUFF_FULL_PORT30_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port buff full reg 1 */
             {6, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90068), LION2_TXQ_QUEUE_ADDR(0x9006c),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_BUFF_FULL_PORT31_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_BUFF_FULL_PORT61_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port buff full reg 2 */
             {7, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90070), LION2_TXQ_QUEUE_ADDR(0x90074),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_BUFF_FULL_PORT62_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_PORT_BUFF_FULL_PORT63_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - PFC Summary */
             {8, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90080), LION2_TXQ_QUEUE_ADDR(0x90084),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_PFC_COUNTERS_PARITY_SUM_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_PFC_MSGS_SETS_OVERRUNS_EGR7_4CAUSE1_SUM_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1, NULL, NULL},

                 {1, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90090), LION2_TXQ_QUEUE_ADDR(0x90094),
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP0_COUNTERS_ERR_E,
                     PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP7_COUNTERS_ERR_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


             /* TxqQueueIntSum  - Congestion Notification (QCN) */
             {9, GT_FALSE,0, NULL, LION2_TXQ_QUEUE_ADDR(0x90180), LION2_TXQ_QUEUE_ADDR(0x90184),
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXQ_QUEUE_QCN_CN_BUFFER_FIFO_OVERRUN_E,
                 PRV_CPSS_LION2_TXQ_QUEUE_QCN_CN_DROP_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* L2 Bridge  */
        {14, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_BRIDGE_UPDATE_FIFO_FULL_E,
            PRV_CPSS_LION2_BRIDGE_UPDATE_SECURITY_BREACH_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* EgressIntCauseSum  */ /*egr0*/
        {15, GT_FALSE,0, NULL, LION2_TXQ_EGR0_ADDR(0xA0), LION2_TXQ_EGR0_ADDR(0xB0),
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_0_PARITY_ERROR_E,
            PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_DESC_RECONST_FIFO_OVERRUN_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /*txq_dist_int_sum */
        {16, GT_FALSE,0, NULL,LION2_TXQ_DIST_ADDR(0x000008),LION2_TXQ_DIST_ADDR(0x00000C),
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TXQ_DIST_BURST_FIFO_0_FULL_E,
            PRV_CPSS_LION2_TXQ_DIST_BURST_FIFO_EXCEED_PFC_XOFF_THRESHOLD_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* FDBIntSum - FDB Interrupt Cause,
           maskRcvIntrEn - both AUProcCompletedInt(bit 9) and AgeViaTriggerEnded(bit 4)
           should never be masked to avoid missed events situation.  */
        {17, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_FDB_NUM_OF_HOP_EX_P_E,
            PRV_CPSS_LION2_FDB_AU_FIFO_TO_CPU_IS_FULL_E,
            FILLED_IN_RUNTIME_CNS, 0x0, (0xFFFFFFFF & ~ (BIT_4 | BIT_9)), 0,NULL, NULL},

        /* CPU Port SUM  */         /*0x0883F020*/                      /*0x0883F024*/
        {19, GT_FALSE,0, NULL, LION2_GE_MAC_PORT_INTERRUPT_CAUSE_MAC(63), LION2_GE_MAC_PORT_INTERRUPT_CAUSE_MAC(63) + 4,
              prvCpssDrvHwPpPortGroupIsrRead,
              prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_LION2_CPU_PORT_SUM_LINK_STATUS_CHANGED_E,
             PRV_CPSS_LION2_CPU_PORT_SUM_TX_FIFO_UNDERRUN_E,
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Pre-Egress */
        {20, GT_FALSE,0, NULL, 0x0B000058, 0x0B00005C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_EQ_INGRESS_STC_SUM_E,
            PRV_CPSS_LION2_EQ_STC_RATE_LIMITERS_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

            /* Ingress STC  */
            {1, GT_FALSE,0, NULL, 0x0B000020, 0x0B000024,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_INGRESS_STC_PORT0_INGRESS_SAMPLE_LOADED_E,
                PRV_CPSS_LION2_INGRESS_STC_PORT11_INGRESS_SAMPLE_LOADED_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* STC Rate Limiters */
            {2, GT_FALSE,0, NULL, 0x0B000060, 0x0B000064,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_1_PKT_DROPPED_E,
                PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_31_PKT_DROPPED_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Tx SDMA  */
        {21, GT_FALSE,0, NULL, 0x00002810, 0x00002818,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE0_E,
            PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE7_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Rx SDMA  */
        {22, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE0_E,
            PRV_CPSS_LION2_RECEIVE_SDMA_PACKET_CNT_OF_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* PCL  */
        {23, GT_FALSE,0, NULL, 0x0B800004, 0x0B800008,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_POLICY_ENGINE_WRONG_ADDR_E,
            PRV_CPSS_LION2_POLICY_ENGINE_INLIF_TABLE_ECC_ERROR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* IPLR0IntSum  */
        {24, GT_FALSE,0, NULL, 0x0C000100, 0x0C000104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_IPLR0_DATA_ERROR_E,
            PRV_CPSS_LION2_IPLR0_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* IPLR1IntSum  */
        {25, GT_FALSE,0, NULL, 0x00800100, 0x00800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_IPLR1_DATA_ERROR_E,
            PRV_CPSS_LION2_IPLR1_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* EgressIntCauseSum  */ /*egr1*/
        {26, GT_FALSE,0, NULL, LION2_TXQ_EGR1_ADDR(0xA0), LION2_TXQ_EGR1_ADDR(0xB0),
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_0_PARITY_ERROR_E,
            PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_DESC_RECONST_FIFO_OVERRUN_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

         /* eplr_int_sum  */
        {27, GT_FALSE,0, NULL, 0x07800100, 0x07800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_EPLR_DATA_ERROR_E,
            PRV_CPSS_LION2_EPLR_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* XSMI  */
        {28, GT_FALSE,0, NULL, 0x00040010, 0x00040014,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_XSMI_WRITE_E,
            PRV_CPSS_LION2_XSMI_WRITE_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* ports_int_sum */
        {29, GT_FALSE,0, NULL, 0x00000080, 0x00000084,
              prvCpssDrvHwPpPortGroupIsrRead,
              prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_LION2_GLOBAL_1_XG_PORT1_SUM_E,
             PRV_CPSS_LION2_GLOBAL_1_XG_PORT0_SUM_E,
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF,
            14 ,NULL, NULL},

            /* --> bit 31 */          LION2_PORT_0_SUB_TREE_MAC,
            /* --> bit 1 */           LION2_PORT_1_11_ODD_TREE_MAC(1),
            /* --> bit 2 */           LION2_PORT_2_14_EVEN_TREE_MAC(2),
            /* --> bit 3 */           LION2_PORT_1_11_ODD_TREE_MAC(3),
            /* --> bit 4 */           LION2_PORT_2_14_EVEN_TREE_MAC(4),
            /* --> bit 5 */           LION2_PORT_1_11_ODD_TREE_MAC(5),
            /* --> bit 6 */           LION2_PORT_2_14_EVEN_TREE_MAC(6),
            /* --> bit 7 */           LION2_PORT_1_11_ODD_TREE_MAC(7),
            /* --> bit 8 */           LION2_PORT_2_14_EVEN_TREE_MAC(8),
            /* --> bit 9 */           LION2_PORT_1_11_ODD_TREE_MAC(9),
            /* --> bit 10 */          LION2_PORT_2_14_EVEN_TREE_MAC(10),
            /* --> bit 11 */          LION2_PORT_1_11_ODD_TREE_MAC(11),
            /* --> bit 12 */          LION2_PORT_2_14_EVEN_TREE_MAC(12),
            /* --> bit 14 */          LION2_PORT_2_14_EVEN_TREE_MAC(14),
        /* Data Path */
        {30, GT_FALSE,0, NULL, 0x000000A4, 0x000000A8,
              prvCpssDrvHwPpPortGroupIsrRead,
              prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_LION2_DATA_PATH_CTU0_SUM_E,
             PRV_CPSS_LION2_DATA_PATH_CPFC_SUM_E,
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 10 ,NULL, NULL},

            /* CTU0 */
            {1, GT_FALSE,0, NULL, 0x07003000, 0x07003004,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_CTU0_FCP0_PORTA_ECC_ERR_E,
                 PRV_CPSS_LION2_CTU0_DESC3_UNUSED_FIFO_ECC_ERR_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

            /* Buffer Management Interrupt Cause0 */
            {2, GT_FALSE,0, NULL, 0x03000040, 0x03000044,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_BM_0_EMPTY_CLEAR_E,
                 PRV_CPSS_LION2_BM_0_LL_P2_ACCESS_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

            /* Buffer Management Interrupt Cause1 */
            {3, GT_FALSE,0, NULL, 0x03000048, 0x0300004C,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT0_E,
                 PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT_CPU_63_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

            /* MPPM 0  */
            {4, GT_FALSE,0, NULL, 0x06800004, 0x06800008,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_MPPM_0_BK0_ECC_1_ERROR_E,
                 PRV_CPSS_LION2_MPPM_0_BK1_ECC_2_OR_MORE_ERROR_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

            /* CTU1  */
            {5, GT_FALSE,0, NULL, 0x0A803000, 0x0A803004,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_CTU1_FCP0_PORTA_ECC_ERR_E,
                 PRV_CPSS_LION2_CTU1_DESC3_UNUSED_FIFO_ECC_ERR_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

            /* MPPM 1  */
            {6, GT_FALSE,0, NULL, 0x0A000004, 0x0A000008,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_MPPM_1_BK0_ECC_1_ERROR_E,
                 PRV_CPSS_LION2_MPPM_1_BK1_ECC_2_OR_MORE_ERROR_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

            /* RX DMA SUMMARY */
            {7, GT_FALSE,0, NULL, 0x0F000070, 0x0F000074,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_RXDMA_SUM_FUNC_SUM_E,
                 PRV_CPSS_LION2_RXDMA_SUM_FUNC_SUM_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1 ,NULL, NULL},

                /* RX DMA Functional */
                {1, GT_FALSE,0, NULL, 0x0F000078, 0x0F00007C,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_RXDMA_FUNC_IBUF_SYNC_FIFO_0_FULL_E,
                     PRV_CPSS_LION2_RXDMA_FUNC_WRDMA15_CT_PKT_FIFO_OVERRUN_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

            /* TxDMA Interrupt Summary */
            {8, GT_FALSE,0, NULL, 0x0F800130, 0x000000F4, /* There is no mask register for TxDMA Interrupt Summary*/
                  prvCpssDrvHwPpPortGroupIsrRead,         /* We use "user defined register" as "Dummy mask" */
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_TXDMA_SUM_CAUSE_SUM_E,
                 PRV_CPSS_LION2_TXDMA_SUM_ERROR_CAUSE_SUM_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2 ,NULL, NULL},

                /* TxDMA Interrupt Cause */
                {1, GT_FALSE,0, NULL, 0x0F800134, 0x0F800138,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_TXDMA_GEN_WRONG_ADDR_E,
                     PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_12_OVERRUN_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* TxDMA Error Interrupt Cause */
                {2, GT_FALSE,0, NULL, 0x0F80013C, 0x0F800140,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_TXDMA_ERROR_CLEAR_FIFO_FULL_E,
                     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO7_BK1_OVERRUN_INT_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},


            /* BMA Engine Interrupt summary */
            {9, GT_FALSE,0, NULL, 0x03880000, 0x03880004,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_BMA_UNIT_0_SUM_E,
                 PRV_CPSS_LION2_BMA_CLASTER_SUM_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 4 ,NULL, NULL},

                    /* BMA Core0 Interrupt summary */
                    {1, GT_FALSE,0, NULL, 0x03810000, 0x03810004,
                          prvCpssDrvHwPpPortGroupIsrRead,
                          prvCpssDrvHwPpPortGroupIsrWrite,
                         PRV_CPSS_LION2_BMA_CORE0_MCCNT_RMW_CLEAR_FIFO_FULL_E,
                         PRV_CPSS_LION2_BMA_CORE0_FINAL_CLEAR_UDB_FIFO3_FULL_E,
                         FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                    /* BMA Core1 Interrupt summary */
                    {2, GT_FALSE,0, NULL, 0x03830000, 0x03830004,
                          prvCpssDrvHwPpPortGroupIsrRead,
                          prvCpssDrvHwPpPortGroupIsrWrite,
                         PRV_CPSS_LION2_BMA_CORE1_MCCNT_RMW_CLEAR_FIFO_FULL_E,
                         PRV_CPSS_LION2_BMA_CORE1_FINAL_CLEAR_UDB_FIFO3_FULL_E,
                         FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                    /* BMA Core2 Interrupt summary */
                    {3, GT_FALSE,0, NULL, 0x03850000, 0x03850004,
                          prvCpssDrvHwPpPortGroupIsrRead,
                          prvCpssDrvHwPpPortGroupIsrWrite,
                         PRV_CPSS_LION2_BMA_CORE2_MCCNT_RMW_CLEAR_FIFO_FULL_E,
                         PRV_CPSS_LION2_BMA_CORE2_FINAL_CLEAR_UDB_FIFO3_FULL_E,
                         FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                    /* BMA Core3 Interrupt summary */
                    {4, GT_FALSE,0, NULL, 0x03870000, 0x03870004,
                          prvCpssDrvHwPpPortGroupIsrRead,
                          prvCpssDrvHwPpPortGroupIsrWrite,
                         PRV_CPSS_LION2_BMA_CORE3_MCCNT_RMW_CLEAR_FIFO_FULL_E,
                         PRV_CPSS_LION2_BMA_CORE3_FINAL_CLEAR_UDB_FIFO3_FULL_E,
                         FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},


            /* CPFC Interrupt Summary */
            {10, GT_FALSE,0, NULL, 0x13800300, 0x13800304,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION2_CPFC_CC_OVER_UNDER_FLOW_SUM_E,
                 PRV_CPSS_LION2_CPFC_MSGS_SETS_OVERRUNS_SUM_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 20 ,NULL, NULL},

                /* CPFC CC Over Under Flow Interrupt */
                {1, GT_FALSE,0, NULL, 0x138003C0, 0x138003C4,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_0_OVERFLOW_E,
                     PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_7_UNDERFLOW_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Counters Over Under Flow Interrupt */
                {2, GT_FALSE,0, NULL, 0x13800320, 0x13800324,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_COUNTERS_PORT_GROUP_0_OVERFLOW_E,
                     PRV_CPSS_LION2_CPFC_COUNTERS_WRONG_ADDR_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <0> Global Overflow Interrupt */
                {3, GT_FALSE,0, NULL, 0x13800330, 0x13800334,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_OVERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_OVERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <1> Global Overflow Interrupt */
                {4, GT_FALSE,0, NULL, 0x13800340, 0x13800344,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_OVERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_OVERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <2> Global Overflow Interrupt */
                {5, GT_FALSE,0, NULL, 0x13800350, 0x13800354,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_OVERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_OVERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <3> Global Overflow Interrupt */
                {6, GT_FALSE,0, NULL, 0x13800360, 0x13800364,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_OVERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_OVERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <4> Global Overflow Interrupt */
                {7, GT_FALSE,0, NULL, 0x13800370, 0x13800374,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_OVERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_OVERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <5> Global Overflow Interrupt */
                {8, GT_FALSE,0, NULL, 0x13800380, 0x13800384,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_OVERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_OVERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <6> Global Overflow Interrupt */
                {9, GT_FALSE,0, NULL, 0x13800390, 0x13800394,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_OVERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_OVERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <7> Global Overflow Interrupt */
                {10, GT_FALSE,0, NULL, 0x138003A0, 0x138003A4,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_OVERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_OVERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <0> Global Underflow Interrupt */
                {11, GT_FALSE,0, NULL, 0x13800338, 0x1380033C,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <1> Global Underflow Interrupt */
                {12, GT_FALSE,0, NULL, 0x13800348, 0x1380034C,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <2> Global Underflow Interrupt */
                {13, GT_FALSE,0, NULL, 0x13800358, 0x1380035C,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <3> Global Underflow Interrupt */
                {14, GT_FALSE,0, NULL, 0x13800368, 0x1380036C,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <4> Global Underflow Interrupt */
                {15, GT_FALSE,0, NULL, 0x13800378, 0x1380037C,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <5> Global Underflow Interrupt */
                {16, GT_FALSE,0, NULL, 0x13800388, 0x1380038C,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <6> Global Underflow Interrupt */
                {17, GT_FALSE,0, NULL, 0x13800398, 0x1380039C,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Port Group <7> Global Underflow Interrupt */
                {18, GT_FALSE,0, NULL, 0x138003A8, 0x138003AC,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_0_E,
                     PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_7_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC FIFOs Overruns Interrup */
                {19, GT_FALSE,0, NULL, 0x138003B0, 0x138003B4,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_FIFOS_OVERRUNS_PORT_GROUP_0_FC_IND_E,
                     PRV_CPSS_LION2_CPFC_FIFOS_OVERRUNS_PORT_GROUP_7_FC_IND_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL},

                /* CPFC Msgs Sets Overruns Interrupt */
                {20, GT_FALSE,0, NULL, 0x138003D0, 0x138003D4,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION2_CPFC_MSGS_PORT_GROUP_0_TXQ0_OVERRUN_E,
                     PRV_CPSS_LION2_CPFC_MSGS_PORT_GROUP_7_GLOBAL_COUNTER_OVERRUN_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0 ,NULL, NULL}

};


/* number of elements in the array of lionB0IntrScanArr[] */
#define LION2_NUM_ELEMENTS_IN_SCAN_TREE_CNS \
    (sizeof(lion2IntrScanArrSinglePortGroup)/sizeof(lion2IntrScanArrSinglePortGroup[0]))

/* number of mask registers -- according to enum of interrupts */
#define LION2_NUM_MASK_REGISTERS_CNS  (PRV_CPSS_LION2_LAST_INT_E / 32)

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 lion2MaskRegMapArr[LION2_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};

/* array per port group for lion2MaskRegMapArr */
static GT_U32 lion2MaskRegMapArr_perPortGroup[LION2_NUM_PORT_GROUPS_CNS][LION2_NUM_MASK_REGISTERS_CNS] =
{
    {FILLED_IN_RUNTIME_CNS}
};

/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 lion2MaskRegDefaultSummaryArr[LION2_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};

/*
    array for the 8 port groups of lion2 .
    for each port group : lion2 copied from Lion 2 and updated in runtime
*/
/* Don't use "const" because we Fill values at initialization stage */
static PRV_CPSS_DRV_INTERRUPT_SCAN_STC lion2IntrScanArr[LION2_NUM_PORT_GROUPS_CNS][LION2_NUM_ELEMENTS_IN_SCAN_TREE_CNS]=
{
    {{FILLED_IN_RUNTIME_CNS}}
};

/* Bobcat2_Gm interrupt tree */
static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC bobcat2_Gm_IntrScanArrSinglePortGroup[] =
{
    /* Global Interrupt Cause */
    {0, GT_FALSE, 0, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_LION2_GLOBAL_PEX_SUM_E,
        PRV_CPSS_LION2_GLOBAL_DFX_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 5 , NULL, NULL},
        /* Miscellaneous */
        {7,GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_MISC_TWSI_TIME_OUT_E,
            PRV_CPSS_LION2_MISC_Z80_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
        /* Tx SDMA  */
        {8, GT_FALSE,0, NULL, 0x00002810, 0x00002818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE0_E,
            PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE7_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
        /* Rx SDMA  */
        {9, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE0_E,
            PRV_CPSS_LION2_RECEIVE_SDMA_PACKET_CNT_OF_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
        /* L2 Bridge  */
        {14, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_BRIDGE_UPDATE_FIFO_FULL_E,
            PRV_CPSS_LION2_BRIDGE_UPDATE_SECURITY_BREACH_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},
        /* FDBIntSum - FDB Interrupt Cause,
           maskRcvIntrEn - both AUProcCompletedInt(bit 9) and AgeViaTriggerEnded(bit 4)
           should never be masked to avoid missed events situation.  */
        {17, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION2_FDB_NUM_OF_HOP_EX_P_E,
            PRV_CPSS_LION2_FDB_AU_FIFO_TO_CPU_IS_FULL_E,
            FILLED_IN_RUNTIME_CNS, 0x0, (0xFFFFFFFF & ~ (BIT_4 | BIT_9)), 0,NULL, NULL},
};

/* number of elements in the array of bobcat2_Gm_IntrScanArr[] */
#define BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS \
    (sizeof(bobcat2_Gm_IntrScanArrSinglePortGroup)/sizeof(bobcat2_Gm_IntrScanArrSinglePortGroup[0]))

#define BOBCAT2_NUM_MASK_REGISTERS_CNS  LION2_NUM_MASK_REGISTERS_CNS

/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 bobcat2_Gm_MaskRegDefaultSummaryArr[BOBCAT2_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 bobcat2_Gm_MaskRegMapArr[BOBCAT2_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};

/* Don't use "const" because we Fill values at initialization stage */
static PRV_CPSS_DRV_INTERRUPT_SCAN_STC bobcat2_Gm_IntrScanArr[BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS] =
{
    {FILLED_IN_RUNTIME_CNS}
};

/* Interrupt cause to unified event map for Lion2
 This Table is for unified event with extended data

  Array structure:
  1. The first element of the array and the elements after MARK_END_OF_UNI_EV_CNS are unified event types.
  2. Elements after unified event type are pairs of interrupt cause and event extended data,
     until MARK_END_OF_UNI_EV_CNS.
*/
static GT_U16 lion2UniEvMapTableWithExtData[] = {

 /* Per Port events */
 CPSS_PP_BM_MAX_BUFF_REACHED_E,
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT0_E,       MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT1_E,       MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT2_E,       MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT3_E,       MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT4_E,       MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT5_E,       MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT6_E,       MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT7_E,       MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT8_E,       MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT9_E,       MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT10_E,      MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT11_E,      MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_BM_1_MAX_BUFF_REACHED_PORT_CPU_63_E, 63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LINK_STATUS_CHANGED_E,
    PRV_CPSS_LION2_PORT0_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_LINK_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_LINK_STATUS_CHANGED_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_LINK_STATUS_CHANGED_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_LINK_STATUS_CHANGED_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_LINK_STATUS_CHANGED_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT0_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_LINK_STATUS_CHANGED_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_LINK_STATUS_CHANGED_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_LINK_STATUS_CHANGED_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_LINK_STATUS_CHANGED_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_LINK_STATUS_CHANGED_E,  63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_AN_COMPLETED_E,
    PRV_CPSS_LION2_PORT0_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_AN_COMPLETED_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_AN_COMPLETED_E,       MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_AN_COMPLETED_E,       MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_AN_COMPLETED_E,       MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_AN_COMPLETED_E,       MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_AN_COMPLETED_E, 63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_RX_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_PORT0_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_RX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_RX_FIFO_OVERRUN_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_RX_FIFO_OVERRUN_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_RX_FIFO_OVERRUN_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_RX_FIFO_OVERRUN_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT0_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_RX_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_RX_FIFO_OVERRUN_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_RX_FIFO_OVERRUN_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_RX_FIFO_OVERRUN_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_RX_FIFO_OVERRUN_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_RX_FIFO_OVERRUN_E,  63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_TX_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_PORT0_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_TX_FIFO_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_TX_FIFO_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_TX_FIFO_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_TX_FIFO_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_TX_FIFO_UNDERRUN_E,  63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_TX_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_PORT0_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_TX_FIFO_OVERRUN_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_TX_FIFO_OVERRUN_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_TX_FIFO_OVERRUN_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_TX_FIFO_OVERRUN_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_TX_FIFO_OVERRUN_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_TX_FIFO_OVERRUN_E,  63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_TX_UNDERRUN_E,
    PRV_CPSS_LION2_PORT0_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_TX_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_TX_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_TX_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_TX_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_TX_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT0_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_TX_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_TX_UNDERRUN_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_TX_UNDERRUN_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_TX_UNDERRUN_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_TX_UNDERRUN_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_TX_UNDERRUN_E,  63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E,
    PRV_CPSS_LION2_PORT0_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_ADDRESS_OUT_OF_RANGE_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_ADDRESS_OUT_OF_RANGE_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_ADDRESS_OUT_OF_RANGE_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_ADDRESS_OUT_OF_RANGE_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT0_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_ADDRESS_OUT_OF_RANGE_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_ADDRESS_OUT_OF_RANGE_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_ADDRESS_OUT_OF_RANGE_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_ADDRESS_OUT_OF_RANGE_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_ADDRESS_OUT_OF_RANGE_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_ADDRESS_OUT_OF_RANGE_E,  63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PRBS_ERROR_E,
    PRV_CPSS_LION2_PORT0_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_PRBS_ERROR_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_PRBS_ERROR_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_PRBS_ERROR_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_PRBS_ERROR_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_PRBS_ERROR_E,  63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_SYNC_STATUS_CHANGED_E,
    PRV_CPSS_LION2_PORT0_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PORT1_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PORT2_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PORT3_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PORT4_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PORT5_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PORT6_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PORT7_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PORT8_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PORT9_SYNC_STATUS_CHANGED_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT10_SYNC_STATUS_CHANGED_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PORT11_SYNC_STATUS_CHANGED_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PORT12_SYNC_STATUS_CHANGED_E,        MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PORT14_SYNC_STATUS_CHANGED_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_CPU_PORT_SUM_SYNC_STATUS_CHANGED_E,  63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_ILLEGAL_SEQUENCE_E,
    PRV_CPSS_LION2_XG_PORT0_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_UNKNOWN_SEQUENCE_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_UNKNOWN_SEQUENCE_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_UNKNOWN_SEQUENCE_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_UNKNOWN_SEQUENCE_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_FAULT_TYPE_CHANGE_E,
    PRV_CPSS_LION2_XG_PORT0_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_FAULT_CHANGED_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_FAULT_CHANGED_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_FAULT_CHANGED_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_FAULT_CHANGED_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_FAULT_CHANGED_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_FC_STATUS_CHANGED_E,
    PRV_CPSS_LION2_XG_PORT0_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_FC_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_FC_STATUS_CHANGED_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_FC_STATUS_CHANGED_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_FC_STATUS_CHANGED_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_FC_STATUS_CHANGED_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_TXQ2_FLUSH_PORT_E,
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT0_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT1_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT2_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT3_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT4_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT5_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT6_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT7_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT8_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT9_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT10_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT11_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_TXQ_DQ_FLUSH_DONE_PORT15_E,     63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_MAC_SFLOW_E,
    PRV_CPSS_LION2_INGRESS_STC_PORT0_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_INGRESS_STC_PORT1_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_INGRESS_STC_PORT2_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_INGRESS_STC_PORT3_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_INGRESS_STC_PORT4_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_INGRESS_STC_PORT5_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_INGRESS_STC_PORT6_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_INGRESS_STC_PORT7_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_INGRESS_STC_PORT8_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_INGRESS_STC_PORT9_INGRESS_SAMPLE_LOADED_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_INGRESS_STC_PORT10_INGRESS_SAMPLE_LOADED_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_INGRESS_STC_PORT11_INGRESS_SAMPLE_LOADED_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_EGRESS_SFLOW_E,
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT0_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT1_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT2_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT3_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT4_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT5_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT6_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT7_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT8_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT9_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT10_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT11_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E,
    PRV_CPSS_LION2_XG_PORT0_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_NO_BUFF_PACKET_DROP_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_NO_BUFF_PACKET_DROP_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_NO_BUFF_PACKET_DROP_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_NO_BUFF_PACKET_DROP_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_NO_BUFF_PACKET_DROP_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_COUNT_COPY_DONE_E,
    PRV_CPSS_LION2_XG_PORT0_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_XG_COUNT_COPY_DONE_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_XG_COUNT_COPY_DONE_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_XG_COUNT_COPY_DONE_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_XG_COUNT_COPY_DONE_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_XG_COUNT_COPY_DONE_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_COUNT_EXPIRED_E,
    PRV_CPSS_LION2_XG_PORT0_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XG_PORT1_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_XG_PORT2_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XG_PORT3_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_XG_PORT4_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XG_PORT5_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_XG_PORT6_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XG_PORT7_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_XG_PORT8_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XG_PORT9_XG_COUNT_EXPIRED_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT10_XG_COUNT_EXPIRED_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XG_PORT11_XG_COUNT_EXPIRED_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_XG_PORT12_XG_COUNT_EXPIRED_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XG_PORT14_XG_COUNT_EXPIRED_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_PORT_DESC_FULL_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT0_E, MARK_PER_HEM_INT_MAC(0),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT1_E, MARK_PER_HEM_INT_MAC(1),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT2_E, MARK_PER_HEM_INT_MAC(2),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT3_E, MARK_PER_HEM_INT_MAC(3),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT4_E, MARK_PER_HEM_INT_MAC(4),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT5_E, MARK_PER_HEM_INT_MAC(5),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT6_E, MARK_PER_HEM_INT_MAC(6),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT7_E, MARK_PER_HEM_INT_MAC(7),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT8_E, MARK_PER_HEM_INT_MAC(8),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT9_E, MARK_PER_HEM_INT_MAC(9),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT10_E, MARK_PER_HEM_INT_MAC(10),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT11_E, MARK_PER_HEM_INT_MAC(11),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT16_E, MARK_PER_HEM_INT_MAC(16),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT17_E, MARK_PER_HEM_INT_MAC(17),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT18_E, MARK_PER_HEM_INT_MAC(18),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT19_E, MARK_PER_HEM_INT_MAC(19),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT20_E, MARK_PER_HEM_INT_MAC(20),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT21_E, MARK_PER_HEM_INT_MAC(21),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT22_E, MARK_PER_HEM_INT_MAC(22),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT23_E, MARK_PER_HEM_INT_MAC(23),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT24_E, MARK_PER_HEM_INT_MAC(24),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT25_E, MARK_PER_HEM_INT_MAC(25),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT26_E, MARK_PER_HEM_INT_MAC(26),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT27_E, MARK_PER_HEM_INT_MAC(27),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT32_E, MARK_PER_HEM_INT_MAC(32),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT33_E, MARK_PER_HEM_INT_MAC(33),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT34_E, MARK_PER_HEM_INT_MAC(34),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT35_E, MARK_PER_HEM_INT_MAC(35),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT36_E, MARK_PER_HEM_INT_MAC(36),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT37_E, MARK_PER_HEM_INT_MAC(37),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT38_E, MARK_PER_HEM_INT_MAC(38),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT39_E, MARK_PER_HEM_INT_MAC(39),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT40_E, MARK_PER_HEM_INT_MAC(40),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT41_E, MARK_PER_HEM_INT_MAC(41),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT42_E, MARK_PER_HEM_INT_MAC(42),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT43_E, MARK_PER_HEM_INT_MAC(43),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT48_E, MARK_PER_HEM_INT_MAC(48),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT49_E, MARK_PER_HEM_INT_MAC(49),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT50_E, MARK_PER_HEM_INT_MAC(50),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT51_E, MARK_PER_HEM_INT_MAC(51),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT52_E, MARK_PER_HEM_INT_MAC(52),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT53_E, MARK_PER_HEM_INT_MAC(53),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT54_E, MARK_PER_HEM_INT_MAC(54),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT55_E, MARK_PER_HEM_INT_MAC(55),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT56_E, MARK_PER_HEM_INT_MAC(56),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT57_E, MARK_PER_HEM_INT_MAC(57),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT58_E, MARK_PER_HEM_INT_MAC(58),
    PRV_CPSS_LION2_TXQ_QUEUE_PORT_DESC_FULL_PORT59_E, MARK_PER_HEM_INT_MAC(59),
 MARK_END_OF_UNI_EV_CNS,

 /* Per Port PCS events */
 CPSS_PP_PORT_PCS_LINK_STATUS_CHANGED_E,
    PRV_CPSS_LION2_XPCS_PORT0_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XPCS_PORT2_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XPCS_PORT4_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XPCS_PORT6_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XPCS_PORT8_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XPCS_PORT10_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XPCS_PORT12_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XPCS_PORT14_LINK_STATUS_CHANGED_E,      MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_DESKEW_TIMEOUT_E,
    PRV_CPSS_LION2_XPCS_PORT0_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XPCS_PORT2_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XPCS_PORT4_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XPCS_PORT6_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XPCS_PORT8_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XPCS_PORT10_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XPCS_PORT12_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XPCS_PORT14_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_DETECTED_COLUMN_IIAII_E,
    PRV_CPSS_LION2_XPCS_PORT0_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XPCS_PORT2_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XPCS_PORT4_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XPCS_PORT6_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XPCS_PORT8_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XPCS_PORT10_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XPCS_PORT12_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XPCS_PORT14_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_DESKEW_ERROR_E,
    PRV_CPSS_LION2_XPCS_PORT0_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XPCS_PORT2_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XPCS_PORT4_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XPCS_PORT6_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XPCS_PORT8_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XPCS_PORT10_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XPCS_PORT12_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XPCS_PORT14_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_XPCS_PORT0_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XPCS_PORT2_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XPCS_PORT4_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XPCS_PORT6_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XPCS_PORT8_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XPCS_PORT10_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XPCS_PORT12_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XPCS_PORT14_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_XPCS_PORT0_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_XPCS_PORT2_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_XPCS_PORT4_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_XPCS_PORT6_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_XPCS_PORT8_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_XPCS_PORT10_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_XPCS_PORT12_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_XPCS_PORT14_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

/* Per Port MMPCS events */
 CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
    PRV_CPSS_LION2_PCS40G_MMPCS0_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_LION2_PCS40G_MMPCS1_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_LION2_PCS40G_MMPCS2_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_LION2_PCS40G_MMPCS3_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_LION2_PCS40G_MMPCS4_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_LION2_PCS40G_MMPCS5_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_LION2_PCS40G_MMPCS6_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_LION2_PCS40G_MMPCS7_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_LION2_PCS40G_MMPCS8_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_LION2_PCS40G_MMPCS9_SIGNAL_DETECT_CHANGE_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PCS40G_MMPCS10_SIGNAL_DETECT_CHANGE_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_LION2_PCS40G_MMPCS11_SIGNAL_DETECT_CHANGE_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_LION2_PCS40G_MMPCS12_SIGNAL_DETECT_CHANGE_E,     MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_LION2_PCS40G_MMPCS14_SIGNAL_DETECT_CHANGE_E,     MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,


 /* Per Port Per lane events */
 CPSS_PP_PORT_LANE_PRBS_ERROR_E,
    PRV_CPSS_LION2_XG_PORT0_LANE0_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_LION2_XG_PORT0_LANE1_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_LION2_XG_PORT0_LANE2_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_LION2_XG_PORT0_LANE3_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_LION2_XG_PORT0_LANE4_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,4),
    PRV_CPSS_LION2_XG_PORT0_LANE5_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,5),
    PRV_CPSS_LION2_XG_PORT2_LANE0_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_LION2_XG_PORT2_LANE1_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_LION2_XG_PORT2_LANE2_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_LION2_XG_PORT2_LANE3_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_LION2_XG_PORT2_LANE4_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,4),
    PRV_CPSS_LION2_XG_PORT2_LANE5_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,5),
    PRV_CPSS_LION2_XG_PORT4_LANE0_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_LION2_XG_PORT4_LANE1_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_LION2_XG_PORT4_LANE2_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_LION2_XG_PORT4_LANE3_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_LION2_XG_PORT4_LANE4_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,4),
    PRV_CPSS_LION2_XG_PORT4_LANE5_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,5),
    PRV_CPSS_LION2_XG_PORT6_LANE0_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_LION2_XG_PORT6_LANE1_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_LION2_XG_PORT6_LANE2_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_LION2_XG_PORT6_LANE3_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_LION2_XG_PORT6_LANE4_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,4),
    PRV_CPSS_LION2_XG_PORT6_LANE5_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,5),
    PRV_CPSS_LION2_XG_PORT8_LANE0_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_LION2_XG_PORT8_LANE1_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_LION2_XG_PORT8_LANE2_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_LION2_XG_PORT8_LANE3_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_LION2_XG_PORT8_LANE4_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,4),
    PRV_CPSS_LION2_XG_PORT8_LANE5_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,5),
    PRV_CPSS_LION2_XG_PORT10_LANE0_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_LION2_XG_PORT10_LANE1_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_LION2_XG_PORT10_LANE2_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_LION2_XG_PORT10_LANE3_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
    PRV_CPSS_LION2_XG_PORT10_LANE4_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,4),
    PRV_CPSS_LION2_XG_PORT10_LANE5_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,5),
    PRV_CPSS_LION2_XG_PORT12_LANE0_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,0),
    PRV_CPSS_LION2_XG_PORT12_LANE1_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,1),
    PRV_CPSS_LION2_XG_PORT12_LANE2_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,2),
    PRV_CPSS_LION2_XG_PORT12_LANE3_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,3),
    PRV_CPSS_LION2_XG_PORT12_LANE4_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,4),
    PRV_CPSS_LION2_XG_PORT12_LANE5_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,5),
    PRV_CPSS_LION2_XG_PORT14_LANE0_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,0),
    PRV_CPSS_LION2_XG_PORT14_LANE1_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,1),
    PRV_CPSS_LION2_XG_PORT14_LANE2_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,2),
    PRV_CPSS_LION2_XG_PORT14_LANE3_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,3),
    PRV_CPSS_LION2_XG_PORT14_LANE4_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,4),
    PRV_CPSS_LION2_XG_PORT14_LANE5_SUM_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,5),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_DISPARITY_ERROR_E,
    PRV_CPSS_LION2_XG_PORT0_LANE0_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_LION2_XG_PORT0_LANE1_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_LION2_XG_PORT0_LANE2_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_LION2_XG_PORT0_LANE3_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_LION2_XG_PORT0_LANE4_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,4),
    PRV_CPSS_LION2_XG_PORT0_LANE5_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,5),
    PRV_CPSS_LION2_XG_PORT2_LANE0_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_LION2_XG_PORT2_LANE1_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_LION2_XG_PORT2_LANE2_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_LION2_XG_PORT2_LANE3_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_LION2_XG_PORT2_LANE4_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,4),
    PRV_CPSS_LION2_XG_PORT2_LANE5_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,5),
    PRV_CPSS_LION2_XG_PORT4_LANE0_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_LION2_XG_PORT4_LANE1_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_LION2_XG_PORT4_LANE2_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_LION2_XG_PORT4_LANE3_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_LION2_XG_PORT4_LANE4_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,4),
    PRV_CPSS_LION2_XG_PORT4_LANE5_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,5),
    PRV_CPSS_LION2_XG_PORT6_LANE0_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_LION2_XG_PORT6_LANE1_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_LION2_XG_PORT6_LANE2_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_LION2_XG_PORT6_LANE3_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_LION2_XG_PORT6_LANE4_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,4),
    PRV_CPSS_LION2_XG_PORT6_LANE5_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,5),
    PRV_CPSS_LION2_XG_PORT8_LANE0_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_LION2_XG_PORT8_LANE1_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_LION2_XG_PORT8_LANE2_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_LION2_XG_PORT8_LANE3_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_LION2_XG_PORT8_LANE4_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,4),
    PRV_CPSS_LION2_XG_PORT8_LANE5_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,5),
    PRV_CPSS_LION2_XG_PORT10_LANE0_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_LION2_XG_PORT10_LANE1_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_LION2_XG_PORT10_LANE2_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_LION2_XG_PORT10_LANE3_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
    PRV_CPSS_LION2_XG_PORT10_LANE4_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,4),
    PRV_CPSS_LION2_XG_PORT10_LANE5_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,5),
    PRV_CPSS_LION2_XG_PORT12_LANE0_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,0),
    PRV_CPSS_LION2_XG_PORT12_LANE1_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,1),
    PRV_CPSS_LION2_XG_PORT12_LANE2_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,2),
    PRV_CPSS_LION2_XG_PORT12_LANE3_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,3),
    PRV_CPSS_LION2_XG_PORT12_LANE4_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,4),
    PRV_CPSS_LION2_XG_PORT12_LANE5_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,5),
    PRV_CPSS_LION2_XG_PORT14_LANE0_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,0),
    PRV_CPSS_LION2_XG_PORT14_LANE1_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,1),
    PRV_CPSS_LION2_XG_PORT14_LANE2_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,2),
    PRV_CPSS_LION2_XG_PORT14_LANE3_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,3),
    PRV_CPSS_LION2_XG_PORT14_LANE4_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,4),
    PRV_CPSS_LION2_XG_PORT14_LANE5_SUM_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,5),
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_PORT_LANE_SYMBOL_ERROR_E,
    PRV_CPSS_LION2_XG_PORT0_LANE0_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_LION2_XG_PORT0_LANE1_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_LION2_XG_PORT0_LANE2_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_LION2_XG_PORT0_LANE3_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_LION2_XG_PORT0_LANE4_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,4),
    PRV_CPSS_LION2_XG_PORT0_LANE5_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,5),
    PRV_CPSS_LION2_XG_PORT2_LANE0_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_LION2_XG_PORT2_LANE1_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_LION2_XG_PORT2_LANE2_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_LION2_XG_PORT2_LANE3_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_LION2_XG_PORT2_LANE4_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,4),
    PRV_CPSS_LION2_XG_PORT2_LANE5_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,5),
    PRV_CPSS_LION2_XG_PORT4_LANE0_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_LION2_XG_PORT4_LANE1_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_LION2_XG_PORT4_LANE2_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_LION2_XG_PORT4_LANE3_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_LION2_XG_PORT4_LANE4_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,4),
    PRV_CPSS_LION2_XG_PORT4_LANE5_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,5),
    PRV_CPSS_LION2_XG_PORT6_LANE0_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_LION2_XG_PORT6_LANE1_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_LION2_XG_PORT6_LANE2_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_LION2_XG_PORT6_LANE3_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_LION2_XG_PORT6_LANE4_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,4),
    PRV_CPSS_LION2_XG_PORT6_LANE5_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,5),
    PRV_CPSS_LION2_XG_PORT8_LANE0_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_LION2_XG_PORT8_LANE1_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_LION2_XG_PORT8_LANE2_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_LION2_XG_PORT8_LANE3_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_LION2_XG_PORT8_LANE4_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,4),
    PRV_CPSS_LION2_XG_PORT8_LANE5_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,5),
    PRV_CPSS_LION2_XG_PORT10_LANE0_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_LION2_XG_PORT10_LANE1_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_LION2_XG_PORT10_LANE2_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_LION2_XG_PORT10_LANE3_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
    PRV_CPSS_LION2_XG_PORT10_LANE4_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,4),
    PRV_CPSS_LION2_XG_PORT10_LANE5_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,5),
    PRV_CPSS_LION2_XG_PORT12_LANE0_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,0),
    PRV_CPSS_LION2_XG_PORT12_LANE1_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,1),
    PRV_CPSS_LION2_XG_PORT12_LANE2_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,2),
    PRV_CPSS_LION2_XG_PORT12_LANE3_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,3),
    PRV_CPSS_LION2_XG_PORT12_LANE4_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,4),
    PRV_CPSS_LION2_XG_PORT12_LANE5_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,5),
    PRV_CPSS_LION2_XG_PORT14_LANE0_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,0),
    PRV_CPSS_LION2_XG_PORT14_LANE1_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,1),
    PRV_CPSS_LION2_XG_PORT14_LANE2_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,2),
    PRV_CPSS_LION2_XG_PORT14_LANE3_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,3),
    PRV_CPSS_LION2_XG_PORT14_LANE4_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,4),
    PRV_CPSS_LION2_XG_PORT14_LANE5_SUM_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,5),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_CJR_PAT_ERROR_E,
    PRV_CPSS_LION2_XG_PORT0_LANE0_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_LION2_XG_PORT0_LANE1_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_LION2_XG_PORT0_LANE2_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_LION2_XG_PORT0_LANE3_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_LION2_XG_PORT0_LANE4_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,4),
    PRV_CPSS_LION2_XG_PORT0_LANE5_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,5),
    PRV_CPSS_LION2_XG_PORT2_LANE0_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_LION2_XG_PORT2_LANE1_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_LION2_XG_PORT2_LANE2_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_LION2_XG_PORT2_LANE3_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_LION2_XG_PORT2_LANE4_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,4),
    PRV_CPSS_LION2_XG_PORT2_LANE5_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,5),
    PRV_CPSS_LION2_XG_PORT4_LANE0_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_LION2_XG_PORT4_LANE1_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_LION2_XG_PORT4_LANE2_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_LION2_XG_PORT4_LANE3_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_LION2_XG_PORT4_LANE4_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,4),
    PRV_CPSS_LION2_XG_PORT4_LANE5_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,5),
    PRV_CPSS_LION2_XG_PORT6_LANE0_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_LION2_XG_PORT6_LANE1_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_LION2_XG_PORT6_LANE2_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_LION2_XG_PORT6_LANE3_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_LION2_XG_PORT6_LANE4_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,4),
    PRV_CPSS_LION2_XG_PORT6_LANE5_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,5),
    PRV_CPSS_LION2_XG_PORT8_LANE0_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_LION2_XG_PORT8_LANE1_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_LION2_XG_PORT8_LANE2_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_LION2_XG_PORT8_LANE3_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_LION2_XG_PORT8_LANE4_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,4),
    PRV_CPSS_LION2_XG_PORT8_LANE5_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,5),
    PRV_CPSS_LION2_XG_PORT10_LANE0_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_LION2_XG_PORT10_LANE1_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_LION2_XG_PORT10_LANE2_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_LION2_XG_PORT10_LANE3_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
    PRV_CPSS_LION2_XG_PORT10_LANE4_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,4),
    PRV_CPSS_LION2_XG_PORT10_LANE5_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,5),
    PRV_CPSS_LION2_XG_PORT12_LANE0_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,0),
    PRV_CPSS_LION2_XG_PORT12_LANE1_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,1),
    PRV_CPSS_LION2_XG_PORT12_LANE2_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,2),
    PRV_CPSS_LION2_XG_PORT12_LANE3_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,3),
    PRV_CPSS_LION2_XG_PORT12_LANE4_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,4),
    PRV_CPSS_LION2_XG_PORT12_LANE5_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,5),
    PRV_CPSS_LION2_XG_PORT14_LANE0_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,0),
    PRV_CPSS_LION2_XG_PORT14_LANE1_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,1),
    PRV_CPSS_LION2_XG_PORT14_LANE2_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,2),
    PRV_CPSS_LION2_XG_PORT14_LANE3_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,3),
    PRV_CPSS_LION2_XG_PORT14_LANE4_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,4),
    PRV_CPSS_LION2_XG_PORT14_LANE5_SUM_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,5),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,
    PRV_CPSS_LION2_XG_PORT0_LANE0_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_LION2_XG_PORT0_LANE1_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_LION2_XG_PORT0_LANE2_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_LION2_XG_PORT0_LANE3_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_LION2_XG_PORT0_LANE4_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,4),
    PRV_CPSS_LION2_XG_PORT0_LANE5_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,5),
    PRV_CPSS_LION2_XG_PORT2_LANE0_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_LION2_XG_PORT2_LANE1_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_LION2_XG_PORT2_LANE2_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_LION2_XG_PORT2_LANE3_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_LION2_XG_PORT2_LANE4_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,4),
    PRV_CPSS_LION2_XG_PORT2_LANE5_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,5),
    PRV_CPSS_LION2_XG_PORT4_LANE0_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_LION2_XG_PORT4_LANE1_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_LION2_XG_PORT4_LANE2_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_LION2_XG_PORT4_LANE3_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_LION2_XG_PORT4_LANE4_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,4),
    PRV_CPSS_LION2_XG_PORT4_LANE5_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,5),
    PRV_CPSS_LION2_XG_PORT6_LANE0_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_LION2_XG_PORT6_LANE1_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_LION2_XG_PORT6_LANE2_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_LION2_XG_PORT6_LANE3_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_LION2_XG_PORT6_LANE4_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,4),
    PRV_CPSS_LION2_XG_PORT6_LANE5_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,5),
    PRV_CPSS_LION2_XG_PORT8_LANE0_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_LION2_XG_PORT8_LANE1_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_LION2_XG_PORT8_LANE2_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_LION2_XG_PORT8_LANE3_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_LION2_XG_PORT8_LANE4_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,4),
    PRV_CPSS_LION2_XG_PORT8_LANE5_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,5),
    PRV_CPSS_LION2_XG_PORT10_LANE0_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_LION2_XG_PORT10_LANE1_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_LION2_XG_PORT10_LANE2_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_LION2_XG_PORT10_LANE3_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
    PRV_CPSS_LION2_XG_PORT10_LANE4_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,4),
    PRV_CPSS_LION2_XG_PORT10_LANE5_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,5),
    PRV_CPSS_LION2_XG_PORT12_LANE0_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,0),
    PRV_CPSS_LION2_XG_PORT12_LANE1_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,1),
    PRV_CPSS_LION2_XG_PORT12_LANE2_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,2),
    PRV_CPSS_LION2_XG_PORT12_LANE3_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,3),
    PRV_CPSS_LION2_XG_PORT12_LANE4_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,4),
    PRV_CPSS_LION2_XG_PORT12_LANE5_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,5),
    PRV_CPSS_LION2_XG_PORT14_LANE0_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,0),
    PRV_CPSS_LION2_XG_PORT14_LANE1_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,1),
    PRV_CPSS_LION2_XG_PORT14_LANE2_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,2),
    PRV_CPSS_LION2_XG_PORT14_LANE3_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,3),
    PRV_CPSS_LION2_XG_PORT14_LANE4_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,4),
    PRV_CPSS_LION2_XG_PORT14_LANE5_SUM_SIGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,5),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E,
    PRV_CPSS_LION2_XG_PORT0_LANE0_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_LION2_XG_PORT0_LANE1_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_LION2_XG_PORT0_LANE2_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_LION2_XG_PORT0_LANE3_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_LION2_XG_PORT0_LANE4_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,4),
    PRV_CPSS_LION2_XG_PORT0_LANE5_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,5),
    PRV_CPSS_LION2_XG_PORT2_LANE0_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_LION2_XG_PORT2_LANE1_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_LION2_XG_PORT2_LANE2_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_LION2_XG_PORT2_LANE3_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_LION2_XG_PORT2_LANE4_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,4),
    PRV_CPSS_LION2_XG_PORT2_LANE5_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,5),
    PRV_CPSS_LION2_XG_PORT4_LANE0_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_LION2_XG_PORT4_LANE1_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_LION2_XG_PORT4_LANE2_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_LION2_XG_PORT4_LANE3_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_LION2_XG_PORT4_LANE4_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,4),
    PRV_CPSS_LION2_XG_PORT4_LANE5_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,5),
    PRV_CPSS_LION2_XG_PORT6_LANE0_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_LION2_XG_PORT6_LANE1_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_LION2_XG_PORT6_LANE2_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_LION2_XG_PORT6_LANE3_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_LION2_XG_PORT6_LANE4_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,4),
    PRV_CPSS_LION2_XG_PORT6_LANE5_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,5),
    PRV_CPSS_LION2_XG_PORT8_LANE0_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_LION2_XG_PORT8_LANE1_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_LION2_XG_PORT8_LANE2_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_LION2_XG_PORT8_LANE3_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_LION2_XG_PORT8_LANE4_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,4),
    PRV_CPSS_LION2_XG_PORT8_LANE5_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,5),
    PRV_CPSS_LION2_XG_PORT10_LANE0_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_LION2_XG_PORT10_LANE1_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_LION2_XG_PORT10_LANE2_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_LION2_XG_PORT10_LANE3_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
    PRV_CPSS_LION2_XG_PORT10_LANE4_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,4),
    PRV_CPSS_LION2_XG_PORT10_LANE5_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,5),
    PRV_CPSS_LION2_XG_PORT12_LANE0_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,0),
    PRV_CPSS_LION2_XG_PORT12_LANE1_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,1),
    PRV_CPSS_LION2_XG_PORT12_LANE2_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,2),
    PRV_CPSS_LION2_XG_PORT12_LANE3_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,3),
    PRV_CPSS_LION2_XG_PORT12_LANE4_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,4),
    PRV_CPSS_LION2_XG_PORT12_LANE5_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,5),
    PRV_CPSS_LION2_XG_PORT14_LANE0_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,0),
    PRV_CPSS_LION2_XG_PORT14_LANE1_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,1),
    PRV_CPSS_LION2_XG_PORT14_LANE2_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,2),
    PRV_CPSS_LION2_XG_PORT14_LANE3_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,3),
    PRV_CPSS_LION2_XG_PORT14_LANE4_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,4),
    PRV_CPSS_LION2_XG_PORT14_LANE5_SUM_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,5),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_DETECTED_IIAII_E,
    PRV_CPSS_LION2_XG_PORT0_LANE0_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_LION2_XG_PORT0_LANE1_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_LION2_XG_PORT0_LANE2_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_LION2_XG_PORT0_LANE3_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_LION2_XG_PORT0_LANE4_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,4),
    PRV_CPSS_LION2_XG_PORT0_LANE5_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,5),
    PRV_CPSS_LION2_XG_PORT2_LANE0_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_LION2_XG_PORT2_LANE1_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_LION2_XG_PORT2_LANE2_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_LION2_XG_PORT2_LANE3_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_LION2_XG_PORT2_LANE4_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,4),
    PRV_CPSS_LION2_XG_PORT2_LANE5_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,5),
    PRV_CPSS_LION2_XG_PORT4_LANE0_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_LION2_XG_PORT4_LANE1_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_LION2_XG_PORT4_LANE2_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_LION2_XG_PORT4_LANE3_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_LION2_XG_PORT4_LANE4_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,4),
    PRV_CPSS_LION2_XG_PORT4_LANE5_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,5),
    PRV_CPSS_LION2_XG_PORT6_LANE0_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_LION2_XG_PORT6_LANE1_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_LION2_XG_PORT6_LANE2_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_LION2_XG_PORT6_LANE3_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_LION2_XG_PORT6_LANE4_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,4),
    PRV_CPSS_LION2_XG_PORT6_LANE5_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,5),
    PRV_CPSS_LION2_XG_PORT8_LANE0_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_LION2_XG_PORT8_LANE1_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_LION2_XG_PORT8_LANE2_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_LION2_XG_PORT8_LANE3_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_LION2_XG_PORT8_LANE4_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,4),
    PRV_CPSS_LION2_XG_PORT8_LANE5_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,5),
    PRV_CPSS_LION2_XG_PORT10_LANE0_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_LION2_XG_PORT10_LANE1_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_LION2_XG_PORT10_LANE2_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_LION2_XG_PORT10_LANE3_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
    PRV_CPSS_LION2_XG_PORT10_LANE4_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,4),
    PRV_CPSS_LION2_XG_PORT10_LANE5_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,5),
    PRV_CPSS_LION2_XG_PORT12_LANE0_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,0),
    PRV_CPSS_LION2_XG_PORT12_LANE1_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,1),
    PRV_CPSS_LION2_XG_PORT12_LANE2_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,2),
    PRV_CPSS_LION2_XG_PORT12_LANE3_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,3),
    PRV_CPSS_LION2_XG_PORT12_LANE4_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,4),
    PRV_CPSS_LION2_XG_PORT12_LANE5_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(9,5),
    PRV_CPSS_LION2_XG_PORT14_LANE0_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,0),
    PRV_CPSS_LION2_XG_PORT14_LANE1_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,1),
    PRV_CPSS_LION2_XG_PORT14_LANE2_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,2),
    PRV_CPSS_LION2_XG_PORT14_LANE3_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,3),
    PRV_CPSS_LION2_XG_PORT14_LANE4_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,4),
    PRV_CPSS_LION2_XG_PORT14_LANE5_SUM_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(11,5),
 MARK_END_OF_UNI_EV_CNS,

 /* Per Queue events */
 CPSS_PP_TX_BUFFER_QUEUE_E,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE0_E,  0,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE1_E,  1,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE2_E,  2,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE3_E,  3,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE4_E,  4,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE5_E,  5,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE6_E,  6,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_BUFFER_QUEUE7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TX_ERR_QUEUE_E,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_ERROR_QUEUE0_E,  0,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_ERROR_QUEUE1_E,  1,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_ERROR_QUEUE2_E,  2,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_ERROR_QUEUE3_E,  3,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_ERROR_QUEUE4_E,  4,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_ERROR_QUEUE5_E,  5,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_ERROR_QUEUE6_E,  6,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_ERROR_QUEUE7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TX_END_E,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE0_E,  0,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE1_E,  1,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE2_E,  2,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE3_E,  3,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE4_E,  4,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE5_E,  5,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE6_E,  6,
    PRV_CPSS_LION2_TRANSMIT_SDMA_TX_END_QUEUE7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

  /* Per Index events */
 CPSS_PP_GPP_E,
    PRV_CPSS_LION2_MISC_GPP_0_E,  0,
    PRV_CPSS_LION2_MISC_GPP_1_E,  1,
    PRV_CPSS_LION2_MISC_GPP_2_E,  2,
    PRV_CPSS_LION2_MISC_GPP_3_E,  3,
    PRV_CPSS_LION2_MISC_GPP_4_E,  4,
    PRV_CPSS_LION2_MISC_GPP_5_E,  5,
    PRV_CPSS_LION2_MISC_GPP_6_E,  6,
    PRV_CPSS_LION2_MISC_GPP_7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_CNC_WRAPAROUND_BLOCK_E,
    PRV_CPSS_LION2_CNC_BLOCK0_WRAPAROUND_E,  0,
    PRV_CPSS_LION2_CNC_BLOCK1_WRAPAROUND_E,  1,
    PRV_CPSS_LION2_CNC_BLOCK2_WRAPAROUND_E,  2,
    PRV_CPSS_LION2_CNC_BLOCK3_WRAPAROUND_E,  3,
    PRV_CPSS_LION2_CNC_BLOCK4_WRAPAROUND_E,  4,
    PRV_CPSS_LION2_CNC_BLOCK5_WRAPAROUND_E,  5,
    PRV_CPSS_LION2_CNC_BLOCK6_WRAPAROUND_E,  6,
    PRV_CPSS_LION2_CNC_BLOCK7_WRAPAROUND_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E,
    PRV_CPSS_LION2_BM_0_PORT_RX_BUFFERS_CNT_UNDERRUN_E, 0,
    PRV_CPSS_LION2_BM_0_GE_CNT_UNDERRUN_E,              1,
    PRV_CPSS_LION2_BM_0_GLOBAL_CNT_UNDERRUN_E,          3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E,
    PRV_CPSS_LION2_BM_0_PORT_RX_BUFFERS_CNT_OVERRUN_E,  0,
    PRV_CPSS_LION2_BM_0_GE_CNT_OVERRUN_E,               1,
    PRV_CPSS_LION2_BM_0_GLOBAL_CNT_OVERRUN_E,           3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_BM_MISC_E,
    PRV_CPSS_LION2_BM_0_LL_PORT2_PARITY_ERROR_E,  0,
    PRV_CPSS_LION2_BM_0_LL_PORT1_PARITY_ERROR_E,  1,
    PRV_CPSS_LION2_BM_0_CNRL_MEM_PARITY_ERROR_E,  2,
    PRV_CPSS_LION2_BM_0_MC_CNT_PARITY_ERROR_E,    3,
    PRV_CPSS_LION2_BM_0_PORT_RX_FULL_E,           4,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_CNTR_OVERFLOW_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RESOURCE_ERROR_CNT_OF_E,  0,
    PRV_CPSS_LION2_RECEIVE_SDMA_BYTE_CNT_OF_E,            1,
    PRV_CPSS_LION2_RECEIVE_SDMA_PACKET_CNT_OF_E,          2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_MISC_E,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_WRONG_ADDR_E,          0,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_GLOBAL_DESC_FULL_E,    1,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_GLOBAL_BUFF_FULL_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_SCT_RATE_LIMITER_E,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_1_PKT_DROPPED_E,    1,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_2_PKT_DROPPED_E,    2,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_3_PKT_DROPPED_E,    3,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_4_PKT_DROPPED_E,    4,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_5_PKT_DROPPED_E,    5,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_6_PKT_DROPPED_E,    6,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_7_PKT_DROPPED_E,    7,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_8_PKT_DROPPED_E,    8,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_9_PKT_DROPPED_E,    9,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_10_PKT_DROPPED_E,    10,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_11_PKT_DROPPED_E,    11,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_12_PKT_DROPPED_E,    12,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_13_PKT_DROPPED_E,    13,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_14_PKT_DROPPED_E,    14,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_15_PKT_DROPPED_E,    15,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_16_PKT_DROPPED_E,    16,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_17_PKT_DROPPED_E,    17,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_18_PKT_DROPPED_E,    18,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_19_PKT_DROPPED_E,    19,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_20_PKT_DROPPED_E,    20,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_21_PKT_DROPPED_E,    21,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_22_PKT_DROPPED_E,    22,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_23_PKT_DROPPED_E,    23,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_24_PKT_DROPPED_E,    24,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_25_PKT_DROPPED_E,    25,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_26_PKT_DROPPED_E,    26,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_27_PKT_DROPPED_E,    27,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_28_PKT_DROPPED_E,    28,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_29_PKT_DROPPED_E,    29,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_30_PKT_DROPPED_E,    30,
    PRV_CPSS_LION2_STC_RATE_LIMITERS_STC_RATE_LIMITER_31_PKT_DROPPED_E,    31,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E,
    PRV_CPSS_LION2_IPLR0_WRONG_ADDR_E,    0,
    PRV_CPSS_LION2_EPLR_WRONG_ADDR_E,     1,
    PRV_CPSS_LION2_IPLR1_WRONG_ADDR_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_DATA_ERR_E,
    PRV_CPSS_LION2_EPLR_DATA_ERROR_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E,
    PRV_CPSS_LION2_IPLR0_IPFIX_WRAP_AROUND_E,    0,
    PRV_CPSS_LION2_EPLR_IPFIX_WRAP_AROUND_E,     1,
    PRV_CPSS_LION2_IPLR1_IPFIX_WRAP_AROUND_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_ALARM_E,
    PRV_CPSS_LION2_IPLR0_IPFIX_ALARM_E,    0,
    PRV_CPSS_LION2_EPLR_IPFIX_ALARM_E,     1,
    PRV_CPSS_LION2_IPLR1_IPFIX_ALARM_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
    PRV_CPSS_LION2_IPLR0_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,    0,
    PRV_CPSS_LION2_EPLR_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,     1,
    PRV_CPSS_LION2_IPLR1_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PCL_LOOKUP_DATA_ERROR_E,
    PRV_CPSS_LION2_POLICY_ENGINE_PCL0_TCAM_ACCESS_DATA_ERROR_E,       0,
    PRV_CPSS_LION2_POLICY_ENGINE_PCL0_1_TCAM_ACCESS_DATA_ERROR_E,     1,
    PRV_CPSS_LION2_POLICY_ENGINE_PCL1_TCAM_ACCESS_DATA_ERROR_E,       2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PCL_LOOKUP_FIFO_FULL_E,
    PRV_CPSS_LION2_POLICY_ENGINE_PCL0_FIFO_FULL_E,       0,
    PRV_CPSS_LION2_POLICY_ENGINE_PCL0_1_FIFO_FULL_E,     1,
    PRV_CPSS_LION2_POLICY_ENGINE_PCL1_FIFO_FULL_E,       2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TCC_TCAM_BIST_FAILED_E,
    PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_BIST_FAILED_E,     0,
    PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_BIST_FAILED_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_GTS_GLOBAL_FIFO_FULL_E,
    PRV_CPSS_LION2_TTI_GTS_GLOBAL_FIFO_FULL_E,     0,
    PRV_CPSS_LION2_HA_GTS_GLOBAL_FIFO_OVERRUN_E,   1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_GTS_VALID_TIME_SAMPLE_MESSAGE_E,
    PRV_CPSS_LION2_TTI_GTS_VALID_TIME_SAMPLE_MESSAGE_E,  0,
    PRV_CPSS_LION2_HA_GTS_VALID_TIME_SAMPLE_MESSAGE_E,   1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE0_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE0_E,     0,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE1_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE1_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE2_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE2_E,     2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE3_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE3_E,     3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE4_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE4_E,     4,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE5_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE5_E,     5,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE6_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE6_E,     6,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE7_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_BUFFER_QUEUE7_E,     7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE0_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_ERROR_QUEUE0_E,      0,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE1_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_ERROR_QUEUE1_E,      1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE2_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_ERROR_QUEUE2_E,      2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE3_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_ERROR_QUEUE3_E,      3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE4_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_ERROR_QUEUE4_E,      4,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE5_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_ERROR_QUEUE5_E,      5,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE6_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_ERROR_QUEUE6_E,      6,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE7_E,
    PRV_CPSS_LION2_RECEIVE_SDMA_RX_ERROR_QUEUE7_E,      7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_CRITICAL_HW_ERROR_E,
    PRV_CPSS_LION2_TXDMA_GEN_BURST_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_TXDMA_GEN_BURST_FIFO_UNDERRUN_E,
    /* PRV_CPSS_LION2_TXDMA_ERROR_CLEAR_FIFO_FULL_E,      PRV_CPSS_LION2_TXDMA_ERROR_CLEAR_FIFO_FULL_E, false alarm in FWS tests 360 Mhz core clock */
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_0_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_0_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_1_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_1_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_2_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_2_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_3_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_3_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_4_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_4_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_5_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_5_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_6_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_6_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_7_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_7_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_8_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_8_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_9_OVERRUN_E,       PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_9_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_10_OVERRUN_E,      PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_10_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_11_OVERRUN_E,      PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_11_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_12_OVERRUN_E,      PRV_CPSS_LION2_TXDMA_GEN_TXFIFO_12_OVERRUN_E,
    PRV_CPSS_LION2_TXDMA_ERROR_CUT_THROUGH_ERROR_E,    PRV_CPSS_LION2_TXDMA_ERROR_CUT_THROUGH_ERROR_E,
    PRV_CPSS_LION2_RXDMA_FUNC_IBUF_SYNC_FIFO_0_FULL_E, PRV_CPSS_LION2_RXDMA_FUNC_IBUF_SYNC_FIFO_0_FULL_E,
    PRV_CPSS_LION2_RXDMA_FUNC_IBUF_SYNC_FIFO_1_FULL_E, PRV_CPSS_LION2_RXDMA_FUNC_IBUF_SYNC_FIFO_1_FULL_E,
    PRV_CPSS_LION2_CTU0_FCP0_PORTA_ECC_ERR_E,          PRV_CPSS_LION2_CTU0_FCP0_PORTA_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_FCP1_PORTA_ECC_ERR_E,          PRV_CPSS_LION2_CTU0_FCP1_PORTA_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_FCP2_PORTA_ECC_ERR_E,          PRV_CPSS_LION2_CTU0_FCP2_PORTA_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_FCP3_PORTA_ECC_ERR_E,          PRV_CPSS_LION2_CTU0_FCP3_PORTA_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_FCP0_PORTB_ECC_ERR_E,          PRV_CPSS_LION2_CTU0_FCP0_PORTB_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_FCP1_PORTB_ECC_ERR_E,          PRV_CPSS_LION2_CTU0_FCP1_PORTB_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_FCP2_PORTB_ECC_ERR_E,          PRV_CPSS_LION2_CTU0_FCP2_PORTB_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_FCP3_PORTB_ECC_ERR_E,          PRV_CPSS_LION2_CTU0_FCP3_PORTB_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_OCP0_START_ON_VALID_ENTRY_E,   PRV_CPSS_LION2_CTU0_OCP0_START_ON_VALID_ENTRY_E,
    PRV_CPSS_LION2_CTU0_OCP1_START_ON_VALID_ENTRY_E,   PRV_CPSS_LION2_CTU0_OCP1_START_ON_VALID_ENTRY_E,
    PRV_CPSS_LION2_CTU0_OCP2_START_ON_VALID_ENTRY_E,   PRV_CPSS_LION2_CTU0_OCP2_START_ON_VALID_ENTRY_E,
    PRV_CPSS_LION2_CTU0_OCP3_START_ON_VALID_ENTRY_E,   PRV_CPSS_LION2_CTU0_OCP3_START_ON_VALID_ENTRY_E,
    PRV_CPSS_LION2_CTU0_OCP0_END_ON_INVALID_ENTRY_E,   PRV_CPSS_LION2_CTU0_OCP0_END_ON_INVALID_ENTRY_E,
    PRV_CPSS_LION2_CTU0_OCP1_END_ON_INVALID_ENTRY_E,   PRV_CPSS_LION2_CTU0_OCP1_END_ON_INVALID_ENTRY_E,
    PRV_CPSS_LION2_CTU0_OCP2_END_ON_INVALID_ENTRY_E,   PRV_CPSS_LION2_CTU0_OCP2_END_ON_INVALID_ENTRY_E,
    PRV_CPSS_LION2_CTU0_OCP3_END_ON_INVALID_ENTRY_E,   PRV_CPSS_LION2_CTU0_OCP3_END_ON_INVALID_ENTRY_E,
    PRV_CPSS_LION2_CTU0_FIFO_OVERRUN_E,                PRV_CPSS_LION2_CTU0_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_CTU0_FCP0_SAME_ADDR_WR_A_RD_B_E,    PRV_CPSS_LION2_CTU0_FCP0_SAME_ADDR_WR_A_RD_B_E,
    PRV_CPSS_LION2_CTU0_FCP1_SAME_ADDR_WR_A_RD_B_E,    PRV_CPSS_LION2_CTU0_FCP1_SAME_ADDR_WR_A_RD_B_E,
    PRV_CPSS_LION2_CTU0_FCP2_SAME_ADDR_WR_A_RD_B_E,    PRV_CPSS_LION2_CTU0_FCP2_SAME_ADDR_WR_A_RD_B_E,
    PRV_CPSS_LION2_CTU0_FCP3_SAME_ADDR_WR_A_RD_B_E,    PRV_CPSS_LION2_CTU0_FCP3_SAME_ADDR_WR_A_RD_B_E,
    PRV_CPSS_LION2_CTU0_FCP0_SAME_ADDR_WR_B_RD_A_E,    PRV_CPSS_LION2_CTU0_FCP0_SAME_ADDR_WR_B_RD_A_E,
    PRV_CPSS_LION2_CTU0_FCP1_SAME_ADDR_WR_B_RD_A_E,    PRV_CPSS_LION2_CTU0_FCP1_SAME_ADDR_WR_B_RD_A_E,
    PRV_CPSS_LION2_CTU0_FCP2_SAME_ADDR_WR_B_RD_A_E,    PRV_CPSS_LION2_CTU0_FCP2_SAME_ADDR_WR_B_RD_A_E,
    PRV_CPSS_LION2_CTU0_FCP3_SAME_ADDR_WR_B_RD_A_E,    PRV_CPSS_LION2_CTU0_FCP3_SAME_ADDR_WR_B_RD_A_E,
    PRV_CPSS_LION2_CTU0_DESC0_UNUSED_FIFO_ECC_ERR_E,   PRV_CPSS_LION2_CTU0_DESC0_UNUSED_FIFO_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_DESC1_UNUSED_FIFO_ECC_ERR_E,   PRV_CPSS_LION2_CTU0_DESC1_UNUSED_FIFO_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_DESC2_UNUSED_FIFO_ECC_ERR_E,   PRV_CPSS_LION2_CTU0_DESC2_UNUSED_FIFO_ECC_ERR_E,
    PRV_CPSS_LION2_CTU0_DESC3_UNUSED_FIFO_ECC_ERR_E,   PRV_CPSS_LION2_CTU0_DESC3_UNUSED_FIFO_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_FCP0_PORTA_ECC_ERR_E,          PRV_CPSS_LION2_CTU1_FCP0_PORTA_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_FCP1_PORTA_ECC_ERR_E,          PRV_CPSS_LION2_CTU1_FCP1_PORTA_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_FCP2_PORTA_ECC_ERR_E,          PRV_CPSS_LION2_CTU1_FCP2_PORTA_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_FCP3_PORTA_ECC_ERR_E,          PRV_CPSS_LION2_CTU1_FCP3_PORTA_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_FCP0_PORTB_ECC_ERR_E,          PRV_CPSS_LION2_CTU1_FCP0_PORTB_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_FCP1_PORTB_ECC_ERR_E,          PRV_CPSS_LION2_CTU1_FCP1_PORTB_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_FCP2_PORTB_ECC_ERR_E,          PRV_CPSS_LION2_CTU1_FCP2_PORTB_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_FCP3_PORTB_ECC_ERR_E,          PRV_CPSS_LION2_CTU1_FCP3_PORTB_ECC_ERR_E,
    PRV_CPSS_LION2_CTU1_OCP0_START_ON_VALID_ENTRY_E,   PRV_CPSS_LION2_CTU1_OCP0_START_ON_VALID_ENTRY_E,
    PRV_CPSS_LION2_CTU1_OCP1_START_ON_VALID_ENTRY_E,   PRV_CPSS_LION2_CTU1_OCP1_START_ON_VALID_ENTRY_E,
    PRV_CPSS_LION2_CTU1_OCP2_START_ON_VALID_ENTRY_E,   PRV_CPSS_LION2_CTU1_OCP2_START_ON_VALID_ENTRY_E,
    PRV_CPSS_LION2_CTU1_OCP3_START_ON_VALID_ENTRY_E,   PRV_CPSS_LION2_CTU1_OCP3_START_ON_VALID_ENTRY_E,
    PRV_CPSS_LION2_CTU1_OCP0_END_ON_INVALID_ENTRY_E,   PRV_CPSS_LION2_CTU1_OCP0_END_ON_INVALID_ENTRY_E,
    PRV_CPSS_LION2_CTU1_OCP1_END_ON_INVALID_ENTRY_E,   PRV_CPSS_LION2_CTU1_OCP1_END_ON_INVALID_ENTRY_E,
    PRV_CPSS_LION2_CTU1_OCP2_END_ON_INVALID_ENTRY_E,   PRV_CPSS_LION2_CTU1_OCP2_END_ON_INVALID_ENTRY_E,
    PRV_CPSS_LION2_CTU1_OCP3_END_ON_INVALID_ENTRY_E,   PRV_CPSS_LION2_CTU1_OCP3_END_ON_INVALID_ENTRY_E,
    PRV_CPSS_LION2_CTU1_FIFO_OVERRUN_E,                PRV_CPSS_LION2_CTU1_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_CTU1_FCP0_SAME_ADDR_WR_A_RD_B_E ,   PRV_CPSS_LION2_CTU1_FCP0_SAME_ADDR_WR_A_RD_B_E ,
    PRV_CPSS_LION2_CTU1_FCP1_SAME_ADDR_WR_A_RD_B_E ,   PRV_CPSS_LION2_CTU1_FCP1_SAME_ADDR_WR_A_RD_B_E ,
    PRV_CPSS_LION2_CTU1_FCP2_SAME_ADDR_WR_A_RD_B_E ,   PRV_CPSS_LION2_CTU1_FCP2_SAME_ADDR_WR_A_RD_B_E ,
    PRV_CPSS_LION2_CTU1_FCP3_SAME_ADDR_WR_A_RD_B_E ,   PRV_CPSS_LION2_CTU1_FCP3_SAME_ADDR_WR_A_RD_B_E ,
    PRV_CPSS_LION2_CTU1_FCP0_SAME_ADDR_WR_B_RD_A_E ,   PRV_CPSS_LION2_CTU1_FCP0_SAME_ADDR_WR_B_RD_A_E ,
    PRV_CPSS_LION2_CTU1_FCP1_SAME_ADDR_WR_B_RD_A_E ,   PRV_CPSS_LION2_CTU1_FCP1_SAME_ADDR_WR_B_RD_A_E ,
    PRV_CPSS_LION2_CTU1_FCP2_SAME_ADDR_WR_B_RD_A_E ,   PRV_CPSS_LION2_CTU1_FCP2_SAME_ADDR_WR_B_RD_A_E ,
    PRV_CPSS_LION2_CTU1_FCP3_SAME_ADDR_WR_B_RD_A_E ,   PRV_CPSS_LION2_CTU1_FCP3_SAME_ADDR_WR_B_RD_A_E ,
    PRV_CPSS_LION2_CTU1_DESC0_UNUSED_FIFO_ECC_ERR_E ,  PRV_CPSS_LION2_CTU1_DESC0_UNUSED_FIFO_ECC_ERR_E ,
    PRV_CPSS_LION2_CTU1_DESC1_UNUSED_FIFO_ECC_ERR_E ,  PRV_CPSS_LION2_CTU1_DESC1_UNUSED_FIFO_ECC_ERR_E ,
    PRV_CPSS_LION2_CTU1_DESC2_UNUSED_FIFO_ECC_ERR_E ,  PRV_CPSS_LION2_CTU1_DESC2_UNUSED_FIFO_ECC_ERR_E ,
    PRV_CPSS_LION2_CTU1_DESC3_UNUSED_FIFO_ECC_ERR_E ,  PRV_CPSS_LION2_CTU1_DESC3_UNUSED_FIFO_ECC_ERR_E ,

    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA0_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA0_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA1_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA1_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA2_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA2_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA3_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA3_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA4_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA4_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA5_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA5_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA6_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA6_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA7_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA7_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA8_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA8_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA9_CT_PKT_FIFO_OVERRUN_E,    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA9_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA10_CT_PKT_FIFO_OVERRUN_E,   PRV_CPSS_LION2_RXDMA_FUNC_WRDMA10_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA11_CT_PKT_FIFO_OVERRUN_E,   PRV_CPSS_LION2_RXDMA_FUNC_WRDMA11_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA12_CT_PKT_FIFO_OVERRUN_E,   PRV_CPSS_LION2_RXDMA_FUNC_WRDMA12_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA13_CT_PKT_FIFO_OVERRUN_E,   PRV_CPSS_LION2_RXDMA_FUNC_WRDMA13_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA14_CT_PKT_FIFO_OVERRUN_E,   PRV_CPSS_LION2_RXDMA_FUNC_WRDMA14_CT_PKT_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_RXDMA_FUNC_WRDMA15_CT_PKT_FIFO_OVERRUN_E,   PRV_CPSS_LION2_RXDMA_FUNC_WRDMA15_CT_PKT_FIFO_OVERRUN_E,

/*
    PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_0_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_0_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_1_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_1_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_2_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_2_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_3_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_3_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_4_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_4_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_5_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_5_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_6_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_6_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_7_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK0_2_ECC_ERROR_PORT_GROUP_7_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_0_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_0_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_1_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_1_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_2_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_2_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_3_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_3_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_4_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_4_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_5_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_5_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_6_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_6_E,
    PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_7_E,  PRV_CPSS_LION2_TXDMA_GEN_BANK1_2_ECC_ERROR_PORT_GROUP_7_E,
*/
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO0_BK0_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO0_BK0_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO1_BK0_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO1_BK0_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO2_BK0_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO2_BK0_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO3_BK0_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO3_BK0_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO4_BK0_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO4_BK0_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO5_BK0_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO5_BK0_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO6_BK0_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO6_BK0_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO7_BK0_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO7_BK0_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO0_BK1_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO0_BK1_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO1_BK1_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO1_BK1_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO2_BK1_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO2_BK1_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO3_BK1_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO3_BK1_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO4_BK1_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO4_BK1_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO5_BK1_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO5_BK1_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO6_BK1_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO6_BK1_OVERRUN_INT_E,
    PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO7_BK1_OVERRUN_INT_E,     PRV_CPSS_LION2_TXDMA_ERROR_RD_FIFO7_BK1_OVERRUN_INT_E,

    PRV_CPSS_LION2_BM_0_EMPTY_CLEAR_E,                         PRV_CPSS_LION2_BM_0_EMPTY_CLEAR_E,
    PRV_CPSS_LION2_BM_0_XG_CNT_UNDERRUN_E,                     PRV_CPSS_LION2_BM_0_XG_CNT_UNDERRUN_E,
    PRV_CPSS_LION2_BM_0_XG_CNT_OVERRUN_E,                      PRV_CPSS_LION2_BM_0_XG_CNT_OVERRUN_E,
    PRV_CPSS_LION2_BM_0_MC_INC_OVERFLOW_E,                     PRV_CPSS_LION2_BM_0_MC_INC_OVERFLOW_E,
    PRV_CPSS_LION2_BM_0_MC_INC_UNDERRUN_E,                     PRV_CPSS_LION2_BM_0_MC_INC_UNDERRUN_E,
    PRV_CPSS_LION2_BM_0_CTRL_ACCESS_ERROR_E,                   PRV_CPSS_LION2_BM_0_CTRL_ACCESS_ERROR_E,
    PRV_CPSS_LION2_BM_0_LL_P1_ACCESS_E,                        PRV_CPSS_LION2_BM_0_LL_P1_ACCESS_E,
    PRV_CPSS_LION2_BM_0_LL_P2_ACCESS_E,                        PRV_CPSS_LION2_BM_0_LL_P2_ACCESS_E,

    PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_WRONG_ADDR_E,                 PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_WRONG_ADDR_E,
    PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_DESC_RECONST_FIFO_FULL_E,     PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_DESC_RECONST_FIFO_FULL_E,
    PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_DESC_RECONST_FIFO_OVERRUN_E,  PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_DESC_RECONST_FIFO_OVERRUN_E,

    PRV_CPSS_LION2_TXQ_QUEUE_GEN_TAIL_DROP_LATENCY_FIFO_OVERFLOW_E, PRV_CPSS_LION2_TXQ_QUEUE_GEN_TAIL_DROP_LATENCY_FIFO_OVERFLOW_E,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_LFIFO_OVERRUN_E,                   PRV_CPSS_LION2_TXQ_QUEUE_GEN_LFIFO_OVERRUN_E,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_LIMIT_ID_FIFO_OVERRUN_E,           PRV_CPSS_LION2_TXQ_QUEUE_GEN_LIMIT_ID_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_0_OVERRUN_E,      PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_0_OVERRUN_E,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_1_OVERRUN_E,      PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_1_OVERRUN_E,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_2_OVERRUN_E,      PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_2_OVERRUN_E,
    PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_3_OVERRUN_E,      PRV_CPSS_LION2_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_3_OVERRUN_E,


    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_0_OVERFLOW_E,  PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_0_OVERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_0_UNDERFLOW_E, PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_0_UNDERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_1_OVERFLOW_E,  PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_1_OVERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_1_UNDERFLOW_E, PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_1_UNDERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_2_OVERFLOW_E,  PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_2_OVERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_2_UNDERFLOW_E, PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_2_UNDERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_3_OVERFLOW_E,  PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_3_OVERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_3_UNDERFLOW_E, PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_3_UNDERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_4_OVERFLOW_E,  PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_4_OVERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_4_UNDERFLOW_E, PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_4_UNDERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_5_OVERFLOW_E,  PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_5_OVERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_5_UNDERFLOW_E, PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_5_UNDERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_6_OVERFLOW_E,  PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_6_OVERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_6_UNDERFLOW_E, PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_6_UNDERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_7_OVERFLOW_E,  PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_7_OVERFLOW_E,
    PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_7_UNDERFLOW_E, PRV_CPSS_LION2_CPFC_CC_PORT_GROUP_7_UNDERFLOW_E,

    PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_0_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_0_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_1_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_1_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_2_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_2_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_3_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_3_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_4_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_4_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_5_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_5_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_6_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_6_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_7_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_7_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_0_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_0_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_1_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_1_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_2_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_2_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_3_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_3_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_4_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_4_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_5_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_5_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_6_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_6_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_7_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_1_GLOBAL_UNDERFLOW_TC_7_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_0_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_0_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_1_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_1_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_2_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_2_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_3_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_3_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_4_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_4_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_5_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_5_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_6_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_6_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_7_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_2_GLOBAL_UNDERFLOW_TC_7_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_0_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_0_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_1_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_1_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_2_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_2_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_3_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_3_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_4_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_4_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_5_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_5_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_6_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_6_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_7_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_3_GLOBAL_UNDERFLOW_TC_7_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_0_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_0_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_1_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_1_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_2_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_2_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_3_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_3_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_4_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_4_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_5_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_5_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_6_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_6_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_7_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_4_GLOBAL_UNDERFLOW_TC_7_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_0_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_0_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_1_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_1_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_2_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_2_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_3_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_3_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_4_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_4_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_5_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_5_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_6_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_6_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_7_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_5_GLOBAL_UNDERFLOW_TC_7_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_0_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_0_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_1_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_1_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_2_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_2_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_3_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_3_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_4_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_4_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_5_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_5_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_6_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_6_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_7_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_6_GLOBAL_UNDERFLOW_TC_7_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_0_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_0_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_1_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_1_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_2_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_2_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_3_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_3_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_4_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_4_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_5_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_5_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_6_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_6_E,
    PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_7_E,  PRV_CPSS_LION2_CPFC_PORT_GROUP_7_GLOBAL_UNDERFLOW_TC_7_E,

   /* PRV_CPSS_LION2_TXQ_LL_GEN_ID_FIFO_OVERRUN_E,               PRV_CPSS_LION2_TXQ_LL_GEN_ID_FIFO_OVERRUN_E,   false alarm event in FWS tests  */
    PRV_CPSS_LION2_TXQ_LL_GEN_LFIFO_OVERRUN_E,                 PRV_CPSS_LION2_TXQ_LL_GEN_LFIFO_OVERRUN_E,
    PRV_CPSS_LION2_TXQ_LL_GEN_LIMIT_ID_FIFO_OVERRUN_E,         PRV_CPSS_LION2_TXQ_LL_GEN_LIMIT_ID_FIFO_OVERRUN_E,


    PRV_CPSS_LION2_HGL_PORT0_HYPERG_LINK_MAC_TX_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT0_HYPERG_LINK_MAC_TX_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT2_HYPERG_LINK_MAC_TX_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT2_HYPERG_LINK_MAC_TX_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT4_HYPERG_LINK_MAC_TX_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT4_HYPERG_LINK_MAC_TX_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT6_HYPERG_LINK_MAC_TX_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT6_HYPERG_LINK_MAC_TX_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT8_HYPERG_LINK_MAC_TX_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT8_HYPERG_LINK_MAC_TX_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT10_HYPERG_LINK_MAC_TX_OVERRUN_E,  PRV_CPSS_LION2_HGL_PORT10_HYPERG_LINK_MAC_TX_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT12_HYPERG_LINK_MAC_TX_OVERRUN_E,  PRV_CPSS_LION2_HGL_PORT12_HYPERG_LINK_MAC_TX_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT14_HYPERG_LINK_MAC_TX_OVERRUN_E,  PRV_CPSS_LION2_HGL_PORT14_HYPERG_LINK_MAC_TX_OVERRUN_E,

    PRV_CPSS_LION2_HGL_PORT0_TX_DESCRIPTOR_FIFO_UNDERRUN_E,  PRV_CPSS_LION2_HGL_PORT0_TX_DESCRIPTOR_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT2_TX_DESCRIPTOR_FIFO_UNDERRUN_E,  PRV_CPSS_LION2_HGL_PORT2_TX_DESCRIPTOR_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT4_TX_DESCRIPTOR_FIFO_UNDERRUN_E,  PRV_CPSS_LION2_HGL_PORT4_TX_DESCRIPTOR_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT6_TX_DESCRIPTOR_FIFO_UNDERRUN_E,  PRV_CPSS_LION2_HGL_PORT6_TX_DESCRIPTOR_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT8_TX_DESCRIPTOR_FIFO_UNDERRUN_E,  PRV_CPSS_LION2_HGL_PORT8_TX_DESCRIPTOR_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT10_TX_DESCRIPTOR_FIFO_UNDERRUN_E, PRV_CPSS_LION2_HGL_PORT10_TX_DESCRIPTOR_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT12_TX_DESCRIPTOR_FIFO_UNDERRUN_E, PRV_CPSS_LION2_HGL_PORT12_TX_DESCRIPTOR_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT14_TX_DESCRIPTOR_FIFO_UNDERRUN_E, PRV_CPSS_LION2_HGL_PORT14_TX_DESCRIPTOR_FIFO_UNDERRUN_E,

    PRV_CPSS_LION2_HGL_PORT0_TX_DESCRIPTOR_FIFO_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT0_TX_DESCRIPTOR_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT2_TX_DESCRIPTOR_FIFO_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT2_TX_DESCRIPTOR_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT4_TX_DESCRIPTOR_FIFO_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT4_TX_DESCRIPTOR_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT6_TX_DESCRIPTOR_FIFO_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT6_TX_DESCRIPTOR_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT8_TX_DESCRIPTOR_FIFO_OVERRUN_E,   PRV_CPSS_LION2_HGL_PORT8_TX_DESCRIPTOR_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT10_TX_DESCRIPTOR_FIFO_OVERRUN_E,  PRV_CPSS_LION2_HGL_PORT10_TX_DESCRIPTOR_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT12_TX_DESCRIPTOR_FIFO_OVERRUN_E,  PRV_CPSS_LION2_HGL_PORT12_TX_DESCRIPTOR_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT14_TX_DESCRIPTOR_FIFO_OVERRUN_E,  PRV_CPSS_LION2_HGL_PORT14_TX_DESCRIPTOR_FIFO_OVERRUN_E,

    PRV_CPSS_LION2_HGL_PORT0_TX_DSA_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_HGL_PORT0_TX_DSA_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT2_TX_DSA_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_HGL_PORT2_TX_DSA_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT4_TX_DSA_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_HGL_PORT4_TX_DSA_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT6_TX_DSA_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_HGL_PORT6_TX_DSA_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT8_TX_DSA_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_HGL_PORT8_TX_DSA_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT10_TX_DSA_FIFO_UNDERRUN_E,   PRV_CPSS_LION2_HGL_PORT10_TX_DSA_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT12_TX_DSA_FIFO_UNDERRUN_E,   PRV_CPSS_LION2_HGL_PORT12_TX_DSA_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT14_TX_DSA_FIFO_UNDERRUN_E,   PRV_CPSS_LION2_HGL_PORT14_TX_DSA_FIFO_UNDERRUN_E,

    PRV_CPSS_LION2_HGL_PORT0_TX_DSA_FIFO_OVERRUN_E,     PRV_CPSS_LION2_HGL_PORT0_TX_DSA_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT2_TX_DSA_FIFO_OVERRUN_E,     PRV_CPSS_LION2_HGL_PORT2_TX_DSA_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT4_TX_DSA_FIFO_OVERRUN_E,     PRV_CPSS_LION2_HGL_PORT4_TX_DSA_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT6_TX_DSA_FIFO_OVERRUN_E,     PRV_CPSS_LION2_HGL_PORT6_TX_DSA_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT8_TX_DSA_FIFO_OVERRUN_E,     PRV_CPSS_LION2_HGL_PORT8_TX_DSA_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT10_TX_DSA_FIFO_OVERRUN_E,    PRV_CPSS_LION2_HGL_PORT10_TX_DSA_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT12_TX_DSA_FIFO_OVERRUN_E,    PRV_CPSS_LION2_HGL_PORT12_TX_DSA_FIFO_OVERRUN_E,
    PRV_CPSS_LION2_HGL_PORT14_TX_DSA_FIFO_OVERRUN_E,    PRV_CPSS_LION2_HGL_PORT14_TX_DSA_FIFO_OVERRUN_E,

    PRV_CPSS_LION2_HGL_PORT0_TX_PAYLOAD_FIFO_UNDERRUN_E,     PRV_CPSS_LION2_HGL_PORT0_TX_PAYLOAD_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT2_TX_PAYLOAD_FIFO_UNDERRUN_E,     PRV_CPSS_LION2_HGL_PORT2_TX_PAYLOAD_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT4_TX_PAYLOAD_FIFO_UNDERRUN_E,     PRV_CPSS_LION2_HGL_PORT4_TX_PAYLOAD_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT6_TX_PAYLOAD_FIFO_UNDERRUN_E,     PRV_CPSS_LION2_HGL_PORT6_TX_PAYLOAD_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT8_TX_PAYLOAD_FIFO_UNDERRUN_E,     PRV_CPSS_LION2_HGL_PORT8_TX_PAYLOAD_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT10_TX_PAYLOAD_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_HGL_PORT10_TX_PAYLOAD_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT12_TX_PAYLOAD_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_HGL_PORT12_TX_PAYLOAD_FIFO_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT14_TX_PAYLOAD_FIFO_UNDERRUN_E,    PRV_CPSS_LION2_HGL_PORT14_TX_PAYLOAD_FIFO_UNDERRUN_E,

    PRV_CPSS_LION2_HGL_PORT0_TX_CONVERTER_CELL_UNDERRUN_E,   PRV_CPSS_LION2_HGL_PORT0_TX_CONVERTER_CELL_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT2_TX_CONVERTER_CELL_UNDERRUN_E,   PRV_CPSS_LION2_HGL_PORT2_TX_CONVERTER_CELL_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT4_TX_CONVERTER_CELL_UNDERRUN_E,   PRV_CPSS_LION2_HGL_PORT4_TX_CONVERTER_CELL_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT6_TX_CONVERTER_CELL_UNDERRUN_E,   PRV_CPSS_LION2_HGL_PORT6_TX_CONVERTER_CELL_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT8_TX_CONVERTER_CELL_UNDERRUN_E,   PRV_CPSS_LION2_HGL_PORT8_TX_CONVERTER_CELL_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT10_TX_CONVERTER_CELL_UNDERRUN_E,  PRV_CPSS_LION2_HGL_PORT10_TX_CONVERTER_CELL_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT12_TX_CONVERTER_CELL_UNDERRUN_E,  PRV_CPSS_LION2_HGL_PORT12_TX_CONVERTER_CELL_UNDERRUN_E,
    PRV_CPSS_LION2_HGL_PORT14_TX_CONVERTER_CELL_UNDERRUN_E,  PRV_CPSS_LION2_HGL_PORT14_TX_CONVERTER_CELL_UNDERRUN_E,

 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_DATA_INTEGRITY_ERROR_E,
    PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_ONE_ERROR_E,             PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_ONE_ERROR_E,
    PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E,    PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E,
    PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_0_PARITY_ERROR_E,            PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_0_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_0_PARITY_ERROR_E,            PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_0_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_1_PARITY_ERROR_E,            PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_1_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_1_PARITY_ERROR_E,            PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_1_PARITY_ERROR_E,
/*    PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_3_PARITY_ERROR_E,            PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_3_PARITY_ERROR_E, false alarm for IPMC routed packets */
    PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_3_PARITY_ERROR_E,            PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_3_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_SHT_GEN_EGR_VLAN_TBL_PARITY_ERROR_E,              PRV_CPSS_LION2_TXQ_SHT_GEN_EGR_VLAN_TBL_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_SHT_GEN_ING_VLAN_TBL_PARITY_ERROR_E,              PRV_CPSS_LION2_TXQ_SHT_GEN_ING_VLAN_TBL_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_SHT_GEN_L2_PORT_ISOLATION_TBL_PARITY_ERROR_E,     PRV_CPSS_LION2_TXQ_SHT_GEN_L2_PORT_ISOLATION_TBL_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_SHT_GEN_L3_PORT_ISOLATION_TBL_PARITY_ERROR_E,     PRV_CPSS_LION2_TXQ_SHT_GEN_L3_PORT_ISOLATION_TBL_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_SHT_GEN_VIDX_TBL_PARITY_ERROR_E,                  PRV_CPSS_LION2_TXQ_SHT_GEN_VIDX_TBL_PARITY_ERROR_E,
    PRV_CPSS_LION2_IP_ROUTER_NHE_DIP_DATA_ERROR_E,                       PRV_CPSS_LION2_IP_ROUTER_NHE_DIP_DATA_ERROR_E,
    PRV_CPSS_LION2_IP_ROUTER_NHE_SIP_DATA_ERROR_E,                       PRV_CPSS_LION2_IP_ROUTER_NHE_SIP_DATA_ERROR_E,
    PRV_CPSS_LION2_IP_ROUTER_STG2_DATA_FIFO_ECC_ERROR_E,                 PRV_CPSS_LION2_IP_ROUTER_STG2_DATA_FIFO_ECC_ERROR_E,
    PRV_CPSS_LION2_IP_ROUTER_STG4_DATA_FIFO_ECC_ERROR_E,                 PRV_CPSS_LION2_IP_ROUTER_STG4_DATA_FIFO_ECC_ERROR_E,
    PRV_CPSS_LION2_IP_ROUTER_UNUSED_DATA_FIFO_ECC_ERROR_E,               PRV_CPSS_LION2_IP_ROUTER_UNUSED_DATA_FIFO_ECC_ERROR_E,
    PRV_CPSS_LION2_MPPM_0_BK0_ECC_1_ERROR_E,                             PRV_CPSS_LION2_MPPM_0_BK0_ECC_1_ERROR_E,
    PRV_CPSS_LION2_MPPM_0_BK1_ECC_1_ERROR_E,                             PRV_CPSS_LION2_MPPM_0_BK1_ECC_1_ERROR_E,
    PRV_CPSS_LION2_MPPM_1_BK0_ECC_1_ERROR_E,                             PRV_CPSS_LION2_MPPM_1_BK0_ECC_1_ERROR_E,
    PRV_CPSS_LION2_MPPM_1_BK1_ECC_1_ERROR_E,                             PRV_CPSS_LION2_MPPM_1_BK1_ECC_1_ERROR_E,
    PRV_CPSS_LION2_MPPM_0_BK0_ECC_2_OR_MORE_ERROR_E,                     PRV_CPSS_LION2_MPPM_0_BK0_ECC_2_OR_MORE_ERROR_E,
    PRV_CPSS_LION2_MPPM_0_BK1_ECC_2_OR_MORE_ERROR_E,                     PRV_CPSS_LION2_MPPM_0_BK1_ECC_2_OR_MORE_ERROR_E,
    PRV_CPSS_LION2_MPPM_1_BK0_ECC_2_OR_MORE_ERROR_E,                     PRV_CPSS_LION2_MPPM_1_BK0_ECC_2_OR_MORE_ERROR_E,
    PRV_CPSS_LION2_MPPM_1_BK1_ECC_2_OR_MORE_ERROR_E,                     PRV_CPSS_LION2_MPPM_1_BK1_ECC_2_OR_MORE_ERROR_E,
/*    PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_TOKEN_BUCKET_PRIO_PARITY_ERROR_E, PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_TOKEN_BUCKET_PRIO_PARITY_ERROR_E, false alarm event for shaper configuration */
    PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_STATE_VARIABLE_PARITY_ERROR_E,    PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_STATE_VARIABLE_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_LL_GEN_FBUF_ECC_ERROR_E,                          PRV_CPSS_LION2_TXQ_LL_GEN_FBUF_ECC_ERROR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_QCN_CN_BUFFER_FIFO_PARITY_ERROR_E,          PRV_CPSS_LION2_TXQ_QUEUE_QCN_CN_BUFFER_FIFO_PARITY_ERROR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP0_COUNTERS_ERR_E,      PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP0_COUNTERS_ERR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP1_COUNTERS_ERR_E,      PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP1_COUNTERS_ERR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP2_COUNTERS_ERR_E,      PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP2_COUNTERS_ERR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP3_COUNTERS_ERR_E,      PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP3_COUNTERS_ERR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP4_COUNTERS_ERR_E,      PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP4_COUNTERS_ERR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP5_COUNTERS_ERR_E,      PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP5_COUNTERS_ERR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP6_COUNTERS_ERR_E,      PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP6_COUNTERS_ERR_E,
    PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP7_COUNTERS_ERR_E,      PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP7_COUNTERS_ERR_E,
    PRV_CPSS_LION2_BMA_CORE0_MCCNT_PARITY_ERROR_E,                       PRV_CPSS_LION2_BMA_CORE0_MCCNT_PARITY_ERROR_E,
    PRV_CPSS_LION2_BMA_CORE1_MCCNT_PARITY_ERROR_E,                       PRV_CPSS_LION2_BMA_CORE1_MCCNT_PARITY_ERROR_E,
    PRV_CPSS_LION2_BMA_CORE2_MCCNT_PARITY_ERROR_E,                       PRV_CPSS_LION2_BMA_CORE2_MCCNT_PARITY_ERROR_E,
    PRV_CPSS_LION2_BMA_CORE3_MCCNT_PARITY_ERROR_E,                       PRV_CPSS_LION2_BMA_CORE3_MCCNT_PARITY_ERROR_E,
    PRV_CPSS_LION2_IPLR0_DATA_ERROR_E,                                   PRV_CPSS_LION2_IPLR0_DATA_ERROR_E,
    PRV_CPSS_LION2_IPLR1_DATA_ERROR_E,                                   PRV_CPSS_LION2_IPLR1_DATA_ERROR_E,
    PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E,                  PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E,
    PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E,                  PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E,

 MARK_END_OF_UNI_EV_CNS

};
/* Interrupt cause to unified event map for Lion2 with extended data size */
static GT_U32 lion2UniEvMapTableWithExtDataSize = (sizeof(lion2UniEvMapTableWithExtData)/(sizeof(GT_U16)));


/* Interrupt cause to unified event map for Lion2
 This Table is for unified event without extended data
*/
static PRV_CPSS_DRV_EVENT_INFO_TYPE lion2UniEvMapTable[] =
{
 {CPSS_PP_MISC_TWSI_TIME_OUT_E,          PRV_CPSS_LION2_MISC_TWSI_TIME_OUT_E},
 {CPSS_PP_MISC_TWSI_STATUS_E,            PRV_CPSS_LION2_MISC_TWSI_STATUS_E},
 {CPSS_PP_MISC_ILLEGAL_ADDR_E,           PRV_CPSS_LION2_MISC_ILLEGAL_ADDR_E},
 {CPSS_PP_MISC_CPU_PORT_RX_OVERRUN_E,    PRV_CPSS_LION2_CPU_PORT_SUM_RX_FIFO_OVERRUN_E},
 {CPSS_PP_BM_AGED_PACKET_E,              PRV_CPSS_LION2_BM_0_AGED_PACKET_E},
 {CPSS_PP_BM_MAX_BUFF_REACHED_E,         PRV_CPSS_LION2_BM_0_MAX_BUFF_REACHED_E},
 {CPSS_PP_BM_WRONG_SRC_PORT_E,           PRV_CPSS_LION2_BM_0_WRONG_SRC_PORT_E},
 {CPSS_PP_TQ_SNIFF_DESC_DROP_E,          PRV_CPSS_LION2_TXQ_DQ_GEN_EGRESS_MIRROR_DESC_DROP_E},
 {CPSS_PP_EB_NA_FIFO_FULL_E,             PRV_CPSS_LION2_BRIDGE_UPDATE_FIFO_FULL_E},
 {CPSS_PP_EB_SECURITY_BREACH_UPDATE_E,   PRV_CPSS_LION2_BRIDGE_UPDATE_SECURITY_BREACH_E},
 {CPSS_PP_MAC_NUM_OF_HOP_EXP_E,          PRV_CPSS_LION2_FDB_NUM_OF_HOP_EX_P_E},
 {CPSS_PP_MAC_NA_LEARNED_E,              PRV_CPSS_LION2_FDB_NA_LEARNT_E},
 {CPSS_PP_MAC_NA_NOT_LEARNED_E,          PRV_CPSS_LION2_FDB_NA_NOT_LEARNT_E},
 {CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E,   PRV_CPSS_LION2_FDB_AGE_VIA_TRIGGER_ENDED_E},
 {CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E,    PRV_CPSS_LION2_FDB_AU_PROC_COMPLETED_E},
 {CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E,    PRV_CPSS_LION2_FDB_AU_MSG_TOCPU_READY_E},
 {CPSS_PP_MAC_NA_SELF_LEARNED_E,         PRV_CPSS_LION2_FDB_NA_SELF_LEARNT_E},
 {CPSS_PP_MAC_NA_FROM_CPU_LEARNED_E,     PRV_CPSS_LION2_FDB_N_AFROM_CPU_LEARNED_E},
 {CPSS_PP_MAC_NA_FROM_CPU_DROPPED_E,     PRV_CPSS_LION2_FDB_N_AFROM_CPU_DROPPED_E},
 {CPSS_PP_MAC_AGED_OUT_E,                PRV_CPSS_LION2_FDB_AGED_OUT_E},
 {CPSS_PP_MAC_FIFO_2_CPU_EXCEEDED_E,     PRV_CPSS_LION2_FDB_AU_FIFO_TO_CPU_IS_FULL_E},
 {CPSS_PP_EB_AUQ_PENDING_E,              PRV_CPSS_LION2_MISC_AUQ_PENDING_E},
 {CPSS_PP_EB_AUQ_FULL_E,                 PRV_CPSS_LION2_MISC_AU_QUEUE_FULL_E},
 {CPSS_PP_EB_AUQ_OVER_E,                 PRV_CPSS_LION2_MISC_AUQ_OVERRUN_E},
 {CPSS_PP_EB_AUQ_ALMOST_FULL_E,          PRV_CPSS_LION2_MISC_AUQ_ALMOST_FULL_E},
 {CPSS_PP_EB_FUQ_PENDING_E,              PRV_CPSS_LION2_MISC_FUQ_PENDING_E},
 {CPSS_PP_EB_FUQ_FULL_E,                 PRV_CPSS_LION2_MISC_FU_QUEUE_FULL_E},
 {CPSS_PP_MISC_GENXS_READ_DMA_DONE_E,    PRV_CPSS_LION2_MISC_GENXS_READ_DMA_DONE_E},
 {CPSS_PP_MISC_PEX_ADDR_UNMAPPED_E,      PRV_CPSS_LION2_MISC_PEX_ADDR_UNMAPPED_E},
 {CPSS_PP_EB_MG_ADDR_OUT_OF_RANGE_E,     PRV_CPSS_LION2_BRIDGE_WRONG_ADDR_E},
 {CPSS_PP_PEX_DL_DOWN_TX_ACC_ERR_E,      PRV_CPSS_LION2_PEX_DL_DWN_TX_ACC_ERR_E},
 {CPSS_PP_PEX_MASTER_DISABLED_E,         PRV_CPSS_LION2_PEX_MDIS_E},
 {CPSS_PP_PEX_ERROR_WR_TO_REG_E,         PRV_CPSS_LION2_PEX_ERR_WR_TO_REG_E},
 {CPSS_PP_PEX_HIT_DEFAULT_WIN_ERR_E,     PRV_CPSS_LION2_PEX_HIT_DFLT_WIN_ERR_E},
 {CPSS_PP_PEX_COR_ERROR_DET_E,           PRV_CPSS_LION2_PEX_COR_ERR_DET_E},
 {CPSS_PP_PEX_NON_FATAL_ERROR_DET_E,     PRV_CPSS_LION2_PEX_NF_ERR_DET_E},
 {CPSS_PP_PEX_FATAL_ERROR_DET_E,         PRV_CPSS_LION2_PEX_F_ERR_DET_E},
 {CPSS_PP_PEX_DSTATE_CHANGED_E,          PRV_CPSS_LION2_PEX_DSTATE_CHANGED_E},
 {CPSS_PP_PEX_BIST_E,                    PRV_CPSS_LION2_PEX_BIST_E},
 {CPSS_PP_PEX_RCV_ERROR_FATAL_E,         PRV_CPSS_LION2_PEX_RCV_ERR_FATAL_E},
 {CPSS_PP_PEX_RCV_ERROR_NON_FATAL_E,     PRV_CPSS_LION2_PEX_RCV_ERR_NON_FATAL_E},
 {CPSS_PP_PEX_RCV_ERROR_COR_E,           PRV_CPSS_LION2_PEX_RCV_ERR_COR_E},
 {CPSS_PP_PEX_RCV_CRS_E,                 PRV_CPSS_LION2_PEX_RCV_CRS_E},
 {CPSS_PP_PEX_PEX_SLAVE_HOT_RESET_E,     PRV_CPSS_LION2_PEX_PEX_SLV_HOT_RESET_E},
 {CPSS_PP_PEX_PEX_SLAVE_DISABLE_LINK_E,  PRV_CPSS_LION2_PEX_PEX_SLV_DIS_LINK_E},
 {CPSS_PP_PEX_PEX_SLAVE_LOOPBACK_E,      PRV_CPSS_LION2_PEX_PEX_SLV_LB_E},
 {CPSS_PP_PEX_PEX_LINK_FAIL_E,           PRV_CPSS_LION2_PEX_PEX_LINK_FAIL_E},
 {CPSS_PP_PEX_RCV_A_E,                   PRV_CPSS_LION2_PEX_RCV_A_E},
 {CPSS_PP_PEX_RCV_B_E,                   PRV_CPSS_LION2_PEX_RCV_B_E},
 {CPSS_PP_PEX_RCV_C_E,                   PRV_CPSS_LION2_PEX_RCV_C_E},
 {CPSS_PP_PEX_RCV_D_E,                   PRV_CPSS_LION2_PEX_RCV_D_E},
 {CPSS_PP_CNC_DUMP_FINISHED_E,           PRV_CPSS_LION2_CNC_DUMP_FINISHED_E},
 {CPSS_PP_TTI_CPU_ADDRESS_OUT_OF_RANGE_E,PRV_CPSS_LION2_TTI_ENG_WRONG_ADDR_E},
 {CPSS_PP_TTI_ACCESS_DATA_ERROR_E,       PRV_CPSS_LION2_TTI_ENG_ACCESS_DATA_ERROR_E},
 {CPSS_PP_PCL_MG_ADDR_OUT_OF_RANGE_E,    PRV_CPSS_LION2_POLICY_ENGINE_WRONG_ADDR_E},
 {CPSS_PP_PORT_802_3_AP_E,               PRV_CPSS_LION2_MISC_Z80_E},
 {CPSS_PP_DATA_INTEGRITY_ERROR_E,        PRV_CPSS_LION2_GLOBAL_DFX_SUM_E},
 {CPSS_PP_MAC_BANK_LEARN_COUNTERS_OVERFLOW_E, PRV_CPSS_LION3_FDB_BLC_OVERFLOW_E}/* new in Lion3; Bobcat2_Gm */
};

/* Interrupt cause to unified event map for Lion2 without extended data size */
static GT_U32 lion2UniEvMapTableSize = (sizeof(lion2UniEvMapTable)/(sizeof(PRV_CPSS_DRV_EVENT_INFO_TYPE)));


/*******************************************************************************
* prvCpssDrvPpIntDefDxChLion2Init
*
* DESCRIPTION:
*      set info needed by chIntCauseToUniEvConvertDedicatedTables(...) for the
*      Lion2 devices.
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
static void setLion2DedicatedEventsConvertInfo(IN GT_U8   devNum)
{
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataArr   = lion2UniEvMapTableWithExtData;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataSize  = lion2UniEvMapTableWithExtDataSize;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableArr              = lion2UniEvMapTable;
    prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableSize             = lion2UniEvMapTableSize;
    prvCpssDrvPpConfig[devNum]->intCtrl.numReservedPorts              = 16;

    return;
}

/*******************************************************************************
* prvCpssDrvPpIntDefDxChLion2Init
*
* DESCRIPTION:
*       Interrupts initialization for the Lion 2 devices.
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
void prvCpssDrvPpIntDefDxChLion2Init(IN GT_U8   devNum)
{
    GT_U32  portGroupId;/*port group Id*/
    GT_U32  ii;
    GT_STATUS   rc;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */
    GT_U32  dummyMaskRegAddr = 0x000000F4;/*user defined register*/
    GT_U32  causeRegAddr;/*cause reg addr */
    GT_U32  unitBaseAddr;/*base address of the relevant unit */

    /*set info needed by chIntCauseToUniEvConvertDedicatedTables(...) */
    setLion2DedicatedEventsConvertInfo(devNum);

    if(prvCpssDrvPpConfig[devNum]->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) CPSS_TBD_BOOKMARK /* need proper tree for Lion3 */
    {
        static GT_BOOL initDone = GT_FALSE; /* "already init" flag for lion2IntrScanArrSinglePortGroup */

        /* init static lion2IntrScanArrSinglePortGroup only once */
        if (initDone == GT_FALSE)
        {
        /* update the DB */
        for(ii = 0 ; ii < LION2_NUM_ELEMENTS_IN_SCAN_TREE_CNS; ii++)
        {
            causeRegAddr = lion2IntrScanArrSinglePortGroup[ii].causeRegAddr;

            if((causeRegAddr == 0x010001C0)||/* tti_int_sum */
               (causeRegAddr == 0x01000004)||/* TTI Engine Interrupt */
               (causeRegAddr == 0x01002028))/* TTI GTS Interrupt */
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    regsAddrPtr->TTI.TTIEngine.TTIEngineInterruptCause = 0x00000004;
                    regsAddrPtr->TTI.TTIEngine.TTIEngineInterruptMask = 0x00000008;
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_TTI_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + (causeRegAddr - 0x01000000);
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + (causeRegAddr - 0x01000000 + 4);
            }
            else
            if(causeRegAddr == 0x02040130) /*L2I*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    L2I.bridgeEngineInterrupts.bridgeInterruptCause;
                    L2I.bridgeEngineInterrupts.bridgeInterruptMask;
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_L2I_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00002100;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x00002104;
            }
            else
            if(causeRegAddr == 0x06000018) /*FDB*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    FDB.FDBCore.FDBInterrupt.FDBInterruptCauseReg;
                    FDB.FDBCore.FDBInterrupt.FDBInterruptMaskReg;
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_FDB_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x000001B0;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x000001B4;
            }
            else
            if(causeRegAddr == 0x0B000058) /* EQ*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    EQ.preEgrInterrupt.preEgrInterruptSummary = 0x00000058;
                    EQ.preEgrInterrupt.preEgrInterruptSummaryMask = 0x0000005c;
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_EQ_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00000058;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x0000005c;
            }
            else
            if(causeRegAddr == 0x0B000020) /*EQ - Ingress STC*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    EQ.ingrSTCInterrupt.ingrSTCInterruptCause[0];
                    EQ.ingrSTCInterrupt.ingrSTCInterruptMask[0];
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_EQ_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00000620;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x00000624;
            }
            else
            if(causeRegAddr == 0x0B000060) /*EQ -  STC Rate Limiters */
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    EQ.SCTRateLimitersInterrupt.CPUCodeRateLimitersInterruptCause[0];
                    EQ.SCTRateLimitersInterrupt.CPUCodeRateLimitersInterruptMask[0];
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_EQ_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00010020;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x00010024;
            }
            else
            if(causeRegAddr == LION2_TXQ_EGR0_ADDR(0xA0) ||
               causeRegAddr == LION2_TXQ_EGR1_ADDR(0xA0) )/*EGF_EFT*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    EGF_eft.global.EFTInterruptsCause = 0x000010a0;
                    EGF_eft.global.EFTInterruptsMask = 0x000010b0;
                #endif /*0*/

                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_EGF_EFT_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x000010a0;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x000010b0;
            }
            else
            if(causeRegAddr == LION2_TXQ_DIST_ADDR(0x000008)) /*EGF_QAG*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    EGF_qag.global.QAGInterruptsCause = 0x00f00010;
                    EGF_qag.global.QAGInterruptsMask = 0x00f00020;
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_EGF_QAG_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00f00010;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x00f00020;
            }
            else
            if (causeRegAddr == 0x118F0120) /*EGF_SHT*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    EGF_sht.global.SHTInterruptsCause = 0x00f00010;
                    EGF_sht.global.SHTInterruptsMask = 0x00f00020;
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_EGF_SHT_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x06020010;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x06020020;
            }
            else
            if (causeRegAddr == 0x0E8003A8) /*HA ha_int_sum*/
            {
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_HA_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00000150;/*dummy register*/
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x00000154;/*dummy register*/
            }
            else
            if (causeRegAddr == 0x0E8003A0) /*HA Summary*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    HA.HAInterruptCause = 0x00000300;
                    HA.HAInterruptMask = 0x00000304;
                #endif /*0*/
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_HA_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00000300;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x00000304;
            }
            else
            if (causeRegAddr == 0x0E800828) /*HA Summary*/
            {
                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_ETS_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00000028;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x0000002C;
            }
            else
            if (causeRegAddr == 0x0B800004) /*PCL Summary*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    PCL.PCLUnitInterruptCause = 0x00000004;
                    PCL.PCLUnitInterruptMask = 0x00000008;
                #endif /*0*/

                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_PCL_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00000004;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x00000008;
            }
            else
            if (causeRegAddr == 0x0F000070) /*RXDMA Summary*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    rxDMA.interrupts.rxDMAInterruptSummaryCause = 0x00001048;
                    rxDMA.interrupts.rxDMAInterruptSummaryMask = 0x0000104C;
                #endif /*0*/

                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_RXDMA_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00001048;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x0000104C;
            }
            else
            if (causeRegAddr == 0x0F000078) /*RXDMA Functional*/
            {
                #if 0 /* allow the editor tagging to see those 2 registers */
                    rxDMA.interrupts.rxDMAInterruptSummary0Cause = 0x00001028;
                    rxDMA.interrupts.rxDMAInterruptSummary0Mask = 0x0000102C;
                #endif /*0*/

                unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_RXDMA_E,NULL);
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = unitBaseAddr + 0x00001028;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = unitBaseAddr + 0x0000102C;
            }
            else/*legacy TCC_PCL and TCC_IPVX units */
            if (causeRegAddr == 0x0D0001BC ||
                causeRegAddr == 0x0D0001A4 ||
                causeRegAddr == 0x0D800078 ||
                causeRegAddr == 0x0D800060 )
            {
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = dummyMaskRegAddr;
            }
            else/*legacy CNC units */
            if (causeRegAddr == 0x08000100 ||
                causeRegAddr == 0x08000108 ||
                causeRegAddr == 0x08000200)
            {
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = dummyMaskRegAddr;
            }
            else/*legacy TXQ DQ units*/
            if (causeRegAddr == 0x11000100 ||
                causeRegAddr == 0x11000108 ||
                causeRegAddr == 0x11000110 ||
                causeRegAddr == 0x11000118 ||
                causeRegAddr == 0x11000120)
            {
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = dummyMaskRegAddr;
            }
            else/*legacy TXQ LL units*/
            if (causeRegAddr == 0x108A0008)
            {
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = dummyMaskRegAddr;
            }
            else/*legacy TXQ QUEUE units*/
            if (causeRegAddr == 0x0A090000 ||
                causeRegAddr == 0x0A090008 ||
                causeRegAddr == 0x0A090040 ||
                causeRegAddr == 0x0A090048 ||
                causeRegAddr == 0x0A090050 ||
                causeRegAddr == 0x0A090060 ||
                causeRegAddr == 0x0A090068 ||
                causeRegAddr == 0x0A090070 ||
                causeRegAddr == 0x0A090180)
            {
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = dummyMaskRegAddr;
            }
            else/*legacy CPFC unit*/
            if (causeRegAddr == 0x13800300 ||
                causeRegAddr == 0x138003C0 ||
                causeRegAddr == 0x13800320 ||
                causeRegAddr == 0x13800330 ||
                causeRegAddr == 0x13800340 ||
                causeRegAddr == 0x13800350 ||
                causeRegAddr == 0x13800360 ||
                causeRegAddr == 0x13800370 ||
                causeRegAddr == 0x13800380 ||
                causeRegAddr == 0x13800390 ||
                causeRegAddr == 0x138003A0 ||
                causeRegAddr == 0x13800338 ||
                causeRegAddr == 0x13800348 ||
                causeRegAddr == 0x13800358 ||
                causeRegAddr == 0x13800368 ||
                causeRegAddr == 0x13800378 ||
                causeRegAddr == 0x13800388 ||
                causeRegAddr == 0x13800398 ||
                causeRegAddr == 0x138003A8 ||
                causeRegAddr == 0x138003B0 ||
                causeRegAddr == 0x138003D0)
            {
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = dummyMaskRegAddr;
            }
            else/*legacy PFC unit*/
            if (causeRegAddr == 0x10090080 ||
                causeRegAddr == 0x10090090)
            {
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = dummyMaskRegAddr;
            }
            else /*legacy BMA claster and BMA core*/
            if (causeRegAddr == 0x03880000 ||
                causeRegAddr == 0x03810000 ||
                causeRegAddr == 0x03830000 ||
                causeRegAddr == 0x03850000 ||
                causeRegAddr == 0x03870000)
            {
                lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                lion2IntrScanArrSinglePortGroup[ii].maskRegAddr  = dummyMaskRegAddr;
            }
            else
            if(lion2IntrScanArrSinglePortGroup[ii].causeRegAddr != PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS)
            {
                /* this is 'legacy unit' that was not covered yet , so we can only adjust it's unit address */
                rc = convertRegAddrToNewUnitsBaseAddr(devNum,&lion2IntrScanArrSinglePortGroup[ii].causeRegAddr);
                if(rc != GT_OK ||
                    lion2IntrScanArrSinglePortGroup[ii].causeRegAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
                {
                    /* this unit is not supported in the device */
                    lion2IntrScanArrSinglePortGroup[ii].causeRegAddr = dummyMaskRegAddr;
                    lion2IntrScanArrSinglePortGroup[ii].maskRegAddr = dummyMaskRegAddr;

                    continue;
                }

                rc = convertRegAddrToNewUnitsBaseAddr(devNum,&lion2IntrScanArrSinglePortGroup[ii].maskRegAddr);
                if(rc != GT_OK)
                {
                    /* error */
                }
            }
            }
            initDone = GT_TRUE;
        }
    }

    /*
       1. fill the array of mask registers addresses
       2. fill the array of default values for the mask registers
       3. update the bits of nonSumBitMask in the scan tree
    */
    rc = prvCpssDrvExMxDxHwPpMaskRegInfoGet(
            CPSS_PP_FAMILY_DXCH_LION2_E,
            LION2_NUM_ELEMENTS_IN_SCAN_TREE_CNS,
            lion2IntrScanArrSinglePortGroup,
            LION2_NUM_MASK_REGISTERS_CNS,
            lion2MaskRegDefaultSummaryArr,
            lion2MaskRegMapArr);
    if(rc != GT_OK)
    {
        /* error */
        return ;
    }

    /* lion2 info , port group 0
       code will loop on all port groups */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_LION2_PORT_GROUP_ID_0_E];

    for(portGroupId = 0; portGroupId < LION2_NUM_PORT_GROUPS_CNS; portGroupId++ ,
        devFamilyPtr++)
    {
        cpssOsMemCpy(lion2MaskRegMapArr_perPortGroup[portGroupId],
                    lion2MaskRegMapArr,
                    sizeof(lion2MaskRegMapArr));

        for(ii = 0 ; ii < LION2_NUM_MASK_REGISTERS_CNS; ii++)
        {
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            /* check if some addresses are not applicable in some port groups */
            if(GT_TRUE == prvCpssDuplicatedMultiPortGroupsGet(devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,lion2MaskRegMapArr[ii],
                    &portGroupsBmp,NULL,NULL))
            {
                if(0 == (portGroupsBmp & (1 << portGroupId)))
                {
                    lion2MaskRegMapArr_perPortGroup[portGroupId][ii] = dummyMaskRegAddr;
                }
            }
        }

        devFamilyPtr->numOfInterrupts = PRV_CPSS_LION2_LAST_INT_E;

        devFamilyPtr->maskRegistersAddressesArray = lion2MaskRegMapArr_perPortGroup[portGroupId];
        devFamilyPtr->maskRegistersDefaultValuesArray = lion2MaskRegDefaultSummaryArr;
        devFamilyPtr->interruptsScanArray = &lion2IntrScanArr[portGroupId][0];
        devFamilyPtr->numOfScanElements = LION2_NUM_ELEMENTS_IN_SCAN_TREE_CNS;
        devFamilyPtr->fdbTrigEndedId = 0;/* don't care */
        devFamilyPtr->fdbTrigEndedCbPtr = NULL;
        devFamilyPtr->hasFakeInterrupts = GT_FALSE;
        devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
        devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
        devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
        devFamilyPtr->notAccessibleBeforeStartInitPtr=NULL;

        /* copy interrupts generic tree into the per port group tree */
        cpssOsMemCpy((PRV_CPSS_DRV_INTERRUPT_SCAN_STC *)devFamilyPtr->interruptsScanArray,/*casting to remove the 'const'*/
            lion2IntrScanArrSinglePortGroup,
            LION2_NUM_ELEMENTS_IN_SCAN_TREE_CNS * sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC));
    }
}

/*******************************************************************************
* prvCpssDrvPpIntDefDxChBobcat2Init__GM
*
* DESCRIPTION:
*       Interrupts initialization for the Bobcat2_Gm devices.
*
* INPUTS:
*       Node.
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
GT_STATUS prvCpssDrvPpIntDefDxChBobcat2Init__GM
(
    IN GT_U8   devNum
)
{
    GT_U32  ii;             /* index iterator */
    GT_STATUS   rc;         /* return code */
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC * devInterruptInfoPtr;
                            /* pointer to device interrupt info */
    GT_U32  causeRegAddr;   /* cause register address */
    GT_U32  unitBaseAddr;/*base address of the relevant unit */

    /* use the info of Lion2 */
     /*set info needed by chIntCauseToUniEvConvertDedicatedTables(...) */
    setLion2DedicatedEventsConvertInfo(devNum);

    /* update the tree interrupt offsets */
    for(ii = 0 ; ii < lion2UniEvMapTableSize; ii++)
    {
        if (lion2UniEvMapTable[ii][0] == CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E ) {
            lion2UniEvMapTable[ii][1] = PRV_CPSS_LION2_FDB_RESERVED11_E;
        }
    }

    /* update the DB */
    for(ii = 0 ; ii < BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS; ii++)
    {
        causeRegAddr = bobcat2_Gm_IntrScanArrSinglePortGroup[ii].causeRegAddr;

        if(causeRegAddr == 0x02040130) /*L2I*/
        {
            unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_L2I_E,NULL);
            bobcat2_Gm_IntrScanArrSinglePortGroup[ii].causeRegAddr =
                unitBaseAddr + 0x00002100;
            bobcat2_Gm_IntrScanArrSinglePortGroup[ii].maskRegAddr =
                unitBaseAddr + 0x00002104;
        }
        else
        if(causeRegAddr == 0x06000018) /*FDB*/
        {
            unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,PRV_CPSS_DXCH_UNIT_FDB_E,NULL);
            bobcat2_Gm_IntrScanArrSinglePortGroup[ii].causeRegAddr =
                unitBaseAddr + 0x000001B0;
            bobcat2_Gm_IntrScanArrSinglePortGroup[ii].maskRegAddr =
                unitBaseAddr + 0x000001B4;
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
            bobcat2_Gm_IntrScanArrSinglePortGroup,
            BOBCAT2_NUM_MASK_REGISTERS_CNS,
            bobcat2_Gm_MaskRegDefaultSummaryArr,
            bobcat2_Gm_MaskRegMapArr);
    if(rc != GT_OK)
    {
        /* error */
        return rc;
    }

    /* lion2 info, port group 0 */
    devInterruptInfoPtr =
        &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_LION2_PORT_GROUP_ID_0_E];


    devInterruptInfoPtr->numOfInterrupts = PRV_CPSS_LION2_LAST_INT_E;
    devInterruptInfoPtr->maskRegistersAddressesArray = bobcat2_Gm_MaskRegMapArr;
    devInterruptInfoPtr->maskRegistersDefaultValuesArray = bobcat2_Gm_MaskRegDefaultSummaryArr;
    devInterruptInfoPtr->interruptsScanArray = bobcat2_Gm_IntrScanArr;
    devInterruptInfoPtr->numOfScanElements = BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS;
    devInterruptInfoPtr->fdbTrigEndedId = 0;/* don't care */
    devInterruptInfoPtr->fdbTrigEndedCbPtr = NULL;
    devInterruptInfoPtr->hasFakeInterrupts = GT_FALSE;
    devInterruptInfoPtr->firstFakeInterruptId = 0;/* don't care */
    devInterruptInfoPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
    devInterruptInfoPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
    devInterruptInfoPtr->notAccessibleBeforeStartInitPtr = NULL;

    /* copy interrupts generic tree into the per port group tree */
    cpssOsMemCpy(
        (PRV_CPSS_DRV_INTERRUPT_SCAN_STC *)devInterruptInfoPtr->interruptsScanArray, /*casting to remove the 'const'*/
        bobcat2_Gm_IntrScanArrSinglePortGroup,
        BOBCAT2_NUM_ELEMENTS_IN_SCAN_TREE_CNS * sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC));

    return GT_OK;
}


#ifdef DUMP_DEFAULT_INFO

/*******************************************************************************
* prvCpssDrvPpIntDefLion2Print
*
* DESCRIPTION:
*      print the interrupts arrays info, of lion2 devices
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
void  prvCpssDrvPpIntDefLion2Print(
    void
)
{
    cpssOsPrintf("Lion2 - start : \n");
    /* port group 0 */
    prvCpssDrvPpIntDefPrint(LION2_NUM_ELEMENTS_IN_SCAN_TREE_CNS,lion2IntrScanArrSinglePortGroup,
                            LION2_NUM_MASK_REGISTERS_CNS,lion2MaskRegDefaultSummaryArr,lion2MaskRegMapArr);
    cpssOsPrintf("Lion2 - End : \n");

}
#endif/*DUMP_DEFAULT_INFO*/

