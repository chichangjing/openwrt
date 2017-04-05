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
* appDemoBoardConfig.h
*
* DESCRIPTION:
*       Includes board specific initialization definitions and data-structures.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 29 $
*
*******************************************************************************/
#ifndef __appDemoBoardConfigh
#define __appDemoBoardConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef IMPL_FA
#include <cpssFa/generic/cpssFaHwInit/gtCoreFaHwInit.h>
#endif

#ifdef IMPL_XBAR
#include <cpssXbar/generic/xbarHwInit/gtCoreXbarHwInit.h>
#endif

#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#define IMPL_DBG_TRACE

#ifdef IMPL_DBG_TRACE
    #ifdef  _WIN32
        #define DBG_TRACE(x) osPrintf x
    #else /*!_WIN32*/
        #define DBG_TRACE(x) osPrintSync x
    #endif /*!_WIN32*/
#else
    #define DBG_TRACE(x)
#endif

/* flag to state that the trace will print also functions that return GT_OK */
/* usually we not care about 'GT_OK' only about fails */
/* the flag can be changed before running the cpssInitSystem(...) --> from terminal vxWorks */
extern GT_U32  appDemoTraceOn_GT_OK;

#define CPSS_ENABLER_DBG_TRACE_RC_MAC(_title, _rc)     \
   if(_rc != GT_OK || appDemoTraceOn_GT_OK)             \
       DBG_TRACE(("%s returned: 0x%X at file %s, line = %d\r\n", _title, _rc, __FILE__, __LINE__))

/*******************************************************************************************/

/* Trace enabled by appDemoTraceEnableFilterAdd(line_start, line_end, file_name_substring) */
/*******************************************************************************
* appDemoTraceEnableFilterCheck
*
* DESCRIPTION:
*       This function ckecks is trace printing enable for this trace statement
*
* INPUTS:
*       filePath - path to source file returned by __FILE__ macro.
*       lineNum  - number of line returned by __LINE__ macro.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*******************************************************************************/
GT_BOOL appDemoTraceEnableFilterCheck
(
    IN GT_U8  *filePath,
    IN GT_U32 lineNum
);

/* Trace */
/* Trace print line */
#define CPSS_ENABLER_FILTERED_TRACE_MAC(x) \
    if (appDemoTraceEnableFilterCheck((GT_U8*)__FILE__, (GT_U32)__LINE__) != GT_FALSE) {DBG_TRACE(x);}
/* Trace call evaluate expession on trace condition */
#define CPSS_ENABLER_FILTERED_TRACE_EXEC_MAC(x) \
    if (appDemoTraceEnableFilterCheck((GT_U8*)__FILE__, (GT_U32)__LINE__) != GT_FALSE) {x;}
/*******************************************************************************************/

/* when appDemoOnDistributedSimAsicSide == 1
   this image is running on simulation:
   1. our application on distributed "Asic side"
   2. the Other application that run on the "application side" is in
      charge about all Asic configurations
   3. our application responsible to initialize the cpss DB.
   4. the main motivation is to allow "Galtis wrappers" to be called on
      this side !
      (because the Other side application may have difficult running
       "Galtis wrappers" from there)
*/
extern GT_U32  appDemoOnDistributedSimAsicSide;

/*
 * Typedef: FUNCP_GET_BOARD_INFO
 *
 * Description:
 *      Provides board specific memory size configuration.
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      numOfMemBytes   - Memory size in bytes.
 *
 */
typedef GT_STATUS (*FUNCP_GET_BOARD_MEMORY_INFO)
(
    IN  GT_U8   boardRevId,
    OUT GT_U32  *numOfMemBytes
);

/*
 * Typedef: FUNCP_BOARD_CLEAN_DB_DURING_SYSTEM_RESET
 *
 * Description:
 *      clear the DB of the specific board config file , as part of the 'system rest'
 *      to allow the 'cpssInitSystem' to run again as if it is the first time it runs
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *
 */
typedef GT_STATUS (*FUNCP_BOARD_CLEAN_DB_DURING_SYSTEM_RESET)
(
    IN  GT_U8   boardRevId
);

/*
 * Typedef: FUNCP_GET_BOARD_INFO
 *
 * Description:
 *      General board information for initial initialization.
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      numOfPp         - Number of Packet processors in system.
 *      numOfFa         - Number of Fabric Adaptors in system.
 *      numOfXbar       - Number of Crossbar devices in system.
 *      isB2bSystem     - GT_TRUE, the system is a B2B system.
 *
 */
typedef GT_STATUS (*FUNCP_GET_BOARD_INFO)
(
    IN  GT_U8   boardRevId,
    OUT GT_U8   *numOfPp,
    OUT GT_U8   *numOfFa,
    OUT GT_U8   *numOfXbar,
    OUT GT_BOOL *isB2bSystem
);


#ifdef IMPL_XBAR
/*
 * Typedef: FUNCP_GET_CORE_XBAR_CFG_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      coreCfg1Params  - Core xbar config parameters struct.
 */
typedef GT_STATUS (*FUNCP_GET_CORE_XBAR_CFG_PARAMS)
(
    IN  GT_U8                   boardRevId,
    OUT GT_XBAR_CORE_SYS_CFG    *xbarCfgParams
);


/*
 * Typedef: FUNCP_GET_XBAR_PHASE1_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      xbarIdx         - The Xbar device index to get the parameters for.
 *      xbarPhase1Params- Phase1 parameters struct.
 *
 */
typedef GT_STATUS (*FUNCP_GET_XBAR_PHASE1_PARAMS)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       xbarIdx,
    OUT GT_XBAR_PHASE1_INIT_PARAMS  *phase1Params
);

/*
 * Typedef: FUNCP_GET_XBAR_PHASE2_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      oldXbardevNum   - The old Xbar device number to get the parameters for.
 *      xbarPhase2Params- Phase2 parameters struct.
 *
 */
typedef GT_STATUS (*FUNCP_GET_XBAR_PHASE2_PARAMS)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       oldXbardevNum,
    OUT GT_XBAR_PHASE2_INIT_PARAMS  *phase2Params
);

#endif

#ifdef IMPL_FA
/*
 * Typedef: FUNCP_GET_FA_PHASE1_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      faIdx           - The Fa device index to get the parameters for.
 *      faPhase1Params  - Phase1 parameters struct.
 *
 */
typedef GT_STATUS (*FUNCP_GET_FA_PHASE1_PARAMS)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       faIdx,
    OUT GT_FA_PHASE1_INIT_PARAMS    *phase1Params
);

/*
 * Typedef: FUNCP_GET_FA_PHASE2_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      oldFaDevNum     - The old Fa device number to get the parameters for.
 *      faPhase2Params  - Phase2 parameters struct.
 *
 */
typedef GT_STATUS (*FUNCP_GET_FA_PHASE2_PARAMS)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       oldFaDevNum,
    OUT GT_FA_PHASE2_INIT_PARAMS    *phase2Params
);


/*
 * Typedef: FUNCP_GET_FA_REG_CFG_LIST
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      devNum          - The device number to get the parameters for.
 *      regCfgList      - A pointer to the register list.
 *      regCfgListSize  - Number of elements in regListPtr.
 *
 */
typedef GT_STATUS (*FUNCP_GET_FA_REG_CFG_LIST)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devNum,
    OUT CPSS_REG_VALUE_INFO_STC     **regCfgList,
    OUT GT_U32                      *regCfgListSize
);
#endif


/*
 * Typedef: FUNCP_BEFORE_PHASE1_CONFIG
 *
 * Description:  prototype for function that initialize "pre-phase 1"
 *               configuration
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *
 */
typedef GT_STATUS (*FUNCP_BEFORE_PHASE1_CONFIG)
(
    IN  GT_U8                       boardRevId
);

/*
 * Typedef: FUNCP_GET_PP_PHASE1_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      devIdx          - The Pp Index to get the parameters for.
 *      ppPhase1Params  - Phase1 parameters struct.
 *
 */
typedef GT_STATUS (*FUNCP_GET_PP_PHASE1_PARAMS)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devIdx,
    OUT CPSS_PP_PHASE1_INIT_PARAMS *phase1Params
);


/*
 * Typedef: FUNCP_AFTER_PHASE1_CONFIG
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *
 */
typedef GT_STATUS (*FUNCP_AFTER_PHASE1_CONFIG)
(
    IN  GT_U8   boardRevId
);


/*
 * Typedef: FUNCP_GET_PP_PHASE2_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      oldDevNum       - The old Pp device number to get the parameters for.
 *      ppPhase2Params  - Phase2 parameters struct.
 *
 */
typedef GT_STATUS (*FUNCP_GET_PP_PHASE2_PARAMS)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       oldDevNum,
    OUT CPSS_PP_PHASE2_INIT_PARAMS  *phase2Params
);


/*
 * Typedef: FUNCP_AFTER_PHASE2_CONFIG
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *
 */
typedef GT_STATUS (*FUNCP_AFTER_PHASE2_CONFIG)
(
    IN  GT_U8   boardRevId
);

/*
 * Typedef: FUNCP_GET_PP_LOGICAL_INIT_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      devNum          - The Pp device number to get the parameters for.
 *      ppLogInitParams - Pp logical init parameters struct.
 *
 */
typedef GT_STATUS (*FUNCP_GET_PP_LOGICAL_INIT_PARAMS)
(
    IN  GT_U8                      boardRevId,
    IN  GT_U8                      devNum,
    OUT CPSS_PP_CONFIG_INIT_STC   *ppLogInitParams
);


/*
 * Typedef: FUNCP_GET_LIB_INIT_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      devNum          - The Pp device number to get the parameters for.
 *      libInitParams   - Library initialization parameters.
 *
 */
typedef GT_STATUS (*FUNCP_GET_LIB_INIT_PARAMS)
(
    IN  GT_U8                        boardRevId,
    IN  GT_U8                        devNum,
    OUT APP_DEMO_LIB_INIT_PARAMS    *libInitParams
);


/*
 * Typedef: FUNCP_AFTER_INIT_CONFIG
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      libInitParams   - Library initialization parameters.
 *
 */
typedef GT_STATUS (*FUNCP_AFTER_INIT_CONFIG)
(
    IN  GT_U8                       boardRevId
);

/* The following 5 function pointer were added to support stack. */
/*
 * Typedef: FUNCP_GET_PP_REG_CFG_LIST
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      devNum          - The device number to get the parameters for.
 *      regCfgList      - A pointer to the register list.
 *      regCfgListSize  - Number of elements in regListPtr.
 *
 */
typedef GT_STATUS (*FUNCP_GET_PP_REG_CFG_LIST)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devNum,
    OUT CPSS_REG_VALUE_INFO_STC     **regCfgList,
    OUT GT_U32                      *regCfgListSize
);

/*
 * Typedef: FUNCP_GET_STACK_CONFIG_PARAMS
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      deviceIDsPtr    - List of the device IDs
 *      localUnitNum    - Local unit number
 *      stackParams     - Stack configuration parameters.
 *
 */
typedef GT_STATUS (*FUNCP_GET_STACK_CONFIG_PARAMS)
(
    IN  GT_U8                       boardRevId,
    IN  GT_U32                      *deviceIDsPtr,
    OUT GT_VOID                     *stackParams
);

/*
 * Typedef: FUNCP_GET_MNG_PORT_NAME
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *      mngPortName     - Management port name string.
 *
 */
typedef GT_STATUS (*FUNCP_GET_MNG_PORT_NAME)
(
    IN  GT_U8                       boardRevId,
    OUT GT_U8                       *mngPortName
);

/*
 * Typedef: FUNCP_AFTER_STACK_READY_CONFIG
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *
 */
typedef GT_STATUS (*FUNCP_AFTER_STACK_READY_CONFIG)
(
    IN  GT_U8                       boardRevId
);

/*
 * Typedef: FUNCP_SIMPLE_INIT
 *
 * Description:
 *
 * Fields:
 *      boardRevId      - The board revision Id.
 *
 */
typedef GT_STATUS (*FUNCP_SIMPLE_INIT)
(
    IN  GT_U8                       boardRevId
);

/*
 * Typedef: struct GT_BOARD_CONFIG_FUNCS
 *
 * Description:
 *      Includes board specific control functions.
 *
 * Fields:
 *      boardGetMemInfo             - Returns memory size for the board.
 *      boardGetInfo                - Returns general board information.
 *      boardGetXbarCfgParams       - Returns parameters for the xbarSysConfig()
 *                                    function.
 *      boardGetPpPh1Params         - Returns parameters for the
 *                                    corePpHwPhase1Init() function.
 *      boardGetFaPh1Params         - Returns parameters for the
 *                                    coreFaHwPhase1Init() function.
 *      boardGetXbarPh1Params       - Returns parameters for the
 *                                    coreXbarHwPhase1Init() function.
 *      boardAfterPhase1Config      - Board specific configurations that should
 *                                    be done after core phase1 initialization.
 *      boardGetPpPh2Params         - Returns parameters for the
 *                                    corePpHwPhase2Init() function.
 *      boardGetFaPh2Params         - Returns parameters for the
 *                                    coreFaHwPhase2Init() function.
 *      boardGetXbarPh2Params       - Returns parameters for the
 *                                    coreXbarHwPhase2Init() function.
 *      boardAfterPhase2Config      - Board specific configurations that should
 *                                    be done after core phase2 initialization.
 *      boardGetSysCfgParams        - Returns parameters for the sysConfig()
 *                                    function.
 *      boardGetPpLogInitParams     - Returns parameters for the
 *                                    sysPpLogicalInit() function.
 *      boardGetLibInitParams       - Returns parameters for Tapi libraries
 *                                    initialization.
 *      afterInitConfig             - Board specific configurations that should
 *                                    be done after board initialization.
 *
 *      boardGetPpRegCfgList        - The following pointers were added to
 *      boardGetFaRegCfgList          support stack.
 *      boardGetStackConfigParams
 *      boardGetMngPortName
 *      boardAfterStackReadyConfig
 *      boardCleanDbDuringSystemReset - clear the DB of the specific board config file ,
 *                                      as part of the 'system rest'
 *                                      to allow the 'cpssInitSystem' to run again
 *                                      as if it is the first time it runs
 *      boardSimpleInit          - Simplified initialization
 */
typedef struct
{
    FUNCP_GET_BOARD_MEMORY_INFO         boardGetMemInfo;
    FUNCP_GET_BOARD_INFO                boardGetInfo;
#ifdef IMPL_XBAR
    FUNCP_GET_CORE_XBAR_CFG_PARAMS      boardGetXbarCfgParams;
#endif

#if defined (IMPL_FA) || defined (IMPL_XBAR)
    FUNCP_GET_XBAR_PHASE1_PARAMS        boardGetXbarPh1Params;
    FUNCP_GET_XBAR_PHASE2_PARAMS        boardGetXbarPh2Params;
#endif
#ifdef IMPL_FA
    FUNCP_GET_FA_PHASE1_PARAMS          boardGetFaPh1Params;
    FUNCP_GET_FA_PHASE2_PARAMS          boardGetFaPh2Params;
    FUNCP_GET_FA_REG_CFG_LIST           boardGetFaRegCfgList;
#endif

    FUNCP_BEFORE_PHASE1_CONFIG          boardBeforePhase1Config;

    FUNCP_GET_PP_PHASE1_PARAMS          boardGetPpPh1Params;
    FUNCP_AFTER_PHASE1_CONFIG           boardAfterPhase1Config;
    FUNCP_GET_PP_PHASE2_PARAMS          boardGetPpPh2Params;
    FUNCP_AFTER_PHASE2_CONFIG           boardAfterPhase2Config;
    FUNCP_GET_PP_LOGICAL_INIT_PARAMS    boardGetPpLogInitParams;
    FUNCP_GET_LIB_INIT_PARAMS           boardGetLibInitParams;
    FUNCP_AFTER_INIT_CONFIG             boardAfterInitConfig;
    FUNCP_GET_PP_REG_CFG_LIST           boardGetPpRegCfgList;
    FUNCP_GET_STACK_CONFIG_PARAMS       boardGetStackConfigParams;
    FUNCP_GET_MNG_PORT_NAME             boardGetMngPortName;
    FUNCP_AFTER_STACK_READY_CONFIG      boardAfterStackReadyConfig;

    FUNCP_BOARD_CLEAN_DB_DURING_SYSTEM_RESET boardCleanDbDuringSystemReset;
    FUNCP_SIMPLE_INIT                   boardSimpleInit;
}GT_BOARD_CONFIG_FUNCS;


/*
 * Typedef: FUNCP_REGISTER_BOARD_FUNCS
 *
 * Description:
 *      Register board initialization functions.
 *
 * Fields:
 *
 */
typedef GT_STATUS (*FUNCP_REGISTER_BOARD_FUNCS)
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardFuncs
);

/* number of different initializations for the specific board */
#define GT_BOARD_LIST_NUM_OF_BORAD_INITS_CNS   40
/* number of characters in name of board / name of revision */
#define GT_BOARD_LIST_STRING_LEN_CNS            100
/* string for revision that not used */
#define GT_BOARD_LIST_NOT_USE_REVISION_STR_CNS      "not used"

/*
 * Typedef: struct GT_BOARD_LIST_ELEMENT
 *
 * Description:
 *      Holds a board's information for the show function and registration.
 *
 * Fields:
 *      registerFunc    - Registration function pointer.
 *      boardName       - Board's name.
 *
 *
 */
typedef struct
{
    FUNCP_REGISTER_BOARD_FUNCS  registerFunc;
    GT_CHAR                     boardName[GT_BOARD_LIST_STRING_LEN_CNS];
    GT_U8                       numOfRevs;
    GT_CHAR                     boardRevs[GT_BOARD_LIST_NUM_OF_BORAD_INITS_CNS][GT_BOARD_LIST_STRING_LEN_CNS];
}GT_BOARD_LIST_ELEMENT;

/*
 * typedef: enum  APP_DEMO_CPSS_PP_ACCESS_TYPE_ENT
 *
 * Description: PP access type enumeration
 *
 * Fields:
 *      APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E  - PP access type is read.
 *      APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E - PP access type is write
 *      APP_DEMO_TRACE_HW_ACCESS_TYPE_BOTH_E  - PP access type is read and write
 *      APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_DELAY_E - PP access type is write and delay
 *      APP_DEMO_TRACE_HW_ACCESS_TYPE_ALL_E - PP access type is read,write and delay
 */
typedef enum
{
    APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E,
    APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E,
    APP_DEMO_TRACE_HW_ACCESS_TYPE_BOTH_E,
    APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_DELAY_E,
    APP_DEMO_TRACE_HW_ACCESS_TYPE_ALL_E

}APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT;

/*
 * typedef: enum  APP_DEMO_TRACE_OUTPUT_MODE_ENT
 *
 * Description: PP access type enumeration
 *
 * Fields:
 *      APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_E         - use osPrintf.
 *      APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_SYNC_E    - use osPrintSynch need fo ISR debug
 *      APP_DEMO_TRACE_OUTPUT_MODE_DB_E             - store the data in db
 *      APP_DEMO_TRACE_OUTPUT_MODE_FILE_E           - store the data in file
 *      APP_DEMO_TRACE_OUTPUT_MODE_LAST_E           - the last parameter
 */

typedef enum
{
    APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_E,
    APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_SYNC_E,
    APP_DEMO_TRACE_OUTPUT_MODE_DB_E,
    APP_DEMO_TRACE_OUTPUT_MODE_FILE_E,
    APP_DEMO_TRACE_OUTPUT_MODE_LAST_E
}APP_DEMO_TRACE_OUTPUT_MODE_ENT;


/*
 * Typedef: enum APP_DEMO_CPSS_LOG_TASK_MODE_ENT
 *
 * Description:
 *      Determines if to log all CPSS API calls or just calls by  a specified task(s).
 *
 * Enumerations:
 *     APP_DEMO_CPSS_LOG_TASK_ALL_E    - log all threads API calls
 *     APP_DEMO_CPSS_LOG_TASK_SINGLE_E - log single task API calls
 *     APP_DEMO_CPSS_LOG_TASK_REF_E    - log task(s) whose id is read from
 *                                         some specified address
 *
 *
 *  Comments:
 *      None.
 *
 */

typedef enum {
    APP_DEMO_CPSS_LOG_TASK_ALL_E,
    APP_DEMO_CPSS_LOG_TASK_SINGLE_E,
    APP_DEMO_CPSS_LOG_TASK_REF_E
}APP_DEMO_CPSS_LOG_TASK_MODE_ENT;

/*******************************************************************************
* appDemoShowBoardsList
** DESCRIPTION:
*       This function displays the boards list which have an automatic
*       initialization support.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       The printed table should look like:
*       +-------------------------------+-----------------------+
*       | Board name                    | Revisions             |
*       +-------------------------------+-----------------------+
*       | 01 - RD-EX120-24G             |                       |
*       |                               | 01 - Rev 1.0          |
*       |                               | 02 - Rev 2.0          |
*       +-------------------------------+-----------------------+
*       | 02 - DB-MX610-48F4GS          |                       |
*       |                               | 01 - Rev 1.0          |
*       |                               | 02 - Rev 2.5          |
*       +-------------------------------+-----------------------+
*
*******************************************************************************/
GT_STATUS appDemoShowBoardsList
(
    GT_VOID
);


/*******************************************************************************
* cpssInitSystem
*
* DESCRIPTION:
*       This is the main board initialization function for CPSS driver.
*
* INPUTS:
*       boardIdx      - The index of the board to be initialized from the board
*                       list.
*       boardRevId    - Board revision Id.
*       reloadEeprom  - Whether the Eeprom should be reloaded when
*                       corePpHwStartInit() is called.
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
GT_STATUS cpssInitSystem
(
    IN  GT_U32  boardIdx,
    IN  GT_U32  boardRevId,
    IN  GT_U32  reloadEeprom
);

/*******************************************************************************
* appDemoTraceOn_GT_OK_Set
*
* DESCRIPTION:
*       This function configures 'print returning GT_OK' functions flag
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       - in unix-like systems special function needed to configure appDemoTraceOn_GT_OK
*           specific ifdef's needless, because this function can't harm anything
*       - must be external to ensure it's not ommited by optimization.
*******************************************************************************/
GT_VOID appDemoTraceOn_GT_OK_Set
(
    GT_U32 enable
);


/*******************************************************************************
* appDemoOnDistributedSimAsicSideSet
*
* DESCRIPTION:
*       set value to the appDemoOnDistributedSimAsicSide flag.
*
* INPUTS:
*       value  - value to be set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_STATE - if called after initialization of system -- after cpssInitSystem
*
* COMMENTS:
*       Function is called from the TERMINAL
*
*******************************************************************************/
GT_STATUS appDemoOnDistributedSimAsicSideSet
(
    IN GT_U32  value
);

/******************************************************************************
* appDemoTraceHwAccessEnable
*
* DESCRIPTION:
*       Trace HW read access information.
*
* INPUTS:
*       devNum      - PP device number
*       accessType  - access type: read or write
*       enable      - GT_TRUE: enable tracing for given access type
*                     GT_FALSE: disable tracing for given access type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on fail
*       GT_BAD_PARAM - on bad access type
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessEnable
(
    GT_U8                               devNum,
    APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT   accessType,
    GT_BOOL                             enable
);

/******************************************************************************
* appDemoTraceHwAccessOutputModeSet
*
* DESCRIPTION:
*       Set output tracing mode.
*
* INPUTS:
*       mode        - output tracing mode: print, printSync or store.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_STATE - on bad state
*       GT_BAD_PARAM - on bad mode
*
* COMMENTS:
*       It is not allowed to change mode, while one of the HW Access DB
*       is enabled.
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessOutputModeSet
(
    IN APP_DEMO_TRACE_OUTPUT_MODE_ENT   mode
);

/*******************************************************************************
* appDemoDbPpDump
*
* DESCRIPTION:
*       Dump App Demo DB
*
* INPUTS:
*       none.
* OUTPUTS:
*       none.
* RETURNS:
*       GT_OK   - GT_OK ant case
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDbPpDump(void);

/******************************************************************************
* appDemoDebugDeviceIdSet
*
* DESCRIPTION:
*       Set given device ID value for specific device.
*       Should be called before cpssInitSystem().
*
* INPUTS:
*       devIndex - index of the device in the Application DB (0..127)
*       deviceId - device type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PTR - on NULL pointer
*       GT_OUT_OF_RANGE - on devIndex > 127
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDebugDeviceIdSet
(
    IN GT_U8    devNum,
    IN GT_U32   deviceId
);

/******************************************************************************
* appDemoTrafficDisableSet
*
* DESCRIPTION:
*       Set traffic enable/disable mode - API that will enable/disable
*       execution of traffic enable algorithm during cpssInitSystem
*
* INPUTS:
*       disable    - GT_TRUE - do not enable traffic during cpssInitSystem
*                  - GT_FALSE - enable traffic during cpssInitSystem
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
******************************************************************************/
GT_VOID appDemoTrafficDisableSet
(
    IN GT_BOOL disable
);

/******************************************************************************
* appDemoTraceHwAccessDbLine
*
* DESCRIPTION:
*       returns a line of the trace database, either the read or write database,
*       delay uses the write database
*
* INPUTS:
*       index    - index of the line to retreive
*       accessType - access type, either read, write or write and delay
*
* OUTPUTS:
*       dbLine - A non-null pointer to a APP_DEMO_CPSS_HW_ACCESS_DB_STC struct
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessDbLine
(
    GT_U32 index,
    OUT APP_DEMO_CPSS_HW_ACCESS_DB_STC * dbLine
);

/******************************************************************************
* appDemoTraceHwAccessClearDb
*
* DESCRIPTION:
*       Clear HW access db
*
* INPUTS:
*      none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessClearDb
(
    GT_VOID
);

/******************************************************************************
* appDemoTraceHwAccessDbIsCorrupted
*
* DESCRIPTION:
*       Check if the HW access data base has reached the limit
*
* INPUTS:
*      none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/

GT_STATUS appDemoTraceHwAccessDbIsCorrupted
(
    GT_BOOL * corrupted
);



/******************************************************************************
* appDemoSupportAaMessageSet
*
* DESCRIPTION:
*       Configure the appDemo to support/not support the AA messages to CPU
*
* INPUTS:
*      supportAaMessage -
*                      GT_FALSE - not support processing of AA messages
*                      GT_TRUE - support processing of AA messages
*
* OUTPUTS:
*       none
*
* RETURNS:
*       nothing
*
* COMMENTS:
*
*
******************************************************************************/
GT_VOID appDemoSupportAaMessageSet
(
    IN GT_BOOL supportAaMessage
);

/******************************************************************************
* cpssResetSystem
*
* DESCRIPTION:
*       API performs CPSS reset that includes device remove, mask interrupts,
*       queues and tasks destroy, all static variables/arrays clean.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/
GT_STATUS cpssResetSystem(void);

/******************************************************************************
* cpssDevSupportSystemResetSet
*
* DESCRIPTION:
*       API controls if SYSTEM reset is supported, it is used to disable the
*       support on devices that do support. vice versa is not possible
*
* INPUTS:
*       IN  GT_U8       devNum
*       IN  GT_BOOL     enable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/
GT_STATUS cpssDevSupportSystemResetSet(

    IN  GT_U8       devNum,
    IN  GT_BOOL     enable

);

/*******************************************************************************
* appDemoDevIdxGet
*
* DESCRIPTION:
*       Find device index in "appDemoPpConfigList" array based on device SW number.
*
* INPUTS:
*       devNum - SW device number
*
* OUTPUTS:
*       devIdxPtr - (pointer to) device index in array "appDemoPpConfigList".
*
* RETURNS:
*       GT_OK   - on success, match found for devNum.
*       GT_FAIL - no match found for devNum.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDevIdxGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *devIdxPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoBoardConfigh */


