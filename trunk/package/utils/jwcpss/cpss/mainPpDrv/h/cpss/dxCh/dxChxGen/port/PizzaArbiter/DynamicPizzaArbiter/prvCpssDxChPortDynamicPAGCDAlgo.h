/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPAGCDAlgo.h
*
* DESCRIPTION:
*       GCD (greatest common divider) algorithm
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_DYN_PA_GCD_ALGO_H
#define __PRV_CPSS_DXCH_DYN_PA_GCD_ALGO_H

#ifndef STAND_ALONE
    #include <cpss/generic/cpssTypes.h>
#else
    #include "CpssInclude/cpssTypes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChDynPAGCDAlgo
*
* DESCRIPTION:
*         GCD (greatest common divider) algorithm
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       a  - first number 
*       b  - second number
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvCpssDxChDynPAGCDAlgo
(
    IN GT_U32 a, 
    IN GT_U32 b
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
