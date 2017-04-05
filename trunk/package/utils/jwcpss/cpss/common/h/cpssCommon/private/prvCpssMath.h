/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssMath.h
*
* DESCRIPTION:
*       Math operations for CPSS.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#ifndef __prvCpssMathh
#define __prvCpssMathh

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssMathAdd64
*
* DESCRIPTION:
*       Summarize two 64 bits values.
*
* INPUTS:
*       x - first value for sum.
*       y - second value for sum
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Calculated sum.
*
* COMMENTS:
*       This function does not care 65 bit overflow.
*
*******************************************************************************/
GT_U64  prvCpssMathAdd64
(
    IN  GT_U64 x,
    IN  GT_U64 y
);

/*******************************************************************************
* prvCpssMathSub64
*
* DESCRIPTION:
*       Subtract two 64 bits values.
*
* INPUTS:
*       x - first value for difference.
*       y - second value for difference
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Calculated difference x - y.
*
* COMMENTS:
*
*******************************************************************************/
GT_U64  prvCpssMathSub64
(
    IN  GT_U64 x,
    IN  GT_U64 y
);

/*******************************************************************************
* prvCpssMathDiv64By16
*
* DESCRIPTION:
*       Divides 64 bits value by 16 bits value.
*
* INPUTS:
*       x - dividend.
*       y - divisor
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Calculated quotient.
*
* COMMENTS:
*
*******************************************************************************/
GT_U64  prvCpssMathDiv64By16
(
    IN  GT_U64 x,
    IN  GT_U16 y
);

/*******************************************************************************
* prvCpssMathMod64By16
*
* DESCRIPTION:
*       Calculate reminder from division 64-bits value by 16-bits value.
*
* INPUTS:
*       x - dividend.
*       y - divisor
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Calculated reminder.
*
* COMMENTS:
*
*******************************************************************************/
GT_U16  prvCpssMathMod64By16
(
    IN  GT_U64 x,
    IN  GT_U16 y
);

/*******************************************************************************
* prvCpssMathMul64
*
* DESCRIPTION:
*       Calculate multiplication of 32-bits value by 32-bits value.
*
* INPUTS:
*       x - dividend.
*       y - divisor
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Calculated x*y.
*
* COMMENTS:
*
*******************************************************************************/
GT_U64 prvCpssMathMul64
(
    IN GT_U32 x,
    IN GT_U32 y
);

/*******************************************************************************
* prvCpssMathIsPowerOf2
*
* DESCRIPTION:
*       This routine checks whether number is a power of 2.
*
* INPUTS:
*       number - the number to verify.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE  - for true.
*       GT_FALSE - for false.
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvCpssMathIsPowerOf2
(
    IN GT_U32 number
);

/*******************************************************************************
* prvCpssMathLog2
*
* DESCRIPTION:
*       This function calculates log(x) by base 2.
*
* INPUTS:
*       num - The number to perform the calculation on, This number must be a
*             power of 2.
*
* OUTPUTS:
*       resultPtr  - pointer to log(num) result.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise (if the number is not a power of 2).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMathLog2
(
    IN  GT_U32  num,
    OUT GT_U32  *resultPtr
);

/*******************************************************************************
* prvCpssMathPowerOf2
*
* DESCRIPTION:
*       Calculate power of 2
*
* INPUTS:
*       exponent  - the exponent, a value between 0 to 7
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_BAD_PARAM - if exponent is not between 0 to 7
*       GT_OK        - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMathPowerOf2
(
    IN  GT_U8   exponent,
    OUT GT_U8   *result
);

/*******************************************************************************
* prvCpssRoundDiv
*
* DESCRIPTION:
*   Divide and round a value with accuracy of 3 digits.
*
* INPUTS:
*   value   - Value to divide.
*   divisor - the divisor.
*
* OUTPUTS:
*   resultPtr   - pointer to result after round
*
* RETURNS :
*   GT_OK
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssRoundDiv
(
    IN  GT_U32  value,
    IN  GT_U32  divisor,
    OUT GT_U32 *resultPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssMathh */

