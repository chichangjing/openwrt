#ifndef _PARSE_SYSTEM
#define _PARSE_SYSTEM
int parse_display_history_cmd(char* argv);
int parse_display_system_info(char* argv);
int parse_sysinfo_set_name(char* argv);
int parse_sysinfo_set_location(char* argv);
int parse_sysinfo_set_ip_mode(char* argv);
int parse_display_network(char* argv);
int parse_netconfig_set_static_ip(char* argv);
int parse_reboot(char* argv);
int parse_save_config(char* argv);
int parse_pass_word(char* argv);
int parse_ping(char* argv);
#endif
