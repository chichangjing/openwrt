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
*       $Revision: 1.1.2.5 $
*******************************************************************************/
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/version.h>

#include <asm/dma-mapping.h>

#include "./include/prestera_glob.h"
#include "./include/presteraIrq.h"
#include "./include/prestera.h"

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssTypes.h>

/*#define PRESTERA_DEBUG*/
#ifdef PRESTERA_DEBUG
#define DBG_PRINTK(x)   printk x
#else 
#define DBG_PRINTK(x)
#endif 

/* local variables and variables */
static int                  prestera_opened      = 0;
static int                  prestera_initialized = 0;
static struct Prestera_Dev  *prestera_dev;
static struct pci_dev       *pcidevs[PRV_CPSS_MAX_PP_DEVICES_CNS];
static struct PP_Dev        *ppdevs[PRV_CPSS_MAX_PP_DEVICES_CNS];
static int                  founddevs = 0;
static unsigned long        dev_size = 0;
static int                  prestera_major = PRESTERA_MAJOR;
static struct cdev          prestera_cdev;
static unsigned long        legalDevTypes[] = {CPSS_DEV_TYPES};
static unsigned long        dma_base;
static unsigned long        dma_len;
static unsigned long        *dma_area;
                   
module_param(prestera_major,int, S_IRUGO);
MODULE_AUTHOR("Marvell Semi.");
MODULE_LICENSE("GPL");

/* Forward declarations */
/* Forward function declarations */

/************************************************************************
 *
 * And the init and cleanup functions come last
 *
 ************************************************************************/
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


/************************************************************************
 * prestera_read: this should be the read device function, for now in
 * current prestera driver implemention it dose nothig
 ************************************************************************/
static ssize_t prestera_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    return -ERESTARTSYS;
}

/************************************************************************
 *
 * prestera_write: this should be the write device function, for now in
 * current prestera driver implemention it dose nothig
 *
 ************************************************************************/
static ssize_t prestera_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    return -ERESTARTSYS;
}


/************************************************************************
 *
 * prestera_ioctl: The device ioctl() implementation
 */
static int prestera_ioctl(struct inode *inode, struct file *filp,
                          unsigned int cmd, unsigned long arg)
{
    struct pci_dev          *dev;
    PP_PCI_REG              pciReg;
    PciConfigReg_STC        pciConfReg;
    GT_PCI_Dev_STC          gtDev;
    GT_Intr2Vec             int2vec;
    GT_VecotrCookie_STC     vector_cookie;
    GT_RANGE_STC            range;
    struct intData          *intData;
    int                     i, instance;
    unsigned long           devfn;
    
    /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
    if (_IOC_TYPE(cmd) != PRESTERA_IOC_MAGIC)
    {
        printk("wrong ioctl magic key\n");
        return -ENOTTY;
    }

    /* GETTING DATA */
    switch(cmd)
    {
        case PRESTERA_IOC_READREG:
            /* read and parse user data structur */
            if (copy_from_user(&pciReg,(PP_PCI_REG*) arg, sizeof(PP_PCI_REG)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            /* USER READS */
            if (copy_to_user((PP_PCI_REG*)arg, &pciReg, sizeof(PP_PCI_REG)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_WRITEREG:
            /* read and parse user data structur */
            if (copy_from_user(&pciReg,(PP_PCI_REG*) arg, sizeof(PP_PCI_REG)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_HWRESET:
            break;

        case PRESTERA_IOC_INTCONNECT:
            /* read and parse user data structure */
            if (copy_from_user(&vector_cookie,(PP_PCI_REG*) arg, sizeof(GT_VecotrCookie_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            intConnect(vector_cookie.vector, 0, (struct intData **)&vector_cookie.cookie);
            /* USER READS */
            if (copy_to_user((PP_PCI_REG*)arg, &vector_cookie, sizeof(GT_VecotrCookie_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_INTENABLE:
            /* clear the mask reg on device 0x10*/
            if (arg > 64)
            {
                printk("DEBUG!!!\n");
                send_sig_info(SIGSTOP, (struct siginfo*)1, current);
            }
            enable_irq(arg); 
            break;

        case PRESTERA_IOC_INTDISABLE:
            disable_irq(arg);
            break;

        case PRESTERA_IOC_WAIT:
            /* cookie */
            intData = (struct intData *) arg;

            /* enable the interrupt vector */
            enable_irq(intData->intVec); 

            if (down_interruptible(&intData->sem))
            {
                return -ERESTARTSYS;
            }
            break;

        case PRESTERA_IOC_FIND_DEV:
            /* read and parse user data structurr */
            if (copy_from_user(&gtDev,(GT_PCI_Dev_STC*) arg, sizeof(GT_PCI_Dev_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            instance = 0;
            
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
                if (gtDev.vendorId != dev->vendor)
                {
                    continue;
                }
                if (gtDev.devId != dev->device)
                {
                    continue;
                }
                if (instance < gtDev.instance)
                {
                    instance++;
                    continue;
                }
                /* Found */
                gtDev.busNo  = (unsigned long)dev->bus->number;
                gtDev.devSel = PCI_SLOT(dev->devfn); 
                gtDev.funcNo = PCI_FUNC(dev->devfn);
                break;
            }
            if (i == founddevs)
            {
                return -ENODEV;
            }
            /* READ */
            if (copy_to_user((GT_PCI_Dev_STC*)arg, &gtDev, sizeof(GT_PCI_Dev_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_PCICONFIGWRITEREG:
            /* read and parse user data structure */
            if (copy_from_user(&pciConfReg,(PciConfigReg_STC*) arg, sizeof(PciConfigReg_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            devfn = PCI_DEVFN(pciConfReg.devSel, pciConfReg.funcNo);
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
                if (pciConfReg.busNo != (unsigned long)dev->bus->number)
                {
                    continue;
                }
                if (devfn != dev->devfn)
                {
                    continue;
                }
                /* Found */
                pci_write_config_dword(dev, pciConfReg.regAddr, pciConfReg.data);
                break;
            }
            if (i == founddevs)
            {
                return -ENODEV;
            }
            break;

        case PRESTERA_IOC_PCICONFIGREADREG:
            /* read and parse user data structure */
            if (copy_from_user(&pciConfReg,(PciConfigReg_STC*) arg, sizeof(PciConfigReg_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            /* READ */
            devfn = PCI_DEVFN(pciConfReg.devSel, pciConfReg.funcNo);
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
                if (pciConfReg.busNo != (unsigned long)dev->bus->number)
                {
                    continue;
                }
                if (devfn != dev->devfn)
                {
                    continue;
                }
                /* Found */
                pci_read_config_dword(dev, pciConfReg.regAddr, (u32 *)&pciConfReg.data);
                break;
            }
            if (i == founddevs)
            {
                return -ENODEV;
            }
            if (copy_to_user((PciConfigReg_STC*)arg, &pciConfReg, sizeof(PciConfigReg_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_GETINTVEC:
            if (copy_from_user(&int2vec,(GT_Intr2Vec*) arg, sizeof(GT_Intr2Vec)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }

            dev = pcidevs[0];
            int2vec.vector = dev->irq;

            if (copy_to_user((GT_Intr2Vec*)arg, &int2vec, sizeof(GT_Intr2Vec)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_FLUSH:
            /* read and parse user data structure */
            if (copy_from_user(&range,(GT_RANGE_STC*) arg, sizeof(GT_RANGE_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
           /*ALEXL consistent_sync(range.address, range.length, PCI_DMA_TODEVICE); */
			__dma_sync((void *)range.address, range.length, PCI_DMA_TODEVICE); 
            break;

        case PRESTERA_IOC_INVALIDATE:
            /* read and parse user data structure */
            if (copy_from_user(&range,(GT_RANGE_STC*) arg, sizeof(GT_RANGE_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
           /* ALEXL consistent_sync(range.address, range.length, PCI_DMA_FROMDEVICE); */
            __dma_sync((void *)range.address, range.length, PCI_DMA_FROMDEVICE);
			break;

        case PRESTERA_IOC_GETBASEADDR:
            if (copy_to_user((long*)arg, &dma_base, sizeof(long)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_ISFIRSTCLIENT:
            if (prestera_opened == 1)
                return 0;
            return 1;

        default:
            printk (KERN_WARNING "Unknown ioctl (%x).\n", cmd);
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
    .nopage = NULL,
};


/************************************************************************
 *
 * prestera_mmap: The device mmap() implementation
 */
static int prestera_mmap(struct file * file, struct vm_area_struct *vma)
{
    unsigned long   phys;
    unsigned long   ppNum;
    unsigned long   offset;
    unsigned long   pageSize;
    struct pci_dev  *dev;
    
    if (((vma->vm_pgoff)<<PAGE_SHIFT) & (PAGE_SIZE-1))
    {
        /* need aligned offsets */                
        printk("prestera_mmap offset not aligned\n");
        return -ENXIO;  
    }
    
    /* bind the prestera_vm_ops */
    vma->vm_ops = &prestera_vm_ops;

    /* VM_IO for I/O memory */
    vma->vm_flags |= VM_IO;

    /* disable caching on mapped memory */
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    vma->vm_private_data = prestera_dev;
    
    switch (vma->vm_start & 0xf0000000)
    {
        case 0x50000000:
            /* DMA memory */
            phys = dma_base;
            pageSize = dma_len;
            break;

        case 0x60000000:
            /* PSS config */
            offset = (vma->vm_start - 0x60000000) + ((vma->vm_pgoff)<<PAGE_SHIFT);
            ppNum = offset / PAGE_SIZE;
            dev = pcidevs[ppNum];
            phys = pci_resource_start(dev, 0);
            pageSize = PAGE_SIZE;
            break;

        case 0x70000000:
            /* PSS regs */
            offset = (vma->vm_start - 0x70000000) + ((vma->vm_pgoff)<<PAGE_SHIFT);
            ppNum = offset / _64MB;
            dev = pcidevs[ppNum];
            phys = pci_resource_start(dev, 1);
            pageSize = _64MB;
            break;

        default:
            /* ??? */
            printk("unknown range (0%0x)\n", (int)vma->vm_start);
            return 1;
    }

    DBG_PRINTK(("remap_pfn_range(0x%X, 0x%X, 0x%X, 0x%X)\n",
                (int)(vma->vm_start), (int)(phys >> PAGE_SHIFT),
                (int)pageSize, (int)(vma->vm_page_prot)));

    if (remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT, pageSize,
                        vma->vm_page_prot))
    {
        printk("remap_pfn_range failed\n");
        return 1;
    }

    prestera_vma_open(vma);
    
    return 0;
}


/************************************************************************
 *
 * prestera_open: The device open() implementation
 */
static int prestera_open(struct inode * inode, struct file * filp)
{
    if (down_interruptible(&prestera_dev->sem))
    {
        return -ERESTARTSYS;
    }

    if (!prestera_initialized)
    {
        up(&prestera_dev->sem);
        return -EIO;
    }

#ifndef SHARED_MEMORY
    /* Avoid single-usage restriction for shared memory:
     * device should be accessible for multiple clients. */
    if (prestera_opened)
    {
        up(&prestera_dev->sem);
        return -EBUSY;
    }
#endif

    filp->private_data = prestera_dev;

    prestera_opened++;
    up(&prestera_dev->sem);

    DBG_PRINTK(("presteraDrv: prestera device opened successfuly\n"));

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

/************************************************************************
 *
 * proc read data rooutine
 */
int prestera_read_proc_mem(char * page, char **start, off_t offset, int count, int *eof, void *data)
{
    int len;
    struct pci_dev *dev;
    struct PP_Dev *ppdev;
    unsigned long address, value;
    volatile unsigned long *ptr;
    int i, j;
    extern int short_bh_count;
    
    len = 0;
    len += sprintf(page+len,"short_bh_count %d\n", short_bh_count);
    for (i = 0; i < founddevs; i++)
    {
        dev = pcidevs[i];
        ppdev = ppdevs[i];
        len += sprintf(page+len,"Device %d\n", i);
        len += sprintf(page+len,"\tbus(0x%0x) slot(0x%0x) func(0x%0lx)\n",(int)dev->bus, PCI_SLOT(dev->devfn), (unsigned long)PCI_FUNC(dev->devfn));
        len += sprintf(page+len,"\tvendor_id(0x%0x) device_id(0x%0x)\n",dev->vendor, dev->device);
        len += sprintf(page+len,"\tirq(0x%0x)\n",dev->irq);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
        len += sprintf(page+len,"\tname(%s)\n",pci_pretty_name(dev));
#else
        len += sprintf(page+len,"\tname(%s)\n",pci_name(dev));
#endif
        for (j = 0; j < 2; j++)
        {
          len += sprintf(page+len,"\t\t%d start(0x%0lx) end(0x%0lx)\n",j, (unsigned long)dev->resource[j].start, (unsigned long)dev->resource[j].end);
        }
        address = ppdev->config.base;
        len += sprintf(page+len,"\tconfig map to (0x%0lx)\n", address);
        for (j = 0; j < 0x14; j += 4)
        {
            ptr = (volatile unsigned long *) address;
            value = *ptr;
            GT_SYNC;
            len += sprintf(page+len,"\t\toff(0x%0x) add(0x%0x) val(0x%0x)\n", j, (int)ptr, le32_to_cpu(value));
            address += 4;
        }
        /* Mask */
        address = ppdev->config.base + 0x118;
        ptr = (volatile unsigned long *) address;
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\tMASK(0x%0x) add(0x%0x) val(0x%0x)\n", 0x118, (int)ptr, le32_to_cpu(value));

        /* Cause */
        address = ppdev->config.base + 0x114;
        ptr = (volatile unsigned long *) address;
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\tMASK(0x%0x) add(0x%0x) val(0x%0x)\n", 0x114, (int)ptr, le32_to_cpu(value));

        address = ppdev->ppregs.base;
        len += sprintf(page+len,"\tregs map to (0x%0lx)\n", address);

        ptr = (unsigned long *) (address + 0x0);
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\t\toff(0x%0x) val(0x%0x)\n", 0x0, le32_to_cpu(value));

        ptr = (unsigned long *) (address + 0x1000000);
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\t\toff(0x%0x) val(0x%0x)\n", 0x1000000, le32_to_cpu(value));
        
        ptr = (unsigned long *) (address + 0x2000000);
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\t\toff(0x%0x) val(0x%0x)\n", 0x2000000, le32_to_cpu(value));
        
        ptr = (unsigned long *) (address + 0x3000000);
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\t\toff(0x%0x) val(0x%0x)\n", 0x3000000, le32_to_cpu(value));
    } 

    *eof = 1;

    return len;
}


static int pp_find_all_devices(void)
{
    struct pci_dev  *dev;
    struct PP_Dev   *ppdev;
    int             type, i;
    unsigned long   start; 
    unsigned long   len;
    unsigned long   data;
    unsigned long   addrCompletion;
    
    founddevs   = 0;
    dev_size    = 0;
    type        = 0;
    dev         = NULL;

    while (legalDevTypes[type] != CPSS_DEV_END)
    {
        dev = pci_get_device(legalDevTypes[type] & 0xFFFF,
                              (legalDevTypes[type] & 0xFFFF0000) >> 16, dev);

        if (NULL == dev)
        {
            /* no more devices */
            type++;
            continue;
        }

        /* dev->irq, as part of the whole cpss bringup in this environment TBD */
        CPSS_TBD_BOOKMARK;

        /* Save PCI device */
        ppdev = (struct PP_Dev *) kmalloc(sizeof(struct PP_Dev), GFP_KERNEL);
        if (NULL == ppdev)
        {
            printk("kmalloc failed\n");
            return -ENOMEM;
        }
        
        /* enable the device */
        if(pci_enable_device(dev))
        {
            printk("pci_enable_device fail \n");
            return -ENODEV;
        }

        /* enable bus mastering */
        pci_set_master(dev);
        
        ppdevs[founddevs] = ppdev;
        pcidevs[founddevs] = dev;
        founddevs++;
        
    }
    
    /* Map registers space */
    for (i = 0; i < founddevs; i++)
    {
        dev   = pcidevs[i];
        ppdev = ppdevs[i];
        
        start = pci_resource_start(dev, 1);
        len   = pci_resource_len(dev, 1);

        /* Reserve BAR1 region for normal IO access */
        if (!request_mem_region(start, len, "prestera")) 
        {
            printk("Cannot reserve MMI0 region (prestera_mem_reg) 0x%lx @ 0x%lx\n",
                    start, len);
            return -ENODEV;
        }

        /* Find registers space size */
        ppdev->ppregs.size = len;

        /* Map rgisters space to kernel virtual address space */
        ppdev->ppregs.base = (unsigned long)ioremap_nocache(start, len);
        dev_size += len;

        /* Default configuration for address completion */
        data = *((unsigned long *)ppdev->ppregs.base);

        addrCompletion = cpu_to_le32(0x01000100);

        *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
        GT_SYNC;
        *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
        GT_SYNC;
        *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
        GT_SYNC;
        *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
        GT_SYNC;
        
        data = *((unsigned long *)ppdev->ppregs.base);
    }

    /* Map config space */
    for (i = 0; i < founddevs; i++)
    {
        dev   = pcidevs[i];
        ppdev = ppdevs[i];
        
        start = pci_resource_start(dev, 0);
        len   = pci_resource_len(dev, 0);

        /* Reserve BAR0 region for extended PCI configuration */ 
        if (!request_mem_region(start, len, "prestera")) 
        {
            printk("Cannot reserve MMIO region (prestera_ext_pci) 0x%lx @ 0x%lx\n",
                    start, len);
            return -ENODEV;
        }

        /* Find config space size */
        ppdev->config.size = len;

        /* Map config space to kernel virtual address space */
        ppdev->config.base = (unsigned long)ioremap_nocache(start, len);
        dev_size += len;
        data = *((unsigned long *)ppdev->config.base);
    }

    for (i = 0; i < founddevs; i++)
    {
        ppdev = ppdevs[i];
        data = *((unsigned long *)ppdev->config.base);
        data = *((unsigned long *)ppdev->ppregs.base);
    }

    return 0;
}


static struct file_operations prestera_fops =
{
    .llseek = prestera_lseek,
    .read   = prestera_read,
    .write  = prestera_write,
    .ioctl  = prestera_ioctl,
    .mmap   = prestera_mmap,
    .open   = prestera_open,
    .release= prestera_release /* A.K.A close */
};

/************************************************************************
 *
 * prestera_cleanup: 
 */
static void prestera_cleanup(void)
{
    int i;
    struct PP_Dev *ppdev;
    struct pci_dev *dev;

    printk("Prestera Says: Bye world from kernel\n");

    prestera_initialized = 0;

    cleanupInterrupts();

    for (i = 0; i < founddevs; i++)
    {
        ppdev = ppdevs[i];
        dev   = pcidevs[i];
        
        /* Unmap the memory regions */
        iounmap((void *)ppdev->config.base);
        iounmap((void *)ppdev->ppregs.base);
        
        /* relaese BAR0 */
        release_mem_region(pci_resource_start(dev, 0),
                           pci_resource_len(dev, 0));
        /* relaese BAR1 */
        release_mem_region(pci_resource_start(dev, 1),
                           pci_resource_len(dev, 1));
        
        pci_disable_device(dev);
    }
    founddevs = 0;

    remove_proc_entry("mvPP", NULL);

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

    printk(KERN_DEBUG "prestera_init\n");

    /* first thing register the device at OS */
    
    /* Register your major. */
    result = register_chrdev_region(MKDEV(prestera_major, 0), 1, "mvPP");
    if (result < 0)
    {
        printk("prestera_init: register_chrdev_region err= %d\n", result);
        return result;
    }

    cdev_init(&prestera_cdev, &prestera_fops);

    prestera_cdev.owner = THIS_MODULE;

    result = cdev_add(&prestera_cdev, MKDEV(prestera_major, 0), 1);
    if (result)
    {
        unregister_chrdev_region(MKDEV(prestera_major, 0), 1);
        printk("prestera_init: cdev_add err= %d\n", result);
        return result;
    }

    printk(KERN_DEBUG "prestera_major = %d\n", prestera_major);

    prestera_dev = (struct Prestera_Dev *) kmalloc(sizeof(struct Prestera_Dev), GFP_KERNEL);
    if (!prestera_dev)
    {
        printk("\nPresteraDrv: Failed allocating memory for device\n");
        result = -ENOMEM;
        goto fail;
    }

    /* create proc entry */
    create_proc_read_entry("mvPP", 0, NULL, prestera_read_proc_mem, NULL);

    /* initialize the device main semaphore */
    sema_init(&prestera_dev->sem, 1);

    result = pp_find_all_devices();
    if (0 != result)
    {
        goto fail;
    }

    result = initInterrupts();
    if (0 != result)
    {
        goto fail;
    }
 
    dma_base = __pa(high_memory);
    dma_len  = 2 * _1MB;
    if (!request_mem_region(dma_base, dma_len, "prestera-dma")) 
    {
        printk("Cannot reserve DMA region 0x%lx @ 0x%lx\n",
                dma_base, dma_len);
        return -ENODEV;
    }
    /* check dma area */
    dma_area = (unsigned long *)ioremap_nocache(dma_base, dma_len);

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
