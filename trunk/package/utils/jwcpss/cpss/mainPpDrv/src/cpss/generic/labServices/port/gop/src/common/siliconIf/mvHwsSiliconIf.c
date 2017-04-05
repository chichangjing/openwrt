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
* mvHwsSiliconIf.c
*
* DESCRIPTION:
*       General silicon related HW Services API
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>

MV_OS_EXACT_DELAY_FUNC   hwsOsExactDelayPtr = NULL;
MV_OS_MICRO_DELAY_FUNC   hwsOsMicroDelayPtr = NULL;
MV_OS_TIME_WK_AFTER_FUNC hwsOsTimerWkFuncPtr = NULL;
MV_OS_MEM_SET_FUNC       hwsOsMemSetFuncPtr = NULL;
MV_OS_FREE_FUNC          hwsOsFreeFuncPtr = NULL;
MV_OS_MALLOC_FUNC        hwsOsMallocFuncPtr = NULL;
MV_SERDES_REG_ACCESS_SET hwsSerdesRegSetFuncPtr = NULL;
MV_SERDES_REG_ACCESS_GET hwsSerdesRegGetFuncPtr = NULL;
MV_SERVER_REG_ACCESS_SET hwsServerRegSetFuncPtr = NULL;
MV_SERVER_REG_ACCESS_GET hwsServerRegGetFuncPtr = NULL;
MV_OS_MEM_COPY_FUNC      hwsOsMemCopyFuncPtr = NULL;

/* Pointer to a function which returns the ILKN registers DB */
MV_SIL_ILKN_REG_DB_GET hwsIlknRegDbGetFuncPtr = NULL;

/* Contains related data for specific device */
HWS_DEVICE_INFO hwsDeviceSpecInfo[HWS_MAX_DEVICE_NUM];

/* store base address and unit index per unit per device type */
static HWS_UNIT_INFO   hwsDeviceSpecUnitInfo[LAST_SIL_TYPE][LAST_UNIT];
/* device specific functions pointers */
static MV_HWS_DEV_FUNC_PTRS hwsDevFunc[LAST_SIL_TYPE];

GT_STATUS genInterlakenRegSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask);
GT_STATUS genInterlakenRegGet (GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask);

/* bobcat3 pipe1 address indication */
#define BOBCAT3_PIPE_1_INDICATION_CNS 0x80000000

static GT_U32   hwsBobcat3ForceMode = 0;
void hwsBobcat3ForceModeSet(void)
{
    hwsBobcat3ForceMode = 1;
}
/* indication that bobcat3 used instead of bobk */
GT_U32 hwsBobcat3ForceModeGet(void)
{
    return hwsBobcat3ForceMode;
}


static void hwsBobcat3GlobalPortToPipeInfoConvert(
    IN GT_UOPT   globalPort,
    OUT GT_UOPT *localPortPtr,
    OUT GT_U32  *pipeIdPtr
)
{
    GT_UOPT localPort;
    GT_U32  pipeId;

    if(hwsBobcat3ForceMode)
    {
        if(globalPort == 72 || globalPort < 36)
        {
            /* pipe 0 */
            pipeId = 0;
        }
        else
        {
            /* pipe 1 */
            pipeId = 1;
        }

        localPort = (globalPort < 72) ?
                            (globalPort % 36)/*0..71*/ :
                            36;/*72,73*/
    }
    else
    {
        localPort = globalPort;
        pipeId = 0;
    }

    if(localPortPtr)
    {
        *localPortPtr = localPort;
    }

    if(pipeIdPtr)
    {
        *pipeIdPtr = pipeId;
    }

    return ;
}

/*******************************************************************************
* hwsOsLocalMicroDelay
*
* DESCRIPTION:
*       This API is used in case micro sec counter is not supported
*       It will convert micro to msec and round up in case needed
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - Port Group
*       microSec  - Delay in micro sec
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsOsLocalMicroDelay
(
    IN GT_U8  devNum,
    IN GT_U32 portGroup,
    IN GT_U32 microSec
)
{
    GT_U32 msec  = (microSec / 1000) + ((microSec % 1000) ? 1 : 0);

    hwsOsExactDelayPtr(devNum, portGroup, msec);

    return GT_OK;
}

/*******************************************************************************
* mvUnitExtInfoGet
*
* DESCRIPTION:
*       Return silicon specific base address and index for specified unit based on
*       unit index.
*
* INPUTS:
*       devNum    - Device Number
*       unitId    - unit ID (MAC, PCS, SERDES)
*       unitIndex - unit index
*
* OUTPUTS:
*       baseAddr  - unit base address in device
*       unitIndex - unit index in device
*       localUnitNumPtr - (pointer to) local unit number (support for Bobcat3 multi-pipe)
* RETURNS:
*       None
*
*******************************************************************************/
void  mvUnitExtInfoGet
(
    GT_U8           devNum,
    MV_HWS_UNITS_ID unitId,
    GT_U32          unitNum,
    GT_U32          *baseAddr,
    GT_U32          *unitIndex,
    GT_U32          *localUnitNumPtr
)
{
    if ((baseAddr == NULL) || (unitIndex == NULL) || (unitId >= LAST_UNIT))
    {
        return;
    }
    *baseAddr = hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][unitId].baseAddr;
    *unitIndex = hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][unitId].regOffset;
    if(localUnitNumPtr)
    {
        *localUnitNumPtr = unitNum;
    }

    if(hwsBobcat3ForceMode)
    {
        GT_U32  pipeId;
        /* bobcat3 support : convert the global port to local port */
        hwsBobcat3GlobalPortToPipeInfoConvert(unitNum/*global port*/,localUnitNumPtr/*local port*/,&pipeId);
        if(pipeId)
        {
            /* pipe 1 */
            *baseAddr |= BOBCAT3_PIPE_1_INDICATION_CNS;
        }

        return;
    }

    if (hwsDeviceSpecInfo[devNum].devType == BobcatA0 || hwsDeviceSpecInfo[devNum].devType == BobK)
    {
        /* fixed base address for some units (according to CIDER) */
        switch (unitId)
        {
        case GEMAC_UNIT:
        case XLGMAC_UNIT:
        case MMPCS_UNIT:
        case XPCS_UNIT:
            if (unitNum >= 56)
            {
                /* Unit calculation is: Base + 0x200000 + 0x1000*(K-56): where K (56-71) represents Port
                   in addition, since in register access level we also add INDEX*K (INDEX here is 0x1000),
                   we need to remove here INDEX*56 so total calculation will be
                   INDEX*K - INDEX*56 = INDEX*(K-56) */
                *baseAddr += (0x200000 - ((*unitIndex) * 56));
            }
            break;
        case SERDES_UNIT:
            if ((hwsDeviceSpecInfo[devNum].devType == BobK) && (unitNum >= 20))
            {
                /* Unit calc is: Base + 0x40000 + 0x1000*S: where S (20-35) represents Serdes */
                *baseAddr += 0x40000;
            }
            break;
        default:
            break;
        }
    }
}

/*******************************************************************************
* mvUnitInfoGet
*
* DESCRIPTION:
*       Return silicon specific base address and index for specified unit
*
* INPUTS:
*       devNum    - Device Number
*       unitId    - unit ID (MAC, PCS, SERDES)
*
* OUTPUTS:
*       baseAddr  - unit base address in device
*       unitIndex - unit index in device
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void  mvUnitInfoGet
(
    GT_U8           devNum,
    MV_HWS_UNITS_ID unitId,
    GT_U32          *baseAddr,
    GT_U32          *unitIndex
)
{
    if ((baseAddr == NULL) || (unitIndex == NULL) || (unitId >= LAST_UNIT))
    {
        return;
    }
    *baseAddr = hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][unitId].baseAddr;
    *unitIndex = hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][unitId].regOffset;
}

/*******************************************************************************
* mvUnitInfoSet
*
* DESCRIPTION:
*       Init silicon specific base address and index for specified unit
*
* INPUTS:
*       devType   - Device type
*       unitId    - unit ID (MAC, PCS, SERDES)
*       baseAddr  - unit base address in device
*       unitIndex - unit index in device
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS  mvUnitInfoSet
(
    MV_HWS_DEV_TYPE devType,
    MV_HWS_UNITS_ID unitId,
    GT_U32          baseAddr,
    GT_U32          unitIndex
)
{
    if ((devType >= LAST_SIL_TYPE) || (unitId >= LAST_UNIT))
    {
        return GT_BAD_PARAM;
    }
    hwsDeviceSpecUnitInfo[devType][unitId].baseAddr = baseAddr;
    hwsDeviceSpecUnitInfo[devType][unitId].regOffset = unitIndex;

    return GT_OK;
}

/*******************************************************************************
* mvUnitInfoGetByAddr
*
* DESCRIPTION:
*       Return unit ID by unit address in device
*
* INPUTS:
*       devNum    - Device Number
*       baseAddr  - unit base address in device
*
* OUTPUTS:
*       unitId    - unit ID (MAC, PCS, SERDES)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void  mvUnitInfoGetByAddr
(
    GT_U8           devNum,
    GT_U32          baseAddr,
    MV_HWS_UNITS_ID *unitId
)
{
    GT_U32 i;

    if (unitId == NULL)
    {
        return;
    }
    *unitId = LAST_UNIT;
    for (i = 0; i < LAST_UNIT; i++)
    {
        if (baseAddr == hwsDeviceSpecUnitInfo[hwsDeviceSpecInfo[devNum].devType][i].baseAddr)
        {
            *unitId = i;
            return;
        }
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
GT_STATUS mvHwsRedundancyVectorGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  *sdVector
)
{
    if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].redundVectorGetFunc == NULL)
    {
		*sdVector = 0;
		return GT_OK;
    }

    return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].redundVectorGetFunc(devNum, portGroup, sdVector);
}

/*******************************************************************************
* mvHwsClockSelectorConfig
*
* DESCRIPTION:
*       Configures the DP/Core Clock Selector on port 
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
GT_STATUS mvHwsClockSelectorConfig
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode 
)
{
    if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].clkSelCfgGetFunc == NULL)
    {
		return GT_OK;
    }

    return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].clkSelCfgGetFunc(devNum, portGroup, phyPortNum, portMode);
}

/*******************************************************************************
* hwsDeviceSpecGetFuncPtr
*
* DESCRIPTION:
*       Get function structure pointer.
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
void hwsDeviceSpecGetFuncPtr(MV_HWS_DEV_FUNC_PTRS **hwsFuncsPtr)
{
	/* TBD - need to update function signature to get device type */
	static GT_BOOL devFuncInitDone = GT_FALSE;

	if(devFuncInitDone == GT_FALSE)
	{
		hwsOsMemSetFuncPtr(hwsDevFunc, 0, sizeof(MV_HWS_DEV_FUNC_PTRS) * LAST_SIL_TYPE);
		devFuncInitDone = GT_TRUE;
	}

    *hwsFuncsPtr = &hwsDevFunc[0];
}

/*******************************************************************************
* genUnitRegisterSet
*
* DESCRIPTION:
*       Implement write access to device registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       address   - address to access
*       data      - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS genUnitRegisterSet
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    MV_HWS_UNITS_ID unitId,
    GT_UOPT         unitNum,
    GT_UREG_DATA    regOffset,
    GT_UREG_DATA    data,
    GT_UREG_DATA    mask
)
{
    GT_U32 address;
    GT_U32 unitAddr = 0;
    GT_U32 unitIndex = 0;

    /* get unit base address and unit index for current device */
    mvUnitExtInfoGet(devNum, unitId, unitNum, &unitAddr, &unitIndex,&unitNum);
    if ((unitAddr == 0) || (unitIndex == 0))
    {
        return GT_BAD_PARAM;
    }
    address = unitAddr + unitIndex * unitNum + regOffset;

	if ((unitId == INTLKN_RF_UNIT) || (unitId == ETI_ILKN_RF_UNIT))
    {
        CHECK_STATUS(genInterlakenRegSet(devNum, portGroup, address, data, mask));
    }
    else
    {
		CHECK_STATUS(genRegisterSet(devNum, portGroup, address, data, mask));
    }
    
    return GT_OK;
}

/*******************************************************************************
* genUnitRegisterGet
*
* DESCRIPTION:
*       Read access to device registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       address   - address to access
*
* OUTPUTS:
*       data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS genUnitRegisterGet
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    MV_HWS_UNITS_ID unitId,
    GT_UOPT         unitNum,
    GT_UREG_DATA    regOffset,
    GT_UREG_DATA    *data,
    GT_UREG_DATA    mask
)
{
    GT_U32 address;
    GT_U32 unitAddr = 0;
    GT_U32 unitIndex = 0;

    /* get unit base address and unit index for current device */
    mvUnitExtInfoGet(devNum, unitId, unitNum, &unitAddr, &unitIndex,&unitNum);
    if ((unitAddr == 0) || (unitIndex == 0))
    {
        return GT_BAD_PARAM;
    }
    address = unitAddr + unitIndex * unitNum + regOffset;

	if ((unitId == INTLKN_RF_UNIT) || (unitId == ETI_ILKN_RF_UNIT))
    {
        CHECK_STATUS(genInterlakenRegGet(devNum, portGroup, address, data, mask));
    }
    else
    {
		CHECK_STATUS(genRegisterGet(devNum, portGroup, address, data, mask));
    }

    return GT_OK;
}

/*******************************************************************************
* genInterlakenRegSet
*
* DESCRIPTION:
*       Write ILKN registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       address   - address to access
*
* OUTPUTS:
*       data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS genInterlakenRegSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask)
{
    MV_INTLKN_REG *dbArray;
    GT_U32 regData;
    GT_U32 offset;

	if(hwsIlknRegDbGetFuncPtr == NULL)
	{
		return GT_NOT_INITIALIZED;
	}

    dbArray = hwsIlknRegDbGetFuncPtr(devNum, portGroup, address);
	
	if(dbArray == NULL)
	{
		return GT_NOT_SUPPORTED;
	}

	/*osPrintf("genInterlakenRegSet address 0x%x\n", address);*/

    if (mask == 0)
    {
        /* store the register data */
        regData = data;
    }
    else
    {
		CHECK_STATUS(genInterlakenRegGet(devNum, portGroup, address, &regData, 0));

		/*osPrintf("Data: 0x%x", regData);*/

		/* Update the relevant bits at the register data */
        regData = (regData & ~mask) | (data & mask);

		/*osPrintf("\tNew Data: 0x%x\n", regData);*/
    }

	offset = address & 0x7FF;

	/*osPrintf("(Set) offset 0x%x\n", offset);*/
	
    /* store new value in shadow */
    switch (offset)
    {
    case 0:
        dbArray->ilkn0MacCfg0 = regData;
        break;
    case 4:
        dbArray->ilkn0ChFcCfg0 = regData;
        break;
    case 0xC:
        dbArray->ilkn0MacCfg2 = regData;
        break;
    case 0x10:
        dbArray->ilkn0MacCfg3 = regData;
        break;
    case 0x14:
        dbArray->ilkn0MacCfg4 = regData;
        break;
    case 0x1C:
        dbArray->ilkn0MacCfg6 = regData;
        break;
    case 0x60:
        dbArray->ilkn0ChFcCfg1 = regData;
        break;
    case 0x200:
        dbArray->ilkn0PcsCfg0 = regData;
        break;
    case 0x204:
        dbArray->ilkn0PcsCfg1 = regData;
        break;
    case 0x20C:
        dbArray->ilkn0En = regData;
        break;
    case 0x238:
        dbArray->ilkn0StatEn = regData;
        break;
    default:
        return GT_NOT_SUPPORTED;
    }

    return genRegisterSet(devNum, portGroup, address, regData, 0);
}

/*******************************************************************************
* genInterlakenRegGet
*
* DESCRIPTION:
*       Read ILKN registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       address   - address to access
*
* OUTPUTS:
*       data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS genInterlakenRegGet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask)
{
    MV_INTLKN_REG *dbArray;
    GT_U32 regData;
    GT_U32 offset;

    devNum = devNum;
    portGroup = portGroup;

    if (data == NULL)
    {
        return GT_BAD_PARAM;
    }

	if(hwsIlknRegDbGetFuncPtr == NULL)
	{
		return GT_NOT_INITIALIZED;
	}

	/*osPrintf("genInterlakenRegGet address 0x%x\n", address);*/

	dbArray = hwsIlknRegDbGetFuncPtr(devNum, portGroup, address);
	
	if(dbArray == NULL)
	{
		return GT_NOT_SUPPORTED;
	}

	offset = address & 0x7FF;

	/*osPrintf("(Set) offset 0x%x\n", offset);*/

	switch (offset)
    {
    case 0:      /*ILKN_0_MAC_CFG_0 = 0,*/
        regData = dbArray->ilkn0MacCfg0;
        break;
    case 4:
        regData = dbArray->ilkn0ChFcCfg0;
        break;
    case 0xC:
        regData = dbArray->ilkn0MacCfg2;
        break;
    case 0x10:
        regData = dbArray->ilkn0MacCfg3;
        break;
    case 0x14:
        regData = dbArray->ilkn0MacCfg4;
        break;
    case 0x1C:
        regData = dbArray->ilkn0MacCfg6;
        break;
    case 0x60:
        regData = dbArray->ilkn0ChFcCfg1;
        break;
    case 0x200:
        regData = dbArray->ilkn0PcsCfg0;
        break;
    case 0x204:
        regData = dbArray->ilkn0PcsCfg1;
        break;
    case 0x20C:
        regData = dbArray->ilkn0En;
        break;
    case 0x238:
        regData = dbArray->ilkn0StatEn;
        break;
    case 0x10C:
		/* these registers offsets are readable */
		CHECK_STATUS(genRegisterGet(devNum, portGroup, address, &regData, 0));
        break;
    default:
        *data = 0;
        return GT_NOT_SUPPORTED;
    }

    if (mask == 0)
    {
        *data = regData;
    }
    else
    {
        /* Retrieve the relevant bits from the register's data and shift left */
        *data = (regData & mask);
    }
    return GT_OK;
}

#ifndef MV_HWS_REDUCED_BUILD
/*******************************************************************************
* mvHwsReNumberDevNum
*
* DESCRIPTION:
*       Replace the ID of a device from old device number
*       to a new device number
*
* INPUTS:
*       oldDevNum  - old device num
*       newDevNum  - new device num
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS mvHwsReNumberDevNum
(
    GT_U8   oldDevNum,
    GT_U8   newDevNum
)
{
	MV_HWS_DEV_TYPE devType;

	if ((newDevNum >= HWS_MAX_DEVICE_NUM) || (hwsDeviceSpecInfo[oldDevNum].devNum != oldDevNum))
    {
        return GT_BAD_PARAM;
    }

	devType = hwsDeviceSpecInfo[oldDevNum].devType;

	/* Init device */
	mvHwsDeviceInit(newDevNum, devType, NULL);

	/* Close device */
	mvHwsDeviceClose(oldDevNum);

	hwsOsMemSetFuncPtr(&hwsDeviceSpecInfo[oldDevNum], 0, sizeof (HWS_DEVICE_INFO));

    return GT_OK;
}

/*******************************************************************************
* mvHwsDeviceInit
*
* DESCRIPTION:
*       Init device,
*		according to device type call function to init all software related DB:
*		DevInfo, Port (Port modes, MAC, PCS and SERDES) and address mapping.
*
* INPUTS:
*		devType - enum of the device type
*       funcPtr - pointer to structure that hold the "os"
*                 functions needed be bound to HWS.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS mvHwsDeviceInit
(
	GT_U8             devNum,
	MV_HWS_DEV_TYPE   devType,
	HWS_OS_FUNC_PTR   *funcPtr
)
{
	switch (devType)
	{
	case Lion2A0:
	case Lion2B0:
		hwsLion2IfInit(devNum, funcPtr);
		break;
	case HooperA0:
		hwsHooperIfInit(devNum, funcPtr);
		break;
	case BobcatA0:
		hwsBobcat2IfInit(devNum, funcPtr);
		break;
	case Alleycat3A0:
		mvHwsAlleycat3IfInit(devNum, funcPtr);
		break;
	case BobK:
		hwsBobKIfInit(devNum, funcPtr);
		break;
	default:
		break;
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsDeviceClose
*
* DESCRIPTION:
*		Close device,
*       according to device type call function to free all resource allocated for ports initialization.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS mvHwsDeviceClose
(
	GT_U8   devNum
)
{
	switch (hwsDeviceSpecInfo[devNum].devType)
	{
	case Lion2A0:
	case Lion2B0:
		hwsLion2IfClose(devNum);
		break;
	case HooperA0:
		hwsHooperIfClose(devNum);
		break;
	case BobcatA0:
		hwsBobcat2IfClose(devNum);
		break;
	case Alleycat3A0:
		hwsAlleycat3IfClose(devNum);
		break;
	case BobK:
		hwsBobKIfClose(devNum);
		break;
	default:
		break;
	}

	return GT_OK;
}
#endif


