/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenBrgLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>
#include <cpss/generic/bridge/cpssGenBrgSecurityBreachTypes.h>
#include <cpss/generic/bridge/cpssGenBrgSrcId.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/bridge/private/prvCpssGenBrgLog.h>
#include <cpss/generic/ip/private/prvCpssGenIpLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_BRG_IPM_MODE_ENT[]  =
{
    "CPSS_BRG_IPM_SGV_E",
    "CPSS_BRG_IPM_GV_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_BRG_IPM_MODE_ENT);
char * prvCpssLogEnum_CPSS_BRG_MODE_ENT[]  =
{
    "CPSS_BRG_MODE_802_1Q_E",
    "CPSS_BRG_MODE_802_1D_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_BRG_MODE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_BRG_SECUR_BREACH_EVENTS_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_ARP_SA_MISMATCH_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_OVER_MC_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_BRIDGE_ACCESS_MATRIX_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_SECURE_AUTO_LEARN_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_FRAGMENT_ICMP_TYPE_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FIN_URG_PSH_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_FIN_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_RST_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_UDP_SRC_DST_PORT_ZERO),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_AUTO_LEARN_NO_RELEARN_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_ENT);
char * prvCpssLogEnum_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT[]  =
{
    "CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E",
    "CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E",
    "CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT);
char * prvCpssLogEnum_CPSS_BRG_TPID_SIZE_TYPE_ENT[]  =
{
    "CPSS_BRG_TPID_SIZE_TYPE_4_BYTES_E",
    "CPSS_BRG_TPID_SIZE_TYPE_8_BYTES_E",
    "CPSS_BRG_TPID_SIZE_TYPE_6_BYTES_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_BRG_TPID_SIZE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_BRIDGE_INGR_CNTR_MODES_ENT[]  =
{
    "CPSS_BRG_CNT_MODE_0_E",
    "CPSS_BRG_CNT_MODE_1_E",
    "CPSS_BRG_CNT_MODE_2_E",
    "CPSS_BRG_CNT_MODE_3_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_BRIDGE_INGR_CNTR_MODES_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_ETHER_MODE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_VLAN_ETHERTYPE0_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_VLAN_ETHERTYPE1_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_ETHER_MODE_ENT);
char * prvCpssLogEnum_CPSS_FDB_ACTION_MODE_ENT[]  =
{
    "CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E",
    "CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E",
    "CPSS_FDB_ACTION_DELETING_E",
    "CPSS_FDB_ACTION_TRANSPLANTING_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_FDB_ACTION_MODE_ENT);
char * prvCpssLogEnum_CPSS_IGMP_SNOOP_MODE_ENT[]  =
{
    "CPSS_IGMP_ALL_TRAP_MODE_E",
    "CPSS_IGMP_SNOOP_TRAP_MODE_E",
    "CPSS_IGMP_ROUTER_MIRROR_MODE_E",
    "CPSS_IGMP_FWD_DECISION_MODE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_IGMP_SNOOP_MODE_ENT);
char * prvCpssLogEnum_CPSS_MAC_ACTION_MODE_ENT[]  =
{
    "CPSS_ACT_AUTO_E",
    "CPSS_ACT_TRIG_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_MAC_ACTION_MODE_ENT);
char * prvCpssLogEnum_CPSS_MAC_HASH_FUNC_MODE_ENT[]  =
{
    "CPSS_MAC_HASH_FUNC_XOR_E",
    "CPSS_MAC_HASH_FUNC_CRC_E",
    "CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_MAC_HASH_FUNC_MODE_ENT);
char * prvCpssLogEnum_CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT[]  =
{
    "CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E",
    "CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E",
    "CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E",
    "CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E",
    "CPSS_MAC_SECURE_AUTO_LEARN_UNK_FORWARD_E",
    "CPSS_MAC_SECURE_AUTO_LEARN_UNK_MIRROR_TO_CPU_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_MAC_VL_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_IVL_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_SVL_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_MAC_VL_ENT);
char * prvCpssLogEnum_CPSS_PORT_ACCEPT_FRAME_TYPE_ENT[]  =
{
    "CPSS_PORT_ACCEPT_FRAME_TAGGED_E",
    "CPSS_PORT_ACCEPT_FRAME_ALL_E",
    "CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E",
    "CPSS_PORT_ACCEPT_FRAME_NONE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PORT_ACCEPT_FRAME_TYPE_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_PORT_LOCK_CMD_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_LOCK_FRWRD_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_LOCK_DROP_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_LOCK_TRAP_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_LOCK_MIRROR_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_LOCK_SOFT_DROP_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_PORT_LOCK_CMD_ENT);
char * prvCpssLogEnum_CPSS_STP_STATE_ENT[]  =
{
    "CPSS_STP_DISABLED_E",
    "CPSS_STP_BLCK_LSTN_E",
    "CPSS_STP_LRN_E",
    "CPSS_STP_FRWRD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_STP_STATE_ENT);
char * prvCpssLogEnum_CPSS_VLAN_TAG_TYPE_ENT[]  =
{
    "CPSS_VLAN_TAG0_E",
    "CPSS_VLAN_TAG1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_VLAN_TAG_TYPE_ENT);
char * prvCpssLogEnum_CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT[]  =
{
    "CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E",
    "CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_MAC_ENTRY_EXT_TYPE_ENT[]  =
{
    "CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E",
    "CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E",
    "CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E",
    "CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E",
    "CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E",
    "CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E",
    "CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E",
    "CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E",
    "CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_MAC_ENTRY_EXT_TYPE_ENT);
char * prvCpssLogEnum_CPSS_MAC_TABLE_CMD_ENT[]  =
{
    "CPSS_MAC_TABLE_FRWRD_E",
    "CPSS_MAC_TABLE_DROP_E",
    "CPSS_MAC_TABLE_INTERV_E",
    "CPSS_MAC_TABLE_CNTL_E",
    "CPSS_MAC_TABLE_MIRROR_TO_CPU_E",
    "CPSS_MAC_TABLE_SOFT_DROP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_MAC_TABLE_CMD_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_RATE_LIMIT_MODE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_RATE_LIMIT_PCKT_BASED_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_RATE_LIMIT_BYTE_BASED_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_RATE_LIMIT_MODE_ENT);
char * prvCpssLogEnum_CPSS_UPD_MSG_TYPE_ENT[]  =
{
    "CPSS_NA_E",
    "CPSS_QA_E",
    "CPSS_QR_E",
    "CPSS_AA_E",
    "CPSS_TA_E",
    "CPSS_SA_E",
    "CPSS_QI_E",
    "CPSS_FU_E",
    "CPSS_HR_E",
    "CPSS_NA_MOVED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_UPD_MSG_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dropCntMode, CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, port);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlan);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_BRG_SECUR_BREACH_MSG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_BRG_SECUR_BREACH_MSG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ETH_MAC(valPtr, macSa);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vlan);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, port);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, code, CPSS_BRG_SECUR_BREACH_EVENTS_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_BRIDGE_INGRESS_CNTR_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_BRIDGE_INGRESS_CNTR_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, gtBrgInFrames);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, gtBrgVlanIngFilterDisc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, gtBrgSecFilterDisc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, gtBrgLocalPropDisc);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_MAC_ENTRY_EXT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_MAC_ENTRY_EXT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, key, CPSS_MAC_ENTRY_EXT_KEY_STC);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, dstInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, age);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isStatic);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, daRoute);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, mirrorToRxAnalyzerPortEn);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, sourceID);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, userDefined);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, daQosIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, saQosIndex);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, daSecurityLevel);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, saSecurityLevel);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, appSpecificCpuCode);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, spUnknown);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, saMirrorToRxAnalyzerPortEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, daMirrorToRxAnalyzerPortEn);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, fdbRoutingInfo, CPSS_FDB_UC_ROUTING_INFO_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_MAC_UPDATE_MSG_EXT_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_MAC_UPDATE_MSG_EXT_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, updType, CPSS_UPD_MSG_TYPE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, entryWasFound);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, macEntryIndex);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, macEntry, CPSS_MAC_ENTRY_EXT_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, skip);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, aging);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, associatedHwDevNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, queryHwDevNum);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, naChainIsTooLong);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, entryOffset);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, portGroupId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, vid1);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, up0);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isMoved);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, oldSrcId);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, oldDstInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, oldAssociatedHwDevNum);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_PROT_CLASS_ENCAP_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_PROT_CLASS_ENCAP_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ethV2);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, nonLlcSnap);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, llcSnap);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_FDB_UC_ROUTING_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_FDB_UC_ROUTING_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
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
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dipAccessLevel);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ICMPRedirectEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, mtuProfileIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTunnelStart);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopVlanId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopARPPointer);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopTunnelPointer);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nextHopDataBankNumber);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, scopeCheckingEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, siteId, CPSS_IP_SITE_ID_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_BRG_IPM_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_IPM_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_BRG_IPM_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_BRG_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_BRG_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_BRG_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_BRG_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_BRG_SECUR_BREACH_EVENTS_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_SECUR_BREACH_EVENTS_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_BRG_SECUR_BREACH_EVENTS_ENT);
}
void prvCpssLogParamFunc_CPSS_BRG_SECUR_BREACH_MSG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_BRG_SECUR_BREACH_MSG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_BRG_SECUR_BREACH_MSG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_BRG_TPID_SIZE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_TPID_SIZE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_BRG_TPID_SIZE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_BRG_TPID_SIZE_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRG_TPID_SIZE_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_BRG_TPID_SIZE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_BRIDGE_INGRESS_CNTR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_BRIDGE_INGRESS_CNTR_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_BRIDGE_INGRESS_CNTR_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_BRIDGE_INGR_CNTR_MODES_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRIDGE_INGR_CNTR_MODES_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_BRIDGE_INGR_CNTR_MODES_ENT);
}
void prvCpssLogParamFunc_CPSS_BRIDGE_INGR_CNTR_MODES_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_BRIDGE_INGR_CNTR_MODES_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_BRIDGE_INGR_CNTR_MODES_ENT);
}
void prvCpssLogParamFunc_CPSS_ETHER_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_ETHER_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_ETHER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_ETHER_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_ETHER_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, *paramVal, CPSS_ETHER_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_FDB_ACTION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_FDB_ACTION_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_FDB_ACTION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_FDB_ACTION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_FDB_ACTION_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_FDB_ACTION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_IGMP_SNOOP_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IGMP_SNOOP_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_IGMP_SNOOP_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_IGMP_SNOOP_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_IGMP_SNOOP_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_IGMP_SNOOP_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_ACTION_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_ACTION_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_MAC_ACTION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_ACTION_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_ACTION_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_MAC_ACTION_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_MAC_ENTRY_EXT_KEY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_MAC_ENTRY_EXT_KEY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_MAC_ENTRY_EXT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_MAC_ENTRY_EXT_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_MAC_ENTRY_EXT_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_MAC_HASH_FUNC_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_HASH_FUNC_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_MAC_HASH_FUNC_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_HASH_FUNC_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_HASH_FUNC_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_MAC_HASH_FUNC_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_VL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_VL_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_MAC_VL_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_VL_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_VL_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, *paramVal, CPSS_MAC_VL_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_ACCEPT_FRAME_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_ACCEPT_FRAME_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PORT_ACCEPT_FRAME_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_ACCEPT_FRAME_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_ACCEPT_FRAME_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_PORT_ACCEPT_FRAME_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_LOCK_CMD_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_LOCK_CMD_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, paramVal, CPSS_PORT_LOCK_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_PORT_LOCK_CMD_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PORT_LOCK_CMD_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAP_MAC(namePtr, *paramVal, CPSS_PORT_LOCK_CMD_ENT);
}
void prvCpssLogParamFunc_CPSS_PROT_CLASS_ENCAP_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_PROT_CLASS_ENCAP_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_PROT_CLASS_ENCAP_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_STP_STATE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_STP_STATE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_STP_STATE_ENT);
}
void prvCpssLogParamFunc_CPSS_STP_STATE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_STP_STATE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_STP_STATE_ENT);
}
void prvCpssLogParamFunc_CPSS_VLAN_TAG_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_VLAN_TAG_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_VLAN_TAG_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_VLAN_TAG_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_VLAN_TAG_TYPE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_VLAN_TAG_TYPE_ENT);
}

