/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxLion2_X2_Bobcat2.c
*
* DESCRIPTION:
*       Initialization functions for the two Lion2 and one Bobcat2 board.
*       This board implicitly assume that the first two devices are Lion2 and
*       the next one is the Bobcat2.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/boardConfig/appDemoLion2TrunkWa.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/sysHwConfig/gtAppDemoLioncubConfig.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/init/cpssInit.h>

/* structures definition for the use of Dx Cheetah Prestera core SW */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>

#include <cpss/generic/smi/cpssGenSmi.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>

#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSrcId.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSecurityBreach.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgE2Phy.h>
#include <cpss/dxCh/dxChxGen/cutThrough/cpssDxChCutThrough.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortAp.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>
#include <cpss/dxCh/dxChxGen/diag/private/prvCpssDxChDiag.h>

#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGluePfc.h>

/* Pizza Arbiter and slices management for specific units */
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>

#include <appDemo/boardConfig/gtDbDxBobcat2Mappings.h>
#include <appDemo/boardConfig/gtDbDxBobcat2PhyConfig.h>

/* include the external services */
#include <cmdShell/common/cmdExtServices.h>

#include <gtOs/gtOsTimer.h>

/*******************************************************************************
 * External definitions
 ******************************************************************************/
extern CPSS_TM_SCENARIO   appDemoTmScenarioMode;

/*******************************************************************************
 * Global variables
 ******************************************************************************/
static GT_U8    ppCounter = 0;      /* Number of all Packet processors. */
static GT_U8    ppLion2Counter = 0; /* Number of Lion2 Packet processors. */

extern GT_STATUS appDemoDebugDeviceIdSet
(
    IN GT_U8    devNum,
    IN GT_U32   deviceId
);


extern GT_STATUS appDemoDebugDeviceIdGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *deviceIdPtr
);

extern GT_STATUS lion2AppDemoCpuPortPhyConfig
(
    IN  GT_U8   devNum
);

extern GT_STATUS lion2AppDemoPortInterfaceInit
(
    IN  GT_U8 dev,
    IN  GT_U8 boardRevId
);

extern GT_STATUS appDemoDxChMaxMappedPortSet
(
    IN  GT_U8  dev,
    IN  GT_U32 mapArrLen,
    IN  CPSS_DXCH_PORT_MAP_STC *mapArrPtr
);

extern GT_STATUS appDemoBc2PortInterfaceInit
(
    IN  GT_U8 dev,
    IN  GT_U8 boardRevId
);

extern GT_STATUS appDemoB2PtpConfig
(
    IN  GT_U8  devNum
);

#ifndef ASIC_SIMULATION_NO_HW
/* I intentionaly not define it as static to enable init at any time from command
   line */
extern GT_STATUS lion2AppDemoApInit
(
    IN  GT_U8   devNum
);

extern unsigned __TASKCONV appDemoLinkFix
(
    GT_VOID * param
);

#endif

extern GT_STATUS prvCpssDxChPortTraceDelay
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_U32      millisec
);

/* default board revision for standalone BC2 anf for standalone Hooper */
#define HOOPER_DEF_STANDALONE_BOARD_REV      1
#define BOBCAT2_DEF_STANDALONE_BOARD_REV     1

extern GT_STATUS appDemoBc2IpLpmRamDefaultConfigCalc
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   maxNumOfPbrEntries,
    OUT APP_DEMO_CPSS_LPM_RAM_MEM_BLOCKS_CFG_STC *ramDbCfgPtr
);

/* Device ID index for appDemoPpConfigList access*/
/* First Lion2*/
#define LION2_0_DEV_IND 0
/* Second Lion2*/
#define  LION2_1_DEV_IND 1
/* Bobcat2*/
/*#define  BOBCAT2_DEV_IND 2*/
/* default value - 2 (0,1) - Lions/Hoopers, 2 - BC2 */
/* For one Hooper and BC2 confuiguration 0 - Hooper, 1 - BC2 */
static int  BOBCAT2_DEV_IND = 2;


/* SW devic IDs*/
/* SW device ID for first Lion2 */
#define LION2_0_SW_DEV_NUM 0
/* SW device ID for second Lion2, must be pair, therefore 2 is selectd */
#define LION2_1_SW_DEV_NUM 2
/* SW device ID for Bobcat2 */
#define BOBCAT2_SW_DEV_NUM 4

/* HW devic IDs */
/* HW device ID for first Lion2 */
#define LION2_0_HW_DEV_NUM 28
/* HW device ID for second Lion2 */
#define LION2_1_HW_DEV_NUM 30
/* HW device ID for Bobcat2 */
#define BOBCAT2_HW_DEV_NUM 8


#define LION2_NUM_OF_PORT_GROUPS 8

#define PRV_CPSS_DXCH_UNUSED_PARAM_MAC(p) p = p

#define BC2_IS_ON

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
    GT_U8       i, j;                      /* Loop index.                  */
    GT_STATUS   rc;
    GT_U32      managementBus;
    GT_U8       basePceSlot[16];
    GT_U8       currNumOfPceSlots;
    GT_U8       idx[3];
    GT_U8       numOfPciDevs;
    GT_U8       nextPciDev;
    GT_BOOL     dualDev;

    /* This is explicitly two Lion2 and one Bobcat2 (optional). */
    /* boardRevId == 1: two Lion2 */
    /* boardRevId == 2: two Lion2 and one Bobcat2 */
    /* boardRevId == 3: two Hoopers and one Bobcat2 */
    /* boardRevId == 4: one Hooper  and one Bobcat2 */
    if ((boardRevId < 1) || (boardRevId > 4))
    {
        cpssOsPrintf("Not supported boadRevId[%d]\n",boardRevId);
        return GT_FAIL;
    }

    if(appDemoDbEntryGet("bus", &managementBus) != GT_OK)
    {
        managementBus = 0;
    }

    if(0 == managementBus)
    {
        gtPresteraPciDontMap = GT_TRUE;
        rc = appDemoSysGetPciInfo();
        if(rc != GT_OK)
        {
            return GT_NOT_FOUND;
        }
    }
    else
    {
        rc = appDemoSysGetSmiInfo();
        if(rc != GT_OK)
        {
            return GT_NOT_FOUND;
        }

        CPSS_TBD_BOOKMARK_LION2
        /* SMI supports traffic to CPU only by RGMII interface */
    }

    /* special case - Bobcat2 and two Hoopers */
    /* or Bobcat2 and one Hooper              */
    if ( (boardRevId == 3) || (boardRevId == 4) )
    {
        {GT_U32 tmpData = 0;
        if ((appDemoDbEntryGet("debugPrint", &tmpData) == GT_OK) && (tmpData != 0))
        {
            appDemoDbPpDump();
            osTimerWkAfter(300); /* time to finish previous print */
        }}

        dualDev = GT_FALSE;
        numOfPciDevs = 0;
        nextPciDev = SYSTEM_DEV_NUM_MAC(0);
        for (i = SYSTEM_DEV_NUM_MAC(0); (i < APP_DEMO_PP_CONFIG_SIZE_CNS); i++)
        {
            if (i < nextPciDev)
            {
                /* check slots being skipped */
                if (appDemoPpConfigList[i].valid == GT_FALSE)
                {
                    cpssOsPrintf("Error in PCE devices sequence (validity)\n");
                    return GT_FAIL;
                }
                if (appDemoPpConfigList[i].devFamily !=
                    appDemoPpConfigList[basePceSlot[numOfPciDevs - 1]].devFamily)
                {
                    cpssOsPrintf("Error in PCE devices sequence (family)\n");
                    return GT_FAIL;
                }
                /* skip non first slots of device */
                appDemoPpConfigList[i].valid = GT_FALSE;
                continue;
            }
            if (! appDemoPpConfigList[i].valid)
            {
                break;
            }
            basePceSlot[numOfPciDevs] = i;
            if (appDemoPpConfigList[i].devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
            {
                if ((appDemoPpConfigList[i].pciInfo.pciDevVendorId.devId & 0xFF00) == 0x8000)
                {
                    /* HOOPER */
                    currNumOfPceSlots = 4;
                }
                else
                {
                    /* LION2*/
                    currNumOfPceSlots = 8;
                    dualDev = GT_FALSE;
                }
            }
            else
            {
                /* BOBCAT2*/
                currNumOfPceSlots = 1;
            }
            /* update base PCE Slot to represent all port grops */
            appDemoPpConfigList[i].numOfPortGroups =
                (currNumOfPceSlots > 1) ? currNumOfPceSlots : 0;

            for (j = 0; (j < currNumOfPceSlots) && (i+j < APP_DEMO_PP_CONFIG_SIZE_CNS); j++)
            {
                if (CPSS_CHANNEL_PCI_E == appDemoPpConfigList[i].channel)
                {
                    appDemoPpConfigList[i].portGroupsInfo[j].portGroupPciInfo =
                        appDemoPpConfigList[i+j].pciInfo;
                }
                else
                {
                    appDemoPpConfigList[i].portGroupsInfo[j].portGroupSmiInfo =
                        appDemoPpConfigList[i+j].smiInfo;
                }
                appDemoPpConfigList[i].portGroupsInfo[j].portGroupActive = GT_TRUE;
            }

            /* validate devices */
            if (currNumOfPceSlots > 1)
            {
                appDemoPpConfigList[i].pciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
                appDemoPpConfigList[i].pciInfo.internalPciBase =  CPSS_PARAM_NOT_USED_CNS;
                appDemoPpConfigList[i].smiInfo.smiIdSel = CPSS_PARAM_NOT_USED_CNS;
            }

            if ((appDemoSysConfig.cpuEtherPortUsed)  /* LION2_MII */
                && (currNumOfPceSlots > 1))
            {
                appDemoPpConfigList[i].cpuPortMode = CPSS_NET_CPU_PORT_MODE_MII_E;
            }

            /* for next iteration */
            nextPciDev = basePceSlot[numOfPciDevs] + currNumOfPceSlots;
            numOfPciDevs ++;
            if (numOfPciDevs >= 16)
            {
                return GT_BAD_STATE;
            }
        }
        if ((boardRevId == 3) && (numOfPciDevs != 3))
        {
            cpssOsPrintf("Found %d PCE devices instead of 3\n", numOfPciDevs);
            return GT_FAIL;
        }
        if ((boardRevId == 4) && (numOfPciDevs != 2))
        {
            cpssOsPrintf("Found %d PCE devices instead of 2\n", numOfPciDevs);
            return GT_FAIL;
        }

        /* move PCE devices to predefined positions */
        /* indexes Hooper0, Hooper1, Bobcat2        */
        for (i = 0; (i < numOfPciDevs); i++) {idx[i] = 0xFF;}
        for (i = 0; (i < numOfPciDevs); i++)
        {
            if (appDemoPpConfigList[basePceSlot[i]].devFamily
                == CPSS_PP_FAMILY_DXCH_LION2_E)
            {
                if (idx[0] == 0xFF)
                {
                    idx[0] = basePceSlot[i];
                }
                else
                {
                    /* second Hooper - only for (boardRevId == 3) */
                    idx[1] = basePceSlot[i];
                }
            }
            if (appDemoPpConfigList[basePceSlot[i]].devFamily
                == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
            {
                idx[numOfPciDevs - 1] = basePceSlot[i];
            }
        }
        for (i = 0; (i < numOfPciDevs); i++)
        {
            if (idx[i] == 0xFF)
            {
                cpssOsPrintf("Error in PCE devices sequence\n");
                return GT_FAIL;
            }
        }
        /* copy to appDemoPpConfigList[100]-[102] to avoid overlapping*/
        for (i = 0; (i < numOfPciDevs); i++)
        {
            cpssOsMemCpy(
                &(appDemoPpConfigList[100 + i]),
                &(appDemoPpConfigList[idx[i]]),
                sizeof(APP_DEMO_PP_CONFIG));
            appDemoPpConfigList[idx[i]].valid = GT_FALSE;
        }
        /* copy from appDemoPpConfigList[100]-[102] to proper places */
        if (boardRevId == 3)
        {
            BOBCAT2_DEV_IND = 2;
            idx[0] = LION2_0_DEV_IND;
            idx[1] = LION2_1_DEV_IND;
            idx[2] = BOBCAT2_DEV_IND;
        }
        else
        {
            /*boardRevId == 4*/
            BOBCAT2_DEV_IND = 1;
            idx[0] = LION2_0_DEV_IND;
            idx[1] = BOBCAT2_DEV_IND;
        }
        for (i = 0; (i < numOfPciDevs); i++)
        {
            cpssOsMemCpy(
                &(appDemoPpConfigList[idx[i]]),
                &(appDemoPpConfigList[100 + i]),
                sizeof(APP_DEMO_PP_CONFIG));
            appDemoPpConfigList[100 + i].valid = GT_FALSE;
        }

        rc = cpssSystemDualDeviceIdModeEnableSet(dualDev);
        if(rc != GT_OK)
        {
            return rc;
        }

        ppCounter = numOfPciDevs;
        ppLion2Counter = (ppCounter - 1);
        *numOfPp    = ppCounter;
        *numOfFa    = 0;
        *numOfXbar  = 0;
        *isB2bSystem = GT_FALSE;
        return GT_OK;
    }

    /* NO GENERIC algorithem implemented - explicitly assume the following order: */
    /* - First Lion2    */
    /* - Second Lion2   */
    /* - Bobcat2        */

    /* Therefore due to scan appDemoPpConfigList[index] contain the following */
    /* data per index:                                                        */
    /* 1-8 - First Lion2    */
    /* 9-16 - Second Lion2  */
    /* 0    - Bobcat 2      */

    /* After the following manipulations appDemoPpConfigList[index] will      */
    /* contain the following data per index:                                  */
    /* 0 - First Lion2      */
    /* 1 - Second Lion2     */
    /* 2 - Bobcat 2         */

    /* Bobcat2 existance is relevant only for boardRevId == 2 */

    if ( boardRevId == 2 )
    {
        /* Copy Bobcat2 appDemoPpConfigList data from index 0 to index 17 */
        cpssOsMemCpy(&appDemoPpConfigList[2*LION2_NUM_OF_PORT_GROUPS+1],
                     &appDemoPpConfigList[LION2_0_DEV_IND],
                     sizeof(APP_DEMO_PP_CONFIG));

        /* Move Lion2 devices appDemoPpConfigList data from indices 1-16 to 0-15 */
        for ( i = LION2_0_DEV_IND ; i < 2*LION2_NUM_OF_PORT_GROUPS ; i++ )
        {
            cpssOsMemCpy(&appDemoPpConfigList[i],
                         &appDemoPpConfigList[i+1],
                         sizeof(APP_DEMO_PP_CONFIG));
        }
        appDemoPpConfigList[2*LION2_NUM_OF_PORT_GROUPS].valid = GT_FALSE;
    }

    if (appDemoSysConfig.cpuEtherPortUsed)  /* LION2_MII */
    {
        appDemoPpConfigList[LION2_0_DEV_IND].cpuPortMode =
        appDemoPpConfigList[LION2_1_DEV_IND].cpuPortMode =
                                                CPSS_NET_CPU_PORT_MODE_MII_E;
    }

    /* The Lion2 devices have 8 port groups */
    appDemoPpConfigList[LION2_0_DEV_IND].numOfPortGroups =
    appDemoPpConfigList[LION2_1_DEV_IND].numOfPortGroups =
                                                LION2_NUM_OF_PORT_GROUPS;

    /* set the devices to hold all the port groups */
    for(i = 0 ; i < LION2_NUM_OF_PORT_GROUPS ; i++)
    {
        appDemoPpConfigList[i].valid =
        appDemoPpConfigList[i + LION2_NUM_OF_PORT_GROUPS].valid = GT_FALSE;

        appDemoPpConfigList[LION2_0_DEV_IND].portGroupsInfo[i].portGroupActive =
        appDemoPpConfigList[LION2_1_DEV_IND].portGroupsInfo[i].portGroupActive =
                                                                        GT_TRUE;

        if (CPSS_CHANNEL_PCI_E == appDemoPpConfigList[LION2_0_DEV_IND].channel)
        {
            appDemoPpConfigList[LION2_0_DEV_IND].portGroupsInfo[i].portGroupPciInfo =
                appDemoPpConfigList[i].pciInfo;
            appDemoPpConfigList[LION2_1_DEV_IND].portGroupsInfo[i].portGroupPciInfo =
                appDemoPpConfigList[i + LION2_NUM_OF_PORT_GROUPS].pciInfo;
        }
        else
        {
            appDemoPpConfigList[LION2_0_DEV_IND].portGroupsInfo[i].portGroupSmiInfo =
                appDemoPpConfigList[i].smiInfo;
            appDemoPpConfigList[LION2_1_DEV_IND].portGroupsInfo[i].portGroupSmiInfo =
                appDemoPpConfigList[i + LION2_NUM_OF_PORT_GROUPS].smiInfo;
        }

    }

    /* validate the devices */
    appDemoPpConfigList[LION2_0_DEV_IND].valid = GT_TRUE;

    appDemoPpConfigList[LION2_0_DEV_IND].pciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[LION2_0_DEV_IND].pciInfo.internalPciBase =  CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[LION2_0_DEV_IND].smiInfo.smiIdSel = CPSS_PARAM_NOT_USED_CNS;

    appDemoPpConfigList[LION2_1_DEV_IND].valid = GT_TRUE;

    appDemoPpConfigList[LION2_1_DEV_IND].pciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[LION2_1_DEV_IND].pciInfo.internalPciBase =  CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[LION2_1_DEV_IND].smiInfo.smiIdSel = CPSS_PARAM_NOT_USED_CNS;

    if ( boardRevId == 2 )
    {
        /* Move Bobcat2 appDemoPpConfigList to index 2 */
        cpssOsMemCpy(&appDemoPpConfigList[BOBCAT2_DEV_IND],
                     &appDemoPpConfigList[2*LION2_NUM_OF_PORT_GROUPS+1],
                     sizeof(APP_DEMO_PP_CONFIG));
        appDemoPpConfigList[2*LION2_NUM_OF_PORT_GROUPS+1].valid = GT_FALSE;

        ppCounter = 3;
    }
    else /* boardRevId == 1 -> only two Lion2 */
    {
        ppCounter = 2;
    }

    ppLion2Counter = 2;

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = GT_FALSE;/* no B2B with Lion2 and Bobcat2 devices */

    /* All Lion2 configurations are dual device enabled */
    /* at this stage we are after cpssPpInit(...) and before cpssDxChHwPpPhase1Init(...) */
    rc = cpssSystemDualDeviceIdModeEnableSet(GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
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
    OUT CPSS_PP_PHASE1_INIT_PARAMS  *phase1Params
)
{
    GT_STATUS rc = GT_OK;
    CPSS_PP_PHASE1_INIT_PARAMS    localPpPh1Config = CPSS_PP_PHASE1_DEFAULT;
    void    *intVecNum;
    GT_U32  ii;

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(boardRevId);

    if (devIdx == LION2_0_DEV_IND)
    {
        /* First Lion2 or Hooper */
        localPpPh1Config.devNum = LION2_0_SW_DEV_NUM;
    }
    else if (devIdx == LION2_1_DEV_IND)
    {
        if (BOBCAT2_DEV_IND == LION2_1_DEV_IND)
        {
            /* Bobcat2 */
            localPpPh1Config.devNum = BOBCAT2_SW_DEV_NUM;
        }
        else
        {
            /* Second Lion2 or Hooper */
            localPpPh1Config.devNum = LION2_1_SW_DEV_NUM;
        }
    }
    else if (devIdx == BOBCAT2_DEV_IND)
    {
        /* Bobcat2 */
        localPpPh1Config.devNum = BOBCAT2_SW_DEV_NUM;
    }
    else
    {
        /* No additional devices are expected */
        cpssOsPrintf("getPpPhase1Config: No additional devices are expected.\n");
    }

    if (CPSS_CHANNEL_PCI_E == appDemoPpConfigList[devIdx].channel)
    {
        if (BOBCAT2_DEV_IND == devIdx)
        {
            /* map registers for old driver */
            gtPresteraPciDoMap(&(appDemoPpConfigList[devIdx].pciInfo), _4M);
        }
        localPpPh1Config.internalPciBase =
                            appDemoPpConfigList[devIdx].pciInfo.internalPciBase;

        if (BOBCAT2_DEV_IND == devIdx)
        {
            /* Both PP base address & Reset and Init Controller (DFX) are accessed */
            /* through BAR1. The Reset and Init Controller window is the one after */
            /* the PP window, while the PP window is 64MB wide.                    */
            localPpPh1Config.resetAndInitControllerBase =
                            appDemoPpConfigList[devIdx].pciInfo.resetAndInitControllerBase;
        }

        localPpPh1Config.deviceId =
            ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
             (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

        localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PEX_E;
        /* TODO: localPpPh1Config.mngInterfaceType = */
        if (BOBCAT2_DEV_IND == devIdx)
        {
            localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PEX_MBUS_E;
            localPpPh1Config.baseAddr =
                appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
        }
        else /* Lion2 */
        {
            localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PEX_KERNEL_E;
            localPpPh1Config.appAddrCompletionRegionsBmp = 0x0f; /* 4 region addr completion */
        }
    }
    else /* CPSS_CHANNEL_SMI_E */
    {
        localPpPh1Config.baseAddr = appDemoPpConfigList[devIdx].smiInfo.smiIdSel;
        localPpPh1Config.internalPciBase  = 0x0;
        localPpPh1Config.deviceId =
           ((appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.devId << 16) |
            (appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.vendorId));
        localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_SMI_E;
    }

    localPpPh1Config.numOfPortGroups = appDemoPpConfigList[devIdx].numOfPortGroups;

    if (BOBCAT2_DEV_IND == devIdx)
    {
        localPpPh1Config.portGroupsInfo[0].busBaseAddr =
            appDemoPpConfigList[devIdx].portGroupsInfo[0].portGroupPciInfo.pciBaseAddr;
        localPpPh1Config.portGroupsInfo[0].internalPciBase =
            appDemoPpConfigList[devIdx].portGroupsInfo[0].portGroupPciInfo.internalPciBase;
#ifndef ASIC_SIMULATION
        if (boardRevId == 4)
        {
            GT_U32 intVec;
            extDrvGetPciIntVec(GT_PCI_INT_A, &intVecNum);
            intVec = ((GT_U32)((GT_UINTPTR)intVecNum)) + 1;
            localPpPh1Config.intVecNum = intVec;
            localPpPh1Config.intMask   = intVec;
        }
        else
        {
            extDrvGetPciIntVec(GT_PCI_INT_A, &intVecNum);
            localPpPh1Config.intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
            extDrvGetIntMask(GT_PCI_INT_A, &localPpPh1Config.intMask);
        }
#endif
    }
    else /* LION2_0_DEV_IND or LION2_1_DEV_IND */
    {
        for(ii = 0 ; ii < appDemoPpConfigList[devIdx].numOfPortGroups ;ii++)
        {
            if (CPSS_CHANNEL_PCI_E == appDemoPpConfigList[devIdx].channel)
            {
                localPpPh1Config.portGroupsInfo[ii].busBaseAddr =
                    appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.pciBaseAddr;
                localPpPh1Config.portGroupsInfo[ii].internalPciBase =
                    appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.internalPciBase;
                /* required for kernel driver */
                localPpPh1Config.portGroupsInfo[ii].hwAddr.busNo =
                    appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.pciBusNum;
                localPpPh1Config.portGroupsInfo[ii].hwAddr.devSel =
                    appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.pciIdSel;
                localPpPh1Config.portGroupsInfo[ii].hwAddr.funcNo =
                    appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.funcNo;
            }
            else
            {
                localPpPh1Config.portGroupsInfo[ii].busBaseAddr =
                    appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupSmiInfo.smiIdSel;
                localPpPh1Config.portGroupsInfo[ii].internalPciBase = 0x0;
            }

            if(CPSS_PARAM_NOT_USED_CNS == localPpPh1Config.portGroupsInfo[ii].busBaseAddr)
            {
                continue;
            }

            extDrvGetPciIntVec(GT_PCI_INT_D - (ii%4) , &intVecNum);
            localPpPh1Config.portGroupsInfo[ii].intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
            extDrvGetIntMask(GT_PCI_INT_D - (ii%4) , &localPpPh1Config.portGroupsInfo[ii].intMask);
        }
    }

    /* Interrupt line overriden by predefinitions */
    {
        char entryName[40];
        GT_U32 intVec, intMask;

        cpssOsSprintf(entryName, "intVec_devIdx%d", devIdx);
        if (GT_OK == appDemoDbEntryGet(entryName, &intVec))
        {
            localPpPh1Config.intVecNum = intVec;
            intMask = intVec;
            cpssOsSprintf(entryName, "intMask_devIdx%d", devIdx);
            appDemoDbEntryGet(entryName, &intMask);
            localPpPh1Config.intMask = intMask;
        }
        for(ii = 0 ; ii < appDemoPpConfigList[devIdx].numOfPortGroups ;ii++)
        {
            cpssOsSprintf(entryName, "intVec_devIdx%d_portGroup%d", devIdx, ii);
            if(GT_OK == appDemoDbEntryGet(entryName, &intVec))
            {
                localPpPh1Config.portGroupsInfo[ii].intVecNum = intVec;
                intMask = intVec;
                cpssOsSprintf(entryName, "intMask_devIdx%d_portGroup%d", devIdx, ii);
                appDemoDbEntryGet(entryName, &intMask);
                localPpPh1Config.portGroupsInfo[ii].intMask = intMask;
            }
        }
    }

    {GT_U32 tmpData, ii;
    if ((appDemoDbEntryGet("intVecPrint", &tmpData) == GT_OK) && (tmpData != 0))
    {
        osTimerWkAfter(800); /* time to finish previous print */
        cpssOsPrintf("\n\n\n");
        cpssOsPrintf(
            "Interrupt Vector devIdx %d, devNum %d, vector 0x%08X, mask 0x%08X\n",
            devIdx, localPpPh1Config.devNum,
            localPpPh1Config.intVecNum, localPpPh1Config.intMask);
        for(ii = 0 ; ii < localPpPh1Config.numOfPortGroups ;ii++)
        {
            cpssOsPrintf(
                "Port Grop %d, vector 0x%08X, mask 0x%08X\n",
                ii, localPpPh1Config.portGroupsInfo[ii].intVecNum,
                localPpPh1Config.portGroupsInfo[ii].intMask);
        }
        osTimerWkAfter(300); /* time to finish previous print */
    }}


    /* retrieve PP Core Clock from HW */
    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

    /* current Lion box has 156.25MHz differencial serdesRefClock */
    localPpPh1Config.serdesRefClock = APPDEMO_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E;

    localPpPh1Config.uplinkCfg = CPSS_PP_NO_UPLINK_E;

    appDemoPpConfigList[devIdx].flowControlDisable = GT_TRUE;

    osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));

    /* check previouse settings of debug device ID */
    rc = appDemoDebugDeviceIdGet(devIdx, &ii);
    if (rc == GT_NO_SUCH)
    {
        /* Due to binding of previous generic device Id to the GPON family, */
        /* imitate previous generic Id when new one is observed.            */
        if( CPSS_98CX8248_CNS == localPpPh1Config.deviceId)
        {
            rc = appDemoDebugDeviceIdSet(devIdx, CPSS_98CX8248_1_CNS);
        }
        else if( CPSS_98CX8224_CNS == localPpPh1Config.deviceId)
        {
            rc = appDemoDebugDeviceIdSet(devIdx, CPSS_98CX8224_1_CNS);
        }
        else
            rc = GT_OK;
    }

    return rc;
}


/*******************************************************************************
* lion2WaInit
*
* DESCRIPTION:
*       init the Lion2 WA needed after phase1.
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
static GT_STATUS lion2WaInit
(
    IN  GT_U8   boardRevId
)
{
    GT_STATUS   rc;
    CPSS_DXCH_IMPLEMENT_WA_ENT   waFromCpss[CPSS_DXCH_IMPLEMENT_WA_LAST_E];
    GT_U32  waIndex=0;
    GT_U32  ii;
    GT_U8   devNum;
    GT_U32  enableAddrFilterWa; /* do we want to enable Restricted Address
                                   Filtering or not */
    GT_U32  enableAuFifoWa; /* enable FIFO to send AU messages to CPU */
    GT_U32  enableIpmWa;    /* enable IPM bridge copy get dropped WA */
    GT_U32  enableMcBandWa; /* enable Relay port dynamic management */
    GT_U32  disableMismatchLinksWa; /* disable WA for false link up on mismatch
                                        ports */
    GT_U32  enableSerdesAccessWa; /* sequential access to internal serdes registers WA */

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(boardRevId);

    if(GT_OK != appDemoDbEntryGet("RESTRICTED_ADDRESS_FILTERING_WA_E", &enableAddrFilterWa))
    {
        enableAddrFilterWa = 0;
    }

    if(GT_OK != appDemoDbEntryGet("enableAuFifoWa", &enableAuFifoWa))
    {
        enableAuFifoWa = 0;
    }

    if(GT_OK != appDemoDbEntryGet("IPM_BRIDGE_COPY_GET_DROPPED_WA", &enableIpmWa))
    {
        enableIpmWa = 1;
    }

    if(GT_OK != appDemoDbEntryGet("GT_CORE_SYS_WA_MC_BANDWIDTH_RESTRICTION_WA_E", &enableMcBandWa))
    {
        /* This errata relevant only to devices with more then one hemisphere */
        devNum = appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devNum;
        if (PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(devNum) > 1)
        {
            enableMcBandWa = 1;
        }
    }

    if(GT_OK != appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_SERDES_INTERNAL_REG_ACCESS_WA_E", &enableSerdesAccessWa))
    {
        enableSerdesAccessWa = 0;
    }

    for(ii = SYSTEM_DEV_NUM_MAC(0); ii < SYSTEM_DEV_NUM_MAC(ppLion2Counter); ++ii)
    {
        waIndex = 0;
        devNum = appDemoPpConfigList[ii].devNum;

        /* state that application want CPSS to implement the WA */
        if(GT_FALSE == PRV_CPSS_DXCH_LION2_B0_AND_ABOVE_CHECK_MAC(devNum))
        {/* if WA enable bit 16 in 0x600006C set on by default */
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FU_FROM_NON_SOURCE_PORT_GROUP_E;
        }

        if (enableAddrFilterWa == 1)
        {
            /* state that application want CPSS to implement the Restricted
               Address Filtering WA */
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E;
        }

        if(enableAuFifoWa == 1)
        {
            /* state that application wants PP to send AU messages over FIFO */
            waFromCpss[waIndex++] = CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FIFO_E;
        }

        if(enableIpmWa == 1)
        {
            /* state that application want CPSS to implement
               IPM bridge copy get dropped WA */
            waFromCpss[waIndex++] = CPSS_DXCH_IMPLEMENT_WA_IPM_BRIDGE_COPY_GET_DROPPED_E;
        }

        if (enableMcBandWa == 1)
        {
            /* enable relay port managment in VLAN and VIDX table */
            waFromCpss[waIndex++] = CPSS_DXCH_IMPLEMENT_WA_MC_BANDWIDTH_RESTRICTION_E;
        }

        if(enableSerdesAccessWa)
        {
            /* state that application want CPSS to implement the serdes internal
                registers sequencial access WA */
            waFromCpss[waIndex++] =
                            CPSS_DXCH_IMPLEMENT_WA_SERDES_INTERNAL_REG_ACCESS_E;
        }

        if(waIndex)
        {
            rc = cpssDxChHwPpImplementWaInit(devNum,waIndex,&waFromCpss[0], NULL);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpImplementWaInit", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        if(GT_OK == appDemoDbEntryGet("disableMismatchLinksWa", &disableMismatchLinksWa))
        {
            if(disableMismatchLinksWa)
            {
                PRV_CPSS_DXCH_ERRATA_CLEAR_MAC(devNum,
                                         PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E);
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* bc2WaInit
*
* DESCRIPTION:
*       init the Bobcat2 WA needed after phase1.
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
static GT_STATUS bc2WaInit
(
    IN  GT_U8   boardRevId
)
{
    GT_STATUS   rc;
    static CPSS_DXCH_IMPLEMENT_WA_ENT   waFromCpss[CPSS_DXCH_IMPLEMENT_WA_LAST_E];
    GT_U32  waIndex=0;
    GT_U8   devNum;
    GT_U32  dbEntryValue;
    GT_BOOL enable30GWa = GT_FALSE;

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(boardRevId);

    if(GT_OK == appDemoDbEntryGet("BC2_RevA0_40G_2_30GWA", &dbEntryValue))
    {
        enable30GWa = (GT_BOOL)dbEntryValue;
    }

    waIndex = 0;
    devNum = appDemoPpConfigList[BOBCAT2_DEV_IND].devNum;

    /* state that application want CPSS to implement the WA */
    if(GT_FALSE != enable30GWa)
    {/* if WA enable bit 16 in 0x600006C set on by default */
        waFromCpss[waIndex++] = CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_REV_A0_40G_NOT_THROUGH_TM_IS_PA_30G_E;
    }


    if(waIndex)
    {
        rc = cpssDxChHwPpImplementWaInit(devNum,waIndex,&waFromCpss[0], NULL);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpImplementWaInit", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

#ifdef  INCLUDE_UTF
typedef enum
{
    PRV_TGF_DEVICE_PORT_MODE_DEFAULT_E,  /* Default mode, UT defines ports by itself */
    PRV_TGF_DEVICE_PORT_MODE_XLG_E      /* Lion2 40G mode - local ports 0, 4, 8, 9 */

} PRV_TGF_DEVICE_PORT_MODE_ENT;

GT_STATUS prvTgfDefPortsArrayModeSet
(
    IN PRV_TGF_DEVICE_PORT_MODE_ENT     portMode
);

#endif

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
    GT_STATUS rc = GT_FAIL; /* default*/
    GT_U32                   mapArrLen;
    CPSS_DXCH_PORT_MAP_STC  *mapArrPtr, *mapPtr;
    GT_U32  noCpu;
    GT_U32    i;
    GT_U8   dev;

    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        if(appDemoPpConfigList[dev].valid == GT_TRUE)
        {
            CPSS_RESOURCE_MAPPING_STC resourceMapping;

            resourceMapping.sramBaseAddress = appDemoPpConfigList[dev].pciInfo.resourceMapping.sram.base;
            resourceMapping.sramSize = (GT_U32)appDemoPpConfigList[dev].pciInfo.resourceMapping.sram.size;
            rc = cpssHwInitResourceMappingSet(
                        appDemoPpConfigList[dev].devNum, &resourceMapping);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssHwInitResourceMappingSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
#ifdef  INCLUDE_UTF
    switch(boardRevId)
    {
        case 2:
        case 4:
            /* XLG MACs are used */
            rc = prvTgfDefPortsArrayModeSet(PRV_TGF_DEVICE_PORT_MODE_XLG_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvTgfDefPortsArrayModeSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
            break;
        default:
            /* default is for 10G port configuration */
            break;
    }
#endif

    if (boardRevId != 3)
    {
        rc = lion2WaInit(boardRevId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("lion2WaInit", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if( boardRevId == 2 )
    {
        /* get port map for non-TM system */
        rc = configBoardAfterPhase1MappingGet(BOBCAT2_DEF_STANDALONE_BOARD_REV,
                                              /*OUT*/&mapArrPtr,&mapArrLen);
        if(rc != GT_OK)
        {
            return rc;
        }

        if (mapArrPtr == NULL)
        {
            cpssOsPrintf("\n-->ERROR : configBoardAfterPhase1() mapping : rev =%d is not supported", boardRevId);
            return GT_NOT_SUPPORTED;
        }

        rc = appDemoDbEntryGet("noCpu", &noCpu);
        if(rc != GT_OK)
        {
            noCpu = 0;
        }

        if(1 == noCpu)
        {
            for (i = 0, mapPtr = mapArrPtr; i < mapArrLen; i++, mapPtr++)
            {
                if(63 == mapPtr->physicalPortNumber)
                {
                    mapPtr->physicalPortNumber = 83;
                    mapPtr->mappingType = CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E;
                    mapPtr->interfaceNum = 63;
                    break;
                }
            }
        }

        rc = cpssDxChPortPhysicalPortMapSet(BOBCAT2_SW_DEV_NUM, mapArrLen, mapArrPtr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }

        rc = appDemoDxChMaxMappedPortSet(BOBCAT2_SW_DEV_NUM, mapArrLen, mapArrPtr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }

        rc = bc2WaInit(boardRevId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("bc2WaInit", rc);
    }

    /* for Hooper, Hooper, Bobcat2 */
    if ( (boardRevId >= 3) && (boardRevId <= 4) )
    {
        rc = lion2WaInit(HOOPER_DEF_STANDALONE_BOARD_REV);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("lion2WaInit", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* as Bobcat2 - cpssInitSystem 29,1,0*/
        rc = configBoardAfterPhase1MappingGet(BOBCAT2_DEF_STANDALONE_BOARD_REV,/*OUT*/&mapArrPtr,&mapArrLen);
        if(rc != GT_OK)
        {
            return rc;
        }

        if (mapArrPtr == NULL)
        {
            cpssOsPrintf("\n-->ERROR : configBoardAfterPhase1() mapping : rev =0 is not supported");
            return GT_NOT_SUPPORTED;
        }

        rc = appDemoDbEntryGet("noCpu", &noCpu);
        if(rc != GT_OK)
        {
            noCpu = 0;
        }

        if(1 == noCpu)
        {
            for (i = 0, mapPtr = mapArrPtr; i < mapArrLen; i++, mapPtr++)
            {
                if(63 == mapPtr->physicalPortNumber)
                {
                    mapPtr->physicalPortNumber = 83;
                    mapPtr->mappingType = CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E;
                    mapPtr->interfaceNum = 63;
                    break;
                }
            }
        }

        rc = cpssDxChPortPhysicalPortMapSet(BOBCAT2_SW_DEV_NUM, mapArrLen, mapArrPtr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }

        rc = appDemoDxChMaxMappedPortSet(BOBCAT2_SW_DEV_NUM, mapArrLen, mapArrPtr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }

        rc = bc2WaInit(BOBCAT2_DEF_STANDALONE_BOARD_REV);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("bc2WaInit", rc);
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
    OUT CPSS_PP_PHASE2_INIT_PARAMS  *phase2Params
)
{
    CPSS_PP_PHASE2_INIT_PARAMS  localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS                   retVal = GT_OK;

    GT_U32                      devIdx;

    GT_U32                      auDescNum;
    GT_U32                      fuDescNum;
    GT_U32                      tmpData;

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(boardRevId);

    localPpPh2Config.devNum     = oldDevNum;

    retVal = appDemoDevIdxGet(oldDevNum, &devIdx);
    if ( GT_OK != retVal )
    {
        return retVal;
    }

    localPpPh2Config.deviceId   = appDemoPpConfigList[devIdx].deviceId;

    localPpPh2Config.fuqUseSeparate = GT_TRUE;

    /*if ( BOBCAT2_SW_DEV_NUM == oldDevNum )*/
    if (appDemoPpConfigList[devIdx].numOfPortGroups <= 1)
    {
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
            retVal = appDemoDbEntryAdd("fuDescNum", fuDescNum);
            if(retVal != GT_OK)
            {
                return retVal;
            }
        }

        /* Check if to use the default configuration for appDemo of */
        /* Tx SDMA Packet Generator queues.  */
        if( appDemoDbEntryGet("skipTxSdmaGenDefaultCfg", &tmpData) == GT_NO_SUCH )
        {
            /* allow to work in Tx queue generator mode */
            retVal = appDemoDbEntryAdd("useMultiNetIfSdma", GT_TRUE);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            /* Enable Tx queue 3 to work in Tx queue generator mode */
            retVal = appDemoDbEntryAdd("txGenQueue_3", GT_TRUE);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            /* Enable Tx queue 6 to work in Tx queue generator mode */
            retVal = appDemoDbEntryAdd("txGenQueue_6", GT_TRUE);
            if(retVal != GT_OK)
            {
                return retVal;
            }
        }
    }
    else /* LION2_0_SW_DEV_NUM or LION2_1_SW_DEV_NUM */
    {
        /*auDescNum = AU_DESC_NUM_DEF*LION2_NUM_OF_PORT_GROUPS;*/
        auDescNum = AU_DESC_NUM_DEF * appDemoPpConfigList[devIdx].numOfPortGroups;
    }

    if ((appDemoDbEntryGet("debugSwapHoopersDmaMems", &tmpData) == GT_OK) && (tmpData != 0))
    {
        static CPSS_PP_PHASE2_INIT_PARAMS firstHooperLocalPpPh2Config;
        if (localPpPh2Config.devNum == LION2_0_SW_DEV_NUM)
        {
            firstHooperLocalPpPh2Config = localPpPh2Config;
            /* save first allocation call results for second device */
            retVal = appDemoAllocateDmaMem(
                localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                RX_BUFF_SIZE_DEF, RX_BUFF_ALLIGN_DEF,
                TX_DESC_NUM_DEF,
                auDescNum, &firstHooperLocalPpPh2Config);
            /* second allocation call results to first device */
            retVal = appDemoAllocateDmaMem(
                localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                RX_BUFF_SIZE_DEF, RX_BUFF_ALLIGN_DEF,
                TX_DESC_NUM_DEF,
                auDescNum, &localPpPh2Config);
        }
        else if (localPpPh2Config.devNum == LION2_1_SW_DEV_NUM)
        {
            /* first allocation call results to second device */
            localPpPh2Config.fuqUseSeparate = firstHooperLocalPpPh2Config.fuqUseSeparate;
            localPpPh2Config.useDoubleAuq = firstHooperLocalPpPh2Config.useDoubleAuq;
            localPpPh2Config.useSecondaryAuq = firstHooperLocalPpPh2Config.useSecondaryAuq;
            localPpPh2Config.useMultiNetIfSdma = firstHooperLocalPpPh2Config.useMultiNetIfSdma;
            localPpPh2Config.auqCfg.auDescBlock = firstHooperLocalPpPh2Config.auqCfg.auDescBlock;
            localPpPh2Config.auqCfg.auDescBlockSize = firstHooperLocalPpPh2Config.auqCfg.auDescBlockSize;
            localPpPh2Config.fuqCfg.auDescBlock = firstHooperLocalPpPh2Config.fuqCfg.auDescBlock;
            localPpPh2Config.fuqCfg.auDescBlockSize = firstHooperLocalPpPh2Config.fuqCfg.auDescBlockSize;
            localPpPh2Config.netIfCfg.txDescBlockSize = firstHooperLocalPpPh2Config.netIfCfg.txDescBlockSize;
            localPpPh2Config.netIfCfg.txDescBlock = firstHooperLocalPpPh2Config.netIfCfg.txDescBlock;
            localPpPh2Config.netIfCfg.rxDescBlockSize = firstHooperLocalPpPh2Config.netIfCfg.rxDescBlockSize;
            localPpPh2Config.netIfCfg.rxDescBlock = firstHooperLocalPpPh2Config.netIfCfg.rxDescBlock;
            localPpPh2Config.netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize =
                firstHooperLocalPpPh2Config.netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize;
            localPpPh2Config.netIfCfg.rxBufInfo.buffersInCachedMem =
                firstHooperLocalPpPh2Config.netIfCfg.rxBufInfo.buffersInCachedMem;
            localPpPh2Config.netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr =
                firstHooperLocalPpPh2Config.netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr;
        }
        else
        {
            retVal = appDemoAllocateDmaMem(
                localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                RX_BUFF_SIZE_DEF, RX_BUFF_ALLIGN_DEF,
                TX_DESC_NUM_DEF,
                auDescNum, &localPpPh2Config);
        }
    }
    else
    {
        retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                                       RX_BUFF_SIZE_DEF, RX_BUFF_ALLIGN_DEF,
                                       TX_DESC_NUM_DEF,
                                       auDescNum, &localPpPh2Config);
    }

    {GT_U32 tmpData = 0;
    if ((appDemoDbEntryGet("debugPrint", &tmpData) == GT_OK) && (tmpData != 0))
    {
        cpssOsPrintf("appDemoAllocateDmaMem results for oldDevNum = %d\n", oldDevNum);
        cpssOsPrintf("auqCfg.auDescBlock 0x%08X\n", localPpPh2Config.auqCfg.auDescBlock);
        cpssOsPrintf("auqCfg.auDescBlockSize 0x%08X\n", localPpPh2Config.auqCfg.auDescBlockSize);
        cpssOsPrintf("fuqCfg.auDescBlock 0x%08X\n", localPpPh2Config.fuqCfg.auDescBlock);
        cpssOsPrintf("fuqCfg.auDescBlockSize 0x%08X\n", localPpPh2Config.fuqCfg.auDescBlockSize);
    }}

    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

    phase2Params->auMessageLength= CPSS_AU_MESSAGE_LENGTH_8_WORDS_E;

    if ( LION2_0_SW_DEV_NUM == oldDevNum || LION2_1_SW_DEV_NUM == oldDevNum )
    {
        if(CPSS_NET_CPU_PORT_MODE_MII_E == appDemoPpConfigList[oldDevNum].cpuPortMode)
        {/* in existing DB boards RGMII interface welded to core #1,
            also could be updated by appDemoUpdatePpPhase2Params using appDemoDbEntry
            mechanism */
            phase2Params->netifSdmaPortGroupId = 1;
        }
    }

    return retVal;
}

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
    IN  GT_PHYSICAL_PORT_NUM  port_num
)
{

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(devNum);
    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(port_num);

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
    GT_U8                   dev;  /* device number */
    GT_PHYSICAL_PORT_NUM    port; /* port number */
    GT_U8                   index;
    GT_STATUS               rc;     /* return code */
    GT_U32                  mcFifo; /* multicast FIFO allocated for port */
    GT_U8                   phyAddr;/* PHY address */
    CPSS_PHY_SMI_INTERFACE_ENT smiIf; /* SMI Master interface for PHY management */
    GT_U8                   devNum;
    GT_PORT_NUM             portNum;
    CPSS_INTERFACE_INFO_STC physicalInfo;
    GT_U32                  numOfPhysicalPorts;
    GT_U32                  trunkNum;  /* number of trunks in device */

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(boardRevId);

    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        devNum = appDemoPpConfigList[dev].devNum;
        index = 0;

        appDemoPpConfigList[dev].internal10GPortConfigFuncPtr = internalXGPortConfig;
        appDemoPpConfigList[dev].internalCscdPortConfigFuncPtr = cascadePortConfig;

        for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS;port++)
        {
            if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(devNum)->existingPorts, port)))
            {
                continue;
            }

            if ( BOBCAT2_SW_DEV_NUM == devNum )
            {
                /* split ports between MC FIFOs for Multicast arbiter */
                rc = cpssDxChPortTxMcFifoSet(devNum, port, port%2);
                if( GT_OK != rc )
                {
                    return rc;
                }
            }
            else
            {
                appDemoPpConfigList[dev].ports10GToConfigureArr[index] = port;
                index++;

                /* split ports between MC FIFOs for Multicast arbiter, can't use modulo 2,
                    because 40G except port 9 can be defined on even ports only and so
                    the share won't be event between */
                mcFifo = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,
                                                                port) < 6) ? 0 : 1;
                rc = cpssDxChPortTxMcFifoSet((GT_U8)devNum, port, mcFifo);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxMcFifoSet", rc);
                if( GT_OK != rc )
                {
                    return rc;
                }
            }
        }

        if ( BOBCAT2_SW_DEV_NUM == devNum )
        {
            /* configure QSGMII ports PHY related mappings */

           /*
            Serdes# MAC Port#   M_SMI#  Phy Address[H]
            ++++ DB Board +++++++++++++++++++
                0   0 - 3           2   4 - 7
                1   4 - 7           2   8 - B
                2   8 - 11          2   C - F
                3   12 - 15         3   4 - 7
                4   16 - 19         3   8 - B
                5   20 - 23         3   C - F
                6   24 - 27         0   4 - 7
                7   28 - 31         0   8 - B
                8   32 - 35         0   C - F
                9   36 - 39         1   4 - 7
                10  40 - 43         1   8 - B
                11  44 - 47         1   C - F
            ++++ RD Board +++++++++++++++++++
                0   0 - 3           0   0 - 3
                1   4 - 7           0   4 - 7
                2   8 - 11          0   8 - B
                3   12 - 15         0   C - F
                4   16 - 19         1   0x10 - 0x13
                5   20 - 23         1   0x14 - 0x17
                6   24 - 27         2   0 - 3
                7   28 - 31         2   4 - 7
                8   32 - 35         2   8 - B
                9   36 - 39         2   C - F
                10  40 - 43         3   0x10 - 0x13
                11  44 - 47         3   0x14 - 0x17
            */
            for(port = 0; port < 48; port++)
            {
                CPSS_ENABLER_PORT_SKIP_CHECK(devNum,port);

                 /*DB Board */
                 phyAddr = (GT_U8)(4 + (port % 12));

                 if (port < 12)
                 {
                     smiIf = CPSS_PHY_SMI_INTERFACE_2_E;
                 }
                 else if (port < 24)
                 {
                     smiIf = CPSS_PHY_SMI_INTERFACE_3_E;
                 }
                 else if (port < 36)
                 {
                     smiIf = CPSS_PHY_SMI_INTERFACE_0_E;
                 }
                 else
                 {
                     smiIf = CPSS_PHY_SMI_INTERFACE_1_E;
                 }


                /* configure PHY address */
                rc = cpssDxChPhyPortAddrSet(devNum, port, phyAddr);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortAddrSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* configure SMI interface */
                rc = cpssDxChPhyPortSmiInterfaceSet(devNum, port, smiIf);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiInterfaceSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* enable in-band auto-negotiation because out of band cannot
                   work on this DB board */
                rc = cpssDxChPortInbandAutoNegEnableSet(devNum, port, GT_TRUE);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortInbandAutoNegEnableSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        else
        {
            appDemoPpConfigList[dev].numberOf10GPortsToConfigure = index;

            /* Set Threshold for Interrupt */
            rc = cpssDxChDiagDeviceTemperatureThresholdSet(devNum, 115);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChDiagDeviceTemperatureThresholdSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Select Temperature Sensors */
            rc = cpssDxChDiagDeviceTemperatureSensorsSelectSet(devNum,
                                       CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_AVERAGE_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChDiagDeviceTemperatureSensorsSelectSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            if (CPSS_NET_CPU_PORT_MODE_MII_E == appDemoPpConfigList[dev].cpuPortMode)
            {
                /* must init SMI here - we need it for PHY configuration*/
                rc = cpssDxChPhyPortSmiInit(devNum);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiInit", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* configure the lion2 CPU port PHY - we need it here before
                   cpssDxChNetIfMiiInit, because there BSP checks link up */
                cpssOsPrintf("configBoardAfterPhase2: configuring Lion2 MII CPU port PHY 88E1512.\n");
                rc = lion2AppDemoCpuPortPhyConfig(devNum);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    rc = cpssDxChCfgHwDevNumSet(LION2_0_SW_DEV_NUM, LION2_0_HW_DEV_NUM);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    if (boardRevId != 4)
    {
        rc = cpssDxChCfgHwDevNumSet(LION2_1_SW_DEV_NUM, LION2_1_HW_DEV_NUM);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    if ((boardRevId >= 2) && (boardRevId <= 4))
    {
        rc = cpssDxChCfgHwDevNumSet(BOBCAT2_SW_DEV_NUM, BOBCAT2_HW_DEV_NUM);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* Loop on the first 256 (num of physical ports , and CPU port (63)) entries
           of the table */
        numOfPhysicalPorts =
            PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(BOBCAT2_SW_DEV_NUM);

        for(portNum=0; portNum < numOfPhysicalPorts; portNum++)
        {
            rc = cpssDxChBrgEportToPhysicalPortTargetMappingTableGet(BOBCAT2_SW_DEV_NUM,
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
                physicalInfo.devPort.hwDevNum = BOBCAT2_HW_DEV_NUM;
                rc = cpssDxChBrgEportToPhysicalPortTargetMappingTableSet(BOBCAT2_SW_DEV_NUM,
                                                                         portNum,
                                                                         &physicalInfo);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgEportToPhysicalPortTargetMappingTableSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    /* default - no cascade ports and trunks - may be overridden */
    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        appDemoPpConfigList[dev].numberOfCscdTrunks = 0;
        appDemoPpConfigList[dev].numberOfCscdPorts  = 0;
    }

    if (boardRevId == 2)
    {
        /* Configure the following cascade trunks: */
        /* First Lion2, ports 72,80  <--> Bobcat2, ports 24,28 */
        /* Second Lion2, ports 72,80 <--> Bobcat2, ports 16,20 */
        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppLion2Counter); dev++)
        {
            /* get number of trunks in device. */
            rc = cpssDxChCfgTableNumEntriesGet(appDemoPpConfigList[dev].devNum,
                                               CPSS_DXCH_CFG_TABLE_TRUNK_E,
                                               &trunkNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgTableNumEntriesGet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            appDemoPpConfigList[dev].numberOfCscdTrunks = 1;
            appDemoPpConfigList[dev].numberOfCscdPorts = 2;

            appDemoPpConfigList[dev].cscdPortsArr[0].portNum = 72;
            appDemoPpConfigList[dev].cscdPortsArr[0].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[dev].cscdPortsArr[0].trunkId = (GT_TRUNK_ID)trunkNum;

            appDemoPpConfigList[dev].cscdPortsArr[1].portNum = 80;
            appDemoPpConfigList[dev].cscdPortsArr[1].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[dev].cscdPortsArr[1].trunkId = (GT_TRUNK_ID)trunkNum;

            appDemoPpConfigList[dev].numberOfCscdTargetDevs = 2;

            appDemoPpConfigList[dev].cscdTargetDevsArr[0].targetDevNum = BOBCAT2_SW_DEV_NUM;
            appDemoPpConfigList[dev].cscdTargetDevsArr[0].linkToTargetDev.linkNum =
            appDemoPpConfigList[dev].cscdTargetDevsArr[1].linkToTargetDev.linkNum = trunkNum;
            appDemoPpConfigList[dev].cscdTargetDevsArr[0].linkToTargetDev.linkType =
            appDemoPpConfigList[dev].cscdTargetDevsArr[1].linkToTargetDev.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;

            if ( LION2_0_DEV_IND == dev )
            {
                appDemoPpConfigList[dev].cscdTargetDevsArr[1].targetDevNum = LION2_1_SW_DEV_NUM;
            }
            else /* LION2_1_DEV_IND */
            {
                appDemoPpConfigList[dev].cscdTargetDevsArr[1].targetDevNum = LION2_0_SW_DEV_NUM;
            }
        }
        dev = BOBCAT2_DEV_IND;

        /* Cascade trunk number in Bobcat2 is limited to 255 */
        trunkNum = 255;

        appDemoPpConfigList[dev].numberOfCscdTrunks = 2;
        appDemoPpConfigList[dev].numberOfCscdPorts = 4;

        appDemoPpConfigList[dev].cscdPortsArr[0].portNum = 24;
        appDemoPpConfigList[dev].cscdPortsArr[0].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
        appDemoPpConfigList[dev].cscdPortsArr[0].trunkId = (GT_TRUNK_ID)trunkNum;

        appDemoPpConfigList[dev].cscdPortsArr[1].portNum = 28;
        appDemoPpConfigList[dev].cscdPortsArr[1].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
        appDemoPpConfigList[dev].cscdPortsArr[1].trunkId = (GT_TRUNK_ID)trunkNum;

        appDemoPpConfigList[dev].cscdPortsArr[2].portNum = 16;
        appDemoPpConfigList[dev].cscdPortsArr[2].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
        appDemoPpConfigList[dev].cscdPortsArr[2].trunkId = (GT_TRUNK_ID)(trunkNum - 1);

        appDemoPpConfigList[dev].cscdPortsArr[3].portNum = 20;
        appDemoPpConfigList[dev].cscdPortsArr[3].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
        appDemoPpConfigList[dev].cscdPortsArr[3].trunkId = (GT_TRUNK_ID)(trunkNum - 1);

        appDemoPpConfigList[dev].numberOfCscdTargetDevs = 2;

        appDemoPpConfigList[dev].cscdTargetDevsArr[0].targetDevNum = LION2_0_SW_DEV_NUM;
        appDemoPpConfigList[dev].cscdTargetDevsArr[0].linkToTargetDev.linkNum = trunkNum;
        appDemoPpConfigList[dev].cscdTargetDevsArr[0].linkToTargetDev.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;

        appDemoPpConfigList[dev].cscdTargetDevsArr[1].targetDevNum = LION2_1_SW_DEV_NUM;
        appDemoPpConfigList[dev].cscdTargetDevsArr[1].linkToTargetDev.linkNum = trunkNum - 1;
        appDemoPpConfigList[dev].cscdTargetDevsArr[1].linkToTargetDev.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
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
    GT_U32                   rc = GT_OK;
    CPSS_PP_CONFIG_INIT_STC  localPpCfgParams = PP_LOGICAL_CONFIG_DEFAULT;

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(boardRevId);

    localPpCfgParams.maxNumOfIpv4Prefixes = 3920;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
    {
        localPpCfgParams.numOfTrunks = _8K-1;
        localPpCfgParams.maxNumOfPbrEntries = _8K;

        rc = appDemoBc2IpLpmRamDefaultConfigCalc(devNum,localPpCfgParams.maxNumOfPbrEntries,&(localPpCfgParams.lpmRamMemoryBlocksCfg));
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        localPpCfgParams.numOfTrunks = _2K-1;
    }

    localPpCfgParams.maxNumOfVirtualRouters = 1;
    localPpCfgParams.lpmDbFirstTcamLine = 100;
    localPpCfgParams.lpmDbLastTcamLine = 1204;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
    {
        if (CPSS_NET_CPU_PORT_MODE_MII_E == appDemoPpConfigList[devNum].cpuPortMode)
        {
          localPpCfgParams.miiNumOfTxDesc = TX_DESC_NUM_DEF;
          localPpCfgParams.miiTxInternalBufBlockSize = (TX_DESC_NUM_DEF / 2) *
            CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS;
          localPpCfgParams.miiRxBufSize = RX_BUFF_SIZE_DEF;
          localPpCfgParams.miiRxBufBlockSize = RX_DESC_NUM_DEF * RX_BUFF_SIZE_DEF;
        }
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
    GT_U32                    ipDevIdsBitmap = 0;
    GT_BOOL                   ipDevIdsBitmapUsed = GT_FALSE;

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(boardRevId);

    localLibInitParams.initClassifier            = GT_FALSE;

    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
    localLibInitParams.initMpls                  = GT_FALSE;
    localLibInitParams.initMplsTunnel            = GT_FALSE;
    localLibInitParams.initPcl                   = GT_TRUE;

    /* there is no Policer lib init for CH3 devices */
    localLibInitParams.initPolicer               = GT_FALSE;

    ipDevIdsBitmapUsed = GT_FALSE;
    if(appDemoDbEntryGet("ipDevIdsBitmap", &ipDevIdsBitmap) == GT_OK)
    {
        ipDevIdsBitmapUsed = GT_TRUE;
    }

    if (ipDevIdsBitmapUsed != GT_FALSE)
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
        {
            localLibInitParams.initTcam                  = GT_TRUE;
        }
        if (ipDevIdsBitmap & (1 << devNum))
        {
            localLibInitParams.initIpv6                  = GT_TRUE;
            localLibInitParams.initIpv4                  = GT_TRUE;
        }
        else
        {
            localLibInitParams.initIpv6                  = GT_FALSE;
            localLibInitParams.initIpv4                  = GT_FALSE;
        }
    }
    else
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
        {
            localLibInitParams.initIpv6                  = GT_TRUE;
            localLibInitParams.initTcam                  = GT_TRUE;
        }
        else /* CPSS_PP_FAMILY_DXCH_LION2_E */
        {
            if( boardRevId == 1 )
            {
                localLibInitParams.initIpv6              = GT_TRUE;
            }
            else /* boardRevId == 2 or 3 or 4 */
            {
                /* initIpv6 == GT_FALE by default */
                localLibInitParams.initIpv4              = GT_FALSE;
            }
        }
    }

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
    GT_STATUS   rc, rcGetNext;
    GT_U8       dev;
    GT_U8       devNum;

    GT_U32      skipPhyInit;
    GT_U32      initSerdesDefaults;
    GT_U32      portNum;
    GT_U8       bc2BoardRevId;
    GT_U8       lion2BoardRevId;

    bc2BoardRevId   = boardRevId;
    lion2BoardRevId = boardRevId;
    if ((boardRevId == 3) || (boardRevId == 4))
    {
        bc2BoardRevId   = BOBCAT2_DEF_STANDALONE_BOARD_REV;
        lion2BoardRevId = HOOPER_DEF_STANDALONE_BOARD_REV;
    }

    /* allow processing of AA messages */
    appDemoSysConfig.supportAaMessage = GT_TRUE;

    if(appDemoSysConfig.forceAutoLearn == GT_FALSE)
    {
        /* the Lion2 & Bobcat2 need 'Controlled aging' because the port groups can't share refresh info */
        /* the AA to CPU enabled from appDemoDxChFdbInit(...) --> call cpssDxChBrgFdbAAandTAToCpuSet(...)*/
        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
        {
            devNum = appDemoPpConfigList[dev].devNum;
            rc = cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }
    /* note that the appDemo sets the aging mode to 'Triggered mode' by default
       so no auto aging unless you set it explicitly */

    for(dev = 0xff; (rcGetNext = cpssPpCfgNextDevGet(dev, &dev)) == GT_OK;)
    {
        if (systemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E)
        {
            if (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
            {
                CPSS_ENABLER_FILTERED_TRACE_MAC(
                    ("BC2 appDemoBc2PortInterfaceInit dev = %d, rev = %d\n", dev, bc2BoardRevId));
                rc = appDemoBc2PortInterfaceInit(dev, bc2BoardRevId);
            }
            else /* CPSS_PP_FAMILY_DXCH_LION2_E */
            {
                CPSS_ENABLER_FILTERED_TRACE_MAC(
                    ("LION2 lion2AppDemoPortInterfaceInit dev = %d, rev = %d\n", dev, bc2BoardRevId));
                rc = lion2AppDemoPortInterfaceInit(dev, lion2BoardRevId);
            }

            if(rc != GT_OK)
            {
                return rc;
            }
        }

        if (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
        {
            /* add port 83 to default VLAN, although it could be not mapped */
            rc = cpssDxChBrgVlanMemberAdd(dev, 1, 83, GT_FALSE,
                                          CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanMemberAdd(83 to default VLAN)", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            rc = appDemoDbEntryGet("initSerdesDefaults", /*out */&initSerdesDefaults);
            if(rc != GT_OK)
            {
                initSerdesDefaults = 1;
            }

            if (initSerdesDefaults != 0) /* if initSerdesDefaults == 0 .i.e. skip port config, */
            {                            /*     than skip also phy config */
                rc = appDemoDbEntryGet("skipPhyInit", &skipPhyInit);
                if(rc != GT_OK)
                {
                    skipPhyInit = 0;
                }
            }
            else
            {
                skipPhyInit = 1;
            }

            if(skipPhyInit != 1)
            {
                /* configure PHYs */
                CPSS_ENABLER_FILTERED_TRACE_MAC(
                    ("BC2 bobcat2BoardPhyConfig dev = %d, rev = %d\n", dev, bc2BoardRevId));
                rc = bobcat2BoardPhyConfig(bc2BoardRevId, dev);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("boardPhyConfig", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            /* PTP (and TAIs) configurations */
            rc = appDemoB2PtpConfig(dev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvBobcat2PtpConfig", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            if(PRV_CPSS_SIP_5_10_CHECK_MAC(dev))
            {
                /* TBD: FE HA-3259 fix and removed from CPSS.
                   Allow to the CPU to get the original vlan tag as payload after
                   the DSA tag , so the info is not changed. */
                rc = cpssDxChBrgVlanForceNewDsaToCpuEnableSet(dev, GT_TRUE);
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
                rc = cpssDxChBrgSecurBreachEventPacketCommandSet(dev,
                    CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E,
                    CPSS_PACKET_CMD_FORWARD_E);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSecurBreachEventPacketCommandSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            /* Flow Control Initializations */
            for(portNum = 0; portNum < (appDemoPpConfigList[dev].maxPortNumber); portNum++)
            {
                CPSS_ENABLER_PORT_SKIP_CHECK(dev, portNum);

                rc = cpssDxChPortMacSaLsbSet(dev, portNum, (GT_U8)portNum);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortMacSaLsbSet", rc);
            }

            if(PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.trafficManager.supported)
            {
                /* Configure PFC response mode to TXQ in order:
                    - to be backward compatible
                    - to fix default registers value
                */
                rc = cpssDxChTmGluePfcResponseModeSet(dev, 0, CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_TXQ_E);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTmGluePfcResponseModeSet", rc);
            }

        }
        else /* CPSS_PP_FAMILY_DXCH_LION2_E */
        {
#ifndef ASIC_SIMULATION_NO_HW
            /* For B0 and above */
            if(PRV_CPSS_PP_MAC(dev)->revision > 0)
            {
                GT_U32    apEnable;

                /* check if user wants to use AP */
                rc = appDemoDbEntryGet("apEnable", &apEnable);
                if(rc != GT_OK)
                {/* by default for now disabled */
                    apEnable = 0;
                }

                if(apEnable)
                {
                    rc = lion2AppDemoApInit(dev);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
#endif
        }
    }

    if((rcGetNext != GT_OK) && (rcGetNext != GT_NO_MORE))
    {
        return rcGetNext;
    }

#ifndef ASIC_SIMULATION_NO_HW
    if(GT_FALSE == PRV_CPSS_DXCH_LION2_B0_AND_ABOVE_CHECK_MAC(SYSTEM_DEV_NUM_MAC(0)))
    {
        GT_TASK   linkFixTid;         /* Task Id */

        return osTaskCreate("linkFix",        /* Task Name      */
                            250,              /* Task Priority  */
                            _8KB,             /* Stack Size     */
                            appDemoLinkFix,   /* Starting Point */
                            NULL,             /* Arguments list */
                            &linkFixTid);     /* task ID        */
    }
#endif

    return GT_OK;
}

/*******************************************************************************
* gtDbDxLion2_X2_Bobcat2_BoardReg
*
* DESCRIPTION:
*       Registration function for the two Lion2 and one Bobcat2 ports board .
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
GT_STATUS gtDbDxLion2_X2_Bobcat2_BoardReg
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(boardRevId);

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

    /*
    Done boardCfgFuncs->boardGetInfo                 = getBoardInfo;
    Done boardCfgFuncs->boardGetPpPh1Params          = getPpPhase1Config;
    Done boardCfgFuncs->boardAfterPhase1Config       = configBoardAfterPhase1;
    Done boardCfgFuncs->boardGetPpPh2Params          = getPpPhase2Config;
    Done boardCfgFuncs->boardAfterPhase2Config       = configBoardAfterPhase2;
    Done boardCfgFuncs->boardGetPpLogInitParams      = getPpLogicalInitParams;
    Done boardCfgFuncs->boardGetLibInitParams        = getTapiLibInitParams;
    Done boardCfgFuncs->boardAfterInitConfig         = afterInitBoardConfig;
    boardCfgFuncs->boardCleanDbDuringSystemReset= gtDbDxBobcat2BoardCleanDbDuringSystemReset;
    */

    return GT_OK;
}










