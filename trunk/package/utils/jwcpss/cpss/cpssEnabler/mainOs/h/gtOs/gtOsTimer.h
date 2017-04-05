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
* gtOsTimer.h
*
* DESCRIPTION:
*       Operating System wrapper. Timer facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#ifndef __gtOsTimerh
#define __gtOsTimerh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

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
);

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
GT_U32 osTickGet(void);


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
GT_U32 osTime(void);

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
GT_STATUS osTimeRT
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
GT_STATUS osGetSysClockRate
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
GT_STATUS osDelay
(
    IN GT_U32 nanosec
);


#ifdef __cplusplus
}
#endif

#endif  /* __gtOsTimerh */
/* Do Not Add Anything Below This Line */


