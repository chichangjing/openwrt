
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

#include "ctl_socket.h"
#include "log.h"

static int ctl_client_fd = -1;

int ctl_client_init(void)
{
	int s;
	struct sockaddr_un sa_client;
	struct sockaddr_un sa_server;
	char tmpname[64];

	if((s = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
		log_err("couldn't open unix socket: %m");
		return -1;
	}

	set_socket_address(&sa_server, CTL_SERVER_SOCK_NAME);
	sprintf(tmpname, "RSTP_CTL_%d", getpid());
	set_socket_address(&sa_client, tmpname);

	if (bind(s, (struct sockaddr *)&sa_client, sizeof(sa_client)) != 0) {
		log_err("couldn't bind socket: %m");
		close(s);
		return -1;
	}

	if (connect(s, (struct sockaddr *)&sa_server, sizeof(sa_server)) != 0) {
		log_err("couldn't connect to server");
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

int ctl_client_send(int cmd, void *inbuf, int lin, void *outbuf, int *lout, int *res)
{
	struct ctl_msg_hdr mhdr;
	struct msghdr msg;
	struct iovec iov[2];
	int l;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
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
		log_err("error sending message to server: %m");
		return -1;
	}
	if (l != sizeof(mhdr) + lin) {
		log_err("error sending message to server: Partial write");
		return -1;
	}

	iov[1].iov_base = outbuf;
	iov[1].iov_len = lout != NULL ? *lout : 0;

	{
		struct pollfd pfd;
		int timeout = 5000;	/* 5 s */
		int r;

		pfd.fd = ctl_client_fd;
		pfd.events = POLLIN;
		do {
			r = poll(&pfd, 1, timeout);
			if (r == 0) {
				log_err("error getting message from server: Timeout");
				return -1;
			}
			if (r < 0) {
				log_err("error getting message from server: poll error: %m");
				return -1;
			}
		} while ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL | POLLIN)) == 0);

		l = recvmsg(ctl_client_fd, &msg, 0);
		if (l < 0) {
			log_err("error getting message from server: %m");
			return -1;
		}
		if (l < sizeof(mhdr) || l != sizeof(mhdr) + mhdr.lout || mhdr.cmd != cmd) {
			log_err("error getting message from server: Bad format");
			return -1;
		}
	}
	
	if (lout)
		*lout = mhdr.lout;
	if (res)
		*res = mhdr.res;

	return 0;
}


