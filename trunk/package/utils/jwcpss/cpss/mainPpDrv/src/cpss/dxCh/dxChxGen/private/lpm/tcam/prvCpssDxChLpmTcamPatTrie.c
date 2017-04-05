/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamPatTrie.c
*
* DESCRIPTION:
*       This file includes implementation of a patricia trie for the purpose
*       of hierarchy managing to the Ip tree structure
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 64 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamMgm.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamPatTrie.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamEntriesHandler.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamDispatcher.h>


#define DEBUG_IP_PAT_TRIE

#undef DEBUG_IP_PAT_TRIE

#ifdef DEBUG_IP_PAT_TRIE
#define DBG_IP_PAT_TRIE_PRINT(x) cpssOsPrintf x
#else
#define DBG_IP_PAT_TRIE_PRINT(x)
#endif

static GT_U32 dbgTcamWrites = 0;
#ifdef DEBUG_IP_PAT_TRIE
static GT_U32 dbgNumOf = 0;
#endif

/*******************************************************************************
 * Local defs                                                                  *
 ******************************************************************************/

/*
 * typedef: struct PRV_CPSS_DXCH_TRAVERSE_DEL_FUN_EXTRA_DATA_STC
 *
 * Description: the ipPatTrieTraverseDelFun extra data.
 *
 *
 * Fields:
 *      delPrefixFunc - the del prefix function.
 *      tcamEntriesHandlerPtr - points to TCAM entries handler
 *      entryType             - entry type
 *      freeFunc      - the nodes free function.
 */
typedef struct PRV_CPSS_DXCH_TRAVERSE_DEL_FUN_EXTRA_DATA_STCT
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_DEL_PREFIX_FUN            delPrefixFunc;
    IN PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC             *tcamEntriesHandlerPtr;
    IN PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType;
    GT_VOIDFUNCPTR                                            freeFunc;
}PRV_CPSS_DXCH_TRAVERSE_DEL_FUN_EXTRA_DATA_STC;

/*******************************************************************************
* getIpBit
*
* DESCRIPTION:
*       returns the n- bit of an Ip GT_U8 array address
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipAddrPtr - ip address.
*       bitIndex  - the bit index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*      the value of the bit.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U8 getIpBit
(
    IN GT_U8  *ipAddrPtr,
    IN GT_32  bitIndex
)
{
    GT_U8 byteInd;
    GT_U8 bitInd;
    GT_U8 res;

    byteInd = (GT_U8)(bitIndex/8);
    bitInd = (GT_U8)(7 - (bitIndex %8));
    res =  (GT_U8)((ipAddrPtr[byteInd] >> bitInd) & 0x1);
    return res;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieCopyIpAddr
*
* DESCRIPTION:
*       copies one Ip GT_U8 array address to  another
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       destIpAddrPtr    - (pointer to)destination ip address.
*       sourceIpAddrPtr  - (pointer to)source ip address.
*       startBitSource   - start bit index to copy from.
*       endBitSource     - end bit index to copy from.
*       startBitDest     - start bit to copy to in the destination.
*
* OUTPUTS:
*       destIpAddrPtr    - (pointer to)destination ip address.
*       sourceIpAddrPtr  - (pointer to)source ip address.
*
* RETURNS:
*      None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssDxChLpmTcamPatTrieCopyIpAddr
(
    INOUT GT_U8 *destIpAddrPtr,
    INOUT GT_U8 *sourceIpAddrPtr,
    IN GT_U32   startBitSource,
    IN GT_U32   endBitSource,
    IN GT_U32   startBitDest
)
{
    GT_U32 iD, iS;

    GT_U32 endOfBits;

    endOfBits = startBitDest + endBitSource - startBitSource ;


    for (iD = startBitDest, iS = startBitSource; iD < endOfBits; iD++, iS++)
    {
        destIpAddrPtr[iD/8] &= ~(1 << (7 - (iD%8)));
        destIpAddrPtr[iD/8] |= getIpBit(sourceIpAddrPtr,iS) << (7 - (iD%8));
    }
    return;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieGetBoundingSon
*
* DESCRIPTION:
*        retrive the bounding son from below the given trie node (bounding it
*        as the bounding place it can move down the trie , rule position wise)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       trieNodePtr    - the trie node .
*       ipTcamShadowPtr        - pointer to ip shadow we are working on.
*
* OUTPUTS:
*       boundingSonTrieNodePtrPtr - pointer to boundingSonTrieNode.
*
* RETURNS:
*      the bounding son.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieGetBoundingSon
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *trieNodePtr,
    IN  struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT   *ipTcamShadowPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    **boundingSonTrieNodePtrPtr
)
{
    GT_COMP_RES result;
    GT_STATUS ret = GT_OK;
    GT_PTR leftSonRulePtr = NULL;
    GT_PTR rightSonRulePtr = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *boundTrieNodePtr = NULL;

    if ((trieNodePtr->leftSon == NULL) && (trieNodePtr->rightSon != NULL))
    {
        PRV_CPSS_DXCH_LPM_TCAM_SET_BOUNDING_SON_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                    trieNodePtr->rightSon,
                                                    boundTrieNodePtr);
         *boundingSonTrieNodePtrPtr = boundTrieNodePtr;
    }
    else if ((trieNodePtr->rightSon == NULL) && (trieNodePtr->leftSon != NULL))
    {
        PRV_CPSS_DXCH_LPM_TCAM_SET_BOUNDING_SON_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                    trieNodePtr->leftSon,
                                                    boundTrieNodePtr);
        *boundingSonTrieNodePtrPtr = boundTrieNodePtr;

    }
    else if ((trieNodePtr->rightSon != NULL) && (trieNodePtr->leftSon != NULL))
    {
        ret = prvCpssDxChLpmTcamDispatcherRuleSet(trieNodePtr->leftSon,
                                                  ipTcamShadowPtr,
                                                  &leftSonRulePtr);
        if (ret != GT_OK)
        {
            return ret;
        }
        ret = prvCpssDxChLpmTcamDispatcherRuleSet(trieNodePtr->rightSon,
                                                  ipTcamShadowPtr,
                                                  &rightSonRulePtr);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* check which son is in bounding position*/
        ret = prvCpssDxChLpmTcamDispatcherCompRulesPos(ipTcamShadowPtr,
                                                       leftSonRulePtr,
                                                       rightSonRulePtr,
                                                       &result);
        if (ret != GT_OK)
        {
            return ret;
        }

        if (result == GT_GREATER)
        {
            /* bounding son is taking from leftSon trie node */
            PRV_CPSS_DXCH_LPM_TCAM_SET_BOUNDING_SON_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                        trieNodePtr->leftSon,
                                                        boundTrieNodePtr);
        }
        else
        {
            /* bounding son is taking from rightSon trie node */
            PRV_CPSS_DXCH_LPM_TCAM_SET_BOUNDING_SON_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                        trieNodePtr->rightSon,
                                                        boundTrieNodePtr);
        }
    }
    else
    {
        boundTrieNodePtr = NULL;
    }
    *boundingSonTrieNodePtrPtr = boundTrieNodePtr;
    return ret;
}

/*******************************************************************************
* setNonValidRuleNodeBoundingSon
*
* DESCRIPTION:
*       set the pointer to valid node bounding son's rule under this no-valid
*       rule. works up until it reaches a valid node.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       trieNodePtr            - the trie node to start working up from.
*       ipTcamShadowPtr        - the ip shadow we are working on.
*       indicateUpdateChildren - in the case of bulk insert, whether to
*                                to indicate in those update fathers that
*                                one of the children needs an update.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*      GT_OK   - on OK.
*      GT_FAIL - on FAIL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS setNonValidRuleNodeBoundingSon
(
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *trieNodePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *ipTcamShadowPtr,
    IN GT_BOOL                                   indicateUpdateChildren
)
{
    GT_STATUS ret = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *boundingSon  = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currTrieNode = trieNodePtr;

    /* look out for the highest position rule of both sons*/
    while ((currTrieNode != NULL) && (currTrieNode->pData == NULL))
    {
        /* retrive bounding son */
        ret= prvCpssDxChLpmTcamPatTrieGetBoundingSon(currTrieNode,ipTcamShadowPtr,&boundingSon);
        if (ret != GT_OK)
        {
            return ret;
        }
        if (boundingSon == NULL)
        {
            /* shouldn't happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        /* non-valid rule updated by bounding son */
        ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_COPY_E,
                                                         ipTcamShadowPtr->tcamManagementMode,
                                                         boundingSon,
                                                         currTrieNode,
                                                         NULL);
        if (ret != GT_OK)
        {
            return ret;
        }
        if (indicateUpdateChildren == GT_TRUE)
            currTrieNode->updateStatus |= PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_CHILDREN_E;
        currTrieNode = currTrieNode->father;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieGetValidFather
*
* DESCRIPTION:
*       get the first valid father node of a give trie node.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       trieNodePtr    - the trie node .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*      GT_OK.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *prvCpssDxChLpmTcamPatTrieGetValidFather
(
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *trieNodePtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currTrieNode;
    if (trieNodePtr->father == NULL)
    {
        return NULL;
    }

    currTrieNode = trieNodePtr->father;

    while ((currTrieNode != NULL) && (currTrieNode->pData == NULL))
    {
        currTrieNode = currTrieNode->father;
    }

    return currTrieNode;
}

/*******************************************************************************
* recordOldNode
*
* DESCRIPTION:
*        incase of a bulk inserting , a node might overwrite another node
*        due to a push. record the overwritten node.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       trieNodePtr                - the overwriting node
*       oldTrieNodePtr             - the overwritten node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*      GT_OK  - if the push worked.
*      GT_OUT_OF_PP_MEM - if we found no free place.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS recordOldNode
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *oldTrieNodePtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC *oldNodeStc;
    GT_BOOL                                           nodeAlreadyOverwriting;

    nodeAlreadyOverwriting =
        ((trieNodePtr->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E) > 0)?
        GT_TRUE: GT_FALSE;

    /* since this node will take place of an old existing node we need to
       invalidate the space first */
    trieNodePtr->updateStatus |= PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E;

    /* since this node will take place of an old existing node
       record which one is it */
    if (nodeAlreadyOverwriting == GT_FALSE)
    {
        oldNodeStc = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC));
        if (oldNodeStc == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        oldNodeStc->pData = trieNodePtr->pData;
        oldNodeStc->oldNode = oldTrieNodePtr;
        trieNodePtr->pData = (GT_PTR)oldNodeStc;
    }
    else
    {
        oldNodeStc = (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC*)trieNodePtr->pData;
        oldNodeStc->oldNode = oldTrieNodePtr;
    }

    return GT_OK;
}


/*******************************************************************************
* releaseOldNodeRecord
*
* DESCRIPTION:
*        incase of a bulk inserting , a node might have overwriten another node
*        and the record of the that node needs to be freed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       trieNodePtr                - the node with the record
*
* OUTPUTS:
*       None.
*
* RETURNS:
*      GT_OK  - if the push worked.
*      GT_OUT_OF_PP_MEM - if we found no free place.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS releaseOldNodeRecord
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *trieNodePtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC *oldNodeStc;

    /* if it once took a place of an existing node , but not
       now. release him from recording it */
    oldNodeStc = (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC*)trieNodePtr->pData;
    trieNodePtr->pData = oldNodeStc->pData;
    cpssOsFree(oldNodeStc);

    return GT_OK;
}

/*******************************************************************************
* pushUpTrie
*
* DESCRIPTION:
*        push up the above fathers of this node, until you find a place ,or not.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId                - the virtual router id.
*       ipTcamShadowPtr     - the ip shadow we are working on.
*       entryType           - entry type
*       trieNodePtr         - the trie node to start from.
*       originalTrieNodePtr - the trie node which rule will be freed by this push
*       ipTcamFuncsPtr      - the utility functions to use for tcam access
*       isBulkInsert        - the type of insert this is: Bulk or regular.
*       defragEnable        - whether to defrag entries in case there is no
*                             free place for this entry cause of entries
*                             fragmentation; relevant only if the TCAM entries
*                             handler was created with partitionEnable = GT_FALSE
*
* OUTPUTS:
*       allocNodePtrPtr       - the rule that was freed from this push, if there was one.
*       isInvalidateNeededPtr - to indicate the higher level whether the place it
*                               got needs invalidting first.
*       oldNodePtrPtr         - if the place needs invalidating then there was a
*                               prefix there, this is the pointer for the prefix
*                               node.
*
* RETURNS:
*      GT_OK  - if the push worked.
*      GT_OUT_OF_PP_MEM - if we found no free place.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS pushUpTrie
(
    IN  GT_U32                                                vrId,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                     *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC              *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC              *originalTrieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC         *ipTcamFuncsPtr,
    IN  GT_BOOL                                               isBulkInsert,
    IN  GT_BOOL                                               defragEnable,
    OUT GT_PTR                                                *allocNodePtrPtr,
    OUT GT_BOOL                                               *isInvalidateNeededPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC              **oldNodePtrPtr
)
{
    GT_STATUS ret  = GT_OK;
    GT_PTR upperRulePtr = NULL;
    GT_PTR lowerRulePtr = NULL;
    GT_U32 sonsStackIdx = 1;
    GT_32 i;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC **sonsStack;/* stack to record the passed sons*/
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currValidFather,*currTrieNode = NULL;
    GT_BOOL invalidateFirst = GT_FALSE;
    GT_BOOL hadInvlidateFirst = GT_FALSE;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *oldNode = NULL, *preOldNode = NULL;
    GT_PTR moveFromTcamRulePtr = NULL;
    GT_PTR moveToTcamRulePtr = NULL;
    GT_PTR alocatedTcamRulePtr = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC *allocTempPtr = NULL;
    GT_PTR freeRule = NULL;

    sonsStack = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *)*128);/* at most 128 sons*/
    if (sonsStack == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    sonsStack[0] = trieNodePtr;
    currTrieNode = trieNodePtr;
    while (currTrieNode != NULL)
    {
        currValidFather = prvCpssDxChLpmTcamPatTrieGetValidFather(currTrieNode);
        if (currValidFather)
        {
            ret = prvCpssDxChLpmTcamDispatcherRuleSet(currValidFather,
                                                      ipTcamShadowPtr,
                                                      &upperRulePtr);
            if (ret != GT_OK)
            {
                cpssOsFree(sonsStack);
                return ret;
            }
        }
        else
            upperRulePtr = NULL;


        ret = prvCpssDxChLpmTcamDispatcherRuleSet(currTrieNode,
                                                  ipTcamShadowPtr,
                                                  &lowerRulePtr);
        if (ret != GT_OK)
        {
            cpssOsFree(sonsStack);
            return ret;
        }

        ret =  prvCpssDxChLpmTcamDispatchAllocate(ipTcamShadowPtr,
                                                  entryType,
                                                  lowerRulePtr,
                                                  upperRulePtr,
                                                  PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MID_E,
                                                  defragEnable,
                                                  currTrieNode,
                                                  &alocatedTcamRulePtr);

        if (ret == GT_OK)
        {
            break;
        }

        currTrieNode = currValidFather;
        sonsStack[sonsStackIdx] = currTrieNode;
        sonsStackIdx++;
    }

    if (alocatedTcamRulePtr == NULL)
    {
        cpssOsFree(sonsStack);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* now check if an empty place up the trie was found, and if so use it down*/
    if (ret == GT_OK)
    {
        for (i = sonsStackIdx-1; i>= 0; i--)
        {
            ret = prvCpssDxChLpmTcamDispatcherRuleSet(sonsStack[i],
                                                      ipTcamShadowPtr,
                                                      &freeRule);
            if (ret != GT_OK)
            {
                cpssOsFree(sonsStack);
                return ret;
            }

            /* moving entry! */
            if (isBulkInsert == GT_FALSE)
            {
                dbgTcamWrites++;
                /* set move from tcam Rule */
                moveFromTcamRulePtr = freeRule;
                /* set move to tcam Rule */
                moveToTcamRulePtr = alocatedTcamRulePtr;

                ret = ipTcamFuncsPtr->ipTcamMovePrefix(vrId,
                                                       moveFromTcamRulePtr,
                                                       moveToTcamRulePtr,
                                                       sonsStack[i], /* trie node with free rule */
                                                       *ipTcamFuncsPtr->workDevListPtr);
            }
            else
            {
                /* record whether this node used to have an invalidate first
                   flag */

                if ((sonsStack[i]->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E) == 0 )
                {
                    hadInvlidateFirst = GT_FALSE;
                }
                else
                {
                    hadInvlidateFirst = GT_TRUE;
                    preOldNode = ((PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC*)(sonsStack[i]->pData))->oldNode;
                }

                /* check if this node will have to invalidate the space it's
                   writing to before writing */
                if (invalidateFirst == GT_TRUE)
                {
                    ret = recordOldNode(sonsStack[i],oldNode);
                    if (ret != GT_OK)
                    {
                        cpssOsFree(sonsStack);
                        return ret;
                    }
                }
                else if (hadInvlidateFirst == GT_TRUE)
                {
                    sonsStack[i]->updateStatus &= ~PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E;
                    ret = releaseOldNodeRecord(sonsStack[i]);
                    if (ret != GT_OK)
                    {
                        cpssOsFree(sonsStack);
                        return ret;
                    }
                }

                /* now check if the node was already written in hw meaning the
                   one that takes it's place will need to invalidate the space
                   first , if it does record the old node*/

                if ((sonsStack[i]->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E) == 0)
                {
                    invalidateFirst = GT_TRUE;
                    oldNode = sonsStack[i];
                }
                else if (hadInvlidateFirst == GT_TRUE)
                {
                    invalidateFirst = GT_TRUE;
                    oldNode = preOldNode;
                }
                else
                    invalidateFirst = GT_FALSE;

                /* now idicate in was moved and now needs a hw update */

                sonsStack[i]->updateStatus |= PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E;
            }
            if (ret != GT_OK)
            {
                cpssOsFree(sonsStack);
                return ret;
            }

            if(ipTcamShadowPtr->tcamManagementMode ==
                    PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
            {
                ret = prvCpssDxChLpmTcamEntriesHandlerFree(ipTcamShadowPtr->tcamEntriesHandler,
                                                           entryType,
                                                           sonsStack[i]->tcamAllocInfo.ruleNode);
                if(ret != GT_OK)
                {
                    cpssOsFree(sonsStack);
                    return ret;
                }
            }

            ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_AND_TRIE_UPDATE_E,
                                                             ipTcamShadowPtr->tcamManagementMode,
                                                             NULL,
                                                             sonsStack[i],
                                                             alocatedTcamRulePtr);
            if(ret != GT_OK)
            {
                cpssOsFree(sonsStack);
                return ret;
            }

            ret = setNonValidRuleNodeBoundingSon(sonsStack[i]->father,
                                                 ipTcamShadowPtr,
                                                 GT_FALSE);
            if(ret != GT_OK)
            {
                cpssOsFree(sonsStack);
                return ret;
            }
            if(ipTcamShadowPtr->tcamManagementMode ==
                    PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
            {
                /* now alloc the freed one for the next node down */
                ret = prvCpssDxChLpmTcamEntriesHandlerAllocByGivenRule(ipTcamShadowPtr->tcamEntriesHandler,
                                                                       entryType,
                                                                       (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)freeRule,
                                                                       &allocTempPtr);
                alocatedTcamRulePtr = (GT_PTR)allocTempPtr;
            }
            else
            {
                alocatedTcamRulePtr = (GT_PTR)freeRule;
                ret = cpssDxChTcamManagerEntryClientCookieUpdate(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                                 ipTcamShadowPtr->clientId,
                                                                 (GT_U32)(sonsStack[i]->tcamAllocInfo.tokenNode->tcamManagerEntryToken),
                                                                 sonsStack[i]);
            }
            if (ret != GT_OK)
            {
                cpssOsFree(sonsStack);
                return ret;
            }
        }

        /* set the last freed rule, to the given allocNodePtrPtr*/
        *allocNodePtrPtr = alocatedTcamRulePtr;

        if(ipTcamShadowPtr->tcamManagementMode ==
                PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
        {
            ret = cpssDxChTcamManagerEntryClientCookieUpdate(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                             ipTcamShadowPtr->clientId,
                                                             (GT_U32)((GT_UINTPTR)alocatedTcamRulePtr),
                                                             (GT_PTR)originalTrieNodePtr);
        }
        *isInvalidateNeededPtr = invalidateFirst;
        *oldNodePtrPtr = oldNode;
    }
    cpssOsFree(sonsStack);
    return ret;
}

/*******************************************************************************
* checkNonValidFatherForNewSonsWithNotAllocRule
*
* DESCRIPTION:
*        check whether this node is non-valid father having "new" sons with
*        non-allocated rules
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       trieNodePtr       - the trie node .
*       ipTcamShadowPtr   - the ip shadow we are working on.
*
* OUTPUTS:
*       isNewSonExixstPtr - GT_TRUE: if there is any sone with non-allocated rooles.
*                         - GT_FALSE: otherwise.
* RETURNS:
*      GT_OK              - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS checkNonValidFatherForNewSonsWithNotAllocRule
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC        *ipTcamShadowPtr,
    OUT GT_BOOL                                  *isNewSonExixstPtr
)
{
    if(ipTcamShadowPtr->tcamManagementMode ==
                        PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
    {
        if( (trieNodePtr->pData == NULL)                 &&
            ( (trieNodePtr->leftSon != NULL &&
               trieNodePtr->leftSon->tcamAllocInfo.tokenNode->tcamManagerEntryToken == PRV_CPSS_DXCH_LPM_TCAM_NON_INITIALIZED_TOKEN_CNS)   ||
              (trieNodePtr->rightSon != NULL &&
               trieNodePtr->rightSon->tcamAllocInfo.tokenNode->tcamManagerEntryToken == PRV_CPSS_DXCH_LPM_TCAM_NON_INITIALIZED_TOKEN_CNS) )  )
        {
            *isNewSonExixstPtr = GT_TRUE;
        }
        else
            *isNewSonExixstPtr = GT_FALSE;
    }

    else
    {
        if( (trieNodePtr->pData == NULL)                 &&
            ( (trieNodePtr->leftSon != NULL &&
               trieNodePtr->leftSon->tcamAllocInfo.ruleNode == NULL)   ||
              (trieNodePtr->rightSon != NULL &&
               trieNodePtr->rightSon->tcamAllocInfo.ruleNode == NULL) )  )
        {
            *isNewSonExixstPtr = GT_TRUE;
        }
        else
            *isNewSonExixstPtr = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* pushDownTrie
*
* DESCRIPTION:
*        push down the below bounding sons of this node, until you find a place
*        ,or not.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId              - the virtual router id.
*       ipTcamShadowPtr   - the ip shadow we are working on.
*       entryType         - entry type
*       trieNodePtr       - the trie node to start from.
*       trieDestNodePtr   - trie node that allocation rule is making for
*       ipTcamFuncsPtr    - the utility functions to use for tcam access
*       isBulkInsert      - the type of insert this is: Bulk or regular.
*       defragEnable      - whether to defrag entries in case there is no
*                           free place for this entry cause of entries
*                           fragmentation; relevant only if the TCAM entries
*                           handler was created with partitionEnable = GT_FALSE
*
* OUTPUTS:
*       allocNodePtrPtr       - the rule that was freed from this push, if there was one.
*       isInvalidateNeededPtr - to indicate the higher level whether the place it
*                               got needs invalidting first.
*       oldNodePtrPtr         - if the place needs invalidating then there was a
*                               prefix there, this is the pointer for the prefix
*                               node.
*
* RETURNS:
*      GT_OK  - if the push worked.
*      GT_OUT_OF_PP_MEM - if we found no free place.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS pushDownTrie
(
    IN  GT_U32                                                  vrId,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                       *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC                *trieNodePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC                *trieDestNodePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC           *ipTcamFuncsPtr,
    IN  GT_BOOL                                                 isBulkInsert,
    IN  GT_BOOL                                                 defragEnable,
    OUT GT_PTR                                                  *allocNodePtrPtr,
    OUT GT_BOOL                                                 *isInvalidateNeededPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC                **oldNodePtrPtr
)
{
    GT_STATUS ret = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *boundingSon =NULL;
    GT_PTR lowerIdx = NULL;
    GT_PTR upperIdx = NULL;
    GT_PTR moveFromTcamRulePtr= NULL;
    GT_PTR moveToTcamRulePtr= NULL;
    GT_PTR alocatedTcamRulePtr = NULL;
    GT_BOOL invalidateFirst = GT_FALSE;
    GT_BOOL hadInvlidateFirst = GT_FALSE;
    GT_BOOL isNewSonWithNullRuleExist = GT_FALSE;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *oldNode = NULL, *preOldNode = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currTrieNode = trieNodePtr;
    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC *allocTempPtr = NULL;
    GT_PTR freeRule = NULL;

    while (currTrieNode != NULL)
    {
        /* get bounding son */
        ret = prvCpssDxChLpmTcamPatTrieGetBoundingSon(currTrieNode,ipTcamShadowPtr, &boundingSon);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* get lower rule */
        if (boundingSon == NULL )
        {
            lowerIdx = NULL;
        }
        else
        {
            ret = prvCpssDxChLpmTcamDispatcherRuleSet(boundingSon,
                                                      ipTcamShadowPtr,
                                                      &lowerIdx);
            if (ret != GT_OK)
            {
                return ret;
            }
        }
        /* get upper rule */
        ret = prvCpssDxChLpmTcamDispatcherRuleSet(currTrieNode,
                                                  ipTcamShadowPtr,
                                                  &upperIdx);
        if (ret != GT_OK)
        {
            return ret;
        }

        ret =  prvCpssDxChLpmTcamDispatchAllocate(ipTcamShadowPtr,
                                                  entryType,
                                                  lowerIdx,
                                                  upperIdx,
                                                  PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MID_E,
                                                  defragEnable,
                                                  currTrieNode,
                                                  &alocatedTcamRulePtr);
        if (ret == GT_OK)
        {
            break;
        }

        currTrieNode = boundingSon;
    }

    if (ret == GT_OK)
    {
        ret = GT_FAIL;
        /* we found the place, use it for the push */
        while (currTrieNode != NULL)
        {
            ret = prvCpssDxChLpmTcamDispatcherRuleSet(currTrieNode,
                                                      ipTcamShadowPtr,
                                                      &freeRule);
            if (ret != GT_OK)
            {
                return ret;
            }

            /* moving entry! */
            if (isBulkInsert == GT_FALSE)
            {
                dbgTcamWrites++;
                /* set move from tcam Rule */
                moveFromTcamRulePtr = freeRule;
                /* set move to tcam Rule */
                moveToTcamRulePtr = alocatedTcamRulePtr;
                ret = ipTcamFuncsPtr->ipTcamMovePrefix(vrId,
                                                       moveFromTcamRulePtr,
                                                       moveToTcamRulePtr,
                                                       currTrieNode, /* trie node with free rule */
                                                       *ipTcamFuncsPtr->workDevListPtr);

            }
            else
            {
                /* record whether this node used to have an invalidate first
                   flag */

                if ((currTrieNode->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E) == 0 )
                {
                    hadInvlidateFirst = GT_FALSE;
                }
                else
                {
                    hadInvlidateFirst = GT_TRUE;
                    preOldNode = ((PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC*)(currTrieNode->pData))->oldNode;
                }

                /* check if this node will have to invalidate the space it's
                   writing to before writing */
                if (invalidateFirst == GT_TRUE)
                {
                    ret = recordOldNode(currTrieNode,oldNode);
                    if (ret != GT_OK)
                    {
                        return ret;
                    }
                }
                else if (hadInvlidateFirst == GT_TRUE)
                {
                    currTrieNode->updateStatus &= ~PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E;
                    ret = releaseOldNodeRecord(currTrieNode);
                    if (ret != GT_OK)
                    {
                        return ret;
                    }
                }

                /* now check if the node was already written in hw meaning the
                   one that takes it's place will need to invalidate the space
                   first , if it does record the old node*/

                if ((currTrieNode->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E) == 0)
                {
                    invalidateFirst = GT_TRUE;
                    oldNode = currTrieNode;
                }
                else if (hadInvlidateFirst == GT_TRUE)
                {
                    invalidateFirst = GT_TRUE;
                    oldNode = preOldNode;
                }
                else
                    invalidateFirst = GT_FALSE;

                /* now idicate in was moved and now needs a hw update */

                currTrieNode->updateStatus |= PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E;
                ret = GT_OK;
            }
            if (ret != GT_OK)
                break;

            if(ipTcamShadowPtr->tcamManagementMode ==
                    PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
            {
                ret = prvCpssDxChLpmTcamEntriesHandlerFree(ipTcamShadowPtr->tcamEntriesHandler,
                                                           entryType,
                                                           currTrieNode->tcamAllocInfo.ruleNode);
                if(ret != GT_OK)
                    return ret;
            }
            ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_AND_TRIE_UPDATE_E,
                                                             ipTcamShadowPtr->tcamManagementMode,
                                                             NULL,
                                                             currTrieNode,
                                                             alocatedTcamRulePtr);
            if(ret != GT_OK)
                return ret;

            /* check if the father is non-valid and have sons with NULL rules*/
            ret = checkNonValidFatherForNewSonsWithNotAllocRule(currTrieNode->father,
                                                                ipTcamShadowPtr,
                                                                &isNewSonWithNullRuleExist);
            if(ret != GT_OK)
                return ret;

            /* since the node switched a rule we need to update anyone who
               recorded he's rule as the bounding son, plus indicate
               update children in the case of bulk insert*/

            /* update bounding son to all non valid nodes other than the node
               that is pushed down; that node that is pushed down is non-valid
               during the push down but will be update later to valid node */
            if (currTrieNode != trieNodePtr)
            {
                ret = setNonValidRuleNodeBoundingSon(currTrieNode->father,
                                                     ipTcamShadowPtr,
                                                     isBulkInsert);
                if(ret != GT_OK)
                    return ret;
            }
            if(ipTcamShadowPtr->tcamManagementMode ==
                    PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
            {
                /* allocate the index for the next node */
                ret = prvCpssDxChLpmTcamEntriesHandlerAllocByGivenRule(ipTcamShadowPtr->tcamEntriesHandler,
                                                                       entryType,
                                                                       (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)freeRule,
                                                                       &allocTempPtr);
                alocatedTcamRulePtr = (GT_PTR)allocTempPtr;
            }
            else
            {
                alocatedTcamRulePtr = (GT_PTR)freeRule;
                ret = cpssDxChTcamManagerEntryClientCookieUpdate(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                                 ipTcamShadowPtr->clientId,
                                                                 (GT_U32)(currTrieNode->tcamAllocInfo.tokenNode->tcamManagerEntryToken),
                                                                 (GT_PTR)currTrieNode );
            }

            if (ret != GT_OK)
                break;

            if (currTrieNode == trieNodePtr)
            {
                /* stop when we reach the trie node we started from */
                break;
            }
            boundingSon = currTrieNode;
            currTrieNode = prvCpssDxChLpmTcamPatTrieGetValidFather(currTrieNode);
        }

        /* set the last freed rule, to the given allocNodePtrPtr*/

            *allocNodePtrPtr = alocatedTcamRulePtr;
            if(ipTcamShadowPtr->tcamManagementMode ==
               PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
            {
                ret = cpssDxChTcamManagerEntryClientCookieUpdate(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                                 ipTcamShadowPtr->clientId,
                                                                 (GT_U32)((GT_UINTPTR)alocatedTcamRulePtr),
                                                                 (GT_PTR)trieDestNodePtr );
                if(ret != GT_OK)
                    return ret;
            }
            /* now allocation is completed */
            if ((isNewSonWithNullRuleExist == GT_TRUE) && (currTrieNode == trieNodePtr))
            {
                ret = setNonValidRuleNodeBoundingSon(currTrieNode->father,
                                                     ipTcamShadowPtr,
                                                     isBulkInsert);
                if(ret != GT_OK)
                    return ret;
            }

            *isInvalidateNeededPtr = invalidateFirst;
            *oldNodePtrPtr = oldNode;
    }

    return ret;
}



/*******************************************************************************
* ipPatTriePush
*
* DESCRIPTION:
*        allocate a place for the given trie node.
*
* INPUTS:
*       vrId              - the virtual router id.
*       ipTcamShadowPtr   - the ip shadow we are working on.
*       entryType         - entry type
*       boundingSonPtr    - the sons which bounds this node from below.
*       validFatherPtr    - valid father of trieNode
*       ipTcamFuncsPtr    - the utility functions to use for tcam access
*       isBulkInsert      - the type of insert this is: Bulk or regular.
*       isMoveOnly        - if GT_TRUE only prefix moving is done
*       defragEnable      - whether to defrag entries in case there is no
*                           free place for this entry cause of entries
*                           fragmentation; relevant only if the TCAM entries
*                           handler was created with partitionEnable = GT_FALSE
*
* OUTPUTS:
*       trieNodePtr           - the rule node of this trie node is updated
*       isInvalidateNeededPtr - to indicate the higher level whether the place it
*                               got needs invalidting first.
*       oldNodePtrPtr         - if the place needs invalidating then there was a
*                               prefix there, this is the pointer for the prefix
*                               node.
*
* RETURNS:
*      GT_OK               - if the allocation worked.
*      GT_OUT_OF_PP_MEM    - if we found no free place.
*      GT_FAIL             - if fatal error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipPatTriePush
(
    IN    GT_U32                                                vrId,
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                     *ipTcamShadowPtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT entryType,
    IN    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC              *boundingSonPtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC              *validFatherPtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC         *ipTcamFuncsPtr,
    IN    GT_BOOL                                               isBulkInsert,
    IN    GT_BOOL                                               isMoveOnly,
    IN    GT_BOOL                                               defragEnable,
    OUT   PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC              *trieNodePtr,
    OUT   GT_BOOL                                               *isInvalidateNeededPtr,
    OUT   PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC              **oldNodePtrPtr
)
{
    GT_STATUS ret = GT_FAIL;
    GT_PTR alocatedTcamRulePtr;
    /* we don't have a between place , try to push up the father
       if there is a father*/

    /* prevent compiler warnings */
    isMoveOnly = isMoveOnly;

    if (validFatherPtr != NULL)
    {
        ret = pushUpTrie(vrId,
                         ipTcamShadowPtr,
                         entryType,
                         validFatherPtr,
                         trieNodePtr,
                         ipTcamFuncsPtr,
                         isBulkInsert,
                         defragEnable,
                         &alocatedTcamRulePtr,
                         isInvalidateNeededPtr,
                         oldNodePtrPtr);
        if (ret == GT_OK)
        {
            ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_AND_TRIE_UPDATE_E,
                                                             ipTcamShadowPtr->tcamManagementMode,
                                                             NULL,
                                                             trieNodePtr,
                                                             alocatedTcamRulePtr);
            if (ret != GT_OK)
                return ret;
        }
    }

    /* if we got here with no free index , try to push down.
       push down we should do also if push up was not performed due to
       validFatherPtr == NULL */
    if (ret != GT_OK)
    {
        if (boundingSonPtr != NULL)
        {
            ret = pushDownTrie(vrId,
                               ipTcamShadowPtr,
                               entryType,
                               boundingSonPtr,
                               trieNodePtr,
                               ipTcamFuncsPtr,
                               isBulkInsert,
                               defragEnable,
                               &alocatedTcamRulePtr,
                               isInvalidateNeededPtr,
                               oldNodePtrPtr);
            if (ret != GT_OK)
            {
                return ret;
            }
            ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_AND_TRIE_UPDATE_E,
                                                             ipTcamShadowPtr->tcamManagementMode,
                                                             NULL,
                                                             trieNodePtr,
                                                             alocatedTcamRulePtr);
            if (ret != GT_OK)
                return ret;
            DBG_IP_PAT_TRIE_PRINT(("Rule allocated = %d\n", trieNodePtr->tcamAllocInfo.ruleNode->offset));
        }
    }
    return ret;
}






/*******************************************************************************
* ipPatTrieNodeAllocRule
*
* DESCRIPTION:
*        allocate a place for the given trie node.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId              - the virtual router id.
*       ipTcamShadowPtr   - the ip shadow we are working on.
*       entryType         - entry type
*       trieNodePtr       - the trie node to start from.
*       boundingSonPtr    - the sons which bounds this node from below.
*       allocMethod       - the allocation method to use.
*       ruleNodePtr       - if not NULL then this is the rule to allocate (HSU use)
*       isBulkInsert      - the type of insert this is: Bulk or regular.
*       ipTcamFuncsPtr    - the utility functions to use for tcam access
*       isMoveOnly        - if GT_TRUE only prefix moving is done
*       defragEnable      - whether to defrag entries in case there is no
*                           free place for this entry cause of entries
*                           fragmentation; relevant only if the TCAM entries
*                           handler was created with partitionEnable = GT_FALSE
*
* OUTPUTS:
*       trieNodePtr           - the trie node to start from.
*       isInvalidateNeededPtr - to indicate the higher level whether the place it
*                            got needs invalidting first.
*       oldNodePtrPtr         - if the place needs invalidating then there was a
*                            prefix there, this is the pointer for the prefix
*                            node.
*
* RETURNS:
*      GT_OK  - if the allocation worked.
*      GT_OUT_OF_PP_MEM - if we found no free place.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipPatTrieNodeAllocRule
(
    IN    GT_U32                                             vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                     *ipTcamShadowPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT entryType,
    INOUT PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC           *trieNodePtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC           *boundingSonPtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_METHOD_ENT        allocMethod,
    IN    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC               *ruleNodePtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC      *ipTcamFuncsPtr,
    IN    GT_BOOL                                            isBulkInsert,
    IN    GT_BOOL                                            isMoveOnly,
    IN    GT_BOOL                                            defragEnable,
    OUT   GT_BOOL                                            *isInvalidateNeededPtr,
    OUT   PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC           **oldNodePtrPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *validFather, *tmpBoundingSonPtr;
    GT_PTR upperIdx = NULL;
    GT_PTR lowerIdx = NULL;
    GT_PTR alocatedTcamRulePtr = NULL;

    GT_STATUS ret = GT_OK;

    /* now try to allocate a place for this prefix */
    /* look for the upper and lower bounds of this node */
    validFather = prvCpssDxChLpmTcamPatTrieGetValidFather(trieNodePtr);
    if (validFather != NULL)
    {
        ret = prvCpssDxChLpmTcamDispatcherRuleSet(validFather,
                                                  ipTcamShadowPtr,
                                                  &upperIdx);
        if (ret != GT_OK)
        {
            return ret;
        }

    }
    else
    {
        upperIdx = NULL;
    }
    if (boundingSonPtr != NULL)
    {
        /* If given bounding son is non-valid node then chnage it to 'real' bounding son */
        if(boundingSonPtr->pData == NULL)
        {
            ret = prvCpssDxChLpmTcamPatTrieGetBoundingSon(boundingSonPtr, ipTcamShadowPtr, &tmpBoundingSonPtr);
            if(ret != GT_OK)
                return ret;

            boundingSonPtr = tmpBoundingSonPtr;
        }

        ret = prvCpssDxChLpmTcamDispatcherRuleSet(boundingSonPtr,
                                                  ipTcamShadowPtr,
                                                  &lowerIdx);
        if (ret != GT_OK)
        {
            return ret;
        }
    }
    else
    {
        lowerIdx = NULL;
    }

    if(isMoveOnly == GT_FALSE)
    {
        DBG_IP_PAT_TRIE_PRINT(("Allocating rule\n"));

        if (ruleNodePtr == NULL)
        {
            /* try to alloc between the lower and the upper*/

            ret =  prvCpssDxChLpmTcamDispatchAllocate(ipTcamShadowPtr,
                                                      entryType,
                                                      lowerIdx,
                                                      upperIdx,
                                                      allocMethod,
                                                      defragEnable,
                                                      trieNodePtr,
                                                      &alocatedTcamRulePtr);
            if (ret == GT_OK)
            {
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_UPDATE_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 NULL,
                                                                 trieNodePtr,
                                                                 alocatedTcamRulePtr);
                if (ret != GT_OK)
                    return ret;
            }
        }
        else
        {
            if(ipTcamShadowPtr->tcamManagementMode ==
               PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
            {
                ret = prvCpssDxChLpmTcamEntriesHandlerAllocByGivenRule(ipTcamShadowPtr->tcamEntriesHandler,
                                                                       entryType,
                                                                       ruleNodePtr,
                                                                       &trieNodePtr->tcamAllocInfo.ruleNode);
                /* in this case it can't be that it wouldn't succeed*/
                if (ret != GT_OK)
                {
                    /* fatal error ! */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                DBG_IP_PAT_TRIE_PRINT(("Rule allocated = %d\n", trieNodePtr->tcamAllocInfo.ruleNode->offset));
            }
            else
            {
                trieNodePtr->tcamAllocInfo.tokenNode->tcamManagerEntryToken = (GT_UINTPTR)ruleNodePtr;
                ret = cpssDxChTcamManagerEntryClientCookieUpdate(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                                 ipTcamShadowPtr->clientId,
                                                                 (GT_U32)(trieNodePtr->tcamAllocInfo.tokenNode->tcamManagerEntryToken),
                                                                 (GT_PTR)trieNodePtr);
                    if (ret != GT_OK)
                    {
                        /* fatal error ! */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
            }
        }
    }
    if ( (ret == GT_OUT_OF_PP_MEM)||  /* allocation was fail     */
         (isMoveOnly == GT_TRUE) )    /* only move was requested */
    {
        /* we don't have a between place , try to push make push */
        ret = ipPatTriePush(vrId,
                            ipTcamShadowPtr,
                            entryType,
                            boundingSonPtr,
                            validFather,
                            ipTcamFuncsPtr,
                            isBulkInsert,
                            isMoveOnly,
                            defragEnable,
                            trieNodePtr,
                            isInvalidateNeededPtr,
                            oldNodePtrPtr);

    }

    return ret;

}

/******************************************************************************
* prvCpssDxChLpmTcamPatTrieInsert
*
* DESCRIPTION:
*       Insert a new NEXT_POINTER_PTR to the ip trie structure.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId                    - the virtual router id.
*       rootPtr                 - Root of the trie to insert the entry.
*       ipTcamShadowPtr         - the ip shadow we are working on.
*       entryType               - entry type
*       ipAddrPtr               - IP address associated with the given entry.
*       prefixLen               - prefix of ipAddr.
*       ecmpPrefixLen           - the "ECMP prefix" length (the route num of the ecmp next
*                                 hop)
*       entry                    - The entry to be inserted.
*       isMaxPrefix              - is this prefix is at max prefix legnth for this protocol
*                                  stack.
*       isBulkInsert             - the type of insert this is: Bulk or regular.
*       defragEnable             - whether to defrag entries in case there is no
*                                  free place for this entry cause of entries
*                                  fragmentation; relevant only if the TCAM entries
*                                  handler was created with partitionEnable = GT_FALSE
*       ruleNodePtr              - if this isn't NULL then this is the rule node which this
*                                  prefix should occupy (HSU use)
*       ipTcamFuncsPtr           - the utility functions to use for tcam access
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if succeeded,
*       GT_OUT_OF_CPU_MEM if failed to allocate memory,
*       GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieInsert
(
    IN GT_U32                                        vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *rootPtr,
    IN struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT     *ipTcamShadowPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT    entryType,
    IN GT_U8                                         *ipAddrPtr,
    IN GT_U32                                        prefixLen,
    IN GT_U8                                         ecmpPrefixLen,
    IN GT_PTR                                        entry,
    IN GT_BOOL                                       isMaxPrefix,
    IN GT_BOOL                                       isBulkInsert,
    IN GT_BOOL                                       defragEnable,
    IN PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC          *ruleNodePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipTcamFuncsPtr
)
{
    GT_U32 numOfBytes;
    GT_U32 i,sizeAddition = 0;
    GT_U8  keySuffix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U8  currPrefix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 keySuffixLen =0,currPrefixLen;
    GT_U8  rootSuffix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 rootSuffixLen;
    GT_BOOL isEcmpPrefixSearch = GT_FALSE;
    GT_BOOL invalidatePrefix = GT_FALSE;
    OUT PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *oldNodePtr;

    GT_STATUS ret = GT_OK;
    GT_STATUS ret2 = GT_OK;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *newRootTrie,*newKeyTrie;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *boundingSon,*keepOldBoundingSon;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC **contTrie;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_METHOD_ENT allocMethod =
        PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MID_E;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currTrieNode;
    GT_PTR tcamRulePtr = NULL;

    DBG_IP_PAT_TRIE_PRINT(("Adding :\n"));
    DBG_IP_PAT_TRIE_PRINT(("    Addr : "));
#ifdef DEBUG_IP_PAT_TRIE
    for (i=0; i<prefixLen; i+=8)
    {
        DBG_IP_PAT_TRIE_PRINT(("%03d:", ipAddrPtr[i/8]));
    }
#endif
    DBG_IP_PAT_TRIE_PRINT((" / %d\n", prefixLen));

    if (rootPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (entry == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (rootPtr->size == 0)
    {
        /*special handle for empty trie */
        numOfBytes = ((prefixLen == 0)? 0: (prefixLen-1) / 8) + 1;
        rootPtr->size = 1;
        cpssOsMemCpy(rootPtr->ipAddr,ipAddrPtr,sizeof(GT_U8)*numOfBytes);
        rootPtr->prefixLen = prefixLen;
        rootPtr->pData = entry;
        PRV_CPSS_DXCH_LPM_TCAM_ALLOCATE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                       rootPtr);
        rootPtr->prefixFlagType = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ADDR_E;
        rootPtr->vrId = vrId;
        rootPtr->entryType = entryType;
        rootPtr->ipTcamFuncsPtr = ipTcamFuncsPtr;
        rootPtr->ipTcamShadowPtr = ipTcamShadowPtr;
        /* check if we need to allocate a rule or we have a given one */
        if (ruleNodePtr == NULL)
        {
                ret =  prvCpssDxChLpmTcamDispatchAllocate(ipTcamShadowPtr,
                                                          entryType,
                                                          NULL,
                                                          NULL,
                                                          PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MAX_E,
                                                          defragEnable,
                                                          rootPtr,
                                                          &tcamRulePtr);
        }
        else
        {
            if(ipTcamShadowPtr->tcamManagementMode ==
                                PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_ENTRIES_HANDLER_MODE_E)
            {
                ret = prvCpssDxChLpmTcamEntriesHandlerAllocByGivenRule(ipTcamShadowPtr->tcamEntriesHandler,
                                                                       entryType,
                                                                       ruleNodePtr,
                                                                       &rootPtr->tcamAllocInfo.ruleNode);
                tcamRulePtr = (GT_PTR)rootPtr->tcamAllocInfo.ruleNode;
            }
            else
            {
                tcamRulePtr = ruleNodePtr;
                ret = cpssDxChTcamManagerEntryClientCookieUpdate(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                                 ipTcamShadowPtr->clientId,
                                                                 (GT_U32)((GT_UINTPTR)tcamRulePtr),
                                                                 (GT_PTR)rootPtr);
                if (ret != GT_OK)
                    return ret;
            }
        }

        if (ret != GT_OK)
        {
            return ret;
        }
        /* update allocated rule */
        ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_UPDATE_E,
                                                         ipTcamShadowPtr->tcamManagementMode,
                                                         NULL,
                                                         rootPtr,
                                                         tcamRulePtr);
        if (ret != GT_OK)
            return ret;

        /* update TCAM rule data */
        ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                         ipTcamShadowPtr->tcamManagementMode,
                                                         rootPtr,
                                                         rootPtr,
                                                         NULL);
        if (ret != GT_OK)
            return ret;

        if (isBulkInsert == GT_FALSE)
        {
            dbgTcamWrites++;
            DBG_IP_PAT_TRIE_PRINT(("Adding root rule = %d\n", rootPtr->tcamAllocInfo.ruleNode->offset));

            ret = ipTcamFuncsPtr->ipTcamSetPrefix(vrId,
                                                  ipAddrPtr,
                                                  prefixLen,
                                                  ecmpPrefixLen,
                                                  tcamRulePtr,
                                                  ipTcamShadowPtr,
                                                  entry,
                                                  *ipTcamFuncsPtr->workDevListPtr);
            if (ret != GT_OK)
            {
                ret2 = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,rootPtr);
                if (ret2 != GT_OK)
                    return ret2;
                PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,rootPtr);
            }
        }
        else
        {
            rootPtr->updateStatus |=
                PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E | PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_NEW_INSERT_E;
        }
        return ret;
    }

    newRootTrie = NULL;

    cpssOsMemCpy(currPrefix,ipAddrPtr,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
    currPrefixLen = prefixLen;
    currTrieNode = rootPtr;

    while (currTrieNode!= NULL)
    {

        cpssOsMemSet(rootSuffix,0,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
        cpssOsMemSet(keySuffix,0,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);

        i =0;
        if ((isEcmpPrefixSearch == GT_FALSE) &&
            (currTrieNode->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
        {
            /* can not happen , we skipped over these entries */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        else if ((isEcmpPrefixSearch == GT_FALSE) ||
                 (currTrieNode->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
        {

            /*find the part which is the same in both prefixes */
            for (; (i+7 < currTrieNode->prefixLen) && (i+7 < currPrefixLen); i += 8)
            {
                if (currPrefix[i/8] != currTrieNode->ipAddr[i/8])
                {
                    break;
                }
            }

            for (; (i < currTrieNode->prefixLen) && (i < currPrefixLen); i++)
            {
                if (getIpBit(currPrefix,i) != getIpBit(currTrieNode->ipAddr,i))
                {
                    break;
                }
            }

            /* copy out what's left of the key prefix*/
            prvCpssDxChLpmTcamPatTrieCopyIpAddr(keySuffix,currPrefix,i,currPrefixLen,0);
            keySuffixLen = currPrefixLen-i;
        }

        if (i < currTrieNode->prefixLen) /* Split node*/
        {
            if (isEcmpPrefixSearch == GT_TRUE)
            {
                /* not allowed */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            /* copy out what's left of the rootPtr prefix */
            prvCpssDxChLpmTcamPatTrieCopyIpAddr(rootSuffix,currTrieNode->ipAddr,i,currTrieNode->prefixLen,0);
            rootSuffixLen = currTrieNode->prefixLen - i;

            /* create a new rootPtr node */
            newRootTrie = (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *)cpssOsMalloc(
                sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));
            if (newRootTrie == NULL)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
            }
            ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_RULE_E,
                                                             ipTcamShadowPtr->tcamManagementMode,
                                                             NULL,
                                                             newRootTrie,
                                                             NULL);
            if (ret != GT_OK)
            {
                return ret;
            }
            cpssOsMemCpy(newRootTrie->ipAddr,currTrieNode->ipAddr,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
            newRootTrie->prefixLen = i;
            newRootTrie->prefixFlagType = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ADDR_E;
            newRootTrie->size = currTrieNode->size;
            newRootTrie->father = currTrieNode->father;
            newRootTrie->updateStatus = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_STATUS_OK_E;
            newRootTrie->vrId = vrId;
            newRootTrie->entryType = entryType;
            newRootTrie->ipTcamFuncsPtr = ipTcamFuncsPtr;
            newRootTrie->ipTcamShadowPtr = ipTcamShadowPtr;
            /* if father exits update his son */
            if (newRootTrie->father != NULL)
            {
                if (newRootTrie->father->leftSon == currTrieNode)
                    newRootTrie->father->leftSon = newRootTrie;
                else
                    newRootTrie->father->rightSon = newRootTrie;
            }
            /* Because currTrieNode's bounding son may be removed to different location then keep it. */
            ret = prvCpssDxChLpmTcamPatTrieGetBoundingSon(currTrieNode, ipTcamShadowPtr, &keepOldBoundingSon);
            if(ret != GT_OK)
                return ret;

            /* check if a new node needs to be created */
            if (i < currPrefixLen)
            {
                /* create a new key node */
                newKeyTrie = (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *)cpssOsMalloc(
                    sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));
                if (newKeyTrie == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }
                newKeyTrie->size = 1;
                cpssOsMemCpy(newKeyTrie->ipAddr,keySuffix,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
                newKeyTrie->prefixLen = keySuffixLen;
                newKeyTrie->leftSon = NULL;
                newKeyTrie->rightSon = NULL;
                newKeyTrie->pData = entry;
                PRV_CPSS_DXCH_LPM_TCAM_ALLOCATE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                               newKeyTrie);
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INIT_TOKEN_RULE_INFO_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 NULL,
                                                                 newKeyTrie,
                                                                 NULL);
                if (ret != GT_OK)
                {
                    return ret;
                }
                /* in split mode isEcmpPrefixSearch must be GT_FALSE and therefore
                   prefixFlagType must be regular prefix and can't be ECMP */
                newKeyTrie->prefixFlagType = (GT_U8)PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ADDR_E;
                newKeyTrie->father = newRootTrie;
                newKeyTrie->vrId = vrId;
                newKeyTrie->entryType = entryType;
                newKeyTrie->ipTcamFuncsPtr = ipTcamFuncsPtr;
                newKeyTrie->ipTcamShadowPtr = ipTcamShadowPtr;
                /* the new rootPtr node is non-valid node */
                newRootTrie->pData = NULL;

                allocMethod = (isMaxPrefix == GT_TRUE)?
                    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MIN_E:
                    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MID_E;

                /* now try to allocate a place for this prefix */
                ret = ipPatTrieNodeAllocRule(vrId,
                                             ipTcamShadowPtr,
                                             entryType,
                                             newKeyTrie,
                                             NULL,
                                             allocMethod,
                                             ruleNodePtr,
                                             ipTcamFuncsPtr,
                                             isBulkInsert,
                                             GT_FALSE,
                                             defragEnable,
                                             &invalidatePrefix,
                                             &oldNodePtr);

                if (ret == GT_OK)
                {

                    /* two nodes were added to this node */
                    sizeAddition = 2;

                    /* writing node */
                    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                                     ipTcamShadowPtr->tcamManagementMode,
                                                                     newKeyTrie,
                                                                     newKeyTrie,
                                                                     NULL);
                    if (ret != GT_OK)
                        return ret;

                    if (isBulkInsert == GT_FALSE)
                    {
                        dbgTcamWrites++;
                        DBG_IP_PAT_TRIE_PRINT(("Adding prefix rule = %d\n", newKeyTrie->tcamAllocInfo.ruleNode->offset));
                        ret = prvCpssDxChLpmTcamDispatcherRuleSet(newKeyTrie,
                                                                  ipTcamShadowPtr,
                                                                  &tcamRulePtr);
                        if (ret != GT_OK)
                        {
                            return ret;
                        }

                        ret = ipTcamFuncsPtr->ipTcamSetPrefix(vrId,
                                                              ipAddrPtr,
                                                              prefixLen,
                                                              ecmpPrefixLen,
                                                              tcamRulePtr,
                                                              ipTcamShadowPtr,
                                                              entry,
                                                              *ipTcamFuncsPtr->workDevListPtr);
                        if (ret != GT_OK)
                        {
                            ret2 = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,newKeyTrie);
                            if (ret2 != GT_OK)
                                return ret2;
                        }
                        newKeyTrie->updateStatus = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_STATUS_OK_E;
                    }
                    else
                    {
                        newKeyTrie->updateStatus =
                            PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E | PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_NEW_INSERT_E;
                        if (invalidatePrefix == GT_TRUE)
                        {
                            ret = recordOldNode(newKeyTrie,oldNodePtr);
                            if (ret != GT_OK)
                                return ret;
                        }
                    }
                }
                if (ret != GT_OK)
                {
                    /* no empty space , release buffers and undo the
                       registrations */
                    if (currTrieNode->father != NULL)
                    {
                        if (currTrieNode->father->leftSon == newRootTrie)
                            currTrieNode->father->leftSon = currTrieNode;
                        else
                            currTrieNode->father->rightSon = currTrieNode;
                    }
                    PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                               newKeyTrie);
                    cpssOsFree(newKeyTrie);
                    cpssOsFree(newRootTrie);
                    break;
                }

                if (isBulkInsert == GT_TRUE)
                    /* the father has one of it's sons update , indicate it */
                    newRootTrie->updateStatus = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_CHILDREN_E;
            }
            else
            {
                /* the new rootPtr node will be used for this prefix */
                newKeyTrie = NULL;
                newRootTrie->pData = entry;
                PRV_CPSS_DXCH_LPM_TCAM_ALLOCATE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                               newRootTrie);
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INIT_TOKEN_RULE_INFO_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 NULL,
                                                                 newRootTrie,
                                                                 NULL);
                if (ret != GT_OK)
                {
                    return ret;
                }

                if ((isBulkInsert == GT_TRUE) &&
                    (currTrieNode->updateStatus != PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_STATUS_OK_E))
                    /* if the current should be update in the hw/or children
                       need update, it means now that one of the children needs
                       an update */
                        newRootTrie->updateStatus = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_CHILDREN_E;
                else
                    newRootTrie->updateStatus = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_STATUS_OK_E;

                /* now try to allocate a place for this prefix */
                ret = ipPatTrieNodeAllocRule(vrId,
                                             ipTcamShadowPtr,
                                             entryType,
                                             newRootTrie,
                                             currTrieNode,
                                             allocMethod,
                                             ruleNodePtr,
                                             ipTcamFuncsPtr,
                                             isBulkInsert,
                                             GT_FALSE,
                                             defragEnable,
                                             &invalidatePrefix,
                                             &oldNodePtr);

                if (ret == GT_OK)
                {


                    /* only one node was added to this node */
                    sizeAddition =1;

                    /* writing node */
                    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                                     ipTcamShadowPtr->tcamManagementMode,
                                                                     newRootTrie,
                                                                     newRootTrie,
                                                                     NULL);
                    if (ret != GT_OK)
                        return ret;

                    if (isBulkInsert == GT_FALSE)
                    {
                        dbgTcamWrites++;
                        DBG_IP_PAT_TRIE_PRINT(("Adding rule = %d\n", newRootTrie->tcamAllocInfo.ruleNode->offset));
                        ret = prvCpssDxChLpmTcamDispatcherRuleSet(newRootTrie,
                                                                  ipTcamShadowPtr,
                                                                  &tcamRulePtr);
                        if (ret != GT_OK)
                        {
                            return ret;
                        }

                        ret = ipTcamFuncsPtr->ipTcamSetPrefix(vrId,
                                                              ipAddrPtr,
                                                              prefixLen,
                                                              ecmpPrefixLen,
                                                              tcamRulePtr,
                                                              ipTcamShadowPtr,
                                                              entry,
                                                              *ipTcamFuncsPtr->workDevListPtr);
                        if (ret != GT_OK)
                        {
                            ret2 = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,newRootTrie);
                            if (ret2 != GT_OK)
                                return ret2;
                        }
                    }
                    else
                    {
                        newRootTrie->updateStatus |=
                            PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E | PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_NEW_INSERT_E;
                        if (invalidatePrefix == GT_TRUE)
                        {
                            ret = recordOldNode(newRootTrie,oldNodePtr);
                            if (ret != GT_OK)
                                return ret;
                        }
                    }
                }

                if (ret != GT_OK)
                {
                    /* no empty space , release buffers and undo the
                       registrations */
                    if (currTrieNode->father != NULL)
                    {
                        if (currTrieNode->father->leftSon == newRootTrie)
                            currTrieNode->father->leftSon = currTrieNode;
                        else
                            currTrieNode->father->rightSon = currTrieNode;
                    }
                    PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                               newRootTrie);
                    cpssOsFree(newRootTrie);
                    break;
                }
            }

            /* now after the allocation succeded update everything*/
            /* update the current node */

            cpssOsMemCpy(currTrieNode->ipAddr,rootSuffix,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
            currTrieNode->prefixLen = rootSuffixLen;
            currTrieNode->father = newRootTrie;
            if (getIpBit(rootSuffix,0) == 1)
            {
                newRootTrie->leftSon = newKeyTrie;
                newRootTrie->rightSon = currTrieNode;

            }
            else
            {
                newRootTrie->leftSon = currTrieNode;
                newRootTrie->rightSon = newKeyTrie;
            }
            /* In this case if currTrieNode is not valid, it is possible that
            it's bounding son moved to different tcam-position, so need to
            update correctly all bounding sons of non-valid nodes between
            currTrieNode and its bouding son including currTrieNode itself. */
            if(currTrieNode->pData == NULL)
            {
                ret = setNonValidRuleNodeBoundingSon(keepOldBoundingSon->father,
                                                     ipTcamShadowPtr,
                                                     isBulkInsert);
                if(ret != GT_OK)
                    return ret;
            }


            /* set the current node to the new rootPtr for the size, bounding son
               update */
            currTrieNode = newRootTrie;

            break;
        }
        else
        {
            /* first check for exact match, meaing overwrite */
            if (((isEcmpPrefixSearch == GT_FALSE) && (i == currPrefixLen) &&
                 (ecmpPrefixLen == 0)) ||
                ((isEcmpPrefixSearch == GT_TRUE) && (i == currPrefixLen)))
            {
                if (currTrieNode->pData != NULL)
                {
                    if (isBulkInsert == GT_FALSE)
                    {
                        /* overwrite the existing entry */
                        DBG_IP_PAT_TRIE_PRINT(("Overriding rule to root = %d\n", rootPtr->tcamAllocInfo.ruleNode->offset));
                        ret = prvCpssDxChLpmTcamDispatcherRuleSet(currTrieNode,
                                                                  ipTcamShadowPtr,
                                                                  &tcamRulePtr);
                        if (ret != GT_OK)
                        {
                            return ret;
                        }

                        ret =
                            ipTcamFuncsPtr->ipTcamUpdateDataIdx(currTrieNode->pData,
                                                                entry,
                                                                tcamRulePtr,
                                                                GT_TRUE,
                                                                ipTcamShadowPtr,
                                                                *ipTcamFuncsPtr->workDevListPtr);
                    }
                    else
                    {
                        /* an update happend */
                        currTrieNode->updateStatus |=
                            PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E | PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_NEW_INSERT_E;
                        ret = GT_OK;
                    }

                    currTrieNode->pData = entry;
                    break;

                }

                /* we found the place for the prefix */
                /* now try to allocate a place for this prefix */

                /* retrive bounding son from below*/
                ret= prvCpssDxChLpmTcamPatTrieGetBoundingSon(currTrieNode,ipTcamShadowPtr,&boundingSon);
                if (ret != GT_OK)
                {
                    return ret;
                }

                PRV_CPSS_DXCH_LPM_TCAM_ALLOCATE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                               currTrieNode);
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INIT_TOKEN_RULE_INFO_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 NULL,
                                                                 currTrieNode,
                                                                 NULL);

                ret = ipPatTrieNodeAllocRule(vrId,
                                             ipTcamShadowPtr,
                                             entryType,
                                             currTrieNode,
                                             boundingSon,
                                             allocMethod,
                                             ruleNodePtr,
                                             ipTcamFuncsPtr,
                                             isBulkInsert,
                                             GT_FALSE,
                                             defragEnable,
                                             &invalidatePrefix,
                                             &oldNodePtr);

                if (ret != GT_OK)
                {
                    /* no place found */
                    break;
                }

                /* writing node */
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 currTrieNode,
                                                                 currTrieNode,
                                                                 NULL);
                if (ret != GT_OK)
                    return ret;

                currTrieNode->pData = entry;

                if (isBulkInsert == GT_FALSE)
                {
                    dbgTcamWrites++;
                    DBG_IP_PAT_TRIE_PRINT(("Adding rule = %d\n", currTrieNode->tcamAllocInfo.ruleNode->offset));
                    ret = prvCpssDxChLpmTcamDispatcherRuleSet(currTrieNode,
                                                              ipTcamShadowPtr,
                                                              &tcamRulePtr);
                    if (ret != GT_OK)
                    {
                        return ret;
                    }

                    ret = ipTcamFuncsPtr->ipTcamSetPrefix(vrId,
                                                          ipAddrPtr,
                                                          prefixLen,
                                                          ecmpPrefixLen,
                                                          tcamRulePtr,
                                                          ipTcamShadowPtr,
                                                          entry,
                                                          *ipTcamFuncsPtr->workDevListPtr);
                }
                else
                {
                    currTrieNode->updateStatus |=
                        PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E | PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_NEW_INSERT_E;
                    if (invalidatePrefix == GT_TRUE)
                    {
                        ret = recordOldNode(currTrieNode,oldNodePtr);
                        if (ret != GT_OK)
                            return ret;
                    }
                }
                /* In this case (override), and since it is possible that bounding son of currTrieNode moved
                to different tcam-position, so need to update correctly all bounding sons of non-valid nodes
                between currTrieNode and its bouding son. */
                ret = setNonValidRuleNodeBoundingSon(boundingSon->father,
                                                     ipTcamShadowPtr,
                                                     isBulkInsert);
                if (ret != GT_OK)
                    return ret;

                break;

            }
            else if (((isEcmpPrefixSearch == GT_FALSE) &&
                      (i == currPrefixLen) && (ecmpPrefixLen > 0)))
            {
                /* this means we reached the ecmp route num 0, and from here
                   on were adding a "ECMP prefix" node */
                isEcmpPrefixSearch = GT_TRUE;
                keySuffix[0] = 0xFF;
                keySuffixLen = ecmpPrefixLen;
            }


            if (isEcmpPrefixSearch == GT_FALSE)
            {
                /* skip over ecmp prefix nodes when searching an address */
                while ((currTrieNode->rightSon != NULL) &&
                       (currTrieNode->rightSon->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
                {
                    currTrieNode = currTrieNode->rightSon;
                }
            }

            contTrie = (getIpBit(keySuffix,0) == 0)?
                &currTrieNode->leftSon : &currTrieNode->rightSon;

            if (isBulkInsert == GT_TRUE)
            {
                /* since we are going through this node we will update one of it's
                   children, thus indicate it */

                currTrieNode->updateStatus |= PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_CHILDREN_E;
            }

            if ((*contTrie == NULL) ||
                /* if we're in ecmp prefix addion and we reached a regular prefix
                   node , we need to add in the ecmp node */
                ((isEcmpPrefixSearch == GT_TRUE) && (*contTrie != NULL) &&
                ((*contTrie)->prefixFlagType != PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E)))
            {
                if ((isEcmpPrefixSearch == GT_FALSE) && (ecmpPrefixLen != 0))
                {
                    /* no support to add an ecmp route 1-8 before 0*/
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                /* create a new key node */
                newKeyTrie = (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *)cpssOsMalloc(
                    sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));
                if (newKeyTrie == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                }
                newKeyTrie->updateStatus = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_STATUS_OK_E;
                cpssOsMemCpy(newKeyTrie->ipAddr,keySuffix,
                             PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
                newKeyTrie->prefixLen = keySuffixLen;
                newKeyTrie->vrId = vrId;
                newKeyTrie->entryType = entryType;
                newKeyTrie->ipTcamFuncsPtr = ipTcamFuncsPtr;
                newKeyTrie->ipTcamShadowPtr = ipTcamShadowPtr;
                if (isEcmpPrefixSearch == GT_TRUE)
                {
                    newKeyTrie->size = currTrieNode->size;
                    newKeyTrie->leftSon = currTrieNode->leftSon;
                    newKeyTrie->rightSon = currTrieNode->rightSon;
                }
                else
                {
                    newKeyTrie->size = 1;
                    newKeyTrie->leftSon = NULL;
                    newKeyTrie->rightSon = NULL;
                }

                newKeyTrie->pData = entry;
                PRV_CPSS_DXCH_LPM_TCAM_ALLOCATE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                               newKeyTrie);
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INIT_TOKEN_RULE_INFO_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 NULL,
                                                                 newKeyTrie,
                                                                 NULL);
                if (ret != GT_OK)
                {
                    return ret;
                }
                newKeyTrie->prefixFlagType = (GT_U8)((isEcmpPrefixSearch == GT_TRUE) ?
                    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E : PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ADDR_E);
                newKeyTrie->father = currTrieNode;

                /* now try to allocate a place for this prefix */
                allocMethod = (isMaxPrefix == GT_TRUE)?
                    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MIN_E:
                    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MID_E;

                if (*contTrie == NULL)
                   boundingSon = NULL;
                else
                {
                    /* retrive bounding son from below*/
                    ret = prvCpssDxChLpmTcamPatTrieGetBoundingSon(currTrieNode,ipTcamShadowPtr,&boundingSon);
                    if (ret != GT_OK)
                    {
                       return ret;
                    }
                }

                ret = ipPatTrieNodeAllocRule(vrId,
                                             ipTcamShadowPtr,
                                             entryType,
                                             newKeyTrie,
                                             boundingSon,
                                             allocMethod,
                                             ruleNodePtr,
                                             ipTcamFuncsPtr,
                                             isBulkInsert,
                                             GT_FALSE,
                                             defragEnable,
                                             &invalidatePrefix,
                                             &oldNodePtr);

                if (ret != GT_OK)
                {
                    /* no place found , release buffers */
                    PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                               newKeyTrie);
                    cpssOsFree(newKeyTrie);
                    break;
                }


                /* writing node */
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 newKeyTrie,
                                                                 newKeyTrie,
                                                                 NULL);
                if (ret != GT_OK)
                    return ret;

                if (isBulkInsert == GT_FALSE)
                {
                    dbgTcamWrites++;
                    DBG_IP_PAT_TRIE_PRINT(("Adding rule = %d\n", newKeyTrie->tcamAllocInfo.ruleNode->offset));
                    ret = prvCpssDxChLpmTcamDispatcherRuleSet(newKeyTrie,
                                                              ipTcamShadowPtr,
                                                              &tcamRulePtr);
                    if (ret != GT_OK)
                    {
                        return ret;
                    }

                    ret = ipTcamFuncsPtr->ipTcamSetPrefix(vrId,
                                                          ipAddrPtr,
                                                          prefixLen,
                                                          ecmpPrefixLen,
                                                          tcamRulePtr,
                                                          ipTcamShadowPtr,
                                                          entry,
                                                          *ipTcamFuncsPtr->workDevListPtr);
                    if (ret != GT_OK)
                    {
                        ret2 = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,newKeyTrie);
                        if (ret2 != GT_OK)
                            return ret2;
                    }
                }
                else
                {
                    newKeyTrie->updateStatus |=
                        PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E | PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_NEW_INSERT_E;
                    if (invalidatePrefix == GT_TRUE)
                    {
                        ret = recordOldNode(newKeyTrie,oldNodePtr);
                        if (ret != GT_OK)
                            return ret;
                    }

                }
                if (ret != GT_OK)
                {
                    PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                               newKeyTrie);
                    cpssOsFree(newKeyTrie);
                    break;
                }


                /* now that everything succeded , update rootPtr and childs*/
                if (isEcmpPrefixSearch == GT_TRUE)
                {
                    currTrieNode->leftSon = NULL;
                    currTrieNode->rightSon = NULL;
                    /*if the sons exits update thier parent */
                    if (newKeyTrie->leftSon != NULL)
                    {
                        newKeyTrie->leftSon->father = newKeyTrie;
                    }
                    if (newKeyTrie->rightSon != NULL)
                    {
                        newKeyTrie->rightSon->father = newKeyTrie;
                    }
                }

                *contTrie = newKeyTrie;
                sizeAddition =1;
                break;
            }
        }
        currTrieNode = *contTrie;
        currPrefixLen = keySuffixLen;
        cpssOsMemCpy(currPrefix,keySuffix,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);


    }

    /* if the insert succeded update up */
    if (ret == GT_OK)
    {

        while (currTrieNode != NULL)
        {
            /* in the case of non valid node, record the higest index (bounding
               son) of the sons */
            if (currTrieNode->pData == NULL)
            {
                /*look for bounding son from below*/

                ret = prvCpssDxChLpmTcamPatTrieGetBoundingSon(currTrieNode,ipTcamShadowPtr,&boundingSon);
                if (ret != GT_OK)
                {
                    return ret;
                }

                if (boundingSon != NULL)
                {
                    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_COPY_E,
                                                                     ipTcamShadowPtr->tcamManagementMode,
                                                                     boundingSon,
                                                                     currTrieNode,
                                                                     NULL);
                    if (ret != GT_OK)
                    {
                        return ret;
                    }
                }
            }
            /* update size */
            currTrieNode->size = (GT_U16)(sizeAddition + currTrieNode->size);

            currTrieNode = currTrieNode->father;
        }

    }
    return ret;
}



/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieSearch
*
* DESCRIPTION:
*       Search for the entry which is associated with ipAddr & ipPrefix .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       rootPtr    - Root of the trie to search for the entry.
*       ipAddrPtr  - IP address associated with the given entry.
*       ipPrefix   - prefix of ipAddr.
*       entryPtr   - Indicates whether to return the entry in the searched node
*                   or not.
*
* OUTPUTS:
*       entryPtr      - A pointer to the searched entry if found, NULL otherwise.
*       numOfRulesPtr - the number of rules this prefix takes in the
*                       TCAM (for ecmp connected to prefixes)
*
* RETURNS:
*       A pointer to the node in the trie including the searched entry.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC* prvCpssDxChLpmTcamPatTrieSearch
(
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *rootPtr,
    IN GT_U8                              *ipAddrPtr,
    IN GT_U32                             prefixLen,
    OUT GT_U32                            *numOfRulesPtr,
    INOUT GT_PTR                          *entryPtr
)
{
    GT_U32 i;
    GT_U8  keySuffix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 keySuffixLen;
    GT_U8  currPrefix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 currPrefixLen;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currTrieNode,*resultTrieNode;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC **contTrie;

    cpssOsMemSet(keySuffix,0,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);

    cpssOsMemCpy(currPrefix,ipAddrPtr,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
    currPrefixLen = prefixLen;

    currTrieNode = rootPtr;

    DBG_IP_PAT_TRIE_PRINT(("Searching for :\n"));
    DBG_IP_PAT_TRIE_PRINT(("    Addr : "));
#ifdef DEBUG_IP_PAT_TRIE
    for (i=0; i<prefixLen; i+=8)
    {
        DBG_IP_PAT_TRIE_PRINT(("%03d:", ipAddrPtr[i/8]));
    }
#endif
    DBG_IP_PAT_TRIE_PRINT((" / %d\n", prefixLen));

    if ((rootPtr == NULL) || (rootPtr->size == 0))
        return NULL;

    while (currTrieNode != NULL)
    {
        /*find the part which is the same in both prefixes , start byte byte*/
        for (i =0; (i+7 < currTrieNode->prefixLen) && (i+7 < currPrefixLen); i += 8)
        {
            if (currPrefix[i/8] != currTrieNode->ipAddr[i/8])
            {
                break;
            }
        }
        /* move to bit, bit */
        for (; (i < currTrieNode->prefixLen) && (i < currPrefixLen); i++)
        {
            if (getIpBit(currPrefix,i) != getIpBit(currTrieNode->ipAddr,i))
            {
                break;
            }
        }
        /* first check that we reached the end of the node , if not then
           this prefix is not in the trie */
        if (i != currTrieNode->prefixLen)
        {
            return NULL;
        }

        /* from here (i == currTrieNode->prefixLen)*/

        /* copy out what's left of the key prefix*/
        prvCpssDxChLpmTcamPatTrieCopyIpAddr(keySuffix,currPrefix,i,currPrefixLen,0);
        keySuffixLen = currPrefixLen-i ;

        /* first check for exact match */
        if (i == currPrefixLen)
        {
            /* first check if there is an element */
            if (currTrieNode->pData == NULL)
            {
                return NULL;
            }

            /* retrive the entry */
            if (entryPtr != NULL)
            {
                *entryPtr = currTrieNode->pData;
            }

            resultTrieNode = currTrieNode;
            if (numOfRulesPtr != NULL)
            {
                /* calculate the number of rules this prefix takes
                   it could be one or more in case of ecmp */
                *numOfRulesPtr = 1;
                while ((currTrieNode->rightSon != NULL) &&
                       (currTrieNode->rightSon->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
                {
                    (*numOfRulesPtr)++;
                    currTrieNode = currTrieNode->rightSon;
                }
            }

            DBG_IP_PAT_TRIE_PRINT(("Found at rule = %d\n", resultTrieNode->tcamAllocInfo.ruleNode->offset));
            return resultTrieNode;
        }

        /* skip over ecmp prefix nodes when searching an address */
        while ((currTrieNode->rightSon != NULL) &&
               (currTrieNode->rightSon->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
        {
            currTrieNode = currTrieNode->rightSon;
        }

        contTrie = (getIpBit(keySuffix,0) == 0)?
                   &currTrieNode->leftSon:&currTrieNode->rightSon;

        currTrieNode = *contTrie;
        cpssOsMemCpy(currPrefix,keySuffix,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
        currPrefixLen = keySuffixLen;
    }
    return NULL;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieGetAddrFromNode
*
* DESCRIPTION:
*       construct an ip address from a given node
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       nodePtr       - the node form which to start constructing
*
* OUTPUTS:
*       ipAddrPtr       - IP address associated with the given node.
*       prefixLenPtr    - prefix of ipAddr.
*
* RETURNS:
*       GT_OK
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieGetAddrFromNode
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *nodePtr,
    OUT GT_U8                                     *ipAddrPtr,
    OUT GT_U32                                    *prefixLenPtr
)
{
    GT_U8  currPrefix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 currPrefixLen= 0;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC    *currTrieNode;

    currTrieNode = nodePtr;
    cpssOsMemSet(currPrefix, 0, sizeof(currPrefix));
    /* construct the ip address from the bottom */
    while (currTrieNode != NULL)
    {
        currPrefixLen += currTrieNode->prefixLen;

        prvCpssDxChLpmTcamPatTrieCopyIpAddr(currPrefix,currTrieNode->ipAddr,0,currTrieNode->prefixLen,
                   128-currPrefixLen);
        currTrieNode = currTrieNode->father;

        /* skip over ecmp prefix nodes */
        while ((currTrieNode != NULL) &&
               (currTrieNode->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
        {
            currTrieNode = currTrieNode->father;
        }
    }

    prvCpssDxChLpmTcamPatTrieCopyIpAddr(ipAddrPtr,currPrefix,128-currPrefixLen,128,0);
    *prefixLenPtr = currPrefixLen;
    return GT_OK;


}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieGetNext
*
* DESCRIPTION:
*       Search for the entry which is associated with larger ipAddr & ipPrefix .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       rootPtr         - Root of the trie to search for the entry.
*       ipAddrPtr       - IP address associated with the given entry.
*       prefixLenPtr    - prefix of ipAddr.
*       entryPtr        - Indicates whether to return the entry in the searched node
*                       or not.
*
* OUTPUTS:
*       ipAddrPtr    - the address of the next prefix
*       prefixLenPtr - the prefix length of the next prefix
*       entryPtr     - A pointer to the searched entry if found, NULL otherwise.
*
* RETURNS:
*       A pointer to the node in the trie including the next entry.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC* prvCpssDxChLpmTcamPatTrieGetNext
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *rootPtr,
    INOUT GT_U8                                    *ipAddrPtr,
    INOUT GT_U32                                   *prefixLenPtr,
    INOUT GT_PTR                                   *entryPtr
)
{
    GT_U32 i;
    GT_U8  keySuffix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 keySuffixLen;
    GT_U8  currPrefix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 currPrefixLen;
    GT_STATUS rc;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currTrieNode,*otherFather= NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *exactMatchNode = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC **contTrie;

    cpssOsMemSet(keySuffix,0,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
    cpssOsMemCpy(currPrefix,ipAddrPtr,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);

    currPrefixLen = *prefixLenPtr;

    currTrieNode = rootPtr;

    if ((rootPtr == NULL) || (rootPtr->size == 0))
        return NULL;

    /* first find the match entry place */
    while (currTrieNode != NULL)
    {
        /*find the part which is the same in both prefixes , start byte byte*/
        for (i =0; (i+7 < currTrieNode->prefixLen) && (i+7 < currPrefixLen); i += 8)
        {
            if (currPrefix[i/8] != currTrieNode->ipAddr[i/8])
            {
                break;
            }
        }
        /* move to bit, bit */
        for (; (i < currTrieNode->prefixLen) && (i < currPrefixLen); i++)
        {
            if (getIpBit(currPrefix,i) != getIpBit(currTrieNode->ipAddr,i))
            {
                break;
            }
        }
        /* first check that we reached the end of the node , if not then
           this prefix is not in the trie */
        if (i != currTrieNode->prefixLen)
        {
            break;
        }

        /* from here (i == currTrieNode->prefixLen)*/

        /* copy out what's left of the key prefix*/
        prvCpssDxChLpmTcamPatTrieCopyIpAddr(keySuffix,currPrefix,i,currPrefixLen,0);
        keySuffixLen = currPrefixLen-i ;

        /* first check for exact match */
        if (i == currPrefixLen)
        {
            /* first check if there is an element */
            if (currTrieNode->pData != NULL)
            {
                exactMatchNode = currTrieNode;

            }
            break;
        }

        /* skip over ecmp prefix nodes when searching an address */
        while ((currTrieNode->rightSon != NULL) &&
               (currTrieNode->rightSon->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
        {
            currTrieNode = currTrieNode->rightSon;
        }

        contTrie = (getIpBit(keySuffix,0) == 0)?
                   &currTrieNode->leftSon:&currTrieNode->rightSon;

        /* record another next father that if what we end in is non-children node */
        if ((contTrie == &currTrieNode->leftSon) && (currTrieNode->rightSon != NULL))
        {
            otherFather = currTrieNode->rightSon;
        }

        currTrieNode = *contTrie;


        cpssOsMemCpy(currPrefix,keySuffix,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
        currPrefixLen = keySuffixLen;
    }

    /* first decide where to start from */

    if ((currTrieNode != NULL) && (currTrieNode == exactMatchNode) &&
        (currTrieNode->leftSon == NULL) && (currTrieNode->rightSon == NULL))
    {
        currTrieNode = otherFather;
    }

    /* now look for the next */
    while ((currTrieNode != NULL) &&
           ((currTrieNode->pData == NULL) ||
            (currTrieNode == exactMatchNode)))
    {
        if (currTrieNode->leftSon != NULL)
        {
            currTrieNode = currTrieNode->leftSon;
        }
        else
        {
            currTrieNode = currTrieNode->rightSon;
        }
    }

    if (currTrieNode != NULL)
    {
        /* retrive the entry */
        if (entryPtr != NULL)
        {
            *entryPtr = currTrieNode->pData;
        }
        rc = prvCpssDxChLpmTcamPatTrieGetAddrFromNode(currTrieNode,ipAddrPtr,prefixLenPtr);
        if (rc != GT_OK)
        {
            return NULL;
        }
    }

    return currTrieNode;
}

/*******************************************************************************
* ipPatTrieTraverse
*
* DESCRIPTION:
*       a general pat trie DFS traverse function.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr     - ip shadow we are working on.
*       vrId                - the virtual router id.
*       rootPtr             - Root of the trie to search for the entry.
*       activateOnValidOnly - whether to preform the given functions on valid
*                             nodes only , or on all nodes.
*       downTraverseFunc    - the function to preform on the nodes on the way
*                             down. (can be NULL for no function )
*       upTraverseFunc      - the function to preform on the nodes on the way
*                             up. (can be NULL for no function )
*       invalidateTraverseFunc - the function to preform on the nodes on
*                             invalidation (can be NULL for no function )
*       extraData           - extra data to trasfer to the functions.
*       devListPtr          - pointer to the device list to preform the actions
*                             on.
*       preformOnRoot       - whether to preform the actions on the root node.
*       isUpdateOnly        - is this an update only or a whole trie action.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipPatTrieTraverse
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr,
    IN GT_U32                                           vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC         *rootPtr,
    IN GT_BOOL                                          activateOnValidOnly,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_TRAVERSE_FUN     downTraverseFunc,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_TRAVERSE_FUN     upTraverseFunc,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_TRAVERSE_FUN     invalidateTraverseFunc,
    IN GT_PTR                                           extraData,
    IN struct PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STCT   *devListPtr,
    IN GT_BOOL                                          preformOnRoot,
    IN GT_BOOL                                          isUpdateOnly
)
{
    GT_U8  currPrefix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 currPrefixLen;
    GT_U32 currentTriePrefixLen = 0;
    GT_STATUS ret;
    GT_BOOL wayDownWrite,wayUpWrite;
    GT_BOOL invalidateFirst = GT_FALSE;
    GT_COMP_RES compRuleRes;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC* oldNodeStc;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currTrieNode, *currParentTrieNode;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *nextTrieNode;
    GT_PTR currentRulePtr = NULL;
    GT_PTR oldRulePtr = NULL;
    if (rootPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (rootPtr->size == 0)
        return GT_OK;

    currTrieNode = rootPtr;

    cpssOsMemSet(currPrefix,0,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
    currPrefixLen = 0;

    if ((isUpdateOnly == GT_TRUE) && (invalidateTraverseFunc == NULL))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* first find the match entry place */
    while (currTrieNode != NULL)
    {
        /* copy out the prefix part from this node.*/
        prvCpssDxChLpmTcamPatTrieCopyIpAddr(currPrefix,currTrieNode->ipAddr,0,currTrieNode->prefixLen,
                   currPrefixLen);
        currPrefixLen += currTrieNode->prefixLen;

        nextTrieNode = NULL;

        if (isUpdateOnly == GT_TRUE)
        {
            if ((currTrieNode->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E) > 0)
            {
                invalidateFirst =
                    ((currTrieNode->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E) > 0)?
                    GT_TRUE:GT_FALSE;
                if (invalidateFirst == GT_TRUE)
                {
                    oldNodeStc = (PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_OLD_NODE_REC_STC*)currTrieNode->pData;
                    ret = prvCpssDxChLpmTcamDispatcherRuleSet(currTrieNode,
                                                              ipTcamShadowPtr,
                                                              &currentRulePtr);
                    if (ret != GT_OK)
                    {
                        return ret;
                    }

                    ret = prvCpssDxChLpmTcamDispatcherRuleSet(oldNodeStc->oldNode,
                                                              ipTcamShadowPtr,
                                                              &oldRulePtr);
                    if (ret != GT_OK)
                    {
                        return ret;
                    }

                    ret =  prvCpssDxChLpmTcamDispatcherCompRulesPos(ipTcamShadowPtr,
                                                                    currentRulePtr,
                                                                    oldRulePtr,
                                                                    &compRuleRes);
                    if (compRuleRes != GT_GREATER)
                    {
                        if (compRuleRes == GT_EQUAL)
                        {
                            /* this means the node didn't change place after all
                               no need to rewrite */
                            wayDownWrite = GT_FALSE;
                        }else
                            /* This means the overwriten node already been
                               written in it's new place and we can overwrite it
                               (with invalidating the place first ofcourse) */
                            wayDownWrite = GT_TRUE;
                    }
                    else
                        /* This means the overwriten node hasn't been written
                           yet in it's new place and we can't overwrite it */
                        wayDownWrite = GT_FALSE;

                    /* since we finished using the old place record we
                           release it */
                    releaseOldNodeRecord(currTrieNode);
                }
                else
                    /* the node is indicated for update and doesn't take no
                       one's place , just write it */
                    wayDownWrite = GT_TRUE;
            }
            else
                /* no need to write since the node isn't indicated for update
                   (invalidateFirst irrelevant) */
                wayDownWrite = GT_FALSE;

            if ((currTrieNode->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_NEW_INSERT_E) > 0)
                /* on any case a new node allways gets written after it's
                   children has been set in their place */
                wayDownWrite = GT_FALSE;

        }
        else
        {
            /* this is a regular (non bulk) update */
            invalidateFirst = GT_FALSE;
            wayDownWrite = GT_TRUE;
        }


        /* check for validty if needed then preform if needed the way down func */
        if (((preformOnRoot == GT_TRUE) || (currTrieNode != rootPtr)) &&
             (downTraverseFunc != NULL) &&
            ((activateOnValidOnly == GT_FALSE) || (currTrieNode->pData != NULL)) &&
            (wayDownWrite == GT_TRUE))
        {
            if (invalidateFirst == GT_TRUE)
            {
                ret = invalidateTraverseFunc(ipTcamShadowPtr,
                                             vrId,
                                             currPrefix,
                                             currPrefixLen,
                                             currTrieNode,
                                             extraData,
                                             devListPtr);
                if (ret != GT_OK)
                    return ret;

                currTrieNode->updateStatus &= ~PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E;
            }
            ret = downTraverseFunc(ipTcamShadowPtr,
                                   vrId,
                                   currPrefix,
                                   currPrefixLen,
                                   currTrieNode,
                                   extraData,devListPtr);
            if (ret != GT_OK)
            {
                return ret;
            }

            /* indicate this node is updated in the HW */
            currTrieNode->updateStatus &= ~PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E;
        }

        /* skip over ecmp prefix nodes when traversing */
        while ((currTrieNode->rightSon != NULL) &&
               (currTrieNode->rightSon->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
        {
            currTrieNode = currTrieNode->rightSon;
        }

        if ((isUpdateOnly == GT_FALSE) ||
            ((currTrieNode->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_CHILDREN_E) > 0))
        {
            /* in the case of an update only , there is no need to continue down
               the trie if the update status doesn't indcate that some of the
               children need an update */

            /* now go first to left ,if not to the right */
            nextTrieNode = (currTrieNode->leftSon != NULL)?
                           currTrieNode->leftSon :
                           (currTrieNode->rightSon != NULL)?
                           currTrieNode->rightSon : NULL;
        }
        else
            nextTrieNode = NULL;

        if (nextTrieNode == NULL)
        {
            /* it means we have to go up a level, until we find a right
               (not left) son we didn't use*/

            CPSS_COVERITY_NON_ISSUE_BOOKMARK
            /* coverity[check_after_deref] */
            while (currTrieNode != NULL)
            {
                /* we are going up */
                if (isUpdateOnly == GT_TRUE)
                {
                    if ((currTrieNode->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_HW_E) > 0)
                    {
                        invalidateFirst =
                            ((currTrieNode->updateStatus & PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_INVALIDATE_FIRST_E) > 0)?
                            GT_TRUE:GT_FALSE;

                        /* write it since there is still an indication to
                           write */
                        wayUpWrite = GT_TRUE;
                    }
                    else
                        /* it doesn't have an indcation to update hw
                        (invalidateFirst irrelevant) */
                        wayUpWrite = GT_FALSE;
                }
                else
                {
                    /* this is a regular (non bulk) update */
                    wayUpWrite = GT_TRUE;
                    invalidateFirst = GT_FALSE;
                }

                /* incase of an update , here we indicate this node and it's
                   children are done, and it's update status is ok (fully
                   updated) */
                if (isUpdateOnly == GT_TRUE)
                    currTrieNode->updateStatus = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_STATUS_OK_E;
                /* get the father */
                currParentTrieNode = currTrieNode->father;
                currentTriePrefixLen = currTrieNode->prefixLen;

                /* check for validty if needed then preform if needed the way down func */
                if (((preformOnRoot == GT_TRUE) || (currTrieNode != rootPtr)) &&
                    (upTraverseFunc != NULL) &&
                    ((activateOnValidOnly == GT_FALSE) || (currTrieNode->pData != NULL)) &&
                     (wayUpWrite == GT_TRUE))
                {
                    if (invalidateFirst == GT_TRUE)
                    {
                        ret = invalidateTraverseFunc(ipTcamShadowPtr,
                                                     vrId,
                                                     currPrefix,
                                                     currPrefixLen,
                                                     currTrieNode,
                                                     extraData,devListPtr);
                        if (ret != GT_OK)
                            return ret;
                    }
                    ret = upTraverseFunc(ipTcamShadowPtr,
                                         vrId,
                                         currPrefix,
                                         currPrefixLen,
                                         currTrieNode,
                                         extraData,
                                         devListPtr);
                    if (ret != GT_OK)
                    {
                        return ret;
                    }
                }


                /*deduct the prefix of this level*/
                currPrefixLen -= currentTriePrefixLen;

                if (currParentTrieNode == NULL)
                {
                    /* no father, we we're in the rootPtr,
                       finished with the traversing */
                    return GT_OK;
                }

                /* check if the right son isn't the one we've been */
                if ((currParentTrieNode->rightSon != NULL) &&
                    (currParentTrieNode->rightSon != currTrieNode))
                {
                    /* ok cool we didn't use this one */
                    currTrieNode = currParentTrieNode->rightSon;
                    break;
                }

                /* ok we finished with that level, go up */
                currTrieNode = currParentTrieNode;
            }
        }
        else
            /* continue down */
            currTrieNode = nextTrieNode;
    }

    return GT_OK;
}

/*******************************************************************************
* ipPatTrieTraverseWriteFun
*
* DESCRIPTION:
*       a traverse write function
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr     - ip shadow we are working on.
*       vrId        - the virtual router id.
*       ipAddrPtr      - IP Uc address
*       prefixLen   - Ip Uc prefix length
*       trieNodePtr - the trie node that we are currently at.
*       extraData   - extra passed data
*       devListPtr  - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipPatTrieTraverseWriteFun
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC        *ipTcamShadowPtr,
    IN GT_U32                                   vrId,
    IN GT_U8                                    *ipAddrPtr,
    IN GT_U32                                   prefixLen,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *trieNodePtr,
    IN GT_PTR                                   extraData,
    IN PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC   *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_SET_PREFIX_FUN setPrefixFunc;
    GT_PTR tcamLocationRulePtr = NULL;

    setPrefixFunc =
        ((PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC*)extraData)->ipTcamSetPrefix;

    ret = prvCpssDxChLpmTcamDispatcherRuleSet(trieNodePtr,
                                              ipTcamShadowPtr,
                                              &tcamLocationRulePtr);
    if (ret != GT_OK)
    {
        return ret;
    }


    DBG_IP_PAT_TRIE_PRINT(("Setting rule = %d\n", trieNodePtr->tcamAllocInfo.ruleNode->offset));
    ret = setPrefixFunc(vrId,
                        ipAddrPtr,
                        prefixLen,
                        0,
                        tcamLocationRulePtr,
                        ipTcamShadowPtr,
                        trieNodePtr->pData,
                        devListPtr);
    return ret;
}

/*******************************************************************************
* ipPatTrieTraverseInvalidateFun
*
* DESCRIPTION:
*       a traverse invalidate function
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr - pointer to ip shadow we are working on.
*       vrId            - the virtual router id.
*       ipAddrPtr       - IP Uc address
*       prefixLen       - Ip Uc prefix length
*       trieNodePtr     - the trie node that we are currently at.
*       extraData       - extra passed data
*       devListPtr      - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipPatTrieTraverseInvalidateFun
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC        *ipTcamShadowPtr,
    IN GT_U32                                   vrId,
    IN GT_U8                                    *ipAddrPtr,
    IN GT_U32                                   prefixLen,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *trieNodePtr,
    IN GT_PTR                                   extraData,
    IN PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC   *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_DEL_PREFIX_FUN delPrefixFunc;
    GT_PTR tcamRulePtr;
    /* fix warning */
    ipAddrPtr = ipAddrPtr;
    prefixLen = prefixLen;

    delPrefixFunc =
        ((PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC*)extraData)->ipTcamDelPrefix;
    ret = prvCpssDxChLpmTcamDispatcherRuleSet(trieNodePtr,
                                              ipTcamShadowPtr,
                                              &tcamRulePtr);
    if (ret != GT_OK)
    {
        return ret;
    }


    ret = delPrefixFunc(vrId,
                        tcamRulePtr,
                        ipTcamShadowPtr,
                        trieNodePtr->pData,
                        devListPtr);
    return ret;
}


/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieWriteToHw
*
* DESCRIPTION:
*       write the whole patricai tree to the HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr - the shadow to write the HW.
*       vrId            - the virtual router id.
*       rootPtr         - Root of the trie to search for the entry.
*       ipTcamFuncsPtr  - the utility functions to use for tcam access
*       isUpdateOnly    - is this an update only or a whole trie overwrite.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieWriteToHw
(
    IN struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT     *ipTcamShadowPtr,
    IN GT_U32                                        vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC      *rootPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC *ipTcamFuncsPtr,
    IN GT_BOOL                                       isUpdateOnly
)
{
    GT_STATUS ret;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_TRAVERSE_FUN upTraverseFunc;

    upTraverseFunc = (isUpdateOnly == GT_TRUE)? ipPatTrieTraverseWriteFun:NULL;

    ret = ipPatTrieTraverse(ipTcamShadowPtr,
                            vrId,
                            rootPtr,
                            GT_TRUE,
                            ipPatTrieTraverseWriteFun,
                            upTraverseFunc,
                            ipPatTrieTraverseInvalidateFun,
                            (GT_PTR)ipTcamFuncsPtr,
                            *ipTcamFuncsPtr->workDevListPtr,
                            GT_TRUE,
                            isUpdateOnly);
    return ret;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieDel
*
* DESCRIPTION:
*       Deletes a previously inserted entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId            - the virtual router id.
*       rootPtr         - Root of the trie to delete from.
*       ipTcamShadowPtr - the ip shadow we are working on.
*       entryType       - entry type
*       ipAddrPtr       - IP address associated with the entry to be deleted.
*       prefixLen       - prefix of ipAddr.
*       ecmpPrefixLen   - the "ECMP prefix" length (the route num of the ecmp next
*                       hop)
*       ipTcamFuncsPtr- the utility functions to use for tcam access
*
* OUTPUTS:
*       entryPtr   - A pointer to the entry of the deleted node if found, NULL
*                 otherwise.
* RETURNS:
*       GT_OK if succeeded, GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function does not free the pointers to the data stored in the
*       trie node, it's the responsibility of the caller to do that.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieDel
(
    IN GT_U32                                                  vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC                *rootPtr,
    IN struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT               *ipTcamShadowPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN GT_U8                                                   *ipAddrPtr,
    IN GT_U32                                                  prefixLen,
    IN GT_U8                                                   ecmpPrefixLen,
    OUT GT_PTR                                                 *entryPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC           *ipTcamFuncsPtr
)
{
    GT_STATUS ret = GT_NOT_FOUND;
    GT_U32 i,numOfDeletedNodes = 0;
    GT_U8  keySuffix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 keySuffixLen;
    GT_U8  currPrefix[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 currPrefixLen;
    GT_U32 usedPrefixLen;
    GT_BOOL isEcmpPrefixSearch = GT_FALSE;

    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *newRootTrie= NULL,*currTrieNode;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *currSonNode;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC **contTrie,**mergeTrie = NULL;
    GT_PTR tcamRulePtr = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *boundingSon =NULL;
    cpssOsMemSet(keySuffix,0,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);

    cpssOsMemCpy(currPrefix,ipAddrPtr,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
    currPrefixLen = prefixLen;

    currTrieNode = rootPtr;

    DBG_IP_PAT_TRIE_PRINT(("Deleting :\n"));
    DBG_IP_PAT_TRIE_PRINT(("    Addr : "));
#ifdef DEBUG_IP_PAT_TRIE
    for (i=0; i<prefixLen; i+=8)
    {
        DBG_IP_PAT_TRIE_PRINT(("%03d:", ipAddrPtr[i/8]));
    }
#endif
    DBG_IP_PAT_TRIE_PRINT((" / %d\n", prefixLen));

    if (rootPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (rootPtr->size == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    while (currTrieNode != NULL)
    {
        /*find the part which is the same in both prefixes , start byte byte*/
        for (i =0; (i+7 < currTrieNode->prefixLen) && (i+7 < currPrefixLen); i += 8)
        {
            if (currPrefix[i/8] != currTrieNode->ipAddr[i/8])
            {
                break;
            }
        }
        /* move to bit, bit */
        for (; (i < currTrieNode->prefixLen) && (i < currPrefixLen); i++)
        {
            if (getIpBit(currPrefix,i) != getIpBit(currTrieNode->ipAddr,i))
            {
                break;
            }
        }
        /* first check that we reached the end of the node , if not then
           this prefix is not in the trie */
        if (i != currTrieNode->prefixLen)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }

        /* from here (i == currTrieNode->prefixLen)*/

        /* copy out what's left of the key prefix*/
        prvCpssDxChLpmTcamPatTrieCopyIpAddr(keySuffix,currPrefix,i,currPrefixLen,0);
        keySuffixLen = currPrefixLen-i ;

        /* first check for exact match */
        if (((isEcmpPrefixSearch == GT_FALSE) && (i == currPrefixLen) &&
             (ecmpPrefixLen == 0)) ||
            ((isEcmpPrefixSearch == GT_TRUE) && (i == currPrefixLen)))
        {
            /* first check if there is an element */
            if (currTrieNode->pData == NULL)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
            }

            /* retrive the entry */
            if (entryPtr != NULL)
            {
                *entryPtr = currTrieNode->pData;
            }

            /* removing rule writing !*/
            dbgTcamWrites++;
            DBG_IP_PAT_TRIE_PRINT(("Deleting rule = %d\n",currTrieNode->tcamAllocInfo.ruleNode->offset));
            ret = prvCpssDxChLpmTcamDispatcherRuleSet(currTrieNode,
                                                      ipTcamShadowPtr,
                                                      &tcamRulePtr);
            if (ret != GT_OK)
            {
                return ret;
            }

            ret = ipTcamFuncsPtr->ipTcamDelPrefix(vrId,
                                                  tcamRulePtr,
                                                  ipTcamShadowPtr,
                                                  currTrieNode->pData,
                                                  *ipTcamFuncsPtr->workDevListPtr);

            if (ret != GT_OK)
            {
                return ret;
            }

            currTrieNode->pData = NULL;
            ret =  prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,currTrieNode);
            if (ret != GT_OK)
            {
                return ret;
            }
            ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                             ipTcamShadowPtr->tcamManagementMode,
                                                             NULL,
                                                             currTrieNode,
                                                             NULL);
            if (ret != GT_OK)
                return ret;

            /* free token node*/
             PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_FOR_VALID_TRIE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                                       currTrieNode);
             ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_RULE_E,
                                                              ipTcamShadowPtr->tcamManagementMode,
                                                              NULL,
                                                              currTrieNode,
                                                              NULL);
             if (ret != GT_OK)
             {
                 return ret;
             }

            if (((currTrieNode->leftSon != NULL) && (currTrieNode->rightSon == NULL))||
                ((currTrieNode->rightSon != NULL) && (currTrieNode->leftSon == NULL)))
            {
                /* if this node is left out with one child merge it*/
                mergeTrie = (currTrieNode->leftSon != NULL)?
                            &currTrieNode->leftSon:&currTrieNode->rightSon;
            }


            ret = GT_OK;
            break;
        }
        else if (((isEcmpPrefixSearch == GT_FALSE) &&
                  (i == currPrefixLen) && (ecmpPrefixLen > 0)))
        {
            /* this means we reached the ecmp route num 0, and from here
               on were adding a "ECMP prefix" node */
            isEcmpPrefixSearch = GT_TRUE;
            keySuffix[0] = 0xFF;
            keySuffixLen = ecmpPrefixLen;
        }

        if (isEcmpPrefixSearch == GT_FALSE)
        {
            /* skip over ecmp prefix nodes when searching an address */
            while ((currTrieNode->rightSon != NULL) &&
                   (currTrieNode->rightSon->prefixFlagType == PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
            {
                currTrieNode = currTrieNode->rightSon;
            }
        }

        contTrie = (getIpBit(keySuffix,0) == 0)?
                   &currTrieNode->leftSon:&currTrieNode->rightSon;

        currTrieNode = *contTrie;
        cpssOsMemCpy(currPrefix,keySuffix,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
        currPrefixLen = keySuffixLen;
    }

    if (ret == GT_OK)
    {
        currSonNode = NULL;
        /* check for the case where we deleted the last node in the trie */
        if ((currTrieNode->father == NULL) &&
            (currTrieNode->size == 1))
        {
            currTrieNode->size = 0;
        }
        /* if we found and deleted , merge up */
        else while (currTrieNode != NULL)
        {
            /* now check if son node was emptied */
            if ((currSonNode != NULL) &&
                (currSonNode->size == 1) &&
                (currSonNode->pData == NULL))
            {
                if (currSonNode == currTrieNode->rightSon)
                {
                    currTrieNode->rightSon = NULL;
                    mergeTrie = &currTrieNode->leftSon;
                }
                else
                {
                    currTrieNode->leftSon = NULL;
                    mergeTrie = &currTrieNode->rightSon;
                }

                /* free the deleted node */
                cpssOsFree(currSonNode);
                numOfDeletedNodes++;
            }

            /* update currnet node size */
            currTrieNode->size = (GT_U16)(currTrieNode->size - (GT_U16)numOfDeletedNodes);

            /* if we had a delete and this node doesn't hold data , combine it
               (unless this node has no father which means it is dummy MC source
               trie root) */
            if ((mergeTrie != NULL) &&
                (*mergeTrie != NULL) &&
                (currTrieNode->pData == NULL) &&
                (currTrieNode->father != NULL))
            {
                 /* if TCAM Manager mode is running, token node memory
                   should be freed before merge operation */

                PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_FOR_VALID_TRIE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                                          currTrieNode);
                newRootTrie = *mergeTrie;
                currTrieNode->pData = newRootTrie->pData;
                currTrieNode->prefixFlagType = newRootTrie->prefixFlagType;
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_COPY_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 newRootTrie,
                                                                 currTrieNode,
                                                                 NULL);
                if (ret != GT_OK)
                {
                    return ret;
                }

                if (currTrieNode->pData != NULL)
                {
                    if(ipTcamShadowPtr->tcamManagementMode == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
                    {
                        ret = cpssDxChTcamManagerEntryClientCookieUpdate(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                                         ipTcamShadowPtr->clientId,
                                                                         (GT_U32)(currTrieNode->tcamAllocInfo.tokenNode->tcamManagerEntryToken),
                                                                         (GT_PTR)currTrieNode);
                        if (ret != GT_OK)
                        {
                            return ret;
                        }
                    }

                    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                                     ipTcamShadowPtr->tcamManagementMode,
                                                                     currTrieNode,
                                                                     currTrieNode,
                                                                     NULL);
                    if (ret != GT_OK)
                        return ret;

                }
                currTrieNode->leftSon = newRootTrie->leftSon;
                currTrieNode->rightSon = newRootTrie->rightSon;
                if (currTrieNode->leftSon != NULL)
                    currTrieNode->leftSon->father = currTrieNode;
                if (currTrieNode->rightSon != NULL)
                    currTrieNode->rightSon->father = currTrieNode;

                usedPrefixLen = (isEcmpPrefixSearch == GT_TRUE)?
                    0: currTrieNode->prefixLen;

                prvCpssDxChLpmTcamPatTrieCopyIpAddr(currTrieNode->ipAddr,newRootTrie->ipAddr,0,
                           newRootTrie->prefixLen,usedPrefixLen);
                currTrieNode->prefixLen = usedPrefixLen + newRootTrie->prefixLen;
                currTrieNode->size = newRootTrie->size;
                cpssOsFree(newRootTrie);

                numOfDeletedNodes++;
            }

            if ((currTrieNode->pData == NULL) && (currTrieNode->size > 1) &&
                (currTrieNode->prefixLen != 0) &&
                (currTrieNode->leftSon != NULL) &&
                (currTrieNode->rightSon != NULL))
            {
                /* at this point if a node has no data (and it's not the deleted
                   one), it must be that it has two childs , take the lower one
                   and place a pointer in the this node*/

                /* retrive bounding son */
                ret= prvCpssDxChLpmTcamPatTrieGetBoundingSon(currTrieNode,ipTcamShadowPtr,&boundingSon);
                if (ret != GT_OK)
                {
                    return ret;
                }
                /* if bounfing son is changed the token node currTrieNode should be freed */
                PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_FOR_VALID_TRIE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                                          currTrieNode);
                ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_COPY_E,
                                                                 ipTcamShadowPtr->tcamManagementMode,
                                                                 boundingSon,
                                                                 currTrieNode,
                                                                 NULL);
                if (ret != GT_OK)
                {
                    return ret;
                }
            }
            mergeTrie = NULL;
            currSonNode = currTrieNode;
            currTrieNode = currTrieNode->father;
        }
    }
    return ret;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieInit
*
* DESCRIPTION:
*       Initialize a new Pat-Trie for the prefixes
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       rootPtrPtr      - the root of the new trie.
*
* RETURNS:
*       GT_OK if succeeded, GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieInit
(
    IN struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT            *ipTcamShadowPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC            **rootPtrPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *newTrie;
    GT_STATUS ret = GT_OK;
    if (rootPtrPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    newTrie = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));
    if (newTrie == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    cpssOsMemSet(newTrie,0,sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));

    newTrie->size = 0;
    newTrie->rightSon = NULL;
    newTrie->leftSon = NULL;
    newTrie->pData = NULL;
    newTrie->prefixLen = 0;
    newTrie->father = NULL;
    newTrie->updateStatus = PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_UPDATE_STATUS_OK_E;
    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_RULE_E,
                                                     ipTcamShadowPtr->tcamManagementMode,
                                                     NULL,
                                                     newTrie,
                                                     NULL);
    if (ret != GT_OK)
    {
        return ret;
    }
    *rootPtrPtr = newTrie;
    return GT_OK;

}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieDestroy
*
* DESCRIPTION:
*       Delete a Pat-Trie , only if it is an empty one
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr - the ip shadow we are working on.
*       rootPtr         - the root of the trie.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK if succeeded, GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieDestroy
(
    IN struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT            *ipTcamShadowPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC             *rootPtr
)
{
    if (rootPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* check whether the tree is empty */
    switch (rootPtr->size)
    {
    case 0:
        /*empty tree */
        break;
    case 1:
        if (( rootPtr->leftSon == NULL)
            && (rootPtr->rightSon == NULL)
            && (rootPtr->pData == NULL) )
        {
            /* it is ip mc source tree dummy */
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    PRV_CPSS_DXCH_LPM_TCAM_FREE_ROOT_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                    rootPtr);
    cpssOsFree(rootPtr);
    return GT_OK;
}

/*******************************************************************************
* ipPatTrieTraverseDelFun
*
* DESCRIPTION:
*       a traverse del function
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr - the shadow pointer we are working on
*       vrId            - the virtual router id.
*       ipAddrPtr       - IP Uc address
*       prefixLen       - Ip Uc prefix length
*       trieNodePtr     - the trie node that we are currently at.
*       extraData       - extra passed data
*       devListPtr      - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipPatTrieTraverseDelFun
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC              *ipTcamShadowPtr,
    IN GT_U32                                         vrId,
    IN GT_U8                                          *ipAddrPtr,
    IN GT_U32                                         prefixLen,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC       *trieNodePtr,
    IN GT_PTR                                         extraData,
    IN PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC         *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_DEL_PREFIX_FUN delPrefixFunc;
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType;
    GT_VOIDFUNCPTR freeFunc;
    GT_PTR tcamRulePtr = NULL;
    PRV_CPSS_DXCH_TRAVERSE_DEL_FUN_EXTRA_DATA_STC *delFunExtraData;

    /* fix warning */
    ipAddrPtr = ipAddrPtr;
    prefixLen = prefixLen;

    /* if this is an invalid node return ok */
    if (trieNodePtr->pData == NULL)
    {
        return GT_OK;
    }
    /* retrive the extra data */
    delFunExtraData = (PRV_CPSS_DXCH_TRAVERSE_DEL_FUN_EXTRA_DATA_STC*)extraData;

    delPrefixFunc = delFunExtraData->delPrefixFunc;
    entryType = delFunExtraData->entryType;
    freeFunc = delFunExtraData->freeFunc;

    dbgTcamWrites++;

    ret = prvCpssDxChLpmTcamDispatcherRuleSet(trieNodePtr,
                                              ipTcamShadowPtr,
                                              &tcamRulePtr);
    if (ret != GT_OK)
    {
        return ret;
    }


    ret = delPrefixFunc(vrId,
                        tcamRulePtr,
                        ipTcamShadowPtr,
                        trieNodePtr->pData,
                        devListPtr);

    if (ret != GT_OK)
    {
        return ret;
    }

    if (freeFunc != NULL)
    {
        freeFunc(trieNodePtr->pData);
    }

    trieNodePtr->pData = NULL;
    ret =  prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,entryType,trieNodePtr);
    if (ret != GT_OK)
    {
        return ret;
    }
    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                     ipTcamShadowPtr->tcamManagementMode,
                                                     NULL,
                                                     trieNodePtr,
                                                     NULL);
    if (ret != GT_OK)
    {
        return ret;
    }

    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_OLD_RULE_E,
                                                     ipTcamShadowPtr->tcamManagementMode,
                                                     NULL,
                                                     trieNodePtr,
                                                     NULL);
    return ret;
}


/*******************************************************************************
* ipPatTrieTraverseFreeFun
*
* DESCRIPTION:
*       a traverse free function
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr - the shadow pointer we are working on
*       vrId            - the virtual router id.
*       ipAddrPtr       - IP Uc address
*       prefixLen       - Ip Uc prefix length
*       trieNodePtr     - the trie node that we are currently at.
*       extraData       - extra passed data
*       devListPtr      - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
static GT_STATUS ipPatTrieTraverseFreeFun
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC        *ipTcamShadowPtr,
    IN GT_U32                                   vrId,
    IN GT_U8                                    *ipAddrPtr,
    IN GT_U32                                   prefixLen,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *trieNodePtr,
    IN GT_PTR                                   extraData,
    IN PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC   *devListPtr
)
{
    GT_STATUS ret = GT_OK;
    /* fix warnings */
    vrId = vrId;
    ipAddrPtr = ipAddrPtr;
    prefixLen = prefixLen;
    extraData = extraData;
    devListPtr = devListPtr;
    if (trieNodePtr->father != NULL)
    {
        if (trieNodePtr->father->leftSon == trieNodePtr)
        {
            trieNodePtr->father->leftSon = NULL;
        }
        else if (trieNodePtr->father->rightSon == trieNodePtr)
        {
            trieNodePtr->father->rightSon = NULL;
        }
        else
            /* can't be , has to be one of the two */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    /* this macro freed token node if father of this trie node is valid node */
    PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_FOR_VALID_TRIE_MAC(ipTcamShadowPtr->tcamManagementMode,
                                                              trieNodePtr);
    ret = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_INVALID_RULE_E,
                                                     ipTcamShadowPtr->tcamManagementMode,
                                                     NULL,
                                                     trieNodePtr,
                                                     NULL);
    if (ret != GT_OK)
    {
        return ret;
    }

    cpssOsFree(trieNodePtr);
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieFlush
*
* DESCRIPTION:
*       flushes all trie (along with deleting all data nodes).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       rootPtr         - Root of the trie to delete from.
*       ipTcamShadowPtr - the ip shadow we are working on
*       entryType       - entry type
*       ipTcamFuncsPtr  - the utility functions to use for tcam access
*       dataFreeFunc    - A pointer to a function to be called for deleteing the
*                         user data, or NULL if no free operation should be done
*                         on the user's data.
*       flushDefault    - whether to flush the default rule as well
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieFlush
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC               *rootPtr,
    IN  struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT              *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC          *ipTcamFuncsPtr,
    IN  GT_VOIDFUNCPTR                                         dataFreeFunc,
    IN  GT_BOOL                                                flushDefault
)
{
    GT_STATUS ret;
    PRV_CPSS_DXCH_TRAVERSE_DEL_FUN_EXTRA_DATA_STC delFunExtraData;

    /* set the extra data */
    delFunExtraData.delPrefixFunc = ipTcamFuncsPtr->ipTcamDelPrefix;
    delFunExtraData.tcamEntriesHandlerPtr = ipTcamShadowPtr->tcamEntriesHandler;
    delFunExtraData.entryType = entryType;
    delFunExtraData.freeFunc = dataFreeFunc;

    /* vrid is irrelevant since we're just invalidting rules -so set it 0*/
    ret = ipPatTrieTraverse(ipTcamShadowPtr,
                            0,
                            rootPtr,
                            GT_FALSE,
                            ipPatTrieTraverseDelFun,
                            ipPatTrieTraverseFreeFun,
                            NULL,
                            (GT_PTR)&delFunExtraData,
                            *ipTcamFuncsPtr->workDevListPtr,
                            flushDefault,
                            GT_FALSE);
    if (ret != GT_OK)
    {
        return ret;
    }

    rootPtr->size = 1;

    return ret;

}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieIsEmpty
*
* DESCRIPTION:
*       check whether the trie is empty
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       rootPtr - the root of the trie.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       whether the trie is empty
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL prvCpssDxChLpmTcamPatTrieIsEmpty
(
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *rootPtr
)
{
    if (rootPtr == NULL)
    {
        return GT_TRUE;
    }


    switch (rootPtr->size)
    {
    case 0:
        /*empty tree */
        return GT_TRUE;
    case 1:
        if (( rootPtr->leftSon == NULL)
            && (rootPtr->rightSon == NULL)
            && (rootPtr->pData == NULL) )
        {
            /* it is ip mc src tree dummy */
            return GT_TRUE;
        }
        else
        {
            return GT_FALSE;
        }
    default:
        break;

    }
    return GT_FALSE;

}

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode
*
* DESCRIPTION:
*       Retrieves the next node to process in post order DFS scan.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       nodePtrPtr        - points to the last node retrieved by the function;
*                           on first call, use the trie root
*
* OUTPUTS:
*       nodePtrPtr        - points to the next trie node to process
*       lastNodePtr       - GT_TRUE:  nodePtr is the last node to process in the trie
*                           GT_FALSE: there are additional nodes to process in the trie
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       1. The trie root is always the last node to be processed in post order DFS
*          scan. Therefore calling the function with the trie root is used as an
*          indication that this is the first call.
*       2. Using NULL as one of the pointers is not valid.
*       3. Use the following code skeleton to scan trie:
*              nodePtr = trieRootPtr;
*              isLastNode = GT_FALSE;
*              while (isLastNode == GT_FALSE)
*              {
*                  prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode(&nodePtr,&isLastNode);
*                  process node...
*              }
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieGetNextPostOrderTrieNode
(
    INOUT PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  **nodePtrPtr,
    OUT   GT_BOOL                                   *lastNodePtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(nodePtrPtr);
    CPSS_NULL_PTR_CHECK_MAC((*nodePtrPtr));
    CPSS_NULL_PTR_CHECK_MAC(lastNodePtr);

    if ((*nodePtrPtr)->father != NULL)
    {
        /* if nodePtrPtr is right son, then its father is next node to be processed */
        if ((*nodePtrPtr) == (((*nodePtrPtr)->father)->rightSon))
        {
            *nodePtrPtr = (*nodePtrPtr)->father;
            *lastNodePtr = ((*nodePtrPtr)->father == NULL) ? GT_TRUE : GT_FALSE;
            return GT_OK;
        }
        else /* node is left son of its father */
        {
            /* if nodePtrPtr is left son, and its father has no right son, then its
               father is next node to be processed */
            if ((*nodePtrPtr)->father->rightSon == NULL)
            {
                *nodePtrPtr = (*nodePtrPtr)->father;
                *lastNodePtr = ((*nodePtrPtr)->father == NULL) ? GT_TRUE : GT_FALSE;
                return GT_OK;
            }
            /* if nodePtrPtr is left son, and its father has right son, continue
               the DFS scan with the right son */
            else
            {
                *nodePtrPtr = (*nodePtrPtr)->father->rightSon;
            }
        }
    }

    /* find first node to process */
    while ((*nodePtrPtr)->leftSon != NULL)
        *nodePtrPtr = (*nodePtrPtr)->leftSon;
    while (GT_TRUE)
    {
        if ((*nodePtrPtr)->rightSon == NULL)
        {
            break;
        }
        else /* nodePtr has right son */
        {
            *nodePtrPtr = (*nodePtrPtr)->rightSon;
            while ((*nodePtrPtr)->leftSon != NULL)
                *nodePtrPtr = (*nodePtrPtr)->leftSon;
        }
    }

    /* check if the returned node is the root (meaning it is the last node) */
    *lastNodePtr = ((*nodePtrPtr)->father == NULL) ? GT_TRUE : GT_FALSE;

    return GT_OK;
};

/*******************************************************************************
* prvCpssDxChLpmTcamPatTrieRelocatePrefix
*
* DESCRIPTION:
*       Move a prefix from a specific location in the TCAM to another location
*       in the TCAM.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr       - the shadow pointer we are working on
*       prefixPtr             - the prefix to relocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - if success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPatTrieRelocatePrefix
(
    IN struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT  *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  *prefixPtr
)
{
    GT_STATUS                                       rc = GT_OK;
    GT_STATUS                                       rc2 = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC        *boundingSon;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC        *validFather;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC   *ipTcamFuncsPtr;
    GT_BOOL                                         isInvalidateNeeded;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC        *oldNodePtr;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC        prefixBeforeRelocation;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_METHOD_ENT     allocMethod;

    /* pointer to TCAM entries handler */
    GT_PTR oldTcamLocationRulePtr = NULL;
    GT_PTR newTcamLocationRulePtr = NULL;
    GT_PTR upperIdx = NULL;
    GT_PTR lowerIdx = NULL;
    GT_PTR alocatedTcamRulePtr = NULL;
    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);

    /* backup the prefixPtr information so it can be deleted from its */
    /* original location after it will be relocated                   */
    cpssOsMemCpy(&prefixBeforeRelocation,prefixPtr,sizeof(PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC));
    PRV_CPSS_DXCH_LPM_TCAM_ALLOCATE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
    rc = prvCpssDxChLpmTcamDispatcherRuleSet(prefixPtr,ipTcamShadowPtr,&oldTcamLocationRulePtr);
    rc = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_UPDATE_E,
                                                    ipTcamShadowPtr->tcamManagementMode,
                                                    NULL,
                                                    &prefixBeforeRelocation,
                                                    oldTcamLocationRulePtr);
    if (rc != GT_OK)
    {
        PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
        return rc;
    }

    /* get bounding son, valid father and TCAM functions */
    rc = prvCpssDxChLpmTcamPatTrieGetBoundingSon(prefixPtr,ipTcamShadowPtr,&boundingSon);
    if (rc != GT_OK)
    {
        PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
        return rc;
    }
    validFather    = prvCpssDxChLpmTcamPatTrieGetValidFather(prefixPtr);
    ipTcamFuncsPtr = prefixPtr->ipTcamFuncsPtr;
    /* choose allocation methood */
    allocMethod = PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MID_E;
    if (validFather == NULL)
    {
        allocMethod = PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MAX_E;
    }


    /* try to alloc between the lower and the upper*/

    if (validFather != NULL)
    {
        rc = prvCpssDxChLpmTcamDispatcherRuleSet(validFather,
                                                 ipTcamShadowPtr,
                                                 &upperIdx);
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
            return rc;
        }
    }
    else
        upperIdx = NULL;

    if(boundingSon!= NULL)
    {
        rc = prvCpssDxChLpmTcamDispatcherRuleSet(boundingSon,
                                                 ipTcamShadowPtr,
                                                 &lowerIdx);
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
            return rc;
        }
    }
    else
    {
        lowerIdx = NULL;
    }

    rc =  prvCpssDxChLpmTcamDispatchAllocate(ipTcamShadowPtr,
                                             prefixPtr->entryType,
                                             lowerIdx,
                                             upperIdx,
                                             allocMethod,
                                             GT_FALSE,
                                             prefixPtr,
                                             &alocatedTcamRulePtr);
    if(rc == GT_OK)
    {
        rc = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_UPDATE_E,
                                                        ipTcamShadowPtr->tcamManagementMode,
                                                        NULL,
                                                        prefixPtr,
                                                        alocatedTcamRulePtr);
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
            return rc;

        }
    }

    if (rc != GT_OK)
    {
        /* push the prefix to another TCAM location */
        rc = ipPatTriePush(prefixPtr->vrId,
                           ipTcamShadowPtr,
                           prefixPtr->entryType,
                           boundingSon,
                           validFather,
                           ipTcamFuncsPtr,
                           GT_FALSE,
                           GT_TRUE,
                           GT_FALSE,
                           prefixPtr,
                           &isInvalidateNeeded,
                           &oldNodePtr);
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
            return rc;
        }
    }

    /* writing node */
    rc = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_RULE_UPDATE_TRIE_E,
                                                    ipTcamShadowPtr->tcamManagementMode,
                                                    prefixPtr,
                                                    prefixPtr,
                                                    NULL);
    if (rc != GT_OK)
    {
        PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
        return rc;
    }

    /* get the new TCAM entry for the relocated prefix */
    rc = prvCpssDxChLpmTcamDispatcherRuleSet(prefixPtr,
                                             ipTcamShadowPtr,
                                             &newTcamLocationRulePtr);
    if (rc != GT_OK)
    {
        PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
        return rc;
    }

    /* get the old TCAM entry for the relocated prefix */
    rc = prvCpssDxChLpmTcamDispatcherRuleSet(&prefixBeforeRelocation,
                                             ipTcamShadowPtr,
                                             &oldTcamLocationRulePtr);
    if (rc != GT_OK)
    {
        PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
        return rc;
    }

    /* move the relocated prefix from the old TCAM entry to the new TCAM entry */
    rc = ipTcamFuncsPtr->ipTcamMovePrefix(prefixPtr->vrId,
                                          oldTcamLocationRulePtr,
                                          newTcamLocationRulePtr,
                                          prefixPtr,
                                          *ipTcamFuncsPtr->workDevListPtr);
    if (rc != GT_OK)
    {
        rc2 = prvCpssDxChLpmTcamDispatchFree(ipTcamShadowPtr,
                                             prefixPtr->entryType,
                                             prefixPtr);
        if (rc2 != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        rc2 = prvCpssDxChLpmTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_LPM_TCAM_DISPATCHER_ALLOCATED_RULE_AND_TRIE_UPDATE_E,
                                                         ipTcamShadowPtr->tcamManagementMode,
                                                         NULL,
                                                         prefixPtr,
                                                         oldTcamLocationRulePtr);
        if (rc2 != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);
        return rc;
    }

    /* decrease number of prefixes allocated in the TCAM handler by 1 */
    if (ipTcamShadowPtr->tcamEntriesHandler != NULL)
    {
        ipTcamShadowPtr->tcamEntriesHandler->allocatedIpv4 = ipTcamShadowPtr->tcamEntriesHandler->allocatedIpv4 - 1;
    }

    PRV_CPSS_DXCH_LPM_TCAM_FREE_TOKEN_NODE_MAC(ipTcamShadowPtr->tcamManagementMode,&prefixBeforeRelocation);

    /* update bounding son to all non valid nodes */
    rc = setNonValidRuleNodeBoundingSon(prefixPtr->father,
                                        ipTcamShadowPtr,
                                        GT_FALSE);
    return rc;
}

