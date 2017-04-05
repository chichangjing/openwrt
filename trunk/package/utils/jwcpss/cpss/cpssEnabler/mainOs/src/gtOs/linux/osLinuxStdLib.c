/*******************************************************************************
* osLinuxStdLib.c
*
* DESCRIPTION:
*       Operating System wrapper. stdlib functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

/************* Includes *******************************************************/

#include <gtOs/gtOsStdLib.h>

#include <stdlib.h>

/************* Functions ******************************************************/

/*******************************************************************************
* osQsort
*
* DESCRIPTION:
*       Sort an array
*
* INPUTS:
*       array      - Address of first element in array
*       nItems     - Number of items in array
*       itemSize   - Size of one memner
*       comparFunc - Function which compares two items
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osQsort
(
    IN  GT_VOID_PTR             array,
    IN  GT_U32                  nItems,
    IN  GT_U32                  itemSize,
    IN  COMPARE_ITEMS_FUNC_PTR  comparFunc
)
{
    qsort(array, nItems, itemSize, comparFunc);
    return GT_OK;
}

/*******************************************************************************
* osBsearch
*
* DESCRIPTION:
*       binary search of a sorted array
*
* INPUTS:
*       key        - key to search
*       array      - Address of first element in array
*       nItems     - Number of items in array
*       itemSize   - Size of one memner
*       comparFunc - Function which compares two items
*
* OUTPUTS:
*       result     - pointer to item found or NULL
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osBsearch
(
    IN  const void *            key,
    IN  const void *            array,
    IN  GT_U32                  nItems,
    IN  GT_U32                  itemSize,
    IN  COMPARE_ITEMS_FUNC_PTR  comparFunc,
    OUT GT_VOID_PTR             *result
)
{
    void *ret;

    ret = bsearch(key, array, nItems, itemSize, comparFunc);

    if (result != NULL)
        *result = ret;

    return (ret == NULL) ? GT_FAIL : GT_OK;
}
