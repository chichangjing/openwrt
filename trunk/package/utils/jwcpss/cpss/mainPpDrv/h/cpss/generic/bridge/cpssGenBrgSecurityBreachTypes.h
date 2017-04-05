/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenBrgSecurityBreachTypes.h
*
* DESCRIPTION:    Common Bridge Security Breach definitions
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __cpssGenBrgSecurityBreachTypesh
#define __cpssGenBrgSecurityBreachTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>

/*
 * typedef: enum CPSS_BRG_SECUR_BREACH_EVENTS_ENT
 *
 * Description: Enumeration identifies the type of security breach reported
 *
 * Enumerations:
 *      CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E        - FDB entry command Soft or
 *                                                Hard Drop
 *      CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NA_E          - port new address is a
 *                                                security breach
 *      CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E   - invalid MAC source address
 *      CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E     - vlan not valid
 *      CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E - port not member in vlan
 *      CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E  - vlan range drop
 *      CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E     - moved static address is a
 *                                                security breach
 *      CPSS_BRG_SECUR_BREACH_EVENTS_ARP_SA_MISMATCH_E  - ARP header MAC SA and L2
 *                                                   header MAC SA Mismatch
 *      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_E          - TCP SYN packets with data
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_OVER_MC_E      - TCP over Multicast/Broadcast
 *                                                        packets
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_BRIDGE_ACCESS_MATRIX_E - Bridge Access Matrix table
 *                                                   drop configuration
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_SECURE_AUTO_LEARN_E   - the source MAC address is
 *                                                   NOT found in the FDB
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE         - Packet that does not match
 *                                                   the Acceptable Frame Type
 *                                                   configuration
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_FRAGMENT_ICMP_TYPE_E  - IPv4 fragmented packet
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E    - TCP packet with the flags
 *                                                   all set to zero
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FIN_URG_PSH_E  - TCP packets with the TCP
 *                                                   FIN, URG, and PSH flags
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_FIN_E      - TCP packet with the TCP SYN
 *                                                   and FIN flags
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_RST_E      - TCP packet with the TCP SYN
 *                                                   and RST flags
 *
 *      CPSS_BRG_SECUR_BREACH_EVENTS_TCP_UDP_SRC_DST_PORT_ZERO - TCP/UDP packet with
 *                                                   a zero  source or destination port
 *      CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E          - vlan entry new address is a
 *                                                security breach
 *      CPSS_BRG_SECUR_BREACH_EVENTS_AUTO_LEARN_NO_RELEARN_E - MAC SA is found
 *                                           in FDB, but changed it's location
 */
typedef enum {

    CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E = 1,
    CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NA_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_ARP_SA_MISMATCH_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_TCP_OVER_MC_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_BRIDGE_ACCESS_MATRIX_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_SECURE_AUTO_LEARN_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE,
    CPSS_BRG_SECUR_BREACH_EVENTS_FRAGMENT_ICMP_TYPE_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FLAG_ZERO_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_TCP_FIN_URG_PSH_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_FIN_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_TCP_SYN_RST_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_TCP_UDP_SRC_DST_PORT_ZERO,
    CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_NA_E,
    CPSS_BRG_SECUR_BREACH_EVENTS_AUTO_LEARN_NO_RELEARN_E
} CPSS_BRG_SECUR_BREACH_EVENTS_ENT;

/*
 * typedef: struct CPSS_BRG_SECUR_BREACH_MSG_STC
 *
 * Description: Security Breach Message
 *
 * Fields:
 *      macSa   - Source MAC Address of the breaching packet
 *      vlan    - The VID assigned to the breaching packet
 *      port    - Source port on which the security breach packet was received
 *      code    - security breach code, that the security breach event was
 *                reported with
 */
typedef struct {

    GT_ETHERADDR                        macSa;
    GT_U16                              vlan;
    GT_PORT_NUM                         port;
    CPSS_BRG_SECUR_BREACH_EVENTS_ENT    code;

} CPSS_BRG_SECUR_BREACH_MSG_STC;

/*
 * typedef: enum CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT
 *
 * Description: Enumeration of security breach drop port/vlan counter mode
 *
 * Enumerations:
 *      CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E - Port counter - counts all
 *                          packets received on SecurityBreach DropCntPort
 *                          and dropped due to security breach.
 *      CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E - Vlan counter - counts all
 *                          packets assigned with VID from SecurityBreach
 *                          DropCntVID and dropped due to security breach.
 *
 */
typedef enum
{
    CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E,
    CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E

} CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT;

/*
 * typedef: struct CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC
 *
 * Description: Security Breach Port/Vlan counter configuration entry
 *
 * Fields:
 *      dropCntMode - security breach port/vlan drop counter port or vlan mode
 *      port        - port number
 *                    If the drop count mode is Port, then all packets received
 *                    on this port and dropped due to security breach are
 *                    counted by the Security Breach Drop counter.
 *      vlan        - vlan ID. If the drop counter mode is VID, then all packets
 *                    assigned with this VID and dropped due to security breach
 *                    are counted by the Security Breach Drop counter.
 */
typedef struct {

    CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT   dropCntMode;
    GT_PHYSICAL_PORT_NUM                        port;
    GT_U16                                      vlan;

} CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenBrgSecurityBreachTypesh */


