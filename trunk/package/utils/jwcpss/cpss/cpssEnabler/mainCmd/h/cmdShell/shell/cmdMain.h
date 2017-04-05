/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdMain.h
*
* DESCRIPTION:
*       command shell entry point implementation
*
* DEPENDENCIES:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 2$
*
*******************************************************************************/
#ifndef __cmdMainh
#define __cmdMainh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>
#if (defined PSS_PRODUCT) || (defined CPSS_PRODUCT)
    #include <cmdShell/shell/cmdMultiTapi.h>
#else
    #include <cpss/generic/cpssTypes.h>
#endif

/*******************************************************************************
* cmdInit
*
* DESCRIPTION:
*       commander initialization (entry point); this function spawns
*       configured streaming threads and immediately returns to caller.
*
* INPUTS:
*       devNum  - The device number of the serial port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_NO_RESOURCE     - failed to allocate memory for socket
*       GT_FAIL            - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdInit
(
    IN  GT_U32  devNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdMainh */
