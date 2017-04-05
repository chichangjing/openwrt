/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChIpLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpNat.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/ip/private/prvCpssDxChIpLog.h>
#include <cpss/dxCh/dxChxGen/tunnel/private/prvCpssDxChTunnelLog.h>
#include <cpss/generic/cpssHwInit/private/prvCpssGenCpssHwInitLog.h>
#include <cpss/generic/ip/private/prvCpssGenIpLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/port/private/prvCpssGenPortLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_IP_BRG_EXCP_CMD_ENT[]  =
{
    "CPSS_DXCH_IP_BRG_UC_IPV4_TRAP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_UC_IPV6_TRAP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_UC_IPV4_SOFT_DROP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_UC_IPV6_SOFT_DROP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_MC_IPV4_TRAP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_MC_IPV6_TRAP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_MC_IPV4_SOFT_DROP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_MC_IPV6_SOFT_DROP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_ARP_TRAP_EXCP_CMD_E",
    "CPSS_DXCH_IP_BRG_ARP_SOFT_DROP_EXCP_CMD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_BRG_EXCP_CMD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT[]  =
{
    "CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E",
    "CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E",
    "CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_BRG_SERVICE_ENT[]  =
{
    "CPSS_DXCH_IP_HEADER_CHECK_BRG_SERVICE_E",
    "CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E",
    "CPSS_DXCH_IP_SIP_SA_CHECK_BRG_SERVICE_E",
    "CPSS_DXCH_IP_SIP_FILTER_BRG_SERVICE_E",
    "CPSS_DXCH_IP_BRG_SERVICE_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_BRG_SERVICE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_CNT_SET_MODE_ENT[]  =
{
    "CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E",
    "CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_CNT_SET_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_DROP_CNT_MODE_ENT[]  =
{
    "CPSS_DXCH_IP_DROP_CNT_COUNT_ALL_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_IP_HEADER_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_DIP_DA_MISMATCH_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_SIP_SA_MISMATCH_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_ILLEGAL_ADDRESS_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_UC_RPF_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_MC_RPF_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_TTL_HOP_LIMIT_EXCEEDED_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_MTU_EXCEEDED_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_OPTION_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_IPV6_SCOPE_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_UC_SIP_FILTER_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_NH_CMD_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_ACCESS_MATRIX_MODE_E",
    "CPSS_DXCH_IP_DROP_CNT_SIP_ALL_ZEROS_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_DROP_CNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_EXCEPTION_TYPE_ENT[]  =
{
    "CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E",
    "CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E",
    "CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E",
    "CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E",
    "CPSS_DXCH_IP_EXCP_UC_DIP_DA_MISMATCH_E",
    "CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E",
    "CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_E",
    "CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E",
    "CPSS_DXCH_IP_EXCP_UC_ALL_ZERO_SIP_E",
    "CPSS_DXCH_IP_EXCP_MC_ALL_ZERO_SIP_E",
    "CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E",
    "CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E",
    "CPSS_DXCH_IP_EXCP_UC_NON_HOP_BY_HOP_EXT_E",
    "CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E",
    "CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E",
    "CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E",
    "CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E",
    "CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_NON_DF_E",
    "CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_NON_DF_E",
    "CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_DF_E",
    "CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_DF_E",
    "CPSS_DXCH_IP_EXCEPTION_TYPE_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_EXCEPTION_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_HEADER_ERROR_ENT[]  =
{
    "CPSS_DXCH_IP_HEADER_ERROR_CHECKSUM_ENT",
    "CPSS_DXCH_IP_HEADER_ERROR_VERSION_ENT",
    "CPSS_DXCH_IP_HEADER_ERROR_LENGTH_ENT",
    "CPSS_DXCH_IP_HEADER_ERROR_SIP_DIP_ENT"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_HEADER_ERROR_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT[]  =
{
    "CPSS_DXCH_IP_MLL_PAIR_WRITE_FIRST_LAST_BIT_E",
    "CPSS_DXCH_IP_MLL_PAIR_WRITE_SECOND_LAST_BIT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT[]  =
{
    "CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E",
    "CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT[]  =
{
    "CPSS_DXCH_IP_MT_UC_SCHED_MTU_2K_E",
    "CPSS_DXCH_IP_MT_UC_SCHED_MTU_8K_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_URPF_MODE_ENT[]  =
{
    "CPSS_DXCH_IP_URPF_DISABLE_MODE_E",
    "CPSS_DXCH_IP_URPF_VLAN_MODE_E",
    "CPSS_DXCH_IP_URPF_PORT_TRUNK_MODE_E",
    "CPSS_DXCH_IP_URPF_LOOSE_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_URPF_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT[]  =
{
    "CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_64_CORE_CLOCKS_E",
    "CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_1024_CORE_CLOCKS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT);
char * prvCpssLogEnum_CPSS_IP_NAT_TYPE_ENT[]  =
{
    "CPSS_IP_NAT_TYPE_NAT44_E",
    "CPSS_IP_NAT_TYPE_NAT66_E",
    "CPSS_IP_NAT_TYPE_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_IP_NAT_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_CPU_CODE_INDEX_ENT[]  =
{
    "CPSS_DXCH_IP_CPU_CODE_IDX_0_E",
    "CPSS_DXCH_IP_CPU_CODE_IDX_1_E",
    "CPSS_DXCH_IP_CPU_CODE_IDX_2_E",
    "CPSS_DXCH_IP_CPU_CODE_IDX_3_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_CPU_CODE_INDEX_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT[]  =
{
    "CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_RPF_CHECK_E",
    "CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_BIDIRECTIONAL_TREE_CHECK_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT[]  =
{
    "CPSS_DXCH_IP_MULTICAST_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E",
    "CPSS_DXCH_IP_MULTICAST_MLL_RPF_FAIL_COMMAND_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT[]  =
{
    "CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E",
    "CPSS_DXCH_IP_PORT_CNT_MODE_E",
    "CPSS_DXCH_IP_TRUNK_CNT_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT[]  =
{
    "CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E",
    "CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_QOS_E",
    "CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT[]  =
{
    "CPSS_DXCH_IP_UC_ROUTE_ENTRY_E",
    "CPSS_DXCH_IP_UC_ECMP_RPF_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_VLAN_CNT_MODE_ENT[]  =
{
    "CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E",
    "CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_VLAN_CNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT[]  =
{
    "CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E",
    "CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E",
    "CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_ADDRESS_E",
    "CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_ADDRESS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_IPV4_PREFIX_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IPV4_PREFIX_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vrId);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, ipAddr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isMcSource);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mcGroupIndexRow);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mcGroupIndexColumn);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IPV6_PREFIX_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IPV6_PREFIX_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vrId);
    PRV_CPSS_LOG_STC_IPV6_MAC(valPtr, ipAddr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isMcSource);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mcGroupIndexRow);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_COUNTER_SET_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_COUNTER_SET_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inUcPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inMcPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inUcNonRoutedExcpPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inUcNonRoutedNonExcpPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inMcNonRoutedExcpPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inMcNonRoutedNonExcpPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inUcTrappedMirrorPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, inMcTrappedMirrorPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mcRfpFailPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, outUcRoutedPkts);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_ECMP_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_ECMP_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, randomEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfPaths);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, routeEntryBaseIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_LTT_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, routeType, CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfPaths);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, routeEntryBaseIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ucRPFCheckEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, sipSaCheckMismatchEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipv6MCGroupScopeLevel, CPSS_IPV6_PREFIX_SCOPE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, cmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, cpuCodeIdx, CPSS_DXCH_IP_CPU_CODE_INDEX_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, appSpecificCpuCodeEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ttlHopLimitDecEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ttlHopLimDecOptionsExtChkByPass);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ingressMirror);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ingressMirrorToAnalyzerIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, qosProfileMarkingEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfileIndex);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, countSet, CPSS_IP_CNT_SET_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, multicastRPFCheckEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, multicastIngressVlanCheck, CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, multicastRPFVlan);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, multicastRPFRoutingSharedTreeIndex);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, multicastRPFFailCommandMode, CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, RPFFailCommand, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, scopeCheckingEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, siteId, CPSS_IP_SITE_ID_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mtuProfileIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, internalMLLPointer);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, externalMLLPointer);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_MLL_PAIR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_MLL_PAIR_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, firstMllNode, CPSS_DXCH_IP_MLL_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, secondMllNode, CPSS_DXCH_IP_MLL_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextPointer);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_MLL_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_MLL_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, mllRPFFailCommand, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTunnelStart);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, nextHopInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopVlanId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopTunnelPointer);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tunnelStartPassengerType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ttlHopLimitThreshold);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, excludeSrcVlan);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, last);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_NAT44_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_NAT44_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macDa);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, modifyDip);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, newDip);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, modifySip);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, newSip);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, modifyTcpUdpDstPort);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, newTcpUdpDstPort);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, modifyTcpUdpSrcPort);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, newTcpUdpSrcPort);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_NAT66_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_NAT66_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macDa);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyCommand, CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT);
    PRV_CPSS_LOG_STC_IPV6_MAC(valPtr, address);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, prefixSize);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, vlanArray, 8, GT_U16);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, cmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, cpuCodeIdx, CPSS_DXCH_IP_CPU_CODE_INDEX_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, appSpecificCpuCodeEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, unicastPacketSipFilterEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ttlHopLimitDecEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ttlHopLimDecOptionsExtChkByPass);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ingressMirror);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ingressMirrorToAnalyzerIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, qosProfileMarkingEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfileIndex);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, countSet, CPSS_IP_CNT_SET_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, trapMirrorArpBcEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sipAccessLevel);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dipAccessLevel);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ICMPRedirectEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, scopeCheckingEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, siteId, CPSS_IP_SITE_ID_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mtuProfileIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTunnelStart);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isNat);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopVlanId);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, nextHopInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopARPPointer);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopTunnelPointer);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopNatPointer);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopVlanId1);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_IPV4_PREFIX_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IPV4_PREFIX_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IPV4_PREFIX_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IPV6_PREFIX_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IPV6_PREFIX_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IPV6_PREFIX_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_BRG_EXCP_CMD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_BRG_EXCP_CMD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_BRG_EXCP_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_BRG_SERVICE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_BRG_SERVICE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_BRG_SERVICE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_CNT_SET_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_CNT_SET_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_CNT_SET_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_CNT_SET_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_CNT_SET_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_IP_CNT_SET_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_COUNTER_SET_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_COUNTER_SET_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_COUNTER_SET_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_DROP_CNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_DROP_CNT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_DROP_CNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_DROP_CNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_DROP_CNT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_IP_DROP_CNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_ECMP_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_ECMP_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_ECMP_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_EXCEPTION_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_EXCEPTION_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_EXCEPTION_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_HEADER_ERROR_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_HEADER_ERROR_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_HEADER_ERROR_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_LTT_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_MLL_PAIR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_MLL_PAIR_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_MLL_PAIR_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_NAT_ENTRY_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IP_NAT_ENTRY_UNT*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IP_NAT_ENTRY_UNT_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_URPF_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_URPF_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IP_URPF_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IP_URPF_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IP_URPF_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_IP_URPF_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT);
}
void prvCpssLogParamFunc_CPSS_IP_NAT_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IP_NAT_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_IP_NAT_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_IP_NAT_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IP_NAT_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_IP_NAT_TYPE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC_PTR_routeEntriesArray = {
     "routeEntriesArray", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IPV4_PREFIX_STC_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPV4_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IPV4_PREFIX_STC_PTR_prefixPtr = {
     "prefixPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPV4_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IPV6_PREFIX_STC_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPV6_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IPV6_PREFIX_STC_PTR_prefixPtr = {
     "prefixPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPV6_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_BRG_EXCP_CMD_ENT_bridgeExceptionCmd = {
     "bridgeExceptionCmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_BRG_EXCP_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT_enableDisableMode = {
     "enableDisableMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_BRG_SERVICE_ENT_bridgeService = {
     "bridgeService", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_BRG_SERVICE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_CNT_SET_MODE_ENT_cntSetMode = {
     "cntSetMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_CNT_SET_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR_interfaceCfgPtr = {
     "interfaceCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR_interfaceModeCfgPtr = {
     "interfaceModeCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_COUNTER_SET_STC_PTR_countersPtr = {
     "countersPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_COUNTER_SET_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_DROP_CNT_MODE_ENT_dropCntMode = {
     "dropCntMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_DROP_CNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_ECMP_ENTRY_STC_PTR_ecmpEntryPtr = {
     "ecmpEntryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_ECMP_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_EXCEPTION_TYPE_ENT_exceptionType = {
     "exceptionType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_EXCEPTION_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_HEADER_ERROR_ENT_ipHeaderErrorType = {
     "ipHeaderErrorType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_HEADER_ERROR_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_lttEntryPtr = {
     "lttEntryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LTT_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC_PTR_routeEntryPtr = {
     "routeEntryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT_mllEntryPart = {
     "mllEntryPart", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_MLL_PAIR_STC_PTR_mllPairEntryPtr = {
     "mllPairEntryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_MLL_PAIR_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT_schedulingMode = {
     "schedulingMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT_schedMtu = {
     "schedMtu", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_NAT_ENTRY_UNT_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_NAT_ENTRY_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC_PTR_routeEntriesArray = {
     "routeEntriesArray", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IP_URPF_MODE_ENT_uRpfMode = {
     "uRpfMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IP_URPF_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT_tokenBucketIntervalUpdateRatio = {
     "tokenBucketIntervalUpdateRatio", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IPV6_MLL_SELECTION_RULE_ENT_mllSelectionRule = {
     "mllSelectionRule", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IPV6_MLL_SELECTION_RULE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScope = {
     "addressScope", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IPV6_PREFIX_SCOPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeDest = {
     "addressScopeDest", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IPV6_PREFIX_SCOPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeSrc = {
     "addressScopeSrc", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IPV6_PREFIX_SCOPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IP_CNT_SET_ENT_cntSet = {
     "cntSet", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IP_CNT_SET_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IP_NAT_TYPE_ENT_natType = {
     "natType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IP_NAT_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IP_UNICAST_MULTICAST_ENT_ucMcEnable = {
     "ucMcEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IP_UNICAST_MULTICAST_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IP_UNICAST_MULTICAST_ENT_ucMcSet = {
     "ucMcSet", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IP_UNICAST_MULTICAST_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_SA_LSB_MODE_ENT_saLsbMode = {
     "saLsbMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_MAC_SA_LSB_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PACKET_CMD_ENT_arpBcMode = {
     "arpBcMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PACKET_CMD_ENT_scopeCommand = {
     "scopeCommand", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT_tokenBucketMode = {
     "tokenBucketMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PORT_TX_DROP_SHAPER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_UNICAST_MULTICAST_ENT_prefixType = {
     "prefixType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_UNICAST_MULTICAST_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_borderCrossed = {
     "borderCrossed", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enableRouterTrigger = {
     "enableRouterTrigger", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enableRouting = {
     "enableRouting", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_enableService = {
     "enableService", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_lastBit = {
     "lastBit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_mask = {
     "mask", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_mllBridgeEnable = {
     "mllBridgeEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_multiTargetRateShaperEnable = {
     "multiTargetRateShaperEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_refreshEnable = {
     "refreshEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_ucSPEnable = {
     "ucSPEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_PTR_arpMacAddrPtr = {
     "arpMacAddrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_PTR_macSaAddrPtr = {
     "macSaAddrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPV6ADDR_prefix = {
     "prefix", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_vlanPortId = {
     "vlanPortId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_baseRouteEntryIndex = {
     "baseRouteEntryIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ctrlMultiTargetTCQueue = {
     "ctrlMultiTargetTCQueue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dropPkts = {
     "dropPkts", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ecmpEntryIndex = {
     "ecmpEntryIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_failRpfMultiTargetTCQueue = {
     "failRpfMultiTargetTCQueue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lttTtiColumn = {
     "lttTtiColumn", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lttTtiRow = {
     "lttTtiRow", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxBucketSize = {
     "maxBucketSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mcWeight = {
     "mcWeight", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mllOutMCPkts = {
     "mllOutMCPkts", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mtu = {
     "mtu", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mtuProfileIndex = {
     "mtuProfileIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_multiTargetTCQueue = {
     "multiTargetTCQueue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_multiTargetTcQueue = {
     "multiTargetTcQueue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_natIndex = {
     "natIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfBits = {
     "numOfBits", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfRouteEntries = {
     "numOfRouteEntries", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_prefixScopeIndex = {
     "prefixScopeIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_qosProfile = {
     "qosProfile", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_queueWeight = {
     "queueWeight", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routeEntryIndex = {
     "routeEntryIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routeEntryOffset = {
     "routeEntryOffset", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerArpIndex = {
     "routerArpIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerBridgedExceptionPkts = {
     "routerBridgedExceptionPkts", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerMacSaIndex = {
     "routerMacSaIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerNextHopTableAgeBitsEntry = {
     "routerNextHopTableAgeBitsEntry", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerNextHopTableAgeBitsEntryIndex = {
     "routerNextHopTableAgeBitsEntryIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerTtiTcamColumn = {
     "routerTtiTcamColumn", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerTtiTcamRow = {
     "routerTtiTcamRow", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_seed = {
     "seed", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sourceIdMask = {
     "sourceIdMask", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_startBit = {
     "startBit", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tokenBucketIntervalSlowUpdateRatio = {
     "tokenBucketIntervalSlowUpdateRatio", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tokenBucketUpdateInterval = {
     "tokenBucketUpdateInterval", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ucWeight = {
     "ucWeight", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_mrstBmp = {
     "mrstBmp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_saMac = {
     "saMac", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPV4_PREFIX_STC_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPV4_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPV4_PREFIX_STC_PTR_prefixPtr = {
     "prefixPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPV4_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPV6_PREFIX_STC_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPV6_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPV6_PREFIX_STC_PTR_prefixPtr = {
     "prefixPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPV6_PREFIX_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_CNT_SET_MODE_ENT_PTR_cntSetModePtr = {
     "cntSetModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_CNT_SET_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR_interfaceModeCfgPtr = {
     "interfaceModeCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_COUNTER_SET_STC_PTR_countersPtr = {
     "countersPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_COUNTER_SET_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_DROP_CNT_MODE_ENT_PTR_dropCntModePtr = {
     "dropCntModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_DROP_CNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_ECMP_ENTRY_STC_PTR_ecmpEntryPtr = {
     "ecmpEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_ECMP_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_lttEntryPtr = {
     "lttEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_LTT_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC_PTR_routeEntryPtr = {
     "routeEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_MLL_PAIR_STC_PTR_mllPairEntryPtr = {
     "mllPairEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_MLL_PAIR_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT_PTR_schedulingModePtr = {
     "schedulingModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT_PTR_schedMtuPtr = {
     "schedMtuPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_NAT_ENTRY_UNT_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_NAT_ENTRY_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IP_URPF_MODE_ENT_PTR_uRpfModePtr = {
     "uRpfModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IP_URPF_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT_PTR_tokenBucketIntervalUpdateRatioPtr = {
     "tokenBucketIntervalUpdateRatioPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_IPV6_MLL_SELECTION_RULE_ENT_PTR_mllSelectionRulePtr = {
     "mllSelectionRulePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_IPV6_MLL_SELECTION_RULE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_IPV6_PREFIX_SCOPE_ENT_PTR_addressScopePtr = {
     "addressScopePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_IPV6_PREFIX_SCOPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_IP_NAT_TYPE_ENT_PTR_natTypePtr = {
     "natTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_IP_NAT_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_SA_LSB_MODE_ENT_PTR_saLsbModePtr = {
     "saLsbModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_SA_LSB_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PACKET_CMD_ENT_PTR_arpBcModePtr = {
     "arpBcModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PACKET_CMD_ENT_PTR_exceptionCmdPtr = {
     "exceptionCmdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PACKET_CMD_ENT_PTR_scopeCommandPtr = {
     "scopeCommandPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT_PTR_tokenBucketModePtr = {
     "tokenBucketModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORT_TX_DROP_SHAPER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_enableRouterTriggerPtr = {
     "enableRouterTriggerPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_enableRoutingPtr = {
     "enableRoutingPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_enableServicePtr = {
     "enableServicePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_mllBridgeEnablePtr = {
     "mllBridgeEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_multiTargetRateShaperEnablePtr = {
     "multiTargetRateShaperEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_refreshEnablePtr = {
     "refreshEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_ucSPEnablePtr = {
     "ucSPEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_arpMacAddrPtr = {
     "arpMacAddrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_macSaAddrPtr = {
     "macSaAddrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_IPV6ADDR_PTR_prefixPtr = {
     "prefixPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ctrlMultiTargetTCQueuePtr = {
     "ctrlMultiTargetTCQueuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dropPktsPtr = {
     "dropPktsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_failRpfMultiTargetTCQueuePtr = {
     "failRpfMultiTargetTCQueuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxBucketSizePtr = {
     "maxBucketSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mcWeightPtr = {
     "mcWeightPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mtuPtr = {
     "mtuPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_multiTargetTCQueuePtr = {
     "multiTargetTCQueuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_natDropPktsPtr = {
     "natDropPktsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfBitsPtr = {
     "numOfBitsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_queueWeightPtr = {
     "queueWeightPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_routeEntryOffsetPtr = {
     "routeEntryOffsetPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_routerBridgedExceptionPktsPtr = {
     "routerBridgedExceptionPktsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_routerMacSaIndexPtr = {
     "routerMacSaIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_routerNextHopTableAgeBitsEntryPtr = {
     "routerNextHopTableAgeBitsEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_seedPtr = {
     "seedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_skipCounterPtr = {
     "skipCounterPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_sourceIdMaskPtr = {
     "sourceIdMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_startBitPtr = {
     "startBitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tokenBucketIntervalSlowUpdateRatioPtr = {
     "tokenBucketIntervalSlowUpdateRatioPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tokenBucketUpdateIntervalPtr = {
     "tokenBucketUpdateIntervalPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ucWeightPtr = {
     "ucWeightPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_mrstBmpPtr = {
     "mrstBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_saMacPtr = {
     "saMacPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpSpecialRouterTriggerEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_BRG_EXCP_CMD_ENT_bridgeExceptionCmd,
    &IN_GT_BOOL_enableRouterTrigger
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpSpecialRouterTriggerEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_BRG_EXCP_CMD_ENT_bridgeExceptionCmd,
    &OUT_GT_BOOL_PTR_enableRouterTriggerPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpBridgeServiceEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_BRG_SERVICE_ENT_bridgeService,
    &IN_CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT_enableDisableMode,
    &IN_GT_BOOL_enableService
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpBridgeServiceEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_BRG_SERVICE_ENT_bridgeService,
    &IN_CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT_enableDisableMode,
    &OUT_GT_BOOL_PTR_enableServicePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpSetDropCntMode_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_DROP_CNT_MODE_ENT_dropCntMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpExceptionCommandSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_EXCEPTION_TYPE_ENT_exceptionType,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_CPSS_PACKET_CMD_ENT_command
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpExceptionCommandGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_EXCEPTION_TYPE_ENT_exceptionType,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_exceptionCmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpHeaderErrorMaskSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_HEADER_ERROR_ENT_ipHeaderErrorType,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_CPSS_UNICAST_MULTICAST_ENT_prefixType,
    &IN_GT_BOOL_mask
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpHeaderErrorMaskGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_IP_HEADER_ERROR_ENT_ipHeaderErrorType,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_CPSS_UNICAST_MULTICAST_ENT_prefixType,
    &OUT_GT_BOOL_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6UcScopeCommandSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeSrc,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeDest,
    &IN_GT_BOOL_borderCrossed,
    &IN_CPSS_PACKET_CMD_ENT_scopeCommand
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6McScopeCommandSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeSrc,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeDest,
    &IN_GT_BOOL_borderCrossed,
    &IN_CPSS_PACKET_CMD_ENT_scopeCommand,
    &IN_CPSS_IPV6_MLL_SELECTION_RULE_ENT_mllSelectionRule
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6UcScopeCommandGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeSrc,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeDest,
    &IN_GT_BOOL_borderCrossed,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_scopeCommandPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6McScopeCommandGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeSrc,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScopeDest,
    &IN_GT_BOOL_borderCrossed,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_scopeCommandPtr,
    &OUT_CPSS_IPV6_MLL_SELECTION_RULE_ENT_PTR_mllSelectionRulePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpCntSetModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_CNT_SET_ENT_cntSet,
    &IN_CPSS_DXCH_IP_CNT_SET_MODE_ENT_cntSetMode,
    &IN_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR_interfaceModeCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpCntSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_CNT_SET_ENT_cntSet,
    &IN_CPSS_DXCH_IP_COUNTER_SET_STC_PTR_countersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpCntSetModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_CNT_SET_ENT_cntSet,
    &OUT_CPSS_DXCH_IP_CNT_SET_MODE_ENT_PTR_cntSetModePtr,
    &OUT_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR_interfaceModeCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpCntGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_CNT_SET_ENT_cntSet,
    &OUT_CPSS_DXCH_IP_COUNTER_SET_STC_PTR_countersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterSourceIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_UNICAST_MULTICAST_ENT_ucMcSet,
    &IN_GT_U32_sourceId,
    &IN_GT_U32_sourceIdMask
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterSourceIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_UNICAST_MULTICAST_ENT_ucMcSet,
    &OUT_GT_U32_PTR_sourceIdPtr,
    &OUT_GT_U32_PTR_sourceIdMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterMacSaLsbModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_SA_LSB_MODE_ENT_saLsbMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpArpBcModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PACKET_CMD_ENT_arpBcMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllMultiTargetShaperTokenBucketModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT_tokenBucketMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRoutingEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_enableRouting
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllBridgeEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_mllBridgeEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMultiTargetRateShaperSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_multiTargetRateShaperEnable,
    &IN_GT_U32_windowSize
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpUcRouteAgingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_refreshEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMultiTargetUcSchedModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_ucSPEnable,
    &IN_GT_U32_ucWeight,
    &IN_GT_U32_mcWeight,
    &IN_CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT_schedMtu
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6AddrPrefixScopeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_IPV6ADDR_prefix,
    &IN_GT_U32_prefixLen,
    &IN_CPSS_IPV6_PREFIX_SCOPE_ENT_addressScope,
    &IN_GT_U32_prefixScopeIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortRouterMacSaLsbModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_MAC_SA_LSB_MODE_ENT_saLsbMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterPortMacSaLsbSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U8_saMac
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortRouterMacSaLsbModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_MAC_SA_LSB_MODE_ENT_PTR_saLsbModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterPortMacSaLsbGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_U8_PTR_saMacPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupCntSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_IP_CNT_SET_ENT_cntSet,
    &IN_CPSS_DXCH_IP_COUNTER_SET_STC_PTR_countersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupCntGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_IP_CNT_SET_ENT_cntSet,
    &OUT_CPSS_DXCH_IP_COUNTER_SET_STC_PTR_countersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupRouterSourceIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_IP_UNICAST_MULTICAST_ENT_ucMcSet,
    &IN_GT_U32_sourceId,
    &IN_GT_U32_sourceIdMask
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupRouterSourceIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_IP_UNICAST_MULTICAST_ENT_ucMcSet,
    &OUT_GT_U32_PTR_sourceIdPtr,
    &OUT_GT_U32_PTR_sourceIdMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupMultiTargetRateShaperSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_BOOL_multiTargetRateShaperEnable,
    &IN_GT_U32_windowSize
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupDropCntSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_dropPkts
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupMllCntSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_mllCntSet,
    &IN_GT_U32_mllOutMCPkts
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_routerBridgedExceptionPkts
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupMultiTargetRateShaperGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_BOOL_PTR_multiTargetRateShaperEnablePtr,
    &OUT_GT_U32_PTR_windowSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupDropCntGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_dropPktsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_routerBridgedExceptionPktsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllPortGroupSkippedEntriesCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_skipCounterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortRoutingEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_IP_UNICAST_MULTICAST_ENT_ucMcEnable,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_GT_BOOL_enableRouting
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpPortRoutingEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_IP_UNICAST_MULTICAST_ENT_ucMcEnable,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &OUT_GT_BOOL_PTR_enableRoutingPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterPortGlobalMacSaIndexSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_routerMacSaIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterPortGlobalMacSaIndexGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_routerMacSaIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpUcRpfModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vid,
    &IN_CPSS_DXCH_IP_URPF_MODE_ENT_uRpfMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpUcRpfModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vid,
    &OUT_CPSS_DXCH_IP_URPF_MODE_ENT_PTR_uRpfModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterVlanMacSaLsbSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlan,
    &IN_GT_U8_saMac
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterVlanMacSaLsbGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlan,
    &OUT_GT_U8_PTR_saMacPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpVlanMrstBitmapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U64_mrstBmp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpVlanMrstBitmapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_GT_U64_PTR_mrstBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterPortVlanMacSaLsbSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanPortId,
    &IN_GT_U8_saMac
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterPortVlanMacSaLsbGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanPortId,
    &OUT_GT_U8_PTR_saMacPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpUcRouteEntriesWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_baseRouteEntryIndex,
    &IN_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC_PTR_routeEntriesArray,
    &IN_GT_U32_numOfRouteEntries
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpUcRouteEntriesRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_baseRouteEntryIndex,
    &INOUT_CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC_PTR_routeEntriesArray,
    &IN_GT_U32_numOfRouteEntries
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ctrlMultiTargetTCQueue,
    &IN_GT_U32_failRpfMultiTargetTCQueue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMultiTargetQueueFullDropCntSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_dropPkts
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpEcmpEntryWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ecmpEntryIndex,
    &IN_CPSS_DXCH_IP_ECMP_ENTRY_STC_PTR_ecmpEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpEcmpEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_ecmpEntryIndex,
    &OUT_CPSS_DXCH_IP_ECMP_ENTRY_STC_PTR_ecmpEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLttWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_lttTtiRow,
    &IN_GT_U32_lttTtiColumn,
    &IN_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_lttEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpLttRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_lttTtiRow,
    &IN_GT_U32_lttTtiColumn,
    &OUT_CPSS_DXCH_IP_LTT_ENTRY_STC_PTR_lttEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllMultiTargetShaperConfigurationSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_maxBucketSize,
    &INOUT_GT_U32_PTR_maxRatePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpSetMllCntInterface_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mllCntSet,
    &IN_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC_PTR_interfaceCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllCntSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mllCntSet,
    &IN_GT_U32_mllOutMCPkts
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMLLPairRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mllPairEntryIndex,
    &IN_CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT_mllPairReadForm,
    &OUT_CPSS_DXCH_IP_MLL_PAIR_STC_PTR_mllPairEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMLLPairWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mllPairEntryIndex,
    &IN_CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT_mllPairWriteForm,
    &IN_CPSS_DXCH_IP_MLL_PAIR_STC_PTR_mllPairEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMLLLastBitWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mllPairEntryIndex,
    &IN_GT_BOOL_lastBit,
    &IN_CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT_mllEntryPart
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllMultiTargetShaperMtuSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mtu
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMtuProfileSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mtuProfileIndex,
    &IN_GT_U32_mtu
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMtuProfileGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mtuProfileIndex,
    &OUT_GT_U32_PTR_mtuPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMultiTargetTCQueueSchedModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_multiTargetTcQueue,
    &IN_CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT_schedulingMode,
    &IN_GT_U32_queueWeight
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMultiTargetTCQueueSchedModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_multiTargetTcQueue,
    &OUT_CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT_PTR_schedulingModePtr,
    &OUT_GT_U32_PTR_queueWeightPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpNatEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_natIndex,
    &IN_CPSS_IP_NAT_TYPE_ENT_natType,
    &IN_CPSS_DXCH_IP_NAT_ENTRY_UNT_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpNatEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_natIndex,
    &OUT_CPSS_IP_NAT_TYPE_ENT_PTR_natTypePtr,
    &OUT_CPSS_DXCH_IP_NAT_ENTRY_UNT_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpQosProfileToMultiTargetTCQueueMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_qosProfile,
    &IN_GT_U32_multiTargetTCQueue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpQosProfileToRouteEntryMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_qosProfile,
    &IN_GT_U32_routeEntryOffset
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpQosProfileToMultiTargetTCQueueMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_qosProfile,
    &OUT_GT_U32_PTR_multiTargetTCQueuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpQosProfileToRouteEntryMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_qosProfile,
    &OUT_GT_U32_PTR_routeEntryOffsetPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMcRouteEntriesWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routeEntryIndex,
    &IN_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC_PTR_routeEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMcRouteEntriesRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routeEntryIndex,
    &OUT_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC_PTR_routeEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterArpAddrWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerArpIndex,
    &IN_GT_ETHERADDR_PTR_arpMacAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterArpAddrRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerArpIndex,
    &OUT_GT_ETHERADDR_PTR_arpMacAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterBridgedPacketsExceptionCntSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerBridgedExceptionPkts
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterGlobalMacSaSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerMacSaIndex,
    &IN_GT_ETHERADDR_PTR_macSaAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterGlobalMacSaGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerMacSaIndex,
    &OUT_GT_ETHERADDR_PTR_macSaAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterNextHopTableAgeBitsEntryWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerNextHopTableAgeBitsEntryIndex,
    &IN_GT_U32_routerNextHopTableAgeBitsEntry
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterNextHopTableAgeBitsEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerNextHopTableAgeBitsEntryIndex,
    &OUT_GT_U32_PTR_routerNextHopTableAgeBitsEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6PrefixInvalidate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTtiTcamRow
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6PrefixSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTtiTcamRow,
    &IN_CPSS_DXCH_IPV6_PREFIX_STC_PTR_prefixPtr,
    &IN_CPSS_DXCH_IPV6_PREFIX_STC_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv4PrefixInvalidate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTtiTcamRow,
    &IN_GT_U32_routerTtiTcamColumn
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv4PrefixSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTtiTcamRow,
    &IN_GT_U32_routerTtiTcamColumn,
    &IN_CPSS_DXCH_IPV4_PREFIX_STC_PTR_prefixPtr,
    &IN_CPSS_DXCH_IPV4_PREFIX_STC_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv4PrefixGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTtiTcamRow,
    &IN_GT_U32_routerTtiTcamColumn,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_CPSS_DXCH_IPV4_PREFIX_STC_PTR_prefixPtr,
    &OUT_CPSS_DXCH_IPV4_PREFIX_STC_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6PrefixGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTtiTcamRow,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_CPSS_DXCH_IPV6_PREFIX_STC_PTR_prefixPtr,
    &OUT_CPSS_DXCH_IPV6_PREFIX_STC_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpEcmpHashSeedValueSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_seed
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpEcmpHashNumBitsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_startBit,
    &IN_GT_U32_numOfBits
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpTcDpToMultiTargetTcQueueMapSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_tc,
    &IN_CPSS_DP_LEVEL_ENT_dp,
    &IN_GT_U32_multiTargetTCQueue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpTcDpToMultiTargetTcQueueMapGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_tc,
    &IN_CPSS_DP_LEVEL_ENT_dp,
    &OUT_GT_U32_PTR_multiTargetTCQueuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllMultiTargetShaperIntervalConfigurationSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_tokenBucketIntervalSlowUpdateRatio,
    &IN_GT_U32_tokenBucketUpdateInterval,
    &IN_CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT_tokenBucketIntervalUpdateRatio
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpGetDropCntMode_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_IP_DROP_CNT_MODE_ENT_PTR_dropCntModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterMacSaLsbModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_MAC_SA_LSB_MODE_ENT_PTR_saLsbModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpArpBcModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_arpBcModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllMultiTargetShaperTokenBucketModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT_PTR_tokenBucketModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRoutingEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_enableRoutingPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllBridgeEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_mllBridgeEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMultiTargetRateShaperGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_multiTargetRateShaperEnablePtr,
    &OUT_GT_U32_PTR_windowSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpUcRouteAgingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_refreshEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMultiTargetUcSchedModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_ucSPEnablePtr,
    &OUT_GT_U32_PTR_ucWeightPtr,
    &OUT_GT_U32_PTR_mcWeightPtr,
    &OUT_CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT_PTR_schedMtuPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpv6AddrPrefixScopeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_IPV6ADDR_PTR_prefixPtr,
    &OUT_GT_U32_PTR_prefixLenPtr,
    &OUT_CPSS_IPV6_PREFIX_SCOPE_ENT_PTR_addressScopePtr,
    &IN_GT_U32_prefixScopeIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_ctrlMultiTargetTCQueuePtr,
    &OUT_GT_U32_PTR_failRpfMultiTargetTCQueuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMultiTargetQueueFullDropCntGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_dropPktsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllMultiTargetShaperConfigurationGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_maxBucketSizePtr,
    &OUT_GT_U32_PTR_maxRatePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllMultiTargetShaperMtuGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_mtuPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpNatDroppedPacketsCntGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_natDropPktsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpRouterBridgedPacketsExceptionCntGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_routerBridgedExceptionPktsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpEcmpHashSeedValueGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_seedPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllSkippedEntriesCountersGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_skipCounterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpEcmpHashNumBitsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_startBitPtr,
    &OUT_GT_U32_PTR_numOfBitsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpMllMultiTargetShaperIntervalConfigurationGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_tokenBucketIntervalSlowUpdateRatioPtr,
    &OUT_GT_U32_PTR_tokenBucketUpdateIntervalPtr,
    &OUT_CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT_PTR_tokenBucketIntervalUpdateRatioPtr
};


/********* lib API DB *********/

extern void cpssDxChIpUcRouteEntriesWrite_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpUcRouteEntriesWrite_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpNatEntrySet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpNatEntryGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChIpLogLibDb[] = {
    {"cpssDxChIpv4PrefixSet", 5, cpssDxChIpv4PrefixSet_PARAMS, NULL},
    {"cpssDxChIpv4PrefixGet", 6, cpssDxChIpv4PrefixGet_PARAMS, NULL},
    {"cpssDxChIpv4PrefixInvalidate", 3, cpssDxChIpv4PrefixInvalidate_PARAMS, NULL},
    {"cpssDxChIpv6PrefixSet", 4, cpssDxChIpv6PrefixSet_PARAMS, NULL},
    {"cpssDxChIpv6PrefixGet", 5, cpssDxChIpv6PrefixGet_PARAMS, NULL},
    {"cpssDxChIpv6PrefixInvalidate", 2, cpssDxChIpv6PrefixInvalidate_PARAMS, NULL},
    {"cpssDxChIpLttWrite", 4, cpssDxChIpLttWrite_PARAMS, NULL},
    {"cpssDxChIpLttRead", 4, cpssDxChIpLttRead_PARAMS, NULL},
    {"cpssDxChIpUcRouteEntriesWrite", 4, cpssDxChIpUcRouteEntriesWrite_PARAMS, cpssDxChIpUcRouteEntriesWrite_preLogic},
    {"cpssDxChIpUcRouteEntriesRead", 4, cpssDxChIpUcRouteEntriesRead_PARAMS, cpssDxChIpUcRouteEntriesWrite_preLogic},
    {"cpssDxChIpMcRouteEntriesWrite", 3, cpssDxChIpMcRouteEntriesWrite_PARAMS, NULL},
    {"cpssDxChIpMcRouteEntriesRead", 3, cpssDxChIpMcRouteEntriesRead_PARAMS, NULL},
    {"cpssDxChIpRouterNextHopTableAgeBitsEntryWrite", 3, cpssDxChIpRouterNextHopTableAgeBitsEntryWrite_PARAMS, NULL},
    {"cpssDxChIpRouterNextHopTableAgeBitsEntryRead", 3, cpssDxChIpRouterNextHopTableAgeBitsEntryRead_PARAMS, NULL},
    {"cpssDxChIpMLLPairWrite", 4, cpssDxChIpMLLPairWrite_PARAMS, NULL},
    {"cpssDxChIpMLLPairRead", 4, cpssDxChIpMLLPairRead_PARAMS, NULL},
    {"cpssDxChIpMLLLastBitWrite", 4, cpssDxChIpMLLLastBitWrite_PARAMS, NULL},
    {"cpssDxChIpRouterArpAddrWrite", 3, cpssDxChIpRouterArpAddrWrite_PARAMS, NULL},
    {"cpssDxChIpRouterArpAddrRead", 3, cpssDxChIpRouterArpAddrRead_PARAMS, NULL},
    {"cpssDxChIpEcmpEntryWrite", 3, cpssDxChIpEcmpEntryWrite_PARAMS, NULL},
    {"cpssDxChIpEcmpEntryRead", 3, cpssDxChIpEcmpEntryRead_PARAMS, NULL},
    {"cpssDxChIpEcmpHashNumBitsSet", 3, cpssDxChIpEcmpHashNumBitsSet_PARAMS, NULL},
    {"cpssDxChIpEcmpHashNumBitsGet", 3, cpssDxChIpEcmpHashNumBitsGet_PARAMS, NULL},
    {"cpssDxChIpEcmpHashSeedValueSet", 2, cpssDxChIpEcmpHashSeedValueSet_PARAMS, NULL},
    {"cpssDxChIpEcmpHashSeedValueGet", 2, cpssDxChIpEcmpHashSeedValueGet_PARAMS, NULL},
    {"cpssDxChIpSpecialRouterTriggerEnable", 3, cpssDxChIpSpecialRouterTriggerEnable_PARAMS, NULL},
    {"cpssDxChIpSpecialRouterTriggerEnableGet", 3, cpssDxChIpSpecialRouterTriggerEnableGet_PARAMS, NULL},
    {"cpssDxChIpExceptionCommandSet", 4, cpssDxChIpExceptionCommandSet_PARAMS, NULL},
    {"cpssDxChIpExceptionCommandGet", 4, cpssDxChIpExceptionCommandGet_PARAMS, NULL},
    {"cpssDxChIpHeaderErrorMaskSet", 5, cpssDxChIpHeaderErrorMaskSet_PARAMS, NULL},
    {"cpssDxChIpHeaderErrorMaskGet", 5, cpssDxChIpHeaderErrorMaskGet_PARAMS, NULL},
    {"cpssDxChIpUcRouteAgingModeSet", 2, cpssDxChIpUcRouteAgingModeSet_PARAMS, NULL},
    {"cpssDxChIpUcRouteAgingModeGet", 2, cpssDxChIpUcRouteAgingModeGet_PARAMS, NULL},
    {"cpssDxChIpRouterSourceIdSet", 4, cpssDxChIpRouterSourceIdSet_PARAMS, NULL},
    {"cpssDxChIpRouterSourceIdGet", 4, cpssDxChIpRouterSourceIdGet_PARAMS, NULL},
    {"cpssDxChIpPortGroupRouterSourceIdSet", 5, cpssDxChIpPortGroupRouterSourceIdSet_PARAMS, NULL},
    {"cpssDxChIpPortGroupRouterSourceIdGet", 5, cpssDxChIpPortGroupRouterSourceIdGet_PARAMS, NULL},
    {"cpssDxChIpRouterSourceIdOverrideEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChIpRouterSourceIdOverrideEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet", 3, cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet_PARAMS, NULL},
    {"cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueGet", 3, cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueGet_PARAMS, NULL},
    {"cpssDxChIpQosProfileToMultiTargetTCQueueMapSet", 3, cpssDxChIpQosProfileToMultiTargetTCQueueMapSet_PARAMS, NULL},
    {"cpssDxChIpQosProfileToMultiTargetTCQueueMapGet", 3, cpssDxChIpQosProfileToMultiTargetTCQueueMapGet_PARAMS, NULL},
    {"cpssDxChIpTcDpToMultiTargetTcQueueMapSet", 4, cpssDxChIpTcDpToMultiTargetTcQueueMapSet_PARAMS, NULL},
    {"cpssDxChIpTcDpToMultiTargetTcQueueMapGet", 4, cpssDxChIpTcDpToMultiTargetTcQueueMapGet_PARAMS, NULL},
    {"cpssDxChIpMultiTargetQueueFullDropCntGet", 2, cpssDxChIpMultiTargetQueueFullDropCntGet_PARAMS, NULL},
    {"cpssDxChIpMultiTargetQueueFullDropCntSet", 2, cpssDxChIpMultiTargetQueueFullDropCntSet_PARAMS, NULL},
    {"cpssDxChIpMultiTargetTCQueueSchedModeSet", 4, cpssDxChIpMultiTargetTCQueueSchedModeSet_PARAMS, NULL},
    {"cpssDxChIpMultiTargetTCQueueSchedModeGet", 4, cpssDxChIpMultiTargetTCQueueSchedModeGet_PARAMS, NULL},
    {"cpssDxChIpBridgeServiceEnable", 4, cpssDxChIpBridgeServiceEnable_PARAMS, NULL},
    {"cpssDxChIpBridgeServiceEnableGet", 4, cpssDxChIpBridgeServiceEnableGet_PARAMS, NULL},
    {"cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet", 3, cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet_PARAMS, NULL},
    {"cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet", 3, cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet_PARAMS, NULL},
    {"cpssDxChIpRouterBridgedPacketsExceptionCntGet", 2, cpssDxChIpRouterBridgedPacketsExceptionCntGet_PARAMS, NULL},
    {"cpssDxChIpRouterBridgedPacketsExceptionCntSet", 2, cpssDxChIpRouterBridgedPacketsExceptionCntSet_PARAMS, NULL},
    {"cpssDxChIpMllBridgeEnable", 2, cpssDxChIpMllBridgeEnable_PARAMS, NULL},
    {"cpssDxChIpMllBridgeEnableGet", 2, cpssDxChIpMllBridgeEnableGet_PARAMS, NULL},
    {"cpssDxChIpMultiTargetRateShaperSet", 3, cpssDxChIpMultiTargetRateShaperSet_PARAMS, NULL},
    {"cpssDxChIpMultiTargetRateShaperGet", 3, cpssDxChIpMultiTargetRateShaperGet_PARAMS, NULL},
    {"cpssDxChIpMultiTargetUcSchedModeSet", 5, cpssDxChIpMultiTargetUcSchedModeSet_PARAMS, NULL},
    {"cpssDxChIpMultiTargetUcSchedModeGet", 5, cpssDxChIpMultiTargetUcSchedModeGet_PARAMS, NULL},
    {"cpssDxChIpArpBcModeSet", 2, cpssDxChIpArpBcModeSet_PARAMS, NULL},
    {"cpssDxChIpArpBcModeGet", 2, cpssDxChIpArpBcModeGet_PARAMS, NULL},
    {"cpssDxChIpPortRoutingEnable", 5, cpssDxChIpPortRoutingEnable_PARAMS, NULL},
    {"cpssDxChIpPortRoutingEnableGet", 5, cpssDxChIpPortRoutingEnableGet_PARAMS, NULL},
    {"cpssDxChIpQosProfileToRouteEntryMapSet", 3, cpssDxChIpQosProfileToRouteEntryMapSet_PARAMS, NULL},
    {"cpssDxChIpQosProfileToRouteEntryMapGet", 3, cpssDxChIpQosProfileToRouteEntryMapGet_PARAMS, NULL},
    {"cpssDxChIpRoutingEnable", 2, cpssDxChIpRoutingEnable_PARAMS, NULL},
    {"cpssDxChIpRoutingEnableGet", 2, cpssDxChIpRoutingEnableGet_PARAMS, NULL},
    {"cpssDxChIpCntGet", 3, cpssDxChIpCntGet_PARAMS, NULL},
    {"cpssDxChIpCntSetModeSet", 4, cpssDxChIpCntSetModeSet_PARAMS, NULL},
    {"cpssDxChIpCntSetModeGet", 4, cpssDxChIpCntSetModeGet_PARAMS, NULL},
    {"cpssDxChIpCntSet", 3, cpssDxChIpCntSet_PARAMS, NULL},
    {"cpssDxChIpMllSkippedEntriesCountersGet", 2, cpssDxChIpMllSkippedEntriesCountersGet_PARAMS, NULL},
    {"cpssDxChIpMllPortGroupSkippedEntriesCountersGet", 3, cpssDxChIpMllPortGroupSkippedEntriesCountersGet_PARAMS, NULL},
    {"cpssDxChIpSetMllCntInterface", 3, cpssDxChIpSetMllCntInterface_PARAMS, NULL},
    {"cpssDxChIpMllCntGet", 3, prvCpssLogGenDevNumMllCntSetMllOutMCPktsPtr_PARAMS, NULL},
    {"cpssDxChIpMllCntSet", 3, cpssDxChIpMllCntSet_PARAMS, NULL},
    {"cpssDxChIpMllSilentDropCntGet", 2, prvCpssLogGenDevNumSilentDropPktsPtr_PARAMS, NULL},
    {"cpssDxChIpMllPortGroupSilentDropCntGet", 3, prvCpssLogGenDevNumPortGroupsBmpSilentDropPktsPtr_PARAMS, NULL},
    {"cpssDxChIpDropCntSet", 2, cpssDxChIpMultiTargetQueueFullDropCntSet_PARAMS, NULL},
    {"cpssDxChIpSetDropCntMode", 2, cpssDxChIpSetDropCntMode_PARAMS, NULL},
    {"cpssDxChIpGetDropCntMode", 2, cpssDxChIpGetDropCntMode_PARAMS, NULL},
    {"cpssDxChIpDropCntGet", 2, cpssDxChIpMultiTargetQueueFullDropCntGet_PARAMS, NULL},
    {"cpssDxChIpMtuProfileSet", 3, cpssDxChIpMtuProfileSet_PARAMS, NULL},
    {"cpssDxChIpMtuProfileGet", 3, cpssDxChIpMtuProfileGet_PARAMS, NULL},
    {"cpssDxChIpv6AddrPrefixScopeSet", 5, cpssDxChIpv6AddrPrefixScopeSet_PARAMS, NULL},
    {"cpssDxChIpv6AddrPrefixScopeGet", 5, cpssDxChIpv6AddrPrefixScopeGet_PARAMS, NULL},
    {"cpssDxChIpv6UcScopeCommandSet", 5, cpssDxChIpv6UcScopeCommandSet_PARAMS, NULL},
    {"cpssDxChIpv6UcScopeCommandGet", 5, cpssDxChIpv6UcScopeCommandGet_PARAMS, NULL},
    {"cpssDxChIpv6McScopeCommandSet", 6, cpssDxChIpv6McScopeCommandSet_PARAMS, NULL},
    {"cpssDxChIpv6McScopeCommandGet", 6, cpssDxChIpv6McScopeCommandGet_PARAMS, NULL},
    {"cpssDxChIpRouterMacSaBaseSet", 2, prvCpssLogGenDevNumMacPtr_PARAMS, NULL},
    {"cpssDxChIpRouterMacSaBaseGet", 2, prvCpssLogGenDevNumMacPtr2_PARAMS, NULL},
    {"cpssDxChIpRouterMacSaLsbModeSet", 2, cpssDxChIpRouterMacSaLsbModeSet_PARAMS, NULL},
    {"cpssDxChIpRouterMacSaLsbModeGet", 2, cpssDxChIpRouterMacSaLsbModeGet_PARAMS, NULL},
    {"cpssDxChIpPortRouterMacSaLsbModeSet", 3, cpssDxChIpPortRouterMacSaLsbModeSet_PARAMS, NULL},
    {"cpssDxChIpPortRouterMacSaLsbModeGet", 3, cpssDxChIpPortRouterMacSaLsbModeGet_PARAMS, NULL},
    {"cpssDxChIpRouterPortVlanMacSaLsbSet", 3, cpssDxChIpRouterPortVlanMacSaLsbSet_PARAMS, NULL},
    {"cpssDxChIpRouterPortVlanMacSaLsbGet", 3, cpssDxChIpRouterPortVlanMacSaLsbGet_PARAMS, NULL},
    {"cpssDxChIpRouterPortMacSaLsbSet", 3, cpssDxChIpRouterPortMacSaLsbSet_PARAMS, NULL},
    {"cpssDxChIpRouterPortMacSaLsbGet", 3, cpssDxChIpRouterPortMacSaLsbGet_PARAMS, NULL},
    {"cpssDxChIpRouterVlanMacSaLsbSet", 3, cpssDxChIpRouterVlanMacSaLsbSet_PARAMS, NULL},
    {"cpssDxChIpRouterVlanMacSaLsbGet", 3, cpssDxChIpRouterVlanMacSaLsbGet_PARAMS, NULL},
    {"cpssDxChIpRouterGlobalMacSaSet", 3, cpssDxChIpRouterGlobalMacSaSet_PARAMS, NULL},
    {"cpssDxChIpRouterGlobalMacSaGet", 3, cpssDxChIpRouterGlobalMacSaGet_PARAMS, NULL},
    {"cpssDxChIpRouterPortGlobalMacSaIndexSet", 3, cpssDxChIpRouterPortGlobalMacSaIndexSet_PARAMS, NULL},
    {"cpssDxChIpRouterPortGlobalMacSaIndexGet", 3, cpssDxChIpRouterPortGlobalMacSaIndexGet_PARAMS, NULL},
    {"cpssDxChIpRouterMacSaModifyEnable", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChIpRouterMacSaModifyEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChIpEcmpUcRpfCheckEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChIpEcmpUcRpfCheckEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChIpPortGroupCntSet", 4, cpssDxChIpPortGroupCntSet_PARAMS, NULL},
    {"cpssDxChIpPortGroupCntGet", 4, cpssDxChIpPortGroupCntGet_PARAMS, NULL},
    {"cpssDxChIpPortGroupDropCntSet", 3, cpssDxChIpPortGroupDropCntSet_PARAMS, NULL},
    {"cpssDxChIpPortGroupDropCntGet", 3, cpssDxChIpPortGroupDropCntGet_PARAMS, NULL},
    {"cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet", 3, cpssDxChIpPortGroupDropCntGet_PARAMS, NULL},
    {"cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet", 3, cpssDxChIpPortGroupDropCntSet_PARAMS, NULL},
    {"cpssDxChIpPortGroupMllCntGet", 4, prvCpssLogGenDevNumPortGroupsBmpMllCntSetMllOutMCPktsPtr_PARAMS, NULL},
    {"cpssDxChIpPortGroupMllCntSet", 4, cpssDxChIpPortGroupMllCntSet_PARAMS, NULL},
    {"cpssDxChIpUcRpfModeSet", 3, cpssDxChIpUcRpfModeSet_PARAMS, NULL},
    {"cpssDxChIpUcRpfModeGet", 3, cpssDxChIpUcRpfModeGet_PARAMS, NULL},
    {"cpssDxChIpPortSipSaEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChIpPortSipSaEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChIpPortGroupMultiTargetRateShaperSet", 4, cpssDxChIpPortGroupMultiTargetRateShaperSet_PARAMS, NULL},
    {"cpssDxChIpPortGroupMultiTargetRateShaperGet", 4, cpssDxChIpPortGroupMultiTargetRateShaperGet_PARAMS, NULL},
    {"cpssDxChIpTcamEccDaemonEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChIpTcamEccDaemonEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChIpUcRoutingVid1AssignEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChIpUcRoutingVid1AssignEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperBaselineSet", 2, prvCpssLogGenDevNumBaseline_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperBaselineGet", 2, prvCpssLogGenDevNumBaselinePtr_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperMtuSet", 2, cpssDxChIpMllMultiTargetShaperMtuSet_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperMtuGet", 2, cpssDxChIpMllMultiTargetShaperMtuGet_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperTokenBucketModeSet", 2, cpssDxChIpMllMultiTargetShaperTokenBucketModeSet_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperTokenBucketModeGet", 2, cpssDxChIpMllMultiTargetShaperTokenBucketModeGet_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperConfigurationSet", 3, cpssDxChIpMllMultiTargetShaperConfigurationSet_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperConfigurationGet", 3, cpssDxChIpMllMultiTargetShaperConfigurationGet_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperIntervalConfigurationSet", 4, cpssDxChIpMllMultiTargetShaperIntervalConfigurationSet_PARAMS, NULL},
    {"cpssDxChIpMllMultiTargetShaperIntervalConfigurationGet", 4, cpssDxChIpMllMultiTargetShaperIntervalConfigurationGet_PARAMS, NULL},
    {"cpssDxChIpPbrBypassRouterTriggerRequirementsEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChIpPbrBypassRouterTriggerRequirementsEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChIpVlanMrstBitmapSet", 3, cpssDxChIpVlanMrstBitmapSet_PARAMS, NULL},
    {"cpssDxChIpVlanMrstBitmapGet", 3, cpssDxChIpVlanMrstBitmapGet_PARAMS, NULL},
    {"cpssDxChIpNatEntrySet", 4, cpssDxChIpNatEntrySet_PARAMS, cpssDxChIpNatEntrySet_preLogic},
    {"cpssDxChIpNatEntryGet", 4, cpssDxChIpNatEntryGet_PARAMS, cpssDxChIpNatEntryGet_preLogic},
    {"cpssDxChIpNatDroppedPacketsCntGet", 2, cpssDxChIpNatDroppedPacketsCntGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_IP(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChIpLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChIpLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

