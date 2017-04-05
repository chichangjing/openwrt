/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCn.h
*
* DESCRIPTION:
*       CPSS DXCH Port Congestion Notification API.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*******************************************************************************/

#ifndef __cpssDxChPortCnh
#define __cpssDxChPortCnh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/generic/port/cpssPortTx.h>

/*
* typedef: enum CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT
*
* Description: Enumeration of packet length used by the CN frames triggering logic.
*
* Enumerations:
*  CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E - Original packet length in bytes
*  CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E  - 1.5KB
*  CPSS_DXCH_PORT_CN_LENGTH_2_KB_E - 2KB
*  CPSS_DXCH_PORT_CN_LENGTH_10_KB_E - 10KB
*
* Comments:
*         None.
*/
typedef enum
{
    CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E,
    CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E,
    CPSS_DXCH_PORT_CN_LENGTH_2_KB_E,
    CPSS_DXCH_PORT_CN_LENGTH_10_KB_E
}CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT;

/*
* typedef: enum CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT
*
* Description: Enumeration that indicates the locations of the priority/speed
*       bits in the CNM header. (Defines the index in table ....)
*
* Enumerations:
*  CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E -
*      priority/speed bits are in CPID[2:0].
*  CPSS_DXCH_PORT_CN_SDU_UP_E - priority/speed bits are in
*     CN-header<Encapsulated UP> field.
*  CPSS_DXCH_PORT_CN_SPEED_E - priority/speed bits are in the location of
*     the speed bits in xCAT CCFC
*
* Comments:
*         None.
*/
typedef enum
{
    CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E,
    CPSS_DXCH_PORT_CN_SDU_UP_E,
    CPSS_DXCH_PORT_CN_SPEED_E
}CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT;


/*
* typedef: enum CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT
*
* Description: Enumeration describes congestion notification message type
*
* Enumerations:
*
*   CPSS_DXCH_PORT_CN_MESSAGE_TYPE_QCN_E -
*       Quantized Congestion Notification protocol message
*
*   CPSS_DXCH_PORT_CN_MESSAGE_TYPE_CCFC_E -
 *      Cross Chip Flow Control message
*
* Comments:
*         None.
*/
typedef enum
{
    CPSS_DXCH_PORT_CN_MESSAGE_TYPE_QCN_E,
    CPSS_DXCH_PORT_CN_MESSAGE_TYPE_CCFC_E
}CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT;

/*
 * typedef: struct CPSS_PORT_CN_PROFILE_CONFIG_STC
 *
 * Description: CN Profile configurations.
 *
 * Fields:
 *      cnAware              - GT_FALSE - CN Blind: Congestion in this queue
 *                                        does not generate CN messages.
 *                             GT_TRUE  - CN Aware: This queue is enabled
 *                                        for triggering CN messages.
 *      threshold            - buffer limit threshold for triggering CN frame.
 *
 * Comments:
 *      None.
 */
typedef struct {
    GT_BOOL cnAware;
    GT_U32  threshold;
} CPSS_PORT_CN_PROFILE_CONFIG_STC;


/*
 * typedef: struct CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC
 *
 * Description: CN Sample Interval Table entry.
 *
 * Fields:
 *      interval    - CN sampling interval in resolution of 16 bytes.
 *                    (0..0xFFFF)
 *      randBitmap  - bitmap that indicates which sampling interval bits
 *                    are randomize. 0 - don't randomize; 1 - randomize.
 *                    (0..0xFFFF)
 * Comments:
 *      None.
 */
typedef struct {
    GT_U32 interval;
    GT_U32 randBitmap;
} CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC;

/*
 * typedef: struct CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC
 *
 * Description: Feedback calculation configurations.
 *
 * Fields:
 *      wExp   - Exponent weight for Fb calculation.(-8..7)
 *      fbLsb  - bits that are used for qFb calculation.
 *               qFb = Fb[fbLsb+5:fbLsb] (0..0x1F)
 *      deltaEnable - GT_TRUE: enables Delta (Qlen-QlenOld)
 *                    inclusion in Fb calculation
 *                    GT_FALSE: Delta (Qlen-QlenOld) is not included
 *                    in Fb calculation.
 *      fbMin       - Minimum value of Fb. (0..0xFFFFF)
 *                    If |Fb| < FbMin then |Fb| is set to 0.
 *      fbMax       - Maximum value of |Fb|. (0..0xFFFFF)
 *                    If |Fb| > FbMax then |Fb| is set to FbMax.
 * Comments:
 *      None.
 */
typedef struct {
    GT_32   wExp;
    GT_U32  fbLsb;
    GT_BOOL deltaEnable;
    GT_U32  fbMin;
    GT_U32  fbMax;
} CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC;

/*
 * typedef: struct CPSS_PORT_CNM_GENERATION_CONFIG_STC
 *
 * Description: CNM generation configurations
 *
 * Fields:
 *      qosProfileId     - The QoS Profile associated with CNM. (0..0x7F)
 *                         (APPLICABLE DEVICES: Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
 *      isRouted         - Value of DSA<routed> bit associated with CNM.
 *                         Setting this flag to '1' allows the egress device
 *                         in a cascaded system to modify the CNM MAC-SA (0,1)
 *                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3)
 *      overrideUp       - Allow to override UP assignment.
 *                         GT_FALSE: UP is copied from the sampled frame
 *                         GT_TRUE: UP is cnmUp configuration.
 *                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3)
 *      cnmUp            - The UP assigned to CNM. Used when overrideUp==GT_TRUE
 *                         (APPLICABLE DEVICES: Lion; Lion2)
 *      defaultVlanId    - The VID associated with CNM if the sampled frame is
 *                         untagged.(0..0xFFF)
 *                         (APPLICABLE DEVICES: Lion; xCat2; Lion2)
 *      scaleFactor      - Scale factor of Qoffset, Qdelta fields in CNM.
 *                         The device calculates these fields in buffer resolution.
 *                         This configuration provides the conversion between
 *                         the buffer size and 64 Bytes. Valid values: 256,512.
 *                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3)
 *      version          - CNM-header<Version> field for CNMs generated by
 *                         the device. (0..0xF)
 *                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3)
 *      cpidMsb          - CNM-header<CPID[63:10]> field for CNMs generated
 *                         by the device (network order).
 *                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3)
 *      cnUntaggedEnable - Enables generation of CNM if the sample frame is not CN-tagged.
 *                         GT_FALSE: Disable; CNM is generated only if the
 *                         sampled frame is CN-tagged
 *                         GT_TRUE: Enable; CNM is generated regardless of the
 *                         CN tag state of the sampled frame.
 *                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3)
 *      forceCnTag       - GT_FALSE: QCN is CN-tagged only if the sampled frame is CN-tagged.
 *                         GT_TRUE: QCN is always CN-tagged.A CN-tag is added if the sampled
 *                         frame is not CN-tagged.
 *                         Relevant only in QCN mode.
 *                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3)
 *      flowIdTag        - CN-tag FlowID used when a CN-tag is added to QCN frames generated
 *                         by the device.Relevant only in QCN mode. (0..0xFFFF)
 *                         (APPLICABLE DEVICES: Lion; Lion2)
 *      appendPacket     - GT_FALSE: The CNM is 0-padded up to total length of 64B.
 *                         GT_TRUE: Append sampled packet. The CNM header is followed by the
 *                         SDU starting from the SDU-DA.The entire CNM is truncated to 128B.
 *                         Relevant only in QCN mode.
 *                         (APPLICABLE DEVICES: Lion; xCat2; Lion2)
 *      sourceId         - source ID number
 *                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 * Comments:
 *      None.
 */
typedef struct {

    GT_U32   qosProfileId;
    GT_U8    isRouted;
    GT_BOOL  overrideUp;
    GT_U8    cnmUp;
    GT_U16   defaultVlanId;
    GT_U32   scaleFactor;
    GT_U32   version;
    GT_U8    cpidMsb[7];
    GT_BOOL  cnUntaggedEnable;
    GT_BOOL  forceCnTag;
    GT_U32   flowIdTag;
    GT_BOOL  appendPacket;
    GT_U32   sourceId;

} CPSS_PORT_CNM_GENERATION_CONFIG_STC;


/*******************************************************************************
* cpssDxChPortCnModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification (CN) mechanism.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable congestion notification mechanism.
*                    GT_FALSE: Disable congestion notification mechanism.
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
*       Do not use the global enable/disable CCFC under traffic - instead use
*       cpssDxChPortCnQueueAwareEnableSet to enable/disable queue for BCN.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortCnModeEnableGet
*
* DESCRIPTION:
*       Gets the current status of Congestion Notification (CN) mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification mode
*                     GT_TRUE:  Congestion Notification mode enabled.
*                     GT_FALSE: Congestion Notification mode disabled.
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
GT_STATUS cpssDxChPortCnModeEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortCnProfileSet
*
* DESCRIPTION:
*       Binds a port to a Congestion Notification profile.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (0..7).
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
*       1. To disable port for triggering CN frames bind port to profile with
*          all thresholds set to maximum.
*       2. On Lion device CN Profile is part of the Tail-Drop Profile.
*          To bind port to Tail-Drop Profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   profileIndex
);

/*******************************************************************************
* cpssDxChPortCnProfileGet
*
* DESCRIPTION:
*       Gets the port's Congestion Notification profile.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
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
GT_STATUS cpssDxChPortCnProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *profileIndexPtr
);

/*******************************************************************************
* cpssDxChPortCnProfileQueueThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*       threshold    - buffer limit threshold for triggering CN frame on a given
*                      tc queue (0..0x1FFF).
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
*       To bind port to CN profile in Lion use
*          cpssDxChPortCnProfileQueueConfigSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN GT_U32   threshold
);

/*******************************************************************************
* cpssDxChPortCnProfileQueueThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                      triggering CN frame on a given tc queue.
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueThresholdGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *thresholdPtr
);


/*******************************************************************************
* cpssDxChPortCnProfileThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       threshold    - buffer limit threshold
*                      for triggering CN frame (0..0x1FFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U32   threshold
);

/*******************************************************************************
* cpssDxChPortCnProfileThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                     triggering CN frame.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileThresholdGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  profileIndex,
    OUT GT_U32  *thresholdPtr
);

/*******************************************************************************
* cpssDxChPortCnQueueAwareEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*       enable     - GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueAwareEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortCnQueueAwareEnableGet
*
* DESCRIPTION:
*       Gets the status of Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification awareness
*                    GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueAwareEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortCnFrameQueueSet
*
* DESCRIPTION:
*       Set traffic class queue associated with CN frames generated by device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue associated with CN frames (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN frames must be enqueued to priority queues disabled to trigger CN.
*       Bobcat2; Caelum; Bobcat3 - The device use 'traffic class' from qosProfileId set by
*       cpssDxChPortCnMessageGenerationConfigSet()
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFrameQueueSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue
);

/*******************************************************************************
* cpssDxChPortCnFrameQueueGet
*
* DESCRIPTION:
*       Get traffic class queue associated with CN frames generated by device.
*
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       tcQueuePtr - (pointer to) traffic class queue associated with CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFrameQueueGet
(
    IN  GT_U8    devNum,
    OUT GT_U8    *tcQueuePtr
);

/*******************************************************************************
* cpssDxChPortCnEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CN frames.
*       This EtherType also inserted in the CN header of generated CN frames.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number.
*       etherType   - EtherType for CN frames (0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       The CN EtherType must be unique in the system.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
);

/*******************************************************************************
* cpssDxChPortCnEtherTypeGet
*
* DESCRIPTION:
*       Gets the EtherType for CN frames. The EtherType inserted in the CN
*       header.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       etherTypePtr - (pointer to) EtherType for CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnEtherTypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherTypePtr
);

/*******************************************************************************
* cpssDxChPortCnSpeedIndexSet
*
* DESCRIPTION:
*       Sets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       portSpeedIndex - port speed index (0..7).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range port speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Congested ports should use Port Speed Index 1 to 6 if
*          Panic Pause is used.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSpeedIndexSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   portSpeedIndex
);

/*******************************************************************************
* cpssDxChPortCnSpeedIndexGet
*
* DESCRIPTION:
*       Gets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       portNum    -      - port number.
*
* OUTPUTS:
*       portSpeedIndexPtr - (pointer to) port speed index.
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
GT_STATUS cpssDxChPortCnSpeedIndexGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32   *portSpeedIndexPtr
);


/*******************************************************************************
* cpssDxChPortCnFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable generation of flow control packets as result of CN
*       frame termination or Panic Pause.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable. Flow control packets can be issued.
*                    GT_FALSE: disable. Flow control packets can't be issued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortCnFcEnableGet
*
* DESCRIPTION:
*       Gets the status of generation of flow control packets as result of CN
*       frame termination or Panic Pause on a given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable. Flow control packets is issued.
*                    GT_FALSE: disable. Flow control packets is not issued.
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
GT_STATUS cpssDxChPortCnFcEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortCnPauseTriggerEnableSet
*
* DESCRIPTION:
*       Enable the CN triggering engine to trigger a pause frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable.
*                    GT_FALSE: disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        To enable proper operation of CCFC, this configuration must be
*        enabled for all network ports and disabled for all cascade ports.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPauseTriggerEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortCnPauseTriggerEnableGet
*
* DESCRIPTION:
*       Gets the status triggering engine a pause frame on a given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable.
*                    GT_FALSE: disable.
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
GT_STATUS cpssDxChPortCnPauseTriggerEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortCnFcTimerSet
*
* DESCRIPTION:
*       Sets timer value for the IEEE 802.3x/PFC frame issued as result
*       of CN frame termination.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       index          - index (0..7)
*       timer          - 802.3x/PFC pause time (0..0xFFFF).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_OUT_OF_RANGE          - on out of range pause time value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. xCat,xCat3: Panic Pause uses Port Speed Index 0 and 7 to indicate
*          XON and XOFF. Port timer configurations 0 and 7 must be configured
*          to 0x0 and 0xFFFF if Panic Pause is used.
*       2. xCat,xCat3: index is always congested port speed index (CCFC mode).
*       3. Lion, xCat2: index determined by
*          cpssDxChPortCnPrioritySpeedLocationSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcTimerSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   index,
    IN GT_U32   timer
);

/*******************************************************************************
* cpssDxChPortCnFcTimerGet
*
* DESCRIPTION:
*       Gets timer value for the IEEE 802.3x/PFC frame issued as result
*       of CN frame termination.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       index          - index (0..7).
*
* OUTPUTS:
*       timerPtr       - (pointer to) 802.3x/PFC Pause time
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. xCat,xCat3: index is always congested port speed index (CCFC mode).
*       2. Lion, xCat2: index determined by
*          cpssDxChPortCnPrioritySpeedLocationSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcTimerGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32   index,
    OUT GT_U32   *timerPtr
);


/*******************************************************************************
* cpssDxChPortCnPanicPauseThresholdsSet
*
* DESCRIPTION:
*       Sets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       enable      - GT_TRUE: Enable Panic Pause.
*                     GT_FALSE: Disable Panic Pause.
*       xoffLimit   - X-OFF limit (0..1FFF).Relevant only if enable==GT_TRUE.
*       xonLimit    - X-ON limit (0..1FFF).Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range xoffLimit or
*                                  xonLimit value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Port timer configurations 0 and 7 must be configured
*           to 0x0 and 0xFFFF, see cpssDxChPortCnFcTimerSet.
*          Congested ports should use Port Speed Index 1 to 6,
*          see cpssDxChPortCnSpeedIndexSet.
*       2. Panic Pause cannot operate together with the IEEE 802.3x triggers.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPanicPauseThresholdsSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U32   xoffLimit,
    IN GT_U32   xonLimit
);

/*******************************************************************************
* cpssDxChPortCnPanicPauseThresholdsGet
*
* DESCRIPTION:
*       Gets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       enablePtr    - (pointer to):
*                      GT_TRUE: Enable Panic Pause.
*                      GT_FALSE: Disable Panic Pause.
*       xoffLimitPtr - (pointer to) the X-OFF limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*       xonLimitPtr  - (pointer to) the X-ON limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPanicPauseThresholdsGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U32   *xoffLimitPtr,
    OUT GT_U32   *xonLimitPtr
);

/*******************************************************************************
* cpssDxChPortCnTerminationEnableSet
*
* DESCRIPTION:
*       Enable/Disable termination of CNM (Congestion Notification Message) on given port.
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
*       enable     - GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnTerminationEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortCnTerminationEnableGet
*
* DESCRIPTION:
*       Gets the status of CNM termination on given port.
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
*       enablePtr  - (pointer to)
*                    GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
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
GT_STATUS cpssDxChPortCnTerminationEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortCnProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       profileSet      - profile set.
*       tcQueue         - traffic class queue (0..7).
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN Profile is part of the Tail-Drop Profile.
*       To bind port to Tail-Drop Profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueConfigSet
(
    IN GT_U8                               devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN GT_U8                               tcQueue,
    IN CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
);

/*******************************************************************************
* cpssDxChPortCnProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       profileSet   - profile set.
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueConfigGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_U8                               tcQueue,
    OUT CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
);

/*******************************************************************************
* cpssDxChPortCnQueueStatusModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable queue status mode. When enabled, CNM is sent to a
*       configurable target that examines the queue status.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: CNM is sent to a targetDev/targetPort.
*                    GT_FALSE: CNM is sent to a sampled packet's source.
*       targetHwDev- Target device for the CNM.
*                    Relevant only if enable==GT_TRUE.
*       targetPort - Target port for the CNM.
*                    Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range targetDev or targetPort
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueStatusModeEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable,
    IN GT_HW_DEV_NUM    targetHwDev,
    IN GT_PORT_NUM      targetPort
);

/*******************************************************************************
* cpssDxChPortCnQueueStatusModeEnableGet
*
* DESCRIPTION:
*        Gets the current status of queue status mode.
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
*       enablePtr     - (pointer to) status of queue status mode.
*                       GT_TRUE: CNM is sent to a targetDev/targetPort.
*                       GT_FALSE: CNM is sent to a sampled packet's source.
*       targetHwDevPtr- (pointer to) Target device for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*       targetPortPtr - (pointer to) Target port for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueStatusModeEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr,
    OUT GT_HW_DEV_NUM   *targetHwDevPtr,
    OUT GT_PORT_NUM     *targetPortPtr
);

/*******************************************************************************
* cpssDxChPortCnSampleEntrySet
*
* DESCRIPTION:
*       Sets CN sample interval entry.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       entryIndex  - index: qFb 3 msb. (0..7)
*       entryPtr   - (pointer to) entry settings.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.To implement CCFC functionality set entryPtr fields to:
*          interval = (MTU+256b)/16
*          randBitmap = 0
*       2.To achieve uniform distribution of random values, clear the interval
*         to be randomized to 0. For example, if interval = 0xF0 and
*         randBitmap = 0x0F then the sampling interval is a random value
*         in [0xF0,0xFF].
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSampleEntrySet
(
    IN GT_U8    devNum,
    IN GT_U8    entryIndex,
    IN CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
);

/*******************************************************************************
* cpssDxChPortCnSampleEntryGet
*
* DESCRIPTION:
*       Gets CN sample interval entry.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       entryIndex  - index: qFb 3 msb. (0..7)
* OUTPUTS:
*       entryPtr   - (pointer to) entry settings.
*
* RETURNS:
*       GT_OK                    - on success
*
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSampleEntryGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    entryIndex,
    OUT CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
);

/*******************************************************************************
* cpssDxChPortCnFbCalcConfigSet
*
* DESCRIPTION:
*       Sets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To implement CCFC functionality set fbCalcCfgPtr fields to:
*          deltaEnable = 0
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFbCalcConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
);

/*******************************************************************************
* cpssDxChPortCnFbCalcConfigGet
*
* DESCRIPTION:
*       Gets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFbCalcConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
);

/*******************************************************************************
* cpssDxChPortCnPacketLengthSet
*
* DESCRIPTION:
*       Sets packet length used by the CN frames triggering logic.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       packetLength - packet length.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or packet length.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Configuring a value other than CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E
*       should be used when the packet length is unknown, e.g. Cut-through mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPacketLengthSet
(
    IN GT_U8                               devNum,
    IN CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT packetLength
);

/*******************************************************************************
* cpssDxChPortCnPacketLengthGet
*
* DESCRIPTION:
*       Gets packet length used by the CN frames triggering logic.
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
*       packetLengthPtr - (pointer to) packet length.
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
GT_STATUS cpssDxChPortCnPacketLengthGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT  *packetLengthPtr
);

/*******************************************************************************
* cpssDxChPortCnMessageGenerationConfigSet
*
* DESCRIPTION:
*       Sets CNM generation configuration.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum - device number.
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageGenerationConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
);

/*******************************************************************************
* cpssDxChPortCnMessageGenerationConfigGet
*
* DESCRIPTION:
*       Gets CNM generation configuration.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageGenerationConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
);



/*******************************************************************************
* cpssDxChPortCnPrioritySpeedLocationSet
*
* DESCRIPTION:
*       Sets location of the priority/speed bits in the CNM header for parsing
*       and generation of CNM.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       prioritySpeedLocation   - priority speed location.
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
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPrioritySpeedLocationSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT prioritySpeedLocation
);

/*******************************************************************************
* cpssDxChPortCnPrioritySpeedLocationGet
*
* DESCRIPTION:
*       Gets location of the index (priority/speed) bits in the CNM header
*       for parsing and generation of CNM.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       prioritySpeedLocationPtr - (pointer to) priority speed location.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPrioritySpeedLocationGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT *prioritySpeedLocationPtr
);

/*******************************************************************************
* cpssDxChPortCnMessageTypeSet
*
* DESCRIPTION:
*       Sets congestion notification message type - QCN or CCFC
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       mType       - CN message type
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
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageTypeSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT   mType
);


/*******************************************************************************
* cpssDxChPortCnMessageTypeGet
*
* DESCRIPTION:
*       Gets congestion notification message type - QCN or CCFC
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       mTypePtr    - (pointer to) CN message type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageTypeGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT   *mTypePtr
);


/*******************************************************************************
* cpssDxChPortCnMessageTriggeringStateSet
*
* DESCRIPTION:
*       Sets CNM Triggering State Variables
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       tcQueue     - traffic class queue (0..7).
*       qcnSampleInterval -  parameter that defines the probability to trigger
*                     the next CN frame based on the current value of the
*                     calculated feedback (Fb), measured in Bytes.
*                     (APPLICABLE RANGES: 0..0xFFFFF)
*       qlenOld     - a snapshot of current length of the queue in buffers,
*                     taken when the QcnSampleInterval expires.
*                     (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number,portNum,tc
*       GT_OUT_OF_RANGE          - on wrong qcnSampleInterval, qlenOld
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       qlenOld reset value and QcnSampleInterval init value must be set by the
*       application prior to configuring the queue to be CN aware
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageTriggeringStateSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U8                   tcQueue,
    IN  GT_U32                  qcnSampleInterval,
    IN  GT_U32                  qlenOld
);


/*******************************************************************************
* cpssDxChPortCnMessageTriggeringStateGet
*
* DESCRIPTION:
*       Gets CNM Triggering State Variables
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       tcQueue     - traffic class queue (0..7).
*
* OUTPUTS:
*       qcnSampleIntervalPtr -  (pointer to) parameter that defines the probability to trigger
*                     the next CN frame based on the current value of the
*                     calculated feedback (Fb), measured in Bytes.
*       qlenOldPtr  - (pointer to) snapshot of current length of the queue in buffers,
*                     taken when the QcnSampleInterval expires.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number,portNum,tc
*       GT_OUT_OF_RANGE          - on wrong qcnSampleInterval, qlenOld
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       qlenOld reset value and QcnSampleInterval init value must be set by the
*       application prior to configuring the queue to be CN aware
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageTriggeringStateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U8                   tcQueue,
    OUT GT_U32                  *qcnSampleIntervalPtr,
    OUT GT_U32                  *qlenOldPtr
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortCnh */

