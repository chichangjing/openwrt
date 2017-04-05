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
* gtDbFx950_DX3_Config.c
*
* DESCRIPTION:
*       Initialization functions for the FX950 and DxCh3 boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
/* cascading support */
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>

/* sructures definition for the use of Dx Cheetah Prestera core SW */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>

/* Dune includes */
#include <sweep/Fap20m/include/sweep_fap20m_param.h>
#include <sweep/Fap20m/include/sweep_fap20m_system_consts.h>
#include <sweep/Fap20m/include/sweep_fap20m_app.h>
#include <sweep/Fap20m/include/sweep_fap20m_gfa.h>
#include <Pub/include/dune_chips.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/
extern APP_DEMO_PP_CONFIG    appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* This will hold the unit # of the devices on the board */
/* Assume num of PP, num of FA and num of XBAR are the same */
/*extern GT_U8 unitDevList[PRV_CPSS_MAX_PP_DEVICES_CNS];*/
/* Init the SMI interface */
extern GT_STATUS hwIfSmiInitDriver
(
    GT_VOID
);
/* memory allocation */
extern void *malloc(size_t);

static unsigned __TASKCONV netPortsForceLinkUp
(
    GT_VOID * unused
);


static GT_U8    ppCounter = 0; /* Number of Packet processors. */

/* Data base holds ports link status for all devices */
static GT_U32 portLinkStatusBmp[PRV_CPSS_MAX_PP_DEVICES_CNS];

#define DEV_PORT_LINK_STATUS_SET(dev, port, status) \
    portLinkStatusBmp[dev] &= ~(1 << (port)); \
    portLinkStatusBmp[dev] |= ((status) << (port));

#define DEV_PORT_LINK_STATUS_GET(dev, port) \
    ((portLinkStatusBmp[dev] >> (port)) & 0x1)


extern unsigned long       PEX_FAP_base_addr;   /* base address of FAP on PEX */

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
    GT_U8       i;                      /* Loop index.                  */
    GT_STATUS   pciStat;

    pciStat = appDemoSysGetPciInfo();
    if(pciStat == GT_OK)
    {
        for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            if(appDemoPpConfigList[i].valid == GT_TRUE)
            {
                ppCounter++;
            }
        }
    }
    else
        ppCounter = 0;


    /* No PCI devices found */
    if(ppCounter == 0)
    {
        appDemoSysConfig.cpuEtherPortUsed = GT_TRUE;
        appDemoSysConfig.cpuEtherInfo.devNum = NOT_INIT_CPU_ETHER_DEV;

        /* Scan for SMI device info  */
        retVal = appDemoSysGetSmiInfo();
        if(retVal != GT_OK)
        {
            return retVal;
        }
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
    }

    if(ppCounter == 0)
    {
        return GT_NOT_FOUND;
    }

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = ((ppCounter == 2) ? GT_TRUE : GT_FALSE);

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

    localPpPh1Config.devNum = devIdx;

    if (appDemoPpConfigList[devIdx].channel == CPSS_CHANNEL_PCI_E)
    {
        localPpPh1Config.baseAddr =
                                appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
        localPpPh1Config.internalPciBase =
                            appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
        localPpPh1Config.deviceId =
            ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
             (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

        localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PEX_E;
    }
    else /* CPSS_CHANNEL_SMI_E */
    {
        localPpPh1Config.baseAddr         = appDemoPpConfigList[devIdx].smiInfo.smiIdSel;
        localPpPh1Config.internalPciBase  = 0x0;
        localPpPh1Config.deviceId         =
           ((appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.devId << 16) |
            (appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.vendorId));
        localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_SMI_E;
    }

    if(2 == ppCounter)
    {/* RD-DX285-48G-4XG */
        extDrvGetPciIntVec((devIdx == 0) ? GT_PCI_INT_A : GT_PCI_INT_B,
                            &intVecNum);
        localPpPh1Config.intVecNum = (GT_U32)intVecNum;
        extDrvGetIntMask((devIdx == 0) ? GT_PCI_INT_A : GT_PCI_INT_B,
                            &(localPpPh1Config.intMask));
    }
    else
    {
        extDrvGetPciIntVec(GT_PCI_INT_D,&intVecNum);
        localPpPh1Config.intVecNum = (GT_U32)intVecNum;
        extDrvGetIntMask(GT_PCI_INT_D,&(localPpPh1Config.intMask));
    }

    /* the CLK is 270 MHz */
    localPpPh1Config.coreClk = 270;

    localPpPh1Config.uplinkCfg = CPSS_PP_NO_UPLINK_E;

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
    GT_U8    i;                      /* Loop index.                  */

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            cpssDxChBrgFdbAAandTAToCpuSet(appDemoPpConfigList[i].devNum,GT_TRUE);
        }
    }
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
    CPSS_PP_PHASE2_INIT_PARAMS  localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS                   retVal;

    localPpPh2Config.devNum     = oldDevNum;
    if(appDemoPpConfigList[oldDevNum].channel == CPSS_CHANNEL_SMI_E)
    {
        localPpPh2Config.baseAddr   =
            appDemoPpConfigList[oldDevNum].smiInfo.smiIdSel;
    }

    localPpPh2Config.deviceId   = appDemoPpConfigList[oldDevNum].deviceId;

    localPpPh2Config.fuqUseSeparate = GT_TRUE;

    retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF,RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,AU_DESC_NUM_DEF,
                                   &localPpPh2Config);

    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

    return retVal;
}

/*
    functions :
        cpuMemSpacePrint ,
        cpuMemSpaceByteWrite ,
        cpuMemSpaceWrite

    moved to file:
        <appDemo\sysHwConfig\gtAppDemoPciConfig.c>
*/

/*******************************************************************************
* internalXGPortConfig
*
* DESCRIPTION:
*       Internal function performs all needed configurations of 10G ports.
*
* INPUTS:
*       devNum      - Device Id.
*       portNum     - Port Number.
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
static GT_STATUS prvXGPortConfig
(
    IN  GT_U8     devNum,
    IN  GT_U8     port_num
)
{
    return GT_OK;
}

/*******************************************************************************
* cascadePortConfig
*
* DESCRIPTION:
*       Additional configuration for cascading ports
*
* INPUTS:
*       devNum  - Device number.
*       portNum - Cascade port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success,
*       GT_HW_ERROR - HW failure
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cascadePortConfig
(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum
)
{
    GT_STATUS       rc;

    /* set the MRU of the cascade port to be big enough for DSA tag */
    rc = cpssDxChPortMruSet(devNum, portNum, 1536);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortMruSet", rc);
    if (GT_OK != rc)
    {
        return GT_FAIL;
    }

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
    GT_U32  i;
    GT_STATUS               retVal;
    CPSS_DXCH_LED_CLASS_CONF_STC   ledClassConf[LED_MAX_NUM_OF_CLASS]; /* Leds registers class data */
    CPSS_DXCH_LED_GROUP_CONF_STC   ledGroupConf[LED_MAX_NUM_OF_GROUP]; /* Leds registers group data */
    GT_U32                         confRregVal = 0x337bff00;  /* Leds control registers data */


    if (ppCounter > 2)
    {
        return GT_FAIL;

    }

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

    osMemSet(ledClassConf, 0, sizeof(ledClassConf));

    /* FD */
    ledClassConf[0].classNum  = 2;
    ledClassConf[0].classConf = 0x0;

    /* Lnk */
    ledClassConf[1].classNum  = 3;
    ledClassConf[1].classConf = 0x0;

    /* ~Act & Blink */
    ledClassConf[2].classNum  = 4;
    ledClassConf[2].classConf = 0xE000;

    /* HD */
    ledClassConf[3].classNum  = 5;
    ledClassConf[3].classConf = 0x0;

    /* PD(0) */
    ledClassConf[4].classNum  = 6;
    ledClassConf[4].classConf = 0x11000;


    for(i = SYSTEM_DEV_NUM_MAC(0); i < SYSTEM_DEV_NUM_MAC(ppCounter); ++i)
    {
        appDemoPpConfigList[i].internal10GPortConfigFuncPtr = prvXGPortConfig;
        appDemoPpConfigList[i].internalCscdPortConfigFuncPtr = cascadePortConfig;

        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            /* LED stream configuration */
            retVal = cpssDxChLedStreamClassAndGroupConfig(appDemoPpConfigList[i].devNum, confRregVal,
                                                  ledClassConf, 5,
                                                  ledGroupConf, LED_MAX_NUM_OF_GROUP);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChLedStreamClassAndGroupConfig", retVal);
            if (retVal != GT_OK)
            {
                return retVal;
            }
        }

    }

    if (ppCounter > 1)
    {/* ppCounter == 2 */

        for(i = SYSTEM_DEV_NUM_MAC(0); i < SYSTEM_DEV_NUM_MAC(ppCounter); ++i)
        {
            /* DxCh3 with 24 ports plus 4 Stack ports */
            appDemoPpConfigList[i].numberOfCscdTrunks = 1;
            appDemoPpConfigList[i].numberOfCscdPorts = 4;
            /* Port 24 */
            appDemoPpConfigList[i].cscdPortsArr[0].portNum = 24;
            appDemoPpConfigList[i].cscdPortsArr[0].cscdPortType =
                                        CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[i].cscdPortsArr[0].trunkId =
                                                    APP_DEMO_127_TRUNKS_CNS;
            /* Port 25 */
            appDemoPpConfigList[i].cscdPortsArr[1].portNum = 25;
            appDemoPpConfigList[i].cscdPortsArr[1].cscdPortType =
                                        CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[i].cscdPortsArr[1].trunkId =
                                                    APP_DEMO_127_TRUNKS_CNS;
            /* Port 26*/
            appDemoPpConfigList[i].cscdPortsArr[2].portNum = 26;
            appDemoPpConfigList[i].cscdPortsArr[2].cscdPortType =
                                        CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[i].cscdPortsArr[2].trunkId =
                                                    APP_DEMO_127_TRUNKS_CNS;
            /* Port 27 */
            appDemoPpConfigList[i].cscdPortsArr[3].portNum = 27;
            appDemoPpConfigList[i].cscdPortsArr[3].cscdPortType =
                                        CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[i].cscdPortsArr[3].trunkId =
                                                    APP_DEMO_127_TRUNKS_CNS;

            /* CPSS_PORTS_BMP_PORT_SET_MAC
            (&appDemoPpConfigList[i].cscdPortsBmp, cscdPortNum); */

            appDemoPpConfigList[i].numberOfCscdTargetDevs = 1;
            appDemoPpConfigList[i].cscdTargetDevsArr[0].targetDevNum =
                                                        (0 == i) ? 1 : 0;
            appDemoPpConfigList[i].cscdTargetDevsArr[0].
                    linkToTargetDev.linkNum = APP_DEMO_127_TRUNKS_CNS;
            appDemoPpConfigList[i].cscdTargetDevsArr[0].
                    linkToTargetDev.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
        }
    }
    else
    {
        /* Standalone Cheetah 3 device */
        /* DxCh3 with 24 ports plus 4 XG ports */
        if(appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5128_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId ==CPSS_98DX287_CNS )
        {
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].numberOf10GPortsToConfigure = 4;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[0] = 24;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[1] = 25;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[2] = 26;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[3] = 27;
        }
        else if((appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX8110_CNS) ||
               (appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX8111_CNS))
        {
            /* DxCh3 with 10 XG ports */
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].numberOf10GPortsToConfigure = 10;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[0] = 0;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[1] = 4;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[2] = 10;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[3] = 12;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[4] = 16;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[5] = 22;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[6] = 24;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[7] = 25;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[8] = 26;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[9] = 27;
        }
        else
        {
            return GT_FAIL;
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

    localPpCfgParams.maxNumOfIpv4Prefixes = 3920;
    localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;
    /* Ch2 without Vr support case */
    localPpCfgParams.maxNumOfVirtualRouters = 1;
    localPpCfgParams.lpmDbFirstTcamLine = 100;
    localPpCfgParams.lpmDbLastTcamLine = 1204;


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

    localLibInitParams.initClassifier            = GT_FALSE;
    localLibInitParams.initIpv6                  = GT_TRUE;

    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
    localLibInitParams.initMpls                  = GT_FALSE;
    localLibInitParams.initMplsTunnel            = GT_FALSE;
    localLibInitParams.initPcl                   = GT_TRUE;

    /* there is no Policer lib init for CH3 devices */
    localLibInitParams.initPolicer               = GT_FALSE;


    osMemCpy(libInitParams,&localLibInitParams,
             sizeof(APP_DEMO_LIB_INIT_PARAMS));
    return GT_OK;
}


/*******************************************************************************
* afterInitBoardConfigPp
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
static GT_STATUS afterInitBoardConfigPp
(
    IN  GT_U8       boardRevId
)
{
    GT_U32  i;
    GT_U8   devNum;                         /* Device number */
    GT_U32  boardRevId_U32 = boardRevId;    /* Board revision ID in U32 format. Only to prevent casting warnings */
#ifndef ASIC_SIMULATION
    GT_STATUS rc;                           /* return code */
    GT_U32  portNum;                        /* port number */
#endif

#ifndef ASIC_SIMULATION

    for(i = SYSTEM_DEV_NUM_MAC(0); i < SYSTEM_DEV_NUM_MAC(ppCounter); i++)
    {
        /* Do nothing with devices other than CPSS_98DX5128_CNS and CPSS_98DX287_CNS */
        if( appDemoPpConfigList[i].deviceId != CPSS_98DX5128_CNS &&
            appDemoPpConfigList[i].deviceId != CPSS_98DX287_CNS )
        {
            continue;
        }

        devNum = appDemoPpConfigList[i].devNum;

        for( portNum = 0 ; portNum < 24 ; portNum++ )
        {
            /* select page 2 */
            rc = cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, 22, 2);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* disable SGMII auto neg*/
            rc = cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, 21, 0x0040);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* AMP=0x6/AVDDT - Internal*/
            rc = cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, 26, 0x0006);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* return to page 2 */
            rc = cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, 22, 0);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* Apply PHY soft reset*/
            rc = cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, 0, 0x9140);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* select page 3 */
            rc = cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, 22, 3);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* Change LED polarity */
            rc = cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, 17, 0x4405);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
        }

        osTimerWkAfter(2000);

        /* Enable autoNeg Port <-> PHY */
        rc = cpssDxChPhyAutonegSmiSet(devNum, 0, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyAutonegSmiSet", rc);
        if( rc != GT_OK )
            return rc;

        rc = cpssDxChPhyAutonegSmiSet(devNum, 12, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyAutonegSmiSet", rc);
        if( rc != GT_OK )
            return rc;

    }

#else
    return GT_OK;
#endif
}

/*******************************************************************************
* afterInitBoardConfigFx950
*
* DESCRIPTION:
*       FX950 specific configuration
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
static GT_STATUS afterInitBoardConfigFx950
(
    IN  GT_U8       boardRevId
)
{
    FAP20M_SWEEP_PARAMS  *sweep_params=NULL; /* sweep parameters */

    unsigned int        max_nof_faps_in_system; /* maximum number of FAPs in system */
    unsigned int        faps_id_in_2_steps;     /* FAP ID step is 2*/
    unsigned int        nof_fabric_cards;       /* number of FE cards */
    unsigned int        device_id;              /* device ID of FAP in SW */
    unsigned int        fap_id;                 /* FAP id in HW */

    int                 ret; /* return code */


    /* Set parameters according to Negev system */
    max_nof_faps_in_system = FAP20M_SWP_NEGEV_NOF_FAPS * 2;
    faps_id_in_2_steps     = TRUE;
    nof_fabric_cards       = 1;

    /* temporary solution for ASIC_SIMULATION only.
      PEX_FAP_base_addr is used in chip_sim_read */
    PEX_FAP_base_addr = 0x10;

    /* assign FAP ID to be equal PP Device Number */
    fap_id = appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devNum;

    /* allocate SWEEP parameters */
    ret = 0;
    sweep_params = cpssOsMalloc(sizeof(FAP20M_SWEEP_PARAMS));
    if (NULL == sweep_params)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    sweep_clear_FAP20M_SWEEP_PARAMS(sweep_params);

    /* set default parameters */
    if( sweep_load_fap20m_SWEEP_PARAMS(sweep_params) )
    {
      ret = -2;
      goto exit;
    }

    /* here it's possible to update default parameters of the FX950 */


    /* init SAND and FX950 */
    ret = sweep_fap20m_app(
            &device_id,
            sweep_params,
            faps_id_in_2_steps,
            max_nof_faps_in_system,
            2*nof_fabric_cards, /*Number of FEs*/
            fap_id,
            PEX_FAP_base_addr,
            FALSE,
            FALSE
          );

exit:

  if (NULL != sweep_params)
  {
    cpssOsFree(sweep_params);
  }
  return ret;
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
    GT_STATUS rc;                           /* return code */

    /* Packet Processor board specific configurations */
    rc = afterInitBoardConfigPp(boardRevId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("afterInitBoardConfigPp", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Ocelot configuration */
    rc = afterInitBoardConfigFx950(boardRevId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("afterInitBoardConfigFx950", rc);
    if(rc != GT_OK)
    {
        return rc;
    }


    return GT_OK;

}


/*******************************************************************************
* gtDbFx950_DX3BoardReg
*
* DESCRIPTION:
*       Registeration function for the DB-FX950-DX3-GP board (Cheetah 3 devices).
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
GT_STATUS gtDbFx950_DX3BoardReg
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
    /*boardCfgFuncs->boardGetUserPorts            = getUserPorts;*/

    return GT_OK;
}

