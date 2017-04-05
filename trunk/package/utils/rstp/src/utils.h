
#ifndef _UTILS_H_
#define _UTILS_H_

int netsock_init(void);
int set_bridge_stp(char *br_name, int stp_val);
int get_hwaddr(char *ifname, unsigned char *hwaddr);
int get_bridge_name(char *br_name);
int get_if_mac(int if_index, unsigned char *mac);
int get_port_count(char *br_name, int *count);

#endif


