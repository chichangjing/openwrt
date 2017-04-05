
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "log.h"
#include "base.h"
#include "jrppd.h"
#include "ctl_socket.h"
#include "ctl_server.h"

#define msg_buf_len 4096
unsigned char msg_inbuf[4096];
unsigned char msg_outbuf[4096];
struct uloop_fd ctl_handler;

int ctl_handle_message(int cmd, void *inbuf, int lin, void *outbuf, int *lout)
{
	switch (cmd) {
		SERVER_MESSAGE_CASE(enable_jrpp);
		SERVER_MESSAGE_CASE(add_ring);
		SERVER_MESSAGE_CASE(del_ring);
		SERVER_MESSAGE_CASE(add_ringport);
		SERVER_MESSAGE_CASE(del_ringport);
		SERVER_MESSAGE_CASE(enable_ring);
		SERVER_MESSAGE_CASE(set_ring_config);
		SERVER_MESSAGE_CASE(get_ring_state);
		SERVER_MESSAGE_CASE(set_debug_level);
		SERVER_MESSAGE_CASE(set_topo);
		SERVER_MESSAGE_CASE(update_ports);

    case CMD_CODE_get_ring_topo:
        return 0;
	default:
		LOG_ERROR("CTL: Unknown command %d", cmd);
		return -1;
	}
}

void ctl_rcv_handler(struct uloop_fd *p,uint32_t events)
{
	struct ctl_msg_hdr mhdr;
	struct msghdr msg;
	struct sockaddr_un sa;
	struct iovec iov[2];
	int l;

	msg.msg_name = &sa;
	msg.msg_namelen = sizeof(sa);
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	iov[0].iov_base = &mhdr;
	iov[0].iov_len = sizeof(mhdr);
	iov[1].iov_base = msg_inbuf;
	iov[1].iov_len = msg_buf_len;
	if ((l = recvmsg(p->fd, &msg, MSG_NOSIGNAL | MSG_DONTWAIT)) < 0) {
		LOG_ERROR("CTL: Recv data error");
		return;
	}
	if (msg.msg_flags != 0 || l < sizeof(mhdr) || l != sizeof(mhdr) + mhdr.lin || mhdr.lout > msg_buf_len || mhdr.cmd < 0) {
		LOG_ERROR("CTL: Unexpected message. Ignoring");
		return;
	}

	if (mhdr.lout)
		mhdr.res = ctl_handle_message(mhdr.cmd, msg_inbuf, mhdr.lin, msg_outbuf, &mhdr.lout);
	else
		mhdr.res = ctl_handle_message(mhdr.cmd, msg_inbuf, mhdr.lin, NULL, NULL);

    if(mhdr.cmd == CMD_CODE_get_ring_topo)
    {
        mhdr.res = CTL_get_ring_topo_s(*(int *)msg_inbuf,(char *)&sa,p->fd);
        if(mhdr.res == 0)
            return;
    }
	if (mhdr.res < 0)
		mhdr.lout = 0;
	
	iov[1].iov_base = msg_outbuf;
	iov[1].iov_len = mhdr.lout;
	l = sendmsg(p->fd, &msg, MSG_NOSIGNAL);
	if (l < 0)
		LOG_ERROR("CTL: Couldn't send response: %m");
	else if (l != sizeof(mhdr) + mhdr.lout) {
		LOG_ERROR("CTL: Couldn't send full response, sent %d bytes instead of %zd.", l, sizeof(mhdr) + mhdr.lout);
	}
}


int ctl_server_init(void)
{
	struct sockaddr_un sa;
	int s;

	if ((s = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1) 
		return -1;

	set_socket_address(&sa, CTL_SERVER_SOCK_NAME);
	if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
		close(s);
		return -1;
	}

	ctl_handler.fd = s;
	ctl_handler.cb = ctl_rcv_handler;

	if(uloop_fd_add(&ctl_handler,ULOOP_READ) != 0)
		return -1;
	
	return 0;
}

void ctl_server_cleanup(void)
{
	uloop_fd_delete(&ctl_handler);
	close(ctl_handler.fd);
}



