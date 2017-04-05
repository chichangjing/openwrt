/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChMirrorLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChMirrorLogh
#define __prvCpssDxChMirrorLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_STC_COUNT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_STC_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_STC_COUNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_STC_COUNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_STC_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_analyzerDp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR_interfacePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR_analyzerVlanTagConfigPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_STC_COUNT_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_STC_TYPE_ENT_stcType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_RX_CPU_CODE_ENT_dropCode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_analyzerHwDev;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_analyzerPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_mirrPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cntr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ratio;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_analyzerTc;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_analyzerDpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR_interfacePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR_analyzerVlanTagConfigPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_STC_COUNT_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_dropCodePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_analyzerHwDevPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_analyzerPortPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ratioPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_analyzerTcPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorAnalyzerVlanTagEnable_E = (CPSS_LOG_LIB_MIRROR_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorAnalyzerVlanTagEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxAnalyzerVlanTagConfig_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxAnalyzerVlanTagConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorRxAnalyzerVlanTagConfig_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorRxAnalyzerVlanTagConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxAnalyzerPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxAnalyzerPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorRxAnalyzerPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorRxAnalyzerPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxCascadeMonitorEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxCascadeMonitorEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorRxPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorRxPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxPortSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxPortGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrRxStatMirroringToAnalyzerRatioSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrRxStatMirroringToAnalyzerRatioGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrRxStatMirrorToAnalyzerEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrRxStatMirrorToAnalyzerEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrRxAnalyzerDpTcSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrRxAnalyzerDpTcGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrTxAnalyzerDpTcSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrTxAnalyzerDpTcGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrTxStatMirroringToAnalyzerRatioSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrTxStatMirroringToAnalyzerRatioGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrTxStatMirrorToAnalyzerEnable_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrTxStatMirrorToAnalyzerEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorToAnalyzerForwardingModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorToAnalyzerForwardingModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorAnalyzerInterfaceSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorAnalyzerInterfaceGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorAnalyzerMirrorOnDropEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorAnalyzerMirrorOnDropEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorEnhancedMirroringPriorityModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorEnhancedMirroringPriorityModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxPortVlanEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChMirrorTxPortVlanEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcIngressCountModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcIngressCountModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcReloadModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcReloadModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcPortLimitSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcPortLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcPortReadyForNewLimitGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcPortCountdownCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcPortSampledPacketsCntrSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcPortSampledPacketsCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcEgressAnalyzerIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChStcEgressAnalyzerIndexGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChMirrorLogh */
