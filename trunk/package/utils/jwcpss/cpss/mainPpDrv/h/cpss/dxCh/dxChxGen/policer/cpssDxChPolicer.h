
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPolicer.h
*
* DESCRIPTION:
*       CPSS DxCh Ingress Policing Engine API
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#ifndef __cpssDxChPolicerh
#define __cpssDxChPolicerh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/policer/cpssGenPolicerTypes.h>


/*
 * typedef: struct CPSS_DXCH_POLICER_TB_PARAMS_STC
 *
 * Description: Structure for Policer Token Bucket Parameters
 *
 * Fields:
 *  cir - commited information rate in kilobits/sec (kilo = 1000)
 *  cbs - commited burst size in bytes
 *
 */
typedef struct{
    GT_U32   cir;
    GT_U32   cbs;
} CPSS_DXCH_POLICER_TB_PARAMS_STC;

/*
 * typedef: enum CPSS_DXCH_POLICER_CMD_ENT
 *
 * Description: Enumeration of Policer Commands is applied to packets that were
 *              classified as Red (out-of-profile) by the traffic meter.
 *
 * Enumerations:
 *      CPSS_DXCH_POLICER_CMD_NONE_E     - performs no action on the packet.
 *      CPSS_DXCH_POLICER_CMD_DROP_RED_E - the packet is SOFT or HARD dropped
 *                                        according to a global configuration
 *                                        set by cpssDxChPolicerDropRedModeSet.
 *      CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E  - preserve forwarding decision, but
 *                                        modify QoS setting according to QoS
 *                                        parameters in this entry
 *      CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E - preserve forwarding
 *                                        decision, but their QoS parameters
 *                                        setting is modified as follows:
 *                                        - QoSProfile is extracted from the
 *                                          Policers QoS Remarking and
 *                                          Initial DP Table Entry indexed by the
 *                                          QoSProfile assigned to the packet by
 *                                          the previous QoS assignment
 *                                          engines in the ingress pipe.
 *                                       - ModifyUP is modified by this entry.
 *                                       - ModifyDSCP is modified by this entry
 */
typedef enum {
    CPSS_DXCH_POLICER_CMD_NONE_E,
    CPSS_DXCH_POLICER_CMD_DROP_RED_E,
    CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E,
    CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E
} CPSS_DXCH_POLICER_CMD_ENT;

/*
 * typedef: struct CPSS_DXCH_POLICER_ENTRY_STC
 *
 * Description: Structure for Policer Entry.
 *
 * Fields:
 *  policerEnable    - Policer Enable
 *                     GT_FALSE - Policer Disabled. Preserve all QoS fields,
 *                                don't perform Token Bucket algorithm,
 *                                and don't update counters.
 *                     GT_TRUE - Policer enabled.
 *
 *  meterColorMode   - Policer's meter color mode
 *                     CPSS_POLICER_COLOR_BLIND_E - The packet's conformance
 *                        level (i.e., conforming or non-conforming)
 *                        is determined according to the Token Bucket
 *                        meter result only.
 *                     CPSS_POLICER_COLOR_AWARE_E - The packet's conformance
 *                        level (i.e., conforming or non-conforming)
 *                        is determined according to the Token Bucket meter
 *                        result and the incoming QosProfile Drop Precedence
 *                        parameter extracted from the Policers QoS Remarking
 *                        and Initial DP Table as follows:
 *                        - If the Token Bucket meter result is conforming, and
 *                          <IntialDP> = Green the packet is marked as conforming.
 *                        - If the Token Bucket meter result is non-conforming, or
 *                          <IntialDP> = Red the packet is marked as non-conforming.
 *
 *  tbParams         - token bucket parameters
 *
 *  counterEnable    - enables counting In Profile and Out of Profile packets,
 *                     using one of the 16 counters sets
 *                     GT_FALSE - Counting is disabled.
 *                     GT_TRUE  - Counting is enabled. The counters set used
 *                               is the counters set pointed to by counterSetIndex.
 *
 *  counterSetIndex  - policing counters set index (APPLICABLE RANGES: 0..15)
 *                     relevant when counterEnable == GT_TRUE
 *
 *  cmd              - policer command
 *                     Out-of-profile action is applied to packets that were
 *                     classified as Red (out-of-profile) by the traffic meter.
 *
 *  qosProfile       - The QoSProfile assigned to non-conforming packets
 *                     when cmd == CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E
 *
 *  modifyDscp       - modify Dscp mode
 *                      Relevant for non-conforming packets when
 *                     cmd == CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E or
 *                     cmd == CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E
 *                      Relevant for IPv4/IPv6 packets only.Enables modification
 *                     of the packet's DSCP field.
 *                      When this field is set to
 *                     CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E, the packet's DSCP
 *                     field is modified to the DSCP extracted from the
 *                     QoS Profile Table Entry when the packet is transmitted.
 *
 *  modifyUp         - modify Up mode
 *                      Relevant for non-conforming packets when
 *                     cmd == CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E or
 *                     cmd == CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E
 *                      Relevant for packets that are transmitted VLAN Tagged or
 *                     packets that are transmitted via cascading ports with a
 *                     DSA tag in a FORWARD format.
 *                     Enables the modification of the packet's IEEE 802.1p
 *                     User Priority field. When this field is set to
 *                     CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E, the packet's
 *                     802.1p User Priority field is set to the UP extracted
 *                     from the QoS Profile Table Entry when the packet is
 *                     transmitted, regardless of the incoming packet tag format
 *                     (Tagged or Untagged).
 *
 */
typedef struct{
      GT_BOOL                                   policerEnable;
      CPSS_POLICER_COLOR_MODE_ENT               meterColorMode;
      CPSS_DXCH_POLICER_TB_PARAMS_STC           tbParams;
      GT_BOOL                                   counterEnable;
      GT_U32                                    counterSetIndex;
      CPSS_DXCH_POLICER_CMD_ENT                 cmd;
      GT_U32                                    qosProfile;
      CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT     modifyDscp;
      CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT     modifyUp;
} CPSS_DXCH_POLICER_ENTRY_STC;

/*
 * typedef: struct CPSS_DXCH_POLICER_COUNTERS_STC
 *
 * Description: Structure for Policer Counters Entry.
 *
 * Fields:
 *  outOfProfileBytesCnt    -   Out-of-Profile bytes counter
 *  inProfileBytesCnt       -   In-Profile bytes counter
 *
 */
typedef struct{
      GT_U32    outOfProfileBytesCnt;
      GT_U32    inProfileBytesCnt;
} CPSS_DXCH_POLICER_COUNTERS_STC;


/*******************************************************************************
* cpssDxChPolicerInit
*
* DESCRIPTION:
*       Init Traffic Conditioner facility on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - on illegal devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerInit
(
    IN  GT_U8       devNum
);


/*******************************************************************************
* cpssDxChPolicerPacketSizeModeSet
*
* DESCRIPTION:
*      Configure Policing mode for non tunnel terminated packets
*      The Policy engine provides the following modes to define packet size:
*       - Layer 1 metering. Packet size includes the
*         entire packet + IPG + preamble.
*       - Layer 2 metering. Packet size includes the entire packet,
*         including Layer 2 header and CRC.
*       - Layer 3 metering. Packet size includes Layer 3 information only,
*         excluding Layer 2 header and CRC.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - Policing mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPacketSizeModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT        mode
);

/*******************************************************************************
* cpssDxChPolicerDropRedModeSet
*
* DESCRIPTION:
*      Set the type of the Policer drop action for red packets
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       dropRedMode - Red Drop mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or dropRedMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerDropRedModeSet
(
    IN  GT_U8                      devNum,
    IN  CPSS_DROP_MODE_TYPE_ENT    dropRedMode
);

/*******************************************************************************
* cpssDxChPolicerPacketSizeModeForTunnelTermSet
*
* DESCRIPTION:
*      Set the policing counting mode for tunnel terminated packets
*      The Policy engine provides the following modes to define packet size:
*       - Layer 1 metering. Packet size includes the
*         entire packet + IPG + preamble.
*       - Layer 2 metering. Packet size includes the entire packet,
*         including Layer 2 header and CRC.
*       - Passenger packet metering.
*         For IP-Over-x packets, the counting includes the passenger packet's
*         BC, excluding the tunnel header and the packets CRC
*         For, Ethernet-Over-x packets, the counting includes the passenger
*         packet's BC, excluding the tunnel header and if packet includes Two
*         CRC patterns (one for the inner packet and one for the outer packets)
*         also excluding the outer CRC
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - policing mode for tunnel terminated packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPacketSizeModeForTunnelTermSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_POLICER_PACKET_SIZE_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChPolicerTokenBucketModeSet
*
* DESCRIPTION:
*      Set the policer token bucket mode and policer MRU.
*      To implement the token bucket algorithm, each of the 256 policers
*      incorporates a Bucket Size Counter (BucketSizeCnt).
*      This counter is incremented with tokens, according to the configured
*      policer rate (CIR) up to a maximal value of the configured
*      Policer Burst Size (CBS)
*      The Byte Count of each conforming packet is decremented from the counter.
*      When a new packet arrives, according to this configuration, the packet
*      conformance is checked according to one of the following modes:
*      - Strict Rate Limiter - If BucketSizeCnt > Packet's Byte Count the packet
*          is conforming else, it is out of profile.
*      - Loose Rate Limiter - If BucketSizeCnt > MRU the packet is conforming
*          else, it is out of profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - Policer Token Bucket mode.
*       mru         - Policer MRU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerTokenBucketModeSet
(
    IN GT_U8                      devNum,
    IN CPSS_POLICER_TB_MODE_ENT   mode,
    IN CPSS_POLICER_MRU_ENT       mru
);

/*******************************************************************************
* cpssDxChPolicerEntrySet
*
* DESCRIPTION:
*      Set Policer Entry configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*       entryPtr            - pointer to policer entry
*
* OUTPUTS:
*       tbParamsPtr         - pointer to actual policer token bucket parameters.
*                            The token bucket parameters are returned as output
*                            values. This is due to the hardware rate resolution,
*                            the exact rate or burst size requested may not be
*                            honored. The returned value gives the user the
*                            actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex or entry parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEntrySet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            entryIndex,
    IN  CPSS_DXCH_POLICER_ENTRY_STC       *entryPtr,
    OUT CPSS_DXCH_POLICER_TB_PARAMS_STC   *tbParamsPtr
);


/*******************************************************************************
* cpssDxChPolicerEntryGet
*
* DESCRIPTION:
*      Get Policer Entry parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*
* OUTPUTS:
*       entryPtr             - pointer to policer entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          entryIndex,
    OUT CPSS_DXCH_POLICER_ENTRY_STC     *entryPtr
);

/*******************************************************************************
* cpssDxChPolicerEntryInvalidate
*
* DESCRIPTION:
*      Invalidate Policer Entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEntryInvalidate
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  entryIndex
);

/*******************************************************************************
* cpssDxChPolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*      Calculate Token Bucket parameters in the Application format without
*      HW update.
*      The token bucket parameters are returned as output values. This is due to
*      the hardware rate resolution, the exact rate or burst size requested may
*      not be honored. The returned value gives the user the actual parameters
*      configured in the hardware.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       tbInParamsPtr   - pointer to Token bucket input parameters
*
* OUTPUTS:
*       tbOutParamsPtr  - pointer to Token bucket output paramters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEntryMeterParamsCalculate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_POLICER_TB_PARAMS_STC        *tbInParamsPtr,
    OUT CPSS_DXCH_POLICER_TB_PARAMS_STC        *tbOutParamsPtr
);

/*******************************************************************************
* cpssDxChPolicerCountersGet
*
* DESCRIPTION:
*      Get Policer Counters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       countersPtr  - pointer to counters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or counterSetIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountersGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            counterSetIndex,
    OUT CPSS_DXCH_POLICER_COUNTERS_STC    *countersPtr

);

/*******************************************************************************
* cpssDxChPolicerCountersSet
*
* DESCRIPTION:
*      Set Policer Counters.
*      To reset counters use zero values.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index (APPLICABLE RANGES: 0..15)
*       countersPtr  - pointer to counters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or counterSetIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountersSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          counterSetIndex,
    IN  CPSS_DXCH_POLICER_COUNTERS_STC  *countersPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPolicerh */

