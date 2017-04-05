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
* gtDbDx270-24G3XGConfig.c
*
* DESCRIPTION:
*       Initialization functions for the Cheetah DB-270-24G-3XG boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>


/*******************************************************************************
 * External variables
 ******************************************************************************/

/* Init the SMI interface */
extern GT_STATUS hwIfSmiInitDriver
(
    GT_VOID
);
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
    GT_U8       devNum;                 /* Device number                */
    GT_STATUS   smiSupported;


#if !defined (ASIC_SIMULATION)
    smiSupported = hwIfSmiInitDriver();
#else
    smiSupported = GT_NOT_SUPPORTED;
#endif

    /* PCI devices must be probed first
     * because there is no way to list all devices on SMI bus
     */
        retVal = appDemoSysGetPciInfo();
    if(retVal != GT_OK && smiSupported == GT_NOT_SUPPORTED)
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

    /* If no PCI devices was found then SMI device will be tried */
    if (ppCounter == 0 && smiSupported != GT_NOT_SUPPORTED)
    {
        /* no PCI device found, try SMI */

        devNum  = SYSTEM_DEV_NUM_MAC(0);
        appDemoPpConfigList[devNum].channel = CPSS_CHANNEL_SMI_E;
        appDemoPpConfigList[devNum].devNum  = devNum;
        appDemoPpConfigList[devNum].smiInfo.smiDevVendorId.devId       = 0xD804;
        appDemoPpConfigList[devNum].smiInfo.smiDevVendorId.vendorId    = 0x11ab;
        appDemoPpConfigList[devNum].smiInfo.smiIdSel                   = 16 ;
        appDemoPpConfigList[devNum].valid = GT_TRUE;
        appDemoPpConfigList[devNum].devFamily = CPSS_PP_FAMILY_CHEETAH_E;

        appDemoSysConfig.cpuEtherPortUsed = GT_TRUE;
        appDemoSysConfig.cpuEtherInfo.devNum = devNum;
        ppCounter = 1;
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
    void    *intVecNum;

    localPpPh1Config.devNum                                = devIdx;

    if (appDemoPpConfigList[devIdx].channel == CPSS_CHANNEL_PCI_E)
    {
       localPpPh1Config.baseAddr =
                                appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
       localPpPh1Config.internalPciBase =
                            appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
       localPpPh1Config.deviceId =
            ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
             (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

#if defined(_linux) && defined(CPU_ARM) && !defined(ASIC_SIMULATION)
       /* the Socrates PCI bus irq number is not correct, so we set the */
       /* intVecNum hard coded.                                         */
       intVecNum = (void*)39;
       localPpPh1Config.intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
       localPpPh1Config.intMask   = (GT_U32)((GT_UINTPTR)intVecNum);
#else
       extDrvGetPciIntVec(GT_PCI_INT_D,&intVecNum);
       localPpPh1Config.intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
       extDrvGetIntMask(GT_PCI_INT_D,&(localPpPh1Config.intMask));
#endif
       localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PCI_E;
    }
    else
    {
       localPpPh1Config.baseAddr         =
                                   appDemoPpConfigList[devIdx].smiInfo.smiIdSel;
       localPpPh1Config.internalPciBase  = 0x0;
       localPpPh1Config.deviceId         =
        ((appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.devId << 16) |
         (appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.vendorId));

#if defined (CPU_946ES) || defined (ASIC_SIMULATION)
       /* FireFox or White Mode */
       localPpPh1Config.intVecNum        = 10; /* GPIO */
       localPpPh1Config.intMask          = 10; /* GPIO */
#else        /* Orion and other CPUs */
#if defined(_linux) && defined(CPU_ARM)
           /* Linux Orion SMI */
           localPpPh1Config.intVecNum        = 35;  /* GPIO */
           localPpPh1Config.intMask          = 35;  /* GPIO */
#else
           localPpPh1Config.intVecNum        = 3;  /* GPIO */
           localPpPh1Config.intMask          = 3;  /* GPIO */
#endif
#endif

       localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_SMI_E;
    }

    localPpPh1Config.uplinkCfg = CPSS_PP_NO_UPLINK_E;

    /* retrieve PP Core Clock from HW */
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
    OUT CPSS_PP_PHASE2_INIT_PARAMS  *phase2Params
)
{
    CPSS_PP_PHASE2_INIT_PARAMS    localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    CPSS_RX_BUF_INFO_STC          *pRxBufInfo;
    GT_U32                        bufferSize;
    GT_U32                        txHdrBufSize;
    GT_STATUS                     retVal;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = CPSS_98DX270_CNS;
    if(appDemoPpConfigList[oldDevNum].channel == CPSS_CHANNEL_PCI_E)
    {
        localPpPh2Config.baseAddr =
                            appDemoPpConfigList[oldDevNum].pciInfo.pciBaseAddr;
    }
    else
    {
        localPpPh2Config.baseAddr =
                            appDemoPpConfigList[oldDevNum].smiInfo.smiIdSel;
        /* Cheetah utilizes the Tx desc for Tx header usage (MarvellTag+Cookie).
           There are 100 Tx buffers in HW.                                   */

        /*coreGetTxDescSize(localPpPh2Config.deviceId, &txHdrBufSize);*/
        /* OK ![SergeyL] coreGetTxDescSize - not implemented(?) for DX, see  src\appDemo\boardConfig\appDemoCfgMisc.c  (589)*/

        cpssDxChHwTxDescSizeGet(localPpPh2Config.deviceId, &txHdrBufSize);

        bufferSize = txHdrBufSize * 100;
#if defined(_linux) || defined(_FreeBSD)
                localPpPh2Config.netIfCfg.txDescBlock     = osCacheDmaMalloc(bufferSize);
#else
                localPpPh2Config.netIfCfg.txDescBlock     = malloc(bufferSize);
#endif
        localPpPh2Config.netIfCfg.txDescBlockSize = bufferSize;
        localPpPh2Config.netIfCfg.rxDescBlock     = 0;
        localPpPh2Config.netIfCfg.rxDescBlockSize = 0;

        pRxBufInfo = &localPpPh2Config.netIfCfg.rxBufInfo;
        pRxBufInfo->allocMethod = CPSS_RX_BUFF_STATIC_ALLOC_E;

        /* There are 100 Rx buffers in HW per queue. */
        bufferSize = RX_BUFF_SIZE_DEF * 100 * 4;
#if defined(_linux) || defined(_FreeBSD)
                pRxBufInfo->buffData.staticAlloc.rxBufBlockPtr     = osCacheDmaMalloc(bufferSize);
#else
                pRxBufInfo->buffData.staticAlloc.rxBufBlockPtr     = malloc(bufferSize);
#endif
        pRxBufInfo->buffData.staticAlloc.rxBufBlockSize = bufferSize;

        localPpPh2Config.auqCfg.auDescBlock       = NULL;
        localPpPh2Config.auqCfg.auDescBlockSize   = 0;

        osMemCpy(phase2Params,&localPpPh2Config,
                                              sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

        return GT_OK;
    }
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
    GT_U8       devNum;    /* device number */
    GT_U8       portNum;   /* port number */
    GT_STATUS   retVal;


    devNum = appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devNum;


    /* This is the WA added for the Orion PRPMC CPU for DX273 DB. Gig ports are
       configured to work in wrong interface mode when using Orion CPU.
       The WA sets proper mode - SGMII and disables inband autonegatiation.
       Ports 0,1 are combo ports connected to PHY 1112 that uses inband
       autonegatiation.
       Therefore the WA doesn't change this setting for ports 0,1. */

    for(portNum = 0; portNum < 24; portNum++)
    {
        /*  Sets Interface mode to SGMII */
        retVal = cpssDxChPortInterfaceModeSet(devNum, portNum,
                                          CPSS_PORT_INTERFACE_MODE_SGMII_E);

        if(retVal != GT_OK)
        {
            return retVal;
        }

        /* in ports 0,1 bit 2 should be 1 for this phy configuration */
        if(portNum > 1)
        {
            /* Configure Auto-Negotiation in out-of-band via the device's
               Master SMI interface mode for a port */
            retVal =
                cpssDxChPortInbandAutoNegEnableSet(devNum, portNum, GT_FALSE);

            if(retVal != GT_OK)
            {
                return retVal;
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

    localPpCfgParams.maxNumOfIpv4Prefixes = 900;
    localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;

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

    /*localLibInitParams.initArp                   = GT_FALSE;*/
    /*localLibInitParams.macArpResGap              = 0;*/
    /*localLibInitParams.ctrlMacLearn              = GT_CONTROLED_MSSG;*/
    localLibInitParams.initClassifier            = GT_FALSE;
    /*localLibInitParams.initHa                    = GT_FALSE;*/
    localLibInitParams.initIpv4                  = GT_FALSE;
    localLibInitParams.initIpv6                  = GT_FALSE;
    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
    /*localLibInitParams.inlifMode                 = INLIF_MIXED_MODE;*/
    /*localLibInitParams.vidRange.portInLifMinVid  = 0;*/
    /*localLibInitParams.vidRange.portInLifMaxVid  = 500;*/
    /*localLibInitParams.numOfOutlifs              = 1000;*/
    localLibInitParams.initMpls                  = GT_FALSE;
    localLibInitParams.initMplsTunnel            = GT_FALSE;
    /*localLibInitParams.initNat                   = GT_FALSE;*/

    /*localLibInitParams.netCfg.gtBufNum           = 100;*/
    /*localLibInitParams.netCfg.gtBufSegNum        = 200;*/
    localLibInitParams.initPcl                   = GT_TRUE;
    /*localLibInitParams.maxTcamWidth              = 24;*/
    /*localLibInitParams.lastSearchSlot            = 1;*/
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
*       Board specific configurations that should be done after board
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
*
*******************************************************************************/
static GT_STATUS afterInitBoardConfig
(
    IN  GT_U8       boardRevId
)
{
    /* code from here moved to appDemoDxChFdbInit(...) */

    return GT_OK;
}


/*******************************************************************************
* gtDbDx27024G3XGBoardReg
*
* DESCRIPTION:
*       Registeration function for the DB-270-24G-3XG board.
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
GT_STATUS gtDbDx27024G3XGBoardReg
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
    boardCfgFuncs->boardGetPpPh1Params          = getPpPhase1Config;
    boardCfgFuncs->boardAfterPhase1Config       = configBoardAfterPhase1;
    boardCfgFuncs->boardGetPpPh2Params          = getPpPhase2Config;
    boardCfgFuncs->boardAfterPhase2Config       = configBoardAfterPhase2;
    boardCfgFuncs->boardGetPpLogInitParams      = getPpLogicalInitParams;
    boardCfgFuncs->boardGetLibInitParams        = getTapiLibInitParams;
    boardCfgFuncs->boardAfterInitConfig         = afterInitBoardConfig;

    return GT_OK;
}
