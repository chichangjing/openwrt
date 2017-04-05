/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssOsRand.h
*
* DESCRIPTION:
*       Operating System wrapper. Random facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssOsRandh
#define __cpssOsRandh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/************* Functions ******************************************************/
/*******************************************************************************
* CPSS_OS_RAND_FUNC
*
* DESCRIPTION:
*       Generates a pseudo-random integer between 0 and RAND_MAX
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       rValue    - pseudo-random integer
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_OS_RAND_FUNC)
(
    void
);

/*******************************************************************************
* CPSS_OS_SRAND_FUNC
*
* DESCRIPTION:
*       Reset the value of the seed used to generate random numbers.
*
* INPUTS:
*       seed  - random number seed .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef void (*CPSS_OS_SRAND_FUNC)
(
    IN GT_U32 seed
);

/* CPSS_OS_RAND_BIND_STC -
   structure that hold the "os random numbers" functions needed be bound to cpss

*/
typedef struct{
    CPSS_OS_RAND_FUNC      osRandFunc;
    CPSS_OS_SRAND_FUNC     osSrandFunc;

}CPSS_OS_RAND_BIND_STC;

#ifdef __cplusplus
}
#endif

#endif  /* __cpssOsRandh */


