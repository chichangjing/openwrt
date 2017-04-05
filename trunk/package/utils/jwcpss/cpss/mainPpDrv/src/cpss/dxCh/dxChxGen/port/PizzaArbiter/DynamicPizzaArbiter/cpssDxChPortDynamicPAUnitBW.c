/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortDynamicPAUnitBW.c
*
* DESCRIPTION:
*       bobcat2 and higher dynamic (algorithmic) pizza arbiter 
*
* FILE REVISION NUMBER:
*       $Revision: 3$
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitBW.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitDrv.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAPortSpeedDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPAUnitBW.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiterWS.h>


static GT_STATUS prvCpssDxChPortPizzaArbiterSpeedGreaterCheck
(
	IN GT_U8     devNum,
	IN GT_U32    minSpeedMbps,
	OUT GT_BOOL *isGreaterPtr
)
{
	GT_STATUS rc;
    GT_PHYSICAL_PORT_NUM               portNum;
	CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
	GT_BOOL                            isInitilized;
	GT_U32                             speedInMBit;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

	CPSS_NULL_PTR_CHECK_MAC(isGreaterPtr);

	*isGreaterPtr = GT_TRUE;
    for (portNum = 0 ; portNum < PRV_CPSS_DXCH_PORT_NUM_CNS; portNum++)
    {
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

		if(portMapShadowPtr->portMap.mappingType != CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E)
		{
			rc = prvCpssDxChPortDynamicPAPortSpeedDBGet(devNum,portNum,/*OUT*/&isInitilized,&speedInMBit);
			if (isInitilized == GT_TRUE)
			{
				if (speedInMBit < minSpeedMbps)
				{
					*isGreaterPtr = GT_FALSE;
					cpssOsPrintf("\nport %d speed = %5d Mpbs < resolution (%5d Mbps)",portNum,speedInMBit,minSpeedMbps);  
				}
			}
		}
    }
	return GT_OK;
}

typedef struct 
{
	CPSS_DXCH_MIN_SPEED_ENT minSpeedMbpsEnt;
	GT_U32                  minSpeedMbps;
}CPSS_DXCH_MIN_SPEED_CON_STC;
static CPSS_DXCH_MIN_SPEED_CON_STC arr[] = 
{
     { CPSS_DXCH_MIN_SPEED_500_Mbps_E   ,    500 }
	,{ CPSS_DXCH_MIN_SPEED_1000_Mbps_E	,   1000 }
	,{ CPSS_DXCH_MIN_SPEED_2000_Mbps_E	,   2000 }
	,{ CPSS_DXCH_MIN_SPEED_5000_Mbps_E	,   5000 }
	,{ CPSS_DXCH_MIN_SPEED_10000_Mbps_E	,  10000 }
};


GT_U32 prvCpssDxChPortPizzaArbiterSpeedResolutionByEnmConv
(
	CPSS_DXCH_MIN_SPEED_ENT minSpeedMbpsEnt
)
{
	GT_U32 i;
	for (i = 0 ; i < sizeof(arr)/sizeof(arr[0]); i++)
	{
		if (arr[i].minSpeedMbpsEnt == minSpeedMbpsEnt)
		{
			return arr[i].minSpeedMbps;
		}
	}
	return 0;
}

CPSS_DXCH_MIN_SPEED_ENT prvCpssDxChPortPizzaArbiterSpeedResolutionBySpeedConv
(
	GT_U32 minSpeedMbps
)
{
	GT_U32 i;
	for (i = 0 ; i < sizeof(arr)/sizeof(arr[0]); i++)
	{
		if (arr[i].minSpeedMbps == minSpeedMbps)
		{
			return arr[i].minSpeedMbpsEnt;
		}
	}
	return CPSS_DXCH_MIN_SPEED_INVALID_E;
}


/*******************************************************************************
* internal_cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet
*
* DESCRIPTION:
*       set pipe BW for TxDMA, RXDMA, TxQ, Tx-FIFO, Eth-Tx-FIFO and slice resolution for TxQ
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum               - device number
*       pipeBandwithInGbps   - pipe bandwidth in GBits
*       minimalPortSpeedMBps - min port speed resolution in MBps
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, combination of BW and slice resolution
*       GT_FAIL         - on error
*
* COMMENTS:
*       a.	API does not change system BW, it only informs pizza arbiter about requested BW.
*       b.	API returns GT_BAD_PARAM, if number of available slices * resolution is smaller than requested BW,
*           ex: Bobcat2 has 340 slices, default resolution is 0.5G , max BW that can be supported is 340[slices] * 0.5[Gbps/slice] = 170 [Gpbs]
*           ex: if one like to support BW 240Gpbs, than ceil(240[Gpbs]/340[slices]) = 1[Gpbs/slice] = 1000 [Mpbs/slice]
*       c.	API checks whether there are ports with speed smaller than resolution. If any is present, API returns with error (GT_FAIL).
*       d.  API cpssDxChPortModeSpeedSet() will return error, if requested speed is smaller than speed resolution.
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet
(
    IN GT_U8                     devNum,
    IN GT_U32                    pipeBandwithInGbps,
    IN CPSS_DXCH_MIN_SPEED_ENT   minimalPortSpeedMBps
)
{
	GT_STATUS rc;
    CPSS_DXCH_PA_UNIT_ENT unitType;
	GT_U32                    bwEstInGBpsByMinSliceRes;
	GT_BOOL                   isGreater;
	GT_U32                    minPortSpeedMbps;
    GT_U32                    unitIdx;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr;
	
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


	if (pipeBandwithInGbps == 0)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
	}

	minPortSpeedMbps = prvCpssDxChPortPizzaArbiterSpeedResolutionByEnmConv(minimalPortSpeedMBps);
	if (minPortSpeedMbps == 0)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
	}

	bwEstInGBpsByMinSliceRes = PRV_CPSS_DXCH_BC2_PA_MAP_TOTAL_FLDS_REG_CNS* minPortSpeedMbps;
	if (bwEstInGBpsByMinSliceRes <  pipeBandwithInGbps*1000)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
	}

	rc = prvCpssDxChPortPizzaArbiterSpeedGreaterCheck(devNum,minPortSpeedMbps,/*OUT*/&isGreater);
    if (GT_OK != rc)
    {
        return rc;
    }
	if (isGreater == GT_FALSE) /* there are ports having speed smaller than minSpeed */
	{
		cpssOsPrintf("\ncpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet() failed");  
		cpssOsPrintf("\n    there are ports smaller than resolution");  
		cpssOsPrintf("\n");  
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
	}


    rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (unitIdx = 0 ; paWsPtr->prv_DeviceUnitListPtr[unitIdx] != CPSS_DXCH_PA_UNIT_UNDEFINED_E; unitIdx++)
    {
        unitType = paWsPtr->prv_DeviceUnitListPtr[unitIdx];
        if (paWsPtr->prv_unitListConfigByPipeBwSetBmp[unitType] == GT_TRUE) /* configure !!! */
        {
            rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWSet(devNum, unitType, pipeBandwithInGbps);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
    }

	PRV_CPSS_PP_MAC(devNum)->paMinSliceResolutionMbps = minPortSpeedMbps;
	return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet
*
* DESCRIPTION:
*       set pipe BW for TxDMA, RXDMA, TxQ, Tx-FIFO, Eth-Tx-FIFO and slice resolution for TxQ
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum               - device number
*       pipeBandwithInGbps   - pipe bandwidth in GBits
*       minimalPortSpeedMBps - min port speed resolution in MBps
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, combination of BW and slice resolution
*       GT_FAIL         - on error
*
* COMMENTS:
*       a.	API does not change system BW, it only informs pizza arbiter about requested BW.
*       b.	API returns GT_BAD_PARAM, if number of available slices * resolution is smaller than requested BW,
*           ex: Bobcat2 has 340 slices, default resolution is 0.5G , max BW that can be supported is 340[slices] * 0.5[Gbps/slice] = 170 [Gpbs]
*           ex: if one like to support BW 240Gpbs, than ceil(240[Gpbs]/340[slices]) = 1[Gpbs/slice] = 1000 [Mpbs/slice]
*       c.	API checks whether there are ports with speed smaller than resolution. If any is present, API returns with error (GT_FAIL).
*       d.  API cpssDxChPortModeSpeedSet() will return error, if requested speed is smaller than speed resolution.
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet
(
    IN GT_U8                     devNum,
    IN GT_U32                    pipeBandwithInGbps,
    IN CPSS_DXCH_MIN_SPEED_ENT   minimalPortSpeedMBps
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pipeBandwithInGbps, minimalPortSpeedMBps));

    rc = internal_cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(devNum, pipeBandwithInGbps, minimalPortSpeedMBps);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pipeBandwithInGbps, minimalPortSpeedMBps));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet
*
* DESCRIPTION:
*       get pipe BW for TxDMA, RXDMA, TxQ, Tx-FIFO, Eth-Tx-FIFO and slice resolution for TxQ
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum                       - device number
*
* OUTPUTS:
*       pipeBandwithInGbpsPtr           - pipe bandwidth in GBits
*       minimalSliceResolutionInMBpsPtr - min port speed resolution in MBps
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - wrong devNum, combination of BW and slice resolution
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet
(
    IN  GT_U8                     devNum,
    OUT GT_U32                   *pipeBandwithInGbpsPtr,
    OUT CPSS_DXCH_MIN_SPEED_ENT  *minimalPortSpeedResolutionInMBpsPtr
)
{
	GT_STATUS rc;
    PRV_CPSS_DXCH_PA_WORKSPACE_STC    *paWsPtr;
    CPSS_DXCH_PA_UNIT_ENT unitType;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

	if (pipeBandwithInGbpsPtr == NULL && minimalPortSpeedResolutionInMBpsPtr == NULL)
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
	}

	if (pipeBandwithInGbpsPtr != NULL)
	{
        rc = prvCpssDxChPortDynamicPizzaArbiterWSGet(devNum,/*OUT*/&paWsPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        unitType = paWsPtr->prv_unitListConfigByPipeBwSetPtr[0];
        rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWGet(devNum, unitType, /*OUT*/pipeBandwithInGbpsPtr);
		if (GT_OK != rc)
		{
			return rc;
		}
	}

	if (minimalPortSpeedResolutionInMBpsPtr != NULL)
	{
		*minimalPortSpeedResolutionInMBpsPtr = prvCpssDxChPortPizzaArbiterSpeedResolutionBySpeedConv(PRV_CPSS_PP_MAC(devNum)->paMinSliceResolutionMbps);
	}
	return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet
*
* DESCRIPTION:
*       get pipe BW for TxDMA, RXDMA, TxQ, Tx-FIFO, Eth-Tx-FIFO and slice resolution for TxQ
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum                       - device number
*
* OUTPUTS:
*       pipeBandwithInGbpsPtr           - pipe bandwidth in GBits
*       minimalSliceResolutionInMBpsPtr - min port speed resolution in MBps
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - wrong devNum, combination of BW and slice resolution
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet
(
    IN  GT_U8                     devNum,
    OUT GT_U32                   *pipeBandwithInGbpsPtr,
    OUT CPSS_DXCH_MIN_SPEED_ENT  *minimalPortSpeedResolutionInMBpsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pipeBandwithInGbpsPtr, minimalPortSpeedResolutionInMBpsPtr));

    rc = internal_cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet(devNum, pipeBandwithInGbpsPtr, minimalPortSpeedResolutionInMBpsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pipeBandwithInGbpsPtr, minimalPortSpeedResolutionInMBpsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

