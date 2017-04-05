/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssAvlTree.c
*
* DESCRIPTION:
*       AVL Tree implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/


/********* include ***********************************************************/

#include <cpssCommon/private/prvCpssAvlTree.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/cpssPresteraDefs.h>

/************ Internal Typedefs ***********************************************/


typedef GT_COMP_RES (*prototypeCmpFunc)(void* elem_PTR1,void* elem_PTR2);
typedef void (*prototypeDataFreeFunc)(void* cookiePtr, void* dataPtr);

/*
 * Typedef: enum PRV_CPSS_AVLSKEW_ENT
 *
 * Description: Which of a given node's subtrees is higher?
 *
 */
typedef enum
{
    PRV_CPSS_AVLSKEW_NONE_E,
    PRV_CPSS_AVLSKEW_LEFT_E,
    PRV_CPSS_AVLSKEW_RIGHT_E
}PRV_CPSS_AVLSKEW_ENT;


/*
 * Typedef: enum PRV_CPSS_AVL_RES_ENT
 *
 * Description: Private error code for private insertion/deletion function.
 *
 */
typedef enum
{
    PRV_CPSS_AVL_RES_OK_E,
    PRV_CPSS_AVL_RES_BALANCE_E,
    PRV_CPSS_AVL_RES_EXIST_E,   /* Item already exist                  */
    PRV_CPSS_AVL_RES_NO_NEXT_E, /* No next item for the requested item */
    PRV_CPSS_AVL_RES_NO_MEM_E,  /* No memory available                 */
    PRV_CPSS_AVL_RES_ERROR_E    /* General error                       */

} PRV_CPSS_AVL_RES_ENT;

/*
 * Typedef: PRV_CPSS_AVL_NODE_STC
 *
 * Description: Defines node for the AVL tree.
 *
 * Fields:
 *          left, *right - Pointer to left/right sub-tree
 *          pData - data holded at that node.
 *          skew  - Determin which side of the node is skew.
 *
 */
typedef struct prvCpssAvlnode
{
    struct prvCpssAvlnode *left, *right;
    GT_VOID*              pData;
    PRV_CPSS_AVLSKEW_ENT  skew;

} PRV_CPSS_AVL_NODE_STC;


/*
 * Typedef: struct PRV_CPSS_AVL_HEAD_STC
 *
 * Description: Defines node for the AVL tree.
 *
 * Fields:
 *          compareFunc - Comparison function used to sort the nodes in the tree.
 *                        recieves two node's data and returns:
 *                              GT_EQUAL / GT_GREATER / GT_SMALLER
 *
 *          PoolId      - Buffer's pool to use for nodes allocation.
 *                        If that field is CPSS_BM_POOL_NULL_ID, the cpssOsMalloc is used.
 *          pHead       - Hold the tree's head.
 *
 *          IsFree      - TRUE indicates that entry is free for use
 */
typedef struct
{
    prototypeCmpFunc         compareFunc;
    CPSS_BM_POOL_ID          poolId;
    PRV_CPSS_AVL_NODE_STC*   pHead;

} PRV_CPSS_AVL_HEAD_STC;


/************ Private Functions ************************************************/

static int prvCpssAvlFindLowest
(
    IN     PRV_CPSS_AVL_NODE_STC*  target,
    IN     PRV_CPSS_AVL_RES_ENT*                 res,
    IN     CPSS_BM_POOL_ID         poolId,
    INOUT  PRV_CPSS_AVL_NODE_STC** ppNode
);

static int prvCpssAvlFindHighest
(
    IN     PRV_CPSS_AVL_NODE_STC*  target,
    IN     PRV_CPSS_AVL_RES_ENT*                 res,
    IN     CPSS_BM_POOL_ID         poolId,
    INOUT  PRV_CPSS_AVL_NODE_STC** ppNode
);

static PRV_CPSS_AVL_RES_ENT prvCpssAvlLeftShrunk
(
    INOUT PRV_CPSS_AVL_NODE_STC** ppNode
);

static PRV_CPSS_AVL_RES_ENT prvCpssAvlRightShrunk
(
    INOUT PRV_CPSS_AVL_NODE_STC** ppNode
);

static void prvCpssAvlRotLeft
(
    INOUT PRV_CPSS_AVL_NODE_STC** ppNode
);

static void prvCpssAvlRotRight
(
    INOUT PRV_CPSS_AVL_NODE_STC** ppNode
);

static PRV_CPSS_AVL_RES_ENT prvCpssAvlLeftGrown
(
    INOUT PRV_CPSS_AVL_NODE_STC** ppNode
);

static PRV_CPSS_AVL_RES_ENT prvCpssAvlRightGrown
(
    INOUT PRV_CPSS_AVL_NODE_STC** ppNode
);

static PRV_CPSS_AVL_RES_ENT prvCpssAvlRecursiveInsert
(
    IN      GT_VOID*                  pData,
    IN      prototypeCmpFunc          compareFunc,
    IN      CPSS_BM_POOL_ID           poolId,
    INOUT   PRV_CPSS_AVL_NODE_STC**   ppNode
);

static PRV_CPSS_AVL_RES_ENT prvCpssAvlRecursiveRemove
(
    IN     GT_VOID*                   pData,
    IN     prototypeCmpFunc           compareFunc,
    IN     CPSS_BM_POOL_ID            poolId,
    INOUT  PRV_CPSS_AVL_NODE_STC**    ppNode,
    OUT    GT_VOID**                  pRetData
);

static PRV_CPSS_AVL_NODE_STC *prvCpssAvlRecusFindFirst
(
    IN  PRV_CPSS_AVL_NODE_STC*  pHead,
    IN  GT_VOID*                pData,
    IN  prototypeCmpFunc        compareFunc
);

static PRV_CPSS_AVL_NODE_STC *prvCpssAvlRecusFindNext
(
    IN  PRV_CPSS_AVL_NODE_STC*  pHead,
    IN  GT_VOID*                pData,
    IN  prototypeCmpFunc        compareFunc
);

static GT_VOID* prvCpssAvlRecusSearch
(
    IN  PRV_CPSS_AVL_NODE_STC*  pHead,
    IN  GT_VOID*                pData,
    IN  prototypeCmpFunc        compareFunc
);

static void prvCpssAvlRecusDestroyAll
(
    IN PRV_CPSS_AVL_NODE_STC*   pHead,
    IN GT_VOIDFUNCPTR           dataFreeFunc,
    IN GT_VOID*                 cookiePtr,
    IN CPSS_BM_POOL_ID          poolId
);

/************ Local variables ************************************************/

/************ Public Functions ************************************************/

/*******************************************************************************
* prvCpssAvlMemPoolCreate
*
* DESCRIPTION:  Create memory pool to be used by the AVL tree.
*
* INPUTS:
*       maxNode - Max AVL nodes expected to be useed. The function will
*                 allocate memory to support that number of nodes.
*
* OUTPUTS:
           pPoolId - Return the created pool's ID.
*
* RETURNS:
*       GT_STATUS      GT_OK - For successful operation.
*                      Otherwise return the returned status from the pool function.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssAvlMemPoolCreate
(
    IN   GT_U32            maxNode,
    OUT  CPSS_BM_POOL_ID*  pPoolId
)
{
    GT_STATUS status;

    status = cpssBmPoolCreate(
        sizeof(PRV_CPSS_AVL_NODE_STC), CPSS_BM_POOL_4_BYTE_ALIGNMENT_E,
        maxNode, pPoolId);

    return(status);
}


/*******************************************************************************
* prvCpssAvlMemPoolDelete
*
* DESCRIPTION:  Delete memory pool used by AVL tree.
*
* INPUTS:
*       PoolId - Pool ID to delete.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS      GT_OK - For successful operation.
*                      Otherwise return the returned status from the pool function.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssAvlMemPoolDelete
(
    IN  CPSS_BM_POOL_ID   poolId
)
{
    GT_STATUS status;

    status = cpssBmPoolDelete( poolId);

    return(status);
}


/*******************************************************************************
* prvCpssAvlTreeCreate
*
* DESCRIPTION:  Create AVL tree
*
*
* INPUTS:
*       compareFunc - Comparison function used to sort the nodes in the tree.
*                     recieves two node's data and returns:
*                           GT_EQUAL / GT_GREATER / GT_SMALLER
*       poolId      - pool ID of the buffers's pool to be used for memory allocation.
*                     If In case pool ID is CPSS_BM_POOL_NULL_ID, memory will be allocated
*                     by malloc function.
*
* OUTPUTS:
*       pTreeId    - Return the ID (handler) of the new created tree.
*
* RETURNS:
*       GT_STATUS      GT_OK - For successful operation.
*                      GT_NO_RESOURCE - No memory is available to create that tree.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssAvlTreeCreate
(
    IN   GT_INTFUNCPTR                compareFunc,
    IN   CPSS_BM_POOL_ID              poolId,
    OUT  PRV_CPSS_AVL_TREE_ID*        pTreeId
)
{
    PRV_CPSS_AVL_HEAD_STC *treePtr;
    /* Allocate memory for tree header used as tree ID */
    treePtr = (PRV_CPSS_AVL_HEAD_STC*)cpssOsMalloc(sizeof(PRV_CPSS_AVL_HEAD_STC));
    if (NULL == treePtr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);

    treePtr->compareFunc = (prototypeCmpFunc)compareFunc;
    treePtr->poolId = poolId;
    treePtr->pHead = NULL;
    *pTreeId = (PRV_CPSS_AVL_TREE_ID)treePtr;

    return(GT_OK);
}

/*******************************************************************************
* prvCpssAvlTreeDelete
*
* DESCRIPTION:  Delete AVL tree
*
* INPUTS:
*       treeId       - ID of the tree to be deleted.
*       dataFreeFunc - A pointer to a function to be called for deleteing the
*                      user data, or NULL if no free operation should be done
*                      on the user's data. The function gets 2 parameters:
*                      cookie pointer and data ponter from node.
*       cookiePtr    - cookie parameter to be passed to dataFreeFunc function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS      GT_OK - For successful operation.
*                      GT_NOT_INITIALIZED - the given tree has not been initialized.
*                      GT_BAD_STATE - In case the tree is not empty.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssAvlTreeDelete
(
    IN  PRV_CPSS_AVL_TREE_ID      treeId,
    IN  GT_VOIDFUNCPTR            dataFreeFunc,
    IN  GT_VOID*                  cookiePtr
)
{
    PRV_CPSS_AVL_HEAD_STC *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if( NULL == treePtr )
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    /* free all nodes and the user's data */
    prvCpssAvlRecusDestroyAll(
        treePtr->pHead, dataFreeFunc, cookiePtr, treePtr->poolId);

    /* Release the tree ID used as tree header */
    cpssOsFree(treeId);

    return(GT_OK);
}

/*******************************************************************************
* prvCpssAvlTreeReset
*
* DESCRIPTION:  Reset AVL tree - remove all nodes
*
* INPUTS:
*       treeId       - ID of the tree to be reseted.
*       dataFreeFunc - A pointer to a function to be called for deleteing the
*                      user data, or NULL if no free operation should be done
*                      on the user's data. The function gets 2 parameters:
*                      cookie pointer and data ponter from node.
*       cookiePtr    - cookie parameter to be passed to dataFreeFunc function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS      GT_OK - For successful operation.
*                      GT_NOT_INITIALIZED - the given tree has not been initialized.
*                      GT_BAD_STATE - In case the tree is not empty.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssAvlTreeReset
(
    IN  PRV_CPSS_AVL_TREE_ID      treeId,
    IN  GT_VOIDFUNCPTR            dataFreeFunc,
    IN  GT_VOID*                  cookiePtr
)
{
    PRV_CPSS_AVL_HEAD_STC *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if( NULL == treePtr )
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    /* free all nodes and the user's data */
    prvCpssAvlRecusDestroyAll(
        treePtr->pHead, dataFreeFunc, cookiePtr, treePtr->poolId);

    return(GT_OK);
}

/*******************************************************************************
* prvCpssAvlItemInsert
*
* DESCRIPTION:  insert item into the AVL tree.
*
*
* INPUTS:
*       treeId - ID of the tree.
*       pData - user's data pointer to be inserted into the tree.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS      GT_OK - For successful operation.
*                      GT_NOT_INITIALIZED - The given tree has not been initialized.
*                      GT_ALREADY_EXIST - Item already exist in the tree.
*                      GT_NO_RESOURCE - There is no memory to add the new item.
*                      GT_FAIL - General error.
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssAvlItemInsert
(
    IN  PRV_CPSS_AVL_TREE_ID  treeId,
    IN  GT_VOID*              pData
)
{
    PRV_CPSS_AVL_RES_ENT  result;
    GT_STATUS             status;
    PRV_CPSS_AVL_HEAD_STC *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if( NULL == treeId )
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    result = prvCpssAvlRecursiveInsert(
        pData, (prototypeCmpFunc)treePtr->compareFunc,
        treePtr->poolId, &(treePtr->pHead));

    if ( (result == PRV_CPSS_AVL_RES_OK_E ) || (result == PRV_CPSS_AVL_RES_BALANCE_E) )
        status = GT_OK;
    else if (result == PRV_CPSS_AVL_RES_EXIST_E)
        status = GT_ALREADY_EXIST;
    else if (result == PRV_CPSS_AVL_RES_NO_MEM_E)
        status = GT_NO_RESOURCE;
    else
        status = GT_FAIL;

    return(status);
}


/*******************************************************************************
* prvCpssAvlItemRemove
*
* DESCRIPTION:  Remove item from the tree
*
*
* INPUTS:
*       treeId - ID of the tree.
*       pData - user's data pointer to be removed from the tree.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID* - A pointer to the data stored in the deleted node, or
*                  NULL if no such node exists.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID* prvCpssAvlItemRemove
(
    IN  PRV_CPSS_AVL_TREE_ID  treeId,
    IN  GT_VOID*              pData
)
{
    PRV_CPSS_AVL_RES_ENT   result;
    GT_VOID*               pRetData;
    PRV_CPSS_AVL_HEAD_STC  *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if( NULL == treeId )
        return(NULL);

    result = prvCpssAvlRecursiveRemove(
        pData, treePtr->compareFunc,
        treePtr->poolId, &(treePtr->pHead), &pRetData);

    if ( (result != PRV_CPSS_AVL_RES_OK_E ) && (result != PRV_CPSS_AVL_RES_BALANCE_E) )
        pRetData = NULL;
    /* Otherwise, the pRetData returned with valid pointer to the user's data */

    return(pRetData);
}

/*******************************************************************************
* prvCpssAvlSearch
*
* DESCRIPTION:
*       This function searches for a given key in the avl tree.
*
* INPUTS:
*       treeId - ID of the tree.
*       pData   - a pointer to the data to be searched.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_VOID* - A pointer to the data stored in the found node, or
*                  NULL if no such node exists.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID* prvCpssAvlSearch
(
    IN  PRV_CPSS_AVL_TREE_ID  treeId,
    IN  GT_VOID*              pData
)
{
    GT_VOID*              pRetData;
    PRV_CPSS_AVL_HEAD_STC *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if( NULL == treeId )
        return(NULL);

    pRetData = prvCpssAvlRecusSearch(treePtr->pHead, pData, treePtr->compareFunc);

    return(pRetData);
}


/*******************************************************************************
* prvCpssAvlIsEmpty
*
* DESCRIPTION:
*       checks if a the given tree is empty.
*
* INPUTS:
*       treeId - ID of the tree.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if the tree is empty, GT_FALSE otherwise.
*
* COMMENTS:
*       The funtion returns with GT_TRUE also in case of error.
*
*******************************************************************************/
GT_BOOL prvCpssAvlIsEmpty
(
    IN  PRV_CPSS_AVL_TREE_ID  treeId
)
{
    PRV_CPSS_AVL_HEAD_STC *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if (NULL == treePtr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    if (treePtr->pHead == NULL)
        return(GT_TRUE);

    return(GT_FALSE);
}

/*******************************************************************************
* prvCpssAvlFirstDataGet
*
* DESCRIPTION:
*       search for a given key in the AVL tree.
*       This function returns a pointer to the data stored in the first node
*       with key >= KEY(pData), OR NULL if there is no such node.
*
* INPUTS:
*       treeId - ID of the tree.
*       pData - user's data pointer to search for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_VOID* - A pointer to the first node with data >= pData.
*       NULL will be returned if no such node exists or the given tree
*       has not been initialized.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID* prvCpssAvlFirstDataGet
(
    IN  PRV_CPSS_AVL_TREE_ID  treeId,
    IN  GT_VOID*              pData
)
{
    PRV_CPSS_AVL_TREE_ITERATOR_ID      iterator;
    GT_VOID*                           pRetData;
    PRV_CPSS_AVL_HEAD_STC              *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if( (NULL == treePtr) || (NULL == treePtr->pHead) )
        return(NULL);

    pRetData = prvCpssAvlIteratorForNodeGet(treeId, pData, &iterator);

    return(pRetData);
}


/*******************************************************************************
* prvCpssAvlIteratorForNodeGet
*
* DESCRIPTION:
*       Returns the iteratorId for the the first data which is >= pData.
*       this iterator can be used to pass over the tree from a
*       specific location.
*
* INPUTS:
*       treeId - ID of the tree.
*       pData - A pointer to the data to be searched.
*
* OUTPUTS:
*       iteratorId  - Return the iterator ID of the given data.
*
* RETURNS:
*       The data represented by iteratorId if succeeded, or NULL otherwise.
*
* COMMENTS:
*       The returned iterator represents the first data which is >= pData.
*
*******************************************************************************/
GT_VOID* prvCpssAvlIteratorForNodeGet
(
    IN  PRV_CPSS_AVL_TREE_ID                   treeId,
    IN  GT_VOID*                               pData,
    OUT PRV_CPSS_AVL_TREE_ITERATOR_ID*         pIteratorId
)
{
    PRV_CPSS_AVL_NODE_STC*   pNode;
    prototypeCmpFunc         compareFunc;
    GT_VOID*                 pRetData;
    PRV_CPSS_AVL_HEAD_STC    *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;


    if( (NULL == treePtr) || (NULL == treePtr->pHead) )
    {
        *pIteratorId = NULL;
        return(NULL);
    }

    compareFunc = treePtr->compareFunc;

    if (NULL == pData)
    {
       *pIteratorId = NULL;    /* Get the first Item */
       pRetData = prvCpssAvlGetNext(treeId, pIteratorId);
    }
    else
    {
        pNode = prvCpssAvlRecusFindFirst(treePtr->pHead, pData, compareFunc);
        *pIteratorId = (PRV_CPSS_AVL_TREE_ITERATOR_ID)pNode;

        if (pNode != NULL)
            pRetData = pNode->pData;
        else
            pRetData = NULL;
    }

    return(pRetData);
}


/*******************************************************************************
* prvCpssAvlGetNext
*
* DESCRIPTION:
*       This function is used for iterations over the AVL tree. The function return
*       the iterator ID of the next item of the data represented by the given
*       iterator ID.
*
* INPUTS:
*       treeId - ID of the tree.
*       pData - A pointer to the data to be searched.
*       iteratorId - the iterator ID of the data the next item will be searched.
*
* OUTPUTS:
*       iteratorId  - Return the iterator ID of the given data.
*
* RETURNS:
*       A pointer to the next node data if exists, NULL otherwise.
*
* COMMENTS:
*       1.  If (*iteratorId == NULL): the first node in the tree is returned.
*       2.  The new value of iterator Id should be sent in the next call to
*           this function to get the next data.
*
*******************************************************************************/
GT_VOID* prvCpssAvlGetNext
(
    IN    PRV_CPSS_AVL_TREE_ID                 treeId,
    INOUT PRV_CPSS_AVL_TREE_ITERATOR_ID        *pIteratorId
)
{
    PRV_CPSS_AVL_NODE_STC*   pNode;
    PRV_CPSS_AVL_NODE_STC*   pUserNode;
    GT_VOID*                 pRetData;
    PRV_CPSS_AVL_HEAD_STC    *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if( (NULL == treePtr) || (NULL == treePtr->pHead) )
    {
        *pIteratorId = NULL;
        return(NULL);
    }


    pUserNode = (PRV_CPSS_AVL_NODE_STC*)(*pIteratorId);

    if ( pUserNode == NULL )  /* Get the first item (the smaller) */
    {
        pNode = treePtr->pHead;
        while (pNode->left != NULL)
            pNode = pNode->left;
    }
    else if (pUserNode->right != NULL)
    /* If the node has right branch, the next item is the smallest item on that branch.
     * Otherwise, it's one of the previos parents of that node.
     */
    {
        pNode = pUserNode->right;
        while (pNode->left != NULL)
            pNode = pNode->left;
    }
    else  /* The next item is one of the previous father. */
        pNode = prvCpssAvlRecusFindNext(treePtr->pHead, pUserNode->pData, treePtr->compareFunc);


    *pIteratorId = (PRV_CPSS_AVL_TREE_ITERATOR_ID)(pNode);
    if (pNode != NULL)
        pRetData = pNode->pData;
    else
        pRetData = NULL;

    return(pRetData);
}


/************ Private Functions ************************************************/

/*******************************************************************************
* prvCpssAvlRecursiveInsert
*
* DESCRIPTION:
*       Recursively insert new node into the AVL tree.
*
* INPUTS:
*       pData - data to be inserted.
*       compareFunc - Comparison function used to sort the nodes in the tree.
*                     recieves two node's data and returns:
*                           GT_EQUAL / GT_GREATER / GT_SMALLER
*       poolId      - pool ID of the buffers's pool to be used for memory allocation.
*                     If In case pool ID is CPSS_BM_POOL_NULL_ID, memory will be allocated
*                     by malloc function.
*       ppNode - Address of a pointer to a node represent a tree
*
* OUTPUTS:
*        ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*        PRV_CPSS_AVL_RES_OK_E - None of the subtrees of the node that ppNode points to
*                 has grown, the AVL tree is valid.
*        PRV_CPSS_AVL_RES_BALANCE_E - One of the subtrees of the node that ppNode points to
*                      has grown, the node's "skew" flag needs adjustment,
*                      and the AVL tree may have become unbalanced.
*        PRV_CPSS_AVL_RES_NO_MEM_E - There is not enought memory to add the new Data.
*        PRV_CPSS_AVL_RES_EXIST_E - Data already exist in the tree.
*
* COMMENTS:
*
*******************************************************************************/
static PRV_CPSS_AVL_RES_ENT prvCpssAvlRecursiveInsert
(
    IN      GT_VOID*         pData,
    IN      prototypeCmpFunc compareFunc,
    IN      CPSS_BM_POOL_ID       poolId,
    INOUT   PRV_CPSS_AVL_NODE_STC**    ppNode
)
{
    PRV_CPSS_AVL_RES_ENT           insertResult;
    GT_COMP_RES  cprResult;

    if ( *ppNode == NULL )
    {
        if (poolId == CPSS_BM_POOL_NULL_ID)
            *ppNode = cpssOsMalloc(sizeof(PRV_CPSS_AVL_NODE_STC));
        else
            *ppNode = cpssBmPoolBufGet(poolId);

        if ( *ppNode == NULL)
            return (PRV_CPSS_AVL_RES_NO_MEM_E);

        (*ppNode)->left = (*ppNode)->right = NULL;
        (*ppNode)->pData = pData;
        (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;

        return (PRV_CPSS_AVL_RES_BALANCE_E);
    }

    cprResult = compareFunc(pData, (*ppNode)->pData);
    if (cprResult == GT_SMALLER)  /* pData < (*ppNode)->pData */
    {
        insertResult = prvCpssAvlRecursiveInsert(pData, compareFunc,
                                          poolId, &((*ppNode)->left));

        if ( insertResult == PRV_CPSS_AVL_RES_BALANCE_E )
        {
            return prvCpssAvlLeftGrown(ppNode);
        }
        else
            return insertResult;
    }

    if ( cprResult == GT_GREATER)  /* pData > (*ppNode)->pData */
    {
        insertResult = prvCpssAvlRecursiveInsert(pData, compareFunc,
                                          poolId, &((*ppNode)->right));
        if ( insertResult == PRV_CPSS_AVL_RES_BALANCE_E )
        {
            return prvCpssAvlRightGrown(ppNode);
        }
        else
            return insertResult;
    }

    return PRV_CPSS_AVL_RES_EXIST_E;
}


/*******************************************************************************
* prvCpssAvlRecursiveRemove
*
* DESCRIPTION:
*       Recursively remove an item from the tree
*
* INPUTS:
*       pData - data to be removed.
*       compareFunc - Comparison function used to sort the nodes in the tree.
*                     recieves two node's data and returns:
*                           GT_EQUAL / GT_GREATER / GT_SMALLER
*       poolId - pool ID of the buffers's pool to be used to free node's memory.
*       ppNode - Address of a pointer to a node represent a tree
*
* OUTPUTS:
*        ppNode - Address of a pointer to a new node represent a balanced tree.
*        pData -  A pointer to the data stored in the deleted node. The value
*                 at this pointer is valid only if the function return with no
*                 error.
*
* RETURNS:
*        PRV_CPSS_AVL_RES_OK_E - None of the subtrees of the node that ppNode points to
*                 has grown, the AVL tree is valid.
*        PRV_CPSS_AVL_RES_BALANCE_E - One of the subtrees of the node that ppNode points to
*                           has shrunk, the node's "skew" flag needs adjustment,
*                           and the AVL tree may have become unbalanced.
*        PRV_CPSS_AVL_RES_ERROR_E - The requested data is not exist in the tree.
*
* COMMENTS:
*
*******************************************************************************/
static PRV_CPSS_AVL_RES_ENT prvCpssAvlRecursiveRemove
(
    IN      GT_VOID*       pData,
    IN      prototypeCmpFunc  compareFunc,
    IN      CPSS_BM_POOL_ID     poolId,
    INOUT   PRV_CPSS_AVL_NODE_STC**  ppNode,
    OUT     GT_VOID**      pRetData
)
{
    GT_COMP_RES  cprResult;
    PRV_CPSS_AVL_RES_ENT retVal = PRV_CPSS_AVL_RES_BALANCE_E;


    if ( *ppNode == NULL )
        return PRV_CPSS_AVL_RES_ERROR_E;


    cprResult = compareFunc(pData, (*ppNode)->pData);
    switch (cprResult)
    {
        case GT_SMALLER:  /* pData < (*ppNode)->pData */
        {
            retVal = prvCpssAvlRecursiveRemove(pData, compareFunc,
                                        poolId, &((*ppNode)->left), pRetData);
            if ( retVal == PRV_CPSS_AVL_RES_BALANCE_E)
            {
                retVal = prvCpssAvlLeftShrunk(ppNode);
            }
        }
        break;

        case GT_GREATER:  /* pData > (*ppNode)->pData */
        {
            retVal = prvCpssAvlRecursiveRemove(pData, compareFunc,
                                        poolId, &((*ppNode)->right), pRetData);
            if ( retVal == PRV_CPSS_AVL_RES_BALANCE_E)
            {
                retVal = prvCpssAvlRightShrunk(ppNode);
            }
        }
        break;

        case GT_EQUAL:  /* pData == (*ppNode)->pData */
        {
            *pRetData = (*ppNode)->pData;

            if ( (*ppNode)->left != NULL)
            {
                if (prvCpssAvlFindHighest(*ppNode, &retVal, poolId, &((*ppNode)->left) )  )
                {
                    if (retVal == PRV_CPSS_AVL_RES_BALANCE_E)
                    {
                        retVal = prvCpssAvlLeftShrunk(ppNode);
                    }

                    return(retVal);
                }
            }/* left */

            if ( (*ppNode)->right != NULL )
            {
                if ( prvCpssAvlFindLowest(*ppNode, &retVal, poolId, &((*ppNode)->right) ) )
                {
                    if (retVal == PRV_CPSS_AVL_RES_BALANCE_E)
                    {
                        retVal = prvCpssAvlRightShrunk(ppNode);
                    }

                    return(retVal);
                }
            }/* right */

            if (poolId == CPSS_BM_POOL_NULL_ID)
                cpssOsFree(*ppNode);
            else
                cpssBmPoolBufFree(poolId, *ppNode);

            *ppNode = NULL;

            retVal = PRV_CPSS_AVL_RES_BALANCE_E;

        }/*GT_EQUAL*/
        break;

        default:
            retVal = PRV_CPSS_AVL_RES_ERROR_E;

    }/*switch*/

    return (retVal);
}


/*******************************************************************************
* prvCpssAvlRecursiveRemove
*
* DESCRIPTION:
*       perform counterclockwise rotation
*
* INPUTS:
*       ppNode - Address of a pointer to a node represent a tree
*
* OUTPUTS:
*       ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void prvCpssAvlRotLeft
(
    INOUT   PRV_CPSS_AVL_NODE_STC**   ppNode
)
{
    PRV_CPSS_AVL_NODE_STC *pTmp = *ppNode;

    *ppNode = (*ppNode)->right;
    pTmp->right = (*ppNode)->left;
    (*ppNode)->left = pTmp;
}

/*******************************************************************************
* prvCpssAvlRotRight
*
* DESCRIPTION:
*       perform clockwise rotation
*
* INPUTS:
*       ppNode - Address of a pointer to a node represent a tree
*
* OUTPUTS:
*       ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static void prvCpssAvlRotRight
(
    INOUT   PRV_CPSS_AVL_NODE_STC**   ppNode
)
{
    PRV_CPSS_AVL_NODE_STC *pTmp = *ppNode;

    *ppNode = (*ppNode)->left;
    pTmp->left = (*ppNode)->right;
    (*ppNode)->right = pTmp;
}


/*******************************************************************************
* prvCpssAvlLeftGrown
*
* DESCRIPTION:
*       helper function for avlinsert
*
* INPUTS:
*       ppNode - Address of a pointer to a node. This node's left
*                subtree has just grown due to item insertion; its
*                "skew" flag needs adjustment, and the local tree
*                (the subtree of which this node is the root node) may
*                have become unbalanced.
*
* OUTPUTS:
*       ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*       PRV_CPSS_AVL_RES_OK_E - The local tree could be rebalanced or was balanced
*                from the start. The parent activations of the avlinsert
*                activation that called this function may assume the
*                entire tree is valid.
*
*       PRV_CPSS_AVL_RES_BALANCE_E - The local tree was balanced, but has grown in height.
*                     Do not assume the entire tree is valid.
*
* COMMENTS:
*
*******************************************************************************/
static PRV_CPSS_AVL_RES_ENT prvCpssAvlLeftGrown
(
    INOUT   PRV_CPSS_AVL_NODE_STC**   ppNode
)
{
    switch ((*ppNode)->skew)
    {
        case PRV_CPSS_AVLSKEW_LEFT_E:
        {
            if ((*ppNode)->left->skew == PRV_CPSS_AVLSKEW_LEFT_E)
            {
                (*ppNode)->skew = (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_NONE_E;
                prvCpssAvlRotRight(ppNode);
            }
            else
            {
                switch ((*ppNode)->left->right->skew)
                {
                    case PRV_CPSS_AVLSKEW_LEFT_E:
                        (*ppNode)->skew = PRV_CPSS_AVLSKEW_RIGHT_E;
                        (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_NONE_E;
                        break;

                    case PRV_CPSS_AVLSKEW_RIGHT_E:
                        (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
                        (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_LEFT_E;
                        break;

                    default:
                        (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
                        (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_NONE_E;
                }
                (*ppNode)->left->right->skew = PRV_CPSS_AVLSKEW_NONE_E;
                prvCpssAvlRotLeft(& (*ppNode)->left);
                prvCpssAvlRotRight(ppNode);
            }

            return PRV_CPSS_AVL_RES_OK_E;
        }

        case PRV_CPSS_AVLSKEW_RIGHT_E:
        {
            (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
            return PRV_CPSS_AVL_RES_OK_E;
        }

        default:
        {
            (*ppNode)->skew = PRV_CPSS_AVLSKEW_LEFT_E;
            return PRV_CPSS_AVL_RES_BALANCE_E;
        }
    }
}

/*******************************************************************************
* prvCpssAvlRightGrown
*
* DESCRIPTION:
*       helper function for avlinsert. See prvCpssAvlLeftGrown for details.
*
* INPUTS:
*       ppNode - Address of a pointer to a node.
*
* OUTPUTS:
*       ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*       PRV_CPSS_AVL_RES_OK_E - The local tree could be rebalanced or was balanced
*                from the start. The parent activations of the avlinsert
*                activation that called this function may assume the
*                entire tree is valid.
*
*       PRV_CPSS_AVL_RES_BALANCE_E - The local tree was balanced, but has grown in height.
*                     Do not assume the entire tree is valid.
*
* COMMENTS:
*
*******************************************************************************/
static PRV_CPSS_AVL_RES_ENT prvCpssAvlRightGrown
(
    INOUT   PRV_CPSS_AVL_NODE_STC**   ppNode
)
{
    switch ((*ppNode)->skew) {
    case PRV_CPSS_AVLSKEW_LEFT_E:
        (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
        return PRV_CPSS_AVL_RES_OK_E;

    case PRV_CPSS_AVLSKEW_RIGHT_E:
        if ((*ppNode)->right->skew == PRV_CPSS_AVLSKEW_RIGHT_E) {
            (*ppNode)->skew = (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_NONE_E;
            prvCpssAvlRotLeft(ppNode);
        }
        else {
            switch ((*ppNode)->right->left->skew) {
            case PRV_CPSS_AVLSKEW_RIGHT_E:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_LEFT_E;
                (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_NONE_E;
                break;

            case PRV_CPSS_AVLSKEW_LEFT_E:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
                (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_RIGHT_E;
                break;

            default:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
                (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_NONE_E;
            }
            (*ppNode)->right->left->skew = PRV_CPSS_AVLSKEW_NONE_E;
            prvCpssAvlRotRight(& (*ppNode)->right);
            prvCpssAvlRotLeft(ppNode);
        }
        return PRV_CPSS_AVL_RES_OK_E;

    default:
        (*ppNode)->skew = PRV_CPSS_AVLSKEW_RIGHT_E;
        return PRV_CPSS_AVL_RES_BALANCE_E;
    }
}


/*******************************************************************************
* prvCpssAvlLeftShrunk
*
* DESCRIPTION:
*       helper function for avlremove and prvCpssAvlFindLowest.
*
* INPUTS:
*       ppNode - Address of a pointer to a node. The node's left
*                subtree has just shrunk due to item removal; its
*                "skew" flag needs adjustment, and the local tree
*                (the subtree of which this node is the root node) may
*                have become unbalanced.
*
* OUTPUTS:
*       ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*       PRV_CPSS_AVL_RES_OK_E - The parent activation of the avlremove activation
*                that called this function may assume the entire
*                tree is valid.
*
*       PRV_CPSS_AVL_RES_BALANCE_E - Do not assume the entire tree is valid.
*
* COMMENTS:
*
*******************************************************************************/
static PRV_CPSS_AVL_RES_ENT prvCpssAvlLeftShrunk
(
    INOUT PRV_CPSS_AVL_NODE_STC** ppNode
)
{
    switch ((*ppNode)->skew) {
    case PRV_CPSS_AVLSKEW_LEFT_E:
        (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
        return PRV_CPSS_AVL_RES_BALANCE_E;

    case PRV_CPSS_AVLSKEW_RIGHT_E:
        if ((*ppNode)->right->skew == PRV_CPSS_AVLSKEW_RIGHT_E) {
            (*ppNode)->skew = (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_NONE_E;
            prvCpssAvlRotLeft(ppNode);
            return PRV_CPSS_AVL_RES_BALANCE_E;
        }
        else if ((*ppNode)->right->skew == PRV_CPSS_AVLSKEW_NONE_E) {
            (*ppNode)->skew = PRV_CPSS_AVLSKEW_RIGHT_E;
            (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_LEFT_E;
            prvCpssAvlRotLeft(ppNode);
            return PRV_CPSS_AVL_RES_OK_E;
        }
        else {
            switch ((*ppNode)->right->left->skew) {
            case PRV_CPSS_AVLSKEW_LEFT_E:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
                (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_RIGHT_E;
                break;

            case PRV_CPSS_AVLSKEW_RIGHT_E:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_LEFT_E;
                (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_NONE_E;
                break;

            default:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
                (*ppNode)->right->skew = PRV_CPSS_AVLSKEW_NONE_E;
            }
            (*ppNode)->right->left->skew = PRV_CPSS_AVLSKEW_NONE_E;
            prvCpssAvlRotRight(& (*ppNode)->right);
            prvCpssAvlRotLeft(ppNode);
            return PRV_CPSS_AVL_RES_BALANCE_E;
        }

    default:
        (*ppNode)->skew = PRV_CPSS_AVLSKEW_RIGHT_E;
        return PRV_CPSS_AVL_RES_OK_E;
    }
}


/*******************************************************************************
* prvCpssAvlRightShrunk
*
* DESCRIPTION:
*       helper function for avlremove and prvCpssAvlFindHighest. See prvCpssAvlLeftShrunk for details.
*
* INPUTS:
*       ppNode - Address of a pointer to a node. The node's left
*                subtree has just shrunk due to item removal; its
*                "skew" flag needs adjustment, and the local tree
*                (the subtree of which this node is the root node) may
*                have become unbalanced.
*
* OUTPUTS:
*       ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*       PRV_CPSS_AVL_RES_OK_E - The parent activation of the avlremove activation
*                that called this function may assume the entire
*                tree is valid.
*
*       PRV_CPSS_AVL_RES_BALANCE_E - Do not assume the entire tree is valid.
*
* COMMENTS:
*
*******************************************************************************/
static PRV_CPSS_AVL_RES_ENT prvCpssAvlRightShrunk
(
    INOUT   PRV_CPSS_AVL_NODE_STC**   ppNode
)
{
    switch ((*ppNode)->skew) {
    case PRV_CPSS_AVLSKEW_RIGHT_E:
        (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
        return PRV_CPSS_AVL_RES_BALANCE_E;

    case PRV_CPSS_AVLSKEW_LEFT_E:
        if ((*ppNode)->left->skew == PRV_CPSS_AVLSKEW_LEFT_E) {
            (*ppNode)->skew = (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_NONE_E;
            prvCpssAvlRotRight(ppNode);
            return PRV_CPSS_AVL_RES_BALANCE_E;
        }
        else if ((*ppNode)->left->skew == PRV_CPSS_AVLSKEW_NONE_E) {
            (*ppNode)->skew = PRV_CPSS_AVLSKEW_LEFT_E;
            (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_RIGHT_E;
            prvCpssAvlRotRight(ppNode);
            return PRV_CPSS_AVL_RES_OK_E;
        }
        else {
            switch ((*ppNode)->left->right->skew) {
            case PRV_CPSS_AVLSKEW_LEFT_E:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_RIGHT_E;
                (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_NONE_E;
                break;

            case PRV_CPSS_AVLSKEW_RIGHT_E:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
                (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_LEFT_E;
                break;

            default:
                (*ppNode)->skew = PRV_CPSS_AVLSKEW_NONE_E;
                (*ppNode)->left->skew = PRV_CPSS_AVLSKEW_NONE_E;
            }
            (*ppNode)->left->right->skew = PRV_CPSS_AVLSKEW_NONE_E;
            prvCpssAvlRotLeft(& (*ppNode)->left);
            prvCpssAvlRotRight(ppNode);
            return PRV_CPSS_AVL_RES_BALANCE_E;
        }

    default:
        (*ppNode)->skew = PRV_CPSS_AVLSKEW_LEFT_E;
        return PRV_CPSS_AVL_RES_OK_E;
    }
}


/*******************************************************************************
* prvCpssAvlFindHighest
*
* DESCRIPTION:
*       Replace a node with a subtree's highest-ranking item.
*
* INPUTS:
*       target - Pointer to node to be replaced.
*       ppNode - Address of pointer to subtree.
*       res    - Pointer to variable used to tell the caller whether
*                further checks are necessary; analog to the return
*                values of prvCpssAvlLeftGrown and prvCpssAvlLeftShrunk (see there).
* OUTPUTS:
*       ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*       1        A node was found; the target node has been replaced.
*
*       0        The target node could not be replaced because
*                the subtree provided was empty.
*
* COMMENTS:
*
*******************************************************************************/
static int prvCpssAvlFindHighest
(
    IN     PRV_CPSS_AVL_NODE_STC*   target,
    IN     PRV_CPSS_AVL_RES_ENT*        res,
    IN     CPSS_BM_POOL_ID     poolId,
    INOUT  PRV_CPSS_AVL_NODE_STC**  ppNode
)
{
    PRV_CPSS_AVL_NODE_STC *tmp;

    *res = PRV_CPSS_AVL_RES_BALANCE_E;
    if (!(*ppNode)) {
        return 0;
    }
    if ((*ppNode)->right) {
        if (!prvCpssAvlFindHighest(target, res, poolId, &((*ppNode)->right) ) ) {
            return 0;
        }
        if (*res == PRV_CPSS_AVL_RES_BALANCE_E) {
            *res = prvCpssAvlRightShrunk(ppNode);
        }
        return 1;
    }
    target->pData  = (*ppNode)->pData;
    tmp = *ppNode;
    *ppNode = (*ppNode)->left;


    if (poolId == CPSS_BM_POOL_NULL_ID)
        cpssOsFree(tmp);
    else
        cpssBmPoolBufFree(poolId, tmp);

    return 1;
}


/*******************************************************************************
* prvCpssAvlFindHighest
*
* DESCRIPTION:
*       replace node with a subtree's lowest-ranking item.
*       See prvCpssAvlFindHighest for the details.
*
* INPUTS:
*       target - Pointer to node to be replaced.
*       ppNode - Address of pointer to subtree.
*       res    - Pointer to variable used to tell the caller whether
*                further checks are necessary; analog to the return
*                values of prvCpssAvlLeftGrown and prvCpssAvlLeftShrunk (see there).
* OUTPUTS:
*       ppNode - Address of a pointer to a new node represent a balanced tree.
*
* RETURNS:
*       1        A node was found; the target node has been replaced.
*
*       0        The target node could not be replaced because
*                the subtree provided was empty.
*
* COMMENTS:
*
*******************************************************************************/
static int prvCpssAvlFindLowest
(
    IN     PRV_CPSS_AVL_NODE_STC*   target,
    IN     PRV_CPSS_AVL_RES_ENT*        res,
    IN     CPSS_BM_POOL_ID     poolId,
    INOUT  PRV_CPSS_AVL_NODE_STC**  ppNode
)
{
    PRV_CPSS_AVL_NODE_STC *tmp;

    *res = PRV_CPSS_AVL_RES_BALANCE_E;
    if (!(*ppNode)) {
        return 0;
    }
    if ((*ppNode)->left) {
        if (!prvCpssAvlFindLowest(target, res, poolId, &((*ppNode)->left) )) {
            return 0;
        }
        if (*res == PRV_CPSS_AVL_RES_BALANCE_E) {
            *res =  prvCpssAvlLeftShrunk(ppNode);
        }
        return 1;
    }
    target->pData = (*ppNode)->pData;
    tmp = *ppNode;
    *ppNode = (*ppNode)->right;


    if (poolId == CPSS_BM_POOL_NULL_ID)
        cpssOsFree(tmp);
    else
        cpssBmPoolBufFree(poolId, tmp);

    return 1;
}


/*******************************************************************************
* prvCpssAvlRecusFindFirst
*
* DESCRIPTION:
*       Recusivley find the first item in the tree that equel or gretter than pData
*
* INPUTS:
*       pHead - Head of tree to serch in.
*       pData - data to search.
*       compareFunc - Comparison function used to sort the nodes in the tree.
*                     recieves two node's data and returns:
*                           GT_EQUAL / GT_GREATER / GT_SMALLER
* OUTPUTS:
*       None.
*
* RETURNS:
*       PRV_CPSS_AVL_NODE_STC*  - the first node in the tree that equel or gretter than pData
*
* COMMENTS:
*
*******************************************************************************/
static PRV_CPSS_AVL_NODE_STC *prvCpssAvlRecusFindFirst
(
    IN  PRV_CPSS_AVL_NODE_STC*    pHead,
    IN  GT_VOID*        pData,
    IN  prototypeCmpFunc compareFunc
)
{
    PRV_CPSS_AVL_NODE_STC*   pNextItem;
    GT_COMP_RES   cprResult;

    if (pHead == NULL)
        return (NULL); /* Not found, Next item will be returned */

    cprResult = compareFunc(pHead->pData, pData);
    if (cprResult == GT_GREATER)  /* pHead->pData > pData */
    {
        pNextItem = prvCpssAvlRecusFindFirst(pHead->left, pData, compareFunc);
    }
    else if ( cprResult == GT_SMALLER)  /* pHead->pData < pData */
    {
        pNextItem = prvCpssAvlRecusFindFirst(pHead->right, pData, compareFunc);
    }
    else /* equal: pHead->pData == pData */
    {
        return(pHead); /* item has been found, end of search */
    }

    if (pNextItem == NULL) /* item hasn't found, try to find the next item */
    {
        /* Check if the current is applicable */
        cprResult = compareFunc(pHead->pData, pData);
        if (cprResult == GT_GREATER)
            pNextItem = pHead;
    }

    return(pNextItem);
}

/*******************************************************************************
* prvCpssAvlRecusFindNext
*
* DESCRIPTION:
*       Recusivley find the next item in the tree that is gretter than pData
*
* INPUTS:
*       pHead - Head of tree to serch in.
*       pData - data to search.
*       compareFunc - Comparison function used to sort the nodes in the tree.
*                     recieves two node's data and returns:
*                           GT_EQUAL / GT_GREATER / GT_SMALLER
* OUTPUTS:
*       None.
*
* RETURNS:
*       PRV_CPSS_AVL_NODE_STC*  - the first node in the tree that equel or gretter than pData
*
* COMMENTS:
*
*******************************************************************************/
static PRV_CPSS_AVL_NODE_STC *prvCpssAvlRecusFindNext
(
    IN  PRV_CPSS_AVL_NODE_STC*    pHead,
    IN  GT_VOID*        pData,
    IN  prototypeCmpFunc compareFunc
)
{
    PRV_CPSS_AVL_NODE_STC*   pNextItem;
    GT_COMP_RES    cprResult;

    if (pHead == NULL)
        return (NULL); /* Not found, End of search */

    cprResult = compareFunc(pHead->pData, pData);
    if (cprResult == GT_GREATER)  /* pData > (*ppNode)->pData */
    {
        pNextItem = prvCpssAvlRecusFindNext(pHead->left, pData, compareFunc);
    }
    else if ( cprResult == GT_SMALLER)  /* pData < (*ppNode)->pData */
    {
        pNextItem = prvCpssAvlRecusFindNext(pHead->right, pData, compareFunc);
    }
    else /* equal */
    {
        return(NULL); /* End of search */
    }

    if (pNextItem == NULL) /* Next item hasn't found yet */
    {
        /* Check if the current is applicable */
        cprResult = compareFunc(pHead->pData, pData);
        if (cprResult == GT_GREATER)
            pNextItem = pHead;
    }

    return(pNextItem);
}

/*******************************************************************************
* prvCpssAvlRecusSearch
*
* DESCRIPTION:
*       Recusivley search for an item in the tree
*
* INPUTS:
*       pHead - Head of tree to serch in.
*       pData - data to search.
*       compareFunc - Comparison function used to sort the nodes in the tree.
*                     recieves two node's data and returns:
*                           GT_EQUAL / GT_GREATER / GT_SMALLER
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_VOID*  - Return pointer to the data itself if it exist, otherwise
*                   return NULL.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID*  prvCpssAvlRecusSearch
(
    IN  PRV_CPSS_AVL_NODE_STC*    pHead,
    IN  GT_VOID*        pData,
    IN  prototypeCmpFunc compareFunc
)
{
    GT_VOID*      pRetData;
    GT_COMP_RES   cprResult;


    if (pHead == NULL)
        return (NULL); /* Not found, End of search */

    cprResult = compareFunc(pHead->pData, pData);
    if (cprResult == GT_GREATER)  /* pData > (*ppNode)->pData */
    {
        pRetData = prvCpssAvlRecusSearch(pHead->left, pData, compareFunc);
    }
    else if ( cprResult == GT_SMALLER)  /* pData < (*ppNode)->pData */
    {
        pRetData = prvCpssAvlRecusSearch(pHead->right, pData, compareFunc);
    }
    else /* equal */
    {
        pRetData = pHead->pData; /* End of search */
    }

    return(pRetData);
}


/*******************************************************************************
* prvCpssAvlRecusDestroyAll
*
* DESCRIPTION:
*       Recusivley destroy all the user data and the tree's nodes
*
* INPUTS:
*       pHead        - Head of tree to destroy.
*       dataFreeFunc - A pointer to a function to be called for deleteing the
*                      user data, or NULL if no free operation should be done
*                      on the user's data. The function gets 2 parameters:
*                      cookie pointer and data ponter from node.
*       cookiePtr    - cookie parameter to be passed to dataFreeFunc function.
*       PoolId       - Buffer's pool to use for nodes free.
*                      If that field is CPSS_BM_POOL_NULL_ID, the cpssOsFree is used.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_STATUS      GT_OK - For successful operation.
*
* COMMENTS:
*
*******************************************************************************/
static void prvCpssAvlRecusDestroyAll
(
    IN PRV_CPSS_AVL_NODE_STC*   pHead,
    IN GT_VOIDFUNCPTR           dataFreeFunc,
    IN GT_VOID*                 cookiePtr,
    IN CPSS_BM_POOL_ID          poolId
)
{
    if (pHead == NULL)
        return; /* End */


    /* Destroy the left tree */
    prvCpssAvlRecusDestroyAll(pHead->left,dataFreeFunc, cookiePtr, poolId);

    /* Destroy the right tree */
    prvCpssAvlRecusDestroyAll(pHead->right, dataFreeFunc, cookiePtr, poolId);

    /* Delete the user's data */
    if ( (pHead->pData != NULL) && (dataFreeFunc != NULL) )
        ((prototypeDataFreeFunc)dataFreeFunc)(cookiePtr, pHead->pData);

    /* Delete the head of the tree */
    if (poolId == CPSS_BM_POOL_NULL_ID)
        cpssOsFree(pHead);
    else
        cpssBmPoolBufFree(poolId, pHead);
}

/******************************************************************************/
/*                       Fast iteration functions                             */
/******************************************************************************/

#define AVL_TREE_MAX_DEEP_PATH_CNS 57
/* get/set full path deepness into 6 LSBs */

#define PRV_AVL_TREE_PATH_BYTE_MAC(_path, _deep) _path[(_deep + 6) / 8]
#define PRV_AVL_TREE_PATH_BIT_MAC(_deep) (1 << ((_deep + 6) % 8))

#define AVL_TREE_PATH_DEEP_GET_MAC(_path) (_path[0] & 0x3F)
#define AVL_TREE_PATH_DEEP_SET_MAC(_path, _deep) \
    _path[0] = (GT_U8)((_path[0] & (~0x3F)) | (_deep & 0x3F))
/* get/set way (left == 0, rigt = 1) at deepness = _deep in the (_deep + 6)th bit */
#define AVL_TREE_PATH_WAY_GET_MAC(_path, _deep) \
    ((PRV_AVL_TREE_PATH_BYTE_MAC(_path, _deep) & PRV_AVL_TREE_PATH_BIT_MAC(_deep)) ? 1 : 0)
#define AVL_TREE_PATH_WAY_SET_0_MAC(_path, _deep) \
    PRV_AVL_TREE_PATH_BYTE_MAC(_path, _deep) &= (GT_U8)(~ PRV_AVL_TREE_PATH_BIT_MAC(_deep))
#define AVL_TREE_PATH_WAY_SET_1_MAC(_path, _deep) \
    PRV_AVL_TREE_PATH_BYTE_MAC(_path, _deep) |= (GT_U8)PRV_AVL_TREE_PATH_BIT_MAC(_deep)

/*******************************************************************************
* prvCpssAvlPathFind
*
* DESCRIPTION:
*       Find node and return path to it.
*
* INPUTS:
*       treeId   - ID of the tree.
*       findType - type of found (see enum).
*       pData    - A pointer to the data to be searched.
*
* OUTPUTS:
*       path          - The path to found node. It is array (i.e. passed as pointer)
*       pFoundDataPtr - (pointer to) found value.
*
* RETURNS:
*       GT_TRUE - found, GT_FALSE - not found.
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvCpssAvlPathFind
(
    IN  PRV_CPSS_AVL_TREE_ID       treeId,
    IN  PRV_CPSS_AVL_TREE_FIND_ENT findType,
    IN  GT_VOID*                   pData,
    OUT PRV_CPSS_AVL_TREE_PATH     path,
    OUT GT_VOID**                  pFoundDataPtr
)
{
    GT_U32                    depth;
    GT_U32                    lastDepthOfSmaller;
    GT_U32                    lastDepthOfGreater;
    GT_VOID*                  lastDataOfSmaller;
    GT_VOID*                  lastDataOfGreater;
    PRV_CPSS_AVL_NODE_STC*    pNode;
    GT_COMP_RES               cprResult;
    PRV_CPSS_AVL_HEAD_STC     *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    cpssOsMemSet(path, 0, sizeof(PRV_CPSS_AVL_TREE_PATH));

    if( NULL == treePtr )
        return GT_FALSE;

    lastDepthOfSmaller = 0xFFFFFFFF;
    lastDepthOfGreater = 0xFFFFFFFF;
    lastDataOfGreater  = NULL;
    lastDataOfSmaller  = NULL;
    for (depth = 0, pNode = treePtr->pHead;
          ((pNode != NULL) && (depth <= AVL_TREE_MAX_DEEP_PATH_CNS));
          depth++)
    {
        cprResult =  treePtr->compareFunc(pNode->pData, pData);
        switch (cprResult)
        {
            case GT_EQUAL:
                AVL_TREE_PATH_DEEP_SET_MAC(path, depth);
                *pFoundDataPtr  = pNode->pData;
                return GT_TRUE;
            case GT_GREATER:
                AVL_TREE_PATH_WAY_SET_0_MAC(path, depth);
                lastDepthOfGreater = depth;
                lastDataOfGreater  = pNode->pData;
                pNode = pNode->left;
                break;
            case GT_SMALLER:
                AVL_TREE_PATH_WAY_SET_1_MAC(path, depth);
                lastDepthOfSmaller = depth;
                lastDataOfSmaller  = pNode->pData;
                pNode = pNode->right;
                break;
        }
    }

    switch (findType)
    {
        case PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E:
            if (lastDepthOfSmaller == 0xFFFFFFFF)
            {
                return GT_FALSE;
            }
            AVL_TREE_PATH_DEEP_SET_MAC(path, lastDepthOfSmaller);
            *pFoundDataPtr = lastDataOfSmaller;
            break;
        case PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E:
            if (lastDepthOfGreater == 0xFFFFFFFF)
            {
                return GT_FALSE;
            }
            AVL_TREE_PATH_DEEP_SET_MAC(path, lastDepthOfGreater);
            *pFoundDataPtr = lastDataOfGreater;
            break;
        default : return GT_FALSE;
    }

    return GT_TRUE;
}

/*******************************************************************************
* prvCpssAvlPathSeek
*
* DESCRIPTION:
*       Seek node relative from given node.
*
* INPUTS:
*       treeId   - ID of the tree.
*       seekType - type of seek (see enum).
*       path     - The start path node. It is array (i.e. passed as pointer)
*
* OUTPUTS:
*       path          - The path to seeked node. It is array (i.e. passed as pointer)
*       pSeekedDataPtr - (pointer to) seeked value.
*
* RETURNS:
*       GT_TRUE - seeked, GT_FALSE - not seeked.
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvCpssAvlPathSeek
(
    IN    PRV_CPSS_AVL_TREE_ID       treeId,
    IN    PRV_CPSS_AVL_TREE_SEEK_ENT seekType,
    INOUT PRV_CPSS_AVL_TREE_PATH     path,
    OUT   GT_VOID**                  pSeekedDataPtr
)
{
    GT_U32                    depth;
    GT_U32                    pathDepth;
    GT_U32                    way;
    GT_U32                    lastDepthOfLeft;
    GT_U32                    lastDepthOfRight;
    PRV_CPSS_AVL_NODE_STC*    pNode;
    PRV_CPSS_AVL_NODE_STC*    pLastLeftNode;
    PRV_CPSS_AVL_NODE_STC*    pLastRightNode;
    GT_BOOL                   lastLeftIsUpper;
    GT_BOOL                   lastRightIsUpper;
    PRV_CPSS_AVL_HEAD_STC     *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if( NULL == treePtr )
        return GT_FALSE;

    if (seekType == PRV_CPSS_AVL_TREE_SEEK_FIRST_E)
    {
        for (depth = 0, pNode = treePtr->pHead;
              ((pNode != NULL) && (depth <= AVL_TREE_MAX_DEEP_PATH_CNS));
              depth++, pNode = pNode->left)
        {
            AVL_TREE_PATH_WAY_SET_0_MAC(path, depth);
            *pSeekedDataPtr = pNode->pData;
        }
        if (depth > 0)
        {
            AVL_TREE_PATH_DEEP_SET_MAC(path, (depth - 1));
            return GT_TRUE;
        }
        return GT_FALSE;
    }

    if (seekType == PRV_CPSS_AVL_TREE_SEEK_LAST_E)
    {
        for (depth = 0, pNode = treePtr->pHead;
              ((pNode != NULL) && (depth <= AVL_TREE_MAX_DEEP_PATH_CNS));
              depth++, pNode = pNode->right)
        {
            AVL_TREE_PATH_WAY_SET_1_MAC(path, depth);
            *pSeekedDataPtr = pNode->pData;
        }
        if (depth > 0)
        {
            AVL_TREE_PATH_DEEP_SET_MAC(path, (depth - 1));
            return GT_TRUE;
        }
        return GT_FALSE;
    }

    pathDepth = AVL_TREE_PATH_DEEP_GET_MAC(path);
    lastDepthOfLeft  = 0xFFFFFFFF;
    lastDepthOfRight = 0xFFFFFFFF;
    pLastLeftNode    = NULL;
    pLastRightNode   = NULL;
    lastLeftIsUpper  = GT_FALSE; /* default */
    lastRightIsUpper = GT_FALSE; /* default */
    for (depth = 0, pNode = treePtr->pHead;
          ((pNode != NULL) && (depth < pathDepth));
          depth++)
    {
        /* go according to given path */
        way = AVL_TREE_PATH_WAY_GET_MAC(path, depth);
        if (way)
        {
            /* save last alternative branches */
            lastDepthOfLeft = depth;
            pLastLeftNode   = pNode;
            lastLeftIsUpper = GT_TRUE;
            /* right */
            pNode = pNode->right;
        }
        else
        {
            /* save last alternative branches */
            lastDepthOfRight = depth;
            pLastRightNode   = pNode;
            lastRightIsUpper = GT_TRUE;
            /* left */
            pNode = pNode->left;
        }
    }

    if (pNode == NULL)
    {
        /* case of wrong path parameter    */
        /* path not reaches up to it's end */
        return GT_FALSE;
    }

    if (seekType == PRV_CPSS_AVL_TREE_SEEK_THIS_E)
    {
        *pSeekedDataPtr = pNode->pData;
        return GT_TRUE;
    }

    /* save last alternative branches            */
    /* additional iteration on last node in path */
    if (pNode->left)
    {
        /* left */
        lastDepthOfLeft = pathDepth + 1;
        pLastLeftNode   = pNode->left;
        lastLeftIsUpper = GT_FALSE;
    }
    if (pNode->right)
    {
        /* right */
        lastDepthOfRight = pathDepth + 1;
        pLastRightNode   = pNode->right;
        lastRightIsUpper = GT_FALSE;
    }

    if (seekType == PRV_CPSS_AVL_TREE_SEEK_NEXT_E)
    {
        if (pLastRightNode == NULL)
        {
            return GT_FALSE;
        }
        if (lastRightIsUpper != GT_FALSE)
        {
            AVL_TREE_PATH_DEEP_SET_MAC(path, lastDepthOfRight);
            *pSeekedDataPtr = pLastRightNode->pData;
            return GT_TRUE;
        }
        /* once to right and then to left while possible */
        AVL_TREE_PATH_WAY_SET_1_MAC(path, (lastDepthOfRight - 1));
        for (depth = lastDepthOfRight, pNode = pLastRightNode;
              ((pNode != NULL) && (depth <= AVL_TREE_MAX_DEEP_PATH_CNS));
              depth++, pNode = pNode->left)
        {
            AVL_TREE_PATH_WAY_SET_0_MAC(path, depth);
            *pSeekedDataPtr = pNode->pData;
        }
        AVL_TREE_PATH_DEEP_SET_MAC(path, (depth - 1));
        return GT_TRUE;
    }

    if (seekType == PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E)
    {
        if (pLastLeftNode == NULL)
        {
            return GT_FALSE;
        }
        if (lastLeftIsUpper != GT_FALSE)
        {
            AVL_TREE_PATH_DEEP_SET_MAC(path, lastDepthOfLeft);
            *pSeekedDataPtr = pLastLeftNode->pData;
            return GT_TRUE;
        }
        /* once to left and then to right while possible */
        AVL_TREE_PATH_WAY_SET_0_MAC(path, (lastDepthOfLeft - 1));
        for (depth = lastDepthOfLeft, pNode = pLastLeftNode;
              ((pNode != NULL) && (depth <= AVL_TREE_MAX_DEEP_PATH_CNS));
              depth++, pNode = pNode->right)
        {
            AVL_TREE_PATH_WAY_SET_1_MAC(path, depth);
            *pSeekedDataPtr = pNode->pData;
        }
        AVL_TREE_PATH_DEEP_SET_MAC(path, (depth - 1));
        return GT_TRUE;
    }

    return GT_FALSE;
}

/*******************************************************************************
* prvCpssAvlTreeIsOrderCorrect
*
* DESCRIPTION:
*       Check whether the tree data poiner by nodes ascending according
*       the given compare function
*
* INPUTS:
*       treeId   - ID of the tree.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE - correct, GT_FALSE - incorrect.
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvCpssAvlTreeIsOrderCorrect
(
    IN    PRV_CPSS_AVL_TREE_ID       treeId
)
{
    PRV_CPSS_AVL_TREE_PATH      path;
    GT_VOID_PTR                 dbEntry1Ptr;
    GT_VOID_PTR                 dbEntry2Ptr;
    PRV_CPSS_AVL_HEAD_STC       *treePtr = (PRV_CPSS_AVL_HEAD_STC*)treeId;

    if (GT_FALSE == prvCpssAvlPathSeek(
        treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
        path, &dbEntry1Ptr))
    {
        /* empty tree*/
        return GT_TRUE;
    }
    while (GT_FALSE != prvCpssAvlPathSeek(
        treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
        path, &dbEntry2Ptr))
    {
        if (GT_SMALLER != treePtr->compareFunc(dbEntry1Ptr, dbEntry2Ptr))
        {
            return GT_FALSE;
        }
        dbEntry1Ptr = dbEntry2Ptr;
    }
    return GT_TRUE;
}

/*******************************************************************************
* prvCpssAvlIterationDo
*
* DESCRIPTION:
*       Generic iretation in AVL Tree passing loop.
*       Serves to avoid additional finds/reseeks in loop body.
*
* INPUTS:
*       iteratorPtr   - (pointer to)iterator structure.
*
* OUTPUTS:
*       iteratorPtr   - (pointer to) updated iterator
*       dataPtr       - (pointer to) found/seeked value.
*
* RETURNS:
*       GT_TRUE - found/seeked, GT_FALSE - end of iteration reached.
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvCpssAvlIterationDo
(
    INOUT PRV_CPSS_AVL_TREE_ITERATOR_STC*   iteratorPtr,
    OUT   GT_VOID**                         dataPtr
)
{
    GT_BOOL result;

    if ((iteratorPtr->doFind == GT_FALSE)
        && (iteratorPtr->doSeek == GT_FALSE))
    {
        iteratorPtr->doDefaultSeek = GT_TRUE;
    }

    if (iteratorPtr->doFind != GT_FALSE)
    {
        /* clear */
        iteratorPtr->doFind = GT_FALSE;
        /* find */
        result = prvCpssAvlPathFind(
            iteratorPtr->treeId,
            iteratorPtr->findType,
            iteratorPtr->bookmarkPtr,
            iteratorPtr->path, dataPtr);
        if (result == GT_FALSE)
        {
            /* clear */
            iteratorPtr->doSeek        = GT_FALSE;
            iteratorPtr->doDefaultSeek = GT_FALSE;
            return GT_FALSE;
        }
    }

    if (iteratorPtr->doSeek != GT_FALSE)
    {
        /* clear */
        iteratorPtr->doSeek = GT_FALSE;
        /* seek */
        result = prvCpssAvlPathSeek(
            iteratorPtr->treeId,
            iteratorPtr->seekType,
            iteratorPtr->path, dataPtr);
        if (result == GT_FALSE)
        {
            /* clear */
            iteratorPtr->doDefaultSeek = GT_FALSE;
            return GT_FALSE;
        }
    }

    if (iteratorPtr->doDefaultSeek != GT_FALSE)
    {
        iteratorPtr->doDefaultSeek = GT_FALSE;
        /* default seek */
        return prvCpssAvlPathSeek(
            iteratorPtr->treeId,
            iteratorPtr->defaultSeekType,
            iteratorPtr->path, dataPtr);
    }

    return GT_TRUE;
}



