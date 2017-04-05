/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcam.h
*
* DESCRIPTION:
*       This file includes internal typedefs declarations for controlling the
*       IP tables and structures, and structures definitions for shadow
*       management.
*
* FILE REVISION NUMBER:
*       $Revision: 37 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmTcamh
#define __prvCpssDxChLpmTcamh

#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamCommonTypes.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManagerTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Pointers to dummy trie nodes to support policy based routing MC defaults */
extern GT_U32                              pbrDefaultMcTrieNodePtrArraySize;
extern PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC  **pbrDefaultMcTrieNodePtrArray;

/*******************************************************************************
* prvCpssDxChLpmTcamMoveToLocation
*
* DESCRIPTION:
*       Copy single entry from one TCAM location to another TCAM location
*       and to invalidate the entry in the original location
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       fromPtr           - points to the original hardware TCAM location
*       toPtr             - points to the new hardware TCAM location
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamMoveToLocation
(
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *fromPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *toPtr,
    IN  GT_VOID                                     *clientCookiePtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamMoveToAnywhere
*
* DESCRIPTION:
*       Move single entry from TCAM location and to invalidate the entry in
*       the original location.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamMoveToAnywhere
(
    IN  GT_VOID         *clientCookiePtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamCheckIfDefaultLocation
*
* DESCRIPTION:
*       Return if an entry from TCAM is a default entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       isDefaultEntryLocationPtr - (pointer to)
*                                GT_TRUE: clientCookiePtr is a default entry
*                                GT_FALSE:clientCookiePtr is not a default entry
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamCheckIfDefaultLocation
(
    IN  GT_VOID         *clientCookiePtr,
    OUT GT_BOOL         *isDefaultEntryLocationPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamCheckCapacityForShadow
*
* DESCRIPTION:
*       This function checks the capacity given in lpb db create or capacity update.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       shadowType              - the type of shadow we're building
*       indexesRangePtr         - the range of TCAM indexes availble for this
*                                 LPM DB (see explanation in
*                                 PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC)
*       partitionEnable         - whether to enable partitioning of the TCAM
*                                 according to the tcamLpmManagerCapcaityCfgPtr
*       capacityCfgPtr          - The capcity configuration struct.
*
* OUTPUTS:
*       totalLinesPtr           - total lines needed
*       totalSingleEntriesPtr   - total single entries needed
*       totalQuadEntriesPtr     - total quad entries needed
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM on bad capacityCfg.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamCheckCapacityForShadow
(
    IN PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT                shadowType,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    IN GT_BOOL                                          partitionEnable,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC   *capacityCfgPtr,
    OUT GT_U32                                          *totalLinesPtr,
    OUT GT_U32                                          *totalSingleEntriesPtr,
    OUT GT_U32                                          *totalQuadEntriesPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamPrepareReservation
*
* DESCRIPTION:
*       This function returns a pointer to a new IP shadow struct.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       shadowType                  - the type of shadow we're building (cheetah+ or cheetah2)
*       partitionEnable             - whether to enable partitioning of the TCAM
*                                     according to tcamLpmManagerCapacityCfgPtr
*       capacityCfgPtr              - The capcity configuration struct.
*       totalPlacesNeeded           - total places needed
*       totalCapacity               - total number of lines possible
*       sizeOfArrayPtr              - points to allocated size of the
*                                     requestedEntriesArrayPtr array
*
* OUTPUTS:
*       reservationTypePtr          - type of reservation needed
*       requestedEntriesArrayPtr    - array of amounts needed per entry type
*       sizeOfArrayPtr              - number of elements filled in the
*                                     requestedEntriesArrayPtr array
*       numberOfUnusedTcamEntriesPtr  - points to unused Tcam entires
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM on bad capacityCfg.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPrepareReservation
(
    IN    PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT                   shadowType,
    IN    GT_BOOL                                             partitionEnable,
    IN    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC       *capacityCfgPtr,
    IN    GT_U32                                              totalPlacesNeeded,
    IN    GT_U32                                              totalCapacity,
    OUT   CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT *reservationTypePtr,
    OUT   CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             *requestedEntriesArrayPtr,
    INOUT GT_U32                                              *sizeOfArrayPtr,
    OUT   GT_U32                                              *numberOfUnusedTcamEntriesPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamFreeSpaceCountersCheck
*
* DESCRIPTION:
*       This function check whether TCAM has enough free space for
*       added IPv4/Ipv6 prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       numberOfPrefixes      - number of added prefixes
*       ipMode                - ip uc/mc selection
*       mcEntryType           - mc entry type: relevant only if ipMode is mc.
*       protocolStack         - the protocol stack to support
*       ipTcamShadowPtr       - ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success.
*       GT_BAD_PARAM          - on wrong number.
*       GT_FULL               - if TCAM is full.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamFreeSpaceCountersCheck
(
    IN GT_U32                                          numberOfPrefixes,
    IN CPSS_IP_UNICAST_MULTICAST_ENT                   ipMode,
    IN PRV_CPSS_DXCH_LPM_TCAM_IP_MC_ENTRY_TYPE_ENT     mcEntryType,
    IN CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC               *ipTcamShadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - the device number.
*       lttTtiRow     - the entry's row index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttTtiColumn  - the entry's column index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttEntryPtr   - the lookup translation table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active or
*                         invalid ipv6MCGroupScopeLevel value.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Converts an entry of type PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC to
*       CPSS_DXCH_IP_LTT_ENTRY_STC and calls cpssDxChIpLttWrite.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamLttWrite
(
    IN  GT_U8                                          devNum,
    IN  GT_U32                                         lttTtiRow,
    IN  GT_U32                                         lttTtiColumn,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC      *routeEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamLttRead
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number.
*       lttTtiRow       - the entry's row index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*       lttTtiColumn    - the entry's column index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Calls cpssDxChIpLttRead and converts an entry of type
*       PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC to CPSS_DXCH_IP_LTT_ENTRY_STC.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamLttRead
(
    IN  GT_U8                                          devNum,
    IN  GT_U32                                         lttTtiRow,
    IN  GT_U32                                         lttTtiColumn,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC      *routeEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamInit
*
* DESCRIPTION:
*       Init the Ip unit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       shadowType              - the type of shadow we're building (cheetah+ or
*                                 cheetah2)
*       rangeIndexsPtr          - the range of TCAM indexes availble for this
*                                 LPM DB (see explanation in
*                                 PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC)
*       partitionEnable         - whether to enable partitioning of the TCAM
*                                 according to the tcamLpmManagercapacityCfgPtr
*       capacityCfgPtr          - The capcity configuration struct.
*       protocolStack           - the protocol stack to support
*       tcamManagerHandlerPtr   - the TCAM manager handler
*
* OUTPUTS:
*       ipTcamShadowPtrPtr - the ip shadow we created.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamInit
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT                shadowType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *rangeIndexsPtr,
    IN  GT_BOOL                                          partitionEnable,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC    *capacityCfgPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    IN  GT_VOID                                          *tcamManagerHandlerPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                **ipTcamShadowPtrPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDeleteShadowStruct
*
* DESCRIPTION:
*       This function deletes IP TCAM shadow struct.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr       - A pointer to the ip shadow struct.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDeleteShadowStruct
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC   *ipTcamShadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamCheetah2VrSupportInit
*
* DESCRIPTION:
*       Init the Ip unit for Cheetah2 devices with virtual router support.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - the pcl tcam configuration
*       routerTcamCfgPtr - the router tcam configuration
*
* OUTPUTS:
*       ipTcamShadowPtrPtr  - the ip shadow we created.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamCheetah2VrSupportInit
(
    IN  CPSS_IP_PROTOCOL_STACK_ENT                              protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                       **ipTcamShadowPtrPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamAddVirtualRouter
*
* DESCRIPTION:
*       This function adds a new virtual router to the valid virtual routers
*       in system for specific shared shadow devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                        - The virtual's router ID (in cheetah+ this
*                                     will be the pcl id).
*       defIpv4UcNextHopInfoPtr     - The next hop info representing the default IPv4-Uc
*                                     route entry.
*       defIpv6UcNextHopInfoPtr     - The next hop info representing the default IPv6-Uc
*                                     route entry.

*       defUcNextHopRuleIdxArr      - if not NULL, then this is the internal rule idx
*                                     of the defualt NextHop prefix. (array of 2
*                                     for each of the protocol Stacks)
*       defIpv4McRouteLttEntryPtr   - The default IPv4_Mc LTT entry
*       defIpv6McRouteLttEntryPtr   - The default IPv6_Mc LTT entry
*       defMcRouteRuleIdxArr        - if not null then this is the internal Rule idx
*                                     of the default mc route (prefix) (array of 2
*                                     for each of the protocol Stacks)
*       protocolStack               - types of IP stack used in this virtual router.
*       ipTcamShadowPtr             - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the given next hop Id is not valid.
*       GT_ALREADY_EXIST         - if the virtual router already exists
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamAddVirtualRouter
(
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       *defIpv4UcNextHopInfoPtr,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       *defIpv6UcNextHopInfoPtr,
    IN GT_U32                                       defUcNextHopRuleIdxArr[],
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defIpv4McRouteLttEntryPtr,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defIpv6McRouteLttEntryPtr,
    IN GT_U32                                       defMcRouteRuleIdxArr[],
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC            *ipTcamShadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamGetVirtualRouter
*
* DESCRIPTION:
*       This function gets the virtual router configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                    - The virtual's router ID (in cheetah+ this
*                                 will be the pcl id).
*       ipTcamShadowPtr         - the ip shadow we are working on.
*
* OUTPUTS:
*       supportIpv4UcPtr           - Whether the virtual router supports IPv4 UC
*       defIpv4UcNextHopInfoPtr    - The next hop info representing the default Ipv4 Uc
*                                    route entry.
*                                    Relevant only when <supportIpv4UcPtr> == GT_TRUE
*       supportIpv6UcPtr           - Whether the virtual router supports IPv6 UC
*       defIpv6UcNextHopInfoPtr    - The next hop info representing the default Ipv6 Uc
*                                    route entry.
*                                    Relevant only when <supportIpv6UcPtr> == GT_TRUE

*       defUcNextHopRuleIdxArr     - if not NULL, then this is the internal rule idx
*                                    of the defualt NextHop prefix. (array of 2
*                                    for each of the protocol Stacks)
*                                    Relevant only when <supportIpv4UcPtr> == GT_TRUE
*                                    or <supportIpv6UcPtr> == GT_TRUE
*       supportIpv4McPtr           - Whether the virtual router supports Ipv4 MC
*       defIpv4McRouteLttEntryPtr  - The default Ipv4 MC LTT entry
*                                    Relevant only when <supportIpv4McPtr> == GT_TRUE
*       supportIpv6McPtr           - Whether the virtual router supports Ipv6 MC
*       defIpv6McRouteLttEntryPtr  - The default Ipv6 MC LTT entry
*                                    Relevant only when <supportIpv6McPtr> == GT_TRUE
*       defMcRouteRuleIdxArr       - if not null then this is the internal Rule idx
*                                    of the default mc route (prefix) (array of 2
*                                    for each of the protocol Stacks)
*                                    Relevant only when <supportIpv4McPtr> == GT_TRUE
*                                    or <supportIpv6McPtr> == GT_TRUE
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the given next hop Id is not valid.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetVirtualRouter
(
    IN  GT_U32                                       vrId,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC            *ipTcamShadowPtr,
    OUT GT_BOOL                                      *supportIpv4UcPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       *defIpv4UcNextHopInfoPtr,
    OUT GT_BOOL                                      *supportIpv6UcPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT       *defIpv6UcNextHopInfoPtr,
    OUT GT_U32                                       defUcNextHopRuleIdxArr[],
    OUT GT_BOOL                                      *supportIpv4McPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defIpv4McRouteLttEntryPtr,
    OUT GT_BOOL                                      *supportIpv6McPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defIpv6McRouteLttEntryPtr,
    OUT GT_U32                                       defMcRouteRuleIdxArr[]
);

/*******************************************************************************
* prvCpssDxChLpmTcamDeleteVirtualRouter
*
* DESCRIPTION:
*       This function deletes an existing virtual router from the vr table, The
*       IP tables (Uc & Mc) are flushed as a result of deletion.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId          - The virtual's router ID to be deleted.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if the given vrId does not exist.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDeleteVirtualRouter
(
    IN GT_U32                              vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC   *ipTcamShadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamAddDevToShadow
*
* DESCRIPTION:
*       add the device to the ip shadow
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devListPtr  - The device list to add.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamAddDevToShadow
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC *devListPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC      *ipTcamShadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamRemoveDevsFromShadow
*
* DESCRIPTION:
*       removes the devices from belonging to the shadow.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devListPtr  - The device list to remove
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamRemoveDevsFromShadow
(
    IN PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC *devListPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC      *ipTcamShadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamGetDevsFromShadow
*
* DESCRIPTION:
*       Get the devices from belonging to the shadow.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr   - the ip shadow we are working on.
*       numOfDevsPtr      - points to the size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr      - points to the number of devices retrieved
*       devListArray      - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_SIZE              - in case not enough memory was allocated to device list
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The application should allocate memory for the device list array.
*       If the function returns GT_OK, then number of devices holds
*       the number of devices the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the device list is not enough. In this case the
*       number of devices will hold the size of array needed.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetDevsFromShadow
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    OUT   GT_U8                                      devListArray[],
    INOUT GT_U32                                     *numOfDevsPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamTrieSetPrefixIpCheetah2
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId            - The virtual's router ID.
*       ipAddrPtr       - IP Uc address
*       prefixLen       - Ip Uc prefix length
*       ecmpRouteNum    - the ecmp route number to write
*       isMcSrc         - if this entry is MC source
*       gIndex          - the G-index to write (relevant if isMcSrc == GT_TRUE)
*       tcamRulePtr     - the tcam rule to set it in
*       ipTcamShadowPtr - ip shadow we are working on.
*       pData           - the data to attach to this prefix.
*       protocolStack   - the type of the prefix.
*       devListPtr      - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to add a default IP group, use prefixLen = 0.
*       2.  Bit 15 in the vrId indicates whether this is shared virtual router
*           or not (if the bit is 1 this is shared virtual router).
*           For shared virtual router:
*             - <match all> entries (meaning perfix len is 0) are not written to
*               the hardware
*             - all other prefixes are written with <don't care> masking for
*               the virtual router field.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamTrieSetPrefixIpCheetah2
(
    IN  GT_U32                                    vrId,
    IN  GT_U8                                     *ipAddrPtr,
    IN  GT_U32                                    prefixLen,
    IN  GT_U8                                     ecmpRouteNum,
    IN  GT_BOOL                                   isMcSrc,
    IN  GT_U16                                    gIndex,
    IN  GT_PTR                                    tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *ipTcamShadowPtr,
    IN  GT_PTR                                    pData,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC    *devListPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamTrieDelPrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       tcamRulePtr         - the tcam rule to delete it from.
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data entry that was attached to this prefix.
*       protocolStack       - the protocol stack of the prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamTrieDelPrefixCheetah2
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr,
    IN  GT_PTR                                        pData,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                    protocolStack,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamTrieDelIpv4PrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       tcamRulePtr         - the tcam rule to delete it from.
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data entry that was attached to this prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamTrieDelIpv4PrefixCheetah2
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr,
    IN  GT_PTR                                        pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamTrieDelIpv6PrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       tcamRulePtr         - the tcam rule to delete it from.
*       ipTcamShadowPtr     - ip shadow we are working on.
*       pData               - the data entry that was attached to this prefix.
*       devListPtr          - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamTrieDelIpv6PrefixCheetah2
(
    IN  GT_U32                                        vrId,
    IN  GT_PTR                                        tcamRulePtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC             *ipTcamShadowPtr,
    IN  GT_PTR                                        pData,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC        *devListPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamUpdateHw
*
* DESCRIPTION:
*       Perform the actual HW update at the end of a hot sync operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr - the shadow to write the HW.
*       updateMode      - the update mode.
*       protocolStack   - which protocol stack to update.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamUpdateHw
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *ipTcamShadowPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_UPDATE_TYPE_ENT    updateMode,
    IN CPSS_IP_PROTOCOL_STACK_ENT                protocolStack
);

/*******************************************************************************
* prvCpssDxChLpmTcamPbrDefaultMcRuleSet
*
* DESCRIPTION:
*       Sets "do-nothing" PCL rule to capture all MC traffic for specific
*       protocol stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr   - the ip shadow we are working on
*       devListPtr        - list of devices to set the rule at
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - Illegal parameter in function called
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPbrDefaultMcRuleSet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *ipTcamShadowPtr,
    IN  PRV_CPSS_DXCH_LPM_SHADOW_DEVS_LIST_STC  *devListPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamGetTcamOffset
*
* DESCRIPTION:
*       Get  IP TCAM hardware offset
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr     - ip shadow we are working on.
*       tcamRulePtr         - pointer to tcam rule
*       isToken             - whether the tcamRulePtr represent TCAM location
*                             or token (relevant only in TCAM manager mode)
*
* OUTPUTS:
*       offsetPtr           - pointer to TCAM rule hardware offset.
*
* RETURNS:
*      GT_OK        - on OK
*      GT_FAIL      - on FAIL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetTcamOffset
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    IN  GT_PTR                                    tcamRulePtr,
    IN  GT_BOOL                                   isToken,
    OUT GT_U32                                    *offsetPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamGetTcamRowColumn
*
* DESCRIPTION:
*       Get  IP TCAM hardware location :row and column
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipTcamShadowPtr     - ip shadow we are working on.
*       tcamRulePtr         - pointer to tcam rule
*
* OUTPUTS:
*       tcamRowPtr          - pointer to TCAM row.
*       tcamColumnPtr       - pointer to TCAM column.
*
* RETURNS:
*      GT_OK        - on OK
*      GT_FAIL      - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetTcamRowColumn
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *ipTcamShadowPtr,
    IN  GT_PTR                                    tcamRulePtr,
    OUT GT_U32                                    *tcamRowPtr,
    OUT GT_U32                                    *tcamColumnPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbCreate
*
* DESCRIPTION:
*   This function creates an LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtrPtr           - the LPM DB information
*       shadowType            - the type of shadow to maintain
*       protocolStack         - the type of protocol stack this LPM DB support
*       memoryCfgPtr          - the memory configuration of this LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_SUPPORTED         - request is not supported if partitioning
*                                  is disabled.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbCreate
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     **lpmDbPtrPtr,
    IN PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT     shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT            protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_CONFIG_STC     *memoryCfgPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbDelete
*
* DESCRIPTION:
*   This function deletes LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr              - the LPM DB information
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - the LPM DB is not empty
*       GT_FAIL                   - on error
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       Deleting LPM DB is allowed only if no VR present in the LPM DB (since
*       prefixes reside within VR it means that no prefixes present as well).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbDelete
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbConfigGet
*
* DESCRIPTION:
*   This function retrieves configuration of the LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr              - the LPM DB information
*
* OUTPUTS:
*       protocolStack         - the type of protocol stack this LPM DB support
*       memoryCfgPtr          - the memory configuration of this LPM DB
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbConfigGet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *lpmDbPtr,
    OUT CPSS_IP_PROTOCOL_STACK_ENT           *protocolStackPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_CONFIG_STC    *memoryCfgPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbCapacityUpdate
*
* DESCRIPTION:
*   This function updates the initial LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr                    - the LPM DB information
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       tcamLpmManagerCapcityCfgPtr - the new capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means new prefixes
*                                     partition, when this was set to GT_FALSE
*                                     this means the new prefixes guaranteed
*                                     allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*       GT_NOT_FOUND                - if the LPM DB id is not found
*       GT_NO_RESOURCE              - failed to allocate resources needed to the
*                                     new configuration
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbCapacityUpdate
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *lpmDbPtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    IN PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC    *tcamLpmManagerCapcityCfgPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbCapacityGet
*
* DESCRIPTION:
*   This function gets the current LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr                    - the LPM DB information
*
* OUTPUTS:
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       partitionEnablePtr          - GT_TRUE:  the TCAM is partitioned according
*                                     to the capacityCfgPtr, any unused TCAM entries
*                                     were allocated to IPv4 UC entries.
*                                     GT_FALSE: TCAM entries are allocated on demand
*                                     while entries are guaranteed as specified
*                                     in capacityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - the current capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means current
*                                     prefixes partition, when this was set to
*                                     GT_FALSE this means the current guaranteed
*                                     prefixes allocation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NO_RESOURCE           - failed to allocate resources needed to the
*                                  new configuration.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbCapacityGet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                *lpmDbPtr,
    OUT GT_BOOL                                          *partitionEnablePtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC    *tcamLpmManagerCapcityCfgPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbCheetah2VrSupportCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment in Cheetah2
*   devices with support for virtual router.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtrPtr      - the LPM DB information
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - the pcl tcam configuration
*       routerTcamCfgPtr - the router tcam configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - NULL pointer.
*
* COMMENTS:
*       Since LTT rules are shared between PCL and Router Tcam, several checks
*       are made to supplied parameters to make sure that all prefixes could be
*       inserted.
*
*       Note that LTT entries corresponding to routerTcamCfgPtr->tcamRange
*       are allocated to Lpm Manager.
*
*       Furthermore:
*
*       routerTcamCfgPtr->tcamRange.lastIndex -
*       routerTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Router Tcam lines).
*
*       pclTcamCfgPtr->tcamRange.lastIndex -
*       pclTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Pcl Tcam lines).
*
*       (routerTcamCfgPtr->tcamRange.lastIndex -
*        routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5
*           must be larger or equal to
*       pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes
*
*       Multicast is not supported.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbCheetah2VrSupportCreate
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                        **lpmDbPtrPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB. this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr       - the LPM DB information
*       devListArr     - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbDevListAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U8                                 devListArray[],
    IN GT_U32                                numOfDevs
);

/*******************************************************************************
* prvCpssDxChLpmTcamDbDevListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing LPM DB. this remove will
*   invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr       - the LPM DB information
*       devListArr     - the array of device ids to remove from the
*                        LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbDevListRemove
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U8                                 devListArray[],
    IN GT_U32                                numOfDevs
);

/*******************************************************************************
* prvCpssDxChLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr                  - the LPM DB information
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamLinesReserve
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC               *lpmDbPtr,
    IN  GT_U32                                          linesToReserveArray[],
    IN  GT_U32                                          numberOfLinesToReserve,
    IN  PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC   allocateReserveLinesAs
);

/*******************************************************************************
* prvCpssDxChLpmTcamPolicyBasedRoutingDefaultMcSet
*
* DESCRIPTION:
*   The function uses available TCAM rules at the beginning of the TCAM range
*   allocated to the LPM DB and place there "do nothing" rules that will make
*   sure that Multicast traffic is matched by these rules and not by the
*   Unicast default rule.
*   When policy based routing Unicast traffic coexists with IP based Multicast
*   (S,G,V) bridging (used in IGMP and MLD protocols), there is a need to add
*   default Multicast rules in order to make sure that the Multicast traffic
*   will not be matched by the Unicast default rule.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       lpmDbPtr                  - the LPM DB information
*       pclIdArrayLen             - size of the PCL ID array (range 1..1048575)
*       pclIdArray                - array of PCL ID that may be in used by the LPM DB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_BAD_PARAM              - if wrong value in any of the parameters
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_ALREADY_EXIST          - if default MC already set
*       GT_NOT_SUPPORTED          - if the LPM DB doesn't configured to
*                                   operate in policy based routing mode
*       GT_FULL                   - if TCAM is full
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - the LPM DB must be configured to use policy based routing
*       One default Multicast rule will be set for each protocol stack supported
*       by the LPM DB and for each PCL ID that may be used in the LPM DB (each
*       virtaul router uses unique PCL ID).
*       For each protocol stack, the Multicast default rules will use the
*       prefixes allocated for Unicast prefixes for this protocol. For example
*       if the application allocates 10 IPv4 Unicast prefixes, then after setting
*       one default Multicast prefix, only 9 IPv4 Unicast prefixes will be
*       available.
*       The default Multicast rules will capture all IPv4/IPv6 Multicast
*       traffic with the same assigned PCL ID. As a result all rules configured
*       to match IP Multicast traffic with same assigned PCL ID that reside
*       after the Multicast defaults will not be matched.
*       The default Multicast rules can not be deleted after set.
*       The default Multicast rules can be set no more than once.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPolicyBasedRoutingDefaultMcSet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC               *lpmDbPtr,
    IN  GT_U32                                          pclIdArrayLen,
    IN  GT_U32                                          pclIdArray[]
);

/*******************************************************************************
* prvCpssDxChLpmTcamPolicyBasedRoutingDefaultMcGet
*
* DESCRIPTION:
*   The function gets whether the LPM DB is configured to support default MC
*   rules in policy based routing mode and the rule indexes and PCL ID of those
*   default rules.
*   Refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet for more details.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       lpmDbPtr                          - the LPM DB information
*       defaultIpv4RuleIndexArrayLenPtr   - points to the allocated size of the array
*       defaultIpv6RuleIndexArrayLenPtr   - points to the allocated size of the array
*       pclIdArrayLenPtr                  - points to the allocated size of the array
*
* OUTPUTS:
*       defaultMcUsedPtr                  - whether default MC is used for this LPM DB
*       protocolStackPtr                  - protocol stack supported by this LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArray         - rule indexes of the default IPv4 MC (in case
*                                           the LPM DB support IPv4)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArrayLenPtr   - points to number of elements filled in the array
*       defaultIpv6RuleIndexArray         - rule indexes of the default IPv6 MC (in case
*                                           the LPM DB support IPv6)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv6RuleIndexArrayLenPtr   - points to number of elements filled in the array
*       pclIdArray                        - array of PCL ID that may be in used by the LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       pclIdArrayLenPtr                  - points to number of elements filled in the array
*
* RETURNS:
*       GT_OK                             - on success
*       GT_NOT_FOUND                      - if the LPM DB id is not found
*       GT_BAD_PTR                        - if one of the parameters is NULL pointer.
*       GT_FULL                           - if any of the arrays is not big enough
*       GT_FAIL                           - on failure
*       GT_NOT_APPLICABLE_DEVICE          - on not applicable device
*
* COMMENTS:
*       The rule indexes are according to explanations in cpssDxChPclRuleSet.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamPolicyBasedRoutingDefaultMcGet
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC  *lpmDbPtr,
    OUT   GT_BOOL                            *defaultMcUsedPtr,
    OUT   CPSS_IP_PROTOCOL_STACK_ENT         *protocolStackPtr,
    OUT   GT_U32                             defaultIpv4RuleIndexArray[],
    INOUT GT_U32                             *defaultIpv4RuleIndexArrayLenPtr,
    OUT   GT_U32                             defaultIpv6RuleIndexArray[],
    INOUT GT_U32                             *defaultIpv6RuleIndexArrayLenPtr,
    OUT   GT_U32                             pclIdArray[],
    INOUT GT_U32                             *pclIdArrayLenPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr            - the LPM DB information
*       vrId                - The virtual's router ID.
*       vrConfigPtr         - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamVirtualRouterAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC  *vrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamVirtualRouterSharedAdd
*
* DESCRIPTION:
*   This function adds a shared virtual router in system for specific LPM DB.
*   Prefixes that reside within shared virtual router will participate in the
*   lookups of all virtual routers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr            - the LPM DB information
*       vrId                - The virtual router ID.
*       vrConfigPtr         - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  Only one shared virtual router can exists at any time within a
*           given LPM DB.
*       2.  Virtual router ID that is used for non-shared virtual router can't
*           be used for the shared virtual router and via versa.
*       3.  Shared prefixes can't overlap non-shared prefixes. If the shared
*           virtual router supports shared prefixes type then adding non-shared
*           virtual router that supports the same prefixes type will fail.
*           Also, if a non-shared virtual router that supports prefixes type
*           exists, then adding a shared virtual router that supports the same
*           prefixes type will fail.
*       4.  When the shared virtual router supports IPv4 UC prefixes, then the
*           the following will apply:
*             - The <match all> default TCAM entry will not be written to TCAM
*               (however TCAM entry will still be allocated to this entry)
*             - The following prefixes will be added to match all non-MC traffic:
*               0x00/1, 0x80/2, 0xC0/3, 0xF0/4
*           Same applies when the shared virtual router supports IPv6 UC
*           prefixes. The prefixes added to match all non-MC traffic are:
*           0x00/1, 0x80/2, 0xC0/3, 0xE0/4, 0xF0/5, 0xF8/6, 0xFC/7, 0xFE/8
*       5.  The application should not delete the non-MC prefixes (when added).
*       6.  Manipulation of the default UC will not be applied on the non-MC
*           prefixes. The application should manipulate those non-MC entries
*           directly when needed.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamVirtualRouterSharedAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC  *vrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr            - the LPM DB information
*       vrId                - The virtual's router ID.
*
* OUTPUTS:
*       vrConfigPtr    - Virtual router configuration
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamVirtualRouterGet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *lpmDbPtr,
    IN  GT_U32                               vrId,
    OUT PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC *vrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamVirtualRouterDel
*
* DESCRIPTION:
*   This function removes a virtual router in system for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr            - the LPM DB information
*       vrId                - The virtual's router ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamVirtualRouterDel
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - the LPM DB information
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4UcPrefixAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *lpmDbPtr,
    IN GT_U32                                    vrId,
    IN GT_IPADDR                                 ipAddr,
    IN GT_U32                                    prefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    IN GT_BOOL                                   override
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4UcPrefixBulkAdd
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr           - the LPM DB information
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4UcPrefixBulkAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                        *lpmDbPtr,
    IN GT_U32                                                   ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC                      *ipv4PrefixArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - the LPM DB information
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE          - If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4UcPrefixDel
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId,
    IN GT_IPADDR                             ipAddr,
    IN GT_U32                                prefixLen
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4UcPrefixBulkDel
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr           - the LPM DB information
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of the prefixes' lengths is too big
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4UcPrefixBulkDel
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                        *lpmDbPtr,
    IN GT_U32                                                   ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC                      *ipv4PrefixArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr    - the LPM DB information
*       vrId        - The virtual router identifier.
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
GT_STATUS prvCpssDxChLpmTcamIpv4UcPrefixesFlush
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4UcPrefixGet
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - the LPM DB information
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - if found, this is the route entry info.
*                               accosiated with this UC prefix.
*       tcamRowIndexPtr      - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr   - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4UcPrefixGet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *lpmDbPtr,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr     - the LPM DB information
*       vrId         - The virtual router Id to get the entry from.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - The ip address of the found entry.
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - If found, this is the route entry info accosiated
*                              with this UC prefix.
*       tcamRowIndexPtr      - TCAM row index of this uc prefix.
*       tcamColumnIndexPtr   - TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4UcPrefixGetNext
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT   GT_U32                                    *tcamRowIndexPtr,
    OUT   GT_U32                                    *tcamColumnIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP multicast route for a particular/all source and a particular
*   group address. this is done for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr            - the LPM DB information
*       vrId                - The virtual private network identifier.
*       ipGroup             - The IP multicast group address.
*       ipGroupPrefixLen    - The number of bits that are actual valid in,
*                             the ipGroup.
*       ipSrc               - the root address for source base multi tree
*                             protocol.
*       ipSrcPrefixLen      - The number of bits that are actual valid in,
*                             the ipSrc.
*       mcRouteLttEntryPtr  - the LTT entry pointing to the MC route entry
*                             associated with this MC route.
*       override            - whether to override an mc Route pointer for the
*                             given prefix
*       defragmentationEnable - whether to enable performance costing
*                             de-fragmentation process in the case that there
*                             is no place to insert the prefix. To point of the
*                             process is just to make space for this prefix.
*                             relevant only if the LPM DB was created with
*                             partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - if one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4McEntryAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC            *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPADDR                                    ipGroup,
    IN GT_U32                                       ipGroupPrefixLen,
    IN GT_IPADDR                                    ipSrc,
    IN GT_U32                                       ipSrcPrefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *mcRouteEntryPtr,
    IN GT_BOOL                                      override,
    IN GT_BOOL                                      defragmentationEnable
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - the LPM DB information
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4McEntryDel
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipGroup,
    IN GT_U32                               ipGroupPrefixLen,
    IN GT_IPADDR                            ipSrc,
    IN GT_U32                               ipSrcPrefixLen
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr    - the LPM DB information
*       vrId        - The virtual router identifier.
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
GT_STATUS prvCpssDxChLpmTcamIpv4McEntriesFlush
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *lpmDbPtr,
    IN GT_U32                               vrId
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc,ipGroup TCAM indexes.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr            - the LPM DB information
*       vrId                - The virtual router Id.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - The next ip Group address.
*       ipGroupPrefixLenPtr        - the ipGroup prefix Length.
*       ipSrcPtr                   - The next ip Source address.
*       ipSrcPrefixLenPtr          - ipSrc prefix length.
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4McEntryGetNext
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *lpmDbPtr,
    IN    GT_U32                                     vrId,
    INOUT GT_IPADDR                                  *ipGroupPtr,
    INOUT GT_U32                                     *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                                  *ipSrcPtr,
    INOUT GT_U32                                     *ipSrcPrefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  *mcRouteLttEntryPtr,
    OUT   GT_U32                                     *tcamGroupRowIndexPtr,
    OUT   GT_U32                                     *tcamGroupColumnIndexPtr,
    OUT   GT_U32                                     *tcamSrcRowIndexPtr,
    OUT   GT_U32                                     *tcamSrcColumnIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv4McEntryGet
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr         - the LPM DB information
*       vrId             - The virtual router Id.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv4McEntryGet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC           *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPADDR                                   ipGroup,
    IN  GT_U32                                      ipGroupPrefixLen,
    IN  GT_IPADDR                                   ipSrc,
    IN  GT_U32                                      ipSrcPrefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *mcRouteLttEntryPtr,
    OUT GT_U32                                      *tcamGroupRowIndexPtr,
    OUT GT_U32                                      *tcamGroupColumnIndexPtr,
    OUT GT_U32                                      *tcamSrcRowIndexPtr,
    OUT GT_U32                                      *tcamSrcColumnIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - the LPM DB information
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override the existing entry if it already exists
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6UcPrefixAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC      *lpmDbPtr,
    IN GT_U32                                 vrId,
    IN GT_IPV6ADDR                            ipAddr,
    IN GT_U32                                 prefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    IN GT_BOOL                                override,
    IN GT_BOOL                                defragmentationEnable
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6UcPrefixBulkAdd
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr           - the LPM DB information
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6UcPrefixBulkAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                         *lpmDbPtr,
    IN GT_U32                                                    ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC                       *ipv6PrefixArrayPtr,
    IN GT_BOOL                                                   defragmentationEnable
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - The LPM DB information
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6UcPrefixDel
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6UcPrefixBulkDel
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr           - the LPM DB information
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big, or
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6UcPrefixBulkDel
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC                        *lpmDbPtr,
    IN GT_U32                                                   ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC                      *ipv6PrefixArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr    - the LPM DB information
*       vrId        - The virtual router identifier.
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
GT_STATUS prvCpssDxChLpmTcamIpv6UcPrefixesFlush
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *lpmDbPtr,
    IN GT_U32                               vrId
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6UcPrefixGet
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - the LPM DB information
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If  found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6UcPrefixGet
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC        *lpmDbPtr,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr      - the LPM DB information
*       vrId          - The virtual router Id to get the entry from.
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - If found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr - If found, TCAM column index of this uc prefix.
*
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6UcPrefixGetNext
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *lpmDbPtr,
    IN    GT_U32                                  vrId,
    INOUT GT_IPV6ADDR                             *ipAddrPtr,
    INOUT GT_U32                                  *prefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT   GT_U32                                  *tcamRowIndexPtr,
    OUT   GT_U32                                  *tcamColumnIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - the LPM DB information
*       vrId            - The virtual private network identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - the LTT entry pointing to the MC route entry
*                            associated with this MC route.
*       override        - weather to override the mcRoutePointerPtr for the
*                         given prefix
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6McEntryAdd
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC          *lpmDbPtr,
    IN GT_U32                                     vrId,
    IN GT_IPV6ADDR                                ipGroup,
    IN GT_U32                                     ipGroupPrefixLen,
    IN GT_IPV6ADDR                                ipSrc,
    IN GT_U32                                     ipSrcPrefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  *mcRouteEntryPtr,
    IN GT_BOOL                                    override,
    IN GT_BOOL                                    defragmentationEnable
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr        - the LPM DB information
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6McEntryDel
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipGroup,
    IN GT_U32                               ipGroupPrefixLen,
    IN GT_IPV6ADDR                          ipSrc,
    IN GT_U32                               ipSrcPrefixLen
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr    - the LPM DB information
*       vrId        - The virtual router identifier.
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
GT_STATUS prvCpssDxChLpmTcamIpv6McEntriesFlush
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC    *lpmDbPtr,
    IN GT_U32                               vrId
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc,ipGroup TCAM indexes.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr            - the LPM DB information
*       vrId                - The virtual router Id.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - ipGroup prefix length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr           - The next ip Group address.
*       ipGroupPrefixLenPtr  - ipGroup prefix length.
*       ipSrcPtr             - The next ip Source address.
*       ipSrcPrefixLenPtr    - ipSrc prefix length.
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.  The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*           values, it means that they exist in the IP-Mc Table, unless this is
*           the first call to this function, then the value of (ipGroupPtr,
*           ipSrcPtr) is (0,0).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6McEntryGetNext
(
    IN    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipGroupPtr,
    INOUT GT_U32                                    *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                               *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcRouteEntryPtr,
    OUT   GT_U32                                    *tcamGroupRowIndexPtr,
    OUT   GT_U32                                    *tcamSrcRowIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamIpv6McEntryGet
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB information
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamIpv6McEntryGet
(
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC         *lpmDbPtr,
    IN  GT_U32                                    vrId,
    IN  GT_IPV6ADDR                               ipGroup,
    IN  GT_U32                                    ipGroupPrefixLen,
    IN  GT_IPV6ADDR                               ipSrc,
    IN  GT_U32                                    ipSrcPrefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcRouteEntryPtr,
    OUT GT_U32                                    *tcamGroupRowIndexPtr,
    OUT GT_U32                                    *tcamSrcRowIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamRowsClear
*
* DESCRIPTION:
*       restore tcamRows to 'pre-init' state
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmTcamRowsClear
(
    GT_VOID
);

/*******************************************************************************
* prvCpssDxChLpmTcamConvertIpLttEntryToLpmRouteEntry
*
* DESCRIPTION:
*       Convert IP LTT entry structure to private LPM route entry structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ipLttEntryPtr   - the IP LTT entry
*
* OUTPUTS:
*       routeEntryPtr   - the LPM route entry
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmTcamConvertIpLttEntryToLpmRouteEntry
(
    IN  CPSS_DXCH_IP_LTT_ENTRY_STC                      *ipLttEntryPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *routeEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamConvertLpmRouteEntryToIpLttEntry
*
* DESCRIPTION:
*       Convert private LPM route entry structure to IP LTT entry structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       routeEntryPtr   - the LPM route entry
*
* OUTPUTS:
*       ipLttEntryPtr   - the IP LTT entry
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmTcamConvertLpmRouteEntryToIpLttEntry
(
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *routeEntryPtr,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC                      *ipLttEntryPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmTcamh */

