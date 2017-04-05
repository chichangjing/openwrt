
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

#include "libnetlink.h"

extern struct rtnl_handle rth_state;

static int _br_set_state(struct rtnl_handle *rth, unsigned ifindex, __u8 state)
{
	struct {
		struct nlmsghdr n;
		struct ifinfomsg ifi;
		char buf[256];
	} req;

	memset(&req, 0, sizeof(req));

	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_REPLACE;
	req.n.nlmsg_type = RTM_SETLINK;
	req.ifi.ifi_family = AF_BRIDGE;
	req.ifi.ifi_index = ifindex;

	addattr32(&req.n, sizeof(req.buf), IFLA_PROTINFO, state);

	return rtnl_talk(rth, &req.n, 0, 0, NULL, NULL, NULL);
}

int br_set_state(int ifindex, int brstate)
{
	int err = _br_set_state(&rth_state, ifindex, brstate);
	if (err < 0) {
		fprintf(stderr, "Couldn't set bridge state, ifindex %d, state %d\n", ifindex, brstate);
		return -1;
	}
	return 0;
}

