/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxBobcat2.c
*
* DESCRIPTION:
*       Initialization functions for the Bobcat2 - SIP5 - board.
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/gtAppDemoTmConfig.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtDbDxBobcat2Mappings.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgE2Phy.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/init/cpssInit.h>

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSecurityBreach.h>

extern GT_STATUS appDemoBc2IpLpmRamDefaultConfigCalc
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   maxNumOfPbrEntries,
    OUT APP_DEMO_CPSS_LPM_RAM_MEM_BLOCKS_CFG_STC *ramDbCfgPtr
);

/*******************************************************************************
 * Global variables
 ******************************************************************************/
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
        return GT_NOT_FOUND;
    }

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = GT_FALSE;/* no B2B with Bobcat2 devices */

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
    GT_STATUS rc = GT_OK;
    CPSS_PP_PHASE1_INIT_PARAMS    localPpPh1Config = CPSS_PP_PHASE1_DEFAULT;

    boardRevId = boardRevId;

    localPpPh1Config.devNum = devIdx + 1;

    localPpPh1Config.baseAddr =
                        appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
    localPpPh1Config.internalPciBase =
                        appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
    localPpPh1Config.deviceId =
        ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
         (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

    localPpPh1Config.intMask   =
    localPpPh1Config.intVecNum = devIdx + 1;
    localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PEX_E;
    localPpPh1Config.numOfPortGroups = appDemoPpConfigList[devIdx].numOfPortGroups;

    /* retrieve PP Core Clock from HW */
    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

    /* current Bobcat2 box has 156.25MHz differencial serdesRefClock */
    localPpPh1Config.serdesRefClock = APPDEMO_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E;

    localPpPh1Config.uplinkCfg = CPSS_PP_NO_UPLINK_E;

    appDemoPpConfigList[devIdx].flowControlDisable = GT_TRUE;

    osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));

    return rc;
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

#define GT_NA ((GT_U32)~0)
/*
typedef struct
{
    GT_PHYSICAL_PORT_NUM                physicalPortNumber;
    CPSS_DXCH_PORT_MAPPING_TYPE_ENT     mappingType;
    GT_U32                              portGroup;
    GT_U32                              interfaceNum;
    GT_U32                              txqPortNumber;
    GT_BOOL                             tmEnable;
} CPSS_DXCH_PORT_MAP_STC;
*/

/* ports 48 - 56 have one to one mapping for physical, MAC and TxQ ports.
  MAC 60 use physical port 80 because 60 is reserved one */

static CPSS_DXCH_PORT_MAP_STC bc2defaultMap[] =
{ /* Port,            mappingType                portGroupm, intefaceNum, txQPort, trafficManagerEnable, tmPortInd*/
    {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       0,       0,      GT_FALSE,          GT_NA},
    {   1, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       1,       1,      GT_FALSE,          GT_NA},
    {   2, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       2,       2,      GT_FALSE,          GT_NA},
    {   3, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       3,       3,      GT_FALSE,          GT_NA},
    {   4, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       4,       4,      GT_FALSE,          GT_NA},
    {   5, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       5,       5,      GT_FALSE,          GT_NA},
    {   6, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       6,       6,      GT_FALSE,          GT_NA},
    {   7, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       7,       7,      GT_FALSE,          GT_NA},
    {   8, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       8,       8,      GT_FALSE,          GT_NA},
    {   9, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,       9,       9,      GT_FALSE,          GT_NA},
    {  10, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      10,      10,      GT_FALSE,          GT_NA},
    {  11, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      11,      11,      GT_FALSE,          GT_NA},
    {  12, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      12,      12,      GT_FALSE,          GT_NA},
    {  13, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      13,      13,      GT_FALSE,          GT_NA},
    {  14, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      14,      14,      GT_FALSE,          GT_NA},
    {  15, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      15,      15,      GT_FALSE,          GT_NA},
    {  16, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      16,      16,      GT_FALSE,          GT_NA},
    {  17, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      17,      17,      GT_FALSE,          GT_NA},
    {  18, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      18,      18,      GT_FALSE,          GT_NA},
    {  19, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      19,      19,      GT_FALSE,          GT_NA},
    {  20, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      20,      20,      GT_FALSE,          GT_NA},
    {  21, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      21,      21,      GT_FALSE,          GT_NA},
    {  22, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      22,      22,      GT_FALSE,          GT_NA},
    {  23, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      23,      23,      GT_FALSE,          GT_NA},
    {  24, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      24,      24,      GT_FALSE,          GT_NA},
    {  25, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      25,      25,      GT_FALSE,          GT_NA},
    {  26, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      26,      26,      GT_FALSE,          GT_NA},
    {  27, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      27,      27,      GT_FALSE,          GT_NA},
    {  28, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      28,      28,      GT_FALSE,          GT_NA},
    {  29, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      29,      29,      GT_FALSE,          GT_NA},
    {  30, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      30,      30,      GT_FALSE,          GT_NA},
    {  31, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      31,      31,      GT_FALSE,          GT_NA},
    {  32, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      32,      32,      GT_FALSE,          GT_NA},
    {  33, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      33,      33,      GT_FALSE,          GT_NA},
    {  34, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      34,      34,      GT_FALSE,          GT_NA},
    {  35, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      35,      35,      GT_FALSE,          GT_NA},
    {  36, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      36,      36,      GT_FALSE,          GT_NA},
    {  37, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      37,      37,      GT_FALSE,          GT_NA},
    {  38, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      38,      38,      GT_FALSE,          GT_NA},
    {  39, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      39,      39,      GT_FALSE,          GT_NA},
    {  40, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      40,      40,      GT_FALSE,          GT_NA},
    {  41, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      41,      41,      GT_FALSE,          GT_NA},
    {  42, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      42,      42,      GT_FALSE,          GT_NA},
    {  43, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      43,      43,      GT_FALSE,          GT_NA},
    {  44, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      44,      44,      GT_FALSE,          GT_NA},
    {  45, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      45,      45,      GT_FALSE,          GT_NA},
    {  46, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      46,      46,      GT_FALSE,          GT_NA},
    {  47, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      47,      47,      GT_FALSE,          GT_NA},
    {  48, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      48,      48,      GT_FALSE,          GT_NA},
    {  49, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      49,      49,      GT_FALSE,          GT_NA},
    {  50, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      50,      50,      GT_FALSE,          GT_NA},
    {  51, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      51,      51,      GT_FALSE,          GT_NA},
    {  52, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      52,      52,      GT_FALSE,          GT_NA},
    {  53, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      53,      53,      GT_FALSE,          GT_NA},
    {  54, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      54,      54,      GT_FALSE,          GT_NA},
    {  55, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      55,      55,      GT_FALSE,          GT_NA},
    {  56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      56,      56,      GT_FALSE,          GT_NA},
    {  80, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,   0,      60,      60,      GT_FALSE,          GT_NA}, /* use Physical port 80 because 60 is reserved one */
    {  63, CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E,       0,   GT_NA,      63,      GT_FALSE,          GT_NA}
};

static GT_STATUS configBoardAfterPhase1
(
    IN  GT_U8   boardRevId
)
{

    GT_U8       dev;
    GT_STATUS   rc;
    GT_U8       tempPpCounter = ppCounter;

    /*for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)*/
    for (dev = 0; dev < PRV_CPSS_MAX_PP_DEVICES_CNS && tempPpCounter; dev++)
    {
        CPSS_RESOURCE_MAPPING_STC resourceMapping;

        if (!appDemoPpConfigList[dev].valid)
        {
            continue;
        }

        tempPpCounter--;

        resourceMapping.sramBaseAddress = appDemoPpConfigList[dev].pciInfo.resourceMapping.sram.base;
        resourceMapping.sramSize = (GT_U32)appDemoPpConfigList[dev].pciInfo.resourceMapping.sram.size;
        rc = cpssHwInitResourceMappingSet(
                    appDemoPpConfigList[dev].devNum, &resourceMapping);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssHwInitResourceMappingSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortPhysicalPortMapSet(appDemoPpConfigList[dev].devNum,
											sizeof(bc2defaultMap)/sizeof(bc2defaultMap[0]), 
											&bc2defaultMap[0]);
		CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
		if (GT_OK != rc)
		{
			return rc;
		}

		rc = appDemoDxChMaxMappedPortSet(appDemoPpConfigList[dev].devNum,
										 sizeof(bc2defaultMap)/sizeof(bc2defaultMap[0]), 
										 &bc2defaultMap[0]);
		CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
		if (GT_OK != rc)
		{
			return rc;
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
    OUT CPSS_PP_PHASE2_INIT_PARAMS  *phase2Params
)
{
    GT_STATUS                   rc;
    CPSS_PP_PHASE2_INIT_PARAMS  localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS                   retVal;
    GT_U32                      auDescNum;
    GT_U32                      fuDescNum;
    GT_U32                      tmpData;

    boardRevId = boardRevId;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = appDemoPpConfigList[oldDevNum].deviceId;

    localPpPh2Config.fuqUseSeparate = GT_TRUE;

    /* Single AUQ, since single FDB unit */
    auDescNum = AU_DESC_NUM_DEF;

    fuDescNum = AU_DESC_NUM_DEF;

    if(appDemoDbEntryGet("fuDescNum", &tmpData) == GT_OK)
    {
        /* Someone already stated the number of fuDescNum, so we NOT override it ! */
    }
    else
    {
        /* Add the fuDescNum to the DB, to allow appDemoAllocateDmaMem(...) to use it ! */
        rc = appDemoDbEntryAdd("fuDescNum", fuDescNum);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Check if to use the default configuration for appDemo of */
    /* Tx SDMA Packet Generator queues.  */
    if( appDemoDbEntryGet("skipTxSdmaGenDefaultCfg", &tmpData) == GT_NO_SUCH )
    {
        /* allow to work in Tx queue generator mode */
        rc = appDemoDbEntryAdd("useMultiNetIfSdma", GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Enable Tx queue 3 to work in Tx queue generator mode */
        rc = appDemoDbEntryAdd("txGenQueue_3", GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Enable Tx queue 6 to work in Tx queue generator mode */
        rc = appDemoDbEntryAdd("txGenQueue_6", GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId, RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF, RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,
                                   auDescNum, &localPpPh2Config);

    osMemCpy(phase2Params,&localPpPh2Config, sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

    phase2Params->auMessageLength = CPSS_AU_MESSAGE_LENGTH_8_WORDS_E;

    return retVal;
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
    GT_U32  dev;
    GT_PHYSICAL_PORT_NUM  port; /* port number */
    GT_STATUS   rc;

    boardRevId = boardRevId;

    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        /* internalXGPortConfig is not used for Bobcat2 devices with all ports 10 or 40G */
        appDemoPpConfigList[dev].numberOf10GPortsToConfigure = 0;
        appDemoPpConfigList[dev].internalCscdPortConfigFuncPtr = cascadePortConfig;

        /* ports 56 and 60 are cascade one for Fabric connection  */
        appDemoPpConfigList[dev].numberOfCscdPorts = 2;

        appDemoPpConfigList[dev].cscdPortsArr[0].portNum = 56;
        appDemoPpConfigList[dev].cscdPortsArr[0].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E;

        appDemoPpConfigList[dev].cscdPortsArr[1].portNum = 80; /* MAC 60 use Physical Port 80 */
        appDemoPpConfigList[dev].cscdPortsArr[1].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E;

        for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS;port++)
        {
            if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                &PRV_CPSS_PP_MAC(appDemoPpConfigList[dev].devNum)->existingPorts, port)))
            {
                continue;
            }

            /* split ports between MC FIFOs for Multicast arbiter */
            rc = cpssDxChPortTxMcFifoSet(appDemoPpConfigList[dev].devNum, port, port%2);
            if( GT_OK != rc )
            {
                return rc;
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
    GT_U32 rc = GT_OK;
    CPSS_PP_CONFIG_INIT_STC  localPpCfgParams = PP_LOGICAL_CONFIG_DEFAULT;

    boardRevId = boardRevId;
    devNum = devNum;

    localPpCfgParams.maxNumOfIpv4Prefixes = 3920;
    localPpCfgParams.numOfTrunks = _8K-1;
    localPpCfgParams.maxNumOfVirtualRouters = 1;
    localPpCfgParams.lpmDbFirstTcamLine = 100;
    localPpCfgParams.lpmDbLastTcamLine = 1204;
    localPpCfgParams.maxNumOfPbrEntries = _8K;

    rc = appDemoBc2IpLpmRamDefaultConfigCalc(devNum,localPpCfgParams.maxNumOfPbrEntries,&(localPpCfgParams.lpmRamMemoryBlocksCfg));
    if (rc != GT_OK)
    {
        return rc;
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

    boardRevId = boardRevId;
    devNum = devNum;

    localLibInitParams.initClassifier            = GT_FALSE;
    localLibInitParams.initIpv6                  = GT_TRUE;
    localLibInitParams.initIpv4                  = GT_TRUE;

    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
    localLibInitParams.initMpls                  = GT_FALSE;
    localLibInitParams.initMplsTunnel            = GT_FALSE;
    localLibInitParams.initPcl                   = GT_TRUE;
    localLibInitParams.initTcam                  = GT_TRUE;

    /* there is no Policer lib init for Bobcat2 devices */
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
    GT_STATUS   rc;
    GT_U8       dev;
    GT_U32      portNumber;
    CPSS_PORTS_BMP_STC initPortsBmp;/* bitmap of ports to init */
    GT_U32  initSerdesDefaults; /* should appDemo configure ports modes */
    boardRevId = boardRevId;

    /* allow processing of AA messages */
    appDemoSysConfig.supportAaMessage = GT_TRUE;

    if(appDemoSysConfig.forceAutoLearn == GT_FALSE)
    {
        /* the Bobcat2 need 'Controlled aging' because the port groups can't share refresh info */
        /* the AA to CPU enabled from appDemoDxChFdbInit(...) --> call cpssDxChBrgFdbAAandTAToCpuSet(...)*/
        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
        {
            rc = cpssDxChBrgFdbActionModeSet(
                appDemoPpConfigList[dev].devNum,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionModeSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        /* check if user wants to init ports to default values */
        rc = appDemoDbEntryGet("initSerdesDefaults", &initSerdesDefaults);
        if(rc != GT_OK)
        {
            initSerdesDefaults = 1;
        }

        if(initSerdesDefaults == 1)
        {
            if (CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E ==
                                                    systemRecoveryInfo.systemRecoveryProcess)
            {
                for (portNumber = 0; portNumber <= 18; portNumber++)
                {

                    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
                    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp,portNumber);
                    rc = cpssDxChPortModeSpeedSet(
                        appDemoPpConfigList[dev].devNum, initPortsBmp, GT_TRUE,
                        CPSS_PORT_INTERFACE_MODE_QSGMII_E,
                        CPSS_PORT_SPEED_1000_E);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortModeSpeedSet", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }

                for (portNumber = 48; portNumber <= 55; portNumber++)
                {

                    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
                    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp,portNumber);
                    rc = cpssDxChPortModeSpeedSet(
                        appDemoPpConfigList[dev].devNum, initPortsBmp, GT_TRUE,
                        CPSS_PORT_INTERFACE_MODE_KR_E,
                        CPSS_PORT_SPEED_10000_E);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortModeSpeedSet", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
                CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp,56);
                rc = cpssDxChPortModeSpeedSet(
                    appDemoPpConfigList[dev].devNum, initPortsBmp, GT_TRUE,
                    CPSS_PORT_INTERFACE_MODE_KR_E,
                    CPSS_PORT_SPEED_10000_E);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortModeSpeedSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
                CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
                CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp,80);
                rc = cpssDxChPortModeSpeedSet(
                    appDemoPpConfigList[dev].devNum, initPortsBmp, GT_TRUE,
                    CPSS_PORT_INTERFACE_MODE_KR_E,
                    CPSS_PORT_SPEED_10000_E);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortModeSpeedSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

            }
        }


        if(boardRevId == 2) {
            rc = appDemoTmGeneral48PortsInit(appDemoPpConfigList[dev].devNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoTmGeneral48PortsInit", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(appDemoPpConfigList[dev].devNum))
        {
            /* TBD: FE HA-3259 fix and removed from CPSS.
               Allow to the CPU to get the original vlan tag as payload after
               the DSA tag , so the info is not changed. */
            rc = cpssDxChBrgVlanForceNewDsaToCpuEnableSet(
                appDemoPpConfigList[dev].devNum, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanForceNewDsaToCpuEnableSet", rc);
            if( GT_OK != rc )
            {
                return rc;
            }

            /* RM of Bridge default values of Command registers have fixed in B0.
               But some bits need to be changed to A0 values.
               set bits 15..17 in PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).bridgeEngineConfig.bridgeCommandConfig0 */
            /* set the command of 'SA static moved' to be 'forward' because this command
               applied also on non security breach event ! */
            rc = cpssDxChBrgSecurBreachEventPacketCommandSet(
                appDemoPpConfigList[dev].devNum,
                CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E,
                CPSS_PACKET_CMD_FORWARD_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSecurBreachEventPacketCommandSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* gtDbDxBobcat2_X6_BoardReg
*
* DESCRIPTION:
*       Registration function for the Six BobCat2 (SIP5) board .
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
GT_STATUS gtDbDxBobcat2_X6_BoardReg
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    boardRevId = boardRevId;

    if(boardCfgFuncs == NULL)
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


