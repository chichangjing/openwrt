/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxLion3_96XG.c
*
* DESCRIPTION:
*       Initialization functions for the Lion board.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/sysHwConfig/gtAppDemoLioncubConfig.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

/* structures definition for the use of Dx Cheetah Prestera core SW */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>

#include <cpss/generic/smi/cpssGenSmi.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>

#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>

/*******************************************************************************
 * External definitions
 ******************************************************************************/
 extern GT_STATUS qt2025PhyConfig(GT_U8 dev, GT_U32 *xsmiAddrArrayPtr);
 extern GT_STATUS appDemoLion2PortInterfaceInit(
    IN  GT_U8 dev,
    IN  GT_U8 boardRevId
);

/*******************************************************************************
 * Global variables
 ******************************************************************************/
static GT_U8    ppCounter = 0; /* Number of Packet processors. */
static GT_U32   numOfActivePortGroups = 0; /* number of port groups -- assume single device */
static GT_U32   activePortGroupsBmp = 0x000000FF;/* bitmap of active port groups
    some options to set this BMP:
    1. pre-init system :
        use  string "activePortGropsBmp" with the bmp value
    2. according to the boardRevId
    3. read from the device it's devType and know the active port groups from it.
        but the cpss was not initialized yet  so only 'de-reference' to the PP
        memory mapped on the CPU can help us.
        (for simulation can use scib functions)

    currently I'll add the string option
*/

extern GT_STATUS appDemoDebugDeviceIdSet
(
    IN GT_U8    devNum,
    IN GT_U32   deviceId
);


extern GT_STATUS appDemoDebugDeviceIdGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *deviceIdPtr
);

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
    GT_U8       i;                      /* Loop index.                  */
    GT_STATUS   pciStat;
    GT_U32      usedDevices[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS]={0};
    GT_U32      index=0,devIndex0,activeGroupsNum;
    GT_U32      value;
    GT_U32      presteraDevId;
    GT_BOOL     foundFirstDev;
    GT_U8       firstDev;

    if(boardRevId == 2)
    {
        /* running with doFdbCuckoo */
        appDemoDbEntryAdd("doFdbCuckoo", 1);
    }

    /* ability to set different active port group bitmap */
    if(appDemoDbEntryGet((GT_CHAR*)"activePortGropsBmp", &value) == GT_OK)
    {
        activePortGroupsBmp = value;
    }

    pciStat = appDemoSysGetPciInfo();
    if(pciStat != GT_OK)
    {
        return GT_NOT_FOUND;
    }

    presteraDevId =
        (((GT_U32)appDemoPpConfigList[0].pciInfo.pciDevVendorId.devId) << 16) |
        MARVELL_VENDOR_ID;

    switch(presteraDevId)
    {
        case CPSS_LION_PORT_GROUPS_01___CNS:
            activePortGroupsBmp = BIT_0 | BIT_1;
            break;
        default:
            /* 4 port groups */
            break;
    }

    foundFirstDev = GT_FALSE;
    firstDev = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            if(index == APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS)
            {
                return GT_FULL;
            }

            if( GT_FALSE == foundFirstDev )
            {
                foundFirstDev = GT_TRUE;
                firstDev = i;
            }

            if(((1<<(i-firstDev)) & activePortGroupsBmp) == 0)
            {
                appDemoPpConfigList[i].valid = GT_FALSE;
                continue;
            }

            usedDevices[index++] = i;
            numOfActivePortGroups++;
        }
    }


    /* No PCI devices found */
    if(numOfActivePortGroups == 0)
    {
        return GT_NOT_FOUND;
    }

    devIndex0 =  usedDevices[0];

    /* the lion3 device has 8 port groups */
    appDemoPpConfigList[devIndex0].numOfPortGroups = 8;

    /* set the first device to hold info about all the port groups */
    for(i = 0, index = 0, activeGroupsNum=0; i < appDemoPpConfigList[devIndex0].numOfPortGroups; i++, index++)
    {
        /*invalid all the devices , the first device will be validated after the loop */
        appDemoPpConfigList[usedDevices[index]].valid = GT_FALSE;

        if(((1<<i) & activePortGroupsBmp) == 0)
        {
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupActive = GT_FALSE;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo.internalPciBase = CPSS_PARAM_NOT_USED_CNS;
            continue;
        }

        appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupActive = GT_TRUE;

        appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo =
            appDemoPpConfigList[usedDevices[index]].pciInfo;

        activeGroupsNum++;
    }

    /* validate the first device */
    appDemoPpConfigList[devIndex0].valid = GT_TRUE;

    appDemoPpConfigList[devIndex0].pciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[devIndex0].pciInfo.internalPciBase =  CPSS_PARAM_NOT_USED_CNS;


    if(activeGroupsNum != numOfActivePortGroups)
    {
        /* the bmp of active port groups filtered one of the existing port groups */
        return GT_BAD_STATE;
    }

    ppCounter = 1;

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = GT_FALSE;/* no B2B with Lion devices */

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
    OUT CPSS_PP_PHASE1_INIT_PARAMS  *phase1Params
)
{
    GT_STATUS rc = GT_OK;
    CPSS_PP_PHASE1_INIT_PARAMS    localPpPh1Config = CPSS_PP_PHASE1_DEFAULT;
    void    *intVecNum;
    GT_U32  ii;

    boardRevId = boardRevId;

    localPpPh1Config.devNum = devIdx;

    localPpPh1Config.baseAddr =
                            appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
    localPpPh1Config.internalPciBase =
                        appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
    localPpPh1Config.deviceId =
        ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
         (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

    localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PEX_E;

    localPpPh1Config.numOfPortGroups = appDemoPpConfigList[devIdx].numOfPortGroups;
    for(ii = 0 ; ii < appDemoPpConfigList[devIdx].numOfPortGroups ;ii++)
    {
        localPpPh1Config.portGroupsInfo[ii].busBaseAddr =
            appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.pciBaseAddr;
        localPpPh1Config.portGroupsInfo[ii].internalPciBase =
            appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.internalPciBase;

        if(appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.pciBaseAddr ==
           CPSS_PARAM_NOT_USED_CNS)
        {
            continue;
        }

        extDrvGetPciIntVec(8 - ii , &intVecNum);
        localPpPh1Config.portGroupsInfo[ii].intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
        extDrvGetIntMask(8 - ii , &localPpPh1Config.portGroupsInfo[ii].intMask);
    }

    /* retrieve PP Core Clock from HW */
    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

    /* current Lion box has 156.25MHz differencial serdesRefClock */
    localPpPh1Config.serdesRefClock = APPDEMO_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E;

    localPpPh1Config.uplinkCfg = CPSS_PP_NO_UPLINK_E;

    appDemoPpConfigList[devIdx].flowControlDisable = GT_TRUE;

    osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));

    /* check previouse settings of debug device ID */
    rc = appDemoDebugDeviceIdGet(devIdx, &ii);
    if (rc == GT_NO_SUCH)
    {
        /* Due to binding of previous generic device Id to the GPON family, */
        /* imitate previous generic Id when new one is observed.            */
        if( CPSS_98CX8248_CNS == localPpPh1Config.deviceId)
        {
            rc = appDemoDebugDeviceIdSet(devIdx, CPSS_98CX8248_1_CNS);
        }
        else if( CPSS_98CX8224_CNS == localPpPh1Config.deviceId)
        {
            rc = appDemoDebugDeviceIdSet(devIdx, CPSS_98CX8224_1_CNS);
        }
        else
            rc = GT_OK;
    }

    return rc;
}


/*******************************************************************************
* waInit
*
* DESCRIPTION:
*       init the WA needed after phase1.
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
static GT_STATUS waInit
(
    IN  GT_U8   boardRevId
)
{
    GT_STATUS   rc;
    CPSS_DXCH_IMPLEMENT_WA_ENT   waFromCpss[CPSS_DXCH_IMPLEMENT_WA_LAST_E];
    GT_U32  waIndex=0;
    GT_U32  ii;
    GT_U8   devNum;
    GT_U32  enableAddrFilterWa; /* do we want to enable Restricted Address
                                   Filtering or not */

    boardRevId = boardRevId;


    if(GT_OK != appDemoDbEntryGet("RESTRICTED_ADDRESS_FILTERING_WA_E", &enableAddrFilterWa))
    {
        enableAddrFilterWa = 0;
    }


    for(ii = SYSTEM_DEV_NUM_MAC(0); ii < SYSTEM_DEV_NUM_MAC(ppCounter); ++ii)
    {
        waIndex = 0;
        devNum = appDemoPpConfigList[ii].devNum;

        if (enableAddrFilterWa == 1)
        {
            /* state that application want CPSS to implement the Restricted
               Address Filtering WA */
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E;
        }

        if(waIndex)
        {
            rc = cpssDxChHwPpImplementWaInit(devNum,waIndex,&waFromCpss[0], NULL);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpImplementWaInit", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

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
    GT_STATUS rc;

    rc = waInit(boardRevId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("waInit", rc);

    return rc;
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
    GT_STATUS       rc;
    CPSS_PP_PHASE2_INIT_PARAMS  localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS                   retVal;
    GT_U32                      auDescNum;
    GT_U32                      fuDescNum;
    GT_U32                      tmpData;

    boardRevId = boardRevId;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = appDemoPpConfigList[oldDevNum].deviceId;

    localPpPh2Config.fuqUseSeparate = GT_TRUE;

    /* single AUQ , since single FDB unit */
    auDescNum = AU_DESC_NUM_DEF;
    /*FUQ per port group to support the CNC per port group */
    fuDescNum = AU_DESC_NUM_DEF*numOfActivePortGroups;

    if(appDemoDbEntryGet("fuDescNum", &tmpData) == GT_OK)
    {
        /* someone already stated the number of fuDescNum , so we NOT override it ! */
    }
    else
    {
        /* add the fuDescNum to the DB , to allow appDemoAllocateDmaMem(...) to use it ! */
        rc = appDemoDbEntryAdd("fuDescNum", fuDescNum);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Check if to use the default configuration for appDemo of */
    /* Tx SDMA Packet Generator queues.  */
    if( appDemoDbEntryGet("skipTxSdmaGenDefaultCfg", &tmpData) == GT_NO_SUCH )
    {
        /* allow to work in Tx queue generator mode */
        rc = appDemoDbEntryAdd("useMultiNetIfSdma", GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Enable Tx queue 3 to work in Tx queue generator mode */
        rc = appDemoDbEntryAdd("txGenQueue_3", GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Enable Tx queue 6 to work in Tx queue generator mode */
        rc = appDemoDbEntryAdd("txGenQueue_6", GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF,RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,
                                   auDescNum,
                                   &localPpPh2Config);

    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

    phase2Params->auMessageLength= CPSS_AU_MESSAGE_LENGTH_8_WORDS_E;

    return retVal;
}

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
static GT_STATUS internalXGPortConfig
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  port_num
)
{

    devNum = devNum;
    port_num = port_num;

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
    IN GT_PHYSICAL_PORT_NUM     portNum
)
{
    GT_STATUS       rc;

    /* set the MRU of the cascade port to be big enough for DSA tag */
    rc = cpssDxChPortMruSet(devNum, portNum, 1536);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortMruSet", rc);
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
    GT_U32  dev;
    GT_PHYSICAL_PORT_NUM  port; /* port number */
    GT_U8  index;
    GT_STATUS   rc;

    boardRevId = boardRevId;

    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        index = 0;
        appDemoPpConfigList[dev].internal10GPortConfigFuncPtr = internalXGPortConfig;
        appDemoPpConfigList[dev].internalCscdPortConfigFuncPtr = cascadePortConfig;

        for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS;port++)
        {
            if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(dev)->existingPorts, port)))
            {
                continue;
            }

            appDemoPpConfigList[dev].ports10GToConfigureArr[index] = port;
            index++;

            /* split ports between MC FIFOs for Multicast arbiter */
            rc = cpssDxChPortTxMcFifoSet((GT_U8)dev, port, port%2);
            if( GT_OK != rc )
            {
                return rc;
            }
        }

        appDemoPpConfigList[dev].numberOf10GPortsToConfigure = index;

        /* lion48Init((GT_U8)dev); */
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

    boardRevId = boardRevId;
    devNum = devNum;

    localPpCfgParams.maxNumOfIpv4Prefixes = 3920;
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;
    }
    else
    {
        localPpCfgParams.numOfTrunks = _2K-1;
    }

    localPpCfgParams.maxNumOfVirtualRouters = 1;
    localPpCfgParams.lpmDbFirstTcamLine = 100;
    localPpCfgParams.lpmDbLastTcamLine = 1204;
    localPpCfgParams.maxNumOfPbrEntries = _8K;

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

    boardRevId = boardRevId;
    devNum = devNum;

    localLibInitParams.initClassifier            = GT_FALSE;
    localLibInitParams.initIpv6                  = GT_TRUE;
    localLibInitParams.initIpv4                  = GT_TRUE;

    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
    localLibInitParams.initMpls                  = GT_FALSE;
    localLibInitParams.initMplsTunnel            = GT_FALSE;
    localLibInitParams.initPcl                   = GT_TRUE;
    localLibInitParams.initTcam                  = GT_TRUE;

    /* there is no Policer lib init for CH3 devices */
    localLibInitParams.initPolicer               = GT_FALSE;

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
    GT_STATUS   rc;
    GT_U8       dev;

    boardRevId = boardRevId;

    /* allow processing of AA messages */
    appDemoSysConfig.supportAaMessage = GT_TRUE;

    if(appDemoSysConfig.forceAutoLearn == GT_FALSE)
    {
        /* the Lion need 'Controlled aging' because the port groups can't share refresh info */
        /* the AA to CPU enabled from appDemoDxChFdbInit(...) --> call cpssDxChBrgFdbAAandTAToCpuSet(...)*/
        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
        {
            rc = cpssDxChBrgFdbActionModeSet(dev,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* note that the appDemo sets the aging mode to 'Triggered mode' by default
       so no auto aging unless you set it explicitly */
CPSS_TBD_BOOKMARK_EARCH /*calling this cause port 0 to be 'portEn = 0' */
#if 0
    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        appDemoLion2PortInterfaceInit(dev, boardRevId);
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* gtDbDxLion396XgBoardReg
*
* DESCRIPTION:
*       Registration function for the Lion3 96XG ports board .
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
GT_STATUS gtDbDxLion396XgBoardReg
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    boardRevId = boardRevId;

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

