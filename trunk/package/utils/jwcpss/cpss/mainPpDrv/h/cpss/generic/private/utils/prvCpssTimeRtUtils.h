/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssTimeRtUtils.h
*
* DESCRIPTION:
*       CPSS implementation of ns timer
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __PRV_CPSS_TIMER_RT_NS_H
#define __PRV_CPSS_TIMER_RT_NS_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: struct PRV_CPSS_DRV_FLD_DEF_STC
 *
 * time in sec:nanoSec
 *
 * Fields:
 *     nanoSec - nano seconds
 *     sec - seconds;
 */
typedef struct 
{
    GT_U32 nanoSec;
    GT_U32 sec;
}GT_TIME;



/*******************************************************************************
* prvCpssOsTimeRTns
*
* DESCRIPTION:
*       get time in sec:nano-sec
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*
* OUTPUTS:
*       nsPtr - place to store time stamp
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssOsTimeRTns
(
    OUT GT_TIME *nsPtr
);

/*******************************************************************************
* prvCpssOsTimeRTDiff
*
* DESCRIPTION:
*       callc diff ftom taken time in msec-nsec
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       start : time point 
* OUTPUTS:
*       prv_paTime_msPtr - place to store time diff msec
*       prv_paTime_usPtr - place to store time diff nsec
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssOsTimeRTDiff
(
    IN   GT_TIME start,
    OUT  GT_U32 *prv_paTime_msPtr,
    OUT  GT_U32 *prv_paTime_usPtr  
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



