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
* gtDbEx106Ex108Config.c
*
* DESCRIPTION:
*       Initialization functions for the DB-EX126-12G boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/gtAppDemoXbarConfig.h>

#include <cpssXbar/generic/xbarHwInit/gtCoreXbarHwInit.h>

#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>


/*******************************************************************************
 * External variables
 ******************************************************************************/
extern APP_DEMO_PP_CONFIG    appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];


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
    GT_STATUS   retVal;                 /* Function return value.       */
    GT_U8       ppCounter;              /* Number of Packet processors. */
    GT_U8       i;                      /* Loop index.                  */

    retVal = appDemoSysGetPciInfo();
    if(retVal != GT_OK)
    {
        return retVal;
    }

    ppCounter = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            ppCounter++;
        }
    }

    *numOfPp      = ppCounter;
    numPpsOnBorad = ppCounter;

    *numOfFa      = 0;
    *numOfXbar    = 0;

    *isB2bSystem = GT_FALSE;

    return GT_OK;
}


#ifdef IMPL_XBAR
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
#endif

/*******************************************************************************
* getPpPhase1Config
*
* DESCRIPTION:
*       Returns the configuration parameters for corePpHwPhase1Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devIdx          - The Pp Index to get the parameters for.
*
* OUTPUTS:
*       ppPhase1Params  - Phase1 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpPhase1Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devIdx,
    OUT CPSS_PP_PHASE1_INIT_PARAMS    *phase1Params
)
{
    CPSS_PP_PHASE1_INIT_PARAMS    localPpPh1Config = CPSS_PP_PHASE1_DEFAULT;
    void    *intVecNum;

    localPpPh1Config.devNum             = devIdx;
    localPpPh1Config.baseAddr           =
        appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
    localPpPh1Config.internalPciBase    =
        appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
    localPpPh1Config.deviceId           =
        ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
         (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

    localDeviceId = localPpPh1Config.deviceId;

    extDrvGetPciIntVec(GT_PCI_INT_D,&intVecNum);
    localPpPh1Config.intVecNum  = (GT_U32)intVecNum;
    extDrvGetIntMask(GT_PCI_INT_D,&(localPpPh1Config.intMask));

    localPpPh1Config.uplinkCfg  = CPSS_PP_NO_UPLINK_E;

    /* External buffer Dram configurations  */
    localPpPh1Config.bufferDram = CPSS_BUFFER_DRAM_USE_DEFAULT_DRAM_CONF_E;
    localPpPh1Config.bufferDramCfg.dramFrequency         = 0x12C;
    localPpPh1Config.bufferDramCfg.dramTiming.rd2rdDelay = 0x0;
    localPpPh1Config.bufferDramCfg.dramTiming.rd2wrDelay = 0x1;
    localPpPh1Config.bufferDramCfg.dramTiming.readCount  = 0x7;
    localPpPh1Config.bufferDramCfg.dramTiming.tdqss      = 0x0;
    localPpPh1Config.bufferDramCfg.dramTiming.tmrd       = 0x0;
    localPpPh1Config.bufferDramCfg.dramTiming.tras       = 0x7;
    localPpPh1Config.bufferDramCfg.dramTiming.trcd       = 0x3;
    localPpPh1Config.bufferDramCfg.dramTiming.trfc       = 0x5;
    localPpPh1Config.bufferDramCfg.dramTiming.trp        = 0x1;
    localPpPh1Config.bufferDramCfg.dramTiming.trrd       = 0x1;
    localPpPh1Config.bufferDramCfg.dramTiming.twr        = 0x2;
    localPpPh1Config.bufferDramCfg.dramTiming.twtr       = 0x1;
    localPpPh1Config.bufferDramCfg.dramTiming.writeCount = 0x7;
    localPpPh1Config.bufferDramCfg.emrs                  = 0x2;
    localPpPh1Config.bufferDramCfg.mrs                   = 0x40;
    localPpPh1Config.bufferDramCfg.operation             = 0x0;
    localPpPh1Config.bufferDramCfg.overwriteDef          = GT_TRUE;
    localPpPh1Config.bufferDramCfg.refreshCounter        = 0x8C0;
    localPpPh1Config.bufferDramCfg.trCdWr                = 0x0;

    /* No flowDram in 98EX_126 and 98EX136 */

    localPpPh1Config.sramsCfg[0].sramSize       = CPSS_SRAM_SIZE_2MB_E;
    localPpPh1Config.sramsCfg[0].sramFrequency  = CPSS_SRAM_FREQ_250_MHZ_E;
    localPpPh1Config.sramsCfg[0].sramRate       = CPSS_SRAM_RATE_DDR_E;
    localPpPh1Config.sramsCfg[0].externalMemory = CPSS_EXTERNAL_MEMORY_0_E;
    localPpPh1Config.sramsCfg[0].sramType       = CPSS_SRAM_TYPE_NARROW_E;

    if(boardRevId == 1)
    {
        localPpPh1Config.routingSramCfgType = CPSS_ONE_INTERNAL_E;
        localPpPh1Config.numOfSramsCfg = 0;
    }
    else
        return GT_BAD_PARAM;
                                                                /*fix CQ 41271,41370 */
    localPpPh1Config.coreClk                              = 142;/* 142MHz leads to 7 nano sec*/

    localPpPh1Config.bufferDramCfg.dramPageSize           = CPSS_DRAM_PAGE_SIZE_512B_E;
    localPpPh1Config.flowDramCfg.dramPageSize             = CPSS_DRAM_PAGE_SIZE_1K_E;
    localPpPh1Config.mngInterfaceType                     = CPSS_CHANNEL_PCI_E;


    if((localPpPh1Config.deviceId == CPSS_98EX106_CNS)||
       (localPpPh1Config.deviceId == CPSS_98EX108_CNS))
    {
        localPpPh1Config.flowDram = CPSS_FLOW_DRAM_USE_DEFAULT_DRAM_CONF_E;
        localPpPh1Config.flowDramCfg.dramSize              = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.trp        = 0x3;
        localPpPh1Config.flowDramCfg.dramTiming.readCount  = 0x3;
        localPpPh1Config.flowDramCfg.dramTiming.trrd       = 0x1;
        localPpPh1Config.flowDramCfg.dramTiming.tras       = 0x9;
        localPpPh1Config.flowDramCfg.mrs                   = 0x0;
        localPpPh1Config.flowDramCfg.refreshCounter        = 0x0;
        localPpPh1Config.flowDramCfg.operation             = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.trfc       = 0xF;
        localPpPh1Config.flowDramCfg.dramTiming.tdqss      = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.rd2wrDelay = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.twtr       = 0x1;
        localPpPh1Config.flowDramCfg.overwriteDef          = GT_FALSE;
        localPpPh1Config.flowDramCfg.dramTiming.tmrd       = 0x1;
        localPpPh1Config.flowDramCfg.dramTiming.trcd       = 0x3;
        localPpPh1Config.flowDramCfg.trCdWr                = 0x3;
        localPpPh1Config.flowDramCfg.dramTiming.twr        = 0x2;
        localPpPh1Config.flowDramCfg.dramFrequency         = 0xA6;
        localPpPh1Config.flowDramCfg.emrs                  = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.rd2rdDelay = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.writeCount = 0x3;

        localPpPh1Config.bufferDramCfg.mrs                 = 0x41;
        localPpPh1Config.bufferDramCfg.refreshCounter      = 0x73A;
        localPpPh1Config.bufferDramCfg.dramTiming.twtr     = 0x3;
        localPpPh1Config.bufferDramCfg.dramFrequency       = 0xFA;
        localPpPh1Config.bufferDramCfg.emrs                = 0x0;
    }

    osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));
    return GT_OK;
}

#ifdef IMPL_FA
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
    return GT_OK;
}
#endif

#if defined (IMPL_FA) || defined (IMPL_XBAR)
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
    return GT_OK;
}
#endif

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
* getPpPhase2Config
*
* DESCRIPTION:
*       Returns the configuration parameters for corePpHwPhase2Init().
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
static GT_STATUS getPpPhase2Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       oldDevNum,
    OUT CPSS_PP_PHASE2_INIT_PARAMS    *phase2Params
)
{
    CPSS_PP_PHASE2_INIT_PARAMS    localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS   retVal;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = appDemoPpConfigList[oldDevNum].deviceId;

    if((localPpPh2Config.deviceId != CPSS_98EX106_CNS) &&
       (localPpPh2Config.deviceId != CPSS_98EX108_CNS))
    {
        return GT_FAIL;
    }

    localPpPh2Config.baseAddr   =
        appDemoPpConfigList[oldDevNum].pciInfo.pciBaseAddr;

    retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId,
                                   RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF,
                                   RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,
                                   AU_DESC_NUM_DEF,
                                   &localPpPh2Config);

    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));
    return retVal;
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
    return GT_OK;
}

#ifdef IMPL_FA
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
    return GT_OK;
}
#endif

#if defined (IMPL_FA) || defined (IMPL_XBAR)
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
    return GT_OK;
}
#endif


/*******************************************************************************
* getPpLogicalInitParams
*
* DESCRIPTION:
*       Returns the parameters needed for sysPpLogicalInit() function.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - The Pp device number to get the parameters for.
*
* OUTPUTS:
*       ppLogInitParams - Pp logical init parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpLogicalInitParams
(
    IN  GT_U8           boardRevId,
    IN  GT_U8           devNum,
    OUT CPSS_PP_CONFIG_INIT_STC    *ppLogInitParams
)
{
    CPSS_PP_CONFIG_INIT_STC  localPpCfgParams = PP_LOGICAL_CONFIG_DEFAULT;

    /*localSysCfgParams.macTblSize = GT_MAC_TBL_16K;*/

    /* 2048 actions of PCEs, 2 for default PCl and IPV6MC */
    localPpCfgParams.maxNumOfPclAction = 2050;
    localPpCfgParams.pclActionSize = GT_PCL_ACTION_SIZE_16_BYTE;
    /* MPLS             */
    localPpCfgParams.maxNumOfNhlfe             = 0;
    localPpCfgParams.maxNumOfMplsIfs           = 0;

    osMemCpy(ppLogInitParams,&localPpCfgParams,sizeof(CPSS_PP_CONFIG_INIT_STC));
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
    OUT APP_DEMO_LIB_INIT_PARAMS    *libInitParams
)
{
    APP_DEMO_LIB_INIT_PARAMS  localLibInitParams = LIB_INIT_PARAMS_DEFAULT;

    /*localLibInitParams.lastSearchSlot  = GT_PCL_SEARCH_SLOT1;*/

    osMemCpy(libInitParams,&localLibInitParams,
                                              sizeof(APP_DEMO_LIB_INIT_PARAMS));

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
    /* Enable all AU messages types */
    /*return gfdbSetAu2CpuEnable(GT_MAC_AU_MESSAGE_ALL, GT_TRUE);*/

    GT_STATUS   rc = GT_OK;
    GT_U8       i;

    /* go over all devices and virtual ports per device */
    for (i = 0; (i < PRV_CPSS_MAX_PP_DEVICES_CNS) && (GT_OK == rc); i++)
    {
        if (GT_TRUE == appDemoPpConfigList[i].valid)
        {
            GT_U8 dev;
            GT_U8 port;
            GT_U8 numOfPorts;

            /* get device id and number of virtual ports per device */
            dev = appDemoPpConfigList[i].devNum;
            numOfPorts = PRV_CPSS_PP_MAC(dev)->numOfVirtPorts;

            for (port = 0; (port < numOfPorts) && (GT_OK == rc); port++)
            {
                rc = cpssExMxBrgFdbPortAuToCpuSet(dev, port, CPSS_MAC_AU_MESSAGE_ALL_E, GT_TRUE);
            }
        }
    }
    return rc;
}

/*******************************************************************************
* gtDbEx106Ex108BoardReg
*
* DESCRIPTION:
*       Registeration function for the GTDB98EX106, GTDB98EX108.
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
GT_STATUS gtDbEx106Ex108BoardReg
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
    /*boardCfgFuncs->boardGetCoreCfgPh1Params     = getCoreSysCfgPhase1;*/
#ifdef IMPL_XBAR
    boardCfgFuncs->boardGetXbarCfgParams        = getXbarCfgParams;
#endif
    boardCfgFuncs->boardGetPpPh1Params          = getPpPhase1Config;
#ifdef IMPL_FA
    boardCfgFuncs->boardGetFaPh1Params          = getFaPhase1Config;
    boardCfgFuncs->boardGetFaPh2Params          = getFaPhase2Config;
#endif
#if defined (IMPL_FA) || defined (IMPL_XBAR)
    boardCfgFuncs->boardGetXbarPh1Params        = getXbarPhase1Config;
    boardCfgFuncs->boardGetXbarPh2Params        = getXbarPhase2Config;
#endif
    boardCfgFuncs->boardAfterPhase1Config       = configBoardAfterPhase1;
    /*boardCfgFuncs->boardGetCoreCfgPh2Params     = getCoreSysCfgPhase2;*/
    boardCfgFuncs->boardGetPpPh2Params          = getPpPhase2Config;
    boardCfgFuncs->boardAfterPhase2Config       = configBoardAfterPhase2;
    /*boardCfgFuncs->boardGetSysCfgParams         = getSysConfigParams;*/
    boardCfgFuncs->boardGetPpLogInitParams      = getPpLogicalInitParams;
    boardCfgFuncs->boardGetLibInitParams        = getTapiLibInitParams;
    boardCfgFuncs->boardAfterInitConfig         = afterInitBoardConfig;

    return GT_OK;
}


