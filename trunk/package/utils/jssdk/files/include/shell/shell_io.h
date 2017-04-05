#ifndef _SHELL_IO_H
#define _SHELL_IO_H

#include "sw.h"
#include "sw_api.h"
#include "fal.h"

#define SW_TYPE_DEF(type, parser, show) {type, parser, show}
typedef struct {
    sw_data_type_e data_type;
    sw_error_t(*param_check) ();
    void (*show_func) ();
} sw_data_type_t;

void  set_talk_mode(int mode);
int get_talk_mode(void);
void set_full_cmdstrp(char **cmdstrp);
sw_data_type_t * cmd_data_type_find(sw_data_type_e type);
void  cmd_strtol(char *str, a_uint32_t * arg_val);


sw_error_t cmd_data_check_portmap(char *cmdstr, fal_pbmp_t * val, a_uint32_t size);
void cmd_data_print_portmap(a_uint8_t * param_name, fal_pbmp_t val, a_uint32_t size);
sw_error_t cmd_data_check_confirm(char *cmdstr, a_bool_t def, a_bool_t * val, a_uint32_t size);
void cmd_data_print_confirm(a_uint8_t * param_name, a_bool_t val, a_uint32_t size);
sw_error_t cmd_data_check_uint32(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void cmd_data_print_uint32(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t cmd_data_check_uint16(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void cmd_data_print_uint16(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t cmd_data_check_enable(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void cmd_data_print_enable(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t cmd_data_check_pbmp(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void cmd_data_print_pbmp(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t cmd_data_check_jwsportlist(char *cmd_str, fal_portlist_t *port_list, a_uint32_t size);

#ifdef IN_PORTCONTROL
void cmd_data_print_port_type(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size);
void cmd_data_print_port_link_status(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size);
sw_error_t cmd_data_check_port_physical_mode(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size);
void cmd_data_print_port_physical_mode(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size);
void cmd_data_print_port_physical_status(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size);
sw_error_t cmd_data_check_port_dot1d_state(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size);
void cmd_data_print_port_dot1d_state(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size);
sw_error_t cmd_data_check_port_comb_type(char * cmd_str,a_uint32_t *arg_val,a_uint32_t size);
void cmd_data_print_port_comb_type(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size);
void cmd_data_print_port_comb_port(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size);

#endif

#ifdef IN_MIB
void cmd_data_print_mib(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
#endif

#ifdef IN_MIRROR
sw_error_t cmd_data_check_mirror_source_port_mode(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size);
void cmd_data_print_mirror_source_port_mode(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size);
#endif

#ifdef IN_TRUNK
sw_error_t cmd_data_check_trunk_load_balance(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size);
void cmd_data_print_trunk_load_balance(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size);
sw_error_t cmd_data_check_trunk_pag_mode(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size);
void cmd_data_print_trunk_pag_mode(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size);
//sw_error_t cmd_data_check_trunk_members(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size);
#endif

#ifdef IN_RATE
void cmd_data_print_rate_speed(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size);
#endif

#ifdef IN_MCAST
sw_error_t cmd_data_check_multicast_addr(char * cmd_str, fal_multicast_addr_t *mac_val, a_uint32_t size);
#endif

#ifdef IN_VLAN
sw_error_t cmd_data_check_vlan_mode(char * cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void cmd_data_print_vlan_mode(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size);
sw_error_t cmd_data_check_vlan_egress_tag(char * cmd_str, fal_vlan_egress_tag_t * arg_val, a_uint32_t size);
void cmd_data_print_vlan_egress_tag(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size);
#endif

#ifdef IN_QOS
sw_error_t cmd_data_check_qos_map(char * cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void cmd_data_print_qos_map(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size);
sw_error_t cmd_data_check_qos_schedule_mode(char * cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void cmd_data_print_qos_schedule_mode(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size);
sw_error_t cmd_data_check_qos_assign_mode(char * cmd_str, a_uint32_t * arg_val, a_uint32_t size);
sw_error_t cmd_data_check_qos_default_prio(char * cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#endif

#ifdef IN_FDB
sw_error_t cmd_data_check_mac_blackwhite_list(char * cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void cmd_data_print_mac_blackwhite_list(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size);
sw_error_t cmd_data_check_8021x_mac_atu_status(char * cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#endif

void cmd_data_print_ssdk_cfg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

#endif
