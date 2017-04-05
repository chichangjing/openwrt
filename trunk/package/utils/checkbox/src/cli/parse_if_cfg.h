#ifndef _PARSE_IF_CFG
#define _PARSE_IF_CFG

int parse_display_interface(char* argv);

int parse_interface_view_init(char* argv);

int parse_port_isolate(char* argv);
int parse_port_unisolate(char* argv);

int parse_interface_port_mirror_set_mode(char* argv);
int parse_interface_port_mirror_disable(char * argv);
int parse_interface_port_mirror_set_destnation(char* argv);
int parse_interface_admin_mode(char* argv);
int parse_interface_flow_control_mode(char* argv);
int parse_interface_physical_mode(char* argv);

int parse_interface_qos_mode(char* argv);
int parse_interface_qos_schedul_mode(char* argv);
int parse_interface_default_priority(char* argv);
int parse_interface_map_rule(char* argv);

int parse_if_trap_threshold_tx_disable(char* argv);
int parse_if_trap_threshold_tx_enable(char* argv);
int parse_if_trap_threshold_rx_disable(char* argv);
int parse_if_trap_threshold_rx_enable(char* argv);
int parse_if_trap_threshold_tx_value(char* argv);
int parse_if_trap_threshold_rx_value(char* argv);
int parse_if_trap_port_link_enable(char* argv);
int parse_if_trap_port_link_disable(char* argv);

#if 0
int parse_port_config_set_admin_mode_one(char* argv);
int parse_port_config_set_physical_mode_one(char* argv);
int parse_port_config_set_flow_control_mode_one(char* argv);


int parse_port_config_get_config_all_all(char* argv);
int parse_port_config_get_config_one_port_all(char* argv);
int parse_port_config_get_type_all_port(char* argv);
int parse_port_config_get_admin_mode_all_port(char* argv);
int parse_port_config_get_operational_status_all_port(char* argv);
int parse_port_config_get_physical_mode_all_port(char* argv);
int parse_port_config_get_physical_status_all_port(char* argv);
int parse_port_config_get_flow_control_mode_all_port(char* argv);
int parse_port_config_get_dot1d_state_mode_all_port(char* argv);
#endif

#if 0
int parse_port_config_set_admin_mode_all(char* argv);
int parse_port_config_set_physical_mode_all(char* argv);
int parse_port_config_set_flow_control_mode_all(char* argv);
#endif

#endif
