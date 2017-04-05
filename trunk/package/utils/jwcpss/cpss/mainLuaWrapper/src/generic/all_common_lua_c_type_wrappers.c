/*************************************************************************
* /local/store/yedidiaf/git/temp_embede_files/tmp_bld_cpssAPI/all_common_lua_c_type_wrappers.c
*
*       WARNING!!! this is a generated file, please don't edit it manually
*       See COMMENTS for command line
*
* DESCRIPTION:
*       A lua type wrapper
*       It implements support for the following types:
*           enum    CPSS_LED_CLASS_13_SELECT_ENT
*           struct  CPSS_TM_SHAPING_PROFILE_PARAMS_STC
*           struct  CPSS_BUFF_MGMT_CFG_STC
*           enum    CPSS_LOG_TYPE_ENT
*           struct  CPSS_GEN_DRAGONITE_CHIP_STC
*           struct  CPSS_HW_ADDR_STC
*           struct  CPSS_VLAN_INFO_STC
*           struct  CPSS_VCT_ACCURATE_CABLE_LEN_STC
*           enum    CPSS_HW_PP_INIT_STAGE_ENT
*           enum    CPSS_PHY_ERRATA_WA_ENT
*           enum    CPSS_IP_SITE_ID_ENT
*           enum    CPSS_PORT_REF_CLOCK_SOURCE_ENT
*           enum    CPSS_DIAG_TEST_PROFILE_ENT
*           struct  CPSS_TM_ELIG_PRIO_FUNC_OUT_STC
*           enum    CPSS_LED_CLOCK_OUT_FREQUENCY_ENT
*           enum    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC
*           struct  CPSS_TM_QUANTUM_LIMITS_STC
*           enum    CPSS_PCL_RULE_SIZE_ENT
*           struct  CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC
*           struct  CPSS_TM_PORT_STATUS_STC
*           enum    CPSS_MAC_TABLE_CMD_ENT
*           enum    CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT
*           enum    CPSS_UNI_EV_CAUSE_ENT
*           struct  CPSS_MAC_ENTRY_STC
*           enum    CPSS_TUNNEL_TYPE_ENT
*           enum    CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT
*           enum    CPSS_PORT_FLOW_CONTROL_ENT
*           struct  CPSS_TM_B_NODE_PARAMS_STC
*           enum    CPSS_BM_POOL_ALIGNMENT_ENT
*           enum    CPSS_LOG_LIB_ENT
*           enum    CPSS_VCT_ACTION_ENT
*           enum    CPSS_L4_PROTOCOL_ENT
*           struct  CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC
*           enum    CPSS_DRAM_SIZE_ENT
*           enum    CPSS_BUF_MODE_ENT
*           enum    CPSS_BRG_SECUR_BREACH_EVENTS_ENT
*           enum    CPSS_PCL_LOOKUP_NUMBER_ENT
*           enum    CPSS_PORT_LOCK_CMD_ENT
*           struct  CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC
*           enum    CPSS_LED_BLINK_DUTY_CYCLE_ENT
*           struct  CPSS_VCT_CABLE_STATUS_STC
*           enum    CPSS_MAC_QOS_RESOLVE_ENT
*           enum    CPSS_PORT_RX_FC_PROFILE_SET_ENT
*           struct  CPSS_TM_CTL_LAD_INF_PARAM_STC
*           struct  CPSS_VCT_EXTENDED_STATUS_STC
*           enum    CPSS_NST_CHECK_ENT
*           enum    CPSS_PCL_DIRECTION_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC
*           enum    CPSS_RX_BUFF_ALLOC_METHOD_ENT
*           enum    CPSS_VCT_PAIR_SWAP_ENT
*           struct  CPSS_RESOURCE_MAPPING_STC
*           enum    CPSS_GEN_DRAGONITE_CHIP_STATE_ENT
*           struct  CPSS_QOS_ENTRY_STC
*           enum    CPSS_VCT_STATE
*           enum    CPSS_CSCD_LINK_TYPE_ENT
*           enum    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT
*           enum    CPSS_TUNNEL_ETHERTYPE_TYPE_ENT
*           enum    CPSS_EVENT_MASK_SET_ENT
*           enum    CPSS_PORT_DUPLEX_ENT
*           struct  CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC
*           enum    CPSS_VCT_TEST_STATUS_ENT
*           enum    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT
*           struct  CPSS_PORT_EGRESS_CNTR_STC
*           enum    CPSS_NET_PROT_ENT
*           struct  CPSS_PORT_MAC_COUNTER_SET_STC
*           struct  CPSS_BRG_SECUR_BREACH_MSG_STC
*           enum    CPSS_VLAN_TAG_TYPE_ENT
*           struct  CPSS_PP_PORT_GROUP_INIT_INFO_STC
*           enum    CPSS_SRAM_SIZE_ENT
*           enum    CPSS_TM_ELIG_FUNC_QUEUE_ENT
*           enum    CPSS_VCT_CABLE_LEN_ENT
*           struct  CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC
*           enum    CPSS_NET_CPU_PORT_MODE_ENT
*           enum    CPSS_LED_BLINK_SELECT_ENT
*           enum    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT
*           struct  CPSS_MAC_UPDATE_MSG_STC
*           struct  CPSS_TM2TM_DELTA_RANGE_STC
*           enum    CPSS_DRAM_PAGE_SIZE_ENT
*           enum    CPSS_PHY_XSMI_INTERFACE_ENT
*           enum    CPSS_NARROW_SRAM_CFG_ENT
*           enum    CPSS_VCT_DOWNSHIFT_STATUS_ENT
*           enum    CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT
*           enum    CPSS_BRG_IPM_MODE_ENT
*           enum    CPSS_CSCD_PORT_TYPE_ENT
*           struct  CPSS_TM_QUEUE_PARAMS_STC
*           enum    CPSS_PORT_TX_WRR_MTU_ENT
*           enum    CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT
*           enum    CPSS_PORT_DIRECTION_ENT
*           enum    CPSS_SYSTEM_RECOVERY_STATE_ENT
*           enum    CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT
*           enum    CPSS_FDB_ACTION_MODE_ENT
*           enum    CPSS_VCT_NORMAL_CABLE_LEN_ENT
*           struct  CPSS_PORT_ATTRIBUTES_STC
*           enum    CPSS_GEN_DRAGONITE_PRIORITY_ENT
*           struct  CPSS_PORTS_BMP_STC
*           enum    CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT
*           struct  CPSS_TM_BAP_UNIT_ERROR_STATUS_STC
*           struct  CPSS_CSCD_LINK_TYPE_STC
*           struct  CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC
*           enum    CPSS_PORT_TX_Q_ARB_GROUP_ENT
*           struct  CPSS_GEN_DRAGONITE_POWER_BUDGET_STC
*           enum    CPSS_UPD_MSG_TYPE_ENT
*           enum    CPSS_SRAM_FREQUENCY_ENT
*           enum    CPSS_PP_INTERFACE_CHANNEL_ENT
*           enum    CPSS_IGMP_SNOOP_MODE_ENT
*           struct  CPSS_TM_LIB_INIT_PARAMS_STC
*           enum    CPSS_NST_AM_PARAM_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_LAYER2_STC
*           struct  CPSS_BRIDGE_INGRESS_CNTR_STC
*           struct  CPSS_TM2TM_EXTERNAL_HDR_STC
*           struct  CPSS_LED_CONF_STC
*           enum    CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT
*           struct  CPSS_TM_PORT_DROP_PER_COS_STC
*           enum    CPSS_GEN_DRAGONITE_DATA_TYPE_ENT
*           enum    CPSS_EXTERNAL_MEMORY_ENT
*           enum    CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT
*           enum    CPSS_DRV_HW_TRACE_TYPE_ENT
*           enum    CPSS_PP_FAMILY_BIT_ENT
*           enum    CPSS_DRAM_SPEED_BIN_ENT
*           enum    CPSS_BRG_MODE_ENT
*           struct  CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS
*           enum    CPSS_TM_COLOR_NUM_ENT
*           enum    CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT
*           enum    CPSS_POLICER_COLOR_MODE_ENT
*           enum    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT
*           struct  CPSS_VCT_CABLE_EXTENDED_STATUS_STC
*           enum    CPSS_MAC_ACTION_MODE_ENT
*           enum    CPSS_RATE_LIMIT_MODE_ENT
*           enum    CPSS_PORT_MAC_TYPE_ENT
*           struct  CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC
*           enum    CPSS_PORT_INTERFACE_MODE_ENT
*           struct  CPSS_TM_TREE_PARAMS_STC
*           struct  CPSS_GEN_DRAGONITE_SYSTEM_STC
*           struct  CPSS_PORT_MAC_STATUS_STC
*           enum    CPSS_PORT_SPEED_ENT
*           union   CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT
*           enum    CPSS_IPV6_MLL_SELECTION_RULE_ENT
*           struct  CPSS_LED_CLASS_MANIPULATION_STC
*           enum    CPSS_PHY_SMI_INTERFACE_ENT
*           enum    CPSS_PORT_SERDES_TUNING_TYPE_ENT
*           enum    CPSS_EVENT_GPP_ID_ENT
*           struct  CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC
*           struct  CPSS_SRAM_CFG_STC
*           struct  CPSS_LED_GROUP_CONF_STC
*           struct  CPSS_PORT_MODE_SPEED_STC
*           enum    CPSS_SRAM_RATE_ENT
*           enum    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT
*           enum    CPSS_MAC_HASH_FUNC_MODE_ENT
*           enum    CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT
*           enum    CPSS_LED_ORDER_MODE_ENT
*           enum    CPSS_TM_ELIG_FUNC_NODE_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_CTRL_STC
*           enum    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT
*           struct  CPSS_SYSTEM_RECOVERY_INFO_STC
*           enum    CPSS_ETHER_MODE_ENT
*           enum    CPSS_PORT_TX_DROP_PROFILE_SET_ENT
*           enum    CPSS_NST_EGRESS_FRW_FILTER_ENT
*           struct  CPSS_VCT_PAIR_SKEW_STC
*           struct  CPSS_TM_PORT_PARAMS_STC
*           enum    CPSS_IP_PROTOCOL_STACK_ENT
*           enum    CPSS_LED_BLINK_DURATION_ENT
*           struct  CPSS_TRUNK_MEMBER_STC
*           enum    CPSS_SYS_HA_MODE_ENT
*           struct  CPSS_TM_UNITS_ERROR_STATUS_STC
*           struct  CPSS_SYSTEM_RECOVERY_MODE_STC
*           enum    CPSS_PORT_PCS_RESET_MODE_ENT
*           enum    CPSS_TM_PORT_BW_ENT
*           enum    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT
*           enum    CPSS_BRIDGE_INGR_CNTR_MODES_ENT
*           enum    CPSS_POLICER_TB_MODE_ENT
*           struct  CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC
*           struct  CPSS_TM_DROP_PROFILE_PARAMS_STC
*           enum    CPSS_HSU_DATA_TYPE_ENT
*           enum    CPSS_DRAM_BUS_WIDTH_ENT
*           enum    CPSS_TUNNEL_MULTICAST_TYPE_ENT
*           enum    CPSS_LED_PULSE_STRETCH_ENT
*           enum    CPSS_DIAG_PP_MEM_TYPE_ENT
*           struct  CPSS_TM_ERROR_INFO_STC
*           enum    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT
*           enum    CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT
*           enum    CPSS_DRAM_FREQUENCY_ENT
*           struct  CPSS_VCT_CONFIG_STC
*           enum    CPSS_INTERFACE_TYPE_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC
*           enum    CPSS_UNICAST_MULTICAST_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_MATRIX_STC
*           struct  CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC
*           enum    CPSS_PACKET_CMD_ENT
*           enum    CPSS_VCT_PHY_TYPES_ENT
*           enum    CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT
*           enum    CPSS_DROP_MODE_TYPE_ENT
*           enum    CPSS_LED_CLASS_5_SELECT_ENT
*           enum    CPSS_NST_INGRESS_FRW_FILTER_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_COMMAND_STC
*           enum    CPSS_PORT_TX_WRR_MODE_ENT
*           struct  CPSS_GEN_CFG_DEV_INFO_STC
*           enum    CPSS_TX_BUFF_ALLOC_METHOD_ENT
*           enum    CPSS_PP_SUB_FAMILY_TYPE_ENT
*           struct  CPSS_VERSION_INFO_STC
*           enum    CPSS_MAC_SA_LSB_MODE_ENT
*           enum    CPSS_DRAM_TEMPERATURE_ENT
*           enum    CPSS_AU_MESSAGE_LENGTH_ENT
*           enum    CPSS_LOG_POINTER_FORMAT_ENT
*           enum    CPSS_STP_STATE_ENT
*           enum    CPSS_LOG_API_FORMAT_ENT
*           enum    CPSS_BRG_TPID_SIZE_TYPE_ENT
*           enum    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT
*           struct  CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC
*           struct  CPSS_VCT_STATUS_STC
*           enum    CPSS_TRUNK_TYPE_ENT
*           enum    CPSS_IP_UNICAST_MULTICAST_ENT
*           enum    CPSS_PP_UPLINK_CONFIG_ENT
*           enum    CPSS_MAC_VL_ENT
*           enum    CPSS_TUNNEL_PASSENGER_PACKET_ENT
*           struct  CPSS_TM_NODE_STATUS_STC
*           enum    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT
*           struct  CPSS_COS_MAP_STC
*           enum    CPSS_TM_LEVEL_ENT
*           enum    CPSS_POLICER_PACKET_SIZE_MODE_ENT
*           enum    CPSS_HW_PP_RESET_SKIP_TYPE_ENT
*           enum    CPSS_DIAG_PP_REG_TYPE_ENT
*           enum    CPSS_NET_RX_CPU_CODE_ENT
*           enum    CPSS_MEMORY_DUMP_TYPE_ENT
*           struct  CPSS_TM_C_NODE_PARAMS_STC
*           enum    CPSS_TM2TM_CHANNEL_ENT
*           struct  CPSS_MAC_ENTRY_EXT_STC
*           enum    CPSS_COMPARE_OPERATOR_ENT
*           struct  CPSS_COS_STC
*           struct  CPSS_MAC_UPDATE_MSG_EXT_STC
*           enum    CPSS_L4_PROTOCOL_PORT_TYPE_ENT
*           struct  CPSS_TM_QMR_PKT_STATISTICS_STC
*           enum    CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT
*           enum    CPSS_PORT_EGRESS_CNT_MODE_ENT
*           enum    CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT
*           enum    CPSS_SYSTEM_RECOVERY_PROCESS_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_STATUSES_STC
*           struct  CPSS_TM_LEVEL_PERIODIC_PARAMS_STC
*           struct  CPSS_FDB_HASH_OPTION_STC
*           enum    CPSS_VCT_POLARITY_SWAP_ENT
*           enum    CPSS_PORT_MAC_COUNTERS_ENT
*           enum    CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT
*           enum    CPSS_FLOW_DRAM_PARAM_ENT
*           enum    CPSS_PP_FAMILY_TYPE_ENT
*           enum    CPSS_TM_DP_SOURCE_ENT
*           enum    CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT
*           enum    CPSS_PORT_XGMII_MODE_ENT
*           enum    CPSS_DP_LEVEL_ENT
*           enum    CPSS_TM_SCHD_MODE_ENT
*           enum    CPSS_BUFFER_DRAM_PARAM_ENT
*           enum    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT
*           struct  CPSS_TM_RCB_PKT_STATISTICS_STC
*           enum    CPSS_QOS_PORT_TRUST_MODE_ENT
*           enum    CPSS_POLICER_MRU_ENT
*           enum    CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT
*           enum    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT
*           enum    CPSS_LOG_TIME_FORMAT_ENT
*           enum    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT
*           enum    CPSS_DRV_HW_ACCESS_STAGE_ENT
*           struct  CPSS_TM2TM_CNTRL_PKT_STRUCT_STC
*           struct  CPSS_REG_VALUE_INFO_STC
*           enum    CPSS_PORT_XG_FIXED_IPG_ENT
*           enum    CPSS_TM_DROP_MODE_ENT
*           enum    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT
*           struct  CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC
*           enum    CPSS_IP_CNT_SET_ENT
*           enum    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT
*           enum    CPSS_MAC_ENTRY_EXT_TYPE_ENT
*           struct  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC
*           struct  CPSS_GEN_DRAGONITE_CONFIG_STC
*           enum    CPSS_IPV6_PREFIX_SCOPE_ENT
*           enum    CPSS_SRAM_TYPE_ENT
*           struct  CPSS_TM_A_NODE_PARAMS_STC
*           enum    CPSS_TX_SDMA_QUEUE_MODE_ENT
*           enum    CPSS_DIRECTION_ENT
*           enum    CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT
*           struct  CPSS_FDB_UC_ROUTING_INFO_STC
*           struct  CPSS_PROT_CLASS_ENCAP_STC
*           struct  CPSS_TRUNK_WEIGHTED_MEMBER_STC
*
* DEPENDENCIES:
*
* COMMENTS:
*       Generated at Sun Jan 31 21:42:06 2016
*       /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss/mainLuaWrapper/scripts/make_type_wrapper.py -i /local/store/yedidiaf/git/temp_embede_files/tmp_bld_cpssAPI/type_info_file_dx -p /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss/mainLuaWrapper/scripts/already_implemented.lst -X /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss/mainLuaWrapper/scripts/excludelist -C /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss -M /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss/mainLuaWrapper/scripts/lua_C_mapping -N -T -F NONE -o /local/store/yedidiaf/git/temp_embede_files/tmp_bld_cpssAPI/all_common_lua_c_type_wrappers.c
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
**************************************************************************/
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <cpss/generic/cpssHwInit/cpssLedCtrl.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/dragonite/cpssGenDragonite.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/phy/cpssGenPhyVct.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/generic/diag/cpssDiag.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/pcl/cpssPcl.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpssCommon/cpssFormatConvert.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/tunnel/cpssGenTunnelTypes.h>
#include <cpssCommon/cpssBuffManagerPool.h>
#include <cpss/generic/bridge/cpssGenBrgSecurityBreachTypes.h>
#include <cpss/generic/port/cpssPortTx.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/tm/cpssTmCtl.h>
#include <cpss/generic/nst/cpssNstTypes.h>
#include <cpss/generic/port/cpssPortStat.h>
#include <cpss/generic/cscd/cpssGenCscd.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/policer/cpssGenPolicerTypes.h>
#include <cpss/generic/events/cpssGenEventCtrl.h>
#include <cpss/generic/bridge/cpssGenBrgSrcId.h>
#include <cpss/generic/systemRecovery/hsu/cpssGenHsu.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/generic/version/cpssGenVersion.h>
#include <cpssDriver/pp/config/generic/cpssDrvPpGenDump.h>
#include <cpss/extServices/os/gtOs/gtOs.h>

/***** declarations ********/

use_prv_enum(CPSS_LED_CLASS_13_SELECT_ENT);
use_prv_struct(CPSS_TM_SHAPING_PROFILE_PARAMS_STC);
use_prv_struct(CPSS_BUFF_MGMT_CFG_STC);
use_prv_enum(CPSS_LOG_TYPE_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_CHIP_STC);
use_prv_struct(CPSS_HW_ADDR_STC);
use_prv_struct(CPSS_VLAN_INFO_STC);
use_prv_struct(CPSS_VCT_ACCURATE_CABLE_LEN_STC);
use_prv_struct(GT_ETHERADDR);
use_prv_enum(CPSS_PHY_ERRATA_WA_ENT);
use_prv_enum(CPSS_IP_SITE_ID_ENT);
use_prv_enum(CPSS_PACKET_CMD_ENT);
use_prv_enum(CPSS_PORT_REF_CLOCK_SOURCE_ENT);
use_prv_enum(CPSS_DIAG_TEST_PROFILE_ENT);
use_prv_struct(CPSS_TM_ELIG_PRIO_FUNC_OUT_STC);
use_prv_enum(CPSS_LED_CLOCK_OUT_FREQUENCY_ENT);
use_prv_enum(CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC);
use_prv_struct(CPSS_TM_QUANTUM_LIMITS_STC);
use_prv_enum(CPSS_PCL_RULE_SIZE_ENT);
use_prv_struct(CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC);
use_prv_struct(CPSS_TM_PORT_STATUS_STC);
use_prv_enum(CPSS_MAC_TABLE_CMD_ENT);
use_prv_enum(CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT);
use_prv_enum(CPSS_UNI_EV_CAUSE_ENT);
use_prv_struct(CPSS_MAC_ENTRY_STC);
use_prv_enum(CPSS_TUNNEL_TYPE_ENT);
use_prv_enum(CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT);
use_prv_enum(CPSS_PORT_FLOW_CONTROL_ENT);
use_prv_struct(CPSS_TM_B_NODE_PARAMS_STC);
use_prv_enum(CPSS_BM_POOL_ALIGNMENT_ENT);
use_prv_enum(CPSS_LOG_LIB_ENT);
use_prv_enum(CPSS_VCT_ACTION_ENT);
use_prv_enum(CPSS_L4_PROTOCOL_ENT);
use_prv_struct(CPSS_INTERFACE_INFO_STC);
use_prv_struct(CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC);
use_prv_enum(CPSS_DRAM_SIZE_ENT);
use_prv_enum(CPSS_BUF_MODE_ENT);
use_prv_enum(CPSS_BRG_SECUR_BREACH_EVENTS_ENT);
use_prv_enum(CPSS_PCL_LOOKUP_NUMBER_ENT);
use_prv_enum(CPSS_PORT_LOCK_CMD_ENT);
use_prv_struct(CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC);
use_prv_enum(CPSS_LED_BLINK_DUTY_CYCLE_ENT);
use_prv_struct(CPSS_VCT_CABLE_STATUS_STC);
use_prv_enum(CPSS_MAC_QOS_RESOLVE_ENT);
use_prv_enum(CPSS_PORT_RX_FC_PROFILE_SET_ENT);
use_prv_struct(CPSS_TM_CTL_LAD_INF_PARAM_STC);
use_prv_struct(CPSS_VCT_EXTENDED_STATUS_STC);
use_prv_enum(CPSS_NST_CHECK_ENT);
use_prv_enum(CPSS_PCL_DIRECTION_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC);
use_prv_struct(CPSS_PORT_EGRESS_CNTR_STC);
use_prv_enum(CPSS_RX_BUFF_ALLOC_METHOD_ENT);
use_prv_enum(CPSS_VCT_PAIR_SWAP_ENT);
use_prv_struct(CPSS_RESOURCE_MAPPING_STC);
use_prv_enum(CPSS_SHIFT_OPERATION_ENT);
use_prv_enum(CPSS_GEN_DRAGONITE_CHIP_STATE_ENT);
use_prv_struct(CPSS_QOS_ENTRY_STC);
use_prv_enum(CPSS_VCT_STATE);
use_prv_enum(CPSS_CSCD_LINK_TYPE_ENT);
use_prv_enum(CPSS_NET_TCP_UDP_PACKET_TYPE_ENT);
use_prv_enum(CPSS_TUNNEL_ETHERTYPE_TYPE_ENT);
use_prv_enum(CPSS_EVENT_MASK_SET_ENT);
use_prv_enum(CPSS_PORT_DUPLEX_ENT);
use_prv_struct(CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC);
use_prv_enum(CPSS_VCT_TEST_STATUS_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC);
use_prv_enum(CPSS_NET_PROT_ENT);
use_prv_struct(CPSS_PORT_MAC_COUNTER_SET_STC);
use_prv_struct(CPSS_BRG_SECUR_BREACH_MSG_STC);
use_prv_enum(CPSS_VLAN_TAG_TYPE_ENT);
use_prv_struct(CPSS_PP_PORT_GROUP_INIT_INFO_STC);
use_prv_enum(CPSS_SRAM_SIZE_ENT);
use_prv_enum(CPSS_TM_ELIG_FUNC_QUEUE_ENT);
use_prv_enum(CPSS_VCT_CABLE_LEN_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_POWER_BUDGET_STC);
use_prv_enum(CPSS_NET_CPU_PORT_MODE_ENT);
use_prv_enum(CPSS_LED_BLINK_SELECT_ENT);
use_prv_enum(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT);
use_prv_struct(CPSS_MAC_UPDATE_MSG_STC);
use_prv_struct(CPSS_TM2TM_DELTA_RANGE_STC);
use_prv_enum(CPSS_DRAM_PAGE_SIZE_ENT);
use_prv_enum(CPSS_PHY_XSMI_INTERFACE_ENT);
use_prv_enum(CPSS_NARROW_SRAM_CFG_ENT);
use_prv_enum(CPSS_VCT_DOWNSHIFT_STATUS_ENT);
use_prv_enum(CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT);
use_prv_enum(CPSS_BRG_IPM_MODE_ENT);
use_prv_enum(CPSS_CSCD_PORT_TYPE_ENT);
use_prv_struct(CPSS_TM_QUEUE_PARAMS_STC);
use_prv_enum(CPSS_PORT_TX_WRR_MTU_ENT);
use_prv_struct(CPSS_DRAM_CFG_STC);
use_prv_enum(CPSS_PORT_DIRECTION_ENT);
use_prv_enum(CPSS_SYSTEM_RECOVERY_STATE_ENT);
use_prv_enum(CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT);
use_prv_enum(CPSS_FDB_ACTION_MODE_ENT);
use_prv_enum(CPSS_VCT_NORMAL_CABLE_LEN_ENT);
use_prv_struct(CPSS_PORT_ATTRIBUTES_STC);
use_prv_enum(CPSS_GEN_DRAGONITE_PRIORITY_ENT);
use_prv_struct(CPSS_PORTS_BMP_STC);
use_prv_enum(CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT);
use_prv_struct(CPSS_TM_BAP_UNIT_ERROR_STATUS_STC);
use_prv_struct(CPSS_CSCD_LINK_TYPE_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_DATA_STC);
use_prv_enum(CPSS_PORT_TX_Q_ARB_GROUP_ENT);
use_prv_struct(CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC);
use_prv_enum(CPSS_UPD_MSG_TYPE_ENT);
use_prv_enum(CPSS_SRAM_FREQUENCY_ENT);
use_prv_enum(CPSS_PP_INTERFACE_CHANNEL_ENT);
use_prv_enum(CPSS_IGMP_SNOOP_MODE_ENT);
use_prv_struct(CPSS_TM_LIB_INIT_PARAMS_STC);
use_prv_enum(CPSS_NST_AM_PARAM_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_LAYER2_STC);
use_prv_struct(CPSS_BRIDGE_INGRESS_CNTR_STC);
use_prv_struct(CPSS_TM2TM_EXTERNAL_HDR_STC);
use_prv_struct(CPSS_LED_CONF_STC);
use_prv_enum(CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT);
use_prv_struct(CPSS_TM_PORT_DROP_PER_COS_STC);
use_prv_enum(CPSS_GEN_DRAGONITE_DATA_TYPE_ENT);
use_prv_enum(CPSS_EXTERNAL_MEMORY_ENT);
use_prv_enum(CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT);
use_prv_enum(CPSS_DRV_HW_TRACE_TYPE_ENT);
use_prv_enum(CPSS_PP_FAMILY_BIT_ENT);
use_prv_enum(CPSS_DRAM_SPEED_BIN_ENT);
use_prv_enum(CPSS_BRG_MODE_ENT);
use_prv_struct(CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS);
use_prv_enum(CPSS_TM_COLOR_NUM_ENT);
use_prv_enum(CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT);
use_prv_enum(CPSS_POLICER_COLOR_MODE_ENT);
use_prv_enum(CPSS_PORT_ACCEPT_FRAME_TYPE_ENT);
use_prv_struct(CPSS_VCT_CABLE_EXTENDED_STATUS_STC);
use_prv_enum(CPSS_MAC_ACTION_MODE_ENT);
use_prv_enum(CPSS_RATE_LIMIT_MODE_ENT);
use_prv_enum(CPSS_PORT_MAC_TYPE_ENT);
use_prv_struct(CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC);
use_prv_enum(CPSS_PORT_INTERFACE_MODE_ENT);
use_prv_struct(CPSS_TM_TREE_PARAMS_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_SYSTEM_STC);
use_prv_struct(CPSS_PORT_MAC_STATUS_STC);
use_prv_enum(CPSS_PORT_SPEED_ENT);
use_prv_enum(CPSS_IPV6_MLL_SELECTION_RULE_ENT);
use_prv_struct(CPSS_LED_CLASS_MANIPULATION_STC);
use_prv_enum(CPSS_PHY_SMI_INTERFACE_ENT);
use_prv_enum(CPSS_PORT_SERDES_TUNING_TYPE_ENT);
use_prv_enum(CPSS_EVENT_GPP_ID_ENT);
use_prv_struct(CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC);
use_prv_struct(CPSS_SRAM_CFG_STC);
use_prv_struct(CPSS_LED_GROUP_CONF_STC);
use_prv_struct(CPSS_PORT_MODE_SPEED_STC);
use_prv_struct(CPSS_TM_UNITS_ERROR_STATUS_STC);
use_prv_enum(CPSS_SRAM_RATE_ENT);
use_prv_enum(CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT);
use_prv_enum(CPSS_MAC_HASH_FUNC_MODE_ENT);
use_prv_enum(CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT);
use_prv_enum(CPSS_LED_ORDER_MODE_ENT);
use_prv_enum(CPSS_TM_ELIG_FUNC_NODE_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_CTRL_STC);
use_prv_struct(CPSS_RLDRAM_CFG_STC);
use_prv_enum(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT);
use_prv_struct(CPSS_SYSTEM_RECOVERY_INFO_STC);
use_prv_enum(CPSS_ETHER_MODE_ENT);
use_prv_enum(CPSS_PORT_TX_DROP_PROFILE_SET_ENT);
use_prv_enum(CPSS_NST_EGRESS_FRW_FILTER_ENT);
use_prv_struct(CPSS_VCT_PAIR_SKEW_STC);
use_prv_struct(CPSS_TM_PORT_PARAMS_STC);
use_prv_enum(CPSS_IP_PROTOCOL_STACK_ENT);
use_prv_enum(CPSS_LED_BLINK_DURATION_ENT);
use_prv_struct(CPSS_TRUNK_MEMBER_STC);
use_prv_enum(CPSS_SYS_HA_MODE_ENT);
use_prv_enum(CPSS_ADJUST_OPERATION_ENT);
use_prv_struct(CPSS_SYSTEM_RECOVERY_MODE_STC);
use_prv_enum(CPSS_PORT_PCS_RESET_MODE_ENT);
use_prv_enum(CPSS_TM_PORT_BW_ENT);
use_prv_enum(CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT);
use_prv_enum(CPSS_BRIDGE_INGR_CNTR_MODES_ENT);
use_prv_enum(CPSS_POLICER_TB_MODE_ENT);
use_prv_struct(CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC);
use_prv_struct(CPSS_TM_DROP_PROFILE_PARAMS_STC);
use_prv_enum(CPSS_HSU_DATA_TYPE_ENT);
use_prv_enum(CPSS_DRAM_BUS_WIDTH_ENT);
use_prv_enum(CPSS_TUNNEL_MULTICAST_TYPE_ENT);
use_prv_enum(CPSS_LED_PULSE_STRETCH_ENT);
use_prv_enum(CPSS_DIAG_PP_MEM_TYPE_ENT);
use_prv_struct(CPSS_TM_ERROR_INFO_STC);
use_prv_enum(CPSS_UNREG_MC_EGR_FILTER_CMD_ENT);
use_prv_enum(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT);
use_prv_enum(CPSS_DRAM_FREQUENCY_ENT);
use_prv_struct(CPSS_VCT_CONFIG_STC);
use_prv_enum(CPSS_INTERFACE_TYPE_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC);
use_prv_enum(CPSS_UNICAST_MULTICAST_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_MATRIX_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC);
use_prv_enum(CPSS_HW_PP_INIT_STAGE_ENT);
use_prv_enum(CPSS_VCT_PHY_TYPES_ENT);
use_prv_enum(CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT);
use_prv_enum(CPSS_DROP_MODE_TYPE_ENT);
use_prv_enum(CPSS_LED_CLASS_5_SELECT_ENT);
use_prv_enum(CPSS_NST_INGRESS_FRW_FILTER_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_COMMAND_STC);
use_prv_enum(CPSS_PORT_TX_WRR_MODE_ENT);
use_prv_struct(CPSS_GEN_CFG_DEV_INFO_STC);
use_prv_enum(CPSS_TX_BUFF_ALLOC_METHOD_ENT);
use_prv_enum(CPSS_PP_SUB_FAMILY_TYPE_ENT);
use_prv_struct(CPSS_VERSION_INFO_STC);
use_prv_enum(CPSS_MAC_SA_LSB_MODE_ENT);
use_prv_struct(CPSS_MAC_ENTRY_EXT_KEY_STC);
use_prv_enum(CPSS_DRAM_TEMPERATURE_ENT);
use_prv_enum(CPSS_AU_MESSAGE_LENGTH_ENT);
use_prv_enum(CPSS_LOG_POINTER_FORMAT_ENT);
use_prv_enum(CPSS_STP_STATE_ENT);
use_prv_enum(CPSS_LOG_API_FORMAT_ENT);
use_prv_enum(CPSS_BRG_TPID_SIZE_TYPE_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
use_prv_struct(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC);
use_prv_struct(CPSS_VCT_STATUS_STC);
use_prv_enum(CPSS_TRUNK_TYPE_ENT);
use_prv_enum(CPSS_IP_UNICAST_MULTICAST_ENT);
use_prv_enum(CPSS_PP_UPLINK_CONFIG_ENT);
use_prv_enum(CPSS_MAC_VL_ENT);
use_prv_enum(CPSS_TUNNEL_PASSENGER_PACKET_ENT);
use_prv_struct(CPSS_TM_NODE_STATUS_STC);
use_prv_enum(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT);
use_prv_struct(CPSS_COS_MAP_STC);
use_prv_enum(CPSS_TM_LEVEL_ENT);
use_prv_enum(CPSS_POLICER_PACKET_SIZE_MODE_ENT);
use_prv_enum(CPSS_HW_PP_RESET_SKIP_TYPE_ENT);
use_prv_enum(CPSS_DIAG_PP_REG_TYPE_ENT);
use_prv_enum(CPSS_NET_RX_CPU_CODE_ENT);
use_prv_enum(CPSS_MEMORY_DUMP_TYPE_ENT);
use_prv_struct(CPSS_TM_C_NODE_PARAMS_STC);
use_prv_enum(CPSS_TM2TM_CHANNEL_ENT);
use_prv_struct(CPSS_MAC_ENTRY_EXT_STC);
use_prv_enum(CPSS_COMPARE_OPERATOR_ENT);
use_prv_struct(CPSS_COS_STC);
use_prv_struct(CPSS_MAC_UPDATE_MSG_EXT_STC);
use_prv_enum(CPSS_L4_PROTOCOL_PORT_TYPE_ENT);
use_prv_struct(CPSS_TM_QMR_PKT_STATISTICS_STC);
use_prv_enum(CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT);
use_prv_enum(CPSS_PORT_EGRESS_CNT_MODE_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
use_prv_enum(CPSS_SYSTEM_RECOVERY_PROCESS_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_STATUSES_STC);
use_prv_struct(CPSS_TM_LEVEL_PERIODIC_PARAMS_STC);
use_prv_struct(CPSS_FDB_HASH_OPTION_STC);
use_prv_enum(CPSS_VCT_POLARITY_SWAP_ENT);
use_prv_enum(CPSS_PORT_MAC_COUNTERS_ENT);
use_prv_enum(CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);
use_prv_enum(CPSS_FLOW_DRAM_PARAM_ENT);
use_prv_enum(CPSS_PP_FAMILY_TYPE_ENT);
use_prv_enum(CPSS_TM_DP_SOURCE_ENT);
use_prv_enum(CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT);
use_prv_enum(CPSS_PORT_XGMII_MODE_ENT);
use_prv_enum(CPSS_DP_LEVEL_ENT);
use_prv_enum(CPSS_TM_SCHD_MODE_ENT);
use_prv_enum(CPSS_BUFFER_DRAM_PARAM_ENT);
use_prv_enum(CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT);
use_prv_struct(CPSS_TM_RCB_PKT_STATISTICS_STC);
use_prv_enum(CPSS_QOS_PORT_TRUST_MODE_ENT);
use_prv_enum(CPSS_POLICER_MRU_ENT);
use_prv_enum(CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT);
use_prv_enum(CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT);
use_prv_enum(CPSS_LOG_TIME_FORMAT_ENT);
use_prv_enum(CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT);
use_prv_enum(CPSS_DRV_HW_ACCESS_STAGE_ENT);
use_prv_struct(CPSS_TM2TM_CNTRL_PKT_STRUCT_STC);
use_prv_struct(CPSS_REG_VALUE_INFO_STC);
use_prv_enum(CPSS_PORT_XG_FIXED_IPG_ENT);
use_prv_enum(CPSS_TM_DROP_MODE_ENT);
use_prv_enum(CPSS_PORT_TX_DROP_SHAPER_MODE_ENT);
use_prv_struct(CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC);
use_prv_enum(CPSS_IP_CNT_SET_ENT);
use_prv_enum(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT);
use_prv_enum(CPSS_MAC_ENTRY_EXT_TYPE_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_CONFIG_STC);
use_prv_enum(CPSS_IPV6_PREFIX_SCOPE_ENT);
use_prv_struct(GT_U64);
use_prv_enum(CPSS_SRAM_TYPE_ENT);
use_prv_struct(CPSS_TM_A_NODE_PARAMS_STC);
use_prv_enum(CPSS_TX_SDMA_QUEUE_MODE_ENT);
use_prv_enum(CPSS_DIRECTION_ENT);
use_prv_enum(CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT);
use_prv_enum(CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT);
use_prv_struct(CPSS_FDB_UC_ROUTING_INFO_STC);
use_prv_struct(CPSS_PROT_CLASS_ENCAP_STC);
use_prv_struct(CPSS_TRUNK_WEIGHTED_MEMBER_STC);

/***** declarations ********/

enumDescr enumDescr_CPSS_LED_CLASS_13_SELECT_ENT[] = {
    ENUM_ENTRY(CPSS_LED_CLASS_13_SELECT_LINK_DOWN_E),
    ENUM_ENTRY(CPSS_LED_CLASS_13_SELECT_COPPER_LINK_UP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LED_CLASS_13_SELECT_ENT);

void prv_lua_to_c_CPSS_TM_SHAPING_PROFILE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, cirBw, GT_U32);
    F_NUMBER(val, -1, cbs, GT_U32);
    F_NUMBER(val, -1, eirBw, GT_U32);
    F_NUMBER(val, -1, ebs, GT_U32);
}

void prv_c_to_lua_CPSS_TM_SHAPING_PROFILE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, cirBw, GT_U32);
    FO_NUMBER(val, t, cbs, GT_U32);
    FO_NUMBER(val, t, eirBw, GT_U32);
    FO_NUMBER(val, t, ebs, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_SHAPING_PROFILE_PARAMS_STC);

void prv_lua_to_c_CPSS_BUFF_MGMT_CFG_STC(
    lua_State *L,
    CPSS_BUFF_MGMT_CFG_STC *val
)
{
    F_BOOL(val, -1, overwriteDef);
    F_NUMBER(val, -1, numPpBuffers, GT_U16);
    F_NUMBER(val, -1, numUplinkBuffers, GT_U16);
}

void prv_c_to_lua_CPSS_BUFF_MGMT_CFG_STC(
    lua_State *L,
    CPSS_BUFF_MGMT_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, overwriteDef);
    FO_NUMBER(val, t, numPpBuffers, GT_U16);
    FO_NUMBER(val, t, numUplinkBuffers, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_BUFF_MGMT_CFG_STC);

enumDescr enumDescr_CPSS_LOG_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_LOG_TYPE_INFO_E),
    ENUM_ENTRY(CPSS_LOG_TYPE_ENTRY_LEVEL_FUNCTION_E),
    ENUM_ENTRY(CPSS_LOG_TYPE_NON_ENTRY_LEVEL_FUNCTION_E),
    ENUM_ENTRY(CPSS_LOG_TYPE_DRIVER_E),
    ENUM_ENTRY(CPSS_LOG_TYPE_ERROR_E),
    ENUM_ENTRY(CPSS_LOG_TYPE_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LOG_TYPE_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_CHIP_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_CHIP_STC *val
)
{
    F_NUMBER(val, -1, chipInfo, GT_U32);
    F_NUMBER(val, -1, measuredTemp, GT_U32);
    F_NUMBER(val, -1, maxMeasuredTemp, GT_U32);
    F_NUMBER(val, -1, numOfPorts, GT_U32);
    F_BOOL(val, -1, vmainHighError);
    F_BOOL(val, -1, overTempError);
    F_BOOL(val, -1, disablePortsActiveError);
    F_BOOL(val, -1, vmainLowAfError);
    F_BOOL(val, -1, vmainLowAtError);
    F_BOOL(val, -1, tempAlarm);
    F_NUMBER(val, -1, chipDevId, GT_U32);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_CHIP_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_CHIP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, chipInfo, GT_U32);
    FO_NUMBER(val, t, measuredTemp, GT_U32);
    FO_NUMBER(val, t, maxMeasuredTemp, GT_U32);
    FO_NUMBER(val, t, numOfPorts, GT_U32);
    FO_BOOL(val, t, vmainHighError);
    FO_BOOL(val, t, overTempError);
    FO_BOOL(val, t, disablePortsActiveError);
    FO_BOOL(val, t, vmainLowAfError);
    FO_BOOL(val, t, vmainLowAtError);
    FO_BOOL(val, t, tempAlarm);
    FO_NUMBER(val, t, chipDevId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_CHIP_STC);

void prv_lua_to_c_CPSS_HW_ADDR_STC(
    lua_State *L,
    CPSS_HW_ADDR_STC *val
)
{
    F_NUMBER(val, -1, busNo, GT_U32);
    F_NUMBER(val, -1, devSel, GT_U32);
    F_NUMBER(val, -1, funcNo, GT_U32);
}

void prv_c_to_lua_CPSS_HW_ADDR_STC(
    lua_State *L,
    CPSS_HW_ADDR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, busNo, GT_U32);
    FO_NUMBER(val, t, devSel, GT_U32);
    FO_NUMBER(val, t, funcNo, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_HW_ADDR_STC);

void prv_lua_to_c_CPSS_VLAN_INFO_STC(
    lua_State *L,
    CPSS_VLAN_INFO_STC *val
)
{
    F_BOOL(val, -1, hasLocals);
    F_BOOL(val, -1, hasUplinks);
    F_BOOL(val, -1, isCpuMember);
    F_BOOL(val, -1, learnEnable);
    F_ENUM(val, -1, unregMcFilterCmd, CPSS_UNREG_MC_EGR_FILTER_CMD_ENT);
    F_BOOL(val, -1, perVlanSaMacEnabled);
    F_NUMBER(val, -1, saMacSuffix, GT_U8);
    F_NUMBER(val, -1, stpId, GT_U32);
}

void prv_c_to_lua_CPSS_VLAN_INFO_STC(
    lua_State *L,
    CPSS_VLAN_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, hasLocals);
    FO_BOOL(val, t, hasUplinks);
    FO_BOOL(val, t, isCpuMember);
    FO_BOOL(val, t, learnEnable);
    FO_ENUM(val, t, unregMcFilterCmd, CPSS_UNREG_MC_EGR_FILTER_CMD_ENT);
    FO_BOOL(val, t, perVlanSaMacEnabled);
    FO_NUMBER(val, t, saMacSuffix, GT_U8);
    FO_NUMBER(val, t, stpId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VLAN_INFO_STC);

void prv_lua_to_c_CPSS_VCT_ACCURATE_CABLE_LEN_STC(
    lua_State *L,
    CPSS_VCT_ACCURATE_CABLE_LEN_STC *val
)
{
    F_ARRAY_START(val, -1, isValid);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_BOOL(val, isValid, idx, GT_BOOL);
        }
    }
    F_ARRAY_END(val, -1, isValid);
    F_ARRAY_START(val, -1, cableLen);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, cableLen, idx, GT_U16);
        }
    }
    F_ARRAY_END(val, -1, cableLen);
}

void prv_c_to_lua_CPSS_VCT_ACCURATE_CABLE_LEN_STC(
    lua_State *L,
    CPSS_VCT_ACCURATE_CABLE_LEN_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, isValid);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_BOOL(val, isValid, idx, GT_BOOL);
        }
    }
    FO_ARRAY_END(val, t, isValid);
    FO_ARRAY_START(val, t, cableLen);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, cableLen, idx, GT_U16);
        }
    }
    FO_ARRAY_END(val, t, cableLen);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VCT_ACCURATE_CABLE_LEN_STC);

enumDescr enumDescr_CPSS_HW_PP_INIT_STAGE_ENT[] = {
    ENUM_ENTRY(CPSS_HW_PP_INIT_STAGE_INIT_DURING_RESET_E),
    ENUM_ENTRY(CPSS_HW_PP_INIT_STAGE_EEPROM_DONE_INT_MEM_DONE_E),
    ENUM_ENTRY(CPSS_HW_PP_INIT_STAGE_EEPROM_NOT_DONE_INT_MEM_DONE_E),
    ENUM_ENTRY(CPSS_HW_PP_INIT_STAGE_FULLY_FUNC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_HW_PP_INIT_STAGE_ENT);

enumDescr enumDescr_CPSS_PHY_ERRATA_WA_ENT[] = {
    ENUM_ENTRY(CPSS_PHY_ERRATA_WA_88E1240_SGMII_LOCKUP_E),
    ENUM_ENTRY(CPSS_PHY_ERRATA_WA_88E1340_BGA_INIT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PHY_ERRATA_WA_ENT);

enumDescr enumDescr_CPSS_IP_SITE_ID_ENT[] = {
    ENUM_ENTRY(CPSS_IP_SITE_ID_INTERNAL_E),
    ENUM_ENTRY(CPSS_IP_SITE_ID_EXTERNAL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_IP_SITE_ID_ENT);

enumDescr enumDescr_CPSS_PORT_REF_CLOCK_SOURCE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_REF_CLOCK_SOURCE_PRIMARY_E),
    ENUM_ENTRY(CPSS_PORT_REF_CLOCK_SOURCE_SECONDARY_E),
    ENUM_ENTRY(CPSS_PORT_REF_CLOCK_SOURCE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_REF_CLOCK_SOURCE_ENT);

enumDescr enumDescr_CPSS_DIAG_TEST_PROFILE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_TEST_RANDOM_E),
    ENUM_ENTRY(CPSS_DIAG_TEST_INCREMENTAL_E),
    ENUM_ENTRY(CPSS_DIAG_TEST_BIT_TOGGLE_E),
    ENUM_ENTRY(CPSS_DIAG_TEST_AA_55_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_TEST_PROFILE_ENT);

void prv_lua_to_c_CPSS_TM_ELIG_PRIO_FUNC_OUT_STC(
    lua_State *L,
    CPSS_TM_ELIG_PRIO_FUNC_OUT_STC *val
)
{
    F_NUMBER(val, -1, maxTb, GT_U32);
    F_NUMBER(val, -1, minTb, GT_U32);
    F_NUMBER(val, -1, propPrio, GT_U32);
    F_NUMBER(val, -1, schedPrio, GT_U32);
    F_NUMBER(val, -1, elig, GT_U32);
}

void prv_c_to_lua_CPSS_TM_ELIG_PRIO_FUNC_OUT_STC(
    lua_State *L,
    CPSS_TM_ELIG_PRIO_FUNC_OUT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, maxTb, GT_U32);
    FO_NUMBER(val, t, minTb, GT_U32);
    FO_NUMBER(val, t, propPrio, GT_U32);
    FO_NUMBER(val, t, schedPrio, GT_U32);
    FO_NUMBER(val, t, elig, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_ELIG_PRIO_FUNC_OUT_STC);

enumDescr enumDescr_CPSS_LED_CLOCK_OUT_FREQUENCY_ENT[] = {
    ENUM_ENTRY(CPSS_LED_CLOCK_OUT_FREQUENCY_500_E),
    ENUM_ENTRY(CPSS_LED_CLOCK_OUT_FREQUENCY_1000_E),
    ENUM_ENTRY(CPSS_LED_CLOCK_OUT_FREQUENCY_2000_E),
    ENUM_ENTRY(CPSS_LED_CLOCK_OUT_FREQUENCY_3000_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LED_CLOCK_OUT_FREQUENCY_ENT);

enumDescr enumDescr_CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E),
    ENUM_ENTRY(CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E),
    ENUM_ENTRY(CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E),
    ENUM_ENTRY(CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E),
    ENUM_ENTRY(CPSS_MAC_SECURE_AUTO_LEARN_UNK_FORWARD_E),
    ENUM_ENTRY(CPSS_MAC_SECURE_AUTO_LEARN_UNK_MIRROR_TO_CPU_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC *val
)
{
    F_BOOL(val, -1, underload);
    F_BOOL(val, -1, overload);
    F_BOOL(val, -1, shortCircuit);
    F_BOOL(val, -1, invalidSignature);
    F_BOOL(val, -1, validSignature);
    F_BOOL(val, -1, powerDenied);
    F_BOOL(val, -1, validCapacitor);
    F_BOOL(val, -1, backoff);
    F_BOOL(val, -1, classError);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, underload);
    FO_BOOL(val, t, overload);
    FO_BOOL(val, t, shortCircuit);
    FO_BOOL(val, t, invalidSignature);
    FO_BOOL(val, t, validSignature);
    FO_BOOL(val, t, powerDenied);
    FO_BOOL(val, t, validCapacitor);
    FO_BOOL(val, t, backoff);
    FO_BOOL(val, t, classError);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC);

void prv_lua_to_c_CPSS_TM_QUANTUM_LIMITS_STC(
    lua_State *L,
    CPSS_TM_QUANTUM_LIMITS_STC *val
)
{
    F_NUMBER(val, -1, minQuantum, GT_U32);
    F_NUMBER(val, -1, maxQuantum, GT_U32);
    F_NUMBER(val, -1, resolution, GT_U32);
}

void prv_c_to_lua_CPSS_TM_QUANTUM_LIMITS_STC(
    lua_State *L,
    CPSS_TM_QUANTUM_LIMITS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, minQuantum, GT_U32);
    FO_NUMBER(val, t, maxQuantum, GT_U32);
    FO_NUMBER(val, t, resolution, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_QUANTUM_LIMITS_STC);

enumDescr enumDescr_CPSS_PCL_RULE_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_STD_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_30_BYTES_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_EXT_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_60_BYTES_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_ULTRA_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_80_BYTES_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_10_BYTES_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_20_BYTES_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_40_BYTES_E),
    ENUM_ENTRY(CPSS_PCL_RULE_SIZE_50_BYTES_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PCL_RULE_SIZE_ENT);

void prv_lua_to_c_CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC(
    lua_State *L,
    CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC *val
)
{
    F_ARRAY_START(val, -1, counterArray);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, counterArray, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, counterArray);
}

void prv_c_to_lua_CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC(
    lua_State *L,
    CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, counterArray);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, counterArray, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, counterArray);
    lua_settop(L, t);
}
add_mgm_type(CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC);

void prv_lua_to_c_CPSS_TM_PORT_STATUS_STC(
    lua_State *L,
    CPSS_TM_PORT_STATUS_STC *val
)
{
    F_NUMBER(val, -1, maxBucketLevel, GT_U32);
    F_NUMBER(val, -1, minBucketLevel, GT_U32);
    F_ARRAY_START(val, -1, deficitArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, deficitArr, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, deficitArr);
}

void prv_c_to_lua_CPSS_TM_PORT_STATUS_STC(
    lua_State *L,
    CPSS_TM_PORT_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, maxBucketLevel, GT_U32);
    FO_NUMBER(val, t, minBucketLevel, GT_U32);
    FO_ARRAY_START(val, t, deficitArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, deficitArr, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, deficitArr);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_PORT_STATUS_STC);

enumDescr enumDescr_CPSS_MAC_TABLE_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_MAC_TABLE_FRWRD_E),
    ENUM_ENTRY(CPSS_MAC_TABLE_DROP_E),
    ENUM_ENTRY(CPSS_MAC_TABLE_INTERV_E),
    ENUM_ENTRY(CPSS_MAC_TABLE_CNTL_E),
    ENUM_ENTRY(CPSS_MAC_TABLE_MIRROR_TO_CPU_E),
    ENUM_ENTRY(CPSS_MAC_TABLE_SOFT_DROP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_MAC_TABLE_CMD_ENT);

enumDescr enumDescr_CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CONVERT_BY_TABLE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CONVERT_BY_FUNCTION_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT);

enumDescr enumDescr_CPSS_UNI_EV_CAUSE_ENT[] = {
    ENUM_ENTRY(CPSS_PP_UNI_EV_MIN_E),
    ENUM_ENTRY(CPSS_PP_SLV_WRITE_ERR_E),
    ENUM_ENTRY(CPSS_PP_MAS_WRITE_ERR_E),
    ENUM_ENTRY(CPSS_PP_ADDR_ERR_E),
    ENUM_ENTRY(CPSS_PP_MAS_ABORT_E),
    ENUM_ENTRY(CPSS_PP_TARGET_ABORT_E),
    ENUM_ENTRY(CPSS_PP_SLV_READ_ERR_E),
    ENUM_ENTRY(CPSS_PP_MAS_READ_ERR_E),
    ENUM_ENTRY(CPSS_PP_RETRY_CNTR_E),
    ENUM_ENTRY(CPSS_PP_MISC_TWSI_TIME_OUT_E),
    ENUM_ENTRY(CPSS_PP_MISC_TWSI_STATUS_E),
    ENUM_ENTRY(CPSS_PP_MISC_ILLEGAL_ADDR_E),
    ENUM_ENTRY(CPSS_PP_MISC_CPU_PORT_RX_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_MISC_CPU_PORT_TX_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_MISC_TX_CRC_PORT_E),
    ENUM_ENTRY(CPSS_PP_MISC_C2C_W_FAR_END_UP_E),
    ENUM_ENTRY(CPSS_PP_MISC_C2C_N_FAR_END_UP_E),
    ENUM_ENTRY(CPSS_PP_MISC_C2C_DATA_ERR_E),
    ENUM_ENTRY(CPSS_PP_MISC_MSG_TIME_OUT_E),
    ENUM_ENTRY(CPSS_PP_MISC_UPDATED_STAT_E),
    ENUM_ENTRY(CPSS_PP_GPP_E),
    ENUM_ENTRY(CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E),
    ENUM_ENTRY(CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E),
    ENUM_ENTRY(CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E),
    ENUM_ENTRY(CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E),
    ENUM_ENTRY(CPSS_PP_MISC_PEX_ADDR_UNMAPPED_E),
    ENUM_ENTRY(CPSS_PP_MISC_GENXS_READ_DMA_DONE_E),
    ENUM_ENTRY(CPSS_PP_BM_EMPTY_CLEAR_E),
    ENUM_ENTRY(CPSS_PP_BM_EMPTY_INC_E),
    ENUM_ENTRY(CPSS_PP_BM_AGED_PACKET_E),
    ENUM_ENTRY(CPSS_PP_BM_MAX_BUFF_REACHED_E),
    ENUM_ENTRY(CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_BM_INVALID_ADDRESS_E),
    ENUM_ENTRY(CPSS_PP_BM_WRONG_SRC_PORT_E),
    ENUM_ENTRY(CPSS_PP_BM_MC_INC_OVERFLOW_E),
    ENUM_ENTRY(CPSS_PP_BM_MC_INC_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_BM_TQ_PARITY_ERROR_PORT_E),
    ENUM_ENTRY(CPSS_PP_BM_RX_MEM_READ_ECC_ERROR_E),
    ENUM_ENTRY(CPSS_PP_BM_VLT_ECC_ERR_E),
    ENUM_ENTRY(CPSS_PP_BM_MISC_E),
    ENUM_ENTRY(CPSS_PP_BM_TRIGGER_AGING_DONE_E),
    ENUM_ENTRY(CPSS_PP_BM_PORT_RX_FULL_E),
    ENUM_ENTRY(CPSS_PP_BM_ALL_GIGA_PORTS_BUFF_LIMIT_REACHED_E),
    ENUM_ENTRY(CPSS_PP_BM_ALL_HG_STACK_PORTS_BUFF_LIMIT_REACHED_E),
    ENUM_ENTRY(CPSS_PP_PORT_LINK_STATUS_CHANGED_E),
    ENUM_ENTRY(CPSS_PP_PORT_AN_COMPLETED_E),
    ENUM_ENTRY(CPSS_PP_PORT_RX_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_PORT_TX_FIFO_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_PORT_TX_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_PORT_TX_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_PORT_PRBS_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PORT_SYNC_STATUS_CHANGED_E),
    ENUM_ENTRY(CPSS_PP_PORT_TX_CRC_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PORT_ILLEGAL_SEQUENCE_E),
    ENUM_ENTRY(CPSS_PP_PORT_IPG_TOO_SMALL_E),
    ENUM_ENTRY(CPSS_PP_PORT_FAULT_TYPE_CHANGE_E),
    ENUM_ENTRY(CPSS_PP_PORT_FC_STATUS_CHANGED_E),
    ENUM_ENTRY(CPSS_PP_PORT_CONSECUTIVE_TERM_CODE_E),
    ENUM_ENTRY(CPSS_PP_INTERNAL_PHY_E),
    ENUM_ENTRY(CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E),
    ENUM_ENTRY(CPSS_PP_PORT_XAUI_PHY_E),
    ENUM_ENTRY(CPSS_PP_PORT_COUNT_COPY_DONE_E),
    ENUM_ENTRY(CPSS_PP_PORT_COUNT_EXPIRED_E),
    ENUM_ENTRY(CPSS_PP_GOP_ADDRESS_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_GOP_COUNT_EXPIRED_E),
    ENUM_ENTRY(CPSS_PP_GOP_COUNT_COPY_DONE_E),
    ENUM_ENTRY(CPSS_PP_GOP_GIG_BAD_FC_PACKET_GOOD_CRC_E),
    ENUM_ENTRY(CPSS_PP_GOP_XG_BAD_FC_PACKET_GOOD_CRC_E),
    ENUM_ENTRY(CPSS_PP_XSMI_WRITE_DONE_E),
    ENUM_ENTRY(CPSS_PP_TQ_WATCHDOG_EX_PORT_E),
    ENUM_ENTRY(CPSS_PP_TQ_TXQ2_FLUSH_PORT_E),
    ENUM_ENTRY(CPSS_PP_TQ_TXQ2_MG_FLUSH_E),
    ENUM_ENTRY(CPSS_PP_TQ_ONE_ECC_ERROR_E),
    ENUM_ENTRY(CPSS_PP_TQ_TWO_ECC_ERROR_E),
    ENUM_ENTRY(CPSS_PP_TQ_MG_READ_ERR_E),
    ENUM_ENTRY(CPSS_PP_TQ_HOL_REACHED_PORT_E),
    ENUM_ENTRY(CPSS_PP_TQ_RED_REACHED_PORT_E),
    ENUM_ENTRY(CPSS_PP_TQ_TOTAL_DESC_UNDERFLOW_E),
    ENUM_ENTRY(CPSS_PP_TQ_TOTAL_DESC_OVERFLOW_E),
    ENUM_ENTRY(CPSS_PP_TQ_TOTAL_BUFF_UNDERFLOW_E),
    ENUM_ENTRY(CPSS_PP_TQ_SNIFF_DESC_DROP_E),
    ENUM_ENTRY(CPSS_PP_TQ_MLL_PARITY_ERR_E),
    ENUM_ENTRY(CPSS_PP_TQ_MC_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_TQ_MISC_E),
    ENUM_ENTRY(CPSS_PP_TQ_PORT_DESC_FULL_E),
    ENUM_ENTRY(CPSS_PP_TQ_MC_DESC_FULL_E),
    ENUM_ENTRY(CPSS_PP_TQ_GIGA_FIFO_FULL_E),
    ENUM_ENTRY(CPSS_PP_TQ_XG_MC_FIFO_FULL_E),
    ENUM_ENTRY(CPSS_PP_TQ_PORT_FULL_XG_E),
    ENUM_ENTRY(CPSS_PP_EB_AUQ_FULL_E),
    ENUM_ENTRY(CPSS_PP_EB_AUQ_PENDING_E),
    ENUM_ENTRY(CPSS_PP_EB_AUQ_OVER_E),
    ENUM_ENTRY(CPSS_PP_EB_AUQ_ALMOST_FULL_E),
    ENUM_ENTRY(CPSS_PP_EB_FUQ_FULL_E),
    ENUM_ENTRY(CPSS_PP_EB_FUQ_PENDING_E),
    ENUM_ENTRY(CPSS_PP_EB_NA_FIFO_FULL_E),
    ENUM_ENTRY(CPSS_PP_EB_MG_ADDR_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_EB_VLAN_TBL_OP_DONE_E),
    ENUM_ENTRY(CPSS_PP_EB_SECURITY_BREACH_UPDATE_E),
    ENUM_ENTRY(CPSS_PP_EB_VLAN_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_NA_NOT_LEARNED_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_SA_MSG_DISCARDED_E),
    ENUM_ENTRY(CPSS_PP_EB_QA_MSG_DISCARDED_E),
    ENUM_ENTRY(CPSS_PP_EB_SA_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_DA_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_DA_SA_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_NA_ON_LOCKED_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_MAC_RANGE_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_INVALID_SA_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_VLAN_NOT_VALID_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_VLAN_NOT_MEMBER_DROP_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_VLAN_RANGE_DROP_SECURITY_BREACH_E),
    ENUM_ENTRY(CPSS_PP_EB_INGRESS_FILTER_PCKT_E),
    ENUM_ENTRY(CPSS_PP_MAC_TBL_READ_ECC_ERR_E),
    ENUM_ENTRY(CPSS_PP_MAC_SFLOW_E),
    ENUM_ENTRY(CPSS_PP_MAC_NUM_OF_HOP_EXP_E),
    ENUM_ENTRY(CPSS_PP_MAC_NA_LEARNED_E),
    ENUM_ENTRY(CPSS_PP_MAC_NA_NOT_LEARNED_E),
    ENUM_ENTRY(CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E),
    ENUM_ENTRY(CPSS_PP_MAC_MG_ADDR_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E),
    ENUM_ENTRY(CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E),
    ENUM_ENTRY(CPSS_PP_MAC_NA_SELF_LEARNED_E),
    ENUM_ENTRY(CPSS_PP_MAC_NA_FROM_CPU_LEARNED_E),
    ENUM_ENTRY(CPSS_PP_MAC_NA_FROM_CPU_DROPPED_E),
    ENUM_ENTRY(CPSS_PP_MAC_AGED_OUT_E),
    ENUM_ENTRY(CPSS_PP_MAC_FIFO_2_CPU_EXCEEDED_E),
    ENUM_ENTRY(CPSS_PP_MAC_1ECC_ERRORS_E),
    ENUM_ENTRY(CPSS_PP_MAC_2ECC_ERRORS_E),
    ENUM_ENTRY(CPSS_PP_MAC_TBL_OP_DONE_E),
    ENUM_ENTRY(CPSS_PP_MAC_PACKET_RSSI_LESS_THAN_THRESH_E),
    ENUM_ENTRY(CPSS_PP_MAC_UPD_AVG_RSSI_LESS_THAN_THRESH_E),
    ENUM_ENTRY(CPSS_PP_MAC_STG_TBL_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_MAC_VIDX_TBL_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_LX_LB_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_CTRL_MEM_2_RF_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_TCB_CNTR_E),
    ENUM_ENTRY(CPSS_PP_LX_IPV4_MC_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_IPV4_LPM_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_IPV4_ROUTE_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_IPV4_CNTR_E),
    ENUM_ENTRY(CPSS_PP_LX_L3_L7_ERR_ADDR_E),
    ENUM_ENTRY(CPSS_PP_LX_TRUNK_ADDR_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_LX_IPV4_REFRESH_AGE_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_LX_PCE_PAR_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_TC_2_RF_CNTR_ALRM_E),
    ENUM_ENTRY(CPSS_PP_LX_TC_2_RF_PLC_ALRM_E),
    ENUM_ENTRY(CPSS_PP_LX_TC_2_RF_TBL_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_CLASSIFIER_HASH_PAR_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_FLOW_LKUP_PAR_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_FLOW_KEY_TBL_PAR_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_MPLS_ILM_TBL_PAR_ERR_E),
    ENUM_ENTRY(CPSS_PP_LX_MPLS_CNTR_E),
    ENUM_ENTRY(CPSS_PP_EB_TCC_E),
    ENUM_ENTRY(CPSS_PP_RX_BUFFER_QUEUE0_E),
    ENUM_ENTRY(CPSS_PP_RX_BUFFER_QUEUE1_E),
    ENUM_ENTRY(CPSS_PP_RX_BUFFER_QUEUE2_E),
    ENUM_ENTRY(CPSS_PP_RX_BUFFER_QUEUE3_E),
    ENUM_ENTRY(CPSS_PP_RX_BUFFER_QUEUE4_E),
    ENUM_ENTRY(CPSS_PP_RX_BUFFER_QUEUE5_E),
    ENUM_ENTRY(CPSS_PP_RX_BUFFER_QUEUE6_E),
    ENUM_ENTRY(CPSS_PP_RX_BUFFER_QUEUE7_E),
    ENUM_ENTRY(CPSS_PP_RX_ERR_QUEUE0_E),
    ENUM_ENTRY(CPSS_PP_RX_ERR_QUEUE1_E),
    ENUM_ENTRY(CPSS_PP_RX_ERR_QUEUE2_E),
    ENUM_ENTRY(CPSS_PP_RX_ERR_QUEUE3_E),
    ENUM_ENTRY(CPSS_PP_RX_ERR_QUEUE4_E),
    ENUM_ENTRY(CPSS_PP_RX_ERR_QUEUE5_E),
    ENUM_ENTRY(CPSS_PP_RX_ERR_QUEUE6_E),
    ENUM_ENTRY(CPSS_PP_RX_ERR_QUEUE7_E),
    ENUM_ENTRY(CPSS_PP_RX_CNTR_OVERFLOW_E),
    ENUM_ENTRY(CPSS_PP_TX_BUFFER_QUEUE_E),
    ENUM_ENTRY(CPSS_PP_TX_ERR_QUEUE_E),
    ENUM_ENTRY(CPSS_PP_TX_END_E),
    ENUM_ENTRY(CPSS_PP_PORT_LANE_PRBS_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PORT_LANE_DISPARITY_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PORT_LANE_SYMBOL_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PORT_LANE_CJR_PAT_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E),
    ENUM_ENTRY(CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E),
    ENUM_ENTRY(CPSS_PP_PORT_LANE_DETECTED_IIAII_E),
    ENUM_ENTRY(CPSS_PP_PORT_PCS_LINK_STATUS_CHANGED_E),
    ENUM_ENTRY(CPSS_PP_PORT_PCS_DESKEW_TIMEOUT_E),
    ENUM_ENTRY(CPSS_PP_PORT_PCS_DETECTED_COLUMN_IIAII_E),
    ENUM_ENTRY(CPSS_PP_PORT_PCS_DESKEW_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_PORT_PCS_ALIGN_LOCK_LOST_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_EMPTY_INC_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_AGED_PACKET_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_MAX_BUFF_REACHED_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_PORT_RX_BUFFERS_CNT_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_PORT_RX_BUFFERS_CNT_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_INVALID_ADDRESS_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_WRONG_SRC_PORT_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_MC_INC_OVERFLOW_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_MC_INC_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_TQ_PARITY_ERROR_PORT_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_RX_MEM_READ_ECC_ERROR_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_VLT_ECC_ERR_E),
    ENUM_ENTRY(CPSS_PP_BM_EGRESS_MISC_E),
    ENUM_ENTRY(CPSS_PP_PEX_DL_DOWN_TX_ACC_ERR_E),
    ENUM_ENTRY(CPSS_PP_PEX_MASTER_DISABLED_E),
    ENUM_ENTRY(CPSS_PP_PEX_ERROR_WR_TO_REG_E),
    ENUM_ENTRY(CPSS_PP_PEX_HIT_DEFAULT_WIN_ERR_E),
    ENUM_ENTRY(CPSS_PP_PEX_COR_ERROR_DET_E),
    ENUM_ENTRY(CPSS_PP_PEX_NON_FATAL_ERROR_DET_E),
    ENUM_ENTRY(CPSS_PP_PEX_FATAL_ERROR_DET_E),
    ENUM_ENTRY(CPSS_PP_PEX_DSTATE_CHANGED_E),
    ENUM_ENTRY(CPSS_PP_PEX_BIST_E),
    ENUM_ENTRY(CPSS_PP_PEX_RCV_ERROR_FATAL_E),
    ENUM_ENTRY(CPSS_PP_PEX_RCV_ERROR_NON_FATAL_E),
    ENUM_ENTRY(CPSS_PP_PEX_RCV_ERROR_COR_E),
    ENUM_ENTRY(CPSS_PP_PEX_RCV_CRS_E),
    ENUM_ENTRY(CPSS_PP_PEX_PEX_SLAVE_HOT_RESET_E),
    ENUM_ENTRY(CPSS_PP_PEX_PEX_SLAVE_DISABLE_LINK_E),
    ENUM_ENTRY(CPSS_PP_PEX_PEX_SLAVE_LOOPBACK_E),
    ENUM_ENTRY(CPSS_PP_PEX_PEX_LINK_FAIL_E),
    ENUM_ENTRY(CPSS_PP_PEX_RCV_A_E),
    ENUM_ENTRY(CPSS_PP_PEX_RCV_B_E),
    ENUM_ENTRY(CPSS_PP_PEX_RCV_C_E),
    ENUM_ENTRY(CPSS_PP_PEX_RCV_D_E),
    ENUM_ENTRY(CPSS_PP_BUF_MEM_MAC_ERROR_E),
    ENUM_ENTRY(CPSS_PP_BUF_MEM_ONE_ECC_ERROR_E),
    ENUM_ENTRY(CPSS_PP_BUF_MEM_TWO_OR_MORE_ECC_ERRORS_E),
    ENUM_ENTRY(CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E),
    ENUM_ENTRY(CPSS_PP_POLICER_DATA_ERR_E),
    ENUM_ENTRY(CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E),
    ENUM_ENTRY(CPSS_PP_POLICER_IPFIX_ALARM_E),
    ENUM_ENTRY(CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E),
    ENUM_ENTRY(CPSS_PP_PCL_TCC_ECC_ERR_E),
    ENUM_ENTRY(CPSS_PP_PCL_MG_ADDR_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_PCL_LOOKUP_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PCL_ACTION_ERROR_DETECTED_E),
    ENUM_ENTRY(CPSS_PP_PCL_MG_LOOKUP_RESULTS_READY_E),
    ENUM_ENTRY(CPSS_PP_PCL_INLIF_TABLE_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PCL_CONFIG_TABLE_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PCL_LOOKUP_FIFO_FULL_E),
    ENUM_ENTRY(CPSS_PP_CNC_WRAPAROUND_BLOCK_E),
    ENUM_ENTRY(CPSS_PP_CNC_DUMP_FINISHED_E),
    ENUM_ENTRY(CPSS_PP_SCT_RATE_LIMITER_E),
    ENUM_ENTRY(CPSS_PP_EGRESS_SFLOW_E),
    ENUM_ENTRY(CPSS_PP_INGRESS_SFLOW_SAMPLED_CNTR_E),
    ENUM_ENTRY(CPSS_PP_EGRESS_SFLOW_SAMPLED_CNTR_E),
    ENUM_ENTRY(CPSS_PP_TTI_CPU_ADDRESS_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_TTI_ACCESS_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_DIT_MC_DIT_TTL_TRHOLD_EXCEED_E),
    ENUM_ENTRY(CPSS_PP_DIT_UC_DIT_TTL_TRHOLD_EXCEED_E),
    ENUM_ENTRY(CPSS_PP_DIT_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_DIT_MC0_PARITY_ERROR_E),
    ENUM_ENTRY(CPSS_PP_DIT_INT_TABLE_ECC_ERROR_CNTR_E),
    ENUM_ENTRY(CPSS_PP_DIT_MC0_IP_TV_PARITY_ERROR_CNTR_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_INT_SUM_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_IO_ERR_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_RX_PARITY_ERR_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_MG_CMD_EXE_DONE_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_NEW_DATA_IN_MAILBOX_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_MG_WRONG_EXP_RES_LEN_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_RSP_IDQ_EMPTY_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_PHASE_ERROR_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_TCAM_TRAINING_SEQUENCE_DONE_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_MEMORY_PARITY_ERROR_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_MEMORY_ECC_ERROR_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_LOW_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_HIGH_E),
    ENUM_ENTRY(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_MG_CMD_DONE_E),
    ENUM_ENTRY(CPSS_PP_VOQ_DROP_DEV_EN_E),
    ENUM_ENTRY(CPSS_PP_VOQ_GLOBAL_DESC_FULL_E),
    ENUM_ENTRY(CPSS_PP_VOQ_ADDR_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_VOQ_ECC_ERR_E),
    ENUM_ENTRY(CPSS_PP_VOQ_GLOBAL_BUF_FULL_E),
    ENUM_ENTRY(CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_RL_E),
    ENUM_ENTRY(CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_RL_E),
    ENUM_ENTRY(CPSS_PP_XBAR_RX_CELL_CNTR_RL_E),
    ENUM_ENTRY(CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL_E),
    ENUM_ENTRY(CPSS_PP_XBAR_SHP_TX_DROP_CNTR_RL_E),
    ENUM_ENTRY(CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_RL_E),
    ENUM_ENTRY(CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_INC_E),
    ENUM_ENTRY(CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_INC_E),
    ENUM_ENTRY(CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_INC_E),
    ENUM_ENTRY(CPSS_PP_XBAR_RX_CELL_CNTR_INC_E),
    ENUM_ENTRY(CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC_E),
    ENUM_ENTRY(CPSS_PP_XBAR_SHP_TX_DROP_CNTR_INC_E),
    ENUM_ENTRY(CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF_E),
    ENUM_ENTRY(CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON_E),
    ENUM_ENTRY(CPSS_PP_XBAR_INVALID_ADDR_E),
    ENUM_ENTRY(CPSS_PP_XBAR_TX_CELL_CNTR_RL_E),
    ENUM_ENTRY(CPSS_PP_XBAR_TX_CELL_CNTR_INC_E),
    ENUM_ENTRY(CPSS_PP_CRX_MAIL_ARRIVED_E),
    ENUM_ENTRY(CPSS_PP_CRX_READ_EMPTY_CPU_E),
    ENUM_ENTRY(CPSS_PP_CRX_DATA_DROP_E),
    ENUM_ENTRY(CPSS_PP_CRX_CPU_DROP_E),
    ENUM_ENTRY(CPSS_PP_CRX_AGED_OUT_CONTEXT_E),
    ENUM_ENTRY(CPSS_PP_CRX_CRC_DROP_E),
    ENUM_ENTRY(CPSS_PP_CRX_PACKET_LENGTH_DROP_E),
    ENUM_ENTRY(CPSS_PP_CRX_MAX_BUFFERS_DROP_E),
    ENUM_ENTRY(CPSS_PP_CRX_BAD_CONTEXT_DROP_E),
    ENUM_ENTRY(CPSS_PP_CRX_NO_BUFFERS_DROP_E),
    ENUM_ENTRY(CPSS_PP_CRX_MULTICAST_CONGESTION_DROP_E),
    ENUM_ENTRY(CPSS_PP_CRX_DATA_FIFO_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_CRX_DATA_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_CRX_CPU_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_CRX_CPU_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_CRX_E2E_FIFO_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_CRX_E2E_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_CRX_UNKNOWN_CELL_TYPE_E),
    ENUM_ENTRY(CPSS_PP_CRX_COUNTER_BAD_ADDR_E),
    ENUM_ENTRY(CPSS_PP_CRX_PSM_BAD_ADDR_E),
    ENUM_ENTRY(CPSS_PP_TXD_SEG_FIFO_PARITY_ERR_E),
    ENUM_ENTRY(CPSS_PP_TXD_CPU_MAIL_SENT_E),
    ENUM_ENTRY(CPSS_PP_TXD_SEG_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_TXD_WRONG_CH_TO_FABRIC_DIST_E),
    ENUM_ENTRY(CPSS_PP_IPVX_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_IPVX_ADDRESS_OUT_OF_MEMORY_E),
    ENUM_ENTRY(CPSS_PP_IPVX_LPM_DATA_ERROR_E),
    ENUM_ENTRY(CPSS_PP_STAT_INF_TX_SYNC_FIFO_FULL_E),
    ENUM_ENTRY(CPSS_PP_STAT_INF_TX_SYNC_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_STAT_INF_TX_SYNC_FIFO_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_STAT_INF_RX_SYNC_FIFO_FULL_E),
    ENUM_ENTRY(CPSS_PP_STAT_INF_RX_SYNC_FIFO_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_STAT_INF_RX_SYNC_FIFO_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_PING_RECEIVED_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_PING_SENT_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_MAC_TX_OVERRUN_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_MAC_TX_UNDERRUN_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_RX_ERR_CNTR_RL_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_BAD_PCS_TO_MAC_REFORMAT_RX_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_BAD_HEAD_CELL_RX_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_BAD_LENGTH_CELL_RX_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_FC_CELL_RX_INT_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_FC_CELL_TX_INT_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC0_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC1_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC2_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC3_E),
    ENUM_ENTRY(CPSS_PP_HGLINK_ADDR_OUT_OF_RANGE_E),
    ENUM_ENTRY(CPSS_PP_TCC_TCAM_ERROR_DETECTED_E),
    ENUM_ENTRY(CPSS_PP_TCC_TCAM_BIST_FAILED_E),
    ENUM_ENTRY(CPSS_PP_BCN_COUNTER_WRAP_AROUND_ERR_E),
    ENUM_ENTRY(CPSS_PP_GTS_GLOBAL_FIFO_FULL_E),
    ENUM_ENTRY(CPSS_PP_GTS_VALID_TIME_SAMPLE_MESSAGE_E),
    ENUM_ENTRY(CPSS_PP_CRITICAL_HW_ERROR_E),
    ENUM_ENTRY(CPSS_PP_PORT_PRBS_ERROR_QSGMII_E),
    ENUM_ENTRY(CPSS_PP_PORT_802_3_AP_E),
    ENUM_ENTRY(CPSS_PP_PORT_802_3_AP_CORE_1_E),
    ENUM_ENTRY(CPSS_PP_PORT_802_3_AP_CORE_2_E),
    ENUM_ENTRY(CPSS_PP_PORT_802_3_AP_CORE_3_E),
    ENUM_ENTRY(CPSS_PP_PORT_802_3_AP_CORE_4_E),
    ENUM_ENTRY(CPSS_PP_PORT_802_3_AP_CORE_5_E),
    ENUM_ENTRY(CPSS_PP_PORT_802_3_AP_CORE_6_E),
    ENUM_ENTRY(CPSS_PP_PORT_802_3_AP_CORE_7_E),
    ENUM_ENTRY(CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E),
    ENUM_ENTRY(CPSS_PP_DATA_INTEGRITY_ERROR_E),
    ENUM_ENTRY(CPSS_PP_MAC_BANK_LEARN_COUNTERS_OVERFLOW_E),
    ENUM_ENTRY(CPSS_PP_PORT_EEE_E),
    ENUM_ENTRY(CPSS_PP_OAM_EXCEPTION_KEEPALIVE_AGING_E),
    ENUM_ENTRY(CPSS_PP_OAM_EXCEPTION_EXCESS_KEEPALIVE_E),
    ENUM_ENTRY(CPSS_PP_OAM_EXCEPTION_INVALID_KEEPALIVE_E),
    ENUM_ENTRY(CPSS_PP_OAM_EXCEPTION_RDI_STATUS_E),
    ENUM_ENTRY(CPSS_PP_OAM_EXCEPTION_TX_PERIOD_E),
    ENUM_ENTRY(CPSS_PP_OAM_EXCEPTION_MEG_LEVEL_E),
    ENUM_ENTRY(CPSS_PP_OAM_EXCEPTION_SOURCE_INTERFACE_E),
    ENUM_ENTRY(CPSS_PP_OAM_ILLEGAL_ENTRY_INDEX_E),
    ENUM_ENTRY(CPSS_PP_UNI_EV_DUMMY_MAX_E),
    ENUM_ENTRY(CPSS_PP_UNI_EV_MAX_E),
    ENUM_ENTRY(CPSS_XBAR_UNI_EV_MIN_E),
    ENUM_ENTRY(CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E),
    ENUM_ENTRY(CPSS_XBAR_HG_LANE_COMMA_ALIGN_CHANGE_E),
    ENUM_ENTRY(CPSS_XBAR_GPORT_INVALID_TRG_DROP_CNT_INC_E),
    ENUM_ENTRY(CPSS_XBAR_GPORT_DIAG_CELL_RX_CNTR_INC_E),
    ENUM_ENTRY(CPSS_XBAR_MISC_E),
    ENUM_ENTRY(CPSS_XBAR_HG_PING_RECEIVED_E),
    ENUM_ENTRY(CPSS_XBAR_HOST_RX_FIFO_PEND_E),
    ENUM_ENTRY(CPSS_XBAR_HOST_SEND_CELL_E),
    ENUM_ENTRY(CPSS_XBAR_HG_ALIGNMENT_LOCK_CHANGE_E),
    ENUM_ENTRY(CPSS_XBAR_HG_PING_E),
    ENUM_ENTRY(CPSS_XBAR_SD_INV_ADDR_E),
    ENUM_ENTRY(CPSS_XBAR_SXBAR_INV_ADDR_E),
    ENUM_ENTRY(CPSS_XBAR_S_INV_ADDR_E),
    ENUM_ENTRY(CPSS_XBAR_G_INV_ADDR_E),
    ENUM_ENTRY(CPSS_XBAR_MC_INV_ADDR_E),
    ENUM_ENTRY(CPSS_XBAR_UNI_EV_DUMMY_MAX_E),
    ENUM_ENTRY(CPSS_XBAR_UNI_EV_MAX_E),
    ENUM_ENTRY(CPSS_FA_UNI_EV_MIN_E),
    ENUM_ENTRY(CPSS_FA_UC_PRIO_BUF_FULL_E),
    ENUM_ENTRY(CPSS_FA_MC_PRIO_BUF_FULL_E),
    ENUM_ENTRY(CPSS_FA_UC_RX_BUF_FULL_E),
    ENUM_ENTRY(CPSS_FA_MC_RX_BUF_FULL_E),
    ENUM_ENTRY(CPSS_FA_WD_FPORT_GRP_E),
    ENUM_ENTRY(CPSS_FA_MC_WD_EXP_E),
    ENUM_ENTRY(CPSS_FA_GLBL_DESC_FULL_E),
    ENUM_ENTRY(CPSS_FA_MAC_UPD_THRS_E),
    ENUM_ENTRY(CPSS_FA_DROP_DEV_EN_E),
    ENUM_ENTRY(CPSS_FA_MISC_I2C_TIMEOUT_E),
    ENUM_ENTRY(CPSS_FA_MISC_STATUS_E),
    ENUM_ENTRY(CPSS_FA_UNI_EV_DUMMY_MAX_E),
    ENUM_ENTRY(CPSS_FA_UNI_EV_MAX_E),
    ENUM_ENTRY(CPSS_DRAGONITE_UNI_EV_MIN_E),
    ENUM_ENTRY(CPSS_DRAGONITE_PORT_ON_E),
    ENUM_ENTRY(CPSS_DRAGONITE_PORT_OFF_E),
    ENUM_ENTRY(CPSS_DRAGONITE_DETECT_FAIL_E),
    ENUM_ENTRY(CPSS_DRAGONITE_PORT_FAULT_E),
    ENUM_ENTRY(CPSS_DRAGONITE_PORT_UDL_E),
    ENUM_ENTRY(CPSS_DRAGONITE_PORT_FAULT_ON_STARTUP_E),
    ENUM_ENTRY(CPSS_DRAGONITE_PORT_PM_E),
    ENUM_ENTRY(CPSS_DRAGONITE_POWER_DENIED_E),
    ENUM_ENTRY(CPSS_DRAGONITE_OVER_TEMP_E),
    ENUM_ENTRY(CPSS_DRAGONITE_TEMP_ALARM_E),
    ENUM_ENTRY(CPSS_DRAGONITE_DEVICE_FAULT_E),
    ENUM_ENTRY(CPSS_DRAGONITE_OVER_CONSUM_E),
    ENUM_ENTRY(CPSS_DRAGONITE_VMAIN_LOW_AF_E),
    ENUM_ENTRY(CPSS_DRAGONITE_VMAIN_LOW_AT_E),
    ENUM_ENTRY(CPSS_DRAGONITE_VMAIN_HIGH_E),
    ENUM_ENTRY(CPSS_DRAGONITE_READ_EVENT_E),
    ENUM_ENTRY(CPSS_DRAGONITE_WRITE_EVENT_E),
    ENUM_ENTRY(CPSS_DRAGONITE_ERROR_E),
    ENUM_ENTRY(CPSS_DRAGONITE_UNI_EV_DUMMY_MAX_E),
    ENUM_ENTRY(CPSS_DRAGONITE_UNI_EV_MAX_E),
    ENUM_ENTRY(CPSS_UNI_EV_DUMMY_MAX_E),
    ENUM_ENTRY(CPSS_UNI_EVENT_COUNT_E),
    ENUM_ENTRY(CPSS_UNI_RSRVD_EVENT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_UNI_EV_CAUSE_ENT);

void prv_lua_to_c_CPSS_MAC_ENTRY_STC(
    lua_State *L,
    CPSS_MAC_ENTRY_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, macAddr, GT_ETHERADDR);
    F_STRUCT(val, -1, dstInterface, CPSS_INTERFACE_INFO_STC);
    F_BOOL(val, -1, isStatic);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_NUMBER(val, -1, srcTc, GT_U8);
    F_NUMBER(val, -1, dstTc, GT_U8);
    F_ENUM(val, -1, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    F_ENUM(val, -1, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    F_BOOL(val, -1, saClass);
    F_BOOL(val, -1, daClass);
    F_BOOL(val, -1, saCib);
    F_BOOL(val, -1, daCib);
    F_BOOL(val, -1, daRoute);
    F_BOOL(val, -1, cosIpv4En);
    F_BOOL(val, -1, mirrorToRxAnalyzerPortEn);
}

void prv_c_to_lua_CPSS_MAC_ENTRY_STC(
    lua_State *L,
    CPSS_MAC_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, macAddr, GT_ETHERADDR);
    FO_STRUCT(val, t, dstInterface, CPSS_INTERFACE_INFO_STC);
    FO_BOOL(val, t, isStatic);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_NUMBER(val, t, srcTc, GT_U8);
    FO_NUMBER(val, t, dstTc, GT_U8);
    FO_ENUM(val, t, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    FO_ENUM(val, t, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    FO_BOOL(val, t, saClass);
    FO_BOOL(val, t, daClass);
    FO_BOOL(val, t, saCib);
    FO_BOOL(val, t, daCib);
    FO_BOOL(val, t, daRoute);
    FO_BOOL(val, t, cosIpv4En);
    FO_BOOL(val, t, mirrorToRxAnalyzerPortEn);
    lua_settop(L, t);
}
add_mgm_type(CPSS_MAC_ENTRY_STC);

enumDescr enumDescr_CPSS_TUNNEL_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_TUNNEL_IPV4_OVER_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_IPV6_OVER_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_IP_OVER_MPLS_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERNET_OVER_MPLS_E),
    ENUM_ENTRY(CPSS_TUNNEL_X_OVER_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_X_OVER_GRE_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_X_OVER_MPLS_E),
    ENUM_ENTRY(CPSS_TUNNEL_IP_OVER_X_E),
    ENUM_ENTRY(CPSS_TUNNEL_MAC_IN_MAC_E),
    ENUM_ENTRY(CPSS_TUNNEL_TRILL_E),
    ENUM_ENTRY(CPSS_TUNNEL_Q_IN_Q_E),
    ENUM_ENTRY(CPSS_TUNNEL_GENERIC_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_GENERIC_IPV6_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TUNNEL_TYPE_ENT);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_POWER_SOURCE_UNKNOWN_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_POWER_SOURCE_PRIMARY_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_POWER_SOURCE_BACKUP_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_POWER_SOURCE_RESERVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT);

enumDescr enumDescr_CPSS_PORT_FLOW_CONTROL_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_FLOW_CONTROL_DISABLE_E),
    ENUM_ENTRY(CPSS_PORT_FLOW_CONTROL_RX_TX_E),
    ENUM_ENTRY(CPSS_PORT_FLOW_CONTROL_RX_ONLY_E),
    ENUM_ENTRY(CPSS_PORT_FLOW_CONTROL_TX_ONLY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_FLOW_CONTROL_ENT);

void prv_lua_to_c_CPSS_TM_B_NODE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_B_NODE_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, shapingProfilePtr, GT_U32);
    F_NUMBER(val, -1, quantum, GT_U32);
    F_NUMBER(val, -1, dropProfileInd, GT_U32);
    F_ENUM(val, -1, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_NODE_ENT);
    F_ARRAY_START(val, -1, schdModeArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_ENUM(val, schdModeArr, idx, CPSS_TM_SCHD_MODE_ENT);
        }
    }
    F_ARRAY_END(val, -1, schdModeArr);
    F_NUMBER(val, -1, numOfChildren, GT_U32);
}

void prv_c_to_lua_CPSS_TM_B_NODE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_B_NODE_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, shapingProfilePtr, GT_U32);
    FO_NUMBER(val, t, quantum, GT_U32);
    FO_NUMBER(val, t, dropProfileInd, GT_U32);
    FO_ENUM(val, t, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_NODE_ENT);
    FO_ARRAY_START(val, t, schdModeArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_ENUM(val, schdModeArr, idx, CPSS_TM_SCHD_MODE_ENT);
        }
    }
    FO_ARRAY_END(val, t, schdModeArr);
    FO_NUMBER(val, t, numOfChildren, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_B_NODE_PARAMS_STC);

enumDescr enumDescr_CPSS_BM_POOL_ALIGNMENT_ENT[] = {
    ENUM_ENTRY(CPSS_BM_POOL_1_BYTE_ALIGNMENT_E),
    ENUM_ENTRY(CPSS_BM_POOL_4_BYTE_ALIGNMENT_E),
    ENUM_ENTRY(CPSS_BM_POOL_8_BYTE_ALIGNMENT_E),
    ENUM_ENTRY(CPSS_BM_POOL_16_BYTE_ALIGNMENT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BM_POOL_ALIGNMENT_ENT);

enumDescr enumDescr_CPSS_LOG_LIB_ENT[] = {
    ENUM_ENTRY(CPSS_LOG_LIB_APP_DRIVER_CALL_E),
    ENUM_ENTRY(CPSS_LOG_LIB_BRIDGE_E),
    ENUM_ENTRY(CPSS_LOG_LIB_CNC_E),
    ENUM_ENTRY(CPSS_LOG_LIB_CONFIG_E),
    ENUM_ENTRY(CPSS_LOG_LIB_COS_E),
    ENUM_ENTRY(CPSS_LOG_LIB_HW_INIT_E),
    ENUM_ENTRY(CPSS_LOG_LIB_CSCD_E),
    ENUM_ENTRY(CPSS_LOG_LIB_CUT_THROUGH_E),
    ENUM_ENTRY(CPSS_LOG_LIB_DIAG_E),
    ENUM_ENTRY(CPSS_LOG_LIB_FABRIC_E),
    ENUM_ENTRY(CPSS_LOG_LIB_IP_E),
    ENUM_ENTRY(CPSS_LOG_LIB_IPFIX_E),
    ENUM_ENTRY(CPSS_LOG_LIB_IP_LPM_E),
    ENUM_ENTRY(CPSS_LOG_LIB_L2_MLL_E),
    ENUM_ENTRY(CPSS_LOG_LIB_LOGICAL_TARGET_E),
    ENUM_ENTRY(CPSS_LOG_LIB_LPM_E),
    ENUM_ENTRY(CPSS_LOG_LIB_MIRROR_E),
    ENUM_ENTRY(CPSS_LOG_LIB_MULTI_PORT_GROUP_E),
    ENUM_ENTRY(CPSS_LOG_LIB_NETWORK_IF_E),
    ENUM_ENTRY(CPSS_LOG_LIB_NST_E),
    ENUM_ENTRY(CPSS_LOG_LIB_OAM_E),
    ENUM_ENTRY(CPSS_LOG_LIB_PCL_E),
    ENUM_ENTRY(CPSS_LOG_LIB_PHY_E),
    ENUM_ENTRY(CPSS_LOG_LIB_POLICER_E),
    ENUM_ENTRY(CPSS_LOG_LIB_PORT_E),
    ENUM_ENTRY(CPSS_LOG_LIB_PROTECTION_E),
    ENUM_ENTRY(CPSS_LOG_LIB_PTP_E),
    ENUM_ENTRY(CPSS_LOG_LIB_SYSTEM_RECOVERY_E),
    ENUM_ENTRY(CPSS_LOG_LIB_TCAM_E),
    ENUM_ENTRY(CPSS_LOG_LIB_TM_GLUE_E),
    ENUM_ENTRY(CPSS_LOG_LIB_TRUNK_E),
    ENUM_ENTRY(CPSS_LOG_LIB_TTI_E),
    ENUM_ENTRY(CPSS_LOG_LIB_TUNNEL_E),
    ENUM_ENTRY(CPSS_LOG_LIB_VNT_E),
    ENUM_ENTRY(CPSS_LOG_LIB_RESOURCE_MANAGER_E),
    ENUM_ENTRY(CPSS_LOG_LIB_VERSION_E),
    ENUM_ENTRY(CPSS_LOG_LIB_TM_E),
    ENUM_ENTRY(CPSS_LOG_LIB_SMI_E),
    ENUM_ENTRY(CPSS_LOG_LIB_INIT_E),
    ENUM_ENTRY(CPSS_LOG_LIB_DRAGONITE_E),
    ENUM_ENTRY(CPSS_LOG_LIB_VIRTUAL_TCAM_E),
    ENUM_ENTRY(CPSS_LOG_LIB_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LOG_LIB_ENT);

enumDescr enumDescr_CPSS_VCT_ACTION_ENT[] = {
    ENUM_ENTRY(CPSS_VCT_START_E),
    ENUM_ENTRY(CPSS_VCT_GET_RES_E),
    ENUM_ENTRY(CPSS_VCT_ABORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_ACTION_ENT);

enumDescr enumDescr_CPSS_L4_PROTOCOL_ENT[] = {
    ENUM_ENTRY(CPSS_L4_PROTOCOL_TCP_E),
    ENUM_ENTRY(CPSS_L4_PROTOCOL_UDP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_L4_PROTOCOL_ENT);

void prv_lua_to_c_CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC(
    lua_State *L,
    CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC *val
)
{
    F_NUMBER(val, -1, portGroupId, GT_U32);
    F_NUMBER(val, -1, mppmId, GT_U32);
    F_NUMBER(val, -1, bankId, GT_U32);
}

void prv_c_to_lua_CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC(
    lua_State *L,
    CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, portGroupId, GT_U32);
    FO_NUMBER(val, t, mppmId, GT_U32);
    FO_NUMBER(val, t, bankId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC);

enumDescr enumDescr_CPSS_DRAM_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DRAM_NOT_USED_E),
    ENUM_ENTRY(CPSS_DRAM_64KB_E),
    ENUM_ENTRY(CPSS_DRAM_128KB_E),
    ENUM_ENTRY(CPSS_DRAM_8MB_E),
    ENUM_ENTRY(CPSS_DRAM_16MB_E),
    ENUM_ENTRY(CPSS_DRAM_32MB_E),
    ENUM_ENTRY(CPSS_DRAM_64MB_E),
    ENUM_ENTRY(CPSS_DRAM_128MB_E),
    ENUM_ENTRY(CPSS_DRAM_256MB_E),
    ENUM_ENTRY(CPSS_DRAM_512MB_E),
    ENUM_ENTRY(CPSS_DRAM_1GB_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DRAM_SIZE_ENT);

enumDescr enumDescr_CPSS_BUF_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_BUF_1536_E),
    ENUM_ENTRY(CPSS_BUF_2K_E),
    ENUM_ENTRY(CPSS_BUF_10K_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BUF_MODE_ENT);

enumDescr enumDescr_CPSS_BRG_SECUR_BREACH_EVENTS_ENT[] = {
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NA_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_ARP_SA_MISMATCH_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_OVER_MC_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_BRIDGE_ACCESS_MATRIX_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_SECURE_AUTO_LEARN_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_FRAGMENT_ICMP_TYPE_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FIN_URG_PSH_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_FIN_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_RST_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_TCP_UDP_SRC_DST_PORT_ZERO),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_EVENTS_AUTO_LEARN_NO_RELEARN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BRG_SECUR_BREACH_EVENTS_ENT);

enumDescr enumDescr_CPSS_PCL_LOOKUP_NUMBER_ENT[] = {
    ENUM_ENTRY(CPSS_PCL_LOOKUP_0_E),
    ENUM_ENTRY(CPSS_PCL_LOOKUP_1_E),
    ENUM_ENTRY(CPSS_PCL_LOOKUP_0_0_E),
    ENUM_ENTRY(CPSS_PCL_LOOKUP_0_1_E),
    ENUM_ENTRY(CPSS_PCL_LOOKUP_NUMBER_0_E),
    ENUM_ENTRY(CPSS_PCL_LOOKUP_NUMBER_1_E),
    ENUM_ENTRY(CPSS_PCL_LOOKUP_NUMBER_2_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PCL_LOOKUP_NUMBER_ENT);

enumDescr enumDescr_CPSS_PORT_LOCK_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_LOCK_FRWRD_E),
    ENUM_ENTRY(CPSS_LOCK_DROP_E),
    ENUM_ENTRY(CPSS_LOCK_TRAP_E),
    ENUM_ENTRY(CPSS_LOCK_MIRROR_E),
    ENUM_ENTRY(CPSS_LOCK_SOFT_DROP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_LOCK_CMD_ENT);

void prv_lua_to_c_CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC(
    lua_State *L,
    CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, dp0MinThreshold, GT_U16);
    F_NUMBER(val, -1, dp1MinThreshold, GT_U16);
    F_NUMBER(val, -1, dp2MinThreshold, GT_U16);
    F_NUMBER(val, -1, dp0MaxThreshold, GT_U16);
    F_NUMBER(val, -1, dp1MaxThreshold, GT_U16);
    F_NUMBER(val, -1, dp2MaxThreshold, GT_U16);
    F_NUMBER(val, -1, dp0MaxProb, GT_U8);
    F_NUMBER(val, -1, dp1MaxProb, GT_U8);
    F_NUMBER(val, -1, dp2MaxProb, GT_U8);
    F_NUMBER(val, -1, RedQWeightFactor, GT_U8);
}

void prv_c_to_lua_CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC(
    lua_State *L,
    CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, dp0MinThreshold, GT_U16);
    FO_NUMBER(val, t, dp1MinThreshold, GT_U16);
    FO_NUMBER(val, t, dp2MinThreshold, GT_U16);
    FO_NUMBER(val, t, dp0MaxThreshold, GT_U16);
    FO_NUMBER(val, t, dp1MaxThreshold, GT_U16);
    FO_NUMBER(val, t, dp2MaxThreshold, GT_U16);
    FO_NUMBER(val, t, dp0MaxProb, GT_U8);
    FO_NUMBER(val, t, dp1MaxProb, GT_U8);
    FO_NUMBER(val, t, dp2MaxProb, GT_U8);
    FO_NUMBER(val, t, RedQWeightFactor, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC);

enumDescr enumDescr_CPSS_LED_BLINK_DUTY_CYCLE_ENT[] = {
    ENUM_ENTRY(CPSS_LED_BLINK_DUTY_CYCLE_0_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DUTY_CYCLE_1_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DUTY_CYCLE_2_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DUTY_CYCLE_3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LED_BLINK_DUTY_CYCLE_ENT);

void prv_lua_to_c_CPSS_VCT_CABLE_STATUS_STC(
    lua_State *L,
    CPSS_VCT_CABLE_STATUS_STC *val
)
{
    F_ARRAY_START(val, -1, cableStatus);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_STRUCT(val, cableStatus, idx, CPSS_VCT_STATUS_STC);
        }
    }
    F_ARRAY_END(val, -1, cableStatus);
    F_ENUM(val, -1, normalCableLen, CPSS_VCT_NORMAL_CABLE_LEN_ENT);
    F_ENUM(val, -1, phyType, CPSS_VCT_PHY_TYPES_ENT);
}

void prv_c_to_lua_CPSS_VCT_CABLE_STATUS_STC(
    lua_State *L,
    CPSS_VCT_CABLE_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, cableStatus);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_STRUCT(val, cableStatus, idx, CPSS_VCT_STATUS_STC);
        }
    }
    FO_ARRAY_END(val, t, cableStatus);
    FO_ENUM(val, t, normalCableLen, CPSS_VCT_NORMAL_CABLE_LEN_ENT);
    FO_ENUM(val, t, phyType, CPSS_VCT_PHY_TYPES_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VCT_CABLE_STATUS_STC);

enumDescr enumDescr_CPSS_MAC_QOS_RESOLVE_ENT[] = {
    ENUM_ENTRY(CPSS_MAC_QOS_GET_FROM_DA_E),
    ENUM_ENTRY(CPSS_MAC_QOS_GET_FROM_SA_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_MAC_QOS_RESOLVE_ENT);

enumDescr enumDescr_CPSS_PORT_RX_FC_PROFILE_SET_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_RX_FC_PROFILE_1_E),
    ENUM_ENTRY(CPSS_PORT_RX_FC_PROFILE_2_E),
    ENUM_ENTRY(CPSS_PORT_RX_FC_PROFILE_3_E),
    ENUM_ENTRY(CPSS_PORT_RX_FC_PROFILE_4_E),
    ENUM_ENTRY(CPSS_PORT_RX_FC_PROFILE_5_E),
    ENUM_ENTRY(CPSS_PORT_RX_FC_PROFILE_6_E),
    ENUM_ENTRY(CPSS_PORT_RX_FC_PROFILE_7_E),
    ENUM_ENTRY(CPSS_PORT_RX_FC_PROFILE_8_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_RX_FC_PROFILE_SET_ENT);

void prv_lua_to_c_CPSS_TM_CTL_LAD_INF_PARAM_STC(
    lua_State *L,
    CPSS_TM_CTL_LAD_INF_PARAM_STC *val
)
{
    F_NUMBER(val, -1, minPkgSize, GT_U32);
    F_NUMBER(val, -1, pagesPerBank, GT_U32);
    F_NUMBER(val, -1, pkgesPerBank, GT_U32);
    F_NUMBER(val, -1, portChunksEmitPerSel, GT_U32);
    F_ARRAY_START(val, -1, bapIDs);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, bapIDs, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, bapIDs);
}

void prv_c_to_lua_CPSS_TM_CTL_LAD_INF_PARAM_STC(
    lua_State *L,
    CPSS_TM_CTL_LAD_INF_PARAM_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, minPkgSize, GT_U32);
    FO_NUMBER(val, t, pagesPerBank, GT_U32);
    FO_NUMBER(val, t, pkgesPerBank, GT_U32);
    FO_NUMBER(val, t, portChunksEmitPerSel, GT_U32);
    FO_ARRAY_START(val, t, bapIDs);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, bapIDs, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, bapIDs);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_CTL_LAD_INF_PARAM_STC);

void prv_lua_to_c_CPSS_VCT_EXTENDED_STATUS_STC(
    lua_State *L,
    CPSS_VCT_EXTENDED_STATUS_STC *val
)
{
    F_BOOL(val, -1, isValid);
    F_ARRAY_START(val, -1, pairSwap);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_ENUM(val, pairSwap, idx, CPSS_VCT_PAIR_SWAP_ENT);
        }
    }
    F_ARRAY_END(val, -1, pairSwap);
    F_ARRAY_START(val, -1, pairPolarity);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_ENUM(val, pairPolarity, idx, CPSS_VCT_POLARITY_SWAP_ENT);
        }
    }
    F_ARRAY_END(val, -1, pairPolarity);
    F_STRUCT(val, -1, pairSkew, CPSS_VCT_PAIR_SKEW_STC);
}

void prv_c_to_lua_CPSS_VCT_EXTENDED_STATUS_STC(
    lua_State *L,
    CPSS_VCT_EXTENDED_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, isValid);
    FO_ARRAY_START(val, t, pairSwap);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_ENUM(val, pairSwap, idx, CPSS_VCT_PAIR_SWAP_ENT);
        }
    }
    FO_ARRAY_END(val, t, pairSwap);
    FO_ARRAY_START(val, t, pairPolarity);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_ENUM(val, pairPolarity, idx, CPSS_VCT_POLARITY_SWAP_ENT);
        }
    }
    FO_ARRAY_END(val, t, pairPolarity);
    FO_STRUCT(val, t, pairSkew, CPSS_VCT_PAIR_SKEW_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VCT_EXTENDED_STATUS_STC);

enumDescr enumDescr_CPSS_NST_CHECK_ENT[] = {
    ENUM_ENTRY(CPSS_NST_CHECK_TCP_SYN_DATA_E),
    ENUM_ENTRY(CPSS_NST_CHECK_TCP_OVER_MAC_MC_BC_E),
    ENUM_ENTRY(CPSS_NST_CHECK_TCP_FLAG_ZERO_E),
    ENUM_ENTRY(CPSS_NST_CHECK_TCP_FLAGS_FIN_URG_PSH_E),
    ENUM_ENTRY(CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E),
    ENUM_ENTRY(CPSS_NST_CHECK_TCP_FLAGS_SYN_RST_E),
    ENUM_ENTRY(CPSS_NST_CHECK_TCP_UDP_PORT_ZERO_E),
    ENUM_ENTRY(CPSS_NST_CHECK_TCP_ALL_E),
    ENUM_ENTRY(CPSS_NST_CHECK_FRAG_IPV4_ICMP_E),
    ENUM_ENTRY(CPSS_NST_CHECK_ARP_MAC_SA_MISMATCH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NST_CHECK_ENT);

enumDescr enumDescr_CPSS_PCL_DIRECTION_ENT[] = {
    ENUM_ENTRY(CPSS_PCL_DIRECTION_INGRESS_E),
    ENUM_ENTRY(CPSS_PCL_DIRECTION_EGRESS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PCL_DIRECTION_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC *val
)
{
    F_NUMBER(val, -1, pseAllocPower, GT_U32);
    F_ENUM(val, -1, psePriority, CPSS_GEN_DRAGONITE_PRIORITY_ENT);
    F_ENUM(val, -1, pseType, CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pseAllocPower, GT_U32);
    FO_ENUM(val, t, psePriority, CPSS_GEN_DRAGONITE_PRIORITY_ENT);
    FO_ENUM(val, t, pseType, CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC);

enumDescr enumDescr_CPSS_RX_BUFF_ALLOC_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_RX_BUFF_DYNAMIC_ALLOC_E),
    ENUM_ENTRY(CPSS_RX_BUFF_STATIC_ALLOC_E),
    ENUM_ENTRY(CPSS_RX_BUFF_NO_ALLOC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_RX_BUFF_ALLOC_METHOD_ENT);

enumDescr enumDescr_CPSS_VCT_PAIR_SWAP_ENT[] = {
    ENUM_ENTRY(CPSS_VCT_CABLE_STRAIGHT_E),
    ENUM_ENTRY(CPSS_VCT_CABLE_CROSSOVER_E),
    ENUM_ENTRY(CPSS_VCT_NOT_APPLICABLE_SWAP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_PAIR_SWAP_ENT);

void prv_lua_to_c_CPSS_RESOURCE_MAPPING_STC(
    lua_State *L,
    CPSS_RESOURCE_MAPPING_STC *val
)
{
    F_NUMBER(val, -1, sramBaseAddress, GT_UINTPTR);
    F_NUMBER(val, -1, sramSize, GT_U32);
}

void prv_c_to_lua_CPSS_RESOURCE_MAPPING_STC(
    lua_State *L,
    CPSS_RESOURCE_MAPPING_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, sramBaseAddress, GT_UINTPTR);
    FO_NUMBER(val, t, sramSize, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_RESOURCE_MAPPING_STC);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_CHIP_STATE_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_CHIP_STATE_NOT_EXISTS_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_CHIP_STATE_EXISTS_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_CHIP_STATE_NA_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_CHIP_STATE_ERROR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_CHIP_STATE_ENT);

void prv_lua_to_c_CPSS_QOS_ENTRY_STC(
    lua_State *L,
    CPSS_QOS_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, qosProfileId, GT_U32);
    F_ENUM(val, -1, assignPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_ENUM(val, -1, enableModifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    F_ENUM(val, -1, enableModifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
}

void prv_c_to_lua_CPSS_QOS_ENTRY_STC(
    lua_State *L,
    CPSS_QOS_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, qosProfileId, GT_U32);
    FO_ENUM(val, t, assignPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_ENUM(val, t, enableModifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    FO_ENUM(val, t, enableModifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_QOS_ENTRY_STC);

enumDescr enumDescr_CPSS_VCT_STATE[] = {
    ENUM_ENTRY(CPSS_VCT_READY_E),
    ENUM_ENTRY(CPSS_VCT_STARTED_E),
    ENUM_ENTRY(CPSS_VCT_REDO_PHASE_1_E),
    ENUM_ENTRY(CPSS_VCT_REDO_PHASE_2_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_STATE);

enumDescr enumDescr_CPSS_CSCD_LINK_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_CSCD_LINK_TYPE_TRUNK_E),
    ENUM_ENTRY(CPSS_CSCD_LINK_TYPE_PORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_CSCD_LINK_TYPE_ENT);

enumDescr enumDescr_CPSS_NET_TCP_UDP_PACKET_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_NET_TCP_UDP_PACKET_UC_E),
    ENUM_ENTRY(CPSS_NET_TCP_UDP_PACKET_MC_E),
    ENUM_ENTRY(CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NET_TCP_UDP_PACKET_TYPE_ENT);

enumDescr enumDescr_CPSS_TUNNEL_ETHERTYPE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE1_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_MULTICAST_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_MIM_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_TRILL_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE0_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE1_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED1_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED3_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED5_E),
    ENUM_ENTRY(CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED6_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TUNNEL_ETHERTYPE_TYPE_ENT);

enumDescr enumDescr_CPSS_EVENT_MASK_SET_ENT[] = {
    ENUM_ENTRY(CPSS_EVENT_MASK_E),
    ENUM_ENTRY(CPSS_EVENT_UNMASK_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_EVENT_MASK_SET_ENT);

enumDescr enumDescr_CPSS_PORT_DUPLEX_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_FULL_DUPLEX_E),
    ENUM_ENTRY(CPSS_PORT_HALF_DUPLEX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_DUPLEX_ENT);

void prv_lua_to_c_CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC(
    lua_State *L,
    CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC *val
)
{
    F_ENUM(val, -1, arrayType, CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT);
    F_NUMBER(val, -1, ruleIndex, GT_U32);
}

void prv_c_to_lua_CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC(
    lua_State *L,
    CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, arrayType, CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT);
    FO_NUMBER(val, t, ruleIndex, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC);

enumDescr enumDescr_CPSS_VCT_TEST_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_VCT_TEST_FAIL_E),
    ENUM_ENTRY(CPSS_VCT_NORMAL_CABLE_E),
    ENUM_ENTRY(CPSS_VCT_OPEN_CABLE_E),
    ENUM_ENTRY(CPSS_VCT_SHORT_CABLE_E),
    ENUM_ENTRY(CPSS_VCT_IMPEDANCE_MISMATCH_E),
    ENUM_ENTRY(CPSS_VCT_SHORT_WITH_PAIR0_E),
    ENUM_ENTRY(CPSS_VCT_SHORT_WITH_PAIR1_E),
    ENUM_ENTRY(CPSS_VCT_SHORT_WITH_PAIR2_E),
    ENUM_ENTRY(CPSS_VCT_SHORT_WITH_PAIR3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_TEST_STATUS_ENT);

enumDescr enumDescr_CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E),
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E),
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);

void prv_lua_to_c_CPSS_PORT_EGRESS_CNTR_STC(
    lua_State *L,
    CPSS_PORT_EGRESS_CNTR_STC *val
)
{
    F_NUMBER(val, -1, outUcFrames, GT_U32);
    F_NUMBER(val, -1, outMcFrames, GT_U32);
    F_NUMBER(val, -1, outBcFrames, GT_U32);
    F_NUMBER(val, -1, brgEgrFilterDisc, GT_U32);
    F_NUMBER(val, -1, txqFilterDisc, GT_U32);
    F_NUMBER(val, -1, outCtrlFrames, GT_U32);
    F_NUMBER(val, -1, egrFrwDropFrames, GT_U32);
    F_NUMBER(val, -1, mcFifo3_0DropPkts, GT_U32);
    F_NUMBER(val, -1, mcFifo7_4DropPkts, GT_U32);
}

void prv_c_to_lua_CPSS_PORT_EGRESS_CNTR_STC(
    lua_State *L,
    CPSS_PORT_EGRESS_CNTR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, outUcFrames, GT_U32);
    FO_NUMBER(val, t, outMcFrames, GT_U32);
    FO_NUMBER(val, t, outBcFrames, GT_U32);
    FO_NUMBER(val, t, brgEgrFilterDisc, GT_U32);
    FO_NUMBER(val, t, txqFilterDisc, GT_U32);
    FO_NUMBER(val, t, outCtrlFrames, GT_U32);
    FO_NUMBER(val, t, egrFrwDropFrames, GT_U32);
    FO_NUMBER(val, t, mcFifo3_0DropPkts, GT_U32);
    FO_NUMBER(val, t, mcFifo7_4DropPkts, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_EGRESS_CNTR_STC);

enumDescr enumDescr_CPSS_NET_PROT_ENT[] = {
    ENUM_ENTRY(CPSS_NET_PROT_UDP_E),
    ENUM_ENTRY(CPSS_NET_PROT_TCP_E),
    ENUM_ENTRY(CPSS_NET_PROT_BOTH_UDP_TCP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NET_PROT_ENT);

void prv_lua_to_c_CPSS_PORT_MAC_COUNTER_SET_STC(
    lua_State *L,
    CPSS_PORT_MAC_COUNTER_SET_STC *val
)
{
    F_STRUCT(val, -1, goodOctetsRcv, GT_U64);
    F_STRUCT(val, -1, badOctetsRcv, GT_U64);
    F_STRUCT(val, -1, macTransmitErr, GT_U64);
    F_STRUCT(val, -1, goodPktsRcv, GT_U64);
    F_STRUCT(val, -1, badPktsRcv, GT_U64);
    F_STRUCT(val, -1, brdcPktsRcv, GT_U64);
    F_STRUCT(val, -1, mcPktsRcv, GT_U64);
    F_STRUCT(val, -1, pkts64Octets, GT_U64);
    F_STRUCT(val, -1, pkts65to127Octets, GT_U64);
    F_STRUCT(val, -1, pkts128to255Octets, GT_U64);
    F_STRUCT(val, -1, pkts256to511Octets, GT_U64);
    F_STRUCT(val, -1, pkts512to1023Octets, GT_U64);
    F_STRUCT(val, -1, pkts1024tomaxOoctets, GT_U64);
    F_STRUCT(val, -1, goodOctetsSent, GT_U64);
    F_STRUCT(val, -1, goodPktsSent, GT_U64);
    F_STRUCT(val, -1, excessiveCollisions, GT_U64);
    F_STRUCT(val, -1, mcPktsSent, GT_U64);
    F_STRUCT(val, -1, brdcPktsSent, GT_U64);
    F_STRUCT(val, -1, unrecogMacCntrRcv, GT_U64);
    F_STRUCT(val, -1, fcSent, GT_U64);
    F_STRUCT(val, -1, goodFcRcv, GT_U64);
    F_STRUCT(val, -1, dropEvents, GT_U64);
    F_STRUCT(val, -1, undersizePkts, GT_U64);
    F_STRUCT(val, -1, fragmentsPkts, GT_U64);
    F_STRUCT(val, -1, oversizePkts, GT_U64);
    F_STRUCT(val, -1, jabberPkts, GT_U64);
    F_STRUCT(val, -1, macRcvError, GT_U64);
    F_STRUCT(val, -1, badCrc, GT_U64);
    F_STRUCT(val, -1, collisions, GT_U64);
    F_STRUCT(val, -1, lateCollisions, GT_U64);
    F_STRUCT(val, -1, badFcRcv, GT_U64);
    F_STRUCT(val, -1, ucPktsRcv, GT_U64);
    F_STRUCT(val, -1, ucPktsSent, GT_U64);
    F_STRUCT(val, -1, multiplePktsSent, GT_U64);
    F_STRUCT(val, -1, deferredPktsSent, GT_U64);
    F_STRUCT(val, -1, pkts1024to1518Octets, GT_U64);
    F_STRUCT(val, -1, pkts1519toMaxOctets, GT_U64);
}

void prv_c_to_lua_CPSS_PORT_MAC_COUNTER_SET_STC(
    lua_State *L,
    CPSS_PORT_MAC_COUNTER_SET_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, goodOctetsRcv, GT_U64);
    FO_STRUCT(val, t, badOctetsRcv, GT_U64);
    FO_STRUCT(val, t, macTransmitErr, GT_U64);
    FO_STRUCT(val, t, goodPktsRcv, GT_U64);
    FO_STRUCT(val, t, badPktsRcv, GT_U64);
    FO_STRUCT(val, t, brdcPktsRcv, GT_U64);
    FO_STRUCT(val, t, mcPktsRcv, GT_U64);
    FO_STRUCT(val, t, pkts64Octets, GT_U64);
    FO_STRUCT(val, t, pkts65to127Octets, GT_U64);
    FO_STRUCT(val, t, pkts128to255Octets, GT_U64);
    FO_STRUCT(val, t, pkts256to511Octets, GT_U64);
    FO_STRUCT(val, t, pkts512to1023Octets, GT_U64);
    FO_STRUCT(val, t, pkts1024tomaxOoctets, GT_U64);
    FO_STRUCT(val, t, goodOctetsSent, GT_U64);
    FO_STRUCT(val, t, goodPktsSent, GT_U64);
    FO_STRUCT(val, t, excessiveCollisions, GT_U64);
    FO_STRUCT(val, t, mcPktsSent, GT_U64);
    FO_STRUCT(val, t, brdcPktsSent, GT_U64);
    FO_STRUCT(val, t, unrecogMacCntrRcv, GT_U64);
    FO_STRUCT(val, t, fcSent, GT_U64);
    FO_STRUCT(val, t, goodFcRcv, GT_U64);
    FO_STRUCT(val, t, dropEvents, GT_U64);
    FO_STRUCT(val, t, undersizePkts, GT_U64);
    FO_STRUCT(val, t, fragmentsPkts, GT_U64);
    FO_STRUCT(val, t, oversizePkts, GT_U64);
    FO_STRUCT(val, t, jabberPkts, GT_U64);
    FO_STRUCT(val, t, macRcvError, GT_U64);
    FO_STRUCT(val, t, badCrc, GT_U64);
    FO_STRUCT(val, t, collisions, GT_U64);
    FO_STRUCT(val, t, lateCollisions, GT_U64);
    FO_STRUCT(val, t, badFcRcv, GT_U64);
    FO_STRUCT(val, t, ucPktsRcv, GT_U64);
    FO_STRUCT(val, t, ucPktsSent, GT_U64);
    FO_STRUCT(val, t, multiplePktsSent, GT_U64);
    FO_STRUCT(val, t, deferredPktsSent, GT_U64);
    FO_STRUCT(val, t, pkts1024to1518Octets, GT_U64);
    FO_STRUCT(val, t, pkts1519toMaxOctets, GT_U64);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_MAC_COUNTER_SET_STC);

void prv_lua_to_c_CPSS_BRG_SECUR_BREACH_MSG_STC(
    lua_State *L,
    CPSS_BRG_SECUR_BREACH_MSG_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_NUMBER(val, -1, vlan, GT_U16);
    F_NUMBER(val, -1, port, GT_PORT_NUM);
    F_ENUM(val, -1, code, CPSS_BRG_SECUR_BREACH_EVENTS_ENT);
}

void prv_c_to_lua_CPSS_BRG_SECUR_BREACH_MSG_STC(
    lua_State *L,
    CPSS_BRG_SECUR_BREACH_MSG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_NUMBER(val, t, vlan, GT_U16);
    FO_NUMBER(val, t, port, GT_PORT_NUM);
    FO_ENUM(val, t, code, CPSS_BRG_SECUR_BREACH_EVENTS_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_BRG_SECUR_BREACH_MSG_STC);

enumDescr enumDescr_CPSS_VLAN_TAG_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_VLAN_TAG0_E),
    ENUM_ENTRY(CPSS_VLAN_TAG1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VLAN_TAG_TYPE_ENT);

void prv_lua_to_c_CPSS_PP_PORT_GROUP_INIT_INFO_STC(
    lua_State *L,
    CPSS_PP_PORT_GROUP_INIT_INFO_STC *val
)
{
    F_NUMBER(val, -1, busBaseAddr, GT_UINTPTR);
    F_NUMBER(val, -1, internalPciBase, GT_UINTPTR);
    F_STRUCT(val, -1, hwAddr, CPSS_HW_ADDR_STC);
    F_NUMBER(val, -1, intVecNum, GT_U32);
    F_NUMBER(val, -1, intMask, GT_UINTPTR);
}

void prv_c_to_lua_CPSS_PP_PORT_GROUP_INIT_INFO_STC(
    lua_State *L,
    CPSS_PP_PORT_GROUP_INIT_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, busBaseAddr, GT_UINTPTR);
    FO_NUMBER(val, t, internalPciBase, GT_UINTPTR);
    FO_STRUCT(val, t, hwAddr, CPSS_HW_ADDR_STC);
    FO_NUMBER(val, t, intVecNum, GT_U32);
    FO_NUMBER(val, t, intMask, GT_UINTPTR);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PP_PORT_GROUP_INIT_INFO_STC);

enumDescr enumDescr_CPSS_SRAM_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_SRAM_SIZE_NOT_USED_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_12KB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_32KB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_64KB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_128KB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_256KB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_512KB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_1MB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_2MB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_4MB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_8MB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_16MB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_32MB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_64MB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_128MB_E),
    ENUM_ENTRY(CPSS_SRAM_SIZE_256MB_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_SRAM_SIZE_ENT);

enumDescr enumDescr_CPSS_TM_ELIG_FUNC_QUEUE_ENT[] = {
    ENUM_ENTRY(CPSS_TM_ELIG_Q_PRIO0_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_PRIO1_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_PRIO2_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_PRIO3_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MIN_SHP_PRIO0_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MIN_SHP_PRIO1_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MIN_SHP_PRIO2_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MIN_SHP_PRIO3_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_PRIO4_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_PRIO5_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_PRIO6_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_PRIO7_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MIN_SHP_PRIO4_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MIN_SHP_PRIO5_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MIN_SHP_PRIO6_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MIN_SHP_PRIO7_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_MAX_INC_MIN_SHP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_SCHED00_PROP00),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_SCHED10_PROP10),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_SCHED20_PROP20),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_SCHED30_PROP30),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_SCHED40_PROP40),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_SCHED50_PROP50),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_SCHED60_PROP60),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_SHP_SCHED70_PROP70),
    ENUM_ENTRY(CPSS_TM_ELIG_Q_DEQ_DIS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM_ELIG_FUNC_QUEUE_ENT);

enumDescr enumDescr_CPSS_VCT_CABLE_LEN_ENT[] = {
    ENUM_ENTRY(CPSS_VCT_CABLE_LESS_10M_E),
    ENUM_ENTRY(CPSS_VCT_CABLE_GREATER_10M_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_CABLE_LEN_ENT);

void prv_lua_to_c_CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC(
    lua_State *L,
    CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC *val
)
{
    F_NUMBER(val, -1, stcValue, GT_U32);
    F_NUMBER(val, -1, rawValue, GT_U32);
}

void prv_c_to_lua_CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC(
    lua_State *L,
    CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, stcValue, GT_U32);
    FO_NUMBER(val, t, rawValue, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC);

enumDescr enumDescr_CPSS_NET_CPU_PORT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_NET_CPU_PORT_MODE_SDMA_E),
    ENUM_ENTRY(CPSS_NET_CPU_PORT_MODE_MII_E),
    ENUM_ENTRY(CPSS_NET_CPU_PORT_MODE_NONE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NET_CPU_PORT_MODE_ENT);

enumDescr enumDescr_CPSS_LED_BLINK_SELECT_ENT[] = {
    ENUM_ENTRY(CPSS_LED_BLINK_SELECT_0_E),
    ENUM_ENTRY(CPSS_LED_BLINK_SELECT_1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LED_BLINK_SELECT_ENT);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_ON_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_ON_TM_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_TM_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SEARCHING_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_INVALID_SIGNATURE_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_CLASS_ERROR_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_TEST_MODE_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_VALID_SIGNATURE_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_DISABLED_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_OVERLOAD_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_UNDERLOAD_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_SHORT_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_DVDT_FAIL_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_TEST_ERROR_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_OVERLOAD_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_UNDERLOAD_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SHORT_CIRCUIT_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_POWER_MANAGE_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SYSTEM_DISABLED_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_UNKNOWN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT);

void prv_lua_to_c_CPSS_MAC_UPDATE_MSG_STC(
    lua_State *L,
    CPSS_MAC_UPDATE_MSG_STC *val
)
{
    F_ENUM(val, -1, updType, CPSS_UPD_MSG_TYPE_ENT);
    F_BOOL(val, -1, entryWasFound);
    F_NUMBER(val, -1, macEntryIndex, GT_U32);
    F_STRUCT(val, -1, macEntry, CPSS_MAC_ENTRY_STC);
    F_BOOL(val, -1, skip);
    F_BOOL(val, -1, aging);
    F_NUMBER(val, -1, associatedDevNum, GT_U8);
    F_NUMBER(val, -1, queryDevNum, GT_U8);
    F_BOOL(val, -1, msgCpu);
}

void prv_c_to_lua_CPSS_MAC_UPDATE_MSG_STC(
    lua_State *L,
    CPSS_MAC_UPDATE_MSG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, updType, CPSS_UPD_MSG_TYPE_ENT);
    FO_BOOL(val, t, entryWasFound);
    FO_NUMBER(val, t, macEntryIndex, GT_U32);
    FO_STRUCT(val, t, macEntry, CPSS_MAC_ENTRY_STC);
    FO_BOOL(val, t, skip);
    FO_BOOL(val, t, aging);
    FO_NUMBER(val, t, associatedDevNum, GT_U8);
    FO_NUMBER(val, t, queryDevNum, GT_U8);
    FO_BOOL(val, t, msgCpu);
    lua_settop(L, t);
}
add_mgm_type(CPSS_MAC_UPDATE_MSG_STC);

void prv_lua_to_c_CPSS_TM2TM_DELTA_RANGE_STC(
    lua_State *L,
    CPSS_TM2TM_DELTA_RANGE_STC *val
)
{
    F_NUMBER(val, -1, upperRange0, GT_U32);
    F_NUMBER(val, -1, upperRange1, GT_U32);
    F_NUMBER(val, -1, upperRange2, GT_U32);
}

void prv_c_to_lua_CPSS_TM2TM_DELTA_RANGE_STC(
    lua_State *L,
    CPSS_TM2TM_DELTA_RANGE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, upperRange0, GT_U32);
    FO_NUMBER(val, t, upperRange1, GT_U32);
    FO_NUMBER(val, t, upperRange2, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM2TM_DELTA_RANGE_STC);

enumDescr enumDescr_CPSS_DRAM_PAGE_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DRAM_PAGE_SIZE_512B_E),
    ENUM_ENTRY(CPSS_DRAM_PAGE_SIZE_1K_E),
    ENUM_ENTRY(CPSS_DRAM_PAGE_SIZE_2K_E),
    ENUM_ENTRY(CPSS_DRAM_PAGE_SIZE_4K_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DRAM_PAGE_SIZE_ENT);

enumDescr enumDescr_CPSS_PHY_XSMI_INTERFACE_ENT[] = {
    ENUM_ENTRY(CPSS_PHY_XSMI_INTERFACE_0_E),
    ENUM_ENTRY(CPSS_PHY_XSMI_INTERFACE_1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PHY_XSMI_INTERFACE_ENT);

enumDescr enumDescr_CPSS_NARROW_SRAM_CFG_ENT[] = {
    ENUM_ENTRY(CPSS_TWIST_INTERNAL_E),
    ENUM_ENTRY(CPSS_TWIST_EXTERNAL_E),
    ENUM_ENTRY(CPSS_TWO_EXTERNAL_250MHZ_E),
    ENUM_ENTRY(CPSS_TWO_EXTERNAL_300MHZ_E),
    ENUM_ENTRY(CPSS_DROP_IN_CAPACITY_E),
    ENUM_ENTRY(CPSS_DROP_IN_SPEED_E),
    ENUM_ENTRY(CPSS_TWO_INTERNAL_E),
    ENUM_ENTRY(CPSS_ONE_INTERNAL_E),
    ENUM_ENTRY(CPSS_NRAM_NOT_EXISTS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NARROW_SRAM_CFG_ENT);

enumDescr enumDescr_CPSS_VCT_DOWNSHIFT_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_VCT_NO_DOWNSHIFT_E),
    ENUM_ENTRY(CPSS_VCT_DOWNSHIFT_E),
    ENUM_ENTRY(CPSS_VCT_NOT_APPLICABLE_DOWNSHIFT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_DOWNSHIFT_STATUS_ENT);

enumDescr enumDescr_CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E),
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT);

enumDescr enumDescr_CPSS_BRG_IPM_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_BRG_IPM_SGV_E),
    ENUM_ENTRY(CPSS_BRG_IPM_GV_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BRG_IPM_MODE_ENT);

enumDescr enumDescr_CPSS_CSCD_PORT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_CSCD_PORT_DSA_MODE_1_WORD_E),
    ENUM_ENTRY(CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E),
    ENUM_ENTRY(CPSS_CSCD_PORT_NETWORK_E),
    ENUM_ENTRY(CPSS_CSCD_PORT_DSA_MODE_3_WORDS_E),
    ENUM_ENTRY(CPSS_CSCD_PORT_DSA_MODE_4_WORDS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_CSCD_PORT_TYPE_ENT);

void prv_lua_to_c_CPSS_TM_QUEUE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_QUEUE_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, shapingProfilePtr, GT_U32);
    F_NUMBER(val, -1, quantum, GT_U32);
    F_NUMBER(val, -1, dropProfileInd, GT_U32);
    F_ENUM(val, -1, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_QUEUE_ENT);
}

void prv_c_to_lua_CPSS_TM_QUEUE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_QUEUE_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, shapingProfilePtr, GT_U32);
    FO_NUMBER(val, t, quantum, GT_U32);
    FO_NUMBER(val, t, dropProfileInd, GT_U32);
    FO_ENUM(val, t, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_QUEUE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_QUEUE_PARAMS_STC);

enumDescr enumDescr_CPSS_PORT_TX_WRR_MTU_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_256_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_2K_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_8K_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_64_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_128_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_512_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_1K_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_4K_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_16K_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_32K_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_MTU_64K_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_TX_WRR_MTU_ENT);

enumDescr enumDescr_CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_CPSS_E),
    ENUM_ENTRY(CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_APPLICATION_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT);

enumDescr enumDescr_CPSS_PORT_DIRECTION_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_DIRECTION_RX_E),
    ENUM_ENTRY(CPSS_PORT_DIRECTION_TX_E),
    ENUM_ENTRY(CPSS_PORT_DIRECTION_BOTH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_DIRECTION_ENT);

enumDescr enumDescr_CPSS_SYSTEM_RECOVERY_STATE_ENT[] = {
    ENUM_ENTRY(CPSS_SYSTEM_RECOVERY_PREPARATION_STATE_E),
    ENUM_ENTRY(CPSS_SYSTEM_RECOVERY_INIT_STATE_E),
    ENUM_ENTRY(CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E),
    ENUM_ENTRY(CPSS_SYSTEM_RECOVERY_HW_CATCH_UP_STATE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_SYSTEM_RECOVERY_STATE_ENT);

enumDescr enumDescr_CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E),
    ENUM_ENTRY(CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT);

enumDescr enumDescr_CPSS_FDB_ACTION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E),
    ENUM_ENTRY(CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E),
    ENUM_ENTRY(CPSS_FDB_ACTION_DELETING_E),
    ENUM_ENTRY(CPSS_FDB_ACTION_TRANSPLANTING_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_FDB_ACTION_MODE_ENT);

enumDescr enumDescr_CPSS_VCT_NORMAL_CABLE_LEN_ENT[] = {
    ENUM_ENTRY(CPSS_VCT_LESS_THAN_50M_E),
    ENUM_ENTRY(CPSS_VCT_50M_80M_E),
    ENUM_ENTRY(CPSS_VCT_80M_110M_E),
    ENUM_ENTRY(CPSS_VCT_110M_140M_E),
    ENUM_ENTRY(CPSS_VCT_MORE_THAN_140_E),
    ENUM_ENTRY(CPSS_VCT_UNKNOWN_LEN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_NORMAL_CABLE_LEN_ENT);

void prv_lua_to_c_CPSS_PORT_ATTRIBUTES_STC(
    lua_State *L,
    CPSS_PORT_ATTRIBUTES_STC *val
)
{
    F_BOOL(val, -1, portLinkUp);
    F_ENUM(val, -1, portSpeed, CPSS_PORT_SPEED_ENT);
    F_ENUM(val, -1, portDuplexity, CPSS_PORT_DUPLEX_ENT);
}

void prv_c_to_lua_CPSS_PORT_ATTRIBUTES_STC(
    lua_State *L,
    CPSS_PORT_ATTRIBUTES_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, portLinkUp);
    FO_ENUM(val, t, portSpeed, CPSS_PORT_SPEED_ENT);
    FO_ENUM(val, t, portDuplexity, CPSS_PORT_DUPLEX_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_ATTRIBUTES_STC);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_PRIORITY_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PRIORITY_UNKNOWN_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PRIORITY_CRITICAL_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PRIORITY_HIGH_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PRIORITY_LOW_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_PRIORITY_ENT);

void prv_lua_to_c_CPSS_PORTS_BMP_STC(
    lua_State *L,
    CPSS_PORTS_BMP_STC *val
)
{
    F_ARRAY_START(val, -1, ports);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            F_ARRAY_NUMBER(val, ports, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, ports);
}

void prv_c_to_lua_CPSS_PORTS_BMP_STC(
    lua_State *L,
    CPSS_PORTS_BMP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, ports);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            FO_ARRAY_NUMBER(val, ports, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, ports);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORTS_BMP_STC);

enumDescr enumDescr_CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_SINGLE_E),
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_MULTIPLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT);

void prv_lua_to_c_CPSS_TM_BAP_UNIT_ERROR_STATUS_STC(
    lua_State *L,
    CPSS_TM_BAP_UNIT_ERROR_STATUS_STC *val
)
{
    F_NUMBER(val, -1, bapStatus, GT_U32);
    F_NUMBER(val, -1, bapNum, GT_U8);
}

void prv_c_to_lua_CPSS_TM_BAP_UNIT_ERROR_STATUS_STC(
    lua_State *L,
    CPSS_TM_BAP_UNIT_ERROR_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, bapStatus, GT_U32);
    FO_NUMBER(val, t, bapNum, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_BAP_UNIT_ERROR_STATUS_STC);

void prv_lua_to_c_CPSS_CSCD_LINK_TYPE_STC(
    lua_State *L,
    CPSS_CSCD_LINK_TYPE_STC *val
)
{
    F_NUMBER(val, -1, linkNum, GT_U32);
    F_ENUM(val, -1, linkType, CPSS_CSCD_LINK_TYPE_ENT);
}

void prv_c_to_lua_CPSS_CSCD_LINK_TYPE_STC(
    lua_State *L,
    CPSS_CSCD_LINK_TYPE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, linkNum, GT_U32);
    FO_ENUM(val, t, linkType, CPSS_CSCD_LINK_TYPE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_CSCD_LINK_TYPE_STC);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC *val
)
{
    F_NUMBER(val, -1, pdRequestedPower, GT_U32);
    F_ENUM(val, -1, pdPriority, CPSS_GEN_DRAGONITE_PRIORITY_ENT);
    F_NUMBER(val, -1, portCableLen, GT_U32);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pdRequestedPower, GT_U32);
    FO_ENUM(val, t, pdPriority, CPSS_GEN_DRAGONITE_PRIORITY_ENT);
    FO_NUMBER(val, t, portCableLen, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC);

enumDescr enumDescr_CPSS_PORT_TX_Q_ARB_GROUP_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_TX_WRR_ARB_GROUP_0_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_ARB_GROUP_1_E),
    ENUM_ENTRY(CPSS_PORT_TX_SP_ARB_GROUP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_TX_Q_ARB_GROUP_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_POWER_BUDGET_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_POWER_BUDGET_STC *val
)
{
    F_NUMBER(val, -1, availablePower, GT_U32);
    F_ENUM(val, -1, powerSourceType, CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_POWER_BUDGET_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_POWER_BUDGET_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, availablePower, GT_U32);
    FO_ENUM(val, t, powerSourceType, CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_POWER_BUDGET_STC);

enumDescr enumDescr_CPSS_UPD_MSG_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_NA_E),
    ENUM_ENTRY(CPSS_QA_E),
    ENUM_ENTRY(CPSS_QR_E),
    ENUM_ENTRY(CPSS_AA_E),
    ENUM_ENTRY(CPSS_TA_E),
    ENUM_ENTRY(CPSS_SA_E),
    ENUM_ENTRY(CPSS_QI_E),
    ENUM_ENTRY(CPSS_FU_E),
    ENUM_ENTRY(CPSS_HR_E),
    ENUM_ENTRY(CPSS_NA_MOVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_UPD_MSG_TYPE_ENT);

enumDescr enumDescr_CPSS_SRAM_FREQUENCY_ENT[] = {
    ENUM_ENTRY(CPSS_SRAM_FREQ_125_MHZ_E),
    ENUM_ENTRY(CPSS_SRAM_FREQ_166_MHZ_E),
    ENUM_ENTRY(CPSS_SRAM_FREQ_200_MHZ_E),
    ENUM_ENTRY(CPSS_SRAM_FREQ_250_MHZ_E),
    ENUM_ENTRY(CPSS_SRAM_FREQ_300_MHZ_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_SRAM_FREQUENCY_ENT);

enumDescr enumDescr_CPSS_PP_INTERFACE_CHANNEL_ENT[] = {
    ENUM_ENTRY(CPSS_CHANNEL_PCI_E),
    ENUM_ENTRY(CPSS_CHANNEL_SMI_E),
    ENUM_ENTRY(CPSS_CHANNEL_TWSI_E),
    ENUM_ENTRY(CPSS_CHANNEL_PEX_E),
    ENUM_ENTRY(CPSS_CHANNEL_PEX_MBUS_E),
    ENUM_ENTRY(CPSS_CHANNEL_PEX_KERNEL_E),
    ENUM_ENTRY(CPSS_CHANNEL_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PP_INTERFACE_CHANNEL_ENT);

enumDescr enumDescr_CPSS_IGMP_SNOOP_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_IGMP_ALL_TRAP_MODE_E),
    ENUM_ENTRY(CPSS_IGMP_SNOOP_TRAP_MODE_E),
    ENUM_ENTRY(CPSS_IGMP_ROUTER_MIRROR_MODE_E),
    ENUM_ENTRY(CPSS_IGMP_FWD_DECISION_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_IGMP_SNOOP_MODE_ENT);

void prv_lua_to_c_CPSS_TM_LIB_INIT_PARAMS_STC(
    lua_State *L,
    CPSS_TM_LIB_INIT_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, tmMtu, GT_U32);
}

void prv_c_to_lua_CPSS_TM_LIB_INIT_PARAMS_STC(
    lua_State *L,
    CPSS_TM_LIB_INIT_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, tmMtu, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_LIB_INIT_PARAMS_STC);

enumDescr enumDescr_CPSS_NST_AM_PARAM_ENT[] = {
    ENUM_ENTRY(CPSS_NST_AM_SA_AUTO_LEARNED_E),
    ENUM_ENTRY(CPSS_NST_AM_DA_AUTO_LEARNED_E),
    ENUM_ENTRY(CPSS_NST_AM_SA_UNKNOWN_E),
    ENUM_ENTRY(CPSS_NST_AM_DA_UNKNOWN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NST_AM_PARAM_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_LAYER2_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_LAYER2_STC *val
)
{
    F_STRUCT(val, -1, layer2PdInfo, CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC);
    F_STRUCT(val, -1, layer2PseInfo, CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_LAYER2_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_LAYER2_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, layer2PdInfo, CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC);
    FO_STRUCT(val, t, layer2PseInfo, CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_LAYER2_STC);

void prv_lua_to_c_CPSS_BRIDGE_INGRESS_CNTR_STC(
    lua_State *L,
    CPSS_BRIDGE_INGRESS_CNTR_STC *val
)
{
    F_NUMBER(val, -1, gtBrgInFrames, GT_U32);
    F_NUMBER(val, -1, gtBrgVlanIngFilterDisc, GT_U32);
    F_NUMBER(val, -1, gtBrgSecFilterDisc, GT_U32);
    F_NUMBER(val, -1, gtBrgLocalPropDisc, GT_U32);
}

void prv_c_to_lua_CPSS_BRIDGE_INGRESS_CNTR_STC(
    lua_State *L,
    CPSS_BRIDGE_INGRESS_CNTR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, gtBrgInFrames, GT_U32);
    FO_NUMBER(val, t, gtBrgVlanIngFilterDisc, GT_U32);
    FO_NUMBER(val, t, gtBrgSecFilterDisc, GT_U32);
    FO_NUMBER(val, t, gtBrgLocalPropDisc, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_BRIDGE_INGRESS_CNTR_STC);

void prv_lua_to_c_CPSS_TM2TM_EXTERNAL_HDR_STC(
    lua_State *L,
    CPSS_TM2TM_EXTERNAL_HDR_STC *val
)
{
    F_NUMBER(val, -1, size, GT_U32);
    F_ARRAY_START(val, -1, contentArr);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_STRUCT(val, contentArr, idx, GT_U64);
        }
    }
    F_ARRAY_END(val, -1, contentArr);
}

void prv_c_to_lua_CPSS_TM2TM_EXTERNAL_HDR_STC(
    lua_State *L,
    CPSS_TM2TM_EXTERNAL_HDR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, size, GT_U32);
    FO_ARRAY_START(val, t, contentArr);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_STRUCT(val, contentArr, idx, GT_U64);
        }
    }
    FO_ARRAY_END(val, t, contentArr);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM2TM_EXTERNAL_HDR_STC);

void prv_lua_to_c_CPSS_LED_CONF_STC(
    lua_State *L,
    CPSS_LED_CONF_STC *val
)
{
    F_ENUM(val, -1, ledOrganize, CPSS_LED_ORDER_MODE_ENT);
    F_BOOL(val, -1, disableOnLinkDown);
    F_ENUM(val, -1, blink0DutyCycle, CPSS_LED_BLINK_DUTY_CYCLE_ENT);
    F_ENUM(val, -1, blink0Duration, CPSS_LED_BLINK_DURATION_ENT);
    F_ENUM(val, -1, blink1DutyCycle, CPSS_LED_BLINK_DUTY_CYCLE_ENT);
    F_ENUM(val, -1, blink1Duration, CPSS_LED_BLINK_DURATION_ENT);
    F_ENUM(val, -1, pulseStretch, CPSS_LED_PULSE_STRETCH_ENT);
    F_NUMBER(val, -1, ledStart, GT_U32);
    F_NUMBER(val, -1, ledEnd, GT_U32);
    F_BOOL(val, -1, clkInvert);
    F_ENUM(val, -1, class5select, CPSS_LED_CLASS_5_SELECT_ENT);
    F_ENUM(val, -1, class13select, CPSS_LED_CLASS_13_SELECT_ENT);
    F_BOOL(val, -1, invertEnable);
    F_ENUM(val, -1, ledClockFrequency, CPSS_LED_CLOCK_OUT_FREQUENCY_ENT);
}

void prv_c_to_lua_CPSS_LED_CONF_STC(
    lua_State *L,
    CPSS_LED_CONF_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, ledOrganize, CPSS_LED_ORDER_MODE_ENT);
    FO_BOOL(val, t, disableOnLinkDown);
    FO_ENUM(val, t, blink0DutyCycle, CPSS_LED_BLINK_DUTY_CYCLE_ENT);
    FO_ENUM(val, t, blink0Duration, CPSS_LED_BLINK_DURATION_ENT);
    FO_ENUM(val, t, blink1DutyCycle, CPSS_LED_BLINK_DUTY_CYCLE_ENT);
    FO_ENUM(val, t, blink1Duration, CPSS_LED_BLINK_DURATION_ENT);
    FO_ENUM(val, t, pulseStretch, CPSS_LED_PULSE_STRETCH_ENT);
    FO_NUMBER(val, t, ledStart, GT_U32);
    FO_NUMBER(val, t, ledEnd, GT_U32);
    FO_BOOL(val, t, clkInvert);
    FO_ENUM(val, t, class5select, CPSS_LED_CLASS_5_SELECT_ENT);
    FO_ENUM(val, t, class13select, CPSS_LED_CLASS_13_SELECT_ENT);
    FO_BOOL(val, t, invertEnable);
    FO_ENUM(val, t, ledClockFrequency, CPSS_LED_CLOCK_OUT_FREQUENCY_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_LED_CONF_STC);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_RESERVED_0_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ALTER_A_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ALTER_B_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_RESERVED_3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT);

void prv_lua_to_c_CPSS_TM_PORT_DROP_PER_COS_STC(
    lua_State *L,
    CPSS_TM_PORT_DROP_PER_COS_STC *val
)
{
    F_NUMBER(val, -1, dropCosMap, GT_U32);
    F_ARRAY_START(val, -1, dropProfileIndArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, dropProfileIndArr, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, dropProfileIndArr);
}

void prv_c_to_lua_CPSS_TM_PORT_DROP_PER_COS_STC(
    lua_State *L,
    CPSS_TM_PORT_DROP_PER_COS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, dropCosMap, GT_U32);
    FO_ARRAY_START(val, t, dropProfileIndArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, dropProfileIndArr, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, dropProfileIndArr);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_PORT_DROP_PER_COS_STC);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_DATA_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_CNTRS_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_STATUS_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_MEASURE_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_DEBUG_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_OTHER_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_DATA_TYPE_MAX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_DATA_TYPE_ENT);

enumDescr enumDescr_CPSS_EXTERNAL_MEMORY_ENT[] = {
    ENUM_ENTRY(CPSS_EXTERNAL_MEMORY_0_E),
    ENUM_ENTRY(CPSS_EXTERNAL_MEMORY_1_E),
    ENUM_ENTRY(CPSS_EXTERNAL_MEMORY_2_E),
    ENUM_ENTRY(CPSS_EXTERNAL_MEMORY_3_E),
    ENUM_ENTRY(CPSS_EXTERNAL_MEMORY_INVALID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_EXTERNAL_MEMORY_ENT);

enumDescr enumDescr_CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_NATIVE_WEIGHTED_E),
    ENUM_ENTRY(CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORBID_ALL_E),
    ENUM_ENTRY(CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORCE_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT);

enumDescr enumDescr_CPSS_DRV_HW_TRACE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DRV_HW_TRACE_TYPE_READ_E),
    ENUM_ENTRY(CPSS_DRV_HW_TRACE_TYPE_WRITE_E),
    ENUM_ENTRY(CPSS_DRV_HW_TRACE_TYPE_BOTH_E),
    ENUM_ENTRY(CPSS_DRV_HW_TRACE_TYPE_WRITE_DELAY_E),
    ENUM_ENTRY(CPSS_DRV_HW_TRACE_TYPE_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DRV_HW_TRACE_TYPE_ENT);

enumDescr enumDescr_CPSS_PP_FAMILY_BIT_ENT[] = {
    ENUM_ENTRY(CPSS_UNDEFINED_DEVICE_E),
    ENUM_ENTRY(CPSS_CH1_E),
    ENUM_ENTRY(CPSS_CH1_DIAMOND_E),
    ENUM_ENTRY(CPSS_CH2_E),
    ENUM_ENTRY(CPSS_CH3_E),
    ENUM_ENTRY(CPSS_XCAT_E),
    ENUM_ENTRY(CPSS_LION_E),
    ENUM_ENTRY(CPSS_XCAT2_E),
    ENUM_ENTRY(CPSS_LION2_E),
    ENUM_ENTRY(CPSS_PUMA_E),
    ENUM_ENTRY(CPSS_PUMA3_E),
    ENUM_ENTRY(CPSS_BOBCAT2_E),
    ENUM_ENTRY(CPSS_XCAT3_E),
    ENUM_ENTRY(CPSS_BOBCAT3_E),
    ENUM_ENTRY(CPSS_CAELUM_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_BIT_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PP_FAMILY_BIT_ENT);

enumDescr enumDescr_CPSS_DRAM_SPEED_BIN_ENT[] = {
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_800D_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_800E_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1066E_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1066F_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1066G_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1333F_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1333G_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1333H_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1333J_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1600G_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1600H_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1600J_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1600K_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1866J_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1866K_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1866L_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_1866M_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_2133K_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_2133L_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_2133M_E),
    ENUM_ENTRY(CPSS_DRAM_SPEED_BIN_DDR3_2133N_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DRAM_SPEED_BIN_ENT);

enumDescr enumDescr_CPSS_BRG_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_BRG_MODE_802_1Q_E),
    ENUM_ENTRY(CPSS_BRG_MODE_802_1D_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BRG_MODE_ENT);

void prv_lua_to_c_CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS(
    lua_State *L,
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *val
)
{
    F_NUMBER(val, -1, dp0MaxBuffNum, GT_U32);
    F_NUMBER(val, -1, dp1MaxBuffNum, GT_U32);
    F_NUMBER(val, -1, dp2MaxBuffNum, GT_U32);
    F_NUMBER(val, -1, dp0MaxDescrNum, GT_U32);
    F_NUMBER(val, -1, dp1MaxDescrNum, GT_U32);
    F_NUMBER(val, -1, dp2MaxDescrNum, GT_U32);
    F_NUMBER(val, -1, tcMaxBuffNum, GT_U32);
    F_NUMBER(val, -1, tcMaxDescrNum, GT_U32);
    F_NUMBER(val, -1, dp0MaxMCBuffNum, GT_U32);
    F_NUMBER(val, -1, dp1MaxMCBuffNum, GT_U32);
    F_NUMBER(val, -1, dp2MaxMCBuffNum, GT_U32);
    F_BOOL(val, -1, sharedUcAndMcCountersDisable);
}

void prv_c_to_lua_CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS(
    lua_State *L,
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, dp0MaxBuffNum, GT_U32);
    FO_NUMBER(val, t, dp1MaxBuffNum, GT_U32);
    FO_NUMBER(val, t, dp2MaxBuffNum, GT_U32);
    FO_NUMBER(val, t, dp0MaxDescrNum, GT_U32);
    FO_NUMBER(val, t, dp1MaxDescrNum, GT_U32);
    FO_NUMBER(val, t, dp2MaxDescrNum, GT_U32);
    FO_NUMBER(val, t, tcMaxBuffNum, GT_U32);
    FO_NUMBER(val, t, tcMaxDescrNum, GT_U32);
    FO_NUMBER(val, t, dp0MaxMCBuffNum, GT_U32);
    FO_NUMBER(val, t, dp1MaxMCBuffNum, GT_U32);
    FO_NUMBER(val, t, dp2MaxMCBuffNum, GT_U32);
    FO_BOOL(val, t, sharedUcAndMcCountersDisable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS);

enumDescr enumDescr_CPSS_TM_COLOR_NUM_ENT[] = {
    ENUM_ENTRY(CPSS_TM_COLOR_NUM_1_E),
    ENUM_ENTRY(CPSS_TM_COLOR_NUM_2_E),
    ENUM_ENTRY(CPSS_TM_COLOR_NUM_3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM_COLOR_NUM_ENT);

enumDescr enumDescr_CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT[] = {
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CHECK_BITS_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CHECK_BY_MAXVALUE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT);

enumDescr enumDescr_CPSS_POLICER_COLOR_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_POLICER_COLOR_BLIND_E),
    ENUM_ENTRY(CPSS_POLICER_COLOR_AWARE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_POLICER_COLOR_MODE_ENT);

enumDescr enumDescr_CPSS_PORT_ACCEPT_FRAME_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_ACCEPT_FRAME_TAGGED_E),
    ENUM_ENTRY(CPSS_PORT_ACCEPT_FRAME_ALL_E),
    ENUM_ENTRY(CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E),
    ENUM_ENTRY(CPSS_PORT_ACCEPT_FRAME_NONE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_ACCEPT_FRAME_TYPE_ENT);

void prv_lua_to_c_CPSS_VCT_CABLE_EXTENDED_STATUS_STC(
    lua_State *L,
    CPSS_VCT_CABLE_EXTENDED_STATUS_STC *val
)
{
    F_STRUCT(val, -1, vctExtendedCableStatus, CPSS_VCT_EXTENDED_STATUS_STC);
    F_STRUCT(val, -1, accurateCableLen, CPSS_VCT_ACCURATE_CABLE_LEN_STC);
    F_ENUM(val, -1, twoPairDownShift, CPSS_VCT_DOWNSHIFT_STATUS_ENT);
}

void prv_c_to_lua_CPSS_VCT_CABLE_EXTENDED_STATUS_STC(
    lua_State *L,
    CPSS_VCT_CABLE_EXTENDED_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, vctExtendedCableStatus, CPSS_VCT_EXTENDED_STATUS_STC);
    FO_STRUCT(val, t, accurateCableLen, CPSS_VCT_ACCURATE_CABLE_LEN_STC);
    FO_ENUM(val, t, twoPairDownShift, CPSS_VCT_DOWNSHIFT_STATUS_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VCT_CABLE_EXTENDED_STATUS_STC);

enumDescr enumDescr_CPSS_MAC_ACTION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_ACT_AUTO_E),
    ENUM_ENTRY(CPSS_ACT_TRIG_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_MAC_ACTION_MODE_ENT);

enumDescr enumDescr_CPSS_RATE_LIMIT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_RATE_LIMIT_PCKT_BASED_E),
    ENUM_ENTRY(CPSS_RATE_LIMIT_BYTE_BASED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_RATE_LIMIT_MODE_ENT);

enumDescr enumDescr_CPSS_PORT_MAC_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_NOT_EXISTS_E),
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_FE_E),
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_GE_E),
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_XG_E),
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_XLG_E),
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_HGL_E),
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_CG_E),
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_ILKN_E),
    ENUM_ENTRY(CPSS_PORT_MAC_TYPE_NOT_APPLICABLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_MAC_TYPE_ENT);

void prv_lua_to_c_CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC(
    lua_State *L,
    CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, aqlExponent, GT_U8);
    F_ARRAY_START(val, -1, curveIndex);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, curveIndex, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, curveIndex);
    F_ARRAY_START(val, -1, dpCurveScale);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, dpCurveScale, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, dpCurveScale);
    F_ARRAY_START(val, -1, caWredTdMinThreshold);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, caWredTdMinThreshold, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, caWredTdMinThreshold);
    F_ARRAY_START(val, -1, caWredTdMaxThreshold);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, caWredTdMaxThreshold, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, caWredTdMaxThreshold);
}

void prv_c_to_lua_CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC(
    lua_State *L,
    CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, aqlExponent, GT_U8);
    FO_ARRAY_START(val, t, curveIndex);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, curveIndex, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, curveIndex);
    FO_ARRAY_START(val, t, dpCurveScale);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, dpCurveScale, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, dpCurveScale);
    FO_ARRAY_START(val, t, caWredTdMinThreshold);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, caWredTdMinThreshold, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, caWredTdMinThreshold);
    FO_ARRAY_START(val, t, caWredTdMaxThreshold);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, caWredTdMaxThreshold, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, caWredTdMaxThreshold);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC);

enumDescr enumDescr_CPSS_PORT_INTERFACE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_MII_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_SGMII_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_XGMII_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_MGMII_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_1000BASE_X_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_GMII_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_MII_PHY_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_QX_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_HX_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_RXAUI_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_100BASE_FX_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_QSGMII_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_XLG_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_KR_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_HGL_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_CHGL_12_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_ILKN12_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_SR_LR_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_ILKN16_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_ILKN24_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_ILKN4_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_ILKN8_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_XHGS_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_XHGS_SR_E),
    ENUM_ENTRY(CPSS_PORT_INTERFACE_MODE_NA_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_INTERFACE_MODE_ENT);

void prv_lua_to_c_CPSS_TM_TREE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_TREE_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, maxQueues, GT_U32);
    F_NUMBER(val, -1, maxAnodes, GT_U32);
    F_NUMBER(val, -1, maxBnodes, GT_U32);
    F_NUMBER(val, -1, maxCnodes, GT_U32);
    F_NUMBER(val, -1, maxPorts, GT_U32);
}

void prv_c_to_lua_CPSS_TM_TREE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_TREE_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, maxQueues, GT_U32);
    FO_NUMBER(val, t, maxAnodes, GT_U32);
    FO_NUMBER(val, t, maxBnodes, GT_U32);
    FO_NUMBER(val, t, maxCnodes, GT_U32);
    FO_NUMBER(val, t, maxPorts, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_TREE_PARAMS_STC);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_SYSTEM_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_SYSTEM_STC *val
)
{
    F_NUMBER(val, -1, systemMask0, GT_U32);
    F_NUMBER(val, -1, activeBudget, GT_U32);
    F_NUMBER(val, -1, vmain, GT_U32);
    F_ARRAY_START(val, -1, powerBudgetArr);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            F_ARRAY_STRUCT(val, powerBudgetArr, idx, CPSS_GEN_DRAGONITE_POWER_BUDGET_STC);
        }
    }
    F_ARRAY_END(val, -1, powerBudgetArr);
    F_BOOL(val, -1, vmainHigh);
    F_BOOL(val, -1, vmainLowAT);
    F_BOOL(val, -1, vmainLowAF);
    F_BOOL(val, -1, tempAlarm);
    F_BOOL(val, -1, overTemp);
    F_BOOL(val, -1, disablePortsActive);
    F_NUMBER(val, -1, osStatus, GT_U32);
    F_ARRAY_START(val, -1, chipStatusArr);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            F_ARRAY_ENUM(val, chipStatusArr, idx, CPSS_GEN_DRAGONITE_CHIP_STATE_ENT);
        }
    }
    F_ARRAY_END(val, -1, chipStatusArr);
    F_NUMBER(val, -1, sysTotalCriticalCons, GT_U32);
    F_NUMBER(val, -1, sysTotalHighCons, GT_U32);
    F_NUMBER(val, -1, sysTotalLowCons, GT_U32);
    F_NUMBER(val, -1, sysTotalCriticalReq, GT_U32);
    F_NUMBER(val, -1, sysTotalHighReq, GT_U32);
    F_NUMBER(val, -1, sysTotalLowReq, GT_U32);
    F_NUMBER(val, -1, sysTotalCalcPowerCons, GT_U32);
    F_NUMBER(val, -1, sysTotalPowerRequest, GT_U32);
    F_NUMBER(val, -1, sysTotalDeltaPower, GT_U32);
    F_NUMBER(val, -1, sysTotalRealPowerCons, GT_U32);
    F_NUMBER(val, -1, minorVersion, GT_U32);
    F_NUMBER(val, -1, majorVersion, GT_U32);
    F_ARRAY_START(val, -1, bldDateTime);
    {
        int idx;
        for (idx = 0; idx < 20; idx++) {
            F_ARRAY_NUMBER(val, bldDateTime, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, bldDateTime);
    F_NUMBER(val, -1, checkSumErrorCounter, GT_U32);
    F_NUMBER(val, -1, lengthErrorCounter, GT_U32);
    F_NUMBER(val, -1, structVersionErrorCounter, GT_U32);
    F_NUMBER(val, -1, typeErrorCounter, GT_U32);
    F_NUMBER(val, -1, configErrorCounter, GT_U32);
    F_NUMBER(val, -1, irqErrorCounter, GT_U32);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_SYSTEM_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_SYSTEM_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, systemMask0, GT_U32);
    FO_NUMBER(val, t, activeBudget, GT_U32);
    FO_NUMBER(val, t, vmain, GT_U32);
    FO_ARRAY_START(val, t, powerBudgetArr);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            FO_ARRAY_STRUCT(val, powerBudgetArr, idx, CPSS_GEN_DRAGONITE_POWER_BUDGET_STC);
        }
    }
    FO_ARRAY_END(val, t, powerBudgetArr);
    FO_BOOL(val, t, vmainHigh);
    FO_BOOL(val, t, vmainLowAT);
    FO_BOOL(val, t, vmainLowAF);
    FO_BOOL(val, t, tempAlarm);
    FO_BOOL(val, t, overTemp);
    FO_BOOL(val, t, disablePortsActive);
    FO_NUMBER(val, t, osStatus, GT_U32);
    FO_ARRAY_START(val, t, chipStatusArr);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            FO_ARRAY_ENUM(val, chipStatusArr, idx, CPSS_GEN_DRAGONITE_CHIP_STATE_ENT);
        }
    }
    FO_ARRAY_END(val, t, chipStatusArr);
    FO_NUMBER(val, t, sysTotalCriticalCons, GT_U32);
    FO_NUMBER(val, t, sysTotalHighCons, GT_U32);
    FO_NUMBER(val, t, sysTotalLowCons, GT_U32);
    FO_NUMBER(val, t, sysTotalCriticalReq, GT_U32);
    FO_NUMBER(val, t, sysTotalHighReq, GT_U32);
    FO_NUMBER(val, t, sysTotalLowReq, GT_U32);
    FO_NUMBER(val, t, sysTotalCalcPowerCons, GT_U32);
    FO_NUMBER(val, t, sysTotalPowerRequest, GT_U32);
    FO_NUMBER(val, t, sysTotalDeltaPower, GT_U32);
    FO_NUMBER(val, t, sysTotalRealPowerCons, GT_U32);
    FO_NUMBER(val, t, minorVersion, GT_U32);
    FO_NUMBER(val, t, majorVersion, GT_U32);
    FO_ARRAY_START(val, t, bldDateTime);
    {
        int idx;
        for (idx = 0; idx < 20; idx++) {
            FO_ARRAY_NUMBER(val, bldDateTime, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, bldDateTime);
    FO_NUMBER(val, t, checkSumErrorCounter, GT_U32);
    FO_NUMBER(val, t, lengthErrorCounter, GT_U32);
    FO_NUMBER(val, t, structVersionErrorCounter, GT_U32);
    FO_NUMBER(val, t, typeErrorCounter, GT_U32);
    FO_NUMBER(val, t, configErrorCounter, GT_U32);
    FO_NUMBER(val, t, irqErrorCounter, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_SYSTEM_STC);

void prv_lua_to_c_CPSS_PORT_MAC_STATUS_STC(
    lua_State *L,
    CPSS_PORT_MAC_STATUS_STC *val
)
{
    F_BOOL(val, -1, isPortRxPause);
    F_BOOL(val, -1, isPortTxPause);
    F_BOOL(val, -1, isPortBackPres);
    F_BOOL(val, -1, isPortBufFull);
    F_BOOL(val, -1, isPortSyncFail);
    F_BOOL(val, -1, isPortHiErrorRate);
    F_BOOL(val, -1, isPortAnDone);
    F_BOOL(val, -1, isPortFatalError);
}

void prv_c_to_lua_CPSS_PORT_MAC_STATUS_STC(
    lua_State *L,
    CPSS_PORT_MAC_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, isPortRxPause);
    FO_BOOL(val, t, isPortTxPause);
    FO_BOOL(val, t, isPortBackPres);
    FO_BOOL(val, t, isPortBufFull);
    FO_BOOL(val, t, isPortSyncFail);
    FO_BOOL(val, t, isPortHiErrorRate);
    FO_BOOL(val, t, isPortAnDone);
    FO_BOOL(val, t, isPortFatalError);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_MAC_STATUS_STC);

enumDescr enumDescr_CPSS_PORT_SPEED_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_SPEED_10_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_100_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_1000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_10000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_12000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_2500_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_5000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_13600_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_20000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_40000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_16000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_15000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_75000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_100G_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_50000_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_140G_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_11800_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_47200_E),
    ENUM_ENTRY(CPSS_PORT_SPEED_NA_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_SPEED_ENT);

void prv_lua_to_c_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT(
    lua_State *L,
    CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT(
    lua_State *L,
    CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, memLocation, CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC);
    FO_STRUCT(val, t, mppmMemLocation, CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC);
    FO_STRUCT(val, t, tcamMemLocation, CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT);

enumDescr enumDescr_CPSS_IPV6_MLL_SELECTION_RULE_ENT[] = {
    ENUM_ENTRY(CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E),
    ENUM_ENTRY(CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_IPV6_MLL_SELECTION_RULE_ENT);

void prv_lua_to_c_CPSS_LED_CLASS_MANIPULATION_STC(
    lua_State *L,
    CPSS_LED_CLASS_MANIPULATION_STC *val
)
{
    F_BOOL(val, -1, invertEnable);
    F_BOOL(val, -1, blinkEnable);
    F_ENUM(val, -1, blinkSelect, CPSS_LED_BLINK_SELECT_ENT);
    F_BOOL(val, -1, forceEnable);
    F_NUMBER(val, -1, forceData, GT_U32);
    F_BOOL(val, -1, pulseStretchEnable);
    F_BOOL(val, -1, disableOnLinkDown);
}

void prv_c_to_lua_CPSS_LED_CLASS_MANIPULATION_STC(
    lua_State *L,
    CPSS_LED_CLASS_MANIPULATION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, invertEnable);
    FO_BOOL(val, t, blinkEnable);
    FO_ENUM(val, t, blinkSelect, CPSS_LED_BLINK_SELECT_ENT);
    FO_BOOL(val, t, forceEnable);
    FO_NUMBER(val, t, forceData, GT_U32);
    FO_BOOL(val, t, pulseStretchEnable);
    FO_BOOL(val, t, disableOnLinkDown);
    lua_settop(L, t);
}
add_mgm_type(CPSS_LED_CLASS_MANIPULATION_STC);

enumDescr enumDescr_CPSS_PHY_SMI_INTERFACE_ENT[] = {
    ENUM_ENTRY(CPSS_PHY_SMI_INTERFACE_0_E),
    ENUM_ENTRY(CPSS_PHY_SMI_INTERFACE_1_E),
    ENUM_ENTRY(CPSS_PHY_SMI_INTERFACE_2_E),
    ENUM_ENTRY(CPSS_PHY_SMI_INTERFACE_3_E),
    ENUM_ENTRY(CPSS_PHY_SMI_INTERFACE_MAX_E),
    ENUM_ENTRY(CPSS_PHY_SMI_INTERFACE_INVALID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PHY_SMI_INTERFACE_ENT);

enumDescr enumDescr_CPSS_PORT_SERDES_TUNING_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_SERDES_TUNING_NONE_E),
    ENUM_ENTRY(CPSS_PORT_SERDES_TUNING_RX_ONLY_E),
    ENUM_ENTRY(CPSS_PORT_SERDES_TUNING_TRX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_SERDES_TUNING_TYPE_ENT);

enumDescr enumDescr_CPSS_EVENT_GPP_ID_ENT[] = {
    ENUM_ENTRY(CPSS_EVENT_GPP_INT_1_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INT_2_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INT_3_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_UPLINK_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_1_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_2_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_3_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_4_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_5_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_6_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_7_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_8_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_9_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_10_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_11_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_12_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_13_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_14_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_15_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_16_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_INTERNAL_17_E),
    ENUM_ENTRY(CPSS_EVENT_GPP_MAX_NUM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_EVENT_GPP_ID_ENT);

void prv_lua_to_c_CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC(
    lua_State *L,
    CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC *val
)
{
    F_ARRAY_START(val, -1, caTdThreshold);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, caTdThreshold, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, caTdThreshold);
}

void prv_c_to_lua_CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC(
    lua_State *L,
    CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, caTdThreshold);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, caTdThreshold, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, caTdThreshold);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC);

void prv_lua_to_c_CPSS_SRAM_CFG_STC(
    lua_State *L,
    CPSS_SRAM_CFG_STC *val
)
{
    F_ENUM(val, -1, sramSize, CPSS_SRAM_SIZE_ENT);
    F_ENUM(val, -1, sramFrequency, CPSS_SRAM_FREQUENCY_ENT);
    F_ENUM(val, -1, sramRate, CPSS_SRAM_RATE_ENT);
    F_ENUM(val, -1, externalMemory, CPSS_EXTERNAL_MEMORY_ENT);
    F_ENUM(val, -1, sramType, CPSS_SRAM_TYPE_ENT);
}

void prv_c_to_lua_CPSS_SRAM_CFG_STC(
    lua_State *L,
    CPSS_SRAM_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, sramSize, CPSS_SRAM_SIZE_ENT);
    FO_ENUM(val, t, sramFrequency, CPSS_SRAM_FREQUENCY_ENT);
    FO_ENUM(val, t, sramRate, CPSS_SRAM_RATE_ENT);
    FO_ENUM(val, t, externalMemory, CPSS_EXTERNAL_MEMORY_ENT);
    FO_ENUM(val, t, sramType, CPSS_SRAM_TYPE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_SRAM_CFG_STC);

void prv_lua_to_c_CPSS_LED_GROUP_CONF_STC(
    lua_State *L,
    CPSS_LED_GROUP_CONF_STC *val
)
{
    F_NUMBER(val, -1, classA, GT_U32);
    F_NUMBER(val, -1, classB, GT_U32);
    F_NUMBER(val, -1, classC, GT_U32);
    F_NUMBER(val, -1, classD, GT_U32);
}

void prv_c_to_lua_CPSS_LED_GROUP_CONF_STC(
    lua_State *L,
    CPSS_LED_GROUP_CONF_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, classA, GT_U32);
    FO_NUMBER(val, t, classB, GT_U32);
    FO_NUMBER(val, t, classC, GT_U32);
    FO_NUMBER(val, t, classD, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_LED_GROUP_CONF_STC);

void prv_lua_to_c_CPSS_PORT_MODE_SPEED_STC(
    lua_State *L,
    CPSS_PORT_MODE_SPEED_STC *val
)
{
    F_ENUM(val, -1, ifMode, CPSS_PORT_INTERFACE_MODE_ENT);
    F_ENUM(val, -1, speed, CPSS_PORT_SPEED_ENT);
}

void prv_c_to_lua_CPSS_PORT_MODE_SPEED_STC(
    lua_State *L,
    CPSS_PORT_MODE_SPEED_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, ifMode, CPSS_PORT_INTERFACE_MODE_ENT);
    FO_ENUM(val, t, speed, CPSS_PORT_SPEED_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_MODE_SPEED_STC);

enumDescr enumDescr_CPSS_SRAM_RATE_ENT[] = {
    ENUM_ENTRY(CPSS_SRAM_RATE_DDR_E),
    ENUM_ENTRY(CPSS_SRAM_RATE_QDR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_SRAM_RATE_ENT);

enumDescr enumDescr_CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E),
    ENUM_ENTRY(CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E),
    ENUM_ENTRY(CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT);

enumDescr enumDescr_CPSS_MAC_HASH_FUNC_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_MAC_HASH_FUNC_XOR_E),
    ENUM_ENTRY(CPSS_MAC_HASH_FUNC_CRC_E),
    ENUM_ENTRY(CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_MAC_HASH_FUNC_MODE_ENT);

enumDescr enumDescr_CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_SIZEOF_1BYTE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_SIZEOF_2BYTES_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_SIZEOF_4BYTES_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_SIZEOF_8BYTES_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_BOOL_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_INVERTED_BOOL_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_1BYTE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_2BYTES_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_4BYTES_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_ASCENT_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CONSTANT_VALUE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_DUMMY_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_ENDLIST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT);

enumDescr enumDescr_CPSS_LED_ORDER_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_LED_ORDER_MODE_BY_PORT_E),
    ENUM_ENTRY(CPSS_LED_ORDER_MODE_BY_CLASS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LED_ORDER_MODE_ENT);

enumDescr enumDescr_CPSS_TM_ELIG_FUNC_NODE_ENT[] = {
    ENUM_ENTRY(CPSS_TM_ELIG_N_PRIO1_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PRIO5_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_SHP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PPA_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PPA_SP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PPA_SHP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PPA_SP_MIN_SHP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PPA_SHP_IGN_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PPA_MIN_SHP_SP_IGN_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_FP0_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_FP1_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_FP2_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_FP3_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_FP4_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_FP5_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_FP6_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_FP7_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_FP0_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_FP1_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_FP2_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_FP3_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_FP4_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_FP5_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_FP6_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_FP7_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MAX_INC_MIN_SHP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_MIN_SHP_PP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_PP_SHP_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_SHP_4P_MIN_4P_MAX_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_SHP_PP_TB_E),
    ENUM_ENTRY(CPSS_TM_ELIG_N_SHP_PP_MAX_TB_0),
    ENUM_ENTRY(CPSS_TM_ELIG_N_DEQ_DIS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM_ELIG_FUNC_NODE_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_CTRL_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_CTRL_STC *val
)
{
    F_ENUM(val, -1, pseEnable, CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT);
    F_ENUM(val, -1, pairControl, CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT);
    F_ENUM(val, -1, portMode, CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT);
    F_ENUM(val, -1, portPriority, CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_CTRL_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_CTRL_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, pseEnable, CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT);
    FO_ENUM(val, t, pairControl, CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT);
    FO_ENUM(val, t, portMode, CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT);
    FO_ENUM(val, t, portPriority, CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_CTRL_STC);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_DISABLED_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_SEARCHING_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_DELIVERING_POWER_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_TEST_MODE_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_TEST_ERROR_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_IMPLEMENTATION_SPECIFIC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT);

void prv_lua_to_c_CPSS_SYSTEM_RECOVERY_INFO_STC(
    lua_State *L,
    CPSS_SYSTEM_RECOVERY_INFO_STC *val
)
{
    F_ENUM(val, -1, systemRecoveryState, CPSS_SYSTEM_RECOVERY_STATE_ENT);
    F_STRUCT(val, -1, systemRecoveryMode, CPSS_SYSTEM_RECOVERY_MODE_STC);
    F_ENUM(val, -1, systemRecoveryProcess, CPSS_SYSTEM_RECOVERY_PROCESS_ENT);
}

void prv_c_to_lua_CPSS_SYSTEM_RECOVERY_INFO_STC(
    lua_State *L,
    CPSS_SYSTEM_RECOVERY_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, systemRecoveryState, CPSS_SYSTEM_RECOVERY_STATE_ENT);
    FO_STRUCT(val, t, systemRecoveryMode, CPSS_SYSTEM_RECOVERY_MODE_STC);
    FO_ENUM(val, t, systemRecoveryProcess, CPSS_SYSTEM_RECOVERY_PROCESS_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_SYSTEM_RECOVERY_INFO_STC);

enumDescr enumDescr_CPSS_ETHER_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_VLAN_ETHERTYPE0_E),
    ENUM_ENTRY(CPSS_VLAN_ETHERTYPE1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_ETHER_MODE_ENT);

enumDescr enumDescr_CPSS_PORT_TX_DROP_PROFILE_SET_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_1_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_2_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_3_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_4_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_5_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_6_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_7_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_8_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_9_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_10_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_11_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_12_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_13_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_14_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_15_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_PROFILE_16_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_TX_DROP_PROFILE_SET_ENT);

enumDescr enumDescr_CPSS_NST_EGRESS_FRW_FILTER_ENT[] = {
    ENUM_ENTRY(CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E),
    ENUM_ENTRY(CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E),
    ENUM_ENTRY(CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E),
    ENUM_ENTRY(CPSS_NST_EGRESS_FRW_FILTER_MPLS_E),
    ENUM_ENTRY(CPSS_NST_EGRESS_FRW_FILTER_L2VPN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NST_EGRESS_FRW_FILTER_ENT);

void prv_lua_to_c_CPSS_VCT_PAIR_SKEW_STC(
    lua_State *L,
    CPSS_VCT_PAIR_SKEW_STC *val
)
{
    F_BOOL(val, -1, isValid);
    F_ARRAY_START(val, -1, skew);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, skew, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, skew);
}

void prv_c_to_lua_CPSS_VCT_PAIR_SKEW_STC(
    lua_State *L,
    CPSS_VCT_PAIR_SKEW_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, isValid);
    FO_ARRAY_START(val, t, skew);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, skew, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, skew);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VCT_PAIR_SKEW_STC);

void prv_lua_to_c_CPSS_TM_PORT_PARAMS_STC(
    lua_State *L,
    CPSS_TM_PORT_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, cirBw, GT_U32);
    F_NUMBER(val, -1, eirBw, GT_U32);
    F_NUMBER(val, -1, cbs, GT_U32);
    F_NUMBER(val, -1, ebs, GT_U32);
    F_ARRAY_START(val, -1, quantumArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, quantumArr, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, quantumArr);
    F_ARRAY_START(val, -1, schdModeArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_ENUM(val, schdModeArr, idx, CPSS_TM_SCHD_MODE_ENT);
        }
    }
    F_ARRAY_END(val, -1, schdModeArr);
    F_NUMBER(val, -1, dropProfileInd, GT_U32);
    F_ENUM(val, -1, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_NODE_ENT);
    F_NUMBER(val, -1, numOfChildren, GT_U32);
}

void prv_c_to_lua_CPSS_TM_PORT_PARAMS_STC(
    lua_State *L,
    CPSS_TM_PORT_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, cirBw, GT_U32);
    FO_NUMBER(val, t, eirBw, GT_U32);
    FO_NUMBER(val, t, cbs, GT_U32);
    FO_NUMBER(val, t, ebs, GT_U32);
    FO_ARRAY_START(val, t, quantumArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, quantumArr, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, quantumArr);
    FO_ARRAY_START(val, t, schdModeArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_ENUM(val, schdModeArr, idx, CPSS_TM_SCHD_MODE_ENT);
        }
    }
    FO_ARRAY_END(val, t, schdModeArr);
    FO_NUMBER(val, t, dropProfileInd, GT_U32);
    FO_ENUM(val, t, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_NODE_ENT);
    FO_NUMBER(val, t, numOfChildren, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_PORT_PARAMS_STC);

enumDescr enumDescr_CPSS_IP_PROTOCOL_STACK_ENT[] = {
    ENUM_ENTRY(CPSS_IP_PROTOCOL_IPV4_E),
    ENUM_ENTRY(CPSS_IP_PROTOCOL_IPV6_E),
    ENUM_ENTRY(CPSS_IP_PROTOCOL_IPV4V6_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_IP_PROTOCOL_STACK_ENT);

enumDescr enumDescr_CPSS_LED_BLINK_DURATION_ENT[] = {
    ENUM_ENTRY(CPSS_LED_BLINK_DURATION_0_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DURATION_1_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DURATION_2_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DURATION_3_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DURATION_4_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DURATION_5_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DURATION_6_E),
    ENUM_ENTRY(CPSS_LED_BLINK_DURATION_7_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LED_BLINK_DURATION_ENT);

void prv_lua_to_c_CPSS_TRUNK_MEMBER_STC(
    lua_State *L,
    CPSS_TRUNK_MEMBER_STC *val
)
{
    F_NUMBER(val, -1, port, GT_PHYSICAL_PORT_NUM);
    F_NUMBER_N(val, -1, hwDevice, device, GT_HW_DEV_NUM);
}

void prv_c_to_lua_CPSS_TRUNK_MEMBER_STC(
    lua_State *L,
    CPSS_TRUNK_MEMBER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, port, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER_N(val, t, hwDevice, device, GT_HW_DEV_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TRUNK_MEMBER_STC);

enumDescr enumDescr_CPSS_SYS_HA_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_SYS_HA_MODE_ACTIVE_E),
    ENUM_ENTRY(CPSS_SYS_HA_MODE_STANDBY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_SYS_HA_MODE_ENT);

void prv_lua_to_c_CPSS_TM_UNITS_ERROR_STATUS_STC(
    lua_State *L,
    CPSS_TM_UNITS_ERROR_STATUS_STC *val
)
{
    F_NUMBER(val, -1, qmngrStatus, GT_U32);
    F_NUMBER(val, -1, dropStatus, GT_U32);
    F_NUMBER(val, -1, schedStatus, GT_U32);
    F_NUMBER(val, -1, rcbStatus, GT_U32);
}

void prv_c_to_lua_CPSS_TM_UNITS_ERROR_STATUS_STC(
    lua_State *L,
    CPSS_TM_UNITS_ERROR_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, qmngrStatus, GT_U32);
    FO_NUMBER(val, t, dropStatus, GT_U32);
    FO_NUMBER(val, t, schedStatus, GT_U32);
    FO_NUMBER(val, t, rcbStatus, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_UNITS_ERROR_STATUS_STC);

void prv_lua_to_c_CPSS_SYSTEM_RECOVERY_MODE_STC(
    lua_State *L,
    CPSS_SYSTEM_RECOVERY_MODE_STC *val
)
{
    F_BOOL(val, -1, continuousRx);
    F_BOOL(val, -1, continuousTx);
    F_BOOL(val, -1, continuousAuMessages);
    F_BOOL(val, -1, continuousFuMessages);
    F_BOOL(val, -1, haCpuMemoryAccessBlocked);
}

void prv_c_to_lua_CPSS_SYSTEM_RECOVERY_MODE_STC(
    lua_State *L,
    CPSS_SYSTEM_RECOVERY_MODE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, continuousRx);
    FO_BOOL(val, t, continuousTx);
    FO_BOOL(val, t, continuousAuMessages);
    FO_BOOL(val, t, continuousFuMessages);
    FO_BOOL(val, t, haCpuMemoryAccessBlocked);
    lua_settop(L, t);
}
add_mgm_type(CPSS_SYSTEM_RECOVERY_MODE_STC);

enumDescr enumDescr_CPSS_PORT_PCS_RESET_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_PCS_RESET_MODE_RX_E),
    ENUM_ENTRY(CPSS_PORT_PCS_RESET_MODE_TX_E),
    ENUM_ENTRY(CPSS_PORT_PCS_RESET_MODE_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_PCS_RESET_MODE_ENT);

enumDescr enumDescr_CPSS_TM_PORT_BW_ENT[] = {
    ENUM_ENTRY(CPSS_TM_PORT_BW_1G_E),
    ENUM_ENTRY(CPSS_TM_PORT_BW_2HG_E),
    ENUM_ENTRY(CPSS_TM_PORT_BW_10G_E),
    ENUM_ENTRY(CPSS_TM_PORT_BW_40G_E),
    ENUM_ENTRY(CPSS_TM_PORT_BW_50G_E),
    ENUM_ENTRY(CPSS_TM_PORT_BW_100G_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM_PORT_BW_ENT);

enumDescr enumDescr_CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_NONE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_NOT_SAVE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_SAVE_NOT_USE_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_AND_SAVE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT);

enumDescr enumDescr_CPSS_BRIDGE_INGR_CNTR_MODES_ENT[] = {
    ENUM_ENTRY(CPSS_BRG_CNT_MODE_0_E),
    ENUM_ENTRY(CPSS_BRG_CNT_MODE_1_E),
    ENUM_ENTRY(CPSS_BRG_CNT_MODE_2_E),
    ENUM_ENTRY(CPSS_BRG_CNT_MODE_3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BRIDGE_INGR_CNTR_MODES_ENT);

enumDescr enumDescr_CPSS_POLICER_TB_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_POLICER_TB_STRICT_E),
    ENUM_ENTRY(CPSS_POLICER_TB_LOOSE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_POLICER_TB_MODE_ENT);

void prv_lua_to_c_CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC(
    lua_State *L,
    CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC *val
)
{
    F_ENUM(val, -1, fieldTypeOrSizeOf, CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT);
    F_NUMBER(val, -1, stcOrExtMemByteOffset, GT_U16);
    F_NUMBER(val, -1, mask, GT_U32);
    F_NUMBER(val, -1, pattern, GT_U32);
}

void prv_c_to_lua_CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC(
    lua_State *L,
    CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, fieldTypeOrSizeOf, CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT);
    FO_NUMBER(val, t, stcOrExtMemByteOffset, GT_U16);
    FO_NUMBER(val, t, mask, GT_U32);
    FO_NUMBER(val, t, pattern, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC);

void prv_lua_to_c_CPSS_TM_DROP_PROFILE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_DROP_PROFILE_PARAMS_STC *val
)
{
    F_ENUM(val, -1, dropMode, CPSS_TM_DROP_MODE_ENT);
    F_NUMBER(val, -1, cbTdThresholdBytes, GT_U32);
    F_STRUCT(val, -1, caTdDp, CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC);
    F_STRUCT(val, -1, caWredDp, CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC);
}

void prv_c_to_lua_CPSS_TM_DROP_PROFILE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_DROP_PROFILE_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dropMode, CPSS_TM_DROP_MODE_ENT);
    FO_NUMBER(val, t, cbTdThresholdBytes, GT_U32);
    FO_STRUCT(val, t, caTdDp, CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC);
    FO_STRUCT(val, t, caWredDp, CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_DROP_PROFILE_PARAMS_STC);

enumDescr enumDescr_CPSS_HSU_DATA_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_HSU_DATA_TYPE_TCAM_MANAGER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_HSU_DATA_TYPE_ENT);

enumDescr enumDescr_CPSS_DRAM_BUS_WIDTH_ENT[] = {
    ENUM_ENTRY(CPSS_DRAM_BUS_WIDTH_4_E),
    ENUM_ENTRY(CPSS_DRAM_BUS_WIDTH_8_E),
    ENUM_ENTRY(CPSS_DRAM_BUS_WIDTH_16_E),
    ENUM_ENTRY(CPSS_DRAM_BUS_WIDTH_32_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DRAM_BUS_WIDTH_ENT);

enumDescr enumDescr_CPSS_TUNNEL_MULTICAST_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_TUNNEL_MULTICAST_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_MULTICAST_IPV6_E),
    ENUM_ENTRY(CPSS_TUNNEL_MULTICAST_MPLS_E),
    ENUM_ENTRY(CPSS_TUNNEL_MULTICAST_TRILL_E),
    ENUM_ENTRY(CPSS_TUNNEL_MULTICAST_PBB_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TUNNEL_MULTICAST_TYPE_ENT);

enumDescr enumDescr_CPSS_LED_PULSE_STRETCH_ENT[] = {
    ENUM_ENTRY(CPSS_LED_PULSE_STRETCH_0_NO_E),
    ENUM_ENTRY(CPSS_LED_PULSE_STRETCH_1_E),
    ENUM_ENTRY(CPSS_LED_PULSE_STRETCH_2_E),
    ENUM_ENTRY(CPSS_LED_PULSE_STRETCH_3_E),
    ENUM_ENTRY(CPSS_LED_PULSE_STRETCH_4_E),
    ENUM_ENTRY(CPSS_LED_PULSE_STRETCH_5_E),
    ENUM_ENTRY(CPSS_LED_PULSE_STRETCH_6_E),
    ENUM_ENTRY(CPSS_LED_PULSE_STRETCH_7_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LED_PULSE_STRETCH_ENT);

enumDescr enumDescr_CPSS_DIAG_PP_MEM_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_BUFFER_DRAM_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_WIDE_SRAM_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_NARROW_SRAM_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_0_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_1_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_0_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_1_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_FLOW_DRAM_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PP_MEM_TYPE_ENT);

void prv_lua_to_c_CPSS_TM_ERROR_INFO_STC(
    lua_State *L,
    CPSS_TM_ERROR_INFO_STC *val
)
{
    F_NUMBER(val, -1, errorCounter, GT_U32);
    F_NUMBER(val, -1, exceptionCounter, GT_U32);
}

void prv_c_to_lua_CPSS_TM_ERROR_INFO_STC(
    lua_State *L,
    CPSS_TM_ERROR_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, errorCounter, GT_U32);
    FO_NUMBER(val, t, exceptionCounter, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_ERROR_INFO_STC);

enumDescr enumDescr_CPSS_UNREG_MC_EGR_FILTER_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_UNREG_MC_VLAN_FRWD_E),
    ENUM_ENTRY(CPSS_UNREG_MC_VLAN_CPU_FRWD_E),
    ENUM_ENTRY(CPSS_UNREG_MC_CPU_FRWD_E),
    ENUM_ENTRY(CPSS_UNREG_MC_NOT_CPU_FRWD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_UNREG_MC_EGR_FILTER_CMD_ENT);

enumDescr enumDescr_CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT[] = {
    ENUM_ENTRY(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E),
    ENUM_ENTRY(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E),
    ENUM_ENTRY(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E),
    ENUM_ENTRY(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E),
    ENUM_ENTRY(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E),
    ENUM_ENTRY(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E),
    ENUM_ENTRY(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_MAX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT);

enumDescr enumDescr_CPSS_DRAM_FREQUENCY_ENT[] = {
    ENUM_ENTRY(CPSS_DRAM_FREQ_100_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_125_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_166_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_200_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_250_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_300_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_667_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_800_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_933_MHZ_E),
    ENUM_ENTRY(CPSS_DRAM_FREQ_1066_MHZ_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DRAM_FREQUENCY_ENT);

void prv_lua_to_c_CPSS_VCT_CONFIG_STC(
    lua_State *L,
    CPSS_VCT_CONFIG_STC *val
)
{
    F_ENUM(val, -1, length, CPSS_VCT_CABLE_LEN_ENT);
}

void prv_c_to_lua_CPSS_VCT_CONFIG_STC(
    lua_State *L,
    CPSS_VCT_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, length, CPSS_VCT_CABLE_LEN_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VCT_CONFIG_STC);

enumDescr enumDescr_CPSS_INTERFACE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_INTERFACE_PORT_E),
    ENUM_ENTRY(CPSS_INTERFACE_TRUNK_E),
    ENUM_ENTRY(CPSS_INTERFACE_VIDX_E),
    ENUM_ENTRY(CPSS_INTERFACE_VID_E),
    ENUM_ENTRY(CPSS_INTERFACE_DEVICE_E),
    ENUM_ENTRY(CPSS_INTERFACE_FABRIC_VIDX_E),
    ENUM_ENTRY(CPSS_INTERFACE_INDEX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_INTERFACE_TYPE_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC *val
)
{
    F_ARRAY_START(val, -1, invalidSignatureCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            F_ARRAY_NUMBER(val, invalidSignatureCounter, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, invalidSignatureCounter);
    F_ARRAY_START(val, -1, powerDeniedCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            F_ARRAY_NUMBER(val, powerDeniedCounter, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, powerDeniedCounter);
    F_ARRAY_START(val, -1, overloadCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            F_ARRAY_NUMBER(val, overloadCounter, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, overloadCounter);
    F_ARRAY_START(val, -1, shortCyrcuitCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            F_ARRAY_NUMBER(val, shortCyrcuitCounter, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, shortCyrcuitCounter);
    F_ARRAY_START(val, -1, underloadCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            F_ARRAY_NUMBER(val, underloadCounter, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, underloadCounter);
    F_ARRAY_START(val, -1, classErrorCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            F_ARRAY_NUMBER(val, classErrorCounter, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, classErrorCounter);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, invalidSignatureCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            FO_ARRAY_NUMBER(val, invalidSignatureCounter, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, invalidSignatureCounter);
    FO_ARRAY_START(val, t, powerDeniedCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            FO_ARRAY_NUMBER(val, powerDeniedCounter, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, powerDeniedCounter);
    FO_ARRAY_START(val, t, overloadCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            FO_ARRAY_NUMBER(val, overloadCounter, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, overloadCounter);
    FO_ARRAY_START(val, t, shortCyrcuitCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            FO_ARRAY_NUMBER(val, shortCyrcuitCounter, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, shortCyrcuitCounter);
    FO_ARRAY_START(val, t, underloadCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            FO_ARRAY_NUMBER(val, underloadCounter, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, underloadCounter);
    FO_ARRAY_START(val, t, classErrorCounter);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            FO_ARRAY_NUMBER(val, classErrorCounter, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, classErrorCounter);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC);

enumDescr enumDescr_CPSS_UNICAST_MULTICAST_ENT[] = {
    ENUM_ENTRY(CPSS_UNICAST_E),
    ENUM_ENTRY(CPSS_MULTICAST_E),
    ENUM_ENTRY(CPSS_UNICAST_MULTICAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_UNICAST_MULTICAST_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_MATRIX_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_MATRIX_STC *val
)
{
    F_NUMBER(val, -1, physicalPortNumber, GT_U8);
    F_NUMBER(val, -1, chipNumber, GT_U8);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_MATRIX_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_MATRIX_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, physicalPortNumber, GT_U8);
    FO_NUMBER(val, t, chipNumber, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_MATRIX_STC);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC *val
)
{
    F_NUMBER(val, -1, powerConsumption, GT_U32);
    F_NUMBER(val, -1, current, GT_U32);
    F_NUMBER(val, -1, volt, GT_U32);
    F_NUMBER(val, -1, calcPowerConsumption, GT_U32);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, powerConsumption, GT_U32);
    FO_NUMBER(val, t, current, GT_U32);
    FO_NUMBER(val, t, volt, GT_U32);
    FO_NUMBER(val, t, calcPowerConsumption, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC);

enumDescr enumDescr_CPSS_PACKET_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_PACKET_CMD_FORWARD_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_MIRROR_TO_CPU_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_TRAP_TO_CPU_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_DROP_HARD_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_DROP_SOFT_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_ROUTE_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_BRIDGE_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_NONE_E),
    ENUM_ENTRY(CPSS_PACKET_CMD_LOOPBACK_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PACKET_CMD_ENT);

enumDescr enumDescr_CPSS_VCT_PHY_TYPES_ENT[] = {
    ENUM_ENTRY(CPSS_VCT_PHY_100M_E),
    ENUM_ENTRY(CPSS_VCT_PHY_1000M_E),
    ENUM_ENTRY(CPSS_VCT_PHY_10000M_E),
    ENUM_ENTRY(CPSS_VCT_PHY_1000M_B_E),
    ENUM_ENTRY(CPSS_VCT_PHY_1000M_MP_E),
    ENUM_ENTRY(CPSS_VCT_PHY_1000M_MP_NO_FIBER_E),
    ENUM_ENTRY(CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_PHY_TYPES_ENT);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_DISABLED_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENABLED_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_FORCE_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_RESERVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT);

enumDescr enumDescr_CPSS_DROP_MODE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DROP_MODE_SOFT_E),
    ENUM_ENTRY(CPSS_DROP_MODE_HARD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DROP_MODE_TYPE_ENT);

enumDescr enumDescr_CPSS_LED_CLASS_5_SELECT_ENT[] = {
    ENUM_ENTRY(CPSS_LED_CLASS_5_SELECT_HALF_DUPLEX_E),
    ENUM_ENTRY(CPSS_LED_CLASS_5_SELECT_FIBER_LINK_UP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LED_CLASS_5_SELECT_ENT);

enumDescr enumDescr_CPSS_NST_INGRESS_FRW_FILTER_ENT[] = {
    ENUM_ENTRY(CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E),
    ENUM_ENTRY(CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E),
    ENUM_ENTRY(CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NST_INGRESS_FRW_FILTER_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_COMMAND_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_COMMAND_STC *val
)
{
    F_STRUCT(val, -1, portCtrl, CPSS_GEN_DRAGONITE_PORT_CTRL_STC);
    F_NUMBER(val, -1, portPpl, GT_U32);
    F_NUMBER(val, -1, portTppl, GT_U32);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_COMMAND_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_COMMAND_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, portCtrl, CPSS_GEN_DRAGONITE_PORT_CTRL_STC);
    FO_NUMBER(val, t, portPpl, GT_U32);
    FO_NUMBER(val, t, portTppl, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_COMMAND_STC);

enumDescr enumDescr_CPSS_PORT_TX_WRR_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_TX_WRR_BYTE_MODE_E),
    ENUM_ENTRY(CPSS_PORT_TX_WRR_PACKET_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_TX_WRR_MODE_ENT);

void prv_lua_to_c_CPSS_GEN_CFG_DEV_INFO_STC(
    lua_State *L,
    CPSS_GEN_CFG_DEV_INFO_STC *val
)
{
    F_NUMBER(val, -1, devType, CPSS_PP_DEVICE_TYPE);
    F_NUMBER(val, -1, revision, GT_U8);
    F_ENUM(val, -1, devFamily, CPSS_PP_FAMILY_TYPE_ENT);
    F_ENUM(val, -1, devSubFamily, CPSS_PP_SUB_FAMILY_TYPE_ENT);
    F_NUMBER(val, -1, maxPortNum, GT_U8);
    F_NUMBER(val, -1, numOfVirtPorts, GT_U8);
    F_STRUCT(val, -1, existingPorts, CPSS_PORTS_BMP_STC);
    F_ENUM(val, -1, hwDevNumMode, CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);
    F_ENUM(val, -1, cpuPortMode, CPSS_NET_CPU_PORT_MODE_ENT);
    F_NUMBER(val, -1, numOfLedInfPerPortGroup, GT_U32);
}

void prv_c_to_lua_CPSS_GEN_CFG_DEV_INFO_STC(
    lua_State *L,
    CPSS_GEN_CFG_DEV_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, devType, CPSS_PP_DEVICE_TYPE);
    FO_NUMBER(val, t, revision, GT_U8);
    FO_ENUM(val, t, devFamily, CPSS_PP_FAMILY_TYPE_ENT);
    FO_ENUM(val, t, devSubFamily, CPSS_PP_SUB_FAMILY_TYPE_ENT);
    FO_NUMBER(val, t, maxPortNum, GT_U8);
    FO_NUMBER(val, t, numOfVirtPorts, GT_U8);
    FO_STRUCT(val, t, existingPorts, CPSS_PORTS_BMP_STC);
    FO_ENUM(val, t, hwDevNumMode, CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);
    FO_ENUM(val, t, cpuPortMode, CPSS_NET_CPU_PORT_MODE_ENT);
    FO_NUMBER(val, t, numOfLedInfPerPortGroup, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_CFG_DEV_INFO_STC);

enumDescr enumDescr_CPSS_TX_BUFF_ALLOC_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_TX_BUFF_DYNAMIC_ALLOC_E),
    ENUM_ENTRY(CPSS_TX_BUFF_STATIC_ALLOC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TX_BUFF_ALLOC_METHOD_ENT);

enumDescr enumDescr_CPSS_PP_SUB_FAMILY_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_PP_SUB_FAMILY_NONE_E),
    ENUM_ENTRY(CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E),
    ENUM_ENTRY(CPSS_MAX_SUB_FAMILY),
    ENUM_ENTRY(CPSS_BAD_SUB_FAMILY),
    { 0, NULL }
};
add_mgm_enum(CPSS_PP_SUB_FAMILY_TYPE_ENT);

void prv_lua_to_c_CPSS_VERSION_INFO_STC(
    lua_State *L,
    CPSS_VERSION_INFO_STC *val
)
{
    F_ARRAY_START(val, -1, version);
    {
        int idx;
        for (idx = 0; idx < 30; idx++) {
            F_ARRAY_NUMBER(val, version, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, version);
}

void prv_c_to_lua_CPSS_VERSION_INFO_STC(
    lua_State *L,
    CPSS_VERSION_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, version);
    {
        int idx;
        for (idx = 0; idx < 30; idx++) {
            FO_ARRAY_NUMBER(val, version, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, version);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VERSION_INFO_STC);

enumDescr enumDescr_CPSS_MAC_SA_LSB_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_SA_LSB_PER_PORT_E),
    ENUM_ENTRY(CPSS_SA_LSB_PER_PKT_VID_E),
    ENUM_ENTRY(CPSS_SA_LSB_PER_VLAN_E),
    ENUM_ENTRY(CPSS_SA_LSB_FULL_48_BIT_GLOBAL),
    { 0, NULL }
};
add_mgm_enum(CPSS_MAC_SA_LSB_MODE_ENT);

enumDescr enumDescr_CPSS_DRAM_TEMPERATURE_ENT[] = {
    ENUM_ENTRY(CPSS_DRAM_TEMPERATURE_NORMAL_E),
    ENUM_ENTRY(CPSS_DRAM_TEMPERATURE_HIGH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DRAM_TEMPERATURE_ENT);

enumDescr enumDescr_CPSS_AU_MESSAGE_LENGTH_ENT[] = {
    ENUM_ENTRY(CPSS_AU_MESSAGE_LENGTH_4_WORDS_E),
    ENUM_ENTRY(CPSS_AU_MESSAGE_LENGTH_8_WORDS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_AU_MESSAGE_LENGTH_ENT);

enumDescr enumDescr_CPSS_LOG_POINTER_FORMAT_ENT[] = {
    ENUM_ENTRY(CPSS_LOG_POINTER_FORMAT_SIMPLE_E),
    ENUM_ENTRY(CPSS_LOG_POINTER_FORMAT_PREFIX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LOG_POINTER_FORMAT_ENT);

enumDescr enumDescr_CPSS_STP_STATE_ENT[] = {
    ENUM_ENTRY(CPSS_STP_DISABLED_E),
    ENUM_ENTRY(CPSS_STP_BLCK_LSTN_E),
    ENUM_ENTRY(CPSS_STP_LRN_E),
    ENUM_ENTRY(CPSS_STP_FRWRD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_STP_STATE_ENT);

enumDescr enumDescr_CPSS_LOG_API_FORMAT_ENT[] = {
    ENUM_ENTRY(CPSS_LOG_API_FORMAT_NO_PARAMS_E),
    ENUM_ENTRY(CPSS_LOG_API_FORMAT_ALL_PARAMS_E),
    ENUM_ENTRY(CPSS_LOG_API_FORMAT_NON_ZERO_PARAMS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LOG_API_FORMAT_ENT);

enumDescr enumDescr_CPSS_BRG_TPID_SIZE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_BRG_TPID_SIZE_TYPE_4_BYTES_E),
    ENUM_ENTRY(CPSS_BRG_TPID_SIZE_TYPE_8_BYTES_E),
    ENUM_ENTRY(CPSS_BRG_TPID_SIZE_TYPE_6_BYTES_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BRG_TPID_SIZE_TYPE_ENT);

enumDescr enumDescr_CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT[] = {
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E),
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);

void prv_lua_to_c_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC(
    lua_State *L,
    CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC *val
)
{
    F_NUMBER(val, -1, dfxPipeId, GT_U32);
    F_NUMBER(val, -1, dfxClientId, GT_U32);
    F_NUMBER(val, -1, dfxMemoryId, GT_U32);
}

void prv_c_to_lua_CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC(
    lua_State *L,
    CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, dfxPipeId, GT_U32);
    FO_NUMBER(val, t, dfxClientId, GT_U32);
    FO_NUMBER(val, t, dfxMemoryId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC);

void prv_lua_to_c_CPSS_VCT_STATUS_STC(
    lua_State *L,
    CPSS_VCT_STATUS_STC *val
)
{
    F_ENUM(val, -1, testStatus, CPSS_VCT_TEST_STATUS_ENT);
    F_NUMBER(val, -1, errCableLen, GT_U8);
}

void prv_c_to_lua_CPSS_VCT_STATUS_STC(
    lua_State *L,
    CPSS_VCT_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, testStatus, CPSS_VCT_TEST_STATUS_ENT);
    FO_NUMBER(val, t, errCableLen, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_VCT_STATUS_STC);

enumDescr enumDescr_CPSS_TRUNK_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_TRUNK_TYPE_FREE_E),
    ENUM_ENTRY(CPSS_TRUNK_TYPE_REGULAR_E),
    ENUM_ENTRY(CPSS_TRUNK_TYPE_CASCADE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TRUNK_TYPE_ENT);

enumDescr enumDescr_CPSS_IP_UNICAST_MULTICAST_ENT[] = {
    ENUM_ENTRY(CPSS_IP_UNICAST_E),
    ENUM_ENTRY(CPSS_IP_MULTICAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_IP_UNICAST_MULTICAST_ENT);

enumDescr enumDescr_CPSS_PP_UPLINK_CONFIG_ENT[] = {
    ENUM_ENTRY(CPSS_PP_UPLINK_BACK_TO_BACK_E),
    ENUM_ENTRY(CPSS_PP_UPLINK_FA_E),
    ENUM_ENTRY(CPSS_PP_NO_UPLINK_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PP_UPLINK_CONFIG_ENT);

enumDescr enumDescr_CPSS_MAC_VL_ENT[] = {
    ENUM_ENTRY(CPSS_IVL_E),
    ENUM_ENTRY(CPSS_SVL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_MAC_VL_ENT);

enumDescr enumDescr_CPSS_TUNNEL_PASSENGER_PACKET_ENT[] = {
    ENUM_ENTRY(CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E),
    ENUM_ENTRY(CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E),
    ENUM_ENTRY(CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E),
    ENUM_ENTRY(CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TUNNEL_PASSENGER_PACKET_ENT);

void prv_lua_to_c_CPSS_TM_NODE_STATUS_STC(
    lua_State *L,
    CPSS_TM_NODE_STATUS_STC *val
)
{
    F_NUMBER(val, -1, maxBucketLevel, GT_U32);
    F_NUMBER(val, -1, minBucketLevel, GT_U32);
    F_NUMBER(val, -1, deficit, GT_U32);
}

void prv_c_to_lua_CPSS_TM_NODE_STATUS_STC(
    lua_State *L,
    CPSS_TM_NODE_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, maxBucketLevel, GT_U32);
    FO_NUMBER(val, t, minBucketLevel, GT_U32);
    FO_NUMBER(val, t, deficit, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_NODE_STATUS_STC);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_0_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_1_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_2_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_3_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_4_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ERROR_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_RESERVED_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_UNDEFINED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT);

void prv_lua_to_c_CPSS_COS_MAP_STC(
    lua_State *L,
    CPSS_COS_MAP_STC *val
)
{
    F_NUMBER(val, -1, dscp, GT_U8);
    F_STRUCT(val, -1, cosParam, CPSS_COS_STC);
    F_NUMBER(val, -1, expField, GT_U8);
    F_NUMBER(val, -1, userDef, GT_U16);
}

void prv_c_to_lua_CPSS_COS_MAP_STC(
    lua_State *L,
    CPSS_COS_MAP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, dscp, GT_U8);
    FO_STRUCT(val, t, cosParam, CPSS_COS_STC);
    FO_NUMBER(val, t, expField, GT_U8);
    FO_NUMBER(val, t, userDef, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_COS_MAP_STC);

enumDescr enumDescr_CPSS_TM_LEVEL_ENT[] = {
    ENUM_ENTRY(CPSS_TM_LEVEL_Q_E),
    ENUM_ENTRY(CPSS_TM_LEVEL_A_E),
    ENUM_ENTRY(CPSS_TM_LEVEL_B_E),
    ENUM_ENTRY(CPSS_TM_LEVEL_C_E),
    ENUM_ENTRY(CPSS_TM_LEVEL_P_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM_LEVEL_ENT);

enumDescr enumDescr_CPSS_POLICER_PACKET_SIZE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E),
    ENUM_ENTRY(CPSS_POLICER_PACKET_SIZE_L3_ONLY_E),
    ENUM_ENTRY(CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E),
    ENUM_ENTRY(CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_POLICER_PACKET_SIZE_MODE_ENT);

enumDescr enumDescr_CPSS_HW_PP_RESET_SKIP_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E),
    ENUM_ENTRY(CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E),
    ENUM_ENTRY(CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E),
    ENUM_ENTRY(CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E),
    ENUM_ENTRY(CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E),
    ENUM_ENTRY(CPSS_HW_PP_RESET_SKIP_TYPE_ALL_E),
    ENUM_ENTRY(CPSS_HW_PP_RESET_SKIP_TYPE_ALL_EXCLUDE_PEX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_HW_PP_RESET_SKIP_TYPE_ENT);

enumDescr enumDescr_CPSS_DIAG_PP_REG_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PP_REG_INTERNAL_E),
    ENUM_ENTRY(CPSS_DIAG_PP_REG_PCI_CFG_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PP_REG_TYPE_ENT);

enumDescr enumDescr_CPSS_NET_RX_CPU_CODE_ENT[] = {
    ENUM_ENTRY(CPSS_NET_CONTROL_E),
    ENUM_ENTRY(CPSS_NET_UN_KNOWN_UC_E),
    ENUM_ENTRY(CPSS_NET_UN_REGISTERD_MC_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_BPDU_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_DEST_MAC_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_SRC_MAC_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_MAC_RANGE_TRAP_E),
    ENUM_ENTRY(CPSS_NET_RX_SNIFFER_TRAP_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_ARP_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_IGMP_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_SA_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_DA_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_SA_DA_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_PORT_LOCK_E),
    ENUM_ENTRY(CPSS_NET_EXP_TNL_BAD_IPV4_HDR_E),
    ENUM_ENTRY(CPSS_NET_EXP_TNL_BAD_VLAN_E),
    ENUM_ENTRY(CPSS_NET_MLL_RPF_TRAP_E),
    ENUM_ENTRY(CPSS_NET_RESERVED_SIP_TRAP_E),
    ENUM_ENTRY(CPSS_NET_INTERNAL_SIP_TRAP_E),
    ENUM_ENTRY(CPSS_NET_SIP_SPOOF_TRAP_E),
    ENUM_ENTRY(CPSS_NET_DEF_KEY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IP_CLASS_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CLASS_KEY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_TCP_RST_FIN_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CLASS_KEY_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_TRAP_BY_DEFAULT_ENTRY0_E),
    ENUM_ENTRY(CPSS_NET_TRAP_BY_DEFAULT_ENTRY1_E),
    ENUM_ENTRY(CPSS_NET_TRAP_UD_INVALID_E),
    ENUM_ENTRY(CPSS_NET_RESERVED_DIP_TRAP_E),
    ENUM_ENTRY(CPSS_NET_MC_BOUNDARY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_INTERNAL_DIP_E),
    ENUM_ENTRY(CPSS_NET_IP_ZERO_TTL_TRAP_E),
    ENUM_ENTRY(CPSS_NET_BAD_IP_HDR_CHECKSUM_E),
    ENUM_ENTRY(CPSS_NET_RPF_CHECK_FAILED_E),
    ENUM_ENTRY(CPSS_NET_OPTIONS_IN_IP_HDR_E),
    ENUM_ENTRY(CPSS_NET_END_OF_IP_TUNNEL_E),
    ENUM_ENTRY(CPSS_NET_BAD_TUNNEL_HDR_E),
    ENUM_ENTRY(CPSS_NET_IP_HDR_ERROR_E),
    ENUM_ENTRY(CPSS_NET_ROUTE_ENTRY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_DIP_CHECK_ERROR_E),
    ENUM_ENTRY(CPSS_NET_ILLEGAL_DIP_E),
    ENUM_ENTRY(CPSS_NET_POLICY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_DEFAULT_ROUTE_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IP_MTU_EXCEED_E),
    ENUM_ENTRY(CPSS_NET_MPLS_MTU_EXCEED_E),
    ENUM_ENTRY(CPSS_NET_CLASS_MTU_EXCEED_E),
    ENUM_ENTRY(CPSS_NET_MPLS_ZERO_TTL_TRAP_E),
    ENUM_ENTRY(CPSS_NET_NHLFE_ENTRY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_ILLEGAL_POP_E),
    ENUM_ENTRY(CPSS_NET_INVALID_MPLS_IF_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ICMP_MLD_E),
    ENUM_ENTRY(CPSS_NET_IPV6_IGMP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_PCL_E),
    ENUM_ENTRY(CPSS_NET_IPV6_HOP_BY_HOP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_EXT_HEADER_E),
    ENUM_ENTRY(CPSS_NET_IPV6_BAD_HEADER_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ILLIGAL_DIP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_DIP_ERROR_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ROUTE_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ROUTER_RPF_E),
    ENUM_ENTRY(CPSS_NET_IPV6_SCOP_FAIL_E),
    ENUM_ENTRY(CPSS_NET_IPV6_TTL_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_DEFAULT_ROUTE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_PCL_RST_FIN_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_PCL_PCE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_BRIDGE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_MLD_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IGMP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_RIPV1_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_TTL_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_RPF_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IP_OPTIONS_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_ROUTE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_ICMP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_HOP_BY_HOP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_EXT_HEADER_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_HEADER_ERROR_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_ILLIGAL_IP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_SPECIAL_DIP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IP_SCOP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IP_MTU_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_INLIF_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_PCL_MTU_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_DEFAULT_ROUTE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_MC_ROUTE1_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_MC_ROUTE2_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_MC_ROUTE3_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IPV4_BC_E),
    ENUM_ENTRY(CPSS_NET_ETH_BRIDGED_LLT_E),
    ENUM_ENTRY(CPSS_NET_IPV4_ROUTED_LLT_E),
    ENUM_ENTRY(CPSS_NET_UC_MPLS_LLT_E),
    ENUM_ENTRY(CPSS_NET_MC_MPLS_LLT_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ROUTED_LLT_E),
    ENUM_ENTRY(CPSS_NET_L2CE_LLT_E),
    ENUM_ENTRY(CPSS_NET_EGRESS_MIRROR_TO_CPU_E),
    ENUM_ENTRY(CPSS_NET_UNKNOWN_UC_E),
    ENUM_ENTRY(CPSS_NET_UNREG_MC_E),
    ENUM_ENTRY(CPSS_NET_LOCK_PORT_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ICMP_PACKET_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E),
    ENUM_ENTRY(CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E),
    ENUM_ENTRY(CPSS_NET_IPV4_BROADCAST_PACKET_E),
    ENUM_ENTRY(CPSS_NET_NON_IPV4_BROADCAST_PACKET_E),
    ENUM_ENTRY(CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E),
    ENUM_ENTRY(CPSS_NET_UNREGISTERED_MULTICAST_E),
    ENUM_ENTRY(CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E),
    ENUM_ENTRY(CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E),
    ENUM_ENTRY(CPSS_NET_UNKNOWN_UNICAST_E),
    ENUM_ENTRY(CPSS_NET_UDP_BC_MIRROR_TRAP0_E),
    ENUM_ENTRY(CPSS_NET_UDP_BC_MIRROR_TRAP1_E),
    ENUM_ENTRY(CPSS_NET_UDP_BC_MIRROR_TRAP2_E),
    ENUM_ENTRY(CPSS_NET_UDP_BC_MIRROR_TRAP3_E),
    ENUM_ENTRY(CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E),
    ENUM_ENTRY(CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E),
    ENUM_ENTRY(CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E),
    ENUM_ENTRY(CPSS_NET_BRIDGED_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E),
    ENUM_ENTRY(CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E),
    ENUM_ENTRY(CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E),
    ENUM_ENTRY(CPSS_NET_CPU_TO_CPU_E),
    ENUM_ENTRY(CPSS_NET_EGRESS_SAMPLED_E),
    ENUM_ENTRY(CPSS_NET_INGRESS_SAMPLED_E),
    ENUM_ENTRY(CPSS_NET_INVALID_PCL_KEY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E),
    ENUM_ENTRY(CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E),
    ENUM_ENTRY(CPSS_NET_ROUTED_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_IP_DIP_DA_MISMATCH_E),
    ENUM_ENTRY(CPSS_NET_IP_UC_SIP_SA_MISMATCH_E),
    ENUM_ENTRY(CPSS_NET_IPV4_UC_ROUTE1_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_UC_ROUTE2_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_UC_ROUTE3_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_MC_ROUTE0_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_MC_ROUTE1_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_MC_ROUTE2_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_MC_ROUTE3_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_UC_ROUTE1_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_UC_ROUTE2_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_UC_ROUTE3_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_ROUTE0_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_ROUTE1_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_ROUTE2_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_ROUTE3_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IP_UC_RPF_FAIL_E),
    ENUM_ENTRY(CPSS_NET_ARP_BC_TO_ME_E),
    ENUM_ENTRY(CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E),
    ENUM_ENTRY(CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E),
    ENUM_ENTRY(CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E),
    ENUM_ENTRY(CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_TT_HEADER_ERROR_E),
    ENUM_ENTRY(CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E),
    ENUM_ENTRY(CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E),
    ENUM_ENTRY(CPSS_NET_ARP_REPLY_TO_ME_E),
    ENUM_ENTRY(CPSS_NET_CPU_TO_ALL_CPUS_E),
    ENUM_ENTRY(CPSS_NET_TCP_SYN_TO_CPU_E),
    ENUM_ENTRY(CPSS_NET_MC_BRIDGED_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_MC_ROUTED_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_L2VPN_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_MPLS_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_ARP_BC_EGRESS_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_VPLS_UNREGISTERED_MC_EGRESS_FILTER_E),
    ENUM_ENTRY(CPSS_NET_VPLS_UNKWONW_UC_EGRESS_FILTER_E),
    ENUM_ENTRY(CPSS_NET_VPLS_BC_EGRESS_FILTER_E),
    ENUM_ENTRY(CPSS_NET_MC_CPU_TO_CPU_E),
    ENUM_ENTRY(CPSS_NET_PCL_MIRRORED_TO_ANALYZER_E),
    ENUM_ENTRY(CPSS_NET_VPLS_UNTAGGED_MRU_FILTER_E),
    ENUM_ENTRY(CPSS_NET_TT_MPLS_HEADER_CHECK_E),
    ENUM_ENTRY(CPSS_NET_TT_MPLS_TTL_EXCEED_E),
    ENUM_ENTRY(CPSS_NET_TTI_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_MPLS_SRC_FILTERING_E),
    ENUM_ENTRY(CPSS_NET_IPV4_TTL1_EXCEEDED_E),
    ENUM_ENTRY(CPSS_NET_IPV6_HOPLIMIT1_EXCEED_E),
    ENUM_ENTRY(CPSS_NET_OAM_PDU_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPCL_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_MPLS_TTL1_EXCEEDED_E),
    ENUM_ENTRY(CPSS_NET_MPLS_ROUTE_ENTRY_1_E),
    ENUM_ENTRY(CPSS_NET_MPLS_ROUTE_ENTRY_2_E),
    ENUM_ENTRY(CPSS_NET_MPLS_ROUTE_ENTRY_3_E),
    ENUM_ENTRY(CPSS_NET_PTP_HEADER_ERROR_E),
    ENUM_ENTRY(CPSS_NET_IPV4_6_SIP_FILTERING_E),
    ENUM_ENTRY(CPSS_NET_IPV4_6_SIP_IS_ZERO_E),
    ENUM_ENTRY(CPSS_NET_ACCESS_MATRIX_E),
    ENUM_ENTRY(CPSS_NET_FCOE_DIP_LOOKUP_NOT_FOUND_OR_FCOE_EXCEPTION_E),
    ENUM_ENTRY(CPSS_NET_FCOE_SIP_NOT_FOUND_E),
    ENUM_ENTRY(CPSS_NET_IP_MC_ROUTE_BIDIR_RPF_FAIL_E),
    ENUM_ENTRY(CPSS_NET_MET_RED_DROP_CODE_E),
    ENUM_ENTRY(CPSS_NET_ECC_DROP_CODE_E),
    ENUM_ENTRY(CPSS_NET_BAD_ANALYZER_INDEX_DROP_ERROR_E),
    ENUM_ENTRY(CPSS_NET_RXDMA_DROP_E),
    ENUM_ENTRY(CPSS_NET_TARGET_NULL_PORT_E),
    ENUM_ENTRY(CPSS_NET_TARGET_EPORT_MTU_EXCEEDED_OR_RX_PROTECTION_SWITCHING_OR_INGRESS_FORWARDING_RESTRICTIONS_E),
    ENUM_ENTRY(CPSS_NET_UNTAGGED_OVERSIZE_DROP_CODE_E),
    ENUM_ENTRY(CPSS_NET_IPV6_TT_UNSUP_GRE_ERROR_E),
    ENUM_ENTRY(CPSS_NET_CCFC_PKT_DROP_E),
    ENUM_ENTRY(CPSS_NET_BAD_FC_PKT_DROP_E),
    ENUM_ENTRY(CPSS_NET_FIRST_USER_DEFINED_E),
    ENUM_ENTRY(CPSS_NET_LAST_USER_DEFINED_E),
    ENUM_ENTRY(CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E),
    ENUM_ENTRY(CPSS_NET_LAST_UNKNOWN_HW_CPU_CODE_E),
    ENUM_ENTRY(CPSS_NET_ALL_CPU_OPCODES_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_NET_RX_CPU_CODE_ENT);

enumDescr enumDescr_CPSS_MEMORY_DUMP_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_MEMORY_DUMP_BYTE_E),
    ENUM_ENTRY(CPSS_MEMORY_DUMP_SHORT_E),
    ENUM_ENTRY(CPSS_MEMORY_DUMP_WORD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_MEMORY_DUMP_TYPE_ENT);

void prv_lua_to_c_CPSS_TM_C_NODE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_C_NODE_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, shapingProfilePtr, GT_U32);
    F_NUMBER(val, -1, quantum, GT_U32);
    F_NUMBER(val, -1, dropCosMap, GT_U32);
    F_ARRAY_START(val, -1, dropProfileIndArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, dropProfileIndArr, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, dropProfileIndArr);
    F_ENUM(val, -1, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_NODE_ENT);
    F_ARRAY_START(val, -1, schdModeArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_ENUM(val, schdModeArr, idx, CPSS_TM_SCHD_MODE_ENT);
        }
    }
    F_ARRAY_END(val, -1, schdModeArr);
    F_NUMBER(val, -1, numOfChildren, GT_U32);
}

void prv_c_to_lua_CPSS_TM_C_NODE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_C_NODE_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, shapingProfilePtr, GT_U32);
    FO_NUMBER(val, t, quantum, GT_U32);
    FO_NUMBER(val, t, dropCosMap, GT_U32);
    FO_ARRAY_START(val, t, dropProfileIndArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, dropProfileIndArr, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, dropProfileIndArr);
    FO_ENUM(val, t, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_NODE_ENT);
    FO_ARRAY_START(val, t, schdModeArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_ENUM(val, schdModeArr, idx, CPSS_TM_SCHD_MODE_ENT);
        }
    }
    FO_ARRAY_END(val, t, schdModeArr);
    FO_NUMBER(val, t, numOfChildren, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_C_NODE_PARAMS_STC);

enumDescr enumDescr_CPSS_TM2TM_CHANNEL_ENT[] = {
    ENUM_ENTRY(CPSS_TM2TM_CHANNEL_NODE_E),
    ENUM_ENTRY(CPSS_TM2TM_CHANNEL_PORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM2TM_CHANNEL_ENT);

void prv_lua_to_c_CPSS_MAC_ENTRY_EXT_STC(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_STC *val
)
{
    F_STRUCT(val, -1, key, CPSS_MAC_ENTRY_EXT_KEY_STC);
    F_STRUCT(val, -1, dstInterface, CPSS_INTERFACE_INFO_STC);
    F_BOOL(val, -1, age);
    F_BOOL(val, -1, isStatic);
    F_ENUM(val, -1, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    F_ENUM(val, -1, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    F_BOOL(val, -1, daRoute);
    F_BOOL(val, -1, mirrorToRxAnalyzerPortEn);
    F_NUMBER(val, -1, sourceID, GT_U32);
    F_NUMBER(val, -1, userDefined, GT_U32);
    F_NUMBER(val, -1, daQosIndex, GT_U32);
    F_NUMBER(val, -1, saQosIndex, GT_U32);
    F_NUMBER(val, -1, daSecurityLevel, GT_U32);
    F_NUMBER(val, -1, saSecurityLevel, GT_U32);
    F_BOOL(val, -1, appSpecificCpuCode);
    F_BOOL(val, -1, spUnknown);
    F_BOOL(val, -1, saMirrorToRxAnalyzerPortEn);
    F_BOOL(val, -1, daMirrorToRxAnalyzerPortEn);
    F_STRUCT(val, -1, fdbRoutingInfo, CPSS_FDB_UC_ROUTING_INFO_STC);
}

void prv_c_to_lua_CPSS_MAC_ENTRY_EXT_STC(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, key, CPSS_MAC_ENTRY_EXT_KEY_STC);
    FO_STRUCT(val, t, dstInterface, CPSS_INTERFACE_INFO_STC);
    FO_BOOL(val, t, age);
    FO_BOOL(val, t, isStatic);
    FO_ENUM(val, t, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    FO_ENUM(val, t, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    FO_BOOL(val, t, daRoute);
    FO_BOOL(val, t, mirrorToRxAnalyzerPortEn);
    FO_NUMBER(val, t, sourceID, GT_U32);
    FO_NUMBER(val, t, userDefined, GT_U32);
    FO_NUMBER(val, t, daQosIndex, GT_U32);
    FO_NUMBER(val, t, saQosIndex, GT_U32);
    FO_NUMBER(val, t, daSecurityLevel, GT_U32);
    FO_NUMBER(val, t, saSecurityLevel, GT_U32);
    FO_BOOL(val, t, appSpecificCpuCode);
    FO_BOOL(val, t, spUnknown);
    FO_BOOL(val, t, saMirrorToRxAnalyzerPortEn);
    FO_BOOL(val, t, daMirrorToRxAnalyzerPortEn);
    FO_STRUCT(val, t, fdbRoutingInfo, CPSS_FDB_UC_ROUTING_INFO_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_MAC_ENTRY_EXT_STC);

enumDescr enumDescr_CPSS_COMPARE_OPERATOR_ENT[] = {
    ENUM_ENTRY(CPSS_COMPARE_OPERATOR_INVALID_E),
    ENUM_ENTRY(CPSS_COMPARE_OPERATOR_LTE),
    ENUM_ENTRY(CPSS_COMPARE_OPERATOR_GTE),
    ENUM_ENTRY(CPSS_COMPARE_OPERATOR_NEQ),
    { 0, NULL }
};
add_mgm_enum(CPSS_COMPARE_OPERATOR_ENT);

void prv_lua_to_c_CPSS_COS_STC(
    lua_State *L,
    CPSS_COS_STC *val
)
{
    F_ENUM(val, -1, dropPrecedence, CPSS_DP_LEVEL_ENT);
    F_NUMBER(val, -1, userPriority, GT_U8);
    F_NUMBER(val, -1, trafficClass, GT_U8);
}

void prv_c_to_lua_CPSS_COS_STC(
    lua_State *L,
    CPSS_COS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dropPrecedence, CPSS_DP_LEVEL_ENT);
    FO_NUMBER(val, t, userPriority, GT_U8);
    FO_NUMBER(val, t, trafficClass, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_COS_STC);

void prv_lua_to_c_CPSS_MAC_UPDATE_MSG_EXT_STC(
    lua_State *L,
    CPSS_MAC_UPDATE_MSG_EXT_STC *val
)
{
    F_ENUM(val, -1, updType, CPSS_UPD_MSG_TYPE_ENT);
    F_BOOL(val, -1, entryWasFound);
    F_NUMBER(val, -1, macEntryIndex, GT_U32);
    F_STRUCT(val, -1, macEntry, CPSS_MAC_ENTRY_EXT_STC);
    F_BOOL(val, -1, skip);
    F_BOOL(val, -1, aging);
    F_NUMBER_N(val, -1, associatedHwDevNum, associatedDevNum, GT_HW_DEV_NUM);
    F_NUMBER_N(val, -1, queryHwDevNum, queryDevNum, GT_HW_DEV_NUM);
    F_BOOL(val, -1, naChainIsTooLong);
    F_NUMBER(val, -1, entryOffset, GT_U32);
    F_NUMBER(val, -1, portGroupId, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U16);
    F_NUMBER(val, -1, up0, GT_U32);
    F_BOOL(val, -1, isMoved);
    F_NUMBER(val, -1, oldSrcId, GT_U32);
    F_STRUCT(val, -1, oldDstInterface, CPSS_INTERFACE_INFO_STC);
    F_NUMBER_N(val, -1, oldAssociatedHwDevNum, oldAssociatedDevNum, GT_HW_DEV_NUM);
}

void prv_c_to_lua_CPSS_MAC_UPDATE_MSG_EXT_STC(
    lua_State *L,
    CPSS_MAC_UPDATE_MSG_EXT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, updType, CPSS_UPD_MSG_TYPE_ENT);
    FO_BOOL(val, t, entryWasFound);
    FO_NUMBER(val, t, macEntryIndex, GT_U32);
    FO_STRUCT(val, t, macEntry, CPSS_MAC_ENTRY_EXT_STC);
    FO_BOOL(val, t, skip);
    FO_BOOL(val, t, aging);
    FO_NUMBER_N(val, t, associatedHwDevNum, associatedDevNum, GT_HW_DEV_NUM);
    FO_NUMBER_N(val, t, queryHwDevNum, queryDevNum, GT_HW_DEV_NUM);
    FO_BOOL(val, t, naChainIsTooLong);
    FO_NUMBER(val, t, entryOffset, GT_U32);
    FO_NUMBER(val, t, portGroupId, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U16);
    FO_NUMBER(val, t, up0, GT_U32);
    FO_BOOL(val, t, isMoved);
    FO_NUMBER(val, t, oldSrcId, GT_U32);
    FO_STRUCT(val, t, oldDstInterface, CPSS_INTERFACE_INFO_STC);
    FO_NUMBER_N(val, t, oldAssociatedHwDevNum, oldAssociatedDevNum, GT_HW_DEV_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_MAC_UPDATE_MSG_EXT_STC);

enumDescr enumDescr_CPSS_L4_PROTOCOL_PORT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_L4_PROTOCOL_PORT_SRC_E),
    ENUM_ENTRY(CPSS_L4_PROTOCOL_PORT_DST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_L4_PROTOCOL_PORT_TYPE_ENT);

void prv_lua_to_c_CPSS_TM_QMR_PKT_STATISTICS_STC(
    lua_State *L,
    CPSS_TM_QMR_PKT_STATISTICS_STC *val
)
{
    F_STRUCT(val, -1, numPktsToUnInstallQueue, GT_U64);
}

void prv_c_to_lua_CPSS_TM_QMR_PKT_STATISTICS_STC(
    lua_State *L,
    CPSS_TM_QMR_PKT_STATISTICS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, numPktsToUnInstallQueue, GT_U64);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_QMR_PKT_STATISTICS_STC);

enumDescr enumDescr_CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E),
    ENUM_ENTRY(CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT);

enumDescr enumDescr_CPSS_PORT_EGRESS_CNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_EGRESS_CNT_PORT_E),
    ENUM_ENTRY(CPSS_EGRESS_CNT_VLAN_E),
    ENUM_ENTRY(CPSS_EGRESS_CNT_TC_E),
    ENUM_ENTRY(CPSS_EGRESS_CNT_DP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_EGRESS_CNT_MODE_ENT);

enumDescr enumDescr_CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E),
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E),
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E),
    ENUM_ENTRY(CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);

enumDescr enumDescr_CPSS_SYSTEM_RECOVERY_PROCESS_ENT[] = {
    ENUM_ENTRY(CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E),
    ENUM_ENTRY(CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E),
    ENUM_ENTRY(CPSS_SYSTEM_RECOVERY_PROCESS_HA_E),
    ENUM_ENTRY(CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_SYSTEM_RECOVERY_PROCESS_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_STATUSES_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_STATUSES_STC *val
)
{
    F_STRUCT(val, -1, portSr, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC);
    F_ENUM(val, -1, lastDisconnect, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT);
    F_STRUCT(val, -1, indications, CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_STATUSES_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_STATUSES_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, portSr, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC);
    FO_ENUM(val, t, lastDisconnect, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT);
    FO_STRUCT(val, t, indications, CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_STATUSES_STC);

void prv_lua_to_c_CPSS_TM_LEVEL_PERIODIC_PARAMS_STC(
    lua_State *L,
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC *val
)
{
    F_BOOL(val, -1, periodicState);
    F_BOOL(val, -1, shaperDecoupling);
}

void prv_c_to_lua_CPSS_TM_LEVEL_PERIODIC_PARAMS_STC(
    lua_State *L,
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, periodicState);
    FO_BOOL(val, t, shaperDecoupling);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_LEVEL_PERIODIC_PARAMS_STC);

void prv_lua_to_c_CPSS_FDB_HASH_OPTION_STC(
    lua_State *L,
    CPSS_FDB_HASH_OPTION_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, macLookupMask, GT_ETHERADDR);
    F_NUMBER(val, -1, vidLookupMask, GT_U16);
    F_NUMBER(val, -1, macShiftLeft, GT_U32);
    F_NUMBER(val, -1, vidShiftLeft, GT_U32);
}

void prv_c_to_lua_CPSS_FDB_HASH_OPTION_STC(
    lua_State *L,
    CPSS_FDB_HASH_OPTION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, macLookupMask, GT_ETHERADDR);
    FO_NUMBER(val, t, vidLookupMask, GT_U16);
    FO_NUMBER(val, t, macShiftLeft, GT_U32);
    FO_NUMBER(val, t, vidShiftLeft, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_FDB_HASH_OPTION_STC);

enumDescr enumDescr_CPSS_VCT_POLARITY_SWAP_ENT[] = {
    ENUM_ENTRY(CPSS_VCT_POSITIVE_POLARITY_E),
    ENUM_ENTRY(CPSS_VCT_NEGATIVE_POLARITY_E),
    ENUM_ENTRY(CPSS_VCT_NOT_APPLICABLE_POLARITY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_VCT_POLARITY_SWAP_ENT);

enumDescr enumDescr_CPSS_PORT_MAC_COUNTERS_ENT[] = {
    ENUM_ENTRY(CPSS_GOOD_OCTETS_RCV_E),
    ENUM_ENTRY(CPSS_BAD_OCTETS_RCV_E),
    ENUM_ENTRY(CPSS_MAC_TRANSMIT_ERR_E),
    ENUM_ENTRY(CPSS_GOOD_PKTS_RCV_E),
    ENUM_ENTRY(CPSS_BAD_PKTS_RCV_E),
    ENUM_ENTRY(CPSS_BRDC_PKTS_RCV_E),
    ENUM_ENTRY(CPSS_MC_PKTS_RCV_E),
    ENUM_ENTRY(CPSS_PKTS_64_OCTETS_E),
    ENUM_ENTRY(CPSS_PKTS_65TO127_OCTETS_E),
    ENUM_ENTRY(CPSS_PKTS_128TO255_OCTETS_E),
    ENUM_ENTRY(CPSS_PKTS_256TO511_OCTETS_E),
    ENUM_ENTRY(CPSS_PKTS_512TO1023_OCTETS_E),
    ENUM_ENTRY(CPSS_PKTS_1024TOMAX_OCTETS_E),
    ENUM_ENTRY(CPSS_GOOD_OCTETS_SENT_E),
    ENUM_ENTRY(CPSS_GOOD_PKTS_SENT_E),
    ENUM_ENTRY(CPSS_EXCESSIVE_COLLISIONS_E),
    ENUM_ENTRY(CPSS_MC_PKTS_SENT_E),
    ENUM_ENTRY(CPSS_BRDC_PKTS_SENT_E),
    ENUM_ENTRY(CPSS_UNRECOG_MAC_CNTR_RCV_E),
    ENUM_ENTRY(CPSS_FC_SENT_E),
    ENUM_ENTRY(CPSS_GOOD_FC_RCV_E),
    ENUM_ENTRY(CPSS_DROP_EVENTS_E),
    ENUM_ENTRY(CPSS_UNDERSIZE_PKTS_E),
    ENUM_ENTRY(CPSS_FRAGMENTS_PKTS_E),
    ENUM_ENTRY(CPSS_OVERSIZE_PKTS_E),
    ENUM_ENTRY(CPSS_JABBER_PKTS_E),
    ENUM_ENTRY(CPSS_MAC_RCV_ERROR_E),
    ENUM_ENTRY(CPSS_BAD_CRC_E),
    ENUM_ENTRY(CPSS_COLLISIONS_E),
    ENUM_ENTRY(CPSS_LATE_COLLISIONS_E),
    ENUM_ENTRY(CPSS_BadFC_RCV_E),
    ENUM_ENTRY(CPSS_GOOD_UC_PKTS_RCV_E),
    ENUM_ENTRY(CPSS_GOOD_UC_PKTS_SENT_E),
    ENUM_ENTRY(CPSS_MULTIPLE_PKTS_SENT_E),
    ENUM_ENTRY(CPSS_DEFERRED_PKTS_SENT_E),
    ENUM_ENTRY(CPSS_PKTS_1024TO1518_OCTETS_E),
    ENUM_ENTRY(CPSS_PKTS_1519TOMAX_OCTETS_E),
    ENUM_ENTRY(CPSS_LAST_MAC_COUNTER_NUM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_MAC_COUNTERS_ENT);

enumDescr enumDescr_CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_CFG_HW_DEV_NUM_MODE_SINGLE_E),
    ENUM_ENTRY(CPSS_GEN_CFG_HW_DEV_NUM_MODE_DUAL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);

enumDescr enumDescr_CPSS_FLOW_DRAM_PARAM_ENT[] = {
    ENUM_ENTRY(CPSS_FLOW_DRAM_USE_DEFAULT_DRAM_CONF_E),
    ENUM_ENTRY(CPSS_FLOW_DRAM_SAMSUNG_REV_A_250MHZ_E),
    ENUM_ENTRY(CPSS_FLOW_DRAM_SAMSUNG_REV_E_250MHZ_E),
    ENUM_ENTRY(CPSS_FLOW_DRAM_SAMSUNG_REV_A_166MHZ_E),
    ENUM_ENTRY(CPSS_FLOW_DRAM_SAMSUNG_REV_E_166MHZ_E),
    ENUM_ENTRY(CPSS_FLOW_DRAM_SAMSUNG_REV_A_200MHZ_E),
    ENUM_ENTRY(CPSS_FLOW_DRAM_SAMSUNG_REV_E_200MHZ_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_FLOW_DRAM_PARAM_ENT);

enumDescr enumDescr_CPSS_PP_FAMILY_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_PP_FAMILY_START_EXMX_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_TWISTC_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_TWISTD_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_SAMBA_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_TIGER_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_END_EXMX_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_START_DXSAL_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_SALSA_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_END_DXSAL_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_START_DXCH_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_CHEETAH_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_CHEETAH2_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_CHEETAH3_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_XCAT_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_XCAT3_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_LION_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_XCAT2_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_LION2_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_LION3_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_BOBCAT2_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_BOBCAT3_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_DXCH_ALDRIN_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_END_DXCH_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_START_EXMXPM_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_PUMA_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_PUMA3_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_END_EXMXPM_E),
    ENUM_ENTRY(CPSS_PP_FAMILY_LAST_E),
    ENUM_ENTRY(CPSS_MAX_FAMILY),
    { 0, NULL }
};
add_mgm_enum(CPSS_PP_FAMILY_TYPE_ENT);

enumDescr enumDescr_CPSS_TM_DP_SOURCE_ENT[] = {
    ENUM_ENTRY(CPSS_TM_DP_SOURCE_AQL_E),
    ENUM_ENTRY(CPSS_TM_DP_SOURCE_QL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM_DP_SOURCE_ENT);

enumDescr enumDescr_CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BYTE_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BAD_PKT_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_PKT_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_CRC_ERR_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BYTE_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BAD_PKT_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_PKT_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_BLK_TYPE_ERR_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_DIAG_CRC_ERR_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_WORD_SYNC_ERR_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_ERR_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_FAIL_E),
    ENUM_ENTRY(CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BURST_SIZE_ERR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT);

enumDescr enumDescr_CPSS_PORT_XGMII_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_XGMII_LAN_E),
    ENUM_ENTRY(CPSS_PORT_XGMII_WAN_E),
    ENUM_ENTRY(CPSS_PORT_XGMII_FIXED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_XGMII_MODE_ENT);

enumDescr enumDescr_CPSS_DP_LEVEL_ENT[] = {
    ENUM_ENTRY(CPSS_DP_GREEN_E),
    ENUM_ENTRY(CPSS_DP_YELLOW_E),
    ENUM_ENTRY(CPSS_DP_RED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DP_LEVEL_ENT);

enumDescr enumDescr_CPSS_TM_SCHD_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_TM_SCHD_MODE_RR_E),
    ENUM_ENTRY(CPSS_TM_SCHD_MODE_DWRR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM_SCHD_MODE_ENT);

enumDescr enumDescr_CPSS_BUFFER_DRAM_PARAM_ENT[] = {
    ENUM_ENTRY(CPSS_BUFFER_DRAM_USE_DEFAULT_DRAM_CONF_E),
    ENUM_ENTRY(CPSS_BUFFER_DRAM_SAMSUNG_REV_A_250MHZ_E),
    ENUM_ENTRY(CPSS_BUFFER_DRAM_SAMSUNG_REV_E_250MHZ_E),
    ENUM_ENTRY(CPSS_BUFFER_DRAM_HYNIX_HY5DU283222F_33_250MHZ_E),
    ENUM_ENTRY(CPSS_BUFFER_DRAM_SAMSUNG_REV_A_300MHZ_E),
    ENUM_ENTRY(CPSS_BUFFER_DRAM_SAMSUNG_REV_E_300MHZ_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BUFFER_DRAM_PARAM_ENT);

enumDescr enumDescr_CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E),
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ECC_E),
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT);

void prv_lua_to_c_CPSS_TM_RCB_PKT_STATISTICS_STC(
    lua_State *L,
    CPSS_TM_RCB_PKT_STATISTICS_STC *val
)
{
    F_STRUCT(val, -1, numPktsToSms, GT_U64);
    F_STRUCT(val, -1, numCrcErrPktsToSms, GT_U64);
    F_STRUCT(val, -1, numErrsFromSmsToDram, GT_U64);
    F_STRUCT(val, -1, numPortFlushDrpDataPkts, GT_U64);
}

void prv_c_to_lua_CPSS_TM_RCB_PKT_STATISTICS_STC(
    lua_State *L,
    CPSS_TM_RCB_PKT_STATISTICS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, numPktsToSms, GT_U64);
    FO_STRUCT(val, t, numCrcErrPktsToSms, GT_U64);
    FO_STRUCT(val, t, numErrsFromSmsToDram, GT_U64);
    FO_STRUCT(val, t, numPortFlushDrpDataPkts, GT_U64);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_RCB_PKT_STATISTICS_STC);

enumDescr enumDescr_CPSS_QOS_PORT_TRUST_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_QOS_PORT_NO_TRUST_E),
    ENUM_ENTRY(CPSS_QOS_PORT_TRUST_L2_E),
    ENUM_ENTRY(CPSS_QOS_PORT_TRUST_L3_E),
    ENUM_ENTRY(CPSS_QOS_PORT_TRUST_L2_L3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_QOS_PORT_TRUST_MODE_ENT);

enumDescr enumDescr_CPSS_POLICER_MRU_ENT[] = {
    ENUM_ENTRY(CPSS_POLICER_MRU_1536_E),
    ENUM_ENTRY(CPSS_POLICER_MRU_2K_E),
    ENUM_ENTRY(CPSS_POLICER_MRU_10K_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_POLICER_MRU_ENT);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AF_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AT_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AT4PAIR_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_RESERVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT);

enumDescr enumDescr_CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_1_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_2_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_3_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_4_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_5_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_6_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_7_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_8_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_9_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_10_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_11_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_12_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_13_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_14_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_15_E),
    ENUM_ENTRY(CPSS_PORT_TX_SCHEDULER_PROFILE_16_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT);

enumDescr enumDescr_CPSS_LOG_TIME_FORMAT_ENT[] = {
    ENUM_ENTRY(CPSS_LOG_TIME_FORMAT_DATE_TIME_E),
    ENUM_ENTRY(CPSS_LOG_TIME_FORMAT_SHORT_DATE_TIME_E),
    ENUM_ENTRY(CPSS_LOG_TIME_FORMAT_SHORT_DATE_ISO_TIME_E),
    ENUM_ENTRY(CPSS_LOG_TIME_FORMAT_NO_DATE_TIME_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_LOG_TIME_FORMAT_ENT);

enumDescr enumDescr_CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT[] = {
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_TAB_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_FUNC_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E),
    ENUM_ENTRY(CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT);

enumDescr enumDescr_CPSS_DRV_HW_ACCESS_STAGE_ENT[] = {
    ENUM_ENTRY(CPSS_DRV_HW_ACCESS_STAGE_PRE_E),
    ENUM_ENTRY(CPSS_DRV_HW_ACCESS_STAGE_POST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DRV_HW_ACCESS_STAGE_ENT);

void prv_lua_to_c_CPSS_TM2TM_CNTRL_PKT_STRUCT_STC(
    lua_State *L,
    CPSS_TM2TM_CNTRL_PKT_STRUCT_STC *val
)
{
    F_NUMBER(val, -1, portsNum, GT_U32);
    F_NUMBER(val, -1, nodesNum, GT_U32);
}

void prv_c_to_lua_CPSS_TM2TM_CNTRL_PKT_STRUCT_STC(
    lua_State *L,
    CPSS_TM2TM_CNTRL_PKT_STRUCT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, portsNum, GT_U32);
    FO_NUMBER(val, t, nodesNum, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM2TM_CNTRL_PKT_STRUCT_STC);

void prv_lua_to_c_CPSS_REG_VALUE_INFO_STC(
    lua_State *L,
    CPSS_REG_VALUE_INFO_STC *val
)
{
    F_NUMBER(val, -1, addrType, GT_U32);
    F_NUMBER(val, -1, regMask, GT_U32);
    F_NUMBER(val, -1, regVal, GT_U32);
    F_NUMBER(val, -1, repeatCount, GT_U32);
}

void prv_c_to_lua_CPSS_REG_VALUE_INFO_STC(
    lua_State *L,
    CPSS_REG_VALUE_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, addrType, GT_U32);
    FO_NUMBER(val, t, regMask, GT_U32);
    FO_NUMBER(val, t, regVal, GT_U32);
    FO_NUMBER(val, t, repeatCount, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_REG_VALUE_INFO_STC);

enumDescr enumDescr_CPSS_PORT_XG_FIXED_IPG_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_XG_FIXED_IPG_8_BYTES_E),
    ENUM_ENTRY(CPSS_PORT_XG_FIXED_IPG_12_BYTES_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_XG_FIXED_IPG_ENT);

enumDescr enumDescr_CPSS_TM_DROP_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E),
    ENUM_ENTRY(CPSS_TM_DROP_MODE_CB_TD_CA_TD_E),
    ENUM_ENTRY(CPSS_TM_DROP_MODE_CB_TD_ONLY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TM_DROP_MODE_ENT);

enumDescr enumDescr_CPSS_PORT_TX_DROP_SHAPER_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E),
    ENUM_ENTRY(CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_TX_DROP_SHAPER_MODE_ENT);

void prv_lua_to_c_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC(
    lua_State *L,
    CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC *val
)
{
    F_ENUM(val, -1, dropCntMode, CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT);
    F_NUMBER(val, -1, port, GT_PHYSICAL_PORT_NUM);
    F_NUMBER(val, -1, vlan, GT_U16);
}

void prv_c_to_lua_CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC(
    lua_State *L,
    CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dropCntMode, CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT);
    FO_NUMBER(val, t, port, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, vlan, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC);

enumDescr enumDescr_CPSS_IP_CNT_SET_ENT[] = {
    ENUM_ENTRY(CPSS_IP_CNT_SET0_E),
    ENUM_ENTRY(CPSS_IP_CNT_SET1_E),
    ENUM_ENTRY(CPSS_IP_CNT_SET2_E),
    ENUM_ENTRY(CPSS_IP_CNT_SET3_E),
    ENUM_ENTRY(CPSS_IP_CNT_NO_SET_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_IP_CNT_SET_ENT);

enumDescr enumDescr_CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E),
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E),
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E),
    ENUM_ENTRY(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT);

enumDescr enumDescr_CPSS_MAC_ENTRY_EXT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E),
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E),
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E),
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E),
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E),
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E),
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E),
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E),
    ENUM_ENTRY(CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_MAC_ENTRY_EXT_TYPE_ENT);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC *val
)
{
    F_ENUM(val, -1, status, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT);
    F_ENUM(val, -1, extStatus, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT);
    F_ENUM(val, -1, portClass, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT);
    F_BOOL(val, -1, portAtBehavior);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, status, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT);
    FO_ENUM(val, t, extStatus, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT);
    FO_ENUM(val, t, portClass, CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT);
    FO_BOOL(val, t, portAtBehavior);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_CONFIG_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_CONFIG_STC *val
)
{
    F_BOOL(val, -1, dcDisconnectEn);
    F_BOOL(val, -1, externalSyncDis);
    F_BOOL(val, -1, capDis);
    F_BOOL(val, -1, disPortsOverride);
    F_BOOL(val, -1, rprDisable);
    F_BOOL(val, -1, vmainAtPolicyEn);
    F_BOOL(val, -1, class0EqAf);
    F_BOOL(val, -1, class123EqAf);
    F_BOOL(val, -1, classBypass2ndError);
    F_BOOL(val, -1, classErrorEq0);
    F_BOOL(val, -1, classErrorEq4);
    F_BOOL(val, -1, layer2En);
    F_BOOL(val, -1, portPrioritySetByPD);
    F_BOOL(val, -1, privateLableBit);
    F_BOOL(val, -1, lowPriDiscoForHiPriStartupEn);
    F_NUMBER(val, -1, portMethodLimitMode, GT_U32);
    F_NUMBER(val, -1, portMethodCalcMode, GT_U32);
    F_NUMBER(val, -1, guardBandValue, GT_U32);
    F_BOOL(val, -1, startupHiPrOverride);
    F_NUMBER(val, -1, IcutMaxAt, GT_U32);
    F_NUMBER(val, -1, tempAlarmTh, GT_U32);
    F_NUMBER(val, -1, vmainHighTh, GT_U32);
    F_NUMBER(val, -1, vmainAtLowTh, GT_U32);
    F_NUMBER(val, -1, vmainAfLowTh, GT_U32);
    F_ARRAY_START(val, -1, matrixPort);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            F_ARRAY_STRUCT(val, matrixPort, idx, CPSS_GEN_DRAGONITE_PORT_MATRIX_STC);
        }
    }
    F_ARRAY_END(val, -1, matrixPort);
    F_NUMBER(val, -1, commPollingTime, GT_U32);
    F_NUMBER(val, -1, irqGenTime, GT_U32);
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_CONFIG_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, dcDisconnectEn);
    FO_BOOL(val, t, externalSyncDis);
    FO_BOOL(val, t, capDis);
    FO_BOOL(val, t, disPortsOverride);
    FO_BOOL(val, t, rprDisable);
    FO_BOOL(val, t, vmainAtPolicyEn);
    FO_BOOL(val, t, class0EqAf);
    FO_BOOL(val, t, class123EqAf);
    FO_BOOL(val, t, classBypass2ndError);
    FO_BOOL(val, t, classErrorEq0);
    FO_BOOL(val, t, classErrorEq4);
    FO_BOOL(val, t, layer2En);
    FO_BOOL(val, t, portPrioritySetByPD);
    FO_BOOL(val, t, privateLableBit);
    FO_BOOL(val, t, lowPriDiscoForHiPriStartupEn);
    FO_NUMBER(val, t, portMethodLimitMode, GT_U32);
    FO_NUMBER(val, t, portMethodCalcMode, GT_U32);
    FO_NUMBER(val, t, guardBandValue, GT_U32);
    FO_BOOL(val, t, startupHiPrOverride);
    FO_NUMBER(val, t, IcutMaxAt, GT_U32);
    FO_NUMBER(val, t, tempAlarmTh, GT_U32);
    FO_NUMBER(val, t, vmainHighTh, GT_U32);
    FO_NUMBER(val, t, vmainAtLowTh, GT_U32);
    FO_NUMBER(val, t, vmainAfLowTh, GT_U32);
    FO_ARRAY_START(val, t, matrixPort);
    {
        int idx;
        for (idx = 0; idx < 96; idx++) {
            FO_ARRAY_STRUCT(val, matrixPort, idx, CPSS_GEN_DRAGONITE_PORT_MATRIX_STC);
        }
    }
    FO_ARRAY_END(val, t, matrixPort);
    FO_NUMBER(val, t, commPollingTime, GT_U32);
    FO_NUMBER(val, t, irqGenTime, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_CONFIG_STC);

enumDescr enumDescr_CPSS_IPV6_PREFIX_SCOPE_ENT[] = {
    ENUM_ENTRY(CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E),
    ENUM_ENTRY(CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E),
    ENUM_ENTRY(CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E),
    ENUM_ENTRY(CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_IPV6_PREFIX_SCOPE_ENT);

enumDescr enumDescr_CPSS_SRAM_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_SRAM_TYPE_NARROW_E),
    ENUM_ENTRY(CPSS_SRAM_TYPE_WIDE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_SRAM_TYPE_ENT);

void prv_lua_to_c_CPSS_TM_A_NODE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_A_NODE_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, shapingProfilePtr, GT_U32);
    F_NUMBER(val, -1, quantum, GT_U32);
    F_NUMBER(val, -1, dropProfileInd, GT_U32);
    F_ENUM(val, -1, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_NODE_ENT);
    F_ARRAY_START(val, -1, schdModeArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_ENUM(val, schdModeArr, idx, CPSS_TM_SCHD_MODE_ENT);
        }
    }
    F_ARRAY_END(val, -1, schdModeArr);
    F_NUMBER(val, -1, numOfChildren, GT_U32);
}

void prv_c_to_lua_CPSS_TM_A_NODE_PARAMS_STC(
    lua_State *L,
    CPSS_TM_A_NODE_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, shapingProfilePtr, GT_U32);
    FO_NUMBER(val, t, quantum, GT_U32);
    FO_NUMBER(val, t, dropProfileInd, GT_U32);
    FO_ENUM(val, t, eligiblePrioFuncId, CPSS_TM_ELIG_FUNC_NODE_ENT);
    FO_ARRAY_START(val, t, schdModeArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_ENUM(val, schdModeArr, idx, CPSS_TM_SCHD_MODE_ENT);
        }
    }
    FO_ARRAY_END(val, t, schdModeArr);
    FO_NUMBER(val, t, numOfChildren, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TM_A_NODE_PARAMS_STC);

enumDescr enumDescr_CPSS_TX_SDMA_QUEUE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_TX_SDMA_QUEUE_MODE_NORMAL_E),
    ENUM_ENTRY(CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_TX_SDMA_QUEUE_MODE_ENT);

enumDescr enumDescr_CPSS_DIRECTION_ENT[] = {
    ENUM_ENTRY(CPSS_DIRECTION_INGRESS_E),
    ENUM_ENTRY(CPSS_DIRECTION_EGRESS_E),
    ENUM_ENTRY(CPSS_DIRECTION_BOTH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIRECTION_ENT);

enumDescr enumDescr_CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT[] = {
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_CRITICAL_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_HIGH_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_LOW_E),
    ENUM_ENTRY(CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_RESERVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT);

void prv_lua_to_c_CPSS_FDB_UC_ROUTING_INFO_STC(
    lua_State *L,
    CPSS_FDB_UC_ROUTING_INFO_STC *val
)
{
    F_BOOL(val, -1, ttlHopLimitDecEnable);
    F_BOOL(val, -1, ttlHopLimDecOptionsExtChkByPass);
    F_BOOL(val, -1, ingressMirror);
    F_NUMBER(val, -1, ingressMirrorToAnalyzerIndex, GT_U32);
    F_BOOL(val, -1, qosProfileMarkingEnable);
    F_NUMBER(val, -1, qosProfileIndex, GT_U32);
    F_ENUM(val, -1, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_ENUM(val, -1, modifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    F_ENUM(val, -1, modifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    F_ENUM(val, -1, countSet, CPSS_IP_CNT_SET_ENT);
    F_BOOL(val, -1, trapMirrorArpBcEnable);
    F_NUMBER(val, -1, dipAccessLevel, GT_U32);
    F_BOOL(val, -1, ICMPRedirectEnable);
    F_NUMBER(val, -1, mtuProfileIndex, GT_U32);
    F_BOOL(val, -1, isTunnelStart);
    F_NUMBER(val, -1, nextHopVlanId, GT_U16);
    F_NUMBER(val, -1, nextHopARPPointer, GT_U32);
    F_NUMBER(val, -1, nextHopTunnelPointer, GT_U32);
    F_NUMBER(val, -1, nextHopDataBankNumber, GT_U32);
    F_BOOL(val, -1, scopeCheckingEnable);
    F_ENUM(val, -1, siteId, CPSS_IP_SITE_ID_ENT);
}

void prv_c_to_lua_CPSS_FDB_UC_ROUTING_INFO_STC(
    lua_State *L,
    CPSS_FDB_UC_ROUTING_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, ttlHopLimitDecEnable);
    FO_BOOL(val, t, ttlHopLimDecOptionsExtChkByPass);
    FO_BOOL(val, t, ingressMirror);
    FO_NUMBER(val, t, ingressMirrorToAnalyzerIndex, GT_U32);
    FO_BOOL(val, t, qosProfileMarkingEnable);
    FO_NUMBER(val, t, qosProfileIndex, GT_U32);
    FO_ENUM(val, t, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_ENUM(val, t, modifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    FO_ENUM(val, t, modifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    FO_ENUM(val, t, countSet, CPSS_IP_CNT_SET_ENT);
    FO_BOOL(val, t, trapMirrorArpBcEnable);
    FO_NUMBER(val, t, dipAccessLevel, GT_U32);
    FO_BOOL(val, t, ICMPRedirectEnable);
    FO_NUMBER(val, t, mtuProfileIndex, GT_U32);
    FO_BOOL(val, t, isTunnelStart);
    FO_NUMBER(val, t, nextHopVlanId, GT_U16);
    FO_NUMBER(val, t, nextHopARPPointer, GT_U32);
    FO_NUMBER(val, t, nextHopTunnelPointer, GT_U32);
    FO_NUMBER(val, t, nextHopDataBankNumber, GT_U32);
    FO_BOOL(val, t, scopeCheckingEnable);
    FO_ENUM(val, t, siteId, CPSS_IP_SITE_ID_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_FDB_UC_ROUTING_INFO_STC);

void prv_lua_to_c_CPSS_PROT_CLASS_ENCAP_STC(
    lua_State *L,
    CPSS_PROT_CLASS_ENCAP_STC *val
)
{
    F_BOOL(val, -1, ethV2);
    F_BOOL(val, -1, nonLlcSnap);
    F_BOOL(val, -1, llcSnap);
}

void prv_c_to_lua_CPSS_PROT_CLASS_ENCAP_STC(
    lua_State *L,
    CPSS_PROT_CLASS_ENCAP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, ethV2);
    FO_BOOL(val, t, nonLlcSnap);
    FO_BOOL(val, t, llcSnap);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PROT_CLASS_ENCAP_STC);

void prv_lua_to_c_CPSS_TRUNK_WEIGHTED_MEMBER_STC(
    lua_State *L,
    CPSS_TRUNK_WEIGHTED_MEMBER_STC *val
)
{
    F_STRUCT(val, -1, member, CPSS_TRUNK_MEMBER_STC);
    F_NUMBER(val, -1, weight, GT_U32);
}

void prv_c_to_lua_CPSS_TRUNK_WEIGHTED_MEMBER_STC(
    lua_State *L,
    CPSS_TRUNK_WEIGHTED_MEMBER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, member, CPSS_TRUNK_MEMBER_STC);
    FO_NUMBER(val, t, weight, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_TRUNK_WEIGHTED_MEMBER_STC);

