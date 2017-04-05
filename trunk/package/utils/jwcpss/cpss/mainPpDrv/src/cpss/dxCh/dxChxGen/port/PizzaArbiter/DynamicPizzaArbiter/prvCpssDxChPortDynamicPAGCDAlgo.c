/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPAGCDAlgo.c
*
* DESCRIPTION:
*       GCD (greatest common divider) algorithm
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAGCDAlgo.h>


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
)
{
    while(b) 
    {
        /* b ^= a ^= b ^= a %= b; */   
        a %= b;
        /* exchange */
        b ^= a;
        a ^= b; 
        b ^= a;     
    }
    return a;
}
