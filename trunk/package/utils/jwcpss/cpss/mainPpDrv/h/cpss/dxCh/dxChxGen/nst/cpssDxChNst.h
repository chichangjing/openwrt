/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNst.h
*
* DESCRIPTION:
*        Network Shield Technology facility Cheetah CPSS declarations.
*
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

#ifndef __cpssDxChNsth
#define __cpssDxChNsth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/nst/cpssNstTypes.h>



/*******************************************************************************
* cpssDxChNstBridgeAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      command        - command assigned to a packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstBridgeAccessMatrixCmdSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    IN  CPSS_PACKET_CMD_ENT         command
);

/*******************************************************************************
* cpssDxChNstBridgeAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstBridgeAccessMatrixCmdGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    OUT CPSS_PACKET_CMD_ENT         *commandPtr
);

/*******************************************************************************
* cpssDxChNstDefaultAccessLevelsSet
*
* DESCRIPTION:
*     Set default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to
*                       configure default access level .
*      accessLevel  -  default security access level for parameter
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,paramType,accessLevel
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstDefaultAccessLevelsSet
(
    IN GT_U8                    devNum,
    IN CPSS_NST_AM_PARAM_ENT    paramType,
    IN GT_U32                   accessLevel
);

/*******************************************************************************
* cpssDxChNstDefaultAccessLevelsGet
*
* DESCRIPTION:
*     Get default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to
*                       configure default access level .
* OUTPUTS:
*      accessLevelPtr - (pointer to) default security access level for parameter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,paramType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstDefaultAccessLevelsGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NST_AM_PARAM_ENT    paramType,
    OUT GT_U32                   *accessLevelPtr
);

/*******************************************************************************
* cpssDxChNstProtSanityCheckSet
*
* DESCRIPTION:
*      Set packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
*      enable    - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,checkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstProtSanityCheckSet
(
    IN GT_U8                 devNum,
    IN CPSS_NST_CHECK_ENT    checkType,
    IN GT_BOOL               enable
);

/*******************************************************************************
* cpssDxChNstProtSanityCheckGet
*
* DESCRIPTION:
*      Get packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
* OUTPUTS:
*      enablePtr - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,checkType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstProtSanityCheckGet
(
    IN  GT_U8                 devNum,
    IN  CPSS_NST_CHECK_ENT    checkType,
    OUT GT_BOOL               *enablePtr
);

/*******************************************************************************
* cpssDxChNstPortIngressFrwFilterSet
*
* DESCRIPTION:
*     Set port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - ingress filter type
*       enable      - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIngressFrwFilterSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    IN GT_BOOL                          enable
);

/*******************************************************************************
* cpssDxChNstPortIngressFrwFilterGet
*
* DESCRIPTION:
*     Get port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - ingress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIngressFrwFilterGet
(
    IN  GT_U8                            devNum,
    IN  GT_PORT_NUM                      portNum,
    IN  CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    OUT GT_BOOL                          *enablePtr
);


/*******************************************************************************
* cpssDxChNstPortEgressFrwFilterSet
*
* DESCRIPTION:
*     Set port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - egress filter type
*       enable      - GT_TRUE - enable egress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restiriction
*                     according to filterType.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortEgressFrwFilterSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    IN GT_BOOL                          enable
);

/*******************************************************************************
* cpssDxChNstPortEgressFrwFilterGet
*
* DESCRIPTION:
*     Get port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - egress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable egress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restiriction
*                     according to filterType.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortEgressFrwFilterGet
(
    IN  GT_U8                            devNum,
    IN  GT_PORT_NUM                      portNum,
    IN  CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    OUT GT_BOOL                          *enablePtr
);

/*******************************************************************************
* cpssDxChNstIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       ingressCnt  - the counter value
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstIngressFrwFilterDropCntrSet
(
    IN GT_U8       devNum,
    IN GT_U32      ingressCnt
);

/*******************************************************************************
* cpssDxChNstIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstIngressFrwFilterDropCntrGet
(
    IN GT_U8       devNum,
    OUT GT_U32     *ingressCntPtr
);

/*******************************************************************************
* cpssDxChNstRouterAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP.
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*      devNum          - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*      command         - command assigned to a packet
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstRouterAccessMatrixCmdSet
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     sipAccessLevel,
    IN  GT_U32                     dipAccessLevel,
    IN  CPSS_PACKET_CMD_ENT        command
);

/*******************************************************************************
* cpssDxChNstRouterAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP.
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*      devNum         - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstRouterAccessMatrixCmdGet
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     sipAccessLevel,
    IN  GT_U32                     dipAccessLevel,
    OUT  CPSS_PACKET_CMD_ENT        *commandPtr
);

/*******************************************************************************
* cpssDxChNstPortGroupIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       ingressCnt      - the counter value
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortGroupIngressFrwFilterDropCntrSet
(
    IN GT_U8                   devNum,
    IN GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN GT_U32                  ingressCnt
);

/*******************************************************************************
* cpssDxChNstPortGroupIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortGroupIngressFrwFilterDropCntrGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *ingressCntPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChNsth */

