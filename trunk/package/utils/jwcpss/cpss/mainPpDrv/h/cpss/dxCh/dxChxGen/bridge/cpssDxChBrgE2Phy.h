/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgE2Phy.h
*
* DESCRIPTION:
*       Eport to Physical port tables facility CPSS DxCh implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#ifndef __cpssDxChBrgE2Phy
#define __cpssDxChBrgE2Phy

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnelTypes.h>

/*
 * Typedef: struct CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC
 *
 * Description:
 *      Egress port info
 *
 * Fields:
 *
 *      forceArpTsPortDecision   - Determines whether the HA egress ePort table
 *                                 tunnel start / ARP related fields
 *                                 (i.e. modify MAC DA / SA, TS pointer etc')
 *                                 affects the TS/ARP assignment logic.
 *                                 False: Do not force HA egress ePort
 *                                        table TS/ARP decision
 *                                 True: Force HA egress ePort table
 *                                       TS/ARP decision
 *      tunnelStart            - GT_TRUE:  redirected to Tunnel Egress Interface
 *                               GT_FALSE: redirected to non-Tunnel Egress Interface
 *      tunnelStartPtr         - pointer to the Tunnel Start entry. valid if <tunnelStart>
 *                               is GT_TRUE
 *      tsPassengerPacketType  - Type of passenger packet for packet redirected
 *                               to Tunnel-Start.
 *      arpPtr                 - pointer to the ARP entry
 *      modifyMacSa            - Indicates that the MAC SA of the packet is changed
 *                               to the address of the current device.
 *                               Relevant only when the <TS> flag is disabled.
 *      modifyMacDa            - Indicates that the MAC DA of the packet should
 *                               be changed according to the <ARP pointer> field.
 *                               Relevant only when the <TS> flag is disabled.
 */
typedef struct
{
    GT_BOOL                                 tunnelStart;
    GT_U32                                  tunnelStartPtr;
    CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT     tsPassengerPacketType;
    GT_U32                                  arpPtr;
    GT_BOOL                                 modifyMacSa;
    GT_BOOL                                 modifyMacDa;

} CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC;

/*******************************************************************************
* cpssDxChBrgEportToPhysicalPortTargetMappingTableSet
*
* DESCRIPTION:
*       Set physical info for the given ePort in the E2PHY mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       physicalInfoPtr - (pointer to) physical Information related to the ePort
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       E2PHY table is accessed by the packet's target ePort
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEportToPhysicalPortTargetMappingTableSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_INTERFACE_INFO_STC          *physicalInfoPtr
);

/*******************************************************************************
* cpssDxChBrgEportToPhysicalPortTargetMappingTableGet
*
* DESCRIPTION:
*       Get physical info for the given ePort in the E2PHY mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       physicalInfoPtr - (pointer to) physical Information related to the ePort
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       E2PHY table is accessed by the packet's target ePort
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEportToPhysicalPortTargetMappingTableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_INTERFACE_INFO_STC             *physicalInfoPtr
);

/*******************************************************************************
* cpssDxChBrgEportToPhysicalPortEgressPortInfoSet
*
* DESCRIPTION:
*       Set physical info for the given ePort in the HA Egress port table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       egressInfoPtr   - (pointer to) physical information related to the egress port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEportToPhysicalPortEgressPortInfoSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_NUM                          portNum,
    IN CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC   *egressInfoPtr
);

/*******************************************************************************
* cpssDxChBrgEportToPhysicalPortEgressPortInfoGet
*
* DESCRIPTION:
*       Get physical info for the given ePort in the HA Egress port table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       egressInfoPtr - (pointer to) physical Information related to the egress port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEportToPhysicalPortEgressPortInfoGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC  *egressInfoPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgE2Phy */

