/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtSkipList.h
*
* DESCRIPTION:
*       This file includes a generic skip-list implementation, it provides the
*       following functions for the user:
*       - skipInit: initializes a new skiplist, the function recieves a pointer
*         to the comprison function (will be described later).
*         Returns a pointer to the new skip-list.
*
*       - skipIsEmpty(pSlHead): returns GT_TRUE if the skip-list pointed by
*         pSkipList is an empty list.
*
*       - skipSearch(pSlHead,pData): searches for a given key in the
*         skip-list, and returns it if found or NULL otherwise.
*
*       - skipGetFirst(pSlHead,pData): returns the first node that includes a
*         data which is >= pData, the comparison is done according to the user
*         compare function.
*
*       - skipGetFirstData(pSlHead,pData): returns the data stored in the node
*         which  is returned from skipGetFirst().
*         NULL in the pData field returns the first data in the skip-list.
*
*       - skipGetNext(pSlHead,iteratorId): Iterator function, returns the next
*         data in the skip-list, according to the input interatorId, or NULL if
*         no nodes left.
*
*       - skipGetIteratorForNode(pSlHead,pData,iteratorId): Returns the
*         iteratorId for the given data, this iterator can be used to pass
*         over the skip-list from a specific location.
*
*       - skipAdd2DownLevel(pSlHead,pData): inserts a node to the lowest
*         level of the skip-list.
*
*       - skipAdd(pSlHead,pData): inserts a new node, containing pData, to
*         the skip-list, this pointer is to a memory area which is already
*         allocated in memory.
*
*       - skipDelete(pSlHead,pData): deletes the skip-list node that cont-
*         ains the data pData, and returns a pointer to the Node-Data.
*
*       - skipDestroy(pSlHead, dataFreeFunc): Destroys the given skip-list, and
*         frees the user data by calling dataFreeFun with the user stored data
*         as a parameter.
*
*       - Requirements from the compare function:
*           - prototype: GT_LL_COMP_RES compFunc(pStruct1,pStruct2) where
*             pStructi is a pointer to the data structures stroed in the
*             skip-list.
*           - functionality:
*
*               if((pStruct1 == NULL) && (pStruct2 == NULL))
*                   return GT_SL_EQUAL;
*               if((pStruct1 == NULL) && (pStruct2 != NULL))
*                   return GT_SL_SMALLER;
*               if((pStruct1 != NULL) && (pStruct2 == NULL))
*                   return GT_SL_GREATER;
*
*               if(pStruct1 == pStruct2)
*                   return GT_SL_EQUAL;
*
*               if (pStruct1 < pStruct2)
*                   return GT_SL_SMALLER;
*
*               return GT_SL_GREATER;
*
*       Where the comparison operators are defined according to the user's needs.
*
*       Inorder for the user to be able to use the above functionality, the file
*       "common/gtSkipList.h" must be included.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#ifndef __gtSkipListh
#define __gtSkipListh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtOs/gtGenTypes.h>

/* Values to be returned by the skip-list compare function */
typedef enum
{
    GT_SL_EQUAL   = 0,
    GT_SL_GREATER = 1,
    GT_SL_SMALLER = 2
}GT_SL_COMP_RES;

typedef void STRUCT_SL_DATA;


typedef struct skipListNode
{
    STRUCT_SL_DATA  *pSLData;

    struct  skipListNode* pNext;    /* a pointer to the next node           */
    struct  skipListNode* pPrev;    /* a pointer to the previous node       */
    struct  skipListNode* pDown;    /* a pointer to the lower node          */
}STRUCT_SL_NODE;


typedef struct
{
    GT_INTFUNCPTR compareFunc;  /* skip-list nodes comparison function,     */
                                /* recieves two structs and returns         */
                                /* GT_SL_EQUAL/GT_SL_GREATER/GT_SL_SMALLER  */
    GT_UINTPTR  skiplistSem;    /* a semaphore to forbid more than one process  */
                            /* to change the skiplist                       */
    struct  skipListNode* pFirstNode;   /* a pointer to the skip-list       */

}STRUCT_SL_HEAD;


/*******************************************************************************
* skipInit
*
* DESCRIPTION:
*       Initializes a new skipList.
*
* INPUTS:
*       pCompFunc - A pointer to the skipList comparison function.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the new skipList, or NULL if memory allocation failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_SL_HEAD* skipInit
(
    IN GT_INTFUNCPTR pCompFunc
);


/*******************************************************************************
* skipIsEmpty
*
* DESCRIPTION:
*       checks if a skip-List is empty.
*
* INPUTS:
*       pSlHead - a pointer to the head of the skipList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if the skipList is empty, GT_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL skipIsEmpty
(
    IN STRUCT_SL_HEAD *pSlHead
);



/*******************************************************************************
* skipGetFirst
*
* DESCRIPTION:
*       search for a given key in the skip list.
*       This function returns a pointer to the first node with key >= pData,
*       OR NULL if there is no such node.
*
* INPUTS:
*       pSlHead - a pointer to the head of the skipList.
*       pData   - the key of the element to be searched.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       a pointer to the first node with data >= pData, OR NULL if no such node
*       exists.
*
* COMMENTS:
*       None.
*******************************************************************************/
STRUCT_SL_NODE* skipGetFirst
(
    IN STRUCT_SL_HEAD *pSlHead,
    IN STRUCT_SL_DATA *pData
);



/*******************************************************************************
* skipGetFirstData
*
* DESCRIPTION:
*       search for a given key in the skip list.
*       This function returns a pointer to the data stored in the first node
*       with key >= pData, OR NULL if there is no such node.
*
* INPUTS:
*       pSlHead - a pointer to the head of the skipList.
*       pData   - the key of the element to be searched.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       a pointer to the first node with data >= pData, OR NULL if no such node
*       exists.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_SL_DATA* skipGetFirstData
(
    IN STRUCT_SL_HEAD *pSlHead,
    IN STRUCT_SL_DATA *pData
);



/*******************************************************************************
* skipGetNext
*
* DESCRIPTION:
*       This function is used for iterations over the skip-list.
*
* INPUTS:
*       pSlHead     - A pointer to the skip-list head.
*       iteratorId  - The iterator Id that was returned from the last call to
*                     this function.
*
* OUTPUTS:
*       iteratorId  - The iteration Id to be sent in the next call to this
*                     function to get the next data.
*
* RETURNS:
*       A pointer to the next node data if exists, NULL otherwise.
*
* COMMENTS:
*       1.  If (*iteratorId == 0): the first node in the skip-list is returned.
*       2.  The new value of iterator Id should be sent in the next call to
*           this function to get the next data.
*
*******************************************************************************/
STRUCT_SL_DATA* skipGetNext
(
    IN STRUCT_SL_HEAD *pSlHead,
    INOUT GT_UINTPTR *iteratorId
);


/*******************************************************************************
* skipGetIteratorForNode
*
* DESCRIPTION:
*       Returns the iteratorId for the given data, this iterator can be used to
*       pass over the skip-list from a specific location.
*
* INPUTS:
*       pSlHead - The skip-list head.
*       pData   - A pointer to the data to be searched.
*
* OUTPUTS:
*       iteratorId  - The iterator Id.
*
* RETURNS:
*       The data represented by iteratorId if succeeded, or NULL otherwise.
*
* COMMENTS:
*       1.  The returned iterator represents the first data which is >= pData.
*
*******************************************************************************/
STRUCT_SL_DATA* skipGetIteratorForNode
(
    IN  STRUCT_SL_HEAD  *pSlHead,
    IN  STRUCT_SL_DATA  *pData,
    OUT GT_U32          *iteratorId
);

/*******************************************************************************
* skipAdd2DownLevelAndGetNode
*
* DESCRIPTION:
*       Adds a new node to the skipList at the lowest level.
*
* INPUTS:
*       pSlHead - A pointer to the skipList to which this node is added.
*       pData   - A pointer to the new data to be added.
*
* OUTPUTS:
*       addedNode   - Points to the new added node as a result of the insertion.
*
* RETURNS:
*       A pointer to the new added node.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS skipAdd2DownLevelAndGetNode
(
    IN  STRUCT_SL_HEAD  *pSlHead,
    IN  STRUCT_SL_DATA  *pData,
    OUT STRUCT_SL_NODE  **addedNode
);


/*******************************************************************************
* skipAdd2DownLevel
*
* DESCRIPTION:
*       Adds a new node to the skipList at the lowest level.
*
* INPUTS:
*       pSlHead - A pointer to the skipList to which this node is added.
*       pData   - A pointer to the new data to be added.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the new added node.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS skipAdd2DownLevel
(
    IN STRUCT_SL_HEAD *pSlHead,
    IN STRUCT_SL_DATA *pData
);


/*******************************************************************************
* skipAddAndGetNode
*
* DESCRIPTION:
*       adds a new node to the skipList.
*       This function returns a pointer to the new skipList, containing the data
*       pData. it also adds a new level to the skipList if a new node was added
*       to the top level (by probability of 0.5).
*
* INPUTS:
*       pSlHead - a pointer to the skipList, to which the new node will be
*                 added.
*       pData   - A pointer to the new data to be added.
*       newAddedNode - A pointer to the new added node, or NULL if this
*                      information is not required.
*
* OUTPUTS:
*       newAddedNode    - A pointer to the new added node at the bottom level
*                         of the skip-list.
*
* RETURNS:
*       GT_OK if succeeded, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS skipAddAndGetNode
(
    IN      STRUCT_SL_HEAD  *pSlHead,
    IN      STRUCT_SL_DATA  *pData,
    INOUT   STRUCT_SL_NODE  **newAddedNode
);


/*******************************************************************************
* skipAdd
*
* DESCRIPTION:
*       adds a new node to the skipList.
*       This function returns a pointer to the new skipList, containing the data
*       pData. it also adds a new level to the skipList if a new node was added
*       to the top level (by probability of 0.5).
*
* INPUTS:
*       pSlHead - a pointer to the skipList, to which the new node will be
*                 added.
*       pData   - A pointer to the new data to be added.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if succeeded, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS skipAdd
(
    IN STRUCT_SL_HEAD *pSlHead,
    IN STRUCT_SL_DATA *pData
);


/*******************************************************************************
* skipAddAfter
*
* DESCRIPTION:
*       adds a new node after a given node.
*       This function returns a pointer to the added node, which has the data
*       pData.
*
* INPUTS:
*       pData   - A pointer to the data of the element to be added.
*       pNode   - A pointer to the node that the new node should be added after.
*
* OUTPUTS:
*       pNode   - Next pointer will point to a new node holding pData.
*
* RETURNS:
*       A pointer to the node that has been added, OR NULL if can't allocate
*       memory.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_SL_NODE* skipAddAfter
(
    IN STRUCT_SL_DATA *pData,
    INOUT STRUCT_SL_NODE *pNode
);


/*******************************************************************************
* skipDelete
*
* DESCRIPTION:
*       Deletes a node from the skipList.
*
* INPUTS:
*       pSlHead - A pointer to the SkipList head to delete from.
*       pData   - A pointer to the data of the node to be deleted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the skip-list data which was deleted, or NULL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_SL_DATA* skipDelete
(
    IN STRUCT_SL_HEAD *pSlHead,
    IN STRUCT_SL_DATA *pData
);


/*******************************************************************************
* skipSearch
*
* DESCRIPTION:
*       This function searches for a given key in the skip-list.
*
* INPUTS:
*       pSlHead - a pointer to the head of the skip-list to be searched.
*       pData   - a pointer to the data to be searched.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the data stored in the found node, or NULL if no such node
*       exists.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_SL_DATA* skipSearch
(
    IN STRUCT_SL_HEAD *pSlHead,
    IN STRUCT_SL_DATA *pData
);



/*******************************************************************************
* skipDestroy
*
* DESCRIPTION:
*       Destroys the given skip-list.
*
* INPUTS:
*       pSlHead     - A pointer to the skip-list to be destroyed.
*       dataFreeFunc- A pointer to a function to be called for deleteing the
*                     user data, or NULL if no free operation should be done
*                     on the user's data.
*
* OUTPUTS:
*       pSlHead     - Points to NULL, this node will also be deleted.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS skipDestroy
(
    IN STRUCT_SL_HEAD *pSlHead,
    IN GT_VOIDFUNCPTR dataFreeFunc
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtSkipListh */

