#ifndef _SYSTEM_INFO
#define _SYSTEM_INFO

void sysinfo_get_all(void);
void sysinfo_set_name(const char *description);
void sysinfo_set_location(const char *sys_location);
void save_config(void);
#endif
