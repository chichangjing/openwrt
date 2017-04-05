/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortDynamicPizzaArbiterWorkConserving.c
*
* DESCRIPTION:
*       bobcat2 and higher dynamic (algorithmic) pizza arbiter
*
* FILE REVISION NUMBER:
*       $Revision: 93 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitDrv.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPizzaArbiterWorkConserving.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiterWS.h>


GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet
(
    IN GT_U8                 devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unit,
    IN GT_BOOL               status
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 unitIdx;
    CPSS_DXCH_PA_UNIT_ENT unitType;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC *paWsPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_NONE_E && PRV_CPSS_PP_MAC(devNum)->revision == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }


    if (unit == CPSS_DXCH_PA_UNIT_UNDEFINED_E) /* for all */
    {

        for (unitIdx = 0 ; paWsPtr->prv_DeviceUnitListPtr[unitIdx] !=  CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
        {
            unitType = paWsPtr->prv_DeviceUnitListPtr[unitIdx];

            rc = bobcat2PizzaArbiterUnitDrvWorkConservingModeSet(devNum,unitType,status);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    else
    {
        if (unit < CPSS_DXCH_PA_UNIT_RXDMA_E || unit >= CPSS_DXCH_PA_UNIT_MAX_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (paWsPtr->prv_DeviceUnitListBmp[unit] != GT_TRUE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        rc = bobcat2PizzaArbiterUnitDrvWorkConservingModeSet(devNum,unit,status);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if (unit == CPSS_DXCH_PA_UNIT_TXQ_E || unit == CPSS_DXCH_PA_UNIT_UNDEFINED_E)
    {
        GT_U32 txqPort;
        for (txqPort = 0 ; txqPort < PRV_CPSS_DXCH_BC2B0_PORT_WORK_PORTN_CNS; txqPort++)
        {
            rc = prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeSet(devNum,txqPort,status);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet
*
* DESCRIPTION:
*       Configure work conserving mode at one or all units
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       unit     - RxDMA/TxDMA/TXQ/Tx-FIFO/Eth-Tx-FIFO/Ilkn-Tx-FIFO or CPSS_DXCH_PA_UNIT_UNDEFINED_E (meaning all !!)
*       status   - enable (1) disable (0)
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
GT_STATUS cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet
(
    IN GT_U8                 devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unit,
    IN GT_BOOL               status
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, unit, status));

    rc = prvCpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet(devNum, unit, status);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, unit, status));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


static GT_STATUS internal_cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeGet
(
    IN  GT_U8                 devNum,
    IN  CPSS_DXCH_PA_UNIT_ENT unit,
    OUT GT_BOOL              *statusPtr
)
{
    GT_STATUS   rc;         /* return code */
    PRV_CPSS_DXCH_PA_WORKSPACE_STC *paWsPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_NONE_E && PRV_CPSS_PP_MAC(devNum)->revision == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    if (unit <= CPSS_DXCH_PA_UNIT_UNDEFINED_E || unit >= CPSS_DXCH_PA_UNIT_MAX_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (paWsPtr->prv_DeviceUnitListBmp[unit] != GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc = bobcat2PizzaArbiterUnitDrvWorkConservingModeGet(devNum,unit,/*OUT*/statusPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeGet
*
* DESCRIPTION:
*       get work conserving mode of one units
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       unit     - RxDMA/TxDMA/TXQ/Tx-FIFO/Eth-Tx-FIFO/Ilkn-Tx-FIFO
*
* OUTPUTS:
*       statusPtr - pointer to status.: enable (1) disable (0)
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
GT_STATUS cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeGet
(
    IN  GT_U8                 devNum,
    IN  CPSS_DXCH_PA_UNIT_ENT unit,
    OUT GT_BOOL              *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, unit, statusPtr));

    rc = internal_cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeGet(devNum, unit, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, unit, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


