#ifndef _PORT_MIRROR
#define _PORT_MIRROR
void port_mirror_get_destination_port(void);
void port_mirror_get_mode_all(void);
void port_mirror_get_mode_one(const char *port);

void port_mirror_set_destination(const char *port);
void port_mirror_set_mode_all(const char *mode);
void port_mirror_set_mode_one(const char *port, const char *mode);
#endif
