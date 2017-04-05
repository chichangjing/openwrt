#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "libepoll.h"
#include "rawsock.h"

#define ETH_IF_IDX  4096

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
    if(len <= 0)
    {
        LOG_ERROR("recvmsg error,len = %d",len);
        return;
    }

    crpp_recv(buf,len);
}

int rawsock_send(int fd,uint8_t *data,int len)
{
    struct sockaddr_ll addr = {
        .sll_ifindex    = ETH_IF_IDX,
        .sll_family     = AF_PACKET,
        .sll_protocol   = htons(ETH_P_ALL), 
    };
    struct iovec iov = {
        .iov_base    = data,
        .iov_len     = len
    };
    struct msghdr hdr = {
        .msg_name    = &addr,
        .msg_namelen = sizeof(struct sockaddr_ll),
        .msg_iov     = &iov,
        .msg_iovlen  = 1
    };

    int ret = sendmsg(fd,&hdr,0);
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

struct uloop_fd *rawsock_init(struct sock_fprog *prog)
{
    int fd;

    fd = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(fd < 0)
        return NULL;

    struct ifreq ifr;
    strcpy(ifr.ifr_name,"eth0");
    if(ioctl(fd,SIOCGIFINDEX,&ifr))
        goto out_close;

    struct sockaddr_ll toaddr = {
        .sll_ifindex    = ifr.ifr_ifindex,
        .sll_family     = AF_PACKET,
        .sll_protocol   = htons(ETH_P_ALL), 
    };
    
    //LOG_NORMAL("this is in %s - %d",__func__,__LINE__);
    if(bind(fd,(struct sockaddr *)&toaddr,sizeof(toaddr)) < 0)
    {
        LOG_ERROR("bind fail");
        goto out_close;
    }

    if(prog != NULL && setsockopt(fd,SOL_SOCKET,SO_ATTACH_FILTER,prog,sizeof(struct sock_fprog)) < 0)
    {
        LOG_ERROR("set socket option fail");
        goto out_close;
    }

    fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_NONBLOCK);

    struct uloop_fd *h = calloc(1,sizeof(struct uloop_fd));
    h->fd = fd;
    h->cb = rawsock_recv;
    if(uloop_fd_add(h, ULOOP_READ) < 0)
    {
        LOG_ERROR("add rawsock fd into epoll fail");
        goto out_free;
    }
    
    return h;

out_free:
    free(h);
out_close:
    LOG_ERROR("%m");
    close(fd);
    return NULL;
}

