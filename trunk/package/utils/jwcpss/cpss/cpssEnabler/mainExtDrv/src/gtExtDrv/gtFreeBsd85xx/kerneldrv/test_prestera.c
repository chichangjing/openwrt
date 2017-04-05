#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include "include/pssBspApis.h"
#include "./include/prestera_glob.h"

typedef unsigned long ulong;

static void dump(int barnum, volatile unsigned long v_addr, int offset)
{
  printf("bar %d, offset 0x%08x = 0x%08lx\n", barnum,
         offset, *(unsigned long *)(v_addr+offset));
}

int main(void) 
{
	int fd, error, i;
	ulong tmp;
	volatile ulong *memPtr;
	ulong bar0 = 0;
	ulong bar2 = 0;

	fd = open("/dev/mvPP", O_RDWR);
	if (fd == -1)
		err(1, "open failed for /dev/mvPP");

	bar0 = 0x80000000;
	bar2 = 0x84000000;

	printf("bar0 = 0x%x\n", bar0);
	dump(0, bar0, 0x70000);	

	printf("bar2 = 0x%x\n", bar2);
	dump(2, bar2, 0x50);	

	memPtr = (ulong*)(bar2+0x58);
	tmp = *memPtr;
	printf("0x%x->0x%x\n", memPtr, tmp);
	tmp &= 0xfffffffe;
	*memPtr = tmp;
	tmp &= 0xdfffffff;
	*memPtr = tmp;
	tmp = *memPtr;
	printf("0x%x->0x%x\n", memPtr, tmp);

	/* sleep 1 sec. */
	usleep(100000);

	tmp = *memPtr;
	printf("0x%x->0x%x\n", memPtr, tmp);

/*
#define PRESTERA_IOC_HWRESET_INIT      _IO(PRESTERA_IOC_MAGIC,   0)
#define PRESTERA_IOC_HWRESET           _IO(PRESTERA_IOC_MAGIC,   1)
#define PRESTERA_IOC_WRITEDMA          _IOW(PRESTERA_IOC_MAGIC,  2,  GT_DMA_MEM_STC)
#define PRESTERA_IOC_READDMA           _IOWR(PRESTERA_IOC_MAGIC, 3,  GT_DMA_MEM_STC)
#define PRESTERA_IOC_CACHEDMAMALLOC    _IOWR(PRESTERA_IOC_MAGIC, 4,  GT_DMA_MALLOC_STC)
#define PRESTERA_IOC_CACHEDMAFREE      _IOW(PRESTERA_IOC_MAGIC,  5,  void*)
#define PRESTERA_IOC_INTCONNECT        _IOWR(PRESTERA_IOC_MAGIC, 6,  GT_VecotrCookie_STC)
#define PRESTERA_IOC_INTENABLE         _IOW(PRESTERA_IOC_MAGIC,  7,  long)
#define PRESTERA_IOC_INTDISABLE        _IOW(PRESTERA_IOC_MAGIC,  8,  long)
#define PRESTERA_IOC_WAIT              _IOW(PRESTERA_IOC_MAGIC,  9,  long)
#define PRESTERA_IOC_FIND_DEV          _IOWR(PRESTERA_IOC_MAGIC, 10, GT_PCI_Dev_STC) 
#define PRESTERA_IOC_PCICONFIGWRITEREG _IOW(PRESTERA_IOC_MAGIC,  11, PciConfigReg_STC)
#define PRESTERA_IOC_PCICONFIGREADREG  _IOWR(PRESTERA_IOC_MAGIC, 12, PciConfigReg_STC)
#define PRESTERA_IOC_GETINTVEC         _IOWR(PRESTERA_IOC_MAGIC, 13, GT_Intr2Vec)
#define PRESTERA_IOC_FLUSH             _IOW(PRESTERA_IOC_MAGIC,  14, GT_RANGE_STC)
#define PRESTERA_IOC_INVALIDATE        _IOW(PRESTERA_IOC_MAGIC,  15, GT_RANGE_STC)
#define PRESTERA_IOC_GETBASEADDR       _IOR(PRESTERA_IOC_MAGIC,  16, long*)
*/
/*
{
	long dmaBaseAddr;

	error = ioctl(fd, PRESTERA_IOC_GETBASEADDR, &dmaBaseAddr);
	if (error == -1)
		err(1, "ioctl");
	printf("PRESTERA_IOC_GETBASEADDR:dmaBaseAddr=0x%x\n", dmaBaseAddr);
}
*/
	close(fd);

	exit(0);
}
