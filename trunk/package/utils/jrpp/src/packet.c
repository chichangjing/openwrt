#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <syslog.h>
#include <signal.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <linux/un.h>
#include <linux/types.h>
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <net/if.h>

#include "log.h"
#include "rpp_pdu.h"
#include "jrpp_if.h"
#include "jrppd.h"

static struct uloop_fd event_handler;

#if 1
/* * tcpdump ether dst 0d:a4:2a:00:00:05 -dd * */
static struct sock_filter pdu_filter[] = {
	{ 0x20, 0, 0, 0x00000002 },
	{ 0x15, 0, 3, 0x2a000005 },
	{ 0x28, 0, 0, 0x00000000 },
	{ 0x15, 0, 1, 0x00000da4 },
	{ 0x6, 0, 0, 0x00040000 },
	{ 0x6, 0, 0, 0x00000000 },
};
#else
/* * tcpdump ether dst 01:80:c2:00:00:00 -dd * */
static struct sock_filter pdu_filter[] = {
	{ 0x20, 0, 0, 0x00000002 },
	{ 0x15, 0, 3, 0xc2000000 },
	{ 0x28, 0, 0, 0x00000000 },
	{ 0x15, 0, 1, 0x00000180 },
	{ 0x6, 0, 0, 0x00040000 },
	{ 0x6, 0, 0, 0x00000000 },
};
#endif

void pdu_pkt_send(int ifindex, const unsigned char *data, int len)
{
	int sendLen;
	
	struct sockaddr_ll sl = {
		.sll_family = AF_PACKET,
		.sll_protocol = htons(ETH_P_ALL),
		.sll_ifindex = 4096,
		.sll_pkttype = PACKET_MULTICAST,
		.sll_halen = ETH_ALEN,
	};

	memcpy(&sl.sll_addr, data, ETH_ALEN);

#if 0
	RPP_HEADER_T *pduHdr = (RPP_HEADER_T *)data;
    unsigned short len8023 = ntohs(*(unsigned short *)pduHdr->len8023);
	log_debug("\033[1;36mTx packet to port%2d, len=%d, len8023=%d\033[0m", ifindex, len, len8023);
	dump_hex((void *)data, len);
#endif
	
	sendLen = sendto(event_handler.fd, data, len, MSG_DONTWAIT, (struct sockaddr *)&sl, sizeof(sl));
	if (sendLen < 0) {
		LOG_ERROR("pdu_pkt_send (ifindex=%d) failed. errno=%d\n", ifindex, errno);
	} else if (sendLen != len) {
		LOG_ERROR("pdu_pkt_send (ifindex=%d) failed. sendLen != len\n", ifindex);
	}
}

static void pdu_pkt_rcv(struct uloop_fd *h,uint32_t events)
{
	int cc;
	unsigned char buf[2048];
	struct sockaddr_ll sl;
	socklen_t salen = sizeof sl;
	
	if((cc = recvfrom(h->fd, &buf, sizeof(buf), 0, (struct sockaddr *)&sl, &salen)) <= 0) {
		LOG_ERROR("recvfrom failed");
		return;
	}

#if 0
	log_debug("\033[1;35mRx packet on port%2d from %02x:%02x:%02x:%02x:%02x:%02x\033[0m", 
			sl.sll_ifindex, 
			sl.sll_addr[0], sl.sll_addr[1], sl.sll_addr[2],
			sl.sll_addr[3], sl.sll_addr[4], sl.sll_addr[5]);
	dump_hex(buf, cc);
#endif
	/* To be add */
	br_pdu_rcv(buf, cc);
}

int pdu_sock_init(void)
{
	int s;
	struct sock_fprog prog = {
		.len = sizeof(pdu_filter) / sizeof(pdu_filter[0]),
		.filter = pdu_filter,
	};

	if((s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		return -1;
	}

    struct sockaddr_ll ph_addr;
    struct ifreq ifr;

    strcpy(ifr.ifr_name,"eth0");
    if(ioctl(s,SIOCGIFINDEX,&ifr))
    {
        LOG_ERROR("ioctl error!\n");
        return -1;
    }
    ph_addr.sll_ifindex = ifr.ifr_ifindex;
    ph_addr.sll_family  = PF_PACKET;
    ph_addr.sll_protocol= htons(ETH_P_ALL);

    if(bind(s,(struct sockaddr *)&ph_addr,sizeof(ph_addr)))
    {
        LOG_ERROR("bind error = %d!\n",errno);
        return -1;
    }

	if(setsockopt(s, SOL_SOCKET, SO_ATTACH_FILTER, &prog, sizeof(prog)) < 0) 
		LOG_ERROR("setsockopt packet filter failed");
	else if (fcntl(s, F_SETFL, O_NONBLOCK) < 0)
		LOG_ERROR("fcntl set nonblock failed");
	else {
		event_handler.fd = s;
		event_handler.cb = pdu_pkt_rcv;
		if (uloop_fd_add(&event_handler,ULOOP_READ) == 0)
			return 0;
	}

	close(s);

	return -1;
}



