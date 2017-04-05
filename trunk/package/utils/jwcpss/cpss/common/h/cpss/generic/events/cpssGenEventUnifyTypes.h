/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenEventUnifyTypes.h
*
* DESCRIPTION:
*       This file includes all different hardware driven event types
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*
*******************************************************************************/
#ifndef __cpssGenEventUnifyTypesh
#define __cpssGenEventUnifyTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/**
 * typedef: enum CPSS_UNI_EV_CAUSE_ENT
 *
 * Description: Enumeration of types of events that CPSS allow application to
 *              get from the HW.
 *              Each PP may support only some of those events
 *
 *  Note : 1. most of those listed events retrieved by calling function
 *            cpssEventRecv(...)
 *         2. function cpssEventRecv(...) will retrieve the device number on
 *            which event occur via parameter evDevPtr
 *         3. function cpssEventRecv(...) will retrieve extra info via parameter
 *            evExtDataPtr for the next events:
 *            (*evExtDataPtr) as port number -
 *                              CPSS_PP_BM_MAX_BUFF_REACHED_E
 *
 *                              CPSS_PP_PORT_LINK_STATUS_CHANGED_E
 *                              CPSS_PP_PORT_AN_COMPLETED_E
 *                              CPSS_PP_PORT_RX_FIFO_OVERRUN_E
 *                              CPSS_PP_PORT_TX_FIFO_UNDERRUN_E
 *                              CPSS_PP_PORT_TX_FIFO_OVERRUN_E
 *                              CPSS_PP_PORT_TX_UNDERRUN_E
 *                              CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E
 *                              CPSS_PP_PORT_PRBS_ERROR_E
 *                              CPSS_PP_PORT_SYNC_STATUS_CHANGED_E
 *                              CPSS_PP_PORT_TX_CRC_ERROR_E
 *                              CPSS_PP_PORT_ILLEGAL_SEQUENCE_E
 *                              CPSS_PP_PORT_IPG_TOO_SMALL_E
 *                              CPSS_PP_PORT_FAULT_TYPE_CHANGE_E
 *                              CPSS_PP_PORT_FC_STATUS_CHANGED_E
 *                              CPSS_PP_PORT_CONSECUTIVE_TERM_CODE_E
 *
 *                              CPSS_PP_TQ_WATCHDOG_EX_PORT_E
 *                              CPSS_PP_TQ_TXQ2_FLUSH_PORT_E
 *                              CPSS_PP_TQ_HOL_REACHED_PORT_E
 *                              CPSS_PP_TQ_RED_REACHED_PORT_E
 *
 *                              CPSS_PP_MAC_SFLOW_E
 *                              CPSS_PP_EGRESS_SFLOW_E
 *                              CPSS_PP_INGRESS_SFLOW_SAMPLED_CNTR_E
 *                              CPSS_PP_EGRESS_SFLOW_SAMPLED_CNTR_E
 *
 *                              CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E
 *                              CPSS_PP_PORT_XAUI_PHY_E,
 *                              CPSS_PP_PORT_COUNT_COPY_DONE_E
 *                              CPSS_PP_PORT_COUNT_EXPIRED_E
 *
 *                              CPSS_PP_TQ_PORT_FULL_XG_E
 *                              CPSS_PP_TQ_PORT_DESC_FULL_E
 *
 *                              -- Per Port PCS (Physical Coding Sub-layer) events --
 *                              CPSS_PP_PORT_PCS_LINK_STATUS_CHANGED_E
 *                                  supported by : xcat,lion,exMxPm
 *                              CPSS_PP_PORT_PCS_DESKEW_TIMEOUT_E        -
 *                                  supported by : xcat,lion,exMxPm
 *                              CPSS_PP_PORT_PCS_DETECTED_COLUMN_IIAII_E -
 *                                  supported by : xcat,exMxPm
 *                              CPSS_PP_PORT_PCS_DESKEW_ERROR_E          -
 *                                  supported by : xcat,exMxPm
 *                              CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E     -
 *                                  supported by : xcat,lion,exMxPm
 *                              CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E      -
 *                                  supported by : xcat,lion,exMxPm
 *                              CPSS_PP_PORT_PCS_ALIGN_LOCK_LOST_E       -
 *                                  supported by : 40G PCS
 *
 *            (*evExtDataPtr) as port number and lane number -
 *                              -- Per Port Per lane events --
 *                                  formula is:
 *                                      (*evExtDataPtr) = (port) << 8 + (lane);
 *                                  for example when (*evExtDataPtr) = 0x1a03
 *                                  meaning : event on port 0x1a (26) , lane 3
 *                              the events are :
 *                              CPSS_PP_PORT_LANE_PRBS_ERROR_E            -
 *                                  supported by : xcat,lion,exMxPm
 *                              CPSS_PP_PORT_LANE_DISPARITY_ERROR_E       -
 *                                  supported by : xcat,exMxPm
 *                              CPSS_PP_PORT_LANE_SYMBOL_ERROR_E          -
 *                                  supported by : xcat,exMxPm
 *                              CPSS_PP_PORT_LANE_CJR_PAT_ERROR_E         -
 *                                  supported by : xcat,lion,exMxPm
 *                              CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E -
 *                                  supported by : xcat,exMxPm,Lion2
 *                              CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E   -
 *                                  supported by : xcat,exMxPm
 *                              CPSS_PP_PORT_LANE_DETECTED_IIAII_E        -
 *                                  supported by : xcat,exMxPm
 *
 *            (*evExtDataPtr) as queue number -
 *                              CPSS_PP_TX_BUFFER_QUEUE_E
 *                              CPSS_PP_TX_ERR_QUEUE_E
 *                              CPSS_PP_TX_END_E
 *                              CPSS_PP_RX_BUFFER_QUEUE0_E
 *                              CPSS_PP_RX_BUFFER_QUEUE1_E
 *                              CPSS_PP_RX_BUFFER_QUEUE2_E
 *                              CPSS_PP_RX_BUFFER_QUEUE3_E
 *                              CPSS_PP_RX_BUFFER_QUEUE4_E
 *                              CPSS_PP_RX_BUFFER_QUEUE5_E
 *                              CPSS_PP_RX_BUFFER_QUEUE6_E
 *                              CPSS_PP_RX_BUFFER_QUEUE7_E
 *                              CPSS_PP_RX_ERR_QUEUE0_E
 *                              CPSS_PP_RX_ERR_QUEUE1_E
 *                              CPSS_PP_RX_ERR_QUEUE2_E
 *                              CPSS_PP_RX_ERR_QUEUE3_E
 *                              CPSS_PP_RX_ERR_QUEUE4_E
 *                              CPSS_PP_RX_ERR_QUEUE5_E
 *                              CPSS_PP_RX_ERR_QUEUE6_E
 *                              CPSS_PP_RX_ERR_QUEUE7_E
 *
 *            (*evExtDataPtr) as index -
 *                              CPSS_PP_GPP_E -
 *                                  GPP index
 *
 *                              CPSS_PP_CNC_WRAPAROUND_BLOCK_E -
 *                                  CNC block
 *
 *                              CPSS_PP_LX_IPV4_LPM_ERR_E
 *                                  exmx - lpm index
 *
 *                              CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E
 *                                  dxch - 0 - port ,1 - GE , 2 - XG , 3 - global
 *
 *                              CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E
 *                                  dxch - 0 - port ,1 - GE , 2 - XG , 3 - global
 *
 *                              CPSS_PP_BM_MISC_E
 *                                  dxch -
 *                                      0 - LL port2 parity error
 *                                      1 - LL port1 parity error
 *                                      2 - control mem parity error
 *                                      3 - MC counter parity error
 *                                      4 - port RX full
 *                                      5 - MPPM parity error
 *
 *                                  exmxpm -
 *                                      0 - ingress/egress port rx full
 *                                      1 - ingress/egress clear before bind
 *                                      2 - ingress/egress triggered aging done
 *
 *                              CPSS_PP_RX_CNTR_OVERFLOW_E
 *                                  dxch :
 *                                      0 - sdma rx error resource counter off
 *                                      1 - sdma rx byte counter off
 *                                      2 - sdma rx packet counter off
 *
 *                              CPSS_PP_GOP_ADDRESS_OUT_OF_RANGE_E
 *                              CPSS_PP_GOP_COUNT_EXPIRED_E
 *                              CPSS_PP_GOP_COUNT_COPY_DONE_E
 *                              CPSS_PP_GOP_XG_BAD_FC_PACKET_GOOD_CRC_E
 *                              CPSS_PP_GOP_GIG_BAD_FC_PACKET_GOOD_CRC_E
 *                                  GOP index
 *
 *                              CPSS_PP_TQ_MISC_E
 *                                  0 - bad address
 *                                  1 - descriptors full
 *                                  2 - buffer full
 *                                  3 - descriptor parity error
 *                                  4 - pointer parity error
 *
 *                              CPSS_PP_IPVX_LPM_DATA_ERROR_E
 *                                  Only for ExMxPm XG devices
 *                                  - 0 - IPVX LPM0 Data Error
 *                                  - 1 - IPVX LPM1 Data Error
 *                                  - 2 - IPVX LPM2 Data Error
 *                                  - 3 - IPVX LPM3 Data Error
 *                                  - 4 - IPVX LPM4 Data Error
 *                                  - 5 - IPVX LPM5 Data Error
 *                                  - 6 - IPVX LPM6 Data Error
 *                                  - 7 - IPVX LPM7 Data Error
 *
 *
 *                              CPSS_PP_SCT_RATE_LIMITER_E
 *                                  - rate limiter
 *
 *                              CPSS_PP_EXTERNAL_MEMORY_PARITY_ERROR_E
 *                                  Only for ExMxPm devices
 *                                  - 0 - LPM SRAM4 Parity Error
 *                                  - 1 - LPM SRAM3 Parity Error
 *                                  - 2 - LPM SRAM2 Parity Error
 *                                  - 3 - LPM SRAM1 Parity Error
 *                                  - 4 - Control memory Parity Error
 *                                  - 5 - Ingress buffer memory Parity Error
 *                                  - 6 - Egress buffer memory Parity Error
 *
 *                              CPSS_PP_EXTERNAL_MEMORY_ECC_ERROR_E
 *                                  Only for ExMxPm devices
 *                                  - 0 - LPM SRAM4 ECC Error
 *                                  - 1 - LPM SRAM3 ECC Error
 *                                  - 2 - LPM SRAM2 ECC Error
 *                                  - 3 - LPM SRAM1 ECC Error
 *                                  - 4 - Control memory ECC Error
 *
 *                              CPSS_PP_STAT_INF_TX_SYNC_FIFO_FULL_E
 *                              CPSS_PP_STAT_INF_TX_SYNC_FIFO_OVERRUN_E
 *                              CPSS_PP_STAT_INF_TX_SYNC_FIFO_UNDERRUN_E
 *                              CPSS_PP_STAT_INF_RX_SYNC_FIFO_FULL_E
 *                              CPSS_PP_STAT_INF_RX_SYNC_FIFO_OVERRUN_E
 *                              CPSS_PP_STAT_INF_RX_SYNC_FIFO_UNDERRUN_E
 *                                  Only for ExMxPm devices
 *                                  - 0 - PCS Egress EQ
 *                                  - 1 - PCS Egress DQ
 *                                  - 2 - PCS Ingress DQ
 *
 *                              CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E
 *                              CPSS_PP_POLICER_DATA_ERR_E
 *                                  ExMxPm devices :
 *                                      0 - ingerss policer ,
 *                                      1 - egress policer
 *                                  xcat :
 *                                      0 - ingerss policer stage 0,
 *                                      1 - egress policer ,
 *                                      2 - ingress policer stage 1
 *
 *                              CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E
 *                              CPSS_PP_POLICER_IPFIX_ALARM_E
 *                              CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E
 *                                  xcat :
 *                                      0 - ingerss policer stage 0,
 *                                      1 - egress policer ,
 *                                      2 - ingress policer stage 1
 *
 *                              CPSS_PP_PCL_LOOKUP_DATA_ERROR_E
 *                                  - ExMxPm :
 *                                      0 - ingress PCL lookup 00
 *                                      1 - ingress PCL lookup 01
 *                                      2 - ingress PCL lookup 10
 *                                      3 - ingress PCL lookup 11
 *                                  - xcat :
 *                                      0 - ingress PCL lookup 00
 *                                      1 - ingress PCL lookup 01
 *                                      2 - ingress PCL lookup 1
 *
 *                              CPSS_PP_PCL_LOOKUP_FIFO_FULL_E
 *                                  - ExMxPm :
 *                                      0 - ingress PCL lookup 0
 *                                      2 - ingress PCL lookup 1
 *                                  - xcat :
 *                                      0 - ingress PCL lookup 00
 *                                      1 - ingress PCL lookup 01
 *                                      2 - ingress PCL lookup 1
 *
 *                              CPSS_PP_PCL_CONFIG_TABLE_DATA_ERROR_E
 *                                  - ExMxPm : 0
 *                                  - xcat : 0 - table 0 , 1 - table 1
 *
 *                              CPSS_PP_TCC_TCAM_ERROR_DETECTED_E
 *                              CPSS_PP_TCC_TCAM_BIST_FAILED_E
 *                                  - xcat : 0 - ingress PCL , 1 - ipvx
 *
 *                              CPSS_PP_GTS_GLOBAL_FIFO_FULL_E
 *                              CPSS_PP_GTS_VALID_TIME_SAMPLE_MESSAGE_E
 *                              -   lion : 0 - ingress , 1 - egress
 *  
 *                             CPSS_PP_OAM_EXCEPTION_KEEPALIVE_AGING_E   
 *                             CPSS_PP_OAM_EXCEPTION_EXCESS_KEEPALIVE_E  
 *                             CPSS_PP_OAM_EXCEPTION_INVALID_KEEPALIVE_E 
 *                             CPSS_PP_OAM_EXCEPTION_RDI_STATUS_E        
 *                             CPSS_PP_OAM_EXCEPTION_TX_PERIOD_E         
 *                             CPSS_PP_OAM_EXCEPTION_MEG_LEVEL_E         
 *                             CPSS_PP_OAM_EXCEPTION_SOURCE_INTERFACE_E  
 *                             CPSS_PP_OAM_ILLEGAL_ENTRY_INDEX_E
 *                                 0 - ingress , 1 - egress
 *
 *            (*evExtDataPtr) as device specific event number -
 *                              CPSS_PP_CRITICAL_HW_ERROR_E - HW event which
 *                                          requires application reference,
 *                                          e.g. soft reset.
 *
 *                              CPSS_PP_PORT_PRBS_ERROR_QSGMII_E
 *                                  - in XCAT2 B1 this interrupt
 *                                  means Signal Detect change
 *
 *                              CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E
 *                                  - in Lion2 means or MMPCS
 *                                  signal detect change or
 *                                  gearbox lock change
 *            (*evExtDataPtr) as port number and 'specific event' -
 *                                  formula is:
 *                                      (*evExtDataPtr) = (port) << 8 + (specific event);
 *                                  for example when (*evExtDataPtr) = 0x1a02
 *                                  meaning : event on port 0x1a (26) , specific event = 2
 *                         CPSS_PP_PORT_EEE_E - IEEE 802.3az Energy Efficient Ethernet (EEE) related.
 *                                  Hold next 'specific events' :
 *                                  0 --> PCS_RX_PATH_RECEIVED_LPI
 *                                  1 --> PCS_TX_PATH_RECEIVED_LPI
 *                                  2 --> MAC_RX_PATH_RECEIVED_LPI
 *
 *            (*evExtDataPtr) as fabric port number -
 *                              CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_RL_E
 *                              CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_RL_E
 *                              CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_RL_E
 *                              CPSS_PP_XBAR_RX_CELL_CNTR_RL_E
 *                              CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL_E
 *                              CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF_E
 *                              CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON_E
 *                              CPSS_PP_XBAR_SHP_TX_DROP_CNTR_RL_E
 *                              CPSS_PP_XBAR_SHP_TX_DROP_CNTR_INC_E
 *                              CPSS_PP_XBAR_INVALID_ADDR_E
 *                              CPSS_PP_XBAR_TX_CELL_CNTR_RL_E
 *                              CPSS_PP_XBAR_TX_CELL_CNTR_INC_E
 *
 *                              CPSS_PP_HGLINK_PING_RECEIVED_E
 *                              CPSS_PP_HGLINK_PING_SENT_E
 *                              CPSS_PP_HGLINK_MAC_TX_OVERRUN_E
 *                              CPSS_PP_HGLINK_MAC_TX_UNDERRUN_E
 *                              CPSS_PP_HGLINK_RX_ERR_CNTR_RL_E
 *                              CPSS_PP_HGLINK_BAD_PCS_TO_MAC_REFORMAT_RX_E
 *                              CPSS_PP_HGLINK_BAD_HEAD_CELL_RX_E
 *                              CPSS_PP_HGLINK_BAD_LENGTH_CELL_RX_E
 *                              CPSS_PP_HGLINK_FC_CELL_RX_INT_E
 *                              CPSS_PP_HGLINK_FC_CELL_TX_INT_E
 *                              CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC0_E
 *                              CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC1_E
 *                              CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC2_E
 *                              CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC3_E
 *                              CPSS_PP_HGLINK_ADDR_OUT_OF_RANGE_E
 *
 *******************************************************************************
 *                      Dragonite events -
 *                              CPSS_DRAGONITE_PORT_ON_E
 *                              CPSS_DRAGONITE_PORT_OFF_E
 *                              CPSS_DRAGONITE_DETECT_FAIL_E
 *                              CPSS_DRAGONITE_PORT_FAULT_E
 *                              CPSS_DRAGONITE_PORT_UDL_E
 *                              CPSS_DRAGONITE_PORT_FAULT_ON_STARTUP_E
 *                              CPSS_DRAGONITE_PORT_PM_E
 *                              CPSS_DRAGONITE_POWER_DENIED_E
 *                              CPSS_DRAGONITE_PORT_SPARE0_E           - reserved
 *                              CPSS_DRAGONITE_PORT_SPARE1_E           - reserved
 *                              CPSS_DRAGONITE_OVER_TEMP_E
 *                              CPSS_DRAGONITE_TEMP_ALARM_E
 *                              CPSS_DRAGONITE_DEVICE_FAULT_E
 *                              CPSS_DRAGONITE_DEVICE_SPARE0_E         - reserved
 *                              CPSS_DRAGONITE_DEVICE_SPARE1_E         - reserved
 *                              CPSS_DRAGONITE_OVER_CONSUM_E
 *                              CPSS_DRAGONITE_VMAIN_LOW_AF_E
 *                              CPSS_DRAGONITE_VMAIN_LOW_AT_E
 *                              CPSS_DRAGONITE_VMAIN_HIGH_E
 *                              CPSS_DRAGONITE_SYSTEM_SPARE0_E           - reserved
 *                              CPSS_DRAGONITE_SYSTEM_SPARE1_E           - reserved
 *                              CPSS_DRAGONITE_READ_EVENT_E
 *                              CPSS_DRAGONITE_WRITE_EVENT_E
 *                              CPSS_DRAGONITE_ERROR_E         - SW event for error in DRAGONITE IRQ mechanism,
 *                                                        at this moment I'm aware of one possible scenario
 *                                                        Case IMO_bit != 0 means: Kirkwood get signal on
 *                                                        DRAGONITE interrupt line, but when it approach to read
 *                                                        IRQ_Cause_Out register it sees that Dragonite is
 *                                                        still owner of IRQ memory. It's possible only
 *                                                        as result of firmware internal bug. Application
 *                                                        will decide what should be done in such case.
 *******************************************************************************
 *            NOTE: about multi-port groups indication.
 *            for the next events the (*evExtDataPtr) also hold the (portGroupId) indication
 *            this is not relevant for the non 'multi-port groups' devices.
 *            The formula is :  (*evExtDataPtr) = index + (portGroupId << 16)
 *              for example : CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E with value of
 *                  (*evExtDataPtr) = 0x00020001 --> means : portGroupId = 2 , index = 1 --> egress policer
 *
 *            The events are :
 *               -- GPP --
 *               CPSS_PP_GPP_E
 *               -- CNC --
 *               CPSS_PP_CNC_WRAPAROUND_BLOCK_E
 *               CPSS_PP_CNC_DUMP_FINISHED_E
 *               -- Policer --
 *               CPSS_PP_POLICER_DATA_ERR_E
 *               CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E
 *               CPSS_PP_POLICER_IPFIX_ALARM_E
 *               CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E
 *
 */
typedef enum
{
    CPSS_PP_UNI_EV_MIN_E                               = (0                 ) ,  /*    */

    /* PCI */
    CPSS_PP_SLV_WRITE_ERR_E                        = (CPSS_PP_UNI_EV_MIN_E  ) ,  /* 0  */
    CPSS_PP_MAS_WRITE_ERR_E                                                   ,  /* 1  */
    CPSS_PP_ADDR_ERR_E                                                        ,  /* 2  */
    CPSS_PP_MAS_ABORT_E                                                       ,  /* 3  */
    CPSS_PP_TARGET_ABORT_E                                                    ,  /* 4  */
    CPSS_PP_SLV_READ_ERR_E                                                    ,  /* 5  */
    CPSS_PP_MAS_READ_ERR_E                                                    ,  /* 6  */
    CPSS_PP_RETRY_CNTR_E                                                      ,  /* 7  */

    /* Misc */
    CPSS_PP_MISC_TWSI_TIME_OUT_E                                              ,  /* 8  */
    CPSS_PP_MISC_TWSI_STATUS_E                                                ,  /* 9  */
    CPSS_PP_MISC_ILLEGAL_ADDR_E                                               ,  /* 10 */
    CPSS_PP_MISC_CPU_PORT_RX_OVERRUN_E                                        ,  /* 11 */
    CPSS_PP_MISC_CPU_PORT_TX_OVERRUN_E                                        ,  /* 12 */
    CPSS_PP_MISC_TX_CRC_PORT_E                                                ,  /* 13 */
    CPSS_PP_MISC_C2C_W_FAR_END_UP_E                                           ,  /* 14 */
    CPSS_PP_MISC_C2C_N_FAR_END_UP_E                                           ,  /* 15 */
    CPSS_PP_MISC_C2C_DATA_ERR_E                                               ,  /* 16 */
    CPSS_PP_MISC_MSG_TIME_OUT_E                                               ,  /* 17 */
    CPSS_PP_MISC_UPDATED_STAT_E                                               ,  /* 18 */
    CPSS_PP_GPP_E                                                             ,  /* 19 */
    CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E                                       ,  /* 20 */
    CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E                                     ,  /* 21 */
    CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E                                       ,  /* 22 */
    CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E                                     ,  /* 23 */
    CPSS_PP_MISC_PEX_ADDR_UNMAPPED_E                                          ,  /* 24 */
    CPSS_PP_MISC_GENXS_READ_DMA_DONE_E                                        ,  /* 25 */

    /* Ingress Buffer Manager */
    CPSS_PP_BM_EMPTY_CLEAR_E                                                  ,  /* 26 */
    CPSS_PP_BM_EMPTY_INC_E                                                    ,  /* 27 */
    CPSS_PP_BM_AGED_PACKET_E                                                  ,  /* 28 */
    CPSS_PP_BM_MAX_BUFF_REACHED_E                                             ,  /* 29 */
    CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E                                 ,  /* 30 */
    CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E                                  ,  /* 31 */
    CPSS_PP_BM_INVALID_ADDRESS_E                                              ,  /* 32 */
    CPSS_PP_BM_WRONG_SRC_PORT_E                                               ,  /* 33 */
    CPSS_PP_BM_MC_INC_OVERFLOW_E                                              ,  /* 34 */
    CPSS_PP_BM_MC_INC_UNDERRUN_E                                              ,  /* 35 */
    CPSS_PP_BM_TQ_PARITY_ERROR_PORT_E                                         ,  /* 36 */
    CPSS_PP_BM_RX_MEM_READ_ECC_ERROR_E                                        ,  /* 37 */
    CPSS_PP_BM_VLT_ECC_ERR_E                                                  ,  /* 38 */
    CPSS_PP_BM_MISC_E                                                         ,  /* 39 */
    CPSS_PP_BM_TRIGGER_AGING_DONE_E                                           ,  /* 40 */
    CPSS_PP_BM_PORT_RX_FULL_E                                                 ,  /* 41 */
    CPSS_PP_BM_ALL_GIGA_PORTS_BUFF_LIMIT_REACHED_E                            ,  /* 42 */
    CPSS_PP_BM_ALL_HG_STACK_PORTS_BUFF_LIMIT_REACHED_E                        ,  /* 43 */

    /* Port */
    CPSS_PP_PORT_LINK_STATUS_CHANGED_E                                        ,  /* 44 */
    CPSS_PP_PORT_AN_COMPLETED_E                                               ,  /* 45 */
    CPSS_PP_PORT_RX_FIFO_OVERRUN_E                                            ,  /* 46 */
    CPSS_PP_PORT_TX_FIFO_UNDERRUN_E                                           ,  /* 47 */
    CPSS_PP_PORT_TX_FIFO_OVERRUN_E                                            ,  /* 48 */
    CPSS_PP_PORT_TX_UNDERRUN_E                                                ,  /* 49 */
    CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E                                       ,  /* 50 */
    CPSS_PP_PORT_PRBS_ERROR_E                                                 ,  /* 51 */
    CPSS_PP_PORT_SYNC_STATUS_CHANGED_E                                        ,  /* 52 */
    CPSS_PP_PORT_TX_CRC_ERROR_E                                               ,  /* 53 */
    CPSS_PP_PORT_ILLEGAL_SEQUENCE_E                                           ,  /* 54 */
    CPSS_PP_PORT_IPG_TOO_SMALL_E                                              ,  /* 55 */
    CPSS_PP_PORT_FAULT_TYPE_CHANGE_E                                          ,  /* 56 */
    CPSS_PP_PORT_FC_STATUS_CHANGED_E                                          ,  /* 57 */
    CPSS_PP_PORT_CONSECUTIVE_TERM_CODE_E                                      ,  /* 58 */

    CPSS_PP_INTERNAL_PHY_E                                                    ,  /* 59 */
    CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E                                        ,  /* 60 */
    CPSS_PP_PORT_XAUI_PHY_E                                                   ,  /* 61*/
    CPSS_PP_PORT_COUNT_COPY_DONE_E                                            ,  /* 62*/
    CPSS_PP_PORT_COUNT_EXPIRED_E                                              ,  /* 63 */

    /* Group Of Ports */
    CPSS_PP_GOP_ADDRESS_OUT_OF_RANGE_E                                        ,  /* 64 */
    CPSS_PP_GOP_COUNT_EXPIRED_E                                               ,  /* 65 */
    CPSS_PP_GOP_COUNT_COPY_DONE_E                                             ,  /* 66 */
    CPSS_PP_GOP_GIG_BAD_FC_PACKET_GOOD_CRC_E                                  ,  /* 67 */
    CPSS_PP_GOP_XG_BAD_FC_PACKET_GOOD_CRC_E                                   ,  /* 68 */
    CPSS_PP_XSMI_WRITE_DONE_E                                                 ,  /* 69 */

    /* Tx Queue */
    CPSS_PP_TQ_WATCHDOG_EX_PORT_E                                             ,  /* 70 */
    CPSS_PP_TQ_TXQ2_FLUSH_PORT_E                                              ,  /* 71 */
    CPSS_PP_TQ_TXQ2_MG_FLUSH_E                                                ,  /* 72 */
    CPSS_PP_TQ_ONE_ECC_ERROR_E                                                ,  /* 73 */
    CPSS_PP_TQ_TWO_ECC_ERROR_E                                                ,  /* 74 */
    CPSS_PP_TQ_MG_READ_ERR_E                                                  ,  /* 75 */
    CPSS_PP_TQ_HOL_REACHED_PORT_E                                             ,  /* 76 */
    CPSS_PP_TQ_RED_REACHED_PORT_E                                             ,  /* 77 */
    CPSS_PP_TQ_TOTAL_DESC_UNDERFLOW_E                                         ,  /* 78 */
    CPSS_PP_TQ_TOTAL_DESC_OVERFLOW_E                                          ,  /* 79 */
    CPSS_PP_TQ_TOTAL_BUFF_UNDERFLOW_E                                         ,  /* 80 */
    CPSS_PP_TQ_SNIFF_DESC_DROP_E                                              ,  /* 81 */
    CPSS_PP_TQ_MLL_PARITY_ERR_E                                               ,  /* 82 */
    CPSS_PP_TQ_MC_FIFO_OVERRUN_E                                              ,  /* 83 */
    CPSS_PP_TQ_MISC_E                                                         ,  /* 84 */
    CPSS_PP_TQ_PORT_DESC_FULL_E                                               ,  /* 85 */
    CPSS_PP_TQ_MC_DESC_FULL_E                                                 ,  /* 86 */
    CPSS_PP_TQ_GIGA_FIFO_FULL_E                                               ,  /* 87 */
    CPSS_PP_TQ_XG_MC_FIFO_FULL_E                                              ,  /* 88 */
    CPSS_PP_TQ_PORT_FULL_XG_E                                                 ,  /* 89 */

    /* Ethernet Bridge */
    CPSS_PP_EB_AUQ_FULL_E                                                     ,  /*  90 */
    CPSS_PP_EB_AUQ_PENDING_E                                                  ,  /*  91 */
    CPSS_PP_EB_AUQ_OVER_E                                                     ,  /*  92 */
    CPSS_PP_EB_AUQ_ALMOST_FULL_E                                              ,  /*  93 */
    CPSS_PP_EB_FUQ_FULL_E                                                     ,  /*  94 */
    CPSS_PP_EB_FUQ_PENDING_E                                                  ,  /*  95 */
    CPSS_PP_EB_NA_FIFO_FULL_E                                                 ,  /*  96 */
    CPSS_PP_EB_MG_ADDR_OUT_OF_RANGE_E                                         ,  /*  97 */
    CPSS_PP_EB_VLAN_TBL_OP_DONE_E                                             ,  /*  98 */
    CPSS_PP_EB_SECURITY_BREACH_UPDATE_E                                       ,  /*  99 */
    CPSS_PP_EB_VLAN_SECURITY_BREACH_E                                         ,  /* 100 */
    CPSS_PP_EB_NA_NOT_LEARNED_SECURITY_BREACH_E                               ,  /* 101 */
    CPSS_PP_EB_SA_MSG_DISCARDED_E                                             ,  /* 102 */
    CPSS_PP_EB_QA_MSG_DISCARDED_E                                             ,  /* 103 */
    CPSS_PP_EB_SA_DROP_SECURITY_BREACH_E                                      ,  /* 104 */
    CPSS_PP_EB_DA_DROP_SECURITY_BREACH_E                                      ,  /* 105 */
    CPSS_PP_EB_DA_SA_DROP_SECURITY_BREACH_E                                   ,  /* 106 */
    CPSS_PP_EB_NA_ON_LOCKED_DROP_SECURITY_BREACH_E                            ,  /* 107 */
    CPSS_PP_EB_MAC_RANGE_DROP_SECURITY_BREACH_E                               ,  /* 108 */
    CPSS_PP_EB_INVALID_SA_DROP_SECURITY_BREACH_E                              ,  /* 109 */
    CPSS_PP_EB_VLAN_NOT_VALID_DROP_SECURITY_BREACH_E                          ,  /* 110 */
    CPSS_PP_EB_VLAN_NOT_MEMBER_DROP_DROP_SECURITY_BREACH_E                    ,  /* 111 */
    CPSS_PP_EB_VLAN_RANGE_DROP_SECURITY_BREACH_E                              ,  /* 112 */
    CPSS_PP_EB_INGRESS_FILTER_PCKT_E                                          ,  /* 113 */
    CPSS_PP_MAC_TBL_READ_ECC_ERR_E                                            ,  /* 114 */
    CPSS_PP_MAC_SFLOW_E                                                       ,  /* 115 */
    CPSS_PP_MAC_NUM_OF_HOP_EXP_E                                              ,  /* 116 */
    CPSS_PP_MAC_NA_LEARNED_E                                                  ,  /* 117 */
    CPSS_PP_MAC_NA_NOT_LEARNED_E                                              ,  /* 118 */
    CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E                                       ,  /* 119 */
    CPSS_PP_MAC_MG_ADDR_OUT_OF_RANGE_E                                        ,  /* 120 */
    CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E                                        ,  /* 121 */
    CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E                                        ,  /* 122 */
    CPSS_PP_MAC_NA_SELF_LEARNED_E                                             ,  /* 123 */
    CPSS_PP_MAC_NA_FROM_CPU_LEARNED_E                                         ,  /* 124 */
    CPSS_PP_MAC_NA_FROM_CPU_DROPPED_E                                         ,  /* 125 */
    CPSS_PP_MAC_AGED_OUT_E                                                    ,  /* 126 */
    CPSS_PP_MAC_FIFO_2_CPU_EXCEEDED_E                                         ,  /* 127 */
    CPSS_PP_MAC_1ECC_ERRORS_E                                                 ,  /* 128 */
    CPSS_PP_MAC_2ECC_ERRORS_E                                                 ,  /* 129 */
    CPSS_PP_MAC_TBL_OP_DONE_E                                                 ,  /* 130 */
    CPSS_PP_MAC_PACKET_RSSI_LESS_THAN_THRESH_E                                ,  /* 131 */
    CPSS_PP_MAC_UPD_AVG_RSSI_LESS_THAN_THRESH_E                               ,  /* 132 */
    CPSS_PP_MAC_STG_TBL_DATA_ERROR_E                                          ,  /* 133 */
    CPSS_PP_MAC_VIDX_TBL_DATA_ERROR_E                                         ,  /* 134*/
    CPSS_PP_LX_LB_ERR_E                                                       ,  /* 135 */
    CPSS_PP_LX_CTRL_MEM_2_RF_ERR_E                                            ,  /* 136 */
    CPSS_PP_LX_TCB_CNTR_E                                                     ,  /* 137 */
    CPSS_PP_LX_IPV4_MC_ERR_E                                                  ,  /* 138 */
    CPSS_PP_LX_IPV4_LPM_ERR_E                                                 ,  /* 139 */
    CPSS_PP_LX_IPV4_ROUTE_ERR_E                                               ,  /* 140 */
    CPSS_PP_LX_IPV4_CNTR_E                                                    ,  /* 141 */
    CPSS_PP_LX_L3_L7_ERR_ADDR_E                                               ,  /* 142 */
    CPSS_PP_LX_TRUNK_ADDR_OUT_OF_RANGE_E                                      ,  /* 143 */
    CPSS_PP_LX_IPV4_REFRESH_AGE_OVERRUN_E                                     ,  /* 144 */
    CPSS_PP_LX_PCE_PAR_ERR_E                                                  ,  /* 145 */
    CPSS_PP_LX_TC_2_RF_CNTR_ALRM_E                                            ,  /* 146 */
    CPSS_PP_LX_TC_2_RF_PLC_ALRM_E                                             ,  /* 147 */
    CPSS_PP_LX_TC_2_RF_TBL_ERR_E                                              ,  /* 148 */
    CPSS_PP_LX_CLASSIFIER_HASH_PAR_ERR_E                                      ,  /* 149 */
    CPSS_PP_LX_FLOW_LKUP_PAR_ERR_E                                            ,  /* 150 */
    CPSS_PP_LX_FLOW_KEY_TBL_PAR_ERR_E                                         ,  /* 151 */
    CPSS_PP_LX_MPLS_ILM_TBL_PAR_ERR_E                                         ,  /* 152 */
    CPSS_PP_LX_MPLS_CNTR_E                                                    ,  /* 153 */
    CPSS_PP_EB_TCC_E                                                          ,  /* 154 */

    /* Network Interface */
    CPSS_PP_RX_BUFFER_QUEUE0_E                                                ,  /* 155 */
    CPSS_PP_RX_BUFFER_QUEUE1_E                                                ,  /* 156 */
    CPSS_PP_RX_BUFFER_QUEUE2_E                                                ,  /* 157 */
    CPSS_PP_RX_BUFFER_QUEUE3_E                                                ,  /* 158 */
    CPSS_PP_RX_BUFFER_QUEUE4_E                                                ,  /* 159 */
    CPSS_PP_RX_BUFFER_QUEUE5_E                                                ,  /* 160 */
    CPSS_PP_RX_BUFFER_QUEUE6_E                                                ,  /* 161 */
    CPSS_PP_RX_BUFFER_QUEUE7_E                                                ,  /* 162 */
    CPSS_PP_RX_ERR_QUEUE0_E                                                   ,  /* 163 */
    CPSS_PP_RX_ERR_QUEUE1_E                                                   ,  /* 164 */
    CPSS_PP_RX_ERR_QUEUE2_E                                                   ,  /* 165 */
    CPSS_PP_RX_ERR_QUEUE3_E                                                   ,  /* 166 */
    CPSS_PP_RX_ERR_QUEUE4_E                                                   ,  /* 167 */
    CPSS_PP_RX_ERR_QUEUE5_E                                                   ,  /* 168 */
    CPSS_PP_RX_ERR_QUEUE6_E                                                   ,  /* 169 */
    CPSS_PP_RX_ERR_QUEUE7_E                                                   ,  /* 170 */
    CPSS_PP_RX_CNTR_OVERFLOW_E                                                ,  /* 171 */
    CPSS_PP_TX_BUFFER_QUEUE_E                                                 ,  /* 172 */
    CPSS_PP_TX_ERR_QUEUE_E                                                    ,  /* 173 */
    CPSS_PP_TX_END_E                                                          ,  /* 174 */

    /* Per Port Per lane events */
    CPSS_PP_PORT_LANE_PRBS_ERROR_E                                            ,  /* 175 */
    CPSS_PP_PORT_LANE_DISPARITY_ERROR_E                                       ,  /* 176 */
    CPSS_PP_PORT_LANE_SYMBOL_ERROR_E                                          ,  /* 177 */
    CPSS_PP_PORT_LANE_CJR_PAT_ERROR_E                                         ,  /* 178 */
    CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E                                 ,  /* 179 */
    CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E                                   ,  /* 180 */
    CPSS_PP_PORT_LANE_DETECTED_IIAII_E                                        ,  /* 181 */

    /* Per Port PCS events */
    CPSS_PP_PORT_PCS_LINK_STATUS_CHANGED_E                                    ,  /* 182 */
    CPSS_PP_PORT_PCS_DESKEW_TIMEOUT_E                                         ,  /* 183 */
    CPSS_PP_PORT_PCS_DETECTED_COLUMN_IIAII_E                                  ,  /* 184 */
    CPSS_PP_PORT_PCS_DESKEW_ERROR_E                                           ,  /* 185 */
    CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E                                      ,  /* 186 */
    CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E                                       ,  /* 187 */
    CPSS_PP_PORT_PCS_ALIGN_LOCK_LOST_E                                        ,  /* 188 */

    /* Egress Buffer Manager */
    CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E                                           ,  /* 189 */
    CPSS_PP_BM_EGRESS_EMPTY_INC_E                                             ,  /* 190 */
    CPSS_PP_BM_EGRESS_AGED_PACKET_E                                           ,  /* 191 */
    CPSS_PP_BM_EGRESS_MAX_BUFF_REACHED_E                                      ,  /* 192 */
    CPSS_PP_BM_EGRESS_PORT_RX_BUFFERS_CNT_UNDERRUN_E                          ,  /* 193 */
    CPSS_PP_BM_EGRESS_PORT_RX_BUFFERS_CNT_OVERRUN_E                           ,  /* 194 */
    CPSS_PP_BM_EGRESS_INVALID_ADDRESS_E                                       ,  /* 195 */
    CPSS_PP_BM_EGRESS_WRONG_SRC_PORT_E                                        ,  /* 196 */
    CPSS_PP_BM_EGRESS_MC_INC_OVERFLOW_E                                       ,  /* 197 */
    CPSS_PP_BM_EGRESS_MC_INC_UNDERRUN_E                                       ,  /* 198 */
    CPSS_PP_BM_EGRESS_TQ_PARITY_ERROR_PORT_E                                  ,  /* 199 */
    CPSS_PP_BM_EGRESS_RX_MEM_READ_ECC_ERROR_E                                 ,  /* 200 */
    CPSS_PP_BM_EGRESS_VLT_ECC_ERR_E                                           ,  /* 201 */
    CPSS_PP_BM_EGRESS_MISC_E                                                  ,  /* 202 */

    /************************** PEX events *********************************/
    CPSS_PP_PEX_DL_DOWN_TX_ACC_ERR_E                                          ,  /* 203 */
    CPSS_PP_PEX_MASTER_DISABLED_E                                             ,  /* 204 */
    CPSS_PP_PEX_ERROR_WR_TO_REG_E                                             ,  /* 205 */
    CPSS_PP_PEX_HIT_DEFAULT_WIN_ERR_E                                         ,  /* 206 */
    CPSS_PP_PEX_COR_ERROR_DET_E                                               ,  /* 207 */
    CPSS_PP_PEX_NON_FATAL_ERROR_DET_E                                         ,  /* 208 */
    CPSS_PP_PEX_FATAL_ERROR_DET_E                                             ,  /* 209 */
    CPSS_PP_PEX_DSTATE_CHANGED_E                                              ,  /* 210 */
    CPSS_PP_PEX_BIST_E                                                        ,  /* 211 */
    CPSS_PP_PEX_RCV_ERROR_FATAL_E                                             ,  /* 212 */
    CPSS_PP_PEX_RCV_ERROR_NON_FATAL_E                                         ,  /* 213 */
    CPSS_PP_PEX_RCV_ERROR_COR_E                                               ,  /* 214 */
    CPSS_PP_PEX_RCV_CRS_E                                                     ,  /* 215 */
    CPSS_PP_PEX_PEX_SLAVE_HOT_RESET_E                                         ,  /* 216 */
    CPSS_PP_PEX_PEX_SLAVE_DISABLE_LINK_E                                      ,  /* 217 */
    CPSS_PP_PEX_PEX_SLAVE_LOOPBACK_E                                          ,  /* 218 */
    CPSS_PP_PEX_PEX_LINK_FAIL_E                                               ,  /* 219 */
    CPSS_PP_PEX_RCV_A_E                                                       ,  /* 220 */
    CPSS_PP_PEX_RCV_B_E                                                       ,  /* 221 */
    CPSS_PP_PEX_RCV_C_E                                                       ,  /* 222 */
    CPSS_PP_PEX_RCV_D_E                                                       ,  /* 223 */

    /******************** Buffer Memory events ****************************/
    CPSS_PP_BUF_MEM_MAC_ERROR_E                                               ,  /* 224 */
    CPSS_PP_BUF_MEM_ONE_ECC_ERROR_E                                           ,  /* 225 */
    CPSS_PP_BUF_MEM_TWO_OR_MORE_ECC_ERRORS_E                                  ,  /* 226 */


    /************************** Policer events *****************************/
    CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E                                      ,  /* 227 */
    CPSS_PP_POLICER_DATA_ERR_E                                                ,  /* 228 */
    CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E                                       ,  /* 229 */
    CPSS_PP_POLICER_IPFIX_ALARM_E                                             ,  /* 230 */
    CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E                  ,  /* 231 */

    /************************** Policy Engine events ***********************/
    CPSS_PP_PCL_TCC_ECC_ERR_E                                                 ,  /* 232 */
    CPSS_PP_PCL_MG_ADDR_OUT_OF_RANGE_E                                        ,  /* 233 */
    CPSS_PP_PCL_LOOKUP_DATA_ERROR_E                                           ,  /* 234 */
    CPSS_PP_PCL_ACTION_ERROR_DETECTED_E                                       ,  /* 235 */
    CPSS_PP_PCL_MG_LOOKUP_RESULTS_READY_E                                     ,  /* 236 */
    CPSS_PP_PCL_INLIF_TABLE_DATA_ERROR_E                                      ,  /* 237 */
    CPSS_PP_PCL_CONFIG_TABLE_DATA_ERROR_E                                     ,  /* 238 */
    CPSS_PP_PCL_LOOKUP_FIFO_FULL_E                                            ,  /* 239 */

    /**************** Centralized Counters (CNC) events *********************/
    CPSS_PP_CNC_WRAPAROUND_BLOCK_E                                            ,  /* 240 */
    CPSS_PP_CNC_DUMP_FINISHED_E                                               ,  /* 241 */

    /************************** SCT *****************************************/
    CPSS_PP_SCT_RATE_LIMITER_E                                                ,  /* 242 */

    /************************** Egress and Ingress SFLOW (STC)***************/
    CPSS_PP_EGRESS_SFLOW_E                                                    ,  /* 243 */
    CPSS_PP_INGRESS_SFLOW_SAMPLED_CNTR_E                                      ,  /* 244 */
    CPSS_PP_EGRESS_SFLOW_SAMPLED_CNTR_E                                       ,  /* 245 */

    /************************** TTI events *****************************/
    CPSS_PP_TTI_CPU_ADDRESS_OUT_OF_RANGE_E                                    ,  /* 246 */
    CPSS_PP_TTI_ACCESS_DATA_ERROR_E                                           ,  /* 247 */

    /************************** DIT events *****************************/
    CPSS_PP_DIT_MC_DIT_TTL_TRHOLD_EXCEED_E                                    ,  /* 248 */
    CPSS_PP_DIT_UC_DIT_TTL_TRHOLD_EXCEED_E                                    ,  /* 249 */
    CPSS_PP_DIT_DATA_ERROR_E                                                  ,  /* 250 */
    CPSS_PP_DIT_MC0_PARITY_ERROR_E                                            ,  /* 251 */
    CPSS_PP_DIT_INT_TABLE_ECC_ERROR_CNTR_E                                    ,  /* 252 */
    CPSS_PP_DIT_MC0_IP_TV_PARITY_ERROR_CNTR_E                                 ,  /* 253 */

    /************************** External TCAM events ***********************/
    CPSS_PP_EXTERNAL_TCAM_INT_SUM_E                                           ,  /* 254 */
    CPSS_PP_EXTERNAL_TCAM_IO_ERR_E                                            ,  /* 255 */
    CPSS_PP_EXTERNAL_TCAM_RX_PARITY_ERR_E                                     ,  /* 256 */
    CPSS_PP_EXTERNAL_TCAM_MG_CMD_EXE_DONE_E                                   ,  /* 257 */
    CPSS_PP_EXTERNAL_TCAM_NEW_DATA_IN_MAILBOX_E                               ,  /* 258 */
    CPSS_PP_EXTERNAL_TCAM_MG_WRONG_EXP_RES_LEN_E                              ,  /* 259 */
    CPSS_PP_EXTERNAL_TCAM_RSP_IDQ_EMPTY_E                                     ,  /* 260 */
    CPSS_PP_EXTERNAL_TCAM_PHASE_ERROR_E                                       ,  /* 261 */
    CPSS_PP_EXTERNAL_TCAM_TRAINING_SEQUENCE_DONE_E                            ,  /* 262 */

    /******************** External Memory events ******************** */
    CPSS_PP_EXTERNAL_MEMORY_PARITY_ERROR_E                                    ,  /* 263 */
    CPSS_PP_EXTERNAL_MEMORY_ECC_ERROR_E                                       ,  /* 264 */
    CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_LOW_E                      ,  /* 265 */
    CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_HIGH_E                     ,  /* 266 */
    CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_MG_CMD_DONE_E                         ,  /* 267 */


    /******************** PP VOQ events ****************************/
    CPSS_PP_VOQ_DROP_DEV_EN_E                                                 ,  /* 268 */
    CPSS_PP_VOQ_GLOBAL_DESC_FULL_E                                            ,  /* 269 */
    CPSS_PP_VOQ_ADDR_OUT_OF_RANGE_E                                           ,  /* 270 */
    CPSS_PP_VOQ_ECC_ERR_E                                                     ,  /* 271 */
    CPSS_PP_VOQ_GLOBAL_BUF_FULL_E                                             ,  /* 272 */

    /************************** PP XBAR events ********************************/
    CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_RL_E                                   ,  /* 273 */
    CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_RL_E                                    ,  /* 274 */
    CPSS_PP_XBAR_RX_CELL_CNTR_RL_E                                            ,  /* 275 */
    CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL_E                               ,  /* 276 */
    CPSS_PP_XBAR_SHP_TX_DROP_CNTR_RL_E                                        ,  /* 277 */
    CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_RL_E                                    ,  /* 278 */
    CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_INC_E                                   ,  /* 279 */
    CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_INC_E                                  ,  /* 280 */
    CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_INC_E                                   ,  /* 281 */
    CPSS_PP_XBAR_RX_CELL_CNTR_INC_E                                           ,  /* 282 */
    CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC_E                              ,  /* 283 */
    CPSS_PP_XBAR_SHP_TX_DROP_CNTR_INC_E                                       ,  /* 284 */
    CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF_E                         ,  /* 285 */
    CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON_E                          ,  /* 286 */
    CPSS_PP_XBAR_INVALID_ADDR_E                                               ,  /* 287 */
    CPSS_PP_XBAR_TX_CELL_CNTR_RL_E                                            ,  /* 288 */
    CPSS_PP_XBAR_TX_CELL_CNTR_INC_E                                           ,  /* 289 */

    /******************** PP Fabric Reassembly engine events ******************/
    CPSS_PP_CRX_MAIL_ARRIVED_E                                                ,  /* 290 */
    CPSS_PP_CRX_READ_EMPTY_CPU_E                                              ,  /* 291 */
    CPSS_PP_CRX_DATA_DROP_E                                                   ,  /* 292 */
    CPSS_PP_CRX_CPU_DROP_E                                                    ,  /* 293 */
    CPSS_PP_CRX_AGED_OUT_CONTEXT_E                                            ,  /* 294 */
    CPSS_PP_CRX_CRC_DROP_E                                                    ,  /* 295 */
    CPSS_PP_CRX_PACKET_LENGTH_DROP_E                                          ,  /* 296 */
    CPSS_PP_CRX_MAX_BUFFERS_DROP_E                                            ,  /* 297 */
    CPSS_PP_CRX_BAD_CONTEXT_DROP_E                                            ,  /* 298 */
    CPSS_PP_CRX_NO_BUFFERS_DROP_E                                             ,  /* 299 */
    CPSS_PP_CRX_MULTICAST_CONGESTION_DROP_E                                   ,  /* 300 */
    CPSS_PP_CRX_DATA_FIFO_UNDERRUN_E                                          ,  /* 301 */
    CPSS_PP_CRX_DATA_FIFO_OVERRUN_E                                           ,  /* 302 */
    CPSS_PP_CRX_CPU_UNDERRUN_E                                                ,  /* 303 */
    CPSS_PP_CRX_CPU_OVERRUN_E                                                 ,  /* 304 */
    CPSS_PP_CRX_E2E_FIFO_UNDERRUN_E                                           ,  /* 305 */
    CPSS_PP_CRX_E2E_FIFO_OVERRUN_E                                            ,  /* 306 */
    CPSS_PP_CRX_UNKNOWN_CELL_TYPE_E                                           ,  /* 307 */
    CPSS_PP_CRX_COUNTER_BAD_ADDR_E                                            ,  /* 308 */
    CPSS_PP_CRX_PSM_BAD_ADDR_E                                                ,  /* 309 */

    /*************** PP Fabric Segmentation engine events **********************/
    CPSS_PP_TXD_SEG_FIFO_PARITY_ERR_E                                         ,  /* 310 */
    CPSS_PP_TXD_CPU_MAIL_SENT_E                                               ,  /* 311 */
    CPSS_PP_TXD_SEG_FIFO_OVERRUN_E                                            ,  /* 312 */
    CPSS_PP_TXD_WRONG_CH_TO_FABRIC_DIST_E                                     ,  /* 313 */

    /************************** IPVX events ***********************************/
    CPSS_PP_IPVX_DATA_ERROR_E                                                 ,  /* 314 */
    CPSS_PP_IPVX_ADDRESS_OUT_OF_MEMORY_E                                      ,  /* 315 */
    CPSS_PP_IPVX_LPM_DATA_ERROR_E                                             ,  /* 316 */

    /************************** Statistic Interface ******************************/
    CPSS_PP_STAT_INF_TX_SYNC_FIFO_FULL_E                                      ,  /* 317 */
    CPSS_PP_STAT_INF_TX_SYNC_FIFO_OVERRUN_E                                   ,  /* 318 */
    CPSS_PP_STAT_INF_TX_SYNC_FIFO_UNDERRUN_E                                  ,  /* 319 */
    CPSS_PP_STAT_INF_RX_SYNC_FIFO_FULL_E                                      ,  /* 320 */
    CPSS_PP_STAT_INF_RX_SYNC_FIFO_OVERRUN_E                                   ,  /* 321 */
    CPSS_PP_STAT_INF_RX_SYNC_FIFO_UNDERRUN_E                                  ,  /* 322 */

    /******************** PP HyperG Link ports events **************************/
    CPSS_PP_HGLINK_PING_RECEIVED_E                                            ,  /* 323 */
    CPSS_PP_HGLINK_PING_SENT_E                                                ,  /* 324 */
    CPSS_PP_HGLINK_MAC_TX_OVERRUN_E                                           ,  /* 325 */
    CPSS_PP_HGLINK_MAC_TX_UNDERRUN_E                                          ,  /* 326 */
    CPSS_PP_HGLINK_RX_ERR_CNTR_RL_E                                           ,  /* 327 */
    CPSS_PP_HGLINK_BAD_PCS_TO_MAC_REFORMAT_RX_E                               ,  /* 328 */
    CPSS_PP_HGLINK_BAD_HEAD_CELL_RX_E                                         ,  /* 329 */
    CPSS_PP_HGLINK_BAD_LENGTH_CELL_RX_E                                       ,  /* 330 */
    CPSS_PP_HGLINK_FC_CELL_RX_INT_E                                           ,  /* 331 */
    CPSS_PP_HGLINK_FC_CELL_TX_INT_E                                           ,  /* 332 */
    CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC0_E                                    ,  /* 333 */
    CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC1_E                                    ,  /* 334 */
    CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC2_E                                    ,  /* 335 */
    CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC3_E                                    ,  /* 336 */
    CPSS_PP_HGLINK_ADDR_OUT_OF_RANGE_E                                        ,  /* 337 */

    /**************************** TCC-TCAM ************************************/
    CPSS_PP_TCC_TCAM_ERROR_DETECTED_E                                         ,  /* 338 */
    CPSS_PP_TCC_TCAM_BIST_FAILED_E                                            ,  /* 339 */
    /**************************** BCN *****************************************/
    CPSS_PP_BCN_COUNTER_WRAP_AROUND_ERR_E                                     ,  /* 340 */

    /**************************** GTS (ingress/egress time stamp) *************/
    CPSS_PP_GTS_GLOBAL_FIFO_FULL_E                                            ,  /* 341 */
    CPSS_PP_GTS_VALID_TIME_SAMPLE_MESSAGE_E                                   ,  /* 342 */

    /**************************** Critical HW Error ***************************/
    CPSS_PP_CRITICAL_HW_ERROR_E                                               ,  /* 343 */

    /**************************** Port related addition ***************************/
    CPSS_PP_PORT_PRBS_ERROR_QSGMII_E                                          ,  /* 344 */
    CPSS_PP_PORT_802_3_AP_E                                                   ,  /* 345 */
    CPSS_PP_PORT_802_3_AP_CORE_1_E                                            ,  /* 346 */
    CPSS_PP_PORT_802_3_AP_CORE_2_E                                            ,  /* 347 */
    CPSS_PP_PORT_802_3_AP_CORE_3_E                                            ,  /* 348 */
    CPSS_PP_PORT_802_3_AP_CORE_4_E                                            ,  /* 349 */
    CPSS_PP_PORT_802_3_AP_CORE_5_E                                            ,  /* 350 */
    CPSS_PP_PORT_802_3_AP_CORE_6_E                                            ,  /* 351 */
    CPSS_PP_PORT_802_3_AP_CORE_7_E                                            ,  /* 352 */
    CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E                                 ,  /* 353 */

    /**************************** Data Integrity related events *******************/
    CPSS_PP_DATA_INTEGRITY_ERROR_E                                            ,  /* 354 */

    /**************************** MAC table (FDB) addition ********************/
    CPSS_PP_MAC_BANK_LEARN_COUNTERS_OVERFLOW_E                                ,  /* 355 */

    CPSS_PP_PORT_EEE_E                                                        ,  /* 356 */

    CPSS_PP_OAM_EXCEPTION_KEEPALIVE_AGING_E                                   ,  /* 357 */
    CPSS_PP_OAM_EXCEPTION_EXCESS_KEEPALIVE_E                                  ,  /* 358 */
    CPSS_PP_OAM_EXCEPTION_INVALID_KEEPALIVE_E                                 ,  /* 359 */
    CPSS_PP_OAM_EXCEPTION_RDI_STATUS_E                                        ,  /* 360 */
    CPSS_PP_OAM_EXCEPTION_TX_PERIOD_E                                         ,  /* 361 */
    CPSS_PP_OAM_EXCEPTION_MEG_LEVEL_E                                         ,  /* 362 */
    CPSS_PP_OAM_EXCEPTION_SOURCE_INTERFACE_E                                  ,  /* 363 */
    CPSS_PP_OAM_ILLEGAL_ENTRY_INDEX_E                                         ,  /* 364 */

    CPSS_PP_UNI_EV_DUMMY_MAX_E,/* dummy -- must be 1 before CPSS_PP_UNI_EV_MAX_E */

    CPSS_PP_UNI_EV_MAX_E                             = (CPSS_PP_UNI_EV_DUMMY_MAX_E-1),

    /************************** Xbar events ***********************************/
    CPSS_XBAR_UNI_EV_MIN_E                         = (CPSS_PP_UNI_EV_MAX_E + 1) ,

    CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E               = (CPSS_XBAR_UNI_EV_MIN_E) ,   /* 365 */
    CPSS_XBAR_HG_LANE_COMMA_ALIGN_CHANGE_E                                    ,   /* 366 */
    CPSS_XBAR_GPORT_INVALID_TRG_DROP_CNT_INC_E                                ,   /* 367 */
    CPSS_XBAR_GPORT_DIAG_CELL_RX_CNTR_INC_E                                   ,   /* 368 */
    CPSS_XBAR_MISC_E                                                          ,   /* 369 */
    CPSS_XBAR_HG_PING_RECEIVED_E                                              ,   /* 370 */
    CPSS_XBAR_HOST_RX_FIFO_PEND_E                                             ,   /* 371 */
    CPSS_XBAR_HOST_SEND_CELL_E                                                ,   /* 372 */
    CPSS_XBAR_HG_ALIGNMENT_LOCK_CHANGE_E                                      ,   /* 373 */
    CPSS_XBAR_HG_PING_E                                                       ,   /* 374 */
    CPSS_XBAR_SD_INV_ADDR_E                                                   ,   /* 375 */
    CPSS_XBAR_SXBAR_INV_ADDR_E                                                ,   /* 376 */
    CPSS_XBAR_S_INV_ADDR_E                                                    ,   /* 377 */
    CPSS_XBAR_G_INV_ADDR_E                                                    ,   /* 378 */
    CPSS_XBAR_MC_INV_ADDR_E                                                   ,   /* 379 */

    CPSS_XBAR_UNI_EV_DUMMY_MAX_E,/* dummy -- must be 1 before CPSS_XBAR_UNI_EV_MAX_E */

    CPSS_XBAR_UNI_EV_MAX_E                        = (CPSS_XBAR_UNI_EV_DUMMY_MAX_E-1),

    /************************** Fa events *************************************/
    CPSS_FA_UNI_EV_MIN_E                        = (CPSS_XBAR_UNI_EV_MAX_E + 1)  ,

    CPSS_FA_UC_PRIO_BUF_FULL_E                    = (CPSS_FA_UNI_EV_MIN_E)    ,   /* 380 */
    CPSS_FA_MC_PRIO_BUF_FULL_E                                                ,   /* 381 */
    CPSS_FA_UC_RX_BUF_FULL_E                                                  ,   /* 382 */
    CPSS_FA_MC_RX_BUF_FULL_E                                                  ,   /* 383 */
    CPSS_FA_WD_FPORT_GRP_E                                                    ,   /* 384 */
    CPSS_FA_MC_WD_EXP_E                                                       ,   /* 385 */
    CPSS_FA_GLBL_DESC_FULL_E                                                  ,   /* 386 */
    CPSS_FA_MAC_UPD_THRS_E                                                    ,   /* 387 */
    CPSS_FA_DROP_DEV_EN_E                                                     ,   /* 388 */
    CPSS_FA_MISC_I2C_TIMEOUT_E                                                ,   /* 389 */
    CPSS_FA_MISC_STATUS_E                                                     ,   /* 390 */

    CPSS_FA_UNI_EV_DUMMY_MAX_E,/* dummy -- must be 1 before CPSS_FA_UNI_EV_MAX_E */

    CPSS_FA_UNI_EV_MAX_E                          = (CPSS_FA_UNI_EV_DUMMY_MAX_E-1)   ,

    /************************** DRAGONITE events *************************************/
    CPSS_DRAGONITE_UNI_EV_MIN_E                         = (CPSS_FA_UNI_EV_MAX_E + 1),

    CPSS_DRAGONITE_PORT_ON_E                = (CPSS_DRAGONITE_UNI_EV_MIN_E)   ,  /* 391 */
    CPSS_DRAGONITE_PORT_OFF_E                                                 ,  /* 392 */
    CPSS_DRAGONITE_DETECT_FAIL_E                                              ,  /* 393 */
    CPSS_DRAGONITE_PORT_FAULT_E                                               ,  /* 394 */
    CPSS_DRAGONITE_PORT_UDL_E                                                 ,  /* 395 */
    CPSS_DRAGONITE_PORT_FAULT_ON_STARTUP_E                                    ,  /* 396 */
    CPSS_DRAGONITE_PORT_PM_E                                                  ,  /* 397 */
    CPSS_DRAGONITE_POWER_DENIED_E                                             ,  /* 398 */
    CPSS_DRAGONITE_OVER_TEMP_E                                                ,  /* 399 */
    CPSS_DRAGONITE_TEMP_ALARM_E                                               ,  /* 400 */
    CPSS_DRAGONITE_DEVICE_FAULT_E                                             ,  /* 401 */
    CPSS_DRAGONITE_OVER_CONSUM_E                                              ,  /* 402 */
    CPSS_DRAGONITE_VMAIN_LOW_AF_E                                             ,  /* 403 */
    CPSS_DRAGONITE_VMAIN_LOW_AT_E                                             ,  /* 404 */
    CPSS_DRAGONITE_VMAIN_HIGH_E                                               ,  /* 405 */
    CPSS_DRAGONITE_READ_EVENT_E                                               ,  /* 406 */
    CPSS_DRAGONITE_WRITE_EVENT_E                                              ,  /* 407 */
    CPSS_DRAGONITE_ERROR_E                                                    ,  /* 408 */

    CPSS_DRAGONITE_UNI_EV_DUMMY_MAX_E,/* dummy -- must be 1 before CPSS_DRAGONITE_UNI_EV_MAX_E */

    CPSS_DRAGONITE_UNI_EV_MAX_E                          = (CPSS_DRAGONITE_UNI_EV_DUMMY_MAX_E-1),
    /************************** End of events *********************************/
    CPSS_UNI_EV_DUMMY_MAX_E,/* dummy -- must be 1 before CPSS_UNI_EVENT_COUNT_E */

    CPSS_UNI_EVENT_COUNT_E                           = (CPSS_UNI_EV_DUMMY_MAX_E),

    CPSS_UNI_RSRVD_EVENT_E                             = (0xFFFF            )     /* 0xFFFF */

} CPSS_UNI_EV_CAUSE_ENT;

/* macro to define the size of GT_U32 bmp represent the events bmp */
#define CPSS_UNI_EV_BITMAP_SIZE_CNS       ((CPSS_UNI_EVENT_COUNT_E + 31) / 32)




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenEventUnifyTypesh */

