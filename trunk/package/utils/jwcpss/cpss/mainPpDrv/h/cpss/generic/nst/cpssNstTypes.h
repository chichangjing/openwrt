/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssNstTypes.h
*
* DESCRIPTION:
*       Includes structures definition of Network Shield Technology (NST) module.
*
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#ifndef __cpssNstTypesh
#define __cpssNstTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>

/*
 * typedef: enum GT_NST_AM_PARAM_ENT
 *
 * Description: specifies type of configuration parameter
 *
 * Enumerations:
 *   CPSS_NST_AM_SA_AUTO_LEARNED_E - To configure the security level of
 *                                        source MAC address for FDB entry which
 *                                        is learned in the FDB by automatic
 *                                        learning
 *   CPSS_NST_AM_DA_AUTO_LEARNED_E - To configure the security level of
 *                                        destination MAC address  for FDB
 *                                        entry which is learned in the FDB by
 *                                        automatic learning
 *   CPSS_NST_AM_SA_UNKNOWN_E      - To configure security level for
 *                                         unknown source MAC address
 *   CPSS_NST_AM_DA_UNKNOWN_E      - To configure security level for
 *                                        unknown destination MAC address
 */
typedef enum{
    CPSS_NST_AM_SA_AUTO_LEARNED_E,
    CPSS_NST_AM_DA_AUTO_LEARNED_E,
    CPSS_NST_AM_SA_UNKNOWN_E,
    CPSS_NST_AM_DA_UNKNOWN_E
}CPSS_NST_AM_PARAM_ENT;

/*
 * typedef: enum CPSS_NST_CHECK_ENT
 *
 * Description: Specifies sanity checks.
 *
 * Enumerations:
 * CPSS_NST_CHECK_TCP_SYN_DATA_E           - TCP SYN packets with data check
 * CPSS_NST_CHECK_TCP_OVER_MAC_MC_BC_E     - TCP over MC/BC packets check
 * CPSS_NST_CHECK_TCP_FLAG_ZERO_E          - TCP packets with all flags zero
 *                                                check
 * CPSS_NST_CHECK_TCP_FLAGS_FIN_URG_PSH_E  - TCP packets with the TCP FIN,
 *                                                 URG, and PSH flags check
 * CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E      - TCP packet with the TCP SYN
 *                                                and FIN flags
 * CPSS_NST_CHECK_TCP_FLAGS_SYN_RST_E      - TCP packet with the TCP SYN
 *                                                and RST flags check
 * CPSS_NST_CHECK_TCP_UDP_PORT_ZERO_E      - TCP/UDP packet with a zero
 *                                                source or destination port
 * CPSS_NST_CHECK_TCP_ALL_E                - all TCP checks
 * CPSS_NST_CHECK_FRAG_IPV4_ICMP_E         - IPv4 fragmented packet check
 * CPSS_NST_CHECK_ARP_MAC_SA_MISMATCH_E    - ARP MAC SA Mismatch check
 *
 */
typedef enum{
   CPSS_NST_CHECK_TCP_SYN_DATA_E,
   CPSS_NST_CHECK_TCP_OVER_MAC_MC_BC_E,
   CPSS_NST_CHECK_TCP_FLAG_ZERO_E,
   CPSS_NST_CHECK_TCP_FLAGS_FIN_URG_PSH_E,
   CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E,
   CPSS_NST_CHECK_TCP_FLAGS_SYN_RST_E,
   CPSS_NST_CHECK_TCP_UDP_PORT_ZERO_E,
   CPSS_NST_CHECK_TCP_ALL_E,
   CPSS_NST_CHECK_FRAG_IPV4_ICMP_E,
   CPSS_NST_CHECK_ARP_MAC_SA_MISMATCH_E
}CPSS_NST_CHECK_ENT;


/*
 * typedef: enum CPSS_NST_INGRESS_FRW_FILTER_ENT
 *
 * Description: ingress forwarding restrictions types.
 *
 * Enumerations:
 *  CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E  - to another network port
 *  CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E      - to a CPU port
 *  CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E - to an analyzer port
 *                                          (isn't supported for ExMxPm devices)
 */
typedef enum{
    CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E,
    CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E,
    CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E
} CPSS_NST_INGRESS_FRW_FILTER_ENT;

/*
 * typedef: enum CPSS_NST_EGRESS_FRW_FILTER_ENT
 *
 * Description: egress forwarding restrictions types.
 *
 * Enumerations:
 *  CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E - from the CPU
 *  CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E  - from the bridging engine
 *  CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E   - from the routing engine
 *  CPSS_NST_EGRESS_FRW_FILTER_MPLS_E     - from MPLS engine
 *                                          APPLICABLE DEVICES: Puma2
 *  CPSS_NST_EGRESS_FRW_FILTER_L2VPN_E    - from L2 VPN engine
 *                                          APPLICABLE DEVICES: Puma2
 *
 */
typedef enum{
   CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E,
   CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E,
   CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E,
   CPSS_NST_EGRESS_FRW_FILTER_MPLS_E,
   CPSS_NST_EGRESS_FRW_FILTER_L2VPN_E

} CPSS_NST_EGRESS_FRW_FILTER_ENT;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssNstTypesh */

