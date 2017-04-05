/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefDxChLionB.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- DxCh Lion devices
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
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLionB.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxUtilLion.h>

#define LION_NUM_PORT_GROUPS_CNS  4

/* TxqQueueIntSum  - PFC Port Group <0..3> Global Overflow Interrupt Cause */
#define PFC_PORT_GROUP_GLOBAL_OVERFLOW_MAC(_portGroup) \
                 {3+2*(_portGroup), GT_FALSE,0, NULL, 0x0A0900B0+0x10*(_portGroup), 0x0A0900B4+0x10*(_portGroup),   \
                      prvCpssDrvHwPpPortGroupIsrRead,                                                       \
                      prvCpssDrvHwPpPortGroupIsrWrite,                                                      \
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_PORT_GROUP_0_GLOBAL_OVERFLOW_TC_0_E + (32*2*_portGroup), \
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_PORT_GROUP_0_GLOBAL_OVERFLOW_TC_7_E + (32*2*_portGroup), \
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL}

/* TxqQueueIntSum  - PFC Port Group <0..3> Global Underflow Interrupt Cause */
#define PFC_PORT_GROUP_GLOBAL_UNDERFLOW_MAC(_portGroup) \
                 {4+2*(_portGroup), GT_FALSE,0, NULL, 0x0A0900B8+0x10*(_portGroup), 0x0A0900BC+0x10*(_portGroup),   \
                      prvCpssDrvHwPpPortGroupIsrRead,                                                       \
                      prvCpssDrvHwPpPortGroupIsrWrite,                                                      \
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_0_E + (32*2*(_portGroup)), \
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_PORT_GROUP_0_GLOBAL_UNDERFLOW_TC_7_E + (32*2*(_portGroup)), \
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL}

#define XLG_XPCS_LANE_INT_LEAF_MAC(lane)    \
                {2+(lane), GT_FALSE,0, NULL,    \
                    XLG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(LION_B_XLG_PORT_NUM_CNS,(lane)), /*0x08803200 + 0xD0 + lane*8*/ \
                    XLG_PORT_XPCS_LANE_INTERRUPT_MASK_MAC(LION_B_XLG_PORT_NUM_CNS,(lane)), /*0x08803200 + 0xD0 + lane*8*/  \
                    prvCpssDrvHwPpPortGroupIsrRead,                                                                        \
                    prvCpssDrvHwPpPortGroupIsrWrite,                                                                       \
                    PRV_CPSS_LION_B_XLG_PORT10_LANE0_TX_GB_SYNC_UNDERRUN_E + DISTANCE_BETWEEN_LANES_INTERRUPTS_CNS(LION_B) * (lane), \
                    PRV_CPSS_LION_B_XLG_PORT10_LANE0_RX_GB_LOCK_LOST_E  + DISTANCE_BETWEEN_LANES_INTERRUPTS_CNS(LION_B) * (lane) ,   \
                    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}



extern GT_BOOL prvCpssLionDuplicatedMultiPortGroupsGet
(
    IN GT_U32                   regAddr,
    OUT  GT_PORT_GROUPS_BMP     *portGroupsBmpPtr
);

/* first emulated GIG MAC interrupt summary bit */
#define FIRST_EMULATED_GIG_BIT_CNS 19

/*******************************************************************************
* lionBXgPortMultiplexerHwPpPortGroupIsrRead
*
* DESCRIPTION:
*       Read a Global1 Interrupt Cause register value using special interrupt
*       address completion region in the specific port group in the device.
*       Emulate GIG MAC summary interrupts
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*                     Note - regAddr should be < 0x1000000
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
static GT_STATUS lionBXgPortMultiplexerHwPpPortGroupIsrRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   *dataPtr
)
{
    GT_STATUS rc; /* return code */
    GT_U32    portsBmp; /* bitmap of ports with set interrupt summary */

    /* read cause register */
    rc = prvCpssDrvHwPpPortGroupIsrRead(devNum, portGroupId, regAddr, dataPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get Port_n_Int_sum  bits 1-11 and 31 */
    portsBmp = ((*dataPtr) >> 31) | ((*dataPtr) & 0xFFE);

    /* set emulated Gig MAC bits */
    (*dataPtr) |= (portsBmp << FIRST_EMULATED_GIG_BIT_CNS);

    return GT_OK;
}



/* Don't use "const" because we Fill some values at initialization stage */
/* a scan tree of single port group */
static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC lionIntrScanArrSinglePortGroup[] =
{
    /* Global Interrupt Cause */
    {0, GT_FALSE, 0, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_LION_B_GLOBAL_PEX_SUM_E,
        PRV_CPSS_LION_B_GLOBAL_EPLR_SUM_E,
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 28 , NULL, NULL},

        /* PEX */
        {1,GT_FALSE,0, NULL, 0x00071900 , 0x00071910,
             prvCpssDrvHwPpPortGroupReadInternalPciReg,
             prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            PRV_CPSS_LION_B_PEX_RCV_A_E,
            PRV_CPSS_LION_B_PEX_RCV_D_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


        /* PEX ERROR */
        {2,GT_FALSE,0, NULL, 0x00071900 , 0x00071910,
             prvCpssDrvHwPpPortGroupReadInternalPciReg,
             prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            PRV_CPSS_LION_B_PEX_DL_DWN_TX_ACC_ERR_E,
            PRV_CPSS_LION_B_PEX_PEX_LINK_FAIL_E,
            FILLED_IN_RUNTIME_CNS, 0xFF1F1B, 0xFFFFFFFF, 0,NULL, NULL},


        /* CNC */
        {3,GT_FALSE,0, NULL, 0x08000108, 0x0800010c,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_CNC_BLOCK0_WRAPAROUND_E,
            PRV_CPSS_LION_B_CNC_CNC_UPDATE_LOST_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* txq_sht_int_sum */
        {5,GT_FALSE,0, NULL, 0x118F0100, 0x118F0110,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TXQ_SHT_GEN_SUM_E,
            PRV_CPSS_LION_B_TXQ_SHT_DFX_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

            /* TXQ_SHT_GEN */
            {1,GT_FALSE,0, NULL, 0x118F0120, 0x118F0130,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TXQ_SHT_GEN_EGR_VLAN_TBL_PARITY_ERROR_E,
                PRV_CPSS_LION_B_TXQ_SHT_GEN_WRONG_ADDR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TXQ_SHT_DFX - SHT RAMS interrupt cause */
            {2,GT_FALSE,0, NULL, 0x118F0D98, 0x118F0D9C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TXQ_SHT_DFX_EGR_STP_TBL_PARITY_ERROR_E,
                PRV_CPSS_LION_B_TXQ_SHT_DFX_ING_STP_TBL_PARITY_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* txq_dq_int_sum */
        {6,GT_FALSE,0, NULL, 0x11000100, 0x11000104,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TXQ_DQ_FLUSH_DONE_SUM_E,
            PRV_CPSS_LION_B_TXQ_DQ_GEN_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 4,NULL, NULL},

            /* TXQ_DQ Flush Done */
            {1,GT_FALSE,0, NULL, 0x11000108, 0x1100010c,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TXQ_DQ_FLUSH_DONE_PORT0_E,
                PRV_CPSS_LION_B_TXQ_DQ_FLUSH_DONE_PORT15_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TXQ_DQ Memory Error */
            {2,GT_FALSE,0, NULL, 0x11000110, 0x11000114,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TXQ_DQ_MEMORY_ERROR_TOKEN_BUCKET_PRIO_PARITY_ERROR_E,
                PRV_CPSS_LION_B_TXQ_DQ_MEMORY_ERROR_STATE_VARIABLE_PARITY_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TXQ_DQ Egress STC */
            {3,GT_FALSE,0, NULL, 0x11000118, 0x1100011c,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT0_E,
                PRV_CPSS_LION_B_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT15_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TXQ_DQ General Interrupt */
            {4,GT_FALSE,0, NULL, 0x11000120, 0x11000124,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TXQ_DQ_GEN_EGRESS_MIRROR_DESC_DROP_E,
                PRV_CPSS_LION_B_TXQ_DQ_GEN_WRONG_ADDR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* tti_int_sum */
        {7,GT_FALSE,0, NULL, 0x010001C0, 0x010001C4,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TTI_ENG_SUM_E,
            PRV_CPSS_LION_B_TTI_ITS_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 3,NULL, NULL},

            /* TTI Engine Interrupt */
            {1,GT_FALSE,0, NULL, 0x01000004, 0x01000008,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TTI_ENG_WRONG_ADDR_E,
                PRV_CPSS_LION_B_TTI_ENG_ACCESS_DATA_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* TTI DFX Interrupt */
            {2,GT_FALSE,0, NULL, 0x01001850, 0x01001854,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TTI_DFX_DATA_ERROR_0_E,
                PRV_CPSS_LION_B_TTI_DFX_DATA_ERROR_2_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* TTI GTS Interrupt */
            {3,GT_FALSE,0, NULL, 0x01002028, 0x0100202C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TTI_GTS_GLOBAL_FIFO_FULL_E,
                PRV_CPSS_LION_B_TTI_GTS_WRONG_ADDR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* tcc_lower_int_sum (PCL) */
        {8,GT_FALSE,0, NULL, 0x0D0001BC, 0x0D0001C0,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TCC_LOWER_GEN_SUM_E,
            PRV_CPSS_LION_B_TCC_LOWER_DFX_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

            /* tcc_lower gen (PCL) */
            {1,GT_FALSE,0, NULL, 0x0D0001A4, 0x0D0001A8,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E,
                PRV_CPSS_LION_B_TCC_LOWER_GEN_TCAM_BIST_FAILED_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* tcc_lower DFX (PCL) */
            {2,GT_FALSE,0, NULL, 0x0D002010, 0x0D002014,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TCC_LOWER_DFX_DATA_ERROR_0_E,
                PRV_CPSS_LION_B_TCC_LOWER_DFX_DATA_ERROR_3_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* tcc_upper_int_sum (IPvX) */
        {9,GT_FALSE,0, NULL, 0x0D800078, 0x0D80007C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TCC_UPPER_GEN_SUM_E,
            PRV_CPSS_LION_B_TCC_UPPER_DFX_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

            /* tcc_upper gen (IPvX) */
            {1,GT_FALSE,0, NULL, 0x0D800060, 0x0D800064,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E,
                PRV_CPSS_LION_B_TCC_UPPER_GEN_TCAM_BIST_FAILED_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* tcc_upper DFX (IPvX) */
            {2,GT_FALSE,0, NULL, 0x0D802050, 0x0D802054,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TCC_UPPER_DFX_DATA_ERROR_0_E,
                PRV_CPSS_LION_B_TCC_UPPER_DFX_DATA_ERROR_15_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* ha_int_sum  (header alteration) */
        {10,GT_FALSE,0, NULL, 0x0E8003A8, 0x0E8003AC,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_HA_GEN_SUM_E,
            PRV_CPSS_LION_B_HA_ETS_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},

            /* HA Summary (header alteration) */
            {1,GT_FALSE,0, NULL, 0x0E8003A0, 0x0E8003A4,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_HA_GEN_TS_ARP_TABLE_PARITY_ERROR_E,
                PRV_CPSS_LION_B_HA_GEN_TS_ARP_TABLE_PARITY_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GTS Summary (header alteration) */
            {3,GT_FALSE,0, NULL, 0x0E800828, 0x0E80082C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_HA_GTS_GLOBAL_FIFO_OVERRUN_E,
                PRV_CPSS_LION_B_HA_GTS_WRONG_ADDR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Miscellaneous */
        {11,GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_MISC_TWSI_TIME_OUT_E,
            PRV_CPSS_LION_B_MISC_PEX_FIFO_PARITY_ERROR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


        /* TxqllIntSum */
        {12,GT_FALSE,0, NULL, 0x108A0000, 0x108A0004,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TXQ_LL_GEN_SUM_E,
            PRV_CPSS_LION_B_TXQ_LL_GEN_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1, NULL, NULL},

            /* Txq LL Interrupt Cause */
            {1,GT_FALSE,0, NULL, 0x108A0008, 0x108A000C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TXQ_LL_GEN_ID_FIFO_OVERRUN_E,
                PRV_CPSS_LION_B_TXQ_LL_GEN_LL_WRONG_ADDR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* TxqQueueIntSum   */
        {13, GT_FALSE,0, NULL, 0x0A090000, 0x0A090004,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TXQ_QUEUE_GEN_SUM_E,
            PRV_CPSS_LION_B_TXQ_QUEUE_QCN_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 9, NULL, NULL},

             /* TxqQueueIntSum  - gen */
             {1, GT_FALSE,0, NULL, 0x0A090008, 0x0A09000C,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_GEN_GLOBAL_DESC_FULL_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_GEN_CLEAR_DEMUX_FIFO_3_OVERRUN_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port Descriptor reg 0 */
             {2, GT_FALSE,0, NULL, 0x0A090040, 0x0A090044,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_DESC_FULL_PORT0_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_DESC_FULL_PORT30_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port Descriptor reg 1 */
             {3, GT_FALSE,0, NULL, 0x0A090048, 0x0A09004c,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_DESC_FULL_PORT31_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_DESC_FULL_PORT61_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port Descriptor reg 2 */
             {4, GT_FALSE,0, NULL, 0x0A090050, 0x0A090054,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_DESC_FULL_PORT62_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_DESC_FULL_PORT63_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


             /* TxqQueueIntSum  - Port buff full reg 0 */
             {5, GT_FALSE,0, NULL, 0x0A090060, 0x0A090064,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_BUFF_FULL_PORT0_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_BUFF_FULL_PORT30_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port buff full reg 1 */
             {6, GT_FALSE,0, NULL, 0x0A090068, 0x0A09006c,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_BUFF_FULL_PORT31_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_BUFF_FULL_PORT61_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Port buff full reg 2 */
             {7, GT_FALSE,0, NULL, 0x0A090070, 0x0A090074,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_BUFF_FULL_PORT62_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PORT_BUFF_FULL_PORT63_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - PFC sum */
             {8, GT_FALSE,0, NULL, 0x0A090080, 0x0A090084,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PFC_COUNTERS_PARITY_SUM_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_PFC_MSGS_SETS_OVERRUNS_SUM_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 12, NULL, NULL},


                 /* TxqQueueIntSum  - PFC counters parity */
                 {1, GT_FALSE,0, NULL, 0x0A090090, 0x0A090094,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_COUNTERS_PARITY_PORT_GROUP_0_E,
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_COUNTERS_PARITY_PORT_GROUP_3_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                 /* TxqQueueIntSum  - PFC counters oerv/under flow */
                 {2, GT_FALSE,0, NULL, 0x0A0900A0, 0x0A0900A4,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_COUNTERS_PORT_GROUP_0_OVERFLOW_E ,
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_COUNTERS_PORT_GROUP_3_UNDERFLOW_E ,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                 /* bit 3, TxqQueueIntSum  - PFC Port Group <0> Global Overflow Interrupt Cause */
                 PFC_PORT_GROUP_GLOBAL_OVERFLOW_MAC(0),

                 /* bit 4, TxqQueueIntSum  - PFC Port Group <0> Global Underflow Interrupt Cause */
                 PFC_PORT_GROUP_GLOBAL_UNDERFLOW_MAC(0),

                 /* bit 5, TxqQueueIntSum  - PFC Port Group <1> Global Overflow Interrupt Cause */
                 PFC_PORT_GROUP_GLOBAL_OVERFLOW_MAC(1),

                 /* bit 6, TxqQueueIntSum  - PFC Port Group <1> Global Underflow Interrupt Cause */
                 PFC_PORT_GROUP_GLOBAL_UNDERFLOW_MAC(1),

                 /* bit 7, TxqQueueIntSum  - PFC Port Group <2> Global Overflow Interrupt Cause */
                 PFC_PORT_GROUP_GLOBAL_OVERFLOW_MAC(2),

                 /* bit 8, TxqQueueIntSum  - PFC Port Group <2> Global Underflow Interrupt Cause */
                 PFC_PORT_GROUP_GLOBAL_UNDERFLOW_MAC(2),

                 /* bit 9, TxqQueueIntSum  - PFC Port Group <3> Global Overflow Interrupt Cause */
                 PFC_PORT_GROUP_GLOBAL_OVERFLOW_MAC(3),

                 /* bit 10, TxqQueueIntSum  - PFC Port Group <3> Global Underflow Interrupt Cause */
                 PFC_PORT_GROUP_GLOBAL_UNDERFLOW_MAC(3),

                 /* TxqQueueIntSum  - PFC FIFOs Overruns Cause */
                 {11, GT_FALSE,0, NULL, 0x0A090100, 0x0A090104,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_FIFOS_OVERRUNS_PORT_GROUP_0_EQ_E,
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_FIFOS_OVERRUNS_PORT_GROUP_3_FC_IND_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

                 /* TxqQueueIntSum  - PFC Msgs Sets Overruns Cause */
                 {12, GT_FALSE,0, NULL, 0x0A090110, 0x0A090114,
                      prvCpssDrvHwPpPortGroupIsrRead,
                      prvCpssDrvHwPpPortGroupIsrWrite,
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_MSGS_SETS_OVERRUNS_PORT_GROUP_0_QCN_MSG_OVERRUN_E,
                     PRV_CPSS_LION_B_TXQ_QUEUE_PFC_MSGS_SETS_OVERRUNS_PORT_GROUP_3_EGR_3_MSG_OVERRUN_E,
                     FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* TxqQueueIntSum  - Congestion Notification (QCN) */
             {9, GT_FALSE,0, NULL, 0x0A090120, 0x0A090124,
                  prvCpssDrvHwPpPortGroupIsrRead,
                  prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_LION_B_TXQ_QUEUE_QCN_CN_BUFFER_FIFO_OVERRUN_E,
                 PRV_CPSS_LION_B_TXQ_QUEUE_QCN_CN_DROP_E,
                 FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* L2 Bridge  */
        {14, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_BRIDGE_UPDATE_FIFO_FULL_E,
            PRV_CPSS_LION_B_BRIDGE_UPDATE_SECURITY_BREACH_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* txq_egr_int_sum */
        {15, GT_FALSE,0, NULL, 0x01800080, 0x01800090,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TXQ_EGRESS_GEN_SUM_E,
            PRV_CPSS_LION_B_TXQ_EGRESS_GEN_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1,NULL, NULL},

            /* EgressIntCauseSum  */
            {1, GT_FALSE,0, NULL, 0x018000A0, 0x018000B0,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_TXQ_EGRESS_GEN_MC_FIFO_0_PARITY_ERROR_E,
                PRV_CPSS_LION_B_TXQ_EGRESS_GEN_WRONG_ADDR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* XLG (40G port)  */
        {16, GT_FALSE,0, NULL,
            XG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(LION_B_XLG_PORT_NUM_CNS)/*0x08803058*/ ,
            XG_PORT_SUMMARY_INTERRUPT_MASK_MAC(LION_B_XLG_PORT_NUM_CNS)/*0x0880305C*/ ,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_XLG_PORT_SUM_XG_E,
            PRV_CPSS_LION_B_XLG_PORT_SUM_XPCS_SUMMARY_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL},


            /* XLG - 40G MAC */
            {1, GT_FALSE,0, NULL,
                XG_MAC_PORT_INTERRUPT_CAUSE_MAC(LION_B_XLG_PORT_NUM_CNS) ,  /*0x08803014*/
                XG_MAC_PORT_INTERRUPT_CAUSE_MAC(LION_B_XLG_PORT_NUM_CNS) + 4,/*0x08803018*/
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_XLG_PORT10_LINK_STATUS_CHANGED_E,
                PRV_CPSS_LION_B_XLG_PORT10_MIBX_PARITY_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* XLG - 40G PCS */
            {2, GT_FALSE,0, NULL,
                XLG_PORT_XPCS_INTERRUPT_CAUSE_MAC(LION_B_XLG_PORT_NUM_CNS),
                XLG_PORT_XPCS_INTERRUPT_MASK_MAC(LION_B_XLG_PORT_NUM_CNS),
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_XPCS_XLG_PORT10_GEN_SUM_E,
                PRV_CPSS_LION_B_XPCS_XLG_PORT10_LANE7_SUM_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 9,NULL, NULL},

                /* XLG GEN - 40G PCS */
                {1, GT_FALSE,0, NULL,
                    XLG_PORT_XPCS_GEN_INTERRUPT_CAUSE_MAC(LION_B_XLG_PORT_NUM_CNS), /*0x08803208*/
                    XLG_PORT_XPCS_GEN_INTERRUPT_MASK_MAC(LION_B_XLG_PORT_NUM_CNS),
                     prvCpssDrvHwPpPortGroupIsrRead,
                     prvCpssDrvHwPpPortGroupIsrWrite,
                    PRV_CPSS_LION_B_XPCS_XLG_PORT10_TX_PPM_UNDERRUN_E,
                    PRV_CPSS_LION_B_XPCS_XLG_PORT10_ALIGN_LOCK_LOST_E,
                    FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

                /* XLG lanes - 40G PCS */
                /*bit 2 */XLG_XPCS_LANE_INT_LEAF_MAC(0), /* lane 0 */
                /*bit 3 */XLG_XPCS_LANE_INT_LEAF_MAC(1), /* lane 1 */
                /*bit 4 */XLG_XPCS_LANE_INT_LEAF_MAC(2), /* lane 2 */
                /*bit 5 */XLG_XPCS_LANE_INT_LEAF_MAC(3), /* lane 3 */
                /*bit 6 */XLG_XPCS_LANE_INT_LEAF_MAC(4), /* lane 4 */
                /*bit 7 */XLG_XPCS_LANE_INT_LEAF_MAC(5), /* lane 5 */
                /*bit 8 */XLG_XPCS_LANE_INT_LEAF_MAC(6), /* lane 6 */
                /*bit 9 */XLG_XPCS_LANE_INT_LEAF_MAC(7), /* lane 7 */

        /* FDBIntSum - FDB Interrupt Cause,
           maskRcvIntrEn - both AUProcCompletedInt(bit 9) and AgeViaTriggerEnded(bit 4)
           should never be masked to avoid missed events situation.  */
        {17, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_FDB_NUM_OF_HOP_EX_P_E,
            PRV_CPSS_LION_B_FDB_MT_MEMORY_PARITY_ERROR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, (0xFFFFFFFF & ~ (BIT_4 | BIT_9)), 0,NULL, NULL},


        /*
           bit 18 for the PortsIntSum was left 'unbound' because the register
           should not be connected to any real interrupts :
           the tri-speed (for GE ports) moved to XG port interrupts
           the GOP (MIB counters) not needed since using the XG port interrupts
        */

        /* CPU Port SUM  */         /*0x0A80FC20*/                      /*0x0A80FC24*/
        {19, GT_FALSE,0, NULL, GE_MAC_PORT_INTERRUPT_CAUSE_MAC(63), GE_MAC_PORT_INTERRUPT_CAUSE_MAC(63) + 4,
              prvCpssDrvHwPpPortGroupIsrRead,
              prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_LION_B_CPU_PORT_SUM_LINK_STATUS_CHANGED_E,
             PRV_CPSS_LION_B_CPU_PORT_SUM_TX_FIFO_UNDERRUN_E,
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Global1IntSum  */
        {20, GT_FALSE,0, NULL, 0x00000080, 0x00000084,
              lionBXgPortMultiplexerHwPpPortGroupIsrRead, /* use special function */
              prvCpssDrvHwPpPortGroupIsrWrite,
             PRV_CPSS_LION_B_GLOBAL_1_XG_PORT1_SUM_E,
             PRV_CPSS_LION_B_GLOBAL_1_XG_PORT0_SUM_E,
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 28 ,NULL, NULL},

            /* full per port - XG Ports */

            /* --> bit 31 !!! */      XG_PORT_0_SUB_TREE_MAC(LION_B),
            /* --> bit 1 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,1),
            /* --> bit 2 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,2),
            /* --> bit 3 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,3),
            /* --> bit 4 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,4),
            /* --> bit 5 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,5),
            /* --> bit 6 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,6),
            /* --> bit 7 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,7),
            /* --> bit 8 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,8),
            /* --> bit 9 */           XG_PORT_1_11_SUB_TREE_MAC(LION_B,9),
            /* --> bit 10 */          XG_PORT_1_11_SUB_TREE_MAC(LION_B,10),
            /* --> bit 11 */          XG_PORT_1_11_SUB_TREE_MAC(LION_B,11),

                                                 /*bit*//*port*/
   /*bit 19*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS,  0),  /* dummy GE Port 0 */
   /*bit 20*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 1,  1),  /* dummy GE Port 1 */
   /*bit 21*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 2,  2),  /* dummy GE Port 2 */
   /*bit 22*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 3,  3),  /* dummy GE Port 3 */
   /*bit 23*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 4,  4),  /* dummy GE Port 4 */
   /*bit 24*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 5,  5),  /* dummy GE Port 5 */
   /*bit 25*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 6,  6),  /* dummy GE Port 6 */
   /*bit 26*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 7,  7),  /* dummy GE Port 7 */
   /*bit 27*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 8,  8),  /* dummy GE Port 8 */
   /*bit 28*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 9,  9),  /* dummy GE Port 9 */
   /*bit 29*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 10,  10), /* dummy GE Port 10 */
   /*bit 30*/EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(LION_B,  FIRST_EMULATED_GIG_BIT_CNS + 11,  11), /* dummy GE Port 11 */

            /* ctu_int_sum   */
            {12, GT_FALSE,0, NULL, 0x07003000, 0x07003004,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_CTU_FCP0_PORTA_ECC_ERR_E,
                PRV_CPSS_LION_B_CTU_DESC3_UNUSED_FIFO_ECC_ERR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* bm_int_sum0   */
            {13, GT_FALSE,0, NULL, 0x03000040, 0x03000044,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_BM_0_EMPTY_CLEAR_E,
                PRV_CPSS_LION_B_BM_0_PORT_RX_FULL_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* bm_int_sum1   */
            {14, GT_FALSE,0, NULL, 0x03000048, 0x0300004c,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_BM_1_MAX_BUFF_REACHED_PORT0_E,
                PRV_CPSS_LION_B_BM_1_MAX_BUFF_REACHED_PORT_CPU_63_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /*mppm_int_sum */
            {15, GT_FALSE,0, NULL, 0x06800004, 0x06800008,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_MPPM_BK0_ECC_1_ERROR_E,
                PRV_CPSS_LION_B_MPPM_XG_SMI_WRITE_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Tx SDMA  */
        {21, GT_FALSE,0, NULL, 0x00002810, 0x00002818,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TRANSMIT_SDMA_TX_BUFFER_QUEUE0_E,
            PRV_CPSS_LION_B_TRANSMIT_SDMA_TX_END_QUEUE7_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* Rx SDMA  */
        {22, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_RECEIVE_SDMA_RX_BUFFER_QUEUE0_E,
            PRV_CPSS_LION_B_RECEIVE_SDMA_PACKET_CNT_OF_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

        /* PCL  */
        {23, GT_FALSE,0, NULL, 0x0B800060, 0x0B800064,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_PCL_ENGINE_SUM_E,
            PRV_CPSS_LION_B_PCL_ENGINE_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 1, NULL, NULL},

            {1, GT_FALSE,0, NULL, 0x0B800004, 0x0B800008,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_POLICY_ENGINE_WRONG_ADDR_E,
                PRV_CPSS_LION_B_POLICY_ENGINE_PCL_CONFIG_TABLE_1_PARITY_ERROR_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* IPLR0IntSum  */
        {24, GT_FALSE,0, NULL, 0x0C000100, 0x0C000104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_IPLR0_DATA_ERROR_E,
            PRV_CPSS_LION_B_IPLR0_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* IPLR1IntSum  */
        {25, GT_FALSE,0, NULL, 0x00800100, 0x00800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_IPLR1_DATA_ERROR_E,
            PRV_CPSS_LION_B_IPLR1_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* RxdmaIntSum   */
        {26, GT_FALSE,0, NULL, 0x0F000038, 0x0F00003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_RXDMA_GEN_IBUF_SYNC_FIFO_0_FULL_E,
            PRV_CPSS_LION_B_RXDMA_GEN_IBUF_SYNC_FIFO_1_FULL_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* TxdmaIntSum   */
        {27, GT_FALSE,0, NULL, 0x0F800184, 0x0F800188,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_TXDMA_GEN_WRONG_ADDR_E,
            PRV_CPSS_LION_B_TXDMA_GEN_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


        /* Pre-Egress */
        {28, GT_FALSE,0, NULL, 0x0B000058, 0x0B00005C,
             prvCpssDrvHwPpPortGroupIsrRead,
             prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_EQ_INGRESS_STC_SUM_E,
            PRV_CPSS_LION_B_EQ_PARITY_ERROR_SUM_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 3,NULL, NULL},

            /* Ingress STC  */
            {1, GT_FALSE,0, NULL, 0x0B000020, 0x0B000024,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_INGRESS_STC_PORT0_INGRESS_SAMPLE_LOADED_E,
                PRV_CPSS_LION_B_INGRESS_STC_PORT11_INGRESS_SAMPLE_LOADED_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* STC Rate Limiters */
            {2, GT_FALSE,0, NULL, 0x0B000060, 0x0B000064,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_STC_RATE_LIMITERS_STC_RATE_LIMITER_1_PKT_DROPPED_E,
                PRV_CPSS_LION_B_STC_RATE_LIMITERS_STC_RATE_LIMITER_31_PKT_DROPPED_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* ParityErrorIntSum */
            {3, GT_FALSE,0, NULL, 0x0B000088, 0x0B00008C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_LION_B_EQ_PARITY_ERROR_CPU_CODE_TABLE_E,
                PRV_CPSS_LION_B_EQ_PARITY_ERROR_VPM_TABLE_E,
                FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

         /* BmaIntSum   */
        {29, GT_FALSE,0, NULL, 0x03800000, 0x03800004,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_BMA_DIST_CLEAR_FIFO_ECC_ERR_E,
            PRV_CPSS_LION_B_BMA_CLEAR_SHIFTER_ECC_ERR_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

         /* eplr_int_sum  */
        {31, GT_FALSE,0, NULL, 0x07800100, 0x07800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_LION_B_EPLR_DATA_ERROR_E,
            PRV_CPSS_LION_B_EPLR_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

};

/* number of elements in the array of lionIntrScanArr[] */
#define LION_NUM_ELEMENTS_IN_SCAN_TREE_CNS \
    (sizeof(lionIntrScanArrSinglePortGroup)/sizeof(lionIntrScanArrSinglePortGroup[0]))

/* number of mask registers -- according to enum of interrupts */
#define LION_NUM_MASK_REGISTERS_CNS  (PRV_CPSS_LION_B_LAST_INT_E / 32)

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 lionMaskRegMapArr[LION_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};

/* array per port group for lionMaskRegMapArr */
static GT_U32 lionMaskRegMapArr_perPortGroup[LION_NUM_PORT_GROUPS_CNS][LION_NUM_MASK_REGISTERS_CNS] =
{
    {FILLED_IN_RUNTIME_CNS}
};

/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 lionMaskRegDefaultSummaryArr[LION_NUM_MASK_REGISTERS_CNS] =
{
    FILLED_IN_RUNTIME_CNS
};

/*
    array for the 4 port groups of lion .
    for each port group : lion copied from Lion B0 and updated in runtime
*/
/* Don't use "const" because we Fill values at initialization stage */
static PRV_CPSS_DRV_INTERRUPT_SCAN_STC lionIntrScanArr[LION_NUM_PORT_GROUPS_CNS][LION_NUM_ELEMENTS_IN_SCAN_TREE_CNS]=
{
    {{FILLED_IN_RUNTIME_CNS}}
};


/*******************************************************************************
* prvCpssDrvPpIntDefDxChLionInit
*
* DESCRIPTION:
*       Interrupts initialization for the Lion devices.
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
void prvCpssDrvPpIntDefDxChLionInit(void)
{
    GT_U32  portGroupId;/*port group Id*/
    GT_U32  ii;
    GT_STATUS   rc;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */
    GT_U32  dummyMaskRegAddr = 0x000000F4;/*user defined register*/

    /*
       1. fill the array of mask registers addresses
       2. fill the array of default values for the mask registers
       3. update the bits of nonSumBitMask in the scan tree
    */

    rc = prvCpssDrvExMxDxHwPpMaskRegInfoGet(
            CPSS_PP_FAMILY_DXCH_LION_E,
            LION_NUM_ELEMENTS_IN_SCAN_TREE_CNS,
            lionIntrScanArrSinglePortGroup,
            LION_NUM_MASK_REGISTERS_CNS,
            lionMaskRegDefaultSummaryArr,
            lionMaskRegMapArr);
    if(rc != GT_OK)
    {
        /* error */
        return ;
    }

    /* lion info , port group 0
       code will loop on all port groups */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_0_E];

    for(portGroupId = 0; portGroupId < LION_NUM_PORT_GROUPS_CNS; portGroupId++ ,
        devFamilyPtr++)
    {
        cpssOsMemCpy(lionMaskRegMapArr_perPortGroup[portGroupId],
                    lionMaskRegMapArr,
                    sizeof(lionMaskRegMapArr));

        for(ii = 0 ; ii < LION_NUM_MASK_REGISTERS_CNS; ii++)
        {
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            /* check if some addresses are not applicable in some port groups */
            if(GT_TRUE == prvCpssLionDuplicatedMultiPortGroupsGet(lionMaskRegMapArr[ii],&portGroupsBmp))
            {
                if(0 == (portGroupsBmp & (1 << portGroupId)))
                {
                    lionMaskRegMapArr_perPortGroup[portGroupId][ii] = dummyMaskRegAddr;
                }
            }
        }

        devFamilyPtr->numOfInterrupts = PRV_CPSS_LION_B_LAST_INT_E;

        devFamilyPtr->maskRegistersAddressesArray = lionMaskRegMapArr_perPortGroup[portGroupId];
        devFamilyPtr->maskRegistersDefaultValuesArray = lionMaskRegDefaultSummaryArr;
        devFamilyPtr->interruptsScanArray = &lionIntrScanArr[portGroupId][0];
        devFamilyPtr->numOfScanElements = LION_NUM_ELEMENTS_IN_SCAN_TREE_CNS;
        devFamilyPtr->fdbTrigEndedId = 0;/* don't care !!!! */
        devFamilyPtr->fdbTrigEndedCbPtr = NULL;
        devFamilyPtr->hasFakeInterrupts = GT_FALSE;
        devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
        devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
        devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
        devFamilyPtr->notAccessibleBeforeStartInitPtr=NULL;

        /* copy interrupts generic tree into the per port group tree */
        cpssOsMemCpy((PRV_CPSS_DRV_INTERRUPT_SCAN_STC *)devFamilyPtr->interruptsScanArray,/*casting to remove the 'const'*/
            lionIntrScanArrSinglePortGroup,
            LION_NUM_ELEMENTS_IN_SCAN_TREE_CNS * sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC));
    }
}

#ifdef DUMP_DEFAULT_INFO
/*******************************************************************************
* prvCpssDrvPpIntDefLionPrint
*
* DESCRIPTION:
*      print the interrupts arrays info, of lion devices
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
void  prvCpssDrvPpIntDefLionPrint(
    void
)
{
    cpssOsPrintf("Lion - start : \n");
    /* port group 0 */
    prvCpssDrvPpIntDefPrint(LION_NUM_ELEMENTS_IN_SCAN_TREE_CNS,lionIntrScanArrSinglePortGroup,
                            LION_NUM_MASK_REGISTERS_CNS,lionMaskRegDefaultSummaryArr,lionMaskRegMapArr);
    cpssOsPrintf("Lion - End : \n");

}
#endif/*DUMP_DEFAULT_INFO*/

