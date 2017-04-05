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
* appDemoFdbIpv6UcLinkedList.c
*
* DESCRIPTION:
*       the implementation of functions to manipulate of FDB IPv6 UC Route
*       entries linked list, that holds AddressIndex to DataIndex mapping.
*
*
* FILE REVISION NUMBER:
*       $Revision: 26 $
******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <extUtils/cpssEnabler/appDemoFdbIpv6UcLinkedList.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
    #include <cpss/extServices/private/prvCpssBindFunc.h>
#endif  /* CHX_FAMILY */

#ifdef CHX_FAMILY

/*
 * Typedef: struct GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STCT
 *
 * Description: Stores FDB IPv6 UC ROUTE Entry AddressIndex and DataIndex of an ipv6 entry
 *
 * Fields:
 *      addressIndex - index of the ipv6 address entry bound to dataIndex
 *      dataIndex    - index of the ipv6 data entry binded to addressIndex
 *      nextElemPtr  - (pointer to) next element in the Linked List
 *
 */
typedef struct GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STCT
{
    GT_U32 addressIndex;
    GT_U32 dataIndex;
    struct GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STCT *nextElemPtr;

}GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC;

/* IPv6 UC Indexes mapping database (key/data pairs) */
GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6LinkedListHeadPtr;

static GT_U32 ipv6LinkedListNumOfElements=0;

#define PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS 0xFFFFFFFF  /* TODO: убрать отседова, юзать уже существующий, сделав его общим */

/*******************************************************************************
* appDemoBrgFdbIpv6AddrDataLinkedListDump
*
* DESCRIPTION:
*       Dump FDB IPv6 UC Route entries linked list,
*       that holds AddressIndex to DataIndex mapping
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID appDemoBrgFdbIpv6AddrDataLinkedListDump
(
    GT_VOID
)
{
    GT_U32                              i=0;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6NodePtr;

    ipv6NodePtr = ipv6LinkedListHeadPtr;

    /* no elements to free */
    if(ipv6LinkedListNumOfElements==0)
    {
        cpssOsPrintf("FDB IPV6 UC address/data index mapping DB is empty \n");
        return;
    }

    cpssOsPrintf("FDB IPV6 UC address/data index mapping DB have [%d] elements \n",ipv6LinkedListNumOfElements);

    for(i=0; i<ipv6LinkedListNumOfElements; i++)
    {
        cpssOsPrintf("pair number [%d]: addressIndex[%d],dataIndex[%d] \n",i,ipv6NodePtr->addressIndex, ipv6NodePtr->dataIndex);

        if(ipv6NodePtr->nextElemPtr!=NULL)
        {
            ipv6NodePtr=ipv6NodePtr->nextElemPtr;
        }
        else
        {
            return;
        }
    }

    return;

}

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
)
{
    GT_STATUS                           rc = GT_OK;
    GT_U32                              i=0;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6NodePtr;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6PrevNodePtr;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6NodeToDeletePtr;
    GT_U32                              tempIpv6LinkedListNumOfElements;
    GT_BOOL                             valid;
    GT_BOOL                             skip;

    ipv6NodePtr = ipv6LinkedListHeadPtr;
    ipv6PrevNodePtr = ipv6LinkedListHeadPtr;

    tempIpv6LinkedListNumOfElements = ipv6LinkedListNumOfElements;

    /* no elements to free */
    if(ipv6LinkedListNumOfElements==0)
        return GT_OK;

    for(i=0; i<tempIpv6LinkedListNumOfElements; i++)
    {
        if(ipv6NodePtr->addressIndex!=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)
        {
            rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,ipv6NodePtr->addressIndex,&valid,&skip);
            if(rc != GT_OK)
                return rc;
            if(valid==GT_FALSE)
            {
                /* the address entry was deleted --> update the LinkedList with invalid index value */
                ipv6NodePtr->addressIndex = PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
            }
        }

        if(ipv6NodePtr->dataIndex!=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)
        {
            rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,ipv6NodePtr->dataIndex,&valid,&skip);
            if(rc != GT_OK)
                return rc;
            if(valid==GT_FALSE)
            {
                /* the data entry was deleted --> update the LinkedList with invalid index value */
                ipv6NodePtr->dataIndex = PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
            }
        }

        /* if both data entry and address entry were deleted from HW then delete it from Linked List */
        if((ipv6NodePtr->addressIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS) &&
           (ipv6NodePtr->dataIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS))
        {
            /* if this is not the last element in the linked list */
            if(ipv6NodePtr->nextElemPtr!=NULL)
            {
                /* if we delete the first element in the list need to update the head pointer */
                if(ipv6NodePtr == ipv6LinkedListHeadPtr)
                {
                    ipv6LinkedListHeadPtr=ipv6LinkedListHeadPtr->nextElemPtr;
                    ipv6PrevNodePtr=ipv6LinkedListHeadPtr;
                    ipv6NodeToDeletePtr = ipv6NodePtr;
                    ipv6NodePtr=ipv6LinkedListHeadPtr;
                }
                else
                {
                    ipv6PrevNodePtr->nextElemPtr = ipv6NodePtr->nextElemPtr;
                    ipv6NodeToDeletePtr = ipv6NodePtr;
                    ipv6NodePtr=ipv6NodePtr->nextElemPtr;
                }

                 cpssOsFree(ipv6NodeToDeletePtr);
                 ipv6LinkedListNumOfElements--;
            }
            else
            {
                if(ipv6NodePtr == ipv6LinkedListHeadPtr)
                {
                    /* deleting the last and only element in the list */
                    ipv6LinkedListHeadPtr=NULL;
                }
                else
                {
                    /* deleting last element in the list */
                    ipv6PrevNodePtr->nextElemPtr=NULL;
                }

                cpssOsFree(ipv6NodePtr);
                ipv6LinkedListNumOfElements--;
                return GT_OK;
            }
        }
        else
        {
            if(ipv6NodePtr->nextElemPtr!=NULL)
            {
                /* this is the first iterration */
                if(ipv6NodePtr==ipv6PrevNodePtr)
                {
                    ipv6NodePtr=ipv6NodePtr->nextElemPtr;
                }
                else
                {
                    ipv6NodePtr=ipv6NodePtr->nextElemPtr;
                    ipv6PrevNodePtr=ipv6PrevNodePtr->nextElemPtr;
                }
            }
            else
            {
                /* reached end of the list */
                if((i+1)!=tempIpv6LinkedListNumOfElements)
                {
                    /* some error occurred.
                      if we get to this point it means we went over all the Linked List,
                      so "i" should be equal to the size of the Linked List */
                    return GT_BAD_STATE;
                }
                else
                    return GT_OK;
            }

        }
    }

    return GT_OK;

}


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
)
{
    GT_STATUS rc=GT_OK;
    GT_U32    i=0;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6LinkedListElemPtr;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6NodePtr;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6PrevNodePtr;
    GT_BOOL                             addrValid;
    GT_BOOL                             addrSkip;
    GT_BOOL                             dataValid;
    GT_BOOL                             dataSkip;
    GT_BOOL                             valid;
    GT_BOOL                             skip;


    rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,addrIndex,&addrValid,&addrSkip);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,dataIndex,&dataValid,&dataSkip);
    if(rc != GT_OK)
        return rc;

    if(deleteElement==GT_FALSE)
    {
        /* check if the address entry and the data entry are valid,
           meaning was already added to HW */
        if((addrValid==GT_FALSE)||(dataValid==GT_FALSE))
        {
            cpssOsPrintf("both addressIndex and dataIndex entries should be valid in HW\n"
                         "before adding the pair to the Linked List\n");
            return GT_FAIL;
        }
    }

    if(ipv6LinkedListHeadPtr==NULL)
    {
        if(deleteElement==GT_FALSE)
        {
            /* create first element in the list */
            ipv6LinkedListHeadPtr = (GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC*)cpssOsMalloc(sizeof(GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC));
            if (ipv6LinkedListHeadPtr == NULL)
            {
                /* fail to allocate space */
                return GT_OUT_OF_CPU_MEM;
            }

            ipv6LinkedListHeadPtr->addressIndex=addrIndex;
            ipv6LinkedListHeadPtr->dataIndex=dataIndex;
            ipv6LinkedListHeadPtr->nextElemPtr=NULL;
            ipv6LinkedListNumOfElements++;

            return GT_OK;
        }
        else
        {
            /* the List is empty the element is not found */
            return GT_NOT_FOUND;
        }
    }
    else
    {
        ipv6NodePtr=ipv6LinkedListHeadPtr;
        ipv6PrevNodePtr=ipv6LinkedListHeadPtr;

        /* add element to list - check if the element is already defined,
           if it is defined then need to update the values */

        for(i=0; i<ipv6LinkedListNumOfElements; i++)
        {
            /* trying to add am element to Linked List */
            if(deleteElement==GT_FALSE)
            {
                 if((ipv6NodePtr->addressIndex==addrIndex)&&(ipv6NodePtr->dataIndex!=dataIndex)&&
                    (ipv6NodePtr->dataIndex!=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS))
                 {
                     /* check if the ipv6NodePtr->dataIndex is deleted from HW, if so we can overwrite it with the new value */
                     rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,ipv6NodePtr->dataIndex,&valid,&skip);
                     if(rc != GT_OK)
                        return rc;
                     if(valid==GT_FALSE)
                     {
                         ipv6NodePtr->dataIndex = dataIndex;
                         return GT_OK;
                     }
                     else
                     {
                         cpssOsPrintf("can not overwrite the pair since dataIndex[%d] previously mapped to addrIndex[%d]\n"
                                      "is a valid entry in HW by overwrite the value we will lose the pointer for deleting\n"
                                      "this entry in the future by the messaging mechanism\n",ipv6NodePtr->dataIndex, addrIndex);
                         return GT_FAIL;

                     }
                 }
                 if((ipv6NodePtr->dataIndex==dataIndex)&&(ipv6NodePtr->addressIndex!=addrIndex)&&
                    (ipv6NodePtr->addressIndex!=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS))
                 {
                     /* check if the ipv6NodePtr->addressIndex is deleted from HW, if so we can overwrite it with the new value */
                     rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,ipv6NodePtr->addressIndex,&valid,&skip);
                     if(rc != GT_OK)
                        return rc;
                     if(valid==GT_FALSE)
                     {
                         ipv6NodePtr->addressIndex = addrIndex;
                         return GT_OK;
                     }
                     else
                     {
                         cpssOsPrintf("can not overwrite the pair since addrIndex[%d] previously mapped to dataIndex[%d]\n"
                                      "is a valid entry in HW by overwrite the value we will lose the pointer for deleting\n"
                                      "this entry in the future by the messaging mechanism \n",ipv6NodePtr->addressIndex, dataIndex);
                         return GT_FAIL;
                     }
                 }
                 if(ipv6NodePtr->addressIndex==dataIndex)
                 {
                     /* check that the addrIndex bind to the current dataIndex is invalid in HW,
                        if so we can overwrite the pair with a new one */

                     if(ipv6NodePtr->dataIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)
                     {
                         valid=GT_FALSE;
                     }
                     else
                     {
                         rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,ipv6NodePtr->dataIndex,&valid,&skip);
                         if(rc != GT_OK)
                             return rc;
                     }
                     if(valid==GT_FALSE)
                     {
                         ipv6NodePtr->addressIndex = addrIndex;
                         ipv6NodePtr->dataIndex = dataIndex;
                         return GT_OK;
                     }
                     else
                     {
                         cpssOsPrintf("can not overwrite the pair since dataIndex[%d] previously mapped to addressIndex[%d]\n"
                                      "is a valid entry in HW by overwrite the value we will lose the pointer for deleting\n"
                                      "this entry in the future by the messaging mechanism \n",ipv6NodePtr->dataIndex, ipv6NodePtr->addressIndex);

                         return GT_FAIL;
                     }
                 }
                 if(ipv6NodePtr->dataIndex==addrIndex)
                 {
                     /* check that the addrIndex bind to the current dataIndex is invalid in HW,
                        if so we can overwrite the pair with a new one */
                     if(ipv6NodePtr->addressIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)
                     {
                         valid=GT_FALSE;
                     }
                     else
                     {
                         rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,ipv6NodePtr->addressIndex,&valid,&skip);
                         if(rc != GT_OK)
                             return rc;
                     }
                     if(valid==GT_FALSE)
                     {
                         ipv6NodePtr->addressIndex = addrIndex;
                         ipv6NodePtr->dataIndex = dataIndex;
                         return GT_OK;
                     }
                     else
                     {
                         cpssOsPrintf("can not overwrite the pair since addrIndex[%d] previously mapped to dataIndex[%d]\n"
                                      "is a valid entry in HW by overwrite the value we will lose the pointer for deleting\n"
                                      "this entry in the future by the messaging mechanism \n",ipv6NodePtr->addressIndex, ipv6NodePtr->dataIndex);
                         return GT_FAIL;
                     }
                 }
            }


            if(((ipv6NodePtr->addressIndex==addrIndex)&&(ipv6NodePtr->dataIndex==dataIndex))||
               ((ipv6NodePtr->addressIndex==addrIndex)&&(ipv6NodePtr->dataIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS))||
               ((ipv6NodePtr->addressIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)&&(ipv6NodePtr->dataIndex==dataIndex)))
            {
                /* entry already defined */
                if(deleteElement==GT_FALSE)
                {
                    /* set the values in case one of them are PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS */
                    ipv6NodePtr->addressIndex=addrIndex;
                    ipv6NodePtr->dataIndex=dataIndex;
                    return GT_OK;
                }
                else
                {
                    /* check if the element need to be deleted or just updated with
                       the invalid PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS value */
                    if(((addrValid==GT_FALSE)&&(dataValid==GT_FALSE))||
                       ((addrValid==GT_FALSE)&&(ipv6NodePtr->dataIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS))||
                       ((ipv6NodePtr->addressIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)&&(dataValid==GT_FALSE)))
                    {
                        /*  entry found need to delete it */

                        /* if this is the first element in the linked list and
                           there are more elements in the list*/
                        if((ipv6NodePtr==ipv6LinkedListHeadPtr)&&(ipv6LinkedListHeadPtr->nextElemPtr!=NULL))
                        {
                            ipv6LinkedListHeadPtr=ipv6NodePtr->nextElemPtr;
                            cpssOsFree(ipv6NodePtr);
                            ipv6LinkedListNumOfElements--;

                            return GT_OK;
                        }
                        else
                        {   /* if this is the first and the only element in the linked list */
                            if((ipv6NodePtr==ipv6LinkedListHeadPtr)&&(ipv6LinkedListHeadPtr->nextElemPtr==NULL))
                            {
                                cpssOsFree(ipv6NodePtr);
                                ipv6LinkedListHeadPtr=NULL;
                                ipv6LinkedListNumOfElements--;
                                return GT_OK;
                            }
                            else
                            {
                                /* if this is the last element to delete from the Linked List */
                                if(ipv6NodePtr->nextElemPtr==NULL)
                                {
                                    ipv6PrevNodePtr->nextElemPtr=NULL;
                                    cpssOsFree(ipv6NodePtr);
                                    ipv6LinkedListNumOfElements--;

                                    return GT_OK;
                                }
                                else
                                {
                                    /* if we are deleting from the middle */
                                    ipv6PrevNodePtr->nextElemPtr = ipv6NodePtr->nextElemPtr;
                                    cpssOsFree(ipv6NodePtr);
                                    ipv6LinkedListNumOfElements--;

                                    return GT_OK;
                                }
                            }
                        }
                    }
                    else
                    {
                        if(addrValid==GT_FALSE)
                        {
                            ipv6NodePtr->addressIndex=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
                            /* we want to give an indication to the user that
                               something was wrong with the delete */
                            cpssOsPrintf("can not delete the pair from Linked List since dataIndex is a valid entry in HW\n."
                                         "update addressIndex value in the pair to be PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS\n");
                            return GT_FAIL;
                        }
                        else
                        {
                            if(dataValid==GT_FALSE)
                            {
                                ipv6NodePtr->dataIndex=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
                                /* we want to give an indication to the user that
                                   something was wrong with the delete */
                                cpssOsPrintf("can not delete the pair from the Linked List since addressIndex is a valid entry in HW\n."
                                             "update dataIndex value in the pair to be PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS\n");
                                return GT_FAIL;
                            }
                            else
                            {
                                cpssOsPrintf("can not delete the pair from the Linked List since both "
                                             "addressIndex and dataIndex entries are valid in HW\n");
                                return GT_FAIL;
                            }
                        }
                    }
                }
            }
            else
            {
                if(ipv6NodePtr->nextElemPtr!=NULL)
                {
                    /* continue to look for a match addrIndex,
                       update the LinkedList pointer */
                    ipv6PrevNodePtr = ipv6NodePtr;
                    ipv6NodePtr=ipv6NodePtr->nextElemPtr;
                }
                else
                {
                    if(deleteElement==GT_FALSE)
                    {
                        /* if we get to the end of the Linked List and we do not find a match addrIndex,
                           then create a new node to the Linked List */
                        ipv6LinkedListElemPtr = (GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC*)cpssOsMalloc(sizeof(GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC));
                        if (ipv6LinkedListElemPtr == NULL)
                        {
                            /* fail to allocate space */
                            return GT_OUT_OF_CPU_MEM;
                        }

                        ipv6LinkedListElemPtr->addressIndex=addrIndex;
                        ipv6LinkedListElemPtr->dataIndex=dataIndex;
                        ipv6LinkedListElemPtr->nextElemPtr=NULL;
                        ipv6NodePtr->nextElemPtr=ipv6LinkedListElemPtr;

                        ipv6LinkedListNumOfElements++;

                        return GT_OK;
                    }
                    else
                    {
                        /* could not find the pair needed */
                        return GT_NOT_FOUND;
                    }
                }
            }
        }

        /* should not get to this point */
        return GT_ERROR;
    }
}


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
)
{
    GT_STATUS rc=GT_OK;
    GT_U32    i=0;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6NodePtr;
    GT_FDB_IPV6_UC_ROUTE_ENTRY_INFO_STC *ipv6PrevNodePtr;

    GT_U32    addressIndex;
    GT_U32    dataIndex;

    GT_BOOL   addressValid;
    GT_BOOL   addressSkip;

    GT_BOOL   dataValid;
    GT_BOOL   dataSkip;

    ipv6NodePtr=ipv6LinkedListHeadPtr;
    ipv6PrevNodePtr=ipv6LinkedListHeadPtr;

    /* if the Linked List empty no search is needed */
    if(ipv6LinkedListNumOfElements==0)
    {
        *pairIndexPtr=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
        return GT_NOT_FOUND;
    }

    switch (entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            /* loop on Linked List and look for DataIndex suitable for AddressIndex */
            for(i=0; i<ipv6LinkedListNumOfElements; i++)
            {
                if(ipv6NodePtr->addressIndex==index)
                {
                    *pairIndexPtr=ipv6NodePtr->dataIndex;

                    /* the pair element is not valid */
                    if((ipv6NodePtr->dataIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)&&
                       (deleteElement==GT_FALSE))
                    {
                        /* no legal element found */
                        *pairIndexPtr=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
                        return GT_NOT_FOUND;
                    }
                    break;
                }
                else
                {
                    if(ipv6NodePtr->nextElemPtr!=NULL)
                    {
                        /* update the LinkedList pointer */
                        ipv6PrevNodePtr = ipv6NodePtr;
                        ipv6NodePtr=ipv6NodePtr->nextElemPtr;
                    }
                    else
                    {
                        /* got to the end of the Linked List and cound not find a match */
                        *pairIndexPtr=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
                        return GT_NOT_FOUND;
                    }
                }
            }
            addressIndex = index;
            dataIndex    = *pairIndexPtr;
            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            /* loop on Linked List and look for AddressIndex suitable for DataIndex */
            for(i=0; i<ipv6LinkedListNumOfElements; i++)
            {
                if(ipv6NodePtr->dataIndex==index)
                {
                    *pairIndexPtr=ipv6NodePtr->addressIndex;

                    /* the pair element is not valid */
                    if((ipv6NodePtr->addressIndex==PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)&&
                       (deleteElement==GT_FALSE))
                    {
                        /* no legal element found */
                        *pairIndexPtr=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
                        return GT_NOT_FOUND;
                    }
                    break;
                }
                else
                {
                    if(ipv6NodePtr->nextElemPtr!=NULL)
                    {
                        /* update the LinkedList pointer */
                        ipv6PrevNodePtr = ipv6NodePtr;
                        ipv6NodePtr=ipv6NodePtr->nextElemPtr;
                    }
                    else
                    {
                        /* got to the end of the Linked List and cound not find a match */
                        *pairIndexPtr=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
                        return GT_NOT_FOUND;
                    }
                }
            }
            addressIndex = *pairIndexPtr;
            dataIndex    = index;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(deleteElement==GT_TRUE)
    {
        /* check if both addr index and data index was deleted from HW */
        if(addressIndex!=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)
        {
            rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,addressIndex,&addressValid,&addressSkip);
            if(rc != GT_OK)
                return rc;
        }
        else
        {
            addressValid=GT_FALSE;
        }
        if(dataIndex!=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)
        {
            rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,dataIndex,&dataValid,&dataSkip);
            if(rc != GT_OK)
                return rc;
        }
        else
        {
            dataValid=GT_FALSE;
        }

        if((addressValid==GT_FALSE)&&(dataValid==GT_FALSE))
        {
            /* if this is the first element in the linked list and
               there are more elements in the list*/
            if((ipv6NodePtr==ipv6LinkedListHeadPtr)&&(ipv6LinkedListHeadPtr->nextElemPtr!=NULL))
            {
                ipv6LinkedListHeadPtr=ipv6NodePtr->nextElemPtr;
                cpssOsFree(ipv6NodePtr);
                ipv6LinkedListNumOfElements--;

                return GT_OK;
            }
            else
            {   /* if this is the first and the only element in the linked list */
                if((ipv6NodePtr==ipv6LinkedListHeadPtr)&&(ipv6LinkedListHeadPtr->nextElemPtr==NULL))
                {
                    cpssOsFree(ipv6NodePtr);
                    ipv6LinkedListHeadPtr=NULL;
                    ipv6LinkedListNumOfElements--;
                    return GT_OK;
                }
                else
                {
                    /* if this is the last element to delete from the Linked List */
                    if(ipv6NodePtr->nextElemPtr==NULL)
                    {
                        ipv6PrevNodePtr->nextElemPtr=NULL;
                        cpssOsFree(ipv6NodePtr);
                        ipv6LinkedListNumOfElements--;

                        return GT_OK;
                    }
                    else
                    {
                        /* if we are deleting from the middle */
                        ipv6PrevNodePtr->nextElemPtr = ipv6NodePtr->nextElemPtr;
                        cpssOsFree(ipv6NodePtr);
                        ipv6LinkedListNumOfElements--;

                        return GT_OK;
                    }
                }
            }
        }
        else
        {
            if(addressValid==GT_FALSE)
            {
                ipv6NodePtr->addressIndex=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
            }
            else/*dataValid==GT_FALSE*/
            {
                ipv6NodePtr->dataIndex=PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS;
            }

            return GT_OK;
        }
    }
    return rc;
}


#endif /* CHX_FAMILY */
