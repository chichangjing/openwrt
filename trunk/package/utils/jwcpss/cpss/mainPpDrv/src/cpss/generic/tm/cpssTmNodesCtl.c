/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmNodesCtl.c
*
* DESCRIPTION:
*       TM nodes control APIs
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
#include <cpss/generic/tm/cpssTmNodesCtl.h>
#include <tm_nodes_ctl.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>


/*******************************************************************************
* prvCpssTmTreeChangeCpy
*
* DESCRIPTION:
*       Copy CPSS TM_TREE_CHANGE structure to TM TM_TREE_CHANGE structure.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       prmsPtr         - TM Tree change parameters structure pointer.
*
* OUTPUTS:
*       paramsPtr       - CPSS Tree change parameters structure pointer.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvCpssTmTreeChangeCpy
(

    IN  struct tm_tree_change      *prmsPtr,
    OUT CPSS_TM_TREE_CHANGE_STC    *paramsPtr
)
{
    paramsPtr->index = prmsPtr->index;
    paramsPtr->oldIndex = prmsPtr->old_index;
    paramsPtr->newIndex = prmsPtr->new_index;
}


/*******************************************************************************
* internal_cpssTmNodesCtlQueueInstall
*
* DESCRIPTION:
*       Install queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Makes a queue capable of getting traffic.
*******************************************************************************/
static GT_STATUS internal_cpssTmNodesCtlQueueInstall
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_install_queue(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, queueInd);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmNodesCtlQueueInstall
*
* DESCRIPTION:
*       Install queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Makes a queue capable of getting traffic.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlQueueInstall
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodesCtlQueueInstall);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueInd));

    rc = internal_cpssTmNodesCtlQueueInstall(devNum, queueInd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueInd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmNodesCtlQueueUninstall
*
* DESCRIPTION:
*       Uninstall queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       - Makes a queue incapable of getting traffic. Queue must be not in use!
*       - User should flush Q before this call.
*******************************************************************************/
static GT_STATUS internal_cpssTmNodesCtlQueueUninstall
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_uninstall_queue(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, queueInd);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmNodesCtlQueueUninstall
*
* DESCRIPTION:
*       Uninstall queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       - Makes a queue incapable of getting traffic. Queue must be not in use!
*       - User should flush Q before this call.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlQueueUninstall
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodesCtlQueueUninstall);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueInd));

    rc = internal_cpssTmNodesCtlQueueUninstall(devNum, queueInd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueInd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmNodesCtlQueueInstallStatusGet
*
* DESCRIPTION:
*       Retrieves install status if the queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
* OUTPUTS:
*       statusPtr         - (Pointer to) queue install status .
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL statusPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmNodesCtlQueueInstallStatusGet
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd,
    OUT GT_U32  *statusPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
	uint8_t		status;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_queue_install_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, queueInd, &status);
    *statusPtr = (GT_U32)status;

    rc = XEL_TO_CPSS_ERR_CODE(ret);
    return rc;
}

/*******************************************************************************
* cpssTmNodesCtlQueueInstallStatusGet
*
* DESCRIPTION:
*       Retrieves install status if the queue by a given index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       queueInd          - Queue index (APPLICABLE RANGES: 0..16383).
* OUTPUTS:
*       statusPtr         - (Pointer to) queue install status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL statusPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlQueueInstallStatusGet
(
    IN GT_U8    devNum,
    IN GT_U32   queueInd,
    OUT GT_U32  *statusPtr
)
{
	GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodesCtlQueueInstallStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueInd, statusPtr));

    rc = internal_cpssTmNodesCtlQueueInstallStatusGet(devNum, queueInd, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueInd, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;

}



/*******************************************************************************
* internal_cpssTmNodesCtlNodeDelete
*
* DESCRIPTION:
*       Delete node from scheduling hierarchy.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       level             - Scheduling level: Queue/A/B/C-node/Port.
*       index             - Queue/node index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_STATE             - on not free node's subtree.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmNodesCtlNodeDelete
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    IN GT_U32                   index
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_delete_node(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, level, index);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmNodesCtlNodeDelete
*
* DESCRIPTION:
*       Delete node from scheduling hierarchy.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       level             - Scheduling level: Queue/A/B/C-node/Port.
*       index             - Queue/node index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_STATE             - on not free node's subtree.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlNodeDelete
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    IN GT_U32                   index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodesCtlNodeDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, index));

    rc = internal_cpssTmNodesCtlNodeDelete(devNum, level, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmNodesCtlTransPortDelete
*
* DESCRIPTION:
*       Delete port and all its subtree from scheduling hierarchy.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
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
static GT_STATUS internal_cpssTmNodesCtlTransPortDelete
(
    IN GT_U8    devNum,
    IN GT_U32   index
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_delete_trans_port(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)index);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmNodesCtlTransPortDelete
*
* DESCRIPTION:
*       Delete port and all its subtree from scheduling hierarchy.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssTmNodesCtlTransPortDelete
(
    IN GT_U8    devNum,
    IN GT_U32   index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodesCtlTransPortDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index));

    rc = internal_cpssTmNodesCtlTransPortDelete(devNum, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmQueueFlush
*
* DESCRIPTION:
*       Flush queue by a given index. All packets dequeued from this queue are dropped.
*       If block=GT_TRUE, wait till flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*       block             - Blocking Enabled/Disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmQueueFlush
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    IN GT_BOOL                  block
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_flush_queue(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, (uint8_t)block);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmQueueFlush
*
* DESCRIPTION:
*       Flush queue by a given index. All packets dequeued from this queue are dropped.
*       Wait till flush queue has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush queue blocking API.
*******************************************************************************/
GT_STATUS cpssTmQueueFlush
(
    IN GT_U8                    devNum,
    IN GT_U32                   index
)
{
    GT_STATUS rc;
    GT_BOOL   block = GT_TRUE;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmQueueFlush);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index));

    rc = internal_cpssTmQueueFlush(devNum, index, block);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* cpssTmQueueFlushTriggerActionStart
*
* DESCRIPTION:
*       Activate Flush queue by a given index.
*       All packets dequeued from this queue are dropped.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush queue non blocking API.
*******************************************************************************/
GT_STATUS cpssTmQueueFlushTriggerActionStart
(
    IN GT_U8                    devNum,
    IN GT_U32                   index
)
{
    GT_STATUS rc;
    GT_BOOL   block = GT_FALSE;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmQueueFlushTriggerActionStart);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index));

    rc = internal_cpssTmQueueFlush(devNum, index, block);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmQueueFlushStatusGet
*
* DESCRIPTION:
*       Get whether Flush queue has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       actFinishedPtr  - GT_FALSE - flush is active (packets are dropped on dequeue).
*                       - GT_TRUE  - flush off /completed.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_BAD_PTR               - on NULL actFinishedPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmQueueFlushStatusGet
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    OUT GT_BOOL                 *actFinishedPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    uint8_t     status;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actFinishedPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_flush_queue_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &status);
    if (actFinishedPtr)
        *actFinishedPtr = status ? GT_FALSE : GT_TRUE;
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}


/*******************************************************************************
* cpssTmQueueFlushTriggerActionStatusGet
*
* DESCRIPTION:
*       Get whether Flush queue has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Queue index.
*
* OUTPUTS:
*       actFinishedPtr  - GT_FALSE - flush is active (packets are dropped on dequeue).
*                       - GT_TRUE  - flush off /completed.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_BAD_PTR               - on NULL actFinishedPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmQueueFlushTriggerActionStatusGet
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    OUT GT_BOOL                 *actFinishedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmQueueFlushTriggerActionStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, actFinishedPtr));

    rc = internal_cpssTmQueueFlushStatusGet(devNum, index, actFinishedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, actFinishedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmPortFlush
*
* DESCRIPTION:
*       Flush port by a given index. All packets dequeued from this port (and all its queues)
*       are dropped. If block=GT_TRUE, wait till flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*       block             - Blocking Enabled/Disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmPortFlush
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    IN GT_BOOL                  block
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_flush_port(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, (uint8_t)block);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}


/*******************************************************************************
* cpssTmPortFlush
*
* DESCRIPTION:
*       Flush port by a given index. All packets dequeued from this port (and all its queues)
*       are dropped. Wait till flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush port blocking API.
*******************************************************************************/
GT_STATUS cpssTmPortFlush
(
    IN GT_U8                    devNum,
    IN GT_U32                   index
)
{
    GT_STATUS rc;
    GT_BOOL   block = GT_TRUE;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortFlush);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index));

    rc = internal_cpssTmPortFlush(devNum, index, block);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* cpssTmPortFlushTriggerActionStart
*
* DESCRIPTION:
*       Flush port by a given index. All packets dequeued from this port (and all its queues)
*       are dropped.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Flush port non blocking API.
*******************************************************************************/
GT_STATUS cpssTmPortFlushTriggerActionStart
(
    IN GT_U8                    devNum,
    IN GT_U32                   index
)
{
    GT_STATUS rc;
    GT_BOOL   block = GT_FALSE;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortFlushTriggerActionStart);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index));

    rc = internal_cpssTmPortFlush(devNum, index, block);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssTmPortFlushStatusGet
*
* DESCRIPTION:
*       Get whether Flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       actFinishedPtr  - GT_FALSE - flush is active (packets are dropped on dequeue).
*                       - GT_TRUE  - flush off /completed.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_BAD_PTR               - on NULL actFinishedPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmPortFlushStatusGet
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    OUT GT_BOOL                 *actFinishedPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    uint8_t     status;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actFinishedPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_flush_port_status(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, index, &status);
    if (actFinishedPtr)
        *actFinishedPtr = status ? GT_FALSE : GT_TRUE;
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}


/*******************************************************************************
* cpssTmPortFlushTriggerActionStatusGet
*
* DESCRIPTION:
*       Get whether Flush port has completed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       index             - Port index.
*
* OUTPUTS:
*       actFinishedPtr  - GT_FALSE - flush is active (packets are dropped on dequeue).
*                       - GT_TRUE  - flush off /completed.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or index value or if index not in use.
*       GT_BAD_PTR               - on NULL actFinishedPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmPortFlushTriggerActionStatusGet
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    OUT GT_BOOL                 *actFinishedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortFlushTriggerActionStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, actFinishedPtr));

    rc = internal_cpssTmPortFlushStatusGet(devNum, index, actFinishedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, actFinishedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmNodesCtlReadNextChange
*
* DESCRIPTION:
*       Read next tree index/range change after reshuffling.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       changePtr         - Change structure pointer.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_BAD_PTR               - on NULL changePtr.
*       GT_NO_RESOURCE           - on empty list.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmNodesCtlReadNextChange
(
    IN GT_U8                        devNum,
    OUT CPSS_TM_TREE_CHANGE_STC     *changePtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;
    struct tm_tree_change changePrms;

    CPSS_NULL_PTR_CHECK_MAC(changePtr);

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_nodes_read_next_change(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, &changePrms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    prvCpssTmTreeChangeCpy(&changePrms, changePtr);
    return rc;
}

/*******************************************************************************
* cpssTmNodesCtlReadNextChange
*
* DESCRIPTION:
*       Read next tree index/range change after reshuffling.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       changePtr         - Change structure pointer.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_BAD_PTR               - on NULL changePtr.
*       GT_NO_RESOURCE           - on empty list.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlReadNextChange
(
    IN GT_U8                        devNum,
    OUT CPSS_TM_TREE_CHANGE_STC     *changePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodesCtlReadNextChange);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, changePtr));

    rc = internal_cpssTmNodesCtlReadNextChange(devNum, changePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, changePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmNodesCtlCleanList
*
* DESCRIPTION:
*       Empty list of reshuffling changes.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmNodesCtlCleanList
(
    IN GT_U8                        devNum
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_clean_list(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

/*******************************************************************************
* cpssTmNodesCtlCleanList
*
* DESCRIPTION:
*       Empty list of reshuffling changes.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmNodesCtlCleanList
(
    IN GT_U8                        devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodesCtlCleanList);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssTmNodesCtlCleanList(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

