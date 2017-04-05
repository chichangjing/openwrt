#ifndef _LIBRTNL_H_
#define _LIBRTNL_H_

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

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

struct uloop_fd *rtnl_init(uint32_t groups);
void rtnl_del(struct uloop_fd *h);
int rtnl_get_link(int fd);
int rtnl_set_ifstp(int fd,uint32_t ifindex,uint32_t state,void (*cb)(uint32_t p));
#endif
