/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTmGlueAgingAndDelay.h
*
* DESCRIPTION:
*       The CPSS DXCH Aging and Delay Measurements definitions
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/
#ifndef __cpssDxChTmGlueAgingAndDelayh
#define __cpssDxChTmGlueAgingAndDelayh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*
 * Typedef: struct CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC
 *
 * Description: Aging Profile Thresholds
 *              The time that the packet was stored in TM Queue is compared to
 *              this thresholds. In case time is larger then threshold2  and
 *              "Drop Aged Packet" setting is enabled -->  the packet is dropped,
 *              in all other cases the packet's Queue TM Drop Profile is changed
 *              if a Threshold is crossed.
 *
 * Fields:
 *      threshold0  - first Threshold value (APPLICABLE RANGES: 0..0xFFFFF)
 *      threshold1  - second Threshold value (APPLICABLE RANGES: 0..0xFFFFF)
 *      threshold2  - third Threshold value (APPLICABLE RANGES: 0..0xFFFFF)
 */
typedef struct
{
    GT_U32 threshold0;
    GT_U32 threshold1;
    GT_U32 threshold2;
}CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC;

/*
 * Typedef: struct CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC
 *
 * Description: Read the counters representing packets that were queue to a
 *              specific TM Queue.
 *              depending on cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableSet
 *              aged packets can be taken into account of the statistics
 *
 * Fields:
 *      packetCounter         - number of packets that passed through a TM Queue
 *                              Counter is 24 bits
 *      octetCounter          - number of non Aged octets that passed through a
 *                              TM Queue
 *                              Octet counter is 40 bits
 *      latencyTimeCounter    - count the waiting time of all packets that passed
 *                              through a TM Queue
 *                              latency time counter is 44 bits
 *      agedPacketCounter     - number of aged packets that passed through a TM Queue
 *                              Counter is 24 bits
 *      maxLatencyTime        - maximal waiting time for packets that passed
 *                              through a TM Queue
 *                              Counter is 20 bits
 *      minLatencyTime        - minimal waiting time for packets that  passed
 *                              through a TM Queue
 *                              Counter is 20 bits
 *
 */
typedef struct
{
    GT_U32 packetCounter;
    GT_U64 octetCounter;
    GT_U64 latencyTimeCounter;
    GT_U32 agedPacketCounter;
    GT_U32 maxLatencyTime;
    GT_U32 minLatencyTime;
}CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC;


/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayDropAgedPacketEnableSet
*
* DESCRIPTION:
*       Enable dropping of aged packets.
*       Sets if packets that are stored longer than highest aging threhold should
*       be dropped or queue Drop Profile shuld be changed to highest threshold DP.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE:  Drop
*                      GT_FALSE: No Drop - change DP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayDropAgedPacketEnableSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayDropAgedPacketEnableGet
*
* DESCRIPTION:
*      Get dropping aged packets status
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to)
*                      GT_TRUE:  Drop
*                      GT_FALSE: No Drop - change DP
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayDropAgedPacketEnableGet
(
    IN  GT_U8                    devNum,
    OUT GT_BOOL                  *enablePtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableSet
*
* DESCRIPTION:
*       Enable statistics on packets dropped due to aging in
*       TM queue aging and delay counter set
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE:  Count
*                      GT_FALSE: Do not count
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableGet
*
* DESCRIPTION:
*       Get status of enable statistics on packets dropped due to aging in
*       TM queue aging and delay counter set
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to)
*                      GT_TRUE:  Count
*                      GT_FALSE: Do not count
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableGet
(
    IN  GT_U8                    devNum,
    OUT GT_BOOL                  *enablePtr
);

/*******************************************************************************
* cpssDxChTmGlueGeneralPerPortDropOutgoingPacketEnableSet
*
* DESCRIPTION:
*      Enable Drop of packets transmitted through TM-Port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                  - device number
*       tmPort                  - TM port
*                                (APPLICABLE RANGES: 0..191)
*       enable                  - if enabled all packet transmitted through
*                                 TM-Port are dropped.
*                                 GT_TRUE  = Dropped Packets
*                                 GT_FALSE = Regular functionality
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueGeneralPerPortDropOutgoingPacketEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_U32           tmPort,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChTmGlueGeneralPerPortDropOutgoingPacketEnableGet
*
* DESCRIPTION:
*      Return drop status of packets transmitted through TM-Port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                  - device number
*       tmPort                  - TM port
*                                (APPLICABLE RANGES: 0..191)
*
* OUTPUTS:
*       enablePtr               - (pointer to) drop status of all packet
*                                 transmitted through TM-Port
*                                 GT_TRUE  = Dropped Packets
*                                 GT_FALSE = Regular functionality
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueGeneralPerPortDropOutgoingPacketEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           tmPort,
    IN  GT_BOOL          *enablePtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayAgedPacketCounterConfigSet
*
* DESCRIPTION:
*      Set Configuration for Dropped Aged Packet Counter
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                        - device number
*       agedPacketCouterQueueMask     - This is the mask on queue number for
*                                       triggering the aging drop counter
*                                       (APPLICABLE RANGES: 0..16383)
*       agedPacketCouterQueue         - This is the queue number for triggering
*                                       the aging drop counter
*                                       (APPLICABLE RANGES: 0..16383)
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counter is increment if the following logic apply:
*       (agedPacketCouterQueueMask & PacketQueue) = agedPacketCouterQueue
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayAgedPacketCounterConfigSet
(
    IN  GT_U8            devNum,
    IN  GT_U32           agedPacketCouterQueueMask,
    IN  GT_U32           agedPacketCouterQueue
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayAgedPacketCounterConfigGet
*
* DESCRIPTION:
*       Get Configuration for Dropped Aged Packet Counter
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       agedPacketCouterQueueMaskPtr - (pointer to) the mask on queue number
*                                      for triggering the aging drop counter
*       agedPacketCouterQueuePtr     - (pointer to) the queue number for
*                                      triggering the aging drop counter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counter is increment if the following logic apply:
*       (agedPacketCouterQueueMask & PacketQueue) = agedPacketCouterQueue
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayAgedPacketCounterConfigGet
(
    IN  GT_U8            devNum,
    OUT GT_U32           *agedPacketCouterQueueMaskPtr,
    OUT GT_U32           *agedPacketCouterQueuePtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayAgedPacketCounterGet
*
* DESCRIPTION:
*      Read the counter that counts the number of aged packets that were dropped
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       agedPacketCounterPtr        - (pointer to) the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Clear On Read Counter
*       The counter is increment if the following logic apply:
*       (agedPacketCouterQueueMask & PacketQueue) = agedPacketCouterQueue
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayAgedPacketCounterGet
(
    IN  GT_U8            devNum,
    OUT GT_U32           *agedPacketCounterPtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayTimerResolutionSet
*
* DESCRIPTION:
*      Set aging's timer units resolution.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       timerResolution     - timer resolution in nanosec
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayTimerResolutionSet
(
    IN    GT_U8                    devNum,
    INOUT GT_U32                   *timerResolutionPtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayTimerResolutionGet
*
* DESCRIPTION:
*       Get aging's timer units resolution.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       timerResolutionPtr       - (pointer to) timer resolution in nanosec
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayTimerResolutionGet
(
    IN  GT_U8                    devNum,
    OUT GT_U32                   *timerResolutionPtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayTimerGet
*
* DESCRIPTION:
*       The aging and delay timer is the reference clock used for calculating
*       the time each packet was stored.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       timerPtr            - (pointer to) the aging and delay timer
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The timer is incremented by 1 each time the free running counter
*       reach "0".
*       see cpssDxChTmGlueAgingAndDelayTimerResolutionSet
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayTimerGet
(
    IN  GT_U8                    devNum,
    OUT GT_U32                   *timerPtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayAgingProfileThresholdsSet
*
* DESCRIPTION:
*      Configure the Aging Profile Thresholds Table
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       profileId           - profileId
*                             (APPLICABLE RANGES: 0..15)
*       thresholdsPtr       - (pointer to) thresholds values.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The thresholds units are set in cpssDxChTmGlueAgingAndDelayTimerResolutionSet
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayAgingProfileThresholdsSet
(
    IN GT_U8                                                devNum,
    IN GT_U32                                               profileId,
    IN CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC  *thresholdsPtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayAgingProfileThresholdsGet
*
* DESCRIPTION:
*        Get the Aging Profile Thresholds Table configuration
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       profileId           - profileId
*                             (APPLICABLE RANGES: 0..15)
* OUTPUTS:
*       thresholdsPtr       - (pointer to) thresholds values.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The thresholds units are set in cpssDxChTmGlueAgingAndDelayTimerResolutionSet
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayAgingProfileThresholdsGet
(
    IN  GT_U8                                               devNum,
    IN  GT_U32                                              profileId,
    OUT CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC *thresholdsPtr
);


/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayQueueAgingProfileIdSet
*
* DESCRIPTION:
*      Configure Queue Aging Profile Table that Maps a Traffic Manager Queue ID
*      to an aging profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       queueId             - Queue Id to configure
*                             (APPLICABLE RANGES: 0..16383)
*       profileId           - Profile Id value to set for the given queueId
*                             (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayQueueAgingProfileIdSet
(
    IN GT_U8        devNum,
    IN GT_U32       queueId,
    IN GT_U32       profileId
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayQueueAgingProfileIdGet
*
* DESCRIPTION:
*       Get Queue Aging Profile Table configuration that Maps a Traffic Manager
*       Queue ID to an aging profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       queueId             - Queue Id to configure
*                             (APPLICABLE RANGES: 0..16383)
* OUTPUTS:
*       profileIdPtr        - (pointer to)
*                             Profile Id value to set for the given queueId
*                             (APPLICABLE RANGES: 0..15)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayQueueAgingProfileIdGet
(
    IN  GT_U8        devNum,
    IN  GT_U32       queueId,
    OUT GT_U32       *profileIdPtr
);


/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayCounterQueueIdSet
*
* DESCRIPTION:
*       Map one of aging and delay counter sets to a queuedId number.
*       The counters in the set perform delay and aging statistics on the packets
*       queued to the configured Queue-ID.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                      - device number
*       counterSetIndex             - Aging and delay counter set index
*                                     (APPLICABLE RANGES: 0..99)
*       queueId                     - queue id
*                                     (APPLICABLE RANGES: 0..16383)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayCounterQueueIdSet
(
    IN GT_U8                    devNum,
    IN GT_U32                   counterSetIndex,
    IN GT_U32                   queueId
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayCounterQueueIdGet
*
* DESCRIPTION:
*       Get Mapping of aging and delay counter set to a queuedId number.
*       The counters in the set perform delay and aging statistics on the packets
*       queued to the configured Queue-ID.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                      - device number
*       counterSetIndex             - Aging and delay counter set index
*                                     (APPLICABLE RANGES: 0..99)
* OUTPUTS:
*       queueIdPtr                  - (pointer to) queue id
*                                     (APPLICABLE RANGES: 0..16383)
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayCounterQueueIdGet
(
    IN  GT_U8                    devNum,
    IN  GT_U32                   counterSetIndex,
    OUT GT_U32                   *queueIdPtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayCountersGet
*
* DESCRIPTION:
*      Read all the counters of aging and delay counter set
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       counterSetIndex     - aging and delay counter set
*                                (APPLICABLE RANGES: 0..99)
*
* OUTPUTS:
*       agingCountersPtr    - (pointer to) aging and delay counters
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Clear On Read Counter
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayCountersGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      counterSetIndex,
    OUT CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC *agingCountersPtr
);

/*******************************************************************************
* cpssDxChTmGlueAgingAndDelayPerPortAgedPacketCounterGet
*
* DESCRIPTION:
*      Read the counter that counts the number of aged out packets
*      that should have been transmiited from tm-port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                  - device number
*       tmPort                  - TM port
*                                (APPLICABLE RANGES: 0..191)
*
* OUTPUTS:
*       agedOutPacketCounterPtr   - (pointer to) Aged Out Packet Counter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Clear On Read Counter
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueAgingAndDelayPerPortAgedPacketCounterGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           tmPort,
    OUT GT_U32           *agedOutPacketCounterPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChAgingAndDMh */

