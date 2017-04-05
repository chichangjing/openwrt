
#include <sys/epoll.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

#include "log.h"
#include "epoll_loop.h"
#include "rstp_if.h"

int epoll_fd = -1;
struct timeval nexttimeout;

int init_epoll(void)
{
	int r = epoll_create(128);
	if (r < 0) {
		log_err("epoll_create failed: %m\n");
		return -1;
	}
	epoll_fd = r;
	return 0;
}

int add_epoll(struct epoll_event_handler *h)
{
	struct epoll_event ev = {
		.events = EPOLLIN,
		.data.ptr = h,
	};
	h->ref_ev = NULL;
	int r = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, h->fd, &ev);
	if (r < 0) {
		log_err("epoll_ctl_add failed: %m\n");
		return -1;
	}
	return 0;
}

int remove_epoll(struct epoll_event_handler *h)
{
	int r = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, h->fd, NULL);
	if (r < 0) {
		log_err("epoll_ctl_del failed: %m\n");
		return -1;
	}
	if (h->ref_ev && h->ref_ev->data.ptr == h) {
		h->ref_ev->data.ptr = NULL;
		h->ref_ev = NULL;
	}
	return 0;
}

void clear_epoll(void)
{
	if (epoll_fd >= 0)
		close(epoll_fd);
}

int time_diff(struct timeval *second, struct timeval *first)
{
	return (second->tv_sec - first->tv_sec) * 1000 + (second->tv_usec - first->tv_usec) / 1000;
}

void run_timeouts(void)
{
	br_one_second();
	nexttimeout.tv_sec++;
}

int epoll_main_loop(void)
{
	gettimeofday(&nexttimeout, NULL);
	nexttimeout.tv_sec++;
#define EV_SIZE 8
	struct epoll_event ev[EV_SIZE];

	while (1) {
		int r, i;
		int timeout;

		struct timeval tv;
		gettimeofday(&tv, NULL);
		timeout = time_diff(&nexttimeout, &tv);
		if (timeout < 0) {
			run_timeouts();
			timeout = 0;
		}

		r = epoll_wait(epoll_fd, ev, EV_SIZE, timeout);
		if (r < 0 && errno != EINTR) {
			log_err("epoll_wait error: %m\n");
			continue;
			//return -1;
		}
		for (i = 0; i < r; i++) {
			struct epoll_event_handler *p = ev[i].data.ptr;
			if (p != NULL)
				p->ref_ev = &ev[i];
		}
		for (i = 0; i < r; i++) {
			struct epoll_event_handler *p = ev[i].data.ptr;
			if (p && p->handler)
				p->handler(ev[i].events, p);
		}
		for (i = 0; i < r; i++) {
			struct epoll_event_handler *p = ev[i].data.ptr;
			if (p != NULL)
				p->ref_ev = NULL;
		}
	}

	return 0;
}

