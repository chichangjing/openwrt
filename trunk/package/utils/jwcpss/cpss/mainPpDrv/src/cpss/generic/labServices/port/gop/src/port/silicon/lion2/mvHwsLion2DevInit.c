/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsLion2IfDevInit.c
*
* DESCRIPTION:
*       Lion2 specific HW Services init
*
* FILE REVISION NUMBER:
*       $Revision: 34 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <private/mvHwsPortPrvIf.h>
#include <mvHwsPortInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <silicon/lion2/mvHwsLion2PortIf.h>
#include <mac/mvHwsMacIf.h>
#include <mac/geMac/mvHwsGeMacIf.h>
#include <mac/geMac/mvHwsGeMacSgIf.h>
#include <mac/hglMac/mvHwsHglMacIf.h>
#include <mac/xgMac/mvHwsXgMacIf.h>
#include <mac/xlgMac/mvHwsXlgMacIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <pcs/gPcs/mvHwsGPcsIf.h>
#include <pcs/hglPcs/mvHwsHglPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcsV2If.h>
#include <pcs/xPcs/mvHwsXPcsIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/comPhyH/mvComPhyHIf.h>
#include <serdes/comPhyHRev2/mvComPhyHRev2If.h>

#define LION2_GIG_BASE (0x08800000)
#define LION2_XPCS_BASE (0x089c0200)
#define LION2_XLG_BASE  (0x088c0000)
#define LION2_MMPCS_BASE (0x088c0400)
#define LION2_SERDES_BASE (0x09800000)
#define SERDES_EXTERNAL_REGS (0x09800000)

#define GIG_IND_OFFSET   (0x1000)
#define XPCS_IND_OFFSET   (0x2000)
#define MMPCS_IND_OFFSET (0x1000)
#define MSM_IND_OFFSET (0x1000)
#define SERDES_IND_OFFSET  (0x400)

extern MV_HWS_PORT_INIT_PARAMS *hwsPortsLion2Params[];
/* init per device */
static GT_BOOL hwsInitDone[HWS_MAX_DEVICE_NUM] = {0};
static GT_BOOL lion2DbInitDone = GT_FALSE;

extern GT_STATUS hwsPortFixAlign90ExtMultipleLaneOptimization (
	GT_U8 devNum, GT_U32 portGroup, GT_U32 phyPortNum,
	MV_HWS_PORT_STANDARD portMode,  GT_U32 optAlgoMask);

/*******************************************************************************
* hwsPcsIfInit
*
* DESCRIPTION:
*       Init all supported PCS types relevant for Lion2 devices.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS pcsIfInit(GT_U8 devNum)
{
	MV_HWS_PCS_FUNC_PTRS *hwsPcsFuncsPtr;

	CHECK_STATUS(hwsPcsGetFuncPtr(devNum, &hwsPcsFuncsPtr));

	CHECK_STATUS(mvHwsGPcsIfInit(hwsPcsFuncsPtr));
	if (hwsDeviceSpecInfo[devNum].devType == Lion2A0)
	{
		CHECK_STATUS(mvHwsMMPcsIfInit(hwsPcsFuncsPtr));
	}
	else  /* Lion2 B0 */
	{
		CHECK_STATUS(mvHwsMMPcsV2IfInit(hwsPcsFuncsPtr));
	}
	CHECK_STATUS(mvHwsHglPcsIfInit(hwsPcsFuncsPtr));
	CHECK_STATUS(mvHwsXPcsIfInit(hwsPcsFuncsPtr));

	return GT_OK;
}

/*******************************************************************************
* hwsPcsIfClose
*
* DESCRIPTION:
*       Free all resources allocated by supported PCS types.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static void pcsIfClose(MV_HWS_DEV_TYPE devType)
{
	hwsGPcsIfClose();
	hwsMMPcsIfClose();
	hwsHglPcsIfClose();
	hwsXPcsIfClose();
	if (devType == Lion2A0)
	{
		hwsMMPcsIfClose();
	}
	else  /* Lion2 B0 */
	{
		hwsMMPcsRev2IfClose();
	}
}

/*******************************************************************************
* hwsMacIfInit
*
* DESCRIPTION:
*       Init all Lion2 supported MAC types.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS macIfInit(GT_U8 devNum)
{
	MV_HWS_MAC_FUNC_PTRS *hwsMacFuncsPtr;

	CHECK_STATUS(hwsMacGetFuncPtr(devNum, &hwsMacFuncsPtr));

	CHECK_STATUS(hwsGeMacIfInit(devNum, hwsMacFuncsPtr));
	CHECK_STATUS(hwsGeMacSgIfInit(devNum, hwsMacFuncsPtr));
	CHECK_STATUS(hwsHglMacIfInit(hwsMacFuncsPtr));
	CHECK_STATUS(hwsXgMacIfInit(devNum, hwsMacFuncsPtr));
	CHECK_STATUS(hwsXlgMacIfInit(hwsMacFuncsPtr));

	return GT_OK;
}

/*******************************************************************************
* hwsMacIfClose
*
* DESCRIPTION:
*       Free all resources allocated by supported MAC types.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static void macIfClose(void)
{
	hwsGeMacIfClose();
	hwsHglMacIfClose();
	hwsXgMacIfClose();
	hwsXlgMacIfClose();
}

/*******************************************************************************
* hwsSerdesIfInit
*
* DESCRIPTION:
*       Init all supported PCS types.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS serdesIfInit(GT_U8 devNum)
{
	MV_HWS_SERDES_FUNC_PTRS *hwsSerdesFuncsPtr;

	CHECK_STATUS(hwsSerdesGetFuncPtr(devNum, &hwsSerdesFuncsPtr));
	if (hwsDeviceSpecInfo[devNum].serdesType == COM_PHY_H)
	{
		CHECK_STATUS(mvHwsComHIfInit(hwsSerdesFuncsPtr));
	}
	else
	{
		CHECK_STATUS(mvHwsComHRev2IfInit(hwsSerdesFuncsPtr));
	}

	return GT_OK;
}

/*******************************************************************************
* hwsSerdesIfClose
*
* DESCRIPTION:
*       Free all resources allocated by supported serdes types.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static void serdesIfClose(MV_HWS_SERDES_TYPE type)
{
	if (type == COM_PHY_H)
	{
		mvHwsComHIfClose();
	}
	else
	{
		mvHwsComHRev2IfClose();
	}
}

static void hwsSerdesSpeedUpdates(GT_U8 devNum)
{
	GT_U32  portGroup;
	GT_U32  data;
	static GT_BOOL runUpdates = GT_TRUE;

	if (!runUpdates)
	{
		return;
	}

	/* e-fuse registers can be accessed only from core 1 */
	portGroup = 1;
	genRegisterGet(devNum, portGroup, 0x018f8500, &data, 0);

	if (data & 1)
	{
		switch ((data >> 6) & 7)
		{
		case 0:
			/* Rev43 with rx_init (just as bc use) */
			hwsChange1GSpeedCfgV43();
			break;
		case 1:
			/* Rev50 with rx_init (just as bc use) */
			break;
		case 2:
			/* Rev50 without rx_init, with latest MSI recomendation */
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x098, 0x1866);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x13C, 0xE026);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x01C, 0xF247);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x128, 0x0600);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x160, 0x0100);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x168, 0xE028);
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x128, 0x0600);
			break;
		case 3:
			/* Rev52 without rx_init same as Rev50 but with cont_vdd_calibration */
			/* with latest MSI recomendation */
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x098, 0x1866);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x13C, 0xE026);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x01C, 0xF247);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x160, 0x0100);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x108, 0x81DF);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x10C, 0x8CC0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x110, 0xF0C0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x114, 0x35F0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x134, 0x3040);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x128, 0x0600);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x168, 0xE028);

			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x108, 0xB1C0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x10C, 0x8CC8);
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x110, 0xF0C8);
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x114, 0x3400);
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x128, 0x0600);
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x134, 0x3040);
			break;
		case 7:
			/* latest MSI recomendation*/
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x098, 0x1866);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x13C, 0xE026);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x01C, 0xF247);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x128, 0x0600);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x160, 0x0100);

			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x128, 0x0600);

			/* read from EFuse and config */
			genRegisterGet(devNum,portGroup, 0x018F8504, &data, 0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x108, (data >> 16));
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x108, data & 0xFFFF);

			genRegisterGet(devNum,portGroup, 0x018F8508, &data, 0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x10C, (data >> 16));
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x10C, data & 0xFFFF);

			genRegisterGet(devNum,portGroup, 0x018F850C, &data, 0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x110, (data >> 16));
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x110, data & 0xFFFF);

			genRegisterGet(devNum,portGroup, 0x018F8518, &data, 0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x114, (data >> 16));
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x114, data & 0xFFFF);

			genRegisterGet(devNum,portGroup, 0x018F851C, &data, 0);
			hwsChangeSpeedCfgParams(SERDES_SPEED_1_25G, 0x134, (data >> 16));
			hwsChangeSpeedCfgParams(SERDES_SPEED_10_3125G, 0x134, data & 0xFFFF);
			break;
		default:
			break;
		}

		runUpdates = GT_FALSE;
	}
	else
	{
		runUpdates = GT_FALSE;
		/* not B1B silicon */
		return;
	}
}

/*******************************************************************************
* mvHwsRedundancyVectorGet
*
* DESCRIPTION:
*       Get SD vector.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS lion2RedundancyVectorGet
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	*sdVector
)
{
	GT_U32 data;
	GT_U32 accessAddr = MACRO_CONTROL;
	GT_U32 unitIndex;
	GT_U32 baseAddr;

	mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);

	accessAddr  = baseAddr + MACRO_CONTROL;
	CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
	*sdVector = data & 0xF;
	accessAddr  = baseAddr + MACRO_CONTROL + 4*unitIndex;
	CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
	*sdVector += ((data & 0xF) << 6);
	accessAddr  = baseAddr + MACRO_CONTROL + 8*unitIndex;
	CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
	*sdVector += ((data & 0xF) << 12);
	accessAddr  = baseAddr + MACRO_CONTROL + 12*unitIndex;
	CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
	*sdVector += ((data & 0xF) << 18);

	return GT_OK;
}

/*******************************************************************************
* hwsLion2IfInit
*
* DESCRIPTION:
*       Init all supported units needed for port initialization.
*       Must be called per device.
*
* INPUTS:
*       funcPtr - pointer to structure that hold the "os"
*                 functions needed be bound to HWS.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsLion2IfInit(GT_U8 devNum, HWS_OS_FUNC_PTR *funcPtr)
{
	GT_U32  portGroup = devNum;
	GT_U32  portNum;
	GT_U32  portMode;
	GT_U32  data, version;
	MV_HWS_DEV_TYPE devType;
	MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

	if (hwsInitDone[devNum])
	{
		return GT_ALREADY_EXIST;
	}

	if(lion2DbInitDone == GT_FALSE)
	{
		if (funcPtr == NULL)
			return GT_FAIL;

		if ((funcPtr->osTimerWkPtr == NULL) ||
			(funcPtr->osMemSetPtr == NULL) ||
			(funcPtr->osFreePtr == NULL) ||
			(funcPtr->osMallocPtr == NULL) ||
			(funcPtr->osExactDelayPtr == NULL) ||
			(funcPtr->sysDeviceInfo == NULL))
				return GT_BAD_PARAM;

		hwsOsExactDelayPtr = funcPtr->osExactDelayPtr;
		hwsOsTimerWkFuncPtr = funcPtr->osTimerWkPtr;
		hwsOsMemSetFuncPtr = funcPtr->osMemSetPtr;
		hwsOsFreeFuncPtr = funcPtr->osFreePtr;
		hwsOsMallocFuncPtr = funcPtr->osMallocPtr;
	}

	/* Init configuration sequence executer */
	mvCfgSeqExecInit();

	hwsDeviceSpecInfo[devNum].devNum = devNum;
	hwsDeviceSpecInfo[devNum].portsNum = 12;
	hwsDeviceSpecInfo[devNum].gopRev = GOP_40NM_REV1;
	hwsDeviceSpecInfo[devNum].lastSupPortMode = _48GBase_SR + 1;

	/* define Host to HWS connection type  - now Legacy mode*/
	hwsDeviceSpecInfo[devNum].ipcConnType = HOST2HWS_LEGACY_MODE;

	hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

	if(funcPtr != NULL)
	{
		funcPtr->sysDeviceInfo(devNum, &data, &version);
	}
	else
	{
		/* when doing device re-number, funcPtr will be NULL so we use registered function
		   (Lion2B0 can used since this function was registered for this device */
		hwsDevFunc[Lion2B0].deviceInfoFunc(devNum, &data, &version);
	}

	if (version > 0)
	{
		hwsDeviceSpecInfo[devNum].devType = Lion2B0;
		hwsDeviceSpecInfo[devNum].serdesType = COM_PHY_H_REV2;
		/* to change SERDES speed configuration parameters read EFuse data and config */
		hwsSerdesSpeedUpdates(devNum);

		/* Configures  device  (B0) handler functions */
		hwsDevFunc[hwsDeviceSpecInfo[devNum].devType].portAutoTuneStopFunc = mvHwsPortLion2AutoTuneStop;
		hwsDevFunc[hwsDeviceSpecInfo[devNum].devType].portAutoTuneSetWaFunc = NULL;
	}
	else
	{
		hwsDeviceSpecInfo[devNum].devType = Lion2A0;
		hwsDeviceSpecInfo[devNum].serdesType = COM_PHY_H;

		/* Configures  device  (A0) handler functions */
		hwsDevFunc[hwsDeviceSpecInfo[devNum].devType].portAutoTuneSetWaFunc = mvHwsPortTxAutoTuneUpdateWaExt;
		hwsDevFunc[hwsDeviceSpecInfo[devNum].devType].portAutoTuneStopFunc = NULL;
	}

	devType = hwsDeviceSpecInfo[devNum].devType;

	if(lion2DbInitDone == GT_FALSE)
	{
		hwsDevFunc[devType].deviceInfoFunc = funcPtr->sysDeviceInfo;

		hwsDevFunc[devType].redundVectorGetFunc = lion2RedundancyVectorGet;

		/* Configures port init / reset functions */
		hwsDevFunc[devType].devPortinitFunc  = mvHwsLion2PortInit;
		hwsDevFunc[devType].devPortResetFunc = mvHwsLion2PortReset;

		/* Configures  device  handler functions */
		hwsDevFunc[devType].pcsMarkModeFunc        = mvHwsLion2PCSMarkModeSet;
		hwsDevFunc[devType].portAutoTuneSetExtFunc = mvHwsLion2PortAutoTuneSetExt;
		hwsDevFunc[devType].portExtModeCfgFunc     = NULL;
		hwsDevFunc[devType].portExtModeCfgGetFunc  = mvHwsPortLion2ExtendedModeCfgGet;
		hwsDevFunc[devType].portLbSetWaFunc        = NULL;
		hwsDevFunc[devType].portFixAlign90Func     = NULL;

		/* Register silicon port modes parameters in the system */
		hwsInitPortsModesParam(devType, &hwsPortsLion2Params[0]);

		mvUnitInfoSet(devType, GEMAC_UNIT, LION2_GIG_BASE, GIG_IND_OFFSET);
		mvUnitInfoSet(devType, MMPCS_UNIT, LION2_MMPCS_BASE, MMPCS_IND_OFFSET);
		mvUnitInfoSet(devType, XPCS_UNIT, LION2_XPCS_BASE, XPCS_IND_OFFSET);
		mvUnitInfoSet(devType, HGLMAC_UNIT, LION2_MMPCS_BASE, MMPCS_IND_OFFSET);
		mvUnitInfoSet(devType, XLGMAC_UNIT, LION2_XLG_BASE, MSM_IND_OFFSET);
		mvUnitInfoSet(devType, SERDES_UNIT, LION2_SERDES_BASE, SERDES_IND_OFFSET);
	}

	/* Init all MAC units relevant for current device */
	CHECK_STATUS(macIfInit(devNum));

	/* Init all PCS units relevant for current device */
	CHECK_STATUS(pcsIfInit(devNum));

	/* Init SERDES unit relevant for current device */
	CHECK_STATUS(serdesIfInit(devNum));

	/* init active lines in ports DB */
	for (portNum = 0; (!lion2DbInitDone) && (portNum < HWS_CORE_PORTS_NUM(devNum)); portNum++)
	{
		for (portMode = 0; portMode < HWS_DEV_PORT_MODES(devNum); portMode++)
		{
			mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
		}
	}

    /* init active lines for extended port, since extended port parameters are taken from different DB */
    for (portNum = HWS_CORE_PORTS_NUM(devNum); (!lion2DbInitDone) && (portNum < HWS_CORE_PORTS_NUM(devNum)+2); portNum++)
    {
        for (portMode = 0; portMode < HWS_DEV_PORT_MODES(devNum); portMode++)
        {
            mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
        }
    }

	lion2DbInitDone = GT_TRUE;
	hwsInitDone[devNum] = GT_TRUE;

	return GT_OK;
}

/*******************************************************************************
* hwsLion2IfClose
*
* DESCRIPTION:
*       Free all resource allocated for ports initialization.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void hwsLion2IfClose(GT_U8 devNum)
{
	GT_U32  portGroup = devNum;
	GT_U32  portNum;
	GT_U32  portMode;
	GT_U32  i;

	if (hwsInitDone[devNum])
	{
		hwsInitDone[devNum] = GT_FALSE;
	}
	/* if there is no active device remove DB */
	for (i = 0; i < HWS_MAX_DEVICE_NUM; i++)
	{
		if (hwsInitDone[i])
		{
			return;
		}
	}

	lion2DbInitDone = GT_FALSE;

	serdesIfClose(hwsDeviceSpecInfo[devNum].devType);
	pcsIfClose(hwsDeviceSpecInfo[devNum].devType);
	macIfClose();

	/* free active lines allocated memory in ports DB */
	for (portNum = 0; (portNum < 12); portNum++)
	{
		for (portMode = 0; portMode < _12_5GBase_KR+1; portMode++)
		{
			mvHwsDestroyActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
		}
	}

	/* Unregister silicon port modes parameters in the system */
	hwsInitPortsModesParam(hwsDeviceSpecInfo[devNum].devType, 0);
}

