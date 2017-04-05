#include "notify.h"

#define AUTH_SERVER "/var/run/auth.sock"
static void notify_server(struct uloop_fd **h)
{
    *h = unixsock_init(AUTH_SERVER);
    if(*h == NULL)
    {
        LOG_ERROR("unixsock init error");
        return;
    }
}

int notify_init(void (**cb)(struct uloop_fd **h))
{
    *cb = notify_server;

    return 0;
}
