/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* IOStreamImpl.h
*
* DESCRIPTION:
*       This file contains types and routines for mainCmd I/O streams
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __IOStreamImplh
#define __IOStreamImplh

#include <extUtils/IOStream/IOStream.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* IOStreamCreateSocket
*
* DESCRIPTION:
*       Create socket stream
*
* INPUTS:
*       socket - socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - socket stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR IOStreamCreateSocket
(
    IN CPSS_SOCKET_FD socket
);

/*******************************************************************************
* IOStreamCreateTelnet
*
* DESCRIPTION:
*       Create Telnet protocol stream
*
* INPUTS:
*       socket - socket I/O stream
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - socket stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR IOStreamCreateTelnet
(
    IN IOStreamPTR socket
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __IOStreamImplh */
