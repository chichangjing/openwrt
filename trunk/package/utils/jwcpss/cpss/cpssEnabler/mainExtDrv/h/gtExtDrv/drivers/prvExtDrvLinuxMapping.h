/*******************************************************************************
*              (c), Copyright 2014, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvExtDrvLinuxMapping.h
*
* DESCRIPTION:
*       This file contains a set of userspace virtual addresses used in CPSS
*       (Linux only)
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
* COMMENT:
*       See also mainOs/h/gtOs/gtOsSharedMemoryRemapper.h
*
*******************************************************************************/
#ifndef __prvExtDrvLinuxMapping_h
#define __prvExtDrvLinuxMapping_h

/* addresses which must fit in 32bit address space */
/* DMA memory (2M) */
#define LINUX_VMA_DMABASE       0x19000000 /* 2M */

/* Defined in mainOs/h/gtOs/gtOsSharedMemoryRemapper.h:
SHARED_MEMORY_MALLOC_VIRT_ADDR_MAC 0x11000000
*/


#define LINUX_VMA_PP_CONF_BASE  0x19400000
#define LINUX_VMA_PP_DFX_BASE   0x1b000000
#define LINUX_VMA_PP_REGS_BASE  0x20000000

/* XCAT only */
#define LINUX_VMA_DRAGONITE     0x80000000

/* HAS_HSU */
/* if xcat then 0x90000000 else 0x40000000 */
#if (defined(__KERNEL__) && defined(CONFIG_ARCH_FEROCEON_KW)) || (!defined(__KERNEL__) && defined(XCAT_DRV))
#define LINUX_VMA_HSU           0x90000000
#else
#define LINUX_VMA_HSU           0x40000000
#endif

#endif /* __prvExtDrvLinuxMapping_h */
