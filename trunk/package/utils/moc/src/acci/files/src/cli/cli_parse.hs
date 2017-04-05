#ifndef _CLI_PARSE
#define _CLI_PARSE
#include "cli.h"
extern struct _cmd_list _view_system_list[];
extern struct _cmd_list _view_aggr_list[];
extern struct _cmd_list _view_vlan_list[];
extern struct _cmd_list _view_mcast_list[];
extern struct _cmd_list _view_if_interface_list[];
extern struct _cmd_list _view_obring_list[];
int ip_check(const char * ip_address);
int parse_system_view_init(char * argv);
#define system_view_init parse_system_view_init
#endif
