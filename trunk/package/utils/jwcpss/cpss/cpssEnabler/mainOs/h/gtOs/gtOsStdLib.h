/*******************************************************************************
* gtOsStdLib.h
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

#ifndef __gtOsStdLibh
#define __gtOsStdLibh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/************* Typedefs *******************************************************/
/*
 * Typedef: COMPARE_ITEMS_FUNC_PTR
 *
 * Description: Comparison function type
 *
 * Inputs:
 *      a - pointer to one item
 *      b - pointer to another item
 *
 * Outputs:
 *      None
 *
 * Returns:
 *      Less than zero if a < b
 *      Zero if a == b
 *      greather than zero if a > b
 *
 */
typedef int(*COMPARE_ITEMS_FUNC_PTR)
(
    const void *a,
    const void *b
);

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
    IN  void *                  array,
    IN  GT_U32                  nItems,
    IN  GT_U32                  itemSize,
    IN  COMPARE_ITEMS_FUNC_PTR  comparFunc
);

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
);


#ifdef __cplusplus
}
#endif

#endif  /* __gtOsStdLibh */
