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
* osFreeBsdMem.c
*
* DESCRIPTION:
*       FreeBsd User Mode Operating System wrapper. Memory manipulation facility.
*
* DEPENDENCIES:
*       FreeBsd, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsExc.h>

#include <gtOs/gtOsSharedMalloc.h>


#ifdef  LINUX_SIM
#define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
#include <os/simTypesBind.h>
#include <os/simOsProcess.h>
#include <asicSimulation/SDistributed/sdistributed.h>
#include <asicSimulation/SInit/sinit.h>
#else
#include <gtExtDrv/drivers/gtDmaDrv.h> /* for extDrvCacheDma... */
#endif

/***** Function Declaration ********************************************/
#ifdef LINUX_SIM
static void* dmaAsSharedMemoryMalloc(IN GT_U32 size);
#endif

/***** External Functions **********************************************/
void *osMemPoolsAlloc(GT_U32 size);
void *osMemPoolsRealloc(void *ptr, GT_U32 size);
void osMemPoolFree(void *memBlock);
void *osStaticMallocFromChunk(GT_U32 size);

extern GT_VOID prvOsCacheDmaMallocOverflowDetectionSupportGet
(
    INOUT GT_U32    *sizePtr
);
extern GT_STATUS osCacheDmaRegisterPtr(
    IN GT_VOID *    allocPtr,
    IN GT_U32       numOfBytes
);
extern void osCacheDmaUnregisterPtr(
    IN GT_VOID *    oldPointer
);
extern GT_STATUS osCacheDmaSystemReset(void);
/***** Static variables ************************************************/

static GT_BOOL  firstInitialization = GT_TRUE;

#ifdef LINUX_SIM
/* a number that indicate the 'key' representing the shared memory */
#define SIM_DMA_AS_SHARED_MEM_KEY_CNS   5678

/* id of the shared memory */
static GT_SH_MEM_ID simDmaAsSharedMemId = 0;

static GT_MUTEX simDmaMutexId;
/* pointer to the start of the shared memory */
static void* simDmaAsSharedMemPtr=NULL;

/* size of the shared memory */
static GT_U32 simDmaAsSharedMemSize = 0;

/* offset ued from start of the shared memory  */
static GT_U32 simDmaAsSharedMemOffsetUsed = 0;
#endif /* LINUX_SIM */

SHARED_MEM_STC cdma_mem;
GT_UINTPTR baseAddr = 0;
GT_UINTPTR baseVirtAddr = 0;
GT_U32 dmaSize = 2 * (1024 * 1024); /*default value*/

/***** Public Functions ************************************************/

/*******************************************************************************
* osMemInit
*
* DESCRIPTION:
*       Initialize the memory management.
*
* INPUTS:
*       size            - memory size to manage.
*       reallocOnDemand - defines whether in the case of memory chunk depleation
*                         a new chunk will be allocated or an out of memory will
*                         be reported.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_NO_RESOURCE - not enough memory in the system.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMemInit
(
    IN GT_U32 size,
    IN GT_BOOL reallocOnDemand
)
{
    GT_MEMORY_MANAGE_METHOD_ENT memAllocMethod;
    GT_STATUS ret;

    if (reallocOnDemand == GT_TRUE)
    {
        memAllocMethod = GT_MEMORY_MANAGE_METHOD_REALLOC_ON_DEMAND_E;
    }
    else
    {
        memAllocMethod = GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E;
    }

#ifdef LINUX_SIM
    /* Create the simDma semaphore */
    if (osMutexCreate("osSimDma", &simDmaMutexId) != GT_OK)
    {
        return GT_FAIL;
    }
    if(sasicgSimulationRole == SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_VIA_BROKER_E)
    {
        /* the application side that work with broker may decide how the broker
           will handle DMA read/write requests:
            1. use shared memory and do read/write by itself.
            2. use socket to send to application the request from read/write DMA
        */

        if(brokerDmaMode == BROKER_NOT_USE_SOCKET)
        {
            /* steal 1/2 of memory for DMA */
            if(NULL == dmaAsSharedMemoryMalloc(size/2))
            {
                return GT_NO_RESOURCE;
            }

            /* send DMA registration info */
            simDistributedRegisterDma((GT_U32)simDmaAsSharedMemPtr,/* the address of the pointer in our memory */
                    simDmaAsSharedMemSize,/* the size of shared memory */
                    SIM_DMA_AS_SHARED_MEM_KEY_CNS,/* the key to retrieve the shared memory */
                    BROKER_DMA_MODE_SHARED_MEMORY);

            /* use other 1/2 of memory for non-DMA */
            size = size / 2;
        }
        else if(brokerDmaMode == BROKER_USE_SOCKET)
        {
            /* send DMA registration info -- we work in socket mode */
            simDistributedRegisterDma(0,/* don't care */
                                      0,/* don't care */
                                      0,/* don't care */
                                      BROKER_DMA_MODE_SOCKET);
        }
        else
        {
            return GT_BAD_STATE;
        }
    }
#endif
    ret = osMemLibInit(SHARED_MALLOC_MAC,SHARED_FREE_MAC,size, memAllocMethod);
    firstInitialization = GT_FALSE;
    return ret;
}

/*******************************************************************************
* osStaticMalloc
*
* DESCRIPTION:
*       Allocates memory block of specified size, this memory will not be free.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available or calling after first init.
*
* COMMENTS:
*       Usage of this function is only on FIRST initialization.
*
*******************************************************************************/
void *osStaticMalloc
(
    IN GT_U32 size
)
{
    if (firstInitialization == GT_TRUE)
    {
        return osMalloc(size);
    }
    return osStaticMallocFromChunk(size);
}

/*******************************************************************************
* osMalloc
*
* DESCRIPTION:
*       Allocates memory block of specified size.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
void *osMalloc
(
    IN GT_U32 size
)
{
    IS_WRAPPER_OPEN_PTR;

    if( size == 0) return NULL;

    if (firstInitialization == GT_TRUE)
    {
        OSMEM_NOPOOLS_HDR_STC *hdrPtr;
        hdrPtr = (OSMEM_NOPOOLS_HDR_STC*)SHARED_MALLOC_MAC(size+sizeof(OSMEM_NOPOOLS_HDR_STC));
        if (hdrPtr == NULL)
        {
            return NULL;
        }
        hdrPtr->magic = (GT_UINTPTR)hdrPtr;
        hdrPtr->size = size;
        hdrPtr++;
        return (GT_VOID*)hdrPtr;
    }
    else
    {
        return osMemPoolsAlloc(size);
    }
}

/*******************************************************************************
* osRealloc
*
* DESCRIPTION:
*       Reallocate memory block of specified size.
*
* INPUTS:
*       ptr  - pointer to previously allocated buffer
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*       Recommended to avoid usage of this function.
*
*******************************************************************************/
void *osRealloc
(
    IN void * ptr ,
    IN GT_U32 size
)
{
    OSMEM_NOPOOLS_HDR_STC *oldHdr;

    IS_WRAPPER_OPEN_PTR;

    if (ptr == NULL)
    {
        return osMalloc(size);
    }

    oldHdr = (OSMEM_NOPOOLS_HDR_STC*)((GT_UINTPTR)ptr - sizeof(OSMEM_NOPOOLS_HDR_STC));
    if (firstInitialization == GT_TRUE)
    {
        OSMEM_NOPOOLS_HDR_STC *allocPtr;
        if (oldHdr->magic != (GT_UINTPTR)oldHdr)
        {
            osFatalError(OS_FATAL_WARNING, "osRealloc(): bad magic");
        }
        allocPtr = realloc(oldHdr, size+sizeof(OSMEM_NOPOOLS_HDR_STC));
        if (allocPtr == NULL)
        {
            return NULL;
        }
        allocPtr->magic = (GT_UINTPTR)allocPtr;
        allocPtr->size = size;
        return (GT_VOID*)(allocPtr + 1);
    }
    else
    {
        if (oldHdr->magic == (GT_UINTPTR)oldHdr)
        {
            /* realloc memory allocated by malloc() */
            GT_VOID *newPtr = osMalloc(size);
            if (newPtr == NULL)
            {
                return NULL;
            }
            osMemCpy(newPtr, ptr, oldHdr->size < size ? oldHdr->size : size);
#ifdef LINUX_SIM /* in black mode allocated memory shouldn't be freed */
            SHARED_FREE_MAC(oldHdr);
#endif
            return newPtr;

        }
        return osMemPoolsRealloc(ptr, size);
    }
}

/*******************************************************************************
* osFree
*
* DESCRIPTION:
*       Deallocates or frees a specified memory block.
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*       !!! In Black mode memory can be returned to pool,
*           but never actually freed !!!
*******************************************************************************/
void osFree
(
    IN void* const memblock
)
{
    OSMEM_NOPOOLS_HDR_STC *oldHdr = NULL;
    assert(memblock != NULL);
    oldHdr = (OSMEM_NOPOOLS_HDR_STC*)((GT_UINTPTR)memblock - sizeof(OSMEM_NOPOOLS_HDR_STC));
    if (firstInitialization == GT_TRUE)
    {
        if (oldHdr->magic != (GT_UINTPTR)oldHdr)
        {
            osFatalError(OS_FATAL_WARNING, "osFree(): bad magic");
        }
#ifdef LINUX_SIM /* in black mode allocated memory shouldn't be freed */
        SHARED_FREE_MAC(oldHdr);
#endif
    }
    else
    {
        /* check if this free is trying to free something that wasn't
           allocated by owr pools manager */
        if (oldHdr->magic == (GT_UINTPTR)oldHdr)
        {
/*#ifdef OSMEMPOOLS_DEBUG
            osPrintf("Warning:Trying to Free a pointer allocated by malloc after osMemInit !\n");
#endif bothers CLI */
#ifdef LINUX_SIM /* in black mode allocated memory shouldn't be freed */
            SHARED_FREE_MAC(oldHdr);
#endif
        }
        else
        {
            osMemPoolFree(memblock);
        }
    }
    return;
}

/*******************************************************************************
* osCacheDmaMalloc
*
* DESCRIPTION:
*       Allocate a cache-safe buffer  of specified size for DMA devices
*       and drivers
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the cache-safe buffer, or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
void *osCacheDmaMalloc
(
    IN GT_U32 size
)
{
    void * ptr;

    /* check if need to get modified size to hold support for 'overflow detection' */
    prvOsCacheDmaMallocOverflowDetectionSupportGet(&size);

#ifndef LINUX_SIM
    IS_WRAPPER_OPEN_PTR;

    extDrvCacheDmaMalloc(size, &ptr);

#else
    IS_WRAPPER_OPEN_PTR;
    if(simDmaAsSharedMemSize)
    {
        char* tmpPtr; /* tmp pointer to start of memory to return */
        /* take sem */
        osMutexLock(simDmaMutexId);

        /* check if we have enough space for the new memory request */
        if((simDmaAsSharedMemOffsetUsed + size) > simDmaAsSharedMemSize)
        {
            /* free sem */
            osMutexUnlock(simDmaMutexId);
            return NULL;
        }

        tmpPtr = (char*)simDmaAsSharedMemPtr + simDmaAsSharedMemOffsetUsed;

        simDmaAsSharedMemOffsetUsed += size;

        /* free sem */
        osMutexUnlock(simDmaMutexId);

        ptr = tmpPtr;
    }
    else
    {
        ptr = SHARED_MALLOC_MAC(size) ;
    }
#endif

    /* register the dma pointer for system reset */
    osCacheDmaRegisterPtr(ptr,size);

    return ptr;

}

/*******************************************************************************
* osCacheDmaFree
*
* DESCRIPTION:
*       Free the buffer acquired with osCacheDmaMalloc()
*
* INPUTS:
*       ptr - pointer to malloc/free buffer
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osCacheDmaFree
(
    IN void *ptr
)
{
    IS_WRAPPER_OPEN_STATUS;

    /* unregister the ptr */
    (void)osCacheDmaUnregisterPtr(ptr);

#ifndef LINUX_SIM
    return extDrvCacheDmaFree(ptr);
#else
    if(simDmaAsSharedMemSize)
    {
        /* not supporting free of this memory */
        return GT_OK;
    }
    SHARED_FREE_MAC(ptr) ;
    return GT_OK;
#endif
}

/*******************************************************************************
* osCacheDmaFreeAll
*
* DESCRIPTION:
*       Free ALL DMA memory
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osCacheDmaFreeAll(GT_VOID)
{
    /* let the manager clear the DB one by one (will be calling osCacheDmaFree(...))*/
    return osCacheDmaSystemReset();
}

/*******************************************************************************
* osPhy2Virt
*
* DESCRIPTION:
*       Converts physical address to virtual.
*
* INPUTS:
*       phyAddr  - physical address
*
* OUTPUTS:
*       virtAddr - virtual address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osPhy2Virt
(
    IN  GT_UINTPTR phyAddr,
    OUT GT_UINTPTR *virtAddr
)
{
    IS_WRAPPER_OPEN_STATUS;
#ifndef LINUX_SIM
    return extDrvPhy2Virt(phyAddr, virtAddr);
#else
    *virtAddr = phyAddr;
    return GT_OK;
#endif
}

/*******************************************************************************
* osVirt2Phy
*
* DESCRIPTION:
*       Converts virtual address to physical.
*
* INPUTS:
*       virtAddr - virtual address
*
* OUTPUTS:
*       phyAddr  - physical address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osVirt2Phy
(
    IN  GT_UINTPTR virtAddr,
    OUT GT_UINTPTR *phyAddr
)
{
    IS_WRAPPER_OPEN_STATUS;
#ifndef LINUX_SIM
    return extDrvVirt2Phy(virtAddr, phyAddr);
#else
    *phyAddr = virtAddr;
    return GT_OK;
#endif
}

/***** Private ********************************************************/

#ifdef LINUX_SIM
/*******************************************************************************
* dmaAsSharedMemoryMalloc
*
* DESCRIPTION:
*       Allocates shared memory block of specified size, this shared memory will
*       be used for 'DMA memory'
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to the shared memory (on NULL on fail)
*
* COMMENTS:
*       Usage of this function is only on FIRST initialization.
*       used only when distributed simulation of application side is working
*       with broker , in mode of DMA as shared memory(shared between the
*       broker and the application).
*
*******************************************************************************/
static void* dmaAsSharedMemoryMalloc
(
    IN GT_U32 size
)
{
    GT_STATUS   rc;

    rc = simOsSharedMemGet((GT_SH_MEM_KEY)SIM_DMA_AS_SHARED_MEM_KEY_CNS,size,0,&simDmaAsSharedMemId);
    if(rc != GT_OK)
    {
        return NULL;
    }

    /* Now we attach (map) the shared memory to our process memory */
    simDmaAsSharedMemPtr = simOsSharedMemAttach(simDmaAsSharedMemId,
            NULL ,/* NULL means we attach it to no specific memory , we let OS to decide */
            0);/*no specific flags*/

    if(simDmaAsSharedMemPtr)
    {
        simDmaAsSharedMemSize = size;
    }

    return simDmaAsSharedMemPtr;
}
#endif /* LINUX_SIM */
