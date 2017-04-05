#ifndef _DEV_INFO_H
#define _DEV_INFO_H
enum {
    DEV_PORT_1,
    DEV_PORT_2,
    DEV_PORT_3,
    DEV_PORT_4,
    DEV_PORT_5,
    DEV_PORT_6,
    DEV_PORT_7,
    DEV_PORT_8,
    DEV_PORT_9,
    DEV_PORT_10,
    _DEV_PORT_MAX,
};
enum {
	_DEV_PORT_CONFIG_ADMIN_MODE_INVALID,
	DEV_PORT_CONFIG_ADMIN_MODE_ENABLED,
	DEV_PORT_CONFIG_ADMIN_MODE_DISABLED,
	_DEV_PORT_CONFIG_ADMIN_MODE_MAX,
};
extern const char * dev_port_name[_DEV_PORT_MAX];
extern const char * dev_port_admin_mode[_DEV_PORT_CONFIG_ADMIN_MODE_MAX];
#endif
