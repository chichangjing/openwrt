/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPizzaArbiter.c
*
* DESCRIPTION:
*       CPSS implementation for SerDes configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 70 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/private/utils/prvCpssTimeRtUtils.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortTablePizzaArbiter.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>



extern GT_STATUS prvCpssDxChPortTablePizzaArbiterIfInit
(
    IN  GT_U8    devNum
);
extern GT_STATUS prvCpssDxChPortTablePizzaArbiterIfCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
);
extern GT_STATUS prvCpssDxChPortTablePizzaArbiterIfConfigure
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
);

extern GT_STATUS prvCpssDxChPortTablePizzaArbiterIfDevStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    OUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
);



GT_BOOL pa_TimeTake = GT_FALSE;
GT_U32   prv_paTime_ms;  
GT_U32   prv_paTime_us;
    
GT_STATUS prvCpssDxChPortPATimeTakeEnable
(
    GT_VOID
)
{
    pa_TimeTake = GT_TRUE;
    return GT_OK;
}

GT_STATUS prvCpssDxChPortPATimeTakeDisable
(
    GT_VOID
)
{
    pa_TimeTake = GT_FALSE;
    return GT_OK;
}


GT_STATUS prvCpssDxChPortPATimeDiffGet
(
    OUT GT_U32 *prv_paTime_msPtr, 
    OUT GT_U32 *prv_paTime_usPtr
)
{
    if (pa_TimeTake == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    *prv_paTime_msPtr = prv_paTime_ms;
    *prv_paTime_usPtr = prv_paTime_us;
    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfInit
*
* DESCRIPTION:
*       Pizza arbiter initialization in all Units where it's present
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       I forced to place this function here, because it needs number of port
*       group where CPU port is connected and there is just no more suitable
*       place.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfInit
(
    IN  GT_U8                   devNum
)
{
    GT_STATUS   rc;                 /* return code */
    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; /* pionter to device to be processed*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);


    devPtr = PRV_CPSS_PP_MAC(devNum);

    if (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E )
    {
        if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E)
        {
            rc = prvCpssDxChPortTablePizzaArbiterIfInit(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        return GT_OK;
    }
    else if (PRV_CPSS_SIP_5_CHECK_MAC(devNum)) /* &&      CPSS_PP_FAMILY_DXCH_BOBCAT2_E, */
    {
        rc = prvCpssDxChPortDynamicPizzaArbiterIfInit(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfCheckSupport
*
* DESCRIPTION:
*       Check whether Pizza Arbiter can be configured to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portSpeed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
)
{
    GT_STATUS   rc;                 /* return code */
    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; /* pionter to device to be processed*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);


    devPtr = PRV_CPSS_PP_MAC(devNum);

    if (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) /* &&      CPSS_PP_FAMILY_DXCH_BOBCAT2_E, */
    {
        rc = prvCpssDxChPortTablePizzaArbiterIfCheckSupport(devNum,portNum, portSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    if (devPtr->devFamily >= CPSS_PP_FAMILY_DXCH_BOBCAT2_E) /* &&      CPSS_PP_FAMILY_DXCH_BOBCAT2_E, */
    {
        GT_U32 portSpeedInMBit;

        rc = prvCpssDxChPortDynamicPizzaArbiterSpeedConv(devNum,portNum,portSpeed,/*OUT*/&portSpeedInMBit);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortDynamicPizzaArbiterIfCheckSupport(devNum,portNum, portSpeedInMBit);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);;
}




/*******************************************************************************
* prvCpssDxChPortIfPizzaArbiterConfigure
*
* DESCRIPTION:
*       Configure Pizza Arbiter accoringly to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portSpeed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfConfigure
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
)
{
    GT_STATUS   rc;                 /* return code */
    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; /* pionter to device to be processed*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    devPtr = PRV_CPSS_PP_MAC(devNum);

    if (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) /* &&      CPSS_PP_FAMILY_DXCH_BOBCAT2_E, */
    {
        rc = prvCpssDxChPortTablePizzaArbiterIfConfigure(devNum,portNum, portSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    if (devPtr->devFamily >= CPSS_PP_FAMILY_DXCH_BOBCAT2_E) /* &&      CPSS_PP_FAMILY_DXCH_BOBCAT2_E, */
    {
        GT_U32 portSpeedInMBit;
        GT_TIME timeStart = {0,0};

        if (pa_TimeTake == GT_TRUE)
        {
            rc = prvCpssOsTimeRTns(&timeStart);
            if (rc != GT_OK)
            {
                return rc;
            }
        }


        rc = prvCpssDxChPortDynamicPizzaArbiterSpeedConv(devNum,portNum,portSpeed,/*OUT*/&portSpeedInMBit);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortDynamicPizzaArbiterIfConfigure(devNum,portNum, portSpeedInMBit);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (pa_TimeTake)
        {
            prvCpssOsTimeRTDiff(timeStart, /*OUT*/&prv_paTime_ms, &prv_paTime_us);
        }                                

        return GT_OK;
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfDelete
*
* DESCRIPTION:
*       Delete Pizza Arbiter
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfDelete
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum
)
{
    GT_STATUS   rc;                 /* return code */
    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; /* pionter to device to be processed*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    devPtr = PRV_CPSS_PP_MAC(devNum);

    if (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        rc = prvCpssDxChPortTablePizzaArbiterIfDelete(devNum, portNum);
    }
    else if (devPtr->devFamily >= CPSS_PP_FAMILY_DXCH_BOBCAT2_E) /* &&      CPSS_PP_FAMILY_DXCH_BOBCAT2_E, */
    {
        rc = prvCpssDxChPortDynamicPizzaArbiterIfConfigure(devNum,portNum, 0);
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortPizzaArbiterDevStateInit
*
* DESCRIPTION:
*       Init the structure CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* OUTPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on pizzaDeviceStatePtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPizzaArbiterDevStateInit
(
    INOUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
)
{
    if (NULL == pizzaDeviceStatePtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    cpssOsMemSet(pizzaDeviceStatePtr,0,sizeof(*pizzaDeviceStatePtr));
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterDevStateInit
*
* DESCRIPTION:
*       Init the structure CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* OUTPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on pizzaDeviceStatePtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterDevStateInit
(
    INOUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPizzaArbiterDevStateInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, pizzaDeviceStatePtr));

    rc = internal_cpssDxChPortPizzaArbiterDevStateInit(pizzaDeviceStatePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, pizzaDeviceStatePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortPizzaArbiterDevStateGet
*
* DESCRIPTION:
*       Lion 2,3 Pizza Arbiter State which includes state of all
*         -
*         - state of slices
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*
* OUTPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPizzaArbiterDevStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    OUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
)
{
    GT_STATUS   rc;                 /* return code */
    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; /* pionter to device to be processed*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(pizzaDeviceStatePtr);

    rc = cpssDxChPortPizzaArbiterDevStateInit(pizzaDeviceStatePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    devPtr = PRV_CPSS_PP_MAC(devNum);

    if (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) /* &&      CPSS_PP_FAMILY_DXCH_BOBCAT2_E, */
    {
        rc = prvCpssDxChPortTablePizzaArbiterIfDevStateGet(devNum,portGroupId, pizzaDeviceStatePtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    if (devPtr->devFamily >= CPSS_PP_FAMILY_DXCH_BOBCAT2_E) /* &&      CPSS_PP_FAMILY_DXCH_BOBCAT2_E, */
    {
        rc = prvCpssDxChPortDynamicPizzaArbiterIfDevStateGet(devNum,portGroupId, pizzaDeviceStatePtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);;
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterDevStateGet
*
* DESCRIPTION:
*       Lion 2,3 Pizza Arbiter State which includes state of all
*         -
*         - state of slices
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*
* OUTPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterDevStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    OUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPizzaArbiterDevStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, pizzaDeviceStatePtr));

    rc = internal_cpssDxChPortPizzaArbiterDevStateGet(devNum, portGroupId, pizzaDeviceStatePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupId, pizzaDeviceStatePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



