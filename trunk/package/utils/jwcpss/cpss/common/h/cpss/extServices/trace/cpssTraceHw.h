/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTraceHw.h
*
* DESCRIPTION:
*       HW Trace wrapper. Trace facility.
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifndef __cpssTraceHwh
#define __cpssTraceHwh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*
 * Typedef: enum CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT
 *
 * Description: Memory space type
 *
 * Enumerations:
 *  CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E        - usual (not PCI/PEX or DFX) 
 *                                                address space.
 *  CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E   - PCI/PEX address space. 
 *  CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E   - Reset and Init 
 *                                              Controller (DFX) address space.
 */
typedef enum
{
    CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E                    = 0,
    CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E               = 1,
    CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E   = 2
} CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT;


/*******************************************************************************
* CPSS_TRACE_HW_ACCESS_WRITE_FUNC
*
* DESCRIPTION:
*       Trace HW write access information.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       isrContext  - GT_TRUE: called from ISR context (cpssDrvPpHwIsrWrite, 
*                                                       cpssDrvPpHwIsrRead)
*                    GT_FALSE: called from not ISR context.
*       addrSpace   - registers address space
*       addr        - start address that the access was made to
*       length      - length of the data that was written in words
*       dataPtr     - (pointer to) data that was written
*       
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_TRACE_HW_ACCESS_WRITE_FUNC)
(
    IN GT_U8  	devNum,
    IN GT_U32 	portGroupId,
    IN GT_BOOL 	isrContext, 
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT 	addrSpace, 
    IN GT_U32 	addr,	
    IN GT_U32 	length,	
    IN GT_U32 	*dataPtr
);

/*******************************************************************************
* CPSS_TRACE_HW_ACCESS_READ_FUNC
*
* DESCRIPTION:
*       Trace HW read access information.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       isrContext  - GT_TRUE: called from ISR context (cpssDrvPpHwIsrWrite, 
*                                                       cpssDrvPpHwIsrRead)
*                    GT_FALSE: called from not ISR context.
*       addrSpace   - registers address space
*       addr        - start address that the access was made to
*       length      - length of the data that was written in words
*       dataPtr     - (pointer to) data that was read
*       
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_TRACE_HW_ACCESS_READ_FUNC)
(
    IN GT_U8  	devNum,
    IN GT_U32 	portGroupId,
    IN GT_BOOL 	isrContext, 
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT 	addrSpace, 
    IN GT_U32 	addr,	
    IN GT_U32 	length,	
    IN GT_U32 	*dataPtr
);

/*******************************************************************************
* CPSS_TRACE_HW_ACCESS_DELAY_FUNC
*
* DESCRIPTION:
*       Trace HW delay access information.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       millisec    - time in milli seconds the delay was called
*       
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_TRACE_HW_ACCESS_DELAY_FUNC)
(
    IN GT_U8  	devNum,
    IN GT_U32 	portGroupId,
    IN GT_U32 	millisec	
);

/* CPSS_TRACE_HW_ACCESS_FUNC_BIND_STC -
    structure that hold the "Trace HW" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_TRACE_HW_ACCESS_WRITE_FUNC    traceHwAccessWriteFunc;
    CPSS_TRACE_HW_ACCESS_READ_FUNC     traceHwAccessReadFunc;
    CPSS_TRACE_HW_ACCESS_DELAY_FUNC     traceHwAccessDelayFunc;
}CPSS_TRACE_HW_ACCESS_FUNC_BIND_STC;


#ifdef __cplusplus
}
#endif

#endif  /* __cpssTraceHwh */


