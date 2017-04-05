/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRamMc.h
*
* DESCRIPTION:
*       This file includes functions declarations for controlling the LPM MC
*       tables and structures, and structures definitions for shadow management.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmRamMch
#define __prvCpssDxChLpmRamMch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>

/*******************************************************************************
* prvCpssDxChLpmRamMcEntryAdd
*
* DESCRIPTION:
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address.
*
* INPUTS:
*       vrId              - The virtual private network identifier.
*       ipGroupPtr        - The IP multicast group address.
*       ipGroupPrefix     - The number of bits that are actual valid in,
*                           the ipGroup. (CIDR like)
*       ipSrcArr             - the root address for source base multi tree protocol.
*       ipSrcPrefix       - The number of bits that are actual valid in,
*                           the ipSrcArr. (CIDR like)
*       mcRoutePointerPtr - the mc route entry pointer.
*       override          - update or override an existing entry, or create a new one.
*       insertMode        - how to insert the new entry.
*       protocolStack     - protocol to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success, or
*       GT_BAD_PARAM              - if inserted group is already exist and new ipGroupRuleIndex
*                                   is not equal to current ipGroupRuleIndex, or
*       GT_ERROR                  - if the virtual router does not exist,
*                                   or ipGroupPrefix == 0,
*       GT_FAIL                   - otherwise.
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntryAdd
(
    IN GT_U32                                       vrId,
    IN GT_U8                                        *ipGroupPtr,
    IN GT_U32                                       ipGroupPrefix,
    IN GT_U8                                        ipSrcArr[],
    IN GT_U32                                       ipSrcPrefix,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *mcRoutePointerPtr,
    IN GT_BOOL                                      override,
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT   insertMode,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMcEntryDelete
*
* DESCRIPTION:
*       To delete a particular mc route
*
* INPUTS:
*       vrId                    - The virtual private network identifier.
*       ipGroupPtr              - The IP multicast group address.
*       ipGroupPrefix           - The number of bits that are actual valid in,
*                                 the ipGroup. (CIDR like)
*       ipSrcArr                 - the root address for source base multi tree protocol.
*       ipSrcPrefix              - The number of bits that are actual valid in,
*                                  the ipSrcArr. (CIDR like)
*       protocolStack            - type of ip protocol stack to work on.
*       shadowPtr                - the shadow relevant for the devices asked to act on.
*       roollBack                - GT_TRUE: rollback is taking place.
*                                  GT_FALSE: otherwise.
*       rollBackSrcBucketPtr     - pointer to src bucket.(used only for rollback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to delete the all the multicast entries, call this function
*       with ipSrcArr[0] = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntryDelete
(
    IN GT_U32                                   vrId,
    IN GT_U8                                    *ipGroupPtr,
    IN GT_U32                                   ipGroupPrefix,
    IN GT_U8                                    ipSrcArr[],
    IN GT_U32                                   ipSrcPrefix,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr,
    IN GT_BOOL                                  rollBack,
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *rollBackSrcBucketPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMcEntriesFlush
*
* DESCRIPTION:
*       flushes the multicast IP Routing table and stays with the default entry
*       only.
*
* INPUTS:
*       vrId           - The virtual private network identifier.
*       protocolStack  - protocol to work on.
*       shadowPtr      - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntriesFlush
(
    IN GT_U32                               vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMcEntryGetNext
*
* DESCRIPTION:
*       This function returns the next muticast (ipSrc,ipGroup) entry.
*
* INPUTS:
*       vrId    - The virtual router Id.
*       ipGroupPtr - The ip Group address to get the next entry for.
*       ipGroupPrefixPtr - ipGroup prefix length.
*       ipSrcPtr   - The ip Source address to get the next entry for.
*       ipSrcPrefixPtr - ipSrc prefix length.
*       protocolStack  - type of ip protocol stack to work on.
*       shadowPtr      - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       ipGroupPtr         - The next ip Group address.
*       ipGroupPrefixPtr   - ipGroup prefix length.
*       ipSrcPtr           - The next ip Source address. (must be 16 x GT_U8!)
*       ipSrcPrefixPtr     - ipSrc prefix length.
*       mcRoutePointerPtr- the mc route entry (NH) pointer.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (ipGroup,ipGroupPrefix) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipGroupPrefix) is (0).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntryGetNext
(
    IN    GT_U32                                    vrId,
    INOUT GT_U8                                     *ipGroupPtr,
    INOUT GT_U32                                    *ipGroupPrefixPtr,
    INOUT GT_U8                                     *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcRoutePointerPtr,
    IN    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocolStack,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMcEntrySearch
*
* DESCRIPTION:
*       This function searches for the route entries associates with a given
*       (S,G), prefixes.
*
* INPUTS:
*       vrId                  - The virtual router Id.
*       ipGroupPtr            - The ip Group address to get the next entry for.
*       ipGroupPrefix         - ipGroupPtr prefix length.
*       ipSrcArr              - The ip Source address to get the next entry for.
*       ipSrcPrefix           - ipSrcArr prefix length.
*       protocolStack         - the protocol to work on.
*       shadowPtr             - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       ipNextHopEntryPtr     - A pointer to a next hop struct including the HW
*                               pointer
*
* RETURNS:
*       GT_OK if successful, or
*       GT_NOT_FOUND if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Calling this function with ipGroupPrefix = 0, will return the default
*       Mc route entries.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcEntrySearch
(
    IN GT_U32                                       vrId,
    IN GT_U8                                        *ipGroupPtr,
    IN GT_U32                                       ipGroupPrefix,
    IN GT_U8                                        ipSrcArr[],
    IN GT_U32                                       ipSrcPrefix,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *ipNextHopEntryPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMcDefRouteGet
*
* DESCRIPTION:
*       retrieves the default MC route for the given VR router.
*
* INPUTS:
*       vrId                    - The vrId to which this tree belongs to.
*       shadowPtr               - the shadow relevant for the devices asked to act on.
*       protocol                - the protocol
*
* OUTPUTS:
*       defMcRoutePointerPtr    - A pointer to the default mc route
*
* RETURNS:
*       GT_OK                 - on success.
*       GT_FAIL               - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcDefRouteGet
(
    IN  GT_U32                                    vrId,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC **defMcRoutePointerPtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol
);

/*******************************************************************************
* prvCpssDxChLpmRamMcTraverse
*
* DESCRIPTION:
*       a tree traverse function to set/retrive/count all the memory allocations
*       in this mc , and it's lpm src tree. it is done in DFS fashion.
*
* INPUTS:
*       traverseOp            - the traverse operation needed.
*       memAllocArrayPtr      - the array that holds the information of memory
*                               allocations.
*       memAllocArrayIndexPtr - the index in the array this function should start
*                               using.
*       memAllocArraySize     - the size of the above array.
*       vrId                  - The vrId to which this table belongs to.
*       shadowPtr             - the shadow relevant for the devices asked to act on.
*       protocol              - the protocol
*
* OUTPUTS:
*       stopPointIterPtr      - an iterator that is returned to point where we stopped
*                               in the lpm tree, so we can continue from there in the
*                               next call.
*
* RETURNS:
*       GT_OK                 - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM      - otherwise.
*
* COMMENTS:
*       if *stopPointIter ==0 we start at the begining.
*       and if the returned *stopPointIter==0 the we finished with this tree.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMcTraverse
(
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_OP_ENT    traverseOp,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC     *memAllocArrayPtr,
    IN GT_U32                                       *memAllocArrayIndexPtr,
    IN GT_U32                                       memAllocArraySize,
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    OUT GT_UINTPTR                                  *stopPointIterPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmRamMch */

