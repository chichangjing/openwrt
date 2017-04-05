/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmHwTypes.h
*
* DESCRIPTION:
*       HW memory read/write internal definitions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmHwTypesh
#define __prvCpssDxChLpmHwTypesh

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/ip/cpssIpTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: enum PRV_CPSS_DXCH_LPM_CNT_SET_ENT
 *
 * Description: Each next hop can be linked with one the below counter sets.
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_LPM_CNT_SET0_E   - counter set 0
 *  PRV_CPSS_DXCH_LPM_CNT_SET1_E   - counter set 1
 *  PRV_CPSS_DXCH_LPM_CNT_SET2_E   - counter set 2
 *  PRV_CPSS_DXCH_LPM_CNT_SET3_E   - counter set 3
 *  PRV_CPSS_DXCH_LPM_CNT_NO_SET_E - do not link route entry with a counter set
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_CNT_SET0_E   = 0,
    PRV_CPSS_DXCH_LPM_CNT_SET1_E   = 1,
    PRV_CPSS_DXCH_LPM_CNT_SET2_E   = 2,
    PRV_CPSS_DXCH_LPM_CNT_SET3_E   = 3,
    PRV_CPSS_DXCH_LPM_CNT_NO_SET_E = 4
} PRV_CPSS_DXCH_LPM_CNT_SET_ENT;

/*
 * typedef: enum PRV_CPSS_DXCH_LPM_CPU_CODE_INDEX_ENT
 *
 * Description:  this value is added to the CPU code assignment in case of trap
 *               or mirror
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_LPM_CPU_CODE_IDX_0_E - CPU code added index 0
 *  PRV_CPSS_DXCH_LPM_CPU_CODE_IDX_1_E - CPU code added index 1
 *  PRV_CPSS_DXCH_LPM_CPU_CODE_IDX_2_E - CPU code added index 2
 *  PRV_CPSS_DXCH_LPM_CPU_CODE_IDX_3_E - CPU code added index 3
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_CPU_CODE_IDX_0_E   = 0,
    PRV_CPSS_DXCH_LPM_CPU_CODE_IDX_1_E   = 1,
    PRV_CPSS_DXCH_LPM_CPU_CODE_IDX_2_E   = 2,
    PRV_CPSS_DXCH_LPM_CPU_CODE_IDX_3_E   = 3
} PRV_CPSS_DXCH_LPM_CPU_CODE_INDEX_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_FORMAT_STC
 *
 * Description: Unicast Route Entry
 *
 * Fields:
 *  cmd                - Route entry command. supported commands:
 *                          CPSS_PACKET_CMD_ROUTE_E,
 *                          CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
 *                          CPSS_PACKET_CMD_TRAP_TO_CPU_E,
 *                          CPSS_PACKET_CMD_DROP_SOFT_E ,
 *                          CPSS_PACKET_CMD_DROP_HARD_E.
 *  cpuCodeIdx         - the cpu code index of the specific code when trap or
 *                       mirror.
 *  appSpecificCpuCodeEnable- Enable CPU code overwritting according to the
 *                        application specific CPU Code assignment mechanism.
 *  unicastPacketSipFilterEnable - Enable: Assign the command HARD DROP if the
 *                       SIP lookup matches this entry
 *                       Disable: Do not assign the command HARD DROP due to
 *                       the SIP lookup matching this entry
 *  ttlHopLimitDecEnable - Enable TTL/Hop Limit Decrement
 *  ttlHopLimDecOptionsExtChkByPass- TTL/HopLimit Decrement and option/
 *                       Extention check bypass.
 *  ingressMirror      - mirror to ingress analyzer.
 *  ingressMirrorToAnalyzerIndex - One of seven possible analyzers. Relevant
 *                       when ingressMirror is GT_TRUE.
 *                       If a previous engine in the pipe assigned a different
 *                       analyzer index, the higher index wins.
 *                       (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *  qosProfileMarkingEnable- Enable Qos profile assignment.
 *  qosProfileIndex    - the qos profile to assign in case of
 *                       qosProfileMarkingEnable = GT_TRUE
 *  qosPrecedence      - whether this packet Qos parameters can be overridden
 *                       after this assigment.
 *  modifyUp           - whether to change the packets UP and how.
 *  modifyDscp         - whether to change the packets DSCP and how.
 *  countSet           - The counter set this route entry is linked to
 *  trapMirrorArpBcEnable - enable Trap/Mirror ARP Broadcasts with DIP matching
 *                       this entry
 *  sipAccessLevel     - The security level associated with the SIP.
 *  dipAccessLevel     - The security level associated with the DIP.
 *  ICMPRedirectEnable - Enable performing ICMP Redirect Exception Mirroring.
 *  scopeCheckingEnable- Enable IPv6 Scope Checking.
 *  siteId             - The site id of this route entry.
 *  mtuProfileIndex    - One of the eight global configurable MTU sizes (0..7).
 *  isTunnelStart      - weather this nexthop is tunnel start enrty, in which
 *                       case the outInteface & mac are irrlevant and the tunnel
 *                       id is used.
 *  nextHopVlanId      - the output vlan id (used also for SIP RPF check, and
 *                       ICMP check)
 *  nextHopInterface   - the output interface this next hop sends to. relevant
 *                       only if the isTunnelStart = GT_FALSE
 *  nextHopARPPointer  - The ARP Pointer indicating the routed packet MAC DA,
 *                       relevant only if the isTunnelStart = GT_FALSE
 *  nextHopTunnelPointer- the tunnel pointer in case this is a tunnel start
 *                       isTunnelStart = GT_TRUE
 *  nextHopVid1         - The next hop VID1 associated with the Unicast DIP
 *                        lookup and the unicast RPF VID1 assocaited with the
 *                        SIP lookup.
 *                       (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 */
typedef struct PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_FORMAT_STCT
{
    CPSS_PACKET_CMD_ENT                 cmd;
    PRV_CPSS_DXCH_LPM_CPU_CODE_INDEX_ENT cpuCodeIdx;
    GT_BOOL                             appSpecificCpuCodeEnable;
    GT_BOOL                             unicastPacketSipFilterEnable;
    GT_BOOL                             ttlHopLimitDecEnable;
    GT_BOOL                             ttlHopLimDecOptionsExtChkByPass;
    GT_BOOL                             ingressMirror;
    GT_U32                              ingressMirrorToAnalyzerIndex;
    GT_BOOL                             qosProfileMarkingEnable;
    GT_U32                              qosProfileIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDscp;
    PRV_CPSS_DXCH_LPM_CNT_SET_ENT       countSet;
    GT_BOOL                             trapMirrorArpBcEnable;
    GT_U32                              sipAccessLevel;
    GT_U32                              dipAccessLevel;
    GT_BOOL                             ICMPRedirectEnable;
    GT_BOOL                             scopeCheckingEnable;
    CPSS_IP_SITE_ID_ENT                 siteId;
    GT_U32                              mtuProfileIndex;
    GT_BOOL                             isTunnelStart;
    GT_U16                              nextHopVlanId;
    CPSS_INTERFACE_INFO_STC             nextHopInterface;
    GT_U32                              nextHopARPPointer;
    GT_U32                              nextHopTunnelPointer;
    GT_U16                              nextHopVid1;
} PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_FORMAT_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_HW_UC_ECMP_RPF_FORMAT_STC
 *
 * Description: Unicast ECMP/QoS RPF check route entry format
 *
 * Fields:
 *      vlanArray     - array of vlan Ids that are compared against the
 *                      packet vlan as part of the unicast RPF check
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_HW_UC_ECMP_RPF_FORMAT_STCT
{
    GT_U16      vlanArray[8]; 
} PRV_CPSS_DXCH_LPM_HW_UC_ECMP_RPF_FORMAT_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_UNT
 *
 * Description:
 *      Holds the two possible unicast route entry formats.
 *
 * Fields:
 *      regularEntry - regular route entry format
 *      ecmpRpfCheck - special route entry format for unicast ECMP/QOS RPF check
 */
typedef struct PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_UNTT
{
    PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_FORMAT_STC     regularEntry;
    PRV_CPSS_DXCH_LPM_HW_UC_ECMP_RPF_FORMAT_STC        ecmpRpfCheck;
} PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_UNT;

/*
 * typedef: enum PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_TYPE_ENT (type)
 *
 * Description: the type of UC route entry
 *
 * Enumerations:
 *      CPSS_DXCH_IP_UC_ROUTE_ENTRY_E - regular route entry format
 *      CPSS_DXCH_IP_UC_ECMP_RPF_E    - special route entry format for unicast
 *                                      ECMP/QOS RPF check
 */
typedef enum PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_TYPE_ENTT
{
    PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_E   = 0,
    PRV_CPSS_DXCH_LPM_HW_UC_ECMP_RPF_E      = 1
} PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_TYPE_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_STC
 *
 * Description: Unicast route entry
 *
 * Fields:
 *      type      - type of the route entry
 *      entry     - the route entry information
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_STCT
{
    PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_TYPE_ENT    type;
    PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_UNT         entry;
} PRV_CPSS_DXCH_LPM_HW_UC_ROUTE_ENTRY_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmHwTypesh */

