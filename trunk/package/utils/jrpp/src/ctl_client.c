
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
#include "base.h"

#define JRPP_CTL_PATH "/var/run/"
static int ctl_client_fd = -1;

int ctl_client_init(void)
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
	sprintf(un.sun_path + 1, "%s%05d", JRPP_CTL_PATH,getpid());
    unlink(un.sun_path);
	
	if (bind(s, (struct sockaddr *)&un, sizeof(struct sockaddr_un)) != 0) 
    {
		fprintf(stderr,"couldn't bind socket: %m");
		close(s);
		return -1;
	}

	set_socket_address(&un, CTL_SERVER_SOCK_NAME);
	if (connect(s, (struct sockaddr *)&un, sizeof(struct sockaddr_un)) != 0) 
    {
		fprintf(stderr,"couldn't connect to server");
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
		fprintf(stderr,"error sending message to server: %m");
		return -1;
	}
	if (l != sizeof(mhdr) + lin) {
		fprintf(stderr,"error sending message to server: Partial write");
		return -1;
	}

	iov[1].iov_base = outbuf;
	iov[1].iov_len = lout != NULL ? *lout : 0;

    l = recvmsg(ctl_client_fd, &msg, 0);
    if (l < 0) {
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


