/*******************************************************************************
* presteraPpDriverPci.c
*
* DESCRIPTION:
*       PCI/PEX driver, 4 regions
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

/* offset of the address completion for PEX 8 completion regions */
#define PEX_MBUS_ADDR_COMP_REG_MAC(_index) (0x120 + (4 * _index))
/* bits of address passes as is throw PCI window */
#define NOT_ADDRESS_COMPLETION_BITS_NUM_CNS 19
/* bits of address extracted from address completion registers */
#define ADDRESS_COMPLETION_BITS_MASK_CNS    (0xFFFFFFFF << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS)

#define REG_ACCESS_ADDRESS(_regAddr, _compIdx) \
	(((_compIdx) << NOT_ADDRESS_COMPLETION_BITS_NUM_CNS) | \
	 ((_regAddr) & (~ADDRESS_COMPLETION_BITS_MASK_CNS)))

struct ppDriverPexMbusData_STC {
	struct PP_Dev      *dev;
	uintptr_t           ppRegsBase;
	uintptr_t           pciRegsBase;
	uintptr_t           dfxRegsBase;
	uint32_t            addrCompletShadow[8];
	spinlock_t          hwComplSem;
	uint8_t             compIdx;
};


static void hwCompletion(
	struct ppDriverPexMbusData_STC *drv,
	uint32_t                        regAddr,
	uint8_t                        *compIdxPtr,
	uintptr_t                      *addressPtr
)
{
	uint32_t    addrRegion;  /* 13 bit MSB value of PP internal address */
	uintptr_t   address;  /*physical access address for PCI access */
	uint32_t    compIdx; /* address completion register field index 0-7*/
	uint8_t     i;      /* count iterator for the completion index compare loop*/

	/* check if addrRegion is 0 */
	if ((regAddr & ADDRESS_COMPLETION_BITS_MASK_CNS) == 0) {
		compIdx = 0;
	} else {
		spin_lock(&(drv->hwComplSem));

		addrRegion = (regAddr >> NOT_ADDRESS_COMPLETION_BITS_NUM_CNS);
		/* compare addr region to existing Address regions*/
		for (i = 1; i < 8; i++) {
			if (addrRegion == drv->addrCompletShadow[i])
				break;
		}
		if (i == 8) {
			/* Set addrRegion in AddrCompletion register */

			/*round robin on Region index : 1,2,3*/
			drv->compIdx++;
			if (drv->compIdx > 7)
				drv->compIdx = 1;
			compIdx = drv->compIdx;

			/*update Address Completion shadow*/
			drv->addrCompletShadow[compIdx] = addrRegion;

			/* update Hw Address Completion - using completion region 0 */
			address = drv->ppRegsBase + PEX_MBUS_ADDR_COMP_REG_MAC(compIdx);
			write_u32(addrRegion, address);
		} else
			compIdx = i;
	}

	address = drv->ppRegsBase + (uintptr_t)REG_ACCESS_ADDRESS(regAddr, compIdx);
	*compIdxPtr = compIdx;
	*addressPtr = address;
}

static int hwReadWrite(
	struct ppDriverPexMbusData_STC *drv,
	int                             isReadOp,
	uint32_t                        regAddr,
	uint32_t                        length,
	uint32_t                        *dataPtr
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
	nextRegionAddr =  (uint32_t)(drv->addrCompletShadow[compIdx] + 1)<<NOT_ADDRESS_COMPLETION_BITS_NUM_CNS;
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
	uint32_t size,
	uint32_t regAddr,
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

static int presteraPpDriverPciPexReset(struct ppDriverPexMbusData_STC *drv)
{
	uintptr_t address;
	uint32_t  data;
	int       i;

	spin_lock(&(drv->hwComplSem));

	/* set 8-region mode: regAddr = 0x140, set bit16 to 0 */
	address = drv->ppRegsBase + 0x140;
	data = read_u32(address);
	data &= (~(1 << 16));
	write_u32(data, address);

	/* Update Address Completion shadow     */
	for (i = 0; i < 8; i++) {
		drv->addrCompletShadow[i] = 0;
		/* Reset Hw Address Completion          */
		address = drv->ppRegsBase+PEX_MBUS_ADDR_COMP_REG_MAC(i);
		write_u32(0, address);
	}
	drv->compIdx = 1;

	spin_unlock(&(drv->hwComplSem));

	return 0;
}

static int presteraPpDriverPciPexDestroy(struct ppDriverPexMbusData_STC *drv)
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

static int presteraPpDriverPexMbusIo(struct ppDriverPexMbusData_STC *drv, struct mvPpDrvDriverIo_STC *io)
{
	if (io == NULL) {
		/* destroy */
		return presteraPpDriverPciPexDestroy(drv);
	}
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

int presteraPpDriverPexMbusCreate(struct PP_Dev *dev)
{
	struct ppDriverPexMbusData_STC *drv;

	drv = kmalloc(sizeof(struct ppDriverPexMbusData_STC), GFP_KERNEL);
	memset(drv, 0, sizeof(*drv));

	drv->dev = dev;

	drv->ppRegsBase = (uintptr_t)ioremap_nocache(dev->ppregs.phys, 4*1024*1024);
	drv->pciRegsBase = (uintptr_t)ioremap_nocache(dev->config.phys, dev->config.size);
	if (dev->dfx.phys)
		drv->dfxRegsBase = (uintptr_t)ioremap_nocache(dev->dfx.phys, dev->dfx.size);
	spin_lock_init(&(drv->hwComplSem));
	drv->compIdx = 1;

	dev->ppdriver = (PP_DRIVER_FUNC)presteraPpDriverPexMbusIo;
	dev->ppdriverType = (int)mvPpDrvDriverType_PexMbus_E;
	dev->ppdriverData = drv;

	return 0;
}
