/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTmGlueLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChTmGlueLogh
#define __prvCpssDxChTmGlueLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_GLUE_DRAM_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_GLUE_DROP_MASK_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_DRAM_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_DROP_MASK_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_timerResolutionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NET_DSA_CMD_ENT_packetDsaCommand;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC_PTR_thresholdsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC_PTR_dramAlgoPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_DRAM_CFG_STC_PTR_dramCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_DROP_MASK_STC_PTR_dropMaskCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT_responseMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC_PTR_lengthOffsetPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT_mcMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT_toCpuMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_PTR_enablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_bypass;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_egressEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_forceCounting;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_forceMeter;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_ingressEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isUnicast;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_targetHwDevId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_pfcPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_physicalPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_agedPacketCouterQueue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_agedPacketCouterQueueMask;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cNodeValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_externalMemoryInitFlags;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queueId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tmPortInd;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tmTc;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_toCpuSelectorIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_xOffThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_xOnThreshold;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC_PTR_thresholdsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC_PTR_agingCountersPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_GLUE_DROP_MASK_STC_PTR_dropMaskCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT_PTR_responseModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC_PTR_lengthOffsetPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT_PTR_mcModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT_PTR_toCpuModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_bypassPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_egressEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_forceCountingPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_forceMeterPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_ingressEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PHYSICAL_PORT_NUM_PTR_pfcPortNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_agedOutPacketCounterPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_agedPacketCounterPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_agedPacketCouterQueueMaskPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_agedPacketCouterQueuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cNodeValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_entryIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_timerResolutionPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tmTcPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_toCpuSelectorIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_xOffThresholdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_xOnThresholdPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayDropAgedPacketEnableSet_E = (CPSS_LOG_LIB_TM_GLUE_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayDropAgedPacketEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueGeneralPerPortDropOutgoingPacketEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueGeneralPerPortDropOutgoingPacketEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayAgedPacketCounterConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayAgedPacketCounterConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayAgedPacketCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayTimerResolutionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayTimerResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayTimerGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayAgingProfileThresholdsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayAgingProfileThresholdsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayQueueAgingProfileIdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayQueueAgingProfileIdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayCounterQueueIdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayCounterQueueIdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayCountersGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueAgingAndDelayPerPortAgedPacketCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueDramInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueDramInitFlagsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueDropQueueProfileIdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueDropQueueProfileIdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueDropProfileDropMaskSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueDropProfileDropMaskGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueDropTcToCosSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueDropTcToCosGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueFlowControlEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueFlowControlEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueFlowControlEgressCounterSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueFlowControlEgressCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueFlowControlEgressThresholdsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueFlowControlEgressThresholdsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueFlowControlPortSpeedSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueFlowControlPortSpeedGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGluePfcTmTcPort2CNodeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGluePfcTmTcPort2CNodeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGluePfcResponseModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGluePfcResponseModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGluePfcPortMappingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGluePfcPortMappingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapBypassEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapBypassEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapBitSelectTableEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapBitSelectTableEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapCpuCodeToTcMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapCpuCodeToTcMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapTcToTcMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapTcToTcMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapEgressPolicerForceSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapEgressPolicerForceGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapToCpuModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapToCpuModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapMcModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapMcModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapSelectorTableAccessModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapSelectorTableAccessModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChTmGlueLogh */
