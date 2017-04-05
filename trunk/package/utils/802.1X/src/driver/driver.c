#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <linux/filter.h>
#include <net/if_arp.h>

#include "driver/driver.h"
#include "driver/librtnl.h"
#include "ubus.h"

#include <linux/if_bridge.h>
struct ieee8023_hdr {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
}STRUCT_PACKED;

static const uint8_t pae_group_addr[ETH_ALEN] ={ 0x01, 0x80, 0xc2, 0x00, 0x00, 0x03 };

#define GET_DRIV_BY_8021X(h) container_of(h,struct driver_data,fd_8021x)
#define GET_DRIV_BY_DHCP(h) container_of(h,struct driver_data,fd_dhcp)

static void *nas_driver_init(void *ctx,struct driver_init_params *param);

static void rawsock_recv(struct uloop_fd *h,uint32_t events)
{
    struct msghdr hdr;
    struct iovec iov[1];
    struct sockaddr_ll addr;
    uint8_t buf[1024];
    int len;
    
    hdr.msg_name    = &addr;
    hdr.msg_namelen = sizeof(struct sockaddr_ll);
    hdr.msg_iov     = iov;
    hdr.msg_iovlen  = 1;
    hdr.msg_control = NULL;
    hdr.msg_controllen  = 0;
    iov->iov_base   = buf;
    iov->iov_len    = sizeof(buf);

    len = recvmsg(h->fd,&hdr,MSG_TRUNC);
    if(len <= 0 || len > sizeof(buf))
    {
        LOG_ERROR("recvmsg error,len = %d",len);
        return;
    }

    if(len < ETH_HLEN)
    {
        LOG_ERROR("len = %d,too short",len);
        return;
    }

    struct ieee8023_hdr *ethhdr = (struct ieee8023_hdr *)buf;
    if(ntohs(ethhdr->ethertype) == ETH_P_PAE)
    {
        LOG_NORMAL("receive an EAPOL packet");
        
        void *ctx = GET_BSS(h,8021X);    
        union event_data event = {
            .eapol_rx.src = ethhdr->src,
            .eapol_rx.data = (uint8_t *)(ethhdr + 1),
            .eapol_rx.data_len = len - ETH_HLEN
        };

        bss_event_handle(ctx,EVENT_EAPOL_RX,&event);
    }
    else
    {
        LOG_ERROR("invalid ether type");
        return;
    }
}


static int rawsock_send(void *priv, const uint8_t *addr, const uint8_t *data,size_t data_len,const uint8_t *own_addr)
{
    struct driver_data *drv = priv;
    struct ieee8023_hdr *ethhdr;
    size_t len;

    len = sizeof(struct ieee8023_hdr) + data_len;
    ethhdr = calloc(1,len);
    if(ethhdr == NULL)
        return -1;

    memcpy(ethhdr->dst,drv->use_pae_group_addr?pae_group_addr:addr,ETH_ALEN);
    memcpy(ethhdr->src,own_addr,ETH_ALEN);
    ethhdr->ethertype = htons(ETH_P_PAE);
    memcpy(ethhdr + 1,data,data_len);

    struct iovec iov = {
        .iov_base    = (uint8_t *)ethhdr,
        .iov_len     = len
    };
    struct msghdr hdr = {
        .msg_name    = NULL,
        .msg_namelen = 0,
        .msg_iov     = &iov,
        .msg_iovlen  = 1
    };

    int ret = sendmsg(drv->fd_8021x.fd,&hdr,0);
    if(ret < 0)
    {
        LOG_ERROR("sendmsg error = %d",ret);
        return -1;
    }
    else if(ret != len)
    {
        LOG_ERROR("sendmsg len = %d shorter than expect",ret);
        return -1;
    }
    
    return 0;
}

static int rawsock_init(struct uloop_fd *fd_8021x,struct driver_init_params *param)
{
    int fd;
    fd = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_PAE));
    if(fd < 0)
        return -1;

    struct ifreq ifr;
    memset(&ifr,0,sizeof(struct ifreq));
    strcpy(ifr.ifr_name,param->ifname);
    if(ioctl(fd,SIOCGIFINDEX,&ifr))
        goto out_close;

    struct sockaddr_ll toaddr = {
        .sll_ifindex    = ifr.ifr_ifindex,
        .sll_family     = AF_PACKET,
        .sll_protocol   = htons(ETH_P_PAE), 
    };
    
    //LOG_NORMAL("this is in %s - %d",__func__,__LINE__);
    if(bind(fd,(struct sockaddr *)&toaddr,sizeof(toaddr)) < 0)
    {
        LOG_ERROR("bind fail");
        goto out_close;
    }

    if(param->prog != NULL && setsockopt(fd,SOL_SOCKET,SO_ATTACH_FILTER,param->prog,sizeof(struct sock_fprog)) < 0)
    {
        LOG_ERROR("set socket option fail");
        goto out_close;
    }

    struct packet_mreq mreq;
    memset(&mreq,0,sizeof(struct packet_mreq));
    mreq.mr_ifindex = ifr.ifr_ifindex;
    mreq.mr_type = PACKET_MR_MULTICAST;
    mreq.mr_alen = ETH_ALEN;
    memcpy(mreq.mr_address, pae_group_addr, ETH_ALEN);

    if(setsockopt(fd,SOL_PACKET,PACKET_ADD_MEMBERSHIP,&mreq,sizeof(struct packet_mreq)))
    {
        LOG_ERROR("add 802.1x socket into mutilcast group address fail");
        goto out_close;
    }

    memset(&ifr,0,sizeof(struct ifreq));
    strcpy(ifr.ifr_name,param->ifname);
    if(ioctl(fd,SIOCGIFHWADDR,&ifr) || ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
        goto out_close;

    LOG_D("iface addr  = "MACSTR,MAC2STR(ifr.ifr_hwaddr.sa_data));
    memcpy(param->own_addr,ifr.ifr_hwaddr.sa_data,ETH_ALEN);

    fd_8021x->fd = fd;
    fd_8021x->cb = rawsock_recv;

    return 0;

out_close:
    LOG_ERROR("%m");
    close(fd);
    return -1;
}

static int set_port_authoriz(void *priv,void *link,int op)
{
    struct driver_data *drv = priv;

    int state = op?UBUS_PORT_FORWARDING:UBUS_PORT_BLOCKING;

    drv->ubus_ops(drv->ctx,UBUS_CMD_PORT_STATE,&state);
    if(*(int *)link == 0)
        return 0;

    state = op?BR_STATE_FORWARDING:BR_STATE_BLOCKING;
    return rtnl_set_ifstp(drv->fd_rtnl.fd,atoi(drv->ifname + 4),state,NULL);
}

static int nas_set_port_state(void *priv,int access_mode)
{
    struct driver_data *drv = priv;
    int state;

    if(access_mode == BO_PORT)
    {
        state = UBUS_PORT_BLOCKING;
        return drv->ubus_ops(drv->ctx,UBUS_CMD_PORT_STATE,&state);
    }

    state = BR_STATE_FORWARDING;
    return rtnl_set_ifstp(drv->fd_rtnl.fd,atoi(drv->ifname + 4),state,NULL);
}

static int set_mac_authoriz(void *priv,void *addr,int op)
{
    struct driver_data *drv = priv;
    
    return drv->ubus_ops(drv->ctx,op?UBUS_CMD_IFACE_BUNDLE_MAC:UBUS_CMD_IFACE_UNBUNDLE_MAC,addr);
}

struct driver_ops driver_ops = {
    .driver_init = nas_driver_init,
    .driver_send_eapol = rawsock_send,
    .driver_set_port_state = nas_set_port_state,
};

static void *nas_driver_init(void *ctx,struct driver_init_params *param)
{
    struct driver_data *drv;
    drv = calloc(1,sizeof(struct driver_data));
    if(drv == NULL)
        return NULL;
    
    drv->ctx = ctx;
    drv->ifname = param->ifname;
    drv->use_pae_group_addr = param->use_pae_group_addr;
    drv->ifindex = param->ifindex;
    drv->ubus_ops = param->ubus_ops;

    if(rawsock_init(&drv->fd_8021x,param))
    {
        LOG_ERROR("raw socket init error");
        goto out_free;
    }

    if(rtnl_init(&drv->fd_rtnl,param))
    {
        LOG_ERROR("rtnetlink init error");
        goto out_free;
    }

    if(uloop_fd_add(&drv->fd_8021x, ULOOP_READ) < 0)
    {
        LOG_ERROR("add rawsock fd into epoll fail");
        goto out_free;
    }

    if(uloop_fd_add(&drv->fd_rtnl, ULOOP_READ) < 0)
    {
        LOG_ERROR("add rtnetlink fd into epoll fail");
        goto out_free;
    }

    if(rtnl_get_link(drv->fd_rtnl.fd))
    {
        LOG_ERROR("rtnetlink get link status fail");
        goto out_free;
    }

    int state;
    if(param->access_mode == BO_PORT)
    {
        state = UBUS_PORT_BLOCKING; 
        driver_ops.driver_set_authorized = set_port_authoriz;
    }
    else
    {
        state = UBUS_PORT_FORWARDING;  
        driver_ops.driver_set_authorized = set_mac_authoriz;
    }

    drv->ubus_ops(drv->ctx,UBUS_CMD_PORT_STATE,&state);

    return drv;

out_free:
    free(drv);
    return NULL;
}


