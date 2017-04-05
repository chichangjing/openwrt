/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemSharedIPC.c
*
* DESCRIPTION:
*       This file contains common data for OS/SharedMemory modules
*       which should be shared between different processes, thus should
*       be linked into libcpss.so (only that object
*       has shared BSS/DATA sections).
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#include <gtOs/gtEnvDep.h>
#include <gtOs/gtOsSharedMalloc.h>
#include <gtOs/gtOsSharedPp.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsSharedIPC.h>


#ifdef SHARED_MEMORY


/******** implementation **********************************/
typedef struct {
    GT_SEM      semMutex;
    GT_SEM      semCommand;
    GT_SEM      semDone;
    enum {
        STATE_NONE_E,
        STATE_WAIT_E,
        STATE_INPROGRESS_E
    }           state;
    GT_STATUS   rc;
    CPSS_MP_REMOTE_COMMAND command;
} CPSS_MP_REMOTE;

static CPSS_MP_REMOTE remoteArray[CPSS_MP_REMOTE_MAX_E]; /* will be initialized with zeroes */
const char* remoteNames[CPSS_MP_REMOTE_MAX_E] = {
    "RxTx",
    "FdbLearning"
};

/*******************************************************************************
* cpssMultiProcComExecute
*
* DESCRIPTION:
*       
*	Used to execute function in another process
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_MP_REMOTE_COMMAND     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComExecute
(
	CPSS_MP_REMOTE_COMMAND *paramPtr
)
{
    CPSS_MP_REMOTE  *mp;
    GT_STATUS       rc;

    /* check parameters */
    if (!paramPtr)
        return GT_BAD_PARAM;

    if (paramPtr->remote >= CPSS_MP_REMOTE_MAX_E)
        return GT_BAD_VALUE;

    mp = &(remoteArray[paramPtr->remote]);

    if (!mp->semMutex) {
        /* not initialized yet.
         * application not started/doesn't listen for commands
         */
        return GT_NOT_INITIALIZED;
    }

    /* Only one command at time for each application */
    osSemWait(mp->semMutex, OS_WAIT_FOREVER);

    /* store command parameters */
    osMemCpy(&(mp->command), paramPtr, sizeof(CPSS_MP_REMOTE_COMMAND));
    /* signal to application */
    osSemSignal(mp->semCommand);
    /* wait till command done */
    osSemWait(mp->semDone, OS_WAIT_FOREVER);
    /* handle timeout/failure */

    /* Command result */
    rc = mp->rc;
    osMemCpy(paramPtr, &(mp->command), sizeof(CPSS_MP_REMOTE_COMMAND));

    osSemSignal(mp->semMutex);

    return rc;
}

/*******************************************************************************
* cpssMultiProcComWait
*
* DESCRIPTION:
*       
*	Used to to wait for command in another process
*
* INPUTS:
*       program  - program identifier which waits for command
*       
*
* OUTPUTS:
*  		paramPtr - pointer to struct CPSS_MP_REMOTE_COMMAND     
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComWait
(
    IN  CPSS_MP_REMOTE_DEST    program,
	OUT CPSS_MP_REMOTE_COMMAND *paramPtr
)
{
    CPSS_MP_REMOTE *mp;

    /* check parameters */
    if (program >= CPSS_MP_REMOTE_MAX_E)
        return GT_BAD_PARAM;

    if (paramPtr == NULL)
        return GT_BAD_PARAM;

    mp = &(remoteArray[program]);
    if (!mp->semMutex) {
        /* not initialized yet */
        char name[64];

        osSprintf(name, "MP_%s_Cmd", remoteNames[program]);
        if (osSemBinCreate(name, OS_SEMB_EMPTY, &(mp->semCommand)) != GT_OK)
        {
            return GT_FAIL;
        }
        osSprintf(name, "MP_%s_Done", remoteNames[program]);
        if (osSemBinCreate(name, OS_SEMB_EMPTY, &(mp->semDone)) != GT_OK)
        {
            osSemDelete(mp->semCommand);
            return GT_FAIL;
        }
        osSprintf(name, "MP_%s_Mtx", remoteNames[program]);
        if (osSemBinCreate(name, OS_SEMB_FULL, &(mp->semMutex)) != GT_OK)
        {
            osSemDelete(mp->semCommand);
            osSemDelete(mp->semDone);
            return GT_FAIL;
        }
        mp->state = STATE_NONE_E;
    }

    mp->rc = GT_OK;
    mp->state = STATE_WAIT_E;
    /* wait for command */
    osSemWait(mp->semCommand, OS_WAIT_FOREVER);
    /* handle fail & timeout */

    /* semaphore signalled, parameters available */
    osMemCpy(paramPtr, &(mp->command), sizeof(CPSS_MP_REMOTE_COMMAND));

    mp->state = STATE_INPROGRESS_E;

    return GT_OK;
}


/*******************************************************************************
* cpssMultiProcComComplete
*
* DESCRIPTION:
*       
*	Used to confirm remote command finished
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_MP_REMOTE_COMMAND     
*       rc       - return code
*       
* OUTPUTS:
*       none
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComComplete
(
	IN  CPSS_MP_REMOTE_COMMAND *paramPtr,
	IN  GT_STATUS              rc
)
{
    CPSS_MP_REMOTE *mp;

    /* check parameters */
    if (!paramPtr)
        return GT_BAD_PARAM;

    if (paramPtr->remote >= CPSS_MP_REMOTE_MAX_E)
        return GT_BAD_VALUE;

    mp = &(remoteArray[paramPtr->remote]);
    if (!mp->semMutex) {
        /* not initialized yet, bad usage */
        return GT_NOT_INITIALIZED;
    }
    if (mp->state != STATE_INPROGRESS_E)
        return GT_FAIL;

    /* copy return values */
    osMemCpy(&(mp->command), paramPtr, sizeof(CPSS_MP_REMOTE_COMMAND));
    mp->rc = rc;
    mp->state = STATE_NONE_E;

    /* signal to caller */
    osSemSignal(mp->semDone);
    return GT_OK;
}

#endif

