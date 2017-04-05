/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssOsTimer.h
*
* DESCRIPTION:
*       Operating System wrapper. Timer facility.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __cpssOsTimerh
#define __cpssOsTimerh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*******************************************************************************
* CPSS_OS_TIME_WK_AFTER_FUNC
*
* DESCRIPTION:
*       Puts current task to sleep for specified number of millisecond.
*
* INPUTS:
*       mils - time to sleep in milliseconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_OS_TIME_WK_AFTER_FUNC)
(
    IN GT_U32 mils
);

/*******************************************************************************
* CPSS_OS_TIME_TICK_GET_FUNC
*
* DESCRIPTION:
*       Gets the value of the kernel's tick counter.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The tick counter value.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_U32 (*CPSS_OS_TIME_TICK_GET_FUNC)
(
    void
);


/*******************************************************************************
* CPSS_OS_TIME_GET_FUNC
*
* DESCRIPTION:
*       Gets number of seconds passed since system boot
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The second counter value.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_U32 (*CPSS_OS_TIME_GET_FUNC)
(
    void
);

/*******************************************************************************
* osStrftime
*
* DESCRIPTION:
*       Convert time value into a formatted string
*
* INPUTS:
*       n       - size of string buffer
*       format  - format string, see strftime()
*       tim     - time value returned by cpssOsTime()
*
* OUTPUTS:
*       s       - String buffer for resulting string
*
* RETURNS:
*       The total number of characters copied to buffer.
*
* COMMENTS:
*       None
*       
*
*******************************************************************************/
typedef GT_U32 (*CPSS_OS_STRF_TIME_FUNC)
(
    OUT char *  s,
    IN  GT_U32  n,
    IN  const char* format,
    IN  GT_U32  tim
);

/*******************************************************************************
* osTimeRT
*
* DESCRIPTION:
*       Get the current time with nanoseconds
*
* INPUTS:
*       None
*
* OUTPUTS:
*       seconds     - elapsed time in seconds
*       nanoSeconds - elapsed time within a second
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_OS_TIME_RT_FUNC)
(
    OUT GT_U32  *seconds,
    OUT GT_U32  *nanoSeconds
);

/*******************************************************************************
* osGetSysClockRate
*
* DESCRIPTION:
*       Get the system clock rate
*
* INPUTS:
*       None
*
* OUTPUTS:
*       ticks - The number of ticks per second of the system clock.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_OS_GET_SYS_CLOCK_RATE_FUNC)
(
    OUT GT_U32  *ticks
);

/*******************************************************************************
* osDelay
*
* DESCRIPTION:
*       System Delay nanoseconds.
*
* INPUTS:
*       delay - non operational time in nanoseconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_OS_DELAY_FUNC)
(
    IN GT_U32 nanosec
);

/* CPSS_OS_TIME_BIND_STC -
    structure that hold the "os time" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_OS_TIME_WK_AFTER_FUNC    osTimeWkAfterFunc;
    CPSS_OS_TIME_TICK_GET_FUNC    osTimeTickGetFunc;
    CPSS_OS_TIME_GET_FUNC         osTimeGetFunc;
    CPSS_OS_TIME_RT_FUNC          osTimeRTFunc;
    CPSS_OS_GET_SYS_CLOCK_RATE_FUNC osGetSysClockRateFunc;
    CPSS_OS_DELAY_FUNC            osDelayFunc;
    CPSS_OS_STRF_TIME_FUNC        osStrftimeFunc;  
}CPSS_OS_TIME_BIND_STC;


#ifdef __cplusplus
}
#endif

#endif  /* __cpssOsTimerh */


