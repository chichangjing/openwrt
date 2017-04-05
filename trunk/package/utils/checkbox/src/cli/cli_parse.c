#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "parse_aggr.h"
#include "parse_flow.h"
#include "parse_if_cfg.h"
#include "parse_lldp.h"
#include "parse_mcast.h"
#include "parse_mirror.h"
#include "parse_qos.h"
#include "parse_snmp.h"
#include "parse_system.h"
#include "parse_vlan.h"
#include "parse_rate_limit.h"
#include "parse_fdb.h"
#include "parse_obring.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#define CB_SUCCESS  0
#define CB_FAIL     -1

int parse_system_view_init(char * argv);

_NEW_OPT_NODE(_optn_ports,                port_name[2]);
_NEW_OPT_NODE(_optn_mirror_mode,          port_mirror_mode_tbl[0]);
_NEW_OPT_NODE(_optn_aggr_mode,            port_aggr_mode_tbl[0]);
_NEW_OPT_NODE(_optn_aggr_load_balance,    port_aggr_load_balance_tbl[0]);
_NEW_OPT_NODE(_optn_mcast_enable_status,  mcast_enable_status_tbl[0]);
_NEW_OPT_NODE(_optn_vlan_mode,            vlan_mode_tbl[0]);
_NEW_OPT_NODE(_optn_q8021_vlan_port_mode, q8021_vlan_port_mode_tbl[0]);
_NEW_OPT_NODE(_optn_ip_mode,              ip_mode[0]);
_NEW_OPT_NODE(_optn_lldp_status,          lldp_status_tbl[0]);
_NEW_OPT_NODE(_optn_snmp_status,          snmp_status_tbl[0]);
_NEW_OPT_NODE(_optn_snmp_trap_status,     snmp_trap_status_tbl[0]);
_NEW_OPT_NODE(_optn_if_speed,             port_config_physical_mode_tbl[0]);
_NEW_OPT_NODE(_optn_if_admin_mode,        port_config_admin_mode_tbl[0]);
_NEW_OPT_NODE(_optn_if_flow_ctl,          port_config_flow_control_mode_tbl[0]);
_NEW_OPT_NODE(_optn_qos_assingment,       qos_assingment);
_NEW_OPT_NODE(_optn_qos_default_priority, qos_default_priority);
_NEW_OPT_NODE(_optn_qos_schedul_mode,     qos_schedul_mode);
_NEW_OPT_NODE(_optn_qos_map_rule,         qos_map_rule);
_NEW_OPT_NODE(_optn_rate_limit_frame,     rate_limit_frame_type);
_NEW_OPT_NODE(_optn_if_fdb_mode,          if_fdb_mode);
_NEW_OPT_NODE(_optn_ring_prio,            ring_prio);

_NEW_OPT_HELP(_optn_qos_priority,         "<priority>");
_NEW_OPT_HELP(_optn_qos_queue,            "<queue>");
_NEW_OPT_HELP(_optn_mac_address,          "<mac-address>");
_NEW_OPT_HELP(_optn_vlan_id,              "<vlan-id>");
_NEW_OPT_HELP(_optn_vlan_name,            "<describe>");
_NEW_OPT_HELP(_optn_udp_port,             "<1~65535>");
_NEW_OPT_HELP(_optn_community,            "<community-name>");
_NEW_OPT_HELP(_optn_server,               "<server-ip>");
_NEW_OPT_HELP(_optn_device_name,          "<device-name>");
_NEW_OPT_HELP(_optn_device_location,      "<device-location>");
_NEW_OPT_HELP(_optn_ip_address,           "<ip-address>");
_NEW_OPT_HELP(_optn_netmask,              "<netmask>");
_NEW_OPT_HELP(_optn_gateway,              "<gateway>");
_NEW_OPT_HELP(_optn_aggregation,          "<aggregation-id>");
_NEW_OPT_HELP(_optn_ip,                   "<ip>");
_NEW_OPT_HELP(_optn_aging_time,           "<aging-time>");
_NEW_OPT_HELP(_optn_ring_id,              "<ring-id>");

_NEW_OPT_LIST(_mirror_destnation_optns,         _optn_ports);
_NEW_OPT_LIST(_mirror_type_interface_optns,     _optn_ports,            _optn_mirror_mode);
_NEW_OPT_LIST(_mirror_type_general_optns,       _optn_mirror_mode);
_NEW_OPT_LIST(_display_mirror_interface_optns,  _optn_ports);
_NEW_OPT_LIST(_aggr_load_balance_optns,         _optn_aggr_load_balance);
_NEW_OPT_LIST(_lldp_status_optns,               _optn_lldp_status);
_NEW_OPT_LIST(_system_ip_mode_optns,            _optn_ip_mode);
_NEW_OPT_LIST(_aggr_mode_optns,                 _optn_aggr_mode);
_NEW_OPT_LIST(_multicast_optns,                 _optn_mcast_enable_status);
_NEW_OPT_LIST(_snmp_status_optns,               _optn_snmp_status);
_NEW_OPT_LIST(_snmp_trap_status_optns,          _optn_snmp_trap_status);
_NEW_OPT_LIST(_aggr_mode_members_optns,         _optn_ports,            _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports);
_NEW_OPT_LIST(_mcast_members_optns,             _optn_ports,            _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports,    _optn_ports);
_NEW_OPT_LIST(_if_interface_optns,              _optn_ports);
_NEW_OPT_LIST(_if_speed_optns,                  _optn_if_speed);
_NEW_OPT_LIST(_if_admin_mode_optns,             _optn_if_admin_mode);
_NEW_OPT_LIST(_if_flow_ctl_optns,               _optn_if_flow_ctl);
_NEW_OPT_LIST(_qos_assingment_optns,            _optn_qos_assingment);
_NEW_OPT_LIST(_qos_default_priority_optns,      _optn_qos_default_priority);
_NEW_OPT_LIST(_qos_schedul_mode_optns,          _optn_qos_schedul_mode);
_NEW_OPT_LIST(_qos_map_rule_optns,              _optn_qos_map_rule);
_NEW_OPT_LIST(_rate_limit_frame_optns,          _optn_rate_limit_frame,_optn_rate_limit_frame,_optn_rate_limit_frame,_optn_rate_limit_frame,_optn_rate_limit_frame);
_NEW_OPT_LIST(_qos_map_optns,                   _optn_qos_priority,     _optn_qos_queue);
_NEW_OPT_LIST(_multicast_mac_address_optns,     _optn_mac_address);
_NEW_OPT_LIST(_vlan_id_optns,                   _optn_vlan_id);
_NEW_OPT_LIST(_vlan_name_optns,                 _optn_vlan_name);
_NEW_OPT_LIST(_vlan_mode_optns,                 _optn_vlan_mode);
_NEW_OPT_LIST(_aggregation_optns,               _optn_aggregation);
_NEW_OPT_LIST(_udp_port_optns,                  _optn_udp_port);
_NEW_OPT_LIST(_community_optns,                 _optn_community);
_NEW_OPT_LIST(_server_optns,                    _optn_server);
_NEW_OPT_LIST(_device_name_optns,               _optn_device_name);
_NEW_OPT_LIST(_device_location_optns,           _optn_device_location);
_NEW_OPT_LIST(_ip_address_optns,                _optn_ip_address,       _optn_netmask,  _optn_gateway);
_NEW_OPT_LIST(_ip_optns,                        _optn_ip);
_NEW_OPT_LIST(_mac_aging_time_optns,            _optn_aging_time);
_NEW_OPT_LIST(_if_fdb_mode_optns,               _optn_if_fdb_mode);
_NEW_OPT_LIST(_ring_id_optns,                   _optn_ring_id);
_NEW_OPT_LIST(_ring_prio_optns,                 _optn_ring_prio);

_NEW_NOD_HELP(_1_display,               "display",              "display device information eg.mirror, vlan, lldp, snmp");
_NEW_NOD_HELP(_2_flow_statistics,       "flow-statistics",      "display the traffic statistics for all ports in realtime");
_NEW_NOD_HELP(_2_mirror,                "mirror",               "display mirroring configuration, eg.destnation, mirroring mode");
_NEW_NOD_HELP(_3_destnation,            "destnation",           "display the destnation port of the device");
_NEW_NOD_HELP(_3_interface,             "interface",            "display the mirroring configuration for a singal port ");
_NEW_NOD_HELP(_3_general,               "general",              "display the mirroring configuration for all ports");
_NEW_NOD_HELP(_2_aggregation,           "trunk",                "display trunk configuration, eg.aggregation mode, load-balance");
_NEW_NOD_HELP(_3_aggr_mode,             "mode",                 "display trunk mode, either enable or disable");
_NEW_NOD_HELP(_3_load_balance,          "load-balance",         "display trunk load-balance mode, either Xor or Hash");
_NEW_NOD_HELP(_3_aggr_group,            "group",                "display ID and members for all aggregation groups");
_NEW_NOD_HELP(_2_multicast,             "multicast",            "display multicast configuration");
_NEW_NOD_HELP(_3_multi_group,           "group",                "display mac and members for all multicast groups");
_NEW_NOD_HELP(_2_vlan,                  "vlan",                 "display vlan configuration");
_NEW_NOD_HELP(_3_vlan_general,          "general",              "display vlan id and members for all vlan groups");
_NEW_NOD_HELP(_2_isolate,               "isolate",              "display isolate infomation");
_NEW_NOD_HELP(_2_neighbor,              "neighbor",             "display infomation of neighbors for all ports");
_NEW_NOD_HELP(_2_snmp,                  "snmp-agent",           "display snmp configuration, eg.community name, port");
_NEW_NOD_HELP(_2_snmp_trap,             "trap",                 "display snmp-trap configuration, eg.ip, udp port of the trap server ");
_NEW_NOD_HELP(_2_device,                "device",               "display device information");
_NEW_NOD_HELP(_2_ip,                    "ip",                   "display device ip information");
_NEW_NOD_HELP(_2_qos_map,               "qos-map",              "");
_NEW_NOD_HELP(_3_qos_map_q8021,         "802.1p",               "");
_NEW_NOD_HELP(_3_qos_map_dscp,          "dscp",                 "");
_NEW_NOD_HELP(_2_rate_limit,            "rate-limit",           "");
_NEW_NOD_HELP(_2_interface,             "interface",            "display traffic statistics, port configuration for a port");
_NEW_NOD_HELP(_2_obring,                "ring",                 "display ob ring information");
_NEW_NOD_HELP(_2_display_cmd,           "command",              "display command history");
_NEW_NOD_HELP(_2_display_cmd_max,       "max",                  "display command history");
_NEW_NOD_HELP(_3_display_cmd_history,   "history",              "display command history");
_NEW_NOD_HELP(_1_aging_time,            "aging-time",           "set mac aging time(15~3600)");
_NEW_NOD_HELP(_1_mirror,                "mirror",               "set mirror destnation or mode");
_NEW_NOD_HELP(_2_mirror_disable,        "disable",              "disable mirror");
_NEW_NOD_HELP(_2_destnation,            "destnation",           "enable mirror function and set mirror destnation port");
_NEW_NOD_HELP(_2_mirror_type,           "type",                 "set mirror mode for singal port or all ports");
_NEW_NOD_HELP(_3_mirror_interface,      "interface",            "set mirror mode for singal port");
_NEW_NOD_HELP(_3_mirror_general,        "general",              "set mirror mode for all ports");
_NEW_NOD_HELP(_1_aggr,                  "trunk",                "trunk configuration, eg.mode, load-balance, group members");
_NEW_NOD_HELP(_2_aggr_group,            "group",                "enter trunk group view with a aggregation id in 1~7");
_NEW_NOD_HELP(_2_aggr_enable,           "static",               "enable trunk for static");
_NEW_NOD_HELP(_2_aggr_disable,          "disable",              "enable trunk for static");
_NEW_NOD_HELP(_2_aggr_load_balance,     "load-balance",         "set trunk load-balance for Xor or Hash");
_NEW_NOD_HELP(_2_aggr,                  "trunk",                "trunk configuration, eg.mode, load-balance, group members");
_NEW_NOD_HELP(_1_aggr_members,          "port",                 "trunk group members configuration");
_NEW_NOD_HELP(_2_aggr_members_set,      "set",                  "set trunk group members");
_NEW_NOD_HELP(_2_aggr_members_add,      "add",                  "add trunk group members");
_NEW_NOD_HELP(_2_aggr_members_del,      "delete",               "delete trunk group members");
_NEW_NOD_HELP(_1_aggr_clear,            "clear",                "clear current trunk group then exit");
_NEW_NOD_HELP(_1_aggr_exit,             "exit",                 "exit current trunk group view");
_NEW_NOD_HELP(_1_multicast,             "multicast",            "set multicast status and group members");
_NEW_NOD_HELP(_2_multicast_enable,      "enable",               "enable multicast");
_NEW_NOD_HELP(_2_multicast_disable,     "disable",              "disable multicast");
_NEW_NOD_HELP(_2_mcast_group,           "mac-address",          "enter multicast group view with a multicast mac address");
_NEW_NOD_HELP(_1_mcast_members,         "port",                 "multicast group members configuration");
_NEW_NOD_HELP(_2_mcast_members_set,     "set",                  "set multicast group members");
_NEW_NOD_HELP(_2_mcast_members_add,     "add",                  "add multicast group members");
_NEW_NOD_HELP(_2_mcast_members_del,     "delete",               "delete multicast group members");
_NEW_NOD_HELP(_1_mcast_clear,           "clear",                "clear current multicast group then exit");
_NEW_NOD_HELP(_1_mcast_exit,            "exit",                 "exit current multicast group view");
_NEW_NOD_HELP(_1_vlan,                  "vlan",                 "select a vlan mode");
_NEW_NOD_HELP(_1_set,                   "set",                  "set device name, location and network");
_NEW_NOD_HELP(_2_name,                  "name",                 "set device name in 3~64 characters");
_NEW_NOD_HELP(_2_loction,               "location",             "set device location in 3~64 characters");
_NEW_NOD_HELP(_1_ip,                    "ip",                   "set device ip");
_NEW_NOD_HELP(_2_address,               "address",              "set device ip address");
_NEW_NOD_HELP(_2_ip_mode,               "ip-mode",              "set device ip mode in static or dynamic");
_NEW_NOD_HELP(_1_snmp_agent,            "snmp-agent",           "set snmp status, port and community name");
_NEW_NOD_HELP(_2_snmp_agent_enable,     "enable",               "enable snmp");
_NEW_NOD_HELP(_2_snmp_agent_disable,    "disable",              "disable snmp");
_NEW_NOD_HELP(_2_snmp_agent_port,       "port",                 "set snmp udp port");
_NEW_NOD_HELP(_2_snmp_agent_community,  "community",            "set snmp udp port");
_NEW_NOD_HELP(_3_snmp_agent_read_only,  "read-only",            "set snmp read-only community name");
_NEW_NOD_HELP(_3_snmp_read_write,       "read&write",           "set snmp read&write community name");
_NEW_NOD_HELP(_1_trap,                  "trap",                 "set snmp-trap status, port or server ip");
_NEW_NOD_HELP(_2_trap_power,            "power",                "trap power on/off to the server");
_NEW_NOD_HELP(_3_trap_power_enable,     "enable",               "enable trap power function");
_NEW_NOD_HELP(_3_trap_power_disable,    "disable",              "disable trap power function");
_NEW_NOD_HELP(_2_trap_snmp_enable,      "enable",               "enable snmp-trap");
_NEW_NOD_HELP(_2_trap_snmp_disable,     "disable",              "disable snmp-trap");
_NEW_NOD_HELP(_2_trap_snmp_port,        "port",                 "set snmp-trap udp port");
_NEW_NOD_HELP(_2_trap_snmp_server,      "server",               "set snmp-trap server ip");
_NEW_NOD_HELP(_1_lldp,                  "lldp",                 "set lldp status");
_NEW_NOD_HELP(_2_lldp_enable,           "enable",               "enable device to transmit lldp message and recive neighbor info");
_NEW_NOD_HELP(_2_lldp_disable,          "disable",              "disable device to transmit lldp message and recive neighbor info");
_NEW_NOD_HELP(_1_interface,             "interface",            "enter a interface to continue");
_NEW_NOD_HELP(_2_port,                  "port",                 "enter a port view");
_NEW_NOD_HELP(_1_if_view_admin_mode,    "mode",                 "enable or disable current port to configure");
_NEW_NOD_HELP(_1_if_view_speed,         "speed",                "select a speed for current port");
_NEW_NOD_HELP(_1_if_view_flow_ctl,      "flow-control",         "disable or enable flow-control in current port");
_NEW_NOD_HELP(_1_if_view_mirror,        "mirror",               "configure current port mirror mode");
_NEW_NOD_HELP(_2_if_view_mirror_type,   "type",                 "set mirror type current port");
_NEW_NOD_HELP(_2_if_view_mirror_dest,   "destnation",           "select current port to be a mirror destnation port");
_NEW_NOD_HELP(_2_if_mirror_disable,     "disable",              "disable mirror function");
_NEW_NOD_HELP(_1_if_isolate,            "isoport",              "isoport two port");
_NEW_NOD_HELP(_1_if_unisolate,          "unisoport",            "unisoport two port");
_NEW_NOD_HELP(_1_if_view_qos,           "qos",                  "configure quality of service for current interface");
_NEW_NOD_HELP(_2_if_view_qos_mode,      "mode",                 "select qos a mode for current interface");
_NEW_NOD_HELP(_2_if_view_qos_def_pri,   "priority",             "specify a default priority for current port");
_NEW_NOD_HELP(_2_if_view_qos_sch_mode,  "schedule",             "select a schedule mode");
_NEW_NOD_HELP(_2_if_view_qos_map_rule,  "map-rule",             "select a qos map rule");
_NEW_NOD_HELP(_1_if_trap,               "trap",                 "config trap function of this port");
_NEW_NOD_HELP(_2_if_trap_port_status,   "link-status",          "select link status trap mode for the interface");
_NEW_NOD_HELP(_3_if_trap_port_enable,   "enable",               "enable link status trap function in this interface");
_NEW_NOD_HELP(_3_if_trap_port_disable,  "disable",              "disable link status trap function in this interface");
_NEW_NOD_HELP(_2_if_trap_thres,         "threshold",            "config threshold trap function for the port");
_NEW_NOD_HELP(_3_if_trap_thres_tx,      "tx",                   "config threshold trap function in the tx direction");
_NEW_NOD_HELP(_3_if_trap_thres_rx,      "rx",                   "config threshold trap function in the rx direction");
_NEW_NOD_HELP(_4_if_trap_thres_enable,  "enable",               "enable threshold trap function");
_NEW_NOD_HELP(_4_if_trap_thres_disable, "disable",              "disable threshold trap function");
_NEW_NOD_HELP(_4_if_trap_thres_value,   "value",                "specify a threshold value in percent");
_NEW_NOD_HELP(_1_if_rate_limit,         "rate-limit",           "config port based rate limiting");
_NEW_NOD_HELP(_2_if_rate_limit_in,      "inbound",              "limit the inbound rate on the port");
_NEW_NOD_HELP(_2_if_rate_limit_out,     "outbound",             "limit the outbound rate on the port");
_NEW_NOD_HELP(_3_if_rate_limit_bucket0, "bucket-0",             "config bucket 0 rate limit for this port");
_NEW_NOD_HELP(_3_if_rate_limit_bucket1, "bucket-1",             "config bucket 1 rate limit for this port");
_NEW_NOD_HELP(_3_if_rate_limit_bucket2, "bucket-2",             "config bucket 2 rate limit for this port");
_NEW_NOD_HELP(_3_if_rate_limit_bucket3, "bucket-3",             "config bucket 3 rate limit for this port");
_NEW_NOD_HELP(_3_if_rate_limit_bucket4, "bucket-4",             "config bucket 4 rate limit for this port");
_NEW_NOD_HELP(_4_if_rate_limit_frame,   "frame",                "select the frame type to limit");
_NEW_NOD_HELP(_4_if_rate_limit_value,   "value",                "specify a rate value to limit");
_NEW_NOD_HELP(_4_if_rate_limit_enable,  "enable",               "select rate limit enable");
_NEW_NOD_HELP(_4_if_rate_limit_disable, "disable",              "select rate limit disable");
_NEW_NOD_HELP(_1_fdb,                   "port-security",        "port-security function");
_NEW_NOD_HELP(_2_fdb_enable,            "enable",               "enable port-security function");
_NEW_NOD_HELP(_2_fdb_disable,           "disable",              "disable port-security function");
_NEW_NOD_HELP(_2_fdb_mac,               "mac-address",          "add a security mac address to this port");
_NEW_NOD_HELP(_1_if_view_exit,          "exit",                 "exit current interface view");
_NEW_NOD_HELP(_2_if_vlan,               "vlan",                 "enter a vlan view");
_NEW_NOD_HELP(_1_vlan_port,             "port",                 "specify port for vlan");
_NEW_NOD_HELP(_2_vlan_tagged,           "tag",                  "specify tag port for vlan");
_NEW_NOD_HELP(_2_vlan_untagged,         "untag",                "specify untag port for vlan");
_NEW_NOD_HELP(_3_vlan_add,              "add",                  "add port for vlan");
_NEW_NOD_HELP(_3_vlan_set,              "set",                  "set port for vlan");
_NEW_NOD_HELP(_3_vlan_del,              "delete",               "delete port from vlan");
_NEW_NOD_HELP(_1_vlan_describe,         "describe",             "describe for vlan");
_NEW_NOD_HELP(_1_vlan_clear,            "clear",                "delete current vlan");
_NEW_NOD_HELP(_1_vlan_exit,             "exit",                 "exit vlan view");
_NEW_NOD_HELP(_1_obring,                "ring",                 "config the ob-ring fuction");
_NEW_NOD_HELP(_2_ring_enable,           "enable",               "enable the ob-ring");
_NEW_NOD_HELP(_2_ring_disable,          "disable",              "disable the ob-ring");
_NEW_NOD_HELP(_2_ring_id,               "ring",                 "enter a ring view for further operation");
_NEW_NOD_HELP(_1_ring_view_enable,      "enable",               "enable current ring");
_NEW_NOD_HELP(_1_ring_view_disable,     "disable",              "disable current ring");
_NEW_NOD_HELP(_1_ring_view_prio,        "priority",             "specify a priority for current ring");
_NEW_NOD_HELP(_1_ring_port,             "port",                 "set member port for the ring");
_NEW_NOD_HELP(_2_ring_master,           "master",               "specify a master port");
_NEW_NOD_HELP(_2_ring_slave,            "slave",                "specify a slave port");
_NEW_NOD_HELP(_1_ring_clear,            "clear",                "clear current ring");
_NEW_NOD_HELP(_1_ring_view_exit,        "exit",                 "exit current ring view");
_NEW_NOD_HELP(_1_qos_map,               "qos-map",              "map quality of service to schedule priority");
_NEW_NOD_HELP(_2_qos_map_q8021,         "802.1p",               "");
_NEW_NOD_HELP(_2_qos_map_dscp,          "dscp",                 "");
_NEW_NOD_HELP(_1_reboot,                "reboot",               "reboot device at once");
_NEW_NOD_HELP(_1_save,                  "save",                 "save current configuration as a default configuration for the device");
_NEW_NOD_HELP(_1_exit_cli,              "exit",                 "exit the terminal");
_NEW_NOD_HELP(_1_password,              "password",             "modify the passwd for current user");
_NEW_NOD_HELP(_1_ping,                  "ping",                 "detect wether an ip can be reached or not");
_NEW_NOD_HELP(_1_clear,                 "clear",                "clear configuration");
_NEW_NOD_HELP(_2_clear_statistics,      "flow-statistics",      "clear traffic statistics to zero on all ports");
_NEW_NOD_HELP(_2_clear_ring,            "ring",                 "clear ring configuration");

_NEW_NOD_LIST(_display_flow_statistics_nodes,               _1_display,             _2_flow_statistics);
_NEW_NOD_LIST(_display_mirror_destnation_nodes,             _1_display,             _2_mirror,              _3_destnation);
_NEW_NOD_LIST(_display_mirror_interface_nodes,              _1_display,             _2_mirror,              _3_interface);
_NEW_NOD_LIST(_display_mirror_general_nodes,                _1_display,             _2_mirror,              _3_general);
_NEW_NOD_LIST(_display_aggregation_mode_nodes,              _1_display,             _2_aggregation,         _3_aggr_mode);
_NEW_NOD_LIST(_display_aggregation_load_balance_nodes,      _1_display,             _2_aggregation,         _3_load_balance);
_NEW_NOD_LIST(_display_aggregation_general_nodes,           _1_display,             _2_aggregation,         _3_general);
_NEW_NOD_LIST(_display_multicast_group_nodes,               _1_display,             _2_multicast);
_NEW_NOD_LIST(_display_vlan_general_nodes,                  _1_display,             _2_vlan);
_NEW_NOD_LIST(_display_isot_general_nodes,                  _1_display,             _2_isolate);
_NEW_NOD_LIST(_display_qos_map_8021p_nodes,                 _1_display,             _2_qos_map,             _3_qos_map_q8021);
_NEW_NOD_LIST(_display_qos_map_dscp_nodes,                  _1_display,             _2_qos_map,             _3_qos_map_dscp);
_NEW_NOD_LIST(_display_lldp_neighbor_nodes,                 _1_display,             _2_neighbor);
_NEW_NOD_LIST(_display_snmp_nodes,                          _1_display,             _2_snmp);
_NEW_NOD_LIST(_display_snmp_trap_nodes,                     _1_display,             _2_snmp_trap);
_NEW_NOD_LIST(_display_device_nodes,                        _1_display,             _2_device);
_NEW_NOD_LIST(_display_ip_nodes,                            _1_display,             _2_ip);
_NEW_NOD_LIST(_display_rate_limit_nodes,                    _1_display,             _2_rate_limit);
_NEW_NOD_LIST(_display_interface_nodes,                     _1_display,             _2_interface);
_NEW_NOD_LIST(_display_obring_nodes,                        _1_display,             _2_obring);
_NEW_NOD_LIST(_display_cmd_history_nodes,                   _1_display,             _2_display_cmd,         _3_display_cmd_history);
_NEW_NOD_LIST(_mirror_destnation_nodes,                     _1_mirror,              _2_destnation);
_NEW_NOD_LIST(_mirror_disable_nodes,                        _1_mirror,              _2_mirror_disable);
_NEW_NOD_LIST(_mirror_type_interface_nodes,                 _1_mirror,              _2_mirror_type,         _3_mirror_interface);
_NEW_NOD_LIST(_mirror_type_general_nodes,                   _1_mirror,              _2_mirror_type,         _3_mirror_general);
_NEW_NOD_LIST(_aggr_enable_nodes,                           _1_aggr,                _2_aggr_enable);
_NEW_NOD_LIST(_aggr_disable_nodes,                          _1_aggr,                _2_aggr_disable);
_NEW_NOD_LIST(_aggr_group_nodes,                            _1_interface,           _2_aggr);
_NEW_NOD_LIST(_aggr_load_balance_nodes,                     _1_aggr,                _2_aggr_load_balance);
_NEW_NOD_LIST(_aggr_view_clear_nodes,                       _1_aggr_clear);
_NEW_NOD_LIST(_aggr_view_members_set_nodes,                 _1_aggr_members,        _2_aggr_members_set);
_NEW_NOD_LIST(_aggr_view_members_add_nodes,                 _1_aggr_members,        _2_aggr_members_add);
_NEW_NOD_LIST(_aggr_view_members_del_nodes,                 _1_aggr_members,        _2_aggr_members_del);
_NEW_NOD_LIST(_aggr_view_exit_nodes,                        _1_aggr_exit);
_NEW_NOD_LIST(_mucast_enable_nodes,                         _1_multicast,           _2_multicast_enable);
_NEW_NOD_LIST(_mucast_disable_nodes,                        _1_multicast,           _2_multicast_disable);
_NEW_NOD_LIST(_mucast_groups_nodes,                         _1_multicast,           _2_mcast_group);
_NEW_NOD_LIST(_mcast_view_clear_nodes,                      _1_aggr_clear);
_NEW_NOD_LIST(_mcast_view_members_set_nodes,                _1_mcast_members,       _2_mcast_members_set);
_NEW_NOD_LIST(_mcast_view_members_add_nodes,                _1_mcast_members,       _2_mcast_members_add);
_NEW_NOD_LIST(_mcast_view_members_del_nodes,                _1_mcast_members,       _2_mcast_members_del);
_NEW_NOD_LIST(_mcast_view_exit_nodes,                       _1_mcast_exit);
_NEW_NOD_LIST(_vlan_mode_nodes,                             _1_vlan);
_NEW_NOD_LIST(_interface_if_view_nodes,                     _1_interface,           _2_port);
_NEW_NOD_LIST(_if_view_speed_nodes,                         _1_if_view_speed);
_NEW_NOD_LIST(_if_view_flow_ctl_nodes,                      _1_if_view_flow_ctl);
_NEW_NOD_LIST(_if_mirror_disable_nodes,                     _1_if_view_mirror,      _2_if_mirror_disable);
_NEW_NOD_LIST(_if_isolate_nodes,                            _1_if_isolate);
_NEW_NOD_LIST(_if_unisolate_nodes,                          _1_if_unisolate);
_NEW_NOD_LIST(_if_view_admin_mode_nodes,                    _1_if_view_admin_mode);
_NEW_NOD_LIST(_if_view_mirror_type_nodes,                   _1_if_view_mirror,      _2_if_view_mirror_type);
_NEW_NOD_LIST(_if_view_mirror_destnation_nodes,             _1_if_view_mirror,      _2_if_view_mirror_dest);
_NEW_NOD_LIST(_if_view_qos_mode_nodes,                      _1_if_view_qos,         _2_if_view_qos_mode);
_NEW_NOD_LIST(_if_view_qos_sche_mode_nodes,                 _1_if_view_qos,         _2_if_view_qos_sch_mode);
_NEW_NOD_LIST(_if_view_qos_default_pri_nodes,               _1_if_view_qos,         _2_if_view_qos_def_pri);
_NEW_NOD_LIST(_if_view_qos_map_rule_nodes,                  _1_if_view_qos,         _2_if_view_qos_map_rule);
_NEW_NOD_LIST(_if_trap_port_enable_nodes,                   _1_if_trap,             _2_if_trap_port_status,     _3_if_trap_port_enable);
_NEW_NOD_LIST(_if_trap_port_disable_nodes,                  _1_if_trap,             _2_if_trap_port_status,     _3_if_trap_port_disable);
_NEW_NOD_LIST(_if_trap_threshold_tx_enable_nodes,           _1_if_trap,             _2_if_trap_thres,           _3_if_trap_thres_tx,            _4_if_trap_thres_enable);
_NEW_NOD_LIST(_if_trap_threshold_tx_disable_nodes,          _1_if_trap,             _2_if_trap_thres,           _3_if_trap_thres_tx,            _4_if_trap_thres_disable);
_NEW_NOD_LIST(_if_trap_threshold_tx_value_nodes,            _1_if_trap,             _2_if_trap_thres,           _3_if_trap_thres_tx,            _4_if_trap_thres_value);
_NEW_NOD_LIST(_if_trap_threshold_rx_enable_nodes,           _1_if_trap,             _2_if_trap_thres,           _3_if_trap_thres_rx,            _4_if_trap_thres_enable);
_NEW_NOD_LIST(_if_trap_threshold_rx_disable_nodes,          _1_if_trap,             _2_if_trap_thres,           _3_if_trap_thres_rx,            _4_if_trap_thres_disable);
_NEW_NOD_LIST(_if_trap_threshold_rx_value_nodes,            _1_if_trap,             _2_if_trap_thres,           _3_if_trap_thres_rx,            _4_if_trap_thres_value);
_NEW_NOD_LIST(_if_in_buck0_enable_nodes,                    _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket0,       _4_if_rate_limit_enable);
_NEW_NOD_LIST(_if_in_buck0_disable_nodes,                   _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket0,       _4_if_rate_limit_disable);
_NEW_NOD_LIST(_if_in_buck0_value_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket0,       _4_if_rate_limit_value);
_NEW_NOD_LIST(_if_in_buck0_frame_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket0,       _4_if_rate_limit_frame);
_NEW_NOD_LIST(_if_in_buck1_enable_nodes,                    _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket1,       _4_if_rate_limit_enable);
_NEW_NOD_LIST(_if_in_buck1_disable_nodes,                   _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket1,       _4_if_rate_limit_disable);
_NEW_NOD_LIST(_if_in_buck1_value_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket1,       _4_if_rate_limit_value);
_NEW_NOD_LIST(_if_in_buck1_frame_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket1,       _4_if_rate_limit_frame);
_NEW_NOD_LIST(_if_in_buck2_enable_nodes,                    _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket2,       _4_if_rate_limit_enable);
_NEW_NOD_LIST(_if_in_buck2_disable_nodes,                   _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket2,       _4_if_rate_limit_disable);
_NEW_NOD_LIST(_if_in_buck2_value_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket2,       _4_if_rate_limit_value);
_NEW_NOD_LIST(_if_in_buck2_frame_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket2,       _4_if_rate_limit_frame);
_NEW_NOD_LIST(_if_in_buck3_enable_nodes,                    _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket3,       _4_if_rate_limit_enable);
_NEW_NOD_LIST(_if_in_buck3_disable_nodes,                   _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket3,       _4_if_rate_limit_disable);
_NEW_NOD_LIST(_if_in_buck3_value_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket3,       _4_if_rate_limit_value);
_NEW_NOD_LIST(_if_in_buck3_frame_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket3,       _4_if_rate_limit_frame);
_NEW_NOD_LIST(_if_in_buck4_enable_nodes,                    _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket4,       _4_if_rate_limit_enable);
_NEW_NOD_LIST(_if_in_buck4_disable_nodes,                   _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket4,       _4_if_rate_limit_disable);
_NEW_NOD_LIST(_if_in_buck4_value_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket4,       _4_if_rate_limit_value);
_NEW_NOD_LIST(_if_in_buck4_frame_nodes,                     _1_if_rate_limit,       _2_if_rate_limit_in,        _3_if_rate_limit_bucket4,       _4_if_rate_limit_frame);
_NEW_NOD_LIST(_if_out_enable_nodes,                         _1_if_rate_limit,       _2_if_rate_limit_out,       _4_if_rate_limit_enable);
_NEW_NOD_LIST(_if_out_disable_nodes,                        _1_if_rate_limit,       _2_if_rate_limit_out,       _4_if_rate_limit_disable);
_NEW_NOD_LIST(_if_out_value_nodes,                          _1_if_rate_limit,       _2_if_rate_limit_out,       _4_if_rate_limit_value);
_NEW_NOD_LIST(_if_fdb_enable_nodes,                         _1_fdb,                 _2_fdb_enable);
_NEW_NOD_LIST(_if_fdb_disable_nodes,                        _1_fdb,                 _2_fdb_disable);
_NEW_NOD_LIST(_if_fdb_mac_nodes,                            _1_fdb,                 _2_fdb_mac);
_NEW_NOD_LIST(_if_view_exit_nodes,                          _1_if_view_exit);
_NEW_NOD_LIST(_if_vlan_nodes,                               _1_interface,           _2_if_vlan);
_NEW_NOD_LIST(_if_vlan_port_tag_add_nodes,                  _1_vlan_port,           _2_vlan_tagged,             _3_vlan_add);
_NEW_NOD_LIST(_if_vlan_port_tag_set_nodes,                  _1_vlan_port,           _2_vlan_tagged,             _3_vlan_set);
_NEW_NOD_LIST(_if_vlan_port_tag_del_nodes,                  _1_vlan_port,           _2_vlan_tagged,             _3_vlan_del);
_NEW_NOD_LIST(_if_vlan_port_untag_add_nodes,                _1_vlan_port,           _2_vlan_untagged,           _3_vlan_add);
_NEW_NOD_LIST(_if_vlan_port_untag_set_nodes,                _1_vlan_port,           _2_vlan_untagged,           _3_vlan_set);
_NEW_NOD_LIST(_if_vlan_port_untag_del_nodes,                _1_vlan_port,           _2_vlan_untagged,           _3_vlan_del);
_NEW_NOD_LIST(_if_vlan_describe_nodes,                      _1_vlan_describe);
_NEW_NOD_LIST(_vlan_clear_nodes,                            _1_vlan_clear);
_NEW_NOD_LIST(_vlan_exit_nodes,                             _1_vlan_exit);
_NEW_NOD_LIST(_jrpp_enable_nodes,                           _1_obring,              _2_ring_enable);
_NEW_NOD_LIST(_jrpp_disable_nodes,                          _1_obring,              _2_ring_disable);
_NEW_NOD_LIST(_jrpp_name_nodes,                             _1_interface,           _2_ring_id);
_NEW_NOD_LIST(_ring_enable_nodes,                           _1_ring_view_enable);
_NEW_NOD_LIST(_ring_disable_nodes,                          _1_ring_view_disable);
_NEW_NOD_LIST(_ring_priori_nodes,                           _1_ring_view_prio);
_NEW_NOD_LIST(_ring_port_master_nodes,                      _1_ring_port,           _2_ring_master);
_NEW_NOD_LIST(_ring_port_slave_nodes,                       _1_ring_port,           _2_ring_slave);
_NEW_NOD_LIST(_ring_clear_nodes,                            _1_ring_clear);
_NEW_NOD_LIST(_jrpp_exit_view_nodes,                        _1_ring_view_exit);
_NEW_NOD_LIST(_set_name_nodes,                              _1_set,                 _2_name);
_NEW_NOD_LIST(_set_location_nodes,                          _1_set,                 _2_loction);
_NEW_NOD_LIST(_set_ip_address_nodes,                        _1_ip,                  _2_address);
_NEW_NOD_LIST(_set_ip_mode_nodes,                           _1_ip,                  _2_ip_mode);
_NEW_NOD_LIST(_lldp_enable_nodes,                           _1_lldp,                _2_lldp_enable);
_NEW_NOD_LIST(_lldp_disable_nodes,                          _1_lldp,                _2_lldp_disable);
_NEW_NOD_LIST(_snmp_agent_enable_nodes,                     _1_snmp_agent,          _2_snmp_agent_enable);
_NEW_NOD_LIST(_snmp_agent_disable_nodes,                    _1_snmp_agent,          _2_snmp_agent_disable);
_NEW_NOD_LIST(_snmp_agent_port_nodes,                       _1_snmp_agent,          _2_snmp_agent_port);
_NEW_NOD_LIST(_snmp_agent_read_only_nodes,                  _1_snmp_agent,          _2_snmp_agent_community,    _3_snmp_agent_read_only);
_NEW_NOD_LIST(_snmp_agent_read_write_nodes,                 _1_snmp_agent,          _2_snmp_agent_community,    _3_snmp_read_write);
_NEW_NOD_LIST(_trap_snmp_enable_nodes,                      _1_trap,                _2_snmp,                    _2_trap_snmp_enable);
_NEW_NOD_LIST(_trap_snmp_disable_nodes,                     _1_trap,                _2_snmp,                    _2_trap_snmp_disable);
_NEW_NOD_LIST(_trap_snmp_port_nodes,                        _1_trap,                _2_snmp,                    _2_trap_snmp_port);
_NEW_NOD_LIST(_trap_snmp_server_nodes,                      _1_trap,                _2_snmp,                    _2_trap_snmp_server);
_NEW_NOD_LIST(_trap_power_enable_nodes,                     _1_trap,                _2_trap_power,              _3_trap_power_enable);
_NEW_NOD_LIST(_trap_power_disable_nodes,                    _1_trap,                _2_trap_power,              _3_trap_power_disable);
_NEW_NOD_LIST(_qos_map_p8021_nodes,                         _1_qos_map,             _2_qos_map_q8021);
_NEW_NOD_LIST(_qos_map_dscp_nodes,                          _1_qos_map,             _2_qos_map_dscp);
_NEW_NOD_LIST(_reboot_nodes,                                _1_reboot);
_NEW_NOD_LIST(_save_nodes,                                  _1_save);
_NEW_NOD_LIST(_exit_cli_nodes,                              _1_exit_cli);
_NEW_NOD_LIST(_password_nodes,                              _1_password);
_NEW_NOD_LIST(_ping_nodes,                                  _1_ping);
_NEW_NOD_LIST(_clear_statistics_nodes,                      _1_clear,               _2_clear_statistics);
_NEW_NOD_LIST(_clear_rings_nodes,                           _1_clear,               _2_clear_ring);
_NEW_NOD_LIST(_aging_time_nodes,                            _1_aging_time);

_NEW_CMD_NULL(_display_flow_statistics,             _display_flow_statistics_nodes,             parse_eth_stats_get_all_all);
_NEW_CMD_NULL(_display_mirror_destnation,           _display_mirror_destnation_nodes,           parse_port_mirror_get_destination_port);
_NEW_CMD_OPTN(_display_mirror_interface,            _display_mirror_interface_nodes,            parse_port_mirror_get_mode_one,         _display_mirror_interface_optns);
_NEW_CMD_NULL(_display_mirror_general,              _display_mirror_general_nodes,              parse_port_mirror_get_mode_all);
_NEW_CMD_NULL(_display_aggregation_mode,            _display_aggregation_mode_nodes,            parse_port_aggr_get_mode_select);
_NEW_CMD_NULL(_display_aggregation_load_balance,    _display_aggregation_load_balance_nodes,    parse_port_aggr_get_load_balance);
_NEW_CMD_NULL(_display_aggregation_general,         _display_aggregation_general_nodes,         parse_port_aggr_get_channel_all);
_NEW_CMD_NULL(_display_multicast_group,             _display_multicast_group_nodes,             parse_mcast_config_get_group_members);
_NEW_CMD_NULL(_display_vlan_general,                _display_vlan_general_nodes,                parse_vlan_get_general);
_NEW_CMD_NULL(_display_isot_general,                _display_isot_general_nodes,                parse_isolate_get_general);
_NEW_CMD_NULL(_display_qos_map_8021p,               _display_qos_map_8021p_nodes,               parse_display_p8021_map_table);
_NEW_CMD_NULL(_display_qos_map_dscp,                _display_qos_map_dscp_nodes,                parse_display_dscp_map_table);
_NEW_CMD_NULL(_display_lldp_neighbor,               _display_lldp_neighbor_nodes,               parse_display_lldp_geighbor);
_NEW_CMD_NULL(_display_snmp,                        _display_snmp_nodes,                        parse_display_snmp);
_NEW_CMD_NULL(_display_snmp_trap,                   _display_snmp_trap_nodes,                   parse_display_snmp_trap);
_NEW_CMD_NULL(_display_device,                      _display_device_nodes,                      parse_display_system_info);
_NEW_CMD_NULL(_display_ip,                          _display_ip_nodes,                          parse_display_network);
_NEW_CMD_NULL(_display_rate_limit,                  _display_rate_limit_nodes,                  parse_display_rate_limit);
_NEW_CMD_OPTN(_display_interface,                   _display_interface_nodes,                   parse_display_interface,                _if_interface_optns);
_NEW_CMD_OPTN(_display_obring,                      _display_obring_nodes,                      parse_display_ring,                     _ring_id_optns);
_NEW_CMD_NULL(_display_cmd_history,                 _display_cmd_history_nodes,                 parse_display_history_cmd);
_NEW_CMD_OPTN(_mac_aging_time,                      _aging_time_nodes,                          parse_mac_aging_time,                   _mac_aging_time_optns);
_NEW_CMD_NULL(_mirror_disable,                      _mirror_disable_nodes,                      parse_port_mirror_disable);
_NEW_CMD_OPTN(_mirror_destnation,                   _mirror_destnation_nodes,                   parse_port_mirror_set_destination,      _mirror_destnation_optns);
_NEW_CMD_OPTN(_mirror_type_interface,               _mirror_type_interface_nodes,               parse_port_mirror_set_mode_one,         _mirror_type_interface_optns);
_NEW_CMD_OPTN(_mirror_type_general,                 _mirror_type_general_nodes,                 parse_port_mirror_set_mode_all,         _mirror_type_general_optns);
_NEW_CMD_OPTN(_if_isolate,                          _if_isolate_nodes,                          parse_port_isolate,                     _if_interface_optns);
_NEW_CMD_OPTN(_if_unisolate,                        _if_unisolate_nodes,                        parse_port_unisolate,                   _if_interface_optns);
_NEW_CMD_OPTN(_aggr_load_balance,                   _aggr_load_balance_nodes,                   parse_port_aggr_set_load_balance,       _aggr_load_balance_optns);
_NEW_CMD_NULL(_aggr_enable,                         _aggr_enable_nodes,                         parse_aggr_enable);
_NEW_CMD_NULL(_aggr_disable,                        _aggr_disable_nodes,                        parse_aggr_disable);
_NEW_CMD_OPTN(_aggr_group,                          _aggr_group_nodes,                          parse_aggr_view_init,                   _aggregation_optns);
_NEW_CMD_NULL(_aggr_view_clear,                     _aggr_view_clear_nodes,                     parse_port_aggr_clear_one);
_NEW_CMD_OPTN(_aggr_view_members_set,               _aggr_view_members_set_nodes,               parse_port_aggr_set_channel_member_one, _aggr_mode_members_optns);
_NEW_CMD_OPTN(_aggr_view_members_add,               _aggr_view_members_add_nodes,               parse_port_aggr_add_channel_member_one, _aggr_mode_members_optns);
_NEW_CMD_OPTN(_aggr_view_members_del,               _aggr_view_members_del_nodes,               parse_port_aggr_del_channel_member_one, _aggr_mode_members_optns);
_NEW_CMD_NULL(_aggr_view_exit,                      _aggr_view_exit_nodes,                      parse_system_view_init);
_NEW_CMD_NULL(_mucast_enable,                       _mucast_enable_nodes,                       parse_mcast_enable);
_NEW_CMD_NULL(_mucast_disable,                      _mucast_disable_nodes,                      parse_mcast_disable);
_NEW_CMD_OPTN(_mucast_groups,                       _mucast_groups_nodes,                       parse_mcast_view_init,                  _multicast_mac_address_optns);
_NEW_CMD_NULL(_mcast_view_clear,                    _mcast_view_clear_nodes,                    parse_mcast_clear);
_NEW_CMD_OPTN(_mcast_view_members_set,              _mcast_view_members_set_nodes,              parse_mcast_set_group_members_one,      _mcast_members_optns);
_NEW_CMD_OPTN(_mcast_view_members_add,              _mcast_view_members_add_nodes,              parse_mcast_add_group_members_one,      _mcast_members_optns);
_NEW_CMD_OPTN(_mcast_view_members_del,              _mcast_view_members_del_nodes,              parse_mcast_del_group_members_one,      _mcast_members_optns);
_NEW_CMD_NULL(_mcast_view_exit,                     _mcast_view_exit_nodes,                     parse_system_view_init);
_NEW_CMD_OPTN(_vlan_mode,                           _vlan_mode_nodes,                           parse_vlan_set_mode,                    _vlan_mode_optns);
_NEW_CMD_OPTN(_interface_if_view,                   _interface_if_view_nodes,                   parse_interface_view_init,              _if_interface_optns);
_NEW_CMD_OPTN(_if_view_speed,                       _if_view_speed_nodes,                       parse_interface_physical_mode,          _if_speed_optns);
_NEW_CMD_OPTN(_if_view_flow_ctl,                    _if_view_flow_ctl_nodes,                    parse_interface_flow_control_mode,      _if_flow_ctl_optns);
_NEW_CMD_OPTN(_if_view_admin_mode,                  _if_view_admin_mode_nodes,                  parse_interface_admin_mode,             _if_admin_mode_optns);
_NEW_CMD_OPTN(_if_view_mirror_type,                 _if_view_mirror_type_nodes,                 parse_interface_port_mirror_set_mode,   _mirror_type_general_optns);
_NEW_CMD_NULL(_if_mirror_disable,                   _if_mirror_disable_nodes,                   parse_port_mirror_disable);
_NEW_CMD_NULL(_if_view_mirror_destnation,           _if_view_mirror_destnation_nodes,           parse_interface_port_mirror_set_destnation);
_NEW_CMD_OPTN(_if_view_qos_mode,                    _if_view_qos_mode_nodes,                    parse_interface_qos_mode,               _qos_assingment_optns);
_NEW_CMD_OPTN(_if_view_qos_sche_mode,               _if_view_qos_sche_mode_nodes,               parse_interface_qos_schedul_mode,       _qos_schedul_mode_optns);
_NEW_CMD_OPTN(_if_view_qos_default_pri,             _if_view_qos_default_pri_nodes,             parse_interface_default_priority,       _qos_default_priority_optns);
_NEW_CMD_OPTN(_if_view_qos_map_rule,                _if_view_qos_map_rule_nodes,                parse_interface_map_rule,               _qos_map_rule_optns);
_NEW_CMD_NULL(_if_trap_port_enable,                 _if_trap_port_enable_nodes,                 parse_if_trap_port_link_enable);
_NEW_CMD_NULL(_if_trap_port_disable,                _if_trap_port_disable_nodes,                parse_if_trap_port_link_disable);
_NEW_CMD_NULL(_if_trap_threshold_tx_enable,         _if_trap_threshold_tx_enable_nodes,         parse_if_trap_threshold_tx_enable);
_NEW_CMD_NULL(_if_trap_threshold_tx_disable,        _if_trap_threshold_tx_disable_nodes,        parse_if_trap_threshold_tx_disable);
_NEW_CMD_NULL(_if_trap_threshold_tx_value,          _if_trap_threshold_tx_value_nodes,          parse_if_trap_threshold_tx_value);
_NEW_CMD_NULL(_if_trap_threshold_rx_enable,         _if_trap_threshold_rx_enable_nodes,         parse_if_trap_threshold_rx_enable);
_NEW_CMD_NULL(_if_trap_threshold_rx_disable,        _if_trap_threshold_rx_disable_nodes,        parse_if_trap_threshold_rx_disable);
_NEW_CMD_NULL(_if_trap_threshold_rx_value,          _if_trap_threshold_rx_value_nodes,          parse_if_trap_threshold_rx_value);
_NEW_CMD_NULL(_if_in_buck0_enable,                  _if_in_buck0_enable_nodes,                  parse_in_buck0_enable);
_NEW_CMD_NULL(_if_in_buck0_disable,                 _if_in_buck0_disable_nodes,                 parse_in_buck0_disable);
_NEW_CMD_NULL(_if_in_buck0_value,                   _if_in_buck0_value_nodes,                   parse_in_buck0_value);
_NEW_CMD_OPTN(_if_in_buck0_frame,                   _if_in_buck0_frame_nodes,                   parse_in_buck0_frame,                   _rate_limit_frame_optns);
_NEW_CMD_NULL(_if_in_buck1_enable,                  _if_in_buck1_enable_nodes,                  parse_in_buck1_enable);
_NEW_CMD_NULL(_if_in_buck1_disable,                 _if_in_buck1_disable_nodes,                 parse_in_buck1_disable);
_NEW_CMD_NULL(_if_in_buck1_value,                   _if_in_buck1_value_nodes,                   parse_in_buck1_value);
_NEW_CMD_OPTN(_if_in_buck1_frame,                   _if_in_buck1_frame_nodes,                   parse_in_buck1_frame,                   _rate_limit_frame_optns);
_NEW_CMD_NULL(_if_in_buck2_enable,                  _if_in_buck2_enable_nodes,                  parse_in_buck2_enable);
_NEW_CMD_NULL(_if_in_buck2_disable,                 _if_in_buck2_disable_nodes,                 parse_in_buck2_disable);
_NEW_CMD_NULL(_if_in_buck2_value,                   _if_in_buck2_value_nodes,                   parse_in_buck2_value);
_NEW_CMD_OPTN(_if_in_buck2_frame,                   _if_in_buck2_frame_nodes,                   parse_in_buck2_frame,                   _rate_limit_frame_optns);
_NEW_CMD_NULL(_if_in_buck3_enable,                  _if_in_buck3_enable_nodes,                  parse_in_buck3_enable);
_NEW_CMD_NULL(_if_in_buck3_disable,                 _if_in_buck3_disable_nodes,                 parse_in_buck3_disable);
_NEW_CMD_NULL(_if_in_buck3_value,                   _if_in_buck3_value_nodes,                   parse_in_buck3_value);
_NEW_CMD_OPTN(_if_in_buck3_frame,                   _if_in_buck3_frame_nodes,                   parse_in_buck3_frame,                   _rate_limit_frame_optns);
_NEW_CMD_NULL(_if_in_buck4_enable,                  _if_in_buck4_enable_nodes,                  parse_in_buck4_enable);
_NEW_CMD_NULL(_if_in_buck4_disable,                 _if_in_buck4_disable_nodes,                 parse_in_buck4_disable);
_NEW_CMD_NULL(_if_in_buck4_value,                   _if_in_buck4_value_nodes,                   parse_in_buck4_value);
_NEW_CMD_OPTN(_if_in_buck4_frame,                   _if_in_buck4_frame_nodes,                   parse_in_buck4_frame,                   _rate_limit_frame_optns);
_NEW_CMD_NULL(_if_out_enable,                       _if_out_enable_nodes,                       parse_out_enable);
_NEW_CMD_NULL(_if_out_disable,                      _if_out_disable_nodes,                      parse_out_disable);
_NEW_CMD_NULL(_if_out_value,                        _if_out_value_nodes,                        parse_out_value);
_NEW_CMD_NULL(_if_fdb_enable,                       _if_fdb_enable_nodes,                       parse_if_fdb_enable);
_NEW_CMD_NULL(_if_fdb_disable,                      _if_fdb_disable_nodes,                      parse_if_fdb_disable);
_NEW_CMD_OPTN(_if_fdb_mac,                          _if_fdb_mac_nodes,                          parse_if_fdb_mac,                       _multicast_mac_address_optns);
_NEW_CMD_NULL(_if_view_exit,                        _if_view_exit_nodes,                        parse_system_view_init);
_NEW_CMD_OPTN(_if_vlan,                             _if_vlan_nodes,                             parse_Q8021_vlan_view_init,             _vlan_id_optns);
_NEW_CMD_OPTN(_if_vlan_port_tag_set,                _if_vlan_port_tag_set_nodes,                parse_Q8021_vlan_tag_port_set,          _mcast_members_optns);
_NEW_CMD_OPTN(_if_vlan_port_tag_add,                _if_vlan_port_tag_add_nodes,                parse_Q8021_vlan_tag_port_add,          _mcast_members_optns);
_NEW_CMD_OPTN(_if_vlan_port_tag_del,                _if_vlan_port_tag_del_nodes,                parse_Q8021_vlan_tag_port_del,          _mcast_members_optns);
_NEW_CMD_OPTN(_if_vlan_port_untag_set,              _if_vlan_port_untag_set_nodes,              parse_Q8021_vlan_untag_port_set,        _mcast_members_optns);
_NEW_CMD_OPTN(_if_vlan_port_untag_add,              _if_vlan_port_untag_add_nodes,              parse_Q8021_vlan_untag_port_add,        _mcast_members_optns);
_NEW_CMD_OPTN(_if_vlan_port_untag_del,              _if_vlan_port_untag_del_nodes,              parse_Q8021_vlan_untag_port_del,        _mcast_members_optns);
_NEW_CMD_OPTN(_if_vlan_describe,                    _if_vlan_describe_nodes,                    parse_Q8021_vlan_name,                  _vlan_name_optns);
_NEW_CMD_NULL(_vlan_clear,                          _vlan_clear_nodes,                          parse_Q8021_vlan_clear);
_NEW_CMD_NULL(_vlan_exit,                           _vlan_exit_nodes,                           parse_system_view_init);
_NEW_CMD_NULL(_jrpp_enable,                         _jrpp_enable_nodes,                         parse_obring_enable);
_NEW_CMD_NULL(_jrpp_disable,                        _jrpp_disable_nodes,                        parse_obring_disable);
_NEW_CMD_OPTN(_jrpp_name,                           _jrpp_name_nodes,                           parse_obring_view_init,                 _ring_id_optns);
_NEW_CMD_NULL(_ring_enable,                         _ring_enable_nodes,                         parse_obring_set_enable);
_NEW_CMD_NULL(_ring_disable,                        _ring_disable_nodes,                        parse_obring_set_disable);
_NEW_CMD_OPTN(_ring_port_master,                    _ring_port_master_nodes,                    parse_obring_set_primary_port,          _mirror_destnation_optns);
_NEW_CMD_OPTN(_ring_port_slave,                     _ring_port_slave_nodes,                     parse_obring_set_second_port,           _mirror_destnation_optns);
_NEW_CMD_OPTN(_ring_priori,                         _ring_priori_nodes,                         parse_obring_set_priority,              _ring_prio_optns);
_NEW_CMD_NULL(_ring_clear,                          _ring_clear_nodes,                          parse_obring_view_clear);
_NEW_CMD_NULL(_jrpp_exit_view,                      _jrpp_exit_view_nodes,                      parse_system_view_init);
_NEW_CMD_OPTN(_set_name,                            _set_name_nodes,                            parse_sysinfo_set_name,                 _device_name_optns);
_NEW_CMD_OPTN(_set_location,                        _set_location_nodes,                        parse_sysinfo_set_location,             _device_location_optns);
_NEW_CMD_OPTN(_set_ip_addr,                         _set_ip_address_nodes,                      parse_netconfig_set_static_ip,          _ip_address_optns);
_NEW_CMD_OPTN(_set_ip_mode,                         _set_ip_mode_nodes,                         parse_sysinfo_set_ip_mode,              _system_ip_mode_optns);
_NEW_CMD_NULL(_lldp_disable,                        _lldp_disable_nodes,                        parse_lldp_disable);
_NEW_CMD_NULL(_lldp_enable,                         _lldp_enable_nodes,                         parse_lldp_enable);
_NEW_CMD_NULL(_snmp_agent_enable,                   _snmp_agent_enable_nodes,                   parse_snmp_enable);
_NEW_CMD_NULL(_snmp_agent_disable,                  _snmp_agent_disable_nodes,                  parse_snmp_disable);
_NEW_CMD_OPTN(_snmp_agent_port,                     _snmp_agent_port_nodes,                     parse_snmp_port,                        _udp_port_optns);
_NEW_CMD_OPTN(_snmp_agent_read_only,                _snmp_agent_read_only_nodes,                parse_snmp_readonly_name,               _community_optns);
_NEW_CMD_OPTN(_snmp_agent_read_write,               _snmp_agent_read_write_nodes,               parse_snmp_readwrite_name,              _community_optns);
_NEW_CMD_NULL(_trap_snmp_enable,                    _trap_snmp_enable_nodes,                    parse_snmp_trap_enable);
_NEW_CMD_NULL(_trap_snmp_disable,                   _trap_snmp_disable_nodes,                   parse_snmp_trap_disable);
_NEW_CMD_OPTN(_trap_snmp_port,                      _trap_snmp_port_nodes,                      parse_snmp_trap_port,                   _udp_port_optns);
_NEW_CMD_OPTN(_trap_snmp_server,                    _trap_snmp_server_nodes,                    parse_snmp_trap_server_ip,              _server_optns);
_NEW_CMD_NULL(_trap_power_enable,                   _trap_power_enable_nodes,                   parse_power_trap_enable);
_NEW_CMD_NULL(_trap_power_disable,                  _trap_power_disable_nodes,                  parse_power_trap_disable);
_NEW_CMD_OPTN(_qos_map_p8021,                       _qos_map_p8021_nodes,                       parse_p8021_map_table_set,               _qos_map_optns);
_NEW_CMD_OPTN(_qos_map_dscp,                        _qos_map_dscp_nodes,                        parse_dscp_map_table_set,                _qos_map_optns);
_NEW_CMD_NULL(_reboot,                              _reboot_nodes,                              parse_reboot);
_NEW_CMD_NULL(_save,                                _save_nodes,                                parse_save_config);
_NEW_CMD_NULL(_exit_cli,                            _exit_cli_nodes,                            exit_cli);
_NEW_CMD_NULL(_pass_word,                           _password_nodes,                            parse_pass_word);
_NEW_CMD_OPTN(_ping,                                _ping_nodes,                                parse_ping,                              _ip_optns);
_NEW_CMD_NULL(_clear_statistics,                    _clear_statistics_nodes,                    parse_clear_statistics_all);
_NEW_CMD_OPTN(_clear_rings,                         _clear_rings_nodes,                         parse_clear_ring,                        _ring_id_optns);


#define _INCLUDE_DISPLAY_CMD    _display_flow_statistics,\
                                _display_interface,\
                                _display_obring,\
                                _display_mirror_destnation,\
                                _display_mirror_interface,\
                                _display_mirror_general,\
                                _display_aggregation_mode,\
                                _display_aggregation_load_balance,\
                                _display_aggregation_general,\
                                _display_multicast_group,\
                                _display_vlan_general,\
                                _display_isot_general,\
                                _display_qos_map_8021p,\
                                _display_qos_map_dscp,\
                                _display_lldp_neighbor,\
                                _display_rate_limit,\
                                _display_snmp,\
                                _display_snmp_trap,\
                                _display_device,\
                                _display_ip,\
                                _display_cmd_history,\
                                _mac_aging_time

struct _cmd_ * _view_system[] = {
                    _INCLUDE_DISPLAY_CMD,

                    _mirror_disable,
                    _mirror_destnation,
                    _mirror_type_interface,
                    _mirror_type_general,

                    _aggr_enable,
                    _aggr_disable,
                    _aggr_load_balance,
                    _aggr_group,

                    _mucast_enable,
                    _mucast_disable,
                    _mucast_groups,

                    _vlan_mode,
                    _interface_if_view,

                    _jrpp_enable,
                    _jrpp_disable,
                    _jrpp_name,

                    _if_vlan,

                    _qos_map_p8021,
                    _qos_map_dscp,

                    _lldp_enable,
                    _lldp_disable,

                    _snmp_agent_enable,
                    _snmp_agent_disable,
                    _snmp_agent_port,
                    _snmp_agent_read_only,
                    _snmp_agent_read_write,

                    _trap_snmp_enable,
                    _trap_snmp_disable,
                    _trap_snmp_port,
                    _trap_snmp_server,
                    _trap_power_enable,
                    _trap_power_disable,

                    _set_name,
                    _set_location,
                    _set_ip_addr,
                    //_system_ip_mode,
                    _pass_word,
                    _ping,
                    _clear_statistics,
                    _clear_rings,
                    _exit_cli,
                    _save,
                    _reboot,
};

struct _cmd_ * _view_aggr[] = {
                    _INCLUDE_DISPLAY_CMD,
                    _aggr_view_members_set,
                    _aggr_view_members_add,
                    _aggr_view_members_del,
                    _aggr_view_clear,
                    _aggr_view_exit,
};

struct _cmd_ * _view_vlan[] = {
                    _INCLUDE_DISPLAY_CMD,
                    _if_vlan_describe,
                    _if_vlan_port_tag_set,
                    _if_vlan_port_tag_add,
                    _if_vlan_port_tag_del,
                    _if_vlan_port_untag_set,
                    _if_vlan_port_untag_add,
                    _if_vlan_port_untag_del,
                    _vlan_clear,
                    _vlan_exit,
};

struct _cmd_ * _view_mcast[] = {
                    _INCLUDE_DISPLAY_CMD,
                    _mcast_view_members_set,
                    _mcast_view_members_add,
                    _mcast_view_members_del,
                    _mcast_view_clear,
                    _mcast_view_exit,
};

struct _cmd_ * _view_obring[] = {
                    _INCLUDE_DISPLAY_CMD,
                    _ring_enable,
                    _ring_disable,
                    _ring_priori,
                    _ring_port_master,
                    _ring_port_slave,
                    _ring_clear,
                    _jrpp_exit_view,
};

struct _cmd_ * _view_if_inetrface[] = {
                    _INCLUDE_DISPLAY_CMD,

                    _if_mirror_disable,
                    _if_view_mirror_destnation,
                    _if_view_mirror_type,
                    _if_isolate,
                    _if_unisolate,
                    _if_view_admin_mode,
                    _if_view_speed,
                    _if_view_flow_ctl,

                    _if_in_buck0_enable,
                    _if_in_buck0_disable,
                    _if_in_buck0_value,
                    _if_in_buck0_frame,
                    _if_in_buck1_enable,
                    _if_in_buck1_disable,
                    _if_in_buck1_value,
                    _if_in_buck1_frame,
                    _if_in_buck2_enable,
                    _if_in_buck2_disable,
                    _if_in_buck2_value,
                    _if_in_buck2_frame,
                    _if_in_buck3_enable,
                    _if_in_buck3_disable,
                    _if_in_buck3_value,
                    _if_in_buck3_frame,
                    _if_in_buck4_enable,
                    _if_in_buck4_disable,
                    _if_in_buck4_value,
                    _if_in_buck4_frame,
                    _if_out_enable,
                    _if_out_disable,
                    _if_out_value,

                    _if_trap_port_enable,
                    _if_trap_port_disable,
                    _if_trap_threshold_tx_enable,
                    _if_trap_threshold_tx_disable,
                    _if_trap_threshold_tx_value,
                    _if_trap_threshold_rx_enable,
                    _if_trap_threshold_rx_disable,
                    _if_trap_threshold_rx_value,

                    _if_view_qos_mode,
                    _if_view_qos_sche_mode,
                    _if_view_qos_default_pri,
                    _if_view_qos_map_rule,

                    _if_fdb_enable,
                    _if_fdb_disable,
                    _if_fdb_mac,

                    _if_view_exit,
};

_NEW_CMD_LIST(_view_system_list,        _view_system);
_NEW_CMD_LIST(_view_aggr_list,          _view_aggr);
_NEW_CMD_LIST(_view_vlan_list,          _view_vlan);
_NEW_CMD_LIST(_view_mcast_list,         _view_mcast);
_NEW_CMD_LIST(_view_if_interface_list,  _view_if_inetrface);
_NEW_CMD_LIST(_view_obring_list,        _view_obring);

int ip_check(const char * ip_address){
	int i, j, m;
	for(i=0, j=0, m=0; i<strlen(ip_address); i++){
		if('.' != ip_address[i] && ('0'>ip_address[i] || '9'<ip_address[i]))
			return CB_FAIL;
		if(0==j && '.' == ip_address[i])
			return CB_FAIL;
		if(j>0  && '.' == ip_address[i]){
			j=0;
			m++;
			continue;
		}
		if(m>3)
			return CB_FAIL;
		j++;
	}
	if(m<3 || '.' == ip_address[i-1])
		return CB_FAIL;
	return CB_SUCCESS;
}

int parse_system_view_init(char * argv){
    char * extra;
    if(!(extra=strtok(argv, " "))){
        memset(device_gt, 0, sizeof(struct device_gt));
        memset(_terminal_view, 0, sizeof(struct _terminal_view));
        view_init(device_name, _view_system_list);
        printf(" %% Enter system view.\n");
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (argv-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}
