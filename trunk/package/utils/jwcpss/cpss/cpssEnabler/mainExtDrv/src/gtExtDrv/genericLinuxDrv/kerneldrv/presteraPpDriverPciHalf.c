/*******************************************************************************
* presteraPpDriverPciHalf.c
*
* DESCRIPTION:
*       PCI/PEX driver, 2 of 4 regions
*
* DEPENDENCIES:
*
*******************************************************************************/
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/io.h>

#include "./include/prestera.h"
#include "./include/presteraPpDriverGlob.h"

#include "./include/presteraDebug.h"

#define ADDR_COMP_REG   0
#define REG_ACCESS_ADDRESS(_regAddr, _compIdx) \
	(((_compIdx) << 24) | ((_regAddr) & 0x00ffffff))

struct ppDriverPciData_STC {
	struct PP_Dev      *dev;
	uintptr_t           ppRegsBase;
	uintptr_t           pciRegsBase;
	uintptr_t           dfxRegsBase;
	uint8_t             addrCompletShadow;
	spinlock_t          hwComplSem;
};


static void hwCompletion(
		struct ppDriverPciData_STC *drv,
		uint32_t                    regAddr,
		uint8_t                    *compIdxPtr,
		uintptr_t                  *addressPtr
)
{
	uint8_t     addrRegion; /* 8 bit MST value of PP internal address*/
	uintptr_t   address;  /*physical access address for PCI access */
	uintptr_t   compIdx; /* address completion register field index 0-3*/
	uint32_t    data;   /* data to be write to memory */

	/* check if addrRegion is 0 */
	if ((regAddr & 0xFF000000) == 0)
		compIdx = 0;
	else {
		spin_lock(&(drv->hwComplSem));

		compIdx = 1;
		addrRegion = (uint8_t)(regAddr >> 24);
		if (drv->addrCompletShadow != addrRegion) {
			/* Set addrRegion in AddrCompletion register */

			/*update Address Completion shadow*/
			drv->addrCompletShadow = addrRegion;

			/* update Hw Address Completion - using completion region 0 */
			address = drv->ppRegsBase + ADDR_COMP_REG;
			data = (drv->addrCompletShadow<<8);

			/*write the address completion 3 times.
			  because the PP have a 2 entry write buffer
			  so, the 3 writes will make sure we do get
			  to the hardware register itself */
			write_u32(data, address);
			write_u32(data, address);
			write_u32(data, address);
		}
	}

	address = drv->ppRegsBase + (uintptr_t)REG_ACCESS_ADDRESS(regAddr, compIdx);
	*compIdxPtr = compIdx;
	*addressPtr = address;
}

static int hwReadWrite(
		struct ppDriverPciData_STC *drv,
		int                         isReadOp,
		uint32_t                    regAddr,
		uint32_t                    length,
		uint32_t            *dataPtr
)
{
	uintptr_t address; /*physical address for PCI access */
	uint8_t   compIdx; /* address completion register field index 0-3*/
	uint32_t  j = 0;   /* count iterator for the write loop*/
	uint32_t  nextRegionAddr; /* address of the next region after the one
								 currently used */
	uint32_t  loopLength = 0; /* when length exceeds region addr, Set to end of
								 region range */
	uint32_t  data;

	hwCompletion(drv, regAddr, &compIdx, &address);

	/* check whether completion region boundaries exceeded*/
	nextRegionAddr =  (uint32_t)(drv->addrCompletShadow + 1)<<24;
	loopLength = length;
	if ((uintptr_t)(regAddr + length * 4) > nextRegionAddr)
		loopLength = (nextRegionAddr - regAddr) / 4;

	for (j = 0; j < loopLength; j++) {
		if (isReadOp) {
			data = read_u32(address);
			if (put_user(data, dataPtr+j)) {
				if (compIdx != 0)
					spin_unlock(&(drv->hwComplSem));
				return -EFAULT;
			}
		} else {
			if (get_user(data, dataPtr+j)) {
				if (compIdx != 0)
					spin_unlock(&(drv->hwComplSem));
				return -EFAULT;
			}

			write_u32(data, address);
		}

		address += 4;
	}
	if (compIdx != 0)
		spin_unlock(&(drv->hwComplSem));

	if (loopLength < length) {
		/* Recursive call for rest of data in next region.  */
		return hwReadWrite(drv, isReadOp, nextRegionAddr, length-loopLength,
				dataPtr+loopLength);
	}
	return 0;
}

static int presteraPpDriverPciPexRead(
		uintptr_t base,
		uint32_t  size,
		uint32_t  regAddr,
		uint32_t *dataPtr
)
{
	uint32_t data;
	if (base == 0 || regAddr >= size)
		return -EFAULT;

	base += regAddr;

	data = read_u32(base);

	if (put_user(data, dataPtr))
		return -EFAULT;

	return 0;
}

static int presteraPpDriverPciPexWrite(
		uintptr_t base,
		uint32_t  size,
		uint32_t  regAddr,
		uint32_t *dataPtr
)
{
	uint32_t data;
	if (base == 0 || regAddr >= size)
		return -EFAULT;

	if (get_user(data, dataPtr))
		return -EFAULT;

	base += regAddr;
	write_u32(data, base);

	return 0;
}

static int presteraPpDriverPciPexReset(struct ppDriverPciData_STC *drv)
{
	uintptr_t address;

	spin_lock(&(drv->hwComplSem));

	/* Update Address Completion shadow     */
	drv->addrCompletShadow = 0;

	/* Reset Hw Address Completion          */
	address = drv->ppRegsBase + ADDR_COMP_REG;
	write_u32(0, address);
	write_u32(0, address);
	write_u32(0, address);

	spin_unlock(&(drv->hwComplSem));

	return 0;
}

static int presteraPpDriverPciPexDestroy(struct ppDriverPciData_STC *drv)
{
	struct PP_Dev *dev = drv->dev;
	iounmap((void *)drv->ppRegsBase);
	iounmap((void *)drv->pciRegsBase);
	if (drv->dfxRegsBase)
		iounmap((void *)drv->dfxRegsBase);

	dev->ppdriver = (PP_DRIVER_FUNC)NULL;
	dev->ppdriverType = 0;
	dev->ppdriverData = NULL;

	kfree(drv);

	return 0;
}

static int presteraPpDriverPciPexIo(struct ppDriverPciData_STC *drv, struct mvPpDrvDriverIo_STC *io)
{
	if (io == NULL)
		return presteraPpDriverPciPexDestroy(drv);

	switch (io->op) {
	case mvPpDrvDriverIoOps_PpRegRead_E:
		return hwReadWrite(drv, 1, io->regAddr, 1, (uint32_t *)(io->dataPtr));
	case mvPpDrvDriverIoOps_PpRegWrite_E:
		return hwReadWrite(drv, 0, io->regAddr, 1, (uint32_t *)(io->dataPtr));
	case mvPpDrvDriverIoOps_RamRead_E:
		return hwReadWrite(drv, 1, io->regAddr, io->length, (uint32_t *)(io->dataPtr));
	case mvPpDrvDriverIoOps_RamWrite_E:
		return hwReadWrite(drv, 0, io->regAddr, io->length, (uint32_t *)(io->dataPtr));
	case mvPpDrvDriverIoOps_Reset_E:
		return presteraPpDriverPciPexReset(drv);
	case mvPpDrvDriverIoOps_Destroy_E:
		return presteraPpDriverPciPexDestroy(drv);
	case mvPpDrvDriverIoOps_PciRegRead_E:
		return presteraPpDriverPciPexRead(
				drv->pciRegsBase, drv->dev->config.size,
				io->regAddr, (uint32_t *)(io->dataPtr));
	case mvPpDrvDriverIoOps_PciRegWrite_E:
		return presteraPpDriverPciPexWrite(
				drv->pciRegsBase, drv->dev->config.size,
				io->regAddr, (uint32_t *)(io->dataPtr));
	case mvPpDrvDriverIoOps_DfxRegRead_E:
		return presteraPpDriverPciPexRead(
				drv->dfxRegsBase, drv->dev->dfx.size,
				io->regAddr, (uint32_t *)(io->dataPtr));
	case mvPpDrvDriverIoOps_DfxRegWrite_E:
		return presteraPpDriverPciPexWrite(
				drv->dfxRegsBase, drv->dev->dfx.size,
				io->regAddr, (uint32_t *)(io->dataPtr));
	}
	return -EFAULT;
}

int presteraPpDriverPciPexHalfCreate(struct PP_Dev *dev)
{
	struct ppDriverPciData_STC *drv;

	drv = kmalloc(sizeof(struct ppDriverPciData_STC), GFP_KERNEL);
	memset(drv, 0, sizeof(*drv));

	drv->dev = dev;

	drv->ppRegsBase = (uintptr_t)ioremap_nocache(dev->ppregs.phys, 32*1024*1024);
	drv->pciRegsBase = (uintptr_t)ioremap_nocache(dev->config.phys, dev->config.size);
	if (dev->dfx.phys)
		drv->dfxRegsBase = (uintptr_t)ioremap_nocache(dev->dfx.phys, dev->dfx.size);
	spin_lock_init(&(drv->hwComplSem));

	dev->ppdriver = (PP_DRIVER_FUNC)presteraPpDriverPciPexIo;
	dev->ppdriverType = (int)mvPpDrvDriverType_PciHalf_E;
	dev->ppdriverData = drv;

	return 0;
}
