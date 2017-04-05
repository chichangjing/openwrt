#ifndef _PARSE_VLAN
#define _PARSE_VLAN
int parse_isolate_get_general(char* argv);
int parse_vlan_set_mode(char* argv);
int parse_vlan_get_general(char* argv);

int parse_Q8021_vlan_view_init(char * argv);
int parse_Q8021_vlan_tag_port_set(char* argv);
int parse_Q8021_vlan_tag_port_add(char* argv);
int parse_Q8021_vlan_tag_port_del(char* argv);
int parse_Q8021_vlan_untag_port_set(char* argv);
int parse_Q8021_vlan_untag_port_add(char* argv);
int parse_Q8021_vlan_untag_port_del(char* argv);

int parse_Q8021_vlan_name(char* argv);
int parse_Q8021_vlan_clear(char* argv);

#endif
