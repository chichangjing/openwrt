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
*       FreeBSD kernel loadable device driver
*
* DEPENDENCIES:
*       - FSP
*
* NOTES:
*
* For guidelines see 
* http://www.freebsd.org/doc/en_US.ISO8859-1/books/arch-handbook/driverbasics.html
*
* For example/skeleton code see 
*   http://www.captain.at/programming/freebsd/echodev.php
*
* Another code example distributed with FreeBSD: /usr/share/examples/kld/cdev/
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/conf.h>		/* cdevsw stuff */
#include <sys/kernel.h>		/* SYSINIT stuff */
#include <sys/uio.h>		/* SYSINIT stuff */
#include <sys/malloc.h>		/* malloc region definitions */
#include <sys/module.h>
#include <sys/proc.h>
#include <sys/time.h>

#ifdef STANDALONE_TEST
#include "include/pssBspApis.h"
#else
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssTypes.h>
#define _FreeBSD
#include <gtExtDrv/drivers/pssBspApis.h>
#endif 

#include "./include/prestera_glob.h"
#include "./include/presteraIrq.h"

/* #define INT_DEBUG */
/* #define DMA_MEM_MNG */
/* #define LEVEL_0_DEBUG */
/* #define FIND_DEV_DEBUG */

/* #define EXTDRV_DEBUG */
#ifdef EXTDRV_DEBUG
#define DBG_PRINTF(x)   printf x
#else 
#define DBG_PRINTF(x)
#endif 

/* Typedefs */

static d_open_t		prestera_open;
static d_close_t	prestera_close;
static d_ioctl_t	prestera_ioctl;

static struct cdevsw prestera_cdevsw = {
	.d_version  =	D_VERSION,
	.d_open     =	prestera_open,
	.d_close    =	prestera_close,
	.d_ioctl    =	prestera_ioctl,
	.d_name     =	"mvPP"
};

/* Globals */

extern void *pmap_mapdev(vm_offset_t, vm_size_t);
extern unsigned int mv_high_memory_paddr;

/* Locals */

struct cdev *prestera_dev; /* on PCI express bus may be connected only one device, so 
                            * didn't build structure type for array of devices -
                            * all characteristical data is in specific variables
                            */
static int                  prestera_opened      = 0;
static unsigned long        dma_base;

static int *dummy;
static void *evtchn_waddr = &dummy;

/*******************************************************************************
* prestera_loader
*
* DESCRIPTION:
*       Device module event handler - in this case execute actions required for
*										the driver load/unload.
*
* INPUTS:
*       struct module *m - Each module in the kernel is described by a module_t 
*							structure. The structure contains the name of the 
*							device, a unique ID number, a pointer to an event 
*							handler function and to an argument, which is given 
*							to the event handler, as well as some kernel internal 
*							data.
*       int         what - type of event
*       void        *arg - argument provided as third parameter for DEV_MODULE macro
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       0 - on success
*		error code - otherwise
*
* COMMENTS:
*		If the module returns non-zero to MOD_UNLOAD, the unload will not happen.
*		MOD_UNLOAD should only fail if it is impossible to unload the module, for 
*		instance because there are memory references to the module which cannot be
*		revoked.
*
*******************************************************************************/

static int prestera_loader
(
    struct module   *m, 
    int              what, 
    void            *arg
)
{
    int err = 0;

    switch (what) 
    {
        case MOD_LOAD:                /* insmod */

#ifdef LEVEL_0_DEBUG
            DBG_PRINTF(("prestera_loader:MOD_LOAD;\n"));
#endif 
            prestera_dev = make_dev(&prestera_cdevsw,
                                    0,
                                    UID_ROOT,
                                    GID_WHEEL,
                                    0666,
                                    "mvPP");

            /* use OS external call and special OS variable - mv_high_memory_paddr,
             * to get base dma address, second param is page size
             */
            dma_base = (unsigned long)pmap_mapdev(mv_high_memory_paddr, 4096);

#ifdef DMA_MEM_MNG 
			DBG_PRINTF(("prestera_loader:mv_high_memory_paddr=0x%x,dma_base=0x%lx;\n", mv_high_memory_paddr, dma_base));
#endif 
        break;

        case MOD_UNLOAD:
		case MOD_SHUTDOWN:

#ifdef LEVEL_0_DEBUG
			DBG_PRINTF(("prestera_loader:MOD_UNLOAD/MOD_SHUTDOWN;\n"));
#endif 
            destroy_dev(prestera_dev);

        break;

        default:

            err = EOPNOTSUPP;

        break;
    }

    return(err);
}


/*******************************************************************************
* prestera_isr
*
* DESCRIPTION:
*       Interrupt service routine
*
* INPUTS:
*    cookie - pinter to cookie describing interrupt
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       0 - on success
*
* COMMENTS:
*		none.
*
*******************************************************************************/
static int prestera_isr
(
	IN unsigned long *cookie
)
{
	struct intData *intDataPtr;
	GT_STATUS		rc;

#ifdef INT_DEBUG
	DBG_PRINTF(("prestera_isr:start\n"));
#endif 

	intDataPtr = (struct intData*)cookie;

	if((rc = bspIntDisable(intDataPtr->intVec)) != GT_OK)
	{		
#ifdef INT_DEBUG
	DBG_PRINTF(("prestera_isr:bspIntDisable fail:intVec=%d,rc=%d\n", 
				intDataPtr->intVec, rc));
#endif 
		return rc;
	}

#ifdef INT_DEBUG
	DBG_PRINTF(("prestera_isr:bspIntDisable:intVec=%d\n", 
				intDataPtr->intVec));
#endif 

	/* wakeup return void nothing to check for return status */
	wakeup(evtchn_waddr);

#ifdef INT_DEBUG
	DBG_PRINTF(("prestera_isr:finish\n"));
#endif 

	return 0;
}

/*******************************************************************************
* prestera_ioctl
*
* DESCRIPTION:
*       Device specific control - execute actions upon ioctl() system call parameters
*
* INPUTS:
*    struct cdev     *dev - device
*    u_long           cmd - The device specific operation to perform.
*    caddr_t          arg - argument provided as third parameter of ioctl(2)
*							system call
*    int              mode - different flags ???
*    struct thread   *td - The calling thread.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       0 - on success
*		error code - otherwise
*
* COMMENTS:
*		If the module returns non-zero to MOD_UNLOAD, the unload will not happen.
*		MOD_UNLOAD should only fail if it is impossible to unload the module, for 
*		instance because there are memory references to the module which cannot be
*		revoked.
*
*******************************************************************************/

static int prestera_ioctl
(
    struct cdev     *dev, 
    u_long           cmd, 
    caddr_t          arg, 
    int              mode,
    struct thread   *td
)
{
    PciConfigReg_STC    pciConfReg;
    GT_PCI_Dev_STC      gtDev;
    GT_Intr2Vec         int2vec;
    GT_VecotrCookie_STC vector_cookie;
    GT_RANGE_STC        range;
    int                 error = 0;    
    GT_DMA_MEM_STC      dmaMemPrms;
    GT_DMA_MALLOC_STC   cacheDmaMallocPrms;    
	GT_STATUS			rc;
	struct intData 		intData;
	char				wmesg[6];
	int 				err;

#ifdef LEVEL_0_DEBUG
	struct proc         *procp = td->td_proc;

    DBG_PRINTF(("prestera_ioctl: dev_t=%d, cmd=%lx, arg=%p, mode=%x procp=%p\n",
	            dev2udev(dev), cmd, arg, mode, procp));
#endif 

    /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
    if (IOCGROUP(cmd) != PRESTERA_IOC_MAGIC)
    {
        printf("wrong ioctl magic key\n");
        return ENOTTY;
    }

    switch(cmd)
    {
        case PRESTERA_IOC_CACHEDMAMALLOC:			

			bcopy(arg, &cacheDmaMallocPrms, sizeof(GT_DMA_MALLOC_STC));

			#ifdef DMA_MEM_MNG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_CACHEDMAMALLOC:size=%lu\n",
							cacheDmaMallocPrms.bytes));
			#endif

            if((cacheDmaMallocPrms.ptr = bspCacheDmaMalloc(cacheDmaMallocPrms.bytes)) == NULL)
			{
				printf("prestera_ioctl:bspCacheDmaMalloc failed,size=%lu\n", 
					   cacheDmaMallocPrms.bytes);
				error = ENODEV;
			}

			#ifdef DMA_MEM_MNG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_CACHEDMAMALLOC:ptr=0x%x\n",
							cacheDmaMallocPrms.ptr));
			#endif

			bcopy(&cacheDmaMallocPrms, arg, sizeof(GT_DMA_MALLOC_STC));

            break;

        case PRESTERA_IOC_CACHEDMAFREE:

			#ifdef DMA_MEM_MNG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_CACHEDMAFREE:ptr=0x%x\n", arg));
			#endif

			if((rc = bspCacheDmaFree((void*)arg)) != GT_OK)
			{
				printf("prestera_ioctl:bspCacheDmaFree failed,ptr=0x%x\n", (unsigned int)arg);
				error = ENODEV;
			}

            break;

        case PRESTERA_IOC_READDMA:

			#ifdef DMA_IO_DBG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_READDMA\n"));
			#endif 

			bcopy(arg, &dmaMemPrms, sizeof(GT_DMA_MEM_STC));
            
            if(bspDmaRead(dmaMemPrms.address, dmaMemPrms.length, 
                            dmaMemPrms.burstLimit, dmaMemPrms.buffer) != GT_OK)
            {
                error = ENODEV;
            }

			bcopy(&dmaMemPrms, arg, sizeof(GT_DMA_MEM_STC));

            break;

        case PRESTERA_IOC_WRITEDMA:

			#ifdef DMA_IO_DBG
				DBG_PRINTF(("prestera_ioctl: PRESTERA_IOC_WRITEDMA\n"));
			#endif

			bcopy(arg, &dmaMemPrms, sizeof(GT_DMA_MEM_STC));
			
            if(bspDmaWrite(dmaMemPrms.address, dmaMemPrms.buffer, 
                            dmaMemPrms.length, dmaMemPrms.burstLimit) != GT_OK)
            {
                error = ENODEV;
            }
            break;

		case PRESTERA_IOC_HWRESET_INIT:

			DBG_PRINTF(("prestera_ioctl: PRESTERA_IOC_HWRESET_INIT\n"));
			bspResetInit(); 
			break;

		case PRESTERA_IOC_HWRESET:

			DBG_PRINTF(("prestera_ioctl: PRESTERA_IOC_HWRESET\n"));
			bspReset();
            break;

        case PRESTERA_IOC_INTCONNECT:

			bcopy(arg, &vector_cookie, sizeof(GT_VecotrCookie_STC));

			#ifdef INT_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_INTCONNECT:vectNum=%d\n", 
							vector_cookie.vector));
			#endif 

			intConnect(vector_cookie.vector, (void*)prestera_isr, (struct intData **)&vector_cookie.cookie);

			bcopy(&vector_cookie, arg, sizeof(GT_VecotrCookie_STC));			

            break;

        case PRESTERA_IOC_INTENABLE:
			
			#ifdef INT_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_INTENABLE:arg=%d\n", *((GT_U32*)*arg)));
			#endif 
            if((rc = bspIntEnable((GT_U32)*((long*)arg))) != GT_OK)
			{
				error = ENODEV;
			}

			#ifdef INT_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_INTENABLE:bspIntEnable rc=%d\n", rc));
			#endif 

            break;

        case PRESTERA_IOC_INTDISABLE:

			#ifdef INT_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_INTDISABLE:arg=%d\n", *((GT_U32*)*arg)));
			#endif 

            if((rc = bspIntDisable((GT_U32)*((long*)arg))) != GT_OK)
			{
				error = ENODEV;
			}

			#ifdef INT_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_INTDISABLE:bspIntDisable rc=%d\n", rc));
			#endif 

            break;

        case PRESTERA_IOC_WAIT:

				bcopy((struct intData*)*((long*)arg), &intData, sizeof(struct intData));

				#ifdef INT_DEBUG
					DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_WAIT:intVec=%d\n", intData.intVec));
				#endif 

                /* enable the interrupt vector */
                if((rc = bspIntEnable(intData.intVec)) != GT_OK)
				{
					error = ENODEV;
				}

				#ifdef INT_DEBUG
					DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_WAIT:bspIntEnable rc=%d\n", rc));
				#endif 

				/* go sleep untill interrupt happens */
				sprintf(wmesg, "intT%lu", intData.intVec);
				if((err = tsleep(evtchn_waddr, PWAIT | PCATCH, wmesg, 0)) < 0)
				{
					#ifdef INT_DEBUG
						DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_WAIT:tsleep failed:%d\n", err));
					#endif 
					error = ENODEV;
				}

				#ifdef INT_DEBUG
					DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_WAIT:%s wakedup\n", wmesg));
				#endif

            break;

        case PRESTERA_IOC_FIND_DEV:

			bcopy(arg, &gtDev, sizeof(GT_PCI_Dev_STC));

			#ifdef FIND_DEV_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_FIND_DEV:IN:vendorId=0x%x,devId=0x%x,instance=0x%lx\n",
							gtDev.vendorId, gtDev.devId, gtDev.instance));
			#endif

			if(bspPciFindDev(gtDev.vendorId, gtDev.devId, gtDev.instance,
                             &gtDev.busNo, &gtDev.devSel, &gtDev.funcNo) != GT_OK)
            {
				#ifdef FIND_DEV_DEBUG
					DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_FIND_DEV:bspPciFindDev failed\n"));
				#endif
				error = ENODEV;
            }

			#ifdef FIND_DEV_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_FIND_DEV:OUT:busNo=0x%lx,devSel=0x%lx,funcNo=0x%lx\n",
							gtDev.busNo, gtDev.devSel, gtDev.funcNo));
			#endif
			
			bcopy(&gtDev, arg, sizeof(GT_PCI_Dev_STC));

            break;

        case PRESTERA_IOC_PCICONFIGWRITEREG:

			bcopy(arg, &pciConfReg, sizeof(PciConfigReg_STC));

			/* #define PCICONFIGWRITEREG_DEBUG */
			#ifdef PCICONFIGWRITEREG_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_PCICONFIGWRITEREG:IN:busNo=0x%lx,devSel=0x%lx,funcNo=0x%lx,addr=0x%lx,data=0x%lx\n", 
							pciConfReg.busNo, pciConfReg.devSel, pciConfReg.funcNo, pciConfReg.regAddr, pciConfReg.data));
			#endif

            if(bspPciConfigWriteReg(pciConfReg.busNo, pciConfReg.devSel, 
                                        pciConfReg.funcNo, pciConfReg.regAddr, 
                                        pciConfReg.data) != GT_OK)
            {
                error = ENODEV;
            }

			#ifdef PCICONFIGWRITEREG_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_PCICONFIGWRITEREG:result=%d\n", error));
			#endif

			break;

        case PRESTERA_IOC_PCICONFIGREADREG:            

			bcopy(arg, &pciConfReg, sizeof(PciConfigReg_STC));

			/* #define PCICONFIGREADREG_DEBUG */
			#ifdef PCICONFIGREADREG_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_PCICONFIGREADREG:IN:busNo=0x%x,devSel=0x%x,funcNo=0x%x,addr=0x%x\n", 
							pciConfReg.busNo, pciConfReg.devSel, pciConfReg.funcNo, pciConfReg.regAddr));
			#endif

			if(bspPciConfigReadReg(pciConfReg.busNo, pciConfReg.devSel, 
                                    pciConfReg.funcNo, pciConfReg.regAddr, 
                                    &pciConfReg.data) != GT_OK)
            {
                error = ENODEV;
            }

			#ifdef PCICONFIGREADREG_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_PCICONFIGREADREG:OUT:data=0x%x\n", pciConfReg.data));
			#endif

			bcopy(&pciConfReg, arg, sizeof(PciConfigReg_STC));

            break;

        case PRESTERA_IOC_GETINTVEC:

			bcopy(arg, &int2vec, sizeof(GT_Intr2Vec));            

			#ifdef INT_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_GETINTVEC:IN:intrLine=%d\n", 
							int2vec.intrLine));
			#endif 			

			bspPciGetIntVec(int2vec.intrLine, (void*)&int2vec.vector);

			#ifdef INT_DEBUG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_GETINTVEC:OUT:vector=%d\n", 
							int2vec.vector));
			#endif 

			bcopy(&int2vec, arg, sizeof(GT_Intr2Vec));

            break;

        case PRESTERA_IOC_FLUSH:

			bcopy(arg, &range, sizeof(GT_RANGE_STC));

			DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_FLUSH:addr=0x%lx,len=0x%lx\n", 
						range.address, range.length));

            bspCacheFlush(bspCacheType_DataCache_E, (void*)range.address,
                          range.length);
            break;

        case PRESTERA_IOC_INVALIDATE:

			bcopy(arg, &range, sizeof(GT_RANGE_STC));

			DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_INVALIDATE:addr=0x%lx,len=0x%lx\n", 
						range.address, range.length));
            
            bspCacheInvalidate(bspCacheType_DataCache_E, (void*)range.address,
                               range.length);
			break;

        case PRESTERA_IOC_GETBASEADDR:

			#ifdef DMA_MEM_MNG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_GETBASEADDR:dma_base=0x%lx\n", dma_base));
			#endif 

			bcopy(&dma_base, arg, sizeof(unsigned long));            

            break;

		case PRESTERA_IOC_PHY2VIRT:

		{
			unsigned long phyAddr, virtAddr;

			bcopy(arg, &phyAddr, sizeof(unsigned long));			

			#ifdef DMA_MEM_MNG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_PHY2VIRT:phyAddr=0x%lx\n", phyAddr));
			#endif 

			if((virtAddr = bspPhys2Virt(phyAddr)) == 0)
			{
				error = ENODEV;
			}
			else
			{
				bcopy(&virtAddr, arg, sizeof(unsigned long));				
			}

			#ifdef DMA_MEM_MNG
				DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_PHY2VIRT:virtAddr=0x%lx\n", virtAddr));
			#endif 
		}	

		break;

		case PRESTERA_IOC_VIRT2PHY:

			{
				unsigned long phyAddr, virtAddr;				

				bcopy(arg, &virtAddr, sizeof(unsigned long));			

				#ifdef DMA_MEM_MNG
					DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_VIRT2PHY:virtAddr=0x%lx\n", virtAddr));
				#endif 

				if((phyAddr = bspVirt2Phys(virtAddr)) == 0)
				{
					error = ENODEV;
				}
				else
				{					
					bcopy(&phyAddr, arg, sizeof(unsigned long));				
				}

				#ifdef DMA_MEM_MNG
					DBG_PRINTF(("prestera_ioctl:PRESTERA_IOC_VIRT2PHY:phyAddr=0x%lx\n", phyAddr));
				#endif 
			}	

		break;

        case PRESTERA_IOC_ISFIRSTCLIENT:
            return (prestera_opened == 1) ? 0 : 1;

        default:
            error = EINVAL;
    }

#ifdef LEVEL_0_DEBUG
    DBG_PRINTF(("prestera_ioctl: return=%d(dec)\n", error));
#endif 

    return error;
}


/*******************************************************************************
* prestera_open
*
* DESCRIPTION:
*       Device open - execute actions upon open() system call
*
* INPUTS:
*    struct cdev     *dev - device
*    int             flag - flags provided with open(2) call
*    int             otyp - device type ???
*    struct thread   *td - The calling thread.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       0 - on success
*		error code - otherwise
*
* COMMENTS:
*		None.
*
*******************************************************************************/

static int prestera_open
(
    struct cdev    *dev, 
    int             flag, 
    int             otyp, 
    struct thread  *td
)
{
#ifdef LEVEL_0_DEBUG
    struct proc *procp = td->td_proc;

    DBG_PRINTF(("prestera_open: dev_t=%d, flag=%x, otyp=%x, procp=%p\n",
	            dev2udev(dev), flag, otyp, procp));
#endif 

    prestera_opened++;

    return 0;
}


/*******************************************************************************
* prestera_close
*
* DESCRIPTION:
*       Device close - execute actions upon close() system call
*
* INPUTS:
*    struct cdev     *dev - device
*    int             flag - flags provided with open(2) call
*    int             otyp - device type ???
*    struct thread   *td - The calling thread.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       0 - on success
*		error code - otherwise
*
* COMMENTS:
*		None.
*
*******************************************************************************/

static int prestera_close
(
    struct cdev    *dev, 
    int             flag, 
    int             otyp, 
    struct thread  *td
)
{
#ifdef LEVEL_0_DEBUG
    struct proc *procp = td->td_proc;

    DBG_PRINTF(("prestera_close: dev_t=%d, flag=%x, otyp=%x, procp=%p\n",
	            dev2udev(dev), flag, otyp, procp));
#endif 

    prestera_opened--;

    return 0;
}


DEV_MODULE(mvPp, prestera_loader, NULL);
