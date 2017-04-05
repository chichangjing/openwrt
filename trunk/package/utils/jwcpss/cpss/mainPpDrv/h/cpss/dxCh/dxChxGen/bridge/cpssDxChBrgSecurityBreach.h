/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgSecurityBreach.h
*
* DESCRIPTION:
*       Implementation for CPSS DxCh Bridge Security Breach Functionality.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __cpssDxChBrgSecurityBreachh
#define __cpssDxChBrgSecurityBreachh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgSecurityBreachTypes.h>

/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanDropCntrModeSet
*
* DESCRIPTION:
*      Set Port/VLAN Security Breach Drop Counter to count security breach
*      dropped packets based on there ingress port or their assigned VID.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       cntrCfgPtr          - (pointer to) security breach port/vlan counter
*                             mode and it's configuration parameters.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanDropCntrModeSet
(
    IN GT_U8                                        dev,
    IN CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC     *cntrCfgPtr
);

/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanDropCntrModeGet
*
* DESCRIPTION:
*       Get mode and it's configuration parameters for security breach
*       Port/VLAN drop counter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*
* OUTPUTS:
*       cntrCfgPtr          - (pointer to) security breach port/vlan counter
*                             mode and it's configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanDropCntrModeGet
(
    IN  GT_U8                                        dev,
    OUT CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC     *cntrCfgPtr
);

/*******************************************************************************
* cpssDxChBrgSecurBreachGlobalDropCntrGet
*
* DESCRIPTION:
*       Get values of security breach global drop counter. Global security breach
*       counter counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach global counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachGlobalDropCntrGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *counValuePtr
);

/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanCntrGet
*
* DESCRIPTION:
*       Get value of security breach Port/Vlan drop counter. This counter counts
*       all of the packets received according to Port/Vlan security breach drop
*       count mode configuration and dropped due to any security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach Port/Vlan drop counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanCntrGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *counValuePtr
);

/*******************************************************************************
* cpssDxChBrgSecurBreachNaPerPortSet
*
* DESCRIPTION:
*       Enable/Disable per port the unknown Source Addresses to be considered as
*       security breach event. When enabled, the NA message is not sent to CPU
*       and this address is not learned.
*       Only relevant in controlled learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - physical device number
*       portNum             - port number
*       enable              - GT_TRUE - New Address is security breach event
*                             GT_FALSE - New Address is not security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachNaPerPortSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChBrgSecurBreachNaPerPortGet
*
* DESCRIPTION:
*       Get per port the status of NA security breach event (enabled/disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - physical device number
*       portNum             - port number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE - NA is security breach event
*                             GT_FALSE - NA is not security breach event
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachNaPerPortGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    OUT GT_BOOL             *enablePtr
);

/*******************************************************************************
* cpssDxChBrgSecurBreachMovedStaticAddrSet
*
* DESCRIPTION:
*       When the FDB entry is signed as a static one, then this entry is not
*       subject to aging. In addition, there may be cases where the interface
*       does't match the interface from which this packet was received. In this
*       case, this feature enables/disables to regard moved static adresses as
*       a security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       enable              - GT_TRUE - moved static addresses are regarded as
*                                   security breach event and are dropped.
*                             GT_FALSE - moved static addresses are not regarded
*                                   as security breach event and are forwarded.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachMovedStaticAddrSet
(
    IN GT_U8                dev,
    IN GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChBrgSecurBreachMovedStaticAddrGet
*
* DESCRIPTION:
*       Get if static addresses that are moved, are regarded as Security Breach
*       or not (this is only in case that the fdb entry is static or the
*       interface does't match the interface from which this packet was received).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE - moved static addresses are regarded as
*                                   security breach event and are dropped.
*                             GT_FALSE - moved static addresses are not regarded
*                                   as security breach event and are forwarded.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachMovedStaticAddrGet
(
    IN  GT_U8                dev,
    OUT GT_BOOL              *enablePtr
);


/*******************************************************************************
* cpssDxChBrgSecurBreachEventDropModeSet
*
* DESCRIPTION:
*       Set for given security breach event it's drop mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       eventType           - security breach event type.
*       dropMode            - soft or hard drop mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventDropModeSet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    IN CPSS_DROP_MODE_TYPE_ENT              dropMode
);

/*******************************************************************************
* cpssDxChBrgSecurBreachEventDropModeGet
*
* DESCRIPTION:
*      Get for given security breach event it's drop mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       eventType           - security breach event type.
*
* OUTPUTS:
*       dropModePtr         - (pointer to) drop mode - soft or hard.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventDropModeGet
(
    IN  GT_U8                               dev,
    IN  CPSS_BRG_SECUR_BREACH_EVENTS_ENT    eventType,
    OUT CPSS_DROP_MODE_TYPE_ENT             *dropModePtr
);

/*******************************************************************************
* cpssDxChSecurBreachMsgGet
*
* DESCRIPTION:
*       Reads the Security Breach message update. When a security breach occures
*       and the CPU had read the previous message (the message is locked untill
*       CPU reads it, after it the new message can be written) the security
*       massage parameters are updated. Those parameters include: MAC address of
*       the breaching packet, security breach port, VID and security breach code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*
* OUTPUTS:
*       sbMsgPtr    - (pointer to) security breach message
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChSecurBreachMsgGet
(
    IN  GT_U8                           dev,
    OUT CPSS_BRG_SECUR_BREACH_MSG_STC   *sbMsgPtr
);

/*******************************************************************************
* cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet
*
* DESCRIPTION:
*       Get value of security breach Port/Vlan drop counter. This counter counts
*       all of the packets received according to Port/Vlan security breach drop
*       count mode configuration and dropped due to any security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
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
*       counValuePtr    - (pointer to) secure breach Port/Vlan drop counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counValuePtr
);

/*******************************************************************************
* cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet
*
* DESCRIPTION:
*       Get values of security breach global drop counter. Global security breach
*       counter counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
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
*       counValuePtr    - (pointer to) secure breach global counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counValuePtr
);

/*******************************************************************************
* cpssDxChBrgSecurBreachEventPacketCommandSet
*
* DESCRIPTION:
*       Set for given security breach event it's packet command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - device number
*       eventType           - security breach event type.
*                             The acceptable events are:
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*                               CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE :
*                                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       command             - packet command. (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E;
*                                   CPSS_PACKET_CMD_DROP_SOFT_E)
*                             for Bobcat2; Caelum; Bobcat3 : also (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_FORWARD_E;
*                                   CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
*                                   CPSS_PACKET_CMD_TRAP_TO_CPU_E)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this function similar to cpssDxChBrgSecurBreachEventDropModeSet , but
*       allow additional packet commands.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventPacketCommandSet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    IN CPSS_PACKET_CMD_ENT                  command
);

/*******************************************************************************
* cpssDxChBrgSecurBreachEventPacketCommandGet
*
* DESCRIPTION:
*       Get for given security breach event it's packet command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - device number
*       eventType           - security breach event type.
*                             The acceptable events are:
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*                               CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE :
*                                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
* OUTPUTS:
*       commandPtr           - (pointer to) packet command.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this function similar to cpssDxChBrgSecurBreachEventDropModeGet , but
*       allow additional packet commands.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventPacketCommandGet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgSecurityBreachh */


