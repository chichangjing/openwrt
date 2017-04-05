/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDb98Ex240Config.c
*
* DESCRIPTION:
*       Initialization functions for the ExMxPm 98Ex240 boards.
*
*
* FILE REVISION NUMBER:
*       $Revision: 47 $
*
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>
#include <appDemo/sysHwConfig/appDemoDb.h>

#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

#include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmHwInit.h>
#include <cpss/exMxPm/exMxPmGen/config/cpssExMxPmCfg.h>

#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgVlan.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgMc.h>
#include <cpss/exMxPm/exMxPmGen/inlif/cpssExMxPmInlif.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgStp.h>
#include <cpss/exMxPm/exMxPmGen/policer/cpssExMxPmPolicer.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortTx.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricConfig.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricVoq.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricXbar.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricHGLink.h>
#include <cpss/exMxPm/exMxPmGen/fabric/private/prvCpssExMxPmFabricInfo.h>
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/private/prvCpssExMxPmHwInit.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
#include <cpss/exMxPm/exMxPmGen/intTcam/cpssExMxPmIntTcam.h>
#include <cpss/exMxPm/exMxPmGen/vb/cpssExMxPmVb.h>
#include <cpss/exMxPm/exMxPmGen/phy/cpssExMxPmPhySmi.h>
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmLedCtrl.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgGen.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

/* number of cascade ports in the DX devices */
#define NUM_DX_CSCD_PORTS_CNS   8
/* number of network ports in the DX devices */
#define NUM_DX_NET_PORTS_CNS    16
CPSS_TBD_BOOKMARK /* need to implement function that return this !!! */
#define DUMMY_IMPLEMENTATION    GT_OK

/* LPM memory configuration */
#define LPM_MEMORY_MODE      APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E
/*#define LPM_MEMORY_MODE      APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E*/

/*******************************************************************************
 * External variables
 ******************************************************************************/

/* memory allocation */
extern void *malloc(size_t);

static GT_STATUS gtDb98Ex240Config_afterInitBoardConfig_inVb(IN  GT_U8 boardRevId);

/* private Fabric Initial Configuration function declaration */
GT_STATUS prvCpssExMxPmInitialFaConfig
(
IN  GT_U8          devNum
);

#ifndef ASIC_SIMULATION
/* private Phy Initial Configuration function declaration */
GT_STATUS prvCpssExMxPmInitialPhyConfig
(
IN  GT_U8          devNum
);
#endif

/* private TX Lanes Swap  & LED0/1 Controll function declaration */
GT_STATUS prvCpssExMxPmSerdesTxSwpSelAndLedCntrlSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      boardRevId
);

/* private periodic flow control disable function declaration */
GT_STATUS prvCpssExMxPmPeriodicFlowControlDisableSet
(
    IN  GT_U8     devNum
);

/* User configured bitmap for SERDES power down */
GT_U32 appDemoExMxPmPowerDownBmp = 0;

#define BMP_CONTINUES_PORTS_MAC(x)  ((1<<(x)) - 1)

/* bmp of 52 ports 0..51 -- physical ports (not virtual) */
#define BMP_PORTS_MORE_32_MAC(portsBmpPtr,numPorts) \
    portsBmpPtr->ports[0] = 0xFFFFFFFF;    \
    portsBmpPtr->ports[1] = BMP_CONTINUES_PORTS_MAC(numPorts - 32)

/* bmp of less then 32 ports -- physical ports (not virtual) */
#define BMP_PORTS_LESS_32_MAC(portsBmpPtr,numPorts)    \
    portsBmpPtr->ports[0] = BMP_CONTINUES_PORTS_MAC(numPorts);  \
    portsBmpPtr->ports[1] = 0

/* macro to set range of ports in bmp --
   NOTE :  it must be (startPort + numPorts) <= 31
*/
#define PORT_RANGE_MAC(startPort,numPorts)\
    (BMP_CONTINUES_PORTS_MAC(numPorts) << startPort)

static const CPSS_PORTS_BMP_STC portsBmp24to25 = {{PORT_RANGE_MAC(24,2) ,0}};

static CPSS_REG_VALUE_INFO_STC   pumaEX240QdrRegValInfoList[]= GT_PUMA_QDR_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   pumaEX240RldRegValInfoList[]= GT_PUMA_RLD_REG_VAL_INFO_LIST;
static GT_U32 rldPcsTxLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS] = {0x5cc,0x4cc,0x3cc,0x2cc,0x1cc,0xcc};
static GT_U32 qdrPcsTxLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS] = {0xcc,0x1cc,0x2cc,0x3cc,0x4cc,0x5cc};

/*flag to state if we init the device as part of VB */
GT_U32  exMxPmAsVb = 0;
GT_U32  exMxPmBoardVbDevicesExists = 0;

/*******************************************************************************
* appDemoExMxPmPowerDownBmpSet
*
* DESCRIPTION:
*       Debug function that sets AppDemo power down SERDES bitmask.
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
GT_STATUS appDemoExMxPmPowerDownBmpSet
(
    IN GT_U32   powerDownBmp
)
{
    appDemoExMxPmPowerDownBmp = powerDownBmp;
    return GT_OK;
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
    GT_STATUS   retVal;                 /* Function return value.       */
    GT_U8       i;                      /* Loop index.                  */
    GT_U8    ppCounter;                 /* Number of Packet processors. */

    retVal = appDemoSysGetPciInfo();
    if (retVal != GT_OK)
    {
        return retVal;
    }

    ppCounter = 0;
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if (appDemoPpConfigList[i].valid == GT_TRUE)
        {

            /* check that device is not FX950 (Fabric Adapter) one */
            if (appDemoPpConfigList[i].devFamily == APPDEMO_FX950_FAMILY_TYPE)
            {
                appDemoPpConfigList[i].valid = GT_FALSE;

                continue;
            }

            ppCounter++;
        }
    }

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    if (ppCounter > 1)
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
    localPpPh1Config.devNum             = devIdx;
    localPpPh1Config.baseAddr           =
    appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
    localPpPh1Config.internalPciBase    =
    appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
    localPpPh1Config.deviceId           =
    ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
     (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

    extDrvGetPciIntVec(GT_PCI_INT_D,&intVecNum);
    localPpPh1Config.intVecNum  = (GT_U32)((GT_UINTPTR)intVecNum);
    extDrvGetIntMask(GT_PCI_INT_D,&(localPpPh1Config.intMask));

    /* override BSP settings for vxWorks */
#if (!(defined _linux) && !(defined _FreeBSD) && !(defined ASIC_SIMULATION) && (CPU == ARMARCH5))

{
    GT_BOOL pexSwichOnBoard = GT_FALSE;
    GT_STATUS rc;
    rc = gtPresteraIsPexSwitchOnBoard(&pexSwichOnBoard);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (pexSwichOnBoard == GT_FALSE)
    {
        localPpPh1Config.intVecNum  = 33;
        localPpPh1Config.intMask = 33;
    }
    else
    {
        localPpPh1Config.intVecNum  = 35;
        localPpPh1Config.intMask = 35;

    }
}
#endif

    localPpPh1Config.mngInterfaceType                      = CPSS_CHANNEL_PEX_E;

    /* retrieve PP Core Clock from HW */
    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

    localPpPh1Config.powerDownPortsBmp.ports[0] = appDemoExMxPmPowerDownBmp;

    switch (LPM_MEMORY_MODE)
    {
    case APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E:
        localPpPh1Config.routingSramCfgType = CPSS_TWIST_EXTERNAL_E;
        localPpPh1Config.numOfSramsCfg = 3;

        localPpPh1Config.sramsCfg[0].sramSize       = _2MB;
        localPpPh1Config.sramsCfg[0].sramFrequency  = CPSS_SRAM_FREQ_250_MHZ_E;
        localPpPh1Config.sramsCfg[0].sramRate       = CPSS_SRAM_RATE_DDR_E;
        localPpPh1Config.sramsCfg[0].externalMemory = CPSS_EXTERNAL_MEMORY_0_E;
        localPpPh1Config.sramsCfg[0].sramType       = CPSS_SRAM_TYPE_NARROW_E;

        localPpPh1Config.sramsCfg[1].sramSize       = _2MB;
        localPpPh1Config.sramsCfg[1].sramFrequency  = CPSS_SRAM_FREQ_250_MHZ_E;
        localPpPh1Config.sramsCfg[1].sramRate       = CPSS_SRAM_RATE_DDR_E;
        localPpPh1Config.sramsCfg[1].externalMemory = CPSS_EXTERNAL_MEMORY_1_E;
        localPpPh1Config.sramsCfg[1].sramType       = CPSS_SRAM_TYPE_NARROW_E;

        localPpPh1Config.sramsCfg[2].sramSize       = _2MB;
        localPpPh1Config.sramsCfg[2].sramFrequency  = CPSS_SRAM_FREQ_250_MHZ_E;
        localPpPh1Config.sramsCfg[2].sramRate       = CPSS_SRAM_RATE_DDR_E;
        localPpPh1Config.sramsCfg[2].externalMemory = CPSS_EXTERNAL_MEMORY_2_E;
        localPpPh1Config.sramsCfg[2].sramType       = CPSS_SRAM_TYPE_NARROW_E;
        break;

    case APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E:
        localPpPh1Config.routingSramCfgType = CPSS_TWIST_INTERNAL_E;
        localPpPh1Config.numOfSramsCfg = 0;
        break;

    default:
        return GT_BAD_PARAM;
    }

    localPpPh1Config.controlSramUsedArray[0]    = GT_TRUE;
    localPpPh1Config.controlSramProtectArray[0] = APPDEMO_EXMXPM_CSU_PROTECT_PARITY_E;
    localPpPh1Config.controlSramUsedArray[1]    = GT_TRUE;

    if ((boardRevId == 2) || (boardRevId == 3))
    {
        localPpPh1Config.externalTcamUsed = GT_FALSE;
    }
    else
    {
        localPpPh1Config.externalTcamUsed = GT_TRUE;
    }

    switch (boardRevId) {
    case 1:
    case 2:
        localPpPh1Config.egressBufferRamType = APPDEMO_EXMXPM_RAM_TYPE_SRAM_E;
        localPpPh1Config.egressBufferSramCfg.sramSize = _4M;
        localPpPh1Config.ingressBufferSramCfg.sramSize = 0;
        localPpPh1Config.ingressBufferSize = APPDEMO_EXMXPM_BUFFER_SIZE_256_E;
        localPpPh1Config.egressBufferSize = APPDEMO_EXMXPM_BUFFER_SIZE_512_E;
        break;
    case 3:
    case 4:
        localPpPh1Config.egressBufferRamType = APPDEMO_EXMXPM_RAM_TYPE_RLDRAM_E;
        localPpPh1Config.egressBufferRldramCfg.dramSize = _64M + _8M; /* 72M */
        localPpPh1Config.egressBufferRldramCfg.overwriteDef = GT_FALSE;
        localPpPh1Config.ingressBufferSramCfg.sramSize = _4M;
        localPpPh1Config.ingressBufferSize = APPDEMO_EXMXPM_BUFFER_SIZE_1K_E;
        localPpPh1Config.egressBufferSize = APPDEMO_EXMXPM_BUFFER_SIZE_8K_E;
        break;
    default:
        return GT_BAD_PARAM;
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
    GT_STATUS rc = GT_OK;
    GT_U8 devId;
    for (devId = 0; devId < PRV_CPSS_MAX_PP_DEVICES_CNS ; devId++)
    {
        if ((GT_FALSE == appDemoPpConfigList[devId].valid) ||
            (CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devId].devFamily) == 0))
        {
            continue;
        }
        rc = prvCpssExMxPmMetalFixRev2RegisterConfig(devId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssExMxPmMetalFixRev2RegisterConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

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
    CPSS_PP_PHASE2_INIT_PARAMS    localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS   rc = GT_OK;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = appDemoPpConfigList[oldDevNum].deviceId;

    localPpPh2Config.baseAddr   =
    appDemoPpConfigList[oldDevNum].pciInfo.pciBaseAddr;
    if( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E) ||
        ((systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E)  &&
         ((systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)              &&
            (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE))) )
    {
        rc = appDemoAllocateDmaMem(localPpPh2Config.deviceId,
                                   RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF,
                                   RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,
                                   AU_DESC_NUM_DEF,
                                   &localPpPh2Config);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

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
    GT_U32                          i;           /* loop index */
    CPSS_LED_CONF_STC               ledConf;     /* LED stream configuration parameters */
    CPSS_LED_CLASS_MANIPULATION_STC classParams; /* class manipulation parameters */
    CPSS_LED_GROUP_CONF_STC         groupParams; /* group configuration parameters */
    GT_STATUS                       rc;          /* return code */


    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            ledConf.ledStart            = 0x0;
            ledConf.ledEnd              = 0xFF;
            ledConf.pulseStretch        = CPSS_LED_PULSE_STRETCH_3_E;
            ledConf.blink0Duration      = CPSS_LED_BLINK_DURATION_6_E;
            ledConf.blink0DutyCycle     = CPSS_LED_BLINK_DUTY_CYCLE_1_E;
            ledConf.blink1Duration      = CPSS_LED_BLINK_DURATION_3_E;
            ledConf.blink1DutyCycle     = CPSS_LED_BLINK_DUTY_CYCLE_2_E;
            ledConf.disableOnLinkDown   = GT_TRUE;
            ledConf.ledOrganize         = CPSS_LED_ORDER_MODE_BY_PORT_E;
            ledConf.clkInvert           = GT_FALSE;
            ledConf.class5select        = CPSS_LED_CLASS_5_SELECT_HALF_DUPLEX_E;
            ledConf.class13select       = CPSS_LED_CLASS_13_SELECT_LINK_DOWN_E;

            rc =  cpssExMxPmLedStreamConfigSet(appDemoPpConfigList[i].devNum, 0, &ledConf);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamConfigSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc =  cpssExMxPmLedStreamConfigSet(appDemoPpConfigList[i].devNum, 1, &ledConf);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamConfigSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            classParams.invertEnable    = GT_TRUE;
            classParams.blinkEnable     = GT_TRUE;
            classParams.blinkSelect     = CPSS_LED_BLINK_SELECT_1_E;
            classParams.forceEnable     = GT_FALSE;
            classParams.forceData       = 0x0;

            rc = cpssExMxPmLedStreamClassManipulationSet(appDemoPpConfigList[i].devNum, 0, 4, &classParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamClassManipulationSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssExMxPmLedStreamClassManipulationSet(appDemoPpConfigList[i].devNum, 1, 4, &classParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamClassManipulationSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            classParams.invertEnable    = GT_FALSE;
            classParams.blinkEnable     = GT_FALSE;
            classParams.blinkSelect     = CPSS_LED_BLINK_SELECT_0_E;
            classParams.forceEnable     = GT_TRUE;
            classParams.forceData       = 0x0;

            rc = cpssExMxPmLedStreamClassManipulationSet(appDemoPpConfigList[i].devNum, 0, 6, &classParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamClassManipulationSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssExMxPmLedStreamClassManipulationSet(appDemoPpConfigList[i].devNum, 1, 6, &classParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamClassManipulationSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            groupParams.classA = 4;
            groupParams.classB = 11;
            groupParams.classC = 4;
            groupParams.classD = 1;

            rc = cpssExMxPmLedStreamGroupConfigSet(appDemoPpConfigList[i].devNum, 0, 0, &groupParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamGroupConfigSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssExMxPmLedStreamGroupConfigSet(appDemoPpConfigList[i].devNum, 1, 0, &groupParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamGroupConfigSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            groupParams.classA = 0;
            groupParams.classB = 4;
            groupParams.classC = 0;
            groupParams.classD = 4;

            rc = cpssExMxPmLedStreamGroupConfigSet(appDemoPpConfigList[i].devNum, 0, 1, &groupParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamGroupConfigSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssExMxPmLedStreamGroupConfigSet(appDemoPpConfigList[i].devNum, 1, 1, &groupParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamGroupConfigSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            switch(PRV_CPSS_PP_MAC(appDemoPpConfigList[i].devNum)->devType)
            {
                case PRV_CPSS_PUMA_24G_DEVICES_CASES_MAC:
                        groupParams.classA = 5;
                        groupParams.classB = 5;
                        groupParams.classC = 5;
                        groupParams.classD = 5;

                        rc = cpssExMxPmLedStreamGroupConfigSet(appDemoPpConfigList[i].devNum, 0, 2, &groupParams);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamGroupConfigSet", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }

                        rc = cpssExMxPmLedStreamGroupConfigSet(appDemoPpConfigList[i].devNum, 1, 2, &groupParams);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamGroupConfigSet", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }

                        groupParams.classA = 2;
                        groupParams.classB = 2;
                        groupParams.classC = 2;
                        groupParams.classD = 2;

                        rc = cpssExMxPmLedStreamGroupConfigSet(appDemoPpConfigList[i].devNum, 0, 3, &groupParams);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamGroupConfigSet", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }

                        rc = cpssExMxPmLedStreamGroupConfigSet(appDemoPpConfigList[i].devNum, 1, 3, &groupParams);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmLedStreamGroupConfigSet", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                        break;
                default:
                        break;
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

    /* note lpmDbMemCfg is static in order to save memory allocation and free... */
    static APP_DEMO_CPSS_IP_MEM_CFG_STC  lpmDbMemCfg[1];

    GT_U32  mplsInterfaceMemoryNeeded; /* memory needed for MPLS interface tables */

    localPpCfgParams.numOfTrunks = APP_DEMO_255_TRUNKS_CNS;
    localPpCfgParams.fdbTableInfo.fdbTableMode = CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_INTERNAL_SPLIT_E;
    localPpCfgParams.fdbTableInfo.fdbToLutRatio = 0;/* don't care -- internal memory */
    localPpCfgParams.fdbTableInfo.fdbHashSize = CPSS_EXMXPM_PP_CONFIG_FDB_HASH_SIZE_32K_E;

    /* override MPLS parameters */
    localPpCfgParams.maxNumOfMplsIfs = 10;
    localPpCfgParams.maxNumOfNhlfe = 100;

    /* each MPLS entry takes 72 bits of memory (16 bytes in memory address space) */
    mplsInterfaceMemoryNeeded = localPpCfgParams.maxNumOfMplsIfs * 9;

    /* set LPM DB parameters; note that MPLS interface table is shared with */
    /* IP LPM memory and reside in LPM SRAM 0                               */
    switch (LPM_MEMORY_MODE)
    {
    case APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E:
        lpmDbMemCfg[0].routingSramCfgType = APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
        lpmDbMemCfg[0].numOfSramsSizes    = 3;
        lpmDbMemCfg[0].sramsSizeArray[0]  = CPSS_SRAM_SIZE_2MB_E - mplsInterfaceMemoryNeeded; /* LPM 0 */
        lpmDbMemCfg[0].sramsSizeArray[1]  = CPSS_SRAM_SIZE_2MB_E;   /* LPM 1 */
        lpmDbMemCfg[0].sramsSizeArray[2]  = CPSS_SRAM_SIZE_2MB_E;   /* LPM 2 */
        lpmDbMemCfg[0].sramsSizeArray[3]  = 0;                      /* LPM 3 */
        break;

    case APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E:
        lpmDbMemCfg[0].routingSramCfgType = APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E;
        lpmDbMemCfg[0].numOfSramsSizes    = 4;
        lpmDbMemCfg[0].sramsSizeArray[0]  = (108 * _1K) - mplsInterfaceMemoryNeeded; /* LPM 0 */
        lpmDbMemCfg[0].sramsSizeArray[1]  = 108 * _1K;              /* LPM 1 */
        lpmDbMemCfg[0].sramsSizeArray[2]  = 108 * _1K;              /* LPM 2 */
        lpmDbMemCfg[0].sramsSizeArray[3]  = 108 * _1K;              /* LPM 3 */
        break;

    default:
        return GT_BAD_PARAM;
    }

    localPpCfgParams.lpmDbNumOfMemCfg = 1;
    localPpCfgParams.lpmDbMemCfgArray = lpmDbMemCfg;

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
    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
    localLibInitParams.initMpls                  = GT_FALSE;
    localLibInitParams.initMplsTunnel            = GT_FALSE;
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
    GT_STATUS                               rc = GT_OK;
    GT_U8                                   i;
    GT_U8                                   dev;
    GT_U8                                   port;
    GT_U8                                   numOfPorts;
    GT_U32                                  inlifIndex;
    CPSS_PORTS_BMP_STC                      vidxPortsMembers;
    CPSS_PORTS_BMP_STC                      portsMembers;
    CPSS_PORTS_BMP_STC                      portsTagging;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC           vlanInfo;
    CPSS_EXMXPM_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS tailDropProfile;
    GT_U32                                  profile;
    GT_U32                                  tc;
    GT_U32          devTable[CPSS_EXMXPM_FDB_DEV_TABLE_SIZE_CNS] = {0,0,0,0};
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC         fdbScanOutlifMask;
    GT_BOOL                                 autoLearn = GT_TRUE;
    GT_U32                                  tmpData;

    if(appDemoDbEntryGet("ctrlMacLearn", &tmpData) == GT_OK)
    {
        /* --> use same string as in PSS : values are:
            typedef enum
            {
                GT_AUTOMATIC        = 0,
                GT_CONTROLED_MSSG   = 1,
                GT_CONTROLED_TAG    = 2
            } GT_LEARNING_MODE;
        */
        /* allow to work in controlled learning */
        autoLearn = (GT_BOOL)((tmpData == 0) ? GT_TRUE : GT_FALSE);
    }

    /* go over all devices and virtual ports per device */
    for (i = 0; (i < PRV_CPSS_MAX_PP_DEVICES_CNS) ; i++)
    {
        if (GT_FALSE == appDemoPpConfigList[i].valid)
        {
            continue;
        }

        if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[i].devFamily) == 0)
        {
            /* support for this function called from VB board initialization */
            continue;
        }

        /* get device id and number of virtual ports per device */
        dev = appDemoPpConfigList[i].devNum;

        /* FA HW regs configuration */
        rc = prvCpssExMxPmInitialFaConfig(dev);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssExMxPmInitialFaConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* create vlan 1 */
        numOfPorts = PRV_CPSS_PP_MAC(dev)->numOfVirtPorts;

        switch (PRV_CPSS_PP_MAC(dev)->devType)
        {
        case CPSS_98EX260_CNS:
        case CPSS_98EX2210_CNS:
        case CPSS_98EX8262_CNS:
        case CPSS_98EX2110_CNS:
        case CPSS_98MX2310_CNS :
        case CPSS_98EX8302_CNS:
        case CPSS_98EX8305_CNS:
        case CPSS_98EX8502_CNS:
        case CPSS_98EX8307_CNS:

                portsMembers.ports[0] = 0x3000000;
                break;
            default:
                portsMembers.ports[0] = 0xFFFFFF;
                break;
        }

        portsMembers.ports[1] = 0;
        portsTagging.ports[0] = 0;
        portsTagging.ports[1] = 0;
        if(exMxPmAsVb) /*this flag set to 1 from the VB file*/
        {
            /* running for VB , set all 54 virtual ports members in vlan */
            portsMembers.ports[0] = 0xFFFFFFFF;
            portsMembers.ports[1] = 0x003FFFFF;
        }
        cpssOsMemSet(&vlanInfo, 0, sizeof(vlanInfo));
        vlanInfo.cpuMember = GT_FALSE;
        vlanInfo.ipv4IpmBrgEn = GT_FALSE;
        vlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E;
        vlanInfo.ipv6IpmBrgEn = GT_FALSE;
        vlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_SGV_E;
        vlanInfo.mirrorToTxAnalyzerEn = GT_FALSE;
        vlanInfo.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;
        vlanInfo.stgId = 0;
        {/* configure STG 0 in the STG Table */
            GT_U32      stgEntry[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];
            osMemSet(stgEntry, 0xff, sizeof(GT_U32)*CPSS_EXMXPM_STG_ENTRY_SIZE_CNS);
            rc = cpssExMxPmBrgStpEntryWrite(dev, 0, stgEntry);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgStpEntryWrite", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        rc = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, 0, 0x5f2);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgVlanUntaggedMruProfileValueSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssExMxPmBrgVlanEntryWrite(dev, 1, &portsMembers, &portsTagging, &vlanInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgVlanEntryWrite", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* VLAN Inlif configuration for VLANs
           This settings may be in use only when Port is configured in VLAN InLif mode.
           By default InLif mode is per Port. */
        for (inlifIndex = 0 ; inlifIndex < PRV_CPSS_MAX_NUM_VLANS_CNS; inlifIndex++)
        {
            /* Enable learning */
            rc = cpssExMxPmInlifAutoLearnEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, inlifIndex, autoLearn);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifAutoLearnEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* set New Address is NOT Security Breach */
            rc = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, inlifIndex, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifUnkSaNotSecurBreachEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Enable Bridging */
            rc = cpssExMxPmInlifBridgeEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, inlifIndex, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifBridgeEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* Enable NA send to CPU (only for vlan 1) */
        rc = cpssExMxPmInlifNaToCpuEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, 1, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifNaToCpuEnableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Configuration of flooding regardless of VIDX entry  */
        vidxPortsMembers.ports[0] = 0xFFFFFFFF;
        vidxPortsMembers.ports[1] = 0x003FFFFF;
        rc = cpssExMxPmBrgMcEntryWrite(dev, 0xFFF, GT_TRUE, &vidxPortsMembers);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgMcEntryWrite", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Port InLif configuration for Auto Learning */
        for (port = 0 ; port < numOfPorts; port++)
        {
            /* set PVID = 1*/
            rc = cpssExMxPmBrgVlanPortPvidSet(dev, port, 1);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgVlanPortPvidSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            inlifIndex = port;

            /* set New Address is NOT Security Breach */
            rc = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_PORT_E, inlifIndex, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifUnkSaNotSecurBreachEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Enable NA to CPU */
            rc = cpssExMxPmInlifNaToCpuEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_PORT_E, inlifIndex, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifNaToCpuEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Enable Auto Learning */
            rc = cpssExMxPmInlifAutoLearnEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_PORT_E, inlifIndex, autoLearn);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifAutoLearnEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Enable Bridging */
            rc = cpssExMxPmInlifBridgeEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_PORT_E, inlifIndex, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifBridgeEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* set port's default inLif ID */
            rc = cpssExMxPmInlifPortDefaultInlifIdSet(dev,port,inlifIndex);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifPortDefaultInlifIdSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Enable Auto Learning in Ingress Port Bridge Entry */
            rc = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbPortAutoLearnEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Enable NA To CPU message in Ingress Port Bridge Entry */
            rc = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbPortNaToCpuEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

        }


#ifdef ASIC_SIMULATION
        /* Port MAC configuration */
        for (port = 0 ; port < 27; port++)
        {

            if( !(PRV_CPSS_PP_MAC(dev)->existingPorts.ports[0] & (1 << port)))
            {
                continue;
            }

            /* CRC check disable */
            /* This only needed when real HW SMB used */
            /* This code should be removed when solution for CRC cut on nics will be found */
            rc = cpssExMxPmPortCrcCheckEnableSet(dev, port, GT_FALSE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortCrcCheckEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
#endif

        /* Global : disable NA to CPU --> on auto learn       */
        /* Global : enable  NA to CPU --> on controlled learn */
        rc = cpssExMxPmBrgFdbNaToCpuSet(dev, (autoLearn == GT_TRUE) ? GT_FALSE : GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbNaToCpuSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Global : disable AA/TA to CPU --> on auto learn       */
        /* Global : enable  AA/TA to CPU --> on controlled learn */
        rc = cpssExMxPmBrgFdbAaAndTaToCpuSet(dev,(autoLearn == GT_TRUE) ? GT_FALSE : GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbAaAndTaToCpuSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Global : aging mode : triggered  --> on auto learn       --> keep backward compatible */
        /* Global : aging mode : auto       --> on controlled learn  */
        rc = cpssExMxPmBrgFdbTriggerModeSet(dev,(autoLearn == GT_TRUE) ?
                                            CPSS_EXMXPM_FDB_TRIGGER_TRIG_E :
                                            CPSS_EXMXPM_FDB_TRIGGER_AUTO_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbTriggerModeSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(autoLearn == GT_FALSE)
        {
            rc = cpssExMxPmBrgFdbActionModeSet(dev,CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbActionModeSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }


        /* Set IVL mode */
        rc = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, CPSS_IVL_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbMacVlanLookupModeSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Set device in FDB device table */
        devTable[dev/32] = ( 1 << (dev%32) );
        rc = cpssExMxPmBrgFdbDeviceTableSet(dev, devTable);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbDeviceTableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Unset FDB scanning Outlif mask */
        fdbScanOutlifMask.devNum        = 0x0;
        fdbScanOutlifMask.isMultiple    = 0x0;
        fdbScanOutlifMask.isTrunk       = 0x0;
        fdbScanOutlifMask.portTrunk     = 0x0;
        fdbScanOutlifMask.vidx          = 0x0;
        rc = cpssExMxPmBrgFdbActionActiveOutlifSet(dev,
                       &fdbScanOutlifMask, &fdbScanOutlifMask);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbActionActiveOutlifSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Initialization of Ingres Policer Metering and Counting Table */
        /* Policer changed in Puma2 - should be updated */
        {/*
            GT_U32                                      policerEntryIndex;
            CPSS_EXMXPM_POLICER_METERING_ENTRY_STC      initPolicerEntry;
            CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT     tbOutParam;
            osMemSet(&initPolicerEntry, 0, sizeof(CPSS_EXMXPM_POLICER_METERING_ENTRY_STC));
            osMemSet(&tbOutParam, 0, sizeof(CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT));
            for (policerEntryIndex = 0; policerEntryIndex < 256; policerEntryIndex++)
            {
                rc = cpssExMxPmPolicerMeteringEntrySet(dev,
                                                       policerEntryIndex,
                                                       &initPolicerEntry,
                                                       &tbOutParam);
            }*/
        }

        /* Disable Txq resource sharing */
        rc = cpssExMxPmPortTxSharingResourcesEnableSet(dev, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortTxSharingResourcesEnableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Set tail drop profile default values per device/profile/tc */
        for (profile = 0; profile < 8; profile++)
        {
            for (tc = 0; tc < CPSS_TC_RANGE_CNS; tc++)
            {
                tailDropProfile.dp0MaxBuffNum = 5;
                tailDropProfile.dp0MaxDescrNum = 3;
                tailDropProfile.dp1MaxBuffNum = 5;
                tailDropProfile.dp1MaxDescrNum = 3;
                tailDropProfile.dp2MaxBuffNum = 5;
                tailDropProfile.dp2MaxDescrNum = 3;
                tailDropProfile.dp0MaxMCBuffNum = 0x28;
                tailDropProfile.dp1MaxMCBuffNum = 0x28;
                tailDropProfile.dp2MaxMCBuffNum = 0x28;
                rc = cpssExMxPmPortTxTailDropProfileSet(dev, profile, tc,
                                                   &tailDropProfile);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortTxTailDropProfileSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        /* set default egress virtual to physical port mapping */
        for (port = 0; port < 64; port++)
        {
            for (tc = 0; tc < CPSS_TC_RANGE_CNS; tc++)
            {
                if (port < 26)
                {
                    rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, port ,tc);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                else if (port == 52)
                {
                    rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, 27 ,tc);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                else if (port == 56)
                {
                    rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, 26 ,tc);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                else if (port == 63)
                {
                    rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, 28 ,tc);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                else /* all other (unwanted) ports are set with non-existing port 30 */
                {
                    rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, 30 ,tc);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
        }

        /* set default ingress physical to virtual port mapping */
        for (port = 0; port <= 25; port++)
        {
            if( !(PRV_CPSS_PP_MAC(dev)->existingPorts.ports[0] & (1 << port)))
                continue;

            rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, port);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* set Service Port ingress physical to virtual port mapping */
        rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, 27, 52);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set LBP ingress physical to virtual port mapping */
        rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, 26, 56);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Set STP Table Access mode to be according STG index from VLAN table */
        rc = cpssExMxPmBrgStpTableAccessModeSet(dev,
                     CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_STG_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgStpTableAccessModeSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Set internal TCAM write format */
        rc = cpssExMxPmIntTcamWriteModeSet(dev,
                    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIntTcamWriteModeSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* HGL0/HGL1/HGL2/HGL3 TX Lanes Swap for IB cable & LED0/1 Control
        set to link Indication */
        rc = prvCpssExMxPmSerdesTxSwpSelAndLedCntrlSet(dev,boardRevId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssExMxPmSerdesTxSwpSelAndLedCntrlSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* make periodic flow control disable */
        rc = prvCpssExMxPmPeriodicFlowControlDisableSet(dev);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssExMxPmPeriodicFlowControlDisableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        #ifndef ASIC_SIMULATION
        rc = prvCpssExMxPmInitialPhyConfig(dev);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssExMxPmInitialPhyConfig", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        #endif
    }

    if(exMxPmAsVb && rc ==GT_OK)
    {
        rc = gtDb98Ex240Config_afterInitBoardConfig_inVb(boardRevId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("gtDb98Ex240Config_afterInitBoardConfig_inVb", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}



/*******************************************************************************
* getPpRegCfgList
*
* DESCRIPTION:
*       This function returns the list of registers to be configured to a given
*       device before and after the startInit operation. (Passed to
*       corePpHwStartInit() ).
*
* INPUTS:
*       boardRevId - Revision of this board
*       devNum     - device number
*
* OUTPUTS:
*       regCfgList      - A pointer to the register list.
*       regCfgListSize  - Number of elements in regListPtr.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpRegCfgList
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devNum,
    OUT CPSS_REG_VALUE_INFO_STC     **regCfgList,
    OUT GT_U32                      *regCfgListSize
)
{
    switch (PRV_CPSS_PP_MAC(devNum)->devType)
    {
    /* Puma */
    case (CPSS_98EX240_CNS):
    case (CPSS_98EX240_1_CNS):
    case (CPSS_98EX260_CNS):
    case (CPSS_98EX2210_CNS):
    case (CPSS_98EX8262_CNS):
    case (CPSS_98EX2110_CNS):
    case (CPSS_98MX2310_CNS) :
    case (CPSS_98EX8302_CNS):
    case (CPSS_98EX8305_CNS):
    case (CPSS_98EX8502_CNS):
    case (CPSS_98EX2106_CNS):
    case (CPSS_98EX2206_CNS):
    case (CPSS_98MX2306_CNS):
    case (CPSS_98EX8261_CNS):
    case (CPSS_98EX8301_CNS):
    case (CPSS_98EX8303_CNS):
    case (CPSS_98EX8306_CNS):
    case (CPSS_98EX8501_CNS):
    case (CPSS_98EX8307_CNS):
        break;
    default:
        return GT_NOT_APPLICABLE_DEVICE;
    }

    switch (boardRevId) {
    case 1:
    case 2:
        *regCfgList     = pumaEX240QdrRegValInfoList;
        *regCfgListSize = (sizeof(pumaEX240QdrRegValInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;
    case 3:
    case 4:
        *regCfgList     = pumaEX240RldRegValInfoList;
        *regCfgListSize = (sizeof(pumaEX240RldRegValInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;
    default:
        break;
    }

    return GT_OK;

}
/*******************************************************************************
* gtDb98Ex240BoardReg
*
* DESCRIPTION:
*       Registration function for the DB-98Ex240 board. -- EXMXPM 24G ports
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
GT_STATUS gtDb98Ex240BoardReg
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
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
    boardCfgFuncs->boardGetPpRegCfgList         = getPpRegCfgList;

    return GT_OK;
}

/*******************************************************************************
* prvCpssExMxPmInitialFaConfig
*
* DESCRIPTION:
*       Fabric Initial Configuration set.
*
* INPUTS:
*       devNum - device number.
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
GT_STATUS prvCpssExMxPmInitialFaConfig
(
    IN  GT_U8          devNum
)
{
    CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC  vidxConfig; /*vidx configuration struct*/
#if 0
    CPSS_PORTS_BMP_STC                         enableBmp;
#endif
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

    vidxConfig.fabricVIDX0 = 0x1;
    vidxConfig.fabricVIDX1 = 0x1;
    vidxConfig.fabricVidxBit0IsLbiBit0 = GT_FALSE;
    vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_FLOOD_ALL_E;
    vidxConfig.copyFabricVIDXToVIDX = GT_FALSE;
    vidxConfig.fabricVIDXAssignMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_ASSIGN_MODE_0_E;
    /* LBI and Fabric VIDX configuration */
    rc = cpssExMxPmFabricConfigVidxSet(devNum,&vidxConfig);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricConfigVidxSet", rc);
    if(rc != GT_OK)
        return rc;

    voqTrgDevsBmpArr[0] = 0xffffffff;
    voqTrgDevsBmpArr[1] = 0xffffffff;
    voqTrgDevsBmpArr[2] = 0xffffffff;
    voqTrgDevsBmpArr[3] = 0xffffffff;
    /* enable VOQ 0 devs */
    rc = cpssExMxPmFabricVoqDevTblSet(devNum, 0, voqTrgDevsBmpArr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqDevTblSet", rc);
    if(rc != GT_OK)
        return rc;

    /* enable VOQ 1 devs */
    rc = cpssExMxPmFabricVoqDevTblSet(devNum, 1, voqTrgDevsBmpArr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqDevTblSet", rc);
    if(rc != GT_OK)
        return rc;

    for (fport = 0 ; fport < CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS ; fport++)
    {
        /* enable uc fabric ports units 0,1 */
        rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 0, fport, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
        if(rc != GT_OK)
            return rc;
        rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 1, fport, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
        if(rc != GT_OK)
            return rc;
    }

    /* enable mc fabric ports units 0,1 */
    rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 0,
                                                  CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
    if(rc != GT_OK)
        return rc;

    /* enable mc fabric ports units 0,1 */
    rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 1,
                                                  CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqFportTxEnableSet", rc);
    if(rc != GT_OK)
        return rc;

    for (index = 0 ; index < CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS ; index++)
        fportsArr[index] = 0x1;


    for (trgDev = 0 ; trgDev < 64 ; trgDev++)
    {
        /* enable VOQ 0,1 UC table */
        rc = cpssExMxPmFabricVoqDevToFportMapSet(devNum, 0, trgDev, fportsArr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqDevToFportMapSet", rc);
        if(rc != GT_OK)
            return rc;
        rc = cpssExMxPmFabricVoqDevToFportMapSet(devNum, 1, trgDev, fportsArr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricVoqDevToFportMapSet", rc);
        if(rc != GT_OK)
            return rc;
    }

    xbarConfig.mode = CPSS_EXMXPM_FABRIC_XBAR_ENABLED_E;
    xbarConfig.xbarTcModeEnable = GT_FALSE;
    xbarConfig.voQUnit0ActiveHyperGlink = 0;
    xbarConfig.voQUnit1ActiveHyperGlink = 2;

    /*  Crossbar Global Control Address - enable XBAR, disable Traffic Classes
        XBAR mode and configure FPort 0  and 2 as active  for standby mode */
    rc = cpssExMxPmFabricXbarConfigSet(devNum, &xbarConfig);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarConfigSet", rc);
    if(rc != GT_OK)
        return rc;

    fportConfig.bcSrcDevFilterEnable            = GT_FALSE;
    fportConfig.ucSrcDevFilterEnable            = GT_FALSE;
    fportConfig.diagSrcDevFilterEnable          = GT_FALSE;
    fportConfig.diagCellSuperHighPriorityAssign = GT_TRUE;
    fportConfig.mcSrcDevFilterEnable            = GT_FALSE;
    fportConfig.portEnable                      = GT_TRUE;

    /* Gunit Gen config per fabric port */
    for(fport = 0; fport < 6; fport++)
    {
        rc = cpssExMxPmFabricXbarFportConfigSet(devNum, fport, &fportConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportConfigSet", rc);
        if(rc != GT_OK)
            return rc;
    }

    /* Configure the unicast table */
    for(fport = 4; fport < 6; fport++)
    {
        for (trgDev = 0 ; trgDev < 64 ; trgDev++)
        {
            for (lbh = 0; lbh < 4; lbh++)
            {
                rc =
                    cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(devNum,
                             fport, trgDev , lbh, GT_TRUE, fport);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportUnicastCellSwitchingSet", rc);
                if(rc != GT_OK)
                    return rc;
            }

        }
    }
    /* Configure the multicast table */
    rc = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(devNum,
                     CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E, 1, 0x10);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportMulticastCellSwitchingSet", rc);
    if(rc != GT_OK)
        return rc;

    rc = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(devNum,
                     CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E, 1, 0x20);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricXbarFportMulticastCellSwitchingSet", rc);
    if(rc != GT_OK)
        return rc;


#if 0

    enableBmp.ports[0] = 0xFFFFFFFF;
    enableBmp.ports[1] = 0xFFFFFFFF;
    /* HyperGLink Link Level Flow Control Enable */
    for(fport = 0; fport < 4; fport++)
    {
        rc = cpssExMxPmFabricHGLinkFlowControlEnableSet(devNum, fport, &enableBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmFabricHGLinkFlowControlEnableSet", rc);
        if(rc != GT_OK)
            return rc;
    }



#endif


   return GT_OK;

}



#ifndef ASIC_SIMULATION
/*******************************************************************************
* prvCpssExMxPmInitialPhyConfig
*
* DESCRIPTION:
*       Phy Initial Configuration set.
*
* INPUTS:
*       devNum - device number.
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
GT_STATUS prvCpssExMxPmInitialPhyConfig
(
    IN  GT_U8          devNum
)
{
    GT_U8 portNum;
    GT_STATUS rc;

    if( PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX2110_CNS ||
        PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX2210_CNS ||
        PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98MX2310_CNS ||
        PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX8262_CNS ||
        PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX8302_CNS ||
        PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX8305_CNS ||
        PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX8307_CNS ||
        PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98EX8502_CNS  )
    {
        GT_U32 regAddr;

        /* Enable autoNeg Port <-> PHY */

        regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->
                                LMS0.LMS0Group0.PHYAutoNegConfigReg0;
        rc = prvCpssHwPpWriteRegister(devNum,regAddr,0x40);
        if( rc != GT_OK )
        {
            return rc;
        }
        regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->
                                legacyUnits.LMS1.LMS1Group0.PHYAutoNegConfigReg2;
        rc = prvCpssHwPpWriteRegister(devNum,regAddr,0x40);
        if( rc != GT_OK )
        {
            return rc;
        }

        /* Service port 1112 initialization */

        rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, 27, 22, 2);
        if( rc != GT_OK )
            return rc;

        /* Disable Auto-Neg */
        rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, 27, 0, 0x140);
        if( rc != GT_OK )
            return rc;

        /* PHY soft reset */
        rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, 27, 0, 0x8140);
        if( rc != GT_OK )
            return rc;

        /* MDI reg. access (Return to page 0)*/
        rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, 27, 22, 0);
        if( rc != GT_OK )
            return rc;

        /* Select SERDES register Identity (move to page 1) */
        rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, 27, 22, 1);
        if( rc != GT_OK )
            return rc;

        /* MDI reg. access (Return to page 0)*/
        rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, 27, 22, 0);
        return rc;
    }

    /* Enable autoNeg Port <-> PHY */
    rc = cpssExMxPmPhyAutonegSmiEnableSet(devNum, 0, GT_TRUE);
    if( rc != GT_OK )
        return rc;

    rc = cpssExMxPmPhyAutonegSmiEnableSet(devNum, 12, GT_TRUE);
    if (rc != GT_OK)
        return rc;

    /* Set auto-media port configuration */

    /* Setting ports 12,13,22,23 to 1112 mode */

    for( portNum = 12; portNum < 24; portNum++ )
    {
        if ((portNum >13) && (portNum < 22))
            continue;
        rc =  cpssExMxPmPhySmiAutoMediaSelectSet(devNum,portNum,GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhySmiAutoMediaSelectSet", rc);
        if (rc != GT_OK)
            return rc;
    }

    for( portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++ )
    {
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum))
            continue;
        if( (portNum < 12) ||
            ((portNum > 13) && (portNum < 22) ) )
        {
            /* select page 2 */
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 22, 2);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
            /* disable SGMII auto neg*/
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 21, 0x0040);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
            /* AMP=0x6/AVDDT - Internal*/
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 26, 0x0006);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* return to page 2 */
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 22, 0);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* Apply PHY soft reset*/
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 0, 0x9140);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
        }
        else
        {
            /* select page 2 */
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 22, 2);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
            /* Disable Auto-Neg */
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 0, 0x140);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
            /* PHY soft reset */
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 0, 0x8140);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
            /* MDI reg. access (Return to page 0)*/
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 22, 0);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
            /* Select SERDES register Identity (move to page 1) */
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 22, 1);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;

            /* MDI reg. access (Return to page 0)*/
            rc = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, 22, 0);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortSmiRegisterWrite", rc);
            if( rc != GT_OK )
                return rc;
        }

        /* workaround for the 88E1240 SGMII lock-up problem */
        rc = cpssPhyErrataWaExecute(devNum, portNum,
                                    CPSS_PHY_ERRATA_WA_88E1240_SGMII_LOCKUP_E);
        if(rc != GT_NOT_APPLICABLE_DEVICE)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssPhyErrataWaExecute", rc);
        }
        if( rc != GT_OK && rc != GT_NOT_APPLICABLE_DEVICE )
            return rc;
    }

    return GT_OK;
}
#endif



/*******************************************************************************
* prvCpssExMxPmSerdesTxSwpSelAndLedCntrlSet
*
* DESCRIPTION:
*     This routine sets the connection between PCS Tx lanes and SERDES Tx lanes
*     and led control
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number.
*       boardRevId      - board revision id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_OUT_OF_RANGE          - on wrong lane value from pcsTxLaneArr
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS prvCpssExMxPmSerdesTxSwpSelAndLedCntrlSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      boardRevId
)
{
    GT_U32      regAddr;        /* Register address */
    GT_FPORT    fport;          /* loop iterator    */
    GT_U32      i;              /* loop iterator    */
    GT_STATUS   rc;             /* return status    */
    GT_BOOL     oldNotInReset;  /* The previous PCS state (Reset or normal) */
    GT_BOOL     rldBoard = GT_FALSE;

    /* validity checks */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(devNum);

    switch (boardRevId) {
    case 1:
    case 2:
        rldBoard = GT_FALSE;
        break;
    case 3:
    case 4:
        rldBoard = GT_TRUE;
        break;
    default:
        return GT_BAD_PARAM;
    }

    for (fport = 0; fport < 4; fport++)
    {
        /* set the connection between PCS Tx lanes and SERDES Tx lanes
           and  led control */

        /* Set RESET state */
        rc = prvCpssExMxPmFabricPcsResetStateSet(devNum, fport, GT_FALSE, &oldNotInReset);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssExMxPmFabricPcsResetStateSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (i = 0; i < CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS; i++)
        {
            regAddr = PRV_CPSS_EXMXPM_PP_MAC(devNum)->regsAddr.
                legacyUnits.HG_XPCS.laneRegs.laneCfg.laneCfg1[fport][i];
            if(rldBoard == GT_TRUE)
            {
                rc = prvCpssHwPpWriteRegister(devNum, regAddr, rldPcsTxLaneArr[i]);
            }
            else
            {
                rc = prvCpssHwPpWriteRegister(devNum, regAddr, qdrPcsTxLaneArr[i]);
            }
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssHwPpWriteRegister", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* restore PCS state after update bit */
        if (oldNotInReset == GT_TRUE)
        {
            /* Set Normal state */
            rc = prvCpssExMxPmFabricPcsResetStateSet(devNum, fport, GT_TRUE, &oldNotInReset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssExMxPmFabricPcsResetStateSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssExMxPmPeriodicFlowControlDisableSet
*
* DESCRIPTION:
*     This routine sets priodic flow control disable for all ports in device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_OUT_OF_RANGE          - on wrong lane value from pcsTxLaneArr
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/

GT_STATUS prvCpssExMxPmPeriodicFlowControlDisableSet
(
    IN  GT_U8     devNum
)

{
    GT_U8     portNum;           /* loop iterator */

    GT_STATUS  rc = GT_OK;               /* return code */

    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum))
            continue;
        rc = cpssExMxPmPortFlowControlXonPeriodicEnableSet(devNum,portNum,GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortFlowControlXonPeriodicEnableSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return rc;
}



GT_STATUS   appDemoPmFaVoqCntrsDump
(
    IN  GT_U8 devNum
)
{
    GT_U32                                              i,m;
    GT_FPORT                                            fport;
    CPSS_EXMXPM_FABRIC_VOQ_GLOBAL_CNTRS_STC             globalCntrs;
    CPSS_EXMXPM_FABRIC_VOQ_GLOBAL_RESOURCES_CNTRS_STC   rsrcCntrs;
    CPSS_EXMXPM_FABRIC_VOQ_FPORT_RESOURCES_CNTRS_STC    fportRsrcCntrs;
    CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_STC               setCntrs;
    GT_STATUS                                           rc;

    cpssOsPrintf(" VOQ Statistics\n");
    cpssOsPrintf("---------------\n");

    for (i = 0 ; i <= 1 ; i++)
    {
        cpssOsPrintf("\nVOQ unit %d :\n\n", i);
        cpssOsPrintf("Global Counters: \n");

        rc = cpssExMxPmFabricVoqGetGlobalCountersGet(devNum,i,&globalCntrs);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet(devNum,i,&rsrcCntrs);
        if (rc != GT_OK)
        {
            return rc;
        }


        cpssOsPrintf("Number of good rx descriptors per PP TC:\n");

        cpssOsPrintf ("tc0:%8d tc1:%8d tc2:%8d tc3:%8d tc4:%8d tc5:%8d tc6:%8d tc7:%8d\n",
                      globalCntrs.goodRxTcCount[0],globalCntrs.goodRxTcCount[1],
                      globalCntrs.goodRxTcCount[2],globalCntrs.goodRxTcCount[3],
                      globalCntrs.goodRxTcCount[4],globalCntrs.goodRxTcCount[5],
                      globalCntrs.goodRxTcCount[6],globalCntrs.goodRxTcCount[7]);


        cpssOsPrintf("Number of buffers dropped due to tail drop:%d \n",
                                                 globalCntrs.dropOnTailCount);

        cpssOsPrintf("Number of buffers dropped due to device disabled:%d \n",
                                                 globalCntrs.dropOnDevDisCount);

        cpssOsPrintf("Number of buffers dropped due to an ECC error:%d \n",
                                                 globalCntrs.dropOnEccCount);

        cpssOsPrintf("Number of descriptors currently allocated:%d \n",
                                                 rsrcCntrs.globalDescCnt);

        cpssOsPrintf("Number of buffers currently allocated:%d \n",
                                                 rsrcCntrs.globalBufCnt);

        /* puma 1 - only 1 uc fport.. */
        for (fport = 0; fport < 1; fport++)
        {
            rc = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(
                                            devNum,i,fport,&fportRsrcCntrs);
            if (rc != GT_OK)
            {
                return rc;
            }

            cpssOsPrintf("fport %d:\n",fport);

            cpssOsPrintf("  Number of buffers currently allocated to fabric port VOQs: %d \n",
                                                 fportRsrcCntrs.buffCount);

            cpssOsPrintf("  Number of descriptors currently allocated to fabric port %d per tc:\n", fport);
            cpssOsPrintf ("tc0:%8d tc1:%8d tc2:%8d tc3:%8d\n", fportRsrcCntrs.tcDescCount[0],fportRsrcCntrs.tcDescCount[1],fportRsrcCntrs.tcDescCount[2],fportRsrcCntrs.tcDescCount[3]);
            cpssOsPrintf("  Number of buffers currently allocated to fabric port %d per tc:\n", fport);
            cpssOsPrintf ("tc0:%8d tc1:%8d tc2:%8d tc3:%8d\n", fportRsrcCntrs.tcBuffCount[0],fportRsrcCntrs.tcBuffCount[1],fportRsrcCntrs.tcBuffCount[2],fportRsrcCntrs.tcBuffCount[3]);

        }

        rc = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(
              devNum,i,CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS,&fportRsrcCntrs);
        if (rc != GT_OK)
        {
            return rc;
        }

        cpssOsPrintf("multicast fport: ");

        cpssOsPrintf("  Number of buffers currently allocated to mc fabric port VOQs: %08d \n",
                                             fportRsrcCntrs.buffCount);
        cpssOsPrintf("  Number of descriptors currently allocated to mc fabric port per tc:\n");
        cpssOsPrintf ("tc0:%8d tc1:%8d tc2:%8d tc3:%d\n", fportRsrcCntrs.tcDescCount[0],fportRsrcCntrs.tcDescCount[1],fportRsrcCntrs.tcDescCount[2],fportRsrcCntrs.tcDescCount[3]);
        cpssOsPrintf("  Number of buffers currently allocated to mc fabric port per tc:\n", fport);
        cpssOsPrintf ("tc0:%8d tc1:%8d tc2:%8d tc3:%8d\n", fportRsrcCntrs.tcBuffCount[0],fportRsrcCntrs.tcBuffCount[1],fportRsrcCntrs.tcBuffCount[2],fportRsrcCntrs.tcBuffCount[3]);

        cpssOsPrintf("Per Set Counters: \n");
        for (m = 0; m <= 3; m++)
        {
            cpssOsPrintf(" Set %d :\n", m);

            rc = cpssExMxPmFabricVoqStatCntrsGet(devNum,i,m,&setCntrs);
            if (rc != GT_OK)
            {
                return rc;
            }

            cpssOsPrintf("Number of valid descriptors received: %d \n",
                                                    setCntrs.goodRxCount);
            cpssOsPrintf("Number of descriptors tail dropped: %d \n",
                                                    setCntrs.eqDropCount);
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtDb98Ex240Config_afterInitBoardConfig_inVb
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
static GT_STATUS gtDb98Ex240Config_afterInitBoardConfig_inVb
(
    IN  GT_U8                       boardRevId
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U8                       ii;
    GT_U8                       dev;
    GT_U8                       port;
    GT_U32                      tc;
#ifdef CHX_FAMILY
    GT_U8                       jj;
    GT_U32                      networkPortsNum=NUM_DX_NET_PORTS_CNS;
    GT_U8                       networkPortsArr[NUM_DX_NET_PORTS_CNS];
    GT_U8                       networkPortCscdArr[NUM_DX_NET_PORTS_CNS];
    GT_U32                      cascadePortsNum=NUM_DX_CSCD_PORTS_CNS;
    GT_U8                       cascadePortsArr[NUM_DX_CSCD_PORTS_CNS];
#endif /*CHX_FAMILY*/
    CPSS_EXMXPM_VB_CSCD_PORT_INFO_STC    cscdPortsArray[24]; /* 24 cascade ports */
    CPSS_EXMXPM_VB_VIRT_PORT_INFO_STC    virtPortsInfo[24][2];

    /* go over all devices and virtual ports per device */
    for (ii = 0; (ii < PRV_CPSS_MAX_PP_DEVICES_CNS) && (GT_OK == rc); ii++)
    {
        if (GT_FALSE == appDemoPpConfigList[ii].valid)
        {
            continue;
        }
        /* get device id and number of virtual ports per device */
        dev = appDemoPpConfigList[ii].devNum;

        if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[ii].devFamily) == 0)
        {
            /*******************************************************************************/
            /************************ Value Blade Configuration ****************************/
            /*******************************************************************************/
            /* Set the access mode of the "DSA Virtual Port-ID assignment" table, *
             * In Puma2-24G it must be DSA                                        *
             * Must be called before cpssExMxPmVbConfigSet or                     *
             * cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet                  */
            rc = cpssExMxPmVbVirtualPortsMapModeSet(dev, CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_DSA_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbVirtualPortsMapModeSet", rc);
            if(rc != GT_OK)
                return rc;

            for (port = 0 ; port < 24 ; port++)
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

            for (port = 0 ; port < 24 ; port++)
            {
                cscdPortsArray[port].cscdIngressPhysicalPortNum = port;
                cscdPortsArray[port].cscdIngressPortDsaType     = CPSS_EXMXPM_NET_DSA_TYPE_REGULAR_E;
                cscdPortsArray[port].virtPortsInfoSize          = 2;
                cscdPortsArray[port].virtPortsInfoPtr           = virtPortsInfo[port];

                virtPortsInfo[port][0].portNum        = (GT_U8)(port*2);
                /* set device numbers that come after the puma device number */
                virtPortsInfo[port][0].dsaTagSrcDev   = SYSTEM_DEV_NUM_MAC((port / 8) + 1); /* 8 cascade ports per Target device */
                virtPortsInfo[port][0].dsaTagSrcPort  = (GT_U8)((port%8)*2); /* 16 Network ports on each Target device */

                virtPortsInfo[port][1].portNum        = (GT_U8)(virtPortsInfo[port][0].portNum + 1);
                virtPortsInfo[port][1].dsaTagSrcDev   = virtPortsInfo[port][0].dsaTagSrcDev; /* 8 cascade ports per Target device */
                virtPortsInfo[port][1].dsaTagSrcPort  = (GT_U8)(virtPortsInfo[port][0].dsaTagSrcPort + 1); /* 16 Network ports on each Target device */

                for (tc=0; tc < CPSS_TC_RANGE_CNS; tc++)
                {
                    virtPortsInfo[port][0].egressPhysicalPortsNum[tc] = port;
                    virtPortsInfo[port][0].egressPhysicalTcMap[tc] = tc;

                    virtPortsInfo[port][1].egressPhysicalPortsNum[tc] = port;
                    virtPortsInfo[port][1].egressPhysicalTcMap[tc] = tc;
                }
            }

            /*  Set the VB configuration for all virtual ports to the network ports in *
             *  the mux devices.                                                       *
             *  All physical ExMxPm ports that are not included in the INPUT info      *
             *  (in cscdPortsArray) will be set to HW as non cascade ports.            */
            rc = cpssExMxPmVbConfigSet(dev, 24, cscdPortsArray);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbConfigSet", rc);
            if(rc != GT_OK)
                return rc;

            /* Recommended Configuration:
            * Speed: 1 Gbps                    *
            * Duplex: Full-duplex              *
            * Flow Control: Disabled           *
            * Pause: Do not send Pause frames  */
            if(exMxPmBoardVbDevicesExists)
            {
                /* change the inter port/phy config ONLY on board that
                   actually have VB devices ---
                   because this function allow to set a Puma device on board
                   that is not real connected to Dx mux devices */

                for (port = 0 ; port < 24; port++)
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
          /********************* End of Value Blade Configuration ************************/
          /*******************************************************************************/
        }
#ifdef CHX_FAMILY
        else /* appDemoPpConfigList[ii].devFamily != CPSS_PP_FAMILY_PUMA_E ==>
                Cheetah devices in VB configuration */
        {
            /*******************************************************************************/
            /********************* Value Blade Configuration ************************/
            /*******************************************************************************/
            for(jj = 0 ; jj < NUM_DX_CSCD_PORTS_CNS ; jj++)
            {
                /* cascade ports start after the network ports */
                cascadePortsArr[jj] = (GT_U8)(NUM_DX_NET_PORTS_CNS + jj);
            }

            for(jj = 0 ; jj < NUM_DX_NET_PORTS_CNS ; jj++)
            {
                networkPortsArr[jj] = jj;
                /* every 2 network ports mapped to the same cascade port (PVE) */
                networkPortCscdArr[jj] = cascadePortsArr[jj/2];
            }

            rc = appDemoDxChVbMuxInit(ii,
                                networkPortsNum,networkPortsArr,networkPortCscdArr,
                                cascadePortsNum,cascadePortsArr);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChVbMuxInit", rc);
            if(rc != GT_OK)
                return rc;

        }
#endif/*CHX_FAMILY*/
    }

    return rc;
}

/* set is it VB on not .
  called from terminal (before cpssInitSystem)or from the VB board config
  (internally in code) */
void exMxPmAsVbSet(GT_U32  newVal)
{
    exMxPmAsVb = newVal;
}

