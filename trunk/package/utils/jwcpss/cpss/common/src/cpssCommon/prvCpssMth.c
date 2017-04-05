/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssMath.c
*
* DESCRIPTION:
*       Math operations for CPSS.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
******************************************************************************/

#include <cpssCommon/private/prvCpssMath.h>
#include <cpssCommon/cpssPresteraDefs.h>

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
)
{
    GT_U64  z;
    GT_U32  maxVal;

    maxVal = y.l[0];
    if (x.l[0] > y.l[0])
        maxVal = x.l[0];

    z.l[0] = x.l[0] + y.l[0];           /* low order word sum  */
    z.l[1] = x.l[1] + y.l[1];           /* high order word sum */
    z.l[1] += (z.l[0] < maxVal) ? 1:0;  /* low-word overflow */

    return z;
}

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
)
{
    GT_U64  z;

    z.l[0] = x.l[0] - y.l[0];            /* low order word difference  */
    z.l[1] = x.l[1] - y.l[1];            /* high order word difference */
    z.l[1] -= (x.l[0] < y.l[0]) ? 1 : 0; /* low-word borrow            */

    return z;
}

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
)
{
    GT_U64  z; /* result   */
    GT_U32  v; /* value    */
    GT_U32  r; /* reminder */
    GT_U32  q; /* quotient */

    /* represent the 64-bit value as 4 16-bit figures number ABCD */
    /* divide it as manually by one 16-bit figure nymber Y        */

    /* begin with figures AB */
    v = x.l[1];
    q = v / y;
    r = v % y;

    /* save result */
    z.l[1] = q;

    /* including the figure C */
    v = ((r << 16) | ((x.l[0] >> 16) & 0xFFFF));
    q = v / y;
    r = v % y;

    /* save result */
    z.l[0] = (q << 16);

    /* including the last figure D, reminder not needed */
    v = ((r << 16) | (x.l[0] & 0xFFFF));
    q = v / y;

    /* save result */
    z.l[0] |= q;

    return z;
}

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
)
{
    GT_U32  v; /* value    */
    GT_U32  r; /* reminder */

    /* represent the 64-bit value as 4 16-bit figures number ABCD */
    /* devide it as manually by one 16-bit figure nymber Y        */

    /* begin with figures AB */
    v = x.l[1];
    r = v % y;

    /* including the figure C */
    v = ((r << 16) | ((x.l[0] >> 16) & 0xFFFF));
    r = v % y;

    /* including the last figure D */
    v = ((r << 16) | (x.l[0] & 0xFFFF));
    r = v % y;

    return (GT_U16)r;
}

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
)
{
    GT_U32 x0, x1, y0, y1, k, t;
    GT_U32 w1, w2, w3;
    GT_U64 z;

    x0 = x >> 16; x1 = x & 0xFFFF;
    y0 = y >> 16; y1 = y & 0xFFFF;

    t = x1*y1;
    w3 = t & 0xFFFF;
    k = t >> 16;

    t = x0*y1 + k;
    w2 = t & 0xFFFF;
    w1 = t >> 16;

    t = x1*y0 + w2;
    k = t >> 16;

    z.l[0] = (t << 16) + w3;
    z.l[1] = x0*y0 + w1 + k;;
    return z;
}

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
)
{
    if (number == 0)
        return GT_FALSE;

    if(((number | (number - 1)) - (number - 1)) == number)
        return GT_TRUE;
    else
        return GT_FALSE;
}

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
)
{
    /* Holds the most signifacant part of the remaining number. */
    GT_U32  msPart;
    /* Holds the least signifacant part of the remaining number. */
    GT_U32  lsPart;
    /* Mask to be used to get the lsPart from the current number. */
    GT_U32  lsMask;
    /* Number of bits to be shifted inorder to get the msPart from the current
       number. */
    GT_U32  msShift;
    /* Calculated log2(num). */
    GT_U32  res;

    if(prvCpssMathIsPowerOf2(num) == GT_FALSE)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    if(prvCpssMathIsPowerOf2(num) == GT_FALSE)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    res     = 0;
    lsMask  = 0xFFFF;
    msShift = 16;

    lsPart  = num & lsMask;
    msPart  = num >> msShift;

    while((msPart & 0x1) != 1)
    {
        if(lsPart != 0)
        {
            msShift = msShift / 2;
            lsMask  = lsMask >> msShift;
            msPart  = lsPart >> msShift;
            lsPart  = lsPart & lsMask;
        }
        else    /* msPart != 0 */
        {
            res     += msShift;
            msShift = msShift / 2;
            lsMask  = lsMask >> msShift;
            lsPart  = msPart & lsMask;
            msPart  = msPart >> msShift;
        }
    }

    res += msShift;
    *resultPtr = res;
    return GT_OK;
}

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
)
{
    GT_U8 i;

    if (exponent > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *result = 1;

    for (i = 1; i <= exponent; i++)
    {
        *result <<= 1;
    }
    return GT_OK;
}

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
)
{
    GT_U32 big_val;
    GT_U32 big_rem; /* remainder */
    GT_U32 ret_result;

    ret_result = value / divisor; /* Not rounded result */
    big_val = value * 1000;
    big_rem = (big_val / divisor) % 1000;
    if ( 500 <= big_rem )
    {
        ++ret_result;
    }
    *resultPtr = ret_result;
    return GT_OK;
}

