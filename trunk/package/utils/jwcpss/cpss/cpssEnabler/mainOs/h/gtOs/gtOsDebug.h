/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsDebug.h
*
* DESCRIPTION:
*       Operating System wrapper. Debug facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __gtOsDebugh
#define __gtOsDebugh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>
#include <gtOs/gtOsTask.h>

#define OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS  101

/*
 * Typedef: struct OS_DEBUG_UTIL_MEAS_STC
 *
 * Description:
 *      Utilization measurement.
 *
 * Fields:
 *      utilize_5_sec - utilization for last 5 seconds in percents.
 *      utilize_min   - utilization for last minute in percents.
 *      utilize_5_min - utilization for last 5 minutes in percents.
 *
 * Comments:
 *      OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS is used when utilization
 *      measurement is not available.
 * 
 */

typedef struct
{
    GT_U32  utilize_5_sec;
    GT_U32  utilize_min;
    GT_U32  utilize_5_min;
} OS_DEBUG_UTIL_MEAS_STC;

/*******************************************************************************
* osDebugServicesInit
*
* DESCRIPTION:
*       Initializes the Debug Services.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugServicesInit(GT_VOID);

/*******************************************************************************
* osDebugTaskUtilStart
*
* DESCRIPTION:
*       Starts task utilization measurement.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugTaskUtilStart(GT_VOID);

/*******************************************************************************
* osDebugTaskUtilStop
*
* DESCRIPTION:
*       Stops task utilization measurement.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugTaskUtilStop(GT_VOID);

/*******************************************************************************
* osDebugTaskUtilGet
*
* DESCRIPTION:
*       Gets task utilization measurements for given task.
*
* INPUTS:
*       taskId - ID of task whose measurement is to be given.
*
* OUTPUTS:
*       utilizationPtr - (pointer to) utilization measurements.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugTaskUtilGet
(
    IN GT_TASK  taskId,
    OUT OS_DEBUG_UTIL_MEAS_STC *utilizationPtr
);

/*******************************************************************************
* osDebugCpuUtilGet
*
* DESCRIPTION:
*       Gets CPU utilization measurement.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       utilizationPtr - (pointer to) utilization measurements.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugCpuUtilGet
(
    OUT OS_DEBUG_UTIL_MEAS_STC *utilizationPtr
);

/*******************************************************************************
* osDebugFreeMemGet
*
* DESCRIPTION:
*       Gets the free memory in the system.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       freeMemPtr - (pointer to) free memory in Kbytes.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugFreeMemGet
(
    OUT GT_U32 *freeMemPtr
);

#ifdef __cplusplus
}
#endif

#endif  /* __gtOsDebugh */
