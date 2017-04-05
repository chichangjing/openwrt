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
* gtRdDx270-48G2XGConfig.c
*
* DESCRIPTION:
*       Initialization functions for the Cheetah DB-270-24G-3XG boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>

#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>


/*******************************************************************************
 * External variables
 ******************************************************************************/


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
#if 0  /*[SL...]*/
static GT_STATUS cascadePortConfig
(
    IN GT_U8                    devNum,
    IN GT_U32                   portNum
)
{
    GT_U8       lane;

    /* set the MRU of the cascade port to be big enough for DSA tag */
    if (coreSetPortMru(devNum, portNum, GT_MRU_1536) != GT_OK)
    {
        return GT_FAIL;
    }

    /* Set pre-emphasis to 54%.- HWCFG_MODE = 0xA  */
    for (lane = 0; lane < 4; lane++)
    {
        osTimerWkAfter(1);
        if (coreWritePort10GSmiPhyRegister(devNum, portNum, (0x8000|lane),
                                           0x803a ) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    return GT_OK;
}
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
    static  GT_U32 devCnt = 0;

    localPpPh1Config.devNum                                = devIdx;
    localPpPh1Config.baseAddr                              =
        appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
    localPpPh1Config.internalPciBase                       =
        appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
    localPpPh1Config.deviceId                              =
        ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
         (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

    /*localPpPh1Config.hwDevNum = localPpPh1Config.devNum;*/

#ifndef ASIC_SIMULATION
#if defined(_linux) || defined(_FreeBSD)
    /* unmask PP interrupt in CPLD done by the intConnect routine in kernel */
#else
    /* unmask PP interrupt in CPLD */
    *((volatile GT_U8*)0xFF010006) |= (1 << devCnt);
#endif
#endif

    /* get the Interrupt vector number from extDrv */
    extDrvGetPciIntVec(GT_PCI_INT_D, &intVecNum);

    /* Set the interrupt vector and mask numbers.                           */
    /* The following is possible becuase the int vectors are incremental.   */
    /* NOTE: The extDrv/BSP routins needs to support several int vectors !! */
    localPpPh1Config.intVecNum  = (GT_U32)((GT_UINTPTR)intVecNum) + devCnt;
    localPpPh1Config.intMask    = (GT_U32)((GT_UINTPTR)intVecNum) + devCnt;

    devCnt++;

    localPpPh1Config.uplinkCfg                             = CPSS_PP_NO_UPLINK_E;  /* ? */
    localPpPh1Config.mngInterfaceType                      = CPSS_CHANNEL_PCI_E;

    /* the CLK is 200 MHz */
    localPpPh1Config.coreClk = 200;

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
    CPSS_PP_PHASE2_INIT_PARAMS    localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS   retVal;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = CPSS_98DX270_CNS;
    localPpPh2Config.baseAddr   =
        appDemoPpConfigList[oldDevNum].pciInfo.pciBaseAddr;

    localPpPh2Config.hwDevNum = localPpPh2Config.devNum;

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
*
*             -------------------              -------------------
*             |      Dev0       |              |      Dev1       |
*             |     98DX270     |              |     98DX270     |
*             |              25 |--------------|25               |
*             |                 |    Trunk     |                 |
* XFP/SFP*****|26            24 |--------------|26            24 |*****XFP/SFP
*             |                 |              |                 |
*             |                 |              |                 |
*             |                 |              |                 |
*             | 0 1 2 ... 22 23 |              | 0 1 2 ... 22 23 |
*             -------------------              -------------------
*               * * * ... *  *                   * * * ...  * *
*               * * * ... *  *                   * * * ...  * *
*               * * * ... *  *                   * * * ...  * *
*               * * * ... *  *                   * * * ...  * *
*               * * * ... *  *                   * * * ...  * *
*
*******************************************************************************/
static GT_STATUS configBoardAfterPhase2
(
    IN  GT_U8   boardRevId
)
{
#if 0   /*[SergiyL] [SL...]  - cscd stuff not found in cpss*/
    GT_STATUS                retVal;
    GT_U8                    numberOfCascadeTrunks; /* number of trunks used for cascading */
    GT_U32                   trunkId;               /* the cascade trunkId */
    GT_U8                    portNum;
    GT_CSCD_LINK_TYPE_STC    cascadeLink;           /* holds the cascade link type(port or trunk)
                                                       and the link number leading to the target device. */
    GT_CSCD_PORT_TYPE_ENT    portType;              /* Enum of cascade port or network port */
    GT_U8                    devNum;                /* Source device number */
    GT_U8                    targetDevNum;          /* the device to be reached via cascade port for analyzing */
    GT_U16                   groupId;               /* Vlan/Vidx number that the cascade link is added to */
    GT_BOOL                  action;                /* Set or unset the port membership in the VLAN/Vidx */


    /* Cascading configurations for Cheetah Device #0 */
    /* ============================================== */
    devNum = 0;

    /* 1. Initialize the cascade library */
    numberOfCascadeTrunks = 1;
    retVal = coreCscdLibInit(devNum,numberOfCascadeTrunks,&trunkId);
    if(retVal != GT_OK)
        return retVal;


    /* 2. Set ports 24,25 to cascade ports. */
    portType = GT_CSCD_PORT_DSA_MODE_EXTEND_E;
    portNum = 24;
    retVal = coreCscdSetPortType(devNum,portNum,portType);
    if(retVal != GT_OK)
        return retVal;

    retVal = cascadePortConfig(devNum, portNum);
    if(retVal != GT_OK)
        return retVal;

    portNum = 25;
    retVal = coreCscdSetPortType(devNum,portNum,portType);
    if(retVal != GT_OK)
        return retVal;

    retVal = cascadePortConfig(devNum, portNum);
    if(retVal != GT_OK)
        return retVal;

    /* 3. Add ports 24,25 to trunk */
    cascadeLink.linkNum  = trunkId;
    cascadeLink.linkType = GT_CSCD_LINK_TYPE_TRUNK_E;
    portNum = 24;
    retVal = coreCscdAddPortToLink(devNum,cascadeLink,portNum);
    if(retVal != GT_OK)
        return retVal;
    portNum = 25;
    retVal = coreCscdAddPortToLink(devNum,cascadeLink,portNum);
    if(retVal != GT_OK)
        return retVal;

    /* 4. Configures the unicast intrastack forwarding table */
    targetDevNum = 1;
    retVal = coreCscdSetUCRouteTable(devNum,targetDevNum,cascadeLink);
    if(retVal != GT_OK)
        return retVal;

    /* 5. Configure the port VLAN/VIDX membership */
    groupId = GT_CSCD_ALL_GROUPS_CNS;  /* indicate to PSS to add the cascadeLink to each new defined VLAN/Vidx. */
    action  = GT_TRUE;
    retVal  = coreCscdSetMCRouteTable(devNum,groupId,cascadeLink,action);
    if(retVal != GT_OK)
        return retVal;

    /* 6. Set HyperG stack ports 24, 25 speed 12Gbps */
    coreCscdSetHyperGInternalPortMode(devNum, 24,
                                GT_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E);

    coreCscdSetHyperGInternalPortMode(devNum, 25,
                                GT_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E);

    /* Cascading configurations for Cheetah Device #1 */
    /* ============================================== */
    devNum = 1;

    /* 1. Initialize the cascade library */
    numberOfCascadeTrunks = 1;
    retVal = coreCscdLibInit(devNum,numberOfCascadeTrunks,&trunkId);
    if(retVal != GT_OK)
        return retVal;

    /* 2. Set ports 25,26 to cascade ports. */
    portType = GT_CSCD_PORT_DSA_MODE_EXTEND_E;
    portNum = 25;
    retVal = coreCscdSetPortType(devNum,portNum,portType);
    if(retVal != GT_OK)
        return retVal;

    retVal = cascadePortConfig(devNum, portNum);
    if(retVal != GT_OK)
        return retVal;

    portNum = 26;
    retVal = coreCscdSetPortType(devNum,portNum,portType);
    if(retVal != GT_OK)
        return retVal;

    retVal = cascadePortConfig(devNum, portNum);
    if(retVal != GT_OK)
        return retVal;

    /* 3. Add ports 25,26 to trunk */
    cascadeLink.linkNum  = trunkId;
    cascadeLink.linkType = GT_CSCD_LINK_TYPE_TRUNK_E;
    portNum = 25;
    retVal = coreCscdAddPortToLink(devNum,cascadeLink,portNum);
    if(retVal != GT_OK)
        return retVal;
    portNum = 26;
    retVal = coreCscdAddPortToLink(devNum,cascadeLink,portNum);
    if(retVal != GT_OK)
        return retVal;

    /* 4. Configures the unicast intrastack forwarding table */
    targetDevNum = 0;
    retVal = coreCscdSetUCRouteTable(devNum,targetDevNum,cascadeLink);
    if(retVal != GT_OK)
        return retVal;

    /* 5. Configure the port VLAN/VIDX membership */
    groupId = GT_CSCD_ALL_GROUPS_CNS;   /* indicate to PSS to add the cascadeLink to each new defined VLAN/Vidx. */
    action  = GT_TRUE;
    retVal  = coreCscdSetMCRouteTable(devNum,groupId,cascadeLink,action);
    if(retVal != GT_OK)
        return retVal;

    /* 6. Set HyperG stack ports 25, 26 speed 12Gbps */
    coreCscdSetHyperGInternalPortMode(devNum, 25,
                                GT_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E);

    coreCscdSetHyperGInternalPortMode(devNum, 26,
                                GT_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E);

#endif
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
    localLibInitParams.initIpv6                  = GT_FALSE;
    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
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

            rc = cpssDxChBrgFdbAAandTAToCpuSet(dev, GT_TRUE);
            if (GT_OK != rc)
            {
                return rc;
            }
            for (port = 0; (port < numOfPorts) && (GT_OK == rc); port++)
            {
                rc = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, GT_TRUE);
                if (GT_OK != rc)
                {
                    return rc;
                }
            }
        }
    }
    return rc;
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
GT_STATUS gtRdDx27048G2XGBoardReg
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


