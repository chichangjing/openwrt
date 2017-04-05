/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChNetworkIfLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChNetworkIfLogh
#define __prvCpssDxChNetworkIfLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_FORWARD_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_FROM_CPU_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_TO_ANALYZER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_TO_CPU_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_NET_DSA_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_NET_DSA_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_NETIF_MII_INIT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_RX_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_TX_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_SDMA_TX_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_NETIF_MII_INIT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_DSA_CMD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_RX_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_NET_TX_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfBuffPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NETIF_MII_INIT_STC_PTR_miiInitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR_entryInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR_dsaInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT_rateMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_packetParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_pcktParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_PROT_ENT_protocol;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_TCP_UDP_PACKET_TYPE_ENT_packetType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_burstEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_designatedHwDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_maxDstPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_minDstPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_buffLenArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_buffLenList;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_rxBuffSizeList;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_buffListLen;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_burstPacketsNumber;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dropCntrVal;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfBufs;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_packetDataLength;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_packetId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pktLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rangeIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_rateLimiterIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_statisticalRateLimit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_windowResolution;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_rateValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_PTR_buffList;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_PTR_packetBuffsArrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_PTR_rxBuffList;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_dsaBytesPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_packetDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_queue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_queueIdx;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_rxQueue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_txQueue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_UINTPTR_hndl;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC_PTR_entryInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR_dsaInfoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_RX_PARAMS_STC_PTR_rxParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC_PTR_rxCountersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT_PTR_burstStatusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT_PTR_rateModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_PROT_ENT_PTR_protocolPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC_PTR_rxErrCountPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_TCP_UDP_PACKET_TYPE_ENT_PTR_packetTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_designatedHwDevNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_STATUS_PTR_statusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_maxDstPortPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_minDstPortPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_buffLenArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dropCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numberOfFreeTxDescriptorsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_packetCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_packetIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pktLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_statisticalRateLimitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_windowResolutionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_actualRateValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_rateValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_PTR_packetBuffsArrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_devPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_dsaBytesPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_protocolPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_queuePtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfDsaTagParse_E = (CPSS_LOG_LIB_NETWORK_IF_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfDsaTagBuild_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfDuplicateEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfDuplicateEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortDuplicateToCpuSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortDuplicateToCpuGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfAppSpecificCpuCodeTcpSynSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfAppSpecificCpuCodeTcpSynGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeIpLinkLocalProtSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeIpLinkLocalProtGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeDesignatedDeviceTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeDesignatedDeviceTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeRateLimiterTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeRateLimiterTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeRateLimiterDropCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfCpuCodeRateLimiterDropCntrSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfFromCpuDpSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfFromCpuDpGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaRxResourceErrorModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaRxResourceErrorModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfRemove_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfMiiTxPacketSend_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfMiiSyncTxPacketSend_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfMiiTxBufferQueueGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfMiiRxPacketGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfMiiRxBufFree_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfMiiInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfRxBufFree_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfRxBufFreeWithSize_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaRxPacketGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPrePendTwoBytesHeaderSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfPrePendTwoBytesHeaderGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaRxCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaRxErrorCountGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaRxQueueEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaRxQueueEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxPacketSend_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaSyncTxPacketSend_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxQueueEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxQueueEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfTxBufferQueueGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxGeneratorPacketAdd_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxGeneratorPacketUpdate_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxGeneratorPacketRemove_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxGeneratorRateSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxGeneratorRateGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxGeneratorEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxGeneratorDisable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChNetIfSdmaTxGeneratorBurstStatusGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChNetworkIfLogh */
