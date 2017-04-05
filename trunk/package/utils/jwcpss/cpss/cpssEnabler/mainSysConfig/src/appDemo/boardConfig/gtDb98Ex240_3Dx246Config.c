/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDb98Ex240_3Dx246Config.c
*
* DESCRIPTION:
*       Initialization functions for the ExMxPm 98Ex240 + 3Dx246 boards.
*
*
* FILE REVISION NUMBER:
*       $Revision: 29 $
*
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/
extern void exMxPmAsVbSet(GT_U32  newVal);
extern GT_U32  exMxPmBoardVbDevicesExists;

/* private Fabric Initial Configuration function declaration */
GT_STATUS prvCpssExMxPmInitialFaConfig
(
IN  GT_U8          devNum
);

/*******************************************************************************
 * External variables
 ******************************************************************************/
/* pumaBoardConfigFun will hold the puma board configuration */
/* we will use those pointers as "set default" values that we can override */
static GT_BOARD_CONFIG_FUNCS pumaBoardConfigFun;

/*******************************************************************************/
/***********************  Value Blade Configuration ****************************/
/*******************************************************************************/
/*  The Value blade  Configuration                                             */
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*                       ************************                              */
/*                       *                      *                              */
/*                       *     PUMA             *                              */
/*                       *     24 GE ports      *                              */
/*                       *                      *                              */
/*                       *      Dev 0           *                              */
/*                       *                      *                              */
/*                       ************************                              */
/*                       |        |            |                               */
/*          8 1GE Ports,0-7  8 1GE Ports,8-15  8 1GE Ports,16-23               */
/*                       |        |            |                               */
/*            ------------        |            |---------                      */
/*            |                   |                     |                      */
/*            |                   |                     |                      */
/*     8 1GE Ports,16-23      8 1GE Ports,16-23        8 1GE Port,16-23        */
/*            |                   |                     |                      */
/*            |                   |                     |                      */
/*    *****************      *****************        *****************        */
/*    *               *      *               *        *               *        */
/*    *  Cheetah      *      *  Cheetah      *        *  Cheetah      *        */
/*    *  24 GE ports  *      *  24 GE ports  *        *  24 GE ports  *        */
/*    *               *      *               *        *               *        */
/*    *    Dev 1      *      *    Dev 2      *        *   Dev 3       *        */
/*    *               *      *               *        *               *        */
/*    *****************      *****************        *****************        */
/*            |                     |                       |                  */
/*            |                     |                       |                  */
/*   16 1GE Ports 0-15       16 1GE Ports 0-15        16 1GE Ports 0-15        */
/*            |                     |                       |                  */
/*            |                     |                       |                  */
/*                                                                             */
/*   Virtual Ports 0-15      Virtual Ports 16-31      Virtual Ports 32-47      */
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
    GT_U8       ppCounter;              /* Number of Packet processors. */
    GT_U8       i;                      /* Loop index.                  */

    retVal = appDemoSysGetPciInfo();
    if (retVal != GT_OK)
    {
        return retVal;
    }


    /* Invalidate information of the CH+ devices */
    appDemoPpConfigList[1].valid = GT_FALSE;
    appDemoPpConfigList[2].valid = GT_FALSE;
    appDemoPpConfigList[3].valid = GT_FALSE;

    retVal = appDemoSysGetSmiInfo();
    if(retVal != GT_OK)
    {
        return retVal;
    }

    ppCounter = 0;
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if (appDemoPpConfigList[i].valid == GT_TRUE)
        {
            ppCounter++;
        }
    }

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = GT_FALSE;

    /* we run as VB */
    exMxPmAsVbSet(1);

    /* state that board have VB devices */
    exMxPmBoardVbDevicesExists = 1;

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

    localPpPh1Config.devNum             = SYSTEM_DEV_NUM_MAC(devIdx);

    if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devIdx].devFamily)) /* Puma device */
    {
        return pumaBoardConfigFun.boardGetPpPh1Params(boardRevId,devIdx,phase1Params);
    }
    else /* CH+ devices */
    {
        localPpPh1Config.baseAddr         = appDemoPpConfigList[devIdx].smiInfo.smiIdSel;
        localPpPh1Config.internalPciBase  = 0x0;
        localPpPh1Config.deviceId         =
            ((appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.devId << 16) |
             (appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.vendorId));

        localPpPh1Config.intVecNum        = 12; /* GPIO */
        localPpPh1Config.intMask          = 12; /* GPIO */

        localPpPh1Config.mngInterfaceType                      = CPSS_CHANNEL_SMI_E;

        /* the CLK is 188 MHz */
        localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

        /* the device Id must be unique since the Puma will be configured using :
        CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_DSA_E */
        appDemoPpConfigList[devIdx].hwDevNum = SYSTEM_DEV_NUM_MAC(devIdx);
    }

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
    GT_STATUS rc;
    GT_U32 i;

    rc = pumaBoardConfigFun.boardAfterPhase1Config(boardRevId);
    if(rc != GT_OK)
    {
        return rc;
    }

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if( (appDemoPpConfigList[i].valid == GT_TRUE) &&
            (0 == CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[i].devFamily)))
        {
            /* Remove initialization of CPSS libraries(ip, bridge ...)  *
             * and general init configuration for the DX devices        */
            appDemoPpConfigList[i].sysConfigFuncs.cpssLibrariesInit = NULL;
            appDemoPpConfigList[i].sysConfigFuncs.cpssPpGeneralInit = NULL;
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

    if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[oldDevNum].devFamily)) /* Puma device */
    {
        return pumaBoardConfigFun.boardGetPpPh2Params(boardRevId,oldDevNum,phase2Params);
    }
    else /* CH+ devices */
    {
        localPpPh2Config.devNum     = oldDevNum;
        localPpPh2Config.baseAddr   =
            appDemoPpConfigList[oldDevNum].smiInfo.smiIdSel;
        localPpPh2Config.deviceId   =
            ((appDemoPpConfigList[oldDevNum].smiInfo.smiDevVendorId.devId << 16) |
             (appDemoPpConfigList[oldDevNum].smiInfo.smiDevVendorId.vendorId));


        localPpPh2Config.auqCfg.auDescBlockSize = 0;
        localPpPh2Config.auqCfg.auDescBlock       = NULL;
        localPpPh2Config.fuqCfg = localPpPh2Config.auqCfg;
        localPpPh2Config.netIfCfg.txDescBlock     = NULL;
        localPpPh2Config.netIfCfg.txDescBlockSize = 0;
        localPpPh2Config.netIfCfg.rxDescBlock     = NULL;
        localPpPh2Config.netIfCfg.rxDescBlockSize = 0;
        localPpPh2Config.netIfCfg.rxBufInfo.allocMethod = CPSS_RX_BUFF_NO_ALLOC_E;
        localPpPh2Config.rxBufSize = 0;
        localPpPh2Config.headerOffset = 0;
    }

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
    GT_U8                   numOfPorts = 24;
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


    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if( (appDemoPpConfigList[i].valid == GT_TRUE) &&
            (0 == CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[i].devFamily)))
        {
            /* configure leds, led organized by port (port data used) */
            /*in pss - ledStreamClassAndGroupConfig*/
            retVal = cpssDxChLedStreamClassAndGroupConfig(appDemoPpConfigList[i].devNum, confRregVal,
                                                  ledClassConf, 1,
                                                  ledGroupConf, 4);

            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChLedStreamClassAndGroupConfig", retVal);
            if (retVal != GT_OK)
            {
                return retVal;
            }

            for(j = 0; j < numOfPorts; j++)
            {
                /* init the phy location on the baord*/
                retVal = appDemoSetPortPhyAddr(appDemoPpConfigList[i].devNum,j,
                                            (GT_U8)(0x4 + (j%12)));
                CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoSetPortPhyAddr", retVal);
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
    CPSS_PP_CONFIG_INIT_STC     localPpCfgParams = PP_LOGICAL_CONFIG_DEFAULT;

    if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily)) /* Puma device */
    {
        return pumaBoardConfigFun.boardGetPpLogInitParams(boardRevId,devNum,ppLogInitParams);
    }
    else /* DX devices */
    {
        osMemSet(&localPpCfgParams, 0, sizeof(localPpCfgParams));
    }

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

    if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily)) /* Puma device */
    {
        return pumaBoardConfigFun.boardGetLibInitParams(boardRevId,devNum,libInitParams);
    }
    else /* DX3 device */
    {
        osMemSet(&localLibInitParams, 0, sizeof(localLibInitParams));
    }

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
    GT_STATUS   rc;

    /* let the puma board do it's things */
    rc = pumaBoardConfigFun.boardAfterInitConfig(boardRevId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("pumaBoardConfigFun.boardAfterInitConfig", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* gtDb98Ex240_3Dx246BoardReg
*
* DESCRIPTION:
*       Registration function for the 98Ex240_3Dx246 Board Value Blade
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
GT_STATUS gtDb98Ex240_3Dx246BoardReg
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    GT_STATUS rc;

    if (boardCfgFuncs == NULL)
    {
        return GT_FAIL;
    }

    /* let the generic board configuration , give us the functions of it's
       implementation */
    /* we will use those pointers as "set default" values that we can override */
    rc = gtDb98Ex240BoardReg(boardRevId,&pumaBoardConfigFun);
    if(rc != GT_OK)
    {
        return rc;
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

