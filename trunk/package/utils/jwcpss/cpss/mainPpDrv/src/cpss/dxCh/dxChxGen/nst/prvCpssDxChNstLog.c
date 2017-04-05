/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChNstLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>
#include <cpss/dxCh/dxChxGen/nst/private/prvCpssDxChNstLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/nst/private/prvCpssGenNstLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT[]  =
{
    "CPSS_DXCH_NST_PORT_ISOLATION_DISABLE_E",
    "CPSS_DXCH_NST_PORT_ISOLATION_L2_ENABLE_E",
    "CPSS_DXCH_NST_PORT_ISOLATION_L3_ENABLE_E",
    "CPSS_DXCH_NST_PORT_ISOLATION_ALL_ENABLE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT[]  =
{
    "CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E",
    "CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT_trafficType = {
     "trafficType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_INTERFACE_INFO_STC_srcInterface = {
     "srcInterface", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NST_AM_PARAM_ENT_paramType = {
     "paramType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NST_AM_PARAM_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NST_CHECK_ENT_checkType = {
     "checkType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NST_CHECK_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NST_EGRESS_FRW_FILTER_ENT_filterType = {
     "filterType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NST_EGRESS_FRW_FILTER_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NST_INGRESS_FRW_FILTER_ENT_filterType = {
     "filterType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NST_INGRESS_FRW_FILTER_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORTS_BMP_STC_PTR_localPortsMembersPtr = {
     "localPortsMembersPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_cpuPortMember = {
     "cpuPortMember", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_accessLevel = {
     "accessLevel", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_daAccessLevel = {
     "daAccessLevel", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dipAccessLevel = {
     "dipAccessLevel", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ingressCnt = {
     "ingressCnt", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numberOfDeviceBits = {
     "numberOfDeviceBits", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numberOfPortBits = {
     "numberOfPortBits", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numberOfTrunkBits = {
     "numberOfTrunkBits", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_saAccessLevel = {
     "saAccessLevel", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sipAccessLevel = {
     "sipAccessLevel", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_trunkIndexBase = {
     "trunkIndexBase", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORTS_BMP_STC_PTR_localPortsMembersPtr = {
     "localPortsMembersPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_cpuPortMemberPtr = {
     "cpuPortMemberPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_accessLevelPtr = {
     "accessLevelPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ingressCntPtr = {
     "ingressCntPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numberOfDeviceBitsPtr = {
     "numberOfDeviceBitsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numberOfPortBitsPtr = {
     "numberOfPortBitsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numberOfTrunkBitsPtr = {
     "numberOfTrunkBitsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_trunkIndexBasePtr = {
     "trunkIndexBasePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationTableEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT_trafficType,
    &IN_CPSS_INTERFACE_INFO_STC_srcInterface,
    &IN_GT_BOOL_cpuPortMember,
    &IN_CPSS_PORTS_BMP_STC_PTR_localPortsMembersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationPortAdd_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT_trafficType,
    &IN_CPSS_INTERFACE_INFO_STC_srcInterface,
    &IN_GT_PHYSICAL_PORT_NUM_portNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationTableEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT_trafficType,
    &IN_CPSS_INTERFACE_INFO_STC_srcInterface,
    &OUT_GT_BOOL_PTR_cpuPortMemberPtr,
    &OUT_CPSS_PORTS_BMP_STC_PTR_localPortsMembersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstDefaultAccessLevelsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_NST_AM_PARAM_ENT_paramType,
    &IN_GT_U32_accessLevel
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstDefaultAccessLevelsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_NST_AM_PARAM_ENT_paramType,
    &OUT_GT_U32_PTR_accessLevelPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstProtSanityCheckSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_NST_CHECK_ENT_checkType,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstProtSanityCheckGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_NST_CHECK_ENT_checkType,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortGroupIngressFrwFilterDropCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_ingressCnt
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortGroupIngressFrwFilterDropCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_ingressCntPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortEgressFrwFilterSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_NST_EGRESS_FRW_FILTER_ENT_filterType,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortEgressFrwFilterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_NST_EGRESS_FRW_FILTER_ENT_filterType,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIngressFrwFilterSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_NST_INGRESS_FRW_FILTER_ENT_filterType,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIngressFrwFilterGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_NST_INGRESS_FRW_FILTER_ENT_filterType,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstIngressFrwFilterDropCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ingressCnt
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationLookupBitsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_numberOfPortBits,
    &IN_GT_U32_numberOfDeviceBits,
    &IN_GT_U32_numberOfTrunkBits
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstBridgeAccessMatrixCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_saAccessLevel,
    &IN_GT_U32_daAccessLevel,
    &IN_CPSS_PACKET_CMD_ENT_command
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstBridgeAccessMatrixCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_saAccessLevel,
    &IN_GT_U32_daAccessLevel,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstRouterAccessMatrixCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_sipAccessLevel,
    &IN_GT_U32_dipAccessLevel,
    &IN_CPSS_PACKET_CMD_ENT_command
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstRouterAccessMatrixCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_sipAccessLevel,
    &IN_GT_U32_dipAccessLevel,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationLookupTrunkIndexBaseSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_trunkIndexBase
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstIngressFrwFilterDropCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_ingressCntPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationLookupBitsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_numberOfPortBitsPtr,
    &OUT_GT_U32_PTR_numberOfDeviceBitsPtr,
    &OUT_GT_U32_PTR_numberOfTrunkBitsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNstPortIsolationLookupTrunkIndexBaseGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_trunkIndexBasePtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChNstLogLibDb[] = {
    {"cpssDxChNstBridgeAccessMatrixCmdSet", 4, cpssDxChNstBridgeAccessMatrixCmdSet_PARAMS, NULL},
    {"cpssDxChNstBridgeAccessMatrixCmdGet", 4, cpssDxChNstBridgeAccessMatrixCmdGet_PARAMS, NULL},
    {"cpssDxChNstDefaultAccessLevelsSet", 3, cpssDxChNstDefaultAccessLevelsSet_PARAMS, NULL},
    {"cpssDxChNstDefaultAccessLevelsGet", 3, cpssDxChNstDefaultAccessLevelsGet_PARAMS, NULL},
    {"cpssDxChNstProtSanityCheckSet", 3, cpssDxChNstProtSanityCheckSet_PARAMS, NULL},
    {"cpssDxChNstProtSanityCheckGet", 3, cpssDxChNstProtSanityCheckGet_PARAMS, NULL},
    {"cpssDxChNstPortIngressFrwFilterSet", 4, cpssDxChNstPortIngressFrwFilterSet_PARAMS, NULL},
    {"cpssDxChNstPortIngressFrwFilterGet", 4, cpssDxChNstPortIngressFrwFilterGet_PARAMS, NULL},
    {"cpssDxChNstPortEgressFrwFilterSet", 4, cpssDxChNstPortEgressFrwFilterSet_PARAMS, NULL},
    {"cpssDxChNstPortEgressFrwFilterGet", 4, cpssDxChNstPortEgressFrwFilterGet_PARAMS, NULL},
    {"cpssDxChNstIngressFrwFilterDropCntrSet", 2, cpssDxChNstIngressFrwFilterDropCntrSet_PARAMS, NULL},
    {"cpssDxChNstIngressFrwFilterDropCntrGet", 2, cpssDxChNstIngressFrwFilterDropCntrGet_PARAMS, NULL},
    {"cpssDxChNstRouterAccessMatrixCmdSet", 4, cpssDxChNstRouterAccessMatrixCmdSet_PARAMS, NULL},
    {"cpssDxChNstRouterAccessMatrixCmdGet", 4, cpssDxChNstRouterAccessMatrixCmdGet_PARAMS, NULL},
    {"cpssDxChNstPortGroupIngressFrwFilterDropCntrSet", 3, cpssDxChNstPortGroupIngressFrwFilterDropCntrSet_PARAMS, NULL},
    {"cpssDxChNstPortGroupIngressFrwFilterDropCntrGet", 3, cpssDxChNstPortGroupIngressFrwFilterDropCntrGet_PARAMS, NULL},
    {"cpssDxChNstPortIsolationEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChNstPortIsolationEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChNstPortIsolationTableEntrySet", 5, cpssDxChNstPortIsolationTableEntrySet_PARAMS, NULL},
    {"cpssDxChNstPortIsolationTableEntryGet", 5, cpssDxChNstPortIsolationTableEntryGet_PARAMS, NULL},
    {"cpssDxChNstPortIsolationPortAdd", 4, cpssDxChNstPortIsolationPortAdd_PARAMS, NULL},
    {"cpssDxChNstPortIsolationPortDelete", 4, cpssDxChNstPortIsolationPortAdd_PARAMS, NULL},
    {"cpssDxChNstPortIsolationModeSet", 3, cpssDxChNstPortIsolationModeSet_PARAMS, NULL},
    {"cpssDxChNstPortIsolationModeGet", 3, cpssDxChNstPortIsolationModeGet_PARAMS, NULL},
    {"cpssDxChNstPortIsolationLookupBitsSet", 4, cpssDxChNstPortIsolationLookupBitsSet_PARAMS, NULL},
    {"cpssDxChNstPortIsolationLookupBitsGet", 4, cpssDxChNstPortIsolationLookupBitsGet_PARAMS, NULL},
    {"cpssDxChNstPortIsolationOnEportsEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChNstPortIsolationOnEportsEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChNstPortIsolationLookupTrunkIndexBaseSet", 2, cpssDxChNstPortIsolationLookupTrunkIndexBaseSet_PARAMS, NULL},
    {"cpssDxChNstPortIsolationLookupTrunkIndexBaseGet", 2, cpssDxChNstPortIsolationLookupTrunkIndexBaseGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_NST(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChNstLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChNstLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

