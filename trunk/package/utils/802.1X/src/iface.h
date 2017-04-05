#ifndef __IFACE_H
#define __IFACE_H

#include "common/common.h"
#include "conf.h"

#define PORT_NUM 11     // port number (contain cpu port)
#define PORT_CPU_H  9   // physical cpu-port
#define PORT_CPU_L  11  // logical cpu-port 
#define PORT_IF_VALID(lport) (lport < PORT_CPU_L)
#define MAC_LEN 6

typedef enum {
    AUTH_ENABLED = 1,
    AUTH_DISABLED
}eAuthState;

typedef enum {
    LINK_DOWN,
    LINK_UP,
}ePortLinkState;

typedef enum {
    DISABLED,
    LISTENING,
    LEARNING,
    FORWARDING,
    BLOCKING
}ePortStp;

struct eapol_rx;
struct bss_data{
    struct interface *iface;

    uint8_t own_addr[ETH_ALEN];

    const struct driver_ops *driver;
    void *drv_priv;

    ePortLinkState link;
    int num_sta;
    struct sta_info *sta_list;
#define STA_HASH_SIZE 256
#define STA_HASH(sta) (sta[5])
    struct sta_info *sta_hash[STA_HASH_SIZE];

    void (*new_assoc_sta_cb)(struct bss_data *bss,struct sta_info *sta, int reassoc);

    struct radius_client_data *radius;
    struct eapol_authenticator *eapol_auth;
    struct eapol_rx *later;
    
    int ctrl_sock;
};

#define BSS_GET_CONF(bss) (&bss->iface->conf)

struct interface{
    struct iface_conf conf;
    struct bss_data *bss;

    struct list_head node;
};

struct ubus_iface_param;
struct ieee802_1x_conf;

void iface_free(struct interface *iface);
int iface_init(struct ieee802_1x_conf *conf_g,struct ubus_iface_param *param);
int iface_bss_setup(struct interface *iface,struct ieee802_1x_conf *conf);
#endif
