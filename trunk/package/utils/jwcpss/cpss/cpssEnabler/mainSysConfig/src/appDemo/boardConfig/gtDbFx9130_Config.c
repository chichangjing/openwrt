/*******************************************************************************
*              Copyright 2002, MARVELL TECHNOLOGY, LTD.
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
* gtDbFx9130_Config.c
*
* DESCRIPTION:
*       Initialization functions for the FX9130 boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sandConfig/appDemoFe200Config.h>
/* SAND includes */
#include <SAND/Management/include/sand_low_level.h>
#include <SAND/SAND_FM/include/sand_mem_access.h>
#include <SAND/SAND_FM/include/sand_chip_defines.h>

#include <FE200/include/fe200_api_init.h>
#include <FE200/include/fe200_api_general.h>
#include <FE200/include/fe200_api_configuration.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/

static GT_U32 unitNum = 0;

/* compile mainCmd shell with -D PRE_SHELL_FUNCTION=duneFe200Brd0002aAutoInit */
/* for automatic execution                                            */
void gtDbFx9130BoardAutoInit(void)
{
    GT_STATUS rc;
    
    osPrintf("automatic cpssInitSystem(2, 1, 0) run...\n");
    rc = cpssInitSystem(2, 1, 0);
    osPrintf("automatic gtInitSystem(2, 1, 0) returned %d\n", rc);
}
/*******************************************************************************
* appDemoFe200UnitNumSet
*
* DESCRIPTION:
*       cofigure Unit Num before cpssInitSystem.
*
* INPUTS:
*       unitNumNew  - The board unit Num.
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
GT_VOID appDemoFe200UnitNumSet
(
    IN GT_U32 unitNumNew
)
{
    unitNum = unitNumNew;
}

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
    *numOfPp    = 0;
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
    OUT CPSS_PP_PHASE1_INIT_PARAMS    *phase1Params
)
{
    CPSS_PP_PHASE1_INIT_PARAMS    localPpPh1Config = CPSS_PP_PHASE1_DEFAULT;

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

    osMemCpy(libInitParams,&localLibInitParams,
             sizeof(APP_DEMO_LIB_INIT_PARAMS));
    return GT_OK;
}


/*******************************************************************************/
/* number of FE planes, default is 8 for NEGEV chassys */
static APPDEMO_FE200_INIT_NOF_PLANES brd02ASetNofFe200Planes =
    APPDEMO_FE200_INIT_8_PLANES;
static GT_BOOL brd02ASetNofFe200PlanesWasSet = GT_FALSE;

/*******************************************************************************
* Brd02ASetNofFe200Planes
*
* DESCRIPTION:
*       Sets number of FE200 planes used in gtInitSystem.
*
* INPUTS:
*       boardRevId  - The board revision Id.
*
* OUTPUTS:
*       nofFe200Planes    - number of FE200 planes
*                           (APPDEMO_FE200_INIT_NOF_PLANES value )
*
* RETURNS:
*       None
*
* COMMENTS:
*       must be called before gtInitSystem.
*
*******************************************************************************/
GT_VOID Brd02ASetNofFe200Planes
(
    IN  APPDEMO_FE200_INIT_NOF_PLANES   nofFe200Planes
)
{
    brd02ASetNofFe200Planes = nofFe200Planes;
    brd02ASetNofFe200PlanesWasSet = GT_TRUE;
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
    unsigned int baseAddr[2] =
        {0x40000000, 0x40100000};      /* base addresses */
    GT_U32                   i;        /* loop index */
    GT_U32                   port;     /* loop index */
    GT_STATUS                retVal;   /* returned status */
    unsigned int             tcm_enable_flag;
    GT_U32 errorId;                    /* error id returned from SAND */

    static FE200_ALL_SERDES_STATUS  serdes_status ; /* serdes initial status */
    static FE200_ALL_SERDES_STATUS  serdes_param ;  /* serdes read, update, write */
    static SAND_PHYSICAL_ACCESS     physicalAccess; /* access bind structure */
    static APPDEMO_FE200_INIT_PARAMS  feInit;         /* FE initial configuration */
    static FE200_ALL_LINK_CONNECTIVITY connectMap;  /* connectivity map to read */
    unsigned int             device_handle;  /* SW device id             */
    unsigned long            chip_id;        /* HW device id             */

    if (brd02ASetNofFe200PlanesWasSet == GT_FALSE)
    {
        /* initialization for NEGEV */
        if (boardRevId == 1)
        {
            brd02ASetNofFe200Planes = APPDEMO_FE200_INIT_16_PLANES;
        }

        /* initialization for GOBY */
        if (boardRevId == 2)
        {
            brd02ASetNofFe200Planes = APPDEMO_FE200_INIT_1_PLANE;
        }
    }

    osMemSet(&feInit, 0, sizeof(feInit));

    /* flag "to change" and pointer to serdes configuration */
    /* the "serdes_status" filled by appDemoDuneGetSerdesParameters below */
    feInit.change_serdes_status = 1;
    feInit.serdes_status_ptr = &serdes_status;

    /* flag "don't change" mode, assumed FE2 */
    feInit.fe2_not_fe13.change_var = 0;
    feInit.fe2_not_fe13.var = 1;

    /* flag "to change" and profile id for version B */
    feInit.initial_profile_id.change_var = 1;
    feInit.initial_profile_id.var = FE200_STANDARD_PROFILE_REV_B;

    /* flag "don't change" is_multi_stage */
    feInit.is_multi_stage.change_var = 0;
    feInit.is_multi_stage.var = 0;

    /* flag "don't change" repeator */
    feInit.is_repeater.change_var = 0;
    feInit.is_repeater.var = 0;

    /* all devices in system - revision B */
    feInit.is_rev_a_device_in_system = 0;

    feInit.mci_4_prio_enable = 1;
    feInit.nof_planes.change_var = 1;
    feInit.nof_planes.var = brd02ASetNofFe200Planes;

    for (i = 0; (i < FE200_FE2_LINKS_NOF_LONGS); i++)
    {
        feInit.fabric_link_fc_setting.rx_enable[i] = 0xFFFFFFFF;
        feInit.fabric_link_fc_setting.tx_enable[i] = 0xFFFFFFFF;
    }


    /* initialize SAND module */
    if ((retVal = appDemoSandInit ()) != GT_OK)
    {
        return retVal;
    }

    tcm_enable_flag = sand_tcm_get_enable_flag() ;

    sand_tcm_set_enable_flag(FALSE) ;

    /* bind physical access read/write functions */
    physicalAccess.physical_write = sand_eci_write;
    physicalAccess.physical_read = sand_eci_read;

    if (sand_set_physical_access_hook(&physicalAccess) != SAND_OK)
    {
        return GT_FAIL;
    }


    for (i = 0; (i < 2); i++)
    {
        feInit.base_address = (unsigned long*)(baseAddr[i]);

        feInit.chip_id.change_var = 0;
        feInit.chip_id.var = unitNum + i;

        for (port = 0; (port < FE200_NUM_OF_SERDESES); port++)
        {
            appDemoDuneGetSerdesParameters(
                DUNE_CHASSY_NEGEV,
                1000 /*DUNE_BOARD_BRD0012A*/, /*- cause unsupported case */
                DUNE_BOARD_BRD0002A,
                GT_FALSE /*serdesOnFap*/,
                0 /*slotNum*/,
                i /*devRelNum*/,
                port /*portNum*/,
                &(serdes_status.single_serdes_status[port]));
        }


        errorId = appDemoFe200InitDevice(
            &feInit /*fmc_n_fe200_init_params*/,
            (SAND_RESET_DEVICE_FUNC_PTR)NULL,
            FE2_DEVICE,
            &physicalAccess,
            &device_handle,
            &chip_id);

        if (sand_get_error_code_from_error_word(errorId) != SAND_OK)
        {
            return GT_FAIL;
        }

        /* phase 2 init */
        errorId = fe200_init_sequence_phase2(device_handle);
        if (sand_get_error_code_from_error_word(errorId) != SAND_OK)
        {
            return GT_FAIL;
        }

        /* wait 2 mili second to stable state */
        osTimerWkAfter(2);

        /* clear_various_counters */
        errorId = appDemoFe200ClearVariousCounters(device_handle);
        if (sand_get_error_code_from_error_word(errorId) != SAND_OK)
        {
            return GT_FAIL;
        }

        /*fmc_n_fe200_spatial_multicast_app initializes VIDs 1-31 only */
        /* we add also 4090-4105 (IPV4 MC has id 4094)                 */
        errorId = appDemoFe200SpatialMulticastConfigGroupRange(
            device_handle, TRUE/*silent*/, 1, 31);
        if (sand_get_error_code_from_error_word(errorId) != SAND_OK)
        {
            return GT_FAIL;
        }
        errorId = appDemoFe200SpatialMulticastConfigGroupRange(
            device_handle, TRUE/*silent*/, 4090,16);
        if (sand_get_error_code_from_error_word(errorId) != SAND_OK)
        {
            return GT_FAIL;
        }

        /* wait 5 second to stable state */
        osTimerWkAfter(5000);

        /* power down loop backed serdeses */
        errorId = fe200_get_connectivity_map(
            device_handle,
            &connectMap);
        if (sand_get_error_code_from_error_word(errorId) != SAND_OK)
        {
            return GT_FAIL;
        }

        errorId = fe200_get_serdes_params(
            device_handle,
            (unsigned long*)&serdes_param);
        if (sand_get_error_code_from_error_word(errorId) != SAND_OK)
        {
            return GT_FAIL;
        }

        for (port = 0; (port < FE200_NUM_OF_SERDESES); port++)
        {
            if (connectMap.single_link_connectivity[port].
                other_entity_illegal == 0)
            {
                /* valid link - not change */
                serdes_param.single_serdes_status[port].valid = 0;
                continue;
            }

            /* power down */
            serdes_param.single_serdes_status[port].valid = 1;
            serdes_param.single_serdes_status[port].power_down_rx_tx = 1;
        }

        errorId = fe200_set_serdes_params(
            device_handle,
            (unsigned long*)&serdes_param);
        if (sand_get_error_code_from_error_word(errorId) != SAND_OK)
        {
            return GT_FAIL;
        }

    }


    sand_tcm_set_enable_flag(tcm_enable_flag) ;

    osPrintf("afterInitDuneBoardConfig success\n");
    return GT_OK;
}


/*******************************************************************************
* gtDbFx9130BoardReg
*
* DESCRIPTION:
*       Registeration function for the DB-FX9130 board (FE200).
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
GT_STATUS gtDbFx9130BoardReg
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

