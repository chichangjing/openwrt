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
* gtSkipList.c
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
*           - prototype: GT_SL_COMP_RES compFunc(pStruct1,pStruct2) where
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
*       $Revision: 1 $
*
*******************************************************************************/

#include <common/dataStructures/gtSkipList.h>


/*******************************************************************************
 * Skip-list library semaphore
 ******************************************************************************/
static GT_SEM   skiplistLibSem;
static GT_BOOL  skiplistSemInitialized = GT_FALSE;


/* Forward functions declarations */
static int toss();
static STRUCT_SL_NODE* skipNewNode(STRUCT_SL_DATA *pData,STRUCT_SL_NODE *down,
                                   STRUCT_SL_NODE *next,STRUCT_SL_NODE *prev);
static STRUCT_SL_NODE* skipInsert
(
    INOUT   STRUCT_SL_NODE *pSkipList,
    IN      STRUCT_SL_DATA *pData,
    IN      GT_INTFUNCPTR   compFunc,
    INOUT   STRUCT_SL_NODE  **newAddedNode
);


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
)
{
    static GT_32 semNameId = 0;    /* This is the number to be             */
                                    /* concatinated to the semaphore name.  */
    STRUCT_SL_HEAD *pHead;
    GT_U8  semName[20];

    pHead = (STRUCT_SL_HEAD*)hwsSOptOsMallocFuncPtr(sizeof(STRUCT_SL_HEAD));
    if (pHead == NULL)
    {
        return NULL;
    }

    if((pHead->pFirstNode = skipNewNode(NULL,NULL,NULL,NULL)) == NULL)
    {
        return NULL;
    }

    pHead->compareFunc  =   pCompFunc;

    if(skiplistSemInitialized == GT_FALSE)
    {
        hwsSOptOsSprintfFuncPtr(semName,"SLSem%ld",semNameId);
        if(hwsSOptOsSemBCreateFuncPtr(semName,1,&skiplistLibSem) != GT_OK)
        {
            return NULL;
        }
        skiplistSemInitialized  = GT_TRUE;
    }

    pHead->skiplistSem  = skiplistLibSem;

    semNameId++;

    return pHead;
}


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
)
{
    if((pSlHead->pFirstNode->pDown == NULL) &&
       (pSlHead->pFirstNode->pNext == NULL))
    {
        return GT_TRUE;
    }
    return GT_FALSE;
}


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
)
{
    GT_INTFUNCPTR compFunc;
    STRUCT_SL_NODE *pSkipList;

    hwsSOptOsSemWaitFuncPtr(pSlHead->skiplistSem,OS_WAIT_FOREVER);

    pSkipList = pSlHead->pFirstNode;

    if(pData == NULL)
    {
        while(pSkipList->pDown != NULL)
        {
            pSkipList = pSkipList->pDown;
        }

        hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
        return pSkipList->pNext;
    }

    compFunc = pSlHead->compareFunc;

    while(1)
    {
        if (pSkipList == NULL)
        {
            hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
            return NULL;
        }

        for (;
            ((pSkipList->pNext != NULL) &&
             (compFunc(pSkipList->pNext->pSLData,pData) == GT_SL_SMALLER));
            pSkipList = pSkipList->pNext);

        if (pSkipList->pDown == NULL)
        {
            while (pSkipList->pNext != NULL)
            {
                if (compFunc(pSkipList->pNext->pSLData,pData) == GT_SL_SMALLER)
                {
                    pSkipList = pSkipList->pNext;
                }
                else
                {
                    hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
                    return pSkipList->pNext;
                }
            }
            hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
            return NULL;
        }
        pSkipList=pSkipList->pDown;
    }
}


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
)
{
    STRUCT_SL_NODE *node;

    node = skipGetFirst(pSlHead,pData);
    if(node == NULL)
    {
        return NULL;
    }

    return node->pSLData;
}


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
    INOUT GT_U32 *iteratorId
)
{
    STRUCT_SL_NODE *pNode;

    if(*iteratorId == 0)
    {
        pNode = skipGetFirst(pSlHead,NULL);
    }
    else
    {
        pNode = (STRUCT_SL_NODE*)(*iteratorId);
        pNode = pNode->pNext;
    }

    if(pNode == NULL)
    {
        return NULL;
    }

    *iteratorId = (GT_U32)pNode;

    return pNode->pSLData;
}


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
)
{
    STRUCT_SL_NODE *pNode;

    pNode = skipGetFirst(pSlHead,pData);
    if(pNode == NULL)
    {
        return NULL;
    }

    *iteratorId = (GT_U32)pNode;

    return pNode->pSLData;
}

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
)
{
    STRUCT_SL_NODE **ppSkipList = &(pSlHead->pFirstNode);
    STRUCT_SL_NODE *currSkipList;
    GT_INTFUNCPTR compFunc;

    currSkipList = (*ppSkipList);
    compFunc = pSlHead->compareFunc;

    hwsSOptOsSemWaitFuncPtr(pSlHead->skiplistSem,OS_WAIT_FOREVER);

    while((*ppSkipList)->pDown != NULL)
    {
        (*ppSkipList) = (*ppSkipList)->pDown;
    }

    while(((*ppSkipList)->pNext != NULL) &&
          (compFunc((*ppSkipList)->pNext->pSLData,pData) == GT_SL_SMALLER))
    {
        (*ppSkipList) = (*ppSkipList)->pNext;
    }

    skipAddAfter(pData,(*ppSkipList));
    if(addedNode != NULL)
    {
        *addedNode = (*ppSkipList)->pNext;
    }
    (*ppSkipList) = currSkipList;

    hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
    return GT_OK;
}


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
)
{
    return skipAdd2DownLevelAndGetNode(pSlHead,pData,NULL);
}


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
    IN STRUCT_SL_HEAD *pSlHead,
    IN      STRUCT_SL_DATA  *pData,
    INOUT   STRUCT_SL_NODE  **newAddedNode
)
{
    STRUCT_SL_NODE  **ppSkipList;
    STRUCT_SL_NODE   *pTop; /* a pointer to the node that was added to the  */
                            /* top level, If no such node was added, it     */
                            /* gets NULL.                                   */
    STRUCT_SL_NODE   *pTmp;
    STRUCT_SL_NODE   *currSkipList;
    GT_INTFUNCPTR           compFunc;

    ppSkipList      = &(pSlHead->pFirstNode);
    currSkipList    = (*ppSkipList);

    compFunc = pSlHead->compareFunc;

    /* check if this key already exists. */
    pTmp = skipGetFirst(pSlHead,pData);
    if ((pTmp != NULL) && (compFunc(pData,pTmp->pSLData) == GT_SL_EQUAL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    hwsSOptOsSemWaitFuncPtr(pSlHead->skiplistSem,OS_WAIT_FOREVER);
    pTop = skipInsert((*ppSkipList),pData,compFunc,newAddedNode);

    if((pTop == NULL) || (toss()==0))
    {
        (*ppSkipList) = currSkipList;
        hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
        return GT_OK;
    }

    (*ppSkipList) = skipNewNode(NULL,currSkipList,NULL,NULL);

    if ((*ppSkipList) == NULL)
    {
        hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
    return GT_OK;
}

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
)
{
    return skipAddAndGetNode(pSlHead,pData,NULL);
}


/*******************************************************************************
* skipInsert
*
* DESCRIPTION:
*       inserts a new node to a given skipList.
*       This function returns a pointer to the last added node, which has the
*       data pointed by pData. It works in a recursive way, it first inserts a
*       node in the lowest level, and then goes up (recursively) and inserts a
*       new node to the upper levels in a random way (by a probability of 0.5).
*
* INPUTS:
*       pSkipList   - a pointer to the skipList that the new node should be
*                     added to (recursively).
*       pData       - A pointer to the data of the element to be added.
*       compFunc    - The user data compare function.
*
* OUTPUTS:
*       pSkipList   - a pointer to the new skipList after adding the new data
*
* RETURNS:
*       A pointer to the last node that has been added, OR NULL if there was no
*       added node to the upper level.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static STRUCT_SL_NODE* skipInsert
(
    INOUT STRUCT_SL_NODE *pSkipList,
    IN STRUCT_SL_DATA *pData,
    IN      GT_INTFUNCPTR   compFunc,
    INOUT   STRUCT_SL_NODE  **newAddedNode
)
{
    STRUCT_SL_NODE   *pDeeper;  /* a pointer to the node that was inserted in */
                                /* the last recursive call to skipInsert. If  */
                                /* no such node was added, it gets NULL.      */
    STRUCT_SL_NODE   *pNewNode; /* a pointer to the new added nodes.          */


    for (;
        ((pSkipList->pNext != NULL) &&
         (compFunc(pSkipList->pNext->pSLData,pData) == GT_SL_SMALLER));
        pSkipList = pSkipList->pNext);

    /* check if reached the lowest level of the skipList. */
    if (pSkipList->pDown == NULL)
    {
        pNewNode = skipAddAfter(pData,pSkipList);
        if (pNewNode == NULL)
        {
            return NULL;
        }
        else
        {
            if(newAddedNode != NULL)
            {
                *newAddedNode = pNewNode;
            }
            return pNewNode;
    }
    }

    pDeeper = skipInsert(pSkipList->pDown,pData,compFunc,newAddedNode);
    if ((pDeeper == NULL) || (toss() == 0))
    {
        return NULL;
    }

    /* add a node to this level. */
    pNewNode = skipAddAfter(pData,pSkipList);
    if (pNewNode == NULL)
    {
        return NULL;
    }

    pNewNode->pDown = pDeeper;

    return pNewNode;
}


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
)
{
    STRUCT_SL_NODE    **ppSkipList;
    STRUCT_SL_NODE    *currSkipList;    /* a temporary pointer that holds the */
                                        /* SkipList for further checks at the */
                                        /* end of the function.               */
    STRUCT_SL_NODE    *pDelNode;        /* a pointer to the node which should */
                                        /* be deleted in order to free its    */
                                        /* memory.                            */
    GT_BOOL           entryFound;       /* Indicates of the entry was found   */
                                        /* in the skip-list.                  */
    STRUCT_SL_DATA    *pRetVal = NULL;
    GT_INTFUNCPTR compFunc;
    ppSkipList      = &(pSlHead->pFirstNode);
    currSkipList    = (*ppSkipList);
    compFunc        = pSlHead->compareFunc;

    entryFound = GT_FALSE;

    hwsSOptOsSemWaitFuncPtr(pSlHead->skiplistSem,OS_WAIT_FOREVER);

    while ((*ppSkipList) != NULL)
    {
        for (;
            (((*ppSkipList)->pNext != NULL) &&
             (compFunc((*ppSkipList)->pNext->pSLData,pData) == GT_SL_SMALLER));
            (*ppSkipList) = (*ppSkipList)->pNext);

        if(((*ppSkipList)->pNext != NULL) &&
           (compFunc((*ppSkipList)->pNext->pSLData,pData)) == GT_SL_EQUAL)
        {
            pDelNode = (*ppSkipList)->pNext;
            pRetVal = (*ppSkipList)->pNext->pSLData;
            (*ppSkipList)->pNext = (*ppSkipList)->pNext->pNext;
            if ((*ppSkipList)->pNext != NULL)
            {
                (*ppSkipList)->pNext->pPrev = (*ppSkipList);
            }
            hwsSOptOsFreeFuncPtr(pDelNode);

            entryFound = GT_TRUE;
        }

        (*ppSkipList) = (*ppSkipList)->pDown;
    }

    /* check if the highest level of the SkipList has no nodes (except of the */
    /* header) & it's not the last node in the structure, If yes remove the   */
    /* highest level and return the new SkipList.                             */
    if(entryFound == GT_TRUE)
    {
    while ((currSkipList->pNext == NULL) && (currSkipList->pDown != NULL))
    {
        pDelNode = currSkipList;
        currSkipList = currSkipList->pDown;
        hwsSOptOsFreeFuncPtr(pDelNode);
    }
    }
    (*ppSkipList) = currSkipList;
    hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
    return pRetVal;
}


/*******************************************************************************
* skipNewNode
*
* DESCRIPTION:
*       Creates a new node.
*
*       This function returns a pointer to a new node that has the following
*       values:
*       -   slData= pData
*       -   pNext = next
*       -   pPrev = prev
*       -   pDown = down
*
* INPUTS:
*       pData   - A pointer to the data in the created node.
*       next    - a pointer to node that the new node should point to by pNext.
*       prev    - a pointer to node that the new node should point to by pPrev.
*       down    - a pointer to node that the new node should point to by pDown.
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
static STRUCT_SL_NODE* skipNewNode
(
    IN STRUCT_SL_DATA *pData,
    IN STRUCT_SL_NODE *down,
    IN STRUCT_SL_NODE *next,
    IN STRUCT_SL_NODE *prev
)
{
    STRUCT_SL_NODE*   pNewNode;   /* pointer to the new added node. */

    pNewNode = (STRUCT_SL_NODE*)hwsSOptOsMallocFuncPtr(sizeof(STRUCT_SL_NODE));
    if (pNewNode == NULL)
    {
        return NULL;
    }

    pNewNode->pSLData = pData;
    pNewNode->pDown = down;
    pNewNode->pNext = next;
    pNewNode->pPrev = prev;

    return pNewNode;
}


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
)
{
    STRUCT_SL_NODE*   pTmpNode;      /* the node to be added.*/

    pTmpNode = skipNewNode(pData,NULL,pNode->pNext,pNode);
    if(pTmpNode == NULL)
        return NULL;

    if(pNode->pNext != NULL)
        pNode->pNext->pPrev = pTmpNode;

    pNode->pNext = pTmpNode;

    return pTmpNode;
}


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
)
{
    STRUCT_SL_NODE *pSkipList;
    GT_INTFUNCPTR compFunc;

    pSkipList = pSlHead->pFirstNode;

    if(pData == NULL)
    {
        return NULL;
    }

    hwsSOptOsSemWaitFuncPtr(pSlHead->skiplistSem,OS_WAIT_FOREVER);

    compFunc = pSlHead->compareFunc;

    while(1)
    {
        if (pSkipList == NULL)
        {
            hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
            return NULL;
        }

        for (;
            ((pSkipList->pNext != NULL) &&
             (compFunc(pSkipList->pNext->pSLData,pData) == GT_SL_SMALLER));
            pSkipList = pSkipList->pNext);

        if(pSkipList->pDown == NULL)
        {
            while(pSkipList->pNext != NULL)
            {
                if(compFunc(pSkipList->pNext->pSLData,pData) == GT_SL_EQUAL)
                {
                    hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
                    return pSkipList->pNext->pSLData;
                }

                if(compFunc(pSkipList->pNext->pSLData,pData) == GT_SL_SMALLER)
                    pSkipList = pSkipList->pNext;
                else if(compFunc(pSkipList->pNext->pSLData,pData) ==
                        GT_SL_GREATER)
                {
                    hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
                    return NULL;
                }
            }
            hwsSOptOsSemSignalFuncPtr(pSlHead->skiplistSem);
            return NULL;
        }
        pSkipList = pSkipList->pDown;
    }
}



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
)
{
    STRUCT_SL_DATA *pDelData;

    while((pDelData = skipGetFirstData(pSlHead,NULL)) != NULL)
    {
        pDelData = skipDelete(pSlHead,pDelData);
        if(dataFreeFunc != NULL)
            dataFreeFunc(pDelData);
    }

    hwsSOptOsFreeFuncPtr(pSlHead->pFirstNode);

    /*osSemDelete(pSlHead->skiplistSem);*/
    hwsSOptOsFreeFuncPtr(pSlHead);

    return GT_OK;
}



/*******************************************************************************
* toss
*
* DESCRIPTION:
*       returns 0 or 1 in equal probability.
*
* INPUTS:
*       param1 - ...
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0 or 1.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static int toss()
{
    return (hwsSOptOsRandFuncPtr() % 2);     /* return 0 OR 1. (each by probability 0.5) */
}



#undef DEBUG_SL
#ifdef DEBUG_SL

void skipPrint
(
    IN STRUCT_SL_HEAD *pSlHead,
    IN GT_INTFUNCPTR  printFunc
)
{
    STRUCT_SL_NODE *slNode;
    slNode = pSlHead->pFirstNode;

    while(slNode->pDown != NULL)
        slNode = slNode->pDown;

    slNode = slNode->pNext;

    while(slNode != NULL)
    {
        printFunc(slNode->pSLData);
        slNode = slNode->pNext;
    }

    return;
}


typedef struct
{
    GT_U32 a;
}STRUCT_SL_TEST;


GT_SL_COMP_RES slTestCompFunc(STRUCT_SL_TEST *pStruct1,STRUCT_SL_TEST *pStruct2)
{
    if((pStruct1 == NULL) && (pStruct2 == NULL))
        return GT_SL_EQUAL;
    if((pStruct1 == NULL) && (pStruct2 != NULL))
        return GT_SL_SMALLER;
    if((pStruct1 != NULL) && (pStruct2 == NULL))
        return GT_SL_GREATER;

    if(pStruct1->a == pStruct2->a)
        return GT_SL_EQUAL;

    if(pStruct1->a < pStruct2->a)
        return GT_SL_SMALLER;

    return GT_SL_GREATER;
}


GT_STATUS slPrintTestData(STRUCT_SL_TEST *pData)
{
    osPrintf("%d\n",pData->a);
    return GT_OK;
}

void slTestFree(void *ptr)
{
    osFree(ptr);
}


GT_STATUS skipTest()
{
    STRUCT_SL_HEAD *pHead;
    STRUCT_SL_TEST *pData;
    GT_U32 i;

    pHead = skipInit(slTestCompFunc,GT_TRUE);

    i = 0;

    while(i < 100)
    {
        pData = osMalloc(sizeof(STRUCT_SL_TEST));

        pData->a = rand() % 10000;

        while(skipSearch(pHead,pData) != NULL)
            pData->a = rand() % 10000;

        if(skipAdd(pHead,pData) == GT_FAIL)
            osPrintf("ERROR..........\n");
        i++;
        skipPrint(pHead,slPrintTestData);

    }

    skipDestroy(pHead,slTestFree);

    return GT_OK;
}

#endif /* DEBUG_SL */
