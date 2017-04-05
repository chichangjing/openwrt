/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamEntriesHandler.h
*
* DESCRIPTION:
*       This file contains object to handle TCAM entries.
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
*       $Revision: 6 $
*
*******************************************************************************/

#ifndef __prvCpssDxChLpmTcamEntriesHandlerh
#define __prvCpssDxChLpmTcamEntriesHandlerh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamMgm.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamCommonTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamEntriesHandlerTypes.h>

#define PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_NUM_OF_RULES_IN_COLUMN_CNS 1024
#define PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_NUM_OF_RULES_IN_COLUMN_CNS  512
#define PRV_CPSS_DXCH_LPM_TCAM_CHEETAH3_TCAM_NUM_ROWS_CEILING_CNS 10240
#define PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_TCAM_NUM_OF_RULES_CEILING_CNS 10240

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
);

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
);

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
);

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
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                      *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT  entryType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *lowerBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   *upperBoundPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                                defragEnable,
    OUT PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC                   **allocNodePtrPtr
);

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
);


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
);

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
);

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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmTcamEntriesHandlerh */

