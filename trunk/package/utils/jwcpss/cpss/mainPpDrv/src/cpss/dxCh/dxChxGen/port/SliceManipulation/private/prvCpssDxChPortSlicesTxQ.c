/*******************************************************************************
*              (c), Copyright 2012, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortSlicesTxQ.c
*
* DESCRIPTION:
*       TxQ Unit Slices Manipulation Interface
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxQ.h>

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQCpuPortPortGroupIdGet
*
* DESCRIPTION:
*      The function return port group id having CPU 
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum    - pointer to device 
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 prvLion2PortPizzaArbiterTxQCpuPortPortGroupIdGet
(
    IN GT_U8 devNum
)
{
    GT_U32 cpuPortPortGroupId;
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr = PRV_CPSS_PP_MAC(devNum);
    if (devPtr->cpuPortMode != CPSS_NET_CPU_PORT_MODE_SDMA_E)
    {
        cpuPortPortGroupId = devPtr->portGroupsInfo.cpuPortPortGroupId;
    }
    else
    {
        cpuPortPortGroupId = devPtr->netifSdmaPortGroupId;
    }
    return cpuPortPortGroupId;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterConfigureTxQSliceNumWoCPUPortGet
*
* DESCRIPTION:
*      The function return number of slice without CPU slices
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devPtr           - pointer to device
*       portGroupId      - port group id 
*       slicesNumberPerGop - slices Number Per GOP
*       txQCpuPortRepetitionNum - txQ pattern Repetition Num
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32  prvLion2PortPizzaArbiterConfigureTxQSliceNumWoCPUPortGet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 slicesNumberPerGop,
    IN GT_U32 txQCpuPortRepetitionNum
)
{
    GT_U32 txQSlicesNum;
    GT_U32 cpuPortPortGroupId;
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr = PRV_CPSS_PP_MAC(devNum);


    if (devPtr->noTraffic2CPU == GT_FALSE)
    {
        cpuPortPortGroupId = prvLion2PortPizzaArbiterTxQCpuPortPortGroupIdGet(devNum);
        if(portGroupId != cpuPortPortGroupId)
        {
            txQSlicesNum = slicesNumberPerGop;
        }
        else
        {/* to minimize tx queue time wasted on CPU port lets multiply number of
            slices on GOP where CPU connected by 10, and add slices for CPU;
            CPU port txq slice mapping already defined at init */
            txQSlicesNum = slicesNumberPerGop*txQCpuPortRepetitionNum;
        }
    }
    else /* devPtr->noTraffic2CPU == GT_TRUE,i.e. no traffic */
    {
        txQSlicesNum = slicesNumberPerGop;
    }
    return txQSlicesNum;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQAdditonalCpuSliceNumGet
*
* DESCRIPTION:
*      The function return number of slice for CPU outside regular pattern
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum           - pointer to device
*       cpuType          - cpu type
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32  prvLion2PortPizzaArbiterTxQAdditonalCpuSliceNumGet
(
    IN GT_U8                       devNum,
    IN CPSS_DXCH_CPUPORT_TYPE_ENT  cpuType
)
{
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr;
    GT_U32 cpuSliceNum;
    static GT_U32 Lion2_CpuSliceNumber[] = 
    {
        /* CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E */0,
        /* CPSS_DXCH_CPUPORT_TYPE_FIXED_E     */0,
        /* CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E  */1
    };

    devPtr = PRV_CPSS_PP_MAC(devNum),
    cpuSliceNum = Lion2_CpuSliceNumber[cpuType];
    if (devPtr->revision >= PRV_CPSS_DXCH_LION2_REV_B0) /* Lion2B0, Lion2B1 and above */
    {
        cpuSliceNum *=2;
    }
    return cpuSliceNum;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSliceNumInclCPUPortGet
*
* DESCRIPTION:
*      The function return number of slice for CPU outside regular pattern
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum           - pointer to device
*       portGroupId      - port group id 
*       slicesNumberPerGop - slices Number Per GOP
*       txQDefPtr          - definiton of TxQ
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32  prvLion2PortPizzaArbiterTxQSliceNumInclCPUPortGet
(
    IN GT_U8                       devNum,
    IN GT_U32                      slicesNumberPerGop,
    IN GT_BOOL                     isCpuAttached,
    IN CPSS_DXCH_TXQDEF_STC       *txQDefPtr
)
{
    GT_U32 txQSlicesNum;
    GT_U32 cpuSliceNum;

    if (0 == isCpuAttached)
    {
        txQSlicesNum = slicesNumberPerGop;
    }
    else
    {/* to minimize tx queue time wasted on CPU port lets multiply number of
        slices on GOP where CPU connected by 10, and add slice for CPU;
        CPU port txq slice mapping already defined at init */
        txQSlicesNum = slicesNumberPerGop*txQDefPtr->pizzaRepetitionNum;
        cpuSliceNum = prvLion2PortPizzaArbiterTxQAdditonalCpuSliceNumGet(devNum,txQDefPtr->cpuPortDef.type);
        txQSlicesNum += cpuSliceNum;
    }
    return txQSlicesNum;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSlicesNumSet
*
* DESCRIPTION:
*      The function sets slice number used in Pizza Arbiter for BM unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       slicesNum   - slices number to be set in pizza arbiter
*       isCpuAttached - is cpuu attached to core
*       txQDefPtr   - (pointer to) tx definitoon 
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSlicesNumSet
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   slicesNum,
    IN GT_BOOL  isCpuAttached,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
)
{
    GT_STATUS   rc;                 /* return code */
    GT_U32      txQSlicesNum;
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);



    txQSlicesNum =  prvLion2PortPizzaArbiterTxQSliceNumInclCPUPortGet(devNum,
                                                                      slicesNum,
                                                                      isCpuAttached,
                                                                      txQDefPtr);
    if(0 == txQSlicesNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc = prvLion2PortPizzaArbiterTxQDrvSlicesNumSet(devNum, portGroupId,txQSlicesNum);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSliceNumPerGopBytxQSliceNumGet
*
* DESCRIPTION:
*      The function gets total slice number and slice number per GOP in TxQ
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devPtr      - pointer to device 
*       portGroupId - port group id
*       txQSlicesNum   - slices number to be set in pizza arbiter
*       txQDefPtr   - (pointer to) tx definitoon 
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32  prvLion2PortPizzaArbiterTxQSliceNumPerGopBytxQSliceNumGet
(
    IN GT_U8                 devNum,
    IN GT_U32                portGroupId,
    IN GT_U32                txQSlicesNum,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
)
{
    GT_U32 slicesNumberPerGop;
    GT_U32 cpuPortPortGroupId;
    GT_U32 cpuSliceNum;
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr;

    devPtr = PRV_CPSS_PP_MAC(devNum),

    slicesNumberPerGop = txQSlicesNum;
    if (devPtr->noTraffic2CPU == GT_FALSE)
    {
        cpuPortPortGroupId = prvLion2PortPizzaArbiterTxQCpuPortPortGroupIdGet(devNum);
        if(portGroupId == cpuPortPortGroupId)
        {
            cpuSliceNum = prvLion2PortPizzaArbiterTxQAdditonalCpuSliceNumGet(devNum,txQDefPtr->cpuPortDef.type);
            txQSlicesNum -= cpuSliceNum;
            slicesNumberPerGop = txQSlicesNum / txQDefPtr->pizzaRepetitionNum;
        }
    }
    return slicesNumberPerGop;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSlicesNumGet
*
* DESCRIPTION:
*      The function get slice number used in Pizza Arbiter and strict priority state bit
*      for BM unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       txQDefPtr   - (ponter to)txq definiton
*
* OUTPUTS:
*       slicesNumPtr      - place to store slices number used in pizza arbiter
*       slicesNumInGopPtr - where to store slice number in GOP
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_BAD_PTR      - bad slicesNumPtr, enableStrictPriority4CPUPtr
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSlicesNumGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  CPSS_DXCH_TXQDEF_STC *txQDefPtr,
    OUT GT_U32               *slicesNumPtr,
    OUT GT_U32               *slicesNumInGopPtr
)
{
    GT_STATUS   rc;                 /* return code */
    GT_U32      regAddr;            /* register's address */
    GT_U32      fieldLen;           /* register's field length */
    GT_U32      txQSlicesNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (NULL == slicesNumPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == slicesNumInGopPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).scheduler.portArbiterConfig.portsArbiterConfig;
        fieldLen = 9;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.portArbiter.pizzaArbiterConfig;
        fieldLen = 8;
    }
    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            0, fieldLen, &txQSlicesNum);
    if (GT_OK != rc)
    {
        return rc;
    }
    *slicesNumPtr      =  txQSlicesNum;
    *slicesNumInGopPtr =  prvLion2PortPizzaArbiterTxQSliceNumPerGopBytxQSliceNumGet(/*IN*/ devNum,
                                                                                    /*IN*/ portGroupId,
                                                                                    /*IN*/ txQSlicesNum,
                                                                                    /*IN*/ txQDefPtr);
    if(0 == *slicesNumInGopPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}


static GT_STATUS CpuPortSliceListFind
(
    IN  CPSS_DXCH_PIZZA_SLICE_ID *sliceList,
    IN  CPSS_DXCH_PIZZA_SLICE_ID  sliceId,
    OUT GT_BOOL                  *isFoundPtr
)
{
    GT_U32 i;
    if (sliceList == NULL || isFoundPtr==NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    for (i = 0 ; sliceList[i] != CPSS_PA_INVALID_SLICE; i++)
    {
        if (sliceList[i] == sliceId)
        {
            *isFoundPtr = GT_TRUE;
            return GT_OK;
        }
    }
    *isFoundPtr = GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSliceOccupy
*
* DESCRIPTION:
*      The function occupy the slice (i.e. set the spicific slice
*      be asigned to specific port for BM unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to be occupied
*       portId      - local port id (inside portGroup) to which slice is assigned
*       slicesNumberPerGop - number of slices in pizza
*       txQDefPtr   - (ponter to)txq definiton
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_OUT_OF_RANGE - sliceId or portId are greater than available
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSliceOccupy
(
    IN GT_U8   devNum,
    IN GT_U32  portGroupId,
    IN GT_U32  sliceId,
    IN GT_PHYSICAL_PORT_NUM portId,
    IN GT_U32  slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
)
{
    GT_STATUS rc;
    GT_U32 txQSlicesNum;
    GT_U32 txQSlicesId;
    PRV_CPSS_GEN_PP_CONFIG_STC* pDev;
    GT_BOOL isFound;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    pDev = PRV_CPSS_PP_MAC(devNum);
    if (pDev->revision >= PRV_CPSS_DXCH_LION2_REV_B0)
    {
        /* Lion2B0, Lion2B1 and above */
        sliceId *= 2;
        slicesNumberPerGop *= 2;
    }

    if (sliceId >= slicesNumberPerGop)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }


    if (slicesNumberPerGop >= TxQ_TOTAL_AVAIBLE_SLICES_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* field is 4 bit length */
    if (portId >= PRV_CPSS_GEN_PP_MAX_PORTS_IN_PORT_GROUP_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    txQSlicesNum = prvLion2PortPizzaArbiterConfigureTxQSliceNumWoCPUPortGet(devNum,
                                                                            portGroupId,
                                                                            slicesNumberPerGop,
                                                                            txQDefPtr->pizzaRepetitionNum);

    if (txQSlicesNum >= TxQ_TOTAL_AVAIBLE_SLICES_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    /* occupy all relevant slice */
    for(txQSlicesId = sliceId; txQSlicesId < txQSlicesNum; txQSlicesId += slicesNumberPerGop)
    {
        if (pDev->noTraffic2CPU == GT_FALSE) /* traffic to CPU is allowed */
        {
            if (txQDefPtr->cpuPortDef.type == CPSS_DXCH_CPUPORT_TYPE_FIXED_E)
            {
                rc = CpuPortSliceListFind(&txQDefPtr->cpuPortDef.cpuPortSlices[0],(CPSS_DXCH_PIZZA_SLICE_ID)txQSlicesId, /*OUT*/&isFound);
                if (rc != GT_OK) 
                {
                    return rc;
                }
                if (isFound == GT_TRUE)  /* found in slice list of CPU --> skip */
                {
                    continue;
                }
            }
        }
        rc = prvLion2PortPizzaArbiterTxQDrvSliceStateSet(devNum,portGroupId,txQSlicesId,GT_TRUE,portId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSliceRelease
*
* DESCRIPTION:
*      The function release the slice (i.e. set the spicific slice
*      to be disable to any port) for BM unit)
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to be released
*       slicesNumberPerGop - number of slices in pizza
*       txQDefPtr   - (ponter to)txq definiton
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_OUT_OF_RANGE - sliceId or portId are greater than available
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSliceRelease
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId,
    IN GT_U32 slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
)
{
    GT_STATUS rc;
    GT_U32 txQSlicesNum;
    GT_U32 txQSlicesId;
    GT_BOOL isFound;
    PRV_CPSS_GEN_PP_CONFIG_STC* pDev;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    pDev = PRV_CPSS_PP_MAC(devNum);
    if (pDev->revision >= PRV_CPSS_DXCH_LION2_REV_B0)
    {
        /* Lion2B0, Lion2B1 and above */
        sliceId *= 2;
        slicesNumberPerGop *= 2;
    }

    if (sliceId >= slicesNumberPerGop)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }


    if (slicesNumberPerGop >= TxQ_TOTAL_AVAIBLE_SLICES_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }


    txQSlicesNum = prvLion2PortPizzaArbiterConfigureTxQSliceNumWoCPUPortGet(devNum,
                                                                            portGroupId,
                                                                            slicesNumberPerGop,
                                                                            txQDefPtr->pizzaRepetitionNum);

    if (txQSlicesNum >= TxQ_TOTAL_AVAIBLE_SLICES_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    /* release all relevant slice */
    for(txQSlicesId = sliceId; txQSlicesId < txQSlicesNum; txQSlicesId+= slicesNumberPerGop)
    {
        if (pDev->noTraffic2CPU == GT_FALSE) /* traffic to CPU is allowed */
        {
            if (txQDefPtr->cpuPortDef.type == CPSS_DXCH_CPUPORT_TYPE_FIXED_E)
            {
                rc = CpuPortSliceListFind(&txQDefPtr->cpuPortDef.cpuPortSlices[0],(CPSS_DXCH_PIZZA_SLICE_ID)txQSlicesId, /*OUT*/&isFound);
                if (rc != GT_OK) 
                {
                    return rc;
                }
                if (isFound == GT_TRUE)  /* found in slice list of CPU --> skip */
                {
                    continue;
                }
            }
        }
        rc = prvLion2PortPizzaArbiterTxQDrvSliceStateSet(devNum,portGroupId,txQSlicesId,GT_FALSE,0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSliceGetState
*
* DESCRIPTION:
*      The function gets the state of the slice (i.e. if slace is disable or enable
*      and if it enbaled to which port it is assigned) for BM unit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to get state of
*
* OUTPUTS:
*       isOccupiedPtr - place to store whether slice is occupied
*       portNumPtr   - plavce to store to which port slice is assigned
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_OUT_OF_RANGE - sliceId are greater than available
*       GT_BAD_PTR      - isOccupiedPtr or portNumPtr are NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSliceGetState
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId,
    OUT GT_BOOL              *isOccupiedPtr,
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_GEN_PP_CONFIG_STC* pDev;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    pDev = PRV_CPSS_PP_MAC(devNum);
    if (pDev->revision >= PRV_CPSS_DXCH_LION2_REV_B0)
    {
        /* Lion2B0, Lion2B1 and above */
        sliceId *= 2;
    }

    rc = prvLion2PortPizzaArbiterTxQDrvSliceStateGet(devNum,portGroupId,sliceId,/*OUT*/isOccupiedPtr,portNumPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvLion2PortPizzaArbiterTxQInit
*
* DESCRIPTION:
*      Init BM unit (sets number of slice to use , strict priority to CPU bit
*      and disables all slices of the device.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       portGroupId   - port group id
*       sliceNum2Init - number of slices to be used
*       txQDefPtr   - (ponter to)txq definiton
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_OUT_OF_RANGE - sliceNum2Init is greater than available
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQInit
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceNum2Init,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_GEN_PP_CONFIG_STC* pDev;
    GT_U32    i;
    GT_U32    cpuPortPortGroupId;
    GT_U32    txQSlicesNumWoExtCpu;

    GT_BOOL     isCpuAttached;


    CPSS_DXCH_PIZZA_SLICE_ID    extCpuSliceList[2] = 
    {
        (CPSS_DXCH_PIZZA_PORT_ID)CPSS_PA_INVALID_SLICE,
        (CPSS_DXCH_PIZZA_PORT_ID)CPSS_PA_INVALID_SLICE
    };

    CPSS_DXCH_PIZZA_SLICE_ID * cpuSliceListPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    pDev = PRV_CPSS_PP_MAC(devNum);

    rc = prvLion2PortPizzaArbiterTxQDrvInit(devNum,portGroupId, /*sliceNum2Init=*/0);
    if(rc != GT_OK)
    {
        return rc;
    }


    if (pDev->revision >= PRV_CPSS_DXCH_LION2_REV_B0)         /* Lion2B0, Lion2B1 and above */
    {
        sliceNum2Init *= 2;
    }


    cpuPortPortGroupId = prvLion2PortPizzaArbiterTxQCpuPortPortGroupIdGet(devNum);
    if (pDev->noTraffic2CPU == GT_FALSE)
    {
        isCpuAttached = cpuPortPortGroupId == portGroupId;
    }
    else
    {
        isCpuAttached = GT_FALSE;
    }

    rc = prvLion2PortPizzaArbiterTxQSlicesNumSet(devNum,portGroupId,sliceNum2Init,isCpuAttached,txQDefPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    if (pDev->noTraffic2CPU == GT_FALSE) /* traffic to CPU is allowed */
    {
        /* configure CPU txq slice */
        if(portGroupId == cpuPortPortGroupId)
        {
            if (txQDefPtr->cpuPortDef.type != CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E)
            {
                switch (txQDefPtr->cpuPortDef.type)
                {
                    case CPSS_DXCH_CPUPORT_TYPE_FIXED_E:
                        cpuSliceListPtr = &txQDefPtr->cpuPortDef.cpuPortSlices[0];
                    break;
                    case CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E:
                        txQSlicesNumWoExtCpu =  prvLion2PortPizzaArbiterConfigureTxQSliceNumWoCPUPortGet(
                                                               devNum,
                                                               portGroupId,
                                                               sliceNum2Init,
                                                               txQDefPtr->pizzaRepetitionNum);

                        extCpuSliceList[0] = (CPSS_DXCH_PIZZA_SLICE_ID)txQSlicesNumWoExtCpu;
                        cpuSliceListPtr = &extCpuSliceList[0];
                    break;
                    default:
                    {
                        cpuSliceListPtr = (CPSS_DXCH_PIZZA_SLICE_ID * )NULL;
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
                    }
                }
                if (cpuSliceListPtr != NULL)
                {

                    for (i = 0; cpuSliceListPtr[i] != CPSS_PA_INVALID_SLICE; i++)
                    {
                        rc = prvLion2PortPizzaArbiterTxQDrvSliceStateSet(devNum,portGroupId,cpuSliceListPtr[i],
                                                                         GT_TRUE,txQDefPtr->cpuPortDef.cpuPortNum);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
            }
        }
    }

    return GT_OK;
}
