/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChIpLpmEngineLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/ip/private/prvCpssDxChIpLog.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/prvCpssDxChIpLpmEngineLog.h>
#include <cpss/dxCh/dxChxGen/lpm/private/prvCpssDxChLpmLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT[]  =
{
    "CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E",
    "CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E",
    "CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E",
    "CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E",
    "CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E",
    "CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E",
    "CPSS_DXCH_IP_LPM_SHADOW_TYPE_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vrId);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, ipAddr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, prefixLen);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, nextHopInfo, CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, override);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, returnStatus);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vrId);
    PRV_CPSS_LOG_STC_IPV6_MAC(valPtr, ipAddr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, prefixLen);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, nextHopInfo, CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, override);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, returnStatus);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, tcamDbCfg, CPSS_DXCH_IP_LPM_TCAM_CONFIG_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, ramDbCfg, CPSS_DXCH_LPM_RAM_CONFIG_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_LPM_VR_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_LPM_VR_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, supportIpv4Uc);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, defIpv4UcNextHopInfo, CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, supportIpv4Mc);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, defIpv4McRouteLttEntry, CPSS_DXCH_IP_LTT_ENTRY_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, supportIpv6Uc);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, defIpv6UcNextHopInfo, CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, supportIpv6Mc);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, defIpv6McRouteLttEntry, CPSS_DXCH_IP_LTT_ENTRY_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfIpv4Prefixes);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfIpv4McSourcePrefixes);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfIpv6Prefixes);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, prefixesCfg, CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, tcamRange, CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, firstIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, lastIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_LPM_TCAM_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_LPM_TCAM_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_PTR_MAC(valPtr, indexesRangePtr, CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, partitionEnable);
    PRV_CPSS_LOG_STC_STC_PTR_MAC(valPtr, tcamLpmManagerCapcityCfgPtr, CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, tcamManagerHandlerPtr);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfIpv4Prefixes);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfIpv6Prefixes);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_LPM_VR_CONFIG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_LPM_VR_CONFIG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_LPM_VR_CONFIG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_PTR(contextLib, logType, namePtr, &paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_IPADDR_PTR_ipAddrPtr = {
     "ipAddrPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_IPADDR_PTR_ipGroupPtr = {
     "ipGroupPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_IPADDR_PTR_ipSrcPtr = {
     "ipSrcPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_IPV6ADDR_PTR_ipAddrPtr = {
     "ipAddrPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_IPV6ADDR_PTR_ipGroupPtr = {
     "ipGroupPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_IPV6ADDR_PTR_ipSrcPtr = {
     "ipSrcPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_defaultIpv4RuleIndexArrayLenPtr = {
     "defaultIpv4RuleIndexArrayLenPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_defaultIpv6RuleIndexArrayLenPtr = {
     "defaultIpv6RuleIndexArrayLenPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_ipGroupPrefixLenPtr = {
     "ipGroupPrefixLenPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_ipSrcPrefixLenPtr = {
     "ipSrcPrefixLenPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_lpmDbMemBlockSizePtr = {
     "lpmDbMemBlockSizePtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfDevsPtr = {
     "numOfDevsPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_pclIdArrayLenPtr = {
     "pclIdArrayLenPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_prefixLenPtr = {
     "prefixLenPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_UINTPTR_PTR_iterPtr = {
     "iterPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_UINTPTR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC_PTR_ipv4PrefixArrayPtr = {
     "ipv4PrefixArrayPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC_PTR_ipv6PrefixArrayPtr = {
     "ipv6PrefixArrayPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT_PTR_memoryCfgPtr = {
     "memoryCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT_shadowType = {
     "shadowType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_LPM_VR_CONFIG_STC_PTR_vrConfigPtr = {
     "vrConfigPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LPM_VR_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_mcRouteLttEntryPtr = {
     "mcRouteLttEntryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LTT_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_PTR_tcamLpmManagerCapcityCfgPtr = {
     "tcamLpmManagerCapcityCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_allocateReserveLinesAs = {
     "allocateReserveLinesAs", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC_PTR_pclTcamCfgPtr = {
     "pclTcamCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC_PTR_routerTcamCfgPtr = {
     "routerTcamCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC_PTR_indexesRangePtr = {
     "indexesRangePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr = {
     "nextHopInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_tcamDefragmentationEnable = {
     "tcamDefragmentationEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPADDR_ipAddr = {
     "ipAddr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_IPADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPADDR_ipGroup = {
     "ipGroup", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_IPADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPADDR_ipSrc = {
     "ipSrc", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_IPADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPV6ADDR_ipAddr = {
     "ipAddr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPV6ADDR_ipGroup = {
     "ipGroup", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPV6ADDR_ipSrc = {
     "ipSrc", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_linesToReserveArray = {
     "linesToReserveArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_lpmDbMemBlockPtr = {
     "lpmDbMemBlockPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_pclIdArray = {
     "pclIdArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ipGroupPrefixLen = {
     "ipGroupPrefixLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ipSrcPrefixLen = {
     "ipSrcPrefixLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ipv4PrefixArrayLen = {
     "ipv4PrefixArrayLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ipv6PrefixArrayLen = {
     "ipv6PrefixArrayLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lpmDBId = {
     "lpmDBId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lpmDbId = {
     "lpmDbId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numberOfLinesToReserve = {
     "numberOfLinesToReserve", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pclIdArrayLen = {
     "pclIdArrayLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vrId = {
     "vrId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT_PTR_memoryCfgPtr = {
     "memoryCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT_PTR_shadowTypePtr = {
     "shadowTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_LPM_VR_CONFIG_STC_PTR_vrConfigPtr = {
     "vrConfigPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LPM_VR_CONFIG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_mcRouteLttEntryPtr = {
     "mcRouteLttEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LTT_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_PTR_tcamLpmManagerCapcityCfgPtr = {
     "tcamLpmManagerCapcityCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC_PTR_indexesRangePtr = {
     "indexesRangePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr = {
     "nextHopInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_IP_PROTOCOL_STACK_ENT_PTR_protocolStackPtr = {
     "protocolStackPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_IP_PROTOCOL_STACK_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_defaultMcUsedPtr = {
     "defaultMcUsedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_partitionEnablePtr = {
     "partitionEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_defaultIpv4RuleIndexArray = {
     "defaultIpv4RuleIndexArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_defaultIpv6RuleIndexArray = {
     "defaultIpv6RuleIndexArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_lpmDbMemBlockPtr = {
     "lpmDbMemBlockPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_lpmDbSizePtr = {
     "lpmDbSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pclIdArray = {
     "pclIdArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamColumnIndexPtr = {
     "tcamColumnIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamGroupColumnIndexPtr = {
     "tcamGroupColumnIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamGroupRowIndexPtr = {
     "tcamGroupRowIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamRowIndexPtr = {
     "tcamRowIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamSrcColumnIndexPtr = {
     "tcamSrcColumnIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tcamSrcRowIndexPtr = {
     "tcamSrcRowIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_devListArray = {
     "devListArray", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBCreate_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT_shadowType,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT_PTR_memoryCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBCapacityUpdate_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC_PTR_indexesRangePtr,
    &IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_PTR_tcamLpmManagerCapcityCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBCheetah2VrSupportCreate_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC_PTR_pclTcamCfgPtr,
    &IN_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC_PTR_routerTcamCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBImport_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_PTR_lpmDbMemBlockPtr,
    &INOUT_GT_U32_PTR_lpmDbMemBlockSizePtr,
    &INOUT_GT_UINTPTR_PTR_iterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4UcPrefixAddBulk_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_ipv4PrefixArrayLen,
    &IN_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC_PTR_ipv4PrefixArrayPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6UcPrefixDelBulk_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_ipv6PrefixArrayLen,
    &IN_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC_PTR_ipv6PrefixArrayPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6UcPrefixAddBulk_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_ipv6PrefixArrayLen,
    &IN_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC_PTR_ipv6PrefixArrayPtr,
    &IN_GT_BOOL_tcamDefragmentationEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmVirtualRouterDel_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmVirtualRouterAdd_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_CPSS_DXCH_IP_LPM_VR_CONFIG_STC_PTR_vrConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4UcPrefixDel_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPADDR_ipAddr,
    &IN_GT_U32_prefixLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4UcPrefixAdd_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPADDR_ipAddr,
    &IN_GT_U32_prefixLen,
    &IN_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr,
    &IN_GT_BOOL_override
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4UcPrefixSearch_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPADDR_ipAddr,
    &IN_GT_U32_prefixLen,
    &OUT_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr,
    &OUT_GT_U32_PTR_tcamRowIndexPtr,
    &OUT_GT_U32_PTR_tcamColumnIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4UcPrefixGet_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPADDR_ipAddr,
    &OUT_GT_U32_PTR_prefixLenPtr,
    &OUT_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr,
    &OUT_GT_U32_PTR_tcamRowIndexPtr,
    &OUT_GT_U32_PTR_tcamColumnIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4McEntryDel_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPADDR_ipGroup,
    &IN_GT_U32_ipGroupPrefixLen,
    &IN_GT_IPADDR_ipSrc,
    &IN_GT_U32_ipSrcPrefixLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4McEntryAdd_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPADDR_ipGroup,
    &IN_GT_U32_ipGroupPrefixLen,
    &IN_GT_IPADDR_ipSrc,
    &IN_GT_U32_ipSrcPrefixLen,
    &IN_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_mcRouteLttEntryPtr,
    &IN_GT_BOOL_override,
    &IN_GT_BOOL_tcamDefragmentationEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4McEntrySearch_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPADDR_ipGroup,
    &IN_GT_U32_ipGroupPrefixLen,
    &IN_GT_IPADDR_ipSrc,
    &IN_GT_U32_ipSrcPrefixLen,
    &OUT_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_mcRouteLttEntryPtr,
    &OUT_GT_U32_PTR_tcamGroupRowIndexPtr,
    &OUT_GT_U32_PTR_tcamGroupColumnIndexPtr,
    &OUT_GT_U32_PTR_tcamSrcRowIndexPtr,
    &OUT_GT_U32_PTR_tcamSrcColumnIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6UcPrefixDel_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPV6ADDR_ipAddr,
    &IN_GT_U32_prefixLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6UcPrefixAdd_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPV6ADDR_ipAddr,
    &IN_GT_U32_prefixLen,
    &IN_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr,
    &IN_GT_BOOL_override,
    &IN_GT_BOOL_tcamDefragmentationEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6UcPrefixSearch_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPV6ADDR_ipAddr,
    &IN_GT_U32_prefixLen,
    &OUT_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr,
    &OUT_GT_U32_PTR_tcamRowIndexPtr,
    &OUT_GT_U32_PTR_tcamColumnIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6UcPrefixGet_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPV6ADDR_ipAddr,
    &OUT_GT_U32_PTR_prefixLenPtr,
    &OUT_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr,
    &OUT_GT_U32_PTR_tcamRowIndexPtr,
    &OUT_GT_U32_PTR_tcamColumnIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6McEntryDel_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPV6ADDR_ipGroup,
    &IN_GT_U32_ipGroupPrefixLen,
    &IN_GT_IPV6ADDR_ipSrc,
    &IN_GT_U32_ipSrcPrefixLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6McEntryAdd_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPV6ADDR_ipGroup,
    &IN_GT_U32_ipGroupPrefixLen,
    &IN_GT_IPV6ADDR_ipSrc,
    &IN_GT_U32_ipSrcPrefixLen,
    &IN_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_mcRouteLttEntryPtr,
    &IN_GT_BOOL_override,
    &IN_GT_BOOL_tcamDefragmentationEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6McEntrySearch_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &IN_GT_IPV6ADDR_ipGroup,
    &IN_GT_U32_ipGroupPrefixLen,
    &IN_GT_IPV6ADDR_ipSrc,
    &IN_GT_U32_ipSrcPrefixLen,
    &OUT_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_mcRouteLttEntryPtr,
    &OUT_GT_U32_PTR_tcamGroupRowIndexPtr,
    &OUT_GT_U32_PTR_tcamSrcRowIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4UcPrefixGetNext_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &INOUT_GT_IPADDR_PTR_ipAddrPtr,
    &INOUT_GT_U32_PTR_prefixLenPtr,
    &OUT_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr,
    &OUT_GT_U32_PTR_tcamRowIndexPtr,
    &OUT_GT_U32_PTR_tcamColumnIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv4McEntryGetNext_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &INOUT_GT_IPADDR_PTR_ipGroupPtr,
    &INOUT_GT_U32_PTR_ipGroupPrefixLenPtr,
    &INOUT_GT_IPADDR_PTR_ipSrcPtr,
    &INOUT_GT_U32_PTR_ipSrcPrefixLenPtr,
    &OUT_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_mcRouteLttEntryPtr,
    &OUT_GT_U32_PTR_tcamGroupRowIndexPtr,
    &OUT_GT_U32_PTR_tcamGroupColumnIndexPtr,
    &OUT_GT_U32_PTR_tcamSrcRowIndexPtr,
    &OUT_GT_U32_PTR_tcamSrcColumnIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6UcPrefixGetNext_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &INOUT_GT_IPV6ADDR_PTR_ipAddrPtr,
    &INOUT_GT_U32_PTR_prefixLenPtr,
    &OUT_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT_PTR_nextHopInfoPtr,
    &OUT_GT_U32_PTR_tcamRowIndexPtr,
    &OUT_GT_U32_PTR_tcamColumnIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmIpv6McEntryGetNext_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &INOUT_GT_IPV6ADDR_PTR_ipGroupPtr,
    &INOUT_GT_U32_PTR_ipGroupPrefixLenPtr,
    &INOUT_GT_IPV6ADDR_PTR_ipSrcPtr,
    &INOUT_GT_U32_PTR_ipSrcPrefixLenPtr,
    &OUT_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_mcRouteLttEntryPtr,
    &OUT_GT_U32_PTR_tcamGroupRowIndexPtr,
    &OUT_GT_U32_PTR_tcamSrcRowIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmVirtualRouterGet_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U32_vrId,
    &OUT_CPSS_DXCH_IP_LPM_VR_CONFIG_STC_PTR_vrConfigPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBDevsListRemove_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &IN_GT_U8_PTR_devListArr,
    &IN_GT_U32_numOfDevs
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBConfigGet_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &OUT_CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT_PTR_shadowTypePtr,
    &OUT_CPSS_IP_PROTOCOL_STACK_ENT_PTR_protocolStackPtr,
    &OUT_CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT_PTR_memoryCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBCapacityGet_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &OUT_GT_BOOL_PTR_partitionEnablePtr,
    &OUT_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC_PTR_indexesRangePtr,
    &OUT_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC_PTR_tcamLpmManagerCapcityCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBExport_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &OUT_GT_U32_PTR_lpmDbMemBlockPtr,
    &INOUT_GT_U32_PTR_lpmDbMemBlockSizePtr,
    &INOUT_GT_UINTPTR_PTR_iterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBMemSizeGet_PARAMS[] =  {
    &IN_GT_U32_lpmDBId,
    &OUT_GT_U32_PTR_lpmDbSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBDelete_PARAMS[] =  {
    &IN_GT_U32_lpmDbId
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBDevListAdd_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmDBDevListGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLpmTcamLinesReserve_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChIpLpmDBCreate_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmDBConfigGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmDBDevListAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmDBDevListAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmDBDevListGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmTcamLinesReserve_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpLpmIpv4UcPrefixAdd_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChIpLpmEngineLogLibDb[] = {
    {"cpssDxChIpLpmDBCreate", 4, cpssDxChIpLpmDBCreate_PARAMS, cpssDxChIpLpmDBCreate_preLogic},
    {"cpssDxChIpLpmDBDelete", 1, cpssDxChIpLpmDBDelete_PARAMS, NULL},
    {"cpssDxChIpLpmDBConfigGet", 4, cpssDxChIpLpmDBConfigGet_PARAMS, cpssDxChIpLpmDBConfigGet_preLogic},
    {"cpssDxChIpLpmDBCapacityUpdate", 3, cpssDxChIpLpmDBCapacityUpdate_PARAMS, NULL},
    {"cpssDxChIpLpmDBCapacityGet", 4, cpssDxChIpLpmDBCapacityGet_PARAMS, NULL},
    {"cpssDxChIpLpmDBCheetah2VrSupportCreate", 4, cpssDxChIpLpmDBCheetah2VrSupportCreate_PARAMS, NULL},
    {"cpssDxChIpLpmDBDevListAdd", 3, cpssDxChIpLpmDBDevListAdd_PARAMS, cpssDxChIpLpmDBDevListAdd_preLogic},
    {"cpssDxChIpLpmDBDevsListRemove", 3, cpssDxChIpLpmDBDevsListRemove_PARAMS, cpssDxChIpLpmDBDevListAdd_preLogic},
    {"cpssDxChIpLpmDBDevListGet", 3, cpssDxChIpLpmDBDevListGet_PARAMS, cpssDxChIpLpmDBDevListGet_preLogic},
    {"cpssDxChIpLpmTcamLinesReserve", 4, cpssDxChIpLpmTcamLinesReserve_PARAMS, cpssDxChIpLpmTcamLinesReserve_preLogic},
    {"cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet", 3, cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet_PARAMS, cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet_preLogic},
    {"cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet", 9, cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet_PARAMS, cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet_preLogic},
    {"cpssDxChIpLpmVirtualRouterAdd", 3, cpssDxChIpLpmVirtualRouterAdd_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmVirtualRouterSharedAdd", 3, cpssDxChIpLpmVirtualRouterAdd_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmVirtualRouterGet", 3, cpssDxChIpLpmVirtualRouterGet_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmVirtualRouterDel", 2, cpssDxChIpLpmVirtualRouterDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv4UcPrefixAdd", 6, cpssDxChIpLpmIpv4UcPrefixAdd_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmIpv4UcPrefixAddBulk", 3, cpssDxChIpLpmIpv4UcPrefixAddBulk_PARAMS, cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic},
    {"cpssDxChIpLpmIpv4UcPrefixDel", 4, cpssDxChIpLpmIpv4UcPrefixDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv4UcPrefixDelBulk", 3, cpssDxChIpLpmIpv4UcPrefixAddBulk_PARAMS, cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic},
    {"cpssDxChIpLpmIpv4UcPrefixesFlush", 2, cpssDxChIpLpmVirtualRouterDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv4UcPrefixSearch", 7, cpssDxChIpLpmIpv4UcPrefixSearch_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmIpv4UcPrefixGetNext", 7, cpssDxChIpLpmIpv4UcPrefixGetNext_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmIpv4UcPrefixGet", 7, cpssDxChIpLpmIpv4UcPrefixGet_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmIpv4McEntryAdd", 9, cpssDxChIpLpmIpv4McEntryAdd_PARAMS, NULL},
    {"cpssDxChIpLpmIpv4McEntryDel", 6, cpssDxChIpLpmIpv4McEntryDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv4McEntriesFlush", 2, cpssDxChIpLpmVirtualRouterDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv4McEntryGetNext", 11, cpssDxChIpLpmIpv4McEntryGetNext_PARAMS, NULL},
    {"cpssDxChIpLpmIpv4McEntrySearch", 11, cpssDxChIpLpmIpv4McEntrySearch_PARAMS, NULL},
    {"cpssDxChIpLpmIpv6UcPrefixAdd", 7, cpssDxChIpLpmIpv6UcPrefixAdd_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmIpv6UcPrefixAddBulk", 4, cpssDxChIpLpmIpv6UcPrefixAddBulk_PARAMS, cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic},
    {"cpssDxChIpLpmIpv6UcPrefixDel", 4, cpssDxChIpLpmIpv6UcPrefixDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv6UcPrefixDelBulk", 3, cpssDxChIpLpmIpv6UcPrefixDelBulk_PARAMS, cpssDxChIpLpmIpv4UcPrefixAddBulk_preLogic},
    {"cpssDxChIpLpmIpv6UcPrefixesFlush", 2, cpssDxChIpLpmVirtualRouterDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv6UcPrefixSearch", 7, cpssDxChIpLpmIpv6UcPrefixSearch_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmIpv6UcPrefixGetNext", 7, cpssDxChIpLpmIpv6UcPrefixGetNext_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmIpv6UcPrefixGet", 7, cpssDxChIpLpmIpv6UcPrefixGet_PARAMS, cpssDxChIpLpmIpv4UcPrefixAdd_preLogic},
    {"cpssDxChIpLpmIpv6McEntryAdd", 9, cpssDxChIpLpmIpv6McEntryAdd_PARAMS, NULL},
    {"cpssDxChIpLpmIpv6McEntryDel", 6, cpssDxChIpLpmIpv6McEntryDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv6McEntriesFlush", 2, cpssDxChIpLpmVirtualRouterDel_PARAMS, NULL},
    {"cpssDxChIpLpmIpv6McEntryGetNext", 9, cpssDxChIpLpmIpv6McEntryGetNext_PARAMS, NULL},
    {"cpssDxChIpLpmIpv6McEntrySearch", 9, cpssDxChIpLpmIpv6McEntrySearch_PARAMS, NULL},
    {"cpssDxChIpLpmDBMemSizeGet", 2, cpssDxChIpLpmDBMemSizeGet_PARAMS, NULL},
    {"cpssDxChIpLpmDBExport", 4, cpssDxChIpLpmDBExport_PARAMS, NULL},
    {"cpssDxChIpLpmDBImport", 4, cpssDxChIpLpmDBImport_PARAMS, NULL},
    {"cpssDxChIpLpmDbgHwOctetPerBlockPrint", 1, cpssDxChIpLpmDBDelete_PARAMS, NULL},
    {"cpssDxChIpLpmDbgHwOctetPerProtocolPrint", 1, cpssDxChIpLpmDBDelete_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_IP_LPM(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChIpLpmEngineLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChIpLpmEngineLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

