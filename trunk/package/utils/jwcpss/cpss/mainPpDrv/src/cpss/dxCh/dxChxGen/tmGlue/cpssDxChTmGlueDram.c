/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTmGlueDram.c
*
* DESCRIPTION:
*       TM related DRAM APIs
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGlueLog.h>

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueDram.h>
#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGlue.h>

/*#include <cpss/generic/tm/prvCpssTmDefs.h>*/
#include <cpss/generic/tm/prvCpssTmCtl.h>
#include <cpss/generic/tm/cpssTmCtl.h>
#include <platform/tm_regs.h>

#include <mvHwsDdr3Bap.h>
#include <mvHwsDdr3Bc2.h>
#include <mvDdr3TrainingIpFlow.h>
/* forward declaration */
GT_STATUS ddr3BapSetAlgoFlow
(
	GT_U8       devNum,
	GT_U32		algoFlowMask
);

static GT_U8 prvCpssDxChTmGlueDramInitSteps = 0;

GT_STATUS prvCpssDxChTmGlueDramSetInitSteps(GT_U8 numOfInitSteps)
{
    prvCpssDxChTmGlueDramInitSteps = numOfInitSteps;
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChTmGlueDramConvertConfiguration
*
* DESCRIPTION:
*       Convert CPSS DRAM Configuration structure to HWS DRAM topology mapping.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       cpssDramCfgPtr      - (pointer to) CPSS DRAM configurations.
*
* OUTPUTS:
*       mvHwsTopologyPtr    - (pointer to) HWS DRAM topology mappings.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTmGlueDramConvertConfiguration
(

    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TM_GLUE_DRAM_CFG_STC  *cpssDramCfgPtr,
    OUT MV_HWS_TOPOLOGY_MAP             *mvHwsTopologyPtr
)
{
    GT_U32                  inf;        /* interface loop iterator */
    GT_U32                  bus;        /* bus loop iterator */
    CPSS_DRAM_SPEED_BIN_ENT speedBin;   /* DRAM bin*/
    CPSS_DRAM_SIZE_ENT      memorySize; /* DRAM size */
    CPSS_DRAM_FREQUENCY_ENT memoryFreq; /* DRAM frequency */
    GT_U8                   nextInterface;

    cpssOsMemSet(mvHwsTopologyPtr, 0x0, sizeof(MV_HWS_TOPOLOGY_MAP));

    PRV_CPSS_TM_CTL_CHECK_DRAM_INF_NUM_MAC(devNum, cpssDramCfgPtr->activeInterfaceNum);

    /* interfaceActiveMask */
    mvHwsTopologyPtr->interfaceActiveMask =
        (GT_U8)BIT_MASK_MAC(cpssDramCfgPtr->activeInterfaceNum);

    if (cpssDramCfgPtr->activeInterfaceMask)
    {
        mvHwsTopologyPtr->interfaceActiveMask = (GT_U8)cpssDramCfgPtr->activeInterfaceMask; 
    }
        
    /* numOfBusPerInterface */
    /* set 4 active buses 8 bit each */
    mvHwsTopologyPtr->activeBusMask = 0xF;

    /* interfaceParams */
    inf = 0;

    /* asBusParams */
    for( bus = 0 ; bus < ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE) ; bus++ )
    {
        /* csBitmask */
        mvHwsTopologyPtr->interfaceParams[inf].asBusParams[bus].csBitmask =
              (GT_U8)cpssDramCfgPtr->interfaceParams.busParams[bus].csBitmask;

        /* mirrorEnableBitmask */
        mvHwsTopologyPtr->interfaceParams[inf].asBusParams[bus].
                                                    mirrorEnableBitmask =
            BOOL2BIT_MAC(
              cpssDramCfgPtr->interfaceParams.busParams[bus].mirrorEn);

        /* isDqsSwap */
        mvHwsTopologyPtr->interfaceParams[inf].asBusParams[bus].isDqsSwap =
            BOOL2BIT_MAC(
              cpssDramCfgPtr->interfaceParams.busParams[bus].dqsSwapEn);

        /* isCkSwap*/
        mvHwsTopologyPtr->interfaceParams[inf].asBusParams[bus].isCkSwap =
            BOOL2BIT_MAC(
              cpssDramCfgPtr->interfaceParams.busParams[bus].ckSwapEn);
    }

    /* speedBin */
    switch(cpssDramCfgPtr->interfaceParams.speedBin)
    {
        case CPSS_DRAM_SPEED_BIN_DDR3_1333F_E:
                                            speedBin = SPEED_BIN_DDR_1333F;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1333G_E:
                                            speedBin = SPEED_BIN_DDR_1333G;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1333H_E:
                                            speedBin = SPEED_BIN_DDR_1333H;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1333J_E:
                                            speedBin = SPEED_BIN_DDR_1333J;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1600G_E:
                                            speedBin = SPEED_BIN_DDR_1600G;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1600H_E:
                                            speedBin = SPEED_BIN_DDR_1600H;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1600J_E:
                                            speedBin = SPEED_BIN_DDR_1600J;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1600K_E:
                                            speedBin = SPEED_BIN_DDR_1600K;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1866J_E:
                                            speedBin = SPEED_BIN_DDR_1866J;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1866K_E:
                                            speedBin = SPEED_BIN_DDR_1866K;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1866L_E:
                                            speedBin = SPEED_BIN_DDR_1866L;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_1866M_E:
                                            speedBin = SPEED_BIN_DDR_1866M;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_2133K_E:
                                            speedBin = SPEED_BIN_DDR_2133K;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_2133L_E:
                                            speedBin = SPEED_BIN_DDR_2133L;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_2133M_E:
                                            speedBin = SPEED_BIN_DDR_2133M;
                                            break;
        case CPSS_DRAM_SPEED_BIN_DDR3_2133N_E:
                                            speedBin = SPEED_BIN_DDR_2133N;
                                            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    mvHwsTopologyPtr->interfaceParams[inf].speedBinIndex = speedBin;

    /* busWidth */
    switch(cpssDramCfgPtr->interfaceParams.busWidth)
    {
        case CPSS_DRAM_BUS_WIDTH_8_E:
            mvHwsTopologyPtr->interfaceParams[inf].busWidth = BUS_WIDTH_8;
            break;
        case CPSS_DRAM_BUS_WIDTH_16_E:
            mvHwsTopologyPtr->interfaceParams[inf].busWidth = BUS_WIDTH_16;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* memorySize */
    switch(cpssDramCfgPtr->interfaceParams.memorySize)
    {
        case CPSS_DRAM_64MB_E:  memorySize = MEM_512M;
                                break;
        case CPSS_DRAM_128MB_E: memorySize = MEM_1G;
                                break;
        case CPSS_DRAM_256MB_E: memorySize = MEM_2G;
                                break;
        case CPSS_DRAM_512MB_E: memorySize = MEM_4G;
                                break;
        case CPSS_DRAM_1GB_E:   memorySize = MEM_8G;
                                break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    mvHwsTopologyPtr->interfaceParams[inf].memorySize = memorySize;

    /* memoryFreq */
    switch(cpssDramCfgPtr->interfaceParams.memoryFreq)
    {
        case CPSS_DRAM_FREQ_667_MHZ_E: memoryFreq = DDR_FREQ_667;
                                       break;
        case CPSS_DRAM_FREQ_800_MHZ_E: memoryFreq = DDR_FREQ_800;
                                       break;
        case CPSS_DRAM_FREQ_933_MHZ_E: memoryFreq = DDR_FREQ_933;
                                       break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    mvHwsTopologyPtr->interfaceParams[inf].memoryFreq = memoryFreq;

    /* casWL */
    mvHwsTopologyPtr->interfaceParams[inf].casWL =
                                (GT_U8)cpssDramCfgPtr->interfaceParams.casWL;

    /* casL */
    mvHwsTopologyPtr->interfaceParams[inf].casL =
                                (GT_U8)cpssDramCfgPtr->interfaceParams.casL;

    /* interfaceTemp*/
    switch(cpssDramCfgPtr->interfaceParams.interfaceTemp)
    {
        case CPSS_DRAM_TEMPERATURE_NORMAL_E:
            mvHwsTopologyPtr->interfaceParams[inf].interfaceTemp =
                                            MV_HWS_TEMP_NORMAL;
                                            break;
        case CPSS_DRAM_TEMPERATURE_HIGH_E:
            mvHwsTopologyPtr->interfaceParams[inf].interfaceTemp =
                                            MV_HWS_TEMP_HIGH;
                                            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* All interfaces are identical */
    nextInterface = mvHwsTopologyPtr->interfaceActiveMask >> 1;

    for( inf = 1 ; inf < MAX_INTERFACE_NUM && nextInterface; inf++)
    {
        if (nextInterface & 0x01)
        {
            mvHwsTopologyPtr->interfaceParams[inf] = 
                                              mvHwsTopologyPtr->interfaceParams[0];
        }

        nextInterface = nextInterface >> 1;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChTmGlueDramConvertAlgorithmParameters
*
* DESCRIPTION:
*       Convert CPSS DRAM Configuration algorithm structure to HWS algorithm
*       parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       cpssDramAlgoPtr     - (pointer to) CPSS DRAM algorithm configurations.
*
* OUTPUTS:
*       mvHwsAlgoParamsPtr  - (pointer to) HWS DRAM algorithm parameters.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTmGlueDramConvertAlgorithmParameters
(

    IN  CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC    *cpssDramAlgoPtr,
    OUT MV_HWS_BAP_ALGO_PARAMS                  *mvHwsAlgoParamsPtr
)
{

    switch(cpssDramAlgoPtr->algoType)
    {
        case CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_DYNAMIC_E:
                mvHwsAlgoParamsPtr->algoType = ALGO_TYPE_DYNAMIC;
                break;
        case CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_STATIC_E:
                mvHwsAlgoParamsPtr->algoType = ALGO_TYPE_STATIC;
                break;
        default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (GT_TRUE == cpssDramAlgoPtr->performWriteLeveling)
    {
        mvHwsAlgoParamsPtr->algoFlowMask = 0x3150f1;
    }
    else
    {
        mvHwsAlgoParamsPtr->algoFlowMask = 0x3110D1;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTmGlueDramInit
*
* DESCRIPTION:
*       Traffic Manager DRAM related configurations
*       (DDR, PHY & TM BAP registers).
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       dramCfgPtr  - (pointer to) DRAM interface configuration parameters
*       dramAlgoPtr - (pointer to) DRAM configuration algorithm parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or configuration parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_CPU_MEM        - no CPU memory allocation fail
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueDramInit
(
    IN  GT_U8                                  devNum,
    IN  CPSS_DXCH_TM_GLUE_DRAM_CFG_STC         *dramCfgPtr,
    IN  CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC   *dramAlgoPtr
)
{
    MV_HWS_BAP_ALGO_PARAMS  algoParams;     /* algorithem parameters */
    GT_STATUS               rc;             /* return code           */
    GT_U32                  algoFlowMask;   /* flags bitmap          */
    GT_U32                  initDoneFlag;   /* flags of init result  */
	/*GT_U64					regData;*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E
                                            | CPSS_LION_E | CPSS_LION2_E |  CPSS_BOBCAT3_E);
	
	prvCpssTmCtlDevInit(devNum); /* verify tm device information is initialized */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr == NULL)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr =
            (MV_HWS_TOPOLOGY_MAP*)cpssOsMalloc(sizeof(MV_HWS_TOPOLOGY_MAP));
        if (PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
    }

	/*
	regData.l[1] = 0xFF;
	regData.l[0] = 0xFF;
	cpssOsPrintf("TM.BAP[2].BAPPHYReadPath: set init value to: 0x%0x\n", regData.l[0]);
	rc = cpssTmCtlWriteRegister( devNum,  TM.BAP[2].BAPPHYReadPath, &regData);
	if (rc) return rc;
	*/

    rc = prvCpssDxChTmGlueDramConvertConfiguration(
        devNum, dramCfgPtr, PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr);
    if( GT_OK != rc )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "TmGlue->prvCpssDxChTmGlueDramConvertConfiguration: devNum: %d, DdrIntNum: %d, DdrIntMask: %0x", 
                                          devNum, dramCfgPtr->activeInterfaceNum, dramCfgPtr->activeInterfaceMask);
        /*return rc;*/
    }

    /* store topology in the DB */
    ddr3TipSetTopologyMap(
        devNum, PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr);    

    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.externalMemoryInitFlags
         & (~ PRV_CPSS_DXCH_TM_GLUE_DRAM_INIT_DONE_MASK_CNS))
        == CPSS_DXCH_TM_GLUE_DRAM_INIT_DDR_PHY_CNS)
    {
        /* initialize external memory controller and Phy only */

        initDoneFlag = PRV_CPSS_DXCH_TM_GLUE_DRAM_INIT_DONE_DDR_PHY_CNS;

        rc = mvHwsDdr3TipSelectDdrController(devNum, GT_TRUE);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "TmGlue->mvHwsDdr3TipSelectDdrController: devNum: %d", devNum);
            /*return rc;*/
        }

        if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
        {
            algoFlowMask = 0;
        }
        else
        {
            algoFlowMask =
                INIT_CONTROLLER_MASK_BIT |
                SET_MEDIUM_FREQ_MASK_BIT |
                WRITE_LEVELING_MASK_BIT |
                LOAD_PATTERN_2_MASK_BIT |
                READ_LEVELING_MASK_BIT |
                SET_TARGET_FREQ_MASK_BIT |
                WRITE_LEVELING_TF_MASK_BIT |
                READ_LEVELING_TF_MASK_BIT |
                CENTRALIZATION_RX_MASK_BIT |
                CENTRALIZATION_TX_MASK_BIT;
        }

        rc = ddr3BapSetAlgoFlow(devNum, algoFlowMask);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "TmGlue->ddr3BapSetAlgoFlow: devNum: %d, algoFlowMask: %0x ", 
                                              devNum, algoFlowMask);
            /*return rc;*/
        }

        rc = mvHwsDdr3TipRunAlg(devNum, ALGO_TYPE_DYNAMIC);

        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "TmGlue->mvHwsDdr3TipRunAlg: devNum: %d, ALGO_TYPE_DYNAMIC", devNum);
        }
    }
    else
    {
        initDoneFlag = PRV_CPSS_DXCH_TM_GLUE_DRAM_INIT_DONE_TM_CNS;

        /* initialize TM and all related HW */
        rc = prvCpssDxChTmGlueDramConvertAlgorithmParameters(dramAlgoPtr, &algoParams);
        if( GT_OK != rc )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "TmGlue->prvCpssDxChTmGlueDramConvertAlgorithmParameters: devNum: %d", devNum);
            /* return rc; */
        }

        rc = mvHwsDdr3BapRunAlg(devNum, &algoParams);
        if( GT_OK != rc )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "TmGlue->mvHwsDdr3BapRunAlg: devNum: %d, algoType: %d, algoFlowMaks: %0x", 
                                              devNum, algoParams.algoType, algoParams.algoFlowMask);
            /* return rc; */
        }

        if(prvCpssDxChTmGlueDramInitSteps == 0)
        {
       
            /*config Tm HW*/
            rc = prvCpssTmCtlHwInit(devNum, (GT_U8)dramCfgPtr->activeInterfaceNum);
            if( GT_OK != rc )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "TmGlue->prvCpssTmCtlHwInit: devNum: %d, numOfLads: %d", 
                                              devNum, dramCfgPtr->activeInterfaceNum);
            }
        }
        /* 
        else 
            prvCpssTmCtlHwInit is initialized separately 
        */ 
    }

    if (rc == GT_OK)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.externalMemoryInitFlags
            |= initDoneFlag;
    }
    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueDramInit
*
* DESCRIPTION:
*       Traffic Manager DRAM related configurations
*       (DDR, PHY & TM BAP registers).
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       dramCfgPtr  - (pointer to) DRAM interface configuration parameters
*       dramAlgoPtr - (pointer to) DRAM configuration algorithm parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or configuration parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_CPU_MEM        - no CPU memory allocation fail
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDramInit
(
    IN  GT_U8                                  devNum,
    IN  CPSS_DXCH_TM_GLUE_DRAM_CFG_STC         *dramCfgPtr,
    IN  CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC   *dramAlgoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueDramInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dramCfgPtr, dramAlgoPtr));

    rc = internal_cpssDxChTmGlueDramInit(devNum, dramCfgPtr, dramAlgoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dramCfgPtr, dramAlgoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueDramInitFlagsSet
*
* DESCRIPTION:
*       Set flags for Traffic Manager DRAM related configurations
*       (DDR, PHY & TM BAP registers).
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                   - physical device number
*       externalMemoryInitFlags  - External Memory Init Flags
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or configuration parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueDramInitFlagsSet
(
    IN  GT_U8                                 devNum,
    IN  GT_U32                                externalMemoryInitFlags
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E
                                            | CPSS_LION_E | CPSS_LION2_E |  CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.externalMemoryInitFlags = externalMemoryInitFlags;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueDramInitFlagsSet
*
* DESCRIPTION:
*       Set flags for Traffic Manager DRAM related configurations
*       (DDR, PHY & TM BAP registers).
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum                   - physical device number
*       externalMemoryInitFlags  - External Memory Init Flags
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or configuration parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDramInitFlagsSet
(
    IN  GT_U8                                 devNum,
    IN  GT_U32                                externalMemoryInitFlags
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueDramInitFlagsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, externalMemoryInitFlags));

    rc = internal_cpssDxChTmGlueDramInitFlagsSet(devNum, externalMemoryInitFlags);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, externalMemoryInitFlags));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
