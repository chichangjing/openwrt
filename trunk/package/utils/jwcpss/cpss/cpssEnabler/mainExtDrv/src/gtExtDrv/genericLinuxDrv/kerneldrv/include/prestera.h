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
* prestera.h
*
* DESCRIPTION:
*       Includes defines and structures needed by the PP device driver
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/
#ifndef __PRESTERA__
#define __PRESTERA__

#define PRESTERA_PP_DRIVER

#include <linux/version.h>
#include "presteraGlob.h"

/*    mv_resource_size_t - parameter type for request_mem_region() */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
  typedef resource_size_t mv_resource_size_t;
#else
  typedef unsigned long mv_resource_size_t;
#endif

/* only allow 2.6.12 and higher */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,12)
# error "This kernel is too old: not supported by this file"
#endif

#ifdef __BIG_ENDIAN
#define CPU_BE
#else
#define CPU_LE
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
#define V320PLUS
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,6)
#define V306PLUS
#endif

#if defined(CONFIG_ARCH_KIRKWOOD) && defined(V320PLUS)
#define XCAT340
#endif

#if defined(CONFIG_ARCH_FEROCEON_KW) || defined(CONFIG_ARCH_KIRKWOOD)
#define XCAT
#endif

#ifdef CONFIG_MV78200
#define DISCODUO
#endif

#ifdef CONFIG_GDA_8548
#define GDA8548
#endif

#ifdef CONFIG_EP3041A
#define EP3041A
#endif

#ifdef XCAT340
#define PRESTERA_MAJOR 244   /* major number */
#endif

#ifdef V306PLUS
#define PRESTERA_MAJOR 244   /* major number */
#endif

#if defined(CONFIG_X86_64) && defined(CONFIG_X86)
#define INTEL64
#define INTEL64_CPU
#define PRESTERA_MAJOR 244   /* major number */
#endif

#ifndef PRESTERA_MAJOR
#define PRESTERA_MAJOR 254   /* major number */
#endif

#ifndef MIPS64
#undef  GT_SYNC
#define GT_SYNC
#endif

struct Prestera_Dev
{
    struct semaphore        sem;            /* Mutual exclusion semaphore   */
    loff_t                  size;           /* prestera mem size            */
};

struct Mem_Region
{
    mv_phys_addr_t      phys;
    mv_resource_size_t  size;
    mv_phys_addr_t      allocbase;
    mv_resource_size_t  allocsize;
    uintptr_t     mmapbase;
    uintptr_t     mmapsize;
    size_t        mmapoffset;
};

struct Mmap_Info_stc
{
    enum {
        MMAP_INFO_TYPE_DMA_E,
        MMAP_INFO_TYPE_PP_CONF_E,
        MMAP_INFO_TYPE_PP_REGS_E,
        MMAP_INFO_TYPE_PP_DFX_E,
        MMAP_INFO_TYPE_PP_DRAGONITE_E,
        MMAP_INFO_TYPE_HSU_E
    }           map_type;
    int         index;
    uintptr_t   addr;
    size_t      length;
    size_t      offset;
};

#ifdef PRESTERA_PP_DRIVER
typedef int (*PP_DRIVER_FUNC)(void* drv, void *io);
#endif
struct PP_Dev
{
    uint16_t       devId;
    uint16_t       vendorId;
    uint32_t       instance;
    uint32_t       busNo;
    uint32_t       devSel;
    uint32_t       funcNo;
    struct Mem_Region config; /* Configuration space */
    struct Mem_Region ppregs; /* PP registers space */
    struct Mem_Region dfx; /* DFX space */

#ifdef PRESTERA_PP_DRIVER
    PP_DRIVER_FUNC ppdriver;
    int            ppdriverType;
    void*          ppdriverData;
#endif
};

#include <linux/io.h>
static inline uint32_t read_u32(uintptr_t addr)
{
        uint32_t data;
        data = __raw_readl((void*)addr); /* also converted from LE */
        GT_SYNC;/* to avoid read combine by some advanced CPUs */
        return le32_to_cpu(data);
}

static inline void  write_u32(uint32_t data, uintptr_t addr)
{
        data = cpu_to_le32(data);
        __raw_writel(data, (void*)addr);
        /* need to put GT_SYNC to avoid from having the CPU doing
        * write combine. Some CPUs like advanced new PPC can do write combining
        * of two writes to continuous addresses. So, to avoid that we use the eioio
        * in PPC architecture;*/
        GT_SYNC;
}

#define CPSS_DEV_END  0x7FFFFFFF

#define CPSS_DEV_TYPES       \
 PRV_CPSS_DX_DEVICES_MAC,    \
 PRV_CPSS_EXMX_DEVICES_MAC,  \
 PRV_CPSS_PUMA_DEVICES_MAC,  \
 PRV_CPSS_PUMA3_DEVICES_MAC,  \
 CPSS_98FX950_CNS,           \
 CPSS_LION_PORT_GROUPS_0123_CNS,   \
 CPSS_LION_PORT_GROUPS_01___CNS,   \
 PRV_CPSS_XCAT_8FE_2STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_8FE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_24FE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_8GE_2STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_8GE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_16GE_2STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_16GE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_16FE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_24GE_NO_STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_24GE_2STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT_24GE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT3_8GE_2STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT3_8GE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT3_16GE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT3_24GE_2STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT3_24GE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_8FE_2STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_8FE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_24FE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_8GE_2STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_8GE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_16GE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_16FE_4STACK_PORTS_DEVICES, \
 PRV_CPSS_XCAT2_24GE_4STACK_PORTS_DEVICES, \
 CPSS_LION2_PORT_GROUPS_01234567_CNS, \
 CPSS_LION2_HOOPER_PORT_GROUPS_0123_CNS, \
 PRV_CPSS_BOBCAT2_DEVICES, \
 PRV_CPSS_BOBK_DEVICES , \
 PRV_CPSS_BOBCAT3_ALL_DEVICES , \
 CPSS_DEV_END


#endif /* __PRESTERA__ */
