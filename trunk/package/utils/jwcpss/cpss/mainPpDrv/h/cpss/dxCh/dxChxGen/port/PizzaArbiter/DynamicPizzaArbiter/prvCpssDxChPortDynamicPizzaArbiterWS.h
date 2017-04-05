/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterWS.h
*
* DESCRIPTION:
*       bobcat2 and higher dynamic (algorithmic) pizza arbiter Data structures (device specific)
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_DYNAMIC_PIZZA_ARBITER_WS_H
#define __PRV_CPSS_DXCH_PORT_DYNAMIC_PIZZA_ARBITER_WS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>


typedef struct 
{
    GT_U32      exactAlgoHighSpeedPortThresh;
    GT_U32      apprAlgoHighSpeedPortThresh_Q20;
}PRV_CPSS_DXCH_HIGH_SPEED_PORT_THRESH_PARAMS_STC;


extern GT_STATUS BuildPizzaDistribution
(
    IN   GT_FLOAT64 *portsConfigArrayPtr,
    IN   GT_U32      size,
    IN   GT_U32      maxPipeSizeMbps,
    IN   GT_U32      minSliceResolutionMpbs,
    IN   PRV_CPSS_DXCH_HIGH_SPEED_PORT_THRESH_PARAMS_STC *highSpeedPortThreshPtr,
    IN   GT_U32      pizzaArraySize,
    OUT  GT_U32     *pizzaArray,
    OUT  GT_U32     *numberOfSlicesPtr,
    OUT  GT_U32     *highSpeedPortNumberPtr,
    OUT  GT_U32      highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
);


extern GT_STATUS BuildPizzaDistributionWithDummySlice
(
    IN   GT_FLOAT64  *portsConfigArrayPtr,
    IN   GT_U32       size,
    IN   GT_U32       maxPipeSize,
    IN   GT_U32       minSliceResolution,
    IN   PRV_CPSS_DXCH_HIGH_SPEED_PORT_THRESH_PARAMS_STC *highSpeedPortThreshPtr,
    IN   GT_U32       pizzaArraySize,
    OUT  GT_U32      *pizzaArray,
    OUT  GT_U32      *numberOfSlicesPtr,
    OUT  GT_U32      *highSpeedPortNumPtr,
    OUT  GT_U32       highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
);

extern GT_STATUS BuildTxQPizzaDistribution
(
    IN   GT_FLOAT64 *portsConfigArrayPtr,
    IN   GT_U32      size,
    IN   GT_U32      maxPipeSizeMbps,
    IN   GT_U32      minSliceResolutionMpbs,
    IN   PRV_CPSS_DXCH_HIGH_SPEED_PORT_THRESH_PARAMS_STC *highSpeedPortThreshPtr,
    IN   GT_U32      pizzaArraySize,
    OUT  GT_U32     *pizzaArray,
    OUT  GT_U32     *numberOfSlicesPtr,
    OUT  GT_U32     *highSpeedPortNumberPtr,
    OUT  GT_U32      highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
);


/* function proptype that converts from Mapping to unit number */

typedef GT_STATUS (*PRV_CPSS_DXCH_MAPPING_2_UNIT_LIST_CONV_FUN)
(
    IN GT_U8 devNum,
    IN CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr,
    OUT GT_U32  portArgArr[CPSS_DXCH_PA_UNIT_MAX_E]
);

typedef GT_STATUS (* PRV_CPSS_DXCH_BUILD_PIZZA_DISTRBUTION_FUN)
(
    IN   GT_FLOAT64 *portsConfigArrayPtr,
    IN   GT_U32      size,
    IN   GT_U32      maxPipeSize,
    IN   GT_U32      minSliceResolutionInMBps,
    IN   PRV_CPSS_DXCH_HIGH_SPEED_PORT_THRESH_PARAMS_STC *highSpeedPortThreshPtr,
    IN   GT_U32      pizzaArraySize,
    OUT  GT_U32     *pizzaArray,
    OUT  GT_U32     *numberOfSlicesPtr,
    OUT  GT_U32     *highSpeedPortNumberPtr,
    OUT  GT_U32      highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
);

typedef GT_STATUS (* PRV_CPSS_DXCH_HIGH_SPEED_PORT_CONFIG_FUN)
(
    IN  GT_U8  devNum,
    IN  GT_U32  highSpeedPortNumber,
    IN  GT_U32  highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
);


typedef struct 
{
    CPSS_DXCH_PA_UNIT_ENT     unit;
    GT_U32                    mppmClientCode;
    GT_U32                    clientWeight;
}PRV_CPSS_DXCH_MPPM_CLIENT_CODING_STC;


typedef struct 
{
    CPSS_DXCH_PA_UNIT_ENT unit;
    GT_U32                clientId;
}PRV_CPSS_DXCH_PA_UNIT_CLIENT_IDX_STC;

/*
 *------------------------------------------------------------------------------------------*
 * configuration data (static,depends on device)                                            *
 *      used to control the flow of PA engine                                               *
 *------------------------------------------------------------------------------------------*
 * prv_DeviceUnitListPtr                                                                    *
 *       list of units, supported by device                                                 *
 *       ex : BC2 :                                                                         *
 *                 rx-dma, txq, tx-fifo, eth-tx-fFifo, tx-dma, ilkn-txdma                   *
 *            BobK pipe0-pipe1                                                              *
 *                 rx-dma,   txq, tx-fifo,   eth-tx-fFifo,   tx-dma,                        *
 *                 rx-dma-1,      tx-fifo-1, eth-tx-fFifo-1, tx-dma-1                       *
 *                 rx-dma-glue tx-dma-glue                                                  *
 *            BobK pipe1                                                                    *
 *                 rx-dma-1, txq  tx-fifo-1, eth-tx-fFifo-1, tx-dma-1                       *
 *                                                                                          *
 * prv_mappingType2UnitConfArr                                                              *
 *       list of units to configure for given mapping type with/wo TM                       *
 * prv_unit2PizzaAlgoFunArr                                                                 *
 *       algorithm to be used for pizza computa6ion for specific unit                       *
 *         ex:  DP units : Tx-Rx-DMA TxFIfo EthTxFifo -- dma algo (no empty slices          *
 *              TxQ      : TxQ algo : full space, min-distance between slices               *
 *              NULL     : unit is not configured                                           *
 * prv_unit2HighSpeedPortConfFunArr                                                         *
 *       function to configure high speed ports (relevant just for TxQ                      *
 *         all units but TxQ shall be initilized to NULL                                    *
 *------------------------------------------------------------------------------------------*
 * working data (stored intermediate results )                                              *
 *      used as input data for pizza compurtaion                                            *
 *      updated after each compuation cycle                                                 *
 *------------------------------------------------------------------------------------------*
 * prv_portSpeedConfigArr                                                                   *
 *      stored speed of each port for each unit                                             *
 *------------------------------------------------------------------------------------------*
 */

typedef struct PRV_CPSS_DXCH_PA_WORKSPACE_STC
{
    GT_U32                                           prv_speedEnt2MBitConvArr[CPSS_PORT_SPEED_NA_E];  /* CPSS API --> MBps speed conversion table */
    CPSS_DXCH_PA_UNIT_ENT                           *prv_DeviceUnitListPtr;
    GT_BOOL                                          prv_DeviceUnitListBmp                 [CPSS_DXCH_PA_UNIT_MAX_E];
    CPSS_DXCH_PA_UNIT_ENT                           *prv_unitListConfigByPipeBwSetPtr;
    GT_BOOL                                          prv_unitListConfigByPipeBwSetBmp      [CPSS_DXCH_PA_UNIT_MAX_E];
    CPSS_DXCH_PA_UNIT_ENT                           *prv_mappingType2UnitConfArr           [CPSS_DXCH_PORT_MAPPING_TYPE_MAX_E][GT_TRUE+1];
    PRV_CPSS_DXCH_BUILD_PIZZA_DISTRBUTION_FUN        prv_unit2PizzaAlgoFunArr              [CPSS_DXCH_PA_UNIT_MAX_E];
    PRV_CPSS_DXCH_HIGH_SPEED_PORT_THRESH_PARAMS_STC  prv_txQHighSpeedPortThreshParams;
    PRV_CPSS_DXCH_HIGH_SPEED_PORT_CONFIG_FUN         prv_unit2HighSpeedPortConfFunArr      [CPSS_DXCH_PA_UNIT_MAX_E];
    PRV_CPSS_DXCH_MAPPING_2_UNIT_LIST_CONV_FUN       mapping2unitConvFunPtr;
    PRV_CPSS_DXCH_MPPM_CLIENT_CODING_STC            *mppmClientCodingListPtr;
    CPSS_DXCH_PA_UNIT_ENT                           *workConservingModeOnUnitListPtr;
    PRV_CPSS_DXCH_PA_UNIT_CLIENT_IDX_STC            *tmUnitClientListPtr;
}PRV_CPSS_DXCH_PA_WORKSPACE_STC;


typedef struct PRV_CPSS_DXCH_BC2_PA_SUBFAMILY_2_WORKSPCE_STC
{
    CPSS_PP_SUB_FAMILY_TYPE_ENT      subFamily;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC  *workSpacePtr;
}PRV_CPSS_DXCH_BC2_PA_SUBFAMILY_2_WORKSPCE_STC;

typedef struct PRV_CPSS_DXCH_PA_WS_SET_STC
{
    GT_BOOL                                    isInit;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC             paWorkSpace_BC2_with_ilkn;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC             paWorkSpace_BC2_wo_ilkn;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC             paWorkSpace_BobK_Caelum_pipe0_pipe1;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC             paWorkSpace_BobK_Cetus_pipe1;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC             paWorkSpace_BC3;
}PRV_CPSS_DXCH_PA_WS_SET_STC;


/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterWSInit
*
* DESCRIPTION:
*       Init Pizza Arbiter WSs (objects)
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterWSInit
(
    IN  GT_U8                   devNum
);


/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterWSGet
*
* DESCRIPTION:
*       Get Pizza Work space (object) by device
*
* APPLICABLE DEVICES:
*         Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       workSpacePtrPtr - pointer to pointer on workspace.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - NULL ptr
*       GT_FAIL         - on error
*
* COMMENTS:
*       I forced to place this function here, because it needs number of port
*       group where CPU port is connected and there is just no more suitable
*       place.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterWSGet
(
    IN  GT_U8                            devNum,
    OUT PRV_CPSS_DXCH_PA_WORKSPACE_STC **workSpacePtrPtr
);

/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterWSSuportedUnitListGet
*
* DESCRIPTION:
*       Get list of supported units
*
* APPLICABLE DEVICES:
*         Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       supportedUnitListPtr - pointer to pointer to list of supported units
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - NULL ptr
*       GT_FAIL         - on error
*
* COMMENTS:
*       I forced to place this function here, because it needs number of port
*       group where CPU port is connected and there is just no more suitable
*       place.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterWSSuportedUnitListGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_DXCH_PA_UNIT_ENT  **supportedUnitListPtrPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif





