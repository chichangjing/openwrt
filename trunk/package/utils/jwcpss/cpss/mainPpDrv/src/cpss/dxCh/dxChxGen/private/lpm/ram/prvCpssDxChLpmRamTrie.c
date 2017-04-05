/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTrie.c
*
* DESCRIPTION:
*       This file includes implementation of a binary trie, which is used for
*       keeping the history of the insertions to the LPM buckets (UC and MC).
*       The pData field in the trie node structure will include
*       pointers to the following structures:
*
*       1. When used for UC LPM purpose:
*           - pData holds a pointer to the route entry pointer inserted into the
*           LPM structure.
*
*       2. When used for MC LPM purpose:
*           - pData holds a pointer to the route entry pointer inserted into the
*           LPM structure.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTrie.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmTypes.h> 
#include <cpss/extServices/private/prvCpssBindFunc.h>

extern void * cpssOsLpmMalloc
(
    IN GT_U32 size
);

extern void cpssOsLpmFree
(
    IN void* const memblock
);

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
)
{
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *trieNode;                          /* The new inserted trie node.          */
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *fatherTrieNode;                    /* The father of the currently handled  */
                                                                            /* Trie node.                           */
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *allocatedTrieNodes[128] = {NULL};  /* list of allocated trie nodes         */
    GT_U32 bit = (GT_U32)-1;                                                /* Indicates whether the current node is*/
                                                                            /* the left or right son of his father. */
    GT_U32 i;

    trieNode = rootPtr;

    for (i = 0; i < prefix; i++)
    {
      fatherTrieNode = trieNode;
      bit = (addrPtr[i / 8] >> (trieDepth - 1 - (i % 8))) & 1;

      if(bit == 0)
          trieNode = trieNode->leftSon;
      else
          trieNode = trieNode->rightSon;

      if(trieNode == NULL)
      {
         trieNode = cpssOsLpmMalloc(sizeof(PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC));
         if(trieNode == NULL)
         {
             GT_U32 j;
             for (j = 0; j < i; j++)
             {
                 if (allocatedTrieNodes[j]->father != NULL)
                 {
                    if(allocatedTrieNodes[j]->father->leftSon == allocatedTrieNodes[j])
                        allocatedTrieNodes[j]->father->leftSon = NULL;
                    if(allocatedTrieNodes[j]->father->rightSon == allocatedTrieNodes[j])
                        allocatedTrieNodes[j]->father->rightSon = NULL;
                 }
                 allocatedTrieNodes[j]->father = NULL;
                 if (allocatedTrieNodes[j]->leftSon != NULL)
                 {
                     allocatedTrieNodes[j]->leftSon->father = NULL;
                 }
                 if (allocatedTrieNodes[j]->rightSon != NULL)
                 {
                     allocatedTrieNodes[j]->rightSon->father = NULL;
                 }

                 cpssOsLpmFree(allocatedTrieNodes[j]);
                 allocatedTrieNodes[j] = NULL;
             }
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
         }
         allocatedTrieNodes[i] = trieNode;
         trieNode->father     = fatherTrieNode;
         trieNode->leftSon    = trieNode->rightSon = NULL;
         trieNode->pData      = NULL;

         /* update the father son with the new node */
         if(bit == 0)
             fatherTrieNode->leftSon = trieNode;
         else
             fatherTrieNode->rightSon = trieNode;
      }
    }

    trieNode->pData   = entry;

    return GT_OK;
}

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
)
{
    GT_U32 bit;                 /* Indicates whether the current node is    */
                                /* the left or right son of his father.     */
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *trieNode;   /* The currently handled Trie node.      */
    GT_U32 i;

    trieNode = root;

    for (i = 0; i < prefix; i++)
    {
        bit = (addr[i /8] >> (trieDepth - 1 - (i % 8))) & 1;
        if(bit == 0)
            trieNode = trieNode->leftSon;
        else
            trieNode = trieNode->rightSon;

        /* The node was not found in the tree */
        if(trieNode == NULL)
        {
            if(entry != NULL)
                *entry = NULL;
            return NULL;
        }
    }

    if(trieNode->pData == NULL)
    {
        if(entry != NULL)
            *entry = NULL;
        return NULL;
    }

    /* The requested node was found */
    if(entry != NULL)
    {
        *entry = trieNode->pData;
    }
    return trieNode;
}

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
)
{
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *trieNode;       /* The currently handled node.          */
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *fatherTrieNode; /* Father of the currently handled node.*/
    GT_U32 bit;                 /* Indicates whether the current node is    */
                                /* the left or right son of his father.     */
    GT_U32 i;

    trieNode = rootPtr;

    /* first we need to find the node to be removed */
    for(i = 0; i < prefix; i++)
    {
        bit = (addrPtr[i / 8] >> (trieDepth - 1 - (i % 8))) & 1;
         if(bit == 0)
            trieNode = trieNode->leftSon;
        else
            trieNode = trieNode->rightSon;

        /* The node was not found in the tree */
        if(trieNode == NULL)
        {
            if(entryPtr != NULL)
                *entryPtr = NULL;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    if(trieNode->pData == NULL)
    {
        *entryPtr = NULL;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    else
    {
        if(entryPtr != NULL)
        {
            *entryPtr = trieNode->pData;
        }
    }

    /* check if this node is a leaf - i.e the node has no sons  */
    if((trieNode->leftSon == NULL) && (trieNode->rightSon == NULL))
    {

        /* delete all the invalid nodes in the way from         */
        /* trieNode to the root, until reaching a valid one.  */
        while(trieNode != rootPtr)
        {
            /* the node has no sons */
            if((trieNode->leftSon == NULL) && (trieNode->rightSon == NULL))
            {
                fatherTrieNode = trieNode->father;

                /* update the father of trieNode to point to NULL */
                if(fatherTrieNode->leftSon == trieNode)
                    fatherTrieNode->leftSon = NULL;
                else
                    fatherTrieNode->rightSon = NULL;

                cpssOsLpmFree(trieNode);

                /* this father is valid - stop pruning */
                if(fatherTrieNode->pData != NULL)
                    break;

                trieNode = fatherTrieNode;
            }
            /* father with one valid branch - stop pruning  */
            else
                break;
        }
    }
    else
    {   /* This node has at list one son so */
        /* just invalid the node itself.    */
        trieNode->pData = NULL;
    }

    return GT_OK;
}

/*******************************************************************************
* findNextValidNode
*
* DESCRIPTION:
*       This function find the closest valid node to a given node.
*
* INPUTS:
*       root - The root of the trie.
*       firstNode  - A pointer to the node that the search will start from.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       The search is performed by the following alg:
*       Start searching at the node firstNode that is given as a parameter
*       then search the left branch ,and then the right one.
*       If firstNode is a valid node, then the function will look for the next
*       valid node, and will ignore the given node.
*       The function uses sonsVisited parameter to mark which of the sons had
*       been allready searched.
*
*******************************************************************************/
static PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC* findNextValidNode
(
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *root,
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *firstNode
)
{

    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *currNode;/* Currently handled node. */
    GT_U32 sonsVisited = 0;     /* 0- no sons visited yet,      */
                                /* 1- visited left son only     */
                                /* 2- visited left & right sons */

    currNode = firstNode;

    do{
        /* allready visited all the subTree */
        if (sonsVisited == 2)
        {
            if (currNode->father == NULL)
                return NULL;
            else
            {
                sonsVisited =((currNode->father->leftSon == currNode)? 0:1) + 1;
                currNode = currNode->father;
            }
        }

        /* we got into this node from the father */
        if (sonsVisited == 0)
        {
            if (currNode->leftSon != NULL)
            {
                currNode = currNode->leftSon;
                sonsVisited = 0;
            }
            else
            {
                sonsVisited = 1;
                if(currNode != firstNode)
                    continue;
            }
        }

        /* the left sub-tree was done, handle the right sub-tree */
        if (sonsVisited == 1)
        {
            if (currNode->rightSon != NULL)
            {
                currNode = currNode->rightSon;
                sonsVisited = 0;
            }
            else
                sonsVisited = 2;

            continue;
        }

    }while ( !(((currNode == root) && (sonsVisited == 2)) ||
              ((sonsVisited != 2) && (currNode->pData != NULL))));

    return currNode;
}

/*******************************************************************************
* buildAddressFromTrieNode
*
* DESCRIPTION:
*       This function gets a pointer to a node in the trie and returns the
*       address and prefix represented by this node.
*
* INPUTS:
*       trieNodePtr   - A pointer to the tree node to calculate the address for
*       trieDepth     - The maximum depth of the trie.
*
* OUTPUTS:
*       addrPtr       - Holds the calculated addr.
*       prefixPtr     - The prefix of AddrPtr.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void buildAddressFromTrieNode
(
    IN  PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *trieNodePtr,
    IN  GT_U32                              trieDepth,
    OUT GT_U8                               *addrPtr,
    OUT GT_U32                              *prefixPtr
)
{
    GT_U8  calcAddr = 0;       /* address & prefix calculated by scanning the */
    GT_U32 calcPrefix = 0;     /* way up to the trie's root.               */
    GT_U8  sonType = 0;

    if (trieDepth  > 8)
        return;

    /* climb up the tree until the root node    */
    while(trieNodePtr->father != NULL)
    {
        sonType = (GT_U8)((trieNodePtr->father->leftSon == trieNodePtr)? 0:1);
        calcAddr = (GT_U8)(calcAddr | (sonType << calcPrefix));
        calcPrefix++;
        trieNodePtr = trieNodePtr->father;
    }

    *prefixPtr = calcPrefix;
    *addrPtr = (GT_U8)(calcAddr << (trieDepth - calcPrefix));

    return;
}

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
*       prefixPtr - the prefix of the address.
*
* OUTPUTS:
*       addrPtr   - The address associated with the next node.
*       prefixPtr - The prefix associated with the address.
*       entryPtr  - A pointer to the entry if found, NULL otherwise.
*
* RETURNS:
*       GT_OK if succeeded, GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  If the given node is the last node in the trie then the values of
*           the address and prefix will not be changed, and entry will get NULL.
*       2.  If force == GT_TRUE and (address,prefix) is invalid, then this
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
)
{
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *trieNode;   /* Currently search node.  */
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC *retTrieNode;/* The Trie search node.*/
    GT_U8 tmpAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];
    GT_U32 tmpPrefix = 0;

    /* Get the pointer to the start node    */
    trieNode = prvCpssDxChLpmTrieSearch(rootPtr,addrPtr,*prefixPtr,trieDepth,NULL);

    if((trieNode == NULL) && (force == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if((trieNode == NULL) && (force == GT_TRUE))
        trieNode = rootPtr;

    /* look for the next valid node */
    retTrieNode = findNextValidNode(rootPtr,trieNode);

    if(retTrieNode != NULL)
    {
        buildAddressFromTrieNode(retTrieNode,trieDepth,tmpAddr,&tmpPrefix);

        if (tmpPrefix != 0)
        {
            cpssOsMemCpy(addrPtr,tmpAddr,(tmpPrefix +7) /8);
            *prefixPtr = tmpPrefix;
            if(entryPtr != NULL)
            {
                *entryPtr = retTrieNode->pData;
            }
        }
        else if(entryPtr != NULL)
            *entryPtr = NULL;
    }

    return GT_OK;
}

