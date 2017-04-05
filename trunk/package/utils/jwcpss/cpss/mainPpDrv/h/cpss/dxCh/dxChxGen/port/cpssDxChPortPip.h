/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPip.h
*
* DESCRIPTION:
*         CPSS DxCh Port's Pre-Ingress Prioritization (PIP).
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssDxChPortPip
#define __cpssDxChPortPip

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * typedef: enum CPSS_DXCH_PORT_PIP_PROTOCOL_ENT
 *
 * Description: Enumeration of PIP type of supported protocols
 *          (etherTypes to identify protocol).
 *
 * Enumerations:
 *   CPSS_DXCH_PORT_PIP_PROTOCOL_VLAN_TAG_E - VLAN tag (TPID)
 *   CPSS_DXCH_PORT_PIP_PROTOCOL_MPLS_E - MPLS protocol
 *   CPSS_DXCH_PORT_PIP_PROTOCOL_IPV4_E - IPV4 protocol
 *   CPSS_DXCH_PORT_PIP_PROTOCOL_IPV6_E - IPV6 protocol
 *   CPSS_DXCH_PORT_PIP_PROTOCOL_UDE_E - 'user defined ethertype' protocol
 *
 */
typedef enum{
    CPSS_DXCH_PORT_PIP_PROTOCOL_VLAN_TAG_E,
    CPSS_DXCH_PORT_PIP_PROTOCOL_MPLS_E   ,
    CPSS_DXCH_PORT_PIP_PROTOCOL_IPV4_E   ,
    CPSS_DXCH_PORT_PIP_PROTOCOL_IPV6_E   ,
    CPSS_DXCH_PORT_PIP_PROTOCOL_UDE_E

}CPSS_DXCH_PORT_PIP_PROTOCOL_ENT;

/*
 * typedef: enum CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT
 *
 * Description: Enumeration for PIP classification of packet for priority assignment (per port).
 *
 * Enumerations:
 *   CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ETHERTYPE_AND_VID_E -
 *              packet with matched etherType and vid assign priority per port
 *   CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_DEFAULT_E -
 *              packet that was not classified assigned 'default' priority per port
 *
 */
typedef enum{
    CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ETHERTYPE_AND_VID_E,
    CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_DEFAULT_E

}CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT
 *
 * Description: Enumeration for PIP classification of packet for priority assignment (per profile).
 *
 * Enumerations:
 *   CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_DSA_QOS_PROFILE_E -
 *              packet with DSA TAG and <QOS PROFILE> assign priority per PIP profile
 *              per <QOS PROFILE> (APPLICABLE RANGES: 0..127)
 *   CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_DSA_UP_E -
 *              packet with DSA TAG assign priority per PIP profile
 *              per <UP> (APPLICABLE RANGES: 0..7)
 *   CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_VLAN_TAG_UP_E -
 *              packet with matched VLAN TAG assign priority per PIP profile
 *              per <UP> (APPLICABLE RANGES: 0..7)
 *   CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_MPLS_EXP_E -
 *              packet with MPLS assign priority per PIP profile
 *              per <EXP> (APPLICABLE RANGES: 0..7)
 *   CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_IPV4_TOS_E -
 *              packet with IPV4 assign priority per PIP profile
 *              per <TOS> (APPLICABLE RANGES: 0..255)
 *   CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_IPV6_TC_E -
 *              packet with IPV6 assign priority per PIP profile
 *              per <TC> (APPLICABLE RANGES: 0..255)
 *   CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_UDE_INDEX_E -
 *              packet with matched UDE (user defined ethertype) assign priority per PIP profile
 *              per <ude index> (APPLICABLE RANGES: 0..3)
 *   CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_MAC_DA_INDEX_E -
 *              packet with matched MAC DA assign priority per PIP profile
 *              per <macDa index> (APPLICABLE RANGES: 0..3)
 *
 */
typedef enum{
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_DSA_QOS_PROFILE_E,
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_DSA_UP_E,
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_VLAN_TAG_UP_E,
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_MPLS_EXP_E,
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_IPV4_TOS_E,
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_IPV6_TC_E,
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_UDE_INDEX_E,
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_MAC_DA_INDEX_E

}CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT;

/*
 * typedef: CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC
 *
 * Description: structure for PIP MAC DA classification info
 *
 * Fields:
 *      macAddrValue - the mac DA value
 *      macAddrMask  - the mac DA mask
 *
*/
typedef struct{
    GT_ETHERADDR  macAddrValue;
    GT_ETHERADDR  macAddrMask;
}CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC;

/*******************************************************************************
* cpssDxChPortPipGlobalEnableSet
*
* DESCRIPTION:
*       Enable/disable the PIP engine. (Global to the device)
*       if enabled packets are dropped according to their parsed priority if FIFOs
*       before the control pipe are above a configurable threshold
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       enable  - GT_TRUE  - enable  PIP
*                 GT_FALSE - disable PIP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChPortPipGlobalEnableGet
*
* DESCRIPTION:
*       Get the state Enable/disable of the PIP engine. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                 GT_TRUE  - enable  PIP
*                 GT_FALSE - disable PIP
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChPortPipGlobalEtherTypeProtocolSet
*
* DESCRIPTION:
*       Set the PIP etherTypes for the protocol. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       protocol  - the protocol for which we define the etherType
*       index     - relevant to 'vlan tag' that supports 4 etherTypes (TPIDs)
*                   relevant to 'mpls' that supports 2 etherTypes
*                   relevant to 'user defined' that supports 4 etherTypes
*                   ignored for other protocols.
*                   (APPLICABLE RANGES: 0..3)
*       etherType - etherType to recognize the protocols packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or protocol or index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalEtherTypeProtocolSet
(
    IN GT_U8        devNum,
    IN CPSS_DXCH_PORT_PIP_PROTOCOL_ENT  protocol,
    IN GT_U32       index,
    IN GT_U16       etherType
);

/*******************************************************************************
* cpssDxChPortPipGlobalEtherTypeProtocolGet
*
* DESCRIPTION:
*       Get the PIP etherTypes for the protocol. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       protocol  - the protocol for which we define the etherType
*       index     - relevant to 'mpls' that supports 2 etherTypes
*                   relevant to 'user defined' that supports 4 etherTypes
*                   ignored for other protocols.
*                   (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       etherType - (pointer to) etherType to recognize the protocols packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or protocol or index
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalEtherTypeProtocolGet
(
    IN GT_U8        devNum,
    IN CPSS_DXCH_PORT_PIP_PROTOCOL_ENT  protocol,
    IN GT_U32       index,
    OUT GT_U16      *etherTypePtr
);

/*******************************************************************************
* cpssDxChPortPipTrustEnableSet
*
* DESCRIPTION:
*       Set port as trusted/not trusted PIP port.
*       A 'trusted' port will set priority level for ingress packets according to
*       packet's fields.
*       Otherwise, for the 'not trusted' port will use the port’s default priority
*       level.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - GT_TRUE  - set port as 'trusted'
*                   GT_FALSE - set port as 'not trusted'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipTrustEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChPortPipTrustEnableGet
*
* DESCRIPTION:
*       Get port's trusted/not trusted .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr   -  (pointer to)
*                       GT_TRUE  - set port as 'trusted'
*                       GT_FALSE - set port as 'not trusted'
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipTrustEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChPortPipProfileSet
*
* DESCRIPTION:
*       Set port's PIP profile.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       pipProfile   - the profile of the port.
*                   (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipProfileSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  pipProfile
);

/*******************************************************************************
* cpssDxChPortPipProfileGet
*
* DESCRIPTION:
*       Get port's PIP profile.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       pipProfilePtr - (pointer to) the profile of the port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipProfileGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *pipProfilePtr
);

/*******************************************************************************
* cpssDxChPortPipVidClassificationEnableSet
*
* DESCRIPTION:
*       Set port PIP enable/disable VID classification .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable   - GT_TRUE   - enable VID classification
*                  GT_FALSE  - disable VID classification
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipVidClassificationEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChPortPipVidClassificationEnableGet
*
* DESCRIPTION:
*       Get port PIP enable/disable VID classification .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                  GT_TRUE   - enable VID classification
*                  GT_FALSE  - disable VID classification
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipVidClassificationEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChPortPipPrioritySet
*
* DESCRIPTION:
*       Set priority per Port for 'classification' type.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       type      - 'classification' type
*       priority  - the priority value to set. (0-high,1-mid,2-low)
*                   (APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum or type
*       GT_OUT_OF_RANGE          - on out of range priority
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipPrioritySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT type,
    IN  GT_U32                  priority
);

/*******************************************************************************
* cpssDxChPortPipPriorityGet
*
* DESCRIPTION:
*       Get priority per Port for 'classification' type.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       type      - 'classification' type
*
* OUTPUTS:
*       priorityPtr  - (pointer to)the priority value to set. (0-high,1-mid,2-low)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum or type
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipPriorityGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT type,
    OUT GT_U32                  *priorityPtr
);

/*******************************************************************************
* cpssDxChPortPipGlobalProfilePrioritySet
*
* DESCRIPTION:
*       Set priority per PIP profile Per 'classification' type. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum          - physical device number
*       pipProfile    - index to the 'profile table'.
*                   (APPLICABLE RANGES: 0..3)
*       type            - 'classification' type
*       fieldIndex      - index of the priority field in the 'profile entry'
*                          see details in description of relevant field in
*                          CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT
*       priority        - the priority value to set. (0-high,1-mid,2-low)
*                   (APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or type or pipProfile or fieldIndex
*       GT_OUT_OF_RANGE          - on out of range priority
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalProfilePrioritySet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  pipProfile,
    IN  CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT type,
    IN  GT_U32                  fieldIndex,
    IN  GT_U32                  priority
);


/*******************************************************************************
* cpssDxChPortPipGlobalProfilePriorityGet
*
* DESCRIPTION:
*       Get priority per PIP profile Per 'classification' type. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum          - physical device number
*       pipProfile    - index to the 'profile table'.
*                   (APPLICABLE RANGES: 0..3)
*       type            - 'classification' type
*       fieldIndex      - index of the priority field in the 'profile entry'
*                          see details in description of relevant field in
*                          CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT
*
* OUTPUTS:
*       priorityPtr        - (pointer to)the priority value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or type or pipProfile or fieldIndex
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalProfilePriorityGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  pipProfile,
    IN  CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT type,
    IN  GT_U32                  fieldIndex,
    OUT GT_U32                  *priorityPtr
);

/*******************************************************************************
* cpssDxChPortPipGlobalVidClassificationSet
*
* DESCRIPTION:
*       Set PIP vid classification value per index. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       index    - one of 4 indexes.
*                   (APPLICABLE RANGES: 0..3)
*       vid      - the vid to match
*                   (APPLICABLE RANGES: 0..(4k-1))
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or index
*       GT_OUT_OF_RANGE          - on out of range of vid
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalVidClassificationSet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    IN  GT_U16                  vid
);

/*******************************************************************************
* cpssDxChPortPipGlobalVidClassificationGet
*
* DESCRIPTION:
*       Get PIP vid classification value per index. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       index    - one of 4 indexes.
*                   (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       vidPtr   - (pointer to)the vid to match
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or index
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalVidClassificationGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_U16                  *vidPtr
);


/*******************************************************************************
* cpssDxChPortPipGlobalMacDaClassificationEntrySet
*
* DESCRIPTION:
*       Set PIP Mac Da classification entry. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       index    - index to the 'mac DA classification table'.
*                   (APPLICABLE RANGES: 0..3)
*       entryPtr - (pointer to) the entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or index
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on out of value in entry
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalMacDaClassificationEntrySet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    IN  CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC *entryPtr
);


/*******************************************************************************
* cpssDxChPortPipGlobalMacDaClassificationEntryGet
*
* DESCRIPTION:
*       Get PIP Mac Da classification entry. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       index    - index to the 'mac DA classification table'.
*                   (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       entryPtr - (pointer to) the entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or index
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalMacDaClassificationEntryGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC *entryPtr
);

/*******************************************************************************
* cpssDxChPortPipGlobalBurstFifoThresholdsSet
*
* DESCRIPTION:
*       Set Burst FIFO Threshold for specific PIP priority. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       priority  - the priority to set the threshold.
*                   (APPLICABLE RANGES: 0..2)
*       threshold  - the threshold value. (number of packets)
*                   (APPLICABLE RANGES: 0..0x3ff)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or priority
*       GT_OUT_OF_RANGE          - on out of range threshold
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalBurstFifoThresholdsSet
(
    IN GT_U8        devNum,
    IN GT_U32       priority,
    IN GT_U32       threshold
);

/*******************************************************************************
* cpssDxChPortPipGlobalBurstFifoThresholdsGet
*
* DESCRIPTION:
*       Get Burst FIFO Threshold for specific PIP priority. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       priority  - the priority to set the threshold.
*                   (APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       thresholdPtr - (pointer to)the threshold value. (number of packets)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or priority
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalBurstFifoThresholdsGet
(
    IN GT_U8        devNum,
    IN GT_U32        priority,
    OUT GT_U32       *thresholdPtr
);

/*******************************************************************************
* cpssDxChPortPipGlobalDropCounterGet
*
* DESCRIPTION:
*      Get all PIP drop counters (Global to the device)
*      NOTE: the counters are 'clear on read'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       countersArr - (pointer to) the counters of 3 priorities.(0-high,1-mid,2-low)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or priority
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipGlobalDropCounterGet
(
    IN GT_U8        devNum,
    OUT GT_U64      countersArr[3] 
);

/*******************************************************************************
* cpssDxChPortPipDropCounterGet
*
* DESCRIPTION:
*      Get PIP drop counter of specific port.
*      NOTE: the counters are 'clear on read'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       counterPtr - (pointer to) the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPipDropCounterGet
(
    IN GT_U8        devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U64      *counterPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortPip */

