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
* SersesRxOptVipsIf.c
*
* DESCRIPTION:
*		VIPS IF implementation needed by Serdes Optimizer system.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
******************************************************************************/

#include "SerdesRxOptAppIf.h"
#include <api/sysConf/gtSysConf.h>
#include <api/hwIf/gtHwIf.h>
#include <common/os/gtOs.h>

#include <EZprmIF.h>

MV_SERDES_TEST_DEV_TYPE mvDeviceInfoGetDevType
(
    GT_U32    devNum
)
{
    return Milos4;
}

GT_U32 getLaneFromAddress(GT_U32 address)
{
	return (address/0x1000);
}

GT_U32 getRegisterOffset(GT_U32 address)
{
	return (address & 0xFFF);
}

int genRegisterSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask)
{
	GT_U32                  regData = 0;
	EZapiIF_SerDesRegister  sSerDesData;  /* struct to access SERDES */
	EZstatus                retVal;

	if (mask != 0)
    {
		genRegisterGet(devNum, portGroup, address, &regData, 0);
		sSerDesData.uiData = (regData & ~mask) | (data & mask);
	}
	else
	{
		sSerDesData.uiData = data;
	}

	sSerDesData.uiSerDes  = getLaneFromAddress(address);
	sSerDesData.uiAddress = getRegisterOffset(address);
	

	retVal = EZprmIF_SerDesAccess( devNum,
								   TRUE,
								   &sSerDesData,
							       FALSE );
	if ( EZrc_IS_ERROR( retVal ) )
	{
		return retVal;
    }

	return GT_OK;
}

int genRegisterGet (GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask)
{
	EZapiIF_SerDesRegister  sSerDesData;  /* struct to access SERDES */
	EZstatus                retVal;
	
	sSerDesData.uiSerDes  = getLaneFromAddress(address);
	sSerDesData.uiAddress = getRegisterOffset(address);
	
	retVal = EZprmIF_SerDesAccess( devNum,
		                           FALSE,
		                           &sSerDesData,
		                           FALSE );
	if ( EZrc_IS_ERROR( retVal ) )
	{
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

	*data = (mask == 0) ? sSerDesData.uiData : (sSerDesData.uiData & mask);
	
    return GT_OK;
}

#include "../algorithm/SerdesRxOptimizer.h"
#include "../algorithm/serdesOptPrivate.h"

static GT_VOID tuningSysInit
(
    GT_VOID
)
{
    MV_SERDES_OPT_OS_FUNC_PTR funcPtrsStc; /* pointers to OS/CPU dependent system 
                                                calls */

    funcPtrsStc.osTimerWkPtr    = osTimerWkAfter;
    funcPtrsStc.osMemSetPtr     = osMemSet;
    funcPtrsStc.osFreePtr       = osFree;
    funcPtrsStc.osMallocPtr     = osMalloc;
    funcPtrsStc.osPrintfPtr     = osPrintf;
    funcPtrsStc.osMemCpyPtr     = osMemCpy;
    funcPtrsStc.osSemBCreatePtr = osSigSemBinCreate;
    funcPtrsStc.osSemDelPtr     = osSigSemDelete;
    funcPtrsStc.osSemSignalPtr  = osSigSemSignal;
    funcPtrsStc.osSemWaitPtr    = osSigSemWait;
    funcPtrsStc.osSprintfPtr    = osSprintf;
    funcPtrsStc.osRandPtr       = osRand;

    mvSerdesTunningSystemInit(&funcPtrsStc);

    return;
}

/* test function for all system configure and running */
void serdesOptSystemRun(
                        GT_U8 devNum, 
                        GT_U32 rxLanesBitMap, 
                        MV_PRBS_TYPE prbs,
                        int delay,
                        MV_SERDES_OPT_REPORT_TYPE reportType
)
{
    MV_SERDES_OPT_RESULTS testRes;
    MV_SERDES_OPT_MODE testMode = AccurAware;
    int i;

    /* Serdes test configuration */
    tuningSysInit();

	/* init report type */
	mvSerdesTunningReportTypeSet(reportType);

    for (i = 0; i < 32; i++)
    {
        if ((rxLanesBitMap >> i) & 1)
        {
            mvSerdesTunningRxLaneRegister(devNum, 0, i, prbs);
            mvSerdesTunningTxLaneRegister(devNum, 0, i, prbs);
        }
    }
    /* TX configuration on all lanes */
    mvSerdesTunningStartTx();    
    
    /* Start the Algorithm main procedure */
    mvSerdesTunningStart(testMode, delay);
    
    /* get results info */
    for (i = 0; i < 32; i++)
    {
        if ((rxLanesBitMap >> i) & 1)
        {
            testRes = mvSerdesTunningReport(devNum,0, i);
			if (testRes.signalDetectFail)
			{
				osPrintf("\nTunning FAILED due to signal detect for Device %d, Lane %d:", devNum, i);
				return;
			}
			osPrintf("\n\nTunning results for Device %d, Lane %d:", devNum, i);
			osPrintf("\nFFE R is \t%d", testRes.ffeRBestVal);
			osPrintf("\nFFE C is \t%d", testRes.ffeCBestVal);
			osPrintf("\nDFE is \t%d", testRes.dfeBestVal);
			osPrintf("\nSampler center is 0x%x", testRes.samplerCenter);
			osPrintf("\nSampler window is %d", testRes.samplerWindow);
			if (testRes.ffeCFittedAttenuation)
			{
				osPrintf("\nWARNING: FFE C fitted attenuation.");
			}
			if (testRes.ffeRFittedAttenuation)
			{
				osPrintf("\nWARNING: FFE R fitted attenuation.");
			}
			if (testRes.timeBerRisk)
			{
				osPrintf("\nWARNING: Time BER risk.");
			}
			if (testRes.volatgeBerRisk)
			{
				osPrintf("\nWARNING: Voltage BER risk.");
			}
			if (testRes.prbsNotLocked)
			{
				osPrintf("\nWARNING: PRBS not locked.");
			}
			osPrintf("\n");
        }
    }

	mvSerdesTunningSystemClose();
}

/* test function for all system configure and running */
void serdesOptDebugRun(
                        GT_U8 devNum, 
                        GT_U32 rxLanesLowBitMap, 
						GT_U32 rxLanesHighBitMap, 
                        MV_PRBS_TYPE prbs,
                        int delay,
                        MV_SERDES_OPT_REPORT_TYPE reportType,
						GT_U32 dfeMin, GT_U32 dfeMax,
						GT_U32 ffeRMin, GT_U32 ffeRMax, 
						GT_U32 ffeCMin, GT_U32 ffeCMax
						)
{
    MV_SERDES_OPT_RESULTS testRes;
    MV_SERDES_OPT_MODE testMode = AccurAware;
    int i, j;
	int lane;
	int bitMapArr[] = {rxLanesLowBitMap, rxLanesHighBitMap};

	osPrintf("Running Serdes Optimizer:\n\n");
	
    /* Serdes test configuration */
    tuningSysInit();

	dfeInfoConfig(dfeMin, dfeMax);
	ffeInfoConfig(ffeRMin, ffeRMax, ffeCMin, ffeCMax);
	
	/* init report type */
	mvSerdesTunningReportTypeSet(reportType);
	
	for (i = 0; i < 2; i++)
    {
		for (j = 0; j < 32; j++)
		{
			if ((bitMapArr[i] >> j) & 1)
			{
				lane = j + i * 32; 
				mvSerdesTunningRxLaneRegister(devNum, 0, lane, prbs);
				mvSerdesTunningTxLaneRegister(devNum, 0, lane, prbs);
			}
		}
    }
    /* TX configuration on all lanes */
    mvSerdesTunningStartTx();    
    
    /* Start the Algorithm main procedure */
    mvSerdesTunningStart(testMode, delay);
    
    /* get results info */
    for (i = 0; i < 32; i++)
    {
		for (j = 0; j < 32; j++)
		{
			if ((bitMapArr[i] >> j) & 1)
			{
				lane = j + i * 32; 

				testRes = mvSerdesTunningReport(devNum,0, lane);
				if (testRes.signalDetectFail)
				{
					osPrintf("\nTunning FAILED due to signal detect for Device %d, Lane %d:", devNum, lane);
					return;
				}
				osPrintf("\n\nTunning results for Device %d, Lane %d:", devNum, lane);
				osPrintf("\nFFE R is \t%d", testRes.ffeRBestVal);
				osPrintf("\nFFE C is \t%d", testRes.ffeCBestVal);
				osPrintf("\nDFE is \t%d", testRes.dfeBestVal);
				osPrintf("\nSampler center is 0x%x", testRes.samplerCenter);
				osPrintf("\nSampler window is %d", testRes.samplerWindow);
				if (testRes.ffeCFittedAttenuation)
				{
					osPrintf("\nWARNING: FFE C fitted attenuation.");
				}
				if (testRes.ffeRFittedAttenuation)
				{
					osPrintf("\nWARNING: FFE R fitted attenuation.");
				}
				if (testRes.timeBerRisk)
				{
					osPrintf("\nWARNING: Time BER risk.");
				}
				if (testRes.volatgeBerRisk)
				{
					osPrintf("\nWARNING: Voltage BER risk.");
				}
				osPrintf("\n");
			}
		}
    }

	mvSerdesTunningSystemClose();
}

void prbsStopConfig(GT_U8 devNum, GT_U32 laneNum)
{
	GT_U32 portGroup = 0;
    GT_U32 baseAddr = 0x09800200 + laneNum * 0x400;
	
    /* stop prbs */
    genRegisterSet(devNum, portGroup,baseAddr + 0x178, 0xE, 0);
}

/* test function for all system configure and running RX only */
void serdesOptSystem(
                        GT_U8 devNum, 
                        GT_U32 rxLanesLowBitMap, 
						GT_U32 rxLanesHighBitMap, 
                        MV_PRBS_TYPE prbs,
                        int delay,
                        MV_SERDES_OPT_REPORT_TYPE reportType
						)
{
    MV_SERDES_OPT_RESULTS testRes;
    MV_SERDES_OPT_MODE testMode = AccurAware;
    int i, j;
	int lane;
	int laneBitMap;
	int bitMapArr[] = {rxLanesLowBitMap, rxLanesHighBitMap};
	
    /* Serdes test configuration */
    tuningSysInit();
	
	/* init report type */
	mvSerdesTunningReportTypeSet(reportType);
	
    for (i = 0; i < 2; i++)
    {
		laneBitMap = bitMapArr[i];

		for (j = 0; j < 32; j++)
		{
			if ((laneBitMap >> j) & 1)
			{
				lane = j + i * 32; 
				mvSerdesTunningRxLaneRegister(devNum, 0, lane, prbs);
			}
		}
    }
    /* Start the Algorithm main procedure */
    mvSerdesTunningStart(testMode, delay);
    
    /* get results info */
    for (i = 0; i < 2; i++)
    {
		laneBitMap = bitMapArr[i];
		
		for (j = 0; j < 32; j++)
		{
			if ((laneBitMap >> j) & 1)
			{
				lane = j + i * 32; 

				testRes = mvSerdesTunningReport(devNum,0, lane);
				if (testRes.signalDetectFail)
				{
					osPrintf("\nTunning FAILED due to signal detect for Device %d, Lane %d:", devNum, lane);
					return;
				}
				osPrintf("\n\nTunning results for Device %d, Lane %d:", devNum, lane);
				osPrintf("\nFFE R is \t%d", testRes.ffeRBestVal);
				osPrintf("\nFFE C is \t%d", testRes.ffeCBestVal);
				osPrintf("\nDFE is \t%d", testRes.dfeBestVal);
				osPrintf("\nSampler center is 0x%x", testRes.samplerCenter);
				osPrintf("\nSampler window is %d", testRes.samplerWindow);
				if (testRes.ffeCFittedAttenuation)
				{
					osPrintf("\nWARNING: FFE C fitted attenuation.");
				}
				if (testRes.ffeRFittedAttenuation)
				{
					osPrintf("\nWARNING: FFE R fitted attenuation.");
				}
				if (testRes.timeBerRisk)
				{
					osPrintf("\nWARNING: Time BER risk.");
				}
				if (testRes.volatgeBerRisk)
				{
					osPrintf("\nWARNING: Voltage BER risk.");
				}
				if (testRes.prbsNotLocked)
				{
					osPrintf("\nWARNING: PRBS not locked.");
				}
				osPrintf("\n");
			}
		}
    }

	mvSerdesTunningSystemClose();
}
