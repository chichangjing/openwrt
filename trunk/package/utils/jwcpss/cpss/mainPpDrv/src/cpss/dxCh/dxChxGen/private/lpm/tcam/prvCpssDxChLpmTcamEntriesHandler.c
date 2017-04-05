/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamEntriesHandler.c
*
* DESCRIPTION:
*       This file contains code for the object to handle TCAM entries.
*       The TCAM entries handler is responsible for handling the TCAM entries
*       and deciding which entries to allocate to each prefix type.
*       The TCAM entries handler has two modes of operation:
*         - Partition mode. In this mode the TCAM range allocated for the TCAM
*           entries handler in partitioned so each entry type has fixed section
*           of the TCAM.
*         - No partition mode. In this mode the TCAM range is not partitioned.
*           All entry types share the TCAM entries and TCAM entries are
*           allocated on demand.
*       In partition mode, the TCAM entries handler is used as a glue layer to
*       the common tcam pool manager code which implements a partitioned TCAM.
*       In no partition mode, the TCAM entries handler implements a dynamic
*       method for a non partitioned TCAM.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 25 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamEntriesHandler.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamMgm.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamPatTrie.h>

/* number of rows in the TCAM */
extern GT_U32  tcamRows;

/* forward declarations */

static GT_STATUS createIpTcamMgmPools
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    IN GT_BOOL                                          partitionEnable,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC    *capcityCfgPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack
);

static GT_STATUS ipTcamMemPoolCheetahInit
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpPrefixes,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr
);

static GT_STATUS ipTcamMemPoolCheetah2Init
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpv4Prefixes,
    IN GT_U32                                           numOfIpv6Prefixes,
    IN GT_U32                                           numOfMcSrcAddr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr
);

static GT_STATUS ipTcamMemPoolRouterTcamInit
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpv4Prefixes,
    IN GT_U32                                           numOfIpv6Prefixes,
    IN GT_U32                                           numOfMcSrcAddr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr,
    IN GT_U32                                           numOfReservedPrefixes,
    OUT PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC *freeIndexesPtr
);

static GT_STATUS ipTcamMemPoolCheetah3Init
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpv4Prefixes,
    IN GT_U32                                           numOfIpv6Prefixes,
    IN GT_U32                                           numOfMcSrcAddr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr
);

static GT_STATUS ipTcamMemPoolXcatInit
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpv4Prefixes,
    IN GT_U32                                           numOfIpv6Prefixes,
    IN GT_U32                                           numOfMcSrcAddr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr
);

static GT_STATUS tcamHandlerGetMemPoolPtr
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    OUT PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC                     **memPoolPtrPtr
);

static GT_STATUS prvCpssDxCh3TcamEntriesHandlerCreate
(
    INOUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN    GT_BOOL                                       partitionEnable,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC *capacityCfgPtr,
    IN    CPSS_IP_PROTOCOL_STACK_ENT                    protocolStack,
    IN    GT_U32                                        tcamLines
);

static GT_STATUS tcamHandlerAllocate
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                      *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *lowerBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *upperBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                                defragEnable,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
);

static GT_STATUS tcamHandlerAllocateEntry
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                *lowerBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                *upperBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                             defragEnable,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                **allocNodePtrPtr
);

static GT_STATUS tcamHandlerAllocateLine
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                   *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                *lowerBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                *upperBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                             defragEnable,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                **allocNodePtrPtr
);

static GT_STATUS tcamHandlerFreeLine
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                    *ipTcamShadowPtr,
    IN  GT_U32                                              lineIndexToFree
);

static GT_STATUS tcamHandlerSetEntryUsage
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  GT_U32                                              entry,
    IN  GT_BOOL                                             usageState
);

static GT_STATUS tcamHandlerSetLineUsage
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  GT_U32                                              line,
    IN  GT_BOOL                                             usageState
);

static GT_STATUS tcamHandlerAllocByGivenRule
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC             *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *allocRulePtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
);

static GT_STATUS tcamHandlerFree
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    *allocNodePtr
);

static GT_STATUS tcamHandlerCheckAvailbleMem
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                                  numOfRulesNeeded
);

static GT_STATUS tcamHandlerGetRuleIdx
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    *allocNodePtr,
    OUT GT_32                                                   *ruleIdxPtr
);

static GT_STATUS tcamHandlerGetRuleByRuleIdx
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  GT_32                                                   ruleIdx,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    **ruleNodePtrPtr
);

static GT_STATUS tcamHandlerSetRealIdxAsAlloc
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC             *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  GT_U32                                                 hwIndex,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
);

static GT_STATUS convertOffsetToIndex
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  GT_U32                                              offset,
    IN  GT_BOOL                                             isEntry,
    OUT GT_U32                                              *indexPtr
);

static GT_STATUS convertIndexToOffset
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  GT_U32                                              index,
    IN  GT_BOOL                                             isEntry,
    OUT GT_U32                                              *offsetPtr
);

static GT_STATUS tcamHandlerUpdatePrefixesAllocated
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                                  amount,
    IN  GT_BOOL                                                 increase
);

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerCreate
*
* DESCRIPTION:
*       Creates TCAM entries handler object.
*       The TCAM entries handler is responsible for handling the TCAM entries
*       and deciding which entries to allocate to each prefix type.
*       The TCAM entries handler has two modes of operation:
*         - Partition mode. In this mode the TCAM range allocated for the TCAM
*           entries handler in partitioned so each entry type has fixed section
*           of the TCAM.
*         - No partition mode. In this mode the TCAM range is not partitioned.
*           All entry types share the TCAM entries and TCAM entries are
*           allocated on demand.
*       In partition mode, the TCAM entries handler is used as a glue layer to
*       the common tcam pool manager code which implements a partitioned TCAM.
*       In no partition mode, the TCAM entries handler implements a dynamic
*       method for a non partitioned TCAM.
*
* INPUTS:
*       ipTcamShadowPtr       - the shadow type to create the TCAM entries handler for
*       indexesRangePtr       - the range of TCAM indexes allocated for this TCAM entries
*                               handler
*       partitionEnable       - GT_TRUE:  partition the TCAM range according to the
*                                         capacityCfgPtr, any unused TCAM entries will
*                                         be allocated to IPv4 UC entries
*                               GT_FALSE: allocate TCAM entries on demand while
*                                         guarantee entries as specified in capacityCfgPtr
*       capacityCfgPtr        - capacity configuration
*       numOfReservedPrefixes - number of entries to reserve for non-ip entries
*                               used for Cheetah2 Multi Virtual Router support only
*       protocolStack         - the protocol stack to support
*       tcamRows              - total number of rows in the TCAM
*
* OUTPUTS:
*       ipTcamShadowPtr       - the shadow type to create the TCAM entries handler for
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerCreate
(
    INOUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN    GT_BOOL                                       partitionEnable,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC *capacityCfgPtr,
    IN    GT_U32                                        numOfReservedPrefixes,
    IN    CPSS_IP_PROTOCOL_STACK_ENT                    protocolStack,
    IN    GT_U32                                        tcamRows
)
{
    GT_STATUS   rc = GT_OK;         /* function return code*/

    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr);
    CPSS_NULL_PTR_CHECK_MAC(indexesRangePtr);
    CPSS_NULL_PTR_CHECK_MAC(capacityCfgPtr);

    /* prevent compiler warnings */
    numOfReservedPrefixes = numOfReservedPrefixes;
    tcamRows = tcamRows;

    /* allocate memory for TCAM entries handler */
    ipTcamShadowPtr->tcamEntriesHandler = (PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC*)
            cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC));
    if (ipTcamShadowPtr->tcamEntriesHandler == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    /* All Tcam pools in tcamEntriesHandler struct
       (pools that will manage  ip TCAM rules for each protocol stack and
       pool that will manage  ip mc source TCAM rules)
       should be initialized to NULL  */
    cpssOsMemSet(ipTcamShadowPtr->tcamEntriesHandler,0,sizeof(PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC));

    ipTcamShadowPtr->tcamEntriesHandler->tcamShadowType = ipTcamShadowPtr->shadowType;

    /* classify and init the TCAM entries handler */
    if ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E) ||
        ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) && (partitionEnable == GT_TRUE)) ||
        ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E) && (partitionEnable == GT_TRUE)) ||
        (ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        ipTcamShadowPtr->tcamEntriesHandler->operationMode = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E;
        rc = createIpTcamMgmPools(ipTcamShadowPtr,indexesRangePtr,
                                  partitionEnable,capacityCfgPtr,
                                  protocolStack);

    }
    else if (((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) && (partitionEnable == GT_FALSE)) ||
             ((ipTcamShadowPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E) && (partitionEnable == GT_FALSE)))
    {
        ipTcamShadowPtr->tcamEntriesHandler->operationMode = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E;
        rc = prvCpssDxCh3TcamEntriesHandlerCreate(ipTcamShadowPtr,indexesRangePtr,
                                                  partitionEnable,capacityCfgPtr,
                                                  protocolStack,tcamRows);
    }
    else
    {
        ipTcamShadowPtr->tcamEntriesHandler->operationMode = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_INVALID_MODE_E;
        rc = GT_NOT_IMPLEMENTED;
    }

    /* free TCAM entries handler memory if initialization failed */
    if (rc != GT_OK)
    {
        cpssOsFree(ipTcamShadowPtr->tcamEntriesHandler);
        ipTcamShadowPtr->tcamEntriesHandler = NULL;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerDelete
*
* DESCRIPTION:
*       Deletes TCAM entries handler object.
*
* INPUTS:
*       ipTcamShadowPtr       - the shadow type to delete the TCAM entries handler from
*
* OUTPUTS:
*       ipTcamShadowPtr       - the shadow type to delete the TCAM entries handler from
*
* RETURNS:
*       GT_OK                 - on success
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerDelete
(
    INOUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      i,j;

    if (ipTcamShadowPtr->tcamEntriesHandler->operationMode == PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E)
    {
        /* free TCAM rules pool, if allocated */
        for (i = 0 ; i < PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_NUM_OF_TCAMS_CNS; i++)
        {
            for (j = 0 ; j < PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_NUM_OF_PROTOCOLS_CNS; j++)
            {
                if (ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[i][j] != NULL)
                {
                    rc = prvCpssDxChLpmTcamMgmDestroy(ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[i][j]);
                    if (rc != GT_OK)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
            }
        }
        /* free IP MC source rules pool, if allocated */
        if (ipTcamShadowPtr->tcamEntriesHandler->ipMcSrcTcamPool != NULL )
        {
            rc = prvCpssDxChLpmTcamMgmDestroy(ipTcamShadowPtr->tcamEntriesHandler->ipMcSrcTcamPool);
            if (rc != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }
    else if (ipTcamShadowPtr->tcamEntriesHandler->operationMode == PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E)
    {
        cpssOsFree(ipTcamShadowPtr->tcamEntriesHandler->prefixPtrArray);
        cpssOsFree(ipTcamShadowPtr->tcamEntriesHandler->entriesUsagePtr);
        cpssOsFree(ipTcamShadowPtr->tcamEntriesHandler->lineUsagePtr);
    }

    /* free TCAM entries handler memory */
    cpssOsFree(ipTcamShadowPtr->tcamEntriesHandler);

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamMemPoolCheetah2PolicyBasedRoutingInit
*
* DESCRIPTION:
*       This function initializes the managment pool for Cheetah2 with multiple
*       routers implemented through policy-based routing.
*
* INPUTS:
*       pclTcamCfgPtr    - the pcl tcam configuration
*       routerTcamCfgPtr - the router tcam configuration
*       protocolStack    - the protocol stack to support
*       ipTcamShadowPtr  - the shadow to alloc the pools for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_OUT_OF_CPU_MEM - if unable to create pool.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamMemPoolCheetah2PolicyBasedRoutingInit
(
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                        *ipTcamShadowPtr
)
{
    GT_STATUS                                           ret_status;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC  pclIndexes;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC  freeIndexes;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC                 *ipTcamMemPoolMgm;
    GT_U32                                              i;

    cpssOsMemSet(&freeIndexes, 0x00, sizeof(freeIndexes));
    cpssOsMemSet(&pclIndexes, 0x00, sizeof(pclIndexes));

    /* Router Tcam Parameters */
    ret_status = ipTcamMemPoolRouterTcamInit(routerTcamCfgPtr->tcamRange.firstIndex,
                                             routerTcamCfgPtr->tcamRange.lastIndex,
                                             routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes,
                                             routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes,
                                             0, protocolStack, ipTcamShadowPtr,
                                             pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
                                             pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes,
                                             &freeIndexes);
    if (GT_OK != ret_status)
        return ret_status;

    /* Policy Based Routing Parameters */

    for (i = 0; i < 2; i++)
    {
        pclIndexes.segment[i].startIndex =
            pclTcamCfgPtr->tcamRange.firstIndex + PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS * i;
        pclIndexes.segment[i].endIndex =
            pclTcamCfgPtr->tcamRange.lastIndex + PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS * i;
        pclIndexes.segment[i].numOfIndexes =
            pclIndexes.segment[i].endIndex - pclIndexes.segment[i].startIndex + 1;
    }

    ipTcamMemPoolMgm = prvCpssDxChLpmTcamMgmCreate(pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
                                                   pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes,
                                                   pclIndexes.segment, 2, freeIndexes.segment,
                                                   5, NULL, NULL);

    ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV4_E] =
        ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV6_E] =
            ipTcamMemPoolMgm;

    return ret_status;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerAllocate
*
* DESCRIPTION:
*       Allocates TCAM entry for specific type of prefix. The entry is allocated
*       in a range that is bounded by two other entries (bounded in the meaning
*       of TCAM search order). The entry is allocated according to specific
*       allocation method.
*
* INPUTS:
*       ipTcamShadowPtr       - the ip shadow pointer we are working on
*       entryType             - entry type to allocate
*       lowerBoundPtr         - the lower bound; the allocated rule reside after
*                               this rule; if NULL then no lower bound
*       upperBoundPtr         - the upper bound; the allocated rule reside before
*                               this rule; if NULL then no upper bound
*       allocMethod           - the allocation method
*       defragEnable          - whether to defrag entries in case there is no
*                               free place for this entry cause of entries
*                               fragmentation; relevant only if the TCAM entries
*                               handler was created with partitionEnable = GT_FALSE
*
* OUTPUTS:
*       allocNodePtrPtr       - the allocated rule node
*
* RETURNS:
*       GT_OK                     - if allocation succedded
*       GT_OUT_OF_PP_MEM          - if there is no space left between the upper
*                                   and lower bounds
*       GT_OUT_OF_RANGE           - if the indexes were out of range
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerAllocate
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                        *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    *lowerBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    *upperBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                                 defragEnable,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    **allocNodePtrPtr
)
{
    GT_STATUS                                   rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC         *pIpTcamPool = NULL;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_METHOD_ENT poolAllocMethod;

    /* pointer to TCAM entries handler */
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC  *tcamHandlerPtr;
    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr->tcamEntriesHandler);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtrPtr);

    /* pointer to TCAM entries handler */
    tcamHandlerPtr = ipTcamShadowPtr->tcamEntriesHandler;

    /* prevent compiler warnings */
    defragEnable = defragEnable;

    switch (tcamHandlerPtr->operationMode)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E:
        rc = tcamHandlerGetMemPoolPtr(tcamHandlerPtr,entryType,&pIpTcamPool);
        if (rc != GT_OK)
            return rc;

        switch (allocMethod)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MIN_E:
            poolAllocMethod = PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MIN_E;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MID_E:
            poolAllocMethod = PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MID_E;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MAX_E:
            poolAllocMethod = PRV_CPSS_DXCH_LPM_TCAM_MGM_ALLOC_MAX_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChLpmTcamMgmAlloc(pIpTcamPool,lowerBoundPtr,upperBoundPtr,
                                        poolAllocMethod,allocNodePtrPtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E:
        rc = tcamHandlerAllocate(ipTcamShadowPtr,entryType,
                                 lowerBoundPtr,upperBoundPtr,
                                 allocMethod,defragEnable,allocNodePtrPtr);
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
        break;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerAllocByGivenRule
*
* DESCRIPTION:
*       Allocates a rule by a another given rule.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       allocRulePtr          - given rule to allocate the new rule from it
*
* OUTPUTS:
*       allocNodePtrPtr       - the allocated rule node
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_ALREADY_EXIST         - if the rule is already allocated.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerAllocByGivenRule
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC             *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *allocRulePtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
)
{
    GT_STATUS                                   rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC         *pIpTcamPool = NULL;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocRulePtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtrPtr);

    switch (tcamHandlerPtr->operationMode)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E:
        rc = tcamHandlerGetMemPoolPtr(tcamHandlerPtr,entryType,&pIpTcamPool);
        if (rc != GT_OK)
            return rc;

        rc = prvCpssDxChLpmTcamMgmAllocByGivenRule(pIpTcamPool,allocRulePtr,allocNodePtrPtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E:
        rc = tcamHandlerAllocByGivenRule(tcamHandlerPtr,entryType,allocRulePtr,allocNodePtrPtr);
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
        break;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerFree
*
* DESCRIPTION:
*       Frees a TCAM entry.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       allocNodePtr          - the entry to free.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_NOT_FOUND             - if the rule wasn't found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerFree
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    *allocNodePtr
)
{
    GT_STATUS                                   rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC         *pIpTcamPool = NULL;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtr);

    switch (tcamHandlerPtr->operationMode)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E:
        rc = tcamHandlerGetMemPoolPtr(tcamHandlerPtr,entryType,&pIpTcamPool);
        if (rc != GT_OK)
            return rc;

        rc = prvCpssDxChLpmTcamMgmFree(pIpTcamPool,allocNodePtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E:
        rc = tcamHandlerFree(tcamHandlerPtr,entryType,allocNodePtr);
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
        break;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerCheckAvailableMem
*
* DESCRIPTION:
*       Checks whether the TCAM has enough available space to add additional
*       entries of specific entry type.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       numOfRulesNeeded      - number of entries needed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if there is room
*       GT_OUT_OF_PP_MEM otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       numOfIpTcamRules != 0!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerCheckAvailableMem
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                                  numOfRulesNeeded
)
{
    GT_STATUS                                   rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC         *pIpTcamPool = NULL;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);

    switch (tcamHandlerPtr->operationMode)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E:
        rc = tcamHandlerGetMemPoolPtr(tcamHandlerPtr,entryType,&pIpTcamPool);
        if (rc != GT_OK)
            return rc;

        rc = prvCpssDxChLpmTcamMgmCheckAvailbleMem(pIpTcamPool,numOfRulesNeeded);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E:
        rc = tcamHandlerCheckAvailbleMem(tcamHandlerPtr,entryType,numOfRulesNeeded);
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
        break;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerGetRuleIdx
*
* DESCRIPTION:
*       The TCAM entries handler maintains internal array that holds rules (those
*       rules are also associated with the prefixes attached to them).
*       This funtion returns the index of the rule in that array.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       allocNodePtr          - the rule to get its index
*
* OUTPUTS:
*       ruleIdxPtr            - the rule idx.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerGetRuleIdx
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    *allocNodePtr,
    OUT GT_32                                                   *ruleIdxPtr
)
{
    GT_STATUS                                   rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC         *pIpTcamPool = NULL;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleIdxPtr);

    switch (tcamHandlerPtr->operationMode)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E:
        rc = tcamHandlerGetMemPoolPtr(tcamHandlerPtr,entryType,&pIpTcamPool);
        if (rc != GT_OK)
            return rc;

        rc = prvCpssDxChLpmTcamMgmGetRuleIdx(pIpTcamPool,allocNodePtr,ruleIdxPtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E:
        rc = tcamHandlerGetRuleIdx(tcamHandlerPtr,entryType,allocNodePtr,ruleIdxPtr);
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
        break;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerGetRuleByRuleIdx
*
* DESCRIPTION:
*       The TCAM entries handler maintains internal array that holds rules (those
*       rules are also associated with the prefixes attached to them).
*       This function returns the rule located at the internal rule index in
*       that array.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       ruleIdx               - the rule idx.
*
* OUTPUTS:
*       ruleNodePtrPtr        - the rule node of this index
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerGetRuleByRuleIdx
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  GT_32                                                   ruleIdx,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    **ruleNodePtrPtr
)
{
    GT_STATUS                                   rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC         *pIpTcamPool = NULL;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleNodePtrPtr);

    switch (tcamHandlerPtr->operationMode)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E:
        rc = tcamHandlerGetMemPoolPtr(tcamHandlerPtr,entryType,&pIpTcamPool);
        if (rc != GT_OK)
            return rc;

        rc = prvCpssDxChLpmTcamMgmGetRuleByRuleIdx(pIpTcamPool,ruleIdx,ruleNodePtrPtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E:
        rc = tcamHandlerGetRuleByRuleIdx(tcamHandlerPtr,entryType,ruleIdx,ruleNodePtrPtr);
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
        break;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerSetRealIdxAsAlloc
*
* DESCRIPTION:
*       Marks specific TCAM entry as used and returns the rule associated with
*       this TCAM entry.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       hwIndex               - the hwRule index.
*
* OUTPUTS:
*       allocNodePtrPtr       - points to allocated node
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_SUPPORTED         - if Tcam handler new mode is used
*       GT_NOT_IMPLEMENTED       - others
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerSetRealIdxAsAlloc
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC             *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  GT_U32                                                 hwIndex,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
)
{
    GT_STATUS                                   rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC         *pIpTcamPool = NULL;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtrPtr);

    switch (tcamHandlerPtr->operationMode)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E:
        rc = tcamHandlerGetMemPoolPtr(tcamHandlerPtr,entryType,&pIpTcamPool);
        if (rc != GT_OK)
            return rc;
        *allocNodePtrPtr = prvCpssDxChLpmTcamMgmSetRealIdxAsAlloc(pIpTcamPool,hwIndex);

        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E:
        rc = tcamHandlerSetRealIdxAsAlloc(tcamHandlerPtr,entryType,hwIndex,allocNodePtrPtr);
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
        break;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerGetRuleInfoByRealIdx
*
* DESCRIPTION:
*       The TCAM entries handler maintains internal array that holds rules (those
*       rules are also associated with the prefixes attached to them).
*       This function gets hardware TCAM index and returns the rule associated
*       with this entry.
*       In static partition, the function also returns the type of the entry,
*       according to the pool that the TCAM index resides in.
*       In dynamic partition, the entry type is not returned because in dynamic
*       partitioning, same entry can be used to more than one entry type.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       hwIndex               - the rule index.
*       entryTypePtr          - entry type, relevant as input only in case of
*                               dynamic partition
*
* OUTPUTS:
*       entryTypePtr          - entry type, relevant as output only in case of
*                               static partition
*       ruleNodePtrPtr        - the rule node of this index
*
* RETURNS:
*       GT_OK                    - if success
*       GT_OUT_OF_RANGE          - if the hw index is not in any of the pools
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In dynamic partition, the rule accosiated with the hwIndex is returned.
*       This rule should be deal with care. It must not be used as IPv6 entry
*       if it is not in the first TCAM column.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamEntriesHandlerGetRuleInfoByRealIdx
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC            *tcamHandlerPtr,
    IN    GT_U32                                                hwIndex,
    INOUT PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT *entryTypePtr,
    OUT   PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                  **ruleNodePtrPtr
)
{
    GT_STATUS                                   rc = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC         *pIpTcamPoolPtr = NULL;
    GT_U32                                      ruleIndex;
    GT_BOOL                                     singleTcamEntry;
    GT_U32                                      i;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(entryTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleNodePtrPtr);

    switch (tcamHandlerPtr->operationMode)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E:
        /* search in IPv6 pool */
        pIpTcamPoolPtr = tcamHandlerPtr->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV6_E];
        if (pIpTcamPoolPtr != NULL)
        {
            for (i = 0 ; i < pIpTcamPoolPtr->coreIpNumOfIpTcamRules ; i++)
            {
                if (hwIndex == pIpTcamPoolPtr->coreIpTcamRulesAllocArray[i].offset)
                {
                    *entryTypePtr = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;
                    *ruleNodePtrPtr = &pIpTcamPoolPtr->coreIpTcamRulesAllocArray[i];
                    return GT_OK;
                }
            }
        }

        /* if not found, search in IPv4 UC pool */
        pIpTcamPoolPtr = tcamHandlerPtr->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV4_E];
        if (pIpTcamPoolPtr != NULL)
        {
            for (i = 0 ; i < pIpTcamPoolPtr->coreIpNumOfIpTcamRules ; i++)
            {
                if (hwIndex == pIpTcamPoolPtr->coreIpTcamRulesAllocArray[i].offset)
                {
                    *entryTypePtr = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
                    *ruleNodePtrPtr = &pIpTcamPoolPtr->coreIpTcamRulesAllocArray[i];
                    return GT_OK;
                }
            }
        }

        /* if not found, search in IPv4 MC Src pool */
        pIpTcamPoolPtr = tcamHandlerPtr->ipMcSrcTcamPool;
        if (pIpTcamPoolPtr != NULL)
        {
            for (i = 0 ; i < pIpTcamPoolPtr->coreIpNumOfIpTcamRules ; i++)
            {
                if (hwIndex == pIpTcamPoolPtr->coreIpTcamRulesAllocArray[i].offset)
                {
                    *entryTypePtr = PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
                    *ruleNodePtrPtr = &pIpTcamPoolPtr->coreIpTcamRulesAllocArray[i];
                    return GT_OK;
                }
            }
        }

        /* the hw index wasn't found in any of the pools */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E:
        /* decide if entry type is single entry or line */
        switch (*entryTypePtr)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
            singleTcamEntry = GT_TRUE;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
            singleTcamEntry = GT_FALSE;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        rc = convertOffsetToIndex(tcamHandlerPtr,hwIndex,singleTcamEntry,&ruleIndex);

        if (rc != GT_OK)
            return rc;
        if (ruleIndex >= tcamHandlerPtr->numOfTcamEntries)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        *ruleNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[ruleIndex];
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
        break;
    }

    return rc;
}

/*******************************************************************************
* createIpTcamMgmPools
*
* DESCRIPTION:
*       In partition mode, the TCAM entries handler is used as a glue layer to
*       the common tcam pool manager code which implements a partitioned TCAM.
*       This function divides the TCAM range to the various entry types and
*       creates pool managers for each of those types.
*
* INPUTS:
*       ipTcamShadowPtr - the shadow to alloc the pools for.
*       indexesRangePtr - the range of TCAM indexes allocated for this TCAM
*                         entries handler
*       partitionEnable - GT_TRUE:  partition the TCAM range according to the
*                                   capacityCfgPtr, any unused TCAM entries will
*                                   be allocated to IPv4 UC entries
*                         GT_FALSE: allocate TCAM entries on demand while
*                                   guarantee entries as specified in capacityCfgPtr
*       capacityCfgPtr  - capacity configuration
*       protocolStack   - the protocol stack to support
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Since this function creates separate pools for each entry type, it can't
*       support no partition and therefore partition enable parameter must be
*       set to GT_TRUE.
*
*******************************************************************************/
static GT_STATUS createIpTcamMgmPools
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    IN GT_BOOL                                          partitionEnable,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC    *capacityCfgPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack
)
{
    GT_STATUS result;

    GT_U32  firstIpTcamLine;        /* first TCAM line allocated to ip      */
    GT_U32  lastIpTcamLine;         /* last TCAM line allocated to ip       */
    GT_U32  numOfIpv4Prefixes = 0;
    GT_U32  numOfIpv6Prefixes = 0;
    GT_U32  numOfMcSrcAddr    = 0;
    GT_U8   i,j;
    GT_U32  numOfIndexes = 0;

    /* Since this function creates separate pools for each entry type, it can't */
    /* support no partition and therefore partition enable parameter must be    */
    /* set to GT_TRUE.                                                          */
    if (partitionEnable == GT_FALSE)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    firstIpTcamLine = indexesRangePtr->firstIndex;
    lastIpTcamLine  = indexesRangePtr->lastIndex;
    if (lastIpTcamLine < firstIpTcamLine)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    numOfIpv4Prefixes   += capacityCfgPtr->numOfIpv4Prefixes;

    numOfIpv6Prefixes   += capacityCfgPtr->numOfIpv6Prefixes;

    numOfMcSrcAddr      = capacityCfgPtr->numOfIpv4McSourcePrefixes;

    /* init Tcam pool pointers for every combination */
    for (i=0; i<2; i++)
    {
        for (j=0; j<2; j++)
        {
            ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[i][j] = NULL;
        }
    }

    switch (ipTcamShadowPtr->shadowType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        if ((lastIpTcamLine >= PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS) ||
            (lastIpTcamLine - firstIpTcamLine + 1 > PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Ch+ PCL TCAM contains 2 columns and search order is column by column */
        numOfIndexes = ((lastIpTcamLine - firstIpTcamLine + 1) * 2);

        if((numOfIpv4Prefixes > numOfIndexes)||
           (numOfIpv6Prefixes > numOfIndexes)||
           (numOfIpv4Prefixes+numOfIpv6Prefixes > numOfIndexes))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        result = ipTcamMemPoolCheetahInit(firstIpTcamLine, lastIpTcamLine,
                                          numOfIpv4Prefixes + numOfIpv6Prefixes,
                                          ipTcamShadowPtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        if ((lastIpTcamLine >= PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_NUM_OF_RULES_IN_COLUMN_CNS) ||
            lastIpTcamLine - firstIpTcamLine + 1 > PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_NUM_OF_RULES_IN_COLUMN_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        result = ipTcamMemPoolCheetah2Init(firstIpTcamLine, lastIpTcamLine,
                                           numOfIpv4Prefixes,
                                           numOfIpv6Prefixes,
                                           numOfMcSrcAddr, protocolStack,
                                           ipTcamShadowPtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
        if ((lastIpTcamLine >= PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS) ||
            lastIpTcamLine - firstIpTcamLine + 1 > PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        result = ipTcamMemPoolCheetah3Init(firstIpTcamLine, lastIpTcamLine,
                                           numOfIpv4Prefixes,
                                           numOfIpv6Prefixes,
                                           numOfMcSrcAddr, protocolStack,
                                           ipTcamShadowPtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
        if ((lastIpTcamLine >= PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS) ||
            lastIpTcamLine - firstIpTcamLine + 1 > PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        result = ipTcamMemPoolXcatInit(firstIpTcamLine, lastIpTcamLine,
                                       numOfIpv4Prefixes,
                                       numOfIpv6Prefixes,
                                       numOfMcSrcAddr, protocolStack,
                                       ipTcamShadowPtr);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        if ((lastIpTcamLine >= PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_TCAM_NUM_OF_RULES_CEILING_CNS) ||
            (lastIpTcamLine - firstIpTcamLine + 1 > PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_TCAM_NUM_OF_RULES_CEILING_CNS))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* xCat PCL TCAM contains 4 ip rules and search order is row by row */
        numOfIndexes = ((lastIpTcamLine - firstIpTcamLine + 1) * 4);
        if((numOfIpv4Prefixes > numOfIndexes)||
           (numOfIpv6Prefixes > numOfIndexes)||
           (numOfIpv4Prefixes+numOfIpv6Prefixes > numOfIndexes))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        result = ipTcamMemPoolCheetahInit(firstIpTcamLine, lastIpTcamLine,
                                          numOfIpv4Prefixes + numOfIpv6Prefixes,
                                          ipTcamShadowPtr);
        break;

    default:
        result = GT_FAIL;
    }

    return result;
}
/*******************************************************************************
* ipTcamMemPoolCheetahInit
*
* DESCRIPTION:
*       This function initializes the managment pool for Cheetah+
*       and for xCat that configured to work in policy based routing.
*
* INPUTS:
*       tcamStartIndex   -  first TCAM line allocated to ip
*       tcamEndIndex     -  last TCAM line allocated to ip
*       numOfIpPrefixes  -  total number of ip prefixes to manage (Ipv4+Ipv6)
*       ipTcamShadowPtr  -  the shadow to alloc the pools for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_OUT_OF_CPU_MEM - if unable to create pool.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamMemPoolCheetahInit
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpPrefixes,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC_IDX_SEG_STC idxSegment[2];
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC             *ipTcamMemPoolMgm;
    GT_U32                                          i;
    GT_U32                                          numOfSegments = 0;

    switch (ipTcamShadowPtr->shadowType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        /* Ch+ PCL TCAM contains 2 columns and search order is column by column */
        for (i=0; i<2; i++)
        {
            idxSegment[i].startIndex   = tcamStartIndex + PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS * i;
            idxSegment[i].endIndex     = tcamEndIndex  + PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS * i;
            idxSegment[i].numOfIndexes = tcamEndIndex - tcamStartIndex + 1;
            numOfSegments++;
        }
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        /* xCat PCL TCAM contains 4 ip rules and search order is row by row */
        idxSegment[0].startIndex   = tcamStartIndex * 4;
        idxSegment[0].endIndex     = ((tcamEndIndex + 1) * 4) - 1;
        idxSegment[0].numOfIndexes = ((tcamEndIndex - tcamStartIndex + 1) * 4);
        numOfSegments++;
        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* create the tcam rules manager for this lpm db*/
    ipTcamMemPoolMgm = prvCpssDxChLpmTcamMgmCreate(numOfIpPrefixes,idxSegment,numOfSegments,
                                                   NULL,0,NULL,NULL);

    if (ipTcamMemPoolMgm == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV4_E] =
        ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV6_E] =
            ipTcamMemPoolMgm;

    return GT_OK;
}

/*******************************************************************************
* ipTcamMemPoolCheetah2Init
*
* DESCRIPTION:
*       This function initializes the managment pool for Cheetah2 in Router Tcam.
*
* INPUTS:
*       tcamStartIndex    -  first TCAM line allocated to ip
*       tcamEndIndex      -  last TCAM line allocated to ip
*       numOfIpv4Prefixes -  total number of IPv4 prefixes to manage
*       numOfIpv6Prefixes -  total number of IPv4 prefixes to manage
*       numOfMcSrcAddr    -  total number of Mc source addresses to manage
*       protocolStack     -  the protocol stack to support
*       ipTcamShadowPtr   -  the shadow to alloc the pools for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NO_RESOURCE    - if number of prefixes exceeds number of indexes in table
*       GT_OUT_OF_CPU_MEM - if unable to create pool.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamMemPoolCheetah2Init
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpv4Prefixes,
    IN GT_U32                                           numOfIpv6Prefixes,
    IN GT_U32                                           numOfMcSrcAddr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr
)
{
    return ipTcamMemPoolRouterTcamInit(tcamStartIndex, tcamEndIndex,
                                       numOfIpv4Prefixes, numOfIpv6Prefixes,
                                       numOfMcSrcAddr, protocolStack,
                                       ipTcamShadowPtr, 0, NULL);
}

/*******************************************************************************
* ipTcamMemPoolRouterTcamInit
*
* DESCRIPTION:
*       This function initializes the managment pool for Cheetah2 in Router Tcam.
*
* INPUTS:
*       tcamStartIndex    -  first TCAM line allocated to ip
*       tcamEndIndex      -  last TCAM line allocated to ip
*       numOfIpv4Prefixes -  total number of IPv4 prefixes to manage
*       numOfIpv6Prefixes -  total number of IPv4 prefixes to manage
*       numOfMcSrcAddr    -  total number of Mc source addresses to manage
*       protocolStack     -  the protocol stack to support
*       ipTcamShadowPtr   -  the shadow to alloc the pools for
*       numOfReservedPrefixes - num of entries (single entries) which must be
*                               reserveed for external management (for example
*                               PCL redirecting to LTT)
*                               Relevant if freeIndexesPtr != NULL
*
* OUTPUTS:
*       freeIndexesPtr    - if != NULL then used to return reserved indexes
*                           requested in numOfReservedPrefixes
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NO_RESOURCE    - if number of prefixes exceeds number of indexes in table
*       GT_OUT_OF_CPU_MEM - if unable to create pool.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamMemPoolRouterTcamInit
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpv4Prefixes,
    IN GT_U32                                           numOfIpv6Prefixes,
    IN GT_U32                                           numOfMcSrcAddr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr,
    IN GT_U32                                           numOfReservedPrefixes,
    OUT PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC *freeIndexesPtr
)
{
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC                 *ipTcamMemPoolMgm;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC  ipv4IdxSegments;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC  mcSrcIdxSegments;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC  ipv6IdxSegments;

    GT_U32  columns;                    /* number of columns in TCAM           */
    GT_BOOL columnSupportMcSrc[10];     /* whether column can hold MC src      */
    GT_U32  maxMcSrcSupported;          /* max number of MC Src that can be    */
                                        /* supported                           */
    GT_U32  indexesInColumn;            /* number of indexes in column         */
    GT_U32  indexesInColumnAfterIpv6;   /* number of indexes in column after   */
                                        /* allocating columns for ipv6         */
    GT_U32  indexesLeftInColumn;        /* number of indexes not allocated yet */
                                        /* in column                           */
    GT_U32  segStartIdx;                /* segment start index                 */
    GT_U32  indexesInSeg;               /* number of indexes in segment        */

    GT_U32  numOfReservedPrefixesAllocAtNonIpv6Lines;   /* number of reserved prefixes that
                                                           are allocated at non IPv6 lines */
    GT_U32  i;                          /* counter */

    GT_U8   ipv4SegNum      = 0;        /* number of Ipv4 prefixes segments     */
    GT_U8   mcSegNum        = 0;        /* number of Ipv4 MC src segments       */
    GT_U8   reservedSegNum  = 0;        /* number of reserved prefixes segments */

    GT_U32  mcSrcToAlloc  = numOfMcSrcAddr; /* number of ip mc src not allocated yet     */
    GT_U32  ipv4ToAlloc;                    /* number of ipv4 prefixes not allocated yet */

    GT_U32  indexesLeftForIpv4;         /* indexes left for IPv4 after allocating
                                           indexes to all other types */

    /*
     * in Cheetah 2 the TCAM lines allocated to the ip are devided in
     * the following way:
     *
     * tcamStartIndex  --> ----------------------------------------------
     *                     |        |        |        |        |        |
     *                     |        |        |        |        |        |
     *                     |        |  ipv4  |  ipv4  |        |        |
     *                     |  ipv4  |   /    |   /    |  ipv4  |  ipv4  |
     *                     |        | MC src | MC src |        |        |
     *                     |        |        |        |        |        |
     *                     |        |        |        |        |        |
     *                     |        |        |        |        |        |
     *                     |        |        |        |        |        |
     *                     |        |        |        |        |        |
     *                     |--------------------------------------------|
     *                     |                                            |
     *                     |                   ipv6                     |
     *                     |                                            |
     * tcamEndIndex   -->  ----------------------------------------------
     *
     *
     * in Cheetah 3 the TCAM lines allocated to the ip are devided in
     * the following way:
     *
     * tcamStartIndex  --> -------------------------------------
     *                     |        |        |        |        |
     *                     |        |        |        |        |
     *                     |  ipv4  |  ipv4  |  ipv4  |  ipv4  |
     *                     |   /    |   /    |   /    |   /    |
     *                     | MC src | MC src | MC src | MC src |
     *                     |        |        |        |        |
     *                     |        |        |        |        |
     *                     |        |        |        |        |
     *                     |        |        |        |        |
     *                     |        |        |        |        |
     *                     |-----------------------------------|
     *                     |                                   |
     *                     |               ipv6                |
     *                     |                                   |
     * tcamEndIndex   -->  -------------------------------------
     *
     *
     * Note that reserved prefixes can reside at any entry in the TCAM.
     * Those prefixes are marked not to be used because the associated LTT entry
     * is needed (for example for policy based routing). It is prefered to
     * allocate those prefixes from lines that contains IPv6 prefix. Each such
     * line uses only the LTT associated with the first column so entries in
     * the other columns can be allocated to the reserved prefixes with no
     * resource lost.
     *
     */

    /* set number of columns and for each column if it can hold MC src entries */
    switch (ipTcamShadowPtr->shadowType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        columns = 5;
        columnSupportMcSrc[0] = GT_FALSE;
        columnSupportMcSrc[1] = GT_TRUE;
        columnSupportMcSrc[2] = GT_TRUE;
        columnSupportMcSrc[3] = GT_FALSE;
        columnSupportMcSrc[4] = GT_FALSE;
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
        columns = 4;
        columnSupportMcSrc[0] = GT_TRUE;
        columnSupportMcSrc[1] = GT_TRUE;
        columnSupportMcSrc[2] = GT_TRUE;
        columnSupportMcSrc[3] = GT_TRUE;
        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* if (freeIndexesPtr == NULL) then numOfReservedPrefixes is not relevant */
    if (freeIndexesPtr == NULL)
        numOfReservedPrefixes = 0;

    /* calc number of indexes in each column */
    indexesInColumn = tcamEndIndex - tcamStartIndex + 1;

    /******************************************************/
    /* make sure the requested number of prefixes can fit */
    /* in the space allocated in the TCAM for the ip      */
    /******************************************************/

    /* check ipv6 prefixes can fit */
    if (numOfIpv6Prefixes > indexesInColumn)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);

    /* check ipv4 MC Src prefixes can fit */
    for (i = 0 , maxMcSrcSupported = 0 ; i < columns ; i++)
    {
        if (columnSupportMcSrc[i] == GT_TRUE)
            maxMcSrcSupported += (indexesInColumn - numOfIpv6Prefixes);
    }
    if (numOfMcSrcAddr > maxMcSrcSupported)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);

    /* calc how many reserved prefixes need to be allocated at non ipv6 lines
       Note that in each IPv6 prefix line, only LTT associated with first column
       is used so LTT associates with other columns in that line can be used
       for reserved prefixes */
    if (numOfReservedPrefixes > (numOfIpv6Prefixes * (columns - 1)))
    {
        numOfReservedPrefixesAllocAtNonIpv6Lines =
            numOfReservedPrefixes - (numOfIpv6Prefixes * (columns - 1));
    }
    else
    {
        numOfReservedPrefixesAllocAtNonIpv6Lines = 0;
    }

    /* calc how many prefixes left for IPv4 prefixes */
    indexesLeftForIpv4 = (columns * (indexesInColumn - numOfIpv6Prefixes)) -
                         numOfMcSrcAddr -
                         numOfReservedPrefixesAllocAtNonIpv6Lines;


    /* check ipv4 prefixes can fit */
    if (numOfIpv4Prefixes > indexesLeftForIpv4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);

    /* if after allocating the required IPv4 prefixes there will still be free
       indexes, then allocate those to IPv4 as well; also update shadow that
       there are more ipv4 prefixes now */
    if (indexesLeftForIpv4 > numOfIpv4Prefixes)
    {
        numOfIpv4Prefixes = indexesLeftForIpv4;
        ipTcamShadowPtr->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E] = numOfIpv4Prefixes;

    }

    /***************************************************************/
    /* now start to allocate TCAM entries for the various prefixes */
    /***************************************************************/

    cpssOsMemSet(&ipv4IdxSegments,  0x00, sizeof(PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC));
    cpssOsMemSet(&mcSrcIdxSegments, 0x00, sizeof(PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC));
    cpssOsMemSet(&ipv6IdxSegments,  0x00, sizeof(PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC));
    if (freeIndexesPtr != NULL)
    {
        cpssOsMemSet(freeIndexesPtr, 0x00, sizeof(PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_IDX_SEG_STC));
    }

    /* ipv6 pool init section */
    if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) ||
        (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
    {
        /* start with ipv6 */
        ipv6IdxSegments.segment[0].startIndex   = tcamEndIndex + 1 - numOfIpv6Prefixes;
        ipv6IdxSegments.segment[0].endIndex     = tcamEndIndex;
        ipv6IdxSegments.segment[0].numOfIndexes = numOfIpv6Prefixes;


        /* create the tcam rules manager for this VR*/
        ipTcamMemPoolMgm = prvCpssDxChLpmTcamMgmCreate(numOfIpv6Prefixes,
                                                       ipv6IdxSegments.segment, 1, NULL,
                                                       0, NULL, NULL);

        if ((ipTcamMemPoolMgm == NULL) && (numOfIpv6Prefixes > 0))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

        ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV6_E]
                                                            = ipTcamMemPoolMgm;
    }

    /* ipv4 pool init section */
    if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E)   ||
        (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) ||
        (numOfReservedPrefixes > 0))
    {
        /* move to ipv4 */

        /* get number of indexes per column after ipv6 allocation */
        indexesInColumnAfterIpv6 = indexesInColumn - numOfIpv6Prefixes;

        /* free indexes were added to number of IPv4 prefixes */
        ipv4ToAlloc = numOfIpv4Prefixes;

        /* Now prepare segments, all indexes not allocated to ipv6 or
           mc src addr or to reserved prefixes are allocated to ipv4.
           Note that all possible reserved prefixes in IPv6 lines are
           allocated to reserved even if it is more than required
           reserved prefixes */

        /* go over all columns supported by the TCAM and prepare segments */
        /* for ipv4 prefixes and ipv4 MC src prefixes                     */
        for (i = 0 ; i < columns ; i++)
        {
            indexesLeftInColumn = indexesInColumnAfterIpv6;
            segStartIdx = i * tcamRows + tcamStartIndex;

            /* allocate mc src if needed and if supported by the column */
            if ((columnSupportMcSrc[i] == GT_TRUE) && (mcSrcToAlloc > 0) && (indexesLeftInColumn > 0))
            {
                if (mcSrcToAlloc > indexesLeftInColumn)
                    indexesInSeg = indexesLeftInColumn;
                else
                    indexesInSeg = mcSrcToAlloc;

                mcSrcIdxSegments.segment[mcSegNum].startIndex   = segStartIdx;
                mcSrcIdxSegments.segment[mcSegNum].endIndex     = segStartIdx + indexesInSeg - 1;
                mcSrcIdxSegments.segment[mcSegNum].numOfIndexes = indexesInSeg;
                mcSegNum++;

                segStartIdx         += indexesInSeg;
                indexesLeftInColumn -= indexesInSeg;
                mcSrcToAlloc        -= indexesInSeg;
            }

            /* allocate ipv4 prefixes */
            if ((ipv4ToAlloc > 0) && (indexesLeftInColumn > 0))
            {
                if (ipv4ToAlloc > indexesLeftInColumn)
                    indexesInSeg = indexesLeftInColumn;
                else
                    indexesInSeg = ipv4ToAlloc;

                ipv4IdxSegments.segment[ipv4SegNum].startIndex   = segStartIdx;
                ipv4IdxSegments.segment[ipv4SegNum].endIndex     = segStartIdx + indexesInSeg - 1;
                ipv4IdxSegments.segment[ipv4SegNum].numOfIndexes = indexesInSeg;
                ipv4SegNum++;

                segStartIdx         += indexesInSeg;
                indexesLeftInColumn -= indexesInSeg;
                ipv4ToAlloc         -= indexesInSeg;
            }

            /* allocate rest of indexes to reserved prefixes; note that number of
               ipv4 prefixes was modified before to include all prefixes not required
               so all indexes left at this point are those required as reserved */
            if (freeIndexesPtr != NULL)
            {
                if (indexesLeftInColumn == 0)
                {
                    if (i != 0)
                    {
                        freeIndexesPtr->segment[reservedSegNum].startIndex   =
                            ipv6IdxSegments.segment[0].startIndex + i * tcamRows;
                        freeIndexesPtr->segment[reservedSegNum].endIndex     =
                            ipv6IdxSegments.segment[0].endIndex + i * tcamRows;
                        freeIndexesPtr->segment[reservedSegNum].numOfIndexes =
                            ipv6IdxSegments.segment[0].numOfIndexes;
                        reservedSegNum++;
                    }
                }
                else
                {
                    freeIndexesPtr->segment[reservedSegNum].startIndex   = segStartIdx;
                    if ((i != 0) && (numOfIpv6Prefixes > 0))
                    {
                        freeIndexesPtr->segment[reservedSegNum].endIndex     =
                            ipv6IdxSegments.segment[0].endIndex + i * tcamRows;
                        freeIndexesPtr->segment[reservedSegNum].numOfIndexes =
                            indexesLeftInColumn + ipv6IdxSegments.segment[0].numOfIndexes;
                    }
                    else
                    {
                        freeIndexesPtr->segment[reservedSegNum].endIndex     = segStartIdx + indexesLeftInColumn - 1;
                        freeIndexesPtr->segment[reservedSegNum].numOfIndexes = indexesLeftInColumn;
                    }
                    reservedSegNum++;
                }
            }
        }

        /* now create the ipv4 pools according to the segments */
        if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E)   ||
            (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
        {
            /* create the ipv4 tcam rules manager for this VR*/
            ipTcamMemPoolMgm = prvCpssDxChLpmTcamMgmCreate(numOfIpv4Prefixes,
                                                           ipv4IdxSegments.segment, ipv4SegNum,
                                                           NULL, 0, NULL,NULL);

            if ((ipTcamMemPoolMgm == NULL) && (numOfIpv4Prefixes > 0))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

            ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV4_E]
                                                                = ipTcamMemPoolMgm;

            /* create the ipv4 mc src tcam rules manager for this VR*/
            ipTcamMemPoolMgm = prvCpssDxChLpmTcamMgmCreate(ipTcamShadowPtr->numOfIpMcSrcAddr,
                                                           mcSrcIdxSegments.segment,mcSegNum,
                                                           NULL, 0, NULL,NULL);

            if ((ipTcamMemPoolMgm == NULL) && (ipTcamShadowPtr->numOfIpMcSrcAddr > 0))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

            ipTcamShadowPtr->tcamEntriesHandler->ipMcSrcTcamPool = ipTcamMemPoolMgm;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* ipTcamMemPoolCheetah3Init
*
* DESCRIPTION:
*       This function initializes the managment pool for Cheetah3 in Router Tcam.
*
* INPUTS:
*       tcamStartIndex    -  first TCAM line allocated to ip
*       tcamEndIndex      -  last TCAM line allocated to ip
*       numOfIpv4Prefixes -  total number of IPv4 prefixes to manage
*       numOfIpv6Prefixes -  total number of IPv4 prefixes to manage
*       numOfMcSrcAddr    -  total number of Mc source addresses to manage
*       protocolStack     -  the protocol stack to support
*       ipTcamShadowPtr   -  the shadow to alloc the pools for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NO_RESOURCE    - if number of prefixes exceeds number of indexes in table
*       GT_OUT_OF_CPU_MEM - if unable to create pool.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamMemPoolCheetah3Init
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpv4Prefixes,
    IN GT_U32                                           numOfIpv6Prefixes,
    IN GT_U32                                           numOfMcSrcAddr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr
)
{
    return ipTcamMemPoolRouterTcamInit(tcamStartIndex, tcamEndIndex,
                                       numOfIpv4Prefixes, numOfIpv6Prefixes,
                                       numOfMcSrcAddr, protocolStack,
                                       ipTcamShadowPtr, 0, NULL);
}

/*******************************************************************************
* ipTcamMemPoolXcatInit
*
* DESCRIPTION:
*       This function initializes the managment pool for Xcat in Router Tcam.
*
* INPUTS:
*       tcamStartIndex    -  first TCAM line allocated to ip
*       tcamEndIndex      -  last TCAM line allocated to ip
*       numOfIpv4Prefixes -  total number of IPv4 prefixes to manage
*       numOfIpv6Prefixes -  total number of IPv4 prefixes to manage
*       numOfMcSrcAddr    -  total number of Mc source addresses to manage
*       protocolStack     -  the protocol stack to support
*       ipTcamShadowPtr   -  the shadow to alloc the pools for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NO_RESOURCE    - if number of prefixes exceeds number of indexes in table
*       GT_OUT_OF_CPU_MEM - if unable to create pool.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamMemPoolXcatInit
(
    IN GT_U32                                           tcamStartIndex,
    IN GT_U32                                           tcamEndIndex,
    IN GT_U32                                           numOfIpv4Prefixes,
    IN GT_U32                                           numOfIpv6Prefixes,
    IN GT_U32                                           numOfMcSrcAddr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *ipTcamShadowPtr
)
{
    GT_STATUS                           rc = GT_OK;
    GT_U32                              row;
    GT_U32                              column;
    GT_U32                              i;
    GT_U32                              tcamIndex;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC *poolPtr;

    /* first call the Cheetah3 init function */
    rc = ipTcamMemPoolRouterTcamInit(tcamStartIndex, tcamEndIndex,
                                     numOfIpv4Prefixes, numOfIpv6Prefixes,
                                     numOfMcSrcAddr, protocolStack,
                                     ipTcamShadowPtr, 0, NULL);
    if (rc != GT_OK)
        return rc;

    /* Cheetah 3 function prepared pools with the assumption that the TCAM
       search order is column by column. In Xcat the TCAM search order is
       row by row. Therefore the real TCAM values will be modified to reflect
       that. Note that the sizes of the pools is the same for Cheetah 3 and
       for Xcat and only the real TCAM indexes are modified.
       The indexes of the TCAM will be column by column (according to Cheetah 3
       format and not according to Xcat format) in order to prevent changes in
       other ip lpm code.
       Note that checking that all prefixes can fit to the TCAM is done in
       pool creation so it is guaranteed that the prefixes can fit in the TCAM
       range allocated to the LPM DB.
       The division of the TCAM will be according to the following figure:

       tcamStartIndex  --> -------------------------------------
                           |        |        |        |        |
                           | MC src | MC src | MC src | MC src |
                           |        |        |        |        |
                           |-----------------------------------|
                           |        |        |        |        |
                           |        |        |        |        |
                           |  ipv4  |  ipv4  |  ipv4  |  ipv4  |
                           |        |        |        |        |
                           |        |        |        |        |
                           |        |        |        |        |
                           |-----------------------------------|
                           |                                   |
                           |               ipv6                |
                           |                                   |
       tcamEndIndex   -->  ------------------------------------- */

    /* index allocation starts at the beginning of LPM DB TCAM lines */
    row = tcamStartIndex;
    column = 0;

    /* first deal with re-index the IPv4 MC src */
    poolPtr = ipTcamShadowPtr->tcamEntriesHandler->ipMcSrcTcamPool;

    /* re-allocate pool indexes only if the pool was created */
    if (poolPtr != NULL)
    {
        for (i = 0 ; i < poolPtr->coreIpNumOfIpTcamRules ; i++)
        {
            tcamIndex = column * PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS + row;

            poolPtr->coreIpTcamRulesAllocArray[i].offset = tcamIndex;

            /* move to next TCAM index (according to Xcat search order) */
            if (column == 3)
            {
                row++;
                column = 0;
            }
            else
                column++;
        }
    }

    /* now deal with IPv4 prefixes */
    poolPtr = ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV4_E];

    /* re-allocate pool indexes only if the pool was created */
    if (poolPtr != NULL)
    {
        for (i = 0 ; i < poolPtr->coreIpNumOfIpTcamRules ; i++)
        {
            tcamIndex = column * PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS + row;

            poolPtr->coreIpTcamRulesAllocArray[i].offset = tcamIndex;

            /* move to next TCAM index (according to Xcat search order) */
            if (column == 3)
            {
                row++;
                column = 0;
            }
            else
                column++;
        }
    }

    /* now deal with IPv6 prefixes */
    poolPtr = ipTcamShadowPtr->tcamEntriesHandler->ipTcamPool.tcamMgm[PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS][CPSS_IP_PROTOCOL_IPV6_E];

    /* IPv6 indexes must be at column 0 */
    if (column != 0)
    {
        column = 0;
        row++;
    }

    /* re-allocate pool indexes only if the pool was created */
    if (poolPtr != NULL)
    {
        for (i = 0 ; i < poolPtr->coreIpNumOfIpTcamRules ; i++)
        {
            tcamIndex = column * PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS + row;

            poolPtr->coreIpTcamRulesAllocArray[i].offset = tcamIndex;

            /* move to next row */
            row++;
        }
    }

    /* make sure that last row is still within TCAM range; note that row is 1
       more than last allocated row as it was increased in the loop */
    if ((row-1) > tcamEndIndex)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerGetMemPoolPtr
*
* DESCRIPTION:
*       In partition mode, the TCAM entries handler is used as a glue layer to
*       the common tcam pool manager code which implements a partitioned TCAM.
*       This function gets the TCAM pool manager for specific entry type.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*
* OUTPUTS:
*       memPoolPtrPtr         - memory pool used by TCAM entries handler for
*                               this entry type
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerGetMemPoolPtr
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    OUT PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC                     **memPoolPtrPtr
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U32                      index;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(memPoolPtrPtr);

    switch (tcamHandlerPtr->tcamShadowType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
        index = PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
        index = /*(GT_U8)((vrId == 0) ?*/
                PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS/* : PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS)*/;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
        index = PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
        index = PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS;
        break;
    case  PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        index = PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
        *memPoolPtrPtr = tcamHandlerPtr->ipTcamPool.tcamMgm[index][0];
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        *memPoolPtrPtr = tcamHandlerPtr->ipMcSrcTcamPool;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        *memPoolPtrPtr = tcamHandlerPtr->ipTcamPool.tcamMgm[index][1];
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NON_IP_ENTRY_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxCh3TcamEntriesHandlerCreate
*
* DESCRIPTION:
*       The TCAM entries handler is responsible for handling the TCAM entries
*       and deciding which entries to allocate to each prefix type.
*       The TCAM entries handler has two modes of operation:
*         - Partition mode. In this mode the TCAM range allocated for the TCAM
*           entries handler in partitioned so each entry type has fixed section
*           of the TCAM.
*         - No partition mode. In this mode the TCAM range is not partitioned.
*           All entry types share the TCAM entries and TCAM entries are
*           allocated on demand.
*       In partition mode, the TCAM entries handler is used as a glue layer to
*       the common tcam pool manager code which implements a partitioned TCAM.
*       In no partition mode, the TCAM entries handler implements a dynamic
*       method for a non partitioned TCAM.
*       This function creates the TCAM entries handler in the no partition mode
*       of operation.
*
* INPUTS:
*       ipTcamShadowPtr       - the shadow type to create the TCAM entries handler for
*       indexesRangePtr       - the range of TCAM indexes allocated for this TCAM entries
*                               handler
*       partitionEnable       - GT_TRUE:  partition the TCAM range according to the
*                                         capacityCfgPtr, any unused TCAM entries will
*                                         be allocated to IPv4 UC entries
*                               GT_FALSE: allocate TCAM entries on demand while
*                                         guarantee entries as specified in capacityCfgPtr
*       capacityCfgPtr        - capacity configuration
*       protocolStack         - the protocol stack to support
*       tcamLines             - total number of lines in the TCAM
*
* OUTPUTS:
*       ipTcamShadowPtr       - the shadow type to create the TCAM entries handler for
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Supported only for Cheetah3 devices with no partition mode.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3TcamEntriesHandlerCreate
(
    INOUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN    GT_BOOL                                       partitionEnable,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC *capacityCfgPtr,
    IN    CPSS_IP_PROTOCOL_STACK_ENT                    protocolStack,
    IN    GT_U32                                        tcamLines
)
{
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC  *tcamHandler;           /* used to shorten text */
    GT_U32                                      lineUsageArraySize;
    GT_U32                                      entriesUsageArraySize;
    GT_U32                                      i;

    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr);
    CPSS_NULL_PTR_CHECK_MAC(indexesRangePtr);
    CPSS_NULL_PTR_CHECK_MAC(capacityCfgPtr);

    /* prevent compiler warnings */
    protocolStack = protocolStack;

    /* currently supported only for Cheetah3/Xcat devices with no partition mode */
    if (((ipTcamShadowPtr->shadowType != PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E) &&
        (ipTcamShadowPtr->shadowType != PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E)) ||
        (partitionEnable != GT_FALSE))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    tcamHandler = ipTcamShadowPtr->tcamEntriesHandler;

    tcamHandler->firstTcamLine    = indexesRangePtr->firstIndex;
    tcamHandler->numOfTcamLines   = indexesRangePtr->lastIndex - indexesRangePtr->firstIndex + 1;
    tcamHandler->numOfTcamEntries = tcamHandler->numOfTcamLines * 4;
    tcamHandler->tcamTotalLines   = tcamLines;

    tcamHandler->allocatedIpv4       = 0;
    tcamHandler->allocatedIpv4McSrc  = 0;
    tcamHandler->allocatedIpv6       = 0;
    tcamHandler->guaranteedIpv4      = capacityCfgPtr->numOfIpv4Prefixes;
    tcamHandler->guaranteedIpv4McSrc = capacityCfgPtr->numOfIpv4McSourcePrefixes;
    tcamHandler->guaranteedIpv6      = capacityCfgPtr->numOfIpv6Prefixes;

    switch (ipTcamShadowPtr->shadowType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E:
    case PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_SHADOW_E:
        tcamHandler->tcamSearchOrder = PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_XCAT_SHADOW_E:
        tcamHandler->tcamSearchOrder = PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* do some sanity checks */
    if ((tcamHandler->firstTcamLine + tcamHandler->numOfTcamLines) >= tcamHandler->tcamTotalLines)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (((tcamHandler->guaranteedIpv6 * 4) +
         tcamHandler->guaranteedIpv4McSrc +
         tcamHandler->guaranteedIpv4) > tcamHandler->numOfTcamEntries)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* allocate entries prefix array */
    tcamHandler->prefixPtrArray = (PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC*)
        cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC) * tcamHandler->numOfTcamEntries);
    if (tcamHandler->prefixPtrArray == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* usage of TCAM entries is represented by 1 bit per entry (32 entries per word) */
    entriesUsageArraySize = (tcamHandler->numOfTcamEntries - 1) / 32 + 1;
    tcamHandler->entriesUsagePtr = (GT_U32 *)cpssOsMalloc(sizeof(GT_U32) * entriesUsageArraySize);
    if (tcamHandler->entriesUsagePtr == NULL)
    {
        cpssOsFree(tcamHandler->prefixPtrArray);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* usage of each TCAM line is represented by 4 bits (8 lines per word) */
    lineUsageArraySize = (tcamHandler->numOfTcamLines - 1) / 8 + 1;
    tcamHandler->lineUsagePtr = (GT_U32 *)cpssOsMalloc(sizeof(GT_U32) * lineUsageArraySize);
    if (tcamHandler->lineUsagePtr == NULL)
    {
        cpssOsFree(tcamHandler->entriesUsagePtr);
        cpssOsFree(tcamHandler->prefixPtrArray);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* mark all tcam entries/lines as not used */
    for (i = 0 ; i < tcamHandler->numOfTcamEntries ; i++)
        tcamHandler->prefixPtrArray[i].valid = GT_FALSE;
    cpssOsMemSet(tcamHandler->entriesUsagePtr,0,(sizeof(GT_U32) * entriesUsageArraySize));
    cpssOsMemSet(tcamHandler->lineUsagePtr,0,(sizeof(GT_U32) * lineUsageArraySize));

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerAllocate
*
* DESCRIPTION:
*       Allocates TCAM entry for specific type of prefix. The entry is allocated
*       in a range that is bounded by two other entries (bounded in the meaning
*       of TCAM search order). The entry is allocated according to specific
*       allocation method.
*
* INPUTS:
*       ipTcamShadowPtr       - the ip shadow pointer we are working on
*       entryType             - entry type to allocate
*       lowerBoundPtr         - the lower bound; the allocated rule reside after
*                               this rule; if NULL then no lower bound
*       upperBoundPtr         - the upper bound; the allocated rule reside before
*                               this rule; if NULL then no upper bound
*       allocMethod           - the allocation method
*       defragEnable          - whether to defrag entries in case there is no
*                               free place for this entry cause of entries
*                               fragmentation; relevant only if the TCAM entries
*                               handler was created with partitionEnable = GT_FALSE
*
* OUTPUTS:
*       allocNodePtrPtr       - the allocated rule node
*
* RETURNS:
*       GT_OK                     - if allocation succedded
*       GT_OUT_OF_PP_MEM          - if there is no space left between the upper
*                                   and lower bounds
*       GT_OUT_OF_RANGE           - if the indexes were out of range
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerAllocate
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                      *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *lowerBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *upperBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                                defragEnable,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
)
{
    GT_STATUS rc = GT_OK;
    /* pointer to TCAM entries handler */
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC  *tcamHandlerPtr;

    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr->tcamEntriesHandler);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtrPtr);

   tcamHandlerPtr = ipTcamShadowPtr->tcamEntriesHandler;

    /* check there is enough place for the new entry */
    rc = tcamHandlerCheckAvailbleMem(tcamHandlerPtr,entryType,1);
    if (rc != GT_OK)
        return rc;

    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        rc = tcamHandlerAllocateEntry(tcamHandlerPtr,lowerBoundPtr,upperBoundPtr,
                                      allocMethod,defragEnable,allocNodePtrPtr);
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        rc = tcamHandlerAllocateLine(ipTcamShadowPtr,lowerBoundPtr,upperBoundPtr,
                                     allocMethod,defragEnable,allocNodePtrPtr);
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NON_IP_ENTRY_E:
    default:
        rc = GT_NOT_SUPPORTED;
    }

    if (rc != GT_OK)
        return rc;

    /* increase number of prefixes allocated in the TCAM handler by 1 */
    rc = tcamHandlerUpdatePrefixesAllocated(tcamHandlerPtr,entryType,1,GT_TRUE);
    if (rc != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerAllocateEntry
*
* DESCRIPTION:
*       Allocates single TCAM entry. The entry is allocated in a range that is
*       bounded by two other entries (bounded in the meaning of TCAM search
*       order). The entry is allocated according to specific allocation method.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       lowerBoundPtr         - the lower bound; the allocated rule reside after
*                               this rule; if NULL then no lower bound
*       upperBoundPtr         - the upper bound; the allocated rule reside before
*                               this rule; if NULL then no upper bound
*       allocMethod           - the allocation method
*       defragEnable          - whether to defrag entries in case there is no
*                               free place for this entry cause of entries
*                               fragmentation; relevant only if the TCAM entries
*                               handler was created with partitionEnable = GT_FALSE
*
* OUTPUTS:
*       allocNodePtrPtr       - the allocated rule node
*
* RETURNS:
*       GT_OK                     - if allocation succedded
*       GT_OUT_OF_PP_MEM          - if there is no space left between the upper
*                                   and lower bounds
*       GT_OUT_OF_RANGE           - if the indexes were out of range
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerAllocateEntry
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                *lowerBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                *upperBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                             defragEnable,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                **allocNodePtrPtr
)
{
    GT_STATUS   rc;                 /* function return code                 */
    GT_U32      lowerBoundIndex;    /* entry index of lower bound           */
    GT_U32      upperBoundIndex;    /* entry index of upper bound           */
    GT_U32      lowerBoundWord;     /* word that contains lower index       */
    GT_U32      upperBoundWord;     /* word that contains upper index       */
    GT_U32      searchWord;         /* word scanned for free entry          */
    GT_BOOL     searchUp;           /* search direction: UP or DOWN         */
    GT_U32      searchWordBitmap = 0xFFFFFFFF; /* entries usage bitmap for search word */
    GT_U32      midAllocLower = 0;  /* lowest word scanned in mid scan      */
    GT_U32      midAllocUpper = 0;  /* uppest word scanned in mid scan      */
    GT_U32      allocatedIndex;     /* index choosen to be allocated        */

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtrPtr);

    /* allocating single entry on Cheetah3 never require defrag */
    /* prevent compiler warnings                                */
    defragEnable = defragEnable;

    /********************************************************************/
    /* first convert upper and lower bounds offsets to indexes that can */
    /* be allocated to the new entry (not including the boundaries)     */
    /********************************************************************/

    if (lowerBoundPtr == NULL)
        lowerBoundIndex = 0;
    else
    {
        rc = convertOffsetToIndex(tcamHandlerPtr,lowerBoundPtr->offset,GT_TRUE,&lowerBoundIndex);
        if (rc != GT_OK)
            return rc;
        if (lowerBoundIndex >= tcamHandlerPtr->numOfTcamEntries - 1)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        lowerBoundIndex++;
    }

    if (upperBoundPtr == NULL)
    {
        upperBoundIndex = tcamHandlerPtr->numOfTcamEntries - 1;
    }
    else
    {
        rc = convertOffsetToIndex(tcamHandlerPtr,upperBoundPtr->offset,GT_TRUE,&upperBoundIndex);
        if (rc != GT_OK)
            return rc;
        if (upperBoundIndex == 0)/* There is no free space in TCAM */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        upperBoundIndex--;
    }

    if (upperBoundIndex >= tcamHandlerPtr->numOfTcamEntries)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (upperBoundIndex < lowerBoundIndex)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);

    /*****************************/
    /* prepare search parameters */
    /*****************************/

    /* convert boundary from index to words containing that index */
    /* remember each word holds usage of 32 entries               */
    lowerBoundWord = lowerBoundIndex / 32;
    upperBoundWord = upperBoundIndex / 32;

    /* set search initial index according to search method */
    switch (allocMethod)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MIN_E:
        searchWord = lowerBoundWord;
        searchUp = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MID_E:
        searchWord = (lowerBoundWord + upperBoundWord) / 2;
        midAllocLower = midAllocUpper = searchWord;
        searchUp = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MAX_E:
        searchWord = upperBoundWord;
        searchUp = GT_FALSE;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /*******************************************************************************/
    /* scan the entries usage array while searched word is still within boundaries */
    /*******************************************************************************/

    while ((lowerBoundWord <= searchWord) && (searchWord <= upperBoundWord))
    {
        searchWordBitmap = tcamHandlerPtr->entriesUsagePtr[searchWord];

        /* lower bound index might be in the middle on the lower bound word */
        /* in such case mark indexes before lower index as if they are used */
        /* remember each word holds usage of 32 entries                     */
        if ((lowerBoundWord == searchWord) && ((lowerBoundIndex % 32) != 0))
            searchWordBitmap |= (0xFFFFFFFF >> (32 - (lowerBoundIndex % 32)));

        /* upper bound index might be in the middle on the upper bound word */
        /* in such case mark indexes after upper index as if they are used  */
        /* remember each word holds usage of 32 entries                     */
        if ((upperBoundWord == searchWord) && ((upperBoundIndex % 32) != 31))
            searchWordBitmap |= (0xFFFFFFFF << ((upperBoundIndex % 32) + 1));

        /* if search word bitmap != 0xFFFFFFFF then there is empty entry in this word */
        if (searchWordBitmap != 0xFFFFFFFF)
            break;

        /* free entry wasn't found, continue to next word according to search method */
        if (allocMethod == PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MID_E)
        {
            /* in mid allocation method, the search starts in the middle */
            /* of the range, then one upper word is scanned, then one    */
            /* lower word, then another upper word and so on till the    */
            /* entire range is scanned for a free entry; the upper and   */
            /* lowest words that were already scanned within the range   */
            /* are stored in midAllocLower and midAllocUpper             */

            if (searchUp == GT_TRUE)
            {
                /* if we reached upper bound, don't cross the upper  */
                /* bound; continue with lower indexes instead        */
                if (searchWord >= upperBoundWord)
                    searchWord = --midAllocLower;
                else
                    searchWord = ++midAllocUpper;
                searchUp = GT_FALSE;
            }
            else
            {
                /* if we reached lower bound, don't cross the lower  */
                /* bound; continue with upper indexes instead        */
                if (searchWord <= lowerBoundWord)
                    searchWord = ++midAllocUpper;
                else
                    searchWord = --midAllocLower;
                searchUp = GT_TRUE;
            }
        }
        else /* allocation mode is min or max */
        {
            if (searchUp == GT_TRUE)
                searchWord++;
            else
                searchWord--;
        }
    }

    /*******************************************/
    /* scan was completed, analyze the results */
    /*******************************************/

    /* if searchWordBitmap == 0xFFFFFFFF then there is no free entry in the range */
    if (searchWordBitmap == 0xFFFFFFFF)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);

    /* free entry was found, calculate the allocatedIndex */
    /* remember each word holds usage of 32 entries       */
    if (searchUp == GT_TRUE)
    {
        allocatedIndex = searchWord * 32;
        for ( ; searchWordBitmap != 0 ; searchWordBitmap >>= 1, allocatedIndex++)
        {
            if ((searchWordBitmap & 0x1) == 0)
                break;
        }
    }
    else /* searchUp == GT_FALSE */
    {
        allocatedIndex = (searchWord + 1) * 32 - 1;
        for ( ; searchWordBitmap != 0 ; searchWordBitmap <<= 1, allocatedIndex--)
        {
            if ((searchWordBitmap & 0x80000000) == 0)
                break;
        }
    }

    /* mark the allocated entry as used */
    rc = tcamHandlerSetEntryUsage(tcamHandlerPtr,allocatedIndex,GT_TRUE);
    if (rc != GT_OK)
        return rc;

    /* convert index to offset */
    *allocNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[allocatedIndex];
    rc = convertIndexToOffset(tcamHandlerPtr,allocatedIndex,GT_TRUE,&((**allocNodePtrPtr).offset));
    if (rc != GT_OK)
        return rc;

    (**allocNodePtrPtr).valid = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerAllocateLine
*
* DESCRIPTION:
*       Allocates line TCAM entry. The line is allocated in a range that is
*       bounded by two other lines (bounded in the meaning of TCAM search
*       order). The line is allocated according to specific allocation method.
*
* INPUTS:
*       ipTcamShadowPtr       - the shadow pointer we are working on
*       lowerBoundPtr         - the lower bound; the allocated rule reside after
*                               this rule; if NULL then no lower bound
*       upperBoundPtr         - the upper bound; the allocated rule reside before
*                               this rule; if NULL then no upper bound
*       allocMethod           - the allocation method
*       defragEnable          - whether to defrag entries in case there is no
*                               free place for this entry cause of entries
*                               fragmentation; relevant only if the TCAM entries
*                               handler was created with partitionEnable = GT_FALSE
*
* OUTPUTS:
*       allocNodePtrPtr       - the allocated rule node
*
* RETURNS:
*       GT_OK                     - if allocation succedded
*       GT_OUT_OF_PP_MEM          - if there is no space left between the upper
*                                   and lower bounds
*       GT_OUT_OF_RANGE           - if the indexes were out of range
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerAllocateLine
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                    *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                *lowerBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                *upperBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                             defragEnable,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                **allocNodePtrPtr
)
{
    GT_STATUS   rc;                     /* function return code             */
    GT_U32      lowerBoundIndex;        /* line index of lower bound        */
    GT_U32      upperBoundIndex;        /* line index of upper bound        */
    GT_U32      searchIndex;            /* index scanned for free line      */
    GT_BOOL     searchUp;               /* search direction: UP or DOWN     */
    GT_U32      midAllocLower = 0;      /* lowest index scanned in mid scan */
    GT_U32      midAllocUpper = 0;      /* uppest index scanned in mid scan */
    GT_U32      lineToFree;             /* line to free in case of defrag   */
    GT_U32      entriesInLineToFree;    /* entries in line to free          */
    GT_U32      currentLineUsage;       /* entry usage of current line      */
    GT_U32      entriesInCurrentLine;   /* entries in current line          */
    GT_U32      i;                      /* counter                          */

    /* pointer to TCAM entries handler */
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC  *tcamHandlerPtr ;

    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr->tcamEntriesHandler);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtrPtr);

    tcamHandlerPtr = ipTcamShadowPtr->tcamEntriesHandler;

    /********************************************************************/
    /* first convert upper and lower bounds offsets to indexes that can */
    /* be allocated to the new entry (not including the boundaries)     */
    /********************************************************************/

    if (lowerBoundPtr == NULL)
        lowerBoundIndex = 0;
    else
    {
        rc = convertOffsetToIndex(tcamHandlerPtr,lowerBoundPtr->offset,GT_FALSE,&lowerBoundIndex);
        if (rc != GT_OK)
            return rc;
        if (lowerBoundIndex >= tcamHandlerPtr->numOfTcamLines - 1)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        lowerBoundIndex++;
    }

    if (upperBoundPtr == NULL)
        upperBoundIndex = tcamHandlerPtr->numOfTcamLines - 1;
    else
    {
        rc = convertOffsetToIndex(tcamHandlerPtr,upperBoundPtr->offset,GT_FALSE,&upperBoundIndex);
        if (rc != GT_OK)
            return rc;
        if (upperBoundIndex == 0)/* There is no free space in TCAM */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        upperBoundIndex--;
    }

    if (upperBoundIndex >= tcamHandlerPtr->numOfTcamLines)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (upperBoundIndex < lowerBoundIndex)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);

    /*****************************/
    /* prepare search parameters */
    /*****************************/

    /* set search initial index according to search method */
    switch (allocMethod)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MIN_E:
        searchIndex = lowerBoundIndex;
        searchUp = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MID_E:
        searchIndex = (lowerBoundIndex + upperBoundIndex) / 2;
        midAllocLower = midAllocUpper = searchIndex;
        searchUp = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MAX_E:
        searchIndex = upperBoundIndex;
        searchUp = GT_FALSE;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    lineToFree = searchIndex;
    entriesInLineToFree = 5;    /* init value larger than all possible values */

    /****************************************************************************/
    /* scan the line usage array while searched word is still within boundaries */
    /* during the scan look for line to be candidate for defrag (first line     */
    /* found with mimimal number of used entries in it)                         */
    /****************************************************************************/

    while ((lowerBoundIndex <= searchIndex) && (searchIndex <= upperBoundIndex))
    {
        /* get the current line entries usage                  */
        /* remember each word in line usage array holds usage  */
        /* of 8 lines and each line is represented with 4 bits */
        currentLineUsage = (tcamHandlerPtr->lineUsagePtr[searchIndex / 8] >> ((searchIndex % 8) * 4)) & 0xF;

        /* check if current line is free */
        if (currentLineUsage == 0)
            break;

        /* count number of entries used in the line currently scanned */
        for (i = 0 , entriesInCurrentLine = 0 ; i < 4 ; i++)
        {
            if (((currentLineUsage >> i) & 0x1) == 1)
                entriesInCurrentLine++;
        }

        /* check if this line is better candidate as line to free in case of defrag   */
        /* remember that lines with less used entries are considered better candidate */
        if (entriesInCurrentLine < entriesInLineToFree)
        {
            lineToFree = searchIndex;
            entriesInLineToFree = entriesInCurrentLine;
        }

        /* free line wasn't found, continue to next word according to search method */
        if (allocMethod == PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MID_E)
        {
            /* in mid allocation method, the search starts in the middle */
            /* of the range, then one upper line is scanned, then one    */
            /* lower line, then another upper line and so on till the    */
            /* entire range is scanned for a free line; the upper and    */
            /* lowest lines that were already scanned within the range   */
            /* are stored in midAllocLower and midAllocUpper             */

            if (searchUp == GT_TRUE)
            {
                /* if we reached upper bound, don't cross the upper  */
                /* bound; continue with lower indexes instead        */
                if (searchIndex >= upperBoundIndex)
                    searchIndex = --midAllocLower;
                else
                    searchIndex = ++midAllocUpper;
                searchUp = GT_FALSE;
            }
            else
            {
                /* if we reached lower bound, don't cross the lower  */
                /* bound; continue with upper indexes instead        */
                if (searchIndex <= lowerBoundIndex)
                    searchIndex = ++midAllocUpper;
                else
                    searchIndex = --midAllocLower;
                searchUp = GT_TRUE;
            }
        }
        else
        {
            if (searchUp == GT_TRUE)
                searchIndex++;
            else
                searchIndex--;
        }
    }

    /*******************************************/
    /* scan was completed, analyze the results */
    /*******************************************/

    /* check if a free line was found */
    if ((searchIndex < lowerBoundIndex) || (searchIndex > upperBoundIndex))
    {
        /**************************/
        /* no free line was found */
        /**************************/

        if (defragEnable == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        }
        else /* defragEnable == GT_TRUE */
        {
            /* if the best line that was found as candidate to be freed during */
            /* the scan of lines within the boundaries has 4 used entries      */
            /* (or more... which is the initial value) then all lines within   */
            /* boundaries are full; in that case no defrag is performed        */
            if (entriesInLineToFree >= 4)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);

            /* verify there are at least 4 free tcam entries, otherwise no defrag */
            /* can free the line that was chosen as candidate to be freed         */
            /* (remember each line contains 4 entries)                            */
            rc = tcamHandlerCheckAvailbleMem(tcamHandlerPtr,PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E,1);
            if (rc != GT_OK)
                return rc;

            /* free the line that was choosen as line to free */
            rc = tcamHandlerFreeLine(ipTcamShadowPtr,lineToFree);
            if (rc != GT_OK)
                return rc;

            /* now the new allocated line is the line that was just freed */
            searchIndex = lineToFree;
        }
    }

    /* mark the allocated line as used and update the entries bitmap as well */
    rc = tcamHandlerSetLineUsage(tcamHandlerPtr,searchIndex,GT_TRUE);
    if (rc != GT_OK)
        return rc;

    /* convert index to offset */
    if (tcamHandlerPtr->tcamSearchOrder == PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E )
    {
        *allocNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[searchIndex];
    }
    else
    {
        *allocNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[searchIndex*4];
    }

    rc = convertIndexToOffset(tcamHandlerPtr,searchIndex,GT_FALSE,&((**allocNodePtrPtr).offset));
    if (rc != GT_OK)
        return rc;

    (**allocNodePtrPtr).valid = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerFreeLine
*
* DESCRIPTION:
*       Free a line by moving the used entries in this line away from the line.
*
* INPUTS:
*       ipTcamShadowPtr           - the shadow pointer we are working on
*       lineIndexToFree           - index of the line to free
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - if allocation succedded
*       GT_OUT_OF_RANGE           - if the indexes were out of range
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerFreeLine
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                    *ipTcamShadowPtr,
    IN  GT_U32                                              lineIndexToFree
)
{
    GT_STATUS   rc;
    GT_STATUS   rc2;
    GT_U32      lineUsage;
    GT_U32      entryIndex;
    GT_U32      i,j;
    GT_BOOL     entryUsage;
    GT_U32      tempAllocatedValue=0; /* number of allocated prefixes before the relocation */
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT entryType;/* type of entry prefix we are about to relocate */
    /* pointer to TCAM entries handler */
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC  *tcamHandlerPtr;

    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipTcamShadowPtr->tcamEntriesHandler);

    tcamHandlerPtr = ipTcamShadowPtr->tcamEntriesHandler;

    /* backup the current line usage                       */
    /* remember each word in line usage array holds usage  */
    /* of 8 lines and each line is represented with 4 bits */
    lineUsage = (tcamHandlerPtr->lineUsagePtr[lineIndexToFree / 8] >> ((lineIndexToFree % 8) * 4)) & 0xF;

    /* mark the line to free as used before start move the entries in it so */
    /* the moved entries will not be allocated in this line again           */
    rc = tcamHandlerSetLineUsage(tcamHandlerPtr,lineIndexToFree,GT_TRUE);
    if (rc != GT_OK)
        return rc;

    /* try to move each of the entries */
    for (i = 0 ; i < 4 ; i++)
    {
        /* move entry only if it is used */
        if (((lineUsage >> i) & 0x1) == 0)
            continue;

        /* calculate the index of the entry to be relocated */
        /* translation from row column to index depends on TCAM search order */
        switch (tcamHandlerPtr->tcamSearchOrder)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E:
            entryIndex =  i * tcamHandlerPtr->numOfTcamLines + lineIndexToFree;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E:
            entryIndex = lineIndexToFree * 4 + i;
            break;
        default: /* should never happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        /* before relocating the entry need to decrease the number of allocated
           prefixes by one. Incase we succeed in relocating the prefix, this
           value will be increased by one*/
        entryType = ipTcamShadowPtr->tcamEntriesHandler->prefixPtrArray[entryIndex].trieNode->entryType;
        switch(entryType)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
                tempAllocatedValue = tcamHandlerPtr->allocatedIpv4;
                tcamHandlerPtr->allocatedIpv4--;
                break;
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
                 tempAllocatedValue = tcamHandlerPtr->allocatedIpv4McSrc;
                 tcamHandlerPtr->allocatedIpv4McSrc--;
                 break;
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
                 tempAllocatedValue = tcamHandlerPtr->allocatedIpv6;
                 tcamHandlerPtr->allocatedIpv6--;
                 break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* relocate the entry */
        rc = prvCpssDxChLpmTcamPatTrieRelocatePrefix(ipTcamShadowPtr,tcamHandlerPtr->prefixPtrArray[entryIndex].trieNode);

        /*  after the relocation of the entry need to go
            back to the previously alocated number of prefixes */
        switch(entryType)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
                tcamHandlerPtr->allocatedIpv4 = tempAllocatedValue;
                break;
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
                 tcamHandlerPtr->allocatedIpv4McSrc = tempAllocatedValue;
                 break;
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
        case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
                 tcamHandlerPtr->allocatedIpv6 = tempAllocatedValue;
                 break;
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[dead_error_begin] */default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (rc != GT_OK)
        {
            /* if the relocation the entry did not succeeded, set line usage */
            /* status to the proper state (free entries or original entries  */
            /* that were successfully relocated, set as free while set the   */
            /* rest as used)                                                 */
            for (j = 0 ; j < 4 ; j++)
            {
                /* translation from row column to index depends on TCAM search order */
                switch (tcamHandlerPtr->tcamSearchOrder)
                {
                case PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E:
                    entryIndex = j * tcamHandlerPtr->numOfTcamLines + lineIndexToFree;
                    break;
                case PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E:
                    entryIndex = lineIndexToFree * 4 + j;
                    break;
                default: /* should never happen */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                entryUsage = (lineUsage >> j) & 0x1;
                rc2 = tcamHandlerSetEntryUsage(tcamHandlerPtr,entryIndex,entryUsage);
                if (rc2 != GT_OK)
                    return rc2;
            }

            return rc;
        }

        /* prefix relocation succeed, update line usage */
        lineUsage &= ~(0x1 << i);
    }

    /* all used prefixes were relocated, mark the entire line as free */
    rc = tcamHandlerSetLineUsage(tcamHandlerPtr,lineIndexToFree,GT_FALSE);
    if (rc != GT_OK)
        return rc;

    return GT_OK;
}


/*******************************************************************************
* tcamHandlerSetEntryUsage
*
* DESCRIPTION:
*       Mark entry as used/free.
*
* INPUTS:
*       tcamHandlerPtr            - points to TCAM entries handler
*       entry                     - entry to mark
*       usageState                - GT_TRUE:  mark as used
*                                   GT_FALSE: mark as free
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - if allocation succedded
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerSetEntryUsage
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  GT_U32                                              entry,
    IN  GT_BOOL                                             usageState
)
{
    GT_U32      lineIndex;
    GT_U32      entryColumn;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);

    /* mark usage state at entry usage array        */
    /* remember each word holds usage of 32 entries */
    if (usageState == GT_TRUE)
        tcamHandlerPtr->entriesUsagePtr[entry / 32] |= (0x1 << (entry % 32));
    else
        tcamHandlerPtr->entriesUsagePtr[entry / 32] &= ~(0x1 << (entry % 32));

    /* mark usage state at line usage array                */
    /* remember each word in line usage array holds usage  */
    /* of 8 lines and each line is represented with 4 bits */

    /* translation of entry index to row column depends on TCAM search order */
    switch (tcamHandlerPtr->tcamSearchOrder)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E:
        lineIndex = entry % tcamHandlerPtr->numOfTcamLines;
        entryColumn = entry / tcamHandlerPtr->numOfTcamLines;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E:
        lineIndex = entry / 4;
        entryColumn = entry % 4;
        break;
    default: /* should never happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if (usageState == GT_TRUE)
        tcamHandlerPtr->lineUsagePtr[lineIndex / 8] |= (0x1 << entryColumn << ((lineIndex % 8) * 4));
    else
        tcamHandlerPtr->lineUsagePtr[lineIndex / 8] &= ~(0x1 << entryColumn << ((lineIndex % 8) * 4));

    return GT_OK;
}


/*******************************************************************************
* tcamHandlerSetLineUsage
*
* DESCRIPTION:
*       Mark entire line as used/free.
*
* INPUTS:
*       tcamHandlerPtr            - points to TCAM entries handler
*       line                      - line to mark
*       usageState                - GT_TRUE:  mark as used
*                                   GT_FALSE: mark as free
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - if allocation succedded
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerSetLineUsage
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  GT_U32                                              line,
    IN  GT_BOOL                                             usageState
)
{
    GT_U32      i;
    GT_U32      entryIndex;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);

    /* mark line usage state                               */
    /* remember each word in line usage array holds usage  */
    /* of 8 lines and each line is represented with 4 bits */
    if (usageState == GT_TRUE)
        tcamHandlerPtr->lineUsagePtr[line / 8] |= (0xf << ((line % 8) * 4));
    else
        tcamHandlerPtr->lineUsagePtr[line / 8] &= ~(0xf << ((line % 8) * 4));

    /* now mark entries usage state                 */
    /* remember each word holds usage of 32 entries */
    for (i = 0 ; i < 4 ; i++)
    {
        /* translation of row column to entry index depends of TCAM search order */
        switch (tcamHandlerPtr->tcamSearchOrder)
        {
        case PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E:
            entryIndex = tcamHandlerPtr->numOfTcamLines * i + line;
            break;
        case PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E:
            entryIndex = line * 4 + i;
            break;
        default: /* should never happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        if (usageState == GT_TRUE)
            tcamHandlerPtr->entriesUsagePtr[entryIndex / 32] |= (0x1 << (entryIndex % 32));
        else
            tcamHandlerPtr->entriesUsagePtr[entryIndex / 32] &= ~(0x1 << (entryIndex % 32));
    }

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerAllocByGivenRule
*
* DESCRIPTION:
*       Allocates a rule by a another given rule.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       allocRulePtr          - given rule to allocate the new rule from it
*
* OUTPUTS:
*       allocNodePtrPtr       - the allocated rule node
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_ALREADY_EXIST         - if the rule is already allocated.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerAllocByGivenRule
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC             *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *allocRulePtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
)
{
    GT_STATUS   rc;
    GT_32      ruleIndex;
    GT_BOOL     singleTcamEntry;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocRulePtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtrPtr);

    /* decide if entry type is single entry or line */
    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_FALSE;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* get the index of allocRulePtr */
    rc = tcamHandlerGetRuleIdx(tcamHandlerPtr,entryType,allocRulePtr,&ruleIndex);
    if (rc != GT_OK)
        return rc;

    /* check the rule is not already allocated */
    if (singleTcamEntry == GT_TRUE)
    {
        /* remember each word holds usage of 32 entries */
        if (((tcamHandlerPtr->entriesUsagePtr[ruleIndex / 32] >> (ruleIndex % 32)) & 0x1) != 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }
    else /* TCAM line */
    {
        /* remember each word in line usage array holds usage  */
        /* of 8 lines and each line is represented with 4 bits */
        if (((tcamHandlerPtr->lineUsagePtr[ruleIndex / 8] >> ((ruleIndex % 8) * 4)) & 0xF) != 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    /* mark new allocated rule as used */
    if (singleTcamEntry == GT_TRUE)
    {
        rc = tcamHandlerSetEntryUsage(tcamHandlerPtr,ruleIndex,GT_TRUE);
        if (rc != GT_OK)
            return rc;
    }
    else /* TCAM line */
    {
        rc = tcamHandlerSetLineUsage(tcamHandlerPtr,ruleIndex,GT_TRUE);
        if (rc != GT_OK)
            return rc;
    }

    /* convert index to offset */
    if (singleTcamEntry == GT_TRUE)
    {
        *allocNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[ruleIndex];
        rc = convertIndexToOffset(tcamHandlerPtr,ruleIndex,singleTcamEntry,&((**allocNodePtrPtr).offset));
    }
    else /* TCAM line */
    {
        if (tcamHandlerPtr->tcamSearchOrder == PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E)
        {
            *allocNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[ruleIndex];
        }
        else
        {
            *allocNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[ruleIndex*4];
        }
        rc = convertIndexToOffset(tcamHandlerPtr,ruleIndex,singleTcamEntry,&((**allocNodePtrPtr).offset));
    }
    if (rc != GT_OK)
        return rc;

    (**allocNodePtrPtr).valid = GT_TRUE;

    /* increase number of prefixes allocated in the TCAM handler by 1 */
    rc = tcamHandlerUpdatePrefixesAllocated(tcamHandlerPtr,entryType,1,GT_TRUE);
    if (rc != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerFree
*
* DESCRIPTION:
*       Frees a TCAM entry.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       allocNodePtr          - the entry to free.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_NOT_FOUND             - if the rule wasn't found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerFree
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    *allocNodePtr
)
{
    GT_STATUS   rc;
    GT_32       ruleIndex;
    GT_BOOL     singleTcamEntry;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtr);

    /* decide if entry type is single entry or line */
    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_FALSE;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* get the index of allocNodePtr */
    rc = tcamHandlerGetRuleIdx(tcamHandlerPtr,entryType,allocNodePtr,&ruleIndex);
    if (rc != GT_OK)
        return rc;

    /* check the rule is not already allocated */
    if (singleTcamEntry == GT_TRUE)
    {
        /* remember each word holds usage of 32 entries */
        if (((tcamHandlerPtr->entriesUsagePtr[ruleIndex / 32] >> (ruleIndex % 32)) & 0x1) != 0x1)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    else /* TCAM line */
    {
        /* remember each word in line usage array holds usage  */
        /* of 8 lines and each line is represented with 4 bits */

        if (((tcamHandlerPtr->lineUsagePtr[ruleIndex / 8] >> ((ruleIndex % 8) * 4)) & 0xF) != 0xF)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* mark rule as free */
    if (singleTcamEntry == GT_TRUE)
    {
        rc = tcamHandlerSetEntryUsage(tcamHandlerPtr,ruleIndex,GT_FALSE);
        if (rc != GT_OK)
            return rc;
    }
    else /* TCAM line */
    {
        rc = tcamHandlerSetLineUsage(tcamHandlerPtr,ruleIndex,GT_FALSE);
        if (rc != GT_OK)
            return rc;
    }

    /* set prefix at prefixes array as not used */
    allocNodePtr->valid = GT_FALSE;
    allocNodePtr->trieNode = NULL;

    /* decrease number of prefixes allocated in the TCAM handler by 1 */
    rc = tcamHandlerUpdatePrefixesAllocated(tcamHandlerPtr,entryType,1,GT_FALSE);
    if (rc != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerCheckAvailbleMem
*
* DESCRIPTION:
*       Checks whether the TCAM has enough available space to add additional
*       entries of specific entry type.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       numOfRulesNeeded      - number of entries needed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if there is room
*       GT_OUT_OF_PP_MEM otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       numOfIpTcamRules != 0!
*
*******************************************************************************/
static GT_STATUS tcamHandlerCheckAvailbleMem
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                                  numOfRulesNeeded
)
{
    GT_U32      *value;
    GT_U32      valueBackup;
    GT_U32      entriesUsed;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);

    if (numOfRulesNeeded == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
        value = &tcamHandlerPtr->allocatedIpv4;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        value = &tcamHandlerPtr->allocatedIpv4McSrc;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        value = &tcamHandlerPtr->allocatedIpv6;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* backup the existing number of rules to be allocated */
    valueBackup = *value;

    /* update number of rules to upcoming calculations */
    *value += numOfRulesNeeded;

    /* calculate number of TCAM entries needed after adding needed rules */
    entriesUsed = 0;
    entriesUsed += (tcamHandlerPtr->allocatedIpv4 > tcamHandlerPtr->guaranteedIpv4) ?
        tcamHandlerPtr->allocatedIpv4 : tcamHandlerPtr->guaranteedIpv4;
    entriesUsed += (tcamHandlerPtr->allocatedIpv4McSrc > tcamHandlerPtr->guaranteedIpv4McSrc) ?
        tcamHandlerPtr->allocatedIpv4McSrc : tcamHandlerPtr->guaranteedIpv4McSrc;
    entriesUsed += (((tcamHandlerPtr->allocatedIpv6 > tcamHandlerPtr->guaranteedIpv6) ?
        tcamHandlerPtr->allocatedIpv6 : tcamHandlerPtr->guaranteedIpv6) * 4);

    /* restore the original number of rules */
    *value = valueBackup;

    /* check if TCAM entries that are needed can't fit in the TCAM handler */
    if (entriesUsed > tcamHandlerPtr->numOfTcamEntries)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerGetRuleIdx
*
* DESCRIPTION:
*       The TCAM entries handler maintains internal array that holds rules (those
*       rules are also associated with the prefixes attached to them).
*       This funtion returns the index of the rule in that array.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       allocNodePtr          - the rule to get its index
*
* OUTPUTS:
*       ruleIdxPtr            - the rule idx.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerGetRuleIdx
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    *allocNodePtr,
    OUT GT_32                                                   *ruleIdxPtr
)
{
    GT_U32      ruleIndex;
    GT_BOOL     singleTcamEntry;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleIdxPtr);

    /* decide if entry type is single entry or line */
    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_FALSE;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* verify rule is in prefix array */
    if (allocNodePtr < tcamHandlerPtr->prefixPtrArray)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    /* get the rule index */
    ruleIndex = (GT_U32)(allocNodePtr - tcamHandlerPtr->prefixPtrArray);

    /* verify rule is in range according to type */
    if (singleTcamEntry == GT_TRUE)
    {
        if (ruleIndex >= tcamHandlerPtr->numOfTcamEntries)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    else /* TCAM line */
    {
        if(tcamHandlerPtr->tcamSearchOrder == PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E)
        {
            ruleIndex = ruleIndex/4;
        }
        if (ruleIndex >= tcamHandlerPtr->numOfTcamLines)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    *ruleIdxPtr = ruleIndex;

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerGetRuleByRuleIdx
*
* DESCRIPTION:
*       The TCAM entries handler maintains internal array that holds rules (those
*       rules are also associated with the prefixes attached to them).
*       This function returns the rule located at the internal rule index in
*       that array.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       ruleIdx               - the rule idx.
*
* OUTPUTS:
*       ruleNodePtrPtr        - the rule node of this index
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamHandlerGetRuleByRuleIdx
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  GT_32                                                   ruleIdx,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                    **ruleNodePtrPtr
)
{
    GT_BOOL     singleTcamEntry;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleNodePtrPtr);

    /* decide if entry type is single entry or line */
    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_FALSE;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if (ruleIdx < 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (singleTcamEntry == GT_TRUE)
    {
        if ((GT_U32)ruleIdx >= tcamHandlerPtr->numOfTcamEntries)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    else /* TCAM line */
    {
        if ((GT_U32)ruleIdx >= tcamHandlerPtr->numOfTcamLines)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    *ruleNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[ruleIdx];
    return GT_OK;
}

/*******************************************************************************
* tcamHandlerSetRealIdxAsAlloc
*
* DESCRIPTION:
*       Marks specific TCAM entry as used and returns the rule associated with
*       this TCAM entry.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       hwIndex               - the hwRule index.
*
* OUTPUTS:
*       allocNodePtrPtr       - points to allocated node
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_SUPPORTED         - if Tcam handler new mode is used
*       GT_NOT_IMPLEMENTED       - others
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS tcamHandlerSetRealIdxAsAlloc
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC             *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  GT_U32                                                 hwIndex,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
)
{
    GT_STATUS   rc;
    GT_BOOL     singleTcamEntry;
    GT_U32      index;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(allocNodePtrPtr);

    /* decide if entry type is single entry or line */
    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_TRUE;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        singleTcamEntry = GT_FALSE;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc = convertOffsetToIndex(tcamHandlerPtr,hwIndex,singleTcamEntry,&index);
    if (rc == GT_OUT_OF_RANGE)
    {
        /* if the hwIndex is out of the range allocated for the TCAM entries
           handler, return NULL node */
        *allocNodePtrPtr = NULL;
        return GT_OK;
    }
    else if (rc != GT_OK)
        return rc;

    /* check the requested rule is free */
    if (singleTcamEntry == GT_TRUE)
    {
        /* remember each word holds usage of 32 entries */
        if (((tcamHandlerPtr->entriesUsagePtr[index / 32] >> (index % 32)) & 0x1) != 0)
        {
            *allocNodePtrPtr = NULL;
            return GT_OK;
        }
    }
    else /* singleTcamEntry == GT_FALSE*/
    {
        /* remember each word in line usage array holds usage  */
        /* of 8 lines and each line is represented with 4 bits */
        if (((tcamHandlerPtr->lineUsagePtr[index / 8] >> ((index % 8) * 4)) & 0xF) != 0)
        {
            *allocNodePtrPtr = NULL;
            return GT_OK;
        }
    }

    /* set the rule as allocated */
    if (singleTcamEntry == GT_TRUE)
    {
        rc = tcamHandlerSetEntryUsage(tcamHandlerPtr,index,GT_TRUE);
        if (rc != GT_OK)
            return rc;
    }
    else /* singleTcamEntry == GT_FALSE*/
    {
        rc = tcamHandlerSetLineUsage(tcamHandlerPtr,index,GT_TRUE);
        if (rc != GT_OK)
            return rc;
    }

    *allocNodePtrPtr = &tcamHandlerPtr->prefixPtrArray[index];

    (*allocNodePtrPtr)->trieNode = NULL;
    (*allocNodePtrPtr)->valid = GT_TRUE;

    /* update number of prefixes allocated in the TCAM handler by 1 */
    rc = tcamHandlerUpdatePrefixesAllocated(tcamHandlerPtr,entryType,1,GT_TRUE);
    if (rc != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* convertOffsetToIndex
*
* DESCRIPTION:
*       Converts offset within the entire TCAM to index within the TCAM section
*       managed by TCAM entries handler.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       offset                - offset in the entire TCAM, ordered column by column
*       isEntry               - GT_TRUE:  converting entry offset
*                               GT_FALSE: converting line offset
*
* OUTPUTS:
*       indexPtr              - points to index within TCAM section managed by TCAM
*                               entries handler
*
* RETURNS:
*       GT_OK                 - on success
*       GT_FAIL               - on error
*       GT_OUT_OF_RANGE       - if offset is out of the range allocated to
*                               the TCAM entries handler
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS convertOffsetToIndex
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  GT_U32                                              offset,
    IN  GT_BOOL                                             isEntry,
    OUT GT_U32                                              *indexPtr
)
{
    GT_U32  tcamLine;       /* TCAM line of the offset   */
    GT_U32  tcamColumn;     /* TCAM column of the offset */

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* Note that on TCAM with row by row search order, the TCAM indexes are
       also row by row. However, LPM DB library still coverts indexes to tcam
       row and tcam column assuming that the indexes are column by column.
       In order to minimize changes in other LPM DB code, the indexes will be
       treated as if they were column by column */

    tcamLine   = offset % tcamHandlerPtr->tcamTotalLines;
    tcamColumn = offset / tcamHandlerPtr->tcamTotalLines;

    /* verify the offset is within the TCAM section allocated to the TCAM entries handler */
    if ((tcamLine < tcamHandlerPtr->firstTcamLine) ||
        (tcamLine > (tcamHandlerPtr->firstTcamLine + tcamHandlerPtr->numOfTcamLines)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch (tcamHandlerPtr->tcamSearchOrder)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E:
        *indexPtr = (tcamColumn  * tcamHandlerPtr->numOfTcamLines) +
            (tcamLine - tcamHandlerPtr->firstTcamLine);
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E:
        /* in case or row search order, sequential lines are not in sequential
           offsets like in case of column search order */
        if (isEntry == GT_TRUE)
        {
            *indexPtr = (tcamLine - tcamHandlerPtr->firstTcamLine) * 4 +
                tcamColumn;
        }
        else /* isEntry == GT_FALSE */
        {
            *indexPtr = tcamLine - tcamHandlerPtr->firstTcamLine;
        }
        break;

    default:
        /* should never happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* convertIndexToOffset
*
* DESCRIPTION:
*       Converts index within the TCAM section managed by TCAM entries handler
*       to offset within the entire TCAM.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       index                 - index within the TCAM section managed by TCAM
*                               entries handler
*       isEntry               - GT_TRUE:  converting entry index
*                               GT_FALSE: converting line index
*
* OUTPUTS:
*       offsetPtr             - points to offset within the entire TCAM, ordered
*                               column by column
*
* RETURNS:
*       GT_OK                 - on success
*       GT_FAIL               - on error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS convertIndexToOffset
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamHandlerPtr,
    IN  GT_U32                                              index,
    IN  GT_BOOL                                             isEntry,
    OUT GT_U32                                              *offsetPtr
)
{
    GT_U32  line;       /* line of the index   */
    GT_U32  column;     /* column of the index */

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);

    /* Note that on TCAM with row by row search order, the TCAM indexes are
       also row by row. However, LPM DB library still coverts indexes to tcam
       row and tcam column assuming that the indexes are column by column.
       In order to minimize changes in other LPM DB code, the indexes will be
       treated as if they were column by column */
    switch (tcamHandlerPtr->tcamSearchOrder)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E:
        line   = index % tcamHandlerPtr->numOfTcamLines;
        column = index / tcamHandlerPtr->numOfTcamLines;
        break;

    case PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E:
        /* in case or row search order, sequential lines are not in sequential
           offsets like in case of column search order */
        if (isEntry == GT_TRUE)
        {
            line   = index / 4;
            column = index % 4;
        }
        else /* isEntry == GT_FALSE */
        {
            line   = index;
            column = 0;
        }
        break;

    default:
        /* should never happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    *offsetPtr = column * (tcamHandlerPtr->tcamTotalLines) +
        line + tcamHandlerPtr->firstTcamLine;

    return GT_OK;
}

/*******************************************************************************
* tcamHandlerUpdatePrefixesAllocated
*
* DESCRIPTION:
*       Update number of prefixes allocated in the TCAM handler.
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       entryType             - entry type
*       amount                - number of entries
*       increase              - GT_TRUE:  increase number of allocated prefixes
*                               GT_FALSE: decrease number of allocated prefixes
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_FAIL               - on error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS tcamHandlerUpdatePrefixesAllocated
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC              *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                                  amount,
    IN  GT_BOOL                                                 increase
)
{
    GT_U32      *value;

    CPSS_NULL_PTR_CHECK_MAC(tcamHandlerPtr);

    switch (entryType)
    {
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E:
        value = &tcamHandlerPtr->allocatedIpv4;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E:
        value = &tcamHandlerPtr->allocatedIpv4McSrc;
        break;
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E:
    case PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E:
        value = &tcamHandlerPtr->allocatedIpv6;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((increase == GT_FALSE) && (*value < amount))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (increase == GT_TRUE)
        *value += amount;
    else
        *value -= amount;

    return GT_OK;
}

