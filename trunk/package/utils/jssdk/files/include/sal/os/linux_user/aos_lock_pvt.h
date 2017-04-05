#ifndef _AOS_LOCK_PVT_H
#define _AOS_LOCK_PVT_H

#include <pthread.h>
#include "sal/os/aos_types.h"

typedef pthread_mutex_t aos_lock_pvt_t;

#define __aos_lock_init(lock) \
	do {\
		pthread_mutexattr_t mattr;\
		pthread_mutexattr_init(&mattr);\
		pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);\
	    pthread_mutex_init(lock, &mattr);\
	} while(0)


#define __aos_lock(lock) pthread_mutex_lock(lock)
#define __aos_unlock(lock) pthread_mutex_unlock(lock)
#define __aos_irq_save(flags)
#define __aos_irq_restore(flags)
#define __aos_default_unlock PTHREAD_MUTEX_INITIALIZER

#endif

