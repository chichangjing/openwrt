#ifndef _DEVICE_INFO
#define _DEVICE_INFO

#include "../caselib/cs_type.h"

#define _IDX_MAX 16
#define _AGGR_PORT 8

struct device_gt{
    int     idx_num;
    int     current_mode;
    int     vlan_port_list;
    int     vlan_tag_list;
    char current_view_name [32];
    union{
        char    name[32];
        cs_u32  port;
        cs_u32  aggr;
        cs_u32  vlan;
        cs_mac  mac;
        cs_u32  ring;
    }view_name;
    char    current_idx[32];
    char    tmp[32];
};
extern struct device_gt device_gt[];
enum {
    SYSTEM_VIEW,
    INTERFACE_VIEW,
    PORT_TRUNK_VIEW,
    VLAN_VIEW,
    MULTICAST_VIEW,
    RSTP_VIEW,
    RING_VIEW,
    _CLI_VIEW_MAX,
};
enum {
	_PORT_TYPE_INVALID,
	_PORT_TYPE_FE,
	_PORT_TYPE_FX,
	_PORT_TYPE_GE,
	_PORT_TYPE_GX,
	_PORT_CONFIG_TYPE_MAX,
};
enum {
	_PORT_CONFIG_ADMIN_MODE_INVALID,
	PORT_CONFIG_ADMIN_MODE_ENABLED,
	PORT_CONFIG_ADMIN_MODE_DISABLED,
	_PORT_CONFIG_ADMIN_MODE_MAX,
};
enum {
	_PORT_COFIG_OPERATIONAL_INVALID,
	_PORT_COFIG_OPERATIONAL_UP,
	_PORT_COFIG_OPERATIONAL_DOWN,
	_PORT_COFIG_OPERATIONAL_TESTING,
	_PORT_CONFIG_OPERATIONAL_STATUS_MAX,
};
enum {
	_SPEED_MODE_INVALID,
	_SPEED_MODE_AUTO_NEGOTIATE,
	_SPEED_MODE_SPEED_HALF_10,
	_SPEED_MODE_SPEED_FULL_10,
	_SPEED_MODE_SPEED_HALF_100,
	_SPEED_MODE_SPEED_FULL_100,
	_SPEED_MODE_SPEED_FULL_1000,
    _PORT_CONFIG_PHYSICAL_MODE_MAX,
};
enum {
	_ACTUAL_INVALID,
	_ACTUAL_INVALID_1,
	ACTUAL_HALF_10,
	ACTUAL_FULL_10,
	ACTUAL_HALF_100,
	ACTUAL_FULL_100,
	ACTUAL_FULL_1000,
	_PORT_CONFIG_PHYSICAL_STATUS_MAX,
};
enum {
    _RATE_LIMITING_MODE_INVALID,
    _RATE_LIMITING_MODE_ENABLE,
    _RATE_LIMITING_MODE_DISABLE,
    _RATE_LIMITING_MODE_MAX,
};
enum {
	_RATE_LIMITING_TYPE_INVALID,
	_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME,
	_RATE_LIMITING_TYPE_UNKNOWN_MULTICAST_FRAME,
	_RATE_LIMITING_TYPE_BROADCAST_FRAME,
	_RATE_LIMITING_TYPE_MULTICAST_FRAME,
	_RATE_LIMITING_TYPE_UNICAST_FRAME,
	_RATE_LIMITING_TYPE_MAX,
};
enum {
	_DOT1D_STATE_INVALID,
	DOT1D_STATE_DISABLED,
	DOT1D_STATE_BLOCKED,
	DOT1D_STATE_LISTENING,
	DOT1D_STATE_LEARNING,
	DOT1D_STATE_FORWARDING,
	_PORT_CONFIG_DOT1D_STATE_MAX,
};
enum {
    _PORT_INVALID,
    PORT_1,
    PORT_2,
    PORT_3,
    PORT_4,
    PORT_5,
    PORT_6,
    PORT_7,
    PORT_8,
    PORT_9,
    PORT_10,
    _PORT_MAX,
};
enum {
    _QOS_ASSINGMENT_INVALID,
    _QOS_ASSINGMENT_PORT_BASED,
    _QOS_ASSINGMENT_P8021,
    _QOS_ASSINGMENT_DSCP,
    _QOS_ASSINGMENT_P8021_DSCP,
    _QOS_ASSINGMENT_MAX,
};
enum {
    _QOS_DEFAULT_PRIORITY_INVALID,
    _QOS_DEFAULT_PRIORITY_LOWEST,
    _QOS_DEFAULT_PRIORITY_SELLOW,
    _QOS_DEFAULT_PRIORITY_SELHIGH,
    _QOS_DEFAULT_PRIORITY_HIGHEST,
    _QOS_DEFAULT_PRIORITY_MAX,
};
enum {
    _QOS_MAP_RULE_INVALID,
    _QOS_MAP_RULE_DSCP,
    _QOS_MAP_RULE_P8021,
    _QOS_MAP_RULE_MAX,
};
enum {
    _QOS_SCHEDUL_MODE_INVALID,
    _QOS_SCHEDUL_MODE_SR,
    _QOS_SCHEDUL_MODE_WRR,
    _QOS_SCHEDUL_MODE_MAX,
};
enum {
    PORT_CFG_TYPE,
    PORT_CFG_ADMIN_MODE,
    PORT_CFG_OPERATIONAL_STATUS,
    PORT_CFG_PHYSICAL_MODE,
    PORT_CFG_PHYSICAL_STATUS,
    //PORT_CFG_FLOW_CONTROL_MODE,
    //PORT_CFG_DOT1D_STATE,
    _PORT_CFG_MAX,
};
enum {
	TX_BYTES,
	RX_BYTES,
	TX_UCAST_PKTS,
	RX_UCAST_PKTS,
	TX_BCAST_PKTS,
	RX_BCAST_PKTS,
	TX_MCAST_PKTS,
	RX_MCAST_PKTS,
	TX_PAUSE_PKTS,
	RX_PAUSE_PKTS,
	_ETH_STATS_TYPE_MAX,
};
enum {
	_PORT_MIRROR_MODE_INVALID,
	_PORT_MIRROR_MODE_TX,
	_PORT_MIRROR_MODE_RX,
	_PORT_MIRROR_MODE_BOTH,
	_PORT_MIRROR_MODE_DISABLED,
	_PORT_MIRROR_MODE_MAX,
};
enum {
	_PORT_AGGR_MODE_INVALID,
	_PORT_AGGR_MODE_DISABLED,
	_PORT_AGGR_MODE_STATIC,
	_PORT_AGGR_MODE_MAX,
};
enum {
	_PORT_AGGR_LOAD_BALANCE_INVALID,
	_PORT_AGGR_LOAD_BALANCE_XOR,
	_PORT_AGGR_LOAD_BALANCE_HASH,
	_PORT_AGGR_LOAD_BALANCE_MAX,
};
enum {
	MCAST_ENABLE_STATUS_INVALID,
	MCAST_ENABLE_STATUS_ENABLE,
	MCAST_ENABLE_STATUS_DISABLE,
	_MCAST_ENABLE_STATUS_MAX,
};
enum {
	VLAN_MODE_INVALID,
	VLAN_MODE_PORTBASED,
	VLAN_MODE_8021D,
	_VLAN_MODE_MAX,
};
enum {
	Q8021_VLAN_PORT_UNTAGGED,
	Q8021_VLAN_PORT_TAGGED,
	_Q8021_VLAN_PORT_MAX,
};
enum {
	_IP_MODE_INVALID,
	_IP_MODE_STATIC,
	_IP_MODE_DHCP,
	_IP_MODE_MAX,
};

enum {
	_LLDP_MODE_INVALID,
	_LLDP_MODE_ENABLE,
	_LLDP_MODE_DISABLED,
	_LLDP_MODE_MAX,
};
enum {
	_SNMP_MODE_INVALID,
	_SNMP_MODE_ENABLE,
	_SNMP_MODE_DISABLED,
	_SNMP_MODE_MAX,
};

enum {
	_SNMP_TRAP_MODE_INVALID,
	_SNMP_TRAP_MODE_ENABLE,
	_SNMP_TRAP_MODE_DISABLED,
	_SNMP_TRAP_MODE_MAX,
};

enum {
    _POWER_TRAP_MODE_INVALID,
    _POWER_TRAP_MODE_ENABLE,
    _POWER_TRAP_MODE_DISABLED,
    _POWER_TRAP_MODE_MAX,
};

enum {
    _IF_TRAP_THRESHOLD_MODE_INVALID,
    _IF_TRAP_THRESHOLD_MODE_ENABLE,
    _IF_TRAP_THRESHOLD_MODE_DISABLE,
    _IF_TRAP_THRESHOLD_MODE_MAX,
};

enum {
    _IF_TRAP_PORT_STATUS_MODE_INVALID,
    _IF_TRAP_PORT_STATUS_MODE_ENABLE,
    _IF_TRAP_PORT_STATUS_MODE_DISABLE,
    _IF_TRAP_PORT_STATUS_MODE_MAX,
};

enum {
    _IF_FDB_MODE_INVALID,
    _IF_FDB_MODE_ENABLE,
    _IF_FDB_MODE_DISABLE,
    _IF_FDB_MODE_MAX,
};

enum {
    _JRPP_MODE_INVALID,
    _JRPP_MODE_ENABLE,
    _JRPP_MODE_DISABLE,
    _JRPP_MODE_MAX,
};

enum {
    _RING_MODE_INVALID,
    _RING_MODE_ENABLE,
    _RING_MODE_DISABLE,
    _RING_MODE_MAX,
};

enum {
    _RING_PRIO_INVALID,
    _RING_PRIO_LOW,
    _RING_PRIO_MID,
    _RING_PRIO_HIG,
    _RING_PRIO_MAX,
};

enum {
	BUCKET0,
	BUCKET1,
	BUCKET2,
	BUCKET3,
	BUCKET4,
	_BUCKET_MAX,
};

extern const char * cli_view_name_tbl[_CLI_VIEW_MAX];
extern const char * port_type_tbl[2][_PORT_CONFIG_TYPE_MAX];
extern const char * port_config_admin_mode_tbl[2][_PORT_CONFIG_ADMIN_MODE_MAX];
extern const char * port_config_operational_status_tbl[2][_PORT_CONFIG_OPERATIONAL_STATUS_MAX];
extern const char * port_config_physical_mode_tbl[2][_PORT_CONFIG_PHYSICAL_MODE_MAX];
extern const char * port_config_physical_status_tbl[2][_PORT_CONFIG_PHYSICAL_STATUS_MAX];
extern const char * port_config_flow_control_mode_tbl[2][_RATE_LIMITING_MODE_MAX];
extern const char * port_config_dot1d_state_tbl[2][_PORT_CONFIG_DOT1D_STATE_MAX];
extern const char * port_name[3][_PORT_MAX];
extern const char * port_config_name[2][_PORT_CFG_MAX];
extern const char * eth_stats_type[2][_ETH_STATS_TYPE_MAX];
extern const char * port_mirror_mode_tbl[2][_PORT_MIRROR_MODE_MAX];
extern const char * port_aggr_mode_tbl[2][_PORT_AGGR_MODE_MAX];
extern const char * port_aggr_load_balance_tbl[2][_PORT_AGGR_LOAD_BALANCE_MAX];
extern const char * mcast_enable_status_tbl[2][_MCAST_ENABLE_STATUS_MAX];
extern const char * vlan_mode_tbl[2][_VLAN_MODE_MAX];
extern const char * q8021_vlan_port_mode_tbl[2][_Q8021_VLAN_PORT_MAX];
extern const char * ip_mode[2][_IP_MODE_MAX];
extern const char * idx_tbl[2][_IDX_MAX];
extern const char * i2a[16];
extern const char * mode[10];
extern const char * lldp_status_tbl[2][_LLDP_MODE_MAX];
extern const char * snmp_status_tbl[2][_SNMP_MODE_MAX];
extern const char * snmp_trap_status_tbl[2][_SNMP_TRAP_MODE_MAX];

extern const char * qos_assingment[_QOS_ASSINGMENT_MAX];
extern const char * qos_default_priority[_QOS_DEFAULT_PRIORITY_MAX];
extern const char * qos_schedul_mode[_QOS_SCHEDUL_MODE_MAX];
extern const char * qos_map_rule[_QOS_MAP_RULE_MAX];
extern const char * rate_limit_mode[_RATE_LIMITING_MODE_MAX];
extern const char * rate_limit_frame_type[_RATE_LIMITING_TYPE_MAX];
extern const char * if_fdb_mode[_IF_FDB_MODE_MAX];
extern const char * jrpp_mode[_JRPP_MODE_MAX];
extern const char * ring_mode[_RING_MODE_MAX];
extern const char * ring_prio[_RING_PRIO_MAX];

struct show_tbl{
	char *name;
	char **tbl;
};
extern struct show_tbl info_tbl[];

#define info_tbl_num  sizeof(info_tbl)/sizeof(info_tbl[0])
const char * port_config_admin_mode_to_mode(const char * str);
const char * port_config_admin_mode_to_mode(const char * str);
const char * port_config_flow_control_to_mode(const char * str);
const char * port_config_speed_to_mode(const char * str);
const char * port_idx_to_port(const char *str);
const char * port_name_to_show(const char * str);
const char * port_ubus_to_show(const char * str);
const char * port_name_to_ubus(const char * str);
const char * port_name_to_port(const char * str);
const char * port_name_to_num(const char * str);
const char * portconfig_parse_show(const char * str);
int   portconfig_parse_ubus(const char * str);
const char * port_mirror_mode_to_show(const char * str);
const char * port_mirror_mode_to_mode(const char * str);
const char * port_aggr_mode_to_show(const char * str);
const char * port_aggr_mode_to_mode(const char * str);
const char * port_aggr_load_balance_to_show(const char * str);
const char * port_aggr_load_balance_to_mode(const char * str);
const char * mcast_enable_status_to_show(const char * str);
const char * mcast_enable_status_to_mode(const char * str);
const char * vlan_mode_to_show(const char * str);
const char * vlan_mode_to_mode(const char * str);
const char * q8021_vlan_port_mode_to_show(const char * str);
const char * q8021_vlan_port_mode_to_mode(const char * str);
const char * lldp_to_mode(const char * str);
const char * snmp_to_mode(const char * str);
const char * snmp_trap_to_mode(const char * str);
const char * power_trap_to_mode(const char * str);
const char * mcast_mac_address_to_ubus(char * mac_address);

const char *qos_assingment_to_ubus(const char *str);
const char *qos_default_priority_to_ubus(const char *str);
const char *qos_schedul_mode_to_ubus(const char *str);
const char *qos_map_rule_to_ubus(const char *str);
struct show_tbl * str_parse_show(const char * str);


cs_32 _str_idx(char * src, char ** array, cs_u32 len);
#define str_idx(__src, __array) (_str_idx(__src, __array, sizeof(__array)/sizeof(__array[0])))
#endif
