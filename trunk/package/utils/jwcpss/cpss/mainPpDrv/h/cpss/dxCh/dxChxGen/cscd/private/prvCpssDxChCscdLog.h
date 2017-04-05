/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCscdLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChCscdLogh
#define __prvCpssDxChCscdLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_CSCD_LINK_TYPE_STC_PTR_cascadeLinkPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_CSCD_PORT_TYPE_ENT_portType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIRECTION_ENT_portDirection;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_cpuToCpuDp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_ctrlDp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_newDp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DP_LEVEL_ENT_remapDp;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT_assignmentMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT_crcMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT_portQosDsaMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT_remapType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC_PTR_tcDpRemappingPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR_tcMappingsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT_srcPortTrunkHashEn;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT_hwDevMode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_DIRECTION_ENT_portDirection;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_egressAttributesLocallyEn;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enableOwnDevFltr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isLooped;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_aggHwDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_remotePhysicalHwDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_reservedHwDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_sourceHwDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_targetHwDevNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_remotePhysicalPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_secondaryTargetPort;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_aggPortNum;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_portBaseEport;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_trunkBaseEport;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_aggSrcId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_newTc;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_physicalPortBase;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_srcDevLsbAmount;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_srcId;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_srcPortLsbAmount;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_srcTrunkLsbAmount;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_ctrlTc;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_remapTc;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_CSCD_LINK_TYPE_STC_PTR_cascadeLinkPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_CSCD_PORT_TYPE_ENT_PTR_portTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_cpuToCpuDpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_ctrlDpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_remapDpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DP_LEVEL_ENT_PTR_remappedDpPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT_PTR_assignmentModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT_PTR_crcModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT_PTR_portQosDsaTrustModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT_PTR_remapTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC_PTR_tcMappingsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT_PTR_srcPortTrunkHashEnPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT_PTR_hwDevModePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_egressAttributesLocallyEnPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_enableOwnDevFltrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isLoopedPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_aggHwDevNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_remotePhysicalHwDevNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_reservedHwDevNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PHYSICAL_PORT_NUM_PTR_remotePhysicalPortNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PHYSICAL_PORT_NUM_PTR_secondaryTargetPortPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_aggPortNumPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_portBaseEportPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_trunkBaseEportPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_aggSrcIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_physicalPortBasePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_remappedTcPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_srcDevLsbAmountPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_srcIdPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_srcPortLsbAmountPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_srcTrunkLsbAmountPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_ctrlTcPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_remapTcPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortTypeSet_E = (CPSS_LOG_LIB_CSCD_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdDevMapTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdDevMapTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdRemapQosModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdRemapQosModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCtrlQosSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCtrlQosGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdRemapDataQosTblSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdRemapDataQosTblGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdDsaSrcDevFilterSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdDsaSrcDevFilterGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdHyperGInternalPortModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdHyperGInternalPortModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdHyperGPortCrcModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdHyperGPortCrcModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdFastFailoverFastStackRecoveryEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdFastFailoverFastStackRecoveryEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdFastFailoverSecondaryTargetPortMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdFastFailoverSecondaryTargetPortMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdFastFailoverPortIsLoopedSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdFastFailoverPortIsLoopedGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdQosPortTcRemapEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdQosPortTcRemapEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdQosTcRemapTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdQosTcRemapTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdQosTcDpRemapTableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdQosTcDpRemapTableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortBridgeBypassEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortBridgeBypassEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdOrigSrcPortFilterEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdOrigSrcPortFilterEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdDevMapLookupModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdDevMapLookupModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortLocalDevMapLookupEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortLocalDevMapLookupEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisModeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisReservedDevNumSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisReservedDevNumGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisSrcIdSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisSrcIdGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdDbRemoteHwDevNumModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdDbRemoteHwDevNumModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortMruCheckOnCascadeEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortMruCheckOnCascadeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMyPhysicalPortBaseSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMyPhysicalPortBaseGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMyPhysicalPortAssignSrcDevEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisMyPhysicalPortAssignSrcDevEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortStackAggregationEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortStackAggregationEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortStackAggregationConfigSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortStackAggregationConfigGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisRemotePhysicalPortMapSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdCentralizedChassisRemotePhysicalPortMapGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortQosDsaModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdPortQosDsaModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdHashInDsaEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChCscdHashInDsaEnableGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChCscdLogh */
