/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfCommonList.h
*
* DESCRIPTION:
*       Double-linked list functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvTgfCommonListh
#define __prvTgfCommonListh

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Typedefs */
/*
 * Typedef: struct PRV_TGF_LIST_ITEM_STC
 *
 * Description: -- item of double-linked list.
 *
 * Fields:
 *        nextPtr - pointer to a next item of list. If NULL item is last.
 *        prevPtr - pointer to a next item of list. If NULL item is first.
 * Comments:
 *        You can use struct-"descendants" of PRV_TGF_LIST_ITEM_STCT
 *        as list items. It means the first field of such struct is
 *        PRV_TGF_LIST_ITEM_STC  and some additional fields can be present.
 */
typedef struct PRV_TGF_LIST_ITEM_STCT
{
    struct PRV_TGF_LIST_ITEM_STCT *nextPtr;
    struct PRV_TGF_LIST_ITEM_STCT *prevPtr;
} PRV_TGF_LIST_ITEM_STC, *PRV_TGF_LIST_ITEM_PTR;


/*
 * Typedef: struct PRV_TGF_LIST_HEAD_STC
 *
 * Description: -- list head information.
 *
 * Fields:
 *        lastPtr  - pointer to last item of list. If NULL list is empty.
 *        firstPtr - pointer to first item of list. If NULL list is empty.
 *        itemSize - size of list item. 
 *        count    - count of list items. It can't exceed max field value.
 *        max      - maximum amount of list items.
 * Comments:
 *        itemSize - is not necessary equal sizeof(PRV_TGF_LIST_ITEM_STC).
 *                   It can be more (if list item is descendant of
 *                   PRV_TGF_LIST_ITEM_STC), but can't be less.
 */
typedef struct
{
    PRV_TGF_LIST_ITEM_PTR lastPtr;
    PRV_TGF_LIST_ITEM_PTR firstPtr;
    GT_U32                itemSize;
    GT_U32                count;
    GT_U32                max;
} PRV_TGF_LIST_HEAD_STC, *PRV_TGF_LIST_HEAD_PTR;


    
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
extern GT_VOID prvTgfListDelAll(INOUT  PRV_TGF_LIST_HEAD_PTR listPtr);


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
*		This should be done by a calling code.
*
*******************************************************************************/
extern PRV_TGF_LIST_ITEM_PTR prvTgfListAddAfter
(
    INOUT PRV_TGF_LIST_HEAD_PTR listPtr,
    INOUT PRV_TGF_LIST_ITEM_PTR prevItemPtr
);


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
extern PRV_TGF_LIST_ITEM_PTR prvTgfListAppend
(
    INOUT PRV_TGF_LIST_HEAD_PTR listPtr
);



/*******************************************************************************
* prvTgfListDelItem
*
* DESCRIPTION:
*		Delete item from list.
*
* INPUT:
*		listPtr - pointer to list head structure.
*		itemPtr - pointer to element which should be deleted.
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
extern GT_VOID prvTgfListDelItem(INOUT    PRV_TGF_LIST_HEAD_PTR listPtr,
                                 IN const PRV_TGF_LIST_ITEM_PTR itemPtr);

    
/*******************************************************************************
* prvTgfListInit
*
* DESCRIPTION:
*		Initialize fields of a newly created list.
*
* INPUT:
*		listPtr  - pointer to list head structure.
*		itemSize - size of item of the list.
*		max      - maximum amount of items in list.
*
* OUTPUT:
*       listPtr  - field values will be changed appropriately.
*
* RETURN:
*		GT_BAD_PARAM - if itemSize is less then sizeof(PRV_TGF_LIST_ITEM_STC)
*       GT_OK        - all ok.
* COMMENTS:
*
*******************************************************************************/
extern GT_STATUS prvTgfListInit(INOUT PRV_TGF_LIST_HEAD_PTR listPtr,
                                IN const GT_U32             itemSize,
                                IN const GT_U32             max);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfCommonListh */
