/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamCommonTypes.h
*
* DESCRIPTION:
*       Private cpss DXCH LPM library type definitions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmTcamCommonTypesh
#define __prvCpssDxChLpmTcamCommonTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamPatTrie.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamEntriesHandlerTypes.h>

struct PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STCT;

/*
 * typedef: struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC
 *
 * Description:
 *      This struct hold the TCAM managment shadow (lpm DB)
 *
 * Fields:
 *  vrTable                 - Vr-Table shadow.
 *  vrSl                    - the virtual routers skip list.
 *  workDevListPtr          - pointer to the device list to preform the actions on.
 *  shadowDevList           - the device list of the devices sharing this shadow.
 *  numOfIpPrefixes         - holds the maximal number of prefixes for each
 *                            protocol stack.
 *  numOfIpMcSrcAddr        - holds the number of Multicast Source addresses.
 *  isIpVerIsInitialized    - boolean function to indicate which ip version was
 *                            initalized for this shadow.
 *  ipUcTcamFuncsPtrs       - array of ip UC Function ptrs registered in the
 *                            init function for the Add/del (etc.) uc prefix.
 *                            functions intialized for [x][y] x - the shadow
 *                            type , y - the protocol stack.
 *  ipMcTcamFuncsPtrs       - array of ip MC Function ptrs registered in the
 *                            init function for the Add/del (etc.) mc prefix.
 *                            functions intialized for [y] y - the protocol
 *                            stack.
 *  tcamEntriesHandler      - The TCAM entries handler
 *  shadowType              - the shadow type.
 *  tcamIndexRange          - TCAM index range allocated to this shadow
 *  usePbrDefaultMc         - whether to set default rules to match all MC traffic
 *                            when working in policy based routing mode; for details
 *                            refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
 *  pbrMcDefaultRule        - the indexes of the rules used for the MC default
 *                            rules (one per protocol stack)
 *  multipleVirtualRouterSupport - multiple Virtual Router support flag
 *  sharedVrExist           - whether shared virtual router exists
 *  sharedVrId              - the vrId of the shared virtual router (relevant
 *                            only when sharedVrExist == GT_TRUE)
 *  isTcamManagerInternal   - whether the TCAM manager is created internally
 *                            in the LPM DB or external (relevant when
 *                            tcamManagementMode == PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
 * tcamManagerHandlerPtr    - points to  created TCAM manager object.
 * clientId                 - client id allocated by TCAM manager.
 * tcamManagementMode       - tcam management mode.
 * tcamPartitionEnable      - whether partitioning of the router TCAM is enable
 * allocatedIpPrefixCounters- contains counters for inserted ip prefixes for each protocol:
 *                             numOfAllocatedIpPrefixes[0]:ipv4 prefixes(uc and mc group),
 *                             numOfAllocatedIpPrefixes[1]:ipv6 prefixes (uc, mc groups and  mc source),
 *                             numOfAllocatedIpv4McSourcePrefixes:ipv4 mc source prefixes.
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STCT
{
    GT_VOID                                       *vrSl;
    PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *workDevListPtr;
    PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        shadowDevList;
    GT_U32                                        numOfIpPrefixes[2];
    GT_U32                                        numOfIpMcSrcAddr;
    GT_BOOL                                       isIpVerIsInitialized[2];
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC ipUcTcamFuncsPtrs[2];
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_FUNCS_OBJ_STC ipMcTcamFuncsPtrs[2];
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC    *tcamEntriesHandler;
    PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT             shadowType;
    PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC tcamIndexRange;
    GT_BOOL                                       usePbrDefaultMc;
    GT_BOOL                                       multipleVirtualRouterSupport;
    GT_BOOL                                       sharedVrExist;
    GT_U32                                        sharedVrId;
    GT_BOOL                                       isTcamManagerInternal;
    GT_VOID                                       *tcamManagerHandlerPtr;
    GT_U32                                        clientId;
    PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_MODE_ENT    tcamManagementMode;
    GT_BOOL                                       tcamPartitionEnable;
    struct
    {
        GT_U32  numOfAllocatedIpPrefixes[2];
        GT_U32  numOfAllocatedIpv4McSourcePrefixes;
    }allocatedIpPrefixCounters;
}PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC
 *
 * Description: This structure represents either a mc group or source.
 *
 * Fields:
 *      gIndex          - the G-Index , in case of group it's the actual
 *                        index in the tcam, i case of source it's the
 *                        source's group index
 *      isAPointer2Def  - in case of group this will indicate the group points
 *                        to the defualt mc route (this is fake group)
 *      ipMcSrcTrie     - the mc source trie in the case of group address
 *      lttEntryPtr     - the LTT entry pointer as was given by the application.
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STCT
{
    GT_BOOL                                             isAPointer2Def;
    struct PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STCT    *ipMcSrcTrie;
    GT_U16                                              gIndex;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC           *lttEntryPtr;
}PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC
 *
 * Description:
 *      This struct hold the TCAM managment Virtual router Information
 *
 * Fields:
 *  vrId            - the virtual router id.
 *  ipUcTrie        - This Trie structure will hold the IPv4/v6-Uc entries.
 *  ipMcTrie              - This Trie structure will hold the IPv4/v6-Mc entries
 *                          (G and the S).
 *  tcamEntriesHandler    - The TCAM entries handler
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STCT
{
    GT_U32                                              vrId;
    GT_U32                                              valid;
    struct PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STCT    *ipUcTrie[2];
    struct PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STCT    *ipMcTrie[2];
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC          *tcamEntriesHandler;
}PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_TCAM_IP_VR_INFO_STC
 *
 * Description: virtual router HSU/Hotsyc info
 *
 * Fields:
 *
 *   vrId                     - the vr id
 *   supportUc                - whether IPv4/v6 UC is supported for this vr
 *   defUcRouteEntrey         - the next hop information for IPv4/v6 UC default
 *   defUcRouteEntreyRuleIdx  - the rule idx for the IPv4/v6 UC defaults
 *   supportMc                - whether IPv4/v6 MC is supported for this vr
 *   defMcRouteEntrey         - the next hop information for IPv4/v6 MC default
 *   defMcRouteEntreyRuleIdx  - the rule idx for the IPv4/v6 MC defaults
 *   protocolStack            - protocol stack supported by this vr
 *   isLast                   - whether this is the last vr
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_IP_VR_INFO_STCT
{
    GT_U32                                      vrId;
    GT_BOOL                                     supportUc[2];
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT      defUcRouteEntrey[2];
    GT_U32                                      defUcRouteEntreyRuleIdx[2];
    GT_BOOL                                     supportMc[2];
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   defMcRouteEntrey[2];
    GT_U32                                      defMcRouteEntreyRuleIdx[2];
    CPSS_IP_PROTOCOL_STACK_ENT                  protocolStack;
    GT_BOOL                                     isLast;

} PRV_CPSS_DXCH_LPM_TCAM_IP_VR_INFO_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_TCAM_IPV4_UC_PREFIX_STC
 *
 * Description: IPv4 UC prefix insert data
 *
 * Fields:
 *
 *    vrId              - The virtual router identifier.
 *    ipAddr            - The destination IP address of this prefix.
 *    prefixLen         - The prefix length of ipAddr
 *    nextHopInfo       - The next hop info to set for this prefix.
 *    ruleIdx           - The prefix's rule index.
 *    last              - whether this is the last entry
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_IPV4_UC_PREFIX_STCT
{
    GT_U32                                  vrId;
    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  ruleIdx;
    GT_BOOL                                 last;
} PRV_CPSS_DXCH_LPM_TCAM_IPV4_UC_PREFIX_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_TCAM_IPV6_UC_PREFIX_STC
 *
 * Description: IPv6 UC prefix insert data
 *
 * Fields:
 *
 *    vrId              - The virtual router identifier.
 *    ipAddr            - The destination IP address of this prefix.
 *    prefixLen         - The prefix length of ipAddr
 *    nextHopInfo       - The next hop info to set for this prefix.
 *    ruleIdx           - The prefix's rule index.
 *    last              - whether this is the last entry
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_IPV6_UC_PREFIX_STCT
{
    GT_U32                                  vrId;
    GT_IPV6ADDR                             ipAddr;
    GT_U32                                  prefixLen;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  ruleIdx;
    GT_BOOL                                 last;
} PRV_CPSS_DXCH_LPM_TCAM_IPV6_UC_PREFIX_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_TCAM_IPV4_MC_PREFIX_STC
 *
 * Description: A structure to hold multicast route entry info. Used for HSU
 *              Pp synchronization.
 *
 * Fields:
 *      vrId          - The virtual router identifier.
 *      mcGroup       - MC group IP address
 *      grpPrefixLen  - MC group prefix length
 *      ipSrc         - MC source IP address
 *      srcPrefixLen  - MC source prefix length
 *      mcLttEntry    - the mc Route ltt entry.
 *      ruleIdx       - the mc route rule idx.
 *      last          - whether this is the last entry
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_IPV4_MC_PREFIX_STCT
{
    GT_U32                                      vrId;
    GT_IPADDR                                   mcGroup;
    GT_U32                                      grpPrefixLen;
    GT_IPADDR                                   ipSrc;
    GT_U32                                      srcPrefixLen;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   mcLttEntry;
    GT_U32                                      ruleIdx[2];
    GT_BOOL                                     last;
}PRV_CPSS_DXCH_LPM_TCAM_IPV4_MC_PREFIX_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_TCAM_IPV6_MC_PREFIX_STC
 *
 * Description: A structure to hold multicast route entry info. Used for HSU
 *              Pp synchronization.
 *
 * Fields:
 *      vrId          - The virtual router identifier.
 *      mcGroup       - MC group IP address
 *      grpPrefixLen  - MC group prefix length
 *      ipSrc         - MC source IP address
 *      srcPrefixLen  - MC source prefix length
 *      mcLttEntry    - the mc Route ltt entry.
 *      ruleIdx       - the mc route rule idx.
 *      last          - whether this is the last entry
 *      groupScope    - the group scope.
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_IPV6_MC_PREFIX_STCT
{
    GT_U32                                      vrId;
    GT_IPV6ADDR                                 mcGroup;
    GT_U32                                      grpPrefixLen;
    GT_IPV6ADDR                                 ipSrc;
    GT_U32                                      srcPrefixLen;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   mcLttEntry;
    GT_U32                                      ruleIdx[2];
    GT_BOOL                                     last;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  groupScope;
}PRV_CPSS_DXCH_LPM_TCAM_IPV6_MC_PREFIX_STC;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_TCAM_HSU_STAGE_ENT
 *
 * Description: A enum representing an LPM shadow iterartor stage
 *
 * Enumerations:
 *      PRV_DXCH_DXCH_LPM_TCAM_HSU_TCAM_MANAGER_STAGE_E  - TCAM manager stage
 *      PRV_DXCH_DXCH_LPM_TCAM_HSU_VR_STAGE_E            - VR router stage
 *      PRV_DXCH_DXCH_LPM_TCAM_HSU_UC_PREFIX_STAGE_E     - ip uc prefix stage
 *      PRV_DXCH_DXCH_LPM_TCAM_HSU_MC_PREFIX_STAGE_E     - ip mc prefix stage
 *      PRV_DXCH_DXCH_LPM_TCAM_HSU_IP_LAST_E             - last value in enum
 */
typedef enum
{
    PRV_DXCH_DXCH_LPM_TCAM_HSU_TCAM_MANAGER_STAGE_E = 0,
    PRV_DXCH_DXCH_LPM_TCAM_HSU_VR_STAGE_E           = 1,
    PRV_DXCH_DXCH_LPM_TCAM_HSU_UC_PREFIX_STAGE_E    = 2,
    PRV_DXCH_DXCH_LPM_TCAM_HSU_MC_PREFIX_STAGE_E    = 3,
    PRV_DXCH_DXCH_LPM_TCAM_HSU_LAST_E               = 4

}PRV_CPSS_DXCH_LPM_TCAM_HSU_STAGE_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_TCAM_UPDATE_TYPE_ENT
 *
 * Description: A enum representing an LPM tries update mode
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_LPM_TCAM_UPDATE_UC_E       - update UC Trie
 *  PRV_CPSS_DXCH_LPM_TCAM_OVERWRITE_UC_MC_E - overwrite the whole MC UC Tries
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_TCAM_UPDATE_UC_E = 0,
    PRV_CPSS_DXCH_LPM_TCAM_OVERWRITE_UC_MC_E
}PRV_CPSS_DXCH_LPM_TCAM_UPDATE_TYPE_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_TCAM_HSU_IP_ITERATOR_STC
 *
 * Description: A struct that holds an ip coreExMx shadow iterartor
 *
 * Fields:
 *      currMemPtr   - the current memory address we're working on
 *      currMemSize  - the current accumulated memory size.
 *      currIter - the current stage iterator.
 *      currStage - the current stage
 *      currProtocolStack - the current Protocol stack.
 *      currVrId - the current vrId.
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_HSU_IP_ITERATOR_STCT
{
    GT_VOID                              *currMemPtr;
    GT_U32                               currMemSize;
    GT_UINTPTR                           currIter;
    PRV_CPSS_DXCH_LPM_TCAM_HSU_STAGE_ENT currStage;
    CPSS_IP_PROTOCOL_STACK_ENT           currProtocolStack;
    GT_UINTPTR                           currVrId;
}PRV_CPSS_DXCH_LPM_TCAM_HSU_IP_ITERATOR_STC;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_TCAM_IP_MC_ENTRY_TYPE_ENT
 *
 * Description: This enum represents mc entry types
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_E  - mc group entry
 *  PRV_CPSS_DXCH_LPM_TCAM_IP_MC_SOURCE_E - mc source entry
 *  PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_AND_SOURCE_E - mc entry that consists of group and source
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_E,
    PRV_CPSS_DXCH_LPM_TCAM_IP_MC_SOURCE_E,
    PRV_CPSS_DXCH_LPM_TCAM_IP_MC_GROUP_AND_SOURCE_E
}PRV_CPSS_DXCH_LPM_TCAM_IP_MC_ENTRY_TYPE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmTcamCommonTypesh */

