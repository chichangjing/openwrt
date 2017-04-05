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
* osWin32Timer.c
*
* DESCRIPTION:
*       Win32 Operating System Simulation. Timer facility.
*
* DEPENDENCIES:
*       Win32, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#include <windows.h>
#include <process.h>
#include <time.h>

#include <gtOs/gtOsTimer.h>

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
    Sleep(mils);

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
    clock_t clockT = clock();

    if(clockT==-1)
    {
        return 0;
    }

    return clockT;
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
    time_t tim_val = (time_t)timeValue;
    return (GT_U32)strftime(bufferPtr, bufferSize, format, localtime(&tim_val));
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
    static int      timer_type = 0;
    static LONGLONG frequency = 0;
    static LONGLONG start;
    static GT_U32   start_sec = 0;

    LARGE_INTEGER   now;
    time_t          t;

    if (timer_type == 0)
    {
        /* uninitialized */
        if (QueryPerformanceFrequency(&now))
        {
            /* timer supported */
            timer_type = 2; /* QueryPerformanceCounter() */
            frequency = now.QuadPart;
            QueryPerformanceCounter(&now);
            start = now.QuadPart;
            start_sec = (GT_U32)time(&t);
        } else {
            timer_type = 1; /* time() */
        }
    }
    if (timer_type == 1)
    {
        /* time() */
        if (seconds != NULL)
            *seconds = (GT_U32)time(&t);
        if (nanoSeconds != NULL)
            *nanoSeconds = 0;
        return GT_OK;
    }
    /* timer_type == 2, QueryPerformanceCounter() */
    QueryPerformanceCounter(&now);
    now.QuadPart -= start;
    if (seconds != NULL)
        *seconds = start_sec + (GT_U32)(now.QuadPart / frequency);
    if (nanoSeconds != NULL)
    {
        now.QuadPart %= frequency;
        now.QuadPart *= 1000000000;
        *nanoSeconds = (GT_U32)(now.QuadPart / frequency);
    }
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
    GT_U32      *ticksPtr
)
{
    if (ticksPtr == NULL)
    {
        return GT_BAD_PTR;
    }
    *ticksPtr = 1;
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
*       
*
*******************************************************************************/
GT_STATUS osDelay
(
    IN GT_U32 delay
)
{
    /* the minimal delay in the Windows is 1 milisecond */
    Sleep(1);

    return GT_OK;

}
