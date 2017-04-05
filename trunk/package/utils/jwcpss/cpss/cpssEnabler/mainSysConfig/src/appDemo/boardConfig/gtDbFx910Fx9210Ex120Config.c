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
* gtDbFx910Fx9210Ex620Config.c
*
* DESCRIPTION:
*       Initialization functions for the DB-FX910-FX9210-EX120 boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <appDemo/os/appOs.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/gtAppDemoXbarConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoFaConfig.h>

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/
extern APP_DEMO_PP_CONFIG    appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];


/********* Global variables ***************************************************/
static GT_U8 numPpsOnBorad = 0;

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

    if((boardRevId != 3) && (boardRevId != 4))
    {
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
        *numOfFa      = ppCounter;
        *numOfXbar    = ppCounter + 1;


        *isB2bSystem = GT_FALSE;

    }
    else
    {
           *numOfPp      = 0;
           numPpsOnBorad = 0;
           *numOfFa      = 0;
           *numOfXbar    = 1;

    }

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
    OUT CPSS_PP_PHASE1_INIT_PARAMS  *phase1Params
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

    extDrvGetPciIntVec(GT_PCI_INT_D,&intVecNum);
    localPpPh1Config.intVecNum  = (GT_U32)intVecNum;
    extDrvGetIntMask(GT_PCI_INT_D,&(localPpPh1Config.intMask));

    localPpPh1Config.uplinkCfg  = CPSS_PP_UPLINK_FA_E;

    /* External buffer Dram configurations  */
    localPpPh1Config.bufferDram = CPSS_BUFFER_DRAM_USE_DEFAULT_DRAM_CONF_E;
    localPpPh1Config.bufferDramCfg.overwriteDef             = GT_TRUE;
    localPpPh1Config.bufferDramCfg.mrs                      = 0x51;
    localPpPh1Config.bufferDramCfg.emrs                     = 0x0;
    localPpPh1Config.bufferDramCfg.dramTiming.tdqss         = 0x0;
    localPpPh1Config.bufferDramCfg.dramTiming.trcd          = 0x5;
    localPpPh1Config.bufferDramCfg.dramTiming.trp           = 0x2;
    localPpPh1Config.bufferDramCfg.dramTiming.twr           = 0x2;
    localPpPh1Config.bufferDramCfg.dramTiming.twtr          = 0x3;
    localPpPh1Config.bufferDramCfg.dramTiming.tras          = 0xA;
    localPpPh1Config.bufferDramCfg.dramTiming.trrd          = 0x1;
    localPpPh1Config.bufferDramCfg.dramTiming.tmrd          = 0x0;

    localPpPh1Config.bufferDramCfg.dramTiming.trfc          = 0x9;
    localPpPh1Config.bufferDramCfg.dramTiming.rd2rdDelay    = 0x0;
    localPpPh1Config.bufferDramCfg.dramTiming.rd2wrDelay    = 0x0;

    localPpPh1Config.bufferDramCfg.dramTiming.writeCount    = 0x3;
    localPpPh1Config.bufferDramCfg.dramTiming.readCount     = 0x3;

    localPpPh1Config.bufferDramCfg.operation                = 0x0;
    localPpPh1Config.bufferDramCfg.trCdWr                   = 0x0;

    /* Set the refresh count according to the BD frequency. */
    localPpPh1Config.bufferDramCfg.dramFrequency = CPSS_DRAM_FREQ_300_MHZ_E;
    switch(localPpPh1Config.bufferDramCfg.dramFrequency)
    {
        case(CPSS_DRAM_FREQ_200_MHZ_E):
            localPpPh1Config.bufferDramCfg.refreshCounter           = 0x550;
            break;
        case(CPSS_DRAM_FREQ_250_MHZ_E):
            localPpPh1Config.bufferDramCfg.refreshCounter           = 0x6D0;
            break;
        case(CPSS_DRAM_FREQ_300_MHZ_E):
            localPpPh1Config.bufferDramCfg.refreshCounter           = 0x850;
            break;
        default:
            localPpPh1Config.bufferDramCfg.refreshCounter           = 0xE00;
    }


    /* Flow Dram configurations  Samsung           */
    localPpPh1Config.flowDram = CPSS_FLOW_DRAM_USE_DEFAULT_DRAM_CONF_E;
    localPpPh1Config.flowDramCfg.overwriteDef             = GT_TRUE;
    localPpPh1Config.flowDramCfg.mrs                      = 0x31;
    localPpPh1Config.flowDramCfg.emrs                     = 0x2;
    localPpPh1Config.flowDramCfg.dramTiming.tdqss         = 0x0;
    localPpPh1Config.flowDramCfg.dramTiming.trcd          = 0x4;
    localPpPh1Config.flowDramCfg.dramTiming.trp           = 0x2;
    localPpPh1Config.flowDramCfg.dramTiming.twr           = 0x1;
    localPpPh1Config.flowDramCfg.dramTiming.twtr          = 0x3;
    localPpPh1Config.flowDramCfg.dramTiming.tras          = 0x7;
    localPpPh1Config.flowDramCfg.dramTiming.trrd          = 0x0;
    localPpPh1Config.flowDramCfg.dramTiming.tmrd          = 0x0;
    localPpPh1Config.flowDramCfg.dramTiming.trfc          = 0x8;
    localPpPh1Config.flowDramCfg.dramTiming.rd2rdDelay    = 0x0;
    localPpPh1Config.flowDramCfg.dramTiming.rd2wrDelay    = 0x0;
    localPpPh1Config.flowDramCfg.dramTiming.writeCount    = 0x3;
    localPpPh1Config.flowDramCfg.dramTiming.readCount     = 0x3;
    localPpPh1Config.flowDramCfg.operation                = 0x0;
    localPpPh1Config.flowDramCfg.trCdWr                   = 0x5;
    localPpPh1Config.flowDramCfg.dramFrequency = CPSS_DRAM_FREQ_250_MHZ_E;
    localPpPh1Config.flowDramCfg.refreshCounter           = 0xE70;

    localPpPh1Config.coreClk = 166;

    localPpPh1Config.bufferDramCfg.dramPageSize = CPSS_DRAM_PAGE_SIZE_512B_E;
    localPpPh1Config.flowDramCfg.dramPageSize = CPSS_DRAM_PAGE_SIZE_1K_E;
    localPpPh1Config.mngInterfaceType                     = CPSS_CHANNEL_PCI_E;

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
    GT_FA_PHASE1_INIT_PARAMS    localFaPh1Config = FA_PHASE1_DEFAULT;


    localFaPh1Config.devNum                 = devIdx;

    localFaPh1Config.intConfig.intType      = GT_INT_2_UPLINK_GPP;
    localFaPh1Config.intConfig.intVecNum    = devIdx;
    localFaPh1Config.bufferDramCfg.overwriteDef = GT_FALSE;

    osMemCpy(phase1Params,&localFaPh1Config,sizeof(GT_FA_PHASE1_INIT_PARAMS));
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
    GT_XBAR_PHASE1_INIT_PARAMS  localXbarFoxPh1Config =
                                        GT_XBAR_FOX_PHASE1_DEF_PARAMS;
    GT_XBAR_PHASE1_INIT_PARAMS  localXbarCapPh1Config =
                                        GT_XBAR_CAP_PHASE1_DEF_PARAMS;
    GT_XBAR_DEVICE              xbarDeviceType;
    void    *intVecNum;


    /****************CAP XBAR configuration ***********************************/
    if (devIdx >= numPpsOnBorad)
    {
        if((3 == boardRevId) || (4 == boardRevId))
        {
            xbarDeviceType = GT_XBAR_98FX9206;
            /* I2C address of the capoeira */
            localXbarCapPh1Config.i2cBaseAddr = 0x2;
        }
        else
        {
            xbarDeviceType = GT_XBAR_98FX9210;
            /* I2C address of the capoeira */
            localXbarCapPh1Config.i2cBaseAddr = 0x58;
        }

        localXbarCapPh1Config.hgParamsListLen = 12;
        localXbarCapPh1Config.hgParamsList = (GT_HG_INIT_PARAMS *)osMalloc(
        localXbarCapPh1Config.hgParamsListLen * sizeof(GT_HG_INIT_PARAMS));

        if(localXbarCapPh1Config.hgParamsList == NULL)
            return GT_OUT_OF_CPU_MEM;


        appDemoHgGetDefInitParams(xbarDeviceType,
                                  &localXbarCapPh1Config.hgParamsListLen,
                                  localXbarCapPh1Config.hgParamsList);

        localXbarCapPh1Config.devNum = devIdx;




        /* Interrupt parameters */
        localXbarCapPh1Config.intConfig.intVecNum = 24; /* interrupt 24 */
        localXbarCapPh1Config.intConfig.intMask   = 24;

        osMemCpy(phase1Params,&localXbarCapPh1Config,
             sizeof(GT_XBAR_PHASE1_INIT_PARAMS));

        return (GT_OK);
    }

    /*****************FOX XBAR configuration **********************************/

    xbarDeviceType     = GT_XBAR_DEF_DEV_TYPE;
    switch(boardRevId)
    {
        case 1:
            localXbarFoxPh1Config.hgParamsListLen = 4;
            localXbarFoxPh1Config.hgParamsList =
                osMalloc(localXbarFoxPh1Config.hgParamsListLen *
                         sizeof(GT_HG_INIT_PARAMS));

            if(localXbarFoxPh1Config.hgParamsList == NULL)
            {
                return GT_FAIL;
            }

            appDemoHgGetDefInitParams(xbarDeviceType,
                                      &localXbarFoxPh1Config.hgParamsListLen,
                                      localXbarFoxPh1Config.hgParamsList);

            localXbarFoxPh1Config.hgParamsList[2].swapLaneOrder = GT_TRUE;
            localXbarFoxPh1Config.devNum = devIdx;

            break;

        case 2:
            localXbarFoxPh1Config.hgParamsListLen = 4;
            localXbarFoxPh1Config.hgParamsList =
                osMalloc(localXbarFoxPh1Config.hgParamsListLen *
                         sizeof(GT_HG_INIT_PARAMS));

            if(localXbarFoxPh1Config.hgParamsList == NULL)
            {
                return GT_FAIL;
            }
            xbarDeviceType = GT_XBAR_98FX900A;

            appDemoHgGetDefInitParams(xbarDeviceType,
                                      &localXbarFoxPh1Config.hgParamsListLen,
                                      localXbarFoxPh1Config.hgParamsList);

            localXbarFoxPh1Config.devNum = devIdx;
            break;

        default:
            return GT_FAIL;
    }

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
    OUT CPSS_PP_PHASE2_INIT_PARAMS  *phase2Params
)
{
    CPSS_PP_PHASE2_INIT_PARAMS  localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS   retVal;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = CPSS_98EX120B_CNS;
    localPpPh2Config.baseAddr   =
        appDemoPpConfigList[oldDevNum].pciInfo.pciBaseAddr;

    retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF,RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,AU_DESC_NUM_DEF,
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


    phase2Params->devNum    =  oldDevNum;
    phase2Params->oldDevNum =  oldDevNum;
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


    phase2Params->devNum    =  oldDevNum;
    phase2Params->oldDevNum =  oldDevNum;
    return GT_OK;
}
#endif


/*******************************************************************************
* getTapiLibInitParams
*
* DESCRIPTION:
*       Returns Tapi library initialization parameters.
*
* INPUTS:
*       boardRevId      - The board revision Id.
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

    osMemCpy(libInitParams,&localLibInitParams,
             sizeof(APP_DEMO_LIB_INIT_PARAMS));
    return GT_OK;
}


/*******************************************************************************
* gtDbFx910Ex120ConfigBoardReg
*
* DESCRIPTION:
*       Registeration function for the DBFx910Fx9210Ex120 board.*
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
GT_STATUS gtDbFx910Fx9210Ex120CapConfigBoardReg
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
    /* boardCfgFuncs->boardGetCoreCfgPh1Params     = getCoreSysCfgPhase1; */
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
    /* boardCfgFuncs->boardGetCoreCfgPh2Params     = getCoreSysCfgPhase2; */
    boardCfgFuncs->boardGetPpPh2Params          = getPpPhase2Config;
    boardCfgFuncs->boardAfterPhase2Config       = configBoardAfterPhase2;
    /* boardCfgFuncs->boardGetSysCfgParams         = getSysConfigParams;    */
    /* boardCfgFuncs->boardGetPpLogInitParams      = getPpLogicalInitParams;*/
    boardCfgFuncs->boardGetLibInitParams        = getTapiLibInitParams;
    boardCfgFuncs->boardAfterInitConfig         = NULL;

    return GT_OK;
}



