/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmNodesRead.c
*
* DESCRIPTION:
*       TM nodes configuration reading APIs
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
#include <cpss/generic/tm/cpssTmNodesRead.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <tm_nodes_read.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

/***************************************************************************
 * Read Configuration
 ***************************************************************************/

/*******************************************************************************
* internal_cpssTmQueueConfigurationRead
*
* DESCRIPTION:
*       Read queue software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) Queue parameters structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmQueueConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_QUEUE_PARAMS_STC        *paramsPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    struct tm_queue_params prms;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
    CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                               | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_read_queue_configuration(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    paramsPtr->shapingProfilePtr = prms.shaping_profile_ref;
    paramsPtr->quantum = prms.quantum;
    paramsPtr->dropProfileInd = prms.wred_profile_ref;
    paramsPtr->eligiblePrioFuncId = prms.elig_prio_func_ptr;
    return rc;
}

/*******************************************************************************
* cpssTmQueueConfigurationRead
*
* DESCRIPTION:
*       Read queue software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) Queue parameters structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmQueueConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_QUEUE_PARAMS_STC        *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmQueueConfigurationRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmQueueConfigurationRead(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmAnodeConfigurationRead
*
* DESCRIPTION:
*       Read A-node software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) A-Node parameters structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmAnodeConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_A_NODE_PARAMS_STC       *paramsPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    int         i;
    struct tm_a_node_params prms;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
    CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                       | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_read_a_node_configuration(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    paramsPtr->shapingProfilePtr = prms.shaping_profile_ref;
    paramsPtr->quantum = prms.quantum;
    for(i=0; i<8; i++)
        paramsPtr->schdModeArr[i] = prms.dwrr_priority[i];
    paramsPtr->dropProfileInd = prms.wred_profile_ref;
    paramsPtr->eligiblePrioFuncId = prms.elig_prio_func_ptr;
    paramsPtr->numOfChildren = prms.num_of_children;
    return rc;
}

/*******************************************************************************
* cpssTmAnodeConfigurationRead
*
* DESCRIPTION:
*       Read A-node software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) A-Node parameters structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmAnodeConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_A_NODE_PARAMS_STC       *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmAnodeConfigurationRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmAnodeConfigurationRead(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmBnodeConfigurationRead
*
* DESCRIPTION:
*       Read B-node software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) B-Node parameters structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmBnodeConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_B_NODE_PARAMS_STC       *paramsPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    int         i;
    struct tm_b_node_params prms;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
    CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                   | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    ret = tm_read_b_node_configuration(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    paramsPtr->shapingProfilePtr = prms.shaping_profile_ref;
    paramsPtr->quantum = prms.quantum;
    for(i=0; i<8; i++)
        paramsPtr->schdModeArr[i] = prms.dwrr_priority[i];
    paramsPtr->dropProfileInd = prms.wred_profile_ref;
    paramsPtr->eligiblePrioFuncId = prms.elig_prio_func_ptr;
    paramsPtr->numOfChildren = prms.num_of_children;
    return rc;
}

/*******************************************************************************
* cpssTmBnodeConfigurationRead
*
* DESCRIPTION:
*       Read B-node software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) B-Node parameters structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmBnodeConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_B_NODE_PARAMS_STC       *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmBnodeConfigurationRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmBnodeConfigurationRead(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmCnodeConfigurationRead
*
* DESCRIPTION:
*       Read C-node software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) C-Node parameters structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmCnodeConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_C_NODE_PARAMS_STC       *paramsPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    int         i;
    struct tm_c_node_params prms;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
    CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                               | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    ret = tm_read_c_node_configuration(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    paramsPtr->dropCosMap = prms.wred_cos;
    for(i=0; i<8; i++)
    {
        paramsPtr->dropProfileIndArr[i] = prms.wred_profile_ref[i];
        paramsPtr->schdModeArr[i]= prms.dwrr_priority[i];
    }
    paramsPtr->eligiblePrioFuncId = prms.elig_prio_func_ptr;
    paramsPtr->numOfChildren = prms.num_of_children;
    paramsPtr->shapingProfilePtr = prms.shaping_profile_ref;
    paramsPtr->quantum = prms.quantum;

    return rc;
}

/*******************************************************************************
* cpssTmCnodeConfigurationRead
*
* DESCRIPTION:
*       Read C-node software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) C-Node parameters structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmCnodeConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_C_NODE_PARAMS_STC       *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmCnodeConfigurationRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr));

    rc = internal_cpssTmCnodeConfigurationRead(devNum, index, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmPortConfigurationRead
*
* DESCRIPTION:
*       Read port software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) Port parameters structure.
*       cosParamsPtr    - (pointer to) Port Drop per Cos structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmPortConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_PORT_PARAMS_STC         *paramsPtr,
    OUT CPSS_TM_PORT_DROP_PER_COS_STC   *cosParamsPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    int         i;
    struct tm_port_params prms;
    struct tm_port_drop_per_cos cos;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
    CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                               | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    ret = tm_read_port_configuration(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &prms, &cos);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    paramsPtr->cirBw = prms.cir_bw;
    paramsPtr->eirBw = prms.eir_bw;
    paramsPtr->cbs = prms.cbs;
    paramsPtr->ebs = prms.ebs;
    for(i=0; i<8; i++) {
        paramsPtr->quantumArr[i] = prms.quantum[i];
        paramsPtr->schdModeArr[i] = prms.dwrr_priority[i];
    }
    paramsPtr->dropProfileInd = prms.wred_profile_ref;
    paramsPtr->eligiblePrioFuncId = prms.elig_prio_func_ptr;
    paramsPtr->numOfChildren = prms.num_of_children;

    cosParamsPtr->dropCosMap = cos.wred_cos;
    for(i=0; i<8; i++)
        cosParamsPtr->dropProfileIndArr[i] = cos.wred_profile_ref[i];

    return rc;
}

/*******************************************************************************
* cpssTmPortConfigurationRead
*
* DESCRIPTION:
*       Read port software configuration.
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
*
* OUTPUTS:
*       paramsPtr       - (pointer to) Port parameters structure.
*       cosParamsPtr    - (pointer to) Port Drop per Cos structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmPortConfigurationRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TM_PORT_PARAMS_STC         *paramsPtr,
    OUT CPSS_TM_PORT_DROP_PER_COS_STC   *cosParamsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortConfigurationRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, paramsPtr, cosParamsPtr));

    rc = internal_cpssTmPortConfigurationRead(devNum, index, paramsPtr, cosParamsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, paramsPtr, cosParamsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

