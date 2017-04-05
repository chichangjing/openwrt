/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgEgrFlt.h
*
* DESCRIPTION:
*       Egress filtering facility DxCh cpss implementation
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*
*******************************************************************************/
#ifndef __cpssDxChBrgEgrFlth
#define __cpssDxChBrgEgrFlth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * Typedef: enum CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT
 *
 * Description:
 *      This enum defines ePort VLAN Egress Filtering VID selection mode.
 *  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Enumerations:
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_TAG1_E       - Tag1 VLAN ID is used
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_EVLAN_E      - The eVLAN is used
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ORIG_VLAN_E  - The original VLAN is used
 *
 *  Comments:
 *
 */
typedef enum
{
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_TAG1_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_EVLAN_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ORIG_VLAN_E
} CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT
 *
 * Description:
 *      This enum defines ePort VLAN Egress Filtering Table access mode.
 *      The table is an array of 2^20 bits seen as matrix with ePort and mappedVid
 *      coordinates. The described mode is the matrix dimensions.
 *      Each bit of the matrix means "port is a VLAN member".
 *  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Enumerations:
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_256_E  - 256 ports * 4K VLANs
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_512_E  - 512 ports * 2K VLANs
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_1K_E   - 1K ports  * 1K VLANs
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_2K_E   - 2K ports  * 512 VLANs
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_4K_E   - 4K ports  * 256 VLANs
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_8K_E   - 8K ports  * 128 VLANs
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_16K_E  - 16K ports * 64 VLANs
 *     CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_32K_E  - 32K ports * 32 VLANs
 *
 *  Comments:
 *
 */
typedef enum
{
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_256_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_512_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_1K_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_2K_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_4K_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_8K_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_16K_E,
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_32K_E
} CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT;

/*******************************************************************************
* cpssDxChBrgPortEgrFltUnkEnable
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unknown Unicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       portNum  - CPU port, port number
*       enable   - GT_TRUE: Unknown Unicast packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: Unknown Unicast packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUnkEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgPortEgrFltUnkEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unknown Unicast packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - CPU port, port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unknown unicast packets filtering
*                   GT_TRUE:  Unknown Unicast packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: Unknown Unicast packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUnkEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregMcastEnable
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unregistered Multicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       portNum  - CPU port, port number
*       enable   - GT_TRUE: Unregistered Multicast packets are filtered and
*                           are not forwarded to this port.
*                  GT_FALSE:Unregistered Multicast packets are not filtered
*                           and may be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregMcastEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregMcastEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unregistered Multicast packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - CPU port, port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unregistered multicast packets filtering
*                   GT_TRUE:  Unregistered Multicast packets are filtered and
*                             are not forwarded to this port.
*                   GT_FALSE: Unregistered Multicast packets are not filtered
*                             and may be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregMcastEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN Egress Filtering on specified device for Bridged
*       Known Unicast packets.
*       If enabled the VLAN egress filter verifies that the egress port is a
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: VLAN egress filtering verifies that the egress
*                          port is a member of the packet's VLAN classification
*                 GT_FLASE: the VLAN egress filtering check is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       VLAN egress filtering is required by 802.1Q, but if desired, this
*       mechanism can be disabled, thus allowing "leaky VLANs".
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgVlanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on specified device for Bridged Known Unicast packets.
*      If enabled the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged known unicast packets filtering
*                   GT_TRUE:   VLAN egress filtering verifies that the egress
*                              port is a member of the packet's VLAN classification
*                   GT_FLASE: the VLAN egress filtering check is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgRoutedUnicastEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN egress filtering on Routed Unicast packets.
*       If disabled, the destination port may or may not be a member of the VLAN.
*       If enabled, the VLAN egress filter verifies that the egress port is a
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: Egress filtering is enabled
*                  GT_FLASE: Egress filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedUnicastEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
);


/*******************************************************************************
* cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on Routed Unicast packets.
*      If disabled, the destination port may or may not be a member of the VLAN.
*      If enabled, the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed unicast packets filtering
*                   GT_TRUE:  Egress filtering is enabled
*                   GT_FLASE: Egress filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgRoutedSpanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable STP egress Filtering on Routed Packets.
*       If disabled the packet may be forwarded to its egress port,
*       regardless of its Span State.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: Span state filtering is enabled
*                  GT_FLASE: Span state filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedSpanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgRoutedSpanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the STP egress Filtering current state (enable/disable)
*      on Routed packets.
*      If disabled the packet may be forwarded to its egress port,
*      regardless of its Span State.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed packets filtering
*                   GT_TRUE:  Span state filtering is enabled
*                   GT_FLASE: Span state filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedSpanEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
);


/*******************************************************************************
* cpssDxChBrgPortEgrFltUregBcEnable
*
* DESCRIPTION:
*       Enables or disables egress filtering of unregistered broadcast packets.
*       Unregistered broadcast packets are:
*        - packets with destination MAC ff-ff-ff-ff-ff-ff
*        - destination MAC address lookup not finds matching entry
*        - packets were not routed
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       dev     - device number
*       portNum - port number
*       enable  - GT_TRUE  - enable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are dropped
*                 GT_FALSE - disable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are not dropped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregBcEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregBcEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      of unregistered broadcast packets.
*      Unregistered broadcast packets are:
*      - packets with destination MAC ff-ff-ff-ff-ff-ff
*      - destination MAC address lookup not finds matching entry
*      - packets were not routed
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) unregistered broadcast packets filtering
*                   GT_TRUE  - enable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are dropped
*                   GT_FALSE - disable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregBcEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgPortEgressMcastLocalEnable
*
* DESCRIPTION:
*       Enable/Disable sending Multicast packets back to its source
*       port on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev     - device number
*       portNum - port number
*       enable  - Boolean value:
*                 GT_TRUE  - Multicast packets may be sent back to
*                            their source port on the local device.
*                 GT_FALSE - Multicast packets are not sent back to
*                            their source port on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For xCat and above devices to enable local switching of Multicast,
*       unknown Unicast, and Broadcast traffic, both egress port configuration
*       and the field in the VLAN entry (by function
*       cpssDxChBrgVlanLocalSwitchingEnableSet) must be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgressMcastLocalEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgPortEgressMcastLocalEnableGet
*
* DESCRIPTION:
*      This function gets current state (enable/disable) for sending
*      Multicast packets back to its source port on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) sending Multicast packets back to its source
*                   GT_TRUE  - Multicast packets may be sent back to
*                              their source port on the local device.
*                   GT_FALSE - Multicast packets are not sent back to
*                              their source port on the local device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgressMcastLocalEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgPortEgrFltIpMcRoutedEnable
*
* DESCRIPTION:
*      Enable/Disable egress filtering for IP Multicast Routed packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       dev      - device number
*       portNum  - port number
*       enable   - GT_TRUE: IP Multicast Routed packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: IP Multicast Routed packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltIpMcRoutedEnable
(
    IN GT_U8            dev,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for IP Multicast Routed packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       dev             - device number
*       portNum         - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) IP Multicast Routed packets filtering
*                   GT_TRUE:  IP Multicast Routed packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: IP Multicast Routed packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortFilteringEnableSet
*
* DESCRIPTION:
*       Enable/Disable ePort VLAN Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       enable  - GT_FALSE - disable ePort VLAN Egress Filtering.
*                 GT_TRUE  - enable ePort VLAN Egress Filtering.
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortFilteringEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortFilteringEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of ePort VLAN Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - egress port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                    GT_FALSE - disable ePort VLAN Egress Filtering.
*                    GT_TRUE  - enable ePort VLAN Egress Filtering.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortFilteringEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortVidSelectModeSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering VID selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       vidSelectMode  - VID selection mode
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortVidSelectModeSet
(
    IN  GT_U8                                                devNum,
    IN  CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT  vidSelectMode
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortVidSelectModeGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering VID selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       vidSelectModePtr  - (pointer to) VID selection mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on unexpected HW contents
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortVidSelectModeGet
(
    IN  GT_U8                                                devNum,
    OUT CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT  *vidSelectModePtr
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortVidMappingSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering VID Mapping Table entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vid           - source VLAN Id.
*       vidIndex      - target VLAN Id.
*                       (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - on out-of-range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortVidMappingSet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vid,
    IN  GT_U32           vidIndex
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortVidMappingGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering VID Mapping Table entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       vid          - source VLAN Id.
*
* OUTPUTS:
*       vidIndexPtr  - (pointer to)target VLAN Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortVidMappingGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vid,
    OUT GT_U32           *vidIndexPtr
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortAccessModeSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering Table access mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       fltTabAccessMode  - Filtering Table access mode
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortAccessModeSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT fltTabAccessMode
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortAccessModeGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering Table access mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       fltTabAccessModePtr  - (pointer to)
*                             Filtering Table access mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortAccessModeGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT *fltTabAccessModePtr
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortMemberSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filter Table ePort-VLAN membership.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vidIndex - VID index (after mapping)
*       portNum  - egress port number
*       isMember - GT_FALSE - ePort is not VLAN member, packet will be filtered.
*                  GT_TRUE  - ePort is VLAN member, packet will not be filtered.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - portNum is out of range based on the
*                                  egress evlan access mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortMemberSet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vidIndex,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          isMember
);

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortMemberGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filter Table ePort-VLAN membership.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vidIndex - VID index (after mapping)
*       portNum  - egress port number
*
* OUTPUTS:
*       isMemberPtr - (pointer to)
*                  GT_FALSE - ePort is not VLAN member, packet will be filtered.
*                  GT_TRUE  - ePort is VLAN member, packet will not be filtered.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - portNum is out of range based on the
*                                  egress evlan access mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortMemberGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vidIndex,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *isMemberPtr
);


/*******************************************************************************
* cpssDxChBrgEgrMeshIdConfigurationSet
*
* DESCRIPTION:
*       Set configuration of Mesh ID for the egress ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_FALSE: MESH ID filtering is not enabled
*                      GT_TRUE:  MESH ID filtering is enabled
*       meshIdOffset - The MESH ID assigned to a packet is conveyed in the SrcID
*                      assigned to the packet.
*                      This configuration specifies the location of the LSB of
*                      the MESH ID in the SrcID field assigned to a packet
*                      (APPLICABLE RANGES: 0..11)
*
*       meshIdSize   - Specifies the number of bits that are used for
*                      the MESH ID in the SrcID field.
*                      Together with <MESH ID offset>, the MESH ID assigned to
*                      a packet can be extracted:
*                      Packet's MESH ID = <SST ID> (<MESH ID size> - 1 +
*                      <MESH ID offset> : <MESH ID offset>)
*                      (APPLICABLE RANGES: 1..4)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_OUT_OF_RANGE          - on wrong meshIdOffset and meshIdSize
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrMeshIdConfigurationSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  enable,
    IN GT_U32                   meshIdOffset,
    IN GT_U32                   meshIdSize
);

/*******************************************************************************
* cpssDxChBrgEgrMeshIdConfigurationGet
*
* DESCRIPTION:
*       Get configuration of Mesh ID for the egress ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*
*       enablePtr       - (pointer to)
*                       GT_FALSE: MESH ID filtering is not enabled
*                       GT_TRUE:  MESH ID filtering is enabled
*       meshIdOffsetPtr - (pointer to) The MESH ID assigned to a packet is conveyed
*                       in the SrcID assigned to the packet.
*                       This configuration specifies the location of the LSB of
*                       the MESH ID in the SrcID field assigned to a packet
*                       (APPLICABLE RANGES: 0..11)
*
*       meshIdSizePtr   - (pointer to) Specifies the number of bits that are used
*                       for the MESH ID in the SrcID field.
*                       Together with <MESH ID offset>, the MESH ID assigned to
*                       a packet can be extracted:
*                       Packet's MESH ID = <SST ID> (<MESH ID size> - 1 +
*                       <MESH ID offset> : <MESH ID offset>)
*                       (APPLICABLE RANGES: 1..4)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrMeshIdConfigurationGet
(
    IN  GT_U8                    devNum,
    OUT GT_BOOL                  *enablePtr,
    OUT GT_U32                   *meshIdOffsetPtr,
    OUT GT_U32                   *meshIdSizePtr
);


/*******************************************************************************
* cpssDxChBrgEgrPortMeshIdSet
*
* DESCRIPTION:
*     Set the Mesh ID number for Source-based (Split Horizon)
*     Filtering per ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - egress port number
*       meshId   - The Egress Mesh ID number for Source-based (Split Horizon)
*                  Filtering per ePort. (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_OUT_OF_RANGE          - on wrong meshId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrPortMeshIdSet
(
    IN GT_U8           devNum,
    IN GT_PORT_NUM     portNum,
    IN GT_U32          meshId
);

/*******************************************************************************
* cpssDxChBrgEgrPortMeshIdGet
*
* DESCRIPTION:
*     Get the Mesh ID number for Source-based (Split Horizon)
*     Filtering per ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - egress port number
*
* OUTPUTS:
*       meshIdPtr- (pointer to) The Egress Mesh ID number for Source-based
*                  (Split Horizon) Filtering per ePort.(APPLICABLE RANGES: 0..15)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrPortMeshIdGet
(
    IN GT_U8           devNum,
    IN GT_PORT_NUM     portNum,
    OUT GT_U32         *meshIdPtr
);

/*******************************************************************************
* cpssDxChBrgEgrFltPortVid1FilteringEnableSet
*
* DESCRIPTION:
*       Enable/Disable port VID1 Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       enable  - GT_FALSE - disable filtering
*                 GT_TRUE  - enable filtering
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortVid1FilteringEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgEgrFltPortVid1FilteringEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of port VID1 Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - egress port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                    GT_FALSE - the filtering is enabled
*                    GT_TRUE  - the filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortVid1FilteringEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
);

/*******************************************************************************
* cpssDxChBrgEgrFltPortVid1Set
*
* DESCRIPTION:
*       Set port associated VID1. Relevant to egress VID1 Filtering per port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       vid1    - VID1 associated with port (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on vid1 value out of an applicable range
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortVid1Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U16           vid1
);

/*******************************************************************************
* cpssDxChBrgEgrFltPortVid1Get
*
* DESCRIPTION:
*       Get port associated VID1. Relevant to egress VID1 Filtering per port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*
* OUTPUTS:
*       vid1Ptr    - (pointer to) VID1 associated with port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL vid1Ptr
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortVid1Get
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_U16           *vid1Ptr
);

/*******************************************************************************
* cpssDxChBrgEgrFltPortLinkEnableSet
*
* DESCRIPTION:
*       Enable/disable egress Filtering for 'Link state' on specified port on specified device.
*       set the port as 'force link up' (no filtering) , or as 'force link down' (filtering)
*
* APPLICABLE DEVICES:
*        Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       enable   - GT_TRUE  for     filtering the port (considered as 'link down')
*                       packets are filtered and are not forwarded to this port.
*                  GT_FALSE for not filtering the port (considered as 'link up')
*                       packets are not filtered and may be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortLinkEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgEgrFltPortLinkEnableGet
*
* DESCRIPTION:
*       This function gets the egress Filtering current state (enable/disable)
*       for 'Link state' on specified port on specified device.
*       is the port as 'force link up' (no filtering) , or as 'force link down' (filtering) ?
*
* APPLICABLE DEVICES:
*        Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr- (pointer to) the filter state :
*                  GT_TRUE  for     filtering the port (considered as 'link down')
*                       packets are filtered and are not forwarded to this port.
*                  GT_FALSE for not filtering the port (considered as 'link up')
*                       packets are not filtered and may be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortLinkEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgEgrFlth */
