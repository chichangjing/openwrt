/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmUserDefs.h
*
* DESCRIPTION:
*       TM Configuration Library Private Definitions.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __prvCpssTmDefs
#define __prvCpssTmDefs
/*
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h> 
*/ 

/******************************************************************************
 * typedef: enum PRV_CPSS_TM_DEVS_ENT
 *
 * Description: TM Supported devices enumeration
 *
 * Enumerations:
 *
 *  	PRV_CPSS_TM_DEV_BC2_E          - CPSS_PP_FAMILY_DXCH_BOBCAT2_E
 *  	PRV_CPSS_TM_DEV_BOBK_CETUS_E   - PRV_CPSS_DXCH_CETUS_CHECK_MAC is TRUE
 *  	PRV_CPSS_TM_DEV_BOBK_CAELUM_E  - PRV_CPSS_DXCH_CAELUM_CHECK_MAC is TRUE
 *
 ******************************************************************************/
typedef enum
{
    PRV_CPSS_TM_DEV_BC2_E,
    PRV_CPSS_TM_DEV_BOBK_CETUS_E,
    PRV_CPSS_TM_DEV_BOBK_CAELUM_E,
    PRV_CPSS_TM_DEV_LAST_E
} PRV_CPSS_TM_DEVS_ENT; 


#endif /* __prvCpssTmDefs */


