/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemCommonMem.c
*
* DESCRIPTION:
*       This file contains common routines for shared memory management.
*       It should be used even without shared memory model to share some
*       buffers (for example, DMA buffer for BM).
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

/************* Includes *******************************************************/
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSharedMalloc.h>

#define PRV_SHARED_BUFFER_INCREMENT_SIZE_CNS (4*1024)

/************* Functions ******************************************************/
/************* Static Functions ***********************************************/

/*******************************************************************************
* extendBufferByOnePage 
* 
* DESCRIPTION:
*       Function extends buffer described by fd by one page.
*
* INPUTS:
*       fd	- file descriptor of buffer    
*       isAbsoluteOffset - methos of seeking: absolute (from the begin of file
*           or relative).
*
* OUTPUTS:
*       pmemPtr	- pointer to shared memory stucure to be updated
*       (only 'last' field).    
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed 
*
* COMMENTS:
*       Page can have any size multiple to OS page (4096 for Linux).
*       Relative seeking MUST be used into init stage (to avoid dereference
*       of pmem BEFORE write, as pmem can points to memory to be allocated).
*       Absolute seeking must be used in other cases (as different client
*       has different file descriptors, they have different current file
*       position: so we should use absolute seeking).
*       Page is appended to the end of shared file.
*
*******************************************************************************/
static GT_STATUS extendBufferByOnePage
(
    IN  GT_U32         fd,
    IN  GT_BOOL        isAbsoluteOffset,
    OUT SHARED_MEM_STC *pmemPtr
)
{
    const GT_U32 deadbeaf = 0xdeadbeaf;
    off_t res;
    
    if(isAbsoluteOffset)
    {
        /* !! NOTE - access to pmemPtr BEFORE write.
            Unacceptable for init stage!*/
        res = lseek(fd,
                    pmemPtr->last - pmemPtr->base +
                    PRV_SHARED_BUFFER_INCREMENT_SIZE_CNS - sizeof(deadbeaf),
                    SEEK_SET);
    }
    else
    {
        /* Related seek, acceptable for init stage */
        res = lseek(fd,
                    PRV_SHARED_BUFFER_INCREMENT_SIZE_CNS - sizeof(deadbeaf),
                    SEEK_CUR);
    }
        
    if(res == -1)
    {
        fprintf(stderr, "lseek() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Write DWORD into the end of unexistent page will cause
     * to file expanding.  */
    if(write(fd, &deadbeaf, sizeof(deadbeaf)) == -1 )
    {
        fprintf(stderr, "write() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* !! NOTE - access to pmemPtr AFTER write, acceptible anyway. */
    pmemPtr->last += PRV_SHARED_BUFFER_INCREMENT_SIZE_CNS;

    /*fprintf(stderr, "pmemPtr->last = 0x%x\n", pmemPtr->last);*/

    return GT_OK;
}

/************* Public Functions ***********************************************/

/*******************************************************************************
* shrMemMmapMemory
* 
* DESCRIPTION:
*       Function maps file into specified address space.
*
* INPUTS:
*       base - base address to be mapped to
*       size - size of memory block to be mapped
*       offset - offset into file to map from 
*       fd	- file descriptor of file to be mapped
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
)
{
  GT_VOID *mmaped_base = mmap((GT_VOID *)base, size,
                              PROT_READ | PROT_WRITE,
                              MAP_FIXED | MAP_SHARED,
                              fd, offset);
  
    /* !! NOTE - pmem should refer to area inside base,
        so don't write result of mmap directly to pmem->base !!!*/ 

    if (MAP_FAILED == mmaped_base)
    {
        fprintf(stderr,"mmap failed errno(%s).\n", strerror(errno));
        return GT_FAIL;
    }

    if(mmaped_base != (GT_VOID *)base)
    {
        fprintf(stderr,"mmap return changed MAP_FIXED address\n");
        return GT_FAIL;
    }

    return GT_OK;
}

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
extern GT_U32 dmaSize;
GT_STATUS shrMemSharedBufferInit
(
    IN      char            *namePtr,
    IN      GT_UINTPTR      base,
    IN      GT_U32          size,
    IN      GT_32           fd,
    IN      GT_BOOL         doWrite,
    OUT     SHARED_MEM_STC  *pmemPtr
)
{
    pthread_mutexattr_t mattr;

    /* Map memory to address space */
#ifndef _FreeBSD /* no need for mmap in FreeBSD */
    if (fd >= 0)
    {
        if(shrMemMmapMemory(base, size, 0, fd) != GT_OK)
            return GT_FAIL;
    }
    /* else it is already mapped */
#endif 
    /* Here we handle special trick when management structure SHARED_MEM_STC is
     * stored into the subordinary shared buffer. */
    if((GT_UINTPTR)pmemPtr == base) /* Want to place info struct into block? */
    {
        /* extend file by one page to fit SHARED_MEM_STC header! */
        if(extendBufferByOnePage(fd, GT_FALSE, pmemPtr) == GT_FAIL)
        {
            fprintf(stderr,"extendBufferByOnePage() failed\n");
            return GT_FAIL;
        }
    }

    /* Now we can refer to pmemPtr */
    /* Store varables into management structure. */
    pmemPtr->base = base;
    pmemPtr->name = namePtr;
    pmemPtr->curr = pmemPtr->base;
    if((GT_UINTPTR)pmemPtr == base)
    {

        pmemPtr->curr += sizeof(*pmemPtr); /* reserve area for SHARED_MEM_STC header */
#ifdef OS_MEM_64_BIT_ALIGNED
        /* align 64bit */
        pmemPtr->curr = (pmemPtr->curr + 7) & (~((GT_UINTPTR)7));
#endif
    }
    pmemPtr->end  = pmemPtr->base + size;

    if (doWrite)
        pmemPtr->last = pmemPtr->base + PRV_SHARED_BUFFER_INCREMENT_SIZE_CNS;
    else
        pmemPtr->last = pmemPtr->end;
    
    /* SHARED_MEMORY is 1 here, don't check it */

    /* Init mutex to guard buffer allocation. */
    pthread_mutexattr_init( &mattr );
    pthread_mutexattr_setpshared( &mattr, PTHREAD_PROCESS_SHARED );
    pthread_mutex_init(&pmemPtr->mutex, &mattr);

    return GT_OK;
}

/*******************************************************************************
* shrMemSharedMallocIntoBlock
*
* DESCRIPTION:
*       Allocate memory into specified block
*
* INPUTS:
*       pmemPtr - pointer to shared memory management stucture
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
    INOUT SHARED_MEM_STC    *pmemPtr,
    IN    GT_U32            fd,
    IN    GT_U32            size
)
{
    GT_VOID * address;
    GT_UINTPTR newEnd;

    /* Guard management structure */
    pthread_mutex_lock(&pmemPtr->mutex);

    /*fprintf(stderr,"shrMemSharedMallocIntoBlock: allocate %d bytes\n", size);*/
    
    if (0 == pmemPtr->base)
    {
        pthread_mutex_unlock(&pmemPtr->mutex);
        fprintf(stderr,"memory %s not yet allocated\n", (char *)pmemPtr->name);
        return NULL;
    }
#if 0
    /* for cookie */
    size += sizeof(unsigned long);
#endif
    /* align size */
#ifdef OS_MEM_64_BIT_ALIGNED
    size = (size + 7) & 0xFFFFFFF8;
#else
    size = (size+3) & 0xfffffffc;
#endif

    /* Extend buffer */
    newEnd = pmemPtr->curr + size;
    if (newEnd > pmemPtr->end)
    {
        pthread_mutex_unlock(&pmemPtr->mutex);
        fprintf(stderr, "memory %s exhausted base=0x%0x curr=0x%0x size=0x%0x end=0x%0x\n",
                (char *)pmemPtr->name, (int)pmemPtr->base, (int)pmemPtr->curr,
                (int)size, (int)pmemPtr->end);
        return NULL;
    }

    /* Reallocate memory page-by-page */
    while (newEnd > pmemPtr->last)
    {
        if(extendBufferByOnePage(fd, GT_TRUE, pmemPtr) == GT_FAIL)
        {
            pthread_mutex_unlock(&pmemPtr->mutex);
            fprintf(stderr,"extendBufferByOnePage() %s\n", strerror(errno));
            return NULL;
        }
    }
#if 0
    /* add the cookie */
    *((unsigned long *) pmemPtr->curr) = pmemPtr->curr;
    pmemPtr->curr += sizeof(unsigned long);
#endif
    address = (GT_VOID *) pmemPtr->curr;
    pmemPtr->curr = newEnd;
    
    pthread_mutex_unlock(&pmemPtr->mutex);
    
    /*fprintf(stderr,"shrMemSharedMallocIntoBlock: allocated sucessfully at %p\n", address);*/
    return address;
}

