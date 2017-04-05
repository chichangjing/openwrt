/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCpssHwInitLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChCpssHwInitLogh
#define __prvCpssDxChCpssHwInitLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH_IMPLEMENT_WA_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_IMPLEMENT_WA_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_LED_CPU_OR_PORT27_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_LED_INDICATION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_LED_PORT_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_LED_CLASS_CONF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_LED_GROUP_CONF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PP_PHASE1_INIT_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_PP_PHASE2_INIT_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_IMPLEMENT_WA_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LED_CLASS_CONF_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LED_CPU_OR_PORT27_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LED_CPU_OR_PORT27_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LED_GROUP_CONF_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LED_INDICATION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LED_INDICATION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LED_PORT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PP_PHASE1_INIT_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_PP_PHASE2_INIT_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_configDevDataBufferSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_void_PTR_configDevDataBufferPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR_windowConfigPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IMPLEMENT_WA_ENT_PTR_waArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IMPLEMENT_WA_ENT_wa;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_CLASS_CONF_STC_PTR_ledClassConfPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_CPU_OR_PORT27_ENT_indicatedPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_GROUP_CONF_STC_PTR_ledGroupConfPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_INDICATION_ENT_indication;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LED_PORT_TYPE_ENT_portType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PP_PHASE1_INIT_INFO_STC_PTR_ppPhase1ParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_PP_PHASE2_INIT_INFO_STC_PTR_ppPhase2ParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_HW_PP_RESET_SKIP_TYPE_ENT_skipType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_LED_CLASS_MANIPULATION_STC_PTR_classParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_LED_CONF_STC_PTR_ledConfPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_LED_GROUP_CONF_STC_PTR_groupParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PP_DEVICE_TYPE_devType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_REG_VALUE_INFO_STC_PTR_initDataListPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_RESOURCE_MAPPING_STC_PTR_resourceMappingPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_invertEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_linkChangeOverride;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_loadFromEeprom;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_skipEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_additionalInfoBmpArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_classNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_configDevDataBufferSize;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_groupNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_initDataListLen;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ledClassNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ledControl;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ledGroupNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ledInterfaceNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfWa;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_period;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_position;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_windowNumber;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_void_PTR_configDevDataBufferPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC_PTR_windowConfigPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LED_CPU_OR_PORT27_ENT_PTR_indicatedPortPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LED_INDICATION_ENT_PTR_indicationPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_HW_PP_INIT_STAGE_ENT_PTR_initStagePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_LED_CLASS_MANIPULATION_STC_PTR_classParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_LED_CONF_STC_PTR_ledConfPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_LED_GROUP_CONF_STC_PTR_groupParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PP_DEVICE_TYPE_PTR_deviceTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_inSyncTrigPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_invertEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isImplemented;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_linkChangeOverridePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_skipEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_hwDevNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_byteAlignmentPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_coreClkDbPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_coreClkHwPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_descSizePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_periodPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_positionPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpPhase1Init_E = (CPSS_LOG_LIB_HW_INIT_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpPhase2Init_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpStartInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpImplementWaInit_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPpHwImplementWaGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwAuDescSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwRxDescSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwTxDescSizeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwRxBufAlignmentGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpSoftResetTrigger_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpSoftResetSkipParamSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpSoftResetSkipParamGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpInitStageGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgHwDevNumSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCfgHwDevNumGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwCoreClockGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwInterruptCoalescingSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwInterruptCoalescingGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwInitLion2GeBackwardCompatibility_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpAddressDecodeWindowConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChHwPpAddressDecodeWindowConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamClassAndGroupConfig_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamHyperGStackTxQStatusEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamHyperGStackTxQStatusEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamCpuOrPort27ModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamCpuOrPort27ModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamClassManipulationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamClassManipulationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamGroupConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamGroupConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamClassIndicationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamClassIndicationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamDirectModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamDirectModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortGroupConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortGroupConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortGroupClassManipulationSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortGroupClassManipulationGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortGroupGroupConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortGroupGroupConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamSyncLedsEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamSyncLedsEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortFlexTriSpeedIndicationEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortFlexTriSpeedIndicationEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortPositionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortPositionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortClassPolarityInvertEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLedStreamPortClassPolarityInvertEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssHwInitResourceMappingSet_E,
    PRV_CPSS_LOG_FUNC_cpssPpInterruptsDisable_E,
    PRV_CPSS_LOG_FUNC_cpssPpConfigDevDataImport_E,
    PRV_CPSS_LOG_FUNC_cpssPpConfigDevDataExport_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChCpssHwInitLogh */
