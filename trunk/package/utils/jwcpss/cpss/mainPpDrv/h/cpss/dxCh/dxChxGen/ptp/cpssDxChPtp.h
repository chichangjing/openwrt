
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPtp.h
*
* DESCRIPTION:
*       CPSS DxCh Precision Time Protocol APIs
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*
*******************************************************************************/

#ifndef __cpssDxChPtph
#define __cpssDxChPtph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>



/*
 * typedef: CPSS_DXCH_PTP_TAI_INSTANCE_ENT
 *
 *  Description:
 *     Instance of TAI type (contains 2 TAI Units).
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E      - Global Instance Only
 *     CPSS_DXCH_PTP_TAI_INSTANCE_PORT_E        - Instance identified by port
 *               There is one instance per several ports - typically for 4 ports.
 *     CPSS_DXCH_PTP_TAI_INSTANCE_ALL_E         - All Instances
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E,
    CPSS_DXCH_PTP_TAI_INSTANCE_PORT_E,
    CPSS_DXCH_PTP_TAI_INSTANCE_ALL_E
} CPSS_DXCH_PTP_TAI_INSTANCE_ENT;

/*
 * typedef: CPSS_DXCH_PTP_TAI_NUMBER_ENT
 *
 *  Description:
 *     Number of TAI in Instance.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TAI_NUMBER_0_E      - TAI0
 *     CPSS_DXCH_PTP_TAI_NUMBER_1_E      - TAI1
 *     CPSS_DXCH_PTP_TAI_NUMBER_ALL_E    - All TAIs
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TAI_NUMBER_0_E,
    CPSS_DXCH_PTP_TAI_NUMBER_1_E,
    CPSS_DXCH_PTP_TAI_NUMBER_ALL_E
} CPSS_DXCH_PTP_TAI_NUMBER_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_TAI_ID_STC
 *
 * Description: Structure for TAI Identification.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *  taiInstance    - TAI Instance
 *  taiNumber      - TAI Number
 *  portNum        - Physical Port Number
 *
 */
typedef struct{
    CPSS_DXCH_PTP_TAI_INSTANCE_ENT      taiInstance;
    CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber;
    GT_PHYSICAL_PORT_NUM                portNum;
} CPSS_DXCH_PTP_TAI_ID_STC;

/*
 * typedef: CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT
 *
 *  Description:
 *     This enum defines possible TOD (Time Of Day) counter functions.
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E               - Copy the value from the TOD
 *                              counter shadow to the TOD counter register
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E            - Add the value of the TOD
 *                              counter shadow to the TOD counter register
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E              - Copy the value of the TOD
 *                              counter to the TOD counter shadow register
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E             - Generate a pulse on the
 *                              external interface at the configured time,
 *                              determined by the TOD counter shadow
 *                              (APPLICABLE DEVICES: Lion, xCat2, Lion2.)
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_DECREMENT_E            - update the value of the TOD Counter
 *                              by subtracting a pre-configured offset.
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_INCREMENT_E - as INCREMENT, but the TOD Counter
 *                              modified by a pre-configured offset over a given period of time.
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_DECREMENT_E - as DECREMENT, but the TOD Counter
 *                              modified by a pre-configured offset over a given period of time.
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_FREQUENCE_UPDATE_E     - Copy the value from the TOD
 *                              counter shadow to the fractional nanosecond drift register
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *     CPSS_DXCH_PTP_TOD_COUNTER_FUNC_NOP_E                  - No Operation.
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E,
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E,
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E,
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E,
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_DECREMENT_E,
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_INCREMENT_E,
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_DECREMENT_E,
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_FREQUENCE_UPDATE_E,
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_NOP_E
} CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_TOD_COUNT_STC
 *
 * Description: Structure for TOD Counter.
 *
 * Fields:
 *  nanoSeconds     - nanosecond value of the time stamping TOD counter
 *  seconds         - second value of the time stamping TOD counter
 *                    48 LSBits are significant.
 *  fracNanoSeconds - fractional nanosecond part of the value.
 *                    30 MSBits are significant.
 *                    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 */
typedef struct{
    GT_U32      nanoSeconds;
    GT_U64      seconds;
    GT_U32      fracNanoSeconds;
} CPSS_DXCH_PTP_TOD_COUNT_STC;

/*
 * typedef: struct CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC
 *
 * Description: Structure for Captured Timestamp Entry.
 *
 * Fields:
 *  portNum      -  PTP source/target port number
 *                  Note: In ingress time stamping this field specifies the
 *                        source port of the incoming PTP packet.
 *                        In egress time stamping this field specifies the
 *                        target port of the outgoing PTP packet.
 *  messageType  -  PTP message type from PTP header.
 *  sequenceId   -  The PTP Sequence ID of the PTP packet.
 *  timeStampVal -  The PTP timestamp value, contains the nanosecond field of
 *                  the TOD counter.
 *                  Note: Bits 31:30 are always 0.
 *
 */
typedef struct{
    GT_U8       portNum;
    GT_U32      messageType;
    GT_U32      sequenceId;
    GT_U32      timeStampVal;
} CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC;


/*============== Bobcat2, Caelum, Bobcat3 Data Types =============*/

/*
 * typedef: CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT
 *
 *  Description:
 *     This enum defines type of value of TOD (Time Of Day) counter.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E      -   The value of the TOD
 *                when the trigger generated for external device.
 *     CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E -   Generate function Mask.
 *     CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E              -   The value to update the TOD
 *                by loading it instead the current TOD
 *                or adding it to the current TOD
 *                or subtracting it from the current TOD.
 *     CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE0_E          -   Copy of the value of the TOD
 *                made by Capture command.
 *     CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE1_E          -   Copy of the value of the TOD
 *                made by Capture command.
 *
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E,
    CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E,
    CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E,
    CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE0_E,
    CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE1_E
} CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT;

/*
 * typedef: CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT
 *
 *  Description:
 *     This enum defines the clock mode.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TAI_CLOCK_MODE_DISABLED_E      - Clock connection disabled
 *     CPSS_DXCH_PTP_TAI_CLOCK_MODE_OUTPUT_E        - Generate external clock
 *               relevant to Global TAI Instance only.
 *     CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_E         - Receive clock and update TOD
 *     CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_ADJUST_E  - Receive clock and update TOD
 *               and adjust frequency
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TAI_CLOCK_MODE_DISABLED_E,
    CPSS_DXCH_PTP_TAI_CLOCK_MODE_OUTPUT_E,
    CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_E,
    CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_ADJUST_E
} CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_TAI_TOD_STEP_STC
 *
 * Description: Structure for value of TOD Step.
 *                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *  nanoSeconds -     whole nanosecond part of the value
 *                    (APPLICABLE RANGES: Bobcat2 0..0xFFFFFFFF;
 *                                        Caelum, Bobcat3 0..0xFFFF)
 *  fracNanoSeconds - fractional nanosecond part of the value.
 *                    Value measured in (1/(2^32)) units.
 *
 */
typedef struct{
    GT_U32      nanoSeconds;
    GT_U32      fracNanoSeconds;
} CPSS_DXCH_PTP_TAI_TOD_STEP_STC;

/*
 * typedef: struct CPSS_DXCH_PTP_TSU_CONTROL_STC
 *
 * Description: Structure for TSU (Time Stamping Unit)  Control.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *  unitEnable          - GT_TRUE  - the time stamping unit in the MAC enabled.
 *                        GT_FALSE - the unit disabled.
 *  rxTaiSelect         - select TAI for ingress time stamping.
 *                        (APPLICABLE RANGES: 0..1.)
 *  tsQueOverrideEnable - GT_TRUE  - if a timestamp has to be written to a full queue,
 *                        the oldest entry in the queue is overwritten,
 *                        and an interrupt is sent to the CPU.
 *                        GT_FALSE - if a timestamp needs to be written to a full queue,
 *                        the timestamp is not written.
 *
 */
typedef struct{
    GT_BOOL                           unitEnable;
    GT_U32                            rxTaiSelect;
    GT_BOOL                           tsQueOverrideEnable;
} CPSS_DXCH_PTP_TSU_CONTROL_STC;

/*
 * typedef: struct CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC
 *
 * Description: Structure for TSU (Time Stamping Unit)  TX Timestamp Queue Entry.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *  entryValid          - GT_TRUE  - entry is valid
 *                        GT_FALSE - Queue is empty.
 *  entryId             - This field is used to match between the egress timestamp
 *                        queue entry and the per-port timestamp entry.
 *                        The same <Queue Entry ID> is also stored in the
 *                        per-port timestamp queue.
 *  taiSelect           - selected TAI. (APPLICABLE RANGES: 0..1.)
 *  todUpdateFlag       - The least significant bit of the <Time Update Counter>
 *                        in the TAI, at the time that <Timestamp> is captured.
 *  timestamp           - timestamp
 *
 */
typedef struct{
    GT_BOOL                           entryValid;
    GT_U32                            entryId;
    GT_U32                            taiSelect;
    GT_U32                            todUpdateFlag;
    GT_U32                            timestamp;
} CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC;

/*
 * typedef: CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT
 *
 *  Description:
 *     This enum defines the packet counter type.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_TOTAL_E    - Counts the total number
 *                of packets that are subject to any kind of time stamping action in
 *                the egress time stamping unit (TSU).
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V1_E      - Counts PTP V1 packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V2_E      - Counts PTP V2 packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_Y1731_E       - Counts Y1731 packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TS_E      - Counts NTP Timestamp packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_RX_E      - Counts NTP received packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TX_E      - Counts NTP transmitted packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_WAMP_E        - Counts WAMP packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NONE_ACTION_E - Counts None Action packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_FORWARD_E     - Counts Forwarded packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_DROP_E        - Counts Dropped packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_E     - Counts Captured packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_TIME_E    - Counts Add Time Action packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_CORR_TIME_E               - Counts Add
 *                Corrected Time Action packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_TIME_E            - Counts Captured
 *                Add Time Action packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_CORR_TIME_E       - Counts Captured
 *                Add Corrected Time Action packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_INGRESS_TIME_E            - Counts
 *                Add Ingress Time Action packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_INGRESS_TIME_E    - Counts
 *                Captured Add Ingress Time Action packets
 *        CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_INGRESS_TIME_E        - Counts
 *                Captured Ingress Time Action packets
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_TOTAL_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V1_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V2_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_Y1731_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TS_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_RX_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TX_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_WAMP_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NONE_ACTION_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_FORWARD_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_DROP_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_TIME_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_CORR_TIME_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_TIME_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_CORR_TIME_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_INGRESS_TIME_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_INGRESS_TIME_E,
    CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_INGRESS_TIME_E
} CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC
 *
 * Description: Structure Timestamp Tag Global Configuration.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *  tsTagParseEnable           - GT_TRUE  - packets with Ethertype == tsTagEtherType are identified
 *                               as timestamp tagged with Extended or Non-Extended Tag.
 *                               GT_FALSE - no packets are identified
 *                               as timestamp tagged with Extended or Non-Extended Tag.
 *  hybridTsTagParseEnable     - GT_TRUE  - packets with Ethertype == hybridTsTagEtherType
 *                               are identified as timestamp tagged with Hybrid Tag.
 *                               GT_FALSE - no packets
 *                               are identified as timestamp tagged with Hybrid Tag.
 *  tsTagEtherType             - Ethertype of Extended and Non Extended TS Tags.
 *                               (APPLICABLE RANGES: 0..0xFFFF.)
 *  hybridTsTagEtherType       - Ethertype of Hybrid TS Tags.
 *                               (APPLICABLE RANGES: 0..0xFFFF.)
 *
 */
typedef struct{
    GT_BOOL         tsTagParseEnable;
    GT_BOOL         hybridTsTagParseEnable;
    GT_U32          tsTagEtherType;
    GT_U32          hybridTsTagEtherType;
} CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC;

/*
 * typedef: CPSS_DXCH_PTP_TS_TAG_MODE_ENT
 *
 *  Description:
 *     This enum defines the Timestamp Tag mode.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TS_TAG_MODE_NONE_E              - Timestamps not added to any packets.
 *     CPSS_DXCH_PTP_TS_TAG_MODE_ALL_NON_EXTENDED_E  - Timestamps (TS Tags) added
 *              to all packets in non-extended format.
 *     CPSS_DXCH_PTP_TS_TAG_MODE_ALL_EXTENDED_E      - Timestamps (TS Tags) added
 *              to all packets in extended format.
 *     CPSS_DXCH_PTP_TS_TAG_MODE_NON_EXTENDED_E      - Timestamps added
 *              only to time protocol packets (PTP, DM) as non-extended TS Tags.
 *     CPSS_DXCH_PTP_TS_TAG_MODE_EXTENDED_E          - Timestamps added
 *              only to time protocol packets (PTP, DM) as extended TS Tags.
 *     CPSS_DXCH_PTP_TS_TAG_MODE_PIGGYBACK_E         - Timestamps added
 *              only to PTP packets, using the reserved field in the PTP header.
 *              Valid only for PTP header of version v2.
 *     CPSS_DXCH_PTP_TS_TAG_MODE_HYBRID_E            - Timestamps added
 *              only to PTP packets, using the hybrid TS tag.
 *              Valid only for PTP header of version v2.
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TS_TAG_MODE_NONE_E,
    CPSS_DXCH_PTP_TS_TAG_MODE_ALL_NON_EXTENDED_E,
    CPSS_DXCH_PTP_TS_TAG_MODE_ALL_EXTENDED_E,
    CPSS_DXCH_PTP_TS_TAG_MODE_NON_EXTENDED_E,
    CPSS_DXCH_PTP_TS_TAG_MODE_EXTENDED_E,
    CPSS_DXCH_PTP_TS_TAG_MODE_PIGGYBACK_E,
    CPSS_DXCH_PTP_TS_TAG_MODE_HYBRID_E
} CPSS_DXCH_PTP_TS_TAG_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC
 *
 * Description: Structure Timestamp Port Configuration.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *      tsReceptionEnable  - Time for ingress timestamp based on :
 *                           GT_TRUE  - the timestamp tag.
 *                           GT_FALSE - the reception time in the current device.
 *      tsPiggyBackEnable  - ingress PTP V2 packets header reserved field Timestamp use:
 *                           GT_TRUE  - contains Piggy Back Timestamp.
 *                           GT_FALSE - contains data not supported by the unit.
 *      tsTagMode          - Mode of time stamping the egress packets.
 *
 */
typedef struct{
    GT_BOOL                          tsReceptionEnable;
    GT_BOOL                          tsPiggyBackEnable;
    CPSS_DXCH_PTP_TS_TAG_MODE_ENT    tsTagMode;
} CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC;

/*
 * typedef: CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT
 *
 *  Description:
 *     This enum defines the TAI selection mode.
 *     Used for selection TAI0 or TAI1 for PTP Header Correction Field.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_INGRESS_E  - use the same TAI as for ingress
 *              Used when the ingress time received by TAI (of current or remote device).
 *     CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_EGRESS_E   - TAI selected from
 *              per-target-port configuration.
 *              Used when the ingress time received by Timestamp TAG from PHY.
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_INGRESS_E,
    CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_EGRESS_E
} CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT;

/*
 * typedef: CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT
 *
 *  Description:
 *     This enum defines Timestamping Mode.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E  - Do Action
 *     CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E - Modify Timestamp Tag
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E,
    CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E
} CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT;

/*
 * typedef: CPSS_DXCH_PTP_TS_ACTION_ENT
 *
 *  Description:
 *     This enum defines the PTP Timestamp Action.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TS_ACTION_NONE_E                        - no operation (forward)
 *     CPSS_DXCH_PTP_TS_ACTION_DROP_E                        - drop
 *     CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E                     - capture
 *     CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E                    - add time
 *     CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E          - add corrected time
 *     CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E            - capture and add time
 *     CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E  - capture and add corrected time
 *     CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E            - add ingress time
 *     CPSS_DXCH_PTP_TS_ACTION_CAPTURE_INGRESS_TIME_E        - capture ingress time
 *     CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_INGRESS_TIME_E    - capture and add ingress time
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TS_ACTION_NONE_E,
    CPSS_DXCH_PTP_TS_ACTION_DROP_E,
    CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E,
    CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E,
    CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E,
    CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E,
    CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E,
    CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E,
    CPSS_DXCH_PTP_TS_ACTION_CAPTURE_INGRESS_TIME_E,
    CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_INGRESS_TIME_E
} CPSS_DXCH_PTP_TS_ACTION_ENT;

/*
 * typedef: CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT
 *
 *  Description:
 *     This enum defines the packet type.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *        CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E      - PTP V1 packets
 *        CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E      - PTP V2 packets
 *        CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E       - Y1731 packets
 *        CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TS_E      - NTP packets timestaming
 *        CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_RX_E      - NTP received packets
 *        CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TX_E      - NTP transmitted packets
 *        CPSS_DXCH_PTP_TS_PACKET_TYPE_WAMP_E        - WAMP packets
 *         CPSS_DXCH_PTP_TS_PACKET_TYPE_RESERVED_E   - reserved
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E,
    CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E,
    CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E,
    CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TS_E,
    CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_RX_E,
    CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TX_E,
    CPSS_DXCH_PTP_TS_PACKET_TYPE_WAMP_E,
    CPSS_DXCH_PTP_TS_PACKET_TYPE_RESERVED_E
} CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT;

/*
 * typedef: CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT
 *
 *  Description:
 *     This enum defines PTP Transport.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E  - Ethernet
 *     CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV4_E  - UDP over IPV4
 *     CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV6_E  - UDP over IPV6
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E,
    CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV4_E,
    CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV6_E
} CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_TS_CFG_ENTRY_STC
 *
 * Description: Structure Timestamp Configuration Table Entry.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *      tsMode             - Timestamping mode.
 *      offsetProfile      - Offset Profile.
 *                           relevant if tsMode==CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E
 *                           field to upate in Extended and Non-Extended TS Tag
 *                           (APPLICABLE RANGES: 0..127.)
 *      OE                 - Offset Enable.
 *                           relevant if tsMode==CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E
 *                           field to upate in Extended and Non-Extended TS Tag
 *                           GT_TRUE - enable, GT_FALSE - disable.
 *      tsAction           - Timestamping Action.
 *                           relevant if tsMode==CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E
 *      packetFormat       - Packet Format.
 *                           relevant if tsMode==CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E
 *                           or packets egressed without Extended and Non-Extended TS Tag
 *      ptpTransport       - PTP Transport.
 *                           relevant if tsMode==CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E
 *                           or packets egressed without Extended and Non-Extended TS Tag
 *      offset             - The offset relative to the beginning of the L3 header.
 *                           The offset is measured in bytes.
 *                           relevant if tsMode==CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E
 *                           or packets egressed without Extended and Non-Extended TS Tag
 *                           (APPLICABLE RANGES: 0..255.)
 *      ptpMessageType     - The PTP message type. Affects the <Ingress Link Delay En>,
 *                           and the <Message Type> in the timestamp queues
 *                           relevant if tsMode==CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E
 *                           and packetFormat is
 *                           CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E or
 *                           CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E.
 *                           (APPLICABLE RANGES: 0..15.)
 *      domain             - PTP message domain index.
 *                           relevant if tsMode==CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E
 *                           and packetFormat is
 *                           CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E or
 *                           CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E.
 *                           (APPLICABLE RANGES: 0..4.)
 *      ingrLinkDelayEnable - Ingress Link Delay Enable.
 *                           Ingress link delay is considered in the ingress timestamp computation.
 *                           GT_TRUE - enable, GT_FALSE - disable.
 *                           This attribute is implicitly enabled for non-PTP packets.
 *      packetDispatchingEnable - Packet Dispatching Enable.
 *                           Typically enabled for 1-step timestamps.
 *                           When enabled, the packet is scheduled to be transmitted at
 *                           the precise time that was embedded in the packet.
 *                           GT_TRUE - enable, GT_FALSE - disable.
 *
 */
typedef struct{
    CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_ENT    tsMode;
    GT_U32                                    offsetProfile;
    GT_BOOL                                   OE;
    CPSS_DXCH_PTP_TS_ACTION_ENT               tsAction;
    CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT          packetFormat;
    CPSS_DXCH_PTP_TRANSPORT_TYPE_ENT          ptpTransport;
    GT_U32                                    offset;
    GT_U32                                    ptpMessageType;
    GT_U32                                    domain;
    GT_BOOL                                   ingrLinkDelayEnable;
    GT_BOOL                                   packetDispatchingEnable;
} CPSS_DXCH_PTP_TS_CFG_ENTRY_STC;

/*
 * typedef: struct CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC
 *
 * Description: Structure Timestamp Local Action Table Entry.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *     tsAction            - Timestamping Action.
 *                           Supported values are:
 *                           CPSS_DXCH_PTP_TS_ACTION_NONE_E
 *                           CPSS_DXCH_PTP_TS_ACTION_DROP_E
 *                           CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E
 *                           CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E
 *                           CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E
 *                           CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E
 *                           CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E
 *     ingrLinkDelayEnable - Ingress Link Delay Enable.
 *                           Ingress link delay is considered in the ingress timestamp computation.
 *                           GT_TRUE - enable, GT_FALSE - disable.
 *                           This attribute is implicitly enabled for non-PTP packets.
 *     packetDispatchingEnable - Packet Dispatching Enable.
 *                           Typically enabled for 1-step timestamps.
 *                           When enabled, the packet is scheduled to be transmitted at
 *                           the precise time that was embedded in the packet.
 *                           GT_TRUE - enable, GT_FALSE - disable.
 *
 */
typedef struct{
    CPSS_DXCH_PTP_TS_ACTION_ENT               tsAction;
    GT_BOOL                                   ingrLinkDelayEnable;
    GT_BOOL                                   packetDispatchingEnable;
} CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC;

/*
 * typedef: CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT
 *
 *  Description:
 *     This enum defines  UDP checksum update modes.
 *     Related to updated timestamps inside UDP payload.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_CLEAR_E        - clear UDP Checksum
 *     CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_RECALCULATE_E  - recalculate UDP Checksum
 *     CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_KEEP_E         - keep original UDP Checksum
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_CLEAR_E,
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_RECALCULATE_E,
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_KEEP_E
} CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC
 *
 * Description: Structure Timestamp UDP Checksum Update Modes.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *     ptpIpv4Mode            - UDP Checksum Update Mode for PTP over IPV4.
 *     ptpIpv6Mode            - UDP Checksum Update Mode for PTP over IPV6.
 *     ntpIpv4Mode            - UDP Checksum Update Mode for NTP over IPV4.
 *     ntpIpv6Mode            - UDP Checksum Update Mode for NTP over IPV6.
 *     wampIpv4Mode           - UDP Checksum Update Mode for WAMP over IPV4.
 *     wampIpv6Mode           - UDP Checksum Update Mode for WAMP over IPV6.
 *
 */
typedef struct{
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT  ptpIpv4Mode;
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT  ptpIpv6Mode;
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT  ntpIpv4Mode;
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT  ntpIpv6Mode;
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT  wampIpv4Mode;
    CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_ENT  wampIpv6Mode;
} CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC;

/*
 * typedef: CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT
 *
 *  Description:
 *     This enum defines PTP domain mode.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *      CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_DISABLE_E - Disable
 *      CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V1_E  - PTP version 1
 *      CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V2_E  - PTP version 2
 *
 */
typedef enum {
    CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_DISABLE_E,
    CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V1_E,
    CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V2_E
} CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC
 *
 * Description: Structure for Egress Domain Table Entry.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *     ptpOverEhernetTsEnable - PTP Over Ehernet Timestamping Enable.
 *                              GT_TRUE - enable, GT_FALSE - disable.
 *     ptpOverUdpIpv4TsEnable - PTP Over UDP IPV4 Timestamping Enable.
 *                              GT_TRUE - enable, GT_FALSE - disable.
 *     ptpOverUdpIpv6TsEnable - PTP Over UDP IPV6 Timestamping Enable.
 *                              GT_TRUE - enable, GT_FALSE - disable.
 *     messageTypeTsEnableBmp - PTP message Timestamping Enable.
 *                              Bitmap of 16 bits indexed by PTP Message Type.
 *                              1 - enable, 0 - disable.
 *                              (APPLICABLE RANGES: 0..0xFFFF.)
 *     transportSpecificTsEnableBmp - PTP message Timestamping Enable.
 *                              Bitmap of 16 bits indexed by 4-bit Transport Specific value.
 *                              1 - enable, 0 - disable.
 *                              (APPLICABLE RANGES: 0..0xFFFF.)
 *
 */
typedef struct{
    GT_BOOL        ptpOverEhernetTsEnable;
    GT_BOOL        ptpOverUdpIpv4TsEnable;
    GT_BOOL        ptpOverUdpIpv6TsEnable;
    GT_U32         messageTypeTsEnableBmp;
    GT_U32         transportSpecificTsEnableBmp;
} CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC;

/*
 * typedef: CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT
 *
 *  Description:
 *     This enum defines ingress PTP packet checking mode.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BASIC_E       - The Basic Mode, checks that:
 *                The <versionPTP> field is either 1 or 2.
 *                The packet is long enough to include the version,
 *                message type, and domain fields in the PTP header.
 *     CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_TRANS_CLK_E   - The Transparent Clock Mode
 *                Additional to the Basic Mode checks it checks that,
 *                the Correction Field
 *                is within the packet length, and within the first 128B.
 *     CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_PIGGY_BACK_E  - The Piggy Back Mode
 *                Additional to the Basic Mode checks it checks that,
 *                the 32-bit Reserved Field
 *                is within the packet length, and within the first 128B.
 *     CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BOUNDRY_CLK_E -  The Boundary Clock Mode
 *                Additional to the Basic Mode checks it checks that,
 *                the Origin Timestamp Field
 *                is within the packet length, and within the first 128B.
 *
 */
typedef enum {
    CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BASIC_E,
    CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_TRANS_CLK_E,
    CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_PIGGY_BACK_E,
    CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BOUNDRY_CLK_E
} CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC
 *
 * Description: Structure for Ingress Exception Configuration.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *     ptpExceptionCommandEnable - enable PTP command assignment when an exception occurs.
 *                                 GT_TRUE - enable, GT_FALSE - disable.
 *     ptpExceptionCommand       - packet command in case of a PTP exception.
 *                                 valid values
 *                                 CPSS_PACKET_CMD_FORWARD_E
 *                                 CPSS_PACKET_CMD_MIRROR_TO_CPU_E
 *                                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
 *                                 CPSS_PACKET_CMD_DROP_HARD_E
 *                                 CPSS_PACKET_CMD_DROP_SOFT_E
 *     ptpExceptionCpuCode       - the CPU/Drop Code when the PTP exception packet command
 *                                 is not FORWARD.
 *     ptpVersionCheckEnable     - enable PTP version checking.
 *                                 GT_TRUE - enable, GT_FALSE - disable.
 *
 */
typedef struct{
    GT_BOOL                      ptpExceptionCommandEnable;
    CPSS_PACKET_CMD_ENT          ptpExceptionCommand;
    CPSS_NET_RX_CPU_CODE_ENT     ptpExceptionCpuCode;
    GT_BOOL                      ptpVersionCheckEnable;
} CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC;

/*
 * typedef: struct CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC
 *
 * Description: Structure for Egress Exception Configuration.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *        invalidPtpPktCmd          - invalid PTP Packet Command.
 *                valid values DROP and FORWARD only.
 *        invalidOutPiggybackPktCmd - invalid Outgoing Piggyback Packet Command.
 *                valid values DROP and FORWARD only.
 *        invalidInPiggybackPktCmd  - invalid Ingoing Piggyback Packet Command.
 *                valid values DROP and FORWARD only.
 *        invalidTsPktCmd           - invalid Timestamp Packet Command.
 *                valid values DROP and FORWARD only.
 *
 */
typedef struct{
    CPSS_PACKET_CMD_ENT          invalidPtpPktCmd;
    CPSS_PACKET_CMD_ENT          invalidOutPiggybackPktCmd;
    CPSS_PACKET_CMD_ENT          invalidInPiggybackPktCmd;
    CPSS_PACKET_CMD_ENT          invalidTsPktCmd;
} CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC;

/*
 * typedef: struct CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC
 *
 * Description: Structure for Egress Exception Counters.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *        invalidPtpPktCnt          - invalid PTP Packet Counter.
 *        invalidOutPiggybackPktCnt - invalid Outgoing Piggyback Packet Counter.
 *        invalidInPiggybackPktCnt  - invalid Ingoing Piggyback Packet Counter.
 *        invalidTsPktCnt           - invalid Timestamp Packet Counter.
 *
 */
typedef struct{
    GT_U32          invalidPtpPktCnt;
    GT_U32          invalidOutPiggybackPktCnt;
    GT_U32          invalidInPiggybackPktCnt;
    GT_U32          invalidTsPktCnt;
} CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC;

/*
 * typedef: struct CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC
 *
 * Description: Structure for Ingress Timestamp Queue Entry.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *        entryValid    - GT_TRUE  - entry is valid
 *                        GT_FALSE - Queue is empty.
 *        isPtpExeption - GT_TRUE  - a PTP exception was triggered for the current packet
 *                        GT_FALSE - no PTP exception was triggered
 *        packetFormat  - Packet Format
 *        taiSelect                 - used TAI: 0 for TAI0, 1 for TAI1
 *        todUpdateFlag - The least significant bit of the Time Update Counter
 *                        in the TAI, at the time that Timestamp is captured
 *        messageType   - Message Type from PTP header.
 *                        0 for not PTP packets.
 *        domainNum     - Domain Number from PTP header.
 *                        0 for not PTP packets.
 *        sequenceId    - The sequenceID field from the PTP packet header.
 *                        For non-PTP packets it is packet's Flow-ID.
 *        timestamp     - The value of the TOD counter at the times of transmission
 *                        or reception of the packet. This is a
 *                        32 bit nanosecond timestamp in the range -2^30 to 2^30.
 *        portNum       - The local-device physical source port.
 *
 */
typedef struct{
    GT_BOOL                          entryValid;
    GT_BOOL                          isPtpExeption;
    CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT packetFormat;
    GT_U32                           taiSelect;
    GT_U32                           todUpdateFlag;
    GT_U32                           messageType;
    GT_U32                           domainNum;
    GT_U32                           sequenceId;
    GT_32                            timestamp;
    GT_PHYSICAL_PORT_NUM             portNum;
} CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC;

/*
 * typedef: struct CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC
 *
 * Description: Structure for Egress Timestamp Queue Entry.
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Fields:
 *        entryValid    - GT_TRUE  - entry is valid
 *                        GT_FALSE - Queue is empty.
 *        isPtpExeption - GT_TRUE  - a PTP exception was triggered for the current packet
 *                        GT_FALSE - no PTP exception was triggered
 *        packetFormat  - Packet Format
 *        messageType   - Message Type from PTP header.
 *                        0 for not PTP packets.
 *        domainNum     - Domain Number from PTP header.
 *                        0 for not PTP packets.
 *        sequenceId    - The sequenceID field from the PTP packet header.
 *                        For non-PTP packets it is packet's Flow-ID.
 *        queueEntryId  - This field is used to match between the egress timestamp
 *                        queue entry and the per-port timestamp entry.
 *                        The same <Queue Entry ID> is also stored
 *                        in the per-port timestamp queue.
 *                        See cpssDxChPtpTsuTxTimestampQueueRead.
 *        portNum       - The local-device physical target port.
 *
 */
typedef struct{
    GT_BOOL                          entryValid;
    GT_BOOL                          isPtpExeption;
    CPSS_DXCH_PTP_TS_PACKET_TYPE_ENT packetFormat;
    GT_U32                           messageType;
    GT_U32                           domainNum;
    GT_U32                           sequenceId;
    GT_U32                           queueEntryId;
    GT_PHYSICAL_PORT_NUM             portNum;
} CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC;

/*
 * typedef: CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT
 *
 *  Description:
 *     This enum defines mode for output signal to be either PTP PClk or Sync-E Recovered Clock.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_PCLK_E       - PTP clock output mode.
 *     CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_RCVR_CLK_E   - Sync-E recovered clock output mode.
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_PCLK_E,
    CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_RCVR_CLK_E
} CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT;

/*
 * typedef: CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT
 *
 *  Description:
 *     This enum defines TAI input clock selection mode.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 * Enumerations:
 *     CPSS_DXCH_PTP_TAI_CLOCK_SELECT_PTP_PLL_E     - TAI PTP PLL clock selection mode.
 *     CPSS_DXCH_PTP_TAI_CLOCK_SELECT_CORE_PLL_E    - TAI Core PLL clock selection mode.
 *
 */
typedef enum {
    CPSS_DXCH_PTP_TAI_CLOCK_SELECT_PTP_PLL_E,
    CPSS_DXCH_PTP_TAI_CLOCK_SELECT_CORE_PLL_E
} CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT;

/*******************************************************************************
* cpssDxChPtpEtherTypeSet
*
* DESCRIPTION:
*       Configure ethertype0/ethertype1 of PTP over Ethernet packets.
*       A packet is identified as PTP over Ethernet if its EtherType matches
*       one of the configured values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index.
*                         (APPLICABLE RANGES: 0..1.)
*       etherType       - PTP EtherType0 or EtherType1 according to the index.
*                         (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_OUT_OF_RANGE          - on wrong etherType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherTypeIndex,
    IN GT_U32   etherType
);


/*******************************************************************************
* cpssDxChPtpEtherTypeGet
*
* DESCRIPTION:
*       Get ethertypes of PTP over Ethernet packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       etherTypePtr    - (pointer to) PTP EtherType0 or EtherType1 according
*                          to the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEtherTypeGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   etherTypeIndex,
    OUT GT_U32   *etherTypePtr
);

/*******************************************************************************
* cpssDxChPtpUdpDestPortsSet
*
* DESCRIPTION:
*       Configure UDP destination port0/port1 of PTP over UDP packets.
*       A packet is identified as PTP over UDP if it is a UDP packet, whose
*       destination port matches one of the configured ports.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index.
*                         (APPLICABLE RANGES: 0..1.)
*       udpPortNum      - UDP port1/port0 number according to the index,
*                         (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_OUT_OF_RANGE          - on wrong udpPortNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpDestPortsSet
(
    IN GT_U8    devNum,
    IN GT_U32   udpPortIndex,
    IN GT_U32   udpPortNum
);

/*******************************************************************************
* cpssDxChPtpUdpDestPortsGet
*
* DESCRIPTION:
*       Get UDP destination port0/port1 of PTP over UDP packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       udpPortNumPtr     - (pointer to) UDP port0/port1 number, according
*                           to the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpDestPortsGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   udpPortIndex,
    OUT GT_U32   *udpPortNumPtr
);

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdSet
*
* DESCRIPTION:
*       Configure packet command per PTP message type.
*       The message type is extracted from the PTP header.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       command         - assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeCmdSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   domainIndex,
    IN GT_U32                   messageType,
    IN CPSS_PACKET_CMD_ENT      command
);

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdGet
*
* DESCRIPTION:
*       Get packet command per PTP message type.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       commandPtr      - (pointer to) assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeCmdGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_U32                   domainIndex,
    IN  GT_U32                   messageType,
    OUT CPSS_PACKET_CMD_ENT      *commandPtr
);

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseSet
*
* DESCRIPTION:
*       Set CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       cpuCode         - The base of CPU code assigned to PTP packets mirrored
*                         or trapped to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpCpuCodeBaseSet
(
    IN GT_U8                        devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT     cpuCode
);

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseGet
*
* DESCRIPTION:
*       Get CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       cpuCodePtr      - (pointer to) The base of CPU code assigned to PTP
*                         packets mirrored or trapped to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpCpuCodeBaseGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT     *cpuCodePtr
);

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TAI_ID_STC                 *taiIdPtr,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
);

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter
*                          functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
);

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionTriggerSet
(
    IN GT_U8                       devNum,
    IN CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC   *taiIdPtr
);

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionTriggerGet
(
    IN GT_U8                       devNum,
    IN CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC   *taiIdPtr,
    OUT GT_BOOL                    *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - (pointer to) TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
);

/*******************************************************************************
* cpssDxChPtpTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
);

/*******************************************************************************
* cpssDxChPtpTodCounterGet
*
* DESCRIPTION:
*       Get TOD time stamping counter value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterGet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
);

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable port for PTP time stamping.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       portNum         - port number
*                         Note: For Ingress time stamping the trigger is based
*                               on the local source port.
*                               For Egress time stamping the trigger is based
*                               on the local target port.
*       enable          - GT_TRUE - The port is enabled for ingress/egress
*                                   time stamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   time stamping.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTimeStampEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableGet
*
* DESCRIPTION:
*       Get port PTP time stamping status (enabled/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       portNum         - port number
*                         Note: For Ingress time stamping the trigger is based
*                               on the local source port.
*                               For Egress time stamping the trigger is based
*                               on the local target port.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The port is enabled for
*                                               ingress/egress time stamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   time stamping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTimeStampEnableGet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
);


/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping over Ethernet packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       enable          - GT_TRUE - PTP Time stamping is enabled over Ethernet.
*                         GT_FALSE - PTP Time stamping is disabled over Ethernet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEthernetTimeStampEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_BOOL                  enable
);


/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status over Ethernet packets (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled
*                                                over Ethernet.
*                         GT_FALSE - PTP Time stamping is disabled over Ethernet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEthernetTimeStampEnableGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                  *enablePtr
);

/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping over UDP packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       protocolStack   - types of IP to set: IPV4 or IPV6.
*       enable          - GT_TRUE - PTP Time stamping is enabled over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Time stamping is disabled over UDP (IPV4/IPV6).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpTimeStampEnableSet
(
    IN  GT_U8                        devNum,
    IN  CPSS_PORT_DIRECTION_ENT      direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN  GT_BOOL                      enable
);


/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status over UDP packets (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       protocolStack   - types of IP to set: IPV4 or IPV6.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled
*                                               over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Time stamping is disabled over UDP (IPV4/IPV6).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpTimeStampEnableGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT      protocolStack,
    OUT GT_BOOL                         *enablePtr
);

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping for specific message type of the packet.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       enable          - GT_TRUE - PTP Time stamping is enabled for specific message type.
*                         GT_FALSE - PTP Time stamping is disabled for specific message type.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeTimeStampEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled for
*                                                specific message type.
*                         GT_FALSE - PTP Time stamping is disabled for specific
*                                    message type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeTimeStampEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping check of the packet's transport specific value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx, Rx or both directions
*       enable      - GT_TRUE - PTP Time stamping check of the packet's transport
*                               specific value is enabled.
*                     GT_FALSE - PTP Time stamping check of the packet's transport
*                                specific value is disabled.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificCheckEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableGet
*
* DESCRIPTION:
*       Get status of PTP Time stamping check of the packet's transport specific
*       value (the check is enabled/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx or Rx
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - PTP Time stamping check of the
*                                  packet's transport specific value is enabled.
*                     GT_FALSE - PTP Time stamping check of the packet's transport
*                                specific value is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificCheckEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableSet
*
* DESCRIPTION:
*       Enable PTP Time stamping for transport specific value of the packet.
*       Note: Relevant only if global configuration determines whether the
*       transport specific field should be checked by
*       cpssDxChPtpTransportSpecificCheckEnableSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                  - device number
*       direction               - Tx, Rx or both directions
*       transportSpecificVal    - transport specific value taken from PTP header
*                                 bits [7:4].
*                                 (APPLICABLE RANGES: 0..15.)
*       enable                  - GT_TRUE - PTP Time stamping is enabled for this
*                                           transport specific value.
*                                GT_FALSE - PTP Time stamping is disabled for this
*                                           transport specific value.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificTimeStampEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       transportSpecificVal    - transport specific value taken from PTP header
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled for
*                                                this transport specific value.
*                          GT_FALSE - PTP Time stamping is disabled for this
*                                     transport specific value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificTimeStampEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTimestampEntryGet
*
* DESCRIPTION:
*       Read current Timestamp entry from the timestamp FIFO queue.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
* OUTPUTS:
*       entryPtr        - (pointer to) timestamp entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  Timestamp entries
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTimestampEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC   *entryPtr
);



/******************************************************************************/
/******************************************************************************/
/******* start of functions with portGroupsBmp parameter **********************/
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*                                  or function
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_GROUPS_BMP                       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
);

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
);

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT  direction
);

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
);


/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - (pointer to) TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
);

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
);

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterGet
*
* DESCRIPTION:
*       Get TOD time stamping counter value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
);

/*******************************************************************************
* cpssDxChPtpTaiClockModeSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Clock Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockMode       - clock interface using mode.
*                         Output mode relevant to Global TAI Instance only.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT        clockMode
);

/*******************************************************************************
* cpssDxChPtpTaiClockModeGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Clock Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockModePtr    - (pointer to) clock interface using mode.
*                         Output mode relevant to Global TAI Instance only.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT        *clockModePtr
);

/*******************************************************************************
* cpssDxChPtpTaiInternalClockGenerateEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Internal Clock Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          - Internal Clock Generate Enable.
*                         GT_TRUE - the internal clock generator
*                         generates a clock signal
*                         GT_FALSE - the internal clock not generated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInternalClockGenerateEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
);

/*******************************************************************************
* cpssDxChPtpTaiInternalClockGenerateEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Internal Clock Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) Internal Clock Generate Enable.
*                         GT_TRUE - the internal clock generator
*                         generates a clock
*                         GT_FALSE - the internal clock not generated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInternalClockGenerateEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) PTP Clock Adjust Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - PTP PClock Drift Adjust Enable.
*                        GT_TRUE - the PClk is affected by the <Drift Adjustment> or not.
*                        GT_FALSE - the PClk reflects the frequency of the free running TOD,
*                        without any frequency adjustments.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
);

/*******************************************************************************
* cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) PTP Clock Adjust Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) PTP PClock Drift Adjust Enable.
*                        GT_TRUE - the PClk is affected by the <Drift Adjustment> or not.
*                        GT_FALSE - the PClk reflects the frequency of the free running TOD,
*                        without any frequency adjustments.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTaiCaptureOverrideEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Capture Override Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - Capture Override Enable.
*                        When both TOD Capture registers are valid and capture
*                        trigger arrives:
*                        GT_TRUE  - the TOD Capture Value0 is overwritten.
*                        GT_FALSE - the TOD Capture Value0 unchanged.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiCaptureOverrideEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
);

/*******************************************************************************
* cpssDxChPtpTaiCaptureOverrideEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Capture Override Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) Capture Override Enable.
*                        When both TOD Capture registers are valid and capture
*                        trigger arrives:
*                        GT_TRUE  - the TOD Capture Value0 is overwritten.
*                        GT_FALSE - the TOD Capture Value0 unchanged.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiCaptureOverrideEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTaiInputTriggersCountEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Count Input Triggers Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - Count Input Triggers Enable.
*                        GT_TRUE  - each incoming trigger is counted.
*                        GT_FALSE - incoming triggers not counted.
*                        see cpssDxChPtpTaiIncomingTriggerCounterSet/Get
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInputTriggersCountEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
);

/*******************************************************************************
* cpssDxChPtpTaiInputTriggersCountEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Count Input Triggers Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) Count Input Triggers Enable.
*                        GT_TRUE  - each incoming trigger is counted.
*                        GT_FALSE - incoming triggers not counted.
*                        see cpssDxChPtpTaiIncomingTriggerCounterSet/Get
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInputTriggersCountEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTaiExternalPulseWidthSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Pulse Width.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       extPulseWidth  - External Pulse Width.
*                        width of the pulse in nanoseconds of generated output trigger.
*                       (APPLICABLE RANGES: 0..2^28-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on bad extPulseWidth
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiExternalPulseWidthSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_U32                                  extPulseWidth
);

/*******************************************************************************
* cpssDxChPtpTaiExternalPulseWidthGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Pulse Width
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       extPulseWidthPtr -  (pointer to) External Pulse Width.
*                           width of the pulse in nanoseconds of generated output trigger.
*                           (APPLICABLE RANGES: 0..2^28-1.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiExternalPulseWidthGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_U32                                  *extPulseWidthPtr
);

/*******************************************************************************
* cpssDxChPtpTaiTodSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD values.
*       The TOD will be updated by triggering an appropriate function.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todValueType    - type of TOD value.
*                         Valid types are
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E.
*       todValuePtr     - (pointer to) TOD value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This API does not activate any triggers, other APIs does it
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT        todValueType,
    IN  CPSS_DXCH_PTP_TOD_COUNT_STC           *todValuePtr
);

/*******************************************************************************
* cpssDxChPtpTaiTodGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD values.
*       The TOD was captured by triggering an appropriate function.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todValueType    - type of TOD value.
*
* OUTPUTS:
*       todValuePtr     - (pointer to) TOD value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT        todValueType,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC           *todValuePtr
);

/*******************************************************************************
* cpssDxChPtpTaiOutputTriggerEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Output Trigger Generation Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          -  enable output trigger generation.
*                          GT_TRUE  - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       use cpssDxChPtpTaiTodSet with
*       CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E and
*       the time to trigger generation.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiOutputTriggerEnableSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_BOOL                               enable
);

/*******************************************************************************
* cpssDxChPtpTaiOutputTriggerEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Output Trigger Generation Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) enable output trigger generation.
*                         GT_TRUE  - enable GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiOutputTriggerEnableGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_BOOL                               *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTaiTodStepSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todStepPtr      - (pointer to) TOD Step.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodStepSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_STEP_STC    *todStepPtr
);

/*******************************************************************************
* cpssDxChPtpTaiTodStepGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       todStepPtr      - (pointer to) TOD Step.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodStepGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    OUT CPSS_DXCH_PTP_TAI_TOD_STEP_STC    *todStepPtr
);

/*******************************************************************************
* cpssDxChPtpTaiPulseDelaySet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Trigger Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds delay.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPulseDelaySet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
);

/*******************************************************************************
* cpssDxChPtpTaiPulseDelayGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Trigger Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPulseDelayGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
);

/*******************************************************************************
* cpssDxChPtpTaiClockDelaySet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Clock Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds delay.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockDelaySet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
);

/*******************************************************************************
* cpssDxChPtpTaiClockDelayGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Clock Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockDelayGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
);

/*******************************************************************************
* cpssDxChPtpTaiFractionalNanosecondDriftSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Fractional Nanosecond Drift.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       fracNanoSecond  - (pointer to) fractional nano seconds drift.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiFractionalNanosecondDriftSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_32                                 fracNanoSecond
);

/*******************************************************************************
* cpssDxChPtpTaiFractionalNanosecondDriftGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Fractional Nanosecond Drift.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       fracNanoSecondPtr   - (pointer to) fractional nano seconds drift.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiFractionalNanosecondDriftGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_32                                 *fracNanoSecondPtr
);

/*******************************************************************************
* cpssDxChPtpTaiPClockCycleSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) PClock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds cycle.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPClockCycleSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
);

/*******************************************************************************
* cpssDxChPtpTaiPClockCycleGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) PClock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds cycle.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPClockCycleGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
);

/*******************************************************************************
* cpssDxChPtpTaiClockCycleSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Clock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       seconds         - seconds part of cycle.
*                         (APPLICABLE RANGES: 0..3.)
*       nanoSeconds     - nano seconds part of cycle.
*                         (APPLICABLE RANGES: 0..2^28-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockCycleSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                seconds,
    IN  GT_U32                                nanoSeconds
);

/*******************************************************************************
* cpssDxChPtpTaiClockCycleGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Clock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       secondsPtr      - (pointer to) seconds part of cycle.
*                         (APPLICABLE RANGES: 0..3.)
*       nanoSecondsPtr  - (pointer to) nano seconds part of cycle.
*                         (APPLICABLE RANGES: 0..2^28-1.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockCycleGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *secondsPtr,
    OUT GT_U32                                *nanoSecondsPtr
);

/*******************************************************************************
* cpssDxChPtpTaiTodCaptureStatusSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD Capture Status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       captureIndex    - Capture Index.
*                         (APPLICABLE RANGES: 0..1.)
*       valid           - TOD Capture is valid.
*                         GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodCaptureStatusSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                captureIndex,
    IN  GT_BOOL                               valid
);

/*******************************************************************************
* cpssDxChPtpTaiTodCaptureStatusGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Capture Status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       captureIndex    - Capture Index.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       validPtr         - (pointer to)Capture is valid.
*                          GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodCaptureStatusGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                captureIndex,
    OUT GT_BOOL                               *validPtr
);

/*******************************************************************************
* cpssDxChPtpTaiTodUpdateCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Update Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       valuePtr        - (pointer to)TOD Update Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counter is Clear On Read. Value incremented at each
*       TOD Update/Increment/Decrement function.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodUpdateCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *valuePtr
);

/*******************************************************************************
* cpssDxChPtpTaiIncomingTriggerCounterSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Incoming Trigger Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       value           - value to set.
*                         (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiIncomingTriggerCounterSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                value
);

/*******************************************************************************
* cpssDxChPtpTaiIncomingTriggerCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Incoming Trigger Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       valuePtr        - (pointer to)value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiIncomingTriggerCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *valuePtr
);

/*******************************************************************************
* cpssDxChPtpTaiIncomingClockCounterSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Incoming Clock Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          - enable counting.
*                         GT_TRUE - enable, GT_FALSE - disable.
*       value           - value to set.
*                         (APPLICABLE RANGES: 0..2^16-1.)
*                         (APPLICABLE DEVICES: Bobcat2.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter became Read Only and Clear on Read starting from Caelum devices.
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiIncomingClockCounterSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_BOOL                               enable,
    IN  GT_U32                                value
);

/*******************************************************************************
* cpssDxChPtpTaiIncomingClockCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Incoming Clock Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to)enable counting.
*                         GT_TRUE - enable, GT_FALSE - disable.
*       valuePtr        - (pointer to)value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiIncomingClockCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_BOOL                               *enablePtr,
    OUT GT_U32                                *valuePtr
);

/*******************************************************************************
* cpssDxChPtpTaiFrequencyDriftThesholdsSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Frequency Drift Thesholds.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       interruptThreshold - the drift value threshold causing interrupt.
*                            Notifies about the clock frequency out of sync.
*                           (APPLICABLE RANGES: 0..2^24-1.)
*       adjustThreshold    - the drift value threshold that triggers
*                            the adjustment logic. Prevents minor jitters
*                            from affecting the drift adjustment.
*                           (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiFrequencyDriftThesholdsSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                interruptThreshold,
    IN  GT_U32                                adjustThreshold
);

/*******************************************************************************
* cpssDxChPtpTaiFrequencyDriftThesholdsGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Frequency Drift Thesholds.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - device number
*       taiIdPtr              - (pointer to) TAI Units identification.
*                               Single TAI unit must be specified.
*                               For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                               (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       interruptThresholdPtr - (pointer to)the drift value threshold causing interrupt.
*                               Notifies about the clock frequency out of sync.
*                               (APPLICABLE RANGES: 0..2^24-1.)
*       adjustThresholdPtr    - (pointer to)the drift value threshold that triggers
*                               the adjustment logic. Prevents minor jitters
*                               from affecting the drift adjustment.
*                               (APPLICABLE RANGES: 0..255.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiFrequencyDriftThesholdsGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *interruptThresholdPtr,
    OUT GT_U32                                *adjustThresholdPtr
);

/*******************************************************************************
* cpssDxChPtpTaiGracefulStepSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Graceful Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       gracefulStep    - Graceful Step in nanoseconds.
*                         (APPLICABLE RANGES: 0..31.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on wrong gracefulStep
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiGracefulStepSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    IN  GT_U32                             gracefulStep
);

/*******************************************************************************
* cpssDxChPtpTaiGracefulStepGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Graceful Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       gracefulStepPtr - (pointer to) Graceful Step in nanoseconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiGracefulStepGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    OUT GT_U32                            *gracefulStepPtr
);

/*******************************************************************************
* cpssDxChPtpTsuControlSet
*
* DESCRIPTION:
*       Set TSU (Time Stamp Unit) Control.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       controlPtr      - (pointer to) control structure.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuControlSet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    IN  CPSS_DXCH_PTP_TSU_CONTROL_STC     *controlPtr
);

/*******************************************************************************
* cpssDxChPtpTsuControlGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) Control.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*
* OUTPUTS:
*       controlPtr      - (pointer to) control structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuControlGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    OUT CPSS_DXCH_PTP_TSU_CONTROL_STC     *controlPtr
);

/*******************************************************************************
* cpssDxChPtpTsuTxTimestampQueueRead
*
* DESCRIPTION:
*       Read TSU (Time Stamp Unit) TX Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       queueNum        - queue number.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       entryPtr        - (pointer to) TX Timestamp Queue Entry structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Entry deleted from the Queue by reading it.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuTxTimestampQueueRead
(
    IN  GT_U8                                              devNum,
    IN  GT_PHYSICAL_PORT_NUM                               portNum,
    IN  GT_U32                                             queueNum,
    OUT CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC     *entryPtr
);

/*******************************************************************************
* cpssDxChPtpTsuCountersClear
*
* DESCRIPTION:
*       Clear All TSU (Time Stamp Unit) Packet counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.

* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuCountersClear
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum
);

/*******************************************************************************
* cpssDxChPtpTsuPacketCouterGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) packet counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       counterType     - counter type (see enum)
*
* OUTPUTS:
*       valuePtr        - (pointer to) counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuPacketCouterGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    IN  CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT counterType,
    OUT GT_U32                                    *valuePtr
);

/*******************************************************************************
* cpssDxChPtpTsuNtpTimeOffsetSet
*
* DESCRIPTION:
*       Set TSU (Time Stamp Unit) NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       ntpTimeOffset   - NTP PTP Time Offset measured in seconds.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuNtpTimeOffsetSet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    IN  GT_U32                            ntpTimeOffset
);

/*******************************************************************************
* cpssDxChPtpTsuNtpTimeOffsetGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number.
*
* OUTPUTS:
*       ntpTimeOffsetPtr - (pointer to) NTP PTP Time Offset measured in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuNtpTimeOffsetGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    OUT GT_U32                            *ntpTimeOffsetPtr
);

/*******************************************************************************
* cpssDxChPtpTsTagGlobalCfgSet
*
* DESCRIPTION:
*       Set Timestamp Global Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       tsTagGlobalCfgPtr   - (pointer to) TS Tag Global Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsTagGlobalCfgSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *tsTagGlobalCfgPtr
);

/*******************************************************************************
* cpssDxChPtpTsTagGlobalCfgGet
*
* DESCRIPTION:
*       Get Timestamp Global Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       tsTagGlobalCfgPtr   - (pointer to) TS Tag Global Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsTagGlobalCfgGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *tsTagGlobalCfgPtr
);

/*******************************************************************************
* cpssDxChPtpTsTagPortCfgSet
*
* DESCRIPTION:
*       Set Timestamp Port Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number.
*       tsTagPortCfgPtr   - (pointer to) TS Tag Port Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsTagPortCfgSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC   *tsTagPortCfgPtr
);

/*******************************************************************************
* cpssDxChPtpTsTagPortCfgGet
*
* DESCRIPTION:
*       Get Timestamp Port Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number.
*
* OUTPUTS:
*       tsTagPortCfgPtr   - (pointer to) TS Tag Port Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsTagPortCfgGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC   *tsTagPortCfgPtr
);

/*******************************************************************************
* cpssDxChPtpTsDelayIngressPortDelaySet
*
* DESCRIPTION:
*       Set Ingress Port Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       ingressDelayCorr    - Ingress Port Delay Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressPortDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_32                   ingressDelayCorr
);

/*******************************************************************************
* cpssDxChPtpTsDelayIngressPortDelayGet
*
* DESCRIPTION:
*       Get Ingress Port Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*
* OUTPUTS:
*       ingressDelayCorrPtr  - (pointer to) Ingress Port Delay Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressPortDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_32                   *ingressDelayCorrPtr
);

/*******************************************************************************
* cpssDxChPtpTsDelayIngressLinkDelaySet
*
* DESCRIPTION:
*       Set Ingress Link Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile
*                             (APPLICABLE RANGES: 0..8.)
*       ingressDelay        - Ingress Port Delay.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressLinkDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    IN  GT_32                   ingressDelay
);

/*******************************************************************************
* cpssDxChPtpTsDelayIngressLinkDelayGet
*
* DESCRIPTION:
*       Get Ingress Link Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*       domainProfile        - domain profile
*                              (APPLICABLE RANGES: 0..8.)
*
* OUTPUTS:
*       ingressDelayPtr      - (pointer to) Ingress Port Delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressLinkDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    OUT GT_32                   *ingressDelayPtr
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressPipeDelaySet
*
* DESCRIPTION:
*       Set Egress Pipe Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrPipeDelayCorr    - Egress Pipe Delay Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressPipeDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_32                   egrPipeDelayCorr
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressPipeDelayGet
*
* DESCRIPTION:
*       Get Egress Pipe Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*
* OUTPUTS:
*       egrPipeDelayCorrPtr  - (pointer to) Egress Pipe Delay Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressPipeDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_32                   *egrPipeDelayCorrPtr
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet
*
* DESCRIPTION:
*       Set Egress Asymmetry Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       egrAsymmetryCorr    - Egress Asymmetry Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    IN  GT_32                   egrAsymmetryCorr
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet
*
* DESCRIPTION:
*       Get Egress Asymmetry Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*       domainProfile        - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*
* OUTPUTS:
*       egrAsymmetryCorrPtr  - (pointer to) Egress Asymmetry Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    OUT GT_32                   *egrAsymmetryCorrPtr
);

/*******************************************************************************
* cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet
*
* DESCRIPTION:
*       Set Ingress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       ingrCorrFldPBEnable - Ingress Correction Field Piggyback Enable.
*                             GT_TRUE  - Correction field of PTP header contains
*                                        ingress timestamp.
*                             GT_FALSE - Correction field of PTP header reserved.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 ingrCorrFldPBEnable
);

/*******************************************************************************
* cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet
*
* DESCRIPTION:
*       Get Ingress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*
* OUTPUTS:
*       ingrCorrFldPBEnablePtr - (pointer to) Ingress Correction Field Piggyback Enable.
*                                GT_TRUE  - Correction field of PTP header contains
*                                           ingress timestamp.
*                                GT_FALSE - Correction field of PTP header reserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *ingrCorrFldPBEnablePtr
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet
*
* DESCRIPTION:
*       Set Egress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrCorrFldPBEnable  - Egress Correction Field Piggyback Enable.
*                             GT_TRUE  - Correction field of PTP header contains
*                                        ingress timestamp.
*                             GT_FALSE - Correction field of PTP header reserved.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 egrCorrFldPBEnable
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet
*
* DESCRIPTION:
*       Get Egress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*
* OUTPUTS:
*       egrCorrFldPBEnablePtr  - (pointer to) Egress Correction Field Piggyback Enable.
*                                GT_TRUE  - Correction field of PTP header contains
*                                           ingress timestamp.
*                                GT_FALSE - Correction field of PTP header reserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *egrCorrFldPBEnablePtr
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet
*
* DESCRIPTION:
*       Set Egress Time Correction TAI Select Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*       egrTimeCorrTaiSelMode  - Egress Time Correction TAI Select Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    IN  CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT   egrTimeCorrTaiSelMode
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet
*
* DESCRIPTION:
*       Get Egress Time Correction TAI Select Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                    - device number
*       portNum                   - port number.
*
* OUTPUTS:
*       egrTimeCorrTaiSelModePtr  - (pointer to)Egress Time Correction TAI Select Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    OUT CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT   *egrTimeCorrTaiSelModePtr
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimestampTaiSelectSet
*
* DESCRIPTION:
*       Set Egress Timestamp TAI Select.
*
* APPLICABLE DEVICES:
*        Bobcat2; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrTsTaiNum         - Egress Timestamp TAI Number.
*                             (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressTimestampTaiSelectSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  egrTsTaiNum
);

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimestampTaiSelectGet
*
* DESCRIPTION:
*       Get Egress Timestamp TAI Select.
*
* APPLICABLE DEVICES:
*        Bobcat2; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*
* OUTPUTS:
*       egrTsTaiNumPtr  - (pointer to)Egress Timestamp TAI Number.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressTimestampTaiSelectGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *egrTsTaiNumPtr
);

/*******************************************************************************
* cpssDxChPtpTsCfgTableSet
*
* DESCRIPTION:
*       Set Timestamp Configuration Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - entry index.
*                             (APPLICABLE RANGES: 0..127.)
*       entryPtr            - (pointer to)Timestamp Configuration Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsCfgTableSet
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         entryIndex,
    IN  CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *entryPtr
);

/*******************************************************************************
* cpssDxChPtpTsCfgTableGet
*
* DESCRIPTION:
*       Get Timestamp Configuration Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - entry index.
*                             (APPLICABLE RANGES: 0..127.)
*
* OUTPUTS:
*       entryPtr            - (pointer to)Timestamp Configuration Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsCfgTableGet
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         entryIndex,
    OUT CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *entryPtr
);

/*******************************************************************************
* cpssDxChPtpTsLocalActionTableSet
*
* DESCRIPTION:
*       Set Timestamp Local Action Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       messageType         - message type
*                             (APPLICABLE RANGES: 0..15.)
*       entryPtr            - (pointer to)Timestamp Local Action Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsLocalActionTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainProfile,
    IN  GT_U32                                  messageType,
    IN  CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *entryPtr
);

/*******************************************************************************
* cpssDxChPtpTsLocalActionTableGet
*
* DESCRIPTION:
*       Get Timestamp Local Action Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       messageType         - message type
*                             (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       entryPtr            - (pointer to)Timestamp Local Action Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsLocalActionTableGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainProfile,
    IN  GT_U32                                  messageType,
    OUT CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *entryPtr
);

/*******************************************************************************
* cpssDxChPtpTsUdpChecksumUpdateModeSet
*
* DESCRIPTION:
*       Set Timestamp UDP Checksum Update Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       udpCsUpdModePtr     - (pointer to)Timestamp UDP Checksum Update Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsUdpChecksumUpdateModeSet
(
    IN  GT_U8                                         devNum,
    IN  CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *udpCsUpdModePtr
);

/*******************************************************************************
* cpssDxChPtpTsUdpChecksumUpdateModeGet
*
* DESCRIPTION:
*       Get Timestamp UDP Checksum Update Mode.
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
*       udpCsUpdModePtr     - (pointer to)Timestamp UDP Checksum Update Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsUdpChecksumUpdateModeGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *udpCsUpdModePtr
);

/*******************************************************************************
* cpssDxChPtpOverEthernetEnableSet
*
* DESCRIPTION:
*       Set enable PTP over Ethernet packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpOverEthernetEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPtpOverEthernetEnableGet
*
* DESCRIPTION:
*       Get enable PTP over Ethernet packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpOverEthernetEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPtpOverUdpEnableSet
*
* DESCRIPTION:
*       Set enable PTP over UDP packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpOverUdpEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPtpOverUdpEnableGet
*
* DESCRIPTION:
*       Get enable PTP over UDP packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpOverUdpEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPtpDomainModeSet
*
* DESCRIPTION:
*       Set PTP domain mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain number.
*                        (APPLICABLE RANGES: 0..3.)
*       domainMode     - domain mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainModeSet
(
    IN  GT_U8                                  devNum,
    IN  GT_U32                                 domainIndex,
    IN  CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT  domainMode
);

/*******************************************************************************
* cpssDxChPtpDomainModeGet
*
* DESCRIPTION:
*       Get PTP domain mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainModePtr   - (pointer to) domain mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainModeGet
(
    IN  GT_U8                                  devNum,
    IN  GT_U32                                 domainIndex,
    OUT CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT  *domainModePtr
);

/*******************************************************************************
* cpssDxChPtpDomainV1IdSet
*
* DESCRIPTION:
*       Set PTP V1 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain index.
*                        (APPLICABLE RANGES: 0..3.)
*       domainIdArr    - domain Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V1 Header contains 128-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainV1IdSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    IN  GT_U32       domainIdArr[4]
);

/*******************************************************************************
* cpssDxChPtpDomainV1IdGet
*
* DESCRIPTION:
*       Get PTP V1 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainIdArr     - domain Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V1 Header contains 128-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainV1IdGet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    OUT GT_U32       domainIdArr[4]
);

/*******************************************************************************
* cpssDxChPtpDomainV2IdSet
*
* DESCRIPTION:
*       Set PTP V2 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain index.
*                        (APPLICABLE RANGES: 0..3.)
*       domainId       - domain Id
*                        (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V2 Header contains 8-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainV2IdSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    IN  GT_U32       domainId
);

/*******************************************************************************
* cpssDxChPtpDomainV2IdGet
*
* DESCRIPTION:
*       Get PTP V2 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainIdPtr     - (pointer to) domain Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V2 Header contains 8-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainV2IdGet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    OUT GT_U32       *domainIdPtr
);

/*******************************************************************************
* cpssDxChPtpEgressDomainTableSet
*
* DESCRIPTION:
*       Set Egress Domain Table Entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainIndex         - domain index.
*                             (APPLICABLE RANGES: 0..4.)
*       entryPtr            - (pointer to) Domain Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressDomainTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainIndex,
    IN  CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* cpssDxChPtpEgressDomainTableGet
*
* DESCRIPTION:
*       Get Egress Domain Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainIndex         - domain index.
*                             (APPLICABLE RANGES: 0..4.)
*
* OUTPUTS:
*       entryPtr            - (pointer to) Egress Domain Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressDomainTableGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainIndex,
    OUT CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* cpssDxChPtpIngressExceptionCfgSet
*
* DESCRIPTION:
*       Set PTP packet Ingress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       ingrExceptionCfgPtr  - (pointer to) PTP packet Ingress Exception Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressExceptionCfgSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC   *ingrExceptionCfgPtr
);

/*******************************************************************************
* cpssDxChPtpIngressExceptionCfgGet
*
* DESCRIPTION:
*       Get PTP packet Ingress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       ingrExceptionCfgPtr  - (pointer to) PTP packet Ingress Exception Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressExceptionCfgGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC   *ingrExceptionCfgPtr
);

/*******************************************************************************
* cpssDxChPtpIngressPacketCheckingModeSet
*
* DESCRIPTION:
*       Set PTP packet Ingress Checking Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*       checkingMode    - PTP packet ingress checking mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressPacketCheckingModeSet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   domainIndex,
    IN  GT_U32                                   messageType,
    IN  CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT  checkingMode
);

/*******************************************************************************
* cpssDxChPtpIngressPacketCheckingModeGet
*
* DESCRIPTION:
*       Get PTP packet Ingress Checking Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       checkingModePtr - (pointer to)PTP packet ingress checking mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressPacketCheckingModeGet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   domainIndex,
    IN  GT_U32                                   messageType,
    OUT CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT  *checkingModePtr
);

/*******************************************************************************
* cpssDxChPtpIngressExceptionCounterGet
*
* DESCRIPTION:
*       Get PTP Ingress Exception packet Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       counterPtr  - (pointer to) PTP Ingress Exception packet Counter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counter is Clear On Read.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressExceptionCounterGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *counterPtr
);

/*******************************************************************************
* cpssDxChPtpEgressExceptionCfgSet
*
* DESCRIPTION:
*       Set PTP packet Egress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       egrExceptionCfgPtr  - (pointer to) PTP packet Egress Exception Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressExceptionCfgSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC   *egrExceptionCfgPtr
);

/*******************************************************************************
* cpssDxChPtpEgressExceptionCfgGet
*
* DESCRIPTION:
*       Get PTP packet Egress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       egrExceptionCfgPtr  - (pointer to) PTP packet Egress Exception Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressExceptionCfgGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC   *egrExceptionCfgPtr
);

/*******************************************************************************
* cpssDxChPtpEgressExceptionCountersGet
*
* DESCRIPTION:
*       Get PTP packet Egress Exception Counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       egrExceptionCntPtr  - (pointer to) PTP packet Egress Exception Counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are Clear On Read.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressExceptionCountersGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC   *egrExceptionCntPtr
);

/*******************************************************************************
* cpssDxChPtpTsIngressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Ingress Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       tsQueueEntryPtr      - (pointer to) Ingress Timestamp Queue Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Queue read causes deleting the entry from the queue.
*       The next read will retrieve the next entry.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsIngressTimestampQueueEntryRead
(
    IN  GT_U8                                                devNum,
    IN  GT_U32                                               queueNum,
    OUT CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC   *tsQueueEntryPtr
);

/*******************************************************************************
* cpssDxChPtpTsEgressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Egress Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       tsQueueEntryPtr      - (pointer to) Egress Timestamp Queue Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Queue read causes deleting the entry from the queue.
*       The next read will retrieve the next entry.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsEgressTimestampQueueEntryRead
(
    IN  GT_U8                                                devNum,
    IN  GT_U32                                               queueNum,
    OUT CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC    *tsQueueEntryPtr
);

/*******************************************************************************
* cpssDxChPtpTsMessageTypeToQueueIdMapSet
*
* DESCRIPTION:
*       Set PTP Message Type To Queue Id Map.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       idMapBmp       - PTP Message Type To Queue Id Map.
*                        16 bits, bit per message type
*                        0 - queue0, 1 - queue1
*                        (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsMessageTypeToQueueIdMapSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       idMapBmp
);

/*******************************************************************************
* cpssDxChPtpTsMessageTypeToQueueIdMapGet
*
* DESCRIPTION:
*       Get PTP Message Type To Queue Id Map.
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
*       idMapBmpPtr    - (pointer to) PTP Message Type To Queue Id Map.
*                        16 bits, bit per message type
*                        0 - queue0, 1 - queue1
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsMessageTypeToQueueIdMapGet
(
    IN  GT_U8        devNum,
    OUT GT_U32       *idMapBmpPtr
);

/*******************************************************************************
* cpssDxChPtpTsQueuesEntryOverrideEnableSet
*
* DESCRIPTION:
*       Set Timestamp Queues Override Enable.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       enable          - when the TS Queue is full the new timestamp
*                         GT_TRUE  - overrides an oldest Queue entry.
*                         GT_FALSE - not queued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       cpssDxChPtpTsuControlSet should configure the same value per port
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsQueuesEntryOverrideEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChPtpTsQueuesEntryOverrideEnableGet
*
* DESCRIPTION:
*       Get Timestamp Queues Override Enable.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to)when the TS Queue is full the new timestamp
*                         GT_TRUE  - overrides an oldest Queue entry.
*                         GT_FALSE - not queued.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsQueuesEntryOverrideEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChPtpTsQueuesSizeSet
*
* DESCRIPTION:
*       Set Timestamp Queues Size.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       queueSize       - The size of each PTP Queue.
*                         (APPLICABLE RANGES: 0..256.)
*                         If Timestamp Queues Override is Enable,
*                         the size should be (maximal - 2) == 254.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsQueuesSizeSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      queueSize
);

/*******************************************************************************
* cpssDxChPtpTsQueuesSizeGet
*
* DESCRIPTION:
*       Get Timestamp Queues Size.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       queueSizePtr    - (pointer to)The size of each PTP Queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsQueuesSizeGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *queueSizePtr
);

/*******************************************************************************
* cpssDxChPtpTsDebugQueuesEntryIdsClear
*
* DESCRIPTION:
*       Set Timestamp Queues Current Entry Ids Clear.
*       For Debug only.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDebugQueuesEntryIdsClear
(
    IN  GT_U8       devNum
);

/*******************************************************************************
* cpssDxChPtpTsDebugQueuesEntryIdsGet
*
* DESCRIPTION:
*       Get Timestamp Queues Current Entry IDs.
*       For Debug only.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       queueEntryId0Ptr    - (pointer to)The PTP Queue0 current entry Id.
*       queueEntryId1Ptr    - (pointer to)The PTP Queue1 current entry Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDebugQueuesEntryIdsGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *queueEntryId0Ptr,
    OUT GT_U32      *queueEntryId1Ptr
);

/*******************************************************************************
* cpssDxChPtpTsNtpTimeOffsetSet
*
* DESCRIPTION:
*       Set TS NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       ntpTimeOffset   - NTP PTP Time Offset measured in seconds.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The same value should be configured per port.
*       See cpssDxChPtpTsuNtpTimeOffsetSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsNtpTimeOffsetSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ntpTimeOffset
);

/*******************************************************************************
* cpssDxChPtpTsNtpTimeOffsetGet
*
* DESCRIPTION:
*       Get TS NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       ntpTimeOffsetPtr - (pointer to) NTP PTP Time Offset measured in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsNtpTimeOffsetGet
(
    IN  GT_U8                             devNum,
    OUT GT_U32                            *ntpTimeOffsetPtr
);

/*******************************************************************************
* cpssDxChPtpTaiPtpPulseIterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP pulse interface.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       pulseInterfaceDirection - PTP pulse interface direction.
*       taiNumber               - TAI number selection.
*                                (relevant for PTP output pulse)
*                                For devices that have single TAI unit (Caelum) parameter ignored.
*                                (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP pulse interface used for generating/receiving discrete pulses
*       that trigger a time-related operation such as Update or Capture.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpPulseIterfaceSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      pulseInterfaceDirection,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT            taiNumber
);

/*******************************************************************************
* cpssDxChPtpTaiPtpPulseIterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP pulse interface.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       pulseInterfaceDirectionPtr  - (pointer to) PTP pulse interface direction.
*       taiNumberPtr                - (pointer to) TAI number selection.
*                                     (relevant for PTP output pulse)
*                                     For devices that have single TAI unit (Caelum) parameter ignored.
*                                     (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP pulse interface used for generating/receiving discrete pulses
*       that trigger a time-related operation such as Update or Capture.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpPulseIterfaceGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DIRECTION_ENT                   *pulseInterfaceDirectionPtr,
    OUT CPSS_DXCH_PTP_TAI_NUMBER_ENT         *taiNumberPtr
);

/*******************************************************************************
* cpssDxChPtpTaiPtpClockInterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP clock interface.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI Number.
*                               For devices that have single TAI unit (Caelum) parameter ignored.
*                               (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockInterfaceDirection - PTP clock interface direction.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP clock interface used for generating/receiving a periodic pulse
*       such as 1 PPS signal.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpClockInterfaceSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT         taiNumber,
    IN  CPSS_DIRECTION_ENT                   clockInterfaceDirection
);

/*******************************************************************************
* cpssDxChPtpTaiPtpClockInterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP clock interface.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI number.
*                               For devices that have single TAI unit (Caelum) parameter ignored.
*                               (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockInterfaceDirectionPtr   - (pointer to) PTP clock interface direction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP clock interface used for generating/receiving a periodic pulse
*       such as 1 PPS signal.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpClockInterfaceGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT         taiNumber,
    OUT CPSS_DIRECTION_ENT                   *clockInterfaceDirectionPtr
);

/*******************************************************************************
* cpssDxChPtpTaiPClkOutputInterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP PClk or Sync-E recovered clock output interface.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       pclkRcvrClkMode     - clock output interface mode.
*       taiNumber           - TAI number - used as clock’s source for PClk output mode.
*                             (relevant for "PTP" mode)
*                             For devices that have single TAI unit (Caelum) parameter ignored.
*                             (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPClkOutputInterfaceSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT    pclkRcvrClkMode,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT                taiNumber
);

/*******************************************************************************
* cpssDxChPtpTaiPClkOutputInterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP Pclock or Sync-E recovered clock output interface.
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
*       pclkRcvrClkModePtr  - (pointer to) clock output interface mode.
*       taiNumberPtr        - (pointer to) TAI number - used as clock’s source for PClk output mode.
*                             (relevant for "PTP" mode)
*                             For devices that have single TAI unit (Caelum) parameter ignored.
*                             (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPClkOutputInterfaceGet
(
    IN  GT_U8                                       devNum,
    OUT CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT    *pclkRcvrClkModePtr,
    OUT CPSS_DXCH_PTP_TAI_NUMBER_ENT                *taiNumberPtr
);

/*******************************************************************************
* cpssDxChPtpTaiInputClockSelectSet
*
* DESCRIPTION:
*       Configures input TAI clock's selection.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       taiNumber           - TAI number.
*                           For devices that have single TAI unit (Caelum) parameter ignored.
*                           (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockSelect         - input clock selection mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInputClockSelectSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber,
    IN  CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT  clockSelect
);

/*******************************************************************************
* cpssDxChPtpTaiInputClockSelectGet
*
* DESCRIPTION:
*       Get input TAI clock's selection.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI number.
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockSelectPtr          - (pointer to) input clock selection mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInputClockSelectGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber,
    OUT CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT  *clockSelectPtr
);

/*******************************************************************************
* cpssDxChPtpTaiTodCounterFunctionAllTriggerSet
*
* DESCRIPTION:
*       Triggeres TOD update operation for all TAI units.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodCounterFunctionAllTriggerSet
(
    IN GT_U8                    devNum
);

/*******************************************************************************
* cpssDxChPtpPortUnitResetSet
*
* DESCRIPTION:
*       Reset/unreset Port PTP unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       resetTxUnit - GT_TRUE - reset PTP trasmit unit,
*                     GT_FALSE - unreset PTP trasmit unit
*                     For Bobcat2 related to both transmit and receive units
*       resetRxUnit - GT_TRUE - reset PTP receive unit
*                     GT_FALSE - unreset PTP receive unit
*                     (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortUnitResetSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         resetTxUnit,
    IN  GT_BOOL                         resetRxUnit
);

/*******************************************************************************
* cpssDxChPtpPortUnitResetGet
*
* DESCRIPTION:
*       Get Reset/unreset Port PTP unit state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       resetTxUnitPtr - (pointer to)
*                        GT_TRUE - reset PTP trasmit unit,
*                        GT_FALSE - unreset PTP trasmit unit
*                        For Bobcat2 related to both transmit and receive units
*       resetRxUnitPtr - (pointer to)
*                        GT_TRUE - reset PTP receive unit
*                        GT_FALSE - unreset PTP receive unit
*                        (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortUnitResetGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_BOOL                         *resetTxUnitPtr,
    OUT GT_BOOL                         *resetRxUnitPtr
);

/*******************************************************************************
* cpssDxChPtpPortTxPipeStatusDelaySet
*
* DESCRIPTION:
*       Set PTP Tx Pipe Status Delay.
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       delay       - delay of empty egress pipe (in core clock cycles).
*                     (APPLICABLE RANGE: 0..0xFFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_OUT_OF_RANGE   - on out of range parameter value
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTxPipeStatusDelaySet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                          delay
);

/*******************************************************************************
* cpssDxChPtpPortTxPipeStatusDelayGet
*
* DESCRIPTION:
*       Get PTP Tx Pipe Status Delay.
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       delayPtr    - (pointer to)delay of empty egress pipe (in core clock cycles).
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTxPipeStatusDelayGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_U32                          *delayPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPtp */

