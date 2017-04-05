/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamDispatcher.h
*
* DESCRIPTION:
*       This file defines which TCAM engine is used  : TCAM entry handler or
*       TCAM Manager
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

#ifndef __prvCpssDxChLpmTcamDispatcherh
#define __prvCpssDxChLpmTcamDispatcherh

#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamEntriesHandler.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>


#define PRV_CPSS_DXCH_LPM_TCAM_NON_INITIALIZED_TOKEN_CNS 0xFFFFFFFF

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_ENT
 *
 * Description: define update tcam rule cases
 *
 * Enumerations:
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E               - update trie node of given rule
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_UPDATE_E          - update rule by new allocated rule.
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_AND_TRIE_UPDATE_E - update rule by new allocated rule
 *                                                                         and update its trie node.
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_COPY_E                      - copy rule from one trie node to another.
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_RULE_E                   - invalid rule node.
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_OLD_RULE_E               - invalid legacy rule node only.
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INIT_TOKEN_RULE_INFO_E           - invalid token rule node only.
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INIT_RULE_INFO_E                 - initilize rule parameters.
 *    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_CHECK_RULE_VALIDITY_E            - check rule validity.
 *
 * Comment:
 *      None.
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_UPDATE_E,
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_AND_TRIE_UPDATE_E,
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_COPY_E,
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_RULE_E,
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_OLD_RULE_E,
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INIT_TOKEN_RULE_INFO_E,
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INIT_RULE_INFO_E,
    PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_CHECK_RULE_VALIDITY_E
} PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_ENT;



#define PRV_CPSS_DXCH_LPM_TCAM_SET_BOUNDING_SON_MAC(tcamManagementMode,trieNodePtr,boundingSonTrieNodePtr)       \
        if((tcamManagementMode) == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)        \
            boundingSonTrieNodePtr = trieNodePtr->tcamAllocInfo.ruleNode->trieNode ;                   \
        else                                                                                            \
            boundingSonTrieNodePtr = trieNodePtr->tcamAllocInfo.tokenNode->trieNode;

#define  PRV_CPSS_DXCH_LPM_TCAM_ALLOCATE_TOKEN_NODE_MAC(tcamManagementMode,trieNodePtr)\
        if((tcamManagementMode) == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)\
          {\
           (trieNodePtr)->tcamAllocInfo.tokenNode = (PRV_CPSS_DXCH_LPM_TCAM_TOKEN_NODE_STC *)cpssOsMalloc(\
                                                    sizeof(PRV_CPSS_DXCH_LPM_TCAM_TOKEN_NODE_STC));\
           if((trieNodePtr)->tcamAllocInfo.tokenNode == NULL)\
             {\
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);\
             }\
          }

#define  PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(tcamManagementMode,trieNodePtr)\
        if((tcamManagementMode) == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)\
          {\
           if((trieNodePtr)->tcamAllocInfo.tokenNode != NULL)\
             cpssOsFree((trieNodePtr)->tcamAllocInfo.tokenNode);\
          }

#define  PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_FOR_VALID_TRIE_MAC(tcamManagementMode,trieNodePtr)\
        if((tcamManagementMode) == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)\
          {\
          if(trieNodePtr->tcamAllocInfo.tokenNode != NULL)\
           if(!( ((trieNodePtr->father != NULL) && \
                 (trieNodePtr->tcamAllocInfo.tokenNode == trieNodePtr->father->tcamAllocInfo.tokenNode))||\
                ((trieNodePtr->leftSon != NULL) && \
                 (trieNodePtr->tcamAllocInfo.tokenNode == trieNodePtr->leftSon->tcamAllocInfo.tokenNode))||\
                ((trieNodePtr->rightSon != NULL) && \
                 (trieNodePtr->tcamAllocInfo.tokenNode == trieNodePtr->rightSon->tcamAllocInfo.tokenNode)) ) )\
           {\
               cpssOsFree(trieNodePtr->tcamAllocInfo.tokenNode);\
           }\
          }
#define  PRV_CPSS_DXCH_LPM_TCAM_FREE_ROOT_TOKEN_NODE_MAC(tcamManagementMode,trieNodePtr)\
        if((tcamManagementMode) == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)\
          {\
           if(trieNodePtr->tcamAllocInfo.tokenNode != NULL)\
               cpssOsFree(trieNodePtr->tcamAllocInfo.tokenNode);\
          }

/*******************************************************************************
* prvCpssDxChLpmTcamDispatcherRuleInfoUpdate
*
* DESCRIPTION:
*       Set TCAM allocate rule
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ruleUpdateAction        - rule update action
*       tcamManagementMode      - tcam management mode
*       srcTrieNodePtr          - trieNode which rule is taking from
*       dstTrieNodePtr          - trieNode which rule is updated
*       allocatedRulePtr        - allocated rule
*
* OUTPUTS:
*       None.
*
* RETURNS:
*      GT_OK        - on OK.
*      GT_BAD_PARAM - on bad ruleUpdateAction.
*      GT_BAD_VALUE - on bad value.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
 GT_STATUS prvCpssDxChLpmTcamDispatcherRuleInfoUpdate
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_ENT ruleUpdateAction,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_MODE_ENT        tcamManagementMode,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC          *srcTrieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC          *dstTrieNodePtr,
    IN  GT_PTR                                            allocatedRulePtr
);


/*******************************************************************************
* prvCpssDxChLpmTcamDispatcherRuleSet
*
* DESCRIPTION:
*       Set TCAM allocate rule
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       trieNodePtr             - trie node associated with tcam allocated rule
*       ipTcamShadowPtr         - pointer to ip shadow we are working on
*
* OUTPUTS:
*       tcamRulePtr             - pointer to TCAM rule allocation info.
*
* RETURNS:
*      GT_OK        - on OK
*      GT_FAIL      - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
 GT_STATUS prvCpssDxChLpmTcamDispatcherRuleSet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC           *ipTcamShadowPtr,
    OUT GT_PTR                                      *tcamRulePtr
);


/*******************************************************************************
* prvCpssDxChLpmTcamDispatchAllocate
*
* DESCRIPTION:
*       It decides which allocation mechanism is involved: TCAM manager or
*       TCAM handler.
*       Allocates TCAM entry for specific type of prefix. The entry is allocated
*       in a range that is bounded by two other entries (bounded in the meaning
*       of TCAM search order). The entry is allocated according to specific
*       allocation method.
*
*
* INPUTS:
*       ipTcamShadowPtr        - pointer to ip shadow we are working on.
*       entryType              - entry type to allocate
*       lowerBound             - lower bound; the allocated rule reside after
*                               this rule; if NULL then no lower bound
*       upperBound             - upper bound; the allocated rule reside before
*                               this rule; if NULL then no upper bound
*       tcamHandlerAllocMethod - TCAM handler allocation method
*       defragEnable           - whether to defrag entries in case there is no
*                                free place for this entry cause of entries
*                                fragmentation; relevant only if the TCAM entries
*                                handler was created with partitionEnable = GT_FALSE
*       trieNodePtr            - trieNode associated with new TCAM rule
*
* OUTPUTS:
*       tcamRulePtr           - the allocated rule node
*
* RETURNS:
*       GT_OK                     - if allocation succedded
*       GT_OUT_OF_PP_MEM          - if there is no space left between the upper
*                                   and lower bounds
*       GT_OUT_OF_RANGE           - if the indexes were out of range
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDispatchAllocate
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                      *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  GT_PTR                                                 lowerBound,
    IN  GT_PTR                                                 upperBound,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_METHOD_ENT            tcamHandlerAllocMethod,
    IN  GT_BOOL                                                defragEnable,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC               *trieNodePtr,
    OUT GT_PTR                                                 *tcamRulePtr
);



/*******************************************************************************
* prvCpssDxChLpmTcamDispatchFree
*
* DESCRIPTION:
*       Frees a TCAM entry.
*
* INPUTS:
*       ipTcamShadowPtr       - pointer to ip shadow we are working on.
*       entryType             - entry type
*       trieNodePtr           - trie node associated with the entry to free.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if allocation succedded.
*       GT_NOT_FOUND             - if the rule wasn't found.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDispatchFree
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                      *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC               *trieNodePtr
);




/*******************************************************************************
* prvCpssDxChLpmTcamDispatcherCompRulesPos
*
* DESCRIPTION:
*       compares two rules in terms of position.
*
* INPUTS:
*       ipTcamShadowPtr        - pointer to ip shadow we are working on.
*       firstTcamRulePtr       - pointer to tcam rule.
*       secondTcamRulePtr      - pointer to tcam rule.
*
* OUTPUTS:
*       resultPtr              - compare result GT_EQUAL , GT_GREATER , or GT_SMALLER.
*
* RETURNS:
*       GT_OK                 - if compare succedded
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDispatcherCompRulesPos
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                    *ipTcamShadowPtr,
    IN  GT_PTR                                               *firstTcamRulePtr,
    IN  GT_PTR                                               *secondTcamRulePtr,
    OUT GT_COMP_RES                                          *resultPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDispatchCheckAvailableMem
*
* DESCRIPTION:
*       compares two rules in terms of position.
*
* INPUTS:
*       ipTcamShadowPtr        - pointer to ip shadow we are working on.
*       entryType              - entry type
*       numOfRulesNeeded       - number of entries needed
*       requestedEntriesArray  - array of amount of entries requested by client
*       sizeOfArray            - number of elemenets in the array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if OK
*       GT_BAD_PARAM          - if wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDispatchCheckAvailableMem
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                     *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT entryType,
    IN  GT_U32                                                numOfRulesNeeded,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC               *requestedEntriesArray,
    IN  GT_U32                                                sizeOfArray
);

/*******************************************************************************
* prvCpssDxChLpmTcamDispatchGetEntryHsuInfo
*
* DESCRIPTION:
*       Gets HSU information for entry.
*
* INPUTS:
*       ipTcamShadowPtr       - pointer to ip shadow we are working on
*       entryType             - entry type
*       trieNodePtr           - trie node associated with the entry
*
* OUTPUTS:
*       hsuInfoPtr            - points to hsu information
*
* RETURNS:
*       GT_OK                    - if allocation succedded.
*       GT_NOT_FOUND             - if the rule wasn't found.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDispatchGetEntryHsuInfo
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                      *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC               *trieNodePtr,
    OUT GT_32                                                  *hsuInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDispatchSetEntryHsuInfo
*
* DESCRIPTION:
*       Sets HSU information for entry.
*
* INPUTS:
*       ipTcamShadowPtr       - pointer to ip shadow we are working on
*       entryType             - entry type
*       hsuInfo               - hsu information
*
* OUTPUTS:
*       tcamRulePtr           - the allocated rule node
*
* RETURNS:
*       GT_OK                    - if allocation succedded.
*       GT_NOT_FOUND             - if the rule wasn't found.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDispatchSetEntryHsuInfo
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                      *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  GT_U32                                                 hsuInfo,
    OUT GT_PTR                                                 *tcamRulePtr
);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmTcamDispatcherh */

