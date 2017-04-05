/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
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
* gtAppDemoDxSysConfig.c
*
* DESCRIPTION:
*       System configuration and initialization control for Dx devices.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*       $Revision: 151 $
*
*******************************************************************************/
#ifdef COW_INTERNAL_COMPILE
    /* those 3 are defined , so remove them */
    #undef EXMXPM_FAMILY
    #undef EX_FAMILY
    #undef CHX_FAMILY

    /* define only the tested one */
    #define CHX_FAMILY

    #define IMPL_IP

#endif /*COW_INTERNAL_COMPILE*/

#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/userExit/userEventHandler.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
/* Cascading support */
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgE2Phy.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>

#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagPacketGenerator.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortAp.h>

/* PFC support */
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>
#include <cpss/generic/port/cpssPortTx.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxUtilLion.h>

#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>
#include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpm.h>

#ifdef IMPL_TM
    #include <cpss/generic/tm/cpssTmCtl.h>
#endif

/*******************************************************************************
 * External variables
 ******************************************************************************/

static CPSS_PP_PORT_GROUP_INIT_INFO_STC portGroupsPhase1InfoArr[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS];

/* DB to save phase 1 parameters used during init */
static CPSS_DXCH_PP_PHASE1_INIT_INFO_STC ppPhase1ParamsDb;

GT_BOOL  changeToDownHappend[CPSS_MAX_PORTS_NUM_CNS];
/*******************************************************************************
 * Local usage variables
 ******************************************************************************/

static FUNCP_CPSS_MODULE_INIT cpssInitList[20]; /* Defined at the end of the file. */

static CPSS_PORTS_BMP_STC trxTrainBmp; /* bitmap of ports to run
                                           TRX training on */
CPSS_PORTS_BMP_STC todoWaBmp; /* bitmap of ports to run WA's on */
static CPSS_PORTS_BMP_STC todoAlign90Bmp; /* bitmap of ports to run align90 on */
/* Id's of training service tasks */
static GT_U32   trxTrainingTaskId = 0;
static GT_U32   align90TaskId = 0;
/* Id's of semaphores used by training service tasks */
CPSS_OS_SIG_SEM waTSmid = 0;
static CPSS_OS_SIG_SEM trxTrSmid = 0;
static CPSS_OS_SIG_SEM al90TSmid = 0;

static GT_U32 trxTimeoutArray[CPSS_MAX_PORTS_NUM_CNS] = { 0 };
GT_U32 locksTimeoutArray[CPSS_MAX_PORTS_NUM_CNS] = { 0 };
static GT_U32 trxTrainingRetryCntArray[CPSS_MAX_PORTS_NUM_CNS] = { 0 };
static GT_U32 al90TimeoutArray[CPSS_MAX_PORTS_NUM_CNS] = { 0 };
/* for now maximal number of serdes per port where align90 could run is 4 */
static CPSS_DXCH_PORT_ALIGN90_PARAMS_STC serdesParams[CPSS_MAX_PORTS_NUM_CNS][4];

static GT_STATUS appDemoConvert2DxChPhase1Info
(
    IN  GT_U8                              dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS         *appCpssPpPhase1ParamsPtr,
    OUT CPSS_DXCH_PP_PHASE1_INIT_INFO_STC  *cpssPpPhase1InfoPtr
);

static GT_STATUS appDemoConvert2DxChPhase2Info
(
    IN  CPSS_PP_PHASE2_INIT_PARAMS          *appCpssPpPhase2ParamsPtr,
    OUT CPSS_DXCH_PP_PHASE2_INIT_INFO_STC   *cpssPpPhase2InfoPtr
);

static GT_VOID appDemoCoreToCpssDxChPpInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC       *sysConfPtr,
    OUT CPSS_DXCH_PP_CONFIG_INIT_STC  *ppLogicalInfoPtr
);

/*static GT_VOID appDemoCoreToCpssPpSalInfoConvert      - For Salsa - not implemented for now..
(
    IN  CPSS_PP_CONFIG_INIT_STC         *sysConfPtr,
    OUT CPSS_EXMXTG_PP_CONFIG_INIT_STC  *tgPpLogicalInfoPtr
);*/

static GT_STATUS prvCscdPortsInit
(
    IN  GT_U8   dev
);

static GT_STATUS prv10GPortsConfig
(
    IN  GT_U8   dev
);

static GT_STATUS appDemoDxChLion2SerdesTrxTrainigStart
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    interface
);

/* User configured bitmap for SERDES power down */
GT_U32 appDemoDxChPowerDownBmp = 0;

/* offset used during HW device ID calculation formula */
extern GT_U8 appDemoHwDevNumOffset;
/* HSU  global DB */
extern CPSS_SYSTEM_RECOVERY_INFO_STC  systemRecoveryInfo;

#ifndef ASIC_SIMULATION
/* semaphore to manage serdes tuning selector task(declared in userEventHandler.c) */
extern CPSS_OS_SIG_SEM tuneSmid;
extern CPSS_PORTS_BMP_STC todoTuneBmp; /* bitmap of ports to run tuning on */
#endif

/* packet size */
#define APP_DEMO_PACKET_SIZE_CNS 68

GT_U32   trainingTrace=0;/* option to disable the print in runtime*/
#define TRAINING_DBG_PRINT_MAC(x) if(trainingTrace) cpssOsPrintSync x

extern GT_U32 trainingControl(IN GT_U32  allowPrintNew)
{
    GT_U32  oldState = trainingTrace;
    trainingTrace = allowPrintNew;

    return oldState;
}

extern GT_STATUS lion2AppDemoDataIntegrityInit
(
    IN  GT_U8 devNum
);

static GT_BOOL appDemoLion3TtiTcamUseAppdemoOffset = GT_TRUE;
static GT_BOOL appDemoLion3PclTcamUseIndexConversion = GT_TRUE;

/* base offset for PCL client in TCAM */
static GT_U32 appDemoTcamPclRuleBaseIndexOffset = 0;

/* base offset for TTI client in TCAM */
static GT_U32 appDemoTcamTtiHit0RuleBaseIndexOffset;
static GT_U32 appDemoTcamTtiHit1RuleBaseIndexOffset;
static GT_U32 appDemoTcamTtiHit2RuleBaseIndexOffset;
static GT_U32 appDemoTcamTtiHit3RuleBaseIndexOffset;
/* number of rules that can be used per lookup of TTI */
static GT_U32 appDemoTcamTtiHit0MaxNum = 0;
static GT_U32 appDemoTcamTtiHit1MaxNum = 0;
static GT_U32 appDemoTcamTtiHit2MaxNum = 0;
static GT_U32 appDemoTcamTtiHit3MaxNum = 0;

/* number of entries for TTI in TCAM */
static GT_U32 tcamEntriesNum;

/* number of entries for TTI in TCAM floor */
static GT_U32 tcamFloorEntriesNum;

/*******************************************************************************
* appDemoDxChDeviceIdAndRevisionPrint
*
* DESCRIPTION:
*       The function prints out ID and revision of device
*
* APPLICABLE DEVICES: All.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS appDemoDxChDeviceIdAndRevisionPrint
(
    GT_U8   devNum
)
{
    CPSS_DXCH_CFG_DEV_INFO_STC devInfo; /* device info */
    char *lionRevName[]={"A0", "B0", "B0A", "B1 or above"}; /* revision names */
    char *xcat3RevName[]={"A0", "A1"}; /* revision names */
    char *revNamePtr = NULL;            /* name of revision */
    int  revision;                      /* revision */
    GT_STATUS   rc;                     /* return code */
    GT_U32 coreClockDb, coreClockHw;    /* core clock value */

    /* get device info */
    rc = cpssDxChCfgDevInfoGet(devNum,&devInfo);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgDevInfoGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    revision = devInfo.genDevInfo.revision;
    switch (devInfo.genDevInfo.devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_LION_E:
            switch (revision)
            {
                case 0:
                case 1:
                    /* A0 revisions */
                    revNamePtr = lionRevName[0];
                    break;
                case 2:
                    /* check shaper API disable flag to distinguish between
                       B0 and B0A.*/
                    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
                    {
                        /* B0 revisions */
                        revNamePtr = lionRevName[1];
                    }
                    else
                    {
                        /* B0A revisions */
                        revNamePtr = lionRevName[2];
                    }
                    break;
                default:
                    revNamePtr = lionRevName[3];
                    break;
            }
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            switch (revision)
            {
                case 3:
                    /* A0 revisions */
                    revNamePtr = xcat3RevName[0];
                    break;
                case 4:
                    revNamePtr = xcat3RevName[1];
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    if (revNamePtr)
    {
        osPrintf("Device[%d] ID 0x%X revision %s\n",devNum, devInfo.genDevInfo.devType, revNamePtr);
    }
    else
    {
        osPrintf("Device[%d] ID 0x%X revision %d\n",devNum, devInfo.genDevInfo.devType, revision);
    }

    rc = cpssDxChHwCoreClockGet(devNum, &coreClockDb, &coreClockHw);
    if(GT_OK != rc)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwCoreClockGet", rc);
        return rc;
    }
    osPrintf("Core clock = %dMhz\n\n", coreClockHw);

    return GT_OK;
}

/*******************************************************************************
 * Public API implementation
 ******************************************************************************/

/*******************************************************************************
* waTriggerInterruptsMask
*
* DESCRIPTION:
*       Mask/unmask specific interrupts on port that can trigger WA's restart
*       or other not needed for now actions.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*       ifMode      - current interface on port
*       operation   - mask/unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       Affected interrupts: CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,
*                           CPSS_PP_PORT_LINK_STATUS_CHANGED_E,
*                           CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E.
*
*******************************************************************************/
GT_STATUS waTriggerInterruptsMask
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
)
{
    GT_STATUS   rc;
    GT_U32      laneNum,
                lanesCount;

    if((ifMode != CPSS_PORT_INTERFACE_MODE_XGMII_E)
       || (operation != CPSS_EVENT_UNMASK_E))
    {
        if((CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode)
           || (CPSS_PORT_INTERFACE_MODE_XGMII_E == ifMode))
        {
            GT_BOOL supported;

            rc = cpssDxChPortInterfaceSpeedSupportGet(devNum, portNum, ifMode,
                                                      CPSS_PORT_SPEED_10000_E,
                                                      &supported);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortInterfaceSpeedSupportGet(portNum=%d,ifMode=%d):rc=%d\n",
                                portNum, ifMode, rc);
                return rc;
            }

            if(supported)
            {
                lanesCount = (CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode) ? 2 : 6;
                for(laneNum = 0; laneNum < lanesCount; laneNum++)
                {
                    if(trainingTrace)
                    {
                        cpssOsPrintf("cpssEventDeviceMaskWithEvExtDataSet(LANE_SIGNAL_DETECT_CHANGED,((portNum(%d)<<8)|laneNum(%d))=0x%x,op=%d\n",
                            portNum, laneNum, ((portNum<<8)|laneNum), operation);
                    }
                    rc = cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                             CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E,
                                                             ((portNum<<8)|laneNum),
                                                             operation);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssEventDeviceMaskWithEvExtDataSet(CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,extData=0x%x):rc=%d\n",
                                        ((portNum<<8)|laneNum), rc);
                        return rc;
                    }
                }

                rc = cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                         CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                                         portNum,
                                                         CPSS_EVENT_MASK_E);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssEventDeviceMaskWithEvExtDataSet(CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,extData=%d):rc=%d\n",
                                    portNum, rc);
                    return rc;
                }
            }
        }
        else if((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode)
                || (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode)
                || (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode)
                || (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E == ifMode))

        {
            rc = cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                     CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                                     portNum,
                                                     operation);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssEventDeviceMaskWithEvExtDataSet(CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,extData=%d):rc=%d\n",
                                portNum, rc);
                return rc;
            }
        }
    }

    rc = cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                             CPSS_PP_PORT_LINK_STATUS_CHANGED_E,
                                             portNum,
                                             operation);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssEventDeviceMaskWithEvExtDataSet(CPSS_PP_PORT_LINK_STATUS_CHANGED_E,extData=%d):rc=%d\n",
                        portNum, rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChPowerDownBmpSet
*
* DESCRIPTION:
*       Sets AppDemo power down SERDES bitmask.
*
* INPUTS:
*       powerDownBmp  - the value to be written.
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
GT_STATUS appDemoDxChPowerDownBmpSet
(
    IN GT_U32   powerDownBmp
)
{
    appDemoDxChPowerDownBmp = powerDownBmp;
    return GT_OK;
}

/*******************************************************************************
* appDemoDxChPhase1ParamsGet
*
* DESCRIPTION:
*       Gets the parameters used during phase 1 init.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       ppPhase1ParamsPtr - (pointer to) parameters used during phase 1 init.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChPhase1ParamsGet
(
    OUT CPSS_DXCH_PP_PHASE1_INIT_INFO_STC *ppPhase1ParamsPtr
)
{
    if( NULL == ppPhase1ParamsPtr )
    {
        return GT_FAIL;
    }

    *ppPhase1ParamsPtr = ppPhase1ParamsDb;
    return GT_OK;
}

/*******************************************************************************
* appDemoDxChFdbInit
*
* DESCRIPTION:
*        flush the FDB of DXCH device after init.
*
* INPUTS:
*       dev  - The Pp device number.
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
static GT_STATUS appDemoDxChFdbInit(
    IN GT_U8 dev
)
{
    GT_STATUS rc;            /* return value */
    GT_U32    port;          /* port iterator  */
    /* GT_TRUE  - triggered action completed
       GT_FALSE - triggered action is not completed yet */
    GT_BOOL   actFinished;
    GT_HW_DEV_NUM   hwDevNum;      /* HW device number */
    GT_U32    fdbBmp;        /* bmp of devices to set */
    GT_U32    i;             /* iterator */
    GT_U32              value;
    CPSS_FDB_ACTION_MODE_ENT    actionMode;
    GT_U32    devIdx; /* index to appDemoPpConfigList */
    GT_U32    numPorts;/*number of port*/

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    if(appDemoDbEntryGet("ctrlMacLearn", &value) == GT_OK)
    {
        /* --> use same string as in PSS : values are:
            typedef enum
            {
                GT_AUTOMATIC        = 0,
                GT_CONTROLED_MSSG   = 1,
                GT_CONTROLED_TAG    = 2
            } GT_LEARNING_MODE;
        */

        /* allow to force auto learning , because by default we set ' Controlled learning'*/
        if(value == 0 /*GT_AUTOMATIC*/)
        {
            appDemoSysConfig.forceAutoLearn = GT_TRUE;
        }
    }

    /*********************************************************/
    /* Set FDB learning mode                                 */
    /*********************************************************/
    rc = cpssDxChBrgFdbMacVlanLookupModeSet(dev, CPSS_IVL_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMacVlanLookupModeSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* restore automatic action mode*/
    rc = cpssDxChBrgFdbMacTriggerModeSet(dev, CPSS_ACT_AUTO_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMacTriggerModeSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* default for the 'appDemo' */
    actionMode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

    switch(PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.fdbMode)
    {
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
            actionMode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            break;
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_NON_MULTI_PORT_GROUP_DEVICE_E:
            if((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
               (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
            {
                actionMode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            }
            break;
        default:
            break;
    }

    /* restore age with/without removal */
    rc = cpssDxChBrgFdbTrigActionStart(dev, actionMode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbTrigActionStart", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* wait for action to be over */
    actFinished = GT_FALSE;
    while(actFinished == GT_FALSE)
    {
        rc = cpssDxChBrgFdbTrigActionStatusGet(dev,&actFinished);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbTrigActionStatusGet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        osTimerWkAfter(1);
    }

    /* enable the sending to CPU of AA/TA */
    if ((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E && systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E) ||
        (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E))
    {
        rc = cpssDxChBrgFdbAAandTAToCpuSet(dev, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Setting aging timeout to default timeout (300 seconds) is needed because
       DX106 core clock is 144MHz and not 200MHz as in other PPs. */
    rc = cpssDxChBrgFdbAgingTimeoutSet(dev, 300);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAgingTimeoutSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    if(GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(dev))
    {
        numPorts = appDemoPpConfigList[devIdx].maxPortNumber;
    }
    else
    {
        /* set range of physical ports although those are in eport range table */
        numPorts = MAX(256,PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(dev));
    }

    for (port = 0; port < numPorts; port++)
    {
        if(GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(dev))
        {
            CPSS_ENABLER_PORT_SKIP_CHECK(dev,port);
        }

        if(appDemoSysConfig.forceAutoLearn == GT_TRUE)
        {
            rc = cpssDxChBrgFdbPortLearnStatusSet(dev, port, GT_TRUE, CPSS_LOCK_FRWRD_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbPortLearnStatusSet - auto learning", rc);
        }
        else
        {
            rc = cpssDxChBrgFdbPortLearnStatusSet(dev, port, GT_FALSE, CPSS_LOCK_FRWRD_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbPortLearnStatusSet - controlled learning", rc);
        }
        if (GT_OK != rc)
        {
            return rc;
        }

        /* note the cascade init will make sure to disable the sending from
           the cascade ports */
        if ((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E && systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E) ||
            (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E))
        {
            rc = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbNaToCpuPerPortSet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
    }

    fdbBmp = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            rc = cpssDxChCfgHwDevNumGet(appDemoPpConfigList[i].devNum, &hwDevNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }
            fdbBmp |= (1 << hwDevNum );
        }
    }

    if (! PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev))
    {
        if(fdbBmp != 0)
        {
            rc = cpssDxChBrgFdbDeviceTableSet(dev, fdbBmp);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbDeviceTableSet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
    }

    /* Deletes all addresses from FDB table */
    rc = appDemoDxChFdbFlush(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChFdbFlush", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}
/*******************************************************************************
* appDemoDxHwPpPhase1Init
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
extern GT_STATUS mvHwsServiceCpuEnable(GT_U8 devNum, GT_U32 enable); /* temporary */
GT_STATUS appDemoDxHwPpPhase1Init
(
    IN  GT_U8                        dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS  *appCpssPpPhase1ParamsPtr,
    OUT CPSS_PP_DEVICE_TYPE         *ppDevTypePtr
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   cpssPpPhase1Params;     /* CPSS phase 1 PP params */
    GT_U32  value;
    CPSS_DXCH_IMPLEMENT_WA_ENT   waFromCpss[CPSS_DXCH_IMPLEMENT_WA_LAST_E];
    GT_U32  waIndex=0;
    GT_U32  ii;
    GT_U8     devNum;
    GT_U32    portGroupId;/*iterator for port group Id*/
    GT_BOOL trunkToCpuWaEnabled = GT_FALSE;

    if ((NULL == appCpssPpPhase1ParamsPtr) ||
        (NULL == ppDevTypePtr))
    {
        return GT_BAD_PTR;
    }

    /* fill CPSS Phase1 init parameters */
    rc = appDemoConvert2DxChPhase1Info(dev,
                                       appCpssPpPhase1ParamsPtr,
                                       &cpssPpPhase1Params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoConvert2DxChPhase1Info", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* use same string as for PSS */
    if(appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            trunkToCpuWaEnabled = GT_TRUE;

            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_SDMA_PKTS_FROM_CPU_STACK_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_SDMA_PKTS_FROM_CPU_STACK_PADDING_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_PADDING_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_RESTRICTED_ADDRESS_FILTERING_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_MC_BANDWIDTH_RESTRICTION_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_MC_BANDWIDTH_RESTRICTION_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_CUT_THROUGH_SLOW_TO_FAST_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_CUT_THROUGH_SLOW_TO_FAST_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_ROUTER_TCAM_RM_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_ROUTER_TCAM_RM_E;
        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_PTP_TIMESTAMP_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E;
        }
    }

    if(appDemoDbEntryGet("hwsChange1GSpeedCfgV43", &value) == GT_OK)
    {
        if(value)
        {
            cpssDxChHwInitLion2GeBackwardCompatibility();
        }
    }

    /* Init cpss with no HW writes */
    if((appDemoDbEntryGet("initRegDefaults", &value) == GT_OK) && (value != 0))
    {
        rc = cpssDxChDiagRegDefaultsEnableSet(GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /* Init cpss with no HW write to DevNum */
    if((appDemoDbEntryGet("fastBootSkipOwnDeviceInit", &value) == GT_OK) && (value != 0))
    {
        rc = cpssDxChDiagFastBootSkipOwnDeviceInitEnableSet(GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /*checking if initialization of the interrupts is needed*/
    if (appDemoDbEntryGet("initSystemWithoutInterrupts", &value) == GT_OK && value == 1)
    {
        /*forcing not doing initialization of the interrupts by puting the
        CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS = 0xFFFFFFFF value in intVecNum
        */
        if (cpssPpPhase1Params.busBaseAddr == CPSS_PARAM_NOT_USED_CNS)
            /* this is indication that the device should be 'multi-port-groups' device */
        {
            devNum = cpssPpPhase1Params.devNum;
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
            {
                cpssPpPhase1Params.multiPortGroupsInfoPtr[portGroupId].intVecNum=0xFFFFFFFF;
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        }
        else
        {
            cpssPpPhase1Params.intVecNum=0xFFFFFFFF;
        }
    }

    switch(appCpssPpPhase1ParamsPtr->deviceId)
    {
        case CPSS_BOBCAT2_ALL_DEVICES_CASES_MAC:
        case CPSS_BOBK_ALL_DEVICES_CASES_MAC:
            {
                GT_U32 apEnable;
                rc = appDemoDbEntryGet("apEnable", &apEnable);
                if(rc != GT_OK)
                {/* by default for now disabled */
                    apEnable = 0;
                }
                rc = cpssDxChPortApEnableSet(dev, 0x1, apEnable);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortApEnableSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            break;
        default:
            break;
    }

    if((appDemoDbEntryGet("doublePhysicalPorts", &value) == GT_OK) && (value != 0))
    {
        /*Bobcat3: 512 instead of 256 */
        cpssPpPhase1Params.maxNumOfPhyPortsToUse = 512;
    }
    else
    {
        /* use defaults or ignored */
        cpssPpPhase1Params.maxNumOfPhyPortsToUse = 0;
    }

    rc = cpssDxChHwPpPhase1Init(&cpssPpPhase1Params, ppDevTypePtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpPhase1Init", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDxChDeviceIdAndRevisionPrint(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChDeviceIdAndRevisionPrint", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(waIndex)
    {
        rc = cpssDxChHwPpImplementWaInit(dev,waIndex,&waFromCpss[0],0);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpImplementWaInit", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        if (trunkToCpuWaEnabled == GT_TRUE)
        {
            for(ii = SYSTEM_DEV_NUM_MAC(0) ; ii < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount);ii++)
            {
                if(appDemoPpConfigList[ii].valid && appDemoPpConfigList[ii].devNum == dev)
                {
                    /* save to DB --> will be used by the galtis Wrappers */
                    appDemoPpConfigList[ii].wa.trunkToCpuPortNum = GT_TRUE;
                    break;
                }
            }
        }
    }

    return GT_OK;
} /* appDemoDxHwPpPhase1Init */

/*******************************************************************************
* appDemoDxHwPpStartInit
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
GT_STATUS appDemoDxHwPpStartInit
(
    IN  GT_U8                       dev,
    IN  GT_BOOL                     fReloadEeprom,
    IN  CPSS_REG_VALUE_INFO_STC     *initDataListPtr,
    IN  GT_U32                      initDataListLen
)
{
    GT_STATUS rc;

    rc = cpssDxChHwPpStartInit(dev,
                                 fReloadEeprom,
                                 initDataListPtr,
                                 initDataListLen);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpStartInit", rc);
    return rc;
} /* appDemoDxHwPpStartInit */

/*******************************************************************************
* appDemoDxHwPpPhase2Init
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
GT_STATUS appDemoDxHwPpPhase2Init
(
    IN  GT_U8                       oldDev,
    IN  CPSS_PP_PHASE2_INIT_PARAMS  *appCpssPpPhase2ParamsPtr
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_DXCH_PP_PHASE2_INIT_INFO_STC   cpssPpPhase2Info;       /* Cpss phase 2 info */
    GT_HW_DEV_NUM                       hwDevNum;               /* HW device number */

    GT_PORT_NUM                         portNum;
    CPSS_INTERFACE_INFO_STC             physicalInfo;
    GT_U32                              numOfPhysicalPorts;
    GT_U32                              numOfEports;
    GT_U8                               devNum;




    if (NULL == appCpssPpPhase2ParamsPtr)
    {
        return GT_BAD_PTR;
    }

    /* Fill CPSS phase2 init structure */
    rc = appDemoConvert2DxChPhase2Info(appCpssPpPhase2ParamsPtr,
                                       &cpssPpPhase2Info);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoConvert2DxChPhase2Info", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssDxChHwPpPhase2Init(oldDev, &cpssPpPhase2Info);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpPhase2Init", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    devNum = cpssPpPhase2Info.newDevNum;
    /* In order to configure HW device ID different from SW device ID the following */
    /* logic is used: HW_device_ID = (SW_device_ID + appDemoHwDevNumOffset) modulo 32 */
    /* (this insures diffrent HW and SW device IDs since the HW device ID is 5 */
    /* bits length). */
    hwDevNum = ((cpssPpPhase2Info.newDevNum + appDemoHwDevNumOffset) & 0x1F);

    rc = cpssDxChCfgHwDevNumSet(devNum, hwDevNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* set all hwDevNum in E2Phy mapping table for all ePorts */
    /* Enable configuration of drop for ARP MAC SA mismatch due to check per port */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.eArch.supported == GT_TRUE)
    {
        /* Loop on the first 256 (num of physical ports , and CPU port (63)) entries
           of the table */
        numOfPhysicalPorts =
            PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum);

        for(portNum=0; portNum < numOfPhysicalPorts; portNum++)
        {
            rc = cpssDxChBrgEportToPhysicalPortTargetMappingTableGet(devNum,
                                                                     portNum,
                                                                     &physicalInfo);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgEportToPhysicalPortTargetMappingTableGet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* If <Target Is Trunk> == 0 && <Use VIDX> == 0 */
            if(physicalInfo.type == CPSS_INTERFACE_PORT_E)
            {
                physicalInfo.devPort.hwDevNum = hwDevNum;
                rc = cpssDxChBrgEportToPhysicalPortTargetMappingTableSet(devNum,
                                                                         portNum,
                                                                         &physicalInfo);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgEportToPhysicalPortTargetMappingTableSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* ARP MAC SA mismatch check per port configuration enabling */
                rc = cpssDxChBrgGenPortArpMacSaMismatchDropEnable(devNum,
                                                                  portNum,
                                                                  GT_TRUE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgGenPortArpMacSaMismatchDropEnable", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                /* set the EGF to filter traffic to ports that are 'link down'.
                   state that all ports are currently 'link down' (expect for 'CPU PORT')

                   see function : sip5_20_linkChange , which handles runtime 'link change' event.
                */
                rc = cpssDxChBrgEgrFltPortLinkEnableSet(devNum,
                    portNum,
                    (portNum != CPSS_CPU_PORT_NUM_CNS) ?
                        GT_TRUE :/*filter*/
                        GT_FALSE/*don't filter*/);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        /* Port Isolation is enabled if all three configurations are enabled:
           In the global TxQ registers, AND
           In the eVLAN egress entry, AND
           In the ePort entry

           For legacy purpose loop on all ePort and Trigger L2 & L3 Port
           Isolation filter for all ePorts */

        numOfEports =
            PRV_CPSS_DXCH_MAX_PORT_NUMBER_MAC(devNum);

        for(portNum=0; portNum < numOfEports; portNum++)
        {
            rc = cpssDxChNstPortIsolationModeSet(devNum,
                                                 portNum,
                                                 CPSS_DXCH_NST_PORT_ISOLATION_ALL_ENABLE_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNstPortIsolationModeSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* for legacy : enable per eport <Egress eVLAN Filtering Enable>
               because Until today there was no enable bit, egress VLAN filtering is
               always performed, subject to the global <BridgedUcEgressFilterEn>. */
            rc = cpssDxChBrgEgrFltVlanPortFilteringEnableSet(devNum,
                                                              portNum,
                                                              GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgEgrFltVlanPortFilteringEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return rc;
} /* appDemoDxHwPpPhase2Init */

/*******************************************************************************
* appDemoDxPpLogicalInit
*
* DESCRIPTION:
*      Implements logical initialization for specific device.
*      Device number is a part of system configuration parameters.
*
* INPUTS:
*      dev               -   device number
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
GT_STATUS appDemoDxPpLogicalInit
(
    IN  GT_U8                        dev,
    IN  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParams
)
{
    GT_STATUS                        rc = GT_OK;
    CPSS_DXCH_PP_CONFIG_INIT_STC     ppLogicalInfo;     /* CPSS format - DxCh info  */


    switch (PRV_CPSS_PP_MAC(dev)->devFamily)
    {
         case CPSS_PP_FAMILY_SALSA_E:
             /* convert core format to specifc CPSS format -- Salsa */
             /*appDemoCoreToCpssDxSalPpInfoConvert(logicalConfigParams,
                                              &tgPpLogicalInfo);*/

             /* call CPSS to do Pp logical init */
             /*rc = cpssDxSalCfgPpLogicalInit(dev, &tgPpLogicalInfo);*/
             rc = GT_FAIL;
             break;
         default:
             /* convert core format to specific CPSS format -- Dx */
             appDemoCoreToCpssDxChPpInfoConvert(logicalConfigParams,
                                            &ppLogicalInfo);

             /* call CPSS to do Pp logical init */
             rc = cpssDxChCfgPpLogicalInit(dev, &ppLogicalInfo);
             CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgPpLogicalInit", rc);
             break;
     }
     return rc;
} /* appDemoDxPpLogicalInit */

/*******************************************************************************
* appDemoDxLibrariesInit
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
GT_STATUS appDemoDxLibrariesInit
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
} /* appDemoDxLibrariesInit */

/*******************************************************************************
* appDemoDxTrafficEnable
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
GT_STATUS appDemoDxTrafficEnable
(
    IN  GT_U8   dev
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      port;
    GT_U32      devIdx; /* index to appDemoPpConfigList */

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }
#if 0
    if (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        if (systemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E)
        {
            /* Lion2 Data Integrity module initialization */
            rc = lion2AppDemoDataIntegrityInit(dev);
            if(GT_OK != rc)
            {
                return rc;
            }
        }
    }
#endif
    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    /* for given device id loop on ports */
    for (port = 0; port < (appDemoPpConfigList[devIdx].maxPortNumber); port++)
    {
        CPSS_ENABLER_PORT_SKIP_CHECK(dev,port);

        rc = cpssDxChPortEnableSet(dev, port, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortEnableSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /* enable device */
    rc = cpssDxChCfgDevEnable(dev, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgDevEnable", rc);

    return rc;
} /* appDemoDxTrafficEnable */

/*******************************************************************************
* appDemoDxPpGeneralInit
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
GT_STATUS appDemoDxPpGeneralInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS           rc = GT_OK;
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC  cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd; /* ports tagging command */
    GT_U32              port;           /* current port number      */
    GT_U16              vid;            /* default VLAN id          */
    GT_U32              mruSize, cpuMruSize;
    GT_BOOL             flowControlEnable;
    CPSS_PORT_MAC_TYPE_ENT  portMacType;
    GT_U32  devIdx; /* index to appDemoPpConfigList */
    GT_U32 value = 0;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    /***********************/
    /* FDB configuration   */
    /***********************/
    rc = appDemoDxChFdbInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChFdbInit", rc);
    /*********************************************************/
    /* Port Configuration                                    */
    /*********************************************************/
    /* Jambo frame support */
    mruSize = 1522; /* default */
    cpuMruSize = CPSS_DXCH_PORT_MAX_MRU_CNS; /* maximum */

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }
    for (port = 0; port < (appDemoPpConfigList[devIdx].maxPortNumber); port++)
    {
        CPSS_ENABLER_PORT_SKIP_CHECK(dev,port);

        /*  The next 2 calls are to set Port-Override-Tc-Enable */
        /* Set port trust mode */
        rc = cpssDxChCosPortQosTrustModeSet(dev, port, CPSS_QOS_PORT_TRUST_L2_L3_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCosPortQosTrustModeSet", rc);
        if (rc != GT_OK)
            return rc;

        /* set: Packet's DSCP is not remapped */
        rc = cpssDxChCosPortReMapDSCPSet(dev, port, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCosPortReMapDSCPSet", rc);
        if (rc != GT_OK)
            return rc;

        rc = cpssDxChPortMacTypeGet(dev, port, &portMacType);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortMacTypeGet", rc);
        if (rc != GT_OK)
            return rc;
        if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E)
        {

            if(portMacType < CPSS_PORT_MAC_TYPE_XG_E)
            {
                rc = cpssDxChPortDuplexModeSet(dev, port, CPSS_PORT_FULL_DUPLEX_E);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortDuplexModeSet", rc);
                if (rc != GT_OK)
                    return rc;

                if ((PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)&&
                     (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E))
                {/* in devices of Lion family half-duplex not supported in any port mode/speed */
                    rc = cpssDxChPortDuplexAutoNegEnableSet(dev, port, GT_TRUE);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortDuplexAutoNegEnableSet", rc);
                    if (rc != GT_OK)
                        return rc;
                }

                rc = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, GT_FALSE, GT_TRUE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortFlowCntrlAutoNegEnableSet", rc);
                if (rc != GT_OK)
                    return rc;

                rc = cpssDxChPortSpeedAutoNegEnableSet(dev, port, GT_TRUE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortSpeedAutoNegEnableSet", rc);
                if (rc != GT_OK)
                    return rc;
            }

            /* Disable Flow Control for revision 3 in DxCh2, DxCh3, XCAT. */
            if(appDemoPpConfigList[devIdx].flowControlDisable)
            {
                flowControlEnable = GT_FALSE;
            }
            else
            {
                flowControlEnable = GT_TRUE;
            }
            rc = cpssDxChPortFlowControlEnableSet(dev, port, flowControlEnable);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortFlowControlEnableSet", rc);
            if (rc != GT_OK)
                return rc;

            rc = cpssDxChPortPeriodicFcEnableSet(dev, port, GT_FALSE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPeriodicFcEnableSet", rc);
            if (rc != GT_OK)
                return rc;
        }
        rc = cpssDxChPortMruSet(dev, port, mruSize);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortMruSet", rc);
        if (rc != GT_OK)
            return rc;

        rc = cpssDxChPortCrcCheckEnableSet(dev, port, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortCrcCheckEnableSet", rc);
        if (rc != GT_OK)
            return rc;

        /* tx Watchdog supported on DxCh1, DxCh2, DxCh3 */
        if((PRV_CPSS_PP_MAC(dev)->devFamily <= CPSS_PP_FAMILY_CHEETAH3_E))
        {
            rc = cpssDxChPortTxWatchdogEnableSet(dev, port, GT_TRUE, 500);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxWatchdogEnableSet", rc);
            if (rc != GT_OK)
                return rc;
        }

        /* call cpss api function with UP == 1 */
        rc = cpssDxChPortDefaultUPSet(dev, port, 1);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortDefaultUPSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* No dedicated MAC for CPU port in E_ARCH */
    if(PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)
    {
        rc = cpssDxChPortMruSet(dev, CPSS_CPU_PORT_NUM_CNS, cpuMruSize);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortMruSet", rc);
        if (rc != GT_OK)
            return rc;
    }

    /*****************************************************/
    /* Cascade ports configuration                       */
    /*****************************************************/
    rc = prvCscdPortsInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCscdPortsInit", rc);
    if (rc != GT_OK)
       return rc;


    /****************************************************/
    /* XG ports additional board-specific configuration */
    /****************************************************/
    rc = prv10GPortsConfig(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prv10GPortsConfig", rc);
    if (rc != GT_OK)
       return rc;




    /*********************************************************/
    /* Default VLAN configuration: VLAN 1 contains all ports */
    /*********************************************************/

    /* default VLAN id is 1 */
    vid = 1;

    /* Fill Vlan info */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    /* default IP MC VIDX */
    cpssVlanInfo.unregIpmEVidx = 0xFFF;

    cpssVlanInfo.naMsgToCpuEn           = GT_TRUE;

    if(appDemoSysConfig.forceAutoLearn == GT_FALSE)
    {
        cpssVlanInfo.autoLearnDisable       = GT_TRUE; /* Disable auto learn on VLAN */
    }

    cpssVlanInfo.unkUcastCmd            = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv6McastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv4McastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregNonIpMcastCmd     = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregNonIpv4BcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv4BcastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.floodVidx              = 0xFFF;

    cpssVlanInfo.mirrToRxAnalyzerIndex = 0;
    cpssVlanInfo.mirrToTxAnalyzerEn = GT_FALSE;
    cpssVlanInfo.mirrToTxAnalyzerIndex = 0;
    cpssVlanInfo.fidValue = vid;
    cpssVlanInfo.unknownMacSaCmd = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.ipv4McBcMirrToAnalyzerEn = GT_FALSE;
    cpssVlanInfo.ipv4McBcMirrToAnalyzerIndex = 0;
    cpssVlanInfo.ipv6McMirrToAnalyzerEn = GT_FALSE;
    cpssVlanInfo.ipv6McMirrToAnalyzerIndex = 0;

    /* Fill ports and tagging members */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembers);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsTagging);
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    /* set all ports as VLAN members */
    for (port = 0; port < (appDemoPpConfigList[devIdx].maxPortNumber); port++)
    {
        CPSS_ENABLER_PORT_SKIP_CHECK(dev,port);

        /* set the port as member of vlan */
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers, port);

        /* Set port pvid */
        rc = cpssDxChBrgVlanPortVidSet(dev, port, CPSS_DIRECTION_INGRESS_E,vid);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanPortVidSet", rc);
        if(rc != GT_OK)
             return rc;

        portsTaggingCmd.portsCmd[port] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }

    /* Write default VLAN entry */
    rc = cpssDxChBrgVlanEntryWrite(dev, vid,
                                   &portsMembers,
                                   &portsTagging,
                                   &cpssVlanInfo,
                                   &portsTaggingCmd);

    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanEntryWrite", rc);
    if (rc != GT_OK)
       return rc;

    /*********************************************************/
    /* Default TTI configuration (xCat A1 and above only):   */
    /*   - Set TTI PCL ID for IPV4 lookup to 1               */
    /*   - Set TTI PCL ID for MPLS lookup to 2               */
    /*   - Set TTI PCL ID for ETH  lookup to 3               */
    /*   - Set TTI PCL ID for MIM  lookup to 4               */
    /*********************************************************/
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
    {
        rc = cpssDxChTtiPclIdSet(dev,CPSS_DXCH_TTI_KEY_IPV4_E,1);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdSet", rc);
        if (rc != GT_OK)
           return rc;
        rc = cpssDxChTtiPclIdSet(dev,CPSS_DXCH_TTI_KEY_MPLS_E,2);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdSet", rc);
        if (rc != GT_OK)
           return rc;
        rc = cpssDxChTtiPclIdSet(dev,CPSS_DXCH_TTI_KEY_ETH_E,3);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdSet", rc);
        if (rc != GT_OK)
           return rc;
        rc = cpssDxChTtiPclIdSet(dev,CPSS_DXCH_TTI_KEY_MIM_E,4);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdSet", rc);
        if (rc != GT_OK)
           return rc;
    }

    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
    {
        /* Mirror initialization -
          init Rx and Tx Global Analyzer indexes to be 0 for Ingress
          and 1 for Egress */
        /* special setting for tastBoot test */

        if((appDemoDbEntryGet("fastBootSetMirrorAnalyzerPort", &value) != GT_OK) || (value == 0))
        {
            rc = cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(dev,
                                                                 GT_TRUE, 1);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet(dev,
                                                                 GT_TRUE, 0);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* eArch device configuration */
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev))
        {
                 /* enable Rx mirroring by FDB DA lookup and set analyser index 0 */
                 rc = cpssDxChBrgFdbDaLookupAnalyzerIndexSet(dev, GT_TRUE, 0);
                 CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbDaLookupAnalyzerIndexSet", rc);
                 if (rc != GT_OK)
                 {
                     return rc;
                 }

                 /* enable Rx mirroring by FDB SA lookup and set analyser index 0 */
                 rc = cpssDxChBrgFdbSaLookupAnalyzerIndexSet(dev, GT_TRUE, 0);
                 CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbSaLookupAnalyzerIndexSet", rc);
                 if (rc != GT_OK)
                 {
                     return rc;
                 }
        }
    }

    return rc;
} /* appDemoDxPpGeneralInit */




/*******************************************************************************
 * Private function implementation
 ******************************************************************************/

/*******************************************************************************
* prvCscdPortsInit
*
* DESCRIPTION:
*       Configure cascade ports (if they are present)
*
* INPUTS:
*       dev                      -   The PP's device number.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function is called from appDemoDxPpGeneralInit
*
*******************************************************************************/
static GT_STATUS prvCscdPortsInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS rc = GT_OK;
    GT_U32                      ii,jj,mm;               /* Loop index.                  */
    GT_TRUNK_ID                 trunkId,trunkIds[3]={0,0,0};    /* the cascade trunkId */
    GT_U32                      trustDsaQosEnable = 0;
    CPSS_QOS_ENTRY_STC          portQosCfg;
    GT_U32                      policyEn;               /* Enable policy Engine for port */
    GT_U8                       targetDevNum;           /* the device to be reached via cascade port for analyzing */
    GT_HW_DEV_NUM               targetHwDevNum;          /* the HW num for device to be reached via cascade port for analyzing */
    GT_U8                       portNum;
    CPSS_CSCD_PORT_TYPE_ENT     cscdPortType;           /* Enum of cascade port or network port */
    CPSS_CSCD_LINK_TYPE_STC     cpssCascadeLink;
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT  currentSrcPortTrunkHashEn;
    GT_BOOL                     egressAttributesLocallyEn;
    CPSS_PORTS_BMP_STC          cscdTrunkBmp;/* bmp of ports members in the cascade trunk */
    GT_U32                      devIdx; /* index to appDemoPpConfigList */

    if (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_BAD_PARAM;
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    if (0 == appDemoPpConfigList[devIdx].numberOfCscdPorts)
    {
        return GT_OK;
    }

    if ((appDemoPpConfigList[devIdx].numberOfCscdPorts > CPSS_MAX_PORTS_NUM_CNS) ||
        (appDemoPpConfigList[devIdx].numberOfCscdTargetDevs > CPSS_CSCD_MAX_DEV_IN_CSCD_CNS))
    {
        return GT_BAD_PARAM;
    }

    /* 1. Initialize the cascade library */

    /* Set Dsa ingress filter bit (only in Cheetah) */
    rc = cpssDxChCscdDsaSrcDevFilterSet(dev, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDsaSrcDevFilterSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    for(ii = 0; ii < appDemoPpConfigList[devIdx].numberOfCscdPorts; ++ii)
    {
        /*2.1. Configure ports to be cascade ports*/

        portNum = appDemoPpConfigList[devIdx].cscdPortsArr[ii].portNum;
        cscdPortType = appDemoPpConfigList[devIdx].cscdPortsArr[ii].cscdPortType;

        rc = cpssDxChCscdPortTypeSet(dev, portNum,CPSS_PORT_DIRECTION_BOTH_E, cscdPortType);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdPortTypeSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        policyEn = 1; /* enable policy engine */
        if((CPSS_CSCD_PORT_DSA_MODE_1_WORD_E == cscdPortType)
           || (CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E == cscdPortType))
        {
            /* disable policy engine for cascading ports with extended DSA tag packets*/
            policyEn = (CPSS_CSCD_PORT_DSA_MODE_1_WORD_E == cscdPortType) ? 1 : 0;

            /* Enable Trust DSA Tag QoS profile for cascade port */
            trustDsaQosEnable = 1;
            /* Cascade port's QoS profile default settings */
            portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            portQosCfg.qosProfileId     = 64; /*#define GT_CORE_CHEETAH_TC_FIRST_ENTRY 64*/
        }
        else if(CPSS_CSCD_PORT_NETWORK_E == cscdPortType)
        {
            /* Disable Trust DSA Tag QoS profile for network port */
            trustDsaQosEnable = 0;
            /* Network port's QoS profile default settings */
            portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            portQosCfg.qosProfileId     = 64; /*#define GT_CORE_CHEETAH_TC_FIRST_ENTRY 64*/
        }
        else
        {
            return GT_BAD_PARAM;
        }

        /* write data to Ports VLAN and QoS Configuration Table, word0*/
        rc = cpssDxChCosTrustDsaTagQosModeSet(dev, portNum, trustDsaQosEnable);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCosTrustDsaTagQosModeSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* enable/disable policy */
        rc = cpssDxChPclPortIngressPolicyEnable(dev, portNum, policyEn);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclPortIngressPolicyEnable", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChCosPortQosConfigSet(dev, portNum, &portQosCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCosPortQosConfigSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* set the "send to CPU AU" from this port -- fix bug #27827 */
        rc = cpssDxChBrgFdbNaToCpuPerPortSet(dev, portNum, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbNaToCpuPerPortSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /*2.2.Board-specific cascade ports configuration */
        if(NULL != appDemoPpConfigList[devIdx].internalCscdPortConfigFuncPtr)
        {
            rc = appDemoPpConfigList[devIdx].internalCscdPortConfigFuncPtr(dev, portNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoPpConfigList[devIdx].internalCscdPortConfigFuncPtr", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }


    if(appDemoPpConfigList[devIdx].numberOfCscdTrunks)
    {
        if(appDemoPpConfigList[devIdx].numberOfCscdTrunks > 3)
        {
            rc = GT_FAIL;
            /* error -- more then 3 cascade trunks needed ? */
            CPSS_ENABLER_DBG_TRACE_RC_MAC("error 1 -- more then 3 cascade trunks needed ?", rc);
            return rc;
        }

        for(ii = 0; ii < appDemoPpConfigList[devIdx].numberOfCscdPorts; ++ii)
        {
            /*2.1. Configure ports to be cascade ports*/

            portNum = appDemoPpConfigList[devIdx].cscdPortsArr[ii].portNum;
            cscdPortType = appDemoPpConfigList[devIdx].cscdPortsArr[ii].cscdPortType;
            trunkId = appDemoPpConfigList[devIdx].cscdPortsArr[ii].trunkId;

            if(0xFFFF == trunkId || trunkId == 0)
            {
                continue;
            }

            /* check if this cascade trunk already built */
            for(jj = 0 ; jj < 3 ;jj++)
            {
                if(trunkId == trunkIds[jj])
                {
                    break;
                }

                if(0 == trunkIds[jj])
                {
                    break;
                }
            }/*jj*/

            if(jj == 3)
            {
                rc = GT_FAIL;
                /* error -- more then 3 cascade trunks needed ? */
                CPSS_ENABLER_DBG_TRACE_RC_MAC("error 2-- more then 3 cascade trunks needed ?", rc);
                return rc;
            }

            if(trunkId == trunkIds[jj])
            {
                /* this trunk already built */
                continue;
            }

            DBG_TRACE((" create on dev[%d] cascade trunk [%d] \n",dev,trunkId));

            trunkIds[jj] = trunkId;

            /* clear the local ports bmp for the new cascade trunk */
            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&cscdTrunkBmp);

            for(mm = 0; mm < appDemoPpConfigList[devIdx].numberOfCscdPorts; mm ++)
            {
                portNum = appDemoPpConfigList[devIdx].cscdPortsArr[mm].portNum;
                cscdPortType = appDemoPpConfigList[devIdx].cscdPortsArr[mm].cscdPortType;
                trunkId = appDemoPpConfigList[devIdx].cscdPortsArr[mm].trunkId;

                if(trunkId != trunkIds[jj])
                {
                    continue;
                }

                CPSS_PORTS_BMP_PORT_SET_MAC(&cscdTrunkBmp,portNum);
            }/*mm*/

            rc = cpssDxChTrunkCascadeTrunkPortsSet(dev,trunkIds[jj],&cscdTrunkBmp);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkCascadeTrunkPortsSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }/*ii*/
    }/* if cascade trunks */

    /* 4. Configure the cascade map table */
    for(ii = 0; ii < appDemoPpConfigList[devIdx].numberOfCscdTargetDevs; ++ii)
    {
        targetDevNum = appDemoPpConfigList[devIdx].cscdTargetDevsArr[ii].targetDevNum;

        if( NULL != prvCpssPpConfig[targetDevNum] )
        {
            rc = cpssDxChCfgHwDevNumGet(targetDevNum, &targetHwDevNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            targetHwDevNum = targetDevNum;
        }

        rc = cpssDxChCscdDevMapTableGet(dev,targetHwDevNum, 0, 0, &cpssCascadeLink,
                                        &currentSrcPortTrunkHashEn, &egressAttributesLocallyEn);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDevMapTableGet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        cpssCascadeLink.linkNum = appDemoPpConfigList[devIdx].cscdTargetDevsArr[ii].linkToTargetDev.linkNum;
        cpssCascadeLink.linkType = appDemoPpConfigList[devIdx].cscdTargetDevsArr[ii].linkToTargetDev.linkType;

        rc = cpssDxChCscdDevMapTableSet(dev,targetHwDevNum, 0, 0, &cpssCascadeLink,
                                        currentSrcPortTrunkHashEn, egressAttributesLocallyEn);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDevMapTableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* 5. Configure the port VLAN/VIDX membership */
    /*empty*/

    return rc;
}

/*******************************************************************************
* prv10GPortsConfig
*
* DESCRIPTION:
*       The function performs additional board-specific onfigure
*       for XG ports.
*
* INPUTS:
*       dev         - The PP's device number.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function is called from appDemoDxPpGeneralInit
*
*******************************************************************************/
static GT_STATUS prv10GPortsConfig
(
    IN  GT_U8   dev
)
{
    GT_STATUS   rc = GT_OK;

    GT_U32      ii;

    GT_U32  devIdx; /* index to appDemoPpConfigList */

    if (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_BAD_PARAM;
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    if((0 == appDemoPpConfigList[devIdx].numberOf10GPortsToConfigure) ||
       (NULL == appDemoPpConfigList[devIdx].internal10GPortConfigFuncPtr))
    {
        return GT_OK;
    }

    if (appDemoPpConfigList[devIdx].numberOf10GPortsToConfigure > CPSS_MAX_PORTS_NUM_CNS)
    {
        return GT_BAD_PARAM;
    }

    for(ii = 0; ii < appDemoPpConfigList[devIdx].numberOf10GPortsToConfigure; ++ii)
    {
        rc = appDemoPpConfigList[devIdx].internal10GPortConfigFuncPtr(dev,
                                      appDemoPpConfigList[devIdx].ports10GToConfigureArr[ii]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoPpConfigList[devIdx].internal10GPortConfigFuncPtr", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return rc;
}


/*******************************************************************************
* appDemoConvert2DxChPhase1Info
*
* DESCRIPTION:
*       Set CPSS info from the core format.
*
* INPUTS:
*       dev                      -   The PP's device number to be initialized.
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
static GT_STATUS appDemoConvert2DxChPhase1Info
(
    IN  GT_U8                               dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS          *appCpssPpPhase1ParamsPtr,
    OUT CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   *cpssPpPhase1InfoPtr
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32  ii;

    if ((NULL == appCpssPpPhase1ParamsPtr) ||
        (NULL == cpssPpPhase1InfoPtr))
    {
        return GT_BAD_PTR;
    }

    osMemSet(cpssPpPhase1InfoPtr, 0, sizeof(*cpssPpPhase1InfoPtr));

    cpssPpPhase1InfoPtr->devNum          = dev;
    cpssPpPhase1InfoPtr->busBaseAddr     = appCpssPpPhase1ParamsPtr->baseAddr;
    cpssPpPhase1InfoPtr->internalPciBase = appCpssPpPhase1ParamsPtr->internalPciBase;
    cpssPpPhase1InfoPtr->resetAndInitControllerBase =
                           appCpssPpPhase1ParamsPtr->resetAndInitControllerBase;
    cpssPpPhase1InfoPtr->hwAddr          = appCpssPpPhase1ParamsPtr->hwAddr;
    cpssPpPhase1InfoPtr->intVecNum       = appCpssPpPhase1ParamsPtr->intVecNum;
    cpssPpPhase1InfoPtr->intMask         = appCpssPpPhase1ParamsPtr->intMask;
    cpssPpPhase1InfoPtr->coreClock =
        (appCpssPpPhase1ParamsPtr->coreClk == APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS) ?
         CPSS_DXCH_AUTO_DETECT_CORE_CLOCK_CNS : appCpssPpPhase1ParamsPtr->coreClk;
    cpssPpPhase1InfoPtr->mngInterfaceType   = appCpssPpPhase1ParamsPtr->mngInterfaceType;
    cpssPpPhase1InfoPtr->ppHAState      = appCpssPpPhase1ParamsPtr->sysHAState;
    cpssPpPhase1InfoPtr->powerDownPortsBmp = appCpssPpPhase1ParamsPtr->powerDownPortsBmp;
    cpssPpPhase1InfoPtr->serdesRefClock = appCpssPpPhase1ParamsPtr->serdesRefClock;
    cpssPpPhase1InfoPtr->initSerdesDefaults = appCpssPpPhase1ParamsPtr->initSerdesDefaults;

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
            portGroupsPhase1InfoArr[ii].hwAddr = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].hwAddr;
            portGroupsPhase1InfoArr[ii].intVecNum = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].intVecNum;
            portGroupsPhase1InfoArr[ii].intMask = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].intMask;
        }
    }

    cpssPpPhase1InfoPtr->isExternalCpuConnected = appCpssPpPhase1ParamsPtr->isExternalCpuConnected;
    cpssPpPhase1InfoPtr->tcamParityCalcEnable = appCpssPpPhase1ParamsPtr->tcamParityCalcEnable;

    cpssPpPhase1InfoPtr->isrAddrCompletionRegionsBmp  =
        appCpssPpPhase1ParamsPtr->isrAddrCompletionRegionsBmp;
    cpssPpPhase1InfoPtr->appAddrCompletionRegionsBmp =
        appCpssPpPhase1ParamsPtr->appAddrCompletionRegionsBmp;

    cpssPpPhase1InfoPtr->enableLegacyVplsModeSupport = appCpssPpPhase1ParamsPtr->enableLegacyVplsModeSupport;

    /* save phase 1 parameters tp local DB */
    ppPhase1ParamsDb = *cpssPpPhase1InfoPtr;

    return rc;
} /* appDemoConvert2DxChPhase1Info */

/*******************************************************************************
* appDemoConvert2DxChPhase2Info
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
static GT_STATUS appDemoConvert2DxChPhase2Info
(
    IN  CPSS_PP_PHASE2_INIT_PARAMS          *appCpssPpPhase2ParamsPtr,
    OUT CPSS_DXCH_PP_PHASE2_INIT_INFO_STC   *cpssPpPhase2InfoPtr
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

    cpssPpPhase2InfoPtr->noTraffic2CPU              = appCpssPpPhase2ParamsPtr->noTraffic2CPU;
    cpssPpPhase2InfoPtr->netifSdmaPortGroupId       = appCpssPpPhase2ParamsPtr->netifSdmaPortGroupId;

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

    cpssPpPhase2InfoPtr->useDoubleAuq           = appCpssPpPhase2ParamsPtr->useDoubleAuq;
    cpssPpPhase2InfoPtr->fuqUseSeparate         = appCpssPpPhase2ParamsPtr->fuqUseSeparate;
    cpssPpPhase2InfoPtr->useSecondaryAuq        = appCpssPpPhase2ParamsPtr->useSecondaryAuq;

    osMemCpy(&cpssPpPhase2InfoPtr->fuqCfg,
             &appCpssPpPhase2ParamsPtr->fuqCfg,
             sizeof(appCpssPpPhase2ParamsPtr->fuqCfg));

    cpssPpPhase2InfoPtr->auMessageLength        = appCpssPpPhase2ParamsPtr->auMessageLength;

    cpssPpPhase2InfoPtr->useMultiNetIfSdma      = appCpssPpPhase2ParamsPtr->useMultiNetIfSdma;
    osMemCpy(&cpssPpPhase2InfoPtr->multiNetIfCfg,
             &appCpssPpPhase2ParamsPtr->multiNetIfCfg,
             sizeof(appCpssPpPhase2ParamsPtr->multiNetIfCfg));

    return rc;
} /* appDemoConvert2DxChPhase2Info */

/*******************************************************************************
* appDemoCoreToCpssDxChPpInfoConvert
*
* DESCRIPTION:
*       convert application format to CPSS format for DxCh "PP logical init"
*
* INPUTS:
*       ppConfPtr    - The PP logical configuration parameters
*
* OUTPUTS:
*      ppLogicalInfoPtr - CPSS format for ExMX device.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID appDemoCoreToCpssDxChPpInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC       *ppConfigPtr,
    OUT CPSS_DXCH_PP_CONFIG_INIT_STC  *ppLogicalInfoPtr
)
{

    /* ip routing mode*/
    ppLogicalInfoPtr->routingMode = ppConfigPtr->routingMode;

    /* LPM PBR entries */
    ppLogicalInfoPtr->maxNumOfPbrEntries = ppConfigPtr->maxNumOfPbrEntries;

} /* appDemoCoreToCpssDxChPpInfoConvert */


/*******************************************************************************
 * CPSS modules initialization routines
 ******************************************************************************/

#ifdef IMPL_PORT
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>

#define CHEETAH_CPU_PORT_PROFILE                   CPSS_PORT_RX_FC_PROFILE_1_E
#define CHEETAH_NET_GE_PORT_PROFILE                CPSS_PORT_RX_FC_PROFILE_2_E
#define CHEETAH_NET_10GE_PORT_PROFILE              CPSS_PORT_RX_FC_PROFILE_3_E
#define CHEETAH_CASCADING_PORT_PROFILE             CPSS_PORT_RX_FC_PROFILE_4_E

#define CHEETAH_GE_PORT_XON_DEFAULT                14 /* 28 Xon buffs per port   */
#define CHEETAH_GE_PORT_XOFF_DEFAULT               35 /* 70 Xoff buffs per port  */
#define CHEETAH_GE_PORT_RX_BUFF_LIMIT_DEFAULT      25 /* 100 buffers per port    */

#define CHEETAH_CPU_PORT_XON_DEFAULT               14 /* 28 Xon buffs per port   */
#define CHEETAH_CPU_PORT_XOFF_DEFAULT              35 /* 70 Xoff buffs per port  */
#define CHEETAH_CPU_PORT_RX_BUFF_LIMIT_DEFAULT     25 /* 100 buffers for CPU port */

#define CHEETAH_XG_PORT_XON_DEFAULT                25 /* 50 Xon buffs per port   */
#define CHEETAH_XG_PORT_XOFF_DEFAULT               85 /* 170 Xoff buffs per port */
#define CHEETAH_XG_PORT_RX_BUFF_LIMIT_DEFAULT      56 /* 224 buffers per port    */

static GT_STATUS dxChPortBufMgInit
(
    IN  GT_U8    dev
)
{
    GT_U8   port;
    GT_U32  macPort;
    GT_STATUS   rc;        /* return code  */
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile;
    GT_U32  buffLimit[4][3] = { /* 4 profiles : values for Xon,Xoff,rxBuff */
    /* Profile 0 - Set CPU ports profile */
        {CHEETAH_CPU_PORT_XON_DEFAULT,CHEETAH_CPU_PORT_XOFF_DEFAULT,CHEETAH_CPU_PORT_RX_BUFF_LIMIT_DEFAULT},
    /* Profile 1 - Set Giga ports profile */
        {CHEETAH_GE_PORT_XON_DEFAULT,CHEETAH_GE_PORT_XOFF_DEFAULT,CHEETAH_GE_PORT_RX_BUFF_LIMIT_DEFAULT},
    /* Profile 2 - Set XG and Cascade ports profile */
        {CHEETAH_XG_PORT_XON_DEFAULT,CHEETAH_XG_PORT_XOFF_DEFAULT,CHEETAH_XG_PORT_RX_BUFF_LIMIT_DEFAULT},
    /* Profile 3 - Set XG and Cascade ports profile */
        {CHEETAH_XG_PORT_XON_DEFAULT,CHEETAH_XG_PORT_XOFF_DEFAULT,CHEETAH_XG_PORT_RX_BUFF_LIMIT_DEFAULT}
    };

    GT_U32  devIdx; /* index to appDemoPpConfigList */

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

/* CPSS should config profile 0 and 1. */
    /* Set default settings for Flow Control Profiles: */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_FALSE)
    {
        for(profile = CPSS_PORT_RX_FC_PROFILE_1_E ; profile <= CPSS_PORT_RX_FC_PROFILE_4_E ; profile++)
        {
            rc = cpssDxChPortXonLimitSet(dev,profile,buffLimit[profile][0]);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortXonLimitSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            rc = cpssDxChPortXoffLimitSet(dev,profile,buffLimit[profile][1]);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortXoffLimitSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            rc = cpssDxChPortRxBufLimitSet(dev,profile,buffLimit[profile][2]);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortRxBufLimitSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }


        /* set the buffer limit profile association for network ports */
        for (port = 0; port < (appDemoPpConfigList[devIdx].maxPortNumber); port++)
        {
            CPSS_ENABLER_PORT_SKIP_CHECK(dev,port);

            PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(dev, port, macPort);

            profile = (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[macPort].portType >= PRV_CPSS_PORT_XG_E) ?
                      CHEETAH_NET_10GE_PORT_PROFILE :
                      CHEETAH_NET_GE_PORT_PROFILE;

            rc = cpssDxChPortRxFcProfileSet(dev, port, profile);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortRxFcProfileSet", rc);
            if (rc != GT_OK)
                return rc;
        }

        /* set the buffer limit profile association for CPU port */
        profile = CHEETAH_CPU_PORT_PROFILE;
        rc = cpssDxChPortRxFcProfileSet(dev,(GT_U8) CPSS_CPU_PORT_NUM_CNS, profile);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortRxFcProfileSet", rc);
        if (rc != GT_OK)
            return rc;

    }

    /* Enable HOL system mode for revision 3 in DxCh2, DxCh3, XCAT. */
    if(appDemoPpConfigList[devIdx].flowControlDisable)
    {
        rc = cpssDxChPortFcHolSysModeSet(dev, CPSS_DXCH_PORT_HOL_E);
    }
    else
    {
        rc = cpssDxChPortFcHolSysModeSet(dev, CPSS_DXCH_PORT_FC_E);

    }

    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortFcHolSysModeSet", rc);

    return rc;
}

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

    rc = cpssDxChPortStatInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortStatInit", rc);
    if( GT_OK != rc)
        return rc;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    rc = dxChPortBufMgInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("dxChPortBufMgInit", rc);
    if( GT_OK != rc)
        return rc;

    rc = cpssDxChPortTxInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxInit", rc);
    if(rc != GT_OK)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_BRIDGE
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>

static GT_STATUS dxChBrgFdbInit
(
    IN  GT_U8                       dev
)
{
    GT_STATUS rc;       /* return code  */
    GT_HW_DEV_NUM     hwDev;    /* HW device number */
    CPSS_MAC_HASH_FUNC_MODE_ENT hashMode;
    GT_U32            maxLookupLen;
    GT_U32   doFdbCuckoo;/* indication for all needed setups for implementing the Fdb Cuckoo algorithm */
    GT_U32  devIdx; /* index to appDemoPpConfigList */

    cpssDxChBrgFdbInit(dev);

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    rc = appDemoDbEntryGet("doFdbCuckoo", &doFdbCuckoo);

    if(GT_OK == rc && doFdbCuckoo == 1) /* should be set only to SIP5 devices (when needed) */
    {
        appDemoPpConfigList[devIdx].doFdbCuckoo = GT_TRUE;

        hashMode = CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E;
        maxLookupLen = 16;

        /* use max number of bits */
        rc = cpssDxChBrgFdb16BitFidHashEnableSet(dev,GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbHashModeSet", rc);
        if( GT_OK != rc)
            return rc;

        /* fill the 80 bits data with '16 bits' of the MAC. */
        rc = cpssDxChBrgFdbCrcHashUpperBitsModeSet(dev,
            CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_MAC_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbHashModeSet", rc);
        if( GT_OK != rc)
            return rc;

    }
    else
    {
        hashMode = CPSS_MAC_HASH_FUNC_XOR_E;/* use XOR hash function */
        maxLookupLen = 4;
    }

    /* Set lookUp mode and lookup length. */
    rc = cpssDxChBrgFdbHashModeSet(dev, hashMode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbHashModeSet", rc);
    if( GT_OK != rc)
        return rc;

    rc = cpssDxChBrgFdbMaxLookupLenSet(dev, maxLookupLen);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMaxLookupLenSet", rc);
    if( GT_OK != rc)
        return rc;


    /******************************/
    /* do specific cheetah coding */
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

    rc = cpssDxChCfgHwDevNumGet(dev, &hwDev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet", rc);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChBrgFdbActionActiveDevSet(dev, hwDev, 0x1F);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionActiveDevSet", rc);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable(dev, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable", rc);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable(dev, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable", rc);
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
    GT_U32      stpEntry[CPSS_DXCH_STG_ENTRY_SIZE_CNS];

    if(libInitParamsPtr->initBridge == GT_FALSE)
        return GT_OK;

    /* Init VLAN */
    rc = cpssDxChBrgVlanInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanInit", rc);
    if( GT_OK != rc)
        return rc;

    /** STP **/
    rc = cpssDxChBrgStpInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgStpInit", rc);
    if( GT_OK != rc)
        return rc;

    rc = dxChBrgFdbInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("dxChBrgFdbInit", rc);
    if( GT_OK != rc)
        return rc;


    rc = cpssDxChBrgMcInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgMcInit", rc);
    if( GT_OK != rc)
        return rc;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

#if 0 /* these actions are performed during CPSS under PSS Vlan init
 - should we perform them here?*/

    /* both vlan0,vlan1 in the same register */
    maxVlanRange = (GT_U16)corePpDevs[dev]->vlanData->maxNumOfVlans - 1;
    /* max num of VLAN's in Cheetah 0xFFE */
    if(maxVlanRange > GT_CORE_MAX_NUM_VLANS_CNS - 1)
        return GT_FAIL;

        /* config etherType0 */
    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    etherType = 0x8100;

    rc = cpssDxChBrgVlanEtherTypeSet(dev,
                                     etherTypeMode,
                                     etherType,
                                     maxVlanRange);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* config etherType1 */
    etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;

    rc = cpssDxChBrgVlanEtherTypeSet(dev,
                                     etherTypeMode,
                                     etherType,
                                     maxVlanRange);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* This bit indicates that cheetah operates as a 802.1D
       Bridge or as a 802.1Q Bridge
    1 = 802.1D Bridge
    0 = 802.1Q Bridge */

    /* like Salsa */
    brgMode = CPSS_BRG_MODE_802_1Q_E;

    rc = cpssDxChBrgVlanBridgingModeSet(dev, brgMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*
       reset word 0 in HW -- invalidate vlan 1
       since the HW start with reset value of valid vlan 1 with all ports
       members in it , we need to invalidate it
    */
    vlanId = 1;

    rc = cpssDxChBrgVlanEntryInvalidate(dev, vlanId);
    return rc;

#endif



    /* set first entry in STP like default entry */
    osMemSet(stpEntry, 0, sizeof(stpEntry));
    rc = cpssDxChBrgStpEntryWrite(dev, 0, stpEntry);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgStpEntryWrite", rc);
    if( GT_OK != rc)
    {
        /* the device not support STP !!! --> it's ok ,we have those ... */
        osPrintf("cpssDxChBrgStpEntryWrite : device[%d] not supported \n",dev);
        rc = GT_OK;
    }

    /*bind vlan 1 to stp 0 by default, Fixme later*/
    rc = cpssDxChBrgVlanToStpIdBind(dev, 1, 0);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanToStpIdBind", rc);
    if( GT_OK != rc)
    {
    	/* the device not support STP !!! --> it's ok ,we have those ... */
    	osPrintf("cpssDxChBrgVlanToStpIdBind : device[%d] not supported \n",dev);
    	rc = GT_OK;
    }

    return GT_OK;
}
#endif


#ifdef IMPL_MIRROR
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>

static GT_STATUS prvMirrorLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_HW_DEV_NUM     hwDev; /* HW device number */
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   interface;
    GT_U32 value;

    if(libInitParamsPtr->initMirror == GT_FALSE)
        return GT_OK;

    /*rc = cpssDxChMirrorInit(dev);
    if( GT_OK != rc)
        return rc;*/

    /* The following mirroring settings are needed because several RDE tests */
    /* assume that that default HW values of analyzers is port 0 SW device ID 0. */
    if( 0 == dev )
    {
        rc = cpssDxChCfgHwDevNumGet(0, &hwDev);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }

        if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            rc = cpssDxChMirrorRxAnalyzerPortSet(dev, 0, hwDev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorRxAnalyzerPortSet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }

            rc = cpssDxChMirrorTxAnalyzerPortSet(dev, 0, hwDev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorTxAnalyzerPortSet", rc);
        }
        else
        {
            interface.interface.type = CPSS_INTERFACE_PORT_E;
            interface.interface.devPort.hwDevNum = hwDev;
            interface.interface.devPort.portNum = 0;

            /* special setting for fastBoot test */
            if((appDemoDbEntryGet("fastBootSetMirrorAnalyzerPort", &value) == GT_OK) && (value != 0))
            {   
                    switch(PRV_CPSS_PP_MAC(dev)->devFamily )
                    {
                    case CPSS_PP_FAMILY_DXCH_LION2_E :
                        interface.interface.devPort.portNum = 8;
                        break;
                    default:
                        interface.interface.devPort.portNum = 4;
                        break;
                    }
              }

            rc = cpssDxChMirrorAnalyzerInterfaceSet(dev, 0, &interface);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorAnalyzerInterfaceSet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }

            rc = cpssDxChMirrorAnalyzerInterfaceSet(dev, 1, &interface);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorAnalyzerInterfaceSet", rc);
        }

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
}
#endif


#ifdef IMPL_NETIF
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>

static GT_STATUS prvNetIfLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 i;
    GT_U8 *buffArr[1] = {0};
    GT_U32 buffSize,buffSize4alloc;
    GT_U32  devIdx; /* index to appDemoPpConfigList */

    if(libInitParamsPtr->initNetworkIf == GT_FALSE)
        return GT_OK;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    if (appDemoPpConfigList[devIdx].cpuPortMode == CPSS_NET_CPU_PORT_MODE_SDMA_E)
    {
        rc = cpssDxChNetIfInit(dev);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfInit", rc);
        /* When CPSS_RX_BUFF_NO_ALLOC_E method is used application is responsible for RX buffer allocation */
        /* and attachment to descriptors.*/
        if (appDemoPpConfigList[devIdx].allocMethod == CPSS_RX_BUFF_NO_ALLOC_E)
        {

            buffSize = APP_DEMO_RX_BUFF_SIZE_DEF;
            buffSize4alloc = (buffSize + (APP_DEMO_RX_BUFF_ALLIGN_DEF -
                                          (buffSize % APP_DEMO_RX_BUFF_ALLIGN_DEF)));

            for (i=0; i < 8; i++)
            {

                while (1)
                {
                    if (buffArr[0] == NULL)
                    {

                        buffArr[0] = cpssOsCacheDmaMalloc(buffSize4alloc);
                        if (buffArr[0] == NULL)
                        {
                            return GT_FAIL;
                        }
                        /* we use 128 bytes aligment to restore the original buffer pointers when the buffers are freed again*/
                        if((((GT_UINTPTR)buffArr[0]) % APP_DEMO_RX_BUFF_ALLIGN_DEF) != 0)
                        {
                            buffArr[0] = (GT_U8*)(((GT_UINTPTR)buffArr[0]) +
                                               (APP_DEMO_RX_BUFF_ALLIGN_DEF - (((GT_UINTPTR)buffArr[0]) % APP_DEMO_RX_BUFF_ALLIGN_DEF)));
                        }
                    }

                    /* attach buffer */
                    rc = cpssDxChNetIfRxBufFreeWithSize(dev,(GT_U8)i,buffArr,&buffSize,1);
                    if ((rc != GT_OK) && (rc != GT_FULL))
                    {
                        return GT_FAIL;
                    }
                    if (rc == GT_FULL)
                    {
                        break;
                    }
                    buffArr[0] = NULL;
                }

                /* enable the queue after all buffers attached*/
                rc = cpssDxChNetIfSdmaRxQueueEnable(dev,(GT_U8)i,GT_TRUE);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }
    else if (appDemoPpConfigList[devIdx].cpuPortMode == CPSS_NET_CPU_PORT_MODE_MII_E)
    {
        CPSS_DXCH_NETIF_MII_INIT_STC miiInit;
        miiInit.numOfTxDesc = sysConfigParamsPtr->miiNumOfTxDesc;
        miiInit.txInternalBufBlockSize = sysConfigParamsPtr->miiTxInternalBufBlockSize;
        miiInit.txInternalBufBlockPtr = cpssOsCacheDmaMalloc(miiInit.txInternalBufBlockSize);
        if (miiInit.txInternalBufBlockPtr == NULL && miiInit.txInternalBufBlockSize != 0)
        {
            return GT_FAIL;
        }
        for (i = 0; i < CPSS_MAX_RX_QUEUE_CNS; i++)
        {
            miiInit.bufferPercentage[i] = sysConfigParamsPtr->miiBufferPercentage[i];
        }
        miiInit.rxBufSize = sysConfigParamsPtr->miiRxBufSize;
        miiInit.headerOffset = sysConfigParamsPtr->miiHeaderOffset;
        miiInit.rxBufBlockSize = sysConfigParamsPtr->miiRxBufBlockSize;
        miiInit.rxBufBlockPtr = cpssOsCacheDmaMalloc(miiInit.rxBufBlockSize);
        if (miiInit.rxBufBlockPtr == NULL && miiInit.rxBufBlockSize != 0)
        {
            return GT_FAIL;
        }
        if (miiInit.rxBufBlockPtr != NULL)
        {
            rc = cpssDxChNetIfMiiInit(dev,&miiInit);
        }
        else
        {
            rc = GT_OK;
        }
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfMiiInit", rc);
    }


    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif


#ifdef IMPL_PCL
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>

static GT_STATUS prvPclLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPcl == GT_FALSE)
        return GT_OK;

    rc = cpssDxChPclInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclInit", rc);
    if( GT_OK != rc)
        return rc;

    rc = cpssDxChPclIngressPolicyEnable(dev,GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclIngressPolicyEnable", rc);
    if( GT_OK != rc)
        return rc;

    if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* configure mixed key type for CH1 devices */
        rc = cpssDxChPclTcamRuleSizeModeSet(dev, GT_TRUE, CPSS_PCL_RULE_SIZE_EXT_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclTcamRuleSizeModeSet", rc);
        if( GT_OK != rc)
            return rc;
    }

    return GT_OK;
}
#endif


/* indication of bc2 b0 */
static GT_U32 isBobcat2B0 = 0;

#ifdef IMPL_TCAM
#include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>

#define GROUP_0             0
#define GROUP_1             1

#define HIT_NUM_0           0
#define HIT_NUM_1           1
#define HIT_NUM_2           2
#define HIT_NUM_3           3
#define END_OF_LIST_MAC     0xFFFFFFFF

typedef struct{
    GT_U32  floorNum;/*if END_OF_LIST_MAC --> not valid */
    GT_U32  bankIndex;
    GT_U32  hitNum;
}BANK_PARTITION_INFO_STC;


static GT_STATUS prvTcamLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_U32 value;
    CPSS_DXCH_TCAM_BLOCK_INFO_STC          floorInfoArr[CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS];
    GT_U32 baseFloorForTtiLookup0 = 0;
    GT_U32 baseFloorForTtiLookup1 = 0;
    GT_U32 baseFloorForTtiLookup2 = 0;
    GT_U32 baseFloorForTtiLookup3 = 0;
    GT_U32 tcamFloorsNum;
    GT_U32  ii,jj,index;
    static BANK_PARTITION_INFO_STC  bc2A0_ttiLookupArr[] = {
                    {6,0                                 ,HIT_NUM_0},
                    {6,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_0},

                    {7,0                                 ,HIT_NUM_0},
                    {7,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_0},

                    {8,0                                 ,HIT_NUM_0},
                    {8,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_0},

                    {9,0                                 ,HIT_NUM_0},
                    {9,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_0},

                    {10,0                                 ,HIT_NUM_1},
                    {10,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_1},

                    {11,0                                 ,HIT_NUM_1},
                    {11,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_1},

                    {END_OF_LIST_MAC,END_OF_LIST_MAC,END_OF_LIST_MAC}
                    };

    static BANK_PARTITION_INFO_STC  bc2B0_ttiLookupArr[] = {
                    {6,0                                 ,HIT_NUM_0},
                    {6,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_0},

                    {7,0                                 ,HIT_NUM_0},
                    {7,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_0},

                    {8,0                                 ,HIT_NUM_3},
                    {8,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_3},

                    {9,0                                 ,HIT_NUM_2},
                    {9,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_2},

                    {10,0                                 ,HIT_NUM_1},
                    {10,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_1},

                    {11,0                                 ,HIT_NUM_1},
                    {11,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_1},

                    {END_OF_LIST_MAC,END_OF_LIST_MAC,END_OF_LIST_MAC}
                    };

    static BANK_PARTITION_INFO_STC  bobk_ttiLookupArr[] = {
                    {3,0                                 ,HIT_NUM_0},
                    {3,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_0},

                    {4,0                                 ,HIT_NUM_2},
                    {4,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_3},

                    {5,0                                 ,HIT_NUM_1},
                    {5,CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS/2,HIT_NUM_1},

                    {END_OF_LIST_MAC,END_OF_LIST_MAC,END_OF_LIST_MAC}
                    };

    BANK_PARTITION_INFO_STC *ttiLookupInfoPtr = NULL;
    GT_BOOL ttiLookup3FromMidFloor = GT_FALSE;
    GT_BOOL ttiLookup0NonStandard = GT_FALSE;


    if(libInitParamsPtr->initTcam == GT_FALSE)
        return GT_OK;

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(dev))
    {
        /* set bobcat3 like in bobcat2 B0 */
        isBobcat2B0 = 1;
        ttiLookupInfoPtr = bc2B0_ttiLookupArr;
    }
    else
    if(PRV_CPSS_SIP_5_10_CHECK_MAC(dev))
    {
        isBobcat2B0 = 1;

        if(PRV_CPSS_SIP_5_15_CHECK_MAC(dev))
        {
            ttiLookupInfoPtr = bobk_ttiLookupArr;
        }
        else
        {
            ttiLookupInfoPtr = bc2B0_ttiLookupArr;
        }
    }
    else
    {
        ttiLookupInfoPtr = bc2A0_ttiLookupArr;
    }

    tcamFloorEntriesNum =
        CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS * CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_BANKS_CNS;

    rc = cpssDxChCfgTableNumEntriesGet(
        dev, CPSS_DXCH_CFG_TABLE_TTI_TCAM_E, &tcamEntriesNum);
    if (rc != GT_OK)
    {
        return GT_FAIL;
    }

    /* value must be a multiple of floor size */
    if (((tcamEntriesNum % tcamFloorEntriesNum) != 0) ||
        (tcamEntriesNum == 0) ||
        (tcamEntriesNum > (CPSS_DXCH_TCAM_MAX_NUM_FLOORS_CNS * tcamFloorEntriesNum)))
    {
        return GT_FAIL;
    }

    tcamFloorsNum = (tcamEntriesNum / tcamFloorEntriesNum);
    baseFloorForTtiLookup0 = ((tcamFloorsNum + 1) / 2);
    baseFloorForTtiLookup1 = (tcamFloorsNum - 1);
    if (baseFloorForTtiLookup1 > 10)
    {
        baseFloorForTtiLookup1 = 10;
    }

    if(isBobcat2B0)
    {
        if ((tcamFloorsNum/2) < 4) /* support bobk and some bc2 flavors */
        {
            /* we can not provide floor for each lookup TTI 0,1,2,3 */
            switch (tcamFloorsNum/2)
            {
                case 2:
                    /* need to allow support for 'half floor' and not full floor */
                    CPSS_TBD_BOOKMARK_BOBCAT2_BOBK
                    /* lookup 0,2 on floor 0 */
                    /* lookup 1,3 on floor 1 */
                    baseFloorForTtiLookup1 = baseFloorForTtiLookup0 + 1;
                    baseFloorForTtiLookup2 = baseFloorForTtiLookup0;
                    baseFloorForTtiLookup3 = baseFloorForTtiLookup1;
                    break;
                case 1:
                    /* need to allow support for 'half floor' and not full floor */
                    /* and probably to allow only 2 lookups and not 4 ! */
                    CPSS_TBD_BOOKMARK_BOBCAT2_BOBK
                    /* lookup 0,1,2,3 on floor 0 */
                    baseFloorForTtiLookup1 = baseFloorForTtiLookup0;
                    baseFloorForTtiLookup2 = baseFloorForTtiLookup0;
                    baseFloorForTtiLookup3 = baseFloorForTtiLookup0;
                    break;
                case 3:
                    /* need to allow support for 'half floor' and not full floor */
                    CPSS_TBD_BOOKMARK_BOBCAT2
                    /* lookup 0   on floor 0 */
                    /* lookup 2,3 on floor 1 */
                    /* lookup 1   on floor 2 */
                    baseFloorForTtiLookup1 = baseFloorForTtiLookup0 + 2;
                    baseFloorForTtiLookup2 = baseFloorForTtiLookup0 + 1;
                    baseFloorForTtiLookup3 = baseFloorForTtiLookup2;
                    ttiLookup3FromMidFloor = GT_TRUE;/* lookup 3 from mid floor */
                    break;
                default:  /*0*/
                    /* should not happen*/
                    break;
            }
        }
        else
        {
            baseFloorForTtiLookup2 = (baseFloorForTtiLookup1 - 1);
            baseFloorForTtiLookup3 = (baseFloorForTtiLookup1 - 2);
        }

        appDemoTcamTtiHit2RuleBaseIndexOffset = (baseFloorForTtiLookup2 * tcamFloorEntriesNum);
        appDemoTcamTtiHit3RuleBaseIndexOffset = (baseFloorForTtiLookup3 * tcamFloorEntriesNum);
        if(ttiLookup3FromMidFloor == GT_TRUE)
        {
            appDemoTcamTtiHit3RuleBaseIndexOffset += (CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS / 2);/*6*/
        }

    }

    appDemoTcamTtiHit0RuleBaseIndexOffset = (baseFloorForTtiLookup0 * tcamFloorEntriesNum);
    appDemoTcamTtiHit1RuleBaseIndexOffset = (baseFloorForTtiLookup1 * tcamFloorEntriesNum);


    /* init TCAM - Divide the TCAM into 2 groups:
       ingress policy 0, ingress policy 1, ingress policy 2 and egress policy belong to group 0; using floors 0-5.
       client tunnel termination belong to group 1; using floor 6-11.
       appDemoDbEntryAdd can change the division such that 6 will be replaced by a different value. */
    rc = cpssDxChTcamPortGroupClientGroupSet(dev,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,CPSS_DXCH_TCAM_IPCL_0_E,GROUP_0,GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamPortGroupClientGroupSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = cpssDxChTcamPortGroupClientGroupSet(dev,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,CPSS_DXCH_TCAM_IPCL_1_E,GROUP_0,GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamPortGroupClientGroupSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = cpssDxChTcamPortGroupClientGroupSet(dev,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,CPSS_DXCH_TCAM_IPCL_2_E,GROUP_0,GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamPortGroupClientGroupSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = cpssDxChTcamPortGroupClientGroupSet(dev,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,CPSS_DXCH_TCAM_EPCL_E,GROUP_0,GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamPortGroupClientGroupSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = cpssDxChTcamPortGroupClientGroupSet(dev,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,CPSS_DXCH_TCAM_TTI_E,GROUP_1,GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamPortGroupClientGroupSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (appDemoDbEntryGet("firstTtiTcamEntry", &value) == GT_OK)
    {
        /* value must be a multiple of floor size */
        if (((value % (CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS * CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_BANKS_CNS)) == 0) &&
            (value > 0) &&
            (value < CPSS_DXCH_TCAM_MAX_NUM_FLOORS_CNS * CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS * CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_BANKS_CNS))
        {
            baseFloorForTtiLookup0 = (value / (CPSS_DXCH_TCAM_MAX_NUM_BANKS_CNS * CPSS_DXCH_TCAM_MAX_NUM_RULES_PER_BANKS_CNS));
            appDemoTcamTtiHit0RuleBaseIndexOffset = value;

            if(baseFloorForTtiLookup0 != ttiLookupInfoPtr[0].floorNum /* 6 in bc2 , 3 in bobk */)
            {
                ttiLookup0NonStandard = GT_TRUE;
            }
        }
        else
        {
            return GT_FAIL;
        }
    }
    for (ii = 0; ii < CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS; ii++)
    {
        floorInfoArr[ii].group  = GROUP_0;
        floorInfoArr[ii].hitNum = HIT_NUM_0;
    }

    /* PCL : All clients are connected to hit num 0 */
    for (value = 0; value < baseFloorForTtiLookup0; value++){
        rc = cpssDxChTcamIndexRangeHitNumAndGroupSet(dev,value,floorInfoArr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamIndexRangeHitNumAndGroupSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    for (ii = 0; ii < CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS; ii++)
    {
        floorInfoArr[ii].group = GROUP_1;
    }

    appDemoTcamTtiHit0MaxNum = 0;
    appDemoTcamTtiHit1MaxNum = 0;
    appDemoTcamTtiHit2MaxNum = 0;
    appDemoTcamTtiHit3MaxNum = 0;

    /* TTI : All clients are connected to hit num 0..3 */
    for(ii = 0 ; ttiLookupInfoPtr[ii].floorNum != END_OF_LIST_MAC ; ii += 2)
    {
        value = ttiLookupInfoPtr[ii+0].floorNum;

        if(value >= tcamFloorsNum)
        {
            /* ignore */
            continue;
        }
        if (PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.tcam.numBanksForHitNumGranularity == 2)
        {
            floorInfoArr[0].hitNum=ttiLookupInfoPtr[ii+0].hitNum;
            floorInfoArr[1].hitNum=ttiLookupInfoPtr[ii+1].hitNum;
        }
        else
        {
            floorInfoArr[0].hitNum=ttiLookupInfoPtr[ii+0].hitNum;
            floorInfoArr[1].hitNum=ttiLookupInfoPtr[ii+0].hitNum;
            floorInfoArr[2].hitNum=ttiLookupInfoPtr[ii+0].hitNum;
            floorInfoArr[3].hitNum=ttiLookupInfoPtr[ii+1].hitNum;
            floorInfoArr[4].hitNum=ttiLookupInfoPtr[ii+1].hitNum;
            floorInfoArr[5].hitNum=ttiLookupInfoPtr[ii+1].hitNum;
        }

        /* support case that the baseFloorForTtiLookup0 was set by 'appDemoDbEntryGet'
           to value > 6 */
        if(baseFloorForTtiLookup0 > ttiLookupInfoPtr[0].floorNum)
        {
            if(baseFloorForTtiLookup0 > value/*current floor*/)
            {
                /* this floor is part of the 'PCL' */
                continue;
            }
            else if(baseFloorForTtiLookup0 == value)
            {
                /* use it for lookup 0 */
                for (jj=0; jj<CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS; jj++) {
                    floorInfoArr[jj].hitNum = HIT_NUM_0;
                }
            }
        }

        /* calculate the number of entries that each lookup can use */
        for (jj = 0 ; jj < 2 ; jj++)
        {
            if (PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.tcam.numBanksForHitNumGranularity == 2)
            {
                index = jj;
            }
            else
            {
                index = 3*jj;
            }
            if (floorInfoArr[index].hitNum == HIT_NUM_0)
            {
                /* another half floor for lookup 0 */
                appDemoTcamTtiHit0MaxNum += tcamFloorEntriesNum / 2;
            }
            else
            if(floorInfoArr[index].hitNum == HIT_NUM_1)
            {
                /* another half floor for lookup 1 */
                appDemoTcamTtiHit1MaxNum += tcamFloorEntriesNum / 2;
            }
            else
            if(floorInfoArr[index].hitNum == HIT_NUM_2)
            {
                /* another half floor for lookup 2 */
                appDemoTcamTtiHit2MaxNum += tcamFloorEntriesNum / 2;
            }
            else
            if(floorInfoArr[index].hitNum == HIT_NUM_3)
            {
                /* another half floor for lookup 3 */
                appDemoTcamTtiHit3MaxNum += tcamFloorEntriesNum / 2;
            }
        }

        rc = cpssDxChTcamIndexRangeHitNumAndGroupSet(dev,value,floorInfoArr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamIndexRangeHitNumAndGroupSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* support case that the baseFloorForTtiLookup0 was set by 'appDemoDbEntryGet'
       to value != 6 */
    if(ttiLookup0NonStandard == GT_TRUE)
    {
        /* dedicated floors for lookup 0 */
        for (ii = 0; ii < CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS; ii++)
        {
            floorInfoArr[ii].hitNum = HIT_NUM_0;
        }

        for(ii = baseFloorForTtiLookup0 ; ii < ttiLookupInfoPtr[0].floorNum ; ii ++)
        {
            value = ii;

            if(value >= tcamFloorsNum)
            {
                /* ignore */
                continue;
            }

            /* calculate the number of entries that each lookup can use */
            /* another floor for lookup 0 */
            appDemoTcamTtiHit0MaxNum += tcamFloorEntriesNum;

            rc = cpssDxChTcamIndexRangeHitNumAndGroupSet(dev,value,floorInfoArr);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamIndexRangeHitNumAndGroupSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }


    /* enable power up for the whole TCAM */

    rc = cpssDxChTcamActiveFloorsSet(dev, tcamFloorsNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTcamActiveFloorsSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}
#endif

#ifdef IMPL_PHY
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>

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

    rc = cpssDxChPhyPortSmiInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiInit", rc);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif


#ifdef IMPL_POLICER
#include <cpss/dxCh/dxChxGen/policer/cpssDxChPolicer.h>

static GT_STATUS prvPolicerLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPolicer == GT_FALSE)
    {
        /* check if Ingress stage #1 exists */
        if (PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.featureInfo.iplrSecondStageSupported
            == GT_TRUE)
        {

            /* Disable Policer Metering on Ingress stage #1 */
            rc = cpssDxCh3PolicerMeteringEnableSet(dev,
                                                   CPSS_DXCH_POLICER_STAGE_INGRESS_1_E,
                                                   GT_FALSE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh3PolicerMeteringEnableSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Disable Policer Counting on Ingress stage #1 */
            rc = cpssDxChPolicerCountingModeSet(dev,
                                                CPSS_DXCH_POLICER_STAGE_INGRESS_1_E,
                                                CPSS_DXCH_POLICER_COUNTING_DISABLE_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingModeSet", rc);
            return rc;
        }
        else
        {
            return GT_OK;
        }
    }

    rc = cpssDxChPolicerInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerInit", rc);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_TRUNK
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>

static GT_STATUS prvTrunkLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_U32      currMaxTrunks = sysConfigParamsPtr->numOfTrunks;

    if(libInitParamsPtr->initTrunk == GT_FALSE)
        return GT_OK;


    if(sysConfigParamsPtr->numOfTrunks > PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.trunksNum)
    {
        currMaxTrunks = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.trunksNum;
    }

    rc = cpssDxChTrunkInit(dev, currMaxTrunks ,
        CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E);

    while(rc == GT_OUT_OF_RANGE && currMaxTrunks)
    {
        currMaxTrunks--;
        rc = cpssDxChTrunkInit(dev, currMaxTrunks ,
            CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E);
    }

    if((rc != GT_OK) &&
       (currMaxTrunks != sysConfigParamsPtr->numOfTrunks))
    {
        osPrintf("cpssDxChTrunkInit : device[%d] support [%d] trunks \n",dev,currMaxTrunks);
    }

    return rc;
}
#endif

#ifdef IMPL_IP
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>

static GT_BOOL lpmDbInitialized = GT_FALSE;     /* keeps if LPM DB was created */
static GT_STATUS prvDxChIpLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS                                       rc;
    GT_U32                                          lpmDBId = 0;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    tcamLpmManagerCapcityCfg;
    CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT              lpmMemoryConfig;
    GT_U32                                          vrId = 0;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC                  vrConfigInfo;
    GT_U32                                          maxNumOfIpv4Prefixes;
    GT_U32                                          maxNumOfIpv6Prefixes;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT                shadowType;
    GT_U32                                          i;
    GT_U32                                          pclIdArray[1];

    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT          *defUcNextHopInfoPtr = NULL;
    GT_U32                          devIdx; /* index to appDemoPpConfigList */

    /********************************************************************/
    /* if lpm db is already created, all that is needed to do is to add */
    /* the device to the lpm db                                         */
    /********************************************************************/
    if (lpmDbInitialized == GT_TRUE)
    {
        rc = cpssDxChIpLpmDBDevListAdd(lpmDBId, &dev, 1);
        return  rc;
    }

    /*****************/
    /* create LPM DB */
    /*****************/

    /* set parameters */
    maxNumOfIpv4Prefixes = sysConfigParamsPtr->maxNumOfIpv4Prefixes;
    maxNumOfIpv6Prefixes = sysConfigParamsPtr->maxNumOfIpv6Prefixes;
    tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = maxNumOfIpv4Prefixes;
    tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 0; /* Ch+ doesn't support MC */
    tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = maxNumOfIpv6Prefixes;

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    /* The same init function is used for Ch+ and for xCat that is configured
       to work in policy based routing. Note that Ch+ PCL TCAM contains 2 columns
       and search order is column by column. xCat PCL TCAM contains 4 ip rules
       and search order is row by row */
    switch (appDemoPpConfigList[devIdx].devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:
        shadowType = CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E;
        indexesRange.firstIndex = 0;
        indexesRange.lastIndex = (maxNumOfIpv4Prefixes + maxNumOfIpv6Prefixes + 1) / 2 - 1;
        break;
    case CPSS_PP_FAMILY_DXCH_XCAT_E:
    case CPSS_PP_FAMILY_DXCH_XCAT3_E:
    case CPSS_PP_FAMILY_DXCH_XCAT2_E:
        shadowType = CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E;
        indexesRange.firstIndex = 0;
        indexesRange.lastIndex = (maxNumOfIpv4Prefixes + maxNumOfIpv6Prefixes + 3) / 4 - 1;
        break;
    default:
        return GT_NOT_IMPLEMENTED;
    }

    if (shadowType != CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E)
    {
        lpmMemoryConfig.tcamDbCfg.indexesRangePtr = &indexesRange;
        lpmMemoryConfig.tcamDbCfg.partitionEnable = GT_TRUE;
        lpmMemoryConfig.tcamDbCfg.tcamLpmManagerCapcityCfgPtr = &tcamLpmManagerCapcityCfg;
        lpmMemoryConfig.tcamDbCfg.tcamManagerHandlerPtr = NULL;
    }

    rc = cpssDxChIpLpmDBCreate(lpmDBId,shadowType,
                               CPSS_IP_PROTOCOL_IPV4V6_E,&lpmMemoryConfig);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* mark the lpm db as created */
    lpmDbInitialized = GT_TRUE;

    /* set default MC rules if required, for more detailes,
       refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet */
    if (sysConfigParamsPtr->usePolicyBasedDefaultMc == GT_TRUE)
    {
        /* CPSS enabler has no knowledge about the possible virtual routers
           the LPM DB will support; Therefore CPSS enalber will add only PCL ID
           that correspond to virtual router 0 that is added by CPSS enalber */
        pclIdArray[0] = 0;
        rc = cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet(lpmDBId,1,pclIdArray);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet", rc);
        if (rc != GT_OK)
            return rc;
    }

    /*******************************/
    /* add active device to LPM DB */
    /*******************************/
    rc = cpssDxChIpLpmDBDevListAdd(lpmDBId, &dev, 1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBDevListAdd", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /**********************/
    /* add virtual router */
    /**********************/

    /* set next hop to all 0 and change only values other than 0 */
    /* note that Ch+ doesn't support MC so default MC route entry is not set */
    cpssOsMemSet(&vrConfigInfo,0,sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));
    vrConfigInfo.supportIpv4Mc = GT_FALSE;
    vrConfigInfo.supportIpv6Mc= GT_FALSE;
    vrConfigInfo.supportIpv4Uc = GT_TRUE;
    vrConfigInfo.supportIpv6Uc= GT_TRUE;
    /* ipv4 uc and ipv6 uc default route are the same */
    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            defUcNextHopInfoPtr = &vrConfigInfo.defIpv4UcNextHopInfo;
        }
        else
        {
            defUcNextHopInfoPtr = &vrConfigInfo.defIpv6UcNextHopInfo;
        }
        defUcNextHopInfoPtr->pclIpUcAction.pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        defUcNextHopInfoPtr->pclIpUcAction.mirror.cpuCode = CPSS_NET_FIRST_USER_DEFINED_E;
        defUcNextHopInfoPtr->pclIpUcAction.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
        defUcNextHopInfoPtr->pclIpUcAction.vlan.ingress.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
        defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute.doIpUcRoute = GT_TRUE;

    }

    rc = cpssDxChIpLpmVirtualRouterAdd(lpmDBId,
                                       vrId,
                                       &vrConfigInfo);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmVirtualRouterAdd", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    return rc;
}

static GT_STATUS prvDxCh2Ch3IpLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS                                       rc = GT_OK;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    cpssLpmDbCapacity;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    cpssLpmDbRange;
    CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT              lpmMemoryConfig;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC                 ucRouteEntry;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC                 mcRouteEntry;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT                shadowType;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT          defUcLttEntry;
    CPSS_DXCH_IP_LTT_ENTRY_STC                      defMcLttEntry;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC                  vrConfigInfo;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC pclTcamCfg;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC routerTcamCfg;
    GT_BOOL                                         isCh2VrSupported;
    GT_U32                                          lpmDbId = 0;
    GT_U32                                          devIdx; /* index to appDemoPpConfigList */
    GT_U32                                          i=0;

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    switch (appDemoPpConfigList[devIdx].devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH2_E:
            shadowType = CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH3_E:
            shadowType = CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
        case CPSS_PP_FAMILY_DXCH_LION_E:
        case CPSS_PP_FAMILY_DXCH_LION2_E:
            shadowType = CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
            shadowType = CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E;
            break;
        default:
            return GT_NOT_IMPLEMENTED;
    }

    /* init default UC and MC entries */
    cpssOsMemSet(&defUcLttEntry,0,sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    cpssOsMemSet(&defMcLttEntry,0,sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

    defUcLttEntry.ipLttEntry.ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
    defUcLttEntry.ipLttEntry.numOfPaths               = 0;
    defUcLttEntry.ipLttEntry.routeEntryBaseIndex      = 0;
    if (shadowType == CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E)
    {
        defUcLttEntry.ipLttEntry.routeType            = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E;
    }
    else
    {
        defUcLttEntry.ipLttEntry.routeType            = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    }
    defUcLttEntry.ipLttEntry.sipSaCheckMismatchEnable = GT_FALSE;
    defUcLttEntry.ipLttEntry.ucRPFCheckEnable         = GT_FALSE;

    defMcLttEntry.ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
    defMcLttEntry.numOfPaths               = 0;
    defMcLttEntry.routeEntryBaseIndex      = 1;
    if (shadowType == CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E)
    {
        defMcLttEntry.routeType            = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E;
    }
    else
    {
        defMcLttEntry.routeType            = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
    }
    defMcLttEntry.sipSaCheckMismatchEnable = GT_FALSE;
    defMcLttEntry.ucRPFCheckEnable         = GT_FALSE;


    cpssOsMemSet(&vrConfigInfo,0,sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));

    if ((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E) &&
        (sysConfigParamsPtr->maxNumOfVirtualRouters > 1))
    {
        isCh2VrSupported = GT_TRUE;
    }
    else
    {
        isCh2VrSupported = GT_FALSE;
    }

    /* define protocolStack */
    if ((sysConfigParamsPtr->lpmDbSupportIpv4 == GT_FALSE) && (sysConfigParamsPtr->lpmDbSupportIpv6 == GT_FALSE))
        return GT_BAD_PARAM;
     if ((sysConfigParamsPtr->lpmDbSupportIpv4 == GT_TRUE) && (sysConfigParamsPtr->lpmDbSupportIpv6 == GT_TRUE))
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    else
        protocolStack = (sysConfigParamsPtr->lpmDbSupportIpv4 == GT_TRUE) ?
            CPSS_IP_PROTOCOL_IPV4_E : CPSS_IP_PROTOCOL_IPV6_E;

    /* fill vrConfigInfo structure */
    switch(protocolStack)
    {
    case CPSS_IP_PROTOCOL_IPV4_E:
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        cpssOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                     &defUcLttEntry.ipLttEntry,
                     sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        vrConfigInfo.supportIpv6Uc = GT_FALSE;
        vrConfigInfo.supportIpv6Mc = GT_FALSE;
        if(isCh2VrSupported == GT_TRUE)
        {
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
        }
        else
        {
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            cpssOsMemCpy(&vrConfigInfo.defIpv4McRouteLttEntry,
                         &defMcLttEntry,
                         sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
        break;
    case CPSS_IP_PROTOCOL_IPV6_E:
        vrConfigInfo.supportIpv4Uc = GT_FALSE;
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
        cpssOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                     &defUcLttEntry.ipLttEntry,
                     sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        vrConfigInfo.supportIpv4Mc = GT_FALSE;
        if(isCh2VrSupported == GT_TRUE)
        {
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
        }
        else
        {
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            cpssOsMemCpy(&vrConfigInfo.defIpv6McRouteLttEntry,
                         &defMcLttEntry,
                         sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }

        break;
    case CPSS_IP_PROTOCOL_IPV4V6_E:
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        cpssOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                     &defUcLttEntry.ipLttEntry,
                     sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
        cpssOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                     &defUcLttEntry.ipLttEntry,
                     sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        vrConfigInfo.supportIpv4Mc = (isCh2VrSupported == GT_TRUE)? GT_FALSE : GT_TRUE;
        vrConfigInfo.supportIpv6Mc = (isCh2VrSupported == GT_TRUE)? GT_FALSE : GT_TRUE;
        if(isCh2VrSupported == GT_TRUE)
        {
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
        }
        else
        {
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            cpssOsMemCpy(&vrConfigInfo.defIpv4McRouteLttEntry,
                         &defMcLttEntry,
                         sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
            cpssOsMemCpy(&vrConfigInfo.defIpv6McRouteLttEntry,
                         &defMcLttEntry,
                         sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }

        break;
    default:
        return GT_BAD_PARAM;
    }


    if( (vrConfigInfo.supportIpv4Uc == GT_TRUE) || (vrConfigInfo.supportIpv6Uc == GT_TRUE) )
    {
        /************************************************************/
        /* in this case ipv4/ipv6 unicast have common default route */
        /* set route entry 0 as default ipv4/6 unicast              */
        /************************************************************/
        cpssOsMemSet(&ucRouteEntry,0,sizeof(ucRouteEntry));
        ucRouteEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
        ucRouteEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        rc = cpssDxChIpUcRouteEntriesWrite(dev, 0, &ucRouteEntry, 1);
        if (rc != GT_OK)
        {
            if(rc == GT_OUT_OF_RANGE)
            {
                /* the device not support any IP (not router device)*/
                rc = GT_OK;

                osPrintf("cpssDxChIpUcRouteEntriesWrite : device[%d] not supported \n",dev);
            }

            return  rc;
        }
    }
    if (GT_FALSE == isCh2VrSupported)
    {
        /* Ch2 without Vr support case                 */
        if( (vrConfigInfo.supportIpv4Mc == GT_TRUE) || (vrConfigInfo.supportIpv6Mc == GT_TRUE) )
        {
            /*******************************************************/
            /* in this case ipv4/ipv6 mc have common default route */
            /* set route entry 1 as default ipv4/6 multicast       */
            /*******************************************************/
            cpssOsMemSet(&mcRouteEntry,0,sizeof(mcRouteEntry));
            mcRouteEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            mcRouteEntry.RPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            rc = cpssDxChIpMcRouteEntriesWrite(dev, 1, &mcRouteEntry);
            if (rc != GT_OK)
            {
                return  rc;
            }
        }
    }
    else
    {
        /* in case of Ch2 with Vr support - no mc support               */
        /* instead we will create another Vr with uc default            */

        /**************************************************************/
        /* in this case ipv4/ipv6 unicast have common default route   */
        /* set route entry 1 as default ipv4/6 unicast for this router*/
        /**************************************************************/
        rc = cpssDxChIpUcRouteEntriesWrite(dev, 1, &ucRouteEntry, 1);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpUcRouteEntriesWrite", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
    }

    /********************************************************************/
    /* if lpm db is already created, all that is needed to do is to add */
    /* the device to the lpm db                                         */
    /********************************************************************/
    if (lpmDbInitialized == GT_TRUE)
    {
        rc = cpssDxChIpLpmDBDevListAdd(lpmDbId, &dev, 1);
        if(rc == GT_BAD_PARAM)
        {
            osPrintf("cpssDxChIpLpmDBDevListAdd : device[%d] not supported \n",dev);
            /* the device not support the router tcam */
            rc = GT_OK;
        }

        return  rc;
    }

    /*****************/
    /* create LPM DB */
    /*****************/

    /* set parameters */

    cpssLpmDbCapacity.numOfIpv4Prefixes         = sysConfigParamsPtr->maxNumOfIpv4Prefixes;
    cpssLpmDbCapacity.numOfIpv6Prefixes         = sysConfigParamsPtr->maxNumOfIpv6Prefixes;
    cpssLpmDbCapacity.numOfIpv4McSourcePrefixes = sysConfigParamsPtr->maxNumOfIpv4McEntries;
    cpssLpmDbRange.firstIndex                   = sysConfigParamsPtr->lpmDbFirstTcamLine;
    cpssLpmDbRange.lastIndex                    = sysConfigParamsPtr->lpmDbLastTcamLine;

    if ((appDemoPpConfigList[devIdx].devFamily == CPSS_PP_FAMILY_CHEETAH2_E) && (cpssLpmDbRange.lastIndex > 1023))
    {
        cpssLpmDbRange.lastIndex = 1023;
    }

    /* now create the lpm db */
    if (GT_TRUE == isCh2VrSupported)
    {
        GT_U32 maxNumOfPclEntriesForIpPrefixes = sysConfigParamsPtr->maxNumOfPceForIpPrefixes;

        if (maxNumOfPclEntriesForIpPrefixes < 1)
        {
            maxNumOfPclEntriesForIpPrefixes = 1;
        }

        routerTcamCfg.tcamRange.firstIndex = cpssLpmDbRange.firstIndex;
        routerTcamCfg.tcamRange.lastIndex = cpssLpmDbRange.lastIndex;
        routerTcamCfg.prefixesCfg.numOfIpv4Prefixes = sysConfigParamsPtr->maxNumOfIpv4Prefixes;
        routerTcamCfg.prefixesCfg.numOfIpv6Prefixes = sysConfigParamsPtr->maxNumOfIpv6Prefixes;

        /* take PCL entries from bottom of the table */
        pclTcamCfg.tcamRange.lastIndex = 511;
        pclTcamCfg.tcamRange.firstIndex = pclTcamCfg.tcamRange.lastIndex + 1 -
                                          (maxNumOfPclEntriesForIpPrefixes + 1)/2;

        /* take all the reserved entries */
        pclTcamCfg.prefixesCfg.numOfIpv4Prefixes = maxNumOfPclEntriesForIpPrefixes/2;
        pclTcamCfg.prefixesCfg.numOfIpv6Prefixes = maxNumOfPclEntriesForIpPrefixes -
                                                   pclTcamCfg.prefixesCfg.numOfIpv4Prefixes;

        rc = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDbId,
                                                    protocolStack,
                                                    &pclTcamCfg,
                                                    &routerTcamCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBCheetah2VrSupportCreate", rc);
    }
    else
    {
        if (shadowType != CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E)
        {
            lpmMemoryConfig.tcamDbCfg.indexesRangePtr = &cpssLpmDbRange;
            lpmMemoryConfig.tcamDbCfg.partitionEnable = sysConfigParamsPtr->lpmDbPartitionEnable;
            lpmMemoryConfig.tcamDbCfg.tcamLpmManagerCapcityCfgPtr = &cpssLpmDbCapacity;
            lpmMemoryConfig.tcamDbCfg.tcamManagerHandlerPtr = NULL;
        }
        else
        {
            lpmMemoryConfig.ramDbCfg.numOfBlocks = sysConfigParamsPtr->lpmRamMemoryBlocksCfg.numOfBlocks;
            for (i=0;i<lpmMemoryConfig.ramDbCfg.numOfBlocks;i++)
            {
                lpmMemoryConfig.ramDbCfg.blocksSizeArray[i]= sysConfigParamsPtr->lpmRamMemoryBlocksCfg.blocksSizeArray[i];
            }

            switch (sysConfigParamsPtr->lpmRamMemoryBlocksCfg.blocksAllocationMethod)
            {
            case APP_DEMO_CPSS_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E:
                lpmMemoryConfig.ramDbCfg.blocksAllocationMethod = CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E;
                break;
            case APP_DEMO_CPSS_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E:
                lpmMemoryConfig.ramDbCfg.blocksAllocationMethod = CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E;
                break;
            default:
                return GT_BAD_PARAM;
            }
        }

        rc = cpssDxChIpLpmDBCreate(lpmDbId, shadowType,
                                   protocolStack, &lpmMemoryConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBCreate", rc);
    }
    if (rc != GT_OK)
    {
        return rc;
    }

    /* mark the lpm db as created */
    lpmDbInitialized = GT_TRUE;

    /*******************************/
    /* add active device to LPM DB */
    /*******************************/
    rc = cpssDxChIpLpmDBDevListAdd(lpmDbId, &dev, 1);
    if (rc != GT_OK)
    {
        if(rc == GT_BAD_PARAM)
        {
            /* the device not support the router tcam */
            osPrintf("cpssDxChIpLpmDBDevListAdd : device[%d] not supported \n",dev);
            rc = GT_OK;
        }

        return  rc;
    }

    /*************************/
    /* create virtual router */
    /*************************/
    if (GT_TRUE == isCh2VrSupported)
    {
        rc = cpssDxChIpLpmVirtualRouterAdd(lpmDbId,
                                           0,
                                           &vrConfigInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmVirtualRouterAdd", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = 1;
        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = 1;

        /*****************************************/
        /* This the Ch2 with Vr support case, so */
        /* create another virtual router in PCL  */
        /*****************************************/
        rc = cpssDxChIpLpmVirtualRouterAdd(lpmDbId,
                                           1,
                                           &vrConfigInfo);
    }
    else
    {
        rc = cpssDxChIpLpmVirtualRouterAdd(lpmDbId,
                                           0,
                                           &vrConfigInfo);
    }
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmVirtualRouterAdd", rc);
    return rc;
}

static GT_VOID *lpmDbSL;
/* the CPSS defined it */
extern GT_VOID *prvCpssDxChIplpmDbSL;

extern GT_VOID * prvCpssSlSearch
(
    IN GT_VOID        *ctrlPtr,
    IN GT_VOID        *dataPtr
);
/*******************************************************************************
* prvIpLpmLibReset_Ram
*
* DESCRIPTION:
*       IP LPM module reset - for LMP in RAM
*
* INPUTS:
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
GT_STATUS prvIpLpmLibReset_Ram()
{
    GT_STATUS       rc;                 /* return code */
    GT_U32          lpmDbId = 0;        /* LPM DB index */
    GT_UINTPTR      slIterator;         /* Skip List iterator */
    GT_U8           devListArray[PRV_CPSS_MAX_PP_DEVICES_CNS]; /* device list */
    GT_U32          numOfDevs;          /* device number (array size) */
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb; /* pointer to and temp instance of LMP DB entry */
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC    *ipShadowPtr; /* pointer to shadow entry */
    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*ipShadowsPtr; /* pointer to shadows DB */
    GT_U32          shadowIdx;          /* shadow index */
    GT_U32          i;                  /* loop iterator */

    while (1)
    {
        slIterator = 0;
        rc =  prvCpssDxChIpLpmDbIdGetNext(&lpmDbId,&slIterator);
        if (rc == GT_NO_MORE)
        {
            break;
        }
        if (rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssDxChIpLpmDbIdGetNext", rc);
            return rc;
        }

        lpmDbSL = prvCpssDxChIplpmDbSL;

        /* check parameters */
        tmpLpmDb.lpmDbId = lpmDbId;
        lpmDbPtr = prvCpssSlSearch(lpmDbSL,&tmpLpmDb);
        if (lpmDbPtr == NULL)
        {
            cpssOsPrintSync("prvIpLpmLibReset_Ram: lpmDbId = %d\r\n", lpmDbId);
            /* can't find the lpm DB */
            return GT_NOT_FOUND;
        }

        ipShadowsPtr = lpmDbPtr->shadow;

        for (shadowIdx = 0; shadowIdx < ipShadowsPtr->numOfShadowCfg; shadowIdx++)
        {
            ipShadowPtr = &ipShadowsPtr->shadowArray[shadowIdx];

            for(i = 0; i < ipShadowPtr->vrfTblSize; i++)
            {
                if(ipShadowPtr->vrRootBucketArray[i].valid == GT_TRUE)
                {
                    rc = cpssDxChIpLpmVirtualRouterDel(lpmDbId, i);
                    if (rc != GT_OK)
                    {
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmVirtualRouterDel", rc);
                        return rc;
                    }
                }
            }
        }

        numOfDevs = PRV_CPSS_MAX_PP_DEVICES_CNS;

        /* Get device list array from skip list for given LMP DB id */
        rc = cpssDxChIpLpmDBDevListGet(lpmDbId, &numOfDevs, devListArray);
        if (rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBDevListGet", rc);
            return rc;
        }

        /* Delete device list array for given LMP DB id */
        rc = cpssDxChIpLpmDBDevsListRemove(lpmDbId, devListArray , numOfDevs);
        if (rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBDevsListRemove", rc);
            return rc;
        }

        rc = cpssDxChIpLpmDBDelete(lpmDbId);
        if (rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBDelete", rc);
            return rc;
        }

    }

    lpmDbInitialized = GT_FALSE;

    return GT_OK;
}

static GT_STATUS prvIpLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_U32    devIdx; /* index to appDemoPpConfigList */

    if ((libInitParamsPtr->initIpv4 == GT_FALSE) && (libInitParamsPtr->initIpv6 == GT_FALSE))
    {
        return GT_OK;
    }

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    switch (appDemoPpConfigList[devIdx].devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:
    case CPSS_PP_FAMILY_DXCH_XCAT2_E:
        rc = prvDxChIpLibInit(dev,libInitParamsPtr,sysConfigParamsPtr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvDxChIpLibInit", rc);
        break;

    case CPSS_PP_FAMILY_CHEETAH2_E:
    case CPSS_PP_FAMILY_CHEETAH3_E:
        rc = prvDxCh2Ch3IpLibInit(dev,libInitParamsPtr,sysConfigParamsPtr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvDxCh2Ch3IpLibInit", rc);
        break;

    case CPSS_PP_FAMILY_DXCH_XCAT_E:
    case CPSS_PP_FAMILY_DXCH_XCAT3_E:
    case CPSS_PP_FAMILY_DXCH_LION_E:
    case CPSS_PP_FAMILY_DXCH_LION2_E:
    case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
    case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
    case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
        /* if policy based routing, use same initialization as Ch+,
           if not use same initialization as Ch2,Ch3 */
        if (sysConfigParamsPtr->usePolicyBasedRouting == GT_TRUE)
        {
            rc = prvDxChIpLibInit(dev,libInitParamsPtr,sysConfigParamsPtr);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvDxChIpLibInit", rc);
        }

        else
        {
            rc = prvDxCh2Ch3IpLibInit(dev,libInitParamsPtr,sysConfigParamsPtr);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvDxCh2Ch3IpLibInit", rc);
        }
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
    }

    return rc;
}

#endif


/*******************************************************************************
* prvFdbActionDelete
*
* DESCRIPTION:
*       Deletes all addresses from FDB table.
*
*
* INPUTS:
*       dev            - physical device number
*
* OUTPUTS:
*       actDevPtr      - pointer to old action device number
*       actDevMaskPtr  - pointer to old action device mask
*       triggerModePtr - pointer to old trigger mode
*       modePtr        - pointer to old action mode
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvFdbActionDelete
(
    IN  GT_U8                       dev,
    OUT GT_U32                      *actDevPtr,
    OUT GT_U32                      *actDevMaskPtr,
    OUT CPSS_MAC_ACTION_MODE_ENT    *triggerModePtr,
    OUT CPSS_FDB_ACTION_MODE_ENT    *modePtr
)
{
    GT_STATUS rc;


    /* save Mac Action action mode */
    rc = cpssDxChBrgFdbActionModeGet(dev, modePtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionModeGet", rc);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* save Mac Action trigger mode */
    rc = cpssDxChBrgFdbMacTriggerModeGet(dev, triggerModePtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMacTriggerModeGet", rc);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* save mac action device parameters */
    rc = cpssDxChBrgFdbActionActiveDevGet(dev,actDevPtr,actDevMaskPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionActiveDevGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* disable actions to be done on the entire MAC table
       before change active configuration */
    rc = cpssDxChBrgFdbActionsEnableSet(dev, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionsEnableSet", rc);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }


    /* delete all entries regardless of device number */
    rc = cpssDxChBrgFdbActionActiveDevSet(dev,0, 0);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionActiveDevSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* enable actions, action mode, triggered mode, trigger the action */
    rc = cpssDxChBrgFdbTrigActionStart(dev, CPSS_FDB_ACTION_DELETING_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbTrigActionStart", rc);

    return rc;
}


/*******************************************************************************
* appDemoDxChFdbFlush
*
* DESCRIPTION:
*       Deletes all addresses from FDB table.
*
*
* INPUTS:
*       dev            - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxChFdbFlush
(
    IN GT_U8   dev
)
{
    GT_STATUS                 rc;
    GT_BOOL                   actionCompleted;      /* Action Trigger Flag         */
    GT_U32                    loopCounter;          /* Counter for busy wait loops */
    /* old value of AA TA messages to CPU enabler */
    GT_BOOL                   aaTaMessegesToCpuEnable;
    GT_U32                    actDev;               /* old action device number */
    GT_U32                    actDevMask;           /* old action device mask */
    CPSS_MAC_ACTION_MODE_ENT  triggerMode;          /* old trigger mode */
    CPSS_FDB_ACTION_MODE_ENT  mode;                 /* old action mode */


    /* enable delete of static entries */
    rc = cpssDxChBrgFdbStaticDelEnable(dev, CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbStaticDelEnable", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* save old value of AA TA messages to CPU enabler */
    rc = cpssDxChBrgFdbAAandTAToCpuGet(dev, &aaTaMessegesToCpuEnable);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(aaTaMessegesToCpuEnable == GT_TRUE)
    {


        /* disable AA and TA messages to CPU */
        rc = cpssDxChBrgFdbAAandTAToCpuSet(dev, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

    }

    rc = prvFdbActionDelete(dev, &actDev, &actDevMask, &triggerMode, &mode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvFdbActionDelete", rc);

    if(rc != GT_OK)
    {
        return rc;
    }

    actionCompleted = GT_FALSE;
    loopCounter = 0;

    /* busy wait for Action Trigger Status */
    while (!actionCompleted)
    {
        rc = cpssDxChBrgFdbTrigActionStatusGet(dev, &actionCompleted);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbTrigActionStatusGet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        loopCounter ++;

        /* check if loop counter has reached 100000000 */
        if (loopCounter >= 100000000)
        {
            return GT_TIMEOUT;
        }
        osTimerWkAfter(1);

    }

    if(aaTaMessegesToCpuEnable == GT_TRUE)
    {
        /* restore AA and TA messages to CPU */
        rc = cpssDxChBrgFdbAAandTAToCpuSet(dev, aaTaMessegesToCpuEnable);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

    }


    /* restore active device parameters */
    rc = cpssDxChBrgFdbActionActiveDevSet(dev,
                                          actDev,
                                          actDevMask);

    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionActiveDevSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* disable delete of static entries */
    rc = cpssDxChBrgFdbStaticDelEnable(dev, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbStaticDelEnable", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* restore Trigger mode - it should be last operation because
       it may start auto aging. All active config should be restored
       before start of auto aging */
    rc = cpssDxChBrgFdbMacTriggerModeSet(dev, triggerMode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMacTriggerModeSet", rc);

    return rc;

}


/*******************************************************************************
* cpuTx
*
* DESCRIPTION:
*       Local function used by workaround for SGMII 2.5GB to send packet from CPU
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum     - physical device number
*       portsNum   - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NO_RESOURCE  - memory resource not available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS cpuTx(GT_U8 port, GT_BOOL sync)
{
    CPSS_DXCH_NET_TX_PARAMS_STC pcktParams;
    GT_U8                       *buffList[1];
    GT_U32                      buffLenList[1];
    GT_U32                      numOfBufs;
    CPSS_UNI_EV_CAUSE_ENT       uniEventArr[1];
    GT_UINTPTR                  txEventHndl;
    GT_U32                      evBitmapArr[8];
    GT_STATUS                   rc;

    uniEventArr[0] = CPSS_PP_TX_BUFFER_QUEUE_E;
    cpssEventBind(uniEventArr, 1, &txEventHndl);

    osMemSet(&pcktParams, 0, sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
    pcktParams.sdmaInfo.txQueue = 0;
    pcktParams.sdmaInfo.recalcCrc = 1;
    pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;
    pcktParams.sdmaInfo.evReqHndl = txEventHndl;
    pcktParams.packetIsTagged = GT_FALSE;
    pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
    /* use here HW device number! */

    pcktParams.dsaParam.dsaInfo.fromCpu.srcHwDev = PRV_CPSS_HW_DEV_NUM_MAC(0);
    pcktParams.dsaParam.dsaInfo.fromCpu.cascadeControl = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.fromCpu.tc = 0;
    pcktParams.dsaParam.dsaInfo.fromCpu.dp = 0;
    pcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.fromCpu.srcId = 0;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(0);
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = port;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = 1;
    pcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU = GT_FALSE;
    pcktParams.dsaParam.commonParams.cfiBit = 0;
    pcktParams.dsaParam.commonParams.vid = 1;
    pcktParams.dsaParam.commonParams.vpt = 0;
    pcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT;
    pcktParams.dsaParam.commonParams.dropOnSource = GT_FALSE;
    pcktParams.dsaParam.commonParams.packetIsLooped = GT_FALSE;

    numOfBufs = 1;
    buffLenList[0] = 64;
    buffList[0] = osMalloc(buffLenList[0]*sizeof(GT_U8));
    buffList[0][0] = 0;
    buffList[0][1] = 0x0;
    buffList[0][2] = 0x22;
    buffList[0][3] = 0x22;
    buffList[0][4] = 0x22;
    buffList[0][5] = 0x22;
    buffList[0][6] = 0;
    buffList[0][7] = 0x0;
    buffList[0][8] = 0x0;
    buffList[0][9] = 0x0;
    buffList[0][10] = 0x0;
    buffList[0][11] = 0x2;
    rc = cpssDxChNetIfSdmaTxQueueEnable(0, 0, GT_TRUE);

    if(sync == GT_TRUE)
    {
        rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&pcktParams,buffList, buffLenList, numOfBufs);
    }
    else
    {
        rc = cpssDxChNetIfSdmaTxPacketSend(0,&pcktParams,buffList, buffLenList, numOfBufs);
        if(rc != GT_OK)
        {
            osFree(buffList[0]);
            return rc;
        }

        rc = cpssEventSelect(txEventHndl, NULL, evBitmapArr, 8);
        if(rc != GT_OK)
        {
            osFree(buffList[0]);
            return rc;
        }
    }

    osFree(buffList[0]);

    return rc;
}

/*******************************************************************************
* appDemoDxChXcatSgmii2_5GbNetworkPortWa
*
* DESCRIPTION:
*       The workaround for SGMII 2.5GB
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum     - physical device number
*       portsBmp   - physical ports bitmap
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NO_RESOURCE  - memory resource not available.
* COMMENTS:
*       This is historical function and its usage discouraged,
*           application should use cpssDxChHwPpImplementWaInit with
*           CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_CFG_E
*           CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_NETWORKP_CFG_E
*           CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_STACKP_CFG_E,
*           like in example in
*           afterInitBoardConfig() in gtRd-xCat-24GE-4HGS.c
*
*******************************************************************************/
GT_STATUS appDemoDxChXcatSgmii2_5GbNetworkPortWa
(
    GT_U8   devNum,
    GT_U32  portsBmp
)
{
    GT_STATUS   rc = GT_OK;         /* return status */
    GT_U8       port;               /* number of port to run WA on */
    GT_BOOL     forceLinkPassState; /* current force link pass definition on port */
    GT_U32      devIdx; /* index to appDemoPpConfigList */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    rc = appDemoDevIdxGet(devNum, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    /* configure SGMII 1G */
    for (port = 0; port < (appDemoPpConfigList[devIdx].maxPortNumber); port++)
    {
        CPSS_ENABLER_PORT_SKIP_CHECK(devNum,port);

        if((portsBmp & (1<<port)) == 0)
            continue;

        rc = cpssDxChPortInterfaceModeSet(devNum, port, CPSS_PORT_INTERFACE_MODE_SGMII_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSpeedAutoNegEnableSet(devNum, port, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSpeedSet(devNum, port, CPSS_PORT_SPEED_1000_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortDuplexAutoNegEnableSet(devNum, port, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortDuplexModeSet(devNum, port, CPSS_PORT_FULL_DUPLEX_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortFlowCntrlAutoNegEnableSet(devNum, port, GT_FALSE, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortFlowControlEnableSet(devNum, port, CPSS_PORT_FLOW_CONTROL_DISABLE_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSerdesPowerStatusSet(devNum, port,
                                              CPSS_PORT_DIRECTION_BOTH_E,
                                              0x1, GT_TRUE);
        if (rc!= GT_OK)
        {
            return rc;
        }

        if((rc = cpssDxChPortForceLinkPassEnableGet(devNum,port,&forceLinkPassState)) != GT_OK)
            return rc;

        if(GT_FALSE == forceLinkPassState)
        {/* force link up to cause packet to pass whole eggress pipe */
            if((rc = cpssDxChPortForceLinkPassEnableSet(devNum,port,GT_TRUE)) != GT_OK)
                return rc;
        }

        rc = cpuTx(port, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(GT_FALSE == forceLinkPassState)
        {/* restore original force link pass configuration */
            if((rc = cpssDxChPortForceLinkPassEnableSet(devNum,port,forceLinkPassState)) != GT_OK)
                return rc;
        }
    }

    /* configure 2.5G */
    for (port = 0; port < (appDemoPpConfigList[devIdx].maxPortNumber); port++)
    {
        CPSS_ENABLER_PORT_SKIP_CHECK(devNum,port);

        if((portsBmp & (1<<port)) == 0)
            continue;

        rc = cpssDxChPortInterfaceModeSet(devNum, port, CPSS_PORT_INTERFACE_MODE_SGMII_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSpeedSet(devNum, port, CPSS_PORT_SPEED_2500_E);
        if (rc!= GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSerdesPowerStatusSet(devNum, port,
                                              CPSS_PORT_DIRECTION_BOTH_E,
                                              0x1, GT_TRUE);
        if (rc!= GT_OK)
        {
            return rc;
        }
    }

    return rc;
}


/*******************************************************************************
* appDemoDxChPrbsTxRxLaneSet
*
* DESCRIPTION:
*      Enable/disable PRBS signal TX and Rx on required serdes lane
*
* INPUTS:
*       devNum  - device number
*       portNum - global port number
*       laneBmp - bitmap of serdes lanes of portNum, where to generate PRBS signal
*       mode    - type of PRBS signal (not relevant if enable == GT_FALSE)
*                 Now supported:
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS15_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS31_E
*       enable  -   GT_TRUE - start PRBS TX/RX,
*                   GT_FALSE - stop PRBS TX/RX.
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
*       There is no option in HW to configure just TX or RX only both at once.
*
*       Marvell recommends running the following PRBS type per interface type:
*           �SERDES using 8/10 bit encoding (DHX,QSGMII,HGS4,HGS,XAUI,QX,
*                                                 SGMII 2.5,SGMII,100FX): PRBS7
*           �RXAUI: PRBS15
*           �SERDES using 64/66 bit encoding (XLG): PRBS31
*
*******************************************************************************/
GT_STATUS appDemoDxChPrbsTxRxLaneSet
(
    IN   GT_U8                            devNum,
    IN   GT_PHYSICAL_PORT_NUM             portNum,
    IN   GT_U32                           laneBmp,
    IN   CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode,
    IN   GT_BOOL                          enable
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;  /* iterator */

    for(i = 0; i < 32; i++)
    {
        if(laneBmp & (1<<i))
        {
            if(GT_TRUE == enable)
            {
                rc = cpssDxChDiagPrbsSerdesTransmitModeSet(devNum,portNum,i,mode);
                if(rc != GT_OK)
                    return rc;
            }

            rc = cpssDxChDiagPrbsSerdesTestEnableSet(devNum,portNum,i,enable);
            if(rc != GT_OK)
                return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChPrbsRxTuningRun
*
* DESCRIPTION:
*      External API for initialization of one port of lioncub
*
* INPUTS:
*       devNum  - device number
*       portNum - global port number
*       laneBmp - bitmap of serdes lanes of portNum, where to generate PRBS signal
*       mode    - type of PRBS signal (not relevant if enable == GT_FALSE) [5-8]
*                 Now supported:
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS15_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS31_E
*       prbsTime - wait time till PRBS test finish (set 0 for CPSS default)
*       dbgPrintEn  -   GT_TRUE - enable report print during run,
*                       GT_FALSE - disable report print.
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
*       Marvell recommends running the following PRBS type per interface type:
*           �SERDES using 8/10 bit encoding (DHX,QSGMII,HGS4,HGS,XAUI,QX,
*                                                 SGMII 2.5,SGMII,100FX): PRBS7
*           �RXAUI: PRBS15
*           �SERDES using 64/66 bit encoding (XLG): PRBS31
*
*******************************************************************************/
GT_STATUS appDemoDxChPrbsRxTuningRun
(
    IN   GT_U8                            devNum,
    IN   GT_PHYSICAL_PORT_NUM             portNum,
    IN   GT_U32                           laneBmp,
    IN   CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode,
    IN   GT_U32                           prbsTime,
    IN   GT_BOOL                          dbgPrintEn
)
{
    GT_STATUS   rc;         /* return code */
    CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC *portLaneArrPtr;
        /* array of (port,laneNum) pairs, where to run the test */
    GT_U32      portLaneArrLength;  /* length of (port,laneNum) array */
    GT_U32      i,j;          /* iterator */
    CPSS_DXCH_PORT_SERDES_TUNE_STC *optResultArrPtr;/* array of tuning result values */
    GT_U32      portGroupId;                        /* port group number */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;    /* speed of serdes */
    GT_U32      firstSerdes,
                numOfSerdesLanes;
    CPSS_DXCH_PORT_SERDES_TUNE_STC tuneValues;

    rc = cpssDxChDiagSerdesTuningTracePrintEnable(devNum,dbgPrintEn);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChDiagSerdesTuningSystemInit(devNum);
    if(rc != GT_OK)
        return rc;

    for(i = 0, portLaneArrLength=0; i < 32; i++)
    {
        if(laneBmp & (1<<i))
        {
            portLaneArrLength++;
        }
    }

    if(NULL == (portLaneArrPtr = (CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC*)
                                    cpssOsMalloc(portLaneArrLength*
                                        sizeof(CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC))))
    {
        return GT_OUT_OF_CPU_MEM;
    }

    if(NULL == (optResultArrPtr = (CPSS_DXCH_PORT_SERDES_TUNE_STC*)
                                    cpssOsMalloc(portLaneArrLength*
                                        sizeof(CPSS_DXCH_PORT_SERDES_TUNE_STC))))
    {
        return GT_OUT_OF_CPU_MEM;
    }

    for(i = 0,j=0; i < 32; i++)
    {
        if(laneBmp & (1<<i))
        {
            portLaneArrPtr[j].portNum = portNum;
            portLaneArrPtr[j].laneNum = i;
            j++;
        }
    }

    rc = cpssDxChDiagSerdesTuningRxTune(devNum, portLaneArrPtr, portLaneArrLength,
                                        mode, prbsTime,
                                        /* the only one supported at this moment */
                                        CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ACCUR_AWARE_E,
                                        optResultArrPtr);
    if(rc != GT_OK)
        return rc;

    rc = prvCpssDxChPortNumberOfSerdesLanesGet(devNum, portNum,
                                               &firstSerdes, &numOfSerdesLanes);
    if (rc != GT_OK)
    {
        cpssOsPrintf("prvCpssDxChPortNumberOfSerdesLanesGet\n");
        return rc;
    }

    /* serdes of one port are in same speed */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    rc = prvGetLpSerdesSpeed(devNum, portGroupId, firstSerdes, &serdesSpeed);
    if (rc != GT_OK)
    {
        cpssOsPrintf("prvGetLpSerdesSpeed\n");
        return rc;
    }

    for(i = 0,j=0; i < 32; i++)
    {
        if(laneBmp & (1<<i))
        {
            rc = cpssDxChPortSerdesTuningGet(devNum,portNum,i,serdesSpeed,&tuneValues);
            if (rc != GT_OK)
            {
                cpssOsPrintf("cpssDxChPortSerdesTuningGet\n");
                return rc;
            }

            optResultArrPtr[j].ffeS     = tuneValues.ffeS;
            optResultArrPtr[j].txAmp    = tuneValues.txAmp;
            optResultArrPtr[j].txAmpAdj = tuneValues.txAmpAdj;
            optResultArrPtr[j].txEmphAmp = tuneValues.txEmphAmp;
            optResultArrPtr[j].txEmphEn = tuneValues.txEmphEn;

            rc = cpssDxChPortSerdesTuningSet(devNum, portNum, (1<<i), serdesSpeed,
                                                &optResultArrPtr[j]);
            if (rc != GT_OK)
            {
                cpssOsPrintf("cpssDxChPortSerdesTuningSet\n");
                return rc;
            }
            j++;
        }
    }

    rc = cpssDxChDiagSerdesTuningSystemClose(devNum);
    if (rc != GT_OK)
    {
        cpssOsPrintf("cpssDxChDiagSerdesTuningSystemClose\n");
        return rc;
    }

    cpssOsFree(portLaneArrPtr);
    cpssOsFree(portLaneArrPtr);

    return GT_OK;
}

/*******************************************************************************
* lion2WaTask
*
* DESCRIPTION:
*       Wait till partner port is ready and execute WA according to port's speed
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
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
unsigned __TASKCONV lion2WaTask
(
    GT_VOID * param
)
{
    GT_U8       portNum;    /* Port number */
    GT_STATUS   rc;         /* Return status */
    GT_U8       devNum;     /* Device number */
    GT_UINTPTR  tmp;        /* temporary variable */

    /* to avoid warning: cast from pointer to integer of different size */
    tmp = (GT_UINTPTR)param;
    devNum = (GT_U8)tmp;

    while(1)
    {
        cpssOsSigSemWait(waTSmid, 0);

        for(portNum = 0; GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&todoWaBmp);
                                portNum = (portNum+1)%CPSS_MAX_PORTS_NUM_CNS)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);

            if(CPSS_CPU_PORT_NUM_CNS == portNum)
            {
                continue;
            }

            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoWaBmp, portNum))
            {
                continue;
            }

            rc = appDemoDxChLion2PortLinkWa(devNum, portNum);
            if(GT_NOT_READY == rc)
            {
                locksTimeoutArray[portNum]++;
                if(locksTimeoutArray[portNum] > 200)
                {
                    cpssOsPrintSync("lion2WaTask(portNum=%d) lock timedout\n",
                                    portNum);
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoWaBmp, portNum);
                }
                cpssOsTimerWkAfter(32);
            }
            else if((rc != GT_OK) && (rc != GT_NOT_READY))
            {
                cpssOsPrintSync("appDemoDxChLion2PortLinkWa(portNum=%d):rc=%d\n",
                                portNum, rc);
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoWaBmp, portNum);
            }
            if(GT_OK == rc)
            {
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoWaBmp, portNum);
            }

            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoWaBmp, portNum))
            {
                rc = cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("lion2WaTask:cpssDxChPortEnableSet(portNum=%d,true):rc=%d", portNum, rc);
                    return rc;
                }

                rc = waTriggerInterruptsMask(devNum, portNum,
                                             /* here important not RXAUI and not XGMII */
                                             CPSS_PORT_INTERFACE_MODE_KR_E,
                                             CPSS_EVENT_UNMASK_E);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("waTriggerInterruptsMask(portNum=%d):rc=%d\n",
                                    portNum, rc);
                }
            }
        }
    }

#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif
}

/*******************************************************************************
* lion2TrxTrainingTask
*
* DESCRIPTION:
*       Perform continuous polling of ports where TRX training started.
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
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static unsigned __TASKCONV lion2TrxTrainingTask
(
    GT_VOID * param
)
{
    GT_U8       portNum;    /* Port number */
    GT_STATUS   rc;         /* Return status */
    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT rxTuneStatus;
    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT txTuneStatus;
    CPSS_PORT_INTERFACE_MODE_ENT    interface;
    GT_U8       devNum;     /* Device number */
    GT_UINTPTR  tmp;        /* temporary variable */

    /* to avoid warning: cast from pointer to integer of different size */
    tmp = (GT_UINTPTR)param;
    devNum = (GT_U8)tmp;

    while(1)
    {
        cpssOsSigSemWait(trxTrSmid, 0);

        for(portNum = 0; GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&trxTrainBmp);
                                portNum = (portNum+1)%CPSS_MAX_PORTS_NUM_CNS)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);

            if(CPSS_CPU_PORT_NUM_CNS == portNum)
            {
                continue;
            }

            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&trxTrainBmp, portNum))
            {
                continue;
            }

            rc = cpssDxChPortSerdesAutoTuneStatusGet(devNum,portNum,&rxTuneStatus,&txTuneStatus);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortSerdesAutoTuneStatusGet(portNum=%d):rc=%d\n", portNum, rc);
                continue;
            }

            switch(txTuneStatus)
            {
                case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_FAIL_E:
                case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E:
                    cpssOsPrintf("port TRX training-%s:portNum=%d\n",
                                 (CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E ==
                                  txTuneStatus) ? "PASS" : "FAIL", portNum);
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&trxTrainBmp, portNum);
                    trxTimeoutArray[portNum] = 0;
                    rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TRAINING_STOP_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_STOP,portNum=%d,txTuneStatus=%d):rc=%d\n",
                                        portNum, txTuneStatus, rc);
                        continue;
                    }

                    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&interface);
                    if (rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortInterfaceModeGet(portNum=%d):rc=%d\n",
                                        portNum, rc);
                        continue;
                    }

                    if(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_FAIL_E == txTuneStatus)
                    {
                        TRAINING_DBG_PRINT_MAC(("TRX fail:port=%d,try=%d\n", portNum,
                                                trxTrainingRetryCntArray[portNum]));
                        if(++(trxTrainingRetryCntArray[portNum]) <= 5)
                        {
                            changeToDownHappend[portNum] = GT_TRUE;
                            (GT_VOID)appDemoDxChLion2SerdesTrxTrainigStart(devNum, portNum,
                                                                           interface);
                            continue;
                        }
                        else
                        {
                            rc = waTriggerInterruptsMask(devNum, portNum, interface,
                                                         CPSS_EVENT_UNMASK_E);
                            if(rc != GT_OK)
                            {
                                cpssOsPrintSync("waTriggerInterruptsMask(portNum=%d,unmask):rc=%d\n",
                                                portNum, rc);
                            }
                        }
                    }

                    break;

                case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_NOT_COMPLITED_E:
                    if((trxTimeoutArray[portNum]+1) > 30)
                    {
                        TRAINING_DBG_PRINT_MAC(("port TRX training-TIMEOUT:portNum=%d\n", portNum));
                        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&trxTrainBmp, portNum);
                        trxTimeoutArray[portNum] = 0;
                        rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                                        CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TRAINING_STOP_E);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_STOP on timeout,portNum=%d):rc=%d\n",
                                            portNum, rc);
                            continue;
                        }

                        rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&interface);
                        if (rc != GT_OK)
                        {
                            cpssOsPrintSync("cpssDxChPortInterfaceModeGet(portNum=%d):rc=%d\n",
                                            portNum, rc);
                            continue;
                        }

                        TRAINING_DBG_PRINT_MAC(("TRX timedout:port=%d,try=%d\n",
                                     portNum, trxTrainingRetryCntArray[portNum]));
                        if(++(trxTrainingRetryCntArray[portNum]) <= 5)
                        {
                            changeToDownHappend[portNum] = GT_TRUE;
                            (GT_VOID)appDemoDxChLion2SerdesTrxTrainigStart(devNum, portNum,
                                                                           interface);
                            continue;
                        }
                        else
                        {
                            rc = waTriggerInterruptsMask(devNum, portNum, interface,
                                                         CPSS_EVENT_UNMASK_E);
                            if(rc != GT_OK)
                            {
                                cpssOsPrintSync("waTriggerInterruptsMask(portNum=%d,unmask):rc=%d\n",
                                                portNum, rc);
                            }
                        }
                    }
                    else
                    {
                        TRAINING_DBG_PRINT_MAC(("TRX wait:port=%d,try=%d\n",
                                        portNum, trxTrainingRetryCntArray[portNum]));
                        trxTimeoutArray[portNum]++;
                                                    cpssOsTimerWkAfter(32);
                        continue;
                    }

                    break;

                default:
                    cpssOsPrintSync("AutoTuneStatus-bad state:portNum=%d,txTuneStatus=%d\n",
                                      portNum, txTuneStatus);
                    break;

            } /* switch(txTuneStatus) */

            /* continue to align90 even if training failed because unreset PCS etc.
                done only in align90 */
            trxTrainingRetryCntArray[portNum] = 0;
            locksTimeoutArray[portNum] = 0;

            rc = cpssDxChPortSerdesAlign90StateSet(devNum, portNum, GT_TRUE,
                (CPSS_DXCH_PORT_ALIGN90_PARAMS_STC*)&(serdesParams[portNum]));
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortSerdesAlign90StateSet:rc=%d,portNum=%d\n",
                                rc, portNum);
            }
            else
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(&todoAlign90Bmp, portNum);
                TRAINING_DBG_PRINT_MAC(("Set port=%d in todoAlign90Bmp\n", portNum));
                rc = cpssOsSigSemSignal(al90TSmid);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("appDemoDxChLion2SerdesRxTrainigRun:cpssOsSigSemSignal:rc=%d\n", rc);
                }
            }

        }/* for(portNum = 0; */

    }/* while(1)*/

#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif
}

/*******************************************************************************
* align90WaTask
*
* DESCRIPTION:
*       Perform continuous polling of ports where Align90 optimization started.
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
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static unsigned __TASKCONV align90WaTask
(
    GT_VOID * param
)
{
    GT_U8       portNum;    /* Port number */
    GT_STATUS   rc;         /* Return status */
    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT align90Status;
    GT_U8       devNum;     /* Device number */
    GT_UINTPTR  tmp;        /* temporary variable */
    GT_BOOL     stopAlign90;

    /* to avoid warning: cast from pointer to integer of different size */
    tmp = (GT_UINTPTR)param;
    devNum = (GT_U8)tmp;

    while(1)
    {
        cpssOsSigSemWait(al90TSmid, 0);

        for(portNum = 0; GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&todoAlign90Bmp);
                                portNum = (portNum+1)%CPSS_MAX_PORTS_NUM_CNS)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);

            if(CPSS_CPU_PORT_NUM_CNS == portNum)
            {
                continue;
            }

            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoAlign90Bmp, portNum))
            {
                continue;
            }

            stopAlign90 = GT_TRUE;
            rc = cpssDxChPortSerdesAlign90StatusGet(devNum, portNum, &align90Status);
            if(rc != GT_OK)
            {/* if error in execution */
                cpssOsPrintSync("cpssDxChPortSerdesAlign90StatusGet(portNum=%d):rc=%d\n",
                                                                    portNum, rc);
            }
                else if(((al90TimeoutArray[portNum]+1) > 14) &&
                (CPSS_DXCH_PORT_SERDES_AUTO_TUNE_NOT_COMPLITED_E == align90Status))
            {/* if timeout*/
                TRAINING_DBG_PRINT_MAC(("Align90-timeout:portNum=%d\n", portNum));
            }
            else if(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_FAIL_E == align90Status)
            {/* if failed */
                TRAINING_DBG_PRINT_MAC(("Align90-fail:portNum=%d\n", portNum));
            }
            else if(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E == align90Status)
            {
                TRAINING_DBG_PRINT_MAC(("Align90-OK:portNum=%d\n", portNum));
            }
            else
            {
                al90TimeoutArray[portNum]++;
                TRAINING_DBG_PRINT_MAC(("Align90-NOT_COMPLETED:portNum=%d\n", portNum));
                stopAlign90 = GT_FALSE;
                cpssOsTimerWkAfter(16);
            }

            if(stopAlign90)
            {
                rc = cpssDxChPortSerdesAlign90StateSet(devNum, portNum, GT_FALSE,
                            (CPSS_DXCH_PORT_ALIGN90_PARAMS_STC*)&(serdesParams[portNum]));
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortSerdesAlign90StateSet:portNum=%d,FALSE,rc=%d\n",
                                    portNum, rc);
                }
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoAlign90Bmp, portNum);
                al90TimeoutArray[portNum] = 0;

                if(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E == align90Status)
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&todoWaBmp, portNum);
                    TRAINING_DBG_PRINT_MAC(("Set port=%d in todoWaBmp\n", portNum));
                    rc = cpssOsSigSemSignal(waTSmid);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("align90WaTask(AUTO_TUNE_PASS):cpssOsSigSemSignal:rc=%d\n", rc);
                    }
                }
                else
                {
                    CPSS_PORT_SPEED_ENT   speed;

                    rc = cpssDxChPortSpeedGet(devNum,portNum,&speed);
                    if (rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortSpeedGet(portNum=%d):rc=%d\n",
                                        portNum, rc);
                    }

                    if(speed != CPSS_PORT_SPEED_10000_E)
                    {
                        rc = waTriggerInterruptsMask(devNum, portNum,
                                                    /* here doesn't matter SR_LR or KR */
                                                    CPSS_PORT_INTERFACE_MODE_KR_E,
                                                     CPSS_EVENT_UNMASK_E);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("waTriggerInterruptsMask(portNum=%d,unmask):rc=%d\n",
                                            portNum, rc);
                        }
                    }
                    else
                    {/* for 10G must run WA to set force link down */
                        CPSS_PORTS_BMP_PORT_SET_MAC(&todoWaBmp, portNum);
                        TRAINING_DBG_PRINT_MAC(("Set port=%d in todoWaBmp\n", portNum));
                        rc = cpssOsSigSemSignal(waTSmid);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("align90WaTask(not AUTO_TUNE_PASS):cpssOsSigSemSignal:rc=%d\n", rc);
                        }
                    }
                }

            } /* if(stopAlign90)*/

        }/* for(portNum = 0; */
    }

#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif
}

/*******************************************************************************
* appDemoDxChLion2SerdesTrxTrainigStart
*
* DESCRIPTION:
*       Start TRX training on given port
*
* INPUTS:
*       devNum - device number
*       portNum - global number of port where to run TRX training
*       interface   - port interface mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoDxChLion2SerdesTrxTrainigStart
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    interface
)
{
    GT_STATUS       rc;                /* return code         */

    TRAINING_DBG_PRINT_MAC(("appDemoDxChLion2SerdesTrxTrainigStart:portNum=%d,interface=%d\n",
                                                                    portNum, interface));

    if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&trxTrainBmp, portNum)
        || CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoWaBmp, portNum)
        || CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoAlign90Bmp, portNum))
    {
        return GT_OK;
    }

    if(0 == trxTrainingTaskId)
    {
        GT_U32   waTaskId;

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&todoWaBmp);
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&trxTrainBmp);
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&todoAlign90Bmp);
        if (cpssOsSigSemBinCreate("waTSem", CPSS_OS_SEMB_EMPTY_E, &waTSmid) != GT_OK)
        {
            return GT_NO_RESOURCE;
        }
        if (cpssOsSigSemBinCreate("trxTrSem", CPSS_OS_SEMB_EMPTY_E, &trxTrSmid) != GT_OK)
        {
            return GT_NO_RESOURCE;
        }
        if (cpssOsSigSemBinCreate("al90Sem", CPSS_OS_SEMB_EMPTY_E, &al90TSmid) != GT_OK)
        {
            return GT_NO_RESOURCE;
        }

        rc = cpssOsTaskCreate("trxTraining",              /* Task Name      */
                              210,                      /* Task Priority  */
                              _32K,                     /* Stack Size     */
                              lion2TrxTrainingTask,      /* Starting Point */
                              (GT_VOID*)((GT_UINTPTR)devNum), /* Arguments list */
                              &trxTrainingTaskId);      /* task ID        */
        if(rc != GT_OK)
        {
            cpssOsPrintf("failed to create TRX training task:rc=%d\n", rc);
            return rc;
        }
        rc = cpssOsTaskCreate("waTask",    /* Task Name      */
                              210,              /* Task Priority  */
                              _32K,             /* Stack Size     */
                              lion2WaTask,      /* Starting Point */
                              (GT_VOID*)((GT_UINTPTR)devNum), /* Arguments list */
                              &waTaskId);       /* task ID        */
        if(rc != GT_OK)
        {
            cpssOsPrintf("failed to create links WA task:rc=%d\n", rc);
            return rc;
        }
        rc = cpssOsTaskCreate("al90Task",    /* Task Name      */
                              210,              /* Task Priority  */
                              _32K,             /* Stack Size     */
                              align90WaTask,      /* Starting Point */
                              (GT_VOID*)((GT_UINTPTR)devNum), /* Arguments list */
                              &align90TaskId);       /* task ID        */
        if(rc != GT_OK)
        {
            cpssOsPrintf("failed to create links WA task:rc=%d\n", rc);
            return rc;
        }
    }

    TRAINING_DBG_PRINT_MAC(("appDemoDxChLion2SerdesTrxTrainigStart:changeToDownHappend[%d]=%d\n",
                                                            portNum, changeToDownHappend[portNum]));
    if(!changeToDownHappend[portNum])
    {
        changeToDownHappend[portNum] = GT_TRUE;
        return GT_OK;
    }
    else
    {
        changeToDownHappend[portNum] = GT_FALSE;
    }

    rc = waTriggerInterruptsMask(devNum, portNum, interface, CPSS_EVENT_MASK_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_CFG_E);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_CFG,portNum=%d):rc=%d\n", portNum, rc);
        return rc;
    }

    rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_START_E);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_START,portNum=%d):rc=%d\n", portNum, rc);
        return rc;
    }

    CPSS_PORTS_BMP_PORT_SET_MAC(&trxTrainBmp, portNum);
    rc = cpssOsSigSemSignal(trxTrSmid);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("appDemoDxChLion2SerdesRxTrainigRun:cpssOsSigSemSignal:rc=%d\n", rc);
    }

    cpssOsPrintf("SERDES TRx training - port %d\n",portNum);

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChLion2SerdesRxTrainigRun
*
* DESCRIPTION:
*      Run RX training only on port
*
* INPUTS:
*       devNum  - device number
*       portNum - global port number
*       interface - current interface of port
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
GT_STATUS appDemoDxChLion2SerdesRxTrainigRun
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    interface
)
{
    GT_STATUS                       rc;

    TRAINING_DBG_PRINT_MAC(("appDemoDxChLion2SerdesRxTrainigRun:portNum=%d\n",
                                                                    portNum));
    if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoWaBmp, portNum))
    {
        return GT_OK;
    }

    TRAINING_DBG_PRINT_MAC(("appDemoDxChLion2SerdesRxTrainigRun:changeToDownHappend[%d]=%d\n",
                                                        portNum, changeToDownHappend[portNum]));
    if(!changeToDownHappend[portNum])
    {
        changeToDownHappend[portNum] = GT_TRUE;
        return GT_OK;
    }
    else
    {
        changeToDownHappend[portNum] = GT_FALSE;
    }

    if(0 == waTSmid)
    {
        GT_U32   waTaskId;

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&todoWaBmp);
        cpssOsMemSet(locksTimeoutArray, sizeof(GT_U32)*CPSS_MAX_PORTS_NUM_CNS, 0);
        if (cpssOsSigSemBinCreate("waTSem", CPSS_OS_SEMB_EMPTY_E, &waTSmid) != GT_OK)
        {
            return GT_NO_RESOURCE;
        }
        rc = cpssOsTaskCreate("waTask",    /* Task Name      */
                              210,              /* Task Priority  */
                              _32KB,             /* Stack Size     */
                              lion2WaTask,      /* Starting Point */
                              (GT_VOID*)((GT_UINTPTR)devNum), /* Arguments list */
                              &waTaskId);       /* task ID        */
        if(rc != GT_OK)
        {
            cpssOsPrintSync("failed to create TRX training task:rc=%d\n", rc);
            return rc;
        }
    }

    rc = waTriggerInterruptsMask(devNum, portNum, interface, CPSS_EVENT_MASK_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("SERDES RX training - port %d\n",portNum);
#ifndef ASIC_SIMULATION
    rc = cpssDxChPortSerdesAutoTune(devNum,portNum,
                            CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_RX_TRAINING_E);
    if(rc != GT_OK)
    {
        cpssOsPrintf("appDemoDxChLion2SerdesRxTrainigRun(portNum=%d):RX training failed:rc=%d",
                                                                    portNum, rc);
        (GT_VOID)waTriggerInterruptsMask(devNum, portNum, interface, CPSS_EVENT_UNMASK_E);
        return rc;
    }
#endif

    locksTimeoutArray[portNum] = 0;
    CPSS_PORTS_BMP_PORT_SET_MAC(&todoWaBmp, portNum);
    rc = cpssOsSigSemSignal(waTSmid);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("appDemoDxChLion2SerdesRxTrainigRun:cpssOsSigSemSignal:rc=%d\n", rc);
    }

    return rc;
}

#ifndef ASIC_SIMULATION
/*******************************************************************************
* appDemoDxChLion2SerdesAutoTuningStartTask
*
* DESCRIPTION:
*      Run required type of training on serdeses of port if it's ready
*
* INPUTS:
*       param - devNum  - device number
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
unsigned __TASKCONV appDemoDxChLion2SerdesAutoTuningStartTask
(
    GT_VOID * param
)
{
    GT_U8               devNum;             /* device number */
    GT_STATUS           rc;                /* return code         */
    CPSS_PORT_INTERFACE_MODE_ENT interface; /* current ports interface */
    GT_U32      trxTraining;    /* what kind of auto-tuning run on port */
    GT_BOOL     signalState;    /* does signal changed to up or to down */
    GT_BOOL                         apEnable; /* AP enable status on port */
    CPSS_DXCH_PORT_AP_PARAMS_STC    apParams; /* parameters of AP on port */
    GT_BOOL     forceLinkUp, /* link up forced on port */
                internalLoopEn; /* MAC and PCS loopback configured on port */
    CPSS_DXCH_PORT_SERDES_LOOPBACK_MODE_ENT serdesLbMode; /* serdes loopback enabled on port */
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32  portsSignalTimeoutArray[CPSS_MAX_PORTS_NUM_CNS] = { 0 };

    devNum = (GT_U8)((GT_UINTPTR)param);
    TRAINING_DBG_PRINT_MAC(("appDemoDxChLion2SerdesAutoTuningStartTask:devNum=%d\n",
                                                                        devNum));
    while(1)
    {
        cpssOsSigSemWait(tuneSmid, 0);

        for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);

            if(CPSS_CPU_PORT_NUM_CNS == portNum)
            {
                continue;
            }

            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoTuneBmp, portNum))
            {
                continue;
            }

            rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&interface);
            if (rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortInterfaceModeGet fail port=%d,rc=%d\n",
                                    portNum, rc);
                return rc;
            }

            /* Although SR_LR supposed to work on optical cables that doesn't support TRX
               training, here in demo we allow such option for debugging purposes only */
            if ((interface != CPSS_PORT_INTERFACE_MODE_KR_E) &&
                (interface != CPSS_PORT_INTERFACE_MODE_SR_LR_E) &&
                (interface != CPSS_PORT_INTERFACE_MODE_XHGS_E) &&
                (interface != CPSS_PORT_INTERFACE_MODE_XHGS_SR_E))
            {
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
                            rc = cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
                            if(rc != GT_OK)
                            {
                                cpssOsPrintf("cpssDxChPortEnableSet(portNum=%d,true):rc=%d\n", portNum, rc);
                            }
                continue;
            }

            rc = cpssDxChPortForceLinkPassEnableGet(devNum,portNum,/*OUT*/&forceLinkUp);
            if (rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortForceLinkPassEnableGet fail port=%d,rc=%d\n",
                                    portNum, rc);
                return rc;
            }

            /* do not perform auto-tune on ports with force link-up*/
            if (forceLinkUp == GT_TRUE)
            {
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
                            rc = cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
                            if(rc != GT_OK)
                            {
                                cpssOsPrintf("cpssDxChPortEnableSet(portNum=%d,true):rc=%d\n", portNum, rc);
                            }
                continue;
            }

            rc = cpssDxChPortInternalLoopbackEnableGet(devNum,portNum,&internalLoopEn);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* do not perform auto-tune on ports with loopback */
            if (internalLoopEn == GT_TRUE)
            {
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
                            rc = cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
                            if(rc != GT_OK)
                            {
                                cpssOsPrintf("cpssDxChPortEnableSet(portNum=%d,true):rc=%d\n", portNum, rc);
                            }
                continue;
            }

            rc = cpssDxChPortSerdesLoopbackModeGet(devNum, portNum, 0, &serdesLbMode);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* do not perform auto-tune on ports with loopback */
            if (serdesLbMode != CPSS_DXCH_PORT_SERDES_LOOPBACK_DISABLE_E)
            {
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
                            rc = cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
                            if(rc != GT_OK)
                            {
                                cpssOsPrintf("cpssDxChPortEnableSet(portNum=%d,true):rc=%d\n", portNum, rc);
                            }
                continue;
            }

            (GT_VOID)cpssDxChPortApPortConfigGet(devNum, portNum, &apEnable, &apParams);

            if(apEnable)
            {/* don't run rx training if AP enabled on port,
                         also no need to re-enable port in case of AP it should be done by
                         application */
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
                continue;
            }

            if(appDemoDbEntryGet("trxTraining", &trxTraining) != GT_OK)
            {
                trxTraining = 0;
            }

            if(!trxTraining)
            {
                /* check that PCS sync OK detected on port - on all its serdes lanes */
                rc = cpssDxChPortPcsSyncStatusGet(devNum, portNum, &signalState);
                if (rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortPcsSyncStatusGet fail port=%d,rc=%d\n",
                                 portNum, rc);
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
                    return rc;
                }
            }
            else
            {/* In case of TRX training if other side started already training
                there won't be PCS sync, so just check signal to see if there is
                partner and try to start training.
                With fiber connection there could be signal detect just because connector
                is in, while no real peer connected to port or peer not configured
                to appropriate mode */
                rc = cpssDxChPortSerdesSignalDetectGet(devNum, portNum, &signalState);
                if (rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortSerdesSignalDetectGet fail port=%d,rc=%d\n",
                                 portNum, rc);
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
                    return rc;
                }
            }

            TRAINING_DBG_PRINT_MAC(("portNum=%d:signalState/pcsSync=%d\n", portNum, signalState));
            if(GT_FALSE == signalState)
            {
                if(++portsSignalTimeoutArray[portNum] >= 100)
                {
                    changeToDownHappend[portNum] = GT_TRUE;
                    TRAINING_DBG_PRINT_MAC(("interface=%d\n", interface));
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
                    portsSignalTimeoutArray[portNum] = 0;
                }
                continue;
            }

                /* although link could be already up partner could be changed to 40G,
                        so must continue till 10G-to-40G WA */
            if(trxTraining)
            {
                rc = appDemoDxChLion2SerdesTrxTrainigStart(devNum, portNum, interface);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("appDemoDxChLion2SerdesAutoTuningRun:appDemoDxChLion2SerdesTrxTrainigStart(port=%d,interface=%d):rc=%d\n",
                                                        portNum, interface, rc);
                }
            }
            else
            {
                rc = appDemoDxChLion2SerdesRxTrainigRun(devNum,portNum,interface);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("appDemoDxChLion2SerdesAutoTuningRun:appDemoDxChLion2SerdesRxTrainigRun(port=%d,interface=%d):rc=%d\n",
                                                        portNum, interface, rc);
                }
            }

            CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoTuneBmp, portNum);
        } /* for(portNum = 0; */

        cpssOsTimerWkAfter(32);

    } /* while(1) */

#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif
}

#endif

#ifndef ASIC_SIMULATION
/*******************************************************************************
* appDemoDxChGbLockCheck
*
* DESCRIPTION:
*      Check if gear box locked for given port
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
static GT_STATUS appDemoDxChGbLockCheck
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS rc; /* return code */
    GT_U32    gearBoxLockCntr; /* retry counter */
    GT_BOOL   gbLock; /* gear box lock status */

    /* Check Gear Box Status */
    for(gearBoxLockCntr = 0, gbLock = GT_FALSE;
         (gearBoxLockCntr < 5) && (!gbLock); gearBoxLockCntr++)
    {/* ensure we get real gear box lock and not false indication */
        rc = cpssDxChPortPcsGearBoxStatusGet(devNum, portNum, &gbLock);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsTimerWkAfter(5);
    }

    TRAINING_DBG_PRINT_MAC(("portNum=%d:gbLock=%d\n", portNum, gbLock));
    if (!gbLock)
        return GT_NOT_READY;
    else
        return GT_OK;
}
#endif

/*******************************************************************************
* appDemoDxChLion2PortLinkWa
*
* DESCRIPTION:
*      For 40G interfaces execute WA - connect/disconnect leaves link down problem
*      For 10/20G interfaces execute WA - false link up with dismatching interface
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
GT_STATUS appDemoDxChLion2PortLinkWa
(
    IN   GT_U8                            devNum,
    IN   GT_PHYSICAL_PORT_NUM             portNum
)
{
#ifndef ASIC_SIMULATION
    GT_STATUS                       rc;         /* return code */
    CPSS_PORT_SPEED_ENT             speed;      /* port speed */
    GT_BOOL                         apEnable;   /* is AP enabled on port */
    CPSS_DXCH_PORT_AP_PARAMS_STC    apParams;   /* AP parameters of port */
    GT_BOOL                         isLinkUp;   /* is link up on port */
    CPSS_DXCH_IMPLEMENT_WA_ENT      waArr[1];   /* array of workarounds to enable */
    GT_U32                          additionalInfoBmpArr[1]; /* additional WA's info */
    GT_U32                          disable40GWa; /* don't run 40G con/discon WA */
    GT_U32                          disableMismatchLinksWa;/* don't run "false link up" WA */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* current interface configured on port */

    TRAINING_DBG_PRINT_MAC(("appDemoDxChLion2PortLinkWa:portNum=%d\n", portNum));

    if (CPSS_CPU_PORT_NUM_CNS == portNum)
    {
        return GT_OK;
    }

    /* no need to check status, valid apEnable status will be in any case */
    (GT_VOID)cpssDxChPortApPortConfigGet(devNum, portNum, &apEnable, &apParams);

    /* don't run WA's if AP enabled on port */
    if(apEnable)
    {
        TRAINING_DBG_PRINT_MAC(("portNum=%d,apEnable==true\n", portNum));
        return GT_OK;
    }

    rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode)
    {
        TRAINING_DBG_PRINT_MAC(("portNum=%d,RXAUI\n", portNum));
        waArr[0] = CPSS_DXCH_IMPLEMENT_WA_RXAUI_LINK_E;
        additionalInfoBmpArr[0] = portNum;
        rc = cpssDxChHwPpImplementWaInit(devNum,1,&waArr[0],
                                         &additionalInfoBmpArr[0]);

        return rc;
    }

    if((CPSS_PORT_INTERFACE_MODE_KR_E != ifMode) &&
       (CPSS_PORT_INTERFACE_MODE_SR_LR_E != ifMode) &&
        (CPSS_PORT_INTERFACE_MODE_XHGS_E != ifMode) &&
       (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E != ifMode))
    {
        TRAINING_DBG_PRINT_MAC(("portNum=%d,ifMode=%d\n", portNum, ifMode));
        return GT_OK;
    }

    rc = cpssDxChPortSpeedGet(devNum, portNum, &speed);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortSpeedGet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(speed)
    {
        case CPSS_PORT_SPEED_40000_E:
        case CPSS_PORT_SPEED_47200_E:

            if(GT_OK == appDemoDbEntryGet("disable40GWa", &disable40GWa))
            {
                if(disable40GWa)
                {
                    return GT_OK;
                }
            }

            rc = cpssDxChPortLinkStatusGet(devNum, portNum, &isLinkUp);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortLinkStatusGet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* interesting only links that are down */
            if(GT_TRUE == isLinkUp)
            {
                TRAINING_DBG_PRINT_MAC(("portNum=%d link up\n", portNum));
                return GT_OK;
            }

            /* Check Gear Box Status */
            if((rc = appDemoDxChGbLockCheck(devNum,portNum)) != GT_OK)
            {
                return rc;
            }

            waArr[0] = CPSS_DXCH_IMPLEMENT_WA_NO_ALLIGNMENT_LOCK_E;
            additionalInfoBmpArr[0] = portNum;
            rc = cpssDxChHwPpImplementWaInit(devNum,1,&waArr[0],
                                             &additionalInfoBmpArr[0]);
            break;

        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_11800_E:
            if(GT_OK == appDemoDbEntryGet("disableMismatchLinksWa", &disableMismatchLinksWa))
            {
                if(disableMismatchLinksWa)
                {
                    return GT_OK;
                }
            }

            /* Check Gear Box Status */
            if((rc = appDemoDxChGbLockCheck(devNum,portNum)) != GT_OK)
            {
                return rc;
            }

            waArr[0] = CPSS_DXCH_IMPLEMENT_WA_DISMATCH_IF_LINK_E;
            additionalInfoBmpArr[0] = portNum;
            rc = cpssDxChHwPpImplementWaInit(devNum,1,&waArr[0],
                                             &additionalInfoBmpArr[0]);

            break;

        default:
            return GT_OK;
    }

    return rc;
#else
    (GT_VOID)devNum;
    (GT_VOID)portNum;

    return GT_OK;
#endif
}

/*******************************************************************************
* appDemoPfcEnableConfiguration
*
* DESCRIPTION:
*     Init PFC feature - with following static configuration - during init.
*           - 10G ports are supported only
*           - two traffic classes in PFC mode - fixed TC5 and TC6
*           - shared memory buffer management
*           - PFC counting mode in packets
*           - Token bucket baseline configuration - 0x3FFFC0
*           - for all ports:
*               . flow control global configuration: CPSS_PORT_FLOW_CONTROL_RX_TX_E
*               . flow control mode: CPSS_DXCH_PORT_FC_MODE_PFC_E
*               . periodic PFC enabled
*               . all ports are binded to PFC profile#1
*           - for PFC traffic classes (TC5 and TC6) - set PFC static configuration
*
*
* INPUTS:
*       dev - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoPfcEnableConfiguration
(
    IN GT_U8                 dev

)
{
    GT_STATUS rc;
    GT_U8  tc;
    GT_U32 pfcProfileIndex;
    GT_PHYSICAL_PORT_NUM portNum;
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     pfcProfileCfgCpss;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT         tailDropForPfcProfileSet;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS   portHolProfileCfgCpss;
    GT_U32 glXoffThresholdTc[CPSS_TC_RANGE_CNS], glDropThresholdTc[CPSS_TC_RANGE_CNS];
    GT_U32 perPortXoffThresholdTc[CPSS_TC_RANGE_CNS], perPortXonThresholdTc[CPSS_TC_RANGE_CNS];
    GT_U32 dpMaxBuffNum[CPSS_TC_RANGE_CNS], dpMaxDescrNum[CPSS_TC_RANGE_CNS];
    GT_U32 devIdx; /* index to appDemoPpConfigList */

    /* Tail Drop - profile 7 (for PFC) configuration */
    tailDropForPfcProfileSet = CPSS_PORT_TX_DROP_PROFILE_7_E;

    /*************************************************************/
    /********************** system configuration          ********/
    /*************************************************************/
    /* buffer management - divided mode */
    rc = cpssDxChPortBuffersModeSet(dev, CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortBuffersModeSet", rc);
    if(rc != GT_OK)
        return rc;

    /* PFC counting mode per packet */
    rc = cpssDxChPortPfcCountingModeSet(dev, CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPfcCountingModeSet", rc);
    if(rc != GT_OK)
        return rc;

    /* Token bucket baseline configuration */
    rc = cpssDxChPortTxShaperBaselineSet(dev, 0x3FFFC0);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxShaperBaselineSet", rc);
    if(rc != GT_OK)
        return rc;

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    /* loop over all ports */
    for(portNum = 0; portNum < (appDemoPpConfigList[devIdx].maxPortNumber); portNum++)
    {
        CPSS_ENABLER_PORT_SKIP_CHECK(dev,portNum);

        /* skip CPU port */
        if (portNum == CPSS_CPU_PORT_NUM_CNS)
            continue;

        /* Flow control global configuration: */
        rc = cpssDxChPortFlowControlEnableSet(dev, portNum, CPSS_PORT_FLOW_CONTROL_RX_TX_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortFlowControlEnableSet", rc);
        if(rc != GT_OK)
            return rc;

        rc = cpssDxChPortFlowControlModeSet(dev, portNum, CPSS_DXCH_PORT_FC_MODE_PFC_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortFlowControlModeSet", rc);
        if(rc != GT_OK)
            return rc;

        /* Periodic PFC configuration */
        rc = cpssDxChPortPeriodicFcEnableSet(dev, portNum, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPeriodicFcEnableSet", rc);
        if(rc != GT_OK)
            return rc;

        /* Bind the port to the PFC profile #1 */
        rc = cpssDxChPortPfcProfileIndexSet(dev, portNum, 1);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPfcProfileIndexSet", rc);
        if(rc != GT_OK)
            return rc;

        /* Bind the port to the Tail Drop profile #7 */
        rc = cpssDxChPortTxBindPortToDpSet(dev, portNum, tailDropForPfcProfileSet);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxBindPortToDpSet", rc);
        if(rc != GT_OK)
            return rc;
    }

    /* Set periodic interval to send 1200 framePerSecond to avoid dropping 4 sequence of periodic frames
       The value to be written in register is 0x51C0  -> the IntervalSet function is multiplying by 25*/
    rc = cpssDxChPortPeriodicFlowControlIntervalSet(dev, CPSS_DXCH_PORT_PERIODIC_FC_TYPE_XG_E, 0x345);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPeriodicFlowControlIntervalSet", rc);
    if(rc != GT_OK)
        return rc;


    /*************************************************************/
    /********************** PFC specific configuration ***********/
    /*************************************************************/

    /* Global PFC threshold = cpssDxChPortPfcGlobalQueueConfigSet*/
    glXoffThresholdTc[0] = 2040;  glDropThresholdTc[0] = 2000;
    glXoffThresholdTc[1] = 2040;  glDropThresholdTc[1] = 2000;
    glXoffThresholdTc[2] = 2040;  glDropThresholdTc[2] = 2000;
    glXoffThresholdTc[3] = 2040;  glDropThresholdTc[3] = 2000;
    glXoffThresholdTc[4] = 2040;  glDropThresholdTc[4] = 2000;
    glXoffThresholdTc[5] =  370;  glDropThresholdTc[5] = 2000;          /* PFC enabled TC = 5 */
    glXoffThresholdTc[6] =  370;  glDropThresholdTc[6] = 2000;          /* PFC enabled TC = 6 */
    glXoffThresholdTc[7] = 2040;  glDropThresholdTc[7] = 2000;

    /* PFC-Profile 1 (PFC) Configuration*/
    pfcProfileIndex = 1;
    perPortXoffThresholdTc[0] = 2040; perPortXonThresholdTc[0] = 2040;
    perPortXoffThresholdTc[1] = 2040; perPortXonThresholdTc[1] = 2040;
    perPortXoffThresholdTc[2] = 2040; perPortXonThresholdTc[2] = 2040;
    perPortXoffThresholdTc[3] = 2040; perPortXonThresholdTc[3] = 2040;
    perPortXoffThresholdTc[4] = 2040; perPortXonThresholdTc[4] = 2040;
    perPortXoffThresholdTc[5] =   42; perPortXonThresholdTc[5] =   24;  /* PFC enabled TC = 5 */
    perPortXoffThresholdTc[6] =   42; perPortXonThresholdTc[6] =   24;  /* PFC enabled TC = 6 */
    perPortXoffThresholdTc[7] = 2040; perPortXonThresholdTc[7] = 2040;

    /* Tail Drop - profile 7 (for PFC) configuration */
    dpMaxBuffNum[0] =     132; dpMaxDescrNum[0] = 44;
    dpMaxBuffNum[1] =     132; dpMaxDescrNum[1] = 44;
    dpMaxBuffNum[2] =     132; dpMaxDescrNum[2] = 44;
    dpMaxBuffNum[3] =     132; dpMaxDescrNum[3] = 44;
    dpMaxBuffNum[4] =     132; dpMaxDescrNum[4] = 44;
    dpMaxBuffNum[5] =  0x3FFF; dpMaxDescrNum[5] = 0x3FFF;   /* PFC enabled TC = 5 */
    dpMaxBuffNum[6] =  0x3FFF; dpMaxDescrNum[6] = 0x3FFF;   /* PFC enabled TC = 6 */
    dpMaxBuffNum[7] =     132; dpMaxDescrNum[7] = 44;


    /* per TC - set PFC default global parameters per traffic classs */
    for (tc = 0; tc < CPSS_TC_RANGE_CNS; tc++)
    {
        /* set PFC global thresholds per traffic class*/
        rc = cpssDxChPortPfcGlobalQueueConfigSet(dev, tc, glXoffThresholdTc[tc], glDropThresholdTc[tc], 0/*don't care for Lion2*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPfcGlobalQueueConfigSet", rc);
        if(rc != GT_OK)
            return rc;

        /* set PFC profile default thresholds for non PFC and PFC ports per traffic class */
        pfcProfileCfgCpss.xoffThreshold = perPortXoffThresholdTc[tc];
        pfcProfileCfgCpss.xonThreshold  = perPortXonThresholdTc[tc];
        rc = cpssDxChPortPfcProfileQueueConfigSet(dev, pfcProfileIndex, tc, &pfcProfileCfgCpss);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPfcProfileQueueConfigSet", rc);
        if(rc != GT_OK)
            return rc;

        /* set Tail Drop configuration per traffic class */
        portHolProfileCfgCpss.dp0MaxBuffNum =
        portHolProfileCfgCpss.dp1MaxBuffNum =
        portHolProfileCfgCpss.dp2MaxBuffNum = dpMaxBuffNum[tc];
        portHolProfileCfgCpss.dp0MaxDescrNum =
        portHolProfileCfgCpss.dp1MaxDescrNum =
        portHolProfileCfgCpss.dp2MaxDescrNum = dpMaxDescrNum[tc];
        portHolProfileCfgCpss.tcMaxBuffNum  = dpMaxBuffNum[tc];
        portHolProfileCfgCpss.tcMaxDescrNum = dpMaxDescrNum[tc];
        portHolProfileCfgCpss.dp0MaxMCBuffNum = 0;
        portHolProfileCfgCpss.dp1MaxMCBuffNum = 0;
        portHolProfileCfgCpss.dp2MaxMCBuffNum = 0;
        portHolProfileCfgCpss.sharedUcAndMcCountersDisable = GT_FALSE;
        rc = cpssDxChPortTx4TcTailDropProfileSet(dev, tailDropForPfcProfileSet, tc, &portHolProfileCfgCpss);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTx4TcTailDropProfileSet", rc);
        if(rc != GT_OK)
            return rc;
    }

    /* set Tail Drop configuration per profile (specific for PFC dedicated profile) */
    rc = cpssDxChPortTxTailDropProfileSet(dev, tailDropForPfcProfileSet, GT_FALSE, dpMaxBuffNum[5], dpMaxDescrNum[5]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxTailDropProfileSet", rc);
    if(rc != GT_OK)
        return rc;

    /* PFC is globally enabled */
    rc = cpssDxChPortPfcEnableSet(dev, CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPfcEnableSet", rc);
    if(rc != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChLion3NewTtiTcamSupportSet
*
* DESCRIPTION:
*       set the useAppdemoOffset for new TTI TCAM mode.
*
* INPUTS:
*       useAppdemoOffset - whether to take the TTI offset in TCAM into consideration
*
* OUTPUTS:
*       none
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID appDemoDxChLion3NewTtiTcamSupportSet
(
    IN  GT_BOOL             useAppdemoOffset
)
{
    appDemoLion3TtiTcamUseAppdemoOffset = useAppdemoOffset;
}

/*******************************************************************************
* appDemoDxChLion3NewTtiTcamSupportGet
*
* DESCRIPTION:
*       get the utilLion3NewTtiTcamSupportEnable for new TTI TCAM mode.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       useAppdemoBase - (pointer to) whether to take the TTI offset in TCAM
*                        into consideration
*
* RETURNS:
*       GT_OK        - OK
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxChLion3NewTtiTcamSupportGet
(
    OUT GT_BOOL *useAppdemoBase
)
{
    CPSS_NULL_PTR_CHECK_MAC(useAppdemoBase);

    *useAppdemoBase = appDemoLion3TtiTcamUseAppdemoOffset;

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChLion3NewPclTcamSupportSet
*
* DESCRIPTION:
*       set the appDemoLion3PclTcamUseIndexConversion for new PCL TCAM mode.
*
* INPUTS:
*       enableIndexConversion - whether to convert PCL TCAM index
*
* OUTPUTS:
*       none
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID appDemoDxChLion3NewPclTcamSupportSet
(
    IN  GT_BOOL             enableIndexConversion
)
{
    appDemoLion3PclTcamUseIndexConversion = enableIndexConversion;
}

/*******************************************************************************
* appDemoDxChLion3NewPclTcamSupportGet
*
* DESCRIPTION:
*       get the appDemoLion3PclTcamUseIndexConversion for new PCL TCAM mode.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       enableIndexConversion - (pointer to) whether to convert PCL TCAM index
*
* RETURNS:
*       GT_OK        - OK
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxChLion3NewPclTcamSupportGet
(
    OUT GT_BOOL *enableIndexConversion
)
{
    CPSS_NULL_PTR_CHECK_MAC(enableIndexConversion);

    *enableIndexConversion = appDemoLion3PclTcamUseIndexConversion;

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChLion3TcamTtiConvertedIndexGet
*
* DESCRIPTION:
*       Gets TCAM converted index for client TTI
*
* INPUTS:
*       devNum   - device number
*       index    - global line index in TCAM to write to.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The converted rule index.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 appDemoDxChLion3TcamTtiConvertedIndexGet
(
    IN     GT_U8                            devNum,
    IN     GT_U32                           index
)
{
    GT_BOOL useAppdemoOffset;
    GT_U32  ttiMaxIndex;
    GT_STATUS st = GT_OK;

    /* Call cpssDxChCfgTableNumEntriesGet. */
    st = cpssDxChCfgTableNumEntriesGet(devNum, CPSS_DXCH_CFG_TABLE_TTI_TCAM_E,&ttiMaxIndex);

    if (st != GT_OK)
    {
        return 0;
    }

    appDemoDxChLion3NewTtiTcamSupportGet(&useAppdemoOffset);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (useAppdemoOffset == GT_TRUE)
            return ((index*3) + appDemoTcamTtiHit0RuleBaseIndexOffset);
        else
        {
            if ((index*3) >= ttiMaxIndex)
                return 3*(index/3);
            else
                return (index*3);
        }
    }
    else
    {
        return index;
    }
}

/*******************************************************************************
* appDemoDxChLion3TcamTtiBaseIndexGet
*
* DESCRIPTION:
*       Gets TCAM base index for TTI rules
*
* INPUTS:
*       devNum   - device number
*       hitNum   - hit index, applicable range depends on device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The converted rule index.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 appDemoDxChLion3TcamTtiBaseIndexGet
(
    IN     GT_U8                            devNum,
    IN     GT_U32                           hitNum
)
{
    if((PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0) ||
       PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
    {
        return 0;
    }
    switch (hitNum)
    {
        case 0: return appDemoTcamTtiHit0RuleBaseIndexOffset;
        case 1: return appDemoTcamTtiHit1RuleBaseIndexOffset;
        case 2: return appDemoTcamTtiHit2RuleBaseIndexOffset;
        case 3: return appDemoTcamTtiHit3RuleBaseIndexOffset;
        default: return 0;
    }
}

/*******************************************************************************
* appDemoDxChLion3TcamTtiNumOfIndexsGet
*
* DESCRIPTION:
*       Gets TCAM number of indexes for TTI rules
*
* INPUTS:
*       devNum   - device number
*       hitNum   - hit index, applicable range depends on device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       TCAM number of indexes for TTI rules.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 appDemoDxChLion3TcamTtiNumOfIndexsGet
(
    IN     GT_U8                            devNum,
    IN     GT_U32                           hitNum
)
{
    GT_U32 numOfIndexes = 0;

    if((PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0) ||
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
    {
        return 0;
    }

    switch (hitNum)
    {
        case 0:
            numOfIndexes = appDemoTcamTtiHit0MaxNum;
            break;

        case 1:
            numOfIndexes = appDemoTcamTtiHit1MaxNum; /* one floor used */
            break;

        case 2:
            numOfIndexes = appDemoTcamTtiHit2MaxNum;
            break;

        case 3:
            numOfIndexes = appDemoTcamTtiHit3MaxNum;
            break;

        default:
            numOfIndexes = 0;
    }

    return numOfIndexes;
}

/*******************************************************************************
* appDemoDxChLion3TcamPclConvertedIndexGet
*
* DESCRIPTION:
*       Gets TCAM converted index for client PCL
*
* INPUTS:
*       devNum     - device number
*       index      - global line index in TCAM to write to.
*       ruleSize   - rule size in TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The converted rule index.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 appDemoDxChLion3TcamPclConvertedIndexGet
(
    IN     GT_U8                                devNum,
    IN     GT_U32                               index,
    IN     CPSS_DXCH_TCAM_RULE_SIZE_ENT         ruleSize
)
{
    GT_BOOL   enableIndexConversion;

    appDemoDxChLion3NewPclTcamSupportGet(&enableIndexConversion);
    if((PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0) ||
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (enableIndexConversion == GT_TRUE)
        {
            switch(ruleSize)
            {
                case CPSS_DXCH_TCAM_RULE_SIZE_10_B_E:
                    return index + appDemoTcamPclRuleBaseIndexOffset;
                case CPSS_DXCH_TCAM_RULE_SIZE_20_B_E:
                    return (index*2) + appDemoTcamPclRuleBaseIndexOffset;
                case CPSS_DXCH_TCAM_RULE_SIZE_40_B_E:
                case CPSS_DXCH_TCAM_RULE_SIZE_50_B_E:
                case CPSS_DXCH_TCAM_RULE_SIZE_60_B_E:
                    return (index*6) + appDemoTcamPclRuleBaseIndexOffset;
                case CPSS_DXCH_TCAM_RULE_SIZE_80_B_E:
                    return (index*12) + appDemoTcamPclRuleBaseIndexOffset;
                case CPSS_DXCH_TCAM_RULE_SIZE_30_B_E:
                default:
                    return (index*3) + appDemoTcamPclRuleBaseIndexOffset;
            }
        }
        else
        {
            return index;
        }
    }
    else
    {
        return index;
    }

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

#ifdef IMPL_MIRROR
   prvMirrorLibInit,
#endif

#ifdef IMPL_PCL
   prvPclLibInit,
#endif

#ifdef IMPL_TCAM
   prvTcamLibInit,
#endif

#ifdef IMPL_POLICER
   prvPolicerLibInit,
#endif

#ifdef IMPL_TRUNK
    prvTrunkLibInit,
#endif

#ifdef IMPL_IP
    prvIpLibInit,
#endif

    0
};


GT_STATUS disableWa
(
    GT_BOOL enable
)
{
    if (enable == GT_FALSE)
    {
        PRV_CPSS_DXCH_ERRATA_CLEAR_MAC(0,PRV_CPSS_DXCH_LION2_SHARE_EN_CHANGE_WA_E);
    }
    else
    {
        PRV_CPSS_DXCH_ERRATA_SET_MAC(0,PRV_CPSS_DXCH_LION2_SHARE_EN_CHANGE_WA_E);
    }

    return GT_OK;
}


/*******************************************************************************
* appDemoDxChMappedPortGet
*
* DESCRIPTION:
*           Get port mapping from appDemoPpConfigList DB.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       mapArrLenPtr   - number of ports
*       mapArrPtr      - ports mapping
*
* RETURNS:
*       GT_OK       - on success,
*       GT_FAIL     - otherwise.
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM- on wrong mapAttLen
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoDxChMaxMappedPortGet
(
    IN  GT_U8                   dev,
    OUT  GT_U32                 *mapArrLenPtr,
    OUT  CPSS_DXCH_PORT_MAP_STC **mapArrPtr
)
{
    GT_U32  devIdx; /* index to appDemoPpConfigList */
    GT_STATUS rc;

    if (mapArrLenPtr == NULL || mapArrPtr == NULL)
    {
        return GT_BAD_PTR;
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    *mapArrPtr = appDemoPpConfigList[devIdx].portsMapArrPtr;
    *mapArrLenPtr = appDemoPpConfigList[devIdx].portsMapArrLen;

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChMaxMappedPortSet
*
* DESCRIPTION:
*           Calculate and set maximal mapped port number,
*           keep port mapping in appDemoPpConfigList DB.
*
* INPUTS:
*       dev         - device number
*       mapArrLen   - number of ports to map, array size
*       mapArrPtr   - pointer to array of mappings
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success,
*       GT_FAIL     - otherwise.
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM- on wrong mapAttLen
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoDxChMaxMappedPortSet
(
    IN  GT_U8  dev,
    IN  GT_U32 mapArrLen,
    IN  CPSS_DXCH_PORT_MAP_STC *mapArrPtr
)
{
    GT_U32 i;
    GT_U32 tempMax = 0;
    GT_U32  devIdx; /* index to appDemoPpConfigList */
    GT_STATUS rc;

    if (NULL == mapArrPtr)
    {
        return GT_BAD_PTR;
    }

    if(mapArrLen == 0)
    {
        return GT_BAD_PARAM;
    }

    /* update current max mapped port number */
    for(i = 0; i < mapArrLen; i++)
    {
        if(mapArrPtr[i].physicalPortNumber > tempMax)
        {
            tempMax = mapArrPtr[i].physicalPortNumber;
        }
    }

    rc = appDemoDevIdxGet(dev, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    /* in order to be backward compatible with current port loops - increment max by 1 */
    appDemoPpConfigList[devIdx].maxPortNumber = (tempMax + 1);

    /* keep port mapping */

    appDemoPpConfigList[devIdx].portsMapArrPtr = mapArrPtr;
    appDemoPpConfigList[devIdx].portsMapArrLen = mapArrLen;

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChDbReset
*
* DESCRIPTION:
*       DxCh DB reset. (part of 'System reset')
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
GT_STATUS appDemoDxChDbReset
(
    IN  GT_U8  devNum
)
{
    GT_STATUS       rc;         /* To hold funcion return code  */
    CPSS_PORTS_BMP_STC portBmp;/* bmp for port */
    GT_U32          port;/* port iterator */
    struct {
        GT_U32  regAddr;
        GT_U32  value;
    } mgDecoding[] = {
        /* Unit Default ID (UDID) Register */
        { 0x00000204, 0 },
        /* Window n registers (n=0..5)
         *    Base Address,
         *    Size,
         *    High Address Remap,
         *    Window Control
         */
        { 0x020c, 0 }, { 0x0210, 0 }, { 0x23c, 0}, { 0x0254, 0 },
        { 0x0214, 0 }, { 0x0218, 0 }, { 0x240, 0}, { 0x0258, 0 },
        { 0x021c, 0 }, { 0x0220, 0 }, { 0x244, 0}, { 0x025c, 0 },
        { 0x0224, 0 }, { 0x0228, 0 }, { 0x248, 0}, { 0x0260, 0 },
        { 0x022c, 0 }, { 0x0230, 0 }, { 0x24c, 0}, { 0x0264, 0 },
        { 0x0234, 0 }, { 0x0248, 0 }, { 0x250, 0}, { 0x0258, 0 },
        { 0, 0 }/* END */
    };
    GT_U32          ii;
    GT_UINTPTR      deviceBaseAddrOnPex;
    GT_U32          doMgDecodingRestore;
    CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel; /* DIAG device channel access type */
    GT_U32          value;
    GT_BOOL         doByteSwap;

    GT_U32  devIdx; /* index to appDemoPpConfigList */

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0)
    {
        return (GT_STATUS)GT_ERROR;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
    {
        /* all SIP5 devices should use PEX_MBUS driver with 8 regions address completion. */
        ifChannel = CPSS_CHANNEL_PEX_MBUS_E;
    }
    else
    {
        ifChannel = CPSS_CHANNEL_PEX_E;
    }

#ifdef ASIC_SIMULATION
    doMgDecodingRestore = 0;
#else
    doMgDecodingRestore = 1;
#endif

    /*osPrintf("appDemoDxChDbReset - start \n");*/

#ifdef IMPL_IP
    /* IP LPM DB Clear - main LBM DB deleted, all prefixes removed, all VR removed */
    rc = prvIpLpmLibReset_Ram();
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvIpLpmLibReset_Ram", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
#endif /*IMPL_IP*/

#ifdef IMPL_TM
    if(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle != NULL)
    {
        /* TM DB Clear */
        rc = cpssTmClose(devNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmClose", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
#endif /*IMPL_TM*/


    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portBmp);

    /* power down all ports */
    for(port = 0 ; port < CPSS_MAX_PORTS_NUM_CNS /*support 256 ports in bc2*/; port++)
    {
        /* set the port in bmp */
        CPSS_PORTS_BMP_PORT_SET_MAC(&portBmp,port);
        rc = cpssDxChPortModeSpeedSet(devNum, portBmp, GT_FALSE,
                            /*don't care*/CPSS_PORT_INTERFACE_MODE_SGMII_E,
                            /*don't care*/CPSS_PORT_SPEED_1000_E);
        if (rc != GT_OK)
        {
            /* do not break .. maybe port not exists but we not have here knowledge about it */
            if(port < PRV_CPSS_PP_MAC(devNum)->numOfPorts)
            {
                /* give indication about ports that are supposed to be in 'valid range'
                    note that for BC2 ports 60..63 could give 'false alarm' indication */
                /*osPrintf("cpssDxChPortModeSpeedSet : failed on port[%d] \n",
                    port);*/
            }
        }

        /* remove the port from bmp */
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portBmp,port);
    }

    rc = appDemoDevIdxGet(devNum, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    deviceBaseAddrOnPex = appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
    doByteSwap = GT_FALSE;

    if(doMgDecodingRestore)
    {
        /* read the registers */
        for(ii = 0 ; mgDecoding[ii].regAddr != 0; ii++)
        {
            rc = prvCpssDrvHwPpReadRegister(devNum,mgDecoding[ii].regAddr,&(mgDecoding[ii].value));
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssDrvHwPpReadRegister", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        rc = cpssDxChDiagRegRead(deviceBaseAddrOnPex,
                    ifChannel,
                    CPSS_DIAG_PP_REG_INTERNAL_E,0x50,
                                 &value, doByteSwap);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChDiagRegRead", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(value == 0xAB110000)
        {
            doByteSwap = GT_TRUE;
        }
    }

    /* Disable All Skip Reset options ,exclude PEX */
    /* this Enable Skip Reset for PEX */
    rc = cpssDxChHwPpSoftResetSkipParamSet(devNum, CPSS_HW_PP_RESET_SKIP_TYPE_ALL_EXCLUDE_PEX_E, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpSoftResetSkipParamSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChHwPpSoftResetTrigger(devNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpSoftResetTrigger", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChCfgDevRemove(devNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgDevRemove", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* let the device minimal time for reset before we restore next registers */
    osTimerWkAfter(1);

    if(doMgDecodingRestore)
    {
        rc = cpssDxChDiagRegRead(deviceBaseAddrOnPex,
                    ifChannel,
                    CPSS_DIAG_PP_REG_INTERNAL_E,0x50,
                                 &value, doByteSwap);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChDiagRegRead", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /*osPrintf("cpssDxChDiagRegRead:deviceBaseAddrOnPex[0x%x] reg[0x50] with value[%d] doByteSwap[%d] \n",
            deviceBaseAddrOnPex,value,doByteSwap);*/


        /* restore the registers of :
           <MG_IP> MG_IP/MG Registers/Address Decoding/Base Address %n
           0x0000020C + n*8: where n (0-5) represents BA

           and:
           <MG_IP> MG_IP/MG Registers/Address Decoding/Size (S) %n
            0x00000210 + n*0x8: where n (0-5) represents SR

           and:
           <MG_IP> MG_IP/MG Registers/Address Decoding/High Address Remap %n
            0x0000023c + n*0x4: where n (0-5) represents HA


           and :
           <MG_IP> MG_IP/MG Registers/Address Decoding/Window Control Register%n
           0x00000254 + n*4: where n (0-5) represents n
        */

        /* write the saved registers */
        /* since the device was removed from cpss ...
           we must use the 'diag' functions to write to the device ... */
        /* restore the registers */
        for(ii = 0 ; mgDecoding[ii].regAddr != 0; ii++)
        {
            rc = cpssDxChDiagRegWrite(deviceBaseAddrOnPex,
                    ifChannel,
                    CPSS_DIAG_PP_REG_INTERNAL_E,
                    mgDecoding[ii].regAddr,
                    mgDecoding[ii].value,
                    doByteSwap);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChDiagRegWrite", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }


    /*osPrintf("appDemoDxChDbReset - ended \n");*/

    return GT_OK;
}

/*******************************************************************************
* appDemoCaelumIsTrafficRunCheck
*
* DESCRIPTION:
*       Function checks that it's called under traffic. The BM counter is used
*       to understand existence of traffic.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       isTrafficRunPtr - (pointer to) is traffic run:
*                           GT_FALSE - there is no traffic in device
*                           GT_TRUE  - there is traffic in device
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoCaelumIsTrafficRunCheck
(
    IN GT_U8         devNum,
    OUT GT_BOOL     *isTrafficRunPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr = 0x16000204; /* RX DMA Allocations counter in BM.
                                       Incremented on each packet came to ingress pipe */
    GT_U32    count1, count2;

    rc = cpssDrvPpHwRegisterRead(devNum, 0, regAddr, &count1);
    if(rc != GT_OK)
    {
        return rc;
    }

    osTimerWkAfter(10);

    rc = cpssDrvPpHwRegisterRead(devNum, 0, regAddr, &count2);
    if(rc != GT_OK)
    {
        return rc;
    }

    *isTrafficRunPtr = (count1 != count2) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* prvAppDemoCaelumEgressCntrReset
*
* DESCRIPTION:
*       Function resets TxQ Egress counters on Caelum devices.
*       The function does next:
*       1.        Take two ports with same speed (enhanced UT uses 10G KR ports 56, 57)
*       2.        Take one VLAN (4095)
*       3.        Save configuration of ports, VLANs, other
*       4.        Use packet generator feature (cpssDxChDiagPacketGeneratorConnectSet,
*           cpssDxChDiagPacketGeneratorTransmitEnable) and send 64K packets of
*           each type that egress counters need.
*           a.        The erratum is:
*             i.         read counter from CPU returns 16 LSBs from previous state IF
*                    counter was not changed after previous read.
*             ii.        16 MSBs always correct.
*             iii.        Value of counter is read correct when counter was changed after
*                   last read.
*             iv.        Each read of counter always resets it?s value inside of device.
*           b.        The WA sends 0x10000 packets per specific counter. This changes
*               counter inside device to be  0x10000 (16 LSBs are ZERO)
*           c.        The following read of counter return value 0x10000 to CPU
*               (see 4-a-iii above).
*           d.        The next read of counter return 0 because 16 MSBs are always correct
*               and 16 LSBs are zero anyway.
*        5.        Restore configuration
*
* INPUTS:
*       devNum - device number
*       pgPort - packet generator port number
*       egrPort - egress port number
*       vid     - vlan ID for the WA
*       cntrSetNum - counter set number to be reset
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvAppDemoCaelumEgressCntrReset
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  pgPort,
    IN GT_PORT_NUM  egrPort,
    IN GT_U16       vid,
    IN GT_U32       cntrSetNum
)
{
    GT_STATUS rc;
    CPSS_PORTS_BMP_STC                   portsMembers;
    CPSS_PORTS_BMP_STC                   portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC          vlanInfo;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC pgConfig;
    GT_ETHERADDR                         macDa;
    CPSS_PORTS_BMP_STC                   portsMembersSave;
    CPSS_PORTS_BMP_STC                   portsTaggingSave;
    CPSS_DXCH_BRG_VLAN_INFO_STC          vlanInfoSave;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmdSave;
    GT_BOOL                              isValidSave;
    GT_BOOL                              egrPortLinkStateSave;
    GT_BOOL                              pgPortLoopbackStateSave;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC mirrInfSave;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC mirrInf;
    GT_HW_DEV_NUM                        hwDev;
    GT_U32                               ii;
    CPSS_PORT_EGRESS_CNT_MODE_ENT        egrCntrSetModeBmp[2];
    GT_PHYSICAL_PORT_NUM                 egrCntrPortNum[2];
    GT_U16                               egrCntrVlanId[2];
    GT_U8                                egrCntrTc[2];
    CPSS_DP_LEVEL_ENT                    egrCntrDpLevel[2];
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT    profileSetSave;
    GT_BOOL                              enableSharingSave, tailDropEnableSave,
                                         ingressPolicyEnableSave, egressPolicyEnableSave,
                                         ttiEthEnableSave;
    GT_U32                               portMaxBuffLimitSave;
    GT_U32                               portMaxDescrLimitSave;
    CPSS_PORT_EGRESS_CNTR_STC            egrCntr;
    GT_U16                               portVidSave;

    /* read to clear counters in HW */
    rc = cpssDxChPortEgressCntrsGet(devNum, (GT_U8)cntrSetNum, &egrCntr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* store counters configuration */
    for(ii = 0; ii < 2; ii++)
    {
        rc = cpssDxChPortEgressCntrModeGet(devNum, (GT_U8)ii, &egrCntrSetModeBmp[ii],
                                           &egrCntrPortNum[ii], &egrCntrVlanId[ii],
                                           &egrCntrTc[ii], &egrCntrDpLevel[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for(ii = 0; ii < 2; ii++)
    {
        if(cntrSetNum == ii)
        {
            /* set counters configuration to count all packets */
            rc = cpssDxChPortEgressCntrModeSet(devNum, (GT_U8)ii, 0, 0, 0, 0, 0);
        }
        else
        {
            /* set counters configuration to NOT count all packets */
            rc = cpssDxChPortEgressCntrModeSet(devNum, (GT_U8)ii, CPSS_EGRESS_CNT_PORT_E | CPSS_EGRESS_CNT_VLAN_E, 255, 0, 0, 0);
        }

        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* save VLAN configuration */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembersSave);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsTaggingSave);
    cpssOsMemSet(&portsTaggingCmdSave, 0, sizeof(portsTaggingCmdSave));
    cpssOsMemSet(&vlanInfoSave, 0, sizeof(vlanInfoSave));
    rc = cpssDxChBrgVlanEntryRead(devNum, vid, &portsMembers, &portsTagging, &vlanInfo, &isValidSave, &portsTaggingCmd);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* store port VID on PG port */
    rc = cpssDxChBrgVlanPortVidGet(devNum, pgPort, CPSS_DIRECTION_INGRESS_E, &portVidSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set port VID on PG port */
    rc = cpssDxChBrgVlanPortVidSet(devNum, pgPort, CPSS_DIRECTION_INGRESS_E, vid);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* store and disable engines like PCL, TTI */
    rc = cpssDxChPclIngressPolicyEnableGet(devNum, &ingressPolicyEnableSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxCh2PclEgressPolicyEnableGet(devNum, &egressPolicyEnableSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChTtiPortLookupEnableGet(devNum,pgPort,CPSS_DXCH_TTI_KEY_ETH_E, &ttiEthEnableSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPclIngressPolicyEnable(devNum, GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxCh2PclEgressPolicyEnable(devNum, GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChTtiPortLookupEnableSet(devNum,pgPort,CPSS_DXCH_TTI_KEY_ETH_E, GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* store MAC loopback state on PG (Packet Generator) port */
    rc = cpssDxChPortInternalLoopbackEnableGet(devNum, pgPort, &pgPortLoopbackStateSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* enable MAC loopback on PG port */
    if(!pgPortLoopbackStateSave)
    {
        rc = cpssDxChPortInternalLoopbackEnableSet(devNum, pgPort, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* store link state on egress port */
    rc = cpssDxChPortLinkStatusGet(devNum, egrPort, &egrPortLinkStateSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(!egrPortLinkStateSave)
    {
        /* force link UP on egress port */
        rc = cpssDxChPortForceLinkPassEnableSet(devNum, egrPort, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* create VLAN with no ports and all ports are 'untagged' */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembers);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsTagging);

    /* clear portsTaggingCmd */
    cpssOsMemSet(&portsTaggingCmd, 0, sizeof(portsTaggingCmd));
    cpssOsMemSet(&vlanInfo, 0, sizeof(vlanInfo));
    vlanInfo.unregIpmEVidx = vlanInfo.floodVidx = 0xFFF;

    rc = cpssDxChBrgVlanEntryWrite(devNum, vid, &portsMembers, &portsTagging, &vlanInfo, &portsTaggingCmd);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* add egress port to VLAN */
    rc = cpssDxChBrgVlanMemberAdd(devNum,vid,egrPort, GT_FALSE, CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* assign UC MAC DA - 00:00:00:00:00:11 */
    cpssOsMemSet(&macDa, 0, sizeof(macDa));
    macDa.arEther[5] = 0x11;

    /* configure packet generator */
    cpssOsMemSet(&pgConfig, 0, sizeof(pgConfig));
    pgConfig.macSa.arEther[5]      = 0x22;
    pgConfig.vlanTagEnable         = GT_TRUE;
    pgConfig.vid                   = vid;
    pgConfig.packetLengthType      = CPSS_DIAG_PG_PACKET_LENGTH_CONSTANT_E;
    pgConfig.packetLength          = 64;
    pgConfig.transmitMode          = CPSS_DIAG_PG_TRANSMIT_SINGLE_BURST_E;
    pgConfig.packetCount           = 1; /* send 64K packets */
    pgConfig.packetCountMultiplier = CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_64K_E;
    pgConfig.ipg                   = 20;
    pgConfig.interfaceSize         = CPSS_DIAG_PG_IF_SIZE_DEFAULT_E;
    pgConfig.payloadType           = CPSS_DIAG_PG_PACKET_PAYLOAD_CONSTANT_E;

    /*******************************************************************************/
    /* clean outMcFrames by send 64K MC packets to VLAN with one port UP           */
    /*******************************************************************************/

    /* assign MC address - FF:00:00:00:00:00 */
    pgConfig.macDa.arEther[0]      = 0xFF;

    rc = cpssDxChDiagPacketGeneratorConnectSet(devNum, pgPort,GT_TRUE, &pgConfig);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* start transmit */
    rc = cpssDxChDiagPacketGeneratorTransmitEnable(devNum, pgPort, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* provide time for burst transmission */
    osTimerWkAfter(100);

    /*******************************************************************************/
    /* clean outUcFrames and txqFilterDisc by send 128K UC packets to VLAN with    */
    /* one port UP and Zero Tail Drop Limits. Half of packets will be dropped      */
    /*******************************************************************************/
    pgConfig.packetCount           = 2; /* send 128K for tail drop case */
    pgConfig.macDa                 = macDa; /* UC MAC DA */
    rc = cpssDxChDiagPacketGeneratorConnectSet(devNum, pgPort, GT_TRUE, &pgConfig);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* store tail drop state */
    rc = cpssDxChPortTxTailDropUcEnableGet(devNum, &tailDropEnableSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* enable tail drop if needed */
    if(!tailDropEnableSave)
    {
        rc = cpssDxChPortTxTailDropUcEnableSet(devNum, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* store tail drop profile for egress port */
    rc = cpssDxChPortTxBindPortToDpGet(devNum, egrPort, &profileSetSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set tail drop profile #16 for egress port */
    rc = cpssDxChPortTxBindPortToDpSet(devNum, egrPort, CPSS_PORT_TX_DROP_PROFILE_16_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* store tail drop thresholds */
    rc = cpssDxChPortTxTailDropProfileGet(devNum, CPSS_PORT_TX_DROP_PROFILE_16_E,
                                          &enableSharingSave, &portMaxBuffLimitSave,
                                          &portMaxDescrLimitSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* reset tail drop thresholds */
    rc = cpssDxChPortTxTailDropProfileSet(devNum, CPSS_PORT_TX_DROP_PROFILE_16_E,
                                          enableSharingSave, 0, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* start transmit */
    rc = cpssDxChDiagPacketGeneratorTransmitEnable(devNum, pgPort, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* provide time for burst transmission */
    osTimerWkAfter(100);

    /* restore tail drop if needed */
    if(!tailDropEnableSave)
    {
        rc = cpssDxChPortTxTailDropUcEnableSet(devNum, tailDropEnableSave);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* restore tail drop profile for egress port */
    rc = cpssDxChPortTxBindPortToDpSet(devNum, egrPort, profileSetSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* restore tail drop thresholds */
    rc = cpssDxChPortTxTailDropProfileSet(devNum, CPSS_PORT_TX_DROP_PROFILE_16_E,
                                          enableSharingSave, portMaxBuffLimitSave,
                                          portMaxDescrLimitSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*******************************************************************************/
    /* clean outBcFrames by send 64K UC packets to VLAN with one port UP           */
    /*******************************************************************************/

    /* assign BC address */
    cpssOsMemSet(&pgConfig.macDa, 0xFF, sizeof(pgConfig.macDa));
    pgConfig.packetCount           = 1; /* send 64K packets */
    rc = cpssDxChDiagPacketGeneratorConnectSet(devNum, pgPort,GT_TRUE, &pgConfig);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* start transmit */
    rc = cpssDxChDiagPacketGeneratorTransmitEnable(devNum, pgPort, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* provide time for burst transmission */
    osTimerWkAfter(100);

    /*******************************************************************************/
    /* clean mcFifo3_0DropPkts by send 64K packets to VLAN without ports           */
    /* and clean outCtrlFrames by send packets to Rx Analyzer also                 */
    /*******************************************************************************/

    /* enable ingress mirror to analyzer 0 */
    rc = cpssDxChBrgVlanIngressMirrorEnable(devNum, vid, GT_TRUE, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChMirrorAnalyzerInterfaceGet(devNum, 0, &mirrInfSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChCfgHwDevNumGet(devNum, &hwDev);
    if (GT_OK != rc)
    {
        return rc;
    }

    mirrInf.interface.type = CPSS_INTERFACE_PORT_E;
    mirrInf.interface.devPort.hwDevNum = hwDev;
    mirrInf.interface.devPort.portNum  = egrPort;

    rc = cpssDxChMirrorAnalyzerInterfaceSet(devNum, 0, &mirrInf);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChBrgVlanPortDelete(devNum,vid,egrPort);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* start transmit */
    rc = cpssDxChDiagPacketGeneratorTransmitEnable(devNum, pgPort, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* provide time for burst transmission */
    osTimerWkAfter(100);

    /*******************************************************************************/
    /* clean brgEgrFilterDisc by send 64K UC packets to disabled Rx Analyzer       */
    /*******************************************************************************/
    rc = cpssDxChPortEnableSet(devNum,egrPort,GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* start transmit */
    rc = cpssDxChDiagPacketGeneratorTransmitEnable(devNum, pgPort, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* provide time for burst transmission */
    osTimerWkAfter(100);

    /*******************************************************************************/
    /* restore configuration                                                       */
    /*******************************************************************************/
    rc = cpssDxChDiagPacketGeneratorConnectSet(devNum, pgPort, GT_FALSE, &pgConfig);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* reset MAC MIB Counters in HW without update shadow
       it must be done before Port APIs call because some of them stores MAC counters */
    rc = prvCpssDxChPortMacCountersOnPortGet(devNum, pgPort, GT_FALSE, NULL,
                                             PRV_DXCH_PORT_MAC_CNTR_READ_MODE_RESET_HW_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* reset MAC MIB Counters in HW without update shadow
       it must be done before Port APIs call because some of them stores MAC counters */
    rc = prvCpssDxChPortMacCountersOnPortGet(devNum, egrPort, GT_FALSE, NULL,
                                             PRV_DXCH_PORT_MAC_CNTR_READ_MODE_RESET_HW_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortEnableSet(devNum,egrPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* restore port VID */
    rc = cpssDxChBrgVlanPortVidSet(devNum, pgPort, CPSS_DIRECTION_INGRESS_E, portVidSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(isValidSave)
    {
        rc = cpssDxChBrgVlanEntryWrite(devNum, vid, &portsMembers, &portsTagging, &vlanInfo, &portsTaggingCmd);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = cpssDxChBrgVlanEntryInvalidate(devNum, vid);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(!pgPortLoopbackStateSave)
    {
        rc = cpssDxChPortInternalLoopbackEnableSet(devNum, pgPort, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(!egrPortLinkStateSave)
    {
        rc = cpssDxChPortForceLinkPassEnableSet(devNum, egrPort, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    rc = cpssDxChMirrorAnalyzerInterfaceSet(devNum, 0, &mirrInfSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* restore and disable engines like PCL, TTI */
    rc = cpssDxChPclIngressPolicyEnable(devNum, ingressPolicyEnableSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxCh2PclEgressPolicyEnable(devNum, egressPolicyEnableSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChTtiPortLookupEnableSet(devNum,pgPort,CPSS_DXCH_TTI_KEY_ETH_E, ttiEthEnableSave);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* restore counters configuration */
    for(ii = 0; ii < 2; ii++)
    {
        rc = cpssDxChPortEgressCntrModeSet(devNum, (GT_U8)ii, egrCntrSetModeBmp[ii],
                                           egrCntrPortNum[ii], egrCntrVlanId[ii],
                                           egrCntrTc[ii], egrCntrDpLevel[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* read to clear counters in HW */
    rc = cpssDxChPortEgressCntrsGet(devNum, (GT_U8)cntrSetNum, &egrCntr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/* ports numbers for Caelum egress counters WA */
static GT_PORT_NUM     appDemoCaelumPgPort = 56;/* must be first in the quad of ports */
static GT_PORT_NUM     appDemoCaelumEgrPort = 57;

/*******************************************************************************
* appDemoCaelumEgressCntrWaPortsSet
*
* DESCRIPTION:
*       Function sets ports numbers for Caelum egress counters WA.
*
* INPUTS:
*       newPgPort  - new packet generator port number
*       newEgrPort - new egress port number
*
* OUTPUTS:
*       none
*
* RETURNS:
*       none
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void appDemoCaelumEgressCntrWaPortsSet
(
    IN GT_PORT_NUM     newPgPort,
    IN GT_PORT_NUM     newEgrPort
)
{
   appDemoCaelumPgPort = newPgPort;
   appDemoCaelumEgrPort = newEgrPort;
   osPrintf("Caelum WA ports: PG Port[%d] Egress Port[%d]\n", appDemoCaelumPgPort, appDemoCaelumEgrPort);
}

/*******************************************************************************
* appDemoCaelumEgressCntrReset
*
* DESCRIPTION:
*       Function resets TxQ Egress counters on Caelum devices.
*
* INPUTS:
*       devNum - device number
*       cntrSetNum - counter set number to be reset
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK         - on success,
*       GT_BAD_STATE  - traffic is still run in device. Cannot execute the WA.
*       GT_FAIL       - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoCaelumEgressCntrReset
(
    IN GT_U8        devNum,
    IN GT_U32       cntrSetNum
)
{
    GT_STATUS       rc;
    GT_U16          vid;
    GT_BOOL         isTrafficRun;

    /* supported only for Cetus and Caelum */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_BOBCAT2_E ||
       PRV_CPSS_PP_MAC(devNum)->devSubFamily != CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
        return GT_OK;

    rc = appDemoCaelumIsTrafficRunCheck(devNum, &isTrafficRun);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(isTrafficRun)
    {
        return GT_BAD_STATE;
    }

    /* store MAC MIB Counters in shadow because prvAppDemoCaelumEgressCntrReset changes them */
    rc = prvCpssDxChPortMacCountersOnPortGet(devNum,appDemoCaelumPgPort, GT_FALSE, NULL,
                                             PRV_DXCH_PORT_MAC_CNTR_READ_MODE_UPDATE_SHADOW_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* store MAC MIB Counters in shadow because prvAppDemoCaelumEgressCntrReset changes them */
    rc = prvCpssDxChPortMacCountersOnPortGet(devNum,appDemoCaelumEgrPort, GT_FALSE, NULL,
                                             PRV_DXCH_PORT_MAC_CNTR_READ_MODE_UPDATE_SHADOW_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    vid     = 4095;
    rc = prvAppDemoCaelumEgressCntrReset(devNum, appDemoCaelumPgPort, appDemoCaelumEgrPort, vid, cntrSetNum);
    if(rc != GT_OK)
    {
        osPrintf("prvAppDemoCaelumEgressCntrReset fails rc = \n", rc);
        return rc;
    }

    return rc;
}

/*******************************************************************************
* appDemoDxChPortBc2PtpTimeStampFixWa
*
* DESCRIPTION:
*   Set the thresholds in ports Tx FIFO
*
* APPLICABLE DEVICES:
*        Bobcat2
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       WA fix PTP timestamp problem
*
*******************************************************************************/
GT_STATUS appDemoDxChPortBc2PtpTimeStampFixWa
(
    IN   GT_U8                            devNum
)
{
    CPSS_DXCH_IMPLEMENT_WA_ENT      waArr[1];   /* array of workarounds to enable */
    GT_STATUS                       rc;         /* return code */

    waArr[0] = CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E;
    rc = cpssDxChHwPpImplementWaInit(devNum, 1, &waArr[0], NULL);

    return rc;
}

