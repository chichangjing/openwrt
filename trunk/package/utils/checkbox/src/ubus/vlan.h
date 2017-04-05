#ifndef _VLAN
#define _VLAN
void vlan_get_mode(void);
void vlan_set_mode(const char *mode);

void port_base_vlan_get_port_list(void);
void port_base_vlan_set_port_list(const char * idx_1, const char *portlist_1, const char * idx_2, const char *portlist_2);

void vlan_get_num(void);
void Q8021_vlan_get_port_list(void);

void Q8021_vlan_set_id(const char *idx, const char * id);
void Q8021_vlan_set_name(const char * idx, const char * name);
void Q8021_vlan_set_port_list(const char *idx, const char *portlist, const char * taglist);
void Q8021_vlan_clear(const char * idx);

void vlan_get_general(void);
void vlan_list(void);
void isolate_list(void);
#endif
