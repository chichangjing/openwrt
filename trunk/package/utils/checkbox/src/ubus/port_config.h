#ifndef _PORT_CONFIG
#define _PORT_CONFIG
#if 0
void port_config_get_type_all_port(void);
void port_config_get_admin_mode_all_port(void);
void port_config_get_operational_status_all_port(void);
void port_config_get_physical_mode_all_port(void);
void port_config_get_physical_status_all_port(void);
void port_config_get_flow_control_mode_all_port(void);
void port_config_get_dot1d_state_all_port(void);
void port_config_get_config_one_port_all(const char *port);
void port_config_get_config_all_all(void);
#endif


void port_config_get_port_config_one(const char * port);
void port_config_get_admin_mode_one(const char * port);
void port_config_set_admin_mode_one(const char *port, const char *mode);
void port_config_set_physical_mode_one(const char *port, const char *mode);
void port_config_set_flow_control_mode_one(const char *port, const char *mode);
void port_config_set_admin_mode_all(const char *mode);
void port_config_set_physical_mode_all(const char *mode);
void port_config_set_flow_control_mode_all(const char *mode);
void set_if_trap_threshold_tx_status(const char * port, const char * mode);
void set_if_trap_threshold_rx_status(const char * port, const char * mode);
void get_if_trap_threshold_tx_status_d(const char * port);
void get_if_trap_threshold_rx_status_d(const char * port);
void set_if_trap_threshold_tx_value(const char * port, const char * value);
void set_if_trap_threshold_rx_value(const char * port, const char * value);
void get_if_trap_all(void);
void set_if_trap_port_link_status(const char * port, const char * mode);
#endif
