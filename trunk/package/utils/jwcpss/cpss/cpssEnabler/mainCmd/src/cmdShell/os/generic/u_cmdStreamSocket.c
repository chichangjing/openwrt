/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* u_cmdStreamSocket.c
*
* DESCRIPTION:
*       This file contains mainCmd socket stream implementation for UNIX OSes
*
* FILE REVISION NUMBER:
*       $Revision: 2$
*
*******************************************************************************/
#include <cmdShell/os/cmdStreamImpl.h>
#include <unistd.h>
#include <signal.h>

/* include generic code */
#include "cmdStreamSocket.c"

/*******************************************************************************
* cmdStreamSocketInit
*
* DESCRIPTION:
*       Initialize TCP/IP socket engine
*
* INPUTS:
*       None
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
GT_STATUS cmdStreamSocketInit(void)
{
    signal(SIGPIPE, SIG_IGN);
    return GT_OK;
}

/*******************************************************************************
* cmdStreamSocketFinish
*
* DESCRIPTION:
*       Close socket engine
*
* INPUTS:
*       None
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
GT_STATUS cmdStreamSocketFinish(void)
{
    return GT_OK;
}

