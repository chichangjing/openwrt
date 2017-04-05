#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "device_info.h"

struct device_gt device_gt[1];

const char * cli_view_name_tbl[_CLI_VIEW_MAX] = {
        [SYSTEM_VIEW]       = "system",
        [INTERFACE_VIEW]    = "interface ",
        [PORT_TRUNK_VIEW]   = "aggregation  ",
        [VLAN_VIEW]         = "vlan ",
        [MULTICAST_VIEW]    = "multicast ",
        [RING_VIEW]         = "ring ",
        [RSTP_VIEW]         = "instance ",
};

#define _view_name(__view) (char *)(&_terminal_view->cmd_head[strlen(cli_view_name_tbl[__view])])

const char * port_type_tbl[2][_PORT_CONFIG_TYPE_MAX] = {
		{
				[_PORT_TYPE_FE] = "fe",
				[_PORT_TYPE_FX] = "fx",
				[_PORT_TYPE_GE] = "ge",
				[_PORT_TYPE_GX] = "gx",
		},
		{
				[_PORT_TYPE_FE] = "FE",
				[_PORT_TYPE_FX] = "FX",
				[_PORT_TYPE_GE] = "GE",
				[_PORT_TYPE_GX] = "GX",
		},
};

const char * port_config_admin_mode_tbl[2][_PORT_CONFIG_ADMIN_MODE_MAX] = {
		{
				[PORT_CONFIG_ADMIN_MODE_ENABLED] 	= "enable",
				[PORT_CONFIG_ADMIN_MODE_DISABLED] 	= "disable",
		},
		{
				[PORT_CONFIG_ADMIN_MODE_ENABLED] 	= "Enable",
				[PORT_CONFIG_ADMIN_MODE_DISABLED] 	= "Disable",

		},
};
const char * port_config_operational_status_tbl[2][_PORT_CONFIG_OPERATIONAL_STATUS_MAX] = {
		{
				[_PORT_COFIG_OPERATIONAL_UP] 		= "up",
				[_PORT_COFIG_OPERATIONAL_DOWN] 		= "down",
				[_PORT_COFIG_OPERATIONAL_TESTING] 	= "test",
		},
		{
				[_PORT_COFIG_OPERATIONAL_UP] 		= "Up",
				[_PORT_COFIG_OPERATIONAL_DOWN] 		= "Down",
				[_PORT_COFIG_OPERATIONAL_TESTING] 	= "Test",
		},
};
const char * port_config_physical_mode_tbl[2][_PORT_CONFIG_PHYSICAL_MODE_MAX] = {
		{
				[_SPEED_MODE_AUTO_NEGOTIATE] 	= "auto-Nego",
				[_SPEED_MODE_SPEED_HALF_10] 	= "half-10m",
				[_SPEED_MODE_SPEED_FULL_10] 	= "full-10m",
				[_SPEED_MODE_SPEED_HALF_100] 	= "half-100m",
				[_SPEED_MODE_SPEED_FULL_100] 	= "full-100m",
				[_SPEED_MODE_SPEED_FULL_1000] 	= "full-1000m",
		},
		{
				[_SPEED_MODE_AUTO_NEGOTIATE] 	= "Auto-Nego",
				[_SPEED_MODE_SPEED_HALF_10] 	= "Half-10M",
				[_SPEED_MODE_SPEED_FULL_10] 	= "Full-10M",
				[_SPEED_MODE_SPEED_HALF_100] 	= "Half-100M",
				[_SPEED_MODE_SPEED_FULL_100] 	= "Full-100M",
				[_SPEED_MODE_SPEED_FULL_1000] 	= "Full-1000M",
		},
};
const char * port_config_physical_status_tbl[2][_PORT_CONFIG_PHYSICAL_STATUS_MAX] = {
		{
				[ACTUAL_HALF_10] 	= "half-10M",
				[ACTUAL_FULL_10] 	= "full-10M",
				[ACTUAL_HALF_100] 	= "half-100M",
				[ACTUAL_FULL_100] 	= "full-100M",
				[ACTUAL_FULL_1000] 	= "full-1000M",
		},
		{
				[ACTUAL_HALF_10] 	= "Half-10M",
				[ACTUAL_FULL_10] 	= "Full-10M",
				[ACTUAL_HALF_100] 	= "Half-100M",
				[ACTUAL_FULL_100] 	= "Full-100M",
				[ACTUAL_FULL_1000] 	= "Full-1000M",
		},
};
const char * port_config_flow_control_mode_tbl[2][_RATE_LIMITING_MODE_MAX] = {
		{
		        [_RATE_LIMITING_MODE_ENABLE] 	= "enable",
			    [_RATE_LIMITING_MODE_DISABLE] 	= "disable",
		},
		{
	            [_RATE_LIMITING_MODE_ENABLE]    = "Enable",
	            [_RATE_LIMITING_MODE_DISABLE]   = "Disable",
		},
};
const char * port_config_dot1d_state_tbl[2][_PORT_CONFIG_DOT1D_STATE_MAX] = {
		{
			[DOT1D_STATE_DISABLED] 		= "disabled",
			[DOT1D_STATE_BLOCKED] 		= "blocked",
			[DOT1D_STATE_LISTENING] 	= "listening",
			[DOT1D_STATE_LEARNING] 		= "learning",
			[DOT1D_STATE_FORWARDING] 	= "forwding",
		},
		{
			[DOT1D_STATE_DISABLED] 		= "Disabled",
			[DOT1D_STATE_BLOCKED] 		= "Blocked",
			[DOT1D_STATE_LISTENING] 	= "Listening",
			[DOT1D_STATE_LEARNING] 		= "Learning",
			[DOT1D_STATE_FORWARDING] 	= "Forwding",
		},
};

const char * port_name[3][_PORT_MAX] = {
		{
			[PORT_1] 	= "idx-1",
	    	[PORT_2] 	= "idx-2",
	    	[PORT_3] 	= "idx-3",
	    	[PORT_4] 	= "idx-4",
	    	[PORT_5] 	= "idx-5",
	    	[PORT_6] 	= "idx-6",
	    	[PORT_7] 	= "idx-7",
	    	[PORT_8] 	= "idx-8",
	    	[PORT_9] 	= "idx-9",
	    	[PORT_10] 	= "idx-10",
		},
		{
		    [PORT_1] 	= "Port-1",
		    [PORT_2] 	= "Port-2",
		    [PORT_3] 	= "Port-3",
		    [PORT_4] 	= "Port-4",
		    [PORT_5] 	= "Port-5",
		    [PORT_6] 	= "Port-6",
		    [PORT_7] 	= "Port-7",
		    [PORT_8] 	= "Port-8",
		    [PORT_9] 	= "Port-9",
		    [PORT_10] 	= "Port-10",
		},
		{
		    [PORT_1] 	= "port-1",
		    [PORT_2] 	= "port-2",
		    [PORT_3] 	= "port-3",
		    [PORT_4] 	= "port-4",
		    [PORT_5] 	= "port-5",
		    [PORT_6] 	= "port-6",
		    [PORT_7] 	= "port-7",
		    [PORT_8] 	= "port-8",
		    [PORT_9] 	= "port-9",
		    [PORT_10] 	= "port-10",
		},
};

const char * port_config_name[2][_PORT_CFG_MAX] = {
		{
			[PORT_CFG_TYPE] 				= "type",
			[PORT_CFG_ADMIN_MODE] 			= "admin_mode",
			[PORT_CFG_OPERATIONAL_STATUS] 	= "operational_status",
			[PORT_CFG_PHYSICAL_MODE] 		= "physical_mode",
			[PORT_CFG_PHYSICAL_STATUS] 		= "physical_status",
			//[PORT_CFG_FLOW_CONTROL_MODE] 	= "flow_control_mode",
			//[PORT_CFG_DOT1D_STATE] 			= "dot1d_state",
		},
		{
			[PORT_CFG_TYPE] 				= "Port Type",
			[PORT_CFG_ADMIN_MODE] 			= "Port Mode",
			[PORT_CFG_OPERATIONAL_STATUS] 	= "Opera Mode",
			[PORT_CFG_PHYSICAL_MODE] 		= "Port Speed",
			[PORT_CFG_PHYSICAL_STATUS] 		= "Port Status",
			//[PORT_CFG_FLOW_CONTROL_MODE] 	= "Flow Contrl",
			//[PORT_CFG_DOT1D_STATE] 			= "Dot1d State",
		},
};

const char * eth_stats_type[2][_ETH_STATS_TYPE_MAX] = {
		{
			[TX_BYTES] 		= "tx_bytes",
			[RX_BYTES] 		= "rx_bytes",
			[TX_UCAST_PKTS] = "tx_unicast_packets",
			[RX_UCAST_PKTS] = "rx_unicast_packets",
			[TX_BCAST_PKTS] = "tx_broadcast_packets",
			[RX_BCAST_PKTS] = "rx_broadcast_packets",
			[TX_MCAST_PKTS] = "tx_multicast_packets",
			[RX_MCAST_PKTS] = "rx_multicast_packets",
			[TX_PAUSE_PKTS] = "tx_pause_packets",
			[RX_PAUSE_PKTS] = "rx_pause_packets",
		},
		{
			[TX_BYTES] 		= "Tx Bytes",
			[RX_BYTES] 		= "Rx Bytes",
			[TX_UCAST_PKTS] = "Tx Unicast",
			[RX_UCAST_PKTS] = "Rx Unicast",
			[TX_BCAST_PKTS] = "Tx Brdcast",
			[RX_BCAST_PKTS] = "Rx Brdcast",
			[TX_MCAST_PKTS] = "Tx Mulcast",
			[RX_MCAST_PKTS] = "Rx Mulcast",
			[TX_PAUSE_PKTS] = "Tx Pause",
			[RX_PAUSE_PKTS] = "Rx Pause",
		},
};

const char * port_mirror_mode_tbl[2][_PORT_MIRROR_MODE_MAX] = {
	{
		[_PORT_MIRROR_MODE_TX] 			= "outbound",
		[_PORT_MIRROR_MODE_RX] 			= "inbound",
		[_PORT_MIRROR_MODE_BOTH] 		= "both",
		[_PORT_MIRROR_MODE_DISABLED] 	= "disabled",
	},
	{
		[_PORT_MIRROR_MODE_TX] 			= "OutBound",
		[_PORT_MIRROR_MODE_RX] 			= "InBound",
		[_PORT_MIRROR_MODE_BOTH] 		= "Both",
		[_PORT_MIRROR_MODE_DISABLED] 	= "Disabled",
	},
};
const char * port_aggr_mode_tbl[2][_PORT_AGGR_MODE_MAX] = {
	{
		[_PORT_AGGR_MODE_DISABLED] 			= "disabled",
		[_PORT_AGGR_MODE_STATIC] 			= "static",
	},
	{
		[_PORT_AGGR_MODE_DISABLED] 			= "Disabled",
		[_PORT_AGGR_MODE_STATIC] 			= "Static",
	},
};
const char * port_aggr_load_balance_tbl[2][_PORT_AGGR_LOAD_BALANCE_MAX] = {
	{
		[_PORT_AGGR_LOAD_BALANCE_XOR] 			= "xor",
		[_PORT_AGGR_LOAD_BALANCE_HASH] 			= "hash",
	},
	{
		[_PORT_AGGR_LOAD_BALANCE_XOR] 			= "Xor",
		[_PORT_AGGR_LOAD_BALANCE_HASH] 			= "Hash",
	},
};
const char * mcast_enable_status_tbl[2][_MCAST_ENABLE_STATUS_MAX] = {
		{
				[MCAST_ENABLE_STATUS_ENABLE] 	= "enable",
				[MCAST_ENABLE_STATUS_DISABLE] 	= "disable",
		},
		{
				[MCAST_ENABLE_STATUS_ENABLE] 	= "Enable",
				[MCAST_ENABLE_STATUS_DISABLE] 	= "Disable",

		},
};

const  char * vlan_mode_tbl[2][_VLAN_MODE_MAX] = {
		{
				[VLAN_MODE_PORTBASED] 	= "portbased",
				[VLAN_MODE_8021D] 		= "802.1Q",
		},
		{
				[VLAN_MODE_PORTBASED] 	= "PortBased",
				[VLAN_MODE_8021D] 		= "802.1Q",
		},
};
const char * q8021_vlan_port_mode_tbl[2][_Q8021_VLAN_PORT_MAX] = {
		{
				[Q8021_VLAN_PORT_UNTAGGED] 	= "untagged",
				[Q8021_VLAN_PORT_TAGGED] 	= "tagged",
		},
		{
				[Q8021_VLAN_PORT_UNTAGGED] 	= "unTagged",
				[Q8021_VLAN_PORT_TAGGED] 	= "Tagged",
		},
};
const char * ip_mode[2][_IP_MODE_MAX] = {
		{
				[_IP_MODE_STATIC]   = "static",
				[_IP_MODE_DHCP]     = "dhcp",
		},
		{
				[_IP_MODE_STATIC]   = "Static",
				[_IP_MODE_DHCP]     = "DHCP",
		},
};

const char * lldp_status_tbl[2][_LLDP_MODE_MAX] ={
		{
				[_LLDP_MODE_ENABLE]     = "enable",
				[_LLDP_MODE_DISABLED]   = "disable",
		},
		{
				[_LLDP_MODE_ENABLE]     = "Enable",
				[_LLDP_MODE_DISABLED]   = "Disable",
		},
};

const char * snmp_status_tbl[2][_SNMP_MODE_MAX] ={
		{
				[_SNMP_MODE_ENABLE]     = "enable",
				[_SNMP_MODE_DISABLED]   = "disable",
		},
		{
				[_LLDP_MODE_ENABLE]     = "Enable",
				[_LLDP_MODE_DISABLED]   = "Disable",
		},
};

const char * snmp_trap_status_tbl[2][_SNMP_TRAP_MODE_MAX] ={
		{
				[_SNMP_TRAP_MODE_ENABLE] 	= "enable",
				[_SNMP_TRAP_MODE_DISABLED] 	= "disable",
		},
		{
				[_SNMP_TRAP_MODE_ENABLE] 	= "Enable",
				[_SNMP_TRAP_MODE_DISABLED] 	= "Disable",
		},
};

const char * power_trap_status_tbl[2][_POWER_TRAP_MODE_MAX] ={
        {
                [_POWER_TRAP_MODE_ENABLE]    = "enable",
                [_POWER_TRAP_MODE_DISABLED]  = "disable",
        },
        {
                [_POWER_TRAP_MODE_ENABLE]    = "Enable",
                [_POWER_TRAP_MODE_DISABLED]  = "Disable",
        },
};


const char * idx_tbl[2][_IDX_MAX] = {
		{
			"idx-1",
			"idx-2",
			"idx-3",
			"idx-4",
			"idx-5",
			"idx-6",
			"idx-7",
			"idx-8",
			"idx-9",
			"idx-10",
			"idx-11",
			"idx-12",
			"idx-13",
			"idx-14",
			"idx-15",
			"idx-16",
		},
		{
			"Idx-1",
			"Idx-2",
			"Idx-3",
			"Idx-4",
			"Idx-5",
			"Idx-6",
			"Idx-7",
			"Idx-8",
			"Idx-9",
			"Idx-10",
			"Idx-11",
			"Idx-12",
			"Idx-13",
			"Idx-14",
			"Idx-15",
			"Idx-16",
		},
};
const char * i2a[16] = {
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
};
const char * mode[10] = {
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
};

const char * qos_assingment[_QOS_ASSINGMENT_MAX] = {
        [_QOS_ASSINGMENT_PORT_BASED]        = "port-based",
        [_QOS_ASSINGMENT_P8021]             = "802.1p",
        [_QOS_ASSINGMENT_DSCP]              = "dscp",
        [_QOS_ASSINGMENT_P8021_DSCP]        = "802.1p&dscp",
};

const char * qos_default_priority[_QOS_DEFAULT_PRIORITY_MAX] = {
        [_QOS_DEFAULT_PRIORITY_LOWEST]      = "lowest",
        [_QOS_DEFAULT_PRIORITY_SELLOW]      = "sellow",
        [_QOS_DEFAULT_PRIORITY_SELHIGH]     = "selhigh",
        [_QOS_DEFAULT_PRIORITY_HIGHEST]     = "highest",
};

const char * qos_schedul_mode[_QOS_SCHEDUL_MODE_MAX] = {
        [_QOS_SCHEDUL_MODE_SR]              = "sr",
        [_QOS_SCHEDUL_MODE_WRR]             = "wrr",
};

const char * qos_map_rule[_QOS_MAP_RULE_MAX] = {
        [_QOS_MAP_RULE_DSCP]                = "dscp",
        [_QOS_MAP_RULE_P8021]               = "802.1p",
};

const char * if_trap_threshold_tbl[_IF_TRAP_THRESHOLD_MODE_MAX] ={
        [_IF_TRAP_THRESHOLD_MODE_ENABLE]    = "enable",
        [_IF_TRAP_THRESHOLD_MODE_DISABLE]   = "disable",
};

const char * rate_limit_mode[_RATE_LIMITING_MODE_MAX] ={
        [_RATE_LIMITING_MODE_ENABLE]    = "enable",
        [_RATE_LIMITING_MODE_DISABLE]   = "disable",
};

const char * rate_limit_frame_type[_RATE_LIMITING_TYPE_MAX] = {
        [_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME]     = "unknow-unicast",
        [_RATE_LIMITING_TYPE_UNKNOWN_MULTICAST_FRAME]   = "unknow-multicast",
        [_RATE_LIMITING_TYPE_BROADCAST_FRAME]           = "broadcast",
        [_RATE_LIMITING_TYPE_MULTICAST_FRAME]           = "multicast",
        [_RATE_LIMITING_TYPE_UNICAST_FRAME]             = "unicast",
};

const char * if_fdb_mode[_IF_FDB_MODE_MAX] = {
        [_IF_FDB_MODE_ENABLE]    = "enable",
        [_IF_FDB_MODE_DISABLE]   = "disable",
};

const char * jrpp_mode[_JRPP_MODE_MAX] = {
        [_JRPP_MODE_ENABLE]    = "enable",
        [_JRPP_MODE_DISABLE]   = "disable",
};

const char * ring_mode[_RING_MODE_MAX] = {
        [_RING_MODE_ENABLE]    = "enable",
        [_RING_MODE_DISABLE]   = "disable",
};

const char * ring_prio[_RING_PRIO_MAX] = {
        [_RING_PRIO_LOW]   = "lowest",
        [_RING_PRIO_MID]   = "middle",
        [_RING_PRIO_HIG]   = "highest",
};

struct show_tbl info_tbl[] = {
		{.name = "type", 					.tbl = (char **)port_type_tbl[1]},
		{.name = "admin_mode", 				.tbl = (char **)port_config_admin_mode_tbl[1]},
		{.name = "operational_status", 		.tbl = (char **)port_config_operational_status_tbl[1]},
		{.name = "physical_mode",			.tbl = (char **)port_config_physical_mode_tbl[1]},
		{.name = "physical_status", 		.tbl = (char **)port_config_physical_status_tbl[1]},
		{.name = "flow_control_mode", 		.tbl = (char **)port_config_flow_control_mode_tbl[1]},
		{.name = "dot1d_state", 			.tbl = (char **)port_config_dot1d_state_tbl[1]},
};

struct show_tbl* str_parse_show(const char * str){
	int i=0;
	for (i=0; i<info_tbl_num; i++){
		if(strcmp(str, info_tbl[i].name) == 0){
			return (struct show_tbl *)(&info_tbl[i]);
		}
	}
	return NULL;
}

const char * port_config_admin_mode_to_mode(const char *str){
	int i;
	for(i=0; i<_PORT_CONFIG_ADMIN_MODE_MAX; i++){
		if(port_config_admin_mode_tbl[0][i] != NULL
				&& strcmp(str, port_config_admin_mode_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char *port_config_speed_to_mode(const char *str){
	int i;
	for(i=0; i<_PORT_CONFIG_PHYSICAL_MODE_MAX; i++){
		if(port_config_physical_mode_tbl[0][i] != NULL
				&& strcmp(str, port_config_physical_mode_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char *port_config_flow_control_to_mode(const char *str){
	int i;
	for(i=0; i<_RATE_LIMITING_MODE_MAX; i++){
		if(port_config_flow_control_mode_tbl[0][i] != NULL
				&& strcmp(str, port_config_flow_control_mode_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char * port_idx_to_port(const char *str){
    int i;
    for(i=PORT_1; i<_PORT_MAX; i++){
        if(strcmp(str, port_name[0][i]) == 0){
            return port_name[2][i];
        }
    }
    return NULL;
}
const char *port_name_to_show(const char *str){
	int i;
	for(i=PORT_1; i<_PORT_MAX; i++){
		if(strcmp(str, port_name[0][i]) == 0){
			return port_name[1][i];
		}
	}
	return NULL;
}
const char *port_ubus_to_show(const char *str){
    int i;
    for(i=PORT_1; i<_PORT_MAX; i++){
        if(strcmp(str, port_name[2][i]) == 0){
            return port_name[1][i];
        }
    }
    return NULL;
}
const char *port_name_to_ubus(const char *str){
	int i;
	for(i=PORT_1; i<_PORT_MAX; i++){
		if(strcmp(str, port_name[2][i]) == 0){
			return port_name[0][i];
		}
	}
	return NULL;
}
const char *port_name_to_port(const char *str){
    int i;
    for(i=PORT_1; i<_PORT_MAX; i++){
        if(strcmp(str, port_name[2][i]) == 0){
            return port_name[2][i];
        }
    }
    return NULL;
}
const char *port_name_to_num(const char *str){
	int i;
	for(i=PORT_1; i<_PORT_MAX; i++){
		if(strcmp(str, port_name[2][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char *portconfig_to_show(const char *str){
	int i;
	for(i=0; i<_PORT_CFG_MAX; i++){
		if(strcmp(str, port_config_name[0][i]) == 0){
			return port_config_name[1][i];
		}
	}
	return NULL;
}
int portconfig_to_ubus(const char *str){
	int i;
	for(i=0; i<_PORT_CFG_MAX; i++){
		if(strcmp(str, port_config_name[0][i]) == 0){
			return i;
		}
	}
	return -1;
}
const char *port_mirror_mode_to_show(const char *str){
	int i;
	for(i=0; i<_PORT_MIRROR_MODE_MAX; i++){
		if(strcmp(str, i2a[i]) == 0){
			return port_mirror_mode_tbl[1][i];
		}
	}
	return NULL;
}
const char *port_mirror_mode_to_mode(const char *str){
	int i;
	for(i=0; i<_PORT_MIRROR_MODE_MAX; i++){
		if(port_mirror_mode_tbl[0][i] != NULL
				&& strcmp(str, port_mirror_mode_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char *port_aggr_mode_to_show(const char *str){
	int i;
	for(i=0; i<_PORT_AGGR_MODE_MAX; i++){
		if(strcmp(str, i2a[i]) == 0){
			return port_aggr_mode_tbl[1][i];
		}
	}
	return NULL;
}
const char *port_aggr_mode_to_mode(const char *str){
	int i;
	for(i=0; i<_PORT_AGGR_MODE_MAX; i++){
		if(port_aggr_mode_tbl[0][i] != NULL
				&& strcmp(str, port_aggr_mode_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char *port_aggr_load_balance_to_show(const char *str){
	int i;
	for(i=0; i<_PORT_AGGR_LOAD_BALANCE_MAX; i++){
		if(strcmp(str, i2a[i]) == 0){
			return port_aggr_load_balance_tbl[1][i];
		}
	}
	return NULL;
}
const char *port_aggr_load_balance_to_mode(const char *str){
	int i;
	for(i=0; i<_PORT_AGGR_LOAD_BALANCE_MAX; i++){
		if(port_aggr_load_balance_tbl[0][i] != NULL
				&& strcmp(str, port_aggr_load_balance_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char *idx_to_show(const char *str){
	int i;
	for(i=0; i<_IDX_MAX; i++){
		if(strcmp(str, idx_tbl[0][i]) == 0){
			return idx_tbl[1][i];
		}
	}
	return NULL;
}
const char *idx_to_ubus(const char *str){
	int i;
	for(i=0; i<_IDX_MAX; i++){
		if(strcmp(str, idx_tbl[0][i]) == 0){
			return idx_tbl[0][i];
		}
	}
	return NULL;
}
const char *mcast_enable_status_to_show(const char *str){
	int i;
	for(i=0; i<_MCAST_ENABLE_STATUS_MAX; i++){
		if(strcmp(str, i2a[i]) == 0){
			return mcast_enable_status_tbl[1][i];
		}
	}
	return NULL;
}
const char *mcast_enable_status_to_mode(const char *str){
	int i;
	for(i=0; i<_MCAST_ENABLE_STATUS_MAX; i++){
		if(mcast_enable_status_tbl[0][i] != NULL
				&& strcmp(str, mcast_enable_status_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}

const char *vlan_mode_to_show(const char *str){
	int i;
	for(i=0; i<_VLAN_MODE_MAX; i++){
		if(strcmp(str, i2a[i]) == 0){
			return vlan_mode_tbl[1][i];
		}
	}
	return NULL;
}
const char *vlan_mode_to_mode(const char *str){
	int i;
	for(i=0; i<_VLAN_MODE_MAX; i++){
		if(vlan_mode_tbl[0][i] != NULL
				&& strcmp(str, vlan_mode_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}

const char *q8021_vlan_port_mode_to_show(const char *str){
	int i;
	for(i=0; i<_Q8021_VLAN_PORT_MAX; i++){
		if(strcmp(str, i2a[i]) == 0){
			return q8021_vlan_port_mode_tbl[1][i];
		}
	}
	return NULL;
}
const char *q8021_vlan_port_mode_to_mode(const char *str){
	int i;
	for(i=0; i<_Q8021_VLAN_PORT_MAX; i++){
		if(vlan_mode_tbl[0][i] != NULL
				&& strcmp(str, q8021_vlan_port_mode_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}

const char *lldp_to_mode(const char *str){
	int i;
	for(i=0; i<_LLDP_MODE_MAX; i++){
		if(lldp_status_tbl[0][i] != NULL
				&& strcmp(str, lldp_status_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char *snmp_to_mode(const char *str){
	int i;
	for(i=0; i<_SNMP_MODE_MAX; i++){
		if(snmp_status_tbl[0][i] != NULL
				&& strcmp(str, snmp_status_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}
const char *snmp_trap_to_mode(const char *str){
	int i;
	for(i=0; i<_SNMP_TRAP_MODE_MAX; i++){
		if(snmp_trap_status_tbl[0][i] != NULL
				&& strcmp(str, snmp_trap_status_tbl[0][i]) == 0){
			return i2a[i];
		}
	}
	return NULL;
}

const char *power_trap_to_mode(const char *str){
    int i;
    for(i=0; i<_POWER_TRAP_MODE_MAX; i++){
        if(power_trap_status_tbl[0][i] != NULL
                && strcmp(str, power_trap_status_tbl[0][i]) == 0){
            return i2a[i];
        }
    }
    return NULL;
}

const char *qos_assingment_to_ubus(const char *str){
    int i;
    for(i=_QOS_ASSINGMENT_PORT_BASED; i<_QOS_ASSINGMENT_MAX; i++){
        if(qos_assingment[i] != NULL
                && strcmp(str, qos_assingment[i]) == 0){
            return i2a[i];
        }
    }
    return NULL;
}
const char *qos_default_priority_to_ubus(const char *str){
    int i;
    for(i=_QOS_DEFAULT_PRIORITY_LOWEST; i<_QOS_DEFAULT_PRIORITY_MAX; i++){
        if(qos_default_priority[i] != NULL
                && strcmp(str, qos_default_priority[i]) == 0){
            return i2a[i];
        }
    }
    return NULL;
}
const char *qos_schedul_mode_to_ubus(const char *str){
    int i;
    for(i=_QOS_SCHEDUL_MODE_SR; i<_QOS_SCHEDUL_MODE_MAX; i++){
        if(qos_schedul_mode[i] != NULL
                && strcmp(str, qos_schedul_mode[i]) == 0){
            return i2a[i];
        }
    }
    return NULL;
}
const char *qos_map_rule_to_ubus(const char *str){
    int i;
    for(i=_QOS_MAP_RULE_DSCP; i<_QOS_MAP_RULE_MAX; i++){
        if(qos_schedul_mode[i] != NULL
                && strcmp(str, qos_map_rule[i]) == 0){
            return i2a[i];
        }
    }
    return NULL;
}
const char *rate_limit_mode_to_ubus(const char *str){
    int i;
    for(i=_RATE_LIMITING_MODE_ENABLE; i<_RATE_LIMITING_MODE_MAX; i++){
        if(rate_limit_mode[i] != NULL
                && strcmp(str, rate_limit_mode[i]) == 0){
            return i2a[i];
        }
    }
    return NULL;
}

const char * mcast_mac_address_to_ubus(char * mac_address){
	int i;
	int addr_len = strlen(mac_address);
	if(17 == addr_len){
		for(i=0; i<addr_len; i++){
		    if(1==i && (mac_address[i]>0x39 ? (mac_address[i]%2):!(mac_address[i]%2)))
		        return NULL;
			if(0 == (i+1)%3 && '-' != mac_address[i])
				return NULL;
			if(0 == (i+1)%3 && '-' == mac_address[i])
				continue;
			if(0x30 > mac_address[i] || 0x67 <= mac_address[i])
				return NULL;
			if(0x3a <= mac_address[i] && 0x60 >= mac_address[i])
				return NULL;
		}
		return mac_address;
	}
	return NULL;
}


cs_32 _str_idx(char * src, char ** array, cs_u32 len){
    cs_32 i;
    if(src && len>0){
        for(i=0; i<len; i++){
            if(array[i] && !(strcmp(src, array[i]))){
                return i;
            }
        }
        return -1;
    }
    return -1;
}
