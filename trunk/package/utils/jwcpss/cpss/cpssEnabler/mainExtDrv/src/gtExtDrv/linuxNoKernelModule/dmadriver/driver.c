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
* driver.c
*
* DESCRIPTION:
*       mvDmaDrv - driver to map DMA memory to userspace
*                  Usage:
*                     fd=open("/dev/mvDmaDrv",O_RDWR);
*                     mmap(,size, ..,fd,0) will allocate DMA block and map it
*                     ...
*                     close(fd) will unmap and free DMA block
*
*       Please note:
*           on intel CPU it requires 'intel_iommu=off' kernel option
*
* DEPENDENCIES:
*
*       $Revision: 31 $
*******************************************************************************/
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
#include <linux/device.h>
#include <linux/slab.h>

static int                  mvDmaDrv_major = 244;
static struct cdev          mvDmaDrv_cdev;
static struct class*        mvDmaDrv_class;
static struct device*       mvDmaDrv_device;

struct dma_mapping {
	void       *virt;
	dma_addr_t  dma;
	phys_addr_t phys;
	size_t      size;
	struct device *dev;
};

static int mvDmaDrv_mmap(struct file * file, struct vm_area_struct *vma)
{
	struct dma_mapping *m;

	printk("mvDmaDrv_mmap(file=%p)\n",file);
	if (file->private_data != NULL)
		return -ENXIO;

	m = kmalloc(sizeof(struct dma_mapping), GFP_KERNEL);
	if (!m)
		return -ENXIO;

	/* allocate DMA for PCI device if pgoff == 0x5XXYZ
	 *    where XX - pciBus:8bit
	 *    Y - pciDev:5bit
	 *    Z - pciFunc:3bit
	 */
	m->dev = NULL;
	if ((vma->vm_pgoff & (7<<16)) == (5<<16)) {
		unsigned int bus, devfn;
		struct pci_dev *pdev;
		bus = (vma->vm_pgoff >> 8) & 0xff;
		devfn = PCI_DEVFN(((vma->vm_pgoff >> 3) & 0x1f), (vma->vm_pgoff) & 0x07);
		printk("got a cookie, looking for bus %x devfn %x\n", bus, devfn);
		pdev = pci_get_bus_and_slot(bus, devfn);
		if (pdev != NULL) {
			m->dev = &(pdev->dev);
			printk("allocating for device %s\n", m->dev->kobj.name);
			if (dma_set_mask(m->dev, DMA_BIT_MASK(32)))
				dev_warn(m->dev, "No suitable DMA available\n");
		}
	}
	m->size = (size_t)(vma->vm_end - vma->vm_start);
	m->virt = dma_alloc_coherent(m->dev, m->size, &(m->dma), GFP_DMA32 | GFP_NOFS/*GFP_KERNEL*/);
	if (!m->virt) {
		printk("dma_alloc_coherent() failed to allocate %x bytes\n",(unsigned)m->size);
		kfree(m);
		return -ENXIO;
	}
	printk("dma_alloc_coherent() virt=%p dma=0x%llx\n", m->virt, (unsigned long long)m->dma);
#ifdef CONFIG_X86
	m->phys = dma_to_phys(m->dev, m->dma);
#else
	m->phys = (phys_addr_t)m->dma;
#endif
	printk("m->phys=0x%llx\n",(unsigned long long)m->phys);
	file->private_data = m;

	/* VM_IO for I/O memory */
	vma->vm_flags |= VM_IO;
	vma->vm_pgoff = m->phys >> PAGE_SHIFT;

#if 0
	/* disable caching on mapped memory */
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
#endif

	printk("remap_pfn_range(phys=0x%llx vm_start=0x%llx, vm_pgoff=0x%llx, vm_size=0x%lx, )\n",
				(unsigned long long)m->phys,
				(unsigned long long)vma->vm_start,
				(unsigned long long)vma->vm_pgoff,
				(unsigned long)m->size);

	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, m->size, vma->vm_page_prot)) {
		printk("remap_pfn_range failed\n");
		return -ENXIO;
	}

	return 0;
}


static int mvDmaDrv_open(struct inode *inode, struct file *file)
{
	printk("mvDmaDrv_open(file=%p)\n", file);
	file->private_data = NULL;

	return 0;
}

static int mvDmaDrv_release(struct inode *inode, struct file *file)
{
	printk("mvDmaDrv_release(file=%p)\n", file);
	if (file->private_data != NULL) {
		struct dma_mapping *m = (struct dma_mapping *)file->private_data;

		printk("dma_free_coherent(dev, 0x%lx, %p, 0x%llx)\n",
				(unsigned long)m->size, m->virt, (unsigned long long)m->dma);

		dma_free_coherent(m->dev ? m->dev : mvDmaDrv_device, m->size, m->virt, m->dma);
		kfree(m);
	}
	return 0;
}

static struct file_operations mvDmaDrv_fops = {
	.mmap   = mvDmaDrv_mmap,
	.open   = mvDmaDrv_open,
	.release= mvDmaDrv_release /* A.K.A close */
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,69)
static char *mvDmaDrv_devnode(struct device *dev, umode_t *mode)
#else /* < 3.4.69 */
static char *mvDmaDrv_devnode(struct device *dev, mode_t *mode)
#endif /* < 3.4.69 */
{
	return kasprintf(GFP_KERNEL, "%s", dev->kobj.name);
}

static void mvDmaDrv_cleanup(void)
{
	printk("mvDmaDrv Says: Bye world from kernel\n");

	device_destroy(mvDmaDrv_class, MKDEV(mvDmaDrv_major, 3));
	class_destroy(mvDmaDrv_class);
	cdev_del(&mvDmaDrv_cdev);

	unregister_chrdev_region(MKDEV(mvDmaDrv_major, 3), 1);
}

static int mvDmaDrv_init(void)
{
	int result = 0;
	dev_t dev = MKDEV(mvDmaDrv_major, 3);

	printk(KERN_DEBUG "mvDmaDrv_init\n");

	/* first thing register the device at OS */
	/* Register your major. */
	result = register_chrdev_region(dev, 1, "mvDmaDrv");
	if (result < 0) {
		printk("mvDmaDrv_init: register_chrdev_region err= %d\n", result);
		return result;
	}

	cdev_init(&mvDmaDrv_cdev, &mvDmaDrv_fops);
	mvDmaDrv_cdev.owner = THIS_MODULE;
	result = cdev_add(&mvDmaDrv_cdev, dev, 1);
	if (result) {
		printk("mvDmaDrv_init: cdev_add err= %d\n", result);
error_region:
		unregister_chrdev_region(dev, 1);
		return result;
	}
	mvDmaDrv_class = class_create(THIS_MODULE, "mvDmaDrv");
	if (IS_ERR(mvDmaDrv_class)) {
		printk(KERN_ERR "Error creating mvDmaDrv class.\n");
		cdev_del(&mvDmaDrv_cdev);
		result = PTR_ERR(mvDmaDrv_class);
		goto error_region;
	}
	mvDmaDrv_class->devnode = mvDmaDrv_devnode;
	mvDmaDrv_device = device_create(mvDmaDrv_class, NULL, dev, NULL, "mvDmaDrv");
#if 0
	if (dma_set_mask(mvDmaDrv_device, DMA_BIT_MASK(32))) {
		dev_warn(mvDmaDrv_device, "mydev: No suitable DMA available\n");
	}
#endif

	printk(KERN_DEBUG "mvDmaDrv_major = %d\n", mvDmaDrv_major);
	printk(KERN_DEBUG "init finished\n");

	return 0;
}

module_init(mvDmaDrv_init);
module_exit(mvDmaDrv_cleanup);

MODULE_AUTHOR("Marvell Semi.");
MODULE_LICENSE("GPL");

