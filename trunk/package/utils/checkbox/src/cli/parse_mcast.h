#ifndef _PARSE_MCAST
#define _PARSE_MCAST
#include "../caselib/cs_type.h"
int parse_mcast_config_get_group_members(char* argv);
int parse_mcast_enable(char* argv);
int parse_mcast_disable(char* argv);
int parse_mcast_view_init(char* argv);
int parse_mcast_set_group_members_one(char* argv);
int parse_mcast_add_group_members_one(char* argv);
int parse_mcast_del_group_members_one(char* argv);
int parse_mcast_clear(char* argv);
#endif
