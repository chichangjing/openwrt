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
* gt64260PciDrv.c
*
* DESCRIPTION:
*       Includes PCI functions wrappers implementation for the GT64260 BSP.
*
* DEPENDENCIES:
*       -   System controller.
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
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

#include "kerneldrv/include/prestera_glob.h"

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

extern GT_32 gtPpFd;
unsigned int gtInternalRegBaseAddr = 0;

#define CONFIG_SIZE (4 * 1024)
#define REGS_SIZE (64 * 1024 * 1024)

/* PCI only, no quirks */
#if 0
/*
 * static struct prvPciDeviceQuirks prvPciDeviceQuirks[]
 *
 * Quirks can be added to GT_PCI_DEV_VENDOR_ID structure
 */
PRV_PCI_DEVICE_QUIRKS_ARRAY_MAC

static struct prvPciDeviceQuirks*
prvPciDeviceGetQuirks(unsigned vendorId, unsigned devId)
{
    int k;
    GT_U32  pciId;

    pciId = (devId << 16) | vendorId;

    for (k = 0; prvPciDeviceQuirks[k].pciId != 0xffffffff; k++)
    {
        if (prvPciDeviceQuirks[k].pciId == pciId)
            break;
    }
    return &(prvPciDeviceQuirks[k]);
}
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
    PciConfigReg_STC pciConfReg;
    
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
    PciConfigReg_STC pciConfReg;
    
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

    if (gtPpFd >= 0)
        return GT_ALREADY_EXIST;
    gtPpFd = open("/dev/mvPP", O_RDWR);
    if (gtPpFd < 0)
    {
        fprintf(stderr, "Cannot open /dev/mvPP for Read-Write, error=%d\n", errno);
        return GT_FAIL;
    }
    if (ioctl(gtPpFd, PRESTERA_IOC_ISFIRSTCLIENT, &first) == 0)
    {
        first = GT_TRUE;
    }
    if (isFirstClient != NULL)
        *isFirstClient = GT_TRUE;
    /*TODO: configure maping if first client */
    /*TODO: map all */
    return GT_OK;
}

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
    /* TODO */
    GT_U32 baseAddr;
    GT_U32 internalBase;
    /* PCI only, no quirks */
#if 0
    struct prvPciDeviceQuirks* quirks;
#endif
    static GT_U32 numOfDevices = 0;/* total number of devices in system */
#if defined(SHARED_MEMORY)
    GT_STATUS ret;
#endif

    if (mapPtr == NULL)
        return GT_BAD_PARAM;
    memset(mapPtr, 0, sizeof(mapPtr));

    numOfDevices++;

    /* PCI only, no quirks */
#if 0
    quirks = prvPciDeviceGetQuirks(vendorId, devId);
#endif

    /* map config space */
    internalBase = 0x60000000 + (numOfDevices - 1) * CONFIG_SIZE;
    internalBase = (GT_U32) mmap((void*)internalBase,
                            CONFIG_SIZE,
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_FIXED,
                            gtPpFd, 0);

    if (MAP_FAILED == (void*)internalBase)
    {
        return GT_FAIL;
    }

    /* map register space */
    baseAddr = (GT_U32) mmap((void*)(0x70000000 + ((numOfDevices - 1) << 26)),
                            REGS_SIZE,
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_FIXED,
                            gtPpFd,
                            0);

    if (MAP_FAILED == (void*)baseAddr)
    {
        return GT_FAIL;
    }

    /*TODO*/
    mapPtr->regs.base = (GT_UINTPTR)baseAddr;
    mapPtr->regs.size = REGS_SIZE;
    /*TODO*/
    mapPtr->config.base = (GT_UINTPTR)internalBase;
    mapPtr->config.size = PCI_CONFIG_SIZE;

#if defined(SHARED_MEMORY)
    /* We should note mapped section to restore mapping for all non-first clients */
    ret = shrMemRegisterPpMmapSection
    (
        (GT_VOID *)(mapPtr->regs.base),
        mapPtr->regs.size,
        0
    );
    if (GT_OK != ret) return ret;
    /* We should note mapped section to restore mapping for all non-first clients */
    ret = shrMemRegisterPpMmapSection
    (
        (GT_VOID *)(mapPtr->config.base),
        mapPtr->config.size,
        0
    );
    if (GT_OK != ret) return ret;
#endif

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
    GT_PCI_Dev_STC dev;
    
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
    GT_Intr2Vec int2vec;
    
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
    *intVec = (void *) int2vec.vector;
    
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
#ifdef GT_PCI
    /* use /proc/bus/pci/devices */
    FILE *f;
    char s[512];
    unsigned pciAddr, devVend, irq;
    unsigned pciAddrQ;

    if (busNo == 0xff && devSel == 0xff && funcNo != 0xff)
    {
        /* MSYS, ask kernel module for interrupt */
        struct GT_Intr2Vec int2vec;

        /* get the PCI interrupt vector */
        /* call driver function */
        int2vec.intrLine = (GT_U32)PCI_INT_A;
        int2vec.bus = 0;
        int2vec.device = 0;
        int2vec.vector = 0;
        if (ioctl(gtPpFd, PRESTERA_IOC_GETINTVEC, &int2vec) < 0)
        {
            return GT_FAIL;
        }
        /* check whether a valid value */
        if (int2vec.vector == 0)
        {
            DBG_INFO(("Failed in gtPciIntrToVecNum\n"));
            return GT_FAIL;
        }
        *intNumPtr = int2vec.vector;

        return GT_OK;
    }

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
#else
    return GT_NOT_SUPPORTED;
#endif
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
