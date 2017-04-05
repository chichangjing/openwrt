
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgSrcId.h
*
* DESCRIPTION:
*       CPSS DxCh Source Id facility API
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

#ifndef __cpssDxChBrgSrcId
#define __cpssDxChBrgSrcId

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgSrcId.h>


/*******************************************************************************
* cpssDxChBrgSrcIdGroupPortAdd
*
* DESCRIPTION:
*         Add a port to Source ID group. Packets assigned with this Source ID
*         may be forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupPortAdd
(
    IN GT_U8                    devNum,
    IN GT_U32                   sourceId,
    IN GT_PHYSICAL_PORT_NUM     portNum
);

/*******************************************************************************
* cpssDxChBrgSrcIdGroupPortDelete
*
* DESCRIPTION:
*         Delete a port from Source ID group. Packets assigned with this
*         Source ID will not be forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is deleted from
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupPortDelete
(
    IN GT_U8                    devNum,
    IN GT_U32                   sourceId,
    IN GT_PHYSICAL_PORT_NUM     portNum
);

/*******************************************************************************
* cpssDxChBrgSrcIdGroupEntrySet
*
* DESCRIPTION:
*        Set entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       sourceId       - Source ID  number
*       cpuSrcIdMember - GT_TRUE - CPU is member of of Src ID group.
*                        GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr - pointer to the bitmap of ports that are
*                        Source ID Members of specified PP device.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId, ports bitmap value
*       GT_BAD_PTR               - portsMembersPtr is NULL pointer
*       GT_OUT_OF_RANGE          - length of portsMembersPtr is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupEntrySet
(
    IN GT_U8               devNum,
    IN GT_U32              sourceId,
    IN GT_BOOL             cpuSrcIdMember,
    IN CPSS_PORTS_BMP_STC  *portsMembersPtr
);

/*******************************************************************************
* cpssDxChBrgSrcIdGroupEntryGet
*
* DESCRIPTION:
*        Get entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       sourceId          - Source ID number
*
* OUTPUTS:
*       cpuSrcIdMemberPtr - GT_TRUE - CPU is member of of Src ID group.
*                           GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr   - pointer to the bitmap of ports are Source ID Members
*                           of specified PP device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              sourceId,
    OUT GT_BOOL             *cpuSrcIdMemberPtr,
    OUT CPSS_PORTS_BMP_STC  *portsMembersPtr
);


/*******************************************************************************
* cpssDxChBrgSrcIdPortDefaultSrcIdSet
*
* DESCRIPTION:
*       Configure Port's Default Source ID.
*       The Source ID is used for source based egress filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number, CPU port
*       defaultSrcId    - Port's Default Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, defaultSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortDefaultSrcIdSet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_U32       defaultSrcId
);

/*******************************************************************************
* cpssDxChBrgSrcIdPortDefaultSrcIdGet
*
* DESCRIPTION:
*       Get configuration of Port's Default Source ID.
*       The Source ID is used for Source based egress filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number, CPU port
*
* OUTPUTS:
*       defaultSrcIdPtr   - Port's Default Source ID
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
GT_STATUS cpssDxChBrgSrcIdPortDefaultSrcIdGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *defaultSrcIdPtr
);

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
*
* DESCRIPTION:
*     Enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       mode      - the assignment mode of the packet Source ID.
*                   CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E mode is
*                   supported for xCat and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       modePtr      - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
(
    IN  GT_U8                            devNum,
    OUT CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*       mode      - the assignment mode of the packet Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdAssignModeSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*
* OUTPUTS:
*       modePtr   - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdAssignModeGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    OUT CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChBrgSrcIdPortUcastEgressFilterSet
*
* DESCRIPTION:
*     Per Egress Port enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*       enable   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* OUTPUTS:
*       None
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
GT_STATUS cpssDxChBrgSrcIdPortUcastEgressFilterSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChBrgSrcIdPortUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Per Egress Port Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
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
GT_STATUS cpssDxChBrgSrcIdPortUcastEgressFilterGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
);


/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdForceEnableSet
*
* DESCRIPTION:
*         Set Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number, CPU port
*       enable    - enable/disable SourceID force mode
*                     GT_TRUE - enable Source ID force mode
*                     GT_FALSE - disable Source ID force mode
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssDxChBrgSrcIdPortSrcIdForceEnableSet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdForceEnableGet
*
* DESCRIPTION:
*         Get Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number, CPU port
*
* OUTPUTS:
*       enablePtr - (pointer to) Source ID Assignment force mode state
*                     GT_TRUE - Source ID force mode is enabled
*                     GT_FALSE - Source ID force mode is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdForceEnableGet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    OUT GT_BOOL     *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ____cpssDxChBrgSrcId */

