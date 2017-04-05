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
* appDemoFdbIpv6UcLinkedList.h
*
* DESCRIPTION:
*       the implementation of functions to manipulate of FDB IPv6 UC Route
*       entries linked list, that holds AddressIndex to DataIndex mapping.
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>

#ifdef CHX_FAMILY

/*******************************************************************************
* appDemoBrgFdbIpv6UcFreeAddrDataLinkedList
*
* DESCRIPTION:
*       Free FDB IPv6 UC Route entries linked list,
*       that holds AddressIndex to DataIndex mapping
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Only elements that were deleted in flush process will be deleted
*       from the Linked List. If only part of the IPV6 UC entry was deleted
*       (address entry or data entry) then the Linked list will be updated
*       with value PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS indicating the
*       entry is not valid
*
*******************************************************************************/
GT_STATUS appDemoBrgFdbIpv6UcFreeAddrDataLinkedList
(
    IN GT_U8  devNum
);

/*******************************************************************************
* appDemoBrgFdbIpv6UcUpdateAddrDataLinkedList
*
* DESCRIPTION:
*       Update FDB IPv6 UC Route entries linked list,
*       that holds AddressIndex to DataIndex mapping
*
* INPUTS:
*       devNum    - the device number
*       addrIndex - FDB IPv6 UC address index bind to dataIndex
*       dataIndex - FDB IPv6 UC data index bind to addrIndex
*       deleteElement - GT_TRUE: delete the element if found
*                       GT_FALSE: update the element if found or add a new one
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success,
*       GT_OUT_OF_CPU_MEM   - on allocation fail
*       GT_FAIL             - otherwise.
*
* COMMENTS:
*       adding a mapping of addrIndex and dataIndex to the Linked List must be
*       of valid entries that were added to HW.
*       if the address entry or the data entry is not added to HW this function will fail.
*       the Linked List should reflect the HW state
*
*******************************************************************************/
GT_STATUS appDemoBrgFdbIpv6UcUpdateAddrDataLinkedList
(
    IN  GT_U8           devNum,
    IN  GT_U32          addrIndex,
    IN  GT_U32          dataIndex,
    IN  GT_BOOL         deleteElement
);

/*******************************************************************************
* prvAppDemoBrgFdbIpv6UcFindPairIndexToDelete
*
* DESCRIPTION:
*       This function get an FDB IPv6 UC index that can be a dataIndex or an
*       addressIndex and return the related index bound to it from the linked list.
*
*
* INPUTS:
*       devNum    - the device number
*       entryType - the type of the index input parameter.
*       index     - according to the entryType this can be a dataIndex or a addressIndex
*       deleteElement - GT_TRUE: delete the element if found
*                       GT_FALSE: just return the index of the elemnt if found
* OUTPUTS:
*       pairIndexPtr - (pointer to) the pair index if found, according to the
*                       entryType this can be an addressIndex or a dataIndex
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_FOUND             - incase there is no matching pair for this index.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvAppDemoBrgFdbIpv6UcFindPairIndexToDelete
(
    IN  GT_U8                           devNum,
    IN  CPSS_MAC_ENTRY_EXT_TYPE_ENT     entryType,
    IN  GT_U32                          index,
    IN  GT_BOOL                         deleteElement,
    OUT  GT_U32                         *pairIndexPtr
);

#endif  /* CHX_FAMILY */
