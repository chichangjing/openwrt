/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLogicalTargetLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChLogicalTargetLogh
#define __prvCpssDxChLogicalTargetLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR_logicalPortMappingTablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vlanIdIndex;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_logicalDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_logicalPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR_logicalPortMappingTablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_vlanIdIndexPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEnableSet_E = (CPSS_LOG_LIB_LOGICAL_TARGET_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingDeviceEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingDeviceEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingTableEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingTableEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressMappingEntrySet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressMappingEntryGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressMemberSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressMemberGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChLogicalTargetMappingEgressMemberTableClear_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChLogicalTargetLogh */
