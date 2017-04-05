/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* vx_cmdStreamSocket.c
*
* DESCRIPTION:
*       This file contains mainCmd socket stream implementation for vxWorks
*
* FILE REVISION NUMBER:
*       $Revision: 3$
*
*******************************************************************************/
#include <ioLib.h>
#include <sioLib.h>
#include <ttyLib.h>
#include <sysLib.h>
#include <selectLib.h>
#include <string.h>
#include <cmdShell/os/cmdStreamImpl.h>

/* include generic code */
#include "../generic/cmdStreamSocket.c"

/***** Global variables ************************************************/
static GT_BOOL osExInitialized = GT_FALSE;

extern int  taskNameToId (char *name);

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
    /* initialize only once */
    int tid = 0;
    GT_U32 old_priority;

    if (!osExInitialized)
    {
        osExInitialized = GT_TRUE;
        tid = taskNameToId("tNetTask");
        if (tid != 0)
        {    /* same as the keep alive task of  the terminal*/
            if (cmdOsSetTaskPrior(tid,STREAM_THREAD_PRIO-1,&old_priority)!= GT_OK)
            {
                return GT_FAIL;
            }
        } 
        else
        {
            return GT_FAIL;
        }
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
        osExInitialized = GT_FALSE;

    return GT_OK;
}

static int socketGrabStd(cmdStreamPTR streamP)
{
    socketStreamPTR stream = (socketStreamPTR)streamP;

    if (!stream)
        return -1;

    /* ioGlobalStdSet(STD_IN, stream->socket); */
    ioGlobalStdSet(STD_OUT, stream->socket);
    ioGlobalStdSet(STD_ERR, stream->socket);

    return 1;
}
