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
* gtDbDx246-24G3XGConfig.c
*
* DESCRIPTION:
*       Initialization functions for the Cheetah DB-246-24G-3XG boards.
*       Cheetah -- SMI
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*
*
* Comments:
*   base on :
*   <appDemo/boardConfig/gtDbDx24124GConfig.c>   -- Salsa (SMI)
*   <appDemo/boardConfig/gtDbDx270-24G3XGConfig.c> -- cheetah -- PCI
*
*******************************************************************************/

/*#include <stdlib.h>*/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
/*[SL...] it's not ok - but need for LED_MAX_NUM_OF_CLASS*/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegs.h>


/*******************************************************************************
 * External variables
 ******************************************************************************/

/* memory allocation */
extern void *malloc(size_t);

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
    GT_BOOL     found = GT_FALSE;       /* is first device found        */

    appDemoSysConfig.cpuEtherPortUsed = GT_TRUE;
    appDemoSysConfig.cpuEtherInfo.devNum = NOT_INIT_CPU_ETHER_DEV;

    retVal = appDemoSysGetSmiInfo();
    if(retVal != GT_OK)
    {
        return retVal;
    }

    ppCounter = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            if(appDemoSysConfig.cpuEtherInfo.devNum == NOT_INIT_CPU_ETHER_DEV)
            {
                /* the first device is the one connected to CPU */
                appDemoSysConfig.cpuEtherInfo.devNum = appDemoPpConfigList[i].devNum;
            }

            /* the first device is the one connected to CPU */
            if (found == GT_FALSE)
            {
                appDemoPpConfigList[i].cpuPortMode = CPSS_NET_CPU_PORT_MODE_MII_E;
                found = GT_TRUE;
            }
            else
            {
                appDemoPpConfigList[i].cpuPortMode = CPSS_NET_CPU_PORT_MODE_NONE_E;
            }

            ppCounter++;
        }
    }

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    if(ppCounter > 1)
    {
        *isB2bSystem = GT_TRUE;
    }
    else
    {
        *isB2bSystem = GT_FALSE;
    }

    return GT_OK;
}


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

    localPpPh1Config.devNum                                = devIdx;
    localPpPh1Config.baseAddr         = appDemoPpConfigList[devIdx].smiInfo.smiIdSel;
    localPpPh1Config.internalPciBase  = 0x0;
    localPpPh1Config.deviceId         =
        ((appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.devId << 16) |
         (appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.vendorId));

    localPpPh1Config.intVecNum        = 12; /* GPIO */
    localPpPh1Config.intMask          = 12; /* GPIO */

    localPpPh1Config.mngInterfaceType                      = CPSS_CHANNEL_SMI_E;

    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

    osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));
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
    /* code from here moved to appDemoDxChFdbInit(...) */

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
    CPSS_RX_BUF_INFO_STC            * pRxBufInfo;
    GT_U32                      bufferSize;
    GT_U32                      txHdrBufSize;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.baseAddr   =
        appDemoPpConfigList[oldDevNum].smiInfo.smiIdSel;
    localPpPh2Config.deviceId   = CPSS_98DX250_CNS;  /* Cheetah 24G managed */

    /* Cheetah utilizes the Tx desc for Tx header usage (MarvellTag+Cookie).  */
    /* There are 100 Tx buffers in HW.                                      */
    cpssDxChHwTxDescSizeGet(localPpPh2Config.deviceId, &txHdrBufSize);
    bufferSize = txHdrBufSize * 100;
    localPpPh2Config.netIfCfg.txDescBlock     = malloc(bufferSize);
    localPpPh2Config.netIfCfg.txDescBlockSize = bufferSize;
    localPpPh2Config.netIfCfg.rxDescBlock     = 0;
    localPpPh2Config.netIfCfg.rxDescBlockSize = 0;

    pRxBufInfo = &localPpPh2Config.netIfCfg.rxBufInfo;
    pRxBufInfo->allocMethod = CPSS_RX_BUFF_STATIC_ALLOC_E;

    /* There are 100 Rx buffers in HW per queue. */
    bufferSize = RX_BUFF_SIZE_DEF * 100 * 4;
    pRxBufInfo->buffData.staticAlloc.rxBufBlockPtr     = malloc(bufferSize);
    pRxBufInfo->buffData.staticAlloc.rxBufBlockSize = bufferSize;

    localPpPh2Config.auqCfg.auDescBlock       = NULL;
    localPpPh2Config.auqCfg.auDescBlockSize   = 0;

    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));
    return GT_OK;
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
    GT_U8                   i;
    GT_U8                   j;
    GT_STATUS               retVal;
    GT_U8                   numOfPorts;
    CPSS_DXCH_LED_CLASS_CONF_STC   ledClassConf[LED_MAX_NUM_OF_CLASS]; /* Leds registers class data */
    CPSS_DXCH_LED_GROUP_CONF_STC   ledGroupConf[LED_MAX_NUM_OF_GROUP]; /* Leds registers group data */
    GT_U32                         confRregVal = 0x4109ff00;  /* Leds control registers data */

    osMemSet(ledClassConf, 0, sizeof(ledClassConf));
    /* ~Act & Blink */
    ledClassConf[4].classNum  = 4;
    ledClassConf[4].classConf = 0xE000;

    /* PD(0) */
    ledClassConf[6].classNum  = 6;
    ledClassConf[6].classConf = 0x1000;

    osMemSet(ledGroupConf, 0, sizeof(ledGroupConf));
    /* G0: A - '1' 100Mbs /  B - '1' 100Mbs C - 'b' 10Mbs / D  - 'b'10 Mbs
       G1: A - '0' ~1000Mbs / B - '0' ~1000Mbs  C - '0' ~1000Mbs / D - '0' ~1000Mbs
    */
    ledGroupConf[0].groupNum  = 0;
    ledGroupConf[0].groupConf = 0x14B4;
    ledGroupConf[1].groupNum  = 1;
    ledGroupConf[1].groupConf = 0x4040;

    /* full duplex green/yellow */
    ledGroupConf[2].groupNum  = 2;
    ledGroupConf[2].groupConf = 0x5555;
    ledGroupConf[3].groupNum  = 3;
    ledGroupConf[3].groupConf = 0x2222;


    if (appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX107_CNS)
    {
        numOfPorts = 10;
    }
    else if ((appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX163_CNS) ||
             (appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX166_CNS) ||
             (appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX167_CNS))
    {
        numOfPorts = 16;
    }
    else
    {
        numOfPorts = 24; /* it isn't yet init in appDemoPpConfigList[i].ppinfo */
    }

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            /* configure leds, led organized by port (port data used) */
            /*in pss - ledStreamClassAndGroupConfig*/
            retVal = cpssDxChLedStreamClassAndGroupConfig(appDemoPpConfigList[i].devNum, confRregVal,
                                                  ledClassConf, LED_MAX_NUM_OF_CLASS,
                                                  ledGroupConf, LED_MAX_NUM_OF_GROUP);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            for(j = 0; j < numOfPorts; j++)
            {
                /* init the phy location on the board */
                retVal = appDemoSetPortPhyAddr(appDemoPpConfigList[i].devNum,j,
                                            (GT_U8)(0x8 + j));
                if(retVal != GT_OK)
                {
                    return retVal;
                }
            }
        }
    }

    return GT_OK;
}


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
    localPpCfgParams.maxNumOfIpv4Prefixes = 900;
    localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;

    /* The maximum number of counters for IP packets  */
    /*localPpCfgParams.maxIpPceCntr = 1;*/
    localPpCfgParams.miiNumOfTxDesc = TX_DESC_NUM_DEF;
    localPpCfgParams.miiTxInternalBufBlockSize = (TX_DESC_NUM_DEF / 2) * 
        CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS;
    localPpCfgParams.miiRxBufSize = RX_BUFF_SIZE_DEF;
    localPpCfgParams.miiRxBufBlockSize = RX_DESC_NUM_DEF * RX_BUFF_SIZE_DEF;

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

    localLibInitParams.initClassifier            = GT_FALSE;
    localLibInitParams.initIpv4                  = GT_FALSE;
    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
    localLibInitParams.initMpls                  = GT_FALSE;
    localLibInitParams.initMplsTunnel            = GT_FALSE;
    localLibInitParams.initPcl                   = GT_TRUE;
    localLibInitParams.initIpv4                  = GT_TRUE;
    localLibInitParams.initIpv6                  = GT_TRUE;

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
    /* code from here moved to appDemoDxChFdbInit(...) */

    return GT_OK;
}

/*******************************************************************************
* gtDbDx24624G3XGBoardReg
*
* DESCRIPTION:
*       Registration function for the DB-246-24G-3XG board. -- Cheetah - SMI
*
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
GT_STATUS gtDbDx24624G3XGBoardReg
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
    boardCfgFuncs->boardGetPpPh1Params          = getPpPhase1Config;
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


