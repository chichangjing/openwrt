/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefPuma.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- Puma devices
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsPuma.h>

#define INTERRUPT_NUM_MAC(firstIntInReg,bit)           (firstIntInReg + bit)

#define PORT_INTERRUPT_CAUSE_MAC(port)      (0x01500020 + (port)*0x400)
#define HGS_PORT_INTERRUPT_CAUSE_MAC(port)  (0x01500014 + (port)*0x400)

#define GE_PORT_INTERRUPT_LEAF_MAC(port) \
            {(port+1),GT_FALSE,0, NULL,PORT_INTERRUPT_CAUSE_MAC(port) , PORT_INTERRUPT_CAUSE_MAC(port) + 4,\
                prvCpssDrvHwPpPortGroupIsrRead,                         \
                prvCpssDrvHwPpPortGroupIsrWrite,                       \
                INTERRUPT_NUM_MAC(port*32 + INDEX_PORT_0_CNS ,1),   \
                INTERRUPT_NUM_MAC(port*32 + INDEX_PORT_0_CNS ,9),   \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define GE_SERVICE_PORT_INTERRUPT_LEAF_MAC \
            {27,GT_FALSE,0, NULL,PORT_INTERRUPT_CAUSE_MAC(27) , PORT_INTERRUPT_CAUSE_MAC(27) + 4,\
                prvCpssDrvHwPpPortGroupIsrRead,                         \
                prvCpssDrvHwPpPortGroupIsrWrite,                       \
                INTERRUPT_NUM_MAC(INDEX_PORT_27_CNS ,1),   \
                INTERRUPT_NUM_MAC(INDEX_PORT_27_CNS ,9),   \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define HGS_PORT_INTERRUPT_LEAF_MAC(port) \
            {(port+1),GT_FALSE,0, NULL,HGS_PORT_INTERRUPT_CAUSE_MAC(port) , HGS_PORT_INTERRUPT_CAUSE_MAC(port) + 4,\
                prvCpssDrvHwPpPortGroupIsrRead,                         \
                prvCpssDrvHwPpPortGroupIsrWrite,                       \
                INTERRUPT_NUM_MAC(port*32 + INDEX_PORT_0_CNS ,1),                                             \
                INTERRUPT_NUM_MAC(port*32 + INDEX_PORT_0_CNS ,12),                                         \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

                /* RateLimitIntSum<%n> 0..7 --  */
#define EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(rlIndex) \
            {3 + (rlIndex), GT_FALSE,0, NULL, 0x00700080 + (rlIndex)*4 , 0x007000A4 + (rlIndex)*4, \
                prvCpssDrvHwPpPortGroupIsrRead,                         \
                prvCpssDrvHwPpPortGroupIsrWrite,                       \
                INTERRUPT_NUM_MAC(INDEX_EQ_RATE_LIMIT_CNS + rlIndex*32, 1),                                      \
                INTERRUPT_NUM_MAC(INDEX_EQ_RATE_LIMIT_CNS + rlIndex*32, 31),                                     \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

                /* RateLimitIntSum<%n> 8 --  */
#define EQ_RATE_LIMIT_8_INTERRUPT_LEAF_MAC \
            {3 + (8), GT_FALSE,0, NULL, 0x00700080 + (8)*4 , 0x007000A4 + (8)*4, \
                prvCpssDrvHwPpPortGroupIsrRead,                         \
                prvCpssDrvHwPpPortGroupIsrWrite,                       \
                INTERRUPT_NUM_MAC(INDEX_EQ_RATE_LIMIT_CNS + 8*32, 1),                                      \
                INTERRUPT_NUM_MAC(INDEX_EQ_RATE_LIMIT_CNS + 8*32, 8),                                     \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

                /* XBAR0 Group interrupt */
#define XBAR_GUNIT_INTERRUPT_MAC(group) \
            {(group)+1, GT_FALSE, 0, NULL, 0x03600004+(0x100000*(group)), 0x03600008+(0x100000*(group)),\
                prvCpssDrvHwPpPortGroupIsrRead,                                      \
                prvCpssDrvHwPpPortGroupIsrWrite,                                     \
                INTERRUPT_NUM_MAC(INDEX_GUNIT0_XBAR_CNS + (group)*32, 1),           \
                INTERRUPT_NUM_MAC(INDEX_GUNIT0_XBAR_CNS + (group)*32, 17),          \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

                /* LPM<%n> - Narrow SRAM<%n> Interrupt
                "lpm" is not name of LPM unit but (3 - LPM) is name of the unit. e.g lpm=0 means LPM3
                */
#define NSRAM_INTERRUPT_MAC(lpm) \
            {(lpm)+6, GT_FALSE, 0, NULL, 0x01C0000C-(0x100000*(lpm)), 0x01C00010-(0x100000*(lpm)),\
                prvCpssDrvHwPpPortGroupIsrRead,                                      \
                prvCpssDrvHwPpPortGroupIsrWrite,                                     \
                INTERRUPT_NUM_MAC(INDEX_NS4_CNS + (lpm)*32, 1),             \
                INTERRUPT_NUM_MAC(INDEX_NS4_CNS + (lpm)*32, 2),             \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

                /* Sunit<%n>_int - HyperG.Link<%n> Main Interrupt
                   "unit" is not name of HGL unit but (3 - unit) is name of the unit. e.g unit=0 means HyperG Link 3 */
#define HGLINK_INTERRUPT_MAC(unit) \
            {(unit)+16, GT_FALSE, 0, NULL, 0x03500008-(0x100000*(unit)), 0x0350000C-(0x100000*(unit)),\
                prvCpssDrvHwPpPortGroupIsrRead,                                      \
                prvCpssDrvHwPpPortGroupIsrWrite,                                     \
                INTERRUPT_NUM_MAC(INDEX_HGLINK3_CNS + (unit)*32, 14),           \
                INTERRUPT_NUM_MAC(INDEX_HGLINK3_CNS + (unit)*32, 29),          \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

                /* HyperG.Port25<%n> Lane Main Interrupt */
#define HG25_LANE_INTERRUPT_MAC(lane) \
            {(lane)+7, GT_FALSE, 0, NULL, 0x01506660+(0x00000044*(lane)), 0x01506664+(0x00000044*(lane)),\
                prvCpssDrvHwPpPortGroupIsrRead,                                      \
                prvCpssDrvHwPpPortGroupIsrWrite,                                     \
                INTERRUPT_NUM_MAC(INDEX_HG25_LANE_CNS + (lane)*32, 1),           \
                INTERRUPT_NUM_MAC(INDEX_HG25_LANE_CNS + (lane)*32, 7),          \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

                /* HyperG.Port24<%n> Lane Main Interrupt */
#define HG24_LANE_INTERRUPT_MAC(lane) \
            {(lane)+7, GT_FALSE, 0, NULL, 0x01506260+(0x00000044*(lane)), 0x01506264+(0x00000044*(lane)),\
                prvCpssDrvHwPpPortGroupIsrRead,                                      \
                prvCpssDrvHwPpPortGroupIsrWrite,                                     \
                INTERRUPT_NUM_MAC(INDEX_HG24_LANE_CNS + (lane)*32, 1),           \
                INTERRUPT_NUM_MAC(INDEX_HG24_LANE_CNS + (lane)*32, 7),          \
                0x0, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/*
 * Typedef: struct INTERRUPT_SCAN
 *
 * Description: Basic struct for accessing interrupt register in hierarchy
 *              tree.
 *
 * Fields:
 *      bitNum          - Sum bit num in upper hierarchy, representing this
 *                        resigter.
 *      isGpp           - Is hierarcy connected to another device.
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

/*order of interrupts*/
#define INDEX_GLOBAL_INTERRUPT_CAUSE_CNS   PRV_CPSS_EXMXPM_GLOBAL_GENERAL_SUM_E
#define INDEX_DSU1_CNS                     PRV_CPSS_EXMXPM_DSU1_SUM_E
#define INDEX_DSU0_CNS                     PRV_CPSS_EXMXPM_DSU0_SUM_E
#define INDEX_VOQ_1_CNS                    PRV_CPSS_EXMXPM_VOQ_1_SUM_E
#define INDEX_VOQ_0_CNS                    PRV_CPSS_EXMXPM_VOQ_0_SUM_E
#define INDEX_PORTS_CNS                    PRV_CPSS_EXMXPM_PORTS_SUMMARY_PORTS_SUM_E
#define INDEX_PORT_0_CNS                   PRV_CPSS_EXMXPM_PORT0_SUM_E
#define INDEX_PORT_27_CNS                  PRV_CPSS_EXMXPM_PORT27_SUM_E
#define INDEX_MISC_CNS                     PRV_CPSS_EXMXPM_MISC_MISC_SUM_E
#define INDEX_TXQ_CNS                      PRV_CPSS_EXMXPM_TXQ_SUMMARY_TXQ_SUM_E
#define INDEX_FLUSHDONE_CNS                PRV_CPSS_EXMXPM_TXQ_FLUSH_DONE_FLUSH_DONE_SUM_E
#define INDEX_TXQGENERAL_CNS               PRV_CPSS_EXMXPM_TXQ_GENERAL_TXQ_GENERAL_SUM_E
#define INDEX_GPP_CNS                      PRV_CPSS_EXMXPM_GPP_SUM_E
#define INDEX_EGRESS_STC_CNS               PRV_CPSS_EXMXPM_EGRESS_STC_EGRESS_STC_SUM_E
#define INDEX_EGRESS_STC_1_CNS             PRV_CPSS_EXMXPM_EGRESS_STC_PORT31_E
#define INDEX_TXQ_FULL_CNS                 PRV_CPSS_EXMXPM_TXQ_FULL_FULL_SUM_E
#define INDEX_L2I_CNS                      PRV_CPSS_EXMXPM_BRIDGE_SUM_E
#define INDEX_BM_EGR_0_CNS                 PRV_CPSS_EXMXPM_BM_EGR0_BM_SUM0_E
#define INDEX_BM_INGR_0_CNS                PRV_CPSS_EXMXPM_BM_ING0_BM_SUM0_E
#define INDEX_BM_INGR_1_CNS                PRV_CPSS_EXMXPM_BM_ING1_BM_SUM1_E
#define INDEX_FDB_CNS                      PRV_CPSS_EXMXPM_FDB_SUM_E
#define INDEX_LMS_CNS                      PRV_CPSS_EXMXPM_PORTS_MIB_COUNTERS_SUMMARY_TRI_SPEED_PORTS_MIB_SUM_E
#define INDEX_GOP0_CNS                     PRV_CPSS_EXMXPM_PORTS_GOP0_MIBS_GOP0_SUM_E
#define INDEX_GOP1_CNS                     PRV_CPSS_EXMXPM_PORTS_GOP1_MIBS_GOP1_SUM_E
#define INDEX_GOP2_CNS                     PRV_CPSS_EXMXPM_PORTS_GOP2_MIBS_GOP2_SUM_E
#define INDEX_GOP3_CNS                     PRV_CPSS_EXMXPM_PORTS_GOP3_AND_XG_MIBS_GOP3_SUM_E
#define INDEX_PLR_CNS                      PRV_CPSS_EXMXPM_INGRESS_POLICER_SUMMARY_E
#define INDEX_WR_DMA_CNS                   PRV_CPSS_EXMXPM_WRITE_DMA_AND_XRMON_WR_DMA_XRMON_SUM_E
#define INDEX_TX_SDMA_CNS                  PRV_CPSS_EXMXPM_TRANSMIT_SDMA_SUM_E
#define INDEX_RX_SDMA_CNS                  PRV_CPSS_EXMXPM_RECEIVE_SDMA_SUM_E
#define INDEX_PCL_CNS                      PRV_CPSS_EXMXPM_POLICY_ENGINE_SUM_E
#define INDEX_CSU36_CNS                    PRV_CPSS_EXMXPM_CSU36_SUMMARY_E
#define INDEX_EXTERNAL_TCAM_CNS            PRV_CPSS_EXMXPM_EXTERNAL_TCAM_EXT_TCAM_SUM_E
#define INDEX_XBAR_CNS                     PRV_CPSS_EXMXPM_XBAR_SUM_SUM_E
#define INDEX_GUNIT0_XBAR_CNS              PRV_CPSS_EXMXPM_XBAR_GCORE0_SUM_SUM_E
#define INDEX_GUNIT1_XBAR_CNS              PRV_CPSS_EXMXPM_XBAR_GCORE1_SUM_SUM_E
#define INDEX_GUNIT2_XBAR_CNS              PRV_CPSS_EXMXPM_XBAR_GCORE2_SUM_SUM_E
#define INDEX_GUNIT3_XBAR_CNS              PRV_CPSS_EXMXPM_XBAR_GCORE3_SUM_SUM_E
#define INDEX_GUNIT4_XBAR_CNS              PRV_CPSS_EXMXPM_XBAR_GCORE4_SUM_SUM_E
#define INDEX_GUNIT5_XBAR_CNS              PRV_CPSS_EXMXPM_XBAR_GCORE5_SUM_SUM_E
#define INDEX_GUNIT6_XBAR_CNS              PRV_CPSS_EXMXPM_XBAR_GCORE6_SUM_SUM_E
#define INDEX_EQ_CNS                       PRV_CPSS_EXMXPM_EQ_GLOBAL_EQ_GLOBAL_SUM_E
#define INDEX_STC_0_CNS                    PRV_CPSS_EXMXPM_INGRESS_STC_INGRESS_STC_0_E
#define INDEX_STC_1_CNS                    PRV_CPSS_EXMXPM_INGRESS_STC_INGRESS_STC_1_E
#define INDEX_EQ_RATE_LIMIT_CNS            PRV_CPSS_EXMXPM_RATE_LIMIT_REG0_RL_SUM_E
#define INDEX_EQ_RATE_LIMIT_8_CNS          PRV_CPSS_EXMXPM_RATE_LIMIT_REG8_RL_SUM_E
#define INDEX_CRX_CNS                      PRV_CPSS_EXMXPM_CRX_SUM_E
#define INDEX_TXD_CNS                      PRV_CPSS_EXMXPM_TXD_SUM_E
#define INDEX_SECONDARY_GLOBAL_CNS         PRV_CPSS_EXMXPM_SECONDARY_GLOBAL_GENERAL_SUM_E
#define INDEX_RLD_CNS                      PRV_CPSS_EXMXPM_EXTERNAL_MEMORY_RLDRAM_UNIT_SUM_E
#define INDEX_PEX_CNS                      PRV_CPSS_EXMXPM_PEX_DL_DWN_TX_ACC_ERR_E
#define INDEX_PEX_ERR_CNS                  PRV_CPSS_EXMXPM_PEX_DL_DWN_TX_ACC_ERR_E
#define INDEX_IPVX_CNS                     PRV_CPSS_EXMXPM_IPVX_SUM_E
#define INDEX_NS4_CNS                      PRV_CPSS_EXMXPM_NARROW_SRAM4_SUMMARY_E
#define INDEX_NS3_CNS                      PRV_CPSS_EXMXPM_NARROW_SRAM3_SUMMARY_E
#define INDEX_NS2_CNS                      PRV_CPSS_EXMXPM_NARROW_SRAM2_SUMMARY_E
#define INDEX_NS1_CNS                      PRV_CPSS_EXMXPM_NARROW_SRAM1_SUMMARY_E
#define INDEX_XPCS1_CNS                    PRV_CPSS_EXMXPM_XPC1_SUMMARY_E
#define INDEX_XPCS0_CNS                    PRV_CPSS_EXMXPM_XPC0_SUMMARY_E
#define INDEX_HGLINK3_CNS                  PRV_CPSS_EXMXPM_HGLINK3_GENERAL_SUM_E
#define INDEX_HGLINK2_CNS                  PRV_CPSS_EXMXPM_HGLINK2_GENERAL_SUM_E
#define INDEX_HGLINK1_CNS                  PRV_CPSS_EXMXPM_HGLINK1_GENERAL_SUM_E
#define INDEX_HGLINK0_CNS                  PRV_CPSS_EXMXPM_HGLINK0_GENERAL_SUM_E
#define INDEX_HG25_LANE_CNS                PRV_CPSS_EXMXPM_LANE0_SUMMARY_PORT25_E
#define INDEX_HG24_LANE_CNS                PRV_CPSS_EXMXPM_LANE0_SUMMARY_PORT24_E
#define INDEX_TCC_CNS                      PRV_CPSS_EXMXPM_TCC_GENERAL_SUM_E
#define INDEX_PCS_EGR_EQ_CNS               PRV_CPSS_EXMXPM_PCS_EGR_EQ_GENERAL_SUM_E
#define INDEX_PCS_EGR_DQ_CNS               PRV_CPSS_EXMXPM_PCS_EGR_DQ_GENERAL_SUM_E
#define INDEX_PCS_INGR_DQ_CNS              PRV_CPSS_EXMXPM_PCS_INGR_DQ_GENERAL_SUM_E
#define INDEX_EG_PLR_CNS                   PRV_CPSS_EXMXPM_EGRESS_POLICER_SUMMARY_E
#define INDEX_TTI_GENERAL_CNS              PRV_CPSS_EXMXPM_TTI_GENERAL_SUM_E
#define INDEX_GLB_DIT_GENERAL_CNS          PRV_CPSS_EXMXPM_DIT_GENERAL_SUM_E



/* Don't use "const" because we Fill some values at initialization stage */
static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC pumaIntrScanArr[] =
{
    /* Global Interrupt Cause */
    {0, GT_FALSE, 0, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        INTERRUPT_NUM_MAC(INDEX_GLOBAL_INTERRUPT_CAUSE_CNS,4),
        INTERRUPT_NUM_MAC(INDEX_GLOBAL_INTERRUPT_CAUSE_CNS,31),
        0, 0, 0x0, 24, NULL, NULL},


        /* DsuEgrIntSum - DSU1 Interrupt Cause */
        {5, GT_FALSE, 0, NULL, 0x01700018, 0x0170001C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_DSU1_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_DSU1_CNS, 6),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* DsuIngIntSum - DSU0 Interrupt Cause */
        {6, GT_FALSE, 0, NULL, 0x01600018, 0x0160001C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_DSU0_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_DSU0_CNS, 6),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* VoQ-1 */
        {7, GT_FALSE, 0, NULL, 0x01284004, 0x01284000,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_VOQ_1_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_VOQ_1_CNS, 31),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* VoQ-0 */
        {8, GT_FALSE, 0, NULL, 0x01204004, 0x01204000,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_VOQ_0_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_VOQ_0_CNS, 31),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* PortsIntSum - Ports Interrupt Summary */
        {10, GT_FALSE, 0, NULL, 0x00A04010, 0x00905114,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_PORTS_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_PORTS_CNS, 27),
            0x0, 0x0, 0x0, 27, NULL, NULL},

                /* TriSpeedPortIntSum[23:0] - Port<%n> Interrupt Cause */
            GE_PORT_INTERRUPT_LEAF_MAC(0),  /* Port 0 */
            GE_PORT_INTERRUPT_LEAF_MAC(1),  /* Port 1 */
            GE_PORT_INTERRUPT_LEAF_MAC(2),  /* Port 2 */
            GE_PORT_INTERRUPT_LEAF_MAC(3),  /* Port 3 */
            GE_PORT_INTERRUPT_LEAF_MAC(4),  /* Port 4 */
            GE_PORT_INTERRUPT_LEAF_MAC(5),  /* Port 5 */
            GE_PORT_INTERRUPT_LEAF_MAC(6),  /* Port 6 */
            GE_PORT_INTERRUPT_LEAF_MAC(7),  /* Port 7 */
            GE_PORT_INTERRUPT_LEAF_MAC(8),  /* Port 8 */
            GE_PORT_INTERRUPT_LEAF_MAC(9),  /* Port 9 */
            GE_PORT_INTERRUPT_LEAF_MAC(10), /* Port 10 */
            GE_PORT_INTERRUPT_LEAF_MAC(11), /* Port 11 */
            GE_PORT_INTERRUPT_LEAF_MAC(12), /* Port 12 */
            GE_PORT_INTERRUPT_LEAF_MAC(13), /* Port 13 */
            GE_PORT_INTERRUPT_LEAF_MAC(14), /* Port 14 */
            GE_PORT_INTERRUPT_LEAF_MAC(15), /* Port 15 */
            GE_PORT_INTERRUPT_LEAF_MAC(16), /* Port 16 */
            GE_PORT_INTERRUPT_LEAF_MAC(17), /* Port 17 */
            GE_PORT_INTERRUPT_LEAF_MAC(18), /* Port 18 */
            GE_PORT_INTERRUPT_LEAF_MAC(19), /* Port 19 */
            GE_PORT_INTERRUPT_LEAF_MAC(20), /* Port 20 */
            GE_PORT_INTERRUPT_LEAF_MAC(21), /* Port 21 */
            GE_PORT_INTERRUPT_LEAF_MAC(22), /* Port 22 */
            GE_PORT_INTERRUPT_LEAF_MAC(23), /* Port 23 */

                /* PortIntSum_24 - HGS Port<%n> Interrupt Cause */
            HGS_PORT_INTERRUPT_LEAF_MAC(24), /* Port 24 */
                /* PortIntSum_25 - HGS Port<%n> Interrupt Cause */
            HGS_PORT_INTERRUPT_LEAF_MAC(25), /* Port 25 */

            GE_SERVICE_PORT_INTERRUPT_LEAF_MAC, /* Port 27 */

             /* MiscIntSum - Miscellaneous Interrupt Cause */
        {11, GT_FALSE, 0, NULL, 0x00000038, 0x0000003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_MISC_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_MISC_CNS, 15),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* TXQIntSum - Transmit Queue Interrupt Summary Cause */
        {13, GT_FALSE, 0, NULL, 0x01100100, 0x01100104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_TXQ_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_TXQ_CNS, 7),
            0x0, 0x0, 0x0, 5, NULL, NULL},

            /* FlushDoneSum - Transmit Queue Flush Done Interrupt Cause */
            {2, GT_FALSE, 0, NULL, 0x01100110, 0x01100114,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_FLUSHDONE_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_FLUSHDONE_CNS, 29),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* TxqGeneralInt Sum - Transmit Queue General Interrupt Cause */
            {3, GT_FALSE, 0, NULL, 0x01100118, 0x0110011C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_TXQGENERAL_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_TXQGENERAL_CNS, 31),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* GPPIntSum - GPP */
            {4, GT_FALSE, 0, NULL, 0x011001CC, 0x011001C0,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_GPP_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_GPP_CNS, 8),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

               /* EgressSTCInt Sum - Egress STC Interrupt Cause */
            {5, GT_FALSE, 0, NULL, 0x01100130, 0x01100134,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_EGRESS_STC_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_EGRESS_STC_CNS, 31),
                0, 0x0, 0xFFFFFFFF, 1, NULL, NULL},

                /* EgressSTCInt Sum - Egress STC Interrupt Cause */
                {0, GT_FALSE, 0, NULL, 0x01100530, 0x01100534,
                    prvCpssDrvHwPpPortGroupIsrRead,
                    prvCpssDrvHwPpPortGroupIsrWrite,
                    INTERRUPT_NUM_MAC(INDEX_EGRESS_STC_1_CNS, 0),
                    INTERRUPT_NUM_MAC(INDEX_EGRESS_STC_1_CNS, 31),
                    0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqFullIntSum - Transmit Queue Full */
            {7, GT_FALSE, 0, NULL, 0x0110018C, 0x011001EC,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_TXQ_FULL_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_TXQ_FULL_CNS, 31),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* L2IIntSum - Bridge Interrupt Cause   */
        {14, GT_FALSE,0, NULL, 0x00340130, 0x00340134,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_L2I_CNS, 2),
            INTERRUPT_NUM_MAC(INDEX_L2I_CNS, 28),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


        /* BmEgr0IntSum - Buffer Management Interrupt Cause Register0 */
        {15, GT_FALSE, 0, NULL, 0x02200040, 0x02200044,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_BM_EGR_0_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_BM_EGR_0_CNS, 23),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* BmIng0IntSum - Buffer Management Interrupt Cause Register0 */
        {16, GT_FALSE, 0, NULL, 0x02100040, 0x02100044,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_BM_INGR_0_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_BM_INGR_0_CNS, 23),
            0, 0x0, 0xFFFFFFFF, 1, NULL, NULL},

            /* BmIng1IntSum - Buffer Management Interrupt Cause Register1 */
            {23, GT_FALSE, 0, NULL, 0x02100048, 0x0210004C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_BM_INGR_1_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_BM_INGR_1_CNS, 31),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* FDBIntSum - FDB Interrupt Cause,
           maskRcvIntrEn - both AUProcCompletedInt(bit 9) and AgeViaTriggerEnded(bit 4)
           should never be masked to avoid missed events situation.  */
        {17, GT_FALSE,0, NULL, 0x00400308, 0x0040030C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_FDB_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_FDB_CNS, 23),
            0, 0x0, (0xFFFFFFFF & ~ (BIT_4 | BIT_9)) , 0, NULL, NULL},

            /* lmsIntSum - Ports MIB Counters Interrupt Summary */
        {18, GT_FALSE, 0, NULL, 0x00804010, 0x00905110,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_LMS_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_LMS_CNS, 4),
            0x0, 0x0, 0x0, 4, NULL, NULL},

                /* GOP<0>MIB IntSum */
            {1, GT_FALSE, 0, NULL, 0x00804000, 0x00804004,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_GOP0_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_GOP0_CNS, 8),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                /* GOP<1>MIB IntSum */
            {2, GT_FALSE, 0, NULL, 0x00904000, 0x00904004,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_GOP1_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_GOP1_CNS, 4),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                /* GOP<2>MIB IntSum */
            {3, GT_FALSE, 0, NULL, 0x00A04000, 0x00A04004,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_GOP2_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_GOP2_CNS, 7),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                /* GOP<3>MIB IntSum */
            {4, GT_FALSE, 0, NULL, 0x00B04000, 0x00B04004,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_GOP3_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_GOP3_CNS, 4),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* PlrIntsum - Policer Interrupt Cause */
        {19, GT_FALSE, 0, NULL, 0x00600100, 0x00600104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_PLR_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_PLR_CNS, 2),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* WrDmaIntSum - Write DMA and Xrmon Interrupt Cause */
        {20, GT_FALSE, 0, NULL, 0x01400024, 0x01400028,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_WR_DMA_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_WR_DMA_CNS, 1),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* TxSDMASum - Transmit SDMA Interrupt Cause */
        {21, GT_FALSE, 0, NULL, 0x00002810, 0x00002818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_TX_SDMA_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_TX_SDMA_CNS, 24),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* RxSDMASum - Receive SDMA Interrupt Cause  */
        {22, GT_FALSE, 0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_RX_SDMA_CNS, 2),
            INTERRUPT_NUM_MAC(INDEX_RX_SDMA_CNS, 21),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* PCL -- Policy Engine Interrupt Cause */
        {23, GT_FALSE, 0, NULL, 0x00200004, 0x00200008,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_PCL_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_PCL_CNS, 15),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Csu36IntSum -  CSU-0/CSU_UNIT(shared)/Interrupt Cause */
        {24, GT_FALSE, 0, NULL, 0x0180000C, 0x01800010,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_CSU36_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_CSU36_CNS, 2),
            0, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* EtcIntSum -  External TCAM Interrupt Cause */
        {25, GT_FALSE, 0, NULL, 0x01300008, 0x0130000C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_EXTERNAL_TCAM_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_EXTERNAL_TCAM_CNS, 13),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* Xbar0 - Crossbar Main Interrupt Cause */
        {27, GT_FALSE, 0, NULL, 0x01D70004, 0x01d70008,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_XBAR_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_XBAR_CNS, 6),
            0, 0x0, 0x0, 6, NULL, NULL},

            XBAR_GUNIT_INTERRUPT_MAC(0),    /* Group interrupt 0 */
            XBAR_GUNIT_INTERRUPT_MAC(1),    /* Group interrupt 1 */
            XBAR_GUNIT_INTERRUPT_MAC(2),    /* Group interrupt 2 */
            XBAR_GUNIT_INTERRUPT_MAC(3),    /* Group interrupt 3 */
            XBAR_GUNIT_INTERRUPT_MAC(4),    /* Group interrupt 4 */
            XBAR_GUNIT_INTERRUPT_MAC(5),    /* Group interrupt 5 */

             /* EqIntSum - Eq Global Interrupt Cause */
        {28, GT_FALSE, 0, NULL, 0x00700390, 0x00700394,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_EQ_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_EQ_CNS, 11),
            0x0, 0x0, 0x0, 11, NULL, NULL},

                /* StcIntSum0 - Ingress STC Interrupt Cause 0  */
            {1, GT_FALSE, 0, NULL, 0x00700060, 0x00700068,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_STC_0_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_STC_0_CNS, 31),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                /* StcIntSum1 - Ingress STC Interrupt Cause 1  */
            {2, GT_FALSE,0, NULL, 0x00700064, 0x0070006C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_STC_1_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_STC_1_CNS, 31),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                /* RateLimitIntSum<%n> 0..7 --  RateLimit Interrupt Cause Reg<%n>*/
            EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(0),
            EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(1),
            EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(2),
            EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(3),
            EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(4),
            EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(5),
            EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(6),
            EQ_RATE_LIMIT_INTERRUPT_LEAF_MAC(7),
            EQ_RATE_LIMIT_8_INTERRUPT_LEAF_MAC,

            /* CrxIntSum - CRX Interrupt Cause */
        {29, GT_FALSE, 0, NULL, 0x01F00010, 0x01F00014,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_CRX_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_CRX_CNS, 27),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* TxdIntSum - TxD Interrupt Cause */
        {30, GT_FALSE,0, NULL, 0x02000098, 0x0200009C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_TXD_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_TXD_CNS, 14),
            0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* SecGlobalIntSum - Secondary Global Interrupt Cause */
        {31, GT_FALSE,0, NULL, 0x00000060, 0x00000064,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_SECONDARY_GLOBAL_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_SECONDARY_GLOBAL_CNS, 28),
            0, 0x0, 0x0, 20, NULL, NULL},

                /* RldIntSum - RL_Dram unit Interrupt */
            {2, GT_FALSE, 0, NULL, 0x02300024, 0x02300028,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_RLD_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_RLD_CNS, 5),
                0, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* PexErrSum -  PEX Error Interrupt Cause */
            {4, GT_FALSE,0, NULL, 0x00071900, 0x00071910,
                prvCpssDrvHwPpPortGroupReadInternalPciReg,
                prvCpssDrvHwPpPortGroupWriteInternalPciReg,
                INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 0),
                INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 27),
                0, 0xFF1F1A, 0xFFFFFFFF, 0, NULL, NULL},

                /* IpvxIntSum - IPVX Interrupt Cause  */
            {5, GT_FALSE, 0, NULL, 0x00500E80, 0x00500E84,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_IPVX_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_IPVX_CNS, 11),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            NSRAM_INTERRUPT_MAC(0),     /* LPM3 - Narrow SRAM4 Interrupt */
            NSRAM_INTERRUPT_MAC(1),     /* LPM2 - Narrow SRAM3 Interrupt */
            NSRAM_INTERRUPT_MAC(2),     /* LPM1 - Narrow SRAM2 Interrupt */
            NSRAM_INTERRUPT_MAC(3),     /* LPM0 - Narrow SRAM1 Interrupt */

            /* XPCS - XG25 Interrupt Cause. */
            {14, GT_FALSE, 0, NULL, 0x01506614, 0x01506618,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_XPCS1_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_XPCS1_CNS, 12),
                0, 0x0, 0xFFFFFFFF, 4, NULL, NULL},

                HG25_LANE_INTERRUPT_MAC(0),
                HG25_LANE_INTERRUPT_MAC(1),
                HG25_LANE_INTERRUPT_MAC(2),
                HG25_LANE_INTERRUPT_MAC(3),

            /* XPCS - XG24 Interrupt Cause. */
            {15, GT_FALSE, 0, NULL, 0x01506214, 0x01506218,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_XPCS0_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_XPCS0_CNS, 12),
                0, 0x0, 0xFFFFFFFF, 4, NULL, NULL},

                HG24_LANE_INTERRUPT_MAC(0),
                HG24_LANE_INTERRUPT_MAC(1),
                HG24_LANE_INTERRUPT_MAC(2),
                HG24_LANE_INTERRUPT_MAC(3),

            /* Sunit<%n>_int - HyperG.Link<%n> Main Interrupt */
            HGLINK_INTERRUPT_MAC(0),    /* Unit 3 */
            HGLINK_INTERRUPT_MAC(1),    /* Unit 2 */
            HGLINK_INTERRUPT_MAC(2),    /* Unit 1 */
            HGLINK_INTERRUPT_MAC(3),    /* Unit 0 */

            /* TccInt - TCC Interrupt Cause */
            {20, GT_FALSE, 0, NULL, 0x02B00060, 0x02B00064,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_TCC_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_TCC_CNS, 4),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* StatPcsEgrEqInt - Interrupt Cause */
            {21, GT_FALSE, 0, NULL, 0x02980104, 0x02980108,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_PCS_EGR_EQ_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_PCS_EGR_EQ_CNS, 7),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* StatPcsEgrDqInt - Interrupt Cause */
            {22, GT_FALSE, 0, NULL, 0x02980004, 0x02980008,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_PCS_EGR_DQ_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_PCS_EGR_DQ_CNS, 7),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* StatPcsIngDqInt - Interrupt Cause */
            {23, GT_FALSE, 0, NULL, 0x02880004, 0x02880008,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_PCS_INGR_DQ_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_PCS_INGR_DQ_CNS, 7),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* EplrInt - E-PLR Interrupt Cause */
            {26, GT_FALSE, 0, NULL, 0x02400100, 0x02400104,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_EG_PLR_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_EG_PLR_CNS, 2),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* TtiInt - TTI Interrupt Cause */
            {27, GT_FALSE, 0, NULL, 0x00100004, 0x00100008,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_TTI_GENERAL_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_TTI_GENERAL_CNS, 3),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* DitInt - DIT Interrupt Cause */
            {28, GT_FALSE, 0, NULL, 0x00C00290, 0x00C00294,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                INTERRUPT_NUM_MAC(INDEX_GLB_DIT_GENERAL_CNS, 1),
                INTERRUPT_NUM_MAC(INDEX_GLB_DIT_GENERAL_CNS, 6),
                0, 0x0, 0xFFFFFFFF, 0, NULL, NULL}
};

#define NUM_CAUSE_REGS \
    sizeof(pumaIntrScanArr)/sizeof(pumaIntrScanArr[0])

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 pumaMaskRegMapArr[NUM_CAUSE_REGS] =
{
    0
};

/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 pumaMaskRegDefaultSummaryArr[NUM_CAUSE_REGS] =
{
    0
};

/* array of interrupt registers that can't be accessed before 'Start Init' */
static GT_U32   notAccessibleBeforeStartInitArray[] =
{
    0x00340134, /*Bridge Interrupt Mask*/
    0x00200014  /* Policy Engine Interrupt Mask */
};

static GT_U32 numOfInterruptRegistersNotAccessibleBeforeStartInit =
                (sizeof(notAccessibleBeforeStartInitArray)/
                sizeof(notAccessibleBeforeStartInitArray[0]));

/*******************************************************************************
* prvCpssDrvPpIntDefPumaInit
*
* DESCRIPTION:
*       Interrupts initialization for the Puma devices.
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
void prvCpssDrvPpIntDefPumaInit(void)
{
    GT_U32  rc;
    GT_U32  ii;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;

    /*
       1. fill the array of mask registers addresses
       2. fill the array of default values for the mask registers
       3. update the bits of nonSumBitMask in the scan tree
    */
    rc = prvCpssDrvExMxDxHwPpMaskRegInfoGet(
            CPSS_PP_FAMILY_PUMA_E,
            NUM_CAUSE_REGS,
            pumaIntrScanArr,
            NUM_CAUSE_REGS,
            pumaMaskRegDefaultSummaryArr,
            pumaMaskRegMapArr);
    if(rc != GT_OK)
    {
        /* error */
        return ;
    }

    /* disable problematic interrupts */
    for(ii = 0 ; ii < NUM_CAUSE_REGS; ii++)
    {
        /* BMInterruptCause1IntSum summary bit (bit 23) is inverted.
           Disable BMInterruptCause1IntSum summary bit   */
        if(pumaMaskRegMapArr[ii] == 0x02100044)
        {
            pumaMaskRegDefaultSummaryArr[ii] &= ~BIT_23;
        }

        /* Ref#: FE-xxx-10010110
           TXQ STC Interrupt is not valid (PRV_CPSS_EXMXPM_FER_TXQ_STC_INTR_NOT_VALID_E).
           Disable summary for egress STC in the Transmit Queue Interrupt Summary Mask */
        if(pumaMaskRegMapArr[ii] == 0x01100104)
        {
            pumaMaskRegDefaultSummaryArr[ii] &= ~BIT_5;
        }

        /* Disable TCC interrupt summary bit */
        if(pumaMaskRegMapArr[ii] == 0x00000064)
        {
            pumaMaskRegDefaultSummaryArr[ii] &= ~BIT_20;
        }
    }

    /* Set Puma info */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_PUMA_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_EXMXPM_LAST_INT_E;
    devFamilyPtr->maskRegistersAddressesArray = pumaMaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = pumaMaskRegDefaultSummaryArr;
    devFamilyPtr->interruptsScanArray = pumaIntrScanArr;
    devFamilyPtr->numOfScanElements = NUM_CAUSE_REGS;
    devFamilyPtr->fdbTrigEndedId = 0;/* don't care !!!! */
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntPumaIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = numOfInterruptRegistersNotAccessibleBeforeStartInit;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=notAccessibleBeforeStartInitArray;

    return;
}

#ifdef DUMP_DEFAULT_INFO
/*******************************************************************************
* prvCpssDrvPpIntDefPumaPrint
*
* DESCRIPTION:
*      print the interrupts arrays info, of puma devices
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
void  prvCpssDrvPpIntDefPumaPrint(
    void
)
{
    prvCpssDrvPpIntDefPrint(NUM_CAUSE_REGS,pumaIntrScanArr,
                            NUM_CAUSE_REGS,pumaMaskRegMapArr,
                            pumaMaskRegDefaultSummaryArr);
}
#endif/*DUMP_DEFAULT_INFO*/

