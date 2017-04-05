#ifndef _QOS
#define _QOS
void qos_get_dscp_queue(void);
void qos_get_P8021_queue(void);
void qos_get_mode(const char * port);
void qos_set_mode(const char * port, const char * mode);
void qos_set_schedul_mode(const char * port, const char * mode);
void qos_set_default_priority(const char * port, const char * mode);
void qos_set_map_rule(const char * port, const char * mode);

void qos_set_P8021_queue(const char *p8021_id, const char *p8021_priority);
void qos_set_dscp_queue(const char *dcsp_id, const char *dscp_priority);
#endif
