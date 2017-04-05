#include <sys/un.h>

#include "unixsock.h"

static void unixsock_recv(struct uloop_fd *h,uint32_t events)
{

}

struct uloop_fd *unixsock_init(char *s_name)
{
    int fd;

    fd = socket(AF_UNIX,SOCK_DGRAM,0);
    if(fd < 0)
        return NULL;

    struct sockaddr_un un;
    memset(&un,0,sizeof(struct sockaddr_un));
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path + 1,"/var/run/%5d",getpid());
    unlink(un.sun_path);

    if (bind(fd, (struct sockaddr *)&un, sizeof(struct sockaddr_un)) < 0)
        goto out_err;

    memset(un.sun_path,0,sizeof(un.sun_path));
    strcpy(un.sun_path + 1,s_name);

    if (connect(fd, (struct sockaddr *)&un, sizeof(struct sockaddr_un)) < 0)
        goto out_err;

    struct uloop_fd *h = calloc(1,sizeof(struct uloop_fd));
    h->fd = fd;
    h->cb = unixsock_recv;
    if(uloop_fd_add(h,ULOOP_READ) < 0)
        goto out_free;
    
    return h;

out_free:
    free(h);
out_err:
    LOG_ERROR("%m");
    close(fd);
    return NULL;
}
