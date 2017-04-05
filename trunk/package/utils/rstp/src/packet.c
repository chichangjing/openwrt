
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

#include <stp_bpdu.h>
#include "log.h"
#include "epoll_loop.h"
#include "rstp_if.h"

static struct epoll_event_handler event_handler;

static struct sock_filter packet_filter[] = {
	{ 0x28, 0, 0, 0x0000000c },
	{ 0x25, 3, 0, 0x000005dc },
	{ 0x30, 0, 0, 0x0000000e },
	{ 0x15, 0, 1, 0x00000042 },
	{ 0x6, 0, 0, 0x00000060 },
	{ 0x6, 0, 0, 0x00000000 },
};

void pdu_pkt_send(int ifindex, const unsigned char *data, int len)
{
	int sendLen;
	unsigned short len8023;
	BPDU_T *bpdu = (BPDU_T *) (data + sizeof(MAC_HEADER_T));
	ETH_HEADER_T *ethHdr = (ETH_HEADER_T *) (data + sizeof(MAC_HEADER_T));
	
	struct sockaddr_ll sl = {
		.sll_family = AF_PACKET,
		.sll_protocol = htons(ETH_P_802_2),
		.sll_ifindex = ifindex,
		.sll_halen = ETH_ALEN,
	};

	memcpy(&sl.sll_addr, data, ETH_ALEN);

#if 1
	len8023 = ((unsigned short)ethHdr->len8023[0] << 8) | (unsigned short)ethHdr->len8023[1];
	log_debug("\033[1;36mTx bpdu-type(%s) to port%02d\033[0m", 
			(bpdu->hdr.bpdu_type == BPDU_CONFIG_TYPE)? "CNF": \
			(bpdu->hdr.bpdu_type == BPDU_RSTP)? "RST": \
			(bpdu->hdr.bpdu_type == BPDU_TOPO_CHANGE_TYPE)? "TCN":"***", sl.sll_ifindex);
	dump_hex((void *)data, len);
#endif
	
	sendLen = sendto(event_handler.fd, data, len, MSG_DONTWAIT, (struct sockaddr *)&sl, sizeof(sl));
	if (sendLen < 0) {
		log_err("pdu_pkt_send (ifindex=%d) failed. errno=%d\n", ifindex, errno);
	} else if (sendLen != len) {
		log_err("pdu_pkt_send (ifindex=%d) failed. sendLen != len\n", ifindex);
	}
}

static void pdu_pkt_rcv(uint32_t events, struct epoll_event_handler *h)
{
	int cc;
	unsigned char buf[2048];
	struct sockaddr_ll sl;
	socklen_t salen = sizeof(struct sockaddr_ll);
	BPDU_T *bpdu = (BPDU_T *) (buf + sizeof(MAC_HEADER_T));
	
	if((cc = recvfrom(h->fd, &buf, sizeof(buf), 0, (struct sockaddr *)&sl, &salen)) <= 0) {
		log_err("recvfrom failed");
		return;
	}

	/* To be add */
	br_bpdu_rcv(sl.sll_ifindex, buf, cc);
}

int pdu_sock_init(void)
{
	int s;
	struct sock_fprog prog = {
		.len = sizeof(packet_filter) / sizeof(packet_filter[0]),
		.filter = packet_filter,
	};

	if((s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		return -1;
	}

	if(setsockopt(s, SOL_SOCKET, SO_ATTACH_FILTER, &prog, sizeof(prog)) < 0) 
		log_err("setsockopt packet filter failed");
	else if(fcntl(s, F_SETFL, O_NONBLOCK) < 0)
		log_err("fcntl set nonblock failed");
	else {
		event_handler.fd = s;
		event_handler.handler = pdu_pkt_rcv;
		if (add_epoll(&event_handler) == 0)
			return 0;
	}

	close(s);

	return -1;
}



