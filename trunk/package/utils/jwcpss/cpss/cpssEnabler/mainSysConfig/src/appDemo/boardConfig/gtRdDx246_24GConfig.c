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
* gtRdDx246-24GConfig.c
*
* DESCRIPTION:
*       Initialization functions for the Cheetah RD-246-24G boards.
*       Cheetah -- SMI
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*
*
* Comments:
*   base on :
*   <appDemo/boardConfig/gtDbDx24124GConfig.c>   -- Salsa (SMI)
*   <appDemo/boardConfig/gtDbDx246-24G3XGConfig.c> -- cheetah -- SMI
*
*******************************************************************************/

/*#include <stdlib.h>*/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <gtExtDrv/drivers/gtSmiDrvCtrl.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/

/* memory allocation */
extern void *malloc(size_t);

static unsigned __TASKCONV comboPortPolling
(
    GT_VOID * unused
);

#ifdef ASIC_SIMULATION
    #define extDrvDirectSmiWriteReg(a,b,c)
    static GT_U32   currentPort23State=0;
#endif

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

    {
        /* External buffer Dram configurations  */
        localPpPh1Config.bufferDram = CPSS_BUFFER_DRAM_USE_DEFAULT_DRAM_CONF_E;
        localPpPh1Config.flowDram = CPSS_FLOW_DRAM_USE_DEFAULT_DRAM_CONF_E;
        localPpPh1Config.flowDramCfg.dramSize                   =
        localPpPh1Config.bufferDramCfg.dramSize                 = CPSS_DRAM_NOT_USED_E;
        localPpPh1Config.flowDramCfg.dramFrequency              =
        localPpPh1Config.bufferDramCfg.dramFrequency            = CPSS_DRAM_FREQ_300_MHZ_E;
        localPpPh1Config.flowDramCfg.overwriteDef               =
        localPpPh1Config.bufferDramCfg.overwriteDef             = GT_FALSE;
        localPpPh1Config.flowDramCfg.mrs                        =
        localPpPh1Config.bufferDramCfg.mrs                      = 0x0;
        localPpPh1Config.flowDramCfg.emrs                       =
        localPpPh1Config.bufferDramCfg.emrs                     = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.tdqss           =
        localPpPh1Config.bufferDramCfg.dramTiming.tdqss         = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.trcd            =
        localPpPh1Config.bufferDramCfg.dramTiming.trcd          = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.trp             =
        localPpPh1Config.bufferDramCfg.dramTiming.trp           = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.twr             =
        localPpPh1Config.bufferDramCfg.dramTiming.twr           = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.twtr            =
        localPpPh1Config.bufferDramCfg.dramTiming.twtr          = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.tras            =
        localPpPh1Config.bufferDramCfg.dramTiming.tras          = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.trrd            =
        localPpPh1Config.bufferDramCfg.dramTiming.trrd          = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.tmrd            =
        localPpPh1Config.bufferDramCfg.dramTiming.tmrd          = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.trfc            =
        localPpPh1Config.bufferDramCfg.dramTiming.trfc          = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.rd2rdDelay      =
        localPpPh1Config.bufferDramCfg.dramTiming.rd2rdDelay    = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.rd2wrDelay      =
        localPpPh1Config.bufferDramCfg.dramTiming.rd2wrDelay    = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.writeCount      =
        localPpPh1Config.bufferDramCfg.dramTiming.writeCount    = 0x0;
        localPpPh1Config.flowDramCfg.dramTiming.readCount       =
        localPpPh1Config.bufferDramCfg.dramTiming.readCount     = 0x0;
        localPpPh1Config.flowDramCfg.refreshCounter             =
        localPpPh1Config.bufferDramCfg.refreshCounter           = 0x0;
        localPpPh1Config.flowDramCfg.trCdWr                     =
        localPpPh1Config.bufferDramCfg.trCdWr                   = 0x0;
        localPpPh1Config.flowDramCfg.operation                  =
        localPpPh1Config.bufferDramCfg.operation                = 0x0;

        localPpPh1Config.bufferMgmtCfg.overwriteDef             = GT_FALSE;
        localPpPh1Config.bufferMgmtCfg.numPpBuffers             = 0x0;
        localPpPh1Config.bufferMgmtCfg.numUplinkBuffers         = 0x0;
        localPpPh1Config.uplinkCfg                              = CPSS_PP_NO_UPLINK_E ;

        localPpPh1Config.bufferDramCfg.dramPageSize = CPSS_DRAM_PAGE_SIZE_512B_E;
        localPpPh1Config.flowDramCfg.dramPageSize = CPSS_DRAM_PAGE_SIZE_1K_E;
    }

    localPpPh1Config.uplinkCfg                             = CPSS_PP_NO_UPLINK_E;
    localPpPh1Config.mngInterfaceType                      = CPSS_CHANNEL_SMI_E;

    /* the CLK is 188 MHz */
    localPpPh1Config.coreClk = 188;

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
    CPSS_DXCH_LED_CLASS_CONF_STC   ledClassConf[1]; /* Leds registers class data */
    CPSS_DXCH_LED_GROUP_CONF_STC   ledGroupConf[4]; /* Leds registers group data */
    GT_U32                         confRregVal = 0x2121D7A8;  /* Leds control registers data */
                                                 /* fixed from 0x6121D7A8 */
    /* Blink enable and invert led output for   */
    /* Activity class indication (class 4)      */
    ledClassConf[0].classNum    = 4;
    ledClassConf[0].classConf   = 0xA000;  /* was 0xA000 in HW */

    /* group A: speed 1G */
    ledGroupConf[0].groupNum    = 0;
    ledGroupConf[0].groupConf   = 0xFFF0;  /* was 0x0000 in HW */

    /* group B: speed 100M */
    ledGroupConf[1].groupNum    = 1;
    ledGroupConf[1].groupConf   = 0xFFF1;  /* was 0x1111 in HW */

    /* group C: Link-Up && Activity */
    ledGroupConf[2].groupNum    = 2;
    ledGroupConf[2].groupConf   = 0xFF34; /* was 0x3434 in HW */

    /* group D: Collision || Full Duplex */
    ledGroupConf[3].groupNum    = 3;
    ledGroupConf[3].groupConf   = 0xF7F2; /* was 0x7722 in HW */

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
                                                  ledClassConf, 1,
                                                  ledGroupConf, 4);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            for(j = 0; j < numOfPorts; j++)
            {
                /* init the phy location on the baord*/
                retVal = appDemoSetPortPhyAddr(appDemoPpConfigList[i].devNum,j,
                                            (GT_U8)(0x4 + (j%12)));
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

    GT_TASK         comboPortPollingTid;

    return osTaskCreate("comboPoll",              /* Task Name      */
                          250,                      /* Task Priority  */
                          _8KB,                     /* Stack Size     */
                          comboPortPolling,         /* Starting Point */
                          (GT_VOID*)0,              /* Arguments list */
                          &comboPortPollingTid);    /* task ID        */
}

/*******************************************************************************
* gtRdDx246_24GBoardReg
*
* DESCRIPTION:
*       Registration function for the RD-246-24G board. -- Cheetah - SMI
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
GT_STATUS gtRdDx246_24GBoardReg
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


/*
 * Typedef: enum MEDIA_TYPE_ENT
 *
 * Description: media types
 *
 * Enumerations:
 *    COPPER_E          - copper media
 *    FIBER_E           - fiber media
 *    INVALID_MEDIA_E   - media not resolved
 */
typedef enum
{
    COPPER_E        = 0,
    FIBER_E         = 1,
    INVALID_MEDIA_E = 2
} MEDIA_TYPE_ENT;

/*
 * Typedef: enum SPEED_TYPE_ENT
 *
 * Description: media types
 *
 * Enumerations:
 *    SPEED_10_E        - 10 Mbps speed
 *    SPEED_100_E       - 100 Mbps speed
 *    SPEED_1000_E      - 1000 Mbps speed
 *    INVALID_SPEED_E   - speed not resolved
 */
typedef enum
{
    SPEED_10_E          = 0,
    SPEED_100_E         = 1,
    SPEED_1000_E        = 2,
    INVALID_SPEED_E     = 3
} SPEED_TYPE_ENT;


/*******************************************************************************
* comboPortPhy1145Reset
*
* DESCRIPTION:
*       Perform reset on 88E1145 phy of combo port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID comboPortPhy1145Reset(GT_VOID)
{
    GT_U16      val;

    (GT_VOID)cpssDxChPhyPortSmiRegisterRead(0, 23, 0, &val);
    val |= 0x8000;
    (GT_VOID)cpssDxChPhyPortSmiRegisterWrite(0, 23, 0, val);
}


/*******************************************************************************
* comboPortInit
*
* DESCRIPTION:
*       Perform init according to arrata on 88E1145 phy of combo port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID comboPortInit(GT_VOID)
{
    GT_U16  val;

    /*
     * C0 erratum treatment for phy init
     */

    /* Reg29 = 0x0006 */
    cpssDxChPhyPortSmiRegisterWrite(0, 23, 29, 0x0006);

    /* Reg30: bit14..15 = '01' */
    cpssDxChPhyPortSmiRegisterRead(0, 23, 30, & val);
    val &= 0x3FFF;
    val |= 0x4000;
    cpssDxChPhyPortSmiRegisterWrite(0, 23, 30, val);

    /* Reg29 = 0x000a */
    cpssDxChPhyPortSmiRegisterWrite(0, 23, 29, 0x000a);

    /* Reg30: bit5 = '0' */
    cpssDxChPhyPortSmiRegisterRead(0, 23, 30, & val);
    val &= 0xFFDF;
    cpssDxChPhyPortSmiRegisterWrite(0, 23, 30, val);

    /* apply software reset */
    comboPortPhy1145Reset();

    /*
     * C0 erratum treatment for RGMII for fiber
     */

    cpssDxChPhyPortSmiRegisterWrite(0, 23, 29, 0x001B);
    cpssDxChPhyPortSmiRegisterWrite(0, 23, 30, 0x418F);
    cpssDxChPhyPortSmiRegisterWrite(0, 23, 29, 0x0016);
    cpssDxChPhyPortSmiRegisterWrite(0, 23, 30, 0xA2DA);

    /* reset the phy after errrata treatment */
    comboPortPhy1145Reset();
}


/*******************************************************************************
* comboPortSpeedSet
*
* DESCRIPTION:
*       Set the speed on 88E1111 phy of combo port and reset 88E1145.
*
* INPUTS:
*       speed - the combo port desired speed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID comboPortSpeedSet
(
    IN  SPEED_TYPE_ENT    speed
)
{
    GT_U32      macIfSpeed;

    switch (speed)
    {
        /* 10 Mbps */
        case SPEED_10_E:
            macIfSpeed = 0x4;
            break;

        /* 100 Mbps */
        case SPEED_100_E:
            macIfSpeed = 0x5;
            break;

         /* 1000 Mbps */
        case SPEED_1000_E:
            macIfSpeed = 0x6;
            break;

        default:
            return;
    }

    macIfSpeed = (0x0C82 | (macIfSpeed << 4));

    /* set the 88E1111 speed and reset phy */
    extDrvDirectSmiWriteReg(0x14, 20, macIfSpeed);
    extDrvDirectSmiWriteReg(0x14, 0, 0x8140);

    /* restart 88E1145 phy */
    comboPortPhy1145Reset();
}


/*******************************************************************************
* comboPortMediaTypeSet
*
* DESCRIPTION:
*       Change the 88E1145 phy (combo port) Speed Auto-Negotiation and
*       advertised speed depending on media type. Set the Cheetah MAC port type
*       depending on media type.
*
* INPUTS:
*       mediaCurr - current media type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID comboPortMediaTypeSet
(
    MEDIA_TYPE_ENT  mediaCurr
)
{
    GT_U16      autoNegAdvertise;
    GT_U16      extndPhySpecificStatus;
    GT_U32      regVal;

    (GT_VOID)cpssDxChPhyPortSmiRegisterRead(0, 23, 4, &autoNegAdvertise);

    if (COPPER_E == mediaCurr)
    {
        /* enable speed Auto-Negotiation */
        (GT_VOID)cpssDxChPortSpeedAutoNegEnableSet(0, 23, GT_TRUE);

        /* advertise 100/10BASE-X FD/HD */
        autoNegAdvertise = (autoNegAdvertise & 0xFE00) | 0x1E1;

        /* disable fiber/copper auto select, select copper AN */
        extndPhySpecificStatus = 0x800B;
    }
    else
    {
        /* disable speed Auto-Negotiation */
        (GT_VOID)cpssDxChPortSpeedAutoNegEnableSet(0, 23, GT_FALSE);

        /* set port speed to 1000M */
        (GT_VOID)cpssDxChPortSpeedSet(0, 23, CPSS_PORT_SPEED_1000_E);

        /* advertise 1000BASE-X FD */
        autoNegAdvertise = (autoNegAdvertise & 0xFF9F) | 0x60;

        /* disable fiber/copper auto select, select fiber AN */
        extndPhySpecificStatus = 0xC003;
    }

    (GT_VOID)cpssDxChPhyPortSmiRegisterWrite(0, 23, 4, autoNegAdvertise);
    (GT_VOID)cpssDxChPhyPortSmiRegisterWrite(0, 23, 27, extndPhySpecificStatus);

    /* reset 1145 phy */
    comboPortPhy1145Reset();

    /* change the Cheetah MAC port type setting (copper/fiber) */
    (GT_VOID)cpssDrvPpHwRegisterRead(0, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,0x0A801700, &regVal);

    if (COPPER_E == mediaCurr)
        regVal &= ~0x2;/* bit 1 */
    else
        regVal |= 0x2;/* bit 1 */

    (GT_VOID)cpssDrvPpHwRegisterWrite(0, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,0x0A801700, regVal);
}


/*******************************************************************************
* comboPortPolling
*
* DESCRIPTION:
*       Perform continuous polling of the combo port. When an SFP with link is
*       inserted in fiber port, the fiber is activated at 1000M speed.
*       Whenever a fiber link does not exist, the copper port is activated.
*       When the speed changes on the copper link the PHY 88E1111 is configured
*       to operate at the speed the 88E1145 phy. The 88E1145 phy speed is the
*       reference that the link speed changed.
*
* INPUTS:
*       unused - this argument is not used.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static unsigned __TASKCONV comboPortPolling
(
    GT_VOID * unused
)
{
    SPEED_TYPE_ENT      speedCurr;
    SPEED_TYPE_ENT      speedHistory;
    MEDIA_TYPE_ENT      mediaCurr;
    MEDIA_TYPE_ENT      mediaHistory;
    GT_U16              regVal;

    mediaHistory    = INVALID_MEDIA_E;
    speedHistory    = INVALID_SPEED_E;

    /* set the 88E1111 MDI pair select mode (1 pair) */
    extDrvDirectSmiWriteReg(0x14, 22, 1);

    comboPortInit();

    while (1)
    {
        osTimerWkAfter(2000);

#ifndef ASIC_SIMULATION
        /* read FireFox GPOI Input Register */
        regVal = *((GT_U16*)0x8000D010);
        /* set the current media type */
        if ((regVal & 0x01A0) == 0)
        {
            mediaCurr = FIBER_E;
        }
        else
        {
            mediaCurr = COPPER_E;
        }

#else /*ASIC_SIMULATION*/
        /* in simulation , change the state every loop */
        /* this allow to debug the code a bit ... */
        mediaCurr = (currentPort23State++ & 0x1) ? COPPER_E : FIBER_E;
#endif /*ASIC_SIMULATION*/

        /* change fiber/copper mode according to SFP presence and link */
        if (mediaHistory != mediaCurr)
        {
            /* set the current media type */
            comboPortMediaTypeSet(mediaCurr);

            /* media type changed, speed is invalidated */
            speedHistory = INVALID_SPEED_E;

            /* save the current media type */
            mediaHistory = mediaCurr;

            /* allow the speed AN for copper to finish */
            if (COPPER_E == mediaCurr)
                continue;
        }

        /* speed setting for copper/fiber mode */
        if (COPPER_E == mediaCurr)
        {
            /* get the link status from 1145 phy */
            cpssDxChPhyPortSmiRegisterRead(0, 23, 1, &regVal);

            /* validate link is up */
            if ((regVal & 0x0004) == 0)
                continue;

            /* link is up, get the current speed */
            cpssDxChPhyPortSmiRegisterRead(0, 23, 17, &regVal);

            speedCurr = ((regVal >> 14) & 0x3);

            if (speedHistory != speedCurr)
            {
                /* speed has changed */
                comboPortSpeedSet(speedCurr);
                speedHistory = speedCurr;
            }
        }
        else if (FIBER_E == mediaCurr)
        {
            if (INVALID_SPEED_E == speedHistory)
            {
                speedHistory = speedCurr = SPEED_1000_E;
                comboPortSpeedSet(speedCurr);
            }
        }
    }
    
#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif
}

