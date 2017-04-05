/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemSharedLibraryInit.c
*
* DESCRIPTION:
*       File contains implementation of init routines for
*       Shared Memory approach.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

/************* Includes *******************************************************/
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsGen.h>

#include <gtOs/gtOsSharedMemory.h>
#include <gtOs/gtOsSharedMemoryRemapper.h>
#include <gtOs/gtOsSharedMalloc.h>
#include <gtOs/gtOsSharedPp.h>

char* shrAddNameSuffix(const char* name, char* buffer, int bufsize);
GT_STATUS shmDoCheckOverlap(void);

/************* Functions ******************************************************/
/************* Public Functions ***********************************************/
/*TODO: keep for compatibility */
static int already_initialized = 0;
/*******************************************************************************
* shrMemInitSharedLibrary_FirstClient
*
* DESCRIPTION:
*       Initialize shared library for the first process (CPSS Enabler process)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       First process should perform additional steps to initialize all
*       shared buffers.
*
*       See also shrMemInitSharedLibrary function for non-first clients.
*
*******************************************************************************/
GT_STATUS shrMemInitSharedLibrary_FirstClient(GT_VOID)
{
    if (already_initialized)
        return GT_OK;

    /* 1st step - unlink old shared buffer.
     * It remains after old instances of CPSS Library. */
    /* Not required? (Truncate it on creation!)
    if(shrMemUnlinkShmObj() == GT_FAIL)
    {
        fprintf(stderr, "shrMemUnlinkShmObj() : error\n");
        return GT_FAIL;
    }
    */
    
    /* 2nd step - create shared buffer and map it instead private BSS/DATA */
    if(shrMemDoShmMap(/*isFirstCLient=*/GT_TRUE) == GT_FAIL)
    {
        fprintf(stderr, "shrMemDoShmMap() : error\n");
        return GT_FAIL;
    }

    /* 3rd step - create and initialize and map shared buffer for osMalloc */
    if (shrMemSharedMallocInit(/*isFirstCLient=*/GT_TRUE) != GT_OK)
    {
        fprintf(stderr, "shrMemSharedMallocInit() : error\n");
        return GT_FAIL;
    }

#ifndef ASIC_SIMULATION /* Applicable only for BM */

    /* 4th step - initialize shared buffer for DMA with PP */
    if (shrMemSharedPpInit(/*isFirstCLient=*/GT_TRUE) != GT_OK)
    {
        fprintf(stderr, "shrMemSharedPpInit() : error\n");
        return GT_FAIL;
    }
#endif

    return GT_OK;
}

/*******************************************************************************
* shrMemInitSharedLibrary
*
* DESCRIPTION:
*       Initialize shared library for non-first processes.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       This routine assumes that all shared buffers are already created,
*       so we can just use them.
*
*       Note - shrMemInitSharedLibrary_FirstClient routine HAVE to be called
*       before by the first client to create shared buffers.
*
*******************************************************************************/
GT_STATUS shrMemInitSharedLibrary(GT_VOID)
{
    if (already_initialized)
        return GT_OK;
    /* 1ts step - map already created shared buffer instead private BSS/DATA */
    if(shrMemDoShmMap(/*isFirstCLient=*/GT_FALSE) == GT_FAIL)         
    {
        fprintf(stderr, "shrMemDoShmMap() : error\n");
        return GT_FAIL;
    }

    /* 2nd step - map already created shared buffer for osMalloc */
    if (shrMemSharedMallocInit(/*isFirstCLient=*/GT_FALSE) != GT_OK)
    {
        fprintf(stderr, "shrMemSharedMallocInit() : error\n");
        return GT_FAIL;
    }

#ifndef ASIC_SIMULATION /* Applicable ONLY for BM */

    /* 3rd step - map already created shared buffer for DMA with PP
     * to processes address space. */
    if (shrMemSharedPpInit(/*isFirstCLient=*/GT_FALSE) != GT_OK)
    {
        fprintf(stderr, "shrMemSharedPpInit() : error\n");
        return GT_FAIL;
    }
#endif

    return GT_OK;
}

#ifndef ASIC_SIMULATION /* Applicable ONLY for BM */
extern GT_STATUS extDrvInitDrv(OUT GT_BOOL  *isFirstClient);
#endif

__attribute__((constructor))
static void libhelper_constructor()
{
    GT_BOOL     first = GT_FALSE;
    GT_STATUS   rc;

    shmDoCheckOverlap();
#ifndef ASIC_SIMULATION /* Applicable ONLY for BM */
    rc = extDrvInitDrv(&first);
    if (rc != GT_OK)
    {
        fprintf(stderr, "extDrvInitDrv() failed, rc=%d\n", rc);
        return;
    }
#else
    {
        char buf[256];
        /* Open named semaphore to guard remapping code from other clients. */
        shrAddNameSuffix(SHARED_MEMORY_DATA_CNS, buf, sizeof(buf));
        first = (access(buf, R_OK|W_OK) == 0) ? GT_FALSE : GT_TRUE;
    }
#endif
    if (first == GT_TRUE)
    {
        rc = shrMemInitSharedLibrary_FirstClient();
        if (rc != GT_OK)
        {
            fprintf(stderr, "shrMemInitSharedLibrary_FirstClient() failed, rc=%d\n",rc);
        }
    }
    else
    {
        rc = shrMemInitSharedLibrary();
        if (rc != GT_OK)
        {
            fprintf(stderr, "shrMemInitSharedLibrary() failed, rc=%d\n",rc);
        }
    }
    if (rc == GT_OK)
        already_initialized = 1;
}
