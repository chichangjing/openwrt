/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRamTrie.h
*
* DESCRIPTION:
*       This file includes implementation of a binary trie, which is used for
*       keeping the history of the insertions to the MC tree structure and to
*       LPM buckets (for UC and MC).
*       The pData field in the trie node structure will include
*       pointers to the following structures:
*
*       1. When used for UC LPM purpose:
*           - pData holds a pointer to the route entry pointer inserted into the
*             LPM structure.
*
*       2. When used for MC LPM purpose:
*           - pData holds a pointer to the route entry pointer inserted into the
*             LPM structure.
*
*       3. When used for MC Tree purpose:
*           - pData holds mc src bucket which holds a pointer to the
*             shadow bucket in CPU memory holding the source prefixes and the
*             HW address in PP's RAM of the source LPM structure.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 0 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmRamTrieh
#define __prvCpssDxChLpmRamTrieh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STCT
 *
 * Description: A struct representing a TRIE node
 *
 * Fields:
 *      father - the parent node
 *      leftSon - the left son - represent 0 bit in the address
 *      rightSon - the right son - represent 1 bit in the address
 *      pData - A pointer to the data, also indicates validity if != NULL
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STCT
{
   struct PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STCT    *father;
   struct PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STCT    *leftSon;
   struct PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STCT    *rightSon;
   GT_PTR                                         pData;
} PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC;

/*******************************************************************************
* prvCpssDxChLpmTrieInsert
*
* DESCRIPTION:
*       Insert a new NEXT_POINTER_PTR to the trie structure.
*
* INPUTS:
*       rootPtr    - Root of the trie to insert the entry.
*       addrPtr    - address associated with the given entry.
*       prefix     - prefix of addrPtr.
*       trieDepth  - The maximum depth of the trie.
*       entry      - The entry to be inserted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - if succeeded
*       GT_OUT_OF_CPU_MEM         - if failed to allocate memory
*       GT_FAIL                   - otherwise
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTrieInsert
(
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC  *rootPtr,
    IN GT_U8                                *addrPtr,
    IN GT_U32                               prefix,
    IN GT_U32                               trieDepth,
    IN GT_PTR                               entry
);

/*******************************************************************************
* prvCpssDxChLpmTrieSearch
*
* DESCRIPTION:
*       Search for the entry which is associated with address & prefix .
*
* INPUTS:
*       root    - Root of the trie to search for the entry.
*       addr    - the address associated with the given entry.
*       prefix  - prefix of addr.
*       trieDepth - The maximum depth of the trie.
*       entry   - Indicates whether to return the entry in the searched node
*                 or not.
*
* OUTPUTS:
*       entry   - A pointer to the searched entry if found, NULL otherwise.
*
* RETURNS:
*       A pointer to the node in the trie including the searched entry.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC* prvCpssDxChLpmTrieSearch
(
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC  *root,
    IN GT_U8                                *addr,
    IN GT_U32                               prefix,
    IN GT_U32                               trieDepth,
    INOUT GT_PTR                            *entry
);

/*******************************************************************************
* prvCpssDxChLpmTrieDel
*
* DESCRIPTION:
*       Deletes a previously inserted entry.
*
* INPUTS:
*       rootPtr   - Root of the trie to delete from.
*       addrPtr   - the address associated with the entry to be deleted.
*       prefix    - the prefix of the address.
*       trieDepth - The maximum depth of the trie.
*
* OUTPUTS:
*       entryPtr  - A pointer to the entry of the deleted node if found, NULL
*                   otherwise.
* RETURNS:
*       GT_OK                     - if succeeded
*       GT_FAIL                   - otherwise
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       This function does not free the pointers to the data stored in the
*       trie node, it's the responsibility of the caller to do that.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTrieDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC  *rootPtr,
    IN GT_U8                                *addrPtr,
    IN GT_U32                               prefix,
    IN GT_U32                               trieDepth,
    OUT GT_PTR                              *entryPtr
);

/*******************************************************************************
* prvCpssDxChLpmTrieGetNext
*
* DESCRIPTION:
*       This function searchs the trie for the next valid node. The search is
*       started from the given address & prefix.
*
* INPUTS:
*       rootPtr   - Root of the trie to be searched.
*       trieDepth - The maximum depth of the trie.
*       force     - Indicates whether to find the next valid entry even if the
*                   given address,prefix are not valid.
*       addrPtr   - the address associated with the search start point.
*       prefixPtr - prefix of ipAddr.
*
* OUTPUTS:
*       addrPtr   - The address associated with the next node.
*       prefixPtr - The prefix associated with ipAddr.
*       entryPtr  - A pointer to the entry if found, NULL otherwise.
*
* RETURNS:
*       GT_OK if succeeded, GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  If the given node is the last node in the trie then the values of
*           ipAddr and prefix will not be changed, and entry will get NULL.
*       2.  If force == GT_TRUE and (ipAddr,ipPrefix) is invalid, then this
*           function will find the first valid node.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTrieGetNext
(
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC  *rootPtr,
    IN GT_U32                               trieDepth,
    IN GT_BOOL                              force,
    INOUT GT_U8                             *addrPtr,
    INOUT GT_U32                            *prefixPtr,
    OUT GT_PTR                              *entryPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmRamTrieh */

