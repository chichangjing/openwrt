/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCosLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCosLog.h>
#include <cpss/generic/bridge/private/prvCpssGenBrgLog.h>
#include <cpss/generic/cos/private/prvCpssGenCosLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_COS_PROFILE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_COS_PROFILE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dropPrecedence, CPSS_DP_LEVEL_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, userPriority);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, trafficClass);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dscp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_COS_PROFILE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_COS_PROFILE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_COS_PROFILE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_COS_PROFILE_STC_PTR_cosPtr = {
     "cosPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_COS_PROFILE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_QOS_RESOLVE_ENT_macQosResolvType = {
     "macQosResolvType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_MAC_QOS_RESOLVE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_QOS_ENTRY_STC_PTR_macQosCfgPtr = {
     "macQosCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_QOS_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_QOS_ENTRY_STC_PTR_portQosCfgPtr = {
     "portQosCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_QOS_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_QOS_PORT_TRUST_MODE_ENT_portQosTrustMode = {
     "portQosTrustMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_QOS_PORT_TRUST_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_VLAN_TAG_TYPE_ENT_vlanTagType = {
     "vlanTagType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_VLAN_TAG_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enableDscpMutation = {
     "enableDscpMutation", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isDefaultVlanTagType = {
     "isDefaultVlanTagType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_upOverrideEnable = {
     "upOverrideEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_useUpAsIndex = {
     "useUpAsIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dscp = {
     "dscp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_entryIdx = {
     "entryIdx", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mappingTableIndex = {
     "mappingTableIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newDscp = {
     "newDscp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newExp = {
     "newExp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newProfileIndex = {
     "newProfileIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newUp = {
     "newUp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_upProfileIndex = {
     "upProfileIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_cfiDeiBit = {
     "cfiDeiBit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_defaultUserPrio = {
     "defaultUserPrio", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_dscp = {
     "dscp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_newDscp = {
     "newDscp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_COS_PROFILE_STC_PTR_cosPtr = {
     "cosPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_COS_PROFILE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_QOS_RESOLVE_ENT_PTR_macQosResolvTypePtr = {
     "macQosResolvTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_QOS_RESOLVE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_QOS_ENTRY_STC_PTR_macQosCfgPtr = {
     "macQosCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_QOS_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_QOS_ENTRY_STC_PTR_portQosCfgPtr = {
     "portQosCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_QOS_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_QOS_PORT_TRUST_MODE_ENT_PTR_portQosTrustModePtr = {
     "portQosTrustModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_QOS_PORT_TRUST_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_VLAN_TAG_TYPE_ENT_PTR_vlanTagTypePtr = {
     "vlanTagTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_VLAN_TAG_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_enableDscpMutationPtr = {
     "enableDscpMutationPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_upOverrideEnablePtr = {
     "upOverrideEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_useUpAsIndexPtr = {
     "useUpAsIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dscpPtr = {
     "dscpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mappingTableIndexPtr = {
     "mappingTableIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_newDscpPtr = {
     "newDscpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_newExpPtr = {
     "newExpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_newProfileIndexPtr = {
     "newProfileIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_newUpPtr = {
     "newUpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_upProfileIndexPtr = {
     "upProfileIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_cfiDeiBitPtr = {
     "cfiDeiBitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_defaultUserPrioPtr = {
     "defaultUserPrioPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_newDscpPtr = {
     "newDscpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosDpToCfiDeiMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DP_LEVEL_ENT_dp,
    &IN_GT_U8_cfiDeiBit
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosDpToCfiDeiMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DP_LEVEL_ENT_dp,
    &OUT_GT_U8_PTR_cfiDeiBitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosMacQosConflictResolutionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_QOS_RESOLVE_ENT_macQosResolvType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortUpProfileIndexSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U8_up,
    &IN_GT_U32_upProfileIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortUpProfileIndexGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U8_up,
    &OUT_GT_U32_PTR_upProfileIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortVlanQoSCfgEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_port,
    &OUT_GT_U32_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortQosConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_QOS_ENTRY_STC_PTR_portQosCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortQosTrustModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_QOS_PORT_TRUST_MODE_ENT_portQosTrustMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortReMapDSCPSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_enableDscpMutation
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosL2TrustModeVlanTagSelectSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_isDefaultVlanTagType,
    &IN_CPSS_VLAN_TAG_TYPE_ENT_vlanTagType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosL2TrustModeVlanTagSelectGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_isDefaultVlanTagType,
    &OUT_CPSS_VLAN_TAG_TYPE_ENT_PTR_vlanTagTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPortModifyUPSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_upOverrideEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortTrustQosMappingTableIndexSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_useUpAsIndex,
    &IN_GT_U32_mappingTableIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortEgressQosMappingTableIndexSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_mappingTableIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPortDefaultUPSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U8_defaultUserPrio
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortQosConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_QOS_ENTRY_STC_PTR_portQosCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortQosTrustModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_QOS_PORT_TRUST_MODE_ENT_PTR_portQosTrustModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortReMapDSCPGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_enableDscpMutationPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPortModifyUPGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_upOverrideEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortTrustQosMappingTableIndexGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_useUpAsIndexPtr,
    &OUT_GT_U32_PTR_mappingTableIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosPortEgressQosMappingTableIndexGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_mappingTableIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPortDefaultUPGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U8_PTR_defaultUserPrioPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosMacQosEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIdx,
    &IN_CPSS_QOS_ENTRY_STC_PTR_macQosCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosMacQosEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIdx,
    &OUT_CPSS_QOS_ENTRY_STC_PTR_macQosCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosRemarkEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_U32_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosEgressDscp2DscpMappingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_dscp,
    &IN_GT_U32_newDscp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosEgressDscp2DscpMappingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_dscp,
    &OUT_GT_U32_PTR_newDscpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosEgressExp2ExpMappingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_exp,
    &IN_GT_U32_newExp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosExpToProfileMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_exp,
    &IN_GT_U32_profileIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosEgressExp2ExpMappingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_exp,
    &OUT_GT_U32_PTR_newExpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosExpToProfileMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_exp,
    &OUT_GT_U32_PTR_profileIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_tc,
    &IN_CPSS_DP_LEVEL_ENT_dp,
    &IN_GT_U32_up,
    &IN_GT_U32_exp,
    &IN_GT_U32_dscp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_tc,
    &IN_CPSS_DP_LEVEL_ENT_dp,
    &OUT_GT_U32_PTR_upPtr,
    &OUT_GT_U32_PTR_expPtr,
    &OUT_GT_U32_PTR_dscpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosEgressUp2UpMappingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_up,
    &IN_GT_U32_newUp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosEgressUp2UpMappingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_up,
    &OUT_GT_U32_PTR_newUpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosUpCfiDeiToProfileMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_upProfileIndex,
    &IN_GT_U8_up,
    &IN_GT_U8_cfiDeiBit,
    &IN_GT_U32_profileIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosUpCfiDeiToProfileMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U32_upProfileIndex,
    &IN_GT_U8_up,
    &IN_GT_U8_cfiDeiBit,
    &OUT_GT_U32_PTR_profileIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosDscpToProfileMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U8_dscp,
    &IN_GT_U32_profileIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosDscpMutationEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U8_dscp,
    &IN_GT_U8_newDscp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosDscpToProfileMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U8_dscp,
    &OUT_GT_U32_PTR_profileIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosDscpMutationEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mappingTableIndex,
    &IN_GT_U8_dscp,
    &OUT_GT_U8_PTR_newDscpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosProfileEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &IN_CPSS_DXCH_COS_PROFILE_STC_PTR_cosPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosRemarkTblEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &IN_GT_U32_newProfileIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosProfileEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &OUT_CPSS_DXCH_COS_PROFILE_STC_PTR_cosPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosRemarkTblEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &OUT_GT_U32_PTR_newProfileIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCosMacQosConflictResolutionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_MAC_QOS_RESOLVE_ENT_PTR_macQosResolvTypePtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChCosLogLibDb[] = {
    {"cpssDxChCosProfileEntrySet", 3, cpssDxChCosProfileEntrySet_PARAMS, NULL},
    {"cpssDxChCosProfileEntryGet", 3, cpssDxChCosProfileEntryGet_PARAMS, NULL},
    {"cpssDxChCosDscpToProfileMapSet", 4, cpssDxChCosDscpToProfileMapSet_PARAMS, NULL},
    {"cpssDxChCosDscpToProfileMapGet", 4, cpssDxChCosDscpToProfileMapGet_PARAMS, NULL},
    {"cpssDxChCosDscpMutationEntrySet", 4, cpssDxChCosDscpMutationEntrySet_PARAMS, NULL},
    {"cpssDxChCosDscpMutationEntryGet", 4, cpssDxChCosDscpMutationEntryGet_PARAMS, NULL},
    {"cpssDxChCosUpCfiDeiToProfileMapSet", 6, cpssDxChCosUpCfiDeiToProfileMapSet_PARAMS, NULL},
    {"cpssDxChCosUpCfiDeiToProfileMapGet", 6, cpssDxChCosUpCfiDeiToProfileMapGet_PARAMS, NULL},
    {"cpssDxChCosPortDpToCfiDeiMapEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCosPortDpToCfiDeiMapEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCosPortDpToCfiDei1MapEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCosPortDpToCfiDei1MapEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCosDpToCfiDeiMapSet", 3, cpssDxChCosDpToCfiDeiMapSet_PARAMS, NULL},
    {"cpssDxChCosDpToCfiDeiMapGet", 3, cpssDxChCosDpToCfiDeiMapGet_PARAMS, NULL},
    {"cpssDxChCosExpToProfileMapSet", 4, cpssDxChCosExpToProfileMapSet_PARAMS, NULL},
    {"cpssDxChCosExpToProfileMapGet", 4, cpssDxChCosExpToProfileMapGet_PARAMS, NULL},
    {"cpssDxChCosPortQosConfigSet", 3, cpssDxChCosPortQosConfigSet_PARAMS, NULL},
    {"cpssDxChCosPortQosConfigGet", 3, cpssDxChCosPortQosConfigGet_PARAMS, NULL},
    {"cpssDxChCosMacQosEntrySet", 3, cpssDxChCosMacQosEntrySet_PARAMS, NULL},
    {"cpssDxChCosMacQosEntryGet", 3, cpssDxChCosMacQosEntryGet_PARAMS, NULL},
    {"cpssDxChCosMacQosConflictResolutionSet", 2, cpssDxChCosMacQosConflictResolutionSet_PARAMS, NULL},
    {"cpssDxChCosMacQosConflictResolutionGet", 2, cpssDxChCosMacQosConflictResolutionGet_PARAMS, NULL},
    {"cpssDxChCosRemarkTblEntrySet", 3, cpssDxChCosRemarkTblEntrySet_PARAMS, NULL},
    {"cpssDxChCosRemarkTblEntryGet", 3, cpssDxChCosRemarkTblEntryGet_PARAMS, NULL},
    {"cpssDxChPortDefaultUPSet", 3, cpssDxChPortDefaultUPSet_PARAMS, NULL},
    {"cpssDxChPortDefaultUPGet", 3, cpssDxChPortDefaultUPGet_PARAMS, NULL},
    {"cpssDxChPortModifyUPSet", 3, cpssDxChPortModifyUPSet_PARAMS, NULL},
    {"cpssDxChPortModifyUPGet", 3, cpssDxChPortModifyUPGet_PARAMS, NULL},
    {"cpssDxChCosPortQosTrustModeSet", 3, cpssDxChCosPortQosTrustModeSet_PARAMS, NULL},
    {"cpssDxChCosPortQosTrustModeGet", 3, cpssDxChCosPortQosTrustModeGet_PARAMS, NULL},
    {"cpssDxChCosPortReMapDSCPSet", 3, cpssDxChCosPortReMapDSCPSet_PARAMS, NULL},
    {"cpssDxChCosPortReMapDSCPGet", 3, cpssDxChCosPortReMapDSCPGet_PARAMS, NULL},
    {"cpssDxChCosPortVlanQoSCfgEntryGet", 3, cpssDxChCosPortVlanQoSCfgEntryGet_PARAMS, NULL},
    {"cpssDxChCosRemarkEntryGet", 3, cpssDxChCosRemarkEntryGet_PARAMS, NULL},
    {"cpssDxChCosQoSProfileEntryGet", 3, cpssDxChCosRemarkEntryGet_PARAMS, NULL},
    {"cpssDxChCosTrustDsaTagQosModeSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCosTrustDsaTagQosModeGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCosTrustExpModeSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCosTrustExpModeGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCosL2TrustModeVlanTagSelectSet", 4, cpssDxChCosL2TrustModeVlanTagSelectSet_PARAMS, NULL},
    {"cpssDxChCosL2TrustModeVlanTagSelectGet", 4, cpssDxChCosL2TrustModeVlanTagSelectGet_PARAMS, NULL},
    {"cpssDxChCosPortUpProfileIndexSet", 4, cpssDxChCosPortUpProfileIndexSet_PARAMS, NULL},
    {"cpssDxChCosPortUpProfileIndexGet", 4, cpssDxChCosPortUpProfileIndexGet_PARAMS, NULL},
    {"cpssDxChCosPortTrustQosMappingTableIndexSet", 4, cpssDxChCosPortTrustQosMappingTableIndexSet_PARAMS, NULL},
    {"cpssDxChCosPortTrustQosMappingTableIndexGet", 4, cpssDxChCosPortTrustQosMappingTableIndexGet_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosMappingTableIndexSet", 3, cpssDxChCosPortEgressQosMappingTableIndexSet_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosMappingTableIndexGet", 3, cpssDxChCosPortEgressQosMappingTableIndexGet_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosExpMappingEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosExpMappingEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosTcDpMappingEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosTcDpMappingEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosUpMappingEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosUpMappingEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosDscpMappingEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChCosPortEgressQosDscpMappingEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChCosEgressDscp2DscpMappingEntrySet", 4, cpssDxChCosEgressDscp2DscpMappingEntrySet_PARAMS, NULL},
    {"cpssDxChCosEgressDscp2DscpMappingEntryGet", 4, cpssDxChCosEgressDscp2DscpMappingEntryGet_PARAMS, NULL},
    {"cpssDxChCosEgressExp2ExpMappingEntrySet", 4, cpssDxChCosEgressExp2ExpMappingEntrySet_PARAMS, NULL},
    {"cpssDxChCosEgressExp2ExpMappingEntryGet", 4, cpssDxChCosEgressExp2ExpMappingEntryGet_PARAMS, NULL},
    {"cpssDxChCosEgressUp2UpMappingEntrySet", 4, cpssDxChCosEgressUp2UpMappingEntrySet_PARAMS, NULL},
    {"cpssDxChCosEgressUp2UpMappingEntryGet", 4, cpssDxChCosEgressUp2UpMappingEntryGet_PARAMS, NULL},
    {"cpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet", 7, cpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet_PARAMS, NULL},
    {"cpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet", 7, cpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_COS(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChCosLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChCosLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

