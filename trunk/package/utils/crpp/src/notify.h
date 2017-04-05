#ifndef _NOTIFY_H_
#define _NOTIFY_H_

#include <libubox/uloop.h>
#include <libubox/utils.h>

#include "libepoll.h"

int notify_init(void (**cb)(struct uloop_fd **h));
#endif
