/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemPpInit.c
*
* DESCRIPTION:
*       File contains routines for Packet Processor initialization
*       (applicable ONLY for BM).
*       Also routine for sections mapping registry is implemented here.
*
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
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
#include <gtOs/gtOsSharedData.h>

GT_32 gtPpFd = -1;
extern GT_UINTPTR baseAddr;
extern GT_UINTPTR baseVirtAddr;
extern GT_U32 dmaSize;
extern SHARED_MEM_STC cdma_mem;
static char *dma_name = "DMA";

/***** External Functions **********************************************/
GT_STATUS extDrvInitDrv(OUT GT_BOOL *isFirstClient);
GT_STATUS extDrvGetDmaBase(OUT GT_UINTPTR * dmaBase);
GT_STATUS extDrvGetDmaVirtBase(OUT GT_UINTPTR *dmaVirtBase);
GT_STATUS extDrvGetDmaSize(OUT GT_U32 * dmaSize);

#ifdef SHARED_MEMORY
/* Registry of mmaped sections (located into shared memory) */
/*TODO: move object definition here, use __SHARED_DATA_MAINOS attribute */
extern PP_MMAP_INFO_STC pp_mmap_sections_registry[PP_REGISTRY_SIZE_CNS];
extern int pp_mmap_registry_index;

/************* Functions ******************************************************/
/************* Public Functions ***********************************************/

/*******************************************************************************
* shrMemRegisterPpMmapSection
*
* DESCRIPTION:
*       Register section which should be mmaped by all non-first clients.
*
* INPUTS:
*       startPtr - start address to be mapped
*       length - length of block to be mapped
*       offset - offset into file to be mapped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success
*       GT_OUT_OF_RANGE - too many sections for static registry,
*         must be increased 
*
* COMMENTS:
*       Operation is relevant only for SHARED_MEMORY=1
*       Also operation is relevant only for undefined ASIC_SIMULATION (BlackMode).
*
*******************************************************************************/
GT_STATUS shrMemRegisterPpMmapSection
(
    IN GT_VOID *startPtr,
    IN size_t  length,
    IN off_t   offset
)
{
    PP_MMAP_INFO_STC *infoPtr;
    if(pp_mmap_registry_index == PP_REGISTRY_SIZE_CNS)
    {
        fprintf(stderr, "shrMemRegisterPpMmapSection: index out of space, please increase PP_REGISTRY_SIZE_CNS\n");
        return GT_OUT_OF_RANGE;
    }
    infoPtr = &pp_mmap_sections_registry[pp_mmap_registry_index++];

    /* Store required variables into registry */
    infoPtr->startPtr  = startPtr;
    infoPtr->length    = length;
    infoPtr->offset    = offset;

    return GT_OK;
}
#endif /* SHARED_MEMORY */

/*******************************************************************************
* shrMemSharedPpInit
*
* DESCRIPTION:
*       Performs sharing operations for DMA special device file.
*
* INPUTS:
*       isFirstClient - the-first-client flag 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Special device file /dev/mvPP is handled by presera kernel module.
*       mmap operation with fixed addresses will be redirected to prestera_mmap
*       to do special operations with DMA, register and config space.
*
*       Operation is relevant only for BlackMode (undefined ASIC_SIMULATION).
*
*******************************************************************************/
#ifndef SHARED_MEMORY
int adv64_malloc_32bit __SHARED_DATA_MAINOS = 0;
#endif

GT_STATUS shrMemSharedPpInit(IN GT_BOOL isFirstClient)
{
    GT_STATUS retVal;
    GT_BOOL first;

    retVal = extDrvInitDrv(&first);
    if (retVal == GT_OK)
    {
        if (first == GT_TRUE)
            isFirstClient = GT_TRUE;
    }
    else
    {
        if (retVal != GT_ALREADY_EXIST)
            return retVal;
    }

    if(isFirstClient)
    {
        /* Get physical address of the dma area need for Virt2Phys and Phys2Virt */    
        extDrvGetDmaBase(&baseAddr);	
        extDrvGetDmaVirtBase(&baseVirtAddr);	
        /* Map shared buffer and init management structure. */
        extDrvGetDmaSize(&dmaSize);
        if (dmaSize & 0x80000000)
        {
          dmaSize &= ~0x80000000;
#ifndef SHARED_MEMORY
          adv64_malloc_32bit = 1;
#endif
        }
        retVal = shrMemSharedBufferInit(dma_name, baseVirtAddr,
                                        dmaSize, -1,
                                        GT_FALSE, &cdma_mem);
        if (GT_OK != retVal)
            return GT_NO_RESOURCE;

    }
    else
    {
#ifdef SHARED_MEMORY
        int i=0;
        /* All other clients just map sections from registry */
        while(i<pp_mmap_registry_index)
        {
            /* Map registry */
            retVal = shrMemMmapMemory((GT_UINTPTR)pp_mmap_sections_registry[i].startPtr,
                                      pp_mmap_sections_registry[i].length, 
                                      pp_mmap_sections_registry[i].offset,
                                      gtPpFd);
            if (GT_OK != retVal)
                return retVal;

            i++;
        }
#else
        /* Whithout shared memory approach only SINGLE client is accessible.
         * So it should be the only first one.*/
        return GT_BAD_VALUE;
#endif
    }

    return GT_OK;
}

