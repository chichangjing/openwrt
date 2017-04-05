#ifndef _MCAST_CONFIG
#define _MCAST_CONFIG
#include "../caselib/cs_type.h"

void mcast_config_get_group_num(void);
void mcast_config_get_enable_status(void);
void mcast_config_get_group_members(void);

void mcast_list(void);
void mcast_get_general(void);

void mcast_config_set_enable_status(const char *mode);
void mcast_config_set_mac_address_one(const char *idx, char * mac);
void mcast_config_set_group_members_one(const char *idx, const char *group_members);
void mcast_config_clear_one(const char *idx);
#endif
