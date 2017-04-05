/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCatchUp.h
*
* DESCRIPTION:
*       private CPSS DxCh catch up data.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __prvCpssDxChCatchUph
#define __prvCpssDxChCatchUph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbAu.h>


/*
 * Typedef: struct PRV_CPSS_DXCH_AUQ_ENABLE_DATA_STC
 *
 * Description: This struct containes AUQ enable/disable status for different message types.
 *
 * Fields:
 *      naToCpuPerPortPtr        - enable/disable status of NA messages to CPU per port.
 *      naStormPreventPortPtr    - enable/disable status of New Address messages Storm Prevention.
 *      naToCpuLearnFail         - enable/disable status of NA messages to the CPU indicating that
 *                                 the device cannot learn a new SA.
 *      aaTaToCpu                - enable/disable status of  AA and TA address update messages to the CPU.
 *      spAaMsgToCpu             - enable/disable status of AA messages to the CPU for the Storm Prevention
 *                                 entries when those entries reach an aged out status.
 *      naTag1VLANassignment     - enable/disable status of Tag1 VLAN Id assignment in vid1 field of the NA AU
 *                                 message CPSS_MAC_UPDATE_MSG_EXT_STC.
 */
typedef struct
{
    GT_BOOL *naToCpuPerPortPtr;
    GT_BOOL *naStormPreventPortPtr;
    GT_BOOL naToCpuLearnFail;
    GT_BOOL aaTaToCpu;
    GT_BOOL spAaMsgToCpu;
    GT_BOOL naTag1VLANassignment;
}PRV_CPSS_DXCH_AUQ_ENABLE_DATA_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_FUQ_ACTION_DATA_STC
 *
 * Description: This struct containes FUQ enable/disable statuses of FDB upload action.
 *
 * Fields:
 *      fdbUploadState              - enable/disable fdb upload state.
 *      currentVid                  - action active vlan.
 *      currentVidMask              - action active vlan mask.
 *      actDev                      - action active device.
 *      actDevMask                  - ection active device mask.
 *      actTrunkPort                - action active interface (port/trunk).
 *      actIsTrunkMask              - action active interface (port/trunk) mask.
 *      actIsTrunk                  - determines if the interface is port or trunk.
 *      actIsTrunkMask              - action active trunk mask.
 *      actionMode                  - fdb action mode.
 *      triggerMode                 - trigger mode.
 *      actionEnable                - action enable state.
 *      maskAuFuMsg2CpuOnNonLocal   - enable/disable sending AU/FU messages to the CPU if the MAC entry
 *                                    does NOT reside on the local portgroup.
 *
 */
typedef struct
{
    GT_BOOL                          fdbUploadState;
    GT_U16                           currentVid ;
    GT_U16                           currentVidMask;
    GT_U32                           actDev ;
    GT_U32                           actDevMask;
    GT_U32                           actTrunkPort;
    GT_U32                           actTrunkPortMask;
    GT_U32                           actIsTrunk;
    GT_U32                           actIsTrunkMask;
    CPSS_FDB_ACTION_MODE_ENT         actionMode ;
    CPSS_MAC_ACTION_MODE_ENT         triggerMode;
    GT_BOOL                          actionEnable;
    GT_BOOL                          maskAuFuMsg2CpuOnNonLocal;
}PRV_CPSS_DXCH_FUQ_ACTION_DATA_STC;

/*******************************************************************************
* prvCpssCatchUpFuncPtr
*
* DESCRIPTION:
*       Define type of functions pointer performing synchronization of sw DB
*       with hw contents.
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on failure
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*prvCpssCatchUpFuncPtr)(GT_U8 devNum);

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpHandle
*
* DESCRIPTION:
*       Perform synchronization of hardware data and software DB after special init sequence.
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChSystemRecoveryCatchUpHandle
(
   GT_VOID
);

/*******************************************************************************
* prvCpssDxChHwDevNumCatchUp
*
* DESCRIPTION:
*       Synchronize hw device number in software DB by its hw value
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwDevNumCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChCpuPortModeCatchUp
*
* DESCRIPTION:
*       Synchronize cpu port mode in software DB by its hw value
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCpuPortModeCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChCpuSdmaPortGroupCatchUp
*
* DESCRIPTION:
*       Synchronize cpu sdma port group in software DB by its hw value
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCpuSdmaPortGroupCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChDevTableCatchUp
*
* DESCRIPTION:
*       Synchronize device table BMP in software DB by its hw value
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDevTableCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChFdbHashParamsModeCatchUp
*
* DESCRIPTION:
*       Synchronize fdb hash params in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFdbHashParamsModeCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChFdbActionHwDevNumActionHwDevNumMaskCatchUp
*
* DESCRIPTION:
*       Synchronize Active device number and active device number mask
*       in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFdbActionHwDevNumActionHwDevNumMaskCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChPrePendTwoBytesCatchUp
*
* DESCRIPTION:
*       Synchronize enable/disable pre-pending a two-byte header
*       in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPrePendTwoBytesCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChPolicerMemorySizeModeCatchUp
*
* DESCRIPTION:
*       Synchronize Policer Memory Size Mode in software DB by its hw values.
*
* APPLICABLE DEVICES:
*        Lion;  Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*         DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPolicerMemorySizeModeCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChSecurBreachPortDropCntrModeAndPortGroupCatchUp
*
* DESCRIPTION:
*       Synchronize Secure Breach Port Drop Counter Mode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSecurBreachPortDropCntrModeAndPortGroupCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChPortEgressCntrModeCatchUp
*
* DESCRIPTION:
*       Synchronize Port Egress Counters Mode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortEgressCntrModeCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChBridgeIngressCntrModeCatchUp
*
* DESCRIPTION:
*       Synchronize Bridge Ingress Counters Mode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBridgeIngressCntrModeCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChBridgeIngressDropCntrModeCatchUp
*
* DESCRIPTION:
*       Synchronize  Ingress Drop Counters Mode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBridgeIngressDropCntrModeCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChBridgeTag1VidFdbEnCatchUp
*
* DESCRIPTION:
*       Synchronize tag1VidFdbEn in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.

*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBridgeTag1VidFdbEnCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChBridgeEgrVlanPortFltTabAccessModeCatchUp
*
* DESCRIPTION:
*       Synchronize vlanfltTabAccessMode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.

*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBridgeEgrVlanPortFltTabAccessModeCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChPortModeParamsCatchUp
*
* DESCRIPTION:
*       Synchronize Port Mode parameters in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortModeParamsCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChPortResourcesCatchUp
*
* DESCRIPTION:
*       Synchronize Port resources parameters in software DB by its hw values
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortResourcesCatchUp
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpValidityCheckEnable
*
* DESCRIPTION:
*       Enable CatchUp validity check
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       None.
* 
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on failure
*
* COMMENTS:
*       Pre-condition for using this function:
*       CatchUp parameters in PP data structure must be equal to the HW values
*
*******************************************************************************/
GT_STATUS prvCpssDxChSystemRecoveryCatchUpValidityCheckEnable
(
    IN  GT_U8   devNum
);

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpValidityCheck
*
* DESCRIPTION:
*       Validates that SW params hold the correct value from HW after CatchUp
*       process.
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       None.
* 
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on failure
*
* COMMENTS:
*       To use this API, cpssDxChCatchUpValidityCheckEnable must be called before
*       performing CatchUp
*
*******************************************************************************/
GT_STATUS prvCpssDxChSystemRecoveryCatchUpValidityCheck
(
    IN  GT_U8   devNum
);

/*******************************************************************************
* prvCpssDxChHaAuqNonContinuousMsgModeHandle
*
* DESCRIPTION:
*       This function performs AUQ workaround after HA event. It makes PP to consider that
*       queue is full and to be ready for reprogramming.
*       The workaround should be used when application can't guarantee the same memory
*       allocated for AUQ before and after HA event.
*       Before calling this function application should disable access CPU memory by device.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHaAuqNonContinuousMsgModeHandle
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChHaFuqNonContinuousMsgModeHandle
*
* DESCRIPTION:
*       This function performs FUQ workaround after HA event. It makes PP to consider that
*       queue is full and to be ready for reprogramming.
*       The workaround should be used when application can't guarantee the same memory
*       allocated for FUQ before and after HA event.
*       Before calling this function application should disable access of device to CPU memory.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHaFuqNonContinuousMsgModeHandle
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChDisableAuqAndSaveAuqCurrentStatus
*
* DESCRIPTION:
*       This function disable AUQ for messages and save current AUQ messages enable status.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDisableAuqAndSaveAuqCurrentStatus
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChRestoreAuqCurrentStatus
*
* DESCRIPTION:
*       This function retieve  AUQ enable/disable message status
*       and apply it on the device.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChRestoreAuqCurrentStatus
(
    IN GT_U8    devNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChHsuh */

