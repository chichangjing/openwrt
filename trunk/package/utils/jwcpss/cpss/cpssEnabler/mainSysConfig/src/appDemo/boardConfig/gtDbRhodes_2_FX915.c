/*******************************************************************************
*              Copyright 2002, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtDbRhodes_2_FX915.c
*
* DESCRIPTION:
*       Initialization functions for the FPGA boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.7 $
*
*******************************************************************************/
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/gtAppDemoXbarConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoFaConfig.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>
#include <cpss/exMx/exMxGen/config/cpssExMxCfgInit.h>

#include <rhodes.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/
extern APP_DEMO_PP_CONFIG appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];


/********* Global variables ***************************************************/
static GT_U8 numPpsOnBorad = 0;
static CPSS_PP_DEVICE_TYPE     localDeviceId = /*GT_PP_END*/0xFFFF; /* [FINDME][SergeyR]: Wasn't found in CPSS */

/*******************************************************************************
* getBoardInfo
*
* DESCRIPTION:
*       Return the board configuration info.
*
* INPUTS:
*       boardRevId  - The board revision Id.
*
* OUTPUTS:
*       isB2bSystem - GT_TRUE, the system is a B2B system.
*       numOfPp     - Number of Packet processors in system.
*       numOfFa     - Number of Fabric Adaptors in system.
*       numOfXbar   - Number of Crossbar devices in system.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getBoardInfo
(
    IN  GT_U8   boardRevId,
    OUT GT_U8   *numOfPp,
    OUT GT_U8   *numOfFa,
    OUT GT_U8   *numOfXbar,
    OUT GT_BOOL *isB2bSystem
)
{
    
    *numOfPp      = 0;
    numPpsOnBorad = 0;
    *numOfFa      = 2;
    *numOfXbar    = 0;
    *isB2bSystem = GT_FALSE;

    return GT_OK;
}



/*******************************************************************************
* getXbarCfgParams
*
* DESCRIPTION:
*       Return the parameters for xbarSysConfig().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*      xbarCoreSysCfg   - xbar sys config phase1 parameters struct.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getXbarCfgParams
(
    IN   GT_U8                   boardRevId,
    OUT  GT_XBAR_CORE_SYS_CFG    *xbarCoreSysCfg
)
{
    GT_XBAR_CORE_SYS_CFG  localXbarCfgParams = GT_XBAR_SYS_CFG_DEF;

    osMemCpy(xbarCoreSysCfg,&localXbarCfgParams,
             sizeof(GT_XBAR_CORE_SYS_CFG));

    return GT_OK;
}


/*******************************************************************************
* getFaPhase1Config
*
* DESCRIPTION:
*       Returns the configuration parameters for coreFaHwPhase1Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devIdx          - The Fa Index to get the parameters for.
*
* OUTPUTS:
*       faPhase1Params  - Phase1 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getFaPhase1Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devIdx,
    OUT GT_FA_PHASE1_INIT_PARAMS    *phase1Params
)
{
    GT_FA_PHASE1_INIT_PARAMS    localFaPh1Config = FA_PHASE1_DEFAULT;

    localFaPh1Config.devNum                 = devIdx;
    localFaPh1Config.intConfig.intType      = GT_INT_OTHER;
    localFaPh1Config.intConfig.intVecNum    = devIdx;
    localFaPh1Config.intConfig.intMask      = 0;
    localFaPh1Config.mgmtIf                 = CPSS_EXTDRV_MGMT_I2C_E; 

    if (devIdx == 0)
    {
        localFaPh1Config.baseAddr = 0xb0;
    }
    else
    {
        localFaPh1Config.baseAddr = 0xb8;
    }

    localFaPh1Config.bufferDramCfg.dramTiming.trp         = 0x1;
    localFaPh1Config.bufferDramCfg.dramTiming.readCount   = 0x7;
    localFaPh1Config.bufferDramCfg.dramTiming.trrd        = 0x1;
    localFaPh1Config.bufferDramCfg.dramTiming.tras        = 0x7;
    localFaPh1Config.bufferDramCfg.mrs                    = 0x41;
    localFaPh1Config.bufferDramCfg.refreshCounter         = 0x8C0;
    localFaPh1Config.bufferDramCfg.operation              = 0x0;
    localFaPh1Config.bufferDramCfg.dramTiming.trfc        = 0x5;
    localFaPh1Config.bufferDramCfg.dramTiming.tdqss       = 0x0;
    localFaPh1Config.bufferDramCfg.dramTiming.rd2wrDelay  = 0x1;
    localFaPh1Config.bufferDramCfg.dramTiming.twtr        = 0x3;
    localFaPh1Config.bufferDramCfg.overwriteDef           = GT_TRUE;
    localFaPh1Config.bufferDramCfg.dramTiming.tmrd        = 0x0;
    localFaPh1Config.bufferDramCfg.dramTiming.trcd        = 0x3;
    localFaPh1Config.bufferDramCfg.trCdWr                 = 0x0;
    localFaPh1Config.bufferDramCfg.dramTiming.twr         = 0x2;
    localFaPh1Config.bufferDramCfg.dramFrequency          = 0x12C;
    localFaPh1Config.bufferDramCfg.emrs                   = 0x2;
    localFaPh1Config.bufferDramCfg.dramTiming.rd2rdDelay  = 0x0;
    localFaPh1Config.bufferDramCfg.dramTiming.writeCount  = 0x7;

    osMemCpy(phase1Params,&localFaPh1Config,sizeof(GT_FA_PHASE1_INIT_PARAMS));
    return GT_OK;
}

/*******************************************************************************
* getXbarPhase1Config
*
* DESCRIPTION:
*       Returns the configuration parameters for coreFaHwPhase1Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devIdx          - The Fa Index to get the parameters for.
*
* OUTPUTS:
*       faPhase1Params  - Phase1 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getXbarPhase1Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devIdx,
    OUT GT_XBAR_PHASE1_INIT_PARAMS  *phase1Params
)
{
    GT_XBAR_PHASE1_INIT_PARAMS  localXbarFoxPh1Config =
                                    GT_XBAR_FOX_PHASE1_DEF_PARAMS;
    GT_XBAR_DEVICE              xbarDeviceType;
    void    *intVecNum;
    GT_U8 i;
    GT_HG_INIT_PARAMS hgTigerInitParamsGtFx900 =
    {
        0,                           /* fport                    */
        GT_TRUE,                     /* Fport is valid           */
        GT_TRUE,                     /* swap lane order          */
        0x6,                         /* Number of hg links lanes */
        0x1C,                        /* pre emphasis             */
        0x1F,                        /* clock speed              */
        GT_FALSE,                    /* repeater mode            */
        GT_HG_NOT_VALID_VCM          /* voltage mode             */
    };

    if(boardRevId == 4)
    {
        hgTigerInitParamsGtFx900.numHglinkLanes =  GT_HG_4_LANES;
    }

    xbarDeviceType     = GT_XBAR_DEF_DEV_TYPE;
    localXbarFoxPh1Config.hgParamsListLen = 4;
    localXbarFoxPh1Config.hgParamsList =
                          osMalloc(localXbarFoxPh1Config.hgParamsListLen *
                          sizeof(GT_HG_INIT_PARAMS));

    if(localXbarFoxPh1Config.hgParamsList == NULL)
    {
        return GT_FAIL;
    }

    for(i = 0; i < 4; i++)
    {
        osMemCpy(&localXbarFoxPh1Config.hgParamsList[i],
                 &hgTigerInitParamsGtFx900,
                 sizeof(GT_HG_INIT_PARAMS));
        localXbarFoxPh1Config.hgParamsList[i].fport = i;
    }

    /*GH 3 is not valid */
    localXbarFoxPh1Config.hgParamsList[2].enable = GT_FALSE;

    localXbarFoxPh1Config.devNum = devIdx;



    if(xbarDeviceType == GT_XBAR_98FX9010A)
    {
        localXbarFoxPh1Config.intConfig.intType = GT_INT_2_PCI_D;
        extDrvGetPciIntVec(GT_PCI_INT_D,&intVecNum);
        localXbarFoxPh1Config.intConfig.intVecNum  = (GT_U32)intVecNum;
        extDrvGetIntMask(GT_PCI_INT_D,&(localXbarFoxPh1Config.intConfig.intMask));
    }
    osMemCpy(phase1Params,&localXbarFoxPh1Config,
             sizeof(GT_XBAR_PHASE1_INIT_PARAMS));
    return GT_OK;
}

/*******************************************************************************
* configBoardAfterPhase1
*
* DESCRIPTION:
*       This function performs all needed configurations that should be done
*       after phase1.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS configBoardAfterPhase1
(
    IN  GT_U8   boardRevId
)
{
    return GT_OK;
}


/*******************************************************************************
* prvExtDrvMgmtWriteRegister
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvExtDrvMgmtWriteRegister
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
	GT_STATUS    rc;
	GT_DEV_ID    devId;

	devId.i2cBaseAddr = (GT_U16)devSlvId;

	rc = extDrvMgmtWriteRegister(GT_MGMT_I2C, devId, regAddr, value);

    return rc;
}

/*******************************************************************************
* prvExtDrvMgmtReadRegister
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvExtDrvMgmtReadRegister
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    OUT GT_U32 *value
)
{
	GT_STATUS    rc;
	GT_DEV_ID    devId;

	devId.i2cBaseAddr = (GT_U16)devSlvId;

	rc = extDrvMgmtReadRegister(GT_MGMT_I2C, devId, regAddr, value);

    return rc;
}


/*******************************************************************************
* configBoardAfterPhase2
*
* DESCRIPTION:
*       This function performs all needed configurations that should be done
*       after phase2.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS configBoardAfterPhase2
(
    IN  GT_U8   boardRevId
)
{
    GT_STATUS                   rc;
    RHODES_EXT_DRV_BIND_STC     extDrvFuncBindInfo;

    RHODES_HW_ACCESS_PARAMS_STC hwAccessParams;

    osMemSet(&extDrvFuncBindInfo,0,sizeof(extDrvFuncBindInfo));
 
    
    extDrvFuncBindInfo.extDrvMemoryAlloc   = osMalloc;
    extDrvFuncBindInfo.extDrvMemoryFree    = osFree;
    extDrvFuncBindInfo.extDrvPrintf        = osPrintf;
    extDrvFuncBindInfo.extDrvWriteRegister = prvExtDrvMgmtWriteRegister;
    extDrvFuncBindInfo.extDrvReadRegister  = prvExtDrvMgmtReadRegister;

    /* perform Rhodes bind */
    rc = rhodesExtServicesBind(&extDrvFuncBindInfo);

    if(rc != GT_OK)
        return rc;

    hwAccessParams.baseAddr = 0xBE;
    hwAccessParams.mngInterfaceType = RHODES_INTERFACE_CHANNEL_I2C_E;
    /* rhodes init */
    rc = rhodesInit(0, 15, &hwAccessParams);
 
    return rc;
    
}

/*******************************************************************************
* getFaPhase2Config
*
* DESCRIPTION:
*       Returns the configuration parameters for coreFaHwPhase2Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       oldDevNum       - The old Pp device number to get the parameters for.
*
* OUTPUTS:
*       ppPhase2Params  - Phase2 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getFaPhase2Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       oldDevNum,
    OUT GT_FA_PHASE2_INIT_PARAMS    *phase2Params
)
{
    phase2Params->devNum    =  oldDevNum;
    phase2Params->oldDevNum =  oldDevNum;
    return GT_OK;
}

/*******************************************************************************
* getXbarPhase2Config
*
* DESCRIPTION:
*       Returns the configuration parameters for coreFaHwPhase2Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       oldDevNum       - The old Pp device number to get the parameters for.
*
* OUTPUTS:
*       ppPhase2Params  - Phase2 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getXbarPhase2Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       oldDevNum,
    OUT GT_XBAR_PHASE2_INIT_PARAMS  *phase2Params
)
{
    phase2Params->devNum    =  oldDevNum;
    phase2Params->oldDevNum =  oldDevNum;
    return GT_OK;
}



/*******************************************************************************
* getTapiLibInitParams
*
* DESCRIPTION:
*       Returns Tapi library initialization parameters.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - The Pp device number to get the parameters for.
*
* OUTPUTS:
*       libInitParams   - Library initialization parameters.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getTapiLibInitParams
(
    IN  GT_U8                       boardRevId,
	IN  GT_U8                       devNum,
    OUT APP_DEMO_LIB_INIT_PARAMS   *libInitParams
)
{
    APP_DEMO_LIB_INIT_PARAMS  localLibInitParams = LIB_INIT_PARAMS_DEFAULT;

    localLibInitParams.initIpv4Tunnel  = GT_TRUE;
    localLibInitParams.initIpv6        = GT_TRUE;
    localLibInitParams.initMpls        = GT_FALSE;
    localLibInitParams.initIpv4Filter  = GT_TRUE;
    /*localLibInitParams.lastSearchSlot  = GT_PCL_SEARCH_SLOT1;*/
    osMemCpy(libInitParams,&localLibInitParams,sizeof(APP_DEMO_LIB_INIT_PARAMS));

    return GT_OK;
}

/*******************************************************************************
* afterInitBoardConfig
*
* DESCRIPTION:
*       PSS Board specific configurations that should be done after board
*       initialization.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS afterInitBoardConfig
(
    IN  GT_U8                       boardRevId
)
{
    return GT_OK;
}

/*******************************************************************************
* gtFpgaRhodesFaBoard
*
* DESCRIPTION:
*       Registeration function for the GTDB98EX116, GTDB98EX126, GTDB98EX136
*       boardS.
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       boardCfgFuncs   - Board configuration functions.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtFpgaRhodesFaBoard
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    if(boardCfgFuncs == NULL)
    {
        return GT_FAIL;
    }

    boardCfgFuncs->boardGetInfo                 = getBoardInfo;
    boardCfgFuncs->boardGetXbarCfgParams        = getXbarCfgParams;
    boardCfgFuncs->boardGetFaPh1Params          = getFaPhase1Config;
    boardCfgFuncs->boardGetXbarPh1Params        = getXbarPhase1Config;
    boardCfgFuncs->boardAfterPhase1Config       = configBoardAfterPhase1;
    boardCfgFuncs->boardGetFaPh2Params          = getFaPhase2Config;
    boardCfgFuncs->boardGetXbarPh2Params        = getXbarPhase2Config;
    boardCfgFuncs->boardAfterPhase2Config       = configBoardAfterPhase2;
    boardCfgFuncs->boardGetLibInitParams        = getTapiLibInitParams;
    boardCfgFuncs->boardAfterInitConfig         = afterInitBoardConfig;

    return GT_OK;
}
