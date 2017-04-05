/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsInit.h
*
* DESCRIPTION:
*       File contains entry point routines
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __gtOsInith
#define __gtOsInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/************* Functions ******************************************************/

/*******************************************************************************
* osStartEngine
*
* DESCRIPTION:
*       Initialize mainOs engine, then start entryPoint routine
*       Need to be inplemented for Linux/FreeBSD
*
* INPUTS:
*       argC        - main() parameter
*       argV        - main() parameter
*       name        - task name
*       entryPoint  - entry point function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osStartEngine
(
    IN int argC,
    IN const char* argV[],
    IN const char* name,
    IN GT_VOIDFUNCPTR entryPoint
);

/*******************************************************************************
* osStopEngine
*
* DESCRIPTION:
*       Stop engine
*       Need to be inplemented for Linux/FreeBSD
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osStopEngine(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __gtOsInith */
