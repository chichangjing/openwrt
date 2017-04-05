/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenTunnelTypes.h
*
* DESCRIPTION:
*       Generic definitions for tunnel.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

#ifndef __cpssGenTunnelTypesh
#define __cpssGenTunnelTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/*
 * typedef: enum CPSS_TUNNEL_GRE_ETHER_TYPE_ENT
 *
 * Description:
 *      GRE ether type.
 *
 * Enumerations:
 *
 *      CPSS_TUNNEL_GRE0_ETHER_TYPE_E  - GRE type 0 - control ether type.
 *      CPSS_TUNNEL_GRE1_ETHER_TYPE_E  - GRE type 1 - data ether type.
 *
 */
typedef enum
{
    CPSS_TUNNEL_GRE0_ETHER_TYPE_E     = 0,
    CPSS_TUNNEL_GRE1_ETHER_TYPE_E     = 1

} CPSS_TUNNEL_GRE_ETHER_TYPE_ENT;


/*
 * typedef: enum CPSS_TUNNEL_TYPE_ENT
 *
 * Description: Enumeration of tunnel types.
 *
 * Enumerations:
 *
 *  CPSS_TUNNEL_IPV4_OVER_IPV4_E      - ipv4 tunnelled over ipv4
 *                                      passenger protocol is ipv4
 *                                      tunnel protocol is ipv4
 *
 *  CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E  - ipv4 tunnelled over GRE ipv4
 *                                      passenger protocol is ipv4
 *                                      tunnel protocol is GRE ipv4
 *
 *  CPSS_TUNNEL_IPV6_OVER_IPV4_E      - ipv6 tunnelled over ipv4
 *                                      passenger protocol is ipv6
 *                                      tunnel protocol is ipv4
 *
 *  CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E  - ipv6 tunnelled over GRE ipv4
 *                                      passenger protocol is ipv6
 *                                      tunnel protocol is GRE ipv4
 *
 *  CPSS_TUNNEL_IP_OVER_MPLS_E        - ipv4/6 tunnelled over MPLS
 *                                      passenger protocol is ipv4 or ipv6
 *                                      tunnel protocol is MPLS
 *
 *  CPSS_TUNNEL_ETHERNET_OVER_MPLS_E  - ethernet tunnelled over MPLS
 *                                      passenger protocol is ethernet
 *                                      tunnel protocol is MPLS
 *
 *  CPSS_TUNNEL_X_OVER_IPV4_E         - X tunnelled over ipv4
 *                                      passenger protocol is not relevant
 *                                      tunnel protocol is ipv4
 *
 *  CPSS_TUNNEL_X_OVER_GRE_IPV4_E     - X tunnelled over GRE ipv4
 *                                      passenger protocol is not relevant
 *                                      tunnel protocol is GRE ipv4
 *
 *  CPSS_TUNNEL_X_OVER_MPLS_E         - X tunnelled over MPLS
 *                                      passenger protocol is not relevant
 *                                      tunnel protocol is MPLS
 *
 *  CPSS_TUNNEL_IP_OVER_X_E           - IP tunnelled over X
 *                                      passenger protocol is ipv4/6
 *                                      tunnel protocol is not relevant
 *
 *  CPSS_TUNNEL_MAC_IN_MAC_E          - MAC in MAC
 *                                      based on IEEE 802.1ah
 *
 *  CPSS_TUNNEL_TRILL_E                - TRILL
 *                                      based on transparent interconnections
 *                                      of lots of links control protocol
 *
 *  CPSS_TUNNEL_Q_IN_Q_E               - Q in Q
 *                                       tunnel like mechanism for ip/mpls
 *                                       packet routing.
 * CPSS_TUNNEL_GENERIC_IPV4_E          - Generic IPv4 Tunnel-start, provide flexible
 *                                       support for tunneling protocols such as:
 *                                       IP4/6-over-IPv4 tunnels,
 *                                       IP4/6-over-IPv4-GRE tunnels,
 *                                          where the GRE header may include configurable extensions,
 *                                       Ethernet-over-IPv4-GRE tunnels, where the GRE header
 *                                          may include configurable extensions.
 *                                       Ethernet-over-IPv4-UDP tunnels, where the UDP header may
 *                                          be followed by a configurable shim extension.
 *
 * CPSS_TUNNEL_GENERIC_IPV6_E          - Generic IPv6 Tunnel-start, provide flexible
 *                                       support for tunneling protocols such as:
 *                                       IP4/6-over-IPv6 tunnels,
 *                                       IP4/6-over-IPv6-GRE tunnels,
 *                                          where the GRE header may include configurable extensions,
 *                                       Ethernet-over-IPv6-GRE tunnels, where the GRE header
 *                                          may include configurable extensions.
 *                                       Ethernet-over-IPv6-UDP tunnels, where the UDP header may
 *                                          be followed by a configurable shim extension.

 *
 */
typedef enum {

    CPSS_TUNNEL_IPV4_OVER_IPV4_E        ,
    CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E    ,
    CPSS_TUNNEL_IPV6_OVER_IPV4_E        ,
    CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E    ,
    CPSS_TUNNEL_IP_OVER_MPLS_E          ,
    CPSS_TUNNEL_ETHERNET_OVER_MPLS_E    ,
    CPSS_TUNNEL_X_OVER_IPV4_E           ,
    CPSS_TUNNEL_X_OVER_GRE_IPV4_E       ,
    CPSS_TUNNEL_X_OVER_MPLS_E           ,
    CPSS_TUNNEL_IP_OVER_X_E             ,
    CPSS_TUNNEL_MAC_IN_MAC_E            ,
    CPSS_TUNNEL_TRILL_E                 ,
    CPSS_TUNNEL_Q_IN_Q_E                ,
    CPSS_TUNNEL_GENERIC_IPV4_E          ,
    CPSS_TUNNEL_GENERIC_IPV6_E
} CPSS_TUNNEL_TYPE_ENT;

/*
 * typedef: enum CPSS_TUNNEL_PASSENGER_PACKET_ENT
 *
 * Description: Enumeration of tunnel passenger packet type.
 *
 * Enumerations:
 *
 *  CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E             - IPv4 passenger packet
 *  CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E             - IPv6 passenger packet
 *  CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E  - ethernet without CRC passenger packet
 *  CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E     - ethernet with CRC passenger packet
 *
 */
typedef enum
{
    CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E            = 0,
    CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E            = 1,
    CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E = 2,
    CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E    = 3

} CPSS_TUNNEL_PASSENGER_PACKET_ENT;

/*
 * typedef: enum CPSS_TUNNEL_ETHERTYPE_TYPE_ENT
 *
 * Description:
 *      Ethertype type.
 *
 * Enumerations:
 *
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E        - IPv4 GRE 0 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE1_E        - IPv4 GRE 1 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E     - MPLS Ethernet type for Unicast packets
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_MULTICAST_E   - MPLS Ethernet type for Multicast packets
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_MIM_E              - Mac in Mac Ethernet type
 *                                                      (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2)
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_TRILL_E            - TRILL Ethernet type (APPLICABLE DEVICES: Bobcat2; Bobcat3; Caelum)
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE0_E        - IPv6 GRE 0 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE1_E        - IPv6 GRE 1 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E    - User defined 0 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED1_E    - User defined 1 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E    - User defined 2 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED3_E    - User defined 3 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E    - User defined 4 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED5_E    - User defined 5 Ethernet type
 *      CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED6_E    - User defined 6 Ethernet type
 *
 */
typedef enum
{
    CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E = 0,
    CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE1_E = 1,
    CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E = 2,
    CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_MULTICAST_E = 3,
    CPSS_TUNNEL_ETHERTYPE_TYPE_MIM_E = 4,
    CPSS_TUNNEL_ETHERTYPE_TYPE_TRILL_E = 5,
    CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE0_E = 6,
    CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE1_E = 7,
    CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E = 8,
    CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED1_E = 9,
    CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E = 10,
    CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED3_E = 11,
    CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E = 12,
    CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED5_E = 13,
    CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED6_E = 14

} CPSS_TUNNEL_ETHERTYPE_TYPE_ENT;

/*
 * typedef: enum CPSS_TUNNEL_MULTICAST_TYPE_ENT
 *
 * Description: Enumeration of multicast tunnel type.
 *
 * Enumerations:
 *
 *  CPSS_TUNNEL_MULTICAST_IPV4_E             - IPv4 multicast tunnel
 *  CPSS_TUNNEL_MULTICAST_IPV6_E             - IPv6 multicast tunnel
 *  CPSS_TUNNEL_MULTICAST_MPLS_E             - MPLS multicast tunnel
 *  CPSS_TUNNEL_MULTICAST_TRILL_E            - TRILL multicast tunnel
 *  CPSS_TUNNEL_MULTICAST_PBB_E              - PBB multicast tunnel
 *
 */
typedef enum
{
    CPSS_TUNNEL_MULTICAST_IPV4_E,
    CPSS_TUNNEL_MULTICAST_IPV6_E,
    CPSS_TUNNEL_MULTICAST_MPLS_E,
    CPSS_TUNNEL_MULTICAST_TRILL_E,
    CPSS_TUNNEL_MULTICAST_PBB_E
} CPSS_TUNNEL_MULTICAST_TYPE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenTunnelTypesh */

