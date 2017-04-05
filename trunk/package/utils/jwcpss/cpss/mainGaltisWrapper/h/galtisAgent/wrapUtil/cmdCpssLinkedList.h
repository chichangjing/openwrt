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
* gtLinkedList.h
*
* DESCRIPTION:
*       This file includes a generic linked-list implementation, it provides the
*       following functions for the user:
*       - llInit(compareFunc, useSem): initializes a new linked list,
*         the function recieves a pointer to the comprison function (will be
*         described later), and a boolean param. which indicates whether this
*         list should be protected by a semaphore for mutual exclusion.
*         Returns a pointer to the new linked-list.
*
*       - llIsEmpty(pLlist): returns GT_TRUE if the linked-list pointed by
*         pLlist is an empty list.
*
*       - llSearch(pLlist,pData): searches for a given key in the
*         linked-list, and returns it if found or NULL otherwise.
*
*       - llGetFirstData(pLlist,pData): returns the data stored in the node
*         which is >= pData.
*         NULL in the pData field returns the first data in the linked-list.
*
*       - llAdd(pLlist,pData): inserts a new node, containing pData, to
*         the linked-list, this pointer is to a memory area which is already
*         allocated.
*
*       - llDelete(pLlist,pData): deletes the linked-list node that contains
*         the data pData, and returns a pointer to the Node-Data.
*
*       - llAddFirst(pLlist,pData): Adds a new data to the beginning of the
*         linked-list.
*
*       - llAddLast(pLlist, pData): Adds a new data to the end of the
*         linked-list.
*
*       - llDestroy(pLlist, dataFreeFunc): Destroys the given linked-list, and
*         frees the user data by calling dataFreeFun with the user stored data
*         as a parameter.
*
*       - llGetFirstNode(pLlist): retuns the first node in the linked-list.
*
*       - llGetNextNode(pNode): retuns the next node after pNode in the
*         linked-list.
*
*       - llGetNext(pSlHead,iteratorId): Iterator function, returns the next
*         data in the linked-list, according to the input interatorId, or NULL
*         if no nodes left.
*
*       - llGetIteratorForNode(pSlHead,pData,iteratorId): Returns the iteratorId
*         for the given data, this iterator can be used to pass over the
*         linked-list from a specific location.
*
*       - Requirements from the compare function:
*           - prototype: GT_LL_COMP_RES compFunc(pStruct1,pStruct2) where
*             pStructi is a pointer to the data structures stroed in the
*             linked-list.
*           - functionality:
*
*               if((pStruct1 == NULL) && (pStruct2 == NULL))
*                   return GT_LL_EQUAL;
*               if((pStruct1 == NULL) && (pStruct2 != NULL))
*                   return GT_LL_SMALLER;
*               if((pStruct1 != NULL) && (pStruct2 == NULL))
*                   return GT_LL_GREATER;
*
*               if(pStruct1 == pStruct2)
*                   return GT_LL_EQUAL;
*
*               if (pStruct1 < pStruct2)
*                   return GT_LL_SMALLER;
*
*               return GT_LL_GREATER;
*
*       Where the comparison operators are defined according to the user's needs
*
*       Inorder for the user to be able to use the above functionality, the file
*       <common/gtLinkedList.h> must be included.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/
#ifndef __gtLinkedListh
#define __gtLinkedListh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/* Values to be returned by the linked-list compare function */
typedef enum
{
    GT_LL_EQUAL   = 0,
    GT_LL_GREATER = 1,
    GT_LL_SMALLER = 2
}GT_LL_COMP_RES;

typedef void STRUCT_LL_DATA;

typedef struct _linkedListNode
{
    STRUCT_LL_DATA  *pLlData;
    struct  _linkedListNode* pNext; /* a pointer to the next node           */
}STRUCT_LL_NODE;


/*
 * Typedef: struct STRUCT_LL_HEAD
 *
 * Description: Holds a pointer to the first node in the linked list, and
 *              all common data needed for linked-list management.
 *
 * Fields:
 *      compareFunc -   linked-list nodes comparison function, recieves two
 *                      structs and returns:
 *                      GT_LL_EQUAL / GT_LL_GREATER / GT_LL_SMALLER.
 *      firstNode     - A pointer to the first linked list node.
 *
 */
typedef struct _llHead
{
    unsigned int  (* compareFunc)  ();

    GT_U32  numOfNodes;

    STRUCT_LL_NODE *firstNode;
}STRUCT_LL_HEAD;



/*******************************************************************************
* llInit
*
* DESCRIPTION:
*       initializes a new linked list.
*
* INPUTS:
*       compareFunc - A pointer to the comprison function, to be used for nodes
*                     sorting.
*       useSem  - Indicates whether this list should be protected by a semaphore
*                 for mutual exclusion.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the new linked-list.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_LL_HEAD* llInit
(
    IN unsigned int  (* compareFunc)  (),
    IN GT_BOOL useSem
);


/*******************************************************************************
* llIsEmpty
*
* DESCRIPTION:
*       Checks whether the linked-list is empty.
*
* INPUTS:
*       pLlist - A pointer to the linked-list to check.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE is the linked-list contains no nodes, GT_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL llIsEmpty
(
    IN STRUCT_LL_HEAD *pLlist
);


/*******************************************************************************
* llSearch
*
* DESCRIPTION:
*       searches for a given key in the linked-list.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to search in.
*       pData   - A pointer to the data to search for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the found data, or NULL if not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_LL_DATA* llSearch
(
    IN STRUCT_LL_HEAD *pLlist,
    IN STRUCT_LL_DATA *pData
);


/*******************************************************************************
* llGetFirstData
*
* DESCRIPTION:
*       Returns the first data stored in the linked list which is >= pData.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to search in.
*       pData   - The data to search for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the found data, or NULL if no such data was found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_LL_DATA* llGetFirstData
(
    IN STRUCT_LL_HEAD *pLlist,
    IN STRUCT_LL_DATA *pData
);



/*******************************************************************************
* llAdd
*
* DESCRIPTION:
*       inserts a new node, containing pData, to the linked-list.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to insert to.
*       pData   - A pointer to the data to be inserted.
*
* OUTPUTS:
*       pLlist  - A pointer to the linked-list after insertion.
*
* RETURNS:
*       GT_OK if insertion succeeded
*       GT_ALREADY_EXIST if the element is already exists.
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS llAdd
(
    IN STRUCT_LL_HEAD *pLlist,
    IN STRUCT_LL_DATA *pData
);

/*******************************************************************************
* llDelete
*
* DESCRIPTION:
*       Deletes the linked-list node that contains pData.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to delete from.
*       pData   - A struct containing the data to be deleted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the data stored in the deleted linked-list node.
*
* COMMENTS:
*       1.  The data stored in the linked-list is not deleted, it's the
*           user's responsibility to free this structure.
*
*******************************************************************************/
STRUCT_LL_DATA* llDelete
(
    STRUCT_LL_HEAD *pLlist,
    STRUCT_LL_DATA *pData
);



/*******************************************************************************
* llAddFirst
*
* DESCRIPTION:
*       Adds a new data to the beginning of the linked-list.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to add to.
*       pData   - A pointer to the data to be added.
*
* OUTPUTS:
*       pLlist  - A pointer to the linked-list after adding pData.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS llAddFirst
(
    IN STRUCT_LL_HEAD *pLlist,
    IN STRUCT_LL_DATA *pData
);



/*******************************************************************************
* llAddLast
*
* DESCRIPTION:
*       Adds a new data to the end of the linked-list.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to add to.
*       pData   - A pointer to the data to be added.
*
* OUTPUTS:
*       pLlist  - A pointer to the linked-list after adding pData.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS llAddLast
(
    IN STRUCT_LL_HEAD *pLlist,
    IN STRUCT_LL_DATA *pData
);


/*******************************************************************************
* llDestroy
*
* DESCRIPTION:
*       Destroys the given linked-list.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to be destroyed.
*       dataFreeFunc - A pointer to a function to be called for deleteing the
*                      user data, or NULL if no free operation should be done
*                      on the user's data.
*
* OUTPUTS:
*       pLlist  - Points to NULL, this node will also be deleted.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS llDestroy
(
    IN STRUCT_LL_HEAD *pLlist,
    IN GT_VOIDFUNCPTR dataFreeFunc
);



/*******************************************************************************
* llGetFirstNode
*
* DESCRIPTION:
*       Retuns the first node in the linked-list.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to get the first node from.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the first node in the linked-list
*
* COMMENTS:
*       None.
*
*******************************************************************************/
STRUCT_LL_NODE* llGetFirstNode
(
    IN STRUCT_LL_HEAD *pLlist
);



/*******************************************************************************
* llGetNextNode
*
* DESCRIPTION:
*       Retuns the next node after pNode in the linked-list.
*
* INPUTS:
*       pNode  - A pointer to the current node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the next node after pNode.
*
* COMMENTS:
*        1. This function may return NULL if pNode is the last node in the
*           linked-list.
*
*******************************************************************************/
STRUCT_LL_NODE* llGetNextNode
(
    IN STRUCT_LL_NODE *pNode
);


/*******************************************************************************
* llGetNext
*
* DESCRIPTION:
*       This function is used for iterations over the linked-list.
*
* INPUTS:
*       pLlHead     - A pointer to the linked-list head.
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
*       1.  If (*iteratorId == 0): the first node in the linked-list is returned
*       2.  The new value of iterator Id should be sent in the next call to
*           this function to get the next data.
*
*******************************************************************************/
STRUCT_LL_DATA* llGetNext
(
    IN STRUCT_LL_HEAD *pLlHead,
    INOUT GT_UINTPTR *iteratorId
);

/*******************************************************************************
* llGetIteratorForNode
*
* DESCRIPTION:
*       Returns the iteratorId for the given data, this iterator can be used to
*       pass over the linked-list from a specific location.
*
* INPUTS:
*       pLlHead - The lined-list head.
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
STRUCT_LL_DATA* llGetIteratorForNode
(
    IN  STRUCT_LL_HEAD  *pLlHead,
    IN  STRUCT_LL_DATA  *pData,
    OUT GT_UINTPTR      *iteratorId
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtLinkedListh */


