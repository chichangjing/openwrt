/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsSharedMemory.h
*
* DESCRIPTION:
*       This file contains init routines for Shared Memory approach.
*       Intt functions should be called BEFORE any other work with
*       with CPSS Shared Library.
*
* FILE REVISION NUMBER:
*       $Revision: 1.4 $
*
*******************************************************************************/
#ifndef __gtOsSharedMemoryh
#define __gtOsSharedMemoryh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/************* Functions ******************************************************/
/*******************************************************************************
* shrMemInitSharedLibrary 
*
* DESCRIPTION:
*       Initialize shared library for processes
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*   GT_OK	- Operation succeeded
*	GT_FAIL	- Operaton failed
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS shrMemInitSharedLibrary
(
    GT_VOID
);

/*******************************************************************************
* shrMemInitSharedLibrary_FirstClient
*
* DESCRIPTION:
*       Initialize shared library for first process
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK	- Operation succeeded
*       GT_FAIL	- Operaton failed
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS shrMemInitSharedLibrary_FirstClient
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __gtOsSharedMemoryh */
