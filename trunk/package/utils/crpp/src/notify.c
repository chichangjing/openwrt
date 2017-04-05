#include "notify.h"
#include "unixsock.h"

#define RSTP_SERVER "/var/run/rstpd.sock"
#define JRPP_SERVER "/var/run/jrppd.sock"

static void notify_server(struct uloop_fd **h)
{
    *h = unixsock_init(JRPP_SERVER);
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
