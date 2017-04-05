/**************************************************************************
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
*       $Revision: 23 $
*
*******************************************************************************/
#include <gtOs/gtGenTypes.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <trainingIp/mvDdr3TrainingIpPrvIf.h>
#include <trainingIp/silicon/mvHwsDdr3NP5.h>

#define MILOS5_SERDES_BASE      (0)
#define MILOS5_SERDES_PHY_BASE  (0x800)
#define SERDES_IND_OFFSET       (0)

/* init per device */
static GT_BOOL hwsInitDone[HWS_MAX_DEVICE_NUM] = {0};
static GT_BOOL np5DbInitDone = GT_FALSE;

#ifndef FreeRTOS
extern GT_STATUS hwsComPhyH28nmSeqInit(void);
extern GT_STATUS hwsComPhyH28nmRev3SeqInit(void);
#endif

/*******************************************************************************
* serdesIfInit
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
static GT_STATUS serdesIfInit(MV_HWS_SERDES_TYPE type, GT_U32 devRevision)
{
#ifdef FreeRTOS
	MV_HWS_SERDES_FUNC_PTRS *hwsSerdesFuncsPtr;

	hwsSerdesGetFuncPtr(0, &hwsSerdesFuncsPtr);

	if ((devRevision == DevVersion_P1_A0)|| (devRevision == DevVersion_P1_A1)) {
		osPrintf("Serdes Rev A\n");
		CHECK_STATUS(mvHwsComH28nmIfInit(hwsSerdesFuncsPtr));
	} else if (devRevision == DevVersion_P2_B0) {
		osPrintf("Serdes Rev B\n");
		CHECK_STATUS(mvHwsComH28nmRev3IfInit(hwsSerdesFuncsPtr));
	} else {
		osPrintf("Serdes Rev C\n");
		CHECK_STATUS(mvHwsComH28nmRev3IfInit(hwsSerdesFuncsPtr));
	}
#else
	if ((devRevision == DevVersion_P1_A0)|| (devRevision == DevVersion_P1_A1)) {
		osPrintf("Serdes Rev A\n");
		CHECK_STATUS(hwsComPhyH28nmSeqInit());
	} else {
		osPrintf("Serdes Rev B\n");
		CHECK_STATUS(hwsComPhyH28nmRev3SeqInit());
	}
#endif
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
static void TrainingIpIfInit(GT_U32 devNum)
{
	MV_HWS_TRAINING_IP_FUNC_PTRS *funcPtrArray;
	GT_U32 board = 0;

	ddr3TipInitNp5(devNum, board);
	mvHwsTrainingIpGetFuncPtr(&funcPtrArray);
	mvHwsTrainingIpIfInit(funcPtrArray);
}


/*******************************************************************************
* hwsNp5IfInit
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
GT_STATUS hwsNp5IfInit(GT_U8 devNum, HWS_OS_FUNC_PTR *funcPtr)
{
	GT_U32  data, version;
	MV_HWS_DEV_TYPE devType = NP5A0;

	if (hwsInitDone[devNum]) {
		return GT_ALREADY_EXIST;
	}

	if (funcPtr == NULL)
		return GT_FAIL;

	if ((funcPtr->osTimerWkPtr == NULL) ||
		(funcPtr->osMemSetPtr == NULL) ||
		(funcPtr->osFreePtr == NULL) ||
		(funcPtr->osMallocPtr == NULL) ||
		(funcPtr->osExactDelayPtr == NULL) ||
		(funcPtr->sysDeviceInfo == NULL) ||
		(funcPtr->serdesRegSetAccess == NULL) ||
		(funcPtr->serdesRegGetAccess == NULL) ||
		(funcPtr->serverRegSetAccess == NULL) ||
		(funcPtr->serverRegGetAccess == NULL))
			return GT_BAD_PARAM;

	/* Init configuration sequence executer */
	/*mvCfgSeqExecInit();*/

	hwsOsExactDelayPtr = funcPtr->osExactDelayPtr;
	hwsOsTimerWkFuncPtr = funcPtr->osTimerWkPtr;
	hwsOsMemSetFuncPtr = funcPtr->osMemSetPtr;
	hwsOsFreeFuncPtr = funcPtr->osFreePtr;
	hwsOsMallocFuncPtr = funcPtr->osMallocPtr;
	hwsSerdesRegSetFuncPtr = funcPtr->serdesRegSetAccess;
	hwsSerdesRegGetFuncPtr = funcPtr->serdesRegGetAccess;
	hwsServerRegSetFuncPtr = funcPtr->serverRegSetAccess;
	hwsServerRegGetFuncPtr = funcPtr->serverRegGetAccess;

	/*
	hwsDeviceSpecInfo[devNum].portsNum = 12;
	hwsDeviceSpecInfo[devNum].lastSupPortMode = _100GBase_SR10;
	*/

	/* get device id and version */
	funcPtr->sysDeviceInfo(devNum, &data, &version);
	osPrintf("Version data is 0x%x (version is 0x%x)\n", data, version);
	if (version == DevVersion_P3_C0) {
		osPrintf("Type: NP5C0\n");
		devType = NP5B0; /*C0 have a similar bihevior*/
	}else if (version == DevVersion_P2_B0) {
		osPrintf("Type: NP5B0\n");
		devType = NP5B0;
	} else {
		osPrintf("Type: NP5A0\n");
		devType = NP5A0;
	}

	hwsDeviceSpecInfo[devNum].devNum = devNum;
	hwsDeviceSpecInfo[devNum].devType = devType;
	hwsDeviceSpecInfo[devNum].gopRev = GOP_40NM_REV1;
	hwsDeviceSpecInfo[devNum].serdesType = COM_PHY_28NM;

	/* define Host to HWS connection type  - now Legacy mode*/
	hwsDeviceSpecInfo[devNum].ipcConnType = HOST2HWS_LEGACY_MODE;

	/* Init SERDES unit relevant for current device */
	CHECK_STATUS(serdesIfInit(hwsDeviceSpecInfo[devNum].devType, version));

	mvUnitInfoSet(hwsDeviceSpecInfo[devNum].devType, SERDES_UNIT, MILOS5_SERDES_BASE, SERDES_IND_OFFSET);
	mvUnitInfoSet(hwsDeviceSpecInfo[devNum].devType, SERDES_PHY_UNIT, MILOS5_SERDES_PHY_BASE, SERDES_IND_OFFSET);

	/* init Training IP functionality */
	TrainingIpIfInit(devNum);

	np5DbInitDone = GT_TRUE;
	hwsInitDone[devNum] = GT_TRUE;

	return GT_OK;
}

/*******************************************************************************
* hwsNp5IfClose
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
void hwsNp5IfClose(GT_U8 devNum)
{
	GT_U32  i;

	if (hwsInitDone[devNum]) {
	    hwsInitDone[devNum] = GT_FALSE;
	}

	/* if there is no active device remove DB */
	for (i = 0; i < HWS_MAX_DEVICE_NUM; i++) {
	    if (hwsInitDone[i]) {
		return;
	    }
	}

	np5DbInitDone = GT_FALSE;

	serdesIfClose(hwsDeviceSpecInfo[devNum].devType);
}

