#ifndef _UBUS_H_
#define _UBUS_H_

#include "common/common.h"

#define UBUS_CMD_PORT_STATE 1
#define UBUS_CMD_IFACE_BUNDLE_MAC 2
#define UBUS_CMD_IFACE_UNBUNDLE_MAC 3

#define UBUS_PORT_FORWARDING 4
#define UBUS_PORT_BLOCKING 2

typedef enum {
    BO_PORT,
    BO_MAC
}eAccessMode;

typedef enum {
    EAP_RELAY,
    EAP_END
}eEapMode;

typedef enum {
    EAP_PASSTHOUGH,
    EAP_LOCAL
}eEapModel;

struct bss_data;
struct ubus_iface_param {
    uint32_t ifindex;
    char *ifname;
    uint8_t reauth;
    uint8_t access_mode;
    int (*ubus_ops)(struct bss_data *bss,int cmd,void *data);
};

struct ieee802_1x_conf;
int ubus_init(void);
int ubus_get_8021x_status(void *state);
int ubus_get_8021x_conf(struct ieee802_1x_conf *conf);
#endif
