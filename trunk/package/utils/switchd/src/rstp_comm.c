
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>
#include <net/if.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#include "rstp_comm.h"
#include "log.h"

CLIENT_SIDE_FUNCTION(enable_rstp)
CLIENT_SIDE_FUNCTION(get_bridge_state)
CLIENT_SIDE_FUNCTION(set_bridge_config)
CLIENT_SIDE_FUNCTION(get_port_state)
CLIENT_SIDE_FUNCTION(set_port_config)

struct command {
	int nargs;
	int optargs;
	const char *name;
	int (*func) (int argc, char *const *argv);
	const char *help;
};
static int set_bridge_cfg_value(unsigned long value, unsigned long val_mask)
{
	UID_STP_CFG_T uid_cfg;
	char *val_name;
	int rc;

	uid_cfg.field_mask = val_mask;
	switch (val_mask) {
	case BR_CFG_STATE:
		uid_cfg.stp_enabled = value;
		val_name = "state";
		break;
	case BR_CFG_PRIO:
		uid_cfg.bridge_priority = value;
		val_name = "priority";
		break;
	case BR_CFG_AGE:
		uid_cfg.max_age = value;
		val_name = "max_age";
		break;
	case BR_CFG_HELLO:
		uid_cfg.hello_time = value;
		val_name = "hello_time";
		break;
	case BR_CFG_DELAY:
		uid_cfg.forward_delay = value;
		val_name = "forward_delay";
		break;
	case BR_CFG_FORCE_VER:
		uid_cfg.force_version = value;
		val_name = "force_version";
		break;
	case BR_CFG_AGE_MODE:
	case BR_CFG_AGE_TIME:
	default:
		printf("Invalid value mask 0X%lx\n", val_mask);
		return -1;
		break;
	}

	rc = CTL_set_bridge_config(&uid_cfg);
	if (0 != rc) {
		printf("Can't change rstp bridge %s\n", val_name);
		return -1;
	}
	return 0;
}

#define BitmapClear(BitmapPtr) \
            { (BitmapPtr)->part0 = 0; }

static int set_port_cfg_value(int port_index, unsigned long value, unsigned long val_mask)
{
	UID_STP_PORT_CFG_T uid_cfg;
	int rc;
	char *val_name;

	BitmapClear(&uid_cfg.port_bmp);
	uid_cfg.field_mask = val_mask;
	switch (val_mask) {
	case PT_CFG_MCHECK:
		val_name = "mcheck";
		break;
	case PT_CFG_COST:
		uid_cfg.admin_port_path_cost = value;
		val_name = "path cost";
		break;
	case PT_CFG_PRIO:
		uid_cfg.port_priority = value;
		val_name = "priority";
		break;
	case PT_CFG_P2P:
		uid_cfg.admin_point2point = (ADMIN_P2P_T) value;
		val_name = "p2p flag";
		break;
	case PT_CFG_EDGE:
		uid_cfg.admin_edge = value;
		val_name = "adminEdge";
		break;
	case PT_CFG_NON_STP:
		uid_cfg.admin_non_stp = value;
		val_name = "adminNonStp";
		break;
#ifdef STP_DBG
	case PT_CFG_DBG_SKIP_TX:
		uid_cfg.skip_tx = value;
		val_name = "skip tx";
		break;
	case PT_CFG_DBG_SKIP_RX:
		uid_cfg.skip_rx = value;
		val_name = "skip rx";
		break;
#endif
	case PT_CFG_STATE:
	default:
		printf("Invalid value mask 0X%lx\n", val_mask);
		return -1;
	}

	rc = CTL_set_port_config(port_index, &uid_cfg);
	if (0 != rc) {
		printf("can't change rstp port[s] %s\n", val_name);
		return -1;
	}
	return 0;
}


int cmd_show_bridge(UID_STP_STATE_T* uid_state, UID_STP_CFG_T* uid_cfg)
{
	int r;
	
	r = CTL_get_bridge_state(uid_cfg, uid_state);
	if (r) {
		fprintf(stderr, "Failed to get bridge state\n");	
		return -1;
	}
	return 0;
}

int cmd_show_portdetail(int port_index, UID_STP_PORT_CFG_T* portCfg, UID_STP_PORT_STATE_T*  portState)
{
	int r;
	
	r = CTL_get_port_state(port_index, portCfg, portState);
	if (r) {
		fprintf(stderr, "Failed to get port%d state\n", port_index);
		return -1;
	}
	return 0;
}


int cmd_rstp(int enable)
{
	int r;
	r = CTL_enable_rstp(enable);
	if (r < 0) {
		fprintf(stderr, "Failed to %s rstp\n", (enable == 1)? "enable":"disable");
		return -1;
	}

	return 0;
}
#if 0
static int cmd_set_bridge_state(int argc, char *const *argv)
{
	return set_bridge_cfg_value(getyesno(argv[1], "on", "off"),BR_CFG_STATE);
}
#endif

int cmd_set_bridge_prio(unsigned long value)
{
	return set_bridge_cfg_value(value, BR_CFG_PRIO);
}

int cmd_set_bridge_hello(unsigned long value)
{
	return set_bridge_cfg_value(value, BR_CFG_HELLO);
}

int cmd_set_bridge_maxage(unsigned long value)
{
	return set_bridge_cfg_value(value, BR_CFG_AGE);
}

int cmd_set_bridge_fdelay(unsigned long value)
{
	return set_bridge_cfg_value(value, BR_CFG_DELAY);
}


int cmd_set_port_prio(int port_index, unsigned long value)
{
	return set_port_cfg_value(port_index, value, PT_CFG_PRIO);
}

int cmd_set_port_pathcost(int port_index, unsigned long value)
{
	return set_port_cfg_value(port_index, value, PT_CFG_COST);
}

int cmd_set_port_edge(int port_index, unsigned long  value)
{
	return set_port_cfg_value(port_index, value, PT_CFG_EDGE);
}

int cmd_set_port_nonstp(int port_index, unsigned long value)
{
	return set_port_cfg_value(port_index, value, PT_CFG_NON_STP);
}

static int ctl_client_fd = -1;
#define RSTP_CTL_PATH "/var/run/"

int ctl_rstp_init(void)
{
    int s;
    struct sockaddr_un un;

    if((s = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) 
    {
        fprintf(stderr,"couldn't open unix socket");
        return -1;
    }

    struct timeval t = {5,0};
    if(setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char *)&t,sizeof(struct timeval)))
    {
        fprintf(stderr,"couldn't set recv timeout error");
        return -1;
    }

    memset(&un,0,sizeof(struct sockaddr_un));
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path + 1, "%s%s", RSTP_CTL_PATH,"rstpd");
    unlink(un.sun_path);
    
    if (bind(s, (struct sockaddr *)&un, sizeof(struct sockaddr_un)) != 0) 
    {
        fprintf(stderr,"couldn't bind socket: %m");
        close(s);
        return -1;
    }

    ctl_client_fd = s;

    return 0;

}

void ctl_client_cleanup(void)
{
    if (ctl_client_fd >= 0) {
        close(ctl_client_fd);
        ctl_client_fd = -1;
    }
}

int ctl_rstp_send(int cmd, void *inbuf, int lin, void *outbuf, int *lout, int *res)
{
    struct ctl_msg_hdr mhdr;
    struct msghdr msg;
    struct iovec iov[2];
    int l;

    struct sockaddr_un un;
    set_socket_address(&un, CTL_SERVER_SOCK_NAME);
    msg.msg_name = &un;
 //   msg.msg_name = NULL;
    msg.msg_namelen = sizeof(struct sockaddr_un);
    msg.msg_iov = iov;
    msg.msg_iovlen = 2;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;

    mhdr.cmd = cmd;
    mhdr.lin = lin;
    mhdr.lout = lout != NULL ? *lout : 0;
    iov[0].iov_base = &mhdr;
    iov[0].iov_len = sizeof(mhdr);
    iov[1].iov_base = (void *)inbuf;
    iov[1].iov_len = lin;

    l = sendmsg(ctl_client_fd, &msg, 0);
    if (l < 0) {
        LOG_ERROR("error sending message to server: %m\n");
        return -1;
    }
    if (l != sizeof(mhdr) + lin) {
        LOG_ERROR("error sending message to server: Partial write\n");
        return -1;
    }

    iov[1].iov_base = outbuf;
    iov[1].iov_len = lout != NULL ? *lout : 0;

    {
        struct pollfd pfd;
        int timeout = 5000; /* 5 s */
        int r;

        pfd.fd = ctl_client_fd;
        pfd.events = POLLIN;
        do {
            r = poll(&pfd, 1, timeout);
            if (r == 0) {
                LOG_ERROR("error getting message from server: Timeout\n");
                return -1;
            }
            if (r < 0) {
                LOG_ERROR("error getting message from server: poll error: %m\n");
                return -1;
            }
        } while ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL | POLLIN)) == 0);

        l = recvmsg(ctl_client_fd, &msg, 0);
        if (l < 0) {
            LOG_ERROR("error getting message from server: %m");
            return -1;
        }
        if (l < sizeof(mhdr) || l != sizeof(mhdr) + mhdr.lout || mhdr.cmd != cmd) {
            LOG_ERROR("error getting message from server: Bad format\n");
            return -1;
        }
    }
    
    if (lout)
        *lout = mhdr.lout;
    if (res)
        *res = mhdr.res;

    return 0;
}

