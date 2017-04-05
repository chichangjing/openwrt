#ifndef _RAWSOCK_H_
#define _RAWSOCK_H_

#include <linux/filter.h>
#include "utils.h"
#include "crpp.h"

struct uloop_fd *rawsock_init(struct sock_fprog *prog);
int rawsock_send(int fd,uint8_t *data,int len);
#endif
