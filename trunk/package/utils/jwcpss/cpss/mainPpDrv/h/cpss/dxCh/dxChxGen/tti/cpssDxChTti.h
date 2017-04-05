/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTti.h
*
* DESCRIPTION:
*       CPSS tunnel termination declarations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 54 $
*
*******************************************************************************/

#ifndef __cpssDxChTtih
#define __cpssDxChTtih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/tti/cpssDxChTtiTypes.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>

/*******************************************************************************
* cpssDxChTtiMacToMeSet
*
* DESCRIPTION:
*       function sets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacToMeSet
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC                      *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    IN  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
);


/*******************************************************************************
* cpssDxChTtiMacToMeGet
*
* DESCRIPTION:
*       This function gets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacToMeGet
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    OUT CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
);

/*******************************************************************************
* cpssDxChTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       keyType       - TTI key type
*       enable        - GT_TRUE: enable TTI lookup
*                       GT_FALSE: disable TTI lookup
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
GT_STATUS cpssDxChTtiPortLookupEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             enable
);
/*******************************************************************************
* cpssDxChTtiPortLookupEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of  the
*       TTI lookup for the specified key type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       keyType       - TTI key type
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup
*                       GT_TRUE: TTI lookup is enabled
*                       GT_FALSE: TTI lookup is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id, port or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortLookupEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for tunneled packets
*                       GT_FALSE: disable IPv4 TTI lookup only for tunneled packets
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
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only tunneled packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       tunneled packets
*                       GT_TRUE: IPv4 TTI lookup only for tunneled packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for tunneled packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for mac to me packets
*                       GT_FALSE: disable IPv4 TTI lookup only for mac to me packets
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
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: IPv4 TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiIpv4McEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for IPv4 multicast
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: enable TTI lookup for IPv4 MC
*                       GT_FALSE: disable TTI lookup for IPv4 MC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4McEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChTtiIpv4McEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of TTI lookup for
*       IPv4 multicast (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup for IPv4 MC
*                       GT_TRUE: TTI lookup for IPv4 MC enabled
*                       GT_FALSE: TTI lookup for IPv4 MC disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4McEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MPLS TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable MPLS TTI lookup only for mac to me packets
*                       GT_FALSE: disable MPLS TTI lookup only for mac to me packets
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
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MPLS TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: MPLS TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MPLS TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MIM TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE:  enable MIM TTI lookup only for mac to me packets
*                       GT_FALSE: disable MIM TTI lookup only for mac to me packets
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
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_BOOL         enable
);

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MIM TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MIM TTI lookup only for
*                       mac to me packets
*                       GT_TRUE:  MIM TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MIM TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_STATE             - a rule of different type was found in the specified index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       index            - Global index of the rule in the TCAM
*       actionType       - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid
);

/*******************************************************************************
* cpssDxChTtiRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr
);

/*******************************************************************************
* cpssDxChTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
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
*       Not needed for TRILL key
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
);

/*******************************************************************************
* cpssDxChTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
);

/*******************************************************************************
* cpssDxChTtiPclIdSet
*
* DESCRIPTION:
*       This function sets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type
*       pclId         - PCL ID value (10 bits)
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
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPclIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_U32                          pclId
);

/*******************************************************************************
* cpssDxChTtiPclIdGet
*
* DESCRIPTION:
*       This function gets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type
*
* OUTPUTS:
*       pclIdPtr      - (points to) PCL ID value (10 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPclIdGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    OUT GT_U32                          *pclIdPtr
);

/*******************************************************************************
* cpssDxChTtiExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
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
*       For Bobcat2, Caelum, Bobcat3:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*       For DxCh3, xCat, xCat3, Lion, xCat2, Lion2:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are:
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E
*           The commands are:
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
);

/*******************************************************************************
* cpssDxChTtiExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Bobcat2, Caelum, Bobcat3:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*       For DxCh3, xCat, xCat3, Lion, xCat2, Lion2:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are:
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E
*           The commands are:
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
);

/*******************************************************************************
* cpssDxChTtiExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set tunnel termination exception CPU code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set CPU code for.
*                       valid values:
*                       CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*       code          - the code for the exception type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCpuCodeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_NET_RX_CPU_CODE_ENT            code
);

/*******************************************************************************
* cpssDxChTtiExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get tunnel termination exception CPU code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to get CPU code for.
*                       valid values:
*                       CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*
* OUTPUTS:
*       codePtr    - (points to) the code for the exception type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCpuCodeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT CPSS_NET_RX_CPU_CODE_ENT            *codePtr
);

/*******************************************************************************
* cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function globally enables/disables bypassing IPv4 header length criteria checks as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
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
*******************************************************************************/
GT_STATUS cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function gets the globally bypassing IPv4 header length criteria check as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable bypass header length check
*                       GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
(
    IN  GT_U8                               devNum,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortIpTotalLengthDeductionEnableSet
*
* DESCRIPTION:
*       For MACSEC packets over IPv4/6 tunnel, that are to be tunnel terminated,
*       this configuration enables aligning the IPv4/6 total header length to the
*       correct offset.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort.
*       enable  - Enable/disable Ipv4/6 Total Length Deduction.
*                 GT_TRUE: When enabled, and ePort default attribute
*                         <IPv4/6 Total Length Deduction Enable> == Enabled:
*                         - for IPv4:
*                           <IPv4 Total Length> = Packet IPv4 header <Total Length> -
*                           Global configuration < IPv4 Total Length Deduction Value>
*                         - for IPv6:
*                           <IPv6 Total Length> = Packet IPv6 header <Total Length> -
*                           Global configuration < IPv6 Total Length Deduction Value>
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpTotalLengthDeductionEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChTtiPortIpTotalLengthDeductionEnableGet
*
* DESCRIPTION:
*       Get if IPv4/6 total header length is aligned to the correct offset
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr -  Enable/disable Ipv4/6 Total Length Deduction.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpTotalLengthDeductionEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChTtiIpTotalLengthDeductionValueSet
*
* DESCRIPTION:
*       Set Global configuration IPv4 or IPv6 Total Length Deduction Value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       ipType - IPv4 or Ipv6; valid values:
*                   CPSS_IP_PROTOCOL_IPV4_E
*                   CPSS_IP_PROTOCOL_IPV6_E
*       value  - IPv4 or IPv6 Total Length Deduction Value .(APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpTotalLengthDeductionValueSet
(
    IN GT_U8                                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT           ipType,
    IN GT_U32                               value
);

/*******************************************************************************
* cpssDxChTtiIpTotalLengthDeductionValueGet
*
* DESCRIPTION:
*       Get Global configuration IPv4 or IPv6 Total Length Deduction Value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       ipType - IPv4 or Ipv6; valid values:
*                   CPSS_IP_PROTOCOL_IPV4_E
*                   CPSS_IP_PROTOCOL_IPV6_E
*
* OUTPUTS:
*       valuePtr -  (pointer to) IPv4 or IPv6 Total Length Deduction Value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpTotalLengthDeductionValueGet
(
    IN  GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT           ipType,
    OUT GT_U32                              *valuePtr
);

/*******************************************************************************
* cpssDxChTtiEthernetTypeSet
*
* DESCRIPTION:
*       This function sets the TTI Ethernet type value that is used
*       to identify packets for TTI triggering.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       ethertypeType - Ethernet type
*       ethertype     - Ethernet type value (range 16 bits)
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
*   If there are 2 registers used for ethertype configuration,
*   one for ethertype identification of incoming tunneled packets in TTI
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration, these registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiEthernetTypeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_ETHERTYPE_TYPE_ENT      ethertypeType,
    IN  GT_U32                              ethertype
);

/*******************************************************************************
* cpssDxChTtiEthernetTypeGet
*
* DESCRIPTION:
*       This function gets the TTI Ethernet type value that is used
*       to identify packets for TTI triggering.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       ethertypeType - Ethernet type
*
* OUTPUTS:
*       ethertypePtr  - Points to Ethernet type value (range 16 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*   If there are 2 registers used for ethertype configuration,
*   one for ethertype identification of incoming tunneled packets in TTI
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration, these registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiEthernetTypeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_ETHERTYPE_TYPE_ENT      ethertypeType,
    OUT GT_U32                              *ethertypePtr
);

/*******************************************************************************
* cpssDxChTtiTrillCpuCodeBaseSet
*
* DESCRIPTION:
*       This function sets the Trill cpu code base.
*       TRILL CPU codes are relative to global configurable CPU code base value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       cpuCodeBase   - The base CPU code value for the TRILL engine .(APPLICABLE RANGES: 192..244)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillCpuCodeBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          cpuCodeBase
);

/*******************************************************************************
* cpssDxChTtiTrillCpuCodeBaseGet
*
* DESCRIPTION:
*       This function sets the Trill cpu code base.
*       TRILL CPU codes are relative to global configurable CPU code base value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*
* OUTPUTS:
*       cpuCodeBasePtr - Points to the base CPU code value for the TRILL engine
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillCpuCodeBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_U32          *cpuCodeBasePtr
);

/*******************************************************************************
* cpssDxChTtiTrillAdjacencyCheckEntrySet
*
* DESCRIPTION:
*       This function sets entry in the TRILL Adjacency Check dedicated TCAM.
*       A TCAM lookup is performed for every TRILL packet processed by the TRILL engine.
*       The TRILL engine uses a single TCAM lookup to implement
*          the following TRILL adjacency checks:
*          1. TRILL IS-IS Adjacency check -
*             Checks that the single-destination TRILL frame arrives from a
*             {neighbor, port} for which an IS-IS adjacency exists.
*          2. TRILL Tree Adjacency Check -
*             Checks that the multi-destination TRILL frame arrives from a
*             {neighbor, port} that is a branch on the given TRILL distribution tree.
*       If there is TCAM MISS, invoke the respective UC or Multi-Target exception command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - entry index in TRILL Adjacency TCAM .(APPLICABLE RANGES: 0..255)
*       valuePtr          - points to TRILL Adjacency STC
*       maskPtr           - points to TRILL Adjacency STC's mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillAdjacencyCheckEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *valuePtr,
    IN  CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *maskPtr
);

/*******************************************************************************
* cpssDxChTtiTrillAdjacencyCheckEntryGet
*
* DESCRIPTION:
*       This function gets entry in the TRILL Adjacency Check dedicated TCAM.
*       A TCAM lookup is performed for every TRILL packet processed by the TRILL engine.
*       The TRILL engine uses a single TCAM lookup to implement
*          the following TRILL adjacency checks:
*          1. TRILL IS-IS Adjacency check -
*             Checks that the single-destination TRILL frame arrives from a
*             {neighbor, port} for which an IS-IS adjacency exists.
*          2. TRILL Tree Adjacency Check -
*             Checks that the multi-destination TRILL frame arrives from a
*             {neighbor, port} that is a branch on the given TRILL distribution tree.
*       If there is TCAM MISS, invoke the respective UC or Multi-Target exception command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - entry index in TRILL Adjacency TCAM .(APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       valuePtr          - points to TRILL Adjacency STC
*       maskPtr           - points to TRILL Adjacency STC's mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillAdjacencyCheckEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *valuePtr,
    OUT CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *maskPtr
);

/*******************************************************************************
* cpssDxChTtiTrillMaxVersionSet
*
* DESCRIPTION:
*       This function sets the max Trill version.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       maxTrillVersion   - max TRILL version value (range 2 bits)
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
GT_STATUS cpssDxChTtiTrillMaxVersionSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          maxTrillVersion
);

/*******************************************************************************
* cpssDxChTtiTrillMaxVersionGet
*
* DESCRIPTION:
*       This function gets the max Trill version.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       maxTrillVersionPtr   - Points to max TRILL version value (range 2 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillMaxVersionGet
(
    IN  GT_U8           devNum,
    OUT GT_U32          *maxTrillVersionPtr
);

/*******************************************************************************
* cpssDxChTtiPortTrillEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TRILL engine on the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable TRILL engine on port
*                       GT_FALSE: disable TRILL engine on port
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
*       Configure TRILL Interface Enable bit for Egress ePort in table “HA Egress ePort Attribute Table 1”
*       Configure TRILL Engine Enable bit for Ingress ePort in table “Pre-TTI Lookup Ingress ePort Table”
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortTrillEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiPortTrillEnableGet
*
* DESCRIPTION:
*       This function gets the current state enables/disables of TRILL engine
*       on the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TRILL
*                       GT_TRUE:  TRILL engine enabled on port
*                       GT_FALSE: TRILL engine disabled on port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Same value is configured for Egress ePort in table “HA Egress ePort Attribute Table 1”
*       and Ingress ePort in table “Pre-TTI Lookup Ingress ePort Table”
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortTrillEnableGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    OUT GT_BOOL             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortTrillOuterVid0Set
*
* DESCRIPTION:
*       This function sets the Outer Tag0 VID that must be for all TRILL packets
*       from the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*       vlanId    - TRILL Outer Tag0 VID .(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortTrillOuterVid0Set
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_U16          vlanId
);

/*******************************************************************************
* cpssDxChTtiPortTrillOuterVid0Get
*
* DESCRIPTION:
*       This function gets the Outer Tag0 VID that must be for all TRILL packets
*       from the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       vlanIdPtr - (pointer to) TRILL Outer Tag0 VID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortTrillOuterVid0Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U16          *vlanIdPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupMacToMeSet
*
* DESCRIPTION:
*       This function sets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacToMeSet
(
    IN  GT_U8                                           devNum,
    IN  GT_PORT_GROUPS_BMP                              portGroupsBmp,
    IN  GT_U32                                          entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC                      *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    IN  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupMacToMeGet
*
* DESCRIPTION:
*       This function gets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacToMeGet
(
    IN  GT_U8                                           devNum,
    IN  GT_PORT_GROUPS_BMP                              portGroupsBmp,
    IN  GT_U32                                          entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    OUT CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
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
*       Not needed for TRILL key
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacModeSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
);

/*******************************************************************************
* cpssDxChTtiPortGroupMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_STATE             - a rule of different type was found in the specified index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index            - Index of the rule in the TCAM
*       actionType       - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*       offsetType   - the type of offset
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type. .(APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_TTI_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
);

/*******************************************************************************
* cpssDxChTtiPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_TTI_OFFSET_TYPE_ENT       *offsetTypePtr,
    OUT GT_U8                               *offsetPtr
);


/*******************************************************************************
* cpssDxChTtiPortPassengerOuterIsTag0Or1Set
*
* DESCRIPTION:
*       Set per port TTI passenger Outer Tag is Tag0 or Tag1.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       mode - recognize Outer Tag existence according to TPID1 or TPID0
*                       CPSS_VLAN_ETHERTYPE0_E: Outer Tag exists if passenger outer VLAN Tag TPID = Tag0 TPID
*                       CPSS_VLAN_ETHERTYPE1_E: Outer Tag exists if passenger outer VLAN tag TPID = Tag1 TPID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port , mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortPassengerOuterIsTag0Or1Set
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    IN  CPSS_ETHER_MODE_ENT          mode
);

/*******************************************************************************
* cpssDxChTtiPortPassengerOuterIsTag0Or1Get
*
* DESCRIPTION:
*       Set per port TTI passenger Outer Tag is Tag0 or Tag1.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       modePtr - (pointer to)recognize Outer Tag existence according to TPID1 or TPID0
*                       CPSS_VLAN_ETHERTYPE0_E: Outer Tag exists if passenger outer VLAN Tag TPID = Tag0 TPID
*                       CPSS_VLAN_ETHERTYPE1_E: Outer Tag exists if passenger outer VLAN tag TPID = Tag1 TPID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortPassengerOuterIsTag0Or1Get
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    OUT  CPSS_ETHER_MODE_ENT          *modePtr
);

/*******************************************************************************
* cpssDxChTtiUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*       offsetType   - the type of offset
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type.(APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_TTI_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
);

/*******************************************************************************
* cpssDxChTtiUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiUserDefinedByteGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_U32                              udbIndex,
    OUT CPSS_DXCH_TTI_OFFSET_TYPE_ENT       *offsetTypePtr,
    OUT GT_U8                               *offsetPtr
);

/*******************************************************************************
* cpssDxChTtiPacketTypeKeySizeSet
*
* DESCRIPTION:
*       function sets key type size.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       size          - key size in TCAM
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
GT_STATUS cpssDxChTtiPacketTypeKeySizeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN CPSS_DXCH_TTI_KEY_SIZE_ENT           size
);

/*******************************************************************************
* cpssDxChTtiPacketTypeKeySizeGet
*
* DESCRIPTION:
*       function gets key type size.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*
* OUTPUTS:
*       sizePtr      - points to key size in TCAM
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPacketTypeKeySizeGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    OUT CPSS_DXCH_TTI_KEY_SIZE_ENT           *sizePtr
);

/*******************************************************************************
* cpssDxChTtiTcamSegmentModeSet
*
* DESCRIPTION:
*       Sets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type
*       segmentMode   - TTI TCAM segment mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTcamSegmentModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT               keyType,
    IN CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT  segmentMode
);

/*******************************************************************************
* cpssDxChTtiTcamSegmentModeGet
*
* DESCRIPTION:
*       Gets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       keyType         - key type
*
* OUTPUTS:
*       segmentModePtr  - (pointer to) TTI TCAM segment mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on invalid HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTcamSegmentModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT              keyType,
    OUT CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT *segmentModePtr
);

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationModeSet
*
* DESCRIPTION:
*       Set the tunnel duplication mode for a specific protocol on a specific
*       port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       protocol      - the protocol
*       mode          - the tunnel duplication mode
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
GT_STATUS cpssDxChTtiMcTunnelDuplicationModeSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_TUNNEL_MULTICAST_TYPE_ENT                   protocol,
    IN CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationModeGet
*
* DESCRIPTION:
*       Get the tunnel duplication mode for a specific protocol on a specific
*       port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       protocol      - the protocol
*
* OUTPUTS:
*       modePtr       - (pointer to) the tunnel duplication mode
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
GT_STATUS cpssDxChTtiMcTunnelDuplicationModeGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN  CPSS_TUNNEL_MULTICAST_TYPE_ENT                  protocol,
    OUT CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT    *modePtr
);

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationUdpDestPortSet
*
* DESCRIPTION:
*       Set the UDP destination port used to trigger IPv4/IPv6 Multicast
*       replication
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       udpPort   - the UDP destination port
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
*       This configuration is relevant only when default ePort
*       <IPv4 MC Duplication IP Protocol Mode> = UDP or Default ePort
*       <IPv6 MC Duplication IP Protocol Mode> = UDP
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMcTunnelDuplicationUdpDestPortSet
(
    IN GT_U8    devNum,
    IN GT_U16   udpPort
);

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationUdpDestPortGet
*
* DESCRIPTION:
*       Get the UDP destination port used to trigger IPv4/IPv6 Multicast
*       replication
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       udpPortPtr - (pointer to) the UDP destination port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This configuration is relevant only when default ePort
*       <IPv4 MC Duplication IP Protocol Mode> = UDP or Default ePort
*       <IPv6 MC Duplication IP Protocol Mode> = UDP
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMcTunnelDuplicationUdpDestPortGet
(
    IN GT_U8    devNum,
    OUT GT_U16  *udpPortPtr
);

/*******************************************************************************
* cpssDxChTtiMplsMcTunnelTriggeringMacDaSet
*
* DESCRIPTION:
*       Set the MPLS multicast MAC DA and mask used for duplication triggering
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       addressPtr  - (pointer to) the MAC DA
*       maskPtr     - (pointer to) the address mask
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
GT_STATUS cpssDxChTtiMplsMcTunnelTriggeringMacDaSet
(
    IN GT_U8            devNum,
    IN GT_ETHERADDR     *addressPtr,
    IN GT_ETHERADDR     *maskPtr
);

/*******************************************************************************
* cpssDxChTtiMplsMcTunnelTriggeringMacDaGet
*
* DESCRIPTION:
*       Get the MPLS multicast MAC DA and mask used for duplication triggering
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       addressPtr - (pointer to) the MAC DA
*       maskPtr    - (pointer to) the address mask
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
GT_STATUS cpssDxChTtiMplsMcTunnelTriggeringMacDaGet
(
    IN GT_U8            devNum,
    OUT GT_ETHERADDR    *addressPtr,
    OUT GT_ETHERADDR    *maskPtr
);

/*******************************************************************************
* cpssDxChTtiPwCwExceptionCmdSet
*
* DESCRIPTION:
*       Set a PW CW exception command
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       exceptionType   - exception type
*       command         - the packet command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported mode
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwExceptionCmdSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT    exceptionType,
    IN CPSS_PACKET_CMD_ENT                  command
);

/*******************************************************************************
* cpssDxChTtiPwCwExceptionCmdGet
*
* DESCRIPTION:
*       Get a PW CW exception command
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       exceptionType   - exception type
*
* OUTPUTS:
*       commandPtr - (pointer to) the packet command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported mode
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT   exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
);

/*******************************************************************************
* cpssDxChTtiPwCwCpuCodeBaseSet
*
* DESCRIPTION:
*       Set the base CPU code value for PWE3
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       cpuCodeBase - the base CPU code value for PWE3
*                     (APPLICABLE RANGES: 192..251)
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
GT_STATUS cpssDxChTtiPwCwCpuCodeBaseSet
(
    IN GT_U8    devNum,
    IN GT_U32   cpuCodeBase
);

/*******************************************************************************
* cpssDxChTtiPwCwCpuCodeBaseGet
*
* DESCRIPTION:
*       Get the base CPU code value for PWE3
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*
* OUTPUTS:
*       cpuCodeBasePtr - (pointer to) the base CPU code value for PWE3
*                        (APPLICABLE RANGES: 192..255)
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
GT_STATUS cpssDxChTtiPwCwCpuCodeBaseGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *cpuCodeBasePtr
);

/*******************************************************************************
* cpssDxChTtiPwCwSequencingSupportEnableSet
*
* DESCRIPTION:
*       This function enables/disables the check for Pseudo Wire-Control Word
*       Data Word format <Sequence Number>==0 in terminated Pseudo Wires.
*       < Sequence Number > are the 16 LSB of "CW as Data Word format".
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: PW-CW sequencing supported
*                       GT_FALSE: PW-CW sequencing is not supported
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwSequencingSupportEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChTtiPwCwSequencingSupportEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of the check
*       for Pseudo Wire-Control Word Data Word format <Sequence Number>==0
*       in terminated Pseudo Wires.
*       < Sequence Number > are the 16 LSB of "CW as Data Word format".
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable PW-CW sequencing support
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwSequencingSupportEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChTtiIPv6ExtensionHeaderSet
*
* DESCRIPTION:
*       Set one of the 2 configurable IPv6 extension headers.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       extensionHeaderId    - the configurable extension header index
*                              (APPLICABLE RANGES: 0..1)
*       extensionHeaderValue - the configurable extension header value
*                              (APPLICABLE RANGES: 0..255)
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
*       The design assumes that the information following the IPv6 header is L4
*       unless there is a known next hop header. The design supports several
*       hard-coded next hop header values and two configurable ones.
*       If any of the following next hop values are identified, the <Is L4 Valid>
*       indication in the IPCL key is set to NOT VALID:
*       - HBH (0)
*       - IPv6 Routing header (43)
*       - IPv6 Fragment header (44)
*       - Encapsulation Security Payload (50)
*       - Authentication Header (51)
*       - IPv6 Destination Options (60)
*       - Mobility Header (135)
*       - <IPv6 Extension Value0>
*       - <IPv6 Extension Value1>
*       To disable a configurable header, set its value to one of the above
*       values.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIPv6ExtensionHeaderSet
(
    IN GT_U8    devNum,
    IN GT_U32   extensionHeaderId,
    IN GT_U32   extensionHeaderValue
);

/*******************************************************************************
* cpssDxChTtiIPv6ExtensionHeaderGet
*
* DESCRIPTION:
*       Get one of the 2 configurable IPv6 extension headers.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       extensionHeaderId        - the configurable extension header index
*                                  (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       extensionHeaderValuePtr  - the configurable extension header value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The design assumes that the information following the IPv6 header is L4
*       unless there is a known next hop header. The design supports several
*       hard-coded next hop header values and two configurable ones.
*       If any of the following next hop values are identified, the <Is L4 Valid>
*       indication in the IPCL key is set to NOT VALID:
*       - HBH (0)
*       - IPv6 Routing header (43)
*       - IPv6 Fragment header (44)
*       - Encapsulation Security Payload (50)
*       - Authentication Header (51)
*       - IPv6 Destination Options (60)
*       - Mobility Header (135)
*       - <IPv6 Extension Value0>
*       - <IPv6 Extension Value1>
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIPv6ExtensionHeaderGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  extensionHeaderId,
    OUT GT_U32  *extensionHeaderValuePtr
);

/*******************************************************************************
* cpssDxChTtiSourceIdBitsOverrideSet
*
* DESCRIPTION:
*       Set the SrcID bits that are overridden by the TTI action of SrcID
*       assignment.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       lookup          - the lookup number (APPLICABLE RANGES: 0..3)
*       overrideBitmap  - 12 bits bitmap. For each bit:
*                         0 - do not override
*                         1 - override
*                         (APPLICABLE RANGES: 0..4095)
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
GT_STATUS cpssDxChTtiSourceIdBitsOverrideSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookup,
    IN GT_U32   overrideBitmap
);

/*******************************************************************************
* cpssDxChTtiSourceIdBitsOverrideGet
*
* DESCRIPTION:
*       Get the SrcID bits that are overridden by the TTI action of SrcID
*       assignment.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       lookup              - the lookup number (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       overrideBitmapPtr   - 12 bits bitmap. For each bit:
*                             0 - do not override
*                             1 - override
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
GT_STATUS cpssDxChTtiSourceIdBitsOverrideGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  lookup,
    OUT GT_U32  *overrideBitmapPtr
);

/*******************************************************************************
* cpssDxChTtiGreExtensionsEnableSet
*
* DESCRIPTION:
*       Enable/Disable parsing of extensions (Checksum, Sequence, Key) on
*       IPv4/IPv6 GRE tunnels.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable/disable GRE extensions
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiGreExtensionsEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChTtiGreExtensionsEnableGet
*
* DESCRIPTION:
*       Gt the enabling status of the parsing of extensions (Checksum, Sequence,
*       Key) on IPv4/IPv6 GRE tunnels.
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
*       enablePtr   - (pointer to) GRE extensions enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiGreExtensionsEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTtih */

