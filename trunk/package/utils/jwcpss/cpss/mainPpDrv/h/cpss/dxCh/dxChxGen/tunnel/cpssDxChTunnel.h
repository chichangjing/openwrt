/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTunnel.h
*
* DESCRIPTION:
*       CPSS tunnel declarations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 37 $
*
*******************************************************************************/

#ifndef __cpssDxChTunnelh
#define __cpssDxChTunnelh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnelTypes.h>

/*******************************************************************************
* cpssDxChTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*       tunnelType    - type of the tunnel; valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
);

/*******************************************************************************
* cpssDxChTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*
* OUTPUTS:
*       tunnelTypePtr - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
);

/*******************************************************************************
* cpssDxChTunnelStartPortGroupEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*       tunnelType    - type of the tunnel; valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
);

/*******************************************************************************
* cpssDxChTunnelStartPortGroupEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start / NAT table (for NAT capable devices)
*
* OUTPUTS:
*       tunnelTypePtr - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*                       CPSS_TUNNEL_MAC_IN_MAC_E (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*                       CPSS_TUNNEL_TRILL_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table (for NAT capable devices), tunnel start entries table
*       and router ARP addresses table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry(for NAT capable devices)
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
);

/*******************************************************************************
* cpssDxChIpv4TunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for ipv4 tunnel termination.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable ipv4 tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4TunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChIpv4TunnelTermPortGet
*
* DESCRIPTION:
*       Get ipv4 tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get ipv4 tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to ipv4 tunnel termination state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4TunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM port,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChMplsTunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for mpls tunnel termination.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable mpls tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMplsTunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChMplsTunnelTermPortGet
*
* DESCRIPTION:
*       Get mpls tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get mpls tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to mpls tunnel termination state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMplsTunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  port,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChTunnelTermEntrySet
*
* DESCRIPTION:
*       Set a tunnel termination entry.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*       tunnelType    - type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IP_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
);

/*******************************************************************************
* cpssDxChTunnelTermEntryGet
*
* DESCRIPTION:
*       Get tunnel termination entry from hardware at a given index.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       validPtr      - points to whether this tunnel termination entry is valid
*       tunnelTypePtr - points to the type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IP_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    OUT GT_BOOL                             *validPtr,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
);

/*******************************************************************************
* cpssDxChTunnelTermEntryInvalidate
*
* DESCRIPTION:
*       Invalidate a tunnel termination entry.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermEntryInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTunnelTermTcamIndex
);

/*******************************************************************************
* cpssDxChTunnelStartEgressFilteringSet
*
* DESCRIPTION:
*       Set globally whether to subject Tunnel Start packets to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgressFilteringSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChTunnelStartEgressFilteringGet
*
* DESCRIPTION:
*       Get if Tunnel Start packets are globally subject to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgressFilteringGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTaggingSet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTaggingSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTaggingGet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTaggingGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTagModeSet
*
* DESCRIPTION:
*       Set the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       tagMode     - tunnel start ethernet-over-x vlan tag mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTagModeSet

(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode
);

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTagModeGet
*
* DESCRIPTION:
*       Get the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       tagModePtr  - pointer to tunnel start ethernet-over-x vlan tag mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_STATE             - on bad state.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       Despite its name, this API is applicable to all Tunnel Starts, not just MPLS.
*
*******************************************************************************/
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTagModeGet

(
    IN  GT_U8                                                devNum,
    OUT CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT *tagModePtr
);

/*******************************************************************************
* cpssDxChTunnelStartPassengerVlanTranslationEnableSet
*
* DESCRIPTION:
*       Controls Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*       enable      - GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPassengerVlanTranslationEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChTunnelStartPassengerVlanTranslationEnableGet
*
* DESCRIPTION:
*       Gets the Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points enable state.
*                     GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPassengerVlanTranslationEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChTunnelTermExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelTermExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
);

/*******************************************************************************
* cpssDxChTunnelCtrlTtExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PTR  - on NULL pointer
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelCtrlTtExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
);

/*******************************************************************************
* cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
*
* DESCRIPTION:
*       This feature allows overriding the <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                port,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
*
* DESCRIPTION:
*       The function gets status of the feature which allows overriding the
*       <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable additional offset to tunnel total length
*                       GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                port,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
*
* DESCRIPTION:
*       This API sets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       additionToLength  - Ip tunnel total length offset (6 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range values
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              additionToLength
);

/*******************************************************************************
* cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
*
* DESCRIPTION:
*       This API gets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       additionToLengthPtr  - (pointer to) Ip tunnel total length offset (6 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
(
    IN  GT_U8                               devNum,
    OUT GT_U32                              *additionToLengthPtr
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelPushEnableSet
*
* DESCRIPTION:
*       Enable/Disable MPLS PW label push.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelPushEnableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelPushEnableGet
*
* DESCRIPTION:
*       Returns if MPLS PW Label Push is enabled or disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelPushEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelSet
*
* DESCRIPTION:
*       Set the MPLS PW label value to push incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       label       - MPLS label (20 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or label.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               label
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelGet
*
* DESCRIPTION:
*       Returns the MPLS PW label value to push incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       labelPtr    - (pointer to) MPLS label
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *labelPtr
);

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelEnableSet
*
* DESCRIPTION:
*       Enable/Disable MPLS flow label per port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsFlowLabelEnableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelEnableGet
*
* DESCRIPTION:
*       Returns if MPLS flow Label is enabled or disabled per port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsFlowLabelEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelTtlSet
*
* DESCRIPTION:
*       Set the MPLS flow label TTL
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       ttl         - the TTL value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsFlowLabelTtlSet
(
    IN GT_U8                devNum,
    IN GT_U8                ttl
);

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelTtlGet
*
* DESCRIPTION:
*       Get the MPLS flow label TTL
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       ttlPtr      - (pointer to) the TTL value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsFlowLabelTtlGet
(
    IN  GT_U8           devNum,
    OUT GT_U8           *ttlPtr
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelExpSet
*
* DESCRIPTION:
*       Set the EXP value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       exp         - EXP value (APPLICABLE RANGES: 0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or exp.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelExpSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               exp
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelExpGet
*
* DESCRIPTION:
*       Returns the EXP value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       expPtr      - (pointer to) EXP value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelExpGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *expPtr
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelTtlSet
*
* DESCRIPTION:
*       Set the TTL value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ttl         - TTL value (APPLICABLE RANGES: 0...255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or ttl.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelTtlSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               ttl
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelTtlGet
*
* DESCRIPTION:
*       Returns the TTL value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       ttlPtr      - (pointer to) TTL value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwLabelTtlGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *ttlPtr
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwControlWordSet
*
* DESCRIPTION:
*       Sets a Pseudo Wire control word
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index
*                     Bobcat2; Caelum; Bobcat3: (APPLICABLE RANGES: 0..14)
*                     xCat, xCat3: (APPLICABLE RANGES: 0..6)
*       value       - the PW word to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwControlWordSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          value
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwControlWordGet
*
* DESCRIPTION:
*       Gets a Pseudo Wire control word
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index
*                     Bobcat2; Caelum; Bobcat3: (APPLICABLE RANGES: 0..14)
*                     xCat; xCat3: (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       valuePtr    - (pointer to) the read PW word
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwControlWordGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *valuePtr
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwETreeEnableSet
*
* DESCRIPTION:
*       Enable/Disable E-TREE assignment for a specified PW Control Word.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*       enable      - enable/disable the E-TREE assignment
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwETreeEnableSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_BOOL         enable
);

/*******************************************************************************
* cpssDxChTunnelStartMplsPwETreeEnableGet
*
* DESCRIPTION:
*       Return whether E-TREE assignment is enabled for a specified PW Control
*       Word.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*
* OUTPUTS:
*       enablePtr   - (pointer to) the E-TREE assignment enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartMplsPwETreeEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChTunnelStartHeaderTpidSelectSet
*
* DESCRIPTION:
*       Function sets index of TPID tunnel-start header
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       tpidEntryIndex - TPID table entry index (APPLICABLE RANGES: 0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when a TS entry is processed (from ingress
*       pipe assignment or from egress ePort assignment).
*       Applicable to all TS types
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartHeaderTpidSelectSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  GT_U32               tpidEntryIndex
);

/*******************************************************************************
* cpssDxChTunnelStartHeaderTpidSelectGet
*
* DESCRIPTION:
*       Function gets index of TPID tunnel-start header
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when a TS entry is processed (from ingress
*       pipe assignment or from egress ePort assignment).
*       Applicable to all TS types
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartHeaderTpidSelectGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    OUT GT_U32               *tpidEntryIndexPtr
);



/*******************************************************************************
* cpssDxChTunnelStartGenProfileTableEntrySet
*
* DESCRIPTION:
*   This API configures Generic Tunnel-start Profile table entry.
*   It serves as logical extension to the Generic IPv4/6 Tunnel-Start
*   when the IPv4/6 tunnel header includes either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartGenProfileTableEntrySet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           profileIndex,
    IN CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC        *profileDataPtr
);

/*******************************************************************************
* cpssDxChTunnelStartGenProfileTableEntryGet
*
* DESCRIPTION:
*   This API retrieves  Generic Tunnel-start Profile table entry content.
*   It serves as logical extension to the Generic IPv4/6 Tunnel-Start
*   when the IPv4/6 tunnel header includes either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                         (APPLICABLE RANGES: 0...7).
*
* OUTPUTS:
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartGenProfileTableEntryGet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           profileIndex,
    OUT CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC       *profileDataPtr
);




/*******************************************************************************
* cpssDxChTunnelStartPortGroupGenProfileTableEntrySet
*
* DESCRIPTION:
*   This API configures Generic Tunnel-start Profile table entry.
*   Generic Tunnel-start Profile table entry serves as logical extension to
*   the Generic IPv4/6 Tunnel-Start when the IPv4/6 tunnel header includes
*   either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
*
*       portGroupsBmp    - bitmap of Port Groups.
*                          NOTEs:
*                          1. for non multi-port groups device this parameter is IGNORED.
*                          2. for multi-port groups device :
*                             bitmap must be set with at least one bit representing
*                             valid port group(s). If a bit of non valid port group
*                             is set then function returns GT_BAD_PARAM.
*                             value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupGenProfileTableEntrySet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN GT_U32                                           profileIndex,
    IN CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC        *profileDataPtr
);

/*******************************************************************************
* cpssDxChTunnelStartPortGroupGenProfileTableEntryGet
*
* DESCRIPTION:
*   This API retrieves  Generic Tunnel-start Profile table entry content.
*   Generic Tunnel-start Profile table entry serves as logical extension to
*   the Generic IPv4/6 Tunnel-Start when the IPv4/6 tunnel header includes
*   either a GRE or UDP header.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number.
*       portGroupsBmp    - bitmap of Port Groups.
*                          NOTEs:
*                          1. for non multi-port groups device this parameter is IGNORED.
*                          2. for multi-port groups device :
*                             bitmap must be set with at least one bit representing
*                             valid port group(s). If a bit of non valid port group
*                             is set then function returns GT_BAD_PARAM.
*                             value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       profileIndex     - entry index in Generic Tunnel-start Profile table.
*                          (APPLICABLE RANGES: 0...7).
* OUTPUTS:
*       profileDataPtr   - pointer to Generic Tunnel-start Profile data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The Profile table entry template generates up to 16 bytes of data
*      after the 4B GRE header or after the 8B UDP header.
*      There are 8 profiles available to use. But if application is
*      going to use legacy ip tunnel types, cpss is reserved profile index 7 for legacy support
*      (empty profile: no extension data is added in tunnel header). Also if usage of new ipv4/6
*      tunnel types take place , and these tunnels don't require profile data generation - empty
*      profile should be reserved. In this case, in order to save empty profiles number, it is
*      recommended to utilize profile 7 as well.
*      Empty profile should be configured to prevent insertion of template data into tunnel header:
*      templateDataSize = CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_NONE_E.
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartPortGroupGenProfileTableEntryGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN GT_U32                                           profileIndex,
    OUT CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC       *profileDataPtr
);

/*******************************************************************************
* cpssDxChTunnelStartEntryExtensionSet
*
* DESCRIPTION:
*       Set tunnel start entry extension value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tsExtension - tunnel start entry extension value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntryExtensionSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               tsExtension
);

/*******************************************************************************
* cpssDxChTunnelStartEntryExtensionGet
*
* DESCRIPTION:
*       Get tunnel start entry extension value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       tsExtensionPtr  - (pointer to) tunnel start entry extension value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEntryExtensionGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *tsExtensionPtr
);

/*******************************************************************************
* cpssDxChTunnelStartEgessVlanTableServiceIdSet
*
* DESCRIPTION:
*       Set egress vlan table service Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id, used as index in the Egress Vlan Translation Table.
*                       (APPLICABLE RANGES:0..8191).
*       vlanServiceId - tunnel start entry extension value (APPLICABLE RANGES: 0...FFFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgessVlanTableServiceIdSet
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN GT_U32               vlanServiceId
);


/*******************************************************************************
* cpssDxChTunnelStartEgessVlanTableServiceIdGet
*
* DESCRIPTION:
*       Get egress vlan table service Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id, used as index in the Egress Vlan Translation Table.
*                       (APPLICABLE RANGES:0..8191).
* OUTPUTS:
*       vlanServiceIdPtr  - (pointer to) vlan service Id.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTunnelStartEgessVlanTableServiceIdGet
(
    IN  GT_U8                devNum,
    IN  GT_U16               vlanId,
    OUT GT_U32               *vlanServiceIdPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTunnelh */

