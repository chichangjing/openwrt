/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* osLinuxGen.c
*
* DESCRIPTION:
*       Operating System wrapper general APIs implementation
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/


/************* Includes *******************************************************/
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsSharedPp.h>
#include <gtOs/gtOsSharedData.h>

#include <gtUtil/gtBmPool.h>

#ifndef SHARED_MEMORY
/* Required for shrMemSharedPpInit */
#   include <gtOs/gtOsSharedPp.h>
#endif


/*************** Globals ******************************************************/
static GT_BOOL gtOsWrapperOpen __SHARED_DATA_MAINOS = GT_FALSE;


/*******************************************************************************
* osWrapperOpen
*
* DESCRIPTION:
*       Initialize OS wrapper facility.
*
* INPUTS:
*       osSpec  - ptr to OS specific init parameter(s)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_ALREADY_EXIST - wrapper was already open
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osWrapperOpen(void * osSpec)
{
    (void)osSpec;
    if (gtOsWrapperOpen == GT_TRUE)
    {
        return GT_OK;
    }
    else
    {
#if (!defined SHARED_MEMORY && !defined LINUX_SIM)
        /*  Open the packet processor device driver. 
            NOTE - share DMA buffer here only wihtout Shared Memory approach!
            Otherwise it should be initialized
            into shrMemInitSharedLibrary[_FirstClient] */
        if (shrMemSharedPpInit(GT_TRUE) != GT_OK)
        {
            return GT_FAIL;
        }

#endif
        /* Must be set to GT_TRUE before use any of OS functions. */
        gtOsWrapperOpen = GT_TRUE;

        gtPoolInit( 500 );
    }

    return GT_OK;
}

/*******************************************************************************
* osWrapperClose
*
* DESCRIPTION:
*       Close OS wrapper facility and free all used resources.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osWrapperClose(void)
{
    IS_WRAPPER_OPEN_STATUS;

    return GT_OK;
}

/*******************************************************************************
* osWrapperIsOpen
*
* DESCRIPTION:
*       Returns GT_TRUE if OS wrapper facility was initialized .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE          - on success
*       GT_FALSE         - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_INLINE GT_BOOL osWrapperIsOpen(void)
{
    return gtOsWrapperOpen;
}


