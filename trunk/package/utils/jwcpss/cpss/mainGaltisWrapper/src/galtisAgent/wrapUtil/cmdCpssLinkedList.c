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
*       Where the comparison operators are defined according to the user's needs.
*
*       Inorder for the user to be able to use the above functionality, the file
*       <common/gtLinkedList.h> must be included.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 29 $
*
*******************************************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <galtisAgent/wrapUtil/cmdCpssLinkedList.h>
#include <cpss/generic/cpssTypes.h>



/*******************************************************************************
 * Linked-list library semaphore
 ******************************************************************************/

/* Forward functions declarations */
typedef GT_COMP_RES (*prototypeCmpFunc)(void* elem_PTR1,void* elem_PTR2);
typedef void (*prototypeDataFreeFunc)(void*);

/*******************************************************************************
* llNewNode
*
* DESCRIPTION:
*       Creates a new node.
*
*       This function returns a pointer to a new node that has the following
*       values:
*       -   llData= pData
*       -   pNext = next
*
* INPUTS:
*       pData   - A pointer to the data in the created node.
*       next    - a pointer to node that the new node should point to by pNext.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the new created node if the operation was successful,
*       NULL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static STRUCT_LL_NODE* llNewNode
(
    IN STRUCT_LL_DATA *pData,
    IN STRUCT_LL_NODE *next
)
{
    STRUCT_LL_NODE*   pNewNode;   /* pointer to the new added node. */

    pNewNode = (STRUCT_LL_NODE*)cmdOsMalloc(sizeof(STRUCT_LL_NODE));
    if (pNewNode == NULL)
            return NULL;

    pNewNode->pLlData   = pData;
    pNewNode->pNext     = next;

    return pNewNode;
}





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
    IN GT_INTFUNCPTR compareFunc,
    IN GT_BOOL useSem
)
{
    STRUCT_LL_HEAD*   pHead;        /* pointer to thelinked-list head.      */

    useSem = GT_FALSE; /* no semaphores at all in linked lists. */

    pHead = (STRUCT_LL_HEAD*)cmdOsMalloc(sizeof(STRUCT_LL_HEAD));
    if (pHead == NULL)
    {
        return NULL;
    }

    pHead->compareFunc  = compareFunc;
    pHead->numOfNodes   = 0;

    /* Create a node to hold the NULL data, which will allway   */
    /* be the first node in the linked-list.                    */
    pHead->firstNode    = llNewNode(NULL,NULL);

    if(pHead->firstNode == NULL)
    {
        cmdOsFree(pHead);
        return NULL;
    }

    return pHead;
}


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
)
{
    if(pLlist->numOfNodes == 0)
        return GT_TRUE;

    return GT_FALSE;
}


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
)
{
    prototypeCmpFunc compFunc;
    STRUCT_LL_NODE *pNode;
    GT_LL_COMP_RES compareRes;
    STRUCT_LL_DATA *retVal = NULL;

    compFunc = (prototypeCmpFunc)pLlist->compareFunc;

    pNode = pLlist->firstNode->pNext;

    while(pNode != NULL)
    {
        compareRes = compFunc(pNode->pLlData,pData);

        if(compareRes == GT_LL_EQUAL)
        {
            retVal = pNode->pLlData;
            break;
        }

        if(compareRes == GT_LL_GREATER)
        {
            retVal = NULL;
            break;
        }

        pNode = pNode->pNext;
    }

    return retVal;
}


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
)
{
    prototypeCmpFunc compFunc;
    STRUCT_LL_NODE *pNode;
    GT_LL_COMP_RES compareRes;
    STRUCT_LL_DATA *retVal = NULL;

    compFunc = (prototypeCmpFunc)pLlist->compareFunc;

    pNode = pLlist->firstNode->pNext;

    while(pNode != NULL)
    {
        compareRes = compFunc(pNode->pLlData,pData);

        if(compareRes != GT_LL_SMALLER)
        {
            retVal = pNode->pLlData;
            break;
        }

        pNode = pNode->pNext;
    }

    return retVal;
}



/*******************************************************************************
* llGetFirst
*
* DESCRIPTION:
*       Returns the first node in the linked list with data >= pData.
*
* INPUTS:
*       pLlist  - A pointer to the linked-list to search in.
*       pData   - The data to search for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the found node, or NULL if no such node was found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static STRUCT_LL_NODE* llGetFirst
(
    IN STRUCT_LL_HEAD *pLlist,
    IN STRUCT_LL_DATA *pData
)
{
    prototypeCmpFunc compFunc;
    STRUCT_LL_NODE *pNode;
    GT_LL_COMP_RES compareRes;
    STRUCT_LL_NODE *retVal = NULL;

    compFunc = (prototypeCmpFunc)pLlist->compareFunc;

    pNode = pLlist->firstNode->pNext;

    while(pNode != NULL)
    {
        compareRes = compFunc(pNode->pLlData,pData);

        if(compareRes != GT_LL_SMALLER)
        {
            retVal = pNode;
            break;
        }

        pNode = pNode->pNext;
    }

    return retVal;
}



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
*       GT_OK if insertion succeeded,
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
)
{
    prototypeCmpFunc compFunc;
    STRUCT_LL_NODE *pNode;
    STRUCT_LL_NODE *pPrevNode = NULL;
    GT_LL_COMP_RES compareRes;
    GT_BOOL insertLast = GT_FALSE;
    GT_STATUS retVal = GT_FAIL;

    compFunc = (prototypeCmpFunc)pLlist->compareFunc;

    pNode       = pLlist->firstNode;

    while(pNode != NULL)
    {
        compareRes = compFunc(pNode->pLlData,pData);

        if(compareRes == GT_LL_EQUAL)
        {
            retVal = GT_ALREADY_EXIST;
            break;
        }

        if((pNode->pNext == NULL) && (compareRes != GT_LL_GREATER))
        {
            pPrevNode   = pNode;
            insertLast  = GT_TRUE;
        }

        if((compareRes == GT_LL_GREATER) || (insertLast))
        {
            if (pPrevNode == NULL)
            {
                retVal = GT_FAIL;
                break;
            }
            pPrevNode->pNext = llNewNode(pData,pPrevNode->pNext);
            if(pPrevNode->pNext == NULL)
                retVal = GT_FAIL;
            else
                retVal = GT_OK;

            break;
        }

        pPrevNode = pNode;
        pNode = pNode->pNext;
    }

    if(retVal == GT_OK)
        pLlist->numOfNodes++;

    return retVal;
}



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
)
{
    prototypeCmpFunc compFunc;
    STRUCT_LL_NODE *pNode;
    STRUCT_LL_NODE *pPrevNode;
    GT_LL_COMP_RES compareRes;
    STRUCT_LL_DATA *retVal = NULL;

    if (pLlist->numOfNodes == 0)
    {
        return NULL;
    }

    compFunc = (prototypeCmpFunc)pLlist->compareFunc;

    pNode       = pLlist->firstNode->pNext;
    pPrevNode   = pLlist->firstNode;

    compareRes = compFunc(pNode->pLlData,pData);

    while(compareRes != GT_LL_EQUAL)
    {
        pPrevNode   = pNode;
        pNode       = pNode->pNext;

        if(pNode == NULL)
            break;

        compareRes = compFunc(pNode->pLlData,pData);
    }


    if(pNode != NULL)
    {
        pPrevNode->pNext = pNode->pNext;
        retVal = pNode->pLlData;
        cmdOsFree(pNode);
    }

    if(retVal != NULL)
        pLlist->numOfNodes--;

    return retVal;
}


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
)
{
    STRUCT_LL_NODE *pNewNode;
    GT_STATUS retVal = GT_OK;

    pNewNode = llNewNode(pData,pLlist->firstNode->pNext);

    if(pNewNode == NULL)
        retVal = GT_FAIL;
    else
        pLlist->firstNode->pNext = pNewNode;

    if(retVal == GT_OK)
        pLlist->numOfNodes++;

    return retVal;
}



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
)
{
    STRUCT_LL_NODE *pNode;
    STRUCT_LL_NODE *pNewNode;
    GT_STATUS retVal = GT_OK;

    pNode       = pLlist->firstNode;

    while(pNode->pNext != NULL)
    {
        pNode = pNode->pNext;
    }

    pNewNode = llNewNode(pData,NULL);

    if(pNewNode == NULL)
        retVal = GT_FAIL;
    else
        pNode->pNext = pNewNode;

    if(retVal == GT_OK)
        pLlist->numOfNodes++;

    return retVal;
}


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
)
{
    STRUCT_LL_NODE *pNode;
    STRUCT_LL_NODE *pDelNode;

    pNode = pLlist->firstNode;

    while(pNode->pNext != NULL)
    {
        pDelNode = pNode->pNext;
        pNode->pNext = pNode->pNext->pNext;
        ((prototypeDataFreeFunc)dataFreeFunc)(pDelNode->pLlData);
        cmdOsFree(pDelNode);
    }

    cmdOsFree(pNode);

    cmdOsFree(pLlist);

    return GT_OK;
}



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
)
{
    return pLlist->firstNode->pNext;
}


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
)
{
    return pNode->pNext;
}



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
)
{
    STRUCT_LL_NODE *pNode;

    if(*iteratorId == 0)
        pNode = llGetFirstNode(pLlHead);
    else
    {
        pNode = (STRUCT_LL_NODE*)(*iteratorId);
        pNode = pNode->pNext;
    }

    if(pNode == NULL)
        return NULL;

    *iteratorId = (GT_UINTPTR)pNode;

    return pNode->pLlData;
}


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
)
{
    STRUCT_LL_NODE *pNode;

    pNode = llGetFirst(pLlHead,pData);
    if(pNode == NULL)
    {
        return NULL;
    }

    *iteratorId = (GT_UINTPTR)pNode;

    return pNode->pLlData;
}

#undef DEBUG_LL
#ifdef DEBUG_LL

typedef struct
{
    GT_U32 a;
}STRUCT_LL_TEST;


GT_STATUS printLL(STRUCT_LL_HEAD *pLlist,GT_VOIDFUNCPTR printData)
{
    STRUCT_LL_NODE *pNode;

    pNode = pLlist->firstNode->pNext;

    while(pNode != NULL)
    {
        printData(pNode->pLlData);
        pNode = pNode->pNext;
    }

    return GT_OK;
}


GT_LL_COMP_RES testCompFunc(STRUCT_LL_TEST *pStruct1,STRUCT_LL_TEST *pStruct2)
{
    if((pStruct1 == NULL) && (pStruct2 == NULL))
        return GT_LL_EQUAL;
    if((pStruct1 == NULL) && (pStruct2 != NULL))
        return GT_LL_SMALLER;
    if((pStruct1 != NULL) && (pStruct2 == NULL))
        return GT_LL_GREATER;

    if(pStruct1->a == pStruct2->a)
        return GT_LL_EQUAL;

    if(pStruct1->a < pStruct2->a)
        return GT_LL_SMALLER;

    return GT_LL_GREATER;
}


GT_STATUS printTestData(STRUCT_LL_TEST *pData)
{
    osPrintf("%d\n",pData->a);
    return GT_OK;
}

void testFree(void *ptr)
{
    cmdOsFree(ptr);
}

GT_STATUS llTest()
{
    STRUCT_LL_HEAD *pHead;
    STRUCT_LL_TEST *pData;
    GT_U32 i;

    pHead = llInit(testCompFunc,GT_TRUE);

    i = 0;

        while(i < 100)
    {
        pData = cmdOsMalloc(sizeof(STRUCT_LL_TEST));

        pData->a = rand() % 10000;

        while(llSearch(pHead,pData) != NULL)
            pData->a = rand() % 10000;

        if(llAdd(pHead,pData) == GT_FAIL)
            osPrintf("ERROR..........\n");
                i++;
                printLL(pHead,printTestData);

    }


    llDestroy(pHead,testFree);

    return GT_OK;
}

#endif /* DEBUG_LL */


