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
#include <mvHwsPortInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <mvHwsIpcApis.h>
#include <mvHwsServiceCpuFwIf.h>
#include <silicon/bobcat2/mvHwsBobcat2PortIf.h>
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

#include <serdes/mvHwsSerdes28nmPrvIf.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmIf.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3If.h>

#ifndef MV_HWS_REDUCED_BUILD
    #include <mvDdr3TrainingIpPrvIf.h>
    #include <mvHwsDdr3Bc2.h>
#endif

/*'global' variable that all HWS files can 'see' and use to diff from BC2*/
GT_U32 bobkSupported = 0;

/************************* definition *****************************************************/

#define BOBCAT2_NW_GIG_BASE     (0x10000000) /* ports 0 - 55 */
#define BOBCAT2_FB_GIG_BASE     (0x10200000) /* ports 56 - 71 */
#define BOBCAT2_NW_XPCS_BASE    (0x10180400)
#define BOBCAT2_FB_XPCS_BASE    (0x10380400)
#define BOBCAT2_NW_XLG_BASE     (0x100C0000) /* ports 0 - 55 */
/*#define BOBCAT2_FB_XLG_BASE     (0x102C0000)*/ /* ports 0 - 15 CHECK WITH VLADIMIR and CIDER */
#define BOBCAT2_NW_MPCS_BASE    (0x10180000) /* ports 48 - 55 */
#define BOBCAT2_FB_MPCS_BASE    (0x10380000) /* ports 56 - 71 */
#define BOBCAT2_ILKN_RF_BASE    (0x01F000000)
#define BOBCAT2_ILKN_BASE       (0x01F000800)

#define BOBCAT2_SERDES_BASE     (0x13000000)
#define BOBCAT2_SERDES_PHY_BASE (0x13000800)

#define IND_OFFSET (0x1000)

#define BC2_MAX_CORE_CLOCK      7

typedef enum
{
    DB_BOARD = 0,
    RD_BOARD = 1
}bc2BoardType;


/************************* Globals *******************************************************/

extern MV_HWS_PORT_INIT_PARAMS *hwsPortsBobcatParams[];

/* init per device */
static MV_INTLKN_REG *deviceInlkRegsShadow[HWS_MAX_DEVICE_NUM] = {0};
static GT_BOOL hwsInitDone[HWS_MAX_DEVICE_NUM] = {0};
static GT_BOOL bobcatDbInitDone = GT_FALSE;

GT_U32 bobcat2A0CoreClkOptions[BC2_MAX_CORE_CLOCK] = {362, 221, 250, 400, 500, 521, 450};
GT_U32 bobcat2B0CoreClkOptions[BC2_MAX_CORE_CLOCK] = {362, 221, 250, 400, 175, 521, 450};

/************************* pre-declaration ***********************************************/
static MV_INTLKN_REG* ilknRegDbGet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address);
static GT_STATUS hwsILknRegDbInit(GT_U8 devNum);

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
static GT_STATUS pcsIfInit(GT_U8   devNum)
{
    MV_HWS_PCS_FUNC_PTRS *hwsPcsFuncsPtr;

    CHECK_STATUS(hwsPcsGetFuncPtr(devNum, &hwsPcsFuncsPtr));

    CHECK_STATUS(mvHwsXPcsIfInit(hwsPcsFuncsPtr));      /* XPCS */
    CHECK_STATUS(mvHwsGPcs28nmIfInit(hwsPcsFuncsPtr));  /* GPCS */
    CHECK_STATUS(mvHwsMMPcs28nmIfInit(hwsPcsFuncsPtr)); /* MMPCS */
#ifndef MV_HWS_REDUCED_BUILD
    /* ILKN configuration is not done in Internal CPU */
    CHECK_STATUS(mvHwsIlknPcsIfInit(hwsPcsFuncsPtr));   /* ILKN */
#endif
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
    CHECK_STATUS(hwsIlknMacIfInit(hwsMacFuncsPtr));           /* ILKN MAC */
#endif
    return GT_OK;
}

/*******************************************************************************
* mvHwsBobcat2CoreClockGet
*
* DESCRIPTION:
*       Return S@R core clock frequency.
*
* INPUTS:
*          devNum - system device number
*
* OUTPUTS:
*       coreClkPtr - core clock frequency
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS mvHwsBobcat2CoreClockGet
(
    GT_U8   devNum,
    GT_U32  *coreClkPtr
)
{
    GT_U32  data;

    /* get the PLL 0 Config value from server */
    hwsServerRegGetFuncPtr(devNum, SERVER_PLL0_CONFIG_REG, &data);
    data = (data >> 21) & 0x7;       /* bits 21-23 */

    if (data >= BC2_MAX_CORE_CLOCK)
    {
        return GT_BAD_STATE;
    }

    *coreClkPtr = (HWS_DEV_IS_BOBCAT2_B0(devNum)) ? bobcat2B0CoreClkOptions[data] : bobcat2A0CoreClkOptions[data];

    return  GT_OK;
}

/*******************************************************************************
* mvHwsBobcat2ClkSelCfg
*
* DESCRIPTION:
*       Configures the DP/Core Clock Selector on port according to core clock,
*          port mode and port number.
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
static GT_STATUS mvHwsBobcat2ClkSelCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode
)
{
    MV_HWS_PORT_INIT_PARAMS   *curPortParams;
    GT_U32  coreClk;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* read core clock */
    CHECK_STATUS(mvHwsBobcat2CoreClockGet(devNum, &coreClk));

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
static GT_STATUS serdesIfInit(GT_U8 devNum, GT_U32 devRevision)
{
    MV_HWS_SERDES_FUNC_PTRS *hwsSerdesFuncsPtr;

    CHECK_STATUS(hwsSerdesGetFuncPtr(devNum, &hwsSerdesFuncsPtr));

    if (devRevision > 0)
    {
        CHECK_STATUS(mvHwsComH28nmRev3IfInit(hwsSerdesFuncsPtr));
    }
    else
    {
        CHECK_STATUS(mvHwsComH28nmIfInit(hwsSerdesFuncsPtr));
    }

    return GT_OK;
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
static GT_STATUS bc2RedundancyVectorGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                 *sdVector
)
{

    devNum = devNum;
    portGroup = portGroup;

    /* there is no Redundancy in BC2 */
    *sdVector = 0x0;

    return GT_OK;
}

#ifndef MV_HWS_REDUCED_BUILD
/*******************************************************************************
* Getbc2BoardType
*
* DESCRIPTION:
*       Get board Type
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
static GT_STATUS Getbc2BoardType(GT_U8 devNum, bc2BoardType *board)
{
    GT_U32 data = 0;
    GT_U32 portGroup = 0xFFFFFFFF;

    if (bobkSupported == 1 || hwsDeviceSpecInfo[devNum].gopRev >= GOP_28NM_REV2)
    {
        /* No LMS unit in BC2-B0 and BobK,
            so can't use BC2 addresses (that not exists any more) */
        *board = DB_BOARD;
        return GT_OK;
    }

    /* read phy to detect board */
    /* TBD should use SMI driver instead - SMI is not ready at this point yet*/

    CHECK_STATUS(genRegisterGet(devNum,  portGroup, 0x21004200, &data, 0));
    if (data & (1<<20))
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, 0x21004200, data & ~(1<<20), 0));
    }
    /* read phy 0 reg 2 for board type */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, 0x21004054, 0x4400000, 0));
    hwsOsExactDelayPtr(devNum,0,10);
    CHECK_STATUS(genRegisterGet(devNum,  portGroup, 0x21004054, &data, 0));

    /*DEBUG("phy board register 0x%x\n", data);*/

    data &= 0xffff;

    if (data == 0x0141)
    {
        /* RD board */
        /*DEBUG("RD Board identify\n");*/
        *board = RD_BOARD;
    }
    else
    {
        /* by default suppose this is DB */
        /* DB board */
        /*DEBUG("DB Board identify\n");*/
        *board = DB_BOARD;
    }

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
    /* DDR configuration is not done in Internal CPU */
    MV_HWS_TRAINING_IP_FUNC_PTRS *funcPtrArray;

    bc2BoardType board = DB_BOARD;

    CHECK_STATUS(Getbc2BoardType(devNum, &board));
    CHECK_STATUS(ddr3TipInitBc2(devNum, board));

    if (bobcatDbInitDone == GT_FALSE)
    {
        mvHwsTrainingIpGetFuncPtr(&funcPtrArray);
        mvHwsDdr3TipIfInit(funcPtrArray);
    }

    return GT_OK;
}
#endif /* MV_HWS_REDUCED_BUILD */

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
GT_STATUS hwsBobcat2IfInit(GT_U8 devNum, HWS_OS_FUNC_PTR *funcPtr)
{
    GT_U32  portGroup = devNum;
    GT_U32  portNum;
#ifndef MV_HWS_REDUCED_BUILD
    GT_U32  portMode;
    GT_STATUS res;
#endif /* MV_HWS_REDUCED_BUILD */
    GT_U32  data, version;
    MV_HWS_DEV_TYPE devType;
    MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

    if (hwsInitDone[devNum])
    {
        return GT_ALREADY_EXIST;
    }

    if (bobcatDbInitDone == GT_FALSE)
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

    hwsDeviceSpecInfo[devNum].devType = BobcatA0;
    hwsDeviceSpecInfo[devNum].devNum = devNum;
    hwsDeviceSpecInfo[devNum].portsNum = 72;
    hwsDeviceSpecInfo[devNum].lastSupPortMode = _48GBase_SR+1;
    hwsDeviceSpecInfo[devNum].serdesType = COM_PHY_28NM;

    devType = hwsDeviceSpecInfo[devNum].devType;

    /* define Host to HWS connection type  - now Legacy mode*/
    hwsDeviceSpecInfo[devNum].ipcConnType = HOST2HWS_LEGACY_MODE;

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

    if (version > 0)
    {
        hwsDeviceSpecInfo[devNum].gopRev = GOP_28NM_REV2;
        hwsDevFunc[devType].pcsMarkModeFunc = NULL;
        /* hwsDevFunc[devType].portBetterAdapSetFunc = mvHwsBobcat2PortBetterAdaptationSet; */
    }
    else
    {
        hwsDeviceSpecInfo[devNum].gopRev = GOP_28NM_REV1;
        hwsDevFunc[devType].pcsMarkModeFunc = mvHwsBobcat2PCSMarkModeSet;
        /* hwsDevFunc[devType].portBetterAdapSetFunc = NULL; */
    }

    hwsDevFunc[devType].redundVectorGetFunc = bc2RedundancyVectorGet;

    /* Configures the DP/Core Clock Selector on port */
    hwsDevFunc[devType].clkSelCfgGetFunc = mvHwsBobcat2ClkSelCfg;

    /* Configures port init / reset functions */
    hwsDevFunc[devType].devPortinitFunc  = mvHwsBobcat2PortInit;
    hwsDevFunc[devType].devPortResetFunc = mvHwsBobcat2PortReset;

    /* Configures  device handler functions */
    hwsDevFunc[devType].portAutoTuneSetExtFunc = mvHwsBobcat2PortAutoTuneSetExt;
    hwsDevFunc[devType].portExtModeCfgFunc     = mvHwsBobcat2PortExtendedModeCfg;
    hwsDevFunc[devType].portExtModeCfgGetFunc  = mvHwsBobcat2PortExtendedModeCfgGet;
    hwsDevFunc[devType].portFixAlign90Func     = mvHwsBobcat2PortFixAlign90Ext;
    hwsDevFunc[devType].portLbSetWaFunc        = NULL;
    hwsDevFunc[devType].portAutoTuneStopFunc   = NULL;
    hwsDevFunc[devType].portAutoTuneSetWaFunc  = NULL;


#ifndef CO_CPU_RUN

    /* Register silicon port modes parameters in the system */
    hwsInitPortsModesParam(devType, &hwsPortsBobcatParams[0]);

    if (bobcatDbInitDone == GT_FALSE)
    {
        hwsBobcatPorts47Cfg(devNum, portGroup);
        hwsBobcatPorts71Cfg(devNum, portGroup);
    }
#endif

    /* Init all MAC units relevant for current device */
    CHECK_STATUS(macIfInit(devNum));

    /* Init all PCS units relevant for current device */
    CHECK_STATUS(pcsIfInit(devNum));

    /* Init SERDES unit relevant for current device */
    CHECK_STATUS(serdesIfInit(devNum, version));

    /* Update HWS After Firmware load and init */
    if (mvHwsServiceCpuEnableGet(devNum))
        CHECK_STATUS(mvHwsServiceCpuFwPostInit(devNum));

    mvUnitInfoSet(devType, GEMAC_UNIT,       BOBCAT2_NW_GIG_BASE,        IND_OFFSET);
    mvUnitInfoSet(devType, MMPCS_UNIT,       BOBCAT2_NW_MPCS_BASE,       IND_OFFSET);
    mvUnitInfoSet(devType, XPCS_UNIT,        BOBCAT2_NW_XPCS_BASE,       IND_OFFSET);
    mvUnitInfoSet(devType, XLGMAC_UNIT,      BOBCAT2_NW_XLG_BASE,        IND_OFFSET);
    mvUnitInfoSet(devType, SERDES_UNIT,      BOBCAT2_SERDES_BASE,        IND_OFFSET);
    mvUnitInfoSet(devType, SERDES_PHY_UNIT,  BOBCAT2_SERDES_PHY_BASE,    IND_OFFSET);
    mvUnitInfoSet(devType, INTLKN_RF_UNIT,   BOBCAT2_ILKN_RF_BASE,       1 /* irrelevant since there is only 1 unit but needed for CPSS index check */);
    mvUnitInfoSet(devType, INTLKN_UNIT,      BOBCAT2_ILKN_BASE,          1 /* irrelevant since there is only 1 unit  but needed for CPSS index check */);
    
#ifndef MV_HWS_REDUCED_BUILD
    /* init active lines in ports DB */
    for (portNum = 0; (!bobcatDbInitDone) && (portNum < HWS_CORE_PORTS_NUM(devNum)); portNum++)
    {
        for (portMode = 0; portMode < HWS_DEV_PORT_MODES(devNum); portMode++)
        {
            res = mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
            if ((res != GT_OK) && (res != GT_NOT_SUPPORTED))
            {
                return res;
            }
        }
    }
#else
    /* Internal CPU receives the port parameters from the Host so no need to hold table per port mode */
    for (portNum = 0; portNum < HWS_CORE_PORTS_NUM(devNum); portNum++)
    {
        CHECK_STATUS(mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)0 /*portMode*/));
    }
#endif

    /* init ILKN reg DB (for write-only registers) */
    CHECK_STATUS(hwsILknRegDbInit(devNum));

#ifndef MV_HWS_REDUCED_BUILD
    /* DDR training initialization */
    res = TrainingIpIfInit(devNum);
    if ((res != GT_OK) && (res != GT_NOT_INITIALIZED))
    {
        return res;
    }
#endif /* MV_HWS_REDUCED_BUILD */

    bobcatDbInitDone = GT_TRUE;
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
static void serdesIfClose(void)
{
    /*mvHwsComH28nmIfClose();*/
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
    hwsIlknPcsIfClose();
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
    hwsIlknMacIfClose();
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
void hwsBobcat2IfClose(GT_U8 devNum)
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

    bobcatDbInitDone = GT_FALSE;

    serdesIfClose();
    pcsIfClose();
    macIfClose();

    /* free active lines allocated memory in ports DB */
    for (portNum = 0; (portNum < 12); portNum++)
    {
        for (portMode = 0; portMode < HWS_DEV_PORT_MODES(devNum); portMode++)
        {
            mvHwsDestroyActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
        }
    }

    /* un-register silicon port modes parameters in the system */
    hwsInitPortsModesParam(hwsDeviceSpecInfo[devNum].devType, 0);
}
#endif /* MV_HWS_REDUCED_BUILD */
static MV_INTLKN_REG* ilknRegDbGet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address)
{
    /* avoid warnings */
    portGroup = portGroup;
    address = address;

    return deviceInlkRegsShadow[devNum];
}

GT_STATUS hwsILknRegDbInit(GT_U8 devNum)
{
    GT_U8 shadowInd;

#ifdef VIPS_RUN
    shadowInd = devNum / 2;
#else
    shadowInd = devNum;
#endif

    /* pointer to a function which returns the ILKN registers DB */
    hwsIlknRegDbGetFuncPtr = ilknRegDbGet;

    if (deviceInlkRegsShadow[shadowInd] == NULL )
    {
        /* init device registers shadow with default values */
        deviceInlkRegsShadow[shadowInd] = (MV_INTLKN_REG *) hwsOsMallocFuncPtr(sizeof(MV_INTLKN_REG));

        deviceInlkRegsShadow[shadowInd]->ilkn0MacCfg0 = 8705;      /*ILKN_0_MAC_CFG_0 = 0,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0ChFcCfg0 = 0;        /*ILKN_0_CH_FC_CFG_0 = 4,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0MacCfg2 = 268500992; /*ILKN_0_MAC_CFG_2 = 0xC,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0MacCfg3 = 2827;      /*ILKN_0_MAC_CFG_3 = 0x10,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0MacCfg4 = 131585;    /*ILKN_0_MAC_CFG_4 = 0x14,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0MacCfg6 = 0;         /*ILKN_0_MAC_CFG_6 = 0x1C,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0ChFcCfg1 = 65280;    /*ILKN_0_CH_FC_CFG_1 = 0x60,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0PcsCfg0 = 4128831;
        deviceInlkRegsShadow[shadowInd]->ilkn0PcsCfg1 = 16777215;  /*ILKN_0_PCS_CFG_1 = 0x204,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0En = 0;              /*ILKN_0_EN = 0x20C,*/
        deviceInlkRegsShadow[shadowInd]->ilkn0StatEn = 0;          /*ILKN_0_STAT_EN = 0x238,*/
    }

    return GT_OK;
}

