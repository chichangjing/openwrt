/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmNodesStatus.c
*
* DESCRIPTION:
*       TM nodes status reading APIs
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
#include <cpss/generic/tm/cpssTmNodesStatus.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <tm_nodes_status.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h> 


/*******************************************************************************
* internal_cpssTmQueueStatusRead
*
* DESCRIPTION:
*       Read Queue status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
static GT_STATUS internal_cpssTmQueueStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_NODE_STATUS_STC     *statusPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    struct tm_node_status stat;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    ret = tm_read_queue_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &stat);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    cpssOsMemCpy(statusPtr, &stat, sizeof(struct tm_node_status));

    return rc;
}

/*******************************************************************************
* cpssTmQueueStatusRead
*
* DESCRIPTION:
*       Read Queue status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
GT_STATUS cpssTmQueueStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_NODE_STATUS_STC     *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmQueueStatusRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, statusPtr));

    rc = internal_cpssTmQueueStatusRead(devNum, index, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmAnodeStatusRead
*
* DESCRIPTION:
*       Read A-Node status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
static GT_STATUS internal_cpssTmAnodeStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_NODE_STATUS_STC     *statusPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    struct tm_node_status stat;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    ret = tm_read_a_node_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &stat);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    cpssOsMemCpy(statusPtr, &stat, sizeof(struct tm_node_status));

    return rc;
}

/*******************************************************************************
* cpssTmAnodeStatusRead
*
* DESCRIPTION:
*       Read A-Node status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
GT_STATUS cpssTmAnodeStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_NODE_STATUS_STC     *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmAnodeStatusRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, statusPtr));

    rc = internal_cpssTmAnodeStatusRead(devNum, index, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmBnodeStatusRead
*
* DESCRIPTION:
*       Read B-Node status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
static GT_STATUS internal_cpssTmBnodeStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_NODE_STATUS_STC     *statusPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    struct tm_node_status stat;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    ret = tm_read_b_node_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &stat);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    cpssOsMemCpy(statusPtr, &stat, sizeof(struct tm_node_status));

    return rc;
}

/*******************************************************************************
* cpssTmBnodeStatusRead
*
* DESCRIPTION:
*       Read B-Node status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
GT_STATUS cpssTmBnodeStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_NODE_STATUS_STC     *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmBnodeStatusRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, statusPtr));

    rc = internal_cpssTmBnodeStatusRead(devNum, index, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmCnodeStatusRead
*
* DESCRIPTION:
*       Read C-Node status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
static GT_STATUS internal_cpssTmCnodeStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_NODE_STATUS_STC     *statusPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    struct tm_node_status stat;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    ret = tm_read_c_node_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &stat);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    cpssOsMemCpy(statusPtr, &stat, sizeof(struct tm_node_status));

    return rc;
}

/*******************************************************************************
* cpssTmCnodeStatusRead
*
* DESCRIPTION:
*       Read C-Node status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
GT_STATUS cpssTmCnodeStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_NODE_STATUS_STC     *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmCnodeStatusRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, statusPtr));

    rc = internal_cpssTmCnodeStatusRead(devNum, index, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmPortStatusRead
*
* DESCRIPTION:
*       Read Port status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
static GT_STATUS internal_cpssTmPortStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_PORT_STATUS_STC     *statusPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    struct tm_port_status stat;
    int i=0;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);
    
    ret = tm_read_port_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)index, &stat);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    statusPtr->maxBucketLevel = stat.max_bucket_level;
    statusPtr->minBucketLevel = stat.min_bucket_level;
    for(i=0; i<8; i++)
    {
        statusPtr->deficitArr[i] = stat.deficit[i];
    }

    return rc;
}

/*******************************************************************************
* cpssTmPortStatusRead
*
* DESCRIPTION:
*       Read Port status. 
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
*       statusPtr       - (pointer to) Node status structure.
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
GT_STATUS cpssTmPortStatusRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT CPSS_TM_PORT_STATUS_STC     *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortStatusRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, statusPtr));

    rc = internal_cpssTmPortStatusRead(devNum, index, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmDropQueueLengthGet
*
* DESCRIPTION:
*       Read Queue drop length. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Node level (Port/C/B/A/Queue).
*       index           - Node index.
*
* OUTPUTS:
*       instantQueueLengthPtr       - (pointer to) Current Queue length structure.
*       averageQueueLengthPtr       - (pointer to) Average Queue length structure.
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
static GT_STATUS internal_cpssTmDropQueueLengthGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_TM_LEVEL_ENT           level,
    IN  GT_U32                      index,
    OUT GT_U32                      *instantQueueLengthPtr,
    OUT GT_U32                      *averageQueueLengthPtr
)
{
    GT_STATUS   rc = GT_OK;
    int         ret = 0;
    
    uint32_t inst_ql;
    uint32_t av_ql;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    CPSS_NULL_PTR_CHECK_MAC(instantQueueLengthPtr);
    CPSS_NULL_PTR_CHECK_MAC(averageQueueLengthPtr);

    ret = tm_drop_get_queue_length(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                   level,
                                   index,
                                   &inst_ql,
                                   &av_ql);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    *instantQueueLengthPtr = inst_ql;
    *averageQueueLengthPtr = av_ql;
    return rc;
}

/*******************************************************************************
* cpssTmDropQueueLengthGet
*
* DESCRIPTION:
*       Read Queue drop length. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Node level (Port/C/B/A/Queue).
*       index           - Node index.
*
* OUTPUTS:
*       instantQueueLengthPtr       - (pointer to) Current Queue length structure.
*       averageQueueLengthPtr       - (pointer to) Average Queue length structure.
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
GT_STATUS cpssTmDropQueueLengthGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_TM_LEVEL_ENT           level,
    IN  GT_U32                      index,
    OUT GT_U32                      *instantQueueLengthPtr,
    OUT GT_U32                      *averageQueueLengthPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmDropQueueLengthGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, index, instantQueueLengthPtr, averageQueueLengthPtr));

    rc = internal_cpssTmDropQueueLengthGet(devNum, level, index, instantQueueLengthPtr, averageQueueLengthPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, index, instantQueueLengthPtr, averageQueueLengthPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

