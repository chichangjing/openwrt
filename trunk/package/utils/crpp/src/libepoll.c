#include <stdio.h>
#include <unistd.h>

#include "libepoll.h"

#define MAX_EVENTS 8

static int epoll_fd = -1;
static struct epoll_event event_pool[MAX_EVENTS];
static LIST_HEAD(timeouts);

static int timer_diff(struct timeval *second, struct timeval *first)
{
	return (second->tv_sec - first->tv_sec) * 1000 + (second->tv_usec - first->tv_usec) / 1000;
}

// 删除定时器
int timer_del(struct uloop_timeout *timer)
{
    if(!timer->pending)
        return -1;

    list_del(&timer->list);
    timer->pending = False;

    return 0;
}

// 添加(复位)定时器
int timer_add(struct uloop_timeout *timer, int msecs)
{
    struct timeval *time = &timer->time;

    if(timer->pending)
        timer_del(timer);

    gettimeofday(time,NULL);

    time->tv_sec += msecs / 1000;
    time->tv_usec += msecs % 1000 * 1000;
    if(time->tv_usec > 1000000)
    {
        time->tv_sec++;
        time->tv_usec -= 1000000;
    }

    struct uloop_timeout *t;
    struct list_head *h = &timeouts;
    list_for_each_entry(t,&timeouts,list)
    {
        if(timer_diff(&t->time,time) > 0)
        {
            h = &t->list;
            break;
        }
    }

    list_add_tail(&timer->list,h);
    timer->pending = True;

    return 0;
}

static void timer_process(struct timeval *cur)
{
    struct uloop_timeout *t;
    while(!list_empty(&timeouts))
    {
        t = list_first_entry(&timeouts,struct uloop_timeout,list);
        if(timer_diff(&t->time,cur) > 0)
            break;

        timer_del(t);
        if(t->cb)
            t->cb(t);
    }
}

static uint32_t timer_next_timeout(struct timeval *cur)
{
    struct uloop_timeout *t;
    int diff;

    if(list_empty(&timeouts))
        return -1;

    t = list_first_entry(&timeouts,struct uloop_timeout,list);
    diff = timer_diff(&t->time,cur);

    return (diff < 0)?0:diff;
}

int epoll_init(void)
{
    epoll_fd = epoll_create(32);
    if(epoll_fd < 0)
        return -1;

    fcntl(epoll_fd,F_SETFD,fcntl(epoll_fd,F_GETFD) | FD_CLOEXEC);
    return 0;
}

int epoll_add(struct epoll_handler_s *h,uint32_t events)
{
    fcntl(h->fd,F_SETFL,fcntl(h->fd,F_GETFL) | O_NONBLOCK);
    fcntl(h->fd,F_SETFD,fcntl(h->fd,F_GETFD) | FD_CLOEXEC);

    struct epoll_event ev = {
        .events     = events,
        .data.ptr   = h,
    };

    return epoll_ctl(epoll_fd,EPOLL_CTL_ADD,h->fd,&ev);
}

int epoll_del(struct epoll_handler_s *h)
{
    return epoll_ctl(epoll_fd,EPOLL_CTL_DEL,h->fd,NULL);
}

static int epoll_deal_events(uint32_t timeout)
{
    int n,nfds;
    struct epoll_handler_s *h;
   
    nfds = epoll_wait(epoll_fd,event_pool,MAX_EVENTS,timeout);

    for(n = 0;n < nfds;n++)
    {
        h = event_pool[n].data.ptr;
        if(!h)
            continue;

        h->cb(h,event_pool[n].events);
    }

    return nfds;
}

void epoll_loop(void)
{
    struct timeval tv;

    while(1)
    {
        gettimeofday(&tv,NULL);   
        timer_process(&tv);
        gettimeofday(&tv,NULL);   

        epoll_deal_events(timer_next_timeout(&tv));
    }
}
