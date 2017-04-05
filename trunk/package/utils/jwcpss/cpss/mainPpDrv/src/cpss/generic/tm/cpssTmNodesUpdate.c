/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmNodesUpdate.c
*
* DESCRIPTION:
*       TM nodes update APIs
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/tm/private/prvCpssGenTmLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/tm/cpssTmServices.h>
#include <cpss/generic/tm/cpssTmNodesUpdate.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <tm_nodes_update.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>


/***************************************************************************
 * Help Function
 ***************************************************************************/

/*******************************************************************************
* prvCpssTmPortDropPerCosParamsUpdCpy
*
* DESCRIPTION:
*       Copy CPSS Port Drop per Cos structure to TM Port Drop per Cos structure.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       paramsPtr       - CPSS Port Drop per Cos parameters structure pointer.
*
* OUTPUTS:
*       prmsPtr         - TM Port Drop per Cos parameters structure pointer.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvCpssTmPortDropPerCosParamsUpdCpy
(
    IN  CPSS_TM_PORT_DROP_PER_COS_STC     *paramsPtr,
    OUT struct tm_port_drop_per_cos       *prmsPtr
)
{
    int i;

    for(i=0; i<8; i++)
        prmsPtr->wred_profile_ref[i] = (uint8_t)paramsPtr->dropProfileIndArr[i];
    prmsPtr->wred_cos = (uint8_t)paramsPtr->dropCosMap;
}


/*******************************************************************************
* prvCpssTmCNodeParamsUpdCpy
*
* DESCRIPTION:
*       Copy CPSS C-Node structure to TM C-Node structure.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       paramsPtr       - CPSS C-Node parameters structure pointer.
*
* OUTPUTS:
*       prmsPtr         - TM C-Node parameters structure pointer.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvCpssTmCNodeParamsUpdCpy
(
    IN CPSS_TM_C_NODE_PARAMS_STC      *paramsPtr,
    OUT struct tm_c_node_params       *prmsPtr
)
{
    int i;

    prmsPtr->shaping_profile_ref = paramsPtr->shapingProfilePtr;
    prmsPtr->quantum = (uint16_t)paramsPtr->quantum;
    for(i=0; i<8; i++)
        prmsPtr->dwrr_priority[i] = (uint8_t)paramsPtr->schdModeArr[i];
    prmsPtr->wred_cos = (uint8_t)paramsPtr->dropCosMap;
    for(i=0; i<8; i++)
        prmsPtr->wred_profile_ref[i] = (uint8_t)paramsPtr->dropProfileIndArr[i];
    prmsPtr->elig_prio_func_ptr = (uint8_t)paramsPtr->eligiblePrioFuncId;
}


/*******************************************************************************
* prvCpssTmBNodeParamsUpdCpy
*
* DESCRIPTION:
*       Copy CPSS B-Node structure to TM B-Node structure.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       paramsPtr       - CPSS B-Node parameters structure pointer.
*
* OUTPUTS:
*       prmsPtr         - TM B-Node parameters structure pointer.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvCpssTmBNodeParamsUpdCpy
(
    IN CPSS_TM_B_NODE_PARAMS_STC      *paramsPtr,
    OUT struct tm_b_node_params       *prmsPtr
)
{
    int i;

    prmsPtr->shaping_profile_ref = paramsPtr->shapingProfilePtr;
    prmsPtr->quantum = (uint16_t)paramsPtr->quantum;
    for(i=0; i<8; i++)
        prmsPtr->dwrr_priority[i] = (uint8_t)paramsPtr->schdModeArr[i];
    prmsPtr->wred_profile_ref = (uint16_t)paramsPtr->dropProfileInd;
    prmsPtr->elig_prio_func_ptr = (uint8_t)paramsPtr->eligiblePrioFuncId;
}


/*******************************************************************************
* prvCpssTmANodeParamsUpdCpy
*
* DESCRIPTION:
*       Copy CPSS A-Node structure to TM A-Node structure.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       paramsPtr       - CPSS A-Node parameters structure pointer.
*
* OUTPUTS:
*       prmsPtr         - TM A-Node parameters structure pointer.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvCpssTmANodeParamsUpdCpy
(
    IN CPSS_TM_A_NODE_PARAMS_STC      *paramsPtr,
    OUT struct tm_a_node_params       *prmsPtr
)
{
    int i;

    prmsPtr->shaping_profile_ref = paramsPtr->shapingProfilePtr;
    prmsPtr->quantum = (uint16_t)paramsPtr->quantum;
    for(i=0; i<8; i++)
        prmsPtr->dwrr_priority[i] = (uint8_t)paramsPtr->schdModeArr[i];
    prmsPtr->wred_profile_ref = (uint16_t)paramsPtr->dropProfileInd;
    prmsPtr->elig_prio_func_ptr = (uint8_t)paramsPtr->eligiblePrioFuncId;
}


/*******************************************************************************
* prvCpssTmQueueParamsUpdCpy
*
* DESCRIPTION:
*       Copy CPSS Queue structure to TM Queue structure.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       paramsPtr       - CPSS Queue parameters structure pointer.
*
* OUTPUTS:
*       prmsPtr         - TM Queue parameters structure pointer.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvCpssTmQueueParamsUpdCpy
(
    IN CPSS_TM_QUEUE_PARAMS_STC      *paramsPtr,
    OUT struct tm_queue_params       *prmsPtr
)
{
    prmsPtr->shaping_profile_ref = paramsPtr->shapingProfilePtr;
    prmsPtr->quantum = (uint16_t)paramsPtr->quantum;
    prmsPtr->wred_profile_ref = (uint16_t)paramsPtr->dropProfileInd;
    prmsPtr->elig_prio_func_ptr = (uint8_t)paramsPtr->eligiblePrioFuncId;
}


/***************************************************************************
 * Parameters Update
 ***************************************************************************/

/*******************************************************************************
* internal_cpssTmQueueUpdate
*
* DESCRIPTION:
*       Update queue parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Queue index.
*       paramsPtr       - Queue parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'dropProfileInd' parameter will be updated in any case,
*       set it's value to be the same as in DB if you don't want to change it.
*******************************************************************************/
static GT_STATUS internal_cpssTmQueueUpdate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_TM_QUEUE_PARAMS_STC         *paramsPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    struct tm_queue_params prms;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    prvCpssTmQueueParamsUpdCpy(paramsPtr, &prms);
    ret = tm_update_queue(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmQueueUpdate
*
* DESCRIPTION:
*       Update queue parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Queue index.
*       paramsPtr       - Queue parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'dropProfileInd' parameter will be updated in any case,
*       set it's value to be the same as in DB if you don't want to change it.
*******************************************************************************/
GT_STATUS cpssTmQueueUpdate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_TM_QUEUE_PARAMS_STC         *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmQueueUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmQueueUpdate(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmAnodeUpdate
*
* DESCRIPTION:
*       Update A-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*       3. 'dropProfileInd' parameter will be updated in any case,
*       set it's value to be the same as in DB if you don't want to change it.
*******************************************************************************/
static GT_STATUS internal_cpssTmAnodeUpdate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_TM_A_NODE_PARAMS_STC        *paramsPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    struct tm_a_node_params prms;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                        | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );
     CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    prvCpssTmANodeParamsUpdCpy(paramsPtr, &prms);
    ret = tm_update_a_node(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmAnodeUpdate
*
* DESCRIPTION:
*       Update A-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*       3. 'dropProfileInd' parameter will be updated in any case,
*       set it's value to be the same as in DB if you don't want to change it.
*******************************************************************************/
GT_STATUS cpssTmAnodeUpdate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_TM_A_NODE_PARAMS_STC        *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmAnodeUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmAnodeUpdate(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmBnodeUpdate
*
* DESCRIPTION:
*       Update B-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*******************************************************************************/
static GT_STATUS internal_cpssTmBnodeUpdate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_TM_B_NODE_PARAMS_STC        *paramsPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    struct tm_b_node_params prms;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    prvCpssTmBNodeParamsUpdCpy(paramsPtr, &prms);
    ret = tm_update_b_node(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmBnodeUpdate
*
* DESCRIPTION:
*       Update B-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*******************************************************************************/
GT_STATUS cpssTmBnodeUpdate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_TM_B_NODE_PARAMS_STC        *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmBnodeUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmBnodeUpdate(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmCnodeUpdate
*
* DESCRIPTION:
*       Update C-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*******************************************************************************/
static GT_STATUS internal_cpssTmCnodeUpdate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_TM_C_NODE_PARAMS_STC        *paramsPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    struct tm_c_node_params prms;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    prvCpssTmCNodeParamsUpdCpy(paramsPtr, &prms);
    ret = tm_update_c_node(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmCnodeUpdate
*
* DESCRIPTION:
*       Update C-node parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*       paramsPtr       - Node parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. When error occurs, the entry is considered inconsistent.
*       2. 'numOfChildren' can't be updated.
*******************************************************************************/
GT_STATUS cpssTmCnodeUpdate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_TM_C_NODE_PARAMS_STC        *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmCnodeUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmCnodeUpdate(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmPortShapingUpdate
*
* DESCRIPTION:
*       Update Port Shaping parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       cirBw           - Port CIR bandwidth in KBit/Sec (APPLICABLE RANGES: 0..100G).
*       eirBw           - Port EIR bandwidth in KBit/Sec (APPLICABLE RANGES: 0..100G).
*       cbs             - Port CIR burst size in kbytes.
*       ebs             - Port EIR burst size in kbytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_BAD_SIZE              - on Port's min/max token too large.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
static GT_STATUS internal_cpssTmPortShapingUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN GT_U32   cirBw,
    IN GT_U32   eirBw,
    IN GT_U32   cbs,
    IN GT_U32   ebs
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_update_port_shaping(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)index,
                                 cirBw, eirBw, cbs, ebs);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmPortShapingUpdate
*
* DESCRIPTION:
*       Update Port Shaping parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*   	index           - Port index.
*       cirBw           - Port CIR bandwidth in KBit/Sec (APPLICABLE RANGES: 0..100G).
*       eirBw           - Port EIR bandwidth in KBit/Sec (APPLICABLE RANGES: 0..100G).
*       cbs             - Port CIR burst size in kbytes.
*       ebs             - Port EIR burst size in kbytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_BAD_SIZE              - on Port's min/max token too large.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
GT_STATUS cpssTmPortShapingUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN GT_U32   cirBw,
    IN GT_U32   eirBw,
    IN GT_U32   cbs,
    IN GT_U32   ebs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortShapingUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, cirBw, eirBw, cbs, ebs));

    rc = internal_cpssTmPortShapingUpdate(devNum, index, cirBw, eirBw, cbs, ebs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, cirBw, eirBw, cbs, ebs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmPortSchedulingUpdate
*
* DESCRIPTION:
*       Update Port Scheduling parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       eligPrioFuncId  - Eligible Priority Function pointer.
*       quantumArrPtr   - Port quantum 8 cell array.
*       schdModeArr     - Port RR/DWRR priority pointer for C-level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
static GT_STATUS internal_cpssTmPortSchedulingUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN CPSS_TM_ELIG_FUNC_NODE_ENT   eligPrioFuncId,
    IN GT_U32   quantumArrPtr[8], /* 8 cells array */
    IN CPSS_TM_SCHD_MODE_ENT  schdModeArr[8]
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    uint8_t prio[8];
    uint16_t quant[8];
    int i;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(schdModeArr);

    for(i=0; i<8; i++) {
        prio[i] = (uint8_t)schdModeArr[i];
        quant[i] = (uint16_t)quantumArrPtr[i];
    }
    ret = tm_update_port_scheduling(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)index,
                                    (uint8_t)eligPrioFuncId, quant, prio);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmPortSchedulingUpdate
*
* DESCRIPTION:
*       Update Port Scheduling parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       eligPrioFuncId  - Eligible Priority Function pointer.
*       quantumArrPtr   - Port quantum 8 cell array.
*       schdModeArr     - Port RR/DWRR priority pointer for C-level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
GT_STATUS cpssTmPortSchedulingUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN CPSS_TM_ELIG_FUNC_NODE_ENT   eligPrioFuncId,
    IN GT_U32   quantumArrPtr[8], /* 8 cells array */
    IN CPSS_TM_SCHD_MODE_ENT  schdModeArr[8]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortSchedulingUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, eligPrioFuncId, quantumArrPtr, schdModeArr));

    rc = internal_cpssTmPortSchedulingUpdate(devNum, index, eligPrioFuncId, quantumArrPtr, schdModeArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, eligPrioFuncId, quantumArrPtr, schdModeArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmPortDropUpdate
*
* DESCRIPTION:
*       Update Port Drop parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       wredProfileRef  - Port Drop Global Profile reference.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
static GT_STATUS internal_cpssTmPortDropUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN GT_U32   wredProfileRef
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_update_port_drop(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)index, (uint8_t)wredProfileRef);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmPortDropUpdate
*
* DESCRIPTION:
*       Update Port Drop parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       wredProfileRef  - Port Drop Global Profile reference.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
GT_STATUS cpssTmPortDropUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN GT_U32   wredProfileRef
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortDropUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, wredProfileRef));

    rc = internal_cpssTmPortDropUpdate(devNum, index, wredProfileRef);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, wredProfileRef));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmPortDropCosUpdate
*
* DESCRIPTION:
*       Update Port Drop per Cos parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       paramsPtr       - Port Drop per Cos parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
static GT_STATUS internal_cpssTmPortDropCosUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN CPSS_TM_PORT_DROP_PER_COS_STC   *paramsPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    struct tm_port_drop_per_cos prms;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    prvCpssTmPortDropPerCosParamsUpdCpy(paramsPtr, &prms);
    ret = tm_update_port_drop_cos(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmPortDropCosUpdate
*
* DESCRIPTION:
*       Update Port Drop per Cos parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Port index.
*       paramsPtr       - Port Drop per Cos parameters structure pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_OUT_OF_RANGE          - on parameter value out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       When error occurs, the entry is considered inconsistent.
*******************************************************************************/
GT_STATUS cpssTmPortDropCosUpdate
(
    IN GT_U8    devNum,
    IN GT_U32   index,
    IN CPSS_TM_PORT_DROP_PER_COS_STC   *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortDropCosUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmPortDropCosUpdate(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
