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
* mvHwsBobcatIfDevInit.c
*
* DESCRIPTION:
*       Bobcat specific HW Services init
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <private/mvHwsPortPrvIf.h>
#include <mvHwsPortCfgIf.h>
#include <mvHwsIpcApis.h>
#include <mvHwsServiceCpuFwIf.h>
#include <silicon/bobk/mvHwsBobKPortIf.h>

#include <mac/mvHwsMacIf.h>
#include <mac/geMac/mvHwsGeMac28nmIf.h>
#include <mac/geMac/mvHwsGeMacSgIf.h>
#include <mac/xgMac/mvHwsXgMac28nmIf.h>
#include <mac/xlgMac/mvHwsXlgMac28nmIf.h>
#include <mac/ilknMac/mvHwsIlknMacIf.h>

#include <pcs/mvHwsPcsIf.h>
#include <pcs/gPcs/mvHwsGPcs28nmIf.h>
#include <pcs/xPcs/mvHwsXPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcs28nmIf.h>
#include <pcs/ilknPcs/mvHwsIlknPcsIf.h>

#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/avago/mvAvagoIf.h>

#ifndef MV_HWS_REDUCED_BUILD
#include <mvDdr3TrainingIpPrvIf.h>
#include <mvHwsDdr3BobK.h>
#endif

/************************* definition *****************************************************/

#define BOBK_GIG_BASE        (0x10000000)
#define BOBK_XPCS_BASE       (0x10180400)
#define BOBK_XLG_BASE        (0x100C0000)
#define BOBK_MPCS_BASE       (0x10180000)

#define BOBK_SERDES_BASE     (0x13000000)

#define IND_OFFSET (0x1000)

#define BOBK_MAX_CORE_CLOCK      7

/************************* Globals *******************************************************/

extern MV_HWS_PORT_INIT_PARAMS *hwsPortsBobKParams[];

/* init per device */
static GT_BOOL hwsInitDone[HWS_MAX_DEVICE_NUM] = {0};
static GT_BOOL bobKDbInitDone = GT_FALSE;

GT_U32 bobKCoreClkOptions[BOBK_MAX_CORE_CLOCK] = {365, 220, 250, 200, 167, 133, 25};

/************************* pre-declaration ***********************************************/


/*****************************************************************************************/

/*******************************************************************************
* hwsPcsIfInit
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
static GT_STATUS pcsIfInit(GT_U8	devNum)
{
    MV_HWS_PCS_FUNC_PTRS *hwsPcsFuncsPtr;

    CHECK_STATUS(hwsPcsGetFuncPtr(devNum, &hwsPcsFuncsPtr));

    CHECK_STATUS(mvHwsXPcsIfInit(hwsPcsFuncsPtr));      /* XPCS */
    CHECK_STATUS(mvHwsGPcs28nmIfInit(hwsPcsFuncsPtr));  /* GPCS */
    CHECK_STATUS(mvHwsMMPcs28nmIfInit(hwsPcsFuncsPtr)); /* MMPCS */

    return GT_OK;
}

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
static GT_STATUS macIfInit(GT_U8 devNum)
{
    MV_HWS_MAC_FUNC_PTRS *hwsMacFuncsPtr;

    CHECK_STATUS(hwsMacGetFuncPtr(devNum, &hwsMacFuncsPtr));

    CHECK_STATUS(hwsGeMac28nmIfInit(devNum, hwsMacFuncsPtr)); /* GE MAC */
    CHECK_STATUS(hwsXgMac28nmIfInit(devNum, hwsMacFuncsPtr)); /* XG MAC */
    CHECK_STATUS(hwsXlgMac28nmIfInit(devNum, hwsMacFuncsPtr));/* XLG MAC */
#ifndef MV_HWS_REDUCED_BUILD
    /* ILKN/SG configuration is not done in Internal CPU */
    CHECK_STATUS(hwsGeMacSgIfInit(devNum, hwsMacFuncsPtr));   /* GE_SG MAC */
#endif
    return GT_OK;
}

/*******************************************************************************
* mvHwsBobKCoreClockGet
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
static GT_STATUS mvHwsBobKCoreClockGet
(
    GT_U8	devNum,
    GT_U32	*coreClkPtr
    )
{
    GT_U32	data;

    /* get the PLL 0 Config value from server */
    hwsServerRegGetFuncPtr(devNum, SERVER_PLL0_CONFIG_REG, &data);
    data = (data >> 21) & 0x7;   /* bits 21-23 */

    if (data > BOBK_MAX_CORE_CLOCK)
    {
        return GT_BAD_STATE;
    }

    if (data == 0x7)
    {
        /* PLL is in BYPASS mode - PLL WA is applied */
        hwsServerRegGetFuncPtr(devNum, SERVER_PLL_CORE_PARAM_REG, &data);
        data = (data >> 12) & 0x1FF;   /* bits 12-20 */

        switch (data)
        {
            case 106:
                *coreClkPtr = bobKCoreClkOptions[4]; /* 167Mhz */
                break;
            case 63:
                *coreClkPtr = bobKCoreClkOptions[3]; /* 200Mhz */
                break;
            case 79:
                *coreClkPtr = bobKCoreClkOptions[2]; /* 250Mhz */
                break;
            case 350:
                *coreClkPtr = bobKCoreClkOptions[0]; /* 365Mhz */
                break;
            default:
                return GT_BAD_STATE;
        }
    }
    else
    {
        /* PLL WA is not active */
        *coreClkPtr = bobKCoreClkOptions[data];
    }

    return  GT_OK;
}

/*******************************************************************************
* mvHwsBobKClkSelCfg
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
static GT_STATUS mvHwsBobKClkSelCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode
)
{
    MV_HWS_PORT_INIT_PARAMS   *curPortParams;
    GT_U32  coreClk, data;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* read core clock */
    CHECK_STATUS(mvHwsBobKCoreClockGet(devNum, &coreClk));

    data = ((portMode == SGMII2_5) && (coreClk < 312)) ? (1 << 5) : 0;

    /* Set DP Clock Selector */
    CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, curPortParams->portMacNumber,
                                    GIG_PORT_MAC_CONTROL_REGISTER4, data, (1 << 5)));

    return GT_OK;
}

/*******************************************************************************
* hwsSerdesIfInit
*
* DESCRIPTION:
*       Init all supported Serdes types.
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

  CHECK_STATUS(mvHwsAvagoIfInit(devNum, hwsSerdesFuncsPtr));

  return GT_OK;
}

/*******************************************************************************
* TrainingIpIfInit
*
* DESCRIPTION:
*       Init training IP functionality.
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
static GT_STATUS TrainingIpIfInit(GT_U8 devNum)
{
#ifndef MV_HWS_REDUCED_BUILD
    MV_HWS_TRAINING_IP_FUNC_PTRS *funcPtrArray;

    CHECK_STATUS(ddr3TipInitBobK(devNum, 0 /* board ID*/));
	if(bobKDbInitDone == GT_FALSE)
	{
		mvHwsTrainingIpGetFuncPtr(&funcPtrArray);
		mvHwsDdr3TipIfInit(funcPtrArray);
	}
#endif
	return GT_OK;
}

/*******************************************************************************
* hwsBobcat2IfInit
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
GT_STATUS hwsBobKIfInit(GT_U8 devNum, HWS_OS_FUNC_PTR *funcPtr)
{
	GT_U32  portGroup = devNum;
	GT_U32  data, version;
	MV_HWS_DEV_TYPE devType;
	MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;
	GT_STATUS res;

	if (hwsInitDone[devNum])
	{
	    return GT_ALREADY_EXIST;
	}

	if(bobKDbInitDone == GT_FALSE)
	{
		if (funcPtr == NULL)
			return GT_FAIL;

		if ((funcPtr->osTimerWkPtr == NULL) || (funcPtr->osMemSetPtr == NULL) ||
			(funcPtr->osFreePtr == NULL) || (funcPtr->osMallocPtr == NULL) ||
			(funcPtr->osExactDelayPtr == NULL) || (funcPtr->sysDeviceInfo == NULL) ||
			(funcPtr->osMemCopyPtr == NULL) ||
			(funcPtr->serdesRegSetAccess == NULL) ||(funcPtr->serdesRegGetAccess == NULL) ||
			(funcPtr->serverRegSetAccess == NULL) || (funcPtr->serverRegGetAccess == NULL))
		{
			return GT_BAD_PARAM;
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

	hwsDeviceSpecInfo[devNum].devType = BobK;
	hwsDeviceSpecInfo[devNum].devNum = devNum;
	hwsDeviceSpecInfo[devNum].portsNum = 72;
	hwsDeviceSpecInfo[devNum].lastSupPortMode = NON_SUP_MODE;
	hwsDeviceSpecInfo[devNum].serdesType = AVAGO;

	devType = hwsDeviceSpecInfo[devNum].devType;

#ifndef MV_HWS_REDUCED_BUILD
	/* define IPC connection Type Host side */
	if (mvHwsServiceCpuEnableGet(devNum))
	{
		/* AP FW is loaded to Service CPU, use IPC mode - API execution at the Service CPU */

		/* !!! Replace HOST2HWS_LEGACY_MODE with HOST2SERVICE_CPU_IPC_CONNECTION once approved by CPSS !!! */
		hwsDeviceSpecInfo[devNum].ipcConnType = HOST2HWS_LEGACY_MODE/*HOST2SERVICE_CPU_IPC_CONNECTION*/;
	}
	else
	{
		/* AP FW is NOT loaded to Service CPU, use Legacy mode - API execution at the Host */
		hwsDeviceSpecInfo[devNum].ipcConnType = HOST2HWS_LEGACY_MODE;
	}
#else
	/* define IPC connection Type Service CPU side */
	hwsDeviceSpecInfo[devNum].ipcConnType = HOST2HWS_LEGACY_MODE;
#endif /* MV_HWS_REDUCED_BUILD */

	/* Init HWS Firmware */
	if (mvHwsServiceCpuEnableGet(devNum))
		CHECK_STATUS(mvHwsServiceCpuFwInit(devNum));

	hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

	if (hwsDevFunc[devType].deviceInfoFunc == NULL)
	{
		hwsDevFunc[devType].deviceInfoFunc = funcPtr->sysDeviceInfo;
	}

	/* get device id and version */
	hwsDevFunc[devType].deviceInfoFunc(devNum, &data, &version);

    hwsDeviceSpecInfo[devNum].gopRev = GOP_28NM_REV3;
	hwsDevFunc[devType].pcsMarkModeFunc = NULL;

	hwsDevFunc[devType].redundVectorGetFunc = NULL;

	/* Configures the DP/Core Clock Selector on port */
	hwsDevFunc[devType].clkSelCfgGetFunc = mvHwsBobKClkSelCfg;

	/* Configures port init / reset functions */
	hwsDevFunc[devType].devPortinitFunc = mvHwsBobKPortInit;
	hwsDevFunc[devType].devPortResetFunc = mvHwsBobKPortReset;

	/* Configures  device handler functions */
	hwsDevFunc[devType].portAutoTuneSetExtFunc = mvHwsPortAvagoAutoTuneSetExt;
	hwsDevFunc[devType].portExtModeCfgFunc     = NULL;                         /* TBD - Need to be implemented */
	hwsDevFunc[devType].portExtModeCfgGetFunc  = NULL;                         /* TBD - Need to be implemented */
	hwsDevFunc[devType].portParamsIndexGetFunc = hwsBobKPortParamIndexGet;
    hwsDevFunc[devType].portLbSetWaFunc        = NULL;
	hwsDevFunc[devType].portAutoTuneStopFunc   = NULL;
	hwsDevFunc[devType].portAutoTuneSetWaFunc  = NULL;
	hwsDevFunc[devType].portFixAlign90Func     = NULL;

	/* Register silicon port modes parameters in the system */
	hwsInitPortsModesParam(devType, &hwsPortsBobKParams[0]);

	if(bobKDbInitDone == GT_FALSE)
	{
		CHECK_STATUS(hwsBobKPortsElementsCfg(devNum, portGroup));
	}

	/* Init all MAC units relevant for current device */
	CHECK_STATUS(macIfInit(devNum));

	/* Init all PCS units relevant for current device */
	CHECK_STATUS(pcsIfInit(devNum));

	/* Init SERDES unit relevant for current device */
	CHECK_STATUS(serdesIfInit(devNum));

	/* Update HWS After Firmware load and init */
	if (mvHwsServiceCpuEnableGet(devNum))
		CHECK_STATUS(mvHwsServiceCpuFwPostInit(devNum));

    /* Init configuration sequence executer */
    mvCfgSeqExecInit();

	mvUnitInfoSet(devType, GEMAC_UNIT,  BOBK_GIG_BASE,    IND_OFFSET);
	mvUnitInfoSet(devType, MMPCS_UNIT,  BOBK_MPCS_BASE,   IND_OFFSET);
	mvUnitInfoSet(devType, XPCS_UNIT,   BOBK_XPCS_BASE,   IND_OFFSET);
	mvUnitInfoSet(devType, XLGMAC_UNIT, BOBK_XLG_BASE,    IND_OFFSET);
	mvUnitInfoSet(devType, SERDES_UNIT, BOBK_SERDES_BASE, IND_OFFSET);

	/* DDR training initialization */
	res = TrainingIpIfInit(devNum);
	if ((res != GT_OK) && (res != GT_NOT_INITIALIZED))
	{
		return res;
	}
	bobKDbInitDone = GT_TRUE;
	hwsInitDone[devNum] = GT_TRUE;

	return GT_OK;
}

#ifndef MV_HWS_REDUCED_BUILD
/*******************************************************************************
* hwsSerdesIfClose
*
* DESCRIPTION:
*       Free all resources allocated by supported serdes types.
*
* INPUTS:
*       GT_U8 devNum
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static void serdesIfClose(GT_U8 devNum)
{
    mvHwsAvagoIfClose(devNum);
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
static void pcsIfClose(void)
{
    hwsXPcsIfClose();
    hwsGPcs28nmIfClose();
    hwsMMPcs28nmIfClose();
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
    hwsGeMac28nmIfClose();
    hwsXgMac28nmIfClose();
    hwsXlgMac28nmIfClose();
}

/*******************************************************************************
* hwsBobcat2IfClose
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
void hwsBobKIfClose(GT_U8 devNum)
{
	GT_U32  portGroup = devNum;
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

	bobKDbInitDone = GT_FALSE;

	serdesIfClose(devNum);
	pcsIfClose();
	macIfClose();

	/* free active lines allocated memory in ports DB */
	mvHwsDestroyDeviceLaneList(devNum, portGroup);

	/* un-register silicon port modes parameters in the system */
	hwsInitPortsModesParam(hwsDeviceSpecInfo[devNum].devType, 0);
}

#endif

