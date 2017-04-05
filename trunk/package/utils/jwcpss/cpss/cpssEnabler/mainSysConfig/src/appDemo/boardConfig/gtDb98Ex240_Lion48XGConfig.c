/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDb98Ex240_Lion48XGConfig.c
*
* DESCRIPTION:
*       Initialization functions for the VB ExMxPm 98Ex240 + Lion boards.
*
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

/* structures definition for the use of Dx Cheetah Prestera core SW */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>

/* structures definition for the use of Puma Prestera core SW */
#include <cpss/exMxPm/exMxPmGen/vb/cpssExMxPmVb.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgGen.h>

#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

/* number of cascade ports in the Lion devices */
#define NUM_LION_CSCD_PORTS_CNS   3
/* number of network ports in the Lion devices */
#define NUM_LION_NET_PORTS_CNS    18
/* number of cascading ports in Puma devices */
#define NUM_PUMA_CSCD_PORTS_CNS   2
/* number of virtual ports per one cascade port in Puma devices */
#define NUM_PUMA_VIRT_PORTS_PER_CSCD_PORT  (NUM_LION_NET_PORTS_CNS /  NUM_LION_CSCD_PORTS_CNS)
/* number of ports in Puma device */
#define PRV_CPSS_PUMA_PORTS_NUM 24

/*******************************************************************************
 * External variables
 ******************************************************************************/
static GT_U8       vbPpCounter;              /* Number of Packet processors. */

/* Puma virtual port to physical port mapping */
static GT_U8 virtPortToDsaTagSrcPortMap[NUM_LION_NET_PORTS_CNS] =
{0, 2, 4, 16, 18, 20, 32, 34, 36, 48, 50, 52, 6, 8, 22, 24, 38, 54};

/* Map network ports to virtual */
static GT_U8 netPortToVirtMap[NUM_LION_NET_PORTS_CNS] =
{0, 1, 2,  8,  9, 10, 18, 19, 20, 21, 22, 23, 4, 5,  6, 15, 16, 17};

/* Virtual ports info */
static CPSS_EXMXPM_VB_VIRT_PORT_INFO_STC
    virtPortsInfo[NUM_PUMA_CSCD_PORTS_CNS][NUM_PUMA_VIRT_PORTS_PER_CSCD_PORT];

/* private Fabric Initial Configuration function declaration */
GT_STATUS prvCpssExMxPmInitialFaConfig
(
    IN  GT_U8       devNum
);


static GT_STATUS gtDb98Ex240ConfigDxLion48XG_afterInitBoardConfig_inVb
(
    IN  GT_U8       boardRevId
);

/*******************************************************************************
 * External variables
 ******************************************************************************/
/* pumaBoardConfigFun will hold the puma board configuration */
/* we will use those pointers as "set default" values that we can override */
static GT_BOARD_CONFIG_FUNCS pumaBoardConfigFun;

static GT_U32   numOfActivePortGroups = 4; /* number of port groups -- assume single device */

/*******************************************************************************/
/***********************  Value Blade Configuration ****************************/
/*******************************************************************************/
/*  The Value blade  Configuration For 3 Pumas and 2 Lions                     */
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*      ********************   ********************   ********************     */
/*      *                  *   *                  *   *                  *     */
/*      *      PUMA        *   *       PUMA       *   *       PUMA       *     */
/*      *      Dev 0       *   *       Dev 1      *   *       Dev 3      *     */
/*      *                  *   *                  *   *                  *     */
/*      *                  *   *                  *   *                  *     */
/*      *  24          25  *   *  24         25   *   *   24         25  *     */
/*      ********************   ********************   ********************     */
/*          |          |          |           |           |          |         */
/*         XG0        XG1        XG0         XG1         XG0        XG1        */
/*          |          |          |           |           |          |         */
/*          |          |          |           |           |          |         */
/*          |          |          |           |           |          |         */
/*    **************************  |           |  **************************    */
/*    *     10         26      *  |           |  *        26         42   *    */
/*    *                        *  |           |  *                        *    */
/*    *          LION        42*__|           |__*10        LION          *    */
/*    *          Dev 4         *                 *          Dev 5         *    */
/*    *                        *                 *                        *    */
/*    *                        *                 *                        *    */
/*    **************************                 **************************    */
/*        |      |     |                               |      |     |          */
/*        |      |     |                               |      |     |          */
/*       18 1GE ports 0-54                            18 1GE ports 0-54        */
/*        |      |     |                               |      |     |          */
/*       Virtual ports 1-18                          Virtual ports 19-36       */
/*        |      |     |                               |      |     |          */
/*                                                                             */
/*******************************************************************************/
/*******************************************************************************/



/*******************************************************************************/
/***********************  Value Blade Configuration ****************************/
/*******************************************************************************/
/*  The Value blade  Configuration 1 Puma and 1 Lion                           */
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*        ********************                                                 */
/*        *                  *                                                 */
/*        *       PUMA       *                                                 */
/*        *       Dev 0      *                                                 */
/*        *                  *                                                 */
/*        *                  *                                                 */
/*        * 0          1     *                                                 */
/*        ********************                                                 */
/*          |          |                                                       */
/*         XG0        XG1                                                      */
/*          |          |                                                       */
/*          |          |                                                       */
/*          |          |                                                       */
/*    **************************                                               */
/*    *     10         26      *                                               */
/*    *                        *                                               */
/*    *          LION          *                                               */
/*    *          Dev 1         *                                               */
/*    *                        *                                               */
/*    *                        *                                               */
/*    ***************************                                              */
/*        |      |     |                                                       */
/*        |      |     |                                                       */
/*       18 1GE ports 0-54                                                     */
/*        |      |     |                                                       */
/*       Virtual ports 0-23                                                    */
/*        |      |     |                                                       */
/*                                                                             */
/*******************************************************************************/
/*******************************************************************************/


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
    GT_U8       lionDevIndex0;          /* Index of the Lion device in the
                                           array                        */
    GT_U32      devIndex;               /* device index                 */
    GT_U32      portGroupIndex;         /* port group index                   */

    retVal = appDemoSysGetPciInfo();
    if (retVal != GT_OK)
    {
        return retVal;
    }


    /* Count Puma devices */
    vbPpCounter = 0;
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if (appDemoPpConfigList[i].valid == GT_TRUE)
        {
            if (CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[i].devFamily))
            {
                vbPpCounter++;
            }
        }
    }

    lionDevIndex0 = vbPpCounter;

    /* each lion device has 4 port groups */
    appDemoPpConfigList[lionDevIndex0].numOfPortGroups = numOfActivePortGroups;

    /* set the first and second Lion devices to hold info about all the port groups */
    for(i = 0 ; i < numOfActivePortGroups; i++)
    {
        /*invalid all the devices , the first device will be validated after the loop */
        appDemoPpConfigList[(lionDevIndex0 + i)].valid = GT_FALSE;

        devIndex = lionDevIndex0;
        portGroupIndex = (i % numOfActivePortGroups);

        appDemoPpConfigList[devIndex].portGroupsInfo[portGroupIndex].portGroupActive = GT_TRUE;

        appDemoPpConfigList[devIndex].portGroupsInfo[portGroupIndex].portGroupPciInfo =
            appDemoPpConfigList[(devIndex + portGroupIndex)].pciInfo;
    }

    /* validate Lion first device */
    appDemoPpConfigList[lionDevIndex0].valid = GT_TRUE;
    appDemoPpConfigList[lionDevIndex0].pciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[lionDevIndex0].pciInfo.internalPciBase =  CPSS_PARAM_NOT_USED_CNS;
    vbPpCounter++;

    *numOfPp    = vbPpCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = GT_FALSE;

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
    CPSS_PP_PHASE1_INIT_PARAMS  localPpPh1Config = CPSS_PP_PHASE1_DEFAULT;
    void                        *intVecNum;
    GT_U32                      ii;

    localPpPh1Config.devNum             = SYSTEM_DEV_NUM_MAC(devIdx);

    if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devIdx].devFamily)) /* Puma device */
    {
        return pumaBoardConfigFun.boardGetPpPh1Params(boardRevId,devIdx,phase1Params);
    }
    else /* Lion devices */
    {
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

            extDrvGetPciIntVec(GT_PCI_INT_D - ii , &intVecNum);
            localPpPh1Config.portGroupsInfo[ii].intVecNum = (GT_U32)intVecNum;
            extDrvGetIntMask(GT_PCI_INT_D - ii , &localPpPh1Config.portGroupsInfo[ii].intMask);
        }

        /* retrieve PP Core Clock from HW */
        localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

        localPpPh1Config.uplinkCfg = CPSS_PP_NO_UPLINK_E;

        osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));

        /* Disable Flow Control for this revision */
        if(boardRevId == 3)
        {
            appDemoPpConfigList[devIdx].flowControlDisable = GT_TRUE;
        }

        osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));

        /* the device Id must be unique since the Puma will be configured using :
        CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_DSA_E */
        appDemoPpConfigList[devIdx].hwDevNum = SYSTEM_DEV_NUM_MAC(devIdx);
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
    return pumaBoardConfigFun.boardAfterPhase1Config(boardRevId);
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
    GT_STATUS                     rc;

    if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[oldDevNum].devFamily))
    {
        /* Puma device */
        rc = pumaBoardConfigFun.boardGetPpPh2Params(boardRevId,oldDevNum,phase2Params);
    }
    else /* Lion devices */
    {
        localPpPh2Config.devNum     = oldDevNum;
        localPpPh2Config.deviceId   = appDemoPpConfigList[oldDevNum].deviceId;

        localPpPh2Config.fuqUseSeparate = GT_TRUE;
        if( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E) ||
            ((systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E)  &&
             ((systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)              &&
                (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE))) )
        {
            rc = appDemoAllocateDmaMem(localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                                           RX_BUFF_SIZE_DEF,RX_BUFF_ALLIGN_DEF,
                                           TX_DESC_NUM_DEF,
                                           AU_DESC_NUM_DEF*numOfActivePortGroups,
                                           &localPpPh2Config);
        }


        osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));
    }

    return rc;
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
    GT_U8                   dev;
    GT_PHYSICAL_PORT_NUM    port; /* port number */
    GT_U8                   index;


    for(dev = 0; dev < PRV_CPSS_MAX_PP_DEVICES_CNS; dev++)
    {
        if((appDemoPpConfigList[dev].valid == GT_TRUE) &&
            (appDemoPpConfigList[dev].devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
        {
            index = 0;
            appDemoPpConfigList[dev].internalCscdPortConfigFuncPtr = cascadePortConfig;

            for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS;port++)
            {
                if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(dev)->existingPorts, port)))
                {
                    continue;
                }

                appDemoPpConfigList[dev].ports10GToConfigureArr[index] = port;
                index++;
            }
        }

        else if ((appDemoPpConfigList[dev].valid == GT_TRUE) &&
            (CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[dev].devFamily)))
        {

            /* XG ports */
            appDemoPpConfigList[dev].numberOf10GPortsToConfigure = 2;
            appDemoPpConfigList[dev].ports10GToConfigureArr[0] = 24;
            appDemoPpConfigList[dev].ports10GToConfigureArr[1] = 25;
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
    CPSS_PP_CONFIG_INIT_STC     localPpCfgParams = PP_LOGICAL_CONFIG_DEFAULT;

    if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily)) /* Puma device */
    {
        /* Puma devices */

        return pumaBoardConfigFun.boardGetPpLogInitParams(boardRevId,devNum,ppLogInitParams);
    }
    else
    {
        /* Lion devices */

        osMemSet(&localPpCfgParams, 0, sizeof(localPpCfgParams));

        localPpCfgParams.maxNumOfIpv4Prefixes = 3920;
        localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;
        localPpCfgParams.maxNumOfVirtualRouters = 1;
        localPpCfgParams.lpmDbFirstTcamLine = 100;
        localPpCfgParams.lpmDbLastTcamLine = 1204;

        osMemCpy(ppLogInitParams,&localPpCfgParams,sizeof(CPSS_PP_CONFIG_INIT_STC));
    }

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

    if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily)) /* Puma device */
    {
        /* Puma devices */

        return pumaBoardConfigFun.boardGetLibInitParams(boardRevId,devNum,libInitParams);
    }
    else
    {
        /* Lion device */

        osMemSet(&localLibInitParams, 0, sizeof(localLibInitParams));
        localLibInitParams.initClassifier            = GT_FALSE;
        localLibInitParams.initIpv6                  = GT_FALSE;
        localLibInitParams.initIpv4                  = GT_FALSE;

        localLibInitParams.initIpv4Filter            = GT_FALSE;
        localLibInitParams.initIpv4Tunnel            = GT_FALSE;
        localLibInitParams.initMpls                  = GT_FALSE;
        localLibInitParams.initMplsTunnel            = GT_FALSE;
        localLibInitParams.initPcl                   = GT_TRUE;

        /* there is no Policer lib init for CH3 devices */
        localLibInitParams.initPolicer               = GT_FALSE;

        osMemCpy(libInitParams,&localLibInitParams,
                 sizeof(APP_DEMO_LIB_INIT_PARAMS));
    }

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
    GT_STATUS   rc;

    /* let the puma board do it's things */
    rc = pumaBoardConfigFun.boardAfterInitConfig(boardRevId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("pumaBoardConfigFun.boardAfterInitConfig", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = gtDb98Ex240ConfigDxLion48XG_afterInitBoardConfig_inVb(boardRevId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("gtDb98Ex240ConfigDxLion48XG_afterInitBoardConfig_inVb", rc);
    return rc;
}

/*******************************************************************************
* gtDb98Ex240_DxLion48XGBoardReg
*
* DESCRIPTION:
*       Registration function for the Db98Ex240_DxLion48XGBoard Board
*       Value Blade.
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
GT_STATUS gtDb98Ex240_DxLion48XGBoardReg
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    GT_STATUS   rc;

    /* we will use those pointers as "set default" values that we can override */
    rc = gtDb98Ex240BoardReg(boardRevId,&pumaBoardConfigFun);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (boardCfgFuncs == NULL)
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



/*******************************************************************************
* gtDb98Ex240ConfigDxLion48XG_afterInitBoardConfig_inVb
*
* DESCRIPTION:
*       extra VB (value blade) specific configurations that should be done after
*       board initialization.
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
static GT_STATUS gtDb98Ex240ConfigDxLion48XG_afterInitBoardConfig_inVb
(
    IN  GT_U8                       boardRevId
)
{
    GT_STATUS                   rc = GT_OK;     /* return status            */
    GT_U8                       ii;             /* loop iterator            */
    GT_U8                       jj;             /* loop iterator            */
    GT_U8                       dev;            /* device number            */
    GT_U8                       firstLionDev;   /* first Lion device */
    GT_U8                       port;           /* port number              */
    GT_U8                       cscdPort;       /* cascade port number      */
    GT_U8                       virtPort;       /* virtual port number      */
    GT_U8                       cscdPortInd;    /* cascade port index       */
    GT_U32                      tc;             /* traffic class            */
    GT_U8                       portGroupId;    /* port group index         */
    GT_U8                       portInd;        /* port index               */
    GT_U32                      networkPortsNum=NUM_LION_NET_PORTS_CNS;
                                                /* number of network ports  */
    GT_U8                       networkPortsArr[NUM_LION_NET_PORTS_CNS];
                                                /* network ports array      */
    GT_U8                       networkPortCscdArr[NUM_LION_NET_PORTS_CNS];
                                                /* cascade ports connected to
                                                   network ports array      */
    GT_U32                      cascadePortsNum=NUM_LION_CSCD_PORTS_CNS;
                                                /* number of  cascade ports */
    GT_U8                       cascadePortsArr[NUM_LION_CSCD_PORTS_CNS];
                                                /* array of cascade ports  for Lion */
    static CPSS_EXMXPM_VB_CSCD_PORT_INFO_STC    cscdPortsArray[PRV_CPSS_PUMA_PORTS_NUM];
                                                /* array of cascade ports for Puma
                                                   2 cascade ports for each device
                                                   and set all not used 22 ports as cascading */

    /* first Lion device should be either 1 or 2 */
    firstLionDev = (vbPpCounter == 2) ? 1 : 2;

    /* go over all devices and virtual ports per device */
    for (ii = 0; (ii < PRV_CPSS_MAX_PP_DEVICES_CNS) && (GT_OK == rc); ii++)
    {
        if (GT_FALSE == appDemoPpConfigList[ii].valid)
        {
            continue;
        }
        /* get device id and number of virtual ports per device */
        dev = appDemoPpConfigList[ii].devNum;

        if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[ii].devFamily))
        {
            /*******************************************************************************/
            /********************** Puma Value Blade Configuration *************************/
            /*******************************************************************************/
            /* Set the access mode of the "DSA Virtual Port-ID assignment" table, *
             * In Puma2-24G it must be DSA                                        *
             * Must be called before cpssExMxPmVbConfigSet or                     *
             * cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet                  */
            rc = cpssExMxPmVbVirtualPortsMapModeSet(dev, CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_DSA_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbVirtualPortsMapModeSet", rc);
            if(rc != GT_OK)
                return rc;

            for (port = 0 ; port < NUM_PUMA_CSCD_PORTS_CNS ; port++)
            {
                /* Enable value mode on port. The index to the Virtual port ID      *
                 * mapping table is controlled by global virtual port map mode,     *
                 * according to the call to cpssExMxPmVbVirtualPortsMapModeSet API  */
                rc = cpssExMxPmVbPortValueModeEnableSet(dev, port, GT_TRUE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbPortValueModeEnableSet", rc);
                if(rc != GT_OK)
                    return rc;

                /* Enable DSA Tag manipulation per port, when packet *
                 * is written to packet buffer memory                */
                rc = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(dev, port, GT_TRUE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbIngressPortDsaTagManipulationEnableSet", rc);
                if(rc != GT_OK)
                    return rc;

            }

            /* Enable cascading - this setting should be internal */
            prvCpssHwPpSetRegField(dev, 0x100000, 12, 2, 0);
            prvCpssHwPpSetRegField(dev, 0x340004, 31, 1, 1);

            /* Set cascade mode per port to be value_port. */
            prvCpssHwPpSetRegField(dev, 0x00D00084, 0, 32, 0x00FFFFFF);

            for (cscdPort = 0 ; cscdPort < NUM_PUMA_CSCD_PORTS_CNS ; cscdPort++)
            {
                cscdPortsArray[cscdPort].cscdIngressPhysicalPortNum = cscdPort;
                /* the VB should use extended DSA tag because Lion has 48 ports
                   and whole 6 bits of srcPort should be used. */
                cscdPortsArray[cscdPort].cscdIngressPortDsaType     = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
                cscdPortsArray[cscdPort].virtPortsInfoSize          = NUM_PUMA_VIRT_PORTS_PER_CSCD_PORT;
                cscdPortsArray[cscdPort].virtPortsInfoPtr           = virtPortsInfo[cscdPort];

                for (virtPort = 0; virtPort < NUM_PUMA_VIRT_PORTS_PER_CSCD_PORT; virtPort++ )
                {
                    if (dev != 1)
                    {
                        /* Puma dev0 and dev2 are using one target device */

                        /* calculate port index in the netToVirt mapping array */
                        portInd = virtPort + (cscdPort * NUM_PUMA_VIRT_PORTS_PER_CSCD_PORT);

                        port = (netPortToVirtMap[portInd] + (dev * 9));
                    }
                    else
                    {
                        /* Puma dev1 is connected to two target devices */

                        /* calculate port index in the netToVirt mapping array */
                        portInd = virtPort + (2 * NUM_PUMA_VIRT_PORTS_PER_CSCD_PORT);

                        port =
                          netPortToVirtMap[portInd] + (NUM_LION_NET_PORTS_CNS * cscdPort);
                    }

                    virtPortsInfo[cscdPort][virtPort].portNum = port;

                    /* set device numbers that come after the puma device number */
                    /* 18 cascade ports per Target device */
                    virtPortsInfo[cscdPort][virtPort].dsaTagSrcDev =
                         firstLionDev + (portInd / (2 * NUM_PUMA_VIRT_PORTS_PER_CSCD_PORT));

                    /* 18 Network ports on each Target device */
                    virtPortsInfo[cscdPort][virtPort].dsaTagSrcPort =
                        virtPortToDsaTagSrcPortMap[portInd];

                    for (tc=0; tc < CPSS_TC_RANGE_CNS; tc++)
                    {
                        virtPortsInfo[cscdPort][virtPort].egressPhysicalPortsNum[tc] =
                            cscdPortsArray[cscdPort].cscdIngressPhysicalPortNum;

                        virtPortsInfo[cscdPort][virtPort].egressPhysicalTcMap[tc] = tc;
                    }
                }
            }

            /* configure not connected ports as cascading */
            for (/* continue prev loop */ ; cscdPort < PRV_CPSS_PUMA_PORTS_NUM ; cscdPort++)
            {
                cscdPortsArray[cscdPort].cscdIngressPhysicalPortNum = cscdPort;
                cscdPortsArray[cscdPort].cscdIngressPortDsaType     = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
                cscdPortsArray[cscdPort].virtPortsInfoSize          = 0;
                cscdPortsArray[cscdPort].virtPortsInfoPtr           = NULL;
            }

            /*  Set the VB configuration for all virtual ports to the network ports in *
             *  the mux devices.                                                       *
             *  All physical ExMxPm ports that are not included in the INPUT info      *
             *  (in cscdPortsArray) will be set to HW as non cascade ports.            */
            rc = cpssExMxPmVbConfigSet(dev, PRV_CPSS_PUMA_PORTS_NUM, cscdPortsArray);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbConfigSet", rc);
            if(rc != GT_OK)
                return rc;

            /* Recommended Configuration:
            * Speed: 1 Gbps                    *
            * Duplex: Full-duplex              *
            * Flow Control: Disabled           *
            * Pause: Do not send Pause frames  */

            /* change the inter port/phy config ONLY on board that
               actually have VB devices ---
               because this function allow to set a Puma device on board
               that is not real connected to Dx mux devices */
            for (port = 0 ; port < NUM_PUMA_CSCD_PORTS_CNS; port++)
            {

                if(!(PRV_CPSS_PP_MAC(dev)->existingPorts.ports[0] & (1 << port)))
                    continue;

                /* Speed 1Gbps */
                rc = cpssExMxPmPortSpeedAutoNegEnableSet(dev, port, GT_FALSE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortSpeedAutoNegEnableSet", rc);
                if(rc != GT_OK)
                    return rc;
                rc = cpssExMxPmPortSpeedSet(dev, port, CPSS_PORT_SPEED_1000_E);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortSpeedSet", rc);
                if(rc != GT_OK)
                    return rc;

                /* Duplex: Full-duplex */
                rc = cpssExMxPmPortDuplexAutoNegEnableSet(dev, port, GT_FALSE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortDuplexAutoNegEnableSet", rc);
                if(rc != GT_OK)
                    return rc;
                rc = cpssExMxPmPortDuplexModeSet(dev, port, CPSS_PORT_FULL_DUPLEX_E);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortDuplexModeSet", rc);
                if(rc != GT_OK)
                    return rc;

                /* Disable flow control over cascade ports. */
                rc = cpssExMxPmPortFlowControlEnableSet(dev, port, GT_FALSE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortFlowControlEnableSet", rc);
                if(rc != GT_OK)
                    return rc;

                /* Do not send Pause frames */
                rc = cpssExMxPmPortFlowControlAutoNegEnableSet(dev, port, GT_FALSE, GT_FALSE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortFlowControlAutoNegEnableSet", rc);
                if(rc != GT_OK)
                    return rc;
            }


            for (port = 0 ; port < 64; port++)
            {
                if(port == PRV_CPSS_PP_MAC(dev)->numOfVirtPorts)
                    port = CPSS_CPU_PORT_NUM_CNS;

                rc = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(dev, port, GT_FALSE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet", rc);
                if(rc != GT_OK)
                    return rc;
            }

            /*******************************************************************************/
            /******************* End of Puma Value Blade Configuration *********************/
            /*******************************************************************************/
        }
        else
        {
            /*******************************************************************************/
            /********************   LION Value Blade Configuration  ************************/
            /*******************************************************************************/

            /* cascade ports start after the network ports
               cascaded port numbers are: 10, 26 and 42 */
            cascadePortsArr[0] = 10;
            cascadePortsArr[1] = 26;
            cascadePortsArr[2] = 42;

            port = 0;
            for (portGroupId = 0; portGroupId < APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS; portGroupId++)
            {
                for (jj = 0; jj < 5; jj++)
                {
                    if (port >= NUM_LION_NET_PORTS_CNS)
                    {
                        break;
                    }

                    networkPortsArr[port] = (portGroupId * 16) + (jj * 2);

                    switch (networkPortsArr[port])
                    {
                        case 0:
                        case 2:
                        case 4:
                        case 16:
                        case 18:
                        case 20:
                            cscdPortInd = 0;
                            break;

                        case 32:
                        case 34:
                        case 36:
                        case 48:
                        case 50:
                        case 52:
                            cscdPortInd = 1;
                            break;

                        default:
                            cscdPortInd = 2;
                            break;
                    }

                    networkPortCscdArr[port] = cascadePortsArr[cscdPortInd];
                    port++;
                }
            }


            rc = appDemoDxChVbMuxInit(ii,
                                networkPortsNum,networkPortsArr,networkPortCscdArr,
                                cascadePortsNum,cascadePortsArr);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChVbMuxInit", rc);
            if(rc != GT_OK)
                return rc;

            /*******************************************************************************/
            /******************  End of LION Value Blade Configuration  ********************/
            /*******************************************************************************/
        }
    }

    return rc;
}


