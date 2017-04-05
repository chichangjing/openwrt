/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTunnelLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnelTypes.h>
#include <cpss/dxCh/dxChxGen/tunnel/private/prvCpssDxChTunnelLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>
#include <cpss/generic/tunnel/private/prvCpssGenTunnelLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E",
    "CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E",
    "CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E",
    "CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E",
    "CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_QOS_MODE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_QOS_KEEP_PREVIOUS_E",
    "CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_E",
    "CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_DSCP_E",
    "CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E",
    "CPSS_DXCH_TUNNEL_QOS_TRUST_MPLS_EXP_E",
    "CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_QOS_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_CONST_0_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_CONST_1_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_0_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_1_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_2_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_3_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_0_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_1_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_2_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_SHIFT_HASH_0_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_SHIFT_HASH_1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_4B_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_8B_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_12B_E",
    "CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_16B_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_FROM_IP_GEN_TS_ENTRY_E",
    "CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_FROM_PACKET_HASH_VALUE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_TO_ZERO_E",
    "CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_TO_PACKET_HASH_VALUE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_IP_E",
    "CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E",
    "CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_UDP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_E",
    "CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_AND_VLAN_ENTRY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_TS_ENTRY_E",
    "CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_VLAN_ENTRY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_UC_E",
    "CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_MC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E",
    "CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT[]  =
{
    "CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E",
    "CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E",
    "CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, templateDataSize, CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, templateDataBitsCfg, CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_MAX_SIZE_CNS, CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hashShiftLeftBitsNumber);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, udpSrcPortMode, CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_TERM_ACTION_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, command, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, passengerPacketType, CPSS_TUNNEL_PASSENGER_PACKET_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, egressInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTunnelStart);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tunnelStartIdx);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, vlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, nestedVlanEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, copyTtlFromTunnelHeader);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosMode, CPSS_DXCH_TUNNEL_QOS_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfile);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, defaultUP);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyUP, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, remapDSCP);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyDSCP, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mirrorToIngressAnalyzerEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, policerEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, policerIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, matchCounterEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, matchCounterIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tagEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dscpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dscp);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macDa);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, dontFragmentFlag);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ttl);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, autoTunnel);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, autoTunnelOffset);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, destIp);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, srcIp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cfi);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, retainCRC);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipHeaderProtocol, CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, profileIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, greProtocolForEthernet);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, greFlagsAndVersion);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, udpDstPort);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, udpSrcPort);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tagEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dscpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dscp);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macDa);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ttl);
    PRV_CPSS_LOG_STC_IPV6_MAC(valPtr, destIp);
    PRV_CPSS_LOG_STC_IPV6_MAC(valPtr, srcIp);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, retainCRC);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipHeaderProtocol, CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, profileIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, greProtocolForEthernet);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, udpDstPort);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, greFlagsAndVersion);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, udpSrcPort);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, flowLabelMode, CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tagEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macDa);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, retainCrc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iSid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iUp);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, iUpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iDp);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, iDpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iTagReserved);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, iSidAssignMode, CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, bDaAssignMode, CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tagEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macDa);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numLabels);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ttl);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ttlMode, CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, label1);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, exp1MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, label2);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, exp2MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp2);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, label3);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, exp3MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp3);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, retainCRC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, setSBit);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cfi);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, controlWordEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, controlWordIndex);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, mplsEthertypeSelect, CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, pushEliAndElAfterLabel1);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, pushEliAndElAfterLabel2);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, pushEliAndElAfterLabel3);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_FIELD_VAL_PTR_MAC(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *valPtr, CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tagEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, nextHopMacDa);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, trillHeader, CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcPortTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcIsTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcDev);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macDa);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, srcIp);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, destIp);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcPortTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcIsTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcDev);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macDa);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, label1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sBit1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, label2);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sBit2);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp2);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, version);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mBit);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, opLength);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hopCount);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, eRbid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iRbid);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TUNNEL_START_CONFIG_UNT*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TUNNEL_TERM_ACTION_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR_configPtr = {
     "configPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_START_CONFIG_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT_tagMode = {
     "tagMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR_profileDataPtr = {
     "profileDataPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR_actionPtr = {
     "actionPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_TERM_ACTION_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configMaskPtr = {
     "configMaskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configPtr = {
     "configPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT_exceptionType = {
     "exceptionType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TUNNEL_TYPE_ENT_tunnelType = {
     "tunnelType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_TUNNEL_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_additionToLength = {
     "additionToLength", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_label = {
     "label", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerArpTunnelStartLineIndex = {
     "routerArpTunnelStartLineIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_routerTunnelTermTcamIndex = {
     "routerTunnelTermTcamIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tsExtension = {
     "tsExtension", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ttl = {
     "ttl", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vlanServiceId = {
     "vlanServiceId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_ttl = {
     "ttl", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR_configPtr = {
     "configPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_START_CONFIG_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT_PTR_tagModePtr = {
     "tagModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR_profileDataPtr = {
     "profileDataPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR_actionPtr = {
     "actionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_TERM_ACTION_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configMaskPtr = {
     "configMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configPtr = {
     "configPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_TUNNEL_TYPE_ENT_PTR_tunnelTypePtr = {
     "tunnelTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_TUNNEL_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_additionToLengthPtr = {
     "additionToLengthPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_labelPtr = {
     "labelPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tsExtensionPtr = {
     "tsExtensionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ttlPtr = {
     "ttlPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_vlanServiceIdPtr = {
     "vlanServiceIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_ttlPtr = {
     "ttlPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChEthernetOverMplsTunnelStartTagModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT_tagMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelTermExceptionCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT_exceptionType,
    &IN_CPSS_PACKET_CMD_ENT_command
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelCtrlTtExceptionCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT_exceptionType,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartPortGroupGenProfileTableEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_profileIndex,
    &IN_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR_profileDataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartPortGroupGenProfileTableEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_profileIndex,
    &OUT_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR_profileDataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartPortGroupEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_routerArpTunnelStartLineIndex,
    &IN_CPSS_TUNNEL_TYPE_ENT_tunnelType,
    &IN_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR_configPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartPortGroupEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_routerArpTunnelStartLineIndex,
    &OUT_CPSS_TUNNEL_TYPE_ENT_PTR_tunnelTypePtr,
    &OUT_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR_configPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartMplsPwLabelExpSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_exp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartMplsPwLabelSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_label
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartEntryExtensionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_tsExtension
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartMplsPwLabelTtlSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_ttl
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartMplsPwLabelExpGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_expPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartMplsPwLabelGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_labelPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartEntryExtensionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_tsExtensionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartMplsPwLabelTtlGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_ttlPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartEgessVlanTableServiceIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U32_vlanServiceId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartEgessVlanTableServiceIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_GT_U32_PTR_vlanServiceIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_additionToLength
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartGenProfileTableEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &IN_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR_profileDataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartGenProfileTableEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &OUT_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC_PTR_profileDataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerArpTunnelStartLineIndex,
    &IN_CPSS_TUNNEL_TYPE_ENT_tunnelType,
    &IN_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR_configPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerArpTunnelStartLineIndex,
    &OUT_CPSS_TUNNEL_TYPE_ENT_PTR_tunnelTypePtr,
    &OUT_CPSS_DXCH_TUNNEL_START_CONFIG_UNT_PTR_configPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelTermEntryInvalidate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTunnelTermTcamIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelTermEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTunnelTermTcamIndex,
    &IN_CPSS_TUNNEL_TYPE_ENT_tunnelType,
    &IN_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configPtr,
    &IN_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configMaskPtr,
    &IN_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR_actionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelTermEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_routerTunnelTermTcamIndex,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_CPSS_TUNNEL_TYPE_ENT_PTR_tunnelTypePtr,
    &OUT_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configPtr,
    &OUT_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT_PTR_configMaskPtr,
    &OUT_CPSS_DXCH_TUNNEL_TERM_ACTION_STC_PTR_actionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartMplsFlowLabelTtlSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_ttl
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChEthernetOverMplsTunnelStartTagModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT_PTR_tagModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_additionToLengthPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTunnelStartMplsFlowLabelTtlGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U8_PTR_ttlPtr
};


/********* lib API DB *********/

extern void cpssDxChTunnelStartEntrySet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTunnelStartEntryGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTunnelStartPortGroupEntrySet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTunnelStartPortGroupEntryGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTunnelStartEntrySet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTunnelTermEntryGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChTunnelLogLibDb[] = {
    {"cpssDxChTunnelStartEntrySet", 4, cpssDxChTunnelStartEntrySet_PARAMS, cpssDxChTunnelStartEntrySet_preLogic},
    {"cpssDxChTunnelStartPortGroupGenProfileTableEntrySet", 4, cpssDxChTunnelStartPortGroupGenProfileTableEntrySet_PARAMS, NULL},
    {"cpssDxChTunnelStartGenProfileTableEntrySet", 3, cpssDxChTunnelStartGenProfileTableEntrySet_PARAMS, NULL},
    {"cpssDxChTunnelStartPortGroupGenProfileTableEntryGet", 4, cpssDxChTunnelStartPortGroupGenProfileTableEntryGet_PARAMS, NULL},
    {"cpssDxChTunnelStartGenProfileTableEntryGet", 3, cpssDxChTunnelStartGenProfileTableEntryGet_PARAMS, NULL},
    {"cpssDxChTunnelStartEntryGet", 4, cpssDxChTunnelStartEntryGet_PARAMS, cpssDxChTunnelStartEntryGet_preLogic},
    {"cpssDxChTunnelStartPortGroupEntrySet", 5, cpssDxChTunnelStartPortGroupEntrySet_PARAMS, cpssDxChTunnelStartPortGroupEntrySet_preLogic},
    {"cpssDxChTunnelStartPortGroupEntryGet", 5, cpssDxChTunnelStartPortGroupEntryGet_PARAMS, cpssDxChTunnelStartPortGroupEntryGet_preLogic},
    {"cpssDxChIpv4TunnelTermPortSet", 3, prvCpssLogGenDevNumPortEnable_PARAMS, NULL},
    {"cpssDxChIpv4TunnelTermPortGet", 3, prvCpssLogGenDevNumPortEnablePtr_PARAMS, NULL},
    {"cpssDxChMplsTunnelTermPortSet", 3, prvCpssLogGenDevNumPortEnable_PARAMS, NULL},
    {"cpssDxChMplsTunnelTermPortGet", 3, prvCpssLogGenDevNumPortEnablePtr_PARAMS, NULL},
    {"cpssDxChTunnelTermEntrySet", 6, cpssDxChTunnelTermEntrySet_PARAMS, cpssDxChTunnelStartEntrySet_preLogic},
    {"cpssDxChTunnelTermEntryGet", 7, cpssDxChTunnelTermEntryGet_PARAMS, cpssDxChTunnelTermEntryGet_preLogic},
    {"cpssDxChTunnelTermEntryInvalidate", 2, cpssDxChTunnelTermEntryInvalidate_PARAMS, NULL},
    {"cpssDxChTunnelStartEgressFilteringSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTunnelStartEgressFilteringGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChEthernetOverMplsTunnelStartTaggingSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChEthernetOverMplsTunnelStartTaggingGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChEthernetOverMplsTunnelStartTagModeSet", 2, cpssDxChEthernetOverMplsTunnelStartTagModeSet_PARAMS, NULL},
    {"cpssDxChEthernetOverMplsTunnelStartTagModeGet", 2, cpssDxChEthernetOverMplsTunnelStartTagModeGet_PARAMS, NULL},
    {"cpssDxChTunnelStartPassengerVlanTranslationEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTunnelStartPassengerVlanTranslationEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTunnelTermExceptionCmdSet", 3, cpssDxChTunnelTermExceptionCmdSet_PARAMS, NULL},
    {"cpssDxChTunnelCtrlTtExceptionCmdGet", 3, cpssDxChTunnelCtrlTtExceptionCmdGet_PARAMS, NULL},
    {"cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet", 3, prvCpssLogGenDevNumPortEnable_PARAMS, NULL},
    {"cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet", 3, prvCpssLogGenDevNumPortEnablePtr_PARAMS, NULL},
    {"cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet", 2, cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet_PARAMS, NULL},
    {"cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet", 2, cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwLabelPushEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwLabelPushEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwLabelSet", 3, cpssDxChTunnelStartMplsPwLabelSet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwLabelGet", 3, cpssDxChTunnelStartMplsPwLabelGet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsFlowLabelEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsFlowLabelEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsFlowLabelTtlSet", 2, cpssDxChTunnelStartMplsFlowLabelTtlSet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsFlowLabelTtlGet", 2, cpssDxChTunnelStartMplsFlowLabelTtlGet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwLabelExpSet", 3, cpssDxChTunnelStartMplsPwLabelExpSet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwLabelExpGet", 3, cpssDxChTunnelStartMplsPwLabelExpGet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwLabelTtlSet", 3, cpssDxChTunnelStartMplsPwLabelTtlSet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwLabelTtlGet", 3, cpssDxChTunnelStartMplsPwLabelTtlGet_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwControlWordSet", 3, prvCpssLogGenDevNumIndexValue_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwControlWordGet", 3, prvCpssLogGenDevNumIndexValuePtr_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwETreeEnableSet", 3, prvCpssLogGenDevNumIndexEnable_PARAMS, NULL},
    {"cpssDxChTunnelStartMplsPwETreeEnableGet", 3, prvCpssLogGenDevNumIndexEnablePtr_PARAMS, NULL},
    {"cpssDxChTunnelStartHeaderTpidSelectSet", 3, prvCpssLogGenDevNumPortNumTpidEntryIndex_PARAMS, NULL},
    {"cpssDxChTunnelStartHeaderTpidSelectGet", 3, prvCpssLogGenDevNumPortNumTpidEntryIndexPtr_PARAMS, NULL},
    {"cpssDxChTunnelStartEntryExtensionSet", 3, cpssDxChTunnelStartEntryExtensionSet_PARAMS, NULL},
    {"cpssDxChTunnelStartEntryExtensionGet", 3, cpssDxChTunnelStartEntryExtensionGet_PARAMS, NULL},
    {"cpssDxChTunnelStartEgessVlanTableServiceIdSet", 3, cpssDxChTunnelStartEgessVlanTableServiceIdSet_PARAMS, NULL},
    {"cpssDxChTunnelStartEgessVlanTableServiceIdGet", 3, cpssDxChTunnelStartEgessVlanTableServiceIdGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_TUNNEL(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChTunnelLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChTunnelLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

