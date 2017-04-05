/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPolicerLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChPolicerLogh
#define __prvCpssDxChPolicerLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_METER_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_COUNTING_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_STORM_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_MODIFY_DSCP_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_MODIFY_UP_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_POLICER_REMARK_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_COUNTING_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_COUNTING_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_STAGE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_STORM_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_confLevel;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DROP_MODE_TYPE_ENT_dropRedMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DROP_MODE_TYPE_ENT_dropType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT_cntrMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR_billingCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METER_MODE_ENT_meterMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT_resolution;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbInParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR_mngCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT_cntrResolution;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_cntrSet;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT_mngCntrSet;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT_mngCntrType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT_timestampTagMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT_ttPacketSizeMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR_billingIndexCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_COUNTERS_STC_PTR_countersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_COUNTING_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT_maxSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT_model;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT_meteringCalcMethod;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT_type;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR_qosParamPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT_remarkTableType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_STORM_TYPE_ENT_stormType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbInParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PACKET_CMD_ENT_cmdTrigger;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_POLICER_MRU_ENT_mru;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_POLICER_PACKET_SIZE_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_POLICER_PACKET_SIZE_MODE_ENT_packetSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_POLICER_TB_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_cbsPbsCalcOnFail;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_couplingFlag0;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bucketIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cirPirAllowedDeviation;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cntrValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_envelopeSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_greenQosTableRemarkIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_interval;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxEnvelopeSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxRate;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_meterEntryBurstSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_meterEntryRate;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_plrIndexMask;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_policerIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_qosProfileIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_redQosTableRemarkIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_remarkParamValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_startAddress;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_startEntryIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_stopAddress;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_yellowQosTableRemarkIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DROP_MODE_TYPE_ENT_PTR_dropTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT_PTR_cntrModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC_PTR_billingCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT_PTR_entryTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT_PTR_resolutionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbOutParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbParamsArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR_tbParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC_PTR_mngCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT_PTR_cntrResolutionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT_PTR_timestampTagModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT_PTR_ttPacketSizeModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC_PTR_billingIndexCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_BUCKETS_STATE_STC_PTR_bucketsStatePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_COUNTERS_STC_PTR_countersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_COUNTING_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT_PTR_maxSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT_PTR_modelPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT_PTR_meteringCalcMethodPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT_PTR_typePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_QOS_PARAM_STC_PTR_qosParamPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbOutParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_TB_PARAMS_STC_PTR_tbParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_POLICER_PACKET_SIZE_MODE_ENT_PTR_packetSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_cbsPbsCalcOnFailPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_couplingFlag0Ptr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cirPirAllowedDeviationPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cntrValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_entryAddrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_envelopeSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_greenQosTableRemarkIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_intervalPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_plrIndexMaskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_redQosTableRemarkIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_startAddressPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_stopAddressPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_yellowQosTableRemarkIndexPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerInit_E = (CPSS_LOG_LIB_POLICER_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPacketSizeModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerDropRedModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPacketSizeModeForTunnelTermSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerTokenBucketModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEntryInvalidate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEntryMeterParamsCalculate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerCountersSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerBillingCountingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerBillingCountingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerCountingModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerCountingModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerBillingCountersModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerBillingCountersModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerPacketSizeModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerPacketSizeModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeterResolutionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeterResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerDropTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerDropTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerCountingColorModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerCountingColorModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerManagementCntrsResolutionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerManagementCntrsResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerPacketSizeModeForTunnelTermSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerPacketSizeModeForTunnelTermGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringAutoRefreshRangeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringAutoRefreshRangeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringAutoRefreshIntervalSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringAutoRefreshIntervalGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringEntryRefresh_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerPortMeteringEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerPortMeteringEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMruSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMruGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerErrorGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerErrorCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerManagementCountersSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerManagementCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerMeteringEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerEntryMeterParamsCalculate_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerBillingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerBillingEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerQosRemarkingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxCh3PolicerQosRemarkingEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEgressQosRemarkingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEgressQosRemarkingEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerStageMeterModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerStageMeterModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMeteringOnTrappedPktsEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMeteringOnTrappedPktsEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortStormTypeIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortStormTypeIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEgressL2RemarkModelSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEgressL2RemarkModelGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEgressQosUpdateEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEgressQosUpdateEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerVlanCntrSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerVlanCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerVlanCountingPacketCmdTriggerSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerVlanCountingPacketCmdTriggerGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerVlanCountingModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerVlanCountingModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPolicyCntrSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPolicyCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMemorySizeModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMemorySizeModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerCountingWriteBackCacheFlush_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMeteringCalcMethodSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMeteringCalcMethodGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerCountingTriggerByPortEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerCountingTriggerByPortEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortModeAddressSelectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortModeAddressSelectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerTriggerEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerTriggerEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupMeteringEntryRefresh_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupErrorGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupErrorCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupManagementCountersSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupManagementCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupMeteringEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupMeteringEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupBillingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupBillingEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupPolicyCntrSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupPolicyCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerTrappedPacketsBillingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerTrappedPacketsBillingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerLossMeasurementCounterCaptureEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerLossMeasurementCounterCaptureEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEAttributesMeteringModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerEAttributesMeteringModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerFlowIdCountingCfgSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerFlowIdCountingCfgGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerHierarchicalTableEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerHierarchicalTableEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerSecondStageIndexMaskSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerSecondStageIndexMaskGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortStormTypeInFlowModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortStormTypeInFlowModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPacketSizeModeForTimestampTagSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPacketSizeModeForTimestampTagGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMeteringEntryEnvelopeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMeteringEntryEnvelopeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMeterTableFlowBasedIndexConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerMeterTableFlowBasedIndexConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerTokenBucketMaxRateSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerTokenBucketMaxRateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerQosProfileToPriorityMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerQosProfileToPriorityMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerPortGroupBucketsCurrentStateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPolicerBucketsCurrentStateGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChPolicerLogh */
