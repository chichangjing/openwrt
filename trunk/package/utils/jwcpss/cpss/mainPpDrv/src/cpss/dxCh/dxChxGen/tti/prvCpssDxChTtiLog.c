/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTtiLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPclLog.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTtiTypes.h>
#include <cpss/dxCh/dxChxGen/tti/private/prvCpssDxChTtiLog.h>
#include <cpss/dxCh/dxChxGen/tunnel/private/prvCpssDxChTunnelLog.h>
#include <cpss/generic/bridge/private/prvCpssGenBrgLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>
#include <cpss/generic/tunnel/private/prvCpssGenTunnelLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_TTI_ACTION_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_ACTION_TYPE1_ENT",
    "CPSS_DXCH_TTI_ACTION_TYPE2_ENT"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_ACTION_TYPE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_DXCH_TTI_EXCEPTION_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_EXCEPTION_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_KEY_SIZE_ENT[]  =
{
    "CPSS_DXCH_TTI_KEY_SIZE_10_B_E",
    "CPSS_DXCH_TTI_KEY_SIZE_20_B_E",
    "CPSS_DXCH_TTI_KEY_SIZE_30_B_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_KEY_SIZE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT[]  =
{
    "CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E",
    "CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_E",
    "CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_2_AND_2_E",
    "CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_1_AND_3_E",
    "CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_3_AND_1_E",
    "CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_FOUR_TCAM_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_KEY_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_KEY_IPV4_E",
    "CPSS_DXCH_TTI_KEY_MPLS_E",
    "CPSS_DXCH_TTI_KEY_ETH_E",
    "CPSS_DXCH_TTI_KEY_MIM_E",
    "CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E",
    "CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E",
    "CPSS_DXCH_TTI_KEY_UDB_MPLS_E",
    "CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E",
    "CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E",
    "CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E",
    "CPSS_DXCH_TTI_KEY_UDB_IPV6_E",
    "CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E",
    "CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E",
    "CPSS_DXCH_TTI_KEY_UDB_UDE_E",
    "CPSS_DXCH_TTI_KEY_UDB_UDE1_E",
    "CPSS_DXCH_TTI_KEY_UDB_UDE2_E",
    "CPSS_DXCH_TTI_KEY_UDB_UDE3_E",
    "CPSS_DXCH_TTI_KEY_UDB_UDE4_E",
    "CPSS_DXCH_TTI_KEY_UDB_UDE5_E",
    "CPSS_DXCH_TTI_KEY_UDB_UDE6_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_KEY_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_MAC_MODE_ENT[]  =
{
    "CPSS_DXCH_TTI_MAC_MODE_DA_E",
    "CPSS_DXCH_TTI_MAC_MODE_SA_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_MAC_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT[]  =
{
    "CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_DISABLE_E",
    "CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_ALL_E",
    "CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_SPECIFIC_GRE_E",
    "CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_GRE_E",
    "CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_SPECIFIC_UDP_PORT_E",
    "CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_UDP_PORT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_OFFSET_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_OFFSET_L2_E",
    "CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E",
    "CPSS_DXCH_TTI_OFFSET_L4_E",
    "CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E",
    "CPSS_DXCH_TTI_OFFSET_METADATA_E",
    "CPSS_DXCH_TTI_OFFSET_INVALID_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_OFFSET_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT[]  =
{
    "CPSS_DXCH_TTI_PW_CW_EXCEPTION_EXPIRY_VCCV_E",
    "CPSS_DXCH_TTI_PW_CW_EXCEPTION_NON_DATA_CONTROL_WORD_E",
    "CPSS_DXCH_TTI_PW_CW_EXCEPTION_FRAGMENTED_E",
    "CPSS_DXCH_TTI_PW_CW_EXCEPTION_SEQUENCE_ERROR_E",
    "CPSS_DXCH_TTI_PW_CW_EXCEPTION_ILLEGAL_CONTROL_WORD_E",
    "CPSS_DXCH_TTI_PW_CW_EXCEPTION_PASSENGER_PW_TAG0_NOT_FOUND_ERROR_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_RULE_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_RULE_IPV4_E",
    "CPSS_DXCH_TTI_RULE_MPLS_E",
    "CPSS_DXCH_TTI_RULE_ETH_E",
    "CPSS_DXCH_TTI_RULE_MIM_E",
    "CPSS_DXCH_TTI_RULE_UDB_10_E",
    "CPSS_DXCH_TTI_RULE_UDB_20_E",
    "CPSS_DXCH_TTI_RULE_UDB_30_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_RULE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_DONT_USE_SRC_INTERFACE_FIELDS_E",
    "CPSS_DXCH_TTI_USE_SRC_INTERFACE_FIELDS_E",
    "CPSS_DXCH_TTI_USE_SRC_INTERFACE_FIELDS_EXCLUDE_SRC_DEVICE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_MODIFY_DSCP_ENT[]  =
{
    "CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E",
    "CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E",
    "CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E",
    "CPSS_DXCH_TTI_MODIFY_DSCP_RESERVED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_MODIFY_UP_ENT[]  =
{
    "CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E",
    "CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E",
    "CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E",
    "CPSS_DXCH_TTI_MODIFY_UP_RESERVED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_MODIFY_UP_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_MPLS_CMD_ENT[]  =
{
    "CPSS_DXCH_TTI_MPLS_NOP_CMD_E",
    "CPSS_DXCH_TTI_MPLS_SWAP_CMD_E",
    "CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E",
    "CPSS_DXCH_TTI_MPLS_POP1_CMD_E",
    "CPSS_DXCH_TTI_MPLS_POP2_CMD_E",
    "CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E",
    "CPSS_DXCH_TTI_MPLS_POP3_CMD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_MPLS_CMD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_DISABLE_E",
    "CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_IP_E",
    "CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_ETH_E",
    "CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_CW_ETH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_PASSENGER_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_PASSENGER_IPV4_E",
    "CPSS_DXCH_TTI_PASSENGER_IPV6_E",
    "CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E",
    "CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E",
    "CPSS_DXCH_TTI_PASSENGER_IPV4V6_E",
    "CPSS_DXCH_TTI_PASSENGER_MPLS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_L2_E",
    "CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_IPV4_UDP_E",
    "CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_IPV6_UDP_E",
    "CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_RESERVED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_PW_TAG_MODE_ENT[]  =
{
    "CPSS_DXCH_TTI_PW_TAG_DISABLED_MODE_E",
    "CPSS_DXCH_TTI_PW_TAG_RAW_PW_MODE_E",
    "CPSS_DXCH_TTI_PW_TAG_TAGGED_PW_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_PW_TAG_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT[]  =
{
    "CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E",
    "CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E",
    "CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E",
    "CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E",
    "CPSS_DXCH_TTI_QOS_UNTRUST_E",
    "CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_NO_REDIRECT_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_VRF_ID_ASSIGN_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_TTI_LOGICAL_PORT_ASSIGN_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT[]  =
{
    "CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E",
    "CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E",
    "CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT);
char * prvCpssLogEnum_CPSS_DXCH_TTI_VLAN_COMMAND_ENT[]  =
{
    "CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E",
    "CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E",
    "CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E",
    "CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_TTI_VLAN_COMMAND_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, includeSrcInterface, CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcHwDevice);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, srcIsTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcPortTrunk);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_MAC_VLAN_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, mac);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_TRILL_ADJACENCY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, outerMacSa);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, trillMBit);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, trillEgressRbid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcHwDevice);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcIsTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcPortTrunk);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ACTION_2_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_ACTION_2_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tunnelTerminate);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ttPassengerPacketType, CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tsPassengerPacketType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ttHeaderLength);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, continueToNextTtiLookup);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, copyTtlExpFromTunnelHeader);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, mplsCommand, CPSS_DXCH_TTI_MPLS_CMD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mplsTtl);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableDecrementTtl);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, passengerParsingOfTransitMplsTunnelMode, CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, passengerParsingOfTransitNonMplsTransitTunnelEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, command, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, redirectCommand, CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, egressInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, arpPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tunnelStart);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tunnelStartPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, routerLttPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vrfId);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, sourceIdSetEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sourceId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tag0VlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag0VlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tag1VlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag1VlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tag0VlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, nestedVlanEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bindToPolicerMeter);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bindToPolicer);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, policerIndex);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, keepPreviousQoS);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, trustUp);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, trustDscp);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, trustExp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfile);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyTag0Up, CPSS_DXCH_TTI_MODIFY_UP_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tag1UpCommand, CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyDscp, CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag0Up);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag1Up);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, remapDSCP);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, qosUseUpAsIndexEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosMappingTableIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mplsLLspQoSProfileEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, pcl0OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, pcl0_1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, pcl1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iPclConfigIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, iPclUdbConfigTableEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, iPclUdbConfigTableIndex, CPSS_DXCH_PCL_PACKET_TYPE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mirrorToIngressAnalyzerEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mirrorToIngressAnalyzerIndex);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bindToCentralCounter);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, centralCounterIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, vntl2Echo);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bridgeBypass);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ingressPipeBypass);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, actionStop);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hashMaskIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, modifyMacSa);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, modifyMacDa);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ResetSrcPortGroupId);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, multiPortGroupTtiEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, sourceEPortAssignmentEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sourceEPort);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, flowId);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, setMacToMe);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, rxProtectionSwitchEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, rxIsProtectionPath);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, pwTagMode, CPSS_DXCH_TTI_PW_TAG_MODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, oamTimeStampEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, oamOffsetIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, oamProcessEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, oamProfile);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, oamChannelTypeToOpcodeMappingEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isPtpPacket);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ptpTriggerType, CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ptpOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, cwBasedPw);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ttlExpiryVccvEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, pwe3FlowLabelExist);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, pwCwBasedETreeEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, applyNonDataCwCommand);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, unknownSaCommandEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unknownSaCommand, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, sourceMeshIdSetEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sourceMeshId);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ACTION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_ACTION_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tunnelTerminate);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, passengerPacketType, CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, copyTtlFromTunnelHeader);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, command, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, redirectCommand, CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, egressInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tunnelStart);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tunnelStartPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, routerLookupPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vrfId);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, sourceIdSetEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sourceId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, vlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, vlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, nestedVlanEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bindToPolicer);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, policerIndex);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosTrustMode, CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfile);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyUpEnable, CPSS_DXCH_TTI_MODIFY_UP_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyDscpEnable, CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, remapDSCP);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mirrorToIngressAnalyzerEnable);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, vntl2Echo);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bridgeBypass);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, actionStop);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, activateCounter);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, counterIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ETH_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_ETH_RULE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up0);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cfi0);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isVlan1Exists);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cfi1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, etherType);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, macToMe);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dsaQosProfile);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag0TpidIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag1TpidIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, eTagGrp);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_IPV4_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_IPV4_RULE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tunneltype);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, srcIp);
    PRV_CPSS_LOG_STC_IPV4_MAC(valPtr, destIp);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isArp);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MIM_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_MIM_RULE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, bUp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, bDp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iSid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iUp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iDp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iRes1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, iRes2);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, macToMe);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, passengerPacketOuterTagExists);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, passengerPacketOuterTagVid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, passengerPacketOuterTagUp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, passengerPacketOuterTagDei);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MPLS_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_MPLS_RULE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, label0);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp0);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, label1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, label2);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, exp2);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfLabels);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, protocolAboveMPLS);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, reservedLabelExist);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, reservedLabelValue);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, channelTypeProfile);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dataAfterInnerLabel);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cwFirstNibble);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_TRILL_RULE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_TRILL_RULE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, trillMBit);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, trillEgressRbid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, trillIngressRbid);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, innerPacketMacDa);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, innerPacketTag0Exists);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, innerPacketTag0Vid);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, innerPacketFieldsAreValid);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, trillMcDescriptorInstance);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_TTI_RULE_COMMON_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_TTI_RULE_COMMON_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, pclId);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, srcIsTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcPortTrunk);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, mac);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTagged);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, dsaSrcIsTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dsaSrcPortTrunk);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dsaSrcDevice);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sourcePortGroupId);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_TTI_ACTION_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_ACTION_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_ACTION_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_ACTION_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TTI_ACTION_UNT*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TTI_ACTION_UNT_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_EXCEPTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_EXCEPTION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_DXCH_TTI_EXCEPTION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_SIZE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_KEY_SIZE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_KEY_SIZE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_SIZE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_KEY_SIZE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TTI_KEY_SIZE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_KEY_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_KEY_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_KEY_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MAC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_MAC_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_MAC_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MAC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_MAC_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TTI_MAC_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TTI_MAC_VLAN_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_OFFSET_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_OFFSET_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_OFFSET_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_OFFSET_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_TTI_OFFSET_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_RULE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_TTI_RULE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_TTI_RULE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_RULE_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TTI_RULE_UNT*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TTI_RULE_UNT_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TTI_TRILL_ADJACENCY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_ACTION_TYPE_ENT_actionType = {
     "actionType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_ACTION_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr = {
     "actionPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_ACTION_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_EXCEPTION_ENT_exceptionType = {
     "exceptionType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_EXCEPTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_KEY_SIZE_ENT_size = {
     "size", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_KEY_SIZE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT_segmentMode = {
     "segmentMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType = {
     "keyType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_KEY_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MAC_MODE_ENT_macMode = {
     "macMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_MAC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR_interfaceInfoPtr = {
     "interfaceInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_MAC_VLAN_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_valuePtr = {
     "valuePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_MAC_VLAN_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_offsetType = {
     "offsetType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_OFFSET_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT_exceptionType = {
     "exceptionType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_RULE_TYPE_ENT_ruleType = {
     "ruleType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_TTI_RULE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_RULE_UNT_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_RULE_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_RULE_UNT_PTR_patternPtr = {
     "patternPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_RULE_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_TRILL_ADJACENCY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_valuePtr = {
     "valuePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_TRILL_ADJACENCY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_ETHER_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_ETHER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IP_PROTOCOL_STACK_ENT_ipType = {
     "ipType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IP_PROTOCOL_STACK_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_RX_CPU_CODE_ENT_code = {
     "code", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NET_RX_CPU_CODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TUNNEL_ETHERTYPE_TYPE_ENT_ethertypeType = {
     "ethertypeType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_TUNNEL_ETHERTYPE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_TUNNEL_MULTICAST_TYPE_ENT_protocol = {
     "protocol", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_TUNNEL_MULTICAST_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_PTR_addressPtr = {
     "addressPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_udpPort = {
     "udpPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_cpuCodeBase = {
     "cpuCodeBase", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ethertype = {
     "ethertype", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_extensionHeaderId = {
     "extensionHeaderId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_extensionHeaderValue = {
     "extensionHeaderValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lookup = {
     "lookup", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxTrillVersion = {
     "maxTrillVersion", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_overrideBitmap = {
     "overrideBitmap", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pclId = {
     "pclId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr = {
     "actionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_ACTION_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_KEY_SIZE_ENT_PTR_sizePtr = {
     "sizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_KEY_SIZE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT_PTR_segmentModePtr = {
     "segmentModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MAC_MODE_ENT_PTR_macModePtr = {
     "macModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_MAC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR_interfaceInfoPtr = {
     "interfaceInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_MAC_VLAN_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_valuePtr = {
     "valuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_MAC_VLAN_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_PTR_offsetTypePtr = {
     "offsetTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_OFFSET_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_RULE_UNT_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_RULE_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_RULE_UNT_PTR_patternPtr = {
     "patternPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_RULE_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_TRILL_ADJACENCY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_valuePtr = {
     "valuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_TTI_TRILL_ADJACENCY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_ETHER_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_ETHER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_codePtr = {
     "codePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_NET_RX_CPU_CODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_addressPtr = {
     "addressPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_udpPortPtr = {
     "udpPortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cpuCodeBasePtr = {
     "cpuCodeBasePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ethertypePtr = {
     "ethertypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_extensionHeaderValuePtr = {
     "extensionHeaderValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxTrillVersionPtr = {
     "maxTrillVersionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_overrideBitmapPtr = {
     "overrideBitmapPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_pclIdPtr = {
     "pclIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiExceptionCpuCodeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_EXCEPTION_ENT_exceptionType,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_code
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiExceptionCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_EXCEPTION_ENT_exceptionType,
    &IN_CPSS_PACKET_CMD_ENT_command
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiExceptionCpuCodeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_EXCEPTION_ENT_exceptionType,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_codePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiExceptionCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_EXCEPTION_ENT_exceptionType,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPacketTypeKeySizeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_CPSS_DXCH_TTI_KEY_SIZE_ENT_size
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiTcamSegmentModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT_segmentMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMacModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_CPSS_DXCH_TTI_MAC_MODE_ENT_macMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPclIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_GT_U32_pclId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiUserDefinedByteSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_GT_U32_udbIndex,
    &IN_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_offsetType,
    &IN_GT_U8_offset
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiUserDefinedByteGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_GT_U32_udbIndex,
    &OUT_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_PTR_offsetTypePtr,
    &OUT_GT_U8_PTR_offsetPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPacketTypeKeySizeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &OUT_CPSS_DXCH_TTI_KEY_SIZE_ENT_PTR_sizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiTcamSegmentModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &OUT_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT_PTR_segmentModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMacModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &OUT_CPSS_DXCH_TTI_MAC_MODE_ENT_PTR_macModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPclIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &OUT_GT_U32_PTR_pclIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPwCwExceptionCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT_exceptionType,
    &IN_CPSS_PACKET_CMD_ENT_command
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPwCwExceptionCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT_exceptionType,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiIpTotalLengthDeductionValueSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_ipType,
    &IN_GT_U32_value
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiIpTotalLengthDeductionValueGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_ipType,
    &OUT_GT_U32_PTR_valuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiEthernetTypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_TUNNEL_ETHERTYPE_TYPE_ENT_ethertypeType,
    &IN_GT_U32_ethertype
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiEthernetTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_TUNNEL_ETHERTYPE_TYPE_ENT_ethertypeType,
    &OUT_GT_U32_PTR_ethertypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMplsMcTunnelTriggeringMacDaSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_ETHERADDR_PTR_addressPtr,
    &IN_GT_ETHERADDR_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupMacModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_CPSS_DXCH_TTI_MAC_MODE_ENT_macMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupUserDefinedByteSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_GT_U32_udbIndex,
    &IN_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_offsetType,
    &IN_GT_U8_offset
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupUserDefinedByteGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_GT_U32_udbIndex,
    &OUT_CPSS_DXCH_TTI_OFFSET_TYPE_ENT_PTR_offsetTypePtr,
    &OUT_GT_U8_PTR_offsetPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupMacModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &OUT_CPSS_DXCH_TTI_MAC_MODE_ENT_PTR_macModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupMacToMeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_valuePtr,
    &IN_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_maskPtr,
    &IN_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR_interfaceInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupMacToMeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_valuePtr,
    &OUT_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_maskPtr,
    &OUT_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR_interfaceInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupRuleActionUpdate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_TTI_ACTION_TYPE_ENT_actionType,
    &IN_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupRuleSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_TTI_RULE_TYPE_ENT_ruleType,
    &IN_CPSS_DXCH_TTI_RULE_UNT_PTR_patternPtr,
    &IN_CPSS_DXCH_TTI_RULE_UNT_PTR_maskPtr,
    &IN_CPSS_DXCH_TTI_ACTION_TYPE_ENT_actionType,
    &IN_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupRuleGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_TTI_RULE_TYPE_ENT_ruleType,
    &OUT_CPSS_DXCH_TTI_RULE_UNT_PTR_patternPtr,
    &OUT_CPSS_DXCH_TTI_RULE_UNT_PTR_maskPtr,
    &IN_CPSS_DXCH_TTI_ACTION_TYPE_ENT_actionType,
    &OUT_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortGroupRuleValidStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortLookupEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortLookupEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_TTI_KEY_TYPE_ENT_keyType,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortPassengerOuterIsTag0Or1Set_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_ETHER_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMcTunnelDuplicationModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_TUNNEL_MULTICAST_TYPE_ENT_protocol,
    &IN_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMcTunnelDuplicationModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_TUNNEL_MULTICAST_TYPE_ENT_protocol,
    &OUT_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortTrillOuterVid0Set_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U16_vlanId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortPassengerOuterIsTag0Or1Get_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_ETHER_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiPortTrillOuterVid0Get_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U16_PTR_vlanIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMcTunnelDuplicationUdpDestPortSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_udpPort
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiTrillCpuCodeBaseSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_cpuCodeBase
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMacToMeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_valuePtr,
    &IN_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_maskPtr,
    &IN_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR_interfaceInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiTrillAdjacencyCheckEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_valuePtr,
    &IN_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMacToMeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_valuePtr,
    &OUT_CPSS_DXCH_TTI_MAC_VLAN_STC_PTR_maskPtr,
    &OUT_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC_PTR_interfaceInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiTrillAdjacencyCheckEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &OUT_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_valuePtr,
    &OUT_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiIPv6ExtensionHeaderSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_extensionHeaderId,
    &IN_GT_U32_extensionHeaderValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiIPv6ExtensionHeaderGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_extensionHeaderId,
    &OUT_GT_U32_PTR_extensionHeaderValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiRuleActionUpdate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_TTI_ACTION_TYPE_ENT_actionType,
    &IN_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiRuleSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_TTI_RULE_TYPE_ENT_ruleType,
    &IN_CPSS_DXCH_TTI_RULE_UNT_PTR_patternPtr,
    &IN_CPSS_DXCH_TTI_RULE_UNT_PTR_maskPtr,
    &IN_CPSS_DXCH_TTI_ACTION_TYPE_ENT_actionType,
    &IN_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiRuleGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_TTI_RULE_TYPE_ENT_ruleType,
    &OUT_CPSS_DXCH_TTI_RULE_UNT_PTR_patternPtr,
    &OUT_CPSS_DXCH_TTI_RULE_UNT_PTR_maskPtr,
    &IN_CPSS_DXCH_TTI_ACTION_TYPE_ENT_actionType,
    &OUT_CPSS_DXCH_TTI_ACTION_UNT_PTR_actionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiRuleValidStatusSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_GT_BOOL_valid
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiRuleValidStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiSourceIdBitsOverrideSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_lookup,
    &IN_GT_U32_overrideBitmap
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiSourceIdBitsOverrideGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_lookup,
    &OUT_GT_U32_PTR_overrideBitmapPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiTrillMaxVersionSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_maxTrillVersion
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMplsMcTunnelTriggeringMacDaGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_ETHERADDR_PTR_addressPtr,
    &OUT_GT_ETHERADDR_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiMcTunnelDuplicationUdpDestPortGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U16_PTR_udpPortPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiTrillCpuCodeBaseGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_cpuCodeBasePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTtiTrillMaxVersionGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_maxTrillVersionPtr
};


/********* lib API DB *********/

extern void cpssDxChTtiRuleSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTtiRuleSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTtiRuleActionUpdate_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTtiPortGroupRuleSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTtiPortGroupRuleSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChTtiPortGroupRuleActionUpdate_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChTtiLogLibDb[] = {
    {"cpssDxChTtiMacToMeSet", 5, cpssDxChTtiMacToMeSet_PARAMS, NULL},
    {"cpssDxChTtiMacToMeGet", 5, cpssDxChTtiMacToMeGet_PARAMS, NULL},
    {"cpssDxChTtiPortLookupEnableSet", 4, cpssDxChTtiPortLookupEnableSet_PARAMS, NULL},
    {"cpssDxChTtiPortLookupEnableGet", 4, cpssDxChTtiPortLookupEnableGet_PARAMS, NULL},
    {"cpssDxChTtiPortIpv4OnlyTunneledEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChTtiPortIpv4OnlyTunneledEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiPortIpv4OnlyMacToMeEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChTtiPortIpv4OnlyMacToMeEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiIpv4McEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTtiIpv4McEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiPortMplsOnlyMacToMeEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChTtiPortMplsOnlyMacToMeEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiPortMimOnlyMacToMeEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChTtiPortMimOnlyMacToMeEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiRuleSet", 7, cpssDxChTtiRuleSet_PARAMS, cpssDxChTtiRuleSet_preLogic},
    {"cpssDxChTtiRuleGet", 7, cpssDxChTtiRuleGet_PARAMS, cpssDxChTtiRuleSet_preLogic},
    {"cpssDxChTtiRuleActionUpdate", 4, cpssDxChTtiRuleActionUpdate_PARAMS, cpssDxChTtiRuleActionUpdate_preLogic},
    {"cpssDxChTtiRuleValidStatusSet", 3, cpssDxChTtiRuleValidStatusSet_PARAMS, NULL},
    {"cpssDxChTtiRuleValidStatusGet", 3, cpssDxChTtiRuleValidStatusGet_PARAMS, NULL},
    {"cpssDxChTtiMacModeSet", 3, cpssDxChTtiMacModeSet_PARAMS, NULL},
    {"cpssDxChTtiMacModeGet", 3, cpssDxChTtiMacModeGet_PARAMS, NULL},
    {"cpssDxChTtiPclIdSet", 3, cpssDxChTtiPclIdSet_PARAMS, NULL},
    {"cpssDxChTtiPclIdGet", 3, cpssDxChTtiPclIdGet_PARAMS, NULL},
    {"cpssDxChTtiExceptionCmdSet", 3, cpssDxChTtiExceptionCmdSet_PARAMS, NULL},
    {"cpssDxChTtiExceptionCmdGet", 3, cpssDxChTtiExceptionCmdGet_PARAMS, NULL},
    {"cpssDxChTtiExceptionCpuCodeSet", 3, cpssDxChTtiExceptionCpuCodeSet_PARAMS, NULL},
    {"cpssDxChTtiExceptionCpuCodeGet", 3, cpssDxChTtiExceptionCpuCodeGet_PARAMS, NULL},
    {"cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiPortIpTotalLengthDeductionEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChTtiPortIpTotalLengthDeductionEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiIpTotalLengthDeductionValueSet", 3, cpssDxChTtiIpTotalLengthDeductionValueSet_PARAMS, NULL},
    {"cpssDxChTtiIpTotalLengthDeductionValueGet", 3, cpssDxChTtiIpTotalLengthDeductionValueGet_PARAMS, NULL},
    {"cpssDxChTtiEthernetTypeSet", 3, cpssDxChTtiEthernetTypeSet_PARAMS, NULL},
    {"cpssDxChTtiEthernetTypeGet", 3, cpssDxChTtiEthernetTypeGet_PARAMS, NULL},
    {"cpssDxChTtiTrillCpuCodeBaseSet", 2, cpssDxChTtiTrillCpuCodeBaseSet_PARAMS, NULL},
    {"cpssDxChTtiTrillCpuCodeBaseGet", 2, cpssDxChTtiTrillCpuCodeBaseGet_PARAMS, NULL},
    {"cpssDxChTtiTrillAdjacencyCheckEntrySet", 4, cpssDxChTtiTrillAdjacencyCheckEntrySet_PARAMS, NULL},
    {"cpssDxChTtiTrillAdjacencyCheckEntryGet", 4, cpssDxChTtiTrillAdjacencyCheckEntryGet_PARAMS, NULL},
    {"cpssDxChTtiTrillMaxVersionSet", 2, cpssDxChTtiTrillMaxVersionSet_PARAMS, NULL},
    {"cpssDxChTtiTrillMaxVersionGet", 2, cpssDxChTtiTrillMaxVersionGet_PARAMS, NULL},
    {"cpssDxChTtiPortTrillEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChTtiPortTrillEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiPortTrillOuterVid0Set", 3, cpssDxChTtiPortTrillOuterVid0Set_PARAMS, NULL},
    {"cpssDxChTtiPortTrillOuterVid0Get", 3, cpssDxChTtiPortTrillOuterVid0Get_PARAMS, NULL},
    {"cpssDxChTtiPortGroupMacToMeSet", 6, cpssDxChTtiPortGroupMacToMeSet_PARAMS, NULL},
    {"cpssDxChTtiPortGroupMacToMeGet", 6, cpssDxChTtiPortGroupMacToMeGet_PARAMS, NULL},
    {"cpssDxChTtiPortGroupMacModeSet", 4, cpssDxChTtiPortGroupMacModeSet_PARAMS, NULL},
    {"cpssDxChTtiPortGroupMacModeGet", 4, cpssDxChTtiPortGroupMacModeGet_PARAMS, NULL},
    {"cpssDxChTtiPortGroupRuleSet", 8, cpssDxChTtiPortGroupRuleSet_PARAMS, cpssDxChTtiPortGroupRuleSet_preLogic},
    {"cpssDxChTtiPortGroupRuleGet", 8, cpssDxChTtiPortGroupRuleGet_PARAMS, cpssDxChTtiPortGroupRuleSet_preLogic},
    {"cpssDxChTtiPortGroupRuleActionUpdate", 5, cpssDxChTtiPortGroupRuleActionUpdate_PARAMS, cpssDxChTtiPortGroupRuleActionUpdate_preLogic},
    {"cpssDxChTtiPortGroupRuleValidStatusSet", 4, prvCpssLogGenDevNumPortGroupsBmpIndexValid_PARAMS, NULL},
    {"cpssDxChTtiPortGroupRuleValidStatusGet", 4, cpssDxChTtiPortGroupRuleValidStatusGet_PARAMS, NULL},
    {"cpssDxChTtiPortGroupUserDefinedByteSet", 6, cpssDxChTtiPortGroupUserDefinedByteSet_PARAMS, NULL},
    {"cpssDxChTtiPortGroupUserDefinedByteGet", 6, cpssDxChTtiPortGroupUserDefinedByteGet_PARAMS, NULL},
    {"cpssDxChTtiPortPassengerOuterIsTag0Or1Set", 3, cpssDxChTtiPortPassengerOuterIsTag0Or1Set_PARAMS, NULL},
    {"cpssDxChTtiPortPassengerOuterIsTag0Or1Get", 3, cpssDxChTtiPortPassengerOuterIsTag0Or1Get_PARAMS, NULL},
    {"cpssDxChTtiPacketTypeKeySizeSet", 3, cpssDxChTtiPacketTypeKeySizeSet_PARAMS, NULL},
    {"cpssDxChTtiPacketTypeKeySizeGet", 3, cpssDxChTtiPacketTypeKeySizeGet_PARAMS, NULL},
    {"cpssDxChTtiTcamSegmentModeSet", 3, cpssDxChTtiTcamSegmentModeSet_PARAMS, NULL},
    {"cpssDxChTtiTcamSegmentModeGet", 3, cpssDxChTtiTcamSegmentModeGet_PARAMS, NULL},
    {"cpssDxChTtiMcTunnelDuplicationModeSet", 4, cpssDxChTtiMcTunnelDuplicationModeSet_PARAMS, NULL},
    {"cpssDxChTtiMcTunnelDuplicationModeGet", 4, cpssDxChTtiMcTunnelDuplicationModeGet_PARAMS, NULL},
    {"cpssDxChTtiMcTunnelDuplicationUdpDestPortSet", 2, cpssDxChTtiMcTunnelDuplicationUdpDestPortSet_PARAMS, NULL},
    {"cpssDxChTtiMcTunnelDuplicationUdpDestPortGet", 2, cpssDxChTtiMcTunnelDuplicationUdpDestPortGet_PARAMS, NULL},
    {"cpssDxChTtiMplsMcTunnelTriggeringMacDaSet", 3, cpssDxChTtiMplsMcTunnelTriggeringMacDaSet_PARAMS, NULL},
    {"cpssDxChTtiMplsMcTunnelTriggeringMacDaGet", 3, cpssDxChTtiMplsMcTunnelTriggeringMacDaGet_PARAMS, NULL},
    {"cpssDxChTtiPwCwExceptionCmdSet", 3, cpssDxChTtiPwCwExceptionCmdSet_PARAMS, NULL},
    {"cpssDxChTtiPwCwExceptionCmdGet", 3, cpssDxChTtiPwCwExceptionCmdGet_PARAMS, NULL},
    {"cpssDxChTtiPwCwCpuCodeBaseSet", 2, cpssDxChTtiTrillCpuCodeBaseSet_PARAMS, NULL},
    {"cpssDxChTtiPwCwCpuCodeBaseGet", 2, cpssDxChTtiTrillCpuCodeBaseGet_PARAMS, NULL},
    {"cpssDxChTtiPwCwSequencingSupportEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTtiPwCwSequencingSupportEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiIPv6ExtensionHeaderSet", 3, cpssDxChTtiIPv6ExtensionHeaderSet_PARAMS, NULL},
    {"cpssDxChTtiIPv6ExtensionHeaderGet", 3, cpssDxChTtiIPv6ExtensionHeaderGet_PARAMS, NULL},
    {"cpssDxChTtiSourceIdBitsOverrideSet", 3, cpssDxChTtiSourceIdBitsOverrideSet_PARAMS, NULL},
    {"cpssDxChTtiSourceIdBitsOverrideGet", 3, cpssDxChTtiSourceIdBitsOverrideGet_PARAMS, NULL},
    {"cpssDxChTtiGreExtensionsEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChTtiGreExtensionsEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChTtiUserDefinedByteSet", 5, cpssDxChTtiUserDefinedByteSet_PARAMS, NULL},
    {"cpssDxChTtiUserDefinedByteGet", 5, cpssDxChTtiUserDefinedByteGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_TTI(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChTtiLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChTtiLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

