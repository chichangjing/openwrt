#include "shell_config.h"
#include "shell_sw.h"


/*cmdline tree descript*/
struct cmd_des_t gcmd_des[] = {

    /**********************************************************************************************************
                                                   Port control 
     **********************************************************************************************************/
    #ifdef IN_PORTCONTROL
    {"port", "config port control",
            {
             {"type",				"get", 		"get type of a port", 						"<port_id>", SW_API_PT_TYPE_GET, NULL}, 
             {"link-status",		"get", 		"get link status of a port", 				"<port_id>", SW_API_PT_LINK_STATUS_GET, NULL},
             {"admin-mode",			"set", 		"set admin mode of a port", 				"<port_id> <enable|disable>",  SW_API_PT_ADMIN_MODE_SET, NULL},
             {"admin-mode",			"get", 		"get admin mode of a port", 				"<port_id>", SW_API_PT_ADMIN_MODE_GET, NULL},
             {"physical-mode",		"set", 		"set physical mode of a port",				"<port_id> <auto-neg|10-half|10-full|100-half|100-full|1000-half|1000-full|auto-copper-fiber|auto-fiber>", SW_API_PT_PHYSICAL_MODE_SET, NULL},
             {"physical-mode",		"get", 		"get physical mode of a port",				"<port_id>", SW_API_PT_PHYSICAL_MODE_GET, NULL},
             {"physical-status",	"get", 		"get physical status process of a port",	"<port_id>", SW_API_PT_PHYSICAL_STATUS_GET, NULL},
             {"flowctrl-mode",		"set", 		"set flow control mode of a port", 			"<port_id> <enable|disable>", SW_API_PT_FLOWCTRL_MODE_SET, NULL},
             {"flowctrl-mode",		"get", 		"get flow control mode of a port", 			"<port_id>", SW_API_PT_FLOWCTRL_MODE_GET, NULL},
             {"dot1d-state",		"set", 		"set dot1d state of a port", 				"<port_id> <disabled|blocking|learning|forwarding>", SW_API_PT_DOT1D_STATE_SET, NULL},
             {"dot1d-state",		"get", 		"get dot1d state of a port", 				"<port_id>", SW_API_PT_DOT1D_STATE_GET, NULL},             
             {"status",				"show", 	"show status of all ports", 				"", SW_API_PT_TEST_SHOW_ALL, NULL},
             {"pcs-status",			"get", 		"get pcs status of a port", 				"<port_id>", SW_API_PT_PCS_STATUS_GET, NULL},
             {"comb-type",			"set", 		"set comb port type", 						"<port_id> <1000base-x|100base-fx|copper|auto-detect>", SW_API_PT_COMB_PORT_SET, NULL},
             {"comb-type",			"get", 		"get comb port type", 						"<port_id>", SW_API_PT_COMB_PORT_GET, NULL},
             {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
            },
    },
    #endif

    /**********************************************************************************************************
                                                   Mib 
     **********************************************************************************************************/
    #ifdef IN_MIB
    {"mib", "MIB statistics information process", 
            {
             {"info",				"get",		"get statistics information of a port", 	"<port_id>", SW_API_PT_MIB_GET, NULL},
             {"info",				"clear",	"clear statistics information of a port", 	"<port_id>", SW_API_PT_MIB_CLEAR, NULL},
             {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
            },
    },
    #endif
	
	/**********************************************************************************************************
													mirror
	 **********************************************************************************************************/
	#ifdef IN_MIRROR
	{"mirror", "config port mirror",
			  {
			   {"dest-port",		"set",		"set mirror destination port",				"<port_id>", SW_API_MIRROR_DEST_PORT_SET, NULL},
			   {"dest-port",		"get",		"get mirror destination port",				"", SW_API_MIRROR_DEST_PORT_GET, NULL},
			   {"source-mode",		"set",		"set source port mode",						"<port_id> <tx|rx|both|disable>", SW_API_MIRROR_SOURCE_PORT_MODE_SET, NULL},
			   {"source-mode",		"get",		"get source port mode",						"<port_id>", SW_API_MIRROR_SOURCE_PORT_MODE_GET, NULL},
			   {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
			  },
	},
	#endif
	
	/**********************************************************************************************************
													trunk
	 **********************************************************************************************************/
	#ifdef IN_TRUNK
	{"trunk", "config trunk",
			 {
			  {"mode",				"set",		"set trunk pag mode set",						"<disable|static>", SW_API_TRUNK_PAG_MODE_SET, NULL},
			  {"mode",				"get",		"set trunk pag mode get",						"", SW_API_TRUNK_PAG_MODE_GET, NULL},
			  {"load-balance",		"set",		"set trunk load balance mode",				"<xor|hash>", SW_API_TRUNK_LOAD_BALANCE_SET, NULL},
			  {"load-balance",		"get",		"get trunk load balance mode",				"", SW_API_TRUNK_LOAD_BALANCE_GET, NULL},
			 /* {"id",				"set",		"set trunk group id form 1 to 8",			"", SW_API_TRUNK_ID_SET, NULL},
			  {"members",			"set",		"set trunk group members mask",				"",	SW_API_TRUNK_MEMBERS_SET, NULL},*/
			  {"row",				"create",	"create a new trunk instance",				"<trunk_id> <trunk_members> <member_size>", SW_API_TRUNK_ROW_CREATE, NULL},
			  {"row",				"destroy",	"remove a trunk instance",					"<trunk_id>",	SW_API_TRUNK_ROW_DESTROY, NULL},
			  {"status",			"show",		"show trunk status",						"<trunk_id>",	SW_API_TRUNK_SHOW_STATUS, NULL},
			  {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
			 },
	},
	#endif

	/**********************************************************************************************************
													rate
	 **********************************************************************************************************/
	#ifdef IN_RATE
	{"rate", "ingress/egress rate config",
			 {
			  {"egress-rate",		"set",		"set port egress rate limit",				"<port_id> <speed> <enable|disable>", SW_API_RATE_BUCKET_EGRL_SET, NULL},
			  {"egress-rate",		"get",		"get port egress rate",						"<port_id>", SW_API_RATE_BUCKET_EGRL_GET, NULL},
			  {"ingress-rate",		"set",		"set port ingress rate limit",				"<port_id> <bucket_id> <type_mask> <speed> <enable|disable>",SW_API_RATE_BUCKET_PIRL_SET, NULL},
			  {"ingress-rate",		"get",		"get port ingress rate",					"<port_id> <bucket_id>", SW_API_RATE_BUCKET_PIRL_GET, NULL},
			  {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
			 },
	},
	#endif

	/**********************************************************************************************************
													multicast
	 **********************************************************************************************************/
	#ifdef IN_MCAST
	{"multicast", "static multicast config",
			 {
			  {"admin-mode",		"set",		"enable static multicast config feature",				"<enable|disable>", SW_API_MULTICAST_ENABLE_SET, NULL},
			  {"admin-mode",		"get",		"get static multicast config current enable status",	"", SW_API_MULTICAST_ENABLE_GET, NULL},
			  {"row",				"create",	"create a new multicast instance",						"<multicast_members> <multicast-addr> <member_size>",SW_API_MULTICAST_ROW_CREATE, NULL},
			  {"row",				"destroy",	"remove a  multicast instance",							"<multicast_members> <multicast-addr> <member_size>", SW_API_MULTICAST_ROW_DESTROY, NULL},
			  {"mgmt-frame",		"set",		"set mgmt frame enable/disable mode",					"<mgmt_addr> <enable|disable>", SW_API_MGMT_FRAME_SET, NULL},
			  {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
			 },
	},
	#endif

	/**********************************************************************************************************
													vlan
	 **********************************************************************************************************/
	#ifdef IN_VLAN
	{"vlan", "port based vlan/ieee8021q vlan config",
			 {
			  {"vlan-mode",			"set",		"set vlan mode: port based vlan or ieee8021q vlan",		"<port-based|ieee8021q>", SW_API_VLAN_MODE_SET, NULL},
			  {"vlan-mode",			"get",		"get vlan mode: port based vlan or ieee8021q vlan",		"", SW_API_VLAN_MODE_GET, NULL},
			  {"port-based",		"set",		"set port based vlan ",									"<port_id> <vlan_members> <member_size>", SW_API_PORT_BASED_VLAN_SET, NULL},
			  {"row",				"create",	"create a new ieee8021q vlan instance",					"<vlan_id> <vlan_members> <member_size> <egress_tag>", SW_API_8021Q_VLAN_ROW_CREATE, NULL},
			  {"row",				"destroy",	"remove a  ieee8021q vlan instance",					"<vlan_id>", SW_API_8021Q_VLAN_ROW_DESTROY, NULL},
			  {"row",				"show",		"show row vlan table status",							"", SW_API_8021Q_VLAN_ROW_STATUS, NULL},
			  {"status",			"show",		"show all vlan table status",							"<min member size>", SW_API_8021Q_VLAN_SHOW_STATUS, NULL},
			  {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
			 },
	},
	#endif

	/**********************************************************************************************************
													QOS
	 **********************************************************************************************************/
	#ifdef IN_QOS
	{"qos",  "qos config",
			 {
			  {"map-rule",			"set",		"set QoS port prio map: ip-mapping or tag-mapping",		"<port_id> <ip-mapping|tag-mapping>", SW_API_QOS_PRIO_MAP_SET, NULL},
			  {"map-rule",			"get",		"get QoS port prio map: ip-mapping or tag-mapping",		"<port_id>", SW_API_QOS_PRIO_MAP_GET, NULL},
			  {"schedule-mode",		"set",		"set QoS port schedule mode: SR or WRR",				"<port_id> <SR|WRR>", SW_API_QOS_SCHEDULE_MODE_SET, NULL},
			  {"schedule-mode",		"get",		"get QoS port schedule mode: SR or WRR",				"<port_id>", SW_API_QOS_SCHEDULE_MODE_GET, NULL},
			  /*{"tag-map",			"set",		"set QoS tag mapping",									"<priority> <queue-num>", SW_API_QOS_TAG_PRIO_MAP_SET, NULL},
			  {"ip-map",			"set",		"set QoS ip mapping",									"<priority> <queue-num>", SW_API_QOS_IP_PRIO_MAP_SET, NULL},*/
			  {"assign-mode",		"set",		"set QoS port priority assign mode",					"<port_id> <use-default|use-tag|use-ip|use-tag-ip>", SW_API_QOS_ASSIGN_MODE_SET, NULL},
			  {"default-prio",		"set",		"set QoS default priority",								"<port_id> <prio-lowest}prio-low|prio-high|prio-highest>", SW_API_QOS_DEFAULT_PRIO_SET, NULL},
			  {"status",			"show",		"show QoS all port status",								"", SW_API_QOS_SHOW_STATUS, NULL},
			  {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
			 },
	},
	#endif

	/**********************************************************************************************************
													MAC
	 **********************************************************************************************************/
	#ifdef IN_FDB
	{"mac",  "fdb mac config",
			 {
			  {"aging-time",		"set",		"set fdb mac adress-table aging timeout",				"<aging_time>", SW_API_MAC_AGING_TIMEOUT_SET, NULL},
			  {"secure-port",		"set",		"set mac secure port and enable secure port",			"<port_id> <enable|disable>", SW_API_MAC_SECURE_PORT_ENABLE_SET, NULL},
			  {"row",				"create",	"create a new fdb mac instance",						"<port_id> <mac_addr>", SW_API_MAC_ROW_CREATE, NULL},
			  {"row",				"destroy",	"destroy a fdb mac instance",							"<mac_addr> <port_id>", SW_API_MAC_ROW_DESTROY, NULL},
			  {"mac-address-table",	"show",		"show all mac-address-table",							"", SW_API_MAC_ADDR_TABLE_SHOW, NULL},
			  {"mac-address-table",	"clear",	"clear all mac-address-table",							"", SW_API_MAC_ADDR_TABLE_CLEAR, NULL},
			  {"blacklist",			"show",		"show blacklist mac",									"", SW_API_MAC_SHOW_BLACKLIST, NULL},
			  {"flush-port",		"set",		"flush fdb for port",									"<port_id>", SW_API_MAC_FLUSH_PORT, NULL},
			  {"8021x_port_auth",	"set",		"set 8021x port auth enable or disable",				"<port_id> <enable|disable>", SW_API_8021X_PORT_AUTH_SET, NULL},
			  {"8021x_mac",			"bind",		"bind a mac to port as dynamic or static",				"<port_id> <dynamic|static> <mac_addr>", SW_API_8021X_MAC_BIND, NULL},
			  {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
			 },
	},
	#endif
	
    /*vlan*/

    /*portvlan*/

    /*fdb*/

    /*acl*/

    /*qos*/

    /*igmp*/

    /*leaky*/

    /*rate*/
    
    /*stp*/

    /*led */

    /*misc*/
    
    /**********************************************************************************************************
                                               Register read/write 
     **********************************************************************************************************/
    {"debug", "read/write register",
        {
              {"reg", "get", "read switch register", "<reg_addr> <4>", SW_API_REG_GET, NULL}, 
              {"reg", "set", "write switch register", "<reg_addr> <value> <4>", SW_API_REG_SET, NULL},  
              {"field", "get", "read switch register field", "<reg_addr> <offset> <len> <4>", SW_API_REG_FIELD_GET, NULL}, 
              {"field", "set", "write switch register field", "<reg_addr> <offset> <len> <value> <4>", SW_API_REG_FIELD_SET, NULL}, 
              {"device",  "reset", "reset device",     "", SW_API_SWITCH_RESET, NULL},
              {"device",  "mode-tag", "set device mode",     "", SW_API_SWITCH_MODE_TAG, NULL},
              {"ssdk",  "config", "show ssdk configuration",     "", SW_API_SSDK_CFG, NULL},
              {"phy", "get", "read phy register", "<phy_id> <reg_addr>", SW_API_PHY_REG_GET, NULL},
              {"phy", "set", "write phy register", "<phy_id> <reg_addr> <value>", SW_API_PHY_REG_SET, NULL},
              {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },

    /*debug*/             
    {"device", "set device id",
            { 
              {"id", "set", "set device id", "<dev_id>", SW_CMD_SET_DEVID, cmd_set_devid},
              {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
            },                                 
    },
    
    {"help", "type ? get help", {{NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/}},
    
    {"quit", "type quit/q quit shell", {{NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/}},
    
    {NULL, NULL, {{NULL, NULL, NULL, NULL, (int)NULL, NULL}}} /*end of desc*/
};

