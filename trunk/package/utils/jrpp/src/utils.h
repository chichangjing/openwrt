
#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

#include <libubox/uloop.h>
#include <libubox/utils.h>

#include "log.h"

int netsock_init(void);
int is_bridge(char *if_name);
int set_bridge_stp(char *br_name, int stp_val);
int get_hwaddr(char *ifname, unsigned char *hwaddr);
int get_bridge_name(char *br_name);
int get_if_mac(const char *if_name, char *mac);
int get_port_count(char *br_name, int *count);

#endif


