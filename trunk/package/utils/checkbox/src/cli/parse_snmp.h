#ifndef _PARSE_SNMP
#define _PARSE_SNMP
int parse_display_snmp(char* argv);
int parse_snmp_enable(char* argv);
int parse_snmp_disable(char* argv);
int parse_snmp_port(char* argv);
int parse_snmp_readonly_name(char* argv);
int parse_snmp_readwrite_name(char* argv);

int parse_display_snmp_trap(char* argv);
int parse_snmp_trap_enable(char* argv);
int parse_snmp_trap_disable(char* argv);
int parse_snmp_trap_port(char* argv);
int parse_snmp_trap_server_ip(char* argv);

int parse_power_trap_enable(char* argv);
int parse_power_trap_disable(char* argv);
#endif
