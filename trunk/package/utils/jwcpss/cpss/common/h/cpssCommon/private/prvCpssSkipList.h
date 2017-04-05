/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssSkipList.h
*
* DESCRIPTION:
*       This file includes a skip-list implementation, it provides the following
*       functions for the user:
*       - prvCpssSlInit: initializes a new skiplist, the function recieves a
*         pointer to the comprison function (will be described later) and the
*         skip list maximum size. Returns a pointer to the new skip-list.
*
*       - prvCpssSlIsEmpty(ctrlPtr): returns GT_TRUE if the skip-list pointed by
*         ctrlPtr is an empty list.
*
*       - prvCpssSlSearch(ctrlPtr, dataPtr): searches for a given key in the
*         skip-list, and returns it if found or NULL otherwise.
*
*       - prvCpssSlGetFirst(ctrlPtr, dataPtr): returns the data stored in the
*         node which is returned from skipGetFirst().
*         NULL in the dataPtr field returns the first data in the skip-list.
*
*       - prvCpssSlGetNext(ctrlPtr, iteratorId): Iterator function, returns the
*         next data in the skip-list, according to the input interatorId, or
*         NULL if no nodes left.
*
*       - prvCpssSlGetPrev(ctrlPtr, iteratorId): Iterator function, returns the
*         previous data in the skip-list, according to the input interatorId, or
*         NULL if no nodes left.
*
*       - prvCpssSlAdd(ctrlPtr, dataPtr): inserts a new node, containing
*         dataPtr, to the skip-list, this pointer is to a memory area which is
*         already allocated in memory.
*
*       - prvCpssSlDelete(ctrlPtr, dataPtr): deletes the skip-list node that
*         contains the data dataPtr, and returns a pointer to the Node-Data.
*
*       - prvCpssSlDestroy(ctrlPtr, dataFreeFunc): Destroys the given skip-list,
*         and frees the user data by calling dataFreeFun with the user stored
*         data as a parameter.
*
*       - Requirements from the compare function:
*           - prototype: GT_COMP_RES compFunc(struct1Ptr, struct2Ptr) where
*             structXPtr is a pointer to the data structures stored in the
*             skip-list.
*           - functionality:
*             {
*                 if(struct1Ptr == struct2Ptr)
*                     return GT_EQUAL;
*
*                 if (struct1Ptr < struct2Ptr)
*                     return GT_SMALLER;
*
*                 return GT_GREATER;
*              }
*       Where the comparison operators are defined according to the user's needs.
*
*       In order that the user will be able to use the above functionality, the
*       file "prvCpssSkipList.h" must be included.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#ifndef __prvCpssSkipListh
#define __prvCpssSkipListh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/* Values to be returned by the skip-list compare function */
typedef GT_COMP_RES (*FUNCP_SL_COMP)(GT_VOID *elem1Ptr, GT_VOID *elem2Ptr);

#define GT_SL_MAXELEM   0x100000  /* Default value for slinit */


/*******************************************************************************
* prvCpssSlHitlessUpgrdActivate
*
* DESCRIPTION:
*       Activates the skip list module
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - success
*       GT_FAIL - otherwise
*
* COMMENTS:
*       Needed for hitless upgrade first phase only.
*       Internal use only.
*
*******************************************************************************/
GT_STATUS prvCpssSlHitlessUpgrdActivate(void);


/*******************************************************************************
* prvCpssSlInit
*
* DESCRIPTION:
*       Initializes a new skipList.
*
* INPUTS:
*       pCompFunc   - A pointer to the SL comparison function.
*       maxElements - The maximum number of elements in the SL.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       A pointer to the new skipList, or NULL if memory allocation failed.
*
* COMMENTS:
*       1) The maxElements should match the 2^n size for best performance.
*
*******************************************************************************/
GT_VOID * prvCpssSlInit
(
    IN FUNCP_SL_COMP    compFuncPtr,
    IN GT_U32           maxElementNumber
);


/*******************************************************************************
* prvCpssSlIsEmpty
*
* DESCRIPTION:
*       checks if a skip-List is empty.
*
* INPUTS:
*       ctrlPtr - A pointer to the skipList control block.
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
GT_BOOL prvCpssSlIsEmpty
(
    IN GT_VOID          *ctrlPtr
);


/*******************************************************************************
* prvCpssSlGetFirst
*
* DESCRIPTION:
*       search for a given key in the skip list.
*       This function returns a pointer to the data stored in the first node
*       with key >= dataPtr, OR NULL if there is no such node.
*
* INPUTS:
*       ctrlPtr     - A pointer to the skipList control block.
*       dataPtr     - The key of the element to be searched.
*       iteratorPtr - The iteration Id to be sent in the next call to this
*                     function to get the next data.
*
* OUTPUTS:
*       iteratorPtr - The iteratorPtr to be sent in the next call to this
*                     function to get the next data.
*
* RETURNS:
*       A pointer to the first node with data >= dataPtr, OR NULL if no such node
*       exists.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID * prvCpssSlGetFirst
(
    IN GT_VOID          *ctrlPtr,
    IN GT_VOID          *dataPtr,
    INOUT GT_UINTPTR    *iteratorPtr
);


/*******************************************************************************
* prvCpssSlGetNext
*
* DESCRIPTION:
*       This function is used for iterations over the skip-list.
*
* INPUTS:
*       ctrlPtr     - A pointer to the skipList control block.
*       iteratorPtr - The iterator Id that was returned from the last call to
*                     this function.
*
* OUTPUTS:
*       iteratorPtr - The iteration Id to be sent in the next call to this
*                     function to get the next data.
*
* RETURNS:
*       A pointer to the next node data if exists, NULL otherwise.
*
* COMMENTS:
*       1.  If (*iteratorPtr == 0): the first node in the skip-list is returned.
*       2.  The new value of iterator Id should be sent in the next call to
*           this function to get the next data.
*       3.  When using the iterator, user application MUST lock (semaphore) the
*           section of code that uses the iterator as input (getNext,GetPrev).
*
*******************************************************************************/
GT_VOID * prvCpssSlGetNext
(
    IN GT_VOID          *ctrlPtr,
    INOUT GT_UINTPTR    *iteratorPtr
);


/*******************************************************************************
* prvCpssSlGetPrev
*
* DESCRIPTION:
*       This function is used for iterations over the skip-list.
*
* INPUTS:
*       ctrlPtr     - A pointer to the skipList control block.
*       iteratorPtr - The iterator Id that was returned from the last call to
*                     this function.
*
* OUTPUTS:
*       iteratorPtr - The iteration Id to be sent in the next call to this
*                     function to get the next data.
*
* RETURNS:
*       A pointer to the next node data if exists, NULL otherwise.
*
* COMMENTS:
*       1.  When using the iterator, user application MUST lock (semaphore) the
*           section of code that uses the iterator as input (getNext,GetPrev).
*
*******************************************************************************/
GT_VOID * prvCpssSlGetPrev
(
    IN GT_VOID          *ctrlPtr,
    INOUT GT_UINTPTR    *iteratorPtr
);


/*******************************************************************************
* prvCpssSlAdd
*
* DESCRIPTION:
*       Adds a new node to the skipList.
*       This function returns a pointer to the new skipList, containing the data
*       dataPtr. It also adds a new level to the skipList if a new node was added
*       to the top level (by probability of 0.5).
*
* INPUTS:
*       ctrlPtr     - A pointer to the skipList control block.
*       dataPtr     - A pointer to the new data to be added.
*
* OUTPUTS:
*       iteratorPtr - The iteration Id to be sent in the next call to this
*                     function to get the next data.
*
* RETURNS:
*       GT_OK - if succeeded, or
*       GT_ALREADY_EXIST - if the node with the key already exists
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssSlAdd
(
    IN GT_VOID          *ctrlPtr,
    IN GT_VOID          *dataPtr,
    OUT GT_UINTPTR      *iteratorPtr
);

/*******************************************************************************
* prvCpssSlGet
*
* DESCRIPTION:
*       This function is used for iterations over the skip-list.
*
* INPUTS:
*       ctrlPtr     - A pointer to the skipList control block.
*       iteratorId  - The iterator Id that describes the required data.
*
* OUTPUTS:
*
* RETURNS:
*       A pointer to the node data if exists, NULL otherwise.
*
* COMMENTS:
*       Use only iteratorId that was returned by slAdd.
*
*******************************************************************************/
GT_VOID * prvCpssSlGet
(
    IN GT_VOID * ctrlPtr,
    IN GT_UINTPTR  iteratorId
);

/*******************************************************************************
* prvCpssSlDelete
*
* DESCRIPTION:
*       Deletes a node from the skipList.
*
* INPUTS:
*       ctrlPtr - A pointer to the skipList control block.
*       dataPtr - A pointer to the data of the node to be deleted.
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
GT_VOID * prvCpssSlDelete
(
    IN GT_VOID          *ctrlPtr,
    IN GT_VOID          *dataPtr
);


/*******************************************************************************
* prvCpssSlSearch
*
* DESCRIPTION:
*       This function searches for a given key in the skip-list.
*
* INPUTS:
*       ctrlPtr - A pointer to the skipList control block.
*       dataPtr - A pointer to the data to be searched.
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
GT_VOID * prvCpssSlSearch
(
    IN GT_VOID          *ctrlPtr,
    IN GT_VOID          *dataPtr
);


/*******************************************************************************
* prvCpssSlDestroy
*
* DESCRIPTION:
*       Destroys the given skip-list.
*
* INPUTS:
*       ctrlPtr     - A pointer to the skipList control block.
*       dataFreeFunc- A pointer to a function to be called for deleteing the
*                     user data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssSlDestroy
(
    IN GT_VOID          *ctrlPtr,
    IN GT_VOIDFUNCPTR   dataFreeFunc
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssSkipListh */


