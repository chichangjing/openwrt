/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssSkipList.c
*
* DESCRIPTION:
*       This file includes a skip-list (refered as SL) implementation, it
*       provides the following functions for the user: prvCpssSlInit,
*       prvCpssSlIsEmpty, prvCpssSlGetFirst, prvCpssSlGetNext, prvCpssSlGetPrev,
*       prvCpssSlAdd, prvCpssSlDelete, prvCpssSlSearch and prvCpssSlDestroy.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/cpssPresteraDefs.h>

#ifdef SL_DEBUG
#define DBG_INFO(x) cpssOsPrintf x
#else
#define DBG_INFO(x)
#endif

#define SL_END (SL_NODE_STC*)slCtrlPtr->hdrPtr

/*
 * Typedef: struct SL_NODE_STCT
 *
 * Description: Includes information
 *
 *
 * Fields:
 *      dataPtr         - key used for searching
 *      prevPtr         - skip list previous node pointer
 *      fwrdArr         - skip list forward pointer table
 */
typedef struct SL_NODE_STCT
{
    GT_VOID             *dataPtr;
    struct SL_NODE_STCT *prevPtr;
    struct SL_NODE_STCT *fwrdArr[1];
} SL_NODE_STC;


/*
 * Typedef: struct SL_CTRL_STCT
 *
 * Description: Includes information
 *
 *
 * Fields:
 *      compareFunc     - skip-list nodes comparison function
 *      maxLevel        - maximum skip-list level
 *      currLevel       - the current skip-list level
 *      hdrPtr          - skip-list header
 *      updateArr       - update pointer array
 */
typedef struct SL_CTRL_STCT
{
    FUNCP_SL_COMP       compareFunc;
    GT_32               maxLevel;
    GT_32               currLevel;
    SL_NODE_STC         *hdrPtr;
    SL_NODE_STC         *updateArr[1];
} SL_CTRL_STC;

static GT_BOOL initDone = GT_FALSE;

/*
 *  Additive random number generator
 *
 *  Modelled after "Algorithm A" in
 *  Knuth, D. E. (1981). The art of computer programming, volume 2, page 27.
 *
 */
#define FRONT_OFFSET    12
#define STATE_TBL_SIZE  33

static GT_U32 state[STATE_TBL_SIZE] = {
    0xd53f1852, 0xdfc78b83, 0x4f256096, 0x0e643df7, 0x82c359bf, 0xc7794dfa,
    0xd5e9ffaa, 0x2c8cb64a, 0x2f07b334, 0xad5a7eb5, 0x96dc0cde, 0x6fc24589,
    0xa5853646, 0xe71576e2, 0x0dae30df, 0xb09ce711, 0x5e56ef87, 0x4b4b0082,
    0x6f4f340e, 0xc5bb17e8, 0xd788d765, 0x67498087, 0x9d7aba26, 0x261351d4,
    0x411ee7ea, 0x0393a263, 0x2c5a5835, 0xc115fcd8, 0x25e9132c, 0xd0c6e906,
    0xc2bc5b2d, 0x6c065c98, 0x6e37bd55 };

static GT_U32   *frontPtr = &state[FRONT_OFFSET];
static GT_U32   *rearPtr  = &state[STATE_TBL_SIZE-1];

/*******************************************************************************
* random
*
* DESCRIPTION:
*       Returns a 32 bit random number.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       return value in the range 0 <= x <= 2**32 - 1
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 random
(
    IN  GT_VOID
)
{
    GT_U32  randVal;

    /* set the random value from fron and rear pointers */
    randVal = *rearPtr;

    randVal ^= *frontPtr--;

    *rearPtr-- = randVal;

    /* treat overflow of front/rear pointers */
    if (rearPtr < state)
    {
        rearPtr = &state[STATE_TBL_SIZE-1];
    }
    else
    {
        if (frontPtr < state)
        {
            frontPtr = &state[STATE_TBL_SIZE-1];
        }
    }
    return randVal;
}


/*******************************************************************************
* sRandom
*
* DESCRIPTION:
*       Returns a 32 bit random number.
*
* INPUTS:
*       seed    - the seed that is used to create the linear congruential table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID sRandom
(
    IN GT_U32   seed
)
{
    GT_U32  i;

    state[0] = seed;

    /* linear congruential initializer */
    for (i = 1; i < STATE_TBL_SIZE; ++i)
    {
        state[i] = 1103515245 * state[i-1] + 12345;
    }

    /* set the front and read pointers */
    frontPtr = &state[FRONT_OFFSET];
    rearPtr  = &state[STATE_TBL_SIZE-1];

    /* overwrite the random numbers with new */
    for (i=0; i < 10*STATE_TBL_SIZE; ++i)
    {
        random();
    }
}




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
GT_STATUS prvCpssSlHitlessUpgrdActivate(void)
{
    return GT_OK;
}



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
    IN GT_U32           maxElements
)
{
    SL_CTRL_STC         *slCtrlPtr;             /* SL control pointer       */
    GT_32               i;                      /* iterator                 */
    GT_32               maxLevel;               /* maximum level            */

    /* validate the input params */
    if (NULL == compFuncPtr || 0 == maxElements)
    {
        DBG_INFO(("Params error, line=%d\n", __LINE__));
        return NULL;
    }

    /* find the maximum level for the Skip-List from the maxElementNumber.  */
    /* Levels range from (0 .. n) for 2^n+1 elements.                       */
    for (maxLevel = 0; maxElements > 2 ; maxLevel++)
    {
        maxElements >>= 1;
    }

    /* allocate memory for the control structure */
    slCtrlPtr = cpssOsMalloc(sizeof(SL_CTRL_STC) +
                             maxLevel * sizeof(SL_NODE_STC *));

    if (NULL == slCtrlPtr)
    {
        DBG_INFO(("insufficient memory, line=%d\n", __LINE__));
        return NULL;
    }

    /* allocate memory for the Skip-List header */
    slCtrlPtr->hdrPtr = cpssOsMalloc(sizeof(SL_NODE_STC) +
                                     maxLevel * sizeof(SL_NODE_STC *));

    if (NULL == slCtrlPtr->hdrPtr)
    {
        cpssOsFree(slCtrlPtr);
        DBG_INFO(("insufficient memory, line=%d\n", __LINE__));
        return NULL;
    }

    /* set all forward pointers of the header node */
    for (i = 0; i <= maxLevel; i++)
    {
        slCtrlPtr->hdrPtr->fwrdArr[i] = SL_END;
    }

    /* set the previous pointer of the header node */
    slCtrlPtr->hdrPtr->prevPtr = SL_END;

    /* set control values */
    slCtrlPtr->currLevel       = 0;
    slCtrlPtr->maxLevel        = maxLevel;
    slCtrlPtr->compareFunc     = compFuncPtr;

    if (initDone == GT_FALSE)
    {
        initDone = GT_TRUE;

        /* seed the random generator */
        sRandom(0x61E9B4E8);
    }

    DBG_INFO(("%p = slInit(%d)\n", slCtrlPtr, maxLevel));

    return (GT_VOID*)slCtrlPtr;
}


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
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer             */

    /* validate the input params */
    if (NULL == ctrlPtr)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        return GT_TRUE;
    }

    slCtrlPtr = (SL_CTRL_STC*)ctrlPtr;

    if (slCtrlPtr->hdrPtr->fwrdArr[0] == SL_END)
    {
        return GT_TRUE;
    }
    return GT_FALSE;
}


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
    OUT GT_UINTPTR      *iteratorPtr
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer             */
    SL_NODE_STC         *nodePtr;         /* SL node pointer                */
    FUNCP_SL_COMP       compFunc;         /* user compare routine           */
    GT_32               i;                /* iterator                       */

    /* validate the input params */
    if (NULL == ctrlPtr || NULL == iteratorPtr)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        return NULL;
    }

    slCtrlPtr = (SL_CTRL_STC*)ctrlPtr;
    compFunc  = slCtrlPtr->compareFunc;
    nodePtr   = slCtrlPtr->hdrPtr;

    if (NULL == dataPtr)
    {
        if (slCtrlPtr->hdrPtr->fwrdArr[0] == SL_END)
        {
            /* the SL is empty */
            return NULL;
        }
        else
        {
            /* non empty SL, return the first node */
            *iteratorPtr = (GT_UINTPTR)slCtrlPtr->hdrPtr->fwrdArr[0];
            return slCtrlPtr->hdrPtr->fwrdArr[0]->dataPtr;
        }
    }

    /* search for the first node that it`s key is smaller than the user key */
    for (i = slCtrlPtr->currLevel; i >= 0; i--)
    {
        while (nodePtr->fwrdArr[i] != SL_END &&
               compFunc(nodePtr->fwrdArr[i]->dataPtr, dataPtr) == GT_SMALLER)
        {
            nodePtr = nodePtr->fwrdArr[i];
        }
    }

    nodePtr = nodePtr->fwrdArr[0];

    /* did we reach the end of SL */
    if (nodePtr == SL_END)
    {
        return NULL;
    }

    /* if we got here, then we found the SL->dataPtr >= dataPtr */
    *iteratorPtr = (GT_UINTPTR)nodePtr;

    return nodePtr->dataPtr;
}


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
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer             */
    SL_NODE_STC         *nodePtr;         /* SL node pointer                */

    /* validate the input params */
    if (NULL == ctrlPtr || NULL == iteratorPtr)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        return NULL;
    }

    slCtrlPtr = (SL_CTRL_STC*)ctrlPtr;

    if (0 == *iteratorPtr)
    {
        return prvCpssSlGetFirst(ctrlPtr, NULL, iteratorPtr);
    }

    /* set the nodePtr according to the iteratorPtr */
    nodePtr = (SL_NODE_STC*)(*iteratorPtr);
    nodePtr = nodePtr->fwrdArr[0];

    /* did we reach the end of SL */
    if (nodePtr == SL_END)
    {
        return NULL;
    }

    /* update user iterator */
    *iteratorPtr = (GT_UINTPTR)nodePtr;

    return nodePtr->dataPtr;
}


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
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer            */
    SL_NODE_STC         *nodePtr;         /* SL node pointer               */

    /* validate the input params */
    if (NULL == ctrlPtr || NULL == iteratorPtr)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        return NULL;
    }

    slCtrlPtr = (SL_CTRL_STC*)ctrlPtr;

    if (0 == *iteratorPtr)
    {
        return NULL;
    }

    /* set the nodePtr according to the iteratorPtr */
    nodePtr = (SL_NODE_STC*)(*iteratorPtr);
    nodePtr = nodePtr->prevPtr;

    /* did we reach the end of SL */
    if (nodePtr == SL_END)
    {
        return NULL;
    }

    /* update user iterator */
    *iteratorPtr = (GT_UINTPTR)nodePtr;

    return nodePtr->dataPtr;
}


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
*       GT_OK if succeeded, or
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
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer             */
    SL_NODE_STC         *nodePtr;         /* SL node pointer                */
    FUNCP_SL_COMP       compFunc;         /* user compare routine           */
    GT_32               i;                /* iterator                       */
    GT_32               newLevel;         /* new level to insert new node   */
    GT_U32              randVal;          /* a random 32 bit value          */

    /* validate the input params */
    if (NULL == ctrlPtr || NULL == dataPtr || NULL == iteratorPtr)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    slCtrlPtr    = (SL_CTRL_STC*)ctrlPtr;
    compFunc     = slCtrlPtr->compareFunc;
    nodePtr      = slCtrlPtr->hdrPtr;
    *iteratorPtr = 0; /* in case of error */

    /* find where key belongs, search for the first node that   */
    /* it`s key is smaller than the user key                    */
    for (i = slCtrlPtr->currLevel; i >= 0; i--)
    {
        while (nodePtr->fwrdArr[i] != SL_END &&
               compFunc(nodePtr->fwrdArr[i]->dataPtr, dataPtr) == GT_SMALLER)
        {
            nodePtr = nodePtr->fwrdArr[i];
        }
        slCtrlPtr->updateArr[i] = nodePtr;
    }

    nodePtr = nodePtr->fwrdArr[0];

    /* key allready exists in SL */
    if (nodePtr != SL_END &&
        compFunc(nodePtr->dataPtr, dataPtr) == GT_EQUAL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    /* get a 32 bit random number from generator */
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    /* use const value as this value impact 'newLevel' that is used for
       allocation size for 'nodePtr'
    */
    randVal = 0x6A5BC4E9;
#else /* ! OS_MALLOC_MEMORY_LEAKAGE_DBG */
    randVal = random();
#endif /* ! OS_MALLOC_MEMORY_LEAKAGE_DBG */

    /* determine level for insertion */
    for (newLevel = 0;
         (randVal & 1) && newLevel < slCtrlPtr->maxLevel;
         newLevel++)
    {
        /* move to the next random bit */
        randVal >>= 1;
    }

    /* create a new level in SL */
    if (newLevel > slCtrlPtr->currLevel)
    {
        for (i = slCtrlPtr->currLevel + 1; i <= newLevel; i++)
        {
            slCtrlPtr->updateArr[i] = slCtrlPtr->hdrPtr;
        }
        slCtrlPtr->currLevel = newLevel;
    }

    /* allocate memory for the new node */
    nodePtr = cpssOsMalloc(sizeof(SL_NODE_STC) + newLevel * sizeof(SL_NODE_STC *));

    if (NULL == nodePtr)
    {
        DBG_INFO(("cpssOsMalloc err, line=%d\n", __LINE__));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* set user key and update the iterator */
    nodePtr->dataPtr = dataPtr;
    *iteratorPtr = (GT_UINTPTR)nodePtr;

    /* update forward links */
    for (i = 0; i <= newLevel; i++)
    {
        nodePtr->fwrdArr[i] = slCtrlPtr->updateArr[i]->fwrdArr[i];
        slCtrlPtr->updateArr[i]->fwrdArr[i] = nodePtr;
    }

    /* update the previous pointer for the new node */
    nodePtr->prevPtr = nodePtr->fwrdArr[0]->prevPtr;

    /* and for the next node */
    nodePtr->fwrdArr[0]->prevPtr = nodePtr;

    return GT_OK;
}

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
    IN GT_UINTPTR iteratorId
)
{
    SL_CTRL_STC * slCtrlPtr;       /* SL control pointer             */
    SL_NODE_STC * nodePtr;         /* SL node pointer                */

    /* validate the input params */
    if (NULL == ctrlPtr)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        return NULL;
    }


    if (0 == iteratorId)
    {
        return NULL;
    }

    slCtrlPtr = (SL_CTRL_STC*)ctrlPtr;

    /* set the nodePtr according to the iteratorId */
    nodePtr = (SL_NODE_STC *)(iteratorId);

    /* If reached the end of the SL (to be on the safe side) */
    if (nodePtr == SL_END)
    {
        return NULL;
    }

    return nodePtr->dataPtr;
} /* slGet */


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
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer             */
    SL_NODE_STC         *nodePtr;         /* SL node pointer                */
    GT_VOID             *retPtr;          /* deleted node user data ptr     */
    FUNCP_SL_COMP       compFunc;         /* user compare routine           */
    GT_32               i;                /* iterator                       */

    /* validate the input params */
    if (NULL == ctrlPtr || NULL == dataPtr)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        return NULL;
    }

    slCtrlPtr = (SL_CTRL_STC*)ctrlPtr;
    compFunc  = slCtrlPtr->compareFunc;
    nodePtr   = slCtrlPtr->hdrPtr;

    /* find where data belongs */

    /* search for the first node that it`s key is smaller than the user key */
    for (i = slCtrlPtr->currLevel; i >= 0; i--)
    {
        while (nodePtr->fwrdArr[i] != SL_END &&
               compFunc(nodePtr->fwrdArr[i]->dataPtr, dataPtr) == GT_SMALLER)
        {
            nodePtr = nodePtr->fwrdArr[i];
        }
        slCtrlPtr->updateArr[i] = nodePtr;
    }

    nodePtr = nodePtr->fwrdArr[0];

    /* key not found */
    if (nodePtr == SL_END ||
        compFunc(nodePtr->dataPtr, dataPtr) != GT_EQUAL)
    {
        return NULL;
    }

    /* adjust forward pointers */
    for (i = 0; i <= slCtrlPtr->currLevel; i++)
    {
        if (slCtrlPtr->updateArr[i]->fwrdArr[i] != nodePtr)
            break;

        slCtrlPtr->updateArr[i]->fwrdArr[i] = nodePtr->fwrdArr[i];
    }

    /* update the previous pointer */
    nodePtr->fwrdArr[0]->prevPtr = nodePtr->prevPtr;

    retPtr = nodePtr->dataPtr;

    cpssOsFree(nodePtr);

    /* adjust header level */
    while (slCtrlPtr->currLevel > 0 &&
           slCtrlPtr->hdrPtr->fwrdArr[slCtrlPtr->currLevel] == SL_END)
    {
        slCtrlPtr->currLevel--;
    }

    return retPtr;
}


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
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer             */
    SL_NODE_STC         *nodePtr;         /* SL node pointer                */
    FUNCP_SL_COMP       compFunc;         /* user compare routine           */
    GT_32               i;                /* iterator                       */

    /* validate the input params */
    if (NULL == ctrlPtr || NULL == dataPtr)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[return_null] */
        return NULL;
    }

    slCtrlPtr = (SL_CTRL_STC*)ctrlPtr;
    compFunc  = slCtrlPtr->compareFunc;
    nodePtr   = slCtrlPtr->hdrPtr;

    /* search for the first node that it`s key is smaller than the user key */
    for (i = slCtrlPtr->currLevel; i >= 0; i--)
    {
        while (nodePtr->fwrdArr[i] != SL_END &&
               compFunc(nodePtr->fwrdArr[i]->dataPtr, dataPtr) == GT_SMALLER)
        {
            nodePtr = nodePtr->fwrdArr[i];
        }
    }

    nodePtr = nodePtr->fwrdArr[0];

    if (nodePtr != SL_END &&
        compFunc(nodePtr->dataPtr, dataPtr) == GT_EQUAL)
    {
        /* this key is not the searched one */
        return nodePtr->dataPtr;
    }

    return NULL;
}


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
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer             */
    SL_NODE_STC         *nodePtr;         /* SL node pointer                */
    SL_NODE_STC         *nextNodePtr;     /* SL next node pointer           */

    /* validate the input params */
    if (NULL == ctrlPtr || NULL == dataFreeFunc)
    {
        DBG_INFO(("invalid input ptr, line=%d\n", __LINE__));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    slCtrlPtr   = (SL_CTRL_STC*)ctrlPtr;
    nodePtr     = slCtrlPtr->hdrPtr;

    /* free all nodes` and user allocated memory */
    if (nodePtr->fwrdArr[0] != SL_END)
    {
        nextNodePtr = nodePtr->fwrdArr[0];

        nodePtr = nextNodePtr;

        while (nodePtr != SL_END)
        {
            if (NULL != nodePtr->dataPtr)
            {
                dataFreeFunc(nodePtr->dataPtr);
            }

            nextNodePtr = nodePtr->fwrdArr[0];

            cpssOsFree(nodePtr);

            nodePtr = nextNodePtr;
        }
    }

    /* free the SL header memory */
    cpssOsFree(slCtrlPtr->hdrPtr);

    /* free the SL control memory */
    cpssOsFree(slCtrlPtr);

    return GT_OK;
}






/******************************************************************************/
/*************************       DEBUG CODE      ******************************/
/******************************************************************************/

#ifdef SL_DEBUG

/* print Skip-List nodes */
GT_STATUS debugSlPrint
(
    IN GT_VOID          *ctrlPtr
)
{
    SL_CTRL_STC         *slCtrlPtr;       /* SL control pointer             */
    SL_NODE_STC         *nodePtr;         /* SL node pointer                */

    /* validate the input params */
    if (NULL == ctrlPtr)
    {
        cpssOsPrintf("invalid input ptr\n");
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    slCtrlPtr   = (SL_CTRL_STC*)ctrlPtr;
    nodePtr     = slCtrlPtr->hdrPtr;

    if (nodePtr->fwrdArr[0] != SL_END)
    {
        cpssOsPrintf(" Skip-List Head Current level = %d\n", slCtrlPtr->currLevel);

        nodePtr = nodePtr->fwrdArr[0];

        while (nodePtr != SL_END)
        {
            cpssOsPrintf(" %p -> %8p %8p %8p\n",
                     nodePtr, nodePtr->dataPtr[0],
                     nodePtr->dataPtr[1],
                     nodePtr->dataPtr[2]);

            nodePtr = nodePtr->fwrdArr[0];
        }
    }
    else
    {
        cpssOsPrintf("Skip-List is empty\n");
    }

    return GT_OK;
}


typedef struct
{
    GT_U32 a;
}SL_TEST_STC;

GT_COMP_RES debugSlCompFunc(GT_VOID *pStruct1, GT_VOID *pStruct2)
{
    SL_TEST_STC * elem1Ptr = (SL_TEST_STC*)pStruct1;
    SL_TEST_STC * elem2Ptr = (SL_TEST_STC*)pStruct2;

    if(elem1Ptr->a == elem2Ptr->a)
        return GT_EQUAL;

    if(elem1Ptr->a < elem2Ptr->a)
        return GT_SMALLER;

    return GT_GREATER;
}

#define ITEMS 0x10000

GT_STATUS debugSlTest(GT_VOID)
{
    GT_VOID       * sl;
    GT_32           i;
    GT_U32        * data;
    GT_UINTPTR      iterator = 1;

    /* allocate the user data table */
    data = cpssOsMalloc(sizeof(GT_U32) * ITEMS);

    /* insert values to the keys */
    for (i = 0; i< ITEMS; i++)
    {
        data[i] = i;
    }

    /* init the skip-list */
    sl = prvCpssSlInit(debugSlCompFunc, ITEMS);

    if (sl == NULL)
        return 1;

    /************** insert & delete all *****************/
    {
        SL_TEST_STC searchItem = { 3 };

        /* validate the list is empty */
        if (prvCpssSlIsEmpty(sl) == GT_FALSE)
        {
            cpssOsPrintf("slIsEmpty ERROR\n");
            return 1;
        }

        /* add all items to skip list */
        for (i = 0; i < ITEMS; i++)
        {
            if (prvCpssSlAdd(sl, &data[i], &iterator) != GT_OK)
            {
                cpssOsPrintf("slAdd ERROR %d\n", i);
                return 1;
            }
        }

        /* search for key==3 */
        if (prvCpssSlSearch(sl, (void*)&searchItem) == NULL)
        {
            cpssOsPrintf("slSearch ERROR\n");
            return 1;
        }

        /* validate the list is not empty */
        if (prvCpssSlIsEmpty(sl) == GT_TRUE)
        {
            cpssOsPrintf("slIsEmpty ERROR\n");
            return 1;
        }

        /* remove all keys from skip-list */
        for (i = 0; i < ITEMS; i++)
        {
            if (prvCpssSlDelete(sl, &data[i]) == NULL)
            {
                cpssOsPrintf("slDelete ERROR %d\n", i);
                return 1;
            }
        }
    }

    /************** insert and delete  *****************/
    {
        /* 1 Delete a deleted key */
        /* 1.1 add a key */
        if (prvCpssSlAdd(sl, &data[1], &iterator) != GT_OK)
        {
            cpssOsPrintf("prvCpssSlAdd ERROR %d\n", 1);
            return 1;
        }

        /* 1.2 remove the key */
        if (prvCpssSlDelete(sl, &data[1]) == NULL)
        {
            cpssOsPrintf("prvCpssSlDelete ERROR %d\n", 1);
            return 1;
        }

        /* 1.3 try to remove the same key again, expect for error */
        if (prvCpssSlDelete(sl, &data[1]) != NULL)
        {
            cpssOsPrintf("prvCpssSlDelete ERROR %d\n", 1);
            return 1;
        }
    }

    {
        /* 2 Delete a key that was not inserted */
        /* 2.1 add a key */
        if (prvCpssSlAdd(sl, &data[1], &iterator) != GT_OK)
        {
            cpssOsPrintf("prvCpssSlAdd ERROR %d\n", 1);
            return 1;
        }

        /* 2.2 delete a different key, expect an error */
        if (prvCpssSlDelete(sl, &data[2]) != NULL)
        {
            cpssOsPrintf("prvCpssSlDelete ERROR %d\n", 2);
            return 1;
        }

        /* 2.3 delete the key */
        if (prvCpssSlDelete(sl, &data[1]) == NULL)
        {
            cpssOsPrintf("prvCpssSlDelete ERROR %d\n", 1);
            return 1;
        }
    }

    {
        /* 3 Addition of two identical keys */
        /* 3.1 add a key */
        if (prvCpssSlAdd(sl, &data[1], &iterator) != GT_OK)
        {
            cpssOsPrintf("prvCpssSlAdd ERROR %d\n", 1);
            return 1;
        }

        /* 3.2 add the same key once again, expect en error */
        if (prvCpssSlAdd(sl, &data[1], &iterator) == GT_OK)
        {
            cpssOsPrintf("prvCpssSlAdd ERROR %d\n", 1);
            return 1;
        }

        /* 3.3 delete the key */
        if (prvCpssSlDelete(sl, &data[1]) == NULL)
        {
            cpssOsPrintf("prvCpssSlDelete ERROR %d\n", 1);
            return 1;
        }
    }

    {
        /* 4 Get first/next routine test */
        SL_TEST_STC searchItem;
        SL_TEST_STC * pElem;

        /* 4.1 get the first key in a empty skip-list, expect NULL return */
        if ((pElem = prvCpssSlGetFirst(sl, NULL, &iterator)) != NULL)
        {
            cpssOsPrintf("prvCpssSlGetFirst ERROR\n");
            return 1;
        }

        /* 4.2 add three keys */
        if (prvCpssSlAdd(sl, &data[0], &iterator) != GT_OK)
        {
            cpssOsPrintf("prvCpssSlAdd ERROR %d\n", 0);
            return 1;
        }

        if (prvCpssSlAdd(sl, &data[1], &iterator) != GT_OK)
        {
            cpssOsPrintf("prvCpssSlAdd ERROR %d\n", 1);
            return 1;
        }

        if (prvCpssSlAdd(sl, &data[2], &iterator) != GT_OK)
        {
            cpssOsPrintf("prvCpssSlAdd ERROR %d\n", 1);
            return 1;
        }

        /* 4.3 get the first key in a non empty skip-list, expect the first key */
        if ((pElem = prvCpssSlGetFirst(sl, NULL, &iterator)) == NULL)
        {
            cpssOsPrintf("prvCpssSlGetFirst ERROR\n");
            return 1;
        }
        else
            cpssOsPrintf("prvCpssSlGetFirst() -> %d\n", pElem->a);

        /* 4.4 set the iterator (last key) and get the first key, expect NULL */
        searchItem.a = 3;
        if (prvCpssSlGetFirst(sl, &searchItem, &iterator) != NULL)
        {
            cpssOsPrintf("prvCpssSlGetFirst ERROR\n");
            return 1;
        }

        /* 4.5 set the iterator (first key) and get the first key, expect the second key */
        searchItem.a = 1;
        if ((pElem = prvCpssSlGetFirst(sl, &searchItem, &iterator)) == NULL)
        {
            cpssOsPrintf("prvCpssSlGetFirst ERROR\n");
            return 1;
        }
        else
            cpssOsPrintf("prvCpssSlGetFirst() -> %d\n", pElem->a);

        /* 4.6 get the next key, expect the third key */
        if ((pElem = prvCpssSlGetNext(sl, &iterator)) == NULL)
        {
            cpssOsPrintf("prvCpssSlGetNext ERROR\n");
            return 1;
        }
        else
            cpssOsPrintf("prvCpssSlGetNext() -> %d\n", pElem->a);

        /* 4.6 get the next key, expect the NULL */
        if ((pElem = prvCpssSlGetNext(sl, &iterator)) != NULL)
        {
            cpssOsPrintf("prvCpssSlGetNext ERROR\n");
            return 1;
        }
    }

    {
        /* 5 destroy the skip list */
        if (prvCpssSlDestroy(sl, cpssOsFree) != GT_OK)
        {
            cpssOsPrintf("prvCpssSlDestroy ERROR\n");
            return 1;
        }
    }

    return 0;
}


#endif /* SL_DEBUG */







