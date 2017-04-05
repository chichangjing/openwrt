/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRamUc.h
*
* DESCRIPTION:
*       This file includes functions declarations for controlling the LPM UC
*       tables and structures, and structures definitions for shadow management.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmRamUch
#define __prvCpssDxChLpmRamUch

#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChLpmRamUcEntryAdd
*
* DESCRIPTION:
*       creates a new or override an exist route entry in the routing
*       structures.
*
* INPUTS:
*       vrId              - The virtual private network identifier.
*       destPtr           - The destination address of this route.
*       prefix            - The number of bits that are actual valid in,
*                           the destPtr.
*       nextHopPointerPtr - The next hop pointer to be associated with the given
*                           (address,prefix).
*       insertMode        - how to insert the prefix
*                           (see PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT)
*       override          - whether this is an override or a new insert.
*       protocolStack     - type of protocol stack to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NOT_FOUND             - If the given nextHopId is not valid, or
*       GT_ALREADY_EXIST         - If overide is false and entry already exist, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntryAdd
(
    IN GT_U32                                       vrId,
    IN GT_U8                                        *destPtr,
    IN GT_U32                                       prefix,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *nextHopPointerPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT   insertMode,
    IN GT_BOOL                                      override,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamUcEntryDel
*
* DESCRIPTION:
*       Delete address/prefix unicast address from a practicular virtual router
*
* INPUTS:
*       vrId              - The virtual private network identifier.
*       destPtr           - The destination address of this route.
*       prefix            - The number of bits that are actual valid in destPtr
*       updateHwAndMem    - whether an HW update and memory alloc should take place
*       protocolStack     - type of protocol to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*       roolBack          - GT_TRUE: rollback is taking place.
*                           GT_FALSE: otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the required vrId is not valid, or
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntryDel
(
    IN GT_U32                               vrId,
    IN GT_U8                                *destPtr,
    IN GT_U32                               prefix,
    IN GT_BOOL                              updateHwAndMem,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *shadowPtr,
    IN GT_BOOL                              roolBack
);

/*******************************************************************************
* prvCpssDxChLpmRamUcEntriesFlush
*
* DESCRIPTION:
*       flushes the unicast Routing table and stays with the default entry
*       only.
*
* INPUTS:
*       vrId           - The virtual private network identifier.
*       protocolStack  - the protocol to work on.
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
GT_STATUS prvCpssDxChLpmRamUcEntriesFlush
(
    IN GT_U32                               vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocolStack,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC     *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamUcEntrySearch
*
* DESCRIPTION:
*       This function searches for a given uc address, and returns the next
*       hop pointer associated with it.
*
* INPUTS:
*       vrId              - The virtual router Id to get the entry from.
*       addrPtr           - The address to search for.
*       prefix            - Prefix length of the address.
*       protocolStack     - protocol stack to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       nextHopPointerPtr - The next hop pointer to which the given prefix is
*                           bound.
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntrySearch
(
    IN  GT_U32                                      vrId,
    IN  GT_U8                                       *addrPtr,
    IN  GT_U32                                      prefix,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopPointerPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT        protocolStack,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC            *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamUcEntryLpmSearch
*
* DESCRIPTION:
*       This function searches for a given uc address, and returns the next
*       hop pointer associated with it. This is a LPM search - the exact prefix
*       doesn't have to exist in DB.
*
* INPUTS:
*       vrId            - The virtual router Id to get the entry from.
*       addrPtr         - The address to search for.
*       protocolStack   - type of protocol stack to work on.
*       shadowPtr       - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       prefixLenPtr        - Points to the number of bits that are actual valid
*                             in the longest match
*       nextHopPointerPtr   - The next hop pointer to which the given prefix is
*                             bound.
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntryLpmSearch
(
    IN  GT_U32                                      vrId,
    IN  GT_U8                                       *addrPtr,
    OUT GT_U32                                      *prefixLenPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopPointerPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT        protocolStack,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC            *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamUcEntryGet
*
* DESCRIPTION:
*       This function returns an Unicast entry with larger (address,prefix) than
*       the given one.
*
* INPUTS:
*       vrId              - The virtual router Id to get the entry from.
*       addrPtr           - The address to start the search from.
*       prefixPtr         - Prefix length of addrPtr.
*       protocolStack     - type of protocol stack to work on.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       addrPtr                 - The address of the found entry.
*       prefixPtr               - The prefix length of the found entry.
*       nextHopPointerPtrPtr    - A pointer to the next hop pointer
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if no more entries are left in the LPM shadow
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUcEntryGet
(
    IN    GT_U32                                    vrId,
    INOUT GT_U8                                     *addrPtr,
    INOUT GT_U32                                    *prefixPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC **nextHopPointerPtrPtr,
    IN    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocolStack,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmRamUch */

