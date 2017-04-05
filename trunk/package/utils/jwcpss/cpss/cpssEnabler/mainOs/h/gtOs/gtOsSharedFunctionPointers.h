/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsSharedFunctionPointers.h
*
* DESCRIPTION:
*       This file contains bind routines for ARM architecture to avoid
*       pointer issue. Work-around uses dlsym system call to avoid
*       private addresses to functions.  
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __gtOsSharedFunctionPointersh
#define __gtOsSharedFunctionPointersh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/************* Functions ******************************************************/

/*******************************************************************************
* shrMemGetDefaultOsBindFuncsThruDynamicLoader
*
* DESCRIPTION:
*       Receives default cpss bind from OS
* INPUTS:
*       None.
* OUTPUTS:
*       osFuncBindPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Uses dlsym system call to overcome private-pointer-to-func issue
*       observed for ARM GnuEABI.
*
*******************************************************************************/
GT_STATUS shrMemGetDefaultOsBindFuncsThruDynamicLoader(
    OUT CPSS_OS_FUNC_BIND_STC *osFuncBindPtr
);

/*******************************************************************************
* cpssGetDefaultExtDrvFuncs
*
* DESCRIPTION:
*       Receives default cpss bind from extDrv
* INPUTS:
*       None.
* OUTPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Function should reside into CPSS library to resolve correct
*       pointers to functions.
*
*******************************************************************************/
GT_STATUS shrMemGetDefaultExtDrvFuncs
(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC  *extDrvFuncBindInfoPtr
);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __gtOsSharedFunctionPointersh */
