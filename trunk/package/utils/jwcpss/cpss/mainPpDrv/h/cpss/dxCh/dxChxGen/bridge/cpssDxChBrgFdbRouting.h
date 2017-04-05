/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgFdbRouting.h
*
* DESCRIPTION:
*       FDB Host Routing CPSS DxCh implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __cpssDxChBrgFdbRoutingh
#define __cpssDxChBrgFdbRoutingh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>

/*******************************************************************************
* cpssDxChBrgFdbRoutingPortIpUcEnableSet
*
* DESCRIPTION:
*   Enable/Disable using of FDB for Destination IP lookup for IP UC routing.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       protocol   - ipv4 or ipv6 or both
*       enable     - GT_TRUE  - enable using of FDB for DIP lookup for IP UC routing,
*                    GT_FALSE - disable using of FDB for DIP lookup for IP UC routing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingPortIpUcEnableSet
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocol,
    IN  GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingPortIpUcEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable state of using of FDB for Destination IP lookup for IP UC routing.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       protocol   - ipv4 or ipv6
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - enable using of FDB for DIP lookup for IP UC routing,
*                    GT_FALSE - disable using of FDB for DIP lookup for IP UC routing
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingPortIpUcEnableGet
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocol,
    OUT GT_BOOL                      *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcRefreshEnableSet
*
* DESCRIPTION:
*   Enable/Disable refresh for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE - Perform refresh for UC Routing entries
*                    GT_FALSE - Do not perform refresh for UC Routing entries
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcRefreshEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcRefreshEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable refresh status for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - Perform refresh for UC Routing entries
*                    GT_FALSE - Do not perform refresh for UC Routing entries
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcRefreshEnableGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcAgingEnableSet
*
* DESCRIPTION:
*   Enable/Disable aging for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE  - UC routing entries are examined for aging and
*                               an AA message is sent out for an entry that is
*                               aged out. Auto delete is not performed
*                    GT_FALSE - UC routing entries are not examined for aging
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcAgingEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcAgingEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable aging status for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - UC routing entries are examined for aging and
*                               an AA message is sent out for an entry that is
*                               aged out. Auto delete is not performed
*                    GT_FALSE - UC routing entries are not examined for aging
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcAgingEnableGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcTransplantEnableSet
*
* DESCRIPTION:
*   Enable/Disable transplanting for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE  - Perform address transplanting for UC Routing entries
*                    GT_FALSE - Do not perform address transplanting for UC Routing entries
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcTransplantEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcTransplantEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable transplanting status for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - Perform address transplanting for UC Routing entries
*                    GT_FALSE - Do not perform address transplanting for UC Routing entries
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcTransplantEnableGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcDeleteEnableSet
*
* DESCRIPTION:
*   Enable/Disable delete for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE  - Perform address deleting for UC Routing entries
*                    GT_FALSE - Do not perform address deleting for UC Routing entries
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcDeleteEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcDeleteEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable delete status for UC Routing entries
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - Perform address deleting for UC Routing entries
*                    GT_FALSE - Do not perform address deleting for UC Routing entries
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcDeleteEnableGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcAAandTAToCpuSet
*
* DESCRIPTION:
*       The device implements a hardware-based aging or transplanting
*       mechanism. When an entry is aged out or is transplanted, a message can
*       be forwarded to the CPU.
*       This field enables/disables forwarding address aged out messages and
*       transplanted address messages to the Host CPU for UC route FDB entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE  - AA and TA messages are not forwarded to the CPU for
*                               UC route entries.
*                    GT_FALSE - AA and TA messages are not forwarded to the CPU for
*                               UC route entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcAAandTAToCpuSet
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingUcAAandTAToCpuGet
*
* DESCRIPTION:
*   The device implements a hardware-based aging or transplanting mechanism
*   When an entry is aged out or is transplanted, a message can be forwarded
*   to the CPU.
*   This field  get the enables/disable status of forwarding address aged out
*   messages and transplanted address messages to the Host CPU for UC route FDB entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - AA and TA messages are not forwarded to the CPU for
*                               UC route entries.
*                    GT_FALSE - AA and TA messages are not forwarded to the CPU for
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingUcAAandTAToCpuGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingNextHopPacketCmdSet
*
* DESCRIPTION:
*       Set Packet Commands for FDB routed packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       nhPacketCmd - Route entry command. supported commands:
*                     CPSS_PACKET_CMD_ROUTE_E,
*                     CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                     CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                     CPSS_PACKET_CMD_DROP_SOFT_E ,
*                     CPSS_PACKET_CMD_DROP_HARD_E,
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingNextHopPacketCmdSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PACKET_CMD_ENT         nhPacketCmd
);

/*******************************************************************************
* cpssDxChBrgFdbRoutingNextHopPacketCmdGet
*
* DESCRIPTION:
*    Get Packet Commands for FDB routed packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       nhPacketCmdPtr - (pointer to) Route entry command. supported commands:
*                       CPSS_PACKET_CMD_ROUTE_E,
*                       CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                       CPSS_PACKET_CMD_DROP_SOFT_E ,
*                       CPSS_PACKET_CMD_DROP_HARD_E,
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_VALUE             - wrong hw value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutingNextHopPacketCmdGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_PACKET_CMD_ENT         *nhPacketCmdPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgFdbRoutingh */

