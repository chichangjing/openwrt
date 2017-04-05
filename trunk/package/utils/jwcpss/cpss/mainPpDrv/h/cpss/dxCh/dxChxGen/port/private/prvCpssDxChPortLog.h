/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChPortLogh
#define __prvCpssDxChPortLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_MIN_SPEED_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_DXCH_PA_UNIT_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_BUFFERS_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_COM_PHY_H_SUB_SEQ_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_FC_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_FEC_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_GROUP_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_HOL_FC_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_DXCH_PORT_MAPPING_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_PFC_ENABLE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_PIP_PROTOCOL_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SERDES_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SERDES_SPEED_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_UNITS_ID_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_PORT_TX_SHARED_DP_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_EL_DB_OPERATION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CPUPORT_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_DETAILED_PORT_MAP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PIZZA_PROFILE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_ALIGN90_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_AP_INTROP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_AP_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_AP_STATS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_AP_STATUS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_COMBO_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_ECN_ENABLERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_EEE_LPI_STATUS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_EOM_MATRIX_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_MAC_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_MAP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_SERDES_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_PORT_CNM_GENERATION_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_PORT_CN_PROFILE_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_PORT_TX_WRTD_MASK_LSB_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_PRIORITY_MATR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_SHADOW_PORT_MAP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TXQDEF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_CPU_PORTDEF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PA_UNIT_ENT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PIZZA_PORT_ID_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_SliceNumXPortPizzaCfgSlicesList_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PIZZA_SLICE_ID_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_DETAILED_PORT_MAP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MIN_SPEED_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MIN_SPEED_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PA_UNIT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PIZZA_PROFILE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PIZZA_PROFILE_STC_PTR_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_ALIGN90_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_AP_INTROP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_AP_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_AP_STATS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_AP_STATUS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_BUFFERS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_BUFFERS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_COMBO_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_COM_PHY_H_SUB_SEQ_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_ECN_ENABLERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_EEE_LPI_STATUS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_EOM_MATRIX_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_FC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_FC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_FEC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_FEC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_GROUP_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_HOL_FC_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_MAC_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_MAPPING_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_MAP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PFC_ENABLE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PFC_ENABLE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PIP_PROTOCOL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC_PTR_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_SPEED_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_UNITS_ID_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PORT_UNITS_ID_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_CNM_GENERATION_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_CN_PROFILE_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_TX_SHARED_DP_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_TX_SHARED_DP_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_PORT_TX_WRTD_MASK_LSB_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC_PTR_pizzaDeviceStatePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_CPSS_DXCH_PORT_ALIGN90_PARAMS_STC_PTR_serdesParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_ADJUST_OPERATION_ENT_bcOp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_PIZZA_ARBITER_UNIT_ENT_unitId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIN_SPEED_ENT_minimalPortSpeedMBps;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PA_UNIT_ENT_unit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PIZZA_PROFILE_STC_PTR_pizzaProfilePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_AP_INTROP_STC_PTR_apIntropPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_AP_PARAMS_STC_PTR_apParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC_PTR_portAnAdvertismentPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_BUFFERS_MODE_ENT_bufferMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT_fifoType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT_mType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT_packetLength;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT_prioritySpeedLocation;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_COMBO_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_COM_PHY_H_SUB_SEQ_ENT_seqType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_ECN_ENABLERS_STC_PTR_enablersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_FC_MODE_ENT_fcMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_FEC_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_GROUP_ENT_portGroup;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_HOL_FC_ENT_modeFcHol;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT_counterMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_MAC_PARAMS_STC_PTR_macPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_MAPPING_TYPE_ENT_origPortType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_MAP_STC_PTR_portMapArrayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT_PTR_portTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT_portType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT_pfcCountMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PFC_ENABLE_ENT_pfcEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC_PTR_lossyDropConfigPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC_PTR_pfcProfileCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT_type;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT_type;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PIP_PROTOCOL_ENT_protocol;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC_PTR_portGroupPortSpeed2SliceNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_ENT_portTuningMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_CONFIG_STC_PTR_serdesCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC_PTR_seqLinePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC_PTR_serdesRxCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_SPEED_ENT_serdesFrequency;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR_tuneValuesPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC_PTR_serdesTxCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT_pllClkSelect;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT_value;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT_recoveryClkType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT_clockSelect;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT_bcChangeEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT_bcMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC_PTR_configsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT_policy;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT_mtu;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PORT_UNITS_ID_ENT_unitId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORTS_BMP_STC_portsBmp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_CNM_GENERATION_CONFIG_STC_PTR_cnmGenerationCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC_PTR_fbCalcCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_CN_PROFILE_CONFIG_STC_PTR_cnProfileCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_DUPLEX_ENT_dMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_EGRESS_CNT_MODE_ENT_setModeBmp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_FLOW_CONTROL_ENT_state;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_INTERFACE_MODE_ENT_ifMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT_counterId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_MAC_COUNTERS_ENT_cntrName;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_MODE_SPEED_STC_PTR_portModeSpeedOptionsArrayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_PCS_RESET_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT_enable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_REF_CLOCK_SOURCE_ENT_refClockSource;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_RX_FC_PROFILE_SET_ENT_profileSet;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_SERDES_TUNING_TYPE_ENT_portAutoTuningType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_DROP_PROFILE_SET_ENT_pfSet;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_DROP_PROFILE_SET_ENT_profile;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_DROP_PROFILE_SET_ENT_profileSet;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC_PTR_weigthsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_Q_ARB_GROUP_ENT_arbGroup;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS_PTR_tailDropProfileParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT_profile;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT_profileSet;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_SHARED_DP_MODE_ENT_enableMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC_PTR_enablersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_WRR_MODE_ENT_wrrMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_WRR_MTU_ENT_wrrMtu;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_WRTD_MASK_LSB_STC_PTR_maskLsbPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_XGMII_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_XG_FIXED_IPG_ENT_ipgBase;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_bcValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_ppmValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_apEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_avbModeEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enableSharing;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_firstLine;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_invertRx;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_invertTx;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_pauseAdvertise;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_powerUp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_targetHwDev;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_firstPhysicalPortNumber;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_localPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_targetPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_burstSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_timeout;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_rxSerdesLaneArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_serdesOptAlgBmpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_txSerdesLaneArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_almostFullThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_channelSpeedResMbps;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cntrIdx;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cntrNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_descFifoThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dropThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_fieldIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_fullThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_gigPortsSlowRateRatio;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_gigPortsTokensRate;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ipg;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_laneBmp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lanesBmp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_liLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lineNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_localPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maskIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maskValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxBufNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxDescNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxSharedBufferLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mcFifo;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mcastMaxBufNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mcastMaxDescNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_minXFifoReadThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfOp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_origPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pfcCounterNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pfcTimer;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pipProfile;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pipeBandwithInGbps;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_poolNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portGroup;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portGroupBmp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portGroupIdx;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portGroupNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portMapArraySize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portMaxBuffLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portMaxDescrLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portSerdesGroup;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portSpeedIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_qcnSampleInterval;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_qlenOld;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rxBufLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rxSniffMaxBufNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rxSniffMaxDescNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_samplingTime;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_serdesNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_serdesOptAlgBmp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_shaperToPortRateRatio;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sharedBufLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sharedDescLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sliceID;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sliceNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sliceNumToConfig;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_speed;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tcQueue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_timer;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tmBandwidthMbps;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tsLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_twLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_txFifoThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_txSniffMaxBufNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_txSniffMaxDescNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_xgPortsTokensRate;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_xoffLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_xoffThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_xonLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_xonThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_cntrSetNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_macSaLsb;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_max_LF;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_min_LF;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_number;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_optionsArrayLen;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_tcQueue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_trafClass;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_trafficClass;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_wrrWeight;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_ADJUST_OPERATION_ENT_PTR_bcOpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DETAILED_PORT_MAP_STC_PTR_portMapShadowPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC_PTR_pizzaDeviceStatePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIN_SPEED_ENT_PTR_minimalPortSpeedResolutionInMBpsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PIZZA_PROFILE_STC_PTR_PTR_pizzaProfilePtrPtrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_AP_INTROP_STC_PTR_apIntropPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_AP_PARAMS_STC_PTR_apParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_AP_STATS_STC_PTR_apStatsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_AP_STATUS_STC_PTR_apStatusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC_PTR_portAnAdvertismentPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_BUFFERS_MODE_ENT_PTR_bufferModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT_PTR_mTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT_PTR_packetLengthPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT_PTR_prioritySpeedLocationPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_COMBO_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_ECN_ENABLERS_STC_PTR_enablersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_EEE_LPI_STATUS_STC_PTR_statusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_EOM_MATRIX_STC_PTR_matrixPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_FC_MODE_ENT_PTR_fcModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_FEC_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT_PTR_typePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT_PTR_counterModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_MAC_PARAMS_STC_PTR_macPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_MAP_STC_PTR_portMapArrayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT_PTR_pfcCountModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_PFC_ENABLE_ENT_PTR_pfcEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC_PTR_lossyDropConfigPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC_PTR_pfcProfileCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC_PTR_PTR_portGroupPortSpeed2SliceNumPtrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT_PTR_rxTuneStatusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT_PTR_statusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT_PTR_txTuneStatusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_CONFIG_STC_PTR_serdesCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC_PTR_seqLinePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC_PTR_serdesRxCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR_serdesTunePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR_tuneValuesPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC_PTR_serdesTxCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC_PTR_dropCntrStcPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT_PTR_pllClkSelectPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT_PTR_valuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT_PTR_bcChangeEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT_PTR_bcModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC_PTR_configsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT_PTR_policyPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT_PTR_mtuPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_UNITS_ID_ENT_PTR_unitIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_ATTRIBUTES_STC_PTR_portAttributSetArrayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_CNM_GENERATION_CONFIG_STC_PTR_cnmGenerationCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC_PTR_fbCalcCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_CN_PROFILE_CONFIG_STC_PTR_cnProfileCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_DUPLEX_ENT_PTR_dModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_EGRESS_CNTR_STC_PTR_egrCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_EGRESS_CNT_MODE_ENT_PTR_setModeBmpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_FLOW_CONTROL_ENT_PTR_statePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_INTERFACE_MODE_ENT_PTR_ifModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_MAC_COUNTER_SET_STC_PTR_portMacCounterSetArrayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_MAC_STATUS_STC_PTR_portMacStatusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_MAC_TYPE_ENT_PTR_portMacTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_MODE_SPEED_STC_PTR_currentModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT_PTR_enablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_REF_CLOCK_SOURCE_ENT_PTR_refClockSourcePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_RX_FC_PROFILE_SET_ENT_PTR_profileSetPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_SERDES_TUNING_TYPE_ENT_PTR_portAutoTuningTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_DROP_PROFILE_SET_ENT_PTR_profileSetPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC_PTR_weigthsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_Q_ARB_GROUP_ENT_PTR_arbGroupPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS_PTR_tailDropProfileParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT_PTR_profileSetPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_SHARED_DP_MODE_ENT_PTR_enableModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC_PTR_enablersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_WRR_MODE_ENT_PTR_wrrModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_WRR_MTU_ENT_PTR_wrrMtuPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_WRTD_MASK_LSB_STC_PTR_maskLsbPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_XGMII_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_XG_FIXED_IPG_ENT_PTR_ipgBasePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_bcValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_ppmValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_apEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_avbModeEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_captureIsDonePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_cdrLockPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_enableSharingPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_gbLockPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_invertRxPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_invertTxPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isLinkUpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isLocalFaultPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isOccupiedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isRemoteFaultPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_pauseAdvertisePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_signalStatePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_supportedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_targetHwDevPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PHYSICAL_PORT_NUM_PTR_physicalPortNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PHYSICAL_PORT_NUM_PTR_portNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_targetPortPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_burstSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_cpuRxBufCntPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_intropAbilityMaxIntervalPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_numPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_numberPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_timeoutPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actualSpeedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_agedBuffersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_almostFullThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_apResolvedPortsBmpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_baseAddrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_baudRatePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cntPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_descFifoThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dfeResPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dropThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_droppedCntPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_fullThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_gigPortsSlowRateRatioPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_gigPortsTokensRatePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_highSpeedPortIdxArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_highSpeedPortNumberPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ipgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_laneNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_lengthPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_liLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_localPortNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maskIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maskValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxBufNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxDescNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxIlknIFSpeedMbpsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxSharedBufferLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mcCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mcFifoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mcastMaxBufNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mcastMaxDescNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_minXFifoReadThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfBuffersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfConfiguredSlicesPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfPacketsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numberPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pfcCounterNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pfcCounterValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pipProfilePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pipeBandwithInGbpsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_poolNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_portMaxBuffLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_portMaxDescrLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_portNumArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_portSerdesGroupPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_portSpeedIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_qcnSampleIntervalPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_qlenOldPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_receivedCntPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rowSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rxBufLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rxNumberPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rxSerdesLaneArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rxSniffMaxBufNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rxSniffMaxDescNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_shaperToPortRateRatioPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_sharedBufLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_sharedDescLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_sliceNumUsedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_speedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_speedResulutionMBpsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcQueuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tmBandwidthMbpsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tsLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_twLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_txFifoThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_txNumberPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_txSerdesLaneArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_txSniffMaxBufNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_txSniffMaxDescNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_txqDqPortNumArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_txqLLPortNumArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_unitIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_voltagePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_xgPortsTokensRatePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_xoffLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_xoffThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_xonLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_xonThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_cntrValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_counterPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_countersArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_macSaLsbPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_numberPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_tcPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_tcQueuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_wrrWeightPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChPortApEnableSet_E = (CPSS_LOG_LIB_PORT_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChPortApEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApPortConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApPortConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApPortStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApResolvedPortsBmpGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApSetActiveMode_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApLock_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApUnLock_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApStatsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApStatsReset_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApIntropSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortApIntropGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortAutoNegAdvertismentConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortAutoNegAdvertismentConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortAutoNegMasterModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortAutoNegMasterModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortModeSpeedAutoDetectAndConfig_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInbandAutoNegRestart_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGePrbsIntReplaceEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGePrbsIntReplaceEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChGlobalXonLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChGlobalXonLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChGlobalXoffLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChGlobalXoffLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRxFcProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRxFcProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXonLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXonLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXoffLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXoffLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRxBufLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRxBufLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCpuRxBufCountGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFcHolSysModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortBuffersModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortBuffersModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGroupXonLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGroupXonLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGroupXoffLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGroupXoffLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGroupRxBufLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGroupRxBufLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCrossChipFcPacketRelayEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCrossChipFcPacketRelayEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortClearByBufferEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortClearByBufferEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGlobalRxBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGroupRxBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRxBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGlobalPacketNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRxNumOfAgedBuffersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRxMcCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortBufMemFifosThresholdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortBufMemFifosThresholdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxdmaBurstLimitEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxdmaBurstLimitEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxdmaBurstLimitThresholdsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxdmaBurstLimitThresholdsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnProfileQueueThresholdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnProfileQueueThresholdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnProfileThresholdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnProfileThresholdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnQueueAwareEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnQueueAwareEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnFrameQueueSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnFrameQueueGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnEtherTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnEtherTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnSpeedIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnSpeedIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnFcEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnFcEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnPauseTriggerEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnPauseTriggerEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnFcTimerSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnFcTimerGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnPanicPauseThresholdsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnPanicPauseThresholdsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnTerminationEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnTerminationEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnProfileQueueConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnProfileQueueConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnQueueStatusModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnQueueStatusModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnSampleEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnSampleEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnFbCalcConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnFbCalcConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnPacketLengthSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnPacketLengthGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnMessageGenerationConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnMessageGenerationConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnPrioritySpeedLocationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnPrioritySpeedLocationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnMessageTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnMessageTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnMessageTriggeringStateSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCnMessageTriggeringStateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesComboModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesComboModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortComboModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortComboModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortComboPortActiveMacSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortComboPortActiveMacGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacSaLsbSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacSaLsbGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDuplexModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDuplexModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDuplexAutoNegEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDuplexAutoNegEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFlowCntrlAutoNegEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFlowCntrlAutoNegEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSpeedAutoNegEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSpeedAutoNegEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFlowControlEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFlowControlEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPeriodicFcEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPeriodicFcEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortBackPressureEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortBackPressureEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortLinkStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortForceLinkPassEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortForceLinkPassEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortForceLinkDownEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortForceLinkDownEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMruSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMruGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMruProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMruProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortProfileMruSizeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortProfileMruSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMruExceptionCommandSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMruExceptionCommandGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMruExceptionCpuCodeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMruExceptionCpuCodeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCrcCheckEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortCrcCheckEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXGmiiModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXGmiiModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIpgBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIpgBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIpgSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIpgGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortExtraIpgSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortExtraIpgGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXgmiiLocalFaultGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXgmiiRemoteFaultGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInternalLoopbackEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInternalLoopbackEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInbandAutoNegEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInbandAutoNegEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortAttributesOnPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPreambleLengthSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPreambleLengthGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacSaBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacSaBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPaddingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPaddingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortExcessiveCollisionDropEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortExcessiveCollisionDropEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInBandAutoNegBypassEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInBandAutoNegBypassEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacErrorIndicationPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacErrorIndicationPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacErrorIndicationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesGroupGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesCalibrationStartSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFlowControlModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFlowControlModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacResetStateSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortForward802_3xEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortForward802_3xEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortForwardUnknownMacControlFramesEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortForwardUnknownMacControlFramesEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPeriodicFlowControlIntervalSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPeriodicFlowControlIntervalGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPeriodicFlowControlIntervalSelectionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPeriodicFlowControlIntervalSelectionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPeriodicFlowControlCounterSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPeriodicFlowControlCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortUnitInfoGetByAddr_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortUnitInfoGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDiagVoltageGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDiagTemperatureGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEcnMarkingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEcnMarkingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEcnMarkingTailDropProfileEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEcnMarkingTailDropProfileEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEeeLpiRequestEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEeeLpiRequestEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEeeLpiManualModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEeeLpiManualModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEeeLpiTimeLimitsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEeeLpiTimeLimitsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEeeLpiStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortsFastInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInterfaceModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInterfaceModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortModeSpeedSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortExtendedModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortExtendedModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInterfaceSpeedSupportGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFecModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFecModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRefClockSourceOverrideEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortRefClockSourceOverrideEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortResourceTmBandwidthSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortResourceTmBandwidthGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortInterlakenCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIlknChannelSpeedResolutionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIlknChannelSpeedResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIlknChannelSpeedSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIlknChannelSpeedGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIlknChannelEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortIlknChannelEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPhysicalPortMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPhysicalPortMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPhysicalPortMapIsValidGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPhysicalPortMapReverseMappingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPhysicalPortDetailedMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPcsLoopbackModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPcsLoopbackModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPcsResetSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPcsGearBoxStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXgLanesSwapEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXgLanesSwapEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXgPscLanesSwapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortXgPscLanesSwapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcProfileIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcProfileIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcProfileQueueConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcProfileQueueConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcCountingModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcCountingModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcGlobalDropEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcGlobalDropEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcGlobalQueueConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcGlobalQueueConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcTimerMapEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcTimerMapEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcTimerToQueueMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcTimerToQueueMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcShaperToPortRateRatioSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcShaperToPortRateRatioGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcForwardEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcForwardEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcLossyDropQueueEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcLossyDropQueueEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcLossyDropConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcLossyDropConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcTerminateFramesEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcTerminateFramesEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortUnrecognizedMacControlFramesCmdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortUnrecognizedMacControlFramesCmdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortFlowControlPacketsCntGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcQueueCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcSourcePortToPfcCounterSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcSourcePortToPfcCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPfcCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalEtherTypeProtocolSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalEtherTypeProtocolGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipTrustEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipTrustEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipVidClassificationEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipVidClassificationEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipPrioritySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipPriorityGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalProfilePrioritySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalProfilePriorityGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalVidClassificationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalVidClassificationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalMacDaClassificationEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalMacDaClassificationEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalBurstFifoThresholdsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalBurstFifoThresholdsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipGlobalDropCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPipDropCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesPowerStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortGroupSerdesPowerStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesTuningSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesTuningGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesLaneTuningSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesLaneTuningGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesResetStateSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTune_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAlign90StateSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAlign90StatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTuneExt_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTuneStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesPolaritySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesPolarityGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesLoopbackModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesLoopbackModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEomDfeResGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEomMatrixGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEomBaudRateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesTxEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesTxEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesPpmSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesPpmGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesManualTxConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesManualTxConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesManualRxConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesManualRxConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesSignalDetectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesLaneSignalDetectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesCDRLockStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTuneOptAlgSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTuneOptAlgGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTuneOptAlgRun_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTuneTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTuneTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesSequenceSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesSequenceGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesAutoTuneResultsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSerdesEnhancedAutoTune_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSpeedSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSpeedGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortStatInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMacCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersOnPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCounterCaptureGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersCaptureOnPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersCaptureTriggerSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersCaptureTriggerGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEgressCntrModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEgressCntrModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortEgressCntrsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersClearOnReadSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersClearOnReadGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersRxHistogramEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacCountersTxHistogramEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacOversizedPacketsCounterModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortMacOversizedPacketsCounterModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortStatTxDebugCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherSecondRecoveryClkEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherSecondRecoveryClkEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherPllRefClkSelectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherPllRefClkSelectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherRecoveryClkConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherRecoveryClkConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherRecoveryClkDividerValueSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherRecoveryClkDividerValueGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQueueEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQueueEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQueueingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQueueingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQueueTxEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQueueTxEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxFlushQueuesSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQShaperEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQShaperEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQShaperProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQShaperProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBindPortToSchedulerProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBindPortToSchedulerProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTx4TcTailDropProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTx4TcTailDropProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSniffedPcktDescrLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSniffedPcktDescrLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcastPcktDescrLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcastPcktDescrLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxWrrGlobalParamSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxWrrGlobalParamGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperGlobalParamsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperGlobalParamsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQWrrProfileSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQWrrProfileGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQArbGroupSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQArbGroupGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBindPortToDpSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBindPortToDpGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxDescNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTx4TcDescNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTx4TcBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxToCpuShaperModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxToCpuShaperModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharingGlobalResourceEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharingGlobalResourceEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharedGlobalResourceLimitsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharedGlobalResourceLimitsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharedPolicySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharedPolicyGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxWatchdogEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxWatchdogEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropUcEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropUcEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBufferTailDropEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBufferTailDropEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBuffersSharingMaxLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxBuffersSharingMaxLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxDp1SharedEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxDp1SharedEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTcSharedProfileEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTcSharedProfileEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperTokenBucketMtuSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperTokenBucketMtuGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxRandomTailDropEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxRandomTailDropEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTxPortAllShapersDisable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTxPortShapersDisable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSchedulerDeficitModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSchedulerDeficitModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxRequestMaskSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxRequestMaskGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxRequestMaskIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxRequestMaskIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperBaselineSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperBaselineGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperConfigurationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperConfigurationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropBufferConsumptionModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropBufferConsumptionModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTcProfileSharedPoolSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTcProfileSharedPoolGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcastBuffersLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcastBuffersLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSniffedBuffersLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSniffedBuffersLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharedPoolLimitsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharedPoolLimitsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxProfileWeightedRandomTailDropEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxProfileWeightedRandomTailDropEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropWrtdMasksSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropWrtdMasksGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxResourceHistogramThresholdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxResourceHistogramThresholdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxResourceHistogramCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxGlobalDescNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxGlobalBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcastDescNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcastBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSniffedDescNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSniffedBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharedResourceDescNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSharedResourceBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperOnStackAsGigEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperOnStackAsGigEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortProfileTxByteCountChangeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortProfileTxByteCountChangeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxByteCountChangeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxByteCountChangeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxByteCountChangeValueSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxByteCountChangeValueGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxGlobalDescLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxGlobalDescLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcFifoSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcFifoGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcFifoArbiterWeigthsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcFifoArbiterWeigthsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQueueGlobalTxEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQueueGlobalTxEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcShaperMaskSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxMcShaperMaskGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropProfileBufferConsumptionModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropProfileBufferConsumptionModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperAvbModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxShaperAvbModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQShaperAvbModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxQShaperAvbModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSchedulerProfileCountModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxSchedulerProfileCountModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropMaskSharedBuffEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropMaskSharedBuffEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropSharedBuffMaxLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTxTailDropSharedBuffMaxLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortTx4TcMcBufNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPizzaArbiterDevStateInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPizzaArbiterDevStateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDynamicPATxQHighSpeedPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDynamicPATxQHighSpeedPortDumpGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPizzaArbiterIfPortStateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPizzaArbiterIfSliceStateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPizzaArbiterIfUserTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortPizzaArbiterIfUserTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChSlicesNumSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChSlicesNumGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChSliceStateSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChSliceStateGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChPortLogh */
