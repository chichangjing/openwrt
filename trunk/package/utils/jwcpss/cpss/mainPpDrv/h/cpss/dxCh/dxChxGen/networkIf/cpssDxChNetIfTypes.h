/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIfTypes.h
*
* DESCRIPTION:    DxCh networkIf facility types definitions
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*******************************************************************************/

#ifndef __cpssDxChGenNetIfTypesh
#define __cpssDxChGenNetIfTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>

/*
 * typedef: enum  CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC
 *
 * Description: parameter of the "cpu code table" entry
 *
 * Fields:
 *      tc                  - The Traffic Class assigned to packets with this
 *                            CPU code (APPLICABLE RANGES: 0..7).
 *      dp                  - The Drop Precedence assigned to packets with this
 *                            CPU code.
 *      truncate            - Enables/disable the truncation of packets assigned
 *                            with this CPU code to up to 128 bytes.
 *                            GT_FALSE - don't truncate packets assigned with
 *                            this CPU code.
 *                            GT_TRUE  - truncate packets assigned with this
 *                            CPU code to up to 128 bytes.
 *      cpuRateLimitMode - Rate Limiting mode
 *                            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E =
 *                            Rate Limiting is done on packets forwarded to the
 *                            CPU by the local device ONLY.
 *                            CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E =
 *                            Rate Limiting is done on all packets forwarded to
 *                            the CPU.
 *      cpuCodeRateLimiterIndex - CPU Code Rate Limiter index
 *                            The index to one of the 31 rate limiters
 *                            0 = This CPU Code is not bonded to any Rate
 *                            Limiter, thus the rate of packets with this CPU
 *                            code is not limited.
 *                            1-31 = CPU Code is bonded to a Rate limiter
 *                            according to this index
 *                            see API :
 *                            cpssDxChNetIfCpuCodeRateLimiterParamsSet(...)
 *
 *      cpuCodeStatRateLimitIndex - Index of one of the 32 statistical rate
 *                            limiters for this CPU Code .
 *                            0-31 = CPU Code is bonded to a statistical Rate
 *                            limiter according to this index
 *                            see API :
 *                            cpssDxChNetIfNetStatisticalRateLimitsTableSet(...)
 *      designatedDevNumIndex - The target device index for packets with this
 *                              CPU code.
 *                              0 = Packets with this CPU code are forwarded to
 *                                  the CPU attached to the local device.
 *                              1-7 = Packets with this CPU code are forwarded
 *                                    to CPU attached to the device indexed by
 *                                    this field
 *                              see API :
 *                              cpssDxChNetIfNetDesignatedDeviceTableSet(...)
 */
typedef struct
{
    GT_U8                       tc;
    CPSS_DP_LEVEL_ENT           dp;
    GT_BOOL                     truncate;
    CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT  cpuRateLimitMode;
    GT_U32                      cpuCodeRateLimiterIndex;
    GT_U32                      cpuCodeStatRateLimitIndex;
    GT_U32                      designatedDevNumIndex;
} CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC;

/*
 * Typedef: struct CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC
 *
 * Description: information about Rx counters -- per Queue (traffic class)
 *
 * Fields:
 *     rxInPkts   - Packets received on this queue since last read
 *     rxInOctets - Octets received on this queue since last read
 *
 */
typedef struct{
    GT_U32    rxInPkts;
    GT_U32    rxInOctets;
}CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC;

/* deprecated legacy names that should not be used ,
    and replaced by CPSS_CSCD_PORT_DSA_MODE_1_WORD_E ,
    and CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E*/
enum{
    CPSS_DXCH_NET_DSA_TYPE_REGULAR_E = 0,
    CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E  = 1
};

/*
 * typedef: enum CPSS_DXCH_NET_DSA_TYPE_ENT
 *
 * Description: Enumeration of DSA tag types
 *
 * Enumerations:
 *    CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT  - regular DSA tag - single word - 4 bytes
 *    CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT  - extended DSA tag- two words - 8 bytes
 *    CPSS_DXCH_NET_DSA_3_WORD_TYPE_ENT  - extended DSA tag- 3 words - 12 bytes
 *    CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT  - extended DSA tag- 4 words - 16 bytes
 *
 */
typedef enum
{
    CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E,
    CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E,
    CPSS_DXCH_NET_DSA_3_WORD_TYPE_ENT,
    CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT
}CPSS_DXCH_NET_DSA_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_NET_DSA_CMD_ENT
 *
 * Description: Enumeration of DSA tag commands
 *
 * Enumerations:
 *    CPSS_DXCH_NET_DSA_CMD_TO_CPU_E  - DSA command is "To CPU"
 *    CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E  - DSA command is "FROM CPU"
 *    CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E - DSA command is "TO Analyzer"
 *    CPSS_DXCH_NET_DSA_CMD_FORWARD_E  - DSA command is "FORWARD"
 *
 */
typedef enum
{
    CPSS_DXCH_NET_DSA_CMD_TO_CPU_E = 0 ,
    CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E    ,
    CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E ,
    CPSS_DXCH_NET_DSA_CMD_FORWARD_E
} CPSS_DXCH_NET_DSA_CMD_ENT;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_COMMON_STC
 *
 * Description: structure of common parameters in all DSA tag formats
 *
 * Fields:
 *  dsaTagType - DSA type regular/extended
 *
 *  vpt - the value vlan priority tag (APPLICABLE RANGES: 0..7)
 *  cfiBit - CFI bit of the vlan tag  (APPLICABLE RANGES: 0..1)
 *  vid - the value of vlan id
 *    (APPLICABLE RANGES: DxCh1, DxCh2, DxCh3, xCat, xCat3, Lion, Lion2 0..4095)
 *    (APPLICABLE RANGES: Bobcat2, Caelum, Bobcat3 0..0xFFFF)
 *  dropOnSource - Drop packet on its source device indicator,
 *                 for Fast Stack Fail over Recovery Support.
 *                 GT_TRUE: If packet is looped (packetIsLooped = 1)and
 *                          SrcDev = OwnDev then packet is dropped
 *                 GT_FALSE: in other cases
 *                 (relevant only for CH3 devices)
 *  packetIsLooped - Packet is looped indicator, for Fast Stack Fail
 *                   over Recovery Support.
 *                  GT_FALSE: Packet is not looped and is being routed via its
 *                            designated route in the Stack.
 *                  GT_TRUE: Packet is looped and its being routed on an
 *                           alternative route in the Stack.
 *                 (relevant only for CH3 devices).
 *
 */
typedef struct{
    CPSS_DXCH_NET_DSA_TYPE_ENT      dsaTagType;

    GT_U8                           vpt;
    GT_U8                           cfiBit;
    GT_U16                          vid;
    GT_BOOL                         dropOnSource;
    GT_BOOL                         packetIsLooped;
}CPSS_DXCH_NET_DSA_COMMON_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_FROM_CPU_STC
 *
 * Description: structure of "from CPU" DSA tag parameters
 *
 * Fields:
 *      dstInterface  - Packet's destination interface:
 *         type == CPSS_INTERFACE_TRUNK_E --> not supported !
 *         when type == CPSS_INTERFACE_PORT_E
 *              devPort.tgtDev - the target device that packet will be send to.
 *                               NOTE : HW device number !!
 *              devPort.tgtPort- the target port on device that packet will be
 *                             send to.
 *         when type == CPSS_INTERFACE_VIDX_E
 *              vidx     - the vidx group that packet will be sent to.
 *         when type == CPSS_INTERFACE_VID_E
 *              vlanId   - must be equal to commonParams.vid !!!
 *
 *      tc - the traffic class for the transmitted packet
 *
 *      dp  - the drop precedence for the transmitted packet
 *            For DxCh3 and above devices the field is not relevant, and the dp is
 *            globally set using the cpssDxChNetIfFromCpuDpSet API.
 *
 *      egrFilterEn - Egress filtering enable.
 *                  GT_FALSE - Packets from the CPU are not egress filtered.
 *                             Unicast packets are forwarded regardless of the
 *                             Egress port Span State or VLAN Membership.
 *                             Multi-destination packets are forwarded to the
 *                             Multicast group members specified in this tag,
 *                             regardless of the target port's Span state.
 *                  GT_TRUE -  Packets from CPU are Egress filtered.
 *
 *      cascadeControl - This field indicates which TC is assigned to the packet
 *                       when it is forwarded over a cascading/stacking port.
 *                  GT_FALSE - If the packet is queued on a port that is enabled
 *                             for Data QoS mapping (typically a cascade port),
 *                             the packet is queued according to the data
 *                             traffic {TC, DP} mapping table, which maps the
 *                             DSA tag TC and DP to a cascade port TC and DP.
 *                             On a port that is disabled for Data QoS mapping (
 *                             a non-cascade ports), the packet is queued
 *                             according to the DSA tag TC and DP.
 *                   GT_TRUE - If the packet is queued on a port that is enabled
 *                             for Control QoS mapping (typically a cascade
 *                             port), the packet is queued according to the
 *                             configured Control TC and DP. On a port that is
 *                             enabled for Control QoS mapping (non-cascade
 *                             ports), the packet is queued according to the DSA
 *                             tag TC and DP.
 *
 *      egrFilterRegistered -  Indicates that the packet is Egress filtered as a
 *                             registered packet.
 *                  GT_FALSE - Packet is egress filtered as an Unregistered
 *                             packet and is forwarded to an Egress port
 *                             according to its type (Unicast or Multicast) and
 *                             the configuration of the Egress port Port<n>
 *                             UnkFilterEn if packet is Unicast, and according
 *                             to the configuration of Port<n> UnregFilterEn if
 *                             the packet is Multicast.
 *                  GT_TRUE  - Packet is egress filtered as a registered packet
 *                             according to the members of the Multicast group.
 *                             NOTE:
 *                             When this field is 0, the type of the packet
 *                             Multicast or Unicast is set according to the
 *                             packet's MAC DA[40].
 *
 *       srcId              - Packet's Source ID
 *
 *       srcHwDev           - Packet's Source HW Device Number.
 *                            NOTE : HW device number !!
 *
 *       extDestInfo - extra destination information:
 *          multiDest - info about multi destination destination , used when:
 *                      type == CPSS_INTERFACE_VIDX_E or
 *                      type == CPSS_INTERFACE_VID_E
 *             excludeInterface - to exclude a "source" interface from the
 *                      destination flooding.
 *                      GT_FALSE - no "source" interface to exclude
 *                      GT_TRUE  - use "source" interface to exclude , see
 *                                 parameter excludedInterface.
 *             excludedInterface - the "source" interface to exclude from the
 *                          Multicast group.
 *                          NOTE: relevant only
 *                                when excludeInterface == GT_TRUE
 *                          when type == CPSS_INTERFACE_PORT_E
 *                              the {devPort.deviceNum,devPort.portNum} will be
 *                              excluded from the vid/vidx
 *                          when type == CPSS_INTERFACE_TRUNK_E
 *                              the trunkId will be excluded from the vid/vidx
 *                          type == CPSS_INTERFACE_VIDX_E not supported
 *                          type == CPSS_INTERFACE_VID_E  not supported
 *
 *             excludedIsPhyPort - supported in extended, 4 words,  DSA tag
 *                          This field determines whether the value in
 *                          <excludedInterface> contains a 20b
 *                           ePort number to exclude from an L2MLL multicast group,
 *                          or a physical 8b port number to exclude from an L2 MC group
 *                          GT_FALSE = The <excludedInterface> field
 *                                    contains a 20b ePort number to exclude
 *                          GT_TRUE =  The <excludedInterface> field
 *                                    contains an 8b Physical port number to exclude
 *                          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *              srcIsTagged - source packet received tagged.
 *                valid options:
 *                  CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E
 *                  CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E
 *                  CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                  CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                  CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *             mirrorToAllCPUs - Mirror the packet to all CPUs
 *                          Enable a CPU to send a Multicast/Broadcast packet to
 *                          all CPUs in the system.
 *                          NOTE: relevant only :
 *                               when (excludeInterface == GT_FALSE) or
 *                               when ((excludeInterface == GT_TRUE) and
 *                                (excludedInterface.type == CPSS_INTERFACE_TRUNK_E))
 *                          GT_FALSE - The Multicast packet sent by the CPU is
 *                                     not mirrored to the CPU attached to the
 *                                     receiving device.
 *                          GT_TRUE - The Multicast packet sent by the CPU is
 *                                    mirrored to the CPU attached to the
 *                                    receiving device with a TO_CPU DSA Tag and
 *                                    MIRROR_TO_ALL_CPUS CPU Code.
 *       devPort - {device,port} info (single destination), used when:
 *                          type == CPSS_INTERFACE_PORT_E
 *             dstIsTagged - GT_FALSE - Packet is sent via network port untagged
 *                           GT_TRUE  - Packet is sent via network port tagged
 *             mailBoxToNeighborCPU - Mail box to Neighbor CPU, for CPU to CPU
 *                              mail box communication.
 *                           NOTE: As a Mail message is sent to a CPU with
 *                              unknown Device Number, the TrgDev
 *                              (dstInterface.devPort.tgtDev) must be set to
 *                              the local device number and TrgPort
 *                              (dstInterface.devPort.tgtPort) must be set to
 *                              the Cascading port number in the local device
 *                              through which this packet is to be transmitted.
 *
 *      isTrgPhyPortValid - Indicates whether the target physical port field
 *                      <TrgPort> in this DSA tag is valid.
 *                      GT_FALSE:The value of <TrgPort> is not valid
 *                      GT_TRUE: The value of <TrgPort> is valid
 *                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *      dstEport     - The assigned destination ePort (dstInterface.type == CPSS_INTERFACE_PORT_E  )
 *                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..0x1FFFF)
 *
 *      tag0TpidIndex - The TPID index of Tag0.
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..7)
 *
 */
typedef struct{
    CPSS_INTERFACE_INFO_STC         dstInterface; /* vid/vidx/port */
    GT_U8                           tc;
    CPSS_DP_LEVEL_ENT               dp;
    GT_BOOL                         egrFilterEn;
    GT_BOOL                         cascadeControl;
    GT_BOOL                         egrFilterRegistered;

    GT_U32                          srcId;
    GT_HW_DEV_NUM                   srcHwDev;

    union{
        struct{
            GT_BOOL                         excludeInterface;
            CPSS_INTERFACE_INFO_STC         excludedInterface; /* port/trunk */
            GT_BOOL                         mirrorToAllCPUs;

            /* supported in extended, 4 words,  DSA tag */
            GT_BOOL                         excludedIsPhyPort;
            CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT   srcIsTagged;

        }multiDest;

        struct{
            GT_BOOL                     dstIsTagged;
            GT_BOOL                     mailBoxToNeighborCPU;
        }devPort;
    }extDestInfo;

   /* supported in extended, 4 words,  DSA tag */
    GT_BOOL                         isTrgPhyPortValid;
    GT_PORT_NUM                     dstEport; /* for port but not for vid/vidx */
    GT_U32                          tag0TpidIndex;

}CPSS_DXCH_NET_DSA_FROM_CPU_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_FORWARD_STC
 *
 * Description: structure of "forward" DSA tag parameters
 *
 * Fields:
 *
 *  srcIsTagged - source packet received tagged.
 *                valid options:
 *                  CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E
 *                  CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E
 *                  CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                  CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                  CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *  srcHwDev - Packet's Source HW Device Number.
 *           NOTE : HW device number !!
 *
 *  srcIsTrunk - the packet was received from trunk
 *     source.trunkId - - Packet's Source trunk id
 *                  NOTE : relevant when srcIsTrunk = GT_TRUE
 *     source.portNum - - Packet's Source port num
 *                  NOTE : relevant when srcIsTrunk = GT_FALSE
 *
 *  srcId              - Packet's Source ID
 *
 *  srcHwDev             - Packet's Source HW Device Number.
 *                       NOTE : HW device number !!
 *
 *  egrFilterRegistered -  Indicates that the packet is Egress filtered as a
 *                         registered packet.
 *              GT_FALSE - Packet is egress filtered as an Unregistered
 *                         packet and is forwarded to an Egress port
 *                         according to its type (Unicast or Multicast) and
 *                         the configuration of the Egress port Port<n>
 *                         UnkFilterEn if packet is Unicast, and according
 *                         to the configuration of Port<n> UnregFilterEn if
 *                         the packet is Multicast.
 *              GT_TRUE  - Packet is egress filtered as a registered packet
 *                         according to the members of the Multicast group.
 *                         NOTE:
 *                         When this field is 0, the type of the packet
 *                         Multicast or Unicast is set according to the
 *                         packet's MAC DA[40].
 *
 *  wasRouted - Indicates whether the packet is routed.
 *              GT_FALSE - Packet has not be Layer 3 routed.
 *              GT_TRUE  - Packet has been Layer 3 routed.
 *
 *  qosProfileIndex - Packet's QoS Profile.
 *
 *  dstInterface  - Packet's destination interface:
 *         type == CPSS_INTERFACE_TRUNK_E --> not supported !
 *         when type == CPSS_INTERFACE_PORT_E
 *              devPort.tgtDev - the target device that packet will be send to.
 *                               NOTE : HW device number !!
 *              devPort.tgtPort- the target port on device that packet will be
 *                             send to.
 *         when type == CPSS_INTERFACE_VIDX_E
 *              vidx     - the vidx group that packet will be sent to.
 *         when type == CPSS_INTERFACE_VID_E
 *              vlanId   - must be equal to commonParams.vid !!!
 *
 *  isTrgPhyPortValid - Indicates whether the target physical port field
 *                      <TrgPort> in this DSA tag is valid.
 *                      GT_FALSE: The value of <TrgPort> is not valid
 *                      GT_TRUE:  The value of <TrgPort> is valid
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *  dstEport - The target ePort assigned to the packet by the ingress device
 *             (dstInterface.type == CPSS_INTERFACE_PORT_E  )
 *             (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..0x1FFFF)
 *
 *  tag0TpidIndex - The TPID index of Tag0.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3),(APPLICABLE RANGES: 0..7)
 *
 *  origSrcPhyIsTrunk - Indicates whether the packet was received in the ingress
 *                      device on a physical port that is a trunk member
 *                      GT_FALSE: Packet arrived on physical port that is
 *                                configured as non-trunk member
 *                      GT_TRUE:  Packet arrived on physical port that is
 *                                configured as a trunk member
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *     origSrcPhy.trunkId - - Packet's Source physical ingress trunk id
 *                  NOTE : relevant when origSrcPhyIsTrunk = GT_TRUE
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..4095)
 *     origSrcPhy.portNum - - Packet's Source physical ingress port num
 *                  NOTE : relevant when origSrcPhyIsTrunk = GT_FALSE
 *                 (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..4095)
 *
 *  phySrcMcFilterEnable - Indicates to the remote device whether to pereform VIDX MC
 *                         source filtering based on <origSrcPhy.portNum/origSrcPhy.trunkId>
 *                         GT_FALSE: Do not perform VIDX physical source filtering
 *                         GT_TRUE:  Perform VIDX physical source filtering
 *                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *  hash                 - a packet's hash.
 *                        (APPLICABLE DEVICES: Bobcat3)
 *
 */
typedef struct{
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT srcIsTagged;
    GT_HW_DEV_NUM                   srcHwDev;
    GT_BOOL                         srcIsTrunk;
    union
    {
        GT_TRUNK_ID                 trunkId;
        GT_PORT_NUM                 portNum;
    }source;

    GT_U32                          srcId;

    GT_BOOL                         egrFilterRegistered;
    GT_BOOL                         wasRouted;
    GT_U32                          qosProfileIndex;

    CPSS_INTERFACE_INFO_STC         dstInterface; /* vid/vidx/port */

   /* supported in extended, 4 words,  DSA tag */
    GT_BOOL                         isTrgPhyPortValid;
    GT_PORT_NUM                     dstEport; /* for port but not for vid/vidx */
    GT_U32                          tag0TpidIndex;
    GT_BOOL                         origSrcPhyIsTrunk;
    union
    {
        GT_TRUNK_ID                 trunkId;
        GT_PHYSICAL_PORT_NUM        portNum;
    }origSrcPhy;
    GT_BOOL                         phySrcMcFilterEnable;
    GT_U32                          hash;
}CPSS_DXCH_NET_DSA_FORWARD_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_TO_ANALYZER_STC
 *
 * Description: structure of "to analyzer" DSA tag parameters
 *
 * Fields:
 *
 *      rxSniffer - is this packet destined to the Rx or to the Tx Analyzer port
 *          GT_FALSE - Packet was Tx sniffed and is forwarded to Target Tx
 *                     sniffer (Analyzer).
 *          GT_TRUE  - Packet was Rx sniffed and is forwarded to Target Rx
 *                     sniffer (Analyzer).
 *
 *      isTagged -  When rxSniffer == GT_TRUE:
 *                      This field is srcTagged.
 *                      srcTagged -
 *                          GT_FALSE - Packet was received from a regular
 *                                     network port untagged and is forwarded
 *                                     to the Ingress analyzer untagged.
 *                          GT_TRUE - Packet was received from a regular network
 *                                    port tagged and is forwarded to the Ingress
 *                                    analyzer tagged, with the same VID and UP
 *                                     with which it was received.
 *                      When rxSniffer == GT_FALSE:
 *                      This field is trgTagged.
 *                      trgTagged -
 *                          GT_FALSE - Packet was sent via a regular network
 *                                     port untagged and is forwarded to the Egress
 *                                     analyzer untagged.
 *                          GT_TRUE - Packet was sent via a regular network port
 *                                    tagged and is forwarded to the Egress
 *                                    analyzer tagged.
 *
 *      devPort - src/dst {dev,port} info :
 *              devPort.hwDevNum -
 *                      SrcDev:
 *                      When rx_sniff = GT_TRUE or when tag is not extended:
 *                      SrcDev indicates the packet's original Ingress port.
 *                      When rx_sniff = GT_FALSE and the tag is extended:
 *                      TrgDev:
 *                      TrgDev indicates the packet's Egress port
 *                      NOTE: When the tag is not extended, this field always
 *                      indicates SrcDev.
 *              devPort.portNum -
 *                      When rx_sniff = GT_TRUE or when tag is not extended:
 *                      SrcPort:
 *                      indicates the packet's original Ingress port.
 *                      When rx_sniff = GT_FALSE and the tag is extended:
 *                      TrgPort:
 *                      indicates the packet's Egress port.
 *                      NOTE: When this tag is not extended, port is a 5-bit
 *                      field.
 *                      When this tag is not extended, this field always
 *                      indicates SrcPort.
 *              devPort.ePort-
 *                      When in source based forwarding mode: TBD.
 *                      When in hop-by-hop mode:
 *                      When <rxSniffer>= GT_TRUE: SRCePort:
 *                      Together with SrcDev, SRCePort indicates the packet's
 *                      original Ingress ePort assignment.
 *                      When <rxSniffer> = GT_FALSE: TRGePort:
 *                      Together with TrgDev, TRGePort indicates the packet's
 *                      assigned egress ePort.
 *
 *      analyzerTrgType - The target Analyzer type :
 *                          CPSS_INTERFACE_PORT_E - see extDestInfo.devPort
 *                          CPSS_INTERFACE_VIDX_E - see extDestInfo.multiDest
 *                          CPSS_INTERFACE_VID_E  - ignore extDestInfo and implicit use vidx=0xFFF
 *      extDestInfo - extra destination (target) information:
 *          multiDest - info about multi destination , used when: analyzerTrgType = CPSS_INTERFACE_VIDX_E
 *              multiDest.analyzerEvidx       - The target analyzer interface eVIDX
 *
 *          devPort - {device,port} info (single destination), used when: analyzerTrgType = CPSS_INTERFACE_PORT_E
 *              devPort.analyzerIsTrgPortValid      - The target Analyzer interface indication that the
 *                      target analyzer port field devPort.analyzerTrgPort has a valid value
 *              devPort.analyzerHwTrgDev    - The target Analyzer interface HW device number
 *              devPort.analyzerTrgPort     - The target Analyzer interface physical port number
 *              devPort.analyzerTrgEport    - The target analyzer interface ePort
 *
 *      tag0TpidIndex - The TPID index of Tag0.
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..7)
 *
 */
typedef struct{
    GT_BOOL                         rxSniffer;

    GT_BOOL                         isTagged;
    struct
    {
        GT_HW_DEV_NUM               hwDevNum;
        GT_PHYSICAL_PORT_NUM        portNum;

        /* supported in extended, 4 words,  DSA tag */
        GT_PORT_NUM                 ePort;
    }devPort;

    /* supported in extended, 4 words,  DSA tag */
    CPSS_INTERFACE_TYPE_ENT         analyzerTrgType;

    union{
        struct{
            GT_U16                          analyzerEvidx;
        }multiDest;

        struct{
            GT_BOOL                         analyzerIsTrgPortValid;
            GT_HW_DEV_NUM                   analyzerHwTrgDev;
            GT_PHYSICAL_PORT_NUM            analyzerTrgPort;
            GT_PORT_NUM                     analyzerTrgEport;
        }devPort;
    }extDestInfo;

    GT_U32                          tag0TpidIndex;

}CPSS_DXCH_NET_DSA_TO_ANALYZER_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_TO_CPU_STC
 *
 * Description: structure of "To CPU" DSA tag parameters
 *
 * Fields:
 *
 *      isEgressPipe - indicates the type of data forwarded to the CPU.
 *                     the data came from egress/ingress pipe (in PP).
 *          GT_FALSE - The packet was forwarded to the CPU by the Ingress pipe
 *                     and this tag contains the packet's source information.
 *          GT_TRUE  - The packet was forwarded to the CPU by the Egress pipe
 *                     and this tag contains the packet's destination
 *                     information.
 *          NOTE: When this tag is not extended, <isEgressPipe> should be
 *                     considered like GT_FALSE.
 *
 *      isTagged -  When isEgressPipe == GT_FALSE:
 *                      This field is srcTagged.
 *                      srcTagged - This tag contains Source Port information
 *                                  and this bit indicates the VLAN Tag format,
 *                                  in which the packet was received on the
 *                                  network port:
 *                          GT_FALSE - Packet was received from a network port
 *                                     untagged.
 *                          GT_TRUE - Packet was received from a network port
 *                                     tagged.
 *                      When isEgressPipe == GT_TRUE:
 *                      This field is trgTagged.
 *                      trgTagged - This tag contains Target Port information
 *                                  and this bit indicates the VLAN Tag format,
 *                                  in which the packet was transmitted via the
 *                                  network port:
 *                          GT_FALSE - Packet was transmitted to a regular
 *                                     network port untagged.
 *                          GT_TRUE - Packet was transmitted to a regular
 *                                     network port tagged.
 *
 *               NOTE: When isEgressPipe == GT_FALSE and the packet
 *                     forwarded to the CPU is received on a customer port on
 *                     which Nested VLAN is implemented, srcTagged is set to
 *                     GT_FALSE, regardless of the packet's VLAN tag format.
 *                     As the packet is considered untagged, when the packet is
 *                     forwarded to the CPU, the customer's VLAN tag (if any)
 *                     resides after the DSA tag.
 *
 *      hwDevNum - When isEgressPipe == GT_FALSE:
 *                  NOTE : HW device number !!
 *                  This field is srcDev.
 *                      This tag contains Source Port information and this field
 *                      indicates the number of the Source HW Device on which the
 *                      packet was received.
 *                  When isEgressPipe == GT_TRUE:
 *                      This field is trgDev.
 *                      This tag contains Target Port information and this field
 *                      indicates the number of the Destination HW Device through
 *                      which the packet was transmitted.
 *
 *      srcIsTrunk -    when dsaTagType == CPSS_DXCH_NET_DSA_TYPE_REGULAR_E or
 *                           dsaTagType == CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E :
 *                          indication if interface.srcTrunkId is valid (srcIsTrunk = GT_TRUE)
 *                          or interface.portNum is valid (srcIsTrunk = GT_FALSE)
 *                          NOTE: interface.ePort is never valid !
 *
 *                          When a packet's DSA Tag is replaced from FORWARD to
 *                                TO_CPU and FORWARD_DSA<SrcIsTrunk> = GT_TRUE and
 *                                isEgressPipe = GT_FALSE, this field is set to
 *                                GT_TRUE and This Tag hold interface.trunkId which is the
 *                                Trunk number parsed from the FORWARD DSA Tag
 *                                When isEgressPipe == GT_FALSE:
 *                                  GT_FALSE - the field of <interface.portNum> indicates the source
 *                                           port number
 *                                  GT_TRUE - the field of <interface.srcTrunkId> indicates the source
 *                                           trunk number
 *                                When isEgressPipe == GT_TRUE:
 *                                  this field should be GT_FALSE.
 *                                  GT_FALSE - the field of <interface.portNum> indicates the
 *                                           destination Port through which the packet was
 *                                           transmitted.
 *                                  GT_TRUE  - not supported !
 *
 *                      when dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT :
 *                          indication if <interface.srcTrunkId> is valid (srcIsTrunk = GT_TRUE)
 *                          or <interface.ePort> is valid (srcIsTrunk = GT_FALSE)
 *                          NOTE: interface.portNum is always valid !
 *
 *                                When isEgressPipe == GT_FALSE:
 *                                  GT_FALSE - <interface.ePort> indicates the source eport number. or
 *                                      (assigned by the ingress device)
 *                                  GT_TRUE - <interface.srcTrunkId> indicates the source trunk number.
 *                                      (assigned by the ingress device)
 *                                When isEgressPipe == GT_TRUE:
 *                                  GT_FALSE - <interface.ePort> indicates the target eport number.
 *                                      (assigned by the egress device)
 *                                  GT_TRUE  - not supported !
 *
 *      interface - interface info
 *             srcTrunkId  - see comments about srcIsTrunk field
 *             portNum     - see comments about srcIsTrunk field
 *             ePort       - see comments about srcIsTrunk field
 *
 *
 *      cpuCode - CPU codes that indicate the reason for sending a packet to
 *                the CPU.
 *                Note : this is unified CPU code format (and not HW format))
 *
 *      wasTruncated - Was packet sent to CPU truncated?
 *                GT_FALSE - packet was not truncated.
 *                GT_TRUE - Indicates that only the first 128 bytes of the
 *                          packet are sent to the CPU. The packet's original
 *                          byte count is in <originByteCount> field.
 *
 *      originByteCount - The packet's original byte count.
 *
 *      timestamp       - IPFIX timestamping. Relevant only for xCat and
 *                        above devices, and used only in case that packet was
 *                        mirrored to the CPU due to IPFIX sampling.
 *
 *      packetIsTT - An indication to the CPU that the trapped/mirrored packet
 *                   was tunnel-terminated by the device processing pipes.
 *                   GT_FALSE: The packet was not tunnel-terminated by the device
 *                             processing engines
 *                   GT_TRUE:  The packet was tunnel-terminated by the device
 *                             processing engines
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *      flowIdTtOffset.flowId - The flow-ID assigned to the packet
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..0xFFFFF)
 *      flowIdTtOffset.ttOffset - the passenger packet offset in the tunnel
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..0xFFFFF)
 *
 *      tag0TpidIndex - The TPID index of Tag0.
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..7)
 *
 *
 */
typedef struct{
   GT_BOOL                          isEgressPipe;

   GT_BOOL                          isTagged;
   GT_HW_DEV_NUM                    hwDevNum;
   GT_BOOL                          srcIsTrunk;
   struct
   {
       GT_TRUNK_ID                  srcTrunkId;
       GT_PHYSICAL_PORT_NUM         portNum;
      /* supported in extended, 4 words,  DSA tag */
       GT_PORT_NUM                  ePort;
   }interface;

   CPSS_NET_RX_CPU_CODE_ENT         cpuCode;
   GT_BOOL                          wasTruncated;
   GT_U32                           originByteCount;
   GT_U32                           timestamp;

   /* supported in extended, 4 words,  DSA tag */
   GT_BOOL                          packetIsTT;
   union
   {
       GT_U32                           flowId;
       GT_U32                           ttOffset;
   }flowIdTtOffset;

   GT_U32                           tag0TpidIndex;

}CPSS_DXCH_NET_DSA_TO_CPU_STC;

/*
 * typedef: structure CPSS_DXCH_NET_SDMA_TX_PARAMS_STC
 *
 * Description: structure of Tx parameters , that should be set to PP when using
 *              the SDMA channel
 *
 * Fields:
 *      recalcCrc - GT_TRUE  - the PP should add CRC to the transmitted packet,
 *                  GT_FALSE - leave packet unchanged.
 *                  NOTE : The DXCH device always add 4 bytes of CRC when need
 *                         to recalcCrc = GT_TRUE
 *
 *
 *      txQueue - the queue that packet should be sent to CPU port. (APPLICABLE RANGES: 0..7)
 *
 *      evReqHndl - The application handle got from cpssEventBind for
 *                  CPSS_PP_TX_BUFFER_QUEUE_E events.
 *
 *     invokeTxBufferQueueEvent - invoke Tx buffer Queue event.
 *              when the SDMA copies the buffers of the packet from the CPU ,
 *              the PP may invoke the event of CPSS_PP_TX_BUFFER_QUEUE_E.
 *              this event notify the CPU that the Tx buffers of the packet can
 *              be reused by CPU for other packets. (this in not event of
 *              CPSS_PP_TX_END_E that notify that packet left the SDMA)
 *              The invokeTxBufferQueueEvent parameter control the invoking of
 *              that event for this Packet.
 *              GT_TRUE - PP will invoke the event when buffers are copied (for
 *                        this packet's buffers).
 *              GT_FALSE - PP will NOT invoke the event when buffers are copied.
 *                        (for this packet's buffers).
 *              NOTE :
 *                  when the sent is done as "Tx synchronic" this parameter
 *                  IGNORED (the behavior will be as GT_FALSE)
 *
 *
 *
 */
typedef struct{
    GT_BOOL                         recalcCrc;
    GT_U8                           txQueue;
    GT_UINTPTR                      evReqHndl;
    GT_BOOL                         invokeTxBufferQueueEvent;
}CPSS_DXCH_NET_SDMA_TX_PARAMS_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_PARAMS_STC
 *
 * Description: structure of DSA parameters , that contain full information on
 *              DSA for RX,Tx packets to/from CPU from/to PP.
 *
 * Fields:
 *      commonParams - common parameters in all DSA tag formats
 *
 *      dsaType - the type of DSA tag.(FORWARD/TO_CPU/FROM_CPU/TO_ANALYZER)
 *
 *      dsaInfo - the DSA tag information:
 *          toCpu - information needed when sending "TO_CPU" packets.
 *              relevant when dsaInfo = TO_CPU
 *          fromCpu - information needed when sending "FROM_CPU" packets.
 *              relevant when dsaInfo = FROM_CPU
 *          toAnalyzer - information needed when sending "TO_ANALYZER" packets.
 *              relevant when dsaInfo = TO_ANALYZER
 *          forward - information needed when sending "FORWARD" packets.
 *              relevant when dsaInfo = FORWARD
 *
*/
typedef struct{
    CPSS_DXCH_NET_DSA_COMMON_STC  commonParams;

    CPSS_DXCH_NET_DSA_CMD_ENT           dsaType;
    union{
        CPSS_DXCH_NET_DSA_TO_CPU_STC         toCpu;
        CPSS_DXCH_NET_DSA_FROM_CPU_STC       fromCpu;
        CPSS_DXCH_NET_DSA_TO_ANALYZER_STC    toAnalyzer;
        CPSS_DXCH_NET_DSA_FORWARD_STC        forward;
    }dsaInfo;
}CPSS_DXCH_NET_DSA_PARAMS_STC;



/*
 * typedef: structure CPSS_DXCH_NET_TX_PARAMS_STC
 *
 * Description: structure of Tx parameters , that contain full information on
 *              how to send TX packet from CPU to PP.
 *
 * Fields:
 *      packetIsTagged - the packet (buffers) contain the vlan TAG inside.
 *                  GT_TRUE  - the packet contain the vlan TAG inside.
 *                  GT_FALSE - the packet NOT contain the vlan TAG inside.
 *
 *      cookie  - The user's data to be returned to the Application when in
 *                the Application's buffers can be re-used .
 *
 *      sdmaInfo - the info needed when send packets using the SDMA.
 *
 *      dsaParam - the DSA parameters
 *
*/
typedef struct{
    GT_BOOL                             packetIsTagged;
    GT_PTR                              cookie;
    CPSS_DXCH_NET_SDMA_TX_PARAMS_STC    sdmaInfo;
    CPSS_DXCH_NET_DSA_PARAMS_STC        dsaParam;
}CPSS_DXCH_NET_TX_PARAMS_STC;

/*
 * typedef: structure CPSS_DXCH_NET_RX_PARAMS_STC
 *
 * Description: structure of Rx parameters , that contain full information on
 *              how packet was received from PP to CPU port.
 *
 * Fields:
 *      dsaParam - the DSA parameters
 *
*/
typedef struct{
    CPSS_DXCH_NET_DSA_PARAMS_STC        dsaParam;
}CPSS_DXCH_NET_RX_PARAMS_STC;


/*
 * typedef: enum CPSS_DXCH_NET_PCKT_ENCAP_ENT
 *
 * Description: Defines the different transmitted packet encapsulations.
 *
 * Enumerations:
 *      CPSS_DXCH_NET_REGULAR_PCKT_E  - Non - control packet.
 *      CPSS_DXCH_NET_CONTROL_PCKT_E  - Control packet.
 *
 * Comments:
 *      Non-control packets are subject to egress bridge filtering due
 *      to VLAN egress filtering or Spanning Tree filtering.
 *
 *      Control packts are not subject to bridge egress filtering.
 */
typedef enum
{
    CPSS_DXCH_NET_REGULAR_PCKT_E  = 0,
    CPSS_DXCH_NET_CONTROL_PCKT_E  = 7
}CPSS_DXCH_NET_PCKT_ENCAP_ENT;


/*
 * Typedef: enum CPSS_DXCH_NET_MAC_TYPE_ENT
 *
 * Description: Defines the different Mac-Da types of a transmitted packet.
 *
 * Enumerations:
 *      CPSS_DXCH_NET_UNICAST_MAC_E         - MAC_DA[0] = 1'b0
 *      CPSS_DXCH_NET_MULTICAST_MAC_E       - MAC_DA[0] = 1'b1
 *      CPSS_DXCH_NET_BROADCAST_MAC_E       - MAC_DA = 0xFFFFFFFF
 *
 */
typedef enum
{
    CPSS_DXCH_NET_UNICAST_MAC_E,
    CPSS_DXCH_NET_MULTICAST_MAC_E,
    CPSS_DXCH_NET_BROADCAST_MAC_E
}CPSS_DXCH_NET_MAC_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT
 *
 * Description: Defines the behavior in case of Rx SDMA resource error
 *
 * Enumerations:
 *      CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E  - the packet remains scheduled for transmission.
 *      CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E  - the packet is dropped.
 *
 */
typedef enum
{
    CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E,
    CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E
}CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT
 *
 * Description: Defines the rate mode for Tx SDMA working as packet Generator
 *
 * Enumerations:
 *      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E  -
 *              rate is measured as packets per second.
 *      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E  -
 *              interval between successive transmissions of the same packet.
 *      CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E             -
 *              no interval between successive transmissions.
 *
 */
typedef enum
{
    CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_GLOBAL_THROUGHPUT_E,
    CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E,
    CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_NO_GAP_E
}CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT
 *
 * Description: Defines Tx SDMA Generator packet burst counter status.
 *
 * Enumerations:
 *      CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_IDLE_E      -
 *                                                     packet counter is idle.
 *      CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_RUN_E       -
 *   packet counter is running and has not yet reached its configurable limit.
 *      CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_LIMIT_E     -
 *                          packet counter has reached its configurable limit.
 *      CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_TERMINATE_E -
 *                              packet counter has been terminated by the CPU.
 */
typedef enum
{
    CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_IDLE_E,
    CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_RUN_E,
    CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_LIMIT_E,
    CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_TERMINATE_E
}CPSS_DXCH_NET_TX_GENERATOR_BURST_STATUS_ENT;


/*
 * Typedef: enum CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT
 *
 * Description: Defines TO_CPU dsa tag field <Flow-ID/TT Offset> to hold 'flowid'
 *              or 'ttOffset'.
 *              related to CPSS_DXCH_NET_DSA_TO_CPU_STC::flowIdTtOffset
 *
 * Enumerations:
 *      CPSS_DXCH_NET_TO_CPU_FLOW_ID_MODE_E - use  'flowid' in <Flow-ID/TT Offset>
 *
 *      CPSS_DXCH_NET_TO_CPU_TT_OFFSET_MODE_E - use  'TT Offset' in <Flow-ID/TT Offset>
 *                          NOTE: In this mode if 'non TT' will hold 'flowid'.
 *
 */
typedef enum
{
    CPSS_DXCH_NET_TO_CPU_FLOW_ID_MODE_E,
    CPSS_DXCH_NET_TO_CPU_TT_OFFSET_MODE_E
}CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __cpssDxChGenNetIfTypesh */

