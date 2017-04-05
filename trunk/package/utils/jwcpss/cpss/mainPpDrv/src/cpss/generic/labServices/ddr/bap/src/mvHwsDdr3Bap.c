/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsDdr3Bap.c
*
* DESCRIPTION: DDR3 BAP Interface
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
******************************************************************************/

#include <mvHwsDdr3Bap.h>
#include <mvDdr3TrainingIp.h>
#include <mvDdr3TrainingIpDef.h>
#include <mvDdr3TrainingIpDb.h>
#include <mvDdr3TrainingIpFlow.h>

/************************** logging ******************************/


/*
 * Debug
 */
/*
#define GT_DEBUG_BAP
*/

#ifdef  GT_DEBUG_BAP
#define DEBUG_BAP(s)  {hwsPrintf s;}
#else
#define DEBUG_BAP(s)
#endif

/************************** definitions ***************************************/

#define TM_BASE_HIGH(BapId)							(0x20000 + (BapId * 0x8000))
#define TM_BASE_LOW									(0x0C000000)

#define TM_DATA_0_REG(regAddr)						(TM_BASE_LOW + regAddr)
#define TM_DATA_1_REG(regAddr)						(TM_BASE_LOW + regAddr + 4)

#define TM_GLOBAL_REG(index)   					    (TM_BASE_LOW + 0xFFFF8 + (4 * index))
#define TM_XCM_DRAM_REQUEST_CONTROL_REG				0x2C0
#define TM_XCM_DRAM_REQUEST_WRITE_DATA_REG(index)	(0x2D0 + (8 * index))
#define TM_XCM_DRAM_RESPONSE_STATUS_REG				0x2E0
#define TM_XCM_DRAM_RESPONSE_DATA_REG(index)		(0x2F0 + (8 * index))

#define TM_PHY_IFC_WRITE_PATH_REG					0x2800
#define TM_PHY_IFC_READ_PATH_REG					0x2810

#define TM_PHY_IFC_PATH_PHY_OFFSET(phyId)			((phyId) ? 0 : 8)
#define TM_PHY_IFC_PATH_PHY_MASK					0x1F

#define TM_CONTROL_REG_LOW_ADDR_MASK				0x7FFFFF
#define TM_CONTROL_REG_LOW_BANK_MASK				0x7F
#define TM_CONTROL_REG_LOW_BANK_OFF					23
#define TM_CONTROL_REG_HIGH_REQ_EN_OFF				8

/************************** enums *********************************************/

typedef enum
{
	DDR_BAP_READ_CMD  = 0,
	DDR_BAP_WRITE_CMD = 1
}MV_HWS_BAP_REQUEST_CMD;

/************************** global data ***************************************/

GT_U32 debugBapAccess = 0;
GT_U32 bapInitBitMask = 0xF;
GT_U32 bapDelay = 10;

extern GT_U32 maskTuneFunc;

/************************** internal function header **************************/


#ifdef __cplusplus
   extern "C"
   {
#endif

GT_STATUS    ddr3BapInitAllController
(
    GT_U8    devNum
);

#ifdef __cplusplus
   }
#endif

extern GT_STATUS    ddr3TipWriteMRSCmd
(
    GT_U32        devNum,
    GT_U32        *csMaskArr,
    GT_U32        cmd,
    GT_U32        data,
    GT_U32        mask
);

extern GT_STATUS ddr3Bc2UpdateTopologyMap
(
    GT_STATUS       devNum,
    MV_HWS_TOPOLOGY_MAP *topologyMap
);

GT_U32 ddr3BapGetDefaultAlgoFlags
(
	GT_U8  devNum
);

GT_STATUS ddr3BapUpdateTopologyMap
(
    GT_U8    devNum
);

/******************************************************************************/

/******************************************************************************
* BAP register read
*/
GT_STATUS    mvHwsDdr3BapRegRead
(
    GT_U8                 devNum,
    GT_U32                bapId,
    GT_U32                regAddr,
	GT_U32                *dataLow,
	GT_U32                *dataHigh,
	GT_U32                maskLow,
    GT_U32                maskHigh
)
{
#ifdef ASIC_SIMULATION
	devNum   = devNum;
	bapId    = bapId;
	maskHigh = maskHigh;
#else
	GT_U32 portGroup = devNum;
#endif

	*dataLow = regAddr;
	*dataHigh = maskLow;

	if (debugBapAccess >= 2)
    {
		DEBUG_BAP(("BAP Read (bapId %d TM base (High) 0x%x)  Addr 0x%x\n",
				 bapId, TM_BASE_HIGH(bapId), regAddr));
	}

#if !defined(ASIC_SIMULATION)
	/* Write address 23 MSBs */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, TM_GLOBAL_REG(0), TM_BASE_HIGH(bapId), MASK_ALL_BITS));

	/* Write don't care data */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, TM_GLOBAL_REG(1), PARAM_NOT_CARE, MASK_ALL_BITS));

	/* Read LSB */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, TM_DATA_0_REG(regAddr), dataLow, maskLow));

	/* Read MSB */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, TM_DATA_1_REG(regAddr), dataHigh, maskHigh));

	if (debugBapAccess >= 2)
    {
		DEBUG_BAP(("BAP Read Data 0x%x%08x\n", *dataHigh, *dataLow));
	}
#endif
	return GT_OK;
}

/******************************************************************************
* BAP register write
*/
GT_STATUS    mvHwsDdr3BapRegWrite
(
    GT_U8                 devNum,
    GT_U32                bapId,
    GT_U32                regAddr,
	GT_U32                dataLow,
    GT_U32                dataHigh,
	GT_U32                maskLow,
    GT_U32                maskHigh
)
{
#ifdef ASIC_SIMULATION
	devNum   = devNum;
	bapId    = bapId;
	regAddr  = regAddr;
	dataLow  = dataLow;
	dataHigh = dataHigh;
	maskLow  = maskLow;
	maskHigh = maskHigh;
#else
	GT_U32 portGroup = devNum;
    GT_U32 regDataHigh, regDataLow;
#endif

	if (debugBapAccess >= 1)
    {
        DEBUG_BAP(("BAP Write (bapId %d TM base (High) 0x%x)  0x%x: 0x%x%08x\n",
				 bapId, TM_BASE_HIGH(bapId), regAddr, dataHigh, dataLow));
    }

#if !defined(ASIC_SIMULATION)
	regDataLow  = dataLow;
	regDataHigh = dataHigh;

    if (maskLow != MASK_ALL_BITS || maskHigh != MASK_ALL_BITS)
    {
        CHECK_STATUS(mvHwsDdr3BapRegRead(devNum, bapId, regAddr, &regDataLow, &regDataHigh, MASK_ALL_BITS, MASK_ALL_BITS));

		regDataLow  = (regDataLow  & (~maskLow))  | ( dataLow  & maskLow );
		regDataHigh = (regDataHigh & (~maskHigh)) | ( dataHigh & maskHigh );
    }

	/* Write address 23 MSBs */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, TM_GLOBAL_REG(0), TM_BASE_HIGH(bapId), MASK_ALL_BITS));

	/* Write don't care data */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, TM_GLOBAL_REG(1), PARAM_NOT_CARE, MASK_ALL_BITS));

	/* Write data LSB */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, TM_DATA_0_REG(regAddr), dataLow, maskLow));

	/* Write data MSB */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, TM_DATA_1_REG(regAddr), dataHigh, maskHigh));

	if (debugBapAccess >= 1)
    {
        DEBUG_BAP(("\tWritten Data 0x%x%08x\n", regDataHigh, regDataLow));
    }
#endif
    return GT_OK;
}

/******************************************************************************
* Interface config
*/
GT_STATUS    ddr3BapInterfaceConfig
( 
    GT_U8                 devNum,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
	GT_U32                dataLow,
    GT_U32                dataHigh,
	GT_U32                maskLow, 
    GT_U32                maskHigh
)
{
	GT_U32 bapId,devId;

    hwsServerRegGetFuncPtr(devNum, 0x000F8240 ,  &devId);
	devId &= 0xFFFF;

	if ((devId == 0xBC00) /*CAELUM DEV_ID*/ && (interfaceId == 3))
        bapId = interfaceId - 1;
    else
        bapId = interfaceId;

	/* configure first TM instance/channel/PHY */
	CHECK_STATUS(mvHwsDdr3BapRegWrite(devNum, bapId, regAddr, dataLow, dataHigh, maskLow, maskHigh));

	/* configure second TM instance/channel/PHY */
	CHECK_STATUS(mvHwsDdr3BapRegWrite(devNum, bapId + 5, regAddr, dataLow, dataHigh, maskLow, maskHigh));

	return GT_OK;
}

/******************************************************************************
* DRAM address convert to BAP format
*/
GT_STATUS ddr3BapConvertDramAddress
(
	 GT_U32      baseAddress,
	 GT_U32      *address,
	 GT_U32      *bank
)
{

	*address = baseAddress & 0x7fffff;
	*bank = (baseAddress >> 23) & 0x7;

	DEBUG_BAP(("Address Convert: Address 0x%x    Bank 0x%x\n", *address, *bank));
	return GT_OK;
}

/******************************************************************************
* external memory access
*/
GT_STATUS    ddr3BapMemPrepareCmd
(  
	GT_U8					devNum,
	GT_U32					bapId,
	GT_U32					baseAddress,
	MV_HWS_BAP_REQUEST_CMD  cmd
)
{
	GT_U32      address;
	GT_U32      bank;
	GT_U32      regDataLow = 0;
	GT_U32      regDataHigh = 0;
	GT_U32      timeout = 100;

	CHECK_STATUS(ddr3BapConvertDramAddress(baseAddress, &address, &bank));
	
	/* set baseAddress (address & bank) */
	regDataLow |= (address & TM_CONTROL_REG_LOW_ADDR_MASK);			/* [21:0]  - address */
	regDataLow |= ((bank   & TM_CONTROL_REG_LOW_BANK_MASK) << TM_CONTROL_REG_LOW_BANK_OFF); /* [25:23] - bank */ 

	if(bapId >= 5)
	{
		regDataLow |= (1 << 26);
	}
	
	/* set read command */
	regDataHigh |= cmd;												/* [32:32] - Rd/Wr Selector */
	
	/* enable request */
	regDataHigh |= (GT_TRUE << TM_CONTROL_REG_HIGH_REQ_EN_OFF);		/* [40:40] - Request */
	
	/* write control register*/
	CHECK_STATUS(mvHwsDdr3BapRegWrite(devNum, bapId, TM_XCM_DRAM_REQUEST_CONTROL_REG, regDataLow, regDataHigh, MASK_ALL_BITS, MASK_ALL_BITS));
	
	/* wait for transaction to be sent */
	do 
	{
		CHECK_STATUS(mvHwsDdr3BapRegRead(devNum, bapId, TM_XCM_DRAM_REQUEST_CONTROL_REG, 
			&regDataLow, &regDataHigh, MASK_ALL_BITS, MASK_ALL_BITS));
	} while((((regDataHigh >> TM_CONTROL_REG_HIGH_REQ_EN_OFF) & 0x1) == GT_TRUE) && (--timeout > 0));
	
	if (timeout == 0)
	{
		DEBUG_BAP(("ddr3BapMemPrepareCmd: Read transaction has received timeout\n"));
		return GT_TIMEOUT;
	}

	return GT_OK;
}

/******************************************************************************
* external read from memory
*/
GT_STATUS    mvHwsDdr3BapMemRead
(  
	GT_U8	    devNum,
	GT_U32      bapId,
	GT_U32      baseAddress,
	GT_U32      data[4]
)
{
	GT_U32      regDataLow = 0;
	GT_U32      regDataHigh = 0;
	GT_U32      timeout = 100;

	CHECK_STATUS(ddr3BapMemPrepareCmd(devNum, bapId, baseAddress, DDR_BAP_READ_CMD));

	/* check status */
	timeout = 100;
	do 
	{
		CHECK_STATUS(mvHwsDdr3BapRegRead(devNum, bapId, TM_XCM_DRAM_RESPONSE_STATUS_REG, 
									&regDataLow, &regDataHigh, MASK_ALL_BITS, MASK_ALL_BITS));
	} while((regDataLow == 0) && (--timeout > 0));

	if (timeout == 0)
	{
		DEBUG_BAP(("ddr3BapMemRead: Read transaction has received timeout\n"));
		return GT_TIMEOUT;
	}
	else if ((regDataLow & 0x1) == 0)
	{
		DEBUG_BAP(("ddr3BapMemRead: Read transaction is not ready\n"));
		return GT_NOT_READY;
	}
	else if ((regDataLow & 0x3) == 0x3)
	{
		DEBUG_BAP(("ddr3BapMemRead: Read transaction has failed\n"));
		return GT_BAD_STATE;
	}
	
	/* data is ready to be read */
	/* read LSB 64 bits */
	CHECK_STATUS(mvHwsDdr3BapRegRead(devNum, bapId, TM_XCM_DRAM_RESPONSE_DATA_REG(0), 
								&data[0], &data[1], MASK_ALL_BITS, MASK_ALL_BITS));
	/* read MSB 64 bits */
	CHECK_STATUS(mvHwsDdr3BapRegRead(devNum, bapId, TM_XCM_DRAM_RESPONSE_DATA_REG(1), 
								&data[2], &data[3], MASK_ALL_BITS, MASK_ALL_BITS));

    return GT_OK;
}

GT_STATUS    mvHwsDdr3BapMemWrite
(  
	GT_U8       devNum,
	GT_U32      bapId,
	GT_U32      baseAddress,
	GT_U32      data[4]
) 
{
	/* write data to */
	/* write LSB 64 bits */
	CHECK_STATUS(mvHwsDdr3BapRegWrite(devNum, bapId, TM_XCM_DRAM_REQUEST_WRITE_DATA_REG(0), 
								 data[0], data[1], MASK_ALL_BITS, MASK_ALL_BITS));
	/* write MSB 64 bits */
	CHECK_STATUS(mvHwsDdr3BapRegWrite(devNum, bapId, TM_XCM_DRAM_REQUEST_WRITE_DATA_REG(1), 
								 data[2], data[3], MASK_ALL_BITS, MASK_ALL_BITS));

	CHECK_STATUS(ddr3BapMemPrepareCmd(devNum, bapId, baseAddress, DDR_BAP_WRITE_CMD));

    return GT_OK;
}

GT_STATUS    dd3BapUpdateControllerConfig
(
	GT_U8   devNum
)
{
	GT_U32 regData;
	GT_U32 mask;
	GT_U32 interfaceId;
	GT_U32 data[MAX_INTERFACE_NUM] = {0};
	MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);

	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
	{
		if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  0)
		{
			continue;
		}

		if (topologyMap == NULL)
		{
			DEBUG_BAP(("topologyMap is NULL\n"));
			return GT_BAD_PTR;
		}
		
		DEBUG_BAP(("\n\tInterface %d:\n", interfaceId));
		DEBUG_BAP(("\tCL is 0x%x CWL is 0x%x\n", 
					topologyMap->interfaceParams[interfaceId].casL, 
					topologyMap->interfaceParams[interfaceId].casWL));
		mask = (TM_PHY_IFC_PATH_PHY_MASK << TM_PHY_IFC_PATH_PHY_OFFSET(1)) | 
			   (TM_PHY_IFC_PATH_PHY_MASK << TM_PHY_IFC_PATH_PHY_OFFSET(0));
		
		/* Set Write Path */
		data[interfaceId] = (topologyMap->interfaceParams[interfaceId].casL - 1) + topologyMap->interfaceParams[interfaceId].casWL;
		regData = (data[interfaceId] << TM_PHY_IFC_PATH_PHY_OFFSET(1)) | 
				  (data[interfaceId] << TM_PHY_IFC_PATH_PHY_OFFSET(0));
		DEBUG_BAP(("\tWrite Path Value: 0x%x\n", regData));
		CHECK_STATUS(ddr3BapInterfaceConfig(devNum, interfaceId, TM_PHY_IFC_WRITE_PATH_REG, regData, 0x101, MASK_ALL_BITS, MASK_ALL_BITS)); 
		
		/* Set Read Path */
		CHECK_STATUS(mvHwsDdr3TipIFRead(devNum, ACCESS_TYPE_UNICAST, interfaceId, READ_DATA_SAMPLE_DELAY, 
										data, MASK_ALL_BITS));
		DEBUG_BAP(("\tReadSampleDelay: 0x%x (Read from TIP controller)\n", data[interfaceId]));

		data[interfaceId] += (topologyMap->interfaceParams[interfaceId].casL - 1);
		
		if(topologyMap->interfaceParams[interfaceId].memoryFreq == DDR_FREQ_667)
		{
			DEBUG_BAP(("\tFor 667MHz Read Path is reduced by 1\n", data[interfaceId]));
			data[interfaceId]--;
		}

		regData = (data[interfaceId] << TM_PHY_IFC_PATH_PHY_OFFSET(1)) | 
				  (data[interfaceId] << TM_PHY_IFC_PATH_PHY_OFFSET(0));
		DEBUG_BAP(("\tRead Path Value: 0x%x\n", regData));
		CHECK_STATUS(ddr3BapInterfaceConfig(devNum, interfaceId, TM_PHY_IFC_READ_PATH_REG, regData, 0, mask, MASK_ALL_BITS)); 
	}

	return GT_OK;
}

/* Fix TM CKE. */
GT_STATUS    ddr3BapCkeFix
(
    GT_U8    devNum
)
{
	GT_U32 data;
	GT_U32 interfaceId, busCnt;
	MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);
	GT_U32   csMask[MAX_INTERFACE_NUM];

	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
	{
		if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  0)
		{
			continue;
		}
		CHECK_STATUS(ddr3BapInterfaceConfig(devNum, interfaceId, 0x50, 0x2, PARAM_NOT_CARE, MASK_ALL_BITS, MASK_ALL_BITS)); 
	}

	/* wait 1ms */
	CHECK_STATUS(hwsOsExactDelayPtr(devNum, devNum, 1));
	
	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
	{
		if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) ==  0)
		{
			continue;
		}
		CHECK_STATUS(ddr3BapInterfaceConfig(devNum, interfaceId, 0x50, 0x0, PARAM_NOT_CARE, MASK_ALL_BITS, MASK_ALL_BITS)); 
	}
	
	/* calculate interface cs mask */
	for(interfaceId = 0; interfaceId < MAX_INTERFACE_NUM; interfaceId++)
	{
		/* cs enable is active low */
		csMask[interfaceId] = CS_BIT_MASK;
		if (IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, interfaceId) == GT_FALSE)
		{
			continue;
		}

		for (busCnt=0; busCnt<ddr3TipDevAttrGet(devNum, MV_ATTR_OCTET_PER_INTERFACE); busCnt++)
		{
			csMask[interfaceId] &= ~topologyMap->interfaceParams[interfaceId].asBusParams[busCnt].csBitmask;
		}
	}

	/* Set Multi CS (instead of Single CS) */
	DEBUG_BAP(("\tSet Multi CS\n"));
	CHECK_STATUS(mvHwsDdr3TipIFWrite(devNum, ACCESS_TYPE_MULTICAST, PARAM_NOT_CARE, CS_ENABLE_REG,  0x42, MASK_ALL_BITS));

	DEBUG_BAP(("\tSet MRS1 to 0x8\n"));
	CHECK_STATUS(ddr3TipWriteMRSCmd(devNum, csMask, MRS1_CMD, (1 << 3), (1 << 3)));
	
	/* Set ODT termination to BAP mode */
	if(hwsServerRegGetFuncPtr == NULL || hwsServerRegSetFuncPtr == NULL)
	{
		DEBUG_BAP(("hwsServerRegGetFuncPtr or hwsServerRegSetFuncPtr are NULL\n"));
		return GT_NOT_INITIALIZED;
	}

	hwsServerRegGetFuncPtr(devNum, 0xf8264, &data);
	data |= (1 << 19);
	hwsServerRegSetFuncPtr(devNum, 0xf8264, data);

	return GT_OK;
}


GT_STATUS    ddr3BapInitController
(
    GT_U8           devNum,
	GT_U32          interfaceId
)
{
	devNum = devNum;
	interfaceId = interfaceId;

	DEBUG_BAP(("*** Warning: ddr3BapInitController is not implemented\n"));
	return GT_OK;
}

/* Initialize all DDR controller.*/
GT_STATUS    ddr3BapInitAllController
(
    GT_U8     devNum
)
{
	devNum = devNum;

	DEBUG_BAP(("*** Warning: ddr3BapInitAllController is not implemented\n"));
	return GT_OK;
}

/* Update the topology map.*/
GT_STATUS ddr3BapUpdateTopologyMap
(
    GT_U8       devNum
)
{
	MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);
	DEBUG_BAP(("\tUpdate Topology\n"));
	CHECK_STATUS(ddr3Bc2UpdateTopologyMap(devNum, topologyMap));

	DEBUG_BAP(("\tLoad Topology\n"));
	CHECK_STATUS(mvHwsDdr3TipLoadTopologyMap(devNum, topologyMap));
	return GT_OK;
}

GT_STATUS ddr3BapSetAlgoFlow
(
	GT_U8       devNum,
	GT_U32		algoFlowMask
)
{
	devNum = devNum;

	DEBUG_BAP(("Setting algorithm flow to 0x%x\n", algoFlowMask));

	maskTuneFunc = algoFlowMask;

	return GT_OK;
}

GT_U32 ddr3BapGetDefaultAlgoFlags
(
	GT_U8   devNum
)
{
	GT_U32 algoFlowMask;

	devNum = devNum;

	algoFlowMask = INIT_CONTROLLER_MASK_BIT |
				   SET_MEDIUM_FREQ_MASK_BIT |
				   WRITE_LEVELING_MASK_BIT |
				   LOAD_PATTERN_2_MASK_BIT |
		           READ_LEVELING_MASK_BIT |
				   SET_TARGET_FREQ_MASK_BIT |
				   WRITE_LEVELING_TF_MASK_BIT |
				   READ_LEVELING_TF_MASK_BIT |
				   /*WRITE_LEVELING_SUPP_TF_MASK_BIT |*/
		           CENTRALIZATION_RX_MASK_BIT |
		           CENTRALIZATION_TX_MASK_BIT;

	return algoFlowMask;
}

/* Execute the DDR3 algorithm.*/
GT_STATUS mvHwsDdr3BapRunAlg
(
    GT_U8					  devNum,
    MV_HWS_BAP_ALGO_PARAMS    *bapAlgoParams
)
{
	GT_U32 data;
	GT_U32 algoFlowMask;
    /* InitCntrParam   initCntrPrm; */
#ifdef GT_DEBUG_BAP
	MV_HWS_TOPOLOGY_MAP *topologyMap = ddr3TipGetTopologyMap(devNum);
#endif

	if(bapInitBitMask & 0x1)
	{
		/* disable burst mode (to MG) */
		DEBUG_BAP(("Disable MG burst mode\n"));
		hwsServerRegGetFuncPtr(devNum, 0xf828C, &data);
		data |= (1 << 4);
		hwsServerRegSetFuncPtr(devNum, 0xf828C, data);

		if (bapAlgoParams->algoFlowMask == 0)
		{
			DEBUG_BAP(("Set default algo flags\n"));
			algoFlowMask = ddr3BapGetDefaultAlgoFlags(devNum);
		}
		else
		{
			algoFlowMask = bapAlgoParams->algoFlowMask;
		}

		DEBUG_BAP(("Algo flags are 0x%x\n", algoFlowMask));

		CHECK_STATUS(ddr3BapSetAlgoFlow(devNum, algoFlowMask));

		DEBUG_BAP(("Update topology map\n"));
		CHECK_STATUS(ddr3BapUpdateTopologyMap(devNum));

		DEBUG_BAP(("Active Interface Mask 0x%x\n", topologyMap->interfaceActiveMask));
	}
	
	if(bapInitBitMask & 0x2)
	{
		DEBUG_BAP(("(%s BAP Configuration)\n", (bapAlgoParams->algoType) ? "Static" : "Dynamic"));

		DEBUG_BAP(("Set MUX to Training IP\n"));
		CHECK_STATUS(mvHwsDdr3TipSelectDdrController(devNum, GT_TRUE));
		
		DEBUG_BAP(("Run Training IP Algo\n"));
		CHECK_STATUS(mvHwsDdr3TipRunAlg(devNum, bapAlgoParams->algoType));
	}

	if(bapInitBitMask & 0x4)
	{
		DEBUG_BAP(("Fix TM CKE\n"));
		CHECK_STATUS(ddr3BapCkeFix(devNum));
	}

	if(bapInitBitMask & 0x8)
	{
		DEBUG_BAP(("Update Controller Config\n"));
		CHECK_STATUS(dd3BapUpdateControllerConfig(devNum));
	}

	DEBUG_BAP(("Set MUX to BAP\n"));
	CHECK_STATUS(mvHwsDdr3TipSelectDdrController(devNum, GT_FALSE));

	return GT_OK;
}


