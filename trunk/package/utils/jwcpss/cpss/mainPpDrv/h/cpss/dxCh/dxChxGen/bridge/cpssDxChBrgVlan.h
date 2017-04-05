/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgVlan.h
*
* DESCRIPTION:
*       CPSS DxCh VLAN facility API
*
* FILE REVISION NUMBER:
*       $Revision: 57 $
*
*******************************************************************************/

#ifndef __cpssDxChBrgVlanh
#define __cpssDxChBrgVlanh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>

 /*
 * typedef: struct CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC
 *
 * Description: Structure for configuring Vlan Protocol based classification
 *              parameters.
 *
 * Fields:
 *    vlanId                    - VLAN id
 *    vlanIdAssignCmd           - VLAN id Assignment command.
 *    vlanIdAssignPrecedence    - VLAN id Assignment precedence.
 *                                Soft precedence - The VID assignment can be
 *                                     overridden by other subsequent
 *                                     mechanisms in the ingress pipeline.
 *                                Hard precedence - The VID assignment cannot
 *                                     be overridden by other subsequent
 *                                     mechanisms in the ingress pipeline.
 */
typedef struct
{
    GT_U16                                      vlanId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT        vlanIdAssignCmd;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT vlanIdAssignPrecedence;
}CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC;

/*
 * typedef: struct CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC
 *
 * Description: Structure for configuring QOS Protocol based classification
 *              parameters.
 *
 * Fields:
 *      qosAssignCmd        - Protocol VID assign QOS mode.
 *      qosProfileId        -  The port's QoSProfile. This is the port's
 *                             default QoSProfile and may be assigned to the
 *                             packet as part of the QoS assignment algorithm.
 *      qosAssignPrecedence -  port marking of the QoS Precedence.
 *                             Soft precedence - The QoS Profile assignment can
 *                                 be overridden by other subsequent mechanisms
 *                                 in the ingress pipeline.
 *                             Hard precedence - The QoS Profile assignment
 *                                 cannot be overridden by other subsequent
 *                                 mechanisms in the ingress pipeline.
 *      enableModifyUp      - enable modification of the UP field
 *                            GT_FALSE - If the packet was received tagged and
 *                                       transmitted tagged, the user priority
 *                                       is not modified. If the packet was
 *                                       received untagged, the user priority is
 *                                       set according to the default port
 *                                       user priority.
 *                            GT_TRUE - If the packet is transmitted tagged,
 *                                      the user priority is set according to
 *                                      the QoS Profile table.
 *      enableModifyDscp    - enable modification of the DSCP field (or EXP for MPLS)
 *                            GT_FALSE - The IPv4/6 packet DSCP is not modified.
 *                            GT_TRUE  - The IPv4/6 packet DSCP is updated
 *                                       according to the QoS Profile assignment.
 *
 */
typedef struct
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT        qosAssignCmd;
    GT_U32                                      qosProfileId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosAssignPrecedence;
    GT_BOOL                                     enableModifyUp;
    GT_BOOL                                     enableModifyDscp;
}CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC;


/*
 * Typedef: enum CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT
 *
 * Description:
 *      This enum defines IP Control to CPU Enable Mode
 * Enumerations:
 *     CPSS_DXCH_BRG_IP_CTRL_NONE_E - Disable both IPv4 and Ipv6 Control
 *                                  traffic to CPU
 *     CPSS_DXCH_BRG_IP_CTRL_IPV4_E  - Enable IPv4 Control traffic to CPU
 *     CPSS_DXCH_BRG_IP_CTRL_IPV6_E  - Enable IPv6 Control traffic to CPU
 *     CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E - Enable both IPv4 and IPv6 traffic
 *                                  to CPU
 *
 *  Comments:
 *
 *   The CPSS_DXCH_BRG_IP_CTRL_IPV4_E and CPSS_DXCH_BRG_IP_CTRL_IPV6_E supported
 *   only by DxCh2 and above.
 */
typedef enum
{
    CPSS_DXCH_BRG_IP_CTRL_NONE_E,
    CPSS_DXCH_BRG_IP_CTRL_IPV4_E,
    CPSS_DXCH_BRG_IP_CTRL_IPV6_E,
    CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
} CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT
 *
 * Description:
 *      This enum defines Vlan packet type
 * Enumerations:
 *      CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E            - Unknown unicast
 *      CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E   - Unregistered Non IP
 *                                                              Multicast
 *      CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E     - Unregistered IPv4
 *                                                              Multicast
 *      CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E     - Unregistered IPv6
 *                                                              Multicast
 *      CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E - Unregistered Non IPv4
 *                                                              Broadcast
 *      CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E     - Unregistered IPv4
 *                                                              Broadcast
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E
} CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT
 *
 * Description: Enumeration of assigning L2 multicast VIDX for Broadcast,
 *              Unregistered Multicast and unknown Unicast packets.
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E -
 *                               Unregistered MC Mode - Assign <Flood VIDX>
 *                               from VLAN entry for Unregistered Multicast
 *                               only. Broadcast and Unknown Unicast packets
 *                               are assigned VIDX = 0xFFF which implies
 *                               flooding the packet to all VLAN port members.
 *    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E -
 *                               Apply <Flood VIDX> from VLAN entry for ALL
 *                               flooded traffic (I.e. Unreg MC, BC, Unk UC)
 *
 */
typedef enum
{
   CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E,
   CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E
} CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT
 *
 * Description: Enumeration of port isolation commands.
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E - Port Isolation feature
 *                                                      is disabled
 *    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E      - L2 packets forwaring
 *                                                      enabled according to
 *                                                      port isolation L2 table
 *    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E      - L3 packets forwaring
 *                                                      enabled according to
 *                                                      port isolation L3 table
 *    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E   - L2&L3 packets forwaring
 *                                                      enabled according to
 *                                                      port isolation L2&L3
 *                                                      tables
 */
typedef enum
{
   CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E
} CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT
 *
 * Description: Enumeration of Unregistered IP Multicast eVidx Mode.
 *           The eVidx assigned to packet calculated using
 *           VLAN entry fields unregIpmEVidx and floodVidx
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_E                 -
 *           The eVIDX value assigned to unregistered IP MC packets is unregIpmEVidx
 *    CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV4_E                 -
 *           The eVIDX value assigned to unregistered IPv4 MC packets is unregIpmEVidx.
 *           The eVIDX value assigned to unregistered IPv6 MC packets is floodVidx.
 *    CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV6_E                 -
 *           The eVIDX value assigned to unregistered IPv6 MC packets is unregIpmEVidx.
 *           The eVIDX value that is assigned to unregistered IPv4 MC packets is floodVidx.
 *           IPV6  unregIpmEVidx
 *    CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_IPV6_INCREMENT_E  -
 *           The eVIDX value that is assigned to unregistered IPv4 MC packets is unregIpmEVidx.
 *           The eVIDX value assigned to unregistered IPv6 MC packets is unregIpmEVidx+1.
 */
typedef enum
{
   CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_E,
   CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV4_E,
   CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV6_E,
   CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_IPV6_INCREMENT_E
} CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_BRG_VLAN_INFO_STC
 *
 * Description: A structure to hold generic VLAN entry info.
 *
 * Fields:
 *      unkSrcAddrSecBreach - Unknown Source Address is security breach event.
 *                           GT_TRUE - Packets with unknown MAC Source Addresses
 *                                      generate security breach events.
 *                           GT_FALSE - Packets with unknown MAC Source Addresses
 *                                     assigned to this VLAN do not generate a
 *                                     security breach event and are processed
 *                                     according to the normal Source Address
 *                                     learning rules.
 *      unregNonIpMcastCmd  - The command of Unregistered non IP Multicast packets
 *                            (with a Multicast MAC_DA! = 01-00-5e-xx-xx-xx or
 *                            33-33-xx-xx-xx-xx) assigned to this VLAN.
 *      unregIpv4McastCmd   - The command of Unregistered IPv4 Multicast packets
 *                            (with MAC_DA= 01-00-5e-xx-xx-xx) assigned to
 *                            this VLAN.
 *      unregIpv6McastCmd   - The command of Unregistered IPv6 Multicast packets
 *                            (with MAC_DA= 33-33-xx-xx-xx-xx) assigned to
 *                            this VLAN.
 *      unkUcastCmd         - The command of Unknown Unicast packets assigned
 *                            to this VLAN.
 *      unregIpv4BcastCmd   - The command of Unregistered IPv4 Broadcast packets
 *                            assigned to this VLAN.
 *      unregNonIpv4BcastCmd - The command of Unregistered non-IPv4-Broadcast
 *                             packets assigned to this VLAN.
 *      ipv4IgmpToCpuEn     - Enable IGMP Trapping or Mirroring to CPU according
 *                            to the global setting
 *                            GT_TRUE - enable
 *                            GT_FALSE - disable
 *      mirrToRxAnalyzerEn  - Enable to mirror packets to Rx Analyzer port
 *                            GT_TRUE - Ingress mirrored traffic assigned to
 *                                  this Vlan to the analyzer port
 *                            GT_FALSE - Don't ingress mirrored traffic assigned
 *                                  to this Vlan to the analyzer port
 *      ipv6IcmpToCpuEn     - Enable/Disable ICMPv6 trapping or mirroring to
 *                            the CPU, according to global ICMPv6 message type
 *                            GT_TRUE - enable
 *                            GT_FALSE - disable
 *      ipCtrlToCpuEn       - IPv4/6 control traffic trapping/mirroring to the CPU
 *                            If IPV4 set for this vlan and the corresponding
 *                            mechanism specific global enable is set and the
 *                            packet is IPV4,then the packet is subject to
 *                            trapping/mirroring for the following packet types:
 *                            1. IP Link local Multicast control check
 *                            2. ARP BC
 *                            3. RIPv1
 *                            If IPv6 set for this vlan and the packet is IPv6,
 *                            then the packet is subject to trapping/mirroring
 *                            for the following packet types:
 *                            1. IP Link local Multicast control check
 *                            2. IPv6 Neighbor Solicitation Multicast
 *      ipv4IpmBrgMode      - IPV4 Multicast Bridging mode
 *                              relevant when IPv4IPM Bridging is enabled
 *      ipv6IpmBrgMode      - IPV6 Multicast Bridging mode
 *                              relevant when IPv6IPM Bridging is enabled
 *      ipv4IpmBrgEn        - IPv4 Multicast Bridging Enable
 *                            GT_TRUE - IPv4 multicast packets are bridged
 *                              according to ipv4IpmBrgMode
 *                            GT_FALSE - IPv4 multicast packets are bridged
 *                              according to MAC DA
 *      ipv6IpmBrgEn        - IPv6 Multicast Bridging Enable
 *                            GT_TRUE - IPv6 multicast packets are bridged
 *                              according to ipv6IpmBrgMode
 *                            GT_FALSE - IPv4 multicast packets are bridged
 *                              according to MAC DA
 *      ipv6SiteIdMode      - IPv6 Site Id Mode
 *                            Used by the router for IPv6 scope checking.
 *                            Not supported and ignored for
 *                            DxCh1, DxCh1_Diamond and xCat2 devices.
 *                            Supported for other devices.
 *      ipv4UcastRouteEn    - Enable/Disable IPv4 Unicast Routing on the vlan
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *      ipv4McastRouteEn    - Enable/Disable IPv4 Multicast Routing on the vlan
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *                            Not supported and ignored for
 *                            DxCh1, DxCh1_Diamond and xCat2 devices.
 *                            Supported for other devices.
 *      ipv6UcastRouteEn    - Enable/Disable IPv6 Unicast Routing on the vlan
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *      ipv6McastRouteEn    - Enable/Disable IPv6 Multicast Routing on the vlan
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *                            Not supported and ignored for
 *                            DxCh1, DxCh1_Diamond and xCat2 devices.
 *                            Supported for other devices.
 *      stgId               - stp group (APPLICABLE RANGES: 0..255)
 *      autoLearnDisable    - Enable/Disable automatic learning for this VLAN
 *                              GT_TRUE - Disable automatic learning
 *                              GT_FALSE - Enable automatic learning
 *                            Not supported and ignored for
 *                            DxCh1 and DxCh1_Diamond devices.
 *                            Supported for other devices.
 *      naMsgToCpuEn        - Enable/Disable new address message sending to CPU
 *                              GT_TRUE - enable new address message to CPU
 *                              GT_FALSE - disable new address message to CPU
 *                            Not supported and ignored for
 *                            DxCh1 and DxCh1_Diamond devices.
 *                            Supported for other devices.
 *      mruIdx              - The index that this Vlan's MRU configuration packets
 *                              are associated with (APPLICABLE RANGES: 0..7)
 *                            Not supported and ignored for
 *                            DxCh1 and DxCh1_Diamond devices.
 *                            Supported for other devices.
 *      bcastUdpTrapMirrEn  - Enables trapping/mirroring of Broadcast UDP packets
 *                              based on their destination UDP port
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *                            Not supported and ignored for
 *                            DxCh1 and DxCh1_Diamond devices.
 *                            Supported for other devices.
 *      vrfId               - Virtual Router ID
 *                            (APPLICABLE DEVICES: DxCh2; DxCh3; xCat; xCat3; Lion;Lion2; Bobcat2; Caelum; Bobcat3)
 *                            (APPLICABLE RANGES: 0..4095)
 *      floodVidx           - The multicast group to which the flooded packet
 *                            is to be transmitted.
 *                            Not supported and ignored for
 *                            DxCh1, DxCh1_Diamond, DxCh2 and DxCh3 devices.
 *                            Supported for other devices.
 *                            valid ranges:
 *                            (APPLICABLE RANGES: xCat2 0..1023, 4095)
 *                            (APPLICABLE RANGES: DxCh1, DxCh2, DxCh3, xCat, xCat3, Lion, Lion2 0..4095)
 *                            (APPLICABLE RANGES: Bobcat2, Caelum, Bobcat3 0..0xFFFF)
 *      floodVidxMode       - Mode for assigning L2 multicast VIDX for
 *                            Broadcast, Unregistered Multicast and
 *                            unknown Unicast packets.
 *                            Not supported and ignored for
 *                            DxCh1, DxCh1_Diamond, DxCh2 and DxCh3 devices.
 *      portIsolationMode   - Port Isolation Mode.
 *                            Not supported and ignored for
 *                            DxCh1, DxCh1_Diamond, DxCh2 and DxCh3 devices.
 *      ucastLocalSwitchingEn - Enable/disable local switching of known Unicast
 *                              traffic on this VLAN.
 *                                GT_TRUE - enable
 *                                GT_FALSE - disable
 *                              In order to enable this feature the per port
 *                              local switching should be enabled by the
 *                              cpssDxChBrgGenUcLocalSwitchingEnable.
 *                              Not supported and ignored for
 *                              DxCh1, DxCh1_Diamond, DxCh2 and DxCh3 devices.
 *      mcastLocalSwitchingEn - Enable/disable local switching of Multicast,
 *                              unknown Unicast and Broadcast traffic on
 *                              this VLAN.
 *                                GT_TRUE - enable
 *                                GT_FALSE - disable
 *                              In order to enable this feature the per port
 *                              local switching should be enabled by the
 *                              cpssDxChBrgPortEgressMcastLocalEnable.
 *                              Not supported and ignored for
 *                              DxCh1, DxCh1_Diamond, DxCh2 and DxCh3 devices.
 *      mirrToRxAnalyzerIndex   - Ingress Analyzer Mirror Index (APPLICABLE RANGES: 0..6)
 *                                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      mirrToTxAnalyzerEn  - Enable to mirror packets to Tx Analyzer port
 *                            GT_TRUE - Egress mirrored traffic assigned to
 *                                  this Vlan to the analyzer port
 *                            GT_FALSE - Don't egress mirrored traffic assigned
 *                                  to this Vlan to the analyzer port
 *                            (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      mirrToTxAnalyzerIndex   - Egress Analyzer Mirror Index (APPLICABLE RANGES: 0..6)
 *                                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      fidValue                - Forwarding ID value
 *                                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      unknownMacSaCmd - bridge forwarding decision for packets with
 *                        unknown Source Address. supported commands:
 *                        CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
 *                        CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_HARD_E
 *                        CPSS_PACKET_CMD_DROP_SOFT_E
 *                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      ipv4McBcMirrToAnalyzerEn - When enabled, IPv4 MC/BC packets in this VLAN
 *                         are mirrored to the analyzer specified in
 *                         ipv4McBcMirrToAnalyzerIndex field.
 *                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      ipv4McBcMirrToAnalyzerIndex - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
 *                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      ipv6McMirrToAnalyzerEn - When enabled, IPv6 MC packets in this VLAN are
 *                         mirrored to the analyzer specified in
 *                         ipv6McMirrToAnalyzerIndex field
 *                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      ipv6McMirrToAnalyzerIndex - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
 *                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      fcoeForwardingEn  - When this field is enabled (GT_TRUE),
 *                          and the ePort<FCoE Forwarding Enable> is enabled,
 *                          the packet an be FCoE Forwarded.
 *                         (depending on additional trigger requirements).
 *                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      unregIpmEVidxMode - Unregistered IPM packet eVidx mode
 *                          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      unregIpmEVidx     -  eVidx for assignment to Unregistered IPM packets
 *                           If this field contains 0, eVidx will be assigned
 *                           from floodVidx regardless unregIpmEVidxMode
 *                           (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      fdbLookupKeyMode  - FDB Lookup Key Mode. Determine if VID1 will be used
 *                          as part of a key to lookup an entry in FDB
 *                          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 */
typedef struct
{
    GT_BOOL                                     unkSrcAddrSecBreach;
    CPSS_PACKET_CMD_ENT                         unregNonIpMcastCmd;
    CPSS_PACKET_CMD_ENT                         unregIpv4McastCmd;
    CPSS_PACKET_CMD_ENT                         unregIpv6McastCmd;
    CPSS_PACKET_CMD_ENT                         unkUcastCmd;
    CPSS_PACKET_CMD_ENT                         unregIpv4BcastCmd;
    CPSS_PACKET_CMD_ENT                         unregNonIpv4BcastCmd;
    GT_BOOL                                     ipv4IgmpToCpuEn;
    GT_BOOL                                     mirrToRxAnalyzerEn;
    GT_BOOL                                     ipv6IcmpToCpuEn;
    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT              ipCtrlToCpuEn;
    CPSS_BRG_IPM_MODE_ENT                       ipv4IpmBrgMode;
    CPSS_BRG_IPM_MODE_ENT                       ipv6IpmBrgMode;
    GT_BOOL                                     ipv4IpmBrgEn;
    GT_BOOL                                     ipv6IpmBrgEn;
    CPSS_IP_SITE_ID_ENT                         ipv6SiteIdMode;
    GT_BOOL                                     ipv4UcastRouteEn;
    GT_BOOL                                     ipv4McastRouteEn;
    GT_BOOL                                     ipv6UcastRouteEn;
    GT_BOOL                                     ipv6McastRouteEn;
    GT_U32                                      stgId;
    GT_BOOL                                     autoLearnDisable;
    GT_BOOL                                     naMsgToCpuEn;
    GT_U32                                      mruIdx;
    GT_BOOL                                     bcastUdpTrapMirrEn;
    GT_U32                                      vrfId;
    GT_U16                                      floodVidx;
    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT      floodVidxMode;
    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT   portIsolationMode;
    GT_BOOL                                     ucastLocalSwitchingEn;
    GT_BOOL                                     mcastLocalSwitchingEn;

    GT_U32                                      mirrToRxAnalyzerIndex;
    GT_BOOL                                     mirrToTxAnalyzerEn;
    GT_U32                                      mirrToTxAnalyzerIndex;
    GT_U32                                      fidValue;
    CPSS_PACKET_CMD_ENT                         unknownMacSaCmd;
    GT_BOOL                                     ipv4McBcMirrToAnalyzerEn;
    GT_U32                                      ipv4McBcMirrToAnalyzerIndex;
    GT_BOOL                                     ipv6McMirrToAnalyzerEn;
    GT_U32                                      ipv6McMirrToAnalyzerIndex;

    GT_BOOL                                     fcoeForwardingEn;
    CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT unregIpmEVidxMode;
    GT_U32                                      unregIpmEVidx;
    CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT       fdbLookupKeyMode;

} CPSS_DXCH_BRG_VLAN_INFO_STC;

/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT
 *
 * Description: Enumeration of tagging egress commands.
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E              - if Tag0 and/or Tag1
 *                                                          were classified in
 *                                                          the incoming packet,
 *                                                          they are removed
 *                                                          from the packet.
 *    CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E                  - packet egress with
 *                                                          Tag0 (Tag0 as
 *                                                          defined in ingress
 *                                                          pipe).
 *    CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E                  - packet egress with
 *                                                          Tag1 (Tag1 as
 *                                                          defined in ingress
 *                                                          pipe).
 *    CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E - Outer Tag0, Inner
 *                                                          Tag1 (tag swap).
 *    CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E - Outer Tag1, Inner
 *                                                          Tag0 (tag swap).
 *    CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E             - TAG0 is added to
 *                                                          the packet
 *                                                          regardless of
 *                                                          whether Tag0 and
 *                                                          TAG1 were
 *                                                          classified in the
 *                                                          incoming packet.
 *                                                          (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2)
 *
 *    CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E         - the incoming packet
 *                                                          outer tag is
 *                                                          removed, regardless
 *                                                          of whether it is
 *                                                          Tag0 or Tag1. This
 *                                                          operation is a NOP
 *                                                          if the packet
 *                                                          arrived with neither
 *                                                          Tag0 nor Tag1
 *                                                          classified.
 *    CPSS_DXCH_BRG_VLAN_PORT_DO_NOT_MODIFY_TAG_CMD_E  - Do not modify any tag,
 *                                                       packet tags are sent
 *                                                       as received (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)

 * Note:
 *      Relevant for devices with TR101 feature support.
 */
typedef enum
{
   CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_DO_NOT_MODIFY_TAG_CMD_E

}CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT;


/*
 * Typedef: structure CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
 *
 * Description: structure that hold an array of ports' Tagging commands
 *
 * Fields:
 *      portsCmd - array of ports' tagging commands
 *
 * Note:
 *      Array index is port number.
 */
typedef struct{
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portsCmd[CPSS_MAX_PORTS_NUM_CNS];
}CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC;

/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT
 *
 * Description: Enumeration of local switching traffic types.
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E - known Unicast
 *
 *    CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E - unknown Unicast,
 *                                                        Broadcast, Multicast
 */
typedef enum
{
   CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E,
   CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E
}CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT;


/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT
 *
 * Description: Enumeration of Tag1 removal mode when VID is zero.
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E - don't remove Tag1
 *
 *    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E - enable only if
 *                          ingress was NOT double tag (single tag or untagged)
 *                          (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
 *    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E - enable regardless of ingress
 *                          tagging
 *    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_AND_INGRESS_WITHOUT_TAG1_E - enable only if
 *                          ingress was without tag1
 *                          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 */
typedef enum
{
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E,
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E,
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E,
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_AND_INGRESS_WITHOUT_TAG1_E
}CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT
 *
 * Description:
 *      This enum defines the egress ePort tag state.
 *
 * Enumerations:
 *      CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EVLAN_MODE_E - Use tag state from
 *                              the eVLAN entry, according to egress physical port
 *
 *      CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EPORT_MODE_E - Use tag state from ePort entry
 *
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EVLAN_MODE_E,
    CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EPORT_MODE_E
} CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_BRG_VLAN_EVB_EGRESS_EPORT_TABLE_ACCESS_MODE_ENT
 *
 * Description:
 *      This enum defines the EVB Egress ePort table access mode.
 *
 * Enumerations:
 *       CPSS_DXCH_BRG_VLAN_EVB_EGRESS_EPORT_TABLE_ACCESS_ORIGINAL_TARGET_MODE_E -
 *                       The EVB Egress ePort attribute table is always accessed
 *                       with target port / ePort of the original packet
 *
 *       CPSS_DXCH_BRG_VLAN_EVB_EGRESS_EPORT_TABLE_ACCESS_SNIFF_TARGET_MODE_E -
 *                       When the Desc<OutGoingMtagCmd> of a packet is
 *                       ToTargetSniffer, the EVB Egress ePort Table is
 *                       accessed using the <SniffTRGePort>.
 *
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_BRG_VLAN_EVB_EGRESS_EPORT_TABLE_ACCESS_ORIGINAL_TARGET_MODE_E,
    CPSS_DXCH_BRG_VLAN_EVB_EGRESS_EPORT_TABLE_ACCESS_SNIFF_TARGET_MODE_E
} CPSS_DXCH_BRG_VLAN_EVB_EGRESS_EPORT_TABLE_ACCESS_MODE_ENT;


/*
 * Typedef: enum CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT
 *
 * Description:
 *      This enum defines the Vlan Push command.
 *
 * Enumerations:
 *      CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_NONE_E - none
 *      CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_TUNNEL_E - Push single Tag on tunnel packet layer 2 header
 *      CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_PASSENGER_E - Push single Tag on passenger packet layer 2 header,
 *                                                                            prior to TS encapsulation
 *                                                                            (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *
 *  Comments:
 *      For tunnel header 2 types of tags can be pushed: 4B VLAN Tag/8B E-Tag
 *      For passenger header only 4B VLAN Tag can be pushed.
 */
typedef enum
{
    CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_NONE_E,
    CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_TUNNEL_E,
    CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_PASSENGER_E
} CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT
 *
 * Description: Enumeration of TAG0/TAG1 TPID Select mode .
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_EGRESS_TAG_TPID_SELECT_MODE_E - TPID of Tag0/Tag1 is
 *                              selected according to <Egress Tag0/Tag1 TPID select>
 *    CPSS_DXCH_BRG_VLAN_VID_TO_TPID_SELECT_TABLE_MODE_E - TPID of Tag0/Tag1 is
 *                              selected according to VID0/VID1 to TPID select table
 */
typedef enum
{
   CPSS_DXCH_BRG_VLAN_EGRESS_TAG_TPID_SELECT_MODE_E,
   CPSS_DXCH_BRG_VLAN_VID_TO_TPID_SELECT_TABLE_MODE_E
} CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT
 *
 * Description: Enumeration of indexing mode to access VLAN related tables.
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E - indexing is based on VLAN value.
 *    CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E - indexing is based on Tag1 VID value.
 */
typedef enum
{
   CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E,
   CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E
} CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT;

/*
* Typedef: enum CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT
*
* Description:
*      This enum defines kinds of ingress/egress VLAN translation
*
* Enumerations:
*     CPSS_DXCH_BRG_VLAN_TRANSLATION_DISABLE_E  - disable translation
*     CPSS_DXCH_BRG_VLAN_TRANSLATION_VID0_E     - translate VID0, use
*                                       Ingress/Egress Vlan Translation Table
*     CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_E - translate to VID0,
*                                       use eVLAN<SERVICE-ID>[23:12].
*                                       Is relevant to egress translation only.
*                                       (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
*     CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_VID1_E - translate to
*                                       VID0, VID1, use eVlan<SERVICE-ID>.
*                                       Is relevant to egress translation only.
*                                       (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
*
* Comments:
*     To setup eVLAN<SERVICE-ID> use
*     cpssDxChTunnelStartEgessVlanTableServiceIdSet / Get functions
*
*/
typedef enum
{
    CPSS_DXCH_BRG_VLAN_TRANSLATION_DISABLE_E = GT_FALSE,
    CPSS_DXCH_BRG_VLAN_TRANSLATION_VID0_E    = GT_TRUE,
    CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_E,
    CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_VID1_E
} CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT;

/*
* typedef: enum CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT
*
* Description: Enumeration of VLAN etherType modes
*
* Enumerations:
*   CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E = CPSS_VLAN_ETHERTYPE0_E -
*       EtherType for Tag0. Ingress recognition of Tag0 existence;
*       Egress building of Tag0 etherType.
*       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E  = CPSS_VLAN_ETHERTYPE1_E -
*       EtherType for Tag1. Ingress recognition of Tag1 existence;
*       Egress building of Tag1 etherType.
*   CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E -
*       EtherType for Passenger Tag0. Ingress recognition of Passenger Tag0
*       existence; Ingress building of Passenger Tag0 etherType.
*       (APPLICABLE DEVICES: xCat, xCat3). xCat device: applicable starting from revision C0
*   CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E -
*       EtherType for Passenger Tag1. Ingress recognition of Passenger Tag0
*       existence; Ingress building of Passenger Tag0 etherType.
*       (APPLICABLE DEVICES: xCat, xCat3). xCat device: applicable starting from revision C0
*   CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E -
*       EtherType for Tunnel Start Tag. Egress recognition of Tunnel
*       Start Tag existence; Egress building of Tunnel Start Tag etherType.
*       (APPLICABLE DEVICES: xCat, xCat3). xCat device: applicable starting from revision C0
*/
typedef enum
{
    CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E = CPSS_VLAN_ETHERTYPE0_E,
    CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E,
    CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E,
    CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E,
    CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E

} CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT;

/*
* typedef: enum GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT
*
* Description: Enumeration of number of bytes to pop
*
* Enumerations:
*   GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_NONE_E - pop none (do not pop any byte).
*   GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_4_E    - pop 4 bytes.
*   GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_8_E    - pop 8 bytes.
*   GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_6_E    - pop 6 bytes.
*
*/
typedef enum
{
    GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_NONE_E    = 0,
    GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_4_E       = 1,
    GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_8_E       = 2,
    GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_6_E       = 3
}GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT;


/*******************************************************************************
* cpssDxChBrgVlanInit
*
* DESCRIPTION:
*       Initialize VLAN for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on system init error.
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanInit
(
    IN GT_U8    devNum
);

/*******************************************************************************
* cpssDxChBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
);

/*******************************************************************************
* cpssDxChBrgVlanEntriesRangeWrite
*
* DESCRIPTION:
*       Function set multiple vlans with the same configuration
*       function needed for performances , when the device use "indirect" access
*       to the vlan table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       vlanId              - first VLAN Id
*       numOfEntries        - number of entries (vlans) to set
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong device number or vlanId
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntriesRangeWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  GT_U32                                numOfEntries,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
);

/*******************************************************************************
* cpssDxChBrgVlanEntryRead
*
* DESCRIPTION:
*       Read vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                                 GT_TRUE = Vlan is valid
*                                 GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
);

/*******************************************************************************
* cpssDxChBrgVlanMemberAdd
*
* DESCRIPTION:
*       Add new port member to vlan entry. This function can be called only for
*       add port belongs to device that already member of the vlan.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*                         CPU port supported
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       portTaggingCmd  - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vid or portTaggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case that added port belongs to device that is new device for vlan
*       other function is used - cpssDxChBrgVlanEntryWrite.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMemberAdd
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vlanId,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
);

/*******************************************************************************
* cpssDxChBrgVlanPortDelete
*
* DESCRIPTION:
*       Delete port member from vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN Id
*       portNum     - port number
*                     CPU port supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortDelete
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN GT_PHYSICAL_PORT_NUM         portNum
);

/*******************************************************************************
* cpssDxChBrgVlanMemberSet
*
* DESCRIPTION:
*       Set specific member at VLAN entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*                         CPU port supported
*       isMember        - GT_TRUE,  port would set as member
*                         GT_FALSE, port would set as not-member
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       taggingCmd      - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or taggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMemberSet
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vlanId,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 isMember,
    IN  GT_BOOL                 isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd
);


/*******************************************************************************
* cpssDxChBrgVlanPortVidGet
*
* DESCRIPTION:
*       For ingress direction : Get port's default VID0.
*       For egress direction  : Get port's VID0 when egress port <VID0 Command>=Enabled
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       direction - ingress/egress direction
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* OUTPUTS:
*       vidPtr   - default VLAN ID.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or port or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN CPSS_DIRECTION_ENT          direction,
    OUT  GT_U16          *vidPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortVidSet
*
* DESCRIPTION:
*       For ingress direction : Set port's default VID0.
*       For egress direction  : Set port's VID0 when egress port <VID0 Command>=Enabled
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       direction - ingress/egress direction
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*       vlanId   - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum port, or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN CPSS_DIRECTION_ENT          direction,
    IN  GT_U16          vlanId
);

/*******************************************************************************
* cpssDxChBrgVlanPortIngFltEnable
*
* DESCRIPTION:
*       Enable/disable Ingress Filtering for specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       enable    - GT_TRUE, enable ingress filtering
*                   GT_FALSE, disable ingress filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngFltEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortIngFltEnableGet
*
* DESCRIPTION:
*       Get status of Ingress Filtering for specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       enablePtr    - Pointer to Ingress Filtering status
*                      - GT_TRUE, ingress filtering is enabled
*                      - GT_FALSE, ingress filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngFltEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassVlanEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
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
*                 In eArch devices portNum is default ePort.
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassVlanEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassVlanEnableGet
*
* DESCRIPTION:
*       Get status of Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*                    In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr  - GT_TRUE for enable, otherwise GT_FALSE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassVlanEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassQosEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
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
*                 In eArch devices portNum is default ePort.
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassQosEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassQosEnableGet
*
* DESCRIPTION:
*       Get status of Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr - GT_TRUE for enable, otherwise GT_FALSE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassQosEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanProtoClassSet
*
* DESCRIPTION:
*       Write etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       entryNum    - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*       etherType   - Ether Type or DSAP/SSAP
*       encListPtr  - encapsulation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    IN GT_U16                           etherType,
    IN CPSS_PROT_CLASS_ENCAP_STC        *encListPtr
);

/*******************************************************************************
* cpssDxChBrgVlanProtoClassGet
*
* DESCRIPTION:
*       Read etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Ether Type or DSAP/SSAP
*       encListPtr   - (pointer to) encapsulation.
*       validEntryPtr - (pointer to) validity bit
*                                   GT_TRUE - the entry is valid
*                                   GT_FALSE - the entry is invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassGet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    OUT GT_U16                          *etherTypePtr,
    OUT CPSS_PROT_CLASS_ENCAP_STC       *encListPtr,
    OUT GT_BOOL                         *validEntryPtr
);

/*******************************************************************************
* cpssDxChBrgVlanProtoClassInvalidate
*
* DESCRIPTION:
*       Invalidates entry of Protocol based classification for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum
);

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosSet
*
* DESCRIPTION:
*       Write Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       entryNum            - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*       vlanCfgPtr          - pointer to VID parameters
*       qosCfgPtr           - pointer to QoS parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It is possible to update only VLAN or QOS parameters, when one
*       of those input pointers is NULL.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN GT_U32                                   entryNum,
    IN CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC    *vlanCfgPtr,
    IN CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC     *qosCfgPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosGet
*
* DESCRIPTION:
*       Get Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       entryNum            - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       vlanCfgPtr          - pointer to VID parameters (can be NULL)
*       qosCfgPtr           - pointer to QoS parameters (can be NULL)
*       validEntryPtr       - pointer to validity bit
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      It is possible to get only one of VLAN or QOS parameters by receiving
*      one of those pointers as NULL pointer or get only validity bit.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosGet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN GT_U32                                   entryNum,
    OUT CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC   *vlanCfgPtr,
    OUT CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC    *qosCfgPtr,
    OUT GT_BOOL                                 *validEntryPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosInvalidate
*
* DESCRIPTION:
*       Invalidate protocol based QoS and VLAN assignment entry for
*       specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portNum      - port number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosInvalidate
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_U32                   entryNum
);
/*******************************************************************************
* cpssDxChBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       vlanId    - vlan Id
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanLearningStateSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_BOOL  status
);


/*******************************************************************************
* cpssDxChBrgVlanIsDevMember
*
* DESCRIPTION:
*       Checks if specified device has members belong to specified vlan
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - if port is member of vlan
*       GT_FALSE  - if port does not member of vlan or other status error
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL cpssDxChBrgVlanIsDevMember
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
);

/*******************************************************************************
* cpssDxChBrgVlanToStpIdBind
*
* DESCRIPTION:
*       Bind VLAN to STP Id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*       stpId   - STP Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_OUT_OF_RANGE          - out of range stpId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanToStpIdBind
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   stpId
);

/*******************************************************************************
* cpssDxChBrgVlanStpIdGet
*
* DESCRIPTION:
*       Read STP Id that bind to specified VLAN.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*
* OUTPUTS:
*       stpIdPtr   - pointer to STP Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanStpIdGet
(
    IN  GT_U8    devNum,
    IN  GT_U16   vlanId,
    OUT GT_U16   *stpIdPtr
);

/*******************************************************************************
* cpssDxChBrgVlanEtherTypeSet
*
* DESCRIPTION:
*       Sets vlan Ether Type mode and vlan range.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*       etherType     - the EtherType to be recognized tagged packets. See
*                       comment.
*       vidRange      - the maximum vlan range to accept for the specified
*                       etheType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal vidRange value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the vid of the incoming packet is greater or equal (>=) than the vlan
*       range value, the packet is filtered.
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType,
    IN  GT_U16                  vidRange
);

/*******************************************************************************
* cpssDxChBrgVlanEtherTypeGet
*
* DESCRIPTION:
*       Gets vlan Ether Type mode and vlan range.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*
* OUTPUTS:
*       etherTypePtr  - (pointer to)the EtherType to be recognized tagged
*                        packets. See comment.
*       vidRangePtr   - (pointer to)the maximum vlan range to accept for
*                        the specified etheType.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal vidRange value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEtherTypeGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    OUT  GT_U16                 *etherTypePtr,
    OUT  GT_U16                 *vidRangePtr
);

/*******************************************************************************
* cpssDxChBrgVlanTableInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN Table entries by writing 0 to the
*       first word.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTableInvalidate
(
    IN GT_U8 devNum
);

/*******************************************************************************
* cpssDxChBrgVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       vlanId     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
);

/*******************************************************************************
* cpssDxChBrgVlanUnkUnregFilterSet
*
* DESCRIPTION:
*       Set per VLAN filtering command for specified Unknown or Unregistered
*       packet type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*             devNum                - device number.
*             vlanId                - VLAN ID
*             packetType            - packet type
*             cmd                   - command for the specified packet type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUnkUnregFilterSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT   packetType,
    IN CPSS_PACKET_CMD_ENT                  cmd

);

/*******************************************************************************
* cpssDxChBrgVlanForcePvidEnable
*
* DESCRIPTION:
*       Set Port VID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE  - PVID is assigned to all packets.
*                  GT_FALSE - PVID is assigned to untagged or priority tagged
*                             packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForcePvidEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgVlanForcePvidEnableGet
*
* DESCRIPTION:
*       Get Port VID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*                  In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr   - GT_TRUE  - PVID is assigned to all packets.
*                     GT_FALSE - PVID is assigned to untagged or priority tagged
*                                packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForcePvidEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortVidPrecedenceSet
*
* DESCRIPTION:
*       Set Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*                     In eArch devices portNum is default ePort.
*       precedence  - precedence type - soft or hard
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence
);

/*******************************************************************************
* cpssDxChBrgVlanPortVidPrecedenceGet
*
* DESCRIPTION:
*       Get Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*                     In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       precedencePtr - (pointer to) precedence type - soft or hard
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidPrecedenceGet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    OUT CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  *precedencePtr
);

/*******************************************************************************
* cpssDxChBrgVlanIpUcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Unicast Routing on Vlan
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       enable      - GT_TRUE - enable ip unicast routing;
*                     GT_FALSE -  disable ip unicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpUcRouteEnable
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocol,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgVlanIpMcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Multicast Routing on Vlan
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       enable      - GT_TRUE - enable ip multicast routing;
*                     GT_FALSE -  disable ip multicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpMcRouteEnable
(
    IN GT_U8                            devNum,
    IN GT_U16                           vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN GT_BOOL                          enable
);

/*******************************************************************************
* cpssDxChBrgVlanNASecurEnable
*
* DESCRIPTION:
*      This function enables/disables per VLAN the generation of
*      security breach event for packets with unknown Source MAC addresses.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN Id
*       enable  - GT_TRUE - enable generation of security breach event
*                 GT_FALSE- disable generation of security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanNASecurEnable
(
    IN GT_U8           devNum,
    IN GT_U16          vlanId,
    IN GT_BOOL         enable
);

/*******************************************************************************
* cpssDxChBrgVlanIgmpSnoopingEnable
*
* DESCRIPTION:
*       Enable/Disable IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*       enable  - GT_TRUE - enable
*                 GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIgmpSnoopingEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgVlanIgmpSnoopingEnableGet
*
* DESCRIPTION:
*       Gets the status of IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable
*                    GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIgmpSnoopingEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr
);

/*****************************************************************************
* cpssDxChBrgVlanIpCntlToCpuSet
*
* DESCRIPTION:
*       Enable/disable IP control traffic trapping/mirroring to CPU.
*       When set, this enables the following control traffic to be eligible
*       for mirroring/trapping to the CPU:
*        - ARP
*        - IPv6 Neighbor Solicitation
*        - IPv4/v6 Control Protocols Running Over Link-Local Multicast
*        - RIPv1 MAC Broadcast
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*       ipCntrlType - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpCntlToCpuSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT       ipCntrlType
);

/*****************************************************************************
* cpssDxChBrgVlanIpCntlToCpuGet
*
* DESCRIPTION:
*       Gets IP control traffic trapping/mirroring to CPU status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*
* OUTPUTS:
*       ipCntrlTypePtr - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpCntlToCpuGet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    OUT CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      *ipCntrlTypePtr
);

/*******************************************************************************
* cpssDxChBrgVlanIpV6IcmpToCpuEnable
*
* DESCRIPTION:
*       Enable/Disable ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*        enable   - GT_TRUE - enable
*                   GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpV6IcmpToCpuEnable
(
    IN GT_U8      devNum,
    IN GT_U16     vlanId,
    IN GT_BOOL    enable
);

/*******************************************************************************
* cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
*
* DESCRIPTION:
*       Gets status of ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*
* OUTPUTS:
*        enablePtr   - GT_TRUE - enable
*                      GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U16     vlanId,
    OUT GT_BOOL    *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
* DESCRIPTION:
*     Enable or disable per Vlan the Broadcast UDP packets Mirror/Trap to the
*     CPU based on their destination UDP port.  Destination UDP port, command
*     (trap or mirror) and CPU code configured by
*     cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*       enable - Enable/disable UDP Trap Mirror.
*                GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUdpBcPktsToCpuEnable
(
    IN GT_U8        devNum,
    IN  GT_U16      vlanId,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
*
* DESCRIPTION:
*     Gets per Vlan if the Broadcast UDP packets are Mirror/Trap to the
*     CPU based on their destination UDP port.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*
* OUTPUTS:
*       enablePtr - UDP Trap Mirror status
*                   GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.

*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanIpv6SourceSiteIdSet
*
* DESCRIPTION:
*       Sets a vlan ipv6 site id
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan.
*       siteId          - the site id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv6SourceSiteIdSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_IP_SITE_ID_ENT                  siteId
);

/*******************************************************************************
* cpssDxChBrgVlanIpmBridgingEnable
*
* DESCRIPTION:
*       IPv4/Ipv6 Multicast Bridging Enable
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device id
*       vlanId  - Vlan Id
*       ipVer   - IPv4 or Ipv6
*       enable  - GT_TRUE - IPv4 multicast packets are bridged
*                   according to ipv4IpmBrgMode
*                 GT_FALSE - IPv4 multicast packets are bridged
*                   according to MAC DA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpmBridgingEnable
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgVlanIpmBridgingModeSet
*
* DESCRIPTION:
*       Sets the IPM bridging mode of Vlan. Relevant when IPM Bridging enabled.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device id
*       vlanId      - Vlan Id
*       ipVer       - IPv4 or Ipv6
*       ipmMode    - IPM bridging mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpmBridgingModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  CPSS_BRG_IPM_MODE_ENT       ipmMode
);

/*******************************************************************************
* cpssDxChBrgVlanIngressMirrorEnable
*
* DESCRIPTION:
*       Enable/Disable ingress mirroring of packets to the Ingress Analyzer
*       port for packets assigned the given VLAN-ID.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device id
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - Ingress mirrored traffic assigned to
*                          this Vlan to the analyzer port
*                 GT_FALSE - Don't ingress mirrored traffic assigned
*                          to this Vlan to the analyzer port
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIngressMirrorEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
);

/*******************************************************************************
* cpssDxChBrgVlanPortAccFrameTypeSet
*
* DESCRIPTION:
*       Set port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       frameType -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortAccFrameTypeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType
);

/*******************************************************************************
* cpssDxChBrgVlanPortAccFrameTypeGet
*
* DESCRIPTION:
*       Get port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       frameTypePtr -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortAccFrameTypeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     *frameTypePtr
);

/*******************************************************************************
* cpssDxChBrgVlanMruProfileIdxSet
*
* DESCRIPTION:
*     Set Maximum Receive Unit MRU profile index for a VLAN.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     Value of MRU for profile is set by cpssDxChBrgVlanMruProfileValueSet.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMruProfileIdxSet
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_U32    mruIndex
);

/*******************************************************************************
* cpssDxChBrgVlanMruProfileValueSet
*
* DESCRIPTION:
*     Set MRU value for a VLAN MRU profile.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     cpssDxChBrgVlanMruProfileIdxSet set index of profile for a specific VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
*       mruValue - MRU value in bytes
*                  (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 0..0xFFFF;
*                                      Bobcat2, Caelum, Bobcat3  0..0x3FFF)
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMruProfileValueSet
(
    IN GT_U8     devNum,
    IN GT_U32    mruIndex,
    IN GT_U32    mruValue
);

/*******************************************************************************
* cpssDxChBrgVlanMruProfileValueGet
*
* DESCRIPTION:
*     Get MRU value for a VLAN MRU profile.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
* OUTPUTS:
*       mruValuePtr - (pointer to) MRU value in bytes
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMruProfileValueGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    mruIndex,
    OUT GT_U32    *mruValuePtr
);

/*******************************************************************************
* cpssDxChBrgVlanNaToCpuEnable
*
* DESCRIPTION:
*     Enable/Disable New Address (NA) Message Sending to CPU per VLAN  .
*     To send NA to CPU both VLAN and port must be set to send NA to CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       enable   - GT_TRUE  - enable New Address Message Sending to CPU
*                  GT_FALSE - disable New Address Message Sending to CPU
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanNaToCpuEnable
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChBrgVlanVrfIdSet
*
* DESCRIPTION:
*       Sets vlan Virtual Router ID
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan id.
*       vrfId           - the virtual router id (APPLICABLE RANGES: 0..4095).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_OUT_OF_RANGE          - when vrfId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanVrfIdSet
(
    IN GT_U8                   devNum,
    IN GT_U16                  vlanId,
    IN GT_U32                  vrfId
);

/*******************************************************************************
* cpssDxChBrgVlanBridgingModeSet
*
* DESCRIPTION:
*     Set bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       brgMode  - bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBridgingModeSet
(
    IN GT_U8                devNum,
    IN CPSS_BRG_MODE_ENT    brgMode
);

/*******************************************************************************
* cpssDxChBrgVlanBridgingModeGet
*
* DESCRIPTION:
*     Get bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
* OUTPUTS:
*       brgModePtr  - (pointer to) bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBridgingModeGet
(
    IN  GT_U8                devNum,
    OUT CPSS_BRG_MODE_ENT    *brgModePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortTranslationEnableSet
*
* DESCRIPTION:
*       Enable/Disable Vlan Translation per ingress or egress port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - port number or CPU port number for egress direction.
*                         In eArch devices portNum is default ePort for ingress direction.
*       direction       - ingress or egress
*       enable          - enable/disable ingress/egress Vlan Translation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortTranslationEnableSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_NUM                        portNum,
    IN CPSS_DIRECTION_ENT                 direction,
    IN CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortTranslationEnableGet
*
* DESCRIPTION:
*       Get the status of Vlan Translation (Enable/Disable) per ingress or
*       egress port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       direction       - ingress or egress
*       portNum         - port number or CPU port number for egress direction.
*                         In eArch devices portNum is default ePort for ingress direction.
*
* OUTPUTS:
*       enablePtr       - (pointer to) ingress/egress Vlan Translation status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortTranslationEnableGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    IN  CPSS_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanTranslationEntryWrite
*
* DESCRIPTION:
*       Write an entry into Vlan Translation Table Ingress or Egress
*       This mapping avoids collisions between a VLAN-ID
*       used for a logical interface and a VLAN-ID used by the local network.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Translation Table
*                         (APPLICABLE RANGES: 0..4095).
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*       direction       - ingress or egress
*       transVlanId     - Translated Vlan ID, use as value in the
*                         Translation Table (APPLICABLE RANGES: 0..4095).
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_OUT_OF_RANGE          - when transVlanId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTranslationEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_U16                       transVlanId
);

/*******************************************************************************
* cpssDxChBrgVlanTranslationEntryRead
*
* DESCRIPTION:
*       Read an entry from Vlan Translation Table, Ingress or Egress.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Translation Table, 0..4095.
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*       direction       - ingress or egress
*
* OUTPUTS:
*       transVlanIdPtr  - (pointer to) Translated Vlan ID, used as value in the
*                         Translation Table.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTranslationEntryRead
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    OUT GT_U16                      *transVlanIdPtr
);

/*******************************************************************************
* cpssDxChBrgVlanValidCheckEnableSet
*
* DESCRIPTION:
*       Enable/disable check of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - physical device number
*       enable    - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanValidCheckEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgVlanValidCheckEnableGet
*
* DESCRIPTION:
*       Get check status  of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - pointer to check status  of "Valid" field in the VLAN entry.
*                 - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to read from hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanValidCheckEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanTpidEntrySet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       tableType   - etherType table selection
*       entryIndex  - entry index for TPID table (APPLICABLE RANGES: 0..7)
*       etherType   - Tag Protocol ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, tableType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E, TPID0 use entry indexes (0,2,4,6)
*       CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E, TPID1 use entry indexes (1,3,5,7)
*
*   Egress Logical Port TPID table each entry defines a Tag0 TPID and Tag1 TPID;
*   4 entries of {16b Tag0 TPID, 16b Tag1 TPID})
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidEntrySet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_ETHERTYPE_TABLE_ENT   tableType,
    IN  GT_U32                          entryIndex,
    IN  GT_U16                          etherType
);

/*******************************************************************************
* cpssDxChBrgVlanTpidEntryGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       tableType   - etherType table selection
*       entryIndex  - entry index for TPID table (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Tag Protocol ID value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, tableType
*       GT_BAD_PTR               - etherTypePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_ETHERTYPE_TABLE_ENT   tableType,
    IN  GT_U32                          entryIndex,
    OUT GT_U16                         *etherTypePtr
);

/*******************************************************************************
* cpssDxChBrgVlanIngressTpidProfileSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) per profile .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - TPID profile. (APPLICABLE RANGES: 0..7)
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent 8 entries at the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, profile, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For the ingress direction multiple TPID may be used to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*       2. bind 'port' to 'profile' using function cpssDxChBrgVlanPortIngressTpidProfileSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIngressTpidProfileSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               profile,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
);

/*******************************************************************************
* cpssDxChBrgVlanIngressTpidProfileGet
*
* DESCRIPTION:
*       Function gts bitmap of TPID (Tag Protocol ID) per profile .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - TPID profile. (APPLICABLE RANGES: 0..7)
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidBmpPtr    - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, profile, ethMode
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIngressTpidProfileGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               profile,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
);

/*******************************************************************************
* cpssDxChBrgVlanTpidTagTypeSet
*
* DESCRIPTION:
*       Set TPID tag type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       index       - TPID index (APPLICABLE RANGES: 0..7)
*       type        - the TPID type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong input parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidTagTypeSet
(
    IN GT_U8                        devNum,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_U32                       index,
    IN CPSS_BRG_TPID_SIZE_TYPE_ENT  type
);

/*******************************************************************************
* cpssDxChBrgVlanTpidTagTypeGet
*
* DESCRIPTION:
*       Get TPID tag type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       index       - TPID index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       typePtr     - (pointer to) the TPID type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong input parameter
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidTagTypeGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DIRECTION_ENT          direction,
    IN  GT_U32                      index,
    OUT CPSS_BRG_TPID_SIZE_TYPE_ENT *typePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidProfileSet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) profile per ingress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number or CPU port.
*                    In eArch devices portNum is default ePort for default profile.
*       ethMode    - TAG0/TAG1 selector
*       isDefaultProfile - indication that the profile is the default profile or
*                          used for re-parse after TTI lookup:
*                            1. Parse inner Layer2 after TT
*                            2. Re-parse Layer2 after popping EVB/BPE tag
*                            3. Parsing of passenger packet
*                        GT_TRUE  - used as default profile
*                        GT_FALSE - used for re-parse after TTI lookup.
*       profile    - TPID profile. (APPLICABLE RANGE: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - profile > 7
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For setting TPID bmp per profile use function cpssDxChBrgVlanIngressTpidProfileSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidProfileSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN GT_BOOL               isDefaultProfile,
    IN  GT_U32               profile
);

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidProfileGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) profile per ingress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number or CPU port.
*                    In eArch devices portNum is default ePort for default profile.
*       ethMode    - TAG0/TAG1 selector
*       isDefaultProfile - indication that the profile is the default profile or
*                          used for re-parse after TTI lookup:
*                            1. Parse inner Layer2 after TT
*                            2. Re-parse Layer2 after popping EVB/BPE tag
*                            3. Parsing of passenger packet
*                        GT_TRUE  - used as default profile
*                        GT_FALSE - used for re-parse after TTI lookup.
*
* OUTPUTS:
*       profilePtr    - (pointer to)TPID profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidProfileGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN GT_BOOL               isDefaultProfile,
    OUT GT_U32               *profilePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - etherType mode selector; valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E
*       tpidBmp    - bitmap represent 8 entries at the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
);

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes  per
*       ingress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - etherType mode selector. valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E
*
* OUTPUTS:
*       tpidBmpPtr - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortEgressTpidSet
*
* DESCRIPTION:
*       Function sets index of TPID (Tag protocol ID) table per egress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       ethMode        - ethertype mode selector; valid values:
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E
*       tpidEntryIndex - TPID table entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortEgressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    IN  GT_U32               tpidEntryIndex
);

/*******************************************************************************
* cpssDxChBrgVlanPortEgressTpidGet
*
* DESCRIPTION:
*       Function gets index of TPID (Tag Protocol ID) table per egress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - ethertype mode selector; valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortEgressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    OUT GT_U32               *tpidEntryIndexPtr
);

/*******************************************************************************
* cpssDxChBrgVlanEgressTagTpidSelectModeSet
*
* DESCRIPTION:
*       Set Tag 0/1 TPID (Tag Protocol Identifies) Select Mode
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ethMode     - TAG0/TAG1 selector
*       mode        - 0: TPID of Tag0/1 is selected according to
*                          <Egress Tag 0/1 TPID select>
*                     1: TPID of Tag0/1 is selected according to
*                           VID0/1 to TPID select table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressTagTpidSelectModeSet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    IN CPSS_ETHER_MODE_ENT                          ethMode,
    IN CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgVlanEgressTagTpidSelectModeGet
*
* DESCRIPTION:
*       Returns Tag 0/1 TPID (Tag Protocol Identifies) Select Mode
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ethMode     - TAG0/TAG1 selector
*
* OUTPUTS:
*       modePtr   - (pointer to)
*                     0: TPID of Tag0/1 is selected according to
*                          <Egress Tag 0/1 TPID select>
*                     1: TPID of Tag0/1 is selected according to
*                           VID0/1 to TPID select table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressTagTpidSelectModeGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    IN  CPSS_ETHER_MODE_ENT                                 ethMode,
    OUT CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT         *modePtr
);


/*******************************************************************************
* cpssDxChBrgVlanRangeSet
*
* DESCRIPTION:
*       Function configures the valid VLAN Range.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       vidRange  - VID range for VLAN filtering (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRangeSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vidRange
);

/*******************************************************************************
* cpssDxChBrgVlanRangeGet
*
* DESCRIPTION:
*       Function gets the valid VLAN Range.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vidRangePtr - (pointer to) VID range for VLAN filtering
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - vidRangePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRangeGet
(
    IN  GT_U8   devNum,
    OUT GT_U16  *vidRangePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortIsolationCmdSet
*
* DESCRIPTION:
*       Function sets port isolation command per egress VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN id
*       cmd     - port isolation command for given VID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or cmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIsolationCmdSet
(
    IN  GT_U8                                      devNum,
    IN  GT_U16                                     vlanId,
    IN  CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgVlanLocalSwitchingEnableSet
*
* DESCRIPTION:
*       Function sets local switching of Multicast, known and unknown Unicast,
*       and Broadcast traffic per VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN id
*       trafficType - local switching traffic type
*       enable      - Enable/disable of local switching
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To enable local switching of Multicast, unknown Unicast, and
*       Broadcast traffic, both this field in the VLAN entry and the
*       egress port configuration must be enabled for Multicast local switching
*       in function cpssDxChBrgPortEgressMcastLocalEnable.
*
*       2. To enable  local switching of known Unicast traffic, both this
*       field in the VLAN entry and the ingress port configuration must
*       be enabled for Unicast local switching in function
*       cpssDxChBrgGenUcLocalSwitchingEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanLocalSwitchingEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    IN  CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT  trafficType,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgVlanMirrorToTxAnalyzerSet
*
* DESCRIPTION:
*       Function sets Tx Analyzer port index to given VLAN.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       vlanId                   - VLAN id
*       enable                   - Enable/disable Tx Mirroring
*                                  GT_TRUE - enable
*                                  GT_FALSE - disable
*       txAnalyzerIndex          - Tx Analyzer index
*                                  (APPLICABLE RANGES: 0..6)
*                                  relevant only if mirrToTxAnalyzerEn == GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMirrorToTxAnalyzerSet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    IN  GT_BOOL     enable,
    IN  GT_U32      txAnalyzerIndex
);

/*******************************************************************************
* cpssDxChBrgVlanFloodVidxModeSet
*
* DESCRIPTION:
*       Function sets Flood VIDX and Flood VIDX Mode per VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id
*       floodVidx     - VIDX value (12bit) - VIDX value applied to Broadcast,
*                       Unregistered Multicast and unknown Unicast packets,
*                       depending on the VLAN entry field <Flood VIDX Mode>.
*                       (APPLICABLE RANGES: xCat2 0..1023, 4095)
*       floodVidxMode - Flood VIDX Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or floodVidxMode
*       GT_OUT_OF_RANGE          - illegal floodVidx
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanFloodVidxModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U16                                  vlanId,
    IN  GT_U16                                  floodVidx,
    IN  CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT  floodVidxMode
);

/*******************************************************************************
* cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*
* DESCRIPTION:
*       Set Tag1 removal mode from the egress port Tag State if Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       mode          - Vlan Tag1 Removal mode when Tag1 VID=0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
*
* DESCRIPTION:
*       Get Tag1 removal mode from the egress port Tag State when Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - (pointer to) Vlan Tag1 Removal mode when Tag1 VID=0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_BAD_STATE             - on bad state of register
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     *modePtr
);

/*******************************************************************************
* cpssDxChBrgVlanForceNewDsaToCpuEnableSet
*
* DESCRIPTION:
*       Enable / Disable preserving the original VLAN tag
*       and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
*       enable        - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForceNewDsaToCpuEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgVlanForceNewDsaToCpuEnableGet
*
* DESCRIPTION:
*       Get Force New DSA To Cpu mode for enabling / disabling
*       preserving the original VLAN tag and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
* OUTPUTS:
*       enablePtr     - pointer to Force New DSA To Cpu mode.
*                     - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForceNewDsaToCpuEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanKeepVlan1EnableSet
*
* DESCRIPTION:
*       Enable/Disable keeping VLAN1 in the packet, for packets received with VLAN1,
*       even-though the tag-state of this egress-port is configured in the
*       VLAN-table to "untagged" or "VLAN0".
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority (APPLICABLE RANGES: 0..7)
*       enable          - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanKeepVlan1EnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U8                       up,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgVlanKeepVlan1EnableGet
*
* DESCRIPTION:
*       Get status of keeping VLAN1 in the packet, for packets received with VLAN1.
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       enablePtr       - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanKeepVlan1EnableGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U8                       up,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet
*
* DESCRIPTION:
*       Enable Learning Of Original Tag1 VID Per Port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - enable learning
*                 GT_FALSE - disable learning
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
GT_STATUS cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet
*
* DESCRIPTION:
*       Get Learning mode Of Original Tag1 VID Per Port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                 GT_TRUE  - enable learning
*                 GT_FALSE - disable learning
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
GT_STATUS cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);


/*******************************************************************************
* cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet
*
* DESCRIPTION:
*       When enabled, IPv4 MC/BC packets in this eVLAN are mirrored to the
*       analyzer specified in this field. This is independent of the analyzer
*       configuration of other traffic in this eVLAN.
*       0x0 is a reserved value that means Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - IPv4 MC/BC packets in this eVLAN are mirrored to the
*                 analyzer specified in this field
*                 GT_FALSE - IPv4 MC/BC packets in this eVLAN are NOT mirrored
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
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
GT_STATUS cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
);

/*******************************************************************************
* cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet
*
* DESCRIPTION:
*       Get Ipv4 MC/BC Mirror To Analyzer Index
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                   GT_TRUE - IPv4 MC/BC packets in this eVLAN are mirrored to the
*                   analyzer specified in this field
*                   GT_FALSE - IPv4 MC/BC packets in this eVLAN are NOT mirrored
*       indexPtr   - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
);


/*******************************************************************************
* cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet
*
* DESCRIPTION:
*       When enabled, IPv6 MC packets in this eVLAN are mirrored to the
*       analyzer. This is independent of the analyzer configuration of other
*       traffic in this eVLAN.
*       0x0 is a reserved value that means Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - IPv6 MC packets in this eVLAN are mirrored to the
*                 analyzer specified in this field
*                 GT_FALSE - IPv6 MC packets in this eVLAN are NOT mirrored to the
*                 analyzer specified in this field
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
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
GT_STATUS cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
);

/*******************************************************************************
* cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet
*
* DESCRIPTION:
*       Get Ipv6 MC Mirror To Analyzer Index
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                   GT_TRUE - IPv6 MC packets in this eVLAN are mirrored to the
*                   analyzer specified in this field
*                   GT_FALSE - IPv6 MC packets in this eVLAN are NOT mirrored to the
*                   analyzer specified in this field
*       indexPtr   - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
);

/*******************************************************************************
* cpssDxChBrgVlanForwardingIdSet
*
* DESCRIPTION:
*       Set Forwarding ID value.
*       Used for virtual birdges per eVLAN.
*       Used by the bridge engine for entry lookup and entry match (replaces VID
*       in bridge entry)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       fidValue - Forwarding ID value
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
GT_STATUS cpssDxChBrgVlanForwardingIdSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_U32  fidValue
);

/*******************************************************************************
* cpssDxChBrgVlanForwardingIdGet
*
* DESCRIPTION:
*       Get Forwarding ID value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       fidValuePtr - (pointer to) Forwarding ID value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForwardingIdGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_U32  *fidValuePtr
);


/*******************************************************************************
* cpssDxChBrgVlanUnknownMacSaCommandSet
*
* DESCRIPTION:
*       Set bridge forwarding decision for packets with unknown Source Address.
*       Only relevant in controlled address learning mode, when <Auto-learning
*       enable> = Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID
*       cmd          - supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_HARD_E
*                      CPSS_PACKET_CMD_DROP_SOFT_E
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
GT_STATUS cpssDxChBrgVlanUnknownMacSaCommandSet
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* cpssDxChBrgVlanUnknownMacSaCommandGet
*
* DESCRIPTION:
*       Get bridge forwarding decision for packets with unknown Source Address.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID
*
* OUTPUTS:
*       cmdPtr   - (pointer to) supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_HARD_E
*                      CPSS_PACKET_CMD_DROP_SOFT_E
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUnknownMacSaCommandGet
(
    IN  GT_U8                devNum,
    IN  GT_U16               vlanId,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
);


/*******************************************************************************
* cpssDxChBrgVlanEgressPortTagStateModeSet
*
* DESCRIPTION:
*       Set the egress port tag state mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       stateMode   - state mode: eVLAN or ePort
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressPortTagStateModeSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT      stateMode
);

/*******************************************************************************
* cpssDxChBrgVlanEgressPortTagStateModeGet
*
* DESCRIPTION:
*       Get the egress port tag state mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       stateModePtr   - (pointer to) state mode: eVLAN or ePort
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressPortTagStateModeGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    OUT CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT         *stateModePtr
);

/*******************************************************************************
* cpssDxChBrgVlanEgressPortTagStateSet
*
* DESCRIPTION:
*       Determines the egress port state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tagCmd      - Port tag state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressPortTagStateSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT              tagCmd
);

/*******************************************************************************
* cpssDxChBrgVlanEgressPortTagStateGet
*
* DESCRIPTION:
*       Returns the egress port state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       tagCmdPtr   - (pointer to) Port tag state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressPortTagStateGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    OUT CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT                 *tagCmdPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortPushVlanCommandSet
*
* DESCRIPTION:
*       Set Push Vlan Command
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
*       vlanCmd - vlan command
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

GT_STATUS cpssDxChBrgVlanPortPushVlanCommandSet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_NUM                                 portNum,
    IN  CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT    vlanCmd
);

/*******************************************************************************
* cpssDxChBrgVlanPortPushVlanCommandGet
*
* DESCRIPTION:
*       Get Push Vlan Command
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
*
* OUTPUTS:
*       vlanCmdPtr - (pointer to)  push vlan command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushVlanCommandGet
(
    IN  GT_U8                                      devNum,
    IN  GT_PORT_NUM                                portNum,
    OUT CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT   *vlanCmdPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet
*
* DESCRIPTION:
*      Set TPID select table index
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
*       tpidEntryIndex  - TPID select table index
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
GT_STATUS cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           tpidEntryIndex
);

/*******************************************************************************
* cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet
*
* DESCRIPTION:
*      Get TPID select table index
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
*
* OUTPUTS:
*       tpidEntryIndexPtr       - (pointer to) TPID select table index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *tpidEntryIndexPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortPushedTagValueSet
*
* DESCRIPTION:
*      Set the tag value to push
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tagValue    - tag value
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
GT_STATUS cpssDxChBrgVlanPortPushedTagValueSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U16           tagValue
);

/*******************************************************************************
* cpssDxChBrgVlanPortPushedTagValueGet
*
* DESCRIPTION:
*      Get the tag value to push
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
*
* OUTPUTS:
*       tagValuePtr   - (pointer to) tag value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushedTagValueGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U16          *tagValuePtr
);


/*******************************************************************************
* cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet
*
* DESCRIPTION:
*       Determines if UP and CFI values will be assign
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
*       enable      - Enable/Disable
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
GT_STATUS cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet
*
* DESCRIPTION:
*       Get if UP and CFI values will be assign
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
*
* OUTPUTS:
*       enablePtr     - (pointer to) Enable/Disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortUpSet
*
* DESCRIPTION:
*       Set UP value to assign
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
*       up          - UP value
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
GT_STATUS cpssDxChBrgVlanPortUpSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up
);

/*******************************************************************************
* cpssDxChBrgVlanPortUpGet
*
* DESCRIPTION:
*       Get UP value to assign
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
*
* OUTPUTS:
*       upPtr         - (pointer to)  UP value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUpGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *upPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortCfiEnableSet
*
* DESCRIPTION:
*       Set CFI value to assign
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
*       enable  - CFI value
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
GT_STATUS cpssDxChBrgVlanPortCfiEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortCfiEnableGet
*
* DESCRIPTION:
*       Get CFI value to assign
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
*
* OUTPUTS:
*       enablePtr     - (pointer to) CFI value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortCfiEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortNumOfTagWordsToPopSet
*
* DESCRIPTION:
*       Set the number of bytes to pop for traffic that ingress from the port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number
*       numBytesToPop     - enumeration to set the number of bytes to pop.
*                           (for popping 0/4/8/6 bytes )
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
GT_STATUS cpssDxChBrgVlanPortNumOfTagWordsToPopSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT numBytesToPop
);

/*******************************************************************************
* cpssDxChBrgVlanPortNumOfTagWordsToPopGet
*
* DESCRIPTION:
*       Get the number of bytes to pop for traffic that ingress from the port.
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
*       numBytesToPopPtr  - (pointer to) enumeration to set the number of bytes to pop.
*                           (for popping 0/4/8/6 bytes )
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on not supported value read from the HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortNumOfTagWordsToPopGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT  GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT *numBytesToPopPtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortVid0CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing VID0 of the egress packet
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
*       enable  - GT_FALSE: Packet VID0 is not changed.
*                 GT_TRUE:  Assign new VID0 to the egress packet
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
GT_STATUS cpssDxChBrgVlanPortVid0CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortVid0CommandEnableGet
*
* DESCRIPTION:
*       Return if changing VID0 of the egress packet is enabled/disabled
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
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet VID0 is not changed.
*                   GT_TRUE:  Assign new VID0 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid0CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortVid1CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing VID1 of the egress packet
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
*       enable  - GT_FALSE: Packet VID1 is not changed.
*                 GT_TRUE:  Assign new VID1 to the egress packet
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
GT_STATUS cpssDxChBrgVlanPortVid1CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortVid1CommandEnableGet
*
* DESCRIPTION:
*       Return if changing VID1 of the egress packet is enabled/disabled
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
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet VID1 is not changed.
*                   GT_TRUE:  Assign new VID1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid1CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortUp0CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing UP1 of the egress packet
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
*       enable  - GT_FALSE: Packet UP1 is not changed.
*                 GT_TRUE:  Assign new UP1 to the egress packet
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
GT_STATUS cpssDxChBrgVlanPortUp0CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortUp0CommandEnableGet
*
* DESCRIPTION:
*       Return if changing UP0 of the egress packet is enabled/disabled
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
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet UP1 is not changed.
*                   GT_TRUE:  Assign new UP1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp0CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortUp1CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing UP1 of the egress packet
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
*       enable  - GT_FALSE: Packet UP1 is not changed.
*                 GT_TRUE:  Assign new UP1 to the egress packet
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
GT_STATUS cpssDxChBrgVlanPortUp1CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortUp1CommandEnableGet
*
* DESCRIPTION:
*       Return if changing UP1 of the egress packet is enabled/disabled
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
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet UP1 is not changed.
*                   GT_TRUE:  Assign new UP1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp1CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanPortVid1Set
*
* DESCRIPTION:
*       For ingress direction : Set port's default Vid1.
*       For egress direction  : Set port's Vid1 when egress port <VID1 Command>=Enabled
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
*                 In eArch devices portNum is default ePort for ingress direction.
*       direction - ingress/egress direction
*       vid1    - VID1 value (APPLICABLE RANGES: 0..4095)
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
GT_STATUS cpssDxChBrgVlanPortVid1Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN CPSS_DIRECTION_ENT          direction,
    IN  GT_U16           vid1
);

/*******************************************************************************
* cpssDxChBrgVlanPortVid1Get
*
* DESCRIPTION:
*       For ingress direction : Get port's default Vid1.
*       For egress direction  : Get port's Vid1 when egress port <VID1 Command>=Enabled
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
*                 In eArch devices portNum is default ePort for ingress direction.
*       direction - ingress/egress direction
*
* OUTPUTS:
*       vid1Ptr    - (pointer to)VID1 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid1Get
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN CPSS_DIRECTION_ENT          direction,
    OUT  GT_U16           *vid1Ptr
);

/*******************************************************************************
* cpssDxChBrgVlanPortUp0Set
*
* DESCRIPTION:
*       Set Up0 assigned to the egress packet if <UP0 Command>=Enabled
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
*       up0     - UP0 value
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
GT_STATUS cpssDxChBrgVlanPortUp0Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up0
);

/*******************************************************************************
* cpssDxChBrgVlanPortUp0Get
*
* DESCRIPTION:
*       Return the UP0 assigned to the egress packet if <UP0 Command>=Enabled
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
*
* OUTPUTS:
*       up0Ptr - (pointer to) UP0 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp0Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *up0Ptr
);

/*******************************************************************************
* cpssDxChBrgVlanPortUp1Set
*
* DESCRIPTION:
*       Set Up1 assigned to the egress packet if <UP1 Command>=Enabled
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
*       up1     - UP1 value
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
GT_STATUS cpssDxChBrgVlanPortUp1Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up1
);

/*******************************************************************************
* cpssDxChBrgVlanPortUp1Get
*
* DESCRIPTION:
*       Return the UP1 assigned to the egress packet if <UP1 Command>=Enabled
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
*
* OUTPUTS:
*       up1Ptr - (pointer to) UP1 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp1Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *up1Ptr
);

/*******************************************************************************
* cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN Tag1 removing from packets arrived without VLAN Tag1.
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
*       enable  - GT_FALSE - according to global configuration
*                            see cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*                 GT_TRUE  - Tag1 removed if original packet arrived without Tag1
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
GT_STATUS cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of VLAN Tag1 removing from packets arrived without VLAN Tag1.
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
*       enablePtr - (pointer to)
*                 GT_FALSE - according to global configuration
*                            see cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*                 GT_TRUE  - Tag1 removed if original packet arrived without Tag1
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
GT_STATUS cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanIndependentNonFloodVidxEnableSet
*
* DESCRIPTION:
*       Enable/Disable Independent non-flood VIDX
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_FALSE: VIDX portlist is always AND'ed with VID portlist
*                 GT_TRUE:  If packet has VIDX target and VIDX != 0xFFF
*                           (i.e. not flooded to all ports in the VLAN),
*                           then the VIDX portlist is the distribution portlist,
*                           regardless of the VID portlist.
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
GT_STATUS cpssDxChBrgVlanIndependentNonFloodVidxEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanIndependentNonFloodVidxEnableGet
*
* DESCRIPTION:
*       Get Independent non-flood VIDX status.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: VIDX portlist is always AND'ed with VID portlist
*                   GT_TRUE:  If packet has VIDX target and VIDX != 0xFFF
*                           (i.e. not flooded to all ports in the VLAN),
*                           then the VIDX portlist is the distribution portlist,
*                           regardless of the VID portlist.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIndependentNonFloodVidxEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgVlanFcoeForwardingEnableSet
*
* DESCRIPTION:
*       Set FCoE Forwarding Enable.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which FCoE Forwarding enabled/disabled..
*       enable  - FCoE Forwarding Enable
*                 GT_TRUE - enable, GT_FALSE - disable.
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
GT_STATUS cpssDxChBrgVlanFcoeForwardingEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_U16           vlanId,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgVlanUnregisteredIpmEVidxSet
*
* DESCRIPTION:
*       Set Unregistered IPM eVidx Assignment Mode and Value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanId             - VLAN ID.
*       unregIpmEVidxMode  - Unregistered IPM eVidx Assignment Mode.
*       unregIpmEVidx      - Unregistered IPM eVidx Assignment Value.
*                            (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUnregisteredIpmEVidxSet
(
    IN GT_U8                                        devNum,
    IN GT_U16                                       vlanId,
    IN CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT  unregIpmEVidxMode,
    IN GT_U32                                       unregIpmEVidx
);

/*******************************************************************************
* cpssDxChBrgVlanMembersTableIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN Members table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       mode        - table access indexing mode
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
GT_STATUS cpssDxChBrgVlanMembersTableIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChBrgVlanMembersTableIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN Members table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
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
GT_STATUS cpssDxChBrgVlanMembersTableIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChBrgVlanStgIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN table to fetch span state
*       group.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       mode        - table access indexing mode
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
GT_STATUS cpssDxChBrgVlanStgIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChBrgVlanStgIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN table to fetch span state
*       group.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
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
GT_STATUS cpssDxChBrgVlanStgIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChBrgVlanTagStateIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN table to fetch tag state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number (CPU port supported)
*       mode        - table access indexing mode
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
GT_STATUS cpssDxChBrgVlanTagStateIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChBrgVlanTagStateIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN table to fetch tag state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number (CPU port supported)
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
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
GT_STATUS cpssDxChBrgVlanTagStateIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*
* DESCRIPTION:
*       Set the BPE E-Tag Ingress_E-CID_base calculation algorithm
*       for Looped Back Forwarded Bridged Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       forceSrcCidDefault  - GT_TRUE - Ingress_E-CID_base field of E-TAG forced
*                             to get default value configured by
*                             cpssDxChBrgVlanBpeTagMcCfgSet (defaultSrcECid).
*                             GT_FALSE - Ingress_E-CID_base field of E-TAG is pushed VID.
*                             configured by cpssDxChBrgVlanPortPushedTagValueSet
*                             (tagValue) per source ePort.
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
*       For regular multicast packets E-Tag Ingress_E-CID_base is always S_VID.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          forceSrcCidDefault
);

/*******************************************************************************
* cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet
*
* DESCRIPTION:
*       Get the BPE E-Tag Ingress_E-CID_base calculation algorithm
*       for Looped Back Forwarded Bridged Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*
* OUTPUTS:
*       forceSrcCidDefaultPtr - (pointer to)
*                             GT_TRUE - Ingress_E-CID_base field of E-TAG forced
*                             to get default value configured by
*                             cpssDxChBrgVlanBpeTagMcCfgSet (defaultSrcECid).
*                             GT_FALSE - Ingress_E-CID_base field of E-TAG is pushed VID.
*                             configured by cpssDxChBrgVlanPortPushedTagValueSet
*                             (tagValue) per source ePort.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For regular multicast packets E-Tag Ingress_E-CID_base is always S_VID.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *forceSrcCidDefaultPtr
);

/*******************************************************************************
* cpssDxChBrgVlanBpeTagMcCfgSet
*
* DESCRIPTION:
*       Set the BPE E-Tag Configuration for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       eCidOffsetNegative  - GT_TRUE - negative, GT_FALSE - positive
*       eCidOffset          - E-CID Offset negative or positive
*                             for Multicast packets, field
*                             {GRP,E-CID_base} calculated as (eVIDX + eCidOffset)
*                             (APPLICABLE RANGES: 0..0x3FFF)
*       defaultSrcECid      - default value for Ingress_E-CID_base field of E-TAG  -
*                             the value that not filtered by source VM filtering.
*                             Used for all Unicast and some looped back Multicast packets.
*                             About looped back Multicast packets see
*                             cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*                             (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - when one of parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagMcCfgSet
(
    IN  GT_U8             devNum,
    IN  GT_BOOL           eCidOffsetNegative,
    IN  GT_U32            eCidOffset,
    IN  GT_U32            defaultSrcECid
);

/*******************************************************************************
* cpssDxChBrgVlanBpeTagMcCfgGet
*
* DESCRIPTION:
*       Get the BPE E-Tag Configuration for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       eCidOffsetNegativePtr  - (pointer to)GT_TRUE - negative, GT_FALSE - positive
*       eCidOffsetPtr          - (pointer to)E-CID Offset negative or positive
*                                for Multicast packets, field
*                                {GRP,E-CID_base} calculated as (eVIDX + eCidOffset)
*       defaultSrcECidPtr      - default value for Ingress_E-CID_base field of E-TAG  -
*                                the value that not filtered by source VM filtering.
*                                Used for all Unicast and some looped back Multicast packets.
*                                About looped back Multicast packets see
*                                cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagMcCfgGet
(
    IN  GT_U8             devNum,
    OUT GT_BOOL           *eCidOffsetNegativePtr,
    OUT GT_U32            *eCidOffsetPtr,
    OUT GT_U32            *defaultSrcECidPtr
);

/*******************************************************************************
* cpssDxChBrgVlanBpeTagReservedFieldsSet
*
* DESCRIPTION:
*       Set the values for BPE E-Tag Reserved Fields of all packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       reValue             - value for reserved field called "re-"
*                             (APPLICABLE RANGES: 0..3)
*       ingressECidExtValue - value for field "ingress_E-CID_ext"
*                             (APPLICABLE RANGES: 0..0xFF)
*       eCidExtValue        - value for field "E-CID_ext"
*                             (APPLICABLE RANGES: 0..0xFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - when one of parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagReservedFieldsSet
(
    IN  GT_U8             devNum,
    IN  GT_U32            reValue,
    IN  GT_U32            ingressECidExtValue,
    IN  GT_U32            eCidExtValue
);

/*******************************************************************************
* cpssDxChBrgVlanBpeTagReservedFieldsGet
*
* DESCRIPTION:
*       Get the values for BPE E-Tag Reserved Fields of all packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       reValuePtr             - (pointer to)value for reserved field called "re-"
*       ingressECidExtValuePtr - (pointer to)value for field "ingress_E-CID_ext"
*       eCidExtValuePtr        - (pointer to)value for field "E-CID_ext"
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagReservedFieldsGet
(
    IN  GT_U8             devNum,
    OUT GT_U32            *reValuePtr,
    OUT GT_U32            *ingressECidExtValuePtr,
    OUT GT_U32            *eCidExtValuePtr
);

/*******************************************************************************
* cpssDxChBrgVlanFdbLookupKeyModeSet
*
* DESCRIPTION:
*       Set an FDB Lookup key mode
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       vlanId                 - VLAN ID
*       mode                   - FDB Lookup key mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanFdbLookupKeyModeSet
(
    IN GT_U8                                 devNum,
    IN GT_U16                                vlanId,
    IN CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChBrgVlan6BytesTagConfigSet
*
* DESCRIPTION:
*       Set global configurations for 6 bytes v-tag.
*       NOTE: not relevant to standard 8 bytes BPE 802.1BR.
*
* APPLICABLE DEVICES:
*       Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum              - device number
*       isPortExtender   - is this 'port Extender' or 'Control bridge'
*                          GT_TRUE - 'port Extender'
*                          GT_FALSE - 'Control bridge'
*      lBitInSrcId - the bit index of 'L bit' in the srcId field passed from ingress pipe to egress pipe.
*                    (APPLICABLE RANGES: 0..15)
*      dBitInSrcId - the bit index of 'D bit' in the srcId field passed from ingress pipe to egress pipe.
*                    (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter devNum
*       GT_OUT_OF_RANGE          - out of range in value of lBitInSrcId or dBitInSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlan6BytesTagConfigSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  isPortExtender,
    IN GT_U32   lBitInSrcId,
    IN GT_U32   dBitInSrcId
);

/*******************************************************************************
* cpssDxChBrgVlan6BytesTagConfigGet
*
* DESCRIPTION:
*       Get global configurations for 6 bytes v-tag.
*       NOTE: not relevant to standard 8 bytes BPE 802.1BR.
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
*       isPortExtenderPtr   - (pointer to) is this 'port Extender' or 'Control bridge'
*                          GT_TRUE - 'port Extender'
*                          GT_FALSE - 'Control bridge'
*      lBitInSrcIdPtr - (pointer to) the bit index of 'L bit' in the srcId field passed from ingress pipe to egress pipe.
*      dBitInSrcIdPtr - (pointer to) the bit index of 'D bit' in the srcId field passed from ingress pipe to egress pipe.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlan6BytesTagConfigGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *isPortExtenderPtr,
    OUT GT_U32   *lBitInSrcIdPtr,
    OUT GT_U32   *dBitInSrcIdPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgVlanh */

