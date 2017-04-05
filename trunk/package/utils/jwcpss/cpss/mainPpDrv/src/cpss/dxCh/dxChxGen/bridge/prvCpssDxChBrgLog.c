/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChBrgLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgCount.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgE2Phy.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgL2Ecmp.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/log/private/prvCpssDxChLog.h>
#include <cpss/dxCh/dxChxGen/tunnel/private/prvCpssDxChTunnelLog.h>
#include <cpss/generic/bridge/private/prvCpssGenBrgLog.h>
#include <cpss/generic/ip/private/prvCpssGenIpLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>
#include <cpss/generic/port/private/prvCpssGenPortLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_BRG_STP_STATE_MODE_ENT[]  =
{
    "CPSS_BRG_STP_E_VLAN_MODE_E",
    "CPSS_BRG_STP_E_PORT_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_BRG_STP_STATE_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT[]  =
{
    "CPSS_DXCH_ARP_BCAST_CMD_MODE_PORT_E",
    "CPSS_DXCH_ARP_BCAST_CMD_MODE_VLAN_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_256_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_512_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_1K_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_2K_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_4K_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_8K_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_16K_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_32K_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_TAG1_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_EVLAN_E",
    "CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ORIG_VLAN_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_REGULAR_E",
    "CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_FORCE_AGE_WITHOUT_REMOVAL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ALL_ZERO_E",
    "CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_FID_E",
    "CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_MAC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_FDB_DEL_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E",
    "CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E",
    "CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_FDB_DEL_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_E",
    "CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_VID1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_GEN_BYPASS_MODE_ALL_EXCEPT_SA_LEARNING_E",
    "CPSS_DXCH_BRG_GEN_BYPASS_MODE_ONLY_FORWARDING_DECISION_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT[]  =
{
    "CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_L3_E",
    "CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_L2_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_MAC_DA_E",
    "CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_MAC_DA_AND_ETHERTYPE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT[]  =
{
    "CPSS_DXCH_BRG_IP_CTRL_NONE_E",
    "CPSS_DXCH_BRG_IP_CTRL_IPV4_E",
    "CPSS_DXCH_BRG_IP_CTRL_IPV6_E",
    "CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EVLAN_MODE_E",
    "CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EPORT_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E",
    "CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E",
    "CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E",
    "CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E",
    "CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E",
    "CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E",
    "CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E",
    "CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E",
    "CPSS_DXCH_BRG_VLAN_PORT_DO_NOT_MODIFY_TAG_CMD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_NONE_E",
    "CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_TUNNEL_E",
    "CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_PASSENGER_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E",
    "CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E",
    "CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E",
    "CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_AND_INGRESS_WITHOUT_TAG1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_EGRESS_TAG_TPID_SELECT_MODE_E",
    "CPSS_DXCH_BRG_VLAN_VID_TO_TPID_SELECT_TABLE_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E",
    "CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_TRANSLATION_DISABLE_E",
    "CPSS_DXCH_BRG_VLAN_TRANSLATION_VID0_E",
    "CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_E",
    "CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_VID1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_E",
    "CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV4_E",
    "CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV6_E",
    "CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_IPV6_INCREMENT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT[]  =
{
    "CPSS_DXCH_BRG_CNT_SET_ID_0_E",
    "CPSS_DXCH_BRG_CNT_SET_ID_1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT[]  =
{
    "CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E",
    "CPSS_DXCH_BRG_DROP_CNTR_FDB_ENTRY_CMD_E",
    "CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E",
    "CPSS_DXCH_BRG_DROP_CNTR_INVALID_SA_E",
    "CPSS_DXCH_BRG_DROP_CNTR_INVALID_VLAN_E",
    "CPSS_DXCH_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E",
    "CPSS_DXCH_BRG_DROP_CNTR_VLAN_RANGE_E",
    "CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E",
    "CPSS_DXCH_BRG_DROP_CNTR_ARP_SA_MISMATCH_E",
    "CPSS_DXCH_BRG_DROP_CNTR_SYN_WITH_DATA_E",
    "CPSS_DXCH_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E",
    "CPSS_DXCH_BRG_DROP_CNTR_ACCESS_MATRIX_E",
    "CPSS_DXCH_BRG_DROP_CNTR_SEC_LEARNING_E",
    "CPSS_DXCH_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E",
    "CPSS_DXCH_BRG_DROP_CNTR_FRAG_ICMP_E",
    "CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E",
    "CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E",
    "CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E",
    "CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E",
    "CPSS_DXCH_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E",
    "CPSS_DXCH_BRG_DROP_CNTR_VLAN_MRU_E",
    "CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E",
    "CPSS_DXCH_BRG_DROP_CNTR_LOCAL_PORT_E",
    "CPSS_DXCH_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E",
    "CPSS_DXCH_BRG_DROP_CNTR_IP_MC_E",
    "CPSS_DXCH_BRG_DROP_CNTR_NON_IP_MC_E",
    "CPSS_DXCH_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E",
    "CPSS_DXCH_BRG_DROP_CNTR_IEEE_RESERVED_E",
    "CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E",
    "CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E",
    "CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E",
    "CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_L2_UC_E",
    "CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E",
    "CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_FDB_QUEUE_TYPE_ENT[]  =
{
    "CPSS_DXCH_FDB_QUEUE_TYPE_AU_E",
    "CPSS_DXCH_FDB_QUEUE_TYPE_FU_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_FDB_QUEUE_TYPE_ENT);
char * prvCpssLogEnum_GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT[]  =
{
    "GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_NONE_E",
    "GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_4_E",
    "GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_8_E",
    "GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_6_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT);
char * prvCpssLogEnum_CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT[]  =
{
    "CPSS_DXCH_BRG_FDB_TBL_SIZE_8K_E",
    "CPSS_DXCH_BRG_FDB_TBL_SIZE_16K_E",
    "CPSS_DXCH_BRG_FDB_TBL_SIZE_32K_E",
    "CPSS_DXCH_BRG_FDB_TBL_SIZE_64K_E",
    "CPSS_DXCH_BRG_FDB_TBL_SIZE_128K_E",
    "CPSS_DXCH_BRG_FDB_TBL_SIZE_256K_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, tunnelStart);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tunnelStartPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tsPassengerPacketType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, arpPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, modifyMacSa);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, modifyMacDa);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, vlanMode, CPSS_MAC_VL_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, size, CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, fid16BitHashEn);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, crcHashUpperBitsMode, CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, useZeroInitValueForCrcHash);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableBc);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableMc);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableMcReg);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableUcUnk);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableUcKnown);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rateLimit);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dropMode, CPSS_DROP_MODE_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, rMode, CPSS_RATE_LIMIT_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, win10Mbps);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, win100Mbps);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, win1000Mbps);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, win10Gbps);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, targetEport);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, targetHwDevice);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ecmpStartIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ecmpNumOfPaths);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ecmpEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosAssignCmd, CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfileId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, qosAssignPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableModifyUp);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, enableModifyDscp);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_VLAN_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_VLAN_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, unkSrcAddrSecBreach);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unregNonIpMcastCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unregIpv4McastCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unregIpv6McastCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unkUcastCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unregIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unregNonIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv4IgmpToCpuEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mirrToRxAnalyzerEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv6IcmpToCpuEn);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipCtrlToCpuEn, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipv4IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipv6IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv4IpmBrgEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv6IpmBrgEn);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipv6SiteIdMode, CPSS_IP_SITE_ID_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv4UcastRouteEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv4McastRouteEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv6UcastRouteEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv6McastRouteEn);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, stgId);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, autoLearnDisable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, naMsgToCpuEn);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mruIdx);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, bcastUdpTrapMirrEn);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vrfId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, floodVidx);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, floodVidxMode, CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, portIsolationMode, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ucastLocalSwitchingEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mcastLocalSwitchingEn);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mirrToRxAnalyzerIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mirrToTxAnalyzerEn);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mirrToTxAnalyzerIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, fidValue);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unknownMacSaCmd, CPSS_PACKET_CMD_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv4McBcMirrToAnalyzerEn);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ipv4McBcMirrToAnalyzerIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ipv6McMirrToAnalyzerEn);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ipv6McMirrToAnalyzerIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, fcoeForwardingEn);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unregIpmEVidxMode, CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, unregIpmEVidx);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, fdbLookupKeyMode, CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, portsCmd, CPSS_MAX_PORTS_NUM_CNS, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlanId);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, vlanIdAssignCmd, CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, vlanIdAssignPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRIDGE_HOST_CNTR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_BRIDGE_HOST_CNTR_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, gtHostInPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, gtHostOutPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, gtHostOutBroadcastPkts);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, gtHostOutMulticastPkts);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_FIELD_VAL_PTR_MAC(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *valPtr, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_BRG_STP_STATE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_STP_STATE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_BRG_STP_STATE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_BRG_STP_STATE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_STP_STATE_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_BRG_STP_STATE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_FDB_DEL_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_FDB_DEL_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_FDB_DEL_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_FDB_DEL_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_FDB_DEL_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_FDB_DEL_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_VLAN_INFO_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_VLAN_INFO_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_BRIDGE_HOST_CNTR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_BRIDGE_HOST_CNTR_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_BRIDGE_HOST_CNTR_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_FDB_QUEUE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_FDB_QUEUE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_FDB_QUEUE_TYPE_ENT);
}
void prvCpssLogParamFunc_GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT);
}
void prvCpssLogParamFunc_GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_crcHashPtr = {
     "crcHashPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_entryOffsetPtr = {
     "entryOffsetPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfAuPtr = {
     "numOfAuPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_numOfFuPtr = {
     "numOfFuPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC INOUT_GT_U32_PTR_xorHashPtr = {
     "xorHashPtr", PRV_CPSS_LOG_PARAM_INOUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_BRG_IPM_MODE_ENT_ipmMode = {
     "ipmMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_BRG_IPM_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_BRG_MODE_ENT_brgMode = {
     "brgMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_BRG_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC_PTR_cntrCfgPtr = {
     "cntrCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_BRG_SECUR_BREACH_EVENTS_ENT_eventType = {
     "eventType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_BRG_STP_STATE_MODE_ENT_stateMode = {
     "stateMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_BRG_STP_STATE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_BRG_TPID_SIZE_TYPE_ENT_type = {
     "type", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_BRG_TPID_SIZE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_BRIDGE_INGR_CNTR_MODES_ENT_setMode = {
     "setMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_BRIDGE_INGR_CNTR_MODES_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DIRECTION_ENT_direction = {
     "direction", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DIRECTION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DROP_MODE_TYPE_ENT_dropMode = {
     "dropMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DROP_MODE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DROP_MODE_TYPE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DROP_MODE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT_cmdMode = {
     "cmdMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC_PTR_egressInfoPtr = {
     "egressInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT_fltTabAccessMode = {
     "fltTabAccessMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT_vidSelectMode = {
     "vidSelectMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT_trunkAgingMode = {
     "trunkAgingMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_FDB_DEL_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_FDB_DEL_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC_PTR_hashParamsPtr = {
     "hashParamsPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT_byteCountMode = {
     "byteCountMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC_PTR_portGfgPtr = {
     "portGfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC_PTR_brgRateLimitPtr = {
     "brgRateLimitPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT_ipCntrlType = {
     "ipCntrlType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC_PTR_ecmpEntryPtr = {
     "ecmpEntryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC_PTR_ecmpLttInfoPtr = {
     "ecmpLttInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC_PTR_qosCfgPtr = {
     "qosCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT_stateMode = {
     "stateMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT_ethMode = {
     "ethMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT_floodVidxMode = {
     "floodVidxMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_INFO_STC_PTR_vlanInfoPtr = {
     "vlanInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT_trafficType = {
     "trafficType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT_packetType = {
     "packetType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC_PTR_portsTaggingCmdPtr = {
     "portsTaggingCmdPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT_cmd = {
     "cmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_portTaggingCmd = {
     "portTaggingCmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_tagCmd = {
     "tagCmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_taggingCmd = {
     "taggingCmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC_PTR_vlanCfgPtr = {
     "vlanCfgPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT_vlanCmd = {
     "vlanCmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT_enable = {
     "enable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT_unregIpmEVidxMode = {
     "unregIpmEVidxMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT_cntrSetId = {
     "cntrSetId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT_dropMode = {
     "dropMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_ETHERTYPE_TABLE_ENT_tableType = {
     "tableType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_ETHERTYPE_TABLE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_FDB_QUEUE_TYPE_ENT_queueType = {
     "queueType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_FDB_QUEUE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_ETHER_MODE_ENT_ethMode = {
     "ethMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_ETHER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_ETHER_MODE_ENT_etherTypeMode = {
     "etherTypeMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_ETHER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_ETHER_MODE_ENT_ingressVlanSel = {
     "ingressVlanSel", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_ETHER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_FDB_ACTION_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_FDB_ACTION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IGMP_SNOOP_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IGMP_SNOOP_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_INTERFACE_INFO_STC_PTR_newInterfacePtr = {
     "newInterfacePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_INTERFACE_INFO_STC_PTR_oldInterfacePtr = {
     "oldInterfacePtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_INTERFACE_INFO_STC_PTR_physicalInfoPtr = {
     "physicalInfoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IP_PROTOCOL_STACK_ENT_protocol = {
     "protocol", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IP_PROTOCOL_STACK_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_IP_SITE_ID_ENT_siteId = {
     "siteId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_IP_SITE_ID_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_ACTION_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_MAC_ACTION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR_entryKeyPtr = {
     "entryKeyPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_ENTRY_EXT_KEY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR_macEntryKeyPtr = {
     "macEntryKeyPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_ENTRY_EXT_KEY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_ENTRY_EXT_STC_PTR_macEntryPtr = {
     "macEntryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_ENTRY_EXT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_HASH_FUNC_MODE_ENT_hashMode = {
     "hashMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_MAC_HASH_FUNC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_HASH_FUNC_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_MAC_HASH_FUNC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MAC_VL_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_MAC_VL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_NET_RX_CPU_CODE_ENT_exceptionCpuCode = {
     "exceptionCpuCode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_NET_RX_CPU_CODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT_precedence = {
     "precedence", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PACKET_CMD_ENT_cmd = {
     "cmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PACKET_CMD_ENT_exceptionCommand = {
     "exceptionCommand", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PACKET_CMD_ENT_nhPacketCmd = {
     "nhPacketCmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORTS_BMP_STC_PTR_portBitmapPtr = {
     "portBitmapPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORTS_BMP_STC_PTR_portsTaggingPtr = {
     "portsTaggingPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_ACCEPT_FRAME_TYPE_ENT_frameType = {
     "frameType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PORT_ACCEPT_FRAME_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_LOCK_CMD_ENT_cmd = {
     "cmd", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PORT_LOCK_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PORT_SPEED_ENT_speedGranularity = {
     "speedGranularity", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_PORT_SPEED_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_PROT_CLASS_ENCAP_STC_PTR_encListPtr = {
     "encListPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PROT_CLASS_ENCAP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_STP_STATE_ENT_state = {
     "state", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_STP_STATE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_age = {
     "age", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_ageOutAllDevOnNonTrunkEnable = {
     "ageOutAllDevOnNonTrunkEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_ageOutAllDevOnTrunkEnable = {
     "ageOutAllDevOnTrunkEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_cpuSrcIdMember = {
     "cpuSrcIdMember", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_deleteStatic = {
     "deleteStatic", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_dstTrunk = {
     "dstTrunk", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_eCidOffsetNegative = {
     "eCidOffsetNegative", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_forceSrcCidDefault = {
     "forceSrcCidDefault", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_ignoreFilter = {
     "ignoreFilter", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_incOrDec = {
     "incOrDec", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isDefaultProfile = {
     "isDefaultProfile", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isPortExtender = {
     "isPortExtender", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_isTagged = {
     "isTagged", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_mcFilterEnable = {
     "mcFilterEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_mtuCheckEnable = {
     "mtuCheckEnable", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_skip = {
     "skip", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT_numBytesToPop = {
     "numBytesToPop", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_PTR_daAddrPtr = {
     "daAddrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_ETHERADDR_PTR_saAddrPtr = {
     "saAddrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_HW_DEV_NUM_dstHwDev = {
     "dstHwDev", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPV6ADDR_PTR_addressPtr = {
     "addressPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_IPV6ADDR_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PHYSICAL_PORT_NUM_cascadePortNum = {
     "cascadePortNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PHYSICAL_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_dstPort = {
     "dstPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_ecmpIndexBaseEport = {
     "ecmpIndexBaseEport", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_floodVidx = {
     "floodVidx", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_stpId = {
     "stpId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_tagValue = {
     "tagValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_transVlanId = {
     "transVlanId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_udpPortNum = {
     "udpPortNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_vid1 = {
     "vid1", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_vidRange = {
     "vidRange", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U16_vlanMask = {
     "vlanMask", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_stpEntryWordArr = {
     "stpEntryWordArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_actDev = {
     "actDev", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_actDevMask = {
     "actDevMask", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_actIsTrunk = {
     "actIsTrunk", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_actIsTrunkMask = {
     "actIsTrunkMask", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_actTrunkPort = {
     "actTrunkPort", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_actTrunkPortMask = {
     "actTrunkPortMask", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_actUerDefined = {
     "actUerDefined", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_actUerDefinedMask = {
     "actUerDefinedMask", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_bankIndex = {
     "bankIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dBitInSrcId = {
     "dBitInSrcId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_defaultSrcECid = {
     "defaultSrcECid", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_defaultSrcId = {
     "defaultSrcId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_devTableBmp = {
     "devTableBmp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dropCnt = {
     "dropCnt", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_eCidExtValue = {
     "eCidExtValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_eCidOffset = {
     "eCidOffset", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_entryNum = {
     "entryNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_fidValue = {
     "fidValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ingressECidExtValue = {
     "ingressECidExtValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lBitInSrcId = {
     "lBitInSrcId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lookupLen = {
     "lookupLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_meshId = {
     "meshId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mruIndex = {
     "mruIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mruValue = {
     "mruValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_msgRate = {
     "msgRate", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mtuProfileIdx = {
     "mtuProfileIdx", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_mtuSize = {
     "mtuSize", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_multiHashStartBankIndex = {
     "multiHashStartBankIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfBanks = {
     "numOfBanks", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfEntries = {
     "numOfEntries", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_origFdbIndex = {
     "origFdbIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_pcid = {
     "pcid", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_reValue = {
     "reValue", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_tpidBmp = {
     "tpidBmp", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_txAnalyzerIndex = {
     "txAnalyzerIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_unregIpmEVidx = {
     "unregIpmEVidx", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_up0 = {
     "up0", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_up1 = {
     "up1", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vid = {
     "vid", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vidIndex = {
     "vidIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_vrfId = {
     "vrfId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U64_value = {
     "value", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_dipBytesSelectMapArr = {
     "dipBytesSelectMapArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_sipBytesSelectMapArr = {
     "sipBytesSelectMapArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_msgType = {
     "msgType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_BRG_MODE_ENT_PTR_brgModePtr = {
     "brgModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRG_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC_PTR_cntrCfgPtr = {
     "cntrCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_BRG_SECUR_BREACH_MSG_STC_PTR_sbMsgPtr = {
     "sbMsgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRG_SECUR_BREACH_MSG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_BRG_STP_STATE_MODE_ENT_PTR_stateModePtr = {
     "stateModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRG_STP_STATE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_BRG_TPID_SIZE_TYPE_ENT_PTR_typePtr = {
     "typePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRG_TPID_SIZE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_BRIDGE_INGRESS_CNTR_STC_PTR_ingressCntrPtr = {
     "ingressCntrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRIDGE_INGRESS_CNTR_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_BRIDGE_INGR_CNTR_MODES_ENT_PTR_setModePtr = {
     "setModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_BRIDGE_INGR_CNTR_MODES_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DROP_MODE_TYPE_ENT_PTR_dropModePtr = {
     "dropModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DROP_MODE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DROP_MODE_TYPE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DROP_MODE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC_PTR_egressInfoPtr = {
     "egressInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT_PTR_fltTabAccessModePtr = {
     "fltTabAccessModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT_PTR_vidSelectModePtr = {
     "vidSelectModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT_PTR_trunkAgingModePtr = {
     "trunkAgingModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_FDB_DEL_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_FDB_DEL_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT_PTR_byteCountModePtr = {
     "byteCountModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC_PTR_portGfgPtr = {
     "portGfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC_PTR_brgRateLimitPtr = {
     "brgRateLimitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT_PTR_ipCntrlTypePtr = {
     "ipCntrlTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC_PTR_ecmpEntryPtr = {
     "ecmpEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC_PTR_ecmpLttInfoPtr = {
     "ecmpLttInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC_PTR_qosCfgPtr = {
     "qosCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT_PTR_stateModePtr = {
     "stateModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_INFO_STC_PTR_vlanInfoPtr = {
     "vlanInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC_PTR_portsTaggingCmdPtr = {
     "portsTaggingCmdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_PTR_tagCmdPtr = {
     "tagCmdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC_PTR_vlanCfgPtr = {
     "vlanCfgPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT_PTR_vlanCmdPtr = {
     "vlanCmdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT_PTR_enablePtr = {
     "enablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT_PTR_dropModePtr = {
     "dropModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_BRIDGE_HOST_CNTR_STC_PTR_hostGroupCntPtr = {
     "hostGroupCntPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_BRIDGE_HOST_CNTR_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_ETHER_MODE_ENT_PTR_ingressVlanSelPtr = {
     "ingressVlanSelPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_ETHER_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_FDB_ACTION_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_FDB_ACTION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_IGMP_SNOOP_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_IGMP_SNOOP_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_INTERFACE_INFO_STC_PTR_newInterfacePtr = {
     "newInterfacePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_INTERFACE_INFO_STC_PTR_oldInterfacePtr = {
     "oldInterfacePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_INTERFACE_INFO_STC_PTR_physicalInfoPtr = {
     "physicalInfoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_INTERFACE_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_ACTION_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_ACTION_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_ENTRY_EXT_STC_PTR_entryPtr = {
     "entryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_ENTRY_EXT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_HASH_FUNC_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_HASH_FUNC_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_UPDATE_MSG_EXT_STC_PTR_auMessagesPtr = {
     "auMessagesPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_UPDATE_MSG_EXT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_UPDATE_MSG_EXT_STC_PTR_fuMessagesPtr = {
     "fuMessagesPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_UPDATE_MSG_EXT_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_MAC_VL_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_MAC_VL_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_exceptionCpuCodePtr = {
     "exceptionCpuCodePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_NET_RX_CPU_CODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT_PTR_precedencePtr = {
     "precedencePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PACKET_CMD_ENT_PTR_cmdPtr = {
     "cmdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PACKET_CMD_ENT_PTR_exceptionCommandPtr = {
     "exceptionCommandPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PACKET_CMD_ENT_PTR_nhPacketCmdPtr = {
     "nhPacketCmdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PACKET_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORTS_BMP_STC_PTR_portBitmapPtr = {
     "portBitmapPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORTS_BMP_STC_PTR_portsTaggingPtr = {
     "portsTaggingPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORTS_BMP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_ACCEPT_FRAME_TYPE_ENT_PTR_frameTypePtr = {
     "frameTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORT_ACCEPT_FRAME_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_LOCK_CMD_ENT_PTR_cmdPtr = {
     "cmdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORT_LOCK_CMD_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PORT_SPEED_ENT_PTR_speedGranularityPtr = {
     "speedGranularityPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PORT_SPEED_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_PROT_CLASS_ENCAP_STC_PTR_encListPtr = {
     "encListPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_PROT_CLASS_ENCAP_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_STP_STATE_ENT_PTR_statePtr = {
     "statePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_STP_STATE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_ageOutAllDevOnNonTrunkEnablePtr = {
     "ageOutAllDevOnNonTrunkEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_ageOutAllDevOnTrunkEnablePtr = {
     "ageOutAllDevOnTrunkEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_agedPtr = {
     "agedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_completedPtr = {
     "completedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_cpuSrcIdMemberPtr = {
     "cpuSrcIdMemberPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_deleteStaticPtr = {
     "deleteStaticPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_dstTrunkPtr = {
     "dstTrunkPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_eCidOffsetNegativePtr = {
     "eCidOffsetNegativePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_endOfQueueReachedPtr = {
     "endOfQueueReachedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_forceSrcCidDefaultPtr = {
     "forceSrcCidDefaultPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_ignoreFilterPtr = {
     "ignoreFilterPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isFinishedPtr = {
     "isFinishedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isFullPtr = {
     "isFullPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_isPortExtenderPtr = {
     "isPortExtenderPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_mcFilterEnablePtr = {
     "mcFilterEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_mtuCheckEnablePtr = {
     "mtuCheckEnablePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_rewindPtr = {
     "rewindPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_skipPtr = {
     "skipPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_succeededPtr = {
     "succeededPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_validEntryPtr = {
     "validEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT_PTR_numBytesToPopPtr = {
     "numBytesToPopPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_daAddrPtr = {
     "daAddrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_ETHERADDR_PTR_saAddrPtr = {
     "saAddrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_ETHERADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_associatedHwDevNumPtr = {
     "associatedHwDevNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_HW_DEV_NUM_PTR_dstHwDevPtr = {
     "dstHwDevPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_HW_DEV_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_IPV6ADDR_PTR_addressPtr = {
     "addressPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_IPV6ADDR_PTR_maskPtr = {
     "maskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_IPV6ADDR)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_GROUPS_BMP_PTR_actFinishedPortGroupsBmpPtr = {
     "actFinishedPortGroupsBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_GROUPS_BMP)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_GROUPS_BMP_PTR_completedPortGroupsBmpPtr = {
     "completedPortGroupsBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_GROUPS_BMP)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_GROUPS_BMP_PTR_isFullPortGroupsBmpPtr = {
     "isFullPortGroupsBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_GROUPS_BMP)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_GROUPS_BMP_PTR_succeededPortGroupsBmpPtr = {
     "succeededPortGroupsBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_GROUPS_BMP)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_dstPortPtr = {
     "dstPortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_ecmpIndexBaseEportPtr = {
     "ecmpIndexBaseEportPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_PORT_NUM_PTR_portPtr = {
     "portPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_stpIdPtr = {
     "stpIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_tagValuePtr = {
     "tagValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_transVlanIdPtr = {
     "transVlanIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_udpPortNumPtr = {
     "udpPortNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_vid1Ptr = {
     "vid1Ptr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_vidRangePtr = {
     "vidRangePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U16_PTR_vlanMaskPtr = {
     "vlanMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U16)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actDevMaskPtr = {
     "actDevMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actDevPtr = {
     "actDevPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actIsTrunkMaskPtr = {
     "actIsTrunkMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actIsTrunkPtr = {
     "actIsTrunkPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actTrunkPortMaskPtr = {
     "actTrunkPortMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actTrunkPortPtr = {
     "actTrunkPortPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actUerDefinedMaskPtr = {
     "actUerDefinedMaskPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_actUerDefinedPtr = {
     "actUerDefinedPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_counValuePtr = {
     "counValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_countValuePtr = {
     "countValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_crcMultiHashArr = {
     "crcMultiHashArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dBitInSrcIdPtr = {
     "dBitInSrcIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_defaultSrcECidPtr = {
     "defaultSrcECidPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_defaultSrcIdPtr = {
     "defaultSrcIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_devTableBmpPtr = {
     "devTableBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dropCntPtr = {
     "dropCntPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_eCidExtValuePtr = {
     "eCidExtValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_eCidOffsetPtr = {
     "eCidOffsetPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_fidValuePtr = {
     "fidValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_hashPtr = {
     "hashPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_ingressECidExtValuePtr = {
     "ingressECidExtValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_lBitInSrcIdPtr = {
     "lBitInSrcIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_lookupLenPtr = {
     "lookupLenPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_matrixCntSaDaPktsPtr = {
     "matrixCntSaDaPktsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_meshIdPtr = {
     "meshIdPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mruValuePtr = {
     "mruValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_msgRatePtr = {
     "msgRatePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mtuProfileIdxPtr = {
     "mtuProfileIdxPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_mtuSizePtr = {
     "mtuSizePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfAuPtr = {
     "numOfAuPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_profilePtr = {
     "profilePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_reValuePtr = {
     "reValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_stpEntryWordArr = {
     "stpEntryWordArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_timeoutPtr = {
     "timeoutPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_tpidBmpPtr = {
     "tpidBmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_up0Ptr = {
     "up0Ptr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_up1Ptr = {
     "up1Ptr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_vidIndexPtr = {
     "vidIndexPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U64_PTR_valuePtr = {
     "valuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U64)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_dipBytesSelectMapArr = {
     "dipBytesSelectMapArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_msgTypePtr = {
     "msgTypePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U8_PTR_sipBytesSelectMapArr = {
     "sipBytesSelectMapArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U8)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbHashByParamsCalc_PARAMS[] =  {
    &IN_CPSS_MAC_HASH_FUNC_MODE_ENT_hashMode,
    &IN_CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC_PTR_hashParamsPtr,
    &IN_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR_macEntryKeyPtr,
    &OUT_GT_U32_PTR_hashPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpInit_PARAMS[] =  {
    &IN_GT_U8_dev
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSecurBreachPortVlanDropCntrModeSet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC_PTR_cntrCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSecurBreachEventDropModeSet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_CPSS_BRG_SECUR_BREACH_EVENTS_ENT_eventType,
    &IN_CPSS_DROP_MODE_TYPE_ENT_dropMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSecurBreachEventPacketCommandSet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_CPSS_BRG_SECUR_BREACH_EVENTS_ENT_eventType,
    &IN_CPSS_PACKET_CMD_ENT_command
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSecurBreachEventDropModeGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_CPSS_BRG_SECUR_BREACH_EVENTS_ENT_eventType,
    &OUT_CPSS_DROP_MODE_TYPE_ENT_PTR_dropModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSecurBreachEventPacketCommandGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_CPSS_BRG_SECUR_BREACH_EVENTS_ENT_eventType,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_commandPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEgressFilteringEnable_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPortEgrFltUnkEnable_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpStateSet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U16_stpId,
    &IN_CPSS_STP_STATE_ENT_state
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpStateGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U16_stpId,
    &OUT_CPSS_STP_STATE_ENT_PTR_statePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPortEgrFltUnkEnableGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_counValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPortEgrFltIpMcRoutedEnable_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPortEgrFltIpMcRoutedEnableGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSecurBreachPortVlanDropCntrModeGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &OUT_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC_PTR_cntrCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChSecurBreachMsgGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &OUT_CPSS_BRG_SECUR_BREACH_MSG_STC_PTR_sbMsgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEgressFilteringEnableGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSecurBreachGlobalDropCntrGet_PARAMS[] =  {
    &IN_GT_U8_dev,
    &OUT_GT_U32_PTR_counValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanBridgingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_BRG_MODE_ENT_brgMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanMembersTableIndexingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanTpidTagTypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &IN_GT_U32_index,
    &IN_CPSS_BRG_TPID_SIZE_TYPE_ENT_type
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanTpidTagTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &IN_GT_U32_index,
    &OUT_CPSS_BRG_TPID_SIZE_TYPE_ENT_PTR_typePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanMembersTableIndexingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &OUT_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenDropIpMcModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DROP_MODE_TYPE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenArpBcastToCpuCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT_cmdMode,
    &IN_CPSS_PACKET_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenArpBcastToCpuCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT_cmdMode,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_cmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltVlanPortAccessModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT_fltTabAccessMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltVlanPortVidSelectModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT_vidSelectMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbTrunkAgingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT_trunkAgingMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbCrcHashUpperBitsModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbStaticDelEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_FDB_DEL_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenBypassModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIpmClassificationModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenRateLimitGlobalCfgSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC_PTR_brgRateLimitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntBridgeIngressCntrModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT_cntrSetId,
    &IN_CPSS_BRIDGE_INGR_CNTR_MODES_ENT_setMode,
    &IN_GT_PORT_NUM_port,
    &IN_GT_U16_vlan
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntBridgeIngressCntrsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT_cntrSetId,
    &OUT_CPSS_BRIDGE_INGRESS_CNTR_STC_PTR_ingressCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntBridgeIngressCntrModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT_cntrSetId,
    &OUT_CPSS_BRIDGE_INGR_CNTR_MODES_ENT_PTR_setModePtr,
    &OUT_GT_PORT_NUM_PTR_portPtr,
    &OUT_GT_U16_PTR_vlanPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntDropCntrModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT_dropMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanTpidEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_ETHERTYPE_TABLE_ENT_tableType,
    &IN_GT_U32_entryIndex,
    &IN_GT_U16_etherType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanTpidEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_ETHERTYPE_TABLE_ENT_tableType,
    &IN_GT_U32_entryIndex,
    &OUT_GT_U16_PTR_etherTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbQueueFullGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_FDB_QUEUE_TYPE_ENT_queueType,
    &OUT_GT_BOOL_PTR_isFullPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAuqFuqMessagesNumberGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_FDB_QUEUE_TYPE_ENT_queueType,
    &OUT_GT_U32_PTR_numOfAuPtr,
    &OUT_GT_BOOL_PTR_endOfQueueReachedPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEtherTypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_ETHER_MODE_ENT_etherTypeMode,
    &IN_GT_U16_etherType,
    &IN_GT_U16_vidRange
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEtherTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_ETHER_MODE_ENT_etherTypeMode,
    &OUT_GT_U16_PTR_etherTypePtr,
    &OUT_GT_U16_PTR_vidRangePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbTrigActionStart_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_FDB_ACTION_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIgmpSnoopModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IGMP_SNOOP_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionTransplantDataSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_INTERFACE_INFO_STC_PTR_oldInterfacePtr,
    &IN_CPSS_INTERFACE_INFO_STC_PTR_newInterfacePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_GT_U8_protocol,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocolStack,
    &IN_GT_U8_protocol,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacTriggerModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_ACTION_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbHashRequestSend_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR_entryKeyPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbQaSend_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR_macEntryKeyPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbHashCalc_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR_macEntryKeyPtr,
    &OUT_GT_U32_PTR_hashPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_ENTRY_EXT_STC_PTR_macEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbHashModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_HASH_FUNC_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbSecureAutoLearnSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacVlanLookupModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_MAC_VL_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenCiscoL2ProtCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PACKET_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbRoutingNextHopPacketCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PACKET_CMD_ENT_nhPacketCmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_ageOutAllDevOnNonTrunkEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_ageOutAllDevOnTrunkEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbStaticOfNonExistDevRemove_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_deleteStatic
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanBpeTagMcCfgSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_eCidOffsetNegative,
    &IN_GT_U32_eCidOffset,
    &IN_GT_U32_defaultSrcECid
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlan6BytesTagConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_isPortExtender,
    &IN_GT_U32_lBitInSrcId,
    &IN_GT_U32_dBitInSrcId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenMtuConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_mtuCheckEnable,
    &IN_CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT_byteCountMode,
    &IN_CPSS_PACKET_CMD_ENT_exceptionCommand,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_exceptionCpuCode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenDropIpMcEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_state
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntMacDaSaSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_ETHERADDR_PTR_saAddrPtr,
    &IN_GT_ETHERADDR_PTR_daAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_IPV6ADDR_PTR_addressPtr,
    &IN_GT_IPV6ADDR_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_cascadePortNum,
    &IN_GT_BOOL_enable,
    &IN_GT_TRUNK_ID_trunkId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_cascadePortNum,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_GT_TRUNK_ID_PTR_trunkIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenPortRateLimitSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_port,
    &IN_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC_PTR_portGfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenPortRateLimitGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_port,
    &OUT_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC_PTR_portGfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanTagStateIndexingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenPortRateLimitSpeedGranularitySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_CPSS_PORT_SPEED_ENT_speedGranularity
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgMcPhysicalSourceFilteringIgnoreSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_ignoreFilter
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPePortPcidMcFilterEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_pcid,
    &IN_GT_BOOL_mcFilterEnable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPePortPcidMcFilterEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U32_pcid,
    &OUT_GT_BOOL_PTR_mcFilterEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanKeepVlan1EnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U8_up,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanKeepVlan1EnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_U8_up,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanTagStateIndexingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenPortRateLimitSpeedGranularityGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_CPSS_PORT_SPEED_ENT_PTR_speedGranularityPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgMcPhysicalSourceFilteringIgnoreGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_ignoreFilterPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntPortGroupBridgeIngressCntrsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT_cntrSetId,
    &OUT_CPSS_BRIDGE_INGRESS_CNTR_STC_PTR_ingressCntrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupQueueFullGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_FDB_QUEUE_TYPE_ENT_queueType,
    &OUT_GT_PORT_GROUPS_BMP_PTR_isFullPortGroupsBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupQaSend_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR_macEntryKeyPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupMacEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_MAC_ENTRY_EXT_STC_PTR_macEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntPortGroupDropCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_dropCnt
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupMacEntryInvalidate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupMacEntryAgeBitSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &IN_GT_BOOL_age
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupMacEntryWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &IN_GT_BOOL_skip,
    &IN_CPSS_MAC_ENTRY_EXT_STC_PTR_macEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupMacEntryStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_GT_BOOL_PTR_skipPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupMacEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_GT_BOOL_PTR_skipPtr,
    &OUT_GT_BOOL_PTR_agedPtr,
    &OUT_GT_HW_DEV_NUM_PTR_associatedHwDevNumPtr,
    &OUT_CPSS_MAC_ENTRY_EXT_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntPortGroupHostGroupCntrsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_CPSS_DXCH_BRIDGE_HOST_CNTR_STC_PTR_hostGroupCntPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupTrigActionStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_PORT_GROUPS_BMP_PTR_actFinishedPortGroupsBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_PORT_GROUPS_BMP_PTR_completedPortGroupsBmpPtr,
    &OUT_GT_PORT_GROUPS_BMP_PTR_succeededPortGroupsBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntPortGroupLearnedEntryDiscGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_countValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntPortGroupDropCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_dropCntPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntPortGroupMatrixGroupCntrsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_matrixCntSaDaPktsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgL2EcmpIndexBaseEportSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_ecmpIndexBaseEport
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdPortSrcIdAssignModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpPortSpanningTreeStateModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_BRG_STP_STATE_MODE_ENT_stateMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortTranslationEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &IN_CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortVid1Set_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &IN_GT_U16_vid1
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortVidSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &IN_GT_U16_vlanId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortTranslationEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &OUT_CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortVid1Get_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &OUT_GT_U16_PTR_vid1Ptr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortVidGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DIRECTION_ENT_direction,
    &OUT_GT_U16_PTR_vidPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEportToPhysicalPortEgressPortInfoSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC_PTR_egressInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenPortIpControlTrapEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT_ipCntrlType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEgressPortTagStateModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT_stateMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortIngressTpidSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT_ethMode,
    &IN_GT_U32_tpidBmp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortEgressTpidSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT_ethMode,
    &IN_GT_U32_tpidEntryIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortIngressTpidGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT_ethMode,
    &OUT_GT_U32_PTR_tpidBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortEgressTpidGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT_ethMode,
    &OUT_GT_U32_PTR_tpidEntryIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEgressPortTagStateSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_tagCmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortPushVlanCommandSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT_vlanCmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbLearnPrioritySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_DXCH_FDB_LEARN_PRIORITY_ENT_learnPriority
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEgressTagTpidSelectModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_ETHER_MODE_ENT_ethMode,
    &IN_CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortIngressTpidProfileSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_ETHER_MODE_ENT_ethMode,
    &IN_GT_BOOL_isDefaultProfile,
    &IN_GT_U32_profile
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortIngressTpidProfileGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_ETHER_MODE_ENT_ethMode,
    &IN_GT_BOOL_isDefaultProfile,
    &OUT_GT_U32_PTR_profilePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEgressTagTpidSelectModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_ETHER_MODE_ENT_ethMode,
    &OUT_CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgNestVlanEtherTypeSelectSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_ETHER_MODE_ENT_ingressVlanSel
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEportToPhysicalPortTargetMappingTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_INTERFACE_INFO_STC_PTR_physicalInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbRoutingPortIpUcEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocol,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbRoutingPortIpUcEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocol,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortVidPrecedenceSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT_precedence
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_PACKET_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortAccFrameTypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_PORT_ACCEPT_FRAME_TYPE_ENT_frameType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpPortSpanningTreeStateSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_CPSS_STP_STATE_ENT_state
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPrvEdgeVlanPortEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_enable,
    &IN_GT_PORT_NUM_dstPort,
    &IN_GT_HW_DEV_NUM_dstHwDev,
    &IN_GT_BOOL_dstTrunk
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_forceSrcCidDefault
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIgmpSnoopEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_status
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortLearnStatusSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_status,
    &IN_CPSS_PORT_LOCK_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortNumOfTagWordsToPopSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT_numBytesToPop
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortPushedTagValueSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U16_tagValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltPortVid1Set_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U16_vid1
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdPortDefaultSrcIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_defaultSrcId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortProtoVlanQosInvalidate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_entryNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortProtoVlanQosSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_entryNum,
    &IN_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC_PTR_vlanCfgPtr,
    &IN_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC_PTR_qosCfgPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortProtoVlanQosGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_entryNum,
    &OUT_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC_PTR_vlanCfgPtr,
    &OUT_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC_PTR_qosCfgPtr,
    &OUT_GT_BOOL_PTR_validEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrPortMeshIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_meshId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenMtuPortProfileIdxSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_mtuProfileIdx
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_profileIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortUpSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_up
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortUp0Set_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_up0
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortUp1Set_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_up1
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbUserGroupSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_U32_userGroup
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdPortSrcIdAssignModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpPortSpanningTreeStateModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_BRG_STP_STATE_MODE_ENT_PTR_stateModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEportToPhysicalPortEgressPortInfoGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC_PTR_egressInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenPortIpControlTrapEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT_PTR_ipCntrlTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEgressPortTagStateModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT_PTR_stateModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEgressPortTagStateGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_PTR_tagCmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortPushVlanCommandGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT_PTR_vlanCmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbLearnPriorityGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_DXCH_FDB_LEARN_PRIORITY_ENT_PTR_learnPriorityPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgNestVlanEtherTypeSelectGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_ETHER_MODE_ENT_PTR_ingressVlanSelPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEportToPhysicalPortTargetMappingTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_INTERFACE_INFO_STC_PTR_physicalInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortVidPrecedenceGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT_PTR_precedencePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_cmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortAccFrameTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_PORT_ACCEPT_FRAME_TYPE_ENT_PTR_frameTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpPortSpanningTreeStateGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_CPSS_STP_STATE_ENT_PTR_statePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgPrvEdgeVlanPortEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_GT_PORT_NUM_PTR_dstPortPtr,
    &OUT_GT_HW_DEV_NUM_PTR_dstHwDevPtr,
    &OUT_GT_BOOL_PTR_dstTrunkPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_forceSrcCidDefaultPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIgmpSnoopEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_statusPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbPortLearnStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_statusPtr,
    &OUT_CPSS_PORT_LOCK_CMD_ENT_PTR_cmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortNumOfTagWordsToPopGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT_PTR_numBytesToPopPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortPushedTagValueGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U16_PTR_tagValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltPortVid1Get_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U16_PTR_vid1Ptr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdPortDefaultSrcIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_defaultSrcIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrPortMeshIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_meshIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenMtuPortProfileIdxGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_mtuProfileIdxPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenPortIeeeReservedMcastProfileIndexGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_profileIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortUp0Get_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_up0Ptr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortUp1Get_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_up1Ptr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortUpGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_upPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbUserGroupGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_U32_PTR_userGroupPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanRangeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vidRange
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgMcGroupDelete_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vidx
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgMcEntryWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vidx,
    &IN_CPSS_PORTS_BMP_STC_PTR_portBitmapPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgMcMemberAdd_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vidx,
    &IN_GT_PHYSICAL_PORT_NUM_portNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgMcEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vidx,
    &OUT_CPSS_PORTS_BMP_STC_PTR_portBitmapPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIsDevMember_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanTranslationEntryWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_DIRECTION_ENT_direction,
    &IN_GT_U16_transVlanId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanTranslationEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_DIRECTION_ENT_direction,
    &OUT_GT_U16_PTR_transVlanIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanFdbLookupKeyModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIpCntlToCpuSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT_ipCntrlType
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanLocalSwitchingEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT_trafficType,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanUnkUnregFilterSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT_packetType,
    &IN_CPSS_PACKET_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortIsolationCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanUnregisteredIpmEVidxSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT_unregIpmEVidxMode,
    &IN_GT_U32_unregIpmEVidx
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIpmBridgingModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_ipVer,
    &IN_CPSS_BRG_IPM_MODE_ENT_ipmMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIpmBridgingEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_ipVer,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIpUcRouteEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_IP_PROTOCOL_STACK_ENT_protocol,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIpv6SourceSiteIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_IP_SITE_ID_ENT_siteId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanUnknownMacSaCommandSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_PACKET_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEntryWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_CPSS_PORTS_BMP_STC_PTR_portsMembersPtr,
    &IN_CPSS_PORTS_BMP_STC_PTR_portsTaggingPtr,
    &IN_CPSS_DXCH_BRG_VLAN_INFO_STC_PTR_vlanInfoPtr,
    &IN_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC_PTR_portsTaggingCmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanNASecurEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIngressMirrorEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_BOOL_enable,
    &IN_GT_U32_index
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanMirrorToTxAnalyzerSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_BOOL_enable,
    &IN_GT_U32_txAnalyzerIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanLearningStateSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_BOOL_status
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanPortDelete_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_PHYSICAL_PORT_NUM_portNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanMemberSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_isMember,
    &IN_GT_BOOL_isTagged,
    &IN_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_taggingCmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanMemberAdd_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &IN_GT_BOOL_isTagged,
    &IN_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT_portTaggingCmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanFloodVidxModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U16_floodVidx,
    &IN_CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT_floodVidxMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanToStpIdBind_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U16_stpId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionActiveVlanSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U16_vlanMask
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanForwardingIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U32_fidValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanMruProfileIdxSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U32_mruIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEntriesRangeWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U32_numOfEntries,
    &IN_CPSS_PORTS_BMP_STC_PTR_portsMembersPtr,
    &IN_CPSS_PORTS_BMP_STC_PTR_portsTaggingPtr,
    &IN_CPSS_DXCH_BRG_VLAN_INFO_STC_PTR_vlanInfoPtr,
    &IN_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC_PTR_portsTaggingCmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanVrfIdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &IN_GT_U32_vrfId
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIpCntlToCpuGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT_PTR_ipCntrlTypePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanUnknownMacSaCommandGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_cmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_CPSS_PORTS_BMP_STC_PTR_portsMembersPtr,
    &OUT_CPSS_PORTS_BMP_STC_PTR_portsTaggingPtr,
    &OUT_CPSS_DXCH_BRG_VLAN_INFO_STC_PTR_vlanInfoPtr,
    &OUT_GT_BOOL_PTR_isValidPtr,
    &OUT_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC_PTR_portsTaggingCmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIgmpSnoopingEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_GT_BOOL_PTR_enablePtr,
    &OUT_GT_U32_PTR_indexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanStpIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_GT_U16_PTR_stpIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanForwardingIdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U16_vlanId,
    &OUT_GT_U32_PTR_fidValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionActiveDevSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_actDev,
    &IN_GT_U32_actDevMask
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionActiveInterfaceSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_actIsTrunk,
    &IN_GT_U32_actIsTrunkMask,
    &IN_GT_U32_actTrunkPort,
    &IN_GT_U32_actTrunkPortMask
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionActiveUserDefinedSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_actUerDefined,
    &IN_GT_U32_actUerDefinedMask
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbBankCounterUpdate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_bankIndex,
    &IN_GT_BOOL_incOrDec
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbBankCounterValueGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_bankIndex,
    &OUT_GT_U32_PTR_valuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbDeviceTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_devTableBmp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntDropCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_dropCnt
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenUdpBcDestPortCfgSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &IN_GT_U16_udpPortNum,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_cpuCode,
    &IN_CPSS_PACKET_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenUdpBcDestPortCfgGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryIndex,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_GT_U16_PTR_udpPortNumPtr,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_cpuCodePtr,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_cmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanProtoClassInvalidate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanProtoClassSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryNum,
    &IN_GT_U16_etherType,
    &IN_CPSS_PROT_CLASS_ENCAP_STC_PTR_encListPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanProtoClassGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_entryNum,
    &OUT_GT_U16_PTR_etherTypePtr,
    &OUT_CPSS_PROT_CLASS_ENCAP_STC_PTR_encListPtr,
    &OUT_GT_BOOL_PTR_validEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgL2EcmpTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC_PTR_ecmpEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgL2EcmpLttTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC_PTR_ecmpLttInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacEntryAgeBitSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_GT_BOOL_age
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacEntryWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_GT_BOOL_skip,
    &IN_CPSS_MAC_ENTRY_EXT_STC_PTR_macEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIcmpv6MsgTypeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &IN_GT_U8_msgType,
    &IN_CPSS_PACKET_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgL2EcmpTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC_PTR_ecmpEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgL2EcmpLttTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC_PTR_ecmpLttInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacEntryStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_GT_BOOL_PTR_skipPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_BOOL_PTR_validPtr,
    &OUT_GT_BOOL_PTR_skipPtr,
    &OUT_GT_BOOL_PTR_agedPtr,
    &OUT_GT_HW_DEV_NUM_PTR_associatedHwDevNumPtr,
    &OUT_CPSS_MAC_ENTRY_EXT_STC_PTR_entryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIcmpv6MsgTypeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_index,
    &OUT_GT_U8_PTR_msgTypePtr,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_cmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMaxLookupLenSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_lookupLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanMruProfileValueSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mruIndex,
    &IN_GT_U32_mruValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanMruProfileValueGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_mruIndex,
    &OUT_GT_U32_PTR_mruValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAuMsgRateLimitSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_msgRate,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacEntryMove_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_origFdbIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIngressTpidProfileSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profile,
    &IN_CPSS_ETHER_MODE_ENT_ethMode,
    &IN_GT_U32_tpidBmp
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanIngressTpidProfileGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profile,
    &IN_CPSS_ETHER_MODE_ENT_ethMode,
    &OUT_GT_U32_PTR_tpidBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenMtuProfileSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profile,
    &IN_GT_U32_mtuSize
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenMtuProfileGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profile,
    &OUT_GT_U32_PTR_mtuSizePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIeeeReservedMcastProtCmdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &IN_GT_U8_protocol,
    &IN_CPSS_PACKET_CMD_ENT_cmd
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIeeeReservedMcastProtCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_profileIndex,
    &IN_GT_U8_protocol,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_cmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanBpeTagReservedFieldsSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_reValue,
    &IN_GT_U32_ingressECidExtValue,
    &IN_GT_U32_eCidExtValue
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdGroupEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_sourceId,
    &IN_GT_BOOL_cpuSrcIdMember,
    &IN_CPSS_PORTS_BMP_STC_PTR_portsMembersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdGroupPortAdd_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_sourceId,
    &IN_GT_PHYSICAL_PORT_NUM_portNum
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdGroupEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_sourceId,
    &OUT_GT_BOOL_PTR_cpuSrcIdMemberPtr,
    &OUT_CPSS_PORTS_BMP_STC_PTR_portsMembersPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAgingTimeoutSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_timeout
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltVlanPortVidMappingSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_vid,
    &IN_GT_U32_vidIndex
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltVlanPortVidMappingGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_vid,
    &OUT_GT_U32_PTR_vidIndexPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltVlanPortMemberSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_vidIndex,
    &IN_GT_PORT_NUM_portNum,
    &IN_GT_BOOL_isMember
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltVlanPortMemberGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_vidIndex,
    &IN_GT_PORT_NUM_portNum,
    &OUT_GT_BOOL_PTR_isMemberPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenRateLimitDropCntrSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U64_value
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAuMsgBlockGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &INOUT_GT_U32_PTR_numOfAuPtr,
    &OUT_CPSS_MAC_UPDATE_MSG_EXT_STC_PTR_auMessagesPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbFuMsgBlockGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &INOUT_GT_U32_PTR_numOfFuPtr,
    &OUT_CPSS_MAC_UPDATE_MSG_EXT_STC_PTR_fuMessagesPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanBridgingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_BRG_MODE_ENT_PTR_brgModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenDropIpMcModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DROP_MODE_TYPE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltVlanPortAccessModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT_PTR_fltTabAccessModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgEgrFltVlanPortVidSelectModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT_PTR_vidSelectModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbTrunkAgingModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT_PTR_trunkAgingModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbCrcHashUpperBitsModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbStaticDelEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_FDB_DEL_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenBypassModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIpmClassificationModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenRateLimitGlobalCfgGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC_PTR_brgRateLimitPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntDropCntrModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT_PTR_dropModePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntHostGroupCntrsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_BRIDGE_HOST_CNTR_STC_PTR_hostGroupCntPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_FDB_ACTION_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIgmpSnoopModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_IGMP_SNOOP_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionTransplantDataGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_INTERFACE_INFO_STC_PTR_oldInterfacePtr,
    &OUT_CPSS_INTERFACE_INFO_STC_PTR_newInterfacePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacTriggerModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_MAC_ACTION_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbHashModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_MAC_HASH_FUNC_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbSecureAutoLearnGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMacVlanLookupModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_MAC_VL_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenCiscoL2ProtCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_cmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbRoutingNextHopPacketCmdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_nhPacketCmdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbTrigActionStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_actFinishedPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_ageOutAllDevOnNonTrunkEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_ageOutAllDevOnTrunkEnablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbFromCpuAuMsgStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_completedPtr,
    &OUT_GT_BOOL_PTR_succeededPtr,
    &INOUT_GT_U32_PTR_entryOffsetPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbStaticOfNonExistDevRemoveGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_deleteStaticPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanBpeTagMcCfgGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_eCidOffsetNegativePtr,
    &OUT_GT_U32_PTR_eCidOffsetPtr,
    &OUT_GT_U32_PTR_defaultSrcECidPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbBankCounterUpdateStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_isFinishedPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlan6BytesTagConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_isPortExtenderPtr,
    &OUT_GT_U32_PTR_lBitInSrcIdPtr,
    &OUT_GT_U32_PTR_dBitInSrcIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenMtuConfigGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_mtuCheckEnablePtr,
    &OUT_CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT_PTR_byteCountModePtr,
    &OUT_CPSS_PACKET_CMD_ENT_PTR_exceptionCommandPtr,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_exceptionCpuCodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbQueueRewindStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_rewindPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenDropIpMcEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_statePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntMacDaSaGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_ETHERADDR_PTR_saAddrPtr,
    &OUT_GT_ETHERADDR_PTR_daAddrPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_IPV6ADDR_PTR_addressPtr,
    &OUT_GT_IPV6ADDR_PTR_maskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgL2EcmpIndexBaseEportGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_PORT_NUM_PTR_ecmpIndexBaseEportPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanRangeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U16_PTR_vidRangePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionActiveVlanGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U16_PTR_vlanIdPtr,
    &OUT_GT_U16_PTR_vlanMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionActiveDevGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_actDevPtr,
    &OUT_GT_U32_PTR_actDevMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionActiveInterfaceGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_actIsTrunkPtr,
    &OUT_GT_U32_PTR_actIsTrunkMaskPtr,
    &OUT_GT_U32_PTR_actTrunkPortPtr,
    &OUT_GT_U32_PTR_actTrunkPortMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbActionActiveUserDefinedGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_actUerDefinedPtr,
    &OUT_GT_U32_PTR_actUerDefinedMaskPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntLearnedEntryDiscGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_countValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbDeviceTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_devTableBmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntDropCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_dropCntPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbMaxLookupLenGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_lookupLenPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgCntMatrixGroupCntrsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_matrixCntSaDaPktsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAuMsgRateLimitGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_msgRatePtr,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgVlanBpeTagReservedFieldsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_reValuePtr,
    &OUT_GT_U32_PTR_ingressECidExtValuePtr,
    &OUT_GT_U32_PTR_eCidExtValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbAgingTimeoutGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_timeoutPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenMtuExceedCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_valuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgGenRateLimitDropCntrGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U64_PTR_valuePtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbHashCrcMultiResultsByParamsCalc_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbHashCrcMultiResultsCalc_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgFdbHashResultsGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgMcIpv6BytesSelectGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgMcIpv6BytesSelectSet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpEntryGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChBrgStpEntryWrite_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChBrgFdbFuMsgBlockGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgFdbFuMsgBlockGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgFdbFuMsgBlockGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgMcIpv6BytesSelectSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgMcIpv6BytesSelectSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgStpEntryGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgStpEntryGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgFdbHashCrcMultiResultsCalc_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgFdbHashCrcMultiResultsByParamsCalc_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChBrgFdbHashResultsGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChBridgeLogLibDb[] = {
    {"cpssDxChBrgCntDropCntrModeSet", 2, cpssDxChBrgCntDropCntrModeSet_PARAMS, NULL},
    {"cpssDxChBrgCntDropCntrModeGet", 2, cpssDxChBrgCntDropCntrModeGet_PARAMS, NULL},
    {"cpssDxChBrgCntMacDaSaSet", 3, cpssDxChBrgCntMacDaSaSet_PARAMS, NULL},
    {"cpssDxChBrgCntMacDaSaGet", 3, cpssDxChBrgCntMacDaSaGet_PARAMS, NULL},
    {"cpssDxChBrgCntBridgeIngressCntrModeSet", 5, cpssDxChBrgCntBridgeIngressCntrModeSet_PARAMS, NULL},
    {"cpssDxChBrgCntBridgeIngressCntrModeGet", 5, cpssDxChBrgCntBridgeIngressCntrModeGet_PARAMS, NULL},
    {"cpssDxChBrgCntDropCntrGet", 2, cpssDxChBrgCntDropCntrGet_PARAMS, NULL},
    {"cpssDxChBrgCntDropCntrSet", 2, cpssDxChBrgCntDropCntrSet_PARAMS, NULL},
    {"cpssDxChBrgCntHostGroupCntrsGet", 2, cpssDxChBrgCntHostGroupCntrsGet_PARAMS, NULL},
    {"cpssDxChBrgCntMatrixGroupCntrsGet", 2, cpssDxChBrgCntMatrixGroupCntrsGet_PARAMS, NULL},
    {"cpssDxChBrgCntBridgeIngressCntrsGet", 3, cpssDxChBrgCntBridgeIngressCntrsGet_PARAMS, NULL},
    {"cpssDxChBrgCntLearnedEntryDiscGet", 2, cpssDxChBrgCntLearnedEntryDiscGet_PARAMS, NULL},
    {"cpssDxChBrgCntPortGroupDropCntrGet", 3, cpssDxChBrgCntPortGroupDropCntrGet_PARAMS, NULL},
    {"cpssDxChBrgCntPortGroupDropCntrSet", 3, cpssDxChBrgCntPortGroupDropCntrSet_PARAMS, NULL},
    {"cpssDxChBrgCntPortGroupHostGroupCntrsGet", 3, cpssDxChBrgCntPortGroupHostGroupCntrsGet_PARAMS, NULL},
    {"cpssDxChBrgCntPortGroupMatrixGroupCntrsGet", 3, cpssDxChBrgCntPortGroupMatrixGroupCntrsGet_PARAMS, NULL},
    {"cpssDxChBrgCntPortGroupBridgeIngressCntrsGet", 4, cpssDxChBrgCntPortGroupBridgeIngressCntrsGet_PARAMS, NULL},
    {"cpssDxChBrgCntPortGroupLearnedEntryDiscGet", 3, cpssDxChBrgCntPortGroupLearnedEntryDiscGet_PARAMS, NULL},
    {"cpssDxChBrgEportToPhysicalPortTargetMappingTableSet", 3, cpssDxChBrgEportToPhysicalPortTargetMappingTableSet_PARAMS, NULL},
    {"cpssDxChBrgEportToPhysicalPortTargetMappingTableGet", 3, cpssDxChBrgEportToPhysicalPortTargetMappingTableGet_PARAMS, NULL},
    {"cpssDxChBrgEportToPhysicalPortEgressPortInfoSet", 3, cpssDxChBrgEportToPhysicalPortEgressPortInfoSet_PARAMS, NULL},
    {"cpssDxChBrgEportToPhysicalPortEgressPortInfoGet", 3, cpssDxChBrgEportToPhysicalPortEgressPortInfoGet_PARAMS, NULL},
    {"cpssDxChBrgPortEgrFltUnkEnable", 3, cpssDxChBrgPortEgrFltUnkEnable_PARAMS, NULL},
    {"cpssDxChBrgPortEgrFltUnkEnableGet", 3, cpssDxChBrgPortEgrFltUnkEnableGet_PARAMS, NULL},
    {"cpssDxChBrgPortEgrFltUregMcastEnable", 3, cpssDxChBrgPortEgrFltUnkEnable_PARAMS, NULL},
    {"cpssDxChBrgPortEgrFltUregMcastEnableGet", 3, cpssDxChBrgPortEgrFltUnkEnableGet_PARAMS, NULL},
    {"cpssDxChBrgVlanEgressFilteringEnable", 2, cpssDxChBrgVlanEgressFilteringEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanEgressFilteringEnableGet", 2, cpssDxChBrgVlanEgressFilteringEnableGet_PARAMS, NULL},
    {"cpssDxChBrgRoutedUnicastEgressFilteringEnable", 2, cpssDxChBrgVlanEgressFilteringEnable_PARAMS, NULL},
    {"cpssDxChBrgRoutedUnicastEgressFilteringEnableGet", 2, cpssDxChBrgVlanEgressFilteringEnableGet_PARAMS, NULL},
    {"cpssDxChBrgRoutedSpanEgressFilteringEnable", 2, cpssDxChBrgVlanEgressFilteringEnable_PARAMS, NULL},
    {"cpssDxChBrgRoutedSpanEgressFilteringEnableGet", 2, cpssDxChBrgVlanEgressFilteringEnableGet_PARAMS, NULL},
    {"cpssDxChBrgPortEgrFltUregBcEnable", 3, cpssDxChBrgPortEgrFltUnkEnable_PARAMS, NULL},
    {"cpssDxChBrgPortEgrFltUregBcEnableGet", 3, cpssDxChBrgPortEgrFltUnkEnableGet_PARAMS, NULL},
    {"cpssDxChBrgPortEgressMcastLocalEnable", 3, cpssDxChBrgPortEgrFltUnkEnable_PARAMS, NULL},
    {"cpssDxChBrgPortEgressMcastLocalEnableGet", 3, cpssDxChBrgPortEgrFltUnkEnableGet_PARAMS, NULL},
    {"cpssDxChBrgPortEgrFltIpMcRoutedEnable", 3, cpssDxChBrgPortEgrFltIpMcRoutedEnable_PARAMS, NULL},
    {"cpssDxChBrgPortEgrFltIpMcRoutedEnableGet", 3, cpssDxChBrgPortEgrFltIpMcRoutedEnableGet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortFilteringEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortFilteringEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortVidSelectModeSet", 2, cpssDxChBrgEgrFltVlanPortVidSelectModeSet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortVidSelectModeGet", 2, cpssDxChBrgEgrFltVlanPortVidSelectModeGet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortVidMappingSet", 3, cpssDxChBrgEgrFltVlanPortVidMappingSet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortVidMappingGet", 3, cpssDxChBrgEgrFltVlanPortVidMappingGet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortAccessModeSet", 2, cpssDxChBrgEgrFltVlanPortAccessModeSet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortAccessModeGet", 2, cpssDxChBrgEgrFltVlanPortAccessModeGet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortMemberSet", 4, cpssDxChBrgEgrFltVlanPortMemberSet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltVlanPortMemberGet", 4, cpssDxChBrgEgrFltVlanPortMemberGet_PARAMS, NULL},
    {"cpssDxChBrgEgrMeshIdConfigurationSet", 4, prvCpssLogGenDevNumEnableMeshIdOffsetMeshIdSize_PARAMS, NULL},
    {"cpssDxChBrgEgrMeshIdConfigurationGet", 4, prvCpssLogGenDevNumEnablePtrMeshIdOffsetPtrMeshIdSizePtr_PARAMS, NULL},
    {"cpssDxChBrgEgrPortMeshIdSet", 3, cpssDxChBrgEgrPortMeshIdSet_PARAMS, NULL},
    {"cpssDxChBrgEgrPortMeshIdGet", 3, cpssDxChBrgEgrPortMeshIdGet_PARAMS, NULL},
    {"cpssDxChBrgEgrFltPortVid1FilteringEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgEgrFltPortVid1FilteringEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgEgrFltPortVid1Set", 3, cpssDxChBrgEgrFltPortVid1Set_PARAMS, NULL},
    {"cpssDxChBrgEgrFltPortVid1Get", 3, cpssDxChBrgEgrFltPortVid1Get_PARAMS, NULL},
    {"cpssDxChBrgEgrFltPortLinkEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChBrgEgrFltPortLinkEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChBrgFdbPortLearnStatusSet", 4, cpssDxChBrgFdbPortLearnStatusSet_PARAMS, NULL},
    {"cpssDxChBrgFdbPortLearnStatusGet", 4, cpssDxChBrgFdbPortLearnStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdbNaToCpuPerPortSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbNaToCpuPerPortGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbNaStormPreventSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbNaStormPreventGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbPortVid1LearningEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbPortVid1LearningEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbDeviceTableSet", 2, cpssDxChBrgFdbDeviceTableSet_PARAMS, NULL},
    {"cpssDxChBrgFdbDeviceTableGet", 2, cpssDxChBrgFdbDeviceTableGet_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupMacEntrySet", 3, cpssDxChBrgFdbPortGroupMacEntrySet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacEntrySet", 2, cpssDxChBrgFdbMacEntrySet_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupQaSend", 3, cpssDxChBrgFdbPortGroupQaSend_PARAMS, NULL},
    {"cpssDxChBrgFdbQaSend", 2, cpssDxChBrgFdbQaSend_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupMacEntryDelete", 3, cpssDxChBrgFdbPortGroupQaSend_PARAMS, NULL},
    {"cpssDxChBrgFdbMacEntryDelete", 2, cpssDxChBrgFdbQaSend_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupMacEntryWrite", 5, cpssDxChBrgFdbPortGroupMacEntryWrite_PARAMS, NULL},
    {"cpssDxChBrgFdbMacEntryWrite", 4, cpssDxChBrgFdbMacEntryWrite_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupMacEntryStatusGet", 5, cpssDxChBrgFdbPortGroupMacEntryStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacEntryStatusGet", 4, cpssDxChBrgFdbMacEntryStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupMacEntryRead", 8, cpssDxChBrgFdbPortGroupMacEntryRead_PARAMS, NULL},
    {"cpssDxChBrgFdbMacEntryRead", 7, cpssDxChBrgFdbMacEntryRead_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupMacEntryInvalidate", 3, cpssDxChBrgFdbPortGroupMacEntryInvalidate_PARAMS, NULL},
    {"cpssDxChBrgFdbMacEntryInvalidate", 2, prvCpssLogGenDevNumIndex_PARAMS, NULL},
    {"cpssDxChBrgFdbMaxLookupLenSet", 2, cpssDxChBrgFdbMaxLookupLenSet_PARAMS, NULL},
    {"cpssDxChBrgFdbMaxLookupLenGet", 2, cpssDxChBrgFdbMaxLookupLenGet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacVlanLookupModeSet", 2, cpssDxChBrgFdbMacVlanLookupModeSet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacVlanLookupModeGet", 2, cpssDxChBrgFdbMacVlanLookupModeGet_PARAMS, NULL},
    {"cpssDxChBrgFdbAuMsgRateLimitSet", 3, cpssDxChBrgFdbAuMsgRateLimitSet_PARAMS, NULL},
    {"cpssDxChBrgFdbAuMsgRateLimitGet", 3, cpssDxChBrgFdbAuMsgRateLimitGet_PARAMS, NULL},
    {"cpssDxChBrgFdbNaMsgOnChainTooLongSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbNaMsgOnChainTooLongGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbSpAaMsgToCpuSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbSpAaMsgToCpuGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbAAandTAToCpuSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbAAandTAToCpuGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbHashModeSet", 2, cpssDxChBrgFdbHashModeSet_PARAMS, NULL},
    {"cpssDxChBrgFdbHashModeGet", 2, cpssDxChBrgFdbHashModeGet_PARAMS, NULL},
    {"cpssDxChBrgFdbAgingTimeoutSet", 2, cpssDxChBrgFdbAgingTimeoutSet_PARAMS, NULL},
    {"cpssDxChBrgFdbAgingTimeoutGet", 2, cpssDxChBrgFdbAgingTimeoutGet_PARAMS, NULL},
    {"cpssDxChBrgFdbSecureAgingSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbSecureAgingGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbSecureAutoLearnSet", 2, cpssDxChBrgFdbSecureAutoLearnSet_PARAMS, NULL},
    {"cpssDxChBrgFdbSecureAutoLearnGet", 2, cpssDxChBrgFdbSecureAutoLearnGet_PARAMS, NULL},
    {"cpssDxChBrgFdbStaticTransEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbStaticTransEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbStaticDelEnable", 2, cpssDxChBrgFdbStaticDelEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbStaticDelEnableGet", 2, cpssDxChBrgFdbStaticDelEnableGet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionsEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbActionsEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbTrigActionStatusGet", 2, cpssDxChBrgFdbTrigActionStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacTriggerModeSet", 2, cpssDxChBrgFdbMacTriggerModeSet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacTriggerModeGet", 2, cpssDxChBrgFdbMacTriggerModeGet_PARAMS, NULL},
    {"cpssDxChBrgFdbStaticOfNonExistDevRemove", 2, cpssDxChBrgFdbStaticOfNonExistDevRemove_PARAMS, NULL},
    {"cpssDxChBrgFdbStaticOfNonExistDevRemoveGet", 2, cpssDxChBrgFdbStaticOfNonExistDevRemoveGet_PARAMS, NULL},
    {"cpssDxChBrgFdbDropAuEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbDropAuEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable", 2, cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet", 2, cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet_PARAMS, NULL},
    {"cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable", 2, cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet", 2, cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionTransplantDataSet", 3, cpssDxChBrgFdbActionTransplantDataSet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionTransplantDataGet", 3, cpssDxChBrgFdbActionTransplantDataGet_PARAMS, NULL},
    {"cpssDxChBrgFdbFromCpuAuMsgStatusGet", 4, cpssDxChBrgFdbFromCpuAuMsgStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionActiveVlanSet", 3, cpssDxChBrgFdbActionActiveVlanSet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionActiveVlanGet", 3, cpssDxChBrgFdbActionActiveVlanGet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionActiveDevSet", 3, cpssDxChBrgFdbActionActiveDevSet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionActiveDevGet", 3, cpssDxChBrgFdbActionActiveDevGet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionActiveInterfaceSet", 5, cpssDxChBrgFdbActionActiveInterfaceSet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionActiveInterfaceGet", 5, cpssDxChBrgFdbActionActiveInterfaceGet_PARAMS, NULL},
    {"cpssDxChBrgFdbIpmcAddrDelEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbIpmcAddrDelEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbUploadEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbUploadEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbTrigActionStart", 2, cpssDxChBrgFdbTrigActionStart_PARAMS, NULL},
    {"cpssDxChBrgFdbActionModeSet", 2, cpssDxChBrgFdbTrigActionStart_PARAMS, NULL},
    {"cpssDxChBrgFdbActionModeGet", 2, cpssDxChBrgFdbActionModeGet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacTriggerToggle", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChBrgFdbTrunkAgingModeSet", 2, cpssDxChBrgFdbTrunkAgingModeSet_PARAMS, NULL},
    {"cpssDxChBrgFdbTrunkAgingModeGet", 2, cpssDxChBrgFdbTrunkAgingModeGet_PARAMS, NULL},
    {"cpssDxChBrgFdbInit", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChBrgFdbAgeBitDaRefreshEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbAgeBitDaRefreshEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutedLearningEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutedLearningEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbTriggerAuQueueWa", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChBrgFdbQueueFullGet", 3, cpssDxChBrgFdbQueueFullGet_PARAMS, NULL},
    {"cpssDxChBrgFdbQueueRewindStatusGet", 2, cpssDxChBrgFdbQueueRewindStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdbMessagesQueueManagerInfoGet", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupMacEntryAgeBitSet", 4, cpssDxChBrgFdbPortGroupMacEntryAgeBitSet_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupTrigActionStatusGet", 3, cpssDxChBrgFdbPortGroupTrigActionStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupQueueFullGet", 4, cpssDxChBrgFdbPortGroupQueueFullGet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacEntryAgeBitSet", 3, cpssDxChBrgFdbMacEntryAgeBitSet_PARAMS, NULL},
    {"cpssDxChBrgFdbNaMsgVid1EnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbNaMsgVid1EnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet", 4, cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdb16BitFidHashEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdb16BitFidHashEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbMaxLengthSrcIdEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbMaxLengthSrcIdEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbVid1AssignmentEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbVid1AssignmentEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbSaLookupAnalyzerIndexSet", 3, prvCpssLogGenDevNumEnableIndex_PARAMS, NULL},
    {"cpssDxChBrgFdbSaLookupAnalyzerIndexGet", 3, prvCpssLogGenDevNumEnablePtrIndexPtr_PARAMS, NULL},
    {"cpssDxChBrgFdbDaLookupAnalyzerIndexSet", 3, prvCpssLogGenDevNumEnableIndex_PARAMS, NULL},
    {"cpssDxChBrgFdbDaLookupAnalyzerIndexGet", 3, prvCpssLogGenDevNumEnablePtrIndexPtr_PARAMS, NULL},
    {"cpssDxChBrgFdbActionActiveUserDefinedSet", 3, cpssDxChBrgFdbActionActiveUserDefinedSet_PARAMS, NULL},
    {"cpssDxChBrgFdbActionActiveUserDefinedGet", 3, cpssDxChBrgFdbActionActiveUserDefinedGet_PARAMS, NULL},
    {"cpssDxChBrgFdbLearnPrioritySet", 3, cpssDxChBrgFdbLearnPrioritySet_PARAMS, NULL},
    {"cpssDxChBrgFdbLearnPriorityGet", 3, cpssDxChBrgFdbLearnPriorityGet_PARAMS, NULL},
    {"cpssDxChBrgFdbUserGroupSet", 3, cpssDxChBrgFdbUserGroupSet_PARAMS, NULL},
    {"cpssDxChBrgFdbUserGroupGet", 3, cpssDxChBrgFdbUserGroupGet_PARAMS, NULL},
    {"cpssDxChBrgFdbCrcHashUpperBitsModeSet", 2, cpssDxChBrgFdbCrcHashUpperBitsModeSet_PARAMS, NULL},
    {"cpssDxChBrgFdbCrcHashUpperBitsModeGet", 2, cpssDxChBrgFdbCrcHashUpperBitsModeGet_PARAMS, NULL},
    {"cpssDxChBrgFdbBankCounterValueGet", 3, cpssDxChBrgFdbBankCounterValueGet_PARAMS, NULL},
    {"cpssDxChBrgFdbBankCounterUpdate", 3, cpssDxChBrgFdbBankCounterUpdate_PARAMS, NULL},
    {"cpssDxChBrgFdbBankCounterUpdateStatusGet", 2, cpssDxChBrgFdbBankCounterUpdateStatusGet_PARAMS, NULL},
    {"cpssDxChBrgFdbMacEntryMove", 2, cpssDxChBrgFdbMacEntryMove_PARAMS, NULL},
    {"cpssDxChBrgFdbFuMsgBlockGet", 3, cpssDxChBrgFdbFuMsgBlockGet_PARAMS, cpssDxChBrgFdbFuMsgBlockGet_preLogic},
    {"cpssDxChBrgFdbAuMsgBlockGet", 3, cpssDxChBrgFdbAuMsgBlockGet_PARAMS, cpssDxChBrgFdbFuMsgBlockGet_preLogic},
    {"cpssDxChBrgFdbAuqFuqMessagesNumberGet", 4, cpssDxChBrgFdbAuqFuqMessagesNumberGet_PARAMS, NULL},
    {"cpssDxChBrgFdbSecondaryAuMsgBlockGet", 3, cpssDxChBrgFdbAuMsgBlockGet_PARAMS, cpssDxChBrgFdbFuMsgBlockGet_preLogic},
    {"cpssDxChBrgFdbRoutingPortIpUcEnableSet", 4, cpssDxChBrgFdbRoutingPortIpUcEnableSet_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingPortIpUcEnableGet", 4, cpssDxChBrgFdbRoutingPortIpUcEnableGet_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcRefreshEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcRefreshEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcAgingEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcAgingEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcTransplantEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcTransplantEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcDeleteEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcDeleteEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcAAandTAToCpuSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingUcAAandTAToCpuGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingNextHopPacketCmdSet", 2, cpssDxChBrgFdbRoutingNextHopPacketCmdSet_PARAMS, NULL},
    {"cpssDxChBrgFdbRoutingNextHopPacketCmdGet", 2, cpssDxChBrgFdbRoutingNextHopPacketCmdGet_PARAMS, NULL},
    {"cpssDxChBrgGenIgmpSnoopEnable", 3, cpssDxChBrgGenIgmpSnoopEnable_PARAMS, NULL},
    {"cpssDxChBrgGenIgmpSnoopEnableGet", 3, cpssDxChBrgGenIgmpSnoopEnableGet_PARAMS, NULL},
    {"cpssDxChBrgGenDropIpMcEnable", 2, cpssDxChBrgGenDropIpMcEnable_PARAMS, NULL},
    {"cpssDxChBrgGenDropIpMcEnableGet", 2, cpssDxChBrgGenDropIpMcEnableGet_PARAMS, NULL},
    {"cpssDxChBrgGenDropNonIpMcEnable", 2, cpssDxChBrgGenDropIpMcEnable_PARAMS, NULL},
    {"cpssDxChBrgGenDropNonIpMcEnableGet", 2, cpssDxChBrgGenDropIpMcEnableGet_PARAMS, NULL},
    {"cpssDxChBrgGenDropInvalidSaEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenDropInvalidSaEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenUcLocalSwitchingEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenUcLocalSwitchingEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenIgmpSnoopModeSet", 2, cpssDxChBrgGenIgmpSnoopModeSet_PARAMS, NULL},
    {"cpssDxChBrgGenIgmpSnoopModeGet", 2, cpssDxChBrgGenIgmpSnoopModeGet_PARAMS, NULL},
    {"cpssDxChBrgGenArpBcastToCpuCmdSet", 3, cpssDxChBrgGenArpBcastToCpuCmdSet_PARAMS, NULL},
    {"cpssDxChBrgGenArpBcastToCpuCmdGet", 3, cpssDxChBrgGenArpBcastToCpuCmdGet_PARAMS, NULL},
    {"cpssDxChBrgGenRipV1MirrorToCpuEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenRipV1MirrorToCpuEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenIeeeReservedMcastTrapEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenIeeeReservedMcastTrapEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenIeeeReservedMcastProtCmdSet", 4, cpssDxChBrgGenIeeeReservedMcastProtCmdSet_PARAMS, NULL},
    {"cpssDxChBrgGenIeeeReservedMcastProtCmdGet", 4, cpssDxChBrgGenIeeeReservedMcastProtCmdGet_PARAMS, NULL},
    {"cpssDxChBrgGenCiscoL2ProtCmdSet", 2, cpssDxChBrgGenCiscoL2ProtCmdSet_PARAMS, NULL},
    {"cpssDxChBrgGenCiscoL2ProtCmdGet", 2, cpssDxChBrgGenCiscoL2ProtCmdGet_PARAMS, NULL},
    {"cpssDxChBrgGenIpV6IcmpTrapEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenIpV6IcmpTrapEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenIcmpv6MsgTypeSet", 4, cpssDxChBrgGenIcmpv6MsgTypeSet_PARAMS, NULL},
    {"cpssDxChBrgGenIcmpv6MsgTypeGet", 4, cpssDxChBrgGenIcmpv6MsgTypeGet_PARAMS, NULL},
    {"cpssDxChBrgGenIpLinkLocalMirrorToCpuEnable", 3, prvCpssLogGenDevNumProtocolStackEnable_PARAMS, NULL},
    {"cpssDxChBrgGenIpLinkLocalMirrorToCpuEnableGet", 3, prvCpssLogGenDevNumProtocolStackEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable", 4, cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable_PARAMS, NULL},
    {"cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnableGet", 4, cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnableGet_PARAMS, NULL},
    {"cpssDxChBrgGenIpV6SolicitedCmdSet", 2, cpssDxChBrgGenCiscoL2ProtCmdSet_PARAMS, NULL},
    {"cpssDxChBrgGenIpV6SolicitedCmdGet", 2, cpssDxChBrgGenCiscoL2ProtCmdGet_PARAMS, NULL},
    {"cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixSet", 3, cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixSet_PARAMS, NULL},
    {"cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixGet", 3, cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixGet_PARAMS, NULL},
    {"cpssDxChBrgGenRateLimitGlobalCfgSet", 2, cpssDxChBrgGenRateLimitGlobalCfgSet_PARAMS, NULL},
    {"cpssDxChBrgGenRateLimitGlobalCfgGet", 2, cpssDxChBrgGenRateLimitGlobalCfgGet_PARAMS, NULL},
    {"cpssDxChBrgGenPortRateLimitSet", 3, cpssDxChBrgGenPortRateLimitSet_PARAMS, NULL},
    {"cpssDxChBrgGenPortRateLimitGet", 3, cpssDxChBrgGenPortRateLimitGet_PARAMS, NULL},
    {"cpssDxChBrgGenPortRateLimitTcpSynSet", 3, prvCpssLogGenDevNumPortEnable_PARAMS, NULL},
    {"cpssDxChBrgGenPortRateLimitTcpSynGet", 3, prvCpssLogGenDevNumPortEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenUdpBcDestPortCfgSet", 5, cpssDxChBrgGenUdpBcDestPortCfgSet_PARAMS, NULL},
    {"cpssDxChBrgGenUdpBcDestPortCfgGet", 6, cpssDxChBrgGenUdpBcDestPortCfgGet_PARAMS, NULL},
    {"cpssDxChBrgGenUdpBcDestPortCfgInvalidate", 2, prvCpssLogGenDevNumEntryIndex_PARAMS, NULL},
    {"cpssDxChBrgGenBpduTrapEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenBpduTrapEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenArpTrapEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenArpTrapEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenDropIpMcModeSet", 2, cpssDxChBrgGenDropIpMcModeSet_PARAMS, NULL},
    {"cpssDxChBrgGenDropIpMcModeGet", 2, cpssDxChBrgGenDropIpMcModeGet_PARAMS, NULL},
    {"cpssDxChBrgGenDropNonIpMcModeSet", 2, cpssDxChBrgGenDropIpMcModeSet_PARAMS, NULL},
    {"cpssDxChBrgGenDropNonIpMcModeGet", 2, cpssDxChBrgGenDropIpMcModeGet_PARAMS, NULL},
    {"cpssDxChBrgGenCfiRelayEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenCfiRelayEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet", 3, cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet_PARAMS, NULL},
    {"cpssDxChBrgGenPortIeeeReservedMcastProfileIndexGet", 3, cpssDxChBrgGenPortIeeeReservedMcastProfileIndexGet_PARAMS, NULL},
    {"cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenPortOversizeUntaggedPacketsFilterCmdSet", 2, prvCpssLogGenDevNumCommand_PARAMS, NULL},
    {"cpssDxChBrgGenPortOversizeUntaggedPacketsFilterCmdGet", 2, prvCpssLogGenDevNumCommandPtr_PARAMS, NULL},
    {"cpssDxChBrgGenRateLimitSpeedCfgEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenRateLimitSpeedCfgEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenPortRateLimitSpeedGranularitySet", 3, cpssDxChBrgGenPortRateLimitSpeedGranularitySet_PARAMS, NULL},
    {"cpssDxChBrgGenPortRateLimitSpeedGranularityGet", 3, cpssDxChBrgGenPortRateLimitSpeedGranularityGet_PARAMS, NULL},
    {"cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableSet", 3, prvCpssLogGenDevNumProtocolStackEnable_PARAMS, NULL},
    {"cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableGet", 3, prvCpssLogGenDevNumProtocolStackEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenPortIeeeReservedMcastLearningEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenPortIeeeReservedMcastLearningEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenPortRateLimitDropCntrEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChBrgGenPortRateLimitDropCntrEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChBrgGenRateLimitDropCntrSet", 2, cpssDxChBrgGenRateLimitDropCntrSet_PARAMS, NULL},
    {"cpssDxChBrgGenRateLimitDropCntrGet", 2, cpssDxChBrgGenRateLimitDropCntrGet_PARAMS, NULL},
    {"cpssDxChBrgGenPortArpMacSaMismatchDropEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenPortArpMacSaMismatchDropEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandSet", 3, cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandSet_PARAMS, NULL},
    {"cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandGet", 3, cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandGet_PARAMS, NULL},
    {"cpssDxChBrgGenIngressPortUnregisteredMcFilterDaCommandSet", 3, cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandSet_PARAMS, NULL},
    {"cpssDxChBrgGenIngressPortUnregisteredMcFilterDaCommandGet", 3, cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandGet_PARAMS, NULL},
    {"cpssDxChBrgGenIngressPortBcFilterDaCommandSet", 3, cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandSet_PARAMS, NULL},
    {"cpssDxChBrgGenIngressPortBcFilterDaCommandGet", 3, cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandGet_PARAMS, NULL},
    {"cpssDxChBrgGenPortIpControlTrapEnableSet", 3, cpssDxChBrgGenPortIpControlTrapEnableSet_PARAMS, NULL},
    {"cpssDxChBrgGenPortIpControlTrapEnableGet", 3, cpssDxChBrgGenPortIpControlTrapEnableGet_PARAMS, NULL},
    {"cpssDxChBrgGenPortBcUdpTrapMirrorEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgGenPortBcUdpTrapMirrorEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgGenBypassModeSet", 2, cpssDxChBrgGenBypassModeSet_PARAMS, NULL},
    {"cpssDxChBrgGenBypassModeGet", 2, cpssDxChBrgGenBypassModeGet_PARAMS, NULL},
    {"cpssDxChBrgGenMtuConfigSet", 5, cpssDxChBrgGenMtuConfigSet_PARAMS, NULL},
    {"cpssDxChBrgGenMtuConfigGet", 5, cpssDxChBrgGenMtuConfigGet_PARAMS, NULL},
    {"cpssDxChBrgGenMtuPortProfileIdxSet", 3, cpssDxChBrgGenMtuPortProfileIdxSet_PARAMS, NULL},
    {"cpssDxChBrgGenMtuPortProfileIdxGet", 3, cpssDxChBrgGenMtuPortProfileIdxGet_PARAMS, NULL},
    {"cpssDxChBrgGenMtuExceedCntrGet", 2, cpssDxChBrgGenMtuExceedCntrGet_PARAMS, NULL},
    {"cpssDxChBrgGenMtuProfileSet", 3, cpssDxChBrgGenMtuProfileSet_PARAMS, NULL},
    {"cpssDxChBrgGenMtuProfileGet", 3, cpssDxChBrgGenMtuProfileGet_PARAMS, NULL},
    {"cpssDxChBrgGenIpmClassificationModeSet", 2, cpssDxChBrgGenIpmClassificationModeSet_PARAMS, NULL},
    {"cpssDxChBrgGenIpmClassificationModeGet", 2, cpssDxChBrgGenIpmClassificationModeGet_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpIndexBaseEportSet", 2, cpssDxChBrgL2EcmpIndexBaseEportSet_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpIndexBaseEportGet", 2, cpssDxChBrgL2EcmpIndexBaseEportGet_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpLttTableSet", 3, cpssDxChBrgL2EcmpLttTableSet_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpLttTableGet", 3, cpssDxChBrgL2EcmpLttTableGet_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpTableSet", 3, cpssDxChBrgL2EcmpTableSet_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpTableGet", 3, cpssDxChBrgL2EcmpTableGet_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpMemberSelectionModeSet", 2, prvCpssLogGenDevNumSelectionMode_PARAMS, NULL},
    {"cpssDxChBrgL2EcmpMemberSelectionModeGet", 2, prvCpssLogGenDevNumSelectionModePtr_PARAMS, NULL},
    {"cpssDxChBrgMcInit", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChBrgMcIpv6BytesSelectSet", 3, cpssDxChBrgMcIpv6BytesSelectSet_PARAMS, cpssDxChBrgMcIpv6BytesSelectSet_preLogic},
    {"cpssDxChBrgMcIpv6BytesSelectGet", 3, cpssDxChBrgMcIpv6BytesSelectGet_PARAMS, cpssDxChBrgMcIpv6BytesSelectSet_preLogic},
    {"cpssDxChBrgMcEntryWrite", 3, cpssDxChBrgMcEntryWrite_PARAMS, NULL},
    {"cpssDxChBrgMcEntryRead", 3, cpssDxChBrgMcEntryRead_PARAMS, NULL},
    {"cpssDxChBrgMcGroupDelete", 2, cpssDxChBrgMcGroupDelete_PARAMS, NULL},
    {"cpssDxChBrgMcMemberAdd", 3, cpssDxChBrgMcMemberAdd_PARAMS, NULL},
    {"cpssDxChBrgMcMemberDelete", 3, cpssDxChBrgMcMemberAdd_PARAMS, NULL},
    {"cpssDxChBrgMcPhysicalSourceFilteringEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgMcPhysicalSourceFilteringEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgMcPhysicalSourceFilteringIgnoreSet", 3, cpssDxChBrgMcPhysicalSourceFilteringIgnoreSet_PARAMS, NULL},
    {"cpssDxChBrgMcPhysicalSourceFilteringIgnoreGet", 3, cpssDxChBrgMcPhysicalSourceFilteringIgnoreGet_PARAMS, NULL},
    {"cpssDxChBrgNestVlanAccessPortSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgNestVlanAccessPortGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgNestVlanEtherTypeSelectSet", 3, cpssDxChBrgNestVlanEtherTypeSelectSet_PARAMS, NULL},
    {"cpssDxChBrgNestVlanEtherTypeSelectGet", 3, cpssDxChBrgNestVlanEtherTypeSelectGet_PARAMS, NULL},
    {"cpssDxChBrgPeEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgPeEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgPePortEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChBrgPePortEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChBrgPePortPcidMcFilterEnableSet", 4, cpssDxChBrgPePortPcidMcFilterEnableSet_PARAMS, NULL},
    {"cpssDxChBrgPePortPcidMcFilterEnableGet", 4, cpssDxChBrgPePortPcidMcFilterEnableGet_PARAMS, NULL},
    {"cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet", 4, cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet_PARAMS, NULL},
    {"cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet", 4, cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet_PARAMS, NULL},
    {"cpssDxChBrgPrvEdgeVlanEnable", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgPrvEdgeVlanEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgPrvEdgeVlanPortEnable", 6, cpssDxChBrgPrvEdgeVlanPortEnable_PARAMS, NULL},
    {"cpssDxChBrgPrvEdgeVlanPortEnableGet", 6, cpssDxChBrgPrvEdgeVlanPortEnableGet_PARAMS, NULL},
    {"cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachPortVlanDropCntrModeSet", 2, cpssDxChBrgSecurBreachPortVlanDropCntrModeSet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachPortVlanDropCntrModeGet", 2, cpssDxChBrgSecurBreachPortVlanDropCntrModeGet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachGlobalDropCntrGet", 2, cpssDxChBrgSecurBreachGlobalDropCntrGet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachPortVlanCntrGet", 2, cpssDxChBrgSecurBreachGlobalDropCntrGet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachNaPerPortSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachNaPerPortGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachMovedStaticAddrSet", 2, cpssDxChBrgVlanEgressFilteringEnable_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachMovedStaticAddrGet", 2, cpssDxChBrgVlanEgressFilteringEnableGet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachEventDropModeSet", 3, cpssDxChBrgSecurBreachEventDropModeSet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachEventDropModeGet", 3, cpssDxChBrgSecurBreachEventDropModeGet_PARAMS, NULL},
    {"cpssDxChSecurBreachMsgGet", 2, cpssDxChSecurBreachMsgGet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet", 3, cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet", 3, cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachEventPacketCommandSet", 3, cpssDxChBrgSecurBreachEventPacketCommandSet_PARAMS, NULL},
    {"cpssDxChBrgSecurBreachEventPacketCommandGet", 3, cpssDxChBrgSecurBreachEventPacketCommandGet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdGroupPortAdd", 3, cpssDxChBrgSrcIdGroupPortAdd_PARAMS, NULL},
    {"cpssDxChBrgSrcIdGroupPortDelete", 3, cpssDxChBrgSrcIdGroupPortAdd_PARAMS, NULL},
    {"cpssDxChBrgSrcIdGroupEntrySet", 4, cpssDxChBrgSrcIdGroupEntrySet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdGroupEntryGet", 4, cpssDxChBrgSrcIdGroupEntryGet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdPortDefaultSrcIdSet", 3, cpssDxChBrgSrcIdPortDefaultSrcIdSet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdPortDefaultSrcIdGet", 3, cpssDxChBrgSrcIdPortDefaultSrcIdGet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdGlobalUcastEgressFilterSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgSrcIdGlobalUcastEgressFilterGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet", 2, cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet", 2, cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdPortSrcIdAssignModeSet", 3, cpssDxChBrgSrcIdPortSrcIdAssignModeSet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdPortSrcIdAssignModeGet", 3, cpssDxChBrgSrcIdPortSrcIdAssignModeGet_PARAMS, NULL},
    {"cpssDxChBrgSrcIdPortUcastEgressFilterSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChBrgSrcIdPortUcastEgressFilterGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChBrgSrcIdPortSrcIdForceEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgSrcIdPortSrcIdForceEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgStpInit", 1, cpssDxChBrgStpInit_PARAMS, NULL},
    {"cpssDxChBrgStpStateSet", 4, cpssDxChBrgStpStateSet_PARAMS, NULL},
    {"cpssDxChBrgStpStateGet", 4, cpssDxChBrgStpStateGet_PARAMS, NULL},
    {"cpssDxChBrgStpEntryGet", 3, cpssDxChBrgStpEntryGet_PARAMS, cpssDxChBrgStpEntryGet_preLogic},
    {"cpssDxChBrgStpEntryWrite", 3, cpssDxChBrgStpEntryWrite_PARAMS, cpssDxChBrgStpEntryGet_preLogic},
    {"cpssDxChBrgStpPortSpanningTreeStateSet", 3, cpssDxChBrgStpPortSpanningTreeStateSet_PARAMS, NULL},
    {"cpssDxChBrgStpPortSpanningTreeStateGet", 3, cpssDxChBrgStpPortSpanningTreeStateGet_PARAMS, NULL},
    {"cpssDxChBrgStpPortSpanningTreeStateModeSet", 3, cpssDxChBrgStpPortSpanningTreeStateModeSet_PARAMS, NULL},
    {"cpssDxChBrgStpPortSpanningTreeStateModeGet", 3, cpssDxChBrgStpPortSpanningTreeStateModeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanInit", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChBrgVlanEntryWrite", 6, cpssDxChBrgVlanEntryWrite_PARAMS, NULL},
    {"cpssDxChBrgVlanEntriesRangeWrite", 7, cpssDxChBrgVlanEntriesRangeWrite_PARAMS, NULL},
    {"cpssDxChBrgVlanEntryRead", 7, cpssDxChBrgVlanEntryRead_PARAMS, NULL},
    {"cpssDxChBrgVlanMemberAdd", 5, cpssDxChBrgVlanMemberAdd_PARAMS, NULL},
    {"cpssDxChBrgVlanPortDelete", 3, cpssDxChBrgVlanPortDelete_PARAMS, NULL},
    {"cpssDxChBrgVlanMemberSet", 6, cpssDxChBrgVlanMemberSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVidGet", 4, cpssDxChBrgVlanPortVidGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVidSet", 4, cpssDxChBrgVlanPortVidSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortIngFltEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortIngFltEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortProtoClassVlanEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortProtoClassVlanEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortProtoClassQosEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortProtoClassQosEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanProtoClassSet", 4, cpssDxChBrgVlanProtoClassSet_PARAMS, NULL},
    {"cpssDxChBrgVlanProtoClassGet", 5, cpssDxChBrgVlanProtoClassGet_PARAMS, NULL},
    {"cpssDxChBrgVlanProtoClassInvalidate", 2, cpssDxChBrgVlanProtoClassInvalidate_PARAMS, NULL},
    {"cpssDxChBrgVlanPortProtoVlanQosSet", 5, cpssDxChBrgVlanPortProtoVlanQosSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortProtoVlanQosGet", 6, cpssDxChBrgVlanPortProtoVlanQosGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortProtoVlanQosInvalidate", 3, cpssDxChBrgVlanPortProtoVlanQosInvalidate_PARAMS, NULL},
    {"cpssDxChBrgVlanLearningStateSet", 3, cpssDxChBrgVlanLearningStateSet_PARAMS, NULL},
    {"cpssDxChBrgVlanIsDevMember", 2, cpssDxChBrgVlanIsDevMember_PARAMS, NULL},
    {"cpssDxChBrgVlanToStpIdBind", 3, cpssDxChBrgVlanToStpIdBind_PARAMS, NULL},
    {"cpssDxChBrgVlanStpIdGet", 3, cpssDxChBrgVlanStpIdGet_PARAMS, NULL},
    {"cpssDxChBrgVlanEtherTypeSet", 4, cpssDxChBrgVlanEtherTypeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanEtherTypeGet", 4, cpssDxChBrgVlanEtherTypeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanTableInvalidate", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChBrgVlanEntryInvalidate", 2, cpssDxChBrgVlanIsDevMember_PARAMS, NULL},
    {"cpssDxChBrgVlanUnkUnregFilterSet", 4, cpssDxChBrgVlanUnkUnregFilterSet_PARAMS, NULL},
    {"cpssDxChBrgVlanForcePvidEnable", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanForcePvidEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVidPrecedenceSet", 3, cpssDxChBrgVlanPortVidPrecedenceSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVidPrecedenceGet", 3, cpssDxChBrgVlanPortVidPrecedenceGet_PARAMS, NULL},
    {"cpssDxChBrgVlanIpUcRouteEnable", 4, cpssDxChBrgVlanIpUcRouteEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanIpMcRouteEnable", 4, cpssDxChBrgVlanIpUcRouteEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanNASecurEnable", 3, cpssDxChBrgVlanNASecurEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanIgmpSnoopingEnable", 3, cpssDxChBrgVlanNASecurEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanIgmpSnoopingEnableGet", 3, cpssDxChBrgVlanIgmpSnoopingEnableGet_PARAMS, NULL},
    {"cpssDxChBrgVlanIpCntlToCpuSet", 3, cpssDxChBrgVlanIpCntlToCpuSet_PARAMS, NULL},
    {"cpssDxChBrgVlanIpCntlToCpuGet", 3, cpssDxChBrgVlanIpCntlToCpuGet_PARAMS, NULL},
    {"cpssDxChBrgVlanIpV6IcmpToCpuEnable", 3, cpssDxChBrgVlanNASecurEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanIpV6IcmpToCpuEnableGet", 3, cpssDxChBrgVlanIgmpSnoopingEnableGet_PARAMS, NULL},
    {"cpssDxChBrgVlanUdpBcPktsToCpuEnable", 3, cpssDxChBrgVlanNASecurEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanUdpBcPktsToCpuEnableGet", 3, cpssDxChBrgVlanIgmpSnoopingEnableGet_PARAMS, NULL},
    {"cpssDxChBrgVlanIpv6SourceSiteIdSet", 3, cpssDxChBrgVlanIpv6SourceSiteIdSet_PARAMS, NULL},
    {"cpssDxChBrgVlanIpmBridgingEnable", 4, cpssDxChBrgVlanIpmBridgingEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanIpmBridgingModeSet", 4, cpssDxChBrgVlanIpmBridgingModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanIngressMirrorEnable", 4, cpssDxChBrgVlanIngressMirrorEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortAccFrameTypeSet", 3, cpssDxChBrgVlanPortAccFrameTypeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortAccFrameTypeGet", 3, cpssDxChBrgVlanPortAccFrameTypeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanMruProfileIdxSet", 3, cpssDxChBrgVlanMruProfileIdxSet_PARAMS, NULL},
    {"cpssDxChBrgVlanMruProfileValueSet", 3, cpssDxChBrgVlanMruProfileValueSet_PARAMS, NULL},
    {"cpssDxChBrgVlanMruProfileValueGet", 3, cpssDxChBrgVlanMruProfileValueGet_PARAMS, NULL},
    {"cpssDxChBrgVlanNaToCpuEnable", 3, cpssDxChBrgVlanNASecurEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanVrfIdSet", 3, cpssDxChBrgVlanVrfIdSet_PARAMS, NULL},
    {"cpssDxChBrgVlanBridgingModeSet", 2, cpssDxChBrgVlanBridgingModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanBridgingModeGet", 2, cpssDxChBrgVlanBridgingModeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortTranslationEnableSet", 4, cpssDxChBrgVlanPortTranslationEnableSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortTranslationEnableGet", 4, cpssDxChBrgVlanPortTranslationEnableGet_PARAMS, NULL},
    {"cpssDxChBrgVlanTranslationEntryWrite", 4, cpssDxChBrgVlanTranslationEntryWrite_PARAMS, NULL},
    {"cpssDxChBrgVlanTranslationEntryRead", 4, cpssDxChBrgVlanTranslationEntryRead_PARAMS, NULL},
    {"cpssDxChBrgVlanValidCheckEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanValidCheckEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanTpidEntrySet", 4, cpssDxChBrgVlanTpidEntrySet_PARAMS, NULL},
    {"cpssDxChBrgVlanTpidEntryGet", 4, cpssDxChBrgVlanTpidEntryGet_PARAMS, NULL},
    {"cpssDxChBrgVlanIngressTpidProfileSet", 4, cpssDxChBrgVlanIngressTpidProfileSet_PARAMS, NULL},
    {"cpssDxChBrgVlanIngressTpidProfileGet", 4, cpssDxChBrgVlanIngressTpidProfileGet_PARAMS, NULL},
    {"cpssDxChBrgVlanTpidTagTypeSet", 4, cpssDxChBrgVlanTpidTagTypeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanTpidTagTypeGet", 4, cpssDxChBrgVlanTpidTagTypeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortIngressTpidProfileSet", 5, cpssDxChBrgVlanPortIngressTpidProfileSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortIngressTpidProfileGet", 5, cpssDxChBrgVlanPortIngressTpidProfileGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortIngressTpidSet", 4, cpssDxChBrgVlanPortIngressTpidSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortIngressTpidGet", 4, cpssDxChBrgVlanPortIngressTpidGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortEgressTpidSet", 4, cpssDxChBrgVlanPortEgressTpidSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortEgressTpidGet", 4, cpssDxChBrgVlanPortEgressTpidGet_PARAMS, NULL},
    {"cpssDxChBrgVlanEgressTagTpidSelectModeSet", 4, cpssDxChBrgVlanEgressTagTpidSelectModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanEgressTagTpidSelectModeGet", 4, cpssDxChBrgVlanEgressTagTpidSelectModeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanRangeSet", 2, cpssDxChBrgVlanRangeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanRangeGet", 2, cpssDxChBrgVlanRangeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortIsolationCmdSet", 3, cpssDxChBrgVlanPortIsolationCmdSet_PARAMS, NULL},
    {"cpssDxChBrgVlanLocalSwitchingEnableSet", 4, cpssDxChBrgVlanLocalSwitchingEnableSet_PARAMS, NULL},
    {"cpssDxChBrgVlanMirrorToTxAnalyzerSet", 4, cpssDxChBrgVlanMirrorToTxAnalyzerSet_PARAMS, NULL},
    {"cpssDxChBrgVlanFloodVidxModeSet", 4, cpssDxChBrgVlanFloodVidxModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet", 2, cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet", 2, cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanForceNewDsaToCpuEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanForceNewDsaToCpuEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanKeepVlan1EnableSet", 4, cpssDxChBrgVlanKeepVlan1EnableSet_PARAMS, NULL},
    {"cpssDxChBrgVlanKeepVlan1EnableGet", 4, cpssDxChBrgVlanKeepVlan1EnableGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet", 4, cpssDxChBrgVlanIngressMirrorEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet", 4, cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet_PARAMS, NULL},
    {"cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet", 4, cpssDxChBrgVlanIngressMirrorEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet", 4, cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet_PARAMS, NULL},
    {"cpssDxChBrgVlanForwardingIdSet", 3, cpssDxChBrgVlanForwardingIdSet_PARAMS, NULL},
    {"cpssDxChBrgVlanForwardingIdGet", 3, cpssDxChBrgVlanForwardingIdGet_PARAMS, NULL},
    {"cpssDxChBrgVlanUnknownMacSaCommandSet", 3, cpssDxChBrgVlanUnknownMacSaCommandSet_PARAMS, NULL},
    {"cpssDxChBrgVlanUnknownMacSaCommandGet", 3, cpssDxChBrgVlanUnknownMacSaCommandGet_PARAMS, NULL},
    {"cpssDxChBrgVlanEgressPortTagStateModeSet", 3, cpssDxChBrgVlanEgressPortTagStateModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanEgressPortTagStateModeGet", 3, cpssDxChBrgVlanEgressPortTagStateModeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanEgressPortTagStateSet", 3, cpssDxChBrgVlanEgressPortTagStateSet_PARAMS, NULL},
    {"cpssDxChBrgVlanEgressPortTagStateGet", 3, cpssDxChBrgVlanEgressPortTagStateGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortPushVlanCommandSet", 3, cpssDxChBrgVlanPortPushVlanCommandSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortPushVlanCommandGet", 3, cpssDxChBrgVlanPortPushVlanCommandGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet", 3, prvCpssLogGenDevNumPortNumTpidEntryIndex_PARAMS, NULL},
    {"cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet", 3, prvCpssLogGenDevNumPortNumTpidEntryIndexPtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortPushedTagValueSet", 3, cpssDxChBrgVlanPortPushedTagValueSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortPushedTagValueGet", 3, cpssDxChBrgVlanPortPushedTagValueGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUpSet", 3, cpssDxChBrgVlanPortUpSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUpGet", 3, cpssDxChBrgVlanPortUpGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortCfiEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortCfiEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortNumOfTagWordsToPopSet", 3, cpssDxChBrgVlanPortNumOfTagWordsToPopSet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortNumOfTagWordsToPopGet", 3, cpssDxChBrgVlanPortNumOfTagWordsToPopGet_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVid0CommandEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVid0CommandEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVid1CommandEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVid1CommandEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUp0CommandEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUp0CommandEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUp1CommandEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUp1CommandEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVid1Set", 4, cpssDxChBrgVlanPortVid1Set_PARAMS, NULL},
    {"cpssDxChBrgVlanPortVid1Get", 4, cpssDxChBrgVlanPortVid1Get_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUp0Set", 3, cpssDxChBrgVlanPortUp0Set_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUp0Get", 3, cpssDxChBrgVlanPortUp0Get_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUp1Set", 3, cpssDxChBrgVlanPortUp1Set_PARAMS, NULL},
    {"cpssDxChBrgVlanPortUp1Get", 3, cpssDxChBrgVlanPortUp1Get_PARAMS, NULL},
    {"cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanIndependentNonFloodVidxEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanIndependentNonFloodVidxEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
    {"cpssDxChBrgVlanFcoeForwardingEnableSet", 3, cpssDxChBrgVlanNASecurEnable_PARAMS, NULL},
    {"cpssDxChBrgVlanUnregisteredIpmEVidxSet", 4, cpssDxChBrgVlanUnregisteredIpmEVidxSet_PARAMS, NULL},
    {"cpssDxChBrgVlanMembersTableIndexingModeSet", 3, cpssDxChBrgVlanMembersTableIndexingModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanMembersTableIndexingModeGet", 3, cpssDxChBrgVlanMembersTableIndexingModeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanStgIndexingModeSet", 3, cpssDxChBrgVlanMembersTableIndexingModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanStgIndexingModeGet", 3, cpssDxChBrgVlanMembersTableIndexingModeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanTagStateIndexingModeSet", 3, cpssDxChBrgVlanTagStateIndexingModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlanTagStateIndexingModeGet", 3, cpssDxChBrgVlanTagStateIndexingModeGet_PARAMS, NULL},
    {"cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet", 3, cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet_PARAMS, NULL},
    {"cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet", 3, cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet_PARAMS, NULL},
    {"cpssDxChBrgVlanBpeTagMcCfgSet", 4, cpssDxChBrgVlanBpeTagMcCfgSet_PARAMS, NULL},
    {"cpssDxChBrgVlanBpeTagMcCfgGet", 4, cpssDxChBrgVlanBpeTagMcCfgGet_PARAMS, NULL},
    {"cpssDxChBrgVlanBpeTagReservedFieldsSet", 4, cpssDxChBrgVlanBpeTagReservedFieldsSet_PARAMS, NULL},
    {"cpssDxChBrgVlanBpeTagReservedFieldsGet", 4, cpssDxChBrgVlanBpeTagReservedFieldsGet_PARAMS, NULL},
    {"cpssDxChBrgVlanFdbLookupKeyModeSet", 3, cpssDxChBrgVlanFdbLookupKeyModeSet_PARAMS, NULL},
    {"cpssDxChBrgVlan6BytesTagConfigSet", 4, cpssDxChBrgVlan6BytesTagConfigSet_PARAMS, NULL},
    {"cpssDxChBrgVlan6BytesTagConfigGet", 4, cpssDxChBrgVlan6BytesTagConfigGet_PARAMS, NULL},
    {"cpssDxChBrgFdbHashCalc", 3, cpssDxChBrgFdbHashCalc_PARAMS, NULL},
    {"cpssDxChBrgFdbHashByParamsCalc", 4, cpssDxChBrgFdbHashByParamsCalc_PARAMS, NULL},
    {"cpssDxChBrgFdbHashCrcMultiResultsCalc", 5, cpssDxChBrgFdbHashCrcMultiResultsCalc_PARAMS, cpssDxChBrgFdbHashCrcMultiResultsCalc_preLogic},
    {"cpssDxChBrgFdbHashCrcMultiResultsByParamsCalc", 5, cpssDxChBrgFdbHashCrcMultiResultsByParamsCalc_PARAMS, cpssDxChBrgFdbHashCrcMultiResultsByParamsCalc_preLogic},
    {"cpssDxChBrgFdbHashRequestSend", 2, cpssDxChBrgFdbHashRequestSend_PARAMS, NULL},
    {"cpssDxChBrgFdbHashResultsGet", 6, cpssDxChBrgFdbHashResultsGet_PARAMS, cpssDxChBrgFdbHashResultsGet_preLogic},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_BRIDGE(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChBridgeLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChBridgeLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

