/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfCommonList.c
*
* DESCRIPTION:
*       Implementation of double-linked list functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <extUtils/tgf/tgfCommonList.h>



/*******************************************************************************
* prvTgfListDelItem
*
* DESCRIPTION:
*		Delete item from list.
*
* INPUT:
*		listPtr - pointer to list head structure.
*		itemPtr - pointer to element, which should be deleted.
*
* OUTPUT:
*       None.
*
* RETURN:
*		None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfListDelItem
(
    INOUT PRV_TGF_LIST_HEAD_PTR       listPtr,
    IN    const PRV_TGF_LIST_ITEM_PTR itemPtr
)
{
    if (NULL == itemPtr)
    {
        return;
    };

    (NULL != itemPtr->prevPtr)
        ? (itemPtr->prevPtr->nextPtr = itemPtr->nextPtr)
        : (listPtr->firstPtr      = itemPtr->nextPtr);

    (NULL != itemPtr->nextPtr)
        ? (itemPtr->nextPtr->prevPtr = itemPtr->prevPtr)
        : (listPtr->lastPtr       = itemPtr->prevPtr);

    cpssOsFree(itemPtr);
    listPtr->count--;
};


/*******************************************************************************
* prvTgfListDelAll
*
* DESCRIPTION:
*		Delete all items of list.
*
* INPUT:
*		listPtr - pointer to the list head structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*		None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfListDelAll
(
    INOUT PRV_TGF_LIST_HEAD_PTR listPtr
)
{
    if (NULL != listPtr)
    {
        while (NULL != listPtr->firstPtr)
        {
            prvTgfListDelItem(listPtr, listPtr->firstPtr);
        };
    };
};


/*******************************************************************************
* prvTgfListAddAfter
*
* DESCRIPTION:
*		Insert new empty item into list after pointed item.
*
* INPUT:
*		listPtr     - pointer to list head structure.
*		prevItemPtr - pointer to element, after which new one will be inserted.
*	  				  If NULL, then new element will become first item of list.
*
* OUTPUT:
*       None.
*
* RETURN:
*		pointer to new inserter element. Or NULL if element was not inserted.
*       It may be caused by cpssOsMalloc error or list can be fool.
*
* COMMENTS:
*		Function doesn't check pointer "listPtr" on NULL.
*
*******************************************************************************/
PRV_TGF_LIST_ITEM_PTR prvTgfListAddAfter
(
  INOUT PRV_TGF_LIST_HEAD_PTR   listPtr,
  INOUT PRV_TGF_LIST_ITEM_PTR   prevItemPtr
)
{
    PRV_TGF_LIST_ITEM_PTR itemPtr;

    if (listPtr->count >= listPtr->max)
    {
        return NULL;
    };
    itemPtr = (PRV_TGF_LIST_ITEM_PTR) cpssOsMalloc(listPtr->itemSize);
    if (NULL == itemPtr) /*cpssOsMalloc error*/
    {
        return NULL;
    };
    itemPtr->prevPtr = prevItemPtr;

    if (NULL == prevItemPtr)   /* add item at the begin of the list*/
    {
        itemPtr->nextPtr = listPtr->firstPtr;
        if ( NULL != listPtr->firstPtr)
        {
            listPtr->firstPtr->prevPtr = itemPtr;
        }
        listPtr->firstPtr = itemPtr;
    }
    else
    {
        itemPtr->nextPtr = prevItemPtr->nextPtr;
        prevItemPtr->nextPtr = itemPtr;
    };

    if (prevItemPtr == listPtr->lastPtr) /*item was added to the end of the list*/
    {
        listPtr->lastPtr = itemPtr;
    };

    listPtr->count++;
    return itemPtr;
};


/*******************************************************************************
* prvTgfListAppend
*
* DESCRIPTION:
*		Insert new empty item into end of list.
*
* INPUT:
*		listPtr - pointer to list head structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*		pointer to the new inserted element. If NULL then
*       a cpssOsMalloc error occurred or list already full.
*
* COMMENTS:
*
*******************************************************************************/
PRV_TGF_LIST_ITEM_PTR prvTgfListAppend
(
  INOUT PRV_TGF_LIST_HEAD_PTR listPtr
)
{
    return prvTgfListAddAfter(listPtr, listPtr->lastPtr);
};
