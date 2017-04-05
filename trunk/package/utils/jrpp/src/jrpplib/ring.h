
#ifndef _RPP_RING_MACHINE_H_
#define _RPP_RING_MACHINE_H_

#include "port.h"

typedef int (*tCmdFunc)(struct rppRing *,tRMsgCmd *);
typedef struct {
    struct uloop_timeout    cmd_timer;
    tCmdFunc                cmd_func;
    tRMsgCmd                cmd_msg;
    int                     ctl_fd;             // unix-domain fd
    char                    u_name[108 + 1];    // unix-domain addr
    unsigned char           ref;                // times that msg will send
}tCmdReq;

typedef struct rppRing {
    RPP_MODE_T      rpp_mode;
    RING_STATUS_T   status;
	NODE_ID_T       master_id;
    eNodeType       node_role;
	unsigned short  switch_cnts; 
    unsigned short  hello_syn;
    unsigned char   endpoint[MAC_LEN];    // endpoint only available in edge node
    tCmdReq         cmd_req;

    struct uloop_timeout fail_timer;
    struct uloop_timeout hello_timer;

    rppPort_t       *primary;
    rppPort_t       *secondary;

    STATE_MACH_T *node;      // 节点状态状态机
    STATE_MACH_T *machines;
}rppRing_t;

/*********************************************************************
                 Functions prototypes 
 *********************************************************************/
int ring_start(rppRing_t *);
int ring_stop(rppRing_t *,eMsgNodeDownType);
int ring_update(rppRing_t *);
int ring_set_state(rppRing_t *,RING_STATUS_T);
void ring_get_topo(rppRing_t *,char *,int);
void ring_set_topo(rppRing_t *ring,char *un,int fd,unsigned char state);
void ring_build_complete_req(rppRing_t *ring,eMsgCompleteType type);
void ring_build_linkdown_req(rppPort_t *this,rppPort_t *peer);
void ring_build_nodedown_req(rppRing_t *ring,rppPort_t *port,eMsgNodeDownType type);
void ring_build_cmd_req(rppRing_t *,eMsgCmdCode,unsigned char *,int);
void ring_build_cmd_rsp(rppRing_t *,rppPort_t *,eMsgCmdCode,unsigned char *,void *);


static inline void ring_set_mode(rppRing_t *ring,RPP_MODE_T mode)
{
    ring->rpp_mode = mode;
}

static inline RPP_MODE_T ring_get_mode(rppRing_t *ring)
{
    return ring->rpp_mode;
}

static inline void ring_reset(rppRing_t *ring)
{
    ring_set_state(ring,RPP_FAULT);
    ring_set_mode(ring,RPP_ENHANCED); // now default rpp mode is enhanced mode
    memset(&ring->master_id,0,sizeof(NODE_ID_T));
    ring->node_role     = NODE_TYPE_TRANSIT;
}



#endif

