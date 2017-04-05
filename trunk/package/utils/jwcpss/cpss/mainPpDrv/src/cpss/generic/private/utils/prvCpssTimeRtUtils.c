/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssTimeRtUtils.c
*
* DESCRIPTION:
*       CPSS implementation of ns timer
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/private/utils/prvCpssTimeRtUtils.h>

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
)
{
    GT_STATUS rc;
    GT_U32 secondsStart;
    GT_U32 nanoSecondsStart;

    if (nsPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = cpssOsTimeRT(&secondsStart,&nanoSecondsStart);
    if (rc != GT_OK)
    {
        return rc;
    }
    nsPtr->nanoSec = nanoSecondsStart;
    nsPtr->sec     = secondsStart;
    return GT_OK;
}


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
)
{
    GT_STATUS rc;
    GT_TIME end;

    GT_U32 secStart;
    GT_U32 nSecStart;
    GT_U32 secEnd;
    GT_U32 nSecEnd;

    GT_U32 seconds;
    GT_U32 nSec;

    if (prv_paTime_msPtr == NULL || prv_paTime_usPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssOsTimeRTns(&end);
    if (rc != GT_OK)
    {
        return rc;
    }


    secStart  = start.sec;
    nSecStart = start.nanoSec;

    secEnd  =  end.sec;
    nSecEnd =  end.nanoSec;

    seconds = secEnd - secStart;
    if(nSecEnd >= nSecStart)
    {
        nSec = nSecEnd - nSecStart;
    }
    else
    {
        nSec = (1000000000 - nSecEnd) + nSecStart;
        seconds--;
    }

    *prv_paTime_msPtr = nSec/1000000;
    *prv_paTime_usPtr = (nSec%1000000)/1000;
    return GT_OK;
}


