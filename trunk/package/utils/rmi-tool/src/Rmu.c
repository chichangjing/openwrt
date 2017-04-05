#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <linux/un.h>
#include <linux/types.h>
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include "Rmu.h"

char rmi_buf[BUF_LEN];

u_int8_t P_GE_Z20208D[PORTS_MV6097]={9, 10, 7, 8, 5, 6, 3, 4, 2, 0, 1};
//                             PORT: 0   1  2  3  4  5  6  7  8  9  10
u_int8_t L_GE_Z20208D[PORTS_MV6097]={9, 10, 8, 6, 7, 4, 5, 2, 3, 0, 1};
//                            LOGIC: 0   1  2  3  4  5  6  7  8  9  10

static u_int32_t sock;
static struct ifreq ifreq;
static pthread_mutex_t mutex;
static u_int8_t seq_num;
static Rmi_mac loacal_mac = {
        {0x0c, 0xa4, 0x2a, 0x00, 0x00, 0x00},
};

static Rmi_mac mv_multicast = {
        .addr = {0x01,0x50, 0x43, 0x00, 0x00, 0x00},
};

void buf_lock(void){
    pthread_mutex_lock(&mutex);
}

void buf_unlock(void){
    pthread_mutex_unlock(&mutex);
}

static struct sock_filter rmi_src_filter[] = {
        { 0x20, 0, 0, 0x00000002 },
        { 0x15, 0, 3, 0x2a000000 },
        { 0x28, 0, 0, 0x00000000 },
        { 0x15, 0, 1, 0x00000ca4 },
        { 0x6,  0, 0, 0x00040000 },
        { 0x6,  0, 0, 0x00000000 },
};

void buf_hex(char * buf, int32_t len){
    int32_t id;
    for(id=0;id<len; id++){
        printf("%02x", buf[id]);
        if(id && 15==(id%16)){
            printf("\n");
            continue;
        }
        if(id && 7==(id%8)){
            printf("    ");
            continue;
        }
        printf(" ");
    }
    printf("\n");
}

int32_t rmi_req_format(Rmi_req * req, Rmi_req_bpu * bpu){
    buf_lock();
    memset(rmi_buf, 0, BUF_LEN);

    memcpy(&bpu->da, &mv_multicast, ETH_ALEN);
    memcpy(&bpu->sa, &loacal_mac, ETH_ALEN);
    bpu->eth_type=htons(RMI_TYPE);

    bpu->tag[0]=(u_int8_t)(0x40|req->dev);
    bpu->tag[1]=(u_int8_t)(0xfa);
    bpu->tag[2]=(u_int8_t)(PRI_MV6097*0x20+0x0f);
    bpu->tag[3]=(u_int8_t)(seq_num);

    bpu->req_format=htons(req->req_format);
    bpu->res=0x0000;
    bpu->req_code=htons(req->req_code);
    bpu->req_data=htons(req->req_data);
    bpu->pad=htons(req->pad);
    return 0;
}

int32_t rmi_pkt_send(char * data, u_int32_t len){
    int32_t sendlen;
    struct sockaddr_ll sockaddr_ll = {
        .sll_family = PF_PACKET,
        .sll_protocol = htons(ETH_P_ALL),
        .sll_ifindex = ifreq.ifr_ifindex,
        .sll_pkttype = PACKET_MULTICAST,
        .sll_halen = ETH_ALEN,
    };

    memcpy(&(sockaddr_ll.sll_addr), data, ETH_ALEN);
    if(len!=(sendlen = sendto(sock, data, len, MSG_DONTWAIT, \
            (struct sockaddr *)&sockaddr_ll, sizeof(struct sockaddr_ll)))){
        LOG_ERROR("Rmi packet send error: %d error: %d!\n", sendlen, errno);
        return RMI_ERR_PACKET_SEND;
    }
    //buf_hex(data, len);
    return 0;
}

int32_t rmi_pkt_rcv(char * data, u_int32_t len){
    int32_t rcv;
    Rmi_req_bpu * rmi_req_bpu;
    do{
        memset(data, 0, len);
        struct sockaddr_ll sockaddr_ll;
        socklen_t sockaddr_ll_len = sizeof(struct sockaddr_ll);
        if((rcv = recvfrom(sock, data, len, 0, (struct sockaddr *)&sockaddr_ll, &sockaddr_ll_len)) <= 0){
            LOG_ERROR("Rmi packet recive error: %d error:%d!\n", rcv, errno);
            return RMI_ERR_PACKET_RECV;
            }
        rmi_req_bpu = (Rmi_req_bpu * )(data);
    }while(!(0x00==rmi_req_bpu->tag[1] && 0xff==rmi_req_bpu->tag[2] && seq_num==rmi_req_bpu->tag[3]
         && RMI_TYPE==ntohs(rmi_req_bpu->eth_type)));
    //buf_hex(data, rcv);
        return 0;
}

int32_t rmi_sock_init(char * if_name){
    //local_mac_init("eth0");
    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0){
        LOG_ERROR("Socket error!\n");
        return -1;
    }
    strcpy(ifreq.ifr_name, if_name);
    if(ioctl(sock, SIOCGIFINDEX, &ifreq)){
        LOG_ERROR("Get %s index error!\n", if_name);
        return RMI_ERR_SOCKET;
    }
    struct sockaddr_ll sockaddr_ll={
        .sll_family  = PF_PACKET,
        .sll_protocol= htons(ETH_P_ALL),
        .sll_ifindex = ifreq.ifr_ifindex,
    };

    if(bind(sock, (struct sockaddr *)&sockaddr_ll, sizeof(sockaddr_ll))){
        LOG_ERROR("Socket bind error: %d %d!\n", errno, sockaddr_ll.sll_ifindex);
        return -1;
    }

    struct sock_fprog fprog = {
        .len = sizeof(rmi_src_filter)/sizeof(rmi_src_filter[0]),
        .filter = rmi_src_filter,
    };

    int32_t nZero=0;
    struct timeval timeout={0, 50000};
    int32_t buf_len=2*1000*1000;
    if(setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &fprog, sizeof(fprog)) < 0
            || setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&nZero,sizeof(nZero))<0
            || setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&nZero,sizeof(nZero))<0
            || setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval))<0
            || setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(struct timeval))<0
            || setsockopt(sock, SOL_SOCKET, SO_RCVBUFFORCE, (char *)&buf_len,sizeof(int32_t))<0
            ){
        LOG_ERROR("Setsockopt error!\n");
        close(sock);
        return RMI_ERR_SOCKET;
#if 0
    }else if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0){
        LOG_ERROR("Fcntl error!\n");
        close(sock);
        return -1;
#endif
    }else{
#if 0
        event_handler.fd = sock;
        event_handler.cb = rmi_pkt_rcv;
        if(!(uloop_fd_add(&event_handler,ULOOP_READ)))
#endif
            return 0;
    }
    pthread_mutex_lock(&mutex);
    return 0;
}

int32_t rmi_id_init(u_int32_t __seq_num){
    if(__seq_num>0xff)
        return -1;
    seq_num =(uint8_t)(__seq_num);
    return 0;
}

char * resolve_u32(int32_t _count){
    static char resolve_str[32];
    memset(resolve_str, 0, sizeof(resolve_str));
    sprintf(resolve_str, "%u", _count);
    int32_t radix = strlen(resolve_str);
    radix = (radix>3) ? (radix/3-(radix%3 ? 0:1)):0;
    char * tail=&resolve_str[strlen(resolve_str)];
    for(;radix>0;){
        tail-=3;
        tail[2+radix] = tail[2];
        tail[1+radix] = tail[1];
        tail[0+radix] = tail[0];
        tail[--radix]=',';
    }
    return resolve_str;
}

char * resolve_u64(int64_t _count){
    static char resolve_str[32];
    memset(resolve_str, 0, sizeof(resolve_str));
    sprintf(resolve_str, "%llu", _count);
    int32_t radix = strlen(resolve_str);
    radix = (radix>3) ? (radix/3-(radix%3 ? 0:1)):0;
    char * tail=&resolve_str[strlen(resolve_str)];
    for(;radix>0;){
        tail-=3;
        tail[2+radix] = tail[2];
        tail[1+radix] = tail[1];
        tail[0+radix] = tail[0];
        tail[--radix]=',';
    }
    return resolve_str;
}
