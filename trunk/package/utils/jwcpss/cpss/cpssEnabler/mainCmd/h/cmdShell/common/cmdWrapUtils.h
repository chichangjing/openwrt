/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdWrapUtils.h
*
* DESCRIPTION:
*       Commander Wrappers utility function implementation
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __cmdWrapUtils_h__
#define __cmdWrapUtils_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Include files ***************************************************/
#include <cmdShell/common/cmdCommon.h>

/*******************************************************************************
* strDuplicate
*
* DESCRIPTION:
*       Cyclic string duplicator
*
* INPUTS:
*       src - Null terminated string
*
* OUTPUTS:
*       none
*
* RETURNS:
*       Null terminated string
*
* COMMENTS:
*
*******************************************************************************/
GT_CHAR* strDuplicate
(
    IN const GT_CHAR *src
);

/*******************************************************************************
* galtisOutput
*
* DESCRIPTION:
*       builds galtis output string
*
* INPUTS:
*       status          - status that will be added to resultString
*       format          - the incoming arguments format (%d%c%x%s...). The %f
*                         can be used for adding a pre-prepared fields output
*                         string (by using fieldOutput function)
*       ...             - list of arguments to put in the resultString. For %f
*                         format no argument is needed
*
* OUTPUTS:
*       resultString    - the result string to write to
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*       the output looks like: @@@status!!!o1!!!o2!!!o3!!! ... on!!!###
*
*******************************************************************************/
GT_VOID galtisOutput
(
    OUT GT_8 *resultString,
    IN GT_STATUS status,
    IN GT_CHAR *format,
    IN ...
);

/*******************************************************************************
* fieldOutput
*
* DESCRIPTION:
*       builds table field output from input to the internal field output
*       string
*
* INPUTS:
*       format  - the incoming arguments format (%d%c%x%s...)
*       ...     - list of arguments to put in the fields output string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*       the output looks like: f1???f2!!!f3!!!f4!!! ... fn???
*
*******************************************************************************/
GT_VOID fieldOutput
(
    IN GT_CHAR* format,
    IN ...
);

/*******************************************************************************
* fieldOutputSetAppendMode
*
* DESCRIPTION:
*       causes fieldOutput to continue the output string
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID fieldOutputSetAppendMode
(
    GT_VOID
);

/*******************************************************************************
* genericTableGetLast
*
* DESCRIPTION:
*   The generic table "GetLast" function.
*   Used for one-line tables as GetNext function that notifies Galtis GUI
*   that the table has no lines more.
*
* APPLICABLE DEVICES:  All Devices
*
* INPUTS:
*       standard arguments of Galtis wrapper
*
* OUTPUTS:
*       standard arguments of Galtis wrapper
*       None

* RETURNS:
*       GT_OK          - on success any case
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
CMD_STATUS genericTableGetLast
(
    IN  GT_UINTPTR inArgs[],
    IN  GT_UINTPTR inFields[],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[]
);

/*******************************************************************************
* genericDummyCommand
*
* DESCRIPTION:
*   The generic dummy command.
*   Only notifies Galtis GUI the successfull execution of the command
*   Used for tables as EndSet function that at the case when
*   all real API calls done in SetFirst and SetNext functions.
*
* APPLICABLE DEVICES:  All Devices
*
* INPUTS:
*       standard arguments of Galtis wrapper
*
* OUTPUTS:
*       standard arguments of Galtis wrapper
*       None

* RETURNS:
*       GT_OK          - on success any case
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
CMD_STATUS genericDummyCommand
(
    IN  GT_UINTPTR inArgs[],
    IN  GT_UINTPTR inFields[],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[]
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdWrapUtils_h__ */

