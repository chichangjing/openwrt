#ifndef _AOS_TIMER_PVT_H
#define _AOS_TIMER_PVT_H

#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/timer.h>

/* timer data type */
typedef struct timer_list	__aos_timer_t;

static inline void __aos_udelay(int usecs)
{
    udelay(usecs);
}

static inline void __aos_mdelay(int msecs)
{
    mdelay(msecs);
}

#endif

