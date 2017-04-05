/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenBrgVlanTypes.h
*
* DESCRIPTION:    Common VLAN facility types definitions
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#ifndef __cpssGenBrgVlanTypesh
#define __cpssGenBrgVlanTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>


/*
 * typedef: enum CPSS_UNREG_MC_EGR_FILTER_CMD_ENT
 *
 * Description: Enumeration of egress filtering modes for
 *              Unknown unicast and multicast packets
 *
 * Enumerations:
 *      CPSS_UNREG_MC_VLAN_FRWD_E - forward according to vlan port membership
 *      CPSS_UNREG_MC_VLAN_CPU_FRWD_E -
 *                          forward according to vlan port membership and to CPU
 *                          (even if CPU is NOT a member of a VLAN)
 *      CPSS_UNREG_MC_CPU_FRWD_E - forward to CPU only
 *                         (even if CPU is NOT a member of a VLAN)
 *      CPSS_UNREG_MC_NOT_CPU_FRWD_E  -
 *                          forward according to vlan port membership and NOT to
 *                          CPU (even if CPU is a member of a VLAN)
 */
typedef enum
{
    CPSS_UNREG_MC_VLAN_FRWD_E,
    CPSS_UNREG_MC_VLAN_CPU_FRWD_E,
    CPSS_UNREG_MC_CPU_FRWD_E,
    CPSS_UNREG_MC_NOT_CPU_FRWD_E
} CPSS_UNREG_MC_EGR_FILTER_CMD_ENT;

/*
 * typedef: enum CPSS_VLAN_TAG_TYPE_ENT
 *
 * Description: Enumeration of packet Vlan Tags
 *
 * Enumerations:
 *      CPSS_VLAN_TAG0_E  - 0-th Vlan Tag
 *      CPSS_VLAN_TAG1_E  - 1-th Vlan Tag
 */
typedef enum
{
    CPSS_VLAN_TAG0_E,
    CPSS_VLAN_TAG1_E
} CPSS_VLAN_TAG_TYPE_ENT;


/*
 * typedef: struct CPSS_VLAN_INFO_STC
 *
 * Description: A structure to hold generic VLAN entry info.
 *
 * Fields:
 *      hasLocals           - GT_FALSE, if VLAN does not have any local member
 *                            ports on this device;
 *      hasUplinks          - GT_FALSE, if VLAN does not have port members
 *                            residing on remote device;
 *      isCpuMember         - GT_TRUE, if CPU is a member of this VLAN
 *      learnEnable         - GT_TRUE, if enabled leraning of MAC SA for this VLAN
 *      unregMcFilterCmd    - CPSS_EXMX_UNREG_MC_VLAN_FRWD_E     0x0
 *                            CPSS_EXMX_UNREG_MC_VLAN_CPU_FRWD_E 0x1
 *                            CPSS_EXMX_UNREG_MC_CPU_FRWD_E      0x2
 *                            CPSS_EXMX_UNREG_MC_NOT_CPU_FRWD_E  0x3
 *      perVlanSaMacEnabled - TRUE if SA MAC per VLAN is in use
 *      saMacSuffix         - SA MAC suffix in bit 10:5 of VLAN entry.
 *                            Relevant only if isSaMacEnabled == TRUE.
 *      stpId               - STP group, range 0-11
 */
typedef struct
{
    GT_BOOL                                 hasLocals;
    GT_BOOL                                 hasUplinks;
    GT_BOOL                                 isCpuMember;
    GT_BOOL                                 learnEnable;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT        unregMcFilterCmd;
    GT_BOOL                                 perVlanSaMacEnabled;
    GT_U8                                   saMacSuffix;
    GT_U32                                  stpId;

}CPSS_VLAN_INFO_STC;

/*
* typedef: enum CPSS_ETHER_MODE_ENT
*
* Description: Enumeration of VLAN etherType modes
*
* Enumerations:
*       CPSS_VLAN_ETHERTYPE0_E - vlan0 used for EtherType associated with
*                                standard VLAN Tagged Packets
*       CPSS_VLAN_ETHERTYPE1_E - vlan1 used for Nested VLAN configuration and
*                                BackEnd configuration
*/
typedef enum
{
    CPSS_VLAN_ETHERTYPE0_E = 1,
    CPSS_VLAN_ETHERTYPE1_E
}CPSS_ETHER_MODE_ENT;


/*
 * typedef: struct CPSS_PROT_CLASS_ENCAP_STC
 *
 * Description: Structure of encapsulation formats for Protocol based
 *              VLAN classification
 *
 * Enumerations:
 *    ethV2      -  EthernetV2 encapsulation
 *    nonLlcSnap -  non-SNAP LLC encapsulation
 *    llcSnap    -  LLC/SNAP encapsulation
 *
 * COMMENT: REMOVE
 *          This struct is defined localy but should be removed after it will
 *          be defined in the cpssExMxBrgClass.h
 *
 */
typedef struct
{
    GT_BOOL ethV2;
    GT_BOOL nonLlcSnap;
    GT_BOOL llcSnap;
}CPSS_PROT_CLASS_ENCAP_STC;

/*
 * typedef: enum CPSS_STP_STATE_ENT
 *
 * Description: Enumeration of STP Port state
 *
 * Enumerations:
 *      CPSS_STP_DISABLED_E   -  STP disabled on this port.
 *      CPSS_STP_BLCK_LSTN_E  -  blocking/Listening.
 *      CPSS_STP_LRN_E        -  learning.
 *      CPSS_STP_FRWRD_E      -  forwarding.
 */
typedef enum
{
    CPSS_STP_DISABLED_E = 0,
    CPSS_STP_BLCK_LSTN_E,
    CPSS_STP_LRN_E,
    CPSS_STP_FRWRD_E
} CPSS_STP_STATE_ENT;

/*
 * typedef: enum CPSS_PORT_ACCEPT_FRAME_TYPE_ENT
 *
 * Description: Enumeration of port frames discard modes
 *
 * Enumerations:
 *           CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
 *                        All Untagged/Priority Tagged packets received on this
 *                        port are discarded. Only Tagged accepted.
 *           CPSS_PORT_ACCEPT_FRAME_ALL_E       -
 *                        Both Tagged and Untagged packets are accepted on the
 *                        port.
 *           CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
 *                        All Tagged packets received on this port are
 *                        discarded. Only Untagged/Priority Tagged packets
 *                        accepted.
 *           CPSS_PORT_ACCEPT_FRAME_NONE_E      -
 *                        Both Tagged and Untagged packets are discarded on the
 *                        port.
 */
typedef enum
{
    CPSS_PORT_ACCEPT_FRAME_TAGGED_E = 0,
    CPSS_PORT_ACCEPT_FRAME_ALL_E,
    CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E,
    CPSS_PORT_ACCEPT_FRAME_NONE_E
}CPSS_PORT_ACCEPT_FRAME_TYPE_ENT;

/*
 * Typedef: enum CPSS_BRG_MODE_ENT
 *
 * Description:
 *      This enum defines bridging mode
 * Fields:
 *     CPSS_BRG_MODE_802_1Q_E - 802.1Q Bridge, VLAN-aware mode
 *     CPSS_BRG_MODE_802_1D_E - 802.1Q Bridge, VLAN-unaware mode
 *
 *  Comments:
 *          In VLAN-unaware mode, the device does not perform any packet
 *          modifications. Packets are always transmitted as-received, without
 *          any modification (i.e., packets received tagged are transmitted
 *          tagged; packets received untagged are transmitted untagged).
 *          Packets are implicitly assigned with VLAN-ID 1, regardless of
 *          VLAN-assignment mechanisms. Packets are implicitly assigned a
 *          VIDX Multicast group index 0xFFF, indicating that the packet flood
 *          domain is according to the VLAN-in this case VLAN 1.
 *          Registered Multicast is not supported in this mode.
 *          All other features are operational in this mode,
 *          including internal packet QoS, trapping, filtering, policy, etc.
 *
 */
typedef enum
{
    CPSS_BRG_MODE_802_1Q_E,
    CPSS_BRG_MODE_802_1D_E
} CPSS_BRG_MODE_ENT;

/*
 * typedef: enum CPSS_BRG_TPID_SIZE_TYPE_ENT
 *
 * Description: Enumeration of packet TPID size type
 *
 * Enumerations:
 *      CPSS_BRG_TPID_SIZE_TYPE_4_BYTES_E  - TPID size is 4 bytes
 *      CPSS_BRG_TPID_SIZE_TYPE_8_BYTES_E  - TPID size is 8 bytes
 *      CPSS_BRG_TPID_SIZE_TYPE_6_BYTES_E  - TPID size is 6 bytes
 */
typedef enum
{
    CPSS_BRG_TPID_SIZE_TYPE_4_BYTES_E,
    CPSS_BRG_TPID_SIZE_TYPE_8_BYTES_E,
    CPSS_BRG_TPID_SIZE_TYPE_6_BYTES_E
} CPSS_BRG_TPID_SIZE_TYPE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __cpssGenBrgVlanTypesh */


