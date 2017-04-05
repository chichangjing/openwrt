/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer. 

      *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

        *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* prestera.c
*
* DESCRIPTION:
*       functions in kernel mode special for prestera.
*
* DEPENDENCIES:
*
*       $Revision: 31 $
*******************************************************************************/
#if defined(CONFIG_MIPS) && defined(CONFIG_64BIT)
#define MIPS64_CPU
#endif

#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/seq_file.h>

#ifdef  PRESTERA_SYSCALLS
#  include <linux/syscalls.h>
#  include <asm/unistd.h>
#endif

#include "./include/presteraGlob.h"
#include "./include/presteraSmiGlob.h"
#include "./include/presteraIrq.h"
#include "./include/prestera.h"
#ifdef PRESTERA_PP_DRIVER
#include "./include/presteraPpDriverGlob.h"
#endif
#include "./include/prvExtDrvLinuxMapping.h"
#include <cpss/generic/cpssTypes.h>
#ifndef PRV_CPSS_MAX_PP_DEVICES_CNS
# define PRV_CPSS_MAX_PP_DEVICES_CNS 128
#endif
#ifndef _32K
# define _32K           (0x8000)
#endif
#ifndef _1M
# define _1M            (0x100000)
#endif
#ifndef _4M
# define _4M            (0x400000)
#endif
#ifndef _64M
# define _64M           (0x4000000)
#endif

#define MV_LINUX_BSP
#include "gtExtDrv/drivers/pssBspApis.h"

/* #define PRESTERA_DEBUG */

#include "./include/presteraDebug.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
/* Don't use obsolette create_proc_read_entry() */
#define USE_PROC_SEQFILE
#endif

#if defined(GDA8548) || defined(EP3041A) || defined(XCAT) || defined(INTEL64)
#define HAS_HSU
#endif

#if defined(GDA8548) || defined(EP3041A)
#define consistent_sync(x...)
#endif

#if defined(INTEL64_CPU) || defined(V306PLUS)
#include <linux/signal.h>
extern int send_sig_info(int, struct siginfo *, struct task_struct *);
#endif

#if defined(MIPS64_CPU) || defined(INTEL64_CPU)
/*
 FULL64_CPU = text running in 64 bit mode, and physical addresses are 64 bit
 however, in our intel and mips sdk, the pex bars are in fact allocated below the 32 line,
 therefore 32bit variables are sufficient
*/
#define FULL64_CPU
#endif

#if defined(EP3041A)
/*
 FULL64_CPU = text running in 32 bit mode, and physical addresses are 64 bit
 in ep3041a, the pex bars are in fact allocated above  the 64 line, 
 therefore 64bit variables are required
*/
#define HALF64_CPU
#endif

#if defined(FULL64_CPU)|| defined(HALF64_CPU)
// helper definition for the union of both defs
#define BOTH64_CPU
#endif

#if defined(MIPS64_CPU) || defined(INTEL64_CPU)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#define consistent_sync(x...)
#endif
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31)
#define consistent_sync(x...)
#endif
#endif

#if defined(V306PLUS)
#define consistent_sync(x...)
#endif

#define PP_MAPPINGS_MAX             64
#define PCI_RESOURCE_PSS_CONFIG     0
#define PCI_RESOURCE_PSS_REGS       1
#define PCI_RESOURCE_PSS_REGS_PEX   2

   /* local variables and variables */
static int                  prestera_opened      = 0;
static int                  prestera_initialized = 0;
static struct Prestera_Dev  *prestera_dev;
static uintptr_t            pci_regs_vma_start = LINUX_VMA_PP_REGS_BASE;
static uintptr_t            pci_regs_vma_end = LINUX_VMA_PP_REGS_BASE;
static uintptr_t            pci_dfx_vma_start = LINUX_VMA_PP_DFX_BASE;
static uintptr_t            pci_dfx_vma_end = LINUX_VMA_PP_DFX_BASE;
static uintptr_t            pci_conf_vma_start = LINUX_VMA_PP_CONF_BASE;
static uintptr_t            pci_conf_vma_end = LINUX_VMA_PP_CONF_BASE;
/* info for mmap */
static struct Mmap_Info_stc mmapInfoArr[PP_MAPPINGS_MAX];
static int                  mmapInfoArrSize = 0;
#define M mmapInfoArr[mmapInfoArrSize]
#ifdef XCAT
static uintptr_t            dragonite_addr_vma_start = LINUX_VMA_DRAGONITE;
static uintptr_t            dragonite_addr_vma_end = LINUX_VMA_DRAGONITE + _32K;
static GT_U32               dragonite_phys_addr;
#endif
static struct PP_Dev        *ppdevs[PRV_CPSS_MAX_PP_DEVICES_CNS];
static int                  founddevs = 0;
static unsigned long        dev_size = 0;
static int                  prestera_major = PRESTERA_MAJOR;
static struct cdev          prestera_cdev;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
static struct class*        prestera_class;
#endif
static unsigned long        legalDevTypes[] = {CPSS_DEV_TYPES};
static uintptr_t            dma_base_vma_start = LINUX_VMA_DMABASE;
static uintptr_t            dma_base_vma_end = LINUX_VMA_DMABASE + 2 * _1M;
static unsigned long        dma_base;
static GT_U32               dma_len;
static void                *dma_area;

#if defined(HAS_HSU)
static uintptr_t            hsu_base_vma_start = LINUX_VMA_HSU;
static uintptr_t            hsu_base_vma_end = LINUX_VMA_HSU + 8 * _1M;
static unsigned long        hsu_base;
static unsigned long        hsu_len;
static void                *hsu_area;
#endif

static void*                dma_tmp_virt = NULL;
static dma_addr_t           dma_tmp_phys = 0;

#ifdef XCAT
#define PROCFS_MAX_SIZE   2560 /* 2,5 Kb */
static unsigned char procfs_buffer[PROCFS_MAX_SIZE];
#endif

module_param(prestera_major,int, S_IRUGO);
MODULE_AUTHOR("Marvell Semi.");
MODULE_LICENSE("GPL");
#define printk_line(x...) printk("error in line %d ", __LINE__); printk(x)
int bspPciFindDevReset(void);
ssize_t presteraSmi_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t presteraSmi_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
int presteraSmi_ioctl(unsigned int, unsigned long);
int presteraSmi_init(void);
static int prestera_DmaRead(
                            unsigned long address,
                            unsigned long length,
                            unsigned long burstLimit,
                            unsigned long buffer);
static int prestera_DmaWrite(
                             unsigned long address,
                             unsigned long length,
                             unsigned long burstLimit,
                             unsigned long buffer);
extern int rx_DSR; /* rx DSR invocation counter */
extern int tx_DSR; /* tx DSR invocation counter */

#ifdef BOTH64_CPU
extern int bspAdv64Malloc32bit; /* in bssBspApis.c */
#endif

/************************************************************************
 *
 * And the init and cleanup functions come last
 *
 ************************************************************************/

/*
 * static struct prvPciDeviceQuirks prvPciDeviceQuirks[]
 *
 * Quirks can be added to GT_PCI_DEV_VENDOR_ID structure
 */
PRV_PCI_DEVICE_QUIRKS_ARRAY_MAC

static struct prvPciDeviceQuirks* prestera_find_quirks(struct PP_Dev *dev)
{
  int k;
  GT_U32 pciId = (dev->devId << 16) | dev->vendorId;

  for (k = 0; prvPciDeviceQuirks[k].pciId != 0xffffffff; k++)
  {
    if (prvPciDeviceQuirks[k].pciId == pciId)
      break;
  }
  return &(prvPciDeviceQuirks[k]);
}


static loff_t prestera_lseek(struct file *filp, loff_t off, int whence)
{
  struct  Prestera_Dev * dev;
  loff_t newpos;
  dev = (struct Prestera_Dev *) filp->private_data;
  switch (whence)
  {
  case 0: /* SEEK_SET */
    newpos = off;
    break;
  case 1: /* SEEK_CUR */
    newpos = filp->f_pos + off;
    break;
  case 2: /* SEEK_END */
    newpos = dev->size + off;
    break;
  default : /* can't happend */
    return -EINVAL;
  }
  if (newpos < 0)
  {
    return -EINVAL;
  }
  if (newpos >= dev->size)
  {
    return -EINVAL;
  }
  filp->f_pos = newpos;
  return newpos;
}

/*
 * find device index
 * return -1 if not found
 */
static int find_pp_device(uint32_t busNo, uint32_t devSel, uint32_t funcNo) 
{
    int             i;
    struct PP_Dev   *pp;
    for (i = 0; i < founddevs; i++)
    {
        pp = ppdevs[i];
        if (    pp->busNo != busNo ||
                pp->devSel != devSel ||
                pp->funcNo != funcNo)
            continue;
        /* found */
        return i;
    }
    return -1; /* not found */
}


/*
 * configure virtual addresses
 */
static void ppdev_set_vma(struct PP_Dev *dev, int devIdx, unsigned long regsSize)
{
	if (dev->ppregs.mmapbase != 0)
		return; /* already configured */

    dev->ppregs.mmapbase = pci_regs_vma_end;
    dev->ppregs.mmapsize = regsSize;
    pci_regs_vma_end += dev->ppregs.mmapsize;
    M.map_type = MMAP_INFO_TYPE_PP_REGS_E;
    M.index  = devIdx;
    M.addr   = dev->ppregs.mmapbase;
    M.length = dev->ppregs.mmapsize;
    M.offset = 0;
    mmapInfoArrSize++;

    dev->config.mmapbase = pci_conf_vma_end;
    dev->config.mmapsize = dev->config.size;
    pci_conf_vma_end += dev->config.mmapsize;
    M.map_type = MMAP_INFO_TYPE_PP_CONF_E;
    M.index  = devIdx;
    M.addr   = dev->config.mmapbase;
    M.length = dev->config.mmapsize;
    M.offset = dev->config.mmapoffset;
    mmapInfoArrSize++;

    if (dev->dfx.phys != 0)
    {
        dev->dfx.mmapbase = pci_dfx_vma_end;
        dev->dfx.mmapsize = dev->dfx.size;
        pci_dfx_vma_end += dev->dfx.mmapsize;
        M.map_type = MMAP_INFO_TYPE_PP_DFX_E;
        M.index  = devIdx;
        M.addr   = dev->dfx.mmapbase;
        M.length = dev->dfx.mmapsize;
        M.offset = 0;
        mmapInfoArrSize++;
    }
}

#ifdef PRESTERA_PP_DRIVER
/* ppDriver */
int presteraPpDriverPciPexCreate(struct PP_Dev *dev);
int presteraPpDriverPciPexHalfCreate(struct PP_Dev *dev);
int presteraPpDriverPexMbusCreate(struct PP_Dev *dev);
static int presteraPpDriver_ioctl(unsigned int cmd, unsigned long arg)
{
	struct PP_Dev *dev;
	
	if (cmd == PRESTERA_PP_DRIVER_IO) {
		struct mvPpDrvDriverIo_STC io;
		
		if (copy_from_user(&io, (struct mvPpDrvDriverIo_STC *)arg, sizeof(io))) {
			printk_line("IOCTL: FAULT\n");
			return -EFAULT;
		}
		if (io.id >= founddevs) {
			return -EFAULT;
		}
		dev = ppdevs[io.id];
		if (dev->ppdriver)
			return dev->ppdriver(dev->ppdriverData, &io);
		return -EFAULT;
	}
	if (cmd == PRESTERA_PP_DRIVER_OPEN) {
		struct mvPpDrvDriverOpen_STC info;
		int i;
		if (copy_from_user(&info,(struct mvPpDrvDriverOpen_STC *)arg, sizeof(info))) {
			printk_line("IOCTL: FAULT\n");
			return -EFAULT;
		}

		i = find_pp_device(info.busNo, info.devSel, info.funcNo);
		if (i < 0) {
			/* device not found */
			return -EFAULT;
		}
		dev = ppdevs[i];

		if (dev->ppdriver != NULL) {
			if ((int)info.type != dev->ppdriverType) {
				/* driver doesn't match */
				return -EFAULT;
			}
			/* driver is the same, skip */
		}
		else {
			switch (info.type) {
				case mvPpDrvDriverType_Pci_E:
					presteraPpDriverPciPexCreate(dev);
					break;
				case mvPpDrvDriverType_PciHalf_E:
					presteraPpDriverPciPexHalfCreate(dev);
					break;
				case mvPpDrvDriverType_PexMbus_E:
					presteraPpDriverPexMbusCreate(dev);
					break;
			}
			if (dev->ppdriver == NULL) {
				/* failed to initialize, return error */
				return -EFAULT;
			}
		}
		info.id = i;

		if (copy_to_user((struct mvPpDrvDriverOpen_STC *)arg, &info, sizeof(info))) {
			printk_line("IOCTL: FAULT\n");
			return -EFAULT;
		}
	}
	return 0;
}
#endif /* defined(PRESTERA_PP_DRIVER) */

/************************************************************************
 *
 * prestera_ioctl: The device ioctl() implementation
 */
static long prestera_ioctl(struct file *filp,
                          unsigned int cmd, unsigned long arg)
{
  struct PP_Dev                 *dev;
  struct PciConfigReg_STC        pciConfReg;
  struct GT_PCI_Dev_STC          gtDev;
  struct GT_Intr2Vec             int2vec;
  struct GT_VectorCookie_STC     vector_cookie;
  struct GT_RANGE_STC            range;
  struct intData                *intData;
  int                            i;
  struct GT_DmaReadWrite_STC     dmaRWparams;
  struct GT_TwsiReadWrite_STC    twsiRWparams;
  GT_STATUS                      rc;
  mv_kmod_size_t                 temp_len;
  struct GT_PCI_Mapping_STC      mapping;

#ifdef PRESTERA_PP_DRIVER
  if (_IOC_TYPE(cmd) == PRESTERA_PP_DRIVER_IOC_MAGIC)
    return presteraPpDriver_ioctl(cmd, arg);
#endif /* defined(PRESTERA_PP_DRIVER) */
  if (_IOC_TYPE(cmd) == PRESTERA_SMI_IOC_MAGIC)
    return presteraSmi_ioctl(cmd, arg);

  /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
  if (_IOC_TYPE(cmd) != PRESTERA_IOC_MAGIC)
  {
    printk_line("wrong ioctl magic key\n");
    return -ENOTTY;
  }

  /* GETTING DATA */
  switch(cmd)
  {

  case PRESTERA_IOC_DMAWRITE:
    if (copy_from_user(&dmaRWparams, (struct GT_DmaReadWrite_STC*)arg, sizeof(dmaRWparams)))
    {
      printk_line("IOCTL: FAUIT: bad param\n");
      return -EFAULT;
    }
    return prestera_DmaWrite(
                             dmaRWparams.address,
                             dmaRWparams.length,
                             dmaRWparams.burstLimit,
                             (unsigned long)dmaRWparams.buffer);
  case PRESTERA_IOC_DMAREAD:
    if (copy_from_user(&dmaRWparams, (struct GT_DmaReadWrite_STC*)arg, sizeof(dmaRWparams)))
    {
      printk_line("IOCTL: FAUIT: bad param\n");
      return -EFAULT;
    }
    return prestera_DmaRead(
                            dmaRWparams.address,
                            dmaRWparams.length,
                            dmaRWparams.burstLimit,
                            (unsigned long)dmaRWparams.buffer);
  case PRESTERA_IOC_HWRESET:
    break;

  case PRESTERA_IOC_INTCONNECT:
    /* read and parse user data structure */
    if (copy_from_user(&vector_cookie,(struct GT_VectorCookie_STC*) arg, sizeof(struct GT_VectorCookie_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    intConnect(vector_cookie.vector, 0, &intData);
    vector_cookie.cookie = (mv_kmod_uintptr_t)((uintptr_t)intData);
    /* USER READS */
    if (copy_to_user((struct GT_VectorCookie_STC*)arg, &vector_cookie, sizeof(struct GT_VectorCookie_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_INTENABLE:
    /* clear the mask reg on device 0x10*/
    if (arg > 64)
    {
      printk_line("DEBUG!!!\n");
      send_sig_info(SIGSTOP, (struct siginfo*)1, current);
    }
    enable_irq(arg);
    break;

  case PRESTERA_IOC_INTDISABLE:
    disable_irq(arg);
    break;

  case PRESTERA_IOC_WAIT:
    /* cookie */
    intData = (struct intData *)((uintptr_t)((mv_kmod_uintptr_t)arg));

    /* enable the interrupt vector */
    enable_irq(intData->intVec);

    if (down_interruptible(&intData->sem))
    {
      disable_irq(intData->intVec);
      return -ERESTARTSYS;
    }
    break;

  case PRESTERA_IOC_FIND_DEV:
    /* read and parse user data structurr */
    if (copy_from_user(&gtDev,(struct GT_PCI_Dev_STC*) arg, sizeof(struct GT_PCI_Dev_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }

    for (i = 0; i < founddevs; i++)
    {
      dev = ppdevs[i];
      if (gtDev.vendorId != dev->vendorId)
      {
        continue;
      }
      if (gtDev.devId != dev->devId)
      {
        continue;
      }
      if (gtDev.instance != dev->instance)
      {
        continue;
      }
      /* Found */
      gtDev.busNo  = dev->busNo;
      gtDev.devSel = dev->devSel;
      gtDev.funcNo = dev->funcNo;
      break;
    }
    if (i == founddevs)
    {
      return -ENODEV;
    }
    /* READ */
    if (copy_to_user((struct GT_PCI_Dev_STC*)arg, &gtDev, sizeof(struct GT_PCI_Dev_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_GETMAPPING:
    /* read and parse user data structurr */
    if (copy_from_user(&mapping,(struct GT_PCI_Mapping_STC*) arg, sizeof(struct GT_PCI_Mapping_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }

    i = find_pp_device(mapping.busNo, mapping.devSel, mapping.funcNo);
    if (i < 0)
    {
      /* not found */
      return -ENODEV;
    }
    dev = ppdevs[i];
    /* configure virtual addresses if not configured yet */
    ppdev_set_vma(dev, i, mapping.regsSize);

    mapping.mapConfig.addr = (mv_kmod_uintptr_t)dev->config.mmapbase;
    mapping.mapConfig.length = (mv_kmod_size_t)(dev->config.mmapsize);
    mapping.mapConfig.offset = (mv_kmod_size_t)(dev->config.mmapoffset);
    mapping.mapRegs.addr = (mv_kmod_uintptr_t)dev->ppregs.mmapbase;
    mapping.mapRegs.length = (mv_kmod_size_t)(dev->ppregs.mmapsize);
    mapping.mapRegs.offset = (mv_kmod_size_t)(dev->ppregs.mmapoffset);
    mapping.mapDfx.addr = (mv_kmod_uintptr_t)dev->dfx.mmapbase;
    mapping.mapDfx.length = (mv_kmod_size_t)(dev->dfx.mmapsize);
    mapping.mapDfx.offset = (mv_kmod_size_t)(dev->dfx.mmapoffset);
    /* WRITE */
    if (copy_to_user((struct GT_PCI_Mapping_STC*)arg, &mapping, sizeof(struct GT_PCI_Mapping_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_PCICONFIGWRITEREG:
    /* read and parse user data structure */
    if (copy_from_user(&pciConfReg,(struct PciConfigReg_STC*) arg, sizeof(struct PciConfigReg_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }

    rc = _bspPciConfigWriteReg(pciConfReg.busNo,pciConfReg.devSel,pciConfReg.funcNo,pciConfReg.regAddr,pciConfReg.data);
    if(rc != GT_OK)
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_PCICONFIGREADREG:
    /* read and parse user data structure */
    if (copy_from_user(&pciConfReg,(struct PciConfigReg_STC*) arg, sizeof(struct PciConfigReg_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    rc = _bspPciConfigReadReg(pciConfReg.busNo,pciConfReg.devSel,pciConfReg.funcNo,pciConfReg.regAddr,&(pciConfReg.data));
    if(rc != GT_OK)
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    if (copy_to_user((struct PciConfigReg_STC*)arg, &pciConfReg, sizeof(struct PciConfigReg_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_GETINTVEC:
    if (copy_from_user(&int2vec,(struct GT_Intr2Vec*) arg, sizeof(struct GT_Intr2Vec)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }

    rc = _bspPciGetIntVec(int2vec.intrLine, (void*)&int2vec.vector);
    if(rc != GT_OK)
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }

    if (copy_to_user((struct GT_Intr2Vec*)arg, &int2vec, sizeof(struct GT_Intr2Vec)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_FLUSH:
    /* read and parse user data structure */
    if (copy_from_user(&range,(struct GT_RANGE_STC*) arg, sizeof(struct GT_RANGE_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    consistent_sync((void*)((uintptr_t)range.address), range.length, PCI_DMA_TODEVICE);
    break;

  case PRESTERA_IOC_INVALIDATE:
    /* read and parse user data structure */
    if (copy_from_user(&range,(struct GT_RANGE_STC*) arg, sizeof(struct GT_RANGE_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    consistent_sync((void*)((uintptr_t)range.address), range.length, PCI_DMA_FROMDEVICE);
    break;

  case PRESTERA_IOC_GETBASEADDR:
    {
    mv_phys_addr_t ret = (mv_phys_addr_t)dma_base;
    if (copy_to_user((void*)arg, &ret, sizeof(ret)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    }
    break;
  case PRESTERA_IOC_GETDMASIZE:
    temp_len = dma_len;

#ifdef BOTH64_CPU
    if (bspAdv64Malloc32bit == 1) /* extern in bssBspApis.c. Defaults to 1 */
      temp_len |= 0x80000000;
#endif

    if (copy_to_user((void *)arg, &temp_len, sizeof(temp_len)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_TWSIINITDRV:
    if(_bspTwsiInitDriver() != GT_OK)
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_TWSIWAITNOBUSY:
    if(_bspTwsiWaitNotBusy() != GT_OK)
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    break;

  case PRESTERA_IOC_TWSIWRITE:
    /* read and parse user data structure */
    if (copy_from_user(&twsiRWparams,(struct GT_TwsiReadWrite_STC*) arg, sizeof(struct GT_TwsiReadWrite_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    if(_bspTwsiMasterWriteTrans(twsiRWparams.devId, (unsigned char*)((uintptr_t)twsiRWparams.pData),
                               twsiRWparams.len, twsiRWparams.stop) != GT_OK)
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }

    break;

  case PRESTERA_IOC_TWSIREAD:
    /* read and parse user data structure */
    if (copy_from_user(&twsiRWparams,(struct GT_TwsiReadWrite_STC*) arg, sizeof(struct GT_TwsiReadWrite_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    if(_bspTwsiMasterReadTrans(twsiRWparams.devId, (unsigned char*)((uintptr_t)twsiRWparams.pData),
                              twsiRWparams.len, twsiRWparams.stop) != GT_OK)
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }
    if (copy_to_user((struct GT_TwsiReadWrite_STC*)arg, &twsiRWparams, sizeof(struct GT_TwsiReadWrite_STC)))
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }

    break;

#ifdef XCAT
  case PRESTERA_IOC_DRAGONITESWDOWNLOAD:
    {
      struct GT_DragoniteSwDownload_STC params;

      /* read and parse user data structure */
      if (copy_from_user(&params, (struct GT_DragoniteSwDownload_STC*) arg, sizeof(struct GT_DragoniteSwDownload_STC)))
      {
        printk_line("IOCTL: copy_from_user FAULT\n");
        return -EFAULT;
      }

      if(_bspDragoniteSWDownload((void*)((uintptr_t)params.buffer), params.length) != GT_OK)
      {
        printk_line("IOCTL: bspDragoniteSWDownload FAULT\n");
        return -EFAULT;
      }
    }

    break;

  case PRESTERA_IOC_DRAGONITEENABLE:
    if(_bspDragoniteEnableSet(arg) != GT_OK)
    {
      printk_line("IOCTL: bspDragoniteEnableSet FAULT\n");
      return -EFAULT;
    }

    break;

  case PRESTERA_IOC_DRAGONITEINIT:
    if(_bspDragoniteInit() != GT_OK)
    {
      printk_line("IOCTL: FAULT\n");
      return -EFAULT;
    }

    break;

  case PRESTERA_IOC_DRAGONITEGETINTVEC:
    {
      GT_U32 intVecNum;

      if(_bspDragoniteGetIntVec(&intVecNum) != GT_OK)
      {
        printk_line("IOCTL: bspDragoniteGetIntVec FAULT\n");
        return -EFAULT;
      }

      if (copy_to_user((long*)arg, &intVecNum, sizeof(long)))
      {
        printk_line("IOCTL: copy_to_user FAULT\n");
        return -EFAULT;
      }
    }

    break;

  case PRESTERA_IOC_DRAGONITESHAREDMEMREAD:
    {
      struct GT_DragoniteMemAccsess_STC params;

      /* read and parse user data structure */
      if (copy_from_user(&params, (struct GT_DragoniteMemAccsess_STC*) arg, sizeof(struct GT_DragoniteMemAccsess_STC)))
      {
        printk_line("IOCTL: FAULT READ 0 \n");
        return -EFAULT;
      }

      if (params.length > PROCFS_MAX_SIZE)
      {
        printk_line("IOCTL: FAULT READ 1 \n");
        return -EFAULT;
      }

      if(_bspDragoniteSharedMemRead(params.offset, (void*)((uintptr_t)procfs_buffer), params.length) != GT_OK)
      {
        printk_line("IOCTL: FAULT READ 2\n");
        return -EFAULT;
      }

      if (copy_to_user(procfs_buffer, (char*)((uintptr_t)params.buffer), params.length))
      {
        printk_line("IOCTL: FAULT READ 3\n");
        return -EFAULT;
      }
    }

    break;

  case PRESTERA_IOC_DRAGONITESHAREDMEMWRITE:
    {
      struct GT_DragoniteMemAccsess_STC params;

      /* read and parse user data structure */
      if (copy_from_user(&params, (struct GT_DragoniteMemAccsess_STC*) arg, sizeof(struct GT_DragoniteMemAccsess_STC)))
      {
        printk_line("IOCTL: WRITE 0\n");
        return -EFAULT;
      }

      if (params.length > PROCFS_MAX_SIZE)
      {
        printk_line("IOCTL: FAULT WRITE 1\n");
        return -EFAULT;
      }

      if (copy_from_user(procfs_buffer, (void*)((uintptr_t)params.buffer), params.length))
      {
        printk_line("IOCTL: FAULT WRITE 2\n");
        return -EFAULT;
      }

      if(_bspDragoniteSharedMemWrite(params.offset, procfs_buffer, params.length) != GT_OK)
      {
        printk_line("IOCTL: FAULT WRITE 3\n");
        return -EFAULT;
      }
    }

    break;

  case PRESTERA_IOC_DRAGONITEITCMCRCCHECK:
    if(_bspDragoniteFwCrcCheck() != GT_OK)
    {
      printk("IOCTL: FAULT\n");
      return -EFAULT;
    }

    break;

#endif /* XCAT */


#if defined(HAS_HSU)
  case PRESTERA_IOC_HSUWARMRESTART:
    _bspWarmRestart();
    break;
#endif

  case PRESTERA_IOC_ISFIRSTCLIENT:
    if (prestera_opened == 1)
        return 0;
    return 1;

  case PRESTERA_IOC_GETMMAPINFO:
    {
        struct GT_PCI_MMAP_INFO_STC mInfo;
        /* read and parse user data structure */
        if (copy_from_user(&mInfo,(struct GT_PCI_MMAP_INFO_STC*) arg, sizeof(struct GT_PCI_MMAP_INFO_STC)))
        {
            printk_line("IOCTL: FAULT\n");
            return -EFAULT;
        }
        if (mInfo.index < 0 || mInfo.index >= mmapInfoArrSize)
        {
            /* out of range */
            return -EFAULT;
        }
        mInfo.addr = (mv_kmod_uintptr_t)mmapInfoArr[mInfo.index].addr;
        mInfo.length = (mv_kmod_size_t)mmapInfoArr[mInfo.index].length;
        mInfo.offset = (mv_kmod_size_t)mmapInfoArr[mInfo.index].offset;
        if (copy_to_user((struct GT_PCI_MMAP_INFO_STC*)arg, &mInfo, sizeof(struct GT_PCI_MMAP_INFO_STC)))
        {
            printk_line("IOCTL: FAULT\n");
            return -EFAULT;
        }
    }
    break;

  case PRESTERA_IOC_GETVMA:
    {
        struct GT_PCI_VMA_ADDRESSES_STC vmaInfo;
        memset(&vmaInfo,0,sizeof(vmaInfo));
        vmaInfo.dmaBase = (mv_kmod_uintptr_t)dma_base_vma_start;
        vmaInfo.ppConfigBase = (mv_kmod_uintptr_t)pci_conf_vma_start;
        vmaInfo.ppRegsBase = (mv_kmod_uintptr_t)pci_regs_vma_start;
        vmaInfo.ppDfxBase = (mv_kmod_uintptr_t)pci_dfx_vma_start;
#ifdef XCAT
        vmaInfo.xCatDraginiteBase = (mv_kmod_uintptr_t)dragonite_addr_vma_start;
#endif
#if defined(HAS_HSU)
        vmaInfo.hsuBaseAddr = (mv_kmod_uintptr_t)hsu_base_vma_start;
#endif
        if (copy_to_user((struct GT_PCI_VMA_ADDRESSES_STC*)arg, &vmaInfo, sizeof(struct GT_PCI_VMA_ADDRESSES_STC)))
        {
            printk_line("IOCTL: FAULT\n");
            return -EFAULT;
        }
    }
    break;
    case PRESTERA_IOC_GETBOARDID:
    {
        mv_board_id_t boardId;

        /* This is a temporary solution in order to
         * enable CPSS software to get the Cetus
         * DB's board-id - '0x50'. (Please check
         * the board-id definitions in uBoot.
         * In the future, it is supposed to support
         * all board-ids of different switch devices.
         */
        boardId = 0x50;

        if (copy_to_user((void *)arg, &boardId, sizeof(boardId))) {
            printk(KERN_ERR "copy_to_user failed\n");
            return -EFAULT;
        }
    }
    break;
  default:
    printk_line (KERN_WARNING "Unknown ioctl (%x).\n", cmd);
    break;
  }
  return 0;
}


/*
 * open and close: just keep track of how many times the device is
 * mapped, to avoid releasing it.
 */

void prestera_vma_open(struct vm_area_struct *vma)
{
  prestera_opened++;
}

void prestera_vma_close(struct vm_area_struct *vma)
{
  prestera_opened--;
}

struct vm_operations_struct prestera_vm_ops = {
  .open   = prestera_vma_open,
  .close  = prestera_vma_close,
};

/************************************************************************
 *
 * prestera_mappedVirt2Phys: convert userspace address to physical
 * Only for mmaped areas
 *
 */
static mv_phys_addr_t prestera_mappedVirt2Phys(
                                              unsigned long address
                                              )
{
  if (address >= dma_base_vma_start && address < dma_base_vma_end)
  {
      address -= dma_base_vma_start;
      return (mv_phys_addr_t)dma_base + address;
  }

  if (address >= pci_regs_vma_start && address < pci_regs_vma_end)
  {
    struct PP_Dev   *dev;
    int             i;
    /* PSS regs */
    for (i = 0; i < founddevs; i++)
    {
      dev = ppdevs[i];
      if (address >= dev->ppregs.mmapbase && address < dev->ppregs.mmapbase + dev->ppregs.mmapsize)
      {
          address -= dev->ppregs.mmapbase;
          return dev->ppregs.phys + address;
      }
    }
    /* should never happen? */
    return 0;
  }
  /* TODO: DFX??? */

#ifdef XCAT
  if (address >= dragonite_addr_vma_start && address < dragonite_addr_vma_end)
  {
    address -= dragonite_addr_vma_start;
    return (mv_phys_addr_t)dragonite_phys_addr + address;
  }
#endif

#if defined(HAS_HSU)
  if (address >= hsu_base_vma_start && address < hsu_base_vma_end)
  {
    address -= hsu_base_vma_start;
    return (mv_phys_addr_t)hsu_base + address;
  }
#endif

  /* default */
  return 0;
}


/************************************************************************
 *
 * prestera_DmaRead: bspDmaRead() wrapper
 */
static int prestera_DmaRead(
                            unsigned long address,
                            unsigned long length,
                            unsigned long burstLimit,
                            unsigned long buffer
                            )
{
  mv_phys_addr_t bufferPhys;
  unsigned long tmpLength;
  mv_phys_addr_t phys;

  /* first convert source address to physical */
  phys = prestera_mappedVirt2Phys(address);
  if (!phys)
    return -EFAULT;

  bufferPhys = prestera_mappedVirt2Phys(buffer);
  if (bufferPhys)
  {
    return _bspDmaRead(phys, length, burstLimit, (GT_U32*)((uintptr_t)bufferPhys));
  }

  /* use dma_tmp buffer */
  while (length > 0)
  {
    tmpLength = (length > (PAGE_SIZE / 4)) ? PAGE_SIZE/4 : length;

    if (_bspDmaRead(phys, tmpLength, burstLimit, (GT_U32*)((uintptr_t)dma_tmp_phys)))
      return -EFAULT;

    length -= tmpLength;
    tmpLength *= 4;
    if (copy_to_user((void*)buffer, dma_tmp_virt, tmpLength))
      return -EFAULT;

    phys += tmpLength;
    buffer += tmpLength;
  }
  return 0;
}

/************************************************************************
 *
 * prestera_DmaWrite: bspDmaWrite() wrapper
 */
static int prestera_DmaWrite(
                             unsigned long address,
                             unsigned long length,
                             unsigned long burstLimit,
                             unsigned long buffer
                             )
{
  mv_phys_addr_t bufferPhys;
  unsigned long tmpLength;
  mv_phys_addr_t phys;

  /* first convert source address to physical */
  phys = prestera_mappedVirt2Phys(address);
  if (!phys)
    return -EFAULT;

  bufferPhys = prestera_mappedVirt2Phys(buffer);
  if (bufferPhys)
  {
    return _bspDmaWrite(phys, (GT_U32*)((uintptr_t)bufferPhys), length, burstLimit);
  }
  /* use dma_tmp buffer */
  while (length > 0)
  {
    tmpLength = (length > (PAGE_SIZE / 4)) ? PAGE_SIZE/4 : length;

    if (copy_from_user(dma_tmp_virt, (void*)buffer, tmpLength * 4))
      return -EFAULT;

    if (_bspDmaWrite(phys, (GT_U32*)((uintptr_t)dma_tmp_phys), tmpLength, burstLimit))
      return -EFAULT;

    length -= tmpLength;
    tmpLength *= 4;
    phys += tmpLength;
    buffer += tmpLength;
  }
  return 0;
}

/************************************************************************
 *
 * prestera_do_mmap: Map physical address to userspace
 */
static int prestera_do_mmap(struct vm_area_struct *vma,
                            mv_phys_addr_t phys,
                            unsigned long pageSize)
{
    /* bind the prestera_vm_ops */
    vma->vm_ops = &prestera_vm_ops;

    /* VM_IO for I/O memory */
    vma->vm_flags |= VM_IO;

    /* disable caching on mapped memory */
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    vma->vm_private_data = prestera_dev;

    vma->vm_pgoff = phys >> PAGE_SHIFT;

    /* fix case when pageSize < length_param_of_mmap */
    vma->vm_end = vma->vm_start + pageSize;

    printk("remap_pfn_range(phys=" MV_PHYS_ADDR_FMT ", PAGE_SHIFT=%x, 0x%lx, 0x%lx, 0x%lx, 0x%lx)\n",
         phys, PAGE_SHIFT,
         (unsigned long)(vma->vm_start), (unsigned long)(vma->vm_pgoff),
         (unsigned long)pageSize,
         *((unsigned long*)(&(vma->vm_page_prot))));

    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, pageSize,
                vma->vm_page_prot))
    {
        printk_line("remap_pfn_range failed\n");
        return 1;
    }

    prestera_vma_open(vma);

    return 0;
}

/************************************************************************
 *
 * prestera_mmap_dyn: map to dynamic address (single process only
 *
 * Key is vma->vm_pgoff where bit:(31-PAGE_SHIFT) == 1
 */
static int prestera_mmap_dyn(struct file * file, struct vm_area_struct *vma)
{
    uint32_t busNo, devSel, funcNo, barNo;
    struct PP_Dev  *ppdev;
    mv_phys_addr_t  phys;
    unsigned long   pageSize = 0;
    int i;

    busNo = (vma->vm_pgoff >> 10) & 0xff;
    devSel = (vma->vm_pgoff >> 5) & 0x1f;
    funcNo = (vma->vm_pgoff >> 2) & 0x07;
    barNo = vma->vm_pgoff & 0x03;
    if (busNo == 0xff && devSel == 0x1f && funcNo == 0x07) /* xCat */
    {
        devSel = 0xff;
        funcNo = 0xff;
    }
    i = find_pp_device(busNo, devSel, funcNo);
    if (i < 0)
    {
        printk_line("mvPP device 0x%02x:%02x.%x not found\n",
                busNo, devSel, funcNo);
        return -ENXIO;
    }
    ppdev = ppdevs[i];

    pageSize = vma->vm_end - vma->vm_start;

    switch (barNo)
    {
        case 0: /* config */
            if (pageSize < ppdev->config.size + ppdev->config.mmapoffset)
            {
                printk_line("No enough address space for config: 0x%lx, required 0x%lx\n",
                        (unsigned long)pageSize,
                        (unsigned long)(ppdev->config.size + ppdev->config.mmapoffset));
                return -ENXIO;
            }
            phys = ppdev->config.phys;
            pageSize = ppdev->config.size;
            ppdev->config.mmapbase = vma->vm_start;
            vma->vm_start += ppdev->config.mmapoffset;
            ppdev->config.mmapsize = pageSize;
            break;
        case 1: /* regs */
            phys = ppdev->ppregs.phys;
            ppdev->ppregs.mmapbase = vma->vm_start;
            ppdev->ppregs.mmapsize = pageSize;
            break;
        case 2: /* dfx */
            if (!ppdev->dfx.phys)
                return -ENXIO; /* ignore */
            if (pageSize < ppdev->dfx.size)
            {
                printk_line("No enough address space for dfx: 0x%lx, required 0x%lx\n",
                        (unsigned long)pageSize,
                        (unsigned long)ppdev->dfx.size);
                return -ENXIO;
            }
            phys = ppdev->dfx.phys;
            pageSize = ppdev->dfx.size;
            ppdev->dfx.mmapbase = vma->vm_start;
            ppdev->dfx.mmapsize = pageSize;
            break;
        default:
            printk_line("bad bar: %d\n", barNo);
            return -ENXIO;
    }

    return prestera_do_mmap(vma, phys, pageSize);
}

/************************************************************************
 *
 * prestera_mmap: The device mmap() implementation
 */
static int prestera_mmap(struct file * file, struct vm_area_struct *vma)
{
  mv_phys_addr_t  phys;
  unsigned long   pageSize = 0;
  struct PP_Dev  *ppdev;
  int             i;

  if ((vma->vm_pgoff & (1<<(31-PAGE_SHIFT))) != 0)
  {
      return prestera_mmap_dyn(file,vma);
  }
  if (((vma->vm_pgoff)<<PAGE_SHIFT) & (PAGE_SIZE-1))
  {
    /* need aligned offsets */
    printk_line("prestera_mmap offset not aligned\n");
    return -ENXIO;
  }

#define D mmapInfoArr[i]
  /* search for mapping */
  for (i=0; i < mmapInfoArrSize; i++)
  {
      if (vma->vm_start == D.addr + D.offset)
          break;
  }
  if (i == mmapInfoArrSize)
  {
    printk_line("unknown range (0%0x)\n", (int)vma->vm_start);
    return 1;
  }
  ppdev = ppdevs[D.index];

  switch (D.map_type)
  {
      case MMAP_INFO_TYPE_PP_DRAGONITE_E:
#ifdef XCAT
        phys = dragonite_phys_addr;
        pageSize = _32K; /* dtcm size */
#endif
        break;
      case MMAP_INFO_TYPE_HSU_E:
#if defined(HAS_HSU)
        phys = hsu_base;
        pageSize = 8 * _1M;
#endif
        break;
      case MMAP_INFO_TYPE_DMA_E:
        phys = dma_base;
        pageSize = dma_len;
        break;
      case MMAP_INFO_TYPE_PP_CONF_E:
        phys = ppdev->config.phys;
        pageSize = ppdev->config.size;

        if (pageSize > vma->vm_end - vma->vm_start)
            pageSize = vma->vm_end - vma->vm_start;
        break;
      case MMAP_INFO_TYPE_PP_REGS_E:
        phys = ppdev->ppregs.phys;
        pageSize = ppdev->ppregs.mmapsize;
        break;
      case MMAP_INFO_TYPE_PP_DFX_E:
        phys = ppdev->dfx.phys;
        pageSize = ppdev->dfx.size;
        break;
  }
#undef D
  if (pageSize == 0)
  {
    printk_line("unknown mapping (0%0x)\n", (int)vma->vm_start);
    return 1;
  }

  return prestera_do_mmap(vma, phys, pageSize);
}


/************************************************************************
 *
 * prestera_open: The device open() implementation
 */
static int prestera_open(struct inode * inode, struct file * filp)
{
  if (down_interruptible(&(prestera_dev->sem)))
  {
    return -ERESTARTSYS;
  }

  if (!prestera_initialized)
  {
    up(&prestera_dev->sem);
    return -EIO;
  }

/*#ifndef SHARED_MEMORY
   Avoid single-usage restriction for shared memory:
   * device should be accessible for multiple clients.
  if (prestera_opened)
  {
    up(&prestera_dev->sem);
    return -EBUSY;
  }
#endif*/

  filp->private_data = prestera_dev;

  prestera_opened++;
  up(&prestera_dev->sem);

  _printk("presteraDrv: prestera device opened successfuly\n");

  return 0;
}


/************************************************************************
 *
 * prestera_release: The device close() implementation
 */
static int prestera_release(struct inode * inode, struct file * file)
{
  printk("prestera_release\n");
  prestera_opened--;
  if (prestera_opened == 0)
  {
    cleanupInterrupts();
  }
  return 0;
}

#ifndef GDA8548

struct dumpregs_stc {
    const char *nm;
    unsigned start;
    unsigned end;
};
#ifdef USE_PROC_SEQFILE
static int dumpregs(struct seq_file *m, struct Mem_Region *mem, struct dumpregs_stc *r)
{
	void *basePtr = ioremap_nocache(mem->phys, PAGE_SIZE);

	for (; r->nm; r++) {
		int i, n;
		uintptr_t address;
		uint32_t  value;

		address = (uintptr_t)basePtr + (uintptr_t)r->start;
		for (i = r->start, n = 0; i <= r->end; i += 4) {
            value = __raw_readl((void *)address);
            GT_SYNC; /* TODO: ??? done in readl() */
			if (n == 0)
				seq_printf(m, "\t%s(0x%04x):", r->nm, i);
			seq_printf(m, " %08x", le32_to_cpu(value));
			n++;
			if (n == 4) {
				n = 0;
				if (i+4 <= r->end)
					seq_putc(m, '\n');
			}
			address += 4;
		}
		seq_putc(m, '\n');
	}

	iounmap(basePtr);
	return 0;
}
#else /* !defined(USE_PROC_SEQFILE) */
static int dumpregs(char *page, int len, mv_phys_addr_t phys, size_t maplen, struct dumpregs_stc *r)
{
    void* basePtr = ioremap_nocache(phys, maplen);
    for (;r->nm; r++)
    {
        int i, n;
        uintptr_t address;
        uint32_t  value;

        address = (uintptr_t)basePtr + (uintptr_t)r->start;
        for (i = r->start, n = 0; i <= r->end; i += 4)
        {
            value = __raw_readl((void *)address);
            GT_SYNC; /* TODO: ??? done in readl() */
            if (n == 0)
            {
                len += sprintf(page+len,"\t%s(0x%04x):",r->nm, i);
            }
            len += sprintf(page+len," %08x", le32_to_cpu(value));
            n++;
            if (n == 4)
            {
                n = 0;
                if (i+4 <= r->end)
                    page[len++] = '\n';
            }
            address += 4;
        }
        page[len++] = '\n';
    }
    iounmap(basePtr);

    return len;
}
#endif /* !defined(USE_PROC_SEQFILE) */

static struct dumpregs_stc ppConf[] = {
    { "\toff",  0,   0x1c },
#if 0
    /* No idea what are these registers,
     * not found in Lion2, Bobcat2 specs
     */
    { "MASK", 0x118, 0x118 },
    { "MASK", 0x114, 0x114 }, /*cause */
#endif
    { NULL, 0, 0 }
};

static struct dumpregs_stc ppRegs[] = {
    { "\toff",  0,   0 },
    { "\toff",0x50,  0x50 },
    { NULL, 0, 0 }
};

#endif /* !defined(GDA8548) */

/************************************************************************
 *
 * proc read data rooutine
 */
extern int short_bh_count;
#ifdef USE_PROC_SEQFILE
static int proc_status_show(struct seq_file *m, void *v)
{
  int i;
  struct PP_Dev *ppdev;

  seq_printf(m, "mvPP major # %d\n", prestera_major);
  seq_printf(m, "short_bh_count %d\n", short_bh_count);
  seq_printf(m, "rx_DSR %d\n", rx_DSR);
  seq_printf(m, "tx_DSR %d\n", tx_DSR);

  seq_printf(m, "DMA area: 0x%lx(virt), base: 0x%lx(phys), len: 0x%x\n",
          (unsigned long)dma_area, dma_base, dma_len);

  for (i = 0; i < founddevs; i++)
  {
    ppdev = ppdevs[i];
    seq_printf(m, "Device %d\n", i);
    seq_printf(m, "\tPCI %02x:%02x.%x  vendor:dev=%04x:%04x\n",
                   (unsigned)ppdev->busNo, (unsigned)ppdev->devSel, (unsigned)ppdev->funcNo,
                   ppdev->vendorId, ppdev->devId);
    /*seq_printf(m, "\tirq(0x%0x)\n",dev->irq);*/

    seq_printf(m, "\tconfig 0x%lx(user virt), phys: " MV_PHYS_ADDR_FMT ". len: 0x%0lx\n",
            (unsigned long)ppdev->config.mmapbase,
            ppdev->config.phys, (unsigned long)ppdev->config.size);

#ifndef GDA8548
    dumpregs(m, &(ppdev->config), ppConf);
#endif

    seq_printf(m, "\tregs 0x%lx(user virt), phys: " MV_PHYS_ADDR_FMT ", len: 0x%0lx\n",
            (unsigned long)ppdev->ppregs.mmapbase,
            ppdev->ppregs.phys, (unsigned long)ppdev->ppregs.size);

#ifndef GDA8548
    dumpregs(m, &(ppdev->ppregs), ppRegs);
#endif

  }

  return 0;
}

static int proc_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_status_show, NULL);
}

static const struct file_operations prestera_read_proc_operations = {
	.open		= proc_status_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

#else /* !defined(USE_PROC_SEQFILE) */
int prestera_read_proc_mem(char * page, char **start, off_t offset, int count, int *eof, void *data)
{
  int len;
  struct PP_Dev *ppdev;
  int i;

  len = 0;
  len += sprintf(page+len,"short_bh_count %d\n", short_bh_count);
  len += sprintf(page+len,"rx_DSR %d\n", rx_DSR);
  len += sprintf(page+len,"tx_DSR %d\n", tx_DSR);

  for (i = 0; i < founddevs; i++)
  {
    ppdev = ppdevs[i];
    len += sprintf(page+len,"Device %d\n", i);
    len += sprintf(page+len,"\tPCI %02x:%02x.%x  vendor:dev=%04x:%04x\n",
                   ppdev->busNo, ppdev->devSel, ppdev->funcNo,
                   ppdev->vendorId, ppdev->devId);
    /*len += sprintf(page+len,"\tirq(0x%0x)\n",dev->irq);*/

    len += sprintf(page+len,"\tconfig 0x%lx(user virt), phys: " MV_PHYS_ADDR_FMT ". len: 0x%0lx\n",
            (unsigned long)ppdev->config.mmapbase,
            ppdev->config.phys, (unsigned long)ppdev->config.size);

#ifndef GDA8548
    len = dumpregs(page, len, ppdev->config.phys, PAGE_SIZE, ppConf);
#endif

    len += sprintf(page+len,"\tregs 0x%lx(user virt), phys: " MV_PHYS_ADDR_FMT ", len: 0x%0lx\n",
            (unsigned long)ppdev->ppregs.mmapbase,
            ppdev->ppregs.phys, (unsigned long)ppdev->ppregs.size);

#ifndef GDA8548
    len = dumpregs(page, len, ppdev->ppregs.phys, PAGE_SIZE, ppRegs);
#endif

  }

  *eof = 1;

  return len;
}
#endif /* !defined(USE_PROC_SEQFILE) */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,69)
static char *prestera_devnode(struct device *dev, umode_t *mode)
#else /* < 3.4.69 */
static char *prestera_devnode(struct device *dev, mode_t *mode)
#endif /* < 3.4.69 */
{
    return kasprintf(GFP_KERNEL, "%s", dev->kobj.name);
}
#endif

#ifdef BOTH64_CPU
GT_STATUS pp_pci_bar_addr(struct PP_Dev *ppdev, int bar, mv_phys_addr_t *start, mv_resource_size_t *len)
{
    GT_STATUS           rc;
    MV_U64              data64;

    rc = _bspPciGetResourceStart(ppdev->busNo, ppdev->devSel, ppdev->funcNo, bar,
                                 &data64);
    if (rc != GT_OK)
    {
      printk("bspPciGetResourceStart error\n");
      return GT_FAIL;
    }
    *start = (mv_phys_addr_t)data64;

    rc = _bspPciGetResourceLen(ppdev->busNo, ppdev->devSel, ppdev->funcNo, bar,
                                  &data64);
    if (rc != GT_OK)
    {
      printk("bspPciGetResourceLen error\n");
      return GT_FAIL;
    }

    *len = (mv_resource_size_t)data64;
    return GT_OK;
}
#else /* !defined(BOTH64_CPU) */
GT_STATUS pp_pci_bar_addr(struct PP_Dev *ppdev, int bar, mv_phys_addr_t *start, mv_resource_size_t *len)
{
    GT_STATUS           rc;
    GT_U32              data32, saved;

    rc = _bspPciConfigReadReg(ppdev->busNo,ppdev->devSel,ppdev->funcNo,0x10+bar*4,&data32);
    if (rc != GT_OK)
    {
      printk("bspPciConfigReadReg error\n");
      return GT_FAIL;
    }

    *start = (mv_phys_addr_t)(data32 & 0xFFFFFFF0);
    *len = 0;
    /* assume length */
    if (bar == 0) /* config */
        *len = _1M;
    if (bar == 1 || bar == 2) /* regs */
        *len = _64M;
#ifdef XCAT
    if (ppdev->busNo != 0xff || ppdev->devSel != 0xff || ppdev->funcNo != 0xff)
#endif
    {
    /* try to autodetect length */
    saved = data32;
    _bspPciConfigWriteReg(ppdev->busNo,ppdev->devSel,ppdev->funcNo,0x10+bar*4,0xffffffff);
    _bspPciConfigReadReg(ppdev->busNo,ppdev->devSel,ppdev->funcNo,0x10+bar*4,&data32);
    _bspPciConfigWriteReg(ppdev->busNo,ppdev->devSel,ppdev->funcNo,0x10+bar*4,saved);
    if (data32 == 0xffffffff)
    {
        /* emulated PP on PEX */
    }
    else
    {
        *len = (mv_phys_addr_t)((~(data32 & 0xfffffff0)) + 1);
    }
    }
    if (*len > (_64M + _1M)) /* limit to 64M + 1M dfx */
    {
        *len = (_64M + _1M);
    }
    /*TODO:rm */
    printk("bar%d phys=0x%x len=0x%x\n", bar, (GT_U32)(*start), (GT_U32)(*len));

    return GT_OK;
}
#endif

GT_STATUS pp_pci_bar_map(
        struct PP_Dev   *ppdev,
        int             ppNum,
        const char      *name,
        int             bar,
        struct Mem_Region *m,
        size_t          offset
)
{
    GT_STATUS           rc;
    mv_phys_addr_t      start;
    mv_resource_size_t  len;

    rc = pp_pci_bar_addr(ppdev, bar, &start, &len);
    if (rc != GT_OK)
        return rc;

    if (offset >= len) /* be paranoid */
        offset = 0;
    start += offset;
    len -= offset;

    if (!request_mem_region(start, len, name))
    {
      printk("Cannot reserve MMIO region bar %d (%s) " MV_PHYS_ADDR_FMT " @ 0x%lx\n",
             bar, name, start, (unsigned long)len);
      return GT_FAIL;
    }

    m->allocbase = start;
    m->allocsize = len;
    m->size = len;
    m->phys = start;
    m->mmapoffset = offset;

    dev_size += len;

    return GT_OK;
}


static int pp_find_all_devices(GT_BOOL look4cpuEnabled)
{
  struct PP_Dev   *ppdev;
  int             type, i;

  GT_U32          data __attribute__((unused));

  GT_STATUS rc;
  GT_U32 busNo,devSel,funcNo;
  GT_U32 instance = 0;
  GT_U16 devId,vendorId;
  /*founddevs   = 0;*/
  dev_size    = 0;
  type        = 0;

  while (legalDevTypes[type] != CPSS_DEV_END)
  {

    devId = legalDevTypes[type] >> 16;
    vendorId = legalDevTypes[type] & 0xFFFF;

    if ((look4cpuEnabled == GT_FALSE && ((devId & 0x2) == 0)) ||
        (look4cpuEnabled == GT_TRUE && ((devId & 0x2) == 0x2)))
    {
      type++;
      instance = 0;
      continue;
    }

    rc = _bspPciFindDev(vendorId,devId,instance,&busNo, &devSel,&funcNo);
    if (rc != GT_OK)
    {
      /* no more devices */
      instance =0;
      type++;
      continue;
    }

    i = find_pp_device(busNo, devSel, funcNo);
    if (i >= 0)
    {
        /* already in array, skip */
        instance++;
        continue;
    }


    /* Save PCI device */
    ppdev = (struct PP_Dev *) kmalloc(sizeof(struct PP_Dev), GFP_KERNEL);
    if (NULL == ppdev)
    {
      printk("kmalloc failed\n");
      return -ENOMEM;
    }
    memset(ppdev, 0, sizeof(*ppdev));

    ppdev->devId = devId;
    ppdev->vendorId = vendorId;
    ppdev->instance = instance;
    ppdev->busNo = busNo;
    ppdev->devSel = devSel;
    ppdev->funcNo = funcNo;

    ppdevs[founddevs] = ppdev;
    instance++;
    founddevs++;
  }

  if (look4cpuEnabled == GT_FALSE)
  {
    return 0;
  }

  /* Map config,registers,dfx space */
  for (i = 0; i < founddevs; i++)
  {
    struct prvPciDeviceQuirks *quirks;

    ppdev = ppdevs[i];
    quirks = prestera_find_quirks(ppdev);


#ifdef ADV64_CPU
    quirks->configOffset = 0x70000;
#endif
    /* Note:
       It seems  that pex offset of 0xf0000 in lion for bar0 is wrong.
       It is 0x70000, and it leads to hangs on gda8548/ep8548.

       It also appears that nobody in cpss uses bar0 !.
       Therefore, instead of fixing the quirks macro in cpss we skip
       reading bar0 in the case of gda8548 . Giora
    */
    rc = pp_pci_bar_map(ppdev, i, "prestera - config",
            0,&(ppdev->config),quirks->configOffset);
    if (rc != GT_OK)
        return -ENODEV;

    rc = pp_pci_bar_map(ppdev,i, "prestera - registers",
            (quirks->isPex)?2:1,&(ppdev->ppregs),0);
    if (rc != GT_OK)
        return -ENODEV;

    if (quirks->hasDfx)
    {
        /* reduce ppregs to 64M */
        ppdev->ppregs.size = _64M;

        ppdev->dfx.phys = ppdev->ppregs.phys + _64M;
        ppdev->dfx.size = _1M;
        ppdev->dfx.allocbase = ppdev->dfx.phys;
        ppdev->dfx.allocsize = ppdev->dfx.size;
    }
  }

#ifdef XCAT
  if(_bspDragoniteSharedMemoryBaseAddrGet(&dragonite_phys_addr) != GT_OK)
  {
    printk("bspDragoniteSharedMemoryBaseAddrGet failed\n");
    return -ENODEV;
  }
#endif

  return 0;
}


static struct file_operations prestera_fops =
  {
    .llseek = prestera_lseek,
    .read   = presteraSmi_read,
    .write  = presteraSmi_write,
    .unlocked_ioctl  = prestera_ioctl,
    .mmap   = prestera_mmap,
    .open   = prestera_open,
    .release= prestera_release /* A.K.A close */
  };

#ifdef PRESTERA_SYSCALLS
/************************************************************************
 *
 * syscall entries for fast calls
 *
 ************************************************************************/
/* fast call to prestera_ioctl() */
asmlinkage long sys_prestera_ctl(unsigned int cmd, unsigned long arg)
{
    return prestera_ioctl(NULL, cmd, arg);
}

extern long sys_call_table[];

#define OWN_SYSCALLS 1

#ifdef __NR_SYSCALL_BASE
#  define __SYSCALL_TABLE_INDEX(name) (__NR_##name-__NR_SYSCALL_BASE)
#else
#  define __SYSCALL_TABLE_INDEX(name) (__NR_##name)
#endif

#define __TBL_ENTRY(name) { __SYSCALL_TABLE_INDEX(name), (long)sys_##name, 0 }
static struct {
    int     entry_number;
    long    own_entry;
    long    saved_entry;
} override_syscalls[OWN_SYSCALLS] = {
    __TBL_ENTRY(prestera_ctl)
};
#undef  __TBL_ENTRY


/*******************************************************************************
* prestera_OverrideSyscalls
*
* DESCRIPTION:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int
prestera_OverrideSyscalls(void)
{
    int k;
    for (k = 0; k < OWN_SYSCALLS; k++)
    {
        override_syscalls[k].saved_entry =
            sys_call_table[override_syscalls[k].entry_number];
        sys_call_table[override_syscalls[k].entry_number] =
            override_syscalls[k].own_entry;
    }
    return 0;
}

/*******************************************************************************
* prestera_RestoreSyscalls
*
* DESCRIPTION:
*       Restore original syscall entries.
*
* INPUTS:
*       None
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
static int
prestera_RestoreSyscalls(void)
{
    int k;
    for (k = 0; k < OWN_SYSCALLS; k++)
    {
        if (override_syscalls[k].saved_entry)
            sys_call_table[override_syscalls[k].entry_number] =
                override_syscalls[k].saved_entry;
    }
    return 0;
}
#endif /* PRESTERA_SYSCALLS */


/************************************************************************
 *
 * prestera_cleanup:
 */
static void prestera_cleanup(void)
{
  int i;
  struct PP_Dev *ppdev;

  printk("Prestera Says: Bye world from kernel\n");

  prestera_initialized = 0;

  cleanupInterrupts();

  for (i = 0; i < founddevs; i++)
  {
    ppdev = ppdevs[i];

#ifdef PRESTERA_PP_DRIVER
    if (ppdev->ppdriver)
    {
        /* destroy driver */
        ppdev->ppdriver(ppdev->ppdriverData, NULL);
    }
#endif /* PRESTERA_PP_DRIVER */

    /* relaese BAR0 */
    release_mem_region(ppdev->config.allocbase,ppdev->config.allocsize);
    /* relaese BAR1 */
    release_mem_region(ppdev->ppregs.allocbase,ppdev->ppregs.allocsize);

    kfree(ppdev);
  }
  founddevs = 0;

  if (dma_tmp_virt)
  {
    dma_free_coherent(NULL, PAGE_SIZE, dma_tmp_virt, dma_tmp_phys);
    dma_tmp_virt = NULL;
  }

   _bspCacheDmaFree(dma_area);

#ifdef PRESTERA_SYSCALLS
  prestera_RestoreSyscalls();
#endif
  remove_proc_entry("mvPP", NULL);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
  device_destroy(prestera_class, MKDEV(prestera_major, 0));
  class_destroy(prestera_class);
#endif
  cdev_del(&prestera_cdev);

  unregister_chrdev_region(MKDEV(prestera_major, 0), 1);
}

/************************************************************************
 *
 * prestera_init:
 */
static int prestera_init(void)
{
  int         result = 0;
  dev_t dev = MKDEV(prestera_major, 0);

  printk(KERN_DEBUG "prestera_init\n");

  /* first thing register the device at OS */

  /* Register your major. */
  result = register_chrdev_region(dev, 1, "mvPP");
  if (result < 0)
  {
    printk("prestera_init: register_chrdev_region err= %d\n", result);
    return result;
  }

  cdev_init(&prestera_cdev, &prestera_fops);

  prestera_cdev.owner = THIS_MODULE;

  result = cdev_add(&prestera_cdev, dev, 1);
  if (result)
  {
    printk("prestera_init: cdev_add err= %d\n", result);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
error_region:
#endif
    unregister_chrdev_region(dev, 1);
    return result;
  }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
  prestera_class = class_create(THIS_MODULE, "mvPP");
  if (IS_ERR(prestera_class))
  {
    printk(KERN_ERR "Error creating mvKernelExt class.\n");
    cdev_del(&prestera_cdev);
    result = PTR_ERR(prestera_class);
    goto error_region;
  }
  prestera_class->devnode = prestera_devnode;
  device_create(prestera_class, NULL, dev, NULL, "mvPP");
#endif

  printk(KERN_DEBUG "prestera_major = %d\n", prestera_major);

  prestera_dev = (struct Prestera_Dev *) kmalloc(sizeof(struct Prestera_Dev), GFP_KERNEL);
  if (!prestera_dev)
  {
    printk("\nPresteraDrv: Failed allocating memory for device\n");
    result = -ENOMEM;
    goto fail;
  }

#ifdef PRESTERA_SYSCALLS
  prestera_OverrideSyscalls();
#endif
  /* create proc entry */
#ifdef USE_PROC_SEQFILE
  if (!proc_create("mvPP", S_IRUGO, NULL, &prestera_read_proc_operations))
  {
    result = -ENOMEM;
    goto fail;
  }
#else
  create_proc_read_entry("mvPP", 0, NULL, prestera_read_proc_mem, NULL);
#endif

  /* initialize the device main semaphore */
  sema_init(&prestera_dev->sem, 1);

  memset(mmapInfoArr, 0, sizeof(mmapInfoArr));

  _bspPciFindDevReset();

  /* first add devices with CPU disable*/
  /* This is done to be in sync with cpss */
  result = pp_find_all_devices(GT_FALSE);
  if (0 != result)
  {
    goto fail;
  }
  /* add devices with CPU enable*/
  result = pp_find_all_devices(GT_TRUE);
  result = presteraSmi_init();
  if (0 != result)
  {
    goto fail;
  }
  result = initInterrupts();
  if (0 != result)
  {
    goto fail;
  }
  dma_len  = 2 * _1M;
  dma_area = (void *)_bspCacheDmaMalloc(dma_len);
  memset(dma_area, 0, dma_len);
  dma_base = _bspVirt2Phys((GT_U32)((uintptr_t)dma_area));
  printk("DMA - dma_area: %p(v) ,dma_base: 0x%lx(p), dma_len: 0x%x\n",
          dma_area,(unsigned long)dma_base, dma_len);
  /* add to mmapInfoArr */
  dma_base_vma_end = dma_base_vma_start + dma_len;
  M.map_type = MMAP_INFO_TYPE_DMA_E;
  M.addr = dma_base_vma_start;
  M.length = dma_len;
  mmapInfoArrSize++;

#if defined(HAS_HSU)
  hsu_len = 8 * _1M;
  hsu_area = (void *)_bspHsuMalloc(hsu_len);
  hsu_base = _bspVirt2Phys((GT_U32)((uintptr_t)hsu_area));
  printk("HSU - hsu_area: 0x%lx(v) ,hsu_base: 0x%lx(p), hsu_len: 0x%lx\n",
          (unsigned long)hsu_area,(unsigned long)hsu_base, hsu_len);

  /* add to mmapInfoArr */
  hsu_base_vma_end = hsu_base_vma_start + hsu_len;
  M.map_type = MMAP_INFO_TYPE_HSU_E;
  M.addr = hsu_base_vma_start;
  M.length = hsu_len;
  mmapInfoArrSize++;
#endif

#ifdef XCAT
  /* add to mmapInfoArr */
  dragonite_addr_vma_end = dragonite_addr_vma_start + _32K;
  M.map_type = MMAP_INFO_TYPE_PP_DRAGONITE_E;
  M.addr = dragonite_addr_vma_start;
  M.length = _32K; /* dtcm size */
  mmapInfoArrSize++;
#endif

#ifndef BOTH64_CPU
  /* allocate temp area for bspDma operations */
  dma_tmp_virt = dma_alloc_coherent(NULL, PAGE_SIZE, &dma_tmp_phys,
                                    GFP_DMA | GFP_KERNEL);
  if (!dma_tmp_virt)
  {
    printk("failed to allocate page for bspDma*() operations\n");
    return -ENOMEM;
  }
#endif

  prestera_initialized = 1;

  printk(KERN_DEBUG "prestera_init finished\n");

  return 0;

 fail:
  prestera_cleanup();

  printk("\nPresteraDrv: Init FAIL!\n");
  return result;
}

module_init(prestera_init);
module_exit(prestera_cleanup);
