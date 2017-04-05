#ifndef _SNMP
#define _SNMP

void snmp_get_status(void);
void snmp_get_info(void);
void snmp_set_status(const char *status);
void snmp_set_readonly_name(const char * readonly_name);
void snmp_set_readwrite_name(const char * readwrite_name);
void snmp_set_port(const char * port);

void snmp_trap_get_status(void);
void snmp_trap_get_info(void);
void power_trap_get_status(void);
void snmp_trap_set_status(const char *status);
void power_trap_set_status(const char *status);
void snmp_trap_set_trap_port(const char * trap_port);
void snmp_trap_set_trap_server_ip(const char * trap_server_ip);

#endif
