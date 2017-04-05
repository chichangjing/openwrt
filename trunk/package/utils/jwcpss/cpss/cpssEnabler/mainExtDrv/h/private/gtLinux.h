/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
* gtLinux.h - Header File for Basic gtCore logic functions and definitions
*
* DESCRIPTION:
*       This header file contains simple read/write macros for addressing
*       the SDRAM, devices, GT`s registers and PCI (using the PCI`s
*       address space). The macros take care of Big/Little endian conversions. 
*
* DEPENDENCIES:
*       None.
*
*       $Revision: 3 $
*
********************************************************************************/

#ifndef __INGtLinux
#define __INGtLinux

/*******************************************************************************
* extDrvGetDmaBase
*
* DESCRIPTION:
*       Get the base address of the DMA area need for Virt2Phys and Phys2Virt
*           translaton
*
* INPUTS:
*       None.
*       type        - type of cache memory data/intraction
*       address_PTR - starting address of memory block to flush
*       size        - size of memory block
*
* OUTPUTS:
*       dmsBase     - he base address of the DMA area.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
extern GT_STATUS extDrvGetDmaBase
(
	OUT GT_UINTPTR * dmaBase
);


#endif /* __INGtLinux */

