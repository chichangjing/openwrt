/*************************************************************************
* /local/store/yedidiaf/git/temp_embede_files/tmp_bld_cpssAPI/all_dxCh_lua_c_type_wrappers.c
*
*       WARNING!!! this is a generated file, please don't edit it manually
*       See COMMENTS for command line
*
* DESCRIPTION:
*       A lua type wrapper
*       It implements support for the following types:
*           enum    CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT
*           enum    CPSS_DXCH_HSU_DATA_TYPE_ENT
*           enum    CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT
*           enum    CPSS_DXCH_LED_CPU_OR_PORT27_ENT
*           enum    CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT
*           enum    CPSS_DXCH_TTI_EXCEPTION_ENT
*           enum    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT
*           struct  CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC
*           enum    CPSS_DXCH_TCAM_CLIENT_ENT
*           struct  CPSS_DXCH_SHADOW_PORT_MAP_STC
*           struct  CPSS_DXCH_LED_CLASS_CONF_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC
*           enum    CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT
*           struct  CPSS_DXCH_POLICER_QOS_PARAM_STC
*           enum    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT
*           enum    CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT
*           struct  CPSS_DXCH_TTI_MIM_RULE_STC
*           enum    CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC
*           enum    CPSS_DXCH_PCL_POLICER_ENABLE_ENT
*           enum    CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT
*           struct  CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC
*           enum    CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT
*           enum    CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC
*           enum    CPSS_DXCH_TCAM_RULE_SIZE_ENT
*           enum    CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT
*           enum    CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT
*           enum    CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT
*           enum    CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT
*           enum    CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT
*           enum    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT
*           struct  CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC
*           struct  CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC
*           struct  CPSS_DXCH_PCL_ACTION_OAM_STC
*           enum    CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT
*           enum    CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT
*           enum    CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT
*           struct  CPSS_DXCH_L2_MLL_PAIR_STC
*           enum    CPSS_DXCH_PTP_TS_TAG_MODE_ENT
*           enum    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT
*           enum    CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT
*           struct  CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC
*           enum    CPSS_DIAG_PP_MEM_BIST_TYPE_ENT
*           struct  CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC
*           enum    CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT
*           struct  CPSS_DXCH_PORT_SERDES_CONFIG_STC
*           enum    CPSS_DXCH_TTI_PW_TAG_MODE_ENT
*           struct  CPSS_DXCH_DIAG_DESCRIPTOR_STC
*           enum    CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT
*           enum    CPSS_DXCH_PORT_HOL_FC_ENT
*           enum    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT
*           enum    CPSS_DXCH_PTP_TAI_NUMBER_ENT
*           enum    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT
*           struct  CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC
*           enum    CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT
*           enum    CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT
*           enum    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT
*           enum    CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT
*           enum    CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT
*           enum    CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT
*           struct  CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC
*           enum    CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT
*           struct  CPSS_DXCH_NET_SDMA_TX_PARAMS_STC
*           enum    CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT
*           enum    CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT
*           enum    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT
*           struct  CPSS_DXCH_BRG_VLAN_INFO_STC
*           enum    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT
*           enum    CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT
*           union   CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT
*           enum    CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT
*           enum    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT
*           enum    CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT
*           enum    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT
*           enum    CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT
*           enum    CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT
*           enum    CPSS_DXCH_TTI_OFFSET_TYPE_ENT
*           struct  CPSS_DXCH_POLICER_ENTRY_STC
*           struct  CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC
*           struct  CPSS_DXCH_CNC_COUNTER_STC
*           enum    CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT
*           enum    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT
*           enum    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT
*           enum    CPSS_DXCH_IP_BRG_EXCP_CMD_ENT
*           enum    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT
*           enum    CPSS_DXCH_PTP_TAI_INSTANCE_ENT
*           struct  CPSS_DXCH_IPV4_PREFIX_STC
*           enum    CPSS_DXCH_PCL_PACKET_TYPE_ENT
*           enum    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT
*           enum    CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT
*           enum    CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT
*           enum    CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT
*           enum    CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT
*           enum    CPSS_DXCH_TTI_ACTION_TYPE_ENT
*           struct  CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC
*           enum    CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC
*           enum    CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT
*           struct  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC
*           enum    CPSS_DXCH_ETHERTYPE_TABLE_ENT
*           enum    CPSS_DXCH_IP_URPF_MODE_ENT
*           enum    CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT
*           enum    CPSS_DXCH_TTI_MAC_MODE_ENT
*           enum    CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT
*           enum    CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT
*           enum    CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT
*           enum    CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT
*           enum    CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT
*           struct  CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC
*           enum    CPSS_DXCH_PORT_PFC_ENABLE_ENT
*           enum    CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT
*           enum    CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT
*           enum    CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT
*           enum    CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT
*           struct  CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC
*           enum    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT
*           struct  CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC
*           enum    CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT
*           struct  CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC
*           enum    CPSS_DXCH_OAM_SRV_MP_TYPE_ENT
*           struct  CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC
*           enum    CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT
*           struct  CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC
*           enum    CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT
*           struct  CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC
*           struct  CPSS_PORT_CNM_GENERATION_CONFIG_STC
*           enum    CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT
*           struct  CPSS_DXCH_LED_GROUP_CONF_STC
*           enum    CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT
*           enum    CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT
*           struct  CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC
*           struct  CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC
*           enum    CPSS_PORT_TX_SHARED_DP_MODE_ENT
*           enum    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT
*           enum    CPSS_DXCH_UDB_ERROR_CMD_ENT
*           struct  CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC
*           enum    CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT
*           enum    CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT
*           union   CPSS_DXCH_TUNNEL_START_CONFIG_UNT
*           struct  CPSS_DXCH_IP_MLL_STC
*           enum    CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT
*           struct  CPSS_DXCH_TTI_IPV4_RULE_STC
*           enum    CPSS_DXCH_NET_DSA_TYPE_ENT
*           enum    CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT
*           enum    CPSS_DXCH_TTI_VLAN_COMMAND_ENT
*           enum    CPSS_BRG_STP_STATE_MODE_ENT
*           struct  CPSS_PORT_TX_WRTD_MASK_LSB_STC
*           struct  CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC
*           enum    CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC
*           enum    CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT
*           struct  CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC
*           enum    CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT
*           enum    CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT
*           enum    CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT
*           enum    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT
*           struct  CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC
*           enum    CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT
*           struct  CPSS_DXCH_POLICER_TB_PARAMS_STC
*           enum    CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT
*           enum    CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC
*           struct  CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC
*           enum    CPSS_DXCH_IP_EXCEPTION_TYPE_ENT
*           enum    CPSS_DXCH_TTI_KEY_SIZE_ENT
*           enum    CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT
*           enum    CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT
*           enum    CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT
*           struct  CPSS_DXCH_TCAM_MANAGER_RANGE_STC
*           enum    CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT
*           enum    CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT
*           struct  CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC
*           enum    CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT
*           enum    CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT
*           union   CPSS_DXCH_PCL_RULE_FORMAT_UNT
*           enum    CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT
*           enum    CPSS_DXCH_PORT_MAPPING_TYPE_ENT
*           enum    CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT
*           enum    CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC
*           struct  CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC
*           enum    CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT
*           enum    CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT
*           enum    CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT
*           struct  CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC
*           struct  CPSS_DXCH_TTI_MAC_VLAN_STC
*           enum    CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT
*           struct  CPSS_PORT_CN_PROFILE_CONFIG_STC
*           enum    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT
*           enum    CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT
*           struct  CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC
*           struct  CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC
*           enum    CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
*           struct  CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC
*           struct  CPSS_DXCH_OAM_SRV_1731_CONFIG_STC
*           struct  CPSS_DXCH_BRIDGE_HOST_CNTR_STC
*           enum    CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT
*           enum    CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT
*           struct  CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC
*           enum    CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
*           struct  CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC
*           enum    CPSS_DXCH_PORT_FEC_MODE_ENT
*           struct  CPSS_DXCH_TTI_TRILL_RBID_TABLE_ENTRY_STC
*           struct  CPSS_DXCH_IP_ECMP_ENTRY_STC
*           enum    CPSS_DXCH_PORT_PIP_PROTOCOL_ENT
*           enum    CPSS_DXCH_FDB_QUEUE_TYPE_ENT
*           enum    CPSS_DXCH_MEMBER_SELECTION_MODE_ENT
*           enum    CPSS_DXCH_TTI_MPLS_CMD_ENT
*           enum    CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT
*           enum    CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT
*           enum    CPSS_DXCH_IP_HEADER_ERROR_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC
*           struct  CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC
*           enum    CPSS_DXCH_LED_INDICATION_ENT
*           enum    CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT
*           struct  CPSS_DXCH_TM_GLUE_DROP_MASK_STC
*           struct  CPSS_DXCH_PCL_UDB_SELECT_STC
*           struct  CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC
*           struct  CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC
*           enum    CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC
*           enum    CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT
*           enum    CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT
*           enum    CPSS_DXCH_PTP_TS_ACTION_ENT
*           enum    CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*           enum    CPSS_DXCH_PORT_EL_DB_OPERATION_ENT
*           enum    CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT
*           struct  CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC
*           enum    CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT
*           struct  CPSS_DIAG_PP_MEM_BIST_COMPARE_MEM_DATA_STC
*           struct  CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC
*           enum    CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT
*           struct  CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC
*           enum    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT
*           struct  CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC
*           enum    CPSS_DXCH_CNC_CLIENT_ENT
*           enum    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT
*           union   CPSS_DXCH_TTI_ACTION_UNT
*           struct  CPSS_DXCH_PORT_ECN_ENABLERS_STC
*           struct  CPSS_DXCH_PORT_ALIGN90_PARAMS_STC
*           enum    CPSS_DXCH_PORT_COM_PHY_H_SUB_SEQ_ENT
*           enum    CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT
*           enum    CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT
*           enum    CPSS_DXCH_TRUNK_LBH_MASK_ENT
*           enum    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT
*           enum    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC
*           enum    CPSS_DXCH_POLICER_MODIFY_UP_ENT
*           enum    CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT
*           enum    CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT
*           enum    CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT
*           enum    CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT
*           struct  CPSS_DXCH_PORT_AP_STATS_STC
*           struct  CPSS_DXCH_IP_NAT44_ENTRY_STC
*           struct  CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC
*           struct  CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC
*           struct  CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC
*           enum    CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT
*           enum    CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT
*           struct  CPSS_DXCH_PORT_SERDES_TUNE_STC
*           enum    CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT
*           struct  CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC
*           struct  CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC
*           struct  CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC
*           struct  CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC
*           enum    CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT
*           enum    CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC
*           enum    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT
*           enum    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT
*           struct  CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC
*           enum    CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT
*           struct  CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC
*           struct  CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC
*           struct  CPSS_DXCH_OAM_SRV_LB_CONFIG_STC
*           enum    CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT
*           struct  CPSS_DXCH_POLICER_COUNTERS_STC
*           enum    CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT
*           enum    CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT
*           enum    CPSS_DXCH_POLICER_MODIFY_DSCP_ENT
*           enum    CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT
*           enum    CPSS_DXCH_IP_CNT_SET_MODE_ENT
*           enum    CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT
*           enum    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT
*           enum    CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT
*           enum    CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT
*           union   CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT
*           enum    CPSS_DXCH_FDB_LEARN_PRIORITY_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC
*           enum    CPSS_DXCH_OAM_OPCODE_TYPE_ENT
*           struct  CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC
*           enum    CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT
*           struct  CPSS_DXCH_OAM_ENTRY_STC
*           enum    CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT
*           enum    CPSS_DXCH3_POLICER_METER_MODE_ENT
*           enum    CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT
*           enum    CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT
*           enum    CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT
*           enum    CPSS_DXCH_TTI_MODIFY_UP_ENT
*           struct  CPSS_DXCH_PTP_TOD_COUNT_STC
*           enum    CPSS_DXCH_MIN_SPEED_ENT
*           enum    CPSS_DXCH_TTI_MODIFY_DSCP_ENT
*           struct  CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC
*           enum    CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT
*           enum    CPSS_DXCH_PORT_BUFFERS_MODE_ENT
*           enum    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_ENT
*           enum    CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT
*           enum    CPSS_DXCH_NET_DSA_CMD_ENT
*           enum    CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT
*           enum    CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT
*           enum    CPSS_DXCH_TTI_PASSENGER_TYPE_ENT
*           enum    CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT
*           struct  CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC
*           struct  CPSS_DXCH_PORT_AP_STATUS_STC
*           struct  CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC
*           enum    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT
*           enum    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT
*           enum    CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT
*           struct  CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC
*           struct  CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC
*           enum    CPSS_DXCH_LED_PORT_TYPE_ENT
*           enum    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT
*           enum    CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT
*           struct  CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC
*           enum    CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT
*           enum    CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT
*           struct  CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
*           enum    CPSS_DXCH_POLICER_STAGE_TYPE_ENT
*           struct  CPSS_DXCH_PTP_TS_CFG_ENTRY_STC
*           enum    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC
*           struct  CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC
*           enum    CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT
*           enum    CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT
*           enum    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT
*           enum    CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT
*           enum    CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT
*           enum    CPSS_DXCH_CFG_ROUTING_MODE_ENT
*           struct  CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC
*           enum    CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT
*           struct  CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC
*           enum    CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC
*           struct  CPSS_DXCH_IPV6_PREFIX_STC
*           struct  CPSS_DXCH_PTP_TAI_ID_STC
*           struct  CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC
*           enum    CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT
*           enum    CPSS_DXCH_POLICER_STORM_TYPE_ENT
*           struct  CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC
*           enum    CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT
*           struct  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC
*           enum    CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT
*           enum    CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT
*           struct  CPSS_DXCH_L2_MLL_ENTRY_STC
*           struct  CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC
*           struct  CPSS_DXCH_CFG_GLOBAL_EPORT_STC
*           enum    CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT
*           struct  CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC
*           enum    CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT
*           enum    CPSS_DXCH_IP_BRG_SERVICE_ENT
*           enum    CPSS_DXCH_TTI_KEY_TYPE_ENT
*           enum    CPSS_DXCH_IP_VLAN_CNT_MODE_ENT
*           struct  CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC
*           enum    CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT
*           enum    CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT
*           struct  CPSS_DXCH_TTI_ETH_RULE_STC
*           struct  CPSS_DXCH_PTP_TAI_TOD_STEP_STC
*           enum    CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT
*           enum    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT
*           enum    CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT
*           enum    CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT
*           struct  CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC
*           enum    CPSS_DXCH_STC_TYPE_ENT
*           enum    CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT
*           enum    CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT
*           enum    CPSS_DXCH_PROTECTION_LOC_STATUS_ENT
*           struct  CPSS_DXCH_PTP_TSU_CONTROL_STC
*           enum    CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT
*           enum    CPSS_DXCH_PORT_GROUP_ENT
*           enum    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT
*           enum    CPSS_DXCH_PORT_FC_MODE_ENT
*           struct  CPSS_DXCH_TTI_RULE_COMMON_STC
*           struct  CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC
*           struct  CPSS_DXCH_TTI_TRILL_ADJACENCY_STC
*           struct  CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC
*           enum    CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC
*           struct  CPSS_DXCH_IPFIX_ENTRY_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC
*           struct  CPSS_DXCH_OAM_SRV_DM_CONFIG_STC
*           enum    CPSS_DXCH_CFG_TABLES_ENT
*           enum    CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT
*           enum    CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT
*           struct  CPSS_DXCH_TCAM_BLOCK_INFO_STC
*           enum    CPSS_IP_NAT_TYPE_ENT
*           enum    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT
*           enum    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT
*           struct  CPSS_DXCH_CFG_DEV_INFO_STC
*           enum    CPSS_DXCH_PORT_SERDES_SPEED_ENT
*           struct  CPSS_DXCH_TTI_TRILL_RULE_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC
*           enum    CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT
*           enum    CPSS_DXCH_OAM_STAGE_TYPE_ENT
*           enum    CPSS_DXCH_IMPLEMENT_WA_ENT
*           enum    CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT
*           struct  CPSS_DXCH_POLICER_BUCKETS_STATE_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC
*           struct  CPSS_DXCH_COS_PROFILE_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC
*           struct  CPSS_DXCH_TTI_ACTION_STC
*           struct  CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC
*           struct  CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC
*           enum    CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT
*           struct  CPSS_DXCH_L2_MLL_LTT_ENTRY_STC
*           enum    CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT
*           enum    CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT
*           enum    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT
*           enum    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC
*           enum    CPSS_DXCH_POLICER_COUNTING_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
*           enum    CPSS_DXCH_TTI_RULE_TYPE_ENT
*           struct  CPSS_DXCH_PORT_MAP_STC
*           enum    CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT
*           struct  CPSS_DXCH_PORT_EEE_LPI_STATUS_STC
*           enum    CPSS_DXCH_PCL_RULE_OPTION_ENT
*           struct  CPSS_DXCH_OAM_SRV_LM_CONFIG_STC
*           struct  CPSS_DXCH_CUT_THROUGH_BYPASS_STC
*           struct  CPSS_DXCH_IP_MLL_PAIR_STC
*           struct  CPSS_DXCH_PCL_ACTION_MIRROR_STC
*           struct  CPSS_DXCH_PCL_ACTION_POLICER_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC
*           enum    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT
*           enum    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT
*           enum    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT
*           struct  CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC
*           struct  CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC
*           enum    CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT
*           enum    CPSS_DXCH_STC_COUNT_MODE_ENT
*           struct  CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC
*           enum    CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT
*           struct  CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC
*           enum    CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT
*           enum    CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT
*           enum    CPSS_DXCH_BRG_FDB_DEL_MODE_ENT
*           struct  CPSS_DXCH_TTI_MPLS_RULE_STC
*           struct  CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC
*           struct  CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC
*           enum    CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT
*           enum    CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT
*           struct  CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC
*           enum    CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT
*           struct  CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC
*           struct  CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC
*           struct  CPSS_DXCH_FABRIC_HGL_RX_DSA_STC
*           enum    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT
*           struct  CPSS_DXCH_VIRTUAL_TCAM_INFO_STC
*           struct  CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC
*           struct  CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC
*           struct  CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC
*           struct  CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC
*           struct  CPSS_DXCH_DETAILED_PORT_MAP_STC
*           struct  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
*           struct  CPSS_DXCH_TTI_ACTION_2_STC
*           enum    CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT
*           enum    CPSS_DXCH_PORT_SERDES_MODE_ENT
*           struct  CPSS_DXCH_NET_RX_PARAMS_STC
*           enum    CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT
*           enum    CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT
*           struct  CPSS_DXCH_LPM_RAM_CONFIG_STC
*           struct  CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC
*           struct  CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC
*           struct  CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC
*           struct  CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC
*           enum    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT
*           struct  CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC
*           enum    CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT
*           enum    CPSS_DXCH_TUNNEL_QOS_MODE_ENT
*           struct  CPSS_DXCH_PORT_AP_INTROP_STC
*           enum    CPSS_DXCH_IP_CPU_CODE_INDEX_ENT
*           struct  CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC
*           union   CPSS_DXCH_IP_NAT_ENTRY_UNT
*           enum    CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_ENT
*           enum    CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT
*           struct  CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC
*           struct  CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC
*           enum    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT
*           struct  CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC
*           struct  CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC
*           struct  CPSS_DXCH_PORT_MAC_PARAMS_STC
*           enum    CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT
*           struct  CPSS_DXCH_PORT_AP_PARAMS_STC
*           struct  CPSS_DXCH_IP_COUNTER_SET_STC
*           enum    CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT
*           enum    CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT
*           struct  CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC
*           struct  CPSS_DXCH_NET_DSA_COMMON_STC
*           enum    CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT
*           enum    CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT
*           struct  CPSS_DXCH3_POLICER_BILLING_ENTRY_STC
*           enum    CPSS_DIAG_PG_IF_SIZE_ENT
*           enum    CPSS_DXCH_POLICER_CMD_ENT
*           enum    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT
*           enum    CPSS_DXCH_IP_DROP_CNT_MODE_ENT
*           enum    CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT
*           struct  CPSS_DXCH_IPFIX_TIMER_STC
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC
*           struct  CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC
*           enum    CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT
*           struct  CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC
*           enum    CPSS_DXCH_PA_UNIT_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC
*           struct  CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC
*           struct  CPSS_DXCH_PORT_COMBO_PARAMS_STC
*           enum    CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT
*           enum    CPSS_DXCH_PORT_UNITS_ID_ENT
*           enum    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT
*           enum    CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT
*           struct  CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC
*           enum    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT
*           enum    CPSS_DXCH_DIAG_BIST_STATUS_ENT
*           enum    CPSS_DXCH_CNC_TM_INDEX_MODE_ENT
*           enum    CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT
*           enum    CPSS_DIAG_PG_TRANSMIT_MODE_ENT
*           struct  CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC
*           struct  CPSS_DXCH_DIAG_BIST_RESULT_STC
*           struct  CPSS_DXCH_TM_GLUE_DRAM_CFG_STC
*           enum    CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT
*           union   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT
*           struct  CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC
*           union   CPSS_DXCH_TTI_RULE_UNT
*           struct  CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC
*           struct  CPSS_DXCH_TUNNEL_TERM_ACTION_STC
*           enum    CPSS_DXCH_POLICER_REMARK_MODE_ENT
*           struct  CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC
*           struct  CPSS_DXCH_PP_CONFIG_INIT_STC
*           struct  CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC
*           struct  CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC
*           enum    CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT
*           enum    CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT
*           enum    CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT
*           struct  CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC
*           enum    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT
*           enum    CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT
*           struct  CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC
*           struct  CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC
*           struct  CPSS_DXCH_IP_NAT66_ENTRY_STC
*           struct  CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC
*           struct  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC
*
* DEPENDENCIES:
*
* COMMENTS:
*       Generated at Sun Jan 31 21:42:00 2016
*       /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss/mainLuaWrapper/scripts/make_type_wrapper.py -i /local/store/yedidiaf/git/temp_embede_files/tmp_bld_cpssAPI/type_info_file_dx -p /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss/mainLuaWrapper/scripts/already_implemented.lst -X /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss/mainLuaWrapper/scripts/excludelist -C /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss -M /local/store/yedidiaf/git/src/linux_cpss.super/cpss.super/cpss/mainLuaWrapper/scripts/lua_C_mapping -N -T -F dxCh -o /local/store/yedidiaf/git/temp_embede_files/tmp_bld_cpssAPI/all_dxCh_lua_c_type_wrappers.c
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
**************************************************************************/
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpmTypes.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/hsu/cpssDxChHsu.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnelTypes.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTtiTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueQueueMap.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/l2mll/cpssDxChL2Mll.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/cpssDxChLogicalTargetMapping.h>
#include <cpss/dxCh/dxChxGen/ipfix/cpssDxChIpfix.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueAgingAndDelay.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOamSrv.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChStc.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDescriptor.h>
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOam.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPip.h>
#include <cpss/generic/diag/cpssDiag.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpNat.h>
#include <cpss/dxCh/dxChxGen/fabric/cpssDxChFabricHGLink.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>
#include <cpss/dxCh/dxChxGen/cutThrough/cpssDxChCutThrough.h>
#include <cpss/dxCh/dxChxGen/policer/cpssDxChPolicer.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgCount.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortEee.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpm.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCn.h>
#include <cpss/extServices/os/gtOs/gtOs.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortSyncEther.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManagerTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h>
#include <cpss/dxCh/dxChxGen/multiPortGroup/cpssDxChMultiPortGroup.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/generic/tunnel/cpssGenTunnelTypes.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagPacketGenerator.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueDram.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgL2Ecmp.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGluePfc.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiterProfile.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueDrop.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortEcn.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortAp.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPAUnitBW.h>
#include <cpss/generic/policer/cpssGenPolicerTypes.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#include <cpss/dxCh/dxChxGen/protection/cpssDxChProtection.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCombo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgE2Phy.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmiPreInit.h>

/***** declarations ********/

use_prv_enum(CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT);
use_prv_struct(CPSS_DXCH_LPM_LEAF_ENTRY_STC);
use_prv_enum(CPSS_DXCH_HSU_DATA_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT);
use_prv_struct(CPSS_INTERFACE_INFO_STC);
use_prv_enum(CPSS_DXCH_LED_CPU_OR_PORT27_ENT);
use_prv_enum(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT);
use_prv_enum(CPSS_DXCH_TTI_EXCEPTION_ENT);
use_prv_enum(CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT);
use_prv_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT);
use_prv_struct(CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC);
use_prv_enum(CPSS_DXCH_TCAM_CLIENT_ENT);
use_prv_struct(CPSS_DXCH_SHADOW_PORT_MAP_STC);
use_prv_struct(CPSS_DXCH_LED_CLASS_CONF_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC);
use_prv_enum(CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT);
use_prv_enum(CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT);
use_prv_struct(CPSS_DXCH_POLICER_QOS_PARAM_STC);
use_prv_enum(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT);
use_prv_enum(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT);
use_prv_struct(CPSS_DXCH_TTI_MIM_RULE_STC);
use_prv_enum(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC);
use_prv_enum(CPSS_DXCH_PCL_POLICER_ENABLE_ENT);
use_prv_enum(CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT);
use_prv_struct(CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC);
use_prv_enum(CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC);
use_prv_enum(CPSS_DXCH_TCAM_RULE_SIZE_ENT);
use_prv_enum(CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT);
use_prv_enum(CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT);
use_prv_enum(CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT);
use_prv_enum(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT);
use_prv_enum(CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT);
use_prv_struct(CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC);
use_prv_struct(CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC);
use_prv_struct(CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC);
use_prv_struct(CPSS_DXCH_PCL_ACTION_OAM_STC);
use_prv_enum(CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT);
use_prv_enum(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT);
use_prv_enum(CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
use_prv_struct(CPSS_DXCH_L2_MLL_PAIR_STC);
use_prv_enum(CPSS_DXCH_PTP_TS_TAG_MODE_ENT);
use_prv_struct(CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC);
use_prv_enum(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT);
use_prv_struct(CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC);
use_prv_enum(CPSS_DIAG_PP_MEM_BIST_TYPE_ENT);
use_prv_struct(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC);
use_prv_enum(CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT);
use_prv_struct(CPSS_DXCH_PORT_SERDES_CONFIG_STC);
use_prv_enum(CPSS_DXCH_TTI_PW_TAG_MODE_ENT);
use_prv_struct(CPSS_DXCH_DIAG_DESCRIPTOR_STC);
use_prv_enum(CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PORT_HOL_FC_ENT);
use_prv_enum(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT);
use_prv_enum(CPSS_DXCH_PTP_TAI_NUMBER_ENT);
use_prv_struct(CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC);
use_prv_struct(CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC);
use_prv_struct(CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC);
use_prv_enum(CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT);
use_prv_enum(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT);
use_prv_struct(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC);
use_prv_enum(CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT);
use_prv_enum(CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT);
use_prv_struct(CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC);
use_prv_enum(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT);
use_prv_enum(CPSS_PORT_SPEED_ENT);
use_prv_struct(CPSS_DXCH_NET_SDMA_TX_PARAMS_STC);
use_prv_enum(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT);
use_prv_enum(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT);
use_prv_enum(CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT);
use_prv_struct(CPSS_DXCH_BRG_VLAN_INFO_STC);
use_prv_enum(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT);
use_prv_enum(CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT);
use_prv_enum(CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT);
use_prv_enum(CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT);
use_prv_enum(CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT);
use_prv_enum(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT);
use_prv_enum(CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT);
use_prv_enum(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT);
use_prv_enum(CPSS_DXCH_TTI_OFFSET_TYPE_ENT);
use_prv_struct(CPSS_DXCH_POLICER_ENTRY_STC);
use_prv_struct(CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC);
use_prv_struct(CPSS_DXCH_CNC_COUNTER_STC);
use_prv_enum(CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
use_prv_enum(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT);
use_prv_enum(CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT);
use_prv_enum(CPSS_DXCH_CNC_COUNTER_FORMAT_ENT);
use_prv_enum(CPSS_DXCH_IP_BRG_EXCP_CMD_ENT);
use_prv_enum(CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT);
use_prv_enum(CPSS_DXCH_PTP_TAI_INSTANCE_ENT);
use_prv_struct(GT_ETHERADDR);
use_prv_struct(CPSS_DXCH_IPV4_PREFIX_STC);
use_prv_enum(CPSS_DXCH_PCL_PACKET_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT);
use_prv_enum(CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
use_prv_struct(CPSS_GEN_CFG_DEV_INFO_STC);
use_prv_enum(CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT);
use_prv_enum(CPSS_NET_RX_CPU_CODE_ENT);
use_prv_enum(CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT);
use_prv_enum(CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT);
use_prv_enum(CPSS_DXCH_TTI_ACTION_TYPE_ENT);
use_prv_struct(CPSS_PORTS_BMP_STC);
use_prv_struct(CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC);
use_prv_enum(CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC);
use_prv_enum(CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT);
use_prv_struct(CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC);
use_prv_enum(CPSS_DXCH_ETHERTYPE_TABLE_ENT);
use_prv_enum(CPSS_DXCH_IP_URPF_MODE_ENT);
use_prv_enum(CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT);
use_prv_struct(CPSS_DXCH_NET_DSA_TO_ANALYZER_STC);
use_prv_enum(CPSS_DXCH_TTI_MAC_MODE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT);
use_prv_enum(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT);
use_prv_enum(CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT);
use_prv_struct(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC);
use_prv_enum(CPSS_DXCH_PORT_PFC_ENABLE_ENT);
use_prv_enum(CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT);
use_prv_enum(CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC);
use_prv_enum(CPSS_DRAM_SIZE_ENT);
use_prv_struct(CPSS_DXCH_IPFIX_ENTRY_STC);
use_prv_enum(CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT);
use_prv_struct(CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC);
use_prv_enum(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
use_prv_struct(CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC);
use_prv_enum(CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT);
use_prv_struct(CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC);
use_prv_enum(CPSS_DXCH_OAM_SRV_MP_TYPE_ENT);
use_prv_struct(CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC);
use_prv_enum(CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT);
use_prv_enum(CPSS_DXCH_LED_INDICATION_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT);
use_prv_struct(CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC);
use_prv_struct(CPSS_PORT_CNM_GENERATION_CONFIG_STC);
use_prv_enum(CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT);
use_prv_struct(GT_U64);
use_prv_struct(CPSS_DXCH_LED_GROUP_CONF_STC);
use_prv_enum(CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT);
use_prv_enum(CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT);
use_prv_struct(CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC);
use_prv_struct(CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC);
use_prv_enum(CPSS_PORT_TX_SHARED_DP_MODE_ENT);
use_prv_enum(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT);
use_prv_enum(CPSS_DXCH_UDB_ERROR_CMD_ENT);
use_prv_enum(CPSS_DROP_MODE_TYPE_ENT);
use_prv_struct(CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC);
use_prv_enum(CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT);
use_prv_struct(GT_IPV6ADDR);
use_prv_struct(CPSS_DXCH_IP_MLL_STC);
use_prv_enum(CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT);
use_prv_struct(CPSS_DXCH_TTI_IPV4_RULE_STC);
use_prv_enum(CPSS_DXCH_NET_DSA_TYPE_ENT);
use_prv_enum(CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT);
use_prv_enum(CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
use_prv_enum(CPSS_BRG_STP_STATE_MODE_ENT);
use_prv_struct(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC);
use_prv_enum(CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC);
use_prv_enum(CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT);
use_prv_struct(CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC);
use_prv_enum(CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT);
use_prv_enum(CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT);
use_prv_enum(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT);
use_prv_struct(CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC);
use_prv_enum(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT);
use_prv_struct(CPSS_DXCH_POLICER_TB_PARAMS_STC);
use_prv_enum(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
use_prv_enum(CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC);
use_prv_struct(CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC);
use_prv_enum(CPSS_DXCH_IP_EXCEPTION_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TTI_KEY_SIZE_ENT);
use_prv_enum(CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT);
use_prv_enum(CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT);
use_prv_enum(CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);
use_prv_struct(CPSS_DXCH_TCAM_MANAGER_RANGE_STC);
use_prv_struct(CPSS_DXCH_PORT_EOM_MATRIX_STC);
use_prv_enum(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT);
use_prv_enum(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT);
use_prv_struct(CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC);
use_prv_enum(CPSS_TUNNEL_PASSENGER_PACKET_ENT);
use_prv_enum(CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT);
use_prv_enum(CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT);
use_prv_enum(CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT);
use_prv_enum(CPSS_DXCH_PORT_MAPPING_TYPE_ENT);
use_prv_enum(CPSS_PORT_DUPLEX_ENT);
use_prv_enum(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT);
use_prv_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC);
use_prv_struct(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC);
use_prv_enum(CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT);
use_prv_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT);
use_prv_struct(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC);
use_prv_struct(CPSS_DXCH_TTI_MAC_VLAN_STC);
use_prv_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT);
use_prv_struct(CPSS_PORT_CN_PROFILE_CONFIG_STC);
use_prv_enum(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
use_prv_enum(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT);
use_prv_struct(CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC);
use_prv_struct(CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
use_prv_enum(CPSS_DRAM_SPEED_BIN_ENT);
use_prv_enum(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC);
use_prv_struct(CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC);
use_prv_struct(CPSS_DXCH_OAM_SRV_1731_CONFIG_STC);
use_prv_struct(CPSS_DXCH_BRIDGE_HOST_CNTR_STC);
use_prv_enum(CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT);
use_prv_enum(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT);
use_prv_struct(CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC);
use_prv_struct(CPSS_DXCH_NET_DSA_PARAMS_STC);
use_prv_enum(CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC);
use_prv_struct(CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC);
use_prv_struct(CPSS_PORT_MODE_SPEED_STC);
use_prv_enum(CPSS_DXCH_PORT_FEC_MODE_ENT);
use_prv_struct(CPSS_DXCH_TTI_TRILL_RBID_TABLE_ENTRY_STC);
use_prv_struct(CPSS_DXCH_IP_ECMP_ENTRY_STC);
use_prv_enum(CPSS_DXCH_PORT_PIP_PROTOCOL_ENT);
use_prv_enum(CPSS_DXCH_FDB_QUEUE_TYPE_ENT);
use_prv_enum(CPSS_DXCH_MEMBER_SELECTION_MODE_ENT);
use_prv_enum(CPSS_MAC_VL_ENT);
use_prv_enum(CPSS_DXCH_TTI_MPLS_CMD_ENT);
use_prv_enum(CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT);
use_prv_enum(CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT);
use_prv_struct(CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC);
use_prv_enum(CPSS_DXCH_IP_HEADER_ERROR_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
use_prv_struct(CPSS_DXCH_PCL_LOOKUP_CFG_STC);
use_prv_struct(CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC);
use_prv_struct(CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC);
use_prv_enum(CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT);
use_prv_struct(CPSS_DXCH_TM_GLUE_DROP_MASK_STC);
use_prv_struct(CPSS_DXCH_PCL_UDB_SELECT_STC);
use_prv_struct(CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC);
use_prv_struct(CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC);
use_prv_enum(CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT);
use_prv_enum(CPSS_DRAM_FREQUENCY_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC);
use_prv_enum(CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT);
use_prv_enum(CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PTP_TS_ACTION_ENT);
use_prv_enum(CPSS_DXCH_PCL_OFFSET_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PORT_EL_DB_OPERATION_ENT);
use_prv_enum(CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT);
use_prv_struct(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC);
use_prv_enum(CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT);
use_prv_struct(CPSS_DIAG_PP_MEM_BIST_COMPARE_MEM_DATA_STC);
use_prv_struct(CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC);
use_prv_enum(CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT);
use_prv_struct(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC);
use_prv_enum(CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT);
use_prv_struct(CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC);
use_prv_enum(CPSS_DXCH_CNC_CLIENT_ENT);
use_prv_enum(CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT);
use_prv_struct(CPSS_DXCH_NET_DSA_FROM_CPU_STC);
use_prv_struct(CPSS_DXCH_PORT_ECN_ENABLERS_STC);
use_prv_struct(CPSS_DXCH_PORT_ALIGN90_PARAMS_STC);
use_prv_enum(CPSS_DXCH_PORT_COM_PHY_H_SUB_SEQ_ENT);
use_prv_enum(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT);
use_prv_struct(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC);
use_prv_enum(CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT);
use_prv_enum(CPSS_DXCH_TRUNK_LBH_MASK_ENT);
use_prv_enum(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC);
use_prv_enum(CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT);
use_prv_enum(CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT);
use_prv_enum(CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT);
use_prv_enum(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT);
use_prv_enum(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT);
use_prv_struct(CPSS_DXCH_PORT_AP_STATS_STC);
use_prv_struct(CPSS_DXCH_IP_NAT44_ENTRY_STC);
use_prv_struct(CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC);
use_prv_struct(CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC);
use_prv_struct(CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC);
use_prv_struct(CPSS_DXCH_PORT_EEE_LPI_STATUS_STC);
use_prv_enum(CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT);
use_prv_struct(CPSS_DXCH_PORT_SERDES_TUNE_STC);
use_prv_enum(CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT);
use_prv_struct(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);
use_prv_struct(CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC);
use_prv_struct(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC);
use_prv_struct(CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC);
use_prv_enum(CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT);
use_prv_enum(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC);
use_prv_enum(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT);
use_prv_enum(CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT);
use_prv_enum(CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT);
use_prv_struct(CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC);
use_prv_enum(CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT);
use_prv_struct(CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC);
use_prv_struct(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC);
use_prv_struct(CPSS_DXCH_OAM_SRV_LB_CONFIG_STC);
use_prv_enum(CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT);
use_prv_struct(CPSS_DXCH_POLICER_COUNTERS_STC);
use_prv_enum(CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT);
use_prv_enum(CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
use_prv_enum(CPSS_DXCH_POLICER_MODIFY_DSCP_ENT);
use_prv_enum(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT);
use_prv_enum(CPSS_DXCH_IP_CNT_SET_MODE_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT);
use_prv_enum(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT);
use_prv_enum(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT);
use_prv_enum(CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT);
use_prv_enum(CPSS_DXCH_FDB_LEARN_PRIORITY_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC);
use_prv_enum(CPSS_DXCH_OAM_OPCODE_TYPE_ENT);
use_prv_struct(CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC);
use_prv_enum(CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT);
use_prv_struct(CPSS_DXCH_OAM_ENTRY_STC);
use_prv_enum(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT);
use_prv_enum(CPSS_DXCH3_POLICER_METER_MODE_ENT);
use_prv_enum(CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);
use_prv_enum(CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT);
use_prv_enum(CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT);
use_prv_enum(CPSS_DXCH_TTI_MODIFY_UP_ENT);
use_prv_struct(CPSS_DXCH_PTP_TOD_COUNT_STC);
use_prv_enum(CPSS_DXCH_MIN_SPEED_ENT);
use_prv_enum(CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
use_prv_struct(CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC);
use_prv_enum(CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT);
use_prv_enum(CPSS_DXCH_PORT_BUFFERS_MODE_ENT);
use_prv_enum(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_ENT);
use_prv_enum(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT);
use_prv_enum(CPSS_DXCH_NET_DSA_CMD_ENT);
use_prv_enum(CPSS_DRAM_TEMPERATURE_ENT);
use_prv_enum(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
use_prv_enum(CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
use_prv_enum(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT);
use_prv_struct(CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC);
use_prv_struct(CPSS_DXCH_PORT_AP_STATUS_STC);
use_prv_struct(CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC);
use_prv_enum(CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT);
use_prv_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
use_prv_struct(CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC);
use_prv_struct(CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC);
use_prv_enum(CPSS_DXCH_LED_PORT_TYPE_ENT);
use_prv_enum(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT);
use_prv_enum(CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT);
use_prv_struct(CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC);
use_prv_enum(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT);
use_prv_enum(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT);
use_prv_struct(CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC);
use_prv_enum(CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT);
use_prv_struct(CPSS_DXCH_PTP_TS_CFG_ENTRY_STC);
use_prv_enum(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC);
use_prv_struct(CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC);
use_prv_enum(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
use_prv_enum(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT);
use_prv_enum(CPSS_POLICER_COLOR_MODE_ENT);
use_prv_enum(CPSS_RATE_LIMIT_MODE_ENT);
use_prv_enum(CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT);
use_prv_enum(CPSS_DXCH_CFG_ROUTING_MODE_ENT);
use_prv_struct(CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC);
use_prv_enum(CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT);
use_prv_struct(CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC);
use_prv_enum(CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT);
use_prv_struct(CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC);
use_prv_struct(CPSS_DXCH_IPV6_PREFIX_STC);
use_prv_struct(CPSS_DXCH_PTP_TAI_ID_STC);
use_prv_enum(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT);
use_prv_enum(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT);
use_prv_enum(CPSS_DXCH_POLICER_STORM_TYPE_ENT);
use_prv_struct(CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC);
use_prv_enum(CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT);
use_prv_struct(CPSS_DXCH_CSCD_QOS_TC_REMAP_STC);
use_prv_enum(CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT);
use_prv_struct(CPSS_DXCH_L2_MLL_ENTRY_STC);
use_prv_struct(CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC);
use_prv_struct(CPSS_DXCH_CFG_GLOBAL_EPORT_STC);
use_prv_enum(CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT);
use_prv_struct(CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC);
use_prv_enum(CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT);
use_prv_enum(CPSS_DXCH_IP_BRG_SERVICE_ENT);
use_prv_enum(CPSS_DRAM_BUS_WIDTH_ENT);
use_prv_struct(CPSS_DXCH_OUTPUT_INTERFACE_STC);
use_prv_enum(CPSS_DXCH_TTI_KEY_TYPE_ENT);
use_prv_enum(CPSS_DXCH_IP_VLAN_CNT_MODE_ENT);
use_prv_enum(CPSS_DXCH_CPUPORT_TYPE_ENT);
use_prv_struct(CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC);
use_prv_enum(CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT);
use_prv_enum(CPSS_PACKET_CMD_ENT);
use_prv_struct(CPSS_DXCH_TTI_ETH_RULE_STC);
use_prv_struct(CPSS_DXCH_PTP_TAI_TOD_STEP_STC);
use_prv_struct(GT_IPADDR);
use_prv_enum(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT);
use_prv_enum(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT);
use_prv_struct(CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC);
use_prv_enum(CPSS_DXCH_STC_TYPE_ENT);
use_prv_enum(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT);
use_prv_enum(CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT);
use_prv_enum(CPSS_DXCH_PROTECTION_LOC_STATUS_ENT);
use_prv_struct(CPSS_DXCH_PTP_TSU_CONTROL_STC);
use_prv_enum(CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT);
use_prv_enum(CPSS_DXCH_PORT_GROUP_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
use_prv_struct(CPSS_DXCH_NET_DSA_TO_CPU_STC);
use_prv_enum(CPSS_DXCH_PORT_FC_MODE_ENT);
use_prv_enum(CPSS_POLICER_PACKET_SIZE_MODE_ENT);
use_prv_struct(CPSS_DXCH_TTI_RULE_COMMON_STC);
use_prv_struct(CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC);
use_prv_struct(CPSS_DXCH_TTI_TRILL_ADJACENCY_STC);
use_prv_struct(CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC);
use_prv_enum(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT);
use_prv_enum(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC);
use_prv_struct(CPSS_PORT_TX_WRTD_MASK_LSB_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC);
use_prv_struct(CPSS_DXCH_OAM_SRV_DM_CONFIG_STC);
use_prv_enum(CPSS_DXCH_CFG_TABLES_ENT);
use_prv_enum(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT);
use_prv_enum(CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT);
use_prv_struct(CPSS_DXCH_TCAM_BLOCK_INFO_STC);
use_prv_enum(CPSS_IP_NAT_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT);
use_prv_enum(CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT);
use_prv_struct(CPSS_DXCH_CFG_DEV_INFO_STC);
use_prv_enum(CPSS_DXCH_PORT_SERDES_SPEED_ENT);
use_prv_struct(CPSS_DXCH_TTI_TRILL_RULE_STC);
use_prv_enum(CPSS_IP_CNT_SET_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC);
use_prv_enum(CPSS_DXCH_OAM_STAGE_TYPE_ENT);
use_prv_enum(CPSS_DXCH_IMPLEMENT_WA_ENT);
use_prv_enum(CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT);
use_prv_struct(CPSS_DXCH_POLICER_BUCKETS_STATE_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC);
use_prv_struct(CPSS_DXCH_COS_PROFILE_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC);
use_prv_struct(CPSS_DXCH_TTI_ACTION_STC);
use_prv_struct(CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC);
use_prv_struct(CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC);
use_prv_enum(CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT);
use_prv_struct(CPSS_DXCH_L2_MLL_LTT_ENTRY_STC);
use_prv_enum(CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT);
use_prv_enum(CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT);
use_prv_enum(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT);
use_prv_enum(CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC);
use_prv_enum(CPSS_DXCH_POLICER_COUNTING_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC);
use_prv_enum(CPSS_DXCH_TTI_RULE_TYPE_ENT);
use_prv_enum(CPSS_IP_SITE_ID_ENT);
use_prv_struct(CPSS_DXCH_PORT_MAP_STC);
use_prv_enum(CPSS_DXCH_POLICER_MODIFY_UP_ENT);
use_prv_struct(CPSS_DXCH_PCL_ACTION_STC);
use_prv_enum(CPSS_DXCH_PCL_RULE_OPTION_ENT);
use_prv_struct(CPSS_DXCH_OAM_SRV_LM_CONFIG_STC);
use_prv_struct(CPSS_DXCH_CUT_THROUGH_BYPASS_STC);
use_prv_struct(CPSS_DXCH_PCL_ACTION_REDIRECT_STC);
use_prv_struct(CPSS_DXCH_IP_MLL_PAIR_STC);
use_prv_struct(CPSS_DXCH_PCL_ACTION_MIRROR_STC);
use_prv_struct(CPSS_DXCH_PCL_ACTION_POLICER_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC);
use_prv_enum(CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
use_prv_enum(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT);
use_prv_struct(CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC);
use_prv_enum(CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT);
use_prv_enum(CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT);
use_prv_enum(CPSS_DXCH_STC_COUNT_MODE_ENT);
use_prv_struct(CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC);
use_prv_enum(CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT);
use_prv_struct(CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC);
use_prv_enum(CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT);
use_prv_enum(CPSS_DXCH_BRG_FDB_DEL_MODE_ENT);
use_prv_struct(CPSS_DXCH_TTI_MPLS_RULE_STC);
use_prv_struct(CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC);
use_prv_struct(CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC);
use_prv_enum(CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT);
use_prv_enum(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT);
use_prv_struct(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC);
use_prv_enum(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT);
use_prv_struct(CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC);
use_prv_struct(CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC);
use_prv_struct(CPSS_DXCH3_POLICER_METERING_ENTRY_STC);
use_prv_struct(CPSS_DXCH_FABRIC_HGL_RX_DSA_STC);
use_prv_struct(CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC);
use_prv_struct(CPSS_DXCH_VIRTUAL_TCAM_INFO_STC);
use_prv_struct(CPSS_DXCH_IP_LPM_VR_CONFIG_STC);
use_prv_struct(CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC);
use_prv_struct(CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC);
use_prv_struct(CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC);
use_prv_struct(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC);
use_prv_struct(CPSS_DXCH_DETAILED_PORT_MAP_STC);
use_prv_struct(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC);
use_prv_struct(CPSS_DXCH_TTI_ACTION_2_STC);
use_prv_enum(CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT);
use_prv_enum(CPSS_DXCH_PORT_SERDES_MODE_ENT);
use_prv_struct(CPSS_DXCH_NET_RX_PARAMS_STC);
use_prv_enum(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT);
use_prv_enum(CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT);
use_prv_struct(CPSS_DXCH_LPM_RAM_CONFIG_STC);
use_prv_struct(CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC);
use_prv_struct(CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC);
use_prv_struct(CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC);
use_prv_struct(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC);
use_prv_enum(CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
use_prv_struct(CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC);
use_prv_enum(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT);
use_prv_struct(CPSS_DXCH_IP_LTT_ENTRY_STC);
use_prv_enum(CPSS_DXCH_TUNNEL_QOS_MODE_ENT);
use_prv_struct(CPSS_DXCH_PORT_AP_INTROP_STC);
use_prv_enum(CPSS_DXCH_IP_CPU_CODE_INDEX_ENT);
use_prv_struct(CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC);
use_prv_enum(CPSS_IP_PROTOCOL_STACK_ENT);
use_prv_enum(CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_ENT);
use_prv_enum(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT);
use_prv_struct(CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC);
use_prv_struct(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC);
use_prv_enum(CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT);
use_prv_struct(CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC);
use_prv_struct(CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC);
use_prv_struct(CPSS_DXCH_PORT_MAC_PARAMS_STC);
use_prv_enum(CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT);
use_prv_struct(CPSS_DXCH_PORT_AP_PARAMS_STC);
use_prv_struct(CPSS_DXCH_IP_COUNTER_SET_STC);
use_prv_enum(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT);
use_prv_enum(CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT);
use_prv_struct(CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC);
use_prv_struct(CPSS_DXCH_NET_DSA_COMMON_STC);
use_prv_enum(CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT);
use_prv_enum(CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT);
use_prv_struct(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC);
use_prv_enum(CPSS_DIAG_PG_IF_SIZE_ENT);
use_prv_enum(CPSS_DXCH_POLICER_CMD_ENT);
use_prv_enum(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT);
use_prv_enum(CPSS_DXCH_IP_DROP_CNT_MODE_ENT);
use_prv_enum(CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
use_prv_struct(CPSS_DXCH_IPFIX_TIMER_STC);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC);
use_prv_struct(CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC);
use_prv_enum(CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT);
use_prv_struct(CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC);
use_prv_enum(CPSS_DXCH_PA_UNIT_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC);
use_prv_struct(CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
use_prv_struct(CPSS_DXCH_PORT_COMBO_PARAMS_STC);
use_prv_enum(CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT);
use_prv_enum(CPSS_DXCH_PORT_UNITS_ID_ENT);
use_prv_struct(CPSS_DXCH_NET_DSA_FORWARD_STC);
use_prv_enum(CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT);
use_prv_struct(CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
use_prv_enum(CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
use_prv_enum(CPSS_DXCH_DIAG_BIST_STATUS_ENT);
use_prv_enum(CPSS_DXCH_CNC_TM_INDEX_MODE_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT);
use_prv_enum(CPSS_DIAG_PG_TRANSMIT_MODE_ENT);
use_prv_struct(CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC);
use_prv_struct(CPSS_DXCH_DIAG_BIST_RESULT_STC);
use_prv_struct(CPSS_DXCH_TM_GLUE_DRAM_CFG_STC);
use_prv_enum(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT);
use_prv_struct(CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC);
use_prv_enum(CPSS_DP_LEVEL_ENT);
use_prv_enum(CPSS_BRG_IPM_MODE_ENT);
use_prv_struct(CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC);
use_prv_struct(CPSS_DXCH_TUNNEL_TERM_ACTION_STC);
use_prv_enum(CPSS_DXCH_POLICER_REMARK_MODE_ENT);
use_prv_struct(CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC);
use_prv_struct(CPSS_DXCH_PP_CONFIG_INIT_STC);
use_prv_struct(CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC);
use_prv_struct(CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC);
use_prv_enum(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT);
use_prv_enum(CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT);
use_prv_enum(CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT);
use_prv_struct(CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC);
use_prv_enum(CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
use_prv_enum(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT);
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC);
use_prv_struct(CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC);
use_prv_struct(CPSS_DXCH_IP_NAT66_ENTRY_STC);
use_prv_struct(CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC);
use_prv_struct(CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC);
use_prv_struct(CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC);

/***** declarations ********/

enumDescr enumDescr_CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_HSU_DATA_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E),
    ENUM_ENTRY(CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E),
    ENUM_ENTRY(CPSS_DXCH_HSU_DATA_TYPE_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_HSU_DATA_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT);

enumDescr enumDescr_CPSS_DXCH_LED_CPU_OR_PORT27_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_LED_CPU_E),
    ENUM_ENTRY(CPSS_DXCH_LED_PORT27_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_LED_CPU_OR_PORT27_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_PCLK_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_RCVR_CLK_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_EXCEPTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_EXCEPTION_ENT);

enumDescr enumDescr_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ZERO_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_EPCL_QUEUE_ID_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_FLOW_ID_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_LOCAL_TARGET_PHY_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_PACKET_HASH_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_DEV_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_PHY_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_EPORT_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ONE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC(
    lua_State *L,
    CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC *val
)
{
    F_ARRAY_START(val, -1, vlanArray);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, vlanArray, idx, GT_U16);
        }
    }
    F_ARRAY_END(val, -1, vlanArray);
}

void prv_c_to_lua_CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC(
    lua_State *L,
    CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, vlanArray);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, vlanArray, idx, GT_U16);
        }
    }
    FO_ARRAY_END(val, t, vlanArray);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC);

enumDescr enumDescr_CPSS_DXCH_TCAM_CLIENT_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TCAM_IPCL_0_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_IPCL_1_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_IPCL_2_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_EPCL_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_TTI_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TCAM_CLIENT_ENT);

void prv_lua_to_c_CPSS_DXCH_SHADOW_PORT_MAP_STC(
    lua_State *L,
    CPSS_DXCH_SHADOW_PORT_MAP_STC *val
)
{
    F_ENUM(val, -1, mappingType, CPSS_DXCH_PORT_MAPPING_TYPE_ENT);
    F_BOOL(val, -1, trafficManagerEn);
    F_NUMBER(val, -1, portGroup, GT_U32);
    F_NUMBER(val, -1, macNum, GT_U32);
    F_NUMBER(val, -1, rxDmaNum, GT_U32);
    F_NUMBER(val, -1, txDmaNum, GT_U32);
    F_NUMBER(val, -1, txFifoPortNum, GT_U32);
    F_NUMBER(val, -1, txqNum, GT_U32);
    F_NUMBER(val, -1, ilknChannel, GT_U32);
    F_NUMBER(val, -1, tmPortIdx, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_SHADOW_PORT_MAP_STC(
    lua_State *L,
    CPSS_DXCH_SHADOW_PORT_MAP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, mappingType, CPSS_DXCH_PORT_MAPPING_TYPE_ENT);
    FO_BOOL(val, t, trafficManagerEn);
    FO_NUMBER(val, t, portGroup, GT_U32);
    FO_NUMBER(val, t, macNum, GT_U32);
    FO_NUMBER(val, t, rxDmaNum, GT_U32);
    FO_NUMBER(val, t, txDmaNum, GT_U32);
    FO_NUMBER(val, t, txFifoPortNum, GT_U32);
    FO_NUMBER(val, t, txqNum, GT_U32);
    FO_NUMBER(val, t, ilknChannel, GT_U32);
    FO_NUMBER(val, t, tmPortIdx, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_SHADOW_PORT_MAP_STC);

void prv_lua_to_c_CPSS_DXCH_LED_CLASS_CONF_STC(
    lua_State *L,
    CPSS_DXCH_LED_CLASS_CONF_STC *val
)
{
    F_NUMBER(val, -1, classNum, GT_U32);
    F_NUMBER(val, -1, classConf, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_LED_CLASS_CONF_STC(
    lua_State *L,
    CPSS_DXCH_LED_CLASS_CONF_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, classNum, GT_U32);
    FO_NUMBER(val, t, classConf, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_LED_CLASS_CONF_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    F_STRUCT(val, -1, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPV6ADDR);
    F_ARRAY_START(val, -1, udb47_49);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, udb47_49, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb47_49);
    F_NUMBER(val, -1, udb0, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    FO_STRUCT(val, t, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_STRUCT(val, t, dip, GT_IPV6ADDR);
    FO_ARRAY_START(val, t, udb47_49);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, udb47_49, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb47_49);
    FO_NUMBER(val, t, udb0, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC);

enumDescr enumDescr_CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_L2MLL_DISREGARD_PORT_TRUNK_CNT_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_L2MLL_PORT_CNT_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_L2MLL_TRUNK_CNT_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_POLICER_QOS_PARAM_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_QOS_PARAM_STC *val
)
{
    F_NUMBER(val, -1, up, GT_U32);
    F_NUMBER(val, -1, dscp, GT_U32);
    F_NUMBER(val, -1, exp, GT_U32);
    F_ENUM(val, -1, dp, CPSS_DP_LEVEL_ENT);
}

void prv_c_to_lua_CPSS_DXCH_POLICER_QOS_PARAM_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_QOS_PARAM_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, up, GT_U32);
    FO_NUMBER(val, t, dscp, GT_U32);
    FO_NUMBER(val, t, exp, GT_U32);
    FO_ENUM(val, t, dp, CPSS_DP_LEVEL_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_POLICER_QOS_PARAM_STC);

enumDescr enumDescr_CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_4K_VID_32_LP_E),
    ENUM_ENTRY(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_2K_VID_64_LP_E),
    ENUM_ENTRY(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_1K_VID_128_LP_E),
    ENUM_ENTRY(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_512_VID_256_LP_E),
    ENUM_ENTRY(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_256_VID_512_LP_E),
    ENUM_ENTRY(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_128_VID_1K_LP_E),
    ENUM_ENTRY(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_64_VID_2K_LP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E),
    ENUM_ENTRY(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_MIM_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_MIM_RULE_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    F_NUMBER(val, -1, bUp, GT_U32);
    F_NUMBER(val, -1, bDp, GT_U32);
    F_NUMBER(val, -1, iSid, GT_U32);
    F_NUMBER(val, -1, iUp, GT_U32);
    F_NUMBER(val, -1, iDp, GT_U32);
    F_NUMBER(val, -1, iRes1, GT_U32);
    F_NUMBER(val, -1, iRes2, GT_U32);
    F_BOOL(val, -1, macToMe);
    F_BOOL(val, -1, passengerPacketOuterTagExists);
    F_NUMBER(val, -1, passengerPacketOuterTagVid, GT_U32);
    F_NUMBER(val, -1, passengerPacketOuterTagUp, GT_U32);
    F_NUMBER(val, -1, passengerPacketOuterTagDei, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TTI_MIM_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_MIM_RULE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    FO_NUMBER(val, t, bUp, GT_U32);
    FO_NUMBER(val, t, bDp, GT_U32);
    FO_NUMBER(val, t, iSid, GT_U32);
    FO_NUMBER(val, t, iUp, GT_U32);
    FO_NUMBER(val, t, iDp, GT_U32);
    FO_NUMBER(val, t, iRes1, GT_U32);
    FO_NUMBER(val, t, iRes2, GT_U32);
    FO_BOOL(val, t, macToMe);
    FO_BOOL(val, t, passengerPacketOuterTagExists);
    FO_NUMBER(val, t, passengerPacketOuterTagVid, GT_U32);
    FO_NUMBER(val, t, passengerPacketOuterTagUp, GT_U32);
    FO_NUMBER(val, t, passengerPacketOuterTagDei, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_MIM_RULE_STC);

enumDescr enumDescr_CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_IDLE_E),
    ENUM_ENTRY(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_RUN_E),
    ENUM_ENTRY(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_LIMIT_E),
    ENUM_ENTRY(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_TERMINATE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    F_STRUCT(val, -1, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPV6ADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPV6ADDR);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_ARRAY_START(val, -1, udb28_35);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, udb28_35, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb28_35);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    FO_STRUCT(val, t, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC);
    FO_STRUCT(val, t, sip, GT_IPV6ADDR);
    FO_STRUCT(val, t, dip, GT_IPV6ADDR);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_ARRAY_START(val, t, udb28_35);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, udb28_35, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb28_35);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_POLICER_ENABLE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_POLICER_ENABLE_ENT);

enumDescr enumDescr_CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_25_SINGLE_ENDED_E),
    ENUM_ENTRY(CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E),
    ENUM_ENTRY(CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E),
    ENUM_ENTRY(CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_SINGLE_ENDED_E),
    ENUM_ENTRY(CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E),
    ENUM_ENTRY(CPSS_DXCH_PP_SERDES_REF_CLOCK_INTERNAL_125_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT);

void prv_lua_to_c_CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC(
    lua_State *L,
    CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC *val
)
{
    F_NUMBER(val, -1, packetCounter, GT_U32);
    F_STRUCT(val, -1, octetCounter, GT_U64);
    F_STRUCT(val, -1, latencyTimeCounter, GT_U64);
    F_NUMBER(val, -1, agedPacketCounter, GT_U32);
    F_NUMBER(val, -1, maxLatencyTime, GT_U32);
    F_NUMBER(val, -1, minLatencyTime, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC(
    lua_State *L,
    CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, packetCounter, GT_U32);
    FO_STRUCT(val, t, octetCounter, GT_U64);
    FO_STRUCT(val, t, latencyTimeCounter, GT_U64);
    FO_NUMBER(val, t, agedPacketCounter, GT_U32);
    FO_NUMBER(val, t, maxLatencyTime, GT_U32);
    FO_NUMBER(val, t, minLatencyTime, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC);

enumDescr enumDescr_CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_MEMBERS_MODE_SOHO_EMULATION_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EVLAN_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EPORT_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC *val
)
{
    F_STRUCT(val, -1, egressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPADDR);
    F_NUMBER(val, -1, ipv4Options, GT_U8);
    F_NUMBER(val, -1, isVidx, GT_U8);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_ARRAY_START(val, -1, udb36_49);
    {
        int idx;
        for (idx = 0; idx < 14; idx++) {
            F_ARRAY_NUMBER(val, udb36_49, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb36_49);
    F_NUMBER(val, -1, udb0, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, egressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC);
    FO_STRUCT(val, t, sip, GT_IPADDR);
    FO_STRUCT(val, t, dip, GT_IPADDR);
    FO_NUMBER(val, t, ipv4Options, GT_U8);
    FO_NUMBER(val, t, isVidx, GT_U8);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_ARRAY_START(val, t, udb36_49);
    {
        int idx;
        for (idx = 0; idx < 14; idx++) {
            FO_ARRAY_NUMBER(val, udb36_49, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb36_49);
    FO_NUMBER(val, t, udb0, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC);

enumDescr enumDescr_CPSS_DXCH_TCAM_RULE_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TCAM_RULE_SIZE_10_B_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_RULE_SIZE_20_B_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_RULE_SIZE_30_B_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_RULE_SIZE_40_B_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_RULE_SIZE_50_B_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_RULE_SIZE_60_B_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_RULE_SIZE_80_B_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TCAM_RULE_SIZE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_ERROR_JABBER_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_ERROR_FRAGMENTS_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_ERROR_UNDERSIZE_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_ERROR_OVERSIZE_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_ERROR_RX_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_ERROR_CRC_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_ERROR_OVERRUN_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_ERROR_NO_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_CLOCK_MODE_DISABLED_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_CLOCK_MODE_OUTPUT_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_ADJUST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_OPCODE_E),
    ENUM_ENTRY(CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_LSBIT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_PCS_LOOPBACK_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PCS_LOOPBACK_TX2RX_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PCS_LOOPBACK_RX2TX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E),
    ENUM_ENTRY(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E),
    ENUM_ENTRY(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT);

void prv_lua_to_c_CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC *val
)
{
    F_ARRAY_START(val, -1, unitList);
    {
        int idx;
        for (idx = 0; idx < 14; idx++) {
            F_ARRAY_ENUM(val, unitList, idx, CPSS_DXCH_PA_UNIT_ENT);
        }
    }
    F_ARRAY_END(val, -1, unitList);
    F_ARRAY_START(val, -1, unitState);
    {
        int idx;
        for (idx = 0; idx < 13; idx++) {
            F_ARRAY_STRUCT(val, unitState, idx, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
        }
    }
    F_ARRAY_END(val, -1, unitState);
}

void prv_c_to_lua_CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, unitList);
    {
        int idx;
        for (idx = 0; idx < 14; idx++) {
            FO_ARRAY_ENUM(val, unitList, idx, CPSS_DXCH_PA_UNIT_ENT);
        }
    }
    FO_ARRAY_END(val, t, unitList);
    FO_ARRAY_START(val, t, unitState);
    {
        int idx;
        for (idx = 0; idx < 13; idx++) {
            FO_ARRAY_STRUCT(val, unitState, idx, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
        }
    }
    FO_ARRAY_END(val, t, unitState);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC *val
)
{
    F_NUMBER(val, -1, outOfSequenceCount, GT_U32);
    F_NUMBER(val, -1, totalCheckedDataTlv, GT_U32);
    F_NUMBER(val, -1, totalInvalidDataTlv, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, outOfSequenceCount, GT_U32);
    FO_NUMBER(val, t, totalCheckedDataTlv, GT_U32);
    FO_NUMBER(val, t, totalInvalidDataTlv, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_ACTION_OAM_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_OAM_STC *val
)
{
    F_BOOL(val, -1, timeStampEnable);
    F_NUMBER(val, -1, offsetIndex, GT_U32);
    F_BOOL(val, -1, oamProcessEnable);
    F_NUMBER(val, -1, oamProfile, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_ACTION_OAM_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_OAM_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, timeStampEnable);
    FO_NUMBER(val, t, offsetIndex, GT_U32);
    FO_BOOL(val, t, oamProcessEnable);
    FO_NUMBER(val, t, oamProfile, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_ACTION_OAM_STC);

enumDescr enumDescr_CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E),
    ENUM_ENTRY(CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_XOFF_E),
    ENUM_ENTRY(CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E),
    ENUM_ENTRY(CPSS_PORT_PERIODIC_FLOW_CONTROL_XOFF_ONLY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_METER_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_L2_MLL_PAIR_STC(
    lua_State *L,
    CPSS_DXCH_L2_MLL_PAIR_STC *val
)
{
    F_STRUCT(val, -1, firstMllNode, CPSS_DXCH_L2_MLL_ENTRY_STC);
    F_STRUCT(val, -1, secondMllNode, CPSS_DXCH_L2_MLL_ENTRY_STC);
    F_NUMBER(val, -1, nextPointer, GT_U32);
    F_ENUM(val, -1, entrySelector, CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT);
}

void prv_c_to_lua_CPSS_DXCH_L2_MLL_PAIR_STC(
    lua_State *L,
    CPSS_DXCH_L2_MLL_PAIR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, firstMllNode, CPSS_DXCH_L2_MLL_ENTRY_STC);
    FO_STRUCT(val, t, secondMllNode, CPSS_DXCH_L2_MLL_ENTRY_STC);
    FO_NUMBER(val, t, nextPointer, GT_U32);
    FO_ENUM(val, t, entrySelector, CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_L2_MLL_PAIR_STC);

enumDescr enumDescr_CPSS_DXCH_PTP_TS_TAG_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TAG_MODE_NONE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TAG_MODE_ALL_NON_EXTENDED_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TAG_MODE_ALL_EXTENDED_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TAG_MODE_NON_EXTENDED_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TAG_MODE_EXTENDED_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TAG_MODE_PIGGYBACK_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_TAG_MODE_HYBRID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TS_TAG_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ALL_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC *val
)
{
    F_BOOL(val, -1, entryValid);
    F_NUMBER(val, -1, entryId, GT_U32);
    F_NUMBER(val, -1, taiSelect, GT_U32);
    F_NUMBER(val, -1, todUpdateFlag, GT_U32);
    F_NUMBER(val, -1, timestamp, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, entryValid);
    FO_NUMBER(val, t, entryId, GT_U32);
    FO_NUMBER(val, t, taiSelect, GT_U32);
    FO_NUMBER(val, t, todUpdateFlag, GT_U32);
    FO_NUMBER(val, t, timestamp, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    F_STRUCT(val, -1, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPADDR);
    F_NUMBER(val, -1, etherType, GT_U16);
    F_NUMBER(val, -1, l2Encap, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_NUMBER(val, -1, ipv4Fragmented, GT_U8);
    F_ARRAY_START(val, -1, udb0_5);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            F_ARRAY_NUMBER(val, udb0_5, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb0_5);
    F_NUMBER(val, -1, tag1Exist, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, trunkHash, GT_U32);
    F_ARRAY_START(val, -1, udb39_46);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, udb39_46, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb39_46);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    FO_STRUCT(val, t, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC);
    FO_STRUCT(val, t, sip, GT_IPADDR);
    FO_STRUCT(val, t, dip, GT_IPADDR);
    FO_NUMBER(val, t, etherType, GT_U16);
    FO_NUMBER(val, t, l2Encap, GT_U32);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_NUMBER(val, t, ipv4Fragmented, GT_U8);
    FO_ARRAY_START(val, t, udb0_5);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            FO_ARRAY_NUMBER(val, udb0_5, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb0_5);
    FO_NUMBER(val, t, tag1Exist, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    FO_ARRAY_START(val, t, udb39_46);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, udb39_46, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb39_46);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC);

enumDescr enumDescr_CPSS_DIAG_PP_MEM_BIST_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PP_MEM_BIST_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC *val
)
{
    F_NUMBER(val, -1, numOfIpv4Prefixes, GT_U32);
    F_NUMBER(val, -1, numOfIpv6Prefixes, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, numOfIpv4Prefixes, GT_U32);
    FO_NUMBER(val, t, numOfIpv6Prefixes, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC);

enumDescr enumDescr_CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E),
    ENUM_ENTRY(CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_SERDES_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, txAmp, GT_U32);
    F_BOOL(val, -1, txEmphEn);
    F_BOOL(val, -1, txEmphType);
    F_NUMBER(val, -1, txEmphAmp, GT_U32);
    F_NUMBER(val, -1, txAmpAdj, GT_U32);
    F_NUMBER(val, -1, ffeSignalSwingControl, GT_U32);
    F_NUMBER(val, -1, ffeResistorSelect, GT_U32);
    F_NUMBER(val, -1, ffeCapacitorSelect, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_SERDES_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, txAmp, GT_U32);
    FO_BOOL(val, t, txEmphEn);
    FO_BOOL(val, t, txEmphType);
    FO_NUMBER(val, t, txEmphAmp, GT_U32);
    FO_NUMBER(val, t, txAmpAdj, GT_U32);
    FO_NUMBER(val, t, ffeSignalSwingControl, GT_U32);
    FO_NUMBER(val, t, ffeResistorSelect, GT_U32);
    FO_NUMBER(val, t, ffeCapacitorSelect, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_SERDES_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_TTI_PW_TAG_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_TAG_DISABLED_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_TAG_RAW_PW_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_TAG_TAGGED_PW_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_PW_TAG_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_DIAG_DESCRIPTOR_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_DESCRIPTOR_STC *val
)
{
    F_ARRAY_START(val, -1, fieldValue);
    {
        int idx;
        for (idx = 0; idx < 321; idx++) {
            F_ARRAY_NUMBER(val, fieldValue, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, fieldValue);
    F_ARRAY_START(val, -1, fieldValueValid);
    {
        int idx;
        for (idx = 0; idx < 321; idx++) {
            F_ARRAY_BOOL(val, fieldValueValid, idx, GT_BOOL);
        }
    }
    F_ARRAY_END(val, -1, fieldValueValid);
}

void prv_c_to_lua_CPSS_DXCH_DIAG_DESCRIPTOR_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_DESCRIPTOR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, fieldValue);
    {
        int idx;
        for (idx = 0; idx < 321; idx++) {
            FO_ARRAY_NUMBER(val, fieldValue, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, fieldValue);
    FO_ARRAY_START(val, t, fieldValueValid);
    {
        int idx;
        for (idx = 0; idx < 321; idx++) {
            FO_ARRAY_BOOL(val, fieldValueValid, idx, GT_BOOL);
        }
    }
    FO_ARRAY_END(val, t, fieldValueValid);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_DIAG_DESCRIPTOR_STC);

enumDescr enumDescr_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_EXCESS_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_SUMMARY_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_EXCEPTION_TYPE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_HOL_FC_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_FC_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_HOL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_HOL_FC_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_REPLACE_MAC_SA_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_LOGICAL_PORT_ASSIGN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TAI_NUMBER_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_NUMBER_0_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_NUMBER_1_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_NUMBER_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TAI_NUMBER_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_FORMAT_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC(
    lua_State *L,
    CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC *val
)
{
    F_NUMBER(val, -1, tc, GT_U32);
    F_ENUM(val, -1, dp, CPSS_DP_LEVEL_ENT);
    F_BOOL(val, -1, isStack);
    F_ENUM(val, -1, dsaTagCmd, CPSS_DXCH_NET_DSA_CMD_ENT);
}

void prv_c_to_lua_CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC(
    lua_State *L,
    CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, tc, GT_U32);
    FO_ENUM(val, t, dp, CPSS_DP_LEVEL_ENT);
    FO_BOOL(val, t, isStack);
    FO_ENUM(val, t, dsaTagCmd, CPSS_DXCH_NET_DSA_CMD_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_4B_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_8B_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_12B_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_16B_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT);

enumDescr enumDescr_CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_L4_LBH_LONG_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_L4_LBH_SHORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_DSA_QOS_PROFILE_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_DSA_UP_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_VLAN_TAG_UP_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_MPLS_EXP_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_IPV4_TOS_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_IPV6_TC_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_UDE_INDEX_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_MAC_DA_INDEX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_CRC_32_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E),
    ENUM_ENTRY(CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E),
    ENUM_ENTRY(CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_ADDRESS_E),
    ENUM_ENTRY(CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_ADDRESS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT);

void prv_lua_to_c_CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, etherType, GT_U32);
    F_NUMBER(val, -1, pfcOpcode, GT_U32);
    F_NUMBER(val, -1, up, GT_U32);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_NUMBER(val, -1, trgDev, GT_U8);
    F_NUMBER(val, -1, trgPort, GT_PHYSICAL_PORT_NUM);
}

void prv_c_to_lua_CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, etherType, GT_U32);
    FO_NUMBER(val, t, pfcOpcode, GT_U32);
    FO_NUMBER(val, t, up, GT_U32);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_NUMBER(val, t, trgDev, GT_U8);
    FO_NUMBER(val, t, trgPort, GT_PHYSICAL_PORT_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC);

enumDescr enumDescr_CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT);

void prv_lua_to_c_CPSS_DXCH_NET_SDMA_TX_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_SDMA_TX_PARAMS_STC *val
)
{
    F_BOOL(val, -1, recalcCrc);
    F_NUMBER(val, -1, txQueue, GT_U8);
    F_NUMBER(val, -1, evReqHndl, GT_UINTPTR);
    F_BOOL(val, -1, invokeTxBufferQueueEvent);
}

void prv_c_to_lua_CPSS_DXCH_NET_SDMA_TX_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_SDMA_TX_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, recalcCrc);
    FO_NUMBER(val, t, txQueue, GT_U8);
    FO_NUMBER(val, t, evReqHndl, GT_UINTPTR);
    FO_BOOL(val, t, invokeTxBufferQueueEvent);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_NET_SDMA_TX_PARAMS_STC);

enumDescr enumDescr_CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_INGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_EGRESS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_GREEN_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_YELLOW_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_RED_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_DROP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT);

void prv_lua_to_c_CPSS_DXCH_BRG_VLAN_INFO_STC(
    lua_State *L,
    CPSS_DXCH_BRG_VLAN_INFO_STC *val
)
{
    F_BOOL(val, -1, unkSrcAddrSecBreach);
    F_ENUM(val, -1, unregNonIpMcastCmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, unregIpv4McastCmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, unregIpv6McastCmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, unkUcastCmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, unregIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, unregNonIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    F_BOOL(val, -1, ipv4IgmpToCpuEn);
    F_BOOL(val, -1, mirrToRxAnalyzerEn);
    F_BOOL(val, -1, ipv6IcmpToCpuEn);
    F_ENUM(val, -1, ipCtrlToCpuEn, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
    F_ENUM(val, -1, ipv4IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    F_ENUM(val, -1, ipv6IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    F_BOOL(val, -1, ipv4IpmBrgEn);
    F_BOOL(val, -1, ipv6IpmBrgEn);
    F_ENUM(val, -1, ipv6SiteIdMode, CPSS_IP_SITE_ID_ENT);
    F_BOOL(val, -1, ipv4UcastRouteEn);
    F_BOOL(val, -1, ipv4McastRouteEn);
    F_BOOL(val, -1, ipv6UcastRouteEn);
    F_BOOL(val, -1, ipv6McastRouteEn);
    F_NUMBER(val, -1, stgId, GT_U32);
    F_BOOL(val, -1, autoLearnDisable);
    F_BOOL(val, -1, naMsgToCpuEn);
    F_NUMBER(val, -1, mruIdx, GT_U32);
    F_BOOL(val, -1, bcastUdpTrapMirrEn);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, floodVidx, GT_U16);
    F_ENUM(val, -1, floodVidxMode, CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
    F_ENUM(val, -1, portIsolationMode, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
    F_BOOL(val, -1, ucastLocalSwitchingEn);
    F_BOOL(val, -1, mcastLocalSwitchingEn);
    F_NUMBER(val, -1, mirrToRxAnalyzerIndex, GT_U32);
    F_BOOL(val, -1, mirrToTxAnalyzerEn);
    F_NUMBER(val, -1, mirrToTxAnalyzerIndex, GT_U32);
    F_NUMBER(val, -1, fidValue, GT_U32);
    F_ENUM(val, -1, unknownMacSaCmd, CPSS_PACKET_CMD_ENT);
    F_BOOL(val, -1, ipv4McBcMirrToAnalyzerEn);
    F_NUMBER(val, -1, ipv4McBcMirrToAnalyzerIndex, GT_U32);
    F_BOOL(val, -1, ipv6McMirrToAnalyzerEn);
    F_NUMBER(val, -1, ipv6McMirrToAnalyzerIndex, GT_U32);
    F_BOOL(val, -1, fcoeForwardingEn);
    F_ENUM(val, -1, unregIpmEVidxMode, CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT);
    F_NUMBER(val, -1, unregIpmEVidx, GT_U32);
    F_ENUM(val, -1, fdbLookupKeyMode, CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_BRG_VLAN_INFO_STC(
    lua_State *L,
    CPSS_DXCH_BRG_VLAN_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, unkSrcAddrSecBreach);
    FO_ENUM(val, t, unregNonIpMcastCmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, unregIpv4McastCmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, unregIpv6McastCmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, unkUcastCmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, unregIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, unregNonIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    FO_BOOL(val, t, ipv4IgmpToCpuEn);
    FO_BOOL(val, t, mirrToRxAnalyzerEn);
    FO_BOOL(val, t, ipv6IcmpToCpuEn);
    FO_ENUM(val, t, ipCtrlToCpuEn, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
    FO_ENUM(val, t, ipv4IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    FO_ENUM(val, t, ipv6IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    FO_BOOL(val, t, ipv4IpmBrgEn);
    FO_BOOL(val, t, ipv6IpmBrgEn);
    FO_ENUM(val, t, ipv6SiteIdMode, CPSS_IP_SITE_ID_ENT);
    FO_BOOL(val, t, ipv4UcastRouteEn);
    FO_BOOL(val, t, ipv4McastRouteEn);
    FO_BOOL(val, t, ipv6UcastRouteEn);
    FO_BOOL(val, t, ipv6McastRouteEn);
    FO_NUMBER(val, t, stgId, GT_U32);
    FO_BOOL(val, t, autoLearnDisable);
    FO_BOOL(val, t, naMsgToCpuEn);
    FO_NUMBER(val, t, mruIdx, GT_U32);
    FO_BOOL(val, t, bcastUdpTrapMirrEn);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, floodVidx, GT_U16);
    FO_ENUM(val, t, floodVidxMode, CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
    FO_ENUM(val, t, portIsolationMode, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
    FO_BOOL(val, t, ucastLocalSwitchingEn);
    FO_BOOL(val, t, mcastLocalSwitchingEn);
    FO_NUMBER(val, t, mirrToRxAnalyzerIndex, GT_U32);
    FO_BOOL(val, t, mirrToTxAnalyzerEn);
    FO_NUMBER(val, t, mirrToTxAnalyzerIndex, GT_U32);
    FO_NUMBER(val, t, fidValue, GT_U32);
    FO_ENUM(val, t, unknownMacSaCmd, CPSS_PACKET_CMD_ENT);
    FO_BOOL(val, t, ipv4McBcMirrToAnalyzerEn);
    FO_NUMBER(val, t, ipv4McBcMirrToAnalyzerIndex, GT_U32);
    FO_BOOL(val, t, ipv6McMirrToAnalyzerEn);
    FO_NUMBER(val, t, ipv6McMirrToAnalyzerIndex, GT_U32);
    FO_BOOL(val, t, fcoeForwardingEn);
    FO_ENUM(val, t, unregIpmEVidxMode, CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT);
    FO_NUMBER(val, t, unregIpmEVidx, GT_U32);
    FO_ENUM(val, t, fdbLookupKeyMode, CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_VLAN_INFO_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_MSB_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_LSB_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_QOS_PROFILE_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_TRUNK_HASH_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_TAG1_INFO_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_UC_E),
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_MC_E),
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_BC_E),
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_OTHER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT(
    lua_State *L,
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT(
    lua_State *L,
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, regularEntry, CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC);
    FO_STRUCT(val, t, ecmpRpfCheck, CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT);

enumDescr enumDescr_CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_8_E),
    ENUM_ENTRY(CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_64_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_QOS_REMAP_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_256_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_512_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_1K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_2K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_4K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_8K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_16K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_32K_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_NO_BYPASS_E),
    ENUM_ENTRY(CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ONE_LOOKUP_E),
    ENUM_ENTRY(CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_TWO_LOOKUP_E),
    ENUM_ENTRY(CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_FULL_BYPASS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_0_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_1_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_2_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_3_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_4_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_5_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_OFFSET_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_OFFSET_L2_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_OFFSET_L4_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_OFFSET_METADATA_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_OFFSET_INVALID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_OFFSET_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_POLICER_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_ENTRY_STC *val
)
{
    F_BOOL(val, -1, policerEnable);
    F_ENUM(val, -1, meterColorMode, CPSS_POLICER_COLOR_MODE_ENT);
    F_STRUCT(val, -1, tbParams, CPSS_DXCH_POLICER_TB_PARAMS_STC);
    F_BOOL(val, -1, counterEnable);
    F_NUMBER(val, -1, counterSetIndex, GT_U32);
    F_ENUM(val, -1, cmd, CPSS_DXCH_POLICER_CMD_ENT);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_ENUM(val, -1, modifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    F_ENUM(val, -1, modifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_POLICER_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, policerEnable);
    FO_ENUM(val, t, meterColorMode, CPSS_POLICER_COLOR_MODE_ENT);
    FO_STRUCT(val, t, tbParams, CPSS_DXCH_POLICER_TB_PARAMS_STC);
    FO_BOOL(val, t, counterEnable);
    FO_NUMBER(val, t, counterSetIndex, GT_U32);
    FO_ENUM(val, t, cmd, CPSS_DXCH_POLICER_CMD_ENT);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_ENUM(val, t, modifyDscp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    FO_ENUM(val, t, modifyUp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_POLICER_ENTRY_STC);

void prv_lua_to_c_CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC(
    lua_State *L,
    CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC *val
)
{
    F_BOOL(val, -1, tcDpLimit);
    F_BOOL(val, -1, portLimit);
    F_BOOL(val, -1, tcLimit);
    F_BOOL(val, -1, sharedPoolLimit);
}

void prv_c_to_lua_CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC(
    lua_State *L,
    CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tcDpLimit);
    FO_BOOL(val, t, portLimit);
    FO_BOOL(val, t, tcLimit);
    FO_BOOL(val, t, sharedPoolLimit);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC);

void prv_lua_to_c_CPSS_DXCH_CNC_COUNTER_STC(
    lua_State *L,
    CPSS_DXCH_CNC_COUNTER_STC *val
)
{
    F_STRUCT(val, -1, byteCount, GT_U64);
    F_STRUCT(val, -1, packetCount, GT_U64);
}

void prv_c_to_lua_CPSS_DXCH_CNC_COUNTER_STC(
    lua_State *L,
    CPSS_DXCH_CNC_COUNTER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, byteCount, GT_U64);
    FO_STRUCT(val, t, packetCount, GT_U64);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_CNC_COUNTER_STC);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_CNC_COUNTER_FORMAT_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_COUNTER_FORMAT_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_BRG_EXCP_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_UC_IPV4_TRAP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_UC_IPV6_TRAP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_UC_IPV4_SOFT_DROP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_UC_IPV6_SOFT_DROP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_MC_IPV4_TRAP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_MC_IPV6_TRAP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_MC_IPV4_SOFT_DROP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_MC_IPV6_SOFT_DROP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_ARP_TRAP_EXCP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_ARP_SOFT_DROP_EXCP_CMD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_BRG_EXCP_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_2K_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_10K_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TAI_INSTANCE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_INSTANCE_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_INSTANCE_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TAI_INSTANCE_ENT);

void prv_lua_to_c_CPSS_DXCH_IPV4_PREFIX_STC(
    lua_State *L,
    CPSS_DXCH_IPV4_PREFIX_STC *val
)
{
    F_NUMBER(val, -1, vrId, GT_U32);
    F_STRUCT_CUSTOM(val, -1, ipAddr, GT_IPADDR);
    F_BOOL(val, -1, isMcSource);
    F_NUMBER(val, -1, mcGroupIndexRow, GT_U32);
    F_NUMBER(val, -1, mcGroupIndexColumn, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IPV4_PREFIX_STC(
    lua_State *L,
    CPSS_DXCH_IPV4_PREFIX_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, vrId, GT_U32);
    FO_STRUCT(val, t, ipAddr, GT_IPADDR);
    FO_BOOL(val, t, isMcSource);
    FO_NUMBER(val, t, mcGroupIndexRow, GT_U32);
    FO_NUMBER(val, t, mcGroupIndexColumn, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IPV4_PREFIX_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_PACKET_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_UDE_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_IPV6_TCP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_IPV6_UDP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_IPV6_OTHER_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_UDE5_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_UDE6_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PACKET_TYPE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_PACKET_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_NO_REDIRECT_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_VRF_ID_ASSIGN_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_LOGICAL_PORT_ASSIGN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);

enumDescr enumDescr_CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_CNT_SET_ID_0_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_CNT_SET_ID_1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT);

enumDescr enumDescr_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_LOOPBACK_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_LOOPBACK_ANALOG_TX2RX_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_LOOPBACK_DIGITAL_TX2RX_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_LOOPBACK_DIGITAL_RX2TX_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_LOOPBACK_MAX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_ACTION_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_ACTION_TYPE1_ENT),
    ENUM_ENTRY(CPSS_DXCH_TTI_ACTION_TYPE2_ENT),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_ACTION_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC *val
)
{
    F_ENUM(val, -1, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, up, GT_U32);
    F_BOOL(val, -1, tagEnable);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_STRUCT_CUSTOM(val, -1, nextHopMacDa, GT_ETHERADDR);
    F_STRUCT(val, -1, trillHeader, CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, up, GT_U32);
    FO_BOOL(val, t, tagEnable);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_STRUCT(val, t, nextHopMacDa, GT_ETHERADDR);
    FO_STRUCT(val, t, trillHeader, CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E),
    ENUM_ENTRY(CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC *val
)
{
    F_NUMBER(val, -1, isIpv4, GT_U8);
    F_NUMBER(val, -1, ipProtocol, GT_U32);
    F_NUMBER(val, -1, dscp, GT_U32);
    F_NUMBER(val, -1, isL4Valid, GT_U8);
    F_NUMBER(val, -1, l4Byte2, GT_U8);
    F_NUMBER(val, -1, l4Byte3, GT_U8);
    F_NUMBER(val, -1, ipv4Fragmented, GT_U8);
    F_NUMBER(val, -1, egrTcpUdpPortComparator, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, isIpv4, GT_U8);
    FO_NUMBER(val, t, ipProtocol, GT_U32);
    FO_NUMBER(val, t, dscp, GT_U32);
    FO_NUMBER(val, t, isL4Valid, GT_U8);
    FO_NUMBER(val, t, l4Byte2, GT_U8);
    FO_NUMBER(val, t, l4Byte3, GT_U8);
    FO_NUMBER(val, t, ipv4Fragmented, GT_U8);
    FO_NUMBER(val, t, egrTcpUdpPortComparator, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC);

enumDescr enumDescr_CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC(
    lua_State *L,
    CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC *val
)
{
    F_ENUM(val, -1, includeSrcInterface, CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT);
    F_NUMBER_N(val, -1, srcHwDevice, srcDevice, GT_HW_DEV_NUM);
    F_BOOL(val, -1, srcIsTrunk);
    F_NUMBER(val, -1, srcPortTrunk, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC(
    lua_State *L,
    CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, includeSrcInterface, CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT);
    FO_NUMBER_N(val, t, srcHwDevice, srcDevice, GT_HW_DEV_NUM);
    FO_BOOL(val, t, srcIsTrunk);
    FO_NUMBER(val, t, srcPortTrunk, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC);

enumDescr enumDescr_CPSS_DXCH_ETHERTYPE_TABLE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_ETHERTYPE_TABLE_INGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_TS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_ETHERTYPE_TABLE_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_URPF_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_URPF_DISABLE_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_URPF_VLAN_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_URPF_PORT_TRUNK_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_URPF_LOOSE_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_URPF_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_UC_ROUTE_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_IP_UC_ECMP_RPF_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_MAC_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_MAC_MODE_DA_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MAC_MODE_SA_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_MAC_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_UC_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_MC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_FROM_TS_ENTRY_AND_VLAN_ENTRY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_64_CORE_CLOCKS_E),
    ENUM_ENTRY(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_1024_CORE_CLOCKS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_MULTI_TARGET_SHAPER_GRANULARITY_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_PKT_TS_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_DISABLED_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENABLED_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_FORCED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC(
    lua_State *L,
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC *val
)
{
    F_STRUCT(val, -1, interface, CPSS_INTERFACE_INFO_STC);
}

void prv_c_to_lua_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC(
    lua_State *L,
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, interface, CPSS_INTERFACE_INFO_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_PFC_ENABLE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_PFC_ENABLE_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV4_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV6_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_VLAN_INDEX_MODE_ORIGINAL_VID_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_VLAN_INDEX_MODE_EVID_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_VLAN_INDEX_MODE_TAG1_VID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_L2_REMARK_MODEL_TC_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_L2_REMARK_MODEL_UP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT);

enumDescr enumDescr_CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_HIT_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_LPM_ECC_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_ECMP_ECC_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_PBR_BUCKET_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_CONTINUE_TO_LOOKUP_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_0_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_1_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_DST_G_IPV4_PACKETS_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_SRC_G_IPV4_PACKETS_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_0_IPV6_PACKETS_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_1_IPV6_PACKETS_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_DST_G_IPV6_PACKETS_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_SRC_G_IPV6_PACKETS_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_FCOE_D_ID_LOOKUP_E),
    ENUM_ENTRY(CPSS_DXCH_LPM_EXCEPTION_FCOE_S_ID_LOOKUP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT);

void prv_lua_to_c_CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC *val
)
{
    F_NUMBER(val, -1, totalConfiguredSlices, GT_U32);
    F_NUMBER(val, -1, slicesNumInGop, GT_U32);
    F_ARRAY_START(val, -1, slice_enable);
    {
        int idx;
        for (idx = 0; idx < 252; idx++) {
            F_ARRAY_BOOL(val, slice_enable, idx, GT_BOOL);
        }
    }
    F_ARRAY_END(val, -1, slice_enable);
    F_ARRAY_START(val, -1, slice_occupied_by);
    {
        int idx;
        for (idx = 0; idx < 252; idx++) {
            F_ARRAY_NUMBER(val, slice_occupied_by, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, slice_occupied_by);
    F_ENUM(val, -1, cpu_port_type, CPSS_DXCH_CPUPORT_TYPE_ENT);
    F_ARRAY_START(val, -1, cpu_port_sliceId);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            F_ARRAY_NUMBER(val, cpu_port_sliceId, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, cpu_port_sliceId);
    F_ARRAY_START(val, -1, cpu_port_slice_is_enable);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            F_ARRAY_BOOL(val, cpu_port_slice_is_enable, idx, GT_BOOL);
        }
    }
    F_ARRAY_END(val, -1, cpu_port_slice_is_enable);
    F_ARRAY_START(val, -1, cpu_port_slice_occupied_by);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            F_ARRAY_NUMBER(val, cpu_port_slice_occupied_by, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, cpu_port_slice_occupied_by);
}

void prv_c_to_lua_CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, totalConfiguredSlices, GT_U32);
    FO_NUMBER(val, t, slicesNumInGop, GT_U32);
    FO_ARRAY_START(val, t, slice_enable);
    {
        int idx;
        for (idx = 0; idx < 252; idx++) {
            FO_ARRAY_BOOL(val, slice_enable, idx, GT_BOOL);
        }
    }
    FO_ARRAY_END(val, t, slice_enable);
    FO_ARRAY_START(val, t, slice_occupied_by);
    {
        int idx;
        for (idx = 0; idx < 252; idx++) {
            FO_ARRAY_NUMBER(val, slice_occupied_by, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, slice_occupied_by);
    FO_ENUM(val, t, cpu_port_type, CPSS_DXCH_CPUPORT_TYPE_ENT);
    FO_ARRAY_START(val, t, cpu_port_sliceId);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            FO_ARRAY_NUMBER(val, cpu_port_sliceId, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, cpu_port_sliceId);
    FO_ARRAY_START(val, t, cpu_port_slice_is_enable);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            FO_ARRAY_BOOL(val, cpu_port_slice_is_enable, idx, GT_BOOL);
        }
    }
    FO_ARRAY_END(val, t, cpu_port_slice_is_enable);
    FO_ARRAY_START(val, t, cpu_port_slice_occupied_by);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            FO_ARRAY_NUMBER(val, cpu_port_slice_occupied_by, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, cpu_port_slice_occupied_by);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);

void prv_lua_to_c_CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC(
    lua_State *L,
    CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC *val
)
{
    F_ARRAY_START(val, -1, l4DstPortArray);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, l4DstPortArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, l4DstPortArray);
    F_ARRAY_START(val, -1, l4SrcPortArray);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, l4SrcPortArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, l4SrcPortArray);
    F_ARRAY_START(val, -1, ipv6FlowArray);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, ipv6FlowArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, ipv6FlowArray);
    F_ARRAY_START(val, -1, ipDipArray);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            F_ARRAY_NUMBER(val, ipDipArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, ipDipArray);
    F_ARRAY_START(val, -1, ipSipArray);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            F_ARRAY_NUMBER(val, ipSipArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, ipSipArray);
    F_ARRAY_START(val, -1, macDaArray);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            F_ARRAY_NUMBER(val, macDaArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, macDaArray);
    F_ARRAY_START(val, -1, macSaArray);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            F_ARRAY_NUMBER(val, macSaArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, macSaArray);
    F_ARRAY_START(val, -1, mplsLabel0Array);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, mplsLabel0Array, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, mplsLabel0Array);
    F_ARRAY_START(val, -1, mplsLabel1Array);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, mplsLabel1Array, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, mplsLabel1Array);
    F_ARRAY_START(val, -1, mplsLabel2Array);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, mplsLabel2Array, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, mplsLabel2Array);
    F_ARRAY_START(val, -1, localSrcPortArray);
    {
        int idx;
        for (idx = 0; idx < 1; idx++) {
            F_ARRAY_NUMBER(val, localSrcPortArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, localSrcPortArray);
    F_ARRAY_START(val, -1, udbsArray);
    {
        int idx;
        for (idx = 0; idx < 32; idx++) {
            F_ARRAY_NUMBER(val, udbsArray, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udbsArray);
}

void prv_c_to_lua_CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC(
    lua_State *L,
    CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, l4DstPortArray);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, l4DstPortArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, l4DstPortArray);
    FO_ARRAY_START(val, t, l4SrcPortArray);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, l4SrcPortArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, l4SrcPortArray);
    FO_ARRAY_START(val, t, ipv6FlowArray);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, ipv6FlowArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, ipv6FlowArray);
    FO_ARRAY_START(val, t, ipDipArray);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            FO_ARRAY_NUMBER(val, ipDipArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, ipDipArray);
    FO_ARRAY_START(val, t, ipSipArray);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            FO_ARRAY_NUMBER(val, ipSipArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, ipSipArray);
    FO_ARRAY_START(val, t, macDaArray);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            FO_ARRAY_NUMBER(val, macDaArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, macDaArray);
    FO_ARRAY_START(val, t, macSaArray);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            FO_ARRAY_NUMBER(val, macSaArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, macSaArray);
    FO_ARRAY_START(val, t, mplsLabel0Array);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, mplsLabel0Array, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, mplsLabel0Array);
    FO_ARRAY_START(val, t, mplsLabel1Array);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, mplsLabel1Array, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, mplsLabel1Array);
    FO_ARRAY_START(val, t, mplsLabel2Array);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, mplsLabel2Array, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, mplsLabel2Array);
    FO_ARRAY_START(val, t, localSrcPortArray);
    {
        int idx;
        for (idx = 0; idx < 1; idx++) {
            FO_ARRAY_NUMBER(val, localSrcPortArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, localSrcPortArray);
    FO_ARRAY_START(val, t, udbsArray);
    {
        int idx;
        for (idx = 0; idx < 32; idx++) {
            FO_ARRAY_NUMBER(val, udbsArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udbsArray);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC);

enumDescr enumDescr_CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_RPF_CHECK_E),
    ENUM_ENTRY(CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_BIDIRECTIONAL_TREE_CHECK_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC *val
)
{
    F_STRUCT(val, -1, basicCounters, CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, basicCounters, CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC);

enumDescr enumDescr_CPSS_DXCH_OAM_SRV_MP_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_OAM_SRV_MP_TYPE_UP_MEP_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_SRV_MP_TYPE_DOWN_MEP_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_SRV_MP_TYPE_MIP_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_SRV_MP_TYPE_OTHER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_OAM_SRV_MP_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC *val
)
{
    F_ENUM(val, -1, command, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    F_BOOL(val, -1, summaryBitEnable);
}

void prv_c_to_lua_CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, command, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_BOOL(val, t, summaryBitEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_SDU_UP_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_SPEED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT);

void prv_lua_to_c_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC *val
)
{
    F_BOOL(val, -1, meteringEnable);
    F_BOOL(val, -1, countingEnable);
    F_NUMBER(val, -1, policerIndex, GT_U32);
    F_BOOL(val, -1, ucKnownEnable);
    F_NUMBER(val, -1, ucKnownOffset, GT_U32);
    F_BOOL(val, -1, ucUnknownEnable);
    F_NUMBER(val, -1, ucUnknownOffset, GT_U32);
    F_BOOL(val, -1, mcRegisteredEnable);
    F_NUMBER(val, -1, mcRegisteredOffset, GT_U32);
    F_BOOL(val, -1, mcUnregisteredEnable);
    F_NUMBER(val, -1, mcUnregisteredOffset, GT_U32);
    F_BOOL(val, -1, bcEnable);
    F_NUMBER(val, -1, bcOffset, GT_U32);
    F_BOOL(val, -1, tcpSynEnable);
    F_NUMBER(val, -1, tcpSynOffset, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, meteringEnable);
    FO_BOOL(val, t, countingEnable);
    FO_NUMBER(val, t, policerIndex, GT_U32);
    FO_BOOL(val, t, ucKnownEnable);
    FO_NUMBER(val, t, ucKnownOffset, GT_U32);
    FO_BOOL(val, t, ucUnknownEnable);
    FO_NUMBER(val, t, ucUnknownOffset, GT_U32);
    FO_BOOL(val, t, mcRegisteredEnable);
    FO_NUMBER(val, t, mcRegisteredOffset, GT_U32);
    FO_BOOL(val, t, mcUnregisteredEnable);
    FO_NUMBER(val, t, mcUnregisteredOffset, GT_U32);
    FO_BOOL(val, t, bcEnable);
    FO_NUMBER(val, t, bcOffset, GT_U32);
    FO_BOOL(val, t, tcpSynEnable);
    FO_NUMBER(val, t, tcpSynOffset, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV4_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV6_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_IPV6_INCREMENT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC(
    lua_State *L,
    CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC *val
)
{
    F_BOOL(val, -1, isExist);
    F_NUMBER(val, -1, vTcamId, GT_U32);
    F_STRUCT(val, -1, vTcamInfo, CPSS_DXCH_VIRTUAL_TCAM_INFO_STC);
}

void prv_c_to_lua_CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC(
    lua_State *L,
    CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, isExist);
    FO_NUMBER(val, t, vTcamId, GT_U32);
    FO_STRUCT(val, t, vTcamInfo, CPSS_DXCH_VIRTUAL_TCAM_INFO_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC);

void prv_lua_to_c_CPSS_PORT_CNM_GENERATION_CONFIG_STC(
    lua_State *L,
    CPSS_PORT_CNM_GENERATION_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, qosProfileId, GT_U32);
    F_NUMBER(val, -1, isRouted, GT_U8);
    F_BOOL(val, -1, overrideUp);
    F_NUMBER(val, -1, cnmUp, GT_U8);
    F_NUMBER(val, -1, defaultVlanId, GT_U16);
    F_NUMBER(val, -1, scaleFactor, GT_U32);
    F_NUMBER(val, -1, version, GT_U32);
    F_ARRAY_START(val, -1, cpidMsb);
    {
        int idx;
        for (idx = 0; idx < 7; idx++) {
            F_ARRAY_NUMBER(val, cpidMsb, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, cpidMsb);
    F_BOOL(val, -1, cnUntaggedEnable);
    F_BOOL(val, -1, forceCnTag);
    F_NUMBER(val, -1, flowIdTag, GT_U32);
    F_BOOL(val, -1, appendPacket);
    F_NUMBER(val, -1, sourceId, GT_U32);
}

void prv_c_to_lua_CPSS_PORT_CNM_GENERATION_CONFIG_STC(
    lua_State *L,
    CPSS_PORT_CNM_GENERATION_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, qosProfileId, GT_U32);
    FO_NUMBER(val, t, isRouted, GT_U8);
    FO_BOOL(val, t, overrideUp);
    FO_NUMBER(val, t, cnmUp, GT_U8);
    FO_NUMBER(val, t, defaultVlanId, GT_U16);
    FO_NUMBER(val, t, scaleFactor, GT_U32);
    FO_NUMBER(val, t, version, GT_U32);
    FO_ARRAY_START(val, t, cpidMsb);
    {
        int idx;
        for (idx = 0; idx < 7; idx++) {
            FO_ARRAY_NUMBER(val, cpidMsb, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, cpidMsb);
    FO_BOOL(val, t, cnUntaggedEnable);
    FO_BOOL(val, t, forceCnTag);
    FO_NUMBER(val, t, flowIdTag, GT_U32);
    FO_BOOL(val, t, appendPacket);
    FO_NUMBER(val, t, sourceId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_CNM_GENERATION_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_MT_TC_QUEUE_SP_SCHED_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_LED_GROUP_CONF_STC(
    lua_State *L,
    CPSS_DXCH_LED_GROUP_CONF_STC *val
)
{
    F_NUMBER(val, -1, groupNum, GT_U32);
    F_NUMBER(val, -1, groupConf, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_LED_GROUP_CONF_STC(
    lua_State *L,
    CPSS_DXCH_LED_GROUP_CONF_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, groupNum, GT_U32);
    FO_NUMBER(val, t, groupConf, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_LED_GROUP_CONF_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_GIGA_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_XG_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_HGL_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_XLG_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_CPU_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_OAM_SRV_FRAME_TYPE_1731_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_SRV_FRAME_TYPE_1711_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC *val
)
{
    F_BOOL(val, -1, link);
    F_ENUM(val, -1, speed, CPSS_PORT_SPEED_ENT);
    F_ENUM(val, -1, duplex, CPSS_PORT_DUPLEX_ENT);
}

void prv_c_to_lua_CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, link);
    FO_ENUM(val, t, speed, CPSS_PORT_SPEED_ENT);
    FO_ENUM(val, t, duplex, CPSS_PORT_DUPLEX_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_AUTONEG_ADVERTISMENT_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC *val
)
{
    F_BOOL(val, -1, entryValid);
    F_BOOL(val, -1, isPtpExeption);
    F_ENUM(val, -1, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    F_NUMBER(val, -1, taiSelect, GT_U32);
    F_NUMBER(val, -1, todUpdateFlag, GT_U32);
    F_NUMBER(val, -1, messageType, GT_U32);
    F_NUMBER(val, -1, domainNum, GT_U32);
    F_NUMBER(val, -1, sequenceId, GT_U32);
    F_NUMBER(val, -1, timestamp, GT_32);
    F_NUMBER(val, -1, portNum, GT_PHYSICAL_PORT_NUM);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, entryValid);
    FO_BOOL(val, t, isPtpExeption);
    FO_ENUM(val, t, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    FO_NUMBER(val, t, taiSelect, GT_U32);
    FO_NUMBER(val, t, todUpdateFlag, GT_U32);
    FO_NUMBER(val, t, messageType, GT_U32);
    FO_NUMBER(val, t, domainNum, GT_U32);
    FO_NUMBER(val, t, sequenceId, GT_U32);
    FO_NUMBER(val, t, timestamp, GT_32);
    FO_NUMBER(val, t, portNum, GT_PHYSICAL_PORT_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC);

enumDescr enumDescr_CPSS_PORT_TX_SHARED_DP_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E),
    ENUM_ENTRY(CPSS_PORT_TX_SHARED_DP_MODE_ALL_E),
    ENUM_ENTRY(CPSS_PORT_TX_SHARED_DP_MODE_DP0_E),
    ENUM_ENTRY(CPSS_PORT_TX_SHARED_DP_MODE_DP0_DP1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_PORT_TX_SHARED_DP_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_5_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT);

enumDescr enumDescr_CPSS_DXCH_UDB_ERROR_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E),
    ENUM_ENTRY(CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E),
    ENUM_ENTRY(CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E),
    ENUM_ENTRY(CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_UDB_ERROR_CMD_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC *val
)
{
    F_BOOL(val, -1, enableMatchCount);
    F_NUMBER(val, -1, matchCounterIndex, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, enableMatchCount);
    FO_NUMBER(val, t, matchCounterIndex, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_PERIODIC_FC_TYPE_GIG_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PERIODIC_FC_TYPE_XG_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_TO_ZERO_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_TO_PACKET_HASH_VALUE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_START_CONFIG_UNT(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_START_CONFIG_UNT(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, ipv4Cfg, CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC);
    FO_STRUCT(val, t, ipv6Cfg, CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC);
    FO_STRUCT(val, t, mplsCfg, CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC);
    FO_STRUCT(val, t, mimCfg, CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC);
    FO_STRUCT(val, t, trillCfg, CPSS_DXCH_TUNNEL_START_TRILL_CONFIG_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_START_CONFIG_UNT);

void prv_lua_to_c_CPSS_DXCH_IP_MLL_STC(
    lua_State *L,
    CPSS_DXCH_IP_MLL_STC *val
)
{
    F_ENUM(val, -1, mllRPFFailCommand, CPSS_PACKET_CMD_ENT);
    F_BOOL(val, -1, isTunnelStart);
    F_STRUCT(val, -1, nextHopInterface, CPSS_INTERFACE_INFO_STC);
    F_NUMBER(val, -1, nextHopVlanId, GT_U16);
    F_NUMBER(val, -1, nextHopTunnelPointer, GT_U32);
    F_ENUM(val, -1, tunnelStartPassengerType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    F_NUMBER(val, -1, ttlHopLimitThreshold, GT_U16);
    F_BOOL(val, -1, excludeSrcVlan);
    F_BOOL(val, -1, last);
}

void prv_c_to_lua_CPSS_DXCH_IP_MLL_STC(
    lua_State *L,
    CPSS_DXCH_IP_MLL_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, mllRPFFailCommand, CPSS_PACKET_CMD_ENT);
    FO_BOOL(val, t, isTunnelStart);
    FO_STRUCT(val, t, nextHopInterface, CPSS_INTERFACE_INFO_STC);
    FO_NUMBER(val, t, nextHopVlanId, GT_U16);
    FO_NUMBER(val, t, nextHopTunnelPointer, GT_U32);
    FO_ENUM(val, t, tunnelStartPassengerType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    FO_NUMBER(val, t, ttlHopLimitThreshold, GT_U16);
    FO_BOOL(val, t, excludeSrcVlan);
    FO_BOOL(val, t, last);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_MLL_STC);

enumDescr enumDescr_CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_ALL_CLIENTS_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_AND_ENTRIES_FOR_ALL_CLIENTS_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_DUMP_INTERNAL_INFO_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_IPV4_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_IPV4_RULE_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    F_NUMBER(val, -1, tunneltype, GT_U32);
    F_STRUCT_CUSTOM(val, -1, srcIp, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, destIp, GT_IPADDR);
    F_BOOL(val, -1, isArp);
}

void prv_c_to_lua_CPSS_DXCH_TTI_IPV4_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_IPV4_RULE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    FO_NUMBER(val, t, tunneltype, GT_U32);
    FO_STRUCT(val, t, srcIp, GT_IPADDR);
    FO_STRUCT(val, t, destIp, GT_IPADDR);
    FO_BOOL(val, t, isArp);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_IPV4_RULE_STC);

enumDescr enumDescr_CPSS_DXCH_NET_DSA_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_3_WORD_TYPE_ENT),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_NET_DSA_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_COLOR_COUNT_DP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_VLAN_COMMAND_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_VLAN_COMMAND_ENT);

enumDescr enumDescr_CPSS_BRG_STP_STATE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_BRG_STP_E_VLAN_MODE_E),
    ENUM_ENTRY(CPSS_BRG_STP_E_PORT_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_BRG_STP_STATE_MODE_ENT);

void prv_lua_to_c_CPSS_PORT_TX_WRTD_MASK_LSB_STC(
    lua_State *L,
    CPSS_PORT_TX_WRTD_MASK_LSB_STC *val
)
{
    F_NUMBER(val, -1, tcDp, GT_U32);
    F_NUMBER(val, -1, port, GT_U32);
    F_NUMBER(val, -1, tc, GT_U32);
    F_NUMBER(val, -1, pool, GT_U32);
}

void prv_c_to_lua_CPSS_PORT_TX_WRTD_MASK_LSB_STC(
    lua_State *L,
    CPSS_PORT_TX_WRTD_MASK_LSB_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, tcDp, GT_U32);
    FO_NUMBER(val, t, port, GT_U32);
    FO_NUMBER(val, t, tc, GT_U32);
    FO_NUMBER(val, t, pool, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_TX_WRTD_MASK_LSB_STC);

void prv_lua_to_c_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *val
)
{
    F_NUMBER(val, -1, numOfIpv4Prefixes, GT_U32);
    F_NUMBER(val, -1, numOfIpv4McSourcePrefixes, GT_U32);
    F_NUMBER(val, -1, numOfIpv6Prefixes, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, numOfIpv4Prefixes, GT_U32);
    FO_NUMBER(val, t, numOfIpv4McSourcePrefixes, GT_U32);
    FO_NUMBER(val, t, numOfIpv6Prefixes, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC);

enumDescr enumDescr_CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_NO_MATCH_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC *val
)
{
    F_ARRAY_START(val, -1, udb);
    {
        int idx;
        for (idx = 0; idx < 50; idx++) {
            F_ARRAY_NUMBER(val, udb, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb);
    F_STRUCT(val, -1, replacedFld, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC);
    F_STRUCT(val, -1, udb60FixedFld, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, udb);
    {
        int idx;
        for (idx = 0; idx < 50; idx++) {
            FO_ARRAY_NUMBER(val, udb, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb);
    FO_STRUCT(val, t, replacedFld, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC);
    FO_STRUCT(val, t, udb60FixedFld, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_LENGTH_2_KB_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_LENGTH_10_KB_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT);

void prv_lua_to_c_CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC(
    lua_State *L,
    CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC *val
)
{
    F_NUMBER(val, -1, threshold0, GT_U32);
    F_NUMBER(val, -1, threshold1, GT_U32);
    F_NUMBER(val, -1, threshold2, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC(
    lua_State *L,
    CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, threshold0, GT_U32);
    FO_NUMBER(val, t, threshold1, GT_U32);
    FO_NUMBER(val, t, threshold2, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC);

enumDescr enumDescr_CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_ROUTER_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_PCL_TCAM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_TS_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_VLAN_ENTRY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_INCLUDED_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_COMPENSATED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E),
    ENUM_ENTRY(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC *val
)
{
    F_NUMBER(val, -1, txFrameCntForward, GT_U32);
    F_NUMBER(val, -1, rxFrameCntForward, GT_U32);
    F_NUMBER(val, -1, txFrameCntBackward, GT_U32);
    F_NUMBER(val, -1, rxFrameCntBackward, GT_U32);
    F_NUMBER(val, -1, farEndFrameLoss, GT_U32);
    F_NUMBER(val, -1, nearEndFrameLoss, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, txFrameCntForward, GT_U32);
    FO_NUMBER(val, t, rxFrameCntForward, GT_U32);
    FO_NUMBER(val, t, txFrameCntBackward, GT_U32);
    FO_NUMBER(val, t, rxFrameCntBackward, GT_U32);
    FO_NUMBER(val, t, farEndFrameLoss, GT_U32);
    FO_NUMBER(val, t, nearEndFrameLoss, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC);

enumDescr enumDescr_CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_2_AND_2_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_1_AND_3_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_3_AND_1_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_FOUR_TCAM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_POLICER_TB_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_TB_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, cir, GT_U32);
    F_NUMBER(val, -1, cbs, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_POLICER_TB_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_TB_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, cir, GT_U32);
    FO_NUMBER(val, t, cbs, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_POLICER_TB_PARAMS_STC);

enumDescr enumDescr_CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_INCLUDE_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_EXCLUDE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NET_TO_CPU_FLOW_ID_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_NET_TO_CPU_TT_OFFSET_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC *val
)
{
    F_NUMBER(val, -1, pclId, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC);

void prv_lua_to_c_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, xonThreshold, GT_U32);
    F_NUMBER(val, -1, xoffThreshold, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, xonThreshold, GT_U32);
    FO_NUMBER(val, t, xoffThreshold, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_IP_EXCEPTION_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_HDR_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_HDR_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_ILLEGAL_ADDRESS_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_ILLEGAL_ADDRESS_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_DIP_DA_MISMATCH_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_DIP_DA_MISMATCH_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_ALL_ZERO_SIP_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_ALL_ZERO_SIP_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_OPTION_HOP_BY_HOP_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_OPTION_HOP_BY_HOP_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_NON_HOP_BY_HOP_EXT_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_NON_HOP_BY_HOP_EXT_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_SIP_SA_FAIL_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_NON_DF_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_NON_DF_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_UC_MTU_EXCEEDED_FOR_DF_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCP_MC_MTU_EXCEEDED_FOR_DF_E),
    ENUM_ENTRY(CPSS_DXCH_IP_EXCEPTION_TYPE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_EXCEPTION_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_KEY_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_SIZE_10_B_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_SIZE_20_B_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_SIZE_30_B_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_KEY_SIZE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_BILLING_INDEX_MODE_STANDARD_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_BILLING_INDEX_MODE_FLOW_ID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_LOCAL_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_FINAL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_TCAM_MANAGER_RANGE_STC(
    lua_State *L,
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC *val
)
{
    F_NUMBER(val, -1, firstLine, GT_U32);
    F_NUMBER(val, -1, lastLine, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TCAM_MANAGER_RANGE_STC(
    lua_State *L,
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, firstLine, GT_U32);
    FO_NUMBER(val, t, lastLine, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TCAM_MANAGER_RANGE_STC);

enumDescr enumDescr_CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V1_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V2_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_REGULAR_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_FORCE_AGE_WITHOUT_REMOVAL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC *val
)
{
    F_ENUM(val, -1, templateDataSize, CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT);
    F_ARRAY_START(val, -1, templateDataBitsCfg);
    {
        int idx;
        for (idx = 0; idx < 128; idx++) {
            F_ARRAY_ENUM(val, templateDataBitsCfg, idx, CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT);
        }
    }
    F_ARRAY_END(val, -1, templateDataBitsCfg);
    F_NUMBER(val, -1, hashShiftLeftBitsNumber, GT_U32);
    F_ENUM(val, -1, udpSrcPortMode, CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, templateDataSize, CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT);
    FO_ARRAY_START(val, t, templateDataBitsCfg);
    {
        int idx;
        for (idx = 0; idx < 128; idx++) {
            FO_ARRAY_ENUM(val, templateDataBitsCfg, idx, CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT);
        }
    }
    FO_ARRAY_END(val, t, templateDataBitsCfg);
    FO_NUMBER(val, t, hashShiftLeftBitsNumber, GT_U32);
    FO_ENUM(val, t, udpSrcPortMode, CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC);

enumDescr enumDescr_CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_PAYLOAD_CYCLIC_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_PAYLOAD_RANDOM_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_PAYLOAD_CONSTANT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IPFIX_SAMPLING_DIST_DETERMINISTIC_E),
    ENUM_ENTRY(CPSS_DXCH_IPFIX_SAMPLING_DIST_RANDOM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_UNT(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_UNT(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, ruleStdNotIp, CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC);
    FO_STRUCT(val, t, ruleStdIpL2Qos, CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC);
    FO_STRUCT(val, t, ruleStdIpv4L4, CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC);
    FO_STRUCT(val, t, ruleStdIpv6Dip, CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC);
    FO_STRUCT(val, t, ruleIngrStdUdb, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC);
    FO_STRUCT(val, t, ruleExtNotIpv6, CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC);
    FO_STRUCT(val, t, ruleExtIpv6L2, CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC);
    FO_STRUCT(val, t, ruleExtIpv6L4, CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC);
    FO_STRUCT(val, t, ruleIngrExtUdb, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC);
    FO_STRUCT(val, t, ruleEgrStdNotIp, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC);
    FO_STRUCT(val, t, ruleEgrStdIpL2Qos, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC);
    FO_STRUCT(val, t, ruleEgrStdIpv4L4, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC);
    FO_STRUCT(val, t, ruleEgrExtNotIpv6, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC);
    FO_STRUCT(val, t, ruleEgrExtIpv6L2, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC);
    FO_STRUCT(val, t, ruleEgrExtIpv6L4, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC);
    FO_STRUCT(val, t, ruleStdIpv4RoutedAclQos, CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC);
    FO_STRUCT(val, t, ruleExtIpv4PortVlanQos, CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC);
    FO_STRUCT(val, t, ruleUltraIpv6PortVlanQos, CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC);
    FO_STRUCT(val, t, ruleUltraIpv6RoutedAclQos, CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC);
    FO_STRUCT(val, t, ruleEgrExtIpv4RaclVacl, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC);
    FO_STRUCT(val, t, ruleEgrUltraIpv6RaclVacl, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC);
    FO_STRUCT(val, t, ruleIngrUdbOnly, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC);
    FO_STRUCT(val, t, ruleEgrUdbOnly, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_UNT);

enumDescr enumDescr_CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IPFIX_SAMPLING_MODE_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_IPFIX_SAMPLING_MODE_PACKET_E),
    ENUM_ENTRY(CPSS_DXCH_IPFIX_SAMPLING_MODE_BYTE_E),
    ENUM_ENTRY(CPSS_DXCH_IPFIX_SAMPLING_MODE_TIME_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_MAPPING_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAPPING_TYPE_REMOTE_PHYSICAL_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAPPING_TYPE_MAX_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAPPING_TYPE_INVALID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_MAPPING_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_2_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_4_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_8_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT);

enumDescr enumDescr_CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_REGULAR_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_USE_TO_CPU_CONFIG_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC *val
)
{
    F_STRUCT(val, -1, ingressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
    F_NUMBER(val, -1, isL2Valid, GT_U8);
    F_NUMBER(val, -1, isBc, GT_U8);
    F_NUMBER(val, -1, isIp, GT_U8);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_NUMBER(val, -1, l2Encap, GT_U32);
    F_NUMBER(val, -1, etherType, GT_U16);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_NUMBER(val, -1, tag1Exist, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, ipFragmented, GT_U8);
    F_NUMBER(val, -1, ipHeaderInfo, GT_U32);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPADDR);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, trunkHash, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_ARRAY_START(val, -1, UdbStdIpL2Qos);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, UdbStdIpL2Qos, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbStdIpL2Qos);
    F_ARRAY_START(val, -1, UdbStdIpV4L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, UdbStdIpV4L4, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbStdIpV4L4);
    F_ARRAY_START(val, -1, UdbExtIpv6L2);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, UdbExtIpv6L2, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbExtIpv6L2);
    F_ARRAY_START(val, -1, UdbExtIpv6L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, UdbExtIpv6L4, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbExtIpv6L4);
    F_ARRAY_START(val, -1, udb5_16);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            F_ARRAY_NUMBER(val, udb5_16, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb5_16);
    F_ARRAY_START(val, -1, udb31_38);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, udb31_38, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb31_38);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, ingressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
    FO_NUMBER(val, t, isL2Valid, GT_U8);
    FO_NUMBER(val, t, isBc, GT_U8);
    FO_NUMBER(val, t, isIp, GT_U8);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_NUMBER(val, t, l2Encap, GT_U32);
    FO_NUMBER(val, t, etherType, GT_U16);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_NUMBER(val, t, tag1Exist, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, ipFragmented, GT_U8);
    FO_NUMBER(val, t, ipHeaderInfo, GT_U32);
    FO_STRUCT(val, t, sip, GT_IPADDR);
    FO_STRUCT(val, t, dip, GT_IPADDR);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_ARRAY_START(val, t, UdbStdIpL2Qos);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, UdbStdIpL2Qos, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbStdIpL2Qos);
    FO_ARRAY_START(val, t, UdbStdIpV4L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, UdbStdIpV4L4, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbStdIpV4L4);
    FO_ARRAY_START(val, t, UdbExtIpv6L2);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, UdbExtIpv6L2, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbExtIpv6L2);
    FO_ARRAY_START(val, t, UdbExtIpv6L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, UdbExtIpv6L4, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbExtIpv6L4);
    FO_ARRAY_START(val, t, udb5_16);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            FO_ARRAY_NUMBER(val, udb5_16, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb5_16);
    FO_ARRAY_START(val, t, udb31_38);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, udb31_38, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb31_38);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *val
)
{
    F_ENUM(val, -1, ptpIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    F_ENUM(val, -1, ptpIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    F_ENUM(val, -1, ntpIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    F_ENUM(val, -1, ntpIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    F_ENUM(val, -1, wampIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    F_ENUM(val, -1, wampIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, ptpIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    FO_ENUM(val, t, ptpIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    FO_ENUM(val, t, ntpIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    FO_ENUM(val, t, ntpIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    FO_ENUM(val, t, wampIpv4Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    FO_ENUM(val, t, wampIpv6Mode, CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC);

enumDescr enumDescr_CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_DYNAMIC_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_STATIC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_IPV6_HASH_MSB_SIP_DIP_FLOW_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_REGULAR_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_FORCE_EPCL_CONFIG_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, tc, GT_U8);
    F_ENUM(val, -1, dp, CPSS_DP_LEVEL_ENT);
    F_BOOL(val, -1, truncate);
    F_ENUM(val, -1, cpuRateLimitMode, CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT);
    F_NUMBER(val, -1, cpuCodeRateLimiterIndex, GT_U32);
    F_NUMBER(val, -1, cpuCodeStatRateLimitIndex, GT_U32);
    F_NUMBER(val, -1, designatedDevNumIndex, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, tc, GT_U8);
    FO_ENUM(val, t, dp, CPSS_DP_LEVEL_ENT);
    FO_BOOL(val, t, truncate);
    FO_ENUM(val, t, cpuRateLimitMode, CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT);
    FO_NUMBER(val, t, cpuCodeRateLimiterIndex, GT_U32);
    FO_NUMBER(val, t, cpuCodeStatRateLimitIndex, GT_U32);
    FO_NUMBER(val, t, designatedDevNumIndex, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_TTI_MAC_VLAN_STC(
    lua_State *L,
    CPSS_DXCH_TTI_MAC_VLAN_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, mac, GT_ETHERADDR);
    F_NUMBER(val, -1, vlanId, GT_U16);
}

void prv_c_to_lua_CPSS_DXCH_TTI_MAC_VLAN_STC(
    lua_State *L,
    CPSS_DXCH_TTI_MAC_VLAN_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, mac, GT_ETHERADDR);
    FO_NUMBER(val, t, vlanId, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_MAC_VLAN_STC);

enumDescr enumDescr_CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ADD_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_SUBTRACT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT);

void prv_lua_to_c_CPSS_PORT_CN_PROFILE_CONFIG_STC(
    lua_State *L,
    CPSS_PORT_CN_PROFILE_CONFIG_STC *val
)
{
    F_BOOL(val, -1, cnAware);
    F_NUMBER(val, -1, threshold, GT_U32);
}

void prv_c_to_lua_CPSS_PORT_CN_PROFILE_CONFIG_STC(
    lua_State *L,
    CPSS_PORT_CN_PROFILE_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, cnAware);
    FO_NUMBER(val, t, threshold, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_CN_PROFILE_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_CLEAR_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_RECALCULATE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_KEEP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_DISABLED_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_EPORT_ENABLED_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_EVLAN_ENABLED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC(
    lua_State *L,
    CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC *val
)
{
    F_BOOL(val, -1, enableBc);
    F_BOOL(val, -1, enableMc);
    F_BOOL(val, -1, enableMcReg);
    F_BOOL(val, -1, enableUcUnk);
    F_BOOL(val, -1, enableUcKnown);
    F_NUMBER(val, -1, rateLimit, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC(
    lua_State *L,
    CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, enableBc);
    FO_BOOL(val, t, enableMc);
    FO_BOOL(val, t, enableMcReg);
    FO_BOOL(val, t, enableUcUnk);
    FO_BOOL(val, t, enableUcKnown);
    FO_NUMBER(val, t, rateLimit, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC);

void prv_lua_to_c_CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC *val
)
{
    F_NUMBER(val, -1, totalConfiguredSlices, GT_U32);
    F_NUMBER(val, -1, totalSlicesOnUnit, GT_U32);
    F_BOOL(val, -1, strictPriorityBit);
    F_ARRAY_START(val, -1, slice_enable);
    {
        int idx;
        for (idx = 0; idx < 32; idx++) {
            F_ARRAY_BOOL(val, slice_enable, idx, GT_BOOL);
        }
    }
    F_ARRAY_END(val, -1, slice_enable);
    F_ARRAY_START(val, -1, slice_occupied_by);
    {
        int idx;
        for (idx = 0; idx < 32; idx++) {
            F_ARRAY_NUMBER(val, slice_occupied_by, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, slice_occupied_by);
}

void prv_c_to_lua_CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, totalConfiguredSlices, GT_U32);
    FO_NUMBER(val, t, totalSlicesOnUnit, GT_U32);
    FO_BOOL(val, t, strictPriorityBit);
    FO_ARRAY_START(val, t, slice_enable);
    {
        int idx;
        for (idx = 0; idx < 32; idx++) {
            FO_ARRAY_BOOL(val, slice_enable, idx, GT_BOOL);
        }
    }
    FO_ARRAY_END(val, t, slice_enable);
    FO_ARRAY_START(val, t, slice_occupied_by);
    {
        int idx;
        for (idx = 0; idx < 32; idx++) {
            FO_ARRAY_NUMBER(val, slice_occupied_by, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, slice_occupied_by);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ALL_EXCEPT_SA_LEARNING_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ONLY_FORWARDING_DECISION_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC *val
)
{
    F_NUMBER(val, -1, isIpv6, GT_U8);
    F_NUMBER(val, -1, ipProtocol, GT_U32);
    F_NUMBER(val, -1, dscp, GT_U32);
    F_NUMBER(val, -1, isL4Valid, GT_U8);
    F_NUMBER(val, -1, l4Byte0, GT_U8);
    F_NUMBER(val, -1, l4Byte1, GT_U8);
    F_NUMBER(val, -1, l4Byte2, GT_U8);
    F_NUMBER(val, -1, l4Byte3, GT_U8);
    F_NUMBER(val, -1, l4Byte13, GT_U8);
    F_NUMBER(val, -1, ipHeaderOk, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, isIpv6, GT_U8);
    FO_NUMBER(val, t, ipProtocol, GT_U32);
    FO_NUMBER(val, t, dscp, GT_U32);
    FO_NUMBER(val, t, isL4Valid, GT_U8);
    FO_NUMBER(val, t, l4Byte0, GT_U8);
    FO_NUMBER(val, t, l4Byte1, GT_U8);
    FO_NUMBER(val, t, l4Byte2, GT_U8);
    FO_NUMBER(val, t, l4Byte3, GT_U8);
    FO_NUMBER(val, t, l4Byte13, GT_U8);
    FO_NUMBER(val, t, ipHeaderOk, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC);

void prv_lua_to_c_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, ecmpStartIndex, GT_U32);
    F_NUMBER(val, -1, ecmpNumOfPaths, GT_U32);
    F_BOOL(val, -1, ecmpEnable);
}

void prv_c_to_lua_CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, ecmpStartIndex, GT_U32);
    FO_NUMBER(val, t, ecmpNumOfPaths, GT_U32);
    FO_BOOL(val, t, ecmpEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_L2_ECMP_LTT_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_1731_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_1731_CONFIG_STC *val
)
{
    F_ENUM(val, -1, mpType, CPSS_DXCH_OAM_SRV_MP_TYPE_ENT);
    F_NUMBER(val, -1, cfmOffset, GT_U32);
    F_STRUCT(val, -1, transmitParams, CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_1731_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_1731_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, mpType, CPSS_DXCH_OAM_SRV_MP_TYPE_ENT);
    FO_NUMBER(val, t, cfmOffset, GT_U32);
    FO_STRUCT(val, t, transmitParams, CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_1731_CONFIG_STC);

void prv_lua_to_c_CPSS_DXCH_BRIDGE_HOST_CNTR_STC(
    lua_State *L,
    CPSS_DXCH_BRIDGE_HOST_CNTR_STC *val
)
{
    F_NUMBER(val, -1, gtHostInPkts, GT_U32);
    F_NUMBER(val, -1, gtHostOutPkts, GT_U32);
    F_NUMBER(val, -1, gtHostOutBroadcastPkts, GT_U32);
    F_NUMBER(val, -1, gtHostOutMulticastPkts, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_BRIDGE_HOST_CNTR_STC(
    lua_State *L,
    CPSS_DXCH_BRIDGE_HOST_CNTR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, gtHostInPkts, GT_U32);
    FO_NUMBER(val, t, gtHostOutPkts, GT_U32);
    FO_NUMBER(val, t, gtHostOutBroadcastPkts, GT_U32);
    FO_NUMBER(val, t, gtHostOutMulticastPkts, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRIDGE_HOST_CNTR_STC);

enumDescr enumDescr_CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ACCUR_AWARE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E),
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_DEV_E),
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_1BIT_SRC_DEV_SRC_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_2BITS_SRC_DEV_SRC_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_3BITS_SRC_DEV_SRC_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_4BITS_SRC_DEV_SRC_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_5BITS_SRC_DEV_SRC_PORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC(
    lua_State *L,
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC *val
)
{
    F_STRUCT(val, -1, outputInterface, CPSS_DXCH_OUTPUT_INTERFACE_STC);
    F_BOOL(val, -1, egressVlanFilteringEnable);
    F_BOOL(val, -1, egressVlanTagStateEnable);
    F_ENUM(val, -1, egressVlanTagState, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
    F_NUMBER(val, -1, egressTagTpidIndex, GT_U32);
    F_BOOL(val, -1, assignVid0Enable);
    F_NUMBER(val, -1, vid0, GT_U16);
}

void prv_c_to_lua_CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC(
    lua_State *L,
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, outputInterface, CPSS_DXCH_OUTPUT_INTERFACE_STC);
    FO_BOOL(val, t, egressVlanFilteringEnable);
    FO_BOOL(val, t, egressVlanTagStateEnable);
    FO_ENUM(val, t, egressVlanTagState, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
    FO_NUMBER(val, t, egressTagTpidIndex, GT_U32);
    FO_BOOL(val, t, assignVid0Enable);
    FO_NUMBER(val, t, vid0, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC);

enumDescr enumDescr_CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_MULTICAST_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_MULTICAST_MLL_RPF_FAIL_COMMAND_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    F_STRUCT(val, -1, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPV6ADDR);
    F_NUMBER(val, -1, dipBits127to120, GT_U8);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_ARRAY_START(val, -1, udb6_11);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            F_ARRAY_NUMBER(val, udb6_11, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb6_11);
    F_NUMBER(val, -1, tag1Exist, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, trunkHash, GT_U32);
    F_ARRAY_START(val, -1, udb47_49);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, udb47_49, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb47_49);
    F_ARRAY_START(val, -1, udb0_4);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            F_ARRAY_NUMBER(val, udb0_4, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb0_4);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    FO_STRUCT(val, t, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC);
    FO_STRUCT(val, t, sip, GT_IPV6ADDR);
    FO_NUMBER(val, t, dipBits127to120, GT_U8);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_ARRAY_START(val, t, udb6_11);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            FO_ARRAY_NUMBER(val, udb6_11, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb6_11);
    FO_NUMBER(val, t, tag1Exist, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    FO_ARRAY_START(val, t, udb47_49);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, udb47_49, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb47_49);
    FO_ARRAY_START(val, t, udb0_4);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            FO_ARRAY_NUMBER(val, udb0_4, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb0_4);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC);

void prv_lua_to_c_CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC(
    lua_State *L,
    CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC *val
)
{
    F_NUMBER(val, -1, badLengthCells, GT_U32);
    F_NUMBER(val, -1, badHeaderCells, GT_U32);
    F_NUMBER(val, -1, reformatErrorCells, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC(
    lua_State *L,
    CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, badLengthCells, GT_U32);
    FO_NUMBER(val, t, badHeaderCells, GT_U32);
    FO_NUMBER(val, t, reformatErrorCells, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_FEC_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_FEC_MODE_ENABLED_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_FEC_MODE_DISABLED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_FEC_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_TRILL_RBID_TABLE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_TTI_TRILL_RBID_TABLE_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, srcTrgPort, GT_PORT_NUM);
    F_NUMBER_N(val, -1, trgHwDevice, trgDevice, GT_HW_DEV_NUM);
}

void prv_c_to_lua_CPSS_DXCH_TTI_TRILL_RBID_TABLE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_TTI_TRILL_RBID_TABLE_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, srcTrgPort, GT_PORT_NUM);
    FO_NUMBER_N(val, t, trgHwDevice, trgDevice, GT_HW_DEV_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_TRILL_RBID_TABLE_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_IP_ECMP_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IP_ECMP_ENTRY_STC *val
)
{
    F_BOOL(val, -1, randomEnable);
    F_NUMBER(val, -1, numOfPaths, GT_U32);
    F_NUMBER(val, -1, routeEntryBaseIndex, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IP_ECMP_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IP_ECMP_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, randomEnable);
    FO_NUMBER(val, t, numOfPaths, GT_U32);
    FO_NUMBER(val, t, routeEntryBaseIndex, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_ECMP_ENTRY_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_PIP_PROTOCOL_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROTOCOL_VLAN_TAG_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROTOCOL_MPLS_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROTOCOL_IPV4_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROTOCOL_IPV6_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_PROTOCOL_UDE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_PIP_PROTOCOL_ENT);

enumDescr enumDescr_CPSS_DXCH_FDB_QUEUE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_FDB_QUEUE_TYPE_AU_E),
    ENUM_ENTRY(CPSS_DXCH_FDB_QUEUE_TYPE_FU_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_FDB_QUEUE_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_MEMBER_SELECTION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_MEMBER_SELECTION_MODE_12_BITS_E),
    ENUM_ENTRY(CPSS_DXCH_MEMBER_SELECTION_MODE_6_LSB_E),
    ENUM_ENTRY(CPSS_DXCH_MEMBER_SELECTION_MODE_6_MSB_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_MEMBER_SELECTION_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_MPLS_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_MPLS_NOP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MPLS_SWAP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MPLS_POP1_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MPLS_POP2_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MPLS_POP3_CMD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_MPLS_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_TXQ_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_TM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_HEADER_ERROR_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_HEADER_ERROR_CHECKSUM_ENT),
    ENUM_ENTRY(CPSS_DXCH_IP_HEADER_ERROR_VERSION_ENT),
    ENUM_ENTRY(CPSS_DXCH_IP_HEADER_ERROR_LENGTH_ENT),
    ENUM_ENTRY(CPSS_DXCH_IP_HEADER_ERROR_SIP_DIP_ENT),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_HEADER_ERROR_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC *val
)
{
    F_NUMBER(val, -1, pclId, GT_U32);
    F_NUMBER(val, -1, macToMe, GT_U8);
    F_NUMBER(val, -1, sourcePort, GT_PHYSICAL_PORT_NUM);
    F_NUMBER(val, -1, sourceDevice, GT_U32);
    F_STRUCT(val, -1, portListBmp, CPSS_PORTS_BMP_STC);
    F_NUMBER(val, -1, isTagged, GT_U8);
    F_NUMBER(val, -1, vid, GT_U32);
    F_NUMBER(val, -1, up, GT_U32);
    F_NUMBER(val, -1, tos, GT_U32);
    F_NUMBER(val, -1, ipProtocol, GT_U32);
    F_NUMBER(val, -1, isL4Valid, GT_U8);
    F_NUMBER(val, -1, l4Byte0, GT_U8);
    F_NUMBER(val, -1, l4Byte1, GT_U8);
    F_NUMBER(val, -1, l4Byte2, GT_U8);
    F_NUMBER(val, -1, l4Byte3, GT_U8);
    F_NUMBER(val, -1, l4Byte13, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_NUMBER(val, t, macToMe, GT_U8);
    FO_NUMBER(val, t, sourcePort, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, sourceDevice, GT_U32);
    FO_STRUCT(val, t, portListBmp, CPSS_PORTS_BMP_STC);
    FO_NUMBER(val, t, isTagged, GT_U8);
    FO_NUMBER(val, t, vid, GT_U32);
    FO_NUMBER(val, t, up, GT_U32);
    FO_NUMBER(val, t, tos, GT_U32);
    FO_NUMBER(val, t, ipProtocol, GT_U32);
    FO_NUMBER(val, t, isL4Valid, GT_U8);
    FO_NUMBER(val, t, l4Byte0, GT_U8);
    FO_NUMBER(val, t, l4Byte1, GT_U8);
    FO_NUMBER(val, t, l4Byte2, GT_U8);
    FO_NUMBER(val, t, l4Byte3, GT_U8);
    FO_NUMBER(val, t, l4Byte13, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC *val
)
{
    F_STRUCT(val, -1, basicCounters, CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, basicCounters, CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC);

enumDescr enumDescr_CPSS_DXCH_LED_INDICATION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_PRIMARY_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_LINK_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_RX_ACT_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_TX_ACT_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_LOCAL_FAULT_OR_PCS_LINK_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_REMOTE_FAULT_OR_RESERVED_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_UKN_SEQ_OR_DUPLEX_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_P_REJ_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_RX_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_JABBER_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_FRAGMENT_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_CRC_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_FC_RX_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_TX_BAD_CRC_OR_LATE_COL_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_RX_BUFFER_FULL_OR_BACK_PR_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_WATCHDOG_EXP_SAMPLE_E),
    ENUM_ENTRY(CPSS_DXCH_LED_INDICATION_RX_TX_ACT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_LED_INDICATION_ENT);

enumDescr enumDescr_CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_LENGTH_CONSTANT_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_LENGTH_RANDOM_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_TM_GLUE_DROP_MASK_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DROP_MASK_STC *val
)
{
    F_BOOL(val, -1, qTailDropUnmask);
    F_BOOL(val, -1, qWredDropUnmask);
    F_BOOL(val, -1, aTailDropUnmask);
    F_BOOL(val, -1, aWredDropUnmask);
    F_BOOL(val, -1, bTailDropUnmask);
    F_BOOL(val, -1, bWredDropUnmask);
    F_BOOL(val, -1, cTailDropUnmask);
    F_BOOL(val, -1, cWredDropUnmask);
    F_BOOL(val, -1, portTailDropUnmask);
    F_BOOL(val, -1, portWredDropUnmask);
    F_BOOL(val, -1, outOfResourceDropUnmask);
    F_ENUM(val, -1, redPacketsDropMode, CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_TM_GLUE_DROP_MASK_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DROP_MASK_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, qTailDropUnmask);
    FO_BOOL(val, t, qWredDropUnmask);
    FO_BOOL(val, t, aTailDropUnmask);
    FO_BOOL(val, t, aWredDropUnmask);
    FO_BOOL(val, t, bTailDropUnmask);
    FO_BOOL(val, t, bWredDropUnmask);
    FO_BOOL(val, t, cTailDropUnmask);
    FO_BOOL(val, t, cWredDropUnmask);
    FO_BOOL(val, t, portTailDropUnmask);
    FO_BOOL(val, t, portWredDropUnmask);
    FO_BOOL(val, t, outOfResourceDropUnmask);
    FO_ENUM(val, t, redPacketsDropMode, CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_GLUE_DROP_MASK_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_UDB_SELECT_STC(
    lua_State *L,
    CPSS_DXCH_PCL_UDB_SELECT_STC *val
)
{
    F_ARRAY_START(val, -1, udbSelectArr);
    {
        int idx;
        for (idx = 0; idx < 50; idx++) {
            F_ARRAY_NUMBER(val, udbSelectArr, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, udbSelectArr);
    F_ARRAY_START(val, -1, ingrUdbReplaceArr);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            F_ARRAY_BOOL(val, ingrUdbReplaceArr, idx, GT_BOOL);
        }
    }
    F_ARRAY_END(val, -1, ingrUdbReplaceArr);
    F_BOOL(val, -1, egrUdb01Replace);
    F_BOOL(val, -1, egrUdbBit15Replace);
}

void prv_c_to_lua_CPSS_DXCH_PCL_UDB_SELECT_STC(
    lua_State *L,
    CPSS_DXCH_PCL_UDB_SELECT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, udbSelectArr);
    {
        int idx;
        for (idx = 0; idx < 50; idx++) {
            FO_ARRAY_NUMBER(val, udbSelectArr, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, udbSelectArr);
    FO_ARRAY_START(val, t, ingrUdbReplaceArr);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            FO_ARRAY_BOOL(val, ingrUdbReplaceArr, idx, GT_BOOL);
        }
    }
    FO_ARRAY_END(val, t, ingrUdbReplaceArr);
    FO_BOOL(val, t, egrUdb01Replace);
    FO_BOOL(val, t, egrUdbBit15Replace);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_UDB_SELECT_STC);

void prv_lua_to_c_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC *val
)
{
    F_BOOL(val, -1, enableWindow);
    F_NUMBER(val, -1, baseAddress, GT_U32);
    F_NUMBER(val, -1, windowSize, GT_U32);
    F_NUMBER(val, -1, remapAddress, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, enableWindow);
    FO_NUMBER(val, t, baseAddress, GT_U32);
    FO_NUMBER(val, t, windowSize, GT_U32);
    FO_NUMBER(val, t, remapAddress, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC);

void prv_lua_to_c_CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC *val
)
{
    F_ENUM(val, -1, algoType, CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT);
    F_BOOL(val, -1, performWriteLeveling);
}

void prv_c_to_lua_CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, algoType, CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT);
    FO_BOOL(val, t, performWriteLeveling);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC);

enumDescr enumDescr_CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_ECC_ECC7_E),
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_ECC_CRC8_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    F_NUMBER(val, -1, isIpv4, GT_U8);
    F_NUMBER(val, -1, etherType, GT_U16);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_NUMBER(val, -1, l2Encap, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_ARRAY_START(val, -1, udb15_17);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, udb15_17, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb15_17);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, trunkHash, GT_U32);
    F_NUMBER(val, -1, tag1Exist, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_ARRAY_START(val, -1, udb23_26);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb23_26, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb23_26);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    FO_NUMBER(val, t, isIpv4, GT_U8);
    FO_NUMBER(val, t, etherType, GT_U16);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_NUMBER(val, t, l2Encap, GT_U32);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_ARRAY_START(val, t, udb15_17);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, udb15_17, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb15_17);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    FO_NUMBER(val, t, tag1Exist, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_ARRAY_START(val, t, udb23_26);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb23_26, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb23_26);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC *val
)
{
    F_NUMBER(val, -1, isIpv6, GT_U8);
    F_NUMBER(val, -1, ipProtocol, GT_U32);
    F_NUMBER(val, -1, dscp, GT_U32);
    F_NUMBER(val, -1, isL4Valid, GT_U8);
    F_NUMBER(val, -1, l4Byte0, GT_U8);
    F_NUMBER(val, -1, l4Byte1, GT_U8);
    F_NUMBER(val, -1, l4Byte2, GT_U8);
    F_NUMBER(val, -1, l4Byte3, GT_U8);
    F_NUMBER(val, -1, l4Byte13, GT_U8);
    F_NUMBER(val, -1, egrTcpUdpPortComparator, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, isIpv6, GT_U8);
    FO_NUMBER(val, t, ipProtocol, GT_U32);
    FO_NUMBER(val, t, dscp, GT_U32);
    FO_NUMBER(val, t, isL4Valid, GT_U8);
    FO_NUMBER(val, t, l4Byte0, GT_U8);
    FO_NUMBER(val, t, l4Byte1, GT_U8);
    FO_NUMBER(val, t, l4Byte2, GT_U8);
    FO_NUMBER(val, t, l4Byte3, GT_U8);
    FO_NUMBER(val, t, l4Byte13, GT_U8);
    FO_NUMBER(val, t, egrTcpUdpPortComparator, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC);

enumDescr enumDescr_CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_COPY_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_INSERT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_20B_E),
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_24B_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TS_ACTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_NONE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_CAPTURE_INGRESS_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_INGRESS_TIME_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TS_ACTION_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_OFFSET_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_L2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_L3_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_L4_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_TUNNEL_L2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_TUNNEL_L3_MINUS_2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_METADATA_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OFFSET_INVALID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_OFFSET_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_EL_DB_OPERATION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_EL_DB_WRITE_OP_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_EL_DB_DELAY_OP_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_EL_DB_POLLING_OP_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_EL_DB_DUNIT_WRITE_OP_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_EL_DB_DUNIT_POLLING_OP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_EL_DB_OPERATION_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_BC_CHANGE_DISABLE_ALL_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SCHEDULER_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_AND_SCHEDULER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT);

void prv_lua_to_c_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *val
)
{
    F_STRUCT(val, -1, prefixesCfg, CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC);
    F_STRUCT(val, -1, tcamRange, CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC);
}

void prv_c_to_lua_CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, prefixesCfg, CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CAPACITY_STC);
    FO_STRUCT(val, t, tcamRange, CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC *val
)
{
    F_STRUCT(val, -1, egressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPV6ADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPV6ADDR);
    F_NUMBER(val, -1, isNd, GT_U8);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_NUMBER(val, -1, isVidx, GT_U8);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, srcPort, GT_U32);
    F_NUMBER(val, -1, trgPort, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_ARRAY_START(val, -1, udb1_4);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb1_4, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb1_4);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, egressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC);
    FO_STRUCT(val, t, sip, GT_IPV6ADDR);
    FO_STRUCT(val, t, dip, GT_IPV6ADDR);
    FO_NUMBER(val, t, isNd, GT_U8);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_NUMBER(val, t, isVidx, GT_U8);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, srcPort, GT_U32);
    FO_NUMBER(val, t, trgPort, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_ARRAY_START(val, t, udb1_4);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb1_4, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb1_4);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_FROM_IP_GEN_TS_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_FROM_PACKET_HASH_VALUE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT);

void prv_lua_to_c_CPSS_DIAG_PP_MEM_BIST_COMPARE_MEM_DATA_STC(
    lua_State *L,
    CPSS_DIAG_PP_MEM_BIST_COMPARE_MEM_DATA_STC *val
)
{
    F_NUMBER(val, -1, pclOpCode, GT_U32);
    F_NUMBER(val, -1, rtrOpCode, GT_U32);
    F_NUMBER(val, -1, expectedHitVal, GT_U32);
}

void prv_c_to_lua_CPSS_DIAG_PP_MEM_BIST_COMPARE_MEM_DATA_STC(
    lua_State *L,
    CPSS_DIAG_PP_MEM_BIST_COMPARE_MEM_DATA_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclOpCode, GT_U32);
    FO_NUMBER(val, t, rtrOpCode, GT_U32);
    FO_NUMBER(val, t, expectedHitVal, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DIAG_PP_MEM_BIST_COMPARE_MEM_DATA_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC *val
)
{
    F_BOOL(val, -1, assignSourcePortEnable);
    F_NUMBER(val, -1, sourcePortValue, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, assignSourcePortEnable);
    FO_NUMBER(val, t, sourcePortValue, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC);

enumDescr enumDescr_CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_ARP_BCAST_CMD_MODE_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_ARP_BCAST_CMD_MODE_VLAN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, queueIdBase, GT_U32);
    F_ARRAY_START(val, -1, bitSelectArr);
    {
        int idx;
        for (idx = 0; idx < 14; idx++) {
            F_ARRAY_STRUCT(val, bitSelectArr, idx, CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC);
        }
    }
    F_ARRAY_END(val, -1, bitSelectArr);
}

void prv_c_to_lua_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, queueIdBase, GT_U32);
    FO_ARRAY_START(val, t, bitSelectArr);
    {
        int idx;
        for (idx = 0; idx < 14; idx++) {
            FO_ARRAY_STRUCT(val, bitSelectArr, idx, CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC);
        }
    }
    FO_ARRAY_END(val, t, bitSelectArr);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC *val
)
{
    F_NUMBER(val, -1, errCounter, GT_U32);
    F_NUMBER(val, -1, lastFailedAddr, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, errCounter, GT_U32);
    FO_NUMBER(val, t, lastFailedAddr, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC);

enumDescr enumDescr_CPSS_DXCH_CNC_CLIENT_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_TTI_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_SRC_EPORT_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_EGRESS_TRG_EPORT_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_0_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_0_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_2_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_3_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_0_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_2_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_3_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_0_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_2_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_3_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_0_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_2_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_3_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_PACKET_TYPE_PASS_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_2_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_3_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_CLIENT_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_CLIENT_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E),
    ENUM_ENTRY(CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E),
    ENUM_ENTRY(CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E),
    ENUM_ENTRY(CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E),
    ENUM_ENTRY(CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E),
    ENUM_ENTRY(CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E),
    ENUM_ENTRY(CPSS_DXCH_IP_LPM_SHADOW_TYPE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_ACTION_UNT(
    lua_State *L,
    CPSS_DXCH_TTI_ACTION_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DXCH_TTI_ACTION_UNT(
    lua_State *L,
    CPSS_DXCH_TTI_ACTION_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, type1, CPSS_DXCH_TTI_ACTION_STC);
    FO_STRUCT(val, t, type2, CPSS_DXCH_TTI_ACTION_2_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_ACTION_UNT);

void prv_lua_to_c_CPSS_DXCH_PORT_ECN_ENABLERS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_ECN_ENABLERS_STC *val
)
{
    F_BOOL(val, -1, tcDpLimit);
    F_BOOL(val, -1, portLimit);
    F_BOOL(val, -1, tcLimit);
    F_BOOL(val, -1, sharedPoolLimit);
}

void prv_c_to_lua_CPSS_DXCH_PORT_ECN_ENABLERS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_ECN_ENABLERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tcDpLimit);
    FO_BOOL(val, t, portLimit);
    FO_BOOL(val, t, tcLimit);
    FO_BOOL(val, t, sharedPoolLimit);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_ECN_ENABLERS_STC);

void prv_lua_to_c_CPSS_DXCH_PORT_ALIGN90_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_ALIGN90_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, startAlign90, GT_U32);
    F_NUMBER(val, -1, rxTrainingCfg, GT_U32);
    F_NUMBER(val, -1, osDeltaMax, GT_U32);
    F_NUMBER(val, -1, adaptedFfeR, GT_U32);
    F_NUMBER(val, -1, adaptedFfeC, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_ALIGN90_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_ALIGN90_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, startAlign90, GT_U32);
    FO_NUMBER(val, t, rxTrainingCfg, GT_U32);
    FO_NUMBER(val, t, osDeltaMax, GT_U32);
    FO_NUMBER(val, t, adaptedFfeR, GT_U32);
    FO_NUMBER(val, t, adaptedFfeC, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_ALIGN90_PARAMS_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_COM_PHY_H_SUB_SEQ_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SD_RESET_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SD_UNRESET_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_RF_RESET_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_RF_UNRESET_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SYNCE_RESET_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SYNCE_UNRESET_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SERDES_POWER_UP_CTRL_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SERDES_POWER_DOWN_CTRL_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SERDES_RXINT_UP_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SERDES_RXINT_DOWN_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SERDES_WAIT_PLL_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_1_25G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_3_125G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_3_75G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_4_25G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_5G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_6_25G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_7_5G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_10_3125G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SD_LPBK_NORMAL_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SD_ANA_TX_2_RX_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SD_DIG_TX_2_RX_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SD_DIG_RX_2_TX_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_PT_AFTER_PATTERN_NORMAL_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_PT_AFTER_PATTERN_TEST_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_RX_TRAINING_ENABLE_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_RX_TRAINING_DISABLE_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_TX_TRAINING_ENABLE_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_TX_TRAINING_DISABLE_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_12_5G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_3_3G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_11_5625G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SERDES_PARTIAL_POWER_DOWN_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SERDES_PARTIAL_POWER_UP_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_11_25G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_CORE_RESET_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_CORE_UNRESET_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_FFE_TABLE_LR_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_FFE_TABLE_SR_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_10_9375G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_12_1875G_SEQ_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_5_625G_SEQ_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_COM_PHY_H_SUB_SEQ_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_MAC_DA_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_MAC_DA_AND_ETHERTYPE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_FIRST_MLL_E),
    ENUM_ENTRY(CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_SECOND_MLL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT);

enumDescr enumDescr_CPSS_DXCH_TRUNK_LBH_MASK_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_MAC_SA_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL1_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL2_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_IPV4_SIP_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_IPV6_DIP_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_IPV6_SIP_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_L4_DST_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_LBH_MASK_ENT);

enumDescr enumDescr_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E),
    ENUM_ENTRY(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E),
    ENUM_ENTRY(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_AND_INGRESS_WITHOUT_TAG1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC *val
)
{
    F_NUMBER(val, -1, pclId, GT_U32);
    F_NUMBER(val, -1, sourcePort, GT_PHYSICAL_PORT_NUM);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_NUMBER(val, -1, isL2Valid, GT_U8);
    F_NUMBER(val, -1, originalVid, GT_U32);
    F_NUMBER(val, -1, isSrcTrunk, GT_U8);
    F_NUMBER(val, -1, srcDevOrTrunkId, GT_U32);
    F_NUMBER_N(val, -1, srcHwDev, srcDev, GT_HW_DEV_NUM);
    F_NUMBER(val, -1, isIp, GT_U8);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_NUMBER(val, -1, tag1Exist, GT_U8);
    F_NUMBER(val, -1, sourceId, GT_U32);
    F_NUMBER(val, -1, tos, GT_U32);
    F_NUMBER(val, -1, ipProtocol, GT_U32);
    F_NUMBER(val, -1, ttl, GT_U32);
    F_NUMBER(val, -1, isL4Valid, GT_U8);
    F_NUMBER(val, -1, l4Byte0, GT_U8);
    F_NUMBER(val, -1, l4Byte1, GT_U8);
    F_NUMBER(val, -1, l4Byte2, GT_U8);
    F_NUMBER(val, -1, l4Byte3, GT_U8);
    F_NUMBER(val, -1, l4Byte13, GT_U8);
    F_NUMBER(val, -1, tcpUdpPortComparators, GT_U32);
    F_NUMBER(val, -1, tc, GT_U32);
    F_NUMBER(val, -1, dp, GT_U32);
    F_NUMBER(val, -1, egrPacketType, GT_U32);
    F_NUMBER(val, -1, srcTrgOrTxMirror, GT_U8);
    F_NUMBER(val, -1, assignedUp, GT_U32);
    F_NUMBER(val, -1, trgPhysicalPort, GT_PHYSICAL_PORT_NUM);
    F_NUMBER(val, -1, rxSniff, GT_U8);
    F_NUMBER(val, -1, isRouted, GT_U8);
    F_NUMBER(val, -1, isIpv6, GT_U8);
    F_NUMBER(val, -1, cpuCode, GT_U32);
    F_NUMBER(val, -1, srcTrg, GT_U8);
    F_NUMBER(val, -1, egrFilterEnable, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_NUMBER(val, t, sourcePort, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_NUMBER(val, t, isL2Valid, GT_U8);
    FO_NUMBER(val, t, originalVid, GT_U32);
    FO_NUMBER(val, t, isSrcTrunk, GT_U8);
    FO_NUMBER(val, t, srcDevOrTrunkId, GT_U32);
    FO_NUMBER_N(val, t, srcHwDev, srcDev, GT_HW_DEV_NUM);
    FO_NUMBER(val, t, isIp, GT_U8);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_NUMBER(val, t, tag1Exist, GT_U8);
    FO_NUMBER(val, t, sourceId, GT_U32);
    FO_NUMBER(val, t, tos, GT_U32);
    FO_NUMBER(val, t, ipProtocol, GT_U32);
    FO_NUMBER(val, t, ttl, GT_U32);
    FO_NUMBER(val, t, isL4Valid, GT_U8);
    FO_NUMBER(val, t, l4Byte0, GT_U8);
    FO_NUMBER(val, t, l4Byte1, GT_U8);
    FO_NUMBER(val, t, l4Byte2, GT_U8);
    FO_NUMBER(val, t, l4Byte3, GT_U8);
    FO_NUMBER(val, t, l4Byte13, GT_U8);
    FO_NUMBER(val, t, tcpUdpPortComparators, GT_U32);
    FO_NUMBER(val, t, tc, GT_U32);
    FO_NUMBER(val, t, dp, GT_U32);
    FO_NUMBER(val, t, egrPacketType, GT_U32);
    FO_NUMBER(val, t, srcTrgOrTxMirror, GT_U8);
    FO_NUMBER(val, t, assignedUp, GT_U32);
    FO_NUMBER(val, t, trgPhysicalPort, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, rxSniff, GT_U8);
    FO_NUMBER(val, t, isRouted, GT_U8);
    FO_NUMBER(val, t, isIpv6, GT_U8);
    FO_NUMBER(val, t, cpuCode, GT_U32);
    FO_NUMBER(val, t, srcTrg, GT_U8);
    FO_NUMBER(val, t, egrFilterEnable, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC);

enumDescr enumDescr_CPSS_DXCH_POLICER_MODIFY_UP_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_MODIFY_UP_KEEP_PREVIOUS_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MODIFY_UP_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MODIFY_UP_ENABLE_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MODIFY_UP_ENABLE_TAG0_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_MODIFY_UP_ENT);

enumDescr enumDescr_CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NST_PORT_ISOLATION_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_NST_PORT_ISOLATION_L2_ENABLE_E),
    ENUM_ENTRY(CPSS_DXCH_NST_PORT_ISOLATION_L3_ENABLE_E),
    ENUM_ENTRY(CPSS_DXCH_NST_PORT_ISOLATION_ALL_ENABLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_PORT_CNT_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_TRUNK_CNT_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_1B_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_16B_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT);

enumDescr enumDescr_CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_3_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_4_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_AVERAGE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_MAX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_AP_STATS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AP_STATS_STC *val
)
{
    F_NUMBER(val, -1, txDisCnt, GT_U16);
    F_NUMBER(val, -1, abilityCnt, GT_U16);
    F_NUMBER(val, -1, abilitySuccessCnt, GT_U16);
    F_NUMBER(val, -1, linkFailCnt, GT_U16);
    F_NUMBER(val, -1, linkSuccessCnt, GT_U16);
    F_NUMBER(val, -1, hcdResoultionTime, GT_U32);
    F_NUMBER(val, -1, linkUpTime, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_AP_STATS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AP_STATS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, txDisCnt, GT_U16);
    FO_NUMBER(val, t, abilityCnt, GT_U16);
    FO_NUMBER(val, t, abilitySuccessCnt, GT_U16);
    FO_NUMBER(val, t, linkFailCnt, GT_U16);
    FO_NUMBER(val, t, linkSuccessCnt, GT_U16);
    FO_NUMBER(val, t, hcdResoultionTime, GT_U32);
    FO_NUMBER(val, t, linkUpTime, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_AP_STATS_STC);

void prv_lua_to_c_CPSS_DXCH_IP_NAT44_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IP_NAT44_ENTRY_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_BOOL(val, -1, modifyDip);
    F_STRUCT_CUSTOM(val, -1, newDip, GT_IPADDR);
    F_BOOL(val, -1, modifySip);
    F_STRUCT_CUSTOM(val, -1, newSip, GT_IPADDR);
    F_BOOL(val, -1, modifyTcpUdpDstPort);
    F_NUMBER(val, -1, newTcpUdpDstPort, GT_U32);
    F_BOOL(val, -1, modifyTcpUdpSrcPort);
    F_NUMBER(val, -1, newTcpUdpSrcPort, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IP_NAT44_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IP_NAT44_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_BOOL(val, t, modifyDip);
    FO_STRUCT(val, t, newDip, GT_IPADDR);
    FO_BOOL(val, t, modifySip);
    FO_STRUCT(val, t, newSip, GT_IPADDR);
    FO_BOOL(val, t, modifyTcpUdpDstPort);
    FO_NUMBER(val, t, newTcpUdpDstPort, GT_U32);
    FO_BOOL(val, t, modifyTcpUdpSrcPort);
    FO_NUMBER(val, t, newTcpUdpSrcPort, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_NAT44_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *val
)
{
    F_BOOL(val, -1, tsTagParseEnable);
    F_BOOL(val, -1, hybridTsTagParseEnable);
    F_NUMBER(val, -1, tsTagEtherType, GT_U32);
    F_NUMBER(val, -1, hybridTsTagEtherType, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tsTagParseEnable);
    FO_BOOL(val, t, hybridTsTagParseEnable);
    FO_NUMBER(val, t, tsTagEtherType, GT_U32);
    FO_NUMBER(val, t, hybridTsTagEtherType, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, ipclConfigIndex, GT_U32);
    F_ENUM(val, -1, pcl0_1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    F_ENUM(val, -1, pcl1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
}

void prv_c_to_lua_CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, ipclConfigIndex, GT_U32);
    FO_ENUM(val, t, pcl0_1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    FO_ENUM(val, t, pcl1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    F_STRUCT(val, -1, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_NUMBER(val, -1, isBc, GT_U8);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPADDR);
    F_NUMBER(val, -1, l4Byte0, GT_U8);
    F_NUMBER(val, -1, l4Byte1, GT_U8);
    F_NUMBER(val, -1, l4Byte13, GT_U8);
    F_ARRAY_START(val, -1, udb20_22);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, udb20_22, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb20_22);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, trunkHash, GT_U32);
    F_ARRAY_START(val, -1, udb31_34);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb31_34, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb31_34);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    FO_STRUCT(val, t, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_NUMBER(val, t, isBc, GT_U8);
    FO_STRUCT(val, t, sip, GT_IPADDR);
    FO_STRUCT(val, t, dip, GT_IPADDR);
    FO_NUMBER(val, t, l4Byte0, GT_U8);
    FO_NUMBER(val, t, l4Byte1, GT_U8);
    FO_NUMBER(val, t, l4Byte13, GT_U8);
    FO_ARRAY_START(val, t, udb20_22);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, udb20_22, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb20_22);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    FO_ARRAY_START(val, t, udb31_34);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb31_34, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb31_34);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC);

void prv_lua_to_c_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC(
    lua_State *L,
    CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC *val
)
{
    F_NUMBER(val, -1, rulesUsed, GT_U32);
    F_NUMBER(val, -1, rulesFree, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC(
    lua_State *L,
    CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, rulesUsed, GT_U32);
    FO_NUMBER(val, t, rulesFree, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC);

enumDescr enumDescr_CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_METER_RESOLUTION_PACKETS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_NONE_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_TUNNEL_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_PASSENGER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_SERDES_TUNE_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_TUNE_STC *val
)
{
    F_NUMBER(val, -1, dfe, GT_U32);
    F_NUMBER(val, -1, ffeR, GT_U32);
    F_NUMBER(val, -1, ffeC, GT_U32);
    F_NUMBER(val, -1, sampler, GT_U32);
    F_NUMBER(val, -1, sqlch, GT_U32);
    F_NUMBER(val, -1, txEmphAmp, GT_U32);
    F_NUMBER(val, -1, txAmp, GT_U32);
    F_NUMBER(val, -1, txAmpAdj, GT_U32);
    F_NUMBER(val, -1, ffeS, GT_U32);
    F_BOOL(val, -1, txEmphEn);
    F_NUMBER(val, -1, txEmph1, GT_U32);
    F_NUMBER(val, -1, align90, GT_U32);
    F_BOOL(val, -1, txEmphEn1);
    F_BOOL(val, -1, txAmpShft);
    F_ARRAY_START(val, -1, dfeValsArray);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            F_ARRAY_NUMBER(val, dfeValsArray, idx, GT_32);
        }
    }
    F_ARRAY_END(val, -1, dfeValsArray);
    F_NUMBER(val, -1, DC, GT_U32);
    F_NUMBER(val, -1, LF, GT_U32);
    F_NUMBER(val, -1, HF, GT_U32);
    F_NUMBER(val, -1, BW, GT_U32);
    F_NUMBER(val, -1, LB, GT_U32);
    F_NUMBER(val, -1, EO, GT_U32);
    F_ARRAY_START(val, -1, DFE);
    {
        int idx;
        for (idx = 0; idx < 13; idx++) {
            F_ARRAY_NUMBER(val, DFE, idx, GT_32);
        }
    }
    F_ARRAY_END(val, -1, DFE);
}

void prv_c_to_lua_CPSS_DXCH_PORT_SERDES_TUNE_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_TUNE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, dfe, GT_U32);
    FO_NUMBER(val, t, ffeR, GT_U32);
    FO_NUMBER(val, t, ffeC, GT_U32);
    FO_NUMBER(val, t, sampler, GT_U32);
    FO_NUMBER(val, t, sqlch, GT_U32);
    FO_NUMBER(val, t, txEmphAmp, GT_U32);
    FO_NUMBER(val, t, txAmp, GT_U32);
    FO_NUMBER(val, t, txAmpAdj, GT_U32);
    FO_NUMBER(val, t, ffeS, GT_U32);
    FO_BOOL(val, t, txEmphEn);
    FO_NUMBER(val, t, txEmph1, GT_U32);
    FO_NUMBER(val, t, align90, GT_U32);
    FO_BOOL(val, t, txEmphEn1);
    FO_BOOL(val, t, txAmpShft);
    FO_ARRAY_START(val, t, dfeValsArray);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            FO_ARRAY_NUMBER(val, dfeValsArray, idx, GT_32);
        }
    }
    FO_ARRAY_END(val, t, dfeValsArray);
    FO_NUMBER(val, t, DC, GT_U32);
    FO_NUMBER(val, t, LF, GT_U32);
    FO_NUMBER(val, t, HF, GT_U32);
    FO_NUMBER(val, t, BW, GT_U32);
    FO_NUMBER(val, t, LB, GT_U32);
    FO_NUMBER(val, t, EO, GT_U32);
    FO_ARRAY_START(val, t, DFE);
    {
        int idx;
        for (idx = 0; idx < 13; idx++) {
            FO_ARRAY_NUMBER(val, DFE, idx, GT_32);
        }
    }
    FO_ARRAY_END(val, t, DFE);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_SERDES_TUNE_STC);

enumDescr enumDescr_CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT);

void prv_lua_to_c_CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC(
    lua_State *L,
    CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *val
)
{
    F_NUMBER(val, -1, type, GT_U32);
    F_NUMBER(val, -1, length, GT_U32);
    F_NUMBER(val, -1, version, GT_U32);
    F_NUMBER(val, -1, reserved1, GT_U32);
    F_NUMBER(val, -1, reserved2, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC(
    lua_State *L,
    CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, type, GT_U32);
    FO_NUMBER(val, t, length, GT_U32);
    FO_NUMBER(val, t, version, GT_U32);
    FO_NUMBER(val, t, reserved1, GT_U32);
    FO_NUMBER(val, t, reserved2, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC);

void prv_lua_to_c_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC *val
)
{
    F_ENUM(val, -1, billingIndexMode, CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT);
    F_NUMBER(val, -1, billingFlowIdIndexBase, GT_U32);
    F_NUMBER(val, -1, billingMinFlowId, GT_U32);
    F_NUMBER(val, -1, billingMaxFlowId, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, billingIndexMode, CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT);
    FO_NUMBER(val, t, billingFlowIdIndexBase, GT_U32);
    FO_NUMBER(val, t, billingMinFlowId, GT_U32);
    FO_NUMBER(val, t, billingMaxFlowId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC);

void prv_lua_to_c_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC(
    lua_State *L,
    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC *val
)
{
    F_ENUM(val, -1, unknownDaCommand, CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT);
    F_STRUCT(val, -1, nextRingInterface, CPSS_INTERFACE_INFO_STC);
}

void prv_c_to_lua_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC(
    lua_State *L,
    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, unknownDaCommand, CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT);
    FO_STRUCT(val, t, nextRingInterface, CPSS_INTERFACE_INFO_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC *val
)
{
    F_BOOL(val, -1, tsReceptionEnable);
    F_BOOL(val, -1, tsPiggyBackEnable);
    F_ENUM(val, -1, tsTagMode, CPSS_DXCH_PTP_TS_TAG_MODE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tsReceptionEnable);
    FO_BOOL(val, t, tsPiggyBackEnable);
    FO_ENUM(val, t, tsTagMode, CPSS_DXCH_PTP_TS_TAG_MODE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ETHERTYPE_AND_VID_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_DEFAULT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ONLY_FAILURES_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC *val
)
{
    F_NUMBER(val, -1, pclId, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_NUMBER(val, -1, vid, GT_U32);
    F_NUMBER(val, -1, srcPort, GT_U32);
    F_NUMBER(val, -1, trgPort, GT_U32);
    F_NUMBER(val, -1, srcDev, GT_U32);
    F_NUMBER(val, -1, trgDev, GT_U32);
    F_NUMBER(val, -1, localDevTrgPhyPort, GT_PHYSICAL_PORT_NUM);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_NUMBER(val, t, vid, GT_U32);
    FO_NUMBER(val, t, srcPort, GT_U32);
    FO_NUMBER(val, t, trgPort, GT_U32);
    FO_NUMBER(val, t, srcDev, GT_U32);
    FO_NUMBER(val, t, trgDev, GT_U32);
    FO_NUMBER(val, t, localDevTrgPhyPort, GT_PHYSICAL_PORT_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    F_NUMBER(val, -1, isIpv4, GT_U8);
    F_NUMBER(val, -1, etherType, GT_U16);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_NUMBER(val, -1, l2Encap, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_ARRAY_START(val, -1, udb0_3);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb0_3, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb0_3);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    FO_NUMBER(val, t, isIpv4, GT_U8);
    FO_NUMBER(val, t, etherType, GT_U16);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_NUMBER(val, t, l2Encap, GT_U32);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_ARRAY_START(val, t, udb0_3);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb0_3, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb0_3);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_EPORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC *val
)
{
    F_NUMBER(val, -1, invalidPtpPktCnt, GT_U32);
    F_NUMBER(val, -1, invalidOutPiggybackPktCnt, GT_U32);
    F_NUMBER(val, -1, invalidInPiggybackPktCnt, GT_U32);
    F_NUMBER(val, -1, invalidTsPktCnt, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, invalidPtpPktCnt, GT_U32);
    FO_NUMBER(val, t, invalidOutPiggybackPktCnt, GT_U32);
    FO_NUMBER(val, t, invalidInPiggybackPktCnt, GT_U32);
    FO_NUMBER(val, t, invalidTsPktCnt, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC);

enumDescr enumDescr_CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_COMPRESS_RANGE_UPDATE_METHOD_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_MOVE_BOTTOM_AND_COMPRESS_RANGE_UPDATE_METHOD_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_MOVE_MIDDLE_AND_COMPRESS_RANGE_UPDATE_METHOD_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_KEEP_OFFSETS_RANGE_UPDATE_METHOD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT);

void prv_lua_to_c_CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC *val
)
{
    F_ENUM(val, -1, vlanMode, CPSS_MAC_VL_ENT);
    F_ENUM(val, -1, size, CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT);
    F_BOOL(val, -1, fid16BitHashEn);
    F_ENUM(val, -1, crcHashUpperBitsMode, CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT);
    F_BOOL(val, -1, useZeroInitValueForCrcHash);
}

void prv_c_to_lua_CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, vlanMode, CPSS_MAC_VL_ENT);
    FO_ENUM(val, t, size, CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT);
    FO_BOOL(val, t, fid16BitHashEn);
    FO_ENUM(val, t, crcHashUpperBitsMode, CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT);
    FO_BOOL(val, t, useZeroInitValueForCrcHash);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC);

void prv_lua_to_c_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, l4DstPortMaskBmp, GT_U32);
    F_NUMBER(val, -1, l4SrcPortMaskBmp, GT_U32);
    F_NUMBER(val, -1, ipv6FlowMaskBmp, GT_U32);
    F_NUMBER(val, -1, ipDipMaskBmp, GT_U32);
    F_NUMBER(val, -1, ipSipMaskBmp, GT_U32);
    F_NUMBER(val, -1, macDaMaskBmp, GT_U32);
    F_NUMBER(val, -1, macSaMaskBmp, GT_U32);
    F_NUMBER(val, -1, mplsLabel0MaskBmp, GT_U32);
    F_NUMBER(val, -1, mplsLabel1MaskBmp, GT_U32);
    F_NUMBER(val, -1, mplsLabel2MaskBmp, GT_U32);
    F_NUMBER(val, -1, localSrcPortMaskBmp, GT_U32);
    F_NUMBER(val, -1, udbsMaskBmp, GT_U32);
    F_BOOL(val, -1, symmetricMacAddrEnable);
    F_BOOL(val, -1, symmetricIpv4AddrEnable);
    F_BOOL(val, -1, symmetricIpv6AddrEnable);
    F_BOOL(val, -1, symmetricL4PortEnable);
}

void prv_c_to_lua_CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, l4DstPortMaskBmp, GT_U32);
    FO_NUMBER(val, t, l4SrcPortMaskBmp, GT_U32);
    FO_NUMBER(val, t, ipv6FlowMaskBmp, GT_U32);
    FO_NUMBER(val, t, ipDipMaskBmp, GT_U32);
    FO_NUMBER(val, t, ipSipMaskBmp, GT_U32);
    FO_NUMBER(val, t, macDaMaskBmp, GT_U32);
    FO_NUMBER(val, t, macSaMaskBmp, GT_U32);
    FO_NUMBER(val, t, mplsLabel0MaskBmp, GT_U32);
    FO_NUMBER(val, t, mplsLabel1MaskBmp, GT_U32);
    FO_NUMBER(val, t, mplsLabel2MaskBmp, GT_U32);
    FO_NUMBER(val, t, localSrcPortMaskBmp, GT_U32);
    FO_NUMBER(val, t, udbsMaskBmp, GT_U32);
    FO_BOOL(val, t, symmetricMacAddrEnable);
    FO_BOOL(val, t, symmetricIpv4AddrEnable);
    FO_BOOL(val, t, symmetricIpv6AddrEnable);
    FO_BOOL(val, t, symmetricL4PortEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LB_CONFIG_STC *val
)
{
    F_BOOL(val, -1, outOfSequenceEnable);
    F_BOOL(val, -1, invalidTlvEnable);
    F_NUMBER(val, -1, tlvDataCrc32Val, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_LB_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LB_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, outOfSequenceEnable);
    FO_BOOL(val, t, invalidTlvEnable);
    FO_NUMBER(val, t, tlvDataCrc32Val, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_LB_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_SERVICE_IPV4_ENABLE_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_SERVICE_IPV6_ENABLE_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_SERVICE_ARP_ENABLE_DISABLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_POLICER_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_COUNTERS_STC *val
)
{
    F_NUMBER(val, -1, outOfProfileBytesCnt, GT_U32);
    F_NUMBER(val, -1, inProfileBytesCnt, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_POLICER_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, outOfProfileBytesCnt, GT_U32);
    FO_NUMBER(val, t, inProfileBytesCnt, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_POLICER_COUNTERS_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_64_CORE_CLOCKS_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_1024_CORE_CLOCKS_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_128_CORE_CLOCKS_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_256_CORE_CLOCKS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TS_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_RX_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TX_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_PACKET_TYPE_WAMP_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TS_PACKET_TYPE_RESERVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_MODIFY_DSCP_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_MODIFY_DSCP_KEEP_PREVIOUS_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MODIFY_DSCP_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MODIFY_DSCP_ENABLE_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_MODIFY_DSCP_ENABLE_INNER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_MODIFY_DSCP_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_TC_UP_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_DSCP_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_EXP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_CNT_SET_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_CNT_SET_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BM_CONTROL_ACCESS_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BM_LINKED_LIST_BUFFERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_FINAL_UDB_CLEAR_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MC_CLEAR_SHIFTER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MC_DIST_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_UC_DIST_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CNC_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CPFC_PFC_IND_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CTU_DBM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CTU_DESC_UNUSED_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPCL_CONF_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPCL_DATA_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPCL_EPLR_DESC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPCL_TCAM_DESC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_BILLING_EGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_IPFIX_AGING_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_IPFIX_WRAPAROUND_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_METERING_EGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_POLICER_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_QOS_REMARKING_TABLE_EGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_CONF_LIMIT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_CPU_CODE_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_QOS_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_RATE_LIMIT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_STC_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_TRUNK_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_VPM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_MAC_SA_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_TSARP_TUNNEL_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_VLAN_TRANSLATION_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_NHE_TABLE_AGING_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_NHE_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG2_DESC_RETURN_TWO_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG4_DESC_RETURN_TWO_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_UNUSED_DATA_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_VLAN_URPF_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_PCR_REGISTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_UNUSED_DATA_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_UPDATE_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_CONFI_PROCESSOR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_GRD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_SDMA_DESCRIPTOR_FILE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_SDMA_REG_FILE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_SDMA_RX_FIFO_2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_SDMA_TX_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_SDMA_WRR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_CONF_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_PCL2MLL_UNUSED_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_REPLICATIONS_IN_USE_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_TABLE_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MT_MAC_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MT_RF_TOP_MAC_UPD_OUT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PCL_CONFIG_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PCL_CRC_HASH_MASK_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PCL_LOOKUP_FIFO_0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PCL_LOOKUP_FIFO_1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PCL_LOOKUP_FIFO_2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PCL_UNUSED_DATA_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_BILLING_INGRESS_PLR0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_BILLING_INGRESS_PLR1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_DESCRIPTOR_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_IPFIX_AGING_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_IPFIX_WRAPAROUND_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_METERING_INGRESS_PLR0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_METERING_INGRESS_PLR1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_POLICER_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_QOS_REMARKING_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCC_LOWER_ACTION_TABLE_LOWER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCC_LOWER_ECC_TABLE_DATA_LOWER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCC_LOWER_ECC_TABLE_MASK_LOWER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCC_LOWER_LOWER_ANSWER_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCC_UPPER_ACTION_TABLE_UPPER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCC_UPPER_ECC_TABLE_DATA_UPPER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCC_UPPER_UPPER_ANSWER_FIFO_CL0_1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCC_UPPER_UPPER_ANSWER_FIFO_CL2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_LOOKUP_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_PORT_PROTOCOL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_PORT_VLAN_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_TXQ_E2E_FC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_UDB_CFG_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_UDB_PROFILE_ID_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_UNUSED_DATA_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_VLAN_TRANSLATION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_CPU_TX_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_DATA_FROM_MPPM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_INFO_DESC_PREFETCH_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_MEM_CLEAR_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_NEXT_LIST_BUFFERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_READ_BURST_STATE_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_TX_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DIST_BURST_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_STC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PRIO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_WRR_STATE_VARIABLES_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_DESIGNATED_TBL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_NON_TRUNK_MEMBER_TBL2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_NON_TRUNK_MEMBER_TBL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_SEC_TRG_TBL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_SST_TBL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_FREE_BUFS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_LINK_LIST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_QHEAD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_QTAIL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_BUFFER_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_DQ_Q_BUF_LIMIT_DP0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_DQ_Q_DESC_LIMIT_DP0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_EQ_Q_LIMIT_DP0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_EQ_Q_LIMIT_DP12_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_3_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_4_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_5_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_6_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_7_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_0_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_1_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_2_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_3_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_4_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_5_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_6_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_7_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_XOFF_THRES_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_XON_THRES_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_SAMPLE_INTERVALS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_SHARED_Q_LIMIT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_EGR_SPT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_EGR_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_ING_SPT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_ING_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_MAP_DEV_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L3_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_VIDX_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_WRDMA_IBUF_BANK_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LION2_LAST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_TRUNK_LTT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_INGRESS_EPORT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_EPORT_LTT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_L2ECMP_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_TX_PROTECTION_SWITCHING_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_OAM_INDEX_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_OAM_PROTECTION_LOC_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EQ_E2PHY_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_MT_MSG_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_MT_CNC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_CONFI_PROC_GP_SRAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_LTT_CONF_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_OAM_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_RAM_SOURCE_INTERFACE_EXCEPTION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_RAM_EXCESS_KEEPALIVE_EXCEPTION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_RAM_SUMMARY_EXCEPTION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_RAM_RDI_STATUS_CHANGE_EXCEPTION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_RAM_TX_PERIOD_EXCEPTION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_RAM_INVALID_KEEPALIVE_HASH_EXCEPTION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_RAM_MEL_EXCEPTION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_OPCODE_PROFILE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_OAM_INTERRUPT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_METERING_INGRESS_PLR2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_BILLING_INGRESS_PLR2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_SIP_PLR_HIER_TBL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_SIP_PLR_E_ATTR_TBL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_BC_UPD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_LINK_LIST_PTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BM_FREE_BUFFERS_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BM_IN_PROG_CT_CLEAR_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BM_NUMBER_OF_BUFFERS_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BM_VALID_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BM_RXDMA_NEXT_UPDATES_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BM_NEXT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_PORT_MAPPING_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_PFC_IND_FIFO_PIPE_X_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_QCN_BUFFER_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_QCN_SAMPLE_INTERVALS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_EFT_SEC_TRG_TBL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_EFT_LOCAL_MC_FIFO_0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_EFT_LOCAL_MC_FIFO_1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_EFT_LOCAL_MC_FIFO_2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_EFT_LOCAL_MC_FIFO_3_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_EFT_LOCAL_UC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_QAG_CPU_CODE_MAPPER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_QAG_TRG_PORT_MAPPER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_QAG_EVLAN_DESC_ATTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_QAG_EPORT_DESC_ATTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_QAG_FWD_FROM_CPU_LB_MAPPER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_DESIGNATED_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_MAP_DEV_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_EPORT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_EGR_VLAN_MEMBERS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_EGR_VLAN_ATTRIBUTE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_EGR_VLAN_SPANNING_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_VIDX_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_NON_TRUNK_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_NON_TRUNK_MEM2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_PORT_ISO_L2_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_PORT_ISO_L3_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_SST_TBL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_EPORT_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_EGR_SPT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EGF_SHT_VID_MAPPER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_ETH_TXFIFO_HEADER_LL_FREE_BUFS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_ETH_TXFIFO_HEADER_LL_LINK_LIST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_ETH_TXFIFO_PAYLOAD_LL_FREE_BUFS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_ETH_TXFIFO_PAYLOAD_LL_LINK_LIST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_ETH_TXFIFO_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_ETH_TXFIFO_IDDB_FREE_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_ETH_TXFIFO_DESCRIPTOR_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ILKN_TXFIFO_DATA_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ILKN_TXFIFO_DESCRIPTOR_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ILKN_TXFIFO_ALIGNER_ANS_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MA_ALIGNED_DATA_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MA_ALIGNED_CTRL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_PERCONFMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_TBMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_TBNEGMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_TBNEG2MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_WFSMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_NODEDWRR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_NODESTATE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_CLS0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_CLS1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_PRNTMAP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_GRNDPRNT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_LAST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_FUNC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHALVL_MYQ_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_PERCONFMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_TBMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_TBNEGMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_TBNEG2MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_WFSMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_NODEDWRR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_NODESTATE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_CLS0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_PRNTMAP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_GRNDPRNT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_LAST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_FUNC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBLVL_MYQ_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHCMDPIPE_NODEDWRR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHCMDPIPE_NODESTATE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHCMDPIPE_CLS0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHCMDPIPE_CLS1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHCMDPIPE_PRNTMAP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHCMDPIPE_GRNDPRNT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHCMDPIPE_LAST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHCMDPIPE_FUNC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CPERCONFMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CTBMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CNODEDWRR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CNODESTATE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CPRNTMAP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CLAST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CFUNC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CMYQ_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_PPERCONFMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_PTBMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_PNODEDWRR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_PFUNC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CTRLQ2AMAP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHPCCTRL_CTRLGRNDPRNT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBNK_PERCONFMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBNK_TBMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBNK_TBNEGMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBNK_TBNEG2MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMSCHBNK_WFSMEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_WR_DATA_BURST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_WR_CTRL_DATA_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_WR_CTRL_ADDR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_RD_CTRL_ADDR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_RD_DATA_PARAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_LL_PTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_LL_BAP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_LL_ATTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_LL_BVAL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_BAP_CHR_SERIAL_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMIG_L1_OVERHEAD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_PAGE_ENQ_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_PAGE_DEQ_CACHE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_PAGE_DEQ_CURR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_PAGE_DEQ_NEXT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_FREE_RD_HEAD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_FREE_WR_TAIL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_FREE_WR_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_PKG_FREE_WR_TAIL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PAGE_PKG_FREE_WR_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_PKG_BOTH_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_PKG_TAIL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_PKG_HEAD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_PAYLOAD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_PKT_ENQ_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_PKT_ENQ_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_PKT_ENQ_MSG_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_FREE_RD_HEAD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TM_QMR_PKTPKG_SYNC_DATA_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_QCOS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_QPROFPNTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_QAQL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_QDP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_QPROF_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_QCURVE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_APROFPNTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_AAQL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_ADP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_APROF_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_ACURVE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_BPROFPNTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_BAQL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_BDP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_BPROF_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_CPROFPNTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_CAQL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_CDP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_CPROF_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_PAQL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_PAQLCOS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_PDP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_PGDP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDRP_PPROF_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMMSG_QNODE_DP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMMSG_QNODE_STATE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMMSG_QCLS_0_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMMSG_QCLS_1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMMSG_PNODE_DP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMRCB_PORT_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMRCB_FIFO_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMRCB_CACHE_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMEG_AGING_PROFILES_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMEG_TMPORT2DMA_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMRCB_TX_SYNC_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMFCU_ETH_DMA_2TM_PORT_MAPPING_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMFCU_ILK_DMA_2TM_PORT_MAPPING_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMFCU_TCPORT2CNODE_MAPPING_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMFCU_INGRESS_TIMERS_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMFCU_PORT_TIMERS_CONF_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_HEADER_LL_FREE_BUFS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_HEADER_LL_LINK_LIST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_PAYLOAD_LL_FREE_BUFS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_PAYLOAD_LL_LINK_LIST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_PAYLOAD_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_PAYLOAD_IDDB_FREE_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_HEADER_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_HEADER_IDDB_FREE_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_CT_BC_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_CT_BC_IDDB_FREE_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXFIFO_DESCRIPTOR_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_CLEAR_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_DESC_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_DESC_IDDB_FREE_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_DESC_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HEADER_REORDER_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_PAYLOAD_REORDER_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_INTERNAL_DESC_IDDB_FREE_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_INTERNAL_DESC_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_NEXT_BC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_CT_START_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_PREF_MEM_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_NEXT_CT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_PAYLOAD_REORDER_MEM_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HEADER_REORDER_MEM_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_BANK_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_RX_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_TXDMA_RD_BURST_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PEX2_RXDATA_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PEX2_RXHDR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_XOR_XE_RAM128X72BE8_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RUNIT_NAND_RAM16X74_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RUNIT_NAND_RAM16X66_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_NF_NFU_RAM272X65_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_G_RAM16X65_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_G_RAM136X24BE5_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_G_RAM768X68BE16_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_G_RAM64X36BE8_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_G_RAM64X73_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_G_RAM80X65_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_G_SRAM2P1280X70_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ILKN_RX_SYNC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PR_EOP_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PR_DATA_MEMORY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RXDMA_IBUF_BANK_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RXDMA_IBUF_DESC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RXDMA_IBUF_HDR_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPCL_CFG_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPCL_UDB_SELECT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPCL_LOOKUP_LAT_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ERMRK_PTP_TIME_STAMP_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ERMRK_PTP_LOCAL_ACTION_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ERMRK_PTP_TARGET_PORT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ERMRK_PTP_SOURCE_PORT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ERMRK_QOS_MAP_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ERMRK_MODIFY_LAT_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ERMRK_PTP_INGRESS_TIMESTAMP_QUEUE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ERMRK_PTP_EGRESS_TIMESTAMP_QUEUE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_VLAN_MAC_SA_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_EVLAN_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_EPORT1_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_EPORT2_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_PHY1_PORT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_PHY2_PORT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_PTP_DOMAIN_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_GLOBAL_MAC_SA_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_GENERIC_TS_PROFILE_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_EPCL_UDB_CFG_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_HA_QOS_TO_EXP_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDROP_QUE_PROFILE_ID_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDROP_DESC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMDROP_DROP_MASK_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMQMAP_QUEUE_SELECTOR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMQMAP_TRG_DEV2INDEX_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TMQMAP_ENABLE_POLICY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCAM_PARITY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TCAM_ACTION_1P_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_PHY_PORT_ATTRIBUTES_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_DEFAULT_EPORT_ATTRIBUTES_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_EPORT_ATTRIBUTES_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_PCL_UDB_CFG_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_TTI_UDB_CFG_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_DSCP2DSCP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_DSCP2QOS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_UP_CFI2QOS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TTI_PTP_PKT_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MT_IN_NA_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MT_OUT_UPDATE_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_SMT_MAC_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_PORT_CONFIG_REG_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_EPORT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_EPORT_LEARN_PRIO_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_VLAN_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_PORT_MEMBER_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_SSG_IDX_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_SPAN_STATE_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_TRUNK_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_RATE_LIMIT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_STAGE2_LATENCY_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2I_STAGE2_REC_ANS_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PCL_LOOKUP_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PCL_UDB_SELECTION_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_EPORT_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_ACCESS_MATRIX_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_EVLAN_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_NHE_AGING_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_NHE_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_QOS_PROFILE_OFFESTS_TABLE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_STG5_DESC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_STG5_FDB_RETURN_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IPVX_STG7_DESC_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LPM_ECMP_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LPM_FCOE_VRF_ID_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LPM_IPV4_VRF_ID_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LPM_IPV6_VRF_ID_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LPM_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DCACHE_L0_DPARITY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DCACHE_L0_DATA_BANK_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DCACHE_L0_TAG_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DCACHE_L1_STATE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DCACHE_L1_DATA_BANK_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DCACHE_L1_DPARITY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DCACHE_L1_TAG_WAY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DCACHE_L1_ATTR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ICACHE_L1_DATA_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ICACHE_L0_DATA_BANK_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ICACHE_L0_TAG_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ICACHE_L1_TAG_WAY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IF_ID_IS_BPM_BANK_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MMU_TLB_DATA_WAY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MMU_TLB_TAG_WAY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CORESIGHT_CS_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CFU_SF_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CIB_WR_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2_DATA_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2_ECC_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2_PLRU_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_L2_TDV_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_D_RAM256X96BE11_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_D_RAM32X74_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_D_RAM32X82_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MG_MGCAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CPFC_IND_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RXDMA_COMMON_BUFFERS_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BOBCAT2_LAST_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_METERING_CONF_EGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_METERING_SIGN_TBL_EGRESS_EVEN_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_EPLR_METERING_SIGN_TBL_EGRESS_ODD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DUAL_TXDMA_ARB_HDR_OUT_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_METERING_CONF_INGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_METERING_SIGN_TBL_INGRESS_EVEN_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_METERING_SIGN_TBL_INGRESS_ODD_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_D_RAM512X96BE11_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_D_RAM64X74_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_D_RAM64X68BE33_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_D_RAM32X68BE16_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POE_ITCM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POE_DTCM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_USB_TX_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_USB_RX_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IHB_TX_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IHB_RX_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CIP_AMB_RAM128X74_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CIP_AMB_RAM32X69_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_CIP_AMB_RAM32X73_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_DDU_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RXDMA_IBUF_HDR_IDDB_MEM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RXDMA_IBUF_DESC_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_RXDMA_IBUF_HDR_IDDB_FREE_ID_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ALL_ZERO_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_FID_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_MAC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_OAM_LM_COUNTING_MODE_RETAIN_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_LM_COUNTING_MODE_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT(
    lua_State *L,
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT(
    lua_State *L,
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, ipv4Cfg, CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC);
    FO_STRUCT(val, t, mplsCfg, CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT);

enumDescr enumDescr_CPSS_DXCH_FDB_LEARN_PRIORITY_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_FDB_LEARN_PRIORITY_LOW_E),
    ENUM_ENTRY(CPSS_DXCH_FDB_LEARN_PRIORITY_HIGH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_FDB_LEARN_PRIORITY_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    F_STRUCT(val, -1, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPV6ADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPV6ADDR);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_ARRAY_START(val, -1, udb12_14);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, udb12_14, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb12_14);
    F_NUMBER(val, -1, tag1Exist, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, trunkHash, GT_U32);
    F_ARRAY_START(val, -1, udb15_22);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, udb15_22, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb15_22);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    FO_STRUCT(val, t, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC);
    FO_STRUCT(val, t, sip, GT_IPV6ADDR);
    FO_STRUCT(val, t, dip, GT_IPV6ADDR);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_ARRAY_START(val, t, udb12_14);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, udb12_14, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb12_14);
    FO_NUMBER(val, t, tag1Exist, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    FO_ARRAY_START(val, t, udb15_22);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, udb15_22, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb15_22);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC);

enumDescr enumDescr_CPSS_DXCH_OAM_OPCODE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_OAM_OPCODE_TYPE_LM_COUNTED_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_OPCODE_TYPE_LM_SINGLE_ENDED_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_OPCODE_TYPE_LM_DUAL_ENDED_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_OPCODE_TYPE_DM_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_OPCODE_TYPE_KEEPALIVE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_OAM_OPCODE_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC *val
)
{
    F_NUMBER(val, -1, csBitmask, GT_U32);
    F_BOOL(val, -1, mirrorEn);
    F_BOOL(val, -1, dqsSwapEn);
    F_BOOL(val, -1, ckSwapEn);
}

void prv_c_to_lua_CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, csBitmask, GT_U32);
    FO_BOOL(val, t, mirrorEn);
    FO_BOOL(val, t, dqsSwapEn);
    FO_BOOL(val, t, ckSwapEn);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC);

enumDescr enumDescr_CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_DONT_USE_SRC_INTERFACE_FIELDS_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_USE_SRC_INTERFACE_FIELDS_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_USE_SRC_INTERFACE_FIELDS_EXCLUDE_SRC_DEVICE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_OAM_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_OAM_ENTRY_STC *val
)
{
    F_BOOL(val, -1, opcodeParsingEnable);
    F_BOOL(val, -1, megLevelCheckEnable);
    F_NUMBER(val, -1, megLevel, GT_U32);
    F_NUMBER(val, -1, packetCommandProfile, GT_U32);
    F_NUMBER(val, -1, cpuCodeOffset, GT_U32);
    F_BOOL(val, -1, sourceInterfaceCheckEnable);
    F_ENUM(val, -1, sourceInterfaceCheckMode, CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT);
    F_STRUCT(val, -1, sourceInterface, CPSS_INTERFACE_INFO_STC);
    F_BOOL(val, -1, lmCounterCaptureEnable);
    F_BOOL(val, -1, dualEndedLmEnable);
    F_ENUM(val, -1, lmCountingMode, CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT);
    F_NUMBER(val, -1, oamPtpOffsetIndex, GT_U32);
    F_BOOL(val, -1, timestampEnable);
    F_BOOL(val, -1, keepaliveAgingEnable);
    F_NUMBER(val, -1, ageState, GT_U32);
    F_NUMBER(val, -1, agingPeriodIndex, GT_U32);
    F_NUMBER(val, -1, agingThreshold, GT_U32);
    F_BOOL(val, -1, hashVerifyEnable);
    F_BOOL(val, -1, lockHashValueEnable);
    F_BOOL(val, -1, protectionLocUpdateEnable);
    F_NUMBER(val, -1, flowHash, GT_U32);
    F_BOOL(val, -1, excessKeepaliveDetectionEnable);
    F_NUMBER(val, -1, excessKeepalivePeriodCounter, GT_U32);
    F_NUMBER(val, -1, excessKeepalivePeriodThreshold, GT_U32);
    F_NUMBER(val, -1, excessKeepaliveMessageCounter, GT_U32);
    F_NUMBER(val, -1, excessKeepaliveMessageThreshold, GT_U32);
    F_BOOL(val, -1, rdiCheckEnable);
    F_NUMBER(val, -1, rdiStatus, GT_U32);
    F_BOOL(val, -1, periodCheckEnable);
    F_NUMBER(val, -1, keepaliveTxPeriod, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_OAM_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_OAM_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, opcodeParsingEnable);
    FO_BOOL(val, t, megLevelCheckEnable);
    FO_NUMBER(val, t, megLevel, GT_U32);
    FO_NUMBER(val, t, packetCommandProfile, GT_U32);
    FO_NUMBER(val, t, cpuCodeOffset, GT_U32);
    FO_BOOL(val, t, sourceInterfaceCheckEnable);
    FO_ENUM(val, t, sourceInterfaceCheckMode, CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_ENT);
    FO_STRUCT(val, t, sourceInterface, CPSS_INTERFACE_INFO_STC);
    FO_BOOL(val, t, lmCounterCaptureEnable);
    FO_BOOL(val, t, dualEndedLmEnable);
    FO_ENUM(val, t, lmCountingMode, CPSS_DXCH_OAM_LM_COUNTING_MODE_ENT);
    FO_NUMBER(val, t, oamPtpOffsetIndex, GT_U32);
    FO_BOOL(val, t, timestampEnable);
    FO_BOOL(val, t, keepaliveAgingEnable);
    FO_NUMBER(val, t, ageState, GT_U32);
    FO_NUMBER(val, t, agingPeriodIndex, GT_U32);
    FO_NUMBER(val, t, agingThreshold, GT_U32);
    FO_BOOL(val, t, hashVerifyEnable);
    FO_BOOL(val, t, lockHashValueEnable);
    FO_BOOL(val, t, protectionLocUpdateEnable);
    FO_NUMBER(val, t, flowHash, GT_U32);
    FO_BOOL(val, t, excessKeepaliveDetectionEnable);
    FO_NUMBER(val, t, excessKeepalivePeriodCounter, GT_U32);
    FO_NUMBER(val, t, excessKeepalivePeriodThreshold, GT_U32);
    FO_NUMBER(val, t, excessKeepaliveMessageCounter, GT_U32);
    FO_NUMBER(val, t, excessKeepaliveMessageThreshold, GT_U32);
    FO_BOOL(val, t, rdiCheckEnable);
    FO_NUMBER(val, t, rdiStatus, GT_U32);
    FO_BOOL(val, t, periodCheckEnable);
    FO_NUMBER(val, t, keepaliveTxPeriod, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_ENTRY_STC);

enumDescr enumDescr_CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH3_POLICER_METER_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_METER_MODE_MEF0_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_METER_MODE_MEF1_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_METER_MODE_START_OF_ENVELOPE_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_METER_MODE_NOT_START_OF_ENVELOPE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_METER_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_AP_FLOW_CONTROL_SYMMETRIC_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_AP_FLOW_CONTROL_ASYMMETRIC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_TBL_SIZE_8K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_TBL_SIZE_16K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_TBL_SIZE_32K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_TBL_SIZE_64K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_TBL_SIZE_128K_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_TBL_SIZE_256K_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_MODIFY_UP_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MODIFY_UP_RESERVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_MODIFY_UP_ENT);

void prv_lua_to_c_CPSS_DXCH_PTP_TOD_COUNT_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TOD_COUNT_STC *val
)
{
    F_NUMBER(val, -1, nanoSeconds, GT_U32);
    F_STRUCT(val, -1, seconds, GT_U64);
    F_NUMBER(val, -1, fracNanoSeconds, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TOD_COUNT_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TOD_COUNT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, nanoSeconds, GT_U32);
    FO_STRUCT(val, t, seconds, GT_U64);
    FO_NUMBER(val, t, fracNanoSeconds, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TOD_COUNT_STC);

enumDescr enumDescr_CPSS_DXCH_MIN_SPEED_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_MIN_SPEED_INVALID_E),
    ENUM_ENTRY(CPSS_DXCH_MIN_SPEED_500_Mbps_E),
    ENUM_ENTRY(CPSS_DXCH_MIN_SPEED_1000_Mbps_E),
    ENUM_ENTRY(CPSS_DXCH_MIN_SPEED_2000_Mbps_E),
    ENUM_ENTRY(CPSS_DXCH_MIN_SPEED_5000_Mbps_E),
    ENUM_ENTRY(CPSS_DXCH_MIN_SPEED_10000_Mbps_E),
    ENUM_ENTRY(CPSS_DXCH_MIN_SPEED_MAX),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_MIN_SPEED_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_MODIFY_DSCP_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MODIFY_DSCP_RESERVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_MODIFY_DSCP_ENT);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC *val
)
{
    F_ENUM(val, -1, frameType, CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT);
    lua_getfield(L, -1, "frame");
    if (lua_istable(L, -1))
    {
        F_STRUCT(&(val->frame), -1, frame1731, CPSS_DXCH_OAM_SRV_1731_CONFIG_STC);
    }
    lua_pop(L, 1);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, frameType, CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT);
    {
        int t1;
        lua_newtable(L);
        t1 = lua_gettop(L);
            FO_STRUCT(&(val->frame), t1, frame1731, CPSS_DXCH_OAM_SRV_1731_CONFIG_STC);
        lua_setfield(L, t, "frame");
    }
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC);

enumDescr enumDescr_CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_DROP_ALL_RED_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ASK_TM_RESP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_BUFFERS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_BUFFERS_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_CFG_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_START_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_STATUS_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_RX_TRAINING_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TRAINING_STOP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_PTP_PLL_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_CORE_PLL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT);

enumDescr enumDescr_CPSS_DXCH_NET_DSA_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_CMD_TO_CPU_E),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_CMD_FORWARD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_NET_DSA_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_REGULAR_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_PASSENGER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_IP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_ETH_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_CW_ETH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_PASSENGER_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_IPV4_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_IPV6_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_IPV4V6_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PASSENGER_MPLS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_NEVER_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ALREADY_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_AFTER_DEFRAG_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_FROM_SCRATCH_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT);

void prv_lua_to_c_CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC(
    lua_State *L,
    CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC *val
)
{
    F_ENUM(val, -1, portTrunkCntMode, CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT);
    F_ENUM(val, -1, ipMode, CPSS_IP_PROTOCOL_STACK_ENT);
    F_ENUM(val, -1, vlanMode, CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT);
    F_NUMBER(val, -1, hwDevNum, GT_HW_DEV_NUM);
}

void prv_c_to_lua_CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC(
    lua_State *L,
    CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, portTrunkCntMode, CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT);
    FO_ENUM(val, t, ipMode, CPSS_IP_PROTOCOL_STACK_ENT);
    FO_ENUM(val, t, vlanMode, CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT);
    FO_NUMBER(val, t, hwDevNum, GT_HW_DEV_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC);

void prv_lua_to_c_CPSS_DXCH_PORT_AP_STATUS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AP_STATUS_STC *val
)
{
    F_NUMBER(val, -1, postApPortNum, GT_U32);
    F_STRUCT(val, -1, portMode, CPSS_PORT_MODE_SPEED_STC);
    F_BOOL(val, -1, hcdFound);
    F_BOOL(val, -1, fecEnabled);
    F_BOOL(val, -1, fcRxPauseEn);
    F_BOOL(val, -1, fcTxPauseEn);
}

void prv_c_to_lua_CPSS_DXCH_PORT_AP_STATUS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AP_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, postApPortNum, GT_U32);
    FO_STRUCT(val, t, portMode, CPSS_PORT_MODE_SPEED_STC);
    FO_BOOL(val, t, hcdFound);
    FO_BOOL(val, t, fecEnabled);
    FO_BOOL(val, t, fcRxPauseEn);
    FO_BOOL(val, t, fcTxPauseEn);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_AP_STATUS_STC);

void prv_lua_to_c_CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC(
    lua_State *L,
    CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, macAddrValue, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macAddrMask, GT_ETHERADDR);
}

void prv_c_to_lua_CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC(
    lua_State *L,
    CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, macAddrValue, GT_ETHERADDR);
    FO_STRUCT(val, t, macAddrMask, GT_ETHERADDR);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC);

enumDescr enumDescr_CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E),
    ENUM_ENTRY(CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_FAIL_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_NOT_COMPLITED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT);

enumDescr enumDescr_CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_LOCAL_TARGET_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_TARGET_DEV_MAP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC *val
)
{
    F_BOOL(val, -1, doIpUcRoute);
    F_NUMBER(val, -1, arpDaIndex, GT_U32);
    F_BOOL(val, -1, decrementTTL);
    F_BOOL(val, -1, bypassTTLCheck);
    F_BOOL(val, -1, icmpRedirectCheck);
}

void prv_c_to_lua_CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, doIpUcRoute);
    FO_NUMBER(val, t, arpDaIndex, GT_U32);
    FO_BOOL(val, t, decrementTTL);
    FO_BOOL(val, t, bypassTTLCheck);
    FO_BOOL(val, t, icmpRedirectCheck);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC *val
)
{
    F_NUMBER(val, -1, pclId, GT_U32);
    F_NUMBER(val, -1, macToMe, GT_U8);
    F_NUMBER(val, -1, sourcePort, GT_PHYSICAL_PORT_NUM);
    F_NUMBER(val, -1, sourceDevice, GT_U32);
    F_STRUCT(val, -1, portListBmp, CPSS_PORTS_BMP_STC);
    F_NUMBER(val, -1, isTagged, GT_U8);
    F_NUMBER(val, -1, vid, GT_U32);
    F_NUMBER(val, -1, up, GT_U32);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_NUMBER(val, -1, isIp, GT_U8);
    F_NUMBER(val, -1, isL2Valid, GT_U8);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_NUMBER(val, t, macToMe, GT_U8);
    FO_NUMBER(val, t, sourcePort, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, sourceDevice, GT_U32);
    FO_STRUCT(val, t, portListBmp, CPSS_PORTS_BMP_STC);
    FO_NUMBER(val, t, isTagged, GT_U8);
    FO_NUMBER(val, t, vid, GT_U32);
    FO_NUMBER(val, t, up, GT_U32);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_NUMBER(val, t, isIp, GT_U8);
    FO_NUMBER(val, t, isL2Valid, GT_U8);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);

void prv_lua_to_c_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC(
    lua_State *L,
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC *val
)
{
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_ENUM(val, -1, vlanIdAssignCmd, CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
    F_ENUM(val, -1, vlanIdAssignPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC(
    lua_State *L,
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_ENUM(val, t, vlanIdAssignCmd, CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
    FO_ENUM(val, t, vlanIdAssignPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC);

enumDescr enumDescr_CPSS_DXCH_LED_PORT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_LED_PORT_TYPE_TRI_SPEED_E),
    ENUM_ENTRY(CPSS_DXCH_LED_PORT_TYPE_XG_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_LED_PORT_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_TAG1_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_EVLAN_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ORIG_VLAN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ORIGINAL_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_LOCAL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC *val
)
{
    F_NUMBER(val, -1, portNum, GT_PHYSICAL_PORT_NUM);
    F_NUMBER(val, -1, laneNum, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, portNum, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, laneNum, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E),
    ENUM_ENTRY(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC(
    lua_State *L,
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *val
)
{
    F_ENUM(val, -1, portTrunkCntMode, CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT);
    F_ENUM(val, -1, ipMode, CPSS_IP_PROTOCOL_STACK_ENT);
    F_ENUM(val, -1, vlanMode, CPSS_DXCH_IP_VLAN_CNT_MODE_ENT);
    F_NUMBER_N(val, -1, hwDevNum, devNum, GT_HW_DEV_NUM);
}

void prv_c_to_lua_CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC(
    lua_State *L,
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, portTrunkCntMode, CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT);
    FO_ENUM(val, t, ipMode, CPSS_IP_PROTOCOL_STACK_ENT);
    FO_ENUM(val, t, vlanMode, CPSS_DXCH_IP_VLAN_CNT_MODE_ENT);
    FO_NUMBER_N(val, t, hwDevNum, devNum, GT_HW_DEV_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    F_STRUCT(val, -1, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_ARRAY_START(val, -1, udb18_19);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, udb18_19, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb18_19);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_ARRAY_START(val, -1, udb27_30);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb27_30, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb27_30);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC);
    FO_STRUCT(val, t, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_ARRAY_START(val, t, udb18_19);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, udb18_19, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb18_19);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_ARRAY_START(val, t, udb27_30);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb27_30, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb27_30);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC);

enumDescr enumDescr_CPSS_DXCH_POLICER_STAGE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_STAGE_INGRESS_0_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_STAGE_INGRESS_1_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_STAGE_EGRESS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_STAGE_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *val
)
{
    F_ENUM(val, -1, tsMode, CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT);
    F_NUMBER(val, -1, offsetProfile, GT_U32);
    F_BOOL(val, -1, OE);
    F_ENUM(val, -1, tsAction, CPSS_DXCH_PTP_TS_ACTION_ENT);
    F_ENUM(val, -1, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    F_ENUM(val, -1, ptpTransport, CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT);
    F_NUMBER(val, -1, offset, GT_U32);
    F_NUMBER(val, -1, ptpMessageType, GT_U32);
    F_NUMBER(val, -1, domain, GT_U32);
    F_BOOL(val, -1, ingrLinkDelayEnable);
    F_BOOL(val, -1, packetDispatchingEnable);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TS_CFG_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, tsMode, CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT);
    FO_NUMBER(val, t, offsetProfile, GT_U32);
    FO_BOOL(val, t, OE);
    FO_ENUM(val, t, tsAction, CPSS_DXCH_PTP_TS_ACTION_ENT);
    FO_ENUM(val, t, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    FO_ENUM(val, t, ptpTransport, CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT);
    FO_NUMBER(val, t, offset, GT_U32);
    FO_NUMBER(val, t, ptpMessageType, GT_U32);
    FO_NUMBER(val, t, domain, GT_U32);
    FO_BOOL(val, t, ingrLinkDelayEnable);
    FO_BOOL(val, t, packetDispatchingEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TS_CFG_ENTRY_STC);

enumDescr enumDescr_CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_256_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_512_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1K_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_4K_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_64K_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1M_E),
    ENUM_ENTRY(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_16M_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC *val
)
{
    F_NUMBER(val, -1, pclId, GT_U32);
    F_NUMBER(val, -1, macToMe, GT_U8);
    F_NUMBER(val, -1, sourcePort, GT_PHYSICAL_PORT_NUM);
    F_NUMBER(val, -1, sourceDevice, GT_U32);
    F_STRUCT(val, -1, portListBmp, CPSS_PORTS_BMP_STC);
    F_NUMBER(val, -1, vid, GT_U32);
    F_NUMBER(val, -1, up, GT_U32);
    F_NUMBER(val, -1, isIp, GT_U8);
    F_NUMBER(val, -1, dscpOrExp, GT_U32);
    F_NUMBER(val, -1, isL2Valid, GT_U8);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_NUMBER(val, -1, pktTagging, GT_U32);
    F_NUMBER(val, -1, l3OffsetInvalid, GT_U8);
    F_NUMBER(val, -1, l4ProtocolType, GT_U32);
    F_NUMBER(val, -1, pktType, GT_U32);
    F_NUMBER(val, -1, ipHeaderOk, GT_U8);
    F_NUMBER(val, -1, macDaType, GT_U32);
    F_NUMBER(val, -1, l4OffsetInvalid, GT_U8);
    F_NUMBER(val, -1, l2Encapsulation, GT_U32);
    F_NUMBER(val, -1, isIpv6Eh, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_NUMBER(val, t, macToMe, GT_U8);
    FO_NUMBER(val, t, sourcePort, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, sourceDevice, GT_U32);
    FO_STRUCT(val, t, portListBmp, CPSS_PORTS_BMP_STC);
    FO_NUMBER(val, t, vid, GT_U32);
    FO_NUMBER(val, t, up, GT_U32);
    FO_NUMBER(val, t, isIp, GT_U8);
    FO_NUMBER(val, t, dscpOrExp, GT_U32);
    FO_NUMBER(val, t, isL2Valid, GT_U8);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_NUMBER(val, t, pktTagging, GT_U32);
    FO_NUMBER(val, t, l3OffsetInvalid, GT_U8);
    FO_NUMBER(val, t, l4ProtocolType, GT_U32);
    FO_NUMBER(val, t, pktType, GT_U32);
    FO_NUMBER(val, t, ipHeaderOk, GT_U8);
    FO_NUMBER(val, t, macDaType, GT_U32);
    FO_NUMBER(val, t, l4OffsetInvalid, GT_U8);
    FO_NUMBER(val, t, l2Encapsulation, GT_U32);
    FO_NUMBER(val, t, isIpv6Eh, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC(
    lua_State *L,
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC *val
)
{
    F_ENUM(val, -1, ipclAccMode, CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT);
    F_ENUM(val, -1, ipclMaxDevPorts, CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT);
    F_ENUM(val, -1, ipclDevPortBase, CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT);
    F_ENUM(val, -1, epclAccMode, CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT);
    F_ENUM(val, -1, epclMaxDevPorts, CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT);
    F_ENUM(val, -1, epclDevPortBase, CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC(
    lua_State *L,
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, ipclAccMode, CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT);
    FO_ENUM(val, t, ipclMaxDevPorts, CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT);
    FO_ENUM(val, t, ipclDevPortBase, CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT);
    FO_ENUM(val, t, epclAccMode, CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT);
    FO_ENUM(val, t, epclMaxDevPorts, CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT);
    FO_ENUM(val, t, epclDevPortBase, CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC);

enumDescr enumDescr_CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_TDM_E),
    ENUM_ENTRY(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_SP_INGRESS_EGRESS_MIRROR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_TOTAL_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V1_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V2_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_Y1731_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TS_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_RX_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TX_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_WAMP_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NONE_ACTION_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_FORWARD_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_CORR_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_CORR_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_INGRESS_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_INGRESS_TIME_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_INGRESS_TIME_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_L2_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_IPV4_UDP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_IPV6_UDP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_RESERVED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_TAG0_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH_CFG_ROUTING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_ROUTER_BASED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CFG_ROUTING_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC *val
)
{
    F_NUMBER(val, -1, egrMirrorDropCntr, GT_U32);
    F_NUMBER(val, -1, egrStcDropCntr, GT_U32);
    F_NUMBER(val, -1, egrQcnDropCntr, GT_U32);
    F_NUMBER(val, -1, dropPfcEventsCntr, GT_U32);
    F_NUMBER(val, -1, clearPacketsDroppedCounter, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, egrMirrorDropCntr, GT_U32);
    FO_NUMBER(val, t, egrStcDropCntr, GT_U32);
    FO_NUMBER(val, t, egrQcnDropCntr, GT_U32);
    FO_NUMBER(val, t, dropPfcEventsCntr, GT_U32);
    FO_NUMBER(val, t, clearPacketsDroppedCounter, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_STAT_TX_DROP_COUNTERS_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, tokensRate, GT_U32);
    F_NUMBER(val, -1, slowRateRatio, GT_U32);
    F_ENUM(val, -1, tokensRateGran, CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT);
    F_NUMBER(val, -1, portsPacketLength, GT_U32);
    F_NUMBER(val, -1, cpuPacketLength, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, tokensRate, GT_U32);
    FO_NUMBER(val, t, slowRateRatio, GT_U32);
    FO_ENUM(val, t, tokensRateGran, CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT);
    FO_NUMBER(val, t, portsPacketLength, GT_U32);
    FO_NUMBER(val, t, cpuPacketLength, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_PORT_CRC_TWO_BYTES_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_PORT_CRC_THREE_BYTES_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC *val
)
{
    F_ARRAY_START(val, -1, udb);
    {
        int idx;
        for (idx = 0; idx < 50; idx++) {
            F_ARRAY_NUMBER(val, udb, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb);
    F_STRUCT(val, -1, replacedFld, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC);
    F_STRUCT(val, -1, udb60FixedFld, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, udb);
    {
        int idx;
        for (idx = 0; idx < 50; idx++) {
            FO_ARRAY_NUMBER(val, udb, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb);
    FO_STRUCT(val, t, replacedFld, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC);
    FO_STRUCT(val, t, udb60FixedFld, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC);

void prv_lua_to_c_CPSS_DXCH_IPV6_PREFIX_STC(
    lua_State *L,
    CPSS_DXCH_IPV6_PREFIX_STC *val
)
{
    F_NUMBER(val, -1, vrId, GT_U32);
    F_STRUCT_CUSTOM(val, -1, ipAddr, GT_IPV6ADDR);
    F_BOOL(val, -1, isMcSource);
    F_NUMBER(val, -1, mcGroupIndexRow, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IPV6_PREFIX_STC(
    lua_State *L,
    CPSS_DXCH_IPV6_PREFIX_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, vrId, GT_U32);
    FO_STRUCT(val, t, ipAddr, GT_IPV6ADDR);
    FO_BOOL(val, t, isMcSource);
    FO_NUMBER(val, t, mcGroupIndexRow, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IPV6_PREFIX_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_TAI_ID_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TAI_ID_STC *val
)
{
    F_ENUM(val, -1, taiInstance, CPSS_DXCH_PTP_TAI_INSTANCE_ENT);
    F_ENUM(val, -1, taiNumber, CPSS_DXCH_PTP_TAI_NUMBER_ENT);
    F_NUMBER(val, -1, portNum, GT_PHYSICAL_PORT_NUM);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TAI_ID_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TAI_ID_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, taiInstance, CPSS_DXCH_PTP_TAI_INSTANCE_ENT);
    FO_ENUM(val, t, taiNumber, CPSS_DXCH_PTP_TAI_NUMBER_ENT);
    FO_NUMBER(val, t, portNum, GT_PHYSICAL_PORT_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TAI_ID_STC);

void prv_lua_to_c_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC(
    lua_State *L,
    CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *val
)
{
    F_NUMBER(val, -1, etherType, GT_U16);
    F_NUMBER(val, -1, vpt, GT_U8);
    F_NUMBER(val, -1, cfi, GT_U8);
    F_NUMBER(val, -1, vid, GT_U16);
}

void prv_c_to_lua_CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC(
    lua_State *L,
    CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, etherType, GT_U16);
    FO_NUMBER(val, t, vpt, GT_U8);
    FO_NUMBER(val, t, cfi, GT_U8);
    FO_NUMBER(val, t, vid, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC);

enumDescr enumDescr_CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_SRC_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_CPU_CODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_STORM_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_STORM_TYPE_UC_KNOWN_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_STORM_TYPE_UC_UNKNOWN_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_STORM_TYPE_MC_UNREGISTERED_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_STORM_TYPE_MC_REGISTERED_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_STORM_TYPE_BC_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_STORM_TYPE_TCP_SYN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_STORM_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC *val
)
{
    F_BOOL(val, -1, tagEnable);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_ENUM(val, -1, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, up, GT_U32);
    F_ENUM(val, -1, dscpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, dscp, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_NUMBER(val, -1, ttl, GT_U32);
    F_STRUCT_CUSTOM(val, -1, destIp, GT_IPV6ADDR);
    F_STRUCT_CUSTOM(val, -1, srcIp, GT_IPV6ADDR);
    F_BOOL(val, -1, retainCRC);
    F_ENUM(val, -1, ipHeaderProtocol, CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
    F_NUMBER(val, -1, profileIndex, GT_U32);
    F_NUMBER(val, -1, greProtocolForEthernet, GT_U32);
    F_NUMBER(val, -1, udpDstPort, GT_U32);
    F_NUMBER(val, -1, greFlagsAndVersion, GT_U32);
    F_NUMBER(val, -1, udpSrcPort, GT_U32);
    F_ENUM(val, -1, flowLabelMode, CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tagEnable);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_ENUM(val, t, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, up, GT_U32);
    FO_ENUM(val, t, dscpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, dscp, GT_U32);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_NUMBER(val, t, ttl, GT_U32);
    FO_STRUCT(val, t, destIp, GT_IPV6ADDR);
    FO_STRUCT(val, t, srcIp, GT_IPV6ADDR);
    FO_BOOL(val, t, retainCRC);
    FO_ENUM(val, t, ipHeaderProtocol, CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
    FO_NUMBER(val, t, profileIndex, GT_U32);
    FO_NUMBER(val, t, greProtocolForEthernet, GT_U32);
    FO_NUMBER(val, t, udpDstPort, GT_U32);
    FO_NUMBER(val, t, greFlagsAndVersion, GT_U32);
    FO_NUMBER(val, t, udpSrcPort, GT_U32);
    FO_ENUM(val, t, flowLabelMode, CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_START_IPV6_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_TRANSLATION_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_TRANSLATION_VID0_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_VID1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT);

void prv_lua_to_c_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC(
    lua_State *L,
    CPSS_DXCH_CSCD_QOS_TC_REMAP_STC *val
)
{
    F_NUMBER(val, -1, forwardLocalTc, GT_U32);
    F_NUMBER(val, -1, forwardStackTc, GT_U32);
    F_NUMBER(val, -1, toAnalyzerLocalTc, GT_U32);
    F_NUMBER(val, -1, toAnalyzerStackTc, GT_U32);
    F_NUMBER(val, -1, toCpuLocalTc, GT_U32);
    F_NUMBER(val, -1, toCpuStackTc, GT_U32);
    F_NUMBER(val, -1, fromCpuLocalTc, GT_U32);
    F_NUMBER(val, -1, fromCpuStackTc, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_CSCD_QOS_TC_REMAP_STC(
    lua_State *L,
    CPSS_DXCH_CSCD_QOS_TC_REMAP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, forwardLocalTc, GT_U32);
    FO_NUMBER(val, t, forwardStackTc, GT_U32);
    FO_NUMBER(val, t, toAnalyzerLocalTc, GT_U32);
    FO_NUMBER(val, t, toAnalyzerStackTc, GT_U32);
    FO_NUMBER(val, t, toCpuLocalTc, GT_U32);
    FO_NUMBER(val, t, toCpuStackTc, GT_U32);
    FO_NUMBER(val, t, fromCpuLocalTc, GT_U32);
    FO_NUMBER(val, t, fromCpuStackTc, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_CSCD_QOS_TC_REMAP_STC);

enumDescr enumDescr_CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_ENTRY_METERING_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_ENTRY_BILLING_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_MESSAGE_TYPE_QCN_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_CN_MESSAGE_TYPE_CCFC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_L2_MLL_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_L2_MLL_ENTRY_STC *val
)
{
    F_BOOL(val, -1, last);
    F_BOOL(val, -1, unknownUcFilterEnable);
    F_BOOL(val, -1, unregMcFilterEnable);
    F_BOOL(val, -1, bcFilterEnable);
    F_BOOL(val, -1, mcLocalSwitchingEnable);
    F_BOOL(val, -1, maxHopCountEnable);
    F_NUMBER(val, -1, maxOutgoingHopCount, GT_U32);
    F_STRUCT(val, -1, egressInterface, CPSS_INTERFACE_INFO_STC);
    F_NUMBER(val, -1, maskBitmap, GT_U32);
    F_NUMBER(val, -1, ttlThreshold, GT_U32);
    F_BOOL(val, -1, bindToMllCounterEnable);
    F_NUMBER(val, -1, mllCounterIndex, GT_U32);
    F_BOOL(val, -1, onePlusOneFilteringEnable);
    F_NUMBER(val, -1, meshId, GT_U32);
    F_BOOL(val, -1, tunnelStartEnable);
    F_NUMBER(val, -1, tunnelStartPointer, GT_U32);
    F_ENUM(val, -1, tunnelStartPassengerType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_L2_MLL_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_L2_MLL_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, last);
    FO_BOOL(val, t, unknownUcFilterEnable);
    FO_BOOL(val, t, unregMcFilterEnable);
    FO_BOOL(val, t, bcFilterEnable);
    FO_BOOL(val, t, mcLocalSwitchingEnable);
    FO_BOOL(val, t, maxHopCountEnable);
    FO_NUMBER(val, t, maxOutgoingHopCount, GT_U32);
    FO_STRUCT(val, t, egressInterface, CPSS_INTERFACE_INFO_STC);
    FO_NUMBER(val, t, maskBitmap, GT_U32);
    FO_NUMBER(val, t, ttlThreshold, GT_U32);
    FO_BOOL(val, t, bindToMllCounterEnable);
    FO_NUMBER(val, t, mllCounterIndex, GT_U32);
    FO_BOOL(val, t, onePlusOneFilteringEnable);
    FO_NUMBER(val, t, meshId, GT_U32);
    FO_BOOL(val, t, tunnelStartEnable);
    FO_NUMBER(val, t, tunnelStartPointer, GT_U32);
    FO_ENUM(val, t, tunnelStartPassengerType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_L2_MLL_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC *val
)
{
    F_ARRAY_START(val, -1, busParams);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_STRUCT(val, busParams, idx, CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC);
        }
    }
    F_ARRAY_END(val, -1, busParams);
    F_ENUM(val, -1, speedBin, CPSS_DRAM_SPEED_BIN_ENT);
    F_ENUM(val, -1, busWidth, CPSS_DRAM_BUS_WIDTH_ENT);
    F_ENUM(val, -1, memorySize, CPSS_DRAM_SIZE_ENT);
    F_ENUM(val, -1, memoryFreq, CPSS_DRAM_FREQUENCY_ENT);
    F_NUMBER(val, -1, casWL, GT_U32);
    F_NUMBER(val, -1, casL, GT_U32);
    F_ENUM(val, -1, interfaceTemp, CPSS_DRAM_TEMPERATURE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, busParams);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_STRUCT(val, busParams, idx, CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC);
        }
    }
    FO_ARRAY_END(val, t, busParams);
    FO_ENUM(val, t, speedBin, CPSS_DRAM_SPEED_BIN_ENT);
    FO_ENUM(val, t, busWidth, CPSS_DRAM_BUS_WIDTH_ENT);
    FO_ENUM(val, t, memorySize, CPSS_DRAM_SIZE_ENT);
    FO_ENUM(val, t, memoryFreq, CPSS_DRAM_FREQUENCY_ENT);
    FO_NUMBER(val, t, casWL, GT_U32);
    FO_NUMBER(val, t, casL, GT_U32);
    FO_ENUM(val, t, interfaceTemp, CPSS_DRAM_TEMPERATURE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC);

void prv_lua_to_c_CPSS_DXCH_CFG_GLOBAL_EPORT_STC(
    lua_State *L,
    CPSS_DXCH_CFG_GLOBAL_EPORT_STC *val
)
{
    F_BOOL(val, -1, enable);
    F_NUMBER(val, -1, pattern, GT_U32);
    F_NUMBER(val, -1, mask, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_CFG_GLOBAL_EPORT_STC(
    lua_State *L,
    CPSS_DXCH_CFG_GLOBAL_EPORT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, enable);
    FO_NUMBER(val, t, pattern, GT_U32);
    FO_NUMBER(val, t, mask, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_CFG_GLOBAL_EPORT_STC);

enumDescr enumDescr_CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_MT_UC_SCHED_MTU_2K_E),
    ENUM_ENTRY(CPSS_DXCH_IP_MT_UC_SCHED_MTU_8K_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC *val
)
{
    F_BOOL(val, -1, toCpuLossyDropEnable);
    F_BOOL(val, -1, toTargetSnifferLossyDropEnable);
    F_BOOL(val, -1, fromCpuLossyDropEnable);
}

void prv_c_to_lua_CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, toCpuLossyDropEnable);
    FO_BOOL(val, t, toTargetSnifferLossyDropEnable);
    FO_BOOL(val, t, fromCpuLossyDropEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_VID1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_BRG_SERVICE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_HEADER_CHECK_BRG_SERVICE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_UC_RPF_CHECK_BRG_SERVICE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_SIP_SA_CHECK_BRG_SERVICE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_SIP_FILTER_BRG_SERVICE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_BRG_SERVICE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_BRG_SERVICE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_KEY_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_IPV4_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_MPLS_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_ETH_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_MIM_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_MPLS_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_IPV6_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_UDE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_UDE1_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_UDE2_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_UDE3_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_UDE4_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_UDE5_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_KEY_UDB_UDE6_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_KEY_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_VLAN_CNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_VLAN_CNT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC *val
)
{
    F_BOOL(val, -1, tagEnable);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_ENUM(val, -1, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, up, GT_U32);
    F_ENUM(val, -1, dscpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, dscp, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_BOOL(val, -1, dontFragmentFlag);
    F_NUMBER(val, -1, ttl, GT_U32);
    F_BOOL(val, -1, autoTunnel);
    F_NUMBER(val, -1, autoTunnelOffset, GT_U32);
    F_STRUCT_CUSTOM(val, -1, destIp, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, srcIp, GT_IPADDR);
    F_NUMBER(val, -1, cfi, GT_U32);
    F_BOOL(val, -1, retainCRC);
    F_ENUM(val, -1, ipHeaderProtocol, CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
    F_NUMBER(val, -1, profileIndex, GT_U32);
    F_NUMBER(val, -1, greProtocolForEthernet, GT_U32);
    F_NUMBER(val, -1, greFlagsAndVersion, GT_U32);
    F_NUMBER(val, -1, udpDstPort, GT_U32);
    F_NUMBER(val, -1, udpSrcPort, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tagEnable);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_ENUM(val, t, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, up, GT_U32);
    FO_ENUM(val, t, dscpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, dscp, GT_U32);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_BOOL(val, t, dontFragmentFlag);
    FO_NUMBER(val, t, ttl, GT_U32);
    FO_BOOL(val, t, autoTunnel);
    FO_NUMBER(val, t, autoTunnelOffset, GT_U32);
    FO_STRUCT(val, t, destIp, GT_IPADDR);
    FO_STRUCT(val, t, srcIp, GT_IPADDR);
    FO_NUMBER(val, t, cfi, GT_U32);
    FO_BOOL(val, t, retainCRC);
    FO_ENUM(val, t, ipHeaderProtocol, CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);
    FO_NUMBER(val, t, profileIndex, GT_U32);
    FO_NUMBER(val, t, greProtocolForEthernet, GT_U32);
    FO_NUMBER(val, t, greFlagsAndVersion, GT_U32);
    FO_NUMBER(val, t, udpDstPort, GT_U32);
    FO_NUMBER(val, t, udpSrcPort, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC);

enumDescr enumDescr_CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_BIST_PURE_MEMORY_TEST_E),
    ENUM_ENTRY(CPSS_DIAG_PP_MEM_BIST_COMPARE_TEST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_ETH_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_ETH_RULE_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    F_NUMBER(val, -1, up0, GT_U32);
    F_NUMBER(val, -1, cfi0, GT_U32);
    F_BOOL(val, -1, isVlan1Exists);
    F_NUMBER(val, -1, vid1, GT_U16);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, etherType, GT_U32);
    F_BOOL(val, -1, macToMe);
    F_NUMBER(val, -1, srcId, GT_U32);
    F_NUMBER(val, -1, dsaQosProfile, GT_U32);
    F_NUMBER(val, -1, tag0TpidIndex, GT_U32);
    F_NUMBER(val, -1, tag1TpidIndex, GT_U32);
    F_NUMBER(val, -1, eTagGrp, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TTI_ETH_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_ETH_RULE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    FO_NUMBER(val, t, up0, GT_U32);
    FO_NUMBER(val, t, cfi0, GT_U32);
    FO_BOOL(val, t, isVlan1Exists);
    FO_NUMBER(val, t, vid1, GT_U16);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, etherType, GT_U32);
    FO_BOOL(val, t, macToMe);
    FO_NUMBER(val, t, srcId, GT_U32);
    FO_NUMBER(val, t, dsaQosProfile, GT_U32);
    FO_NUMBER(val, t, tag0TpidIndex, GT_U32);
    FO_NUMBER(val, t, tag1TpidIndex, GT_U32);
    FO_NUMBER(val, t, eTagGrp, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_ETH_RULE_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_TAI_TOD_STEP_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TAI_TOD_STEP_STC *val
)
{
    F_NUMBER(val, -1, nanoSeconds, GT_U32);
    F_NUMBER(val, -1, fracNanoSeconds, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TAI_TOD_STEP_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TAI_TOD_STEP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, nanoSeconds, GT_U32);
    FO_NUMBER(val, t, fracNanoSeconds, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TAI_TOD_STEP_STC);

enumDescr enumDescr_CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_BASIC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_STRESS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_STRESS_1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT);

enumDescr enumDescr_CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E),
    ENUM_ENTRY(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E),
    ENUM_ENTRY(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_END_TO_END_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_TRUNK_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_L2_ECMP_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_L3_ECMP_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE___LAST___E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_DO_NOT_MODIFY_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_UNTAGGED_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT);

void prv_lua_to_c_CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC(
    lua_State *L,
    CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC *val
)
{
    F_NUMBER(val, -1, srcPort, GT_PHYSICAL_PORT_NUM);
    F_NUMBER(val, -1, trunkPort, GT_PHYSICAL_PORT_NUM);
}

void prv_c_to_lua_CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC(
    lua_State *L,
    CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, srcPort, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, trunkPort, GT_PHYSICAL_PORT_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC);

enumDescr enumDescr_CPSS_DXCH_STC_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_STC_INGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_STC_EGRESS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_STC_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_DISABLED_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_INTERLAKEN_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_DSA_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_DIRECTION_RX_E),
    ENUM_ENTRY(CPSS_DXCH_FABRIC_HGL_DIRECTION_TX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT);

enumDescr enumDescr_CPSS_DXCH_PROTECTION_LOC_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PROTECTION_LOC_NOT_DETECTED_E),
    ENUM_ENTRY(CPSS_DXCH_PROTECTION_LOC_DETECTED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PROTECTION_LOC_STATUS_ENT);

void prv_lua_to_c_CPSS_DXCH_PTP_TSU_CONTROL_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TSU_CONTROL_STC *val
)
{
    F_BOOL(val, -1, unitEnable);
    F_NUMBER(val, -1, rxTaiSelect, GT_U32);
    F_BOOL(val, -1, tsQueOverrideEnable);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TSU_CONTROL_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TSU_CONTROL_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, unitEnable);
    FO_NUMBER(val, t, rxTaiSelect, GT_U32);
    FO_BOOL(val, t, tsQueOverrideEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TSU_CONTROL_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_MODIFY_OUTER_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_MODIFY_INNER_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_GROUP_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_GROUP_GIGA_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_GROUP_HGS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_GROUP_ENT);

enumDescr enumDescr_CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DYNAMIC_LOAD_BALANCING_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_FC_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_FC_MODE_802_3X_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_FC_MODE_PFC_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_FC_MODE_LL_FC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_FC_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_RULE_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_TTI_RULE_COMMON_STC *val
)
{
    F_NUMBER(val, -1, pclId, GT_U32);
    F_BOOL(val, -1, srcIsTrunk);
    F_NUMBER(val, -1, srcPortTrunk, GT_U32);
    F_STRUCT_CUSTOM(val, -1, mac, GT_ETHERADDR);
    F_NUMBER(val, -1, vid, GT_U16);
    F_BOOL(val, -1, isTagged);
    F_BOOL(val, -1, dsaSrcIsTrunk);
    F_NUMBER(val, -1, dsaSrcPortTrunk, GT_U32);
    F_NUMBER(val, -1, dsaSrcDevice, GT_U32);
    F_NUMBER(val, -1, sourcePortGroupId, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TTI_RULE_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_TTI_RULE_COMMON_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_BOOL(val, t, srcIsTrunk);
    FO_NUMBER(val, t, srcPortTrunk, GT_U32);
    FO_STRUCT(val, t, mac, GT_ETHERADDR);
    FO_NUMBER(val, t, vid, GT_U16);
    FO_BOOL(val, t, isTagged);
    FO_BOOL(val, t, dsaSrcIsTrunk);
    FO_NUMBER(val, t, dsaSrcPortTrunk, GT_U32);
    FO_NUMBER(val, t, dsaSrcDevice, GT_U32);
    FO_NUMBER(val, t, sourcePortGroupId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_RULE_COMMON_STC);

void prv_lua_to_c_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *val
)
{
    F_ENUM(val, -1, cmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, cpuCodeIdx, CPSS_DXCH_IP_CPU_CODE_INDEX_ENT);
    F_BOOL(val, -1, appSpecificCpuCodeEnable);
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
    F_BOOL(val, -1, multicastRPFCheckEnable);
    F_ENUM(val, -1, multicastIngressVlanCheck, CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT);
    F_NUMBER(val, -1, multicastRPFVlan, GT_U16);
    F_NUMBER(val, -1, multicastRPFRoutingSharedTreeIndex, GT_U8);
    F_ENUM(val, -1, multicastRPFFailCommandMode, CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT);
    F_ENUM(val, -1, RPFFailCommand, CPSS_PACKET_CMD_ENT);
    F_BOOL(val, -1, scopeCheckingEnable);
    F_ENUM(val, -1, siteId, CPSS_IP_SITE_ID_ENT);
    F_NUMBER(val, -1, mtuProfileIndex, GT_U32);
    F_NUMBER(val, -1, internalMLLPointer, GT_U32);
    F_NUMBER(val, -1, externalMLLPointer, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, cmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, cpuCodeIdx, CPSS_DXCH_IP_CPU_CODE_INDEX_ENT);
    FO_BOOL(val, t, appSpecificCpuCodeEnable);
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
    FO_BOOL(val, t, multicastRPFCheckEnable);
    FO_ENUM(val, t, multicastIngressVlanCheck, CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT);
    FO_NUMBER(val, t, multicastRPFVlan, GT_U16);
    FO_NUMBER(val, t, multicastRPFRoutingSharedTreeIndex, GT_U8);
    FO_ENUM(val, t, multicastRPFFailCommandMode, CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT);
    FO_ENUM(val, t, RPFFailCommand, CPSS_PACKET_CMD_ENT);
    FO_BOOL(val, t, scopeCheckingEnable);
    FO_ENUM(val, t, siteId, CPSS_IP_SITE_ID_ENT);
    FO_NUMBER(val, t, mtuProfileIndex, GT_U32);
    FO_NUMBER(val, t, internalMLLPointer, GT_U32);
    FO_NUMBER(val, t, externalMLLPointer, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC(
    lua_State *L,
    CPSS_DXCH_TTI_TRILL_ADJACENCY_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, outerMacSa, GT_ETHERADDR);
    F_NUMBER(val, -1, trillMBit, GT_U32);
    F_NUMBER(val, -1, trillEgressRbid, GT_U32);
    F_NUMBER(val, -1, srcHwDevice, GT_HW_DEV_NUM);
    F_NUMBER(val, -1, srcIsTrunk, GT_U32);
    F_NUMBER(val, -1, srcPortTrunk, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TTI_TRILL_ADJACENCY_STC(
    lua_State *L,
    CPSS_DXCH_TTI_TRILL_ADJACENCY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, outerMacSa, GT_ETHERADDR);
    FO_NUMBER(val, t, trillMBit, GT_U32);
    FO_NUMBER(val, t, trillEgressRbid, GT_U32);
    FO_NUMBER(val, t, srcHwDevice, GT_HW_DEV_NUM);
    FO_NUMBER(val, t, srcIsTrunk, GT_U32);
    FO_NUMBER(val, t, srcPortTrunk, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_TRILL_ADJACENCY_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC *val
)
{
    F_BOOL(val, -1, assignSourceId);
    F_NUMBER(val, -1, sourceIdValue, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, assignSourceId);
    FO_NUMBER(val, t, sourceIdValue, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC);

enumDescr enumDescr_CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BASIC_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_TRANS_CLK_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_PIGGY_BACK_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BOUNDRY_CLK_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    F_STRUCT(val, -1, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPV6ADDR);
    F_NUMBER(val, -1, dipBits127to120, GT_U8);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_ARRAY_START(val, -1, udb20_27);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, udb20_27, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb20_27);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    FO_STRUCT(val, t, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC);
    FO_STRUCT(val, t, sip, GT_IPV6ADDR);
    FO_NUMBER(val, t, dipBits127to120, GT_U8);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_ARRAY_START(val, t, udb20_27);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, udb20_27, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb20_27);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC);

void prv_lua_to_c_CPSS_DXCH_IPFIX_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IPFIX_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, timeStamp, GT_U32);
    F_NUMBER(val, -1, packetCount, GT_U32);
    F_STRUCT(val, -1, byteCount, GT_U64);
    F_NUMBER(val, -1, dropCounter, GT_U32);
    F_NUMBER(val, -1, randomOffset, GT_U32);
    F_STRUCT(val, -1, lastSampledValue, GT_U64);
    F_STRUCT(val, -1, samplingWindow, GT_U64);
    F_ENUM(val, -1, samplingAction, CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT);
    F_NUMBER(val, -1, logSamplingRange, GT_U32);
    F_ENUM(val, -1, randomFlag, CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT);
    F_ENUM(val, -1, samplingMode, CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT);
    F_NUMBER(val, -1, cpuSubCode, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IPFIX_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IPFIX_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, timeStamp, GT_U32);
    FO_NUMBER(val, t, packetCount, GT_U32);
    FO_STRUCT(val, t, byteCount, GT_U64);
    FO_NUMBER(val, t, dropCounter, GT_U32);
    FO_NUMBER(val, t, randomOffset, GT_U32);
    FO_STRUCT(val, t, lastSampledValue, GT_U64);
    FO_STRUCT(val, t, samplingWindow, GT_U64);
    FO_ENUM(val, t, samplingAction, CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT);
    FO_NUMBER(val, t, logSamplingRange, GT_U32);
    FO_ENUM(val, t, randomFlag, CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT);
    FO_ENUM(val, t, samplingMode, CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT);
    FO_NUMBER(val, t, cpuSubCode, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IPFIX_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    F_STRUCT(val, -1, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_ARRAY_START(val, -1, dipBits0to31);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, dipBits0to31, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, dipBits0to31);
    F_NUMBER(val, -1, l4Byte13, GT_U8);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_ARRAY_START(val, -1, udb4_7);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb4_7, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb4_7);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    FO_STRUCT(val, t, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_ARRAY_START(val, t, dipBits0to31);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, dipBits0to31, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, dipBits0to31);
    FO_NUMBER(val, t, l4Byte13, GT_U8);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_ARRAY_START(val, t, udb4_7);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb4_7, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb4_7);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_DM_CONFIG_STC *val
)
{
    F_BOOL(val, -1, countersEnable);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_DM_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_DM_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, countersEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_DM_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_CFG_TABLES_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_FDB_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_PCL_ACTION_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_PCL_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_QOS_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_TTI_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_MLL_PAIR_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_POLICER_METERS_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_VIDX_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_ARP_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_TUNNEL_START_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_STG_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_CNC_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_TRUNK_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_LPM_RAM_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_L2_MLL_LTT_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_EPORT_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_DEFAULT_EPORT_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_PHYSICAL_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_TABLE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CFG_TABLES_ENT);

enumDescr enumDescr_CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_GLOBAL_E),
    ENUM_ENTRY(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_EXTENDED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_CW_EXCEPTION_EXPIRY_VCCV_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_CW_EXCEPTION_NON_DATA_CONTROL_WORD_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_CW_EXCEPTION_FRAGMENTED_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_CW_EXCEPTION_SEQUENCE_ERROR_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_CW_EXCEPTION_ILLEGAL_CONTROL_WORD_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_PW_CW_EXCEPTION_PASSENGER_PW_TAG0_NOT_FOUND_ERROR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT);

void prv_lua_to_c_CPSS_DXCH_TCAM_BLOCK_INFO_STC(
    lua_State *L,
    CPSS_DXCH_TCAM_BLOCK_INFO_STC *val
)
{
    F_NUMBER(val, -1, group, GT_U32);
    F_NUMBER(val, -1, hitNum, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TCAM_BLOCK_INFO_STC(
    lua_State *L,
    CPSS_DXCH_TCAM_BLOCK_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, group, GT_U32);
    FO_NUMBER(val, t, hitNum, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TCAM_BLOCK_INFO_STC);

enumDescr enumDescr_CPSS_IP_NAT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_IP_NAT_TYPE_NAT44_E),
    ENUM_ENTRY(CPSS_IP_NAT_TYPE_NAT66_E),
    ENUM_ENTRY(CPSS_IP_NAT_TYPE_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_IP_NAT_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK4_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK5_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK6_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK7_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_CFG_DEV_INFO_STC(
    lua_State *L,
    CPSS_DXCH_CFG_DEV_INFO_STC *val
)
{
    F_STRUCT(val, -1, genDevInfo, CPSS_GEN_CFG_DEV_INFO_STC);
}

void prv_c_to_lua_CPSS_DXCH_CFG_DEV_INFO_STC(
    lua_State *L,
    CPSS_DXCH_CFG_DEV_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, genDevInfo, CPSS_GEN_CFG_DEV_INFO_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_CFG_DEV_INFO_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_SERDES_SPEED_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_1_25_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_3_125_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_3_75_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_6_25_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_5_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_4_25_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_2_5_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_5_156_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_10_3125_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_3_333_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_12_5_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_SPEED_NA_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SERDES_SPEED_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_TRILL_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_TRILL_RULE_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    F_BOOL(val, -1, trillMBit);
    F_NUMBER(val, -1, trillEgressRbid, GT_U32);
    F_NUMBER(val, -1, trillIngressRbid, GT_U32);
    F_STRUCT_CUSTOM(val, -1, innerPacketMacDa, GT_ETHERADDR);
    F_BOOL(val, -1, innerPacketTag0Exists);
    F_NUMBER(val, -1, innerPacketTag0Vid, GT_U32);
    F_BOOL(val, -1, innerPacketFieldsAreValid);
    F_BOOL(val, -1, trillMcDescriptorInstance);
}

void prv_c_to_lua_CPSS_DXCH_TTI_TRILL_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_TRILL_RULE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    FO_BOOL(val, t, trillMBit);
    FO_NUMBER(val, t, trillEgressRbid, GT_U32);
    FO_NUMBER(val, t, trillIngressRbid, GT_U32);
    FO_STRUCT(val, t, innerPacketMacDa, GT_ETHERADDR);
    FO_BOOL(val, t, innerPacketTag0Exists);
    FO_NUMBER(val, t, innerPacketTag0Vid, GT_U32);
    FO_BOOL(val, t, innerPacketFieldsAreValid);
    FO_BOOL(val, t, trillMcDescriptorInstance);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_TRILL_RULE_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    F_STRUCT(val, -1, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPADDR);
    F_NUMBER(val, -1, etherType, GT_U16);
    F_NUMBER(val, -1, l2Encap, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_NUMBER(val, -1, ipv4Fragmented, GT_U8);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, isMpls, GT_U8);
    F_NUMBER(val, -1, numOfMplsLabels, GT_U32);
    F_NUMBER(val, -1, protocolTypeAfterMpls, GT_U32);
    F_NUMBER(val, -1, mplsLabel0, GT_U32);
    F_NUMBER(val, -1, mplsExp0, GT_U32);
    F_NUMBER(val, -1, mplsLabel1, GT_U32);
    F_NUMBER(val, -1, mplsExp1, GT_U32);
    F_ARRAY_START(val, -1, udb12_19);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, udb12_19, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb12_19);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    FO_STRUCT(val, t, commonExt, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC);
    FO_STRUCT(val, t, sip, GT_IPADDR);
    FO_STRUCT(val, t, dip, GT_IPADDR);
    FO_NUMBER(val, t, etherType, GT_U16);
    FO_NUMBER(val, t, l2Encap, GT_U32);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_NUMBER(val, t, ipv4Fragmented, GT_U8);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, isMpls, GT_U8);
    FO_NUMBER(val, t, numOfMplsLabels, GT_U32);
    FO_NUMBER(val, t, protocolTypeAfterMpls, GT_U32);
    FO_NUMBER(val, t, mplsLabel0, GT_U32);
    FO_NUMBER(val, t, mplsExp0, GT_U32);
    FO_NUMBER(val, t, mplsLabel1, GT_U32);
    FO_NUMBER(val, t, mplsExp1, GT_U32);
    FO_ARRAY_START(val, t, udb12_19);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, udb12_19, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb12_19);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC);

enumDescr enumDescr_CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_STAGE_METER_MODE_FLOW_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_OAM_STAGE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_OAM_STAGE_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_IMPLEMENT_WA_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FU_FROM_NON_SOURCE_PORT_GROUP_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_PADDING_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_RGMII_EDGE_ALIGN_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_IP_MC_UNDER_STRESS_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_GE_PORT_UNIDIRECT_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_2_5G_SGMII_LINK_UP_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_CFG_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_NETWORKP_CFG_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_STACKP_CFG_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_SERDES_INTERNAL_REG_ACCESS_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_IPM_BRIDGE_COPY_GET_DROPPED_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_NO_ALLIGNMENT_LOCK_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_MC_BANDWIDTH_RESTRICTION_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_DISMATCH_IF_LINK_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_CUT_THROUGH_SLOW_TO_FAST_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_RXAUI_LINK_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_TRI_SPEED_PORT_AN_FC_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_REV_A0_40G_NOT_THROUGH_TM_IS_PA_30G_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_WRONG_MIB_COUNTERS_LINK_DOWN_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_ROUTER_TCAM_RM_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E),
    ENUM_ENTRY(CPSS_DXCH_IMPLEMENT_WA_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IMPLEMENT_WA_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE0_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_POLICER_BUCKETS_STATE_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_BUCKETS_STATE_STC *val
)
{
    F_NUMBER(val, -1, lastTimeUpdate0, GT_U32);
    F_NUMBER(val, -1, lastTimeUpdate1, GT_U32);
    F_BOOL(val, -1, wrapAround0);
    F_BOOL(val, -1, wrapAround1);
    F_NUMBER(val, -1, bucketSize0, GT_U32);
    F_NUMBER(val, -1, bucketSize1, GT_U32);
    F_BOOL(val, -1, bucketSignPositive0);
    F_BOOL(val, -1, bucketSignPositive1);
}

void prv_c_to_lua_CPSS_DXCH_POLICER_BUCKETS_STATE_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_BUCKETS_STATE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, lastTimeUpdate0, GT_U32);
    FO_NUMBER(val, t, lastTimeUpdate1, GT_U32);
    FO_BOOL(val, t, wrapAround0);
    FO_BOOL(val, t, wrapAround1);
    FO_NUMBER(val, t, bucketSize0, GT_U32);
    FO_NUMBER(val, t, bucketSize1, GT_U32);
    FO_BOOL(val, t, bucketSignPositive0);
    FO_BOOL(val, t, bucketSignPositive1);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_POLICER_BUCKETS_STATE_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC *val
)
{
    F_NUMBER(val, -1, pclId, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_NUMBER(val, -1, vid, GT_U32);
    F_NUMBER(val, -1, srcPort, GT_U32);
    F_NUMBER(val, -1, srcDevIsOwn, GT_U8);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, macToMe, GT_U8);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_NUMBER(val, -1, flowId, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_NUMBER(val, t, vid, GT_U32);
    FO_NUMBER(val, t, srcPort, GT_U32);
    FO_NUMBER(val, t, srcDevIsOwn, GT_U8);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, macToMe, GT_U8);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_NUMBER(val, t, flowId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC);

void prv_lua_to_c_CPSS_DXCH_COS_PROFILE_STC(
    lua_State *L,
    CPSS_DXCH_COS_PROFILE_STC *val
)
{
    F_ENUM(val, -1, dropPrecedence, CPSS_DP_LEVEL_ENT);
    F_NUMBER(val, -1, userPriority, GT_U32);
    F_NUMBER(val, -1, trafficClass, GT_U32);
    F_NUMBER(val, -1, dscp, GT_U32);
    F_NUMBER(val, -1, exp, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_COS_PROFILE_STC(
    lua_State *L,
    CPSS_DXCH_COS_PROFILE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dropPrecedence, CPSS_DP_LEVEL_ENT);
    FO_NUMBER(val, t, userPriority, GT_U32);
    FO_NUMBER(val, t, trafficClass, GT_U32);
    FO_NUMBER(val, t, dscp, GT_U32);
    FO_NUMBER(val, t, exp, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_COS_PROFILE_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC *val
)
{
    F_STRUCT(val, -1, ingressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
    F_NUMBER(val, -1, pktType, GT_U32);
    F_NUMBER(val, -1, isNd, GT_U8);
    F_NUMBER(val, -1, tag1Exist, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, mplsOuterLabel, GT_U32);
    F_NUMBER(val, -1, mplsOuterLabExp, GT_U32);
    F_NUMBER(val, -1, mplsOuterLabSBit, GT_U8);
    F_NUMBER(val, -1, ipPacketLength, GT_U32);
    F_NUMBER(val, -1, ipv6HdrFlowLabel, GT_U32);
    F_NUMBER(val, -1, ttl, GT_U32);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPV6ADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPV6ADDR);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_NUMBER(val, -1, isIpv6LinkLocal, GT_U8);
    F_NUMBER(val, -1, isIpv6Mld, GT_U8);
    F_NUMBER(val, -1, ipHeaderOk, GT_U8);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_ARRAY_START(val, -1, UdbStdNotIp);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, UdbStdNotIp, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbStdNotIp);
    F_ARRAY_START(val, -1, UdbStdIpL2Qos);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, UdbStdIpL2Qos, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbStdIpL2Qos);
    F_ARRAY_START(val, -1, UdbStdIpV4L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, UdbStdIpV4L4, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbStdIpV4L4);
    F_ARRAY_START(val, -1, UdbExtNotIpv6);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, UdbExtNotIpv6, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbExtNotIpv6);
    F_ARRAY_START(val, -1, UdbExtIpv6L2);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, UdbExtIpv6L2, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbExtIpv6L2);
    F_ARRAY_START(val, -1, UdbExtIpv6L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, UdbExtIpv6L4, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbExtIpv6L4);
    F_ARRAY_START(val, -1, udb0_11);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            F_ARRAY_NUMBER(val, udb0_11, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb0_11);
    F_ARRAY_START(val, -1, udb17_22);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            F_ARRAY_NUMBER(val, udb17_22, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb17_22);
    F_ARRAY_START(val, -1, udb45_46);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, udb45_46, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb45_46);
    F_NUMBER(val, -1, srcPortOrTrunk, GT_U32);
    F_NUMBER(val, -1, srcIsTrunk, GT_U8);
    F_NUMBER(val, -1, trunkHash, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, ingressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
    FO_NUMBER(val, t, pktType, GT_U32);
    FO_NUMBER(val, t, isNd, GT_U8);
    FO_NUMBER(val, t, tag1Exist, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, mplsOuterLabel, GT_U32);
    FO_NUMBER(val, t, mplsOuterLabExp, GT_U32);
    FO_NUMBER(val, t, mplsOuterLabSBit, GT_U8);
    FO_NUMBER(val, t, ipPacketLength, GT_U32);
    FO_NUMBER(val, t, ipv6HdrFlowLabel, GT_U32);
    FO_NUMBER(val, t, ttl, GT_U32);
    FO_STRUCT(val, t, sip, GT_IPV6ADDR);
    FO_STRUCT(val, t, dip, GT_IPV6ADDR);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_NUMBER(val, t, isIpv6LinkLocal, GT_U8);
    FO_NUMBER(val, t, isIpv6Mld, GT_U8);
    FO_NUMBER(val, t, ipHeaderOk, GT_U8);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_ARRAY_START(val, t, UdbStdNotIp);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, UdbStdNotIp, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbStdNotIp);
    FO_ARRAY_START(val, t, UdbStdIpL2Qos);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, UdbStdIpL2Qos, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbStdIpL2Qos);
    FO_ARRAY_START(val, t, UdbStdIpV4L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, UdbStdIpV4L4, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbStdIpV4L4);
    FO_ARRAY_START(val, t, UdbExtNotIpv6);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, UdbExtNotIpv6, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbExtNotIpv6);
    FO_ARRAY_START(val, t, UdbExtIpv6L2);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, UdbExtIpv6L2, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbExtIpv6L2);
    FO_ARRAY_START(val, t, UdbExtIpv6L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, UdbExtIpv6L4, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbExtIpv6L4);
    FO_ARRAY_START(val, t, udb0_11);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            FO_ARRAY_NUMBER(val, udb0_11, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb0_11);
    FO_ARRAY_START(val, t, udb17_22);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            FO_ARRAY_NUMBER(val, udb17_22, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb17_22);
    FO_ARRAY_START(val, t, udb45_46);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, udb45_46, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb45_46);
    FO_NUMBER(val, t, srcPortOrTrunk, GT_U32);
    FO_NUMBER(val, t, srcIsTrunk, GT_U8);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC);

void prv_lua_to_c_CPSS_DXCH_TTI_ACTION_STC(
    lua_State *L,
    CPSS_DXCH_TTI_ACTION_STC *val
)
{
    F_BOOL(val, -1, tunnelTerminate);
    F_ENUM(val, -1, passengerPacketType, CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
    F_BOOL(val, -1, copyTtlFromTunnelHeader);
    F_ENUM(val, -1, command, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, redirectCommand, CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
    F_STRUCT(val, -1, egressInterface, CPSS_INTERFACE_INFO_STC);
    F_BOOL(val, -1, tunnelStart);
    F_NUMBER(val, -1, tunnelStartPtr, GT_U32);
    F_NUMBER(val, -1, routerLookupPtr, GT_U32);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_BOOL(val, -1, sourceIdSetEnable);
    F_NUMBER(val, -1, sourceId, GT_U32);
    F_ENUM(val, -1, vlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_ENUM(val, -1, vlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_BOOL(val, -1, nestedVlanEnable);
    F_BOOL(val, -1, bindToPolicer);
    F_NUMBER(val, -1, policerIndex, GT_U32);
    F_ENUM(val, -1, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_ENUM(val, -1, qosTrustMode, CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_ENUM(val, -1, modifyUpEnable, CPSS_DXCH_TTI_MODIFY_UP_ENT);
    F_ENUM(val, -1, modifyDscpEnable, CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
    F_NUMBER(val, -1, up, GT_U32);
    F_BOOL(val, -1, remapDSCP);
    F_BOOL(val, -1, mirrorToIngressAnalyzerEnable);
    F_ENUM(val, -1, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    F_BOOL(val, -1, vntl2Echo);
    F_BOOL(val, -1, bridgeBypass);
    F_BOOL(val, -1, actionStop);
    F_BOOL(val, -1, activateCounter);
    F_NUMBER(val, -1, counterIndex, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TTI_ACTION_STC(
    lua_State *L,
    CPSS_DXCH_TTI_ACTION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tunnelTerminate);
    FO_ENUM(val, t, passengerPacketType, CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
    FO_BOOL(val, t, copyTtlFromTunnelHeader);
    FO_ENUM(val, t, command, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, redirectCommand, CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
    FO_STRUCT(val, t, egressInterface, CPSS_INTERFACE_INFO_STC);
    FO_BOOL(val, t, tunnelStart);
    FO_NUMBER(val, t, tunnelStartPtr, GT_U32);
    FO_NUMBER(val, t, routerLookupPtr, GT_U32);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_BOOL(val, t, sourceIdSetEnable);
    FO_NUMBER(val, t, sourceId, GT_U32);
    FO_ENUM(val, t, vlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_ENUM(val, t, vlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_BOOL(val, t, nestedVlanEnable);
    FO_BOOL(val, t, bindToPolicer);
    FO_NUMBER(val, t, policerIndex, GT_U32);
    FO_ENUM(val, t, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_ENUM(val, t, qosTrustMode, CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_ENUM(val, t, modifyUpEnable, CPSS_DXCH_TTI_MODIFY_UP_ENT);
    FO_ENUM(val, t, modifyDscpEnable, CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
    FO_NUMBER(val, t, up, GT_U32);
    FO_BOOL(val, t, remapDSCP);
    FO_BOOL(val, t, mirrorToIngressAnalyzerEnable);
    FO_ENUM(val, t, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_BOOL(val, t, vntl2Echo);
    FO_BOOL(val, t, bridgeBypass);
    FO_BOOL(val, t, actionStop);
    FO_BOOL(val, t, activateCounter);
    FO_NUMBER(val, t, counterIndex, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_ACTION_STC);

void prv_lua_to_c_CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, bitMap, GT_U32);
    F_NUMBER(val, -1, rangeCounter, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, bitMap, GT_U32);
    FO_NUMBER(val, t, rangeCounter, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_BOOL(val, -1, macDaIncrementEnable);
    F_NUMBER(val, -1, macDaIncrementLimit, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_BOOL(val, -1, vlanTagEnable);
    F_NUMBER(val, -1, vpt, GT_U8);
    F_NUMBER(val, -1, cfi, GT_U8);
    F_NUMBER(val, -1, vid, GT_U16);
    F_NUMBER(val, -1, etherType, GT_U16);
    F_ENUM(val, -1, payloadType, CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT);
    F_ARRAY_START(val, -1, cyclicPatternArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, cyclicPatternArr, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, cyclicPatternArr);
    F_ENUM(val, -1, packetLengthType, CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT);
    F_NUMBER(val, -1, packetLength, GT_U32);
    F_BOOL(val, -1, undersizeEnable);
    F_ENUM(val, -1, transmitMode, CPSS_DIAG_PG_TRANSMIT_MODE_ENT);
    F_NUMBER(val, -1, packetCount, GT_U32);
    F_ENUM(val, -1, packetCountMultiplier, CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT);
    F_NUMBER(val, -1, ipg, GT_U32);
    F_ENUM(val, -1, interfaceSize, CPSS_DIAG_PG_IF_SIZE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_BOOL(val, t, macDaIncrementEnable);
    FO_NUMBER(val, t, macDaIncrementLimit, GT_U32);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_BOOL(val, t, vlanTagEnable);
    FO_NUMBER(val, t, vpt, GT_U8);
    FO_NUMBER(val, t, cfi, GT_U8);
    FO_NUMBER(val, t, vid, GT_U16);
    FO_NUMBER(val, t, etherType, GT_U16);
    FO_ENUM(val, t, payloadType, CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT);
    FO_ARRAY_START(val, t, cyclicPatternArr);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, cyclicPatternArr, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, cyclicPatternArr);
    FO_ENUM(val, t, packetLengthType, CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT);
    FO_NUMBER(val, t, packetLength, GT_U32);
    FO_BOOL(val, t, undersizeEnable);
    FO_ENUM(val, t, transmitMode, CPSS_DIAG_PG_TRANSMIT_MODE_ENT);
    FO_NUMBER(val, t, packetCount, GT_U32);
    FO_ENUM(val, t, packetCountMultiplier, CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT);
    FO_NUMBER(val, t, ipg, GT_U32);
    FO_ENUM(val, t, interfaceSize, CPSS_DIAG_PG_IF_SIZE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_BYTE_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_L2_MLL_LTT_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_L2_MLL_LTT_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, mllPointer, GT_U32);
    F_ENUM(val, -1, entrySelector, CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT);
    F_BOOL(val, -1, mllMaskProfileEnable);
    F_NUMBER(val, -1, mllMaskProfile, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_L2_MLL_LTT_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_L2_MLL_LTT_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, mllPointer, GT_U32);
    FO_ENUM(val, t, entrySelector, CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT);
    FO_BOOL(val, t, mllMaskProfileEnable);
    FO_NUMBER(val, t, mllMaskProfile, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_L2_MLL_LTT_ENTRY_STC);

enumDescr enumDescr_CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_WR_DMA_TO_TTI_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_PCL_TO_BRIDGE_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER0_TO_INGRESS_POLICER1_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER1_TO_PRE_EGRESS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_TXQ_TO_HEADER_ALTERATION_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_EGRESS_POLICER_TO_EGRESS_PCL_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_DESCRIPTOR_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_STD_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_EXT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT);

enumDescr enumDescr_CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_ZEROS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_ONES_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_CYCLIC_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS9_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS15_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS31_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_1T_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_2T_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_5T_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_10T_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_DFETraining),
    ENUM_ENTRY(CPSS_DXCH_DIAG_TRANSMIT_MODE_MAX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC *val
)
{
    F_STRUCT(val, -1, commonIngrUdb, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC);
    F_NUMBER(val, -1, isIpv6, GT_U8);
    F_NUMBER(val, -1, ipProtocol, GT_U32);
    F_ARRAY_START(val, -1, sipBits31_0);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, sipBits31_0, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, sipBits31_0);
    F_ARRAY_START(val, -1, sipBits79_32orMacSa);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            F_ARRAY_NUMBER(val, sipBits79_32orMacSa, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, sipBits79_32orMacSa);
    F_ARRAY_START(val, -1, sipBits127_80orMacDa);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            F_ARRAY_NUMBER(val, sipBits127_80orMacDa, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, sipBits127_80orMacDa);
    F_ARRAY_START(val, -1, dipBits127_112);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, dipBits127_112, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, dipBits127_112);
    F_ARRAY_START(val, -1, dipBits31_0);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, dipBits31_0, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, dipBits31_0);
    F_ARRAY_START(val, -1, udb0_15);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            F_ARRAY_NUMBER(val, udb0_15, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb0_15);
    F_NUMBER(val, -1, tag1Exist, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_NUMBER(val, -1, trunkHash, GT_U32);
    F_ARRAY_START(val, -1, udb23_30);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            F_ARRAY_NUMBER(val, udb23_30, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb23_30);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, commonIngrUdb, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC);
    FO_NUMBER(val, t, isIpv6, GT_U8);
    FO_NUMBER(val, t, ipProtocol, GT_U32);
    FO_ARRAY_START(val, t, sipBits31_0);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, sipBits31_0, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, sipBits31_0);
    FO_ARRAY_START(val, t, sipBits79_32orMacSa);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            FO_ARRAY_NUMBER(val, sipBits79_32orMacSa, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, sipBits79_32orMacSa);
    FO_ARRAY_START(val, t, sipBits127_80orMacDa);
    {
        int idx;
        for (idx = 0; idx < 6; idx++) {
            FO_ARRAY_NUMBER(val, sipBits127_80orMacDa, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, sipBits127_80orMacDa);
    FO_ARRAY_START(val, t, dipBits127_112);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, dipBits127_112, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, dipBits127_112);
    FO_ARRAY_START(val, t, dipBits31_0);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, dipBits31_0, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, dipBits31_0);
    FO_ARRAY_START(val, t, udb0_15);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            FO_ARRAY_NUMBER(val, udb0_15, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb0_15);
    FO_NUMBER(val, t, tag1Exist, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    FO_ARRAY_START(val, t, udb23_30);
    {
        int idx;
        for (idx = 0; idx < 8; idx++) {
            FO_ARRAY_NUMBER(val, udb23_30, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb23_30);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC);

enumDescr enumDescr_CPSS_DXCH_POLICER_COUNTING_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_COUNTING_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_COUNTING_POLICY_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_COUNTING_VLAN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_COUNTING_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC *val
)
{
    F_NUMBER(val, -1, isIpv4, GT_U8);
    F_NUMBER(val, -1, ipProtocol, GT_U32);
    F_NUMBER(val, -1, dscp, GT_U32);
    F_NUMBER(val, -1, isL4Valid, GT_U8);
    F_NUMBER(val, -1, l4Byte2, GT_U8);
    F_NUMBER(val, -1, l4Byte3, GT_U8);
    F_NUMBER(val, -1, ipHeaderOk, GT_U8);
    F_NUMBER(val, -1, ipv4Fragmented, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, isIpv4, GT_U8);
    FO_NUMBER(val, t, ipProtocol, GT_U32);
    FO_NUMBER(val, t, dscp, GT_U32);
    FO_NUMBER(val, t, isL4Valid, GT_U8);
    FO_NUMBER(val, t, l4Byte2, GT_U8);
    FO_NUMBER(val, t, l4Byte3, GT_U8);
    FO_NUMBER(val, t, ipHeaderOk, GT_U8);
    FO_NUMBER(val, t, ipv4Fragmented, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC);

enumDescr enumDescr_CPSS_DXCH_TTI_RULE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_RULE_IPV4_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_RULE_MPLS_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_RULE_ETH_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_RULE_MIM_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_RULE_UDB_10_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_RULE_UDB_20_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_RULE_UDB_30_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_RULE_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_MAP_STC(
    lua_State *L,
    CPSS_DXCH_PORT_MAP_STC *val
)
{
    F_NUMBER(val, -1, physicalPortNumber, GT_PHYSICAL_PORT_NUM);
    F_ENUM(val, -1, mappingType, CPSS_DXCH_PORT_MAPPING_TYPE_ENT);
    F_NUMBER(val, -1, portGroup, GT_U32);
    F_NUMBER(val, -1, interfaceNum, GT_U32);
    F_NUMBER(val, -1, txqPortNumber, GT_U32);
    F_BOOL(val, -1, tmEnable);
    F_NUMBER(val, -1, tmPortInd, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_MAP_STC(
    lua_State *L,
    CPSS_DXCH_PORT_MAP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, physicalPortNumber, GT_PHYSICAL_PORT_NUM);
    FO_ENUM(val, t, mappingType, CPSS_DXCH_PORT_MAPPING_TYPE_ENT);
    FO_NUMBER(val, t, portGroup, GT_U32);
    FO_NUMBER(val, t, interfaceNum, GT_U32);
    FO_NUMBER(val, t, txqPortNumber, GT_U32);
    FO_BOOL(val, t, tmEnable);
    FO_NUMBER(val, t, tmPortInd, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_MAP_STC);

enumDescr enumDescr_CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_NONE_E),
    ENUM_ENTRY(CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_CLEAR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_EEE_LPI_STATUS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_EEE_LPI_STATUS_STC *val
)
{
    F_BOOL(val, -1, macTxPathLpi);
    F_BOOL(val, -1, macTxPathLpWait);
    F_BOOL(val, -1, macRxPathLpi);
    F_BOOL(val, -1, pcsTxPathLpi);
    F_BOOL(val, -1, pcsRxPathLpi);
}

void prv_c_to_lua_CPSS_DXCH_PORT_EEE_LPI_STATUS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_EEE_LPI_STATUS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, macTxPathLpi);
    FO_BOOL(val, t, macTxPathLpWait);
    FO_BOOL(val, t, macRxPathLpi);
    FO_BOOL(val, t, pcsTxPathLpi);
    FO_BOOL(val, t, pcsRxPathLpi);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_EEE_LPI_STATUS_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_RULE_OPTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_RULE_OPTION_ENT);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LM_CONFIG_STC *val
)
{
    F_BOOL(val, -1, countersEnable);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_LM_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_LM_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, countersEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_LM_CONFIG_STC);

void prv_lua_to_c_CPSS_DXCH_CUT_THROUGH_BYPASS_STC(
    lua_State *L,
    CPSS_DXCH_CUT_THROUGH_BYPASS_STC *val
)
{
    F_BOOL(val, -1, bypassRouter);
    F_BOOL(val, -1, bypassIngressPolicerStage0);
    F_BOOL(val, -1, bypassIngressPolicerStage1);
    F_BOOL(val, -1, bypassEgressPolicer);
    F_BOOL(val, -1, bypassEgressPcl);
    F_ENUM(val, -1, bypassIngressPcl, CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT);
}

void prv_c_to_lua_CPSS_DXCH_CUT_THROUGH_BYPASS_STC(
    lua_State *L,
    CPSS_DXCH_CUT_THROUGH_BYPASS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, bypassRouter);
    FO_BOOL(val, t, bypassIngressPolicerStage0);
    FO_BOOL(val, t, bypassIngressPolicerStage1);
    FO_BOOL(val, t, bypassEgressPolicer);
    FO_BOOL(val, t, bypassEgressPcl);
    FO_ENUM(val, t, bypassIngressPcl, CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_CUT_THROUGH_BYPASS_STC);

void prv_lua_to_c_CPSS_DXCH_IP_MLL_PAIR_STC(
    lua_State *L,
    CPSS_DXCH_IP_MLL_PAIR_STC *val
)
{
    F_STRUCT(val, -1, firstMllNode, CPSS_DXCH_IP_MLL_STC);
    F_STRUCT(val, -1, secondMllNode, CPSS_DXCH_IP_MLL_STC);
    F_NUMBER(val, -1, nextPointer, GT_U16);
}

void prv_c_to_lua_CPSS_DXCH_IP_MLL_PAIR_STC(
    lua_State *L,
    CPSS_DXCH_IP_MLL_PAIR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, firstMllNode, CPSS_DXCH_IP_MLL_STC);
    FO_STRUCT(val, t, secondMllNode, CPSS_DXCH_IP_MLL_STC);
    FO_NUMBER(val, t, nextPointer, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_MLL_PAIR_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_ACTION_MIRROR_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_MIRROR_STC *val
)
{
    F_ENUM(val, -1, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    F_BOOL(val, -1, mirrorToRxAnalyzerPort);
    F_NUMBER(val, -1, ingressMirrorToAnalyzerIndex, GT_U32);
    F_BOOL(val, -1, mirrorTcpRstAndFinPacketsToCpu);
}

void prv_c_to_lua_CPSS_DXCH_PCL_ACTION_MIRROR_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_MIRROR_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_BOOL(val, t, mirrorToRxAnalyzerPort);
    FO_NUMBER(val, t, ingressMirrorToAnalyzerIndex, GT_U32);
    FO_BOOL(val, t, mirrorTcpRstAndFinPacketsToCpu);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_ACTION_MIRROR_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_ACTION_POLICER_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_POLICER_STC *val
)
{
    F_ENUM(val, -1, policerEnable, CPSS_DXCH_PCL_POLICER_ENABLE_ENT);
    F_NUMBER(val, -1, policerId, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_ACTION_POLICER_STC(
    lua_State *L,
    CPSS_DXCH_PCL_ACTION_POLICER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, policerEnable, CPSS_DXCH_PCL_POLICER_ENABLE_ENT);
    FO_NUMBER(val, t, policerId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_ACTION_POLICER_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC *val
)
{
    F_STRUCT(val, -1, commonIngrUdb, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC);
    F_NUMBER(val, -1, isIpv4, GT_U8);
    F_ARRAY_START(val, -1, udb0_15);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            F_ARRAY_NUMBER(val, udb0_15, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb0_15);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_NUMBER(val, -1, trunkHash, GT_U32);
    F_ARRAY_START(val, -1, udb35_38);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb35_38, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb35_38);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, commonIngrUdb, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC);
    FO_NUMBER(val, t, isIpv4, GT_U8);
    FO_ARRAY_START(val, t, udb0_15);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            FO_ARRAY_NUMBER(val, udb0_15, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb0_15);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    FO_ARRAY_START(val, t, udb35_38);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb35_38, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb35_38);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_PORT_DO_NOT_MODIFY_TAG_CMD_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E),
    ENUM_ENTRY(CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT);

void prv_lua_to_c_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC(
    lua_State *L,
    CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC *val
)
{
    F_ENUM(val, -1, action, CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT);
    F_NUMBER(val, -1, dropThreshold, GT_U32);
    F_NUMBER(val, -1, packetThreshold, GT_U32);
    F_STRUCT(val, -1, byteThreshold, GT_U64);
}

void prv_c_to_lua_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC(
    lua_State *L,
    CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, action, CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT);
    FO_NUMBER(val, t, dropThreshold, GT_U32);
    FO_NUMBER(val, t, packetThreshold, GT_U32);
    FO_STRUCT(val, t, byteThreshold, GT_U64);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *val
)
{
    F_ENUM(val, -1, tsAction, CPSS_DXCH_PTP_TS_ACTION_ENT);
    F_BOOL(val, -1, ingrLinkDelayEnable);
    F_BOOL(val, -1, packetDispatchingEnable);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, tsAction, CPSS_DXCH_PTP_TS_ACTION_ENT);
    FO_BOOL(val, t, ingrLinkDelayEnable);
    FO_BOOL(val, t, packetDispatchingEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC);

enumDescr enumDescr_CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_MLL_PAIR_WRITE_FIRST_LAST_BIT_E),
    ENUM_ENTRY(CPSS_DXCH_IP_MLL_PAIR_WRITE_SECOND_LAST_BIT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT);

enumDescr enumDescr_CPSS_DXCH_STC_COUNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_STC_COUNT_ALL_PACKETS_E),
    ENUM_ENTRY(CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_STC_COUNT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC *val
)
{
    F_ENUM(val, -1, op, CPSS_DXCH_PORT_EL_DB_OPERATION_ENT);
    F_NUMBER(val, -1, indexOffset, GT_U16);
    F_NUMBER(val, -1, regOffset, GT_U32);
    F_NUMBER(val, -1, data, GT_U32);
    F_NUMBER(val, -1, mask, GT_U32);
    F_NUMBER(val, -1, waitTime, GT_U16);
    F_NUMBER(val, -1, numOfLoops, GT_U16);
    F_NUMBER(val, -1, delay, GT_U16);
}

void prv_c_to_lua_CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, op, CPSS_DXCH_PORT_EL_DB_OPERATION_ENT);
    FO_NUMBER(val, t, indexOffset, GT_U16);
    FO_NUMBER(val, t, regOffset, GT_U32);
    FO_NUMBER(val, t, data, GT_U32);
    FO_NUMBER(val, t, mask, GT_U32);
    FO_NUMBER(val, t, waitTime, GT_U16);
    FO_NUMBER(val, t, numOfLoops, GT_U16);
    FO_NUMBER(val, t, delay, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_SERDES_OPERATION_CFG_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_STANDARD_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *val
)
{
    F_NUMBER(val, -1, firstIndex, GT_U32);
    F_NUMBER(val, -1, lastIndex, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, firstIndex, GT_U32);
    FO_NUMBER(val, t, lastIndex, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC);

enumDescr enumDescr_CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_QOS_UNTRUST_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_DO_NOT_MODIFY_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_TAG1_UNTAGGED_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_TAG0_UNTAGGED_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_FDB_DEL_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_FDB_DEL_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TTI_MPLS_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_MPLS_RULE_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    F_NUMBER(val, -1, label0, GT_U32);
    F_NUMBER(val, -1, exp0, GT_U32);
    F_NUMBER(val, -1, label1, GT_U32);
    F_NUMBER(val, -1, exp1, GT_U32);
    F_NUMBER(val, -1, label2, GT_U32);
    F_NUMBER(val, -1, exp2, GT_U32);
    F_NUMBER(val, -1, numOfLabels, GT_U32);
    F_NUMBER(val, -1, protocolAboveMPLS, GT_U32);
    F_BOOL(val, -1, reservedLabelExist);
    F_NUMBER(val, -1, reservedLabelValue, GT_U32);
    F_NUMBER(val, -1, channelTypeProfile, GT_U32);
    F_NUMBER(val, -1, dataAfterInnerLabel, GT_U32);
    F_NUMBER(val, -1, cwFirstNibble, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TTI_MPLS_RULE_STC(
    lua_State *L,
    CPSS_DXCH_TTI_MPLS_RULE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_TTI_RULE_COMMON_STC);
    FO_NUMBER(val, t, label0, GT_U32);
    FO_NUMBER(val, t, exp0, GT_U32);
    FO_NUMBER(val, t, label1, GT_U32);
    FO_NUMBER(val, t, exp1, GT_U32);
    FO_NUMBER(val, t, label2, GT_U32);
    FO_NUMBER(val, t, exp2, GT_U32);
    FO_NUMBER(val, t, numOfLabels, GT_U32);
    FO_NUMBER(val, t, protocolAboveMPLS, GT_U32);
    FO_BOOL(val, t, reservedLabelExist);
    FO_NUMBER(val, t, reservedLabelValue, GT_U32);
    FO_NUMBER(val, t, channelTypeProfile, GT_U32);
    FO_NUMBER(val, t, dataAfterInnerLabel, GT_U32);
    FO_NUMBER(val, t, cwFirstNibble, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_MPLS_RULE_STC);

void prv_lua_to_c_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC *val
)
{
    F_BOOL(val, -1, countingEnable);
    F_BOOL(val, -1, meteringEnable);
    F_NUMBER(val, -1, policerPointer, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, countingEnable);
    FO_BOOL(val, t, meteringEnable);
    FO_NUMBER(val, t, policerPointer, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC *val
)
{
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_NUMBER(val, -1, pclId, GT_U32);
    F_NUMBER(val, -1, vid, GT_U32);
    F_NUMBER(val, -1, srcPort, GT_U32);
    F_NUMBER(val, -1, srcDevIsOwn, GT_U8);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, macToMe, GT_U8);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_NUMBER(val, -1, flowId, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_NUMBER(val, t, pclId, GT_U32);
    FO_NUMBER(val, t, vid, GT_U32);
    FO_NUMBER(val, t, srcPort, GT_U32);
    FO_NUMBER(val, t, srcDevIsOwn, GT_U8);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, macToMe, GT_U8);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_NUMBER(val, t, flowId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC *val
)
{
    F_NUMBER(val, -1, twoWayFrameDelay, GT_U32);
    F_NUMBER(val, -1, oneWayFrameDelayForward, GT_U32);
    F_NUMBER(val, -1, oneWayFrameDelayBackward, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, twoWayFrameDelay, GT_U32);
    FO_NUMBER(val, t, oneWayFrameDelayForward, GT_U32);
    FO_NUMBER(val, t, oneWayFrameDelayBackward, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC);

enumDescr enumDescr_CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_FDB_ENTRY_CMD_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_INVALID_SA_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_INVALID_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_VLAN_RANGE_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_ARP_SA_MISMATCH_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_SYN_WITH_DATA_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_ACCESS_MATRIX_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_SEC_LEARNING_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_FRAG_ICMP_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_VLAN_MRU_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_LOCAL_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_IP_MC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_NON_IP_MC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_IEEE_RESERVED_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_L2_UC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_BYTES_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_PACKETS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC(
    lua_State *L,
    CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC *val
)
{
    F_ENUM(val, -1, uploadMode, CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT);
    F_STRUCT(val, -1, timer, CPSS_DXCH_IPFIX_TIMER_STC);
}

void prv_c_to_lua_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC(
    lua_State *L,
    CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, uploadMode, CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT);
    FO_STRUCT(val, t, timer, CPSS_DXCH_IPFIX_TIMER_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC *val
)
{
    F_ENUM(val, -1, invalidPtpPktCmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, invalidOutPiggybackPktCmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, invalidInPiggybackPktCmd, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, invalidTsPktCmd, CPSS_PACKET_CMD_ENT);
}

void prv_c_to_lua_CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, invalidPtpPktCmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, invalidOutPiggybackPktCmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, invalidInPiggybackPktCmd, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, invalidTsPktCmd, CPSS_PACKET_CMD_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC);

void prv_lua_to_c_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, targetEport, GT_PORT_NUM);
    F_NUMBER(val, -1, targetHwDevice, GT_HW_DEV_NUM);
}

void prv_c_to_lua_CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, targetEport, GT_PORT_NUM);
    FO_NUMBER(val, t, targetHwDevice, GT_HW_DEV_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_L2_ECMP_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_FABRIC_HGL_RX_DSA_STC(
    lua_State *L,
    CPSS_DXCH_FABRIC_HGL_RX_DSA_STC *val
)
{
    F_NUMBER(val, -1, vid, GT_U32);
    F_NUMBER(val, -1, srcId, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_FABRIC_HGL_RX_DSA_STC(
    lua_State *L,
    CPSS_DXCH_FABRIC_HGL_RX_DSA_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, vid, GT_U32);
    FO_NUMBER(val, t, srcId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_FABRIC_HGL_RX_DSA_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT);

void prv_lua_to_c_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC(
    lua_State *L,
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC *val
)
{
    F_NUMBER(val, -1, clientGroup, GT_U32);
    F_NUMBER(val, -1, hitNumber, GT_U32);
    F_ENUM(val, -1, ruleSize, CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT);
    F_BOOL(val, -1, autoResize);
    F_NUMBER(val, -1, guaranteedNumOfRules, GT_U32);
    F_ENUM(val, -1, ruleAdditionMethod, CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT);
}

void prv_c_to_lua_CPSS_DXCH_VIRTUAL_TCAM_INFO_STC(
    lua_State *L,
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, clientGroup, GT_U32);
    FO_NUMBER(val, t, hitNumber, GT_U32);
    FO_ENUM(val, t, ruleSize, CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT);
    FO_BOOL(val, t, autoResize);
    FO_NUMBER(val, t, guaranteedNumOfRules, GT_U32);
    FO_ENUM(val, t, ruleAdditionMethod, CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_VIRTUAL_TCAM_INFO_STC);

void prv_lua_to_c_CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC(
    lua_State *L,
    CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC *val
)
{
    F_NUMBER(val, -1, etherType, GT_U32);
    F_NUMBER(val, -1, prioVlanId, GT_U32);
    F_NUMBER(val, -1, cpId, GT_U32);
    F_NUMBER(val, -1, version, GT_U32);
    F_NUMBER(val, -1, macLowByte, GT_U8);
}

void prv_c_to_lua_CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC(
    lua_State *L,
    CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, etherType, GT_U32);
    FO_NUMBER(val, t, prioVlanId, GT_U32);
    FO_NUMBER(val, t, cpId, GT_U32);
    FO_NUMBER(val, t, version, GT_U32);
    FO_NUMBER(val, t, macLowByte, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC);

void prv_lua_to_c_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC *val
)
{
    F_NUMBER(val, -1, rxInPkts, GT_U32);
    F_NUMBER(val, -1, rxInOctets, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, rxInPkts, GT_U32);
    FO_NUMBER(val, t, rxInOctets, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC *val
)
{
    F_BOOL(val, -1, ptpOverEhernetTsEnable);
    F_BOOL(val, -1, ptpOverUdpIpv4TsEnable);
    F_BOOL(val, -1, ptpOverUdpIpv6TsEnable);
    F_NUMBER(val, -1, messageTypeTsEnableBmp, GT_U32);
    F_NUMBER(val, -1, transportSpecificTsEnableBmp, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, ptpOverEhernetTsEnable);
    FO_BOOL(val, t, ptpOverUdpIpv4TsEnable);
    FO_BOOL(val, t, ptpOverUdpIpv6TsEnable);
    FO_NUMBER(val, t, messageTypeTsEnableBmp, GT_U32);
    FO_NUMBER(val, t, transportSpecificTsEnableBmp, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC *val
)
{
    F_ARRAY_START(val, -1, rxQueueNum);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, rxQueueNum, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, rxQueueNum);
    F_ARRAY_START(val, -1, txQueueNum);
    {
        int idx;
        for (idx = 0; idx < 1; idx++) {
            F_ARRAY_NUMBER(val, txQueueNum, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, txQueueNum);
    F_NUMBER(val, -1, downMepsNum, GT_U32);
    F_NUMBER(val, -1, upMepsNum, GT_U32);
    F_NUMBER(val, -1, rxCcmFlowsNum, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, rxQueueNum);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, rxQueueNum, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, rxQueueNum);
    FO_ARRAY_START(val, t, txQueueNum);
    {
        int idx;
        for (idx = 0; idx < 1; idx++) {
            FO_ARRAY_NUMBER(val, txQueueNum, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, txQueueNum);
    FO_NUMBER(val, t, downMepsNum, GT_U32);
    FO_NUMBER(val, t, upMepsNum, GT_U32);
    FO_NUMBER(val, t, rxCcmFlowsNum, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC);

void prv_lua_to_c_CPSS_DXCH_DETAILED_PORT_MAP_STC(
    lua_State *L,
    CPSS_DXCH_DETAILED_PORT_MAP_STC *val
)
{
    F_BOOL(val, -1, valid);
    F_STRUCT(val, -1, portMap, CPSS_DXCH_SHADOW_PORT_MAP_STC);
}

void prv_c_to_lua_CPSS_DXCH_DETAILED_PORT_MAP_STC(
    lua_State *L,
    CPSS_DXCH_DETAILED_PORT_MAP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, valid);
    FO_STRUCT(val, t, portMap, CPSS_DXCH_SHADOW_PORT_MAP_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_DETAILED_PORT_MAP_STC);

void prv_lua_to_c_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC(
    lua_State *L,
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *val
)
{
    F_ARRAY_START(val, -1, portsCmd);
    {
        int idx;
        for (idx = 0; idx < 512; idx++) {
            F_ARRAY_ENUM(val, portsCmd, idx, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
        }
    }
    F_ARRAY_END(val, -1, portsCmd);
}

void prv_c_to_lua_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC(
    lua_State *L,
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, portsCmd);
    {
        int idx;
        for (idx = 0; idx < 512; idx++) {
            FO_ARRAY_ENUM(val, portsCmd, idx, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
        }
    }
    FO_ARRAY_END(val, t, portsCmd);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC);

void prv_lua_to_c_CPSS_DXCH_TTI_ACTION_2_STC(
    lua_State *L,
    CPSS_DXCH_TTI_ACTION_2_STC *val
)
{
    F_BOOL(val, -1, tunnelTerminate);
    F_ENUM(val, -1, ttPassengerPacketType, CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
    F_ENUM(val, -1, tsPassengerPacketType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    F_NUMBER(val, -1, ttHeaderLength, GT_U32);
    F_BOOL(val, -1, continueToNextTtiLookup);
    F_BOOL(val, -1, copyTtlExpFromTunnelHeader);
    F_ENUM(val, -1, mplsCommand, CPSS_DXCH_TTI_MPLS_CMD_ENT);
    F_NUMBER(val, -1, mplsTtl, GT_U32);
    F_BOOL(val, -1, enableDecrementTtl);
    F_ENUM(val, -1, passengerParsingOfTransitMplsTunnelMode, CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT);
    F_BOOL(val, -1, passengerParsingOfTransitNonMplsTransitTunnelEnable);
    F_ENUM(val, -1, command, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, redirectCommand, CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
    F_STRUCT(val, -1, egressInterface, CPSS_INTERFACE_INFO_STC);
    F_NUMBER(val, -1, arpPtr, GT_U32);
    F_BOOL(val, -1, tunnelStart);
    F_NUMBER(val, -1, tunnelStartPtr, GT_U32);
    F_NUMBER(val, -1, routerLttPtr, GT_U32);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_BOOL(val, -1, sourceIdSetEnable);
    F_NUMBER(val, -1, sourceId, GT_U32);
    F_ENUM(val, -1, tag0VlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    F_NUMBER(val, -1, tag0VlanId, GT_U16);
    F_ENUM(val, -1, tag1VlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    F_NUMBER(val, -1, tag1VlanId, GT_U16);
    F_ENUM(val, -1, tag0VlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_BOOL(val, -1, nestedVlanEnable);
    F_BOOL(val, -1, bindToPolicerMeter);
    F_BOOL(val, -1, bindToPolicer);
    F_NUMBER(val, -1, policerIndex, GT_U32);
    F_ENUM(val, -1, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_BOOL(val, -1, keepPreviousQoS);
    F_BOOL(val, -1, trustUp);
    F_BOOL(val, -1, trustDscp);
    F_BOOL(val, -1, trustExp);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_ENUM(val, -1, modifyTag0Up, CPSS_DXCH_TTI_MODIFY_UP_ENT);
    F_ENUM(val, -1, tag1UpCommand, CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT);
    F_ENUM(val, -1, modifyDscp, CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
    F_NUMBER(val, -1, tag0Up, GT_U32);
    F_NUMBER(val, -1, tag1Up, GT_U32);
    F_BOOL(val, -1, remapDSCP);
    F_BOOL(val, -1, qosUseUpAsIndexEnable);
    F_NUMBER(val, -1, qosMappingTableIndex, GT_U32);
    F_BOOL(val, -1, mplsLLspQoSProfileEnable);
    F_ENUM(val, -1, pcl0OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    F_ENUM(val, -1, pcl0_1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    F_ENUM(val, -1, pcl1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    F_NUMBER(val, -1, iPclConfigIndex, GT_U32);
    F_BOOL(val, -1, iPclUdbConfigTableEnable);
    F_ENUM(val, -1, iPclUdbConfigTableIndex, CPSS_DXCH_PCL_PACKET_TYPE_ENT);
    F_BOOL(val, -1, mirrorToIngressAnalyzerEnable);
    F_NUMBER(val, -1, mirrorToIngressAnalyzerIndex, GT_U32);
    F_ENUM(val, -1, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    F_BOOL(val, -1, bindToCentralCounter);
    F_NUMBER(val, -1, centralCounterIndex, GT_U32);
    F_BOOL(val, -1, vntl2Echo);
    F_BOOL(val, -1, bridgeBypass);
    F_BOOL(val, -1, ingressPipeBypass);
    F_BOOL(val, -1, actionStop);
    F_NUMBER(val, -1, hashMaskIndex, GT_U32);
    F_BOOL(val, -1, modifyMacSa);
    F_BOOL(val, -1, modifyMacDa);
    F_BOOL(val, -1, ResetSrcPortGroupId);
    F_BOOL(val, -1, multiPortGroupTtiEnable);
    F_BOOL(val, -1, sourceEPortAssignmentEnable);
    F_NUMBER(val, -1, sourceEPort, GT_PORT_NUM);
    F_NUMBER(val, -1, flowId, GT_U32);
    F_BOOL(val, -1, setMacToMe);
    F_BOOL(val, -1, rxProtectionSwitchEnable);
    F_BOOL(val, -1, rxIsProtectionPath);
    F_ENUM(val, -1, pwTagMode, CPSS_DXCH_TTI_PW_TAG_MODE_ENT);
    F_BOOL(val, -1, oamTimeStampEnable);
    F_NUMBER(val, -1, oamOffsetIndex, GT_U32);
    F_BOOL(val, -1, oamProcessEnable);
    F_NUMBER(val, -1, oamProfile, GT_U32);
    F_BOOL(val, -1, oamChannelTypeToOpcodeMappingEnable);
    F_BOOL(val, -1, isPtpPacket);
    F_ENUM(val, -1, ptpTriggerType, CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT);
    F_NUMBER(val, -1, ptpOffset, GT_U32);
    F_BOOL(val, -1, cwBasedPw);
    F_BOOL(val, -1, ttlExpiryVccvEnable);
    F_BOOL(val, -1, pwe3FlowLabelExist);
    F_BOOL(val, -1, pwCwBasedETreeEnable);
    F_BOOL(val, -1, applyNonDataCwCommand);
    F_BOOL(val, -1, unknownSaCommandEnable);
    F_ENUM(val, -1, unknownSaCommand, CPSS_PACKET_CMD_ENT);
    F_BOOL(val, -1, sourceMeshIdSetEnable);
    F_NUMBER(val, -1, sourceMeshId, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TTI_ACTION_2_STC(
    lua_State *L,
    CPSS_DXCH_TTI_ACTION_2_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tunnelTerminate);
    FO_ENUM(val, t, ttPassengerPacketType, CPSS_DXCH_TTI_PASSENGER_TYPE_ENT);
    FO_ENUM(val, t, tsPassengerPacketType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    FO_NUMBER(val, t, ttHeaderLength, GT_U32);
    FO_BOOL(val, t, continueToNextTtiLookup);
    FO_BOOL(val, t, copyTtlExpFromTunnelHeader);
    FO_ENUM(val, t, mplsCommand, CPSS_DXCH_TTI_MPLS_CMD_ENT);
    FO_NUMBER(val, t, mplsTtl, GT_U32);
    FO_BOOL(val, t, enableDecrementTtl);
    FO_ENUM(val, t, passengerParsingOfTransitMplsTunnelMode, CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT);
    FO_BOOL(val, t, passengerParsingOfTransitNonMplsTransitTunnelEnable);
    FO_ENUM(val, t, command, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, redirectCommand, CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT);
    FO_STRUCT(val, t, egressInterface, CPSS_INTERFACE_INFO_STC);
    FO_NUMBER(val, t, arpPtr, GT_U32);
    FO_BOOL(val, t, tunnelStart);
    FO_NUMBER(val, t, tunnelStartPtr, GT_U32);
    FO_NUMBER(val, t, routerLttPtr, GT_U32);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_BOOL(val, t, sourceIdSetEnable);
    FO_NUMBER(val, t, sourceId, GT_U32);
    FO_ENUM(val, t, tag0VlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    FO_NUMBER(val, t, tag0VlanId, GT_U16);
    FO_ENUM(val, t, tag1VlanCmd, CPSS_DXCH_TTI_VLAN_COMMAND_ENT);
    FO_NUMBER(val, t, tag1VlanId, GT_U16);
    FO_ENUM(val, t, tag0VlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_BOOL(val, t, nestedVlanEnable);
    FO_BOOL(val, t, bindToPolicerMeter);
    FO_BOOL(val, t, bindToPolicer);
    FO_NUMBER(val, t, policerIndex, GT_U32);
    FO_ENUM(val, t, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_BOOL(val, t, keepPreviousQoS);
    FO_BOOL(val, t, trustUp);
    FO_BOOL(val, t, trustDscp);
    FO_BOOL(val, t, trustExp);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_ENUM(val, t, modifyTag0Up, CPSS_DXCH_TTI_MODIFY_UP_ENT);
    FO_ENUM(val, t, tag1UpCommand, CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT);
    FO_ENUM(val, t, modifyDscp, CPSS_DXCH_TTI_MODIFY_DSCP_ENT);
    FO_NUMBER(val, t, tag0Up, GT_U32);
    FO_NUMBER(val, t, tag1Up, GT_U32);
    FO_BOOL(val, t, remapDSCP);
    FO_BOOL(val, t, qosUseUpAsIndexEnable);
    FO_NUMBER(val, t, qosMappingTableIndex, GT_U32);
    FO_BOOL(val, t, mplsLLspQoSProfileEnable);
    FO_ENUM(val, t, pcl0OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    FO_ENUM(val, t, pcl0_1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    FO_ENUM(val, t, pcl1OverrideConfigIndex, CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);
    FO_NUMBER(val, t, iPclConfigIndex, GT_U32);
    FO_BOOL(val, t, iPclUdbConfigTableEnable);
    FO_ENUM(val, t, iPclUdbConfigTableIndex, CPSS_DXCH_PCL_PACKET_TYPE_ENT);
    FO_BOOL(val, t, mirrorToIngressAnalyzerEnable);
    FO_NUMBER(val, t, mirrorToIngressAnalyzerIndex, GT_U32);
    FO_ENUM(val, t, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_BOOL(val, t, bindToCentralCounter);
    FO_NUMBER(val, t, centralCounterIndex, GT_U32);
    FO_BOOL(val, t, vntl2Echo);
    FO_BOOL(val, t, bridgeBypass);
    FO_BOOL(val, t, ingressPipeBypass);
    FO_BOOL(val, t, actionStop);
    FO_NUMBER(val, t, hashMaskIndex, GT_U32);
    FO_BOOL(val, t, modifyMacSa);
    FO_BOOL(val, t, modifyMacDa);
    FO_BOOL(val, t, ResetSrcPortGroupId);
    FO_BOOL(val, t, multiPortGroupTtiEnable);
    FO_BOOL(val, t, sourceEPortAssignmentEnable);
    FO_NUMBER(val, t, sourceEPort, GT_PORT_NUM);
    FO_NUMBER(val, t, flowId, GT_U32);
    FO_BOOL(val, t, setMacToMe);
    FO_BOOL(val, t, rxProtectionSwitchEnable);
    FO_BOOL(val, t, rxIsProtectionPath);
    FO_ENUM(val, t, pwTagMode, CPSS_DXCH_TTI_PW_TAG_MODE_ENT);
    FO_BOOL(val, t, oamTimeStampEnable);
    FO_NUMBER(val, t, oamOffsetIndex, GT_U32);
    FO_BOOL(val, t, oamProcessEnable);
    FO_NUMBER(val, t, oamProfile, GT_U32);
    FO_BOOL(val, t, oamChannelTypeToOpcodeMappingEnable);
    FO_BOOL(val, t, isPtpPacket);
    FO_ENUM(val, t, ptpTriggerType, CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT);
    FO_NUMBER(val, t, ptpOffset, GT_U32);
    FO_BOOL(val, t, cwBasedPw);
    FO_BOOL(val, t, ttlExpiryVccvEnable);
    FO_BOOL(val, t, pwe3FlowLabelExist);
    FO_BOOL(val, t, pwCwBasedETreeEnable);
    FO_BOOL(val, t, applyNonDataCwCommand);
    FO_BOOL(val, t, unknownSaCommandEnable);
    FO_ENUM(val, t, unknownSaCommand, CPSS_PACKET_CMD_ENT);
    FO_BOOL(val, t, sourceMeshIdSetEnable);
    FO_NUMBER(val, t, sourceMeshId, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_ACTION_2_STC);

enumDescr enumDescr_CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E),
    ENUM_ENTRY(CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_SERDES_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SERDES_MODE_COMBO_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SERDES_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_NET_RX_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_RX_PARAMS_STC *val
)
{
    F_STRUCT(val, -1, dsaParam, CPSS_DXCH_NET_DSA_PARAMS_STC);
}

void prv_c_to_lua_CPSS_DXCH_NET_RX_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_RX_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, dsaParam, CPSS_DXCH_NET_DSA_PARAMS_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_NET_RX_PARAMS_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_L3_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_L2_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT);

enumDescr enumDescr_CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_ALLOC_MID_E),
    ENUM_ENTRY(CPSS_DXCH_TCAM_MANAGER_ALLOC_MAX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT);

void prv_lua_to_c_CPSS_DXCH_LPM_RAM_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_LPM_RAM_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, numOfBlocks, GT_U32);
    F_ARRAY_START(val, -1, blocksSizeArray);
    {
        int idx;
        for (idx = 0; idx < CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS; idx++) {
            F_ARRAY_NUMBER(val, blocksSizeArray, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, blocksSizeArray);
    F_ENUM(val, -1, blocksAllocationMethod, CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT);
}

void prv_c_to_lua_CPSS_DXCH_LPM_RAM_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_LPM_RAM_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, numOfBlocks, GT_U32);
    FO_ARRAY_START(val, t, blocksSizeArray);
    {
        int idx;
        for (idx = 0; idx < CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS; idx++) {
            FO_ARRAY_NUMBER(val, blocksSizeArray, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, blocksSizeArray);
    FO_ENUM(val, t, blocksAllocationMethod, CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_LPM_RAM_CONFIG_STC);

void prv_lua_to_c_CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC(
    lua_State *L,
    CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC *val
)
{
    F_NUMBER(val, -1, mcFifo0, GT_U32);
    F_NUMBER(val, -1, mcFifo1, GT_U32);
    F_NUMBER(val, -1, mcFifo2, GT_U32);
    F_NUMBER(val, -1, mcFifo3, GT_U32);
}

void prv_c_to_lua_CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC(
    lua_State *L,
    CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, mcFifo0, GT_U32);
    FO_NUMBER(val, t, mcFifo1, GT_U32);
    FO_NUMBER(val, t, mcFifo2, GT_U32);
    FO_NUMBER(val, t, mcFifo3, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC);

void prv_lua_to_c_CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, sqlch, GT_U32);
    F_NUMBER(val, -1, ffeRes, GT_U32);
    F_NUMBER(val, -1, ffeCap, GT_U32);
    F_NUMBER(val, -1, align90, GT_U32);
    F_NUMBER(val, -1, dcGain, GT_U32);
    F_NUMBER(val, -1, bandWidth, GT_U32);
    F_NUMBER(val, -1, loopBandwidth, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, sqlch, GT_U32);
    FO_NUMBER(val, t, ffeRes, GT_U32);
    FO_NUMBER(val, t, ffeCap, GT_U32);
    FO_NUMBER(val, t, align90, GT_U32);
    FO_NUMBER(val, t, dcGain, GT_U32);
    FO_NUMBER(val, t, bandWidth, GT_U32);
    FO_NUMBER(val, t, loopBandwidth, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_SERDES_RX_CONFIG_STC);

void prv_lua_to_c_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC *val
)
{
    F_STRUCT(val, -1, duMngCntr, GT_U64);
    F_NUMBER(val, -1, packetMngCntr, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, duMngCntr, GT_U64);
    FO_NUMBER(val, t, packetMngCntr, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC *val
)
{
    F_ENUM(val, -1, selectType, CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT);
    F_NUMBER(val, -1, bitSelector, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, selectType, CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT);
    FO_NUMBER(val, t, bitSelector, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, srcPortTrunk, GT_U32);
    F_NUMBER(val, -1, srcIsTrunk, GT_U32);
    F_NUMBER(val, -1, srcDev, GT_U8);
    F_NUMBER(val, -1, vid, GT_U16);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_NUMBER(val, -1, label1, GT_U32);
    F_NUMBER(val, -1, sBit1, GT_U32);
    F_NUMBER(val, -1, exp1, GT_U32);
    F_NUMBER(val, -1, label2, GT_U32);
    F_NUMBER(val, -1, sBit2, GT_U32);
    F_NUMBER(val, -1, exp2, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, srcPortTrunk, GT_U32);
    FO_NUMBER(val, t, srcIsTrunk, GT_U32);
    FO_NUMBER(val, t, srcDev, GT_U8);
    FO_NUMBER(val, t, vid, GT_U16);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_NUMBER(val, t, label1, GT_U32);
    FO_NUMBER(val, t, sBit1, GT_U32);
    FO_NUMBER(val, t, exp1, GT_U32);
    FO_NUMBER(val, t, label2, GT_U32);
    FO_NUMBER(val, t, sBit2, GT_U32);
    FO_NUMBER(val, t, exp2, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_CONST_0_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_CONST_1_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_0_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_1_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_2_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EPORT_TS_EXT_3_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_0_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_1_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_EVLAN_I_SID_2_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_SHIFT_HASH_0_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_SHIFT_HASH_1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_QOS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_QOS_KEEP_PREVIOUS_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_DSCP_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_QOS_TRUST_MPLS_EXP_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_QOS_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_AP_INTROP_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AP_INTROP_STC *val
)
{
    F_NUMBER(val, -1, attrBitMask, GT_U16);
    F_NUMBER(val, -1, txDisDuration, GT_U16);
    F_NUMBER(val, -1, abilityDuration, GT_U16);
    F_NUMBER(val, -1, abilityMaxInterval, GT_U16);
    F_NUMBER(val, -1, abilityFailMaxInterval, GT_U16);
    F_NUMBER(val, -1, apLinkDuration, GT_U16);
    F_NUMBER(val, -1, apLinkMaxInterval, GT_U16);
    F_NUMBER(val, -1, pdLinkDuration, GT_U16);
    F_NUMBER(val, -1, pdLinkMaxInterval, GT_U16);
}

void prv_c_to_lua_CPSS_DXCH_PORT_AP_INTROP_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AP_INTROP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, attrBitMask, GT_U16);
    FO_NUMBER(val, t, txDisDuration, GT_U16);
    FO_NUMBER(val, t, abilityDuration, GT_U16);
    FO_NUMBER(val, t, abilityMaxInterval, GT_U16);
    FO_NUMBER(val, t, abilityFailMaxInterval, GT_U16);
    FO_NUMBER(val, t, apLinkDuration, GT_U16);
    FO_NUMBER(val, t, apLinkMaxInterval, GT_U16);
    FO_NUMBER(val, t, pdLinkDuration, GT_U16);
    FO_NUMBER(val, t, pdLinkMaxInterval, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_AP_INTROP_STC);

enumDescr enumDescr_CPSS_DXCH_IP_CPU_CODE_INDEX_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_CPU_CODE_IDX_0_E),
    ENUM_ENTRY(CPSS_DXCH_IP_CPU_CODE_IDX_1_E),
    ENUM_ENTRY(CPSS_DXCH_IP_CPU_CODE_IDX_2_E),
    ENUM_ENTRY(CPSS_DXCH_IP_CPU_CODE_IDX_3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_CPU_CODE_INDEX_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, srcPortTrunk, GT_U32);
    F_NUMBER(val, -1, srcIsTrunk, GT_U32);
    F_NUMBER(val, -1, srcDev, GT_U8);
    F_NUMBER(val, -1, vid, GT_U16);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, srcIp, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, destIp, GT_IPADDR);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, srcPortTrunk, GT_U32);
    FO_NUMBER(val, t, srcIsTrunk, GT_U32);
    FO_NUMBER(val, t, srcDev, GT_U8);
    FO_NUMBER(val, t, vid, GT_U16);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, srcIp, GT_IPADDR);
    FO_STRUCT(val, t, destIp, GT_IPADDR);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC);

void prv_lua_to_c_CPSS_DXCH_IP_NAT_ENTRY_UNT(
    lua_State *L,
    CPSS_DXCH_IP_NAT_ENTRY_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DXCH_IP_NAT_ENTRY_UNT(
    lua_State *L,
    CPSS_DXCH_IP_NAT_ENTRY_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, nat44Entry, CPSS_DXCH_IP_NAT44_ENTRY_STC);
    FO_STRUCT(val, t, nat66Entry, CPSS_DXCH_IP_NAT66_ENTRY_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_NAT_ENTRY_UNT);

enumDescr enumDescr_CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_1_TCAM_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_2_AND_2_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_1_AND_3_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_3_AND_1_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_4_TCAMS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC(
    lua_State *L,
    CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC *val
)
{
    F_ENUM(val, -1, dropMode, CPSS_DROP_MODE_TYPE_ENT);
    F_ENUM(val, -1, rMode, CPSS_RATE_LIMIT_MODE_ENT);
    F_NUMBER(val, -1, win10Mbps, GT_U32);
    F_NUMBER(val, -1, win100Mbps, GT_U32);
    F_NUMBER(val, -1, win1000Mbps, GT_U32);
    F_NUMBER(val, -1, win10Gbps, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC(
    lua_State *L,
    CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dropMode, CPSS_DROP_MODE_TYPE_ENT);
    FO_ENUM(val, t, rMode, CPSS_RATE_LIMIT_MODE_ENT);
    FO_NUMBER(val, t, win10Mbps, GT_U32);
    FO_NUMBER(val, t, win100Mbps, GT_U32);
    FO_NUMBER(val, t, win1000Mbps, GT_U32);
    FO_NUMBER(val, t, win10Gbps, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC);

void prv_lua_to_c_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC(
    lua_State *L,
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC *val
)
{
    F_ENUM(val, -1, entryType, CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);
    F_NUMBER(val, -1, amount, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC(
    lua_State *L,
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, entryType, CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT);
    FO_NUMBER(val, t, amount, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC);

enumDescr enumDescr_CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC(
    lua_State *L,
    CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC *val
)
{
    F_BOOL(val, -1, tunnelStart);
    F_NUMBER(val, -1, tunnelStartPtr, GT_U32);
    F_ENUM(val, -1, tsPassengerPacketType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    F_NUMBER(val, -1, arpPtr, GT_U32);
    F_BOOL(val, -1, modifyMacSa);
    F_BOOL(val, -1, modifyMacDa);
}

void prv_c_to_lua_CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC(
    lua_State *L,
    CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tunnelStart);
    FO_NUMBER(val, t, tunnelStartPtr, GT_U32);
    FO_ENUM(val, t, tsPassengerPacketType, CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT);
    FO_NUMBER(val, t, arpPtr, GT_U32);
    FO_BOOL(val, t, modifyMacSa);
    FO_BOOL(val, t, modifyMacDa);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC);

void prv_lua_to_c_CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC *val
)
{
    F_ENUM(val, -1, offsetCmd, CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT);
    F_NUMBER(val, -1, offsetValue, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, offsetCmd, CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT);
    FO_NUMBER(val, t, offsetValue, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC);

void prv_lua_to_c_CPSS_DXCH_PORT_MAC_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_MAC_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, macNum, GT_U32);
    F_NUMBER(val, -1, macPortGroupNum, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_MAC_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_MAC_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, macNum, GT_U32);
    FO_NUMBER(val, t, macPortGroupNum, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_MAC_PARAMS_STC);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_AP_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AP_PARAMS_STC *val
)
{
    F_BOOL(val, -1, fcPause);
    F_ENUM(val, -1, fcAsmDir, CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT);
    F_BOOL(val, -1, fecSupported);
    F_BOOL(val, -1, fecRequired);
    F_BOOL(val, -1, noneceDisable);
    F_NUMBER(val, -1, laneNum, GT_U32);
    F_ARRAY_START(val, -1, modesAdvertiseArr);
    {
        int idx;
        for (idx = 0; idx < 10; idx++) {
            F_ARRAY_STRUCT(val, modesAdvertiseArr, idx, CPSS_PORT_MODE_SPEED_STC);
        }
    }
    F_ARRAY_END(val, -1, modesAdvertiseArr);
}

void prv_c_to_lua_CPSS_DXCH_PORT_AP_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_AP_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, fcPause);
    FO_ENUM(val, t, fcAsmDir, CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT);
    FO_BOOL(val, t, fecSupported);
    FO_BOOL(val, t, fecRequired);
    FO_BOOL(val, t, noneceDisable);
    FO_NUMBER(val, t, laneNum, GT_U32);
    FO_ARRAY_START(val, t, modesAdvertiseArr);
    {
        int idx;
        for (idx = 0; idx < 10; idx++) {
            FO_ARRAY_STRUCT(val, modesAdvertiseArr, idx, CPSS_PORT_MODE_SPEED_STC);
        }
    }
    FO_ARRAY_END(val, t, modesAdvertiseArr);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_AP_PARAMS_STC);

void prv_lua_to_c_CPSS_DXCH_IP_COUNTER_SET_STC(
    lua_State *L,
    CPSS_DXCH_IP_COUNTER_SET_STC *val
)
{
    F_NUMBER(val, -1, inUcPkts, GT_U32);
    F_NUMBER(val, -1, inMcPkts, GT_U32);
    F_NUMBER(val, -1, inUcNonRoutedExcpPkts, GT_U32);
    F_NUMBER(val, -1, inUcNonRoutedNonExcpPkts, GT_U32);
    F_NUMBER(val, -1, inMcNonRoutedExcpPkts, GT_U32);
    F_NUMBER(val, -1, inMcNonRoutedNonExcpPkts, GT_U32);
    F_NUMBER(val, -1, inUcTrappedMirrorPkts, GT_U32);
    F_NUMBER(val, -1, inMcTrappedMirrorPkts, GT_U32);
    F_NUMBER(val, -1, mcRfpFailPkts, GT_U32);
    F_NUMBER(val, -1, outUcRoutedPkts, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IP_COUNTER_SET_STC(
    lua_State *L,
    CPSS_DXCH_IP_COUNTER_SET_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, inUcPkts, GT_U32);
    FO_NUMBER(val, t, inMcPkts, GT_U32);
    FO_NUMBER(val, t, inUcNonRoutedExcpPkts, GT_U32);
    FO_NUMBER(val, t, inUcNonRoutedNonExcpPkts, GT_U32);
    FO_NUMBER(val, t, inMcNonRoutedExcpPkts, GT_U32);
    FO_NUMBER(val, t, inMcNonRoutedNonExcpPkts, GT_U32);
    FO_NUMBER(val, t, inUcTrappedMirrorPkts, GT_U32);
    FO_NUMBER(val, t, inMcTrappedMirrorPkts, GT_U32);
    FO_NUMBER(val, t, mcRfpFailPkts, GT_U32);
    FO_NUMBER(val, t, outUcRoutedPkts, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_COUNTER_SET_STC);

enumDescr enumDescr_CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_UDB21_UDB22_BY_FLOW_ID_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_MPLS_LABEL0_BY_EVLAN_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_MPLS_LABEL1_BY_ORIGINAL_SOURCE_EPORT_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_MPLS_LABEL2_BY_LOCAL_SOURCE_EPORT_E),
    ENUM_ENTRY(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_IPV6_ADDR_24MSB_BY_24UDBS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT);

enumDescr enumDescr_CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_L2MLL_DISREGARD_VLAN_CNT_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_L2MLL_USE_VLAN_CNT_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC *val
)
{
    F_NUMBER(val, -1, txAmp, GT_U32);
    F_BOOL(val, -1, txAmpAdjEn);
    F_NUMBER(val, -1, emph0, GT_U32);
    F_NUMBER(val, -1, emph1, GT_U32);
    F_BOOL(val, -1, txAmpShft);
}

void prv_c_to_lua_CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, txAmp, GT_U32);
    FO_BOOL(val, t, txAmpAdjEn);
    FO_NUMBER(val, t, emph0, GT_U32);
    FO_NUMBER(val, t, emph1, GT_U32);
    FO_BOOL(val, t, txAmpShft);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_SERDES_TX_CONFIG_STC);

void prv_lua_to_c_CPSS_DXCH_NET_DSA_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_COMMON_STC *val
)
{
    F_ENUM(val, -1, dsaTagType, CPSS_DXCH_NET_DSA_TYPE_ENT);
    F_NUMBER(val, -1, vpt, GT_U8);
    F_NUMBER(val, -1, cfiBit, GT_U8);
    F_NUMBER(val, -1, vid, GT_U16);
    F_BOOL(val, -1, dropOnSource);
    F_BOOL(val, -1, packetIsLooped);
}

void prv_c_to_lua_CPSS_DXCH_NET_DSA_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_COMMON_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dsaTagType, CPSS_DXCH_NET_DSA_TYPE_ENT);
    FO_NUMBER(val, t, vpt, GT_U8);
    FO_NUMBER(val, t, cfiBit, GT_U8);
    FO_NUMBER(val, t, vid, GT_U16);
    FO_BOOL(val, t, dropOnSource);
    FO_BOOL(val, t, packetIsLooped);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_NET_DSA_COMMON_STC);

enumDescr enumDescr_CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_UDB_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_MAC_DA_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_EXP_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_QOS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC *val
)
{
    F_STRUCT(val, -1, greenCntr, GT_U64);
    F_STRUCT(val, -1, yellowCntr, GT_U64);
    F_STRUCT(val, -1, redCntr, GT_U64);
    F_ENUM(val, -1, billingCntrMode, CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);
    F_BOOL(val, -1, billingCntrAllEnable);
    F_ENUM(val, -1, lmCntrCaptureMode, CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT);
    F_STRUCT(val, -1, greenCntrSnapshot, GT_U64);
    F_BOOL(val, -1, greenCntrSnapshotValid);
    F_ENUM(val, -1, packetSizeMode, CPSS_POLICER_PACKET_SIZE_MODE_ENT);
    F_ENUM(val, -1, tunnelTerminationPacketSizeMode, CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
    F_ENUM(val, -1, dsaTagCountingMode, CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT);
    F_ENUM(val, -1, timeStampCountingMode, CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
}

void prv_c_to_lua_CPSS_DXCH3_POLICER_BILLING_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, greenCntr, GT_U64);
    FO_STRUCT(val, t, yellowCntr, GT_U64);
    FO_STRUCT(val, t, redCntr, GT_U64);
    FO_ENUM(val, t, billingCntrMode, CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT);
    FO_BOOL(val, t, billingCntrAllEnable);
    FO_ENUM(val, t, lmCntrCaptureMode, CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT);
    FO_STRUCT(val, t, greenCntrSnapshot, GT_U64);
    FO_BOOL(val, t, greenCntrSnapshotValid);
    FO_ENUM(val, t, packetSizeMode, CPSS_POLICER_PACKET_SIZE_MODE_ENT);
    FO_ENUM(val, t, tunnelTerminationPacketSizeMode, CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
    FO_ENUM(val, t, dsaTagCountingMode, CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT);
    FO_ENUM(val, t, timeStampCountingMode, CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC);

enumDescr enumDescr_CPSS_DIAG_PG_IF_SIZE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PG_IF_SIZE_1_BYTE_E),
    ENUM_ENTRY(CPSS_DIAG_PG_IF_SIZE_8_BYTES_E),
    ENUM_ENTRY(CPSS_DIAG_PG_IF_SIZE_16_BYTES_E),
    ENUM_ENTRY(CPSS_DIAG_PG_IF_SIZE_32_BYTES_E),
    ENUM_ENTRY(CPSS_DIAG_PG_IF_SIZE_64_BYTES_E),
    ENUM_ENTRY(CPSS_DIAG_PG_IF_SIZE_DEFAULT_E),
    ENUM_ENTRY(CPSS_DIAG_PG_IF_SIZE_NOT_APPLICABLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PG_IF_SIZE_ENT);

enumDescr enumDescr_CPSS_DXCH_POLICER_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_CMD_NONE_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_CMD_DROP_RED_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_CMD_ENT);

enumDescr enumDescr_CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_IP_DROP_CNT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_COUNT_ALL_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_IP_HEADER_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_DIP_DA_MISMATCH_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_SIP_SA_MISMATCH_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_ILLEGAL_ADDRESS_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_UC_RPF_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_MC_RPF_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_TTL_HOP_LIMIT_EXCEEDED_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_MTU_EXCEEDED_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_OPTION_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_IPV6_SCOPE_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_UC_SIP_FILTER_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_NH_CMD_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_ACCESS_MATRIX_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_IP_DROP_CNT_SIP_ALL_ZEROS_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IP_DROP_CNT_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_IP_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E),
    ENUM_ENTRY(CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_UDP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT);

void prv_lua_to_c_CPSS_DXCH_IPFIX_TIMER_STC(
    lua_State *L,
    CPSS_DXCH_IPFIX_TIMER_STC *val
)
{
    F_NUMBER(val, -1, nanoSecondTimer, GT_U32);
    F_STRUCT(val, -1, secondTimer, GT_U64);
}

void prv_c_to_lua_CPSS_DXCH_IPFIX_TIMER_STC(
    lua_State *L,
    CPSS_DXCH_IPFIX_TIMER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, nanoSecondTimer, GT_U32);
    FO_STRUCT(val, t, secondTimer, GT_U64);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IPFIX_TIMER_STC);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC *val
)
{
    F_STRUCT(val, -1, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    F_STRUCT(val, -1, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC);
    F_NUMBER(val, -1, isArp, GT_U8);
    F_NUMBER(val, -1, isBc, GT_U8);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPADDR);
    F_NUMBER(val, -1, l4Byte0, GT_U8);
    F_NUMBER(val, -1, l4Byte1, GT_U8);
    F_NUMBER(val, -1, l4Byte13, GT_U8);
    F_ARRAY_START(val, -1, udb8_11);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb8_11, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb8_11);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, common, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC);
    FO_STRUCT(val, t, commonStdIp, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC);
    FO_NUMBER(val, t, isArp, GT_U8);
    FO_NUMBER(val, t, isBc, GT_U8);
    FO_STRUCT(val, t, sip, GT_IPADDR);
    FO_STRUCT(val, t, dip, GT_IPADDR);
    FO_NUMBER(val, t, l4Byte0, GT_U8);
    FO_NUMBER(val, t, l4Byte1, GT_U8);
    FO_NUMBER(val, t, l4Byte13, GT_U8);
    FO_ARRAY_START(val, t, udb8_11);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb8_11, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb8_11);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC);

void prv_lua_to_c_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC(
    lua_State *L,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC *val
)
{
    F_NUMBER(val, -1, priority, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC(
    lua_State *L,
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, priority, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC);

enumDescr enumDescr_CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC *val
)
{
    F_BOOL(val, -1, tagEnable);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_ENUM(val, -1, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, up, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_NUMBER(val, -1, numLabels, GT_U32);
    F_NUMBER(val, -1, ttl, GT_U32);
    F_ENUM(val, -1, ttlMode, CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT);
    F_NUMBER(val, -1, label1, GT_U32);
    F_ENUM(val, -1, exp1MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, exp1, GT_U32);
    F_NUMBER(val, -1, label2, GT_U32);
    F_ENUM(val, -1, exp2MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, exp2, GT_U32);
    F_NUMBER(val, -1, label3, GT_U32);
    F_ENUM(val, -1, exp3MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, exp3, GT_U32);
    F_BOOL(val, -1, retainCRC);
    F_BOOL(val, -1, setSBit);
    F_NUMBER(val, -1, cfi, GT_U32);
    F_BOOL(val, -1, controlWordEnable);
    F_NUMBER(val, -1, controlWordIndex, GT_U32);
    F_ENUM(val, -1, mplsEthertypeSelect, CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT);
    F_BOOL(val, -1, pushEliAndElAfterLabel1);
    F_BOOL(val, -1, pushEliAndElAfterLabel2);
    F_BOOL(val, -1, pushEliAndElAfterLabel3);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tagEnable);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_ENUM(val, t, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, up, GT_U32);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_NUMBER(val, t, numLabels, GT_U32);
    FO_NUMBER(val, t, ttl, GT_U32);
    FO_ENUM(val, t, ttlMode, CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT);
    FO_NUMBER(val, t, label1, GT_U32);
    FO_ENUM(val, t, exp1MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, exp1, GT_U32);
    FO_NUMBER(val, t, label2, GT_U32);
    FO_ENUM(val, t, exp2MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, exp2, GT_U32);
    FO_NUMBER(val, t, label3, GT_U32);
    FO_ENUM(val, t, exp3MarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, exp3, GT_U32);
    FO_BOOL(val, t, retainCRC);
    FO_BOOL(val, t, setSBit);
    FO_NUMBER(val, t, cfi, GT_U32);
    FO_BOOL(val, t, controlWordEnable);
    FO_NUMBER(val, t, controlWordIndex, GT_U32);
    FO_ENUM(val, t, mplsEthertypeSelect, CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT);
    FO_BOOL(val, t, pushEliAndElAfterLabel1);
    FO_BOOL(val, t, pushEliAndElAfterLabel2);
    FO_BOOL(val, t, pushEliAndElAfterLabel3);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC);

enumDescr enumDescr_CPSS_DXCH_PA_UNIT_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_UNDEFINED_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_RXDMA_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_TXDMA_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_TXQ_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_TX_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_ETH_TX_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_RXDMA_1_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_TXDMA_1_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_TX_FIFO_1_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_ETH_TX_FIFO_1_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_RXDMA_GLUE_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_TXDMA_GLUE_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_MPPM_E),
    ENUM_ENTRY(CPSS_DXCH_PA_UNIT_MAX_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PA_UNIT_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC *val
)
{
    F_STRUCT(val, -1, ingressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
    F_NUMBER(val, -1, isL2Valid, GT_U8);
    F_NUMBER(val, -1, isNd, GT_U8);
    F_NUMBER(val, -1, isBc, GT_U8);
    F_NUMBER(val, -1, isIp, GT_U8);
    F_NUMBER(val, -1, l2Encap, GT_U32);
    F_NUMBER(val, -1, etherType, GT_U16);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_STRUCT_CUSTOM(val, -1, macSa, GT_ETHERADDR);
    F_NUMBER(val, -1, tag1Exist, GT_U32);
    F_NUMBER(val, -1, vid1, GT_U32);
    F_NUMBER(val, -1, up1, GT_U32);
    F_NUMBER(val, -1, cfi1, GT_U32);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPV6ADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPV6ADDR);
    F_NUMBER(val, -1, isIpv6ExtHdrExist, GT_U8);
    F_NUMBER(val, -1, isIpv6HopByHop, GT_U8);
    F_NUMBER(val, -1, ipHeaderOk, GT_U8);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_ARRAY_START(val, -1, UdbExtNotIpv6);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, UdbExtNotIpv6, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbExtNotIpv6);
    F_ARRAY_START(val, -1, UdbExtIpv6L2);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            F_ARRAY_NUMBER(val, UdbExtIpv6L2, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbExtIpv6L2);
    F_ARRAY_START(val, -1, UdbExtIpv6L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            F_ARRAY_NUMBER(val, UdbExtIpv6L4, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, UdbExtIpv6L4);
    F_ARRAY_START(val, -1, udb0_11);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            F_ARRAY_NUMBER(val, udb0_11, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb0_11);
    F_NUMBER(val, -1, udb12, GT_U8);
    F_ARRAY_START(val, -1, udb39_40);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, udb39_40, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb39_40);
    F_NUMBER(val, -1, srcPortOrTrunk, GT_U32);
    F_NUMBER(val, -1, srcIsTrunk, GT_U8);
    F_NUMBER(val, -1, trunkHash, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, ingressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
    FO_NUMBER(val, t, isL2Valid, GT_U8);
    FO_NUMBER(val, t, isNd, GT_U8);
    FO_NUMBER(val, t, isBc, GT_U8);
    FO_NUMBER(val, t, isIp, GT_U8);
    FO_NUMBER(val, t, l2Encap, GT_U32);
    FO_NUMBER(val, t, etherType, GT_U16);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_STRUCT(val, t, macSa, GT_ETHERADDR);
    FO_NUMBER(val, t, tag1Exist, GT_U32);
    FO_NUMBER(val, t, vid1, GT_U32);
    FO_NUMBER(val, t, up1, GT_U32);
    FO_NUMBER(val, t, cfi1, GT_U32);
    FO_STRUCT(val, t, sip, GT_IPV6ADDR);
    FO_STRUCT(val, t, dip, GT_IPV6ADDR);
    FO_NUMBER(val, t, isIpv6ExtHdrExist, GT_U8);
    FO_NUMBER(val, t, isIpv6HopByHop, GT_U8);
    FO_NUMBER(val, t, ipHeaderOk, GT_U8);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_ARRAY_START(val, t, UdbExtNotIpv6);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, UdbExtNotIpv6, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbExtNotIpv6);
    FO_ARRAY_START(val, t, UdbExtIpv6L2);
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            FO_ARRAY_NUMBER(val, UdbExtIpv6L2, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbExtIpv6L2);
    FO_ARRAY_START(val, t, UdbExtIpv6L4);
    {
        int idx;
        for (idx = 0; idx < 3; idx++) {
            FO_ARRAY_NUMBER(val, UdbExtIpv6L4, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, UdbExtIpv6L4);
    FO_ARRAY_START(val, t, udb0_11);
    {
        int idx;
        for (idx = 0; idx < 12; idx++) {
            FO_ARRAY_NUMBER(val, udb0_11, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb0_11);
    FO_NUMBER(val, t, udb12, GT_U8);
    FO_ARRAY_START(val, t, udb39_40);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, udb39_40, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb39_40);
    FO_NUMBER(val, t, srcPortOrTrunk, GT_U32);
    FO_NUMBER(val, t, srcIsTrunk, GT_U8);
    FO_NUMBER(val, t, trunkHash, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC);

void prv_lua_to_c_CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC *val
)
{
    F_NUMBER(val, -1, totalConfiguredSlices, GT_U32);
    F_NUMBER(val, -1, totalSlicesOnUnit, GT_U32);
    F_BOOL(val, -1, workConservingBit);
    F_BOOL(val, -1, pizzaMapLoadEnBit);
    F_ARRAY_START(val, -1, slice_enable);
    {
        int idx;
        for (idx = 0; idx < 1500; idx++) {
            F_ARRAY_BOOL(val, slice_enable, idx, GT_BOOL);
        }
    }
    F_ARRAY_END(val, -1, slice_enable);
    F_ARRAY_START(val, -1, slice_occupied_by);
    {
        int idx;
        for (idx = 0; idx < 1500; idx++) {
            F_ARRAY_NUMBER(val, slice_occupied_by, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, slice_occupied_by);
}

void prv_c_to_lua_CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, totalConfiguredSlices, GT_U32);
    FO_NUMBER(val, t, totalSlicesOnUnit, GT_U32);
    FO_BOOL(val, t, workConservingBit);
    FO_BOOL(val, t, pizzaMapLoadEnBit);
    FO_ARRAY_START(val, t, slice_enable);
    {
        int idx;
        for (idx = 0; idx < 1500; idx++) {
            FO_ARRAY_BOOL(val, slice_enable, idx, GT_BOOL);
        }
    }
    FO_ARRAY_END(val, t, slice_enable);
    FO_ARRAY_START(val, t, slice_occupied_by);
    {
        int idx;
        for (idx = 0; idx < 1500; idx++) {
            FO_ARRAY_NUMBER(val, slice_occupied_by, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, slice_occupied_by);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);

void prv_lua_to_c_CPSS_DXCH_PORT_COMBO_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_COMBO_PARAMS_STC *val
)
{
    F_ARRAY_START(val, -1, macArray);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_STRUCT(val, macArray, idx, CPSS_DXCH_PORT_MAC_PARAMS_STC);
        }
    }
    F_ARRAY_END(val, -1, macArray);
    F_NUMBER(val, -1, preferredMacIdx, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PORT_COMBO_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_PORT_COMBO_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, macArray);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_STRUCT(val, macArray, idx, CPSS_DXCH_PORT_MAC_PARAMS_STC);
        }
    }
    FO_ARRAY_END(val, t, macArray);
    FO_NUMBER(val, t, preferredMacIdx, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PORT_COMBO_PARAMS_STC);

enumDescr enumDescr_CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_IPFIX_SAMPLING_ACTION_ALARM_E),
    ENUM_ENTRY(CPSS_DXCH_IPFIX_SAMPLING_ACTION_MIRROR_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_UNITS_ID_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_GEMAC_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_XLGMAC_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_HGLMAC_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_XPCS_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_MMPCS_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_CG_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_INTLKN_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_INTLKN_RF_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_SERDES_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_SERDES_PHY_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_ETI_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_ETI_ILKN_RF_UNIT_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_UNITS_ID_D_UNIT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_UNITS_ID_ENT);

enumDescr enumDescr_CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_DECREMENT_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_INCREMENT_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_DECREMENT_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_FREQUENCE_UPDATE_E),
    ENUM_ENTRY(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_NOP_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_SHARED_POLICY_UNCONSTRAINED_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_TX_SHARED_POLICY_CONSTRAINED_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT);

void prv_lua_to_c_CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC *val
)
{
    F_STRUCT(val, -1, rxDMA, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, txDMA, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, TxQ, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, txFIFO, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, ethFxFIFO, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, ilknTxFIFO, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
}

void prv_c_to_lua_CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, rxDMA, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, txDMA, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, TxQ, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, txFIFO, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, ethFxFIFO, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, ilknTxFIFO, CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC);

enumDescr enumDescr_CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_IP_CTRL_NONE_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_IP_CTRL_IPV4_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_IP_CTRL_IPV6_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);

enumDescr enumDescr_CPSS_DXCH_DIAG_BIST_STATUS_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_DIAG_BIST_STATUS_NOT_READY_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_BIST_STATUS_PASS_E),
    ENUM_ENTRY(CPSS_DXCH_DIAG_BIST_STATUS_FAIL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_DIAG_BIST_STATUS_ENT);

enumDescr enumDescr_CPSS_DXCH_CNC_TM_INDEX_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CNC_TM_INDEX_MODE_0_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_TM_INDEX_MODE_1_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_TM_INDEX_MODE_2_E),
    ENUM_ENTRY(CPSS_DXCH_CNC_TM_INDEX_MODE_3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CNC_TM_INDEX_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_EGRESS_TAG_TPID_SELECT_MODE_E),
    ENUM_ENTRY(CPSS_DXCH_BRG_VLAN_VID_TO_TPID_SELECT_TABLE_MODE_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT);

enumDescr enumDescr_CPSS_DIAG_PG_TRANSMIT_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DIAG_PG_TRANSMIT_CONTINUES_E),
    ENUM_ENTRY(CPSS_DIAG_PG_TRANSMIT_SINGLE_BURST_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DIAG_PG_TRANSMIT_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC *val
)
{
    F_STRUCT(val, -1, rxDMA, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, rxDMA_CTU, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, txDMA, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, txDMA_CTU, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, BM, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    F_STRUCT(val, -1, TxQ, CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC);
}

void prv_c_to_lua_CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC(
    lua_State *L,
    CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, rxDMA, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, rxDMA_CTU, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, txDMA, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, txDMA_CTU, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, BM, CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC);
    FO_STRUCT(val, t, TxQ, CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC);

void prv_lua_to_c_CPSS_DXCH_DIAG_BIST_RESULT_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_BIST_RESULT_STC *val
)
{
    F_ENUM(val, -1, memType, CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT);
    F_STRUCT(val, -1, location, CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC);
}

void prv_c_to_lua_CPSS_DXCH_DIAG_BIST_RESULT_STC(
    lua_State *L,
    CPSS_DXCH_DIAG_BIST_RESULT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, memType, CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT);
    FO_STRUCT(val, t, location, CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_DIAG_BIST_RESULT_STC);

void prv_lua_to_c_CPSS_DXCH_TM_GLUE_DRAM_CFG_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DRAM_CFG_STC *val
)
{
    F_NUMBER(val, -1, activeInterfaceNum, GT_U32);
    F_NUMBER(val, -1, activeInterfaceMask, GT_U32);
    F_STRUCT(val, -1, interfaceParams, CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC);
}

void prv_c_to_lua_CPSS_DXCH_TM_GLUE_DRAM_CFG_STC(
    lua_State *L,
    CPSS_DXCH_TM_GLUE_DRAM_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, activeInterfaceNum, GT_U32);
    FO_NUMBER(val, t, activeInterfaceMask, GT_U32);
    FO_STRUCT(val, t, interfaceParams, CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TM_GLUE_DRAM_CFG_STC);

enumDescr enumDescr_CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_DISABLE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_ALL_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_SPECIFIC_GRE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_GRE_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_SPECIFIC_UDP_PORT_E),
    ENUM_ENTRY(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_UDP_PORT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT(
    lua_State *L,
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, pclIpUcAction, CPSS_DXCH_PCL_ACTION_STC);
    FO_STRUCT(val, t, ipLttEntry, CPSS_DXCH_IP_LTT_ENTRY_STC);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC *val
)
{
    F_NUMBER(val, -1, version, GT_U32);
    F_BOOL(val, -1, mBit);
    F_NUMBER(val, -1, opLength, GT_U32);
    F_NUMBER(val, -1, hopCount, GT_U32);
    F_NUMBER(val, -1, eRbid, GT_U32);
    F_NUMBER(val, -1, iRbid, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, version, GT_U32);
    FO_BOOL(val, t, mBit);
    FO_NUMBER(val, t, opLength, GT_U32);
    FO_NUMBER(val, t, hopCount, GT_U32);
    FO_NUMBER(val, t, eRbid, GT_U32);
    FO_NUMBER(val, t, iRbid, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_START_TRILL_HEADER_STC);

void prv_lua_to_c_CPSS_DXCH_TTI_RULE_UNT(
    lua_State *L,
    CPSS_DXCH_TTI_RULE_UNT *val
)
{
    /* just do nothing */
}

void prv_c_to_lua_CPSS_DXCH_TTI_RULE_UNT(
    lua_State *L,
    CPSS_DXCH_TTI_RULE_UNT *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, ipv4, CPSS_DXCH_TTI_IPV4_RULE_STC);
    FO_STRUCT(val, t, mpls, CPSS_DXCH_TTI_MPLS_RULE_STC);
    FO_STRUCT(val, t, eth, CPSS_DXCH_TTI_ETH_RULE_STC);
    FO_STRUCT(val, t, mim, CPSS_DXCH_TTI_MIM_RULE_STC);
    FO_STRUCT(val, t, trill, CPSS_DXCH_TTI_TRILL_RULE_STC);
    FO_ARRAY_START(val, t, udbArray);
    {
        int idx;
        for (idx = 0; idx < 30; idx++) {
            FO_ARRAY_NUMBER(val, udbArray, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udbArray);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TTI_RULE_UNT);

void prv_lua_to_c_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, portNum, GT_U8);
    F_NUMBER(val, -1, messageType, GT_U32);
    F_NUMBER(val, -1, sequenceId, GT_U32);
    F_NUMBER(val, -1, timeStampVal, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, portNum, GT_U8);
    FO_NUMBER(val, t, messageType, GT_U32);
    FO_NUMBER(val, t, sequenceId, GT_U32);
    FO_NUMBER(val, t, timeStampVal, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_TERM_ACTION_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_TERM_ACTION_STC *val
)
{
    F_ENUM(val, -1, command, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    F_ENUM(val, -1, passengerPacketType, CPSS_TUNNEL_PASSENGER_PACKET_ENT);
    F_STRUCT(val, -1, egressInterface, CPSS_INTERFACE_INFO_STC);
    F_BOOL(val, -1, isTunnelStart);
    F_NUMBER(val, -1, tunnelStartIdx, GT_U32);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_ENUM(val, -1, vlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_BOOL(val, -1, nestedVlanEnable);
    F_BOOL(val, -1, copyTtlFromTunnelHeader);
    F_ENUM(val, -1, qosMode, CPSS_DXCH_TUNNEL_QOS_MODE_ENT);
    F_ENUM(val, -1, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_NUMBER(val, -1, qosProfile, GT_U32);
    F_NUMBER(val, -1, defaultUP, GT_U32);
    F_ENUM(val, -1, modifyUP, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    F_BOOL(val, -1, remapDSCP);
    F_ENUM(val, -1, modifyDSCP, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    F_BOOL(val, -1, mirrorToIngressAnalyzerEnable);
    F_BOOL(val, -1, policerEnable);
    F_NUMBER(val, -1, policerIndex, GT_U32);
    F_BOOL(val, -1, matchCounterEnable);
    F_NUMBER(val, -1, matchCounterIndex, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_TERM_ACTION_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_TERM_ACTION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, command, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, userDefinedCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_ENUM(val, t, passengerPacketType, CPSS_TUNNEL_PASSENGER_PACKET_ENT);
    FO_STRUCT(val, t, egressInterface, CPSS_INTERFACE_INFO_STC);
    FO_BOOL(val, t, isTunnelStart);
    FO_NUMBER(val, t, tunnelStartIdx, GT_U32);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_ENUM(val, t, vlanPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_BOOL(val, t, nestedVlanEnable);
    FO_BOOL(val, t, copyTtlFromTunnelHeader);
    FO_ENUM(val, t, qosMode, CPSS_DXCH_TUNNEL_QOS_MODE_ENT);
    FO_ENUM(val, t, qosPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_NUMBER(val, t, qosProfile, GT_U32);
    FO_NUMBER(val, t, defaultUP, GT_U32);
    FO_ENUM(val, t, modifyUP, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    FO_BOOL(val, t, remapDSCP);
    FO_ENUM(val, t, modifyDSCP, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    FO_BOOL(val, t, mirrorToIngressAnalyzerEnable);
    FO_BOOL(val, t, policerEnable);
    FO_NUMBER(val, t, policerIndex, GT_U32);
    FO_BOOL(val, t, matchCounterEnable);
    FO_NUMBER(val, t, matchCounterIndex, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_TERM_ACTION_STC);

enumDescr enumDescr_CPSS_DXCH_POLICER_REMARK_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_REMARK_MODE_L2_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_REMARK_MODE_L3_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_REMARK_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC *val
)
{
    F_BOOL(val, -1, tagEnable);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_ENUM(val, -1, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, up, GT_U32);
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_BOOL(val, -1, retainCrc);
    F_NUMBER(val, -1, iSid, GT_U32);
    F_NUMBER(val, -1, iUp, GT_U32);
    F_ENUM(val, -1, iUpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, iDp, GT_U32);
    F_ENUM(val, -1, iDpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    F_NUMBER(val, -1, iTagReserved, GT_U32);
    F_ENUM(val, -1, iSidAssignMode, CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT);
    F_ENUM(val, -1, bDaAssignMode, CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT);
}

void prv_c_to_lua_CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, tagEnable);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_ENUM(val, t, upMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, up, GT_U32);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_BOOL(val, t, retainCrc);
    FO_NUMBER(val, t, iSid, GT_U32);
    FO_NUMBER(val, t, iUp, GT_U32);
    FO_ENUM(val, t, iUpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, iDp, GT_U32);
    FO_ENUM(val, t, iDpMarkMode, CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT);
    FO_NUMBER(val, t, iTagReserved, GT_U32);
    FO_ENUM(val, t, iSidAssignMode, CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT);
    FO_ENUM(val, t, bDaAssignMode, CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC);

void prv_lua_to_c_CPSS_DXCH_PP_CONFIG_INIT_STC(
    lua_State *L,
    CPSS_DXCH_PP_CONFIG_INIT_STC *val
)
{
    F_ENUM(val, -1, routingMode, CPSS_DXCH_CFG_ROUTING_MODE_ENT);
    F_NUMBER(val, -1, maxNumOfPbrEntries, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_PP_CONFIG_INIT_STC(
    lua_State *L,
    CPSS_DXCH_PP_CONFIG_INIT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, routingMode, CPSS_DXCH_CFG_ROUTING_MODE_ENT);
    FO_NUMBER(val, t, maxNumOfPbrEntries, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PP_CONFIG_INIT_STC);

void prv_lua_to_c_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC(
    lua_State *L,
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC *val
)
{
    F_ENUM(val, -1, qosAssignCmd, CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
    F_NUMBER(val, -1, qosProfileId, GT_U32);
    F_ENUM(val, -1, qosAssignPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    F_BOOL(val, -1, enableModifyUp);
    F_BOOL(val, -1, enableModifyDscp);
}

void prv_c_to_lua_CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC(
    lua_State *L,
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, qosAssignCmd, CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
    FO_NUMBER(val, t, qosProfileId, GT_U32);
    FO_ENUM(val, t, qosAssignPrecedence, CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
    FO_BOOL(val, t, enableModifyUp);
    FO_BOOL(val, t, enableModifyDscp);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC);

void prv_lua_to_c_CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC(
    lua_State *L,
    CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *val
)
{
    F_NUMBER(val, -1, interval, GT_U32);
    F_NUMBER(val, -1, randBitmap, GT_U32);
}

void prv_c_to_lua_CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC(
    lua_State *L,
    CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, interval, GT_U32);
    FO_NUMBER(val, t, randBitmap, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC);

enumDescr enumDescr_CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E),
    ENUM_ENTRY(CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT);

enumDescr enumDescr_CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E),
    ENUM_ENTRY(CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1522_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT);

enumDescr enumDescr_CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_PARTIAL_E),
    ENUM_ENTRY(CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_FULL_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT);

void prv_lua_to_c_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC *val
)
{
    F_BOOL(val, -1, ptpExceptionCommandEnable);
    F_ENUM(val, -1, ptpExceptionCommand, CPSS_PACKET_CMD_ENT);
    F_ENUM(val, -1, ptpExceptionCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    F_BOOL(val, -1, ptpVersionCheckEnable);
}

void prv_c_to_lua_CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC(
    lua_State *L,
    CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, ptpExceptionCommandEnable);
    FO_ENUM(val, t, ptpExceptionCommand, CPSS_PACKET_CMD_ENT);
    FO_ENUM(val, t, ptpExceptionCpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_BOOL(val, t, ptpVersionCheckEnable);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC);

enumDescr enumDescr_CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E),
    ENUM_ENTRY(CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E),
    ENUM_ENTRY(CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT);

enumDescr enumDescr_CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_LSB_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_MSB_E),
    ENUM_ENTRY(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_12_BITS_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT);

void prv_lua_to_c_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC *val
)
{
    F_STRUCT(val, -1, ingressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
    F_NUMBER(val, -1, pktType, GT_U32);
    F_NUMBER(val, -1, ipFragmented, GT_U8);
    F_NUMBER(val, -1, ipHeaderInfo, GT_U32);
    F_NUMBER(val, -1, ipPacketLength, GT_U32);
    F_NUMBER(val, -1, ttl, GT_U32);
    F_STRUCT_CUSTOM(val, -1, sip, GT_IPADDR);
    F_STRUCT_CUSTOM(val, -1, dip, GT_IPADDR);
    F_NUMBER(val, -1, vrfId, GT_U32);
    F_NUMBER(val, -1, tcpUdpPortComparators, GT_U32);
    F_NUMBER(val, -1, isUdbValid, GT_U8);
    F_NUMBER(val, -1, udb5, GT_U8);
    F_ARRAY_START(val, -1, udb41_44);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            F_ARRAY_NUMBER(val, udb41_44, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, udb41_44);
}

void prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC(
    lua_State *L,
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, ingressIpCommon, CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC);
    FO_NUMBER(val, t, pktType, GT_U32);
    FO_NUMBER(val, t, ipFragmented, GT_U8);
    FO_NUMBER(val, t, ipHeaderInfo, GT_U32);
    FO_NUMBER(val, t, ipPacketLength, GT_U32);
    FO_NUMBER(val, t, ttl, GT_U32);
    FO_STRUCT(val, t, sip, GT_IPADDR);
    FO_STRUCT(val, t, dip, GT_IPADDR);
    FO_NUMBER(val, t, vrfId, GT_U32);
    FO_NUMBER(val, t, tcpUdpPortComparators, GT_U32);
    FO_NUMBER(val, t, isUdbValid, GT_U8);
    FO_NUMBER(val, t, udb5, GT_U8);
    FO_ARRAY_START(val, t, udb41_44);
    {
        int idx;
        for (idx = 0; idx < 4; idx++) {
            FO_ARRAY_NUMBER(val, udb41_44, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, udb41_44);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC);

void prv_lua_to_c_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC *val
)
{
    F_BOOL(val, -1, entryValid);
    F_BOOL(val, -1, isPtpExeption);
    F_ENUM(val, -1, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    F_NUMBER(val, -1, messageType, GT_U32);
    F_NUMBER(val, -1, domainNum, GT_U32);
    F_NUMBER(val, -1, sequenceId, GT_U32);
    F_NUMBER(val, -1, queueEntryId, GT_U32);
    F_NUMBER(val, -1, portNum, GT_PHYSICAL_PORT_NUM);
}

void prv_c_to_lua_CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, entryValid);
    FO_BOOL(val, t, isPtpExeption);
    FO_ENUM(val, t, packetFormat, CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT);
    FO_NUMBER(val, t, messageType, GT_U32);
    FO_NUMBER(val, t, domainNum, GT_U32);
    FO_NUMBER(val, t, sequenceId, GT_U32);
    FO_NUMBER(val, t, queueEntryId, GT_U32);
    FO_NUMBER(val, t, portNum, GT_PHYSICAL_PORT_NUM);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_IP_NAT66_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IP_NAT66_ENTRY_STC *val
)
{
    F_STRUCT_CUSTOM(val, -1, macDa, GT_ETHERADDR);
    F_ENUM(val, -1, modifyCommand, CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT);
    F_STRUCT_CUSTOM(val, -1, address, GT_IPV6ADDR);
    F_NUMBER(val, -1, prefixSize, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_IP_NAT66_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH_IP_NAT66_ENTRY_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, macDa, GT_ETHERADDR);
    FO_ENUM(val, t, modifyCommand, CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT);
    FO_STRUCT(val, t, address, GT_IPV6ADDR);
    FO_NUMBER(val, t, prefixSize, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_IP_NAT66_ENTRY_STC);

void prv_lua_to_c_CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC *val
)
{
    F_NUMBER(val, -1, skip, GT_U32);
    F_NUMBER(val, -1, ttl, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC(
    lua_State *L,
    CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, skip, GT_U32);
    FO_NUMBER(val, t, ttl, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC);

void prv_lua_to_c_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC(
    lua_State *L,
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC *val
)
{
    F_NUMBER(val, -1, row, GT_U32);
    F_NUMBER(val, -1, column, GT_U32);
}

void prv_c_to_lua_CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC(
    lua_State *L,
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, row, GT_U32);
    FO_NUMBER(val, t, column, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC);

