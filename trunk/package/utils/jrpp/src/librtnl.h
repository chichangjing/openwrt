#ifndef _LIBRTNL_H_
#define _LIBRTNL_H_

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "utils.h"

typedef enum {
    RTNL_SET_STP
}eRtnlReq;

typedef struct {
    struct list_head list;
    uint32_t seq;
    uint32_t lport;
    eRtnlReq type;
    void (*req_cb)(uint32_t p);
}req_unit_t;

int rtnl_init(uint32_t groups);
void rtnl_del(void);
int rtnl_get_link(void);
int rtnl_set_state(uint32_t ifindex,uint32_t state,void (*cb)(uint32_t p));
#endif
