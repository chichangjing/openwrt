/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChDiagLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChDiagLogh
#define __prvCpssDxChDiagLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH_DIAG_BIST_RESULT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DIAG_PP_MEM_BIST_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_DIAG_BIST_STATUS_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(CPSS_DIAG_PG_IF_SIZE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DIAG_PG_TRANSMIT_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_DIAG_BIST_RESULT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_DIAG_DESCRIPTOR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DIAG_PP_MEM_BIST_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_BIST_STATUS_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_DESCRIPTOR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_eventsNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_rawDataLengthPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_regsNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_resultsNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT_errorType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT_injectMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT_PTR_locationPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT_bistTestType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIAG_PP_MEM_BIST_TYPE_ENT_memBistType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIAG_PP_MEM_TYPE_ENT_memType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIAG_PP_REG_TYPE_ENT_regType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIAG_TEST_PROFILE_ENT_profile;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT_memType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT_descriptorType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT_pattern;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC_PTR_configPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT_optMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC_PTR_portLaneArrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT_sensorType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT_prbsType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_EVENT_MASK_SET_ENT_operation;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PP_INTERFACE_CHANNEL_ENT_ifChannel;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_32_thresholdValue;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_clearMemoryAfterTest;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_connect;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_countEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_doByteSwap;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_injectEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_testWholeMemory;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_cyclicDataArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_testsToRunBmpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_blockIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_evExtData;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_extMemoBitmap;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_phyAddr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_portLaneArrLength;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_prbsTime;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_regMask;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_smiRegOffset;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_startOffset;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_testedAreaLength;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_testedAreaOffset;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_timeOut;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_UINTPTR_baseAddr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT_PTR_errorTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT_PTR_injectModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT_PTR_protectionTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DIAG_BIST_RESULT_STC_PTR_resultsArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DIAG_BIST_STATUS_ENT_PTR_resultsStatusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC_PTR_eventsArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DIAG_DESCRIPTOR_STC_PTR_descriptorPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC_PTR_errorInfoArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC_PTR_configPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT_PTR_sensorTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_PORT_SERDES_TUNE_STC_PTR_optResultArrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_EVENT_MASK_SET_ENT_PTR_operationPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_32_PTR_thresholdValuePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_blockFixedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_burstTransmitDonePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_checkerLockedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_connectPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_countEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_injectEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isNoMoreEventsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_lockedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_testStatusPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_addrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_badRegPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cyclicDataArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_errorCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_errorCounterPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_failedRowPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_failedSyndromePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rawDataLengthPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_rawDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_readValPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_regAddrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_regDataPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_regsNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_replacedIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_testsResultBmpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_writeValPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_patternCntrPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChDiagMemTest_E = (CPSS_LOG_LIB_DIAG_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChDiagAllMemTest_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagMemWrite_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagMemRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagRegWrite_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagRegRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPhyRegWrite_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPhyRegRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagRegsNumGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagResetAndInitControllerRegsNumGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagRegsDump_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagResetAndInitControllerRegsDump_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagRegTest_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagAllRegTest_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsPortTransmitModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsPortTransmitModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsPortGenerateEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsPortGenerateEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsPortCheckEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsPortCheckEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsPortCheckReadyGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsPortStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsCyclicDataSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsCyclicDataGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsSerdesTestEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsSerdesTestEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsSerdesTransmitModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsSerdesTransmitModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsSerdesCounterClearOnReadEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsSerdesCounterClearOnReadEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPrbsSerdesStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagMemoryBistsRun_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagMemoryBistBlockStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDeviceTemperatureSensorsSelectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDeviceTemperatureSensorsSelectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDeviceTemperatureThresholdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDeviceTemperatureThresholdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDeviceTemperatureGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPortGroupMemWrite_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPortGroupMemRead_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPortGroupRegsNumGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPortGroupRegsDump_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagSerdesTuningSystemInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagSerdesTuningTracePrintEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagSerdesTuningSystemClose_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagSerdesTuningRxTune_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagRegDefaultsEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagFastBootSkipOwnDeviceInitEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagBistTriggerAllSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagBistResultsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagExternalMemoriesBistRun_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityEventsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityEventMaskSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityEventMaskGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityErrorInfoGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityErrorInjectionConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityErrorInjectionConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityErrorCountEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityErrorCountEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityProtectionTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDescriptorRawGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDescriptorPortGroupRawGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDescriptorRawSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDescriptorGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagDescriptorPortGroupGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPacketGeneratorConnectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPacketGeneratorConnectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPacketGeneratorTransmitEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChDiagPacketGeneratorBurstTransmitStatusGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChDiagLogh */
