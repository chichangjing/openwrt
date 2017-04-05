/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoExMxPmSysConfig.c
*
* DESCRIPTION:
*       System configuration and initialization control for Puma devices.
*
* FILE REVISION NUMBER:
*
*       $Revision: 44 $
*
*******************************************************************************/
#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamConfig.h>

#include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmHwInit.h>
#include <cpss/exMxPm/exMxPmGen/config/cpssExMxPmCfg.h>

#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricConfig.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricVoq.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricXbar.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricHGLink.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricSerdes.h>
#include <cpss/exMxPm/exMxPmGen/inlif/cpssExMxPmInlif.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpCtrl.h>
#include <cpss/exMxPm/exMxPmGen/extMemory/cpssExMxPmExternalMemory.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <cpss/exMxPm/exMxPmGen/policer/cpssExMxPmPolicer.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
#include <cpss/exMxPm/exMxPmGen/diag/cpssExMxPmDiag.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/
extern GT_STATUS wrapCpssExMxPmBrgAuqMutexCreate(void);

static CPSS_PP_PORT_GROUP_INIT_INFO_STC portGroupsPhase1InfoArr[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS];

CPSS_TBD_BOOKMARK /* need to implement function that return this !!! */
#define DUMMY_IMPLEMENTATION    GT_OK
/*******************************************************************************
 * Local usage variables
 ******************************************************************************/

static FUNCP_CPSS_MODULE_INIT cpssInitList[20]; /* Defined at the end of the file. */

GT_STATUS appDemoConvert2ExMxPmPhase1Info
(
    IN  GT_U8                              devNum,
    IN  CPSS_PP_PHASE1_INIT_PARAMS         *appCpssPpPhase1ParamsPtr,
    OUT CPSS_EXMXPM_PP_PHASE1_INIT_INFO_STC  *cpssPpPhase1InfoPtr
);

GT_STATUS appDemoExMxPmInlifInit
(
    IN  GT_U8                       devNum,
    IN  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParams
);

static GT_STATUS appDemoConvert2ExMxPmPhase2Info
(
    IN  CPSS_PP_PHASE2_INIT_PARAMS          *appCpssPpPhase2ParamsPtr,
    OUT CPSS_EXMXPM_PP_PHASE2_INIT_INFO_STC   *cpssPpPhase2InfoPtr
);

static GT_VOID appDemoCoreToCpssExMxPmPpInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC       *sysConfPtr,
    OUT CPSS_EXMXPM_PP_CONFIG_INIT_STC  *ppLogicaslInfoPtr
);

/*******************************************************************************
 * Public API implementation
 ******************************************************************************/

/*******************************************************************************
* appDemoExMxPmHwPpAfterPhase1Init
*
* DESCRIPTION:
*       Implements HW after phase 1 initialization.
*
* INPUTS:
*      dev                     -   temp device id, could be changed later
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success,
*       GT_BAD_PTR  -   on bad pointers
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmHwPpAfterPhase1Init
(
    IN  GT_U8                        dev
)
{
    CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_ECC_EMULATION_MODE_ENT       readWriteMode;
    CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_ECC_EMULATION_LINES_MODE_ENT linesMode;
    GT_U32  value;
    GT_STATUS rc;

    if((appDemoDbEntryGet("csuEccEmulation.supported", &value) == GT_OK) && value)
    {
        if(appDemoDbEntryGet("csuEccEmulation.readWriteMode", &value) == GT_OK)
        {
            readWriteMode = value;
        }
        else
        {
            /*default*/
            readWriteMode = CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_ECC_EMULATION_MODE_WRITE_ONLY_E;
        }

        if(appDemoDbEntryGet("csuEccEmulation.linesMode", &value) == GT_OK)
        {
            linesMode = value;
        }
        else
        {
            /*default*/
            linesMode = CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_ECC_EMULATION_MULTI_LINES_ONLY_MODE_E;
        }

        rc = cpssExMxPmExternalMemoryCsuEccEmulationModeSet(dev,readWriteMode,linesMode);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmExternalMemoryCsuEccEmulationModeSet", rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmHwPpPhase1Init
*
* DESCRIPTION:
*       Implements HW phase 1 initialization.
*
* INPUTS:
*      dev                     -   temp device id, could be changed later
*      coreSysCfgPh1ParamsPtr  -   phase 1 core specific parameters
*      corePpPhase1ParamsPtr   -   PP phase 1 init parameters
*
* OUTPUTS:
*      ppDeviceTypePtr         -   output correct device type
*
* RETURNS:
*       GT_OK       - on success,
*       GT_BAD_PTR  -   on bad pointers
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmHwPpPhase1Init
(
    IN  GT_U8                        dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS  *appCpssPpPhase1ParamsPtr,
    OUT CPSS_PP_DEVICE_TYPE         *ppDevTypePtr
)
{
    GT_STATUS                             rc;
    CPSS_EXMXPM_PP_PHASE1_INIT_INFO_STC   cpssPpPhase1Params;     /* CPSS phase 1 PP params */
    GT_U32                                value;

    if ((NULL == appCpssPpPhase1ParamsPtr) ||
        (NULL == ppDevTypePtr))
    {
        return GT_BAD_PTR;
    }

    /* create the mutex of AUQ */
    wrapCpssExMxPmBrgAuqMutexCreate();

    appDemoPpConfigList[dev].externalTcamUsed = appCpssPpPhase1ParamsPtr->externalTcamUsed;

    /* fill CPSS Phase1 init parameters */
    rc = appDemoConvert2ExMxPmPhase1Info(dev,
                                       appCpssPpPhase1ParamsPtr,
                                       &cpssPpPhase1Params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoConvert2ExMxPmPhase1Info", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

        /* Init cpss with no HW writes */
    if((appDemoDbEntryGet("initRegDefaults", &value) == GT_OK) && (value != 0))
    {
                rc = cpssExMxPmDiagRegDefaultsEnableSet(GT_TRUE);
                if (GT_OK != rc)
            {
                return rc;
            }
    }

    rc = cpssExMxPmHwPpPhase1Init(&cpssPpPhase1Params, ppDevTypePtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmHwPpPhase1Init", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoExMxPmHwPpAfterPhase1Init(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmHwPpAfterPhase1Init", rc);
    return rc;
} /* appDemoExMxPmHwPpPhase1Init */

/*******************************************************************************
* appDemoExMxPmHwPpStartInit
*
* DESCRIPTION:
*       Implements PP start initialization.
*
* INPUTS:
*      dev             -   device number
*      fReloadEeprom   -   reload Eeprom flag
*      initDataListPtr -   list of registers to init
*      initDataListLen -   length of registers' list
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
GT_STATUS appDemoExMxPmHwPpStartInit
(
    IN  GT_U8                       dev,
    IN  GT_BOOL                     fReloadEeprom,
    IN  CPSS_REG_VALUE_INFO_STC     *initDataListPtr,
    IN  GT_U32                      initDataListLen
)
{
    GT_STATUS rc;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    rc = cpssExMxPmHwPpStartInit(dev,
                                 fReloadEeprom,
                                 initDataListPtr,
                                 initDataListLen);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmHwPpStartInit", rc);
    return rc;

} /* appDemoExMxPmHwPpStartInit */

/*******************************************************************************
* appDemoExMxPmHwPpPhase2Init
*
* DESCRIPTION:
*       Implements HW phase 2 initialization.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*      oldDev                  -   old device id, used for phase 1
*      coreSysCfgPh2ParamsPtr  -   phase 2 core specific parameters
*      corePpPhase2ParamsPtr   -   PP phase 2 init parameters
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  -   on bad pointer
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmHwPpPhase2Init
(
    IN  GT_U8                       oldDev,
    IN  CPSS_PP_PHASE2_INIT_PARAMS  *appCpssPpPhase2ParamsPtr
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_EXMXPM_PP_PHASE2_INIT_INFO_STC   cpssPpPhase2Info;       /* Cpss phase 2 info */


    if (NULL == appCpssPpPhase2ParamsPtr)
    {
        return GT_BAD_PTR;
    }

    /* Fill CPSS phase2 init structure */
    rc = appDemoConvert2ExMxPmPhase2Info(appCpssPpPhase2ParamsPtr,
                                       &cpssPpPhase2Info);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoConvert2ExMxPmPhase2Info", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssExMxPmHwPpPhase2Init(oldDev, &cpssPpPhase2Info);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmHwPpPhase2Init", rc);
    return rc;
} /* appDemoExMxPmHwPpPhase2Init */

CPSS_EXMXPM_PP_CONFIG_INIT_STC     copyPpLogicaslInfo;

/*******************************************************************************
* appDemoExMxPmPpLogicalInit
*
* DESCRIPTION:
*      Implements logical initialization for specific device.
*      Device number is a part of system configuration parameters.
*
* INPUTS:
*      devNum               -   device number
*      logicalConfigParams  -   PP logical configuration parameters
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmPpLogicalInit
(
    IN  GT_U8                        devNum,
    IN  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParams
)
{
    GT_STATUS   rc;
    CPSS_EXMXPM_PP_CONFIG_INIT_STC     ppLogicaslInfo;     /* CPSS format - ExMxPm info  */

    /* init external TCAM management database in case external TCAM is used */
    if( appDemoPpConfigList[devNum].externalTcamUsed == GT_TRUE )
    {
        rc = appDemoExMxPmExternalTcamMngInit(devNum, logicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmExternalTcamMngInit", rc);
        if(rc != GT_OK)
            return rc;
    }

    /* convert core format to specific CPSS format -- ExMxPm */
    appDemoCoreToCpssExMxPmPpInfoConvert(logicalConfigParams,&ppLogicaslInfo);

    /* call CPSS to do Pp logical init */
    rc = cpssExMxPmCfgPpLogicalInit(devNum, &ppLogicaslInfo);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmCfgPpLogicalInit", rc);
    if(rc != GT_OK)
        return rc;

    cpssOsMemCpy(&copyPpLogicaslInfo, &ppLogicaslInfo, sizeof(ppLogicaslInfo));

    /* init external TCAM and ETC in case external TCAM is used */
    if( appDemoPpConfigList[devNum].externalTcamUsed == GT_TRUE )
    {
        rc = appDemoExMxPmExternalTcamConfig(devNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmExternalTcamConfig", rc);
        if(rc != GT_OK)
            return rc;

        rc = appDemoExMxPmExternalTcamControllerConfig(devNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmExternalTcamControllerConfig", rc);
        if(rc != GT_OK)
            return rc;

        rc = appDemoExMxPmExternalTcamPclConfig(devNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmExternalTcamControllerConfig", rc);
        if(rc != GT_OK)
            return rc;
    }

    /* init inlif port mode per port/ per vlan */
    rc = appDemoExMxPmInlifInit(devNum, logicalConfigParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmInlifInit", rc);
    return rc;
} /* appDemoExMxPmPpLogicalInit */

/*******************************************************************************
* appDemoExMxPmLibrariesInit
*
* DESCRIPTION:
*      Initialize all CPSS libraries for specific device.
*      It's entry point where all required CPSS libraries are initialized.
*
* INPUTS:
*      dev                 -   device number
*      libInitParamsPtr    -   initialization parameters
*      sysConfigParamPtr   -   system configuration parameters
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
GT_STATUS appDemoExMxPmLibrariesInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParams,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParams
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       i;          /* Per function loop counter    */


    /* for given device id init all cpss modules */
    i = 0;
    while ((cpssInitList[i] != 0) && (GT_OK == rc))
    {
        /* for given device id init cpss module */
        rc = cpssInitList[i](dev, libInitParams, sysConfigParams);
        i++;
    }
    return rc;
} /* appDemoExMxPmLibrariesInit */

/*******************************************************************************
* appDemoExMxPmTrafficEnable
*
* DESCRIPTION:
*      Enable traffic for specific device.
*      Enable ports and device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmTrafficEnable
(
    IN  GT_U8   dev
)
{
    return DUMMY_IMPLEMENTATION;
} /* appDemoExMxPmTrafficEnable */

/*******************************************************************************
* appDemoExMxPmPpGeneralInit
*
* DESCRIPTION:
*      Implements general initialization for specific device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmPpGeneralInit
(
    IN  GT_U8   dev
)
{
    return DUMMY_IMPLEMENTATION;
} /* appDemoExMxPmPpGeneralInit */




/*******************************************************************************
 * Private function implementation
 ******************************************************************************/



/*******************************************************************************
* appDemoConvert2ExMxPmPhase1Info
*
* DESCRIPTION:
*       Set CPSS info from the core format.
*
* INPUTS:
*       devNum                      -   The PP's device number to be initialized.
*       appCpssPpPhase1ParamsPtr    -   PP phase 1 configuration parameters.
*
* OUTPUTS:
*       cpssPpPhase1InfoPtr - CPSS PP phase 1 configuration parameters.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on bad pointers
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function converts HW data into CPSS phase 1 format.
*
*******************************************************************************/
GT_STATUS appDemoConvert2ExMxPmPhase1Info
(
    IN  GT_U8                               devNum,
    IN  CPSS_PP_PHASE1_INIT_PARAMS          *appCpssPpPhase1ParamsPtr,
    OUT CPSS_EXMXPM_PP_PHASE1_INIT_INFO_STC *cpssPpPhase1InfoPtr
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      i,ii;

    if ((NULL == appCpssPpPhase1ParamsPtr) ||
        (NULL == cpssPpPhase1InfoPtr))
    {
        return GT_BAD_PTR;
    }

    osMemSet(cpssPpPhase1InfoPtr, 0, sizeof(*cpssPpPhase1InfoPtr));

    cpssPpPhase1InfoPtr->devNum          = devNum;
    cpssPpPhase1InfoPtr->busBaseAddr     = appCpssPpPhase1ParamsPtr->baseAddr;
    cpssPpPhase1InfoPtr->internalPciBase = appCpssPpPhase1ParamsPtr->internalPciBase;
    cpssPpPhase1InfoPtr->intVecNum       = appCpssPpPhase1ParamsPtr->intVecNum;
    cpssPpPhase1InfoPtr->intMask         = appCpssPpPhase1ParamsPtr->intMask;
    cpssPpPhase1InfoPtr->coreClock =
        (appCpssPpPhase1ParamsPtr->coreClk == APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS) ?
         CPSS_EXMXPM_AUTO_DETECT_CORE_CLOCK_CNS : appCpssPpPhase1ParamsPtr->coreClk;

    if(appCpssPpPhase1ParamsPtr->numOfPortGroups)
    {
        cpssPpPhase1InfoPtr->busBaseAddr = CPSS_PARAM_NOT_USED_CNS;

        cpssPpPhase1InfoPtr->numOfPortGroups = appCpssPpPhase1ParamsPtr->numOfPortGroups;
        if(appCpssPpPhase1ParamsPtr->numOfPortGroups > APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS)
        {
            return GT_BAD_STATE;
        }

        cpssPpPhase1InfoPtr->multiPortGroupsInfoPtr = portGroupsPhase1InfoArr;

        for(ii = 0 ; ii < appCpssPpPhase1ParamsPtr->numOfPortGroups ; ii++)
        {
            portGroupsPhase1InfoArr[ii].busBaseAddr = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].busBaseAddr;
            portGroupsPhase1InfoArr[ii].internalPciBase = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].internalPciBase;
            portGroupsPhase1InfoArr[ii].intVecNum = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].intVecNum;
            portGroupsPhase1InfoArr[ii].intMask = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].intMask;
        }
    }

    cpssPpPhase1InfoPtr->ingressBufferMemoryEnableScrambling =
        appCpssPpPhase1ParamsPtr->ingressBufferMemoryEnableScrambling;

    cpssPpPhase1InfoPtr->ingressBufferSramCfg.sramSize =
        appCpssPpPhase1ParamsPtr->ingressBufferSramCfg.sramSize;

    cpssPpPhase1InfoPtr->ingressBufferSize = appCpssPpPhase1ParamsPtr->ingressBufferSize;

    cpssPpPhase1InfoPtr->egressBufferMemoryEnableScrambling =
        appCpssPpPhase1ParamsPtr->egressBufferMemoryEnableScrambling;

    cpssPpPhase1InfoPtr->egressBufferRamType =
        appCpssPpPhase1ParamsPtr->egressBufferRamType;

    if( appCpssPpPhase1ParamsPtr->egressBufferRamType == APPDEMO_EXMXPM_RAM_TYPE_RLDRAM_E )
    {
        osMemCpy(&cpssPpPhase1InfoPtr->egressBufferRamCfg.rldramCfg,
                 &appCpssPpPhase1ParamsPtr->egressBufferRldramCfg,
                 sizeof(CPSS_RLDRAM_CFG_STC));
    }
    else /* appCpssPpPhase1ParamsPtr->egressBufferRamType == APPDEMO_EXMXPM_RAM_TYPE_SRAM_E */
    {
        cpssPpPhase1InfoPtr->egressBufferRamCfg.sramCfg.sramSize =
            appCpssPpPhase1ParamsPtr->egressBufferSramCfg.sramSize;
    }

    cpssPpPhase1InfoPtr->egressBufferSize = appCpssPpPhase1ParamsPtr->egressBufferSize;

    cpssPpPhase1InfoPtr->mngInterfaceType = appCpssPpPhase1ParamsPtr->mngInterfaceType;

    cpssPpPhase1InfoPtr->ppHAState        = appCpssPpPhase1ParamsPtr->sysHAState;

    cpssPpPhase1InfoPtr->powerDownPortsBmp = appCpssPpPhase1ParamsPtr->powerDownPortsBmp;

    /* in Puma3 copy external DRAM configuration*/
    cpssPpPhase1InfoPtr->extDramCfg.luDunitDramSize = appCpssPpPhase1ParamsPtr->extDramCfg.luDunitDramSize;
    cpssPpPhase1InfoPtr->extDramCfg.fwdDunitDramSize = appCpssPpPhase1ParamsPtr->extDramCfg.fwdDunitDramSize;

    cpssPpPhase1InfoPtr->extDramCfg.externalDramInUse = appCpssPpPhase1ParamsPtr->extDramCfg.externalDramInUse;
    switch(appCpssPpPhase1ParamsPtr->extDramCfg.dramLogicalMode)
    {
    case APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E:
        cpssPpPhase1InfoPtr->extDramCfg.dramBanksLogicalMode = CPSS_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E;
        break;
    case APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_4BANKS_E:
        cpssPpPhase1InfoPtr->extDramCfg.dramBanksLogicalMode = CPSS_EXMXPM_EXT_MEMORY_LOGICAL_MODE_4BANKS_E;
        break;
    default:
        return GT_BAD_PARAM;
    }
    for (i = 0; i < CPSS_EXMXPM_MAX_NUMBER_OF_EXTERNAL_DRAM_LU_DUNITS_CNS; i++)
    {
        cpssPpPhase1InfoPtr->extDramCfg.luDunitCfgArray[i].enable =
            appCpssPpPhase1ParamsPtr->extDramCfg.luDunitCfgArray[i].enable;
    }
    for (i = 0; i < CPSS_EXMXPM_MAX_NUMBER_OF_EXTERNAL_DRAM_FWD_DUNITS_CNS; i++)
    {
        cpssPpPhase1InfoPtr->extDramCfg.fwdDunitCfgArray[i].enable =
            appCpssPpPhase1ParamsPtr->extDramCfg.fwdDunitCfgArray[i].enable;
    }
    /* copy external LU DRAM static configuration */
    cpssPpPhase1InfoPtr->extDramCfg.luDramStaticNumOfCfg = appCpssPpPhase1ParamsPtr->extDramCfg.luDramStaticNumOfCfg;
    if(cpssPpPhase1InfoPtr->extDramCfg.luDramStaticNumOfCfg==0)
    {
        cpssPpPhase1InfoPtr->extDramCfg.luDramStaticParams=NULL;
    }
    else
    {
        cpssPpPhase1InfoPtr->extDramCfg.luDramStaticParams =
            cpssOsMalloc(sizeof(CPSS_EXMXPM_EXT_DRAM_STATIC_CFG) * cpssPpPhase1InfoPtr->extDramCfg.luDramStaticNumOfCfg);

        cpssOsMemCpy(cpssPpPhase1InfoPtr->extDramCfg.luDramStaticParams,
             appCpssPpPhase1ParamsPtr->extDramCfg.luDramStaticParams,
             sizeof(CPSS_EXMXPM_EXT_DRAM_STATIC_CFG) * cpssPpPhase1InfoPtr->extDramCfg.luDramStaticNumOfCfg);
    }

    /* copy external Forward DRAM static configuration */
    cpssPpPhase1InfoPtr->extDramCfg.fwdDramStaticNumOfCfg = appCpssPpPhase1ParamsPtr->extDramCfg.fwdDramStaticNumOfCfg;

    if(cpssPpPhase1InfoPtr->extDramCfg.fwdDramStaticNumOfCfg==0)
    {
        cpssPpPhase1InfoPtr->extDramCfg.fwdDramStaticParams=NULL;
    }
    else
    {
        cpssPpPhase1InfoPtr->extDramCfg.fwdDramStaticParams =
            cpssOsMalloc(sizeof(CPSS_EXMXPM_EXT_DRAM_STATIC_CFG) * cpssPpPhase1InfoPtr->extDramCfg.fwdDramStaticNumOfCfg);

        cpssOsMemCpy(cpssPpPhase1InfoPtr->extDramCfg.fwdDramStaticParams,
             appCpssPpPhase1ParamsPtr->extDramCfg.fwdDramStaticParams,
             sizeof(CPSS_EXMXPM_EXT_DRAM_STATIC_CFG) * cpssPpPhase1InfoPtr->extDramCfg.fwdDramStaticNumOfCfg);
    }

    cpssPpPhase1InfoPtr->controlSramUsed = appCpssPpPhase1ParamsPtr->controlSramUsedArray[0];
    cpssPpPhase1InfoPtr->controlSramCfg.sramSize = appCpssPpPhase1ParamsPtr->controlSramCfgArray[0].sramSize;
    cpssPpPhase1InfoPtr->controlSramProtect = appCpssPpPhase1ParamsPtr->controlSramProtectArray[0];
    cpssPpPhase1InfoPtr->externalTcamUsed = appCpssPpPhase1ParamsPtr->externalTcamUsed;

    switch (appCpssPpPhase1ParamsPtr->routingSramCfgType)
    {
        /* CPSS_TWIST_INTERNAL_E is mapped to CPSS_EXMXPM_LPM_ALL_INTERNAL_E */
    case CPSS_TWIST_INTERNAL_E:
        cpssPpPhase1InfoPtr->lpmSramCfgType = CPSS_EXMXPM_LPM_ALL_INTERNAL_E;
        break;

        /* CPSS_TWIST_EXTERNAL_E is mapped to CPSS_EXMXPM_LPM_ALL_EXTERNAL_E */
    case CPSS_TWIST_EXTERNAL_E:
        for (i = 0 ; i < appCpssPpPhase1ParamsPtr->numOfSramsCfg ; i++)
        {
            cpssPpPhase1InfoPtr->lpmSramUsedArray[i] = GT_TRUE;
            cpssPpPhase1InfoPtr->lpmSramCfgArray[i].sramSize       = appCpssPpPhase1ParamsPtr->sramsCfg[i].sramSize;
        }
        cpssPpPhase1InfoPtr->lpmSramCfgType = CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
        break;
    case CPSS_ONE_INTERNAL_E:
        cpssPpPhase1InfoPtr->lpmSramCfgType = CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E;
        break;
    case CPSS_TWO_INTERNAL_E:
        cpssPpPhase1InfoPtr->lpmSramCfgType = CPSS_EXMXPM_LPM_DOUBLE_INTERNAL_E;
        break;
    default:
        cpssPpPhase1InfoPtr->lpmSramCfgType = CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E;
        break;
    }
    return rc;
} /* appDemoConvert2ExMxPmPhase1Info */

/*******************************************************************************
* appDemoConvert2ExMxPmPhase2Info
*
* DESCRIPTION:
*       Convert core phase 2 info to CPSS phase 2 info.
*
* INPUTS:
*       appCpssPpPhase2ParamsPtr    -   PP phase 2 configuration parameters.
*
* OUTPUTS:
*       cpssPpPhase2InfoPtr         - CPSS PP phase 2 configuration parameters.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on bad pointers
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function converts HW data into CPSS phase 2 format.
*
*******************************************************************************/
static GT_STATUS appDemoConvert2ExMxPmPhase2Info
(
    IN  CPSS_PP_PHASE2_INIT_PARAMS          *appCpssPpPhase2ParamsPtr,
    OUT CPSS_EXMXPM_PP_PHASE2_INIT_INFO_STC   *cpssPpPhase2InfoPtr
)
{
    GT_STATUS               rc = GT_OK;
    CPSS_RX_BUF_INFO_STC    *cpssRxBufInfoPtr;  /* pointer to cpss rx buf format    */
    CPSS_RX_BUF_INFO_STC    *pssRxBufInfoPtr;   /* pointer to cpss rx buf format     */


    if ((NULL == appCpssPpPhase2ParamsPtr) ||
        (NULL == cpssPpPhase2InfoPtr))
    {
        return GT_BAD_PTR;
    }

    osMemSet(cpssPpPhase2InfoPtr, 0, sizeof(*cpssPpPhase2InfoPtr));

    cpssPpPhase2InfoPtr->newDevNum                  = appCpssPpPhase2ParamsPtr->devNum;
    cpssPpPhase2InfoPtr->netIfCfg.txDescBlock       = appCpssPpPhase2ParamsPtr->netIfCfg.txDescBlock;
    cpssPpPhase2InfoPtr->netIfCfg.txDescBlockSize   = appCpssPpPhase2ParamsPtr->netIfCfg.txDescBlockSize;
    cpssPpPhase2InfoPtr->netIfCfg.rxDescBlock       = appCpssPpPhase2ParamsPtr->netIfCfg.rxDescBlock;
    cpssPpPhase2InfoPtr->netIfCfg.rxDescBlockSize   = appCpssPpPhase2ParamsPtr->netIfCfg.rxDescBlockSize;

    cpssRxBufInfoPtr    = &cpssPpPhase2InfoPtr->netIfCfg.rxBufInfo;
    pssRxBufInfoPtr     = &appCpssPpPhase2ParamsPtr->netIfCfg.rxBufInfo;

    cpssRxBufInfoPtr->allocMethod = pssRxBufInfoPtr->allocMethod;
    osMemCpy(cpssRxBufInfoPtr->bufferPercentage,
             pssRxBufInfoPtr->bufferPercentage,
             sizeof(pssRxBufInfoPtr->bufferPercentage));

    cpssRxBufInfoPtr->rxBufSize     = appCpssPpPhase2ParamsPtr->rxBufSize;
    cpssRxBufInfoPtr->headerOffset  = appCpssPpPhase2ParamsPtr->headerOffset;

    if (CPSS_RX_BUFF_DYNAMIC_ALLOC_E == cpssRxBufInfoPtr->allocMethod)
    {
        cpssRxBufInfoPtr->buffData.dynamicAlloc.mallocFunc =
            (GT_VOID*)pssRxBufInfoPtr->buffData.dynamicAlloc.mallocFunc;

        cpssRxBufInfoPtr->buffData.dynamicAlloc.numOfRxBuffers =
            pssRxBufInfoPtr->buffData.dynamicAlloc.numOfRxBuffers;
    }
    else if (CPSS_RX_BUFF_STATIC_ALLOC_E == cpssRxBufInfoPtr->allocMethod)
    {
        cpssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr =
            pssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr;
        cpssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize =
            pssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize;
    }

    cpssPpPhase2InfoPtr->auqCfg.auDescBlock     = appCpssPpPhase2ParamsPtr->auqCfg.auDescBlock;
    cpssPpPhase2InfoPtr->auqCfg.auDescBlockSize = appCpssPpPhase2ParamsPtr->auqCfg.auDescBlockSize;

    cpssPpPhase2InfoPtr->separateFuqUsed         = appCpssPpPhase2ParamsPtr->fuqUseSeparate;

    osMemCpy(&cpssPpPhase2InfoPtr->fuqCfg,
             &appCpssPpPhase2ParamsPtr->fuqCfg,
             sizeof(appCpssPpPhase2ParamsPtr->fuqCfg));

    return rc;
} /* appDemoConvert2ExMxPmPhase2Info */

/*******************************************************************************
* appDemoCoreToCpssExMxPmPpInfoConvert
*
* DESCRIPTION:
*       convert application format to CPSS format for ExMxPm "PP logical init"
*
* INPUTS:
*       ppConfPtr        - The PP logical configuration parameters
*
* OUTPUTS:
*      ppLogicaslInfoPtr - CPSS format for ExMX device.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID appDemoCoreToCpssExMxPmPpInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC         *ppConfigPtr,
    OUT CPSS_EXMXPM_PP_CONFIG_INIT_STC  *ppLogicalInfoPtr
)
{
    GT_U32 i=0;

    ppLogicalInfoPtr->fdbTableInfo.fdbTableMode  = ppConfigPtr->fdbTableInfo.fdbTableMode;
    ppLogicalInfoPtr->fdbTableInfo.fdbToLutRatio = ppConfigPtr->fdbTableInfo.fdbToLutRatio;
    ppLogicalInfoPtr->fdbTableInfo.fdbHashSize   = ppConfigPtr->fdbTableInfo.fdbHashSize;
    ppLogicalInfoPtr->fdbTableInfo.extFdbHashMode = ppConfigPtr->fdbTableInfo.extFdbHashMode;

    /* IP info */
    ppLogicalInfoPtr->ipInitInfo.routeEntriesNum = ppConfigPtr->maxNumOfIpNextHop;
    ppLogicalInfoPtr->ipInitInfo.extLpmMemSize = ppConfigPtr->extLpmMemSize;

    ppLogicalInfoPtr->ipInitInfo.useLpmInternalSram = ppConfigPtr->lpmDbMemCfgArray->useLpmInternalSram;

    for (i=0; i < CPSS_MAX_NUMBER_LPM_INTERNAL_SRAMS_CNS; i++)
    {
        ppLogicalInfoPtr->ipInitInfo.lpmInternalSramSizeArray[i] = ppConfigPtr->lpmDbMemCfgArray->lpmInternalSramSizeArray[i];
    }

    switch (ppConfigPtr->lpmStagesMemAssociation)
    {
    case APP_DEMO_CPSS_LPM_STAGES_MEM_ASSOCIATION_ALL_INTERNAL_E:
        ppLogicalInfoPtr->ipInitInfo.lpmStagesMemAssociation = CPSS_EXMXPM_LPM_STAGES_MEM_ASSOCIATION_ALL_INTERNAL_E;
        break;
    case APP_DEMO_CPSS_LPM_STAGES_MEM_ASSOCIATION_ALL_EXTERNAL_E:
        ppLogicalInfoPtr->ipInitInfo.lpmStagesMemAssociation = CPSS_EXMXPM_LPM_STAGES_MEM_ASSOCIATION_ALL_EXTERNAL_E;
        break;
    default:
        break;
    }

    /* MPLS info */
    ppLogicalInfoPtr->mplsInitInfo.nhlfeNum = ppConfigPtr->maxNumOfNhlfe;
    ppLogicalInfoPtr->mplsInitInfo.InterfaceEntriesNum = ppConfigPtr->maxNumOfMplsIfs;
    switch (ppConfigPtr->ipMplsMemLocation)
    {
    case APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E:
        ppLogicalInfoPtr->ipMplsMemLocation = CPSS_EXMXPM_PP_MEMORY_LOCATION_EXTERNAL_E;
        break;
    case APP_DEMO_CPSS_MEMORY_LOCATION_INTERNAL_E:
         ppLogicalInfoPtr->ipMplsMemLocation = CPSS_EXMXPM_PP_MEMORY_LOCATION_INTERNAL_E;
        break;
    default:
        break;
    }


    /* DIT info */
    switch (ppConfigPtr->ditMemLocation)
    {
    case APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E:
        ppLogicalInfoPtr->ditMemLocation = CPSS_EXMXPM_PP_MEMORY_LOCATION_EXTERNAL_E;
        break;
    case APP_DEMO_CPSS_MEMORY_LOCATION_INTERNAL_E:
         ppLogicalInfoPtr->ditMemLocation = CPSS_EXMXPM_PP_MEMORY_LOCATION_INTERNAL_E;
        break;
    default:
        break;
    }

    switch (ppConfigPtr->ditMemoryMode)
    {
    case APP_DEMO_CPSS_DIT_SHARED_MEM_E:
        ppLogicalInfoPtr->ditInitInfo.memoryMode = CPSS_EXMXPM_PP_CONFIG_DIT_SHARED_MEM_E;
        break;
    case APP_DEMO_CPSS_DIT_SEPARATE_MEM_E:
        ppLogicalInfoPtr->ditInitInfo.memoryMode = CPSS_EXMXPM_PP_CONFIG_DIT_SEPARATE_MEM_E;
        break;
    default:
        break;
    }

    switch (ppConfigPtr->ditIpMcMode)
    {
    case APP_DEMO_CPSS_DIT_IP_MC_DUAL_MODE_E:
        ppLogicalInfoPtr->ditInitInfo.ipMcMode = CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E;
        break;
    case APP_DEMO_CPSS_DIT_IP_MC_SINGLE_MODE_E:
        ppLogicalInfoPtr->ditInitInfo.ipMcMode = CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_SINGLE_MODE_E;
        break;
    default:
        break;
    }

    ppLogicalInfoPtr->ditInitInfo.ucIpEntriesNum    = ppConfigPtr->maxNumOfDitUcIpEntries;
    ppLogicalInfoPtr->ditInitInfo.mcIpEntriesNum    = ppConfigPtr->maxNumOfDitMcIpEntries;
    ppLogicalInfoPtr->ditInitInfo.ucMplsEntriesNum  = ppConfigPtr->maxNumOfDitUcMplsEntries;
    ppLogicalInfoPtr->ditInitInfo.mcMplsEntriesNum  = ppConfigPtr->maxNumOfDitMcMplsEntries;
    ppLogicalInfoPtr->ditInitInfo.ucOtherEntriesNum = ppConfigPtr->maxNumOfDitUcOtherEntries;
    ppLogicalInfoPtr->ditInitInfo.mcOtherEntriesNum = ppConfigPtr->maxNumOfDitMcOtherEntries;

    /* external TCAM info */
    ppLogicalInfoPtr->externalTcamInfo.extTcamActionLinesInMemoryControl0 =
                    ppConfigPtr->externalTcamInfo.actionLinesInMemoryControl0;
    ppLogicalInfoPtr->externalTcamInfo.extTcamActionLinesInMemoryControl1 =
                    ppConfigPtr->externalTcamInfo.actionLinesInMemoryControl1;
    ppLogicalInfoPtr->externalTcamInfo.extTcamPclBindInfo.extTcamRuleWriteFunc =
    ppLogicalInfoPtr->externalTcamInfo.extTcamTtiBindInfo.extTcamRuleWriteFunc =
                    ppConfigPtr->externalTcamInfo.extTcamRuleWriteFuncPtr;
    ppLogicalInfoPtr->externalTcamInfo.extTcamPclBindInfo.extTcamRuleReadFunc =
    ppLogicalInfoPtr->externalTcamInfo.extTcamTtiBindInfo.extTcamRuleReadFunc =
                    ppConfigPtr->externalTcamInfo.extTcamRuleReadFuncPtr;
    ppLogicalInfoPtr->externalTcamInfo.extTcamPclBindInfo.extTcamActionWriteFunc =
    ppLogicalInfoPtr->externalTcamInfo.extTcamTtiBindInfo.extTcamActionWriteFunc =
                    ppConfigPtr->externalTcamInfo.extTcamActionWriteFuncPtr;
    ppLogicalInfoPtr->externalTcamInfo.extTcamPclBindInfo.extTcamActionReadFunc =
    ppLogicalInfoPtr->externalTcamInfo.extTcamTtiBindInfo.extTcamActionReadFunc =
                    ppConfigPtr->externalTcamInfo.extTcamActionReadFuncPtr;

} /* appDemoCoreToCpssExMxPmPpInfoConvert */



/*******************************************************************************
 * CPSS modules initialization routines
 ******************************************************************************/

#ifdef IMPL_PORT

#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortStat.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortTx.h>

static GT_STATUS prvPortLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPort == GT_FALSE)
        return GT_OK;

    rc = cpssExMxPmPortStatInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortStatInit", rc);
    if( GT_OK != rc)
        return rc;

    rc = cpssExMxPmPortTxInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortTxInit", rc);
    if(rc != GT_OK)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_TRUNK
#include <cpss/exMxPm/exMxPmGen/trunk/cpssExMxPmTrunk.h>

static GT_STATUS prvTrunkLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initTrunk == GT_FALSE)
        return GT_OK;

    rc = cpssExMxPmTrunkInit(dev, sysConfigParamsPtr->numOfTrunks);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmTrunkInit", rc);
    return rc;
}
#endif

#ifdef IMPL_PHY
#include <cpss/exMxPm/exMxPmGen/phy/cpssExMxPmPhySmi.h>


static GT_STATUS prvPhyLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPhy == GT_FALSE)
        return GT_OK;

    rc = cpssExMxPmPhyPortSmiInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiInit", rc);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif



#ifdef IMPL_BRIDGE
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>

static GT_STATUS exMxPmBrgFdbInit
(
    IN  GT_U8                       dev
)
{
    GT_STATUS rc;

    cpssExMxPmBrgFdbInit(dev);

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    /* Set lookUp mode and lookup length. */
    rc = cpssExMxPmBrgFdbHashModeSet(dev, CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E); /* use CRC hash function */
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbHashModeSet", rc);
    if( GT_OK != rc)
        return rc;

    rc = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(dev, 4);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbNumOfEntriesInLookupSet", rc);
    if( GT_OK != rc)
        return rc;

    /******************************/
    /* do specific puma coding */
    /******************************/

    /* the trunk entries and the multicast entries are registered on device 31
       that is to support the "renumbering" feature , but the next configuration
       should not effect the behavior on other systems that not use a
       renumbering ..
    */
    /* age trunk entries on a device that registered from all device
       since we registered the trunk entries on device 31 (and auto learn set it
       on "own device" */
    /* Set Action Active Device Mask and Action Active Device. This is needed
       in order to enable aging only on own device.  */

    #if 0
    rc = cpssExMxPmBrgFdbActionActiveDevSet(dev, dev, 0x1F);
    if(rc != GT_OK)
        return rc;
    #endif

    rc = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet(dev, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet", rc);
    if(rc != GT_OK)
        return rc;

    return GT_OK;
}

static GT_STATUS prvBridgeLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    rc = exMxPmBrgFdbInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("exMxPmBrgFdbInit", rc);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_NETIF
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>

static GT_STATUS prvNetIfLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initNetworkIf == GT_FALSE)
        return GT_OK;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    rc = cpssExMxPmNetIfInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmNetIfInit", rc);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_IP
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTypes.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTables.h>
#include <cpss/exMxPm/exMxPmGen/ipLpmEngine/cpssExMxPmIpLpmTypes.h>
#include <cpss/exMxPm/exMxPmGen/ipLpmEngine/cpssExMxPmIpLpm.h>

static GT_STATUS prvIpLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS                           rc;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    cpssLpmDbConfig;
    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   ucRouteEntry;
    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   mcRouteEntry;
    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    vrConfig;
    GT_ETHERADDR                        arpIpAddr;
    GT_U32                              lpmDbId = 0;
    GT_U32                              vrId = 0;
    GT_U32                              i;

    static GT_BOOL lpmDbInitialized = GT_FALSE;     /* keeps if LPM DB was created */
    static GT_BOOL vrInitialized = GT_FALSE;        /* keeps if virtual router was created */

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    /*************************************************/
    /* create LPM DB if it wasn't previously created */
    /*************************************************/

    if (lpmDbInitialized == GT_FALSE)
    {
        /* set parameters */
        cpssLpmDbConfig.protocolStack       = CPSS_IP_PROTOCOL_IPV4V6_E;
        cpssLpmDbConfig.ipv6MemShare        = sysConfigParamsPtr->ipv6MemShare;
        cpssLpmDbConfig.numOfVirtualRouters = sysConfigParamsPtr->maxNumOfVirtualRouters;
        cpssLpmDbConfig.numOfMemCfg         = sysConfigParamsPtr->lpmDbNumOfMemCfg;
        cpssLpmDbConfig.memCfgArray         = osMalloc(sizeof(CPSS_EXMXPM_IP_MEM_CFG_STC) * cpssLpmDbConfig.numOfMemCfg);
        if(PRV_CPSS_PP_MAC(dev)->devFamily >= CPSS_PP_FAMILY_PUMA3_E)
        {
            cpssLpmDbConfig.shadowType = CPSS_EXMXPM_IP_LPM_PUMA3_SHADOW_E;
        }
        else
        {
             cpssLpmDbConfig.shadowType = CPSS_EXMXPM_IP_LPM_PUMA2_SHADOW_E;
        }
        if (cpssLpmDbConfig.memCfgArray == NULL)
            return GT_OUT_OF_CPU_MEM;
        for (i = 0 ; i < sysConfigParamsPtr->lpmDbNumOfMemCfg ; i++)
        {
                switch (sysConfigParamsPtr->lpmDbMemCfgArray[i].routingSramCfgType)
                {
                case APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E:
                    cpssLpmDbConfig.memCfgArray[i].routingSramCfgType = CPSS_EXMXPM_LPM_ALL_INTERNAL_E;
                    break;
                case APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E:
                    cpssLpmDbConfig.memCfgArray[i].routingSramCfgType = CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
                    break;
                case APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_EXTERNAL_E:
                    cpssLpmDbConfig.memCfgArray[i].routingSramCfgType = CPSS_EXMXPM_LPM_SINGLE_EXTERNAL_E;
                    break;
                case APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E:
                    if(PRV_CPSS_PP_MAC(dev)->devFamily >= CPSS_PP_FAMILY_PUMA3_E)
                    {
                        cpssLpmDbConfig.protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
                    }
                    cpssLpmDbConfig.memCfgArray[i].routingSramCfgType = CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E;
                    break;
                case APP_DEMO_CPSS_EXMXPM_LPM_DOUBLE_INTERNAL_E:
                    if(PRV_CPSS_PP_MAC(dev)->devFamily >= CPSS_PP_FAMILY_PUMA3_E)
                    {
                        cpssLpmDbConfig.protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
                    }
                    cpssLpmDbConfig.memCfgArray[i].routingSramCfgType = CPSS_EXMXPM_LPM_DOUBLE_INTERNAL_E;
                    break;
                case APP_DEMO_CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E:
                    cpssLpmDbConfig.memCfgArray[i].routingSramCfgType = CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E;
                    break;
                default:
                    return GT_BAD_PARAM;
                }

                switch(sysConfigParamsPtr->lpmDbMemCfgArray[i].extMemBankMode)
                {
                case APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_4BANKS_E:
                    cpssLpmDbConfig.memCfgArray[i].extMemBankMode = CPSS_EXMXPM_EXT_MEMORY_LOGICAL_MODE_4BANKS_E;
                    break;
                case APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E:
                    cpssLpmDbConfig.memCfgArray[i].extMemBankMode = CPSS_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E;
                    break;
                default:
                    return GT_BAD_PARAM;
                }
                cpssLpmDbConfig.memCfgArray[i].extMemSize = sysConfigParamsPtr->lpmDbMemCfgArray[i].extMemSize;

                cpssLpmDbConfig.memCfgArray[i].numOfSramsSizes =
                    sysConfigParamsPtr->lpmDbMemCfgArray[i].numOfSramsSizes;
                cpssOsMemCpy(&cpssLpmDbConfig.memCfgArray[i].sramsSizeArray,
                         &sysConfigParamsPtr->lpmDbMemCfgArray[i].sramsSizeArray,
                         sizeof(CPSS_SRAM_SIZE_ENT) * cpssLpmDbConfig.memCfgArray[i].numOfSramsSizes);

                cpssLpmDbConfig.memCfgArray[i].useLpmInternalSram = sysConfigParamsPtr->lpmDbMemCfgArray[i].useLpmInternalSram;

                cpssOsMemCpy(&cpssLpmDbConfig.memCfgArray[i].lpmInternalSramSizeArray,
                         &sysConfigParamsPtr->lpmDbMemCfgArray[i].lpmInternalSramSizeArray,
                         sizeof(GT_U32) * CPSS_MAX_NUMBER_LPM_INTERNAL_SRAMS_CNS);

                if((PRV_CPSS_PP_MAC(dev)->devFamily >= CPSS_PP_FAMILY_PUMA3_E)&&
                   (sysConfigParamsPtr->lpmDbMemCfgArray[i].useLpmInternalSram == GT_TRUE) &&
                   (sysConfigParamsPtr->lpmDbMemCfgArray[i].routingSramCfgType == APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E))
                {
                    /* need to remove mplsMemoryNeeded from lpm memory size.
                       for Puma3 all_internal mode MPLS is located in LPM3 .
                       Each mpls holds 2 lines of data (33 bit in each line).
                       in prvExMxPmlayer3CfgSet the mplsMemoryNeeded was
                       removed from lpmCfg.sramInfo[3].size meaning the value
                       in sysConfigParamsPtr->lpmDbMemCfgArray[i].lpmInternalSramSizeArray[3]
                       includes also MPLS size.
                       When creating the LPM DB for IP we should deduct the MPLS size from
                       cpssLpmDbConfig.memCfgArray[i].lpmInternalSramSizeArray[3].
                       Since the value in this array is in Klines and should be a multiple of 2,
                       we might have a case we deduct extra memory that is not really used
                       for MPLS and this memory will be lost.
                       for example when maxNumOfMplsIfs = 10 then it will hold 20 lines,
                       but we will deduct 2KLines from the
                       cpssLpmDbConfig.memCfgArray[i].lpmInternalSramSizeArray[3] .
                       This deduction must be made, since lpmDb sram size can not be bigger
                       then the size defined per device   */

                    /* each mpls hold 2 lines in the lpm memory --> need to multiply maxNumOfMplsIfs */
                    if((sysConfigParamsPtr->maxNumOfMplsIfs * 2) > (cpssLpmDbConfig.memCfgArray[i].lpmInternalSramSizeArray[3]*_1K))
                    {
                        return GT_BAD_PARAM;
                    }
                    else
                    {
                        cpssLpmDbConfig.memCfgArray[i].lpmInternalSramSizeArray[3] =
                            ((cpssLpmDbConfig.memCfgArray[i].lpmInternalSramSizeArray[3]*_1K) -
                            (sysConfigParamsPtr->maxNumOfMplsIfs * 2))/_1K;

                        if(cpssLpmDbConfig.memCfgArray[i].lpmInternalSramSizeArray[3]%2!=0)
                            cpssLpmDbConfig.memCfgArray[i].lpmInternalSramSizeArray[3]--;
                    }
                }
        }

        /* now create the lpm db */
        rc = cpssExMxPmIpLpmDbCreate(lpmDbId,&cpssLpmDbConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpLpmDbCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* mark the lpm db as created */
        lpmDbInitialized = GT_TRUE;
    }

    /*********************************************/
    /* Workaround:                               */
    /*  Although if not using tunnel start       */
    /*  in DIT - Hw reads ARP/Tunnel table entry */
    /*  thats why it must be initialized         */
    /*********************************************/
    osMemSet(&arpIpAddr,0,sizeof(GT_ETHERADDR));
    for (i = 0; i < 1/*024*/; i++)
    {
        rc = cpssExMxPmIpRouterArpAddrWrite(dev,i,&arpIpAddr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpRouterArpAddrWrite", rc);
        if (rc != GT_OK)
            return  rc;
    }

    /*********************************************/
    /* set route entry 0 as default ipv4 unicast */
    /*********************************************/
    osMemSet(&ucRouteEntry,0,sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC));
    ucRouteEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;
    ucRouteEntry.entry.regularEntry.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    rc = cpssExMxPmIpUcRouteEntriesWrite(dev,0,1,&ucRouteEntry);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpUcRouteEntriesWrite", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }
    /***********************************************/
    /* set route entry 1 as default ipv4 multicast */
    /***********************************************/
    osMemSet(&mcRouteEntry,0,sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC));
    mcRouteEntry.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    rc = cpssExMxPmIpMcRouteEntriesWrite(dev,1,1,&mcRouteEntry);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpMcRouteEntriesWrite", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /*********************************************/
    /* set route entry 2 as default ipv6 unicast */
    /*********************************************/
    osMemSet(&ucRouteEntry,0,sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC));
    ucRouteEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;
    ucRouteEntry.entry.regularEntry.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    rc = cpssExMxPmIpUcRouteEntriesWrite(dev,2,1,&ucRouteEntry);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpUcRouteEntriesWrite", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /***********************************************/
    /* set route entry 3 as default ipv6 multicast */
    /***********************************************/
    osMemSet(&mcRouteEntry,0,sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC));
    mcRouteEntry.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    rc = cpssExMxPmIpMcRouteEntriesWrite(dev,3,1,&mcRouteEntry);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpMcRouteEntriesWrite", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /*******************************/
    /* add active device to LPM DB */
    /*******************************/
    rc = cpssExMxPmIpLpmDbDevListAdd(lpmDbId,1,&dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpLpmDbDevListAdd", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }
    /*********************************************************/
    /* create virtual router if it wasn't previously created */
    /*********************************************************/
    if (vrInitialized == GT_FALSE)
    {
        vrConfig.supportUcIpv4 = GT_TRUE;
        vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 0;
        vrConfig.defaultUcIpv4RouteEntry.blockSize = 1;

        vrConfig.supportMcIpv4 = GT_TRUE;
        vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 1;
        vrConfig.defaultMcIpv4RouteEntry.blockSize = 1;

        vrConfig.supportUcIpv6 = GT_TRUE;
        vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 2;
        vrConfig.defaultUcIpv6RouteEntry.blockSize = 1;

        vrConfig.supportMcIpv6 = GT_TRUE;
        vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 3;
        vrConfig.defaultMcIpv6RouteEntry.blockSize = 1;

        vrConfig.defaultMcIpv6RuleIndex = 8191;
        vrConfig.defaultMcIpv6PclId = 1023;
        rc = cpssExMxPmIpv6McPclIdSet(dev, vrConfig.defaultMcIpv6PclId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpv6McPclIdSet", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }

        rc = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId,vrId,&vrConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIpLpmVirtualRouterAdd", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }

        vrInitialized = GT_TRUE;
    }

    return GT_OK;
}
#endif

static GT_STATUS prvOtherLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    rc = cpssExMxPmPolicerInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPolicerInit", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    return GT_OK;
}


/* Definition of lib init functions. */
static FUNCP_CPSS_MODULE_INIT cpssInitList[] = {
#ifdef IMPL_PORT
        prvPortLibInit,
#endif

#ifdef IMPL_PHY
   prvPhyLibInit,
#endif

#ifdef IMPL_BRIDGE
   prvBridgeLibInit,
#endif

#ifdef IMPL_NETIF
   prvNetIfLibInit,
#endif

#ifdef IMPL_IP
   prvIpLibInit,
#endif

#ifdef IMPL_TRUNK
    prvTrunkLibInit,
#endif

    prvOtherLibInit,

    0
};


/* EXMXPM Fabric configuration */
typedef enum
{
    /* cells from VOQ0 are switched to XBAR port 4 by XBAR
       cells from VOQ1 are switched to XBAR port 5 by XBAR
       No cells are going to HGL ports  */
    APP_DEMO_EXMXPM_FABRIC_MODE_INT_XBAR_LOOPBACK_E,

    /* HGL port 0 connected to HGL 1 by cable,
       HGL port 2 connected to HGL 3 by cable  */
    APP_DEMO_EXMXPM_FABRIC_MODE_EXT_LOOPBACK_E
}APP_DEMO_EXMXPM_FABRIC_MODE_ENT;

/*******************************************************************************
* appDemoExMxPmFabricModeSet
*
* DESCRIPTION:
*       Fabric Configuration set.
*
* INPUTS:
*       devNum - device number.
*       faMode - fabric mode
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmFabricModeSet
(
    IN  GT_U8                           devNum,
    IN  APP_DEMO_EXMXPM_FABRIC_MODE_ENT faMode
)
{
    CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC  vidxConfig; /*vidx configuration struct*/
    CPSS_PORTS_BMP_STC                         enableBmp;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC   fportConfig;
    CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC         xbarConfig;
    GT_U32                              voqTrgDevsBmpArr[4];/* target devs bit
                                                                    map array */
    GT_FPORT                            fport; /* fabric port number */
    GT_U8                               trgDev;/* target device number */
    GT_FPORT                            fportsArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];
    GT_U32                              index;
    GT_U32                              lbh;
    GT_STATUS                           rc; /* return code */
    GT_FPORT                            fportsInUse; /* number of FPorts that VOQ uses */
    GT_FPORT                            fportInUseArr[CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS];
                                        /* list of FPorts that VOQ uses*/
    GT_U32                              ii; /* iterator */
    GT_BOOL                             powerSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
                                        /* power status per SERDES lane */


    /* assign Multicast Fabric VIDX to be 1 for all kind of traffic.
      Only Fabric VIDX 1 is used for multicast cells switching */
    vidxConfig.fabricVIDX0 = 0x1;
    vidxConfig.fabricVIDX1 = 0x1;
    vidxConfig.fabricVidxBit0IsLbiBit0 = GT_FALSE;
    vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_FLOOD_ALL_E;
    vidxConfig.copyFabricVIDXToVIDX = GT_FALSE;
    vidxConfig.fabricVIDXAssignMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_ASSIGN_MODE_0_E;
    /* LBI and Fabric VIDX configuration */
    if ((rc = cpssExMxPmFabricConfigVidxSet(devNum,&vidxConfig)) != GT_OK)
        return rc;

    /* enable first 64 targed devices in the VOQs */
    voqTrgDevsBmpArr[0] = 0xffffffff;
    voqTrgDevsBmpArr[1] = 0xffffffff;
    voqTrgDevsBmpArr[2] = 0x0;
    voqTrgDevsBmpArr[3] = 0x0;

    /* enable VOQ 0 devs */
    rc = cpssExMxPmFabricVoqDevTblSet(devNum, 0, voqTrgDevsBmpArr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqDevTblSet", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /* enable VOQ 1 devs */
    rc = cpssExMxPmFabricVoqDevTblSet(devNum, 1, voqTrgDevsBmpArr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqDevTblSet", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /* disable all VOQs for sanity check */
    for (fport = 0 ; fport < CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS ; fport++)
    {
        /* disable uc fabric ports units 0,1 */
        rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 0, fport, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }

        rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 1, fport, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
    }


    /* enable VOQs only for used Fabric Ports */
    switch(faMode)
    {
        case APP_DEMO_EXMXPM_FABRIC_MODE_INT_XBAR_LOOPBACK_E:
            /* the XBAR is enabled.
               VOQ0 sends cells to internal XBAR port 4,
               VOQ1 sends cells to internal XBAR port 5.*/

            /* enable uc fabric ports units 0,1 */
            rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 0, 4, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
            if (rc != GT_OK)
            {
                return  rc;
            }

            rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 1, 5, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
            if (rc != GT_OK)
            {
                return  rc;
            }
            break;
        case APP_DEMO_EXMXPM_FABRIC_MODE_EXT_LOOPBACK_E:
            /* the XBAR is working in the Active Standby mode.
               VOQ0 sends cells to HGL 0, VOQ1 sends cells to HGL2.
               Cells are looped back to HGL 1 and 3 correspondingly.
               HGL 1 and 3 are "external XBAR"  ports for LL flow control.
               Single destination traffic to HGL0 must be enqueued to VoQ0[1]
               Single destination traffic to HGL2 must be enqueued to VoQ0[3] */

            /* enable uc fabric ports units 0, 1 */
            rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 0, 1, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
            if (rc != GT_OK)
            {
                return  rc;
            }
            rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 1, 3, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
            if (rc != GT_OK)
            {
                return  rc;
            }

            break;
        default:
            return GT_BAD_PARAM;
    }


    /* enable mc fabric ports units 0,1 */
    rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 0,
                                                  CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /* enable mc fabric ports units 0,1 */
    rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 1,
                                            CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }


    /* configure VOQ unicast table */

    /* VOQ 0 */
    switch(faMode)
    {
        case APP_DEMO_EXMXPM_FABRIC_MODE_INT_XBAR_LOOPBACK_E:
            fport = 4; /* use VOQ for internal XBAR port 4 */
            break;
        case APP_DEMO_EXMXPM_FABRIC_MODE_EXT_LOOPBACK_E:
            fport = 1; /* use VOQ for XBAR port 1 */
            break;
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[dead_error_begin] */
        default:
            return GT_BAD_PARAM;
    }

    for (index = 0 ; index < CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS ; index++)
        fportsArr[index] = fport;


    for (trgDev = 0 ; trgDev < 64 ; trgDev++)
    {
        /* enable VOQ 0 UC table */
        rc = cpssExMxPmFabricVoqDevToFportMapSet(devNum, 0, trgDev, fportsArr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqDevToFportMapSet", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
    }

    /* VOQ 1 */
    switch(faMode)
    {
        case APP_DEMO_EXMXPM_FABRIC_MODE_INT_XBAR_LOOPBACK_E:
            fport = 5; /* use VOQ for internal XBAR port 5 */
            break;
        case APP_DEMO_EXMXPM_FABRIC_MODE_EXT_LOOPBACK_E:
            fport = 3; /* use VOQ for XBAR port 3 */
            break;
        default:
            return GT_BAD_PARAM;
    }

    for (index = 0 ; index < CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS ; index++)
        fportsArr[index] = fport;

    for (trgDev = 0 ; trgDev < 64 ; trgDev++)
    {
        /* enable VOQ 1 UC table */
        rc = cpssExMxPmFabricVoqDevToFportMapSet(devNum, 1, trgDev, fportsArr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqDevToFportMapSet", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
    }

    /* XBAR Global mode */
    switch(faMode)
    {
        case APP_DEMO_EXMXPM_FABRIC_MODE_INT_XBAR_LOOPBACK_E:
            /* XBAR enabled */
            xbarConfig.mode = CPSS_EXMXPM_FABRIC_XBAR_ENABLED_E;
            xbarConfig.xbarTcModeEnable = GT_FALSE;
            xbarConfig.voQUnit0ActiveHyperGlink = 0; /* don't care for XBAR enabled */
            xbarConfig.voQUnit1ActiveHyperGlink = 2; /* don't care for XBAR enabled */
            break;
        case APP_DEMO_EXMXPM_FABRIC_MODE_EXT_LOOPBACK_E:
            /* Active standby mode is used with active HGLs 0 and 2*/
            xbarConfig.mode = CPSS_EXMXPM_FABRIC_XBAR_BYPASS_ACTIVE_STANDBY_E;
            xbarConfig.xbarTcModeEnable = GT_FALSE;
            xbarConfig.voQUnit0ActiveHyperGlink = 0;
            xbarConfig.voQUnit1ActiveHyperGlink = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = cpssExMxPmFabricXbarConfigSet(devNum, &xbarConfig);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarConfigSet", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    fportConfig.bcSrcDevFilterEnable            = GT_FALSE;
    fportConfig.ucSrcDevFilterEnable            = GT_FALSE;
    fportConfig.diagSrcDevFilterEnable          = GT_FALSE;
    fportConfig.diagCellSuperHighPriorityAssign = GT_TRUE;
    fportConfig.mcSrcDevFilterEnable            = GT_FALSE;
    fportConfig.portEnable                      = GT_TRUE;

    /* Gunit Gen config per fabric port */
    switch(faMode)
    {
        case APP_DEMO_EXMXPM_FABRIC_MODE_INT_XBAR_LOOPBACK_E:
            fportInUseArr[0] = 4; /* internal XBAR port 4 */
            fportInUseArr[1] = 5; /* internal XBAR port 5 */
            fportsInUse = 2;      /* only two internal XBAR ports are used */
            break;
        case APP_DEMO_EXMXPM_FABRIC_MODE_EXT_LOOPBACK_E:
            fportInUseArr[0] = 0; /* XBAR port 0 */
            fportInUseArr[1] = 1; /* XBAR port 1 */
            fportInUseArr[2] = 2; /* XBAR port 2 */
            fportInUseArr[3] = 3; /* XBAR port 3 */
            fportInUseArr[4] = 4; /* XBAR port 4 */
            fportInUseArr[5] = 5; /* XBAR port 5 */
            fportsInUse = 6;      /* all XBAR ports are used */
            break;
        default:
            return GT_BAD_PARAM;
    }

    for(ii = 0; ii < fportsInUse; ii++)
    {
        fport = fportInUseArr[ii];

        rc = cpssExMxPmFabricXbarFportConfigSet(devNum, fport, &fportConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarConfigSet", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
    }

    /* "disable" unicast for sanity check  */
    for(fport = 0; fport < 6; fport++)
    {
        for (trgDev = 0 ; trgDev < 64 ; trgDev++)
        {
            for (lbh = 0; lbh < 4; lbh++)
            {
                rc = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(devNum,
                             fport, trgDev , lbh, GT_FALSE, 0);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportUnicastCellSwitchingSet", rc);
                if (rc != GT_OK)
                {
                    return  rc;
                }
            }
        }
    }


    /* Configure the unicast table */
    if(faMode == APP_DEMO_EXMXPM_FABRIC_MODE_INT_XBAR_LOOPBACK_E)
    {
        /* perform loopback in the Unicast table. Send all cells to internal ports 4 and 5 */
        for(fport = 4; fport < 6; fport++)
        {
            for (trgDev = 0 ; trgDev < 64 ; trgDev++)
            {
                for (lbh = 0; lbh < 4; lbh++)
                {
                    rc = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(devNum,
                                 fport, trgDev , lbh, GT_TRUE, fport);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportUnicastCellSwitchingSet", rc);
                    if (rc != GT_OK)
                    {
                        return  rc;
                    }
                }

            }
        }

    }
    else if(faMode == APP_DEMO_EXMXPM_FABRIC_MODE_EXT_LOOPBACK_E)
    {
        for (trgDev = 0 ; trgDev < 64 ; trgDev++)
        {
            for (lbh = 0; lbh < 4; lbh++)
            {
                /* configure fabric ports 1 and 3.
                   send all cells to fabric port 4 and 5. */
                rc = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(devNum,
                             1, trgDev , lbh, GT_TRUE, 4);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportUnicastCellSwitchingSet", rc);
                if (rc != GT_OK)
                {
                    return  rc;
                }

                rc = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(devNum,
                             3, trgDev , lbh, GT_TRUE, 5);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportUnicastCellSwitchingSet", rc);
                if (rc != GT_OK)
                {
                    return  rc;
                }

            }
        }
    }

    /* Configure the multicast table for VIDX 1 only */
    rc = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(devNum,
                     CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E, 1, 0x10);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportMulticastCellSwitchingSet", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    rc = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(devNum,
                     CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E, 1, 0x20);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportMulticastCellSwitchingSet", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }


    enableBmp.ports[0] = 0xFFFFFFFF;
    enableBmp.ports[1] = 0xFFFFFFFF;

    if(faMode == APP_DEMO_EXMXPM_FABRIC_MODE_INT_XBAR_LOOPBACK_E)
    {
        return GT_OK;
    }
    /* Configure HGL ports */

    /* HyperGLink Link Level Flow Control Enable */
    for(fport = 0; fport < 4; fport++)
    {
        rc = cpssExMxPmFabricHGLinkFlowControlEnableSet(devNum, fport, &enableBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricHGLinkFlowControlEnableSet", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
    }

    /* configure all 6 SERDES lanes power UP */
    powerSerdesLaneArr[0] = powerSerdesLaneArr[1] = powerSerdesLaneArr[2] =
    powerSerdesLaneArr[3] = powerSerdesLaneArr[4] = powerSerdesLaneArr[5] = GT_TRUE;

    for(fport = 0; fport < 4; fport++)
    {
        rc = cpssExMxPmSerdesPowerSet(devNum,fport,powerSerdesLaneArr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmSerdesPowerSet", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
    }


   return GT_OK;

}

/*******************************************************************************
* appDemoExMxPmInlifInit
*
* DESCRIPTION:
*       This function initializes inlif mode per port.
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum               -  device number.
*       logicalConfigParams  - (pointer to)The PP specific configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmInlifInit
(
    IN  GT_U8                       devNum,
    IN  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParams
)
{
    GT_STATUS rc;
    CPSS_EXMXPM_INLIF_PORT_MODE_ENT  inlifMode = CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E;
    GT_U8 portNum;

    switch (logicalConfigParams->inlifPortMode)
    {
    case APP_DEMO_CPSS_INLIF_PORT_MODE_PORT_E:
        inlifMode = CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E;
        break;
    case APP_DEMO_CPSS_INLIF_PORT_MODE_VLAN_E:
        inlifMode = CPSS_EXMXPM_INLIF_PORT_MODE_VLAN_E;
        break;
    default:
        break;
    }

    for (portNum = 0 ; portNum < PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts ;  portNum++)
    {
        rc = cpssExMxPmInlifPortModeSet(devNum,portNum,inlifMode);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifPortModeSet", rc);
        if (rc != GT_OK)
            return rc;
    }

    rc = cpssExMxPmInlifPortModeSet(devNum,CPSS_CPU_PORT_NUM_CNS,inlifMode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifPortModeSet", rc);
    if (rc != GT_OK)
        return rc;


    return GT_OK;
}

GT_STATUS dbgAppDemoPuma3SwResetLogicalInit
(
    IN  GT_U8 devNum
)
{

    GT_STATUS rc;

    /* call CPSS to do Pp logical init */
    rc = cpssExMxPmCfgPpLogicalInit(devNum, &copyPpLogicaslInfo);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssExMxPmCfgPpLogicalInit: rc = %d\r\n", rc);
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* appDemoExMxPmPuma3PortLinkWa
*
* DESCRIPTION:
*      For 40G interfaces execute WA - connect/disconnect leaves link down problem
*
* INPUTS:
*       devNum  - device number
*       portNum - global port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoExMxPmPuma3PortLinkWa
(
    IN   GT_U8  devNum,
    IN   GT_U8  portNum
)
{
    GT_STATUS                       rc;         /* return code */
    CPSS_EXMXPM_HW_WA_ENT           waArr[1];   /* array of workarounds to enable */
    GT_U32                          additionalInfoBmpArr[1]; /* additional WA's info */
    GT_U32                          disable40GWa; /* don't run 40G con/discon WA */

        rc = appDemoDbEntryGet("disable40GWa", &disable40GWa);
    if ((rc == GT_OK) &&  (disable40GWa != 0))
        return GT_OK;

    waArr[0] = CPSS_EXMXPM3_IMPLEMENT_WA_40G_CONNECT_DISCONNET_WA_E;
    additionalInfoBmpArr[0] = portNum;
    rc = cpssExMxPmHwPpImplementWaInit(devNum,1,&waArr[0], &additionalInfoBmpArr[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmDbReset
*
* DESCRIPTION:
*       Puma3 DB reset.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmDbReset
(
    IN  GT_U8  devNum
)
{
    return GT_NOT_IMPLEMENTED;
}
