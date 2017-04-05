/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* SersesRxOptAppIf.c
*
* DESCRIPTION:
*       CPSS IF implementation needed by Serdes Optimizer system.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>
#include <serdesOpt/private/gtSkipList.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <serdesOpt/SerdesRxOptimizer.h>
#include <serdesOpt/private/SerdesRxOptAppIf.h>

#define MAX_LANES_NUM 100


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
	return (STRUCT_SL_HEAD *)prvCpssSlInit((FUNCP_SL_COMP)pCompFunc, MAX_LANES_NUM);
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
    INOUT GT_UINTPTR *iteratorId
)
{
	return prvCpssSlGetNext(pSlHead, iteratorId);
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
	return prvCpssSlDestroy(pSlHead, dataFreeFunc);
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
    IN      STRUCT_SL_HEAD  *pSlHead,
    IN      STRUCT_SL_DATA  *pData,
    INOUT   STRUCT_SL_NODE  **newAddedNode
)
{
	static GT_UINTPTR  iter = 0;
	if (newAddedNode != NULL)
	{
		*newAddedNode = 0;
	}

	return prvCpssSlAdd(pSlHead, pData, &iter);	
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
	return prvCpssSlDelete(pSlHead, pData);
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
	return prvCpssSlIsEmpty(pSlHead);
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
    return prvCpssSlGetFirst(pSlHead, pData, pData);
}


