
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_bridge.h>
#include <string.h>

#include "log.h"
#include "libnetlink.h"
#include "epoll_loop.h"
#include "rstp_if.h"

struct rtnl_handle rth;
struct rtnl_handle rth_state;
struct epoll_event_handler br_handler;

static int br_filter_message(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
{
	struct ifinfomsg *ifi = NLMSG_DATA(n);
	struct rtattr *tb[IFLA_MAX+1];
	int len = n->nlmsg_len;
	
	if (n->nlmsg_type == NLMSG_DONE)
		return 0;

	len -= NLMSG_LENGTH(sizeof(*ifi));
	if (len < 0)
		return -1;

    if (ifi->ifi_family != AF_BRIDGE && ifi->ifi_family != AF_UNSPEC)
      return 0;

    if (n->nlmsg_type != RTM_NEWLINK && n->nlmsg_type != RTM_DELLINK)
      return 0;

	parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), len);

    if (tb[IFLA_MASTER] && ifi->ifi_family != AF_BRIDGE)
       return 0;

	if (tb[IFLA_IFNAME] == NULL) 
	   return -1;

    if (ifi->ifi_index > 10) {
        log_debug ("ifindex = %d exceed max\n", ifi->ifi_index);
        return 0;
    }
	
	//log_info("rx: nlmsg_type:%d, %-6s, flags:0x%08x\n",  n->nlmsg_type, (const char*)RTA_DATA(tb[IFLA_IFNAME]), ifi->ifi_flags);
	
	br_notify(ifi->ifi_index, (n->nlmsg_type == RTM_NEWLINK), ifi->ifi_flags);

	return 0;
}

static void br_event_handler(uint32_t events, struct epoll_event_handler *h)
{
	if (rtnl_listen(&rth, br_filter_message, NULL) < 0) {
		fprintf(stderr, "Error on bridge monitoring socket\n");
		exit(-1);
	}
}

int init_bridge_ops(void)
{
	if (rtnl_open(&rth, ~RTMGRP_TC) < 0) {
		log_err("rtnl_open failed\n");
		return -1;
	}

	if (rtnl_open(&rth_state, 0) < 0) {
		log_err("rtnl_open failed for setting state\n");
		return -1;
	}
  
	if (rtnl_wilddump_request(&rth, PF_BRIDGE, RTM_GETLINK) < 0) {
		log_err("rtnl_wilddump_request failed\n");
		return -1;
	}

	if (rtnl_dump_filter(&rth, br_filter_message, stdout, NULL, NULL) < 0) {
		log_err("rtnl_dump_filter terminated\n");
		return -1;
	}

	if (fcntl(rth.fd, F_SETFL, O_NONBLOCK) < 0) {
		log_err("error: fcntl setting O_NONBLOCK, %m\n");
		return -1;
	}

	br_handler.fd = rth.fd;
	br_handler.arg = NULL;
	br_handler.handler = br_event_handler;

	if (add_epoll(&br_handler) < 0)
		return -1;

	return 0;
}

void br_get_configuration(void)
{
	if(rtnl_wilddump_request(&rth, PF_BRIDGE, RTM_GETLINK) < 0) {
		fprintf(stderr, "Cannot send dump request: %m\n");
	}
}


