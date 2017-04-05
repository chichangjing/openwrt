/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgGen.h
*
* DESCRIPTION:
*       CPSS DxCh Bridge Generic APIs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 44 $
*******************************************************************************/
#ifndef __cpssDxChBrgGenh
#define __cpssDxChBrgGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/*
 * typedef: enum CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT
 *
 * Description: Enumeration of Bridge Bypass mode.
 *
 * Enumerations:
 *   CPSS_DXCH_BRG_GEN_BYPASS_MODE_ALL_EXCEPT_SA_LEARNING_E -
 *                      bypass the whole bridge module except the SA learning.
 *   CPSS_DXCH_BRG_GEN_BYPASS_MODE_ONLY_FORWARDING_DECISION_E  -
 *                      bypass only the forwarding decision of the bridge.
 */
typedef enum
{
    CPSS_DXCH_BRG_GEN_BYPASS_MODE_ALL_EXCEPT_SA_LEARNING_E,
    CPSS_DXCH_BRG_GEN_BYPASS_MODE_ONLY_FORWARDING_DECISION_E
} CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT
 *
 * Description: Enumeration of byte-count modes for MTU check feature.
 *
 * Enumerations:
 *   CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_L3_E -
 *                      L3 byte-count is used for MTU check.
 *   CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_L2_E -
 *                      L2 byte-count is used for MTU check.
 */
typedef enum
{
    CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_L3_E,
    CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_L2_E
} CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT
 *
 * Description: Enumeration of IPM classification mode.
 *
 * Enumerations:
 *   CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_MAC_DA_E -
 *                      Packet is considered an IPM packet (IPv6 or IPv4) only
 *                      according to MAC address.
 *   CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_MAC_DA_AND_ETHERTYPE_E -
 *                      Packet is considered an IPM packet (IPv6 or IPv4)
 *                      according MAC address and ethertype.
 */
typedef enum
{
    CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_MAC_DA_E,
    CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_MAC_DA_AND_ETHERTYPE_E
} CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT
 *
 * Description: Enumeration of ARP broadcast command mode.
 *
 * Enumerations:
 *   CPSS_DXCH_ARP_BCAST_CMD_MODE_PORT_E -
 *                      set ARP broadcast command for all ports
 *   CPSS_DXCH_ARP_BCAST_CMD_MODE_VLAN_E -
 *                      set ARP broadcast command for all VLANs
 *
 *
 */
typedef enum
{
    CPSS_DXCH_ARP_BCAST_CMD_MODE_PORT_E,
    CPSS_DXCH_ARP_BCAST_CMD_MODE_VLAN_E
} CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT;

/*
 * Typedef: struct CPSS_DXCH_BRG_RATE_LIMIT_STC
 *
 * Description:
 *      Describe rate limit parameters for DxCh devices.
 *
 * Fields:
 *     dropMode     - drop mode (soft or hard)
 *     rMode        - rate mode (bytes or packets)
 *     win10Mbps    - size of time window for 10Mbps port speed
 *     win100Mbps   - size of time window for 100Mbps port speed
 *     win1000Mbps  - size of time window for 1000Mbps port speed
 *     win10Gbps    - size of time window for 10Gbps port speed
 *
 * Comments:
 *   Window sizes specified in microseconds
 *
 *       1000 Mbps: (APPLICABLE RANGES: 256..16384) uSec    granularity - 256 uSec
 *       100  Mbps: (APPLICABLE RANGES: 256..131072) uSec   granularity - 256 uSec
 *       10   Mbps: (APPLICABLE RANGES: 256..1048570) uSec  granularity - 256 uSec
 *       10   Gbps: (APPLICABLE RANGES: 25.6..104857)  uSec  granularity - 25.6 uSec
 *       For DxCh3 and above: The granularity doesn't depend from PP core clock.
 *       For DxCh1, DxCh2:
 *       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
 *       for they changed linearly with the clock value.
 */
typedef struct
{
    CPSS_DROP_MODE_TYPE_ENT         dropMode;
    CPSS_RATE_LIMIT_MODE_ENT        rMode;
    GT_U32                          win10Mbps;
    GT_U32                          win100Mbps;
    GT_U32                          win1000Mbps;
    GT_U32                          win10Gbps;
} CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC;

/*
 * Typedef: struct CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC
 *
 * Description:
 *      Describe a port rate limit parameters.
 *
 * Fields:
 *     enableBc         - enable Bc packets rate limiting
 *     enableMc         - enable Mc packets rate limiting
 *                        for xCat2, Bobcat2, Caelum, Bobcat3 it controls
 *                        unregistered MC packets only
 *     enableMcReg      - enable Mc registered packets rate limiting
 *                        (APPLICABLE DEVICES: xCat2, Bobcat2, Caelum, Bobcat3)
 *     enableUcUnk      - enable Uc Unknown packets rate limiting
 *     enableUcKnown    - enable Uc Known   packets rate limiting
 *     rateLimit        - rate limit specified in 64 bytes/packets
 *                        depends on ratelimit mode
 *                        (APPLICABLE RANGES: 0..65535)
 *
 * Comments:
 *     None
 */
typedef struct
{
    GT_BOOL   enableBc;
    GT_BOOL   enableMc;
    GT_BOOL   enableMcReg;
    GT_BOOL   enableUcUnk;
    GT_BOOL   enableUcKnown;
    GT_U32    rateLimit;
} CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC;

/*******************************************************************************
* cpssDxChBrgGenIgmpSnoopEnable
*
* DESCRIPTION:
*       Enable/disable trapping all IPv4 IGMP packets to the CPU, based on
*       their Ingress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       status - GT_TRUE for enable and GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIgmpSnoopEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      status
);

/*******************************************************************************
* cpssDxChBrgGenIgmpSnoopEnableGet
*
* DESCRIPTION:
*       Get status of trapping all IPv4 IGMP packets to the CPU, based on
*       their Ingress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       statusPtr - (pointer to ) GT_TRUE for enable and GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIgmpSnoopEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *statusPtr
);

/*******************************************************************************
* cpssDxChBrgGenDropIpMcEnable
*
* DESCRIPTION:
*       Discard all non-Control-classified Ethernet packets
*       with a MAC Multicast DA corresponding to the IP Multicast range,
*       i.e. the MAC range 01-00-5e-00-00-00 to 01-00-5e-7f-ff-ff.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       state   - GT_TRUE, enable filtering mode
*                 GT_FALSE, disable filtering mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropIpMcEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  state
);

/*******************************************************************************
* cpssDxChBrgGenDropIpMcEnableGet
*
* DESCRIPTION:
*       Get status of discarding all non-Control-classified Ethernet packets
*       with a MAC Multicast DA corresponding to the IP Multicast range,
*       i.e. the MAC range 01-00-5e-00-00-00 to 01-00-5e-7f-ff-ff.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       statePtr - GT_TRUE, enable filtering mode
*                  GT_FALSE, disable filtering mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropIpMcEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *statePtr
);

/*******************************************************************************
* cpssDxChBrgGenDropNonIpMcEnable
*
* DESCRIPTION:
*       Discard all non-Control-classified Ethernet
*       packets with a MAC Multicast DA (but not the Broadcast MAC address)
*       not corresponding to the IP Multicast range
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       state   - GT_TRUE, enable filtering mode
*                 GT_FALSE, disable filtering mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropNonIpMcEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  state
);

/*******************************************************************************
* cpssDxChBrgGenDropNonIpMcEnableGet
*
* DESCRIPTION:
*       Get status of discarding all non-Control-classified Ethernet
*       packets with a MAC Multicast DA (but not the Broadcast MAC address)
*       not corresponding to the IP Multicast range
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       statePtr - GT_TRUE, enable filtering mode
*                  GT_FALSE, disable filtering mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropNonIpMcEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *statePtr
);

/*******************************************************************************
* cpssDxChBrgGenDropInvalidSaEnable
*
* DESCRIPTION:
*       Drop all Ethernet packets with MAC SA that are Multicast (MACDA[40] = 1)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE, enable filtering mode
*                GT_FALSE, disable filtering mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropInvalidSaEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgGenDropInvalidSaEnableGet
*
* DESCRIPTION:
*       Get status of droping all Ethernet packets with MAC SA that are
*       Multicast (MACDA[40] = 1)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE, enable filtering mode
*                   GT_FALSE, disable filtering mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropInvalidSaEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenUcLocalSwitchingEnable
*
* DESCRIPTION:
*       Enable/disable local switching back through the ingress interface
*       for for known Unicast packets
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk
*                            are assigned with a soft drop command.
*                 GT_TRUE  = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk may be
*                            forwarded back to their source.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. If the packet is received from a cascade port, the packet’s source
*       location is taken from the FORWARD DSA  tag and not according to the
*       local device and port number.
*       Routed packets are not subject to local switching configuration
*       constraints.
*
*       2. For xCat and above devices to enable  local switching of known
*       Unicast traffic, both ingress port configuration and VLAN entry (by
*       function cpssDxChBrgVlanLocalSwitchingEnableSet) must be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUcLocalSwitchingEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgGenUcLocalSwitchingEnableGet
*
* DESCRIPTION:
*       Get status of local switching back through the ingress interface
*       for for known Unicast packets
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr  - GT_FALSE = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk
*                            are assigned with a soft drop command.
*                    GT_TRUE  = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk may be
*                            forwarded back to their source.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       1. If the packet is received from a cascade port, the packet’s source
*       location is taken from the FORWARD DSA  tag and not according to the
*       local device and port number.
*       Routed packets are not subject to local switching configuration
*       constraints.
*
*       2. For xCat and above devices to enable  local switching of known
*       Unicast traffic, both ingress port configuration and VLAN entry (by
*       function cpssDxChBrgVlanLocalSwitchingEnableSet) must be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUcLocalSwitchingEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenIgmpSnoopModeSet
*
* DESCRIPTION:
*       Set global trap/mirror mode for IGMP snoop on specified device.
*       Relevant when IGMP Trap disabled by cpssDxChBrgGenIgmpSnoopEnable
*       and IGMP Trap/Mirror enabled on a VLAN by function
*       cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       mode   - Trap mode: All IGMP packets are trapped to the CPU,
*                   regardless of their type.
*                IGMP Snoop mode: Query messages are mirrored to the CPU.
*                   Non-Query messages are Trapped to the CPU.
*                IGMP Router mode: All IGMP packets are mirrored to the CPU,
*                    regardless of their type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or IGMP snooping mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIgmpSnoopModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_IGMP_SNOOP_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgGenIgmpSnoopModeGet
*
* DESCRIPTION:
*       Get status of global trap/mirror mode for IGMP snoop on specified device.
*       Relevant when IGMP Trap disabled by cpssDxChBrgGenIgmpSnoopEnable
*       and IGMP Trap/Mirror enabled on a VLAN by function
*       cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       modePtr   - (pointer to)Trap/Mirror mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or IGMP snooping mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - wrong hardware value.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIgmpSnoopModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_IGMP_SNOOP_MODE_ENT     *modePtr
);

/*******************************************************************************
* cpssDxChBrgGenArpBcastToCpuCmdSet
*
* DESCRIPTION:
*       Enables trapping or mirroring to CPU ARP Broadcast packets for all VLANs or ports
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       cmdMode – command interface mode
*                    (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
*       cmd     - supported commands:
*                 CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number, command mode or control packet command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For ARP broadcast received on VLAN - IPv4/IPv6 Control Traffic To CPU must be enabled by function
*           cpssDxChBrgVlanIpCntlToCpuSet
*       For ARP broadcast received on port - ARP Broadcast Trap To CPU must be enabled by function
*           cpssDxChBrgGenArpTrapEnable
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenArpBcastToCpuCmdSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT cmdMode,
    IN CPSS_PACKET_CMD_ENT              cmd
);

/*******************************************************************************
* cpssDxChBrgGenArpBcastToCpuCmdGet
*
* DESCRIPTION:
*       Get status of CPU ARP Broadcast packets for all VLANs or ports
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       cmdMode – command interface mode
*                    (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
*
* OUTPUTS:
*       cmdPtr - (pointer to) command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number, command mode or control packet command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - wrong hardware value.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenArpBcastToCpuCmdGet
(
    IN  GT_U8                           devNum,
    IN CPSS_DXCH_ARP_BCAST_CMD_MODE_ENT cmdMode,
    OUT CPSS_PACKET_CMD_ENT             *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenRipV1MirrorToCpuEnable
*
* DESCRIPTION:
*       Enable/disable mirroring of IPv4 RIPv1 control messages to the CPU
*       for specified device for all VLANs,  with IPv4/IPv6 Control Traffic
*       To CPU Enable set by function cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_FALSE = Forward normally
*                GT_TRUE  = Mirror-to-CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRipV1MirrorToCpuEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgGenRipV1MirrorToCpuEnableGet
*
* DESCRIPTION:
*       Get status of mirroring of IPv4 RIPv1 control messages to the CPU
*       for specified device for all VLANs,  with IPv4/IPv6 Control Traffic
*       To CPU Enable set by function cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_FALSE = Forward normally
*                   GT_TRUE  = Mirror-to-CPU
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRipV1MirrorToCpuEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenIeeeReservedMcastTrapEnable
*
* DESCRIPTION:
*       Enables trapping or mirroring to CPU Multicast packets, with MAC_DA in
*       the IEEE reserved Multicast range (01-80-C2-00-00-xx).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_FALSE -
*                   Trapping or mirroring to CPU of packet with
*                   MAC_DA = 01-80-C2-00-00-xx disabled.
*                GT_TRUE -
*                   Trapping or mirroring to CPU of packet, with
*                   MAC_DA = 01-80-C2-00-00-xx enabled according to the setting
*                   of IEEE Reserved Multicast Configuration Register<n> (0<=n<16)
*                   by cpssDxChBrgGenIeeeReservedMcastProtCmdSet function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIeeeReservedMcastTrapEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgGenIeeeReservedMcastTrapEnableGet
*
* DESCRIPTION:
*       Get status of trapping or mirroring to CPU Multicast packets, with MAC_DA in
*       the IEEE reserved Multicast range (01-80-C2-00-00-xx).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_FALSE -
*                   Trapping or mirroring to CPU of packet with
*                   MAC_DA = 01-80-C2-00-00-xx disabled.
*                   GT_TRUE -
*                   Trapping or mirroring to CPU of packet, with
*                   MAC_DA = 01-80-C2-00-00-xx enabled according to the setting
*                   of IEEE Reserved Multicast Configuration Register<n> (0<=n<16)
*                   by cpssDxChBrgGenIeeeReservedMcastProtCmdSet function
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIeeeReservedMcastTrapEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenIeeeReservedMcastProtCmdSet
*
* DESCRIPTION:
*       Enables forwarding, trapping, or mirroring to the CPU any of the
*       packets with MAC DA in the IEEE reserved, Multicast addresses
*       for bridge control traffic, in the range of 01-80-C2-00-00-xx,
*       where 0<=xx<256
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       profileIndex - profile index (APPLICABLE RANGES: 0..3).
*                      (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
*                      The parameter defines profile (table number)
*                      for the 256 protocols. Profile bound per port by
*                      <cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet>.
*       protocol     - specifies the Multicast protocol
*       cmd          - supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_SOFT_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number, protocol or control packet command
*                      of profileIndex.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh1, DxCh2, DxCh3 devices:
*       BPDU packets with MAC DA = 01-80-C2-00-00-00 are not affect
*       by this mechanism. BPDUs are implicitly trapped to the CPU
*       if the ingress port span tree is not disabled.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIeeeReservedMcastProtCmdSet
(
    IN GT_U8                devNum,
    IN GT_U32               profileIndex,
    IN GT_U8                protocol,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgGenIeeeReservedMcastProtCmdGet
*
* DESCRIPTION:
*       Gets command (forwarding, trapping, or mirroring to the CPU) any of the
*       packets with MAC DA in the IEEE reserved, Multicast addresses
*       for bridge control traffic, in the range of 01-80-C2-00-00-xx,
*       where 0<=xx<256
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       profileIndex - profile index (APPLICABLE RANGES: 0..3).
*                      (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
*                      The parameter defines profile (table number)
*                      for the 256 protocols. Profile bound per port by
*                      <cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet>.
*       protocol     - specifies the Multicast protocol
*
* OUTPUTS:
*       cmdPtr       - (pointer to) command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number, protocol or control packet command
*                      of profileIndex.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIeeeReservedMcastProtCmdGet
(
    IN GT_U8                devNum,
    IN GT_U32               profileIndex,
    IN GT_U8                protocol,
    OUT CPSS_PACKET_CMD_ENT *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenCiscoL2ProtCmdSet
*
* DESCRIPTION:
*       Enables trapping or mirroring to the CPU packets,
        with MAC DA = 0x01-00-0C-xx-xx-xx running CISCO Layer 2 proprietary
        protocols for specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*      cmd     - supported commands:
*                 CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or control packet command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenCiscoL2ProtCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgGenCiscoL2ProtCmdGet
*
* DESCRIPTION:
*       Get status (trapping or mirroring) of the CPU packets,
        with MAC DA = 0x01-00-0C-xx-xx-xx running CISCO Layer 2 proprietary
        protocols for specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       cmdPtr  - (pointer to) command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or control packet command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - wrong hardware value.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenCiscoL2ProtCmdGet
(
    IN  GT_U8                devNum,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenIpV6IcmpTrapEnable
*
* DESCRIPTION:
*     Enable/disable IpV6 ICMP trapping per port for specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*        devNum    - device number
*        portNum   - port number
*        enable - GT_FALSE -
*                   IPv6 ICMP packets received on this port are not trapped
*                   to the CPU. These packets may be trapped or mirrored
*                   to the CPU based on the VID assigned to the packet
*                   see cpssDxChBrgVlanIpV6IcmpToCpuEnable function
*                 GT_TRUE -
*                   IPv6 ICMP packets with a ICMP type that matches one of the
*                   ICMP types configured in the IPv6 ICMP Message Type
*                   Configuration Register<n> by function
*                   cpssDxChBrgGenIcmpv6MsgTypeSet are trapped to the CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6IcmpTrapEnable
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgGenIpV6IcmpTrapEnableGet
*
* DESCRIPTION:
*     Gets status of IpV6 ICMP trapping per port for specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*        devNum    - device number
*        portNum   - port number
*
* OUTPUTS:
*        enablePtr - (pointer to)
*                   GT_FALSE -
*                   IPv6 ICMP packets received on this port are not trapped
*                   to the CPU. These packets may be trapped or mirrored
*                   to the CPU based on the VID assigned to the packet
*                   see cpssDxChBrgVlanIpV6IcmpToCpuEnable function
*                 GT_TRUE -
*                   IPv6 ICMP packets with a ICMP type that matches one of the
*                   ICMP types configured in the IPv6 ICMP Message Type
*                   Configuration Register<n> by function
*                   cpssDxChBrgGenIcmpv6MsgTypeSet are trapped to the CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6IcmpTrapEnableGet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenIcmpv6MsgTypeSet
*
* DESCRIPTION:
*       Set trap/mirror/forward command for specified ICMP message type.
*       To enable ICMPv6 trapping/mirroring on a VLAN interface,
*       set the <IPv6ICMP ToCPUEn> bit in the VLAN<n> Entry by function
*       cpssDxChBrgVlanIpV6IcmpToCpuEnable
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - device number
*       index   - the index of the message type(up to eight IPv6 ICMP types)
*                 (APPLICABLE RANGES: 0..7)
*       msgType - ICMP message type
*       cmd     - supported commands:
*                 CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or control packet command
*                      or index > 7
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIcmpv6MsgTypeSet
(
    IN GT_U8                devNum,
    IN GT_U32               index,
    IN GT_U8                msgType,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgGenIcmpv6MsgTypeGet
*
* DESCRIPTION:
*       Get status of trap/mirror/forward command for specified ICMP message type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - device number
*       index   - the index of the message type(up to eight IPv6 ICMP types)
*                 (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       msgTypePtr - ICMP message type
*       cmdPtr  - (pointer to) command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or control packet command
*                      or index > 7
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - wrong hardware value.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIcmpv6MsgTypeGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               index,
    OUT GT_U8                *msgTypePtr,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenIpLinkLocalMirrorToCpuEnable
*
* DESCRIPTION:
*       Enable/disable mirroring to CPU for IP Link Local Control protocol.
*       IPv4 and IPv6 define link-local Multicast addresses that are used
*       by various protocols.
*       In IPv4, the link-local IANA Multicast range is 224.0.0.0/24.
*       In IPv6, the link-local IANA Multicast range is FF02::/16.
*       Relevant only if IP Control to CPU enabled by
*       cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum         - device number
*        protocolStack  - IPv4 or IPv6 protocol version
*        enable         - enable/disable mirroring to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number or IP protocol version
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpLinkLocalMirrorToCpuEnable
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgGenIpLinkLocalMirrorToCpuEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of mirroring to CPU
*       for IP Link Local Control protocol.
*       IPv4 and IPv6 define link-local Multicast addresses that are used
*       by various protocols.
*       In IPv4, the link-local IANA Multicast range is 224.0.0.0/24.
*       In IPv6, the link-local IANA Multicast range is FF02::/16.
*       Relevant only if IP Control to CPU enabled by
*       cpssDxChBrgVlanIpCntlToCpuSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum         - device number
*        protocolStack  - IPv4 or IPv6 protocol version
*
* OUTPUTS:
*        enablePtr      - pointer to enable/disable status of mirroring to CPU.
*                         GT_TRUE  - mirroring to CPU for IP Link Local
*                                    Control protocol is enabled.
*                         GT_FALSE - mirroring to CPU for IP Link Local
*                                    Control protocol is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number or IP protocol version
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpLinkLocalMirrorToCpuEnableGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    OUT GT_BOOL                      *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable
*
* DESCRIPTION:
*       Enable/disable mirroring to CPU for IP Link Local Control protocol.
*       IPv4 Multicast addresses in the range 224.0.0.x and IPv6 Multicast
*       addresses in the range FF:02::/16 are reserved by IANA
*       for link-local control protocols.
*       If the VLAN is enabled for IP Control to CPU by
*       cpssDxChBrgVlanIpCntlToCpuSet and IPv4/6 Link Local Mirror enabled
*       by cpssDxChBrgGenIpLinkLocalMirrorToCpuEnable,
*       any of the IPv4/6 Multicast packets with a DIP in this range
*       may be mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       protocolStack - IPv4 or IPv6 protocol version
*       protocol    - link local IP protocol
*       enable      - enable/disable mirroring to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number, IP protocol version or
                          Link Local IP protocol
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnable
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_U8                        protocol,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnableGet
*
* DESCRIPTION:
*       Get status of mirroring to CPU for IP Link Local Control protocol.
*       IPv4 Multicast addresses in the range 224.0.0.x and IPv6 Multicast
*       addresses in the range FF:02::/16 are reserved by IANA
*       for link-local control protocols.
*       If the VLAN is enabled for IP Control to CPU by
*       cpssDxChBrgVlanIpCntlToCpuSet and IPv4/6 Link Local Mirror enabled
*       by cpssDxChBrgGenIpLinkLocalMirrorToCpuEnable,
*       any of the IPv4/6 Multicast packets with a DIP in this range
*       may be mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       protocolStack - IPv4 or IPv6 protocol version
*       protocol    - link local IP protocol
*
* OUTPUTS:
*       enablePtr   - enable/disable mirroring to CPU
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number, IP protocol version or
                          Link Local IP protocol
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpLinkLocalProtMirrorToCpuEnableGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN  GT_U8                        protocol,
    OUT GT_BOOL                      *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenIpV6SolicitedCmdSet
*
* DESCRIPTION:
*       Set trap/mirror command for IPv6 Neighbor Solicitation Protocol.
*       If the VLAN entry, indexed by the VID assigned to the packet,
*       IP Control to CPU enabled by cpssDxChBrgVlanIpCntlToCpuSet,
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored
*       to the CPU, if the packets DIP matches the configured prefix
*       and according to the setting of IPv6 Neighbor Solicited Node Command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       cmd     - supported commands:   CPSS_PACKET_CMD_FORWARD_E,
*                                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                                       CPSS_PACKET_CMD_TRAP_TO_CPU_E.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM    - on wrong device number or packet control command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6SolicitedCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgGenIpV6SolicitedCmdGet
*
* DESCRIPTION:
*       Get status of trap/mirror command for IPv6 Neighbor Solicitation Protocol.
*       If the VLAN entry, indexed by the VID assigned to the packet,
*       IP Control to CPU enabled by cpssDxChBrgVlanIpCntlToCpuSet,
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored
*       to the CPU, if the packets DIP matches the configured prefix
*       and according to the setting of IPv6 Neighbor Solicited Node Command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       cmdPtr  - (pointer to) command
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM    - on wrong device number or packet control command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - wrong hardware value.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6SolicitedCmdGet
(
    IN  GT_U8                devNum,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixSet
*
* DESCRIPTION:
*       Set IPv6 Solicited-Node Multicast Address Prefix Configuration and Mask
*       If the VLAN entry, indexed by the VID assigned to the packet,
*       IP Control to CPU enabled by cpssDxChBrgVlanIpCntlToCpuSet,
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored
*       to the CPU, if the packets DIP matches the configured prefix
*       and according to the setting of IPv6 Neighbor Solicited Node Command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       addressPtr - (pointer to)IP address
*       maskPtr - (pointer to) mask of the address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixSet
(
    IN GT_U8        devNum,
    IN GT_IPV6ADDR  *addressPtr,
    IN GT_IPV6ADDR  *maskPtr
);

/*******************************************************************************
* cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixGet
*
* DESCRIPTION:
*       Get IPv6 Solicited-Node Multicast Address Prefix Configuration and Mask
*       If the VLAN entry, indexed by the VID assigned to the packet,
*       IP Control to CPU enabled by cpssDxChBrgVlanIpCntlToCpuSet,
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored
*       to the CPU, if the packets DIP matches the configured prefix
*       and according to the setting of IPv6 Neighbor Solicited Node Command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       addressPtr - (pointer to)IP address
*       maskPtr - (pointer to) mask of the address
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpV6SolicitedNodeMcastPrefixGet
(
    IN  GT_U8        devNum,
    OUT GT_IPV6ADDR  *addressPtr,
    OUT GT_IPV6ADDR  *maskPtr
);

/*******************************************************************************
* cpssDxChBrgGenRateLimitGlobalCfgSet
*
* DESCRIPTION:
*       Configures global ingress rate limit parameters - rate limiting mode,
*       Four configurable global time window periods are assigned to ports as
*       a function of their operating speed:
*       10 Gbps, 1 Gbps, 100 Mbps, and 10 Mbps.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2.
*
* INPUTS:
*       devNum             - device number
*       brgRateLimitPtr - pointer to global rate limit parameters structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   Supported windows for DxCh devices:
*       1000 Mbps: range - 256-16384 uSec    granularity - 256 uSec
*       100  Mbps: range - 256-131072 uSec   granularity - 256 uSec
*       10   Mbps: range - 256-1048570 uSec  granularity - 256 uSec
*       10   Gbps: range - 25.6-104857 uSec  granularity - 25.6 uSec
*       For DxCh3 and above: The granularity doesn't depend from PP core clock.
*       For DxCh1, DxCh2:
*       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
*       for they changed linearly with the clock value.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitGlobalCfgSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC     *brgRateLimitPtr
);

/*******************************************************************************
* cpssDxChBrgGenRateLimitGlobalCfgGet
*
* DESCRIPTION:
*     Get global ingress rate limit parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       brgRateLimitPtr - pointer to global rate limit parameters structure
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 and above: The granularity doesn't depend from PP core clock.
*       For DxCh1, DxCh2:
*       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
*       for they changed linearly with the clock value.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitGlobalCfgGet
(
    IN GT_U8                                devNum,
    OUT CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC    *brgRateLimitPtr
);

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitSet
*
* DESCRIPTION:
*       Configures port ingress rate limit parameters
*       Each port maintains rate limits for unknown unicast packets,
*       known unicast packets, multicast packets and broadcast packets,
*       single configurable limit threshold value, and a single internal counter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2.
*
* INPUTS:
*       devNum         - device number
*       port        - port number
*       portGfgPtr  - pointer to rate limit configuration for a port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device number or port
*       GT_OUT_OF_RANGE          - on out of range rate limit values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitSet
(
    IN GT_U8                                    devNum,
    IN GT_PHYSICAL_PORT_NUM                     port,
    IN CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC    *portGfgPtr
);

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitGet
*
* DESCRIPTION:
*     Get port ingress rate limit parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2.
*
* INPUTS:
*       devNum         - device number
*       port        - port number
*
* OUTPUTS:
*       portGfgPtr  - pointer to rate limit configuration for a port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitGet
(
    IN GT_U8                                    devNum,
    IN GT_PHYSICAL_PORT_NUM                     port,
    OUT CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC   *portGfgPtr
);

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitTcpSynSet
*
* DESCRIPTION:
*     Enable or disable TCP SYN packets' rate limit for a port.
*     Value of rate limit is configured by cpssDxChBrgGenPortRateLimitSet and
*     cpssDxChBrgGenRateLimitGlobalCfgSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion; Lion2.
*
* INPUTS:
*       devNum         - device number
*       port        - port number
*       enable      - enable/disable TCP SYN packets rate limit.
*                     GT_TRUE - enable rate limit, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitTcpSynSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     port,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitTcpSynGet
*
* DESCRIPTION:
*     Get the status of TCP SYN packets' rate limit for a port (enable/disable).
*     Value of rate limit can be read by cpssDxChBrgGenPortRateLimitGet and
*     cpssDxChBrgGenRateLimitGlobalCfgGet.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion; Lion2.
*
* INPUTS:
*       devNum      - device number
*       port        - port number
*
* OUTPUTS:
*       enablePtr    - pointer to TCP SYN packets rate limit status.
*                      GT_TRUE - enable rate limit, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitTcpSynGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    port,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenUdpBcDestPortCfgSet
*
* DESCRIPTION:
*     Configure UDP Broadcast Destination Port configuration table.
*     It's possible to configure several UDP destination ports with their Trap
*     or Mirror to CPU command and CPU code. See datasheet of device for maximal
*     number of UDP destination ports, which may be configured. The feature may
*     be enabled or disabled per VLAN by cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
*
* INPUTS:
*       devNum      - device number
*       entryIndex  - index in the global UDP Broadcast Port Table, allows to
*                     application to configure up to 12 UDP ports
*                     (APPLICABLE RANGES: 0..11)
*       udpPortNum  - UDP destination port number
*       cpuCode     - CPU code for packets trapped or mirrored by the feature.
*                     Acceptable CPU codes are: CPSS_NET_UDP_BC_MIRROR_TRAP0_E,
*                     CPSS_NET_UDP_BC_MIRROR_TRAP1_E, CPSS_NET_UDP_BC_MIRROR_TRAP2_E,
*                     CPSS_NET_UDP_BC_MIRROR_TRAP3_E
*       cmd         - packet command for UDP Broadcast packets
*                     either CPSS_DXCH_BRG_GEN_CNTRL_TRAP_E
*                     or CPSS_DXCH_BRG_GEN_CNTRL_MIRROR_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device or CPU code or cmd or
*                                  entryIndex > 11
*       GT_HW_ERROR              - failed to write to hw.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUdpBcDestPortCfgSet
(
   IN GT_U8                     devNum,
   IN GT_U32                    entryIndex,
   IN GT_U16                    udpPortNum,
   IN CPSS_NET_RX_CPU_CODE_ENT  cpuCode,
   IN CPSS_PACKET_CMD_ENT       cmd
);

/*******************************************************************************
* cpssDxChBrgGenUdpBcDestPortCfgGet
*
* DESCRIPTION:
*     Get UDP Broadcast Destination Port configuration table.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - index in the global UDP Broadcast Port Table, allows to
*                        application to configure up to 12 UDP ports
*                        (APPLICABLE RANGES: 0..11)
*
* OUTPUTS:
*       validPtr        - (pointer to) entry validity:
*                         GT_TRUE  - valid entry,
*                         GT_FALSE - invalid entry.
*       udpPortNumPtr   - (pointer to) UDP destination port number
*       cpuCodePtr      - (pointer to) CPU code for packets trapped or mirrored
*                         by the feature. Acceptable CPU codes are:
*                         CPSS_NET_UDP_BC_MIRROR_TRAP0_E,
*                         CPSS_NET_UDP_BC_MIRROR_TRAP1_E,
*                         CPSS_NET_UDP_BC_MIRROR_TRAP2_E,
*                         CPSS_NET_UDP_BC_MIRROR_TRAP3_E
*       cmdPtr          - (pointer to) packet command for UDP Broadcast packets
*                         either CPSS_DXCH_BRG_GEN_CNTRL_TRAP_E
*                         or CPSS_DXCH_BRG_GEN_CNTRL_MIRROR_E
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error
*       GT_BAD_PARAM                - on wrong device or entryIndex > 11
*       GT_HW_ERROR                 - failed to read from hw.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUdpBcDestPortCfgGet
(
   IN GT_U8                     devNum,
   IN GT_U32                    entryIndex,
   OUT GT_BOOL                  *validPtr,
   OUT GT_U16                   *udpPortNumPtr,
   OUT CPSS_NET_RX_CPU_CODE_ENT *cpuCodePtr,
   OUT CPSS_PACKET_CMD_ENT      *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenUdpBcDestPortCfgInvalidate
*
* DESCRIPTION:
*       Invalidate UDP Broadcast Destination Port configuration table.
*       All fields in entry will be reset.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
*
* INPUTS:
*       devNum        - device number
*       entryIndex - index in the global UDP Broadcast Port Table, allows to
*                 application to configure up to 12 UDP ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device or intryIndex > 11
*       GT_HW_ERROR              - failed to write to hw.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenUdpBcDestPortCfgInvalidate
(
   IN GT_U8                                     devNum,
   IN GT_U32                                    entryIndex
);

/*******************************************************************************
* cpssDxChBrgGenBpduTrapEnableSet
*
* DESCRIPTION:
*     Enable or disable trapping of BPDU Multicast MAC address 01-80-C2-00-00-00
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE - trap packets to CPU with BPDU Multicast MAC address
*                01-80-C2-00-00-00 on a ingress port whose span state
*                is not disabled.
*                GT_FALSE - packets with BPDU Multicast MAC address
*                01-80-C2-00-00-00 are not affected by ingress port's span state
*                and forwarded as standard Multicast frames
* OUTPUTS:
*       None
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
GT_STATUS cpssDxChBrgGenBpduTrapEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgGenBpduTrapEnableGet
*
* DESCRIPTION:
*     Get trapping status of BPDU Multicast MAC address 01-80-C2-00-00-00
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE - trap packets to CPU with BPDU Multicast MAC address
*                   01-80-C2-00-00-00 on a ingress port whose span state
*                   is not disabled.
*                   GT_FALSE - packets with BPDU Multicast MAC address
*                   01-80-C2-00-00-00 are not affected by ingress port's span state
*                   and forwarded as standard Multicast frames
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenBpduTrapEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenArpTrapEnable
*
* DESCRIPTION:
*       Enable/disable trapping ARP Broadcast packets to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       enable - GT_TRUE - ARP Broadcast packets received on this port
*                        are trapped to the CPU.
*                GT_FALSE - ARP Broadcast packets received on this port
*                        are not trapped to the CPU.
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
GT_STATUS cpssDxChBrgGenArpTrapEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgGenArpTrapEnableGet
*
* DESCRIPTION:
*       Get trapping ARP Broadcast packets to the CPU status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       enablePtr -  pointer to trapping ARP Broadcast packets status
*                GT_TRUE - ARP Broadcast packets received on this port
*                          are trapped to the CPU.
*                GT_FALSE - ARP Broadcast packets received on this port
*                        are not trapped to the CPU.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenArpTrapEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenDropIpMcModeSet
*
* DESCRIPTION:
*       Set drop mode for IP Multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       mode         - drop mode
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when Drop IP multicast is enabled.
*       See cpssDxChBrgGenDropIpMcEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropIpMcModeSet
(
    IN GT_U8                    devNum,
    IN CPSS_DROP_MODE_TYPE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgGenDropIpMcModeGet
*
* DESCRIPTION:
*       Get drop mode for IP Multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*
*
* OUTPUTS:
*       modePtr       - pointer to drop mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when Drop IP multicast is enabled.
*       See cpssDxChBrgGenDropIpMcEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropIpMcModeGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_DROP_MODE_TYPE_ENT  *modePtr
);



/*******************************************************************************
* cpssDxChBrgGenDropNonIpMcModeSet
*
* DESCRIPTION:
*       Set drop mode for non-IP Multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       mode      - drop mode
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when Drop non-IP multicast is enabled.
*       See cpssDxChBrgGenDropNonIpMcEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropNonIpMcModeSet
(
    IN GT_U8                    devNum,
    IN CPSS_DROP_MODE_TYPE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgGenDropNonIpMcModeGet
*
* DESCRIPTION:
*       Get drop mode for non-IP Multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
*
* OUTPUTS:
*       modePtr    - pointer to drop mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when Drop non-IP multicast is enabled.
*       See cpssDxChBrgGenDropNonIpMcEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenDropNonIpMcModeGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_DROP_MODE_TYPE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChBrgGenCfiRelayEnableSet
*
* DESCRIPTION:
*       Enable / Disable CFI bit relay.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE -  Tagged packets transmitted from the device have
*                            have their CFI bit set according to received
*                            packet CFI bit if the received packet was tagged
*                            or set to 0 if the received packet is untagged.
*               - GT_FALSE - Tagged packets transmitted from the device
*                            have their CFI bit set to 0.
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenCfiRelayEnableSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChBrgGenCfiRelayEnableGet
*
* DESCRIPTION:
*       Get CFI bit relay status.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  -  pointer to CFI bit relay status.
*                  - GT_TRUE -  Tagged packets transmitted from the device have
*                               have their CFI bit set according to received
*                               packet CFI bit if the received packet was tagged
*                               or set to 0 if the received packet is untagged.
*                  - GT_FALSE - Tagged packets transmitted from the device
*                               have their CFI bit set to 0.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenCfiRelayEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet
*
* DESCRIPTION:
*       Select the IEEE Reserved Multicast profile (table) associated with port
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*       portNum      - port number (including CPU port)
*       profileIndex - profile index. The parameter defines profile (table
*                      number) for the 256 protocols (APPLICABLE RANGES: 0..3).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_OUT_OF_RANGE          - for profileIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_U32               profileIndex
);

/*******************************************************************************
* cpssDxChBrgGenPortIeeeReservedMcastProfileIndexGet
*
* DESCRIPTION:
*       Get the IEEE Reserved Multicast profile (table) associated with port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum             - device number
*       portNum            - port number (including CPU port)
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index. The parameter
*                         defines profile (table number) for the 256 protocols.
*                         (APPLICABLE RANGES: 0..3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or profileId
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIeeeReservedMcastProfileIndexGet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    OUT GT_U32              *profileIndexPtr
);

/*******************************************************************************
* cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableSet
*
* DESCRIPTION:
*     Enable/disable dropping all ingress untagged packets received
*     by specified port with byte count more then 1518 bytes.
*     This feature is additional to cpssDxChPortMruSet that sets
*     MRU at the MAC level and is not aware of packet tagging.
*     If MAC level MRU is 1522, untagged packets with size 1519..1522 bytes
*     are not dropped at the MAC level as needed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*        devNum    - device number
*        portNum   - port number (including CPU port)
*                    In eArch devices portNum is default ePort.
*        enable - GT_FALSE -
*                   untagged packets with size more then 1518 bytes not dropped
*                 GT_TRUE -
*                   untagged packets with size more then 1518 bytes dropped
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableGet
*
* DESCRIPTION:
*     Get Enable/disable status of dropping all ingress untagged packets received
*     by specified port with byte count more then 1518 bytes.
*     This feature is additional to cpssDxChPortMruSet that sets
*     MRU at the MAC level and is not aware of packet tagging.
*     If MAC level MRU is 1522, untagged packets with size 1519..1522 bytes
*     are not dropped at the MAC level as needed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*        devNum    - device number
*        portNum   - port number (including CPU port)
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*        enablePtr - (pointer to)
*                   GT_FALSE -
*                      untagged packets with size more then 1518 bytes not dropped
*                   GT_TRUE -
*                      untagged packets with size more then 1518 bytes dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableGet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenPortOversizeUntaggedPacketsFilterCmdSet
*
* DESCRIPTION:
*     Set the oversize packet command for untagged packets with length bigger
*     than 1518 bytes
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       command    - the command. valid values:
*                       CPSS_PACKET_CMD_FORWARD_E
*                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                       CPSS_PACKET_CMD_DROP_HARD_E
*                       CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable/disable filtering of untagged packets with length bigger than
*       1518 bytes on ePort, use cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortOversizeUntaggedPacketsFilterCmdSet
(
    IN GT_U8                    devNum,
    IN CPSS_PACKET_CMD_ENT      command
);

/*******************************************************************************
* cpssDxChBrgGenPortOversizeUntaggedPacketsFilterCmdGet
*
* DESCRIPTION:
*     Get the oversize packet command for untagged packets with length bigger
*     than 1518 bytes
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
*       commandPtr - (pointer to) the command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To get the filtering status on ePort of untagged packets with length bigger
*       than 1518 bytes, use cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableGet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortOversizeUntaggedPacketsFilterCmdGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_PACKET_CMD_ENT     *commandPtr
);

/*******************************************************************************
* cpssDxChBrgGenRateLimitSpeedCfgEnableSet
*
* DESCRIPTION:
*     Enable/Disable the port speed configuration that will be used by the
*     Bridge ingress rate limiter, regardless of the actual speed in which
*     a port actually works.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*        devNum    - device number
*        enable - GT_FALSE -  Port speed for bridge rate limit
*                             calculations is according to the actual
*                             port speed.
*                  GT_TRUE - Port speed for bridge rate limit
*                            calculations is taken from a per-port configuration
*                            cpssDxChBrgGenPortRateLimitSpeedGranularitySet()
*                            and NOT from the port actual speed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat & xCat3 devices the feature is not relevant for Flex link ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitSpeedCfgEnableSet
(
    IN GT_U8      devNum,
    IN GT_BOOL    enable
);

/*******************************************************************************
* cpssDxChBrgGenRateLimitSpeedCfgEnableGet
*
* DESCRIPTION:
*     Get the status of the port speed configuration (Enabled/Disabled) that
*     will be used by the Bridge ingress rate limiter, regardless of the actual
*     speed in which a port actually works.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*        devNum    - device number
*
* OUTPUTS:
*        enablePtr - GT_FALSE -  Port speed for bridge rate limit
*                                calculations is according to the actual
*                                port speed.
*                    GT_TRUE -   Port speed for bridge rate limit
*                                calculations is taken from a per-port configuration
*                                cpssDxChBrgGenPortRateLimitSpeedGranularitySet()
*                                and NOT from the port actual speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat & xCat3 devices the feature is not relevant for Flex link ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitSpeedCfgEnableGet
(
    IN  GT_U8      devNum,
    OUT GT_BOOL    *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitSpeedGranularitySet
*
* DESCRIPTION:
*     Configure 100Mbps/10Mbps/1Gbps/10Gbps port speed for the use of the bridge
*     ingress rate limit calculations.
*     If port speed for Bridge ingress rate limit calculations is enabled,
*     cpssDxChBrgGenRateLimitSpeedCfgEnableSet() API, then this configuration
*     is used.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*        devNum             - device number
*        portNum            - port number
*                             (APPLICABLE RANGES: xCat, xCat3 0..23)
*        speedGranularity   - Port speed granularity
*                            (10Mbps/100Mbps/1Gbps/10Gbps) that
*                             will be used for bridge ingress rate limit
*                             calculations.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portNum or speedGranularity
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat & xCat3 devices the feature is not relevant for Flex link ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitSpeedGranularitySet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_SPEED_ENT      speedGranularity
);

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitSpeedGranularityGet
*
* DESCRIPTION:
*     Get port speed configuration. This configuration is used for bridge
*     ingress rate limit calculations.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*        devNum             - device number
*        portNum            - port number
*                             (APPLICABLE RANGES: xCat, xCat3 0..23)
*
* OUTPUTS:
*        speedGranularityPtr  - (pointer to) port speed granularity
*                               (10Mbps/100Mbps/1Gbps/10Gbps) that will be
*                               used for bridge ingress rate limit calculations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat & xCat3 devices the feature is not relevant for Flex link ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitSpeedGranularityGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_PORT_SPEED_ENT         *speedGranularityPtr
);

/*******************************************************************************
* cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableSet
*
* DESCRIPTION:
*     Enable / Disable excluding unregistered IPv4 / IPv6 Link Local Multicast
*     packets from the per VLAN Unregistered IPv4 / IPv6 Multicast command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*        devNum         - device number
*        protocolStack  - IPv4 or IPv6 protocol version
*        enable         - GT_TRUE -
*                               exclude unregistered IPv4 / IPv6
*                               Link Local Multicast packets from the per VLAN
*                               Unregistered IPv4 / IPv6 Multicast command
*                        - GT_FALSE -
*                               Unregistered link Local Multicast packets
*                               are handled according to the
*                               Unregistered IPv4 / IPv6 Multicast command.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, protocolStack
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableGet
*
* DESCRIPTION:
*     Get enable / disable status for excluding unregistered IPv4 / IPv6
*     Link Local Multicast packets from the per VLAN
*     Unregistered IPv4 / IPv6 Multicast command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*        devNum          - device number
*        protocolStack   - IPv4 or IPv6 protocol version
*
* OUTPUTS:
*        enablePtr       - pointer to status for excluding unregistered
*                          IPv4 / IPv6 Link Local Multicast packets from the
*                          per VLAN Unregistered IPv4 / IPv6 Multicast command.
*                        - GT_TRUE -
*                               exclude unregistered IPv4 / IPv6
*                               Link Local Multicast packets from the per VLAN
*                               Unregistered IPv4 / IPv6 Multicast command
*                        - GT_FALSE -
*                               Unregistered link Local Multicast packets
*                               are handled according to the
*                               Unregistered IPv4 / IPv6 Multicast command.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, protocolStack
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenExcludeLinkLocalMcFromUnregMcFilterEnableGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    OUT GT_BOOL                      *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenPortIeeeReservedMcastLearningEnableSet
*
* DESCRIPTION:
*       Enable/Disable MAC SA learning on reserved IEEE MC packets, that are
*       trapped to CPU.
*
* APPLICABLE DEVICES:
*       xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number
*       enable         - GT_TRUE - learning is performed also for IEEE MC
*                               trapped packets.
*                      - GT_FALSE - No learning is performed for IEEE MC
*                               trapped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIeeeReservedMcastLearningEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgGenPortIeeeReservedMcastLearningEnableGet
*
* DESCRIPTION:
*       Get status of MAC SA learning on reserved IEEE MC packets, that are
*       trapped to CPU (enabled/disabled).
*
* APPLICABLE DEVICES:
*       xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number
*
* OUTPUTS:
*       enablePtr      pointer to status of MAC SA learning on reserved IEEE
*                      MC packets trapped packets:
*                      - GT_TRUE - learning is performed also for IEEE MC
*                                  trapped packets.
*                      - GT_FALSE - No learning is performed for IEEE MC
*                                  trapped packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIeeeReservedMcastLearningEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenPortArpMacSaMismatchDropEnable
*
* DESCRIPTION:
*       Enable/disable configuration for ARP MAC SA check per port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE - if ARP MAC SA mismatch check is performed and it
*                               fails packet is dropped.
*                     GT_FALSE - if ARP MAC SA mismatch check is performed and it
*                               fails packet is not dropped.
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
*       The global ARP MAC SA configuration must remain. When both global
*       configuration is enabled and per port configuration is enabled the
*       ARP MAC SA mismatch check is performed and if fails packet is dropped.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortArpMacSaMismatchDropEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgGenPortArpMacSaMismatchDropEnableGet
*
* DESCRIPTION:
*       Get configuration for ARP MAC SA check per port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_TRUE - if ARP MAC SA mismatch check is performed and it
*                             fails packet is dropped.
*                   GT_FALSE - if ARP MAC SA mismatch check is performed and it
*                             fails packet is not dropped.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortArpMacSaMismatchDropEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandSet
*
* DESCRIPTION:
*       Set Unknown UC DA filter command per port
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
*       cmd     - Unknown DA command
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandGet
*
* DESCRIPTION:
*       Get Unknown UC DA filter command per port
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
*
* OUTPUTS:
*       cmdPtr - (pointer to) Unknown DA command
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIngressPortUnknownUcFilterDaCommandGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT CPSS_PACKET_CMD_ENT     *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenIngressPortUnregisteredMcFilterDaCommandSet
*
* DESCRIPTION:
*       Set Unregistered MC DA filter command per port
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
*       cmd     - Unregistered MC command
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIngressPortUnregisteredMcFilterDaCommandSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgGenIngressPortUnregisteredMcFilterDaCommandGet
*
* DESCRIPTION:
*       Get Unregistered MC DA filter command per port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       cmdPtr - (pointer to) Unregistered MC command
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIngressPortUnregisteredMcFilterDaCommandGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT CPSS_PACKET_CMD_ENT     *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenIngressPortBcFilterDaCommandSet
*
* DESCRIPTION:
*       Set BC DA filter command per port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       cmd    - BC command
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIngressPortBcFilterDaCommandSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgGenIngressPortBcFilterDaCommandGet
*
* DESCRIPTION:
*       Get BC DA filter command per port
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
*
* OUTPUTS:
*       cmdPtr - (pointer to) BC command
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIngressPortBcFilterDaCommandGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT CPSS_PACKET_CMD_ENT     *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgGenPortIpControlTrapEnableSet
*
* DESCRIPTION:
*       Set IPV4/6 Control Trap Enable Per Port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       ipCntrlType     - IP control type
*                       CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                       CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                       CPSS_DXCH_BRG_IP_CTRL_IPV6_E,
*                       CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIpControlTrapEnableSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_NUM                          portNum,
    IN CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT       ipCntrlType
);

/*******************************************************************************
* cpssDxChBrgGenPortIpControlTrapEnableGet
*
* DESCRIPTION:
*       Get IPV4/6 Control Trap Enable Per Port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       ipCntrlTypePtr - (pointer to) IP control type
*                     CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                     CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                     CPSS_DXCH_BRG_IP_CTRL_IPV6_E,
*                     CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortIpControlTrapEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      *ipCntrlTypePtr
);

/*******************************************************************************
* cpssDxChBrgGenPortBcUdpTrapMirrorEnableSet
*
* DESCRIPTION:
*       Set BC UDP Trap Mirror enable Per Port
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
*       enable  - Enable/disable UDP Trap Mirror.
*                 GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.

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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortBcUdpTrapMirrorEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgGenPortBcUdpTrapMirrorEnableGet
*
* DESCRIPTION:
*       Get BC UDP Trap Mirror enable Per Port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       enablePtr - UDP Trap Mirror status
*                   GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortBcUdpTrapMirrorEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitDropCntrEnableSet
*
* DESCRIPTION:
*       Enable/Disable port for counting packets dropped due to ingress rate
*       limiting by the Ingress Rate Limit Drop Counter
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE: enable counting of packets dropped due to
*                              ingress rate.
*                     GT_FALSE: disable counting of packets dropped due to
*                              ingress rate.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device number or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitDropCntrEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChBrgGenPortRateLimitDropCntrEnableGet
*
* DESCRIPTION:
*       Get port status (enabled/disabled) for counting packets dropped due
*       to ingress rate limiting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE: enable counting of packets dropped due to
*                              ingress rate.
*                     GT_FALSE: disable counting of packets dropped due to
*                              ingress rate.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device number or port number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenPortRateLimitDropCntrEnableGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChBrgGenRateLimitDropCntrSet
*
* DESCRIPTION:
*       Set rate limit drop counter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2.
*
* INPUTS:
*       devNum      - device number
*       value       - the value to set the rate limit drop counter.
*                     the value size is 40 bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range drop counter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitDropCntrSet
(
    IN GT_U8        devNum,
    IN GT_U64       value
);

/*******************************************************************************
* cpssDxChBrgGenRateLimitDropCntrGet
*
* DESCRIPTION:
*       Get rate limit drop counter.
*       The counter counts packets or bytes according to ingress rate limit mode
*       set by cpssDxChBrgGenRateLimitGlobalCfgSet.
*       The counter counts packets/bytes that are dropped due to ingress rate
*       limitting on all ports, where the drop rate limit counting is enabled
*       by cpssDxChBrgGenPortRateLimitDropCntrEnableSet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       valuePtr    - (pointer to) the value of rate limit drop counter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenRateLimitDropCntrGet
(
    IN  GT_U8        devNum,
    OUT GT_U64       *valuePtr
);

/*******************************************************************************
* cpssDxChBrgGenBypassModeSet
*
* DESCRIPTION:
*       Set Bridge Bypass mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       mode    - bridge bypass mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for packets which previously assigned <BridgeBypass>, e.g. by
*       TTI or PCL action.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenBypassModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT    mode
);

/*******************************************************************************
* cpssDxChBrgGenBypassModeGet
*
* DESCRIPTION:
*       Get Bridge Bypass mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*
*
* OUTPUTS:
*       modePtr - (pointer to) bridge bypass mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for packets which previously assigned <BridgeBypass>, e.g. by
*       TTI or PCL action.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenBypassModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_BRG_GEN_BYPASS_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChBrgGenMtuConfigSet
*
* DESCRIPTION:
*       Enable/disable Target ePort MTU check feature.
*       and configure the following parameters:
*       - the MTU byte-count user mode for target ePort MTU check.
*       - the exception command for frames which fails the MTU check
*       - the CPU/drop code assigned to a frame which fails the MTU check
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       mtuCheckEnable  - target ePort MTU check feature enabled/disabled
*                         GT_TRUE – MTU check enabled, GT_FALSE - MTU check disabled
*       byteCountMode   - byte-count mode for MTU
*       exceptionCommand- command assigned to frames that exceed the
*                         default ePort MTU size
*       exceptionCpuCode- CPU/drop code assigned to frame which fails MTU check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, byteCountMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenMtuConfigSet
(
    IN GT_U8                                        devNum,
    IN GT_BOOL                                      mtuCheckEnable,
    IN CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT    byteCountMode,
    IN CPSS_PACKET_CMD_ENT                          exceptionCommand,
    IN CPSS_NET_RX_CPU_CODE_ENT                     exceptionCpuCode
);

/*******************************************************************************
* cpssDxChBrgGenMtuConfigGet
*
* DESCRIPTION:
*       Get the target ePort MTU feature parameters:
*       - the enable/disable target ePort MTU check feature.
*       - the MTU byte-count user mode for target ePort MTU check.
*       - the exception command for frames which fails the MTU check
*       - the CPU/drop code assigned to a frame which fails the MTU check
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*
*
* OUTPUTS:
*       mtuCheckEnablePtr - (pointer to) enable/disable target ePort MTU feature.
*                           GT_TRUE – MTU check enabled, GT_FALSE - MTU check disabled.
*       byteCountModePtr - (pointer to) MTU byte-count mode
*       exceptionCommandPtr - (pointer to) exception cmd for frames which
*                              fails MTU check
*       exceptionCpuCodePtr - (pointer to) CPU/drop code assigned to frame
*                              which fails the MTU check
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on wrong HW value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenMtuConfigGet
(
    IN  GT_U8                                         devNum,
    OUT GT_BOOL                                      *mtuCheckEnablePtr,
    OUT CPSS_DXCH_BRG_GEN_BYTE_COUNT_MODE_MTU_ENT    *byteCountModePtr,
    OUT CPSS_PACKET_CMD_ENT                          *exceptionCommandPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT                     *exceptionCpuCodePtr
);


/*******************************************************************************
* cpssDxChBrgGenMtuPortProfileIdxSet
*
* DESCRIPTION:
*       Set MTU profile index for the given target ePort.
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
*       mtuProfileIdx   - index to global MTU entry
*                         (APPLICABLE RANGES: 0..3).
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
*       GT_OUT_OF_RANGE          – mtuProfileIdx out of range
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenMtuPortProfileIdxSet
(
    IN GT_U8         devNum,
    IN GT_PORT_NUM   portNum,
    IN GT_U32        mtuProfileIdx
);

/*******************************************************************************
* cpssDxChBrgGenMtuPortProfileIdxGet
*
* DESCRIPTION:
*       Get MTU profile index for the given ePort.
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
*       mtuProfileIdxPtr - (pointer to)index to the global MTU entry
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
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenMtuPortProfileIdxGet
(
    IN  GT_U8          devNum,
    IN  GT_PORT_NUM    portNum,
    OUT GT_U32         *mtuProfileIdxPtr
);

/*******************************************************************************
* cpssDxChBrgGenMtuExceedCntrGet
*
* DESCRIPTION:
*       Get target ePort MTU exceeded counter.
*       The number of packets that triggered the Target ePort MTU
*       exceeded exception check.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       valuePtr    - (pointer to) the value of target ePort MTU exceeded counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenMtuExceedCntrGet
(
    IN  GT_U8        devNum,
    OUT GT_U32       *valuePtr
);

/*******************************************************************************
* cpssDxChBrgGenMtuProfileSet
*
* DESCRIPTION:
*       Set an MTU size for profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - the profile (APPLICABLE RANGES: 0..3)
*       mtuSize    - max transmit packet size in bytes
*                    (APPLICABLE RANGES: 0..16383)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range mtuSize
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenMtuProfileSet
(
    IN GT_U8    devNum,
    IN GT_U32   profile,
    IN GT_U32   mtuSize
);

/*******************************************************************************
* cpssDxChBrgGenMtuProfileGet
*
* DESCRIPTION:
*       Get an MTU size for ePort profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - the profile (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       mtuSizePtr - (pointer to) max transmit packet size in bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or profile
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenMtuProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  profile,
    OUT GT_U32  *mtuSizePtr
);

/*******************************************************************************
* cpssDxChBrgGenIpmClassificationModeSet
*
* DESCRIPTION:
*       Configue IPM classification mode.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number.
*       mode    - IPM classification mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or mode.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpmClassificationModeSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChBrgGenIpmClassificationModeGet
*
* DESCRIPTION:
*       Get IPM classification mode.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       modePtr - (pointer to)IPM classification mode.
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
*******************************************************************************/
GT_STATUS cpssDxChBrgGenIpmClassificationModeGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_BRG_GEN_IPM_CLASSIFICATION_MODE_ENT   *modePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgGenh */

