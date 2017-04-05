#ifndef _NOTIFY_H_
#define _NOTIFY_H_

#include "unixsock.h"

int notify_init(void (**cb)(struct uloop_fd **h));
#endif
