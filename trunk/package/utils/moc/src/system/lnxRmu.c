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

#include "lnxRmu.h"

#define BUFF_LEN 1024
static GT_UINT sock;
static struct ifreq ifreq;

/* * tcpdump ether src 00:00:00:00:00:00 and ether dst 00:00:00:00:00:01 -dd * */
static struct sock_filter sock_filter[] = {
        { 0x20, 0, 0, 0x00000008 },
        { 0x15, 0, 7, 0x00000000 },
        { 0x28, 0, 0, 0x00000006 },
        { 0x15, 0, 5, 0x00000000 },
        { 0x20, 0, 0, 0x00000002 },
        { 0x15, 0, 3, 0x00000001 },
        { 0x28, 0, 0, 0x00000000 },
        { 0x15, 0, 1, 0x00000000 },
        { 0x6, 0, 0, 0x00040000 },
        { 0x6, 0, 0, 0x00000000 },
};

static void buf_hex(GT_U8 * buf, GT_UINT len){
    GT_32 id;
    TTY_MSG("recived: %d \n", len);
    for(id=0; id<len; id++){
        TTY_MSG("%02x", 0xff & buf[id]);
        if(id && 15==(id%16)){
            TTY_MSG("\n");
            continue;
        }
        if(id && 7==(id%8)){
            TTY_MSG("    ");
            continue;
        }
        TTY_MSG(" ");
    }
    TTY_MSG("\n");
}


static GT_STATUS RmuPktSend(GT_U8 * data, GT_UINT len){
    GT_32 sendlen;
    DBG_MSG("%s called. \n", __func__);
    struct sockaddr_ll sockaddr_ll = {
        .sll_family = PF_PACKET,
        .sll_protocol = htons(ETH_P_ALL),
        .sll_ifindex = ifreq.ifr_ifindex,
        .sll_pkttype = PACKET_MULTICAST,
        .sll_halen = ETH_ALEN,
    };
    DBG_MSG("Send packet on socket. \n");
    if(len!=(sendlen = sendto(sock, data, len, MSG_DONTWAIT, \
            (struct sockaddr *)&sockaddr_ll, sizeof(struct sockaddr_ll)))){
        LOG_MSG("Rmi packet send error: %d/%d!\n", sendlen, errno);
        return PF_FAIL;
    }
    return PF_OK;
}

static GT_STATUS RmuPktRcv(GT_U8 ** data, GT_UINT * len){
    GT_32 rcv;
    DBG_MSG("%s called. \n", __func__);
    static char buff[BUFF_LEN];
    do{
        memset(buff, 0, BUFF_LEN);
        struct sockaddr_ll sockaddr_ll;
        socklen_t sockaddr_ll_len = sizeof(struct sockaddr_ll);
        DBG_MSG("Receive packet from socket. \n");
        if((rcv = recvfrom(sock, buff, BUFF_LEN, 0, (struct sockaddr *)&sockaddr_ll, &sockaddr_ll_len)) <= 0){
            LOG_MSG("Rmi packet receive error: %d/%d!\n", rcv, errno);
            return PF_FAIL;
            }
    }while(!(0x08==buff[16] && 0x00==buff[17]));
    *len = rcv;
    *data = buff;
    return PF_OK;
}

GT_STATUS lnxRmuInit(char * ifName){
    DBG_MSG("Socket Initialization. \n");
    if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0){
        LOG_MSG("Socket error!\n");
        return PF_FAIL;
    }
    strcpy(ifreq.ifr_name, ifName);
    if(ioctl(sock, SIOCGIFINDEX, &ifreq)){
        LOG_MSG("Get %s index error!\n", ifName);
        return PF_FAIL;
    }
    struct sockaddr_ll sockaddr_ll={
        .sll_family  = PF_PACKET,
        .sll_protocol= htons(ETH_P_ALL),
        .sll_ifindex = ifreq.ifr_ifindex,
    };
    DBG_MSG("Socket bind. \n");
    if(bind(sock, (struct sockaddr *)&sockaddr_ll, sizeof(sockaddr_ll))){
        LOG_MSG("Socket bind error: %d/%d!\n", sockaddr_ll.sll_ifindex, errno);
        return PF_FAIL;
    }

    struct sock_fprog fprog = {
        .len = sizeof(sock_filter)/sizeof(sock_filter[0]),
        .filter = sock_filter,
    };

    GT_32 nZero=0;
    struct timeval timeout={0, 500000};
    GT_32 buf_len=2*1000*1000;
    DBG_MSG("Socket set option. \n");
    if(setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &fprog, sizeof(fprog)) < 0
            || setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&nZero,sizeof(nZero))<0
            || setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&nZero,sizeof(nZero))<0
            || setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval))<0
            //|| setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(struct timeval))<0
            || setsockopt(sock, SOL_SOCKET, SO_RCVBUFFORCE, (char *)&buf_len,sizeof(GT_32))<0
            ){
        LOG_MSG("Setsockopt error: %d!\n", errno);
        close(sock);
        return PF_FAIL;
    }
    return PF_OK;
}

GT_STATUS lnxRmuTxRx(GT_U8 * reqPkt, GT_UINT reqPktLen, GT_U8 ** rspPkt, GT_UINT * rspPktLen){
    GT_UINT retVal;
    DBG_MSG("%s called. \n", __func__);
    if(PF_OK != (retVal = RmuPktSend(reqPkt, reqPktLen))){
        LOG_MSG("Rmu packdt send failed! \n");
        return retVal;
    }
    if(PF_OK != (retVal = RmuPktRcv(rspPkt, rspPktLen))){
        LOG_MSG("Rmu packdt receive failed! \n");
        return retVal;
    }
    return PF_OK;
}
GT_BOOL lnxRmuHwAcess(GT_QD_DEV * dev, HW_DEV_REG_ACCESS * regList){
    GT_STATUS retVal;
    DBG_MSG("%s called. \n", __func__);
    if(GT_OK != (retVal = gtRmuRegAccess(dev, regList))){
        LOG_MSG("gtRmuRegAccess failed! \n");
        return GT_FALSE;
    }
    return GT_TRUE;
}

GT_BOOL lnxRmuReadMii(GT_QD_DEV * dev, unsigned int phyAddr, unsigned int miiReg, unsigned int * value){
    GT_STATUS retVal;
    DBG_MSG("%s called. \n", __func__);
    if(GT_OK != (retVal = gtRmuReadRegister(dev, phyAddr, miiReg, value))){
        LOG_MSG("gtRmuReadRegister failed! \n");
        return GT_FALSE;
    }
    return GT_TRUE;
}

GT_BOOL lnxRmuWriteMii(GT_QD_DEV * dev, unsigned int phyAddr, unsigned int miiReg, unsigned int value){
    GT_STATUS retVal;
    DBG_MSG("%s called. \n", __func__);
    if(GT_OK != (retVal = gtRmuWriteRegister(dev, phyAddr, miiReg, value))){
        LOG_MSG("gtRmuWriteRegister failed! \n");
        return GT_FALSE;
    }
    return GT_TRUE;
}
