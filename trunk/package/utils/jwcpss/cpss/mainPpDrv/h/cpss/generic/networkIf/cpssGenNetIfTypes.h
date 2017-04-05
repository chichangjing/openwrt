/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenNetIfTypes.h
*
* DESCRIPTION:    generic network interface facility types definitions
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#ifndef __cpssGenNetIfTypesh
#define __cpssGenNetIfTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>

/*
 * typedef: CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT
 *
 * Description: used for CPU Code Rate Limiting mode
 *
 * Enumerations:
 *      CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E     - Local rate limiting mode.
 *                                    Rate limiting is done on packets
 *                                    forwarded to CPU only by local device.
 *                                    Trapped or Mirrored To CPU packets
 *                                    goes through cascade links are not
 *                                    affected by rate limiting.
 *      CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E - Aggregate rate limiting mode.
 *                                         limiting is done on all packets
 *                                         forwarded to CPU regardless it's path
 *                                         to CPU (direct or through cascade
 *                                         links).
 *
 */

typedef enum
{
    CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E,
    CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E
} CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT;


/*
 * typedef: CPSS_NET_TCP_UDP_PACKET_TYPE_ENT
 *
 * Description: TCP and UDP Packet Type
 *
 * Enumerations:
 *      CPSS_NET_TCP_UDP_PACKET_UC_E         - TCP or UDP Unicast packets
 *      CPSS_NET_TCP_UDP_PACKET_MC_E         - TCP or UDP Multicast packets
 *      CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E - TCP or UDP  both Unicast and
 *                                         Multicast packets
 */
typedef enum
{
    CPSS_NET_TCP_UDP_PACKET_UC_E,
    CPSS_NET_TCP_UDP_PACKET_MC_E,
    CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E
}CPSS_NET_TCP_UDP_PACKET_TYPE_ENT;


/*
 * Typedef: enumeration CPSS_NET_CPU_PORT_MODE_ENT
 *
 * Description: Defines the possible CPU network interface modes
 *
 * Enumerations:
 *    CPSS_NET_CPU_PORT_MODE_SDMA_E   - CPU Tx/Rx by SDMA
 *    CPSS_NET_CPU_PORT_MODE_MII_E    - CPU Tx/Rx by MII
 *    CPSS_NET_CPU_PORT_MODE_NONE_E   - This device isn't connected to CPU
 */
typedef enum
{
    CPSS_NET_CPU_PORT_MODE_SDMA_E,
    CPSS_NET_CPU_PORT_MODE_MII_E,
    CPSS_NET_CPU_PORT_MODE_NONE_E
} CPSS_NET_CPU_PORT_MODE_ENT;


/*
 * typedef: CPSS_NET_PROT_ENT
 *
 * Description: Protocol type
 *
 * Enumerations:
 *      CPSS_NET_PROT_UDP_E           - UDP packets
 *      CPSS_NET_PROT_TCP_E           - TCP packets
 *      CPSS_NET_PROT_BOTH_UDP_TCP_E  - both TCP and UDP packets
 */
typedef enum
{
    CPSS_NET_PROT_UDP_E,
    CPSS_NET_PROT_TCP_E,
    CPSS_NET_PROT_BOTH_UDP_TCP_E
}CPSS_NET_PROT_ENT;

/*
 * Typedef: struct CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC
 *
 * Description: information about Rx Error counters -- per device
 *
 * Fields:
 *     counterArray - array of CPSS_TC_RANGE_CNS (8) counters.
 *                    each counter dedicated to different Queue (traffic class)
 *                    return value since last read
 *
 */
typedef struct{
    GT_U32    counterArray[CPSS_TC_RANGE_CNS];
}CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC;

/*
 * Typedef: CPSS_NET_RX_CPU_CODE_ENT
 *
 * Description: Defines the different CPU codes that indicate the reason for
 *              sending a packet to the CPU.
 *              This enumeration type is enlarged over 8 bits
 *              for none TWIST PP.
 * Enumerations:
 *
 *  Codes set by Ethernet Bridging Engine:
 *      CPSS_NET_CONTROL_E           - Can be initiated only by another CPU.
 *      CPSS_NET_UN_KNOWN_UC_E       - Unknown Unicast packet.
 *      CPSS_NET_UN_REGISTERD_MC_E   - Unregistered Multicast packet.
 *      CPSS_NET_CONTROL_BPDU_E            - Control BPDU packet.
 *      CPSS_NET_CONTROL_DEST_MAC_TRAP_E   - Destination MAC trapped packet.
 *      CPSS_NET_CONTROL_SRC_MAC_TRAP_E    - Source MAC trapped packet.
 *      CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E - Source & Destination MAC trapped packet.*
 *      CPSS_NET_CONTROL_MAC_RANGE_TRAP_E  - MAC Range Trapped packet.
 *      CPSS_NET_RX_SNIFFER_TRAP_E  - Trapped to CPU due to Rx sniffer
 *      CPSS_NET_INTERVENTION_ARP_E - Intervention ARP
 *      CPSS_NET_INTERVENTION_IGMP_E - Intervention IGMP
 *      CPSS_NET_INTERVENTION_SA_E  - Intervention Source address.
 *      CPSS_NET_INTERVENTION_DA_E  - Intervention Destination address.
 *      CPSS_NET_INTERVENTION_SA_DA_E - Intervention Source and Destination addresses*
 *      CPSS_NET_INTERVENTION_PORT_LOCK_E  - Intervention - Port Lock to CPU.
 *      CPSS_NET_EXP_TNL_BAD_IPV4_HDR_E - obsolete
 *      CPSS_NET_EXP_TNL_BAD_VLAN_E     - obsolete
 *  Codes set by L3- L7 Engines:
 *      CPSS_NET_MLL_RPF_TRAP_E - Trapped Due to MLL entry Multicast RPF Fail command
 *      CPSS_NET_RESERVED_SIP_TRAP_E    - Trapped Due to Reserved SIP.
 *      CPSS_NET_INTERNAL_SIP_TRAP_E    - Trapped Due to Internal SIP.
 *      CPSS_NET_SIP_SPOOF_TRAP_E       - Trapped Due to SIP Spoofing
 *  TCB Codes:
 *      CPSS_NET_DEF_KEY_TRAP_E  - Trap by default Key Entry,after no
 *                                          match was found in the Flow Table.
 *      CPSS_NET_IP_CLASS_TRAP_E  - Trap by Ip Classification Tables
 *      CPSS_NET_CLASS_KEY_TRAP_E - Trap by Classifier's Key entry
 *                                after a match was found in the Flow Table.
 *      CPSS_NET_TCP_RST_FIN_TRAP_E - Packet Mirrored to CPU because
 *                                             of TCP Rst_FIN trapping.
 *      CPSS_NET_CLASS_KEY_MIRROR_E - Packet Mirrored to CPU because
 *                                           of mirror bit in Key entry.
 *      CPSS_NET_TRAP_BY_DEFAULT_ENTRY0_E  - Trap by default entry 0
 *      CPSS_NET_TRAP_BY_DEFAULT_ENTRY1_E  - Trap by default entry 1
 *      CPSS_NET_TRAP_UD_INVALID_E         - Trap due to User Defined
 *                                                    bits not all valid
 *      CPSS_NET_CLASS_MTU_EXCEED_E - Classifier MTU exceed when
 *                                redirecting a packet through the classifier.
 *  Ipv4 Codes:
 *      CPSS_NET_RESERVED_DIP_TRAP_E - Reserved DIP Trapping.
 *      CPSS_NET_MC_BOUNDARY_TRAP_E  - Multicast Boundary Trapping.
 *      CPSS_NET_INTERNAL_DIP_E      - Internal DIP.
 *      CPSS_NET_IP_ZERO_TTL_TRAP_E  - Packet was trapped due to
 *                                          TTL = 0 (valid for IP header only).
 *      CPSS_NET_BAD_IP_HDR_CHECKSUM_E - Bad IP header Checksum.
 *      CPSS_NET_RPF_CHECK_FAILED_E    - Packet did not pass RPF check,
 *                                                need to send prune message.
 *      CPSS_NET_OPTIONS_IN_IP_HDR_E  - Packet with Options in the IP header.
 *      CPSS_NET_END_OF_IP_TUNNEL_E   - Packet which is in the End of
 *                                  an IP tunnel sent for reassembly to the CPU.
 *      CPSS_NET_BAD_TUNNEL_HDR_E     - Bad tunnel header - Bad GRE
 *                       header or packet need to be fragmented with DF bit set.
 *      CPSS_NET_IP_HDR_ERROR_E - IP header contains Errors -
 *                                          version!= 4, Ip header length < 20.
 *      CPSS_NET_ROUTE_ENTRY_TRAP_E - Trap Command was found in the
 *                                            Route Entry.
 *      CPSS_NET_DIP_CHECK_ERROR_E - DIP check error
 *      CPSS_NET_ILLEGAL_DIP_E     - Illegal DIP, class D or mismatch
 *      CPSS_NET_POLICY_TRAP_E     - Trap from policy engine
 *      CPSS_NET_DEFAULT_ROUTE_TRAP_E - Packet is trapped due to default
 *                                              rout entry
 *      CPSS_NET_IP_MTU_EXCEED_E     - IPv4 MTU exceed.
 *  MPLS Codes:
 *      CPSS_NET_MPLS_MTU_EXCEED_E   - MPLS MTU exceed.
 *      CPSS_NET_MPLS_ZERO_TTL_TRAP_E - TTL in the MPLS shim header is 0.
 *      CPSS_NET_NHLFE_ENTRY_TRAP_E  - Trap command was found in the NHLFE.
 *      CPSS_NET_ILLEGAL_POP_E       - Illegal pop operation was done.
 *      CPSS_NET_INVALID_MPLS_IF_E   - Invalid MPLS Interface Entry was fetched.
 *
 *  Tiger used additional Codes:
 *
 *  Tiger IPv6 codes:
 *      CPSS_NET_IPV6_ICMP_MLD_E    - ICNPV6 packet trap
 *      CPSS_NET_IPV6_IGMP_E        - IGMP packet trap
 *      CPSS_NET_IPV6_MC_PCL_E      - IPV6 trap due to MC entry
                                             in Policy Control List
 *      CPSS_NET_IPV6_HOP_BY_HOP_E  - IPV6 trap Hop by Hop
 *      CPSS_NET_IPV6_EXT_HEADER_E  - IPV6 trap extension headers
 *      CPSS_NET_IPV6_BAD_HEADER_E  - IPV6 header error
 *      CPSS_NET_IPV6_ILLIGAL_DIP_E - IPV6 illegal address
 *      CPSS_NET_IPV6_DIP_ERROR_E   - IPV6 DIP check error
 *      CPSS_NET_IPV6_ROUTE_TRAP_E  - IPV6 packet is dropped due
 *                                             to route entry
 *      CPSS_NET_IPV6_ROUTER_RPF_E  - IPV6 packet didn't pass RPF check
 *      CPSS_NET_IPV6_SCOP_FAIL_E   - IPV6 packet failed scope check
 *      CPSS_NET_IPV6_TTL_TRAP_E    - IPV6 trap hop limit
 *      CPSS_NET_IPV6_DEFAULT_ROUTE_E - IPV6 packet is trapped due
                                              to default route entry
 *
 *  Tiger Mirror codes
 *      CPSS_NET_MIRROR_PCL_RST_FIN_E      - policy reset FIN mirror
 *      CPSS_NET_MIRROR_PCL_PCE_E          - policy action mirror
 *      CPSS_NET_MIRROR_BRIDGE_E           - bridge mirror
 *      CPSS_NET_MIRROR_MLD_E              - MLD mirror
 *      CPSS_NET_MIRROR_IGMP_E             - IGMP snooping
 *      CPSS_NET_MIRROR_RIPV1_E            - RIPv1 Mirroring
 *      CPSS_NET_MIRROR_TTL_E              - TTL Mirror
 *      CPSS_NET_MIRROR_RPF_E              - RPF Check Fail Mirror
 *      CPSS_NET_MIRROR_IP_OPTIONS_E       - IP Options Mirror
 *      CPSS_NET_MIRROR_ROUTE_E            - Route Entry Mirror
 *      CPSS_NET_MIRROR_ICMP_E             - ICMP Redirect Mirror
 *      CPSS_NET_MIRROR_HOP_BY_HOP_E       - IP Hop by Hop
 *      CPSS_NET_MIRROR_EXT_HEADER_E       - IP Extension Hdr
 *      CPSS_NET_MIRROR_HEADER_ERROR_E     - IP Header Error
 *      CPSS_NET_MIRROR_ILLIGAL_IP_E       - IP Illegal Address
 *      CPSS_NET_MIRROR_SPECIAL_DIP_E      - IP Special DIP
 *      CPSS_NET_MIRROR_IP_SCOP_E          - IP Scope
 *      CPSS_NET_MIRROR_IP_MTU_E           - IP Mirror MTU
 *      CPSS_NET_MIRROR_INLIF_E            - Mirror due to InLif Entry
 *      CPSS_NET_MIRROR_PCL_MTU_E          - Policy Mirror MTU
 *      CPSS_NET_MIRROR_DEFAULT_ROUTE_E    - Default Route Entry Mirror
 *      CPSS_NET_MIRROR_MC_ROUTE1_E        - MC Route Entry Mirror1
 *      CPSS_NET_MIRROR_MC_ROUTE2_E        - MC Route Entry Mirror2
 *      CPSS_NET_MIRROR_MC_ROUTE3_E        - MC Route Entry Mirror3
 *      CPSS_NET_MIRROR_IPV4_BC_E          - IPv4 MC Broadcast

 *
 *  Egress Pipe Codes:
 *      CPSS_NET_ETH_BRIDGED_LLT_E  - Regular packet passed to CPU,
 *                                Last level treated (LLT) = Ethernet Bridged.
 *      CPSS_NET_IPV4_ROUTED_LLT_E - Regular packet passed to CPU,
 *                                LLT = IPv4 Routed.
 *      CPSS_NET_UC_MPLS_LLT_E    - Regular packet passed to CPU,
 *                                           LLT = Unicast MPLS.
 *      CPSS_NET_MC_MPLS_LLT_E    - Regular packet passed to CPU,
 *                                LLT = Multicast MPLS (currently not supported).
 *      CPSS_NET_IPV6_ROUTED_LLT_E - Regular packet passed to CPU,
 *                                LLT = IPv6 Routed (currently not supported).
 *      CPSS_NET_L2CE_LLT_E - Regular packet passed to CPU, LLT = L2CE.
 *      CPSS_NET_EGRESS_MIRROR_TO_CPU_E    - Egress mirrored to CPU frame,
 *       due to sampling to CPU or CPU being configured as egress analyzer port.
 *
 *
 *
 *  DXSal used additional Codes:
 *      CPSS_NET_UNKNOWN_UC_E              - Unknown Unicast packet
 *      CPSS_NET_UNREG_MC_E                - Unregistered Multicast packet

 *  DXSal, DXCH and EXMXPM used additional Codes:
 *      CPSS_NET_LOCK_PORT_MIRROR_E        - Locked Port Mirror packet
 *
 *  DXCH and EXMXPM used additional Codes:
 *      CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E - Packet with MAC DA in
 *         the IEEE Reserved Multicast range trapped or mirrored to the host CPU
 *      CPSS_NET_IPV6_ICMP_PACKET_E - IPv6 ICMP packet trapped or
 *                                         mirrored to the host CPU.
 *      CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E - IPv4 RIPv1 packet
 *                                                     mirrored to the host CPU.
 *      CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E - IPv6 neighbor solicitation
 *                                   packet trapped or mirrored to the host CPU.
 *      CPSS_NET_IPV4_BROADCAST_PACKET_E  - IPv4 Broadcast packet
 *         assigned to a VLAN with IPv4 Broadcast Command set to Trap or Mirror.
 *      CPSS_NET_NON_IPV4_BROADCAST_PACKET_E - Non IPv4 Broadcast packet
 *     assigned to a VLAN with non IPv4 Broadcast Command set to Trap or Mirror.
 *      CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E - A Multicast packet with a
 *          MAC DA in the CISCO AUI range trapped or mirrored to the host CPU.
 *      CPSS_NET_UNREGISTERED_MULTICAST_E - Non IPv4/IPv6
 *          Unregistered Multicast packet assigned to a VLAN with non IPv4/IPv6
 *          Unregistered Multicast Command set to Trap or Mirror.
 *      CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E  - IPv4 Unregistered
 *          Multicast packet assigned to a VLAN with IPv4 Unregistered Multicast
 *          Command set to Trap or Mirror.
 *      CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E  - IPv6 Unregistered
 *          Multicast packet assigned to a VLAN with IPv6 Unregistered Multicast
 *      CPSS_NET_UNKNOWN_UNICAST_E - Packets with unknown unicast destination
 *          address trapped or mirrored to the CPU due to the
 *          VLAN entry configuration.
 *          Command set to Trap or Mirror.
 *      CPSS_NET_UDP_BC_MIRROR_TRAP0_E - UDP Trap or Mirror 0
 *      CPSS_NET_UDP_BC_MIRROR_TRAP1_E - UDP Trap or Mirror 1
 *      CPSS_NET_UDP_BC_MIRROR_TRAP2_E - UDP Trap or Mirror 2
 *      CPSS_NET_UDP_BC_MIRROR_TRAP3_E - UDP Trap or Mirror 3
 *
 *      CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E - CPU Code=1 for Ieee Reserved Multicast Addrs.
 *      CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E - CPU Code=2 for Ieee Reserved Multicast Addrs.
 *      CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E - CPU Code=3 for Ieee Reserved Multicast Addrs.
 *
 *      CPSS_NET_BRIDGED_PACKET_FORWARD_E - Packet forwarded to
 *                  the host CPU by one of the following engines in the device:
 *                  1. Redirected by the PCL engine to the CPU port number.
 *                  2. MAC DA is associated with the CPU port number.
 *                  3. Private VLAN edge target set to CPU port number.
 *      CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E - Ingress mirrored packets
 *       to the analyzer port, when the ingress analyzer port number is set
 *       to the CPU port number.
 *      CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E - Egress mirrored packets
 *          to the analyzer port, when the egress analyzer port number is set
 *          to the CPU port number.
 *      CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E - A packet sent to the local
 *                                  host CPU as a mail from the neighboring CPU.
 *      CPSS_NET_CPU_TO_CPU_E - A packet sent to the local host CPU,
 *                                from one of the other host CPUs in the system.
 *      CPSS_NET_EGRESS_SAMPLED_E - Packet mirrored to the host CPU by
 *                                         STC mechanism.
 *      CPSS_NET_INGRESS_SAMPLED_E - Packet mirrored to the host CPU by the ingress
 *                                         the egress STC mechanism.
 *      CPSS_NET_INVALID_PCL_KEY_TRAP_E - Packet trapped/mirrored to
 *       the host CPU by the Policy Engine due to the following: User-defined
 *       bytes in the key could not be parsed due to packet header length or its
 *       format.
 *
 *      CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E - IPv4/ Pv6 Link Local
 *                             MC DIP packet trapped or mirrored to the host CPU.
 *      CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E - The packet is
 *                             sent to virtual router port.
 *      CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E - IPv4 Unicast packet, triggered
 *                             for routing, Mirrored to CPU due to ICMP redirect.
 *      CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E - IPv6 Unicast packet, triggered
 *                             for routing, Mirrored to CPU due to ICMP redirect.
 *      CPSS_NET_ROUTED_PACKET_FORWARD_E - packet forwarded to the host CPU by the
 *                             router ("IP to me")
 *      CPSS_NET_IP_DIP_DA_MISMATCH_E - IPv4/6 packet, triggered for routing, is Trapped/Mirrored
 *                             due to a mismatch between the destination IP address
 *                             and the destination MAC address.
 *      CPSS_NET_IP_UC_SIP_SA_MISMATCH_E -  IPv4/6 Unicast packet, triggered for routing,
 *                             is Trapped/Mirrored due to a mismatch between the source
 *                             IP address and its configured ARP MAC address.
 *      CPSS_NET_IPV4_UC_ROUTE1_TRAP_E - IPv4 Unicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 1.
 *      CPSS_NET_IPV4_UC_ROUTE2_TRAP_E - IPv4 Unicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 2.
 *      CPSS_NET_IPV4_UC_ROUTE3_TRAP_E - IPv4 Unicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 3.
 *      CPSS_NET_IPV4_MC_ROUTE0_TRAP_E - IPv4 Multicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 0.
 *      CPSS_NET_IPV4_MC_ROUTE1_TRAP_E - IPv4 Multicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 1.
 *      CPSS_NET_IPV4_MC_ROUTE2_TRAP_E - IPv4 Multicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 2.
 *      CPSS_NET_IPV4_MC_ROUTE3_TRAP_E - IPv4 Multicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 3.
 *      CPSS_NET_IPV6_UC_ROUTE1_TRAP_E - IPv6 Unicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 1.
 *      CPSS_NET_IPV6_UC_ROUTE2_TRAP_E - IPv6 Unicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 2.
 *      CPSS_NET_IPV6_UC_ROUTE3_TRAP_E - IPv6 Unicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 3.
 *      CPSS_NET_IPV6_MC_ROUTE0_TRAP_E - IPv6 Multicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 0.
 *      CPSS_NET_IPV6_MC_ROUTE1_TRAP_E - IPv6 Multicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 1.
 *      CPSS_NET_IPV6_MC_ROUTE2_TRAP_E - IPv6 Multicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 2.
 *      CPSS_NET_IPV6_MC_ROUTE3_TRAP_E - IPv6 Multicast packet, triggered for routing,
 *                             Trapped/Mirrored to CPU from Route entry with <CPUCodeIndex> = 3.
 *      CPSS_NET_IP_UC_RPF_FAIL_E  - IPv4/6 Unicast packet, triggered for routing, is
 *                             Trapped/Mirrored due to Unicast RPF Check failure.
 *      CPSS_NET_ARP_BC_TO_ME_E - ARP Broadcast packet, whose destination IP address is the host
 *                             address of the router, as indicated in the Route entry
 *                             <Trap/Mirror ARP BC to CPU>.
 *      CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E - Ipv4/Ipv6 link local
 *                             multicast CPU Code index=1.
 *      CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E - Ipv4/Ipv6 link local
 *                             multicast CPU Code index=2.
 *      CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E - Ipv4/Ipv6 link local
 *                             multicast CPU Code index=3.
 *      CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E - In Secure Automatic learning,an uknown source
 *                             MAC is trapped to CPU.
 *      CPSS_NET_IPV4_TT_HEADER_ERROR_E - Tunnel-terminated packet is trapped to the CPU
 *                             due to an IPV4 tunnel header error.
 *      CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E - Tunnel-terminated packet is trapped to the CPU
 *                             due to the IPV4 tunnel header, which is fragmented or has options.
 *      CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E - Tunnel-terminated packet is trapped to the CPU
 *                             due to an unsupported GRE tunnel header format.
 *      CPSS_NET_IP_MC_ROUTE_BIDIR_RPF_FAIL_E - Indication that
 *          the packet was trapped/mirrored to the CPU due to the bi-directional
 *          tree RPF check failure.(Router)
 *      CPSS_NET_MET_RED_DROP_CODE_E - Used when a non-conforming
 *          packet is dropped by the Ingress Policer engine.(PLR)
 *      CPSS_NET_ECC_DROP_CODE_E - Used when a packet is dropped
 *          by the Ingress Policer engine due to an ECC error.(IPLR)
 *      CPSS_NET_ARP_REPLY_TO_ME_E - The packet is an ARP reply, and forwarded by the FDB
 *                             entry with <destination port> = CPU port 63,
 *                             and <application specific CPU code> = 1.
 *      CPSS_NET_CPU_TO_ALL_CPUS_E  - The packet is sent with a FROM_CPU DSA tag with
 *                             the <mirror2CPU> bit set and this mirrored to the local CPU.
 *      CPSS_NET_TCP_SYN_TO_CPU_E   - The packet is Trapped, Mirrored, forwarded to the
 *                             CPU port 63, or forwarded to the Virtual Router port 63,
 *                             and the packet is a TCP SYN packet.
 *
 *  Puma used additional Codes:
 *
 *      CPSS_NET_MC_BRIDGED_PACKET_FORWARD_E - An L2 packet is sent to the CPU when
 *                                        it is a memeber of the packet's L2 MC
 *                                        group.
 *      CPSS_NET_MC_ROUTED_PACKET_FORWARD_E - An IP MC packet is sent to the CPU when
 *                                        it is a member of the packet's L2 MC
 *                                        group or from the DIT entry.
 *      CPSS_NET_L2VPN_PACKET_FORWARD_E - An VPLS packet is sent to the CPU from
 *                                        the DIT entry or from the MC group
 *                                        pointed to by DIT entry.
 *      CPSS_NET_MPLS_PACKET_FORWARD_E  - An MPLS packet is sent to the CPU from
 *                                        the DIT entry or from the MC group
 *                                        pointed to by DIT entry.
 *      CPSS_NET_ARP_BC_EGRESS_MIRROR_E - An ARP BC packet is trapped to the CPU
 *                                        by the egress pipe.
 *      CPSS_NET_VPLS_UNREGISTERED_MC_EGRESS_FILTER_E - An VPLS packet that is subject to
 *                                        egress filtering is trapped to the CPU.
 *      CPSS_NET_VPLS_UNKWONW_UC_EGRESS_FILTER_E - An VPLS packet that is subject to
 *                                        egress filtering is trapped to the CPU.
 *      CPSS_NET_VPLS_BC_EGRESS_FILTER_E - An VPLS packet that is subject to
 *                                        egress filtering is trapped to the CPU.
 *      CPSS_NET_MC_CPU_TO_CPU_E       - When packet is sent from the CPU to a
 *                                        MC group in which the CPU is a member
 *                                        as well.
 *      CPSS_NET_PCL_MIRRORED_TO_ANALYZER_E  - Ingress policy mirrored packets to the
 *                                        analyzer port
 *      CPSS_NET_VPLS_UNTAGGED_MRU_FILTER_E - A VPLS triggered packet is trapped
 *                                        since it's BC exceeds the configured
 *                                        untagged mru.
 *      CPSS_NET_TT_MPLS_HEADER_CHECK_E - A tunnel terminated MPLS packet is
 *                                        trapped due to an illegal MPLS header
 *                                        or MPLS command in action entry.
 *      CPSS_NET_TT_MPLS_TTL_EXCEED_E - A tunnel terminated MPLS packet is
 *                                        trapped due to an assigned TTL of ZERO.
 *      CPSS_NET_TTI_MIRROR_E         - A packet is mirrored due to TTI Action
 *                                        Entry Command.
 *      CPSS_NET_MPLS_SRC_FILTERING_E - A VPLS packet is trapped/mirrored due
 *                                        to source local filtering.
 *      CPSS_NET_IPV4_TTL1_EXCEEDED_E - IPv4 packet, triggered for routing
 *                                        Trapped/Mirrored to the CPU due to TTL
 *                                        exceeded.
 *      CPSS_NET_IPV6_HOPLIMIT1_EXCEED_E - IPv4 packet, triggered for routing
 *                                        Trapped/Mirrored to the CPU due to
 *                                        Hop Limit exceeded.
 *
 *
 *
 *      CPSS_NET_FIRST_USER_DEFINED_E   - User-Defined CPU codes.
 *                                        The user may use this range for any
 *                                        application specific purpose.
 *
 *      CPSS_NET_LAST_USER_DEFINED_E    - User-Defined CPU codes.
 *                                        The user may use this range for any
 *                                        application specific purpose.
 *
 *      CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E - first code for non-converted DSA
 *             codes.
 *
 *             NOTE :
 *             this range is reserved for CPU codes from the DSA , that are not
 *             covered by the mapping from "DSA CPU opcode" to
 *             "unified Rx CPU opcode"
 *
 *             so value of CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E + 89
 *             means that the DSA tag was with reason 89 , but since the CPSS SW
 *             do not know what is the meaning of 89 , we let application deal
 *             with it.
 *
 *             because the CPSS not want to "drop" this RX packet !
 *
 *      CPSS_NET_LAST_UNKNOWN_HW_CPU_CODE_E - last code for non-converted DSA
 *             codes
 *
 *      CPSS_NET_ALL_CPU_OPCODES_E - to ensure 32 bit size of type's
 *             variables and for cases of actions general for all cpu opcodes
 *
 *      CPSS_NET_OAM_PDU_TRAP_E    - An OAM-PDU packet is trappedto the CPU
 *
 *      CPSS_NET_IPCL_MIRROR_E     - Packet is mirrored due to IPCL Action
 *                                        Entry Command == Mirror but IPCL AE
 *                                        does not support user defined mirror
 *                                        code due to packet redirect.
 *
 *      CPSS_NET_MPLS_TTL1_EXCEEDED_E - MPLs packet, trigered for routing
 *                                        Trapped to the CPU due to TTL = 1 exceeded
 *
 *
 *      CPSS_NET_MPLS_ROUTE_ENTRY_1_E - MPLs packet, trigered for routing,
 *                                        Trapped/Mirror to CPU from NHLFE
 *                                        entry with <CPUCodeIndex>=1
 *
 *      CPSS_NET_MPLS_ROUTE_ENTRY_2_E - MPLs packet, trigered for routing,
 *                                        Trapped/Mirror to CPU from NHLFE
 *                                        entry with <CPUCodeIndex>=2
 *
 *      CPSS_NET_MPLS_ROUTE_ENTRY_3_E - MPLs packet, trigered for routing,
 *                                        Trapped/Mirror to CPU from NHLFE
 *                                        entry with <CPUCodeIndex>=3
 *      CPSS_NET_PTP_HEADER_ERROR_E - PTP packet received by TTI, and the header is too short,
 *                     i.e., it does not contain some of the necessary fields for PTP parsing.
 *
 *      CPSS_NET_IPV4_6_SIP_FILTERING_E - Indicates that the
 *              source IP address was filtered by the SIP filtering mechanism.(Router)
 *      CPSS_NET_IPV4_6_SIP_IS_ZERO_E   - Indicates that the
 *              source IP address is 0. (Router)
 *      CPSS_NET_ACCESS_MATRIX_E        - Indicates that the
 *              packet was dropped/mirrored/trapped. (Router)
 *      CPSS_NET_FCOE_DIP_LOOKUP_NOT_FOUND_OR_FCOE_EXCEPTION_E -
 *              Indicates an invalid field in the FCoE header Or FCoE DIP was not
 *              found. (Router)
 *      CPSS_NET_FCOE_SIP_NOT_FOUND_E   - Indicates that the
 *              FCoE SIP was not found. (Router)
 *      CPSS_NET_BAD_ANALYZER_INDEX_DROP_ERROR_E - Used in Hop-By-Hop mirroring mode,
 *              when a packet is duplicated to a target analyzer, and the
 *              global Ingress Analyzer Index or Egress Analyzer Index disabled.
 *              In this case the packet is hard dropped with this drop code.
 *              NOTE: Configurable CPU Code with (default 78).
 *      CPSS_NET_RXDMA_DROP_E - The drop code for a packet that
 *          was dropped by the RxDMA and forwarded to the ingress pipe.(RXDMA)
 *      CPSS_NET_TARGET_NULL_PORT_E        - Indicates that no
 *          engine has assigned a target ePort to the current packet,
 *          and thus the packet is dropped.(EQ)
 *      CPSS_NET_TARGET_EPORT_MTU_EXCEEDED_OR_RX_PROTECTION_SWITCHING_OR_INGRESS_FORWARDING_RESTRICTIONS_E -
 *          Used when the packet is trapped or dropped due to an MTU check based on the target ePort in the E2PHY.(EQ)
 *          Or when the packet is dropped due to a protection switching filter.(EQ)
 *          Or when a packet is filtered by the ingress forwarding restrictions feature. (EQ)
 *      CPSS_NET_UNTAGGED_OVERSIZE_DROP_CODE_E - Oversize drop -
 *              used only for drop command. (TTI)
 *      CPSS_NET_IPV6_TT_UNSUP_GRE_ERROR_E - Used by the TTI engine
 *              to indicate an error with the GRE tunnel header on tunnel-termination.(TTI)
 *      CPSS_NET_CCFC_PKT_DROP_E - Indicates the Cross-Chip-Flow-Control packet
 *              was terminated by the device and replaced by a either 802.3x or
 *              PFC flow-control frame. (TTI)
 *      CPSS_NET_BAD_FC_PKT_DROP_E - Used for bad IEEE 802.3x Flow Control packets.(TTI)
 */
typedef enum
{
    /* L2 Bridge */
    CPSS_NET_CONTROL_E                          = 1,
    CPSS_NET_UN_KNOWN_UC_E                      = 10,
    CPSS_NET_UN_REGISTERD_MC_E                  = 11,
    CPSS_NET_CONTROL_BPDU_E                     = 16,
    CPSS_NET_CONTROL_DEST_MAC_TRAP_E            = 17,
    CPSS_NET_CONTROL_SRC_MAC_TRAP_E             = 18,
    CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E         = 19,
    CPSS_NET_CONTROL_MAC_RANGE_TRAP_E           = 20,
    /* Pre-egress engine */
    CPSS_NET_RX_SNIFFER_TRAP_E                  = 21,
    CPSS_NET_INTERVENTION_ARP_E                 = 32,
    CPSS_NET_INTERVENTION_IGMP_E                = 33,
    CPSS_NET_INTERVENTION_SA_E                  = 34,
    CPSS_NET_INTERVENTION_DA_E                  = 35,
    CPSS_NET_INTERVENTION_SA_DA_E               = 36,
    CPSS_NET_INTERVENTION_PORT_LOCK_E           = 37,
    CPSS_NET_EXP_TNL_BAD_IPV4_HDR_E             = 38,
    CPSS_NET_EXP_TNL_BAD_VLAN_E                 = 39,
    /* MLL */
    CPSS_NET_MLL_RPF_TRAP_E                     = 120,
    /* Pre-router */
    CPSS_NET_RESERVED_SIP_TRAP_E                = 128,
    CPSS_NET_INTERNAL_SIP_TRAP_E                = 129,
    CPSS_NET_SIP_SPOOF_TRAP_E                   = 130,
    /* PCL */
    CPSS_NET_DEF_KEY_TRAP_E                     = 132,
    CPSS_NET_IP_CLASS_TRAP_E                    = 133,
    CPSS_NET_CLASS_KEY_TRAP_E                   = 134,
    CPSS_NET_TCP_RST_FIN_TRAP_E                 = 135,
    CPSS_NET_CLASS_KEY_MIRROR_E                 = 136,
    CPSS_NET_TRAP_BY_DEFAULT_ENTRY0_E           = 137,
    CPSS_NET_TRAP_BY_DEFAULT_ENTRY1_E           = 138,
    CPSS_NET_TRAP_UD_INVALID_E                  = 139,
    /* IPv4 */
    CPSS_NET_RESERVED_DIP_TRAP_E                = 144,
    CPSS_NET_MC_BOUNDARY_TRAP_E                 = 145,
    CPSS_NET_INTERNAL_DIP_E                     = 146,
    CPSS_NET_IP_ZERO_TTL_TRAP_E                 = 147,
    CPSS_NET_BAD_IP_HDR_CHECKSUM_E              = 148,
    CPSS_NET_RPF_CHECK_FAILED_E                 = 149,
    CPSS_NET_OPTIONS_IN_IP_HDR_E                = 150,
    CPSS_NET_END_OF_IP_TUNNEL_E                 = 151,
    CPSS_NET_BAD_TUNNEL_HDR_E                   = 152,
    CPSS_NET_IP_HDR_ERROR_E                     = 153,
    CPSS_NET_ROUTE_ENTRY_TRAP_E                 = 154,
    CPSS_NET_DIP_CHECK_ERROR_E                  = 155,
    CPSS_NET_ILLEGAL_DIP_E                      = 156,
    CPSS_NET_POLICY_TRAP_E                      = 157,
    CPSS_NET_DEFAULT_ROUTE_TRAP_E               = 158,
    /*MTU Exceeded */
    CPSS_NET_IP_MTU_EXCEED_E                    = 161,
    CPSS_NET_MPLS_MTU_EXCEED_E                  = 162,
    CPSS_NET_CLASS_MTU_EXCEED_E                 = 163,
    /*Mpls */
    CPSS_NET_MPLS_ZERO_TTL_TRAP_E               = 171,
    CPSS_NET_NHLFE_ENTRY_TRAP_E                 = 172,
    CPSS_NET_ILLEGAL_POP_E                      = 173,
    CPSS_NET_INVALID_MPLS_IF_E                  = 174,

    /* Start of additional Values for Tiger use only */
    /* Tiger IPv6 codes */
    CPSS_NET_IPV6_ICMP_MLD_E                    = 175,
    CPSS_NET_IPV6_IGMP_E                        = 176,
    CPSS_NET_IPV6_MC_PCL_E                      = 186,
    CPSS_NET_IPV6_HOP_BY_HOP_E                  = 187,
    CPSS_NET_IPV6_EXT_HEADER_E                  = 188,
    CPSS_NET_IPV6_BAD_HEADER_E                  = 189,
    CPSS_NET_IPV6_ILLIGAL_DIP_E                 = 190,
    CPSS_NET_IPV6_DIP_ERROR_E                   = 191,
    CPSS_NET_IPV6_ROUTE_TRAP_E                  = 192,
    CPSS_NET_IPV6_ROUTER_RPF_E                  = 193,
    CPSS_NET_IPV6_SCOP_FAIL_E                   = 194,
    CPSS_NET_IPV6_TTL_TRAP_E                    = 195,
    CPSS_NET_IPV6_DEFAULT_ROUTE_E               = 196,
    /* Tiger Mirror codes */
    CPSS_NET_MIRROR_PCL_RST_FIN_E               = 201,
    CPSS_NET_MIRROR_PCL_PCE_E                   = 202,
    CPSS_NET_MIRROR_BRIDGE_E                    = 203,
    CPSS_NET_MIRROR_MLD_E                       = 204,
    CPSS_NET_MIRROR_IGMP_E                      = 205,
    CPSS_NET_MIRROR_RIPV1_E                     = 206,
    CPSS_NET_MIRROR_TTL_E                       = 207,
    CPSS_NET_MIRROR_RPF_E                       = 208,
    CPSS_NET_MIRROR_IP_OPTIONS_E                = 209,
    CPSS_NET_MIRROR_ROUTE_E                     = 210,
    CPSS_NET_MIRROR_ICMP_E                      = 211,
    CPSS_NET_MIRROR_HOP_BY_HOP_E                = 212,
    CPSS_NET_MIRROR_EXT_HEADER_E                = 213,
    CPSS_NET_MIRROR_HEADER_ERROR_E              = 214,
    CPSS_NET_MIRROR_ILLIGAL_IP_E                = 215,
    CPSS_NET_MIRROR_SPECIAL_DIP_E               = 216,
    CPSS_NET_MIRROR_IP_SCOP_E                   = 217,
    CPSS_NET_MIRROR_IP_MTU_E                    = 218,
    CPSS_NET_MIRROR_INLIF_E                     = 219,
    CPSS_NET_MIRROR_PCL_MTU_E                   = 220,
    CPSS_NET_MIRROR_DEFAULT_ROUTE_E             = 221,
    CPSS_NET_MIRROR_MC_ROUTE1_E                 = 222,
    CPSS_NET_MIRROR_MC_ROUTE2_E                 = 223,
    CPSS_NET_MIRROR_MC_ROUTE3_E                 = 224,
    CPSS_NET_MIRROR_IPV4_BC_E                   = 225,
    /*End of additional Values for Tiger use only */

    /*Egress pipe */
    CPSS_NET_ETH_BRIDGED_LLT_E                  = 248,
    CPSS_NET_IPV4_ROUTED_LLT_E                  = 249,
    CPSS_NET_UC_MPLS_LLT_E                      = 250,
    CPSS_NET_MC_MPLS_LLT_E                      = 251,
    CPSS_NET_IPV6_ROUTED_LLT_E                  = 252,
    CPSS_NET_L2CE_LLT_E                         = 253,
    CPSS_NET_EGRESS_MIRROR_TO_CPU_E             = 254,

    /* addition values for DXSal devices use only */
    CPSS_NET_UNKNOWN_UC_E                       = 261,
    CPSS_NET_UNREG_MC_E                         = 262,

    /* additional values for DXSal,DXCH and EXMXPM devices */
    CPSS_NET_LOCK_PORT_MIRROR_E                 = 271,

    /* addition values for DXCH and EXMXPM devices */
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E        = 300,
    CPSS_NET_IPV6_ICMP_PACKET_E                 = 301,
    CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E         = 302,
    CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E       = 303,
    CPSS_NET_IPV4_BROADCAST_PACKET_E            = 304,
    CPSS_NET_NON_IPV4_BROADCAST_PACKET_E        = 305,
    CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E        = 306,
    CPSS_NET_UNREGISTERED_MULTICAST_E           = 307,
    CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E      = 308,
    CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E      = 309,
    CPSS_NET_UNKNOWN_UNICAST_E                  = 310,

    CPSS_NET_UDP_BC_MIRROR_TRAP0_E              = 322,
    CPSS_NET_UDP_BC_MIRROR_TRAP1_E              = 323,
    CPSS_NET_UDP_BC_MIRROR_TRAP2_E              = 324,
    CPSS_NET_UDP_BC_MIRROR_TRAP3_E              = 325,

    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E      = 326,
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E      = 327,
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E      = 328,

    CPSS_NET_BRIDGED_PACKET_FORWARD_E           = 400,
    CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E      = 401,
    CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E       = 402,
    CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E           = 403,
    CPSS_NET_CPU_TO_CPU_E                       = 404,
    CPSS_NET_EGRESS_SAMPLED_E                   = 405,
    CPSS_NET_INGRESS_SAMPLED_E                  = 406,
    CPSS_NET_INVALID_PCL_KEY_TRAP_E             = 409,

    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E = 410,
    CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E       = 411,
    CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E        = 412,
    CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E        = 413,
    CPSS_NET_ROUTED_PACKET_FORWARD_E               = 414,
    CPSS_NET_IP_DIP_DA_MISMATCH_E                  = 415,
    CPSS_NET_IP_UC_SIP_SA_MISMATCH_E               = 416,

    CPSS_NET_IPV4_UC_ROUTE1_TRAP_E                 = 417,
    CPSS_NET_IPV4_UC_ROUTE2_TRAP_E                 = 418,
    CPSS_NET_IPV4_UC_ROUTE3_TRAP_E                 = 419,
    CPSS_NET_IPV4_MC_ROUTE0_TRAP_E                 = 420,
    CPSS_NET_IPV4_MC_ROUTE1_TRAP_E                 = 421,
    CPSS_NET_IPV4_MC_ROUTE2_TRAP_E                 = 422,
    CPSS_NET_IPV4_MC_ROUTE3_TRAP_E                 = 423,

    CPSS_NET_IPV6_UC_ROUTE1_TRAP_E                 = 424,
    CPSS_NET_IPV6_UC_ROUTE2_TRAP_E                 = 425,
    CPSS_NET_IPV6_UC_ROUTE3_TRAP_E                 = 426,
    CPSS_NET_IPV6_MC_ROUTE0_TRAP_E                 = 427,
    CPSS_NET_IPV6_MC_ROUTE1_TRAP_E                 = 428,
    CPSS_NET_IPV6_MC_ROUTE2_TRAP_E                 = 430,
    CPSS_NET_IPV6_MC_ROUTE3_TRAP_E                 = 431,
    CPSS_NET_IP_UC_RPF_FAIL_E                      = 432,
    CPSS_NET_ARP_BC_TO_ME_E                        = 433,

    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E = 434,
    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E = 435,
    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E = 436,
    CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E           = 437,
    CPSS_NET_IPV4_TT_HEADER_ERROR_E                  = 438,
    CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E             = 439,
    CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E               = 440,
    CPSS_NET_ARP_REPLY_TO_ME_E                       = 441,
    CPSS_NET_CPU_TO_ALL_CPUS_E                       = 442,
    CPSS_NET_TCP_SYN_TO_CPU_E                        = 443,

    CPSS_NET_MC_BRIDGED_PACKET_FORWARD_E             = 445,
    CPSS_NET_MC_ROUTED_PACKET_FORWARD_E              = 446,
    CPSS_NET_L2VPN_PACKET_FORWARD_E                  = 447,
    CPSS_NET_MPLS_PACKET_FORWARD_E                   = 448,
    CPSS_NET_ARP_BC_EGRESS_MIRROR_E                  = 451,
    CPSS_NET_VPLS_UNREGISTERED_MC_EGRESS_FILTER_E    = 452,
    CPSS_NET_VPLS_UNKWONW_UC_EGRESS_FILTER_E         = 453,
    CPSS_NET_VPLS_BC_EGRESS_FILTER_E                 = 454,
    CPSS_NET_MC_CPU_TO_CPU_E                         = 455,
    CPSS_NET_PCL_MIRRORED_TO_ANALYZER_E              = 456,
    CPSS_NET_VPLS_UNTAGGED_MRU_FILTER_E              = 457,
    CPSS_NET_TT_MPLS_HEADER_CHECK_E                  = 458,
    CPSS_NET_TT_MPLS_TTL_EXCEED_E                    = 459,
    CPSS_NET_TTI_MIRROR_E                            = 460,
    CPSS_NET_MPLS_SRC_FILTERING_E                    = 461,
    CPSS_NET_IPV4_TTL1_EXCEEDED_E                    = 462,
    CPSS_NET_IPV6_HOPLIMIT1_EXCEED_E                 = 463,

    CPSS_NET_OAM_PDU_TRAP_E                          = 464,
    CPSS_NET_IPCL_MIRROR_E                           = 465,
    CPSS_NET_MPLS_TTL1_EXCEEDED_E                    = 468,
    CPSS_NET_MPLS_ROUTE_ENTRY_1_E                    = 470,
    CPSS_NET_MPLS_ROUTE_ENTRY_2_E                    = 471,
    CPSS_NET_MPLS_ROUTE_ENTRY_3_E                    = 472,

    CPSS_NET_PTP_HEADER_ERROR_E                      = 473,

    CPSS_NET_IPV4_6_SIP_FILTERING_E                        = 474,
    CPSS_NET_IPV4_6_SIP_IS_ZERO_E                          = 475,
    CPSS_NET_ACCESS_MATRIX_E                               = 476,
    CPSS_NET_FCOE_DIP_LOOKUP_NOT_FOUND_OR_FCOE_EXCEPTION_E = 477,
    CPSS_NET_FCOE_SIP_NOT_FOUND_E                          = 478,

    CPSS_NET_IP_MC_ROUTE_BIDIR_RPF_FAIL_E                  = 479,
    CPSS_NET_MET_RED_DROP_CODE_E                           = 480,
    CPSS_NET_ECC_DROP_CODE_E                               = 481,

    CPSS_NET_BAD_ANALYZER_INDEX_DROP_ERROR_E               = 482,

    CPSS_NET_RXDMA_DROP_E                                  = 483,
    CPSS_NET_TARGET_NULL_PORT_E                            = 484,
    CPSS_NET_TARGET_EPORT_MTU_EXCEEDED_OR_RX_PROTECTION_SWITCHING_OR_INGRESS_FORWARDING_RESTRICTIONS_E = 485,
    CPSS_NET_UNTAGGED_OVERSIZE_DROP_CODE_E                 = 486,
    CPSS_NET_IPV6_TT_UNSUP_GRE_ERROR_E                     = 487,
    CPSS_NET_CCFC_PKT_DROP_E                               = 488,
    CPSS_NET_BAD_FC_PKT_DROP_E                             = 489,

    CPSS_NET_FIRST_USER_DEFINED_E               = 500,
    CPSS_NET_LAST_USER_DEFINED_E = (CPSS_NET_FIRST_USER_DEFINED_E + 63),

    CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E            = 1024,
    CPSS_NET_LAST_UNKNOWN_HW_CPU_CODE_E             = (CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E + 255),

    CPSS_NET_ALL_CPU_OPCODES_E                  = 0x7FFFFFFF
}CPSS_NET_RX_CPU_CODE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __cpssGenNetIfTypesh */


