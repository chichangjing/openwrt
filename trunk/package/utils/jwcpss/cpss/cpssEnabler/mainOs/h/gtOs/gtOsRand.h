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
* gtOsRand.h
*
* DESCRIPTION:
*       Operating System wrapper. Random facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __gtOsRandh
#define __gtOsRandh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/************* Functions ******************************************************/
/*******************************************************************************
* osRand
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
GT_32 osRand(void);

/*******************************************************************************
* osSrand
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
void osSrand
(
    IN GT_U32 seed
);


#ifdef __cplusplus
}
#endif

#endif  /* __gtOsRandh */
/* Do Not Add Anything Below This Line */

