/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtMemoryUsageTrace.c
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

/********* include ************************************************************/

#include <gtUtil/gtMemoryUsageTrace.h>

/* needed for fast skipping of tracing */
static GT_BOOL gtMemoryUsageTraceEnable = GT_FALSE;

#define MAX_ACHOR_KEYS_CNS  32
#define MAX_ACHOR_KEYS_MEMO_CNS  2048

/* anchor keys - ancor enabled if one of keys is it's name substring substing */
typedef char* CHAR_PTR;
static char anchorKeyMemoArr[MAX_ACHOR_KEYS_MEMO_CNS];
static GT_U32 anchorKeyMemoArrUsed = 0;

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
)
{
    gtMemoryUsageTraceEnable = enable;
}

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
    IN   GT_VOID
)
{
    anchorKeyMemoArrUsed = 0;
}

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
)
{
    GT_U32 keySize;

    keySize = cpssOsStrlen(key) + 1;

    if ((anchorKeyMemoArrUsed + keySize) >= MAX_ACHOR_KEYS_MEMO_CNS) return;

    cpssOsStrCpy(&(anchorKeyMemoArr[anchorKeyMemoArrUsed]), key);
    anchorKeyMemoArrUsed += keySize;
}

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
)
{
    anchorPtr->allocatedBytes = osMemGetHeapBytesAllocated();
}

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
)
{
    GT_U32  allocatedBytes;
    char*  keyPtr;
    GT_BOOL enable;

    if (gtMemoryUsageTraceEnable == GT_FALSE) return;

    /* check if anchor is enabled by key */
    enable = GT_FALSE;
    for (keyPtr = &(anchorKeyMemoArr[0]);
          ((GT_U32)(keyPtr - &(anchorKeyMemoArr[0])) < anchorKeyMemoArrUsed);
          keyPtr += (cpssOsStrlen(keyPtr) + 1))
    {
        if (cpssOsStrStr(anchorPtr->anchorName, keyPtr) != NULL)
        {
            /* key found as substring in anchor name */
            enable = GT_TRUE;
            break;
        }
    }
    if (enable == GT_FALSE) return;

    allocatedBytes = osMemGetHeapBytesAllocated();

    if (anchorPtr->allocatedBytes == 0)
    {
        /* first anchor using - reset */
        anchorPtr->allocatedBytes = allocatedBytes;
        return;
    }

    if (anchorPtr->allocatedBytes == allocatedBytes)
    {
        /* no changes */
        return;
    }

    cpssOsPrintf(
        "total allocated bytes: old %d new %d at %s\n",
        anchorPtr->allocatedBytes, allocatedBytes, nodeName);
    if (anchorPtr->allocatedBytes < allocatedBytes)
    {
        cpssOsPrintf(
            "added allocated bytes  %d \n",
            (allocatedBytes - anchorPtr->allocatedBytes));
    }
    else
    {
        cpssOsPrintf(
            "freed bytes  %d \n",
            (anchorPtr->allocatedBytes - allocatedBytes));
    }

    /* update anchor */
    anchorPtr->allocatedBytes = allocatedBytes;
}
