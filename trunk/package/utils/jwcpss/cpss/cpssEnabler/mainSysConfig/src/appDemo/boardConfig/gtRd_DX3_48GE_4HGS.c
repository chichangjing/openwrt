/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* RD-DX3-48GE-4HGS.c
*
* DESCRIPTION:
*       Initialization functions for the Cheetah 3 DB-DX3-GP boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
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
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>

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

#ifndef ASIC_SIMULATION
static unsigned __TASKCONV netPortsForceLinkUp
(
    GT_VOID * unused
);
/* Data base holds ports link status for all devices */
static GT_U32 portLinkStatusBmp[PRV_CPSS_MAX_PP_DEVICES_CNS];

#define DEV_PORT_LINK_STATUS_SET(dev, port, status) \
    portLinkStatusBmp[dev] &= ~(1 << (port)); \
    portLinkStatusBmp[dev] |= ((status) << (port));

#define DEV_PORT_LINK_STATUS_GET(dev, port) \
    ((portLinkStatusBmp[dev] >> (port)) & 0x1)

#endif /*ASIC_SIMULATION*/


static GT_U8    ppCounter = 0; /* Number of Packet processors. */

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


#if (!(defined _linux) && !(defined ASIC_SIMULATION))
    #define IGNORE_BSP_VEC_NUM
#endif

#ifdef IGNORE_BSP_VEC_NUM
static GT_U32  int_a=36;
static GT_U32  int_b=33;
#endif /*IGNORE_BSP_VEC_NUM*/

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


#ifdef IGNORE_BSP_VEC_NUM
    /* WA to bad values from the bsp */
    if(devIdx == 0)
    {
        intVecNum = (void*)int_a; /*36*/
    }
    else
    {
        intVecNum = (void*)int_b; /*33*/
    }
#endif /*IGNORE_BSP_VEC_NUM*/

        localPpPh1Config.intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
        extDrvGetIntMask((devIdx == 0) ? GT_PCI_INT_A : GT_PCI_INT_B,
                            &(localPpPh1Config.intMask));

#ifdef IGNORE_BSP_VEC_NUM
            localPpPh1Config.intMask = (GT_U32)((GT_UINTPTR)intVecNum);
#endif /*IGNORE_BSP_VEC_NUM*/
    }
    else
    {
        extDrvGetPciIntVec(GT_PCI_INT_D,&intVecNum);
        localPpPh1Config.intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
        extDrvGetIntMask(GT_PCI_INT_D,&(localPpPh1Config.intMask));
    }

    /* retrieve PP Core Clock from HW */
    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

    localPpPh1Config.uplinkCfg = CPSS_PP_NO_UPLINK_E;

    localPpPh1Config.powerDownPortsBmp.ports[0] = appDemoDxChPowerDownBmp;

    osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));

    /* Disable Flow Control for this revision */
    if(boardRevId == 3)
    {
        appDemoPpConfigList[devIdx].flowControlDisable = GT_TRUE;
    }
    
    return GT_OK;
}


/*******************************************************************************
* b2bXgsPortsSerdesConfig
*
* DESCRIPTION:
*       This function configures SERDES swapping for XGS ports in back-to-back systems.
*
* INPUTS:
*       dev  - device number.
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
static GT_STATUS b2bXgsPortsSerdesConfig
(
    IN  GT_U8   dev
)
{
    GT_U8  port;
    GT_U32  lane;
    GT_U32  rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_U32  txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_STATUS rc;

    for( lane = 0 ; lane < CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ; lane++ )
    {
        rxSerdesLaneArr[lane] = lane;
        txSerdesLaneArr[lane] = CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS - lane - 1;
    }

    for( port = 24 ; port < 28 ; port++ )
    {
        rc = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortXgPscLanesSwapSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
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
    GT_U32  additionalInfoBmpArr[CPSS_DXCH_IMPLEMENT_WA_LAST_E];
    GT_U32  waIndex;
    GT_U32  ii;
    GT_U8   devNum;
    
    for(ii = SYSTEM_DEV_NUM_MAC(0); ii < SYSTEM_DEV_NUM_MAC(ppCounter); ++ii)
    {
        waIndex = 0;
        devNum = appDemoPpConfigList[ii].devNum;

        waFromCpss[waIndex] = CPSS_DXCH_IMPLEMENT_WA_IP_MC_UNDER_STRESS_E;
            additionalInfoBmpArr[waIndex++] = 0;
        
        if(waIndex)
        {
            rc = cpssDxChHwPpImplementWaInit(devNum,waIndex,&waFromCpss[0],
                                             &additionalInfoBmpArr[0]);
            if( GT_NOT_APPLICABLE_DEVICE == rc )
            {
                rc = GT_OK;
            }
            
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
    GT_U8    i;                      /* Loop index.                  */
    GT_STATUS rc;

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            rc = cpssDxChBrgFdbAAandTAToCpuSet(appDemoPpConfigList[i].devNum,GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuSet", rc);
            if(rc != GT_OK)
                return rc;

            if (ppCounter > 1)
            {
                /* Swap SERDES for XGS ports in back-to-back sysytem */
                rc = b2bXgsPortsSerdesConfig(i);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("b2bXgsPortsSerdesConfig", rc);
                if(rc != GT_OK)
                    return rc;
            }
        }
    }

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
static GT_STATUS internalXGPortConfig
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum
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
    IN GT_PHYSICAL_PORT_NUM     portNum
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
    GT_U32                         confRregVal = 0x337bff00;           /* Leds control registers data */
    GT_U8                          portNum;                            /* port number */

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
        appDemoPpConfigList[i].internal10GPortConfigFuncPtr = internalXGPortConfig;
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
            appDemoPpConfigList[i].numberOf10GPortsToConfigure = 0;

            /* Cascading */
            portNum = (i == 1) ? 24 : 26;

            appDemoPpConfigList[i].numberOfCscdTrunks = 1;
            appDemoPpConfigList[i].numberOfCscdPorts = 2;

            appDemoPpConfigList[i].cscdPortsArr[0].portNum = portNum;
            appDemoPpConfigList[i].cscdPortsArr[0].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[i].cscdPortsArr[0].trunkId = APP_DEMO_127_TRUNKS_CNS;

            appDemoPpConfigList[i].cscdPortsArr[1].portNum = portNum + 1;
            appDemoPpConfigList[i].cscdPortsArr[1].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[i].cscdPortsArr[1].trunkId = APP_DEMO_127_TRUNKS_CNS;

            appDemoPpConfigList[i].numberOfCscdTargetDevs = 1;

            appDemoPpConfigList[i].cscdTargetDevsArr[0].targetDevNum = (0 == i) ? 1 : 0;
            appDemoPpConfigList[i].cscdTargetDevsArr[0].linkToTargetDev.linkNum = APP_DEMO_127_TRUNKS_CNS;
            appDemoPpConfigList[i].cscdTargetDevsArr[0].linkToTargetDev.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
        }
    }
    else
    {
        /* DxCh3 with 4/8/16/24 Network ports 2 XG ports */
        if(appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5126_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5127_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5151_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5151_1_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5152_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5152_1_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5154_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5154_1_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5156_CNS ||
           appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX5156_1_CNS)
        {
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].numberOf10GPortsToConfigure = 2;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[0] = 24;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[1] = 25;
        }
        /* DxCh3 with 10 XG ports */
        else if(appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX8110_CNS ||
                appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX8110_1_CNS ||
                appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX8111_CNS)
        {
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
        /* DxCh3 with 8 XG ports */
        else if(appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX8108_CNS ||
                appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId == CPSS_98DX8109_CNS)
        {
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].numberOf10GPortsToConfigure = 8;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[0] = 0;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[1] = 4;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[2] = 10;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[3] = 12;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[6] = 24;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[7] = 25;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[8] = 26;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[9] = 27;
        }
        /* Standalone Cheetah 3 device */
        /* DxCh3 with 24 ports plus 4 XG ports */
        else if(appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].deviceId != CPSS_98DX5124_CNS)
        {
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].numberOf10GPortsToConfigure = 4;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[0] = 24;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[1] = 25;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[2] = 26;
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[3] = 27;
        }
        else
        {
            appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].numberOf10GPortsToConfigure = 0;
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
#ifndef ASIC_SIMULATION
    GT_UINTPTR  boardRevId_UINT = boardRevId;    /* Board revision ID in U32 format. Only to prevent casting warnings */
    GT_U32  i;
    GT_U8   devNum;                         /* Device number */
    GT_STATUS rc;                           /* return code */
    GT_U32  portNum;                        /* port number */
    GT_TASK netPortsForceLinkUpTid;         /* Task Id */


    for(i = SYSTEM_DEV_NUM_MAC(0); i < SYSTEM_DEV_NUM_MAC(ppCounter); i++)
    {
        /* Do nothing with XG ports devices */
        switch(appDemoPpConfigList[i].deviceId)
        {
            case PRV_CPSS_CHEETAH3_XG_PORTS_DEVICES_CASES_MAC:
                continue;/* skip the device */
            default:
                break;
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

        /* set port speed to 2.5G for 2.5G ports */
        if(appDemoPpConfigList[i].deviceId == CPSS_98DX5151_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5151_1_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5152_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5152_1_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5154_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5154_1_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5156_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5156_1_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5157_CNS ||
           appDemoPpConfigList[i].deviceId == CPSS_98DX5157_1_CNS)
        {
            /* Serdes power down
               should be done per SERDES group that is 4 adjacent ports.*/
            for( portNum = 0 ; portNum < 24 ; portNum += 4 )
            {
                if( !PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) )
                    continue;
                /* Sets power state of SERDES port lanes per group */ 
                rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum,
                                                           portNum / 4,
                                                           GT_FALSE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortGroupSerdesPowerStatusSet", rc);
                if( rc != GT_OK )
                    return rc;
            }

            /* set port speed to 2.5G */
            for( portNum = 0 ; portNum < 24 ; portNum++ )
            {
                if( !PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) )
                    continue;
                rc = cpssDxChPortSpeedSet(devNum, portNum,
                                          CPSS_PORT_SPEED_2500_E);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortSpeedSet", rc);
                if( rc != GT_OK )
                    return rc;
            }

            /* Serdes power up 
               should be done per SERDES group that is 4 adjacent ports.*/
            for( portNum = 0 ; portNum < 24 ; portNum += 4 )
            {
                if( !PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) )
                    continue;
                rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum,
                                                           portNum / 4,
                                                           GT_TRUE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortGroupSerdesPowerStatusSet", rc);
                if( rc != GT_OK )
                    return rc;
            }
        }
    }

    return osTaskCreate("forceLinkUp",              /* Task Name      */
                          250,                      /* Task Priority  */
                          _8KB,                     /* Stack Size     */
                          netPortsForceLinkUp,      /* Starting Point */
                          (GT_VOID*)boardRevId_UINT, /* Arguments list */
                          &netPortsForceLinkUpTid); /* task ID        */

#else
    return GT_OK;
#endif
}

#ifndef ASIC_SIMULATION
/*******************************************************************************
* netPortsForceLinkUp
*
* DESCRIPTION:
*       Perform continuous polling of the network ports and do following:
*       - read the Phy link status
*       - when the link is up force CH3 link to up state
*       - keep probing the Phy link status and when it is down force down the port.
*
*
* INPUTS:
*       unused - this argument is not used.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applied only on regular network ports for devices:
*       CPSS_98DX5128_CNS and CPSS_98DX287_CNS.
*
*******************************************************************************/
static unsigned __TASKCONV netPortsForceLinkUp
(
    GT_VOID * param
)
{
    GT_U16                          regVal;     /* Register value */
    GT_U8                           portNum;    /* Port number */
    GT_U32                          i;          /* Array index */
    GT_STATUS                       rc;         /* Return status */
    GT_U8                           devNum;     /* Device number */
    GT_U32 boardRevId = (GT_UINTPTR)param;      /* Board revision Id */
    GT_U32 prevStatBit;                         /* Previouse port's link status */
    GT_U32 currStatBit;                         /* Current port's link status */

    osMemSet(portLinkStatusBmp, 0, sizeof(portLinkStatusBmp));

    while (1)
    {
        for(i = SYSTEM_DEV_NUM_MAC(0); i < SYSTEM_DEV_NUM_MAC(ppCounter); i++)
        {
            /* Do nothing with XG ports devices */
            switch(appDemoPpConfigList[i].deviceId)
            {
                case PRV_CPSS_CHEETAH3_XG_PORTS_DEVICES_CASES_MAC:
                    continue;/* skip the device */
                default:
                    break;
            }

            devNum = appDemoPpConfigList[i].devNum;
            /* Skip devices with revision differnt than 0 */
            if(PRV_CPSS_PP_MAC(devNum)->revision != 0)
            {
                continue;
            }

            /* DxCh3 with 24 ports plus 4 XG ports */
            /* Check ports link status and if link state down, force it to up */
            for(portNum = 0; portNum < 24; portNum++)
            {
                if(portNum > 19)
                {
                    /* Serdes combo mode available only on physical ports number,
                       values 20..23 are */
#if 0
                    the function call result in redundant interrupts.
                    the call temporary disabled.
                    rc = cpssDxChPortSerdesComboModeGet(devNum, portNum, &mode);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
#else

#endif

#if 0
                    /* Don't process  ports in SERDES combo mode */
                    if(mode == CPSS_DXCH_PORT_SERDES_MODE_COMBO_E)
                    {
                        continue;
                    }
#endif
                }

                /* get the link status from 88E1112 phy */
                rc = cpssDxChPhyPortSmiRegisterRead(devNum, portNum, 1, &regVal);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterRead", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

                currStatBit = (regVal >> 2) & 0x1;
                prevStatBit = DEV_PORT_LINK_STATUS_GET(devNum, portNum);

                if (prevStatBit != currStatBit)
                {
                    /* link is down */
                    if (currStatBit == 0)
                    {
                        /* Force link status down */
                        rc = cpssDxChPortForceLinkPassEnableSet(devNum, portNum,
                                                                GT_FALSE);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortForceLinkPassEnableSet", rc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    else
                    {
                        /* Force link status up */
                        rc = cpssDxChPortForceLinkPassEnableSet(devNum, portNum,
                                                                GT_TRUE);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortForceLinkPassEnableSet", rc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }

                    DEV_PORT_LINK_STATUS_SET(devNum, portNum, currStatBit);
                }
            }
        }

        /* Make loop only once for boardRevId == 2 of the board */
        if(boardRevId == 2)
        {
            return GT_OK;
        }

        osTimerWkAfter(1000);
    }
}
#endif /*!ASIC_SIMULATION*/

/*******************************************************************************
* gtDbDx348GE4HGSBoardReg
*
* DESCRIPTION:
*       Registeration function for the DB-DX3-GP board (Cheetah 3 devices).
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
GT_STATUS gtDbDx348GE4HGSBoardReg
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

