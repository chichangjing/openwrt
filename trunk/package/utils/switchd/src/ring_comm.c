#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <limits.h>
#include <dirent.h>
#include <net/if.h>

#include "ring_comm.h"
#include "log.h"

CLIENT_SIDE_FUNCTION(enable_jrpp)
CLIENT_SIDE_FUNCTION(add_ring)
CLIENT_SIDE_FUNCTION(del_ring)
CLIENT_SIDE_FUNCTION(add_ringport)
CLIENT_SIDE_FUNCTION(del_ringport)
CLIENT_SIDE_FUNCTION(enable_ring)
CLIENT_SIDE_FUNCTION(set_ring_config)
CLIENT_SIDE_FUNCTION(get_ring_state)
CLIENT_SIDE_FUNCTION(set_debug_level)
CLIENT_SIDE_FUNCTION(get_ring_topo)
CLIENT_SIDE_FUNCTION(update_ports)
CLIENT_SIDE_FUNCTION(set_topo)

// 字符串转无符号整型
unsigned int getuint(const char *s)
{
	char *end;
	long l;
	l = strtoul(s, &end, 0);
	if (*s == 0 || *end != 0 || l > INT_MAX) {
		fprintf(stderr, "Invalid unsigned int arg %s\n", s);
		exit(1);
	}
	return l;
}

int cmd_jrpp(int value)
{
	int r;

	r = CTL_enable_jrpp(value);
	if (r < 0) 
    {
		fprintf(stderr, "Failed to %s jrpp.\n", (value == 1)? "enable":"disable");
		return -1;
	}

	return 0;
}

int cmd_set_topo(int ring_id, int state)
{
    int rc;
 /*   int ring_id = atoi(argv[1]);
    int state;

    if(!strcmp("enable",argv[2]) || !strcmp("on",argv[2]))
    {
        state = RING_ENABLED;
    }
    else if(!strcmp("disable",argv[2]) || !strcmp("off",argv[2]))
    {
        state = RING_DISABLED;
    }
    else
    {
        fprintf(stderr, "invalid arguement\n");
        return -1;
    }
*/
    rc = CTL_set_topo(ring_id, state);
    if (0 != rc) {
        fprintf(stderr, "set ring#%d topo fail\n", ring_id);
        return -1;
    }

    printf("set ring#%d topo %s OK\n",ring_id,(state == RING_ENABLED)?"enable":"disable");

    return 0;
}


int cmd_show_topo(int ring_id, RPP_RING_TOPO_T* topo)
{
	int rc;
    if(ring_id > MAX_RING_ID)
    {
        fprintf(stderr,"Invalid arguement\n");
        return -1;
    }

    rc = CTL_get_ring_topo(ring_id, topo);
	if (rc) 
    {
		fprintf(stderr, "can't get topo for ring\n");
		return -1;
	}

    return 0;
}

int cmd_show_ring(int ring_id, RPP_RING_STATE_T state)
{
	int rc;
    if(ring_id > MAX_RING_ID)
    {
        fprintf(stderr,"Invalid arguement\n");
        return -1;
    }
	
//    memset(&state,0,sizeof(RPP_RING_STATE_T));
	rc = CTL_get_ring_state(ring_id, &state);
	if (rc) 
    {
		fprintf(stderr, "can't get state for ring\n");
		return -1;
	}
#if 0
    printf("			====================================================\n");
	printf("				Rapid Ring Protect Feature is [ %s ]    \n",(state.rpp_state == RPP_ENABLED)?"Enable":"Disable");
    printf("			====================================================\n\n");
    printf("Ring#%d information...\n",ring_id);
    printf("    RingEnable  : %s\n",(state.ring_state == RING_ENABLED)?"Enable":"Disable");

    if(state.ring_state == RING_ENABLED)
    {
        printf("    RingMode    : %s\n",(state.rpp_mode == RPP_ENHANCED)?"Enhanced":"Compatible");
        printf("    RingState   : %s\n",(state.ring_status == RPP_HEALTH)?"Health":"Fault");
        printf("    NodeRole    : %s\n",(state.node_role == NODE_TYPE_MASTER)?"Master":"Transit");
        printf("    NodeId      : %d-%02x:%02x:%02x:%02x:%02x:%02x\n",state.node_id.prio,\
               state.node_id.addr[0],state.node_id.addr[1],state.node_id.addr[2],\
               state.node_id.addr[3],state.node_id.addr[4],state.node_id.addr[5]);
        printf("    Master      : %d-%02x:%02x:%02x:%02x:%02x:%02x\n",state.master_id.prio,\
                   state.master_id.addr[0],state.master_id.addr[1],state.master_id.addr[2],\
                   state.master_id.addr[3],state.master_id.addr[4],state.master_id.addr[5]);
        printf("    NodeMachine : %s\n",state.m_node_st);
        printf("    HelloSeq    : %d\n",state.hello_seq);
    }

    RPP_PORT_STATE_T *port = &state.primary;
    printf("\n");
    printf("    =========================================================================================\n");
    printf("    RingPort    PortRole    Link      STP     AuthMachine     BallotMachine          Neighbor\n");
    printf("    =========================================================================================\n");
    if(port->port_no)
    {
        if(state.ring_state == RING_ENABLED)
        {
            printf("    Port%-4d    %s    %s    %s    %s        %s      [%02x:%02x:%02x:%02x:%02x:%02x]\n",\
                   port->port_no,(port->role == PRIMARY_PORT)?"primary":"secondary",\
                   (port->link_state == LINK_UP)?"Up":"Down",(port->dot1d_state == FORWARDING)?"Forwarding":"Blocking",\
                   port->m_auth_st,port->m_ballot_st,port->neigbor_mac[0],port->neigbor_mac[1],port->neigbor_mac[2],\
                   port->neigbor_mac[3],port->neigbor_mac[4],port->neigbor_mac[5]);
        }
        else
        {
            printf("    Port%-4d    %s\n",port->port_no,(port->role == PRIMARY_PORT)?"primary":"secondary");
        }
    }

    port = &state.secondary;
    if(port->port_no)
    {
        if(state.ring_state == RING_ENABLED)
        {
            printf("    Port%-4d    %s    %s    %s    %s        %s      [%02x:%02x:%02x:%02x:%02x:%02x]\n",\
                   port->port_no,(port->role == PRIMARY_PORT)?"primary":"secondary",\
                   (port->link_state == LINK_UP)?"Up":"Down",(port->dot1d_state == FORWARDING)?"Forwarding":"Blocking",\
                   port->m_auth_st,port->m_ballot_st,port->neigbor_mac[0],port->neigbor_mac[1],port->neigbor_mac[2],\
                   port->neigbor_mac[3],port->neigbor_mac[4],port->neigbor_mac[5]);
        }
        else
        {
            printf("    Port%-4d    %s\n",port->port_no,(port->role == PRIMARY_PORT)?"primary":"secondary");
        }
    }
#endif
	return 0;	
}

int cmd_add_ring(int ring_id)
{
	int rc;
	
	rc = CTL_add_ring(ring_id);
	if (rc) 
    {
		fprintf(stderr, "add ring#%d failed\n", ring_id);
		return -1;
	}
	return 0;
}

int cmd_del_ring(int ring_id)
{
	int rc;
	
	rc = CTL_del_ring(ring_id);
	if (rc) 
    {
		fprintf(stderr, "delete ring#%d failed\n", ring_id);
		return -1;
	}
	return 0;
}

int cmd_add_ringport(int ring_id, int primary_port, int secondary_port)
{
	int rc;
	
	rc = CTL_add_ringport(ring_id, primary_port, secondary_port);
	if (rc) 
    {
		fprintf(stderr, "add ringport failed for ring#%d\n", ring_id);
		return -1;
	}
	return 0;
}

int cmd_del_ringport(int argc, char *const *argv)
{
	int rc;
	int ring_id = getuint(argv[1]);
	
	rc = CTL_del_ringport(ring_id);
	if (rc) 
    {
		fprintf(stderr, "del ringport failed for ring#%d\n", ring_id);
		return -1;
	}
	return 0;
}

int cmd_set_nodeprio(int ring_id, char *prio_val)
{
    int rc;
    unsigned char prio;

    if(!strcmp("low",prio_val))
        prio = 2;
    else if(!strcmp("medium",prio_val))
        prio = 1;
    else if(!strcmp("high",prio_val))
        prio = 0;
    else
    {
        fprintf(stderr,"invalid arguement: [%s]!\n",prio_val);
        return -1;
    }

	RPP_RING_CFG_T ring_cfg = {
        .node_priority      = prio,
        .field_mask         = RING_CFG_PRIO
    };

	rc = CTL_set_ring_config(ring_id, &ring_cfg);
    if(rc)
    {
		fprintf(stderr, "set config failed for ring#%d\n", ring_id);
        return -1; 
    }
	return 0;
}

int cmd_set_ringmode(int ring_id, char *ring_mode)
{
    int rc;
	int enable;
	
    if(!strcmp("enable",ring_mode) || !strcmp("on",ring_mode))
    {
        enable = RING_ENABLED;
    }
    else if(!strcmp("disable",ring_mode) || !strcmp("off",ring_mode))
    {
        enable = RING_DISABLED;
    }
    else
    {
		fprintf(stderr, "invalid arguement\n");
        return -1;
    }

	rc = CTL_enable_ring(ring_id, enable);
	if (0 != rc) {
		fprintf(stderr, "set ring#%d mode fail\n", ring_id);
		return -1;
	}

	return 0;
}

int cmd_debuglevel(int argc, char *const *argv)
{
	return CTL_set_debug_level(getuint(argv[1]));
}

#define JRPP_CTL_PATH "/var/run/"
static int ctl_client_fd = -1;
int ctl_ring_init(void)
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
	sprintf(un.sun_path + 1, "%s%s", JRPP_CTL_PATH,"jrppd");
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

int ctl_ring_send(int cmd, void *inbuf, int lin, void *outbuf, int *lout, int *res)
{
	struct ctl_msg_hdr mhdr;
	struct msghdr msg;
	struct iovec iov[2];
	int l;

	struct sockaddr_un un;
	set_socket_address(&un, CTL_SERVER_SOCK_NAME);
	msg.msg_name = &un;
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

		fprintf(stderr,"error sending message to server: %m");
		return -1;
	}
	if (l != sizeof(mhdr) + lin) {
		fprintf(stderr,"error sending message to server: Partial write");
		return -1;
	}

	iov[1].iov_base = outbuf;
	iov[1].iov_len = lout != NULL ? *lout : 0;
    //sleep(3);

    l = recvmsg(ctl_client_fd, &msg, 0);
    if (l < 0) {
        if(errno == 11){
            return 0;
        }
            fprintf(stderr,"error getting message from server: %m");
            return -1;
    }
    if (l < sizeof(mhdr) || l != sizeof(mhdr) + mhdr.lout || mhdr.cmd != cmd) {
        fprintf(stderr,"error getting message from server: Bad format[%d,%d,%d]",l,mhdr.lout,mhdr.cmd);
        return -1;
    }
	
	if (lout)
		*lout = mhdr.lout;
	if (res)
		*res = mhdr.res;

	return 0;
}

