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
* genDrvPciDrv.c
*
* DESCRIPTION:
*       Includes PCI functions wrappers implementation.
*
* DEPENDENCIES:
*       -   System controller.
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
#include <gtExtDrv/drivers/gtPciDrv.h>
#include <private/8245/gtCore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <pthread.h>

#include "kerneldrv/include/presteraGlob.h"

#ifdef SHARED_MEMORY
#   include <gtOs/gtOsSharedPp.h>
#endif

/*******************************************************************************
* internal definitions
*******************************************************************************/
#ifdef PRESTERA_DEBUG
#define PCI_DRV_DEBUG
#endif

#ifdef PCI_DRV_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

/* TODO: file descriptor type is int (POSIX) */
extern GT_32 gtPpFd;
#if __GNUC__ >= 4
unsigned int volatile gtInternalRegBaseAddr = 0;
#else
unsigned int gtInternalRegBaseAddr = 0;
#endif

/*******************************************************************************
* extDrvPciConfigWriteReg
*
* DESCRIPTION:
*       This routine write register to the PCI configuration space.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regAddr  - Register offset in the configuration space.
*       data     - data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciConfigWriteReg
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    struct PciConfigReg_STC pciConfReg;
    
    pciConfReg.busNo = busNo;
    pciConfReg.devSel = devSel;
    pciConfReg.funcNo = funcNo;
    pciConfReg.regAddr = regAddr;
    pciConfReg.data = data;
    
    /* call driver function */
    if (ioctl(gtPpFd, PRESTERA_IOC_PCICONFIGWRITEREG, &pciConfReg) < 0)
    {
        return GT_FAIL;
    }
    
    return GT_OK;
}


/*******************************************************************************
* extDrvPciConfigReadReg
*
* DESCRIPTION:
*       This routine read register from the PCI configuration space.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regAddr  - Register offset in the configuration space.
*
* OUTPUTS:
*       data     - the read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciConfigReadReg
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    struct PciConfigReg_STC pciConfReg;
    
    pciConfReg.busNo = busNo;
    pciConfReg.devSel = devSel;
    pciConfReg.funcNo = funcNo;
    pciConfReg.regAddr = regAddr;
    
    /* call driver function */
    if (ioctl(gtPpFd, PRESTERA_IOC_PCICONFIGREADREG, &pciConfReg) < 0)
    {
        return GT_FAIL;
    }
    
    *data = pciConfReg.data;
    
    return GT_OK;
}

static int prvPciNumMappingsDone = 0;
static pthread_mutex_t prvPciMappingMtx = PTHREAD_MUTEX_INITIALIZER;

/*******************************************************************************
* prvExtDrvDoMapping
*
* DESCRIPTION:
*       Map all allocated to userspace
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*       Linux only
*
*******************************************************************************/
static GT_STATUS prvExtDrvDoMapping(GT_VOID)
{
    int                     rc;
    struct GT_PCI_MMAP_INFO_STC    mmapInfo;
    void*                   mapped;
    /* map all */
    pthread_mutex_lock(&prvPciMappingMtx);
    while (1)
    {
        mmapInfo.index = prvPciNumMappingsDone;
        rc = ioctl(gtPpFd, PRESTERA_IOC_GETMMAPINFO, &mmapInfo);
        if (rc < 0)
            break;

        mapped = mmap((void*)((uintptr_t)mmapInfo.addr + (size_t)mmapInfo.offset),
                            (size_t)mmapInfo.length,
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_FIXED,
                            gtPpFd,
                            (size_t)mmapInfo.offset);

        if (MAP_FAILED == mapped)
        {
            pthread_mutex_unlock(&prvPciMappingMtx);
            return GT_FAIL;
        }
        prvPciNumMappingsDone++;
    }
    pthread_mutex_unlock(&prvPciMappingMtx);

    return GT_OK;
}

/*******************************************************************************
* extDrvInitDrv
*
* DESCRIPTION:
*       Open mvPP device,
*       If first client initialize it (set virtual addresses for userspace)
*       Map all to userspace
*
* INPUTS:
*       None
*
* OUTPUTS:
*       isFirstClient
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*       Linux only
*
*******************************************************************************/
GT_STATUS extDrvInitDrv
(
    OUT GT_BOOL  *isFirstClient
)
{
    GT_BOOL first = GT_FALSE;
    int rc;

    if (gtPpFd >= 0)
        return GT_ALREADY_EXIST;
    gtPpFd = open("/dev/mvPP", O_RDWR);
    if (gtPpFd < 0)
    {
        fprintf(stderr, "Cannot open /dev/mvPP for Read-Write, error=%d\n", errno);
        fprintf(stderr, "Possible reasons:\n");
        fprintf(stderr, "  *** No driver loaded\n");
        fprintf(stderr, "  *** Driver not initialized because no Marvell PCI devices found\n");
        fprintf(stderr, "      Check /proc/bus/pci/devices\n");
        return GT_FAIL;
    }

    rc = ioctl(gtPpFd, PRESTERA_IOC_ISFIRSTCLIENT, &first);
    if (rc == 0)
        first = GT_TRUE;
    /*TODO: rc < 0  => ??? */
    if (isFirstClient != NULL)
    {
        *isFirstClient = first;
    }
    if (rc < 0)
    {
        return GT_OK;
    }

    return prvExtDrvDoMapping();
}

#ifdef PP_DYNAMIC_MAPPING
#ifdef SHARED_MEMORY
# error "PP_DYNAMIC_MAPPING can't be used with SHARED_MEMORY"
#endif
/*******************************************************************************
* prvExtDrvPciMapDyn
*
* DESCRIPTION:
*       This routine maps PP registers and PCI registers into userspace
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       barNo    - 0 for config, 1 for regs, 2 for dfx
*       mapSize  - map size (64M for PCI)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvExtDrvPciMapDyn
(
    IN  GT_U32                  busNo,
    IN  GT_U32                  devSel,
    IN  GT_U32                  funcNo,
    IN  GT_U32                  barNo,
    IN  GT_UINTPTR              mapSize
)
{
    void*   mapped;
    off_t   mapId;

#ifndef SYSTEM_PAGE_SHIFT
#define SYSTEM_PAGE_SHIFT 12
#endif

    mapId = ((busNo  & 0xff) << 10) | /* 8 bit */
            ((devSel & 0x1f) << 5) | /* 5 bit */
            ((funcNo & 0x07) << 2) | /* 3 bit */
             (barNo  & 0x03) | /* 2 bit */
             (1 << (31-SYSTEM_PAGE_SHIFT)); /* mark: bit31 == 1 */

    mapId <<= SYSTEM_PAGE_SHIFT;

    mapped = mmap(NULL, (size_t)mapSize,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            gtPpFd,
            mapId);

    if (MAP_FAILED == mapped)
    {
        return GT_FAIL;
    }
    return GT_OK;
}
#endif /* PP_DYNAMIC_MAPPING */

/*******************************************************************************
* extDrvPciMap
*
* DESCRIPTION:
*       This routine maps PP registers and PCI registers into userspace
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regsSize - registers size (64M for PCI)
*
* OUTPUTS:
*       mapPtr   - The pointer to all mappings
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciMap
(
    IN  GT_U32                  busNo,
    IN  GT_U32                  devSel,
    IN  GT_U32                  funcNo,
    IN  GT_UINTPTR              regsSize,
    OUT GT_EXT_DRV_PCI_MAP_STC  *mapPtr
)
{
    struct GT_PCI_Mapping_STC mapping;
    GT_STATUS rc;

    if (mapPtr == NULL)
        return GT_BAD_PARAM;
    memset(mapPtr, 0, sizeof(*mapPtr));

#ifdef PP_DYNAMIC_MAPPING
    rc = prvExtDrvPciMapDyn(busNo, devSel, funcNo, 0, _1M);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = prvExtDrvPciMapDyn(busNo, devSel, funcNo, 1, regsSize);
    if (rc != GT_OK)
    {
        return rc;
    }
    prvExtDrvPciMapDyn(busNo, devSel, funcNo, 2, _1M);
#endif

    mapping.busNo = busNo;
    mapping.devSel = devSel;
    mapping.funcNo = funcNo;
    mapping.regsSize = regsSize;

    /* call driver function to get mapping info*/
    if (ioctl(gtPpFd, PRESTERA_IOC_GETMAPPING, &mapping) < 0)
    {
        return GT_FAIL;
    }
    /* map all allocated (if not mapped yet) */
    rc = prvExtDrvDoMapping();
    if (rc != GT_OK)
    {
        return rc;
    }

    mapPtr->regs.size = (GT_UINTPTR)mapping.mapRegs.length;
    mapPtr->regs.base = (GT_UINTPTR) mapping.mapRegs.addr;

    mapPtr->config.size = (GT_UINTPTR)mapping.mapConfig.length;
    mapPtr->config.base = (GT_UINTPTR)mapping.mapConfig.addr;

    mapPtr->dfx.size = (GT_UINTPTR)mapping.mapDfx.length;
    mapPtr->dfx.base = (GT_UINTPTR)mapping.mapDfx.addr;

    if (mapPtr->dfx.size >= _4M)
    {
        mapPtr->sram.base = mapPtr->dfx.base + _2M;
        mapPtr->sram.size = 0; /* not defined here, depends on PP */
    }
    return GT_OK;
}


/*******************************************************************************
* extDrvPciFindDev
*
* DESCRIPTION:
*       This routine returns the next instance of the given device (defined by
*       vendorId & devId).
*
* INPUTS:
*       vendorId - The device vendor Id.
*       devId    - The device Id.
*       instance - The requested device instance.
*
* OUTPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciFindDev
(
    IN  GT_U16  vendorId,
    IN  GT_U16  devId,
    IN  GT_U32  instance,
    OUT GT_U32  *busNo,
    OUT GT_U32  *devSel,
    OUT GT_U32  *funcNo
)
{
    struct GT_PCI_Dev_STC dev;
    
    dev.vendorId = vendorId;
    dev.devId    = devId;
    dev.instance = instance;
    
    /* call driver function */
    if (ioctl(gtPpFd, PRESTERA_IOC_FIND_DEV, &dev) < 0)
    {
        return GT_FAIL;
    }
    
    *busNo  = dev.busNo;
    *devSel = dev.devSel;
    *funcNo = dev.funcNo;
    
    return GT_OK;
}


/*******************************************************************************
* extDrvGetPciIntVec
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intVec - PCI interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvGetPciIntVec
(
    IN  GT_PCI_INT  pciInt,
    OUT void        **intVec
)
{
    struct GT_Intr2Vec int2vec;
    
    /* check parameters */
    if(intVec == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* get the PCI interrupt vector */
    /* call driver function */
    int2vec.intrLine = (GT_U32)pciInt;
    int2vec.bus = 0;
    int2vec.device = 0;
    int2vec.vector = 0;
    if (ioctl(gtPpFd, PRESTERA_IOC_GETINTVEC, &int2vec) < 0)
    {
        return GT_FAIL;
    }
    *intVec = (void *)((GT_UINTPTR)int2vec.vector);
    
    /* check whether a valid value */
    if((*intVec) == NULL)
    {
        DBG_INFO(("Failed in gtPciIntrToVecNum\n"));
        return GT_FAIL;
    }
    return GT_OK;
}


/*******************************************************************************
* extDrvGetIntMask
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intMask - PCI interrupt mask.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       PCI interrupt mask should be used for interrupt disable/enable.
*
*******************************************************************************/
GT_STATUS extDrvGetIntMask
(
    IN  GT_PCI_INT  pciInt,
    OUT GT_UINTPTR  *intMask
)
{
    void        *intVec;

    /* check parameters */
    if(intMask == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* get the PCI interrupt vector */
    extDrvGetPciIntVec(pciInt, &intVec);

    *intMask = (GT_UINTPTR)intVec;

    return GT_OK;
}

/*******************************************************************************
* extDrvPcieGetInterruptNumber
*
* DESCRIPTION:
*       This routine returns interrupt number for PCIe device
*
* INPUTS:
*       busNo      - PCI bus number.
*       devSel     - the device devSel.
*       funcNo     - function number.
*
* OUTPUTS:
*       intNumPtr  - Interrupt number value
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPcieGetInterruptNumber
(
    IN  GT_U32      busNo,
    IN  GT_U32      devSel,
    IN  GT_U32      funcNo,
    OUT GT_U32     *intNumPtr
)
{
    /* use /proc/bus/pci/devices */
    FILE *f;
    char s[512];
    unsigned pciAddr, devVend, irq;
    unsigned pciAddrQ;

    f = fopen("/proc/bus/pci/devices","r");
    if (f == NULL)
        return GT_BAD_PARAM;
    pciAddrQ = ((busNo & 0xff) << 8) |
               ((devSel & 0x1f) << 3) |
               (funcNo & 0x07);
    while (!feof(f))
    {
        if (fgets(s, sizeof(s), f) == NULL)
            break;
        if (sscanf(s, "%x %x %x", &pciAddr, &devVend, &irq) < 3)
            continue;
        if (pciAddr != pciAddrQ)
            continue;
        /* found */
        *intNumPtr = irq;
        fclose(f);
        return GT_OK;
    }
    fclose(f);
    return GT_FAIL;
}


/*******************************************************************************
* extDrvEnableCombinedWrites
*
* DESCRIPTION:
*       This function enables / disables the Pci writes / reads combining
*       feature.
*       Some system controllers support combining memory writes / reads. When a
*       long burst write / read is required and combining is enabled, the master
*       combines consecutive write / read transactions, if possible, and
*       performs one burst on the Pci instead of two. (see comments)
*
* INPUTS:
*       enWrCombine - GT_TRUE enables write requests combining.
*       enRdCombine - GT_TRUE enables read requests combining.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on sucess,
*       GT_NOT_SUPPORTED    - if the controller does not support this feature,
*       GT_FAIL             - otherwise.
*
* COMMENTS:
*       1.  Example for combined write scenario:
*           The controller is required to write a 32-bit data to address 0x8000,
*           while this transaction is still in progress, a request for a write
*           operation to address 0x8004 arrives, in this case the two writes are
*           combined into a single burst of 8-bytes.
*
*******************************************************************************/
GT_STATUS extDrvEnableCombinedPciAccess
(
    IN  GT_BOOL     enWrCombine,
    IN  GT_BOOL     enRdCombine
)
{
    return GT_OK;
}

/*******************************************************************************
* extDrvPciDoubleWrite
*
* DESCRIPTION:
*        This routine will write a 64-bit data  to given address
*
* INPUTS:
*        address - address to write to
*       word1 - the first half of double word to write (MSW)
*       word2 - the second half of double word to write (LSW)
*
* OUTPUTS:
*      none
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciDoubleWrite
(
    IN  GT_U32 address,
    IN  GT_U32 word1, 
    IN  GT_U32 word2
)
{
	return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* extDrvPciDoubleRead
*
* DESCRIPTION:
*        This routine will read a 64-bit data  from given address
*
* INPUTS:
*        address - address to read from
*
* OUTPUTS:
*       data     -  pointer for the received data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciDoubleRead
(
    IN  GT_U32  address,
    OUT GT_U64  *dataPtr
)
{
	return GT_NOT_SUPPORTED;
}
