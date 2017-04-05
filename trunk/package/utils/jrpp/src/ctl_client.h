
#ifndef _CTL_CLIENT_H_
#define _CTL_CLIENT_H_

int ctl_client_init(void);
void ctl_client_cleanup(void);
int ctl_client_send(int cmd, void *inbuf, int lin, void *outbuf, int *lout, int *res);
	
#endif

