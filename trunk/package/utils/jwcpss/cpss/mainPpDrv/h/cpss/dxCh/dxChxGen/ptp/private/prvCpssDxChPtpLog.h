/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPtpLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChPtpLogh
#define __prvCpssDxChPtpLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TAI_NUMBER_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TAI_INSTANCE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TS_ACTION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TS_TAG_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TAI_ID_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_ID_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_NUMBER_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_NUMBER_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIRECTION_ENT_clockInterfaceDirection;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIRECTION_ENT_pulseInterfaceDirection;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR_egrExceptionCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT_checkingMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT_domainMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR_ingrExceptionCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT_clockMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT_clockSelect;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_ID_STC_PTR_taiIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_NUMBER_ENT_taiNumber;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT_pclkRcvrClkMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR_todStepPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT_todValueType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_function;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todCounterPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR_controlPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT_counterType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT_egrTimeCorrTaiSelMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR_tsTagGlobalCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR_tsTagPortCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR_udpCsUpdModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_egrAsymmetryCorr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_egrPipeDelayCorr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_fracNanoSecond;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_ingressDelay;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_ingressDelayCorr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_egrCorrFldPBEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_ingrCorrFldPBEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_resetRxUnit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_resetTxUnit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_domainIdArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_adjustThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_captureIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_delay;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_domainId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_domainIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_domainProfile;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_egrTsTaiNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_etherTypeIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_extPulseWidth;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_gracefulStep;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_idMapBmp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_interruptThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_messageType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_nanoSeconds;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ntpTimeOffset;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queueNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queueSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_seconds;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_transportSpecificVal;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_udpPortIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_udpPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DIRECTION_ENT_PTR_clockInterfaceDirectionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DIRECTION_ENT_PTR_pulseInterfaceDirectionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC_PTR_egrExceptionCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC_PTR_egrExceptionCntPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT_PTR_checkingModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT_PTR_domainModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC_PTR_ingrExceptionCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT_PTR_clockModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT_PTR_clockSelectPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_NUMBER_ENT_PTR_taiNumberPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT_PTR_pclkRcvrClkModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TAI_TOD_STEP_STC_PTR_todStepPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT_PTR_functionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todCounterPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TOD_COUNT_STC_PTR_todValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TSU_CONTROL_STC_PTR_controlPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT_PTR_egrTimeCorrTaiSelModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR_tsQueueEntryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC_PTR_tsQueueEntryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC_PTR_tsTagGlobalCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC_PTR_tsTagPortCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC_PTR_udpCsUpdModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_egrAsymmetryCorrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_egrPipeDelayCorrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_fracNanoSecondPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_ingressDelayCorrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_ingressDelayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_egrCorrFldPBEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_ingrCorrFldPBEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_resetRxUnitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_resetTxUnitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_adjustThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_delayPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_domainIdArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_domainIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_egrTsTaiNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_extPulseWidthPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_gracefulStepPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_idMapBmpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_interruptThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_nanoSecondsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ntpTimeOffsetPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueEntryId0Ptr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueEntryId1Ptr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_secondsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_udpPortNumPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEtherTypeSet_E = (CPSS_LOG_LIB_PTP_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEtherTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpUdpDestPortsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpUdpDestPortsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpMessageTypeCmdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpMessageTypeCmdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpCpuCodeBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpCpuCodeBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTodCounterFunctionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTodCounterFunctionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTodCounterFunctionTriggerSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTodCounterFunctionTriggerGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTodCounterShadowSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTodCounterShadowGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTodCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortTimeStampEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortTimeStampEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEthernetTimeStampEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEthernetTimeStampEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpUdpTimeStampEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpUdpTimeStampEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpMessageTypeTimeStampEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpMessageTypeTimeStampEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTransportSpecificCheckEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTransportSpecificCheckEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTransportSpecificTimeStampEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTransportSpecificTimeStampEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTimestampEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortGroupTodCounterFunctionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortGroupTodCounterFunctionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortGroupTodCounterFunctionTriggerSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortGroupTodCounterFunctionTriggerGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortGroupTodCounterShadowSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortGroupTodCounterShadowGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortGroupTodCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiClockModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiClockModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiInternalClockGenerateEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiInternalClockGenerateEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiCaptureOverrideEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiCaptureOverrideEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiInputTriggersCountEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiInputTriggersCountEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiExternalPulseWidthSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiExternalPulseWidthGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiTodSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiTodGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiOutputTriggerEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiOutputTriggerEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiTodStepSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiTodStepGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPulseDelaySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPulseDelayGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiClockDelaySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiClockDelayGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiFractionalNanosecondDriftSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiFractionalNanosecondDriftGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPClockCycleSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPClockCycleGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiClockCycleSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiClockCycleGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiTodCaptureStatusSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiTodCaptureStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiTodUpdateCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiIncomingTriggerCounterSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiIncomingTriggerCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiIncomingClockCounterSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiIncomingClockCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiFrequencyDriftThesholdsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiFrequencyDriftThesholdsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiGracefulStepSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiGracefulStepGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsuControlSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsuControlGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsuTxTimestampQueueRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsuCountersClear_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsuPacketCouterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsuNtpTimeOffsetSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsuNtpTimeOffsetGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsTagGlobalCfgSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsTagGlobalCfgGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsTagPortCfgSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsTagPortCfgGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayIngressPortDelaySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayIngressPortDelayGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayIngressLinkDelaySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayIngressLinkDelayGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressPipeDelaySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressPipeDelayGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressTimestampTaiSelectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDelayEgressTimestampTaiSelectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsCfgTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsCfgTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsLocalActionTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsLocalActionTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsUdpChecksumUpdateModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsUdpChecksumUpdateModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpOverEthernetEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpOverEthernetEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpOverUdpEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpOverUdpEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpDomainModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpDomainModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpDomainV1IdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpDomainV1IdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpDomainV2IdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpDomainV2IdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEgressDomainTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEgressDomainTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpIngressExceptionCfgSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpIngressExceptionCfgGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpIngressPacketCheckingModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpIngressPacketCheckingModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpIngressExceptionCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEgressExceptionCfgSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEgressExceptionCfgGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpEgressExceptionCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsIngressTimestampQueueEntryRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsEgressTimestampQueueEntryRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsMessageTypeToQueueIdMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsMessageTypeToQueueIdMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsQueuesEntryOverrideEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsQueuesEntryOverrideEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsQueuesSizeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsQueuesSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDebugQueuesEntryIdsClear_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsDebugQueuesEntryIdsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsNtpTimeOffsetSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTsNtpTimeOffsetGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPtpPulseIterfaceSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPtpPulseIterfaceGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPtpClockInterfaceSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPtpClockInterfaceGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPClkOutputInterfaceSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiPClkOutputInterfaceGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiInputClockSelectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiInputClockSelectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpTaiTodCounterFunctionAllTriggerSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortUnitResetSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortUnitResetGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortTxPipeStatusDelaySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPtpPortTxPipeStatusDelayGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChPtpLogh */
