
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "log.h"
#include "epoll_loop.h"
#include "ctl_socket.h"
#include "ctl_server.h"

#define msg_buf_len 1024
unsigned char msg_inbuf[1024];
unsigned char msg_outbuf[1024];
struct epoll_event_handler ctl_handler;

int ctl_handle_message(int cmd, void *inbuf, int lin, void *outbuf, int *lout)
{
	switch (cmd) {
		SERVER_MESSAGE_CASE(enable_rstp);
		SERVER_MESSAGE_CASE(get_bridge_state);
		SERVER_MESSAGE_CASE(set_bridge_config);
		SERVER_MESSAGE_CASE(get_port_state);
		SERVER_MESSAGE_CASE(set_port_config);
		SERVER_MESSAGE_CASE(set_debug_level);
		SERVER_MESSAGE_CASE(set_port_trace);

	default:
		log_err("CTL: Unknown command %d", cmd);
		return -1;
	}
}

void ctl_rcv_handler(uint32_t events, struct epoll_event_handler *p)
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
		return;
	}
	if (msg.msg_flags != 0 || l < sizeof(mhdr) || l != sizeof(mhdr) + mhdr.lin || mhdr.lout > msg_buf_len || mhdr.cmd < 0) {
		log_err("CTL: Unexpected message. Ignoring");
		return;
	}

	if (mhdr.lout)
		mhdr.res = ctl_handle_message(mhdr.cmd, msg_inbuf, mhdr.lin, msg_outbuf, &mhdr.lout);
	else
		mhdr.res = ctl_handle_message(mhdr.cmd, msg_inbuf, mhdr.lin, NULL, NULL);

	if (mhdr.res < 0)
		mhdr.lout = 0;
	
	iov[1].iov_base = msg_outbuf;
	iov[1].iov_len = mhdr.lout;
	l = sendmsg(p->fd, &msg, MSG_NOSIGNAL);
	if (l < 0)
		log_err("CTL: Couldn't send response: %m");
	else if (l != sizeof(mhdr) + mhdr.lout) {
		log_err("CTL: Couldn't send full response, sent %d bytes instead of %zd.", l, sizeof(mhdr) + mhdr.lout);
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
	ctl_handler.handler = ctl_rcv_handler;

	if(add_epoll(&ctl_handler) != 0)
		return -1;
	
	return 0;
}

void ctl_server_cleanup(void)
{
	remove_epoll(&ctl_handler);
	close(ctl_handler.fd);
}



