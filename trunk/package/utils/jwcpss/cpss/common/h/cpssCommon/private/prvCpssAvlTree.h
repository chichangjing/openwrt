/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssAvlTree.h
*
* DESCRIPTION:
*       AVL Tree implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/


#ifndef __prvCpssAvlTree_h
#define __prvCpssAvlTree_h

#ifdef __cplusplus
extern "C" {
#endif


/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpssCommon/cpssBuffManagerPool.h>


/************ Typedefs ***********************************************/

/*
 * Typedef: PRV_CPSS_AVL_TREE_ID
 *
 * Description: type of tree's ID
 *
 */
typedef GT_VOID * PRV_CPSS_AVL_TREE_ID;

/*
 * Typedef: PRV_CPSS_AVL_TREE_ITERATOR_ID
 *
 * Description: type of tree's iterator ID
 *
 */
typedef GT_VOID *PRV_CPSS_AVL_TREE_ITERATOR_ID;

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
);

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
);

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
*                     If In case pool ID is GT_POOL_NULL_ID, memory will be allocated
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
    OUT  PRV_CPSS_AVL_TREE_ID         *pTreeId
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/******************************************************************************/
/*                       Fast iteration functions                             */
/******************************************************************************/

/* this array contais 64 bits                    */
/* - 6  first bits for current deepness          */
/* - 58 last bits for path: 0 - left, 1 - right  */
typedef GT_U8 PRV_CPSS_AVL_TREE_PATH[8];

/*
 * typedef: enum PRV_CPSS_AVL_TREE_FIND_ENT
 *
 * Description:
 *      Find request types for find function.
 *
 * Enumerations:
 *
 *      PRV_CPSS_AVL_TREE_FIND_EQUAL_E                - find equal
 *      PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E    - find maximal less or equal,
 *      PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E - find minimal greater or equal,
 *
 */
typedef enum
{
    PRV_CPSS_AVL_TREE_FIND_EQUAL_E,
    PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,
    PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E
} PRV_CPSS_AVL_TREE_FIND_ENT;

/*
 * typedef: enum PRV_CPSS_AVL_TREE_SEEK_ENT
 *
 * Description:
 *      Seek request types for seek function.
 *
 * Enumerations:
 *
 *      PRV_CPSS_AVL_TREE_SEEK_THIS_E      - seek node by path exactly,
 *      PRV_CPSS_AVL_TREE_SEEK_NEXT_E      - seek next node,
 *      PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E  - seek previous node,
 *      PRV_CPSS_AVL_TREE_SEEK_FIRST_E     - seek first node,
 *      PRV_CPSS_AVL_TREE_SEEK_LAST_E      - seek last node,
 *
 */
typedef enum
{
    PRV_CPSS_AVL_TREE_SEEK_THIS_E,
    PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
    PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E,
    PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
    PRV_CPSS_AVL_TREE_SEEK_LAST_E
} PRV_CPSS_AVL_TREE_SEEK_ENT;

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
);

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
);

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
);

/******************************************************************************/
/*                       Generic iteration function                           */
/******************************************************************************/
/*
 * Typedef: struct PRV_CPSS_AVL_TREE_ITERATOR_STC
 *
 * Description:
 *     This structure AVL Tree generic iterator.
 *
 * Fields:
 *       path              - Tree position path.
 *       treeId            - tree Id.
 *       defaultSeekType   - default Seek Type.
 *                           Default seek done when both doFind and doSeek are GT_FALSE.
 *       doFind            - if GT_TRUE - find instead of default seek.
 *                           Cleared by generic iterator function.
 *       doSeek            - if GT_TRUE - seek instead of default seek.
 *                           Cleared by generic iterator function.
 *       bookmarkPtr       - (pointer to) find data.
 *       findType          - find type.
 *       seekType          - seek type.
 *
 *  Comments:
 *
 */
typedef struct
{
    /* position in tree keeper */
    PRV_CPSS_AVL_TREE_PATH      path;
    /* default seek parameters */
    PRV_CPSS_AVL_TREE_ID        treeId;
    PRV_CPSS_AVL_TREE_SEEK_ENT  defaultSeekType;
    /* initial/reposition find + seek parameters  */
    /* cleaned up by  generic iteration function. */
    GT_BOOL                     doFind;
    GT_BOOL                     doSeek;
    GT_BOOL                     doDefaultSeek;
    GT_VOID                     *bookmarkPtr;
    PRV_CPSS_AVL_TREE_FIND_ENT  findType;
    PRV_CPSS_AVL_TREE_SEEK_ENT  seekType;
} PRV_CPSS_AVL_TREE_ITERATOR_STC;

/* typical iterator reposition after find */
#define PRV_CPSS_AVL_TREE_ITERATOR_RESEEK_MAC(_iter)                     \
    {                                                                    \
        _iter.doDefaultSeek = GT_TRUE;                                   \
    }

/* typical iterator reposition by find */
#define PRV_CPSS_AVL_TREE_ITERATOR_FIND_MAC(_iter, _data, _find_type)    \
    {                                                                    \
        _iter.doFind = GT_TRUE;                                          \
        _iter.bookmarkPtr = _data;                                       \
        _iter.findType = _find_type;                                     \
    }

/* typical iterator initialization */
#define PRV_CPSS_AVL_TREE_ITERATOR_INIT_MAC(_iter, _tree, _ascending)    \
    {                                                                    \
        _iter.treeId = _tree;                                            \
        _iter.doSeek = GT_TRUE;                                          \
        _iter.doFind = GT_FALSE;                                         \
        _iter.doDefaultSeek = GT_FALSE;                                  \
        if (_ascending != GT_FALSE)                                      \
        {                                                                \
            _iter.seekType = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;             \
            _iter.defaultSeekType = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;       \
        }                                                                \
        else                                                             \
        {                                                                \
            _iter.seekType = PRV_CPSS_AVL_TREE_SEEK_LAST_E;              \
            _iter.defaultSeekType = PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E;   \
        }                                                                \
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
);

#ifdef __cplusplus
}
#endif

#endif /* __prvCpssAvlTree_h */


