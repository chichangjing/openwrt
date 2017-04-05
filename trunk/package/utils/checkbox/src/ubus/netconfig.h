#ifndef _NET_CONFIG
#define _NET_CONFIG
void netconfig_get_all(void);
void netconfig_get_ip_mode(void);
void netconfig_set_ip_mode(const char * ip_mode);
void netconfig_set_static_ip(const char *ip_address, const char *netmask, const char *gateway);


#endif
