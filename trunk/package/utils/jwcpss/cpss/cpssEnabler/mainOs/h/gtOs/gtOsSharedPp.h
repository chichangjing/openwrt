/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsSharedPp.h
*
* DESCRIPTION:
*       File contains routines for Packet Processor Initialization
*       (applicable ONLY for BM).
*       Also routine for sections mapping registry is implemented here. 
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __gtOsSharedPph
#define __gtOsSharedPph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************* Includes *******************************************************/

#include <sys/mman.h>
#include <gtOs/gtGenTypes.h>

#define PP_REGISTRY_SIZE_CNS 64

/************* Typedefs ********************************************************/
/*
 * Typedef: struct SECTIONS_STCT
 *
 * Description:
 *      This structure presents 
 *
 * Fields:i
 *      startPtr    - start address to be mapped
 *      length      - length of block to be mapped
 *      offset      - offset into file to be mapped
 *
 *******************************************************************************/
typedef struct PP_MMAP_INFO_STCT
{
    GT_VOID	*startPtr;
    size_t	length;
    off_t   offset;
} PP_MMAP_INFO_STC;

/************* Functions ******************************************************/

/*******************************************************************************
* shrMemRegisterPpMmapSection
*
* DESCRIPTION:
*       Registers section which should be mmaped by all non-first clients.
*
* INPUTS:
*       startPtr    - start address to be mapped
*       length      - length of block to be mapped
*       offset      - offset into file to be mapped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success
*       GT_OUT_OF_RANGE - too many sections for static registry,
*                         must be increased 
*
* COMMENTS:
*       Operation is relevant only for SHARED_MEMORY=1
*
*******************************************************************************/
GT_STATUS shrMemRegisterPpMmapSection
(
    IN GT_VOID  *startPtr,
    IN size_t   length,
    IN off_t    offset
);

/*******************************************************************************
* shrMemSharedPpInit
*
* DESCRIPTION:
*       Performs sharing operation for DMA region.
*
* INPUTS:
*       isFirstClient - the-first-client flag 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Operation is relevant only for SHARED_MEMORY=1
*
*******************************************************************************/
GT_STATUS shrMemSharedPpInit(IN GT_BOOL isFirstCLient);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __gtOsSharedPph */
