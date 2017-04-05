#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "common/common.h"

#define GET_BSS(h,by) ((struct driver_data *)(GET_DRIV_BY_ ## by(h)))->ctx

struct bss_data;
struct driver_init_params {
    int ifindex;
    const char *ifname;
    uint8_t *own_addr;
    int access_mode;

    int use_pae_group_addr;
    struct sock_fprog *prog;
    
    int rtnl_group;

    int (*ubus_ops)(struct bss_data *bss,int cmd,void *data);
};

struct driver_data {
    struct uloop_fd fd_8021x;
    struct uloop_fd fd_dhcp;
    struct uloop_fd fd_rtnl;
    const char *ifname;
    void *ctx;
    int use_pae_group_addr;
    int ifindex;
    int (*ubus_ops)(struct bss_data *bss,int cmd,void *data);
};

struct driver_ops{
    void *(*driver_init)(void *ctx,struct driver_init_params *params);   
    void (*driver_deinit)(void *priv);    
    int (*driver_send_eapol)(void *priv, const uint8_t *addr, const uint8_t *data,size_t data_len,const uint8_t *own_addr);
    int (*driver_set_authorized)(void *priv, void *data,int op);
    //int (*driver_deauth)(void *priv, const uint8_t *own_addr, const uint8_t *addr,int reason);
    int (*driver_set_port_state)(void *priv,int access_mode);
};

enum event_type {
    EVENT_NEW_STA,
    EVENT_EAPOL_RX,
    EVENT_LINK
};

union event_data {
    struct new_sta {
        const uint8_t *addr;
    }new_sta;
    struct eapol_rx {
        const uint8_t *src;
        const uint8_t *data;
        size_t data_len;
    }eapol_rx;
    struct link_notify {
        int lport;
        int link;
    }link_notify;
};


void bss_event_handle(void *ctx,enum event_type type,union event_data *event);
#endif
