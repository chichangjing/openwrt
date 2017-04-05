#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "driver/driver.h"
#include "librtnl.h"

#include <linux/if_bridge.h>
#define NLMSG_TAIL(nmsg) ((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))
#define RTNL_SEQ_LINK   0

static LIST_HEAD(req_l);

static inline void rtnl_parse_rtattr(struct rtattr **tb,int max,struct rtattr *attr,int len)
{
    for(;RTA_OK(attr,len);attr = RTA_NEXT(attr,len))
    {
        if(attr->rta_type <= max)
            tb[attr->rta_type] = attr;
    }
}

static inline int rtnl_addattr32(struct nlmsghdr *n, int maxlen, int type, uint32_t data)
{
    int len = RTA_LENGTH(sizeof(int));
    struct rtattr *rta;

    if (NLMSG_ALIGN(n->nlmsg_len) + len > maxlen)
        return -1;

    rta = NLMSG_TAIL(n);
    rta->rta_type   = type;
    rta->rta_len    = len;
    memcpy(RTA_DATA(rta), &data, len);

    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + len;
    return 0; 
}

#define GET_DRIV_BY_RTNL(h) container_of(h,struct driver_data,fd_rtnl)
static void rtnl_ifinfomsg(struct uloop_fd *h,struct nlmsghdr *nlh)
{
    struct rtattr *tb[IFLA_MAX + 1];
    struct ifinfomsg *ifinfo = NLMSG_DATA(nlh);

    memset(tb,0,sizeof(tb));

    int len = IFLA_PAYLOAD(nlh);

    rtnl_parse_rtattr(tb,IFLA_MAX,IFLA_RTA(ifinfo),len);

    void *ctx = GET_BSS(h,RTNL);    
    union event_data event = {
        .link_notify.lport = ifinfo->ifi_index,
        .link_notify.link = (ifinfo->ifi_flags & IFF_RUNNING)?1:0
    };

    bss_event_handle(ctx,EVENT_LINK,&event);
    //LOG_NORMAL("%s link %s ",(tb[IFLA_IFNAME])?RTA_DATA(tb[IFLA_IFNAME]):"unknown dev",(ifinfo->ifi_flags & IFF_RUNNING)?"up":"down");
    //notify_link(h,ifinfo->ifi_index,(ifinfo->ifi_flags & IFF_RUNNING)?1:0);
}

static int rtnl_deal_ack(uint32_t seq)
{
    req_unit_t *req,*req_bak;

    list_for_each_entry_safe(req,req_bak,&req_l,list)
    {
        if(req->seq == seq)
        {
            if(req->req_cb != NULL)
                req->req_cb(req->lport);
            list_del(&req->list);
            free(req);
            return 0;
        }
    }

    LOG_ERROR("nobody claim seq %d",seq);
    return -1;
}

static void rtnl_recv(struct uloop_fd *h,uint32_t events)
{
    //LOG_NORMAL("netlink interface active .....");
    int len;
    unsigned char buffer[4096];
    struct nlmsghdr *nlh;
    struct iovec iov = {
        .iov_base = buffer,
        .iov_len  = sizeof(buffer)
    };
    struct sockaddr_nl addr = {.nl_family = AF_NETLINK};
    struct msghdr hdr = {
        .msg_name       = &addr,
        .msg_namelen    = sizeof(struct sockaddr_nl),
        .msg_iov        = &iov,
        .msg_iovlen     = 1
    };
    
    len = recvmsg(h->fd,&hdr,0);
    if(len < 0)
    {
        LOG_ERROR("%m");
        return;
    }
    else if(len == 0)
    {
        LOG_NORMAL("recv nothing");
        return;
    }

    for(nlh = (struct nlmsghdr *)buffer;NLMSG_OK(nlh,len);nlh = NLMSG_NEXT(nlh,len))
    {
        //LOG_NORMAL("seq = %d,pid = %d",nlh->nlmsg_seq,nlh->nlmsg_pid);
        if(nlh->nlmsg_type & NLM_F_MULTI)
        {
            //LOG_NORMAL("this netlink message carrying NLM_F_MULTI flag");
        }

        switch(nlh->nlmsg_type)
        {
            case RTM_NEWLINK:
            case RTM_DELLINK:
                rtnl_ifinfomsg(h,nlh);
                break;
            case NLMSG_DONE:
                LOG_D("get done message!");
                break;
            case NLMSG_ERROR:
                if((nlh->nlmsg_len - sizeof(struct nlmsghdr)) < sizeof(struct nlmsgerr))
                {
                    LOG_ERROR("message may be truncated");
                    return;
                }
                struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nlh);
                if(err->error)
                    LOG_ERROR("get a netlink error = %d",err->error);
                if(rtnl_deal_ack(nlh->nlmsg_seq) < 0)
                    LOG_ERROR("deal rtnetlink ack error");
                break;
            default:
                LOG_ERROR("get unhandle msg type = %d",nlh->nlmsg_type);
                break;
        }
    }
}

static int rtnl_send(int fd,uint8_t *data,uint32_t len)
{
    struct sockaddr_nl addr = {
        .nl_family = AF_NETLINK
    };

    struct iovec iov = {
        .iov_base = data,
        .iov_len  = len
    };

    struct msghdr hdr = {
        .msg_name    = &addr,
        .msg_namelen = sizeof(struct sockaddr_nl),
        .msg_iov     = &iov,
        .msg_iovlen  = 1
    };

    if(sendmsg(fd,&hdr,0) < 0)
    {
        LOG_ERROR("sendmsg error");
        return -1; 
    }

    return 0;
}

int rtnl_init(struct uloop_fd *fd_rtnl,struct driver_init_params *param)
{
    int fd = socket(AF_NETLINK,SOCK_RAW,NETLINK_ROUTE);
    if(fd <= 0)
    {
        LOG_ERROR("create socket error");
        return -1;
    }

    int bufsize = 32768;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int)) < 0)
        goto out_err;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(int)) < 0)
        goto out_err;

    struct sockaddr_nl addr;
    addr.nl_family  = AF_NETLINK;
    addr.nl_pid     = getpid();
    addr.nl_groups  = param->rtnl_group;
    int ret = bind(fd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret < 0)
        goto out_err;

    fd_rtnl->fd = fd;
    fd_rtnl->cb = rtnl_recv;

    return 0;

out_err:
    LOG_ERROR("%m");
    close(fd);
    return -1;
}

void rtnl_del(struct uloop_fd *h)
{
    close(h->fd);
    free(h);
}


int rtnl_get_link(int fd)
{
    struct {
        struct nlmsghdr nlh;
        struct rtgenmsg gen;
    }req = {
        .nlh = {
            .nlmsg_len      = NLMSG_LENGTH(sizeof(struct rtgenmsg)),
            .nlmsg_type     = RTM_GETLINK,
            .nlmsg_flags    = NLM_F_REQUEST | NLM_F_DUMP,
            .nlmsg_seq      = RTNL_SEQ_LINK,
            .nlmsg_pid      = getpid()
        },
        .gen = {.rtgen_family = AF_PACKET}
    };

    return rtnl_send(fd,(uint8_t *)&req,req.nlh.nlmsg_len);
}

int rtnl_set_ifstp(int fd,uint32_t ifindex,uint32_t state,void (*cb)(uint32_t p))
{
    req_unit_t *unit = calloc(1,sizeof(req_unit_t));
    unit->type      = RTNL_SET_STP;
    unit->lport     = ifindex;
    unit->req_cb    = cb;

    if(list_empty(&req_l))
        unit->seq = 1;
    else
        unit->seq = list_last_entry(&req_l,req_unit_t,list)->seq + 1;

    list_add_tail(&unit->list,&req_l);

    struct {
        struct nlmsghdr nlh;
        struct ifinfomsg ifi;
        uint8_t data[256];
    }req = {
        .nlh = {
            .nlmsg_len      = NLMSG_LENGTH(sizeof(struct ifinfomsg)),
            .nlmsg_type     = RTM_SETLINK,
            .nlmsg_flags    = NLM_F_REQUEST | NLM_F_DUMP | NLM_F_ACK,
            .nlmsg_seq      = unit->seq,
            .nlmsg_pid      = getpid()
        },
        .ifi = {
            .ifi_family = AF_BRIDGE,
            .ifi_index  = ifindex
        }
    };

    if(rtnl_addattr32(&req.nlh,sizeof(req),IFLA_PROTINFO,state) < 0)
        return -1;

    LOG_NORMAL("rtnetlink set port %d %s OK",ifindex,(state == BR_STATE_FORWARDING)?"forwarding":"blocking");
    return rtnl_send(fd,(uint8_t *)&req,req.nlh.nlmsg_len);
}
