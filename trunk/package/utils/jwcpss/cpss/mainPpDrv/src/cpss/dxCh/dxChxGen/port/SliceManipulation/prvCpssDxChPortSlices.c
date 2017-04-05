/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortSlices.c
*
* DESCRIPTION:
*       Pizza slices management functions
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h> 
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/prvCpssDxChPortSlices.h> 
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesRxDMA.h> 
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesRxDMACTU.h> 
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxDMA.h> 
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxDMACTU.h> 
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesBM.h> 
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxQ.h> 


typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_INIT_FUNC)
(
    IN GT_U8 devNum, 
    IN GT_U32 portGroupId,
    IN GT_U32 slicesNumberPerGop
);

typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_OCCUPY_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceIdx_inGop,
    IN GT_PHYSICAL_PORT_NUM localPort
);

typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_RELEASE_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceIdx_inGop
);

typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_GET_STATE_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceIdx_inGop,
    OUT GT_BOOL *isOccupiedPtr, 
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
);


typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_TXQDQ_INIT_FUNC)
(
    IN GT_U8 devNum, 
    IN GT_U32 portGroupId,
    IN GT_U32 slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
);

typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_TXQDQ_SLICE_OCCUPY_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceIdx_inGop,
    IN GT_PHYSICAL_PORT_NUM localPort,
    IN GT_U32 slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
);

typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_TXQDQ_SLICE_RELEASE_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceIdx_inGop,
    IN GT_U32 slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC * txQDefPtr
);

typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_TXQDQ_SLICE_GET_STATE_FUNC)
(
    IN   GT_U8  devNum,
    IN   GT_U32 portGroupId,
    IN   GT_U32 sliceIdx_inGop,
    OUT  GT_BOOL * isOccupiedPtr, 
    OUT  GT_PHYSICAL_PORT_NUM * portNumPtr
);


typedef struct prcDxChPizzaArbiterUnitFunc_STCT
{
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_INIT_FUNC            initFunc;
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_OCCUPY_FUNC    sliceOccupyFunc;
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_RELEASE_FUNC   sliceReleaseFunc;
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_GET_STATE_FUNC sliceGetStateFunc;    
}prcDxChPizzaArbiterUnitFunc_STC;

typedef struct prcDxChPizzaArbiterTxQDxFunc_STCT
{
    PRV_CPSS_DXCH_PIZZA_ARBITER_TXQDQ_INIT_FUNC            initFunc;
    PRV_CPSS_DXCH_PIZZA_ARBITER_TXQDQ_SLICE_OCCUPY_FUNC    sliceOccupyFunc;
    PRV_CPSS_DXCH_PIZZA_ARBITER_TXQDQ_SLICE_RELEASE_FUNC   sliceReleaseFunc;
    PRV_CPSS_DXCH_PIZZA_ARBITER_TXQDQ_SLICE_GET_STATE_FUNC sliceGetStateFunc;    
}prcDxChPizzaArbiterTxQDxFunc_STC;



typedef struct prcDxChPizzaArbiterDevFunc_STCT
{
    prcDxChPizzaArbiterUnitFunc_STC   rxDma;
    prcDxChPizzaArbiterUnitFunc_STC   rxDmaCTU;
    prcDxChPizzaArbiterUnitFunc_STC   txDma;
    prcDxChPizzaArbiterUnitFunc_STC   txDmaCTU;
    prcDxChPizzaArbiterUnitFunc_STC   BM;
    prcDxChPizzaArbiterTxQDxFunc_STC  TxQDq;
}prcDxChPizzaArbiterDevFunc_STC;


typedef struct prvDxChPizzaArbiterSliceManipSelectionByDevRevClock_STCT
{
    /* key */
    CPSS_PP_FAMILY_TYPE_ENT                         devFamilyType;
    GT_U32                                          devRevision;
    GT_U32                                          devCoreClock;  /* if devClock == 0 does not matter what clock is*/ 
    /* data */
    prcDxChPizzaArbiterDevFunc_STC               *  sliceManipFunc; 
}prvDxChPizzaArbiterSliceManipSelectionByDevRevClock_STC;


/*--------------------------------------------------------*/
/* Slice manipalation default                             */
/*--------------------------------------------------------*/
static prcDxChPizzaArbiterDevFunc_STC defaultSliceManip = 
{
     { prvLion2PortPizzaArbiterRxDMAInit,    prvLion2PortPizzaArbiterRxDMASliceOccupy,    prvLion2PortPizzaArbiterRxDMASliceRelease,    prvLion2PortPizzaArbiterRxDMASliceGetState    }
    ,{ prvLion2PortPizzaArbiterRxDMACTUInit, prvLion2PortPizzaArbiterRxDMACTUSliceOccupy, prvLion2PortPizzaArbiterRxDMACTUSliceRelease, prvLion2PortPizzaArbiterRxDMACTUSliceGetState }
    ,{ prvLion2PortPizzaArbiterTxDMAInit,    prvLion2PortPizzaArbiterTxDMASliceOccupy,    prvLion2PortPizzaArbiterTxDMASliceRelease,    prvLion2PortPizzaArbiterTxDMASliceGetState    }
    ,{ prvLion2PortPizzaArbiterTxDMACTUInit, prvLion2PortPizzaArbiterTxDMACTUSliceOccupy, prvLion2PortPizzaArbiterTxDMACTUSliceRelease, prvLion2PortPizzaArbiterTxDMACTUSliceGetState }
    ,{ prvLion2PortPizzaArbiterBMInit,       prvLion2PortPizzaArbiterBMSliceOccupy,       prvLion2PortPizzaArbiterBMSliceRelease,       prvLion2PortPizzaArbiterBMSliceGetState       }
    ,{ prvLion2PortPizzaArbiterTxQInit,      prvLion2PortPizzaArbiterTxQSliceOccupy,      prvLion2PortPizzaArbiterTxQSliceRelease,      prvLion2PortPizzaArbiterTxQSliceGetState      }
};

/*--------------------------------------------------------*/
/* Slice manipalation B0 360 MHz 12 Slice                 */
/*--------------------------------------------------------*/

static prcDxChPizzaArbiterDevFunc_STC devB0_360MHz_12Slices_SliceManip = 
{
     { prvLion2PortPizzaArbiterRxDMAInit,                  prvLion2PortPizzaArbiterRxDMASliceOccupy,                  prvLion2PortPizzaArbiterRxDMASliceRelease,                  prvLion2PortPizzaArbiterRxDMASliceGetState       }
    ,{ prvLion2PortPizzaArbiterRxDMACTUInit,               prvLion2PortPizzaArbiterRxDMACTUSliceOccupy,               prvLion2PortPizzaArbiterRxDMACTUSliceRelease,               prvLion2PortPizzaArbiterRxDMACTUSliceGetState    }
    ,{ prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_Init, prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceOccupy, prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceRelease, prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceGetState }
    ,{ prvLion2PortPizzaArbiterTxDMACTUInit,               prvLion2PortPizzaArbiterTxDMACTUSliceOccupy,               prvLion2PortPizzaArbiterTxDMACTUSliceRelease,               prvLion2PortPizzaArbiterTxDMACTUSliceGetState    }
    ,{ prvLion2PortPizzaArbiterBMInit,                     prvLion2PortPizzaArbiterBMSliceOccupy,                     prvLion2PortPizzaArbiterBMSliceRelease,                     prvLion2PortPizzaArbiterBMSliceGetState          }
    ,{ prvLion2PortPizzaArbiterTxQInit,                    prvLion2PortPizzaArbiterTxQSliceOccupy,                    prvLion2PortPizzaArbiterTxQSliceRelease,                    prvLion2PortPizzaArbiterTxQSliceGetState         }
};


#if 0
    static prvDxChPizzaArbiterSliceManipSelectionByDevRevClock_STC sliceManipSelec[] = 
    {
         { CPSS_PP_FAMILY_DXCH_LION2_E , REV_B0, 360,  /*OUT */ &devB0_360MHz_12Slices_SliceManip }
    };
#endif


static GT_STATUS prvLion2PortPizzaArbiterFuncSelection
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            sliceN_inGop,
    OUT prcDxChPizzaArbiterDevFunc_STC ** sliceManipFunPtrPtr
)
{
    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; /* pionter to device to be processed*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (sliceManipFunPtrPtr == NULL) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    devPtr = PRV_CPSS_PP_MAC(devNum);

    *sliceManipFunPtrPtr = &defaultSliceManip;
    if (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E && devPtr->revision >= PRV_CPSS_DXCH_LION2_REV_B0 && devPtr->coreClock == 360)
    {
        if (sliceN_inGop == 12)
        {
            *sliceManipFunPtrPtr = &devB0_360MHz_12Slices_SliceManip;
        }
    }
    return GT_OK;
}

/*--------------------------------------------------------------------------------*/
/*                                                                                */
/*--------------------------------------------------------------------------------*/
/*******************************************************************************
* prvLion2PortPizzaArbiterConfigureSlicesInit
*
* DESCRIPTION:
*      Init all arbiter' unit (sets number of slice to use , strict priority to CPU bit
*      and disables all slices of the device.)
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
*       slicesNumberPerGop - number of slices to be used
*       txQDefPtr     - tx pizza definiton (repetition counter how many times the pizza shall be repeated
*                                 for port group with CPU connected)
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_BAD_PTR      - on NULL pointer
*       GT_OUT_OF_RANGE - sliceNum2Init is greater than available
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterConfigureSlicesInit
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
)
{
    GT_STATUS   rc;                 /* return code */
    prcDxChPizzaArbiterDevFunc_STC * sliceManipFunPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    rc = prvLion2PortPizzaArbiterFuncSelection(devNum,slicesNumberPerGop,/*OUT*/&sliceManipFunPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = sliceManipFunPtr->rxDma.initFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/slicesNumberPerGop);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = sliceManipFunPtr->rxDmaCTU.initFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/slicesNumberPerGop);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->txDma.initFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/slicesNumberPerGop);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = sliceManipFunPtr->txDmaCTU.initFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/slicesNumberPerGop);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->BM.initFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/slicesNumberPerGop);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->TxQDq.initFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/slicesNumberPerGop,
									      txQDefPtr);
	if(rc != GT_OK)
	{
		return rc;
	}
	return GT_OK;
}


/*******************************************************************************
* prvLion2PortPizzaArbiterConfigureSliceOccupy
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
*       sliceIdx_inGop - slice to be occupied
*       localPort   - local port id (inside portGroup) to which slice is assigned
*       slicesNumberPerGop - slice number to be configured
*       txQDefPtr   - txQ definition 
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_BAD_PTR      - on NULL pointer
*       GT_OUT_OF_RANGE - sliceId or portId are greater than available
*       GT_ABORT        - on verification error
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterConfigureSliceOccupy
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceIdx_inGop,
    IN GT_PHYSICAL_PORT_NUM localPort,
    IN GT_U32 slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
)
{
    GT_STATUS rc;
	GT_BOOL                          isOccupied;
	GT_PHYSICAL_PORT_NUM             port2Check;
    prcDxChPizzaArbiterDevFunc_STC * sliceManipFunPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    rc = prvLion2PortPizzaArbiterFuncSelection(devNum,slicesNumberPerGop,/*OUT*/&sliceManipFunPtr);
    if(rc != GT_OK)
    {
        return rc;
    }


    rc = sliceManipFunPtr->rxDma.sliceOccupyFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,localPort);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->rxDma.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);

	if (rc != GT_OK) 
	{
		return rc;
	}
    if (isOccupied == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
    }
	if (port2Check != localPort) 
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}
	


	rc = sliceManipFunPtr->rxDmaCTU.sliceOccupyFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,localPort);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->rxDmaCTU.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
									  				  /*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
    if (isOccupied == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
    }
	if (port2Check != localPort) 
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}


	rc = sliceManipFunPtr->txDma.sliceOccupyFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,localPort);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->txDma.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
    if (isOccupied == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
    }
	if (port2Check != localPort) 
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}


	rc = sliceManipFunPtr->txDmaCTU.sliceOccupyFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,localPort);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->txDmaCTU.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
									  				  /*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
    if (isOccupied == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
    }
	if (port2Check != localPort) 
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}


	rc = sliceManipFunPtr->BM.sliceOccupyFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,localPort);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->BM.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
    if (isOccupied == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
    }
	if (port2Check != localPort) 
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}


    rc = sliceManipFunPtr->TxQDq.sliceOccupyFunc(/*IN*/devNum,/*IN*/portGroupId,
													/*IN*/sliceIdx_inGop,
													localPort,
													slicesNumberPerGop,
                                                    txQDefPtr);
	if(rc != GT_OK)
	{
		return rc;
	}

    rc = sliceManipFunPtr->TxQDq.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
    if (isOccupied == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
    }
	if (port2Check != localPort) 
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}
    return GT_OK;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterConfigureSliceRelease
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
*       sliceIdx_inGop - slice to be occupied
*       slicesNumberPerGop - slice number to be configured
*       txQDefPtr   - txQ definition 
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_BAD_PTR      - on NULL pointer
*       GT_OUT_OF_RANGE - sliceId or portId are greater than available
*       GT_ABORT        - on verification error
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterConfigureSliceRelease
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceIdx_inGop,
    IN GT_U32 slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
)
{
	GT_STATUS rc;
	GT_BOOL   isOccupied;
	GT_PHYSICAL_PORT_NUM port2Check;
    prcDxChPizzaArbiterDevFunc_STC * sliceManipFunPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    rc = prvLion2PortPizzaArbiterFuncSelection(devNum,slicesNumberPerGop,/*OUT*/&sliceManipFunPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = sliceManipFunPtr->rxDma.sliceReleaseFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop);
	if(rc != GT_OK)
	{
		return rc;
	}
    rc = sliceManipFunPtr->rxDma.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
	if (isOccupied == GT_TRUE)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}



	rc = sliceManipFunPtr->rxDmaCTU.sliceReleaseFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop);
	if(rc != GT_OK)
	{
		return rc;
	}
	rc = sliceManipFunPtr->rxDmaCTU.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
	if (isOccupied == GT_TRUE)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}


	rc = sliceManipFunPtr->txDma.sliceReleaseFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop);
	if(rc != GT_OK)
	{
		return rc;
	}
	rc = sliceManipFunPtr->txDma.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
	if (isOccupied == GT_TRUE)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}

    rc = sliceManipFunPtr->txDmaCTU.sliceReleaseFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop);
	if(rc != GT_OK)
	{
		return rc;
	}
	rc = sliceManipFunPtr->txDmaCTU.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													   /*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
	if (isOccupied == GT_TRUE)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}


	rc = sliceManipFunPtr->BM.sliceReleaseFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop);
	if(rc != GT_OK)
	{
		return rc;
	}
	rc = sliceManipFunPtr->BM.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
	if (isOccupied == GT_TRUE)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}


	rc = sliceManipFunPtr->TxQDq.sliceReleaseFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop, 
													 slicesNumberPerGop,
													 txQDefPtr);
	if(rc != GT_OK)
	{
		return rc;
	}

	rc = sliceManipFunPtr->TxQDq.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													/*OUT*/&isOccupied,&port2Check);
	if (rc != GT_OK) 
	{
		return rc;
	}
	if (isOccupied == GT_TRUE)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ABORTED, LOG_ERROR_NO_MSG);
	}
	return GT_OK;
}

/*******************************************************************************
* prvLion2PortPizzaArbiterConfigureSliceGetState
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
*       devNum         - device number
*       portGroupId    - port group id
*       sliceIdx_inGop - slice to be released
*
* OUTPUTS:
*       isOccupiedPtr - place to store whether slice is occupied
*        portNumPtr   - plavce to store to which port slice is assigned
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
GT_STATUS prvLion2PortPizzaArbiterConfigureSliceGetState
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceIdx_inGop,
    OUT GT_BOOL              *isOccupiedPtr, 
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
)
{
	GT_STATUS rc;
    prcDxChPizzaArbiterDevFunc_STC * sliceManipFunPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (isOccupiedPtr  == NULL || portNumPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = prvLion2PortPizzaArbiterFuncSelection(devNum,0,/*OUT*/&sliceManipFunPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

	rc = sliceManipFunPtr->rxDma.sliceGetStateFunc(/*IN*/devNum,/*IN*/portGroupId,/*IN*/sliceIdx_inGop,
													    /*OUT*/isOccupiedPtr, 
													    /*OUT*/portNumPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


