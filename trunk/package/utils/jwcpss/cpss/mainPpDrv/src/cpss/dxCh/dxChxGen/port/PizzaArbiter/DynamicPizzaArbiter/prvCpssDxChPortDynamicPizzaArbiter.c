/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPizzaArbiter.c
*
* DESCRIPTION:
*       bobcat2 and higher dynamic (algorithmic) pizza arbiter
*
* FILE REVISION NUMBER:
*       $Revision: 93 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPATypeDef.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitDrv.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAPortSpeedDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAGCDAlgo.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiterWS.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

#ifdef WIN32
    #define PIZZA_DEBUG_PRINT_DUMP         0
    #define __DYNAMIC_PIZZA_ARBITER_TEST   0
#else
    #define PIZZA_DEBUG_PRINT_DUMP         0
    #define __DYNAMIC_PIZZA_ARBITER_TEST   0
#endif

#if (PIZZA_DEBUG_PRINT_DUMP == 1)

    extern GT_STATUS gtBobcat2PortPizzaArbiterIfStateDump(/*IN*/GT_U8  devNum,
                                                         /*IN*/GT_U32 portGroupId);
    int g_printPizzaDump = 1;
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet
(
    IN GT_U8                 devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unit,
    IN GT_BOOL               status
);
#ifdef __cplusplus
}
#endif /* __cplusplus */


GT_STATUS prvCpssDxChPortDynamicPizzaArbiterSpeedConv
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speedEnm,
    OUT GT_U32              *speedPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC *paWsPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    /* PRV_CPSS_GEN_PP_CONFIG_STC *devPtr = PRV_CPSS_PP_MAC(devNum); */
    /* GT_STATUS rc; */
    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }


    devNum  = devNum;
    portNum = portNum;
    if (speedPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (speedEnm == CPSS_PORT_SPEED_NA_E)
    {
        *speedPtr = 0;
        return GT_OK;
    }

    *speedPtr = paWsPtr->prv_speedEnt2MBitConvArr[speedEnm];
    if (*speedPtr == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}






static GT_STATUS prvCpssDxChPortDynamicPizzaArbiterCPUPort2PortDBAdd
(
    GT_U8 devNum,
    GT_U32 speed
)
{
    GT_STATUS rc;
    GT_U32 maxPortNum;
    GT_PHYSICAL_PORT_NUM portNum;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    GT_BOOL                      isCPUInitilized;
    GT_U32                       CPUspeedInMBit;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    maxPortNum = PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum);


    for (portNum = 0 ; portNum < maxPortNum; portNum++)
    {
        /* find local physical port number of CPU port */
        rc =  prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portNum, /*OUT*/&portMapShadowPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (GT_TRUE == portMapShadowPtr->valid)
        {
            if (portMapShadowPtr->portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E)
            {
                rc = prvCpssDxChPortDynamicPAPortSpeedDBGet(devNum,portNum,/*OUT*/&isCPUInitilized,/*OUT*/&CPUspeedInMBit);
                if (rc != GT_OK)
                {
                    return rc;
                }

                if (isCPUInitilized == GT_FALSE)
                {
                    rc = prvCpssDxChPortDynamicPAPortSpeedDBSet(devNum,portNum,speed);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    return GT_OK;
                }
            }
        }
    }
    return GT_OK;
}



static GT_FLOAT64                                 prv_clientSpeedConfigArr         [CPSS_DXCH_PA_UNIT_MAX_E][PRV_CPSS_MAX_PP_PORTS_NUM_CNS];
static GT_U32                                     prv_sliceNum2Config              [CPSS_DXCH_PA_UNIT_MAX_E];
static GT_PORT_NUM                                prv_pizzaArray                   [CPSS_DXCH_PA_UNIT_MAX_E][PRV_CPSS_DXCH_DYNAMIC_PA_SLICE_NUM_CNS];
static GT_U32                                     prv_highSpeedPortNumber          [CPSS_DXCH_PA_UNIT_MAX_E];
static GT_U32                                     prv_highSpeedPortArr             [CPSS_DXCH_PA_UNIT_MAX_E][CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS];

/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfInit
*
* DESCRIPTION:
*       Pizza arbiter initialization in all Units where it's present
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfInit
(
    IN  GT_U8                   devNum
)
{
    GT_STATUS   rc;                 /* return code */
    GT_U32      unitIdx;
    CPSS_DXCH_PA_UNIT_ENT unitType;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC *paWsPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChPortDynamicPAPortSpeedDBInit(devNum);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = prvCpssDxChPortDynamicPizzaArbiterWSInit(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortDynamicPAClientBWListInit(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*--------------*/
    /* set units BW */
    /*--------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWInit(devNum);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

   
    for (unitIdx = 0 ; paWsPtr->prv_DeviceUnitListPtr[unitIdx] != CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
    {
        unitType = paWsPtr->prv_DeviceUnitListPtr[unitIdx];
        if(paWsPtr->prv_unit2PizzaAlgoFunArr[unitType] == NULL)
        {
            continue;
        }

        rc = bobcat2PizzaArbiterUnitDrvInit(devNum,unitType);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /*-------------------------------------------------------------------------*/
    /*   Add to port-DB CPU port (if mapped !!!) with speed of 1G              */
    /*   mapping is called at end of phase 1                                   */
    /*   Pizza arbiter is initialized at and of phase 2                        */
    /*   therefore it is already known whether CPU port is mapped ort not      */
    /*-------------------------------------------------------------------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterCPUPort2PortDBAdd(devNum,1*PRV_CPSS_DXCH_PA_BW_COEFF);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*--------------------------------------------------------------*
     * set work conserving mode for TxQ unit                        *
     *     cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet() *
     *--------------------------------------------------------------*/
    if (paWsPtr->workConservingModeOnUnitListPtr != NULL)
    {
        for ( unitIdx = 0 ; paWsPtr->workConservingModeOnUnitListPtr[unitIdx] !=  CPSS_DXCH_PA_UNIT_UNDEFINED_E ; unitIdx++)
        {
            rc = prvCpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet(devNum,paWsPtr->workConservingModeOnUnitListPtr[unitIdx],GT_TRUE);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
    }

    #if (PIZZA_DEBUG_PRINT_DUMP == 1)
        if (g_printPizzaDump == 1)
        {
            cpssOsPrintf("\nInit dev %2d",devNum);
            rc = gtBobcat2PortPizzaArbiterIfStateDump(devNum,0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

    #endif

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfTMBWSet
*
* DESCRIPTION:
*       define BW of TM units
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfTMBWSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  bwMbps
)
{
    GT_STATUS rc;
    OUT PRV_CPSS_DXCH_PA_WORKSPACE_STC *paWsPtr;
    GT_U32 i;

    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0 ; paWsPtr->tmUnitClientListPtr[i].unit != CPSS_DXCH_PA_UNIT_UNDEFINED_E; i++)
    {
        rc = prvCpssDxChPortDynamicPAClientBWListUpdate(devNum,
                                                        paWsPtr->tmUnitClientListPtr[i].unit,
                                                        paWsPtr->tmUnitClientListPtr[i].clientId,bwMbps);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfCheckSupport
*
* DESCRIPTION:
*       Check whether Pizza Arbiter can be configured to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit
)
{
    GT_STATUS rc;   /* return code */
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    CPSS_DXCH_PA_UNIT_ENT             *configuredUnitListPtr;
    GT_U32                             paMinSliceResolutionMbps;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr;



    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portNum, /*OUT*/&portMapShadowPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (portMapShadowPtr->valid == GT_FALSE) /* port is not mapped, cann't configure it */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }


    configuredUnitListPtr = paWsPtr->prv_mappingType2UnitConfArr[portMapShadowPtr->portMap.mappingType][portMapShadowPtr->portMap.trafficManagerEn];
    if (NULL == configuredUnitListPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    paMinSliceResolutionMbps = PRV_CPSS_PP_MAC(devNum)->paMinSliceResolutionMbps;
    if (portSpeedInMBit > 0) /* 0 for delete operation, minSliceResolution is irrelevant here */
    {
        if (portSpeedInMBit < paMinSliceResolutionMbps)
        {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (portSpeedInMBit % paMinSliceResolutionMbps != 0) /* shall be compain to resolution */
        {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;
}



/*----------------------------------------------------------------*/
/* Build configuration according to stored DB(old configuration)  */
/*----------------------------------------------------------------*/

static GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfBuildConfigByStoredDB
(
    IN  PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr,
    IN  GT_U8                              devNum,
    IN  GT_PHYSICAL_PORT_NUM               newPortNum,
    IN  GT_U32                             portSpeedInMBit,
    OUT GT_FLOAT64                         clientSpeedConfigArr[CPSS_DXCH_PA_UNIT_MAX_E][PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
)
{
    GT_STATUS                   rc;
    GT_BOOL                     isPortConfigured;
    GT_U32                      speedEnt;
    GT_FLOAT64                  speedFloat;
    GT_FLOAT64                  txQspeedFloat;
    CPSS_DXCH_PA_UNIT_ENT *     configuredUnitListPtr;
    CPSS_DXCH_PA_UNIT_ENT       unitType;
    GT_U32                      unitIdx;
    GT_U32                      clientIdx;
    GT_PHYSICAL_PORT_NUM        maxPhysPortNum;
    GT_PHYSICAL_PORT_NUM        physPortNum;
    GT_U32                      specificPortNum;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    GT_U32                      i;
    GT_U32                      portArgArr[CPSS_DXCH_PA_UNIT_MAX_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    /*--------------------*
     *  Init output array *
     *--------------------*/
    for (unitType = (CPSS_DXCH_PA_UNIT_ENT)0; unitType < CPSS_DXCH_PA_UNIT_MAX_E; unitType++)
    {
        for (specificPortNum = 0 ; specificPortNum< PRV_CPSS_MAX_PP_PORTS_NUM_CNS ; specificPortNum++)
        {
            clientSpeedConfigArr[unitType][specificPortNum] = 0;
        }
    }


    maxPhysPortNum = PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum);

    for (physPortNum = 0 ; physPortNum < maxPhysPortNum; physPortNum++)
    {
        if (physPortNum != newPortNum)
        {
            rc = prvCpssDxChPortDynamicPAPortSpeedDBGet(devNum,physPortNum,/*OUT*/&isPortConfigured,/*OUT*/&speedEnt);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            speedEnt         = portSpeedInMBit;
            isPortConfigured = GT_TRUE;
        }
        if (GT_TRUE == isPortConfigured)
        {
            speedFloat    = (GT_FLOAT64)speedEnt;
            txQspeedFloat = speedFloat;

            rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,physPortNum,/*OUT*/&portMapShadowPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
            if (portMapShadowPtr->valid == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* according to Speed DB port is configured , but mapping is invalide , inconsistent data */
            }
            /* 30G WA */
            if (GT_TRUE == PRV_CPSS_DXCH_PP_MAC(devNum)->errata.info_PRV_CPSS_DXCH_BOBCAT2_REV_A0_40G_NOT_THROUGH_TM_IS_PA_30G_WA_E.enabled)
            {
                if (PRV_CPSS_DXCH_BOBCAT2_A0_CHECK_MAC(devNum))
                {
                    if (speedFloat == 40000)
                    {
                        if (portMapShadowPtr->portMap.trafficManagerEn == GT_FALSE)
                        {
                            txQspeedFloat = 30000;
                        }
                    }
                }
            }
            /*---------------------------------------------*
             * convert from mapping to array of units      *
             *---------------------------------------------*/
            rc = paWsPtr->mapping2unitConvFunPtr(devNum,portMapShadowPtr,portArgArr);
            if (rc != GT_OK)
            {
                return rc;
            }

            configuredUnitListPtr = paWsPtr->prv_mappingType2UnitConfArr[portMapShadowPtr->portMap.mappingType][portMapShadowPtr->portMap.trafficManagerEn];
            if (NULL == configuredUnitListPtr)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }


            for (unitIdx = 0 ;  configuredUnitListPtr[unitIdx] != CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
            {
                unitType        = configuredUnitListPtr[unitIdx];
                specificPortNum = portArgArr[unitType];
                if (specificPortNum < PRV_CPSS_MAX_PP_PORTS_NUM_CNS) /* is Valid ? */
                {
                    if (unitType != CPSS_DXCH_PA_UNIT_TXQ_E)
                    {
                        clientSpeedConfigArr[unitType][specificPortNum] += speedFloat;
                    }
                    else
                    {
                        clientSpeedConfigArr[unitType][specificPortNum] += txQspeedFloat;
                    }
                }
            }
            /*-------------------------------------*
             * compute client for MPPM units       *
             *-------------------------------------*/
            if (paWsPtr->mppmClientCodingListPtr != NULL)
            {
                for (unitIdx = 0; paWsPtr->mppmClientCodingListPtr[unitIdx].unit != CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
                {
                    unitType        = paWsPtr->mppmClientCodingListPtr[unitIdx].unit;
                    clientIdx       = paWsPtr->mppmClientCodingListPtr[unitIdx].mppmClientCode;
                    specificPortNum = portArgArr[unitType];
                    if (specificPortNum < PRV_CPSS_MAX_PP_PORTS_NUM_CNS)
                    {
                        clientSpeedConfigArr[CPSS_DXCH_PA_UNIT_MPPM_E][clientIdx] += speedFloat * paWsPtr->mppmClientCodingListPtr[unitIdx].clientWeight;
                    }
                }
            }
        }
    }
    
    /*-----------------------------------------------------------------------------------* 
     * now when all client BW is computed as sum, checked whether there is predefined BW *
     *-----------------------------------------------------------------------------------*/
    for (i = 0 ;  ; i++)
    {
        PRV_CPSS_DXCH_PA_SINGLE_CLIENT_BW_STC * ptr;
        
        ptr = prvCpssDxChPortDynamicPAClientBWListEntryByIdxGet(devNum,i);
        if (ptr == NULL)
        {
            break;
        }
        /* overwrite pre-computed value */
        clientSpeedConfigArr[ptr->unitType][ptr->clinetId] = ptr->bwMbps;
    }

    return GT_OK;
}


static GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfComputeUnitDistributions
(
    IN PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr,
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PA_UNIT_ENT              unitType
)
{
    GT_STATUS                          rc;
    GT_PHYSICAL_PORT_NUM               maxPhysPortNum;
    PRV_CPSS_DXCH_BUILD_PIZZA_DISTRBUTION_FUN algoDistribFun;
    GT_U32                             unitBWInGBit;
    GT_U32                             unitBWInMBit;
    GT_U32                             minSliceResolutionInMBps;



    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    maxPhysPortNum = PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum);

    minSliceResolutionInMBps = PRV_CPSS_PP_MAC(devNum)->paMinSliceResolutionMbps;
    algoDistribFun = paWsPtr->prv_unit2PizzaAlgoFunArr[unitType];
    if(NULL != algoDistribFun)
    {
        rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWGet(devNum,unitType, /*OUT*/&unitBWInGBit);
        if (rc != GT_OK)
        {
            return rc;
        }

        unitBWInMBit = unitBWInGBit*PRV_CPSS_DXCH_PA_BW_COEFF;
        rc = (paWsPtr->prv_unit2PizzaAlgoFunArr[unitType])(/*IN*/&prv_clientSpeedConfigArr[unitType][0],
                                                    /*IN*/ maxPhysPortNum,
                                                    /*IN*/ unitBWInMBit,
                                                    /*IN*/ minSliceResolutionInMBps,
                                                    /*IN*/ &(paWsPtr->prv_txQHighSpeedPortThreshParams),
                                                    /*IN*/ PRV_CPSS_DXCH_DYNAMIC_PA_SLICE_NUM_CNS,
                                                    /*OUT*/&prv_pizzaArray[unitType][0],
                                                    /*OUT*/&prv_sliceNum2Config[unitType],
                                                    /*OUT*/&prv_highSpeedPortNumber[unitType],
                                                    /*OUT*/&prv_highSpeedPortArr[unitType][0]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

static GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfConfigDistributionsInHW
(
    PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr,
    GT_U8                              devNum,
    CPSS_DXCH_PA_UNIT_ENT              unitType
)
{
    GT_STATUS                          rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if(NULL != paWsPtr->prv_unit2PizzaAlgoFunArr[unitType]) /* pizza was computed ? */
    {
        if (paWsPtr->prv_unit2HighSpeedPortConfFunArr[unitType] != NULL)
        {
            if (prv_highSpeedPortNumber[unitType] > CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS)
            {
                cpssOsPrintf("\n--> PA ERROR : currently TxQ support SINGLE High Speed Ports configuration");
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            rc = (paWsPtr->prv_unit2HighSpeedPortConfFunArr[unitType])(devNum,prv_highSpeedPortNumber[unitType],&prv_highSpeedPortArr[unitType][0]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        rc = bobcat2PizzaArbiterUnitDrvSet(devNum,unitType,prv_sliceNum2Config[unitType],&prv_pizzaArray[unitType][0]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfConfigure
*
* DESCRIPTION:
*       Configure Pizza Arbiter accoringly to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfConfigure
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit
)
{
    GT_STATUS rc;   /* return code */

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    CPSS_DXCH_PA_UNIT_ENT             *configuredUnitListPtr;
    CPSS_DXCH_PA_UNIT_ENT              unitType;
    GT_U32                             unitIdx;
    PRV_CPSS_GEN_PP_CONFIG_STC        *pDev;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);



    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portNum, /*OUT*/&portMapShadowPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (portMapShadowPtr->valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    pDev = PRV_CPSS_PP_MAC(devNum);
    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* check whether this type is supported */
    configuredUnitListPtr = paWsPtr->prv_mappingType2UnitConfArr[portMapShadowPtr->portMap.mappingType][portMapShadowPtr->portMap.trafficManagerEn];
    if (NULL == configuredUnitListPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }



    if (portSpeedInMBit > 0) /* 0 for delete operation, minSliceResolution is irrelevant here */
    {
        if (portSpeedInMBit < pDev->paMinSliceResolutionMbps)
        {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (portSpeedInMBit % pDev->paMinSliceResolutionMbps != 0) /* shall be compain to resolution */
        {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* Add CPU port to port-DB (if mapped !!!) with speed of 1G */
    rc = prvCpssDxChPortDynamicPizzaArbiterCPUPort2PortDBAdd(devNum,1*PRV_CPSS_DXCH_PA_BW_COEFF);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsMemSet(prv_pizzaArray,0,sizeof(prv_pizzaArray));
    /*----------------------------------------------------------------*/
    /* Build configuration according to stored DB(old configuration)  */
    /*----------------------------------------------------------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterIfBuildConfigByStoredDB(paWsPtr, devNum,portNum,portSpeedInMBit,/*OUT*/prv_clientSpeedConfigArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*----------------------------------*/
    /* now compute pizzas for all units */
    /*----------------------------------*/
    for (unitIdx = 0 ; paWsPtr->prv_DeviceUnitListPtr[unitIdx] != CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
    {
        unitType = paWsPtr->prv_DeviceUnitListPtr[unitIdx];
        rc = prvCpssDxChPortDynamicPizzaArbiterIfComputeUnitDistributions(paWsPtr,devNum, unitType);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /*----------------------------------------------------------------------*/
    /* now all data was prepared , configure HW (High Speed Ports and Pizza)*/
    /*----------------------------------------------------------------------*/
    for (unitIdx = 0 ; paWsPtr->prv_DeviceUnitListPtr[unitIdx] != CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
    {
        unitType = paWsPtr->prv_DeviceUnitListPtr[unitIdx];
        rc = prvCpssDxChPortDynamicPizzaArbiterIfConfigDistributionsInHW(paWsPtr,devNum,unitType);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    /*---------------------*/
    /* update DB           */
    /*---------------------*/
    rc = prvCpssDxChPortDynamicPAPortSpeedDBSet(devNum,portNum,portSpeedInMBit);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if (PIZZA_DEBUG_PRINT_DUMP == 1)
        if (g_printPizzaDump == 1)
        {
            cpssOsPrintf("\nConfigure dev %2d  port %2d speed = %5d",
                                devNum, portNum, portSpeedInMBit);
            printf("\nConfigure dev %2d  port %2d speed = %5d",
                                devNum, portNum, portSpeedInMBit);

            rc = gtBobcat2PortPizzaArbiterIfStateDump(devNum,0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    #endif
    return GT_OK;
}


/*----------------------------------------------------------------*/
/* Build configuration according to stored DB(old configuration)  */
/*----------------------------------------------------------------*/
static GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelBuildConfigByStoredDB
(
    IN  GT_U8       devNum,
    OUT GT_FLOAT64  clientSpeedConfigArr[PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
)
{
    GT_STATUS                   rc;
    GT_BOOL                     isPortConfigured;
    GT_U32                      speedEnt;
    GT_FLOAT64                  speedFloat;
    GT_PHYSICAL_PORT_NUM        maxPhysPortNum;
    GT_PHYSICAL_PORT_NUM        physPortNum;
    GT_U32                      specificChannelNum;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    /* Init output array */
    for (specificChannelNum = 0 ; specificChannelNum< PRV_CPSS_MAX_PP_PORTS_NUM_CNS ; specificChannelNum++)
    {
        clientSpeedConfigArr[specificChannelNum] = 0;
    }

    maxPhysPortNum = PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum);

    for (physPortNum = 0 ; physPortNum < maxPhysPortNum; physPortNum++)
    {
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,physPortNum,/*OUT*/&portMapShadowPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (GT_TRUE == portMapShadowPtr->valid)
        {
            rc = prvCpssDxChPortDynamicPAIlknChannelSpeedDBGet(devNum,physPortNum,/*OUT*/&isPortConfigured,/*OUT*/&speedEnt);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (GT_TRUE == isPortConfigured && portMapShadowPtr->portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E)
            {
                speedFloat = (GT_FLOAT64)speedEnt;
                clientSpeedConfigArr[portMapShadowPtr->portMap.ilknChannel] += speedFloat;
            }
        }
    }

    return GT_OK;
}


/*-------------------------------------------------------*/
/* now input portNum is configured by old configuration  */
/* correct it ports' speed on delta of new configuration */
/*-------------------------------------------------------*/
static GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelCorrectStoredConfigForSpecificPort
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit,
    INOUT GT_FLOAT64            clientSpeedConfigArr[PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
)
{
    GT_STATUS                   rc;
    GT_BOOL                     isPortConfigured;
    GT_U32                      speedEnt;
    GT_FLOAT64                  speedFloat;
    GT_FLOAT64                  entrySpeedFloat;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    GT_U32                      specificChannelNum;
    GT_U32                      speedResolutionMBbps;
    GT_U32                      unitBWInGBit;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    entrySpeedFloat = (GT_FLOAT64)portSpeedInMBit;

    rc = prvCpssDxChPortDynamicPAIlknChannelSpeedDBGet(devNum,portNum,/*OUT*/&isPortConfigured,/*OUT*/&speedEnt);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (GT_TRUE == isPortConfigured)   /* correct speed on already configured value */
    {

        speedFloat = entrySpeedFloat - (GT_FLOAT64)speedEnt;
    }
    else
    {
        speedFloat = entrySpeedFloat;
    }

    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portNum,/*OUT*/&portMapShadowPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    clientSpeedConfigArr[portMapShadowPtr->portMap.ilknChannel] += speedFloat;

    /* PATCH : check whether speed resolution is greater than 40M : shall be chanhged in near future */
    rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWGet(devNum,CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E, /*OUT*/&unitBWInGBit);
    if (rc != GT_OK)
    {
        return rc;
    }
    speedResolutionMBbps = unitBWInGBit*PRV_CPSS_DXCH_PA_BW_COEFF;
    for (specificChannelNum = 0 ; specificChannelNum< PRV_CPSS_MAX_PP_PORTS_NUM_CNS ; specificChannelNum++)
    {
        if (clientSpeedConfigArr[specificChannelNum] > 0)
        {
            speedResolutionMBbps = prvCpssDxChDynPAGCDAlgo((GT_U32)clientSpeedConfigArr[specificChannelNum],speedResolutionMBbps);
            if (speedResolutionMBbps < 40)
            {
                cpssOsPrintf("\nERROR : speed resolution smaller than 40MBps : channel %d",specificChannelNum);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }

    return GT_OK;
}



GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit
)
{
    GT_STATUS rc;   /* return code */

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_ilkn.supported == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portNum, /*OUT*/&portMapShadowPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (portMapShadowPtr->valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    cpssOsMemSet(prv_pizzaArray,0,sizeof(prv_pizzaArray));
    /*----------------------------------------------------------------*/
    /* Build configuration according to stored DB(old configuration)  */
    /*----------------------------------------------------------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelBuildConfigByStoredDB(devNum,/*OUT*/prv_clientSpeedConfigArr[CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*-------------------------------------------------------*/
    /* now input portNum is configured by old configuration  */
    /* correct it ports' speed on delta of new configuration */
    /*-------------------------------------------------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelCorrectStoredConfigForSpecificPort(devNum, portNum,portSpeedInMBit,
                                                                                           /*OUT*/prv_clientSpeedConfigArr[CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E]);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelConfigure
*
* DESCRIPTION:
*       Configure Pizza Arbiter accoringly to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelConfigure
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit
)
{
    GT_STATUS rc;   /* return code */
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    /* PRV_CPSS_GEN_PP_CONFIG_STC        *pDev; */
    PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_ilkn.supported == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portNum, /*OUT*/&portMapShadowPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (portMapShadowPtr->valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }


    cpssOsMemSet(prv_pizzaArray,0,sizeof(prv_pizzaArray));
    /*----------------------------------------------------------------*/
    /* Build configuration according to stored DB(old configuration)  */
    /*----------------------------------------------------------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelBuildConfigByStoredDB(devNum,/*OUT*/prv_clientSpeedConfigArr[CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*-------------------------------------------------------*/
    /* now input portNum is configured by old configuration  */
    /* correct it ports' speed on delta of new configuration */
    /*-------------------------------------------------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelCorrectStoredConfigForSpecificPort(devNum, portNum,portSpeedInMBit,
                                                                                           /*OUT*/prv_clientSpeedConfigArr[CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*----------------------------------*/
    /* now compute pizzas for all units */
    /*----------------------------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterIfComputeUnitDistributions(paWsPtr,devNum,CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*----------------------------------------------------------------------*/
    /* now all data was prepared , configure HW (High Speed Ports and Pizza)*/
    /*----------------------------------------------------------------------*/
    rc = prvCpssDxChPortDynamicPizzaArbiterIfConfigDistributionsInHW(paWsPtr,devNum,CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E);
    if (rc != GT_OK)
    {
        return rc;
    }
    /*---------------------*/
    /* update DB           */
    /*---------------------*/
    rc = prvCpssDxChPortDynamicPAIlknChannelSpeedDBSet(devNum,portNum,portSpeedInMBit);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if (PIZZA_DEBUG_PRINT_DUMP == 1)
        if (g_printPizzaDump == 1)
        {
            cpssOsPrintf("\nConfigure dev %2d  port %2d speed = %5d",
                                devNum, portNum, portSpeedInMBit);
            printf("\nConfigure dev %2d  port %2d speed = %5d",
                                devNum, portNum, portSpeedInMBit);

            rc = gtBobcat2PortPizzaArbiterIfStateDump(devNum,0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    #endif
    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortPizzaArbiterDevStateGet
*
* DESCRIPTION:
*       Bobcat2, Caelum, Bobcat3 Pizza Arbiter State which includes state of all
*         -
*         - state of slices
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Lion2; xCat2.
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfDevStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    OUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
)
{
    GT_STATUS   rc;         /* return code */
    CPSS_DXCH_PA_UNIT_ENT unitType;
    GT_U32 unitIdx;
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC * unitStatePtrArr[CPSS_DXCH_PA_UNIT_MAX_E];

    PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr;



    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; /* pionter to device to be processed*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pizzaDeviceStatePtr);


    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }


    devPtr = PRV_CPSS_PP_MAC(devNum);
	pizzaDeviceStatePtr->devType = devPtr->devFamily;
    /* old interface , shall be disappear after lua will changed to a new interface, just for BC2 !!! */
    if (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E && devPtr->devSubFamily == CPSS_PP_SUB_FAMILY_NONE_E)
    {
        portGroupId = portGroupId;

        unitStatePtrArr[CPSS_DXCH_PA_UNIT_RXDMA_E       ] = &pizzaDeviceStatePtr->devState.bc2.rxDMA;
        unitStatePtrArr[CPSS_DXCH_PA_UNIT_TXDMA_E       ] = &pizzaDeviceStatePtr->devState.bc2.txDMA;
        unitStatePtrArr[CPSS_DXCH_PA_UNIT_TXQ_E         ] = &pizzaDeviceStatePtr->devState.bc2.TxQ;
        unitStatePtrArr[CPSS_DXCH_PA_UNIT_TX_FIFO_E     ] = &pizzaDeviceStatePtr->devState.bc2.txFIFO;
        unitStatePtrArr[CPSS_DXCH_PA_UNIT_ETH_TX_FIFO_E ] = &pizzaDeviceStatePtr->devState.bc2.ethFxFIFO;
        unitStatePtrArr[CPSS_DXCH_PA_UNIT_ILKN_TX_FIFO_E] = &pizzaDeviceStatePtr->devState.bc2.ilknTxFIFO;

        

        for (unitType = (CPSS_DXCH_PA_UNIT_ENT)0 ;  unitType < CPSS_DXCH_PA_UNIT_MAX_E; unitType++)
        {
            if(paWsPtr->prv_unit2PizzaAlgoFunArr[unitType] == NULL)
            {
                continue;
            }
            rc = bobcat2PizzaArbiterUnitDrvGet(devNum,unitType,/*OUT*/unitStatePtrArr[unitType]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    /*----------------------------------
     * copy Unit List 
     *----------------------------------*/
    for (unitIdx = 0 ; unitIdx < sizeof(pizzaDeviceStatePtr->devState.bobK.unitList)/sizeof(pizzaDeviceStatePtr->devState.bobK.unitList[0]); unitIdx++)
    {
        pizzaDeviceStatePtr->devState.bobK.unitList[unitIdx] = CPSS_DXCH_PA_UNIT_UNDEFINED_E;
    }
    for (unitIdx = 0 ; paWsPtr->prv_DeviceUnitListPtr[unitIdx] != CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
    {
        unitType = paWsPtr->prv_DeviceUnitListPtr[unitIdx];
        pizzaDeviceStatePtr->devState.bobK.unitList[unitIdx] = unitType;

        rc = bobcat2PizzaArbiterUnitDrvGet(devNum,unitType,/*OUT*/&pizzaDeviceStatePtr->devState.bobK.unitState[unitIdx]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}



#if (__DYNAMIC_PIZZA_ARBITER_TEST == 1)

    typedef struct
    {
        GT_PHYSICAL_PORT_NUM portNum;
        CPSS_PORT_SPEED_ENT  speed;
        GT_STATUS            res;
    }PRV_CPSS_DXCH_PORT_SPEED_STC;

    typedef struct
    {
        GT_CHAR                      * namePtr;
        PRV_CPSS_DXCH_PORT_SPEED_STC * listPtr;
        GT_U32                         size;
    }PRV_DYNPIZZA_TEST_CASE_STC;

    typedef struct
    {
        CPSS_PORT_SPEED_ENT   speedEnt;
        GT_CHAR              *speedStr;
    }PRV_SPEED2STR_STC;


    static PRV_CPSS_DXCH_PORT_SPEED_STC prv_testPortSpeedArrNotSupported[] =
    {
         {    0, CPSS_PORT_SPEED_10_E        , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_100_E       , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_12000_E     , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_13600_E     , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_15000_E     , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_16000_E     , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_75000_E     , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_50000_E     , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_100G_E      , GT_NOT_SUPPORTED }
        ,{    0, CPSS_PORT_SPEED_140G_E      , GT_NOT_SUPPORTED }
    };

    static PRV_CPSS_DXCH_PORT_SPEED_STC prv_testPortSpeedArrNotMapped[] =
    {
         {   52, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM }
        ,{   53, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM }
        ,{   54, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM }
        ,{   55, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   59, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   60, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   61, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   62, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   64, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   65, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   66, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   67, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   68, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   69, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   70, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   71, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   72, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   73, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   74, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   75, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   76, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   77, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   78, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   79, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   81, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
        ,{   83, CPSS_PORT_SPEED_1000_E       , GT_BAD_PARAM  }
    };


    static PRV_CPSS_DXCH_PORT_SPEED_STC prv_testPortSpeedArr[] =
    {
        /* 1G */
         {    0, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    1, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    2, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    3, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    4, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    5, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    6, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    7, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    8, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{    9, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   10, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   11, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   12, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   13, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   14, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   15, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   16, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   17, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   18, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   19, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   20, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   21, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   22, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   23, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   24, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   25, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   26, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   27, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   28, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   29, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   30, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   31, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   32, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   33, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   34, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   35, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   36, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   37, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   38, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   39, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   40, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   41, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   42, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   43, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   44, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   45, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   46, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   47, CPSS_PORT_SPEED_1000_E      , GT_OK }
        ,{   48, CPSS_PORT_SPEED_1000_E      , GT_OK }
        /* 10 G */
        ,{   49, CPSS_PORT_SPEED_10000_E     , GT_OK }
        ,{   50, CPSS_PORT_SPEED_10000_E     , GT_OK }
        ,{   51, CPSS_PORT_SPEED_10000_E     , GT_OK }
        ,{   56, CPSS_PORT_SPEED_10000_E     , GT_OK }
        ,{   58, CPSS_PORT_SPEED_10000_E     , GT_OK }
        ,{   80, CPSS_PORT_SPEED_10000_E     , GT_OK }
        ,{   82, CPSS_PORT_SPEED_10000_E     , GT_OK }
        /* CPU port */
        ,{   63, CPSS_PORT_SPEED_10000_E     , GT_OK }
    };


    static PRV_DYNPIZZA_TEST_CASE_STC prv_dynPATestList[] =
    {
         {
             "NOT SUPPORTED"
             ,&prv_testPortSpeedArrNotSupported[0]
             ,sizeof(prv_testPortSpeedArrNotSupported)/sizeof(prv_testPortSpeedArrNotSupported[0])
         }
        ,{
             "NOT MAPPED PORTS"
             ,&prv_testPortSpeedArrNotMapped[0]
             ,sizeof(prv_testPortSpeedArrNotMapped)/sizeof(prv_testPortSpeedArrNotMapped[0])
         }
        ,{
             "Regular work"
            ,&prv_testPortSpeedArr[0]
            ,sizeof(prv_testPortSpeedArr)/sizeof(prv_testPortSpeedArr[0])
        }
    };

    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
    #include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
    #include <appDemo/sysHwConfig/appDemoDb.h>

    static PRV_SPEED2STR_STC prv_speedConvArr[] =
    {
         { CPSS_PORT_SPEED_10_E,      "  10M"  }
        ,{ CPSS_PORT_SPEED_100_E,     " 100M"  }
        ,{ CPSS_PORT_SPEED_1000_E,    "   1G"  }
        ,{ CPSS_PORT_SPEED_2500_E,    " 2.5G"  }
        ,{ CPSS_PORT_SPEED_5000_E,    "   5G"  }
        ,{ CPSS_PORT_SPEED_10000_E,   "  10G"  }
        ,{ CPSS_PORT_SPEED_12000_E,   "  12G"  }
        ,{ CPSS_PORT_SPEED_16000_E,   "  16G"  }
        ,{ CPSS_PORT_SPEED_15000_E,   "  15G"  }
        ,{ CPSS_PORT_SPEED_13600_E,   "13.6G"  }
        ,{ CPSS_PORT_SPEED_20000_E,   "  20G"  }
        ,{ CPSS_PORT_SPEED_40000_E,   "  40G"  }
        ,{ CPSS_PORT_SPEED_50000_E,   "  50G"  }
        ,{ CPSS_PORT_SPEED_75000_E,   "  75G"  }
        ,{ CPSS_PORT_SPEED_100G_E,    " 100G"  }
        ,{ CPSS_PORT_SPEED_140G_E,    " 140G"  }
        ,{ CPSS_PORT_SPEED_NA_E  ,    "  N/A"  }
    };

    GT_CHAR * prvTestFindSpeed(CPSS_PORT_SPEED_ENT speedEnm)
    {
        GT_U32 i;
        GT_U32 size;
        PRV_SPEED2STR_STC * ptr;
        ptr = &prv_speedConvArr[0];

        size = sizeof(prv_speedConvArr)/sizeof(prv_speedConvArr[0]);
        prv_speedConvArr[size-1].speedEnt = speedEnm;
        for (i = 0; ptr->speedEnt != speedEnm ; i++, ptr++);
        prv_speedConvArr[size-1].speedEnt = CPSS_PORT_SPEED_NA_E;
        return ptr->speedStr;
    }

    typedef enum
    {
        APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E,
        APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E,
        APP_DEMO_TRACE_HW_ACCESS_TYPE_BOTH_E,
        APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_DELAY_E,
        APP_DEMO_TRACE_HW_ACCESS_TYPE_ALL_E

    }APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT;

    extern GT_STATUS appDemoTraceHwAccessEnable
    (
        IN GT_U8                                devNum,
        IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT    accessType,
        IN GT_BOOL                              enable
    );



    #define GT_NA ((GT_U32)~0)
    #define APP_INV_PORT_CNS ((GT_U32)(~0))
    typedef enum
    {
         PORT_LIST_TYPE_EMPTY = 0
        ,PORT_LIST_TYPE_INTERVAL
        ,PORT_LIST_TYPE_LIST
    }PortListType_ENT;

    typedef struct
    {
        PortListType_ENT             entryType;
        GT_PHYSICAL_PORT_NUM         portList[20];  /* depending on list type */
                                                    /* interval : 0 startPort     */
                                                    /*            1 stopPort      */
                                                    /*            2 step          */
                                                    /*            3 APP_INV_PORT_CNS */
                                                    /* list     : 0...x  ports     */
                                                    /*            APP_INV_PORT_CNS */
        CPSS_PORT_SPEED_ENT          speed;
        CPSS_PORT_INTERFACE_MODE_ENT interfaceMode;
    }PortInitList_STC;

    typedef struct
    {
        GT_PHYSICAL_PORT_NUM         portNum;
        CPSS_PORT_SPEED_ENT          speed;
        CPSS_PORT_INTERFACE_MODE_ENT interfaceMode;
    }PortInitInternal_STC;


    static CPSS_DXCH_PORT_MAP_STC bc2TmEnable_0_2_48_49_Map[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort,        tmEnable,      tmPortInd*/
         {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        0,   GT_NA,             GT_TRUE,            0 }
        ,{   2, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        2,   GT_NA,             GT_TRUE,            2 }
        ,{  48, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       48,   GT_NA,             GT_TRUE,           48 }
        ,{  49, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       49,   GT_NA,             GT_TRUE,           49 }
    };

    static PortInitList_STC portInitlist_TmEnable_0_2_48_49[] =
    {
         { PORT_LIST_TYPE_LIST,      { 0,2,        APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E  }
        ,{ PORT_LIST_TYPE_LIST,      {48,          APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_LIST,      {49,          APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {             APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E      }
    };

    static char * prv_speed2str[] =
    {
        "10M  ",   /* 0 */
        "100M ",   /* 1 */
        "1G   ",   /* 2 */
        "10G  ",   /* 3 */
        "12G  ",   /* 4 */
        "2.5G ",   /* 5 */
        "5G   ",   /* 6 */
        "13.6G",   /* 7 */
        "20G  ",   /* 8 */
        "40G  ",   /* 9 */
        "16G  ",   /* 10 */
        "15G  ",   /* 11 */
        "75G  ",   /* 12 */
        "100G ",   /* 13 */
        "50G  ",   /* 14 */
        "140G "    /* 15 */
    };

    static char * prv_prvif2str[] =
    {
        "REDUCED_10BIT"   /* 0   CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E,    */
        ,"REDUCED_GMII"    /* 1   CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E,     */
        ,"MII         "    /* 2   CPSS_PORT_INTERFACE_MODE_MII_E,              */
        ,"SGMII       "    /* 3   CPSS_PORT_INTERFACE_MODE_SGMII_E,            */
        ,"XGMII       "    /* 4   CPSS_PORT_INTERFACE_MODE_XGMII_E,            */
        ,"MGMII       "    /* 5   CPSS_PORT_INTERFACE_MODE_MGMII_E,            */
        ,"1000BASE_X  "    /* 6   CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,       */
        ,"GMII        "    /* 7   CPSS_PORT_INTERFACE_MODE_GMII_E,             */
        ,"MII_PHY     "    /* 8   CPSS_PORT_INTERFACE_MODE_MII_PHY_E,          */
        ,"QX          "    /* 9   CPSS_PORT_INTERFACE_MODE_QX_E,               */
        ,"HX          "    /* 10  CPSS_PORT_INTERFACE_MODE_HX_E,                */
        ,"RXAUI       "    /* 11  CPSS_PORT_INTERFACE_MODE_RXAUI_E,             */
        ,"100BASE_FX  "    /* 12  CPSS_PORT_INTERFACE_MODE_100BASE_FX_E,        */
        ,"QSGMII      "    /* 13  CPSS_PORT_INTERFACE_MODE_QSGMII_E,            */
        ,"XLG         "    /* 14  CPSS_PORT_INTERFACE_MODE_XLG_E,               */
        ,"LOCAL_XGMII "    /* 15  CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E,       */
        ,"KR          "    /* 16  CPSS_PORT_INTERFACE_MODE_KR_E,                */
        ,"HGL         "    /* 17  CPSS_PORT_INTERFACE_MODE_HGL_E,               */
        ,"CHGL_12     "    /* 18  CPSS_PORT_INTERFACE_MODE_CHGL_12_E,           */
        ,"ILKN12      "    /* 19  CPSS_PORT_INTERFACE_MODE_ILKN12_E,            */
        ,"SR_LR       "    /* 20  CPSS_PORT_INTERFACE_MODE_SR_LR_E,             */
        ,"ILKN16      "    /* 21  CPSS_PORT_INTERFACE_MODE_ILKN16_E,            */
        ,"ILKN24      "    /* 22  CPSS_PORT_INTERFACE_MODE_ILKN24_E,            */
        ,"NA_E        "    /* 23  CPSS_PORT_INTERFACE_MODE_NA_E                 */
    };

    static GT_CHAR * prv_mappingTypeStr[CPSS_DXCH_PORT_MAPPING_TYPE_MAX_E] =
    {
         "ETHERNET"
        ,"CPU-SDMA"
        ,"ILKN-CHL"
        ,"REMOTE-P"
    };


    static GT_STATUS TestPort0x20x48x49_PortInterfaceInit
    (
        IN  GT_U8 dev
    )
    {
        GT_STATUS   rc;             /* return code */
        GT_U32      i;                  /* interator */
        CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                            *initPortsBmpPtr;/* pointer to bitmap */
        PortInitList_STC * portInitList;
        PortInitList_STC * portInitPtr;
        GT_U32         portIdx;
        GT_U32         maxPortIdx;
        GT_PHYSICAL_PORT_NUM portNum;
        static PortInitInternal_STC    portList[256];

        initPortsBmpPtr = &initPortsBmp;

        portInitList = &portInitlist_TmEnable_0_2_48_49[0];

        for (portIdx = 0 ; portIdx < sizeof(portList)/sizeof(portList[0]); portIdx++)
        {
            portList[portIdx].portNum       = APP_INV_PORT_CNS;
            portList[portIdx].speed         = CPSS_PORT_SPEED_NA_E;
            portList[portIdx].interfaceMode = CPSS_PORT_INTERFACE_MODE_NA_E;
        }
        maxPortIdx = 0;
        portInitPtr = &portInitList[0];
        for (i = 0 ; portInitPtr->entryType != PORT_LIST_TYPE_EMPTY; i++,portInitPtr++)
        {
            for (portIdx = 0 ; portInitPtr->portList[portIdx] != APP_INV_PORT_CNS; portIdx++)
            {
                GT_BOOL _isValid;

                portNum = portInitPtr->portList[portIdx];
                rc = cpssDxChPortPhysicalPortMapIsValidGet(dev, portNum, &_isValid);
                if (rc == GT_OK && _isValid == GT_TRUE)
                {
                    portList[maxPortIdx].portNum       = portNum;
                    portList[maxPortIdx].speed         = portInitPtr->speed;
                    portList[maxPortIdx].interfaceMode = portInitPtr->interfaceMode;
                    maxPortIdx++;
                }
            }
        }

        cpssOsPrintf("\n+----+------+-------+--------------+-----------------+------------------------------+------+");
        cpssOsPrintf("\n| #  | Port | Speed |    IF        |   mapping Type  | rxdma txdma mac txq ilkn  tm |  res |");
        cpssOsPrintf("\n+----+------+-------+--------------+-----------------+------------------------------+------+");

        for (portIdx = 0 ; portIdx < maxPortIdx; portIdx++)
        {
            CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapPtr;
            rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(dev,portList[portIdx].portNum,/*OUT*/&portMapPtr);
            if (rc != GT_OK)
            {
                return rc;
            }

            cpssOsPrintf("\n| %2d | %4d | %s | %s |",portIdx,
                                                  portList[portIdx].portNum,
                                                  prv_speed2str[portList[portIdx].speed],
                                                  prv_prvif2str[portList[portIdx].interfaceMode]);
            cpssOsPrintf(" %-15s | %5d %5d %3d %3d %4d %3d |"
                                                ,prv_mappingTypeStr[portMapPtr->portMap.mappingType]
                                                ,portMapPtr->portMap.rxDmaNum
                                                ,portMapPtr->portMap.txDmaNum
                                                ,portMapPtr->portMap.macNum
                                                ,portMapPtr->portMap.txqNum
                                                ,portMapPtr->portMap.ilknChannel
                                                ,portMapPtr->portMap.tmPortIdx);


            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
            CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portList[portIdx].portNum);
            appDemoTraceHwAccessEnable(dev, APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E, GT_TRUE);
            rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,
                                            portList[portIdx].interfaceMode,
                                            portList[portIdx].speed);
            if(rc != GT_OK)
            {
                cpssOsPrintf("\n--> ERROR : cpssDxChPortModeSpeedSet() :rc=%d\n", rc);
                return rc;
            }
            appDemoTraceHwAccessEnable(dev, APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E, GT_FALSE);
            cpssOsPrintf("  OK  |");
        }
        cpssOsPrintf("\n+----+------+-------+--------------+-----------------+------------------------------+------+");
        cpssOsPrintf("\nPORT INTERFACE Init Done.");
        return GT_OK;
    }


    GT_STATUS TestPort0x20x48x49(GT_VOID)
    {
        GT_STATUS rc;
        GT_U32 initSerdesDefaults;
        GT_U8 dev = 0;

        initSerdesDefaults = 0;
        rc = appDemoDbEntryAdd("initSerdesDefaults", initSerdesDefaults);

        rc = cpssInitSystem(29,1,0);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPortPhysicalPortMapSet(dev, sizeof(bc2TmEnable_0_2_48_49_Map)/sizeof(bc2TmEnable_0_2_48_49_Map[0]), &bc2TmEnable_0_2_48_49_Map[0]);
        if (GT_OK != rc)
        {
            return rc;
        }
        rc = TestPort0x20x48x49_PortInterfaceInit(dev);
        return rc;
    }

    GT_STATUS prvTestDynPA(GT_VOID)
    {
        GT_STATUS rc;
        GT_U32    testCase;
        GT_U32    idx;
        PRV_CPSS_DXCH_PORT_SPEED_STC * ptr;


        rc = cpssInitSystem(29,1,0);
        if (rc != GT_OK)
        {
            return rc;
        }

        /*rc = prvCpssDxChPortDynamicPizzaArbiterIfInit(0);
          if (rc != GT_OK)
          {
              return rc;
          }
        */
        for (testCase = 0; testCase< sizeof(prv_dynPATestList)/sizeof(prv_dynPATestList[0]); testCase++)
        {
            printf("\n--------------------------------------");
            printf("\n  %s ",prv_dynPATestList[testCase].namePtr);
            printf("\n--------------------------------------");
            ptr = prv_dynPATestList[testCase].listPtr;
            for (idx = 0 ; idx < prv_dynPATestList[testCase].size; idx++, ptr++)
            {
                printf("\n %4d %2d : speed = %s exp-rc = %d ... ",idx,ptr->portNum, prvTestFindSpeed(ptr->speed), ptr->res);

                rc = prvCpssDxChPortPizzaArbiterIfCheckSupport(0,ptr->portNum,ptr->speed);
                printf("Check rc = %d   ", rc);
                if (ptr->res != rc)
                {
                    printf(".....Error !!!");
                    return rc;
                }

                rc = prvCpssDxChPortPizzaArbiterIfConfigure(0,ptr->portNum,ptr->speed);
                printf("Conf rc = %d", rc);

                if (ptr->res != rc)
                {
                    printf(".....Error !!! once more");
                    rc = prvCpssDxChPortPizzaArbiterIfConfigure(0,ptr->portNum,ptr->speed);
                    return rc;
                }
            }
        }
        return GT_OK;
    }

#endif

