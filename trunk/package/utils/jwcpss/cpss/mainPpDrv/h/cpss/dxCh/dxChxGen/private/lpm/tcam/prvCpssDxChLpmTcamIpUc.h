/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamIpUc.h
*
* DESCRIPTION:
*       This file includes functions declarations for controlling the IP (v6/v4)
*       UC tables and structures, and structures definitions for shadow
*       management.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmTcamIpUch
#define __prvCpssDxChLpmTcamIpUch

#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamCommonTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChLpmTcamAddIpUcEntry
*
* DESCRIPTION:
*       creates a new or override an exist route entry in the IP routing
*       structures.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipDestPtr       - The destination IP address of this route.
*       ipPrefix        - The number of bits that are actual valid in,
*                         the ipDestPtr.
*       nextHopInfoPtr  - the next hop info pointer to associate with this prefix
*       ruleIdxPtr      - if not null then this is the internal rule index for this
*                         prefix (Hsu use)
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*       isBulkInsert    - is this insert is part of a big bulk insertion which
*                         means we do all the operation apart from writing to the HW
*       defragEnable    - whether to defrag entries in case there is no
*                         free place for this entry cause of entries
*                         fragmentation; relevant only if the TCAM entries
*                         handler was created with partitionEnable = GT_FALSE
*       protocolStack   - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NOT_FOUND             - If the given nextHopId is not valid, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamAddIpUcEntry
(
    IN GT_U32                                 vrId,
    IN GT_U8                                  *ipDestPtr,
    IN GT_U32                                 ipPrefix,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    IN GT_U32                                 *ruleIdxPtr,
    IN GT_BOOL                                override,
    IN GT_BOOL                                isBulkInsert,
    IN GT_BOOL                                defragEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT             protocolStack,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *ipTcamShadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamDeleteIpUcEntry
*
* DESCRIPTION:
*       Deletes on ip/prefix unicast address from a practicular virtual private
*       network table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId        - The virtual private network identifier.
*       ipDestPtr   - The destination IP address of this route.
*       ipPrefix    - The number of bits that are actual valid in,
*                     the ipDestPtr. (CIDR like)
*       protocolStack - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the required vrId is not valid, or
*       GT_FAIL                  - on error.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDeleteIpUcEntry
(
    IN GT_U32                             vrId,
    IN GT_U8                              *ipDestPtr,
    IN GT_U32                             ipPrefix,
    IN CPSS_IP_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC  *ipTcamShadowPtr
);


/*******************************************************************************
* prvCpssDxChLpmTcamFlushIpUcEntries
*
* DESCRIPTION:
*       flushes the unicast IP Routing table and stays with the default entry
*       only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId        - The virtual private network identifier.
*       protocolStack - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*       flushDefault - whether to flush the defualt
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamFlushIpUcEntries
(
    IN GT_U32                             vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT         protocolStack,
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC  *ipTcamShadowPtr,
    IN GT_BOOL                            flushDefault
);

/*******************************************************************************
* prvCpssDxChLpmTcamSearchIpUcEntry
*
* DESCRIPTION:
*       This function searches for a given ip-uc address, and returns the next
*       hop/s associated with it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId        - The virtual router Id to get the entry from.
*       ipAddrPtr   - The ip address to search for.
*       prefix      - Prefix length of ipAddrPtr.
*       protocolStack - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       nextHopInfoPtr      - The next hop info to which the given prefix is bound.
*       internalRuleIdxPtr  - if not null then it is filled with internal mangment
*                             pool rule index (Hsu use)
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamSearchIpUcEntry
(
    IN  GT_U32                                      vrId,
    IN  GT_U8                                       *ipAddrPtr,
    IN  GT_U32                                      prefix,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT      *nextHopInfoPtr,
    OUT GT_U32                                      *internalRuleIdxPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                  protocolStack,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC           *ipTcamShadowPtr
);



/*******************************************************************************
* prvCpssDxChLpmTcamUcRuleOffsetGet
*
* DESCRIPTION:
*       This function searches for a given ip-uc address, and returns the
*       Ip Tcam unicast rule (tcam row and tcam column) associated with it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId            - The virtual router Id to get the entry from.
*       ipAddrPtr       - The ip address to search for.
*       prefixLength    - Prefix length of ipAddrPtr.
*       protocolStack   - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       tcamRowIndexPtr    - pointer to TCAM row index.
*       tcamColumnIndexPtr - pointer to TCAM column index.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_BAD_VALUE             - on bad value
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS prvCpssDxChLpmTcamUcRuleOffsetGet
(
    IN  GT_U32                                      vrId,
    IN  GT_U8                                       *ipAddrPtr,
    IN  GT_U32                                      prefixLength,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                  protocolStack,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC           *ipTcamShadowPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr
);

/*******************************************************************************
* prvCpssDxChLpmTcamGetIpUcEntry
*
* DESCRIPTION:
*       This function returns an IP-Unicast entry with larger (ip,prefix) than
*       the given one.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId            - The virtual router Id to get the entry from.
*       ipAddrPtr       - The ip address to start the search from.
*       prefixPtr       - Prefix length of ipAddrPtr.
*       protocolStack   - types of IP we're dealing with.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       ipAddrPtr           - The ip address of the found entry.
*       prefixPtr           - The prefix length of the found entry.
*       nextHopInfoPtr      - The next hop info to which the found prefix is bound.
*       internalRuleIdxPtr  - if not null then it is filled with internal mangment
*                         pool rule index (Hsu use)
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamGetIpUcEntry
(
    IN GT_U32                                   vrId,
    INOUT GT_U8                                 *ipAddrPtr,
    INOUT GT_U32                                *prefixPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocolStack,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *internalRuleIdxPtr,
    IN  PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC       *ipTcamShadowPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmTcamIpUch */

