/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPfc.h
*
* DESCRIPTION:
*       CPSS implementation for Priority Flow Control functionality.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*******************************************************************************/

#ifndef __cpssDxChPortPfch
#define __cpssDxChPortPfch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>


/*
 * typedef: enum CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT
 *
 * Description: Enumeration of PFC Counting modes.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E  -
 *    Counting buffers. PFC thresholds are set in buffers.
 *  CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E -
 *    Counting packets. PFC thresholds are set in packets.
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E,
    CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E
}CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT
 *
 * Description: Enumeration of PFC enable options.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E - PFC triggering only enabled.
 *  CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E -
 *    Both PFC triggering and response are enabled.
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E,
    CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E
}CPSS_DXCH_PORT_PFC_ENABLE_ENT;

/*
 * typedef: struct CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC
 *
 * Description: PFC Profile configurations.
 *
 * Fields:
 *      xonThreshold    - Xon threshold.
 *      xoffThreshold   - Xoff threshold.
 *
 * Comments:
 *      None.
 */
typedef struct {
    GT_U32 xonThreshold;
    GT_U32 xoffThreshold;
} CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC;


/*
 * typedef: struct CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC
 *
 * Description: Lossy drop configurations.
 *
 * Fields:
 *      toCpuLossyDropEnable - GT_TRUE: Enable drop of TO_CPU packets according to the
 *                              lossy drop feature.
 *                             GT_FALSE: Do not drop TO_CPU packets due to to lossy drop
 *                              feature (even if its TC marked as drop).
 *      toTargetSnifferLossyDropEnable - 
 *                             GT_TRUE: Enable drop of TO_TARGET_SNIFFER packets according to the
 *                              lossy drop feature.
 *                             GT_FALSE: Do not drop TO_TARGET_SNIFFER packets due to to lossy drop
 *                              feature (even if its TC marked as drop).
 *                             lossy drop feature. 
 *      fromCpuLossyDropEnable - GT_TRUE: Enable drop of FROM_CPU packets according to the
 *                              lossy drop feature.
 *                             GT_FALSE: Do not drop FROM_CPU packets due to to lossy drop
 *                              feature (even if its TC marked as drop).
 *
 * Comments:
 *      None.
 */
typedef struct {
    GT_BOOL toCpuLossyDropEnable;
    GT_BOOL toTargetSnifferLossyDropEnable;
    GT_BOOL fromCpuLossyDropEnable;
} CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC;

/*******************************************************************************
* cpssDxChPortPfcEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC (Priority Flow Control) response functionality.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       pfcEnable  - PFC enable option.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC enable option
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        If PFC response is enabled, the shaper’s baseline must be
*        at least 0x3FFFC0, see:
*        cpssDxChPortTxShaperBaselineSet.
*        Note: Triggering cannot be disabled by this API.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcEnableSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_ENABLE_ENT pfcEnable
);

/*******************************************************************************
* cpssDxChPortPfcEnableGet
*
* DESCRIPTION:
*       Get the status of PFC response functionality.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       pfcEnablePtr  - (pointer to) PFC enable option.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Triggering is enabled by default.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcEnableGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_ENABLE_ENT *pfcEnablePtr
);

/*******************************************************************************
* cpssDxChPortPfcProfileIndexSet
*
* DESCRIPTION:
*       Binds a source port to a PFC profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileIndexSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   profileIndex
);

/*******************************************************************************
* cpssDxChPortPfcProfileIndexGet
*
* DESCRIPTION:
*       Gets the port's PFC profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *profileIndexPtr
);

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       profileIndex - profile index (APPLICABLE RANGES: 0..7)
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*       pfcProfileCfgPtr - pointer to PFC Profile configurations
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
);

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       profileIndex - profile index (APPLICABLE RANGES: 0..7)
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
);


/*******************************************************************************
* cpssDxChPortPfcCountingModeSet
*
* DESCRIPTION:
*       Sets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcCountMode - PFC counting mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCountingModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountMode
);

/*******************************************************************************
* cpssDxChPortPfcCountingModeGet
*
* DESCRIPTION:
*       Gets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       pfcCountModePtr - (pointer to) PFC counting mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCountingModeGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  *pfcCountModePtr
);


/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable PFC global drop.
*                    GT_FALSE: Disable PFC global drop.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To configure drop threshold use cpssDxChPortPfcGlobalQueueConfigSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalDropEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableGet
*
* DESCRIPTION:
*       Gets the current status of PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of PFC functionality
*                     GT_TRUE:  PFC global drop enabled.
*                     GT_FALSE: PFC global drop disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalDropEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum        - device number
*       tcQueue       - traffic class queue (APPLICABLE RANGES: 0..7)
*       xoffThreshold - XOFF threshold
*                       (APPLICABLE RANGES: Lion, Lion2 0..0x7FF)
*                       (APPLICABLE RANGES: Bobcat2, Caelum, Bobcat3 0..0x1FFFFFFF)
*       dropThreshold - Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped
*                       (APPLICABLE RANGES: Lion, Lion2 0..0x7FF)
*                       (APPLICABLE RANGES: Bobcat2, Caelum, Bobcat3 0..0x1FFFFFFF)
*       xonThreshold  - XON threshold
*                       (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                       (APPLICABLE RANGES: 0..0x1FFFFFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. dropThreshold is used when PFC global drop is enabled.
*       See cpssDxChPortPfcGlobalDropEnableSet.
*       2. All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_U32   xoffThreshold,
    IN GT_U32   dropThreshold,
    IN GT_U32   xonThreshold
);

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       xoffThresholdPtr - (pointer to) Xoff threshold.
*       dropThresholdPtr - (pointer to) Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped.
*       xonThresholdPtr  - (pointer to) Xon threshold.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *xoffThresholdPtr,
    OUT GT_U32   *dropThresholdPtr,
    OUT GT_U32   *xonThresholdPtr
);

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableSet
*
* DESCRIPTION:
*       Enables mapping of PFC timer to priority queue for given scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile.
*       enable     - Determines whether PFC timer to Priority Queue map
*                    is used.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerMapEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_BOOL                                 enable

);

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableGet
*
* DESCRIPTION:
*       Get the status of PFS timer to priority queue mapping for given
*       scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile Set
*
* OUTPUTS:
*       enablePtr   - (pointer to) status of PFC timer to Priority Queue
*                      mapping.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerMapEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT GT_BOOL                                *enablePtr

);

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapSet
*
* DESCRIPTION:
*       Sets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcTimer     - PFC timer (0..7)
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_OUT_OF_RANGE          - on out of traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerToQueueMapSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    IN  GT_U32    tcQueue

);

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapGet
*
* DESCRIPTION:
*       Gets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       pfcTimer        - PFC timer (0..7).
*
* OUTPUTS:
*       tcQueuePtr      - (pointer to) traffic class queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerToQueueMapGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    OUT GT_U32    *tcQueuePtr

);

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioSet
*
* DESCRIPTION:
*       Sets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*       shaperToPortRateRatio - shaper rate to port speed ratio
*                               in percentage (0..100).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range shaper rate to port speed ratio
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    IN  GT_U32                                  shaperToPortRateRatio

);

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioGet
*
* DESCRIPTION:
*       Gets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*
* OUTPUTS:
*       shaperToPortRateRatioPtr - (pointer to)shaper rate to port speed ratio
*                               in percentage.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    OUT GT_U32                                 *shaperToPortRateRatioPtr

);

/*******************************************************************************
* cpssDxChPortPfcForwardEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of PFC frames to the ingress
*       pipeline of a specified port.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number.
*       portNum - physical port number (CPU port not supported)
*       enable  - GT_TRUE:  forward PFC frames to the ingress pipe,
*                 GT_FALSE: do not forward PFC frames to the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcForwardEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_BOOL enable
);


/*******************************************************************************
* cpssDxChPortPfcForwardEnableGet
*
* DESCRIPTION:
*       Get status of PFC frames forwarding
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr   - current forward PFC frames status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcForwardEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPortPfcLossyDropQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable lossy drop for packets with given traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*       enable     - GT_TRUE: Lossy - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are dropped.
*                  - GT_FALSE: Lossless - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are not dropped.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in 
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcLossyDropQueueEnableSet
(
    IN GT_U8     devNum,
    IN GT_U8     tcQueue,
    IN GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortPfcLossyDropQueueEnableGet
*
* DESCRIPTION:
*       Get the status of lossy drop on given traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*
* OUTPUTS:
*       enablePtr  (pointer to)
*                  - GT_TRUE: Lossy - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are dropped.
*                  - GT_FALSE: Lossless - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are not dropped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcLossyDropQueueEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortPfcLossyDropConfigSet
*
* DESCRIPTION:
*       Set lossy drop configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       lossyDropConfigPtr - (pointer to) lossy drop configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in 
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcLossyDropConfigSet
(
    IN GT_U8     devNum,
    IN CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC *lossyDropConfigPtr
);

/*******************************************************************************
* cpssDxChPortPfcLossyDropConfigGet
*
* DESCRIPTION:
*       Get lossy drop configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       lossyDropConfigPtr - (pointer to) lossy drop configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in 
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcLossyDropConfigGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC *lossyDropConfigPtr
);

/*******************************************************************************
* cpssDxChPortPfcTerminateFramesEnableSet
*
* DESCRIPTION:
*       Enable/Disable the ingress control pipe to parse and terminate received
*       flow control frames (PRC/802.3x)
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*       enable      - enable termination of flow control frames
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       If the configuration is disabled, flow control frames are processed by
*       the control pipe as regular data frames (that is they are subject to
*       learning, forwarding, filtering and mirroring) but they are not
*       terminated and the PFC timers are not extracted.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTerminateFramesEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortPfcTerminateFramesEnableGet
*
* DESCRIPTION:
*       Get whether the ingress control pipe to parse and terminate received
*       flow control frames (PRC/802.3x) is enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) termination of flow control frames enabling
*                     status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       If the configuration is disabled, flow control frames are processed by
*       the control pipe as regular data frames (that is they are subject to
*       learning, forwarding, filtering and mirroring) but they are not
*       terminated and the PFC timers are not extracted.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTerminateFramesEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortPfcQueueCounterGet
*
* DESCRIPTION:
*       Get the number buffers/packets per proirity queue.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum     - device number
*       tcQueue    - traffic class queue on the port
*
* OUTPUTS:
*       cntPtr     - (pointer to) number buffers/packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, tcQueue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       Buffer or packet counting mode is set by cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcQueueCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   tcQueue,
    OUT GT_U32  *cntPtr
);


/*******************************************************************************
* cpssDxChPortPfcSourcePortToPfcCounterSet
*
* DESCRIPTION:
*       Set PFC mapping of a physical port to a PFC counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       pfcCounterNum   - PFC counter (APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_OUT_OF_RANGE          - on wrong pfcCounterNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcSourcePortToPfcCounterSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32  pfcCounterNum
);


/*******************************************************************************
* cpssDxChPortPfcSourcePortToPfcCounterGet
*
* DESCRIPTION:
*       Get PFC mapping of a physical port to a PFC counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       pfcCounterNumPtr- (pointer to) PFC counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcSourcePortToPfcCounterGet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *pfcCounterNumPtr
);


/*******************************************************************************
* cpssDxChPortPfcCounterGet
*
* DESCRIPTION:
*       Get PFC counter value per PFC counter and traffic class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       tcQueue         - traffic class queue (APPLICABLE RANGES: 0..7)
*       pfcCounterNum   - PFC counter (APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       pfcCounterValuePtr - (pointer to) PFC counter value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_OUT_OF_RANGE          - on wrong pfcCounterNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   tcQueue,
    IN  GT_U32  pfcCounterNum,
    OUT GT_U32  *pfcCounterValuePtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortPfch */

