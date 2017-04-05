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
* gt64260DmaDrv.c
*
* DESCRIPTION:
*       Includes DMA functions wrappers implementation for the 
*           PQ3-8548 system controller under FreeBSD
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#include <string.h>
#include <errno.h>
#include <gtExtDrv/drivers/gtCacheMng.h>
#include <gtExtDrv/drivers/gtDmaDrv.h>

#include "kerneldrv/include/prestera_glob.h"

/*************** Globals ******************************************************/

/* file descriptor returnd by openning the PP *nix device driver */
extern GT_32 gtPpFd;

/*******************************************************************************
* extDrvCacheDmaMalloc
*
* DESCRIPTION:
*       Allocate a cache-safe buffer of specified size for DMA devices
*       and drivers
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       ptr - pointer to allocated memory
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvCacheDmaMalloc
(
    IN  GT_U32 size,
    OUT void **ptr
)
{
    GT_DMA_MALLOC_STC cacheDmaMallocPrms; 

    cacheDmaMallocPrms.bytes = size;


    if (ioctl (gtPpFd, PRESTERA_IOC_CACHEDMAMALLOC, &cacheDmaMallocPrms))
    {
        fprintf(stderr, "extDrvCacheDmaMalloc failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    if(cacheDmaMallocPrms.ptr == NULL)
	{
		return GT_FAIL;
	}        
    else
    {
        *ptr = cacheDmaMallocPrms.ptr;
        return GT_OK;
    }
}

/*******************************************************************************
* extDrvCacheDmaFree
*
* DESCRIPTION:
*       Free the buffer acquired with extDrvCacheDmaMalloc
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
GT_STATUS extDrvCacheDmaFree
(
    IN void *ptr
)
{
    if (ioctl (gtPpFd, PRESTERA_IOC_CACHEDMAFREE, ptr))
    {
        fprintf(stderr, "extDrvCacheDmaFree failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

	return GT_OK;
}

/*******************************************************************************
* extDrvDmaWrite
*
* DESCRIPTION:
*       Write a given buffer to the given address using the Dma.
*
* INPUTS:
*       address     - The destination address to write to.
*       buffer      - The buffer to be written.
*       length      - Length of buffer in words.
*       burstLimit  - Number of words to be written on each burst.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*       1.  The given buffer is allways 4 bytes aligned, any further allignment
*           requirements should be handled internally by this function.
*       2.  The given buffer may be allocated from an uncached memory space, and
*           it's to the function to handle the cache flushing.
*       3.  The Prestera Driver assumes that the implementation of the DMA is
*           blocking, otherwise the Driver functionality might be damaged.
*
*******************************************************************************/
GT_STATUS extDrvDmaWrite
(
    IN  GT_UINTPTR  address,
    IN  GT_U32      *buffer,
    IN  GT_U32      length,
    IN  GT_U32      burstLimit
)
{
    GT_DMA_MEM_STC dmaMemPrms; 

    if(buffer == NULL)
        return GT_BAD_PTR;

    dmaMemPrms.address  = (GT_U32)address;
    dmaMemPrms.buffer   = buffer;
    dmaMemPrms.burstLimit = burstLimit;
    dmaMemPrms.length   = length;

    if (ioctl (gtPpFd, PRESTERA_IOC_WRITEDMA, &dmaMemPrms))
    {
        fprintf(stderr, "extDrvDmaWrite failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* extDrvDmaRead
*
* DESCRIPTION:
*       Read a memory block from a given address.
*
* INPUTS:
*       address     - The address to read from.
*       length      - Length of the memory block to read (in words).
*       burstLimit  - Number of words to be read on each burst.
*
* OUTPUTS:
*       buffer  - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*       1.  The given buffer is allways 4 bytes aligned, any further allignment
*           requirements should be handled internally by this function.
*       2.  The given buffer may be allocated from an uncached memory space, and
*           it's to the function to handle the cache flushing.
*       3.  The Prestera Driver assumes that the implementation of the DMA is
*           blocking, otherwise the Driver functionality might be damaged.
*
*******************************************************************************/
GT_STATUS extDrvDmaRead
(
    IN  GT_UINTPTR  address,
    IN  GT_U32      length,
    IN  GT_U32      burstLimit,
    OUT GT_U32      *buffer
)
{
    GT_DMA_MEM_STC dmaMemPrms; 

    if(buffer == NULL)
        return GT_BAD_PTR;

    dmaMemPrms.address  = (GT_U32)address;
    dmaMemPrms.buffer   = buffer;
    dmaMemPrms.burstLimit = burstLimit;
    dmaMemPrms.length   = length;

    if (ioctl (gtPpFd, PRESTERA_IOC_READDMA, &dmaMemPrms))
    {
        fprintf(stderr, "extDrvDmaRead failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* extDrvPhy2Virt
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
GT_STATUS extDrvPhy2Virt
(
    IN  GT_UINTPTR phyAddr,
    OUT GT_UINTPTR *virtAddr
)
{
	GT_U32 tmpAddr;

	tmpAddr = (GT_U32)phyAddr;

	if (ioctl (gtPpFd, PRESTERA_IOC_PHY2VIRT, &tmpAddr))
	{
		fprintf(stderr, "extDrvPhy2Virt failed: errno(%s)\n",
						strerror(errno));
		return GT_FAIL;
	}

    *virtAddr = (GT_UINTPTR)tmpAddr;

    return GT_OK;
}

/*******************************************************************************
* extDrvVirt2Phy
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
GT_STATUS extDrvVirt2Phy
(
    IN  GT_UINTPTR virtAddr,
    OUT GT_UINTPTR *phyAddr
)
{
	GT_U32 tmpAddr;

	tmpAddr = (GT_U32)virtAddr;

	if (ioctl (gtPpFd, PRESTERA_IOC_VIRT2PHY, &tmpAddr))
	{
		fprintf(stderr, "extDrvVirt2Phy failed: errno(%s)\n",
						strerror(errno));
		return GT_FAIL;
	}

	*phyAddr = (GT_UINTPTR)tmpAddr;

	return GT_OK;
}

