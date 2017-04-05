/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* appDemoDb.h
*
* DESCRIPTION:
*       App demo database.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#ifndef __appDemoDbh
#define __appDemoDbh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>


/*******************************************************************************
* appDemoDbEntryAdd
*
* DESCRIPTION:
*       Set AppDemo DataBase value.This value will be considered during system
*       initialization process.
*
* INPUTS:
*       namePtr         - points to parameter name
*       value           - parameter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - if name is too long
*       GT_BAD_PTR      - if NULL pointer
*       GT_NO_RESOURCE  - if Database is full
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDbEntryAdd
(
    IN  GT_CHAR *namePtr,
    IN  GT_U32  value
);


/******************************************************************************
* appDemoDbEntryGet
*
* DESCRIPTION:
*       Get parameter value from AppDemo DataBase.
*
* INPUTS:
*       namePtr         - points to parameter name
*
* OUTPUTS:
*       valuePtr        - points to parameter value
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - if name is too long
*       GT_BAD_PTR      - if NULL pointer
*       GT_NO_SUCH      - there is no such parameter in Database
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDbEntryGet
(
    IN  GT_CHAR *namePtr,
    OUT GT_U32  *valuePtr
);


/******************************************************************************
* appDemoDbDump
*
* DESCRIPTION:
*       Dumps entries set in AppDemo database to console.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDbDump
(
    GT_VOID
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoDbh */

