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
* osVxTimer.c
*
* DESCRIPTION:
*       VxWorks Operating System wrapper. Timer facility.
*
* DEPENDENCIES:
*       VxWorks, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <vxWorks.h>
#include <time.h>
#include <tickLib.h>
#include <sysLib.h>
#include <taskLib.h>

#include <gtOs/gtOsTimer.h>
#include <gtOs/gtOsGen.h>

/************ Public Functions ************************************************/

/*******************************************************************************
* osTimerWkAfter
*
* DESCRIPTION:
*       Puts current task to sleep for specified number of milisecond.
*
* INPUTS:
*       mils - time to sleep in miliseconds
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
GT_STATUS osTimerWkAfter
(
    IN GT_U32 mils
)
{
    GT_U32 delay, time;

    IS_WRAPPER_OPEN_STATUS;

    time = sysClkRateGet() * mils;

    delay = (time % 1000) ? (time / 1000 + 1) : (time / 1000);

    taskDelay (delay);

    return GT_OK;
}

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
    IS_WRAPPER_OPEN_PTR;
    return tickGet();
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

   IS_WRAPPER_OPEN_PTR;
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
    IS_WRAPPER_OPEN_PTR;
    localtime_r(&tim_val, &tm_val);
    return (GT_U32)strftime(bufferPtr, bufferSize, format, &tm_val);
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

    if (clock_gettime(CLOCK_REALTIME, &tv) != OK)
        return GT_FAIL;

    if (seconds != NULL)
        *seconds = (GT_U32)tv.tv_sec;
    if (nanoSeconds != NULL)
        *nanoSeconds = (GT_U32)tv.tv_nsec;

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
    volatile GT_U32 i;                /* loop iterator */
    GT_U32          iterationsNum;    /* num of iterations for busy wait */


/* FireFox CPU 88e6218 */
iterationsNum =  (delay / 10000) * 122;

#if (CPU == PPC603)
    iterationsNum = (delay / 10000) * 190;
#endif

#ifdef CPU_926EJ
  /* Orion CPU 88f5181 */
    iterationsNum =  (delay / 10000) * 362;
#endif


    for(i = 0; i < iterationsNum; i++);

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
	IS_WRAPPER_OPEN_STATUS;
    *ticks = sysClkRateGet();
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
GT_STATUS timerDebug(GT_U32 delay, GT_U32 loops)
{
    GT_U32 start, stop, sysClkRate;
    GT_U32 time,ii;


#if (CPU == PPC603)
   osPrintf("inside PPC603\n");
#endif

#ifdef CPU_926EJ
  /* Orion CPU 88f5181 */
   osPrintf("inside Orion CPU 88f5181\n");
#endif

#ifdef CPU_946ES
  /* Firefox CPU 88e6828 */
   osPrintf("inside Firefox CPU 88e6828\n");
#endif

    ii = 0;
    start = tickGet ();
    for (; ii < loops; ii++)
        osDelay(delay);

    stop = tickGet ();
    sysClkRate = sysClkRateGet();
    osPrintf("ii=%d sysClkRate=%d\n", ii, sysClkRate);

    time =  (stop - start) * 1000 / sysClkRate;

    osPrintf("Ticks: = %d,  Mili Seconds: %d\n",((stop - start)), time);

    return GT_OK;


}

#endif



