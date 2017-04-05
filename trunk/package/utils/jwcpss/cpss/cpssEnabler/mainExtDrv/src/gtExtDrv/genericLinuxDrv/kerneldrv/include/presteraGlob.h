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
* presteraGlob.h
*
* DESCRIPTION:
*       This file includes the declaration of the struct we want to send to kernel mode,
*       from user mode.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/
#ifndef __INCPresteraGlobh
#define __INCPresteraGlobh

#ifndef __KERNEL__
#  include <sys/ioctl.h>
   /* uint32_t uintptr_t and so on */
#  include <inttypes.h>
#  include <stddef.h>
#  ifdef  PRESTERA_SYSCALLS
#    include <linux/unistd.h>
#  endif
#  include <gtExtDrv/drivers/prvExtDrvLinuxMapping.h>
#else /* !defined(__KERNEL__) */
    /* uint32_t uintptr_t and so on */
#   include <linux/version.h>
#   include <linux/types.h>
#   include "prvExtDrvLinuxMapping.h"
#   if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
#       ifndef _UINTPTR_T
#           define _UINTPTR_T
            typedef unsigned long uintptr_t;
#       endif
#   endif
#endif /* !defined(__KERNEL__) */


#ifndef MV_KMOD_TYPEDEFS
#define MV_KMOD_TYPEDEFS
/* Marvell typedefs */
/* mv_phys_addr_t - integer enough to store physical addresses (64 bit for EP3041A, x86+PAE, etc) */
#if defined(CONFIG_EP3041A) || defined(PHYSADDR_IS_64BIT)
   typedef uint64_t mv_phys_addr_t;
#  define MV_PHYS_ADDR_FMT "0x%0llx"
#else
   /* other cases phys addresses = virt addresses */
   typedef uintptr_t mv_phys_addr_t;
#  define MV_PHYS_ADDR_FMT "0x%0lx"
#endif
#if defined(KERNEL_IS_64BIT)
   typedef uint64_t mv_kmod_uintptr_t;
   typedef uint64_t mv_kmod_size_t;
#else
   typedef uintptr_t mv_kmod_uintptr_t;
   typedef size_t mv_kmod_size_t;
#endif
#endif /* MV_KMOD_TYPEDEFS */
#define mv_board_id_t       uint16_t


struct PciConfigReg_STC {
	uint32_t    busNo;
	uint32_t    devSel;
	uint32_t    funcNo;
	uint32_t    regAddr;
	uint32_t    data;
};

struct GT_PCI_Dev_STC {
	uint16_t    vendorId;
	uint16_t    devId;
	uint32_t    instance;
	uint32_t    busNo;
	uint32_t    devSel;
	uint32_t    funcNo;
};

struct GT_PCI_Mapping_STC {
	uint32_t    busNo;
	uint32_t    devSel;
	uint32_t    funcNo;
	mv_kmod_size_t  regsSize;
	struct {
		mv_kmod_uintptr_t       addr;
		mv_kmod_size_t          length;
		mv_kmod_size_t          offset;
	} mapConfig, mapRegs, mapDfx;
};

/*TD*/
struct GT_Intr2Vec {
	uint32_t    intrLine;
	uint32_t    bus;
	uint32_t    device;
	uint32_t    vector;
};

/*TD*/
struct GT_VectorCookie_STC {
	uint32_t            vector;
	mv_kmod_uintptr_t   cookie;
};

struct GT_RANGE_STC {
	mv_kmod_uintptr_t address;
	mv_kmod_size_t    length;
};

struct GT_DmaReadWrite_STC {
	mv_kmod_uintptr_t   address;
	mv_kmod_size_t      length;
	mv_kmod_size_t      burstLimit;
	mv_kmod_uintptr_t   buffer;
};

struct GT_TwsiReadWrite_STC {
	unsigned char       devId;  /* I2c slave ID                              */
	unsigned char       len;    /* pData array size (in chars).              */
	unsigned char       stop;   /* Indicates if stop bit is needed in the end  */
	mv_kmod_uintptr_t   pData;  /* Pointer to array of chars (address / data)*/
};

struct GT_DragoniteSwDownload_STC {
	uint32_t            length;
	mv_kmod_uintptr_t   buffer;
};

struct GT_DragoniteMemAccsess_STC {
	mv_kmod_uintptr_t   buffer;
	uint32_t            offset;
	uint32_t            length;
};

struct GT_PCI_VMA_ADDRESSES_STC {
	mv_kmod_uintptr_t   dmaBase;
	mv_kmod_uintptr_t   ppConfigBase;
	mv_kmod_uintptr_t   ppRegsBase;
	mv_kmod_uintptr_t   ppDfxBase;
	mv_kmod_uintptr_t   xCatDraginiteBase;
	mv_kmod_uintptr_t   hsuBaseAddr;
};

struct GT_PCI_MMAP_INFO_STC {
	uint32_t            index;
	mv_kmod_uintptr_t   addr;
	mv_kmod_size_t      length;
	mv_kmod_size_t      offset;
};


#define PRV_CPSS_EXT_DRV_XCAT_DRAGONITE_BASE_ADDR_CNS   LINUX_VMA_DRAGONITE
#define PRV_CPSS_EXT_DRV_HSU_BASE_ADDR_CNS              LINUX_VMA_HSU


#define PRESTERA_IOC_MAGIC 'p'
#define PRESTERA_IOC_HWRESET           _IO(PRESTERA_IOC_MAGIC,   0)
#define PRESTERA_IOC_INTCONNECT        _IOWR(PRESTERA_IOC_MAGIC, 3, struct GT_VectorCookie_STC)
#define PRESTERA_IOC_INTENABLE         _IOW(PRESTERA_IOC_MAGIC,  4, mv_kmod_uintptr_t)
#define PRESTERA_IOC_INTDISABLE        _IOW(PRESTERA_IOC_MAGIC,  5, mv_kmod_uintptr_t)
#define PRESTERA_IOC_WAIT              _IOW(PRESTERA_IOC_MAGIC,  6, mv_kmod_uintptr_t)
#define PRESTERA_IOC_FIND_DEV          _IOWR(PRESTERA_IOC_MAGIC, 7, struct GT_PCI_Dev_STC) 
#define PRESTERA_IOC_PCICONFIGWRITEREG _IOW(PRESTERA_IOC_MAGIC,  8, struct PciConfigReg_STC)
#define PRESTERA_IOC_PCICONFIGREADREG  _IOWR(PRESTERA_IOC_MAGIC, 9, struct PciConfigReg_STC)
#define PRESTERA_IOC_GETINTVEC         _IOWR(PRESTERA_IOC_MAGIC,10, struct GT_Intr2Vec)
#define PRESTERA_IOC_FLUSH             _IOW(PRESTERA_IOC_MAGIC, 11, struct GT_RANGE_STC)
#define PRESTERA_IOC_INVALIDATE        _IOW(PRESTERA_IOC_MAGIC, 12, struct GT_RANGE_STC)
#define PRESTERA_IOC_GETBASEADDR       _IOR(PRESTERA_IOC_MAGIC, 13, mv_phys_addr_t*)
#define PRESTERA_IOC_DMAWRITE          _IOW(PRESTERA_IOC_MAGIC, 14, struct GT_DmaReadWrite_STC)
#define PRESTERA_IOC_DMAREAD           _IOW(PRESTERA_IOC_MAGIC, 15, struct GT_DmaReadWrite_STC)
#define PRESTERA_IOC_GETDMASIZE        _IOR(PRESTERA_IOC_MAGIC, 16, mv_kmod_size_t*)
#define PRESTERA_IOC_TWSIINITDRV       _IO(PRESTERA_IOC_MAGIC, 17)
#define PRESTERA_IOC_TWSIWAITNOBUSY    _IO(PRESTERA_IOC_MAGIC, 18)
#define PRESTERA_IOC_TWSIWRITE         _IOW(PRESTERA_IOC_MAGIC, 19, struct GT_TwsiReadWrite_STC)
#define PRESTERA_IOC_TWSIREAD          _IOWR(PRESTERA_IOC_MAGIC, 20, struct GT_TwsiReadWrite_STC)
#define PRESTERA_IOC_DRAGONITESWDOWNLOAD _IOW(PRESTERA_IOC_MAGIC, 21, struct GT_DragoniteSwDownload_STC)
#define PRESTERA_IOC_DRAGONITEENABLE   _IOW(PRESTERA_IOC_MAGIC, 22, mv_kmod_uintptr_t)
#define PRESTERA_IOC_DRAGONITEINIT     _IO(PRESTERA_IOC_MAGIC,  23)
#define PRESTERA_IOC_DRAGONITEGETINTVEC  _IOR(PRESTERA_IOC_MAGIC, 24, uint32_t*)
#define PRESTERA_IOC_DRAGONITESHAREDMEMREAD  _IOWR(PRESTERA_IOC_MAGIC, 25, struct GT_DragoniteMemAccsess_STC)
#define PRESTERA_IOC_DRAGONITESHAREDMEMWRITE _IOW(PRESTERA_IOC_MAGIC, 26, struct GT_DragoniteMemAccsess_STC)
#define PRESTERA_IOC_DRAGONITEITCMCRCCHECK   _IO(PRESTERA_IOC_MAGIC,  27)
#define PRESTERA_IOC_HSUWARMRESTART          _IO(PRESTERA_IOC_MAGIC,  28)
#define PRESTERA_IOC_GETMAPPING              _IOWR(PRESTERA_IOC_MAGIC, 29, struct GT_PCI_Mapping_STC) 

#define PRESTERA_IOC_ISFIRSTCLIENT     _IO(PRESTERA_IOC_MAGIC,  30)
#define PRESTERA_IOC_GETVMA            _IOR(PRESTERA_IOC_MAGIC,  31, struct GT_PCI_VMA_ADDRESSES_STC)
#define PRESTERA_IOC_GETMMAPINFO       _IOWR(PRESTERA_IOC_MAGIC, 32, struct GT_PCI_MMAP_INFO_STC)
#define PRESTERA_IOC_GETBOARDID        _IOWR(PRESTERA_IOC_MAGIC,	33,	mv_board_id_t*)

#ifdef  PRESTERA_SYSCALLS
/********************************************************
 *
 * Syscall numbers for
 *
 *      long prestera_ctl(unsigned int cmd, unsigned long param)
 *
 ********************************************************/
#  define   __NR_prestera_ctl   __NR_setxattr
#endif /* PRESTERA_SYSCALLS */

#ifndef __KERNEL__
  extern GT_32 gtPpFd;
# ifdef  PRESTERA_SYSCALLS
#       include <unistd.h>
#       include <sys/syscall.h>
#       define prestera_ctl(cmd,arg)    syscall(__NR_prestera_ctl, (cmd), (arg))
# else
#   define prestera_ctl(cmd,arg)   ioctl(gtPpFd,cmd,arg)
# endif
#endif

#endif /* __INCPresteraGlobh */
