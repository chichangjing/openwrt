/*******************************************************************************
* mainOsPthread.h
*
* DESCRIPTION:
*       Pthread implementation of mainOs tasks and semaphores
*
* DEPENDENCIES:
*       Linux, CPU independed, and posix threads implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#ifndef __mainOsPthread_h
#define __mainOsPthread_h

#include <sched.h>
#include <sys/types.h>
#include <time.h>
#include <inttypes.h>

/************* Defines ********************************************************/

#ifndef IN
# define IN
#endif
#ifndef OUT
# define OUT
#endif

#ifndef SHARED_MEMORY
#  define CREATE_MTX(mtx) pthread_mutex_init(mtx, NULL)
#  define CREATE_COND(cond) pthread_cond_init(cond, NULL)
#else /* SHARED_MEMORY */
#  define CREATE_MTX(mtx) do { \
        pthread_mutexattr_t mattr; \
        pthread_mutexattr_init(&mattr); \
        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED); \
        pthread_mutex_init(mtx, &mattr); \
    } while(0)
#  define CREATE_COND(cond) do { \
        pthread_condattr_t cattr; \
        pthread_condattr_init(&cattr); \
        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED); \
        pthread_cond_init(cond, &cattr); \
    } while(0)
#endif /* !SHARED_MEMORY */

/************* lkernelLib.c ***************************************************/
/* internals */

void *V2L_ts_malloc( size_t blksize );
void V2L_ts_free( void *blkaddr );


/************* ltaskLib.c *****************************************************/

/*******************************************************************************
* V2L_ltaskInit
*
* DESCRIPTION:
*       Initialize tasks
*
* INPUTS:
*       name       - root task name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       OK         - on success
*       ERROR      - on error
*
* COMMENTS:
*       This function called from osStartEngine()
*
*******************************************************************************/
int V2L_ltaskInit
(
    IN  const char *name
);


/************* ltimerLib.c ****************************************************/

/********************************
** timespec arithmetic
********************************/

/*****************************************************************************
** timespec_is_zero - returns 1 if value is zero
*****************************************************************************/
extern int timespec_is_zero(struct timespec *value);

/*****************************************************************************
** timespec_gt - returns 1 if a greater than b
*****************************************************************************/
extern int timespec_gt(struct timespec *a, struct timespec *b);

/*****************************************************************************
** milliseconds2timespec - convert time in system ticks to timespec structure
**            ( one tick is currently implemented as ten milliseconds )
*****************************************************************************/
extern void milliseconds2timespec(int milliseconds, struct timespec *result);

/*****************************************************************************
** timespec_add - add value to accumulator
*****************************************************************************/
extern void timespec_add(struct timespec *accumulator, struct timespec *value);

/*****************************************************************************
** timespec_sub - sub b from a, delta = 0 if b >= a, otherwise delta = a - b
*****************************************************************************/
extern void timespec_sub(struct timespec *a, struct timespec *b, struct timespec *delta);


/************* lsemLib.c ******************************************************/

/*******************************************************************************
* V2L_lsemInit
*
* DESCRIPTION:
*       Initialize semaphores
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       OK         - on success
*       ERROR      - on error
*
* COMMENTS:
*       This function called from osStartEngine()
*
*******************************************************************************/
int V2L_lsemInit(void);

/*******************************************************************************
* V2L_lmsgQInit
*
* DESCRIPTION:
*       Initialize message queues
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       OK         - on success
*       ERROR      - on error
*
* COMMENTS:
*       This function called from osStartEngine()
*
*******************************************************************************/
int V2L_lmsgQInit(void);

#endif /* __mainOsPthread_h */

