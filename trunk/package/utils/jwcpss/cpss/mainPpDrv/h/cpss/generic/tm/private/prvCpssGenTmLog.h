/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenTmLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssGenTmLogh
#define __prvCpssGenTmLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_TM_DP_SOURCE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_DROP_PROFILE_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_SCHD_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_ELIG_PRIO_FUNC_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_TREE_CHANGE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_TM2TM_CHANNEL_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_TM_COLOR_NUM_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_TM_DP_SOURCE_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_TM_ELIG_FUNC_NODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_TM_LEVEL_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_TM_PORT_BW_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_TM_SCHD_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_TM_DROP_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_TM_ELIG_FUNC_QUEUE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_TM2TM_CNTRL_PKT_STRUCT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM2TM_DELTA_RANGE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM2TM_EXTERNAL_HDR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_A_NODE_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_BAP_UNIT_ERROR_STATUS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_B_NODE_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_C_NODE_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_DROP_PROFILE_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_LEVEL_PERIODIC_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_LIB_INIT_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_NODE_STATUS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_PORT_DROP_PER_COS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_PORT_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_PORT_STATUS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_QMR_PKT_STATISTICS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_QUANTUM_LIMITS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_QUEUE_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_RCB_PKT_STATISTICS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_SHAPING_PROFILE_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_TREE_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_UNITS_ERROR_STATUS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_ELIG_PRIO_FUNC_OUT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_TM_SCHD_MODE_ENT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_TM2TM_CHANNEL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM2TM_CNTRL_PKT_STRUCT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM2TM_DELTA_RANGE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM2TM_EXTERNAL_HDR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_A_NODE_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_BAP_UNIT_ERROR_STATUS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_B_NODE_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_COLOR_NUM_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_C_NODE_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_DP_SOURCE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_ELIG_FUNC_NODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_ELIG_PRIO_FUNC_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_LEVEL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_LEVEL_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_LEVEL_PERIODIC_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_LIB_INIT_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_NODE_STATUS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_PORT_BW_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_PORT_DROP_PER_COS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_PORT_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_PORT_STATUS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_QMR_PKT_STATISTICS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_QUANTUM_LIMITS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_QUEUE_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_RCB_PKT_STATISTICS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_SHAPING_PROFILE_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_TREE_CHANGE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_TREE_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_TM_UNITS_ERROR_STATUS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_color;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM2TM_CHANNEL_ENT_channel;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM2TM_CHANNEL_ENT_remoteLevel;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM2TM_CNTRL_PKT_STRUCT_STC_PTR_ctrlPacketStrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM2TM_DELTA_RANGE_STC_PTR_rangePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM2TM_EXTERNAL_HDR_STC_PTR_extHeadersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_A_NODE_PARAMS_STC_PTR_aParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_A_NODE_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_B_NODE_PARAMS_STC_PTR_bParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_B_NODE_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_COLOR_NUM_ENT_colorNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_COLOR_NUM_ENT_number;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_C_NODE_PARAMS_STC_PTR_cParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_C_NODE_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_DP_SOURCE_ENT_PTR_dpSourcePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_DP_SOURCE_ENT_source;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_DROP_PROFILE_PARAMS_STC_PTR_profileParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_DROP_PROFILE_PARAMS_STC_PTR_profileStcPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_DROP_PROFILE_PARAMS_STC_PTR_profileStrArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_ELIG_FUNC_NODE_ENT_eligPrioFuncId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_ELIG_PRIO_FUNC_STC_PTR_funcOutArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_LEVEL_ENT_bpLevel;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_LEVEL_ENT_level;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_LEVEL_ENT_localLevel;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_LEVEL_ENT_srcLevel;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_LEVEL_PERIODIC_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_LIB_INIT_PARAMS_STC_PTR_tmLibInitParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_PORT_BW_ENT_portSpeed;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_PORT_DROP_PER_COS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_PORT_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_QUEUE_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_QUEUE_PARAMS_STC_PTR_qParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_SCHD_MODE_ENT_PTR_schdModeArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TM_SHAPING_PROFILE_PARAMS_STC_PTR_profileStrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_8_PTR_nodeName;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_8_PTR_portName;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_8_PTR_queueName;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_PTR_priosPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_portExtBp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_pcbs;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_pebs;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_probabilityArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_quantumArrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_aNodeInd;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_aNodesNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bNodeInd;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bNodesNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bpOffset;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bpXoff;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bpXon;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bytes;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cNodeInd;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cNodesNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cbs;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cirBw;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cosSelector;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ctrlHeader;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dpType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ebs;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_egressElements;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_eirBw;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_eligPrioFunc;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_eligPrioFuncPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_firstQueueInRange;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxProbability;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_nodeAIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_nodeBIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_nodeIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portDp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portInd;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_profileInd;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_profileOffset;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_quantum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queueInd;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queueRangeSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queuesNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_status;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_wredProfileRef;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_PTR_dataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_qmapHeader;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_regAddr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_bapNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_A_NODE_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_BAP_UNIT_ERROR_STATUS_STC_PTR_bapErrorsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_B_NODE_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_C_NODE_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_DP_SOURCE_ENT_PTR_sourcePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_DROP_PROFILE_PARAMS_STC_PTR_profileStcPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_LEVEL_ENT_PTR_levelPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_NODE_STATUS_STC_PTR_statusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_PORT_DROP_PER_COS_STC_PTR_cosParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_PORT_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_PORT_STATUS_STC_PTR_statusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_QMR_PKT_STATISTICS_STC_PTR_pktCntPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_QUANTUM_LIMITS_STC_PTR_quantumLimitsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_QUEUE_PARAMS_STC_PTR_paramsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_RCB_PKT_STATISTICS_STC_PTR_pktCntPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_SHAPING_PROFILE_PARAMS_STC_PTR_profileStrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_TREE_CHANGE_STC_PTR_changePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_TREE_PARAMS_STC_PTR_tmTreeParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TM_UNITS_ERROR_STATUS_STC_PTR_unitsErrorPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_nodeIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_8_PTR_PTR_nodeNamePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_priosPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_aNodeIndPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_averageQueueLengthPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_bNodeIndPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cNodeIndPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_curveIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_instantQueueLengthPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_portDpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_profileIndPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_profileIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueIndPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_statusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_colorNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_resolutionPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssTm2TmGlobalConfig_E = (CPSS_LOG_LIB_TM_E << 16),
    PRV_CPSS_LOG_FUNC_cpssTm2TmChannelPortConfig_E,
    PRV_CPSS_LOG_FUNC_cpssTm2TmChannelNodeConfig_E,
    PRV_CPSS_LOG_FUNC_cpssTm2TmDpQueryResponceLevelSet_E,
    PRV_CPSS_LOG_FUNC_cpssTm2TmLcConfig_E,
    PRV_CPSS_LOG_FUNC_cpssTm2TmEgressDropAqmModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssTm2TmIngressDropAqmModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssTm2TmEnable_E,
    PRV_CPSS_LOG_FUNC_cpssTmCtlWriteRegister_E,
    PRV_CPSS_LOG_FUNC_cpssTmCtlReadRegister_E,
    PRV_CPSS_LOG_FUNC_cpssTmInit_E,
    PRV_CPSS_LOG_FUNC_cpssTmInitExt_E,
    PRV_CPSS_LOG_FUNC_cpssTmClose_E,
    PRV_CPSS_LOG_FUNC_cpssTmTreeParamsGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmUnitsErrorStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmBapUnitErrorStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropWredCurveCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropWredTraditionalCurveCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropProfileCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropProfileUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropProfileDelete_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropProfileRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropColorNumSet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropColorNumResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmAgingChangeStatus_E,
    PRV_CPSS_LOG_FUNC_cpssTmAgingStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDpSourceSet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDpSourceGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropQueryResponceSet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropQueryResponceGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropQueueCosSet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropQueueCosGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropProfileAgingBlockCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropProfileAgingBlockDelete_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropProfileAgingUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodeDropProfileIndexRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmEligPrioFuncQueueConfig_E,
    PRV_CPSS_LOG_FUNC_cpssTmEligPrioFuncNodeConfig_E,
    PRV_CPSS_LOG_FUNC_cpssTmEligPrioFuncConfigAllLevels_E,
    PRV_CPSS_LOG_FUNC_cpssTmNamedPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmNamedAsymPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmNamedQueueToAnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmNamedAnodeToBnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmNamedBnodeToCnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmNamedCnodeToPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmGetLogicalNodeIndex_E,
    PRV_CPSS_LOG_FUNC_cpssTmGetNodeLogicalName_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmAsymPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortDropPerCosConfig_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueToPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmTransQueueToPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueToCnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueToBnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueToAnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueToAnodeByIndexCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmAnodeToPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmAnodeToCnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmAnodeToBnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmAnodeToBnodeWithQueuePoolCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmBnodeToPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmBnodeToCnodeCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmCnodeToPortCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodesCtlQueueInstall_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodesCtlQueueUninstall_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodesCtlQueueInstallStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodesCtlNodeDelete_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodesCtlTransPortDelete_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueFlush_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueFlushTriggerActionStart_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueFlushTriggerActionStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortFlush_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortFlushTriggerActionStart_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortFlushTriggerActionStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodesCtlReadNextChange_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodesCtlCleanList_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueConfigurationRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmAnodeConfigurationRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmBnodeConfigurationRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmCnodeConfigurationRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortConfigurationRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodesSwitch_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueStatusRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmAnodeStatusRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmBnodeStatusRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmCnodeStatusRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortStatusRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmDropQueueLengthGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmTreeChangeStatus_E,
    PRV_CPSS_LOG_FUNC_cpssTmTreeStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmTreeDwrrPrioSet_E,
    PRV_CPSS_LOG_FUNC_cpssTmTreeDwrrPrioGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmQueueUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmAnodeUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmBnodeUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmCnodeUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortShapingUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortSchedulingUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortDropUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortDropCosUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmSchedPortExternalBpSet_E,
    PRV_CPSS_LOG_FUNC_cpssTmSchedPeriodicSchemeConfig_E,
    PRV_CPSS_LOG_FUNC_cpssTmShapingPeriodicUpdateEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssTmShapingPeriodicUpdateStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmSchedPortLvlDwrrBytesPerBurstLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortQuantumLimitsGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmNodeQuantumLimitsGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmToCpssErrCodesInit_E,
    PRV_CPSS_LOG_FUNC_cpssTmShapingProfileCreate_E,
    PRV_CPSS_LOG_FUNC_cpssTmShapingProfileDelete_E,
    PRV_CPSS_LOG_FUNC_cpssTmShapingProfileRead_E,
    PRV_CPSS_LOG_FUNC_cpssTmShapingProfileUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssTmShapingProfileValidate_E,
    PRV_CPSS_LOG_FUNC_cpssTmPortShapingValidate_E,
    PRV_CPSS_LOG_FUNC_cpssTmQmrPktStatisticsGet_E,
    PRV_CPSS_LOG_FUNC_cpssTmRcbPktStatisticsGet_E,
    PRV_CPSS_LOG_FUNC_prvCpssTmDumpPort_E,
    PRV_CPSS_LOG_FUNC_prvCpssTmCtlDumpPortHW_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssGenTmLogh */
