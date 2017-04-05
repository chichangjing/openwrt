/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCosLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChCosLogh
#define __prvCpssDxChCosLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_COS_PROFILE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_COS_PROFILE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_COS_PROFILE_STC_PTR_cosPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_QOS_RESOLVE_ENT_macQosResolvType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_QOS_ENTRY_STC_PTR_macQosCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_QOS_ENTRY_STC_PTR_portQosCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_QOS_PORT_TRUST_MODE_ENT_portQosTrustMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_VLAN_TAG_TYPE_ENT_vlanTagType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enableDscpMutation;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isDefaultVlanTagType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_upOverrideEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_useUpAsIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dscp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_entryIdx;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mappingTableIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newDscp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newExp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newProfileIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newUp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_upProfileIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_cfiDeiBit;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_defaultUserPrio;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_dscp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_newDscp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_COS_PROFILE_STC_PTR_cosPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_QOS_RESOLVE_ENT_PTR_macQosResolvTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_QOS_ENTRY_STC_PTR_macQosCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_QOS_ENTRY_STC_PTR_portQosCfgPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_QOS_PORT_TRUST_MODE_ENT_PTR_portQosTrustModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_VLAN_TAG_TYPE_ENT_PTR_vlanTagTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_enableDscpMutationPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_upOverrideEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_useUpAsIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dscpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_entryPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mappingTableIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_newDscpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_newExpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_newProfileIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_newUpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_upProfileIndexPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_cfiDeiBitPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_defaultUserPrioPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_newDscpPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChCosProfileEntrySet_E = (CPSS_LOG_LIB_COS_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChCosProfileEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosDscpToProfileMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosDscpToProfileMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosDscpMutationEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosDscpMutationEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosUpCfiDeiToProfileMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosUpCfiDeiToProfileMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortDpToCfiDeiMapEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortDpToCfiDeiMapEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortDpToCfiDei1MapEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortDpToCfiDei1MapEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosDpToCfiDeiMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosDpToCfiDeiMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosExpToProfileMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosExpToProfileMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortQosConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortQosConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosMacQosEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosMacQosEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosMacQosConflictResolutionSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosMacQosConflictResolutionGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosRemarkTblEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosRemarkTblEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDefaultUPSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortDefaultUPGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortModifyUPSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChPortModifyUPGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortQosTrustModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortQosTrustModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortReMapDSCPSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortReMapDSCPGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortVlanQoSCfgEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosRemarkEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosQoSProfileEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosTrustDsaTagQosModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosTrustDsaTagQosModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosTrustExpModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosTrustExpModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosL2TrustModeVlanTagSelectSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosL2TrustModeVlanTagSelectGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortUpProfileIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortUpProfileIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortTrustQosMappingTableIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortTrustQosMappingTableIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosMappingTableIndexSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosMappingTableIndexGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosExpMappingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosExpMappingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosTcDpMappingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosTcDpMappingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosUpMappingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosUpMappingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosDscpMappingEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosPortEgressQosDscpMappingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosEgressDscp2DscpMappingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosEgressDscp2DscpMappingEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosEgressExp2ExpMappingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosEgressExp2ExpMappingEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosEgressUp2UpMappingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosEgressUp2UpMappingEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChCosLogh */
