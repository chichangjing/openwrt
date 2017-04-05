/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgPe.h
*
* DESCRIPTION:
*       Header for Bridge Port Extender (BPE) DxCh cpss implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/
#ifndef __cpssDxChBrgPeh
#define __cpssDxChBrgPeh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*******************************************************************************
* cpssDxChBrgPeEnableSet
*
* DESCRIPTION:
*    Enables globally Bridge Port Extender (BPE) feature.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE:  BPE feature enabled.
*                 GT_FALSE: BPE feature disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPeEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgPeEnableGet
*
* DESCRIPTION:
*    Gets Bridge Port Extender (BPE) global enabling status.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE:  BPE feature enabled.
*                                 GT_FALSE: BPE feature disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPeEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgPePortEnableSet
*
* DESCRIPTION:
*    Enables per port Bridge Port Extender (BPE) feature.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*       enable  - GT_TRUE:  port enabled for BPE feature.
*                 GT_FALSE: port disabled for BPE feature.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPePortEnableSet
(
    IN GT_U8                  devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL                enable
);

/*******************************************************************************
* cpssDxChBrgPePortEnableGet
*
* DESCRIPTION:
*    Gets Bridge Port Extender (BPE) enabling status per port.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*
* OUTPUTS:
*       enablePtr - (pointer to) GT_TRUE:  port enabled for BPE feature.
*                                GT_FALSE: port disabled for BPE feature.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device or port number
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPePortEnableGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL                *enablePtr
);


/*******************************************************************************
* cpssDxChBrgPePortPcidMcFilterEnableSet
*
* DESCRIPTION:
*    Configures <port,PCID> pair for downsteam multicast source filtering.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*       pcid           - Port E-Channel Identifier (APPLICABLE RANGES: 0..4095)
*       mcFilterEnable - GT_TRUE:  Multicast source filtering is enabled for the
*                                  <port,PCID> pair.
*                      - GT_FALSE: Multicast source filtering is disabled for the
*                                 <port,PCID> pair.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device, port number or PCID value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPePortPcidMcFilterEnableSet
(
    IN GT_U8                  devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32                 pcid,
    IN GT_BOOL                mcFilterEnable
);

/*******************************************************************************
* cpssDxChBrgPePortPcidMcFilterEnableGet
*
* DESCRIPTION:
*    Gets downsteam multicast source filtering enabling status for <port,PCID> pair.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       pcid    - Port E-Channel Identifier (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       mcFilterEnablePtr - (pointer to)
*                           GT_TRUE:  Multicast source filtering is enabled for the
*                                     <port,PCID> pair.
*                           GT_FALSE: Multicast source filtering is disabled for the
*                                     <port,PCID> pair.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device, port number or PCID value
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPePortPcidMcFilterEnableGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32                 pcid,
    OUT GT_BOOL                *mcFilterEnablePtr
);

/*******************************************************************************
* cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet
*
* DESCRIPTION:
*       Associate the cascade port (not DSA cascade port) with a trunkId.
*       this trunk id should represent the 'uplink trunk'
*       The purpose as described in the Functional specification is for "Trunk-ID for E-Tag<Ingress_E-CID_base>
*       assignment for multi-destination traffic"
*       NOTE: this function MUST not be called on 'Control Bridge'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number
*       cascadePortNum - physical port number of the BPE 802.1br cascade port (not DSA cascade port)
*       enable         - GT_TRUE:  associate the trunkId to cascadePortNum.
*                      - GT_FALSE: disassociate the trunkId from cascadePortNum.
*       trunkId        - the trunkId (of the uplink trunk)
*                        note: relevant only when enable == GT_TRUE
*                        APPLICABLE RANGE: 1..(4K-1).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad devNum, cascadePortNum
*       GT_OUT_OF_RANGE          - trunkId out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     cascadePortNum,
    IN GT_BOOL                  enable,
    IN GT_TRUNK_ID              trunkId
);

/*******************************************************************************
* cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet
*
* DESCRIPTION:
*       Get the associate trunkId with the cascade port (not DSA cascade port) .
*       this trunk id should represent the 'uplink trunk'
*       The purpose as described in the Functional specification is for "Trunk-ID for E-Tag<Ingress_E-CID_base>
*       assignment for multi-destination traffic"
*       NOTE: this function MUST not be called on 'Control Bridge'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number
*       cascadePortNum - physical port number of the BPE 802.1br cascade port (not DSA cascade port)
*
* OUTPUTS:
*       enablePtr      - (pointer to) indication that trunkId associate to the cascadePortNum.
*                      - GT_TRUE:  trunkId associate to the cascadePortNum.
*                      - GT_FALSE: trunkId is not associate to the cascadePortNum.
*       trunkIdPtr     - the trunkId (of the uplink trunk)
*                        note: relevant only when (*enablePtr) == GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad devNum, cascadePortNum
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - inconsistency in HW regarding value of the 'associated trunkId'
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     cascadePortNum,
    OUT GT_BOOL                  *enablePtr,
    OUT GT_TRUNK_ID              *trunkIdPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ____cpssDxChBrgPeh */

