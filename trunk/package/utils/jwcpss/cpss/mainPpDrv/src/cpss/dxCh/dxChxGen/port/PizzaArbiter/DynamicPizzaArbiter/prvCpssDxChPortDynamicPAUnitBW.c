/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* * prvCpssDxChPortDynamicPAUnitBW.c
*
* DESCRIPTION:
*       bobcat2 and higher dynamic (algorithmic) pizza arbiter
*
* FILE REVISION NUMBER:
*       $Revision: 4$
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitBW.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAPortSpeedDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiterWS.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>

/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfUnitBWSet
*
* DESCRIPTION:
*       set pipe BW for specific unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       unit     - unit type
*       bwInGBit - bandwidth in GBits
*
* OUTPUTS:
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfUnitBWSet
(
    IN GT_U8                     devNum,
    IN CPSS_DXCH_PA_UNIT_ENT     unit,
    IN GT_U32                    bwInGBit
)
{
    GT_U32 * unitBWPtr;
    GT_32   unitN;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    unitN = sizeof(PRV_CPSS_PP_MAC(devNum)->paUnitPipeBWInGBitArr)/sizeof(PRV_CPSS_PP_MAC(devNum)->paUnitPipeBWInGBitArr[0]);
    if (0 <= unit && unit < unitN )
    {
        unitBWPtr = &PRV_CPSS_PP_MAC(devNum)->paUnitPipeBWInGBitArr[0];
        unitBWPtr[unit] = bwInGBit;
        return GT_OK;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}


/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfUnitBWGet
*
* DESCRIPTION:
*       Get pipe BW for specific unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       unit     - unit type
*
* OUTPUTS:
*       bwInGBitPtr - bandwidth in GBits
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on wrong ptr
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfUnitBWGet
(
    IN  GT_U8                     devNum,
    IN  CPSS_DXCH_PA_UNIT_ENT     unit,
    OUT GT_U32                   *bwInGBitPtr
)
{
    GT_U32 * unitBWPtr;
    GT_32 unitN;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(bwInGBitPtr);

    unitN = sizeof(PRV_CPSS_PP_MAC(devNum)->paUnitPipeBWInGBitArr)/sizeof(PRV_CPSS_PP_MAC(devNum)->paUnitPipeBWInGBitArr[0]);
    if (0 <= unit && unit < unitN)
    {
        unitBWPtr = &PRV_CPSS_PP_MAC(devNum)->paUnitPipeBWInGBitArr[0];
        * bwInGBitPtr = unitBWPtr[unit];
        return GT_OK;
    }
    else
    {
        * bwInGBitPtr = 0;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}



/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfUnitBWInit
*
* DESCRIPTION:
*       Inti pipe BW for all units depending on device id
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfUnitBWInit
(
    GT_U8 devNum
)
{
    GT_STATUS rc;
    GT_U32 unitBwInGbpsArr[CPSS_DXCH_PA_UNIT_MAX_E];
    CPSS_DXCH_PA_UNIT_ENT unitType;
    GT_U32                unitIdx;
    GT_U32 unitBwInGbps;
    PRV_CPSS_DXCH_PP_CONFIG_STC *pDev;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC *paWsPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* copy pipe BW capacity from fine tuning params */
    unitBwInGbps = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.pipeBWCapacityInGbps;

    if (unitBwInGbps == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }

    /* Init BW */
    for (unitType = (CPSS_DXCH_PA_UNIT_ENT)0; unitType < CPSS_DXCH_PA_UNIT_MAX_E; unitType++)
    {
        unitBwInGbpsArr[unitType] = 0;
    }
    /*--------------------------------------*
     * fill units controlled by pipeBWSet() *
     *--------------------------------------*/
    for (unitIdx = 0 ; paWsPtr->prv_unitListConfigByPipeBwSetPtr[unitIdx] != CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
    {
        unitType = paWsPtr->prv_unitListConfigByPipeBwSetPtr[unitIdx];
        unitBwInGbpsArr[unitType] = unitBwInGbps;
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    /*----------------------------------------------------*
     * fill units that are not contorlled by  pipeBWSet() *
     *----------------------------------------------------*/
    pDev = PRV_CPSS_DXCH_PP_MAC(devNum);
    switch (pDev->genInfo.devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            switch (pDev->genInfo.devSubFamily)
            {
                case CPSS_PP_SUB_FAMILY_NONE_E:
                break;
                case CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E:
#ifndef GM_USED
                        switch (pDev->genInfo.devType)
                        {
                            case CPSS_BOBK_CETUS_DEVICES_CASES_MAC:
                                unitBwInGbpsArr[CPSS_DXCH_PA_UNIT_MPPM_E          ] = (GT_U32)(~0);
                            break;
                            case CPSS_BOBK_CAELUM_DEVICES_CASES_MAC:
                                unitBwInGbpsArr[CPSS_DXCH_PA_UNIT_RXDMA_E         ] = 48;
                                unitBwInGbpsArr[CPSS_DXCH_PA_UNIT_TXDMA_E         ] = 48;
                                unitBwInGbpsArr[CPSS_DXCH_PA_UNIT_TX_FIFO_E       ] = 48;
                                unitBwInGbpsArr[CPSS_DXCH_PA_UNIT_ETH_TX_FIFO_E   ] = 48;
                                unitBwInGbpsArr[CPSS_DXCH_PA_UNIT_MPPM_E          ] = (GT_U32)(~0);
                            break;
                            default:
                            {
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                            }
                        }
#endif
                break;
                default:
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }
            }
        break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
        break;
        default:
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        break;
    }

    for (unitType = (CPSS_DXCH_PA_UNIT_ENT)0; unitType < CPSS_DXCH_PA_UNIT_MAX_E; unitType++)
    {
        rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWSet(devNum, unitType, unitBwInGbpsArr[unitType]);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    PRV_CPSS_PP_MAC(devNum)->paMinSliceResolutionMbps = 1000;  /* 1G */
    return GT_OK;
}


