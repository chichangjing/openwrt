/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssCommonDefs.h
*
* DESCRIPTION:
*       Includes common definitions for all Prestera Sw layers.
*
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __cpssCommonDefsh
#define __cpssCommonDefsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * typedef: enum CPSS_L4_PROTOCOL_ENT
 *
 * Description: general L4 protocol
 *
 * Enumerators:
 *   CPSS_L4_PROTOCOL_TCP_E - TCP
 *   CPSS_L4_PROTOCOL_UDP_E - UDP
  */
typedef  enum
{
    CPSS_L4_PROTOCOL_TCP_E = 6,
    CPSS_L4_PROTOCOL_UDP_E = 17
    /* add other protocols */
} CPSS_L4_PROTOCOL_ENT;

/*
 * typedef: enum CPSS_L4_PROTOCOL_PORT_TYPE_ENT
 *
 * Description: type of TCP/UDP port
 *
 * Enumerators:
 *   CPSS_L4_PROTOCOL_PORT_SRC_E - source port
 *   CPSS_L4_PROTOCOL_PORT_DST_E - destination port
  */
typedef  enum
{
    CPSS_L4_PROTOCOL_PORT_SRC_E,
    CPSS_L4_PROTOCOL_PORT_DST_E
} CPSS_L4_PROTOCOL_PORT_TYPE_ENT;

/*
 * typedef: enum CPSS_COMPARE_OPERATOR_ENT
 *
 * Description: compare operator
 *
 * Enumerators:
 *   CPSS_COMPARE_OPERATOR_INVALID_E - always generates result "0"
 *   CPSS_COMPARE_OPERATOR_LTE     - less or equal
 *   CPSS_COMPARE_OPERATOR_GTE     - greater or equal
 *   CPSS_COMPARE_OPERATOR_NEQ     - not equal
 */
typedef enum
{
    CPSS_COMPARE_OPERATOR_INVALID_E,
    CPSS_COMPARE_OPERATOR_LTE,
    CPSS_COMPARE_OPERATOR_GTE,
    CPSS_COMPARE_OPERATOR_NEQ
} CPSS_COMPARE_OPERATOR_ENT;


/*
 * Typedef: enum CPSS_PACKET_CMD_ENT
 *
 * Description:
 *     This enum defines the packet command.
 * Fields:
 *     CPSS_PACKET_CMD_FORWARD_E           - forward packet
 *     CPSS_PACKET_CMD_MIRROR_TO_CPU_E     - mirror packet to CPU
 *     CPSS_PACKET_CMD_TRAP_TO_CPU_E       - trap packet to CPU
 *     CPSS_PACKET_CMD_DROP_HARD_E         - hard drop packet
 *     CPSS_PACKET_CMD_ROUTE_E             - IP Forward the packets
 *     CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E  - Packet is routed and mirrored to
 *                                           the CPU.
 *     CPSS_PACKET_CMD_DROP_SOFT_E         - soft drop packet
 *     CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E - Bridge and Mirror to CPU.
 *     CPSS_PACKET_CMD_BRIDGE_E            - Bridge only
 *     CPSS_PACKET_CMD_NONE_E              - Do nothing. (disable)
 *     CPSS_PACKET_CMD_LOOPBACK_E          - loopback packet is send back to originator
 *
 *  Comments:
 */
typedef enum
{
    CPSS_PACKET_CMD_FORWARD_E          , /* 00 */
    CPSS_PACKET_CMD_MIRROR_TO_CPU_E    , /* 01 */
    CPSS_PACKET_CMD_TRAP_TO_CPU_E      , /* 02 */
    CPSS_PACKET_CMD_DROP_HARD_E        , /* 03 */
    CPSS_PACKET_CMD_DROP_SOFT_E        , /* 04 */
    CPSS_PACKET_CMD_ROUTE_E            , /* 05 */
    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E , /* 06 */
    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E, /* 07 */
    CPSS_PACKET_CMD_BRIDGE_E           , /* 08 */
    CPSS_PACKET_CMD_NONE_E             , /* 09 */
    CPSS_PACKET_CMD_LOOPBACK_E           /* 10 */
} CPSS_PACKET_CMD_ENT;



/*
 * Typedef: enumeration CPSS_DROP_MODE_TYPE_ENT
 *
 * Description: Enumeration for drop mode for red packets
 *
 * Fields:
 *      CPSS_DROP_MODE_SOFT_E - drop mode is Soft drop
 *      CPSS_DROP_MODE_HARD_E - drop mode is hard drop
 *
 */
typedef enum
{
    CPSS_DROP_MODE_SOFT_E = 0,
    CPSS_DROP_MODE_HARD_E
}CPSS_DROP_MODE_TYPE_ENT;

/*
 * Typedef: enum CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT
 *
 * Description:
 *     Enumerator for modification of packet's attribute
 *     like User Priority and DSCP.
 *
 * Fields:
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E  - Keep
 *                      previous packet's attribute modification command.
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,       - disable
 *                      modification of the packet's attribute.
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E         - enable
 *                      modification of the packet's attribute.
 *  Comments:
 *
 */
typedef enum
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E = 0,
    CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E
} CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT;

/*
 * Typedef: enum CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT
 *
 * Description:
 *      Enumerator for the packet's attribute assignment precedence
 *      for the subsequent assignment mechanism.
 *
 * Fields:
 *      CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E   - Soft precedence:
 *                        The packet's attribute assignment can be overridden
 *                        by the subsequent assignment mechanism
 *      CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E   - Hard precedence:
 *                        The packet's attribute assignment is locked
 *                        to the last value of attribute assigned to the packet
 *                        and cannot be overridden.
 *
 *  Comments:
 *
 */
typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E = 0,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT;

/*
 * Typedef: enum CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT
 *
 * Description: Enumerator for the packet's attribute assignment command.
 *
 * Fields:
 *  CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E     - packet's attribute assignment
 *                         disabled
 *  CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E   - packet's attribute assignment
 *                        only if the packet is VLAN tagged.
 *  CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E - packet's attribute assignment
 *                        only if the packet is untagged or Priority-tagged.
 *  CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E      - packet's attribute assignment
 *                        regardless of packet tagging state.
 *
 */
typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT;

/*
 * Typedef: enum CPSS_DIRECTION_ENT
 *
 * Description:
 *      This enum defines direction: ingress or egress
 * Fields:
 *     CPSS_DIRECTION_INGRESS_E - the direction is ingress
 *     CPSS_DIRECTION_EGRESS_E  - the direction is egress
 *     CPSS_DIRECTION_BOTH_E    - egress and ingress direction
 *
 *  Comments:
 */
typedef enum{
    CPSS_DIRECTION_INGRESS_E,
    CPSS_DIRECTION_EGRESS_E,
    CPSS_DIRECTION_BOTH_E
}CPSS_DIRECTION_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssCommonDefsh */

