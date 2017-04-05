/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdShell.h
*
* DESCRIPTION:
*       This file contains OS shell function
*
* FILE REVISION NUMBER:
*       $Revision: 1$
*
*******************************************************************************/

#ifndef __cmdShellh
#define __cmdShellh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cmdShell/common/cmdCommon.h>
#include <extUtils/IOStream/IOStream.h>


/*******************************************************************************
* cmdOsShell
*
* DESCRIPTION:
*       Start OS shell
*       It is assumed that stdout is redirected to current stream
*
* INPUTS:
*       IOStream  - I/O stream
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdOsShell(IOStreamPTR IOStream);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ifndef __cmdShellh */
