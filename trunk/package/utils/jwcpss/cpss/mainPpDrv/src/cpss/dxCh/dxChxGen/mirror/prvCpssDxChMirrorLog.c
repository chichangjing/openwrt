/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChMirrorLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChStc.h>
#include <cpss/dxCh/dxChxGen/mirror/private/prvCpssDxChMirrorLog.h>
#include <cpss/generic/cos/private/prvCpssGenCosLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT[]  =
{
    "CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_TDM_E",
    "CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_SP_INGRESS_EGRESS_MIRROR_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT[]  =
{
    "CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E",
    "CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E",
    "CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_END_TO_END_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_STC_COUNT_MODE_ENT[]  =
{
    "CPSS_DXCH_STC_COUNT_ALL_PACKETS_E",
    "CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_STC_COUNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT[]  =
{
    "CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E",
    "CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_STC_TYPE_ENT[]  =
{
    "CPSS_DXCH_STC_INGRESS_E",
    "CPSS_DXCH_STC_EGRESS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_STC_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, interface, CPSS_INTERFACE_INFO_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, etherType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vpt);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cfi);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_STC_COUNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_STC_COUNT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_STC_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_STC_COUNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_STC_COUNT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_STC_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_STC_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_STC_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_STC_TYPE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_analyzerDp = {
     "analyzerDp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR_interfacePtr = {
     "interfacePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR_analyzerVlanTagConfigPtr = {
     "analyzerVlanTagConfigPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_STC_COUNT_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_STC_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_STC_TYPE_ENT_stcType = {
     "stcType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_STC_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_RX_CPU_CODE_ENT_dropCode = {
     "dropCode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NET_RX_CPU_CODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_analyzerHwDev = {
     "analyzerHwDev", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_analyzerPort = {
     "analyzerPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_mirrPort = {
     "mirrPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cntr = {
     "cntr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ratio = {
     "ratio", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_analyzerTc = {
     "analyzerTc", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_analyzerDpPtr = {
     "analyzerDpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DP_LEVEL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR_interfacePtr = {
     "interfacePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR_analyzerVlanTagConfigPtr = {
     "analyzerVlanTagConfigPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_STC_COUNT_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_STC_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_dropCodePtr = {
     "dropCodePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_NET_RX_CPU_CODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_analyzerHwDevPtr = {
     "analyzerHwDevPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_analyzerPortPtr = {
     "analyzerPortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ratioPtr = {
     "ratioPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_analyzerTcPtr = {
     "analyzerTcPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrRxAnalyzerDpTcSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DP_LEVEL_ENT_analyzerDp,
    &IN_GT_U8_analyzerTc
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorTxAnalyzerVlanTagConfig_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR_analyzerVlanTagConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorEnhancedMirroringPriorityModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorToAnalyzerForwardingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcIngressCountModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_STC_COUNT_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcReloadModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &IN_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcReloadModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &OUT_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_dropCode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcPortSampledPacketsCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_port,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &IN_GT_U32_cntr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcPortLimitSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_port,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &IN_GT_U32_limit
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcPortReadyForNewLimitGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_port,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &OUT_GT_BOOL_PTR_isReadyPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcPortCountdownCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_port,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &OUT_GT_U32_PTR_cntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcPortLimitGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_port,
    &IN_CPSS_DXCH_STC_TYPE_ENT_stcType,
    &OUT_GT_U32_PTR_limitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorTxAnalyzerPortSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_analyzerPort,
    &IN_GT_HW_DEV_NUM_analyzerHwDev
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorRxPortSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_mirrPort,
    &IN_GT_BOOL_isPhysicalPort,
    &IN_GT_BOOL_enable,
    &IN_GT_U32_index
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorRxPortGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_mirrPort,
    &IN_GT_BOOL_isPhysicalPort,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_GT_U32_PTR_indexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorAnalyzerInterfaceSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR_interfacePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorAnalyzerInterfaceGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC_PTR_interfacePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrRxStatMirroringToAnalyzerRatioSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ratio
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrRxAnalyzerDpTcGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DP_LEVEL_ENT_PTR_analyzerDpPtr,
    &OUT_GT_U8_PTR_analyzerTcPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorTxAnalyzerVlanTagConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC_PTR_analyzerVlanTagConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorEnhancedMirroringPriorityModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorToAnalyzerForwardingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChStcIngressCountModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_STC_COUNT_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_dropCodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrorTxAnalyzerPortGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_PORT_NUM_PTR_analyzerPortPtr,
    &OUT_GT_HW_DEV_NUM_PTR_analyzerHwDevPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMirrRxStatMirroringToAnalyzerRatioGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_ratioPtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChMirrorLogLibDb[] = {
    {"cpssDxChMirrorAnalyzerVlanTagEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChMirrorAnalyzerVlanTagEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChMirrorTxAnalyzerVlanTagConfig", 2, cpssDxChMirrorTxAnalyzerVlanTagConfig_PARAMS, NULL},
    {"cpssDxChMirrorTxAnalyzerVlanTagConfigGet", 2, cpssDxChMirrorTxAnalyzerVlanTagConfigGet_PARAMS, NULL},
    {"cpssDxChMirrorRxAnalyzerVlanTagConfig", 2, cpssDxChMirrorTxAnalyzerVlanTagConfig_PARAMS, NULL},
    {"cpssDxChMirrorRxAnalyzerVlanTagConfigGet", 2, cpssDxChMirrorTxAnalyzerVlanTagConfigGet_PARAMS, NULL},
    {"cpssDxChMirrorTxAnalyzerPortSet", 3, cpssDxChMirrorTxAnalyzerPortSet_PARAMS, NULL},
    {"cpssDxChMirrorTxAnalyzerPortGet", 3, cpssDxChMirrorTxAnalyzerPortGet_PARAMS, NULL},
    {"cpssDxChMirrorRxAnalyzerPortSet", 3, cpssDxChMirrorTxAnalyzerPortSet_PARAMS, NULL},
    {"cpssDxChMirrorRxAnalyzerPortGet", 3, cpssDxChMirrorTxAnalyzerPortGet_PARAMS, NULL},
    {"cpssDxChMirrorTxCascadeMonitorEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChMirrorTxCascadeMonitorEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChMirrorRxPortSet", 5, cpssDxChMirrorRxPortSet_PARAMS, NULL},
    {"cpssDxChMirrorRxPortGet", 5, cpssDxChMirrorRxPortGet_PARAMS, NULL},
    {"cpssDxChMirrorTxPortSet", 5, cpssDxChMirrorRxPortSet_PARAMS, NULL},
    {"cpssDxChMirrorTxPortGet", 5, cpssDxChMirrorRxPortGet_PARAMS, NULL},
    {"cpssDxChMirrRxStatMirroringToAnalyzerRatioSet", 2, cpssDxChMirrRxStatMirroringToAnalyzerRatioSet_PARAMS, NULL},
    {"cpssDxChMirrRxStatMirroringToAnalyzerRatioGet", 2, cpssDxChMirrRxStatMirroringToAnalyzerRatioGet_PARAMS, NULL},
    {"cpssDxChMirrRxStatMirrorToAnalyzerEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChMirrRxStatMirrorToAnalyzerEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChMirrRxAnalyzerDpTcSet", 3, cpssDxChMirrRxAnalyzerDpTcSet_PARAMS, NULL},
    {"cpssDxChMirrRxAnalyzerDpTcGet", 3, cpssDxChMirrRxAnalyzerDpTcGet_PARAMS, NULL},
    {"cpssDxChMirrTxAnalyzerDpTcSet", 3, cpssDxChMirrRxAnalyzerDpTcSet_PARAMS, NULL},
    {"cpssDxChMirrTxAnalyzerDpTcGet", 3, cpssDxChMirrRxAnalyzerDpTcGet_PARAMS, NULL},
    {"cpssDxChMirrTxStatMirroringToAnalyzerRatioSet", 2, cpssDxChMirrRxStatMirroringToAnalyzerRatioSet_PARAMS, NULL},
    {"cpssDxChMirrTxStatMirroringToAnalyzerRatioGet", 2, cpssDxChMirrRxStatMirroringToAnalyzerRatioGet_PARAMS, NULL},
    {"cpssDxChMirrTxStatMirrorToAnalyzerEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChMirrTxStatMirrorToAnalyzerEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChMirrorToAnalyzerForwardingModeSet", 2, cpssDxChMirrorToAnalyzerForwardingModeSet_PARAMS, NULL},
    {"cpssDxChMirrorToAnalyzerForwardingModeGet", 2, cpssDxChMirrorToAnalyzerForwardingModeGet_PARAMS, NULL},
    {"cpssDxChMirrorAnalyzerInterfaceSet", 3, cpssDxChMirrorAnalyzerInterfaceSet_PARAMS, NULL},
    {"cpssDxChMirrorAnalyzerInterfaceGet", 3, cpssDxChMirrorAnalyzerInterfaceGet_PARAMS, NULL},
    {"cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet", 3, prvCpssLogGenDevNumEnableIndex_PARAMS, NULL},
    {"cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet", 3, prvCpssLogGenDevNumEnablePtrIndexPtr_PARAMS, NULL},
    {"cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet", 3, prvCpssLogGenDevNumEnableIndex_PARAMS, NULL},
    {"cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet", 3, prvCpssLogGenDevNumEnablePtrIndexPtr_PARAMS, NULL},
    {"cpssDxChMirrorAnalyzerMirrorOnDropEnableSet", 3, prvCpssLogGenDevNumIndexEnable_PARAMS, NULL},
    {"cpssDxChMirrorAnalyzerMirrorOnDropEnableGet", 3, prvCpssLogGenDevNumIndexEnablePtr_PARAMS, NULL},
    {"cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChMirrorEnhancedMirroringPriorityModeSet", 2, cpssDxChMirrorEnhancedMirroringPriorityModeSet_PARAMS, NULL},
    {"cpssDxChMirrorEnhancedMirroringPriorityModeGet", 2, cpssDxChMirrorEnhancedMirroringPriorityModeGet_PARAMS, NULL},
    {"cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet", 2, cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet_PARAMS, NULL},
    {"cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet", 2, cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet_PARAMS, NULL},
    {"cpssDxChMirrorTxPortVlanEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChMirrorTxPortVlanEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChStcIngressCountModeSet", 2, cpssDxChStcIngressCountModeSet_PARAMS, NULL},
    {"cpssDxChStcIngressCountModeGet", 2, cpssDxChStcIngressCountModeGet_PARAMS, NULL},
    {"cpssDxChStcReloadModeSet", 3, cpssDxChStcReloadModeSet_PARAMS, NULL},
    {"cpssDxChStcReloadModeGet", 3, cpssDxChStcReloadModeGet_PARAMS, NULL},
    {"cpssDxChStcEnableSet", 3, cpssDxChStcEnableSet_PARAMS, NULL},
    {"cpssDxChStcEnableGet", 3, cpssDxChStcEnableGet_PARAMS, NULL},
    {"cpssDxChStcPortLimitSet", 4, cpssDxChStcPortLimitSet_PARAMS, NULL},
    {"cpssDxChStcPortLimitGet", 4, cpssDxChStcPortLimitGet_PARAMS, NULL},
    {"cpssDxChStcPortReadyForNewLimitGet", 4, cpssDxChStcPortReadyForNewLimitGet_PARAMS, NULL},
    {"cpssDxChStcPortCountdownCntrGet", 4, cpssDxChStcPortCountdownCntrGet_PARAMS, NULL},
    {"cpssDxChStcPortSampledPacketsCntrSet", 4, cpssDxChStcPortSampledPacketsCntrSet_PARAMS, NULL},
    {"cpssDxChStcPortSampledPacketsCntrGet", 4, cpssDxChStcPortCountdownCntrGet_PARAMS, NULL},
    {"cpssDxChStcEgressAnalyzerIndexSet", 3, prvCpssLogGenDevNumEnableIndex_PARAMS, NULL},
    {"cpssDxChStcEgressAnalyzerIndexGet", 3, prvCpssLogGenDevNumEnablePtrIndexPtr_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_MIRROR(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChMirrorLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChMirrorLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

