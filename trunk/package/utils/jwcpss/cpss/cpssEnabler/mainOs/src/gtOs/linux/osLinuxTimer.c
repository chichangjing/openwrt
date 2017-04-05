/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* osLinuxTimer.c
*
* DESCRIPTION:
*       Linux User Mode Operating System wrapper. Queue facility.
*
* DEPENDENCIES:
*       Linux , CPU independed  and posix threads implementation..
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/
#define _GNU_SOURCE

#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <gtOs/gtOsTimer.h>

#ifndef CPSS_CLOCK
#define CPSS_CLOCK CLOCK_REALTIME
#endif

/************ Public Functions ************************************************/

/*******************************************************************************
* osTickGet
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
GT_U32 osTickGet(void)
{
    return (GT_U32)(times(NULL));
}


/*******************************************************************************
* osTime
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
GT_U32 osTime(void)
{
   time_t t ;
   return (GT_U32) time(&t);
}

/*******************************************************************************
* osStrftime
*
* DESCRIPTION:
*       convert time value into a formatted string
*
* INPUTS:
*       bufferSize  - size of string buffer
*       format      - format string, see strftime()
*       timeValue   - time value returned by osTime()
*
* OUTPUTS:
*       bufferPtr   - String buffer for resulting string
*
* RETURNS:
*       The second counter value.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 osStrftime(
    OUT char *  bufferPtr,
    IN  GT_U32  bufferSize,
    IN  const char* format,
    IN  GT_U32  timeValue
)
{
    struct tm tm_val;
    time_t tim_val = (time_t)timeValue;
    return (GT_U32)strftime(bufferPtr, bufferSize, format, localtime_r(&tim_val, &tm_val));
}

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
GT_STATUS osTimeRT
(
    OUT GT_U32  *seconds,
    OUT GT_U32  *nanoSeconds
)
{
    struct timespec tv;

    if (clock_gettime(CPSS_CLOCK, &tv) < 0)
        return GT_FAIL;

    if (seconds != NULL)
        *seconds = (GT_U32)tv.tv_sec;
    if (nanoSeconds != NULL)
        *nanoSeconds = (GT_U32)tv.tv_nsec;

    return GT_OK;
}

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
GT_STATUS osGetSysClockRate
(
    OUT GT_U32  *ticks
)
{
    *ticks = (GT_U32)sysconf(_SC_CLK_TCK);
    return GT_OK;
}

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
*       The values for calculation of iterations number, achieved by tests.    
*
*******************************************************************************/
GT_STATUS osDelay
(
    IN GT_U32 delay
)
{
  struct timespec sleep, remain ;
  if (1000000000 < delay)
  {
     /* sleep time to big */
    return GT_BAD_PARAM;
  }
  
  sleep.tv_nsec = delay;
  sleep.tv_sec  = 0 ;
  if ( nanosleep(&sleep, &remain) )
  {
    return GT_FAIL;
  }

    return GT_OK;
}

#ifdef DEBUG_OS_TIMER
/*******************************************************************************
* timerDebug
*
* DESCRIPTION:
*       System Delay nanoseconds.
*
* INPUTS:
*       delay - non operational time in nanoseconds
*       loops - number of timer to perform delay
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
GT_STATUS timerDebug(GT_U32 loops)
{
    struct timespec start, end;
    volatile GT_U32 i;                /* loop iterator */

    clock_gettime(CPSS_CLOCK, &start);
    for(i = 0; i < loops; i++);
    clock_gettime(CPSS_CLOCK, &end);
    osPrintf("Start Time: sec=%d, nanosec=%d \n",start.tv_sec,start.tv_nsec);
    osPrintf("End Time: sec=%d, nanosec=%d \n",end.tv_sec,end.tv_nsec);
    osPrintf("Time: sec=%d, nanosec=%d \n",end.tv_sec - start.tv_sec,end.tv_nsec - start.tv_nsec);
    return GT_OK;


}

#endif

