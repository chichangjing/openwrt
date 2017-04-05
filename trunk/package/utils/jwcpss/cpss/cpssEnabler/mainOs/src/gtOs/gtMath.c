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
* gtMath64.c
*
* DESCRIPTION:
*       Implementation of 64 bits math functions.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/

#include <gtOs/gtGenTypes.h>

/*******************************************************************************
* mathAdd64
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
GT_U64  mathAdd64
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
* mathIsPowerOf2
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
GT_BOOL mathIsPowerOf2
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
* mathLog2
*
* DESCRIPTION:
*       This function calculates log(x) by base 2.
*
* INPUTS:
*       num - The number to perform the calculation on, This number must be a
*             power of 2.
*
* OUTPUTS:
*       result  - log(num) result.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise (if the number is not a power of 2).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS mathLog2
(
    IN  GT_U32  num,
    OUT GT_U32  *result
)
{
    GT_U32  msPart;                 /* Holds the most signifacant part of   */
                                    /* the remaining number.                */
    GT_U32  lsPart;                 /* Holds the least signifacant part of  */
                                    /* the remaining number.                */
    GT_U32  lsMask;                 /* Mask to be used to get the lsPart    */
                                    /* from the current number.             */
    GT_U32  msShift;                /* Number of bits to be shifted inorder */
                                    /* to get the msPart from the current   */
                                    /* number.                              */
    GT_U32  res;                    /* Calculated log2(num).                */

    if(mathIsPowerOf2(num) == GT_FALSE)
        return GT_FAIL;

    if(mathIsPowerOf2(num) == GT_FALSE)
        return GT_FAIL;

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

    res     += msShift;
    *result = res;
    return GT_OK;
}

/*******************************************************************************
* roundDiv
*
* DESCRIPTION:
*   Divide and round a value with accuracy of 3 digits.
*
* INPUTS:
*   value   - Value to divide.
*   divisor - the divisor.
*
* OUTPUTS:
*   res   - result after round
*
* RETURNS :
*   GT_OK
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS roundDiv
(
IN  GT_U32  value,
IN  GT_U32   divisor,
OUT GT_U32 * res
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
    *res = ret_result;
    return GT_OK;
} /* dfcdlRoundDiv */

