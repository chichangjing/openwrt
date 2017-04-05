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
*       $Revision: 3 $
******************************************************************************/

#include <api/hwIf/gtHwIf.h>
#include <gtOs/gtGenTypes.h>
#include <serdesOpt/SerdesRxOptimizer.h>
#include <serdesOpt/private/SerdesRxOptAppIf.h>

MV_SERDES_TEST_DEV_TYPE mvDeviceInfoGetDevType
(
    GT_U32    devNum
)
{
    GT_EX_DEVICE  devType;

    /* get device type from system by laneCfg.devNum */
    sysConfGetDeviceType((unsigned char)devNum, &devType);
    switch (devType)
    {
    case GT_98CX8224_XG:
    case GT_98CX8248_XG:
        return LionB;
        break;
    default:
        return LionB;
        break;
    }

    return LionB;
}

int genRegisterSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask)
{
    if (mask == 0)
    {
        hwIfSetReg(devNum, address, data);
    }
    else
    {
        hwIfSetMaskReg(devNum, address,mask,data);
    }

	return 0;
}

int genRegisterGet (GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask)
{
        
    if (mask == 0)
    {
        hwIfGetReg((GT_U8)devNum, address, data);
    }
    else
    {
        hwIfGetMaskReg((GT_U8)devNum, address, mask, data);
    }
    
    return 0;
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
}

void serdesOptSystemRunShortMode(
                        GT_U8 devNum, 
                        GT_U32 rxLanesBitMap, 
                        MV_PRBS_TYPE prbs,
                        int delay,
                        MV_SERDES_OPT_REPORT_TYPE reportType
						)
{
    MV_SERDES_OPT_RESULTS testRes;
    MV_SERDES_OPT_MODE testMode = AccurAwareShort;
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


/* test function for all system configure and running */
void serdesOptDebugRun(
                        GT_U8 devNum, 
                        GT_U32 rxLanesBitMap, 
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
    int i;
	
    /* Serdes test configuration */
    tuningSysInit();

	dfeInfoConfig(dfeMin, dfeMax);
	ffeInfoConfig(ffeRMin, ffeRMax, ffeCMin, ffeCMax);
	
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
			osPrintf("\n");
        }
    }
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
        }
    }
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
}
