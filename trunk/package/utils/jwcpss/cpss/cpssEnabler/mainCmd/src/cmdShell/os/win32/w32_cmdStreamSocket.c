/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* w32_cmdStreamSocket.c
*
* DESCRIPTION:
*       This file contains mainCmd socket stream implementation for Win32
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

/* WA to avoid next warning :
   due to include to : #include <windows.h>
    c:\program files\microsoft visual studio\vc98\include\rpcasync.h(45) :
    warning C4115: '_RPC_ASYNC_STATE' : named type definition in parentheses
*/
struct _RPC_ASYNC_STATE;

#include <cmdShell/os/cmdStreamImpl.h>
#include <windows.h>
#include <winsock.h>
#include <gtOs/gtGenTypes.h>

/* include generic code */
#include "../generic/cmdStreamSocket.c"

/***** Global variables ************************************************/
static GT_BOOL osExInitialized = GT_FALSE;
/***** Initialization **************************************************/


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
    WSADATA wsaData;

    /* initialize only once */
    if (!osExInitialized)
    {
        /* initialize winsock application interface */
        if (WSAStartup(0x202, &wsaData) != 0)
        {
            WSACleanup();
            return GT_FAIL;
        }
        osExInitialized = GT_TRUE;
    }
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
    if (osExInitialized)
    {
        WSACleanup();
        osExInitialized = GT_FALSE;
    }

    return GT_OK;
}
