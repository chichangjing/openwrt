/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssPortStat.h
*
* DESCRIPTION:
*       CPSS definitions for port statistics and egress counters.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#ifndef __cpssPortStath
#define __cpssPortStath

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/*
 * typedef: enum CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT
 *
 * Description: Enumeration of Interlaken counter IDs
 *
 * Enumerations:
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BYTE_E             -
 *         number of received bytes (64-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BAD_PKT_E          -
 *         number of received wrong packets (64-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_PKT_E              -
 *         number of received packets (64-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_CRC_ERR_E          -
 *         number of received bursts with CRC error (64-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BYTE_E             -
 *         number of transmitted bytes (64-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BAD_PKT_E          -
 *         number of transmitted wrong packets (64-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_PKT_E              -
 *         number of transmitted packets (64-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_BLK_TYPE_ERR_E        -
 *         Per lane counter (0..23)
 *         Number of words received with Control Word framing ('x10')
 *         that don't match one of the defined Control Words
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_DIAG_CRC_ERR_E        -
 *         Per lane counter (0..23)
 *         Number of errors in the lane CRC
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_WORD_SYNC_ERR_E       -
 *         Per lane counter (0..23)
 *         Number of times a lane lost word boundary synchronization
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_ERR_E    -
 *         Number of alignment sequences received in error
 *         (i.e., those that violate the current alignment)
 *         (16-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_FAIL_E   -
 *         Number of times alignment was lost
 *         (after four consecutive RX alignment errors)(16-bit value)
 *    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BURST_SIZE_ERR_E       -
 *         Number of received bursts with incorrect size (44-bit value)
 *
 */
typedef enum
{
    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BYTE_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BAD_PKT_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_PKT_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_CRC_ERR_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BYTE_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_BAD_PKT_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_TX_PKT_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_BLK_TYPE_ERR_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_DIAG_CRC_ERR_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_WORD_SYNC_ERR_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_ERR_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_ALIGNMENT_FAIL_E,
    CPSS_PORT_INTERLAKEN_COUNTER_ID_RX_BURST_SIZE_ERR_E
} CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT;


/*
 * typedef: enum CPSS_PORT_MAC_COUNTERS_ENT
 *
 * Description: Enumeration of the ethernet MAC Counters.
 *
 * Enumerations:
 *  CPSS_GOOD_PKTS_RCV_E    - Number of ethernet frames received that are
 *                            neither bad ethernet frames nor MAC Control pkts.
 *                            This includes Bridge Control packets (LCAP, BPDU)
 *                            Not Supported for DxCh and ExMxPm.
 *  CPSS_GOOD_OCTETS_RCV_E  - Sum of lengths of all good ethernet frames
 *                            received that are neither bad ethernet frames nor
 *                            MAC Control pkts. This does not include 802.3x
 *                            pause messages, but it does include Bridge Control
 *                            packets (LCAP, BPDU)
 *  CPSS_BAD_PKTS_RCV_E     - Number of bad ethernet frames received
 *                            Not Supported for DxCh and ExMxPm.
 *  CPSS_BAD_OCTETS_RCV_E   - Sum of lengths of all bad ethernet frames received
 *  CPSS_BRDC_PKTS_RCV_E    - Total number of good packets received that were
 *                            directed to the broadcast address
 *  CPSS_MC_PKTS_RCV_E      - Total number of good packets received that were
 *                            directed to a multicast address
 *  CPSS_GOOD_PKTS_SENT_E   - Number of ethernet frames sent from this MAC.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *                            Not Supported for DxCh and ExMxPm.
 *  CPSS_GOOD_OCTETS_SENT_E - Sum of lengths of all good ethernet frames sent
 *                            from this MAC.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  CPSS_BRDC_PKTS_SENT_E   - Total number of good packets sent that have a
 *                            broadcast destination MAC address.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  CPSS_MC_PKTS_SENT_E     - Total number of good packets sent that have a
 *                            multicast destination MAC address.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  CPSS_PKTS_64_OCTETS_E   - The number of received and transmitted good and
 *                            bad frames which are 64 bytes in size.
 *                            This does not include MAC Control Frames.
 *  CPSS_PKTS_65TO127_OCTETS_E -
 *                            The number of received and transmitted good and
 *                            and bad frames which are 65 to 127 bytes in size.
 *                            This does not include MAC Control Frames.
 *  CPSS_PKTS_128TO255_OCTETS_E -
 *                            The number of received and transmitted good and
 *                            and bad frames which are 128 to 255 bytes in size.
 *                            This does not include MAC Control Frames.
 *  CPSS_PKTS_256TO511_OCTETS_E -
 *                            The number of received and transmitted good and
 *                            and bad frames which are 256 to 511 bytes in size.
 *                            This does not include MAC Control Frames.
 *  CPSS_PKTS_512TO1023_OCTETS_E -
 *                            The number of received and transmitted good and
 *                            and Bad frames which are 512 to 1023 bytes in size.
 *                            This does not include MAC Control Frames.
 *  CPSS_PKTS_1024TOMAX_OCTETS_E -
 *                            The number of received and transmitted good and
 *                            and bad frames which are more than 1024 bytes in size.
 *                            This does not include MAC Control Frames.
 *                            (xCat2: the behavior of this counter is determined by
 *                            oversized packets counter mode.
 *                            See: cpssDxChMacOversizedPacketsCounterModeSet.)
 *  CPSS_COLLISIONS_E       - Total number of collisions seen by the MAC
 *  CPSS_LATE_COLLISIONS_E  - Total number of late collisions seen by the MAC
 *                            (xCat2: also counts unknown MAC control frames)
 *  CPSS_EXCESSIVE_COLLISIONS_E -
 *                            Number of frames dropped in the transmit MAC
 *                            due to excessive collisions. This is applicable
 *                            for Half-Duplex mode only.
 *  CPSS_MAC_RCV_ERROR_E    - Number of Rx Error events seen by the receive side
 *                            of the MAC
 *  CPSS_MAC_TRANSMIT_ERR_E - Number of frames not transmitted correctly or
 *                            dropped due to internal MAC Tx error
 *                            Used for TX FIFO underrun and TX CRC
 *                            errors MAC MIB Counter.
 *  CPSS_BAD_CRC_E          - Number of CRC error events
 *  CPSS_DROP_EVENTS_E      - Number of instances that the port was unable to
 *                            receive packets due to insufficient bandwidth to
 *                            one of the PP internal resources, such as the DRAM
 *                            or buffer allocation.
 *  CPSS_UNDERSIZE_PKTS_E   - number of undersize packets received.
 *  CPSS_FRAGMENTS_PKTS_E   - number of fragments received.
 *  CPSS_OVERSIZE_PKTS_E    - number of oversize packets received.
 *  CPSS_JABBER_PKTS_E      - number of jabber packets received.
 *  CPSS_UNRECOG_MAC_CNTR_RCV_E - number of received MAC Control frames that
 *                            have an opcode different from 00-01.
 *                            Not Supported for DxCh and ExMxPm.
 *
 *  CPSS_GOOD_FC_RCV_E      - number of good Flow Control frames received.
 *  CPSS_FC_SENT_E          - number of Flow Control frames sent.
 *  CPSS_BadFC_RCV_E        - number of bad Flow Control frames received.
 *                            Not Supported for DxCh, and ExMxPm.
 *  CPSS_GOOD_UC_PKTS_RCV_E -
 *                            Number of Ethernet Unicast frames received that
 *                            are not bad Ethernet frames or MAC Control packets.
 *                            Note that this number includes Bridge Control
 *                            packets such as LCAP and BPDU.
 *                            Supported for DxCh and ExMxPm only.
 *  CPSS_GOOD_UC_PKTS_SENT_E -
 *                            Number of good frames sent that had a Unicast
 *                            destination MAC Address.
 *                            Supported for DxCh and ExMxPm only.
 *
 *  CPSS_MULTIPLE_PKTS_SENT_E -
 *                            Valid Frame transmitted on half-duplex link that
 *                            encountered more then one collision.
 *                            Byte count and cast are valid.
 *                            Supported for DxCh and ExMxPm only.
 *  CPSS_DEFERRED_PKTS_SENT_E -
 *                            Valid frame transmitted on half-duplex link with
 *                            no collisions, but where the frame transmission
 *                            was delayed due to media being busy. Byte count
 *                            and cast are valid.
 *                            Supported for DxCh and ExMxPm only.
 *                            (xCat2: the behavior of this counter is determined by
 *                            oversized packets counter mode.
 *                            See: cpssDxChMacOversizedPacketsCounterModeSet.)
 *  CPSS_PKTS_1024TO1518_OCTETS_E - 
 *                            The number of received good and bad frames that are 
 *                            1024 to 1518 bytes in size.
 *                            Relevant only for Interlaken channel MIB counter
 *                            (APPLICABLE DEVICES: Bobcat2)
 *  CPSS_PKTS_1519TOMAX_OCTETS_E - 
 *                            The number of received good and bad frames that are
 *                            more than 1519 bytes in size.
 *                            Relevant only for Interlaken channel MIB counter
 *                            (APPLICABLE DEVICES: Bobcat2)
 *
 *  CPSS_LAST_MAC_COUNTER_NUM_E - represent last number in enum
 *
 * Comments:
 *         DO NOT CHANGE ORDER.
 */
typedef enum
{
    CPSS_GOOD_OCTETS_RCV_E = 0,
    CPSS_BAD_OCTETS_RCV_E,
    CPSS_MAC_TRANSMIT_ERR_E,
    CPSS_GOOD_PKTS_RCV_E,
    CPSS_BAD_PKTS_RCV_E,
    CPSS_BRDC_PKTS_RCV_E,
    CPSS_MC_PKTS_RCV_E,
    CPSS_PKTS_64_OCTETS_E,
    CPSS_PKTS_65TO127_OCTETS_E,
    CPSS_PKTS_128TO255_OCTETS_E,
    CPSS_PKTS_256TO511_OCTETS_E,
    CPSS_PKTS_512TO1023_OCTETS_E,
    CPSS_PKTS_1024TOMAX_OCTETS_E,
    CPSS_GOOD_OCTETS_SENT_E,
    CPSS_GOOD_PKTS_SENT_E,
    CPSS_EXCESSIVE_COLLISIONS_E,
    CPSS_MC_PKTS_SENT_E,
    CPSS_BRDC_PKTS_SENT_E,
    CPSS_UNRECOG_MAC_CNTR_RCV_E,
    CPSS_FC_SENT_E,
    CPSS_GOOD_FC_RCV_E,
    CPSS_DROP_EVENTS_E,
    CPSS_UNDERSIZE_PKTS_E,
    CPSS_FRAGMENTS_PKTS_E,
    CPSS_OVERSIZE_PKTS_E,
    CPSS_JABBER_PKTS_E,
    CPSS_MAC_RCV_ERROR_E,
    CPSS_BAD_CRC_E,
    CPSS_COLLISIONS_E,
    CPSS_LATE_COLLISIONS_E,
    CPSS_BadFC_RCV_E,  /* Relevant Only for: */
                       /* Twist-D-XG, GT_98EX130A, GT_98MX630A, Samba-XG */
    CPSS_GOOD_UC_PKTS_RCV_E,
    CPSS_GOOD_UC_PKTS_SENT_E,
    CPSS_MULTIPLE_PKTS_SENT_E,
    CPSS_DEFERRED_PKTS_SENT_E,
    CPSS_PKTS_1024TO1518_OCTETS_E,
    CPSS_PKTS_1519TOMAX_OCTETS_E,
    
    CPSS_LAST_MAC_COUNTER_NUM_E

}CPSS_PORT_MAC_COUNTERS_ENT;

/*
 * typedef: struct CPSS_PORT_MAC_COUNTER_SET_STC
 *
 * Description: Counters for BULK API  of the ethernet MAC Counters.
 *              The actual counter size (32 or 64 bits counters) is according
 *              the hardware device, refer to data sheet for more info.
 *
 * Fields:
 *  goodOctetsRcv           - Number of ethernet frames received that are
 *                            not bad ethernet frames or MAC Control pkts.
 *                            This includes Bridge Control packets (LCAP, BPDU)
 *  badOctetsRcv            - Sum of lengths of all bad ethernet frames received
 *  goodPktsRcv             - Number of ethernet frames received that are
 *                            neither bad ethernet frames nor MAC Control pkts.
 *                            This includes Bridge Control packets (LCAP, BPDU)
 *                            Not Supported for DxCh and ExMxPm.
 *  badPktsRcv              - Number of corrupted ethernet frames received
 *                            Not Supported for DxCh and ExMxPm.
 *  brdcPktsRcv             - Total number of undamaged packets received that
 *                            were directed to the broadcast address
 *  mcPktsRcv               - Total number of undamaged packets received that
 *                            were directed to a multicast address
 *  goodPktsSent            - Number of ethernet frames sent from this MAC.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *                            Not Supported for DxCh and ExMxPm.
 *  goodOctetsSent          - Sum of lengths of all good ethernet frames sent
 *                            from this MAC.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  brdcPktsSent            - Total number of good packets sent that have a
 *                            broadcast destination MAC address.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  mcPktsSent              - Total number of good packets sent that have a
 *                            multicast destination MAC address.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  pkts64Octets            - Total number of received and transmitted undamaged
 *                            and damaged frames which are 64 bytes in size.
 *                            This does not include MAC Control Frames.
 *  pkts65to127Octets       - Total number of received and transmitted undamaged
 *                            and damaged frames which are 65 to 127 bytes in
 *                            size. This does not include MAC Control Frames.
 *  pkts128to255Octets      - Total number of received and transmitted undamaged
 *                            and damaged frames which are 128 to 255 bytes in
 *                            size. This does not include MAC Control Frames.
 *  pkts256to511Octets      - Total number of received and transmitted undamaged
 *                            and damaged frames which are 256 to 511 bytes in
 *                            size. This does not include MAC Control Frames.
 *  pkts512to1023Octets     - Total number of received and transmitted undamaged
 *                            and damaged frames which are 512 to 1023 bytes in
 *                            size. This does not include MAC Control Frames.
 *  pkts1024tomaxOoctets    - Total number of received and transmitted undamaged
 *                            and damaged frames which are more than 1024 bytes
 *                            in size. This does not include MAC Control Frames.
 *                            (xCat2: the behavior of this counter is determined by
 *                            oversized packets counter mode.
 *                            See: cpssDxChMacOversizedPacketsCounterModeSet.) 
 *  collisions              - Total number of collisions seen by the MAC
 *  lateCollisions          - Total number of late collisions seen by the MAC
 *                            (xCat2: also counts unknown MAC control frames)
 *  excessiveCollisions     - Number of frames dropped in the transmit MAC due
 *                            to excessive collisions. This is an applicable for
 *                            Half-Duplex mode only.
 *  macRcvError             - Number of Rx Error events seen by the receive side
 *                            of the MAC
 *  macTransmitErr          - Number of frames not transmitted correctly or
 *                            dropped due to internal MAC Tx error
 *  badCrc                  - Number of CRC error events.
 *  dropEvents              - Number of instances that the port was unable to
 *                            receive packets due to insufficient bandwidth to
 *                            one of the PP internal resources, such as the DRAM
 *                            or buffer allocation.
 *  undersizePkts           - Number of undersize packets received.
 *  fragmentsPkts           - Number of fragments received.
 *  oversizePkts            - Number of oversize packets received.
 *  jabberPkts              - Number of jabber packets received.
 *  unrecogMacCntrRcv       - Number of received MAC Control frames that have an
 *                            opcode different from 00-01.
 *                            Not Supported for DxCh and ExMxPm.
 *  goodFcRcv               - Number of good Flow Control frames received
 *  fcSent                  - Number of Flow Control frames sent.
 *  badFcRcv                - Number of bad Flow Control frames received.
 *                            Not Supported for DxCh and ExMxPm.
 *  ucPktsRcv               - Number of Ethernet Unicast frames received that
 *                            are not bad Ethernet frames or MAC Control packets.
 *                            Note that this number includes Bridge Control
 *                            packets such as LCAP and BPDU.
 *                            Supported for DxCh and ExMxPm only.
 *  ucPktsSent              - Number of good frames sent that had a Unicast
 *                            destination MAC Address.
 *                            Supported for DxCh and ExMxPm only.
 *  multiplePktsSent        - Valid Frame transmitted on half-duplex link that
 *                            encountered more then one collision.
 *                            Byte count and cast are valid.
 *                            Supported for DxCh and ExMxPm only.
 *  deferredPktsSent       - Valid frame transmitted on half-duplex link with
 *                            no collisions, but where the frame transmission
 *                            was delayed due to media being busy. Byte count
 *                            and cast are valid.
 *                            Supported for DxCh and ExMxPm only.
 *                            (xCat2: the behavior of this counter is determined by
 *                            oversized packets counter mode.
 *                            See: cpssDxChMacOversizedPacketsCounterModeSet.)
 *  pkts1024to1518Octets    - the number of received good and bad frames that
 *                            are 1024 to 1518 bytes in size
 *                            Relevant only for Interlaken channel MIB counter
 *                            (APPLICABLE DEVICES: Bobcat2)
 *  pkts1519toMaxOctets     - the number of received good and bad frames that
 *                            are more than 1519 bytes in size
 *                            Relevant only for Interlaken channel MIB counter
 *                            (APPLICABLE DEVICES: Bobcat2)
 */

typedef struct
{
    GT_U64 goodOctetsRcv;
    GT_U64 badOctetsRcv;
    GT_U64 macTransmitErr;
    GT_U64 goodPktsRcv;
    GT_U64 badPktsRcv;
    GT_U64 brdcPktsRcv;
    GT_U64 mcPktsRcv;
    GT_U64 pkts64Octets;
    GT_U64 pkts65to127Octets;
    GT_U64 pkts128to255Octets;
    GT_U64 pkts256to511Octets;
    GT_U64 pkts512to1023Octets;
    GT_U64 pkts1024tomaxOoctets;
    GT_U64 goodOctetsSent;
    GT_U64 goodPktsSent;
    GT_U64 excessiveCollisions;
    GT_U64 mcPktsSent;
    GT_U64 brdcPktsSent;
    GT_U64 unrecogMacCntrRcv;
    GT_U64 fcSent;
    GT_U64 goodFcRcv;
    GT_U64 dropEvents;
    GT_U64 undersizePkts;
    GT_U64 fragmentsPkts;
    GT_U64 oversizePkts;
    GT_U64 jabberPkts;
    GT_U64 macRcvError;
    GT_U64 badCrc;
    GT_U64 collisions;
    GT_U64 lateCollisions;
    GT_U64 badFcRcv;
    GT_U64 ucPktsRcv;
    GT_U64 ucPktsSent;
    GT_U64 multiplePktsSent;
    GT_U64 deferredPktsSent;
    GT_U64 pkts1024to1518Octets;
    GT_U64 pkts1519toMaxOctets;

} CPSS_PORT_MAC_COUNTER_SET_STC;

/*
 * typedef: enum CPSS_PORT_EGRESS_CNT_MODE_ENT
 *
 * Description: Enumeration for egress counters mode bitmap
 *
 * Enumerations:
 *    CPSS_EGRESS_CNT_PORT_E - Port bit,
 *                             If clear - all ports
 *                             If set - specific port
 *    CPSS_EGRESS_CNT_VLAN_E - VLAN bit
 *                             If clear - all vlans
 *                             If set - specific vlan
 *    CPSS_EGRESS_CNT_TC_E   - Traffic class bit
 *                             If clear - all priorities
 *                             If set - specific user priority
 *    CPSS_EGRESS_CNT_DP_E   - Drop precedence bit,
 *                             If clear - all DP levels,
 *                             If set - specific DP level
 */
typedef enum
{
    CPSS_EGRESS_CNT_PORT_E = 1,
    CPSS_EGRESS_CNT_VLAN_E = 2,
    CPSS_EGRESS_CNT_TC_E   = 4,
    CPSS_EGRESS_CNT_DP_E   = 8
} CPSS_PORT_EGRESS_CNT_MODE_ENT;

/*
 * typedef: struct CPSS_PORT_EGRESS_CNTR_STC
 *
 * Description: Structure of egress counters.
 *
 * Fields:
 *   outUcFrames      - Number of unicast packets transmitted.
 *   outMcFrames      - Number of multicast packets transmitted. This includes
 *                      registered multicasts, unregistered multicasts and
 *                      unknown unicast packets.
 *   outBcFrames      - Number of broadcast packets transmitted.
 *   brgEgrFilterDisc - Number of IN packets that were Bridge Egress filtered.
 *                      Counts regardless of port, priority or DP configured in
 *                      counter mode.
 *   txqFilterDisc    - Number of IN packets that were filtered due to TxQ
 *                      congestion.
 *   outCtrlFrames    - ExMxPm and DxCh devices:
 *                      Number of out control packets (to cpu, from cpu and to
 *                      analyzer).
 *   egrFrwDropFrames - CH2 and above: Number of packets dropped due to
 *                      egress forwardimg restrictions.
 *   mcFifo3_0DropPkts - counts number of packets dropped due to full TXQ 
 *                       multicast FIFO 3..0. Multicast packets discarded due to
 *                       Congestion in the L2 Replication engine. The discarded 
 *                       descriptors are counted before L2 replications 
 *                       (APPLICABLE DEVICES: Bobcat2)
 *   mcFifo7_4DropPkts - counts number of packets dropped due to full TXQ 
 *                       multicast FIFO 7..4. Multicast packets discarded due to
 *                       Congestion in the L2 Replication engine. The discarded 
 *                       descriptors are counted before L2 replications 
 *                       (APPLICABLE DEVICES: Bobcat2)
 *
 */
typedef struct
{
    GT_U32 outUcFrames;
    GT_U32 outMcFrames;
    GT_U32 outBcFrames;
    GT_U32 brgEgrFilterDisc;
    GT_U32 txqFilterDisc;
    GT_U32 outCtrlFrames;
    GT_U32 egrFrwDropFrames;
    GT_U32 mcFifo3_0DropPkts;
    GT_U32 mcFifo7_4DropPkts;
} CPSS_PORT_EGRESS_CNTR_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssPortStath */

