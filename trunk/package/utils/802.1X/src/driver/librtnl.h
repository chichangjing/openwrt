#ifndef _LIBRTNL_H_
#define _LIBRTNL_H_

#include "common/common.h"

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

struct driver_init_params;
int rtnl_init(struct uloop_fd *fd_rtnl,struct driver_init_params *param);
void rtnl_del(struct uloop_fd *h);
int rtnl_get_link(int fd);
int rtnl_set_ifstp(int fd,uint32_t ifindex,uint32_t state,void (*cb)(uint32_t p));
#endif
