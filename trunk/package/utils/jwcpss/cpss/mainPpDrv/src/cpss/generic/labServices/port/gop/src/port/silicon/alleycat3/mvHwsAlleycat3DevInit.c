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
* mvHwsAlleycat3DevInit.c
*
* DESCRIPTION:
*       Bobcat specific HW Services init
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <mvHwsPortInitIf.h>
#include <mvHwsIpcApis.h>
#include <mvHwsServiceCpuFwIf.h>
#include <private/mvHwsPortPrvIf.h>
#include <mac/mvHwsMacIf.h>
#include <mvHwsPortCfgIf.h>
#include <silicon/alleycat3/mvHwsAlleycat3PortIf.h>
#include <mac/geMac/mvHwsGeMac28nmIf.h>
#include <mac/geMac/mvHwsGeMacSgIf.h>
#include <mac/xgMac/mvHwsXgMac28nmIf.h>
#include <mac/xlgMac/mvHwsXlgMac28nmIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <pcs/gPcs/mvHwsGPcs28nmIf.h>
#include <pcs/xPcs/mvHwsXPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcs28nmIf.h>
#include <serdes/mvHwsSerdes28nmPrvIf.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3If.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3Db.h>

/************************* definition *****************************************************/
#define ALLEYCAT3_NW_GIG_BASE	(0x12000000)
#define ALLEYCAT3_FB_GIG_BASE	(0x10200000)
#define ALLEYCAT3_NW_XPCS_BASE	(0x12180400)
#define ALLEYCAT3_FB_XPCS_BASE	ALLEYCAT3_NW_XPCS_BASE
#define ALLEYCAT3_NW_XLG_BASE	(0x120C0000)
#define ALLEYCAT3_NW_MPCS_BASE	(0x12180000)
#define ALLEYCAT3_FB_MPCS_BASE	ALLEYCAT3_NW_MPCS_BASE

#define ALLEYCAT3_SERDES_BASE	(0x13000000)
#define ALLEYCAT3_SERDES_PHY_BASE	(0x13000800)

#define IND_OFFSET (0x1000)

#define AC3_MAX_CORE_CLOCK 7

/************************* Globals *******************************************************/
GT_U32 alleycat3CoreClkOptions[AC3_MAX_CORE_CLOCK] = {290, 250, 220, 167, 200, 133, 360};

extern MV_HWS_PORT_INIT_PARAMS *hwsPortsAlleycat3Params[];

/* init per device */
static GT_BOOL hwsInitDone[HWS_MAX_DEVICE_NUM] = {0};
static GT_BOOL alleycat3DbInitDone = GT_FALSE;

/************************* pre-declaration ***********************************************/


extern GT_STATUS hwsAlleycat3Ports1To23Cfg (GT_U8 devNum, GT_U32 portGroup);

static GT_STATUS mvHwsAlleycat3CoreClockGet (GT_U8 devNum, GT_U32 *coreClkPtr);

/*****************************************************************************************/

/*******************************************************************************
* hwsAc3PcsIfInit
*
* DESCRIPTION:
*       Init all supported PCS types relevant for devices.
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
static GT_STATUS hwsAc3PcsIfInit(GT_U8 devNum)
{
	MV_HWS_PCS_FUNC_PTRS *hwsPcsFuncsPtr;

	CHECK_STATUS(hwsPcsGetFuncPtr(devNum, &hwsPcsFuncsPtr));

	CHECK_STATUS(mvHwsXPcsIfInit(hwsPcsFuncsPtr));
	CHECK_STATUS(mvHwsGPcs28nmIfInit(hwsPcsFuncsPtr));
	CHECK_STATUS(mvHwsMMPcs28nmIfInit(hwsPcsFuncsPtr));

	return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* hwsAc3PcsIfClose
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
static void hwsAc3PcsIfClose(void)
{
	hwsXPcsIfClose();
	hwsGPcs28nmIfClose();
	hwsMMPcs28nmIfClose();
}
#endif

/*******************************************************************************
* hwsMacIfInit
*
* DESCRIPTION:
*       Init all supported MAC types.
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
static GT_STATUS hwsAc3MacIfInit(GT_U8 devNum)
{
	MV_HWS_MAC_FUNC_PTRS *hwsMacFuncsPtr;

	CHECK_STATUS(hwsMacGetFuncPtr(devNum, &hwsMacFuncsPtr));

	CHECK_STATUS(hwsGeMac28nmIfInit(devNum, hwsMacFuncsPtr));
	CHECK_STATUS(hwsGeMacSgIfInit(devNum, hwsMacFuncsPtr));
	CHECK_STATUS(hwsXgMac28nmIfInit(devNum, hwsMacFuncsPtr));
	CHECK_STATUS(hwsXlgMac28nmIfInit(devNum, hwsMacFuncsPtr));

	return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* hwsAc3MacIfClose
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
static void hwsAc3MacIfClose(void)
{
	hwsGeMac28nmIfClose();
	hwsXgMac28nmIfClose();
	hwsXlgMac28nmIfClose();
}
#endif

/*******************************************************************************
* mvHwsAlleycat3ClkSelCfg
*
* DESCRIPTION:
*       Configures the DP/Core Clock Selector on port according to core clock,
*		port mode and port number.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsAlleycat3ClkSelCfg
(
	GT_U8	devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum,
	MV_HWS_PORT_STANDARD	portMode
)
{
	MV_HWS_PORT_INIT_PARAMS   *curPortParams;
	GT_U32  coreClk;

	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

	/* read core clock */
	CHECK_STATUS(mvHwsAlleycat3CoreClockGet(devNum, &coreClk));

	if ((portMode == SGMII2_5) || (portMode == SGMII))
	{
		if ((portMode == SGMII2_5) && (coreClk < 312))
		{
			/* Set DP Clock Selector */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, curPortParams->portMacNumber,
							GIG_PORT_MAC_CONTROL_REGISTER4, (1 << 5), (1 << 5)));
		}
		else
		{
			/* Set Core Clock Selector */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, curPortParams->portMacNumber,
							GIG_PORT_MAC_CONTROL_REGISTER4, 0, (1 << 5)));
		}
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsAlleycat3CoreClockGet
*
* DESCRIPTION:
*       Return S@R core clock frequency.
*
* INPUTS:
*		devNum - system device number
*
* OUTPUTS:
*       coreClkPtr - core clock frequency
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS mvHwsAlleycat3CoreClockGet
(
	GT_U8	devNum,
	GT_U32	*coreClkPtr
)
{
	GT_U32	data;

	/* get the PLL 0 Config value from server */
	hwsServerRegGetFuncPtr(devNum, SERVER_PLL0_CONFIG_REG, &data);
	data = (data >> 21) & 0x7;	 /* bits 21-23 */

	if (data >= AC3_MAX_CORE_CLOCK)
	{
		return GT_BAD_STATE;
	}

	*coreClkPtr = alleycat3CoreClkOptions[data];

	return  GT_OK;
}

/*******************************************************************************
* hwsAc3SerdesIfInit
*
* DESCRIPTION:
*       Init all supported SERDES types.
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
static GT_STATUS hwsAc3SerdesIfInit(GT_U8 devNum)
{
	MV_HWS_SERDES_FUNC_PTRS *hwsSerdesFuncsPtr;

	CHECK_STATUS(hwsSerdesGetFuncPtr(devNum, &hwsSerdesFuncsPtr));
	CHECK_STATUS(mvHwsComH28nmRev3IfInit(hwsSerdesFuncsPtr));

	return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* hwsAc3SerdesIfClose
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
static void hwsAc3serdesIfClose(void)
{
	mvHwsComH28nmRev3IfClose();
}
#endif

/*******************************************************************************
* mvHwsAlleycat3IfInit
*
* DESCRIPTION:
*       Init all software related DB: DevInfo, Port (Port modes, MAC, PCS and SERDES)
*		and address mapping.
*       Must be called per device.
*
* INPUTS:
*       funcPtr - pointer to structure that hold the "OS"
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
GT_STATUS mvHwsAlleycat3IfInit(GT_U8 devNum, HWS_OS_FUNC_PTR *funcPtr)
{
	GT_U32	portGroup = devNum;
	GT_U32  portNum;
	GT_U32  portMode;
	GT_U32  data, version;
	MV_HWS_DEV_TYPE devType;
	MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

	if (hwsInitDone[devNum])
	{
		return GT_ALREADY_EXIST;
	}

	if(alleycat3DbInitDone == GT_FALSE)
	{
		if (funcPtr == NULL)
			return GT_BAD_PARAM;

		if ((funcPtr->osTimerWkPtr == NULL) || (funcPtr->osMemSetPtr == NULL) ||
			(funcPtr->osFreePtr == NULL) || (funcPtr->osMallocPtr == NULL) ||
			(funcPtr->osExactDelayPtr == NULL) || (funcPtr->sysDeviceInfo == NULL) ||
			(funcPtr->osMemCopyPtr == NULL) ||
			(funcPtr->serdesRegSetAccess == NULL) ||(funcPtr->serdesRegGetAccess == NULL) ||
			(funcPtr->serverRegSetAccess == NULL) || (funcPtr->serverRegGetAccess == NULL))
		{
			return GT_BAD_PARAM;
		}

        if (funcPtr->osMicroDelayPtr == NULL)
        {
            hwsOsMicroDelayPtr = hwsOsLocalMicroDelay;
        }
        else
        {
            hwsOsMicroDelayPtr = funcPtr->osMicroDelayPtr;
        }
		hwsOsExactDelayPtr = funcPtr->osExactDelayPtr;
		hwsOsTimerWkFuncPtr = funcPtr->osTimerWkPtr;
		hwsOsMemSetFuncPtr = funcPtr->osMemSetPtr;
		hwsOsFreeFuncPtr = funcPtr->osFreePtr;
		hwsOsMallocFuncPtr = funcPtr->osMallocPtr;
		hwsSerdesRegSetFuncPtr = funcPtr->serdesRegSetAccess;
		hwsSerdesRegGetFuncPtr = funcPtr->serdesRegGetAccess;
		hwsServerRegSetFuncPtr = funcPtr->serverRegSetAccess;
		hwsServerRegGetFuncPtr = funcPtr->serverRegGetAccess;
		hwsOsMemCopyFuncPtr = funcPtr->osMemCopyPtr;
	}

	hwsDeviceSpecInfo[devNum].devType = Alleycat3A0;
	hwsDeviceSpecInfo[devNum].devNum = devNum;
	hwsDeviceSpecInfo[devNum].portsNum = 32;
	hwsDeviceSpecInfo[devNum].gopRev = GOP_28NM_REV1;
	hwsDeviceSpecInfo[devNum].lastSupPortMode = _5GBaseR + 1;

	devType = hwsDeviceSpecInfo[devNum].devType;

	hwsDeviceSpecInfo[devNum].serdesType = COM_PHY_28NM;

	/* define Host to HWS connection type  - now Legacy mode*/
	hwsDeviceSpecInfo[devNum].ipcConnType = HOST2HWS_LEGACY_MODE;

	/* Init HWS Firmware */
	if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
		CHECK_STATUS(mvHwsServiceCpuFwInit(devNum));

	hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

	if (hwsDevFunc[devType].deviceInfoFunc == NULL)
	{
		hwsDevFunc[devType].deviceInfoFunc = funcPtr->sysDeviceInfo;
	}

	/* get device id and version */
	hwsDevFunc[devType].deviceInfoFunc(devNum, &data, &version);

	/* AlleyCat3 has no redundancy lanes, thus the value of redundVectorGetFunc is NULL */
	hwsDevFunc[devType].redundVectorGetFunc = NULL;

	/* Configures the DP/Core Clock Selector on port */
	hwsDevFunc[devType].clkSelCfgGetFunc = mvHwsAlleycat3ClkSelCfg;

	/* Configures port init / reset functions */
	hwsDevFunc[devType].devPortinitFunc  = mvHwsAlleycat3PortInit;
	hwsDevFunc[devType].devPortResetFunc = mvHwsAlleycat3PortReset;

    /* Configures  device handler functions */
    hwsDevFunc[devType].portAutoTuneSetExtFunc = mvHwsAlleycat3PortAutoTuneSetExt;
    hwsDevFunc[devType].portExtModeCfgFunc     = mvHwsAlleycat3PortExtendedModeCfg;
    hwsDevFunc[devType].portExtModeCfgGetFunc  = mvHwsAlleycat3PortExtendedModeCfgGet;
    hwsDevFunc[devType].portFixAlign90Func     = mvHwsAlleycat3PortFixAlign90Ext;
    hwsDevFunc[devType].pcsMarkModeFunc        = NULL;
    hwsDevFunc[devType].portLbSetWaFunc        = NULL;
    hwsDevFunc[devType].portAutoTuneSetWaFunc  = NULL;
    hwsDevFunc[devType].portAutoTuneStopFunc   = NULL;

	/* Init all MAC units relevant for current device */
	CHECK_STATUS(hwsAc3MacIfInit(devNum));

	/* Init all PCS units relevant for current device */
	CHECK_STATUS(hwsAc3PcsIfInit(devNum));

	/* Init SERDES unit relevant for current device */
	CHECK_STATUS(hwsAc3SerdesIfInit(devNum));

	/* Update HWS After Firmware load and init */
	if (mvHwsServiceCpuEnableGet(devNum))
		CHECK_STATUS(mvHwsServiceCpuFwPostInit(devNum));

	/* Init silicon specific base address and index for specified unit */
	mvUnitInfoSet(devType, GEMAC_UNIT, ALLEYCAT3_NW_GIG_BASE, IND_OFFSET);
	mvUnitInfoSet(devType, MMPCS_UNIT, ALLEYCAT3_NW_MPCS_BASE, IND_OFFSET);
	mvUnitInfoSet(devType, XPCS_UNIT, ALLEYCAT3_NW_XPCS_BASE, IND_OFFSET);
	mvUnitInfoSet(devType, XLGMAC_UNIT, ALLEYCAT3_NW_XLG_BASE, IND_OFFSET);
	mvUnitInfoSet(devType, SERDES_UNIT, ALLEYCAT3_SERDES_BASE, IND_OFFSET);
	mvUnitInfoSet(devType, SERDES_PHY_UNIT, ALLEYCAT3_SERDES_PHY_BASE, IND_OFFSET);

	/* Register silicon port modes parameters in the system */
	hwsInitPortsModesParam(devType, &hwsPortsAlleycat3Params[0]);

	if(alleycat3DbInitDone == GT_FALSE)
	{
		/* Initialize port elements DB for AlleyCat silicon */
		CHECK_STATUS(hwsAlleycat3Ports1To23Cfg(devNum, portGroup));

		/* init active lanes in ports DB */
		for (portNum = 0; (!alleycat3DbInitDone) && (portNum < HWS_CORE_PORTS_NUM(devNum)); portNum++)
		{
			if (hwsPortModeParamsGet(devNum, portGroup, portNum, SGMII /* dummy mode */) == NULL)
			{
				continue;
			}

			for (portMode = 0; portMode < HWS_DEV_PORT_MODES(devNum); portMode++)
			{
				mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
			}
		}
	}

	/* DDR training initialization */
	/*CHECK_STATUS(hwsTrainingIpIfInit(devNum));*/
	alleycat3DbInitDone = GT_TRUE;

	hwsInitDone[devNum] = GT_TRUE;

	return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* hwsAlleycat3IfClose
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
void hwsAlleycat3IfClose(GT_U8 devNum)
{
	GT_U32 portGroup = devNum;
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
	alleycat3DbInitDone = GT_FALSE;

	hwsAc3serdesIfClose();
	hwsAc3PcsIfClose();
	hwsAc3MacIfClose();

	/* free active lines allocated memory in ports DB */
	for (portNum = 0; (portNum < HWS_CORE_PORTS_NUM(devNum)); portNum++)
	{
		if (hwsPortModeParamsGet(devNum, portGroup, portNum, SGMII /* dummy mode */) == NULL)
		{
			continue;
		}

		for (portMode = 0; portMode < HWS_DEV_PORT_MODES(devNum); portMode++)
		{
			mvHwsDestroyActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
		}
	}

	/* Unregister silicon port modes parameters in the system */
	hwsInitPortsModesParam(hwsDeviceSpecInfo[devNum].devType, 0);
}
#endif


