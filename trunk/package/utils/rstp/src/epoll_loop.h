
#ifndef _EPOLL_LOOP_H_
#define _EPOLL_LOOP_H_

#include <sys/epoll.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

struct epoll_event_handler {
	int fd;
	void *arg;
	void (*handler) (uint32_t events, struct epoll_event_handler *p);
	struct epoll_event *ref_ev;
};

int init_epoll(void);
void clear_epoll(void);
int epoll_main_loop(void);
int add_epoll(struct epoll_event_handler *h);
int remove_epoll(struct epoll_event_handler *h);

#endif

