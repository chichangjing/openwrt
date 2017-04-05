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
* gtDbFx950_EX240_Config.c
*
* DESCRIPTION:
*       Initialization functions for the FX950 and Ex240 boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sandConfig/appDemoFap20mConfig.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>


#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricE2e.h>
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/private/prvCpssExMxPmHwInit.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
#include <cpss/exMxPm/exMxPmGen/intTcam/cpssExMxPmIntTcam.h>
#include <cpss/exMxPm/exMxPmGen/vb/cpssExMxPmVb.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricPktReasm.h>


/* Fx950 includes */
#include <FX950/include/fx950_general.h>

#include <FX950/include/fx950_chip_defines.h>
#include <FX950/include/fx950_api_chopping.h>
#include <FX950/include/fx950_api_chunk_context_manager.h>
#include <FX950/include/fx950_api_hgl_cell_reassembly.h>
#include <FX950/include/fx950_api_hgl_interface.h>
#include <FX950/include/fx950_api_hgs_interface.h>
#include <FX950/include/fx950_api_labeling.h>
#include <FX950/include/fx950_api_pcs.h>
#include <FX950/include/fx950_api_queue_selection.h>
#include <FX950/include/fx950_api_scheduler.h>
#include <FX950/include/fx950_api_serdes.h>
#include <FX950/include/fx950_api_statistics_interface.h>
#include <FX950/include/fx950_api_tail_drop.h>
#include <FX950/include/fx950_api_mh_interoperability.h>
#include <FX950/include/fx950_api_mh_marking.h>
#include <FX950/include/fx950_api_flow_control.h>

#include <FX950/include/fap20m_auto_mngr.h>

/* SAND includes */
#include <SAND/Management/include/sand_low_level.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/
extern APP_DEMO_PP_CONFIG    appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* memory allocation */
extern void *malloc(size_t);


#define APPDEMO_FX950_NEGEV_NOF_FAPS       2

/* User configured bitmap for SERDES power down */
/* GT_U32 appDemoExMxPmPowerDownBmp = 0; */

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

static unsigned long pexBaseAddress;

static unsigned long baseAddress;



/*******************************************************************************
 * External variables
 ******************************************************************************/
/* pumaBoardConfigFun will hold the puma board configuration */
/* we will use those pointers as "set default" values that we can override */
static GT_BOARD_CONFIG_FUNCS pumaBoardConfigFun;


/* private Fabric Initial Configuration function declaration */
GT_STATUS prvCpssExMxPmFx950InitialFaConfig
(
    IN  GT_U8          devNum
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
    GT_STATUS   retVal;                 /* Function return value.       */
    GT_U8       ppCounter;              /* Number of Packet processors. */
    GT_U8       i;                      /* Loop index.                  */


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
            if (appDemoPpConfigList[i].devFamily == APPDEMO_FX950_FAMILY_TYPE)
            {
                appDemoPpConfigList[i].valid = GT_FALSE;

                pexBaseAddress = appDemoPpConfigList[i].pciInfo.internalPciBase;

                baseAddress = appDemoPpConfigList[i].pciInfo.pciBaseAddr;
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
    IN  GT_U8                           boardRevId,
    IN  GT_U8                           devIdx,
    OUT CPSS_PP_PHASE1_INIT_PARAMS      *phase1Params
)
{
    return pumaBoardConfigFun.boardGetPpPh1Params(boardRevId,devIdx,phase1Params);
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
    return pumaBoardConfigFun.boardGetPpPh2Params(boardRevId,oldDevNum,phase2Params);
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
    return pumaBoardConfigFun.boardAfterPhase2Config(boardRevId);
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
    return pumaBoardConfigFun.boardGetPpLogInitParams(boardRevId,devNum,ppLogInitParams);
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
    return pumaBoardConfigFun.boardGetLibInitParams(boardRevId,devNum,libInitParams);
}


/*******************************************************************************
* afterInitBoardConfigPp
*
* DESCRIPTION:
*       Board specific configurations that should be done after board
*       initialization.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       ppTypePtr       - pointer to PP type: 24G ports or 2XG ports.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS afterInitBoardConfigPp
(
    IN  GT_U8                       boardRevId,
    OUT APPDEMO_FAP20M_PP_TYPE_ENT  *ppTypePtr
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



    /* go over all devices and virtual ports per device */
    for (i = 0; (i < PRV_CPSS_MAX_PP_DEVICES_CNS) && (GT_OK == rc); i++)
    {
        if (GT_TRUE == appDemoPpConfigList[i].valid)
        {
            /* get device id and number of virtual ports per device */
            dev = appDemoPpConfigList[i].devNum;


            /* FA HW regs configuration */
            rc = prvCpssExMxPmFx950InitialFaConfig(dev);
            if (rc != GT_OK)
            {
                return rc;
            }


            /* create vlan 1 */
            numOfPorts = PRV_CPSS_PP_MAC(dev)->numOfVirtPorts;

            switch (PRV_CPSS_PP_MAC(dev)->devType)
            {
                /* Puma2 XG */
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
                    *ppTypePtr = APPDEMO_FAP20M_PP_EXMXPM_2XG_E;
                    break;
                default:
                    /* Puma2 24G */
                    portsMembers.ports[0] = 0xFFFFFF;
                    *ppTypePtr = APPDEMO_FAP20M_PP_EXMXPM_24G_E;
                    break;
            }

            portsMembers.ports[1] = 0;
            portsTagging.ports[0] = 0;
            portsTagging.ports[1] = 0;
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
                if ((rc = cpssExMxPmBrgStpEntryWrite(dev, 0, stgEntry)) != GT_OK)
                {
                    return rc;
                }
            }
            if ((rc = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, 0, 0x5f2)) != GT_OK)
                return rc;

            if ((rc = cpssExMxPmBrgVlanEntryWrite(dev, 1, &portsMembers, &portsTagging, &vlanInfo)) != GT_OK)
                return rc;

            /* VLAN Inlif configuration for VLANs
               This settings may be in use only when Port is configured in VLAN InLif mode.
               By default InLif mode is per Port. */
            for (inlifIndex = 0 ; inlifIndex < PRV_CPSS_MAX_NUM_VLANS_CNS; inlifIndex++)
            {
                /* Enable learning */
                if ((rc = cpssExMxPmInlifAutoLearnEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, inlifIndex, GT_TRUE)) != GT_OK)
                    return rc;

                /* set New Address is NOT Security Breach */
                if ((rc = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, inlifIndex, GT_TRUE)) != GT_OK)
                    return rc;

                /* Enable Bridging */
                if ((rc = cpssExMxPmInlifBridgeEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, inlifIndex, GT_TRUE)) != GT_OK)
                    return rc;
            }

            /* Enable NA send to CPU (only for vlan 1) */
            if ((rc = cpssExMxPmInlifNaToCpuEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, 1, GT_TRUE)) != GT_OK)
                return rc;

            /* Configuration of flooding regardless of VIDX entry  */
            vidxPortsMembers.ports[0] = 0xFFFFFFFF;
            vidxPortsMembers.ports[1] = 0x3FFFFF;
            if ((rc = cpssExMxPmBrgMcEntryWrite(dev, 0xFFF, GT_TRUE, &vidxPortsMembers)) != GT_OK)
                return rc;

            /* Port InLif configuration for Auto Learning */
            for (port = 0 ; port < numOfPorts; port++)
            {
                /* set PVID = 1*/
                if ((rc = cpssExMxPmBrgVlanPortPvidSet(dev, port, 1)) != GT_OK)
                    return rc;

                inlifIndex = port;

                /* set New Address is NOT Security Breach */
                if ((rc = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_PORT_E, inlifIndex, GT_TRUE)) != GT_OK)
                    return rc;

                /* Enable NA to CPU */
                if ((rc = cpssExMxPmInlifNaToCpuEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_PORT_E, inlifIndex, GT_TRUE)) != GT_OK)
                    return rc;

                /* Enable Auto Learning */
                if ((rc = cpssExMxPmInlifAutoLearnEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_PORT_E, inlifIndex, GT_TRUE)) != GT_OK)
                    return rc;

                /* Enable Bridging */
                if ((rc = cpssExMxPmInlifBridgeEnableSet(dev, CPSS_EXMXPM_INLIF_TYPE_PORT_E, inlifIndex, GT_TRUE)) != GT_OK)
                    return rc;

                /* set port's default inLif ID */
                if ((rc = cpssExMxPmInlifPortDefaultInlifIdSet(dev,port,inlifIndex)) != GT_OK)
                    return rc;

                /* Enable Auto Learning in Ingress Port Bridge Entry */
                if ((rc = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, GT_TRUE)) != GT_OK)
                    return rc;

                /* Enable NA AA To CPU message in Ingress Port Bridge Entry */
                if ((rc = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, GT_TRUE)) != GT_OK)
                    return rc;

            }


#ifdef ASIC_SIMULATION
            /* Port MAC configuration */
            for (port = 0 ; port < 27; port++)
            {

                if( !(portsMembers.ports[0] & (1 << port)))
                {
                    continue;
                }

                /* CRC check disable */
                /* This only needed when real HW SMB used */
                /* This code should be removed when solution for CRC cut on nics will be found */
                if ((rc = cpssExMxPmPortCrcCheckEnableSet(dev, port, GT_FALSE)) != GT_OK)
                    return rc;

            }
#endif

            /* Global disable NA to CPU */
            if((rc = cpssExMxPmBrgFdbNaToCpuSet(dev, GT_FALSE)) != GT_OK)
                return rc;

            /* Set IVL mode */
            if ((rc = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, CPSS_IVL_E)) != GT_OK)
                return rc;

            /* Set device in FDB device table */
            devTable[dev/32] = ( 1 << (dev%32) );
            if ((rc = cpssExMxPmBrgFdbDeviceTableSet(dev, devTable)) != GT_OK)
                return rc;

            /* Unset FDB scanning Outlif mask */
            fdbScanOutlifMask.devNum        = 0x0;
            fdbScanOutlifMask.isMultiple    = 0x0;
            fdbScanOutlifMask.isTrunk       = 0x0;
            fdbScanOutlifMask.portTrunk     = 0x0;
            fdbScanOutlifMask.vidx          = 0x0;
            if ((rc = cpssExMxPmBrgFdbActionActiveOutlifSet(dev,
                           &fdbScanOutlifMask, &fdbScanOutlifMask)) != GT_OK)
                return rc;

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
            if ((rc = cpssExMxPmPortTxSharingResourcesEnableSet(dev, GT_FALSE)) != GT_OK)
                return rc;

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
                    if ((rc = cpssExMxPmPortTxTailDropProfileSet(dev, profile, tc,
                                                       &tailDropProfile))!= GT_OK)
                        return rc;
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
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    else if (port == 52)
                    {
                        rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, 27 ,tc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    else if (port == 56)
                    {
                        rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, 26 ,tc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    else if (port == 63)
                    {
                        rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, 28 ,tc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    else /* all other (unwanted) ports are set with non-existing port 30 */
                    {
                        rc = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, 30 ,tc);
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
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            /* set Service Port ingress physical to virtual port mapping */
            rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, 27, 52);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* set LBP ingress physical to virtual port mapping */
            rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, 26, 56);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Set STP Table Access mode to be according STG index from VLAN table */
            if ((rc = cpssExMxPmBrgStpTableAccessModeSet(
                         dev,
                         CPSS_EXMXPM_BRG_STP_TABLE_ACCESS_MODE_STG_E)) != GT_OK)
                return rc;

            /* Set internal TCAM write format */
            if ((rc = cpssExMxPmIntTcamWriteModeSet(
                        dev,
                        CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E)) !=GT_OK)
                return rc;

        }
    }

    return rc;
}

/*******************************************************************************
* afterInitBoardConfigFx950
*
* DESCRIPTION:
*       FX950 specific configuration
*
* INPUTS:
*       deviceId                - device Id.
*       fapId                   - fap Id
*       maxNumOfFapsInSystem    - maximum number of FAPs in system
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
static GT_STATUS afterInitBoardConfigFx950
(
    IN  unsigned int        deviceId,
    IN  unsigned int        fapId,
    IN  unsigned int        maxNumOfFapsInSystem
)
{
    unsigned int        port;                   /* loop iterator */
    unsigned int        i;                      /* loop iterator */
    FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT  port_mac_params; /* MAC parameters */
    FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT ingr_stat_cfg;
                                            /* statistics ingress configuration */
    FX950_BMP_128       chop_en_dev_bmp;        /* bitmap of chopping device
                                                   enablers */
    FX950_CHOPPING_ENABLE_PARAMS_STRUCT     chop_en_params; /* Chopping enable
                                                               parameters */
    unsigned long       priority_new[8];        /* array of priorities */
    FX950_LABELING_INGRESS_PARAMS_STRUCT    label_params;  /* Labeling parameters */

#ifndef ASIC_SIMULATION
    FX950_LABELING_UC_LABEL_PARAM_ENTRY     uc_label_params; /* UC Label entry parameters */
    FX950_BMP_256       cpu_code_to_link_bmp;   /* bitmap of CPU code to link mapping */
    FX950_BMP_64        port_bmp;               /* bitmap of Port to link mapping */
    unsigned int        baseQueueId;            /* base queue id for specific target fap id */
    unsigned int        trg_pp_dev_num;         /* Target PP device number */
#endif
    FX950_BMP_256       sched_default_state_bmp; /* bitmap of scheduler default states */
    FX950_BMP_256       sched_mode_bmp;          /* bitmap of scheduler mode */
    unsigned long       dp_new[FX950_MH_MARKING_NOF_DP_LEVELS]; /* Dp 2 DP mapping array */
    unsigned long       tc_new[FX950_MH_MARKING_NOF_TC_LEVELS]; /* TC 2 TC mapping array */
    FX950_MH_MARKING_CFG_STRUCT     mh_marking_cfg;  /* egress qos configuration */
    unsigned long       ret;        /* FAP20M return code */
    SAND_RET            sand_ret;   /* SAND return code */


    /* MAC parameters */

    /* ECC type as in Puma2 CPSS_EXMXPM_FABRIC_ECC_ECC7_E */
    port_mac_params.ecc_checker_type = FX950_HGL_INTERFACE_ECC_CHECKER_SECDED;
    /* 6 lanes */
    port_mac_params.serdes_lanes_mode = FX950_HGL_INTERFACE_SERDES_LANES_6;

    for (port = 0; port < 2; port++)
    {
        ret = fx950_api_hgl_interface_set_port_mac_params(deviceId, port, &port_mac_params);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }


    /* Set default value of the scheduler to XOFF */
    for (i = 0; i < (sizeof(sched_default_state_bmp) / sizeof(sched_default_state_bmp.bitmap[0])); i++)
    {
        sched_default_state_bmp.bitmap[i] = 0xFFFFFFFF;
    }

    ret = fx950_api_flow_control_set_scheduler_default_state_bmp(deviceId, &sched_default_state_bmp);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Set scheduler to be dynamically updated according to the Flow Control messages */
    for (i = 0; i < 8; i++)
    {
        sched_mode_bmp.bitmap[i] = 0xFFFFFFFF;
    }

    ret = fx950_api_flow_control_set_scheduler_mode_bmp(deviceId, &sched_mode_bmp);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }


    /* configure Link0DevNum and Link1DevNum according to Ocelot device id */
    ingr_stat_cfg.link_dev_num = appDemoPpConfigList[0].devNum;
    ingr_stat_cfg.statistics_enable = GT_TRUE;
    ingr_stat_cfg.message_include_vid = GT_FALSE;
    ingr_stat_cfg.dq_const_operation = GT_TRUE;
    ingr_stat_cfg.eq_const_operation = GT_TRUE;
    ingr_stat_cfg.dq_const_bc = 0;
    ingr_stat_cfg.eq_const_bc = 0;

    ret = fx950_api_statistics_interface_set_ingress_cfg(deviceId,
                                                         &ingr_stat_cfg);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Choping Enablers */
    /* Chopping */

    /* configure chunk max size and ingress chunk max size to 0xc (128*0xc bytes),
       configure last chunk max size to 0x20 (64*0x20 bytes) */
    ret = fx950_api_chopping_set_max_chunk_sizes(deviceId, 0xc, 0x20);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Enable packet chopping for all targets */
    chop_en_dev_bmp.bitmap[0] = 0xffffffff;
    chop_en_dev_bmp.bitmap[1] = 0xffffffff;
    chop_en_dev_bmp.bitmap[2] = 0xffffffff;
    chop_en_dev_bmp.bitmap[3] = 0xffffffff;

    ret = fx950_api_chopping_set_enable_target_dev_bmp(deviceId,
                                                       &chop_en_dev_bmp);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    chop_en_params.mcChopEn = GT_FALSE;
    chop_en_params.ucChopEn = GT_FALSE;
    /* should be Less then the first CPU VOQ (2000 -> 2255) */
    chop_en_params.maxQueueNum = 0x7CF; /* 1999 */
    ret = fx950_api_chopping_set_enable_params(deviceId, &chop_en_params);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Enable Chopping per TC for both MC and UC packets */
    ret = fx950_api_chopping_set_enable_tc(deviceId,FX950_PACKETS_UNICAST, 0xf);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_chopping_set_enable_tc(deviceId,FX950_PACKETS_MULTICAST, 0xf);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Labeling */

    /* MC Priority mapping */
    priority_new[0] = 0;
    priority_new[1] = 0;
    priority_new[2] = 1;
    priority_new[3] = 1;
    priority_new[4] = 2;
    priority_new[5] = 2;
    priority_new[6] = 3;
    priority_new[7] = 3;

    ret = fx950_api_labeling_set_priority_map(deviceId, FX950_PACKETS_MULTICAST,
                                              priority_new);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Labeling Global Configuration */
    label_params.uc_label_mode = FX950_LABELING_UC_LABEL_BY_PORT;
    label_params.uc_flow_id_num_bit = 16;
    label_params.uc_priority_num_bit = 3;
    label_params.uc_label_tbl_access_mode = FX950_LABELING_UC_LABEL_TBL_ACCESS_BY_TRG_DEVICE;

    label_params.mc_label_mode = FX950_LABELING_MC_LABEL_BY_PRIORITY;
    label_params.mc_flow_vid_vidx_width = 16;
    label_params.mc_prio_width = 3;
    label_params.port_add_enable = TRUE;
    label_params.flow_add_enable = TRUE;
    label_params.mailbox_device = 0;

    ret = fx950_api_labeling_set_ingress_labeling_parameters(deviceId,
                                                             &label_params);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

#ifndef ASIC_SIMULATION
    /* TC 2 XGL Cell TC mapping, should be configured as in Pum2 */
    /* UC Table, if the access mode is according target device, then only 64
       entries are used. */

    uc_label_params.use_flow_mode = FX950_LABELING_UC_LABEL_BY_FLOW;
    uc_label_params.valid = 1;

    for (trg_pp_dev_num = 0; trg_pp_dev_num < maxNumOfFapsInSystem; trg_pp_dev_num++)
    {
        /* The base VOQ is calculated according to fap id,
           calculate the base queue id for this target fapId */
        baseQueueId = fap20m_auto_get_queue_id(trg_pp_dev_num, 0, 0);

        uc_label_params.flow_base_queue_number = baseQueueId;
        uc_label_params.port_base_queue_number = baseQueueId;

        ret = fx950_api_labeling_set_uc_label_tbl_entry(deviceId,
                                                        trg_pp_dev_num, /* targed device id */
                                                        &uc_label_params);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }

    /* The base VOQ is calculated according to fap id,
       calculate the base queue id for this target fapId */
    baseQueueId = fap20m_auto_get_queue_id(fapId, 0, 0);

    /* CPU Configuration Table ,the table is configured to send TO_CPU packets
       to local CPU */
    for (i = 0; i < 256; i++)
    {
        ret = fx950_api_labeling_set_cpu_code_label_map(deviceId,
                                                        i,
                                                        (baseQueueId + (i % 8)));
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }
#endif


    /* Egress */

    /* HGL CRC ECCSelect, same as in Puma2 and  ecc_checker_type in MAC
       parameters: FX950_HGL_INTERFACE_ECC_CHECKER_SECDED */
    /* ECC is the default configuration */


    /* Egress QoS:
       DPSource = 0x1, OverrideMC_TC_DPEn = 0x1, OverrideUC_TC_DPEn = 0x1,
       OverrideHglTcEn = 0x1 */
    mh_marking_cfg.dp_source_mode = FX950_MH_MARKING_DP_SOURCE_DESCR;
    mh_marking_cfg.override_mc_tc_dp_en = TRUE;
    mh_marking_cfg.override_uc_tc_dp_en = TRUE;
    mh_marking_cfg.override_hgl_tc_en = TRUE;
    mh_marking_cfg.mc_dp = 0;
    mh_marking_cfg.uc_dp = 0;
    mh_marking_cfg.override_trap_dp_en = FALSE;
    mh_marking_cfg.override_trap_tc_en = FALSE;

    ret = fx950_api_mh_marking_set_cfg(deviceId, &mh_marking_cfg);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Set DpX2DpUC and  DpX2DpMC 1:1, for both unicast and multicast packets */
    for (i = 0; i < FX950_MH_MARKING_NOF_DP_LEVELS; i++)
    {
        dp_new[i] = i;
    }

    ret = fx950_api_mh_marking_set_dp_to_dp_map(deviceId, FX950_PACKETS_UNICAST, dp_new);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_mh_marking_set_dp_to_dp_map(deviceId, FX950_PACKETS_MULTICAST, dp_new);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Tc2Tc Unicast, map all to 0x1 */
    for (i = 0; i < FX950_MH_MARKING_NOF_TC_LEVELS; i++)
    {
        tc_new[i] = 1;
    }

    ret = fx950_api_mh_marking_set_tc_to_tc_map(deviceId, FX950_PACKETS_UNICAST, tc_new);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Tc2Tc Multicast, 0..5 -> 0, 6..7 -> 2 */
    for (i = 0; i < FX950_MH_MARKING_NOF_TC_LEVELS; i++)
    {
        if (i < 6)
        {
            tc_new[i] = 0;
        }
        else
        {
            tc_new[i] = 2;
        }
    }

    ret = fx950_api_mh_marking_set_tc_to_tc_map(deviceId, FX950_PACKETS_MULTICAST, tc_new);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* HGL TC mapping
       0,1 -> 0, 2,3 -> 1, 4,5 -> 2, 6,7 -> 3 */
    for (i = 0; i < FX950_MH_MARKING_NOF_TC_LEVELS; i++)
    {
        switch ((i >> 1) & 0x11)
        {
            case 0:
                tc_new[i] = 0;
                break;
            case 1:
                tc_new[i] = 1;
                break;
            case 2:
                tc_new[i] = 2;
                break;
            case 3:
                tc_new[i] = 3;
                break;
            default:
                break;
        }
    }

    ret = fx950_api_mh_marking_set_tc_to_hgl_tc_map(deviceId, tc_new);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Tail Drop */

    /* Link Thresholds */
    ret = fx950_api_tail_drop_set_link_thresholds(deviceId, 0x5a6, 0x5a6);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Queue Thresholds */
    for (i  = 0; i < 2; i++)
    {
        ret = fx950_api_tail_drop_set_queue_thresholds(deviceId, i, 0xb4, 0x5fa,
                                                       0xb4, 0x5fa);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }

    /* Chunk Context Manager */
    ret = fx950_api_chunk_context_manager_set_const_context_id(deviceId, 0, 0xBB8);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }


    /* MCDP Table */

#ifndef ASIC_SIMULATION
    /* MCID 2 link Mapping,
       each entry map - mcid(0..15) to link0, mcid(16..31) to link1 */
    for (i = 0; i < 256; i++)
    {
        ret = fx950_api_queue_selection_set_mc_id_range_to_link_map(
                            deviceId,
                            (i * 32),
                            16,
                            FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_LINK0_ENABLED);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }

        ret = fx950_api_queue_selection_set_mc_id_range_to_link_map(
                            deviceId,
                            ((i * 32) + 16),
                            16,
                            FX950_QUEUE_SELECTION_MC_ID_TO_LINK_MAP_LINK1_ENABLED);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }


    /* CPU Code to link Mapping,
       CPU codes(0..127) to link0, CPU codes(128..255) to link1 */
    cpu_code_to_link_bmp.bitmap[0] = 0x0;
    cpu_code_to_link_bmp.bitmap[1] = 0x0;
    cpu_code_to_link_bmp.bitmap[2] = 0xffffffff;
    cpu_code_to_link_bmp.bitmap[3] = 0xffffffff;

    ret = fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp(deviceId,
                                                                  &cpu_code_to_link_bmp);

    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* DevPort to link Mapping,
      ports(0..11) to link0, ports(12..23) to link1,
      port24 to link1, port25 to link0 */
    port_bmp.bitmap[0] = 0x1fff000;
    port_bmp.bitmap[1] = 0x0;

    ret = fx950_api_queue_selection_set_ports_to_link_map_bmp(deviceId,
                                                              &port_bmp);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }


    /* Flow-ID to link/prio mapping,
       4..99 to link0/prio0,
       100..195 to link1/prio0,
       196..203 to link0/prio0,
       204..211 to link1/prio0,
       1004..1099 to link0/prio0,
       1100..1195 to link1/prio0,
       1196..1203 to link0/prio0,
       1204..1211 to link1/prio0,
       2000..2127 to link0/prio1,
       2128..2255 to link1/prio1 */

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 4, 96, 0, 0);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 100, 96, 1, 0);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 196, 8, 0, 0);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 204, 8, 1, 0);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 1004, 96, 0, 0);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 1100, 96, 1, 0);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 1196, 8, 0, 0);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 1204, 8, 1, 0);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 2000, 128, 0, 1);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(
                                                deviceId, 2128, 128, 1, 1);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /* Flow ID to Context ID, FlowID = ContextID,
       Set the Valid bits only on the relevant FlowIds as above */
    for (i = 4; i < 212; i++)
    {
        ret = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(
                                    deviceId, i, GT_TRUE, i);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }

    for (i = 1004; i < 1212; i++)
    {
        ret = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(
                                    deviceId, i, GT_TRUE, i);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }

    for (i = 2000; i < 2256; i++)
    {
        ret = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(
                                    deviceId, i, GT_TRUE, i);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }
#endif

    /* link level to SDWRR weight, scheduler configuration and
       SDWRR weights */
    for (i = 0; i < 2; i++)
    {
        ret = fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(deviceId, i, 0x63);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }

        ret = fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(
                                                    deviceId,
                                                    i,
                                                    FX950_HGL_SCHEDULER_SDWRR);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }

        ret = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(
                                                    deviceId,
                                                    i,
                                                    FX950_HGL_SCHEDULER_SDWRR,
                                                    FX950_HGL_SCHEDULER_SP,
                                                    FX950_HGL_SCHEDULER_SP,
                                                    FX950_HGL_SCHEDULER_SP);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }

        if (i == 0)
        {
            ret = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(
                                            deviceId, i, 0x1, 0x2, 0x2, 0x2);
            sand_ret = sand_get_error_code_from_error_word(ret) ;
            if (sand_ret != SAND_OK)
            {
                return GT_FAIL;
            }
        }
        else
        {
            ret = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(
                                            deviceId, i, 0x2, 0x1, 0x2, 0x2);
            sand_ret = sand_get_error_code_from_error_word(ret) ;
            if (sand_ret != SAND_OK)
            {
                return GT_FAIL;
            }
        }
    }


    return GT_OK;
}

/*******************************************************************************
* afterInitBoardConfigOcelot
*
* DESCRIPTION:
*       FX950 and FAP20M initialization and specific configuration
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       ppType          - PP type: 24G ports or 2XG ports
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
static GT_STATUS afterInitBoardConfigOcelot
(
    IN  GT_U8                       boardRevId,
    IN  APPDEMO_FAP20M_PP_TYPE_ENT  ppType
)
{
    unsigned int        deviceId;               /* device ID of FAP in SW */
    unsigned int        fapId;                  /* Fabric ID of FAP (equal to PP device ID) */
    unsigned int        maxNumOfFapsInSystem;   /* maximum FAPs in the system */
    unsigned long       *fapBaseAddr;
    unsigned long       *fapPexBaseAddr;
    GT_STATUS           rc;                     /* return code */


    /* init SAND module */
    rc = appDemoSandInit();
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Bind read/write callbacks to SAND module */
    rc = appDemoFap20mHwAccessInit();
    if (rc != GT_OK)
    {
        return rc;
    }

/* Get Base Address */
#ifdef ASIC_SIMULATION
    baseAddress = 0x10;
    pexBaseAddress = 0x20;
#endif

    /* this satting is ok only for one FAP, if there are more, it should
       be done in loop for all FAPs, FAP DB should hold all the data */
    fapBaseAddr = (unsigned long *)baseAddress;
    fapPexBaseAddr = (unsigned long *)pexBaseAddress;

    /* assign FAP ID to be equal PP Device Number */
    fapId = appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devNum;

    /* Set parameters according to Negev system */
    maxNumOfFapsInSystem = APPDEMO_FX950_NEGEV_NOF_FAPS * 2;

    rc = appDemoFap20mDeviceInit(fapBaseAddr, fapPexBaseAddr, ppType,
                                 fapId, maxNumOfFapsInSystem, &deviceId);
    if (rc != GT_OK)
    {
        return GT_INIT_ERROR;
    }

    rc = afterInitBoardConfigFx950(deviceId, fapId, maxNumOfFapsInSystem);
    if (rc != GT_OK)
    {
        return rc;
    }

#if (!(defined _linux) && !(defined _FreeBSD) && !(defined ASIC_SIMULATION))
{
    void                *intVec;                /* interrupt vector */
    GT_U32              intVecNum;              /* interrupt vector */
    GT_U32              intMask;                /* interrupt mask */

    /* get interrupt vector and mask according to PEX INT_D signal */
    extDrvGetPciIntVec(GT_PCI_INT_A, &intVec);
    /* INT_D is INT_A + 3 */
    intVecNum  = ((GT_U32)intVec) + 3;

    extDrvGetIntMask(GT_PCI_INT_A,&intMask);
    /* INT_D is INT_A + 3 */
    intMask += 3;

    rc = appDemoFap20mInitInterrupts(deviceId, intVecNum, intMask);
    if (rc != GT_OK)
    {
        return rc;
    }
}
#endif
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
    IN  GT_U8                       boardRevId
)
{
    APPDEMO_FAP20M_PP_TYPE_ENT  ppType;     /* PP type: 24G ports or 2XG ports */
    GT_STATUS rc;                           /* return code */

    /* Packet Processor board specific configurations */
    rc = afterInitBoardConfigPp(boardRevId, &ppType);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Ocelot configuration */
    rc = afterInitBoardConfigOcelot(boardRevId, ppType);
    if(rc != GT_OK)
    {
        return rc;
    }


    return GT_OK;

}

/*******************************************************************************
* gtDbFx950Ex240BoardReg
*
* DESCRIPTION:
*       Registeration function for the DB-FX950-Ex240-GP board (Puma2 devices).
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
GT_STATUS gtDbFx950Ex240BoardReg
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    GT_STATUS rc;                           /* return code */

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

/*******************************************************************************
* prvCpssExMxPmFx950InitialFaConfig
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
GT_STATUS prvCpssExMxPmFx950InitialFaConfig
(
    IN  GT_U8          devNum
)
{
    CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC  vidxConfig; /*vidx configuration struct*/
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC    fportConfig;
    CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC          xbarConfig;
    CPSS_EXMXPM_FABRIC_E2E_MSG_CONFIG_STC       msgConfig;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CONTROL_CONFIG_STC controlConfig;
    CPSS_EXMXPM_FABRIC_E2E_MSG_PATH_STC         msgPath;
    GT_U32                              voqTrgDevsBmpArr[2];/* target devs bit
                                                                    map array */
    GT_FPORT                            fport; /* fabric port number */
    GT_U8                               trgDev;/* target device number */
    GT_FPORT                            fportsArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];
    GT_U32                              index;
    GT_U32                              lbh;
    CPSS_PORTS_BMP_STC                  portsBitmap; /* port bitmap */
    GT_U8                               phyPort;    /* physical port number */
    GT_STATUS                           rc;         /* return code */


    vidxConfig.fabricVIDX0 = 0x1;
    vidxConfig.fabricVIDX1 = 0x1;
    vidxConfig.fabricVidxBit0IsLbiBit0 = GT_FALSE;
    vidxConfig.fabricVIDXMode = CPSS_EXMXPM_FABRIC_CONFIG_VIDX_FLOOD_ALL_E;
    /* LBI and Fabric VIDX configuration */
    if ((rc = cpssExMxPmFabricConfigVidxSet(devNum,&vidxConfig)) != GT_OK)
        return rc;

    voqTrgDevsBmpArr[0] = 0xffffffff;
    voqTrgDevsBmpArr[1] = 0xffffffff;

    /* enable VOQ 0 devs */
    if ((rc = cpssExMxPmFabricVoqDevTblSet(devNum, 0, voqTrgDevsBmpArr)) !=
        GT_OK)
        return rc;

    /* enable VOQ 1 devs */
    if ((rc = cpssExMxPmFabricVoqDevTblSet(devNum, 1, voqTrgDevsBmpArr)) !=
        GT_OK)
        return rc;

    for (fport = 0 ; fport < CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS ; fport++)
    {
        /* enable uc fabric ports units 0,1 */
        if ((rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 0, fport, GT_TRUE))
            != GT_OK)
            return rc;
        if ((rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 1, fport, GT_TRUE))
            != GT_OK)
            return rc;
    }

    /* enable mc fabric ports units 0,1 */
    if ((rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 0,
                                                  CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS, GT_TRUE)) != GT_OK)
        return rc;

    /* enable mc fabric ports units 0,1 */
    if ((rc = cpssExMxPmFabricVoqFportTxEnableSet(devNum, 1,
                                                  CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS, GT_TRUE)) != GT_OK)
        return rc;

    for (index = 0 ; index < CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS ; index++)
        fportsArr[index] = 0x0;

    for (trgDev = 0 ; trgDev < 64 ; trgDev++)
    {
        /* map VOQ unit 0 to fport 0 */
        if ((rc = cpssExMxPmFabricVoqDevToFportMapSet(devNum, 0, trgDev,
                                                      fportsArr))!= GT_OK)
        return rc;
    }

    for (index = 0 ; index < CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS ; index++)
        fportsArr[index] = 0x2;

    for (trgDev = 0 ; trgDev < 64 ; trgDev++)
    {
        /* map VOQ unit 1 to fport 2 */
        if ((rc = cpssExMxPmFabricVoqDevToFportMapSet(devNum, 1, trgDev,
                                                      fportsArr))!= GT_OK)
        return rc;
    }

    xbarConfig.mode = CPSS_EXMXPM_FABRIC_XBAR_BYPASS_ACTIVE_STANDBY_E;
    xbarConfig.xbarTcModeEnable = GT_FALSE;
    xbarConfig.voQUnit0ActiveHyperGlink = 0;
    xbarConfig.voQUnit1ActiveHyperGlink = 2;

    /*  Crossbar Global Control Address - enable XBAR, disable Traffic Classes
        XBAR mode and configure FPort 0  and 2 as active  for standby mode */
    if ((rc = cpssExMxPmFabricXbarConfigSet(devNum, &xbarConfig)) != GT_OK)
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
        if ((rc = cpssExMxPmFabricXbarFportConfigSet(devNum, fport,
                                                     &fportConfig)) != GT_OK)
            return rc;
    }

    /* Configure the unicast table, for fport 0,2, where trgPorts are 4,5
       correspondently */
    for (trgDev = 0 ; trgDev < 64 ; trgDev++)
    {
        for (lbh = 0; lbh < 4; lbh++)
        {
            if((rc =
                cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(devNum,
                         0, trgDev , lbh, GT_TRUE, 4)) != GT_OK)
                return rc;
        }
    }


    for (trgDev = 0 ; trgDev < 64 ; trgDev++)
    {
        for (lbh = 0; lbh < 4; lbh++)
        {
            if((rc =
                cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(devNum,
                         2, trgDev , lbh, GT_TRUE, 5)) != GT_OK)
                return rc;
        }
    }

    /* Configure the multicast table */
    if((rc = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(devNum,
                     CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP0_E, 1, 0x10)) != GT_OK)
       return rc;

    if((rc = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(devNum,
                     CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP1_E, 1, 0x20)) != GT_OK)
       return rc;

    /* E2E configuration */
    msgConfig.e2eEnable = GT_TRUE;
    msgConfig.periodicCellFrequence = 0x200;    /* default */
    msgConfig.urgentCellFrequence = 0xc0;       /* default */
    msgConfig.urgentMode = CPSS_EXMXPM_FABRIC_E2E_URGENT_NOT_READY_FOR_DP1_E; /* default */

    if((rc = cpssExMxPmFabricE2eMsgConfigSet(devNum, &msgConfig)) != GT_OK)
       return rc;



    /* Map for all ports, physical to virtual 1 to 1 */
    for (phyPort = 0; phyPort < 26; phyPort++)
    {
        /* check that port exist */
        if( !(PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[0] & (1 << phyPort)))
            continue;

        portsBitmap.ports[0] = (1 << phyPort);

        if((rc = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(devNum,
                                                                         phyPort,
                                                                         &portsBitmap)) != GT_OK)
           return rc;
    }

    /* For CPU port (63), map to virtual port 63 */
    phyPort = 63;
    portsBitmap.ports[0] = 0;
    portsBitmap.ports[1] = (1 << 31);

    if((rc = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(devNum,
                                                                     phyPort,
                                                                     &portsBitmap)) != GT_OK)
       return rc;

    controlConfig.bindContextMode = CPSS_EXMXPM_FABRIC_BIND_CONTEXT_FLOW_BASED_E;
    controlConfig.checkCrc = GT_TRUE;
    controlConfig.maxNumOfBuffers = 0x14;
    controlConfig.buffersDropEnable = GT_TRUE;
    controlConfig.remoteCpuCellAction =
        CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_PASS_E;

    if((rc = cpssExMxPmFabricPktReasmControlConfigSet(devNum, &controlConfig)) != GT_OK)
        return rc;


    if((rc = cpssExMxPmFabricE2eVoqDropEnableSet(devNum, 0, GT_FALSE)) != GT_OK)
        return rc;
    if((rc = cpssExMxPmFabricE2eVoqDropEnableSet(devNum, 1, GT_FALSE)) != GT_OK)
        return rc;

    msgPath.e2eFabricTc = 3;
    msgPath.e2eVoq = 1;
    if((rc = cpssExMxPmFabricE2eMsgPathSet(devNum, &msgPath)) != GT_OK)
        return rc;


    return GT_OK;

}




