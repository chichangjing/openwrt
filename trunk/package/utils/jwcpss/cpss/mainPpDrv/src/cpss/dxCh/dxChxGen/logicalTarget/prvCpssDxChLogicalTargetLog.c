/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLogicalTargetLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/log/private/prvCpssDxChLog.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/cpssDxChLogicalTargetMapping.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/private/prvCpssDxChLogicalTargetLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT[]  =
{
    "CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_4K_VID_32_LP_E",
    "CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_2K_VID_64_LP_E",
    "CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_1K_VID_128_LP_E",
    "CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_512_VID_256_LP_E",
    "CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_256_VID_512_LP_E",
    "CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_128_VID_1K_LP_E",
    "CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_64_VID_2K_LP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, outputInterface, CPSS_DXCH_OUTPUT_INTERFACE_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, egressVlanFilteringEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, egressVlanTagStateEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, egressVlanTagState, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, egressTagTpidIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, assignVid0Enable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid0);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR_logicalPortMappingTablePtr = {
     "logicalPortMappingTablePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vlanIdIndex = {
     "vlanIdIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_logicalDevNum = {
     "logicalDevNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_logicalPortNum = {
     "logicalPortNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR_logicalPortMappingTablePtr = {
     "logicalPortMappingTablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_vlanIdIndexPtr = {
     "vlanIdIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingEgressMappingEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U32_vlanIdIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingEgressMappingEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_GT_U32_PTR_vlanIdIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingEgressMemberSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_vlanIdIndex,
    &IN_GT_U8_logicalDevNum,
    &IN_GT_U8_logicalPortNum,
    &IN_GT_BOOL_isMember
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingEgressMemberGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_vlanIdIndex,
    &IN_GT_U8_logicalDevNum,
    &IN_GT_U8_logicalPortNum,
    &OUT_GT_BOOL_PTR_isMemberPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingDeviceEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_logicalDevNum,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingTableEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_logicalDevNum,
    &IN_GT_U8_logicalPortNum,
    &IN_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR_logicalPortMappingTablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingTableEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_logicalDevNum,
    &IN_GT_U8_logicalPortNum,
    &OUT_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC_PTR_logicalPortMappingTablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingDeviceEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_logicalDevNum,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT_PTR_modePtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChLogicalTargetLogLibDb[] = {
    {"cpssDxChLogicalTargetMappingEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingDeviceEnableSet", 3, cpssDxChLogicalTargetMappingDeviceEnableSet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingDeviceEnableGet", 3, cpssDxChLogicalTargetMappingDeviceEnableGet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingTableEntrySet", 4, cpssDxChLogicalTargetMappingTableEntrySet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingTableEntryGet", 4, cpssDxChLogicalTargetMappingTableEntryGet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet", 2, cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet", 2, cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet", 2, prvCpssLogGenDevNumCounter_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet", 2, prvCpssLogGenDevNumCounterPtr_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressMappingEntrySet", 3, cpssDxChLogicalTargetMappingEgressMappingEntrySet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressMappingEntryGet", 3, cpssDxChLogicalTargetMappingEgressMappingEntryGet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressMemberSet", 5, cpssDxChLogicalTargetMappingEgressMemberSet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressMemberGet", 5, cpssDxChLogicalTargetMappingEgressMemberGet_PARAMS, NULL},
    {"cpssDxChLogicalTargetMappingEgressMemberTableClear", 1, prvCpssLogGenDevNum_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_LOGICAL_TARGET(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChLogicalTargetLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChLogicalTargetLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

