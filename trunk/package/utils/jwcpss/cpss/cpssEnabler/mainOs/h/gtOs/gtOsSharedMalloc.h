/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsSharedMalloc.h
*
* DESCRIPTION:
*       This file contains declarations for functions which allocate memory 
*       for run-time variables in shared memory.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#ifndef __gtOsSharedMalloch
#define __gtOsSharedMalloch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************* Includes *******************************************************/

#include <pthread.h>

#include <gtOs/gtGenTypes.h>

/************* Defines ********************************************************/

#ifdef SHARED_MEMORY
/* Use shared malloc for Shared Memory approach */
#   define SHARED_MALLOC_MAC shrMemSharedMalloc
#   define SHARED_FREE_MAC  shrMemSharedFree
#else
/* Use regular malloc/free without shared memory */
#   define SHARED_MALLOC_MAC malloc
#   define SHARED_FREE_MAC free
#endif

/************* Typedefs ********************************************************/
/*
* Typedef: struct SHARED_MEM_STC
*
* Description:
*	This structure presents addresses begin and end of section.
*
* Fields:
*	mutex	- mutex of shared memory
*	name	- name of shared memory
*   base	- base adress of shared memory
*   curr	- current adress of shared memory
*   last	- last adress of allocated block of shared memory
*   end	    - end of shared memory
*
*******************************************************************************/

typedef struct
{
    pthread_mutex_t mutex;
    char            *name;
    GT_UINTPTR      base;
    GT_UINTPTR      curr;
    GT_UINTPTR      last;
    GT_UINTPTR      end;
} SHARED_MEM_STC;

/************* Functions ******************************************************/
/*******************************************************************************
* shrMemMmapMemory
*
* DESCRIPTION:
*       Function maps file into specified address space.
*
* INPUTS:
*       base    - base address to be mapped to
*       size    - size of memory block to be mapped
*       offset  - offset into file to map from
*       fd      - file descriptor of file to be mapped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       Performs mapping with FIXED and SHARED flag, RW access rights.
*       Checks for validity (FIXED region should be mapped without address
*       changes).
*
*******************************************************************************/
GT_STATUS shrMemMmapMemory
(
    IN      GT_UINTPTR      base,
    IN      GT_U32          size,
    IN      GT_U32          offset,
    IN      GT_U32          fd
);

/*******************************************************************************
* shrMemSharedBufferInit
*
* DESCRIPTION:
*       Performes sharing of file as shared buffer.
*
* INPUTS:
*       namePtr - name of shared memory
*       base    - base adress of shred memory
*       size    - size of shared memory
*       fd      - file descriptor of buffer
*       doWrite - protected to write(not used yet)
*
* OUTPUTS:
*       pmemPtr - pointer to shared memory stucure to be updated
*
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       pmem CAN refer into head of buffer to be shared. This case
*       should be habled correctly.
*
*
*******************************************************************************/
GT_STATUS shrMemSharedBufferInit
(
    IN      char            *namePtr,
    IN      GT_UINTPTR      base,
    IN      GT_U32          size,
    IN      GT_32           fd,
    IN      GT_BOOL         doWrite,
    OUT     SHARED_MEM_STC  *pmemPtr
);

/*******************************************************************************
* shrMemSharedMallocIntoBlock
*
* DESCRIPTION:
*       Allocate memory into block
*
* INPUTS:
*       pmem    - shared memory stucture
*       fd      - descriptor of file to be extended if necessary 
*       size    - size of shared memory
*
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       void*   - Operation succeeded. Adress of allocated memory
*       NULL    - Operaton failed
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID *shrMemSharedMallocIntoBlock
(
    INOUT SHARED_MEM_STC    *pmem,
    IN    GT_U32            fd,
    IN    GT_U32            size
);

/*******************************************************************************
* shrMemSharedMalloc
*
* DESCRIPTION:
*       Allocate memory in size
*
* INPUTS:
*	size	- size of memory which needs to be allocated
*
* OUTPUTS:
*       None
*
* RETURNS:
*	GT_VOID *   - Operation succeeded. Adress of allocated memory
*   NULL        - Operaton failed
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID *shrMemSharedMalloc
(
    IN unsigned int size
);

/*******************************************************************************
* shrMemSharedFree
*
* DESCRIPTION:
*       Free memory allocated by shrMemSharedMalloc()
*
* INPUTS:
*       ptr - memory which needs to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID shrMemSharedFree
(
    IN GT_VOID *ptr
);

/*******************************************************************************
* shrMemInitSharedMallocInit
*
* DESCRIPTION:
*       Initialize shared library for processes
*
* INPUTS:
*       isFirstClient   - specify if client is the first client (CPSS enabler)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - Operation succeeded
*       GT_FAIL         - Operation failed
*       GT_NO_RESOURCE  - Operaton failed. No resource.
*
* COMMENTS:
*       Inits shared_malloc_fd descriptor (private for each process).
*
*******************************************************************************/
GT_STATUS shrMemSharedMallocInit
(
    IN GT_BOOL isFirstClient
);

/*******************************************************************************
* shrMemAllocStatic
*
* DESCRIPTION:
*       Allocates memory into shared buffer with a given name
*       If memory with such name already allocated then return
*       pointer to such object
*
* INPUTS:
*       memBlockName - the name for memory block
*       size         - size of memory which needs to be allocated
*
* OUTPUTS:
*       None
*
* RETURNS:
*       void*   - Operation succeeded. Adress of allocated memory
*       NULL    - Operaton failed
*
* COMMENTS:
*       Uses shrMemSharedMalloc()
*
*******************************************************************************/
GT_VOID *shrMemAllocStatic(
  IN const char*    memBlockName,
  IN unsigned int   size
);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __gtOsSharedMalloch */
