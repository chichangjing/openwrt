/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtMemoryUsageTrace.h
*
* DESCRIPTION:
*       Tool for tracing memory usage.
*       Based on osMemGetHeapBytesAllocated function;
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __gtMemoryUsageTraceh
#define __gtMemoryUsageTraceh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>
#include <gtOs/gtOsMem.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

/*
 * Typedef: struct GT_MEMORY_USAGE_TRACE_ANCHOR_STC
 *
 * Description:
 *     This structure describe memory usage trace anchor.
 *
 * Fields:
 *     anchorName       - Name of anchor.
 *     allocatedBytes   - total amount of bytes that was allocated
 *                           before last node processing.
 *
 *  Comments:
 *
 */
typedef struct
{
    const char*  anchorName;
    GT_U32 allocatedBytes;
} GT_MEMORY_USAGE_TRACE_ANCHOR_STC;

/* Trace Anchor declaration */
#define GT_MEMORY_USAGE_TRACE_ANCHOR_DEFINE_GEN_MAC(_var, _name) \
    GT_MEMORY_USAGE_TRACE_ANCHOR_STC _var = {_name, 0};

/* Trace Anchor declaration with short variable name */
#define GT_MEMORY_USAGE_TRACE_ANCHOR_DEFINE_MAC(_var, _name) \
    GT_MEMORY_USAGE_TRACE_ANCHOR_DEFINE_GEN_MAC(GT_MEMORY_USAGE_TRACE_ANCHOR##_var, _name) \

/* trace node */
#define GT_MEMORY_USAGE_TRACE_NODE_GEN_MAC(_anchor_var, _node_name) \
    gtMemoryUsageTraceNode(&_anchor_var, _node_name)

/* trace node with short variable name */
#define GT_MEMORY_USAGE_TRACE_NODE_MAC(_anchor_var, _node_name) \
    GT_MEMORY_USAGE_TRACE_NODE_GEN_MAC(GT_MEMORY_USAGE_TRACE_ANCHOR##_anchor_var, _node_name)

/* isolated node with it's own anchore to compare memory allocation */
/* at different executions of the same code lines                   */
#define GT_MEMORY_USAGE_TRACE_ISOLATED_NODE_MAC(_anchor_name, _node_name)           \
    {                                                                               \
        static GT_MEMORY_USAGE_TRACE_ANCHOR_DEFINE_GEN_MAC(__anchor, _anchor_name); \
        GT_MEMORY_USAGE_TRACE_NODE_GEN_MAC(__anchor, _node_name);                   \
    }

/*******************************************************************************
* gtMemoryUsageTraceEnableSet
*
* DESCRIPTION:
*           Enables/Disales Memory Usage Tracing
*
* INPUTS:
*       enable - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID  gtMemoryUsageTraceEnableSet
(
    IN      GT_BOOL enable
);

/*******************************************************************************
* gtMemoryUsageTraceAnchorKeysReset
*
* DESCRIPTION:
*           Resets anchor keys;
*
* INPUTS:
*       None.
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID  gtMemoryUsageTraceAnchorKeysReset
(
    IN    GT_VOID
);

/*******************************************************************************
* gtMemoryUsageTraceAnchorKeyAdd
*
* DESCRIPTION:
*           Add anchor keys.
*
* INPUTS:
*       key - anchor key to be addeed.
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID  gtMemoryUsageTraceAnchorKeyAdd
(
    IN      char* key
);

/*******************************************************************************
* gtMemoryUsageTraceAnchorReset
*
* DESCRIPTION:
*           Reset Anchor.
*
* INPUTS:
*       anchorPtr - (pointer to) anchor.
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID  gtMemoryUsageTraceAnchorReset
(
    IN      GT_MEMORY_USAGE_TRACE_ANCHOR_STC *anchorPtr
);

/*******************************************************************************
* gtMemoryUsageTraceNode
*
* DESCRIPTION:
*           Trace node.
*
* INPUTS:
*       anchorPtr - (pointer to) anchor.
*       nodeName  - name of checked node.
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID  gtMemoryUsageTraceNode
(
    IN      GT_MEMORY_USAGE_TRACE_ANCHOR_STC *anchorPtr,
    IN      const char*                      nodeName
);

#ifdef __cplusplus
}
#endif

#endif  /* __gtMemoryUsageTraceh */
/* Do Not Add Anything Below This Line */

