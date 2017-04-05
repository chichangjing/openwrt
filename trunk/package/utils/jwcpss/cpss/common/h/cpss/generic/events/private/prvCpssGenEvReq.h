/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenEvReq.h
*
* DESCRIPTION:
*       Includes unified CPSS private event routine.
*       this file is used to define bind between the cpssDriver of PP to
*       unify events , and for FA and for XBAR
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/
#ifndef __prvCpssGenEvReqh
#define __prvCpssGenEvReqh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/events/cpssGenEventCtrl.h>
#include <cpss/generic/events/cpssGenEventRequests.h>

/*
 * typedef: enum PRV_CPSS_DEVICE_TYPE_ENT
 *
 * Description: Enumeration of types of devices PP/FA/XBAR/DRAGONITE
 *
 *        PRV_CPSS_DEVICE_TYPE_PP_E - PP device
 *        PRV_CPSS_DEVICE_TYPE_FA_E - FA device
 *        PRV_CPSS_DEVICE_TYPE_XBAR_E - XBAR device
 *        PRV_CPSS_DEVICE_TYPE_DRAGONITE_E - DRAGONITE device
 *
 */
typedef enum{
    PRV_CPSS_DEVICE_TYPE_PP_E,
    PRV_CPSS_DEVICE_TYPE_FA_E,
    PRV_CPSS_DEVICE_TYPE_XBAR_E,
    PRV_CPSS_DEVICE_TYPE_DRAGONITE_E,

    PRV_CPSS_DEVICE_TYPE_LAST_E
}PRV_CPSS_DEVICE_TYPE_ENT;

#define STR(strname)    \
    #strname

#define UNI_EV_NAME                                                             \
    /* PCI */                                                                   \
STR(CPSS_PP_SLV_WRITE_ERR_E),  /* 0  */                                         \
STR(CPSS_PP_MAS_WRITE_ERR_E),  /* 1  */                                         \
STR(CPSS_PP_ADDR_ERR_E),  /* 2  */                                              \
STR(CPSS_PP_MAS_ABORT_E),  /* 3  */                                             \
STR(CPSS_PP_TARGET_ABORT_E),  /* 4  */                                          \
STR(CPSS_PP_SLV_READ_ERR_E),  /* 5  */                                          \
STR(CPSS_PP_MAS_READ_ERR_E),  /* 6  */                                          \
STR(CPSS_PP_RETRY_CNTR_E),  /* 7  */                                            \
                                                                                \
    /* Misc */                                                                  \
STR(CPSS_PP_MISC_TWSI_TIME_OUT_E),  /* 8  */                                    \
STR(CPSS_PP_MISC_TWSI_STATUS_E),  /* 9  */                                      \
STR(CPSS_PP_MISC_ILLEGAL_ADDR_E),  /* 10 */                                     \
STR(CPSS_PP_MISC_CPU_PORT_RX_OVERRUN_E),  /* 11 */                              \
STR(CPSS_PP_MISC_CPU_PORT_TX_OVERRUN_E),  /* 12 */                              \
STR(CPSS_PP_MISC_TX_CRC_PORT_E),  /* 13 */                                      \
STR(CPSS_PP_MISC_C2C_W_FAR_END_UP_E),  /* 14 */                                 \
STR(CPSS_PP_MISC_C2C_N_FAR_END_UP_E),  /* 15 */                                 \
STR(CPSS_PP_MISC_C2C_DATA_ERR_E),  /* 16 */                                     \
STR(CPSS_PP_MISC_MSG_TIME_OUT_E),  /* 17 */                                     \
STR(CPSS_PP_MISC_UPDATED_STAT_E),  /* 18 */                                     \
STR(CPSS_PP_GPP_E),  /* 19 */                                                   \
STR(CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E),  /* 20 */                             \
STR(CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E),  /* 21 */                           \
STR(CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E),  /* 22 */                             \
STR(CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E),  /* 23 */                           \
STR(CPSS_PP_MISC_PEX_ADDR_UNMAPPED_E),  /* 24 */                                \
STR(CPSS_PP_MISC_GENXS_READ_DMA_DONE_E),  /* 25 */                              \
                                                                                \
    /* Ingress Buffer Manager */                                                \
STR(CPSS_PP_BM_EMPTY_CLEAR_E),  /* 26 */                                        \
STR(CPSS_PP_BM_EMPTY_INC_E),  /* 27 */                                          \
STR(CPSS_PP_BM_AGED_PACKET_E),  /* 28 */                                        \
STR(CPSS_PP_BM_MAX_BUFF_REACHED_E),  /* 29 */                                   \
STR(CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E),  /* 30 */                       \
STR(CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E),  /* 31 */                        \
STR(CPSS_PP_BM_INVALID_ADDRESS_E),  /* 32 */                                    \
STR(CPSS_PP_BM_WRONG_SRC_PORT_E),  /* 33 */                                     \
STR(CPSS_PP_BM_MC_INC_OVERFLOW_E),  /* 34 */                                    \
STR(CPSS_PP_BM_MC_INC_UNDERRUN_E),  /* 35 */                                    \
STR(CPSS_PP_BM_TQ_PARITY_ERROR_PORT_E),  /* 36 */                               \
STR(CPSS_PP_BM_RX_MEM_READ_ECC_ERROR_E),  /* 37 */                              \
STR(CPSS_PP_BM_VLT_ECC_ERR_E),  /* 38 */                                        \
STR(CPSS_PP_BM_MISC_E),  /* 39 */                                               \
STR(CPSS_PP_BM_TRIGGER_AGING_DONE_E),  /* 40 */                                 \
STR(CPSS_PP_BM_PORT_RX_FULL_E),  /* 41 */                                       \
STR(CPSS_PP_BM_ALL_GIGA_PORTS_BUFF_LIMIT_REACHED_E),  /* 42 */                  \
STR(CPSS_PP_BM_ALL_HG_STACK_PORTS_BUFF_LIMIT_REACHED_E),  /* 43 */              \
                                                                                \
    /* Port */                                                                  \
STR(CPSS_PP_PORT_LINK_STATUS_CHANGED_E),  /* 44 */                              \
STR(CPSS_PP_PORT_AN_COMPLETED_E),  /* 45 */                                     \
STR(CPSS_PP_PORT_RX_FIFO_OVERRUN_E),  /* 46 */                                  \
STR(CPSS_PP_PORT_TX_FIFO_UNDERRUN_E),  /* 47 */                                 \
STR(CPSS_PP_PORT_TX_FIFO_OVERRUN_E),  /* 48 */                                  \
STR(CPSS_PP_PORT_TX_UNDERRUN_E),  /* 49 */                                      \
STR(CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E),  /* 50 */                             \
STR(CPSS_PP_PORT_PRBS_ERROR_E),  /* 51 */                                       \
STR(CPSS_PP_PORT_SYNC_STATUS_CHANGED_E),  /* 52 */                              \
STR(CPSS_PP_PORT_TX_CRC_ERROR_E),  /* 53 */                                     \
STR(CPSS_PP_PORT_ILLEGAL_SEQUENCE_E),  /* 54 */                                 \
STR(CPSS_PP_PORT_IPG_TOO_SMALL_E),  /* 55 */                                    \
STR(CPSS_PP_PORT_FAULT_TYPE_CHANGE_E),  /* 56 */                                \
STR(CPSS_PP_PORT_FC_STATUS_CHANGED_E),  /* 57 */                                \
STR(CPSS_PP_PORT_CONSECUTIVE_TERM_CODE_E),  /* 58 */                            \
                                                                                \
STR(CPSS_PP_INTERNAL_PHY_E),  /* 59 */                                          \
STR(CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E),  /* 60 */                              \
STR(CPSS_PP_PORT_XAUI_PHY_E),  /* 61*/                                          \
STR(CPSS_PP_PORT_COUNT_COPY_DONE_E),  /* 62*/                                   \
STR(CPSS_PP_PORT_COUNT_EXPIRED_E),  /* 63 */                                    \
                                                                                \
    /* Group Of Ports */                                                        \
STR(CPSS_PP_GOP_ADDRESS_OUT_OF_RANGE_E),  /* 64 */                              \
STR(CPSS_PP_GOP_COUNT_EXPIRED_E),  /* 65 */                                     \
STR(CPSS_PP_GOP_COUNT_COPY_DONE_E),  /* 66 */                                   \
STR(CPSS_PP_GOP_GIG_BAD_FC_PACKET_GOOD_CRC_E),  /* 67 */                        \
STR(CPSS_PP_GOP_XG_BAD_FC_PACKET_GOOD_CRC_E),  /* 68 */                         \
STR(CPSS_PP_XSMI_WRITE_DONE_E),  /* 69 */                                       \
                                                                                \
    /* Tx Queue */                                                              \
STR(CPSS_PP_TQ_WATCHDOG_EX_PORT_E),  /* 70 */                                   \
STR(CPSS_PP_TQ_TXQ2_FLUSH_PORT_E),  /* 71 */                                    \
STR(CPSS_PP_TQ_TXQ2_MG_FLUSH_E),  /* 72 */                                      \
STR(CPSS_PP_TQ_ONE_ECC_ERROR_E),  /* 73 */                                      \
STR(CPSS_PP_TQ_TWO_ECC_ERROR_E),  /* 74 */                                      \
STR(CPSS_PP_TQ_MG_READ_ERR_E),  /* 75 */                                        \
STR(CPSS_PP_TQ_HOL_REACHED_PORT_E),  /* 76 */                                   \
STR(CPSS_PP_TQ_RED_REACHED_PORT_E),  /* 77 */                                   \
STR(CPSS_PP_TQ_TOTAL_DESC_UNDERFLOW_E),  /* 78 */                               \
STR(CPSS_PP_TQ_TOTAL_DESC_OVERFLOW_E),  /* 79 */                                \
STR(CPSS_PP_TQ_TOTAL_BUFF_UNDERFLOW_E),  /* 80 */                               \
STR(CPSS_PP_TQ_SNIFF_DESC_DROP_E),  /* 81 */                                    \
STR(CPSS_PP_TQ_MLL_PARITY_ERR_E),  /* 82 */                                     \
STR(CPSS_PP_TQ_MC_FIFO_OVERRUN_E),  /* 83 */                                    \
STR(CPSS_PP_TQ_MISC_E),  /* 84 */                                               \
STR(CPSS_PP_TQ_PORT_DESC_FULL_E),  /* 85 */                                     \
STR(CPSS_PP_TQ_MC_DESC_FULL_E),  /* 86 */                                       \
STR(CPSS_PP_TQ_GIGA_FIFO_FULL_E),  /* 87 */                                     \
STR(CPSS_PP_TQ_XG_MC_FIFO_FULL_E),  /* 88 */                                    \
STR(CPSS_PP_TQ_PORT_FULL_XG_E),  /* 89 */                                       \
                                                                                \
    /* Ethernet Bridge */                                                       \
STR(CPSS_PP_EB_AUQ_FULL_E),  /*  90 */                                          \
STR(CPSS_PP_EB_AUQ_PENDING_E),  /*  91 */                                       \
STR(CPSS_PP_EB_AUQ_OVER_E),  /*  92 */                                          \
STR(CPSS_PP_EB_AUQ_ALMOST_FULL_E),  /*  93 */                                   \
STR(CPSS_PP_EB_FUQ_FULL_E),  /*  94 */                                          \
STR(CPSS_PP_EB_FUQ_PENDING_E),  /*  95 */                                       \
STR(CPSS_PP_EB_NA_FIFO_FULL_E),  /*  96 */                                      \
STR(CPSS_PP_EB_MG_ADDR_OUT_OF_RANGE_E),  /*  97 */                              \
STR(CPSS_PP_EB_VLAN_TBL_OP_DONE_E),  /*  98 */                                  \
STR(CPSS_PP_EB_SECURITY_BREACH_UPDATE_E),  /*  99 */                            \
STR(CPSS_PP_EB_VLAN_SECURITY_BREACH_E),  /* 100 */                              \
STR(CPSS_PP_EB_NA_NOT_LEARNED_SECURITY_BREACH_E),  /* 101 */                    \
STR(CPSS_PP_EB_SA_MSG_DISCARDED_E),  /* 102 */                                  \
STR(CPSS_PP_EB_QA_MSG_DISCARDED_E),  /* 103 */                                  \
STR(CPSS_PP_EB_SA_DROP_SECURITY_BREACH_E),  /* 104 */                           \
STR(CPSS_PP_EB_DA_DROP_SECURITY_BREACH_E),  /* 105 */                           \
STR(CPSS_PP_EB_DA_SA_DROP_SECURITY_BREACH_E),  /* 106 */                        \
STR(CPSS_PP_EB_NA_ON_LOCKED_DROP_SECURITY_BREACH_E),  /* 107 */                 \
STR(CPSS_PP_EB_MAC_RANGE_DROP_SECURITY_BREACH_E),  /* 108 */                    \
STR(CPSS_PP_EB_INVALID_SA_DROP_SECURITY_BREACH_E),  /* 109 */                   \
STR(CPSS_PP_EB_VLAN_NOT_VALID_DROP_SECURITY_BREACH_E),  /* 110 */               \
STR(CPSS_PP_EB_VLAN_NOT_MEMBER_DROP_DROP_SECURITY_BREACH_E),  /* 111 */         \
STR(CPSS_PP_EB_VLAN_RANGE_DROP_SECURITY_BREACH_E),  /* 112 */                   \
STR(CPSS_PP_EB_INGRESS_FILTER_PCKT_E),  /* 113 */                               \
STR(CPSS_PP_MAC_TBL_READ_ECC_ERR_E),  /* 114 */                                 \
STR(CPSS_PP_MAC_SFLOW_E),  /* 115 */                                            \
STR(CPSS_PP_MAC_NUM_OF_HOP_EXP_E),  /* 116 */                                   \
STR(CPSS_PP_MAC_NA_LEARNED_E),  /* 117 */                                       \
STR(CPSS_PP_MAC_NA_NOT_LEARNED_E),  /* 118 */                                   \
STR(CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E),  /* 119 */                            \
STR(CPSS_PP_MAC_MG_ADDR_OUT_OF_RANGE_E),  /* 120 */                             \
STR(CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E),  /* 121 */                             \
STR(CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E),  /* 122 */                             \
STR(CPSS_PP_MAC_NA_SELF_LEARNED_E),  /* 123 */                                  \
STR(CPSS_PP_MAC_NA_FROM_CPU_LEARNED_E),  /* 124 */                              \
STR(CPSS_PP_MAC_NA_FROM_CPU_DROPPED_E),  /* 125 */                              \
STR(CPSS_PP_MAC_AGED_OUT_E),  /* 126 */                                         \
STR(CPSS_PP_MAC_FIFO_2_CPU_EXCEEDED_E),  /* 127 */                              \
STR(CPSS_PP_MAC_1ECC_ERRORS_E),  /* 128 */                                      \
STR(CPSS_PP_MAC_2ECC_ERRORS_E),  /* 129 */                                      \
STR(CPSS_PP_MAC_TBL_OP_DONE_E),  /* 130 */                                      \
STR(CPSS_PP_MAC_PACKET_RSSI_LESS_THAN_THRESH_E),  /* 131 */                     \
STR(CPSS_PP_MAC_UPD_AVG_RSSI_LESS_THAN_THRESH_E),  /* 132 */                    \
STR(CPSS_PP_MAC_STG_TBL_DATA_ERROR_E),  /* 133 */                               \
STR(CPSS_PP_MAC_VIDX_TBL_DATA_ERROR_E),  /* 134*/                               \
STR(CPSS_PP_LX_LB_ERR_E),  /* 135 */                                            \
STR(CPSS_PP_LX_CTRL_MEM_2_RF_ERR_E),  /* 136 */                                 \
STR(CPSS_PP_LX_TCB_CNTR_E),  /* 137 */                                          \
STR(CPSS_PP_LX_IPV4_MC_ERR_E),  /* 138 */                                       \
STR(CPSS_PP_LX_IPV4_LPM_ERR_E),  /* 139 */                                      \
STR(CPSS_PP_LX_IPV4_ROUTE_ERR_E),  /* 140 */                                    \
STR(CPSS_PP_LX_IPV4_CNTR_E),  /* 141 */                                         \
STR(CPSS_PP_LX_L3_L7_ERR_ADDR_E),  /* 142 */                                    \
STR(CPSS_PP_LX_TRUNK_ADDR_OUT_OF_RANGE_E),  /* 143 */                           \
STR(CPSS_PP_LX_IPV4_REFRESH_AGE_OVERRUN_E),  /* 144 */                          \
STR(CPSS_PP_LX_PCE_PAR_ERR_E),  /* 145 */                                       \
STR(CPSS_PP_LX_TC_2_RF_CNTR_ALRM_E),  /* 146 */                                 \
STR(CPSS_PP_LX_TC_2_RF_PLC_ALRM_E),  /* 147 */                                  \
STR(CPSS_PP_LX_TC_2_RF_TBL_ERR_E),  /* 148 */                                   \
STR(CPSS_PP_LX_CLASSIFIER_HASH_PAR_ERR_E),  /* 149 */                           \
STR(CPSS_PP_LX_FLOW_LKUP_PAR_ERR_E),  /* 150 */                                 \
STR(CPSS_PP_LX_FLOW_KEY_TBL_PAR_ERR_E),  /* 151 */                              \
STR(CPSS_PP_LX_MPLS_ILM_TBL_PAR_ERR_E),  /* 152 */                              \
STR(CPSS_PP_LX_MPLS_CNTR_E),  /* 153 */                                         \
STR(CPSS_PP_EB_TCC_E),  /* 154 */                                               \
                                                                                \
    /* Network Interface */                                                     \
STR(CPSS_PP_RX_BUFFER_QUEUE0_E),  /* 155 */                                     \
STR(CPSS_PP_RX_BUFFER_QUEUE1_E),  /* 156 */                                     \
STR(CPSS_PP_RX_BUFFER_QUEUE2_E),  /* 157 */                                     \
STR(CPSS_PP_RX_BUFFER_QUEUE3_E),  /* 158 */                                     \
STR(CPSS_PP_RX_BUFFER_QUEUE4_E),  /* 159 */                                     \
STR(CPSS_PP_RX_BUFFER_QUEUE5_E),  /* 160 */                                     \
STR(CPSS_PP_RX_BUFFER_QUEUE6_E),  /* 161 */                                     \
STR(CPSS_PP_RX_BUFFER_QUEUE7_E),  /* 162 */                                     \
STR(CPSS_PP_RX_ERR_QUEUE0_E),  /* 163 */                                        \
STR(CPSS_PP_RX_ERR_QUEUE1_E),  /* 164 */                                        \
STR(CPSS_PP_RX_ERR_QUEUE2_E),  /* 165 */                                        \
STR(CPSS_PP_RX_ERR_QUEUE3_E),  /* 166 */                                        \
STR(CPSS_PP_RX_ERR_QUEUE4_E),  /* 167 */                                        \
STR(CPSS_PP_RX_ERR_QUEUE5_E),  /* 168 */                                        \
STR(CPSS_PP_RX_ERR_QUEUE6_E),  /* 169 */                                        \
STR(CPSS_PP_RX_ERR_QUEUE7_E),  /* 170 */                                        \
STR(CPSS_PP_RX_CNTR_OVERFLOW_E),  /* 171 */                                     \
STR(CPSS_PP_TX_BUFFER_QUEUE_E),  /* 172 */                                      \
STR(CPSS_PP_TX_ERR_QUEUE_E),  /* 173 */                                         \
STR(CPSS_PP_TX_END_E),  /* 174 */                                               \
                                                                                \
    /* Per Port Per lane events */                                              \
STR(CPSS_PP_PORT_LANE_PRBS_ERROR_E),  /* 175 */                                 \
STR(CPSS_PP_PORT_LANE_DISPARITY_ERROR_E),  /* 176 */                            \
STR(CPSS_PP_PORT_LANE_SYMBOL_ERROR_E),  /* 177 */                               \
STR(CPSS_PP_PORT_LANE_CJR_PAT_ERROR_E),  /* 178 */                              \
STR(CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E),  /* 179 */                      \
STR(CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E),  /* 180 */                        \
STR(CPSS_PP_PORT_LANE_DETECTED_IIAII_E),  /* 181 */                             \
                                                                                \
    /* Per Port PCS events */                                                   \
STR(CPSS_PP_PORT_PCS_LINK_STATUS_CHANGED_E),  /* 182 */                         \
STR(CPSS_PP_PORT_PCS_DESKEW_TIMEOUT_E),  /* 183 */                              \
STR(CPSS_PP_PORT_PCS_DETECTED_COLUMN_IIAII_E),  /* 184 */                       \
STR(CPSS_PP_PORT_PCS_DESKEW_ERROR_E),  /* 185 */                                \
STR(CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E),  /* 186 */                           \
STR(CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E),  /* 187 */                            \
STR(CPSS_PP_PORT_PCS_ALIGN_LOCK_LOST_E),  /* 188 */                             \
                                                                                \
    /* Egress Buffer Manager */                                                 \
STR(CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E),  /* 189 */                                \
STR(CPSS_PP_BM_EGRESS_EMPTY_INC_E),  /* 190 */                                  \
STR(CPSS_PP_BM_EGRESS_AGED_PACKET_E),  /* 191 */                                \
STR(CPSS_PP_BM_EGRESS_MAX_BUFF_REACHED_E),  /* 192 */                           \
STR(CPSS_PP_BM_EGRESS_PORT_RX_BUFFERS_CNT_UNDERRUN_E),  /* 193 */               \
STR(CPSS_PP_BM_EGRESS_PORT_RX_BUFFERS_CNT_OVERRUN_E),  /* 194 */                \
STR(CPSS_PP_BM_EGRESS_INVALID_ADDRESS_E),  /* 195 */                            \
STR(CPSS_PP_BM_EGRESS_WRONG_SRC_PORT_E),  /* 196 */                             \
STR(CPSS_PP_BM_EGRESS_MC_INC_OVERFLOW_E),  /* 197 */                            \
STR(CPSS_PP_BM_EGRESS_MC_INC_UNDERRUN_E),  /* 198 */                            \
STR(CPSS_PP_BM_EGRESS_TQ_PARITY_ERROR_PORT_E),  /* 199 */                       \
STR(CPSS_PP_BM_EGRESS_RX_MEM_READ_ECC_ERROR_E),  /* 200 */                      \
STR(CPSS_PP_BM_EGRESS_VLT_ECC_ERR_E),  /* 201 */                                \
STR(CPSS_PP_BM_EGRESS_MISC_E),  /* 202 */                                       \
                                                                                \
    /************************** PEX events *********************************/   \
STR(CPSS_PP_PEX_DL_DOWN_TX_ACC_ERR_E),  /* 203 */                               \
STR(CPSS_PP_PEX_MASTER_DISABLED_E),  /* 204 */                                  \
STR(CPSS_PP_PEX_ERROR_WR_TO_REG_E),  /* 205 */                                  \
STR(CPSS_PP_PEX_HIT_DEFAULT_WIN_ERR_E),  /* 206 */                              \
STR(CPSS_PP_PEX_COR_ERROR_DET_E),  /* 207 */                                    \
STR(CPSS_PP_PEX_NON_FATAL_ERROR_DET_E),  /* 208 */                              \
STR(CPSS_PP_PEX_FATAL_ERROR_DET_E),  /* 209 */                                  \
STR(CPSS_PP_PEX_DSTATE_CHANGED_E),  /* 210 */                                   \
STR(CPSS_PP_PEX_BIST_E),  /* 211 */                                             \
STR(CPSS_PP_PEX_RCV_ERROR_FATAL_E),  /* 212 */                                  \
STR(CPSS_PP_PEX_RCV_ERROR_NON_FATAL_E),  /* 213 */                              \
STR(CPSS_PP_PEX_RCV_ERROR_COR_E),  /* 214 */                                    \
STR(CPSS_PP_PEX_RCV_CRS_E),  /* 215 */                                          \
STR(CPSS_PP_PEX_PEX_SLAVE_HOT_RESET_E),  /* 216 */                              \
STR(CPSS_PP_PEX_PEX_SLAVE_DISABLE_LINK_E),  /* 217 */                           \
STR(CPSS_PP_PEX_PEX_SLAVE_LOOPBACK_E),  /* 218 */                               \
STR(CPSS_PP_PEX_PEX_LINK_FAIL_E),  /* 219 */                                    \
STR(CPSS_PP_PEX_RCV_A_E),  /* 220 */                                            \
STR(CPSS_PP_PEX_RCV_B_E),  /* 221 */                                            \
STR(CPSS_PP_PEX_RCV_C_E),  /* 222 */                                            \
STR(CPSS_PP_PEX_RCV_D_E),  /* 223 */                                            \
                                                                                \
    /******************** Buffer Memory events ****************************/    \
STR(CPSS_PP_BUF_MEM_MAC_ERROR_E),  /* 224 */                                    \
STR(CPSS_PP_BUF_MEM_ONE_ECC_ERROR_E),  /* 225 */                                \
STR(CPSS_PP_BUF_MEM_TWO_OR_MORE_ECC_ERRORS_E),  /* 226 */                       \
                                                                                \
                                                                                \
    /************************** Policer events *****************************/   \
STR(CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E),  /* 227 */                           \
STR(CPSS_PP_POLICER_DATA_ERR_E),  /* 228 */                                     \
STR(CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E),  /* 229 */                            \
STR(CPSS_PP_POLICER_IPFIX_ALARM_E),  /* 230 */                                  \
STR(CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E),  /* 231 */       \
                                                                                \
    /************************** Policy Engine events ***********************/   \
STR(CPSS_PP_PCL_TCC_ECC_ERR_E),  /* 232 */                                      \
STR(CPSS_PP_PCL_MG_ADDR_OUT_OF_RANGE_E),  /* 233 */                             \
STR(CPSS_PP_PCL_LOOKUP_DATA_ERROR_E),  /* 234 */                                \
STR(CPSS_PP_PCL_ACTION_ERROR_DETECTED_E),  /* 235 */                            \
STR(CPSS_PP_PCL_MG_LOOKUP_RESULTS_READY_E),  /* 236 */                          \
STR(CPSS_PP_PCL_INLIF_TABLE_DATA_ERROR_E),  /* 237 */                           \
STR(CPSS_PP_PCL_CONFIG_TABLE_DATA_ERROR_E),  /* 238 */                          \
STR(CPSS_PP_PCL_LOOKUP_FIFO_FULL_E),  /* 239 */                                 \
                                                                                \
    /**************** Centralized Counters (CNC) events *********************/  \
STR(CPSS_PP_CNC_WRAPAROUND_BLOCK_E),  /* 240 */                                 \
STR(CPSS_PP_CNC_DUMP_FINISHED_E),  /* 241 */                                    \
                                                                                \
    /************************** SCT *****************************************/  \
STR(CPSS_PP_SCT_RATE_LIMITER_E),  /* 242 */                                     \
                                                                                \
    /************************** Egress and Ingress SFLOW (STC)***************/  \
STR(CPSS_PP_EGRESS_SFLOW_E),  /* 243 */                                         \
STR(CPSS_PP_INGRESS_SFLOW_SAMPLED_CNTR_E),  /* 244 */                           \
STR(CPSS_PP_EGRESS_SFLOW_SAMPLED_CNTR_E),  /* 245 */                            \
                                                                                \
    /************************** TTI events *****************************/       \
STR(CPSS_PP_TTI_CPU_ADDRESS_OUT_OF_RANGE_E),  /* 246 */                         \
STR(CPSS_PP_TTI_ACCESS_DATA_ERROR_E),  /* 247 */                                \
                                                                                \
    /************************** DIT events *****************************/       \
STR(CPSS_PP_DIT_MC_DIT_TTL_TRHOLD_EXCEED_E),  /* 248 */                         \
STR(CPSS_PP_DIT_UC_DIT_TTL_TRHOLD_EXCEED_E),  /* 249 */                         \
STR(CPSS_PP_DIT_DATA_ERROR_E),  /* 250 */                                       \
STR(CPSS_PP_DIT_MC0_PARITY_ERROR_E),  /* 251 */                                 \
STR(CPSS_PP_DIT_INT_TABLE_ECC_ERROR_CNTR_E),  /* 252 */                         \
STR(CPSS_PP_DIT_MC0_IP_TV_PARITY_ERROR_CNTR_E),  /* 253 */                      \
                                                                                \
    /************************** External TCAM events ***********************/   \
STR(CPSS_PP_EXTERNAL_TCAM_INT_SUM_E),  /* 254 */                                \
STR(CPSS_PP_EXTERNAL_TCAM_IO_ERR_E),  /* 255 */                                 \
STR(CPSS_PP_EXTERNAL_TCAM_RX_PARITY_ERR_E),  /* 256 */                          \
STR(CPSS_PP_EXTERNAL_TCAM_MG_CMD_EXE_DONE_E),  /* 257 */                        \
STR(CPSS_PP_EXTERNAL_TCAM_NEW_DATA_IN_MAILBOX_E),  /* 258 */                    \
STR(CPSS_PP_EXTERNAL_TCAM_MG_WRONG_EXP_RES_LEN_E),  /* 259 */                   \
STR(CPSS_PP_EXTERNAL_TCAM_RSP_IDQ_EMPTY_E),  /* 260 */                          \
STR(CPSS_PP_EXTERNAL_TCAM_PHASE_ERROR_E),  /* 261 */                            \
STR(CPSS_PP_EXTERNAL_TCAM_TRAINING_SEQUENCE_DONE_E),  /* 262 */                 \
                                                                                \
    /******************** External Memory events ******************** */        \
STR(CPSS_PP_EXTERNAL_MEMORY_PARITY_ERROR_E),  /* 263 */                         \
STR(CPSS_PP_EXTERNAL_MEMORY_ECC_ERROR_E),  /* 264 */                            \
STR(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_LOW_E),  /* 265 */           \
STR(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_HIGH_E),  /* 266 */          \
STR(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_MG_CMD_DONE_E),  /* 267 */              \
                                                                                \
                                                                                \
    /******************** PP VOQ events ****************************/           \
STR(CPSS_PP_VOQ_DROP_DEV_EN_E),  /* 268 */                                      \
STR(CPSS_PP_VOQ_GLOBAL_DESC_FULL_E),  /* 269 */                                 \
STR(CPSS_PP_VOQ_ADDR_OUT_OF_RANGE_E),  /* 270 */                                \
STR(CPSS_PP_VOQ_ECC_ERR_E),  /* 271 */                                          \
STR(CPSS_PP_VOQ_GLOBAL_BUF_FULL_E),  /* 272 */                                  \
                                                                                \
    /************************** PP XBAR events ******************************/  \
STR(CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_RL_E),  /* 273 */                        \
STR(CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_RL_E),  /* 274 */                         \
STR(CPSS_PP_XBAR_RX_CELL_CNTR_RL_E),  /* 275 */                                 \
STR(CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL_E),  /* 276 */                    \
STR(CPSS_PP_XBAR_SHP_TX_DROP_CNTR_RL_E),  /* 277 */                             \
STR(CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_RL_E),  /* 278 */                         \
STR(CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_INC_E),  /* 279 */                        \
STR(CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_INC_E),  /* 280 */                       \
STR(CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_INC_E),  /* 281 */                        \
STR(CPSS_PP_XBAR_RX_CELL_CNTR_INC_E),  /* 282 */                                \
STR(CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC_E),  /* 283 */                   \
STR(CPSS_PP_XBAR_SHP_TX_DROP_CNTR_INC_E),  /* 284 */                            \
STR(CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF_E),  /* 285 */              \
STR(CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON_E),  /* 286 */               \
STR(CPSS_PP_XBAR_INVALID_ADDR_E),  /* 287 */                                    \
STR(CPSS_PP_XBAR_TX_CELL_CNTR_RL_E),  /* 288 */                                 \
STR(CPSS_PP_XBAR_TX_CELL_CNTR_INC_E),  /* 289 */                                \
                                                                                \
    /******************** PP Fabric Reassembly engine events ****************/  \
STR(CPSS_PP_CRX_MAIL_ARRIVED_E),  /* 290 */                                     \
STR(CPSS_PP_CRX_READ_EMPTY_CPU_E),  /* 291 */                                   \
STR(CPSS_PP_CRX_DATA_DROP_E),  /* 292 */                                        \
STR(CPSS_PP_CRX_CPU_DROP_E),  /* 293 */                                         \
STR(CPSS_PP_CRX_AGED_OUT_CONTEXT_E),  /* 294 */                                 \
STR(CPSS_PP_CRX_CRC_DROP_E),  /* 295 */                                         \
STR(CPSS_PP_CRX_PACKET_LENGTH_DROP_E),  /* 296 */                               \
STR(CPSS_PP_CRX_MAX_BUFFERS_DROP_E),  /* 297 */                                 \
STR(CPSS_PP_CRX_BAD_CONTEXT_DROP_E),  /* 298 */                                 \
STR(CPSS_PP_CRX_NO_BUFFERS_DROP_E),  /* 299 */                                  \
STR(CPSS_PP_CRX_MULTICAST_CONGESTION_DROP_E),  /* 300 */                        \
STR(CPSS_PP_CRX_DATA_FIFO_UNDERRUN_E),  /* 301 */                               \
STR(CPSS_PP_CRX_DATA_FIFO_OVERRUN_E),  /* 302 */                                \
STR(CPSS_PP_CRX_CPU_UNDERRUN_E),  /* 303 */                                     \
STR(CPSS_PP_CRX_CPU_OVERRUN_E),  /* 304 */                                      \
STR(CPSS_PP_CRX_E2E_FIFO_UNDERRUN_E),  /* 305 */                                \
STR(CPSS_PP_CRX_E2E_FIFO_OVERRUN_E),  /* 306 */                                 \
STR(CPSS_PP_CRX_UNKNOWN_CELL_TYPE_E),  /* 307 */                                \
STR(CPSS_PP_CRX_COUNTER_BAD_ADDR_E),  /* 308 */                                 \
STR(CPSS_PP_CRX_PSM_BAD_ADDR_E),  /* 309 */                                     \
                                                                                \
    /*************** PP Fabric Segmentation engine events *******************/  \
STR(CPSS_PP_TXD_SEG_FIFO_PARITY_ERR_E),  /* 310 */                              \
STR(CPSS_PP_TXD_CPU_MAIL_SENT_E),  /* 311 */                                    \
STR(CPSS_PP_TXD_SEG_FIFO_OVERRUN_E),  /* 312 */                                 \
STR(CPSS_PP_TXD_WRONG_CH_TO_FABRIC_DIST_E),  /* 313 */                          \
                                                                                \
    /************************** IPVX events ********************************/   \
STR(CPSS_PP_IPVX_DATA_ERROR_E),  /* 314 */                                      \
STR(CPSS_PP_IPVX_ADDRESS_OUT_OF_MEMORY_E),  /* 315 */                           \
STR(CPSS_PP_IPVX_LPM_DATA_ERROR_E),  /* 316 */                                  \
                                                                                \
    /************************** Statistic Interface *************************/  \
STR(CPSS_PP_STAT_INF_TX_SYNC_FIFO_FULL_E),  /* 317 */                           \
STR(CPSS_PP_STAT_INF_TX_SYNC_FIFO_OVERRUN_E),  /* 318 */                        \
STR(CPSS_PP_STAT_INF_TX_SYNC_FIFO_UNDERRUN_E),  /* 319 */                       \
STR(CPSS_PP_STAT_INF_RX_SYNC_FIFO_FULL_E),  /* 320 */                           \
STR(CPSS_PP_STAT_INF_RX_SYNC_FIFO_OVERRUN_E),  /* 321 */                        \
STR(CPSS_PP_STAT_INF_RX_SYNC_FIFO_UNDERRUN_E),  /* 322 */                       \
                                                                                \
    /******************** PP HyperG Link ports events ***********************/  \
STR(CPSS_PP_HGLINK_PING_RECEIVED_E),  /* 323 */                                 \
STR(CPSS_PP_HGLINK_PING_SENT_E),  /* 324 */                                     \
STR(CPSS_PP_HGLINK_MAC_TX_OVERRUN_E),  /* 325 */                                \
STR(CPSS_PP_HGLINK_MAC_TX_UNDERRUN_E),  /* 326 */                               \
STR(CPSS_PP_HGLINK_RX_ERR_CNTR_RL_E),  /* 327 */                                \
STR(CPSS_PP_HGLINK_BAD_PCS_TO_MAC_REFORMAT_RX_E),  /* 328 */                    \
STR(CPSS_PP_HGLINK_BAD_HEAD_CELL_RX_E),  /* 329 */                              \
STR(CPSS_PP_HGLINK_BAD_LENGTH_CELL_RX_E),  /* 330 */                            \
STR(CPSS_PP_HGLINK_FC_CELL_RX_INT_E),  /* 331 */                                \
STR(CPSS_PP_HGLINK_FC_CELL_TX_INT_E),  /* 332 */                                \
STR(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC0_E),  /* 333 */                         \
STR(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC1_E),  /* 334 */                         \
STR(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC2_E),  /* 335 */                         \
STR(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC3_E),  /* 336 */                         \
STR(CPSS_PP_HGLINK_ADDR_OUT_OF_RANGE_E),  /* 337 */                             \
                                                                                \
    /**************************** TCC-TCAM *********************************/   \
STR(CPSS_PP_TCC_TCAM_ERROR_DETECTED_E),  /* 338 */                              \
STR(CPSS_PP_TCC_TCAM_BIST_FAILED_E),  /* 339 */                                 \
    /**************************** BCN **************************************/   \
STR(CPSS_PP_BCN_COUNTER_WRAP_AROUND_ERR_E),  /* 340 */                          \
                                                                                \
    /**************************** GTS (ingress/egress time stamp) **********/   \
STR(CPSS_PP_GTS_GLOBAL_FIFO_FULL_E),  /* 341 */                                 \
STR(CPSS_PP_GTS_VALID_TIME_SAMPLE_MESSAGE_E),  /* 342 */                        \
                                                                                \
    /**************************** Critical HW Error ***************************/\
STR(CPSS_PP_CRITICAL_HW_ERROR_E_E),  /* 343 */                                  \
                                                                                \
    /**************************** Port related addition ***********************/\
STR(CPSS_PP_PORT_PRBS_ERROR_QSGMII_E),  /* 344 */                               \
STR(CPSS_PP_PORT_802_3_AP_E),           /* 345 */                               \
STR(CPSS_PP_PORT_802_3_AP_CORE_1_E),           /* 346 */                        \
STR(CPSS_PP_PORT_802_3_AP_CORE_2_E),           /* 347 */                        \
STR(CPSS_PP_PORT_802_3_AP_CORE_3_E),           /* 348 */                        \
STR(CPSS_PP_PORT_802_3_AP_CORE_4_E),           /* 349 */                        \
STR(CPSS_PP_PORT_802_3_AP_CORE_5_E),           /* 350 */                        \
STR(CPSS_PP_PORT_802_3_AP_CORE_6_E),           /* 351 */                        \
STR(CPSS_PP_PORT_802_3_AP_CORE_7_E),           /* 352 */                        \
STR(CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E), /* 353 */                       \
                                                                                \
    /**************************** DFX Server related events *******************/\
STR(CPSS_PP_DATA_INTEGRITY_ERROR_E),           /* 354 */                        \
    /**************************** MAC table (FDB) addition ********************/\
STR(CPSS_PP_MAC_BANK_LEARN_COUNTERS_OVERFLOW_E),  /* 355 */                     \
                                                                                \
STR(CPSS_PP_PORT_EEE_E),                          /* 356 */                     \
                                                                                \
STR(CPSS_PP_OAM_EXCEPTION_KEEPALIVE_AGING_E),     /* 357 */                     \
STR(CPSS_PP_OAM_EXCEPTION_EXCESS_KEEPALIVE_E),    /* 358 */                     \
STR(CPSS_PP_OAM_EXCEPTION_INVALID_KEEPALIVE_E),   /* 359 */                     \
STR(CPSS_PP_OAM_EXCEPTION_RDI_STATUS_E),          /* 360 */                     \
STR(CPSS_PP_OAM_EXCEPTION_TX_PERIOD_E),           /* 361 */                     \
STR(CPSS_PP_OAM_EXCEPTION_MEG_LEVEL_E),           /* 362 */                     \
STR(CPSS_PP_OAM_EXCEPTION_SOURCE_INTERFACE_E),    /* 363 */                     \
STR(CPSS_PP_OAM_ILLEGAL_ENTRY_INDEX_E),           /* 364 */                     \
                                                                                \
    /************************** Xbar events ********************************/   \
STR(CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E),           /* 365 */                      \
STR(CPSS_XBAR_HG_LANE_COMMA_ALIGN_CHANGE_E),     /* 366 */                      \
STR(CPSS_XBAR_GPORT_INVALID_TRG_DROP_CNT_INC_E), /* 367 */                      \
STR(CPSS_XBAR_GPORT_DIAG_CELL_RX_CNTR_INC_E),    /* 368 */                      \
STR(CPSS_XBAR_MISC_E),                           /* 369 */                      \
STR(CPSS_XBAR_HG_PING_RECEIVED_E),               /* 370 */                      \
STR(CPSS_XBAR_HOST_RX_FIFO_PEND_E),              /* 371 */                      \
STR(CPSS_XBAR_HOST_SEND_CELL_E),                 /* 372 */                      \
STR(CPSS_XBAR_HG_ALIGNMENT_LOCK_CHANGE_E),       /* 373 */                      \
STR(CPSS_XBAR_HG_PING_E),                        /* 374 */                      \
STR(CPSS_XBAR_SD_INV_ADDR_E),                    /* 375 */                      \
STR(CPSS_XBAR_SXBAR_INV_ADDR_E),                 /* 376 */                      \
STR(CPSS_XBAR_S_INV_ADDR_E),                     /* 377 */                      \
STR(CPSS_XBAR_G_INV_ADDR_E),                     /* 378 */                      \
STR(CPSS_XBAR_MC_INV_ADDR_E),                    /* 379 */                      \
                                                                                \
    /************************** Fa events **********************************/   \
STR(CPSS_FA_UC_PRIO_BUF_FULL_E),    /* 380 */                                   \
STR(CPSS_FA_MC_PRIO_BUF_FULL_E),    /* 381 */                                   \
STR(CPSS_FA_UC_RX_BUF_FULL_E),      /* 382 */                                   \
STR(CPSS_FA_MC_RX_BUF_FULL_E),      /* 383 */                                   \
STR(CPSS_FA_WD_FPORT_GRP_E),        /* 384 */                                   \
STR(CPSS_FA_MC_WD_EXP_E),           /* 385 */                                   \
STR(CPSS_FA_GLBL_DESC_FULL_E),      /* 386 */                                   \
STR(CPSS_FA_MAC_UPD_THRS_E),        /* 387 */                                   \
STR(CPSS_FA_DROP_DEV_EN_E),         /* 388 */                                   \
STR(CPSS_FA_MISC_I2C_TIMEOUT_E),    /* 389 */                                   \
STR(CPSS_FA_MISC_STATUS_E),         /* 390 */                                   \
                                                                                \
    /************************** DRAGONITE events ***************************/   \
STR(CPSS_DRAGONITE_PORT_ON_E),                /* 391 */                         \
STR(CPSS_DRAGONITE_PORT_OFF_E),               /* 392 */                         \
STR(CPSS_DRAGONITE_DETECT_FAIL_E),            /* 393 */                         \
STR(CPSS_DRAGONITE_PORT_FAULT_E),             /* 394 */                         \
STR(CPSS_DRAGONITE_PORT_UDL_E),               /* 395 */                         \
STR(CPSS_DRAGONITE_PORT_FAULT_ON_STARTUP_E),  /* 396 */                         \
STR(CPSS_DRAGONITE_PORT_PM_E),                /* 397 */                         \
STR(CPSS_DRAGONITE_POWER_DENIED_E),           /* 398 */                         \
STR(CPSS_DRAGONITE_OVER_TEMP_E),              /* 399 */                         \
STR(CPSS_DRAGONITE_TEMP_ALARM_E),             /* 400 */                         \
STR(CPSS_DRAGONITE_DEVICE_FAULT_E),           /* 401 */                         \
STR(CPSS_DRAGONITE_OVER_CONSUM_E),            /* 402 */                         \
STR(CPSS_DRAGONITE_VMAIN_LOW_AF_E),           /* 403 */                         \
STR(CPSS_DRAGONITE_VMAIN_LOW_AT_E),           /* 404 */                         \
STR(CPSS_DRAGONITE_VMAIN_HIGH_E),             /* 405 */                         \
STR(CPSS_DRAGONITE_READ_EVENT_E),             /* 406 */                         \
STR(CPSS_DRAGONITE_WRITE_EVENT_E),            /* 407 */                         \
STR(CPSS_DRAGONITE_ERROR_E)                   /* 408 */

/*******************************************************************************
* PRV_CPSS_EVENT_MASK_DEVICE_SET_FUNC
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                     queue number / other ) the event was received upon.
*                   may use value PRV_CPSS_DRV_EV_REQ_UNI_EV_EXTRA_DATA_ANY_CNS
*                   to indicate 'ALL interrupts' that relate to this unified
*                   event
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event or the evExtData within the unified
*                      event are not found in this device interrupts
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       This function called when OS interrupts and the ExtDrv interrupts are
*       locked !
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_EVENT_MASK_DEVICE_SET_FUNC)
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData,
    IN CPSS_EVENT_MASK_SET_ENT  operation
);


/*******************************************************************************
* prvCpssGenEventMaskDeviceBind
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       deviceType - device Type - PP/FA/Xbar device
*       funcPtr    - pointer to function that will be called when the
*                    Application wants to mask/unmask events relate to the
*                    deviceType
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad deviceType value or bad device number
*       GT_BAD_PTR  - funcPtr is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenEventMaskDeviceBind
(
    IN PRV_CPSS_DEVICE_TYPE_ENT    deviceType,
    IN PRV_CPSS_EVENT_MASK_DEVICE_SET_FUNC funcPtr
);

/*******************************************************************************
* PRV_CPSS_EVENT_GENERATE_DEVICE_SET_FUNC
*
* DESCRIPTION:
*       This debug routine configures device to generate unified event for
*       specific element in the event associated with extra data.
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                   queue number / other ) the event was received upon.
*                   Use CPSS_PARAM_NOT_USED_CNS to generate events for all supported evExtData.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device Or the 'extra data' has value that not relate
*                      to the uniEvent
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*       GT_NOT_SUPPORTED - on not supported unify event
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_EVENT_GENERATE_DEVICE_SET_FUNC)
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData
);

/*******************************************************************************
* prvCpssGenEventGenerateDeviceBind
*
* DESCRIPTION:
*       This routine binds function that configure device to generate unified event for
*       specific element in the event associated with extra data.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       deviceType - device Type - PP/FA/Xbar device
*       funcPtr    - pointer to function that will be called when the
*                    Application wants to generate events relate to the
*                    deviceType
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad deviceType value or bad device number
*       GT_BAD_PTR  - funcPtr is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenEventGenerateDeviceBind
(
    IN PRV_CPSS_DEVICE_TYPE_ENT                deviceType,
    IN PRV_CPSS_EVENT_GENERATE_DEVICE_SET_FUNC funcPtr
);

/*******************************************************************************
* PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC)
(
    IN  GT_U8                   devNum,
    IN  CPSS_EVENT_GPP_ID_ENT   gppId,
    IN  CPSS_EVENT_ISR_FUNC     isrFuncPtr,
    IN  void                    *cookie
);

/*******************************************************************************
* prvCpssGenEventGppIsrConnectBind
*
* DESCRIPTION:
*       This function binds function that connects an Isr for a given Gpp
*       interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PTR  - funcPtr is NULL pointer
*
* COMMENTS:
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
GT_STATUS prvCpssGenEventGppIsrConnectBind
(
    IN PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC funcPtr
);

/*******************************************************************************
* prvCpssGenEventRequestsDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the event related.
*       NOTE: function  'free' the allocated memory.
*             and restore DB to 'pre-init' state
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenEventRequestsDbRelease
(
    void
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenEvReqh */

