#ifndef _PARSE_MIRROR
#define _PARSE_MIRROR
int parse_port_mirror_get_destination_port(char* argv);
int parse_port_mirror_get_mode_all(char* argv);
int parse_port_mirror_get_mode_one(char* argv);
int parse_port_mirror_set_destination(char* argv);
int parse_port_mirror_set_mode_all(char* argv);
int parse_port_mirror_set_mode_one(char* argv);
int parse_port_mirror_disable(char* argv);

#endif
